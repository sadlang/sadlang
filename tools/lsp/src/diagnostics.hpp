// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: diagnostics.hpp
 * الوصف: ملف الترويسة لمزود التشخيصات
 * المرحلة: Phase 8 - US6 - أدوات التطوير
 * المهمة: T094
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_LSP_DIAGNOSTICS_HPP
#define SAD_LSP_DIAGNOSTICS_HPP

#include "../lsp_types.h"
#include <vector>

namespace sad {
namespace lsp {

// إعلان مسبق
struct DocumentInfo;

/**
 * @class DiagnosticsProvider
 * @brief يحلل الكود ويكتشف الأخطاء والتحذيرات
 */
class DiagnosticsProvider {
public:
    /**
     * @brief تحليل المستند
     */
    std::vector<Diagnostic> analyze(const DocumentInfo& doc);
};

} // namespace lsp
} // namespace sad

#endif // SAD_LSP_DIAGNOSTICS_HPP
