/**
 * =============================================================================
 * ملف: property_runner.cpp
 * الوصف: منفذ اختبارات الخصائص
 * المهمة: T248 - Test runner for property tests
 * المرحلة: Phase 25 - User Story 22 (Property Testing)
 * =============================================================================
 * 
 * 🏃 دليل المبتدئ لمنفذ اختبار الخصائص
 * ════════════════════════════════════════
 * 
 * ما هو منفذ الاختبارات؟
 * ───────────────────────
 * البرنامج الذي يشغل اختبارات الخصائص ويجمع النتائج.
 * 
 * ماذا يفعل؟
 * ──────────
 * 1. يجد جميع دوال #[اختبار_خاصية]
 * 2. لكل دالة، يولد قيم عشوائية
 * 3. يستدعي الدالة مع كل قيمة
 * 4. إذا فشلت، يقلص القيمة
 * 5. يطبع تقرير النتائج
 * 
 * دورة العمل:
 * ────────────
 * ```
 * 1. اكتشاف الاختبارات
 *    ↓
 * 2. لكل اختبار:
 *    │
 *    ├→ توليد قيم (N تكرار)
 *    │   ↓
 *    ├→ تنفيذ الاختبار
 *    │   ↓
 *    ├→ إذا فشل → تقليص
 *    │   ↓
 *    └→ تسجيل النتيجة
 *    ↓
 * 3. طباعة التقرير
 * ```
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <random>
#include <iostream>
#include <sstream>
#include <optional>

namespace sad::testing {

// =============================================================================
// أنواع القيم
// =============================================================================

/**
 * قيمة ديناميكية للاختبار
 */
struct DynamicValue {
    enum Type {
        Integer,
        Float,
        Boolean,
        String,
        Array,
        Tuple,
        Custom
    } type;
    
    // القيم
    int64_t intVal = 0;
    double floatVal = 0.0;
    bool boolVal = false;
    std::string strVal;
    std::vector<DynamicValue> arrayVal;
    
    // تحويل لنص
    std::string toString() const {
        switch (type) {
            case Integer: return std::to_string(intVal);
            case Float: return std::to_string(floatVal);
            case Boolean: return boolVal ? "صحيح" : "خطأ";
            case String: return "\"" + strVal + "\"";
            case Array: {
                std::ostringstream ss;
                ss << "[";
                for (size_t i = 0; i < arrayVal.size(); i++) {
                    if (i > 0) ss << ", ";
                    ss << arrayVal[i].toString();
                }
                ss << "]";
                return ss.str();
            }
            default: return "?";
        }
    }
    
    // Factory methods
    static DynamicValue Int(int64_t v) {
        DynamicValue val;
        val.type = Integer;
        val.intVal = v;
        return val;
    }
    
    static DynamicValue Float(double v) {
        DynamicValue val;
        val.type = Float;
        val.floatVal = v;
        return val;
    }
    
    static DynamicValue Bool(bool v) {
        DynamicValue val;
        val.type = Boolean;
        val.boolVal = v;
        return val;
    }
    
    static DynamicValue Str(const std::string& v) {
        DynamicValue val;
        val.type = String;
        val.strVal = v;
        return val;
    }
};

// =============================================================================
// مولد القيم العشوائية
// =============================================================================

class ValueGenerator {
public:
    explicit ValueGenerator(uint64_t seed = 0)
        : rng_(seed == 0 ? std::random_device{}() : seed),
          seed_(seed == 0 ? rng_() : seed) {}
    
    uint64_t getSeed() const { return seed_; }
    
    DynamicValue generateInteger() {
        std::uniform_int_distribution<int64_t> dist(-1000000, 1000000);
        
        // 10% حالات حدية
        if (std::uniform_int_distribution<int>(0, 9)(rng_) == 0) {
            std::vector<int64_t> edges = {0, 1, -1, INT32_MAX, INT32_MIN};
            return DynamicValue::Int(edges[rng_() % edges.size()]);
        }
        
        return DynamicValue::Int(dist(rng_));
    }
    
    DynamicValue generateFloat() {
        std::uniform_real_distribution<double> dist(-1000000.0, 1000000.0);
        return DynamicValue::Float(dist(rng_));
    }
    
    DynamicValue generateBoolean() {
        return DynamicValue::Bool(std::uniform_int_distribution<int>(0, 1)(rng_) == 1);
    }
    
    DynamicValue generateString(int maxLen = 50) {
        int len = std::uniform_int_distribution<int>(0, maxLen)(rng_);
        
        // 10% حالات خاصة
        if (std::uniform_int_distribution<int>(0, 9)(rng_) == 0) {
            std::vector<std::string> special = {"", " ", "\n", "null", "مرحبا"};
            return DynamicValue::Str(special[rng_() % special.size()]);
        }
        
        std::string result;
        const std::string chars = "abcdefghijklmnopqrstuvwxyz0123456789";
        for (int i = 0; i < len; i++) {
            result += chars[rng_() % chars.size()];
        }
        return DynamicValue::Str(result);
    }
    
    DynamicValue generateArray(DynamicValue::Type elemType, int maxLen = 20) {
        DynamicValue val;
        val.type = DynamicValue::Array;
        
        int len = std::uniform_int_distribution<int>(0, maxLen)(rng_);
        for (int i = 0; i < len; i++) {
            switch (elemType) {
                case DynamicValue::Integer:
                    val.arrayVal.push_back(generateInteger());
                    break;
                case DynamicValue::Float:
                    val.arrayVal.push_back(generateFloat());
                    break;
                case DynamicValue::Boolean:
                    val.arrayVal.push_back(generateBoolean());
                    break;
                case DynamicValue::String:
                    val.arrayVal.push_back(generateString());
                    break;
                default:
                    break;
            }
        }
        
        return val;
    }
    
private:
    std::mt19937_64 rng_;
    uint64_t seed_;
};

// =============================================================================
// نتائج الاختبار
// =============================================================================

struct PropertyTestResult {
    std::string name;
    bool passed;
    int iterations;
    int failedAt;  // أي تكرار فشل فيه
    std::vector<DynamicValue> failingArgs;
    std::vector<DynamicValue> shrunkArgs;  // بعد التقليص
    std::string errorMessage;
    double duration;  // ثوانٍ
    uint64_t seed;
};

struct PropertyTestReport {
    int total = 0;
    int passed = 0;
    int failed = 0;
    double totalDuration = 0;
    std::vector<PropertyTestResult> results;
    
    void print() const {
        std::cout << "\n";
        std::cout << "═══════════════════════════════════════════════════════════════\n";
        std::cout << "                تقرير اختبار الخصائص\n";
        std::cout << "═══════════════════════════════════════════════════════════════\n\n";
        
        for (const auto& r : results) {
            if (r.passed) {
                std::cout << "  ✅ " << r.name << " (" << r.iterations << " تكرار، "
                          << r.duration << " ث)\n";
            } else {
                std::cout << "  ❌ " << r.name << "\n";
                std::cout << "      فشل في التكرار " << r.failedAt << "\n";
                std::cout << "      القيم الأصلية: ";
                for (size_t i = 0; i < r.failingArgs.size(); i++) {
                    if (i > 0) std::cout << ", ";
                    std::cout << r.failingArgs[i].toString();
                }
                std::cout << "\n";
                
                if (!r.shrunkArgs.empty()) {
                    std::cout << "      بعد التقليص: ";
                    for (size_t i = 0; i < r.shrunkArgs.size(); i++) {
                        if (i > 0) std::cout << ", ";
                        std::cout << r.shrunkArgs[i].toString();
                    }
                    std::cout << "\n";
                }
                
                if (!r.errorMessage.empty()) {
                    std::cout << "      الخطأ: " << r.errorMessage << "\n";
                }
                
                std::cout << "      البذرة: " << r.seed << " (للإعادة)\n";
            }
        }
        
        std::cout << "\n───────────────────────────────────────────────────────────────\n";
        std::cout << "  المجموع: " << total << " اختبار\n";
        std::cout << "  نجح: " << passed << " ✅\n";
        std::cout << "  فشل: " << failed << " ❌\n";
        std::cout << "  الوقت: " << totalDuration << " ثانية\n";
        std::cout << "═══════════════════════════════════════════════════════════════\n";
    }
};

// =============================================================================
// تعريف اختبار الخصائص
// =============================================================================

struct PropertyTestDef {
    std::string name;
    int iterations = 100;
    std::optional<uint64_t> seed;
    int maxSize = 100;
    bool shrinkEnabled = true;
    
    // أنواع المعطيات
    std::vector<DynamicValue::Type> argTypes;
    
    // دالة الاختبار
    std::function<bool(const std::vector<DynamicValue>&)> testFunc;
};

// =============================================================================
// مقلص القيم
// =============================================================================

class ValueShrinker {
public:
    std::vector<DynamicValue> shrink(const DynamicValue& val) {
        std::vector<DynamicValue> result;
        
        switch (val.type) {
            case DynamicValue::Integer:
                result = shrinkInteger(val.intVal);
                break;
            case DynamicValue::Float:
                result = shrinkFloat(val.floatVal);
                break;
            case DynamicValue::String:
                result = shrinkString(val.strVal);
                break;
            case DynamicValue::Array:
                result = shrinkArray(val);
                break;
            default:
                break;
        }
        
        return result;
    }
    
private:
    std::vector<DynamicValue> shrinkInteger(int64_t v) {
        std::vector<DynamicValue> result;
        
        if (v == 0) return result;
        
        result.push_back(DynamicValue::Int(0));
        result.push_back(DynamicValue::Int(v / 2));
        
        if (v > 0) {
            result.push_back(DynamicValue::Int(v - 1));
        } else {
            result.push_back(DynamicValue::Int(v + 1));
            result.push_back(DynamicValue::Int(-v));
        }
        
        return result;
    }
    
    std::vector<DynamicValue> shrinkFloat(double v) {
        std::vector<DynamicValue> result;
        
        if (v == 0.0) return result;
        
        result.push_back(DynamicValue::Float(0.0));
        result.push_back(DynamicValue::Float(std::round(v)));
        result.push_back(DynamicValue::Float(v / 2.0));
        
        return result;
    }
    
    std::vector<DynamicValue> shrinkString(const std::string& s) {
        std::vector<DynamicValue> result;
        
        if (s.empty()) return result;
        
        result.push_back(DynamicValue::Str(""));
        
        if (s.size() > 1) {
            result.push_back(DynamicValue::Str(s.substr(0, s.size() / 2)));
            result.push_back(DynamicValue::Str(s.substr(s.size() / 2)));
        }
        
        for (size_t i = 0; i < s.size(); i++) {
            result.push_back(DynamicValue::Str(s.substr(0, i) + s.substr(i + 1)));
        }
        
        return result;
    }
    
    std::vector<DynamicValue> shrinkArray(const DynamicValue& arr) {
        std::vector<DynamicValue> result;
        
        if (arr.arrayVal.empty()) return result;
        
        // فارغة
        DynamicValue empty;
        empty.type = DynamicValue::Array;
        result.push_back(empty);
        
        // الأنصاف
        if (arr.arrayVal.size() > 1) {
            DynamicValue half1, half2;
            half1.type = DynamicValue::Array;
            half2.type = DynamicValue::Array;
            
            size_t mid = arr.arrayVal.size() / 2;
            half1.arrayVal = std::vector<DynamicValue>(arr.arrayVal.begin(), 
                                                        arr.arrayVal.begin() + mid);
            half2.arrayVal = std::vector<DynamicValue>(arr.arrayVal.begin() + mid,
                                                        arr.arrayVal.end());
            
            result.push_back(half1);
            result.push_back(half2);
        }
        
        // حذف عنصر واحد
        for (size_t i = 0; i < arr.arrayVal.size(); i++) {
            DynamicValue without;
            without.type = DynamicValue::Array;
            for (size_t j = 0; j < arr.arrayVal.size(); j++) {
                if (j != i) {
                    without.arrayVal.push_back(arr.arrayVal[j]);
                }
            }
            result.push_back(without);
        }
        
        return result;
    }
};

// =============================================================================
// منفذ اختبار الخصائص
// =============================================================================

class PropertyTestRunner {
public:
    /**
     * تشغيل اختبار واحد
     */
    PropertyTestResult runTest(const PropertyTestDef& test) {
        PropertyTestResult result;
        result.name = test.name;
        result.iterations = test.iterations;
        
        // إنشاء المولد
        uint64_t seed = test.seed.value_or(std::random_device{}());
        result.seed = seed;
        ValueGenerator gen(seed);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // تنفيذ التكرارات
        for (int i = 0; i < test.iterations; i++) {
            // توليد قيم
            std::vector<DynamicValue> args;
            for (auto type : test.argTypes) {
                switch (type) {
                    case DynamicValue::Integer:
                        args.push_back(gen.generateInteger());
                        break;
                    case DynamicValue::Float:
                        args.push_back(gen.generateFloat());
                        break;
                    case DynamicValue::Boolean:
                        args.push_back(gen.generateBoolean());
                        break;
                    case DynamicValue::String:
                        args.push_back(gen.generateString());
                        break;
                    case DynamicValue::Array:
                        args.push_back(gen.generateArray(DynamicValue::Integer));
                        break;
                    default:
                        break;
                }
            }
            
            // تنفيذ الاختبار
            try {
                bool passed = test.testFunc(args);
                if (!passed) {
                    result.passed = false;
                    result.failedAt = i + 1;
                    result.failingArgs = args;
                    
                    // تقليص
                    if (test.shrinkEnabled) {
                        result.shrunkArgs = shrinkFailure(test.testFunc, args);
                    }
                    
                    break;
                }
            } catch (const std::exception& e) {
                result.passed = false;
                result.failedAt = i + 1;
                result.failingArgs = args;
                result.errorMessage = e.what();
                break;
            }
        }
        
        if (result.failedAt == 0) {
            result.passed = true;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        result.duration = std::chrono::duration<double>(end - start).count();
        
        return result;
    }
    
    /**
     * تشغيل جميع الاختبارات
     */
    PropertyTestReport runAll(const std::vector<PropertyTestDef>& tests) {
        PropertyTestReport report;
        report.total = tests.size();
        
        for (const auto& test : tests) {
            auto result = runTest(test);
            
            if (result.passed) {
                report.passed++;
            } else {
                report.failed++;
            }
            
            report.totalDuration += result.duration;
            report.results.push_back(result);
        }
        
        return report;
    }
    
private:
    /**
     * تقليص حالة الفشل
     */
    std::vector<DynamicValue> shrinkFailure(
        const std::function<bool(const std::vector<DynamicValue>&)>& testFunc,
        const std::vector<DynamicValue>& args,
        int maxAttempts = 100)
    {
        ValueShrinker shrinker;
        auto current = args;
        
        for (int attempt = 0; attempt < maxAttempts; attempt++) {
            bool improved = false;
            
            for (size_t i = 0; i < current.size(); i++) {
                auto shrunk = shrinker.shrink(current[i]);
                
                for (const auto& s : shrunk) {
                    auto candidate = current;
                    candidate[i] = s;
                    
                    try {
                        if (!testFunc(candidate)) {
                            current = candidate;
                            improved = true;
                            break;
                        }
                    } catch (...) {
                        current = candidate;
                        improved = true;
                        break;
                    }
                }
                
                if (improved) break;
            }
            
            if (!improved) break;
        }
        
        return current;
    }
};

} // namespace sad::testing

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadPropertyRunner {
    sad::testing::PropertyTestRunner* runner;
    std::vector<sad::testing::PropertyTestDef> tests;
};

SadPropertyRunner* sad_property_runner_new() {
    auto* ctx = new SadPropertyRunner();
    ctx->runner = new sad::testing::PropertyTestRunner();
    return ctx;
}

void sad_property_runner_run_all(SadPropertyRunner* ctx) {
    auto report = ctx->runner->runAll(ctx->tests);
    report.print();
}

void sad_property_runner_free(SadPropertyRunner* ctx) {
    if (ctx) {
        delete ctx->runner;
        delete ctx;
    }
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef PROPERTY_RUNNER_TEST

void testBasicRunner() {
    std::cout << "🏃 اختبار المنفذ الأساسي...\n";
    
    sad::testing::PropertyTestRunner runner;
    
    sad::testing::PropertyTestDef test;
    test.name = "خاصية_الجمع_التبادلي";
    test.iterations = 50;
    test.argTypes = {sad::testing::DynamicValue::Integer, 
                     sad::testing::DynamicValue::Integer};
    test.testFunc = [](const std::vector<sad::testing::DynamicValue>& args) {
        auto a = args[0].intVal;
        auto b = args[1].intVal;
        return (a + b) == (b + a);
    };
    
    auto result = runner.runTest(test);
    
    assert(result.passed);
    assert(result.iterations == 50);
    
    std::cout << "   ✅ نجح\n";
}

void testShrinking() {
    std::cout << "🔬 اختبار التقليص...\n";
    
    sad::testing::PropertyTestRunner runner;
    
    sad::testing::PropertyTestDef test;
    test.name = "خاصية_فاشلة";
    test.iterations = 100;
    test.argTypes = {sad::testing::DynamicValue::Integer};
    test.testFunc = [](const std::vector<sad::testing::DynamicValue>& args) {
        return args[0].intVal <= 10;  // تفشل مع أي عدد > 10
    };
    
    auto result = runner.runTest(test);
    
    assert(!result.passed);
    assert(!result.shrunkArgs.empty());
    // يجب أن يقلص إلى 11
    
    std::cout << "   القيمة المُقلصة: " << result.shrunkArgs[0].intVal << "\n";
    std::cout << "   ✅ نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات منفذ الخصائص\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testBasicRunner();
    testShrinking();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // PROPERTY_RUNNER_TEST
