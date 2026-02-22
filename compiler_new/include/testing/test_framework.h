// ======================================================================
// test_framework.h - إطار اختبار لغة ص / Sad Language Test Framework
// ======================================================================
// الوصف بالعربية:
//   إطار اختبار مدمج في المترجم يدعم:
//   - اختبارات وحدة (unit tests) بكلمة 'اختبر'
//   - تأكيدات عربية (تأكد، تأكد_يساوي، تأكد_خطأ)
//   - اختبارات BDD (صِف / عندما / يجب)
//   - قياس التغطية (coverage)
//
// English Description:
//   Built-in test framework supporting:
//   - Unit tests with 'test' keyword
//   - Arabic assertions (assert, assert_eq, assert_err)
//   - BDD tests (describe / when / should)
//   - Coverage measurement
// ======================================================================

#ifndef SAD_TEST_FRAMEWORK_H
#define SAD_TEST_FRAMEWORK_H

#include <string>
#include <vector>
#include <functional>
#include <chrono>

namespace Sad {
namespace Testing {

/**
 * @brief Test result / نتيجة الاختبار
 */
enum class TestResult {
    Pass,       ///< ناجح / passed
    Fail,       ///< فاشل / failed
    Skip,       ///< تخطي / skipped
    Error       ///< خطأ / error (unexpected)
};

/**
 * @brief Assertion type / نوع التأكيد
 */
enum class AssertionType {
    Assert,         ///< تأكد / assert — general assertion
    AssertEq,       ///< تأكد_يساوي / assert_eq — equality check
    AssertNe,       ///< تأكد_لا_يساوي / assert_ne — inequality check
    AssertGt,       ///< تأكد_أكبر / assert_gt — greater than
    AssertLt,       ///< تأكد_أصغر / assert_lt — less than
    AssertErr,      ///< تأكد_خطأ / assert_err — expect error
    AssertOk,       ///< تأكد_نجاح / assert_ok — expect success
    AssertNone,     ///< تأكد_لاشيء / assert_none — expect none
    AssertSome,     ///< تأكد_بعض / assert_some — expect some value
    AssertPanic     ///< تأكد_ذعر / assert_panic — expect panic
};

/**
 * @brief Single test case / حالة اختبار واحدة
 */
struct TestCase {
    std::string name;               ///< Test name / اسم الاختبار
    std::string file;               ///< Source file / ملف المصدر
    int line;                       ///< Line number / رقم السطر
    TestResult result;              ///< Result / النتيجة
    std::string failureMessage;     ///< Failure message / رسالة الفشل
    double durationMs;              ///< Duration in ms / المدة بالمللي ثانية
    bool shouldFail;                ///< Expected to fail? / متوقع أن يفشل؟
    bool isIgnored;                 ///< Ignored? / متجاهل؟
    std::vector<std::string> tags;  ///< Tags / وسوم
    
    TestCase(const std::string& n, const std::string& f = "", int l = 0)
        : name(n), file(f), line(l), result(TestResult::Skip),
          durationMs(0), shouldFail(false), isIgnored(false) {}
};

/**
 * @brief Test suite / مجموعة اختبارات
 */
struct TestSuite {
    std::string name;               ///< Suite name / اسم المجموعة
    std::vector<TestCase> tests;    ///< Test cases / حالات الاختبار
    int passed;                     ///< Passed count / عدد الناجح
    int failed;                     ///< Failed count / عدد الفاشل
    int skipped;                    ///< Skipped count / عدد المتخطى
    double totalDurationMs;         ///< Total duration / المدة الإجمالية
    
    TestSuite(const std::string& n = "")
        : name(n), passed(0), failed(0), skipped(0), totalDurationMs(0) {}
    
    /**
     * @brief Add test case / إضافة حالة اختبار
     */
    void addTest(const TestCase& test) {
        tests.push_back(test);
    }
    
    /**
     * @brief Get summary in Arabic / الحصول على ملخص بالعربية
     */
    std::string getArabicSummary() const {
        std::string summary = "نتائج الاختبار: ";
        summary += std::to_string(passed) + " ناجح، ";
        summary += std::to_string(failed) + " فاشل، ";
        summary += std::to_string(skipped) + " متخطى";
        summary += " (المدة: " + std::to_string(totalDurationMs) + " مللي ثانية)";
        return summary;
    }
    
    /**
     * @brief All tests passed? / هل نجحت جميع الاختبارات؟
     */
    bool allPassed() const {
        return failed == 0;
    }
};

/**
 * @brief Coverage info / معلومات التغطية
 */
struct CoverageInfo {
    int totalLines;         ///< Total lines / إجمالي الأسطر
    int coveredLines;       ///< Covered lines / الأسطر المغطاة
    int totalFunctions;     ///< Total functions / إجمالي الدوال
    int coveredFunctions;   ///< Covered functions / الدوال المغطاة
    int totalBranches;      ///< Total branches / إجمالي الفروع
    int coveredBranches;    ///< Covered branches / الفروع المغطاة
    
    CoverageInfo()
        : totalLines(0), coveredLines(0), totalFunctions(0),
          coveredFunctions(0), totalBranches(0), coveredBranches(0) {}
    
    double linePercent() const {
        return totalLines > 0 ? (100.0 * coveredLines / totalLines) : 0;
    }
    
    double functionPercent() const {
        return totalFunctions > 0 ? (100.0 * coveredFunctions / totalFunctions) : 0;
    }
    
    double branchPercent() const {
        return totalBranches > 0 ? (100.0 * coveredBranches / totalBranches) : 0;
    }
    
    /**
     * @brief Get Arabic summary / ملخص بالعربية
     */
    std::string getArabicSummary() const {
        std::string s = "تغطية الكود:\n";
        s += "  الأسطر: " + std::to_string(coveredLines) + "/" + 
             std::to_string(totalLines) + " (" + 
             std::to_string((int)linePercent()) + "%)\n";
        s += "  الدوال: " + std::to_string(coveredFunctions) + "/" + 
             std::to_string(totalFunctions) + " (" + 
             std::to_string((int)functionPercent()) + "%)\n";
        s += "  الفروع: " + std::to_string(coveredBranches) + "/" + 
             std::to_string(totalBranches) + " (" + 
             std::to_string((int)branchPercent()) + "%)";
        return s;
    }
};

/**
 * @brief Test runner / مشغّل الاختبارات
 * 
 * (AR) يجمع ويشغل جميع الاختبارات المعرّفة بكلمة 'اختبر'
 * (EN) Collects and runs all tests defined with 'test' keyword
 */
class TestRunner {
public:
    TestRunner() = default;
    
    /**
     * @brief Register a test / تسجيل اختبار
     */
    void registerTest(const TestCase& test) {
        suite_.addTest(test);
    }
    
    /**
     * @brief Get test suite / الحصول على مجموعة الاختبارات
     */
    const TestSuite& getSuite() const { return suite_; }
    TestSuite& getSuite() { return suite_; }
    
    /**
     * @brief Get test count / عدد الاختبارات
     */
    size_t testCount() const { return suite_.tests.size(); }
    
    /**
     * @brief Print results in Arabic / طباعة النتائج بالعربية
     */
    std::string formatResults() const {
        std::string output;
        output += "═══════════════════════════════════\n";
        output += "     نتائج اختبارات لغة ص\n";
        output += "═══════════════════════════════════\n\n";
        
        for (const auto& test : suite_.tests) {
            switch (test.result) {
                case TestResult::Pass:
                    output += "  ✅ " + test.name + " (ناجح)\n";
                    break;
                case TestResult::Fail:
                    output += "  ❌ " + test.name + " (فاشل)\n";
                    if (!test.failureMessage.empty()) {
                        output += "     السبب: " + test.failureMessage + "\n";
                    }
                    break;
                case TestResult::Skip:
                    output += "  ⏭️ " + test.name + " (متخطى)\n";
                    break;
                case TestResult::Error:
                    output += "  💥 " + test.name + " (خطأ)\n";
                    break;
            }
        }
        
        output += "\n" + suite_.getArabicSummary() + "\n";
        return output;
    }
    
private:
    TestSuite suite_;
};

} // namespace Testing
} // namespace Sad

#endif // SAD_TEST_FRAMEWORK_H
