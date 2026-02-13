/// نظام التشخيصات LSP
///
/// يحلل الكود ويرسل تشخيصات (أخطاء، تحذيرات، معلومات)
/// إلى العميل عبر إشعار publishDiagnostics.

#pragma once

#include "lsp_server.h"
#include <string>
#include <vector>

namespace sad {
namespace lsp {

/// مستوى خطورة التشخيص
enum class DiagnosticSeverity {
    Error = 1,
    Warning = 2,
    Information = 3,
    Hint = 4,
};

/// علامة التشخيص
enum class DiagnosticTag {
    Unnecessary = 1,
    Deprecated = 2,
};

/// تشخيص واحد
struct Diagnostic {
    int start_line = 0;
    int start_char = 0;
    int end_line = 0;
    int end_char = 0;
    DiagnosticSeverity severity = DiagnosticSeverity::Error;
    std::string code;
    std::string source = "sadc";
    std::string message;
    std::string message_ar; // الرسالة بالعربية
    std::vector<DiagnosticTag> tags;
    
    /// تحويل إلى JSON
    std::string toJson(bool teacher_mode = false) const;
};

/// معالج التشخيصات
class LspDiagnostics {
public:
    explicit LspDiagnostics(LspServer& server);

    /// تسجيل المعالجات
    void registerHandlers();

    /// نشر التشخيصات لمستند
    void publishDiagnostics(const std::string& uri,
                           const std::vector<Diagnostic>& diagnostics);

    /// تحليل ملف وإرسال التشخيصات
    void analyzeAndPublish(const std::string& uri, const std::string& content);

    /// مسح التشخيصات لمستند
    void clearDiagnostics(const std::string& uri);

private:
    LspServer& server_;
};

} // namespace lsp
} // namespace sad
