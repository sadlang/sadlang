/// تنفيذ إعادة التسمية

#include "lsp_rename.h"

namespace sad {
namespace lsp {

LspRename::LspRename(LspServer& server) : server_(server) {}

void LspRename::registerHandlers() {
    server_.registerHandler("textDocument/prepareRename",
        [this](const std::string& p) { return handlePrepareRename(p); });
    server_.registerHandler("textDocument/rename",
        [this](const std::string& p) { return handleRename(p); });
}

std::string LspRename::handlePrepareRename(const std::string& params) {
    /// التحقق: هل الموقع على رمز قابل لإعادة التسمية؟
    /// الكلمات المفتاحية غير قابلة لإعادة التسمية
    // TODO: تحليل الموقع وإرجاع نطاق الرمز
    return "null";
}

std::string LspRename::handleRename(const std::string& params) {
    /// خطوات إعادة التسمية:
    /// 1. إيجاد جميع المراجع للرمز
    /// 2. إنشاء تعديلات نصية لكل ملف
    /// 3. إرجاع WorkspaceEdit
    // TODO: تنفيذ إعادة التسمية عبر الملفات
    return "{\"changes\":{}}";
}

} // namespace lsp
} // namespace sad
