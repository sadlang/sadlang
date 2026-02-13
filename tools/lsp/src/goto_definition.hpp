// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: goto_definition.hpp
 * الوصف: ملف الترويسة لمزود الانتقال للتعريف
 * المرحلة: Phase 8 - US6 - أدوات التطوير
 * المهمة: T096
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_LSP_GOTO_DEFINITION_HPP
#define SAD_LSP_GOTO_DEFINITION_HPP

#include "../lsp_types.h"
#include <vector>
#include <optional>
#include <unordered_map>

namespace sad {
namespace lsp {

// إعلان مسبق
struct DocumentInfo;

/**
 * @class GotoDefinitionProvider
 * @brief يوفر خدمة الانتقال للتعريف وإيجاد المراجع
 */
class GotoDefinitionProvider {
public:
    /**
     * @brief الانتقال للتعريف
     */
    std::optional<Location> get_definition(
        const DocumentInfo& doc,
        const Position& pos,
        const std::unordered_map<std::string, DocumentInfo>& all_docs);
    
    /**
     * @brief إيجاد جميع المراجع
     */
    std::vector<Location> get_references(
        const DocumentInfo& doc,
        const Position& pos,
        const std::unordered_map<std::string, DocumentInfo>& all_docs);
    
    /**
     * @brief إيجاد التنفيذات
     */
    std::vector<Location> get_implementations(
        const DocumentInfo& doc,
        const Position& pos,
        const std::unordered_map<std::string, DocumentInfo>& all_docs);
};

} // namespace lsp
} // namespace sad

#endif // SAD_LSP_GOTO_DEFINITION_HPP
