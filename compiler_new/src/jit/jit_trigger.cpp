/**
 * =============================================================================
 * ملف: jit_trigger.cpp
 * الوصف: مُفعّل الترجمة JIT
 * المهمة: T211 - Implement JIT compilation trigger
 * المرحلة: Phase 21 - User Story 18 (JIT & Tiered Compilation)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما هو JIT Trigger؟
 * ──────────────────
 * نظام يقرر متى يُترجم الكود إلى native code.
 * 
 * متى نُفعّل الترجمة؟
 * ───────────────────
 * 1. **عداد التنفيذ**: بعد N تنفيذات
 * 2. **وقت التنفيذ**: إذا استهلك الكود وقتاً طويلاً
 * 3. **الحلقات الساخنة**: حلقات تُنفذ آلاف المرات
 * 4. **طلب صريح**: المبرمج يطلب الترجمة
 * 
 * استراتيجيات الترجمة:
 * ────────────────────
 * ```
 * 1. فوري: ترجمة عند أول استدعاء
 * 2. عتبة: ترجمة بعد N تنفيذات
 * 3. تكيفي: تعديل العتبة حسب السلوك
 * ```
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
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
// سبب التفعيل (Trigger Reason)
// =============================================================================

/**
 * أسباب تفعيل ترجمة JIT
 */
enum class TriggerReason {
    ExecutionCount,     // عداد التنفيذ
    TimeThreshold,      // عتبة الوقت
    HotLoop,            // حلقة ساخنة
    ExplicitRequest,    // طلب صريح
    OSRTrigger,         // On-Stack Replacement
    BackgroundProfile,  // تحليل خلفي
};

inline const char* triggerReasonName(TriggerReason reason) {
    switch (reason) {
        case TriggerReason::ExecutionCount: return "عداد_التنفيذ";
        case TriggerReason::TimeThreshold: return "عتبة_الوقت";
        case TriggerReason::HotLoop: return "حلقة_ساخنة";
        case TriggerReason::ExplicitRequest: return "طلب_صريح";
        case TriggerReason::OSRTrigger: return "OSR";
        case TriggerReason::BackgroundProfile: return "تحليل_خلفي";
    }
    return "مجهول";
}

// =============================================================================
// حالة الترجمة (Compilation State)
// =============================================================================

/**
 * حالة ترجمة دالة
 */
enum class CompilationState {
    NotCompiled,        // لم تُترجم
    Queued,             // في الانتظار
    Compiling,          // جاري الترجمة
    Compiled,           // مُترجمة
    Failed,             // فشلت
    Invalidated,        // أُبطلت
};

inline const char* compilationStateName(CompilationState state) {
    switch (state) {
        case CompilationState::NotCompiled: return "لم_تُترجم";
        case CompilationState::Queued: return "في_الانتظار";
        case CompilationState::Compiling: return "جاري_الترجمة";
        case CompilationState::Compiled: return "مُترجمة";
        case CompilationState::Failed: return "فشلت";
        case CompilationState::Invalidated: return "أُبطلت";
    }
    return "مجهول";
}

// =============================================================================
// إعدادات التفعيل (Trigger Config)
// =============================================================================

/**
 * إعدادات مُفعّل JIT
 */
struct JITTriggerConfig {
    // عتبات التنفيذ
    uint64_t executionThreshold = 1000;     // عتبة عداد التنفيذ
    uint64_t loopIterationThreshold = 100;  // عتبة تكرارات الحلقة
    
    // عتبات الوقت (بالميكروثانية)
    uint64_t timeThresholdUs = 10000;       // عتبة الوقت الكلي
    uint64_t avgTimeThresholdUs = 100;      // عتبة متوسط الوقت
    
    // إعدادات السلوك
    bool backgroundCompilation = true;       // ترجمة خلفية
    bool adaptiveThresholds = true;          // عتبات تكيفية
    int maxCompilationQueueSize = 100;       // حجم صف الترجمة
    int compilationThreads = 2;              // عدد خيوط الترجمة
    
    // OSR
    bool enableOSR = true;                   // تمكين OSR
    int osrCheckInterval = 1000;             // فحص OSR كل N تكرار
};

// =============================================================================
// معلومات الدالة (Function Info)
// =============================================================================

/**
 * معلومات دالة للترجمة
 */
struct FunctionCompilationInfo {
    std::string name;
    uint64_t executionCount = 0;
    uint64_t totalTimeUs = 0;
    CompilationState state = CompilationState::NotCompiled;
    TriggerReason lastTriggerReason;
    void* compiledCode = nullptr;
    size_t compiledCodeSize = 0;
    std::chrono::steady_clock::time_point lastExecution;
    std::chrono::steady_clock::time_point compilationTime;
    int compilationAttempts = 0;
    bool hasHotLoops = false;
    
    double averageTimeUs() const {
        return executionCount > 0 ? 
            static_cast<double>(totalTimeUs) / executionCount : 0.0;
    }
};

// =============================================================================
// طلب ترجمة (Compilation Request)
// =============================================================================

/**
 * طلب ترجمة في الصف
 */
struct CompilationRequest {
    std::string functionName;
    TriggerReason reason;
    int priority;
    std::chrono::steady_clock::time_point requestTime;
    
    bool operator<(const CompilationRequest& other) const {
        return priority < other.priority;  // أولوية أعلى = قيمة أكبر
    }
};

// =============================================================================
// مُفعّل الترجمة (JIT Trigger)
// =============================================================================

/**
 * مُفعّل ترجمة JIT
 */
class JITTrigger {
public:
    using CompilationCallback = std::function<bool(const std::string&, void**, size_t*)>;
    
    explicit JITTrigger(const JITTriggerConfig& config = JITTriggerConfig())
        : config_(config), running_(false) {}
    
    ~JITTrigger() {
        stop();
    }
    
    /**
     * تعيين callback الترجمة
     */
    void setCompilationCallback(CompilationCallback callback) {
        compilationCallback_ = callback;
    }
    
    /**
     * بدء نظام الترجمة الخلفي
     */
    void start() {
        if (running_) return;
        running_ = true;
        
        if (config_.backgroundCompilation) {
            for (int i = 0; i < config_.compilationThreads; i++) {
                compilationThreads_.emplace_back(&JITTrigger::compilationWorker, this);
            }
        }
    }
    
    /**
     * إيقاف نظام الترجمة
     */
    void stop() {
        running_ = false;
        queueCondition_.notify_all();
        
        for (auto& thread : compilationThreads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        compilationThreads_.clear();
    }
    
    /**
     * تسجيل دالة
     */
    void registerFunction(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (functions_.find(name) == functions_.end()) {
            functions_[name] = FunctionCompilationInfo{};
            functions_[name].name = name;
        }
    }
    
    /**
     * تسجيل تنفيذ
     */
    bool recordExecution(const std::string& name, uint64_t executionTimeUs) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = functions_.find(name);
        if (it == functions_.end()) {
            registerFunction(name);
            it = functions_.find(name);
        }
        
        auto& info = it->second;
        info.executionCount++;
        info.totalTimeUs += executionTimeUs;
        info.lastExecution = std::chrono::steady_clock::now();
        
        // فحص إذا كانت مُترجمة بالفعل
        if (info.state == CompilationState::Compiled) {
            return true;  // استخدم الكود المُترجم
        }
        
        // فحص عتبات التفعيل
        return checkTriggers(name, info);
    }
    
    /**
     * تسجيل حلقة ساخنة
     */
    bool recordHotLoop(const std::string& functionName, const std::string& loopId, 
                       uint64_t iterations) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = functions_.find(functionName);
        if (it == functions_.end()) return false;
        
        if (iterations >= config_.loopIterationThreshold) {
            it->second.hasHotLoops = true;
            
            if (it->second.state == CompilationState::NotCompiled) {
                return triggerCompilation(functionName, TriggerReason::HotLoop);
            }
        }
        
        return false;
    }
    
    /**
     * طلب ترجمة صريح
     */
    bool requestCompilation(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        return triggerCompilation(name, TriggerReason::ExplicitRequest);
    }
    
    /**
     * الحصول على الكود المُترجم
     */
    void* getCompiledCode(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = functions_.find(name);
        if (it != functions_.end() && it->second.state == CompilationState::Compiled) {
            return it->second.compiledCode;
        }
        return nullptr;
    }
    
    /**
     * هل الدالة مُترجمة؟
     */
    bool isCompiled(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = functions_.find(name);
        return it != functions_.end() && 
               it->second.state == CompilationState::Compiled;
    }
    
    /**
     * الحصول على معلومات الدالة
     */
    std::optional<FunctionCompilationInfo> getFunctionInfo(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = functions_.find(name);
        if (it != functions_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    /**
     * إحصائيات
     */
    struct Statistics {
        size_t totalFunctions = 0;
        size_t compiledFunctions = 0;
        size_t queuedFunctions = 0;
        size_t failedFunctions = 0;
        uint64_t totalCompilations = 0;
        uint64_t successfulCompilations = 0;
    };
    
    Statistics getStatistics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        Statistics stats;
        stats.totalFunctions = functions_.size();
        
        for (const auto& [name, info] : functions_) {
            switch (info.state) {
                case CompilationState::Compiled:
                    stats.compiledFunctions++;
                    break;
                case CompilationState::Queued:
                case CompilationState::Compiling:
                    stats.queuedFunctions++;
                    break;
                case CompilationState::Failed:
                    stats.failedFunctions++;
                    break;
                default:
                    break;
            }
        }
        
        stats.totalCompilations = totalCompilations_;
        stats.successfulCompilations = successfulCompilations_;
        
        return stats;
    }
    
    /**
     * طباعة تقرير
     */
    void printReport() const {
        auto stats = getStatistics();
        
        std::cout << "\n═══════════════════════════════════════════════\n";
        std::cout << "   تقرير مُفعّل JIT\n";
        std::cout << "═══════════════════════════════════════════════\n\n";
        
        std::cout << "إجمالي الدوال: " << stats.totalFunctions << "\n";
        std::cout << "مُترجمة: " << stats.compiledFunctions << "\n";
        std::cout << "في الانتظار: " << stats.queuedFunctions << "\n";
        std::cout << "فاشلة: " << stats.failedFunctions << "\n";
        std::cout << "\n";
        std::cout << "ترجمات كلية: " << stats.totalCompilations << "\n";
        std::cout << "ترجمات ناجحة: " << stats.successfulCompilations << "\n";
    }

private:
    JITTriggerConfig config_;
    std::unordered_map<std::string, FunctionCompilationInfo> functions_;
    std::priority_queue<CompilationRequest> compilationQueue_;
    std::vector<std::thread> compilationThreads_;
    CompilationCallback compilationCallback_;
    
    mutable std::mutex mutex_;
    std::condition_variable queueCondition_;
    std::atomic<bool> running_;
    
    uint64_t totalCompilations_ = 0;
    uint64_t successfulCompilations_ = 0;
    
    bool checkTriggers(const std::string& name, FunctionCompilationInfo& info) {
        // فحص عداد التنفيذ
        if (info.executionCount >= config_.executionThreshold) {
            return triggerCompilation(name, TriggerReason::ExecutionCount);
        }
        
        // فحص الوقت الكلي
        if (info.totalTimeUs >= config_.timeThresholdUs) {
            return triggerCompilation(name, TriggerReason::TimeThreshold);
        }
        
        // فحص متوسط الوقت (دالة بطيئة)
        if (info.executionCount > 10 && info.averageTimeUs() >= config_.avgTimeThresholdUs) {
            return triggerCompilation(name, TriggerReason::TimeThreshold);
        }
        
        return false;
    }
    
    bool triggerCompilation(const std::string& name, TriggerReason reason) {
        auto it = functions_.find(name);
        if (it == functions_.end()) return false;
        
        auto& info = it->second;
        
        // تجاهل إذا كانت مُترجمة أو في الانتظار
        if (info.state != CompilationState::NotCompiled &&
            info.state != CompilationState::Invalidated) {
            return false;
        }
        
        info.state = CompilationState::Queued;
        info.lastTriggerReason = reason;
        
        // حساب الأولوية
        int priority = calculatePriority(info, reason);
        
        // إضافة للصف
        CompilationRequest request;
        request.functionName = name;
        request.reason = reason;
        request.priority = priority;
        request.requestTime = std::chrono::steady_clock::now();
        
        compilationQueue_.push(request);
        queueCondition_.notify_one();
        
        return true;
    }
    
    int calculatePriority(const FunctionCompilationInfo& info, TriggerReason reason) {
        int priority = 0;
        
        // أولوية حسب السبب
        switch (reason) {
            case TriggerReason::ExplicitRequest: priority += 100; break;
            case TriggerReason::HotLoop: priority += 80; break;
            case TriggerReason::OSRTrigger: priority += 90; break;
            case TriggerReason::ExecutionCount: priority += 50; break;
            case TriggerReason::TimeThreshold: priority += 40; break;
            default: break;
        }
        
        // أولوية حسب التنفيذات
        priority += static_cast<int>(info.executionCount / 100);
        
        // أولوية للدوال البطيئة
        if (info.averageTimeUs() > config_.avgTimeThresholdUs) {
            priority += 30;
        }
        
        return priority;
    }
    
    void compilationWorker() {
        while (running_) {
            CompilationRequest request;
            
            {
                std::unique_lock<std::mutex> lock(mutex_);
                queueCondition_.wait(lock, [this] {
                    return !running_ || !compilationQueue_.empty();
                });
                
                if (!running_ && compilationQueue_.empty()) break;
                
                if (compilationQueue_.empty()) continue;
                
                request = compilationQueue_.top();
                compilationQueue_.pop();
                
                auto it = functions_.find(request.functionName);
                if (it != functions_.end()) {
                    it->second.state = CompilationState::Compiling;
                }
            }
            
            // ترجمة الدالة
            compileFunction(request);
        }
    }
    
    void compileFunction(const CompilationRequest& request) {
        totalCompilations_++;
        
        bool success = false;
        void* compiledCode = nullptr;
        size_t codeSize = 0;
        
        if (compilationCallback_) {
            success = compilationCallback_(request.functionName, &compiledCode, &codeSize);
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = functions_.find(request.functionName);
        if (it != functions_.end()) {
            if (success && compiledCode) {
                it->second.state = CompilationState::Compiled;
                it->second.compiledCode = compiledCode;
                it->second.compiledCodeSize = codeSize;
                it->second.compilationTime = std::chrono::steady_clock::now();
                successfulCompilations_++;
            } else {
                it->second.state = CompilationState::Failed;
                it->second.compilationAttempts++;
            }
        }
    }
};

// =============================================================================
// مفعّل OSR (On-Stack Replacement)
// =============================================================================

/**
 * نقطة OSR
 */
struct OSRPoint {
    std::string functionName;
    int bytecodeOffset;
    int loopDepth;
    uint64_t iterationCount = 0;
};

/**
 * مُفعّل OSR
 * يكتشف متى نستبدل الكود أثناء التنفيذ
 */
class OSRTrigger {
public:
    explicit OSRTrigger(JITTrigger& jitTrigger, int checkInterval = 1000)
        : jitTrigger_(jitTrigger), checkInterval_(checkInterval) {}
    
    /**
     * تسجيل نقطة OSR
     */
    void registerOSRPoint(const std::string& function, int bytecodeOffset, int loopDepth) {
        std::string key = function + ":" + std::to_string(bytecodeOffset);
        osrPoints_[key] = {function, bytecodeOffset, loopDepth, 0};
    }
    
    /**
     * تسجيل تكرار حلقة
     * يُرجع true إذا يجب الانتقال لـ JIT
     */
    bool recordLoopIteration(const std::string& function, int bytecodeOffset) {
        std::string key = function + ":" + std::to_string(bytecodeOffset);
        
        auto it = osrPoints_.find(key);
        if (it == osrPoints_.end()) return false;
        
        it->second.iterationCount++;
        
        // فحص كل N تكرار
        if (it->second.iterationCount % checkInterval_ == 0) {
            return checkOSRTrigger(it->second);
        }
        
        return false;
    }
    
    /**
     * الحصول على عدد التكرارات
     */
    uint64_t getIterationCount(const std::string& function, int bytecodeOffset) const {
        std::string key = function + ":" + std::to_string(bytecodeOffset);
        auto it = osrPoints_.find(key);
        return it != osrPoints_.end() ? it->second.iterationCount : 0;
    }
    
private:
    JITTrigger& jitTrigger_;
    int checkInterval_;
    std::unordered_map<std::string, OSRPoint> osrPoints_;
    
    bool checkOSRTrigger(const OSRPoint& point) {
        // تفعيل إذا كانت الدالة غير مُترجمة
        if (!jitTrigger_.isCompiled(point.functionName)) {
            return jitTrigger_.requestCompilation(point.functionName);
        }
        return true;  // استخدم الكود المُترجم
    }
};

} // namespace sad::jit

// =============================================================================
// واجهة C للتكامل
// =============================================================================

extern "C" {

using namespace sad::jit;

void* sad_jit_trigger_new() {
    return new JITTrigger();
}

void sad_jit_trigger_free(void* trigger) {
    delete static_cast<JITTrigger*>(trigger);
}

void sad_jit_trigger_start(void* trigger) {
    if (trigger) static_cast<JITTrigger*>(trigger)->start();
}

void sad_jit_trigger_stop(void* trigger) {
    if (trigger) static_cast<JITTrigger*>(trigger)->stop();
}

void sad_jit_register_function(void* trigger, const char* name) {
    if (trigger && name) {
        static_cast<JITTrigger*>(trigger)->registerFunction(name);
    }
}

int sad_jit_record_execution(void* trigger, const char* name, uint64_t time_us) {
    if (!trigger || !name) return 0;
    return static_cast<JITTrigger*>(trigger)->recordExecution(name, time_us) ? 1 : 0;
}

int sad_jit_is_compiled(void* trigger, const char* name) {
    if (!trigger || !name) return 0;
    return static_cast<JITTrigger*>(trigger)->isCompiled(name) ? 1 : 0;
}

void* sad_jit_get_compiled_code(void* trigger, const char* name) {
    if (!trigger || !name) return nullptr;
    return static_cast<JITTrigger*>(trigger)->getCompiledCode(name);
}

void sad_jit_print_report(void* trigger) {
    if (trigger) static_cast<JITTrigger*>(trigger)->printReport();
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef JIT_TRIGGER_TEST

#include <cassert>

void testBasicTrigger() {
    sad::jit::JITTriggerConfig config;
    config.executionThreshold = 100;
    config.backgroundCompilation = false;
    
    sad::jit::JITTrigger trigger(config);
    
    trigger.registerFunction("test_func");
    
    // محاكاة تنفيذات
    for (int i = 0; i < 50; i++) {
        trigger.recordExecution("test_func", 10);
    }
    
    auto info = trigger.getFunctionInfo("test_func");
    assert(info.has_value());
    assert(info->executionCount == 50);
    assert(info->state == sad::jit::CompilationState::NotCompiled);
    
    // المزيد من التنفيذات لتجاوز العتبة
    for (int i = 0; i < 60; i++) {
        trigger.recordExecution("test_func", 10);
    }
    
    info = trigger.getFunctionInfo("test_func");
    assert(info->executionCount == 110);
    assert(info->state == sad::jit::CompilationState::Queued);
    
    std::cout << "✅ اختبار التفعيل الأساسي نجح!\n";
}

void testExplicitRequest() {
    sad::jit::JITTriggerConfig config;
    config.backgroundCompilation = false;
    
    sad::jit::JITTrigger trigger(config);
    
    trigger.registerFunction("manual_compile");
    
    bool triggered = trigger.requestCompilation("manual_compile");
    assert(triggered);
    
    auto info = trigger.getFunctionInfo("manual_compile");
    assert(info.has_value());
    assert(info->state == sad::jit::CompilationState::Queued);
    assert(info->lastTriggerReason == sad::jit::TriggerReason::ExplicitRequest);
    
    std::cout << "✅ اختبار الطلب الصريح نجح!\n";
}

void testHotLoop() {
    sad::jit::JITTriggerConfig config;
    config.loopIterationThreshold = 50;
    config.backgroundCompilation = false;
    
    sad::jit::JITTrigger trigger(config);
    
    trigger.registerFunction("loop_func");
    
    // حلقة باردة
    trigger.recordHotLoop("loop_func", "loop_1", 30);
    auto info = trigger.getFunctionInfo("loop_func");
    assert(info->state == sad::jit::CompilationState::NotCompiled);
    
    // حلقة ساخنة
    trigger.recordHotLoop("loop_func", "loop_1", 100);
    info = trigger.getFunctionInfo("loop_func");
    assert(info->state == sad::jit::CompilationState::Queued);
    assert(info->lastTriggerReason == sad::jit::TriggerReason::HotLoop);
    
    std::cout << "✅ اختبار الحلقة الساخنة نجح!\n";
}

void testTimeThreshold() {
    sad::jit::JITTriggerConfig config;
    config.executionThreshold = 1000000;  // عالي جداً
    config.timeThresholdUs = 5000;        // 5ms
    config.backgroundCompilation = false;
    
    sad::jit::JITTrigger trigger(config);
    
    trigger.registerFunction("slow_func");
    
    // دالة بطيئة
    for (int i = 0; i < 10; i++) {
        trigger.recordExecution("slow_func", 600);  // 600μs لكل تنفيذ
    }
    
    auto info = trigger.getFunctionInfo("slow_func");
    assert(info->totalTimeUs == 6000);
    assert(info->state == sad::jit::CompilationState::Queued);
    
    std::cout << "✅ اختبار عتبة الوقت نجح!\n";
}

void testOSRTrigger() {
    sad::jit::JITTriggerConfig config;
    config.backgroundCompilation = false;
    
    sad::jit::JITTrigger jitTrigger(config);
    sad::jit::OSRTrigger osrTrigger(jitTrigger, 100);
    
    jitTrigger.registerFunction("osr_func");
    osrTrigger.registerOSRPoint("osr_func", 42, 1);
    
    // محاكاة تكرارات الحلقة
    for (int i = 0; i < 150; i++) {
        osrTrigger.recordLoopIteration("osr_func", 42);
    }
    
    assert(osrTrigger.getIterationCount("osr_func", 42) == 150);
    
    std::cout << "✅ اختبار OSR نجح!\n";
}

void testStatistics() {
    sad::jit::JITTriggerConfig config;
    config.executionThreshold = 10;
    config.backgroundCompilation = false;
    
    sad::jit::JITTrigger trigger(config);
    
    trigger.registerFunction("func1");
    trigger.registerFunction("func2");
    trigger.registerFunction("func3");
    
    for (int i = 0; i < 20; i++) {
        trigger.recordExecution("func1", 10);
        trigger.recordExecution("func2", 10);
    }
    
    auto stats = trigger.getStatistics();
    assert(stats.totalFunctions == 3);
    assert(stats.queuedFunctions == 2);
    
    trigger.printReport();
    
    std::cout << "✅ اختبار الإحصائيات نجح!\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات مُفعّل JIT\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testBasicTrigger();
    testExplicitRequest();
    testHotLoop();
    testTimeThreshold();
    testOSRTrigger();
    testStatistics();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // JIT_TRIGGER_TEST
