// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file early_termination.hpp
 * @brief الإنهاء المبكر لحل القيود - تحسين الأداء عند اكتشاف الفشل
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *              الإنهاء المبكر في لغة ص (Early Termination)
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يحتوي على نظام الإنهاء المبكر لحل القيود.
 * يوقف الحل فوراً عند اكتشاف استحالة إيجاد حل.
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                    استراتيجيات الإنهاء المبكر
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * 1. التناقض الفوري (Immediate Contradiction):
 *    - تساوي نوعين غير متوافقين (مثل: عدد == نص)
 *    - تضمين دائري مباشر
 * 
 * 2. القيود المستحيلة (Impossible Constraints):
 *    - تطبيق سمة على نوع لا يدعمها
 *    - تضارب القيود العامة
 * 
 * 3. انتهاء الموارد (Resource Exhaustion):
 *    - تجاوز حد التكرار
 *    - تجاوز حد الزمن
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#ifndef SAD_TYPES_EARLY_TERMINATION_HPP
#define SAD_TYPES_EARLY_TERMINATION_HPP

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <atomic>
#include <optional>
#include "type.h"  // لتعريف TypePtr

namespace Sad {
namespace TypeSystem {

// التصريحات المسبقة
class Constraint;
class Substitution;

// ═══════════════════════════════════════════════════════════════════════════
//                    أسباب الإنهاء / Termination Reasons
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @enum TerminationReason
 * @brief سبب الإنهاء المبكر
 */
enum class TerminationReason {
    // لم ينتهِ بعد
    None,                           // لا يوجد سبب - لا زال يعمل
    
    // أسباب نجاح
    SolutionFound,                  // تم إيجاد حل
    
    // أسباب تناقض
    TypeMismatch,                   // تناقض أنواع (عدد != نص)
    OccursCheck,                    // اختبار التضمين الدائري فشل
    TraitNotImplemented,            // سمة غير مُنفذة
    ConstraintContradiction,        // تناقض بين قيود
    
    // أسباب موارد
    IterationLimit,                 // تجاوز حد التكرار
    TimeLimit,                      // تجاوز حد الزمن
    DepthLimit,                     // تجاوز حد العمق
    MemoryLimit,                    // تجاوز حد الذاكرة
    
    // أسباب أخرى
    UserCancelled,                  // إلغاء من المستخدم
    InternalError                   // خطأ داخلي
};

/**
 * @brief تحويل سبب الإنهاء لنص عربي
 */
std::string terminationReasonToArabic(TerminationReason reason);

/**
 * @brief تحويل سبب الإنهاء لنص إنجليزي
 */
std::string terminationReasonToEnglish(TerminationReason reason);

// ═══════════════════════════════════════════════════════════════════════════
//                    معلومات الإنهاء / Termination Info
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct TerminationInfo
 * @brief معلومات تفصيلية عن الإنهاء
 */
struct TerminationInfo {
    TerminationReason reason = TerminationReason::None;
    std::string message;                    // رسالة تفصيلية
    std::string suggestion;                 // اقتراح للإصلاح
    size_t constraintIndex = 0;             // فهرس القيد الذي سبب المشكلة
    std::chrono::milliseconds elapsed{0};   // الوقت المستغرق
    size_t iterationsPerformed = 0;         // عدد التكرارات
    size_t depth = 0;                       // العمق الحالي
    
    bool isSuccess() const {
        return reason == TerminationReason::SolutionFound;
    }
    
    bool isFailure() const {
        return reason != TerminationReason::None && 
               reason != TerminationReason::SolutionFound;
    }
    
    bool isContradiction() const {
        return reason == TerminationReason::TypeMismatch ||
               reason == TerminationReason::OccursCheck ||
               reason == TerminationReason::TraitNotImplemented ||
               reason == TerminationReason::ConstraintContradiction;
    }
    
    bool isResourceExhaustion() const {
        return reason == TerminationReason::IterationLimit ||
               reason == TerminationReason::TimeLimit ||
               reason == TerminationReason::DepthLimit ||
               reason == TerminationReason::MemoryLimit;
    }
    
    std::string toArabicString() const {
        std::string result = terminationReasonToArabic(reason);
        if (!message.empty()) {
            result += ": " + message;
        }
        if (!suggestion.empty()) {
            result += "\nاقتراح: " + suggestion;
        }
        return result;
    }
};

// ═══════════════════════════════════════════════════════════════════════════
//                    إعدادات الإنهاء المبكر / Early Termination Config
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct EarlyTerminationConfig
 * @brief إعدادات الإنهاء المبكر
 */
struct EarlyTerminationConfig {
    size_t maxIterations = 100000;          // الحد الأقصى للتكرارات
    std::chrono::milliseconds maxTime{30000}; // الحد الأقصى للزمن (30 ثانية)
    size_t maxDepth = 1000;                 // الحد الأقصى للعمق
    size_t maxMemoryMB = 500;               // الحد الأقصى للذاكرة
    
    bool enableOccursCheck = true;          // تفعيل اختبار التضمين الدائري
    bool enableContradictionCheck = true;   // تفعيل اكتشاف التناقض
    bool enableProgressCallback = false;    // تفعيل إشعارات التقدم
    
    EarlyTerminationConfig() = default;
    
    // إعدادات سريعة للتطوير (حدود أقل)
    static EarlyTerminationConfig development() {
        EarlyTerminationConfig config;
        config.maxIterations = 10000;
        config.maxTime = std::chrono::milliseconds{5000};
        config.maxDepth = 100;
        return config;
    }
    
    // إعدادات الإنتاج (حدود أعلى)
    static EarlyTerminationConfig production() {
        EarlyTerminationConfig config;
        config.maxIterations = 1000000;
        config.maxTime = std::chrono::milliseconds{60000};
        config.maxDepth = 5000;
        return config;
    }
};

// ═══════════════════════════════════════════════════════════════════════════
//                    مراقب الإنهاء المبكر / Early Termination Monitor
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @class EarlyTerminationMonitor
 * @brief مراقب الإنهاء المبكر لحل القيود
 */
class EarlyTerminationMonitor {
public:
    using ProgressCallback = std::function<void(size_t iteration, size_t total)>;
    using CancellationToken = std::atomic<bool>;
    
    explicit EarlyTerminationMonitor(
        const EarlyTerminationConfig& config = EarlyTerminationConfig());
    
    // ══════════════════════════════════════════════════════════════════════
    //                    التحكم / Control
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief بدء المراقبة
     */
    void start();
    
    /**
     * @brief إيقاف المراقبة
     */
    void stop();
    
    /**
     * @brief إعادة تعيين المراقب
     */
    void reset();
    
    /**
     * @brief طلب الإلغاء
     */
    void requestCancellation();
    
    // ══════════════════════════════════════════════════════════════════════
    //                    الفحص / Checking
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief فحص إذا يجب الإنهاء
     */
    bool shouldTerminate() const;
    
    /**
     * @brief فحص إذا تم طلب الإلغاء
     */
    bool isCancelled() const;
    
    /**
     * @brief الحصول على سبب الإنهاء الحالي
     */
    TerminationReason getTerminationReason() const;
    
    /**
     * @brief الحصول على معلومات الإنهاء الكاملة
     */
    TerminationInfo getTerminationInfo() const;
    
    // ══════════════════════════════════════════════════════════════════════
    //                    التتبع / Tracking
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief تسجيل تكرار جديد
     */
    void recordIteration();
    
    /**
     * @brief تسجيل دخول مستوى عمق
     */
    void pushDepth();
    
    /**
     * @brief تسجيل خروج من مستوى عمق
     */
    void popDepth();
    
    /**
     * @brief الحصول على العمق الحالي
     */
    size_t getCurrentDepth() const { return currentDepth_; }
    
    /**
     * @brief الحصول على عدد التكرارات
     */
    size_t getIterationCount() const { return iterationCount_; }
    
    // ══════════════════════════════════════════════════════════════════════
    //                    الكشف المبكر / Early Detection
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief فحص تناقض أنواع
     * @return معلومات الإنهاء إذا وجد تناقض
     */
    std::optional<TerminationInfo> checkTypeMismatch(
        const TypePtr& left, const TypePtr& right);
    
    /**
     * @brief فحص التضمين الدائري (occurs check)
     * @return معلومات الإنهاء إذا وجد تضمين دائري
     */
    std::optional<TerminationInfo> checkOccurs(
        const std::string& typeVar, const TypePtr& type);
    
    /**
     * @brief فحص تناقض القيود
     * @return معلومات الإنهاء إذا وجد تناقض
     */
    std::optional<TerminationInfo> checkConstraintContradiction(
        const std::vector<Constraint>& constraints);
    
    // ══════════════════════════════════════════════════════════════════════
    //                    الإشعارات / Callbacks
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief تعيين دالة إشعار التقدم
     */
    void setProgressCallback(ProgressCallback callback);
    
    /**
     * @brief تعيين رمز الإلغاء
     */
    void setCancellationToken(CancellationToken* token);

private:
    EarlyTerminationConfig config_;
    
    // حالة المراقبة
    std::chrono::steady_clock::time_point startTime_;
    std::atomic<size_t> iterationCount_{0};
    std::atomic<size_t> currentDepth_{0};
    std::atomic<bool> running_{false};
    
    // الإنهاء
    std::atomic<TerminationReason> terminationReason_{TerminationReason::None};
    std::string terminationMessage_;
    std::string terminationSuggestion_;
    
    // الإشعارات
    ProgressCallback progressCallback_;
    CancellationToken* externalCancellation_ = nullptr;
    std::atomic<bool> internalCancellation_{false};
    
    /**
     * @brief تحديث سبب الإنهاء
     */
    void setTermination(TerminationReason reason, 
                        const std::string& message = "",
                        const std::string& suggestion = "");
    
    /**
     * @brief فحص حدود الموارد
     */
    bool checkResourceLimits();
};

// ═══════════════════════════════════════════════════════════════════════════
//                    مساعدات الإنهاء المبكر / Early Termination Helpers
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief حارس الإنهاء المبكر للعمق
 */
class DepthGuard {
public:
    explicit DepthGuard(EarlyTerminationMonitor& monitor) 
        : monitor_(monitor), active_(true) {
        monitor_.pushDepth();
    }
    
    ~DepthGuard() {
        if (active_) {
            monitor_.popDepth();
        }
    }
    
    void release() { active_ = false; }
    
private:
    EarlyTerminationMonitor& monitor_;
    bool active_;
};

/**
 * @brief حارس الإنهاء المبكر للزمن
 */
class TimeGuard {
public:
    explicit TimeGuard(EarlyTerminationMonitor& monitor)
        : monitor_(monitor) {
        monitor_.start();
    }
    
    ~TimeGuard() {
        monitor_.stop();
    }
    
private:
    EarlyTerminationMonitor& monitor_;
};

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_TYPES_EARLY_TERMINATION_HPP
