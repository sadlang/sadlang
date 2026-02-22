// بسم الله الرحمن الرحيم
// ============================================================================
// security_scanner.h — الحارس الأمني الشامل للغة ص
// Comprehensive Security Scanner for Sad Language
// ============================================================================
//
// (AR) يدمج: فحص الأنماط (Pattern-based) + تحليل تدفق البيانات (Taint)
//      + فحص القدرات (Capability) في نظام أمني موحد.
//      يكشف: SQL injection, Buffer overflow, Weak crypto, Hardcoded secrets,
//      XSS, Command injection, Path traversal, Insecure blocks.
//
// (EN) Merges: Pattern-based scanning + Taint analysis + Capability checking
//      into a unified security system.
//
// ============================================================================

#ifndef SAD_SECURITY_SCANNER_H
#define SAD_SECURITY_SCANNER_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <filesystem>
#include <chrono>

namespace Sad {
namespace Security {

// ============================================================================
// Enumerations
// ============================================================================

/// (AR) مستوى الخطورة
enum class Severity {
    Critical,   ///< حرجة — يجب الإصلاح فوراً
    High,       ///< عالية
    Medium,     ///< متوسطة
    Low,        ///< منخفضة
    Info        ///< معلوماتية
};

/// (AR) فئة القاعدة الأمنية
enum class RuleCategory {
    Injection,      ///< حقن (SQL, Command, XSS)
    BufferOverflow, ///< فيض المخزن
    InputValidation,///< التحقق من المدخلات
    Cryptography,   ///< التشفير الضعيف
    Secrets,        ///< أسرار ثابتة
    Permissions,    ///< صلاحيات خاطئة
    DataFlow,       ///< تدفق بيانات ملوّث
    UnsafeBlock,    ///< كتلة غير آمنة
    Memory,         ///< أمان الذاكرة
    Configuration   ///< الإعدادات
};

/// (AR) تنسيق تقرير الإخراج
enum class ReportFormat {
    Text,       ///< نص عادي
    JSON,       ///< JSON
    HTML,       ///< HTML تفاعلي
    SARIF       ///< SARIF (لأنظمة CI/CD)
};

// ============================================================================
// القاعدة الأمنية — Security Rule
// ============================================================================

/// (AR) تعريف قاعدة أمنية واحدة
struct SecurityRule {
    std::string id;             ///< مُعرِّف (مثال: SQL001)
    std::string nameAr;         ///< الاسم بالعربية
    std::string nameEn;         ///< الاسم بالإنجليزية
    RuleCategory category;      ///< الفئة
    Severity defaultSeverity;   ///< الخطورة الافتراضية
    std::string descriptionAr;  ///< الوصف بالعربية
    std::string descriptionEn;  ///< الوصف بالإنجليزية
    std::string fixSuggestionAr;///< اقتراح الإصلاح بالعربية
    std::string fixSuggestionEn;///< اقتراح الإصلاح بالإنجليزية
    std::vector<std::string> patterns; ///< أنماط البحث
    bool enabled = true;        ///< مفعّلة؟
};

// ============================================================================
// نتيجة فحص — Finding
// ============================================================================

/// (AR) نتيجة فحص أمني (ثغرة مكتشفة)
struct Finding {
    std::string ruleId;         ///< معرف القاعدة
    Severity severity;          ///< الخطورة
    RuleCategory category;      ///< الفئة
    std::string message;        ///< رسالة التحذير
    std::string file;           ///< الملف
    int line = 0;               ///< السطر
    int column = 0;             ///< العمود
    std::string codeLine;       ///< سطر الكود المعني
    std::string suggestion;     ///< اقتراح الإصلاح
    std::string attackExample;  ///< مثال الهجوم
    double confidence = 1.0;    ///< نسبة الثقة (0.0 - 1.0)
    bool autoFixable = false;   ///< قابل للإصلاح التلقائي?
    std::string autoFixCode;    ///< الكود المُصلح

    std::string severityIcon() const;
    std::string severityNameAr() const;
    std::string categoryNameAr() const;
};

// ============================================================================
// إعدادات الفحص — Scanner Config
// ============================================================================

/// (AR) إعدادات الحارس الأمني
struct ScannerConfig {
    // قواعد
    std::vector<std::string> enabledRules;      ///< قائمة بيضاء (فارغة = الكل)
    std::vector<std::string> disabledRules;     ///< قائمة سوداء
    Severity minSeverity = Severity::Low;       ///< الحد الأدنى للخطورة

    // مخرجات
    ReportFormat reportFormat = ReportFormat::Text;
    std::string reportFile;                     ///< ملف التقرير
    bool verbose = false;
    bool criticalOnly = false;
    bool jsonOutput = false;

    // سلوك
    bool autoFix = false;                       ///< إصلاح تلقائي
    bool ciMode = false;                        ///< وضع CI (يفشل عند وجود ثغرات)
    bool taintAnalysis = true;                  ///< تحليل تلوّث البيانات
    bool patternScan = true;                    ///< فحص الأنماط

    // استبعاد
    std::vector<std::string> excludePatterns;   ///< أنماط ملفات مستبعدة
    std::vector<std::string> excludeLines;      ///< تعليقات تجاهل (مثل: // noguard)

    // تحميل
    static ScannerConfig fromFile(const std::string& path);
};

// ============================================================================
// نتيجة الفحص — ScanResult
// ============================================================================

/// (AR) نتيجة فحص أمني شاملة
struct ScanResult {
    std::vector<Finding> findings;
    int filesScanned = 0;
    int totalLines = 0;
    double durationSeconds = 0.0;

    int criticalCount() const;
    int highCount() const;
    int mediumCount() const;
    int lowCount() const;
    int infoCount() const;
    bool hasCritical() const { return criticalCount() > 0; }
    bool passed() const { return criticalCount() == 0 && highCount() == 0; }
};

// ============================================================================
// الماسح الأمني — SecurityScanner
// ============================================================================

/// (AR) الحارس الأمني الشامل
/// (EN) Comprehensive security scanner integrating pattern and taint analysis
class SecurityScanner {
public:
    explicit SecurityScanner(const ScannerConfig& config = ScannerConfig());

    /// (AR) فحص ملف واحد
    ScanResult scanFile(const std::string& path);

    /// (AR) فحص مجلد
    ScanResult scanDirectory(const std::string& dir, bool recursive = true);

    /// (AR) فحص كود مصدري مباشرة
    ScanResult scanSource(const std::string& source, const std::string& filename = "<source>");

    /// (AR) الإعدادات
    const ScannerConfig& config() const { return config_; }
    void setConfig(const ScannerConfig& cfg) { config_ = cfg; }

    /// (AR) إضافة قاعدة مخصصة
    void addCustomRule(const SecurityRule& rule);

    /// (AR) الحصول على جميع القواعد
    const std::vector<SecurityRule>& rules() const { return rules_; }

private:
    ScannerConfig config_;
    std::vector<SecurityRule> rules_;

    void initializeRules();
    bool isRuleEnabled(const std::string& id) const;
    bool shouldExcludeLine(const std::string& line) const;

    // فحص الأنماط
    void patternScan(const std::string& source, const std::string& file, std::vector<Finding>& findings);
    void checkSQLInjection(const std::string& line, int lineNum, const std::string& file, std::vector<Finding>& findings);
    void checkBufferOverflow(const std::string& line, int lineNum, const std::string& file, std::vector<Finding>& findings);
    void checkInputValidation(const std::string& line, int lineNum, const std::string& file, std::vector<Finding>& findings);
    void checkWeakCrypto(const std::string& line, int lineNum, const std::string& file, std::vector<Finding>& findings);
    void checkHardcodedSecrets(const std::string& line, int lineNum, const std::string& file, std::vector<Finding>& findings);
    void checkUnsafeBlocks(const std::string& line, int lineNum, const std::string& file, std::vector<Finding>& findings);
    void checkPermissions(const std::string& line, int lineNum, const std::string& file, std::vector<Finding>& findings);
    void checkMemorySafety(const std::string& line, int lineNum, const std::string& file, std::vector<Finding>& findings);

    // تحليل تلوّث البيانات
    void taintAnalysis(const std::string& source, const std::string& file, std::vector<Finding>& findings);
};

// ============================================================================
// مُنسّق التقارير — ReportFormatter
// ============================================================================

/// (AR) تنسيق تقارير الفحص الأمني
class SecurityReportFormatter {
public:
    /// (AR) تنسيق نصي ملوّن
    static std::string formatText(const ScanResult& result, bool verbose = false);

    /// (AR) تنسيق JSON
    static std::string formatJSON(const ScanResult& result);

    /// (AR) تنسيق HTML
    static std::string formatHTML(const ScanResult& result, const std::string& title = "");

    /// (AR) تنسيق SARIF
    static std::string formatSARIF(const ScanResult& result);

    /// (AR) كتابة التقرير إلى ملف
    static bool writeReport(const ScanResult& result, const std::string& path, ReportFormat format);
};

// ============================================================================
// نقطة دخول CLI
// ============================================================================

/// (AR) نقطة دخول سطر الأوامر للحارس الأمني
int guard_main(int argc, char* argv[]);

} // namespace Security
} // namespace Sad

#endif // SAD_SECURITY_SCANNER_H
