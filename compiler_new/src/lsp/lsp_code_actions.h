/// إجراءات الكود LSP
///
/// يوفر إصلاحات سريعة وإعادة هيكلة الكود.

#pragma once

#include "lsp_server.h"
#include <string>

namespace sad {
namespace lsp {

/// معالج إجراءات الكود
class LspCodeActions {
public:
    explicit LspCodeActions(LspServer& server);
    void registerHandlers();

private:
    /// معالجة طلب إجراءات الكود
    std::string handleCodeAction(const std::string& params);
    /// تنفيذ إجراء محدد
    std::string handleCodeActionResolve(const std::string& params);

    LspServer& server_;
};

} // namespace lsp
} // namespace sad
