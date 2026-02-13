/// البحث عن المراجع LSP
///
/// يوفر البحث عن جميع مراجع رمز في المشروع
/// بما في ذلك تسلسل الاستدعاءات وتسلسل الأنواع.

#pragma once

#include "lsp_server.h"
#include <string>

namespace sad {
namespace lsp {

/// معالج المراجع
class LspReferences {
public:
    explicit LspReferences(LspServer& server);
    void registerHandlers();

private:
    std::string handleReferences(const std::string& params);
    std::string handlePrepareCallHierarchy(const std::string& params);
    std::string handleCallHierarchyIncoming(const std::string& params);
    std::string handleCallHierarchyOutgoing(const std::string& params);
    std::string handlePrepareTypeHierarchy(const std::string& params);
    std::string handleTypeHierarchySupertypes(const std::string& params);
    std::string handleTypeHierarchySubtypes(const std::string& params);
    
    LspServer& server_;
};

} // namespace lsp
} // namespace sad
