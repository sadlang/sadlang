// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: hover.hpp
 * الوصف: ملف الترويسة لمزود معلومات التمرير
 * المرحلة: Phase 8 - US6 - أدوات التطوير
 * المهمة: T095
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_LSP_HOVER_HPP
#define SAD_LSP_HOVER_HPP

#include "../lsp_types.h"
#include <optional>

namespace sad {
namespace lsp {

// إعلان مسبق
struct DocumentInfo;
struct SymbolInfo;

/**
 * @class HoverProvider
 * @brief يوفر معلومات التمرير
 */
class HoverProvider {
public:
    /**
     * @brief الحصول على معلومات التمرير
     */
    std::optional<Hover> get_hover(const DocumentInfo& doc, const Position& pos);
};

} // namespace lsp
} // namespace sad

#endif // SAD_LSP_HOVER_HPP
