// بسم الله الرحمن الرحيم
// ============================================================================
// test_security_scanner.cpp — اختبارات شاملة للحارس الأمني
// Comprehensive Security Scanner Tests
// ============================================================================

#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <filesystem>

#include "security/security_scanner.h"

using namespace Sad::Security;
namespace fs = std::filesystem;

// ============================================================================
// مساعدات الاختبار
// ============================================================================

struct TestStats {
    int total = 0;
    int passed = 0;
    int failed = 0;
    void record(bool ok) { total++; if (ok) passed++; else failed++; }
    void print() const {
        std::cout << "\n═══════════════════════════════════════════\n"
                  << "  📊 النتائج: " << total << " اختبار | ✅ " << passed
                  << " نجح | ❌ " << failed << " فشل\n"
                  << "═══════════════════════════════════════════\n";
    }
};

static TestStats stats;

#define TEST(name) \
    do { \
        bool _ok = true; \
        std::string _name = name;

#define EXPECT(cond) \
    if (!(cond)) { \
        _ok = false; \
        std::cout << "    ✗ فشل: " << #cond << " (سطر " << __LINE__ << ")\n"; \
    }

#define END_TEST \
        stats.record(_ok); \
        std::cout << (_ok ? "  ✓ " : "  ✗ ") << _name << "\n"; \
    } while(0)

// ============================================================================
// 1. اختبارات تكوين الماسح
// ============================================================================

void testScannerConfig() {
    std::cout << "\n╔══════════════════════════════════════════╗\n"
              << "║  اختبارات تكوين الماسح الأمني           ║\n"
              << "╚══════════════════════════════════════════╝\n";

    TEST("إنشاء تكوين افتراضي") {
        ScannerConfig cfg;
        EXPECT(cfg.verbose == false);
        EXPECT(cfg.criticalOnly == false);
        EXPECT(cfg.taintAnalysis == true);
        EXPECT(cfg.patternScan == true);
        EXPECT(cfg.reportFormat == ReportFormat::Text);
        EXPECT(cfg.minSeverity == Severity::Low);
    } END_TEST;

    TEST("تكوين من ملف") {
        // إنشاء ملف تكوين مؤقت
        std::string tmpPath = "test_guard_config.tmp";
        {
            std::ofstream f(tmpPath);
            f << "verbose=true\n"
              << "critical_only=false\n"
              << "taint_analysis=true\n"
              << "report_format=json\n"
              << "min_severity=high\n"
              << "disable_rule=SEC003\n";
        }
        auto cfg = ScannerConfig::fromFile(tmpPath);
        EXPECT(cfg.verbose == true);
        EXPECT(cfg.reportFormat == ReportFormat::JSON);
        EXPECT(cfg.minSeverity == Severity::High);
        EXPECT(cfg.disabledRules.size() == 1);
        EXPECT(cfg.disabledRules[0] == "SEC003");
        fs::remove(tmpPath);
    } END_TEST;

    TEST("تكوين من ملف غير موجود") {
        auto cfg = ScannerConfig::fromFile("nonexistent_file_12345.cfg");
        EXPECT(cfg.verbose == false);  // القيم الافتراضية
    } END_TEST;
}

// ============================================================================
// 2. اختبارات تهيئة القواعد
// ============================================================================

void testRuleInitialization() {
    std::cout << "\n╔══════════════════════════════════════════╗\n"
              << "║  اختبارات تهيئة القواعد الأمنية         ║\n"
              << "╚══════════════════════════════════════════╝\n";

    TEST("تهيئة جميع القواعد") {
        ScannerConfig cfg;
        SecurityScanner scanner(cfg);
        // الماسح يجب أن يبدأ بـ 15+ قاعدة
        // نختبر عبر فحص مصدر فارغ (لا نتائج)
        auto result = scanner.scanSource("", "test.ص");
        EXPECT(result.filesScanned == 1);
        EXPECT(result.findings.empty());
    } END_TEST;

    TEST("تعطيل قاعدة محددة") {
        ScannerConfig cfg;
        cfg.disabledRules.push_back("SQL001");
        SecurityScanner scanner(cfg);
        std::string code = R"(متغير بيانات = نفّذ_SQL("SELECT * FROM users WHERE id=" + مُدخل))";
        auto result = scanner.scanSource(code, "test.ص");
        // SQL001 معطلة، لكن قد تُكتشف بقواعد أخرى
        bool hasSql001 = false;
        for (const auto& f : result.findings)
            if (f.ruleId == "SQL001") hasSql001 = true;
        EXPECT(!hasSql001);
    } END_TEST;

    TEST("تفعيل قواعد محددة فقط") {
        ScannerConfig cfg;
        cfg.enabledRules.push_back("CRY001");
        SecurityScanner scanner(cfg);
        std::string code = "متغير hash = MD5(بيانات)\nمتغير سر = password=\"abc123\"";
        auto result = scanner.scanSource(code, "test.ص");
        for (const auto& f : result.findings)
            EXPECT(f.ruleId == "CRY001");
    } END_TEST;
}

// ============================================================================
// 3. اختبارات كشف حقن SQL
// ============================================================================

void testSQLInjection() {
    std::cout << "\n╔══════════════════════════════════════════╗\n"
              << "║  اختبارات كشف حقن SQL                   ║\n"
              << "╚══════════════════════════════════════════╝\n";

    ScannerConfig cfg;
    cfg.taintAnalysis = false;
    SecurityScanner scanner(cfg);

    TEST("كشف نفّذ_SQL مع دمج نصوص") {
        auto r = scanner.scanSource(
            "نفّذ_SQL(\"SELECT * FROM users WHERE id=\" + مُدخل)", "t.ص");
        bool found = false;
        for (const auto& f : r.findings)
            if (f.ruleId.find("SQL") != std::string::npos) found = true;
        EXPECT(found);
    } END_TEST;

    TEST("كشف استعلم مباشر") {
        auto r = scanner.scanSource(
            "استعلم(\"DELETE FROM logs WHERE date<\" + تاريخ)", "t.ص");
        bool found = false;
        for (const auto& f : r.findings)
            if (f.category == RuleCategory::Injection) found = true;
        EXPECT(found);
    } END_TEST;

    TEST("كود آمن — لا إنذار") {
        auto r = scanner.scanSource(
            "متغير اسم = \"محمد\"\nاطبع(اسم)", "t.ص");
        bool hasSql = false;
        for (const auto& f : r.findings)
            if (f.ruleId.find("SQL") != std::string::npos) hasSql = true;
        EXPECT(!hasSql);
    } END_TEST;
}

// ============================================================================
// 4. اختبارات كشف التشفير الضعيف
// ============================================================================

void testWeakCrypto() {
    std::cout << "\n╔══════════════════════════════════════════╗\n"
              << "║  اختبارات كشف التشفير الضعيف            ║\n"
              << "╚══════════════════════════════════════════╝\n";

    ScannerConfig cfg;
    cfg.taintAnalysis = false;
    SecurityScanner scanner(cfg);

    TEST("كشف MD5") {
        auto r = scanner.scanSource("متغير h = MD5(بيانات)", "t.ص");
        bool found = false;
        for (const auto& f : r.findings)
            if (f.ruleId == "CRY001") found = true;
        EXPECT(found);
    } END_TEST;

    TEST("كشف SHA1") {
        auto r = scanner.scanSource("متغير h = SHA1(ملف)", "t.ص");
        bool found = false;
        for (const auto& f : r.findings)
            if (f.ruleId == "CRY001") found = true;
        EXPECT(found);
    } END_TEST;

    TEST("كشف DES") {
        auto r = scanner.scanSource("متغير مشفر = DES(نص)", "t.ص");
        bool found = false;
        for (const auto& f : r.findings)
            if (f.category == RuleCategory::Cryptography) found = true;
        EXPECT(found);
    } END_TEST;

    TEST("SHA-256 آمن — لا إنذار تشفير") {
        auto r = scanner.scanSource("متغير h = SHA256(بيانات)", "t.ص");
        bool hasCry = false;
        for (const auto& f : r.findings)
            if (f.ruleId == "CRY001") hasCry = true;
        EXPECT(!hasCry);
    } END_TEST;
}

// ============================================================================
// 5. اختبارات كشف الأسرار الثابتة
// ============================================================================

void testHardcodedSecrets() {
    std::cout << "\n╔══════════════════════════════════════════╗\n"
              << "║  اختبارات كشف الأسرار الثابتة           ║\n"
              << "╚══════════════════════════════════════════╝\n";

    ScannerConfig cfg;
    cfg.taintAnalysis = false;
    SecurityScanner scanner(cfg);

    TEST("كشف كلمة مرور ثابتة") {
        auto r = scanner.scanSource("password=\"super_secret_123\"", "t.ص");
        bool found = false;
        for (const auto& f : r.findings)
            if (f.ruleId == "SEC003") found = true;
        EXPECT(found);
    } END_TEST;

    TEST("كشف مفتاح API ثابت") {
        auto r = scanner.scanSource("api_key=\"sk-1234567890abcdef\"", "t.ص");
        bool found = false;
        for (const auto& f : r.findings)
            if (f.ruleId == "SEC003") found = true;
        EXPECT(found);
    } END_TEST;

    TEST("كشف كلمة_مرور عربية") {
        auto r = scanner.scanSource("كلمة_مرور=\"سري_جداً\"", "t.ص");
        bool found = false;
        for (const auto& f : r.findings)
            if (f.category == RuleCategory::Secrets) found = true;
        EXPECT(found);
    } END_TEST;
}

// ============================================================================
// 6. اختبارات تنفيذ أوامر النظام
// ============================================================================

void testCommandInjection() {
    std::cout << "\n╔══════════════════════════════════════════╗\n"
              << "║  اختبارات كشف حقن الأوامر               ║\n"
              << "╚══════════════════════════════════════════╝\n";

    ScannerConfig cfg;
    cfg.taintAnalysis = false;
    SecurityScanner scanner(cfg);

    TEST("كشف نفّذ()") {
        auto r = scanner.scanSource("نفّذ(\"rm -rf /\")", "t.ص");
        bool found = false;
        for (const auto& f : r.findings)
            if (f.ruleId == "SEC002") found = true;
        EXPECT(found);
    } END_TEST;

    TEST("كشف system()") {
        auto r = scanner.scanSource("system(\"del /f /q *.*\")", "t.ص");
        bool found = false;
        for (const auto& f : r.findings)
            if (f.category == RuleCategory::Injection) found = true;
        EXPECT(found);
    } END_TEST;
}

// ============================================================================
// 7. اختبارات تحليل التلوّث (Taint Analysis)
// ============================================================================

void testTaintAnalysis() {
    std::cout << "\n╔══════════════════════════════════════════╗\n"
              << "║  اختبارات تحليل التلوّث                  ║\n"
              << "╚══════════════════════════════════════════╝\n";

    ScannerConfig cfg;
    cfg.patternScan = false;  // فقط تحليل التلوّث
    cfg.taintAnalysis = true;
    SecurityScanner scanner(cfg);

    TEST("تتبع متغير من إدخال إلى SQL") {
        std::string code =
            "متغير مدخل = اقرأ_سطر()\n"
            "نفّذ_SQL(مدخل)\n";
        auto r = scanner.scanSource(code, "t.ص");
        bool found = false;
        for (const auto& f : r.findings)
            if (f.ruleId == "TAINT001") found = true;
        EXPECT(found);
    } END_TEST;

    TEST("تتبع متغير من إدخال إلى shell") {
        std::string code =
            "متغير أمر = اقرأ()\n"
            "نفّذ(أمر)\n";
        auto r = scanner.scanSource(code, "t.ص");
        bool found = false;
        for (const auto& f : r.findings)
            if (f.ruleId == "TAINT001") found = true;
        EXPECT(found);
    } END_TEST;

    TEST("لا تلوّث — متغير ثابت") {
        std::string code =
            "متغير اسم = \"ثابت\"\n"
            "اطبع(اسم)\n";
        auto r = scanner.scanSource(code, "t.ص");
        bool hasTaint = false;
        for (const auto& f : r.findings)
            if (f.ruleId == "TAINT001") hasTaint = true;
        EXPECT(!hasTaint);
    } END_TEST;
}

// ============================================================================
// 8. اختبارات تقارير الإخراج
// ============================================================================

void testReportFormatters() {
    std::cout << "\n╔══════════════════════════════════════════╗\n"
              << "║  اختبارات تقارير الإخراج                 ║\n"
              << "╚══════════════════════════════════════════╝\n";

    ScannerConfig cfg;
    SecurityScanner scanner(cfg);
    auto result = scanner.scanSource("password=\"test123\"\nnفّذ(\"ls\")", "t.ص");

    TEST("تقرير نصي") {
        auto text = SecurityReportFormatter::formatText(result, true);
        EXPECT(!text.empty());
        EXPECT(text.find("تقرير") != std::string::npos);
    } END_TEST;

    TEST("تقرير JSON") {
        auto json = SecurityReportFormatter::formatJSON(result);
        EXPECT(!json.empty());
        EXPECT(json.find("\"findings\"") != std::string::npos);
        EXPECT(json.find("\"summary\"") != std::string::npos);
    } END_TEST;

    TEST("تقرير HTML") {
        auto html = SecurityReportFormatter::formatHTML(result);
        EXPECT(!html.empty());
        EXPECT(html.find("<!DOCTYPE html>") != std::string::npos);
        EXPECT(html.find("rtl") != std::string::npos);
    } END_TEST;

    TEST("تقرير SARIF") {
        auto sarif = SecurityReportFormatter::formatSARIF(result);
        EXPECT(!sarif.empty());
        EXPECT(sarif.find("sarif-schema") != std::string::npos);
        EXPECT(sarif.find("sad-guard") != std::string::npos);
    } END_TEST;

    TEST("حفظ تقرير لملف") {
        std::string tmpFile = "test_report_output.tmp";
        bool ok = SecurityReportFormatter::writeReport(result, tmpFile, ReportFormat::JSON);
        EXPECT(ok);
        EXPECT(fs::exists(tmpFile));
        fs::remove(tmpFile);
    } END_TEST;
}

// ============================================================================
// 9. اختبارات Finding helpers
// ============================================================================

void testFindingHelpers() {
    std::cout << "\n╔══════════════════════════════════════════╗\n"
              << "║  اختبارات مساعدات Finding               ║\n"
              << "╚══════════════════════════════════════════╝\n";

    TEST("أيقونات الخطورة") {
        Finding f;
        f.severity = Severity::Critical;
        EXPECT(f.severityIcon() == "🔴");
        f.severity = Severity::High;
        EXPECT(f.severityIcon() == "🟠");
        f.severity = Severity::Medium;
        EXPECT(f.severityIcon() == "🟡");
        f.severity = Severity::Low;
        EXPECT(f.severityIcon() == "🔵");
        f.severity = Severity::Info;
        EXPECT(f.severityIcon() == "⚪");
    } END_TEST;

    TEST("أسماء الخطورة بالعربية") {
        Finding f;
        f.severity = Severity::Critical;
        EXPECT(f.severityNameAr() == "حرجة");
        f.severity = Severity::High;
        EXPECT(f.severityNameAr() == "عالية");
    } END_TEST;

    TEST("أسماء الفئات بالعربية") {
        Finding f;
        f.category = RuleCategory::Injection;
        EXPECT(f.categoryNameAr() == "حقن");
        f.category = RuleCategory::Cryptography;
        EXPECT(f.categoryNameAr() == "التشفير");
        f.category = RuleCategory::Memory;
        EXPECT(f.categoryNameAr() == "أمان الذاكرة");
    } END_TEST;
}

// ============================================================================
// 10. اختبارات ScanResult
// ============================================================================

void testScanResult() {
    std::cout << "\n╔══════════════════════════════════════════╗\n"
              << "║  اختبارات عدّادات ScanResult             ║\n"
              << "╚══════════════════════════════════════════╝\n";

    TEST("عدّادات الخطورة") {
        ScanResult r;
        r.findings.push_back({"R1", Severity::Critical, RuleCategory::Injection, "خ1", "f", 1, 0, "", "", "", 1.0, false, ""});
        r.findings.push_back({"R2", Severity::Critical, RuleCategory::Injection, "خ2", "f", 2, 0, "", "", "", 1.0, false, ""});
        r.findings.push_back({"R3", Severity::High, RuleCategory::Memory, "خ3", "f", 3, 0, "", "", "", 1.0, false, ""});
        r.findings.push_back({"R4", Severity::Medium, RuleCategory::Secrets, "خ4", "f", 4, 0, "", "", "", 1.0, false, ""});
        r.findings.push_back({"R5", Severity::Low, RuleCategory::Configuration, "خ5", "f", 5, 0, "", "", "", 1.0, false, ""});

        EXPECT(r.criticalCount() == 2);
        EXPECT(r.highCount() == 1);
        EXPECT(r.mediumCount() == 1);
        EXPECT(r.lowCount() == 1);
        EXPECT(r.hasCritical() == true);
    } END_TEST;

    TEST("نتيجة فارغة") {
        ScanResult r;
        EXPECT(r.criticalCount() == 0);
        EXPECT(r.hasCritical() == false);
        EXPECT(r.findings.empty());
    } END_TEST;
}

// ============================================================================
// 11. اختبارات تخطي التعليقات
// ============================================================================

void testCommentSkipping() {
    std::cout << "\n╔══════════════════════════════════════════╗\n"
              << "║  اختبارات تخطي التعليقات                 ║\n"
              << "╚══════════════════════════════════════════╝\n";

    ScannerConfig cfg;
    cfg.taintAnalysis = false;
    SecurityScanner scanner(cfg);

    TEST("تخطي سطور التعليقات") {
        auto r = scanner.scanSource("# password=\"test123\"", "t.ص");
        bool hasSecret = false;
        for (const auto& f : r.findings)
            if (f.ruleId == "SEC003") hasSecret = true;
        EXPECT(!hasSecret);
    } END_TEST;

    TEST("تخطي سطور noguard") {
        auto r = scanner.scanSource("password=\"test123\" // noguard", "t.ص");
        bool hasSecret = false;
        for (const auto& f : r.findings)
            if (f.ruleId == "SEC003") hasSecret = true;
        EXPECT(!hasSecret);
    } END_TEST;
}

// ============================================================================
// نقطة الدخول
// ============================================================================

int main() {
    std::cout << "╔══════════════════════════════════════════════════════╗\n"
              << "║  🛡️  اختبارات الحارس الأمني — لغة ص                 ║\n"
              << "╚══════════════════════════════════════════════════════╝\n";

    testScannerConfig();
    testRuleInitialization();
    testSQLInjection();
    testWeakCrypto();
    testHardcodedSecrets();
    testCommandInjection();
    testTaintAnalysis();
    testReportFormatters();
    testFindingHelpers();
    testScanResult();
    testCommentSkipping();

    stats.print();
    return stats.failed > 0 ? 1 : 0;
}
