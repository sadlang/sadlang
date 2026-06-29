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
    Sad::Lexer::KeywordTable::initialize();

    // بدء خيط التأخير (debouncing)
    start_debounce_thread();
}

// ══════════════════════════════════════════════════════════════════════════════
//  المدمر: تنظيف الموارد
// ══════════════════════════════════════════════════════════════════════════════

LspEngine::~LspEngine() {
    // إيقاف خيط التأخير
    {
        std::lock_guard<std::mutex> lock(debounce_mutex_);
        debounce_running_ = false;
    }
    debounce_cv_.notify_all();
    if (debounce_thread_ && debounce_thread_->joinable()) {
        debounce_thread_->join();
    }
}

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
    caps.call_hierarchy_provider = true;

    // ──── شجرة الأنواع (Type Hierarchy) ────
    // إظهار شجرة الوراثة — الآباء والأبناء
    caps.type_hierarchy_provider = true;

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
    // ① تطبيق التغييرات على المستند (فوري)
    doc_store_->update(uri, changes, version);

    // ② جدولة إعادة التحليل بعد تأخير (debouncing)
    // هذا يمنع إعادة التحليل عند كل ضغطة مفتاح
    schedule_analysis(uri);
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
    ServerInfo info;
    info.name = "خادم لغة ص";
    info.version = "2.1.0";
    return info;
}

// ══════════════════════════════════════════════════════════════════════════════
//  آلية التأخير (Debouncing)
//  تمنع إعادة التحليل عند كل ضغطة مفتاح — تنتظر 200ms بعد آخر تغيير
// ══════════════════════════════════════════════════════════════════════════════

void LspEngine::schedule_analysis(const DocumentUri& uri) {
    {
        std::lock_guard<std::mutex> lock(debounce_mutex_);
        pending_analysis_[uri] = std::chrono::steady_clock::now();
    }
    debounce_cv_.notify_one();
}

void LspEngine::start_debounce_thread() {
    debounce_running_ = true;
    debounce_thread_ = std::make_unique<std::thread>([this]() {
        while (true) {
            std::vector<std::string> uris_to_analyze;

            {
                std::unique_lock<std::mutex> lock(debounce_mutex_);

                // انتظر حتى يصل طلب أو نتوقف
                debounce_cv_.wait(lock, [this]() {
                    return !debounce_running_ || !pending_analysis_.empty();
                });

                if (!debounce_running_) break;

                // انتظر DEBOUNCE_MS ثم تحقق من الطلبات المعلقة
                auto deadline = std::chrono::steady_clock::now() 
                    + std::chrono::milliseconds(DEBOUNCE_MS);

                debounce_cv_.wait_until(lock, deadline, [this]() {
                    return !debounce_running_;
                });

                if (!debounce_running_) break;

                // اجمع URIs التي مر عليها DEBOUNCE_MS منذ آخر تغيير
                auto now = std::chrono::steady_clock::now();
                for (auto it = pending_analysis_.begin(); it != pending_analysis_.end();) {
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now - it->second).count();
                    if (elapsed >= DEBOUNCE_MS) {
                        uris_to_analyze.push_back(it->first);
                        it = pending_analysis_.erase(it);
                    } else {
                        ++it;
                    }
                }
            }

            // حلل خارج القفل
            for (const auto& uri : uris_to_analyze) {
                try {
                    analyze_and_publish(uri);
                } catch (...) {
                    // تجنب انهيار الخيط بسبب استثناء غير متوقع
                }
            }
        }
    });
}

} // namespace lsp
} // namespace sad
