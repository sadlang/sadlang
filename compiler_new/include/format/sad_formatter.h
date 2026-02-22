// بسم الله الرحمن الرحيم
// ============================================================================
// sad_formatter.h — منسّق كود لغة ص المتقدم
// Advanced Code Formatter for Sad Language
// ============================================================================
//
// (AR) نظام تنسيق كود شامل يدعم 30+ قاعدة، ملفات إعدادات، فحص الفروقات،
//      ترتيب الاستيرادات، لفّ الأسطر الذكي، وأوضاع تنسيق متعددة.
//
// (EN) Comprehensive code formatting system with 30+ rules, config files,
//      diff checking, import sorting, smart line wrapping, and format profiles.
//
// ============================================================================

#ifndef SAD_FORMATTER_H
#define SAD_FORMATTER_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <filesystem>
#include <chrono>

namespace Sad {
namespace Format {

// ============================================================================
// تعدادات — Enumerations
// ============================================================================

/// (AR) نوع الرمز في المحلل المعجمي للمنسّق
/// (EN) Token type for the formatter's lexer
enum class FmtTokenType {
    KEYWORD,            ///< كلمة مفتاحية
    IDENTIFIER,         ///< مُعرِّف
    NUMBER,             ///< رقم (صحيح/عشري/ست عشري)
    STRING,             ///< نص (مفرد/مزدوج/خام)
    OPERATOR,           ///< عامل (+، -، *، ...)
    PUNCTUATION,        ///< علامة ترقيم ((، )، [، ]،...)
    COMMENT_LINE,       ///< تعليق سطري #
    COMMENT_BLOCK,      ///< تعليق كتلة #* ... *#
    COMMENT_DOC,        ///< تعليق توثيق ## أو #** ... **#
    WHITESPACE,         ///< مسافة بيضاء (مسافات/تابات)
    NEWLINE,            ///< سطر جديد
    SEMICOLON,          ///< فاصلة منقوطة ; أو ؛
    COMMA,              ///< فاصلة , أو ،
    ARROW,              ///< سهم الإرجاع ->
    COLON,              ///< نقطتان :
    DOT,                ///< نقطة .
    IMPORT_KEYWORD,     ///< كلمة استورد/من
    END_OF_FILE         ///< نهاية الملف
};

/// (AR) وضع التنسيق
/// (EN) Formatting profile
enum class FormatProfile {
    Compact,    ///< مضغوط: حد أدنى من المسافات
    Standard,   ///< قياسي: متوازن (الافتراضي)
    Verbose,    ///< مطوّل: مسافات واسعة وتعليقات محاذاة
    Custom      ///< مخصص: من ملف إعدادات
};

/// (AR) نوع نهاية السطر
/// (EN) Line ending style
enum class LineEnding {
    LF,     ///< \n (Unix/macOS)
    CRLF,   ///< \r\n (Windows)
    CR,     ///< \r (كلاسيكي)
    Auto    ///< تلقائي (حسب النظام)
};

/// (AR) نوع اقتباس النص
/// (EN) String quote style
enum class QuoteStyle {
    Double,     ///< "..."
    Single,     ///< '...'
    Preserve    ///< إبقاء الأصل
};

// ============================================================================
// الرمز — Token
// ============================================================================

/// (AR) رمز مفرد في نظام التنسيق
struct FmtToken {
    FmtTokenType type;
    std::string value;
    int line;       ///< رقم السطر (1-based)
    int column;     ///< رقم العمود (1-based)
    int offset;     ///< الإزاحة من بداية المصدر (0-based)

    bool isBlockOpener() const;
    bool isBlockCloser() const;
    bool isElseKeyword() const;
    bool isImport() const;
    bool isTypeKeyword() const;
};

// ============================================================================
// خيارات التنسيق — Formatter Options
// ============================================================================

/// (AR) إعدادات التنسيق الشاملة (30+ قاعدة)
/// (EN) Comprehensive formatting options (30+ rules)
struct FormatterOptions {
    // ====== المسافة البادئة ======
    int indentSize = 4;                     ///< حجم المسافة البادئة
    bool useSpaces = true;                  ///< مسافات بدل تاب
    bool indentCaseLabels = true;           ///< مسافة بادئة لعندما/افتراضي
    bool indentClassBody = true;            ///< مسافة بادئة لجسم الصنف

    // ====== طول السطر ======
    int maxLineLength = 100;                ///< الحد الأقصى لطول السطر
    bool wrapLongLines = true;              ///< لفّ الأسطر الطويلة تلقائياً
    int wrapIndent = 4;                     ///< مسافة إضافية للسطر الملفوف
    bool breakAfterOperator = false;        ///< الكسر بعد العامل (وليس قبله)
    bool breakBeforeComma = false;          ///< كسر قبل الفاصلة

    // ====== المسافات ======
    bool spaceAfterComma = true;            ///< مسافة بعد ,
    bool spaceAroundOperators = true;       ///< مسافة حول = + - * / ...
    bool spaceAfterKeywords = true;         ///< مسافة بعد إذا/بينما/لكل
    bool spaceAfterColon = true;            ///< مسافة بعد :
    bool spaceBeforeColon = false;          ///< مسافة قبل :
    bool spaceInsideParens = false;         ///< مسافة داخل ()
    bool spaceInsideBrackets = false;       ///< مسافة داخل []
    bool spaceAroundArrow = true;           ///< مسافة حول ->
    bool spaceAfterHash = true;             ///< مسافة بعد # في التعليقات
    bool spaceBeforeParenInCall = false;    ///< مسافة قبل ( في استدعاء دالة
    bool spaceBeforeParenInDef = true;      ///< مسافة قبل ( في تعريف دالة

    // ====== الأسطر الفارغة ======
    int blankLinesAfterImports = 1;         ///< أسطر فارغة بعد الاستيرادات
    int blankLinesBetweenFunctions = 1;     ///< أسطر فارغة بين الدوال
    int blankLinesBetweenClasses = 2;       ///< أسطر فارغة بين الأصناف
    int blankLinesBetweenMethods = 1;       ///< أسطر فارغة بين التوابع
    int maxConsecutiveBlankLines = 2;       ///< الحد الأقصى للأسطر الفارغة المتتالية
    bool blankLineBeforeReturn = false;     ///< سطر فارغ قبل ارجع
    bool blankLineAfterBlockOpen = false;   ///< سطر فارغ بعد فتح كتلة

    // ====== التنظيف ======
    bool trimTrailingWhitespace = true;     ///< حذف الفراغات الزائدة
    bool ensureFinalNewline = true;         ///< سطر أخير فارغ
    bool removeTrailingSemicolons = true;   ///< حذف الفواصل المنقوطة الزائدة
    bool normalizeArabicComma = false;      ///< تحويل ، إلى ,
    bool normalizeSemicolon = false;        ///< تحويل ؛ إلى ;

    // ====== الاستيرادات ======
    bool sortImports = true;                ///< ترتيب أبجدي
    bool groupImports = true;              ///< تجميع حسب المصدر
    bool removeUnusedImports = false;       ///< إزالة غير المستخدمة (يحتاج AST)

    // ====== أنماط ======
    QuoteStyle quoteStyle = QuoteStyle::Preserve;
    LineEnding lineEnding = LineEnding::Auto;
    FormatProfile profile = FormatProfile::Standard;

    // ====== تحميل/حفظ ======
    static FormatterOptions fromFile(const std::string& path);
    static FormatterOptions fromProfile(FormatProfile profile);
    void saveToFile(const std::string& path) const;

    /// (AR) تطبيق وضع مسبق
    void applyProfile(FormatProfile p);
};

// ============================================================================
// نتيجة التنسيق — Format Result
// ============================================================================

/// (AR) نتيجة عملية التنسيق
struct FormatResult {
    std::string output;             ///< الكود المنسّق
    bool changed = false;           ///< هل تغيّر الكود؟
    int linesChanged = 0;           ///< عدد الأسطر المتغيرة
    int totalLines = 0;             ///< إجمالي الأسطر
    double durationMs = 0.0;        ///< المدة بالمللي ثانية
    std::vector<std::string> diffs; ///< قائمة الفروقات
    std::string error;              ///< رسالة الخطأ (إن وجد)

    bool success() const { return error.empty(); }
};

/// (AR) إحصائيات التنسيق الكلية
struct FormatStats {
    int filesProcessed = 0;
    int filesChanged = 0;
    int totalLinesChanged = 0;
    double totalDurationMs = 0.0;
    std::vector<std::string> errors;
    std::vector<std::string> changedFiles;
};

// ============================================================================
// المحلل المعجمي للمنسّق — Formatter Lexer
// ============================================================================

/// (AR) محلل معجمي مصمم خصيصاً لتنسيق كود لغة ص
/// (EN) Lexer specifically designed for formatting Sad language code
class FmtLexer {
public:
    /// (AR) تقسيم المصدر إلى رموز
    std::vector<FmtToken> tokenize(const std::string& source);

private:
    std::string source_;
    size_t pos_ = 0;
    int line_ = 1;
    int column_ = 1;
    std::vector<FmtToken> tokens_;

    char current() const;
    char peek(int ahead = 1) const;
    void advance(int count = 1);
    bool match(const std::string& expected) const;

    bool isDigit(char c) const;
    bool isIdentStart(char c) const;
    bool isIdentChar(char c) const;
    bool isArabicByte(char c) const;

    void scanLineComment();
    void scanBlockComment();
    void scanDocComment();
    void scanString(char quote);
    void scanRawString();
    void scanNumber();
    void scanIdentifier();
    void scanWhitespace();
    void scanNewline();
    void scanOperator();

    void emit(FmtTokenType type, const std::string& value, int startLine, int startCol, int startOffset);
};

// ============================================================================
// منسّق الكود — Code Formatter
// ============================================================================

/// (AR) منسّق كود لغة ص المتقدم
/// (EN) Advanced Sad language code formatter
class SadFormatter {
public:
    explicit SadFormatter(const FormatterOptions& options = FormatterOptions());

    /// (AR) تنسيق نص مصدري
    FormatResult format(const std::string& source);

    /// (AR) تنسيق ملف (يكتب النتيجة)
    FormatResult formatFile(const std::string& path, bool dryRun = false);

    /// (AR) تنسيق مجلد بالكامل
    FormatStats formatDirectory(const std::string& dir, bool recursive = true, bool dryRun = false);

    /// (AR) فحص هل الكود منسّق (بدون تعديل)
    FormatResult check(const std::string& source);

    /// (AR) إنتاج فروقات unified diff
    std::string diff(const std::string& original, const std::string& formatted,
                     const std::string& filename = "<stdin>") const;

    /// (AR) الخيارات
    const FormatterOptions& options() const { return options_; }
    void setOptions(const FormatterOptions& opt) { options_ = opt; }

    /// (AR) تحميل إعدادات من المجلد الحالي (.تنسيق أو .sad-fmt)
    bool loadConfigFromDirectory(const std::string& dir);

private:
    FormatterOptions options_;

    // ====== مرحلة إعادة البناء ======
    std::string rebuild(const std::vector<FmtToken>& tokens);

    // ====== قواعد التنسيق ======
    std::string getIndent(int level) const;
    int computeIndentDelta(const FmtToken& token) const;
    bool shouldDeindentBefore(const FmtToken& token) const;
    bool isBlockOpenerKeyword(const std::string& value) const;
    bool isBlockCloserKeyword(const std::string& value) const;
    bool isInterBlockKeyword(const std::string& value) const;

    std::string formatOperator(const FmtToken& token, const FmtToken* prev, const FmtToken* next) const;
    std::string formatPunctuation(const FmtToken& token, const FmtToken* prev, const FmtToken* next) const;

    bool needsSpaceBefore(const FmtToken& token, const FmtToken* prev) const;
    bool needsSpaceAfter(const FmtToken& token, const FmtToken* next) const;

    // ====== الاستيرادات ======
    struct ImportLine { std::string text; std::string sortKey; int originalOrder; };
    std::string sortAndGroupImports(const std::string& source) const;
    bool isImportLine(const std::string& line) const;

    // ====== لفّ الأسطر ======
    std::string wrapLine(const std::string& line, int currentIndent) const;
    int findBreakPoint(const std::string& line, int maxCol) const;

    // ====== المساعدات ======
    std::string trimRight(const std::string& s) const;
    std::string normalizeLineEndings(const std::string& s) const;
    std::vector<std::string> splitLines(const std::string& s) const;
    std::string joinLines(const std::vector<std::string>& lines) const;
    int lineLength(const std::string& line) const;
};

// ============================================================================
// نقطة دخول CLI
// ============================================================================

/// (AR) نقطة دخول سطر الأوامر للمنسّق
int formatter_main(int argc, char* argv[]);

} // namespace Format
} // namespace Sad

#endif // SAD_FORMATTER_H
