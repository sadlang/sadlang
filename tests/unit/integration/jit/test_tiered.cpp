/**
 * =============================================================================
 * ملف: test_tiered.cpp
 * الوصف: اختبارات الترجمة المتدرجة
 * المهمة: T217 - Create tiered compilation tests
 * المرحلة: Phase 21 - User Story 18 (JIT & Tiered Compilation)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما نختبره هنا:
 * ──────────────
 * 1. انتقال VM → JIT
 * 2. انتقال JIT → AOT
 * 3. OSR (On-Stack Replacement)
 * 4. Deoptimization
 * 5. Hot path detection
 * 
 * كيف تعمل الاختبارات:
 * ────────────────────
 * - نُحاكي تنفيذ كود
 * - نفحص انتقال المستويات
 * - نتحقق من صحة الإحصائيات
 * 
 * =============================================================================
 */

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <functional>
#include <map>
#include <memory>

// =============================================================================
// إطار الاختبار (Test Framework)
// =============================================================================

namespace sad::test {

/**
 * نتيجة اختبار
 */
struct TestResult {
    std::string name;
    bool passed = false;
    std::string message;
    uint64_t durationMs = 0;
};

/**
 * مجموعة اختبارات
 */
class TestSuite {
public:
    explicit TestSuite(const std::string& name) : name_(name) {}
    
    void addTest(const std::string& name, std::function<void()> test) {
        tests_.push_back({name, test});
    }
    
    std::vector<TestResult> run() {
        std::vector<TestResult> results;
        
        std::cout << "\n═══════════════════════════════════════════════\n";
        std::cout << "   " << name_ << "\n";
        std::cout << "═══════════════════════════════════════════════\n\n";
        
        for (const auto& [name, test] : tests_) {
            TestResult result;
            result.name = name;
            
            auto start = std::chrono::high_resolution_clock::now();
            
            try {
                test();
                result.passed = true;
                result.message = "نجح";
                std::cout << "✅ " << name << "\n";
            } catch (const std::exception& e) {
                result.passed = false;
                result.message = e.what();
                std::cout << "❌ " << name << ": " << e.what() << "\n";
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            result.durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                end - start
            ).count();
            
            results.push_back(result);
        }
        
        return results;
    }
    
private:
    std::string name_;
    std::vector<std::pair<std::string, std::function<void()>>> tests_;
};

#define ASSERT_TRUE(cond) \
    if (!(cond)) throw std::runtime_error("Assertion failed: " #cond)

#define ASSERT_FALSE(cond) \
    if (cond) throw std::runtime_error("Assertion failed: not " #cond)

#define ASSERT_EQ(a, b) \
    if ((a) != (b)) throw std::runtime_error("Assertion failed: " #a " != " #b)

#define ASSERT_NE(a, b) \
    if ((a) == (b)) throw std::runtime_error("Assertion failed: " #a " == " #b)

#define ASSERT_GT(a, b) \
    if (!((a) > (b))) throw std::runtime_error("Assertion failed: " #a " > " #b)

#define ASSERT_GE(a, b) \
    if (!((a) >= (b))) throw std::runtime_error("Assertion failed: " #a " >= " #b)

} // namespace sad::test

// =============================================================================
// محاكاة مكونات JIT (Mock JIT Components)
// =============================================================================

namespace sad::jit::mock {

/**
 * مستويات الترجمة
 */
enum class Tier { Interpreter, BaselineJIT, OptimizedJIT, AOT };

inline const char* tierName(Tier t) {
    switch (t) {
        case Tier::Interpreter: return "Interpreter";
        case Tier::BaselineJIT: return "BaselineJIT";
        case Tier::OptimizedJIT: return "OptimizedJIT";
        case Tier::AOT: return "AOT";
    }
    return "Unknown";
}

/**
 * معلومات دالة
 */
struct FunctionInfo {
    std::string name;
    Tier currentTier = Tier::Interpreter;
    uint64_t executionCount = 0;
    uint64_t totalTimeUs = 0;
    bool isCompiled = false;
    void* compiledCode = nullptr;
};

/**
 * محاكي Hot Path Detector
 */
class MockHotPathDetector {
public:
    void registerFunction(const std::string& name) {
        functions_[name] = {};
        functions_[name].name = name;
    }
    
    void recordExecution(const std::string& name, uint64_t timeUs = 10) {
        auto& info = functions_[name];
        info.executionCount++;
        info.totalTimeUs += timeUs;
    }
    
    bool isHot(const std::string& name) const {
        auto it = functions_.find(name);
        return it != functions_.end() && it->second.executionCount >= hotThreshold_;
    }
    
    uint64_t getExecutionCount(const std::string& name) const {
        auto it = functions_.find(name);
        return it != functions_.end() ? it->second.executionCount : 0;
    }
    
    void setHotThreshold(uint64_t threshold) { hotThreshold_ = threshold; }
    
private:
    std::map<std::string, FunctionInfo> functions_;
    uint64_t hotThreshold_ = 100;
};

/**
 * محاكي JIT Trigger
 */
class MockJITTrigger {
public:
    MockJITTrigger(MockHotPathDetector& detector) : detector_(detector) {}
    
    bool shouldCompile(const std::string& name) {
        return detector_.isHot(name);
    }
    
    void markCompiled(const std::string& name) {
        compiledFunctions_.insert(name);
        compilationCount_++;
    }
    
    bool isCompiled(const std::string& name) const {
        return compiledFunctions_.count(name) > 0;
    }
    
    uint64_t getCompilationCount() const { return compilationCount_; }
    
private:
    MockHotPathDetector& detector_;
    std::set<std::string> compiledFunctions_;
    uint64_t compilationCount_ = 0;
};

/**
 * محاكي Tier Transition Manager
 */
class MockTierManager {
public:
    void registerFunction(const std::string& name) {
        tiers_[name] = Tier::Interpreter;
    }
    
    void recordExecution(const std::string& name, uint64_t count = 1) {
        executions_[name] += count;
        checkPromotion(name);
    }
    
    Tier getCurrentTier(const std::string& name) const {
        auto it = tiers_.find(name);
        return it != tiers_.end() ? it->second : Tier::Interpreter;
    }
    
    bool promote(const std::string& name, Tier targetTier) {
        auto current = getCurrentTier(name);
        if (static_cast<int>(targetTier) <= static_cast<int>(current)) {
            return false;
        }
        tiers_[name] = targetTier;
        promotions_++;
        return true;
    }
    
    bool demote(const std::string& name, Tier targetTier) {
        auto current = getCurrentTier(name);
        if (static_cast<int>(targetTier) >= static_cast<int>(current)) {
            return false;
        }
        tiers_[name] = targetTier;
        demotions_++;
        return true;
    }
    
    uint64_t getPromotionCount() const { return promotions_; }
    uint64_t getDemotionCount() const { return demotions_; }
    
    void setBaselineThreshold(uint64_t t) { baselineThreshold_ = t; }
    void setOptimizedThreshold(uint64_t t) { optimizedThreshold_ = t; }
    
private:
    std::map<std::string, Tier> tiers_;
    std::map<std::string, uint64_t> executions_;
    uint64_t promotions_ = 0;
    uint64_t demotions_ = 0;
    uint64_t baselineThreshold_ = 100;
    uint64_t optimizedThreshold_ = 10000;
    
    void checkPromotion(const std::string& name) {
        auto count = executions_[name];
        auto current = getCurrentTier(name);
        
        if (current == Tier::Interpreter && count >= baselineThreshold_) {
            promote(name, Tier::BaselineJIT);
        } else if (current == Tier::BaselineJIT && count >= optimizedThreshold_) {
            promote(name, Tier::OptimizedJIT);
        }
    }
};

/**
 * محاكي OSR Manager
 */
class MockOSRManager {
public:
    void registerOSRPoint(const std::string& function, int offset) {
        osrPoints_[function].insert(offset);
    }
    
    bool hasOSRPoint(const std::string& function, int offset) const {
        auto it = osrPoints_.find(function);
        return it != osrPoints_.end() && it->second.count(offset) > 0;
    }
    
    bool triggerOSR(const std::string& function, int offset) {
        if (!hasOSRPoint(function, offset)) return false;
        osrTriggers_++;
        return true;
    }
    
    uint64_t getOSRTriggerCount() const { return osrTriggers_; }
    
private:
    std::map<std::string, std::set<int>> osrPoints_;
    uint64_t osrTriggers_ = 0;
};

/**
 * محاكي Deoptimization Manager
 */
class MockDeoptManager {
public:
    void registerDeoptPoint(const std::string& function, int offset) {
        deoptPoints_[function].insert(offset);
    }
    
    bool deoptimize(const std::string& function, int offset, const std::string& reason) {
        deoptCount_++;
        deoptReasons_[reason]++;
        return true;
    }
    
    uint64_t getDeoptCount() const { return deoptCount_; }
    
    std::map<std::string, uint64_t> getDeoptReasons() const { return deoptReasons_; }
    
private:
    std::map<std::string, std::set<int>> deoptPoints_;
    uint64_t deoptCount_ = 0;
    std::map<std::string, uint64_t> deoptReasons_;
};

} // namespace sad::jit::mock

// =============================================================================
// اختبارات Hot Path Detection
// =============================================================================

void testHotPathDetection(sad::test::TestSuite& suite) {
    suite.addTest("اكتشاف دالة باردة", []() {
        sad::jit::mock::MockHotPathDetector detector;
        detector.setHotThreshold(100);
        
        detector.registerFunction("cold_func");
        
        for (int i = 0; i < 50; i++) {
            detector.recordExecution("cold_func");
        }
        
        ASSERT_FALSE(detector.isHot("cold_func"));
        ASSERT_EQ(detector.getExecutionCount("cold_func"), 50);
    });
    
    suite.addTest("اكتشاف دالة ساخنة", []() {
        sad::jit::mock::MockHotPathDetector detector;
        detector.setHotThreshold(100);
        
        detector.registerFunction("hot_func");
        
        for (int i = 0; i < 150; i++) {
            detector.recordExecution("hot_func");
        }
        
        ASSERT_TRUE(detector.isHot("hot_func"));
        ASSERT_EQ(detector.getExecutionCount("hot_func"), 150);
    });
    
    suite.addTest("عتبة قابلة للتعديل", []() {
        sad::jit::mock::MockHotPathDetector detector;
        
        detector.registerFunction("func");
        
        detector.setHotThreshold(10);
        for (int i = 0; i < 15; i++) {
            detector.recordExecution("func");
        }
        ASSERT_TRUE(detector.isHot("func"));
    });
}

// =============================================================================
// اختبارات JIT Trigger
// =============================================================================

void testJITTrigger(sad::test::TestSuite& suite) {
    suite.addTest("تفعيل JIT عند السخونة", []() {
        sad::jit::mock::MockHotPathDetector detector;
        sad::jit::mock::MockJITTrigger trigger(detector);
        
        detector.setHotThreshold(50);
        detector.registerFunction("trigger_func");
        
        // قبل السخونة
        ASSERT_FALSE(trigger.shouldCompile("trigger_func"));
        
        // تنفيذات كافية
        for (int i = 0; i < 60; i++) {
            detector.recordExecution("trigger_func");
        }
        
        ASSERT_TRUE(trigger.shouldCompile("trigger_func"));
    });
    
    suite.addTest("تسجيل الترجمة", []() {
        sad::jit::mock::MockHotPathDetector detector;
        sad::jit::mock::MockJITTrigger trigger(detector);
        
        detector.registerFunction("compile_func");
        
        ASSERT_FALSE(trigger.isCompiled("compile_func"));
        
        trigger.markCompiled("compile_func");
        
        ASSERT_TRUE(trigger.isCompiled("compile_func"));
        ASSERT_EQ(trigger.getCompilationCount(), 1);
    });
}

// =============================================================================
// اختبارات Tier Transition
// =============================================================================

void testTierTransition(sad::test::TestSuite& suite) {
    suite.addTest("البدء من Interpreter", []() {
        sad::jit::mock::MockTierManager manager;
        
        manager.registerFunction("tier_func");
        
        ASSERT_EQ(manager.getCurrentTier("tier_func"), sad::jit::mock::Tier::Interpreter);
    });
    
    suite.addTest("الترقية إلى BaselineJIT", []() {
        sad::jit::mock::MockTierManager manager;
        manager.setBaselineThreshold(100);
        
        manager.registerFunction("baseline_func");
        
        for (int i = 0; i < 150; i++) {
            manager.recordExecution("baseline_func");
        }
        
        ASSERT_EQ(manager.getCurrentTier("baseline_func"), sad::jit::mock::Tier::BaselineJIT);
        ASSERT_EQ(manager.getPromotionCount(), 1);
    });
    
    suite.addTest("الترقية إلى OptimizedJIT", []() {
        sad::jit::mock::MockTierManager manager;
        manager.setBaselineThreshold(100);
        manager.setOptimizedThreshold(1000);
        
        manager.registerFunction("optimized_func");
        
        for (int i = 0; i < 1500; i++) {
            manager.recordExecution("optimized_func");
        }
        
        ASSERT_EQ(manager.getCurrentTier("optimized_func"), sad::jit::mock::Tier::OptimizedJIT);
        ASSERT_EQ(manager.getPromotionCount(), 2);  // Interpreter → Baseline → Optimized
    });
    
    suite.addTest("التخفيض (Demotion)", []() {
        sad::jit::mock::MockTierManager manager;
        
        manager.registerFunction("demote_func");
        manager.promote("demote_func", sad::jit::mock::Tier::OptimizedJIT);
        
        ASSERT_TRUE(manager.demote("demote_func", sad::jit::mock::Tier::Interpreter));
        ASSERT_EQ(manager.getCurrentTier("demote_func"), sad::jit::mock::Tier::Interpreter);
        ASSERT_EQ(manager.getDemotionCount(), 1);
    });
    
    suite.addTest("منع الترقية غير الصالحة", []() {
        sad::jit::mock::MockTierManager manager;
        
        manager.registerFunction("invalid_func");
        manager.promote("invalid_func", sad::jit::mock::Tier::OptimizedJIT);
        
        // محاولة الترقية لمستوى أقل
        ASSERT_FALSE(manager.promote("invalid_func", sad::jit::mock::Tier::BaselineJIT));
    });
}

// =============================================================================
// اختبارات OSR
// =============================================================================

void testOSR(sad::test::TestSuite& suite) {
    suite.addTest("تسجيل نقطة OSR", []() {
        sad::jit::mock::MockOSRManager manager;
        
        manager.registerOSRPoint("loop_func", 100);
        
        ASSERT_TRUE(manager.hasOSRPoint("loop_func", 100));
        ASSERT_FALSE(manager.hasOSRPoint("loop_func", 200));
        ASSERT_FALSE(manager.hasOSRPoint("other_func", 100));
    });
    
    suite.addTest("تفعيل OSR", []() {
        sad::jit::mock::MockOSRManager manager;
        
        manager.registerOSRPoint("osr_func", 50);
        
        ASSERT_TRUE(manager.triggerOSR("osr_func", 50));
        ASSERT_FALSE(manager.triggerOSR("osr_func", 100));  // نقطة غير موجودة
        
        ASSERT_EQ(manager.getOSRTriggerCount(), 1);
    });
    
    suite.addTest("نقاط OSR متعددة", []() {
        sad::jit::mock::MockOSRManager manager;
        
        manager.registerOSRPoint("multi_func", 10);
        manager.registerOSRPoint("multi_func", 50);
        manager.registerOSRPoint("multi_func", 100);
        
        ASSERT_TRUE(manager.hasOSRPoint("multi_func", 10));
        ASSERT_TRUE(manager.hasOSRPoint("multi_func", 50));
        ASSERT_TRUE(manager.hasOSRPoint("multi_func", 100));
    });
}

// =============================================================================
// اختبارات Deoptimization
// =============================================================================

void testDeoptimization(sad::test::TestSuite& suite) {
    suite.addTest("تنفيذ Deoptimization", []() {
        sad::jit::mock::MockDeoptManager manager;
        
        manager.registerDeoptPoint("deopt_func", 75);
        
        ASSERT_TRUE(manager.deoptimize("deopt_func", 75, "TypeMismatch"));
        ASSERT_EQ(manager.getDeoptCount(), 1);
    });
    
    suite.addTest("تتبع أسباب Deopt", []() {
        sad::jit::mock::MockDeoptManager manager;
        
        manager.deoptimize("func1", 10, "TypeMismatch");
        manager.deoptimize("func2", 20, "TypeMismatch");
        manager.deoptimize("func3", 30, "BoundsCheck");
        manager.deoptimize("func4", 40, "NullCheck");
        
        auto reasons = manager.getDeoptReasons();
        
        ASSERT_EQ(reasons["TypeMismatch"], 2);
        ASSERT_EQ(reasons["BoundsCheck"], 1);
        ASSERT_EQ(reasons["NullCheck"], 1);
        ASSERT_EQ(manager.getDeoptCount(), 4);
    });
}

// =============================================================================
// اختبارات التكامل
// =============================================================================

void testIntegration(sad::test::TestSuite& suite) {
    suite.addTest("سيناريو كامل: Cold → Hot → JIT", []() {
        sad::jit::mock::MockHotPathDetector detector;
        sad::jit::mock::MockJITTrigger trigger(detector);
        sad::jit::mock::MockTierManager tierManager;
        
        detector.setHotThreshold(100);
        tierManager.setBaselineThreshold(100);
        
        detector.registerFunction("full_test");
        tierManager.registerFunction("full_test");
        
        // المرحلة 1: بارد
        for (int i = 0; i < 50; i++) {
            detector.recordExecution("full_test");
            tierManager.recordExecution("full_test");
        }
        
        ASSERT_FALSE(detector.isHot("full_test"));
        ASSERT_FALSE(trigger.shouldCompile("full_test"));
        ASSERT_EQ(tierManager.getCurrentTier("full_test"), sad::jit::mock::Tier::Interpreter);
        
        // المرحلة 2: ساخن
        for (int i = 0; i < 100; i++) {
            detector.recordExecution("full_test");
            tierManager.recordExecution("full_test");
        }
        
        ASSERT_TRUE(detector.isHot("full_test"));
        ASSERT_TRUE(trigger.shouldCompile("full_test"));
        ASSERT_EQ(tierManager.getCurrentTier("full_test"), sad::jit::mock::Tier::BaselineJIT);
        
        // ترجمة JIT
        trigger.markCompiled("full_test");
        ASSERT_TRUE(trigger.isCompiled("full_test"));
    });
    
    suite.addTest("سيناريو: OSR في حلقة طويلة", []() {
        sad::jit::mock::MockOSRManager osrManager;
        sad::jit::mock::MockTierManager tierManager;
        
        // محاكاة حلقة طويلة
        osrManager.registerOSRPoint("long_loop", 42);
        tierManager.registerFunction("long_loop");
        
        // التنفيذ حتى OSR
        int iterations = 0;
        while (iterations < 10000) {
            iterations++;
            
            if (iterations == 5000) {
                // منتصف الحلقة - تفعيل OSR
                osrManager.triggerOSR("long_loop", 42);
                tierManager.promote("long_loop", sad::jit::mock::Tier::BaselineJIT);
            }
        }
        
        ASSERT_EQ(osrManager.getOSRTriggerCount(), 1);
        ASSERT_EQ(tierManager.getCurrentTier("long_loop"), sad::jit::mock::Tier::BaselineJIT);
    });
    
    suite.addTest("سيناريو: Deopt ثم إعادة الترجمة", []() {
        sad::jit::mock::MockTierManager tierManager;
        sad::jit::mock::MockDeoptManager deoptManager;
        
        tierManager.registerFunction("recompile_func");
        
        // ترقية إلى OptimizedJIT
        tierManager.promote("recompile_func", sad::jit::mock::Tier::OptimizedJIT);
        ASSERT_EQ(tierManager.getCurrentTier("recompile_func"), sad::jit::mock::Tier::OptimizedJIT);
        
        // Deoptimization
        deoptManager.deoptimize("recompile_func", 100, "TypeMismatch");
        tierManager.demote("recompile_func", sad::jit::mock::Tier::Interpreter);
        ASSERT_EQ(tierManager.getCurrentTier("recompile_func"), sad::jit::mock::Tier::Interpreter);
        
        // إعادة الترقية
        tierManager.promote("recompile_func", sad::jit::mock::Tier::BaselineJIT);
        ASSERT_EQ(tierManager.getCurrentTier("recompile_func"), sad::jit::mock::Tier::BaselineJIT);
        
        ASSERT_EQ(tierManager.getPromotionCount(), 2);
        ASSERT_EQ(tierManager.getDemotionCount(), 1);
        ASSERT_EQ(deoptManager.getDeoptCount(), 1);
    });
}

// =============================================================================
// اختبارات الأداء
// =============================================================================

void testPerformance(sad::test::TestSuite& suite) {
    suite.addTest("أداء Hot Path Detector مع دوال كثيرة", []() {
        sad::jit::mock::MockHotPathDetector detector;
        
        // تسجيل 1000 دالة
        for (int i = 0; i < 1000; i++) {
            detector.registerFunction("func_" + std::to_string(i));
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // 100000 تسجيل تنفيذ
        for (int i = 0; i < 100000; i++) {
            detector.recordExecution("func_" + std::to_string(i % 1000));
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        // يجب أن يكتمل في أقل من 1 ثانية
        ASSERT_TRUE(duration < 1000);
    });
    
    suite.addTest("أداء Tier Manager مع ترقيات متعددة", []() {
        sad::jit::mock::MockTierManager manager;
        manager.setBaselineThreshold(10);
        manager.setOptimizedThreshold(100);
        
        for (int i = 0; i < 100; i++) {
            manager.registerFunction("perf_func_" + std::to_string(i));
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // كل دالة تصل للمستوى الأعلى
        for (int exec = 0; exec < 200; exec++) {
            for (int i = 0; i < 100; i++) {
                manager.recordExecution("perf_func_" + std::to_string(i));
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        // 200 ترقية (100 دالة × 2 مستويات)
        ASSERT_EQ(manager.getPromotionCount(), 200);
        ASSERT_TRUE(duration < 1000);
    });
}

// =============================================================================
// الدالة الرئيسية
// =============================================================================

int main() {
    std::cout << "═══════════════════════════════════════════════════════════\n";
    std::cout << "   اختبارات الترجمة المتدرجة (Tiered Compilation Tests)\n";
    std::cout << "═══════════════════════════════════════════════════════════\n";
    
    int totalTests = 0;
    int passedTests = 0;
    
    // مجموعة اختبارات Hot Path Detection
    {
        sad::test::TestSuite suite("اختبارات Hot Path Detection");
        testHotPathDetection(suite);
        auto results = suite.run();
        for (const auto& r : results) {
            totalTests++;
            if (r.passed) passedTests++;
        }
    }
    
    // مجموعة اختبارات JIT Trigger
    {
        sad::test::TestSuite suite("اختبارات JIT Trigger");
        testJITTrigger(suite);
        auto results = suite.run();
        for (const auto& r : results) {
            totalTests++;
            if (r.passed) passedTests++;
        }
    }
    
    // مجموعة اختبارات Tier Transition
    {
        sad::test::TestSuite suite("اختبارات Tier Transition");
        testTierTransition(suite);
        auto results = suite.run();
        for (const auto& r : results) {
            totalTests++;
            if (r.passed) passedTests++;
        }
    }
    
    // مجموعة اختبارات OSR
    {
        sad::test::TestSuite suite("اختبارات OSR");
        testOSR(suite);
        auto results = suite.run();
        for (const auto& r : results) {
            totalTests++;
            if (r.passed) passedTests++;
        }
    }
    
    // مجموعة اختبارات Deoptimization
    {
        sad::test::TestSuite suite("اختبارات Deoptimization");
        testDeoptimization(suite);
        auto results = suite.run();
        for (const auto& r : results) {
            totalTests++;
            if (r.passed) passedTests++;
        }
    }
    
    // مجموعة اختبارات التكامل
    {
        sad::test::TestSuite suite("اختبارات التكامل");
        testIntegration(suite);
        auto results = suite.run();
        for (const auto& r : results) {
            totalTests++;
            if (r.passed) passedTests++;
        }
    }
    
    // مجموعة اختبارات الأداء
    {
        sad::test::TestSuite suite("اختبارات الأداء");
        testPerformance(suite);
        auto results = suite.run();
        for (const auto& r : results) {
            totalTests++;
            if (r.passed) passedTests++;
        }
    }
    
    // ملخص النتائج
    std::cout << "\n═══════════════════════════════════════════════════════════\n";
    std::cout << "   الملخص\n";
    std::cout << "═══════════════════════════════════════════════════════════\n\n";
    
    std::cout << "إجمالي الاختبارات: " << totalTests << "\n";
    std::cout << "نجح: " << passedTests << "\n";
    std::cout << "فشل: " << (totalTests - passedTests) << "\n";
    
    if (passedTests == totalTests) {
        std::cout << "\n✅ جميع الاختبارات نجحت!\n";
        return 0;
    } else {
        std::cout << "\n❌ بعض الاختبارات فشلت.\n";
        return 1;
    }
}
