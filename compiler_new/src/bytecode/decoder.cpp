/**
 * ==========================================================
 * ملف: decoder.cpp
 * الوصف: فك ترميز تعليمات Bytecode
 * المهمة: T273 - نظام Bytecode
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * 
 * 🤔 ما هو الـ Decoder (فاك الترميز)؟
 * -----------------------------------
 * فاك الترميز يحوّل البايتات إلى تعليمات مفهومة.
 * إنه عكس المُرمِّز (Encoder).
 * 
 * المُرمِّز: تعليمة → بايتات
 * فاك الترميز: بايتات → تعليمة
 * 
 * مثال:
 * ------
 * البايتات: 06 05 00 00 00
 *           ↓ (فك الترميز)
 * التعليمة: PUSH_INT32 5
 *           (ضع الرقم 5 على المكدس)
 * 
 * 🔍 عملية فك الترميز:
 * -------------------
 * 1. قراءة أول بايت = كود العملية (Opcode)
 * 2. تحديد عدد المعاملات من الـ opcode
 * 3. قراءة المعاملات حسب أنواعها
 * 4. إرجاع تعليمة مفكوكة الترميز
 */

#include "format.h"
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cstring>

namespace sad {
namespace bytecode {

// ==========================================================
// 📌 هيكل التعليمة المفكوكة
// ==========================================================

/**
 * ⚙️ تعليمة مفكوكة الترميز
 * 
 * تحتوي على كل المعلومات اللازمة لتنفيذ التعليمة
 */
struct DecodedInstruction {
    Opcode opcode;                  // كود العملية
    size_t offset;                  // الموقع في الكود
    size_t size;                    // حجم التعليمة بالبايتات
    
    // المعاملات (حسب التعليمة)
    union {
        int8_t i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
        float f32;
        double f64;
    } operand1;
    
    union {
        int8_t i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
    } operand2;
    
    uint8_t operandCount;           // عدد المعاملات
    
    /**
     * 📝 تحويل إلى string قابل للقراءة
     */
    std::string toString() const {
        std::ostringstream oss;
        oss << opcodeName(opcode);
        
        if (operandCount >= 1) {
            oss << " ";
            switch (opcode) {
                case Opcode::PUSH_INT8:
                    oss << static_cast<int>(operand1.i8);
                    break;
                case Opcode::PUSH_INT16:
                    oss << operand1.i16;
                    break;
                case Opcode::PUSH_INT32:
                    oss << operand1.i32;
                    break;
                case Opcode::PUSH_INT64:
                    oss << operand1.i64;
                    break;
                case Opcode::PUSH_FLOAT32:
                    oss << operand1.f32;
                    break;
                case Opcode::PUSH_FLOAT64:
                    oss << operand1.f64;
                    break;
                case Opcode::JMP:
                case Opcode::JMP_IF:
                case Opcode::JMP_IF_NOT:
                    oss << "@" << operand1.i32;
                    break;
                default:
                    oss << operand1.u16;
                    break;
            }
        }
        
        if (operandCount >= 2) {
            oss << ", " << operand2.u16;
        }
        
        return oss.str();
    }
    
    /**
     * 📝 تحويل إلى string عربي
     */
    std::string toArabicString() const {
        std::ostringstream oss;
        oss << opcodeName(opcode);
        
        if (operandCount >= 1) {
            oss << " ";
            switch (opcode) {
                case Opcode::PUSH_INT8:
                case Opcode::PUSH_INT16:
                case Opcode::PUSH_INT32:
                case Opcode::PUSH_INT64:
                    oss << operand1.i64;
                    break;
                case Opcode::PUSH_FLOAT32:
                case Opcode::PUSH_FLOAT64:
                    oss << operand1.f64;
                    break;
                case Opcode::JMP:
                case Opcode::JMP_IF:
                case Opcode::JMP_IF_NOT:
                    oss << "إلى @" << operand1.i32;
                    break;
                case Opcode::LOAD_LOCAL:
                case Opcode::STORE_LOCAL:
                    oss << "محلي[" << operand1.u16 << "]";
                    break;
                case Opcode::LOAD_GLOBAL:
                case Opcode::STORE_GLOBAL:
                    oss << "عام[" << operand1.u16 << "]";
                    break;
                case Opcode::PUSH_CONST:
                    oss << "ثابت[" << operand1.u16 << "]";
                    break;
                default:
                    oss << operand1.u16;
                    break;
            }
        }
        
        return oss.str();
    }
};

// ==========================================================
// 📌 فئة InstructionDecoder (فاك ترميز التعليمات)
// ==========================================================

/**
 * 🔓 فاك ترميز التعليمات
 * 
 * 📚 الاستخدام:
 * -------------
 * InstructionDecoder decoder(bytecode, length);
 * 
 * while (decoder.hasMore()) {
 *     DecodedInstruction inst = decoder.decode();
 *     std::cout << inst.toString() << std::endl;
 * }
 */
class InstructionDecoder {
private:
    const uint8_t* data_;       // البيانات
    size_t length_;             // الطول الكلي
    size_t offset_;             // الموقع الحالي
    
public:
    /**
     * 🏗️ إنشاء فاك ترميز
     */
    InstructionDecoder(const uint8_t* data, size_t length)
        : data_(data), length_(length), offset_(0) {}
    
    /**
     * 📍 الموقع الحالي
     */
    size_t offset() const { return offset_; }
    
    /**
     * 📏 الطول الكلي
     */
    size_t length() const { return length_; }
    
    /**
     * 🔚 هل هناك المزيد؟
     */
    bool hasMore() const { return offset_ < length_; }
    
    /**
     * ⏩ الانتقال لموقع معين
     */
    void seek(size_t pos) {
        if (pos > length_) {
            throw std::out_of_range("موقع خارج النطاق");
        }
        offset_ = pos;
    }
    
    /**
     * ⏩ تخطي عدد معين من البايتات
     */
    void skip(size_t count) {
        seek(offset_ + count);
    }
    
    // ═══════════════════════════════════════════════════════
    // قراءة البايتات الخام
    // ═══════════════════════════════════════════════════════
    
    uint8_t readByte() {
        if (offset_ >= length_) {
            throw std::out_of_range("نهاية البيانات");
        }
        return data_[offset_++];
    }
    
    int8_t readInt8() {
        return static_cast<int8_t>(readByte());
    }
    
    int16_t readInt16() {
        uint8_t b0 = readByte();
        uint8_t b1 = readByte();
        return static_cast<int16_t>(b0 | (b1 << 8));
    }
    
    uint16_t readUInt16() {
        uint8_t b0 = readByte();
        uint8_t b1 = readByte();
        return static_cast<uint16_t>(b0 | (b1 << 8));
    }
    
    int32_t readInt32() {
        uint8_t b0 = readByte();
        uint8_t b1 = readByte();
        uint8_t b2 = readByte();
        uint8_t b3 = readByte();
        return static_cast<int32_t>(b0 | (b1 << 8) | (b2 << 16) | (b3 << 24));
    }
    
    uint32_t readUInt32() {
        return static_cast<uint32_t>(readInt32());
    }
    
    int64_t readInt64() {
        int64_t result = 0;
        for (int i = 0; i < 8; ++i) {
            result |= static_cast<int64_t>(readByte()) << (i * 8);
        }
        return result;
    }
    
    float readFloat32() {
        uint32_t bits = readUInt32();
        float result;
        std::memcpy(&result, &bits, sizeof(result));
        return result;
    }
    
    double readFloat64() {
        uint64_t bits = static_cast<uint64_t>(readInt64());
        double result;
        std::memcpy(&result, &bits, sizeof(result));
        return result;
    }
    
    // ═══════════════════════════════════════════════════════
    // فك ترميز التعليمات
    // ═══════════════════════════════════════════════════════
    
    /**
     * 🔓 فك ترميز تعليمة واحدة
     */
    DecodedInstruction decode() {
        DecodedInstruction inst;
        inst.offset = offset_;
        inst.operandCount = 0;
        
        // قراءة كود العملية
        inst.opcode = static_cast<Opcode>(readByte());
        
        // قراءة المعاملات حسب التعليمة
        switch (inst.opcode) {
            // تعليمات بدون معاملات
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
            case Opcode::COPY:
            case Opcode::CLONE:
            case Opcode::DEALLOC:
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
                inst.operandCount = 0;
                break;
            
            // تعليمات مع معامل 8 بت
            case Opcode::PUSH_INT8:
                inst.operand1.i8 = readInt8();
                inst.operandCount = 1;
                break;
            
            // تعليمات مع معامل 16 بت
            case Opcode::PUSH_INT16:
                inst.operand1.i16 = readInt16();
                inst.operandCount = 1;
                break;
            
            // تعليمات مع فهرس 16 بت
            case Opcode::PUSH_CONST:
            case Opcode::LOAD_LOCAL:
            case Opcode::STORE_LOCAL:
            case Opcode::LOAD_GLOBAL:
            case Opcode::STORE_GLOBAL:
            case Opcode::BORROW:
            case Opcode::BORROW_MUT:
            case Opcode::MOVE:
            case Opcode::ALLOC:
            case Opcode::NEW_STRUCT:
            case Opcode::NEW_ARRAY:
            case Opcode::NEW_MAP:
            case Opcode::NEW_STRING:
            case Opcode::CAST:
            case Opcode::COERCE:
            case Opcode::BOX:
            case Opcode::CHECK_OWNER:
            case Opcode::CHECK_BORROW:
            case Opcode::CHECK_LIFETIME:
            case Opcode::DEBUG_PRINT:
            case Opcode::DEBUG_TRACE:
            case Opcode::LOOP_START:
            case Opcode::LOOP_END:
                inst.operand1.u16 = readUInt16();
                inst.operandCount = 1;
                break;
            
            // تعليمات مع معامل 32 بت
            case Opcode::PUSH_INT32:
            case Opcode::JMP:
            case Opcode::JMP_IF:
            case Opcode::JMP_IF_NOT:
            case Opcode::JMP_IF_NULL:
            case Opcode::JMP_IF_NOT_NULL:
                inst.operand1.i32 = readInt32();
                inst.operandCount = 1;
                break;
            
            // تعليمات مع معامل 64 بت
            case Opcode::PUSH_INT64:
                inst.operand1.i64 = readInt64();
                inst.operandCount = 1;
                break;
            
            // تعليمات مع معامل عشري 32 بت
            case Opcode::PUSH_FLOAT32:
                inst.operand1.f32 = readFloat32();
                inst.operandCount = 1;
                break;
            
            // تعليمات مع معامل عشري 64 بت
            case Opcode::PUSH_FLOAT64:
                inst.operand1.f64 = readFloat64();
                inst.operandCount = 1;
                break;
            
            // تعليمات مع معاملين
            case Opcode::CALL:
            case Opcode::CALL_METHOD:
            case Opcode::CALL_NATIVE:
            case Opcode::CALL_VIRTUAL:
                inst.operand1.u16 = readUInt16();  // فهرس الدالة
                inst.operand2.u8 = readByte();     // عدد المعاملات
                inst.operandCount = 2;
                break;
            
            case Opcode::LOAD_FIELD:
            case Opcode::STORE_FIELD:
            case Opcode::LOAD_ELEM:
            case Opcode::STORE_ELEM:
            case Opcode::SLICE:
                inst.operand1.u16 = readUInt16();
                inst.operand2.u16 = readUInt16();
                inst.operandCount = 2;
                break;
            
            default:
                // تعليمة غير معروفة
                break;
        }
        
        inst.size = offset_ - inst.offset;
        return inst;
    }
    
    /**
     * 🔓 فك ترميز كل التعليمات
     */
    std::vector<DecodedInstruction> decodeAll() {
        std::vector<DecodedInstruction> instructions;
        
        while (hasMore()) {
            instructions.push_back(decode());
        }
        
        return instructions;
    }
};

// ==========================================================
// 📌 فئة BytecodeReader (قارئ ملف Bytecode كامل)
// ==========================================================

/**
 * 📖 قارئ ملف Bytecode
 * 
 * 📚 الاستخدام:
 * -------------
 * std::ifstream file("program.صكد", std::ios::binary);
 * std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)),
 *                           std::istreambuf_iterator<char>());
 * 
 * BytecodeReader reader(data.data(), data.size());
 * 
 * if (reader.isValid()) {
 *     std::cout << "الإصدار: " << reader.getVersionString() << std::endl;
 *     
 *     for (const auto& inst : reader.getInstructions()) {
 *         std::cout << inst.toArabicString() << std::endl;
 *     }
 * }
 */
class BytecodeReader {
private:
    const uint8_t* data_;
    size_t length_;
    
    bool valid_ = false;
    std::string error_;
    
    FileHeader header_;
    std::vector<SectionHeader> sections_;
    std::vector<DecodedInstruction> instructions_;
    
public:
    /**
     * 🏗️ إنشاء قارئ
     */
    BytecodeReader(const uint8_t* data, size_t length)
        : data_(data), length_(length) {
        parse();
    }
    
    /**
     * ✅ هل الملف صالح؟
     */
    bool isValid() const { return valid_; }
    
    /**
     * ❌ رسالة الخطأ
     */
    const std::string& getError() const { return error_; }
    
    /**
     * 📊 رأس الملف
     */
    const FileHeader& getHeader() const { return header_; }
    
    /**
     * 📑 الأقسام
     */
    const std::vector<SectionHeader>& getSections() const { return sections_; }
    
    /**
     * ⚙️ التعليمات
     */
    const std::vector<DecodedInstruction>& getInstructions() const { 
        return instructions_; 
    }
    
    /**
     * 📊 الإصدار كـ string
     */
    std::string getVersionString() const {
        std::ostringstream oss;
        oss << static_cast<int>(header_.version_major) << "."
            << static_cast<int>(header_.version_minor) << "."
            << static_cast<int>(header_.version_patch);
        return oss.str();
    }
    
    /**
     * 📝 معلومات الملف
     */
    std::string getInfo() const {
        std::ostringstream oss;
        
        oss << "═══════════════════════════════════\n";
        oss << "        معلومات ملف Bytecode\n";
        oss << "═══════════════════════════════════\n\n";
        
        if (!valid_) {
            oss << "❌ الملف غير صالح: " << error_ << "\n";
            return oss.str();
        }
        
        oss << "✅ الملف صالح\n\n";
        
        oss << "📊 Magic: " << MAGIC_STRING << "\n";
        oss << "📊 الإصدار: " << getVersionString() << "\n";
        oss << "📊 الحجم: " << header_.total_size << " بايت\n";
        oss << "📊 عدد الأقسام: " << header_.section_count << "\n\n";
        
        oss << "📑 الأقسام:\n";
        for (const auto& section : sections_) {
            oss << "   - " << sectionTypeName(section.type)
                << " (الموقع: " << section.offset
                << ", الحجم: " << section.size << ")\n";
        }
        
        oss << "\n⚙️ التعليمات: " << instructions_.size() << " تعليمة\n";
        
        return oss.str();
    }
    
    /**
     * 📝 تفريغ التعليمات (Disassembly)
     */
    std::string disassemble() const {
        std::ostringstream oss;
        
        oss << "; ═══════════════════════════════════\n";
        oss << ";        تفريغ Bytecode لغة ص\n";
        oss << "; ═══════════════════════════════════\n\n";
        
        for (const auto& inst : instructions_) {
            oss << std::setw(6) << inst.offset << ": "
                << inst.toArabicString() << "\n";
        }
        
        return oss.str();
    }

private:
    /**
     * 🔍 تحليل الملف
     */
    void parse() {
        // التحقق من الحد الأدنى للحجم
        if (length_ < sizeof(FileHeader)) {
            error_ = "الملف قصير جداً";
            return;
        }
        
        // قراءة رأس الملف
        std::memcpy(&header_, data_, sizeof(FileHeader));
        
        // التحقق من Magic
        if (!isValidMagic(header_.magic)) {
            error_ = "Magic Number غير صالح - ليس ملف .صكد";
            return;
        }
        
        // التحقق من الإصدار
        if (!isCompatibleVersion(header_.version_major, 
                                 header_.version_minor, 
                                 header_.version_patch)) {
            error_ = "إصدار غير متوافق";
            return;
        }
        
        // قراءة رؤوس الأقسام
        size_t offset = sizeof(FileHeader);
        for (uint16_t i = 0; i < header_.section_count && 
             offset + sizeof(SectionHeader) <= length_; ++i) {
            SectionHeader section;
            std::memcpy(&section, data_ + offset, sizeof(SectionHeader));
            sections_.push_back(section);
            offset += sizeof(SectionHeader);
        }
        
        // البحث عن قسم الكود وفك ترميزه
        for (const auto& section : sections_) {
            if (section.type == SectionType::CODE) {
                if (section.offset + section.size <= length_) {
                    InstructionDecoder decoder(data_ + section.offset, section.size);
                    instructions_ = decoder.decodeAll();
                }
                break;
            }
        }
        
        valid_ = true;
    }
};

// ==========================================================
// 📌 فئة Disassembler (مفكك)
// ==========================================================

/**
 * 🔧 مفكك Bytecode لقراءة البشر
 */
class Disassembler {
public:
    /**
     * 📝 خيارات التفريغ
     */
    struct Options {
        bool showOffset = true;         // إظهار الموقع
        bool showHex = false;           // إظهار البايتات
        bool useArabic = true;          // استخدام الأسماء العربية
        bool showComments = true;       // إظهار التعليقات
    };
    
private:
    Options options_;
    
public:
    Disassembler(const Options& opts = Options()) : options_(opts) {}
    
    /**
     * 📝 تفريغ تعليمة واحدة
     */
    std::string disassemble(const DecodedInstruction& inst, 
                           const uint8_t* rawBytes = nullptr) const {
        std::ostringstream oss;
        
        // الموقع
        if (options_.showOffset) {
            oss << std::setw(6) << std::setfill('0') << std::hex 
                << inst.offset << std::dec << std::setfill(' ') << ": ";
        }
        
        // البايتات الخام
        if (options_.showHex && rawBytes) {
            for (size_t i = 0; i < inst.size && i < 8; ++i) {
                oss << std::setw(2) << std::setfill('0') << std::hex
                    << static_cast<int>(rawBytes[inst.offset + i]) << " ";
            }
            oss << std::dec << std::setfill(' ');
            // محاذاة
            for (size_t i = inst.size; i < 8; ++i) {
                oss << "   ";
            }
        }
        
        // التعليمة
        if (options_.useArabic) {
            oss << inst.toArabicString();
        } else {
            oss << inst.toString();
        }
        
        // تعليق
        if (options_.showComments) {
            std::string comment = getComment(inst);
            if (!comment.empty()) {
                oss << "  ; " << comment;
            }
        }
        
        return oss.str();
    }
    
    /**
     * 📝 تفريغ مجموعة تعليمات
     */
    std::string disassemble(const std::vector<DecodedInstruction>& instructions,
                           const uint8_t* rawBytes = nullptr) const {
        std::ostringstream oss;
        
        for (const auto& inst : instructions) {
            oss << disassemble(inst, rawBytes) << "\n";
        }
        
        return oss.str();
    }
    
private:
    /**
     * 📝 تعليق للتعليمة
     */
    std::string getComment(const DecodedInstruction& inst) const {
        switch (inst.opcode) {
            case Opcode::ALLOC:
                return "تخصيص ذاكرة جديدة";
            case Opcode::BORROW:
                return "اقتراض للقراءة فقط";
            case Opcode::BORROW_MUT:
                return "اقتراض للتعديل (حصري)";
            case Opcode::MOVE:
                return "نقل الملكية";
            case Opcode::DROP:
                return "تحرير الذاكرة";
            case Opcode::CLONE:
                return "نسخة عميقة";
            case Opcode::END_BORROW:
                return "إنهاء الاقتراض";
            case Opcode::JMP:
                return "قفز غير مشروط";
            case Opcode::JMP_IF:
                return "قفز إذا كانت القيمة صحيحة";
            case Opcode::JMP_IF_NOT:
                return "قفز إذا كانت القيمة خاطئة";
            case Opcode::CALL:
                return "استدعاء دالة";
            case Opcode::RET:
                return "العودة من الدالة";
            case Opcode::HALT:
                return "إيقاف البرنامج";
            default:
                return "";
        }
    }
};

} // namespace bytecode
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

/**
 * إنشاء decoder جديد
 */
void* sad_instruction_decoder_new(const uint8_t* data, size_t length) {
    return new sad::bytecode::InstructionDecoder(data, length);
}

/**
 * تحرير decoder
 */
void sad_instruction_decoder_free(void* decoder) {
    delete static_cast<sad::bytecode::InstructionDecoder*>(decoder);
}

/**
 * هل هناك المزيد؟
 */
int sad_instruction_decoder_has_more(void* decoder) {
    return static_cast<sad::bytecode::InstructionDecoder*>(decoder)->hasMore() ? 1 : 0;
}

/**
 * الموقع الحالي
 */
size_t sad_instruction_decoder_offset(void* decoder) {
    return static_cast<sad::bytecode::InstructionDecoder*>(decoder)->offset();
}

/**
 * إنشاء قارئ ملف
 */
void* sad_bytecode_reader_new(const uint8_t* data, size_t length) {
    return new sad::bytecode::BytecodeReader(data, length);
}

/**
 * تحرير قارئ
 */
void sad_bytecode_reader_free(void* reader) {
    delete static_cast<sad::bytecode::BytecodeReader*>(reader);
}

/**
 * هل الملف صالح؟
 */
int sad_bytecode_reader_is_valid(void* reader) {
    return static_cast<sad::bytecode::BytecodeReader*>(reader)->isValid() ? 1 : 0;
}

/**
 * رسالة الخطأ
 */
const char* sad_bytecode_reader_error(void* reader) {
    static std::string error;
    error = static_cast<sad::bytecode::BytecodeReader*>(reader)->getError();
    return error.c_str();
}

/**
 * معلومات الملف
 */
const char* sad_bytecode_reader_info(void* reader) {
    static std::string info;
    info = static_cast<sad::bytecode::BytecodeReader*>(reader)->getInfo();
    return info.c_str();
}

/**
 * تفريغ التعليمات
 */
const char* sad_bytecode_reader_disassemble(void* reader) {
    static std::string disasm;
    disasm = static_cast<sad::bytecode::BytecodeReader*>(reader)->disassemble();
    return disasm.c_str();
}

} // extern "C"
