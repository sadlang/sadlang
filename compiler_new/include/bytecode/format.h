/**
 * ==========================================================
 * @file format.h
 * @brief تنسيق ملفات Bytecode للغة "ص" / Sad Bytecode File Format
 * المهمة: T270-T277 - نظام Bytecode
 * ==========================================================
 *
 * 📚 شرح للمبتدئين:
 * ================
 *
 * 🤔 ما هو هذا الملف؟
 * -------------------
 * يحدد هذا الملف التنسيق الثنائي لملفات .صكد (Sad Bytecode).
 * يشمل:
 *   - أكواد العمليات (Opcodes) - التعليمات التي تنفذها الآلة الافتراضية
 *   - أنواع الثوابت (ConstantType) - أنواع القيم في جدول الثوابت
 *   - أنواع الرموز (SymbolType) - أنواع الأسماء في جدول الرموز
 *   - هياكل الملف (FileHeader, SectionHeader) - بنية ملف .صكد
 *   - الثوابت السحرية (Magic Number "صكد") - توقيع الملف
 *   - معلومات الإصدار (Version) - ترقيم الإصدارات
 *
 * 📦 بنية ملف .صكد:
 * -----------------
 * ┌─────────────────────────────────────┐
 * │  رأس الملف (FileHeader) - 16 بايت   │
 * │    - Magic: "صكد" (6 بايتات)        │
 * │    - الإصدار (3 بايتات)              │
 * │    - الأعلام (1 بايت)               │
 * │    - عدد الأقسام (2 بايت)            │
 * │    - الحجم الكلي (4 بايتات)          │
 * ├─────────────────────────────────────┤
 * │  رؤوس الأقسام (SectionHeader×N)     │
 * │    - النوع، الموقع، الحجم، Checksum  │
 * ├─────────────────────────────────────┤
 * │  قسم الثوابت (Constant Pool)         │
 * ├─────────────────────────────────────┤
 * │  قسم الكود (Code)                    │
 * ├─────────────────────────────────────┤
 * │  قسم الرموز (Symbol Table)           │
 * └─────────────────────────────────────┘
 *
 * @details
 * (EN) This header defines the binary format for .صكد (Sad Bytecode)
 *      files. It is the single source of truth for all bytecode types,
 *      constants, and structures used by the encoder, decoder, validator,
 *      constant pool, symbol table, magic, and version modules.
 *
 * @author SadLanguage Compiler Team
 * @version 1.0.0
 * @phase Phase 3: Bytecode Backend
 */

#pragma once

#ifndef SAD_BYTECODE_FORMAT_H
#define SAD_BYTECODE_FORMAT_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <array>
#include <vector>

namespace sad {
namespace bytecode {

// ==========================================================
// 📌 ثوابت الإصدار / Version Constants
// ==========================================================

/**
 * 🔢 إصدار Bytecode الحالي: 1.0.0
 *
 * MAJOR: تغييرات غير متوافقة
 * MINOR: ميزات جديدة متوافقة
 * PATCH: إصلاحات أخطاء
 */
constexpr uint8_t VERSION_MAJOR = 1;
constexpr uint8_t VERSION_MINOR = 0;
constexpr uint8_t VERSION_PATCH = 0;

// ==========================================================
// 📌 ثوابت Magic Number / Magic Constants
// ==========================================================

/**
 * 🔮 Magic String: "صكد" بترميز UTF-8
 *
 * ص (Sad)  = 0xD8 0xB5
 * ك (Kaf)  = 0xD9 0x83
 * د (Dal)  = 0xD8 0xAF
 *
 * أول لغة برمجة عربية لها magic number عربي!
 */
inline const char* const MAGIC_STRING = "\xD8\xB5\xD9\x83\xD8\xAF"; // "صكد"

/**
 * 🔢 Magic Bytes كمصفوفة: {0xD8, 0xB5, 0xD9, 0x83, 0xD8, 0xAF}
 *
 * يدعم begin()/end() و[] للتكرار والوصول المباشر
 */
inline constexpr std::array<uint8_t, 6> MAGIC_BYTES = {
    0xD8, 0xB5,   // ص (Sad)
    0xD9, 0x83,   // ك (Kaf)
    0xD8, 0xAF    // د (Dal)
};

/**
 * 📏 حجم Magic Number بالبايتات
 */
constexpr size_t MAGIC_SIZE = 6;

// ==========================================================
// 📌 حدود النظام / System Limits
// ==========================================================

/**
 * 📏 الحد الأقصى لطول النص في جدول الثوابت
 *
 * يُخزَّن الطول كـ uint16_t, لذا الحد الأقصى 65535 بايت
 */
constexpr size_t MAX_STRING_LENGTH = 65535;

/**
 * 📏 الحد الأقصى لحجم جدول الثوابت
 *
 * الفهارس uint16_t, لذا الحد الأقصى 65536 ثابت
 */
constexpr size_t MAX_CONSTANT_POOL_SIZE = 65536;

// ==========================================================
// 📌 أكواد العمليات / Opcodes
// ==========================================================

/**
 * ⚙️ أكواد العمليات لتعليمات Bytecode
 *
 * مُنظَّمة في مجموعات حسب الوظيفة:
 *   0x00-0x0F: عمليات المكدس (Stack)
 *   0x10-0x1C: عمليات حسابية ومنطقية
 *   0x20-0x25: عمليات بتية (Bitwise)
 *   0x28-0x2E: عمليات مقارنة
 *   0x30-0x38: تحكم في التدفق
 *   0x40-0x45: استدعاء الدوال
 *   0x50-0x57: المتغيرات والحقول
 *   0x60-0x6B: إدارة الملكية والذاكرة
 *   0x70-0x77: هياكل البيانات
 *   0x80-0x83: التحويلات
 *   0x90-0x94: الاستثناءات
 *   0xA0-0xA2: التنقيح
 *   0xFE-0xFF: تعليمات خاصة
 *
 * 📊 كل تعليمة بطول متغير (1-9 بايتات):
 *   ┌────────┬───────────────────┐
 *   │ Opcode │ Operands (0-8)    │
 *   │ 1 بايت │ متغير الطول        │
 *   └────────┴───────────────────┘
 */
enum class Opcode : uint8_t {
    // ═══════════════════════════════════════════════════════
    // عمليات المكدس (Stack Operations) — 0x00-0x0F
    // ═══════════════════════════════════════════════════════
    NOP          = 0x00,  ///< لا عملية / No operation
    PUSH_NULL    = 0x01,  ///< ضع قيمة فارغة / Push null
    PUSH_TRUE    = 0x02,  ///< ضع صح / Push true
    PUSH_FALSE   = 0x03,  ///< ضع خطأ / Push false
    PUSH_INT8    = 0x04,  ///< ضع عدد صحيح 8 بت / Push 8-bit int
    PUSH_INT16   = 0x05,  ///< ضع عدد صحيح 16 بت / Push 16-bit int
    PUSH_INT32   = 0x06,  ///< ضع عدد صحيح 32 بت / Push 32-bit int
    PUSH_INT64   = 0x07,  ///< ضع عدد صحيح 64 بت / Push 64-bit int
    PUSH_FLOAT32 = 0x08,  ///< ضع عدد عشري 32 بت / Push 32-bit float
    PUSH_FLOAT64 = 0x09,  ///< ضع عدد عشري 64 بت / Push 64-bit float
    PUSH_CONST   = 0x0A,  ///< ضع ثابت من الجدول / Push constant from pool
    POP          = 0x0B,  ///< أزل من القمة / Pop and discard
    DUP          = 0x0C,  ///< كرر القمة / Duplicate top
    DUP2         = 0x0D,  ///< كرر القمتين / Duplicate top two
    SWAP         = 0x0E,  ///< بدّل القمتين / Swap top two
    ROT          = 0x0F,  ///< دوّر أعلى 3 / Rotate top three

    // ═══════════════════════════════════════════════════════
    // عمليات حسابية (Arithmetic) — 0x10-0x18
    // ═══════════════════════════════════════════════════════
    ADD          = 0x10,  ///< جمع / Add: a + b
    SUB          = 0x11,  ///< طرح / Subtract: a - b
    MUL          = 0x12,  ///< ضرب / Multiply: a * b
    DIV          = 0x13,  ///< قسمة / Divide: a / b
    MOD          = 0x14,  ///< باقي القسمة / Modulo: a % b
    NEG          = 0x15,  ///< سالب / Negate: -a
    INC          = 0x16,  ///< زد واحد / Increment: a + 1
    DEC          = 0x17,  ///< أنقص واحد / Decrement: a - 1
    POW          = 0x18,  ///< أس / Power: a ** b

    // ═══════════════════════════════════════════════════════
    // عمليات منطقية (Logical) — 0x19-0x1C
    // ═══════════════════════════════════════════════════════
    AND          = 0x19,  ///< و منطقي / Logical AND
    OR           = 0x1A,  ///< أو منطقي / Logical OR
    NOT          = 0x1B,  ///< ليس / Logical NOT
    XOR          = 0x1C,  ///< أو حصري منطقي / Logical XOR

    // ═══════════════════════════════════════════════════════
    // عمليات بتية (Bitwise) — 0x20-0x25
    // ═══════════════════════════════════════════════════════
    BIT_AND      = 0x20,  ///< و بتي / Bitwise AND
    BIT_OR       = 0x21,  ///< أو بتي / Bitwise OR
    BIT_XOR      = 0x22,  ///< حصري بتي / Bitwise XOR
    BIT_NOT      = 0x23,  ///< عكس بتي / Bitwise NOT
    SHL          = 0x24,  ///< إزاحة يسار / Shift left
    SHR          = 0x25,  ///< إزاحة يمين / Shift right

    // ═══════════════════════════════════════════════════════
    // عمليات مقارنة (Comparison) — 0x28-0x2E
    // ═══════════════════════════════════════════════════════
    EQ           = 0x28,  ///< يساوي / Equal
    NE           = 0x29,  ///< لا يساوي / Not equal
    LT           = 0x2A,  ///< أصغر / Less than
    LE           = 0x2B,  ///< أصغر أو يساوي / Less or equal
    GT           = 0x2C,  ///< أكبر / Greater than
    GE           = 0x2D,  ///< أكبر أو يساوي / Greater or equal
    CMP          = 0x2E,  ///< مقارنة ثلاثية / Three-way compare

    // ═══════════════════════════════════════════════════════
    // تحكم في التدفق (Control Flow) — 0x30-0x38
    // ═══════════════════════════════════════════════════════
    JMP              = 0x30,  ///< اقفز / Unconditional jump
    JMP_IF           = 0x31,  ///< اقفز إذا صح / Jump if true
    JMP_IF_NOT       = 0x32,  ///< اقفز إذا خطأ / Jump if false
    JMP_IF_NULL      = 0x33,  ///< اقفز إذا فارغ / Jump if null
    JMP_IF_NOT_NULL  = 0x34,  ///< اقفز إذا ليس فارغ / Jump if not null
    LOOP_START       = 0x35,  ///< بداية حلقة / Loop start marker
    LOOP_END         = 0x36,  ///< نهاية حلقة / Loop end marker
    BREAK            = 0x37,  ///< اخرج من الحلقة / Break out of loop
    CONTINUE         = 0x38,  ///< تابع للتكرار التالي / Continue to next iteration

    // ═══════════════════════════════════════════════════════
    // استدعاء الدوال (Function Calls) — 0x40-0x45
    // ═══════════════════════════════════════════════════════
    CALL          = 0x40,  ///< استدعِ دالة / Call function [index:u16, argc:u8]
    CALL_METHOD   = 0x41,  ///< استدعِ طريقة / Call method [index:u16, argc:u8]
    CALL_NATIVE   = 0x42,  ///< استدعِ أصلي / Call native [index:u16, argc:u8]
    CALL_VIRTUAL  = 0x43,  ///< استدعِ افتراضي / Call virtual [index:u16, argc:u8]
    RET           = 0x44,  ///< ارجع (بدون قيمة) / Return void
    RET_VALUE     = 0x45,  ///< ارجع بقيمة / Return with value

    // ═══════════════════════════════════════════════════════
    // المتغيرات والحقول (Variables & Fields) — 0x50-0x57
    // ═══════════════════════════════════════════════════════
    LOAD_LOCAL    = 0x50,  ///< حمّل محلي / Load local [index:u16]
    STORE_LOCAL   = 0x51,  ///< خزّن محلي / Store local [index:u16]
    LOAD_GLOBAL   = 0x52,  ///< حمّل عام / Load global [index:u16]
    STORE_GLOBAL  = 0x53,  ///< خزّن عام / Store global [index:u16]
    LOAD_FIELD    = 0x54,  ///< حمّل حقل / Load field [obj:u16, field:u16]
    STORE_FIELD   = 0x55,  ///< خزّن حقل / Store field [obj:u16, field:u16]
    LOAD_ELEM     = 0x56,  ///< حمّل عنصر / Load element [arr:u16, idx:u16]
    STORE_ELEM    = 0x57,  ///< خزّن عنصر / Store element [arr:u16, idx:u16]

    // ═══════════════════════════════════════════════════════
    // إدارة الملكية والذاكرة (Ownership & Memory) — 0x60-0x6B
    //   (خاصة بلغة ص! Unique to Sad Language!)
    // ═══════════════════════════════════════════════════════
    ALLOC          = 0x60,  ///< خصص ذاكرة / Allocate [type:u16]
    DEALLOC        = 0x61,  ///< حرر ذاكرة / Deallocate
    BORROW         = 0x62,  ///< اقترض للقراءة / Borrow (read-only) [var:u16]
    BORROW_MUT     = 0x63,  ///< اقترض للتعديل / Borrow (mutable) [var:u16]
    END_BORROW     = 0x64,  ///< أنهِ الاقتراض / End borrow
    MOVE           = 0x65,  ///< انقل الملكية / Move ownership [dest:u16]
    COPY           = 0x66,  ///< انسخ القيمة / Copy value
    CLONE          = 0x67,  ///< استنسخ عميق / Deep clone
    DROP           = 0x68,  ///< أسقط/دمّر / Drop/destroy
    CHECK_OWNER    = 0x69,  ///< تحقق المالك / Check owner [var:u16]
    CHECK_BORROW   = 0x6A,  ///< تحقق الاقتراض / Check borrow [var:u16]
    CHECK_LIFETIME = 0x6B,  ///< تحقق فترة الحياة / Check lifetime [var:u16]

    // ═══════════════════════════════════════════════════════
    // هياكل البيانات (Data Structures) — 0x70-0x77
    // ═══════════════════════════════════════════════════════
    NEW_STRUCT   = 0x70,  ///< أنشئ هيكل / New struct [type:u16]
    NEW_ARRAY    = 0x71,  ///< أنشئ مصفوفة / New array [size:u16]
    NEW_MAP      = 0x72,  ///< أنشئ قاموس / New map [cap:u16]
    NEW_STRING   = 0x73,  ///< أنشئ نص / New string [idx:u16]
    ARRAY_LEN    = 0x74,  ///< طول مصفوفة / Array length
    STRING_LEN   = 0x75,  ///< طول نص / String length
    CONCAT       = 0x76,  ///< ادمج / Concatenate
    SLICE        = 0x77,  ///< قطعة / Slice [start:u16, end:u16]

    // ═══════════════════════════════════════════════════════
    // التحويلات (Conversions) — 0x80-0x83
    // ═══════════════════════════════════════════════════════
    CAST         = 0x80,  ///< حوّل نوع / Type cast [type:u16]
    COERCE       = 0x81,  ///< حوّل ضمني / Implicit coercion [type:u16]
    BOX          = 0x82,  ///< غلّف / Box value [type:u16]
    UNBOX        = 0x83,  ///< فك التغليف / Unbox value

    // ═══════════════════════════════════════════════════════
    // الاستثناءات (Exceptions) — 0x90-0x94
    // ═══════════════════════════════════════════════════════
    TRY_START    = 0x90,  ///< بداية حاول / Try block start
    TRY_END      = 0x91,  ///< نهاية حاول / Try block end
    CATCH        = 0x92,  ///< التقط خطأ / Catch exception
    THROW        = 0x93,  ///< ارمِ خطأ / Throw exception
    PANIC        = 0x94,  ///< فزع (خطأ فادح) / Panic (fatal error)

    // ═══════════════════════════════════════════════════════
    // التنقيح (Debug) — 0xA0-0xA2
    // ═══════════════════════════════════════════════════════
    DEBUG_BREAK  = 0xA0,  ///< نقطة توقف / Debug breakpoint
    DEBUG_PRINT  = 0xA1,  ///< طباعة تنقيح / Debug print [var:u16]
    DEBUG_TRACE  = 0xA2,  ///< تتبع / Debug trace [info:u16]

    // ═══════════════════════════════════════════════════════
    // تعليمات خاصة (Special) — 0xFE-0xFF
    // ═══════════════════════════════════════════════════════
    HALT         = 0xFE,  ///< أوقف التنفيذ / Halt execution
    INVALID      = 0xFF   ///< تعليمة غير صالحة / Invalid instruction
};

// ==========================================================
// 📌 أنواع الثوابت / Constant Types
// ==========================================================

/**
 * 📦 أنواع القيم في جدول الثوابت (Constant Pool)
 *
 * كل ثابت في الجدول يبدأ ببايت واحد يحدد نوعه،
 * يليه البيانات حسب النوع.
 */
enum class ConstantType : uint8_t {
    NULL_VALUE    = 0x00,  ///< قيمة فارغة / Null (0 bytes data)
    BOOL          = 0x01,  ///< منطقي / Boolean (1 byte: 0=false, 1=true)
    INT8          = 0x02,  ///< عدد صحيح 8 بت / 8-bit integer (1 byte)
    INT16         = 0x03,  ///< عدد صحيح 16 بت / 16-bit integer (2 bytes LE)
    INT32         = 0x04,  ///< عدد صحيح 32 بت / 32-bit integer (4 bytes LE)
    INT64         = 0x05,  ///< عدد صحيح 64 بت / 64-bit integer (8 bytes LE)
    FLOAT32       = 0x06,  ///< عدد عشري 32 بت / 32-bit float (4 bytes IEEE754)
    FLOAT64       = 0x07,  ///< عدد عشري 64 بت / 64-bit float (8 bytes IEEE754)
    STRING        = 0x10,  ///< نص (ASCII/UTF-8) / String (u16 len + bytes)
    STRING_ARABIC = 0x11   ///< نص عربي (UTF-8) / Arabic string (u16 len + bytes)
};

// ==========================================================
// 📌 أنواع الرموز / Symbol Types
// ==========================================================

/**
 * 🏷️ أنواع الرموز في جدول الرموز
 *
 * كل "اسم" في البرنامج له نوع يحدد طبيعته
 */
enum class SymbolType : uint8_t {
    VARIABLE  = 0x00,  ///< متغير / Variable
    CONSTANT  = 0x01,  ///< ثابت / Constant
    FUNCTION  = 0x02,  ///< دالة / Function
    METHOD    = 0x03,  ///< طريقة / Method
    STRUCT    = 0x04,  ///< هيكل / Struct
    ENUM      = 0x05,  ///< تعداد / Enum
    MODULE    = 0x06,  ///< وحدة / Module
    PARAMETER = 0x07,  ///< معامل / Parameter
    FIELD     = 0x08,  ///< حقل / Field
    GENERIC   = 0x09,  ///< نوع عام / Generic type parameter
    TRAIT     = 0x0A,  ///< سمة / Trait
    IMPL      = 0x0B   ///< تنفيذ سمة / Trait implementation
};

// ==========================================================
// 📌 أعلام الرموز / Symbol Flags
// ==========================================================

/**
 * 🏳️ أعلام خصائص الرموز (bitmask)
 *
 * تُستخدم كأعلام بتية (flags) لوصف خصائص الرمز
 */
enum class SymbolFlags : uint16_t {
    NONE     = 0x0000,  ///< لا أعلام / No flags
    PUBLIC   = 0x0001,  ///< عام (مرئي خارج الوحدة) / Public visibility
    PRIVATE  = 0x0002,  ///< خاص (مخفي) / Private visibility
    MUTABLE  = 0x0004,  ///< قابل للتعديل / Mutable
    STATIC   = 0x0008,  ///< ساكن / Static
    ASYNC    = 0x0010   ///< غير متزامن / Async
};

/**
 * 🔍 فحص وجود علم / Check if flag is set
 *
 * @param flags مجموعة الأعلام
 * @param flag العلم المطلوب فحصه
 * @return true إذا كان العلم مُعيَّناً
 *
 * 📚 الاستخدام:
 *   if (hasFlag(sym.flags, SymbolFlags::PUBLIC)) { ... }
 */
inline bool hasFlag(SymbolFlags flags, SymbolFlags flag) {
    return (static_cast<uint16_t>(flags) & static_cast<uint16_t>(flag)) != 0;
}

// ==========================================================
// 📌 أنواع الأقسام / Section Types
// ==========================================================

/**
 * 📑 أنواع أقسام ملف Bytecode
 *
 * كل ملف .صكد يتكون من أقسام، كل قسم له نوع يحدد محتواه
 */
enum class SectionType : uint8_t {
    CODE           = 0x01,  ///< قسم الكود (التعليمات) / Code section
    CONSTANT_POOL  = 0x02,  ///< جدول الثوابت / Constant pool
    SYMBOL_TABLE   = 0x03,  ///< جدول الرموز / Symbol table
    FUNCTION_TABLE = 0x04,  ///< جدول الدوال / Function table
    DEBUG_INFO     = 0x05,  ///< معلومات التنقيح / Debug information
    METADATA       = 0x06,  ///< بيانات وصفية / Metadata
    CUSTOM         = 0xFF   ///< قسم مخصص / Custom section
};

// ==========================================================
// 📌 أعلام الملف / File Flags
// ==========================================================

/**
 * 🏳️ أعلام ملف Bytecode
 *
 * تُخزَّن في بايت الأعلام في رأس الملف
 */
enum class FileFlags : uint8_t {
    NONE        = 0x00,  ///< لا أعلام / No flags
    DEBUG       = 0x01,  ///< يحتوي معلومات تنقيح / Contains debug info
    OPTIMIZED   = 0x02,  ///< مُحسَّن / Optimized
    COMPRESSED  = 0x04,  ///< مضغوط / Compressed
    EXECUTABLE  = 0x08,  ///< قابل للتنفيذ / Executable
    LIBRARY     = 0x10   ///< مكتبة / Library
};

// ==========================================================
// 📌 هياكل الملف / File Structures
// ==========================================================

#pragma pack(push, 1)

/**
 * 📄 رأس الملف (File Header)
 *
 * أول 16 بايت من كل ملف .صكد
 *
 * ┌──────┬──────┬──────┬──────┬──────────┬──────────┐
 * │magic │ ver  │flags │sects │total_size│          │
 * │6 bytes│3 b  │1 b   │2 bytes│4 bytes   │ = 16 B   │
 * └──────┴──────┴──────┴──────┴──────────┴──────────┘
 */
struct FileHeader {
    uint8_t  magic[6];         ///< Magic Number: "صكد" (0xD8B5 D983 D8AF)
    uint8_t  version_major;    ///< الإصدار الرئيسي / Major version
    uint8_t  version_minor;    ///< الإصدار الفرعي / Minor version
    uint8_t  version_patch;    ///< إصدار التصحيح / Patch version
    uint8_t  flags;            ///< أعلام الملف (FileFlags) / File flags
    uint16_t section_count;    ///< عدد الأقسام / Number of sections
    uint32_t total_size;       ///< الحجم الكلي بالبايتات / Total file size in bytes
};
static_assert(sizeof(FileHeader) == 16, "FileHeader must be 16 bytes");

/**
 * 📑 رأس القسم (Section Header)
 *
 * يصف موقع وحجم كل قسم في الملف
 *
 * ┌──────┬──────┬──────────┬──────────┬────────┬──────────┐
 * │type  │flags │reserved  │ offset   │ size   │ checksum │
 * │1 byte│1 b   │2 bytes   │ 4 bytes  │4 bytes │ 4 bytes  │
 * └──────┴──────┴──────────┴──────────┴────────┴──────────┘
 */
struct SectionHeader {
    SectionType type;          ///< نوع القسم / Section type
    uint8_t     flags;         ///< أعلام القسم / Section flags
    uint16_t    reserved;      ///< محجوز (يجب أن يكون 0) / Reserved (must be 0)
    uint32_t    offset;        ///< إزاحة القسم من بداية الملف / Offset from file start
    uint32_t    size;          ///< حجم القسم بالبايتات / Section size in bytes
    uint32_t    checksum;      ///< CRC32 checksum للتحقق / CRC32 checksum
};
static_assert(sizeof(SectionHeader) == 16, "SectionHeader must be 16 bytes");

#pragma pack(pop)

// ==========================================================
// 📌 معلومات الدوال / Function Info
// ==========================================================

/**
 * 📦 معلومات دالة في جدول الدوال
 *
 * تصف موقع وخصائص كل دالة في الكود المترجم
 */
struct FunctionInfo {
    uint16_t name_index;       ///< فهرس الاسم في جدول الثوابت / Name index in constant pool
    uint32_t code_offset;      ///< إزاحة الكود / Code offset
    uint32_t code_length;      ///< طول الكود بالبايتات / Code length in bytes
    uint16_t param_count;      ///< عدد المعاملات / Parameter count
    uint16_t local_count;      ///< عدد المتغيرات المحلية / Local variable count
    uint16_t stack_size;       ///< أقصى حجم للمكدس / Maximum stack size
    uint8_t  flags;            ///< أعلام (عام، ساكن، ...) / Flags (public, static, ...)
};

// ==========================================================
// 📌 مدخل جدول الرموز / Symbol Entry
// ==========================================================

/**
 * 🏷️ مدخل رمز واحد في الشكل الثنائي
 *
 * الشكل المتسلسل لرمز واحد في ملف .صكد
 */
struct SymbolEntry {
    uint32_t name_index;       ///< فهرس الاسم في جدول الثوابت / Name index in constant pool
    uint8_t  type;             ///< نوع الرمز (SymbolType) / Symbol type
    uint8_t  scope_depth;      ///< عمق النطاق / Scope depth
    uint16_t flags;            ///< الأعلام (SymbolFlags) / Symbol flags
    uint32_t data_type_index;  ///< فهرس نوع البيانات / Data type name index (0xFFFFFFFF = none)
    uint32_t value_offset;     ///< موقع القيمة/الكود / Value or code offset
};

// ==========================================================
// 📌 دوال مساعدة مضمّنة / Inline Helper Functions
// ==========================================================

/**
 * 🔍 التحقق من صلاحية Magic Number (بدون فحص الطول)
 *
 * @param magic مؤشر لأول 6 بايتات من الملف
 * @return true إذا تطابقت مع "صكد"
 *
 * ⚠️ يفترض أن البيانات لا تقل عن 6 بايتات
 */
inline bool isValidMagic(const uint8_t* magic) {
    for (size_t i = 0; i < MAGIC_SIZE; ++i) {
        if (magic[i] != MAGIC_BYTES[i]) {
            return false;
        }
    }
    return true;
}

/**
 * 🤝 هل الإصدار متوافق مع الإصدار الحالي؟
 *
 * @param major الإصدار الرئيسي
 * @param minor الإصدار الفرعي
 * @param patch إصدار التصحيح (لا يؤثر على التوافق)
 * @return true إذا كان الإصدار متوافقاً
 *
 * 📚 قواعد التوافق:
 *   - MAJOR يجب أن يتطابق تماماً
 *   - MINOR يجب أن يكون ≤ الإصدار الحالي
 *   - PATCH لا يؤثر على التوافق
 */
inline bool isCompatibleVersion(uint8_t major, uint8_t minor, uint8_t /*patch*/) {
    if (major != VERSION_MAJOR) return false;
    if (minor > VERSION_MINOR)  return false;
    return true;
}

/**
 * 🔄 تحويل إلى Little-Endian 16 بت
 *
 * على أنظمة x86/x64 و ARM little-endian، هذه عملية مطابقة.
 * تُستخدم لضمان الترتيب الصحيح في الملف الثنائي.
 */
inline uint16_t toLittleEndian16(uint16_t value) {
    return value;  // x86/x64/ARM LE: no-op
}

/**
 * 🔄 تحويل إلى Little-Endian 32 بت
 */
inline uint32_t toLittleEndian32(uint32_t value) {
    return value;  // x86/x64/ARM LE: no-op
}

/**
 * 📝 اسم نوع القسم / Section type name (Arabic)
 *
 * @param type نوع القسم
 * @return الاسم العربي
 */
inline const char* sectionTypeName(SectionType type) {
    switch (type) {
        case SectionType::CODE:           return "كود";
        case SectionType::CONSTANT_POOL:  return "جدول_الثوابت";
        case SectionType::SYMBOL_TABLE:   return "جدول_الرموز";
        case SectionType::FUNCTION_TABLE: return "جدول_الدوال";
        case SectionType::DEBUG_INFO:     return "معلومات_التنقيح";
        case SectionType::METADATA:       return "بيانات_وصفية";
        case SectionType::CUSTOM:         return "مخصص";
        default:                          return "مجهول";
    }
}

/**
 * 📝 اسم نوع الثابت / Constant type name (Arabic)
 *
 * @param type نوع الثابت
 * @return الاسم العربي
 */
inline const char* constantTypeName(ConstantType type) {
    switch (type) {
        case ConstantType::NULL_VALUE:    return "فارغ";
        case ConstantType::BOOL:          return "منطقي";
        case ConstantType::INT8:          return "صحيح٨";
        case ConstantType::INT16:         return "صحيح١٦";
        case ConstantType::INT32:         return "صحيح٣٢";
        case ConstantType::INT64:         return "صحيح٦٤";
        case ConstantType::FLOAT32:       return "عشري٣٢";
        case ConstantType::FLOAT64:       return "عشري٦٤";
        case ConstantType::STRING:        return "نص";
        case ConstantType::STRING_ARABIC: return "نص_عربي";
        default:                          return "مجهول";
    }
}

// ==========================================================
// 📌 تصريحات الدوال / Function Declarations
// ==========================================================
// (مُعرَّفة في magic.cpp)

/**
 * 📝 اسم التعليمة بالعربية / Opcode name in Arabic
 *
 * @param op كود العملية
 * @return الاسم العربي (مثل: "جمع"، "اقفز"، "استدعِ")
 */
const char* opcodeName(Opcode op);

/**
 * 📊 عدد معاملات التعليمة / Opcode operand count
 *
 * @param op كود العملية
 * @return عدد المعاملات (0، 1، أو 2)
 */
uint8_t opcodeArity(Opcode op);

/**
 * 🔍 التحقق من صلاحية Magic Number (مع فحص الطول)
 *
 * @param data البيانات المراد فحصها
 * @param length طول البيانات
 * @return true إذا كان Magic صحيحاً والطول كافٍ
 */
bool validateMagic(const uint8_t* data, size_t length);

/**
 * ✍️ كتابة Magic Number في مخرج
 *
 * @param output مخرج (يجب أن يكون 6 بايتات على الأقل)
 */
void writeMagic(uint8_t* output);

/**
 * 📊 الحصول على Magic كمصفوفة
 *
 * @return vector يحتوي الـ 6 بايتات
 */
std::vector<uint8_t> getMagicBytes();

/**
 * 📝 الحصول على Magic String
 *
 * @return النص العربي "صكد"
 */
const char* getMagicString();

/**
 * 📝 Magic كنص hex
 *
 * @return "D8 B5 D9 83 D8 AF"
 */
std::string getMagicHexString();

/**
 * 📊 الحصول على معلومات الإصدار
 *
 * @param[out] major الإصدار الرئيسي
 * @param[out] minor الإصدار الفرعي
 * @param[out] patch إصدار التصحيح
 */
void getVersion(uint8_t& major, uint8_t& minor, uint8_t& patch);

/**
 * 📝 إصدار كنص / Version as string
 *
 * @return "1.0.0"
 */
std::string getVersionString();

/**
 * 🤝 التحقق من توافق الإصدار
 *
 * @param major الإصدار الرئيسي
 * @param minor الإصدار الفرعي
 * @param patch إصدار التصحيح
 * @return true إذا كان الإصدار متوافقاً
 */
bool checkVersionCompatibility(uint8_t major, uint8_t minor, uint8_t patch);

/**
 * 🔒 حساب checksum (CRC32) للتحقق من سلامة البيانات
 *
 * @param data البيانات
 * @param length الطول
 * @return قيمة CRC32
 */
uint32_t calculateChecksum(const uint8_t* data, size_t length);

// ==========================================================
// 📌 تصريحات أمامية / Forward Declarations
// ==========================================================

// يُستخدم في symbol_table.cpp لتسلسل/فك تسلسل جدول الرموز
class ConstantPool;

} // namespace bytecode
} // namespace sad

#endif // SAD_BYTECODE_FORMAT_H
