/**
 * @file profiler_hooks.h
 * @brief (AR) خطافات مصحح الأداء — الواجهة بين المصحح والمفسر
 * @brief (EN) Profiler Hooks — Interface between profiler and interpreter
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * الوصف بالعربية:
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يُعرّف الخطافات (Hooks) التي تربط مصحح الأداء بالمفسر.
 * تعمل الخطافات كحراس RAII — عند إنشاء كائن الحارس، يُسجل دخول الدالة.
 * وعند تدمير الحارس (عند الخروج من النطاق)، يُسجل خروج الدالة تلقائياً.
 * 
 * هذا النمط يضمن:
 * 1. عدم نسيان تسجيل خروج الدالة حتى عند حدوث استثناءات
 * 2. قياس دقيق للزمن (لا تفقد وقت لمعالجة الأخطاء)
 * 3. سهولة الدمج مع المفسر (سطر واحد في كل نقطة ربط)
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * أمثلة الاستخدام:
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * @code{.cpp}
 * // في ExpressionEvaluator::visitCallExpr:
 * void visitCallExpr(CallExpr& node) {
 *     PROFILE_FUNCTION(funcName);  // ← ماكرو سهل الاستخدام
 *     // ... تنفيذ عادي ...
 * }
 * 
 * // أو يدوياً:
 * {
 *     ProfileGuard guard("جمع", profiler);
 *     // ... تنفيذ الدالة ...
 * }  // ← يُسجل الخروج تلقائياً عند انتهاء النطاق
 * @endcode
 * 
 * @author فريق تطوير لغة ص
 * @date 2025
 */

#pragma once

#include "profiler_core.h"
#include <string>

namespace Sad {
namespace Tools {

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) حارس التنميط RAII — يسجل دخول/خروج الدالة تلقائياً
// (EN) RAII Profile Guard — automatically records function enter/exit
// ═══════════════════════════════════════════════════════════════════════════════
/**
 * @brief (AR) حارس RAII لتسجيل زمن الدوال تلقائياً
 * @brief (EN) RAII guard for automatic function timing
 * 
 * يُنشأ عند دخول الدالة ويُدمّر عند الخروج منها.
 * يُسجل الزمن المنقضي تلقائياً في مصحح الأداء.
 * 
 * @example
 * @code{.cpp}
 * void myFunction() {
 *     ProfileGuard guard("دالتي", &profiler);
 *     // ... العمل هنا ...
 * }  // ← عند الخروج يُسجل الزمن المنقضي
 * @endcode
 */
class ProfileGuard {
public:
    /**
     * @brief (AR) البناء — يُسجل دخول الدالة
     * @brief (EN) Constructor — records function entry
     * 
     * @param funcName (AR) اسم الدالة / (EN) Function name
     * @param profiler (AR) مؤشر للمصحح / (EN) Pointer to profiler
     * @param isBuiltin (AR) هل الدالة مدمجة؟ / (EN) Is function built-in?
     * @param fileName (AR) اسم الملف / (EN) File name
     * @param line (AR) رقم السطر / (EN) Line number
     */
    ProfileGuard(const std::string& funcName, ProfilerCore* profiler,
                 bool isBuiltin = false, const std::string& fileName = "",
                 size_t line = 0)
        : funcName_(funcName), profiler_(profiler), active_(profiler != nullptr && profiler->isActive())
    {
        if (active_) {
            profiler_->onFunctionEnter(funcName_, isBuiltin, fileName, line);
        }
    }
    
    /// (AR) منع النسخ / (EN) Prevent copying
    ProfileGuard(const ProfileGuard&) = delete;
    ProfileGuard& operator=(const ProfileGuard&) = delete;
    
    /// (AR) دعم النقل / (EN) Support move
    ProfileGuard(ProfileGuard&& other) noexcept
        : funcName_(std::move(other.funcName_))
        , profiler_(other.profiler_)
        , active_(other.active_)
    {
        other.active_ = false;
    }
    
    /**
     * @brief (AR) التدمير — يُسجل خروج الدالة تلقائياً
     * @brief (EN) Destructor — automatically records function exit
     */
    ~ProfileGuard() {
        if (active_ && profiler_) {
            profiler_->onFunctionExit(funcName_);
        }
    }
    
    /// (AR) إلغاء التسجيل (لا يُسجل الخروج) / (EN) Cancel recording
    void cancel() { active_ = false; }
    
private:
    std::string funcName_;
    ProfilerCore* profiler_;
    bool active_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) ماكروهات سهلة الاستخدام
// (EN) Convenience macros
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) ماكرو لتسجيل دالة — يُنشئ حارس RAII تلقائياً
 * (EN) Macro to profile a function — creates RAII guard automatically
 * 
 * يستخدم المصحح العام (global profiler) إذا كان متاحاً.
 * إذا لم يكن متاحاً، لا يفعل شيئاً (تكلفة صفرية).
 */
#define SAD_PROFILE_FUNCTION(name) \
    ::Sad::Tools::ProfileGuard _sad_profile_guard_##__LINE__( \
        (name), ::Sad::Tools::getGlobalProfiler())

/**
 * (AR) ماكرو لتسجيل دالة مع تفاصيل الملف والسطر
 * (EN) Macro to profile a function with file and line details
 */
#define SAD_PROFILE_FUNCTION_DETAIL(name, isBuiltin, fileName, line) \
    ::Sad::Tools::ProfileGuard _sad_profile_guard_##__LINE__( \
        (name), ::Sad::Tools::getGlobalProfiler(), (isBuiltin), (fileName), (line))

/**
 * (AR) ماكرو لتسجيل نطاق مخصوص
 * (EN) Macro to profile a custom scope
 */
#define SAD_PROFILE_SCOPE(name) \
    ::Sad::Tools::ProfileGuard _sad_scope_guard_##__LINE__( \
        (name), ::Sad::Tools::getGlobalProfiler())

} // namespace Tools
} // namespace Sad
