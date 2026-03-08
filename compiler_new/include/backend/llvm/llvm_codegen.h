/*
 * ============================================================================
 * مولد كود LLVM IR - رأس الملف
 * LLVM IR Code Generator - Header File
 * ============================================================================
 * 
 * الوصف (Description):
 *   هذا الملف يحتوي على مولد كود LLVM IR الذي يترجم SIR (Sad Intermediate
 *   Representation) إلى LLVM IR. يوفر تحويل الأنواع، إصدار الدوال، ترجمة
 *   التعليمات، ومعالجة تدفق التحكم.
 * 
 *   This file contains the LLVM IR code generator that translates SIR (Sad
 *   Intermediate Representation) to LLVM IR. Provides type conversion,
 *   function emission, instruction translation, and control flow handling.
 * 
 * المكونات الرئيسية (Main Components):
 *   - LLVMCodeGen: الفئة الرئيسية لتوليد LLVM IR
 *   - LLVMCodeGen: Main class for LLVM IR generation
 * 
 *   - Type Mapping: تحويل أنواع Sad إلى أنواع LLVM
 *   - Type Mapping: Convert Sad types to LLVM types
 * 
 *   - Function Emission: إصدار دوال LLVM من SIR functions
 *   - Function Emission: Emit LLVM functions from SIR functions
 * 
 *   - Instruction Translation: ترجمة تعليمات SIR إلى LLVM IR
 *   - Instruction Translation: Translate SIR instructions to LLVM IR
 * 
 *   - Control Flow: معالجة القفزات والفروع والاستدعاءات
 *   - Control Flow: Handle jumps, branches, and calls
 * 
 * الاستخدام (Usage):
 *   ```cpp
 *   LLVMCodeGen codegen;
 *   codegen.initialize("my_module", "x86_64-linux-gnu");
 *   auto llvm_module = codegen.generate(sir_module);
 *   codegen.emitToFile("output.ll");
 *   ```
 * 
 * التبعيات (Dependencies):
 *   - LLVM Core Libraries
 *   - SIR (Sad IR) definitions
 *   - Type system
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2025
 * الإصدار (Version): 1.0.0
 * ============================================================================
 */

#ifndef SAD_LLVM_CODEGEN_H
#define SAD_LLVM_CODEGEN_H

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// LLVM Headers
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>

// Sad LLVM Components (مكونات Sad LLVM)
#include "llvm_type_mapper.h"
#include "llvm_optimizer.h"  // إضافة محسّن LLVM / Add LLVM optimizer

// Sad SIR Components (مكونات Sad SIR)
// Source: compiler/frontend/include/sir_*.h - مضاف في CMake include_directories line 27
#include "sir_module.h"     // Source: compiler/frontend/include/sir_module.h
#include "sir_instruction.h" // Source: compiler/frontend/include/sir_instruction.h
#include "sir_types.h"       // Source: compiler/frontend/include/sir_types.h

namespace Sad {
namespace LLVM {

// ============================================================================
// Forward Declarations / التصريحات المسبقة
// ============================================================================

// Use SIR types from Compiler namespace
// استخدام أنواع SIR من namespace Compiler
using SIRModule = Compiler::SIR::SIRModule;
using SIRFunction = Compiler::SIR::SIRFunction;
using SIRBasicBlock = Compiler::SIR::SIRBasicBlock;
using SIRInstruction = Compiler::SIR::SIRInstruction;
using SIRType = Compiler::SIR::SIRType;
using SIROperand = Compiler::SIR::SIROperand;
using SIROperandType = Compiler::SIR::SIROperandType;

// ============================================================================
// CodeGenContext - سياق توليد الكود
// CodeGenContext - Code Generation Context
// ============================================================================

/**
 * سياق توليد الكود يحتوي على معلومات الحالة أثناء الترجمة
 * Code generation context holds state information during translation
 */
struct CodeGenContext {
    // الدالة الحالية / Current function
    llvm::Function* currentFunction = nullptr;
    
    // الكتلة الأساسية الحالية / Current basic block
    llvm::BasicBlock* currentBlock = nullptr;
    
    // جدول القيم المسماة (المتغيرات المحلية)
    // Named values table (local variables)
    std::unordered_map<std::string, llvm::Value*> namedValues;
    
    // جدول الدوال / Function table
    std::unordered_map<std::string, llvm::Function*> functions;
    
    // جدول الكتل الأساسية / Basic block table
    std::unordered_map<std::string, llvm::BasicBlock*> basicBlocks;
    
    // جدول القيم الثابتة / Constant values table
    std::unordered_map<std::string, llvm::Constant*> constants;
    
    // خريطة تخصيص الذاكرة / Allocation map
    std::unordered_map<std::string, llvm::AllocaInst*> allocas;
    
    // ================================================================
    // دعم الأصناف / Class Support
    // ================================================================
    
    // خريطة السجل/المتغير → اسم الصنف
    // Register/variable → class name mapping
    std::unordered_map<std::string, std::string> objectClassMap;
    
    // اسم الصنف → نوع الهيكل LLVM
    // Class name → LLVM struct type
    std::unordered_map<std::string, llvm::StructType*> classStructTypes;
    
    // اسم الصنف → أسماء الحقول بالترتيب
    // Class name → ordered field names
    std::unordered_map<std::string, std::vector<std::string>> classFieldNames;
    
    // اسم الصنف → اسم الصنف الأب (للوراثة)
    // Class name → parent class name (for inheritance)
    std::unordered_map<std::string, std::string> classParentMap;
    
    // اسم الصنف الحالي في حالة البناء (فارغ خارج الباني)
    // Current constructor class name (empty outside constructors)
    std::string currentConstructorClass;
    
    // اسم الصنف الحالي في حالة الدالة (فارغ خارج دوال الصنف)
    // Current method class name (empty outside class methods)
    
    // ================================================================
    // دعم vtable والاستدعاء الافتراضي / vtable & Virtual Dispatch
    // ================================================================
    
    // اسم الصنف → متغير عام لـ vtable
    // Class name → vtable global variable
    std::unordered_map<std::string, llvm::GlobalVariable*> classVtableGlobals;
    
    // اسم الصنف → ترتيب الدوال في vtable (اسم الدالة الكامل)
    // Class name → ordered method names in vtable slots
    std::unordered_map<std::string, std::vector<std::string>> classVtableLayout;
    
    // اسم الصنف → اسم دالة الهدم
    // Class name → destructor function name
    std::unordered_map<std::string, std::string> classDestructors;
    
    // أسماء الأصناف المجردة (لا يمكن إنشاء كائنات منها)
    // Abstract class names (cannot be instantiated)
    std::unordered_set<std::string> abstractClasses;
    
    // ================================================================
    // دعم الكوروتين / Coroutine Support
    // ================================================================
    bool isCoroutineFunction = false;          ///< هل الدالة الحالية كوروتين
    bool isGeneratorFunction = false;          ///< هل الدالة الحالية مولّد
    llvm::Value* coroHandle = nullptr;         ///< مقبض الكوروتين الحالي
    llvm::Value* coroPromise = nullptr;        ///< مؤشر الوعد (promise) للقيمة المُرجعة
    llvm::Value* coroId = nullptr;             ///< token من coro.id
    llvm::BasicBlock* coroCleanupBB = nullptr; ///< كتلة تنظيف الكوروتين
    llvm::BasicBlock* coroSuspendBB = nullptr; ///< كتلة تعليق الكوروتين
    llvm::BasicBlock* coroFinalBB = nullptr;   ///< كتلة الإنهاء النهائي
    int coroSuspendCount = 0;                  ///< عداد نقاط التعليق
    std::string currentMethodClass;
};

// ============================================================================
// LLVMCodeGen - مولد كود LLVM الرئيسي
// LLVMCodeGen - Main LLVM Code Generator
// ============================================================================

/**
 * مولد كود LLVM يترجم SIR إلى LLVM IR
 * LLVM code generator translates SIR to LLVM IR
 * 
 * الوظائف الرئيسية (Main Functions):
 *   - initialize(): تهيئة السياق والوحدة
 *   - generate(): توليد LLVM IR من SIR
 *   - emitToFile(): حفظ LLVM IR إلى ملف
 * 
 * مثال (Example):
 *   ```cpp
 *   LLVMCodeGen gen;
 *   gen.initialize("program", "x86_64-linux-gnu");
 *   auto module = gen.generate(sir);
 *   gen.verify();
 *   gen.emitToFile("output.ll");
 *   ```
 */
class LLVMCodeGen {
public:
    // ========================================================================
    // Constructor & Destructor / المنشئ والمدمر
    // ========================================================================
    
    LLVMCodeGen();
    ~LLVMCodeGen();
    
    // منع النسخ / Prevent copying
    LLVMCodeGen(const LLVMCodeGen&) = delete;
    LLVMCodeGen& operator=(const LLVMCodeGen&) = delete;
    
    // ========================================================================
    // Initialization / التهيئة
    // ========================================================================
    
    /**
     * تهيئة مولد الكود
     * Initialize the code generator
     * 
     * @param moduleName اسم الوحدة / Module name
     * @param targetTriple سلسلة الهدف (مثل x86_64-linux-gnu) / Target triple
     * @return true إذا نجحت التهيئة / true if initialization succeeded
     */
    bool initialize(const std::string& moduleName, const std::string& targetTriple);
    
    /**
     * تهيئة السياق فقط بدون هدف محدد
     * Initialize context only without specific target
     * 
     * @param moduleName اسم الوحدة / Module name
     * @return true إذا نجحت التهيئة / true if initialization succeeded
     */
    bool initialize(const std::string& moduleName);
    
    // ========================================================================
    // Main Generation / التوليد الرئيسي
    // ========================================================================
    
    /**
     * توليد LLVM IR من وحدة SIR
     * Generate LLVM IR from SIR module
     * 
     * @param sirModule وحدة SIR المدخلة / Input SIR module
     * @return وحدة LLVM الناتجة / Resulting LLVM module
     */
    std::unique_ptr<llvm::Module> generate(std::shared_ptr<SIRModule> sirModule);
    
    /**
     * الحصول على الوحدة الحالية
     * Get current module
     * 
     * @return مؤشر للوحدة / Pointer to module
     */
    llvm::Module* getModule() const { return module_.get(); }
    
    /**
     * الحصول على مُنشئ التعليمات
     * Get IR builder
     * 
     * @return مؤشر للبانِي / Pointer to IR builder
     */
    llvm::IRBuilder<>* getBuilder() const { return builder_.get(); }
    
    /**
     * الحصول على سياق LLVM
     * Get LLVM context
     * 
     * @return مؤشر للسياق / Pointer to context
     */
    llvm::LLVMContext* getContext() const { return context_.get(); }
    
    /**
     * التحقق من صحة الوحدة
     * Verify module correctness
     * 
     * @return true إذا كانت الوحدة صحيحة / true if module is valid
     */
    bool verify() const;
    
    // ========================================================================
    // Optimization / التحسين
    // ========================================================================
    
    /**
     * تعيين مستوى التحسين
     * Set optimization level
     * 
     * @param level مستوى التحسين (O0, O1, O2, O3, Os, Oz)
     */
    void setOptimizationLevel(sad::OptimizationLevel level);
    
    /**
     * الحصول على مستوى التحسين الحالي
     * Get current optimization level
     * 
     * @return مستوى التحسين / Optimization level
     */
    sad::OptimizationLevel getOptimizationLevel() const;
    
    /**
     * تحسين الوحدة الحالية
     * Optimize current module
     * 
     * @return true إذا نجح التحسين / true if optimization succeeded
     * 
     * @details
     * (AR) يطبق تحسينات LLVM حسب المستوى المحدد:
     *      - O0: بدون تحسين (للتنقيح)
     *      - O1: تحسينات أساسية
     *      - O2: تحسينات قياسية (موصى به)
     *      - O3: تحسينات عدوانية (أقصى أداء)
     *      - Os/Oz: تحسين للحجم
     * 
     * (EN) Applies LLVM optimizations based on level:
     *      - O0: No optimization (debug)
     *      - O1: Basic optimizations
     *      - O2: Standard optimizations (recommended)
     *      - O3: Aggressive optimizations (max performance)
     *      - Os/Oz: Size optimizations
     */
    bool optimize();
    
    /**
     * تمكين/تعطيل التحسين التلقائي بعد التوليد
     * Enable/disable automatic optimization after generation
     * 
     * @param enable true للتمكين / true to enable
     */
    void setAutoOptimize(bool enable) { autoOptimize_ = enable; }
    
    /**
     * الحصول على إحصائيات التحسين
     * Get optimization statistics
     * 
     * @return إحصائيات التحسين / Optimization statistics
     */
    const sad::OptimizationStats& getOptimizationStats() const;
    
    /**
     * طباعة إحصائيات التحسين
     * Print optimization statistics
     */
    void printOptimizationStats() const;
    
    // ========================================================================
    // Module Emission / إصدار الوحدة
    // ========================================================================
    
    /**
     * إصدار وحدة SIR كاملة
     * Emit complete SIR module
     * 
     * @param sirModule وحدة SIR / SIR module
     */
    void emitModule(std::shared_ptr<SIRModule> sirModule);
    
    /**
     * معالجة الأصناف وإنشاء أنواع الهياكل
     * Pre-process classes and create struct types
     * 
     * @param sirModule وحدة SIR / SIR module
     */
    void preprocessClasses(std::shared_ptr<SIRModule> sirModule);
    
    /**
     * إصدار الدوال العامة / Global functions
     * Emit global functions
     * 
     * @param sirModule وحدة SIR / SIR module
     */
    void emitGlobalFunctions(std::shared_ptr<SIRModule> sirModule);
    
    /**
     * إضافة دالة main كـ wrapper للدالة الرئيسية العربية
     * Add main wrapper for Arabic main function
     * 
     * @param sirModule وحدة SIR / SIR module
     */
    void emitMainWrapper(std::shared_ptr<SIRModule> sirModule);
    
    /**
     * إصدار المتغيرات العامة
     * Emit global variables
     * 
     * @param sirModule وحدة SIR / SIR module
     */
    void emitGlobalVariables(std::shared_ptr<SIRModule> sirModule);
    
    /**
     * إصدار الثوابت
     * Emit constants
     * 
     * @param sirModule وحدة SIR / SIR module
     */
    void emitConstants(std::shared_ptr<SIRModule> sirModule);
    
    // ========================================================================
    // Function Emission / إصدار الدوال
    // ========================================================================
    
    /**
     * إصدار دالة LLVM من دالة SIR
     * Emit LLVM function from SIR function
     * 
     * @param sirFunc دالة SIR / SIR function
     * @return دالة LLVM / LLVM function
     */
    llvm::Function* emitFunction(std::shared_ptr<SIRFunction> sirFunc);
    
    /**
     * إصدار جسم الدالة
     * Emit function body
     * 
     * @param sirFunc دالة SIR / SIR function
     * @param llvmFunc دالة LLVM / LLVM function
     */
    void emitFunctionBody(std::shared_ptr<SIRFunction> sirFunc, llvm::Function* llvmFunc);
    
    /**
     * إصدار توقيع الدالة فقط (بدون جسم)
     * Emit function signature only (without body)
     * 
     * @param sirFunc دالة SIR / SIR function
     * @return دالة LLVM / LLVM function
     */
    llvm::Function* emitFunctionPrototype(std::shared_ptr<SIRFunction> sirFunc);
    
    /**
     * إصدار معاملات الدالة
     * Emit function parameters
     * 
     * @param sirFunc دالة SIR / SIR function
     * @param llvmFunc دالة LLVM / LLVM function
     */
    void emitFunctionParameters(std::shared_ptr<SIRFunction> sirFunc, llvm::Function* llvmFunc);
    
    // ========================================================================
    // Basic Block Emission / إصدار الكتل الأساسية
    // ========================================================================
    
    /**
     * إصدار كتلة أساسية
     * Emit basic block
     * 
     * @param sirBlock كتلة SIR الأساسية / SIR basic block
     * @param function الدالة المحتوية / Containing function
     * @return كتلة LLVM الأساسية / LLVM basic block
     */
    llvm::BasicBlock* emitBasicBlock(std::shared_ptr<SIRBasicBlock> sirBlock,
                                     llvm::Function* function);
    
    /**
     * إنشاء كتلة أساسية فارغة
     * Create empty basic block
     * 
     * @param name اسم الكتلة / Block name
     * @param function الدالة المحتوية / Containing function
     * @return كتلة LLVM الأساسية / LLVM basic block
     */
    llvm::BasicBlock* createBasicBlock(const std::string& name, llvm::Function* function);
    
    // ========================================================================
    // Instruction Emission / إصدار التعليمات
    // ========================================================================
    
    /**
     * إصدار تعليمة من SIR
     * Emit instruction from SIR
     * 
     * @param inst تعليمة SIR / SIR instruction
     * @return قيمة LLVM الناتجة / Resulting LLVM value
     */
    llvm::Value* emitInstruction(std::shared_ptr<SIRInstruction> inst);
    
    // ------------------------------------------------------------------------
    // Arithmetic Instructions / التعليمات الحسابية
    // ------------------------------------------------------------------------
    
    llvm::Value* emitAdd(std::shared_ptr<SIRInstruction> inst);      // جمع / Add
    llvm::Value* emitSub(std::shared_ptr<SIRInstruction> inst);      // طرح / Subtract
    llvm::Value* emitMul(std::shared_ptr<SIRInstruction> inst);      // ضرب / Multiply
    llvm::Value* emitDiv(std::shared_ptr<SIRInstruction> inst);      // قسمة / Divide
    llvm::Value* emitMod(std::shared_ptr<SIRInstruction> inst);      // باقي القسمة / Modulo
    llvm::Value* emitNeg(std::shared_ptr<SIRInstruction> inst);      // نفي / Negate
    
    // ------------------------------------------------------------------------
    // Bitwise Instructions / التعليمات الثنائية
    // ------------------------------------------------------------------------
    
    llvm::Value* emitAnd(std::shared_ptr<SIRInstruction> inst);      // AND ثنائي
    llvm::Value* emitOr(std::shared_ptr<SIRInstruction> inst);       // OR ثنائي
    llvm::Value* emitXor(std::shared_ptr<SIRInstruction> inst);      // XOR ثنائي
    llvm::Value* emitNot(std::shared_ptr<SIRInstruction> inst);      // NOT ثنائي
    llvm::Value* emitShl(std::shared_ptr<SIRInstruction> inst);      // إزاحة يسار / Shift left
    llvm::Value* emitShr(std::shared_ptr<SIRInstruction> inst);      // إزاحة يمين / Shift right
    
    // ------------------------------------------------------------------------
    // Comparison Instructions / تعليمات المقارنة
    // ------------------------------------------------------------------------
    
    llvm::Value* emitCmpEq(std::shared_ptr<SIRInstruction> inst);    // يساوي / Equal
    llvm::Value* emitCmpNe(std::shared_ptr<SIRInstruction> inst);    // لا يساوي / Not equal
    llvm::Value* emitCmpLt(std::shared_ptr<SIRInstruction> inst);    // أصغر من / Less than
    llvm::Value* emitCmpLe(std::shared_ptr<SIRInstruction> inst);    // أصغر أو يساوي / Less or equal
    llvm::Value* emitCmpGt(std::shared_ptr<SIRInstruction> inst);    // أكبر من / Greater than
    llvm::Value* emitCmpGe(std::shared_ptr<SIRInstruction> inst);    // أكبر أو يساوي / Greater or equal
    
    // ------------------------------------------------------------------------
    // Memory Instructions / تعليمات الذاكرة
    // ------------------------------------------------------------------------
    
    llvm::Value* emitLoad(std::shared_ptr<SIRInstruction> inst);     // تحميل / Load
    llvm::Value* emitStore(std::shared_ptr<SIRInstruction> inst);    // تخزين / Store
    llvm::Value* emitAlloca(std::shared_ptr<SIRInstruction> inst);   // تخصيص / Allocate
    llvm::Value* emitGEP(std::shared_ptr<SIRInstruction> inst);      // Get Element Ptr
    llvm::Value* emitMove(std::shared_ptr<SIRInstruction> inst);     // نقل / Move (register assignment)
    
    // ------------------------------------------------------------------------
    // Control Flow Instructions / تعليمات تدفق التحكم
    // ------------------------------------------------------------------------
    
    llvm::Value* emitBranch(std::shared_ptr<SIRInstruction> inst);           // فرع / Branch
    llvm::Value* emitCondBranch(std::shared_ptr<SIRInstruction> inst);       // فرع شرطي / Conditional branch
    llvm::Value* emitCall(std::shared_ptr<SIRInstruction> inst);             // استدعاء / Call
    llvm::Value* emitReturn(std::shared_ptr<SIRInstruction> inst);           // رجوع / Return
    llvm::Value* emitSwitch(std::shared_ptr<SIRInstruction> inst);           // تبديل / Switch
    
    // ------------------------------------------------------------------------
    // Builtin Functions / الدوال المضمنة
    // ------------------------------------------------------------------------
    
    llvm::Value* emitBuiltinPrint(std::shared_ptr<SIRInstruction> inst);     // اطبع / Print
    llvm::Value* emitBuiltinRead(std::shared_ptr<SIRInstruction> inst);      // اقرأ / Read/Input
    llvm::Value* emitStringConcat(std::shared_ptr<SIRInstruction> inst);     // دمج نص / String concat
    llvm::Value* emitStringCharAt(std::shared_ptr<SIRInstruction> inst);     // رمز_حرف / Char at index
    llvm::Value* emitStringCmp(std::shared_ptr<SIRInstruction> inst);        // مقارنة نصوص / String compare
    
    // (AR) دالة مساعدة: تحميل القيمة تلقائياً من alloca إذا لزم الأمر
    // (EN) Helper: Auto-load value from alloca pointer if needed
    llvm::Value* resolveValue(llvm::Value* val, SIRType sirType);
    
    llvm::Value* emitStringToI64(std::shared_ptr<SIRInstruction> inst);      // لرقم / String to I64
    llvm::Value* emitStringToF64(std::shared_ptr<SIRInstruction> inst);      // لعشري / String to F64
    llvm::Value* emitBuiltinSqrt(std::shared_ptr<SIRInstruction> inst);      // جذر / Sqrt
    llvm::Value* emitBuiltinPow(std::shared_ptr<SIRInstruction> inst);       // أس / Power
    llvm::Value* emitBuiltinAbs(std::shared_ptr<SIRInstruction> inst);       // مطلق / Abs
    llvm::Value* emitBuiltinRound(std::shared_ptr<SIRInstruction> inst);     // تقريب / Round
    llvm::Value* emitBuiltinFloor(std::shared_ptr<SIRInstruction> inst);     // أرضية / Floor
    llvm::Value* emitBuiltinCeil(std::shared_ptr<SIRInstruction> inst);      // سقف / Ceil
    llvm::Value* emitBuiltinSin(std::shared_ptr<SIRInstruction> inst);       // جيب / Sin
    llvm::Value* emitBuiltinCos(std::shared_ptr<SIRInstruction> inst);       // جيب_تمام / Cos
    llvm::Value* emitBuiltinTan(std::shared_ptr<SIRInstruction> inst);       // ظل / Tan
    
    // String Functions (12)
    llvm::Value* emitBuiltinStringLength(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringToUpper(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringToLower(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringFind(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringReplace(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringSubstring(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringTrim(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringSplit(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringJoin(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringStartsWith(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringEndsWith(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringContains(std::shared_ptr<SIRInstruction> inst);
    
    // Array Functions (10)
    llvm::Value* emitBuiltinArrayAppend(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinArrayRemove(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinArraySize(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinArrayIndexOf(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinArrayContains(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinArrayReverse(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinArraySort(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinArrayFirst(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinArrayLast(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinArraySlice(std::shared_ptr<SIRInstruction> inst);
    
    // File I/O Functions (8)
    llvm::Value* emitBuiltinFileRead(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinFileWrite(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinFileAppend(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinFileDelete(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinFileCopy(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinFileMove(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinFileCreateDir(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinFileListDir(std::shared_ptr<SIRInstruction> inst);
    
    // Utility Functions (4)
    llvm::Value* emitBuiltinRandom(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinSleep(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinExit(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinTypeOf(std::shared_ptr<SIRInstruction> inst);
    
    // New stdlib builtins - دوال المكتبة القياسية الجديدة
    llvm::Value* emitBuiltinIsType(std::shared_ptr<SIRInstruction> inst, const std::string& typeName);
    llvm::Value* emitBuiltinToBool(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinReadLine(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinClearScreen(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinSum(std::shared_ptr<SIRInstruction> inst);
    
    // ================================================================
    // عمليات برمجة أنظمة التشغيل — OS Development Operations
    // ================================================================
    llvm::Value* emitInlineAsm(std::shared_ptr<SIRInstruction> inst);       // تجميع مضمّن
    llvm::Value* emitPortWrite(std::shared_ptr<SIRInstruction> inst);       // منفذ_اكتب / outb/outw/outl
    llvm::Value* emitPortRead(std::shared_ptr<SIRInstruction> inst);        // منفذ_اقرأ / inb/inw/inl
    llvm::Value* emitMemWrite(std::shared_ptr<SIRInstruction> inst);        // ذاكرة_اكتب / poke
    llvm::Value* emitMemRead(std::shared_ptr<SIRInstruction> inst);         // ذاكرة_اقرأ / peek
    llvm::Value* emitInterrupt(std::shared_ptr<SIRInstruction> inst);       // مقاطعة / int
    llvm::Value* emitHalt(std::shared_ptr<SIRInstruction> inst);            // توقف / hlt
    llvm::Value* emitCli(std::shared_ptr<SIRInstruction> inst);             // تعطيل_مقاطعات / cli
    llvm::Value* emitSti(std::shared_ptr<SIRInstruction> inst);             // تفعيل_مقاطعات / sti
    llvm::Value* emitVgaWrite(std::shared_ptr<SIRInstruction> inst);        // شاشة_اكتب / vga_write
    llvm::Value* emitVgaClear(std::shared_ptr<SIRInstruction> inst);        // شاشة_امسح / vga_clear
    llvm::Value* emitMemCopy(std::shared_ptr<SIRInstruction> inst);         // انسخ_ذاكرة / memcpy
    llvm::Value* emitMemSet(std::shared_ptr<SIRInstruction> inst);          // املأ_ذاكرة / memset
    llvm::Value* emitAddrOf(std::shared_ptr<SIRInstruction> inst);          // عنوان / addr_of
    
    // ================================================================
    // عمليات Embedded المتقدمة — Advanced Embedded Operations (18)
    // ================================================================
    // Serial I/O (4)
    llvm::Value* emitSerialInit(std::shared_ptr<SIRInstruction> inst);      // تسلسلي_هيئ / serial_init
    llvm::Value* emitSerialWrite(std::shared_ptr<SIRInstruction> inst);     // تسلسلي_ارسل / serial_send
    llvm::Value* emitSerialRead(std::shared_ptr<SIRInstruction> inst);      // تسلسلي_استقبل / serial_receive
    llvm::Value* emitSerialReady(std::shared_ptr<SIRInstruction> inst);     // تسلسلي_جاهز / serial_ready
    // GPIO (3)
    llvm::Value* emitGpioWrite(std::shared_ptr<SIRInstruction> inst);       // منفذ_رقمي_اكتب / gpio_write
    llvm::Value* emitGpioRead(std::shared_ptr<SIRInstruction> inst);        // منفذ_رقمي_اقرأ / gpio_read
    llvm::Value* emitGpioMode(std::shared_ptr<SIRInstruction> inst);        // حدد_وضع_منفذ / gpio_mode
    // Timer (3)
    llvm::Value* emitTimerInit(std::shared_ptr<SIRInstruction> inst);       // مؤقت_هيئ / timer_init
    llvm::Value* emitTimerRead(std::shared_ptr<SIRInstruction> inst);       // مؤقت_قراءة / timer_read
    llvm::Value* emitTimerWait(std::shared_ptr<SIRInstruction> inst);       // مؤقت_انتظر / timer_wait
    // System Control (3)
    llvm::Value* emitReset(std::shared_ptr<SIRInstruction> inst);           // اعد_تشغيل / reset
    llvm::Value* emitCpuId(std::shared_ptr<SIRInstruction> inst);           // معرف_المعالج / cpuid
    llvm::Value* emitRdtsc(std::shared_ptr<SIRInstruction> inst);           // عداد_الدورات / rdtsc
    // Memory Barriers (3)
    llvm::Value* emitMfence(std::shared_ptr<SIRInstruction> inst);          // حاجز_ذاكرة / mfence
    llvm::Value* emitLfence(std::shared_ptr<SIRInstruction> inst);          // حاجز_قراءة / lfence
    llvm::Value* emitSfence(std::shared_ptr<SIRInstruction> inst);          // حاجز_كتابة / sfence
    // DMA (2)
    llvm::Value* emitDmaInit(std::shared_ptr<SIRInstruction> inst);         // نقل_مباشر_هيئ / dma_init
    llvm::Value* emitDmaStart(std::shared_ptr<SIRInstruction> inst);        // نقل_مباشر_ابدأ / dma_start
    
    // ================================================================
    // دوال الأمان — Security Functions (14)
    // ================================================================
    llvm::Value* emitBuiltinSecurityAssert(std::shared_ptr<SIRInstruction> inst);        // تأكد
    llvm::Value* emitBuiltinSecurityVerify(std::shared_ptr<SIRInstruction> inst);        // تحقق
    llvm::Value* emitBuiltinSecurityIsSafe(std::shared_ptr<SIRInstruction> inst);        // آمن
    llvm::Value* emitBuiltinSecurityPanic(std::shared_ptr<SIRInstruction> inst);         // ذعر
    llvm::Value* emitBuiltinSecurityHash(std::shared_ptr<SIRInstruction> inst);          // هاش
    llvm::Value* emitBuiltinSecurityEncrypt(std::shared_ptr<SIRInstruction> inst);       // شفّر
    llvm::Value* emitBuiltinSecurityDecrypt(std::shared_ptr<SIRInstruction> inst);       // فك_تشفير
    llvm::Value* emitBuiltinSecurityAssertType(std::shared_ptr<SIRInstruction> inst);    // تأكد_نوع
    llvm::Value* emitBuiltinSecurityAssertEqual(std::shared_ptr<SIRInstruction> inst);   // تأكد_مساواة
    llvm::Value* emitBuiltinSecurityAssertGreater(std::shared_ptr<SIRInstruction> inst); // تأكد_أكبر
    llvm::Value* emitBuiltinSecuritySanitize(std::shared_ptr<SIRInstruction> inst);      // نظّف
    llvm::Value* emitBuiltinSecurityTimestamp(std::shared_ptr<SIRInstruction> inst);     // وقت_الآن
    llvm::Value* emitBuiltinSecuritySecureRandom(std::shared_ptr<SIRInstruction> inst);  // عشوائي_آمن
    llvm::Value* emitBuiltinSecurityBase64Encode(std::shared_ptr<SIRInstruction> inst);  // ترميز_64
    
    // ================================================================
    // التكامل مع C/C++ — FFI Functions (20)
    // ================================================================
    llvm::Value* emitFFIPrintf(std::shared_ptr<SIRInstruction> inst);       // طباعة_تنسيق / printf
    llvm::Value* emitFFIMalloc(std::shared_ptr<SIRInstruction> inst);       // حجز / malloc
    llvm::Value* emitFFIFree(std::shared_ptr<SIRInstruction> inst);         // حرر / free
    llvm::Value* emitFFIRealloc(std::shared_ptr<SIRInstruction> inst);      // اعد_حجز / realloc
    llvm::Value* emitFFICalloc(std::shared_ptr<SIRInstruction> inst);       // حجز_صفري / calloc
    llvm::Value* emitFFIStrlen(std::shared_ptr<SIRInstruction> inst);       // طول_نص_س / strlen
    llvm::Value* emitFFIStrcpy(std::shared_ptr<SIRInstruction> inst);       // انسخ_نص_س / strcpy
    llvm::Value* emitFFIStrcmp(std::shared_ptr<SIRInstruction> inst);       // قارن_نص_س / strcmp
    llvm::Value* emitFFIStrcat(std::shared_ptr<SIRInstruction> inst);       // الحق_نص_س / strcat
    llvm::Value* emitFFIMemcpy(std::shared_ptr<SIRInstruction> inst);       // انسخ_ذاكرة_س / memcpy
    llvm::Value* emitFFIMemset(std::shared_ptr<SIRInstruction> inst);       // عبئ_ذاكرة_س / memset
    llvm::Value* emitFFIFopen(std::shared_ptr<SIRInstruction> inst);        // افتح_ملف_س / fopen
    llvm::Value* emitFFIFclose(std::shared_ptr<SIRInstruction> inst);       // اغلق_ملف_س / fclose
    llvm::Value* emitFFIFwrite(std::shared_ptr<SIRInstruction> inst);       // اكتب_ملف_س / fputs
    llvm::Value* emitFFIFread(std::shared_ptr<SIRInstruction> inst);        // اقرأ_ملف_س / fgets
    llvm::Value* emitFFISystem(std::shared_ptr<SIRInstruction> inst);       // نفذ_امر / system
    llvm::Value* emitFFIGetenv(std::shared_ptr<SIRInstruction> inst);       // قيمة_بيئة / getenv
    llvm::Value* emitFFIAtoi(std::shared_ptr<SIRInstruction> inst);         // نص_لعدد / atoi
    llvm::Value* emitFFIAtof(std::shared_ptr<SIRInstruction> inst);         // نص_لعشري / atof
    llvm::Value* emitFFISnprintf(std::shared_ptr<SIRInstruction> inst);     // تنسيق_نص / snprintf
    
    // ========================================================================
    // Async/Await & Concurrency / تعليمات التزامن
    // ========================================================================
    llvm::Value* emitAsyncSpawn(std::shared_ptr<SIRInstruction> inst);        // أنشئ_مهمة
    llvm::Value* emitAsyncAwait(std::shared_ptr<SIRInstruction> inst);        // انتظر_مهمة
    llvm::Value* emitAsyncYield(std::shared_ptr<SIRInstruction> inst);        // أنتج
    llvm::Value* emitAsyncSleep(std::shared_ptr<SIRInstruction> inst);        // نوم_غير_متزامن
    llvm::Value* emitAsyncCreateFuture(std::shared_ptr<SIRInstruction> inst); // أنشئ_مستقبل
    llvm::Value* emitAsyncResolveFuture(std::shared_ptr<SIRInstruction> inst);// أوفِ_مستقبل
    llvm::Value* emitAsyncGetFuture(std::shared_ptr<SIRInstruction> inst);    // احصل_مستقبل
    llvm::Value* emitAsyncCreateChannel(std::shared_ptr<SIRInstruction> inst);// أنشئ_قناة
    llvm::Value* emitAsyncChannelSend(std::shared_ptr<SIRInstruction> inst);  // أرسل_قناة
    llvm::Value* emitAsyncChannelRecv(std::shared_ptr<SIRInstruction> inst);  // استقبل_قناة
    llvm::Value* emitAsyncChannelClose(std::shared_ptr<SIRInstruction> inst); // أغلق_قناة
    llvm::Value* emitAsyncMutexCreate(std::shared_ptr<SIRInstruction> inst);  // أنشئ_قفل
    llvm::Value* emitAsyncMutexLock(std::shared_ptr<SIRInstruction> inst);    // اقفل
    llvm::Value* emitAsyncMutexUnlock(std::shared_ptr<SIRInstruction> inst);  // افتح_قفل
    llvm::Value* emitAsyncThreadSpawn(std::shared_ptr<SIRInstruction> inst);  // أنشئ_خيط
    llvm::Value* emitAsyncThreadJoin(std::shared_ptr<SIRInstruction> inst);   // انضم_خيط
    llvm::Value* emitAsyncAtomicLoad(std::shared_ptr<SIRInstruction> inst);   // حمّل_ذري
    llvm::Value* emitAsyncAtomicStore(std::shared_ptr<SIRInstruction> inst);  // خزّن_ذري
    llvm::Value* emitAsyncAtomicAdd(std::shared_ptr<SIRInstruction> inst);    // أضف_ذري
    llvm::Value* emitAsyncAtomicCAS(std::shared_ptr<SIRInstruction> inst);    // قارن_وبدّل
    llvm::Value* emitAsyncWaitAll(std::shared_ptr<SIRInstruction> inst);      // انتظر_الكل
    llvm::Value* emitAsyncWaitAny(std::shared_ptr<SIRInstruction> inst);      // انتظر_أي
    llvm::Value* emitAsyncSelect(std::shared_ptr<SIRInstruction> inst);       // اختر_قناة
    
    // ================================================================
    // Section 14b: LLVM Coroutine Emit Functions / دوال إصدار الكوروتين
    // ================================================================
    void emitCoroutinePreamble(std::shared_ptr<SIRFunction> sirFunc, llvm::Function* llvmFunc);
    void emitCoroutineEpilogue();
    llvm::Value* emitCoroSuspend(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitCoroReturn(std::shared_ptr<SIRInstruction> inst);
    
    // ================================================================
    // Section 14c: Generator Emit Functions / دوال إصدار المولّد
    // ================================================================
    llvm::Value* emitGeneratorYield(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitGeneratorConsume(std::shared_ptr<SIRInstruction> inst);
    
    // ================================================================
    // Section 15: عمليات وحدات نظام التشغيل المتقدمة / Advanced OS Module Operations
    // (AR) دعم المترجم للمكتبة المنخفضة المستوى الكاملة (19 وحدة)
    // (EN) Compiler support for the full low-level library (19 modules)
    // ================================================================
    
    // 15a. وحدة المعالج المتقدمة / Advanced CPU Module (8)
    llvm::Value* emitLowlevelCpuGetInfo(std::shared_ptr<SIRInstruction> inst);      // معلومات_المعالج
    llvm::Value* emitLowlevelCpuGetFeatures(std::shared_ptr<SIRInstruction> inst);  // ميزات_المعالج
    llvm::Value* emitLowlevelCpuReadMSR(std::shared_ptr<SIRInstruction> inst);      // اقرأ_سجل_نموذج
    llvm::Value* emitLowlevelCpuWriteMSR(std::shared_ptr<SIRInstruction> inst);     // اكتب_سجل_نموذج
    llvm::Value* emitLowlevelCpuReadCR(std::shared_ptr<SIRInstruction> inst);       // اقرأ_سجل_تحكم
    llvm::Value* emitLowlevelCpuWriteCR(std::shared_ptr<SIRInstruction> inst);      // اكتب_سجل_تحكم
    llvm::Value* emitLowlevelCpuInvlpg(std::shared_ptr<SIRInstruction> inst);       // ابطل_صفحة
    llvm::Value* emitLowlevelCpuGetReport(std::shared_ptr<SIRInstruction> inst);    // تقرير_المعالج
    
    // 15b. وحدة GDT (3)
    llvm::Value* emitLowlevelGdtInit(std::shared_ptr<SIRInstruction> inst);         // هيئ_جدول_واصفات
    llvm::Value* emitLowlevelGdtLoad(std::shared_ptr<SIRInstruction> inst);         // حمل_جدول_واصفات
    llvm::Value* emitLowlevelGdtGetReport(std::shared_ptr<SIRInstruction> inst);    // تقرير_واصفات
    
    // 15c. وحدة الترحيل / Paging (5)
    llvm::Value* emitLowlevelPagingInit(std::shared_ptr<SIRInstruction> inst);      // هيئ_ترحيل
    llvm::Value* emitLowlevelPagingMap(std::shared_ptr<SIRInstruction> inst);       // رحل_صفحة
    llvm::Value* emitLowlevelPagingUnmap(std::shared_ptr<SIRInstruction> inst);     // الغ_ترحيل
    llvm::Value* emitLowlevelPagingFlushTlb(std::shared_ptr<SIRInstruction> inst);  // افرغ_ذاكرة_ترجمة
    llvm::Value* emitLowlevelPagingGetReport(std::shared_ptr<SIRInstruction> inst); // تقرير_ترحيل
    
    // 15d. وحدة المقاطعات المتقدمة / Advanced Interrupts/IDT (5)
    llvm::Value* emitLowlevelIdtInit(std::shared_ptr<SIRInstruction> inst);         // هيئ_جدول_مقاطعات
    llvm::Value* emitLowlevelIdtLoad(std::shared_ptr<SIRInstruction> inst);         // حمل_جدول_مقاطعات
    llvm::Value* emitLowlevelIdtRegisterIsr(std::shared_ptr<SIRInstruction> inst);  // سجل_معالج_مقاطعة
    llvm::Value* emitLowlevelIdtEnableIrq(std::shared_ptr<SIRInstruction> inst);    // فعل_طلب_مقاطعة
    llvm::Value* emitLowlevelIdtGetReport(std::shared_ptr<SIRInstruction> inst);    // تقرير_مقاطعات
    
    // 15e. وحدة PCI (5)
    llvm::Value* emitLowlevelPciEnumerate(std::shared_ptr<SIRInstruction> inst);    // عدد_أجهزة_ناقل
    llvm::Value* emitLowlevelPciReadConfig(std::shared_ptr<SIRInstruction> inst);   // اقرأ_اعدادات_ناقل
    llvm::Value* emitLowlevelPciWriteConfig(std::shared_ptr<SIRInstruction> inst);  // اكتب_اعدادات_ناقل
    llvm::Value* emitLowlevelPciGetDeviceCount(std::shared_ptr<SIRInstruction> inst); // عدد_الأجهزة
    llvm::Value* emitLowlevelPciGetReport(std::shared_ptr<SIRInstruction> inst);    // تقرير_ناقل
    
    // 15f. وحدة DMA المتقدمة (4)
    llvm::Value* emitLowlevelDmaInit(std::shared_ptr<SIRInstruction> inst);         // هيئ_نقل_مباشر
    llvm::Value* emitLowlevelDmaTransfer(std::shared_ptr<SIRInstruction> inst);     // ابدأ_نقل
    llvm::Value* emitLowlevelDmaStatus(std::shared_ptr<SIRInstruction> inst);       // حالة_نقل
    llvm::Value* emitLowlevelDmaGetReport(std::shared_ptr<SIRInstruction> inst);    // تقرير_نقل
    
    // 15g. وحدة الشاشة / Framebuffer (8)
    llvm::Value* emitLowlevelFbInit(std::shared_ptr<SIRInstruction> inst);          // هيئ_شاشة
    llvm::Value* emitLowlevelFbSetPixel(std::shared_ptr<SIRInstruction> inst);      // ارسم_نقطة
    llvm::Value* emitLowlevelFbDrawRect(std::shared_ptr<SIRInstruction> inst);      // ارسم_مستطيل
    llvm::Value* emitLowlevelFbFillRect(std::shared_ptr<SIRInstruction> inst);      // املأ_مستطيل
    llvm::Value* emitLowlevelFbDrawLine(std::shared_ptr<SIRInstruction> inst);      // ارسم_خط
    llvm::Value* emitLowlevelFbDrawString(std::shared_ptr<SIRInstruction> inst);    // ارسم_نص
    llvm::Value* emitLowlevelFbClear(std::shared_ptr<SIRInstruction> inst);         // امسح_شاشة
    llvm::Value* emitLowlevelFbGetReport(std::shared_ptr<SIRInstruction> inst);     // تقرير_شاشة
    
    // 15h. وحدة ACPI (4)
    llvm::Value* emitLowlevelAcpiInit(std::shared_ptr<SIRInstruction> inst);        // هيئ_طاقة
    llvm::Value* emitLowlevelAcpiFindTable(std::shared_ptr<SIRInstruction> inst);   // ابحث_جدول_طاقة
    llvm::Value* emitLowlevelAcpiShutdown(std::shared_ptr<SIRInstruction> inst);    // اطفئ
    llvm::Value* emitLowlevelAcpiGetReport(std::shared_ptr<SIRInstruction> inst);   // تقرير_طاقة
    
    // 15i. وحدة التزامن / Sync (8)
    llvm::Value* emitLowlevelSpinlockInit(std::shared_ptr<SIRInstruction> inst);    // هيئ_قفل_دوار
    llvm::Value* emitLowlevelSpinlockLock(std::shared_ptr<SIRInstruction> inst);    // اقفل_دوار
    llvm::Value* emitLowlevelSpinlockUnlock(std::shared_ptr<SIRInstruction> inst);  // افتح_قفل_دوار
    llvm::Value* emitLowlevelMutexInit(std::shared_ptr<SIRInstruction> inst);       // هيئ_كابح
    llvm::Value* emitLowlevelMutexLock(std::shared_ptr<SIRInstruction> inst);       // اقفل_كابح
    llvm::Value* emitLowlevelMutexUnlock(std::shared_ptr<SIRInstruction> inst);     // افتح_كابح
    llvm::Value* emitLowlevelSemaphoreInit(std::shared_ptr<SIRInstruction> inst);   // هيئ_اشارة
    llvm::Value* emitLowlevelBarrierInit(std::shared_ptr<SIRInstruction> inst);     // هيئ_حاجز
    
    // 15j. وحدة المجدول / Scheduler (6)
    llvm::Value* emitLowlevelSchedInit(std::shared_ptr<SIRInstruction> inst);       // هيئ_مجدول
    llvm::Value* emitLowlevelSchedCreateProc(std::shared_ptr<SIRInstruction> inst); // انشئ_عملية
    llvm::Value* emitLowlevelSchedCreateThread(std::shared_ptr<SIRInstruction> inst); // انشئ_خيط_نواة
    llvm::Value* emitLowlevelSchedYield(std::shared_ptr<SIRInstruction> inst);      // تنازل
    llvm::Value* emitLowlevelSchedSleep(std::shared_ptr<SIRInstruction> inst);      // نوم_مجدول
    llvm::Value* emitLowlevelSchedGetReport(std::shared_ptr<SIRInstruction> inst);  // تقرير_مجدول
    
    // 15k. وحدة الإقلاع / Boot (3)
    llvm::Value* emitLowlevelBootGetInfo(std::shared_ptr<SIRInstruction> inst);     // معلومات_اقلاع
    llvm::Value* emitLowlevelBootGetMemoryMap(std::shared_ptr<SIRInstruction> inst);// خريطة_ذاكرة_اقلاع
    llvm::Value* emitLowlevelBootGetReport(std::shared_ptr<SIRInstruction> inst);   // تقرير_اقلاع
    
    // 15l. وحدة نظام الملفات / VFS (7)
    llvm::Value* emitLowlevelVfsMount(std::shared_ptr<SIRInstruction> inst);        // حمل_قرص
    llvm::Value* emitLowlevelVfsUnmount(std::shared_ptr<SIRInstruction> inst);      // افصل_قرص
    llvm::Value* emitLowlevelVfsOpen(std::shared_ptr<SIRInstruction> inst);         // افتح_ملف_نواة
    llvm::Value* emitLowlevelVfsRead(std::shared_ptr<SIRInstruction> inst);         // اقرأ_ملف_نواة
    llvm::Value* emitLowlevelVfsWrite(std::shared_ptr<SIRInstruction> inst);        // اكتب_ملف_نواة
    llvm::Value* emitLowlevelVfsClose(std::shared_ptr<SIRInstruction> inst);        // اغلق_ملف_نواة
    llvm::Value* emitLowlevelVfsGetReport(std::shared_ptr<SIRInstruction> inst);    // تقرير_ملفات
    
    // 15m. وحدة APIC (5)
    llvm::Value* emitLowlevelApicInit(std::shared_ptr<SIRInstruction> inst);        // هيئ_متحكم_مقاطعات
    llvm::Value* emitLowlevelApicSendEoi(std::shared_ptr<SIRInstruction> inst);     // ارسل_نهاية_مقاطعة
    llvm::Value* emitLowlevelApicSendIpi(std::shared_ptr<SIRInstruction> inst);     // ارسل_مقاطعة_معالج
    llvm::Value* emitLowlevelApicSetTimer(std::shared_ptr<SIRInstruction> inst);    // اضبط_مؤقت_متحكم
    llvm::Value* emitLowlevelApicGetReport(std::shared_ptr<SIRInstruction> inst);   // تقرير_متحكم_مقاطعات
    
    // 15n. وحدة HPET (4)
    llvm::Value* emitLowlevelHpetInit(std::shared_ptr<SIRInstruction> inst);        // هيئ_مؤقت_دقيق
    llvm::Value* emitLowlevelHpetRead(std::shared_ptr<SIRInstruction> inst);        // اقرأ_مؤقت_دقيق
    llvm::Value* emitLowlevelHpetSleep(std::shared_ptr<SIRInstruction> inst);       // نوم_دقيق
    llvm::Value* emitLowlevelHpetGetReport(std::shared_ptr<SIRInstruction> inst);   // تقرير_مؤقت_دقيق
    
    // 15o. وحدة استدعاءات النظام / Syscall (4)
    llvm::Value* emitLowlevelSyscallInit(std::shared_ptr<SIRInstruction> inst);     // هيئ_استدعاءات
    llvm::Value* emitLowlevelSyscallRegister(std::shared_ptr<SIRInstruction> inst); // سجل_استدعاء
    llvm::Value* emitLowlevelSyscallInvoke(std::shared_ptr<SIRInstruction> inst);   // نفذ_استدعاء
    llvm::Value* emitLowlevelSyscallGetReport(std::shared_ptr<SIRInstruction> inst);// تقرير_استدعاءات
    
    // 15p. عمليات الذاكرة المتقدمة / Advanced Memory (4)
    llvm::Value* emitLowlevelMemAllocPhys(std::shared_ptr<SIRInstruction> inst);    // خصص_فيزيائي
    llvm::Value* emitLowlevelMemFreePhys(std::shared_ptr<SIRInstruction> inst);     // حرر_فيزيائي
    llvm::Value* emitLowlevelMemMapRegion(std::shared_ptr<SIRInstruction> inst);    // رحل_منطقة
    llvm::Value* emitLowlevelMemGetReport(std::shared_ptr<SIRInstruction> inst);    // تقرير_ذاكرة_نواة
    
    // =================================================================
    // القسم 16: بروتوكول UEFI / UEFI Boot Protocol (37 دالة)
    // =================================================================

    // 16a. التهيئة والتحكم (5)
    llvm::Value* emitLowlevelUefiInit(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiExitBootServices(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiIsInitialized(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiBsExited(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiResetSystem(std::shared_ptr<SIRInstruction> inst);

    // 16b. إدارة الذاكرة (7)
    llvm::Value* emitLowlevelUefiAllocPages(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiFreePages(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiAllocPool(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiFreePool(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiGetMemoryMap(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiGetMemmapKey(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiTotalMemory(std::shared_ptr<SIRInstruction> inst);

    // 16c. بروتوكول الرسوميات GOP (10)
    llvm::Value* emitLowlevelUefiInitGop(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiSetGopMode(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiQueryGopMode(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiGopModeCount(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiCurrentGopMode(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiFramebufferBase(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiFramebufferSize(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiFillScreen(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiDrawRect(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiGopBlt(std::shared_ptr<SIRInstruction> inst);

    // 16d. خدمات وقت التشغيل (4)
    llvm::Value* emitLowlevelUefiGetTime(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiSetTime(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiGetVariable(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiSetVariable(std::shared_ptr<SIRInstruction> inst);

    // 16e. نظام الملفات (6)
    llvm::Value* emitLowlevelUefiOpenVolume(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiOpenFile(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiReadFile(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiWriteFile(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiCloseFile(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiFileInfo(std::shared_ptr<SIRInstruction> inst);

    // 16f. بروتوكولات ومعلومات (5)
    llvm::Value* emitLowlevelUefiLocateProtocol(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiRevision(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiVendor(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiFwRevision(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelUefiReport(std::shared_ptr<SIRInstruction> inst);

    // --- القسم 17: ACPI الموسّع / Extended ACPI ---
    llvm::Value* emitLowlevelAcpiInitFull(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelAcpiInitRsdp(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelAcpiEnable(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelAcpiDisable(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelAcpiIsInitialized(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelAcpiVersion(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelAcpiReboot(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelAcpiSleep(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelAcpiDelayUs(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelAcpiReadPmTimer(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelAcpiIsPm32bit(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelAcpiProcessorCount(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelAcpiLocalApicAddr(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelAcpiEcamBase(std::shared_ptr<SIRInstruction> inst);

    // --- القسم 18: APIC الموسّع / Extended APIC ---
    llvm::Value* emitLowlevelApicSupported(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelApicX2Supported(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelApicId(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelApicIoCount(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelApicInitTimer(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelApicStartTimer(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelApicStopTimer(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelApicTimerCount(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelApicCalibrate(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelApicSetPriority(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelApicDisablePic(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelApicMaskIrq(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelApicUnmaskIrq(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelApicRouteIrq(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelApicSendIpiAll(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelApicSendInitIpi(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelApicSendSipi(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelApicWaitDelivery(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitLowlevelApicInitIo(std::shared_ptr<SIRInstruction> inst);

    // Helper for declaring runtime functions
    llvm::Function* declareRuntimeFunction(const std::string& name, 
                                          llvm::Type* returnType,
                                          const std::vector<llvm::Type*>& argTypes);
    
    // Helper for resolving SIR operands to LLVM values (constants + registers)
    llvm::Value* resolveOperand(const SIROperand& operand);
    
    // ------------------------------------------------------------------------
    // Type Conversion Instructions / تعليمات تحويل الأنواع
    // ------------------------------------------------------------------------
    
    llvm::Value* emitCast(std::shared_ptr<SIRInstruction> inst);             // تحويل عام / Cast
    llvm::Value* emitBitCast(std::shared_ptr<SIRInstruction> inst);          // تحويل ثنائي / Bitcast
    llvm::Value* emitIntToPtr(std::shared_ptr<SIRInstruction> inst);         // عدد إلى مؤشر
    llvm::Value* emitPtrToInt(std::shared_ptr<SIRInstruction> inst);         // مؤشر إلى عدد
    llvm::Value* emitTrunc(std::shared_ptr<SIRInstruction> inst);            // اقتطاع / Truncate
    llvm::Value* emitZExt(std::shared_ptr<SIRInstruction> inst);             // توسيع بصفر / Zero extend
    llvm::Value* emitSExt(std::shared_ptr<SIRInstruction> inst);             // توسيع بإشارة / Sign extend
    
    // SIR Type Conversion opcodes / أكواد تحويل الأنواع
    llvm::Value* emitI64ToF64(std::shared_ptr<SIRInstruction> inst);         // صحيح → عشري
    llvm::Value* emitF64ToI64(std::shared_ptr<SIRInstruction> inst);         // عشري → صحيح
    llvm::Value* emitI64ToBool(std::shared_ptr<SIRInstruction> inst);        // صحيح → منطقي
    llvm::Value* emitBoolToI64(std::shared_ptr<SIRInstruction> inst);        // منطقي → صحيح
    llvm::Value* emitI64ToString(std::shared_ptr<SIRInstruction> inst);      // صحيح → نص
    llvm::Value* emitF64ToString(std::shared_ptr<SIRInstruction> inst);      // عشري → نص
    llvm::Value* emitBoolToString(std::shared_ptr<SIRInstruction> inst);     // منطقي → نص
    
    // ------------------------------------------------------------------------
    // OOP Instructions / تعليمات البرمجة الكائنية
    // ------------------------------------------------------------------------
    llvm::Value* emitObjectNew(std::shared_ptr<SIRInstruction> inst);        // إنشاء كائن
    llvm::Value* emitObjectGet(std::shared_ptr<SIRInstruction> inst);        // قراءة خاصية
    llvm::Value* emitObjectSet(std::shared_ptr<SIRInstruction> inst);        // تعيين خاصية
    llvm::Value* emitObjectCall(std::shared_ptr<SIRInstruction> inst);       // استدعاء طريقة
    llvm::Value* emitInstanceOf(std::shared_ptr<SIRInstruction> inst);       // تحقق النوع
    llvm::Value* emitObjectCast(std::shared_ptr<SIRInstruction> inst);       // تحويل كائن
    llvm::Value* emitClassDef(std::shared_ptr<SIRInstruction> inst);         // تعريف صنف
    llvm::Value* emitMethodDef(std::shared_ptr<SIRInstruction> inst);        // تعريف طريقة
    llvm::Value* emitFieldDef(std::shared_ptr<SIRInstruction> inst);         // تعريف حقل
    llvm::Value* emitConstructorCall(std::shared_ptr<SIRInstruction> inst);  // استدعاء منشئ
    
    // ========================================================================
    // vtable & Virtual Dispatch / جدول الدوال الافتراضية
    // ========================================================================
    
    /// (AR) بناء vtables لجميع الأصناف بعد preprocessClasses
    /// (EN) Build vtables for all classes after preprocessClasses
    void buildClassVtables(std::shared_ptr<SIRModule> sirModule);

    /// (AR) تحديث مداخل vtable المؤجلة بعد إصدار جميع الدوال
    /// (EN) Patch deferred vtable entries after all functions are emitted
    void patchClassVtables();
    
    /// (AR) استدعاء افتراضي عبر vtable
    /// (EN) Virtual dispatch via vtable
    llvm::Value* emitVirtualCall(llvm::Value* objPtr, const std::string& className,
                                 const std::string& methodName,
                                 const std::vector<llvm::Value*>& extraArgs);
    
    /// (AR) تخزين مؤشر vtable في الحقل 0 من الكائن
    /// (EN) Store vtable pointer in field 0 of object
    void storeVtablePtr(llvm::Value* objPtr, const std::string& className);
    
    /// (AR) استدعاء دالة الهدم للكائن
    /// (EN) Call destructor for object
    void emitDestructorCall(llvm::Value* objPtr, const std::string& className);
    
    /// (AR) الحصول على إزاحة الحقول بسبب vtable
    /// (EN) Get field offset due to vtable pointer at index 0
    int getFieldStructIndex(const std::string& className, int userFieldIndex) const;
    
    // ------------------------------------------------------------------------
    // Missing Bitwise / عمليات ثنائية ناقصة
    // ------------------------------------------------------------------------
    llvm::Value* emitSar(std::shared_ptr<SIRInstruction> inst);              // إزاحة حسابية يمين
    llvm::Value* emitRol(std::shared_ptr<SIRInstruction> inst);              // دوران يسار
    
    // ------------------------------------------------------------------------
    // Missing Memory / عمليات ذاكرة ناقصة
    // ------------------------------------------------------------------------
    llvm::Value* emitCallIndirect(std::shared_ptr<SIRInstruction> inst);     // استدعاء غير مباشر
    llvm::Value* emitAllocHeap(std::shared_ptr<SIRInstruction> inst);        // تخصيص كومة
    llvm::Value* emitFreeMem(std::shared_ptr<SIRInstruction> inst);          // تحرير ذاكرة
    llvm::Value* emitAddr(std::shared_ptr<SIRInstruction> inst);             // عنوان متغير
    llvm::Value* emitPtrAdd(std::shared_ptr<SIRInstruction> inst);           // حساب مؤشرات
    llvm::Value* emitPtrCast(std::shared_ptr<SIRInstruction> inst);          // تحويل مؤشر
    
    // ------------------------------------------------------------------------
    // Array core / عمليات المصفوفات الأساسية
    // ------------------------------------------------------------------------
    llvm::Value* emitArrayNew(std::shared_ptr<SIRInstruction> inst);         // إنشاء مصفوفة
    llvm::Value* emitArrayGet(std::shared_ptr<SIRInstruction> inst);         // قراءة عنصر
    llvm::Value* emitArraySet(std::shared_ptr<SIRInstruction> inst);         // تعيين عنصر
    llvm::Value* emitArrayLen(std::shared_ptr<SIRInstruction> inst);         // طول مصفوفة
    
    // String core
    llvm::Value* emitStringNew(std::shared_ptr<SIRInstruction> inst);        // إنشاء نص
    
    // Builtin Extra
    llvm::Value* emitBuiltinMin(std::shared_ptr<SIRInstruction> inst);       // الأصغر
    llvm::Value* emitBuiltinMax(std::shared_ptr<SIRInstruction> inst);       // الأكبر
    llvm::Value* emitBuiltinAssert(std::shared_ptr<SIRInstruction> inst);    // تأكيد
    llvm::Value* emitBuiltinDebug(std::shared_ptr<SIRInstruction> inst);     // تنقيح
    
    // ------------------------------------------------------------------------
    // Aggregate Instructions / تعليمات التجميع
    // ------------------------------------------------------------------------
    
    llvm::Value* emitExtractValue(std::shared_ptr<SIRInstruction> inst);     // استخراج قيمة
    llvm::Value* emitInsertValue(std::shared_ptr<SIRInstruction> inst);      // إدراج قيمة
    llvm::Value* emitExtractElement(std::shared_ptr<SIRInstruction> inst);   // استخراج عنصر
    llvm::Value* emitInsertElement(std::shared_ptr<SIRInstruction> inst);    // إدراج عنصر
    
    // ------------------------------------------------------------------------
    // Phi & Select / فاي والاختيار
    // ------------------------------------------------------------------------
    
    llvm::Value* emitPhi(std::shared_ptr<SIRInstruction> inst);              // عقدة فاي / Phi node
    llvm::Value* emitSelect(std::shared_ptr<SIRInstruction> inst);           // اختيار / Select
    
    // ========================================================================
    // Type Conversion / تحويل الأنواع
    // ========================================================================
    
    /**
     * تحويل نوع Sad إلى نوع LLVM
     * Convert Sad type to LLVM type
     * 
     * @param sadType نوع Sad / Sad type
     * @return نوع LLVM / LLVM type
     */
    llvm::Type* convertType(std::shared_ptr<Type> sadType);
    
    /**
     * تحويل نوع دالة Sad إلى نوع دالة LLVM
     * Convert Sad function type to LLVM function type
     * 
     * @param returnType نوع الرجوع / Return type
     * @param paramTypes أنواع المعاملات / Parameter types
     * @param isVarArg هل الدالة متغيرة المعاملات / Is variadic
     * @return نوع دالة LLVM / LLVM function type
     */
    llvm::FunctionType* convertFunctionType(std::shared_ptr<Type> returnType,
                                           const std::vector<std::shared_ptr<Type>>& paramTypes,
                                           bool isVarArg = false);
    
    /**
     * الحصول على الأنواع الأساسية / Get primitive types
     */
    llvm::Type* getVoidType();                    // نوع فارغ / Void type
    llvm::Type* getInt1Type();                    // منطقي (1 bit) / Boolean
    llvm::Type* getInt8Type();                    // صحيح 8 بت / 8-bit integer
    llvm::Type* getInt16Type();                   // صحيح 16 بت / 16-bit integer
    llvm::Type* getInt32Type();                   // صحيح 32 بت / 32-bit integer
    llvm::Type* getInt64Type();                   // صحيح 64 بت / 64-bit integer
    llvm::Type* getFloatType();                   // عشري 32 بت / 32-bit float
    llvm::Type* getDoubleType();                  // عشري 64 بت / 64-bit double
    llvm::Type* getInt8PtrType();                 // مؤشر حرف / Char pointer (i8*)
    
    // ========================================================================
    // Constants / الثوابت
    // ========================================================================
    
    llvm::Constant* getConstantInt(int64_t value, int bits = 64);
    llvm::Constant* getConstantFloat(double value, bool isDouble = false);
    llvm::Constant* getConstantString(const std::string& value);
    llvm::Constant* getConstantBool(bool value);
    llvm::Constant* getNullPtr(llvm::Type* ptrType);
    
    // ========================================================================
    // Output / الإخراج
    // ========================================================================
    
    /**
     * حفظ LLVM IR إلى ملف نصي
     * Save LLVM IR to text file
     * 
     * @param filename اسم الملف / File name
     * @return true إذا نجح الحفظ / true if save succeeded
     */
    bool emitToFile(const std::string& filename) const;
    
    /**
     * حفظ LLVM IR إلى نص
     * Save LLVM IR to string
     * 
     * @return نص LLVM IR / LLVM IR text
     */
    std::string emitToString() const;
    
    /**
     * إصدار ملف assembly
     * Emit assembly file
     * 
     * @param filename اسم الملف / File name
     * @return true if successful
     */
    bool emitAssembly(const std::string& filename);

    /**
     * إصدار ملف assembly من وحدة خارجية
     * Emit assembly file from external module
     * 
     * @param filename اسم الملف / File name
     * @param module الوحدة / Module
     * @return true if successful
     */
    bool emitAssembly(const std::string& filename, llvm::Module* module);
    
    /**
     * إصدار ملف object
     * Emit object file
     * 
     * @param filename اسم الملف / File name
     * @return true if successful
     */
    bool emitObjectFile(const std::string& filename);

    /**
     * إصدار ملف object من وحدة خارجية
     * Emit object file from external module
     * 
     * @param filename اسم الملف / File name
     * @param module الوحدة / Module
     * @return true if successful
     */
    bool emitObjectFile(const std::string& filename, llvm::Module* module);
    
    /**
     * طباعة LLVM IR إلى stderr
     * Print LLVM IR to stderr
     */
    void dump() const;
    
    // ========================================================================
    // Error Handling / معالجة الأخطاء
    // ========================================================================
    
    /**
     * الإبلاغ عن خطأ
     * Report error
     * 
     * @param message رسالة الخطأ / Error message
     */
    void reportError(const std::string& message);
    
    /**
     * هل حدثت أخطاء؟
     * Has errors occurred?
     * 
     * @return true إذا حدثت أخطاء / true if errors occurred
     */
    bool hasErrors() const { return hasErrors_; }
    
    /**
     * الحصول على رسائل الأخطاء
     * Get error messages
     * 
     * @return قائمة الأخطاء / Error list
     */
    const std::vector<std::string>& getErrors() const { return errors_; }
    
private:
    // ========================================================================
    // Private Members / الأعضاء الخاصة
    // ========================================================================
    
    // LLVM Context (سياق LLVM الرئيسي)
    std::unique_ptr<llvm::LLVMContext> context_;
    
    // LLVM Module (الوحدة)
    std::unique_ptr<llvm::Module> module_;
    
    // LLVM IR Builder (بناء التعليمات)
    std::unique_ptr<llvm::IRBuilder<>> builder_;
    
    // Target Machine (الآلة الهدف)
    llvm::TargetMachine* targetMachine_;
    
    // Code Generation Context (سياق التوليد)
    CodeGenContext context_info_;
    
    // Type Mapper (محول الأنواع) - NEW
    std::unique_ptr<LLVMTypeMapper> typeMapper_;
    
    // Optimizer (محسّن LLVM) - NEW Phase 1.1.3
    std::unique_ptr<sad::LLVMOptimizer> optimizer_;
    
    // SIR Module reference for class info access
    // مرجع وحدة SIR للوصول لمعلومات الأصناف
    std::shared_ptr<SIRModule> sirModule_;
    
    // Optimization settings (إعدادات التحسين)
    sad::OptimizationLevel optimizationLevel_;  // مستوى التحسين / Optimization level
    bool autoOptimize_;                         // تحسين تلقائي / Auto optimize
    
    // Error tracking (تتبع الأخطاء)
    bool hasErrors_;
    std::vector<std::string> errors_;
    
    // ========================================================================
    // Helper Methods / دوال مساعدة
    // ========================================================================
    
    /**
     * الحصول على قيمة متغير
     * Get variable value
     */
    llvm::Value* getNamedValue(const std::string& name);
    
    /**
     * تخزين قيمة متغير
     * Store variable value
     */
    void setNamedValue(const std::string& name, llvm::Value* value);
    
    /**
     * إنشاء entry block alloca لمتغير
     * Create entry block alloca for variable
     */
    llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* function,
                                             const std::string& varName,
                                             llvm::Type* type);
    
    /**
     * الحصول على الكتلة الحالية
     * Get current block
     */
    llvm::BasicBlock* getCurrentBlock() const { return context_info_.currentBlock; }
    
    /**
     * تعيين الكتلة الحالية
     * Set current block
     */
    void setCurrentBlock(llvm::BasicBlock* block) { context_info_.currentBlock = block; }
    
    // ========================================================================
    // القسم 20: دوال Android Runtime
    // Section 20: Android Runtime Functions
    // ========================================================================
    
    /**
     * استدعاء دالة Runtime خارجية لأندرويد
     * Call an external Android runtime function
     * 
     * @param funcName اسم الدالة / Function name
     * @param retType نوع القيمة المُرجعة / Return type
     * @param argTypes أنواع المعاملات / Argument types
     * @param argValues قيم المعاملات / Argument values
     * @return القيمة المُرجعة من الدالة / Return value
     */
    llvm::Value* emitAndroidRuntimeCall(
        const std::string& funcName,
        llvm::Type* retType,
        const std::vector<llvm::Type*>& argTypes,
        const std::vector<llvm::Value*>& argValues);
    
    // 20a. إدارة الذاكرة / Memory Management
    llvm::Value* emitAndroidAlloc(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidFree(std::shared_ptr<SIRInstruction> inst);
    
    // 20b. النصوص / Strings
    llvm::Value* emitAndroidStringCreate(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidStringConcat(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidStringLength(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidStringSubstr(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidStringCompare(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidStringFree(std::shared_ptr<SIRInstruction> inst);
    
    // 20c. المصفوفات / Arrays
    llvm::Value* emitAndroidArrayCreate(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidArrayGet(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidArraySet(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidArrayLength(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidArrayPush(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidArrayPop(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidArrayFree(std::shared_ptr<SIRInstruction> inst);
    
    // 20d. الخرائط / Maps
    llvm::Value* emitAndroidMapCreate(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidMapGet(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidMapSet(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidMapHas(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidMapDelete(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidMapSize(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidMapFree(std::shared_ptr<SIRInstruction> inst);
    
    // 20e. الشبكات / Network
    llvm::Value* emitAndroidNetConnect(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidNetSend(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidNetRecv(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidNetClose(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidHttpRequest(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidWsConnect(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidWsSend(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidWsRecv(std::shared_ptr<SIRInstruction> inst);
    
    // 20f. الخيوط / Threading
    llvm::Value* emitAndroidThreadCreate(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidThreadJoin(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidMutexCreate(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidMutexLock(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidMutexUnlock(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidChannelCreate(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidChannelSend(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidChannelRecv(std::shared_ptr<SIRInstruction> inst);
    
    // 20g. واجهة المستخدم / UI
    llvm::Value* emitAndroidUiInit(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidUiCreateWidget(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidUiSetText(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidUiSetCallback(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidUiShow(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidUiHide(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidUiUpdate(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidUiRun(std::shared_ptr<SIRInstruction> inst);
    
    // 20h. الطباعة والتنقيح / Logging
    llvm::Value* emitAndroidLog(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitAndroidPrint(std::shared_ptr<SIRInstruction> inst);

    // =====================================================================
    // 21. نظام الواجهة الموحد / Unified UI System (sad_ui.h)
    // =====================================================================
    // 21a. مصانع العناصر / Widget Factories
    llvm::Value* emitUiColumn(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiRow(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiStack(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiContainer(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiText(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiTextStyled(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiButton(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiButtonVariant(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiIconButton(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiFab(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiTextField(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiCheckbox(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiSwitch(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiSlider(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiCard(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiScaffold(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiAppBar(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiSpacer(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiDivider(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiDialog(std::shared_ptr<SIRInstruction> inst);
    // 21b. إدارة الشجرة / Tree Management
    llvm::Value* emitUiAddChild(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiRemoveChild(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiClearChildren(std::shared_ptr<SIRInstruction> inst);
    // 21c. ضبط الخصائص / Property Setters
    llvm::Value* emitUiSetText(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiSetSize(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiSetFlex(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiSetBackground(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiSetForeground(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiSetSpacing(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiSetPadding(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiSetAlignment(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiSetBorder(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiSetElevation(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiSetOpacity(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiSetVisibility(std::shared_ptr<SIRInstruction> inst);
    // 21d. إدارة التطبيق / App Management
    llvm::Value* emitUiAppCreate(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiAppSetRoot(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiAppLayout(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiAppRender(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiAppDestroy(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitUiWidgetDestroy(std::shared_ptr<SIRInstruction> inst);
    
    // ================================================================
    // Section 21: التوجيهات / Directives (@حجم, @ذري)
    // (AR) دعم توجيهات اللغة منخفضة المستوى
    // (EN) Support for low-level language directives
    // ================================================================
    llvm::Value* emitSizeof(std::shared_ptr<SIRInstruction> inst);        // @حجم
    llvm::Value* emitAtomicLoad(std::shared_ptr<SIRInstruction> inst);    // @ذري(تحميل)
    llvm::Value* emitAtomicStore(std::shared_ptr<SIRInstruction> inst);   // @ذري(تخزين)
    llvm::Value* emitAtomicAdd(std::shared_ptr<SIRInstruction> inst);     // @ذري(إضافة)
    llvm::Value* emitAtomicSub(std::shared_ptr<SIRInstruction> inst);     // @ذري(طرح)
    llvm::Value* emitAtomicExchange(std::shared_ptr<SIRInstruction> inst);// @ذري(تبادل)
    llvm::Value* emitAtomicCmpXchg(std::shared_ptr<SIRInstruction> inst); // @ذري(مقارنة_وتبديل)
};

} // namespace LLVM
} // namespace Sad

#endif // SAD_LLVM_CODEGEN_H
