/// تنفيذ تلميحات الإدراج

#include "lsp_inlay_hints.h"

namespace sad {
namespace lsp {

LspInlayHints::LspInlayHints(LspServer& server) : server_(server) {}

void LspInlayHints::registerHandlers() {
    server_.registerHandler("textDocument/inlayHint",
        [this](const std::string& p) { return handleInlayHint(p); });
    server_.registerHandler("inlayHint/resolve",
        [this](const std::string& p) { return handleInlayHintResolve(p); });
}

std::string LspInlayHints::handleInlayHint(const std::string& params) {
    // TODO: تحليل النطاق وإرجاع تلميحات الأنواع والمعاملات
    return "[]";
}

std::string LspInlayHints::handleInlayHintResolve(const std::string& params) {
    return params;
}

} // namespace lsp
} // namespace sad
