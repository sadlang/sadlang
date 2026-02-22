/// عدسات الكود LSP
///
/// يوفر معلومات تفاعلية فوق السطور مثل عدد المراجع.

#pragma once

#include "lsp_server.h"
#include <string>

namespace sad {
namespace lsp {

class LspCodeLens {
public:
    explicit LspCodeLens(LspServer& server);
    void registerHandlers();

private:
    std::string handleCodeLens(const std::string& params);
    std::string handleCodeLensResolve(const std::string& params);
    LspServer& server_;
};

} // namespace lsp
} // namespace sad
