/**
 * =============================================================================
 * @file sad_test.h
 * @brief (AR) إطار الاختبارات — Test Framework بلغة ص
 * @brief (EN) Test Framework — Testing in Sad Language
 * 
 * المهمة: T253 - Test Framework بلغة ص
 * المرحلة: سبتمبر 2026 — Test Framework
 * =============================================================================
 * 
 * 🎯 الميزات
 * ══════════
 * ✓ اختبر { } نهاية
 * ✓ تأكد(...) assertions
 * ✓ قبل(), بعد() — setup/teardown
 * ✓ تجميع الاختبارات (suites)
 * ✓ تقارير (console, HTML, XML)
 * ✓ اكتشاف الاختبارات تلقائياً
 * 
 * 📝 مثال على الاستخدام
 * ══════════════════════
 * ```ص
 * اختبر "جمع الأرقام"
 *     تأكد(1 + 1 == 2)
 *     تأكد_مساواة(جمع(2، 3)، 5)
 * نهاية
 * ```
 * 
 * =============================================================================
 */

#ifndef SAD_STDLIB_TEST_H
#define SAD_STDLIB_TEST_H

#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <map>

namespace sad {
namespace test {

// =============================================================================
// الأنواع الأساسية
// =============================================================================

/// نتيجة اختبار
enum class TestResult {
    PASSED,     // ✓ نجح
    FAILED,     // ✗ فشل
    SKIPPED,    // ○ تم تخطيه
    ERROR       // ! خطأ
};

/// تفاصيل فشل
struct FailureInfo {
    std::string message;        // رسالة الخطأ
    std::string file;           // الملف
    int line = 0;               // السطر
    std::string expected;       // القيمة المتوقعة
    std::string actual;         // القيمة الفعلية
};

/// معلومات اختبار واحد
struct TestInfo {
    std::string name;           // اسم الاختبار
    std::string suite;          // اسم المجموعة
    TestResult result = TestResult::PASSED;
    double durationMs = 0;      // المدة بالمللي ثانية
    std::vector<FailureInfo> failures;
    std::string output;         // المخرجات
};

/// معلومات مجموعة اختبارات
struct TestSuite {
    std::string name;
    std::vector<TestInfo> tests;
    std::function<void()> beforeAll;
    std::function<void()> afterAll;
    std::function<void()> beforeEach;
    std::function<void()> afterEach;
    
    int passedCount = 0;
    int failedCount = 0;
    int skippedCount = 0;
    double totalDurationMs = 0;
};

/// إحصائيات التشغيل
struct TestStats {
    int totalTests = 0;
    int passedTests = 0;
    int failedTests = 0;
    int skippedTests = 0;
    double totalDurationMs = 0;
    
    double passRate() const {
        return totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0;
    }
};

// =============================================================================
// منسق التقارير (Reporter)
// =============================================================================

class TestReporter {
public:
    virtual ~TestReporter() = default;
    
    virtual void onRunStart(int totalTests) = 0;
    virtual void onRunEnd(const TestStats& stats) = 0;
    virtual void onSuiteStart(const std::string& name) = 0;
    virtual void onSuiteEnd(const TestSuite& suite) = 0;
    virtual void onTestStart(const std::string& name) = 0;
    virtual void onTestEnd(const TestInfo& test) = 0;
};

/// منسق الطرفية (Console)
class ConsoleReporter : public TestReporter {
public:
    void onRunStart(int totalTests) override {
        std::cout << "\n═══════════════════════════════════════════════════\n";
        std::cout << "  🧪 تشغيل " << totalTests << " اختبار(ات)\n";
        std::cout << "═══════════════════════════════════════════════════\n\n";
    }
    
    void onRunEnd(const TestStats& stats) override {
        std::cout << "\n═══════════════════════════════════════════════════\n";
        std::cout << "  📊 النتائج: ";
        
        if (stats.failedTests == 0) {
            std::cout << "✓ جميع الاختبارات نجحت!\n";
        } else {
            std::cout << "✗ فشل " << stats.failedTests << " اختبار\n";
        }
        
        std::cout << "───────────────────────────────────────────────────\n";
        std::cout << "  ✓ نجح:    " << stats.passedTests << "\n";
        std::cout << "  ✗ فشل:    " << stats.failedTests << "\n";
        std::cout << "  ○ تخطى:   " << stats.skippedTests << "\n";
        std::cout << "  ⏱ الوقت:  " << std::fixed << std::setprecision(2) 
                  << stats.totalDurationMs << " ms\n";
        std::cout << "  📈 النسبة: " << std::fixed << std::setprecision(1)
                  << stats.passRate() << "%\n";
        std::cout << "═══════════════════════════════════════════════════\n\n";
    }
    
    void onSuiteStart(const std::string& name) override {
        std::cout << "📁 " << name << "\n";
    }
    
    void onSuiteEnd(const TestSuite& suite) override {
        std::cout << "   └─ " << suite.passedCount << " نجح، " 
                  << suite.failedCount << " فشل\n\n";
    }
    
    void onTestStart(const std::string& name) override {
        // لا شيء
    }
    
    void onTestEnd(const TestInfo& test) override {
        std::cout << "   ";
        
        switch (test.result) {
            case TestResult::PASSED:
                std::cout << "✓ ";
                break;
            case TestResult::FAILED:
                std::cout << "✗ ";
                break;
            case TestResult::SKIPPED:
                std::cout << "○ ";
                break;
            case TestResult::ERROR:
                std::cout << "! ";
                break;
        }
        
        std::cout << test.name << " (" << std::fixed << std::setprecision(2)
                  << test.durationMs << " ms)\n";
        
        // طباعة الأخطاء
        for (const auto& failure : test.failures) {
            std::cout << "      └─ " << failure.message << "\n";
            if (!failure.expected.empty()) {
                std::cout << "         المتوقع: " << failure.expected << "\n";
                std::cout << "         الفعلي:  " << failure.actual << "\n";
            }
        }
    }
};

/// منسق HTML
class HtmlReporter : public TestReporter {
private:
    std::ofstream file_;
    std::string outputPath_;
    std::vector<TestSuite> suites_;
    TestSuite* currentSuite_ = nullptr;
    
public:
    explicit HtmlReporter(const std::string& outputPath) : outputPath_(outputPath) {}
    
    void onRunStart(int totalTests) override {
        suites_.clear();
    }
    
    void onRunEnd(const TestStats& stats) override {
        generateHtml(stats);
    }
    
    void onSuiteStart(const std::string& name) override {
        suites_.push_back(TestSuite{name});
        currentSuite_ = &suites_.back();
    }
    
    void onSuiteEnd(const TestSuite& suite) override {
        if (currentSuite_) {
            currentSuite_->passedCount = suite.passedCount;
            currentSuite_->failedCount = suite.failedCount;
            currentSuite_->totalDurationMs = suite.totalDurationMs;
        }
        currentSuite_ = nullptr;
    }
    
    void onTestStart(const std::string& name) override {}
    
    void onTestEnd(const TestInfo& test) override {
        if (currentSuite_) {
            currentSuite_->tests.push_back(test);
        }
    }
    
private:
    void generateHtml(const TestStats& stats) {
        file_.open(outputPath_);
        if (!file_) return;
        
        file_ << R"(<!DOCTYPE html>
<html dir="rtl" lang="ar">
<head>
    <meta charset="UTF-8">
    <title>تقرير الاختبارات — لغة ص</title>
    <style>
        :root {
            --pass: #22c55e;
            --fail: #ef4444;
            --skip: #eab308;
            --bg: #0f172a;
            --card: #1e293b;
            --text: #f1f5f9;
            --border: #334155;
        }
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body {
            font-family: 'Segoe UI', Tahoma, sans-serif;
            background: var(--bg);
            color: var(--text);
            padding: 2rem;
            line-height: 1.6;
        }
        .header {
            text-align: center;
            padding: 2rem;
            border-bottom: 1px solid var(--border);
            margin-bottom: 2rem;
        }
        .header h1 { font-size: 2rem; margin-bottom: 0.5rem; }
        .stats {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
            gap: 1rem;
            margin-bottom: 2rem;
        }
        .stat {
            background: var(--card);
            padding: 1.5rem;
            border-radius: 0.5rem;
            text-align: center;
        }
        .stat-value { font-size: 2rem; font-weight: bold; }
        .stat-label { color: #94a3b8; font-size: 0.875rem; }
        .passed .stat-value { color: var(--pass); }
        .failed .stat-value { color: var(--fail); }
        .suite {
            background: var(--card);
            border-radius: 0.5rem;
            margin-bottom: 1rem;
            overflow: hidden;
        }
        .suite-header {
            padding: 1rem 1.5rem;
            border-bottom: 1px solid var(--border);
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
        .suite-name { font-weight: 600; }
        .test {
            padding: 0.75rem 1.5rem;
            border-bottom: 1px solid var(--border);
            display: flex;
            align-items: center;
            gap: 1rem;
        }
        .test:last-child { border-bottom: none; }
        .test-icon { font-size: 1.25rem; }
        .test-name { flex: 1; }
        .test-time { color: #64748b; font-size: 0.875rem; }
        .test.pass .test-icon { color: var(--pass); }
        .test.fail .test-icon { color: var(--fail); }
        .test.skip .test-icon { color: var(--skip); }
        .failure {
            background: rgba(239, 68, 68, 0.1);
            padding: 0.75rem 1.5rem;
            margin: 0 1.5rem 1rem;
            border-radius: 0.25rem;
            border-right: 3px solid var(--fail);
        }
        .progress-bar {
            height: 8px;
            background: var(--border);
            border-radius: 4px;
            overflow: hidden;
            margin-top: 1rem;
        }
        .progress-fill {
            height: 100%;
            background: var(--pass);
            transition: width 0.3s;
        }
    </style>
</head>
<body>
    <div class="header">
        <h1>🧪 تقرير الاختبارات</h1>
        <p>)" << getCurrentDateTime() << R"(</p>
    </div>
    
    <div class="stats">
        <div class="stat passed">
            <div class="stat-value">)" << stats.passedTests << R"(</div>
            <div class="stat-label">نجح</div>
        </div>
        <div class="stat failed">
            <div class="stat-value">)" << stats.failedTests << R"(</div>
            <div class="stat-label">فشل</div>
        </div>
        <div class="stat">
            <div class="stat-value">)" << stats.skippedTests << R"(</div>
            <div class="stat-label">تخطى</div>
        </div>
        <div class="stat">
            <div class="stat-value">)" << std::fixed << std::setprecision(0) << stats.totalDurationMs << R"( ms</div>
            <div class="stat-label">الوقت</div>
        </div>
    </div>
    
    <div class="progress-bar">
        <div class="progress-fill" style="width: )" << stats.passRate() << R"(%"></div>
    </div>
    <p style="text-align: center; margin: 0.5rem 0 2rem; color: #64748b;">
        نسبة النجاح: )" << std::fixed << std::setprecision(1) << stats.passRate() << R"(%
    </p>
)";
        
        // Suites
        for (const auto& suite : suites_) {
            file_ << R"(
    <div class="suite">
        <div class="suite-header">
            <span class="suite-name">📁 )" << suite.name << R"(</span>
            <span>)" << suite.passedCount << " نجح، " << suite.failedCount << R"( فشل</span>
        </div>
)";
            
            for (const auto& test : suite.tests) {
                std::string cls = "pass";
                std::string icon = "✓";
                
                if (test.result == TestResult::FAILED) {
                    cls = "fail";
                    icon = "✗";
                } else if (test.result == TestResult::SKIPPED) {
                    cls = "skip";
                    icon = "○";
                }
                
                file_ << R"(
        <div class="test )" << cls << R"(">
            <span class="test-icon">)" << icon << R"(</span>
            <span class="test-name">)" << test.name << R"(</span>
            <span class="test-time">)" << std::fixed << std::setprecision(2) << test.durationMs << R"( ms</span>
        </div>
)";
                
                for (const auto& failure : test.failures) {
                    file_ << R"(
        <div class="failure">
            <strong>)" << failure.message << R"(</strong>
)";
                    if (!failure.expected.empty()) {
                        file_ << R"(            <p>المتوقع: )" << failure.expected << R"(</p>
            <p>الفعلي: )" << failure.actual << R"(</p>
)";
                    }
                    file_ << R"(        </div>
)";
                }
            }
            
            file_ << "    </div>\n";
        }
        
        file_ << R"(
</body>
</html>
)";
        file_.close();
    }
    
    std::string getCurrentDateTime() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// =============================================================================
// إطار الاختبارات الرئيسي
// =============================================================================

class TestRunner {
public:
    // ─────────────────────────────────────────────
    // Singleton
    // ─────────────────────────────────────────────
    
    static TestRunner& instance() {
        static TestRunner runner;
        return runner;
    }
    
    // ─────────────────────────────────────────────
    // إضافة الاختبارات
    // ─────────────────────────────────────────────
    
    void addTest(const std::string& name, std::function<void()> fn,
                 const std::string& suite = "default") {
        auto& s = getSuite(suite);
        s.tests.push_back(TestInfo{name, suite});
        testFunctions_[suite + "::" + name] = fn;
    }
    
    void addSuite(const std::string& name) {
        if (suites_.find(name) == suites_.end()) {
            suites_[name] = TestSuite{name};
        }
    }
    
    void beforeAll(const std::string& suite, std::function<void()> fn) {
        getSuite(suite).beforeAll = fn;
    }
    
    void afterAll(const std::string& suite, std::function<void()> fn) {
        getSuite(suite).afterAll = fn;
    }
    
    void beforeEach(const std::string& suite, std::function<void()> fn) {
        getSuite(suite).beforeEach = fn;
    }
    
    void afterEach(const std::string& suite, std::function<void()> fn) {
        getSuite(suite).afterEach = fn;
    }
    
    // ─────────────────────────────────────────────
    // إضافة المنسقين
    // ─────────────────────────────────────────────
    
    void addReporter(std::shared_ptr<TestReporter> reporter) {
        reporters_.push_back(reporter);
    }
    
    void useConsoleReporter() {
        addReporter(std::make_shared<ConsoleReporter>());
    }
    
    void useHtmlReporter(const std::string& outputPath) {
        addReporter(std::make_shared<HtmlReporter>(outputPath));
    }
    
    // ─────────────────────────────────────────────
    // التشغيل
    // ─────────────────────────────────────────────
    
    TestStats run() {
        // إضافة console reporter افتراضياً
        if (reporters_.empty()) {
            useConsoleReporter();
        }
        
        int totalTests = 0;
        for (auto& [name, suite] : suites_) {
            totalTests += suite.tests.size();
        }
        
        for (auto& reporter : reporters_) {
            reporter->onRunStart(totalTests);
        }
        
        TestStats stats;
        stats.totalTests = totalTests;
        
        auto runStart = std::chrono::high_resolution_clock::now();
        
        // تشغيل كل مجموعة
        for (auto& [suiteName, suite] : suites_) {
            for (auto& reporter : reporters_) {
                reporter->onSuiteStart(suiteName);
            }
            
            // beforeAll
            if (suite.beforeAll) {
                try { suite.beforeAll(); }
                catch (...) { /* تجاهل */ }
            }
            
            auto suiteStart = std::chrono::high_resolution_clock::now();
            
            // تشغيل الاختبارات
            for (auto& test : suite.tests) {
                runTest(test, suite);
                
                // تحديث الإحصائيات
                switch (test.result) {
                    case TestResult::PASSED:
                        suite.passedCount++;
                        stats.passedTests++;
                        break;
                    case TestResult::FAILED:
                    case TestResult::ERROR:
                        suite.failedCount++;
                        stats.failedTests++;
                        break;
                    case TestResult::SKIPPED:
                        suite.skippedCount++;
                        stats.skippedTests++;
                        break;
                }
                
                for (auto& reporter : reporters_) {
                    reporter->onTestEnd(test);
                }
            }
            
            auto suiteEnd = std::chrono::high_resolution_clock::now();
            suite.totalDurationMs = std::chrono::duration<double, std::milli>(
                suiteEnd - suiteStart
            ).count();
            
            // afterAll
            if (suite.afterAll) {
                try { suite.afterAll(); }
                catch (...) { /* تجاهل */ }
            }
            
            for (auto& reporter : reporters_) {
                reporter->onSuiteEnd(suite);
            }
        }
        
        auto runEnd = std::chrono::high_resolution_clock::now();
        stats.totalDurationMs = std::chrono::duration<double, std::milli>(
            runEnd - runStart
        ).count();
        
        for (auto& reporter : reporters_) {
            reporter->onRunEnd(stats);
        }
        
        return stats;
    }
    
    // ─────────────────────────────────────────────
    // الاختبار الحالي
    // ─────────────────────────────────────────────
    
    TestInfo* currentTest() { return currentTest_; }
    
    void addFailure(const FailureInfo& failure) {
        if (currentTest_) {
            currentTest_->failures.push_back(failure);
            currentTest_->result = TestResult::FAILED;
        }
    }
    
    // ─────────────────────────────────────────────
    // تنظيف
    // ─────────────────────────────────────────────
    
    void clear() {
        suites_.clear();
        testFunctions_.clear();
        reporters_.clear();
        currentTest_ = nullptr;
    }

private:
    TestRunner() = default;
    
    std::map<std::string, TestSuite> suites_;
    std::map<std::string, std::function<void()>> testFunctions_;
    std::vector<std::shared_ptr<TestReporter>> reporters_;
    TestInfo* currentTest_ = nullptr;
    
    TestSuite& getSuite(const std::string& name) {
        if (suites_.find(name) == suites_.end()) {
            suites_[name] = TestSuite{name};
        }
        return suites_[name];
    }
    
    void runTest(TestInfo& test, TestSuite& suite) {
        currentTest_ = &test;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            // beforeEach
            if (suite.beforeEach) {
                suite.beforeEach();
            }
            
            // تشغيل الاختبار
            auto key = test.suite + "::" + test.name;
            if (testFunctions_.count(key)) {
                testFunctions_[key]();
            }
            
            // afterEach
            if (suite.afterEach) {
                suite.afterEach();
            }
            
        } catch (const std::exception& e) {
            addFailure(FailureInfo{
                std::string("استثناء: ") + e.what(),
                "", 0, "", ""
            });
        } catch (...) {
            addFailure(FailureInfo{
                "استثناء غير معروف",
                "", 0, "", ""
            });
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        test.durationMs = std::chrono::duration<double, std::milli>(end - start).count();
        
        currentTest_ = nullptr;
    }
};

// =============================================================================
// دوال التأكيد (Assertions)
// =============================================================================

inline void تأكد(bool condition, const std::string& message = "") {
    if (!condition) {
        TestRunner::instance().addFailure(FailureInfo{
            message.empty() ? "فشل التأكيد" : message,
            "", 0, "صحيح", "خطأ"
        });
    }
}

template<typename T>
inline void تأكد_مساواة(const T& expected, const T& actual, const std::string& message = "") {
    if (expected != actual) {
        std::stringstream expStr, actStr;
        expStr << expected;
        actStr << actual;
        
        TestRunner::instance().addFailure(FailureInfo{
            message.empty() ? "القيم غير متساوية" : message,
            "", 0, expStr.str(), actStr.str()
        });
    }
}

template<typename T>
inline void تأكد_عدم_مساواة(const T& a, const T& b, const std::string& message = "") {
    if (a == b) {
        TestRunner::instance().addFailure(FailureInfo{
            message.empty() ? "القيم متساوية (كان يجب أن تكون مختلفة)" : message,
            "", 0, "", ""
        });
    }
}

inline void تأكد_صحيح(bool value) {
    تأكد(value, "كان يجب أن تكون القيمة صحيح");
}

inline void تأكد_خطأ(bool value) {
    تأكد(!value, "كان يجب أن تكون القيمة خطأ");
}

inline void تأكد_لاشيء(const void* ptr) {
    تأكد(ptr == nullptr, "كان يجب أن تكون القيمة لاشيء");
}

inline void تأكد_ليس_لاشيء(const void* ptr) {
    تأكد(ptr != nullptr, "كان يجب ألا تكون القيمة لاشيء");
}

template<typename T>
inline void تأكد_أكبر(const T& a, const T& b) {
    if (!(a > b)) {
        std::stringstream ss;
        ss << a << " ليست أكبر من " << b;
        TestRunner::instance().addFailure(FailureInfo{ss.str(), "", 0, "", ""});
    }
}

template<typename T>
inline void تأكد_أصغر(const T& a, const T& b) {
    if (!(a < b)) {
        std::stringstream ss;
        ss << a << " ليست أصغر من " << b;
        TestRunner::instance().addFailure(FailureInfo{ss.str(), "", 0, "", ""});
    }
}

inline void تأكد_يحتوي(const std::string& haystack, const std::string& needle) {
    if (haystack.find(needle) == std::string::npos) {
        TestRunner::instance().addFailure(FailureInfo{
            "النص لا يحتوي على القيمة المطلوبة",
            "", 0, needle, haystack
        });
    }
}

template<typename Func>
inline void تأكد_استثناء(Func fn, const std::string& message = "") {
    bool threw = false;
    try {
        fn();
    } catch (...) {
        threw = true;
    }
    
    if (!threw) {
        TestRunner::instance().addFailure(FailureInfo{
            message.empty() ? "كان يجب أن يُرمى استثناء" : message,
            "", 0, "", ""
        });
    }
}

inline void فشل(const std::string& message = "") {
    TestRunner::instance().addFailure(FailureInfo{
        message.empty() ? "فشل مُتعمد" : message,
        "", 0, "", ""
    });
}

// =============================================================================
// ماكرو التسجيل
// =============================================================================

#define اختبر(name) \
    static void _test_##__LINE__(); \
    namespace { \
        struct _TestRegistrar_##__LINE__ { \
            _TestRegistrar_##__LINE__() { \
                sad::test::TestRunner::instance().addTest(name, _test_##__LINE__); \
            } \
        } _registrar_##__LINE__; \
    } \
    static void _test_##__LINE__()

#define مجموعة(suite, name) \
    static void _test_##__LINE__(); \
    namespace { \
        struct _TestRegistrar_##__LINE__ { \
            _TestRegistrar_##__LINE__() { \
                sad::test::TestRunner::instance().addTest(name, _test_##__LINE__, suite); \
            } \
        } _registrar_##__LINE__; \
    } \
    static void _test_##__LINE__()

// =============================================================================
// دوال مساعدة
// =============================================================================

/// تشغيل جميع الاختبارات
inline TestStats شغّل_الاختبارات() {
    return TestRunner::instance().run();
}

/// إضافة تقرير HTML
inline void تقرير_html(const std::string& path) {
    TestRunner::instance().useHtmlReporter(path);
}

/// إضافة تقرير console
inline void تقرير_طرفية() {
    TestRunner::instance().useConsoleReporter();
}

} // namespace test
} // namespace sad

#endif // SAD_STDLIB_TEST_H
