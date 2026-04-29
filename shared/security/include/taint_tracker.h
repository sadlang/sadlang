// ============================================================================
// taint_tracker.h — تتبع تلوث البيانات (Taint Tracking)
// (AR) محرك تتبع التلوث: يتعقب البيانات من مصادر غير موثوقة (مدخلات المستخدم،
//      الشبكة، الملفات) إلى مصارف حساسة (SQL، HTML، أوامر النظام)، مع كشف
//      ثغرات SQL injection و XSS و Command injection و Path traversal.
//      يوفر API بـ C++ وبـ C ABI للاستخدام عبر FFI.
// (EN) Taint tracking engine: tracks data from untrusted sources (user input,
//      network, files) to sensitive sinks (SQL, HTML, system commands), detecting
//      SQL injection, XSS, Command injection, and Path traversal vulnerabilities.
//      Exposes both a C++ API and a C ABI for FFI consumption.
// ============================================================================
//
// (AR) لماذا في الطبقة المشتركة؟
//      التلوث ظاهرة عرضية تخص كل مسار:
//        - المترجم: تحليل ساكن (static analysis) وقت الترجمة
//        - المفسر/VM: تتبع ديناميكي وقت التشغيل
//        - runtime/FFI: مكتبات المستخدم تستخدم C ABI لرفع التحذيرات
// ============================================================================

#ifndef SAD_SHARED_SECURITY_TAINT_TRACKER_H
#define SAD_SHARED_SECURITY_TAINT_TRACKER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Sad {
namespace Security {
namespace Taint {

// ----------------------------------------------------------------------------
// (AR) أنواع التعداد
// (EN) Enumerations
// ----------------------------------------------------------------------------

/// @brief (AR) نوع مصدر التلوث
/// @brief (EN) Taint source type
enum class SourceType {
    UserInput,    // (AR) إدخال المستخدم  (EN) user input
    Network,      // (AR) شبكة            (EN) network data
    File,         // (AR) ملف             (EN) file data
    Database,     // (AR) قاعدة بيانات    (EN) database
    Environment,  // (AR) بيئة            (EN) environment variable
    Unknown
};

/// @brief (AR) نوع المصرف الحساس
/// @brief (EN) Sink type
enum class SinkType {
    SqlQuery,     // SQL injection target
    Html,         // XSS target
    SystemCmd,    // Command injection target
    FilePath,     // Path traversal target
    UrlRedirect,  // Open redirect target
    Serialization,
    Eval
};

/// @brief (AR) مستوى الخطورة
/// @brief (EN) Severity level
enum class Severity { Critical, High, Medium, Low, Info };

// ----------------------------------------------------------------------------
// (AR) هياكل البيانات
// (EN) Data structures
// ----------------------------------------------------------------------------

/// @brief (AR) علامة تلوث واحدة
/// @brief (EN) Single taint marker
struct TaintMark {
    SourceType source = SourceType::Unknown;
    std::string description;
    std::string sourceLocation;  // (AR) "ملف:سطر"  (EN) "file:line"
    int id = 0;
};

/// @brief (AR) حالة تلوث متغير
/// @brief (EN) Per-variable taint state
class TaintState {
public:
    TaintState() = default;

    void taint(const TaintMark& mark);
    void sanitizeFrom(SourceType source, const std::string& method);
    void sanitizeAll(const std::string& method);
    void merge(const TaintState& other);

    bool isTainted() const noexcept { return tainted_ && !marks_.empty(); }
    bool isSanitized() const noexcept { return sanitized_; }
    const std::vector<TaintMark>& marks() const noexcept { return marks_; }
    const std::string& sanitizationMethod() const noexcept { return sanitizationMethod_; }

private:
    std::vector<TaintMark> marks_;
    bool tainted_ = false;
    bool sanitized_ = false;
    std::string sanitizationMethod_;
};

/// @brief (AR) تحذير أمني مكتشف
/// @brief (EN) Detected security warning
struct SecurityWarning {
    Severity severity = Severity::Medium;
    SinkType sink = SinkType::Html;
    std::string message;
    std::string file;
    int line = 0;
    int column = 0;
    std::vector<TaintMark> marks;
    std::string fixSuggestion;
    std::string attackExample;

    /// @brief (AR) الاسم النصي لنوع المصرف (مفيد للتقارير)
    /// @brief (EN) Human-readable sink-type name (useful for reports)
    std::string sinkTypeName() const;
};

// ----------------------------------------------------------------------------
// (AR) محرك التتبع الرئيسي
// (EN) Main tracker engine
// ----------------------------------------------------------------------------

/**
 * @brief متتبع تلوث البيانات
 * @brief (EN) Taint tracker
 *
 * (AR) يحفظ حالة التلوث لكل متغير، يطبق قواعد المصادر/المصارف/التنقية،
 *      ويُولّد تحذيرات أمنية عند وصول بيانات ملوثة إلى مصرف حساس.
 * (EN) Maintains per-variable taint state, applies source/sink/sanitizer rules,
 *      and emits security warnings when tainted data reaches a sensitive sink.
 */
class TaintTracker {
public:
    TaintTracker();
    ~TaintTracker();

    /// @brief (AR) تلويث متغير من مصدر معين
    /// @brief (EN) Mark a variable as tainted from the given source
    void taint(const std::string& variable, SourceType source,
               const std::string& location = "");

    /// @brief (AR) نقل التلوث من متغير لآخر
    /// @brief (EN) Propagate taint from one variable to another
    void propagate(const std::string& from, const std::string& to);

    /// @brief (AR) تنقية متغير عبر طريقة معينة (إذا كانت مناسبة للمصرف)
    /// @brief (EN) Sanitize a variable via the named method (if appropriate for sink)
    void sanitize(const std::string& variable, const std::string& method,
                  SinkType forSink);

    /// @brief (AR) فحص متغير عند مصرف حساس — يرجع التحذير عند الاكتشاف
    /// @brief (EN) Check a variable at a sensitive sink — returns warning if detected
    SecurityWarning* checkSink(const std::string& variable, SinkType sink,
                               const std::string& file, int line);

    /// @brief (AR) فحص استدعاء دالة مع معاملات
    /// @brief (EN) Check a function call with arguments
    void checkCall(const std::string& function,
                   const std::vector<std::string>& args,
                   const std::string& file, int line);

    /// @brief (AR) قائمة التحذيرات المتراكمة
    /// @brief (EN) Accumulated warnings list
    const std::vector<SecurityWarning>& warnings() const noexcept;

    /// @brief (AR) هل المتغير ملوث حالياً؟
    /// @brief (EN) Is the variable currently tainted?
    bool isTainted(const std::string& variable) const;

    /// @brief (AR) مسح جميع الحالات والتحذيرات
    /// @brief (EN) Clear all states and warnings
    void clear();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace Taint
}  // namespace Security
}  // namespace Sad

// ----------------------------------------------------------------------------
// (AR) C ABI للاستخدام عبر FFI
// (EN) C ABI for FFI consumption
// ----------------------------------------------------------------------------

extern "C" {

typedef struct SadTaintTracker SadTaintTracker;

/// @brief (AR) إنشاء متتبع جديد
/// @brief (EN) Create a new tracker
SadTaintTracker* sad_taint_tracker_create(void);

/// @brief (AR) تحرير المتتبع (يقبل nullptr بأمان)
/// @brief (EN) Destroy a tracker (accepts nullptr safely)
void sad_taint_tracker_destroy(SadTaintTracker* tracker);

/// @brief (AR) تلويث متغير (source_type يقابل enum SourceType: 0..5)
/// @brief (EN) Taint a variable (source_type maps to SourceType enum: 0..5)
void sad_taint_tracker_taint(SadTaintTracker* tracker, const char* variable,
                             int source_type, const char* location);

/// @brief (AR) فحص ما إذا كان المتغير ملوثاً (1=نعم، 0=لا)
/// @brief (EN) Check if a variable is tainted (1=yes, 0=no)
int sad_taint_tracker_is_tainted(SadTaintTracker* tracker, const char* variable);

/// @brief (AR) عدد التحذيرات الحالية
/// @brief (EN) Current warning count
int sad_taint_tracker_warning_count(SadTaintTracker* tracker);

}  // extern "C"

#endif  // SAD_SHARED_SECURITY_TAINT_TRACKER_H
