// بسم الله الرحمن الرحيم
// ============================================================================
// security_scanner.cpp — تنفيذ الحارس الأمني الشامل
// Comprehensive Security Scanner Implementation
// ============================================================================

#include "security/security_scanner.h"
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

void SecurityScanner::patternScan(const std::string& source, const std::string& file,
                                   std::vector<Finding>& findings) {
    std::istringstream stream(source);
    std::string line;
    int lineNum = 0;

    while (std::getline(stream, line)) {
        lineNum++;
        if (shouldExcludeLine(line)) continue;

        // تخطي التعليقات
        std::string trimmed = line;
        size_t firstNonSpace = trimmed.find_first_not_of(" \t");
        if (firstNonSpace != std::string::npos) trimmed = trimmed.substr(firstNonSpace);
        if (trimmed.empty() || trimmed[0] == '#') continue;

        checkSQLInjection(line, lineNum, file, findings);
        checkBufferOverflow(line, lineNum, file, findings);
        checkInputValidation(line, lineNum, file, findings);
        checkWeakCrypto(line, lineNum, file, findings);
        checkHardcodedSecrets(line, lineNum, file, findings);
        checkUnsafeBlocks(line, lineNum, file, findings);
        checkPermissions(line, lineNum, file, findings);
        checkMemorySafety(line, lineNum, file, findings);
    }
}

static bool containsPattern(const std::string& line, const std::string& pattern) {
    // (AR) محاولة regex إن كان يحتوي أحرف خاصة
    // NOTE(#24): (AR) يتم ترجمة/بناء التعبير النمطي في كل استدعاء بدون تخزين مؤقت (caching).
    //            يمكن إضافة LRU cache للتعبيرات المتكررة لتحسين الأداء.
    // NOTE(#24): (EN) Regex pattern is compiled on every call without caching.
    //            An LRU cache for frequently-used patterns would improve performance.
    // NOTE(#28): (AR) C++ std::regex لا يدعم مهلة زمنية (timeout) — تعبيرات نمطية معقدة
    //            قد تسبب تجمد (ReDoS). يمكن استخدام RE2 كبديل آمن.
    // NOTE(#28): (EN) C++ std::regex has no timeout support — complex patterns may cause
    //            catastrophic backtracking (ReDoS). Consider RE2 as a safe alternative.
    if (pattern.find('\\') != std::string::npos ||
        pattern.find('[') != std::string::npos ||
        pattern.find('*') != std::string::npos ||
        pattern.find('+') != std::string::npos ||
        pattern.find('.') != std::string::npos) {
        try {
            std::regex re(pattern, std::regex::icase);
            return std::regex_search(line, re);
        } catch (...) {
            return line.find(pattern) != std::string::npos;
        }
    }
    return line.find(pattern) != std::string::npos;
}

static Finding makeFinding(const SecurityRule& rule, const std::string& line,
                           int lineNum, const std::string& file) {
    Finding f;
    f.ruleId = rule.id;
    f.severity = rule.defaultSeverity;
    f.category = rule.category;
    f.message = "[" + rule.id + "] " + rule.nameAr;
    f.file = file;
    f.line = lineNum;
    f.codeLine = line;
    f.suggestion = rule.fixSuggestionAr;
    f.confidence = 0.8;
    return f;
}

// ============================================================================
// دالة فحص عامة بدلاً من 8 دوال متكررة
// Generic check function replacing 8 duplicate methods
// ============================================================================

/// (AR) فحص سطر واحد ضد جميع القواعد المفعّلة — نسخة محسّنة موحّدة
/// (EN) Check a single line against all enabled rules — unified optimized version
static void checkLineAgainstRules(const SecurityScanner* scanner,
                                   const std::vector<SecurityRule>& rules,
                                   const std::string& line, int lineNum,
                                   const std::string& file,
                                   std::vector<Finding>& findings,
                                   const ScannerConfig& config) {
    for (const auto& rule : rules) {
        if (!rule.enabled) continue;
        // فحص القوائم البيضاء/السوداء
        if (!config.enabledRules.empty()) {
            bool found = false;
            for (const auto& r : config.enabledRules) {
                if (r == rule.id) { found = true; break; }
            }
            if (!found) continue;
        }
        bool disabled = false;
        for (const auto& r : config.disabledRules) {
            if (r == rule.id) { disabled = true; break; }
        }
        if (disabled) continue;

        for (const auto& pat : rule.patterns) {
            if (containsPattern(line, pat)) {
                auto f = makeFinding(rule, line, lineNum, file);
                // تخصيص حسب الفئة
                if (rule.category == RuleCategory::Injection && rule.id.find("SQL") != std::string::npos) {
                    f.attackExample = "'; DROP TABLE users; --";
                }
                else if (rule.category == RuleCategory::Injection && rule.id.find("XSS") != std::string::npos) {
                    f.attackExample = "<script>alert('XSS')</script>";
                }
                else if (rule.category == RuleCategory::Injection && rule.id.find("CMD") != std::string::npos) {
                    f.attackExample = "; rm -rf / || del /f /s";
                    f.confidence = 0.85;
                }
                else if (rule.category == RuleCategory::Injection && rule.id.find("SSRF") != std::string::npos) {
                    f.attackExample = "http://169.254.169.254/latest/meta-data/";
                    f.confidence = 0.75;
                }
                else if (rule.category == RuleCategory::Injection && rule.id.find("PATH") != std::string::npos) {
                    f.attackExample = "../../../../etc/passwd";
                }
                else if (rule.category == RuleCategory::Injection && rule.id.find("DESER") != std::string::npos) {
                    f.attackExample = "بيانات مسلسلة خبيثة تنفذ كوداً";
                }
                else if (rule.category == RuleCategory::Secrets) {
                    f.confidence = 0.7; // أسرار ثابتة قد تكون false positive
                }
                findings.push_back(std::move(f));
                break;  // نمط واحد كافٍ لكل قاعدة
            }
        }
    }
}

// === التوافقية: الدوال القديمة تستدعي الدالة العامة ===
void SecurityScanner::checkSQLInjection(const std::string& line, int lineNum,
                                         const std::string& file, std::vector<Finding>& findings) {
    std::vector<SecurityRule> filtered;
    for (const auto& r : rules_) if (r.category == RuleCategory::Injection) filtered.push_back(r);
    checkLineAgainstRules(this, filtered, line, lineNum, file, findings, config_);
}

void SecurityScanner::checkBufferOverflow(const std::string& line, int lineNum,
                                           const std::string& file, std::vector<Finding>& findings) {
    std::vector<SecurityRule> filtered;
    for (const auto& r : rules_) if (r.category == RuleCategory::BufferOverflow) filtered.push_back(r);
    checkLineAgainstRules(this, filtered, line, lineNum, file, findings, config_);
}

void SecurityScanner::checkInputValidation(const std::string& line, int lineNum,
                                            const std::string& file, std::vector<Finding>& findings) {
    std::vector<SecurityRule> filtered;
    for (const auto& r : rules_) if (r.category == RuleCategory::InputValidation) filtered.push_back(r);
    checkLineAgainstRules(this, filtered, line, lineNum, file, findings, config_);
}

void SecurityScanner::checkWeakCrypto(const std::string& line, int lineNum,
                                       const std::string& file, std::vector<Finding>& findings) {
    std::vector<SecurityRule> filtered;
    for (const auto& r : rules_) if (r.category == RuleCategory::Cryptography) filtered.push_back(r);
    checkLineAgainstRules(this, filtered, line, lineNum, file, findings, config_);
}

void SecurityScanner::checkHardcodedSecrets(const std::string& line, int lineNum,
                                             const std::string& file, std::vector<Finding>& findings) {
    std::vector<SecurityRule> filtered;
    for (const auto& r : rules_) if (r.category == RuleCategory::Secrets) filtered.push_back(r);
    checkLineAgainstRules(this, filtered, line, lineNum, file, findings, config_);
}

void SecurityScanner::checkUnsafeBlocks(const std::string& line, int lineNum,
                                         const std::string& file, std::vector<Finding>& findings) {
    std::vector<SecurityRule> filtered;
    for (const auto& r : rules_) if (r.category == RuleCategory::UnsafeBlock) filtered.push_back(r);
    checkLineAgainstRules(this, filtered, line, lineNum, file, findings, config_);
}

void SecurityScanner::checkPermissions(const std::string& line, int lineNum,
                                        const std::string& file, std::vector<Finding>& findings) {
    std::vector<SecurityRule> filtered;
    for (const auto& r : rules_) if (r.category == RuleCategory::Permissions) filtered.push_back(r);
    checkLineAgainstRules(this, filtered, line, lineNum, file, findings, config_);
}

void SecurityScanner::checkMemorySafety(const std::string& line, int lineNum,
                                         const std::string& file, std::vector<Finding>& findings) {
    std::vector<SecurityRule> filtered;
    for (const auto& r : rules_) if (r.category == RuleCategory::Memory) filtered.push_back(r);
    checkLineAgainstRules(this, filtered, line, lineNum, file, findings, config_);
}

// ============================================================================
// Taint analysis
// ============================================================================

void SecurityScanner::taintAnalysis(const std::string& source, const std::string& file,
                                     std::vector<Finding>& findings) {
    // تحليل تلوّث متقدم: تتبع المتغيرات من المصادر عبر التنقية إلى المصارف
    static const std::vector<std::string> taintSources = {
        "اقرأ(", "اقرأ_سطر(", "احصل_على_معامل(",
        "احصل_على_رأس(", "احصل_على_كوكي(", "input(", "readline(",
        "احصل_على_جسم(", "request.body", "request.query",
        "fetch(", "جلب(", "اقرأ_ملف("
    };
    static const std::vector<std::string> taintSinks = {
        "نفّذ_SQL(", "استعلم(", "اكتب_HTML(",
        "نفّذ(", "shell(", "system(", "exec(",
        "افتح_ملف(", "حوّل_إلى(",
        "أرسل_بريد(", "redirect(", "أعد_توجيه(",
        "eval(", "قيّم(", "innerHTML"
    };
    // دوال التنقية — إذا مرّ المتغير عبرها يُزال التلوّث
    static const std::vector<std::string> sanitizers = {
        "نقّ(", "sanitize(", "ترميز_HTML(", "escapeHtml(",
        "تحقق(", "validate(", "فلتر(", "filter(",
        "escape(", "encode(", "تطبيع_مسار(", "normalize_path(",
        "كلمة_مرور_هاش(", "hash_password("
    };

    // المرحلة 1: اكتشاف المتغيرات الملوّثة
    std::map<std::string, int> taintedVars; // اسم -> رقم سطر المصدر
    std::map<std::string, bool> sanitizedVars; // هل تم تنقية المتغير؟
    std::istringstream stream(source);
    std::string line;
    int lineNum = 0;

    while (std::getline(stream, line)) {
        lineNum++;
        if (shouldExcludeLine(line)) continue;

        // البحث عن تعيينات من مصادر ملوّثة
        for (const auto& src : taintSources) {
            if (line.find(src) != std::string::npos) {
                // استخراج اسم المتغير: متغير اسم = مصدر()
                size_t eq = line.find('=');
                if (eq != std::string::npos && line.find(src) > eq) {
                    std::string leftSide = line.substr(0, eq);
                    // التخلص من "متغير" والمسافات
                    size_t varStart = leftSide.find_first_not_of(" \t");
                    if (varStart != std::string::npos) {
                        std::string varDecl = leftSide.substr(varStart);
                        // إزالة "متغير" إن وُجد
                        if (varDecl.find("متغير") == 0) {
                            size_t ws = varDecl.find(' ');
                            if (ws != std::string::npos) varDecl = varDecl.substr(ws + 1);
                        }
                        // إزالة المسافات
                        size_t s = varDecl.find_first_not_of(" \t");
                        size_t e = varDecl.find_last_not_of(" \t");
                        if (s != std::string::npos) {
                            std::string varName = varDecl.substr(s, e - s + 1);
                            taintedVars[varName] = lineNum;
                            sanitizedVars[varName] = false;
                        }
                    }
                }
            }
        }

        // المرحلة 1.5: كشف التنقية — إذا مرّ متغير ملوّث عبر دالة تنقية يُزال تلوّثه
        for (const auto& san : sanitizers) {
            if (line.find(san) != std::string::npos) {
                for (auto& [varName, srcLine] : taintedVars) {
                    if (line.find(varName) != std::string::npos) {
                        sanitizedVars[varName] = true;
                    }
                }
                // أيضاً فحص إذا كانت النتيجة تُعيّن لمتغير جديد
                size_t eq = line.find('=');
                if (eq != std::string::npos) {
                    std::string leftSide = line.substr(0, eq);
                    size_t s = leftSide.find_first_not_of(" \t");
                    size_t e = leftSide.find_last_not_of(" \t");
                    if (s != std::string::npos) {
                        std::string newVar = leftSide.substr(s, e - s + 1);
                        if (newVar.find("متغير") == 0) {
                            size_t ws = newVar.find(' ');
                            if (ws != std::string::npos) newVar = newVar.substr(ws + 1);
                            size_t ns = newVar.find_first_not_of(" \t");
                            size_t ne = newVar.find_last_not_of(" \t");
                            if (ns != std::string::npos) newVar = newVar.substr(ns, ne - ns + 1);
                        }
                        // المتغير الجديد ناتج عن تنقية — لا يُعتبر ملوّثاً
                        sanitizedVars[newVar] = true;
                    }
                }
            }
        }
    }

    if (taintedVars.empty()) return;

    // المرحلة 2: فحص استخدام المتغيرات الملوّثة (غير المنقّاة) في المصارف
    stream.clear();
    stream.str(source);
    lineNum = 0;

    while (std::getline(stream, line)) {
        lineNum++;
        if (shouldExcludeLine(line)) continue;

        for (const auto& sink : taintSinks) {
            if (line.find(sink) != std::string::npos) {
                for (const auto& [varName, srcLine] : taintedVars) {
                    if (line.find(varName) != std::string::npos) {
                        // تخطي المتغيرات المنقّاة
                        auto it = sanitizedVars.find(varName);
                        if (it != sanitizedVars.end() && it->second) continue;

                        Finding f;
                        f.ruleId = "TAINT001";
                        f.severity = Severity::Critical;
                        f.category = RuleCategory::DataFlow;
                        f.message = "بيانات ملوّثة من سطر " + std::to_string(srcLine) +
                                    " تُستخدم في مصرف خطير بدون تنقية";
                        f.file = file;
                        f.line = lineNum;
                        f.codeLine = line;
                        f.suggestion = "نقّ المتغير '" + varName + "' عبر نقّ() أو validate() قبل استخدامه هنا";
                        f.confidence = 0.9;
                        f.autoFixable = true;
                        f.autoFixCode = "نقّ(" + varName + ")";
                        findings.push_back(std::move(f));
                    }
                }
            }
        }
    }
}

// ============================================================================
// ReportFormatter
// ============================================================================

std::string SecurityReportFormatter::formatText(const ScanResult& result, bool verbose) {
    std::ostringstream out;
    out << "\n╔══════════════════════════════════════════════════════════════════╗\n"
        << "║  🛡️  تقرير الحارس الأمني — لغة ص                               ║\n"
        << "╚══════════════════════════════════════════════════════════════════╝\n\n";

    out << "📊 ملخص: " << result.filesScanned << " ملف | "
        << result.totalLines << " سطر | "
        << result.findings.size() << " نتيجة | "
        << std::fixed;
    out.precision(2);
    out << result.durationSeconds << " ثانية\n\n";

    if (result.findings.empty()) {
        out << "  ✅ لم تُكتشف أي ثغرات أمنية!\n\n";
        return out.str();
    }

    out << "  🔴 حرجة: " << result.criticalCount()
        << "  🟠 عالية: " << result.highCount()
        << "  🟡 متوسطة: " << result.mediumCount()
        << "  🔵 منخفضة: " << result.lowCount() << "\n\n";

    out << "─────────────────────────────────────────────────────────────────\n";

    for (const auto& f : result.findings) {
        out << f.severityIcon() << " [" << f.ruleId << "] " << f.message << "\n"
            << "   📁 " << f.file << ":" << f.line << "\n";
        if (verbose && !f.codeLine.empty()) {
            std::string trimmed = f.codeLine;
            size_t s = trimmed.find_first_not_of(" \t");
            if (s != std::string::npos) trimmed = trimmed.substr(s);
            out << "   📝 " << trimmed << "\n";
        }
        if (!f.suggestion.empty())
            out << "   💡 " << f.suggestion << "\n";
        if (verbose && !f.attackExample.empty())
            out << "   ⚔️  مثال هجوم: " << f.attackExample << "\n";
        out << "\n";
    }

    out << "═══════════════════════════════════════════════════════════════════\n";
    if (result.hasCritical())
        out << "  ⛔ تم اكتشاف ثغرات حرجة — يجب الإصلاح قبل النشر!\n";
    else if (result.highCount() > 0)
        out << "  ⚠️  تم اكتشاف ثغرات عالية — يُنصح بالإصلاح.\n";
    else
        out << "  ✅ لا توجد ثغرات حرجة أو عالية.\n";

    return out.str();
}

static std::string escJson(const std::string& s) {
    std::string r; r.reserve(s.size());
    for (char c : s) {
        switch(c) { case '"':r+="\\\"";break; case '\\':r+="\\\\";break;
        case '\n':r+="\\n";break; case '\r':r+="\\r";break; case '\t':r+="\\t";break;
        default: r+=c; }
    }
    return r;
}

std::string SecurityReportFormatter::formatJSON(const ScanResult& result) {
    std::ostringstream out;
    out << "{\n  \"summary\": {\n"
        << "    \"filesScanned\": " << result.filesScanned << ",\n"
        << "    \"totalLines\": " << result.totalLines << ",\n"
        << "    \"totalFindings\": " << result.findings.size() << ",\n"
        << "    \"critical\": " << result.criticalCount() << ",\n"
        << "    \"high\": " << result.highCount() << ",\n"
        << "    \"medium\": " << result.mediumCount() << ",\n"
        << "    \"low\": " << result.lowCount() << ",\n"
        << "    \"durationSeconds\": " << result.durationSeconds << "\n"
        << "  },\n  \"findings\": [\n";

    for (size_t i = 0; i < result.findings.size(); ++i) {
        const auto& f = result.findings[i];
        out << "    {\n"
            << "      \"ruleId\": \"" << escJson(f.ruleId) << "\",\n"
            << "      \"severity\": \"" << escJson(f.severityNameAr()) << "\",\n"
            << "      \"category\": \"" << escJson(f.categoryNameAr()) << "\",\n"
            << "      \"message\": \"" << escJson(f.message) << "\",\n"
            << "      \"file\": \"" << escJson(f.file) << "\",\n"
            << "      \"line\": " << f.line << ",\n"
            << "      \"codeLine\": \"" << escJson(f.codeLine) << "\",\n"
            << "      \"suggestion\": \"" << escJson(f.suggestion) << "\",\n"
            << "      \"confidence\": " << f.confidence << "\n"
            << "    }";
        if (i < result.findings.size() - 1) out << ",";
        out << "\n";
    }
    out << "  ]\n}\n";
    return out.str();
}

std::string SecurityReportFormatter::formatHTML(const ScanResult& result, const std::string& title) {
    std::ostringstream out;
    std::string t = title.empty() ? "تقرير الحارس الأمني" : title;

    out << "<!DOCTYPE html>\n<html dir=\"rtl\" lang=\"ar\"><head><meta charset=\"UTF-8\">\n"
        << "<title>" << t << "</title>\n<style>\n"
        << ":root{--bg:#0f0f23;--card:#1a1a3e;--text:#e8e8f0;--border:#2a2a4a;--accent:#4dabf7}"
        << "body{font-family:'Segoe UI',Tahoma,sans-serif;background:var(--bg);color:var(--text);padding:20px;max-width:1000px;margin:0 auto}"
        << "h1{color:var(--accent);text-align:center}"
        << ".summary{display:grid;grid-template-columns:repeat(5,1fr);gap:10px;margin:20px 0}"
        << ".stat{background:var(--card);padding:15px;border-radius:8px;text-align:center;border:1px solid var(--border)}"
        << ".stat .num{font-size:2em;font-weight:bold}.critical .num{color:#ff6b6b}.high .num{color:#ffa94d}.medium .num{color:#ffd43b}.low .num{color:#74c0fc}"
        << ".finding{background:var(--card);padding:15px;border-radius:8px;margin:10px 0;border-right:4px solid}"
        << ".finding.sev-critical{border-color:#ff6b6b}.finding.sev-high{border-color:#ffa94d}.finding.sev-medium{border-color:#ffd43b}.finding.sev-low{border-color:#74c0fc}"
        << ".finding .rule{font-weight:bold;color:var(--accent)}.finding .loc{color:#888;font-size:.85em}"
        << ".finding code{background:#0a0a1a;padding:4px 8px;border-radius:4px;display:block;margin:5px 0;font-family:monospace;overflow-x:auto}"
        << ".finding .fix{color:#51cf66;font-style:italic}\n</style></head><body>\n"
        << "<h1>🛡️ " << t << "</h1>\n";

    out << "<div class=\"summary\">\n"
        << "<div class=\"stat\"><div class=\"num\">" << result.filesScanned << "</div>ملف</div>\n"
        << "<div class=\"stat critical\"><div class=\"num\">" << result.criticalCount() << "</div>حرجة</div>\n"
        << "<div class=\"stat high\"><div class=\"num\">" << result.highCount() << "</div>عالية</div>\n"
        << "<div class=\"stat medium\"><div class=\"num\">" << result.mediumCount() << "</div>متوسطة</div>\n"
        << "<div class=\"stat low\"><div class=\"num\">" << result.lowCount() << "</div>منخفضة</div>\n"
        << "</div>\n\n";

    if (result.findings.empty()) {
        out << "<div style='text-align:center;padding:40px'><h2>✅ لا توجد ثغرات أمنية</h2></div>\n";
    } else {
        for (const auto& f : result.findings) {
            std::string sevClass;
            switch(f.severity) {
                case Severity::Critical: sevClass = "sev-critical"; break;
                case Severity::High: sevClass = "sev-high"; break;
                case Severity::Medium: sevClass = "sev-medium"; break;
                default: sevClass = "sev-low";
            }
            out << "<div class=\"finding " << sevClass << "\">\n"
                << "  <span class=\"rule\">" << f.severityIcon() << " [" << f.ruleId << "]</span> "
                << f.message << "\n"
                << "  <div class=\"loc\">📁 " << f.file << ":" << f.line << "</div>\n";
            if (!f.codeLine.empty()) out << "  <code>" << f.codeLine << "</code>\n";
            if (!f.suggestion.empty()) out << "  <div class=\"fix\">💡 " << f.suggestion << "</div>\n";
            out << "</div>\n";
        }
    }

    out << "</body></html>\n";
    return out.str();
}

std::string SecurityReportFormatter::formatSARIF(const ScanResult& result) {
    std::ostringstream out;
    out << "{\n  \"$schema\": \"https://raw.githubusercontent.com/oasis-tcs/sarif-spec/master/Schemata/sarif-schema-2.1.0.json\",\n"
        << "  \"version\": \"2.1.0\",\n  \"runs\": [{\n"
        << "    \"tool\": {\"driver\": {\"name\": \"sad-guard\", \"version\": \"1.0.0\"}},\n"
        << "    \"results\": [\n";

    for (size_t i = 0; i < result.findings.size(); ++i) {
        const auto& f = result.findings[i];
        std::string level;
        switch(f.severity) {
            case Severity::Critical: case Severity::High: level = "error"; break;
            case Severity::Medium: level = "warning"; break;
            default: level = "note";
        }
        out << "      {\"ruleId\": \"" << f.ruleId << "\", \"level\": \"" << level
            << "\", \"message\": {\"text\": \"" << escJson(f.message) << "\"}"
            << ", \"locations\": [{\"physicalLocation\": {\"artifactLocation\": {\"uri\": \""
            << escJson(f.file) << "\"}, \"region\": {\"startLine\": " << f.line << "}}}]}";
        if (i < result.findings.size()-1) out << ",";
        out << "\n";
    }
    out << "    ]\n  }]\n}\n";
    return out.str();
}

bool SecurityReportFormatter::writeReport(const ScanResult& result, const std::string& path, ReportFormat format) {
    std::ofstream file(path);
    if (!file.is_open()) return false;
    switch (format) {
        case ReportFormat::Text: file << formatText(result, true); break;
        case ReportFormat::JSON: file << formatJSON(result); break;
        case ReportFormat::HTML: file << formatHTML(result); break;
        case ReportFormat::SARIF: file << formatSARIF(result); break;
    }
    return file.good();
}

// ============================================================================
// CLI
// ============================================================================

int guard_main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "╔══════════════════════════════════════════╗\n"
                  << "║  🛡️  حارس ص الأمني — sad-guard            ║\n"
                  << "╚══════════════════════════════════════════╝\n\n"
                  << "الاستخدام:\n"
                  << "  sad-guard <ملف.ص>            فحص ملف\n"
                  << "  sad-guard <مجلد>             فحص مجلد\n"
                  << "  sad-guard --json <ملف>       إخراج JSON\n"
                  << "  sad-guard --html <ملف>       إخراج HTML\n"
                  << "  sad-guard --sarif <ملف>      إخراج SARIF\n"
                  << "  sad-guard --ci <مجلد>        وضع CI/CD\n"
                  << "  sad-guard --critical <ملف>   الحرجة فقط\n"
                  << "  sad-guard --report <ملف> <هدف>  حفظ التقرير\n"
                  << "  sad-guard --config <ملف>     ملف إعدادات\n\n";
        return 1;
    }

    ScannerConfig config;
    config.excludeLines = {"// noguard", "# noguard", "حارس:تجاهل"};
    std::string path, reportPath;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--json") config.reportFormat = ReportFormat::JSON;
        else if (arg == "--html") config.reportFormat = ReportFormat::HTML;
        else if (arg == "--sarif") config.reportFormat = ReportFormat::SARIF;
        else if (arg == "--ci") config.ciMode = true;
        else if (arg == "--critical") config.criticalOnly = true;
        else if (arg == "--verbose" || arg == "-v") config.verbose = true;
        else if (arg == "--no-taint") config.taintAnalysis = false;
        else if (arg == "--no-pattern") config.patternScan = false;
        else if (arg == "--report" && i + 1 < argc) reportPath = argv[++i];
        else if (arg == "--config" && i + 1 < argc) config = ScannerConfig::fromFile(argv[++i]);
        else if (arg == "--disable" && i + 1 < argc) config.disabledRules.push_back(argv[++i]);
        else path = arg;
    }

    if (path.empty()) { std::cerr << "خطأ: يجب تحديد ملف أو مجلد\n"; return 1; }

    SecurityScanner scanner(config);
    ScanResult result;

    if (fs::is_directory(path)) {
        result = scanner.scanDirectory(path);
    } else if (fs::is_regular_file(path)) {
        result = scanner.scanFile(path);
    } else {
        std::cerr << "خطأ: الهدف غير موجود: " << path << "\n";
        return 1;
    }

    // عرض النتائج
    switch (config.reportFormat) {
        case ReportFormat::JSON: std::cout << SecurityReportFormatter::formatJSON(result); break;
        case ReportFormat::HTML: std::cout << SecurityReportFormatter::formatHTML(result); break;
        case ReportFormat::SARIF: std::cout << SecurityReportFormatter::formatSARIF(result); break;
        default: std::cout << SecurityReportFormatter::formatText(result, config.verbose);
    }

    // حفظ التقرير
    if (!reportPath.empty()) {
        SecurityReportFormatter::writeReport(result, reportPath, config.reportFormat);
        std::cout << "  💾 تم حفظ التقرير: " << reportPath << "\n";
    }

    // وضع CI
    if (config.ciMode) {
        return result.hasCritical() ? 2 : (result.highCount() > 0 ? 1 : 0);
    }

    return result.hasCritical() ? 1 : 0;
}

} // namespace Security
} // namespace Sad
