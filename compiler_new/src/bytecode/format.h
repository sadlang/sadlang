/**
 * ==========================================================
 * ملف: format.h
 * الوصف: مواصفات تنسيق ملفات Bytecode للغة "ص"
 * المهمة: T269 - نظام Bytecode
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * هذا الملف يحدد "التنسيق" أو "الشكل" الذي تُخزَّن به البرامج
 * المترجمة بلغة "ص".
 * 
 * 🤔 ما هو Bytecode؟
 * ------------------
 * عندما تكتب برنامجاً بلغة "ص"، يمر بعدة مراحل:
 * 
 * 1️⃣ الكود المصدري (ما تكتبه):
 *    دالة مرحبا():
 *        اطبع("السلام عليكم")
 *    نهاية
 * 
 * 2️⃣ AST (شجرة الصياغة):
 *    تمثيل هيكلي للكود
 * 
 * 3️⃣ SIR (التمثيل الوسيط):
 *    تحليل الملكية والاقتراض
 * 
 * 4️⃣ Bytecode (الكود الآلي الوسيط):  ← نحن هنا! 🎯
 *    تعليمات يفهمها الحاسوب الافتراضي
 * 
 * 📦 بنية ملف .صكد:
 * -----------------
 * كل ملف bytecode يتكون من أقسام:
 * 
 * ┌─────────────────────────────────────┐
 * │  Magic Number (صكد)                 │ ← للتحقق من نوع الملف
 * ├─────────────────────────────────────┤
 * │  Version (1.0.0)                    │ ← إصدار التنسيق
 * ├─────────────────────────────────────┤
 * │  Constant Pool (جدول الثوابت)        │ ← النصوص والأرقام
 * ├─────────────────────────────────────┤
 * │  Symbol Table (جدول الرموز)          │ ← أسماء الدوال والمتغيرات
 * ├─────────────────────────────────────┤
 * │  Instructions (التعليمات)            │ ← الأوامر الفعلية
 * └─────────────────────────────────────┘
 * 
 * 🔢 الـ Magic Number:
 * -------------------
 * "Magic Number" هو رقم خاص في بداية الملف يُعرِّف نوعه.
 * في لغة "ص"، استخدمنا الحروف العربية "صكد" كـ magic number!
 * - ص = 0xD5 0x35 (UTF-8)
 * - ك = 0xD9 0x83 (UTF-8)
 * - د = 0xD8 0xAF (UTF-8)
 * 
 * 🎯 لماذا نستخدم Bytecode؟
 * -------------------------
 * 1. السرعة: أسرع من تفسير الكود المصدري مباشرة
 * 2. الأمان: لا يمكن قراءة الكود الأصلي بسهولة
 * 3. الحجم: أصغر من الكود المصدري
 * 4. التوافق: يعمل على أي نظام به مفسر "ص"
 */

#ifndef SAD_BYTECODE_FORMAT_H
#define SAD_BYTECODE_FORMAT_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <array>

namespace sad {
namespace bytecode {

// ==========================================================
// 📌 الثوابت الأساسية
// ==========================================================

/**
 * 🔢 Magic Number للغة "ص"
 * 
 * الحروف "صكد" بترميز UTF-8:
 * - ص (Sad) = 0xD8 0xB5
 * - ك (Kaf) = 0xD9 0x83  
 * - د (Dal) = 0xD8 0xAF
 */
constexpr uint8_t MAGIC_BYTE_0 = 0xD8;  // أول بايت من "ص"
constexpr uint8_t MAGIC_BYTE_1 = 0xB5;  // ثاني بايت من "ص"
constexpr uint8_t MAGIC_BYTE_2 = 0xD9;  // أول بايت من "ك"
constexpr uint8_t MAGIC_BYTE_3 = 0x83;  // ثاني بايت من "ك"
constexpr uint8_t MAGIC_BYTE_4 = 0xD8;  // أول بايت من "د"
constexpr uint8_t MAGIC_BYTE_5 = 0xAF;  // ثاني بايت من "د"

// Magic كـ array لسهولة المقارنة
constexpr std::array<uint8_t, 6> MAGIC_BYTES = {
    MAGIC_BYTE_0, MAGIC_BYTE_1,  // ص
    MAGIC_BYTE_2, MAGIC_BYTE_3,  // ك
    MAGIC_BYTE_4, MAGIC_BYTE_5   // د
};

// Magic كـ string للعرض
constexpr const char* MAGIC_STRING = "صكد";

/**
 * 📊 إصدار تنسيق Bytecode
 * 
 * نستخدم Semantic Versioning:
 * - MAJOR: تغييرات غير متوافقة
 * - MINOR: إضافات متوافقة
 * - PATCH: إصلاحات
 */
constexpr uint8_t VERSION_MAJOR = 1;
constexpr uint8_t VERSION_MINOR = 0;
constexpr uint8_t VERSION_PATCH = 0;

// الحجم الأقصى لكل قسم (للأمان)
constexpr size_t MAX_CONSTANT_POOL_SIZE = 65536;    // 64KB
constexpr size_t MAX_SYMBOL_TABLE_SIZE = 65536;     // 64KB
constexpr size_t MAX_CODE_SIZE = 16777216;          // 16MB
constexpr size_t MAX_STRING_LENGTH = 65535;         // أقصى طول نص

// ==========================================================
// 📌 أنواع الأقسام (Section Types)
// ==========================================================

/**
 * 🏷️ نوع_القسم - أنواع أقسام الملف
 * 
 * كل ملف bytecode مقسم إلى أقسام، ولكل قسم نوع
 */
enum class SectionType : uint8_t {
    // أقسام أساسية
    HEADER = 0x00,          // رأس الملف (magic + version)
    CONSTANT_POOL = 0x01,   // جدول الثوابت
    SYMBOL_TABLE = 0x02,    // جدول الرموز
    CODE = 0x03,            // التعليمات
    
    // أقسام إضافية
    DEBUG_INFO = 0x10,      // معلومات التنقيح
    SOURCE_MAP = 0x11,      // ربط مع الكود المصدري
    METADATA = 0x12,        // بيانات وصفية
    
    // أقسام الملكية (خاصة بلغة ص)
    OWNERSHIP_INFO = 0x20,  // معلومات الملكية
    LIFETIME_INFO = 0x21,   // معلومات فترات الحياة
    BORROW_INFO = 0x22,     // معلومات الاقتراض
    
    // للتوسع المستقبلي
    CUSTOM = 0xFF           // قسم مخصص
};

/**
 * 📝 وصف نوع القسم بالعربية
 */
inline const char* sectionTypeName(SectionType type) {
    switch (type) {
        case SectionType::HEADER: return "رأس الملف";
        case SectionType::CONSTANT_POOL: return "جدول الثوابت";
        case SectionType::SYMBOL_TABLE: return "جدول الرموز";
        case SectionType::CODE: return "التعليمات";
        case SectionType::DEBUG_INFO: return "معلومات التنقيح";
        case SectionType::SOURCE_MAP: return "خريطة المصدر";
        case SectionType::METADATA: return "البيانات الوصفية";
        case SectionType::OWNERSHIP_INFO: return "معلومات الملكية";
        case SectionType::LIFETIME_INFO: return "معلومات فترات الحياة";
        case SectionType::BORROW_INFO: return "معلومات الاقتراض";
        case SectionType::CUSTOM: return "قسم مخصص";
        default: return "غير معروف";
    }
}

// ==========================================================
// 📌 أنواع الثوابت (Constant Types)
// ==========================================================

/**
 * 🔢 نوع_الثابت - أنواع القيم في جدول الثوابت
 */
enum class ConstantType : uint8_t {
    // أنواع أساسية
    NULL_VALUE = 0x00,      // قيمة فارغة
    BOOL = 0x01,            // منطقي (صح/خطأ)
    INT8 = 0x02,            // عدد صحيح 8 بت
    INT16 = 0x03,           // عدد صحيح 16 بت
    INT32 = 0x04,           // عدد صحيح 32 بت
    INT64 = 0x05,           // عدد صحيح 64 بت
    UINT8 = 0x06,           // عدد طبيعي 8 بت
    UINT16 = 0x07,          // عدد طبيعي 16 بت
    UINT32 = 0x08,          // عدد طبيعي 32 بت
    UINT64 = 0x09,          // عدد طبيعي 64 بت
    FLOAT32 = 0x0A,         // عدد عشري 32 بت
    FLOAT64 = 0x0B,         // عدد عشري 64 بت
    
    // أنواع مركبة
    STRING = 0x10,          // نص (UTF-8)
    STRING_ARABIC = 0x11,   // نص عربي (تحسين خاص)
    BYTES = 0x12,           // بايتات خام
    
    // مراجع
    SYMBOL_REF = 0x20,      // مرجع لرمز
    TYPE_REF = 0x21,        // مرجع لنوع
    FUNCTION_REF = 0x22     // مرجع لدالة
};

/**
 * 📝 وصف نوع الثابت بالعربية
 */
inline const char* constantTypeName(ConstantType type) {
    switch (type) {
        case ConstantType::NULL_VALUE: return "فارغ";
        case ConstantType::BOOL: return "منطقي";
        case ConstantType::INT8: return "صحيح٨";
        case ConstantType::INT16: return "صحيح١٦";
        case ConstantType::INT32: return "صحيح٣٢";
        case ConstantType::INT64: return "صحيح٦٤";
        case ConstantType::UINT8: return "طبيعي٨";
        case ConstantType::UINT16: return "طبيعي١٦";
        case ConstantType::UINT32: return "طبيعي٣٢";
        case ConstantType::UINT64: return "طبيعي٦٤";
        case ConstantType::FLOAT32: return "عشري٣٢";
        case ConstantType::FLOAT64: return "عشري٦٤";
        case ConstantType::STRING: return "نص";
        case ConstantType::STRING_ARABIC: return "نص_عربي";
        case ConstantType::BYTES: return "بايتات";
        case ConstantType::SYMBOL_REF: return "مرجع_رمز";
        case ConstantType::TYPE_REF: return "مرجع_نوع";
        case ConstantType::FUNCTION_REF: return "مرجع_دالة";
        default: return "غير معروف";
    }
}

// ==========================================================
// 📌 أنواع الرموز (Symbol Types)
// ==========================================================

/**
 * 🏷️ نوع_الرمز - أنواع الرموز في جدول الرموز
 */
enum class SymbolType : uint8_t {
    VARIABLE = 0x01,        // متغير
    CONSTANT = 0x02,        // ثابت (لا يتغير)
    FUNCTION = 0x03,        // دالة
    METHOD = 0x04,          // طريقة (دالة في هيكل)
    STRUCT = 0x05,          // هيكل
    ENUM = 0x06,            // تعداد
    MODULE = 0x07,          // وحدة
    PARAMETER = 0x08,       // معامل دالة
    FIELD = 0x09,           // حقل هيكل
    GENERIC = 0x0A,         // نوع عام
    TRAIT = 0x0B,           // سمة
    IMPL = 0x0C             // تنفيذ
};

/**
 * 🔐 أعلام الرمز - خصائص إضافية
 */
enum class SymbolFlags : uint16_t {
    NONE = 0x0000,
    PUBLIC = 0x0001,        // عام (يمكن الوصول من الخارج)
    PRIVATE = 0x0002,       // خاص (داخلي فقط)
    MUTABLE = 0x0004,       // قابل للتعديل
    STATIC = 0x0008,        // ساكن (مستوى الوحدة)
    ASYNC = 0x0010,         // غير متزامن
    UNSAFE = 0x0020,        // غير آمن
    EXTERN = 0x0040,        // خارجي (FFI)
    INLINE = 0x0080,        // مضمّن
    GENERIC = 0x0100,       // عام (يقبل أنواع)
    BUILTIN = 0x0200        // مدمج (من اللغة)
};

// دمج الأعلام
inline SymbolFlags operator|(SymbolFlags a, SymbolFlags b) {
    return static_cast<SymbolFlags>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
}

inline SymbolFlags operator&(SymbolFlags a, SymbolFlags b) {
    return static_cast<SymbolFlags>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b));
}

inline bool hasFlag(SymbolFlags flags, SymbolFlags flag) {
    return (static_cast<uint16_t>(flags) & static_cast<uint16_t>(flag)) != 0;
}

// ==========================================================
// 📌 تعليمات Bytecode (Opcodes)
// ==========================================================

/**
 * ⚙️ كود_العملية - تعليمات المفسر الافتراضي
 * 
 * 📚 شرح للمبتدئين:
 * كل تعليمة لها "كود" رقمي يفهمه المفسر.
 * مثلاً: PUSH_INT = ضع رقماً على الـ Stack
 */
enum class Opcode : uint8_t {
    // ═══════════════════════════════════════════════════════
    // 📦 عمليات الـ Stack (المكدس)
    // ═══════════════════════════════════════════════════════
    NOP = 0x00,             // لا عملية (No Operation)
    
    // إضافة للمكدس
    PUSH_NULL = 0x01,       // ضع قيمة فارغة
    PUSH_TRUE = 0x02,       // ضع "صح"
    PUSH_FALSE = 0x03,      // ضع "خطأ"
    PUSH_INT8 = 0x04,       // ضع عدد 8 بت
    PUSH_INT16 = 0x05,      // ضع عدد 16 بت
    PUSH_INT32 = 0x06,      // ضع عدد 32 بت
    PUSH_INT64 = 0x07,      // ضع عدد 64 بت
    PUSH_FLOAT32 = 0x08,    // ضع عشري 32 بت
    PUSH_FLOAT64 = 0x09,    // ضع عشري 64 بت
    PUSH_CONST = 0x0A,      // ضع ثابتاً من جدول الثوابت
    
    // إزالة من المكدس
    POP = 0x10,             // أزل من القمة
    DUP = 0x11,             // نسخ القمة
    DUP2 = 0x12,            // نسخ عنصرين
    SWAP = 0x13,            // بدل القمتين
    ROT = 0x14,             // دور ثلاثة عناصر
    
    // ═══════════════════════════════════════════════════════
    // 🧮 عمليات حسابية
    // ═══════════════════════════════════════════════════════
    ADD = 0x20,             // جمع (+)
    SUB = 0x21,             // طرح (-)
    MUL = 0x22,             // ضرب (*)
    DIV = 0x23,             // قسمة (/)
    MOD = 0x24,             // باقي القسمة (%)
    NEG = 0x25,             // سالب (-)
    INC = 0x26,             // زيادة 1
    DEC = 0x27,             // نقصان 1
    POW = 0x28,             // أس (^)
    
    // ═══════════════════════════════════════════════════════
    // 🔀 عمليات منطقية
    // ═══════════════════════════════════════════════════════
    AND = 0x30,             // و (&&)
    OR = 0x31,              // أو (||)
    NOT = 0x32,             // ليس (!)
    XOR = 0x33,             // أو حصري (^)
    
    // عمليات بتية
    BIT_AND = 0x34,         // و بتي (&)
    BIT_OR = 0x35,          // أو بتي (|)
    BIT_XOR = 0x36,         // أو حصري بتي (^)
    BIT_NOT = 0x37,         // عكس بتي (~)
    SHL = 0x38,             // إزاحة يسار (<<)
    SHR = 0x39,             // إزاحة يمين (>>)
    
    // ═══════════════════════════════════════════════════════
    // ⚖️ عمليات مقارنة
    // ═══════════════════════════════════════════════════════
    EQ = 0x40,              // يساوي (==)
    NE = 0x41,              // لا يساوي (!=)
    LT = 0x42,              // أصغر من (<)
    LE = 0x43,              // أصغر أو يساوي (<=)
    GT = 0x44,              // أكبر من (>)
    GE = 0x45,              // أكبر أو يساوي (>=)
    CMP = 0x46,             // مقارنة عامة
    
    // ═══════════════════════════════════════════════════════
    // 📍 عمليات التحكم في التدفق
    // ═══════════════════════════════════════════════════════
    JMP = 0x50,             // قفز غير مشروط
    JMP_IF = 0x51,          // قفز إذا صح
    JMP_IF_NOT = 0x52,      // قفز إذا خطأ
    JMP_IF_NULL = 0x53,     // قفز إذا فارغ
    JMP_IF_NOT_NULL = 0x54, // قفز إذا ليس فارغ
    
    // حلقات
    LOOP_START = 0x55,      // بداية حلقة
    LOOP_END = 0x56,        // نهاية حلقة
    BREAK = 0x57,           // اخرج من الحلقة
    CONTINUE = 0x58,        // انتقل للتكرار التالي
    
    // ═══════════════════════════════════════════════════════
    // 📞 عمليات الدوال
    // ═══════════════════════════════════════════════════════
    CALL = 0x60,            // استدعاء دالة
    CALL_METHOD = 0x61,     // استدعاء طريقة
    CALL_NATIVE = 0x62,     // استدعاء دالة أصلية
    CALL_VIRTUAL = 0x63,    // استدعاء افتراضي
    RET = 0x64,             // إرجاع من دالة
    RET_VALUE = 0x65,       // إرجاع قيمة
    
    // ═══════════════════════════════════════════════════════
    // 💾 عمليات المتغيرات المحلية
    // ═══════════════════════════════════════════════════════
    LOAD_LOCAL = 0x70,      // حمّل متغير محلي
    STORE_LOCAL = 0x71,     // خزّن في متغير محلي
    LOAD_GLOBAL = 0x72,     // حمّل متغير عام
    STORE_GLOBAL = 0x73,    // خزّن في متغير عام
    LOAD_FIELD = 0x74,      // حمّل حقل
    STORE_FIELD = 0x75,     // خزّن في حقل
    LOAD_ELEM = 0x76,       // حمّل عنصر (من مصفوفة)
    STORE_ELEM = 0x77,      // خزّن عنصر
    
    // ═══════════════════════════════════════════════════════
    // 🏠 عمليات الذاكرة والملكية (خاصة بلغة ص!)
    // ═══════════════════════════════════════════════════════
    /**
     * هذه العمليات تنفذ نظام الملكية في وقت التشغيل
     */
    ALLOC = 0x80,           // تخصيص ذاكرة جديدة
    DEALLOC = 0x81,         // تحرير ذاكرة
    BORROW = 0x82,          // اقتراض (مرجع قراءة)
    BORROW_MUT = 0x83,      // اقتراض قابل للتعديل
    END_BORROW = 0x84,      // انتهاء الاقتراض
    MOVE = 0x85,            // نقل الملكية
    COPY = 0x86,            // نسخ القيمة
    CLONE = 0x87,           // استنساخ عميق
    DROP = 0x88,            // تدمير/تحرير
    
    // فحص الملكية (في وقت التشغيل للـ debug)
    CHECK_OWNER = 0x89,     // تحقق من المالك
    CHECK_BORROW = 0x8A,    // تحقق من الاقتراض
    CHECK_LIFETIME = 0x8B,  // تحقق من فترة الحياة
    
    // ═══════════════════════════════════════════════════════
    // 📦 عمليات الهياكل والمصفوفات
    // ═══════════════════════════════════════════════════════
    NEW_STRUCT = 0x90,      // إنشاء هيكل جديد
    NEW_ARRAY = 0x91,       // إنشاء مصفوفة جديدة
    NEW_MAP = 0x92,         // إنشاء قاموس جديد
    NEW_STRING = 0x93,      // إنشاء نص جديد
    ARRAY_LEN = 0x94,       // طول المصفوفة
    STRING_LEN = 0x95,      // طول النص
    CONCAT = 0x96,          // دمج (نصوص أو مصفوفات)
    SLICE = 0x97,           // قطعة
    
    // ═══════════════════════════════════════════════════════
    // 🔄 عمليات التحويل
    // ═══════════════════════════════════════════════════════
    CAST = 0xA0,            // تحويل نوع صريح
    COERCE = 0xA1,          // تحويل ضمني
    BOX = 0xA2,             // تغليف (إلى heap)
    UNBOX = 0xA3,           // فك التغليف
    
    // ═══════════════════════════════════════════════════════
    // 🚨 عمليات الاستثناءات
    // ═══════════════════════════════════════════════════════
    TRY_START = 0xB0,       // بداية كتلة حاول
    TRY_END = 0xB1,         // نهاية كتلة حاول
    CATCH = 0xB2,           // التقاط استثناء
    THROW = 0xB3,           // رمي استثناء
    PANIC = 0xB4,           // خطأ فادح
    
    // ═══════════════════════════════════════════════════════
    // 🐛 عمليات التنقيح
    // ═══════════════════════════════════════════════════════
    DEBUG_BREAK = 0xF0,     // نقطة توقف
    DEBUG_PRINT = 0xF1,     // طباعة للتنقيح
    DEBUG_TRACE = 0xF2,     // تتبع
    
    // ═══════════════════════════════════════════════════════
    // 🔚 عمليات خاصة
    // ═══════════════════════════════════════════════════════
    HALT = 0xFE,            // إيقاف البرنامج
    INVALID = 0xFF          // تعليمة غير صالحة
};

/**
 * 📝 وصف التعليمة بالعربية
 */
const char* opcodeName(Opcode op);

/**
 * 📊 عدد المعاملات لكل تعليمة
 */
uint8_t opcodeArity(Opcode op);

// ==========================================================
// 📌 الهياكل البيانية
// ==========================================================

/**
 * 📄 رأس الملف (File Header)
 * 
 * أول 16 بايت من كل ملف .صكد
 */
struct FileHeader {
    uint8_t magic[6];           // "صكد" = D8 B5 D9 83 D8 AF
    uint8_t version_major;      // إصدار رئيسي
    uint8_t version_minor;      // إصدار فرعي
    uint8_t version_patch;      // إصدار تصحيحي
    uint8_t flags;              // أعلام
    uint16_t section_count;     // عدد الأقسام
    uint32_t total_size;        // الحجم الكلي
};
static_assert(sizeof(FileHeader) == 16, "FileHeader must be 16 bytes");

/**
 * 📑 رأس القسم (Section Header)
 */
struct SectionHeader {
    SectionType type;           // نوع القسم
    uint8_t flags;              // أعلام
    uint16_t reserved;          // محجوز
    uint32_t offset;            // الموقع في الملف
    uint32_t size;              // حجم القسم
    uint32_t checksum;          // للتحقق من السلامة
};
static_assert(sizeof(SectionHeader) == 16, "SectionHeader must be 16 bytes");

/**
 * 🔢 إدخال في جدول الثوابت
 */
struct ConstantEntry {
    ConstantType type;          // نوع الثابت
    uint8_t flags;              // أعلام
    uint16_t length;            // الطول (للنصوص والبايتات)
    // البيانات تتبع (متغيرة الحجم)
};

/**
 * 🏷️ إدخال في جدول الرموز
 */
struct SymbolEntry {
    uint32_t name_index;        // فهرس الاسم في جدول الثوابت
    SymbolType type;            // نوع الرمز
    uint8_t scope_depth;        // عمق النطاق
    SymbolFlags flags;          // أعلام
    uint32_t type_index;        // فهرس النوع
    uint32_t value_offset;      // موقع القيمة/الكود
};

/**
 * ⚙️ تعليمة Bytecode
 */
struct Instruction {
    Opcode opcode;              // كود العملية
    uint8_t operand_count;      // عدد المعاملات
    uint8_t operand_types[2];   // أنواع المعاملات
    // المعاملات تتبع (متغيرة الحجم)
};

/**
 * 📍 معلومات موقع المصدر (للتنقيح)
 */
struct SourceLocation {
    uint32_t code_offset;       // موقع في الـ bytecode
    uint32_t source_line;       // رقم السطر في المصدر
    uint16_t source_column;     // رقم العمود
    uint16_t file_index;        // فهرس الملف
};

/**
 * 📊 معلومات الدالة
 */
struct FunctionInfo {
    uint32_t name_index;        // فهرس الاسم
    uint32_t code_offset;       // بداية الكود
    uint32_t code_length;       // طول الكود
    uint16_t param_count;       // عدد المعاملات
    uint16_t local_count;       // عدد المتغيرات المحلية
    uint16_t stack_size;        // حجم الـ stack المطلوب
    uint16_t flags;             // أعلام
};

// ==========================================================
// 📌 معلومات الملكية (Ownership Metadata)
// ==========================================================

/**
 * 🏠 حالة الملكية
 */
enum class OwnershipState : uint8_t {
    OWNED = 0x01,               // مملوك
    BORROWED = 0x02,            // مُقترَض
    BORROWED_MUT = 0x03,        // مُقترَض للتعديل
    MOVED = 0x04,               // مُنقَل (لم يعد صالحاً)
    DROPPED = 0x05              // مُحرَّر
};

/**
 * 📋 معلومات ملكية متغير
 */
struct OwnershipInfo {
    uint32_t variable_index;    // فهرس المتغير
    OwnershipState state;       // حالة الملكية
    uint8_t borrow_count;       // عدد الاقتراضات الحالية
    uint16_t lifetime_id;       // معرّف فترة الحياة
    uint32_t owner_scope;       // نطاق المالك
};

/**
 * ⏳ معلومات فترة الحياة
 */
struct LifetimeInfo {
    uint16_t lifetime_id;       // معرّف فترة الحياة
    uint32_t start_offset;      // بداية فترة الحياة
    uint32_t end_offset;        // نهاية فترة الحياة
    uint16_t parent_lifetime;   // فترة الحياة الأب (0 = لا يوجد)
};

// ==========================================================
// 📌 أعلام الملف
// ==========================================================

enum class FileFlags : uint8_t {
    NONE = 0x00,
    DEBUG = 0x01,               // يحتوي معلومات تنقيح
    OPTIMIZED = 0x02,           // محسّن
    COMPRESSED = 0x04,          // مضغوط
    SIGNED = 0x08,              // موقّع رقمياً
    ENCRYPTED = 0x10,           // مشفّر
    EXECUTABLE = 0x20,          // قابل للتنفيذ المباشر
    LIBRARY = 0x40              // مكتبة
};

// ==========================================================
// 📌 Endianness والتحويلات
// ==========================================================

/**
 * تحويل من/إلى Little Endian
 * (لغة ص تستخدم Little Endian دائماً)
 */
inline uint16_t toLittleEndian16(uint16_t value) {
    return value; // نفترض النظام little endian
}

inline uint32_t toLittleEndian32(uint32_t value) {
    return value;
}

inline uint64_t toLittleEndian64(uint64_t value) {
    return value;
}

// ==========================================================
// 📌 دوال مساعدة
// ==========================================================

/**
 * حساب checksum للقسم
 */
uint32_t calculateChecksum(const uint8_t* data, size_t length);

/**
 * التحقق من Magic Number
 */
inline bool isValidMagic(const uint8_t* magic) {
    for (size_t i = 0; i < 6; ++i) {
        if (magic[i] != MAGIC_BYTES[i]) {
            return false;
        }
    }
    return true;
}

/**
 * التحقق من توافق الإصدار
 */
inline bool isCompatibleVersion(uint8_t major, uint8_t minor, uint8_t patch) {
    // الإصدار الرئيسي يجب أن يتطابق
    if (major != VERSION_MAJOR) return false;
    // الإصدار الفرعي يجب أن يكون أقل أو يساوي
    if (minor > VERSION_MINOR) return false;
    return true;
}

} // namespace bytecode
} // namespace sad

// ==========================================================
// 📌 C API للتوافق مع لغات أخرى
// ==========================================================

extern "C" {

/**
 * الحصول على Magic String
 */
const char* sad_bytecode_magic_string(void);

/**
 * الحصول على معلومات الإصدار
 */
void sad_bytecode_version(uint8_t* major, uint8_t* minor, uint8_t* patch);

/**
 * التحقق من صلاحية Magic Number
 */
int sad_bytecode_is_valid_magic(const uint8_t* magic);

/**
 * اسم التعليمة بالعربية
 */
const char* sad_bytecode_opcode_name(uint8_t opcode);

} // extern "C"

#endif // SAD_BYTECODE_FORMAT_H
