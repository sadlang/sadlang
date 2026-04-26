/**
 * @file vm.h
 * @brief (AR) الآلة الافتراضية لتنفيذ البايت كود — النواة التنفيذية لمكتبة runtime_new
 * @brief (EN) Bytecode Virtual Machine — execution core of the runtime_new library
 *
 * @details
 * ═══════════════════════════════════════════════════════════════════════════
 * (AR) شرح موسّع — الآلة الافتراضية (Virtual Machine)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * ما هي الآلة الافتراضية؟
 * ─────────────────────
 * الآلة الافتراضية هي المُنفِّذ الفعلي لبرامج لغة ص المُترجَمة إلى بايت كود.
 * تعمل كمعالج (processor) برمجي يقرأ التعليمات واحدة تلو الأخرى وينفذها.
 *
 * البنية الداخلية:
 * ───────────────
 *   ┌───────────────────────────────────────────────────────────┐
 *   │                    VirtualMachine                         │
 *   │                                                           │
 *   │  ┌─────────────┐  ┌──────────────┐  ┌────────────────┐   │
 *   │  │   المكدس    │  │ إطارات       │  │  الثوابت       │   │
 *   │  │   (Stack)   │  │ الاستدعاء    │  │  (Constants)   │   │
 *   │  │  Value[]    │  │ (CallFrames) │  │  Value[]       │   │
 *   │  └─────────────┘  └──────────────┘  └────────────────┘   │
 *   │                                                           │
 *   │  ┌─────────────┐  ┌──────────────┐  ┌────────────────┐   │
 *   │  │ المتغيرات   │  │ الدوال       │  │  جامع         │   │
 *   │  │ العامة      │  │ المحلية      │  │  القمامة       │   │
 *   │  │ (Globals)   │  │ (Natives)    │  │  (GC)          │   │
 *   │  └─────────────┘  └──────────────┘  └────────────────┘   │
 *   └───────────────────────────────────────────────────────────┘
 *
 * دورة التنفيذ (Fetch-Decode-Execute):
 * ────────────────────────────────────
 * 1. جلب (Fetch): قراءة البايت التالي من مصفوفة الكود
 * 2. فك الترميز (Decode): تحديد نوع العملية من كود العملية (opcode)
 * 3. التنفيذ (Execute): تنفيذ العملية على المكدس والذاكرة
 * 4. التكرار حتى الوصول إلى OP_HALT أو خطأ
 *
 * التعليمات المدعومة (106 تعليمة):
 * ────────────────────────────────
 * - عمليات المكدس: PUSH, POP, DUP, SWAP, ROT
 * - الحساب: ADD, SUB, MUL, DIV, MOD, NEG, POW
 * - المقارنة: EQ, NE, LT, LE, GT, GE
 * - المنطق: AND, OR, NOT (بتي ومنطقي)
 * - التحكم: JUMP, JUMP_IF_TRUE, JUMP_IF_FALSE, CALL, RETURN
 * - المتغيرات: LOAD/STORE_LOCAL, LOAD/STORE_GLOBAL
 * - الكائنات: OBJ_NEW, OBJ_GET, OBJ_SET
 * - النصوص: STRING_CONCAT, STRING_LEN, STRING_GET, STRING_SUBSTR
 * - الإدخال/الإخراج: PRINT, IO_OPEN, IO_CLOSE
 * - التحويل: CAST_INT, CAST_FLOAT, CAST_STRING, CAST_BOOL, TYPEOF
 * - الذاكرة: ALLOC, FREE, MMAP, MUNMAP
 *
 * إدارة إطارات الاستدعاء (Call Frames):
 * ─────────────────────────────────────
 * كل استدعاء دالة يُنشئ إطار استدعاء جديد يحتوي:
 * - مؤشر الكود (IP): الموضع الحالي في البايت كود
 * - قاعدة المكدس (stack_base): بداية المتغيرات المحلية
 * - عدد المعاملات ومعلومات الإرجاع
 *
 * جامع القمامة (Garbage Collector):
 * ─────────────────────────────────
 * يستخدم خوارزمية mark-and-sweep ثلاثية الألوان:
 * - أبيض (White): كائن لم يُزَر بعد
 * - رمادي (Gray): كائن يحتاج لمسح مراجعه
 * - أسود (Black): كائن تم مسحه بالكامل
 *
 * ═══════════════════════════════════════════════════════════════════════════
 * (EN) Extended Description — Virtual Machine
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * Stack-based bytecode VM supporting 106 opcodes with call frame management,
 * tri-color mark-and-sweep GC, native function binding, and debug support.
 * Uses a fetch-decode-execute loop over a flat bytecode array.
 *
 * Key components: value stack, call frame stack, global variables table,
 * native function registry, constant pool, and garbage collector.
 *
 * @note الاعتماديات / Dependencies: value.h (نوع القيم)، bytecode_compat.h
 *       (أكواد العمليات والأنواع المتوافقة)
 * @note هذا الملف يعتمد على طبقة التوافق بدلاً من استيراد ملفات المترجم مباشرة
 *
 * @see runtime_new/include/vm/value.h — نظام القيم
 * @see runtime_new/include/bytecode_compat.h — طبقة التوافق
 * @see compiler_new/include/bytecode/opcodes.h — تعريف أكواد العمليات الأصلية
 *
 * @author فريق لغة ص / Sad Language Team
 * @date ديسمبر 2025 — فبراير 2026 / December 2025 — February 2026
 * @version 2.0 — تحديث لاستخدام طبقة التوافق / Updated to use compat layer
 * @phase Phase 3: Bytecode Backend
 */

#pragma once

// ============================================================================
// (AR) ملف الآلة الافتراضية — التعريف الرئيسي
// --------------------------------------------------------------------------
// كان هذا الملف يعتمد على includes مباشرة من:
//   - "bytecode/opcodes.h"
//   - "bytecode/bytecode.h"
// لكن هذه الملفات تقع في compiler_new/include/ بمسارات مختلفة وفضاءات
// أسماء مختلفة (Sad::Compiler::Bytecode vs Sad::Bytecode).
// 
// الحل: نستخدم طبقة التوافق bytecode_compat.h التي توفر:
//   - namespace aliases (Sad::Bytecode → Sad::Compiler::Bytecode)
//   - الأنواع المفقودة (FileHeader, BytecodeLoader, SymbolInfo)
//   - الثوابت المفقودة (MAGIC_NUMBER, FORMAT_VERSION_*, FLAG_*)
// 
// (EN) This file previously included directly from:
//   - "bytecode/opcodes.h" 
//   - "bytecode/bytecode.h"
// But these files live in compiler_new/include/ with different paths and
// namespaces (Sad::Compiler::Bytecode vs Sad::Bytecode).
// 
// Solution: Use bytecode_compat.h compatibility layer which provides:
//   - namespace aliases (Sad::Bytecode → Sad::Compiler::Bytecode)
//   - missing types (FileHeader, BytecodeLoader, SymbolInfo)
//   - missing constants (MAGIC_NUMBER, FORMAT_VERSION_*, FLAG_*)
// ============================================================================

#include "value.h"
#include "bytecode_compat.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <functional>
#include <fstream>
#include <chrono>

namespace Sad {
namespace VM {

// Forward declarations
class VirtualMachine;
struct CallFrame;

// ========================================
// Native Function Type
// نوع الدالة المحلية
// ========================================

/**
 * @brief نوع دالة محلية (C++) / Native function type
 * @brief Functions implemented in C++ that can be called from bytecode
 * 
 * @param vm مؤشر للآلة الافتراضية / Pointer to VM
 * @param argc عدد الوسائط / Argument count
 * @param args مصفوفة الوسائط / Arguments array
 * @return قيمة الإرجاع / Return value
 */
using NativeFunction = std::function<Value(VirtualMachine*, int, Value*)>;

// ========================================
// Call Frame
// إطار الاستدعاء
// ========================================

/**
 * @brief إطار استدعاء دالة / Function call frame
 * @brief Represents a single function invocation
 * 
 * @details
 * (AR) يحتوي على معلومات استدعاء الدالة: عنوان العودة، المتغيرات المحلية، والمكدس.
 * (EN) Contains function call information: return address, local variables, stack.
 */
struct CallFrame {
    uint32_t returnAddress;        ///< عنوان العودة / Return address
    uint32_t basePointer;          ///< مؤشر القاعدة (في المكدس) / Base pointer (in stack)
    uint32_t localCount;           ///< عدد المتغيرات المحلية / Local variable count
    const Bytecode::FunctionInfo* function; ///< معلومات الدالة / Function info
    
    CallFrame()
        : returnAddress(0), basePointer(0), localCount(0), function(nullptr) {}
    
    CallFrame(uint32_t ret, uint32_t bp, uint32_t locals, const Bytecode::FunctionInfo* func)
        : returnAddress(ret), basePointer(bp), localCount(locals), function(func) {}
};

// ========================================
// VM Configuration
// إعدادات الآلة الافتراضية
// ========================================

/**
 * @brief إعدادات الآلة الافتراضية / VM configuration
 */
struct VMConfig {
    size_t maxStackSize;           ///< الحد الأقصى لحجم المكدس / Max stack size
    size_t maxCallDepth;           ///< الحد الأقصى لعمق الاستدعاء / Max call depth
    size_t initialHeapSize;        ///< الحجم الابتدائي للذاكرة / Initial heap size
    bool enableGC;                 ///< تفعيل جامع القمامة / Enable GC
    bool enableDebug;              ///< تفعيل وضع التصحيح / Enable debug mode
    bool enableProfiling;          ///< تفعيل القياس / Enable profiling
    
    VMConfig()
        : maxStackSize(65536)      // 64K values
        , maxCallDepth(1024)       // 1024 nested calls
        , initialHeapSize(1024*1024) // 1MB
        , enableGC(true)
        , enableDebug(false)
        , enableProfiling(false) {}
};

// ========================================
// VM Statistics
// إحصائيات الآلة الافتراضية
// ========================================

/**
 * @brief إحصائيات تنفيذ الآلة الافتراضية / VM execution statistics
 */
struct VMStats {
    uint64_t instructionsExecuted; ///< عدد التعليمات المنفذة / Instructions executed
    uint64_t functionCalls;        ///< عدد استدعاءات الدوال / Function calls
    uint64_t nativeCalls;          ///< عدد استدعاءات الدوال المحلية / Native calls
    uint64_t gcCollections;        ///< عدد مرات جمع القمامة / GC collections
    uint64_t bytesAllocated;       ///< البايتات المخصصة / Bytes allocated
    uint64_t bytesFreed;           ///< البايتات المحررة / Bytes freed
    double executionTime;          ///< وقت التنفيذ (ثانية) / Execution time (seconds)
    
    VMStats()
        : instructionsExecuted(0), functionCalls(0), nativeCalls(0)
        , gcCollections(0), bytesAllocated(0), bytesFreed(0)
        , executionTime(0.0) {}
    
    void reset() {
        instructionsExecuted = 0;
        functionCalls = 0;
        nativeCalls = 0;
        gcCollections = 0;
        bytesAllocated = 0;
        bytesFreed = 0;
        executionTime = 0.0;
    }
};

// ========================================
// Virtual Machine
// الآلة الافتراضية
// ========================================

/**
 * @brief الآلة الافتراضية المعتمدة على المكدس / Stack-based virtual machine
 * @brief Main VM class for bytecode execution
 * 
 * @details
 * (AR) تنفذ البايت كود باستخدام مكدس القيم وإطارات الاستدعاء.
 *      تدعم 106 تعليمة، الدوال المحلية، وإدارة الذاكرة.
 * 
 * (EN) Executes bytecode using value stack and call frames.
 *      Supports 106 opcodes, native functions, and memory management.
 * 
 * @example
 * ```cpp
 * VirtualMachine vm;
 * vm.loadModule(bytecodeModule);
 * Value result = vm.execute();
 * ```
 */
class VirtualMachine {
public:
    /**
     * @brief المُنشئ / Constructor
     */
    VirtualMachine();
    
    /**
     * @brief المُنشئ مع إعدادات / Constructor with config
     */
    explicit VirtualMachine(const VMConfig& config);
    
    /**
     * @brief المُدمر / Destructor
     */
    ~VirtualMachine();
    
    // ========================================
    // Module Loading
    // تحميل الوحدات
    // ========================================
    
    /**
     * @brief تحميل وحدة بايت كود / Load bytecode module
     * @param module الوحدة / Module
     */
    void loadModule(const Bytecode::BytecodeModule* module);
    
    /**
     * @brief تحميل من ملف / Load from file
     * @param filename اسم الملف / Filename
     */
    bool loadFromFile(const std::string& filename);
    
    // ========================================
    // Execution
    // التنفيذ
    // ========================================
    
    /**
     * @brief تنفيذ البايت كود / Execute bytecode
     * @return القيمة المعادة / Return value
     * 
     * @details
     * (AR) ينفذ البايت كود من نقطة الدخول (main) حتى النهاية.
     * (EN) Executes bytecode from entry point (main) to completion.
     */
    Value execute();
    
    /**
     * @brief تنفيذ دالة محددة / Execute specific function
     * @param functionName اسم الدالة / Function name
     * @param args الوسائط / Arguments
     * @return القيمة المعادة / Return value
     */
    Value executeFunction(const std::string& functionName, const std::vector<Value>& args);
    
    /**
     * @brief إعادة تعيين الآلة الافتراضية / Reset VM state
     */
    void reset();
    
    // ========================================
    // Stack Operations
    // عمليات المكدس
    // ========================================
    
    /**
     * @brief دفع قيمة على المكدس / Push value onto stack
     */
    void push(const Value& value);
    
    /**
     * @brief سحب قيمة من المكدس / Pop value from stack
     */
    Value pop();
    
    /**
     * @brief النظر إلى قمة المكدس / Peek at stack top
     */
    Value peek(int distance = 0) const;
    
    /**
     * @brief الحصول على حجم المكدس / Get stack size
     */
    size_t stackSize() const { return stack_.size(); }
    
    // ========================================
    // Native Functions
    // الدوال المحلية
    // ========================================
    
    /**
     * @brief تسجيل دالة محلية / Register native function
     * @param name الاسم / Name
     * @param function الدالة / Function
     */
    void registerNative(const std::string& name, NativeFunction function);
    
    /**
     * @brief تسجيل دالة محلية بمعرف محدد / Register native function with specific ID
     * @param id المعرف / ID
     * @param name الاسم / Name
     * @param function الدالة / Function
     */
    void registerNativeById(uint32_t id, const std::string& name, NativeFunction function);
    
    /**
     * @brief الحصول على معرف دالة محلية / Get native function ID
     * @param name الاسم / Name
     * @return المعرف أو -1 / ID or -1 if not found
     */
    int32_t getNativeId(const std::string& name) const;
    
    /**
     * @brief التحقق من وجود خطأ / Check if error occurred
     * @return true إذا حدث خطأ / true if error occurred
     */
    bool hasError() const { return hasError_; }
    
    /**
     * @brief الحصول على رسالة الخطأ / Get error message
     * @return رسالة الخطأ / Error message
     */
    const std::string& getError() const { return lastError_; }
    
    /**
     * @brief مسح حالة الخطأ / Clear error state
     */
    void clearError() { hasError_ = false; lastError_.clear(); }
    
    /**
     * @brief استدعاء دالة محلية / Call native function
     * @param name الاسم / Name
     * @param argc عدد الوسائط / Argument count
     * @return القيمة المعادة / Return value
     */
    Value callNative(const std::string& name, int argc);
    
    // ========================================
    // Memory Management
    // إدارة الذاكرة
    // ========================================
    
    /**
     * @brief تخصيص كائن / Allocate object
     * @param type النوع / Type
     * @param size الحجم / Size
     * @return الكائن / Object
     */
    Object* allocateObject(ObjectType type, size_t size);
    
    /**
     * @brief تحرير كائن / Free object
     */
    void freeObject(Object* object);
    
    /**
     * @brief تعليم قيمة للـ GC / Mark value for GC
     * (AR) يعلّم القيمة وجميع الكائنات المشار إليها كمستخدمة.
     * (EN) Marks the value and all referenced objects as reachable.
     */
    void markValue(const Value& value);
    
    /**
     * @brief تشغيل جامع القمامة / Run garbage collector
     */
    void collectGarbage();
    
    // ========================================
    // Globals
    // المتغيرات العامة
    // ========================================
    
    /**
     * @brief تعيين متغير عام / Set global variable
     */
    void setGlobal(uint32_t index, const Value& value);
    
    /**
     * @brief الحصول على متغير عام / Get global variable
     */
    Value getGlobal(uint32_t index) const;
    
    // ========================================
    // Debug & Statistics
    // التصحيح والإحصائيات
    // ========================================
    
    /**
     * @brief طباعة حالة المكدس / Print stack state
     */
    void dumpStack() const;
    
    /**
     * @brief طباعة إطارات الاستدعاء / Print call frames
     */
    void dumpCallStack() const;
    
    /**
     * @brief الحصول على الإحصائيات / Get statistics
     */
    const VMStats& getStats() const { return stats_; }
    
    /**
     * @brief إعادة تعيين الإحصائيات / Reset statistics
     */
    void resetStats() { stats_.reset(); }
    
    /**
     * @brief تفعيل/تعطيل وضع التصحيح / Enable/disable debug mode
     */
    void setDebugMode(bool enable) { config_.enableDebug = enable; }

    // ========================================
    // Stack Access for GC
    // وصول المكدس لجامع القمامة
    // ========================================

    /**
     * @brief الحصول على مرجع للمكدس / Get stack reference (for GC root scanning)
     */
    const std::vector<Value>& getStack() const { return stack_; }

    /**
     * @brief الحصول على حجم المكدس / Get stack size
     */
    size_t getStackSize() const { return stack_.size(); }

    /**
     * @brief الحصول على مرجع للمتغيرات العامة / Get globals reference (for GC root scanning)
     */
    const std::vector<Value>& getGlobals() const { return globals_; }

private:
    // ========================================
    // Instruction Execution
    // تنفيذ التعليمات
    // ========================================
    
    /**
     * @brief حلقة التنفيذ الرئيسية / Main execution loop
     */
    Value run();
    
    /**
     * @brief قراءة بايت / Read byte
     */
    uint8_t readByte();
    
    /**
     * @brief قراءة uint16 / Read uint16
     */
    uint16_t readU16();
    
    /**
     * @brief قراءة uint32 / Read uint32
     */
    uint32_t readU32();
    
    /**
     * @brief قراءة uint64 / Read uint64
     */
    uint64_t readU64();
    
    /**
     * @brief قراءة double / Read double
     */
    double readF64();
    
    // ========================================
    // Opcode Handlers
    // معالجات التعليمات
    // ========================================
    
    // Stack operations
    void op_nop();
    void op_push_i8();
    void op_push_i16();
    void op_push_i32();
    void op_push_i64();
    void op_push_f64();
    void op_push_str();
    void op_push_null();
    void op_push_true();
    void op_push_false();
    void op_pop();
    void op_dup();
    void op_dup2();
    void op_swap();
    void op_rot();
    void op_over();
    
    // Arithmetic operations
    void op_add();
    void op_sub();
    void op_mul();
    void op_div();
    void op_mod();
    void op_neg();
    void op_abs();
    void op_pow();
    void op_sqrt();
    void op_inc();
    void op_dec();
    
    // Bitwise operations
    void op_bit_and();
    void op_bit_or();
    void op_bit_xor();
    void op_bit_not();
    void op_bit_shl();
    void op_bit_shr();
    
    // Comparison operations
    void op_cmp_eq();
    void op_cmp_ne();
    void op_cmp_lt();
    void op_cmp_le();
    void op_cmp_gt();
    void op_cmp_ge();
    
    // Logical operations
    void op_log_and();
    void op_log_or();
    void op_log_not();
    
    // Control flow
    void op_jmp();
    void op_jmp_if();
    void op_jmp_not();
    void op_call();
    void op_call_native();
    void op_ret();
    void op_ret_val();
    void op_loop();
    
    // Variable access
    void op_get_local();
    void op_set_local();
    void op_get_global();
    void op_set_global();
    
    // Memory operations
    void op_malloc();
    void op_free();
    void op_mmap();
    void op_munmap();
    
    // Array operations
    void op_array_new();
    void op_array_get();
    void op_array_set();
    void op_array_len();
    void op_array_push();
    void op_array_pop();
    
    // String operations
    void op_string_concat();
    void op_string_len();
    void op_string_get();
    void op_string_substr();
    
    // Object operations
    void op_object_new();
    void op_object_get();
    void op_object_set();
    
    // I/O operations
    void op_io_read();
    void op_io_write();
    void op_io_open();
    void op_io_close();
    
    // Type operations
    void op_typeof();
    void op_cast_int();
    void op_cast_float();
    void op_cast_string();
    void op_cast_bool();
    
    // ========================================
    // Helper Functions
    // دوال مساعدة
    // ========================================
    
    /**
     * @brief رفع خطأ وقت التنفيذ / Raise runtime error
     */
    void runtimeError(const std::string& message);
    
    /**
     * @brief التحقق من نوع القيمة / Check value type
     */
    bool checkValueType(const Value& value, ValueType expected);
    
    /**
     * @brief تحويل إلى عدد صحيح / Convert to integer
     */
    int64_t toInteger(const Value& value);
    
    /**
     * @brief تحويل إلى عدد عشري / Convert to float
     */
    double toFloat(const Value& value);
    
    /**
     * @brief تحويل إلى منطقي / Convert to boolean
     */
    bool toBool(const Value& value);
    
    /**
     * @brief إيجاد دالة / Find function
     */
    const Bytecode::FunctionInfo* findFunction(const std::string& name) const;

private:
    VMConfig config_;                        ///< الإعدادات / Configuration
    VMStats stats_;                          ///< الإحصائيات / Statistics
    
    // Execution state
    const Bytecode::BytecodeModule* module_; ///< الوحدة الحالية / Current module
    const uint8_t* code_;                    ///< البايت كود / Bytecode
    uint32_t ip_;                            ///< عداد البرنامج / Instruction pointer
    
    // Runtime state
    std::vector<Value> stack_;               ///< مكدس القيم / Value stack
    std::vector<CallFrame> frames_;          ///< إطارات الاستدعاء / Call frames
    std::vector<Value> globals_;             ///< المتغيرات العامة / Global variables
    
    // Native functions
    std::unordered_map<std::string, NativeFunction> natives_; ///< الدوال المحلية بالاسم / Native functions by name
    std::unordered_map<uint32_t, NativeFunction> nativesById_; ///< الدوال المحلية بالمعرف / Native functions by ID
    std::unordered_map<std::string, uint32_t> nativeNameToId_; ///< تحويل الاسم إلف ID / Name to ID mapping
    uint32_t nextNativeId_;                                    ///< المعرف التالي / Next ID
    
    // Error state - حالة الخطأ
    bool hasError_;                ///< هل حدث خطأ؟ / Has error occurred?
    std::string lastError_;        ///< آخر رسالة خطأ / Last error message
    
    // File I/O management - إدارة ملفات I/O
    struct FileHandle {
        std::fstream* stream;      ///< تدفق الملف / File stream
        std::string filename;      ///< اسم الملف / Filename
        bool isOpen;               ///< هل مفتوح؟ / Is open?
        
        FileHandle() : stream(nullptr), isOpen(false) {}
        FileHandle(std::fstream* s, const std::string& name) 
            : stream(s), filename(name), isOpen(true) {}
    };
    std::unordered_map<int32_t, FileHandle> fileHandles_; ///< معرفات الملفات / File handles
    int32_t nextFileHandle_;                              ///< المعرف التالي / Next handle ID
    
    // Memory management
    Object* objects_;                        ///< قائمة الكائنات / Object list
    size_t bytesAllocated_;                  ///< البايتات المخصصة / Bytes allocated
    size_t nextGC_;                          ///< العتبة التالية للـ GC / Next GC threshold
    
    // ملكية الوحدة المُحمَّلة من ملف / Loaded module ownership (from loadFromFile)
    std::shared_ptr<Bytecode::BytecodeModule> loadedModule_;
    
    // Profiling
    std::chrono::high_resolution_clock::time_point startTime_; ///< وقت البدء / Start time
};

} // namespace VM
} // namespace Sad
