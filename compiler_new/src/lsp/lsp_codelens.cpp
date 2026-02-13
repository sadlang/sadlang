/// تنفيذ عدسات الكود

#include "lsp_codelens.h"

namespace sad {
namespace lsp {

LspCodeLens::LspCodeLens(LspServer& server) : server_(server) {}

void LspCodeLens::registerHandlers() {
    server_.registerHandler("textDocument/codeLens",
        [this](const std::string& p) { return handleCodeLens(p); });
    server_.registerHandler("codeLens/resolve",
        [this](const std::string& p) { return handleCodeLensResolve(p); });
}

std::string LspCodeLens::handleCodeLens(const std::string& params) {
    // TODO: إرجاع عدسات لكل تعريف دالة/صنف
    return "[]";
}

std::string LspCodeLens::handleCodeLensResolve(const std::string& params) {
    // TODO: حساب عدد المراجع والاختبارات
    return params;
}

} // namespace lsp
} // namespace sad
