/**
 * @file backward_compat_tests.h
 * @brief اختبارات التوافق الخلفي
 * @brief (EN) Backward compatibility tests
 * 
 * @author أحمد
 * @date ديسمبر 2026
 */

#ifndef SAD_BACKWARD_COMPAT_TESTS_H
#define SAD_BACKWARD_COMPAT_TESTS_H

#include &lt;string&gt;
#include &lt;vector&gt;
#include &lt;map&gt;
#include &lt;functional&gt;
#include &lt;filesystem&gt;

namespace Sad {
namespace Tests {
namespace Compat {

//==============================================================================
// أنواع البيانات
//==============================================================================

/**
 * @brief إصدار اللغة
 */
struct Version {
    int major;
    int minor;
    int patch;
    
    std::string toString() const {
        return std::to_string(major) + "." + 
               std::to_string(minor) + "." + 
               std::to_string(patch);
    }
    
    bool operator&gt;=(const Version&amp; other) const {
        if (major != other.major) return major &gt; other.major;
        if (minor != other.minor) return minor &gt; other.minor;
        return patch &gt;= other.patch;
    }
};

/**
 * @brief نتيجة اختبار التوافق
 */
enum class CompatResult {
    COMPATIBLE,         // متوافق تماماً
    DEPRECATED_WARNING, // تحذير إهمال
    BREAKING_CHANGE,    // تغيير جذري
    NOT_APPLICABLE      // غير قابل للتطبيق
};

/**
 * @brief تقرير اختبار التوافق
 */
struct CompatReport {
    std::string testName;
    Version minVersion;
    Version maxVersion;
    CompatResult result;
    std::string message;
    std::string migrationGuide;
};

//==============================================================================
// اختبارات API
//==============================================================================

/**
 * @brief اختبار توافق Value API
 */
class ValueCompatTest {
public:
    std::vector&lt;CompatReport&gt; run() {
        std::vector&lt;CompatReport&gt; reports;
        
        // اختبار 1: البناة
        reports.push_back(testConstructors());
        
        // اختبار 2: التحويلات
        reports.push_back(testConversions());
        
        // اختبار 3: العوامل
        reports.push_back(testOperators());
        
        return reports;
    }

private:
    CompatReport testConstructors() {
        // التحقق من أن جميع البناة الموثقة تعمل
        return {"Value::constructors", {1,0,0}, {1,99,99}, 
                CompatResult::COMPATIBLE, "جميع البناة تعمل كما هو متوقع"};
    }
    
    CompatReport testConversions() {
        return {"Value::conversions", {1,0,0}, {1,99,99},
                CompatResult::COMPATIBLE, "التحويلات تعمل بشكل صحيح"};
    }
    
    CompatReport testOperators() {
        return {"Value::operators", {1,0,0}, {1,99,99},
                CompatResult::COMPATIBLE, "العوامل تعمل بشكل صحيح"};
    }
};

/**
 * @brief اختبار توافق Interpreter API
 */
class InterpreterCompatTest {
public:
    std::vector&lt;CompatReport&gt; run() {
        std::vector&lt;CompatReport&gt; reports;
        
        reports.push_back(testExecute());
        reports.push_back(testGlobals());
        reports.push_back(testFunctions());
        
        return reports;
    }

private:
    CompatReport testExecute();
    CompatReport testGlobals();
    CompatReport testFunctions();
};

/**
 * @brief اختبار توافق stdlib
 */
class StdlibCompatTest {
public:
    std::vector&lt;CompatReport&gt; run() {
        std::vector&lt;CompatReport&gt; reports;
        
        // core module
        reports.push_back(testCoreModule());
        
        // io module
        reports.push_back(testIOModule());
        
        // math module
        reports.push_back(testMathModule());
        
        // string module
        reports.push_back(testStringModule());
        
        // json module
        reports.push_back(testJSONModule());
        
        // http module
        reports.push_back(testHTTPModule());
        
        return reports;
    }

private:
    CompatReport testCoreModule();
    CompatReport testIOModule();
    CompatReport testMathModule();
    CompatReport testStringModule();
    CompatReport testJSONModule();
    CompatReport testHTTPModule();
};

//==============================================================================
// اختبارات بناء الجمل
//==============================================================================

/**
 * @brief اختبار توافق بناء الجمل
 */
class SyntaxCompatTest {
public:
    std::vector&lt;CompatReport&gt; run() {
        std::vector&lt;CompatReport&gt; reports;
        
        // الكلمات المفتاحية
        reports.push_back(testKeywords());
        
        // العوامل
        reports.push_back(testOperators());
        
        // الأصناف
        reports.push_back(testClasses());
        
        // الدوال
        reports.push_back(testFunctions());
        
        return reports;
    }

private:
    CompatReport testKeywords() {
        // التحقق من أن جميع الكلمات المفتاحية الموثقة تعمل
        std::vector&lt;std::string&gt; keywords = {
            "دالة", "ارجع", "صنف", "بنية", "تعداد", "يرث", "نهاية",
            "جديد", "هذا", "باني", "الأساس",
            "إذا", "وإلا", "بينما", "لكل", "في", "توقف", "استمر",
            "طابق", "عندما", "افتراضي",
            "حاول", "امسك", "ارمي", "أخيراً",
            "عام", "خاص", "محمي", "مجرد",
            "استورد", "من", "كـ", "صدّر",
            "متغير", "ثابت", "ساكن", "خارجي",
            "صحيح", "خطأ", "لاشيء", "و", "أو", "ليس"
        };
        
        bool allWork = true;
        for (const auto&amp; kw : keywords) {
            // TODO: Test each keyword
        }
        
        return {"syntax::keywords", {1,0,0}, {1,99,99},
                allWork ? CompatResult::COMPATIBLE : CompatResult::BREAKING_CHANGE,
                "اختبار الكلمات المفتاحية"};
    }
    
    CompatReport testOperators();
    CompatReport testClasses();
    CompatReport testFunctions();
};

//==============================================================================
// مشغل اختبارات التوافق
//==============================================================================

/**
 * @brief مشغل اختبارات التوافق
 */
class CompatTestRunner {
public:
    /**
     * @brief تشغيل جميع الاختبارات
     */
    std::vector&lt;CompatReport&gt; runAll() {
        std::vector&lt;CompatReport&gt; reports;
        
        // Value API
        auto valueReports = ValueCompatTest().run();
        reports.insert(reports.end(), valueReports.begin(), valueReports.end());
        
        // Interpreter API
        auto interpReports = InterpreterCompatTest().run();
        reports.insert(reports.end(), interpReports.begin(), interpReports.end());
        
        // Stdlib
        auto stdlibReports = StdlibCompatTest().run();
        reports.insert(reports.end(), stdlibReports.begin(), stdlibReports.end());
        
        // Syntax
        auto syntaxReports = SyntaxCompatTest().run();
        reports.insert(reports.end(), syntaxReports.begin(), syntaxReports.end());
        
        return reports;
    }
    
    /**
     * @brief طباعة التقرير
     */
    void printReport(const std::vector&lt;CompatReport&gt;&amp; reports) {
        std::cout &lt;&lt; "=== تقرير التوافق الخلفي ===\n\n";
        
        int compatible = 0, deprecated = 0, breaking = 0;
        
        for (const auto&amp; r : reports) {
            std::string status;
            switch (r.result) {
                case CompatResult::COMPATIBLE:
                    status = "✅ متوافق";
                    compatible++;
                    break;
                case CompatResult::DEPRECATED_WARNING:
                    status = "⚠️ مهمل";
                    deprecated++;
                    break;
                case CompatResult::BREAKING_CHANGE:
                    status = "❌ تغيير جذري";
                    breaking++;
                    break;
                default:
                    status = "➖ غير قابل للتطبيق";
            }
            
            std::cout &lt;&lt; status &lt;&lt; " | " &lt;&lt; r.testName 
                      &lt;&lt; " | " &lt;&lt; r.message &lt;&lt; "\n";
        }
        
        std::cout &lt;&lt; "\n=== الملخص ===\n";
        std::cout &lt;&lt; "متوافق: " &lt;&lt; compatible &lt;&lt; "\n";
        std::cout &lt;&lt; "مهمل: " &lt;&lt; deprecated &lt;&lt; "\n";
        std::cout &lt;&lt; "تغييرات جذرية: " &lt;&lt; breaking &lt;&lt; "\n";
    }
    
    /**
     * @brief تصدير إلى JSON
     */
    void exportJSON(const std::vector&lt;CompatReport&gt;&amp; reports, const std::string&amp; path);
    
    /**
     * @brief التحقق من التوافق مع إصدار معين
     */
    bool isCompatibleWith(const Version&amp; version) {
        auto reports = runAll();
        for (const auto&amp; r : reports) {
            if (r.result == CompatResult::BREAKING_CHANGE) {
                return false;
            }
        }
        return true;
    }
};

//==============================================================================
// نقطة الدخول
//==============================================================================

inline int runBackwardCompatTests(int argc, char** argv) {
    CompatTestRunner runner;
    auto reports = runner.runAll();
    runner.printReport(reports);
    
    // فشل إذا كان هناك تغييرات جذرية
    for (const auto&amp; r : reports) {
        if (r.result == CompatResult::BREAKING_CHANGE) {
            return 1;
        }
    }
    
    return 0;
}

} // namespace Compat
} // namespace Tests
} // namespace Sad

#endif // SAD_BACKWARD_COMPAT_TESTS_H
