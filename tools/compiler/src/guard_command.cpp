/**
 * =============================================================================
 * ملف: guard_command.cpp
 * الوصف: أمر CLI للحارس الأمني (ص حارس)
 * المهمة: T224 - Guard CLI command
 * المرحلة: Phase 22 - User Story 19 (Security Linting ص حارس)
 * =============================================================================
 * 
 * 🛡️ دليل المبتدئ لـ "ص حارس"
 * ═══════════════════════════════
 * 
 * ما هو "ص حارس"؟
 * ─────────────────
 * هو حارس أمني لكودك! يفحص برنامجك ويحذرك من المشاكل الأمنية.
 * 
 * مثل حارس المبنى الذي:
 * - يتأكد أن الأبواب مغلقة ✓
 * - يراقب الأماكن المشبوهة ✓
 * - يحذر من المخاطر ✓
 * 
 * كيف تستخدمه؟
 * ──────────────
 * 
 * 1. فحص ملف واحد:
 *    $ ص حارس ملفي.ص
 * 
 * 2. فحص مجلد كامل:
 *    $ ص حارس src/
 * 
 * 3. فحص مع تفاصيل:
 *    $ ص حارس --مفصل ملفي.ص
 * 
 * 4. فحص سريع (فقط الحرج):
 *    $ ص حارس --حرج-فقط ملفي.ص
 * 
 * 5. تقرير HTML:
 *    $ ص حارس --تقرير report.html src/
 * 
 * ماذا يفحص؟
 * ────────────
 * ✓ حقن SQL
 * ✓ فيض المخزن المؤقت
 * ✓ التحقق من المدخلات
 * ✓ استخدام التشفير
 * ✓ الكتل غير الآمنة
 * ✓ كلمات السر في الكود
 * 
 * مستويات الخطورة:
 * ──────────────────
 * 🔴 حرج (Critical) - يجب إصلاحه فوراً!
 * 🟠 عالي (High) - مهم جداً
 * 🟡 متوسط (Medium) - يحتاج اهتمام
 * 🔵 منخفض (Low) - تحسين مقترح
 * ⚪ معلومة (Info) - للعلم فقط
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

namespace sad::security::cli {

// =============================================================================
// تكوين الأمر
// =============================================================================

struct GuardConfig {
    // ملفات للفحص
    std::vector<std::string> paths;
    
    // خيارات الفحص
    bool verbose = false;           // --مفصل / --verbose
    bool criticalOnly = false;      // --حرج-فقط / --critical-only
    bool autoFix = false;           // --إصلاح / --fix
    bool jsonOutput = false;        // --json
    bool ciMode = false;            // --ci
    
    // التقرير
    std::string reportFile;         // --تقرير / --report
    std::string reportFormat = "text"; // text, html, json
    
    // القواعد
    std::set<std::string> enabledRules;   // --قاعدة / --rule
    std::set<std::string> disabledRules;  // --بدون / --disable
    
    // مستوى الخطورة الأدنى
    int minSeverity = 1;            // --خطورة / --severity
    
    // الاستثناءات
    std::vector<std::string> excludePatterns; // --استثناء / --exclude
    
    // التكوين من ملف
    std::string configFile;         // --تكوين / --config
    
    // العرض
    bool noColor = false;           // --بدون-ألوان / --no-color
    bool quiet = false;             // --صامت / --quiet
    
    // الحد الأقصى للمشاكل
    int maxIssues = 0;              // --حد / --max (0 = unlimited)
};

// =============================================================================
// تحليل الوسطاء
// =============================================================================

class ArgParser {
public:
    static GuardConfig parse(int argc, const char* argv[]) {
        GuardConfig config;
        
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            
            // خيارات بقيمة
            if (arg == "--تقرير" || arg == "--report") {
                if (i + 1 < argc) config.reportFile = argv[++i];
            }
            else if (arg == "--تكوين" || arg == "--config") {
                if (i + 1 < argc) config.configFile = argv[++i];
            }
            else if (arg == "--خطورة" || arg == "--severity") {
                if (i + 1 < argc) config.minSeverity = std::stoi(argv[++i]);
            }
            else if (arg == "--حد" || arg == "--max") {
                if (i + 1 < argc) config.maxIssues = std::stoi(argv[++i]);
            }
            else if (arg == "--قاعدة" || arg == "--rule") {
                if (i + 1 < argc) config.enabledRules.insert(argv[++i]);
            }
            else if (arg == "--بدون" || arg == "--disable") {
                if (i + 1 < argc) config.disabledRules.insert(argv[++i]);
            }
            else if (arg == "--استثناء" || arg == "--exclude") {
                if (i + 1 < argc) config.excludePatterns.push_back(argv[++i]);
            }
            else if (arg == "--صيغة" || arg == "--format") {
                if (i + 1 < argc) config.reportFormat = argv[++i];
            }
            // خيارات منطقية
            else if (arg == "--مفصل" || arg == "--verbose" || arg == "-v") {
                config.verbose = true;
            }
            else if (arg == "--حرج-فقط" || arg == "--critical-only") {
                config.criticalOnly = true;
                config.minSeverity = 9;
            }
            else if (arg == "--إصلاح" || arg == "--fix") {
                config.autoFix = true;
            }
            else if (arg == "--json") {
                config.jsonOutput = true;
            }
            else if (arg == "--ci") {
                config.ciMode = true;
                config.noColor = true;
            }
            else if (arg == "--بدون-ألوان" || arg == "--no-color") {
                config.noColor = true;
            }
            else if (arg == "--صامت" || arg == "--quiet" || arg == "-q") {
                config.quiet = true;
            }
            else if (arg == "--مساعدة" || arg == "--help" || arg == "-h") {
                printHelp();
                std::exit(0);
            }
            else if (arg == "--نسخة" || arg == "--version") {
                printVersion();
                std::exit(0);
            }
            // ملفات/مجلدات
            else if (arg[0] != '-') {
                config.paths.push_back(arg);
            }
        }
        
        return config;
    }
    
    static void printHelp() {
        std::cout << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                           🛡️ ص حارس - Security Guard                         ║
║                        الحارس الأمني للغة ص                                   ║
╚══════════════════════════════════════════════════════════════════════════════╝

الاستخدام:
    ص حارس [خيارات] <ملفات/مجلدات>

الأمثلة:
    ص حارس app.ص                     # فحص ملف واحد
    ص حارس src/                      # فحص مجلد
    ص حارس --مفصل app.ص              # فحص مع تفاصيل
    ص حارس --تقرير report.html src/  # تقرير HTML
    ص حارس --ci src/                 # وضع CI/CD

خيارات الفحص:
    --مفصل, --verbose, -v         عرض تفاصيل إضافية
    --حرج-فقط, --critical-only    فقط المشاكل الحرجة
    --خطورة N, --severity N       الحد الأدنى للخطورة (1-10)
    --حد N, --max N               الحد الأقصى للمشاكل

القواعد:
    --قاعدة R, --rule R           تفعيل قاعدة محددة
    --بدون R, --disable R         تعطيل قاعدة
    
    القواعد المتاحة:
      SQL001   - حقن SQL
      BUF001   - فيض المخزن المؤقت
      INP001   - التحقق من المدخلات
      CRY001   - سوء استخدام التشفير
      SEC001   - كتل غير آمنة
      SEC002   - مؤشرات خام
      SEC003   - تسريب بيانات حساسة

الإخراج:
    --تقرير F, --report F         حفظ التقرير في ملف
    --صيغة, --format             صيغة التقرير (text/html/json)
    --json                        إخراج JSON
    --بدون-ألوان, --no-color      بدون ألوان

الإصلاح:
    --إصلاح, --fix                إصلاح تلقائي (حيث ممكن)

التكامل:
    --ci                          وضع CI/CD
    --تكوين F, --config F         ملف تكوين

أخرى:
    --استثناء P, --exclude P      استثناء ملفات
    --صامت, --quiet, -q           بدون إخراج
    --مساعدة, --help, -h          عرض المساعدة
    --نسخة, --version             عرض النسخة

أكواد الخروج:
    0 - لا مشاكل
    1 - مشاكل منخفضة/متوسطة
    2 - مشاكل عالية
    3 - مشاكل حرجة
    4 - خطأ في التنفيذ

)";
    }
    
    static void printVersion() {
        std::cout << "ص حارس (Sad Guard) v1.0.0\n";
        std::cout << "الحارس الأمني للغة ص\n";
        std::cout << "جزء من مجموعة أدوات ص\n";
    }
};

// =============================================================================
// نتيجة الفحص
// =============================================================================

enum class Severity {
    Info = 1,
    Low = 3,
    Medium = 5,
    High = 7,
    Critical = 9
};

struct SecurityIssue {
    std::string ruleId;
    std::string message;
    std::string file;
    int line = 0;
    int column = 0;
    Severity severity = Severity::Medium;
    std::string code;
    std::string explanation;
    std::string fix;
    std::string cweId;
};

class ScanResult {
public:
    void addIssue(const SecurityIssue& issue) {
        issues_.push_back(issue);
    }
    
    const std::vector<SecurityIssue>& getIssues() const { return issues_; }
    
    int getCriticalCount() const {
        return countBySeverity(Severity::Critical);
    }
    
    int getHighCount() const {
        return countBySeverity(Severity::High);
    }
    
    int getMediumCount() const {
        return countBySeverity(Severity::Medium);
    }
    
    int getLowCount() const {
        return countBySeverity(Severity::Low);
    }
    
    int getTotalCount() const {
        return static_cast<int>(issues_.size());
    }
    
    int getExitCode() const {
        if (getCriticalCount() > 0) return 3;
        if (getHighCount() > 0) return 2;
        if (getMediumCount() > 0 || getLowCount() > 0) return 1;
        return 0;
    }
    
private:
    std::vector<SecurityIssue> issues_;
    
    int countBySeverity(Severity sev) const {
        int count = 0;
        for (const auto& issue : issues_) {
            if (issue.severity == sev) count++;
        }
        return count;
    }
};

// =============================================================================
// ماسح الملفات
// =============================================================================

class FileScanner {
public:
    std::vector<std::string> collectFiles(
        const std::vector<std::string>& paths,
        const std::vector<std::string>& excludePatterns
    ) {
        std::vector<std::string> files;
        
        for (const auto& path : paths) {
            fs::path p(path);
            
            if (fs::is_directory(p)) {
                collectFromDirectory(p, files, excludePatterns);
            }
            else if (fs::exists(p)) {
                if (shouldInclude(p.string(), excludePatterns)) {
                    files.push_back(p.string());
                }
            }
        }
        
        return files;
    }
    
private:
    void collectFromDirectory(
        const fs::path& dir,
        std::vector<std::string>& files,
        const std::vector<std::string>& excludePatterns
    ) {
        for (const auto& entry : fs::recursive_directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                auto ext = entry.path().extension().string();
                // ملفات ص فقط
                if (ext == ".ص" || ext == ".sad") {
                    if (shouldInclude(entry.path().string(), excludePatterns)) {
                        files.push_back(entry.path().string());
                    }
                }
            }
        }
    }
    
    bool shouldInclude(
        const std::string& file,
        const std::vector<std::string>& excludePatterns
    ) {
        for (const auto& pattern : excludePatterns) {
            if (file.find(pattern) != std::string::npos) {
                return false;
            }
        }
        return true;
    }
};

// =============================================================================
// فاحص الأمان
// =============================================================================

class SecurityScanner {
public:
    ScanResult scan(const std::string& file, const GuardConfig& config) {
        ScanResult result;
        
        std::ifstream f(file);
        if (!f.is_open()) {
            return result;
        }
        
        std::stringstream buffer;
        buffer << f.rdbuf();
        std::string content = buffer.str();
        
        // فحص كل سطر
        std::istringstream stream(content);
        std::string line;
        int lineNumber = 0;
        
        while (std::getline(stream, line)) {
            lineNumber++;
            
            // فحص القواعد المختلفة
            checkSQLInjection(line, lineNumber, file, config, result);
            checkBufferOverflow(line, lineNumber, file, config, result);
            checkInputValidation(line, lineNumber, file, config, result);
            checkCryptoMisuse(line, lineNumber, file, config, result);
            checkHardcodedSecrets(line, lineNumber, file, config, result);
            checkUnsafeBlocks(line, lineNumber, file, config, result);
        }
        
        return result;
    }
    
private:
    bool isRuleEnabled(const std::string& ruleId, const GuardConfig& config) {
        if (!config.enabledRules.empty() && 
            config.enabledRules.find(ruleId) == config.enabledRules.end()) {
            return false;
        }
        if (config.disabledRules.find(ruleId) != config.disabledRules.end()) {
            return false;
        }
        return true;
    }
    
    void checkSQLInjection(
        const std::string& line, int lineNumber, const std::string& file,
        const GuardConfig& config, ScanResult& result
    ) {
        if (!isRuleEnabled("SQL001", config)) return;
        
        // بحث عن SQL ديناميكي
        if (line.find("استعلام(") != std::string::npos ||
            line.find("query(") != std::string::npos) {
            
            // تحقق من تركيب السلاسل
            if (line.find("+") != std::string::npos || 
                line.find("{}") != std::string::npos) {
                
                SecurityIssue issue;
                issue.ruleId = "SQL001";
                issue.message = "احتمال حقن SQL - استخدام قيم مباشرة في الاستعلام";
                issue.file = file;
                issue.line = lineNumber;
                issue.severity = Severity::Critical;
                issue.code = line;
                issue.cweId = "CWE-89";
                issue.explanation = "دمج المدخلات مباشرة في SQL يسمح بالحقن";
                issue.fix = "استخدم الاستعلامات المعدة (Prepared Statements)";
                
                result.addIssue(issue);
            }
        }
    }
    
    void checkBufferOverflow(
        const std::string& line, int lineNumber, const std::string& file,
        const GuardConfig& config, ScanResult& result
    ) {
        if (!isRuleEnabled("BUF001", config)) return;
        
        std::vector<std::string> unsafeFuncs = {"strcpy", "strcat", "gets", "sprintf", "scanf"};
        
        for (const auto& func : unsafeFuncs) {
            if (line.find(func + "(") != std::string::npos) {
                SecurityIssue issue;
                issue.ruleId = "BUF001";
                issue.message = "استخدام دالة غير آمنة: " + func;
                issue.file = file;
                issue.line = lineNumber;
                issue.severity = Severity::High;
                issue.code = line;
                issue.cweId = "CWE-120";
                issue.fix = "استخدم البدائل الآمنة مع تحديد الحجم";
                
                result.addIssue(issue);
            }
        }
    }
    
    void checkInputValidation(
        const std::string& line, int lineNumber, const std::string& file,
        const GuardConfig& config, ScanResult& result
    ) {
        if (!isRuleEnabled("INP001", config)) return;
        
        // مدخلات مستخدمة مباشرة
        if ((line.find("اقرأ(") != std::string::npos ||
             line.find("input(") != std::string::npos ||
             line.find("طلب.") != std::string::npos) &&
            line.find("تحقق") == std::string::npos &&
            line.find("validate") == std::string::npos) {
            
            SecurityIssue issue;
            issue.ruleId = "INP001";
            issue.message = "مدخلات بدون تحقق";
            issue.file = file;
            issue.line = lineNumber;
            issue.severity = Severity::Medium;
            issue.code = line;
            issue.cweId = "CWE-20";
            issue.fix = "تحقق من المدخلات قبل الاستخدام";
            
            result.addIssue(issue);
        }
    }
    
    void checkCryptoMisuse(
        const std::string& line, int lineNumber, const std::string& file,
        const GuardConfig& config, ScanResult& result
    ) {
        if (!isRuleEnabled("CRY001", config)) return;
        
        std::vector<std::string> weakAlgos = {"MD5", "md5", "SHA1", "sha1", "DES", "RC4"};
        
        for (const auto& algo : weakAlgos) {
            if (line.find(algo) != std::string::npos) {
                SecurityIssue issue;
                issue.ruleId = "CRY001";
                issue.message = "استخدام خوارزمية ضعيفة: " + algo;
                issue.file = file;
                issue.line = lineNumber;
                issue.severity = Severity::High;
                issue.code = line;
                issue.cweId = "CWE-327";
                issue.fix = "استخدم SHA-256 أو AES-256";
                
                result.addIssue(issue);
            }
        }
    }
    
    void checkHardcodedSecrets(
        const std::string& line, int lineNumber, const std::string& file,
        const GuardConfig& config, ScanResult& result
    ) {
        if (!isRuleEnabled("SEC003", config)) return;
        
        std::vector<std::string> patterns = {
            "password", "كلمة_السر", "secret", "سر", "api_key", "token"
        };
        
        for (const auto& pattern : patterns) {
            if (line.find(pattern) != std::string::npos &&
                (line.find("= \"") != std::string::npos ||
                 line.find("= '") != std::string::npos)) {
                
                SecurityIssue issue;
                issue.ruleId = "SEC003";
                issue.message = "سر/كلمة سر ثابتة في الكود";
                issue.file = file;
                issue.line = lineNumber;
                issue.severity = Severity::Critical;
                issue.code = line;
                issue.cweId = "CWE-798";
                issue.fix = "استخدم متغيرات البيئة";
                
                result.addIssue(issue);
            }
        }
    }
    
    void checkUnsafeBlocks(
        const std::string& line, int lineNumber, const std::string& file,
        const GuardConfig& config, ScanResult& result
    ) {
        if (!isRuleEnabled("SEC001", config)) return;
        
        if (line.find("غير_آمن") != std::string::npos ||
            line.find("unsafe") != std::string::npos) {
            
            SecurityIssue issue;
            issue.ruleId = "SEC001";
            issue.message = "استخدام كتلة غير آمنة";
            issue.file = file;
            issue.line = lineNumber;
            issue.severity = Severity::Medium;
            issue.code = line;
            issue.explanation = "الكتل غير الآمنة تتجاوز فحوصات الأمان";
            issue.fix = "تأكد من أن هذا ضروري وموثق";
            
            result.addIssue(issue);
        }
    }
};

// =============================================================================
// مُخرج النتائج
// =============================================================================

class OutputFormatter {
public:
    static std::string formatText(const ScanResult& result, const GuardConfig& config) {
        std::ostringstream ss;
        
        if (!config.quiet) {
            ss << "\n";
            ss << "╔══════════════════════════════════════════════════════════════════════╗\n";
            ss << "║                  🛡️ تقرير الحارس الأمني - ص حارس                     ║\n";
            ss << "╚══════════════════════════════════════════════════════════════════════╝\n";
        }
        
        if (result.getTotalCount() == 0) {
            ss << "\n✅ لم يُكتشف أي مشاكل أمنية - كودك آمن!\n";
            return ss.str();
        }
        
        // الإحصائيات
        ss << "\n📊 ملخص النتائج:\n";
        ss << "├─ 🔴 حرج: " << result.getCriticalCount() << "\n";
        ss << "├─ 🟠 عالي: " << result.getHighCount() << "\n";
        ss << "├─ 🟡 متوسط: " << result.getMediumCount() << "\n";
        ss << "├─ 🔵 منخفض: " << result.getLowCount() << "\n";
        ss << "└─ المجموع: " << result.getTotalCount() << "\n";
        
        // التفاصيل
        ss << "\n" << std::string(70, '─') << "\n";
        
        for (const auto& issue : result.getIssues()) {
            std::string severityIcon;
            switch (issue.severity) {
                case Severity::Critical: severityIcon = "🔴"; break;
                case Severity::High: severityIcon = "🟠"; break;
                case Severity::Medium: severityIcon = "🟡"; break;
                case Severity::Low: severityIcon = "🔵"; break;
                default: severityIcon = "⚪"; break;
            }
            
            ss << "\n" << severityIcon << " [" << issue.ruleId << "] " << issue.message << "\n";
            ss << "   📍 " << issue.file << ":" << issue.line << "\n";
            
            if (!issue.code.empty() && config.verbose) {
                ss << "   📝 " << issue.code << "\n";
            }
            
            if (!issue.cweId.empty()) {
                ss << "   🔗 " << issue.cweId << "\n";
            }
            
            if (!issue.fix.empty()) {
                ss << "   💡 " << issue.fix << "\n";
            }
        }
        
        return ss.str();
    }
    
    static std::string formatJSON(const ScanResult& result) {
        std::ostringstream ss;
        
        ss << "{\n";
        ss << "  \"summary\": {\n";
        ss << "    \"total\": " << result.getTotalCount() << ",\n";
        ss << "    \"critical\": " << result.getCriticalCount() << ",\n";
        ss << "    \"high\": " << result.getHighCount() << ",\n";
        ss << "    \"medium\": " << result.getMediumCount() << ",\n";
        ss << "    \"low\": " << result.getLowCount() << "\n";
        ss << "  },\n";
        ss << "  \"issues\": [\n";
        
        const auto& issues = result.getIssues();
        for (size_t i = 0; i < issues.size(); i++) {
            const auto& issue = issues[i];
            ss << "    {\n";
            ss << "      \"ruleId\": \"" << issue.ruleId << "\",\n";
            ss << "      \"message\": \"" << escapeJSON(issue.message) << "\",\n";
            ss << "      \"file\": \"" << escapeJSON(issue.file) << "\",\n";
            ss << "      \"line\": " << issue.line << ",\n";
            ss << "      \"severity\": " << static_cast<int>(issue.severity) << ",\n";
            ss << "      \"cweId\": \"" << issue.cweId << "\",\n";
            ss << "      \"fix\": \"" << escapeJSON(issue.fix) << "\"\n";
            ss << "    }";
            if (i < issues.size() - 1) ss << ",";
            ss << "\n";
        }
        
        ss << "  ]\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    static std::string formatHTML(const ScanResult& result) {
        std::ostringstream ss;
        
        ss << R"(<!DOCTYPE html>
<html dir="rtl" lang="ar">
<head>
    <meta charset="UTF-8">
    <title>تقرير الحارس الأمني</title>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Arial, sans-serif;
            background: #1a1a2e;
            color: #eee;
            margin: 0;
            padding: 20px;
        }
        .header {
            text-align: center;
            padding: 30px;
            background: linear-gradient(135deg, #16213e, #0f3460);
            border-radius: 10px;
            margin-bottom: 20px;
        }
        .summary {
            display: grid;
            grid-template-columns: repeat(5, 1fr);
            gap: 15px;
            margin-bottom: 20px;
        }
        .stat {
            background: #16213e;
            padding: 20px;
            border-radius: 10px;
            text-align: center;
        }
        .stat .number {
            font-size: 2em;
            font-weight: bold;
        }
        .critical { color: #ff4444; }
        .high { color: #ff8c00; }
        .medium { color: #ffd700; }
        .low { color: #4dabf7; }
        .info { color: #aaa; }
        .issue {
            background: #16213e;
            margin: 10px 0;
            padding: 15px;
            border-radius: 10px;
            border-right: 4px solid;
        }
        .issue.critical { border-color: #ff4444; }
        .issue.high { border-color: #ff8c00; }
        .issue.medium { border-color: #ffd700; }
        .issue.low { border-color: #4dabf7; }
        code {
            background: #0a0a0a;
            padding: 10px;
            display: block;
            border-radius: 5px;
            margin: 10px 0;
        }
        .fix {
            background: #1e4d2b;
            padding: 10px;
            border-radius: 5px;
            margin-top: 10px;
        }
    </style>
</head>
<body>
    <div class="header">
        <h1>🛡️ تقرير الحارس الأمني</h1>
        <p>ص حارس - فحص أمني شامل</p>
    </div>
    
    <div class="summary">
        <div class="stat">
            <div class="number critical">)" << result.getCriticalCount() << R"(</div>
            <div>🔴 حرج</div>
        </div>
        <div class="stat">
            <div class="number high">)" << result.getHighCount() << R"(</div>
            <div>🟠 عالي</div>
        </div>
        <div class="stat">
            <div class="number medium">)" << result.getMediumCount() << R"(</div>
            <div>🟡 متوسط</div>
        </div>
        <div class="stat">
            <div class="number low">)" << result.getLowCount() << R"(</div>
            <div>🔵 منخفض</div>
        </div>
        <div class="stat">
            <div class="number">)" << result.getTotalCount() << R"(</div>
            <div>المجموع</div>
        </div>
    </div>
)";
        
        for (const auto& issue : result.getIssues()) {
            std::string sevClass;
            switch (issue.severity) {
                case Severity::Critical: sevClass = "critical"; break;
                case Severity::High: sevClass = "high"; break;
                case Severity::Medium: sevClass = "medium"; break;
                case Severity::Low: sevClass = "low"; break;
                default: sevClass = "info"; break;
            }
            
            ss << "    <div class=\"issue " << sevClass << "\">\n";
            ss << "        <strong>[" << issue.ruleId << "]</strong> " << escapeHTML(issue.message) << "<br>\n";
            ss << "        <small>📍 " << escapeHTML(issue.file) << ":" << issue.line << "</small>\n";
            if (!issue.code.empty()) {
                ss << "        <code>" << escapeHTML(issue.code) << "</code>\n";
            }
            if (!issue.fix.empty()) {
                ss << "        <div class=\"fix\">💡 " << escapeHTML(issue.fix) << "</div>\n";
            }
            ss << "    </div>\n";
        }
        
        ss << R"(
</body>
</html>
)";
        
        return ss.str();
    }
    
private:
    static std::string escapeJSON(const std::string& s) {
        std::string result;
        for (char c : s) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }
    
    static std::string escapeHTML(const std::string& s) {
        std::string result;
        for (char c : s) {
            switch (c) {
                case '<': result += "&lt;"; break;
                case '>': result += "&gt;"; break;
                case '&': result += "&amp;"; break;
                case '"': result += "&quot;"; break;
                default: result += c; break;
            }
        }
        return result;
    }
};

// =============================================================================
// الأمر الرئيسي
// =============================================================================

class GuardCommand {
public:
    int execute(int argc, const char* argv[]) {
        // تحليل الوسطاء
        GuardConfig config = ArgParser::parse(argc, argv);
        
        // تحقق من وجود ملفات
        if (config.paths.empty()) {
            std::cerr << "خطأ: لم يتم تحديد ملفات للفحص\n";
            std::cerr << "استخدم: ص حارس <ملفات/مجلدات>\n";
            std::cerr << "للمساعدة: ص حارس --مساعدة\n";
            return 4;
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // جمع الملفات
        FileScanner fileScanner;
        auto files = fileScanner.collectFiles(config.paths, config.excludePatterns);
        
        if (files.empty()) {
            std::cout << "لم يُعثر على ملفات .ص للفحص\n";
            return 0;
        }
        
        if (!config.quiet) {
            std::cout << "🔍 فحص " << files.size() << " ملف...\n";
        }
        
        // فحص كل ملف
        SecurityScanner scanner;
        ScanResult totalResult;
        
        for (const auto& file : files) {
            if (config.verbose) {
                std::cout << "  فحص: " << file << "\n";
            }
            
            auto result = scanner.scan(file, config);
            for (const auto& issue : result.getIssues()) {
                if (static_cast<int>(issue.severity) >= config.minSeverity) {
                    totalResult.addIssue(issue);
                }
                
                if (config.maxIssues > 0 && 
                    totalResult.getTotalCount() >= config.maxIssues) {
                    break;
                }
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // إخراج النتائج
        std::string output;
        if (config.jsonOutput) {
            output = OutputFormatter::formatJSON(totalResult);
        } else if (config.reportFormat == "html") {
            output = OutputFormatter::formatHTML(totalResult);
        } else {
            output = OutputFormatter::formatText(totalResult, config);
        }
        
        // حفظ أو عرض
        if (!config.reportFile.empty()) {
            std::ofstream f(config.reportFile);
            f << output;
            if (!config.quiet) {
                std::cout << "📄 تم حفظ التقرير في: " << config.reportFile << "\n";
            }
        } else {
            std::cout << output;
        }
        
        if (!config.quiet) {
            std::cout << "\n⏱️ الوقت: " << duration.count() << " مللي ثانية\n";
        }
        
        return totalResult.getExitCode();
    }
};

} // namespace sad::security::cli

// =============================================================================
// نقطة الدخول
// =============================================================================

int main(int argc, const char* argv[]) {
    sad::security::cli::GuardCommand command;
    return command.execute(argc, argv);
}

// =============================================================================
// اختبارات
// =============================================================================

#ifdef GUARD_COMMAND_TEST

#include <cassert>

void testArgParser() {
    const char* argv[] = {"ص", "حارس", "--مفصل", "app.ص"};
    auto config = sad::security::cli::ArgParser::parse(4, argv);
    
    assert(config.verbose == true);
    assert(config.paths.size() == 1);
    assert(config.paths[0] == "app.ص");
    
    std::cout << "✅ اختبار تحليل الوسطاء نجح\n";
}

void testScanner() {
    sad::security::cli::SecurityScanner scanner;
    sad::security::cli::GuardConfig config;
    
    // ملف مؤقت للاختبار
    std::ofstream f("test_guard.ص");
    f << R"(
دالة خطيرة()
    كلمة_السر = "secret123"
    تجزئة = MD5(بيانات)
نهاية
)";
    f.close();
    
    auto result = scanner.scan("test_guard.ص", config);
    assert(result.getTotalCount() >= 2);
    
    std::remove("test_guard.ص");
    
    std::cout << "✅ اختبار الماسح نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات أمر الحارس\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testArgParser();
    testScanner();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // GUARD_COMMAND_TEST
