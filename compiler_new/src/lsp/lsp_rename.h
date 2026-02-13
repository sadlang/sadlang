/// إعادة التسمية LSP
///
/// يوفر إعادة تسمية الرموز عبر الملفات.

#pragma once

#include "lsp_server.h"
#include <string>

namespace sad {
namespace lsp {

class LspRename {
public:
    explicit LspRename(LspServer& server);
    void registerHandlers();

private:
    /// التحقق من إمكانية إعادة التسمية
    std::string handlePrepareRename(const std::string& params);
    /// تنفيذ إعادة التسمية
    std::string handleRename(const std::string& params);

    LspServer& server_;
};

} // namespace lsp
} // namespace sad
