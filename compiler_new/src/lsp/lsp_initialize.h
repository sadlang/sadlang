/// تهيئة بروتوكول LSP
///
/// يعالج طلب initialize وإشعار initialized
/// ويُكوّن قدرات الخادم.

#pragma once

#include "lsp_server.h"
#include <string>

namespace sad {
namespace lsp {

/// معالج تهيئة LSP
class LspInitialize {
public:
    explicit LspInitialize(LspServer& server);

    /// تسجيل المعالجات
    void registerHandlers();

private:
    /// معالجة طلب initialize
    std::string handleInitialize(const std::string& params);

    /// معالجة إشعار initialized
    void handleInitialized(const std::string& params);

    /// بناء JSON قدرات الخادم
    std::string buildCapabilitiesJson() const;

    /// بناء JSON وسيلة إيضاح الرموز الدلالية
    std::string buildSemanticTokensLegend() const;

    LspServer& server_;
};

} // namespace lsp
} // namespace sad
