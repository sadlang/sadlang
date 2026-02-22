/// معلومات التحويم LSP
///
/// يوفر معلومات عن الرمز عند تمرير المؤشر فوقه
/// بما في ذلك التوقيع والنوع والتوثيق.

#pragma once

#include "lsp_server.h"
#include <string>

namespace sad {
namespace lsp {

/// معالج التحويم
class LspHover {
public:
    explicit LspHover(LspServer& server);
    void registerHandlers();

private:
    std::string handleHover(const std::string& params);
    
    /// بناء معلومات التحويم لكلمة مفتاحية
    std::string buildKeywordHover(const std::string& keyword) const;
    
    /// بناء معلومات التحويم لنوع مدمج
    std::string buildBuiltinTypeHover(const std::string& type_name) const;
    
    LspServer& server_;
};

} // namespace lsp
} // namespace sad
