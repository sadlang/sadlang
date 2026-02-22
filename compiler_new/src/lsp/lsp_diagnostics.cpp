/// تنفيذ نظام التشخيصات

#include "lsp_diagnostics.h"
#include <sstream>

namespace sad {
namespace lsp {

/// تحويل تشخيص إلى JSON
std::string Diagnostic::toJson(bool teacher_mode) const {
    std::ostringstream json;
    json << "{\"range\":{\"start\":{\"line\":" << start_line
         << ",\"character\":" << start_char
         << "},\"end\":{\"line\":" << end_line
         << ",\"character\":" << end_char << "}}";
    
    json << ",\"severity\":" << static_cast<int>(severity);
    
    if (!code.empty()) {
        json << ",\"code\":\"" << code << "\"";
    }
    
    json << ",\"source\":\"" << source << "\"";
    
    // في وضع المعلم نستخدم الرسالة العربية
    if (teacher_mode && !message_ar.empty()) {
        json << ",\"message\":\"" << message_ar << "\"";
    } else {
        json << ",\"message\":\"" << message << "\"";
    }
    
    if (!tags.empty()) {
        json << ",\"tags\":[";
        for (size_t i = 0; i < tags.size(); i++) {
            if (i > 0) json << ",";
            json << static_cast<int>(tags[i]);
        }
        json << "]";
    }
    
    json << "}";
    return json.str();
}

LspDiagnostics::LspDiagnostics(LspServer& server) : server_(server) {}

/// تسجيل المعالجات
void LspDiagnostics::registerHandlers() {
    // التشخيصات تُرسل كإشعارات، لا تحتاج معالجات طلب
}

/// نشر التشخيصات لمستند
void LspDiagnostics::publishDiagnostics(const std::string& uri,
                                         const std::vector<Diagnostic>& diagnostics) {
    std::ostringstream params;
    params << "{\"uri\":\"" << uri << "\",\"diagnostics\":[";
    
    for (size_t i = 0; i < diagnostics.size(); i++) {
        if (i > 0) params << ",";
        params << diagnostics[i].toJson(server_.teacherMode());
    }
    
    params << "]}";
    
    server_.sendNotification("textDocument/publishDiagnostics", params.str());
}

/// تحليل ملف وإرسال التشخيصات
void LspDiagnostics::analyzeAndPublish(const std::string& uri,
                                        const std::string& content) {
    // TODO: استدعاء المحلل اللغوي والدلالي للغة ص
    // وتحويل الأخطاء إلى تشخيصات LSP
    
    std::vector<Diagnostic> diagnostics;
    
    // حالياً — تحليل بسيط للأقواس غير المتطابقة
    int paren_count = 0;
    int brace_count = 0;
    int bracket_count = 0;
    
    for (size_t i = 0; i < content.size(); i++) {
        switch (content[i]) {
            case '(': paren_count++; break;
            case ')': paren_count--; break;
            case '{': brace_count++; break;
            case '}': brace_count--; break;
            case '[': bracket_count++; break;
            case ']': bracket_count--; break;
        }
    }
    
    if (paren_count != 0) {
        Diagnostic d;
        d.severity = DiagnosticSeverity::Error;
        d.code = "E001";
        d.message = "Unmatched parentheses";
        d.message_ar = "أقواس دائرية غير متطابقة";
        diagnostics.push_back(d);
    }
    
    if (brace_count != 0) {
        Diagnostic d;
        d.severity = DiagnosticSeverity::Error;
        d.code = "E002";
        d.message = "Unmatched braces";
        d.message_ar = "أقواس معقوفة غير متطابقة";
        diagnostics.push_back(d);
    }
    
    publishDiagnostics(uri, diagnostics);
}

/// مسح التشخيصات لمستند
void LspDiagnostics::clearDiagnostics(const std::string& uri) {
    publishDiagnostics(uri, {});
}

} // namespace lsp
} // namespace sad
