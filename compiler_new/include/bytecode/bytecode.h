/**
 * @file bytecode.h
 * @brief حاوية البايت كود / Bytecode Container
 * @brief Container for compiled bytecode and metadata
 * 
 * @details
 * (AR) يحتوي هذا الملف على الهياكل اللازمة لتخزين البايت كود المترجم
 *      بما في ذلك مجمعات الثوابت، معلومات الدوال، وجداول الرموز.
 * 
 * (EN) This file contains structures for storing compiled bytecode
 *      including constant pools, function info, and symbol tables.
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 * @phase Phase 3: Bytecode Backend
 */

#pragma once

#include "opcodes.h"
#include "value.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

namespace Sad {
namespace Compiler {
namespace Bytecode {

// ============================================================================
// استيراد نوع Byte من الآلة الافتراضية / Import Byte type from VM namespace
// (AR) نوع Byte مُعرَّف في Sad::VM (opcodes.h) لكن هذا الملف في
//      Sad::Compiler::Bytecode — نحتاج استيراده صراحةً.
// (EN) Byte is defined in Sad::VM (opcodes.h) but this file is in
//      Sad::Compiler::Bytecode — we need to import it explicitly.
// ============================================================================
using Sad::VM::Byte;

// ========================================
// Constant Pool
// مجمع الثوابت
// ========================================

/**
 * @brief مجمع الثوابت / Constant Pool
 * @brief Stores constant values used in bytecode
 * 
 * @details
 * (AR) يخزن جميع الثوابت المستخدمة في البرنامج بكفاءة.
 *      يتم تخزين كل ثابت مرة واحدة فقط ويُشار إليه بفهرس.
 * 
 * (EN) Efficiently stores all constants used in program.
 *      Each constant stored once and referenced by index.
 */
class ConstantPool {
public:
    /**
     * @brief إضافة عدد صحيح / Add integer constant
     * @return Index in pool
     */
    uint32_t addInteger(int64_t value);
    
    /**
     * @brief إضافة عدد عشري / Add float constant
     * @return Index in pool
     */
    uint32_t addFloat(double value);
    
    /**
     * @brief إضافة نص / Add string constant
     * @return Index in pool
     */
    uint32_t addString(const std::string& str);
    
    /**
     * @brief الحصول على ثابت / Get constant
     */
    VM::Value getConstant(uint32_t index) const;
    
    /**
     * @brief عدد الثوابت / Get count
     */
    size_t size() const { return constants_.size(); }
    
    /**
     * @brief الحصول على جميع الثوابت / Get all constants
     */
    const std::vector<VM::Value>& getConstants() const { return constants_; }
    
    /**
     * @brief مسح المجمع / Clear pool
     */
    void clear();

private:
    std::vector<VM::Value> constants_;                    ///< الثوابت / Constants
    std::unordered_map<int64_t, uint32_t> intMap_;       ///< خريطة الأعداد / Int map
    std::unordered_map<double, uint32_t> floatMap_;      ///< خريطة الأعداد العشرية / Float map
    std::unordered_map<std::string, uint32_t> stringMap_; ///< خريطة النصوص / String map
};

// ========================================
// Line Info
// معلومات السطر
// ========================================

/**
 * @brief معلومات السطر / Line Information
 * @brief Maps bytecode offset to source line number
 */
struct LineInfo {
    uint32_t offset;       ///< الإزاحة في البايت كود / Bytecode offset
    uint32_t lineNumber;   ///< رقم السطر / Source line number
    
    LineInfo() : offset(0), lineNumber(0) {}
    LineInfo(uint32_t off, uint32_t line) : offset(off), lineNumber(line) {}
};

// ========================================
// Function Info
// معلومات الدالة
// ========================================

/**
 * @brief معلومات الدالة / Function Information
 * @brief Metadata for a compiled function
 */
struct FunctionInfo {
    std::string name;              ///< اسم الدالة / Function name
    uint32_t arity;                ///< عدد المعاملات / Parameter count
    uint32_t localCount;           ///< عدد المتغيرات المحلية / Local variable count
    uint32_t codeOffset;           ///< إزاحة الكود / Code offset in bytecode
    uint32_t codeSize;             ///< حجم الكود / Code size in bytes
    std::vector<LineInfo> lineInfo; ///< معلومات الأسطر / Line information
    
    FunctionInfo() : arity(0), localCount(0), codeOffset(0), codeSize(0) {}
};

// ========================================
// Global Variable Info
// معلومات المتغير العام
// ========================================

/**
 * @brief متغير عام / Global Variable
 * @brief Information about a global variable
 */
struct GlobalInfo {
    std::string name;              ///< الاسم / Name
    uint32_t index;                ///< الفهرس / Index
    VM::Value initialValue;        ///< القيمة الابتدائية / Initial value
    
    GlobalInfo() : index(0) {}
};

// ========================================
// Debug Info
// معلومات التنقيح
// ========================================

/**
 * @brief معلومات التنقيح / Debug Information
 * @brief Complete debugging information for a module
 * 
 * @details
 * (AR) يحتوي على معلومات مفصلة لمساعدة المنقح (debugger)
 *      في تتبع التنفيذ وربط البايت كود بالكود المصدري.
 * 
 * (EN) Contains detailed information to help debuggers
 *      trace execution and map bytecode to source code.
 */
struct DebugInfo {
    std::string sourceFile;                     ///< ملف المصدر / Source file path
    std::vector<LineInfo> lineInfo;             ///< معلومات الأسطر / Line information
    std::unordered_map<std::string, uint32_t> variableMap; ///< خريطة المتغيرات / Variable map
    std::vector<std::string> localNames;        ///< أسماء المتغيرات المحلية / Local names
    
    DebugInfo() = default;
    DebugInfo(const std::string& file) : sourceFile(file) {}
};

// ========================================
// Bytecode Module
// وحدة البايت كود
// ========================================

/**
 * @brief وحدة البايت كود / Bytecode Module
 * @brief Complete compiled module with all metadata
 * 
 * @details
 * (AR) يحتوي على البايت كود الكامل لبرنامج أو وحدة،
 *      بما في ذلك الثوابت، الدوال، والمتغيرات العامة.
 * 
 * (EN) Contains complete bytecode for a program or module,
 *      including constants, functions, and global variables.
 */
class BytecodeModule {
public:
    /**
     * @brief Constructor / البناء
     */
    BytecodeModule(const std::string& name = "");
    
    /**
     * @brief إضافة بايت / Add byte
     */
    void emitByte(Byte byte);
    
    /**
     * @brief إضافة عدة بايتات / Add multiple bytes
     */
    void emitBytes(const std::vector<Byte>& bytes);
    
    /**
     * @brief إضافة عدد 16 بت / Add 16-bit integer
     */
    void emitU16(uint16_t value);
    
    /**
     * @brief إضافة عدد 32 بت / Add 32-bit integer
     */
    void emitU32(uint32_t value);
    
    /**
     * @brief إضافة عدد 64 بت / Add 64-bit integer
     */
    void emitU64(uint64_t value);
    
    /**
     * @brief إضافة عدد عشري 64 بت / Add 64-bit float
     */
    void emitF64(double value);
    
    /**
     * @brief الحصول على الموضع الحالي / Get current position
     */
    uint32_t getCurrentOffset() const {
        return static_cast<uint32_t>(code_.size());
    }
    
    /**
     * @brief تصحيح بايت في موضع معين / Patch byte at position
     */
    void patchByte(uint32_t offset, Byte byte);
    
    /**
     * @brief تصحيح عدد 32 بت / Patch 32-bit integer
     */
    void patchU32(uint32_t offset, uint32_t value);
    
    /**
     * @brief إضافة معلومات سطر / Add line info
     */
    void addLineInfo(uint32_t lineNumber);
    
    /**
     * @brief إضافة دالة / Add function
     */
    void addFunction(const FunctionInfo& func);
    
    /**
     * @brief إضافة متغير عام / Add global variable
     */
    void addGlobal(const GlobalInfo& global);
    
    /**
     * @brief الوصول إلى مجمع الثوابت / Access constant pool
     */
    ConstantPool& getConstantPool() { return constantPool_; }
    const ConstantPool& getConstantPool() const { return constantPool_; }
    
    /**
     * @brief الحصول على الكود / Get code
     */
    const std::vector<Byte>& getCode() const { return code_; }
    
    /**
     * @brief الحصول على الدوال / Get functions
     */
    const std::vector<FunctionInfo>& getFunctions() const { return functions_; }
    
    /**
     * @brief الحصول على المتغيرات العامة / Get globals
     */
    const std::vector<GlobalInfo>& getGlobals() const { return globals_; }
    
    /**
     * @brief الحصول على الاسم / Get name
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief تعيين نقطة الدخول / Set entry point
     */
    void setEntryPoint(uint32_t offset) { entryPoint_ = offset; }
    
    /**
     * @brief الحصول على نقطة الدخول / Get entry point
     */
    uint32_t getEntryPoint() const { return entryPoint_; }
    
    /**
     * @brief طباعة معلومات تفصيلية / Print disassembly
     */
    void disassemble() const;
    
    /**
     * @brief مسح الوحدة / Clear module
     */
    void clear();
    
    /**
     * @brief البحث عن فهرس دالة بالاسم / Find function index by name
     * @param name اسم الدالة / Function name
     * @return فهرس الدالة أو -1 / Function index or -1 if not found
     */
    int getFunctionIndex(const std::string& name) const;
    
    /**
     * @brief البحث عن رقم دالة مدمجة بالاسم / Find builtin ID by name
     * @param name اسم الدالة المدمجة / Builtin function name
     * @return رقم الدالة المدمجة أو -1 / Builtin ID or -1 if not found
     */
    int getBuiltinId(const std::string& name) const;
    
    /**
     * @brief تسجيل دالة مدمجة / Register builtin function
     * @param name اسم الدالة / Builtin name
     * @param id رقم الدالة / Builtin ID
     */
    void registerBuiltin(const std::string& name, uint32_t id);

private:
    std::string name_;                    ///< اسم الوحدة / Module name
    std::vector<Byte> code_;              ///< البايت كود / Bytecode
    ConstantPool constantPool_;           ///< مجمع الثوابت / Constant pool
    std::vector<FunctionInfo> functions_; ///< الدوال / Functions
    std::vector<GlobalInfo> globals_;     ///< المتغيرات العامة / Globals
    std::vector<LineInfo> lineInfo_;      ///< معلومات الأسطر / Line info
    uint32_t entryPoint_;                 ///< نقطة الدخول / Entry point
    std::unordered_map<std::string, uint32_t> builtinMap_; ///< خريطة الدوال المدمجة / Builtin map
};

// ========================================
// Disassembler Utilities
// أدوات التفكيك
// ========================================

/**
 * @brief تفكيك تعليمة / Disassemble instruction
 * @brief Print human-readable representation of instruction
 * 
 * @param code البايت كود / Bytecode
 * @param offset الإزاحة / Offset
 * @param constantPool مجمع الثوابت / Constant pool
 * @return حجم التعليمة / Instruction size
 */
int disassembleInstruction(
    const std::vector<Byte>& code,
    uint32_t offset,
    const ConstantPool& constantPool
);

/**
 * @brief تفكيك وحدة كاملة / Disassemble complete module
 */
void disassembleModule(const BytecodeModule& module);

} // namespace Bytecode
} // namespace Compiler
} // namespace Sad
