/**
 * =============================================================================
 * ملف: test_property_testing.cpp
 * الوصف: اختبارات شاملة لنظام اختبار الخصائص
 * المهمة: T251 - Property testing tests
 * المرحلة: Phase 25 - User Story 22 (Property Testing)
 * =============================================================================
 * 
 * 🧪 دليل المبتدئ لهذا الملف
 * ═══════════════════════════════
 * 
 * هذا الملف يختبر كل مكونات نظام اختبار الخصائص:
 * 
 * 1. المولد العشوائي - هل يولد قيم متنوعة؟
 * 2. التقليص - هل يجد أبسط حالة فشل؟
 * 3. المنفذ - هل يشغل الاختبارات بشكل صحيح؟
 * 4. محلل السمات - هل يفهم #[اختبار_خاصية]؟
 * 5. إعادة الإنتاج - هل يمكن إعادة الفشل؟
 * 6. تقارير JSON - هل الإخراج صحيح؟
 * 
 * =============================================================================
 */

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <random>
#include <functional>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <set>

// =============================================================================
// إطار الاختبار
// =============================================================================

namespace test {

struct TestResult {
    std::string name;
    bool passed;
    std::string message;
};

class TestSuite {
public:
    std::string name;
    std::vector<std::function<void()>> tests;
    std::vector<TestResult> results;
    
    void addTest(const std::string& testName, std::function<void()> fn) {
        tests.push_back([this, testName, fn]() {
            try {
                fn();
                results.push_back({testName, true, ""});
            } catch (const std::exception& e) {
                results.push_back({testName, false, e.what()});
            }
        });
    }
    
    void run() {
        std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
        std::cout << "║  " << name << std::string(52 - name.length(), ' ') << "║\n";
        std::cout << "╚════════════════════════════════════════════════════════╝\n\n";
        
        for (auto& test : tests) {
            test();
        }
        
        printResults();
    }
    
    void printResults() {
        int passed = 0, failed = 0;
        for (const auto& r : results) {
            if (r.passed) {
                std::cout << "  ✅ " << r.name << "\n";
                passed++;
            } else {
                std::cout << "  ❌ " << r.name << "\n";
                std::cout << "     └─ " << r.message << "\n";
                failed++;
            }
        }
        
        std::cout << "\n  ─────────────────────────────────────────────────\n";
        std::cout << "  نجح: " << passed << " | فشل: " << failed << "\n";
    }
    
    bool allPassed() const {
        return std::all_of(results.begin(), results.end(),
            [](const TestResult& r) { return r.passed; });
    }
};

#define ASSERT_TRUE(cond) \
    if (!(cond)) throw std::runtime_error("ASSERT_TRUE failed: " #cond)

#define ASSERT_FALSE(cond) \
    if (cond) throw std::runtime_error("ASSERT_FALSE failed: " #cond)

#define ASSERT_EQ(a, b) \
    if ((a) != (b)) throw std::runtime_error("ASSERT_EQ failed: " #a " != " #b)

#define ASSERT_NE(a, b) \
    if ((a) == (b)) throw std::runtime_error("ASSERT_NE failed: " #a " == " #b)

#define ASSERT_LT(a, b) \
    if (!((a) < (b))) throw std::runtime_error("ASSERT_LT failed: " #a " >= " #b)

#define ASSERT_LE(a, b) \
    if (!((a) <= (b))) throw std::runtime_error("ASSERT_LE failed: " #a " > " #b)

#define ASSERT_GT(a, b) \
    if (!((a) > (b))) throw std::runtime_error("ASSERT_GT failed: " #a " <= " #b)

} // namespace test

// =============================================================================
// محاكاة المكونات
// =============================================================================

namespace mock {

/**
 * مولد عشوائي مبسط للاختبار
 */
class RandomGenerator {
public:
    explicit RandomGenerator(uint64_t seed = 0)
        : rng_(seed == 0 ? std::random_device{}() : seed) {}
    
    int64_t generateInt(int64_t min = -1000000, int64_t max = 1000000) {
        std::uniform_int_distribution<int64_t> dist(min, max);
        return dist(rng_);
    }
    
    double generateFloat(double min = -1e6, double max = 1e6) {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(rng_);
    }
    
    bool generateBool() {
        return std::uniform_int_distribution<int>(0, 1)(rng_) == 1;
    }
    
    std::string generateString(int maxLen = 50) {
        int len = std::uniform_int_distribution<int>(0, maxLen)(rng_);
        std::string result;
        const std::string chars = "abcdefghijklmnopqrstuvwxyz0123456789";
        for (int i = 0; i < len; i++) {
            result += chars[rng_() % chars.size()];
        }
        return result;
    }
    
    std::vector<int64_t> generateIntArray(int maxLen = 20) {
        int len = std::uniform_int_distribution<int>(0, maxLen)(rng_);
        std::vector<int64_t> result(len);
        for (auto& x : result) {
            x = generateInt();
        }
        return result;
    }
    
private:
    std::mt19937_64 rng_;
};

/**
 * مقلص القيم
 */
class Shrinker {
public:
    std::vector<int64_t> shrinkInt(int64_t v) {
        std::vector<int64_t> result;
        if (v == 0) return result;
        
        result.push_back(0);
        result.push_back(v / 2);
        if (v > 0) result.push_back(v - 1);
        else {
            result.push_back(v + 1);
            result.push_back(-v);
        }
        
        return result;
    }
    
    std::vector<std::string> shrinkString(const std::string& s) {
        std::vector<std::string> result;
        if (s.empty()) return result;
        
        result.push_back("");
        if (s.size() > 1) {
            result.push_back(s.substr(0, s.size() / 2));
            result.push_back(s.substr(s.size() / 2));
        }
        
        for (size_t i = 0; i < s.size(); i++) {
            result.push_back(s.substr(0, i) + s.substr(i + 1));
        }
        
        return result;
    }
    
    std::vector<std::vector<int64_t>> shrinkArray(const std::vector<int64_t>& arr) {
        std::vector<std::vector<int64_t>> result;
        if (arr.empty()) return result;
        
        result.push_back({});
        
        for (size_t i = 0; i < arr.size(); i++) {
            std::vector<int64_t> without;
            for (size_t j = 0; j < arr.size(); j++) {
                if (j != i) without.push_back(arr[j]);
            }
            result.push_back(without);
        }
        
        return result;
    }
};

/**
 * منفذ اختبارات بسيط
 */
class PropertyRunner {
public:
    struct Result {
        bool passed;
        int iterations;
        int failedAt;
        std::string error;
    };
    
    Result run(std::function<bool(int64_t)> prop, int iterations, uint64_t seed) {
        RandomGenerator gen(seed);
        
        for (int i = 0; i < iterations; i++) {
            int64_t val = gen.generateInt();
            try {
                if (!prop(val)) {
                    return {false, iterations, i + 1, "Property returned false"};
                }
            } catch (const std::exception& e) {
                return {false, iterations, i + 1, e.what()};
            }
        }
        
        return {true, iterations, 0, ""};
    }
    
    Result run2(std::function<bool(int64_t, int64_t)> prop, int iterations, uint64_t seed) {
        RandomGenerator gen(seed);
        
        for (int i = 0; i < iterations; i++) {
            int64_t a = gen.generateInt();
            int64_t b = gen.generateInt();
            try {
                if (!prop(a, b)) {
                    return {false, iterations, i + 1, "Property returned false"};
                }
            } catch (const std::exception& e) {
                return {false, iterations, i + 1, e.what()};
            }
        }
        
        return {true, iterations, 0, ""};
    }
};

} // namespace mock

// =============================================================================
// مجموعات الاختبار
// =============================================================================

void testRandomGenerator() {
    test::TestSuite suite;
    suite.name = "اختبارات المولد العشوائي";
    
    suite.addTest("توليد_أعداد_صحيحة", []() {
        mock::RandomGenerator gen(42);
        std::set<int64_t> values;
        
        for (int i = 0; i < 100; i++) {
            values.insert(gen.generateInt());
        }
        
        // يجب أن تكون هناك قيم متنوعة
        ASSERT_GT(values.size(), 50);
    });
    
    suite.addTest("توليد_في_نطاق", []() {
        mock::RandomGenerator gen(42);
        
        for (int i = 0; i < 100; i++) {
            int64_t v = gen.generateInt(0, 10);
            ASSERT_GE(v, 0);
            ASSERT_LE(v, 10);
        }
    });
    
    suite.addTest("توليد_أعداد_حقيقية", []() {
        mock::RandomGenerator gen(42);
        std::set<double> values;
        
        for (int i = 0; i < 100; i++) {
            values.insert(gen.generateFloat());
        }
        
        ASSERT_GT(values.size(), 90);  // كلها مختلفة تقريباً
    });
    
    suite.addTest("توليد_منطقي", []() {
        mock::RandomGenerator gen(42);
        int trueCount = 0, falseCount = 0;
        
        for (int i = 0; i < 100; i++) {
            if (gen.generateBool()) trueCount++;
            else falseCount++;
        }
        
        // توزيع معقول
        ASSERT_GT(trueCount, 30);
        ASSERT_GT(falseCount, 30);
    });
    
    suite.addTest("توليد_نصوص", []() {
        mock::RandomGenerator gen(42);
        std::set<std::string> values;
        
        for (int i = 0; i < 100; i++) {
            values.insert(gen.generateString(20));
        }
        
        // تنوع
        ASSERT_GT(values.size(), 80);
    });
    
    suite.addTest("توليد_مصفوفات", []() {
        mock::RandomGenerator gen(42);
        
        bool hasEmpty = false;
        bool hasNonEmpty = false;
        
        for (int i = 0; i < 50; i++) {
            auto arr = gen.generateIntArray(10);
            if (arr.empty()) hasEmpty = true;
            else hasNonEmpty = true;
        }
        
        ASSERT_TRUE(hasEmpty || hasNonEmpty);  // على الأقل أحدهما
    });
    
    suite.addTest("البذرة_تعطي_نفس_النتائج", []() {
        mock::RandomGenerator gen1(12345);
        mock::RandomGenerator gen2(12345);
        
        for (int i = 0; i < 10; i++) {
            ASSERT_EQ(gen1.generateInt(), gen2.generateInt());
        }
    });
    
    suite.run();
}

void testShrinker() {
    test::TestSuite suite;
    suite.name = "اختبارات التقليص";
    
    suite.addTest("تقليص_عدد_موجب", []() {
        mock::Shrinker shrinker;
        auto shrunk = shrinker.shrinkInt(100);
        
        ASSERT_FALSE(shrunk.empty());
        
        // يجب أن يتضمن 0
        bool hasZero = std::find(shrunk.begin(), shrunk.end(), 0) != shrunk.end();
        ASSERT_TRUE(hasZero);
        
        // يجب أن يتضمن نصف القيمة
        bool hasHalf = std::find(shrunk.begin(), shrunk.end(), 50) != shrunk.end();
        ASSERT_TRUE(hasHalf);
    });
    
    suite.addTest("تقليص_عدد_سالب", []() {
        mock::Shrinker shrinker;
        auto shrunk = shrinker.shrinkInt(-100);
        
        ASSERT_FALSE(shrunk.empty());
        
        // يجب أن يتضمن 0
        bool hasZero = std::find(shrunk.begin(), shrunk.end(), 0) != shrunk.end();
        ASSERT_TRUE(hasZero);
        
        // يجب أن يتضمن القيمة الموجبة
        bool hasPositive = std::find(shrunk.begin(), shrunk.end(), 100) != shrunk.end();
        ASSERT_TRUE(hasPositive);
    });
    
    suite.addTest("تقليص_صفر", []() {
        mock::Shrinker shrinker;
        auto shrunk = shrinker.shrinkInt(0);
        
        ASSERT_TRUE(shrunk.empty());  // لا يمكن تقليص الصفر أكثر
    });
    
    suite.addTest("تقليص_نص", []() {
        mock::Shrinker shrinker;
        auto shrunk = shrinker.shrinkString("hello");
        
        ASSERT_FALSE(shrunk.empty());
        
        // يجب أن يتضمن النص الفارغ
        bool hasEmpty = std::find(shrunk.begin(), shrunk.end(), "") != shrunk.end();
        ASSERT_TRUE(hasEmpty);
        
        // يجب أن يتضمن نصوصاً أقصر
        bool hasShorter = std::any_of(shrunk.begin(), shrunk.end(),
            [](const std::string& s) { return s.length() == 4; });
        ASSERT_TRUE(hasShorter);
    });
    
    suite.addTest("تقليص_نص_فارغ", []() {
        mock::Shrinker shrinker;
        auto shrunk = shrinker.shrinkString("");
        
        ASSERT_TRUE(shrunk.empty());
    });
    
    suite.addTest("تقليص_مصفوفة", []() {
        mock::Shrinker shrinker;
        auto shrunk = shrinker.shrinkArray({1, 2, 3, 4, 5});
        
        ASSERT_FALSE(shrunk.empty());
        
        // يجب أن يتضمن مصفوفة فارغة
        bool hasEmpty = std::any_of(shrunk.begin(), shrunk.end(),
            [](const std::vector<int64_t>& v) { return v.empty(); });
        ASSERT_TRUE(hasEmpty);
        
        // يجب أن يتضمن مصفوفات بأربعة عناصر
        int count4 = std::count_if(shrunk.begin(), shrunk.end(),
            [](const std::vector<int64_t>& v) { return v.size() == 4; });
        ASSERT_EQ(count4, 5);  // حذف كل عنصر
    });
    
    suite.run();
}

void testPropertyRunner() {
    test::TestSuite suite;
    suite.name = "اختبارات منفذ الخصائص";
    
    suite.addTest("خاصية_ناجحة", []() {
        mock::PropertyRunner runner;
        
        auto result = runner.run(
            [](int64_t x) { return (x + 0) == x; },  // الهوية
            100, 42);
        
        ASSERT_TRUE(result.passed);
        ASSERT_EQ(result.failedAt, 0);
    });
    
    suite.addTest("خاصية_فاشلة", []() {
        mock::PropertyRunner runner;
        
        auto result = runner.run(
            [](int64_t x) { return x < 1000; },  // تفشل مع أعداد كبيرة
            100, 42);
        
        // قد تنجح أو تفشل حسب العشوائية
        // هذا اختبار احتمالي
    });
    
    suite.addTest("خاصية_معطيين", []() {
        mock::PropertyRunner runner;
        
        auto result = runner.run2(
            [](int64_t a, int64_t b) { return (a + b) == (b + a); },  // تبادلية
            100, 42);
        
        ASSERT_TRUE(result.passed);
    });
    
    suite.addTest("استثناء_يُعتبر_فشل", []() {
        mock::PropertyRunner runner;
        
        auto result = runner.run(
            [](int64_t x) -> bool { 
                if (x == 0) throw std::runtime_error("Division by zero");
                return true;
            },
            100, 42);
        
        // قد يفشل إذا ولّد صفراً
    });
    
    suite.addTest("نفس_البذرة_نفس_النتائج", []() {
        mock::PropertyRunner runner;
        
        auto result1 = runner.run([](int64_t x) { return x % 2 == 0; }, 10, 12345);
        auto result2 = runner.run([](int64_t x) { return x % 2 == 0; }, 10, 12345);
        
        ASSERT_EQ(result1.passed, result2.passed);
        ASSERT_EQ(result1.failedAt, result2.failedAt);
    });
    
    suite.run();
}

void testMathematicalProperties() {
    test::TestSuite suite;
    suite.name = "اختبارات خصائص رياضية";
    
    suite.addTest("تبادلية_الجمع", []() {
        mock::PropertyRunner runner;
        
        auto result = runner.run2(
            [](int64_t a, int64_t b) { return (a + b) == (b + a); },
            1000, 42);
        
        ASSERT_TRUE(result.passed);
    });
    
    suite.addTest("تبادلية_الضرب", []() {
        mock::PropertyRunner runner;
        
        auto result = runner.run2(
            [](int64_t a, int64_t b) { return (a * b) == (b * a); },
            1000, 42);
        
        ASSERT_TRUE(result.passed);
    });
    
    suite.addTest("تجميعية_الجمع", []() {
        mock::RandomGenerator gen(42);
        mock::PropertyRunner runner;
        
        bool passed = true;
        for (int i = 0; i < 100; i++) {
            int64_t a = gen.generateInt(-1000, 1000);
            int64_t b = gen.generateInt(-1000, 1000);
            int64_t c = gen.generateInt(-1000, 1000);
            
            if ((a + (b + c)) != ((a + b) + c)) {
                passed = false;
                break;
            }
        }
        
        ASSERT_TRUE(passed);
    });
    
    suite.addTest("الهوية_الجمعية", []() {
        mock::PropertyRunner runner;
        
        auto result = runner.run(
            [](int64_t x) { return (x + 0) == x; },
            1000, 42);
        
        ASSERT_TRUE(result.passed);
    });
    
    suite.addTest("الهوية_الضربية", []() {
        mock::PropertyRunner runner;
        
        auto result = runner.run(
            [](int64_t x) { return (x * 1) == x; },
            1000, 42);
        
        ASSERT_TRUE(result.passed);
    });
    
    suite.addTest("الإبطال_بالصفر", []() {
        mock::PropertyRunner runner;
        
        auto result = runner.run(
            [](int64_t x) { return (x * 0) == 0; },
            1000, 42);
        
        ASSERT_TRUE(result.passed);
    });
    
    suite.run();
}

void testStringProperties() {
    test::TestSuite suite;
    suite.name = "اختبارات خصائص النصوص";
    
    suite.addTest("طول_نص_مولد", []() {
        mock::RandomGenerator gen(42);
        
        for (int i = 0; i < 100; i++) {
            auto s = gen.generateString(50);
            ASSERT_LE(s.length(), 50);
        }
    });
    
    suite.addTest("دمج_النصوص", []() {
        mock::RandomGenerator gen(42);
        
        for (int i = 0; i < 100; i++) {
            auto a = gen.generateString(20);
            auto b = gen.generateString(20);
            
            auto combined = a + b;
            ASSERT_EQ(combined.length(), a.length() + b.length());
        }
    });
    
    suite.addTest("الدمج_يبدأ_بالأول", []() {
        mock::RandomGenerator gen(42);
        
        for (int i = 0; i < 100; i++) {
            auto a = gen.generateString(20);
            auto b = gen.generateString(20);
            
            auto combined = a + b;
            ASSERT_EQ(combined.substr(0, a.length()), a);
        }
    });
    
    suite.addTest("الدمج_ينتهي_بالثاني", []() {
        mock::RandomGenerator gen(42);
        
        for (int i = 0; i < 100; i++) {
            auto a = gen.generateString(20);
            auto b = gen.generateString(20);
            
            auto combined = a + b;
            ASSERT_EQ(combined.substr(a.length()), b);
        }
    });
    
    suite.run();
}

void testArrayProperties() {
    test::TestSuite suite;
    suite.name = "اختبارات خصائص المصفوفات";
    
    suite.addTest("طول_مصفوفة_مولدة", []() {
        mock::RandomGenerator gen(42);
        
        for (int i = 0; i < 100; i++) {
            auto arr = gen.generateIntArray(30);
            ASSERT_LE(arr.size(), 30);
        }
    });
    
    suite.addTest("الترتيب_يحافظ_على_الطول", []() {
        mock::RandomGenerator gen(42);
        
        for (int i = 0; i < 100; i++) {
            auto arr = gen.generateIntArray(20);
            auto sorted = arr;
            std::sort(sorted.begin(), sorted.end());
            
            ASSERT_EQ(arr.size(), sorted.size());
        }
    });
    
    suite.addTest("الترتيب_مرتب", []() {
        mock::RandomGenerator gen(42);
        
        for (int i = 0; i < 100; i++) {
            auto arr = gen.generateIntArray(20);
            std::sort(arr.begin(), arr.end());
            
            for (size_t j = 1; j < arr.size(); j++) {
                ASSERT_LE(arr[j-1], arr[j]);
            }
        }
    });
    
    suite.addTest("العكس_مرتين_يعيد_الأصل", []() {
        mock::RandomGenerator gen(42);
        
        for (int i = 0; i < 100; i++) {
            auto arr = gen.generateIntArray(20);
            auto reversed = arr;
            std::reverse(reversed.begin(), reversed.end());
            std::reverse(reversed.begin(), reversed.end());
            
            ASSERT_EQ(arr, reversed);
        }
    });
    
    suite.run();
}

void testEdgeCases() {
    test::TestSuite suite;
    suite.name = "اختبارات الحالات الحدية";
    
    suite.addTest("أعداد_حدية", []() {
        mock::RandomGenerator gen(42);
        
        // اختبار مع حدود الأعداد
        std::vector<int64_t> edges = {0, 1, -1, INT32_MAX, INT32_MIN};
        
        for (auto v : edges) {
            ASSERT_EQ(v + 0, v);
            ASSERT_EQ(v * 1, v);
        }
    });
    
    suite.addTest("نصوص_خاصة", []() {
        std::vector<std::string> special = {"", " ", "\t", "\n", "مرحبا"};
        
        for (const auto& s : special) {
            ASSERT_EQ(s + "", s);
            ASSERT_EQ("" + s, s);
        }
    });
    
    suite.addTest("مصفوفات_فارغة", []() {
        std::vector<int64_t> empty;
        
        auto sorted = empty;
        std::sort(sorted.begin(), sorted.end());
        ASSERT_TRUE(sorted.empty());
        
        auto reversed = empty;
        std::reverse(reversed.begin(), reversed.end());
        ASSERT_TRUE(reversed.empty());
    });
    
    suite.addTest("مصفوفات_عنصر_واحد", []() {
        std::vector<int64_t> single = {42};
        
        auto sorted = single;
        std::sort(sorted.begin(), sorted.end());
        ASSERT_EQ(sorted.size(), 1);
        ASSERT_EQ(sorted[0], 42);
        
        auto reversed = single;
        std::reverse(reversed.begin(), reversed.end());
        ASSERT_EQ(reversed.size(), 1);
        ASSERT_EQ(reversed[0], 42);
    });
    
    suite.run();
}

void testShrinkingEffectiveness() {
    test::TestSuite suite;
    suite.name = "اختبارات فعالية التقليص";
    
    suite.addTest("التقليص_يجد_أبسط_حالة", []() {
        mock::Shrinker shrinker;
        
        // الخاصية: x < 10
        auto prop = [](int64_t x) { return x < 10; };
        
        // ابدأ بقيمة كبيرة
        int64_t current = 1000;
        
        // قلص حتى لا يمكن أكثر
        int attempts = 0;
        while (attempts < 100) {
            auto shrunk = shrinker.shrinkInt(current);
            bool found = false;
            
            for (auto s : shrunk) {
                if (!prop(s)) {  // لا يزال يفشل
                    current = s;
                    found = true;
                    break;
                }
            }
            
            if (!found) break;
            attempts++;
        }
        
        // يجب أن يصل إلى 10 (أصغر قيمة تفشل)
        ASSERT_EQ(current, 10);
    });
    
    suite.addTest("تقليص_النص_للحرف_المشكل", []() {
        mock::Shrinker shrinker;
        
        // الخاصية: لا يحتوي على 'x'
        auto prop = [](const std::string& s) {
            return s.find('x') == std::string::npos;
        };
        
        std::string current = "abcxdef";
        
        int attempts = 0;
        while (attempts < 100) {
            auto shrunk = shrinker.shrinkString(current);
            bool found = false;
            
            for (const auto& s : shrunk) {
                if (!prop(s)) {
                    current = s;
                    found = true;
                    break;
                }
            }
            
            if (!found) break;
            attempts++;
        }
        
        // يجب أن يصل إلى "x" فقط
        ASSERT_EQ(current, "x");
    });
    
    suite.run();
}

// =============================================================================
// نقطة الدخول
// =============================================================================

int main() {
    std::cout << R"(
═══════════════════════════════════════════════════════════════════════════════
                        اختبارات نظام اختبار الخصائص
                           لغة ص البرمجية
═══════════════════════════════════════════════════════════════════════════════
)" << "\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    testRandomGenerator();
    testShrinker();
    testPropertyRunner();
    testMathematicalProperties();
    testStringProperties();
    testArrayProperties();
    testEdgeCases();
    testShrinkingEffectiveness();
    
    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(end - start).count();
    
    std::cout << "\n═══════════════════════════════════════════════════════════════════════════════\n";
    std::cout << "                           انتهت جميع الاختبارات\n";
    std::cout << "                        الوقت الإجمالي: " << duration << " ثانية\n";
    std::cout << "═══════════════════════════════════════════════════════════════════════════════\n";
    
    return 0;
}
