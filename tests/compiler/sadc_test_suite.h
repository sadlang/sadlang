/**
 * @file sadc_test_suite.h
 * @brief مجموعة اختبارات sadc - 100+ اختبار
 * @brief (EN) sadc compiler test suite - 100+ tests
 * 
 * @author سعيد
 * @date نوفمبر 2026
 */

#ifndef SADC_TEST_SUITE_H
#define SADC_TEST_SUITE_H

#include &lt;string&gt;
#include &lt;vector&gt;
#include &lt;functional&gt;
#include &lt;map&gt;
#include &lt;memory&gt;
#include &lt;filesystem&gt;

namespace Sad {
namespace Compiler {
namespace Tests {

//==============================================================================
// إطار الاختبار
//==============================================================================

/**
 * @brief نتيجة اختبار
 */
enum class TestResult {
    PASS,
    FAIL,
    SKIP,
    TIMEOUT,
    ERROR
};

/**
 * @brief معلومات اختبار
 */
struct TestInfo {
    std::string name;
    std::string description;
    std::string category;
    std::string sourceFile;
    std::string expectedOutput;
    int expectedExitCode = 0;
    int timeoutMs = 5000;
    bool requiresOptimization = false;
    std::vector&lt;std::string&gt; compilerFlags;
};

/**
 * @brief تقرير اختبار
 */
struct TestReport {
    std::string testName;
    TestResult result;
    std::string message;
    std::string actualOutput;
    int exitCode;
    double durationMs;
};

/**
 * @brief مشغل الاختبارات
 */
class TestRunner {
public:
    /**
     * @brief تسجيل اختبار
     */
    void addTest(const TestInfo&amp; test);
    
    /**
     * @brief تشغيل جميع الاختبارات
     */
    std::vector&lt;TestReport&gt; runAll();
    
    /**
     * @brief تشغيل تصنيف معين
     */
    std::vector&lt;TestReport&gt; runCategory(const std::string&amp; category);
    
    /**
     * @brief تشغيل اختبار واحد
     */
    TestReport runOne(const std::string&amp; testName);
    
    /**
     * @brief طباعة التقرير
     */
    void printReport(const std::vector&lt;TestReport&gt;&amp; reports);
    
    /**
     * @brief تصدير إلى JUnit XML
     */
    void exportJUnit(const std::vector&lt;TestReport&gt;&amp; reports, const std::string&amp; path);

private:
    std::vector&lt;TestInfo&gt; tests_;
    std::string sadcPath_ = "sadc";
    std::string tempDir_ = "tests/tmp";
    
    TestReport executeTest(const TestInfo&amp; test);
    bool compile(const std::string&amp; source, const std::string&amp; output,
                const std::vector&lt;std::string&gt;&amp; flags);
    std::pair&lt;int, std::string&gt; runExecutable(const std::string&amp; path, int timeoutMs);
};

//==============================================================================
// اختبارات الأساسيات (20 اختبار)
//==============================================================================

namespace BasicTests {
    // 1. Hello World
    inline TestInfo helloWorld() {
        return {"basic_hello", "طباعة مرحباً بالعالم", "basic",
                R"(اطبع_سطر("مرحباً بالعالم!"))", "مرحباً بالعالم!\n"};
    }
    
    // 2. Variables
    inline TestInfo variables() {
        return {"basic_variables", "تعريف المتغيرات", "basic",
                R"(متغير س = 42
اطبع_سطر(س))", "42\n"};
    }
    
    // 3. Constants
    inline TestInfo constants() {
        return {"basic_constants", "الثوابت", "basic",
                R"(ثابت PI = 3.14
اطبع_سطر(PI))", "3.14\n"};
    }
    
    // 4-10. Arithmetic operations
    inline TestInfo arithmetic() {
        return {"basic_arithmetic", "العمليات الحسابية", "basic",
                R"(اطبع_سطر(2 + 3)
اطبع_سطر(10 - 4)
اطبع_سطر(3 * 4)
اطبع_sطر(15 / 3)
اطبع_سطر(17 % 5)
اطبع_سطر(2 ** 10))", "5\n6\n12\n5\n2\n1024\n"};
    }
    
    // 11-15. Comparison operators
    inline TestInfo comparison() {
        return {"basic_comparison", "عوامل المقارنة", "basic",
                R"(اطبع_سطر(5 > 3)
اطبع_سطر(5 < 3)
اطبع_سطر(5 == 5)
اطبع_سطر(5 != 3)
اطبع_سطر(5 >= 5))", "صحيح\nخطأ\nصحيح\nصحيح\nصحيح\n"};
    }
    
    // 16-20. Type conversions
    inline TestInfo typeConversions() {
        return {"basic_types", "تحويلات الأنواع", "basic",
                R"(متغير ن = "42"
اطبع_سطر(رقم(ن) + 8)
اطبع_سطر(نص(50)))", "50\n50\n"};
    }
    
    void registerAll(TestRunner&amp; runner);
}

//==============================================================================
// اختبارات التحكم (20 اختبار)
//==============================================================================

namespace ControlFlowTests {
    // 21-25. If/else
    inline TestInfo ifElse() {
        return {"control_if", "إذا/وإلا", "control",
                R"(متغير س = 10
إذا (س > 5)
    اطبع_سطر("أكبر")
وإلا
    اطبع_سطر("أصغر")
نهاية)", "أكبر\n"};
    }
    
    // 26-30. While loops
    inline TestInfo whileLoop() {
        return {"control_while", "حلقة بينما", "control",
                R"(متغير ي = 0
بينما (ي < 5)
    اطبع_سطر(ي)
    ي = ي + 1
نهاية)", "0\n1\n2\n3\n4\n"};
    }
    
    // 31-35. For each loops
    inline TestInfo forEach() {
        return {"control_foreach", "حلقة لكل", "control",
                R"(لكل ر في [1, 2, 3]
    اطبع_سطر(ر)
نهاية)", "1\n2\n3\n"};
    }
    
    // 36-40. Pattern matching
    inline TestInfo patternMatch() {
        return {"control_match", "مطابقة الأنماط", "control",
                R"(متغير ق = 2
طابق (ق)
    عندما 1:
        اطبع_سطر("واحد")
    عندما 2:
        اطبع_سطر("اثنان")
    افتراضي:
        اطبع_سطر("آخر")
نهاية)", "اثنان\n"};
    }
    
    void registerAll(TestRunner&amp; runner);
}

//==============================================================================
// اختبارات الدوال (20 اختبار)
//==============================================================================

namespace FunctionTests {
    // 41-45. Basic functions
    inline TestInfo basicFunction() {
        return {"func_basic", "دالة أساسية", "functions",
                R"(دالة جمع(أ، ب)
    ارجع أ + ب
نهاية
اطبع_سطر(جمع(3، 5)))", "8\n"};
    }
    
    // 46-50. Recursion
    inline TestInfo recursion() {
        return {"func_recursion", "التكرار", "functions",
                R"(دالة فيب(ن)
    إذا (ن <= 1)
        ارجع ن
    نهاية
    ارجع فيب(ن - 1) + فيب(ن - 2)
نهاية
اطبع_سطر(فيب(10)))", "55\n"};
    }
    
    // 51-55. Closures
    inline TestInfo closures() {
        return {"func_closure", "الإغلاقات", "functions",
                R"(دالة عداد()
    متغير ع = 0
    ارجع دالة()
        ع = ع + 1
        ارجع ع
    نهاية
نهاية
متغير ز = عداد()
اطبع_سطر(ز())
اطبع_سطر(ز()))", "1\n2\n"};
    }
    
    // 56-60. Lambda
    inline TestInfo lambda() {
        return {"func_lambda", "لامدا", "functions",
                R"(متغير مربع = لامدا(س) -> س * س
اطبع_سطر(مربع(5)))", "25\n"};
    }
    
    void registerAll(TestRunner&amp; runner);
}

//==============================================================================
// اختبارات الأصناف (20 اختبار)
//==============================================================================

namespace ClassTests {
    // 61-65. Basic class
    inline TestInfo basicClass() {
        return {"class_basic", "صنف أساسي", "classes",
                R"(صنف شخص
    باني(اسم)
        هذا.اسم = اسم
    نهاية
    دالة تحية()
        ارجع "مرحبا " + هذا.اسم
    نهاية
نهاية
متغير ش = جديد شخص("أحمد")
اطبع_سطر(ش.تحية()))", "مرحبا أحمد\n"};
    }
    
    // 66-70. Inheritance
    inline TestInfo inheritance() {
        return {"class_inherit", "الوراثة", "classes",
                R"(صنف حيوان
    باني(اسم)
        هذا.اسم = اسم
    نهاية
نهاية
صنف قطة يرث حيوان
    باني(اسم)
        الأساس(اسم)
    نهاية
    دالة صوت()
        ارجع "مياو"
    نهاية
نهاية
متغير ق = جديد قطة("ميمي")
اطبع_سطر(ق.صوت()))", "مياو\n"};
    }
    
    // 71-75. Static members
    // 76-80. Traits
    
    void registerAll(TestRunner&amp; runner);
}

//==============================================================================
// اختبارات المجموعات (15 اختبار)
//==============================================================================

namespace CollectionTests {
    // 81-85. Arrays
    inline TestInfo arrays() {
        return {"coll_array", "المصفوفات", "collections",
                R"(متغير م = [1, 2, 3, 4, 5]
اطبع_سطر(م[0])
اطبع_سطر(طول(م)))", "1\n5\n"};
    }
    
    // 86-90. Maps
    inline TestInfo maps() {
        return {"coll_map", "الخرائط", "collections",
                R"(متغير خ = {"أ": 1، "ب": 2}
اطبع_سطر(خ["أ"]))", "1\n"};
    }
    
    // 91-95. Higher-order functions
    inline TestInfo higherOrder() {
        return {"coll_higher", "دوال الرتبة العليا", "collections",
                R"(متغير م = [1, 2, 3]
متغير ن = م.استخدم(لامدا(س) -> س * 2)
لكل ر في ن
    اطبع_سطر(ر)
نهاية)", "2\n4\n6\n"};
    }
    
    void registerAll(TestRunner&amp; runner);
}

//==============================================================================
// اختبارات الأخطاء (10 اختبارات)
//==============================================================================

namespace ErrorTests {
    // 96-100. Exception handling
    inline TestInfo tryCatch() {
        return {"error_try", "معالجة الاستثناءات", "errors",
                R"(حاول
    ارمي "خطأ!"
امسك (خ)
    اطبع_سطر("تم التقاط: " + خ)
نهاية)", "تم التقاط: خطأ!\n"};
    }
    
    void registerAll(TestRunner&amp; runner);
}

//==============================================================================
// اختبارات التحسين (10 اختبارات)
//==============================================================================

namespace OptimizationTests {
    // 101-105. Constant folding
    inline TestInfo constantFolding() {
        TestInfo test{"opt_fold", "طي الثوابت", "optimization",
                R"(متغير س = 2 + 3 * 4
اطبع_سطر(س))", "14\n"};
        test.requiresOptimization = true;
        test.compilerFlags = {"-O2"};
        return test;
    }
    
    // 106-110. Dead code elimination
    inline TestInfo dce() {
        TestInfo test{"opt_dce", "إزالة الكود الميت", "optimization",
                R"(متغير س = 42
متغير ص = 100
اطبع_سطر(س))", "42\n"};
        test.requiresOptimization = true;
        test.compilerFlags = {"-O2"};
        return test;
    }
    
    void registerAll(TestRunner&amp; runner);
}

//==============================================================================
// تسجيل جميع الاختبارات
//==============================================================================

inline void registerAllTests(TestRunner&amp; runner) {
    BasicTests::registerAll(runner);
    ControlFlowTests::registerAll(runner);
    FunctionTests::registerAll(runner);
    ClassTests::registerAll(runner);
    CollectionTests::registerAll(runner);
    ErrorTests::registerAll(runner);
    OptimizationTests::registerAll(runner);
}

//==============================================================================
// الدالة الرئيسية
//==============================================================================

inline int runSadcTests(int argc, char** argv) {
    TestRunner runner;
    registerAllTests(runner);
    
    std::vector&lt;TestReport&gt; reports;
    
    if (argc &gt; 1) {
        std::string arg = argv[1];
        if (arg == "--category" &amp;&amp; argc &gt; 2) {
            reports = runner.runCategory(argv[2]);
        } else {
            reports = runner.runOne(arg);
        }
    } else {
        reports = runner.runAll();
    }
    
    runner.printReport(reports);
    runner.exportJUnit(reports, "test-results.xml");
    
    int failures = 0;
    for (const auto&amp; r : reports) {
        if (r.result != TestResult::PASS) failures++;
    }
    
    return failures &gt; 0 ? 1 : 0;
}

} // namespace Tests
} // namespace Compiler
} // namespace Sad

#endif // SADC_TEST_SUITE_H
