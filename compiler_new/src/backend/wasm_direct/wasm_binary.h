/**
 * @file wasm_binary.h
 * @brief (AR) ثوابت وأنواع تنسيق WebAssembly الثنائي
 * @brief (EN) WebAssembly binary format constants and types
 *
 * يحتوي على تعريفات opcodes وأنواع الأقسام وأنواع القيم
 * لتوليد ملفات .wasm مباشرة بدون LLVM أو Emscripten
 */

#pragma once
#include <cstdint>
#include <vector>
#include <string>

namespace Sad {
namespace Compiler {
namespace WasmDirect {

// ════════════════════════════════════════════════════════════════════════════════
//  WASM Magic & Version
// ════════════════════════════════════════════════════════════════════════════════

constexpr uint8_t WASM_MAGIC[] = {0x00, 0x61, 0x73, 0x6D}; // "\0asm"
constexpr uint8_t WASM_VERSION[] = {0x01, 0x00, 0x00, 0x00}; // version 1

// ════════════════════════════════════════════════════════════════════════════════
//  Section IDs — معرفات الأقسام
// ════════════════════════════════════════════════════════════════════════════════

enum class SectionId : uint8_t {
    Custom   = 0,   // قسم مخصص
    Type     = 1,   // قسم الأنواع (توقيعات الدوال)
    Import   = 2,   // قسم الاستيراد
    Function = 3,   // قسم الدوال (ربط فهرس الدالة بفهرس النوع)
    Table    = 4,   // قسم الجداول
    Memory   = 5,   // قسم الذاكرة
    Global   = 6,   // قسم المتغيرات العامة
    Export   = 7,   // قسم التصدير
    Start    = 8,   // قسم البداية
    Element  = 9,   // قسم العناصر
    Code     = 10,  // قسم الكود (أجسام الدوال)
    Data     = 11,  // قسم البيانات
    DataCount = 12  // عدد مقاطع البيانات
};

// ════════════════════════════════════════════════════════════════════════════════
//  Value Types — أنواع القيم
// ════════════════════════════════════════════════════════════════════════════════

enum class ValType : uint8_t {
    I32    = 0x7F,  // عدد صحيح 32 بت
    I64    = 0x7E,  // عدد صحيح 64 بت
    F32    = 0x7D,  // عدد عشري 32 بت
    F64    = 0x7C,  // عدد عشري 64 بت
    FuncRef = 0x70, // مرجع دالة
    ExternRef = 0x6F // مرجع خارجي
};

constexpr uint8_t FUNC_TYPE_MARKER = 0x60; // علامة نوع الدالة

// ════════════════════════════════════════════════════════════════════════════════
//  Export/Import Kinds — أنواع التصدير/الاستيراد
// ════════════════════════════════════════════════════════════════════════════════

enum class ExportKind : uint8_t {
    Function = 0x00,
    Table    = 0x01,
    Memory   = 0x02,
    Global   = 0x03
};

enum class ImportKind : uint8_t {
    Function = 0x00,
    Table    = 0x01,
    Memory   = 0x02,
    Global   = 0x03
};

// ════════════════════════════════════════════════════════════════════════════════
//  Opcodes — أكواد العمليات
// ════════════════════════════════════════════════════════════════════════════════

namespace Op {
    // ─── التحكم / Control ───
    constexpr uint8_t Unreachable  = 0x00;
    constexpr uint8_t Nop          = 0x01;
    constexpr uint8_t Block        = 0x02;
    constexpr uint8_t Loop         = 0x03;
    constexpr uint8_t If           = 0x04;
    constexpr uint8_t Else         = 0x05;
    constexpr uint8_t End          = 0x0B;
    constexpr uint8_t Br           = 0x0C;
    constexpr uint8_t BrIf         = 0x0D;
    constexpr uint8_t BrTable      = 0x0E;
    constexpr uint8_t Return       = 0x0F;
    constexpr uint8_t Call         = 0x10;
    constexpr uint8_t CallIndirect = 0x11;

    // ─── المراجع / Reference ───
    constexpr uint8_t Drop         = 0x1A;
    constexpr uint8_t Select       = 0x1B;

    // ─── المتغيرات / Variables ───
    constexpr uint8_t LocalGet     = 0x20;
    constexpr uint8_t LocalSet     = 0x21;
    constexpr uint8_t LocalTee     = 0x22;
    constexpr uint8_t GlobalGet    = 0x23;
    constexpr uint8_t GlobalSet    = 0x24;

    // ─── الذاكرة / Memory (i32) ───
    constexpr uint8_t I32Load      = 0x28;
    constexpr uint8_t I64Load      = 0x29;
    constexpr uint8_t F32Load      = 0x2A;
    constexpr uint8_t F64Load      = 0x2B;
    constexpr uint8_t I32Load8S    = 0x2C;
    constexpr uint8_t I32Load8U    = 0x2D;
    constexpr uint8_t I32Load16S   = 0x2E;
    constexpr uint8_t I32Load16U   = 0x2F;
    constexpr uint8_t I32Store     = 0x36;
    constexpr uint8_t I64Store     = 0x37;
    constexpr uint8_t F32Store     = 0x38;
    constexpr uint8_t F64Store     = 0x39;
    constexpr uint8_t I32Store8    = 0x3A;
    constexpr uint8_t I32Store16   = 0x3B;
    constexpr uint8_t MemorySize   = 0x3F;
    constexpr uint8_t MemoryGrow   = 0x40;

    // ─── الثوابت / Constants ───
    constexpr uint8_t I32Const     = 0x41;
    constexpr uint8_t I64Const     = 0x42;
    constexpr uint8_t F32Const     = 0x43;
    constexpr uint8_t F64Const     = 0x44;

    // ─── المقارنة i32 / Comparison ───
    constexpr uint8_t I32Eqz       = 0x45;
    constexpr uint8_t I32Eq        = 0x46;
    constexpr uint8_t I32Ne        = 0x47;
    constexpr uint8_t I32LtS       = 0x48;
    constexpr uint8_t I32LtU       = 0x49;
    constexpr uint8_t I32GtS       = 0x4A;
    constexpr uint8_t I32GtU       = 0x4B;
    constexpr uint8_t I32LeS       = 0x4C;
    constexpr uint8_t I32LeU       = 0x4D;
    constexpr uint8_t I32GeS       = 0x4E;
    constexpr uint8_t I32GeU       = 0x4F;

    // ─── المقارنة f64 / Comparison ───
    constexpr uint8_t F64Eq        = 0x61;
    constexpr uint8_t F64Ne        = 0x62;
    constexpr uint8_t F64Lt        = 0x63;
    constexpr uint8_t F64Gt        = 0x64;
    constexpr uint8_t F64Le        = 0x65;
    constexpr uint8_t F64Ge        = 0x66;

    // ─── الحسابية i32 / Arithmetic ───
    constexpr uint8_t I32Add       = 0x6A;
    constexpr uint8_t I32Sub       = 0x6B;
    constexpr uint8_t I32Mul       = 0x6C;
    constexpr uint8_t I32DivS      = 0x6D;
    constexpr uint8_t I32DivU      = 0x6E;
    constexpr uint8_t I32RemS      = 0x6F;
    constexpr uint8_t I32RemU      = 0x70;
    constexpr uint8_t I32And       = 0x71;
    constexpr uint8_t I32Or        = 0x72;
    constexpr uint8_t I32Xor       = 0x73;
    constexpr uint8_t I32Shl       = 0x74;
    constexpr uint8_t I32ShrS      = 0x75;
    constexpr uint8_t I32ShrU      = 0x76;
    constexpr uint8_t I32Rotl      = 0x77;
    constexpr uint8_t I32Rotr      = 0x78;

    // ─── الحسابية f64 / Arithmetic ───
    constexpr uint8_t F64Abs       = 0x99;
    constexpr uint8_t F64Neg       = 0x9A;
    constexpr uint8_t F64Ceil      = 0x9B;
    constexpr uint8_t F64Floor     = 0x9C;
    constexpr uint8_t F64Sqrt      = 0x9F;
    constexpr uint8_t F64Add       = 0xA0;
    constexpr uint8_t F64Sub       = 0xA1;
    constexpr uint8_t F64Mul       = 0xA2;
    constexpr uint8_t F64Div       = 0xA3;
    constexpr uint8_t F64Min       = 0xA4;
    constexpr uint8_t F64Max       = 0xA5;

    // ─── التحويل / Conversion ───
    constexpr uint8_t I32WrapI64      = 0xA7;
    constexpr uint8_t I32TruncF64S    = 0xAA;
    constexpr uint8_t I64ExtendI32S   = 0xAC;
    constexpr uint8_t F64ConvertI32S  = 0xB7;
    constexpr uint8_t F64ConvertI64S  = 0xB9;
    constexpr uint8_t I32ReinterpretF32 = 0xBC;
    constexpr uint8_t F32ReinterpretI32 = 0xBE;

    // ─── كتلة فارغة / Block void ───
    constexpr uint8_t BlockTypeVoid = 0x40;
} // namespace Op

// ════════════════════════════════════════════════════════════════════════════════
//  LEB128 Encoding — ترميز الأعداد المتغيرة الطول
// ════════════════════════════════════════════════════════════════════════════════

/**
 * ترميز عدد بدون إشارة بصيغة LEB128
 */
inline void encodeLEB128U(std::vector<uint8_t>& out, uint32_t value) {
    do {
        uint8_t byte = value & 0x7F;
        value >>= 7;
        if (value != 0) byte |= 0x80;
        out.push_back(byte);
    } while (value != 0);
}

/**
 * ترميز عدد بإشارة بصيغة LEB128
 */
inline void encodeLEB128S(std::vector<uint8_t>& out, int32_t value) {
    bool more = true;
    while (more) {
        uint8_t byte = value & 0x7F;
        value >>= 7;
        if ((value == 0 && (byte & 0x40) == 0) ||
            (value == -1 && (byte & 0x40) != 0)) {
            more = false;
        } else {
            byte |= 0x80;
        }
        out.push_back(byte);
    }
}

/**
 * ترميز عدد بإشارة 64 بت بصيغة LEB128
 */
inline void encodeLEB128S64(std::vector<uint8_t>& out, int64_t value) {
    bool more = true;
    while (more) {
        uint8_t byte = value & 0x7F;
        value >>= 7;
        if ((value == 0 && (byte & 0x40) == 0) ||
            (value == -1 && (byte & 0x40) != 0)) {
            more = false;
        } else {
            byte |= 0x80;
        }
        out.push_back(byte);
    }
}

/**
 * كتابة نص UTF-8 مع طول LEB128
 */
inline void encodeString(std::vector<uint8_t>& out, const std::string& str) {
    encodeLEB128U(out, static_cast<uint32_t>(str.size()));
    out.insert(out.end(), str.begin(), str.end());
}

/**
 * كتابة عدد عشري 64 بت (IEEE 754)
 */
inline void encodeF64(std::vector<uint8_t>& out, double value) {
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
    out.insert(out.end(), bytes, bytes + 8);
}

/**
 * كتابة عدد عشري 32 بت (IEEE 754)
 */
inline void encodeF32(std::vector<uint8_t>& out, float value) {
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
    out.insert(out.end(), bytes, bytes + 4);
}

// ════════════════════════════════════════════════════════════════════════════════
//  Data Structures — هياكل البيانات
// ════════════════════════════════════════════════════════════════════════════════

/**
 * نوع دالة WASM (توقيع)
 */
struct WasmFuncType {
    std::vector<ValType> params;    // أنواع المدخلات
    std::vector<ValType> results;   // أنواع المخرجات
    
    bool operator==(const WasmFuncType& other) const {
        return params == other.params && results == other.results;
    }
};

/**
 * استيراد WASM
 */
struct WasmImport {
    std::string moduleName;     // اسم الوحدة (مثال: "env")
    std::string fieldName;      // اسم الحقل
    ImportKind kind;
    uint32_t typeIndex;         // فهرس النوع (للدوال)
};

/**
 * تصدير WASM
 */
struct WasmExport {
    std::string name;           // اسم التصدير
    ExportKind kind;
    uint32_t index;             // فهرس العنصر
};

/**
 * متغير عام WASM
 */
struct WasmGlobal {
    ValType type;
    bool mutable_;
    std::vector<uint8_t> initExpr; // تعبير التهيئة
};

/**
 * مقطع بيانات WASM
 */
struct WasmDataSegment {
    uint32_t memoryIndex = 0;
    uint32_t offset;            // عنوان البداية
    std::vector<uint8_t> data;  // البيانات
};

/**
 * متغير محلي في دالة
 */
struct WasmLocal {
    uint32_t count;
    ValType type;
};

/**
 * جسم دالة WASM
 */
struct WasmFuncBody {
    std::vector<WasmLocal> locals;  // المتغيرات المحلية
    std::vector<uint8_t> code;      // أكواد العمليات
};

} // namespace WasmDirect
} // namespace Compiler
} // namespace Sad
