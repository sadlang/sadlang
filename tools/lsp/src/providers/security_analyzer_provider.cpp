// بسم الله الرحمن الرحيم
/**
 * ══════════════════════════════════════════════════════════════════════════════
 * ملف: security_analyzer_provider.cpp
 * الوصف: تنفيذ محلل الأمان الثوري للغة ص
 * ══════════════════════════════════════════════════════════════════════════════
 */

#include "security_analyzer_provider.h"
#include <sstream>
#include <algorithm>
#include <chrono>

namespace sad {
namespace lsp {
namespace security {

// ══════════════════════════════════════════════════════════════════════════════
//  المنشئ والمدمر
// ══════════════════════════════════════════════════════════════════════════════

SecurityAnalyzerProvider::SecurityAnalyzerProvider() {
    initialize_rules();
    initialize_taint_sources();
}

SecurityAnalyzerProvider::~SecurityAnalyzerProvider() = default;

// ══════════════════════════════════════════════════════════════════════════════
//  تهيئة القواعد
// ══════════════════════════════════════════════════════════════════════════════

void SecurityAnalyzerProvider::initialize_rules() {
    // قاعدة: بيانات اعتماد ثابتة
    {
        SecurityRule rule;
        rule.id = "SEC001";
        rule.name = "بيانات اعتماد ثابتة";
        rule.description = "كلمات مرور أو مفاتيح ثابتة في الكود";
        rule.severity = VulnerabilitySeverity::Critical;
        rule.type = VulnerabilityType::HardcodedCredentials;
        rule.cwe = CWECategory::CWE_798;
        rule.pattern_str = R"((كلمة_المرور|password|سر|secret|مفتاح|key|رمز|token)\s*=\s*["'][^"']+["'])";
        rule.pattern = std::regex(rule.pattern_str, std::regex::icase);
        rules_.push_back(rule);
    }
    
    // قاعدة: حقن SQL
    {
        SecurityRule rule;
        rule.id = "SEC002";
        rule.name = "حقن SQL محتمل";
        rule.description = "دمج نص في استعلام SQL";
        rule.severity = VulnerabilitySeverity::Critical;
        rule.type = VulnerabilityType::SQLInjection;
        rule.cwe = CWECategory::CWE_89;
        rule.pattern_str = R"((استعلام|query|sql|نفذ_sql)\s*\(\s*["'][^"']*["']\s*\+)";
        rule.pattern = std::regex(rule.pattern_str, std::regex::icase);
        rules_.push_back(rule);
    }
    
    // قاعدة: حقن أوامر
    {
        SecurityRule rule;
        rule.id = "SEC003";
        rule.name = "حقن أوامر نظام";
        rule.description = "تنفيذ أوامر نظام مع مدخلات غير موثوقة";
        rule.severity = VulnerabilitySeverity::Critical;
        rule.type = VulnerabilityType::CommandInjection;
        rule.cwe = CWECategory::CWE_78;
        rule.pattern_str = R"((نفذ_أمر|shell|system|exec)\s*\(\s*["'][^"']*["']\s*\+)";
        rule.pattern = std::regex(rule.pattern_str, std::regex::icase);
        rules_.push_back(rule);
    }
    
    // قاعدة: اختراق المسار
    {
        SecurityRule rule;
        rule.id = "SEC004";
        rule.name = "اختراق مسار الملفات";
        rule.description = "استخدام مدخلات في مسارات الملفات";
        rule.severity = VulnerabilitySeverity::High;
        rule.type = VulnerabilityType::PathTraversal;
        rule.cwe = CWECategory::CWE_20;
        rule.pattern_str = R"((افتح|اقرأ_ملف|اكتب_ملف|open|read_file)\s*\(\s*\w+\s*\+)";
        rule.pattern = std::regex(rule.pattern_str, std::regex::icase);
        rules_.push_back(rule);
    }
    
    // قاعدة: تسجيل معلومات حساسة
    {
        SecurityRule rule;
        rule.id = "SEC005";
        rule.name = "تسجيل معلومات حساسة";
        rule.description = "طباعة أو تسجيل كلمات مرور أو بيانات سرية";
        rule.severity = VulnerabilitySeverity::High;
        rule.type = VulnerabilityType::InsecureLogging;
        rule.cwe = CWECategory::CWE_200;
        rule.pattern_str = R"((اطبع|سجل|print|log)\s*\([^)]*(?:كلمة_المرور|password|سر|secret)[^)]*\))";
        rule.pattern = std::regex(rule.pattern_str, std::regex::icase);
        rules_.push_back(rule);
    }
    
    // قاعدة: عشوائية ضعيفة
    {
        SecurityRule rule;
        rule.id = "SEC006";
        rule.name = "عشوائية غير آمنة";
        rule.description = "استخدام دالة عشوائية غير آمنة لأغراض أمنية";
        rule.severity = VulnerabilitySeverity::Medium;
        rule.type = VulnerabilityType::InsecureRandom;
        rule.cwe = CWECategory::CWE_330;
        rule.pattern_str = R"((رمز|token|مفتاح|key|جلسة|session)\s*=\s*\w*عشوائي)";
        rule.pattern = std::regex(rule.pattern_str, std::regex::icase);
        rules_.push_back(rule);
    }
    
    // قاعدة: تشفير ضعيف
    {
        SecurityRule rule;
        rule.id = "SEC007";
        rule.name = "تشفير ضعيف";
        rule.description = "استخدام خوارزمية تشفير ضعيفة أو مهملة";
        rule.severity = VulnerabilitySeverity::High;
        rule.type = VulnerabilityType::WeakCryptography;
        rule.cwe = CWECategory::CWE_327;
        rule.pattern_str = R"((md5|sha1|des|rc4)\s*\()";
        rule.pattern = std::regex(rule.pattern_str, std::regex::icase);
        rules_.push_back(rule);
    }
    
    // قاعدة: eval/exec
    {
        SecurityRule rule;
        rule.id = "SEC008";
        rule.name = "تنفيذ كود ديناميكي";
        rule.description = "تنفيذ كود من نص قد يكون من مصدر غير موثوق";
        rule.severity = VulnerabilitySeverity::Critical;
        rule.type = VulnerabilityType::CodeInjection;
        rule.cwe = CWECategory::CWE_94;
        rule.pattern_str = R"((eval|قيّم|نفذ_كود)\s*\()";
        rule.pattern = std::regex(rule.pattern_str, std::regex::icase);
        rules_.push_back(rule);
    }
    
    // قاعدة: وضع التصحيح
    {
        SecurityRule rule;
        rule.id = "SEC009";
        rule.name = "وضع التصحيح مفعل";
        rule.description = "وضع التصحيح مفعل في الإنتاج";
        rule.severity = VulnerabilitySeverity::Medium;
        rule.type = VulnerabilityType::DebugModeEnabled;
        rule.cwe = CWECategory::CWE_200;
        rule.pattern_str = R"((debug|تصحيح)\s*=\s*(true|صحيح|1))";
        rule.pattern = std::regex(rule.pattern_str, std::regex::icase);
        rules_.push_back(rule);
    }
    
    // قاعدة: HTTP بدون تشفير
    {
        SecurityRule rule;
        rule.id = "SEC010";
        rule.name = "اتصال غير مشفر";
        rule.description = "استخدام HTTP بدلاً من HTTPS";
        rule.severity = VulnerabilitySeverity::Medium;
        rule.type = VulnerabilityType::SensitiveDataExposure;
        rule.cwe = CWECategory::CWE_311;
        rule.pattern_str = R"(http://(?!localhost|127\.0\.0\.1))";
        rule.pattern = std::regex(rule.pattern_str, std::regex::icase);
        rules_.push_back(rule);
    }
}

void SecurityAnalyzerProvider::initialize_taint_sources() {
    // مصادر الإدخال الموصومة
    taint_sources_ = {
        "ادخل",
        "اقرأ",
        "input",
        "read",
        "مدخل",
        "طلب",
        "request",
        "params",
        "معلمات"
    };
    
    // الوجهات الخطرة
    dangerous_sinks_ = {
        "استعلام",
        "query",
        "sql",
        "نفذ_أمر",
        "system",
        "shell",
        "exec",
        "eval",
        "قيّم",
        "افتح",
        "open",
        "اكتب_ملف"
    };
}

// ══════════════════════════════════════════════════════════════════════════════
//  التحليل الرئيسي
// ══════════════════════════════════════════════════════════════════════════════

SecurityReport SecurityAnalyzerProvider::analyze(
    const std::string& document,
    const std::string& uri
) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    SecurityReport report;
    report.document_uri = uri;
    
    // تطبيق كل القواعد
    for (const auto& rule : rules_) {
        if (!rule.enabled) continue;
        if (static_cast<int>(rule.severity) < static_cast<int>(min_severity_)) continue;
        
        std::sregex_iterator it(document.begin(), document.end(), rule.pattern);
        std::sregex_iterator end;
        
        while (it != end) {
            Vulnerability vuln;
            vuln.id = rule.id + "_" + std::to_string(it->position());
            vuln.type = rule.type;
            vuln.severity = rule.severity;
            vuln.cwe = rule.cwe;
            vuln.title = rule.name;
            vuln.description = rule.description;
            
            // الموقع
            vuln.location.line = get_line_number(document, it->position());
            vuln.location.column = 0;
            vuln.location.file_path = uri;
            
            // المقطع
            vuln.vulnerable_code = it->str();
            vuln.code_snippet = extract_code_snippet(document, vuln.location.line);
            
            // الثقة
            vuln.confidence = calculate_confidence(it->str(), vuln.code_snippet);
            
            if (vuln.confidence >= confidence_threshold_) {
                // الإصلاح المقترح
                auto fix = suggest_fix(vuln, vuln.code_snippet);
                if (fix) {
                    vuln.has_fix = true;
                    vuln.fix_code = *fix;
                }
                
                // إضافة التأثير
                switch (vuln.type) {
                    case VulnerabilityType::SQLInjection:
                    case VulnerabilityType::CommandInjection:
                        vuln.impact = "قد يسمح للمهاجم بتنفيذ أوامر على النظام";
                        break;
                    case VulnerabilityType::HardcodedCredentials:
                        vuln.impact = "قد يتم تسريب بيانات الاعتماد";
                        break;
                    case VulnerabilityType::PathTraversal:
                        vuln.impact = "قد يسمح بالوصول لملفات خارج المجلد المسموح";
                        break;
                    default:
                        vuln.impact = "ثغرة أمنية محتملة";
                }
                
                // إضافة المراجع
                vuln.references.push_back(get_cwe_url(vuln.cwe));
                
                report.vulnerabilities.push_back(vuln);
            }
            
            ++it;
        }
    }
    
    // تحليل تدفق البيانات
    if (taint_analysis_enabled_) {
        report.taint_flows = analyze_taint_flow(document);
        
        // تحويل تدفقات الموصومات لثغرات
        for (const auto& flow : report.taint_flows) {
            Vulnerability vuln;
            vuln.id = "TAINT_" + flow.variable;
            vuln.type = flow.potential_vuln;
            vuln.severity = VulnerabilitySeverity::High;
            vuln.title = "تدفق بيانات موصومة";
            vuln.description = "البيانات من '" + flow.source + 
                              "' تصل إلى '" + flow.sink + "' بدون تعقيم";
            vuln.confidence = 0.7;
            
            if (!flow.path.empty()) {
                vuln.location = flow.path.back();
            }
            
            report.vulnerabilities.push_back(vuln);
        }
    }
    
    // إحصائيات
    for (const auto& vuln : report.vulnerabilities) {
        switch (vuln.severity) {
            case VulnerabilitySeverity::Critical:
                report.critical_count++;
                break;
            case VulnerabilitySeverity::High:
                report.high_count++;
                break;
            case VulnerabilitySeverity::Medium:
                report.medium_count++;
                break;
            case VulnerabilitySeverity::Low:
                report.low_count++;
                break;
        }
    }
    
    // حساب تصنيف المخاطر
    report.risk_score = report.critical_count * 40 + 
                        report.high_count * 20 + 
                        report.medium_count * 5 + 
                        report.low_count;
    
    if (report.risk_score == 0) {
        report.risk_grade = 'A';
        report.summary = "لم يتم العثور على ثغرات أمنية!";
    } else if (report.risk_score < 10) {
        report.risk_grade = 'B';
        report.summary = "مستوى أمان جيد مع بعض الملاحظات البسيطة";
    } else if (report.risk_score < 30) {
        report.risk_grade = 'C';
        report.summary = "توجد ثغرات تحتاج للمعالجة";
    } else if (report.risk_score < 60) {
        report.risk_grade = 'D';
        report.summary = "توجد ثغرات خطيرة تتطلب إصلاحاً فورياً";
    } else {
        report.risk_grade = 'F';
        report.summary = "مستوى أمان حرج! يجب إصلاح الثغرات فوراً";
    }
    
    // التوصيات
    if (report.critical_count > 0) {
        report.recommendations.push_back("أصلح الثغرات الحرجة فوراً");
    }
    if (report.high_count > 0) {
        report.recommendations.push_back("راجع الثغرات عالية الخطورة");
    }
    report.recommendations.push_back("استخدم متغيرات البيئة للبيانات السرية");
    report.recommendations.push_back("استخدم prepared statements لاستعلامات SQL");
    report.recommendations.push_back("تحقق من صحة جميع المدخلات قبل استخدامها");
    
    // وقت التحليل
    auto end_time = std::chrono::high_resolution_clock::now();
    report.analysis_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time
    );
    
    return report;
}

std::vector<Vulnerability> SecurityAnalyzerProvider::quick_scan(
    const std::string& document
) {
    std::vector<Vulnerability> vulns;
    
    // فحص سريع للثغرات الحرجة فقط
    for (const auto& rule : rules_) {
        if (rule.severity != VulnerabilitySeverity::Critical) continue;
        if (!rule.enabled) continue;
        
        std::sregex_iterator it(document.begin(), document.end(), rule.pattern);
        while (it != std::sregex_iterator()) {
            Vulnerability vuln;
            vuln.id = rule.id;
            vuln.type = rule.type;
            vuln.severity = rule.severity;
            vuln.title = rule.name;
            vuln.description = rule.description;
            vuln.vulnerable_code = it->str();
            vuln.location.line = get_line_number(document, it->position());
            vulns.push_back(vuln);
            ++it;
        }
    }
    
    return vulns;
}

std::vector<Vulnerability> SecurityAnalyzerProvider::analyze_range(
    const std::string& document,
    int start_line,
    int end_line
) {
    // استخراج النطاق
    std::vector<std::string> lines;
    std::istringstream stream(document);
    std::string line;
    int line_num = 0;
    std::string range_code;
    
    while (std::getline(stream, line)) {
        if (line_num >= start_line && line_num <= end_line) {
            range_code += line + "\n";
        }
        line_num++;
    }
    
    // تحليل النطاق
    auto vulns = quick_scan(range_code);
    
    // تعديل أرقام الأسطر
    for (auto& v : vulns) {
        v.location.line += start_line;
    }
    
    return vulns;
}

// ══════════════════════════════════════════════════════════════════════════════
//  فحوصات محددة
// ══════════════════════════════════════════════════════════════════════════════

std::vector<Vulnerability> SecurityAnalyzerProvider::find_hardcoded_credentials(
    const std::string& code
) {
    std::vector<Vulnerability> vulns;
    
    for (const auto& pattern_str : patterns::HARDCODED_CREDENTIALS_PATTERNS) {
        std::regex pattern(pattern_str, std::regex::icase);
        std::sregex_iterator it(code.begin(), code.end(), pattern);
        
        while (it != std::sregex_iterator()) {
            Vulnerability vuln;
            vuln.id = "CRED_" + std::to_string(it->position());
            vuln.type = VulnerabilityType::HardcodedCredentials;
            vuln.severity = VulnerabilitySeverity::Critical;
            vuln.cwe = CWECategory::CWE_798;
            vuln.title = "بيانات اعتماد ثابتة";
            vuln.description = "لا تضع كلمات المرور أو المفاتيح في الكود";
            vuln.vulnerable_code = it->str();
            vuln.location.line = get_line_number(code, it->position());
            
            // إصلاح مقترح
            vuln.has_fix = true;
            vuln.fix_code = "## استخدم متغير بيئة بدلاً من ذلك\n"
                           "## متغير كلمة_المرور = بيئة.اقرأ(\"PASSWORD\")";
            vuln.remediation = "استخدم متغيرات البيئة أو ملف إعداد آمن";
            
            vulns.push_back(vuln);
            ++it;
        }
    }
    
    return vulns;
}

std::vector<Vulnerability> SecurityAnalyzerProvider::find_injection_vulnerabilities(
    const std::string& code
) {
    std::vector<Vulnerability> vulns;
    
    // حقن SQL
    for (const auto& pattern_str : patterns::SQL_INJECTION_PATTERNS) {
        std::regex pattern(pattern_str, std::regex::icase);
        std::sregex_iterator it(code.begin(), code.end(), pattern);
        
        while (it != std::sregex_iterator()) {
            Vulnerability vuln;
            vuln.id = "SQLI_" + std::to_string(it->position());
            vuln.type = VulnerabilityType::SQLInjection;
            vuln.severity = VulnerabilitySeverity::Critical;
            vuln.cwe = CWECategory::CWE_89;
            vuln.title = "حقن SQL";
            vuln.description = "دمج نص مع استعلام SQL يعرض للحقن";
            vuln.vulnerable_code = it->str();
            vuln.location.line = get_line_number(code, it->position());
            
            vuln.has_fix = true;
            vuln.fix_code = "## استخدم استعلام معلمي\n"
                           "استعلام_معلمي(\"SELECT * FROM users WHERE id = ?\", [المعرف])";
            vuln.remediation = "استخدم prepared statements";
            
            vulns.push_back(vuln);
            ++it;
        }
    }
    
    // حقن أوامر
    for (const auto& pattern_str : patterns::COMMAND_INJECTION_PATTERNS) {
        std::regex pattern(pattern_str, std::regex::icase);
        std::sregex_iterator it(code.begin(), code.end(), pattern);
        
        while (it != std::sregex_iterator()) {
            Vulnerability vuln;
            vuln.id = "CMDI_" + std::to_string(it->position());
            vuln.type = VulnerabilityType::CommandInjection;
            vuln.severity = VulnerabilitySeverity::Critical;
            vuln.cwe = CWECategory::CWE_78;
            vuln.title = "حقن أوامر";
            vuln.description = "تنفيذ أمر نظام مع مدخلات غير موثوقة";
            vuln.vulnerable_code = it->str();
            vuln.location.line = get_line_number(code, it->position());
            
            vuln.has_fix = true;
            vuln.fix_code = "## تحقق من المدخل وعقمه\n"
                           "إذا تطابق(المدخل, r\"^[a-zA-Z0-9]+$\")\n"
                           "    نفذ_أمر_آمن(المدخل)\nنهاية";
            vuln.remediation = "تجنب تنفيذ أوامر النظام أو تحقق بدقة من المدخلات";
            
            vulns.push_back(vuln);
            ++it;
        }
    }
    
    return vulns;
}

std::vector<Vulnerability> SecurityAnalyzerProvider::find_cryptography_issues(
    const std::string& code
) {
    std::vector<Vulnerability> vulns;
    
    // خوارزميات ضعيفة
    std::regex weak_algo(R"((md5|sha1|des|rc4|rot13)\s*\()", std::regex::icase);
    std::sregex_iterator it(code.begin(), code.end(), weak_algo);
    
    while (it != std::sregex_iterator()) {
        Vulnerability vuln;
        vuln.id = "CRYPTO_" + std::to_string(it->position());
        vuln.type = VulnerabilityType::WeakCryptography;
        vuln.severity = VulnerabilitySeverity::High;
        vuln.cwe = CWECategory::CWE_327;
        vuln.title = "تشفير ضعيف";
        vuln.description = "الخوارزمية المستخدمة مهملة وغير آمنة";
        vuln.vulnerable_code = it->str();
        vuln.location.line = get_line_number(code, it->position());
        
        vuln.has_fix = true;
        vuln.fix_code = "## استخدم خوارزمية آمنة\n"
                       "sha256(البيانات)  ## أو bcrypt لكلمات المرور";
        vuln.remediation = "استخدم SHA-256 أو أحدث، و bcrypt/argon2 لكلمات المرور";
        
        vulns.push_back(vuln);
        ++it;
    }
    
    return vulns;
}

std::vector<Vulnerability> SecurityAnalyzerProvider::find_input_validation_issues(
    const std::string& code
) {
    std::vector<Vulnerability> vulns;
    
    // اختراق المسار
    for (const auto& pattern_str : patterns::PATH_TRAVERSAL_PATTERNS) {
        std::regex pattern(pattern_str, std::regex::icase);
        std::sregex_iterator it(code.begin(), code.end(), pattern);
        
        while (it != std::sregex_iterator()) {
            Vulnerability vuln;
            vuln.id = "PATH_" + std::to_string(it->position());
            vuln.type = VulnerabilityType::PathTraversal;
            vuln.severity = VulnerabilitySeverity::High;
            vuln.cwe = CWECategory::CWE_20;
            vuln.title = "اختراق مسار محتمل";
            vuln.description = "استخدام مدخل في مسار ملف بدون تحقق";
            vuln.vulnerable_code = it->str();
            vuln.location.line = get_line_number(code, it->position());
            
            vuln.has_fix = true;
            vuln.fix_code = "## تحقق من المسار\n"
                           "متغير مسار_آمن = انضم(المجلد_الأساسي, اسم_الملف)\n"
                           "إذا يبدأ_ب(مسار_آمن, المجلد_الأساسي)\n"
                           "    افتح(مسار_آمن)\nنهاية";
            
            vulns.push_back(vuln);
            ++it;
        }
    }
    
    return vulns;
}

std::vector<Vulnerability> SecurityAnalyzerProvider::find_information_disclosure(
    const std::string& code
) {
    std::vector<Vulnerability> vulns;
    
    // تسجيل معلومات حساسة
    for (const auto& pattern_str : patterns::SENSITIVE_LOGGING_PATTERNS) {
        std::regex pattern(pattern_str, std::regex::icase);
        std::sregex_iterator it(code.begin(), code.end(), pattern);
        
        while (it != std::sregex_iterator()) {
            Vulnerability vuln;
            vuln.id = "INFO_" + std::to_string(it->position());
            vuln.type = VulnerabilityType::InsecureLogging;
            vuln.severity = VulnerabilitySeverity::High;
            vuln.cwe = CWECategory::CWE_200;
            vuln.title = "تسجيل معلومات حساسة";
            vuln.description = "طباعة أو تسجيل بيانات سرية";
            vuln.vulnerable_code = it->str();
            vuln.location.line = get_line_number(code, it->position());
            
            vuln.has_fix = true;
            vuln.fix_code = "## لا تسجل معلومات حساسة\n"
                           "اطبع(\"تم تسجيل الدخول\")  ## بدون كلمة المرور";
            
            vulns.push_back(vuln);
            ++it;
        }
    }
    
    return vulns;
}

// ══════════════════════════════════════════════════════════════════════════════
//  تحليل تدفق البيانات
// ══════════════════════════════════════════════════════════════════════════════

std::vector<TaintedDataFlow> SecurityAnalyzerProvider::analyze_taint_flow(
    const std::string& code
) {
    std::vector<TaintedDataFlow> flows;
    
    // تتبع المتغيرات الموصومة
    std::map<std::string, std::string> tainted_vars;  // var -> source
    
    std::istringstream stream(code);
    std::string line;
    int line_num = 0;
    
    while (std::getline(stream, line)) {
        // البحث عن مصادر موصومة
        for (const auto& source : taint_sources_) {
            if (line.find(source) != std::string::npos) {
                // استخراج المتغير
                std::regex assign_regex(R"(متغير\s+(\w+)\s*=.*)" + source);
                std::smatch match;
                if (std::regex_search(line, match, assign_regex)) {
                    tainted_vars[match[1].str()] = source;
                }
            }
        }
        
        // البحث عن استخدام موصوم في حوض خطر
        for (const auto& [var, source] : tainted_vars) {
            for (const auto& sink : dangerous_sinks_) {
                std::string pattern = sink + R"(\s*\([^)]*)" + var;
                if (std::regex_search(line, std::regex(pattern))) {
                    TaintedDataFlow flow;
                    flow.source = source;
                    flow.variable = var;
                    flow.sink = sink;
                    
                    CodeLocation loc;
                    loc.line = line_num;
                    flow.path.push_back(loc);
                    
                    // تحديد نوع الثغرة
                    if (sink == "استعلام" || sink == "sql" || sink == "query") {
                        flow.potential_vuln = VulnerabilityType::SQLInjection;
                    } else if (sink == "نفذ_أمر" || sink == "shell" || sink == "system") {
                        flow.potential_vuln = VulnerabilityType::CommandInjection;
                    } else if (sink == "eval" || sink == "قيّم") {
                        flow.potential_vuln = VulnerabilityType::CodeInjection;
                    } else if (sink == "افتح" || sink == "open") {
                        flow.potential_vuln = VulnerabilityType::PathTraversal;
                    } else {
                        flow.potential_vuln = VulnerabilityType::Injection;
                    }
                    
                    flows.push_back(flow);
                }
            }
        }
        
        line_num++;
    }
    
    return flows;
}

// ══════════════════════════════════════════════════════════════════════════════
//  الإصلاحات
// ══════════════════════════════════════════════════════════════════════════════

std::optional<std::string> SecurityAnalyzerProvider::suggest_fix(
    const Vulnerability& vuln,
    const std::string& context
) {
    switch (vuln.type) {
        case VulnerabilityType::HardcodedCredentials:
            return "## استخدم متغير بيئة\n"
                   "متغير كلمة_المرور = بيئة.اقرأ(\"PASSWORD\")\n"
                   "إذا كلمة_المرور == عدم\n"
                   "    ارفع خطأ(\"يجب تعيين متغير البيئة PASSWORD\")\n"
                   "نهاية";
        
        case VulnerabilityType::SQLInjection:
            return "## استخدم استعلام معلمي\n"
                   "## بدلاً من: استعلام(\"SELECT * FROM users WHERE id = \" + المعرف)\n"
                   "استعلام_معلمي(\"SELECT * FROM users WHERE id = ?\", [المعرف])";
        
        case VulnerabilityType::CommandInjection:
            return "## تجنب تنفيذ أوامر النظام\n"
                   "## إذا ضروري، تحقق من المدخل:\n"
                   "متغير نمط_آمن = r\"^[a-zA-Z0-9_-]+$\"\n"
                   "إذا لا تطابق(المدخل, نمط_آمن)\n"
                   "    ارفع خطأ(\"مدخل غير صالح\")\n"
                   "نهاية\n"
                   "نفذ_أمر_قائمة([\"command\", المدخل])";
        
        case VulnerabilityType::PathTraversal:
            return "## تحقق من المسار\n"
                   "استورد مسار\n"
                   "متغير المجلد_الآمن = \"/app/data\"\n"
                   "متغير المسار_الكامل = مسار.مطلق(مسار.انضم(المجلد_الآمن, اسم_الملف))\n"
                   "إذا لا يبدأ_ب(المسار_الكامل, المجلد_الآمن)\n"
                   "    ارفع خطأ(\"مسار غير مسموح\")\n"
                   "نهاية";
        
        case VulnerabilityType::WeakCryptography:
            return "## استخدم خوارزمية آمنة\n"
                   "## للتجزئة العامة:\n"
                   "استورد تشفير\n"
                   "متغير التجزئة = تشفير.sha256(البيانات)\n"
                   "## لكلمات المرور:\n"
                   "متغير التجزئة = تشفير.bcrypt_تجزئة(كلمة_المرور)";
        
        case VulnerabilityType::InsecureRandom:
            return "## استخدم عشوائية آمنة\n"
                   "استورد أمان\n"
                   "متغير الرمز = أمان.رمز_عشوائي_آمن(32)";
        
        case VulnerabilityType::InsecureLogging:
            return "## لا تسجل معلومات حساسة\n"
                   "سجل(\"تم تسجيل دخول المستخدم: \" + اسم_المستخدم)\n"
                   "## بدلاً من:\n"
                   "## سجل(\"كلمة المرور: \" + كلمة_المرور)";
        
        default:
            return std::nullopt;
    }
}

std::string SecurityAnalyzerProvider::generate_secure_alternative(
    const std::string& vulnerable_code,
    VulnerabilityType type
) {
    // توليد كود آمن بديل بناءً على النوع
    Vulnerability temp_vuln;
    temp_vuln.type = type;
    auto fix = suggest_fix(temp_vuln, vulnerable_code);
    return fix.value_or("## راجع التوثيق للحصول على بديل آمن");
}

// ══════════════════════════════════════════════════════════════════════════════
//  إدارة القواعد
// ══════════════════════════════════════════════════════════════════════════════

void SecurityAnalyzerProvider::add_rule(const SecurityRule& rule) {
    rules_.push_back(rule);
}

void SecurityAnalyzerProvider::set_rule_enabled(
    const std::string& rule_id,
    bool enabled
) {
    for (auto& rule : rules_) {
        if (rule.id == rule_id) {
            rule.enabled = enabled;
            break;
        }
    }
}

std::vector<SecurityRule> SecurityAnalyzerProvider::get_rules() const {
    return rules_;
}

// ══════════════════════════════════════════════════════════════════════════════
//  أدوات داخلية
// ══════════════════════════════════════════════════════════════════════════════

int SecurityAnalyzerProvider::get_line_number(
    const std::string& code,
    size_t pos
) {
    int line = 0;
    for (size_t i = 0; i < pos && i < code.size(); i++) {
        if (code[i] == '\n') line++;
    }
    return line;
}

std::string SecurityAnalyzerProvider::extract_code_snippet(
    const std::string& code,
    int line,
    int context
) {
    std::vector<std::string> lines;
    std::istringstream stream(code);
    std::string l;
    while (std::getline(stream, l)) {
        lines.push_back(l);
    }
    
    int start = std::max(0, line - context);
    int end = std::min(static_cast<int>(lines.size()), line + context + 1);
    
    std::ostringstream snippet;
    for (int i = start; i < end; i++) {
        snippet << (i == line ? ">>> " : "    ") << lines[i] << "\n";
    }
    
    return snippet.str();
}

double SecurityAnalyzerProvider::calculate_confidence(
    const std::string& match,
    const std::string& context
) {
    double confidence = 0.7;  // افتراضي
    
    // زيادة الثقة إذا كانت في سياق معين
    if (context.find("## TEST") != std::string::npos ||
        context.find("## اختبار") != std::string::npos) {
        confidence *= 0.3;  // تقليل في كود الاختبار
    }
    
    // تقليل الثقة للتعليقات
    if (match.find("##") != std::string::npos) {
        confidence *= 0.5;
    }
    
    // زيادة للأنماط الواضحة
    if (match.find("password") != std::string::npos ||
        match.find("كلمة_المرور") != std::string::npos) {
        confidence *= 1.3;
    }
    
    return std::min(1.0, confidence);
}

// ══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة عامة
// ══════════════════════════════════════════════════════════════════════════════

std::string severity_to_string(VulnerabilitySeverity severity) {
    switch (severity) {
        case VulnerabilitySeverity::Critical: return "حرج";
        case VulnerabilitySeverity::High: return "عالي";
        case VulnerabilitySeverity::Medium: return "متوسط";
        case VulnerabilitySeverity::Low: return "منخفض";
        default: return "غير معروف";
    }
}

std::string severity_to_color(VulnerabilitySeverity severity) {
    switch (severity) {
        case VulnerabilitySeverity::Critical: return "#FF0000";
        case VulnerabilitySeverity::High: return "#FF6600";
        case VulnerabilitySeverity::Medium: return "#FFCC00";
        case VulnerabilitySeverity::Low: return "#00CC00";
        default: return "#CCCCCC";
    }
}

std::string vulnerability_type_to_string(VulnerabilityType type) {
    switch (type) {
        case VulnerabilityType::Injection: return "حقن";
        case VulnerabilityType::SQLInjection: return "حقن SQL";
        case VulnerabilityType::CommandInjection: return "حقن أوامر";
        case VulnerabilityType::CodeInjection: return "حقن كود";
        case VulnerabilityType::BrokenAuthentication: return "مصادقة مكسورة";
        case VulnerabilityType::WeakPassword: return "كلمة مرور ضعيفة";
        case VulnerabilityType::HardcodedCredentials: return "بيانات ثابتة";
        case VulnerabilityType::InsecureTokens: return "رموز غير آمنة";
        case VulnerabilityType::SensitiveDataExposure: return "كشف بيانات";
        case VulnerabilityType::DataLeakage: return "تسرب بيانات";
        case VulnerabilityType::InsecureLogging: return "تسجيل غير آمن";
        case VulnerabilityType::WeakCryptography: return "تشفير ضعيف";
        case VulnerabilityType::InsecureRandom: return "عشوائية ضعيفة";
        case VulnerabilityType::PlaintextStorage: return "تخزين غير مشفر";
        case VulnerabilityType::InputValidation: return "نقص التحقق";
        case VulnerabilityType::PathTraversal: return "اختراق مسار";
        case VulnerabilityType::XSS: return "XSS";
        case VulnerabilityType::InsecureSession: return "جلسة غير آمنة";
        case VulnerabilityType::SessionFixation: return "تثبيت جلسة";
        case VulnerabilityType::BrokenAccessControl: return "تحكم وصول مكسور";
        case VulnerabilityType::PrivilegeEscalation: return "تصعيد صلاحيات";
        case VulnerabilityType::IDOR: return "IDOR";
        case VulnerabilityType::SecurityMisconfiguration: return "تهيئة خاطئة";
        case VulnerabilityType::DebugModeEnabled: return "وضع تصحيح";
        case VulnerabilityType::InsecureDeserialization: return "تفكيك غير آمن";
        case VulnerabilityType::SSRF: return "SSRF";
        case VulnerabilityType::RaceCondition: return "حالة تسابق";
        case VulnerabilityType::ResourceLeak: return "تسرب موارد";
        case VulnerabilityType::UnsafeReflection: return "انعكاس غير آمن";
        default: return "غير معروف";
    }
}

std::string cwe_to_string(CWECategory cwe) {
    switch (cwe) {
        case CWECategory::CWE_20: return "CWE-20";
        case CWECategory::CWE_74: return "CWE-74";
        case CWECategory::CWE_78: return "CWE-78";
        case CWECategory::CWE_89: return "CWE-89";
        case CWECategory::CWE_94: return "CWE-94";
        case CWECategory::CWE_200: return "CWE-200";
        case CWECategory::CWE_259: return "CWE-259";
        case CWECategory::CWE_287: return "CWE-287";
        case CWECategory::CWE_311: return "CWE-311";
        case CWECategory::CWE_327: return "CWE-327";
        case CWECategory::CWE_330: return "CWE-330";
        case CWECategory::CWE_352: return "CWE-352";
        case CWECategory::CWE_400: return "CWE-400";
        case CWECategory::CWE_502: return "CWE-502";
        case CWECategory::CWE_522: return "CWE-522";
        case CWECategory::CWE_601: return "CWE-601";
        case CWECategory::CWE_732: return "CWE-732";
        case CWECategory::CWE_798: return "CWE-798";
        case CWECategory::CWE_918: return "CWE-918";
        default: return "Unknown";
    }
}

std::string get_cwe_url(CWECategory cwe) {
    return "https://cwe.mitre.org/data/definitions/" + 
           cwe_to_string(cwe).substr(4) + ".html";
}

std::string risk_grade_emoji(char grade) {
    switch (grade) {
        case 'A': return "🛡️";
        case 'B': return "✅";
        case 'C': return "⚠️";
        case 'D': return "❌";
        case 'F': return "🚨";
        default: return "❓";
    }
}

} // namespace security
} // namespace lsp
} // namespace sad
