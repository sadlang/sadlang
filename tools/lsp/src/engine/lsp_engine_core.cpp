// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: lsp_engine_core.cpp
// الوصف: نواة المحرك الأساسي لخادم LSP الثوري
// ══════════════════════════════════════════════════════════════════════════════
//
// هذا الملف هو القلب النابض لخادم بروتوكول اللغة (LSP).
// يربط بين كل المكونات: مخزن المستندات، خط أنابيب التحليل، فهرس الرموز.
//
// دورة حياة المحرك:
//   ① المنشئ يُنشئ المكونات الفرعية
//   ② initialize() يهيئ القدرات ويبدأ العمل
//   ③ did_open/did_change/did_close تدير المستندات
//   ④ عند كل تغيير، يُعاد التحليل وتُنشر التشخيصات
//   ⑤ shutdown() ينظف الموارد
//
// المميزات:
//   ✦ تحليل حقيقي باستخدام محلل لغة ص (LexerCore + ParserCore)
//   ✦ فهرسة رموز عبر الملفات مع تتبع النطاقات
//   ✦ آمن للاستخدام من عدة خيوط (thread-safe)
//   ✦ بنية قابلة للتوسيع بمزودات مستقلة
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "lexer_keywords.h"

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  المنشئ: إنشاء كل المكونات الفرعية
// ══════════════════════════════════════════════════════════════════════════════

LspEngine::LspEngine()
    : doc_store_(std::make_unique<DocumentStore>())
    , pipeline_(std::make_unique<AnalysisPipeline>())
    , index_(std::make_unique<SymbolIndex>())
{
    // تهيئة جدول الكلمات المفتاحية للتحليل المعجمي
    // يجب أن يتم مرة واحدة فقط عند بدء البرنامج
    Sad::Lexer::KeywordTable::initialize();
}

// ══════════════════════════════════════════════════════════════════════════════
//  المدمر: تنظيف الموارد
// ══════════════════════════════════════════════════════════════════════════════

LspEngine::~LspEngine() = default;

// ══════════════════════════════════════════════════════════════════════════════
//  تهيئة الخادم: إعداد القدرات المدعومة
// ══════════════════════════════════════════════════════════════════════════════

ServerCapabilities LspEngine::initialize(const std::string& root_uri) {
    root_uri_ = root_uri;

    // ────────────────────────────────────────────────────────
    //  تسجيل الدوال والأنواع المدمجة في الفهرس
    //  حتى يعمل الإكمال التلقائي والتلميحات للدوال القياسية
    //  مثل: اطبع، ادخل، طول، نوع، حول_لنص، إلخ
    // ────────────────────────────────────────────────────────
    index_->register_builtins();

    ServerCapabilities caps;

    // ──── مزامنة المستندات ────
    // نستخدم التغييرات التدريجية (incremental) لأداء أفضل
    caps.text_document_sync.open_close = true;
    caps.text_document_sync.change = 2; // 2 = Incremental

    // ──── الإكمال التلقائي ────
    // يُفعّل عند كتابة النقطة أو القوس أو الفاصلة العربية
    caps.completion_provider.trigger_characters = {".", ":", "(", "\xd8\x8c"}; // الفاصلة العربية ،
    caps.completion_provider.resolve_provider = true;

    // ──── توقيعات الدوال ────
    // تُفعّل عند كتابة قوس الفتح أو الفاصلة
    caps.signature_help_provider.trigger_characters = {"(", ",", "\xd8\x8c"};
    caps.signature_help_provider.retrigger_characters = {",", "\xd8\x8c"};

    // ──── ميزات التنقل ────
    caps.hover_provider = true;
    caps.definition_provider = true;
    caps.declaration_provider = true;
    caps.references_provider = true;

    // ──── ميزات المستند ────
    caps.document_highlight_provider = true;
    caps.document_symbol_provider = true;
    caps.workspace_symbol_provider = true;

    // ──── إجراءات الكود ────
    caps.code_action_provider = true;

    // ──── التنسيق ────
    caps.document_formatting_provider = true;
    caps.document_range_formatting_provider = true;

    // ──── إعادة التسمية ────
    caps.rename_provider = true;

    // ──── الرموز الدلالية (تلوين ذكي) ────
    caps.semantic_tokens_provider.full = true;
    caps.semantic_tokens_provider.range = true;
    caps.semantic_tokens_provider.delta = false;
    // أنواع الرموز المدعومة - بالترتيب (الفهرس مهم!)
    caps.semantic_tokens_provider.token_types = {
        "namespace", "type", "class", "enum", "interface",
        "struct", "typeParameter", "parameter", "variable",
        "property", "enumMember", "event", "function",
        "method", "macro", "keyword", "modifier", "comment",
        "string", "number", "regexp", "operator", "decorator"
    };
    // معدلات الرموز المدعومة
    caps.semantic_tokens_provider.token_modifiers = {
        "declaration", "definition", "readonly", "static",
        "deprecated", "abstract", "async", "modification",
        "documentation", "defaultLibrary"
    };

    // ──── التلميحات المضمنة ────
    caps.inlay_hint_provider = true;

    // ──── الطي ────
    caps.folding_range_provider = true;

    // ══════════════════════════════════════════════════════════════════════════
    //  القدرات الثورية الجديدة - المرحلة ٣
    // ══════════════════════════════════════════════════════════════════════════

    // ──── عدسات الكود (Code Lens) ────
    // تعرض عدد المراجع وعدد الأعضاء فوق الدوال والأصناف
    caps.code_lens_provider = true;

    // ──── نطاق التحديد الذكي (Selection Range) ────
    // توسيع التحديد بذكاء: كلمة → تعبير → جملة → كتلة → دالة → ملف
    caps.selection_range_provider = true;

    // ──── شجرة الاستدعاءات (Call Hierarchy) ────
    // إظهار من يستدعي الدالة (واردة) وماذا تستدعي (صادرة)
    caps.call_hierarchy_provider = true;

    // ──── روابط المستند (Document Links) ────
    // جعل مسارات الاستيراد قابلة للنقر
    caps.document_link_provider = true;

    // ──── التنسيق أثناء الكتابة (On Type Formatting) ────
    // تنسيق تلقائي فوري عند كتابة } أو سطر جديد أو ;
    caps.document_on_type_formatting_provider_enabled = true;

    return caps;
}

// ══════════════════════════════════════════════════════════════════════════════
//  إيقاف الخادم
// ══════════════════════════════════════════════════════════════════════════════

void LspEngine::shutdown() {
    // تنظيف الموارد
    // المدمرات ستتولى الباقي
}

// ══════════════════════════════════════════════════════════════════════════════
//  مزامنة المستندات: فتح / تغيير / إغلاق
// ══════════════════════════════════════════════════════════════════════════════

void LspEngine::did_open(const TextDocumentItem& item) {
    // ① حفظ المستند في المخزن
    doc_store_->open(item);

    // ② تحليل المستند ونشر النتائج (تشخيصات + رموز)
    analyze_and_publish(item.uri);
}

void LspEngine::did_change(const DocumentUri& uri,
                           const std::vector<TextDocumentContentChangeEvent>& changes,
                           int version) {
    // ① تطبيق التغييرات على المستند
    doc_store_->update(uri, changes, version);

    // ② إعادة التحليل ونشر النتائج
    analyze_and_publish(uri);
}

void LspEngine::did_close(const DocumentUri& uri) {
    // ① إزالة المستند من المخزن
    doc_store_->close(uri);

    // ② إزالة الرموز من الفهرس
    index_->remove_document(uri);

    // ③ مسح التشخيصات في المحرر (إرسال قائمة فارغة)
    if (diagnostics_publisher_) {
        diagnostics_publisher_(uri, {});
    }
}

// ══════════════════════════════════════════════════════════════════════════════
//  إعدادات
// ══════════════════════════════════════════════════════════════════════════════

void LspEngine::set_diagnostics_publisher(DiagnosticsPublisher publisher) {
    diagnostics_publisher_ = std::move(publisher);
}

ServerInfo LspEngine::get_server_info() const {
    return ServerInfo{};
}

} // namespace lsp
} // namespace sad
