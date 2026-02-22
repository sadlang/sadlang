/// تلميحات الإدراج LSP
///
/// يوفر تلميحات مضمنة مثل أنواع المتغيرات وأسماء المعاملات.

#pragma once

#include "lsp_server.h"
#include <string>

namespace sad {
namespace lsp {

/// معالج تلميحات الإدراج
class LspInlayHints {
public:
    explicit LspInlayHints(LspServer& server);
    void registerHandlers();

private:
    std::string handleInlayHint(const std::string& params);
    std::string handleInlayHintResolve(const std::string& params);
    LspServer& server_;
};

} // namespace lsp
} // namespace sad
