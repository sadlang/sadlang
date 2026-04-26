// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file early_termination.cpp
 * @brief تنفيذ الإنهاء المبكر لحل القيود
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *         تنفيذ الإنهاء المبكر في لغة ص (Early Termination Implementation)
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#include "types/early_termination.hpp"
#include "types/constraint_solver.hpp"

namespace Sad {
namespace TypeSystem {

// ═══════════════════════════════════════════════════════════════════════════
//                    تحويل أسباب الإنهاء / Termination Reason Conversion
// ═══════════════════════════════════════════════════════════════════════════

std::string terminationReasonToArabic(TerminationReason reason) {
    switch (reason) {
        case TerminationReason::None:
            return "لا يوجد سبب";
        case TerminationReason::SolutionFound:
            return "تم إيجاد الحل";
        case TerminationReason::TypeMismatch:
            return "تناقض في الأنواع";
        case TerminationReason::OccursCheck:
            return "تضمين دائري في النوع";
        case TerminationReason::TraitNotImplemented:
            return "سمة غير مُنفذة";
        case TerminationReason::ConstraintContradiction:
            return "تناقض بين القيود";
        case TerminationReason::IterationLimit:
            return "تجاوز حد التكرارات";
        case TerminationReason::TimeLimit:
            return "تجاوز حد الزمن";
        case TerminationReason::DepthLimit:
            return "تجاوز حد العمق";
        case TerminationReason::MemoryLimit:
            return "تجاوز حد الذاكرة";
        case TerminationReason::UserCancelled:
            return "تم الإلغاء من المستخدم";
        case TerminationReason::InternalError:
            return "خطأ داخلي";
        default:
            return "سبب غير معروف";
    }
}

std::string terminationReasonToEnglish(TerminationReason reason) {
    switch (reason) {
        case TerminationReason::None:
            return "No reason";
        case TerminationReason::SolutionFound:
            return "Solution found";
        case TerminationReason::TypeMismatch:
            return "Type mismatch";
        case TerminationReason::OccursCheck:
            return "Occurs check failed";
        case TerminationReason::TraitNotImplemented:
            return "Trait not implemented";
        case TerminationReason::ConstraintContradiction:
            return "Constraint contradiction";
        case TerminationReason::IterationLimit:
            return "Iteration limit exceeded";
        case TerminationReason::TimeLimit:
            return "Time limit exceeded";
        case TerminationReason::DepthLimit:
            return "Depth limit exceeded";
        case TerminationReason::MemoryLimit:
            return "Memory limit exceeded";
        case TerminationReason::UserCancelled:
            return "Cancelled by user";
        case TerminationReason::InternalError:
            return "Internal error";
        default:
            return "Unknown reason";
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//                    المُنشئ / Constructor
// ═══════════════════════════════════════════════════════════════════════════

EarlyTerminationMonitor::EarlyTerminationMonitor(const EarlyTerminationConfig& config)
    : config_(config)
    , startTime_()
    , iterationCount_(0)
    , currentDepth_(0)
    , running_(false)
    , terminationReason_(TerminationReason::None)
    , terminationMessage_()
    , terminationSuggestion_()
    , progressCallback_(nullptr)
    , externalCancellation_(nullptr)
    , internalCancellation_(false)
{
}

// ═══════════════════════════════════════════════════════════════════════════
//                    التحكم / Control
// ═══════════════════════════════════════════════════════════════════════════

void EarlyTerminationMonitor::start() {
    startTime_ = std::chrono::steady_clock::now();
    running_ = true;
    terminationReason_ = TerminationReason::None;
}

void EarlyTerminationMonitor::stop() {
    running_ = false;
}

void EarlyTerminationMonitor::reset() {
    iterationCount_ = 0;
    currentDepth_ = 0;
    running_ = false;
    terminationReason_ = TerminationReason::None;
    terminationMessage_.clear();
    terminationSuggestion_.clear();
    internalCancellation_ = false;
}

void EarlyTerminationMonitor::requestCancellation() {
    internalCancellation_ = true;
    setTermination(TerminationReason::UserCancelled, 
                   "تم طلب الإلغاء",
                   "أعد المحاولة عند الحاجة");
}

// ═══════════════════════════════════════════════════════════════════════════
//                    الفحص / Checking
// ═══════════════════════════════════════════════════════════════════════════

bool EarlyTerminationMonitor::shouldTerminate() const {
    // التحقق من الإلغاء
    if (isCancelled()) {
        return true;
    }
    
    // التحقق من وجود سبب إنهاء
    if (terminationReason_ != TerminationReason::None) {
        return true;
    }
    
    // التحقق من حدود الموارد
    return const_cast<EarlyTerminationMonitor*>(this)->checkResourceLimits();
}

bool EarlyTerminationMonitor::isCancelled() const {
    if (internalCancellation_) {
        return true;
    }
    if (externalCancellation_ && externalCancellation_->load()) {
        return true;
    }
    return false;
}

TerminationReason EarlyTerminationMonitor::getTerminationReason() const {
    return terminationReason_;
}

TerminationInfo EarlyTerminationMonitor::getTerminationInfo() const {
    TerminationInfo info;
    info.reason = terminationReason_;
    info.message = terminationMessage_;
    info.suggestion = terminationSuggestion_;
    info.iterationsPerformed = iterationCount_;
    info.depth = currentDepth_;
    
    if (running_) {
        auto now = std::chrono::steady_clock::now();
        info.elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - startTime_);
    }
    
    return info;
}

// ═══════════════════════════════════════════════════════════════════════════
//                    التتبع / Tracking
// ═══════════════════════════════════════════════════════════════════════════

void EarlyTerminationMonitor::recordIteration() {
    iterationCount_++;
    
    // استدعاء دالة التقدم إذا كانت مفعلة
    if (config_.enableProgressCallback && progressCallback_) {
        progressCallback_(iterationCount_, config_.maxIterations);
    }
    
    // التحقق من حد التكرارات
    if (iterationCount_ >= config_.maxIterations) {
        setTermination(TerminationReason::IterationLimit,
                       "تجاوز الحد الأقصى للتكرارات: " + std::to_string(config_.maxIterations),
                       "حاول تبسيط الأنواع أو تقليل عدد القيود");
    }
}

void EarlyTerminationMonitor::pushDepth() {
    currentDepth_++;
    
    if (currentDepth_ >= config_.maxDepth) {
        setTermination(TerminationReason::DepthLimit,
                       "تجاوز الحد الأقصى للعمق: " + std::to_string(config_.maxDepth),
                       "قد يكون هناك تعريف دائري للأنواع");
    }
}

void EarlyTerminationMonitor::popDepth() {
    if (currentDepth_ > 0) {
        currentDepth_--;
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//                    الكشف المبكر / Early Detection
// ═══════════════════════════════════════════════════════════════════════════

std::optional<TerminationInfo> EarlyTerminationMonitor::checkTypeMismatch(
    const TypePtr& left, const TypePtr& right) {
    
    if (!config_.enableContradictionCheck) {
        return std::nullopt;
    }
    
    // التحقق من التناقض الفوري
    // مثال: صحيح != نص
    if (left && right) {
        std::string leftStr = left->toString();
        std::string rightStr = right->toString();
        
        // الأنواع الأساسية المتناقضة
        auto isBasicType = [](const std::string& t) {
            return t == "صحيح" || t == "عشري" || t == "نص" || 
                   t == "منطقي" || t == "int" || t == "float" || 
                   t == "string" || t == "bool";
        };
        
        if (isBasicType(leftStr) && isBasicType(rightStr) && leftStr != rightStr) {
            TerminationInfo info;
            info.reason = TerminationReason::TypeMismatch;
            info.message = "لا يمكن مساواة " + leftStr + " مع " + rightStr;
            info.suggestion = "تأكد من تطابق الأنواع أو استخدم تحويل النوع";
            return info;
        }
    }
    
    return std::nullopt;
}

std::optional<TerminationInfo> EarlyTerminationMonitor::checkOccurs(
    const std::string& typeVar, const TypePtr& type) {
    
    if (!config_.enableOccursCheck || !type) {
        return std::nullopt;
    }
    
    // التحقق من احتواء النوع على متغير النوع
    std::string typeStr = type->toString();
    
    // بحث بسيط عن المتغير في تمثيل النوع
    if (typeStr.find(typeVar) != std::string::npos && typeStr != typeVar) {
        TerminationInfo info;
        info.reason = TerminationReason::OccursCheck;
        info.message = "متغير النوع '" + typeVar + "' موجود في النوع '" + typeStr + "'";
        info.suggestion = "هذا يؤدي إلى نوع لانهائي، راجع تعريف الأنواع";
        return info;
    }
    
    return std::nullopt;
}

std::optional<TerminationInfo> EarlyTerminationMonitor::checkConstraintContradiction(
    const std::vector<Constraint>& constraints) {
    
    if (!config_.enableContradictionCheck || constraints.size() < 2) {
        return std::nullopt;
    }
    
    // البحث عن تناقضات بسيطة بين القيود
    for (size_t i = 0; i < constraints.size(); ++i) {
        for (size_t j = i + 1; j < constraints.size(); ++j) {
            const auto& c1 = constraints[i];
            const auto& c2 = constraints[j];
            
            // التحقق من التناقض: A = B و A = C حيث B != C
            if (c1.getKind() == ConstraintKind::Equality &&
                c2.getKind() == ConstraintKind::Equality) {
                
                auto left1 = c1.getType1();
                auto right1 = c1.getType2();
                auto left2 = c2.getType1();
                auto right2 = c2.getType2();
                
                // إذا كان الطرف الأيسر متطابقاً
                if (left1 && left2 && right1 && right2) {
                    if (left1->toString() == left2->toString()) {
                        auto mismatch = checkTypeMismatch(right1, right2);
                        if (mismatch) {
                            mismatch->constraintIndex = i;
                            mismatch->reason = TerminationReason::ConstraintContradiction;
                            return mismatch;
                        }
                    }
                }
            }
        }
    }
    
    return std::nullopt;
}

// ═══════════════════════════════════════════════════════════════════════════
//                    الإشعارات / Callbacks
// ═══════════════════════════════════════════════════════════════════════════

void EarlyTerminationMonitor::setProgressCallback(ProgressCallback callback) {
    progressCallback_ = callback;
}

void EarlyTerminationMonitor::setCancellationToken(CancellationToken* token) {
    externalCancellation_ = token;
}

// ═══════════════════════════════════════════════════════════════════════════
//                    المساعدات الداخلية / Internal Helpers
// ═══════════════════════════════════════════════════════════════════════════

void EarlyTerminationMonitor::setTermination(TerminationReason reason,
                                              const std::string& message,
                                              const std::string& suggestion) {
    // فقط إذا لم يتم تعيين سبب بالفعل
    TerminationReason expected = TerminationReason::None;
    if (terminationReason_.compare_exchange_strong(expected, reason)) {
        terminationMessage_ = message;
        terminationSuggestion_ = suggestion;
    }
}

bool EarlyTerminationMonitor::checkResourceLimits() {
    // التحقق من حد التكرارات
    if (iterationCount_ >= config_.maxIterations) {
        setTermination(TerminationReason::IterationLimit,
                       "تجاوز الحد الأقصى للتكرارات",
                       "حاول تبسيط القيود");
        return true;
    }
    
    // التحقق من حد الزمن
    if (running_) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - startTime_);
        
        if (elapsed >= config_.maxTime) {
            setTermination(TerminationReason::TimeLimit,
                           "تجاوز الحد الأقصى للزمن",
                           "القيود معقدة جداً، حاول تبسيطها");
            return true;
        }
    }
    
    // التحقق من حد العمق
    if (currentDepth_ >= config_.maxDepth) {
        setTermination(TerminationReason::DepthLimit,
                       "تجاوز الحد الأقصى للعمق",
                       "قد يكون هناك تعريف دائري");
        return true;
    }
    
    return false;
}

} // namespace TypeSystem
} // namespace Sad
