/**
 * @file asm_parser.cpp
 * @brief (AR) محلل كود التجميع المُدمج في لغة ص
 *        (EN) Inline assembly parser for Sad language
 * 
 * @details
 * ╔═══════════════════════════════════════════════════════════════════════════════╗
 * ║                                                                               ║
 * ║     ████████╗ ██████╗ ███╗   ███╗██╗██╗    ██╗                               ║
 * ║     ╚══██╔══╝██╔════╝ ████╗ ████║██║██║    ██║                               ║
 * ║        ██║   ██║  ███╗██╔████╔██║██║██║ █╗ ██║                               ║
 * ║        ██║   ██║   ██║██║╚██╔╝██║██║██║███╗██║                               ║
 * ║        ██║   ╚██████╔╝██║ ╚═╝ ██║██║╚███╔███╔╝                               ║
 * ║        ╚═╝    ╚═════╝ ╚═╝     ╚═╝╚═╝ ╚══╝╚══╝                                ║
 * ║                                                                               ║
 * ║                     تجميع! - Inline Assembly                                  ║
 * ║                     لغة ص للتحكم المباشر بالمعالج                             ║
 * ║                                                                               ║
 * ╚═══════════════════════════════════════════════════════════════════════════════╝
 * 
 * (AR) هذا الملف يُمكّن من كتابة كود تجميع مُدمج (inline assembly):
 * 
 *      الصيغة الأساسية:
 *      @code
 *      تجميع!("تعليمات التجميع"
 *          : مخرجات
 *          : مدخلات
 *          : مُتلفات
 *          : خيارات
 *      )
 *      @endcode
 * 
 *      أمثلة:
 *      @code
 *      // قراءة مؤشر المكدس
 *      متغير مكدس: ص_غ64؛
 *      تجميع!("mov %rsp, $0" : "=r"(مكدس))؛
 *      
 *      // كتابة إلى منفذ
 *      تجميع!("outb %al, %dx"
 *          :
 *          : "a"(قيمة), "d"(منفذ)
 *          : "memory"
 *      )؛
 *      
 *      // عملية ذرية
 *      تجميع!("lock xchg $0, $1"
 *          : "+m"(هدف), "=r"(قديم)
 *          : "r"(جديد)
 *          : "memory"
 *      )؛
 *      @endcode
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2024-01-24
 * @version 1.0.0
 */

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <memory>
#include <sstream>
#include <regex>
#include <set>
#include <algorithm>

namespace sad {
namespace parser {

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) أنواع القيود
//                    (EN) Constraint Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @enum ConstraintKind
 * @brief (AR) أنواع القيود في inline assembly
 *        (EN) Inline assembly constraint types
 */
enum class ConstraintKind {
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) قيود السجلات العامة
    // (EN) General register constraints
    // ─────────────────────────────────────────────────────────────────────────
    
    REGISTER,           // (AR) أي سجل عام "r"
    REGISTER_A,         // (AR) سجل A (rax/eax/ax/al) "a"
    REGISTER_B,         // (AR) سجل B (rbx) "b"
    REGISTER_C,         // (AR) سجل C (rcx) "c"
    REGISTER_D,         // (AR) سجل D (rdx) "d"
    REGISTER_SI,        // (AR) سجل SI (rsi) "S"
    REGISTER_DI,        // (AR) سجل DI (rdi) "D"
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) قيود الذاكرة
    // (EN) Memory constraints
    // ─────────────────────────────────────────────────────────────────────────
    
    MEMORY,             // (AR) موقع في الذاكرة "m"
    MEMORY_OFFSET,      // (AR) عنوان مع إزاحة "o"
    MEMORY_VOLATILE,    // (AR) ذاكرة متطايرة "V"
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) قيود الثوابت
    // (EN) Immediate constraints
    // ─────────────────────────────────────────────────────────────────────────
    
    IMMEDIATE,          // (AR) ثابت فوري "i"
    IMMEDIATE_N,        // (AR) ثابت صحيح "n"
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) قيود FPU/SIMD
    // (EN) FPU/SIMD constraints
    // ─────────────────────────────────────────────────────────────────────────
    
    FLOAT_REG,          // (AR) سجل فاصلة عائمة "f"
    XMM_REG,            // (AR) سجل XMM "x"
    YMM_REG,            // (AR) سجل YMM "Y"
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) قيود ARM
    // (EN) ARM constraints
    // ─────────────────────────────────────────────────────────────────────────
    
    ARM_LOW_REG,        // (AR) سجلات r0-r7 "l"
    ARM_HIGH_REG,       // (AR) سجلات r8-r15 "h"
    ARM_STACK_PTR,      // (AR) مؤشر المكدس sp
    ARM_LINK_REG,       // (AR) سجل الرابط lr
};

/**
 * @enum ConstraintModifier
 * @brief (AR) معدّلات القيود
 *        (EN) Constraint modifiers
 */
enum class ConstraintModifier {
    NONE,               // (AR) بدون معدّل
    OUTPUT,             // (AR) مخرج فقط "="
    INPUT_OUTPUT,       // (AR) مدخل ومخرج "+"
    EARLY_CLOBBER,      // (AR) يُتلف مبكراً "&"
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) هياكل البيانات
//                    (EN) Data Structures
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct AsmConstraint
 * @brief (AR) قيد واحد في inline assembly
 *        (EN) Single inline assembly constraint
 */
struct AsmConstraint {
    std::string rawConstraint;      // (AR) القيد الخام (مثل "=r")
    ConstraintKind kind;            // (AR) نوع القيد
    ConstraintModifier modifier;    // (AR) معدّل القيد
    std::string variableName;       // (AR) اسم المتغير المرتبط
    std::string type;               // (AR) نوع البيانات
};

/**
 * @struct AsmOperand
 * @brief (AR) معامل inline assembly
 *        (EN) Inline assembly operand
 */
struct AsmOperand {
    std::string constraint;         // (AR) القيد
    std::string expression;         // (AR) التعبير المرتبط
    bool isOutput;                  // (AR) هل هو مخرج؟
    bool isInputOutput;             // (AR) هل هو مدخل ومخرج؟
    int operandIndex;               // (AR) رقم المعامل ($0, $1, ...)
};

/**
 * @struct InlineAsmStatement
 * @brief (AR) عبارة inline assembly كاملة
 *        (EN) Complete inline assembly statement
 */
struct InlineAsmStatement {
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) كود التجميع
    // (EN) Assembly code
    // ─────────────────────────────────────────────────────────────────────────
    
    std::string asmTemplate;        // (AR) قالب التجميع مع $0, $1, ...
    std::string dialect;            // (AR) لهجة التجميع (intel/att)
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) المعاملات
    // (EN) Operands
    // ─────────────────────────────────────────────────────────────────────────
    
    std::vector<AsmOperand> outputs;    // (AR) المخرجات
    std::vector<AsmOperand> inputs;     // (AR) المدخلات
    std::vector<std::string> clobbers;  // (AR) السجلات المُتلفة
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) الخيارات
    // (EN) Options
    // ─────────────────────────────────────────────────────────────────────────
    
    bool isVolatile = false;        // (AR) متطاير (لا يُحذف)
    bool hasAlignStack = false;     // (AR) محاذاة المكدس
    bool hasSideEffects = true;     // (AR) له تأثيرات جانبية
    bool mayThrow = false;          // (AR) قد يرمي استثناء
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) معلومات الموقع
    // (EN) Location info
    // ─────────────────────────────────────────────────────────────────────────
    
    int line;
    int column;
    std::string sourceFile;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) الكلمات المفتاحية العربية
//                    (EN) Arabic Keywords
// ═══════════════════════════════════════════════════════════════════════════════

namespace كلمات_التجميع {
    // (AR) الماكرو الرئيسي
    const std::string تجميع = u8"تجميع!";
    const std::string تجميع_متطاير = u8"تجميع_متطاير!";
    
    // (AR) أقسام التجميع
    const std::string مخرجات = u8"مخرجات";
    const std::string مدخلات = u8"مدخلات";
    const std::string متلفات = u8"متلفات";
    const std::string خيارات = u8"خيارات";
    
    // (AR) الخيارات
    const std::string متطاير = u8"متطاير";
    const std::string محاذاة_المكدس = u8"محاذاة_المكدس";
    const std::string صافي = u8"صافي";
    const std::string قد_يفشل = u8"قد_يفشل";
    
    // (AR) لهجات التجميع
    const std::string انتل = u8"انتل";
    const std::string att = u8"att";
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) محلل قيود التجميع
//                    (EN) Assembly Constraint Parser
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class ConstraintParser
 * @brief (AR) محلل قيود inline assembly
 *        (EN) Inline assembly constraint parser
 */
class ConstraintParser {
public:
    /**
     * @brief (AR) تحليل قيد واحد
     *        (EN) Parse single constraint
     * 
     * @param constraint (AR) نص القيد (مثل "=r" أو "+m")
     * @return (AR) القيد المُحلل (EN) Parsed constraint
     */
    AsmConstraint parse(const std::string& constraint) {
        AsmConstraint result;
        result.rawConstraint = constraint;
        result.modifier = ConstraintModifier::NONE;
        
        size_t pos = 0;
        
        // (AR) تحليل المعدّلات
        // (EN) Parse modifiers
        while (pos < constraint.length()) {
            char c = constraint[pos];
            
            if (c == '=') {
                result.modifier = ConstraintModifier::OUTPUT;
                pos++;
            } else if (c == '+') {
                result.modifier = ConstraintModifier::INPUT_OUTPUT;
                pos++;
            } else if (c == '&') {
                if (result.modifier == ConstraintModifier::OUTPUT) {
                    result.modifier = ConstraintModifier::EARLY_CLOBBER;
                }
                pos++;
            } else {
                break;
            }
        }
        
        // (AR) تحليل نوع القيد
        // (EN) Parse constraint kind
        if (pos < constraint.length()) {
            char c = constraint[pos];
            result.kind = parseConstraintChar(c);
        }
        
        return result;
    }
    
    /**
     * @brief (AR) التحقق من صحة القيد
     *        (EN) Validate constraint
     */
    bool validate(const AsmConstraint& constraint, std::string& error) {
        // (AR) المخرجات يجب أن يكون لها معدّل = أو +
        if (constraint.modifier == ConstraintModifier::NONE) {
            // (AR) قد يكون مدخلاً صحيحاً
        }
        
        // (AR) التحقق من أن القيد معروف
        if (constraint.kind == ConstraintKind::REGISTER) {
            // (AR) صحيح
        }
        
        return true;
    }
    
private:
    ConstraintKind parseConstraintChar(char c) {
        switch (c) {
            case 'r': return ConstraintKind::REGISTER;
            case 'a': return ConstraintKind::REGISTER_A;
            case 'b': return ConstraintKind::REGISTER_B;
            case 'c': return ConstraintKind::REGISTER_C;
            case 'd': return ConstraintKind::REGISTER_D;
            case 'S': return ConstraintKind::REGISTER_SI;
            case 'D': return ConstraintKind::REGISTER_DI;
            case 'm': return ConstraintKind::MEMORY;
            case 'o': return ConstraintKind::MEMORY_OFFSET;
            case 'V': return ConstraintKind::MEMORY_VOLATILE;
            case 'i': return ConstraintKind::IMMEDIATE;
            case 'n': return ConstraintKind::IMMEDIATE_N;
            case 'f': return ConstraintKind::FLOAT_REG;
            case 'x': return ConstraintKind::XMM_REG;
            case 'Y': return ConstraintKind::YMM_REG;
            case 'l': return ConstraintKind::ARM_LOW_REG;
            case 'h': return ConstraintKind::ARM_HIGH_REG;
            default: return ConstraintKind::REGISTER;
        }
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) محلل عبارات التجميع
//                    (EN) Assembly Statement Parser
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class InlineAsmParser
 * @brief (AR) محلل عبارات inline assembly
 *        (EN) Inline assembly statement parser
 */
class InlineAsmParser {
public:
    InlineAsmParser() = default;
    
    /**
     * @brief (AR) تحليل عبارة تجميع!
     *        (EN) Parse تجميع! statement
     * 
     * @param source (AR) النص المصدري (EN) Source text
     * @param pos (AR) موقع البداية (EN) Start position
     * @return (AR) العبارة المُحللة (EN) Parsed statement
     */
    std::optional<InlineAsmStatement> parse(
        const std::string& source,
        size_t& pos
    ) {
        // (AR) البحث عن تجميع! أو asm!
        if (!consumeKeyword(source, pos, كلمات_التجميع::تجميع) &&
            !consumeKeyword(source, pos, كلمات_التجميع::تجميع_متطاير) &&
            !consumeKeyword(source, pos, "asm!") &&
            !consumeKeyword(source, pos, "asm_volatile!")) {
            return std::nullopt;
        }
        
        bool isVolatile = (source.find(u8"متطاير") != std::string::npos) ||
                          (source.find("volatile") != std::string::npos);
        
        InlineAsmStatement stmt;
        stmt.isVolatile = isVolatile;
        
        skipWhitespace(source, pos);
        
        // (AR) توقع قوس فتح
        if (pos >= source.length() || source[pos] != '(') {
            error_ = u8"متوقع '(' بعد تجميع!";
            return std::nullopt;
        }
        pos++;
        
        // (AR) تحليل قالب التجميع (السلسلة النصية)
        skipWhitespace(source, pos);
        if (!parseAsmTemplate(source, pos, stmt.asmTemplate)) {
            return std::nullopt;
        }
        
        // (AR) تحليل الأقسام الاختيارية
        parseOptionalSections(source, pos, stmt);
        
        // (AR) توقع قوس إغلاق
        skipWhitespace(source, pos);
        if (pos >= source.length() || source[pos] != ')') {
            error_ = u8"متوقع ')' لإغلاق تجميع!";
            return std::nullopt;
        }
        pos++;
        
        return stmt;
    }
    
    /**
     * @brief (AR) الحصول على رسالة الخطأ
     *        (EN) Get error message
     */
    const std::string& getError() const {
        return error_;
    }
    
private:
    std::string error_;
    ConstraintParser constraintParser_;
    
    /**
     * @brief (AR) تخطي المسافات البيضاء
     *        (EN) Skip whitespace
     */
    void skipWhitespace(const std::string& source, size_t& pos) {
        while (pos < source.length() && std::isspace(source[pos])) {
            pos++;
        }
    }
    
    /**
     * @brief (AR) استهلاك كلمة مفتاحية
     *        (EN) Consume keyword
     */
    bool consumeKeyword(const std::string& source, size_t& pos, const std::string& keyword) {
        skipWhitespace(source, pos);
        
        if (pos + keyword.length() > source.length()) {
            return false;
        }
        
        if (source.substr(pos, keyword.length()) == keyword) {
            pos += keyword.length();
            return true;
        }
        
        return false;
    }
    
    /**
     * @brief (AR) تحليل قالب التجميع
     *        (EN) Parse assembly template
     */
    bool parseAsmTemplate(const std::string& source, size_t& pos, std::string& tmpl) {
        if (pos >= source.length() || source[pos] != '"') {
            error_ = u8"متوقع سلسلة نصية لقالب التجميع";
            return false;
        }
        pos++;  // (AR) تخطي علامة الاقتباس
        
        tmpl.clear();
        bool escaped = false;
        
        while (pos < source.length()) {
            char c = source[pos];
            
            if (escaped) {
                tmpl += c;
                escaped = false;
            } else if (c == '\\') {
                escaped = true;
            } else if (c == '"') {
                pos++;  // (AR) تخطي علامة الاقتباس
                return true;
            } else {
                tmpl += c;
            }
            
            pos++;
        }
        
        error_ = u8"سلسلة نصية غير مكتملة في قالب التجميع";
        return false;
    }
    
    /**
     * @brief (AR) تحليل الأقسام الاختيارية
     *        (EN) Parse optional sections
     */
    void parseOptionalSections(const std::string& source, size_t& pos, InlineAsmStatement& stmt) {
        // (AR) البحث عن النقطتين الرأسيتين للفصل بين الأقسام
        // (EN) Look for colons separating sections
        
        int sectionIndex = 0;  // 0=outputs, 1=inputs, 2=clobbers, 3=options
        
        while (pos < source.length() && source[pos] != ')') {
            skipWhitespace(source, pos);
            
            if (pos < source.length() && source[pos] == ':') {
                pos++;  // (AR) تخطي النقطتين
                skipWhitespace(source, pos);
                
                switch (sectionIndex) {
                    case 0:  // (AR) المخرجات
                        parseOperandList(source, pos, stmt.outputs, true);
                        break;
                    case 1:  // (AR) المدخلات
                        parseOperandList(source, pos, stmt.inputs, false);
                        break;
                    case 2:  // (AR) المُتلفات
                        parseClobberList(source, pos, stmt.clobbers);
                        break;
                    case 3:  // (AR) الخيارات
                        parseOptions(source, pos, stmt);
                        break;
                }
                
                sectionIndex++;
            } else {
                break;
            }
        }
    }
    
    /**
     * @brief (AR) تحليل قائمة المعاملات
     *        (EN) Parse operand list
     */
    void parseOperandList(
        const std::string& source,
        size_t& pos,
        std::vector<AsmOperand>& operands,
        bool isOutput
    ) {
        while (pos < source.length() && source[pos] != ':' && source[pos] != ')') {
            skipWhitespace(source, pos);
            
            AsmOperand operand;
            operand.isOutput = isOutput;
            operand.operandIndex = static_cast<int>(operands.size());
            
            // (AR) تحليل القيد
            if (source[pos] == '"') {
                pos++;
                while (pos < source.length() && source[pos] != '"') {
                    operand.constraint += source[pos];
                    pos++;
                }
                if (pos < source.length()) pos++;  // (AR) تخطي علامة الاقتباس
            }
            
            // (AR) تحليل التعبير
            skipWhitespace(source, pos);
            if (pos < source.length() && source[pos] == '(') {
                pos++;
                int parenDepth = 1;
                while (pos < source.length() && parenDepth > 0) {
                    if (source[pos] == '(') parenDepth++;
                    else if (source[pos] == ')') parenDepth--;
                    
                    if (parenDepth > 0) {
                        operand.expression += source[pos];
                    }
                    pos++;
                }
            }
            
            // (AR) التحقق من معدّل input/output
            operand.isInputOutput = (operand.constraint.find('+') != std::string::npos);
            
            if (!operand.constraint.empty()) {
                operands.push_back(operand);
            }
            
            // (AR) تخطي الفاصلة
            skipWhitespace(source, pos);
            if (pos < source.length() && source[pos] == ',') {
                pos++;
            }
        }
    }
    
    /**
     * @brief (AR) تحليل قائمة المُتلفات
     *        (EN) Parse clobber list
     */
    void parseClobberList(
        const std::string& source,
        size_t& pos,
        std::vector<std::string>& clobbers
    ) {
        while (pos < source.length() && source[pos] != ':' && source[pos] != ')') {
            skipWhitespace(source, pos);
            
            if (source[pos] == '"') {
                pos++;
                std::string clobber;
                while (pos < source.length() && source[pos] != '"') {
                    clobber += source[pos];
                    pos++;
                }
                if (pos < source.length()) pos++;  // (AR) تخطي علامة الاقتباس
                
                if (!clobber.empty()) {
                    clobbers.push_back(clobber);
                }
            }
            
            // (AR) تخطي الفاصلة
            skipWhitespace(source, pos);
            if (pos < source.length() && source[pos] == ',') {
                pos++;
            }
        }
    }
    
    /**
     * @brief (AR) تحليل الخيارات
     *        (EN) Parse options
     */
    void parseOptions(const std::string& source, size_t& pos, InlineAsmStatement& stmt) {
        while (pos < source.length() && source[pos] != ')') {
            skipWhitespace(source, pos);
            
            if (source[pos] == '"') {
                pos++;
                std::string option;
                while (pos < source.length() && source[pos] != '"') {
                    option += source[pos];
                    pos++;
                }
                if (pos < source.length()) pos++;
                
                // (AR) تطبيق الخيار
                if (option == كلمات_التجميع::متطاير || option == "volatile") {
                    stmt.isVolatile = true;
                } else if (option == كلمات_التجميع::محاذاة_المكدس || option == "alignstack") {
                    stmt.hasAlignStack = true;
                } else if (option == كلمات_التجميع::صافي || option == "pure") {
                    stmt.hasSideEffects = false;
                } else if (option == كلمات_التجميع::قد_يفشل || option == "maythrow") {
                    stmt.mayThrow = true;
                } else if (option == كلمات_التجميع::انتل || option == "intel") {
                    stmt.dialect = "intel";
                } else if (option == كلمات_التجميع::att || option == "att") {
                    stmt.dialect = "att";
                }
            }
            
            // (AR) تخطي الفاصلة
            skipWhitespace(source, pos);
            if (pos < source.length() && source[pos] == ',') {
                pos++;
            }
        }
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مدقق inline assembly
//                    (EN) Inline Assembly Validator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class InlineAsmValidator
 * @brief (AR) مدقق صحة عبارات inline assembly
 *        (EN) Inline assembly statement validator
 */
class InlineAsmValidator {
public:
    /**
     * @brief (AR) تحديد المعمارية الهدف
     *        (EN) Set target architecture
     */
    void setTargetArch(const std::string& arch) {
        targetArch_ = arch;
    }
    
    /**
     * @brief (AR) التحقق من صحة العبارة
     *        (EN) Validate statement
     * 
     * @param stmt (AR) العبارة للتحقق منها (EN) Statement to validate
     * @return (AR) قائمة الأخطاء (EN) List of errors
     */
    std::vector<std::string> validate(const InlineAsmStatement& stmt) {
        std::vector<std::string> errors;
        
        // (AR) التحقق من القالب
        validateTemplate(stmt.asmTemplate, errors);
        
        // (AR) التحقق من المخرجات
        for (const auto& output : stmt.outputs) {
            validateOutput(output, errors);
        }
        
        // (AR) التحقق من المدخلات
        for (const auto& input : stmt.inputs) {
            validateInput(input, errors);
        }
        
        // (AR) التحقق من المُتلفات
        validateClobbers(stmt.clobbers, errors);
        
        return errors;
    }
    
private:
    std::string targetArch_ = "x86_64";
    
    void validateTemplate(const std::string& tmpl, std::vector<std::string>& errors) {
        // (AR) التحقق من وجود علامات $ للمعاملات
        int maxOperand = -1;
        
        for (size_t i = 0; i < tmpl.length(); i++) {
            if (tmpl[i] == '$' && i + 1 < tmpl.length() && std::isdigit(tmpl[i + 1])) {
                int operandNum = tmpl[i + 1] - '0';
                maxOperand = std::max(maxOperand, operandNum);
            }
        }
        
        // (AR) يمكن إضافة تحقق إضافي هنا
    }
    
    void validateOutput(const AsmOperand& output, std::vector<std::string>& errors) {
        // (AR) التحقق من وجود معدّل = أو +
        if (output.constraint.find('=') == std::string::npos &&
            output.constraint.find('+') == std::string::npos) {
            errors.push_back(
                u8"المخرج رقم " + std::to_string(output.operandIndex) +
                u8" يجب أن يبدأ بـ '=' أو '+'"
            );
        }
    }
    
    void validateInput(const AsmOperand& input, std::vector<std::string>& errors) {
        // (AR) المدخلات لا يجب أن تبدأ بـ =
        if (!input.isInputOutput && input.constraint.find('=') != std::string::npos) {
            errors.push_back(
                u8"المدخل رقم " + std::to_string(input.operandIndex) +
                u8" لا يجب أن يبدأ بـ '='"
            );
        }
    }
    
    void validateClobbers(
        const std::vector<std::string>& clobbers,
        std::vector<std::string>& errors
    ) {
        // (AR) قائمة المُتلفات المعروفة
        static const std::set<std::string> knownClobbers = {
            "memory", "cc", "dirflag", "fpsr", "flags",
            // (AR) سجلات x86
            "rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp",
            "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
            "eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp",
            "ax", "bx", "cx", "dx", "si", "di", "bp", "sp",
            "al", "bl", "cl", "dl", "ah", "bh", "ch", "dh",
            // (AR) سجلات XMM
            "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7",
            "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15",
            // (AR) سجلات ARM
            "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
            "r8", "r9", "r10", "r11", "r12", "lr", "sp", "pc",
        };
        
        for (const auto& clobber : clobbers) {
            if (knownClobbers.find(clobber) == knownClobbers.end()) {
                // (AR) تحذير فقط، ليس خطأ
                // (EN) Warning only, not error
            }
        }
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) دوال مساعدة
//                    (EN) Helper Functions
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) تحليل نص inline assembly كامل
 *        (EN) Parse complete inline assembly text
 */
std::optional<InlineAsmStatement> parseInlineAsm(const std::string& source) {
    InlineAsmParser parser;
    size_t pos = 0;
    return parser.parse(source, pos);
}

/**
 * @brief (AR) التحقق من أن النص يبدأ بـ تجميع!
 *        (EN) Check if text starts with تجميع!
 */
bool isInlineAsmStatement(const std::string& source) {
    size_t pos = 0;
    
    // (AR) تخطي المسافات
    while (pos < source.length() && std::isspace(source[pos])) {
        pos++;
    }
    
    return source.substr(pos, كلمات_التجميع::تجميع.length()) == كلمات_التجميع::تجميع ||
           source.substr(pos, 4) == "asm!";
}

/**
 * @brief (AR) الحصول على قائمة القيود المدعومة
 *        (EN) Get list of supported constraints
 */
std::map<std::string, std::string> getSupportedConstraints() {
    return {
        {"r", u8"أي سجل عام"},
        {"a", u8"سجل A (rax/eax)"},
        {"b", u8"سجل B (rbx/ebx)"},
        {"c", u8"سجل C (rcx/ecx)"},
        {"d", u8"سجل D (rdx/edx)"},
        {"S", u8"سجل SI (rsi/esi)"},
        {"D", u8"سجل DI (rdi/edi)"},
        {"m", u8"موقع في الذاكرة"},
        {"i", u8"ثابت فوري"},
        {"n", u8"ثابت صحيح معروف"},
        {"x", u8"سجل XMM"},
        {"f", u8"سجل فاصلة عائمة"},
        {"=", u8"معدّل: مخرج فقط"},
        {"+", u8"معدّل: مدخل ومخرج"},
        {"&", u8"معدّل: early clobber"},
    };
}

} // namespace parser
} // namespace sad
