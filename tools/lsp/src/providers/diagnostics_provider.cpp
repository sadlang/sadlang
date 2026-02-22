// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: diagnostics_provider.cpp
// الوصف: مزود التشخيصات - يكتشف الأخطاء والتحذيرات ويرسلها للمحرر
// ══════════════════════════════════════════════════════════════════════════════
//
// التشخيصات تُنشر تلقائياً عند كل تغيير في المستند.
// يتم تشغيل خط أنابيب التحليل (Lexer → Parser) الذي ينتج:
//   ✦ أخطاء نحوية (من المحلل النحوي مباشرة)
//   ✦ تحذيرات أسلوبية (أسطر طويلة، كلمات إنجليزية)
//   ✦ اقتراحات تحسين
//
// ملاحظة: التشخيصات تُنشأ في analyze_and_publish() وليس هنا مباشرة.
// هذا الملف يحتوي على دالة analyze_and_publish ومنطق النشر.
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  تحليل المستند ونشر التشخيصات
//  هذه الدالة تُستدعى عند كل تغيير (didOpen, didChange)
// ══════════════════════════════════════════════════════════════════════════════

void LspEngine::analyze_and_publish(const DocumentUri& uri) {
    auto doc = doc_store_->get(uri);
    if (!doc) return;

    // تشغيل خط أنابيب التحليل
    auto result = pipeline_->analyze(doc->content, uri);

    // تحديث الرموز في المستند
    doc->symbols = result.symbols;
    doc->diagnostics = result.diagnostics;
    doc->references = result.references;
    doc->needs_reanalysis = false;

    // تحديث الفهرس العام
    index_->update_document(uri, result.symbols, result.references);

    // نشر التشخيصات للمحرر
    if (diagnostics_publisher_) {
        diagnostics_publisher_(uri, result.diagnostics);
    }
}

} // namespace lsp
} // namespace sad
