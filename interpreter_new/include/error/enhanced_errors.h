/**
 * @file enhanced_errors.h
 * @brief نظام رسائل الأخطاء المحسّنة مع وضع المعلم
 * @brief (EN) Enhanced error messages with Teacher Mode
 * 
 * @details يوفر رسائل خطأ واضحة بالعربية مع اقتراحات إصلاح دقيقة
 * 
 * @author أحمد
 * @date أكتوبر 2026
 */

#ifndef SAD_ENHANCED_ERRORS_H
#define SAD_ENHANCED_ERRORS_H

#include &lt;string&gt;
#include &lt;vector&gt;
#include &lt;map&gt;
#include &lt;memory&gt;
#include &lt;optional&gt;

namespace Sad {
namespace Errors {

/**
 * @brief مستوى خطورة الخطأ
 */
enum class Severity {
    HINT,       // تلميح
    INFO,       // معلومة
    WARNING,    // تحذير
    ERROR,      // خطأ
    FATAL       // خطأ فادح
};

/**
 * @brief نوع الخطأ (للتصنيف)
 */
enum class ErrorKind {
    // أخطاء معجمية
    LEXER_UNEXPECTED_CHAR,
    LEXER_UNTERMINATED_STRING,
    LEXER_INVALID_NUMBER,
    LEXER_INVALID_ESCAPE,
    
    // أخطاء نحوية
    PARSER_UNEXPECTED_TOKEN,
    PARSER_EXPECTED_TOKEN,
    PARSER_MISSING_END,
    PARSER_INVALID_EXPRESSION,
    PARSER_INVALID_STATEMENT,
    
    // أخطاء دلالية
    SEMANTIC_UNDEFINED_VARIABLE,
    SEMANTIC_UNDEFINED_FUNCTION,
    SEMANTIC_UNDEFINED_CLASS,
    SEMANTIC_TYPE_MISMATCH,
    SEMANTIC_DUPLICATE_DEFINITION,
    SEMANTIC_INVALID_OPERATION,
    SEMANTIC_INVALID_RETURN,
    SEMANTIC_INVALID_BREAK,
    
    // أخطاء وقت التشغيل
    RUNTIME_DIVISION_BY_ZERO,
    RUNTIME_INDEX_OUT_OF_BOUNDS,
    RUNTIME_NULL_REFERENCE,
    RUNTIME_STACK_OVERFLOW,
    RUNTIME_TYPE_ERROR,
    RUNTIME_ASSERTION_FAILED,
    
    // أخطاء عامة
    GENERIC_ERROR
};

/**
 * @brief موقع الخطأ في الكود المصدري
 */
struct ErrorLocation {
    std::string filename;   // اسم الملف
    int line;               // رقم السطر (1-based)
    int column;             // رقم العمود (1-based)
    int endLine;            // سطر النهاية
    int endColumn;          // عمود النهاية
    std::string sourceLine; // السطر المصدري
    
    ErrorLocation() : line(0), column(0), endLine(0), endColumn(0) {}
    
    ErrorLocation(const std::string&amp; file, int l, int c)
        : filename(file), line(l), column(c), endLine(l), endColumn(c) {}
    
    std::string toString() const {
        return filename + ":" + std::to_string(line) + ":" + std::to_string(column);
    }
};

/**
 * @brief اقتراح إصلاح
 */
struct FixSuggestion {
    std::string description;    // وصف الإصلاح
    std::string replacement;    // النص البديل
    ErrorLocation location;     // موقع الاستبدال
    double confidence;          // مستوى الثقة (0.0 - 1.0)
    
    FixSuggestion(const std::string&amp; desc, const std::string&amp; rep, 
                  const ErrorLocation&amp; loc, double conf = 1.0)
        : description(desc), replacement(rep), location(loc), confidence(conf) {}
};

/**
 * @brief ملاحظة إضافية مرتبطة بالخطأ
 */
struct ErrorNote {
    std::string message;
    std::optional&lt;ErrorLocation&gt; location;
    
    ErrorNote(const std::string&amp; msg) : message(msg) {}
    ErrorNote(const std::string&amp; msg, const ErrorLocation&amp; loc) 
        : message(msg), location(loc) {}
};

/**
 * @brief رسالة خطأ محسّنة كاملة
 */
class EnhancedError {
public:
    ErrorKind kind;
    Severity severity;
    std::string code;               // رمز الخطأ (مثل E0001)
    std::string title;              // العنوان القصير
    std::string message;            // الرسالة التفصيلية
    std::string explanation;        // شرح "وضع المعلم"
    ErrorLocation location;
    std::vector&lt;FixSuggestion&gt; suggestions;
    std::vector&lt;ErrorNote&gt; notes;
    std::vector&lt;std::string&gt; seeAlso;   // روابط مراجع
    
    EnhancedError(ErrorKind k, Severity s, const std::string&amp; c, 
                  const std::string&amp; t, const std::string&amp; m)
        : kind(k), severity(s), code(c), title(t), message(m) {}
    
    // إضافة اقتراح
    void addSuggestion(const FixSuggestion&amp; suggestion) {
        suggestions.push_back(suggestion);
    }
    
    // إضافة ملاحظة
    void addNote(const ErrorNote&amp; note) {
        notes.push_back(note);
    }
    
    // تعيين الشرح
    void setExplanation(const std::string&amp; exp) {
        explanation = exp;
    }
    
    // إضافة مرجع
    void addReference(const std::string&amp; ref) {
        seeAlso.push_back(ref);
    }
    
    // تنسيق الخطأ للعرض
    std::string format(bool useColors = true, bool teacherMode = true) const;
    
    // تنسيق JSON للأدوات
    std::string formatJson() const;
    
    // تنسيق LSP Diagnostic
    std::string formatLspDiagnostic() const;
};

/**
 * @brief مصنع رسائل الأخطاء المحسّنة
 * 
 * @details يوفر دوال مساعدة لإنشاء رسائل خطأ محددة مسبقاً
 */
class ErrorFactory {
public:
    //==========================================================================
    // أخطاء المتغيرات
    //==========================================================================
    
    /**
     * @brief متغير غير معرّف
     */
    static EnhancedError undefinedVariable(const std::string&amp; name, 
                                            const ErrorLocation&amp; loc,
                                            const std::vector&lt;std::string&gt;&amp; similar = {});
    
    /**
     * @brief إعادة تعريف متغير
     */
    static EnhancedError duplicateVariable(const std::string&amp; name,
                                            const ErrorLocation&amp; loc,
                                            const ErrorLocation&amp; firstDef);
    
    //==========================================================================
    // أخطاء الدوال
    //==========================================================================
    
    /**
     * @brief دالة غير معرّفة
     */
    static EnhancedError undefinedFunction(const std::string&amp; name,
                                            const ErrorLocation&amp; loc,
                                            const std::vector&lt;std::string&gt;&amp; similar = {});
    
    /**
     * @brief عدد وسائط خاطئ
     */
    static EnhancedError wrongArgumentCount(const std::string&amp; funcName,
                                             int expected, int actual,
                                             const ErrorLocation&amp; loc);
    
    /**
     * @brief نوع وسيط خاطئ
     */
    static EnhancedError wrongArgumentType(const std::string&amp; funcName,
                                            int argIndex,
                                            const std::string&amp; expected,
                                            const std::string&amp; actual,
                                            const ErrorLocation&amp; loc);
    
    //==========================================================================
    // أخطاء الأصناف
    //==========================================================================
    
    /**
     * @brief صنف غير معرّف
     */
    static EnhancedError undefinedClass(const std::string&amp; name,
                                         const ErrorLocation&amp; loc,
                                         const std::vector&lt;std::string&gt;&amp; similar = {});
    
    /**
     * @brief خاصية غير موجودة
     */
    static EnhancedError undefinedProperty(const std::string&amp; className,
                                            const std::string&amp; propName,
                                            const ErrorLocation&amp; loc,
                                            const std::vector&lt;std::string&gt;&amp; available = {});
    
    /**
     * @brief طريقة غير موجودة
     */
    static EnhancedError undefinedMethod(const std::string&amp; className,
                                          const std::string&amp; methodName,
                                          const ErrorLocation&amp; loc,
                                          const std::vector&lt;std::string&gt;&amp; available = {});
    
    //==========================================================================
    // أخطاء النحو
    //==========================================================================
    
    /**
     * @brief رمز غير متوقع
     */
    static EnhancedError unexpectedToken(const std::string&amp; found,
                                          const std::string&amp; expected,
                                          const ErrorLocation&amp; loc);
    
    /**
     * @brief كلمة نهاية مفقودة
     */
    static EnhancedError missingEnd(const std::string&amp; blockType,
                                     const ErrorLocation&amp; blockStart,
                                     const ErrorLocation&amp; loc);
    
    /**
     * @brief قوس مفقود
     */
    static EnhancedError missingParenthesis(char type, // '(' or ')'
                                             const ErrorLocation&amp; loc);
    
    //==========================================================================
    // أخطاء الأنواع
    //==========================================================================
    
    /**
     * @brief عدم تطابق الأنواع
     */
    static EnhancedError typeMismatch(const std::string&amp; expected,
                                       const std::string&amp; actual,
                                       const ErrorLocation&amp; loc);
    
    /**
     * @brief عملية غير صالحة على نوع
     */
    static EnhancedError invalidOperation(const std::string&amp; op,
                                           const std::string&amp; type,
                                           const ErrorLocation&amp; loc);
    
    //==========================================================================
    // أخطاء وقت التشغيل
    //==========================================================================
    
    /**
     * @brief قسمة على صفر
     */
    static EnhancedError divisionByZero(const ErrorLocation&amp; loc);
    
    /**
     * @brief فهرس خارج النطاق
     */
    static EnhancedError indexOutOfBounds(int index, int size,
                                           const ErrorLocation&amp; loc);
    
    /**
     * @brief مرجع فارغ
     */
    static EnhancedError nullReference(const std::string&amp; varName,
                                        const ErrorLocation&amp; loc);
    
    //==========================================================================
    // أدوات مساعدة
    //==========================================================================
    
    /**
     * @brief حساب التشابه بين سلسلتين (Levenshtein)
     */
    static int levenshteinDistance(const std::string&amp; s1, const std::string&amp; s2);
    
    /**
     * @brief إيجاد أسماء مشابهة
     */
    static std::vector&lt;std::string&gt; findSimilar(const std::string&amp; name,
                                                  const std::vector&lt;std::string&gt;&amp; candidates,
                                                  int maxResults = 3,
                                                  int maxDistance = 3);
};

/**
 * @brief مجمّع الأخطاء
 * 
 * @details يجمع كل الأخطاء والتحذيرات أثناء التحليل والتنفيذ
 */
class ErrorCollector {
private:
    std::vector&lt;EnhancedError&gt; errors_;
    std::vector&lt;EnhancedError&gt; warnings_;
    std::vector&lt;EnhancedError&gt; hints_;
    bool hasErrors_ = false;
    bool teacherMode_ = true;
    bool useColors_ = true;
    
public:
    /**
     * @brief إضافة خطأ
     */
    void addError(const EnhancedError&amp; error);
    
    /**
     * @brief إضافة تحذير
     */
    void addWarning(const EnhancedError&amp; warning);
    
    /**
     * @brief إضافة تلميح
     */
    void addHint(const EnhancedError&amp; hint);
    
    /**
     * @brief هل يوجد أخطاء؟
     */
    bool hasErrors() const { return hasErrors_; }
    
    /**
     * @brief عدد الأخطاء
     */
    size_t errorCount() const { return errors_.size(); }
    
    /**
     * @brief عدد التحذيرات
     */
    size_t warningCount() const { return warnings_.size(); }
    
    /**
     * @brief الحصول على كل الأخطاء
     */
    const std::vector&lt;EnhancedError&gt;&amp; getErrors() const { return errors_; }
    
    /**
     * @brief الحصول على كل التحذيرات
     */
    const std::vector&lt;EnhancedError&gt;&amp; getWarnings() const { return warnings_; }
    
    /**
     * @brief تفعيل/تعطيل وضع المعلم
     */
    void setTeacherMode(bool enabled) { teacherMode_ = enabled; }
    
    /**
     * @brief تفعيل/تعطيل الألوان
     */
    void setColors(bool enabled) { useColors_ = enabled; }
    
    /**
     * @brief طباعة كل الأخطاء
     */
    void printAll(std::ostream&amp; out) const;
    
    /**
     * @brief مسح كل الأخطاء
     */
    void clear();
    
    /**
     * @brief تصدير بصيغة JSON
     */
    std::string toJson() const;
};

//==============================================================================
// ألوان ANSI للطرفية
//==============================================================================

namespace Colors {
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    
    const std::string BG_RED = "\033[41m";
    const std::string BG_YELLOW = "\033[43m";
    
    // ألوان الأخطاء
    const std::string ERROR_COLOR = RED + BOLD;
    const std::string WARNING_COLOR = YELLOW + BOLD;
    const std::string HINT_COLOR = CYAN;
    const std::string NOTE_COLOR = BLUE;
    const std::string SUGGESTION_COLOR = GREEN;
}

//==============================================================================
// رموز الأخطاء
//==============================================================================

namespace ErrorCodes {
    // أخطاء معجمية (E01xx)
    const std::string UNEXPECTED_CHAR = "E0101";
    const std::string UNTERMINATED_STRING = "E0102";
    const std::string INVALID_NUMBER = "E0103";
    
    // أخطاء نحوية (E02xx)
    const std::string UNEXPECTED_TOKEN = "E0201";
    const std::string EXPECTED_TOKEN = "E0202";
    const std::string MISSING_END = "E0203";
    const std::string INVALID_EXPRESSION = "E0204";
    
    // أخطاء دلالية (E03xx)
    const std::string UNDEFINED_VARIABLE = "E0301";
    const std::string UNDEFINED_FUNCTION = "E0302";
    const std::string UNDEFINED_CLASS = "E0303";
    const std::string TYPE_MISMATCH = "E0304";
    const std::string DUPLICATE_DEFINITION = "E0305";
    
    // أخطاء وقت التشغيل (E04xx)
    const std::string DIVISION_BY_ZERO = "E0401";
    const std::string INDEX_OUT_OF_BOUNDS = "E0402";
    const std::string NULL_REFERENCE = "E0403";
    const std::string STACK_OVERFLOW = "E0404";
}

} // namespace Errors
} // namespace Sad

#endif // SAD_ENHANCED_ERRORS_H
