// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: completion.hpp
 * الوصف: ملف الترويسة لمزود الإكمال التلقائي
 * المرحلة: Phase 8 - US6 - أدوات التطوير
 * المهمة: T093
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_LSP_COMPLETION_HPP
#define SAD_LSP_COMPLETION_HPP

#include "../lsp_types.h"
#include <string>
#include <vector>

namespace sad {
namespace lsp {

// إعلان مسبق
struct DocumentInfo;

/**
 * @class CompletionProvider
 * @brief يوفر اقتراحات الإكمال التلقائي
 */
class CompletionProvider {
public:
    /**
     * @brief الحصول على الإكمالات
     */
    CompletionList get_completions(const DocumentInfo& doc, const Position& pos);
};

} // namespace lsp
} // namespace sad

#endif // SAD_LSP_COMPLETION_HPP
