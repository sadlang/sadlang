/**
 * =============================================================================
 * ملف: osr.cpp
 * الوصف: استبدال على المكدس (On-Stack Replacement)
 * المهمة: T212 - Implement on-stack replacement (OSR)
 * المرحلة: Phase 21 - User Story 18 (JIT & Tiered Compilation)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما هو OSR (On-Stack Replacement)؟
 * ──────────────────────────────────
 * تقنية للانتقال من كود مُفسَّر إلى كود مُترجم أثناء التنفيذ!
 * 
 * لماذا نحتاج OSR؟
 * ────────────────
 * تخيل دالة بها حلقة طويلة جداً:
 * ```
 * دالة حساب_ثقيل()
 *     لكل ن من ١ إلى ١٠٠٠٠٠٠    // مليون تكرار!
 *         // عمليات معقدة
 *     نهاية
 * نهاية
 * ```
 * 
 * بدون OSR: ننتظر حتى تنتهي الدالة ثم نُترجمها
 * مع OSR: نُترجم ونستبدل أثناء تنفيذ الحلقة!
 * 
 * كيف يعمل OSR؟
 * ──────────────
 * 1. اكتشاف حلقة ساخنة
 * 2. ترجمة الكود JIT
 * 3. حفظ حالة المتغيرات
 * 4. الانتقال للكود المُترجم
 * 5. استمرار التنفيذ بسرعة!
 * 
 * ```
 *                 ┌─────────────┐
 *                 │ كود مُفسَّر │
 *                 │    ↓        │
 *                 │  حلقة      │──→ OSR Point
 *                 │    ↓        │         │
 *                 └─────────────┘         │
 *                                         ↓
 *                 ┌─────────────┐   ┌─────────────┐
 *                 │ حالة VM    │──→│ كود JIT    │
 *                 └─────────────┘   └─────────────┘
 * ```
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
#include <variant>
#include <cassert>
#include <iostream>

namespace sad::jit {

// =============================================================================
// أنواع القيم
// =============================================================================

/**
 * نوع قيمة في إطار المكدس
 */
enum class StackValueType {
    Integer,        // صحيح
    Float,          // عشري
    Boolean,        // منطقي
    Reference,      // مرجع
    Object,         // كائن
    Array,          // مصفوفة
    Nil,            // لا شيء
};

/**
 * قيمة في إطار المكدس
 */
struct StackValue {
    StackValueType type;
    std::variant<
        int64_t,            // Integer
        double,             // Float
        bool,               // Boolean
        void*,              // Reference, Object, Array
        std::monostate      // Nil
    > data;
    
    static StackValue integer(int64_t v) {
        return {StackValueType::Integer, v};
    }
    static StackValue floating(double v) {
        return {StackValueType::Float, v};
    }
    static StackValue boolean(bool v) {
        return {StackValueType::Boolean, v};
    }
    static StackValue reference(void* v) {
        return {StackValueType::Reference, v};
    }
    static StackValue nil() {
        return {StackValueType::Nil, std::monostate{}};
    }
    
    int64_t asInt() const { return std::get<int64_t>(data); }
    double asFloat() const { return std::get<double>(data); }
    bool asBool() const { return std::get<bool>(data); }
    void* asRef() const { return std::get<void*>(data); }
};

// =============================================================================
// إطار المكدس (Stack Frame)
// =============================================================================

/**
 * معلومات إطار دالة
 */
struct FrameInfo {
    std::string functionName;
    int bytecodeOffset;         // موقع في bytecode
    int returnAddress;          // عنوان العودة
    std::vector<StackValue> locals;      // المتغيرات المحلية
    std::vector<StackValue> operands;    // مكدس العمليات
    void* framePointer;         // مؤشر الإطار
};

/**
 * حالة VM الكاملة
 */
struct VMState {
    std::vector<FrameInfo> callStack;    // مكدس الاستدعاءات
    std::vector<StackValue> globalVars;  // المتغيرات العامة
    int currentFrame = 0;                 // الإطار الحالي
    
    FrameInfo& currentFrameInfo() {
        return callStack[currentFrame];
    }
    
    const FrameInfo& currentFrameInfo() const {
        return callStack[currentFrame];
    }
};

// =============================================================================
// نقطة OSR (OSR Entry Point)
// =============================================================================

/**
 * معلومات نقطة OSR
 */
struct OSREntryPoint {
    std::string functionName;
    int bytecodeOffset;          // موقع في bytecode
    int nativeOffset;            // موقع في الكود المُترجم
    
    // خريطة المتغيرات: موقع VM → موقع في JIT
    struct VariableMapping {
        int vmSlot;              // slot في VM
        int jitRegister;         // register أو slot في JIT
        StackValueType type;
    };
    std::vector<VariableMapping> variableMappings;
    
    // إعدادات الإطار
    int frameSize = 0;
    int savedRegistersCount = 0;
};

// =============================================================================
// مُنشئ نقاط OSR (OSR Point Builder)
// =============================================================================

/**
 * مُنشئ نقاط OSR
 * ينشئ نقاط الدخول للكود المُترجم
 */
class OSRPointBuilder {
public:
    /**
     * بدء بناء نقطة OSR
     */
    void startPoint(const std::string& function, int bytecodeOffset) {
        currentPoint_ = OSREntryPoint{};
        currentPoint_.functionName = function;
        currentPoint_.bytecodeOffset = bytecodeOffset;
    }
    
    /**
     * تعيين موقع الكود المُترجم
     */
    void setNativeOffset(int offset) {
        currentPoint_.nativeOffset = offset;
    }
    
    /**
     * إضافة تعيين متغير
     */
    void addVariableMapping(int vmSlot, int jitRegister, StackValueType type) {
        currentPoint_.variableMappings.push_back({vmSlot, jitRegister, type});
    }
    
    /**
     * تعيين حجم الإطار
     */
    void setFrameSize(int size) {
        currentPoint_.frameSize = size;
    }
    
    /**
     * إنهاء وإرجاع النقطة
     */
    OSREntryPoint build() {
        return currentPoint_;
    }
    
private:
    OSREntryPoint currentPoint_;
};

// =============================================================================
// مدير OSR (OSR Manager)
// =============================================================================

/**
 * نتيجة انتقال OSR
 */
struct OSRTransitionResult {
    bool success = false;
    std::string errorMessage;
    void* targetAddress = nullptr;  // عنوان الكود المُترجم
    uint64_t transitionTime = 0;    // وقت الانتقال (μs)
};

/**
 * callback لتنفيذ الكود المُترجم
 */
using JITExecutor = std::function<void(void* entryPoint, void* frameData, size_t frameSize)>;

/**
 * مدير OSR
 * يدير نقاط OSR والانتقالات
 */
class OSRManager {
public:
    OSRManager() = default;
    
    /**
     * تسجيل نقطة OSR
     */
    void registerOSRPoint(const OSREntryPoint& point) {
        std::string key = makeKey(point.functionName, point.bytecodeOffset);
        osrPoints_[key] = point;
    }
    
    /**
     * البحث عن نقطة OSR
     */
    std::optional<OSREntryPoint> findOSRPoint(const std::string& function, 
                                               int bytecodeOffset) const {
        std::string key = makeKey(function, bytecodeOffset);
        auto it = osrPoints_.find(key);
        if (it != osrPoints_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    /**
     * هل توجد نقطة OSR؟
     */
    bool hasOSRPoint(const std::string& function, int bytecodeOffset) const {
        std::string key = makeKey(function, bytecodeOffset);
        return osrPoints_.find(key) != osrPoints_.end();
    }
    
    /**
     * تنفيذ انتقال OSR
     */
    OSRTransitionResult performTransition(const std::string& function,
                                           int bytecodeOffset,
                                           const VMState& vmState,
                                           void* compiledCode) {
        OSRTransitionResult result;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // البحث عن نقطة OSR
        auto point = findOSRPoint(function, bytecodeOffset);
        if (!point) {
            result.success = false;
            result.errorMessage = "لم يُعثر على نقطة OSR";
            return result;
        }
        
        // إنشاء إطار JIT
        std::vector<uint8_t> frameData;
        if (!buildJITFrame(*point, vmState, frameData)) {
            result.success = false;
            result.errorMessage = "فشل بناء إطار JIT";
            return result;
        }
        
        // حساب عنوان الدخول
        result.targetAddress = static_cast<uint8_t*>(compiledCode) + point->nativeOffset;
        result.success = true;
        
        // تسجيل الإحصائيات
        auto endTime = std::chrono::high_resolution_clock::now();
        result.transitionTime = std::chrono::duration_cast<std::chrono::microseconds>(
            endTime - startTime
        ).count();
        
        totalTransitions_++;
        successfulTransitions_++;
        
        return result;
    }
    
    /**
     * إحصائيات
     */
    struct Statistics {
        size_t totalOSRPoints = 0;
        uint64_t totalTransitions = 0;
        uint64_t successfulTransitions = 0;
        uint64_t failedTransitions = 0;
    };
    
    Statistics getStatistics() const {
        Statistics stats;
        stats.totalOSRPoints = osrPoints_.size();
        stats.totalTransitions = totalTransitions_;
        stats.successfulTransitions = successfulTransitions_;
        stats.failedTransitions = totalTransitions_ - successfulTransitions_;
        return stats;
    }
    
    /**
     * طباعة تقرير
     */
    void printReport() const {
        auto stats = getStatistics();
        
        std::cout << "\n═══════════════════════════════════════════════\n";
        std::cout << "   تقرير OSR\n";
        std::cout << "═══════════════════════════════════════════════\n\n";
        
        std::cout << "نقاط OSR المسجلة: " << stats.totalOSRPoints << "\n";
        std::cout << "انتقالات كلية: " << stats.totalTransitions << "\n";
        std::cout << "انتقالات ناجحة: " << stats.successfulTransitions << "\n";
        std::cout << "انتقالات فاشلة: " << stats.failedTransitions << "\n";
        
        if (stats.totalTransitions > 0) {
            double successRate = 100.0 * stats.successfulTransitions / stats.totalTransitions;
            std::cout << "نسبة النجاح: " << successRate << "%\n";
        }
    }

private:
    std::unordered_map<std::string, OSREntryPoint> osrPoints_;
    uint64_t totalTransitions_ = 0;
    uint64_t successfulTransitions_ = 0;
    
    std::string makeKey(const std::string& function, int offset) const {
        return function + ":" + std::to_string(offset);
    }
    
    bool buildJITFrame(const OSREntryPoint& point,
                       const VMState& vmState,
                       std::vector<uint8_t>& frameData) {
        // تخصيص مساحة للإطار
        frameData.resize(point.frameSize);
        
        const auto& vmFrame = vmState.currentFrameInfo();
        
        // نسخ المتغيرات حسب التعيين
        for (const auto& mapping : point.variableMappings) {
            if (mapping.vmSlot >= static_cast<int>(vmFrame.locals.size())) {
                return false;  // slot غير صالح
            }
            
            const auto& value = vmFrame.locals[mapping.vmSlot];
            
            // كتابة القيمة في موقعها الجديد
            if (!writeValueToFrame(frameData, mapping.jitRegister, value)) {
                return false;
            }
        }
        
        return true;
    }
    
    bool writeValueToFrame(std::vector<uint8_t>& frame, int offset, 
                          const StackValue& value) {
        if (offset < 0 || offset + 8 > static_cast<int>(frame.size())) {
            return false;
        }
        
        switch (value.type) {
            case StackValueType::Integer: {
                int64_t v = value.asInt();
                std::memcpy(&frame[offset], &v, sizeof(v));
                break;
            }
            case StackValueType::Float: {
                double v = value.asFloat();
                std::memcpy(&frame[offset], &v, sizeof(v));
                break;
            }
            case StackValueType::Boolean: {
                int64_t v = value.asBool() ? 1 : 0;
                std::memcpy(&frame[offset], &v, sizeof(v));
                break;
            }
            case StackValueType::Reference:
            case StackValueType::Object:
            case StackValueType::Array: {
                void* v = value.asRef();
                std::memcpy(&frame[offset], &v, sizeof(v));
                break;
            }
            case StackValueType::Nil: {
                int64_t v = 0;
                std::memcpy(&frame[offset], &v, sizeof(v));
                break;
            }
        }
        
        return true;
    }
};

// =============================================================================
// مُولّد كود OSR (OSR Code Generator)
// =============================================================================

/**
 * مُولّد stub لـ OSR
 * ينشئ الكود المسؤول عن الانتقال
 */
class OSRStubGenerator {
public:
    /**
     * توليد stub للدخول
     */
    std::vector<uint8_t> generateEntryStub(const OSREntryPoint& point) {
        std::vector<uint8_t> code;
        
        // هذا مثال مبسط - في الواقع نحتاج كود آلة حقيقي
        // prologue: حفظ السجلات
        code.push_back(0x55);  // push rbp (x86-64)
        code.push_back(0x48); code.push_back(0x89); code.push_back(0xe5);  // mov rbp, rsp
        
        // تخصيص مساحة المكدس
        code.push_back(0x48); code.push_back(0x83); code.push_back(0xec);
        code.push_back(static_cast<uint8_t>(point.frameSize));
        
        // TODO: نسخ المتغيرات من إطار VM
        
        return code;
    }
    
    /**
     * توليد stub للخروج
     */
    std::vector<uint8_t> generateExitStub(const OSREntryPoint& point) {
        std::vector<uint8_t> code;
        
        // TODO: نسخ المتغيرات إلى إطار VM
        
        // epilogue
        code.push_back(0x48); code.push_back(0x89); code.push_back(0xec);  // mov rsp, rbp
        code.push_back(0x5d);  // pop rbp
        code.push_back(0xc3);  // ret
        
        return code;
    }
};

// =============================================================================
// فاحص OSR (OSR Checker)
// =============================================================================

/**
 * فاحص نقاط OSR
 * يُدرج في الحلقات لفحص إمكانية OSR
 */
class OSRChecker {
public:
    explicit OSRChecker(OSRManager& manager, int checkInterval = 1000)
        : manager_(manager), checkInterval_(checkInterval) {}
    
    /**
     * فحص عند تكرار حلقة
     * يُرجع true إذا يجب الانتقال لـ OSR
     */
    bool checkAtLoopBack(const std::string& function, int bytecodeOffset) {
        std::string key = function + ":" + std::to_string(bytecodeOffset);
        
        uint64_t& count = iterationCounts_[key];
        count++;
        
        // فحص كل N تكرار
        if (count % checkInterval_ == 0) {
            return manager_.hasOSRPoint(function, bytecodeOffset);
        }
        
        return false;
    }
    
    /**
     * إعادة تعيين العدادات
     */
    void reset() {
        iterationCounts_.clear();
    }
    
private:
    OSRManager& manager_;
    int checkInterval_;
    std::unordered_map<std::string, uint64_t> iterationCounts_;
};

// =============================================================================
// نظام OSR الكامل (Complete OSR System)
// =============================================================================

/**
 * نظام OSR الكامل
 * يجمع كل المكونات
 */
class OSRSystem {
public:
    OSRSystem() : checker_(manager_) {}
    
    /**
     * تسجيل نقطة OSR لدالة
     */
    void registerFunction(const std::string& function, 
                         const std::vector<OSREntryPoint>& points) {
        for (const auto& point : points) {
            manager_.registerOSRPoint(point);
        }
        functionPoints_[function] = points;
    }
    
    /**
     * فحص إمكانية OSR أثناء تنفيذ حلقة
     */
    bool checkLoop(const std::string& function, int bytecodeOffset,
                   const VMState& vmState, void* compiledCode,
                   OSRTransitionResult& result) {
        if (!checker_.checkAtLoopBack(function, bytecodeOffset)) {
            return false;  // ليس وقت الفحص
        }
        
        if (!manager_.hasOSRPoint(function, bytecodeOffset)) {
            return false;  // لا توجد نقطة OSR
        }
        
        // تنفيذ الانتقال
        result = manager_.performTransition(function, bytecodeOffset, 
                                            vmState, compiledCode);
        return result.success;
    }
    
    /**
     * إحصائيات
     */
    OSRManager::Statistics getStatistics() const {
        return manager_.getStatistics();
    }
    
    /**
     * طباعة تقرير
     */
    void printReport() const {
        manager_.printReport();
    }
    
private:
    OSRManager manager_;
    OSRChecker checker_;
    std::unordered_map<std::string, std::vector<OSREntryPoint>> functionPoints_;
};

} // namespace sad::jit

// =============================================================================
// واجهة C للتكامل
// =============================================================================

extern "C" {

using namespace sad::jit;

// OSR Manager
void* sad_osr_manager_new() {
    return new OSRManager();
}

void sad_osr_manager_free(void* manager) {
    delete static_cast<OSRManager*>(manager);
}

void sad_osr_register_point(void* manager, const char* function, 
                            int bytecodeOffset, int nativeOffset) {
    if (!manager || !function) return;
    
    OSREntryPoint point;
    point.functionName = function;
    point.bytecodeOffset = bytecodeOffset;
    point.nativeOffset = nativeOffset;
    
    static_cast<OSRManager*>(manager)->registerOSRPoint(point);
}

int sad_osr_has_point(void* manager, const char* function, int bytecodeOffset) {
    if (!manager || !function) return 0;
    return static_cast<OSRManager*>(manager)->hasOSRPoint(function, bytecodeOffset) ? 1 : 0;
}

void sad_osr_print_report(void* manager) {
    if (manager) {
        static_cast<OSRManager*>(manager)->printReport();
    }
}

// OSR System
void* sad_osr_system_new() {
    return new OSRSystem();
}

void sad_osr_system_free(void* system) {
    delete static_cast<OSRSystem*>(system);
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef OSR_TEST

#include <cassert>

void testStackValue() {
    auto intVal = sad::jit::StackValue::integer(42);
    assert(intVal.type == sad::jit::StackValueType::Integer);
    assert(intVal.asInt() == 42);
    
    auto floatVal = sad::jit::StackValue::floating(3.14);
    assert(floatVal.type == sad::jit::StackValueType::Float);
    assert(floatVal.asFloat() == 3.14);
    
    auto boolVal = sad::jit::StackValue::boolean(true);
    assert(boolVal.asBool() == true);
    
    auto nilVal = sad::jit::StackValue::nil();
    assert(nilVal.type == sad::jit::StackValueType::Nil);
    
    std::cout << "✅ اختبار StackValue نجح!\n";
}

void testOSRPointBuilder() {
    sad::jit::OSRPointBuilder builder;
    
    builder.startPoint("test_func", 100);
    builder.setNativeOffset(0x1000);
    builder.addVariableMapping(0, 0, sad::jit::StackValueType::Integer);  // x: slot 0 → reg 0
    builder.addVariableMapping(1, 8, sad::jit::StackValueType::Float);    // y: slot 1 → reg 8
    builder.setFrameSize(64);
    
    auto point = builder.build();
    
    assert(point.functionName == "test_func");
    assert(point.bytecodeOffset == 100);
    assert(point.nativeOffset == 0x1000);
    assert(point.variableMappings.size() == 2);
    assert(point.frameSize == 64);
    
    std::cout << "✅ اختبار OSRPointBuilder نجح!\n";
}

void testOSRManager() {
    sad::jit::OSRManager manager;
    
    sad::jit::OSREntryPoint point;
    point.functionName = "hot_loop";
    point.bytecodeOffset = 50;
    point.nativeOffset = 0x500;
    point.frameSize = 32;
    
    manager.registerOSRPoint(point);
    
    assert(manager.hasOSRPoint("hot_loop", 50));
    assert(!manager.hasOSRPoint("hot_loop", 100));
    assert(!manager.hasOSRPoint("other_func", 50));
    
    auto found = manager.findOSRPoint("hot_loop", 50);
    assert(found.has_value());
    assert(found->nativeOffset == 0x500);
    
    std::cout << "✅ اختبار OSRManager نجح!\n";
}

void testOSRChecker() {
    sad::jit::OSRManager manager;
    sad::jit::OSRChecker checker(manager, 100);  // فحص كل 100 تكرار
    
    // تسجيل نقطة OSR
    sad::jit::OSREntryPoint point;
    point.functionName = "loop_func";
    point.bytecodeOffset = 25;
    manager.registerOSRPoint(point);
    
    // محاكاة تكرارات الحلقة
    int triggerCount = 0;
    for (int i = 0; i < 350; i++) {
        if (checker.checkAtLoopBack("loop_func", 25)) {
            triggerCount++;
        }
    }
    
    // يجب أن يُفحص 3 مرات (100, 200, 300)
    assert(triggerCount == 3);
    
    std::cout << "✅ اختبار OSRChecker نجح!\n";
}

void testOSRTransition() {
    sad::jit::OSRManager manager;
    
    // إنشاء نقطة OSR
    sad::jit::OSREntryPoint point;
    point.functionName = "transition_func";
    point.bytecodeOffset = 75;
    point.nativeOffset = 256;
    point.frameSize = 64;
    point.variableMappings.push_back({0, 0, sad::jit::StackValueType::Integer});
    
    manager.registerOSRPoint(point);
    
    // إنشاء حالة VM
    sad::jit::VMState vmState;
    sad::jit::FrameInfo frame;
    frame.functionName = "transition_func";
    frame.bytecodeOffset = 75;
    frame.locals.push_back(sad::jit::StackValue::integer(42));
    vmState.callStack.push_back(frame);
    
    // محاكاة كود مُترجم
    std::vector<uint8_t> fakeCompiledCode(1024);
    
    // تنفيذ الانتقال
    auto result = manager.performTransition("transition_func", 75, 
                                            vmState, fakeCompiledCode.data());
    
    assert(result.success);
    assert(result.targetAddress != nullptr);
    
    auto stats = manager.getStatistics();
    assert(stats.totalTransitions == 1);
    assert(stats.successfulTransitions == 1);
    
    std::cout << "✅ اختبار OSR Transition نجح!\n";
}

void testOSRSystem() {
    sad::jit::OSRSystem system;
    
    // تسجيل نقاط OSR
    std::vector<sad::jit::OSREntryPoint> points;
    sad::jit::OSREntryPoint p1;
    p1.functionName = "system_func";
    p1.bytecodeOffset = 100;
    p1.nativeOffset = 0x1000;
    p1.frameSize = 32;
    points.push_back(p1);
    
    system.registerFunction("system_func", points);
    
    auto stats = system.getStatistics();
    assert(stats.totalOSRPoints == 1);
    
    system.printReport();
    
    std::cout << "✅ اختبار OSRSystem نجح!\n";
}

void testOSRStubGenerator() {
    sad::jit::OSRStubGenerator generator;
    
    sad::jit::OSREntryPoint point;
    point.functionName = "stub_func";
    point.frameSize = 32;
    
    auto entryStub = generator.generateEntryStub(point);
    auto exitStub = generator.generateExitStub(point);
    
    assert(!entryStub.empty());
    assert(!exitStub.empty());
    
    // التحقق من prologue
    assert(entryStub[0] == 0x55);  // push rbp
    
    // التحقق من ret في النهاية
    assert(exitStub.back() == 0xc3);  // ret
    
    std::cout << "✅ اختبار OSRStubGenerator نجح!\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات نظام OSR\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testStackValue();
    testOSRPointBuilder();
    testOSRManager();
    testOSRChecker();
    testOSRTransition();
    testOSRSystem();
    testOSRStubGenerator();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // OSR_TEST
