// بسم الله الرحمن الرحيم
// ============================================================================
// security_scanner.cpp — تنفيذ الحارس الأمني الشامل
// Comprehensive Security Scanner Implementation
// ============================================================================

#include "security_scanner.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <cassert>

namespace fs = std::filesystem;

namespace Sad {
namespace Security {

// ============================================================================
// Finding helpers
// ============================================================================

std::string Finding::severityIcon() const {
    switch (severity) {
        case Severity::Critical: return "🔴";
        case Severity::High:     return "🟠";
        case Severity::Medium:   return "🟡";
        case Severity::Low:      return "🔵";
        case Severity::Info:     return "⚪";
    }
    return "❔";
}

std::string Finding::severityNameAr() const {
    switch (severity) {
        case Severity::Critical: return "حرجة";
        case Severity::High:     return "عالية";
        case Severity::Medium:   return "متوسطة";
        case Severity::Low:      return "منخفضة";
        case Severity::Info:     return "معلومات";
    }
    return "غير محدد";
}

std::string Finding::categoryNameAr() const {
    switch (category) {
        case RuleCategory::Injection:       return "حقن";
        case RuleCategory::BufferOverflow:  return "فيض المخزن";
        case RuleCategory::InputValidation: return "التحقق من المدخلات";
        case RuleCategory::Cryptography:    return "التشفير";
        case RuleCategory::Secrets:         return "أسرار ثابتة";
        case RuleCategory::Permissions:     return "الصلاحيات";
        case RuleCategory::DataFlow:        return "تدفق البيانات";
        case RuleCategory::UnsafeBlock:     return "كتلة غير آمنة";
        case RuleCategory::Memory:          return "أمان الذاكرة";
        case RuleCategory::Configuration:   return "الإعدادات";
    }
    return "أخرى";
}

// ============================================================================
// ScanResult
// ============================================================================

int ScanResult::criticalCount() const {
    return (int)std::count_if(findings.begin(), findings.end(),
        [](const Finding& f) { return f.severity == Severity::Critical; });
}
int ScanResult::highCount() const {
    return (int)std::count_if(findings.begin(), findings.end(),
        [](const Finding& f) { return f.severity == Severity::High; });
}
int ScanResult::mediumCount() const {
    return (int)std::count_if(findings.begin(), findings.end(),
        [](const Finding& f) { return f.severity == Severity::Medium; });
}
int ScanResult::lowCount() const {
    return (int)std::count_if(findings.begin(), findings.end(),
        [](const Finding& f) { return f.severity == Severity::Low; });
}
int ScanResult::infoCount() const {
    return (int)std::count_if(findings.begin(), findings.end(),
        [](const Finding& f) { return f.severity == Severity::Info; });
}

// ============================================================================
// ScannerConfig
// ============================================================================

ScannerConfig ScannerConfig::fromFile(const std::string& path) {
    ScannerConfig cfg;
    std::ifstream file(path);
    if (!file.is_open()) return cfg;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);
        auto trim = [](std::string& s) {
            size_t a = s.find_first_not_of(" \t");
            size_t b = s.find_last_not_of(" \t\r\n");
            s = (a != std::string::npos) ? s.substr(a, b - a + 1) : "";
        };
        trim(key); trim(val);

        if (key == "verbose") cfg.verbose = (val == "true");
        else if (key == "critical_only") cfg.criticalOnly = (val == "true");
        else if (key == "auto_fix") cfg.autoFix = (val == "true");
        else if (key == "ci_mode") cfg.ciMode = (val == "true");
        else if (key == "taint_analysis") cfg.taintAnalysis = (val == "true");
        else if (key == "pattern_scan") cfg.patternScan = (val == "true");
        else if (key == "report_format") {
            if (val == "json") cfg.reportFormat = ReportFormat::JSON;
            else if (val == "html") cfg.reportFormat = ReportFormat::HTML;
            else if (val == "sarif") cfg.reportFormat = ReportFormat::SARIF;
            else cfg.reportFormat = ReportFormat::Text;
        }
        else if (key == "report_file") cfg.reportFile = val;
        else if (key == "min_severity") {
            if (val == "critical") cfg.minSeverity = Severity::Critical;
            else if (val == "high") cfg.minSeverity = Severity::High;
            else if (val == "medium") cfg.minSeverity = Severity::Medium;
            else cfg.minSeverity = Severity::Low;
        }
        else if (key == "disable_rule") cfg.disabledRules.push_back(val);
        else if (key == "enable_rule") cfg.enabledRules.push_back(val);
        else if (key == "exclude") cfg.excludePatterns.push_back(val);
    }
    return cfg;
}

// ============================================================================
// SecurityScanner — initialization
// ============================================================================

SecurityScanner::SecurityScanner(const ScannerConfig& config) : config_(config) {
    initializeRules();
}

void SecurityScanner::initializeRules() {
    rules_.clear();

    // === SQL001: حقن SQL ===
    rules_.push_back({
        "SQL001", "حقن SQL", "SQL Injection",
        RuleCategory::Injection, Severity::Critical,
        "بيانات غير منقّاة تُستخدم في استعلام SQL مباشرة",
        "Unsanitized data used directly in SQL query",
        "استخدم استعلامات معاملية (parameterized queries) بدلاً من دمج النصوص",
        "Use parameterized queries instead of string concatenation",
        {"نفّذ_SQL", "استعلم", "raw_query", "SELECT.*\\+", "INSERT.*\\+", "DELETE.*\\+", "UPDATE.*\\+"}
    });

    // === SQL002: تنفيذ SQL خام ===
    rules_.push_back({
        "SQL002", "استعلام خام", "Raw SQL Query",
        RuleCategory::Injection, Severity::High,
        "استعلام SQL خام بدون حماية",
        "Raw SQL query without protection",
        "استخدم ORM أو builder بدلاً من الاستعلامات الخام",
        "Use ORM or query builder instead of raw queries",
        {"raw_query", "نفّذ_خام"}
    });

    // === BUF001: فيض المخزن ===
    rules_.push_back({
        "BUF001", "فيض المخزن المحتمل", "Potential Buffer Overflow",
        RuleCategory::BufferOverflow, Severity::Critical,
        "عمليات على مصفوفات أو مخازن قد تسبب فيض",
        "Operations on arrays/buffers that may cause overflow",
        "تحقق من حدود المصفوفة قبل الوصول",
        "Check array bounds before access",
        {"مصفوفة.*\\[.*\\]", "احصل_على_عنصر", "buffer", "memcpy", "strcpy"}
    });

    // === BUF002: وصول خارج الحدود ===
    rules_.push_back({
        "BUF002", "وصول خارج الحدود", "Out-of-bounds Access",
        RuleCategory::BufferOverflow, Severity::High,
        "وصول لمؤشر ثابت في مصفوفة بدون فحص الحجم",
        "Fixed index access without size check",
        "استخدم التابع .احصل_آمن() أو تحقق من الطول أولاً",
        "Use .safe_get() or check length first",
        {"\\[\\d+\\]"}
    });

    // === INP001: إدخال بدون تحقق ===
    rules_.push_back({
        "INP001", "إدخال بدون تحقق", "Unvalidated Input",
        RuleCategory::InputValidation, Severity::High,
        "إدخال المستخدم يُستخدم مباشرة بدون تحقق",
        "User input used directly without validation",
        "تحقق من الإدخال ونقّه قبل الاستخدام",
        "Validate and sanitize input before use",
        {"اقرأ\\(", "اقرأ_سطر\\(", "احصل_على_معامل", "input\\("}
    });

    // === INP002: عدم تحقق من النوع ===
    rules_.push_back({
        "INP002", "عدم التحقق من النوع", "Missing Type Validation",
        RuleCategory::InputValidation, Severity::Medium,
        "تحويل نوع بيانات بدون التحقق من صلاحيته",
        "Type conversion without validity check",
        "استخدم حاول/امسك حول تحويلات الأنواع",
        "Wrap type conversions in try/catch",
        {"حوّل_إلى_رقم", "حوّل_إلى_عشري", "parseInt", "parseFloat"}
    });

    // === CRY001: خوارزمية تشفير ضعيفة ===
    rules_.push_back({
        "CRY001", "خوارزمية تشفير ضعيفة", "Weak Cryptographic Algorithm",
        RuleCategory::Cryptography, Severity::High,
        "استخدام خوارزمية تشفير معروفة بالضعف",
        "Use of known-weak cryptographic algorithm",
        "استخدم SHA-256 أو أعلى بدلاً من MD5/SHA1، واستخدم AES بدلاً من DES/RC4",
        "Use SHA-256+ instead of MD5/SHA1, use AES instead of DES/RC4",
        {"MD5", "SHA1", "SHA-1", "DES", "RC4", "md5", "sha1"}
    });

    // === CRY002: مفتاح تشفير ثابت ===
    rules_.push_back({
        "CRY002", "مفتاح تشفير ثابت", "Hardcoded Encryption Key",
        RuleCategory::Cryptography, Severity::Critical,
        "مفتاح تشفير مضمّن مباشرة في الكود",
        "Encryption key hardcoded directly in source",
        "استخدم متغيرات البيئة أو مخزن المفاتيح",
        "Use environment variables or key vault",
        {"مفتاح_تشفير.*=.*\"", "encryption_key.*=.*\"", "secret_key.*=.*\""}
    });

    // === SEC001: كتلة غير آمنة ===
    rules_.push_back({
        "SEC001", "كتلة غير آمنة", "Unsafe Block",
        RuleCategory::UnsafeBlock, Severity::Medium,
        "استخدام كتلة 'غير_آمن' تتجاوز فحص الأمان",
        "Use of 'unsafe' block bypasses safety checks",
        "قلّل استخدام الكتل غير الآمنة وأضف تعليقات توضح السبب",
        "Minimize unsafe blocks and document the reason",
        {"غير_آمن", "unsafe"}
    });

    // === SEC002: تنفيذ أوامر نظام ===
    rules_.push_back({
        "SEC002", "تنفيذ أوامر نظام", "System Command Execution",
        RuleCategory::Injection, Severity::Critical,
        "تنفيذ أوامر نظام قد يسمح بحقن الأوامر",
        "System command execution may allow command injection",
        "تجنب تنفيذ أوامر النظام، أو استخدم قائمة بيضاء صارمة للأوامر المسموحة",
        "Avoid system commands, or use strict whitelist",
        {"نفّذ\\(", "shell\\(", "system\\(", "exec\\(", "أمر_نظام"}
    });

    // === SEC003: أسرار ثابتة ===
    rules_.push_back({
        "SEC003", "أسرار ثابتة في الكود", "Hardcoded Secrets",
        RuleCategory::Secrets, Severity::Critical,
        "كلمة مرور أو مفتاح API أو رمز سري ثابت في الكود المصدري",
        "Password, API key, or secret token hardcoded in source",
        "استخدم متغيرات البيئة أو ملف .env أو مخزن الأسرار",
        "Use environment variables, .env file, or secret vault",
        {"password.*=.*\"", "كلمة_مرور.*=.*\"", "api_key.*=.*\"",
         "مفتاح_API.*=.*\"", "token.*=.*\"", "secret.*=.*\"",
         "رمز.*=.*\"", "سر.*=.*\""}
    });

    // === SEC004: وصول ملف بدون تحقق ===
    rules_.push_back({
        "SEC004", "وصول ملف غير آمن", "Unsafe File Access",
        RuleCategory::Injection, Severity::High,
        "فتح ملف باستخدام مسار من مدخلات المستخدم بدون تطبيع",
        "Opening file with user-provided path without normalization",
        "استخدم تطبيع_مسار() وتحقق_مسار() قبل فتح الملفات",
        "Use normalize_path() and validate_path() before opening files",
        {"افتح_ملف.*اقرأ", "افتح.*input", "open.*path"}
    });

    // === MEM001: تسرّب ذاكرة محتمل ===
    rules_.push_back({
        "MEM001", "تسرّب ذاكرة محتمل", "Potential Memory Leak",
        RuleCategory::Memory, Severity::Medium,
        "تخصيص ذاكرة بدون تحرير واضح",
        "Memory allocation without clear deallocation",
        "استخدم RAII أو المؤشرات الذكية",
        "Use RAII or smart pointers",
        {"خصّص\\(", "alloc\\(", "جديد\\s"}
    });

    // === MEM002: استخدام بعد التحرير ===
    rules_.push_back({
        "MEM002", "استخدام بعد التحرير", "Use After Free",
        RuleCategory::Memory, Severity::Critical,
        "الوصول لذاكرة بعد تحريرها",
        "Accessing memory after freeing it",
        "لا تستخدم مؤشرات بعد تحرير الذاكرة التي تشير إليها",
        "Don't use pointers after freeing the memory they point to",
        {"حرّر.*\\n.*استخدم", "free.*use"}
    });

    // === PERM001: صلاحيات واسعة ===
    rules_.push_back({
        "PERM001", "صلاحيات واسعة", "Overly Broad Permissions",
        RuleCategory::Permissions, Severity::Medium,
        "منح صلاحيات أوسع من اللازم",
        "Granting broader permissions than necessary",
        "اتبع مبدأ الحد الأدنى من الصلاحيات",
        "Follow principle of least privilege",
        {"الكل", "All", "777", "0777", "chmod.*777"}
    });

    // === XSS001: حقن النصوص عبر المواقع ===
    rules_.push_back({
        "XSS001", "حقن عبر المواقع (XSS)", "Cross-Site Scripting",
        RuleCategory::Injection, Severity::High,
        "إخراج بيانات المستخدم في HTML بدون ترميز",
        "User data output in HTML without encoding",
        "استخدم ترميز_HTML() أو escapeHtml() قبل عرض بيانات المستخدم",
        "Use html_encode() or escapeHtml() before rendering user data",
        {"اكتب_HTML.*\\+", "innerHTML.*=", "document\\.write", "render_template.*\\+"}
    });

    // === XSS002: تقييم كود ديناميكي ===
    rules_.push_back({
        "XSS002", "تقييم كود ديناميكي", "Dynamic Code Evaluation",
        RuleCategory::Injection, Severity::Critical,
        "تقييم كود من مصدر خارجي — خطر حقن شديد",
        "Evaluating code from external source — severe injection risk",
        "تجنب eval/قيّم — استخدم محلل آمن بدلاً من ذلك",
        "Avoid eval — use a safe parser instead",
        {"eval\\(", "قيّم\\(", "Function\\(.*\\+"}
    });

    // === SSRF001: تزوير طلبات جانب الخادم ===
    rules_.push_back({
        "SSRF001", "تزوير طلبات الخادم (SSRF)", "Server-Side Request Forgery",
        RuleCategory::Injection, Severity::High,
        "إرسال طلب HTTP/شبكة باستخدام عنوان URL من مدخلات المستخدم",
        "Sending HTTP/network request using user-provided URL",
        "تحقق من URL وامنع الوصول لعناوين داخلية (127.0.0.1, 10.x, 192.168.x)",
        "Validate URL and block access to internal addresses",
        {"fetch\\(.*\\+", "جلب\\(.*\\+", "أرسل_طلب\\(.*\\+", "http_get\\(.*\\+",
         "اتصل\\(.*\\+", "connect\\(.*\\+"}
    });

    // === PATH001: اختراق المسار ===
    rules_.push_back({
        "PATH001", "اختراق المسار", "Path Traversal",
        RuleCategory::Injection, Severity::High,
        "استخدام مسار ملف من المستخدم قد يسمح بالوصول لملفات خارج النطاق",
        "User-provided file path may allow access to files outside scope",
        "استخدم تطبيع_مسار() ومنع '../' وتحقق من المسار الأساسي",
        "Use normalize_path(), block '../', and validate base path",
        {"\\.\\./", "\\.\\.\\\\", "../", "..\\\\"}
    });

    // === DESER001: إلغاء تسلسل غير آمن ===
    rules_.push_back({
        "DESER001", "إلغاء تسلسل غير آمن", "Unsafe Deserialization",
        RuleCategory::Injection, Severity::Critical,
        "إلغاء تسلسل بيانات من مصدر غير موثوق قد يسمح بتنفيذ كود",
        "Deserializing untrusted data may allow code execution",
        "تحقق من البيانات قبل إلغاء التسلسل واستخدم صيغ محدودة (JSON)",
        "Validate data before deserialization and use limited formats (JSON)",
        {"حلّل_كائن\\(", "deserialize\\(", "unpickle\\(", "fromBytes\\(.*input"}
    });

    // === RACE001: حالة السباق ===
    rules_.push_back({
        "RACE001", "حالة سباق محتملة", "Potential Race Condition",
        RuleCategory::Memory, Severity::Medium,
        "وصول لمتغير مشترك بدون قفل في سياق متعدد الخيوط",
        "Shared variable access without lock in multi-threaded context",
        "استخدم قفل() أو mutex لحماية الوصول المشترك",
        "Use lock/mutex to protect shared access",
        {"مشترك.*=", "shared.*=.*thread", "global.*خيط", "بدون_قفل"}
    });
}

bool SecurityScanner::isRuleEnabled(const std::string& id) const {
    // إذا وُجدت قائمة بيضاء، يجب أن يكون فيها
    if (!config_.enabledRules.empty()) {
        bool found = false;
        for (const auto& r : config_.enabledRules) {
            if (r == id) { found = true; break; }
        }
        if (!found) return false;
    }
    // القائمة السوداء
    for (const auto& r : config_.disabledRules) {
        if (r == id) return false;
    }
    return true;
}

bool SecurityScanner::shouldExcludeLine(const std::string& line) const {
    // تعليقات التجاهل: // noguard, // حارس:تجاهل
    for (const auto& ex : config_.excludeLines) {
        if (line.find(ex) != std::string::npos) return true;
    }
    if (line.find("// noguard") != std::string::npos) return true;
    if (line.find("# noguard") != std::string::npos) return true;
    if (line.find("حارس:تجاهل") != std::string::npos) return true;
    return false;
}

void SecurityScanner::addCustomRule(const SecurityRule& rule) {
    rules_.push_back(rule);
}

// ============================================================================
// Scanning
// ============================================================================

ScanResult SecurityScanner::scanFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        ScanResult r;
        r.findings.push_back({"ERR", Severity::Info, RuleCategory::Configuration,
            "لا يمكن فتح الملف: " + path, path, 0, 0, "", "", "", 1.0, false, ""});
        return r;
    }
    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return scanSource(source, path);
}

ScanResult SecurityScanner::scanDirectory(const std::string& dir, bool recursive) {
    ScanResult total;
    auto startTime = std::chrono::high_resolution_clock::now();

    try {
        auto scan = [&](const fs::path& p) {
            std::string ext = p.extension().string();
            if (ext != ".ص" && ext != ".sad") return;
            for (const auto& pat : config_.excludePatterns) {
                if (p.string().find(pat) != std::string::npos) return;
            }
            auto r = scanFile(p.string());
            total.filesScanned += r.filesScanned;
            total.totalLines += r.totalLines;
            for (auto& f : r.findings) total.findings.push_back(std::move(f));
        };

        if (recursive) {
            for (const auto& entry : fs::recursive_directory_iterator(dir)) {
                if (entry.is_regular_file()) scan(entry.path());
            }
        } else {
            for (const auto& entry : fs::directory_iterator(dir)) {
                if (entry.is_regular_file()) scan(entry.path());
            }
        }
    } catch (const fs::filesystem_error& e) {
        total.findings.push_back({"ERR", Severity::Info, RuleCategory::Configuration,
            std::string("خطأ في قراءة المجلد: ") + e.what(), dir, 0, 0, "", "", "", 1.0, false, ""});
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    total.durationSeconds = std::chrono::duration<double>(endTime - startTime).count();
    return total;
}

ScanResult SecurityScanner::scanSource(const std::string& source, const std::string& filename) {
    ScanResult result;
    auto startTime = std::chrono::high_resolution_clock::now();

    result.filesScanned = 1;

    // عد الأسطر
    int lineCount = 1;
    for (char c : source) if (c == '\n') lineCount++;
    result.totalLines = lineCount;

    if (config_.patternScan) {
        patternScan(source, filename, result.findings);
    }

    if (config_.taintAnalysis) {
        taintAnalysis(source, filename, result.findings);
    }

    // ترتيب النتائج حسب الخطورة ثم السطر
    std::sort(result.findings.begin(), result.findings.end(),
        [](const Finding& a, const Finding& b) {
            if (a.severity != b.severity) return (int)a.severity < (int)b.severity;
            return a.line < b.line;
        });

    // فلترة حسب الحد الأدنى
    if (config_.criticalOnly) {
        result.findings.erase(
            std::remove_if(result.findings.begin(), result.findings.end(),
                [](const Finding& f) { return f.severity != Severity::Critical; }),
            result.findings.end());
    } else {
        result.findings.erase(
            std::remove_if(result.findings.begin(), result.findings.end(),
                [this](const Finding& f) { return (int)f.severity > (int)config_.minSeverity; }),
            result.findings.end());
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    result.durationSeconds = std::chrono::duration<double>(endTime - startTime).count();
    return result;
}

// ============================================================================
// Pattern-based scanning
// ============================================================================


} // namespace Security
} // namespace Sad

