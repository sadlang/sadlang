// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file generics_parser.hpp
 * @brief محلل المعممات - تحليل صيغة الأنواع المعممة
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *              محلل المعممات في لغة ص (Generics Parser)
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يحتوي على المحلل النحوي (Parser) للأنواع المعممة في لغة ص.
 * المعممات (Generics) تسمح بكتابة كود يعمل مع أنواع متعددة.
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                    الصيغة النحوية (Grammar)
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * معاملات_معممة ::= '<' قائمة_معاملات '>'
 * قائمة_معاملات ::= معامل ('،' معامل)*
 * معامل ::= اسم (':' قيود)?
 * قيود ::= قيد ('+' قيد)*
 * قيد ::= اسم_سمة
 * 
 * تخصيص_معممة ::= '<' قائمة_أنواع '>'
 * قائمة_أنواع ::= نوع ('،' نوع)*
 * نوع ::= نوع_بسيط | نوع_معمم
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                    أمثلة الاستخدام
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * مثال 1: تصريح دالة معممة
 * ```sad
 * دالة هوية<ت>(س: ت) -> ت {
 *     أرجع س
 * }
 * ```
 * 
 * مثال 2: بنية معممة
 * ```sad
 * بنية زوج<أ، ب> {
 *     الأول: أ،
 *     الثاني: ب
 * }
 * ```
 * 
 * مثال 3: معمم مع قيود
 * ```sad
 * دالة اطبع<ت: قابل_للعرض>(قيمة: ت) {
 *     اطبع(قيمة.عرض())
 * }
 * ```
 * 
 * مثال 4: قيود متعددة
 * ```sad
 * دالة قارن_واطبع<ت: قابل_للمقارنة + قابل_للعرض>(أ: ت، ب: ت) {
 *     إذا أ < ب {
 *         اطبع(أ)
 *     } وإلا {
 *         اطبع(ب)
 *     }
 * }
 * ```
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#ifndef SAD_SHARED_PARSER_GENERICS_PARSER_HPP
#define SAD_SHARED_PARSER_GENERICS_PARSER_HPP

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <variant>

namespace Sad {
namespace Parser {

// ═══════════════════════════════════════════════════════════════════════════
//                    التصريحات المسبقة
// ═══════════════════════════════════════════════════════════════════════════

class Token;
class Lexer;
class ParseError;

// ═══════════════════════════════════════════════════════════════════════════
//                    قيد المعامل / Parameter Constraint
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct GenericConstraint
 * @brief قيد على معامل معمم
 * 
 * القيد يحدد السمات التي يجب أن ينفذها النوع المعمم.
 * 
 * أمثلة:
 * - `قابل_للنسخ` - سمة واحدة
 * - `قابل_للمقارنة` - سمة أخرى
 */
struct GenericConstraint {
    std::string traitName;      // اسم السمة
    std::string location;       // موقع في الكود المصدري
    
    GenericConstraint() = default;
    explicit GenericConstraint(const std::string& name, const std::string& loc = "")
        : traitName(name), location(loc) {}
    
    /**
     * @brief تحويل القيد لنص
     */
    std::string toString() const { return traitName; }
};

// ═══════════════════════════════════════════════════════════════════════════
//                    معامل المعمم / Generic Parameter
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct GenericParameterAST
 * @brief تمثيل AST لمعامل معمم
 * 
 * معامل المعمم يحتوي على:
 * - اسم المعامل (مثل: ت، أ، ب)
 * - قائمة القيود (اختيارية)
 * 
 * أمثلة:
 * - `ت` - معامل بدون قيود
 * - `ت: قابل_للنسخ` - معامل بقيد واحد
 * - `ت: قابل_للنسخ + قابل_للمقارنة` - معامل بقيود متعددة
 */
struct GenericParameterAST {
    std::string name;                           // اسم المعامل
    std::vector<GenericConstraint> constraints; // قيود السمات
    std::string location;                       // موقع في الكود
    
    GenericParameterAST() = default;
    explicit GenericParameterAST(const std::string& n, const std::string& loc = "")
        : name(n), location(loc) {}
    
    /**
     * @brief هل المعامل مقيد؟
     */
    bool hasConstraints() const { return !constraints.empty(); }
    
    /**
     * @brief إضافة قيد
     */
    void addConstraint(const GenericConstraint& constraint) {
        constraints.push_back(constraint);
    }
    
    /**
     * @brief تحويل المعامل لنص
     */
    std::string toString() const {
        std::string result = name;
        if (!constraints.empty()) {
            result += ": ";
            for (size_t i = 0; i < constraints.size(); ++i) {
                if (i > 0) result += " + ";
                result += constraints[i].toString();
            }
        }
        return result;
    }
};

// ═══════════════════════════════════════════════════════════════════════════
//                    قائمة معاملات المعممات / Generic Parameters List
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct GenericParametersAST
 * @brief قائمة معاملات المعممات
 * 
 * تمثل القسم `<...>` في التصريحات المعممة.
 * 
 * أمثلة:
 * - `<ت>` - معامل واحد
 * - `<أ، ب>` - معاملين
 * - `<ت: قابل_للنسخ، س: قابل_للمقارنة>` - معاملين مع قيود
 */
struct GenericParametersAST {
    std::vector<GenericParameterAST> parameters;
    std::string location;
    
    GenericParametersAST() = default;
    
    /**
     * @brief هل القائمة فارغة؟
     */
    bool isEmpty() const { return parameters.empty(); }
    
    /**
     * @brief عدد المعاملات
     */
    size_t size() const { return parameters.size(); }
    
    /**
     * @brief إضافة معامل
     */
    void addParameter(const GenericParameterAST& param) {
        parameters.push_back(param);
    }
    
    /**
     * @brief البحث عن معامل بالاسم
     */
    const GenericParameterAST* findParameter(const std::string& name) const {
        for (const auto& param : parameters) {
            if (param.name == name) return &param;
        }
        return nullptr;
    }
    
    /**
     * @brief تحويل القائمة لنص
     */
    std::string toString() const {
        if (parameters.empty()) return "";
        std::string result = "<";
        for (size_t i = 0; i < parameters.size(); ++i) {
            if (i > 0) result += "، ";
            result += parameters[i].toString();
        }
        result += ">";
        return result;
    }
};

// ═══════════════════════════════════════════════════════════════════════════
//                    نوع معمم في AST / Generic Type AST
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct GenericTypeAST
 * @brief تمثيل نوع معمم في AST
 * 
 * يمثل استخدام نوع معمم مع وسائط نوع.
 * 
 * أمثلة:
 * - `قائمة<رقم>` - قائمة من الأرقام
 * - `خريطة<نص، رقم>` - خريطة من نص لرقم
 * - `نتيجة<بيانات، خطأ>` - نتيجة بنوعين
 */
struct GenericTypeAST {
    std::string baseName;                           // الاسم الأساسي (قائمة، خريطة، إلخ)
    std::vector<std::shared_ptr<GenericTypeAST>> typeArguments; // وسائط النوع
    std::string location;
    
    GenericTypeAST() = default;
    explicit GenericTypeAST(const std::string& name, const std::string& loc = "")
        : baseName(name), location(loc) {}
    
    /**
     * @brief هل النوع معمم؟
     */
    bool isGeneric() const { return !typeArguments.empty(); }
    
    /**
     * @brief هل النوع بسيط (غير معمم)؟
     */
    bool isSimple() const { return typeArguments.empty(); }
    
    /**
     * @brief عدد وسائط النوع
     */
    size_t arity() const { return typeArguments.size(); }
    
    /**
     * @brief إضافة وسيط نوع
     */
    void addTypeArgument(std::shared_ptr<GenericTypeAST> arg) {
        typeArguments.push_back(arg);
    }
    
    /**
     * @brief تحويل النوع لنص
     */
    std::string toString() const {
        std::string result = baseName;
        if (!typeArguments.empty()) {
            result += "<";
            for (size_t i = 0; i < typeArguments.size(); ++i) {
                if (i > 0) result += "، ";
                result += typeArguments[i]->toString();
            }
            result += ">";
        }
        return result;
    }
};

// ═══════════════════════════════════════════════════════════════════════════
//                    نتيجة التحليل / Parse Result
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct GenericParseError
 * @brief خطأ في تحليل المعممات
 */
struct GenericParseError {
    std::string message;        // رسالة الخطأ
    std::string location;       // موقع الخطأ
    std::string suggestion;     // اقتراح للإصلاح
    
    GenericParseError() = default;
    GenericParseError(const std::string& msg, const std::string& loc = "",
                      const std::string& sug = "")
        : message(msg), location(loc), suggestion(sug) {}
    
    /**
     * @brief تحويل الخطأ لنص عربي
     */
    std::string toArabicString() const {
        std::string result = "خطأ: " + message;
        if (!location.empty()) {
            result += " في " + location;
        }
        if (!suggestion.empty()) {
            result += " | اقتراح: " + suggestion;
        }
        return result;
    }
};

/**
 * @struct GenericParseResult
 * @brief نتيجة تحليل المعممات
 */
template<typename T>
struct GenericParseResult {
    bool success = false;
    T value;
    std::vector<GenericParseError> errors;
    
    static GenericParseResult<T> ok(const T& val) {
        GenericParseResult<T> result;
        result.success = true;
        result.value = val;
        return result;
    }
    
    static GenericParseResult<T> error(const GenericParseError& err) {
        GenericParseResult<T> result;
        result.success = false;
        result.errors.push_back(err);
        return result;
    }
    
    static GenericParseResult<T> error(const std::string& msg) {
        return error(GenericParseError(msg));
    }
};

// ═══════════════════════════════════════════════════════════════════════════
//                    محلل المعممات / Generics Parser
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @class GenericsParser
 * @brief المحلل النحوي للمعممات
 * 
 * هذا الصنف يقوم بتحليل تصريحات المعممات واستخداماتها في كود لغة ص.
 * 
 * الاستخدام:
 * ```cpp
 * GenericsParser parser;
 * auto result = parser.parseParameters("<ت: قابل_للنسخ>");
 * if (result.success) {
 *     // استخدم result.value
 * }
 * ```
 */
class GenericsParser {
public:
    GenericsParser();
    ~GenericsParser() = default;
    
    // ══════════════════════════════════════════════════════════════════════
    //                    تحليل المعاملات / Parse Parameters
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief تحليل قائمة معاملات المعممات
     * @param source النص المصدري (مثل: "<ت، س: قابل_للنسخ>")
     * @return نتيجة التحليل
     * 
     * مثال:
     * ```cpp
     * auto result = parser.parseParameters("<ت: قابل_للنسخ + قابل_للمقارنة>");
     * ```
     */
    GenericParseResult<GenericParametersAST> parseParameters(const std::string& source);
    
    /**
     * @brief تحليل معامل واحد
     * @param source النص المصدري (مثل: "ت: قابل_للنسخ")
     */
    GenericParseResult<GenericParameterAST> parseParameter(const std::string& source);
    
    /**
     * @brief تحليل قيود المعامل
     * @param source النص المصدري (مثل: "قابل_للنسخ + قابل_للمقارنة")
     */
    GenericParseResult<std::vector<GenericConstraint>> parseConstraints(const std::string& source);
    
    // ══════════════════════════════════════════════════════════════════════
    //                    تحليل الأنواع / Parse Types
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief تحليل نوع معمم
     * @param source النص المصدري (مثل: "قائمة<رقم>")
     * @return نتيجة التحليل
     * 
     * مثال:
     * ```cpp
     * auto result = parser.parseType("خريطة<نص، قائمة<رقم>>");
     * ```
     */
    GenericParseResult<GenericTypeAST> parseType(const std::string& source);
    
    /**
     * @brief تحليل قائمة وسائط النوع
     * @param source النص المصدري (مثل: "<رقم، نص>")
     */
    GenericParseResult<std::vector<std::shared_ptr<GenericTypeAST>>> 
    parseTypeArguments(const std::string& source);
    
    // ══════════════════════════════════════════════════════════════════════
    //                    التحقق / Validation
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief التحقق من صحة اسم المعامل
     * @param name الاسم للتحقق
     * @return هل الاسم صحيح؟
     * 
     * أسماء المعاملات الصحيحة:
     * - حرف عربي واحد (ت، س، ن، أ، ب، إلخ)
     * - كلمة عربية قصيرة
     * - T، U، V (للتوافق)
     */
    bool isValidParameterName(const std::string& name) const;
    
    /**
     * @brief التحقق من صحة اسم السمة
     */
    bool isValidTraitName(const std::string& name) const;
    
    /**
     * @brief التحقق من صحة اسم النوع
     */
    bool isValidTypeName(const std::string& name) const;
    
    // ══════════════════════════════════════════════════════════════════════
    //                    الإعدادات / Settings
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief تفعيل/تعطيل الرسائل العربية
     */
    void setArabicMessages(bool arabic) { arabicMessages_ = arabic; }
    
    /**
     * @brief هل الرسائل العربية مفعلة؟
     */
    bool isArabicMessages() const { return arabicMessages_; }

private:
    // ══════════════════════════════════════════════════════════════════════
    //                    دوال مساعدة داخلية
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief قراءة حرف عربي/Unicode
     */
    std::string readUnicodeChar(const std::string& source, size_t& pos);
    
    /**
     * @brief تخطي المسافات البيضاء
     */
    void skipWhitespace(const std::string& source, size_t& pos);
    
    /**
     * @brief قراءة معرّف (اسم)
     */
    std::string readIdentifier(const std::string& source, size_t& pos);
    
    /**
     * @brief هل الحرف بداية معرف؟
     */
    bool isIdentifierStart(const std::string& ch) const;
    
    /**
     * @brief هل الحرف جزء من معرف؟
     */
    bool isIdentifierPart(const std::string& ch) const;
    
    /**
     * @brief إنشاء رسالة خطأ
     */
    GenericParseError makeError(const std::string& msgAr, const std::string& msgEn,
                                const std::string& loc = "",
                                const std::string& suggestion = "");
    
    // البيانات الداخلية
    bool arabicMessages_ = true;
};

// ═══════════════════════════════════════════════════════════════════════════
//                    دوال مساعدة عامة
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief تحليل سريع لمعاملات المعممات
 */
inline GenericParseResult<GenericParametersAST> parseGenericParameters(const std::string& source) {
    GenericsParser parser;
    return parser.parseParameters(source);
}

/**
 * @brief تحليل سريع لنوع معمم
 */
inline GenericParseResult<GenericTypeAST> parseGenericType(const std::string& source) {
    GenericsParser parser;
    return parser.parseType(source);
}

/**
 * @brief هل النص يحتوي على معممات؟
 */
inline bool containsGenerics(const std::string& source) {
    return source.find('<') != std::string::npos &&
           source.find('>') != std::string::npos;
}

} // namespace Parser
} // namespace Sad

#endif // SAD_SHARED_PARSER_GENERICS_PARSER_HPP
