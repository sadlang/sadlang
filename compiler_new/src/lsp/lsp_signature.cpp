/// تنفيذ مساعدة التوقيع

#include "lsp_signature.h"

namespace sad {
namespace lsp {

LspSignature::LspSignature(LspServer& server) : server_(server) {}

void LspSignature::registerHandlers() {
    server_.registerHandler("textDocument/signatureHelp",
        [this](const std::string& p) { return handleSignatureHelp(p); });
}

std::string LspSignature::handleSignatureHelp(const std::string& params) {
    // TODO: تحليل السياق وإرجاع معلومات التوقيع
    // عند فتح قوس "(" أو فاصلة ","
    return "null";
}

} // namespace lsp
} // namespace sad
