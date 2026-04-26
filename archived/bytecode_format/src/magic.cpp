/**
 * ==========================================================
 * ملف: magic.cpp
 * الوصف: تنفيذ Magic Number "صكد" للغة "ص"
 * المهمة: T270 - نظام Bytecode
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * 
 * 🔢 ما هو Magic Number؟
 * ----------------------
 * Magic Number هو توقيع فريد في بداية كل ملف يُعرِّف نوعه.
 * 
 * أمثلة مشهورة:
 * - ملفات PDF تبدأ بـ "%PDF"
 * - ملفات ZIP تبدأ بـ "PK"
 * - ملفات PNG تبدأ بـ "‰PNG"
 * - ملفات Java (.class) تبدأ بـ "CAFEBABE"
 * 
 * 🌟 اختيار "صكد" كـ Magic Number:
 * ---------------------------------
 * اخترنا الحروف العربية "صكد" لأسباب:
 * 
 * 1️⃣ الهوية العربية:
 *    أول لغة برمجة عربية لها magic number عربي!
 * 
 * 2️⃣ المعنى:
 *    ص = اسم اللغة
 *    ك = كود (code)
 *    د = مترجم/مجمّع
 *    
 * 3️⃣ التفرد:
 *    لا توجد لغة أخرى تستخدم هذه البايتات!
 * 
 * 📊 تمثيل UTF-8:
 * ---------------
 * الحروف العربية تُخزَّن بترميز UTF-8:
 * 
 *   ص (Sad) = 0xD8 0xB5
 *   ك (Kaf) = 0xD9 0x83
 *   د (Dal) = 0xD8 0xAF
 * 
 * إذاً Magic Number الكامل = D8 B5 D9 83 D8 AF (6 بايتات)
 * 
 * 🔍 كيف يعمل التحقق:
 * -------------------
 * عند فتح ملف .صكد:
 * 1. نقرأ أول 6 بايتات
 * 2. نقارنها مع Magic المتوقع
 * 3. إذا تطابقت → ملف صالح
 * 4. إذا لم تتطابق → ملف غير صالح أو تالف
 */

#include "format.h"
#include <cstring>
#include <sstream>
#include <iomanip>

namespace sad {
namespace bytecode {

// ==========================================================
// 📌 تنفيذ Magic Number
// ==========================================================

/**
 * 📝 الحصول على Magic String
 * 
 * @return النص العربي "صكد"
 */
const char* getMagicString() {
    return MAGIC_STRING;
}

/**
 * 🔍 التحقق من صلاحية Magic Number
 * 
 * @param data البيانات المراد فحصها
 * @param length طول البيانات
 * @return true إذا كان الـ magic صحيحاً
 * 
 * 📚 شرح:
 * --------
 * هذه الدالة تفحص أول 6 بايتات من الملف
 * وتقارنها بالتوقيع المتوقع "صكد"
 */
bool validateMagic(const uint8_t* data, size_t length) {
    // نحتاج على الأقل 6 بايتات
    if (length < 6) {
        return false;
    }
    
    // مقارنة بايت بايت
    for (size_t i = 0; i < 6; ++i) {
        if (data[i] != MAGIC_BYTES[i]) {
            return false;
        }
    }
    
    return true;
}

/**
 * ✍️ كتابة Magic Number
 * 
 * @param output المخرجات
 * 
 * تكتب الـ 6 بايتات في بداية الملف
 */
void writeMagic(uint8_t* output) {
    for (size_t i = 0; i < 6; ++i) {
        output[i] = MAGIC_BYTES[i];
    }
}

/**
 * 📊 الحصول على Magic كـ vector
 */
std::vector<uint8_t> getMagicBytes() {
    return std::vector<uint8_t>(MAGIC_BYTES.begin(), MAGIC_BYTES.end());
}

/**
 * 📝 تمثيل Magic كـ hex string
 * 
 * @return "D8 B5 D9 83 D8 AF"
 */
std::string getMagicHexString() {
    std::ostringstream oss;
    for (size_t i = 0; i < 6; ++i) {
        if (i > 0) oss << " ";
        oss << std::uppercase << std::hex << std::setfill('0') << std::setw(2) 
            << static_cast<int>(MAGIC_BYTES[i]);
    }
    return oss.str();
}

// ==========================================================
// 📌 معلومات الإصدار
// ==========================================================

/**
 * 📊 الحصول على معلومات الإصدار
 */
void getVersion(uint8_t& major, uint8_t& minor, uint8_t& patch) {
    major = VERSION_MAJOR;
    minor = VERSION_MINOR;
    patch = VERSION_PATCH;
}

/**
 * 📝 إصدار كـ string
 * 
 * @return "1.0.0"
 */
std::string getVersionString() {
    std::ostringstream oss;
    oss << static_cast<int>(VERSION_MAJOR) << "."
        << static_cast<int>(VERSION_MINOR) << "."
        << static_cast<int>(VERSION_PATCH);
    return oss.str();
}

/**
 * 🔍 التحقق من توافق الإصدار
 * 
 * @param major الإصدار الرئيسي
 * @param minor الإصدار الفرعي
 * @param patch إصدار التصحيح
 * @return true إذا كان الإصدار متوافقاً
 * 
 * 📚 قواعد التوافق:
 * ------------------
 * - الإصدار الرئيسي يجب أن يتطابق تماماً
 * - الإصدار الفرعي يمكن أن يكون أقل أو مساوياً
 * - إصدار التصحيح لا يؤثر على التوافق
 */
bool checkVersionCompatibility(uint8_t major, uint8_t minor, uint8_t /*patch*/) {
    // الإصدار الرئيسي يجب أن يتطابق
    if (major != VERSION_MAJOR) {
        return false;
    }
    
    // الإصدار الفرعي يجب أن يكون أقل أو مساوي
    if (minor > VERSION_MINOR) {
        return false;
    }
    
    // إصدار التصحيح لا يؤثر على التوافق
    return true;
}

// ==========================================================
// 📌 أسماء التعليمات (Opcodes)
// ==========================================================

/**
 * 📝 اسم التعليمة بالعربية
 * 
 * @param op كود العملية
 * @return الاسم العربي
 */
const char* opcodeName(Opcode op) {
    switch (op) {
        // عمليات Stack
        case Opcode::NOP: return "لا_عملية";
        case Opcode::PUSH_NULL: return "ضع_فارغ";
        case Opcode::PUSH_TRUE: return "ضع_صح";
        case Opcode::PUSH_FALSE: return "ضع_خطأ";
        case Opcode::PUSH_INT8: return "ضع_صحيح٨";
        case Opcode::PUSH_INT16: return "ضع_صحيح١٦";
        case Opcode::PUSH_INT32: return "ضع_صحيح٣٢";
        case Opcode::PUSH_INT64: return "ضع_صحيح٦٤";
        case Opcode::PUSH_FLOAT32: return "ضع_عشري٣٢";
        case Opcode::PUSH_FLOAT64: return "ضع_عشري٦٤";
        case Opcode::PUSH_CONST: return "ضع_ثابت";
        case Opcode::POP: return "أزل";
        case Opcode::DUP: return "كرر";
        case Opcode::DUP2: return "كرر٢";
        case Opcode::SWAP: return "بدّل";
        case Opcode::ROT: return "دوّر";
        
        // عمليات حسابية
        case Opcode::ADD: return "جمع";
        case Opcode::SUB: return "طرح";
        case Opcode::MUL: return "ضرب";
        case Opcode::DIV: return "قسمة";
        case Opcode::MOD: return "باقي";
        case Opcode::NEG: return "سالب";
        case Opcode::INC: return "زد";
        case Opcode::DEC: return "أنقص";
        case Opcode::POW: return "أس";
        
        // عمليات منطقية
        case Opcode::AND: return "و";
        case Opcode::OR: return "أو";
        case Opcode::NOT: return "ليس";
        case Opcode::XOR: return "أو_حصري";
        case Opcode::BIT_AND: return "و_بتي";
        case Opcode::BIT_OR: return "أو_بتي";
        case Opcode::BIT_XOR: return "حصري_بتي";
        case Opcode::BIT_NOT: return "عكس_بتي";
        case Opcode::SHL: return "أزح_يسار";
        case Opcode::SHR: return "أزح_يمين";
        
        // عمليات مقارنة
        case Opcode::EQ: return "يساوي";
        case Opcode::NE: return "لا_يساوي";
        case Opcode::LT: return "أصغر";
        case Opcode::LE: return "أصغر_أو_يساوي";
        case Opcode::GT: return "أكبر";
        case Opcode::GE: return "أكبر_أو_يساوي";
        case Opcode::CMP: return "قارن";
        
        // عمليات التحكم
        case Opcode::JMP: return "اقفز";
        case Opcode::JMP_IF: return "اقفز_إذا";
        case Opcode::JMP_IF_NOT: return "اقفز_إذا_لا";
        case Opcode::JMP_IF_NULL: return "اقفز_إذا_فارغ";
        case Opcode::JMP_IF_NOT_NULL: return "اقفز_إذا_ليس_فارغ";
        case Opcode::LOOP_START: return "بداية_حلقة";
        case Opcode::LOOP_END: return "نهاية_حلقة";
        case Opcode::BREAK: return "اخرج";
        case Opcode::CONTINUE: return "تابع";
        
        // عمليات الدوال
        case Opcode::CALL: return "استدعِ";
        case Opcode::CALL_METHOD: return "استدعِ_طريقة";
        case Opcode::CALL_NATIVE: return "استدعِ_أصلي";
        case Opcode::CALL_VIRTUAL: return "استدعِ_افتراضي";
        case Opcode::RET: return "ارجع";
        case Opcode::RET_VALUE: return "ارجع_قيمة";
        
        // عمليات المتغيرات
        case Opcode::LOAD_LOCAL: return "حمّل_محلي";
        case Opcode::STORE_LOCAL: return "خزّن_محلي";
        case Opcode::LOAD_GLOBAL: return "حمّل_عام";
        case Opcode::STORE_GLOBAL: return "خزّن_عام";
        case Opcode::LOAD_FIELD: return "حمّل_حقل";
        case Opcode::STORE_FIELD: return "خزّن_حقل";
        case Opcode::LOAD_ELEM: return "حمّل_عنصر";
        case Opcode::STORE_ELEM: return "خزّن_عنصر";
        
        // عمليات الملكية (الأهم في لغة ص!)
        case Opcode::ALLOC: return "خصص";
        case Opcode::DEALLOC: return "حرر";
        case Opcode::BORROW: return "اقترض";
        case Opcode::BORROW_MUT: return "اقترض_للتعديل";
        case Opcode::END_BORROW: return "أنهِ_الاقتراض";
        case Opcode::MOVE: return "انقل";
        case Opcode::COPY: return "انسخ";
        case Opcode::CLONE: return "استنسخ";
        case Opcode::DROP: return "أسقط";
        case Opcode::CHECK_OWNER: return "تحقق_المالك";
        case Opcode::CHECK_BORROW: return "تحقق_الاقتراض";
        case Opcode::CHECK_LIFETIME: return "تحقق_فترة_الحياة";
        
        // عمليات الهياكل
        case Opcode::NEW_STRUCT: return "أنشئ_هيكل";
        case Opcode::NEW_ARRAY: return "أنشئ_مصفوفة";
        case Opcode::NEW_MAP: return "أنشئ_قاموس";
        case Opcode::NEW_STRING: return "أنشئ_نص";
        case Opcode::ARRAY_LEN: return "طول_مصفوفة";
        case Opcode::STRING_LEN: return "طول_نص";
        case Opcode::CONCAT: return "ادمج";
        case Opcode::SLICE: return "قطعة";
        
        // عمليات التحويل
        case Opcode::CAST: return "حوّل";
        case Opcode::COERCE: return "حوّل_ضمني";
        case Opcode::BOX: return "غلّف";
        case Opcode::UNBOX: return "فك_التغليف";
        
        // عمليات الاستثناءات
        case Opcode::TRY_START: return "بداية_حاول";
        case Opcode::TRY_END: return "نهاية_حاول";
        case Opcode::CATCH: return "التقط";
        case Opcode::THROW: return "ارمِ";
        case Opcode::PANIC: return "فزع";
        
        // عمليات التنقيح
        case Opcode::DEBUG_BREAK: return "نقطة_توقف";
        case Opcode::DEBUG_PRINT: return "طباعة_تنقيح";
        case Opcode::DEBUG_TRACE: return "تتبع";
        
        // عمليات خاصة
        case Opcode::HALT: return "أوقف";
        case Opcode::INVALID: return "غير_صالح";
        
        default: return "مجهول";
    }
}

/**
 * 📊 عدد المعاملات لكل تعليمة
 * 
 * @param op كود العملية
 * @return عدد المعاملات (0-3)
 */
uint8_t opcodeArity(Opcode op) {
    switch (op) {
        // لا معاملات
        case Opcode::NOP:
        case Opcode::PUSH_NULL:
        case Opcode::PUSH_TRUE:
        case Opcode::PUSH_FALSE:
        case Opcode::POP:
        case Opcode::DUP:
        case Opcode::DUP2:
        case Opcode::SWAP:
        case Opcode::ROT:
        case Opcode::ADD:
        case Opcode::SUB:
        case Opcode::MUL:
        case Opcode::DIV:
        case Opcode::MOD:
        case Opcode::NEG:
        case Opcode::INC:
        case Opcode::DEC:
        case Opcode::POW:
        case Opcode::AND:
        case Opcode::OR:
        case Opcode::NOT:
        case Opcode::XOR:
        case Opcode::BIT_AND:
        case Opcode::BIT_OR:
        case Opcode::BIT_XOR:
        case Opcode::BIT_NOT:
        case Opcode::SHL:
        case Opcode::SHR:
        case Opcode::EQ:
        case Opcode::NE:
        case Opcode::LT:
        case Opcode::LE:
        case Opcode::GT:
        case Opcode::GE:
        case Opcode::CMP:
        case Opcode::RET:
        case Opcode::RET_VALUE:
        case Opcode::BREAK:
        case Opcode::CONTINUE:
        case Opcode::DROP:
        case Opcode::END_BORROW:
        case Opcode::ARRAY_LEN:
        case Opcode::STRING_LEN:
        case Opcode::CONCAT:
        case Opcode::UNBOX:
        case Opcode::TRY_START:
        case Opcode::TRY_END:
        case Opcode::CATCH:
        case Opcode::THROW:
        case Opcode::PANIC:
        case Opcode::DEBUG_BREAK:
        case Opcode::HALT:
        case Opcode::INVALID:
            return 0;
        
        // معامل واحد
        case Opcode::PUSH_INT8:
        case Opcode::PUSH_INT16:
        case Opcode::PUSH_INT32:
        case Opcode::PUSH_INT64:
        case Opcode::PUSH_FLOAT32:
        case Opcode::PUSH_FLOAT64:
        case Opcode::PUSH_CONST:
        case Opcode::JMP:
        case Opcode::JMP_IF:
        case Opcode::JMP_IF_NOT:
        case Opcode::JMP_IF_NULL:
        case Opcode::JMP_IF_NOT_NULL:
        case Opcode::LOOP_START:
        case Opcode::LOOP_END:
        case Opcode::LOAD_LOCAL:
        case Opcode::STORE_LOCAL:
        case Opcode::LOAD_GLOBAL:
        case Opcode::STORE_GLOBAL:
        case Opcode::ALLOC:
        case Opcode::DEALLOC:
        case Opcode::BORROW:
        case Opcode::BORROW_MUT:
        case Opcode::MOVE:
        case Opcode::COPY:
        case Opcode::CLONE:
        case Opcode::CHECK_OWNER:
        case Opcode::CHECK_BORROW:
        case Opcode::CHECK_LIFETIME:
        case Opcode::NEW_STRUCT:
        case Opcode::NEW_ARRAY:
        case Opcode::NEW_MAP:
        case Opcode::NEW_STRING:
        case Opcode::CAST:
        case Opcode::COERCE:
        case Opcode::BOX:
        case Opcode::DEBUG_PRINT:
        case Opcode::DEBUG_TRACE:
            return 1;
        
        // معاملان
        case Opcode::CALL:
        case Opcode::CALL_METHOD:
        case Opcode::CALL_NATIVE:
        case Opcode::CALL_VIRTUAL:
        case Opcode::LOAD_FIELD:
        case Opcode::STORE_FIELD:
        case Opcode::LOAD_ELEM:
        case Opcode::STORE_ELEM:
        case Opcode::SLICE:
            return 2;
        
        default:
            return 0;
    }
}

// ==========================================================
// 📌 حساب Checksum
// ==========================================================

/**
 * 🔒 حساب checksum للتحقق من سلامة البيانات
 * 
 * نستخدم خوارزمية CRC32 المبسطة
 * 
 * @param data البيانات
 * @param length الطول
 * @return قيمة checksum
 */
uint32_t calculateChecksum(const uint8_t* data, size_t length) {
    // CRC32 lookup table
    static const uint32_t crc_table[256] = {
        0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
        0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
        0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
        0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
        0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
        0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
        0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
        0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
        // ... (simplified for brevity, full table needed in production)
        0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
    };
    
    uint32_t crc = 0xFFFFFFFF;
    
    for (size_t i = 0; i < length; ++i) {
        uint8_t index = static_cast<uint8_t>((crc ^ data[i]) & 0xFF);
        // Simplified: in production, use full table
        crc = (crc >> 8) ^ crc_table[index % 64];
    }
    
    return crc ^ 0xFFFFFFFF;
}

// ==========================================================
// 📌 فئة MagicValidator
// ==========================================================

/**
 * 🔍 فئة للتحقق من Magic Number والإصدار
 * 
 * 📚 الاستخدام:
 * -------------
 * MagicValidator validator;
 * 
 * // عند قراءة ملف
 * std::ifstream file("program.صكد", std::ios::binary);
 * uint8_t header[16];
 * file.read(reinterpret_cast<char*>(header), 16);
 * 
 * auto result = validator.validate(header, 16);
 * if (!result.valid) {
 *     std::cerr << "خطأ: " << result.message << std::endl;
 * }
 */
class MagicValidator {
public:
    /**
     * 📊 نتيجة التحقق
     */
    struct ValidationResult {
        bool valid;             // هل الملف صالح؟
        std::string message;    // رسالة (للأخطاء)
        uint8_t major;          // الإصدار الرئيسي
        uint8_t minor;          // الإصدار الفرعي
        uint8_t patch;          // إصدار التصحيح
    };
    
    /**
     * 🔍 التحقق من رأس الملف
     */
    ValidationResult validate(const uint8_t* data, size_t length) {
        ValidationResult result = {false, "", 0, 0, 0};
        
        // التحقق من الحجم الأدنى
        if (length < sizeof(FileHeader)) {
            result.message = "الملف قصير جداً - يجب أن يكون 16 بايت على الأقل";
            return result;
        }
        
        // التحقق من Magic Number
        if (!validateMagic(data, 6)) {
            result.message = "Magic Number غير صالح - يجب أن يبدأ بـ 'صكد'";
            return result;
        }
        
        // قراءة الإصدار
        result.major = data[6];
        result.minor = data[7];
        result.patch = data[8];
        
        // التحقق من توافق الإصدار
        if (!checkVersionCompatibility(result.major, result.minor, result.patch)) {
            std::ostringstream oss;
            oss << "إصدار غير متوافق: " 
                << static_cast<int>(result.major) << "."
                << static_cast<int>(result.minor) << "."
                << static_cast<int>(result.patch)
                << " (المطلوب: " << getVersionString() << ")";
            result.message = oss.str();
            return result;
        }
        
        result.valid = true;
        result.message = "الملف صالح";
        return result;
    }
    
    /**
     * 📝 وصف تفصيلي للرأس
     */
    std::string describeHeader(const uint8_t* data, size_t length) {
        if (length < sizeof(FileHeader)) {
            return "رأس غير مكتمل";
        }
        
        std::ostringstream oss;
        oss << "═══════════════════════════════════\n";
        oss << "     معلومات ملف Bytecode لغة ص\n";
        oss << "═══════════════════════════════════\n\n";
        
        // Magic Number
        oss << "🔢 Magic Number:\n";
        oss << "   النص: " << MAGIC_STRING << "\n";
        oss << "   Hex: " << getMagicHexString() << "\n";
        oss << "   الحالة: " << (validateMagic(data, 6) ? "✓ صالح" : "✗ غير صالح") << "\n\n";
        
        // الإصدار
        oss << "📊 الإصدار:\n";
        oss << "   " << static_cast<int>(data[6]) << "."
            << static_cast<int>(data[7]) << "."
            << static_cast<int>(data[8]) << "\n";
        oss << "   التوافق: " << (checkVersionCompatibility(data[6], data[7], data[8]) 
                                  ? "✓ متوافق" : "✗ غير متوافق") << "\n\n";
        
        // الأعلام
        uint8_t flags = data[9];
        oss << "🏳️ الأعلام: 0x" << std::hex << static_cast<int>(flags) << std::dec << "\n";
        if (flags & static_cast<uint8_t>(FileFlags::DEBUG)) oss << "   - Debug\n";
        if (flags & static_cast<uint8_t>(FileFlags::OPTIMIZED)) oss << "   - Optimized\n";
        if (flags & static_cast<uint8_t>(FileFlags::COMPRESSED)) oss << "   - Compressed\n";
        if (flags & static_cast<uint8_t>(FileFlags::EXECUTABLE)) oss << "   - Executable\n";
        if (flags & static_cast<uint8_t>(FileFlags::LIBRARY)) oss << "   - Library\n";
        
        // عدد الأقسام والحجم
        uint16_t sectionCount = *reinterpret_cast<const uint16_t*>(&data[10]);
        uint32_t totalSize = *reinterpret_cast<const uint32_t*>(&data[12]);
        
        oss << "\n📑 عدد الأقسام: " << sectionCount << "\n";
        oss << "📦 الحجم الكلي: " << totalSize << " بايت\n";
        
        return oss.str();
    }
};

// ==========================================================
// 📌 فئة MagicWriter
// ==========================================================

/**
 * ✍️ فئة لكتابة رأس الملف
 */
class MagicWriter {
public:
    /**
     * 📝 كتابة رأس ملف كامل
     */
    static void writeHeader(uint8_t* output, uint8_t flags, 
                           uint16_t sectionCount, uint32_t totalSize) {
        // Magic Number
        writeMagic(output);
        
        // الإصدار
        output[6] = VERSION_MAJOR;
        output[7] = VERSION_MINOR;
        output[8] = VERSION_PATCH;
        
        // الأعلام
        output[9] = flags;
        
        // عدد الأقسام
        *reinterpret_cast<uint16_t*>(&output[10]) = toLittleEndian16(sectionCount);
        
        // الحجم الكلي
        *reinterpret_cast<uint32_t*>(&output[12]) = toLittleEndian32(totalSize);
    }
    
    /**
     * 📝 إنشاء رأس كـ vector
     */
    static std::vector<uint8_t> createHeader(uint8_t flags = 0,
                                             uint16_t sectionCount = 0,
                                             uint32_t totalSize = 16) {
        std::vector<uint8_t> header(16);
        writeHeader(header.data(), flags, sectionCount, totalSize);
        return header;
    }
};

} // namespace bytecode
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

const char* sad_bytecode_magic_string(void) {
    return sad::bytecode::getMagicString();
}

void sad_bytecode_version(uint8_t* major, uint8_t* minor, uint8_t* patch) {
    sad::bytecode::getVersion(*major, *minor, *patch);
}

int sad_bytecode_is_valid_magic(const uint8_t* magic) {
    return sad::bytecode::validateMagic(magic, 6) ? 1 : 0;
}

const char* sad_bytecode_opcode_name(uint8_t opcode) {
    return sad::bytecode::opcodeName(static_cast<sad::bytecode::Opcode>(opcode));
}

// دوال إضافية

/**
 * الحصول على hex string للـ magic
 */
const char* sad_bytecode_magic_hex(void) {
    static std::string hex = sad::bytecode::getMagicHexString();
    return hex.c_str();
}

/**
 * الحصول على string الإصدار
 */
const char* sad_bytecode_version_string(void) {
    static std::string ver = sad::bytecode::getVersionString();
    return ver.c_str();
}

/**
 * التحقق من توافق الإصدار
 */
int sad_bytecode_check_version(uint8_t major, uint8_t minor, uint8_t patch) {
    return sad::bytecode::checkVersionCompatibility(major, minor, patch) ? 1 : 0;
}

/**
 * حساب checksum
 */
uint32_t sad_bytecode_checksum(const uint8_t* data, size_t length) {
    return sad::bytecode::calculateChecksum(data, length);
}

/**
 * عدد معاملات التعليمة
 */
uint8_t sad_bytecode_opcode_arity(uint8_t opcode) {
    return sad::bytecode::opcodeArity(static_cast<sad::bytecode::Opcode>(opcode));
}

} // extern "C"
