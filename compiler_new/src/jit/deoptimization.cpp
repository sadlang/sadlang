/**
 * =============================================================================
 * ملف: deoptimization.cpp
 * الوصف: دعم إلغاء التحسين (Deoptimization)
 * المهمة: T213 - Implement deoptimization support
 * المرحلة: Phase 21 - User Story 18 (JIT & Tiered Compilation)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما هو Deoptimization؟
 * ─────────────────────
 * العودة من الكود المُحسَّن (JIT) إلى الكود المُفسَّر (VM) عندما:
 * - افتراض التحسين يصبح غير صحيح
 * - نحتاج معلومات تصحيح الأخطاء
 * - نريد تعديل الكود أثناء التشغيل
 * 
 * لماذا نحتاج Deoptimization؟
 * ────────────────────────────
 * JIT يفترض أشياء لتحسين الأداء:
 * ```
 * // افتراض: x دائماً صحيح
 * دالة حساب(x)
 *     ارجع x + 1    // مُحسَّن لأرقام صحيحة
 * نهاية
 * 
 * // لكن المستخدم يمرر عشري!
 * حساب(3.14)  // ⚠️ الافتراض خاطئ!
 * ```
 * 
 * الحل: Deopt → العودة للمفسر الذي يتعامل مع كل الأنواع
 * 
 * متى يحدث Deoptimization؟
 * ────────────────────────
 * 1. **تغيير النوع**: نوع مختلف عن المتوقع
 * 2. **تجاوز الحدود**: array[خارج_النطاق]
 * 3. **استثناء**: خطأ أثناء التنفيذ
 * 4. **تعديل الكود**: hot reload
 * 5. **تصحيح الأخطاء**: breakpoint
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
#include <chrono>
#include <atomic>
#include <mutex>
#include <iostream>

namespace sad::jit {

// =============================================================================
// أسباب إلغاء التحسين (Deopt Reasons)
// =============================================================================

/**
 * أسباب إلغاء التحسين
 */
enum class DeoptReason {
    // أسباب متعلقة بالأنواع
    TypeMismatch,           // نوع غير متوقع
    TypeChanged,            // النوع تغيّر
    PolymorphicCall,        // استدعاء متعدد الأشكال
    
    // أسباب متعلقة بالحدود
    ArrayBoundsCheck,       // فحص حدود المصفوفة
    NullCheck,              // فحص null
    DivisionByZero,         // قسمة على صفر
    
    // أسباب متعلقة بالافتراضات
    AssumptionViolated,     // افتراض خاطئ
    GuardFailed,            // حارس فشل
    UnexpectedValue,        // قيمة غير متوقعة
    
    // أسباب خارجية
    DebugBreakpoint,        // نقطة توقف
    HotReload,              // إعادة تحميل
    ProfileUpdate,          // تحديث ملف التشغيل
    
    // أسباب داخلية
    StackOverflow,          // طفح المكدس
    OutOfMemory,            // نفاد الذاكرة
    InternalError,          // خطأ داخلي
};

inline const char* deoptReasonName(DeoptReason reason) {
    switch (reason) {
        case DeoptReason::TypeMismatch: return "نوع_غير_متطابق";
        case DeoptReason::TypeChanged: return "تغيّر_النوع";
        case DeoptReason::PolymorphicCall: return "استدعاء_متعدد";
        case DeoptReason::ArrayBoundsCheck: return "فحص_الحدود";
        case DeoptReason::NullCheck: return "فحص_null";
        case DeoptReason::DivisionByZero: return "قسمة_على_صفر";
        case DeoptReason::AssumptionViolated: return "افتراض_خاطئ";
        case DeoptReason::GuardFailed: return "فشل_الحارس";
        case DeoptReason::UnexpectedValue: return "قيمة_غير_متوقعة";
        case DeoptReason::DebugBreakpoint: return "نقطة_توقف";
        case DeoptReason::HotReload: return "إعادة_تحميل";
        case DeoptReason::ProfileUpdate: return "تحديث_ملف";
        case DeoptReason::StackOverflow: return "طفح_المكدس";
        case DeoptReason::OutOfMemory: return "نفاد_الذاكرة";
        case DeoptReason::InternalError: return "خطأ_داخلي";
    }
    return "مجهول";
}

// =============================================================================
// حالة المكدس (Stack State)
// =============================================================================

/**
 * قيمة في المكدس
 */
struct DeoptValue {
    enum class Type { Int, Float, Bool, Ref, Nil };
    Type type;
    union {
        int64_t intVal;
        double floatVal;
        bool boolVal;
        void* refVal;
    };
    
    static DeoptValue integer(int64_t v) { DeoptValue d; d.type = Type::Int; d.intVal = v; return d; }
    static DeoptValue floating(double v) { DeoptValue d; d.type = Type::Float; d.floatVal = v; return d; }
    static DeoptValue boolean(bool v) { DeoptValue d; d.type = Type::Bool; d.boolVal = v; return d; }
    static DeoptValue reference(void* v) { DeoptValue d; d.type = Type::Ref; d.refVal = v; return d; }
    static DeoptValue nil() { DeoptValue d; d.type = Type::Nil; d.intVal = 0; return d; }
};

/**
 * موقع قيمة في الكود المُترجم
 */
struct ValueLocation {
    enum class Kind {
        Register,       // في سجل
        Stack,          // على المكدس
        Constant,       // ثابت
        Invalid,        // غير صالح
    };
    
    Kind kind;
    int index;          // رقم السجل أو offset المكدس
    DeoptValue constant; // للثوابت
    
    static ValueLocation reg(int r) { return {Kind::Register, r, {}}; }
    static ValueLocation stack(int offset) { return {Kind::Stack, offset, {}}; }
    static ValueLocation constVal(const DeoptValue& v) { return {Kind::Constant, 0, v}; }
    static ValueLocation invalid() { return {Kind::Invalid, 0, {}}; }
};

// =============================================================================
// نقطة إلغاء التحسين (Deopt Point)
// =============================================================================

/**
 * معلومات نقطة deopt
 */
struct DeoptPoint {
    std::string functionName;
    int nativeOffset;           // موقع في الكود المُترجم
    int bytecodeOffset;         // موقع في bytecode
    DeoptReason reason;
    
    // تعيين المتغيرات: JIT location → VM slot
    struct ValueMapping {
        int vmSlot;
        ValueLocation jitLocation;
        std::string variableName;  // للتصحيح
    };
    std::vector<ValueMapping> valueMappings;
    
    // معلومات الإطار
    int frameSize;
    int localCount;
    int stackDepth;
};

// =============================================================================
// سجلات Deoptimization (Deopt Records)
// =============================================================================

/**
 * سجل حدث deopt
 */
struct DeoptRecord {
    std::string functionName;
    DeoptReason reason;
    int nativeOffset;
    int bytecodeOffset;
    std::chrono::steady_clock::time_point timestamp;
    uint64_t recoveryTime;      // وقت الاستعادة (μs)
    bool successful;
};

// =============================================================================
// مدير Deoptimization
// =============================================================================

/**
 * نتيجة عملية deopt
 */
struct DeoptResult {
    bool success = false;
    std::string errorMessage;
    int resumeBytecodeOffset = -1;
    std::vector<DeoptValue> recoveredValues;
    uint64_t deoptTime = 0;
};

/**
 * مدير إلغاء التحسين
 */
class DeoptimizationManager {
public:
    DeoptimizationManager() = default;
    
    /**
     * تسجيل نقطة deopt
     */
    void registerDeoptPoint(const DeoptPoint& point) {
        std::string key = makeKey(point.functionName, point.nativeOffset);
        deoptPoints_[key] = point;
    }
    
    /**
     * البحث عن نقطة deopt
     */
    std::optional<DeoptPoint> findDeoptPoint(const std::string& function, 
                                              int nativeOffset) const {
        std::string key = makeKey(function, nativeOffset);
        auto it = deoptPoints_.find(key);
        if (it != deoptPoints_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    /**
     * تنفيذ deoptimization
     */
    DeoptResult performDeopt(const std::string& function,
                             int nativeOffset,
                             DeoptReason reason,
                             void* framePointer) {
        DeoptResult result;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // البحث عن نقطة deopt
        auto point = findDeoptPoint(function, nativeOffset);
        if (!point) {
            result.success = false;
            result.errorMessage = "لم يُعثر على نقطة deopt";
            recordDeopt(function, reason, nativeOffset, -1, false, 0);
            return result;
        }
        
        // استعادة القيم من إطار JIT
        result.recoveredValues = recoverValues(*point, framePointer);
        result.resumeBytecodeOffset = point->bytecodeOffset;
        result.success = true;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.deoptTime = std::chrono::duration_cast<std::chrono::microseconds>(
            endTime - startTime
        ).count();
        
        recordDeopt(function, reason, nativeOffset, point->bytecodeOffset, 
                   true, result.deoptTime);
        
        // تحديث إحصائيات السبب
        reasonCounts_[reason]++;
        
        return result;
    }
    
    /**
     * إبطال كود دالة
     */
    void invalidateFunction(const std::string& function) {
        // حذف كل نقاط deopt للدالة
        std::vector<std::string> toRemove;
        for (const auto& [key, point] : deoptPoints_) {
            if (point.functionName == function) {
                toRemove.push_back(key);
            }
        }
        for (const auto& key : toRemove) {
            deoptPoints_.erase(key);
        }
        
        invalidatedFunctions_.insert(function);
    }
    
    /**
     * هل الدالة مُبطلة؟
     */
    bool isInvalidated(const std::string& function) const {
        return invalidatedFunctions_.count(function) > 0;
    }
    
    /**
     * إحصائيات
     */
    struct Statistics {
        size_t totalDeoptPoints = 0;
        uint64_t totalDeopts = 0;
        uint64_t successfulDeopts = 0;
        uint64_t failedDeopts = 0;
        uint64_t totalRecoveryTime = 0;
        size_t invalidatedFunctions = 0;
        std::map<DeoptReason, uint64_t> reasonDistribution;
    };
    
    Statistics getStatistics() const {
        Statistics stats;
        stats.totalDeoptPoints = deoptPoints_.size();
        stats.totalDeopts = totalDeopts_;
        stats.successfulDeopts = successfulDeopts_;
        stats.failedDeopts = totalDeopts_ - successfulDeopts_;
        stats.totalRecoveryTime = totalRecoveryTime_;
        stats.invalidatedFunctions = invalidatedFunctions_.size();
        stats.reasonDistribution = reasonCounts_;
        return stats;
    }
    
    /**
     * الحصول على سجلات deopt
     */
    std::vector<DeoptRecord> getRecentRecords(size_t count = 10) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        size_t start = records_.size() > count ? records_.size() - count : 0;
        return std::vector<DeoptRecord>(records_.begin() + start, records_.end());
    }
    
    /**
     * طباعة تقرير
     */
    void printReport() const {
        auto stats = getStatistics();
        
        std::cout << "\n═══════════════════════════════════════════════\n";
        std::cout << "   تقرير Deoptimization\n";
        std::cout << "═══════════════════════════════════════════════\n\n";
        
        std::cout << "نقاط Deopt المسجلة: " << stats.totalDeoptPoints << "\n";
        std::cout << "عمليات Deopt الكلية: " << stats.totalDeopts << "\n";
        std::cout << "عمليات ناجحة: " << stats.successfulDeopts << "\n";
        std::cout << "عمليات فاشلة: " << stats.failedDeopts << "\n";
        std::cout << "دوال مُبطلة: " << stats.invalidatedFunctions << "\n";
        
        if (stats.successfulDeopts > 0) {
            std::cout << "متوسط وقت الاستعادة: " 
                      << (stats.totalRecoveryTime / stats.successfulDeopts) << "μs\n";
        }
        
        if (!stats.reasonDistribution.empty()) {
            std::cout << "\nتوزيع الأسباب:\n";
            for (const auto& [reason, count] : stats.reasonDistribution) {
                std::cout << "  " << deoptReasonName(reason) << ": " << count << "\n";
            }
        }
    }

private:
    std::unordered_map<std::string, DeoptPoint> deoptPoints_;
    std::vector<DeoptRecord> records_;
    std::set<std::string> invalidatedFunctions_;
    std::map<DeoptReason, uint64_t> reasonCounts_;
    
    mutable std::mutex mutex_;
    uint64_t totalDeopts_ = 0;
    uint64_t successfulDeopts_ = 0;
    uint64_t totalRecoveryTime_ = 0;
    
    std::string makeKey(const std::string& function, int offset) const {
        return function + ":" + std::to_string(offset);
    }
    
    std::vector<DeoptValue> recoverValues(const DeoptPoint& point, void* framePointer) {
        std::vector<DeoptValue> values;
        
        for (const auto& mapping : point.valueMappings) {
            DeoptValue value = readValue(mapping.jitLocation, framePointer);
            values.push_back(value);
        }
        
        return values;
    }
    
    DeoptValue readValue(const ValueLocation& location, void* framePointer) {
        switch (location.kind) {
            case ValueLocation::Kind::Constant:
                return location.constant;
                
            case ValueLocation::Kind::Register:
                // في الواقع نحتاج قراءة من السجل المحفوظ
                return DeoptValue::integer(0);
                
            case ValueLocation::Kind::Stack: {
                if (!framePointer) return DeoptValue::nil();
                // قراءة من المكدس
                int64_t* stackPtr = static_cast<int64_t*>(framePointer);
                return DeoptValue::integer(stackPtr[location.index / 8]);
            }
                
            case ValueLocation::Kind::Invalid:
            default:
                return DeoptValue::nil();
        }
    }
    
    void recordDeopt(const std::string& function, DeoptReason reason,
                     int nativeOffset, int bytecodeOffset,
                     bool success, uint64_t recoveryTime) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        DeoptRecord record;
        record.functionName = function;
        record.reason = reason;
        record.nativeOffset = nativeOffset;
        record.bytecodeOffset = bytecodeOffset;
        record.timestamp = std::chrono::steady_clock::now();
        record.recoveryTime = recoveryTime;
        record.successful = success;
        
        records_.push_back(record);
        
        totalDeopts_++;
        if (success) {
            successfulDeopts_++;
            totalRecoveryTime_ += recoveryTime;
        }
        
        // حد أقصى للسجلات
        if (records_.size() > 1000) {
            records_.erase(records_.begin(), records_.begin() + 500);
        }
    }
};

// =============================================================================
// مُنشئ نقاط Deopt (Deopt Point Builder)
// =============================================================================

/**
 * مُنشئ نقاط deopt
 */
class DeoptPointBuilder {
public:
    void startPoint(const std::string& function, int nativeOffset, int bytecodeOffset) {
        current_ = DeoptPoint{};
        current_.functionName = function;
        current_.nativeOffset = nativeOffset;
        current_.bytecodeOffset = bytecodeOffset;
    }
    
    void setReason(DeoptReason reason) {
        current_.reason = reason;
    }
    
    void addValueMapping(int vmSlot, const ValueLocation& jitLocation, 
                        const std::string& name = "") {
        current_.valueMappings.push_back({vmSlot, jitLocation, name});
    }
    
    void setFrameInfo(int frameSize, int localCount, int stackDepth) {
        current_.frameSize = frameSize;
        current_.localCount = localCount;
        current_.stackDepth = stackDepth;
    }
    
    DeoptPoint build() {
        return current_;
    }
    
private:
    DeoptPoint current_;
};

// =============================================================================
// حراس التحسين (Optimization Guards)
// =============================================================================

/**
 * نوع الحارس
 */
enum class GuardType {
    TypeCheck,          // فحص النوع
    BoundsCheck,        // فحص الحدود
    NullCheck,          // فحص null
    RangeCheck,         // فحص النطاق
    ShapeCheck,         // فحص شكل الكائن
};

/**
 * حارس تحسين
 */
struct OptimizationGuard {
    GuardType type;
    int deoptPointId;
    DeoptReason failureReason;
    
    // بيانات الفحص
    std::variant<
        int,            // للأنواع: type ID
        std::pair<int, int>,  // للحدود: (min, max)
        void*           // للكائنات: shape pointer
    > checkData;
};

/**
 * مدير الحراس
 */
class GuardManager {
public:
    /**
     * إضافة حارس
     */
    int addGuard(const OptimizationGuard& guard) {
        guards_.push_back(guard);
        return static_cast<int>(guards_.size() - 1);
    }
    
    /**
     * فحص حارس نوع
     */
    bool checkTypeGuard(int guardId, int actualType) {
        if (guardId < 0 || guardId >= static_cast<int>(guards_.size())) {
            return false;
        }
        
        const auto& guard = guards_[guardId];
        if (guard.type != GuardType::TypeCheck) return false;
        
        int expectedType = std::get<int>(guard.checkData);
        bool passed = (actualType == expectedType);
        
        if (!passed) guardFailures_[guardId]++;
        return passed;
    }
    
    /**
     * فحص حارس حدود
     */
    bool checkBoundsGuard(int guardId, int index) {
        if (guardId < 0 || guardId >= static_cast<int>(guards_.size())) {
            return false;
        }
        
        const auto& guard = guards_[guardId];
        if (guard.type != GuardType::BoundsCheck) return false;
        
        auto [min, max] = std::get<std::pair<int, int>>(guard.checkData);
        bool passed = (index >= min && index < max);
        
        if (!passed) guardFailures_[guardId]++;
        return passed;
    }
    
    /**
     * إحصائيات الحراس
     */
    std::map<int, uint64_t> getFailureStats() const {
        return guardFailures_;
    }
    
private:
    std::vector<OptimizationGuard> guards_;
    std::map<int, uint64_t> guardFailures_;
};

// =============================================================================
// مُولّد كود Deopt (Deopt Code Generator)
// =============================================================================

/**
 * مُولّد كود deopt
 */
class DeoptCodeGenerator {
public:
    /**
     * توليد كود فحص الحارس
     */
    std::vector<uint8_t> generateGuardCheck(const OptimizationGuard& guard,
                                            int deoptTarget) {
        std::vector<uint8_t> code;
        
        // مثال: فحص النوع
        // cmp rax, expectedType
        // jne deoptTarget
        
        switch (guard.type) {
            case GuardType::TypeCheck:
                // cmp eax, imm32
                code.push_back(0x3d);
                {
                    int32_t expected = std::get<int>(guard.checkData);
                    code.insert(code.end(), 
                               reinterpret_cast<uint8_t*>(&expected),
                               reinterpret_cast<uint8_t*>(&expected) + 4);
                }
                break;
                
            case GuardType::BoundsCheck:
                // cmp edi, esi (index vs length)
                code.push_back(0x39);
                code.push_back(0xf7);
                break;
                
            default:
                // (AR) نوع حارس غير معروف — إصدار trap (int3) لمنع تنفيذ خاطئ
                // (EN) Unknown guard type — emit trap (int3) to prevent wrong execution
                std::cerr << "[sadc تحذير] نوع حارس JIT غير معالج: "
                          << static_cast<int>(guard.type) << std::endl;
                code.push_back(0xCC);  // int3 — debug trap
                break;
        }
        
        // jne rel32 (deopt)
        code.push_back(0x0f);
        code.push_back(0x85);
        int32_t offset = deoptTarget - (static_cast<int>(code.size()) + 4);
        code.insert(code.end(),
                   reinterpret_cast<uint8_t*>(&offset),
                   reinterpret_cast<uint8_t*>(&offset) + 4);
        
        return code;
    }
    
    /**
     * توليد stub للانتقال لـ deopt
     */
    std::vector<uint8_t> generateDeoptStub(int deoptPointId) {
        std::vector<uint8_t> code;
        
        // حفظ السجلات
        code.push_back(0x50);  // push rax
        code.push_back(0x51);  // push rcx
        code.push_back(0x52);  // push rdx
        
        // تحميل deopt point ID
        code.push_back(0xb8);  // mov eax, imm32
        code.insert(code.end(),
                   reinterpret_cast<uint8_t*>(&deoptPointId),
                   reinterpret_cast<uint8_t*>(&deoptPointId) + 4);
        
        // استدعاء معالج deopt
        // call deopt_handler
        // (في الواقع نحتاج عنوان المعالج)
        
        return code;
    }
};

} // namespace sad::jit

// =============================================================================
// واجهة C للتكامل
// =============================================================================

extern "C" {

using namespace sad::jit;

void* sad_deopt_manager_new() {
    return new DeoptimizationManager();
}

void sad_deopt_manager_free(void* manager) {
    delete static_cast<DeoptimizationManager*>(manager);
}

void sad_deopt_register_point(void* manager, const char* function,
                               int nativeOffset, int bytecodeOffset) {
    if (!manager || !function) return;
    
    DeoptPoint point;
    point.functionName = function;
    point.nativeOffset = nativeOffset;
    point.bytecodeOffset = bytecodeOffset;
    
    static_cast<DeoptimizationManager*>(manager)->registerDeoptPoint(point);
}

int sad_deopt_perform(void* manager, const char* function,
                      int nativeOffset, int reason, void* framePointer) {
    if (!manager || !function) return -1;
    
    auto result = static_cast<DeoptimizationManager*>(manager)->performDeopt(
        function, nativeOffset, static_cast<DeoptReason>(reason), framePointer
    );
    
    return result.success ? result.resumeBytecodeOffset : -1;
}

void sad_deopt_invalidate_function(void* manager, const char* function) {
    if (!manager || !function) return;
    static_cast<DeoptimizationManager*>(manager)->invalidateFunction(function);
}

int sad_deopt_is_invalidated(void* manager, const char* function) {
    if (!manager || !function) return 0;
    return static_cast<DeoptimizationManager*>(manager)->isInvalidated(function) ? 1 : 0;
}

void sad_deopt_print_report(void* manager) {
    if (manager) {
        static_cast<DeoptimizationManager*>(manager)->printReport();
    }
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef DEOPTIMIZATION_TEST

#include <cassert>

void testDeoptValue() {
    auto intVal = sad::jit::DeoptValue::integer(42);
    assert(intVal.type == sad::jit::DeoptValue::Type::Int);
    assert(intVal.intVal == 42);
    
    auto floatVal = sad::jit::DeoptValue::floating(3.14);
    assert(floatVal.type == sad::jit::DeoptValue::Type::Float);
    
    auto boolVal = sad::jit::DeoptValue::boolean(true);
    assert(boolVal.boolVal == true);
    
    std::cout << "✅ اختبار DeoptValue نجح!\n";
}

void testValueLocation() {
    auto regLoc = sad::jit::ValueLocation::reg(5);
    assert(regLoc.kind == sad::jit::ValueLocation::Kind::Register);
    assert(regLoc.index == 5);
    
    auto stackLoc = sad::jit::ValueLocation::stack(16);
    assert(stackLoc.kind == sad::jit::ValueLocation::Kind::Stack);
    assert(stackLoc.index == 16);
    
    auto constLoc = sad::jit::ValueLocation::constVal(sad::jit::DeoptValue::integer(100));
    assert(constLoc.kind == sad::jit::ValueLocation::Kind::Constant);
    
    std::cout << "✅ اختبار ValueLocation نجح!\n";
}

void testDeoptPointBuilder() {
    sad::jit::DeoptPointBuilder builder;
    
    builder.startPoint("test_func", 0x100, 50);
    builder.setReason(sad::jit::DeoptReason::TypeMismatch);
    builder.addValueMapping(0, sad::jit::ValueLocation::reg(0), "x");
    builder.addValueMapping(1, sad::jit::ValueLocation::stack(8), "y");
    builder.setFrameInfo(64, 2, 0);
    
    auto point = builder.build();
    
    assert(point.functionName == "test_func");
    assert(point.nativeOffset == 0x100);
    assert(point.bytecodeOffset == 50);
    assert(point.reason == sad::jit::DeoptReason::TypeMismatch);
    assert(point.valueMappings.size() == 2);
    assert(point.frameSize == 64);
    
    std::cout << "✅ اختبار DeoptPointBuilder نجح!\n";
}

void testDeoptManager() {
    sad::jit::DeoptimizationManager manager;
    
    // تسجيل نقطة deopt
    sad::jit::DeoptPoint point;
    point.functionName = "deopt_func";
    point.nativeOffset = 256;
    point.bytecodeOffset = 30;
    point.valueMappings.push_back({0, sad::jit::ValueLocation::constVal(
        sad::jit::DeoptValue::integer(42)), "x"});
    
    manager.registerDeoptPoint(point);
    
    // البحث
    auto found = manager.findDeoptPoint("deopt_func", 256);
    assert(found.has_value());
    assert(found->bytecodeOffset == 30);
    
    // تنفيذ deopt
    auto result = manager.performDeopt("deopt_func", 256, 
                                       sad::jit::DeoptReason::TypeMismatch, nullptr);
    assert(result.success);
    assert(result.resumeBytecodeOffset == 30);
    assert(result.recoveredValues.size() == 1);
    assert(result.recoveredValues[0].intVal == 42);
    
    std::cout << "✅ اختبار DeoptManager نجح!\n";
}

void testInvalidation() {
    sad::jit::DeoptimizationManager manager;
    
    sad::jit::DeoptPoint point;
    point.functionName = "invalid_func";
    point.nativeOffset = 100;
    point.bytecodeOffset = 10;
    
    manager.registerDeoptPoint(point);
    
    assert(!manager.isInvalidated("invalid_func"));
    
    manager.invalidateFunction("invalid_func");
    
    assert(manager.isInvalidated("invalid_func"));
    
    // لا يمكن العثور على نقاط deopt للدالة المُبطلة
    auto found = manager.findDeoptPoint("invalid_func", 100);
    assert(!found.has_value());
    
    std::cout << "✅ اختبار الإبطال نجح!\n";
}

void testGuardManager() {
    sad::jit::GuardManager guards;
    
    // إضافة حارس نوع
    sad::jit::OptimizationGuard typeGuard;
    typeGuard.type = sad::jit::GuardType::TypeCheck;
    typeGuard.checkData = 1;  // type ID = 1 (integer)
    int typeGuardId = guards.addGuard(typeGuard);
    
    // إضافة حارس حدود
    sad::jit::OptimizationGuard boundsGuard;
    boundsGuard.type = sad::jit::GuardType::BoundsCheck;
    boundsGuard.checkData = std::make_pair(0, 10);  // [0, 10)
    int boundsGuardId = guards.addGuard(boundsGuard);
    
    // فحص حارس النوع
    assert(guards.checkTypeGuard(typeGuardId, 1));  // صحيح
    assert(!guards.checkTypeGuard(typeGuardId, 2)); // خطأ
    
    // فحص حارس الحدود
    assert(guards.checkBoundsGuard(boundsGuardId, 5));  // صحيح
    assert(!guards.checkBoundsGuard(boundsGuardId, 10)); // خطأ
    assert(!guards.checkBoundsGuard(boundsGuardId, -1)); // خطأ
    
    std::cout << "✅ اختبار GuardManager نجح!\n";
}

void testStatistics() {
    sad::jit::DeoptimizationManager manager;
    
    // تسجيل نقاط deopt
    for (int i = 0; i < 5; i++) {
        sad::jit::DeoptPoint point;
        point.functionName = "stats_func";
        point.nativeOffset = i * 100;
        point.bytecodeOffset = i * 10;
        manager.registerDeoptPoint(point);
    }
    
    // تنفيذ deopts
    manager.performDeopt("stats_func", 0, sad::jit::DeoptReason::TypeMismatch, nullptr);
    manager.performDeopt("stats_func", 100, sad::jit::DeoptReason::TypeMismatch, nullptr);
    manager.performDeopt("stats_func", 200, sad::jit::DeoptReason::ArrayBoundsCheck, nullptr);
    
    auto stats = manager.getStatistics();
    assert(stats.totalDeoptPoints == 5);
    assert(stats.totalDeopts == 3);
    assert(stats.successfulDeopts == 3);
    assert(stats.reasonDistribution[sad::jit::DeoptReason::TypeMismatch] == 2);
    assert(stats.reasonDistribution[sad::jit::DeoptReason::ArrayBoundsCheck] == 1);
    
    manager.printReport();
    
    std::cout << "✅ اختبار الإحصائيات نجح!\n";
}

void testDeoptCodeGenerator() {
    sad::jit::DeoptCodeGenerator generator;
    
    sad::jit::OptimizationGuard guard;
    guard.type = sad::jit::GuardType::TypeCheck;
    guard.checkData = 1;
    
    auto guardCode = generator.generateGuardCheck(guard, 0x1000);
    assert(!guardCode.empty());
    
    auto stubCode = generator.generateDeoptStub(42);
    assert(!stubCode.empty());
    // تحقق من push rax
    assert(stubCode[0] == 0x50);
    
    std::cout << "✅ اختبار DeoptCodeGenerator نجح!\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات نظام Deoptimization\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testDeoptValue();
    testValueLocation();
    testDeoptPointBuilder();
    testDeoptManager();
    testInvalidation();
    testGuardManager();
    testStatistics();
    testDeoptCodeGenerator();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // DEOPTIMIZATION_TEST
