/**
 * ==========================================================
 * ملف: validator.cpp
 * الوصف: التحقق من صلاحية ملفات Bytecode
 * المهمة: T277 - نظام Bytecode
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * 
 * 🤔 ما هو المدقق (Validator)؟
 * ----------------------------
 * المدقق يفحص ملف Bytecode للتأكد من صحته قبل تشغيله.
 * مثل الطبيب الذي يفحص المريض قبل العلاج!
 * 
 * 🔍 ماذا يفحص المدقق؟
 * --------------------
 * 1. Magic Number: هل الملف فعلاً ملف .صكد؟
 * 2. الإصدار: هل الإصدار متوافق؟
 * 3. البنية: هل الأقسام صحيحة؟
 * 4. التعليمات: هل كل تعليمة صالحة؟
 * 5. المراجع: هل كل الفهارس ضمن النطاق؟
 * 6. الملكية: هل عمليات الملكية متسقة؟
 * 
 * 🛡️ لماذا التحقق مهم؟
 * ---------------------
 * 1. الأمان: منع تشغيل كود خبيث
 * 2. الاستقرار: منع الأخطاء عند التشغيل
 * 3. التشخيص: اكتشاف المشاكل مبكراً
 * 
 * 📝 مثال على الاستخدام:
 * ----------------------
 * BytecodeValidator validator;
 * auto result = validator.validate(bytecodeData, length);
 * 
 * if (result.valid) {
 *     // الملف صالح، يمكن تشغيله
 * } else {
 *     // عرض الأخطاء
 *     for (const auto& error : result.errors) {
 *         std::cerr << error.message << std::endl;
 *     }
 * }
 */

#include "format.h"
#include <vector>
#include <string>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <set>

namespace sad {
namespace bytecode {

// ==========================================================
// 📌 أنواع الأخطاء
// ==========================================================

/**
 * 🏷️ نوع الخطأ
 */
enum class ValidationErrorKind {
    // أخطاء الرأس
    FILE_TOO_SHORT,         // الملف قصير جداً
    INVALID_MAGIC,          // Magic Number غير صالح
    VERSION_MISMATCH,       // الإصدار غير متوافق
    
    // أخطاء الأقسام
    INVALID_SECTION_TYPE,   // نوع قسم غير معروف
    SECTION_OVERLAP,        // تداخل الأقسام
    SECTION_OUT_OF_BOUNDS,  // قسم خارج حدود الملف
    MISSING_CODE_SECTION,   // قسم الكود مفقود
    
    // أخطاء التعليمات
    INVALID_OPCODE,         // تعليمة غير صالحة
    TRUNCATED_INSTRUCTION,  // تعليمة مقطوعة
    INVALID_OPERAND,        // معامل غير صالح
    
    // أخطاء المراجع
    CONST_INDEX_OUT_OF_RANGE,   // فهرس ثابت خارج النطاق
    SYMBOL_INDEX_OUT_OF_RANGE,  // فهرس رمز خارج النطاق
    FUNC_INDEX_OUT_OF_RANGE,    // فهرس دالة خارج النطاق
    LOCAL_INDEX_OUT_OF_RANGE,   // فهرس محلي خارج النطاق
    
    // أخطاء التحكم
    JUMP_OUT_OF_BOUNDS,     // قفز خارج الكود
    INVALID_LOOP,           // حلقة غير صحيحة
    
    // أخطاء الملكية
    UNBALANCED_BORROW,      // اقتراض غير متوازن
    DOUBLE_DROP,            // إسقاط مزدوج
    USE_AFTER_DROP,         // استخدام بعد الإسقاط
    
    // أخطاء الـ Stack
    STACK_UNDERFLOW,        // Stack فارغ
    STACK_OVERFLOW,         // Stack ممتلئ
    
    // أخطاء Checksum
    CHECKSUM_MISMATCH,      // Checksum لا يتطابق
    
    // أخطاء عامة
    INTERNAL_ERROR          // خطأ داخلي
};

/**
 * 📝 وصف نوع الخطأ
 */
const char* validationErrorKindName(ValidationErrorKind kind) {
    switch (kind) {
        case ValidationErrorKind::FILE_TOO_SHORT: return "الملف قصير جداً";
        case ValidationErrorKind::INVALID_MAGIC: return "Magic Number غير صالح";
        case ValidationErrorKind::VERSION_MISMATCH: return "الإصدار غير متوافق";
        case ValidationErrorKind::INVALID_SECTION_TYPE: return "نوع قسم غير معروف";
        case ValidationErrorKind::SECTION_OVERLAP: return "تداخل الأقسام";
        case ValidationErrorKind::SECTION_OUT_OF_BOUNDS: return "قسم خارج حدود الملف";
        case ValidationErrorKind::MISSING_CODE_SECTION: return "قسم الكود مفقود";
        case ValidationErrorKind::INVALID_OPCODE: return "تعليمة غير صالحة";
        case ValidationErrorKind::TRUNCATED_INSTRUCTION: return "تعليمة مقطوعة";
        case ValidationErrorKind::INVALID_OPERAND: return "معامل غير صالح";
        case ValidationErrorKind::CONST_INDEX_OUT_OF_RANGE: return "فهرس ثابت خارج النطاق";
        case ValidationErrorKind::SYMBOL_INDEX_OUT_OF_RANGE: return "فهرس رمز خارج النطاق";
        case ValidationErrorKind::FUNC_INDEX_OUT_OF_RANGE: return "فهرس دالة خارج النطاق";
        case ValidationErrorKind::LOCAL_INDEX_OUT_OF_RANGE: return "فهرس محلي خارج النطاق";
        case ValidationErrorKind::JUMP_OUT_OF_BOUNDS: return "قفز خارج الكود";
        case ValidationErrorKind::INVALID_LOOP: return "حلقة غير صحيحة";
        case ValidationErrorKind::UNBALANCED_BORROW: return "اقتراض غير متوازن";
        case ValidationErrorKind::DOUBLE_DROP: return "إسقاط مزدوج";
        case ValidationErrorKind::USE_AFTER_DROP: return "استخدام بعد الإسقاط";
        case ValidationErrorKind::STACK_UNDERFLOW: return "Stack فارغ";
        case ValidationErrorKind::STACK_OVERFLOW: return "Stack ممتلئ";
        case ValidationErrorKind::CHECKSUM_MISMATCH: return "Checksum لا يتطابق";
        case ValidationErrorKind::INTERNAL_ERROR: return "خطأ داخلي";
        default: return "خطأ غير معروف";
    }
}

// ==========================================================
// 📌 هيكل الخطأ
// ==========================================================

/**
 * ❌ خطأ في التحقق
 */
struct ValidationError {
    ValidationErrorKind kind;   // نوع الخطأ
    size_t offset;              // الموقع في الملف
    std::string message;        // رسالة تفصيلية
    std::string context;        // سياق إضافي
    
    /**
     * 📝 تحويل إلى string
     */
    std::string toString() const {
        std::ostringstream oss;
        oss << "❌ " << validationErrorKindName(kind);
        oss << " @ " << offset;
        if (!message.empty()) {
            oss << ": " << message;
        }
        if (!context.empty()) {
            oss << " [" << context << "]";
        }
        return oss.str();
    }
};

// ==========================================================
// 📌 نتيجة التحقق
// ==========================================================

/**
 * 📊 نتيجة التحقق
 */
struct ValidationResult {
    bool valid;                         // هل الملف صالح؟
    std::vector<ValidationError> errors;// الأخطاء
    std::vector<std::string> warnings;  // التحذيرات
    
    // إحصائيات
    size_t fileSize;
    size_t sectionCount;
    size_t instructionCount;
    size_t constantCount;
    size_t symbolCount;
    
    /**
     * ➕ إضافة خطأ
     */
    void addError(ValidationErrorKind kind, size_t offset, 
                  const std::string& message = "",
                  const std::string& context = "") {
        errors.push_back({kind, offset, message, context});
        valid = false;
    }
    
    /**
     * ➕ إضافة تحذير
     */
    void addWarning(const std::string& message) {
        warnings.push_back(message);
    }
    
    /**
     * 📝 تقرير النتيجة
     */
    std::string report() const {
        std::ostringstream oss;
        
        oss << "═══════════════════════════════════\n";
        oss << "        نتيجة التحقق\n";
        oss << "═══════════════════════════════════\n\n";
        
        if (valid) {
            oss << "✅ الملف صالح\n\n";
        } else {
            oss << "❌ الملف غير صالح\n\n";
        }
        
        // الإحصائيات
        oss << "📊 الإحصائيات:\n";
        oss << "   حجم الملف: " << fileSize << " بايت\n";
        oss << "   عدد الأقسام: " << sectionCount << "\n";
        oss << "   عدد التعليمات: " << instructionCount << "\n";
        oss << "   عدد الثوابت: " << constantCount << "\n";
        oss << "   عدد الرموز: " << symbolCount << "\n";
        
        // الأخطاء
        if (!errors.empty()) {
            oss << "\n❌ الأخطاء (" << errors.size() << "):\n";
            for (const auto& error : errors) {
                oss << "   " << error.toString() << "\n";
            }
        }
        
        // التحذيرات
        if (!warnings.empty()) {
            oss << "\n⚠️ التحذيرات (" << warnings.size() << "):\n";
            for (const auto& warning : warnings) {
                oss << "   " << warning << "\n";
            }
        }
        
        return oss.str();
    }
};

// ==========================================================
// 📌 فئة BytecodeValidator (المدقق)
// ==========================================================

/**
 * 🔍 مدقق ملفات Bytecode
 */
class BytecodeValidator {
private:
    // خيارات التحقق
    bool strictMode_ = false;       // وضع صارم
    bool checkOwnership_ = true;    // فحص الملكية
    bool checkStackBalance_ = true; // فحص توازن الـ stack
    
public:
    /**
     * ⚙️ تعيين الوضع الصارم
     */
    void setStrictMode(bool strict) { strictMode_ = strict; }
    
    /**
     * ⚙️ تعيين فحص الملكية
     */
    void setCheckOwnership(bool check) { checkOwnership_ = check; }
    
    /**
     * 🔍 التحقق من ملف Bytecode
     */
    ValidationResult validate(const uint8_t* data, size_t length) {
        ValidationResult result;
        result.valid = true;
        result.fileSize = length;
        result.sectionCount = 0;
        result.instructionCount = 0;
        result.constantCount = 0;
        result.symbolCount = 0;
        
        // 1. التحقق من الحجم الأدنى
        if (length < sizeof(FileHeader)) {
            result.addError(ValidationErrorKind::FILE_TOO_SHORT, 0,
                           "يجب أن يكون 16 بايت على الأقل",
                           "الحجم: " + std::to_string(length));
            return result;
        }
        
        // 2. التحقق من Magic Number
        if (!validateMagic(data)) {
            result.addError(ValidationErrorKind::INVALID_MAGIC, 0,
                           "يجب أن يبدأ بـ 'صكد'",
                           formatHex(data, 6));
            return result;
        }
        
        // 3. قراءة الرأس
        FileHeader header;
        std::memcpy(&header, data, sizeof(header));
        
        // 4. التحقق من الإصدار
        if (!isCompatibleVersion(header.version_major, 
                                 header.version_minor,
                                 header.version_patch)) {
            result.addError(ValidationErrorKind::VERSION_MISMATCH, 6,
                           "الإصدار غير متوافق",
                           formatVersion(header.version_major,
                                        header.version_minor,
                                        header.version_patch));
            // نستمر في الفحص
        }
        
        // 5. التحقق من الحجم الكلي
        if (header.total_size != length) {
            result.addWarning("الحجم المعلن (" + std::to_string(header.total_size) + 
                             ") لا يتطابق مع الحجم الفعلي (" + 
                             std::to_string(length) + ")");
        }
        
        // 6. التحقق من الأقسام
        result.sectionCount = header.section_count;
        validateSections(data, length, header.section_count, result);
        
        // 7. التحقق من قسم الكود
        auto codeSection = findSection(data, length, header.section_count, 
                                       SectionType::CODE);
        if (codeSection.first != nullptr) {
            validateCode(codeSection.first, codeSection.second, result);
        } else {
            result.addWarning("لا يوجد قسم كود");
        }
        
        // 8. التحقق من جدول الثوابت
        auto constSection = findSection(data, length, header.section_count,
                                       SectionType::CONSTANT_POOL);
        if (constSection.first != nullptr) {
            validateConstantPool(constSection.first, constSection.second, result);
        }
        
        return result;
    }
    
    /**
     * 🔍 التحقق السريع (فقط الرأس)
     */
    bool quickValidate(const uint8_t* data, size_t length) {
        if (length < sizeof(FileHeader)) return false;
        if (!isValidMagic(data)) return false;
        
        FileHeader header;
        std::memcpy(&header, data, sizeof(header));
        
        return isCompatibleVersion(header.version_major,
                                  header.version_minor,
                                  header.version_patch);
    }

private:
    /**
     * 🔍 التحقق من الأقسام
     */
    void validateSections(const uint8_t* data, size_t length,
                         uint16_t sectionCount, ValidationResult& result) {
        size_t offset = sizeof(FileHeader);
        std::vector<std::pair<uint32_t, uint32_t>> ranges; // (start, end)
        
        for (uint16_t i = 0; i < sectionCount; ++i) {
            if (offset + sizeof(SectionHeader) > length) {
                result.addError(ValidationErrorKind::FILE_TOO_SHORT, offset,
                               "رأس القسم مقطوع",
                               "القسم " + std::to_string(i));
                return;
            }
            
            SectionHeader section;
            std::memcpy(&section, data + offset, sizeof(section));
            
            // التحقق من نوع القسم
            if (static_cast<uint8_t>(section.type) > 0x30 &&
                section.type != SectionType::CUSTOM) {
                result.addWarning("نوع قسم غير معروف: " + 
                                 std::to_string(static_cast<int>(section.type)));
            }
            
            // التحقق من الحدود
            if (section.offset + section.size > length) {
                result.addError(ValidationErrorKind::SECTION_OUT_OF_BOUNDS, offset,
                               "القسم يتجاوز حدود الملف",
                               "القسم " + std::to_string(i));
            }
            
            // التحقق من التداخل
            for (const auto& range : ranges) {
                if (section.offset < range.second && 
                    section.offset + section.size > range.first) {
                    result.addError(ValidationErrorKind::SECTION_OVERLAP, offset,
                                   "تداخل مع قسم آخر",
                                   "القسم " + std::to_string(i));
                }
            }
            ranges.push_back({section.offset, section.offset + section.size});
            
            // التحقق من Checksum
            if (section.checksum != 0 && section.offset + section.size <= length) {
                uint32_t computed = calculateChecksum(data + section.offset, 
                                                     section.size);
                if (computed != section.checksum) {
                    result.addWarning("Checksum لا يتطابق للقسم " + std::to_string(i));
                }
            }
            
            offset += sizeof(SectionHeader);
        }
    }
    
    /**
     * 🔍 إيجاد قسم
     */
    std::pair<const uint8_t*, size_t> findSection(const uint8_t* data, size_t length,
                                                   uint16_t sectionCount,
                                                   SectionType type) {
        size_t offset = sizeof(FileHeader);
        
        for (uint16_t i = 0; i < sectionCount; ++i) {
            if (offset + sizeof(SectionHeader) > length) break;
            
            SectionHeader section;
            std::memcpy(&section, data + offset, sizeof(section));
            
            if (section.type == type) {
                if (section.offset + section.size <= length) {
                    return {data + section.offset, section.size};
                }
            }
            
            offset += sizeof(SectionHeader);
        }
        
        return {nullptr, 0};
    }
    
    /**
     * 🔍 التحقق من الكود
     */
    void validateCode(const uint8_t* code, size_t length,
                     ValidationResult& result) {
        size_t offset = 0;
        std::set<size_t> jumpTargets;
        
        // المرحلة 1: جمع أهداف القفز
        while (offset < length) {
            if (offset >= length) {
                result.addError(ValidationErrorKind::TRUNCATED_INSTRUCTION, offset,
                               "تعليمة مقطوعة في نهاية الكود");
                break;
            }
            
            Opcode op = static_cast<Opcode>(code[offset]);
            uint8_t arity = opcodeArity(op);
            
            // التحقق من صلاحية الـ opcode
            if (op == Opcode::INVALID || 
                static_cast<uint8_t>(op) > 0xFE) {
                result.addError(ValidationErrorKind::INVALID_OPCODE, offset,
                               "تعليمة غير صالحة",
                               "0x" + formatHex(&code[offset], 1));
            }
            
            // حساب حجم التعليمة
            size_t instSize = 1; // الـ opcode
            
            switch (op) {
                case Opcode::PUSH_INT8:
                    instSize += 1;
                    break;
                case Opcode::PUSH_INT16:
                case Opcode::PUSH_CONST:
                case Opcode::LOAD_LOCAL:
                case Opcode::STORE_LOCAL:
                case Opcode::LOAD_GLOBAL:
                case Opcode::STORE_GLOBAL:
                case Opcode::BORROW:
                case Opcode::BORROW_MUT:
                case Opcode::MOVE:
                case Opcode::ALLOC:
                    instSize += 2;
                    break;
                case Opcode::PUSH_INT32:
                case Opcode::JMP:
                case Opcode::JMP_IF:
                case Opcode::JMP_IF_NOT:
                    instSize += 4;
                    // جمع أهداف القفز
                    if (offset + 5 <= length) {
                        int32_t target = *reinterpret_cast<const int32_t*>(&code[offset + 1]);
                        int32_t absTarget = static_cast<int32_t>(offset) + 5 + target;
                        if (absTarget >= 0 && static_cast<size_t>(absTarget) <= length) {
                            jumpTargets.insert(static_cast<size_t>(absTarget));
                        } else {
                            result.addError(ValidationErrorKind::JUMP_OUT_OF_BOUNDS, offset,
                                           "هدف القفز خارج الكود",
                                           "الهدف: " + std::to_string(absTarget));
                        }
                    }
                    break;
                case Opcode::PUSH_INT64:
                case Opcode::PUSH_FLOAT64:
                    instSize += 8;
                    break;
                case Opcode::CALL:
                case Opcode::CALL_METHOD:
                    instSize += 3; // index(2) + argCount(1)
                    break;
                default:
                    // تعليمات بدون معاملات
                    break;
            }
            
            // التحقق من عدم تجاوز الحدود
            if (offset + instSize > length) {
                result.addError(ValidationErrorKind::TRUNCATED_INSTRUCTION, offset,
                               "التعليمة تتجاوز حدود الكود",
                               "الحجم المطلوب: " + std::to_string(instSize));
                break;
            }
            
            result.instructionCount++;
            offset += instSize;
        }
        
        // المرحلة 2: فحص الـ stack balance (اختياري)
        if (checkStackBalance_) {
            validateStackBalance(code, length, result);
        }
        
        // المرحلة 3: فحص الملكية (اختياري)
        if (checkOwnership_) {
            validateOwnership(code, length, result);
        }
    }
    
    /**
     * 🔍 التحقق من توازن الـ Stack
     */
    void validateStackBalance(const uint8_t* code, size_t length,
                              ValidationResult& result) {
        // تتبع عمق الـ stack (مبسط)
        int stackDepth = 0;
        size_t offset = 0;
        
        while (offset < length) {
            Opcode op = static_cast<Opcode>(code[offset]);
            
            // حساب التأثير على الـ stack
            int effect = getStackEffect(op);
            stackDepth += effect;
            
            if (stackDepth < 0) {
                result.addError(ValidationErrorKind::STACK_UNDERFLOW, offset,
                               "محاولة قراءة من stack فارغ",
                               "العمق: " + std::to_string(stackDepth));
                stackDepth = 0; // إعادة تعيين للاستمرار
            }
            
            // الانتقال للتعليمة التالية
            offset += getInstructionSize(op);
        }
    }
    
    /**
     * 🔍 التحقق من عمليات الملكية
     */
    void validateOwnership(const uint8_t* code, size_t length,
                          ValidationResult& result) {
        // تتبع الاقتراضات النشطة (مبسط)
        int borrowCount = 0;
        int mutBorrowCount = 0;
        
        size_t offset = 0;
        while (offset < length) {
            Opcode op = static_cast<Opcode>(code[offset]);
            
            switch (op) {
                case Opcode::BORROW:
                    borrowCount++;
                    break;
                case Opcode::BORROW_MUT:
                    if (borrowCount > 0 || mutBorrowCount > 0) {
                        result.addWarning("اقتراض للتعديل أثناء وجود اقتراضات أخرى @ " +
                                         std::to_string(offset));
                    }
                    mutBorrowCount++;
                    break;
                case Opcode::END_BORROW:
                    if (borrowCount > 0) {
                        borrowCount--;
                    } else if (mutBorrowCount > 0) {
                        mutBorrowCount--;
                    } else {
                        result.addError(ValidationErrorKind::UNBALANCED_BORROW, offset,
                                       "إنهاء اقتراض غير موجود");
                    }
                    break;
                default:
                    break;
            }
            
            offset += getInstructionSize(op);
        }
        
        if (borrowCount > 0 || mutBorrowCount > 0) {
            result.addWarning("اقتراضات لم تُنهَ: " + 
                             std::to_string(borrowCount + mutBorrowCount));
        }
    }
    
    /**
     * 🔍 التحقق من جدول الثوابت
     */
    void validateConstantPool(const uint8_t* data, size_t length,
                             ValidationResult& result) {
        if (length < 2) {
            result.addWarning("جدول الثوابت قصير جداً");
            return;
        }
        
        uint16_t count = static_cast<uint16_t>(data[0] | (data[1] << 8));
        result.constantCount = count;
        
        // فحص مبسط للهيكل
        size_t offset = 2;
        for (uint16_t i = 0; i < count && offset < length; ++i) {
            ConstantType type = static_cast<ConstantType>(data[offset++]);
            
            switch (type) {
                case ConstantType::NULL_VALUE:
                    break;
                case ConstantType::BOOL:
                case ConstantType::INT8:
                    offset += 1;
                    break;
                case ConstantType::INT16:
                    offset += 2;
                    break;
                case ConstantType::INT32:
                case ConstantType::FLOAT32:
                    offset += 4;
                    break;
                case ConstantType::INT64:
                case ConstantType::FLOAT64:
                    offset += 8;
                    break;
                case ConstantType::STRING:
                case ConstantType::STRING_ARABIC:
                    if (offset + 2 <= length) {
                        uint16_t len = static_cast<uint16_t>(
                            data[offset] | (data[offset + 1] << 8));
                        offset += 2 + len;
                    }
                    break;
                default:
                    result.addWarning("نوع ثابت غير معروف: " + 
                                     std::to_string(static_cast<int>(type)));
                    break;
            }
            
            if (offset > length) {
                result.addError(ValidationErrorKind::CONST_INDEX_OUT_OF_RANGE, offset,
                               "جدول الثوابت مقطوع");
                break;
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════
    // دوال مساعدة
    // ═══════════════════════════════════════════════════════
    
    /**
     * 📊 تأثير التعليمة على الـ Stack
     */
    int getStackEffect(Opcode op) {
        switch (op) {
            // تعليمات تضيف للـ stack
            case Opcode::PUSH_NULL:
            case Opcode::PUSH_TRUE:
            case Opcode::PUSH_FALSE:
            case Opcode::PUSH_INT8:
            case Opcode::PUSH_INT16:
            case Opcode::PUSH_INT32:
            case Opcode::PUSH_INT64:
            case Opcode::PUSH_FLOAT32:
            case Opcode::PUSH_FLOAT64:
            case Opcode::PUSH_CONST:
            case Opcode::LOAD_LOCAL:
            case Opcode::LOAD_GLOBAL:
            case Opcode::DUP:
                return 1;
            case Opcode::DUP2:
                return 2;
                
            // تعليمات تزيل من الـ stack
            case Opcode::POP:
            case Opcode::STORE_LOCAL:
            case Opcode::STORE_GLOBAL:
            case Opcode::DROP:
                return -1;
                
            // تعليمات ثنائية (تأخذ 2 وتعطي 1)
            case Opcode::ADD:
            case Opcode::SUB:
            case Opcode::MUL:
            case Opcode::DIV:
            case Opcode::MOD:
            case Opcode::AND:
            case Opcode::OR:
            case Opcode::EQ:
            case Opcode::NE:
            case Opcode::LT:
            case Opcode::LE:
            case Opcode::GT:
            case Opcode::GE:
                return -1;
                
            // تعليمات محايدة
            case Opcode::NOP:
            case Opcode::SWAP:
            case Opcode::ROT:
            case Opcode::JMP:
            case Opcode::JMP_IF:
            case Opcode::JMP_IF_NOT:
                return 0;
                
            default:
                return 0;
        }
    }
    
    /**
     * 📏 حجم التعليمة
     */
    size_t getInstructionSize(Opcode op) {
        switch (op) {
            case Opcode::PUSH_INT8:
                return 2;
            case Opcode::PUSH_INT16:
            case Opcode::PUSH_CONST:
            case Opcode::LOAD_LOCAL:
            case Opcode::STORE_LOCAL:
            case Opcode::LOAD_GLOBAL:
            case Opcode::STORE_GLOBAL:
            case Opcode::BORROW:
            case Opcode::BORROW_MUT:
            case Opcode::MOVE:
            case Opcode::ALLOC:
                return 3;
            case Opcode::PUSH_INT32:
            case Opcode::JMP:
            case Opcode::JMP_IF:
            case Opcode::JMP_IF_NOT:
            case Opcode::PUSH_FLOAT32:
                return 5;
            case Opcode::PUSH_INT64:
            case Opcode::PUSH_FLOAT64:
                return 9;
            case Opcode::CALL:
            case Opcode::CALL_METHOD:
                return 4;
            default:
                return 1;
        }
    }
    
    /**
     * 📝 تنسيق بايتات كـ hex
     */
    std::string formatHex(const uint8_t* data, size_t len) {
        std::ostringstream oss;
        for (size_t i = 0; i < len; ++i) {
            if (i > 0) oss << " ";
            oss << std::hex << std::uppercase << std::setw(2) 
                << std::setfill('0') << static_cast<int>(data[i]);
        }
        return oss.str();
    }
    
    /**
     * 📝 تنسيق الإصدار
     */
    std::string formatVersion(uint8_t major, uint8_t minor, uint8_t patch) {
        std::ostringstream oss;
        oss << static_cast<int>(major) << "."
            << static_cast<int>(minor) << "."
            << static_cast<int>(patch);
        return oss.str();
    }
    
    /**
     * 🔍 التحقق من Magic
     */
    bool validateMagic(const uint8_t* data) {
        return isValidMagic(data);
    }
};

} // namespace bytecode
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

/**
 * إنشاء مدقق جديد
 */
void* sad_bytecode_validator_new(void) {
    return new sad::bytecode::BytecodeValidator();
}

/**
 * تحرير المدقق
 */
void sad_bytecode_validator_free(void* validator) {
    delete static_cast<sad::bytecode::BytecodeValidator*>(validator);
}

/**
 * تعيين الوضع الصارم
 */
void sad_bytecode_validator_set_strict(void* validator, int strict) {
    static_cast<sad::bytecode::BytecodeValidator*>(validator)->setStrictMode(strict != 0);
}

/**
 * التحقق من ملف
 */
int sad_bytecode_validate(void* validator, const uint8_t* data, size_t length,
                          char** errorMessage) {
    auto* v = static_cast<sad::bytecode::BytecodeValidator*>(validator);
    auto result = v->validate(data, length);
    
    if (!result.valid && errorMessage) {
        std::string report = result.report();
        *errorMessage = new char[report.size() + 1];
        std::strcpy(*errorMessage, report.c_str());
    }
    
    return result.valid ? 1 : 0;
}

/**
 * التحقق السريع
 */
int sad_bytecode_quick_validate(void* validator, const uint8_t* data, size_t length) {
    return static_cast<sad::bytecode::BytecodeValidator*>(validator)->quickValidate(data, length) ? 1 : 0;
}

/**
 * تحرير رسالة الخطأ
 */
void sad_bytecode_free_error_message(char* message) {
    delete[] message;
}

} // extern "C"
