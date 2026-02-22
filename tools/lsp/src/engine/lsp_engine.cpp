// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: lsp_engine.cpp
// الوصف: تنفيذ المحرك الأساسي لخادم LSP الثوري
// ══════════════════════════════════════════════════════════════════════════════
//
// هذا الملف يحتوي على تنفيذ الصنف LspEngine الذي يمثل:
//   ✦ القلب النابض لخادم LSP
//   ✦ ينسق بين مخزن المستندات وخط التحليل وفهرس الرموز
//   ✦ يوفر واجهة موحدة لكل ميزات اللغة
//
// مسؤوليات هذا الملف:
//   ① تهيئة المحرك وتسجيل الدوال المدمجة
//   ② إدارة مزامنة المستندات (فتح/تعديل/إغلاق)
//   ③ تجميع قدرات الخادم (Server Capabilities)
//   ④ ربط كل المكونات معاً
//
// ملاحظة مهمة:
//   الميزات الفعلية (إكمال، تشخيصات، إلخ) منفذة في ملفات المزودات المنفصلة
//   (providers/*.cpp) كأعضاء في LspEngine. هذا الملف يحتوي فقط على
//   المنطق المشترك والبنية التحتية.
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include <iostream>

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  المنشئ والمدمر
// ══════════════════════════════════════════════════════════════════════════════

/// المنشئ: ينشئ كل المكونات الداخلية
/// - مخزن المستندات لتتبع الملفات المفتوحة
/// - خط أنابيب التحليل لتشغيل Lexer + Parser
/// - فهرس الرموز للبحث السريع عبر الملفات
LspEngine::LspEngine()
    : doc_store_(std::make_unique<DocumentStore>()),
      pipeline_(std::make_unique<AnalysisPipeline>()),
      index_(std::make_unique<SymbolIndex>())
{
}

/// المدمر: افتراضي - المؤشرات الذكية تتكفل بالتنظيف
LspEngine::~LspEngine() = default;

// ══════════════════════════════════════════════════════════════════════════════
//  التهيئة وإعداد القدرات
// ══════════════════════════════════════════════════════════════════════════════

/// تهيئة المحرك:
///   ① تسجيل الدوال المدمجة (اطبع، ادخل، طول، إلخ) في الفهرس
///   ② تعيين مسار مساحة العمل الجذري
///   ③ إعداد وإرجاع قدرات الخادم (ماذا يدعم الخادم)
ServerCapabilities LspEngine::initialize(const std::string& root_uri) {
    root_uri_ = root_uri;

    // تسجيل الدوال والأنواع المدمجة في الفهرس
    // حتى يعمل الإكمال التلقائي والتلميحات للدوال القياسية
    index_->register_builtins();

    // بناء قدرات الخادم
    // كل قدرة تمثل ميزة يدعمها الخادم ويخبر بها المحرر
    ServerCapabilities caps;

    // مزامنة المستندات: ندعم التحديث التدريجي (incremental)
    // أي نرسل فقط التغييرات وليس المستند كاملاً
    caps.text_document_sync.open_close = true;
    caps.text_document_sync.change = 2; // 2 = incremental

    // الإكمال التلقائي: يُفعّل عند كتابة . أو : أو (
    caps.completion_provider.trigger_characters = {".", ":", "(", "\xd8\x8c"}; // الفاصلة العربية ،
    caps.completion_provider.resolve_provider = true;

    // مساعدة التوقيع: تُفعّل عند كتابة ( أو ،
    caps.signature_help_provider.trigger_characters = {"(", "\xd8\x8c", ","};
    caps.signature_help_provider.retrigger_characters = {",", "\xd8\x8c"};

    // ميزات الانتقال والبحث
    caps.hover_provider = true;
    caps.definition_provider = true;
    caps.references_provider = true;
    caps.document_symbol_provider = true;
    caps.workspace_symbol_provider = true; // البحث عن رموز مساحة العمل (Ctrl+T)

    // إعادة التسمية
    caps.rename_provider = true;

    // إجراءات الكود (إصلاحات سريعة)
    caps.code_action_provider = true;

    // التنسيق
    caps.document_formatting_provider = true;
    caps.document_range_formatting_provider = true;

    // الرموز الدلالية
    // قائمة بأسماء أنواع الرموز الدلالية (الترتيب مهم!)
    caps.semantic_tokens_provider.full = true;
    caps.semantic_tokens_provider.range = true;
    caps.semantic_tokens_provider.delta = false;
    caps.semantic_tokens_provider.token_types = {
        "namespace", "type", "class", "enum", "interface",
        "struct", "typeParameter", "parameter", "variable",
        "property", "enumMember", "event", "function",
        "method", "macro", "keyword", "modifier",
        "comment", "string", "number", "regexp",
        "operator", "decorator"
    };
    caps.semantic_tokens_provider.token_modifiers = {
        "declaration", "definition", "readonly", "static",
        "deprecated", "abstract", "async", "modification",
        "documentation", "defaultLibrary"
    };

    // التلميحات المضمنة
    caps.inlay_hint_provider = true;

    // الطي
    caps.folding_range_provider = true;

    return caps;
}

/// إيقاف المحرك بشكل نظيف
void LspEngine::shutdown() {
    // لا يوجد موارد خاصة تحتاج تنظيفاً
    // المؤشرات الذكية تتكفل بكل شيء
}

// ══════════════════════════════════════════════════════════════════════════════
//  مزامنة المستندات
// ══════════════════════════════════════════════════════════════════════════════

/// فتح مستند جديد:
///   ① تخزينه في مخزن المستندات
///   ② تشغيل التحليل الأول (Lexer → Parser → Symbols → Diagnostics)
///   ③ نشر التشخيصات للمحرر
void LspEngine::did_open(const TextDocumentItem& item) {
    doc_store_->open(item);
    analyze_and_publish(item.uri);
}

/// تحديث مستند (تغيير تدريجي):
/// عند كل ضغطة مفتاح تقريباً، المحرر يرسل التغييرات، ونحن:
///   ① نطبق التغييرات على المحتوى المُخزّن
///   ② نعيد التحليل (يمكن تأخيره مستقبلاً لتحسين الأداء)
///   ③ ننشر التشخيصات المحدّثة
void LspEngine::did_change(const DocumentUri& uri,
                           const std::vector<TextDocumentContentChangeEvent>& changes,
                           int version) {
    doc_store_->update(uri, changes, version);
    analyze_and_publish(uri);
}

/// إغلاق مستند:
///   ① إزالته من مخزن المستندات
///   ② إزالة رموزه من الفهرس
///   ③ نشر قائمة تشخيصات فارغة (لمسح الأخطاء من المحرر)
void LspEngine::did_close(const DocumentUri& uri) {
    index_->remove_document(uri);

    // مسح التشخيصات عند إغلاق المستند
    if (diagnostics_publisher_) {
        diagnostics_publisher_(uri, {}); // قائمة فارغة = إزالة كل التشخيصات
    }

    doc_store_->close(uri);
}

// ══════════════════════════════════════════════════════════════════════════════
//  إعدادات
// ══════════════════════════════════════════════════════════════════════════════

/// تعيين دالة نشر التشخيصات
/// هذه الدالة تُستدعى من طبقة النقل عند التهيئة
/// لأن الطبقة المسؤولة عن إرسال الإشعارات هي طبقة النقل
void LspEngine::set_diagnostics_publisher(DiagnosticsPublisher publisher) {
    diagnostics_publisher_ = std::move(publisher);
}

/// الحصول على معلومات الخادم (للرد على طلب initialize)
ServerInfo LspEngine::get_server_info() const {
    return ServerInfo{
        "\xd8\xae\xd8\xa7\xd8\xaf\xd9\x85 \xd9\x84\xd8\xba\xd8\xa9 \xd8\xb5 \xd8\xa7\xd9\x84\xd8\xab\xd9\x88\xd8\xb1\xd9\x8a",
        // "خادم لغة ص الثوري"
        "2.0.0"
    };
}

} // namespace lsp
} // namespace sad
