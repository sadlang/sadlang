// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file constraint_parser.hpp
 * @brief محلل القيود العربية - تحليل صيغة "حيث"
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *              محلل القيود في لغة ص (Constraint Parser)
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يحتوي على المحلل النحوي لصيغة القيود العربية.
 * القيود تُستخدم لتحديد متطلبات الأنواع المعممة.
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                    الصيغة النحوية (Grammar)
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * جملة_حيث ::= 'حيث' قائمة_قيود
 * قائمة_قيود ::= قيد ('،' قيد)*
 * قيد ::= معامل ':' قائمة_سمات
 * قائمة_سمات ::= سمة ('+' سمة)*
 * سمة ::= اسم_سمة | سمة_معممة
 * سمة_معممة ::= اسم_سمة '<' قائمة_أنواع '>'
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                    أمثلة الاستخدام
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * مثال 1: قيد بسيط
 * ```sad
 * دالة اطبع<ت>(قيمة: ت) حيث ت: قابل_للعرض { ... }
 * ```
 * 
 * مثال 2: قيود متعددة
 * ```sad
 * دالة قارن<ت>(أ: ت، ب: ت) حيث ت: قابل_للمقارنة + قابل_للترتيب { ... }
 * ```
 * 
 * مثال 3: قيود على معاملات متعددة
 * ```sad
 * دالة حوّل<أ، ب>(قيمة: أ) -> ب 
 *     حيث أ: قابل_للعرض، ب: من<نص> { ... }
 * ```
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#ifndef SAD_SHARED_PARSER_CONSTRAINT_PARSER_HPP
#define SAD_SHARED_PARSER_CONSTRAINT_PARSER_HPP

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>

namespace Sad {
namespace Parser {

// ═══════════════════════════════════════════════════════════════════════════
//                    تمثيل القيد / Constraint Representation
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct TraitBound
 * @brief قيد سمة على معامل معمم
 */
struct TraitBound {
    std::string traitName;                          // اسم السمة
    std::vector<std::string> typeArguments;         // وسائط النوع (للسمات المعممة)
    std::string location;                           // موقع في الكود المصدري
    
    TraitBound() = default;
    explicit TraitBound(const std::string& name) : traitName(name) {}
    
    bool isGeneric() const { return !typeArguments.empty(); }
    
    std::string toString() const {
        std::string result = traitName;
        if (!typeArguments.empty()) {
            result += "<";
            for (size_t i = 0; i < typeArguments.size(); ++i) {
                if (i > 0) result += "، ";
                result += typeArguments[i];
            }
            result += ">";
        }
        return result;
    }
};

/**
 * @struct WhereConstraint
 * @brief قيد في جملة "حيث"
 */
struct WhereConstraint {
    std::string parameterName;          // اسم المعامل المُقيَّد
    std::vector<TraitBound> bounds;     // قيود السمات
    std::string location;
    
    WhereConstraint() = default;
    explicit WhereConstraint(const std::string& param) : parameterName(param) {}
    
    void addBound(const TraitBound& bound) {
        bounds.push_back(bound);
    }
    
    std::string toString() const {
        std::string result = parameterName + ": ";
        for (size_t i = 0; i < bounds.size(); ++i) {
            if (i > 0) result += " + ";
            result += bounds[i].toString();
        }
        return result;
    }
};

/**
 * @struct WhereClause
 * @brief جملة "حيث" كاملة
 */
struct WhereClause {
    std::vector<WhereConstraint> constraints;
    std::string location;
    
    WhereClause() = default;
    
    bool isEmpty() const { return constraints.empty(); }
    size_t size() const { return constraints.size(); }
    
    void addConstraint(const WhereConstraint& constraint) {
        constraints.push_back(constraint);
    }
    
    const WhereConstraint* findConstraint(const std::string& paramName) const {
        for (const auto& c : constraints) {
            if (c.parameterName == paramName) return &c;
        }
        return nullptr;
    }
    
    std::string toString() const {
        if (constraints.empty()) return "";
        std::string result = "حيث ";
        for (size_t i = 0; i < constraints.size(); ++i) {
            if (i > 0) result += "، ";
            result += constraints[i].toString();
        }
        return result;
    }
};

// ═══════════════════════════════════════════════════════════════════════════
//                    خطأ التحليل / Parse Error
// ═══════════════════════════════════════════════════════════════════════════

struct ConstraintParseError {
    std::string messageAr;      // رسالة عربية
    std::string messageEn;      // رسالة إنجليزية
    std::string location;
    std::string suggestion;     // اقتراح للإصلاح
    
    ConstraintParseError() = default;
    ConstraintParseError(const std::string& ar, const std::string& en,
                        const std::string& loc = "", const std::string& sug = "")
        : messageAr(ar), messageEn(en), location(loc), suggestion(sug) {}
    
    std::string getMessage(bool arabic = true) const {
        return arabic ? messageAr : messageEn;
    }
};

/**
 * @struct ConstraintParseResult
 * @brief نتيجة تحليل القيود
 */
template<typename T>
struct ConstraintParseResult {
    bool success = false;
    T value;
    std::vector<ConstraintParseError> errors;
    
    static ConstraintParseResult<T> ok(const T& val) {
        ConstraintParseResult<T> result;
        result.success = true;
        result.value = val;
        return result;
    }
    
    static ConstraintParseResult<T> error(const ConstraintParseError& err) {
        ConstraintParseResult<T> result;
        result.success = false;
        result.errors.push_back(err);
        return result;
    }
};

// ═══════════════════════════════════════════════════════════════════════════
//                    محلل القيود / Constraint Parser
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @class ConstraintParser
 * @brief المحلل النحوي لجمل "حيث"
 */
class ConstraintParser {
public:
    ConstraintParser();
    ~ConstraintParser() = default;
    
    // ══════════════════════════════════════════════════════════════════════
    //                    تحليل جملة حيث / Parse Where Clause
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief تحليل جملة "حيث" كاملة
     * @param source النص المصدري (مثل: "حيث ت: قابل_للنسخ")
     */
    ConstraintParseResult<WhereClause> parseWhereClause(const std::string& source);
    
    /**
     * @brief تحليل قيد واحد
     * @param source النص المصدري (مثل: "ت: قابل_للنسخ + قابل_للمقارنة")
     */
    ConstraintParseResult<WhereConstraint> parseConstraint(const std::string& source);
    
    /**
     * @brief تحليل قيد سمة
     * @param source النص المصدري (مثل: "قابل_للنسخ" أو "من<نص>")
     */
    ConstraintParseResult<TraitBound> parseTraitBound(const std::string& source);
    
    // ══════════════════════════════════════════════════════════════════════
    //                    التحقق / Validation
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief التحقق من وجود كلمة "حيث"
     */
    bool startsWithWhere(const std::string& source) const;
    
    /**
     * @brief التحقق من صحة اسم السمة
     */
    bool isValidTraitName(const std::string& name) const;
    
    // ══════════════════════════════════════════════════════════════════════
    //                    الإعدادات / Settings
    // ══════════════════════════════════════════════════════════════════════
    
    void setArabicMessages(bool arabic) { arabicMessages_ = arabic; }
    bool isArabicMessages() const { return arabicMessages_; }

private:
    bool arabicMessages_ = true;
    
    // دوال مساعدة
    void skipWhitespace(const std::string& source, size_t& pos);
    std::string readIdentifier(const std::string& source, size_t& pos);
    std::string readUnicodeChar(const std::string& source, size_t& pos);
    bool isIdentifierStart(const std::string& ch) const;
    bool isIdentifierPart(const std::string& ch) const;
    bool matchKeyword(const std::string& source, size_t& pos, const std::string& keyword);
    
    ConstraintParseError makeError(const std::string& ar, const std::string& en,
                                   const std::string& loc = "",
                                   const std::string& sug = "");
};

// ═══════════════════════════════════════════════════════════════════════════
//                    دوال مساعدة عامة
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief تحليل سريع لجملة "حيث"
 */
inline ConstraintParseResult<WhereClause> parseWhereClause(const std::string& source) {
    ConstraintParser parser;
    return parser.parseWhereClause(source);
}

/**
 * @brief هل النص يحتوي على جملة "حيث"؟
 */
inline bool containsWhereClause(const std::string& source) {
    // البحث عن كلمة "حيث" في UTF-8
    // حيث = D8 AD D9 8A D8 AB
    return source.find("\xD8\xAD\xD9\x8A\xD8\xAB") != std::string::npos;
}

} // namespace Parser
} // namespace Sad

#endif // SAD_SHARED_PARSER_CONSTRAINT_PARSER_HPP
