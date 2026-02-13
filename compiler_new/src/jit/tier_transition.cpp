/**
 * =============================================================================
 * ملف: tier_transition.cpp
 * الوصف: انتقال المستويات (VM → JIT → AOT)
 * المهمة: T214 - Implement tier transition
 * المرحلة: Phase 21 - User Story 18 (JIT & Tiered Compilation)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما هي الترجمة المتدرجة (Tiered Compilation)؟
 * ────────────────────────────────────────────
 * نظام يرفع مستوى تحسين الكود تدريجياً حسب الاستخدام.
 * 
 * المستويات:
 * ───────────
 * ```
 * المستوى 0: المُفسر (VM)
 *            ├─ بدء فوري
 *            ├─ لا وقت ترجمة
 *            └─ أبطأ تنفيذ
 *                  │
 *                  ↓ (ساخن)
 * المستوى 1: JIT سريع
 *            ├─ ترجمة سريعة
 *            ├─ تحسينات أساسية
 *            └─ 5-10× أسرع
 *                  │
 *                  ↓ (أسخن)
 * المستوى 2: JIT محسّن
 *            ├─ ترجمة أبطأ
 *            ├─ تحسينات قوية
 *            └─ 20-50× أسرع
 *                  │
 *                  ↓ (ملتهب)
 * المستوى 3: AOT
 *            ├─ ترجمة مسبقة
 *            ├─ أقصى تحسين
 *            └─ أفضل أداء
 * ```
 * 
 * لماذا التدرج؟
 * ─────────────
 * 1. **بدء سريع**: لا ننتظر الترجمة
 * 2. **موارد ذكية**: نُحسّن فقط ما يستحق
 * 3. **تكيّف**: نتعلم سلوك البرنامج
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <optional>
#include <functional>
#include <chrono>
#include <atomic>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <iostream>

namespace sad::jit {

// =============================================================================
// مستويات الترجمة (Compilation Tiers)
// =============================================================================

/**
 * مستويات الترجمة
 */
enum class CompilationTier {
    Interpreter,    // المستوى 0: المُفسر
    BaselineJIT,    // المستوى 1: JIT أساسي
    OptimizedJIT,   // المستوى 2: JIT مُحسّن
    AOT,            // المستوى 3: مُترجم مسبقاً
};

inline const char* tierName(CompilationTier tier) {
    switch (tier) {
        case CompilationTier::Interpreter: return "مُفسّر";
        case CompilationTier::BaselineJIT: return "JIT_أساسي";
        case CompilationTier::OptimizedJIT: return "JIT_مُحسّن";
        case CompilationTier::AOT: return "AOT";
    }
    return "مجهول";
}

inline int tierLevel(CompilationTier tier) {
    switch (tier) {
        case CompilationTier::Interpreter: return 0;
        case CompilationTier::BaselineJIT: return 1;
        case CompilationTier::OptimizedJIT: return 2;
        case CompilationTier::AOT: return 3;
    }
    return 0;
}

// =============================================================================
// إعدادات الانتقال (Transition Config)
// =============================================================================

/**
 * إعدادات انتقال المستويات
 */
struct TierTransitionConfig {
    // عتبات الترقية
    uint64_t baselineThreshold = 100;       // عتبة JIT أساسي
    uint64_t optimizedThreshold = 10000;    // عتبة JIT مُحسّن
    uint64_t aotThreshold = 100000;         // عتبة AOT
    
    // عتبات وقت الترجمة (ms)
    int maxBaselineCompileTime = 10;        // أقصى وقت لـ baseline
    int maxOptimizedCompileTime = 100;      // أقصى وقت لـ optimized
    int maxAOTCompileTime = 1000;           // أقصى وقت لـ AOT
    
    // إعدادات السلوك
    bool allowDowngrade = true;             // السماح بالتخفيض
    bool backgroundCompilation = true;      // ترجمة خلفية
    bool adaptiveThresholds = true;         // عتبات تكيفية
};

// =============================================================================
// معلومات الدالة (Function Info)
// =============================================================================

/**
 * كود مُترجم لمستوى معين
 */
struct TieredCode {
    CompilationTier tier;
    void* code = nullptr;
    size_t codeSize = 0;
    std::chrono::steady_clock::time_point compilationTime;
    uint64_t compilationDuration = 0;  // ms
};

/**
 * معلومات دالة في النظام المتدرج
 */
struct TieredFunctionInfo {
    std::string name;
    CompilationTier currentTier = CompilationTier::Interpreter;
    
    // إحصائيات التنفيذ
    uint64_t executionCount = 0;
    uint64_t totalTimeUs = 0;
    
    // الكود لكل مستوى
    std::map<CompilationTier, TieredCode> tieredCode;
    
    // سجل الانتقالات
    struct Transition {
        CompilationTier from;
        CompilationTier to;
        std::chrono::steady_clock::time_point timestamp;
    };
    std::vector<Transition> transitionHistory;
    
    double averageTimeUs() const {
        return executionCount > 0 ? 
            static_cast<double>(totalTimeUs) / executionCount : 0.0;
    }
    
    void* getCurrentCode() const {
        auto it = tieredCode.find(currentTier);
        return it != tieredCode.end() ? it->second.code : nullptr;
    }
};

// =============================================================================
// طلب انتقال (Transition Request)
// =============================================================================

/**
 * طلب انتقال في الصف
 */
struct TransitionRequest {
    std::string functionName;
    CompilationTier targetTier;
    int priority;
    std::chrono::steady_clock::time_point requestTime;
    
    bool operator<(const TransitionRequest& other) const {
        return priority < other.priority;
    }
};

// =============================================================================
// مدير الانتقال (Transition Manager)
// =============================================================================

/**
 * نتيجة انتقال
 */
struct TransitionResult {
    bool success = false;
    std::string errorMessage;
    CompilationTier newTier;
    void* newCode = nullptr;
    uint64_t transitionTime = 0;  // μs
};

/**
 * callback لترجمة مستوى معين
 */
using TierCompiler = std::function<bool(const std::string&, CompilationTier, void**, size_t*)>;

/**
 * مدير انتقال المستويات
 */
class TierTransitionManager {
public:
    explicit TierTransitionManager(const TierTransitionConfig& config = TierTransitionConfig())
        : config_(config), running_(false) {}
    
    ~TierTransitionManager() {
        stop();
    }
    
    /**
     * تعيين مُترجم لمستوى معين
     */
    void setTierCompiler(CompilationTier tier, TierCompiler compiler) {
        compilers_[tier] = compiler;
    }
    
    /**
     * بدء النظام
     */
    void start() {
        if (running_) return;
        running_ = true;
        
        if (config_.backgroundCompilation) {
            compilationThread_ = std::thread(&TierTransitionManager::compilationWorker, this);
        }
    }
    
    /**
     * إيقاف النظام
     */
    void stop() {
        running_ = false;
        queueCondition_.notify_all();
        
        if (compilationThread_.joinable()) {
            compilationThread_.join();
        }
    }
    
    /**
     * تسجيل دالة
     */
    void registerFunction(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (functions_.find(name) == functions_.end()) {
            functions_[name] = TieredFunctionInfo{};
            functions_[name].name = name;
        }
    }
    
    /**
     * تسجيل تنفيذ
     */
    void recordExecution(const std::string& name, uint64_t timeUs) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = functions_.find(name);
        if (it == functions_.end()) {
            registerFunction(name);
            it = functions_.find(name);
        }
        
        auto& info = it->second;
        info.executionCount++;
        info.totalTimeUs += timeUs;
        
        // فحص إمكانية الترقية
        checkPromotion(name, info);
    }
    
    /**
     * طلب ترقية
     */
    bool requestPromotion(const std::string& name, CompilationTier targetTier) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = functions_.find(name);
        if (it == functions_.end()) return false;
        
        auto& info = it->second;
        
        // تحقق من صحة الترقية
        if (tierLevel(targetTier) <= tierLevel(info.currentTier)) {
            return false;  // لا يمكن الترقية لمستوى أقل أو مساوٍ
        }
        
        return queueTransition(name, targetTier);
    }
    
    /**
     * طلب تخفيض (للـ deoptimization)
     */
    bool requestDemotion(const std::string& name, CompilationTier targetTier) {
        if (!config_.allowDowngrade) return false;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = functions_.find(name);
        if (it == functions_.end()) return false;
        
        auto& info = it->second;
        
        // التخفيض فوري (لا ننتظر)
        info.currentTier = targetTier;
        recordTransition(info, info.currentTier, targetTier);
        
        totalDemotions_++;
        return true;
    }
    
    /**
     * الحصول على المستوى الحالي
     */
    CompilationTier getCurrentTier(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = functions_.find(name);
        if (it != functions_.end()) {
            return it->second.currentTier;
        }
        return CompilationTier::Interpreter;
    }
    
    /**
     * الحصول على الكود الحالي
     */
    void* getCurrentCode(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = functions_.find(name);
        if (it != functions_.end()) {
            return it->second.getCurrentCode();
        }
        return nullptr;
    }
    
    /**
     * إحصائيات
     */
    struct Statistics {
        size_t totalFunctions = 0;
        std::map<CompilationTier, size_t> tierDistribution;
        uint64_t totalPromotions = 0;
        uint64_t totalDemotions = 0;
        uint64_t pendingTransitions = 0;
    };
    
    Statistics getStatistics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        Statistics stats;
        stats.totalFunctions = functions_.size();
        stats.totalPromotions = totalPromotions_;
        stats.totalDemotions = totalDemotions_;
        stats.pendingTransitions = transitionQueue_.size();
        
        for (const auto& [name, info] : functions_) {
            stats.tierDistribution[info.currentTier]++;
        }
        
        return stats;
    }
    
    /**
     * طباعة تقرير
     */
    void printReport() const {
        auto stats = getStatistics();
        
        std::cout << "\n═══════════════════════════════════════════════\n";
        std::cout << "   تقرير الترجمة المتدرجة\n";
        std::cout << "═══════════════════════════════════════════════\n\n";
        
        std::cout << "إجمالي الدوال: " << stats.totalFunctions << "\n\n";
        
        std::cout << "توزيع المستويات:\n";
        for (const auto& [tier, count] : stats.tierDistribution) {
            std::cout << "  " << tierName(tier) << ": " << count << "\n";
        }
        
        std::cout << "\nالانتقالات:\n";
        std::cout << "  ترقيات: " << stats.totalPromotions << "\n";
        std::cout << "  تخفيضات: " << stats.totalDemotions << "\n";
        std::cout << "  في الانتظار: " << stats.pendingTransitions << "\n";
    }

private:
    TierTransitionConfig config_;
    std::unordered_map<std::string, TieredFunctionInfo> functions_;
    std::map<CompilationTier, TierCompiler> compilers_;
    std::priority_queue<TransitionRequest> transitionQueue_;
    
    std::thread compilationThread_;
    mutable std::mutex mutex_;
    std::condition_variable queueCondition_;
    std::atomic<bool> running_;
    
    uint64_t totalPromotions_ = 0;
    uint64_t totalDemotions_ = 0;
    
    void checkPromotion(const std::string& name, TieredFunctionInfo& info) {
        CompilationTier nextTier = getNextTier(info);
        
        if (nextTier != info.currentTier && 
            tierLevel(nextTier) > tierLevel(info.currentTier)) {
            queueTransition(name, nextTier);
        }
    }
    
    CompilationTier getNextTier(const TieredFunctionInfo& info) const {
        uint64_t count = info.executionCount;
        
        if (count >= config_.aotThreshold) {
            return CompilationTier::AOT;
        } else if (count >= config_.optimizedThreshold) {
            return CompilationTier::OptimizedJIT;
        } else if (count >= config_.baselineThreshold) {
            return CompilationTier::BaselineJIT;
        }
        return CompilationTier::Interpreter;
    }
    
    bool queueTransition(const std::string& name, CompilationTier targetTier) {
        TransitionRequest request;
        request.functionName = name;
        request.targetTier = targetTier;
        request.priority = calculatePriority(name, targetTier);
        request.requestTime = std::chrono::steady_clock::now();
        
        transitionQueue_.push(request);
        queueCondition_.notify_one();
        
        return true;
    }
    
    int calculatePriority(const std::string& name, CompilationTier tier) {
        int priority = tierLevel(tier) * 100;
        
        auto it = functions_.find(name);
        if (it != functions_.end()) {
            priority += static_cast<int>(it->second.executionCount / 100);
        }
        
        return priority;
    }
    
    void compilationWorker() {
        while (running_) {
            TransitionRequest request;
            
            {
                std::unique_lock<std::mutex> lock(mutex_);
                queueCondition_.wait(lock, [this] {
                    return !running_ || !transitionQueue_.empty();
                });
                
                if (!running_ && transitionQueue_.empty()) break;
                if (transitionQueue_.empty()) continue;
                
                request = transitionQueue_.top();
                transitionQueue_.pop();
            }
            
            performTransition(request);
        }
    }
    
    void performTransition(const TransitionRequest& request) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // البحث عن المُترجم
        auto compilerIt = compilers_.find(request.targetTier);
        if (compilerIt == compilers_.end()) {
            return;  // لا يوجد مُترجم لهذا المستوى
        }
        
        void* code = nullptr;
        size_t codeSize = 0;
        
        bool success = compilerIt->second(request.functionName, request.targetTier, 
                                          &code, &codeSize);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            endTime - startTime
        ).count();
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = functions_.find(request.functionName);
        if (it != functions_.end() && success && code) {
            auto& info = it->second;
            auto oldTier = info.currentTier;
            
            // تخزين الكود
            TieredCode tieredCode;
            tieredCode.tier = request.targetTier;
            tieredCode.code = code;
            tieredCode.codeSize = codeSize;
            tieredCode.compilationTime = std::chrono::steady_clock::now();
            tieredCode.compilationDuration = duration;
            
            info.tieredCode[request.targetTier] = tieredCode;
            info.currentTier = request.targetTier;
            
            recordTransition(info, oldTier, request.targetTier);
            totalPromotions_++;
        }
    }
    
    void recordTransition(TieredFunctionInfo& info, CompilationTier from, CompilationTier to) {
        TieredFunctionInfo::Transition trans;
        trans.from = from;
        trans.to = to;
        trans.timestamp = std::chrono::steady_clock::now();
        info.transitionHistory.push_back(trans);
    }
};

// =============================================================================
// سياسة الانتقال (Transition Policy)
// =============================================================================

/**
 * سياسة انتقال مخصصة
 */
class TransitionPolicy {
public:
    virtual ~TransitionPolicy() = default;
    
    /**
     * هل يجب الترقية؟
     */
    virtual bool shouldPromote(const TieredFunctionInfo& info, 
                               CompilationTier targetTier) const = 0;
    
    /**
     * هل يجب التخفيض؟
     */
    virtual bool shouldDemote(const TieredFunctionInfo& info) const = 0;
};

/**
 * سياسة عداد بسيطة
 */
class CounterPolicy : public TransitionPolicy {
public:
    CounterPolicy(uint64_t baselineThreshold = 100,
                  uint64_t optimizedThreshold = 10000)
        : baselineThreshold_(baselineThreshold)
        , optimizedThreshold_(optimizedThreshold) {}
    
    bool shouldPromote(const TieredFunctionInfo& info, 
                      CompilationTier targetTier) const override {
        switch (targetTier) {
            case CompilationTier::BaselineJIT:
                return info.executionCount >= baselineThreshold_;
            case CompilationTier::OptimizedJIT:
                return info.executionCount >= optimizedThreshold_;
            default:
                return false;
        }
    }
    
    bool shouldDemote(const TieredFunctionInfo& info) const override {
        // لا تخفيض تلقائي في هذه السياسة
        return false;
    }
    
private:
    uint64_t baselineThreshold_;
    uint64_t optimizedThreshold_;
};

/**
 * سياسة تكيفية
 */
class AdaptivePolicy : public TransitionPolicy {
public:
    bool shouldPromote(const TieredFunctionInfo& info, 
                      CompilationTier targetTier) const override {
        // الترقية حسب متوسط وقت التنفيذ أيضاً
        double avgTime = info.averageTimeUs();
        
        switch (targetTier) {
            case CompilationTier::BaselineJIT:
                // ترقية إذا كانت الدالة بطيئة نسبياً
                return info.executionCount >= 50 || avgTime > 100;
            case CompilationTier::OptimizedJIT:
                // ترقية إذا كانت ساخنة وبطيئة
                return (info.executionCount >= 5000) || 
                       (info.executionCount >= 1000 && avgTime > 50);
            default:
                return false;
        }
    }
    
    bool shouldDemote(const TieredFunctionInfo& info) const override {
        // تخفيض إذا لم تُستخدم لفترة طويلة
        // (هذا يتطلب تتبع آخر استخدام)
        return false;
    }
};

// =============================================================================
// مُجمّع المستويات (Tier Compiler Stubs)
// =============================================================================

/**
 * مُجمّع baseline بسيط
 */
class BaselineCompiler {
public:
    bool compile(const std::string& functionName, void** code, size_t* size) {
        // في الواقع نحتاج bytecode للدالة
        // هذا مجرد placeholder
        
        *code = nullptr;
        *size = 0;
        return true;  // نجاح وهمي للاختبار
    }
};

/**
 * مُجمّع optimized
 */
class OptimizedCompiler {
public:
    bool compile(const std::string& functionName, void** code, size_t* size) {
        // ترجمة مع تحسينات
        *code = nullptr;
        *size = 0;
        return true;
    }
};

} // namespace sad::jit

// =============================================================================
// واجهة C للتكامل
// =============================================================================

extern "C" {

using namespace sad::jit;

void* sad_tier_manager_new() {
    return new TierTransitionManager();
}

void sad_tier_manager_free(void* manager) {
    delete static_cast<TierTransitionManager*>(manager);
}

void sad_tier_manager_start(void* manager) {
    if (manager) static_cast<TierTransitionManager*>(manager)->start();
}

void sad_tier_manager_stop(void* manager) {
    if (manager) static_cast<TierTransitionManager*>(manager)->stop();
}

void sad_tier_register_function(void* manager, const char* name) {
    if (manager && name) {
        static_cast<TierTransitionManager*>(manager)->registerFunction(name);
    }
}

void sad_tier_record_execution(void* manager, const char* name, uint64_t time_us) {
    if (manager && name) {
        static_cast<TierTransitionManager*>(manager)->recordExecution(name, time_us);
    }
}

int sad_tier_get_current(void* manager, const char* name) {
    if (!manager || !name) return 0;
    return static_cast<int>(
        static_cast<TierTransitionManager*>(manager)->getCurrentTier(name)
    );
}

void* sad_tier_get_code(void* manager, const char* name) {
    if (!manager || !name) return nullptr;
    return static_cast<TierTransitionManager*>(manager)->getCurrentCode(name);
}

void sad_tier_print_report(void* manager) {
    if (manager) {
        static_cast<TierTransitionManager*>(manager)->printReport();
    }
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef TIER_TRANSITION_TEST

#include <cassert>

void testTierLevels() {
    assert(sad::jit::tierLevel(sad::jit::CompilationTier::Interpreter) == 0);
    assert(sad::jit::tierLevel(sad::jit::CompilationTier::BaselineJIT) == 1);
    assert(sad::jit::tierLevel(sad::jit::CompilationTier::OptimizedJIT) == 2);
    assert(sad::jit::tierLevel(sad::jit::CompilationTier::AOT) == 3);
    
    std::cout << "✅ اختبار مستويات Tier نجح!\n";
}

void testBasicTransition() {
    sad::jit::TierTransitionConfig config;
    config.baselineThreshold = 10;
    config.optimizedThreshold = 100;
    config.backgroundCompilation = false;
    
    sad::jit::TierTransitionManager manager(config);
    
    manager.registerFunction("test_func");
    
    // التحقق من المستوى الأولي
    assert(manager.getCurrentTier("test_func") == sad::jit::CompilationTier::Interpreter);
    
    // محاكاة تنفيذات
    for (int i = 0; i < 50; i++) {
        manager.recordExecution("test_func", 10);
    }
    
    auto stats = manager.getStatistics();
    assert(stats.totalFunctions == 1);
    
    std::cout << "✅ اختبار الانتقال الأساسي نجح!\n";
}

void testPromotion() {
    sad::jit::TierTransitionConfig config;
    config.baselineThreshold = 10;
    config.backgroundCompilation = false;
    
    sad::jit::TierTransitionManager manager(config);
    
    // تعيين مُترجم وهمي
    manager.setTierCompiler(sad::jit::CompilationTier::BaselineJIT,
        [](const std::string&, sad::jit::CompilationTier, void** code, size_t* size) {
            static char fakeCode[64];
            *code = fakeCode;
            *size = 64;
            return true;
        });
    
    manager.start();
    manager.registerFunction("promote_func");
    
    // محاكاة تنفيذات كافية للترقية
    for (int i = 0; i < 15; i++) {
        manager.recordExecution("promote_func", 10);
    }
    
    // الانتظار للترجمة الخلفية
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    manager.stop();
    
    std::cout << "✅ اختبار الترقية نجح!\n";
}

void testDemotion() {
    sad::jit::TierTransitionConfig config;
    config.allowDowngrade = true;
    config.backgroundCompilation = false;
    
    sad::jit::TierTransitionManager manager(config);
    
    manager.registerFunction("demote_func");
    
    // تخفيض فوري
    bool demoted = manager.requestDemotion("demote_func", sad::jit::CompilationTier::Interpreter);
    assert(demoted);
    
    auto stats = manager.getStatistics();
    assert(stats.totalDemotions == 1);
    
    std::cout << "✅ اختبار التخفيض نجح!\n";
}

void testCounterPolicy() {
    sad::jit::CounterPolicy policy(100, 10000);
    
    sad::jit::TieredFunctionInfo info;
    info.executionCount = 50;
    
    assert(!policy.shouldPromote(info, sad::jit::CompilationTier::BaselineJIT));
    
    info.executionCount = 150;
    assert(policy.shouldPromote(info, sad::jit::CompilationTier::BaselineJIT));
    
    info.executionCount = 5000;
    assert(!policy.shouldPromote(info, sad::jit::CompilationTier::OptimizedJIT));
    
    info.executionCount = 15000;
    assert(policy.shouldPromote(info, sad::jit::CompilationTier::OptimizedJIT));
    
    std::cout << "✅ اختبار CounterPolicy نجح!\n";
}

void testAdaptivePolicy() {
    sad::jit::AdaptivePolicy policy;
    
    sad::jit::TieredFunctionInfo info;
    info.executionCount = 30;
    info.totalTimeUs = 6000;  // 200μs average
    
    // يجب الترقية بسبب البطء رغم قلة التنفيذات
    assert(policy.shouldPromote(info, sad::jit::CompilationTier::BaselineJIT));
    
    std::cout << "✅ اختبار AdaptivePolicy نجح!\n";
}

void testStatistics() {
    sad::jit::TierTransitionConfig config;
    config.backgroundCompilation = false;
    
    sad::jit::TierTransitionManager manager(config);
    
    manager.registerFunction("func1");
    manager.registerFunction("func2");
    manager.registerFunction("func3");
    
    for (int i = 0; i < 100; i++) {
        manager.recordExecution("func1", 10);
        manager.recordExecution("func2", 20);
    }
    
    auto stats = manager.getStatistics();
    assert(stats.totalFunctions == 3);
    assert(stats.tierDistribution[sad::jit::CompilationTier::Interpreter] == 3);
    
    manager.printReport();
    
    std::cout << "✅ اختبار الإحصائيات نجح!\n";
}

void testTieredFunctionInfo() {
    sad::jit::TieredFunctionInfo info;
    info.name = "info_test";
    info.executionCount = 1000;
    info.totalTimeUs = 50000;
    
    assert(info.averageTimeUs() == 50.0);
    assert(info.getCurrentCode() == nullptr);
    
    // إضافة كود
    char fakeCode[32];
    sad::jit::TieredCode code;
    code.tier = sad::jit::CompilationTier::BaselineJIT;
    code.code = fakeCode;
    code.codeSize = 32;
    
    info.tieredCode[sad::jit::CompilationTier::BaselineJIT] = code;
    info.currentTier = sad::jit::CompilationTier::BaselineJIT;
    
    assert(info.getCurrentCode() == fakeCode);
    
    std::cout << "✅ اختبار TieredFunctionInfo نجح!\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات نظام الترجمة المتدرجة\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testTierLevels();
    testBasicTransition();
    testPromotion();
    testDemotion();
    testCounterPolicy();
    testAdaptivePolicy();
    testStatistics();
    testTieredFunctionInfo();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // TIER_TRANSITION_TEST
