// بسم الله الرحمن الرحيم
/**
 * ══════════════════════════════════════════════════════════════════════════════
 * ملف: security_analyzer_provider.h
 * الوصف: محلل الأمان الثوري للغة ص
 * ══════════════════════════════════════════════════════════════════════════════
 * 
 * يوفر تحليلاً شاملاً للثغرات الأمنية:
 * - حقن الكود
 * - ثغرات التحقق
 * - كشف البيانات الحساسة
 * - التشفير الضعيف
 * - إدارة المصادقة
 * 
 * ══════════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include "../../include/lsp_protocol_types.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <regex>
#include <optional>
#include <chrono>

namespace sad {
namespace lsp {
namespace security {

// ══════════════════════════════════════════════════════════════════════════════
//  تعدادات الأمان
// ══════════════════════════════════════════════════════════════════════════════

/**
 * خطورة الثغرة الأمنية
 */
enum class VulnerabilitySeverity {
    Low,        // منخفضة
    Medium,     // متوسطة
    High,       // عالية
    Critical    // حرجة
};

/**
 * نوع الثغرة الأمنية (مبني على OWASP)
 */
enum class VulnerabilityType {
    // حقن
    Injection,              // حقن كود
    SQLInjection,           // حقن SQL
    CommandInjection,       // حقن أوامر
    CodeInjection,          // حقن كود برمجي
    
    // مصادقة
    BrokenAuthentication,   // مصادقة مكسورة
    WeakPassword,           // كلمة مرور ضعيفة
    HardcodedCredentials,   // بيانات اعتماد ثابتة
    InsecureTokens,         // رموز غير آمنة
    
    // كشف البيانات
    SensitiveDataExposure,  // كشف بيانات حساسة
    DataLeakage,            // تسرب بيانات
    InsecureLogging,        // تسجيل غير آمن
    
    // تشفير
    WeakCryptography,       // تشفير ضعيف
    InsecureRandom,         // عشوائية غير آمنة
    PlaintextStorage,       // تخزين نص واضح
    
    // تحقق المدخلات
    InputValidation,        // نقص التحقق من المدخلات
    PathTraversal,          // اختراق المسار
    XSS,                    // Cross-Site Scripting
    
    // إدارة الجلسات
    InsecureSession,        // جلسة غير آمنة
    SessionFixation,        // تثبيت الجلسة
    
    // التحكم في الوصول
    BrokenAccessControl,    // تحكم وصول مكسور
    PrivilegeEscalation,    // تصعيد الصلاحيات
    IDOR,                   // Insecure Direct Object Reference
    
    // التهيئة
    SecurityMisconfiguration,   // تهيئة أمنية خاطئة
    DebugModeEnabled,           // وضع التصحيح مفعل
    
    // أخرى
    InsecureDeserialization,    // تفكيك غير آمن
    SSRF,                       // Server-Side Request Forgery
    RaceCondition,              // حالة تسابق
    ResourceLeak,               // تسرب موارد
    UnsafeReflection            // انعكاس غير آمن
};

/**
 * فئة CWE (Common Weakness Enumeration)
 */
enum class CWECategory {
    CWE_20,     // Improper Input Validation
    CWE_74,     // Injection
    CWE_78,     // OS Command Injection
    CWE_89,     // SQL Injection
    CWE_94,     // Code Injection
    CWE_200,    // Information Exposure
    CWE_259,    // Hard-coded Password
    CWE_287,    // Authentication Issues
    CWE_311,    // Missing Encryption
    CWE_327,    // Use of Broken Crypto Algorithm
    CWE_330,    // Insufficient Randomness
    CWE_352,    // Cross-Site Request Forgery
    CWE_400,    // Resource Exhaustion
    CWE_502,    // Deserialization
    CWE_522,    // Insufficiently Protected Credentials
    CWE_601,    // URL Redirection
    CWE_732,    // Incorrect Permission Assignment
    CWE_798,    // Hard-coded Credentials
    CWE_918,    // Server-Side Request Forgery
    Unknown
};

// ══════════════════════════════════════════════════════════════════════════════
//  الهياكل الأساسية
// ══════════════════════════════════════════════════════════════════════════════

/**
 * موقع في الكود
 */
struct CodeLocation {
    int line;
    int column;
    int end_line;
    int end_column;
    std::string file_path;
};

/**
 * ثغرة أمنية
 */
struct Vulnerability {
    std::string id;
    VulnerabilityType type;
    VulnerabilitySeverity severity;
    CWECategory cwe;
    
    std::string title;
    std::string description;
    std::string impact;
    std::string remediation;
    
    CodeLocation location;
    std::string code_snippet;
    std::string vulnerable_code;
    
    // إصلاح مقترح
    bool has_fix;
    std::string fix_code;
    std::string fix_explanation;
    
    // معلومات إضافية
    double confidence;      // نسبة الثقة 0-1
    std::vector<std::string> references;
    std::vector<std::string> tags;
};

/**
 * قاعدة أمان
 */
struct SecurityRule {
    std::string id;
    std::string name;
    std::string description;
    VulnerabilitySeverity severity;
    VulnerabilityType type;
    CWECategory cwe;
    
    // نمط الكشف
    std::regex pattern;
    std::string pattern_str;
    
    // سياقات مستثناة
    std::vector<std::string> exclude_contexts;
    
    // هل مفعلة؟
    bool enabled = true;
};

/**
 * تدفق البيانات الموصومة
 */
struct TaintedDataFlow {
    std::string source;         // المصدر (مثل: مدخل المستخدم)
    std::string variable;       // المتغير الموصوم
    std::vector<CodeLocation> path;  // مسار التدفق
    std::string sink;           // الوجهة الخطرة
    VulnerabilityType potential_vuln;
};

/**
 * تقرير الأمان
 */
struct SecurityReport {
    std::string document_uri;
    std::chrono::milliseconds analysis_time;
    
    // الثغرات
    std::vector<Vulnerability> vulnerabilities;
    
    // إحصائيات
    int critical_count = 0;
    int high_count = 0;
    int medium_count = 0;
    int low_count = 0;
    
    // تصنيف المخاطر
    char risk_grade = 'A';  // A (آمن) إلى F (خطر)
    double risk_score = 0.0;  // 0-100
    
    // ملخص
    std::string summary;
    std::vector<std::string> recommendations;
    
    // تدفق البيانات
    std::vector<TaintedDataFlow> taint_flows;
};

// ══════════════════════════════════════════════════════════════════════════════
//  أنماط الكشف
// ══════════════════════════════════════════════════════════════════════════════

namespace patterns {

/**
 * نمط بيانات اعتماد ثابتة
 */
const std::vector<std::string> HARDCODED_CREDENTIALS_PATTERNS = {
    R"(كلمة_المرور\s*=\s*["'][^"']+["'])",
    R"(password\s*=\s*["'][^"']+["'])",
    R"(سر\s*=\s*["'][^"']+["'])",
    R"(secret\s*=\s*["'][^"']+["'])",
    R"(مفتاح_api\s*=\s*["'][^"']+["'])",
    R"(api_key\s*=\s*["'][^"']+["'])",
    R"(رمز\s*=\s*["'][^"']+["'])",
    R"(token\s*=\s*["'][^"']+["'])"
};

/**
 * نمط حقن SQL
 */
const std::vector<std::string> SQL_INJECTION_PATTERNS = {
    R"(استعلام\s*\(\s*["'][^"']*\s*\+)",
    R"(نفذ_sql\s*\(\s*["'][^"']*\s*\+)",
    R"(execute\s*\(\s*["'][^"']*\s*\+)"
};

/**
 * نمط حقن الأوامر
 */
const std::vector<std::string> COMMAND_INJECTION_PATTERNS = {
    R"(نفذ_أمر\s*\(\s*["'][^"']*\s*\+)",
    R"(shell\s*\(\s*["'][^"']*\s*\+)",
    R"(system\s*\(\s*["'][^"']*\s*\+)"
};

/**
 * نمط تسجيل معلومات حساسة
 */
const std::vector<std::string> SENSITIVE_LOGGING_PATTERNS = {
    R"(اطبع\s*\(\s*كلمة_المرور)",
    R"(سجل\s*\(\s*كلمة_المرور)",
    R"(اطبع\s*\(\s*.*سر)",
    R"(اطبع\s*\(\s*.*رمز)"
};

/**
 * نمط عشوائية ضعيفة
 */
const std::vector<std::string> WEAK_RANDOM_PATTERNS = {
    R"(عشوائي\s*\()",  // بدون بذرة آمنة
    R"(random\s*\()"
};

/**
 * نمط اختراق المسار
 */
const std::vector<std::string> PATH_TRAVERSAL_PATTERNS = {
    R"(افتح\s*\(\s*[^)]*\.\./)",
    R"(اقرأ_ملف\s*\(\s*[^)]*\.\./)",
    R"(افتح\s*\(\s*[^"']+\s*\+)",
    R"(اقرأ_ملف\s*\(\s*مدخل)"
};

} // namespace patterns

// ══════════════════════════════════════════════════════════════════════════════
//  محلل الأمان
// ══════════════════════════════════════════════════════════════════════════════

class SecurityAnalyzerProvider {
public:
    SecurityAnalyzerProvider();
    ~SecurityAnalyzerProvider();
    
    // ════════════════════════════════════════════════════════════════════════
    //  التحليل الرئيسي
    // ════════════════════════════════════════════════════════════════════════
    
    /**
     * تحليل أمني كامل
     */
    SecurityReport analyze(
        const std::string& document,
        const std::string& uri
    );
    
    /**
     * تحليل سريع للثغرات الحرجة فقط
     */
    std::vector<Vulnerability> quick_scan(
        const std::string& document
    );
    
    /**
     * تحليل منطقة محددة
     */
    std::vector<Vulnerability> analyze_range(
        const std::string& document,
        int start_line,
        int end_line
    );
    
    // ════════════════════════════════════════════════════════════════════════
    //  فحوصات محددة
    // ════════════════════════════════════════════════════════════════════════
    
    /**
     * البحث عن بيانات اعتماد ثابتة
     */
    std::vector<Vulnerability> find_hardcoded_credentials(
        const std::string& code
    );
    
    /**
     * البحث عن ثغرات الحقن
     */
    std::vector<Vulnerability> find_injection_vulnerabilities(
        const std::string& code
    );
    
    /**
     * البحث عن مشاكل التشفير
     */
    std::vector<Vulnerability> find_cryptography_issues(
        const std::string& code
    );
    
    /**
     * البحث عن مشاكل التحقق من المدخلات
     */
    std::vector<Vulnerability> find_input_validation_issues(
        const std::string& code
    );
    
    /**
     * البحث عن تسرب المعلومات
     */
    std::vector<Vulnerability> find_information_disclosure(
        const std::string& code
    );
    
    /**
     * تحليل تدفق البيانات الموصومة
     */
    std::vector<TaintedDataFlow> analyze_taint_flow(
        const std::string& code
    );
    
    // ════════════════════════════════════════════════════════════════════════
    //  الإصلاحات التلقائية
    // ════════════════════════════════════════════════════════════════════════
    
    /**
     * اقتراح إصلاح لثغرة
     */
    std::optional<std::string> suggest_fix(
        const Vulnerability& vuln,
        const std::string& context
    );
    
    /**
     * توليد كود آمن بديل
     */
    std::string generate_secure_alternative(
        const std::string& vulnerable_code,
        VulnerabilityType type
    );
    
    // ════════════════════════════════════════════════════════════════════════
    //  إدارة القواعد
    // ════════════════════════════════════════════════════════════════════════
    
    /**
     * إضافة قاعدة مخصصة
     */
    void add_rule(const SecurityRule& rule);
    
    /**
     * تفعيل/تعطيل قاعدة
     */
    void set_rule_enabled(const std::string& rule_id, bool enabled);
    
    /**
     * الحصول على كل القواعد
     */
    std::vector<SecurityRule> get_rules() const;
    
    // ════════════════════════════════════════════════════════════════════════
    //  الإعدادات
    // ════════════════════════════════════════════════════════════════════════
    
    void set_min_severity(VulnerabilitySeverity severity) {
        min_severity_ = severity;
    }
    
    void set_taint_analysis_enabled(bool enabled) {
        taint_analysis_enabled_ = enabled;
    }
    
    void set_confidence_threshold(double threshold) {
        confidence_threshold_ = threshold;
    }
    
private:
    // القواعد
    std::vector<SecurityRule> rules_;
    VulnerabilitySeverity min_severity_ = VulnerabilitySeverity::Low;
    bool taint_analysis_enabled_ = true;
    double confidence_threshold_ = 0.5;
    
    // مصادر موصومة
    std::set<std::string> taint_sources_;
    
    // حوض خطر (sinks)
    std::set<std::string> dangerous_sinks_;
    
    // أدوات داخلية
    void initialize_rules();
    void initialize_taint_sources();
    int get_line_number(const std::string& code, size_t pos);
    std::string extract_code_snippet(const std::string& code, int line, int context = 2);
    double calculate_confidence(const std::string& match, const std::string& context);
};

// ══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة
// ══════════════════════════════════════════════════════════════════════════════

std::string severity_to_string(VulnerabilitySeverity severity);
std::string severity_to_color(VulnerabilitySeverity severity);
std::string vulnerability_type_to_string(VulnerabilityType type);
std::string cwe_to_string(CWECategory cwe);
std::string get_cwe_url(CWECategory cwe);
std::string risk_grade_emoji(char grade);

} // namespace security
} // namespace lsp
} // namespace sad
