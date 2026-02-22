/// مساعدة التوقيع LSP
///
/// يعرض معلومات توقيع الدالة أثناء كتابة المعاملات.

#pragma once

#include "lsp_server.h"
#include <string>

namespace sad {
namespace lsp {

/// معالج مساعدة التوقيع
class LspSignature {
public:
    explicit LspSignature(LspServer& server);
    void registerHandlers();

private:
    std::string handleSignatureHelp(const std::string& params);
    LspServer& server_;
};

} // namespace lsp
} // namespace sad
