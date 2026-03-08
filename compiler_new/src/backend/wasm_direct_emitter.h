/**
 * @file wasm_direct_emitter.h
 * @brief مولد WASM مباشر بدون Emscripten
 * @brief (EN) Direct WASM emitter without Emscripten
 * 
 * @details يحول AST مباشرة إلى WebAssembly bytecode
 * 
 * @author سعيد
 * @date أكتوبر 2026
 */

#ifndef SAD_WASM_DIRECT_EMITTER_H
#define SAD_WASM_DIRECT_EMITTER_H

#include &lt;string&gt;
#include &lt;vector&gt;
#include &lt;map&gt;
#include &lt;memory&gt;
#include &lt;cstdint&gt;

namespace Sad {
namespace Compiler {
namespace WASM {

//==============================================================================
// أنواع WASM
//==============================================================================

enum class ValueType : uint8_t {
    I32 = 0x7F,     // رقم 32-bit
    I64 = 0x7E,     // رقم 64-bit
    F32 = 0x7D,     // عشري 32-bit
    F64 = 0x7C,     // عشري 64-bit
    V128 = 0x7B,    // متجه SIMD
    FUNCREF = 0x70, // مرجع دالة
    EXTERNREF = 0x6F // مرجع خارجي
};

enum class SectionId : uint8_t {
    CUSTOM = 0,
    TYPE = 1,
    IMPORT = 2,
    FUNCTION = 3,
    TABLE = 4,
    MEMORY = 5,
    GLOBAL = 6,
    EXPORT = 7,
    START = 8,
    ELEMENT = 9,
    CODE = 10,
    DATA = 11,
    DATA_COUNT = 12
};

enum class Opcode : uint8_t {
    // تحكم
    UNREACHABLE = 0x00,
    NOP = 0x01,
    BLOCK = 0x02,
    LOOP = 0x03,
    IF = 0x04,
    ELSE = 0x05,
    END = 0x0B,
    BR = 0x0C,
    BR_IF = 0x0D,
    BR_TABLE = 0x0E,
    RETURN = 0x0F,
    CALL = 0x10,
    CALL_INDIRECT = 0x11,
    
    // معاملات
    DROP = 0x1A,
    SELECT = 0x1B,
    
    // متغيرات محلية
    LOCAL_GET = 0x20,
    LOCAL_SET = 0x21,
    LOCAL_TEE = 0x22,
    GLOBAL_GET = 0x23,
    GLOBAL_SET = 0x24,
    
    // ذاكرة
    I32_LOAD = 0x28,
    I64_LOAD = 0x29,
    F32_LOAD = 0x2A,
    F64_LOAD = 0x2B,
    I32_STORE = 0x36,
    I64_STORE = 0x37,
    F32_STORE = 0x38,
    F64_STORE = 0x39,
    MEMORY_SIZE = 0x3F,
    MEMORY_GROW = 0x40,
    
    // ثوابت
    I32_CONST = 0x41,
    I64_CONST = 0x42,
    F32_CONST = 0x43,
    F64_CONST = 0x44,
    
    // مقارنة i32
    I32_EQZ = 0x45,
    I32_EQ = 0x46,
    I32_NE = 0x47,
    I32_LT_S = 0x48,
    I32_LT_U = 0x49,
    I32_GT_S = 0x4A,
    I32_GT_U = 0x4B,
    I32_LE_S = 0x4C,
    I32_LE_U = 0x4D,
    I32_GE_S = 0x4E,
    I32_GE_U = 0x4F,
    
    // حساب i32
    I32_CLZ = 0x67,
    I32_CTZ = 0x68,
    I32_POPCNT = 0x69,
    I32_ADD = 0x6A,
    I32_SUB = 0x6B,
    I32_MUL = 0x6C,
    I32_DIV_S = 0x6D,
    I32_DIV_U = 0x6E,
    I32_REM_S = 0x6F,
    I32_REM_U = 0x70,
    I32_AND = 0x71,
    I32_OR = 0x72,
    I32_XOR = 0x73,
    I32_SHL = 0x74,
    I32_SHR_S = 0x75,
    I32_SHR_U = 0x76,
    I32_ROTL = 0x77,
    I32_ROTR = 0x78,
    
    // حساب f64
    F64_ABS = 0x99,
    F64_NEG = 0x9A,
    F64_CEIL = 0x9B,
    F64_FLOOR = 0x9C,
    F64_TRUNC = 0x9D,
    F64_NEAREST = 0x9E,
    F64_SQRT = 0x9F,
    F64_ADD = 0xA0,
    F64_SUB = 0xA1,
    F64_MUL = 0xA2,
    F64_DIV = 0xA3,
    F64_MIN = 0xA4,
    F64_MAX = 0xA5,
    
    // تحويلات
    I32_WRAP_I64 = 0xA7,
    I32_TRUNC_F64_S = 0xAA,
    I64_EXTEND_I32_S = 0xAC,
    F64_CONVERT_I32_S = 0xB7,
    F64_CONVERT_I64_S = 0xB9,
    I32_REINTERPRET_F32 = 0xBC,
    F32_REINTERPRET_I32 = 0xBE
};

//==============================================================================
// هياكل WASM
//==============================================================================

struct FunctionType {
    std::vector&lt;ValueType&gt; params;
    std::vector&lt;ValueType&gt; results;
};

struct Local {
    uint32_t count;
    ValueType type;
};

struct Function {
    uint32_t typeIndex;
    std::vector&lt;Local&gt; locals;
    std::vector&lt;uint8_t&gt; code;
    std::string name;
};

struct Import {
    std::string module;
    std::string name;
    enum class Kind { FUNC, TABLE, MEMORY, GLOBAL } kind;
    uint32_t typeIndex;
};

struct Export {
    std::string name;
    enum class Kind { FUNC, TABLE, MEMORY, GLOBAL } kind;
    uint32_t index;
};

struct Global {
    ValueType type;
    bool mutable_;
    std::vector&lt;uint8_t&gt; initExpr;
};

struct Memory {
    uint32_t min;
    uint32_t max;
    bool hasMax;
};

struct DataSegment {
    uint32_t memoryIndex;
    std::vector&lt;uint8_t&gt; offsetExpr;
    std::vector&lt;uint8_t&gt; data;
};

//==============================================================================
// مولد WASM
//==============================================================================

/**
 * @brief مولد WebAssembly مباشر
 */
class WASMDirectEmitter {
public:
    WASMDirectEmitter();
    ~WASMDirectEmitter();
    
    //==========================================================================
    // إنشاء الوحدة
    //==========================================================================
    
    /**
     * @brief بدء وحدة جديدة
     */
    void beginModule();
    
    /**
     * @brief إضافة نوع دالة
     */
    uint32_t addFunctionType(const std::vector&lt;ValueType&gt;&amp; params,
                             const std::vector&lt;ValueType&gt;&amp; results);
    
    /**
     * @brief إضافة استيراد
     */
    void addImport(const std::string&amp; module, const std::string&amp; name,
                   Import::Kind kind, uint32_t typeIndex);
    
    /**
     * @brief إضافة دالة
     */
    uint32_t addFunction(uint32_t typeIndex, const std::string&amp; name = "");
    
    /**
     * @brief إضافة تصدير
     */
    void addExport(const std::string&amp; name, Export::Kind kind, uint32_t index);
    
    /**
     * @brief إضافة ذاكرة
     */
    void addMemory(uint32_t minPages, uint32_t maxPages = 0);
    
    /**
     * @brief إضافة متغير عام
     */
    uint32_t addGlobal(ValueType type, bool mutable_, int64_t initValue);
    
    /**
     * @brief إضافة بيانات
     */
    void addDataSegment(uint32_t offset, const std::vector&lt;uint8_t&gt;&amp; data);
    
    /**
     * @brief إضافة بيانات نصية
     */
    uint32_t addStringData(const std::string&amp; str);
    
    //==========================================================================
    // بناء كود الدالة
    //==========================================================================
    
    /**
     * @brief بدء بناء دالة
     */
    void beginFunction(uint32_t funcIndex);
    
    /**
     * @brief إضافة متغير محلي
     */
    uint32_t addLocal(ValueType type);
    
    /**
     * @brief إرسال تعليمة
     */
    void emit(Opcode op);
    void emitI32Const(int32_t value);
    void emitI64Const(int64_t value);
    void emitF64Const(double value);
    void emitLocalGet(uint32_t index);
    void emitLocalSet(uint32_t index);
    void emitGlobalGet(uint32_t index);
    void emitGlobalSet(uint32_t index);
    void emitCall(uint32_t funcIndex);
    void emitIf(ValueType blockType = ValueType::I32);
    void emitElse();
    void emitBlock(ValueType blockType = ValueType::I32);
    void emitLoop(ValueType blockType = ValueType::I32);
    void emitBr(uint32_t depth);
    void emitBrIf(uint32_t depth);
    void emitEnd();
    void emitReturn();
    void emitDrop();
    
    /**
     * @brief إنهاء بناء الدالة
     */
    void endFunction();
    
    //==========================================================================
    // إنتاج الوحدة
    //==========================================================================
    
    /**
     * @brief إنهاء الوحدة وإنتاج البايت كود
     */
    std::vector&lt;uint8_t&gt; finalize();
    
    /**
     * @brief حفظ إلى ملف .wasm
     */
    bool saveToFile(const std::string&amp; path);
    
    /**
     * @brief الحصول على حجم الإخراج المتوقع
     */
    size_t estimatedSize() const;

private:
    class Impl;
    std::unique_ptr&lt;Impl&gt; impl_;
};

//==============================================================================
// مترجم Sad إلى WASM
//==============================================================================

/**
 * @brief مترجم من AST إلى WASM مباشرة
 */
class SadToWASMCompiler {
public:
    SadToWASMCompiler();
    ~SadToWASMCompiler();
    
    /**
     * @brief ترجمة ملف .ص إلى .wasm
     */
    bool compile(const std::string&amp; inputPath, const std::string&amp; outputPath);
    
    /**
     * @brief تفعيل توليد source maps
     */
    void enableSourceMaps(bool enable);
    
    /**
     * @brief تفعيل التحسينات
     */
    void setOptimizationLevel(int level);
    
    /**
     * @brief الحصول على آخر خطأ
     */
    std::string getLastError() const;

private:
    class Impl;
    std::unique_ptr&lt;Impl&gt; impl_;
};

//==============================================================================
// أدوات مساعدة
//==============================================================================

namespace Utils {

/**
 * @brief ترميز LEB128 للأعداد الموجبة
 */
std::vector&lt;uint8_t&gt; encodeULEB128(uint64_t value);

/**
 * @brief ترميز LEB128 للأعداد السالبة
 */
std::vector&lt;uint8_t&gt; encodeSLEB128(int64_t value);

/**
 * @brief ترميز اسم (طول + بايتات)
 */
std::vector&lt;uint8_t&gt; encodeName(const std::string&amp; name);

/**
 * @brief ترميز قسم
 */
std::vector&lt;uint8_t&gt; encodeSection(SectionId id, const std::vector&lt;uint8_t&gt;&amp; content);

} // namespace Utils

} // namespace WASM
} // namespace Compiler
} // namespace Sad

#endif // SAD_WASM_DIRECT_EMITTER_H
