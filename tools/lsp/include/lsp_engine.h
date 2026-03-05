// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: lsp_engine.h
// الوصف: المحرك الأساسي لخادم LSP الثوري
// ══════════════════════════════════════════════════════════════════════════════
//
// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║                    البنية المعمارية الثورية                              ║
// ╠═══════════════════════════════════════════════════════════════════════════╣
// ║                                                                         ║
// ║  ┌────────────────────────────────────────────────────────────────────┐  ║
// ║  │                   طبقة النقل (Transport)                          │  ║
// ║  │              JSON-RPC عبر stdin/stdout                            │  ║
// ║  └────────────────────┬───────────────────────────────────────────────┘  ║
// ║                       │                                                 ║
// ║  ┌────────────────────▼───────────────────────────────────────────────┐  ║
// ║  │               المحرك الأساسي (LSP Engine)                         │  ║
// ║  │   ┌─────────────┐ ┌─────────────┐ ┌──────────────────────────┐    │  ║
// ║  │   │ مخزن        │ │ خط أنابيب  │ │ فهرس الرموز             │    │  ║
// ║  │   │ المستندات   │ │ التحليل    │ │ (Symbol Index)          │    │  ║
// ║  │   │ (DocStore) │ │ (Pipeline) │ │                          │    │  ║
// ║  │   └──────┬──────┘ └──────┬──────┘ └────────────┬─────────────┘    │  ║
// ║  │          │               │                     │                  │  ║
// ║  │   ┌──────▼───────────────▼─────────────────────▼─────────────┐    │  ║
// ║  │   │                  متتبع النطاقات (Scope Tracker)          │    │  ║
// ║  │   │  يتتبع: الدوال، المتغيرات، الأصناف، الوحدات، النطاقات   │    │  ║
// ║  │   └──────────────────────────────────────────────────────────┘    │  ║
// ║  └────────────────────┬───────────────────────────────────────────────┘  ║
// ║                       │                                                 ║
// ║  ┌────────────────────▼───────────────────────────────────────────────┐  ║
// ║  │                   المزودات (Providers)                            │  ║
// ║  │  ┌──────┐┌──────┐┌──────┐┌──────┐┌──────┐┌──────┐┌──────┐        │  ║
// ║  │  │إكمال ││تشخيص ││تلميح ││تعريف ││مراجع ││رموز  ││تنسيق │        │  ║
// ║  │  └──────┘└──────┘└──────┘└──────┘└──────┘└──────┘└──────┘        │  ║
// ║  │  ┌──────┐┌──────┐┌──────┐┌──────┐┌──────┐┌──────┐               │  ║
// ║  │  │دلالي ││توقيع ││تسمية ││مضمن  ││طي    ││إجراء │               │  ║
// ║  │  └──────┘└──────┘└──────┘└──────┘└──────┘└──────┘               │  ║
// ║  └────────────────────────────────────────────────────────────────────┘  ║
// ║                                                                         ║
// ╚═══════════════════════════════════════════════════════════════════════════╝
//
// المميزات الثورية:
//   ① تحليل حقيقي باستخدام محلل لغة ص الفعلي (Lexer + Parser)
//   ② دعم عربي أصيل: تجاهل التشكيل، بحث ضبابي، BiDi
//   ③ فهرسة رموز عبر الملفات مع تتبع النطاقات
//   ④ رموز دلالية تدعم التلوين الذكي للنحو العربي
//   ⑤ تلميحات مضمنة لعرض أنواع البيانات المستنتجة
//   ⑥ بنية قابلة للتوسيع بمزودات مستقلة
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "lsp_protocol_types.h"
#include "arabic_utils.h"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>
#include <mutex>
#include <shared_mutex>
#include <functional>
#include <chrono>
#include <thread>
#include <condition_variable>

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  تعريفات أمامية
// ══════════════════════════════════════════════════════════════════════════════
class DocumentStore;
class AnalysisPipeline;
class SymbolIndex;
class ScopeTracker;

// ══════════════════════════════════════════════════════════════════════════════
//  معلومات رمز مفهرس - تمثل دالة/متغير/صنف/إلخ تم تحليله من الكود
// ══════════════════════════════════════════════════════════════════════════════

/// نوع الرمز المحلل
enum class AnalyzedSymbolKind {
    Variable,       // متغير
    Constant,       // ثابت
    Function,       // دالة
    Method,         // طريقة (دالة داخل صنف)
    Class,          // صنف
    Property,       // خاصية
    Parameter,      // معلمة دالة
    Enum,           // تعداد
    Module,         // وحدة
    Import,         // استيراد
    Constructor,    // منشئ
    Interface,      // واجهة
    Struct          // بنية
};

/// معلومات نوع بيانات
struct TypeInfo {
    std::string name;                // اسم النوع (مثل "رقم", "نص", "قائمة[رقم]")
    bool is_inferred = false;        // هل تم استنتاج النوع تلقائياً؟
    bool is_nullable = false;        // هل يمكن أن يكون عدم؟
    std::vector<TypeInfo> type_args; // معلمات النوع (للأنواع المعممة)
};

/// معلومات رمز واحد مفهرس
struct AnalyzedSymbol {
    std::string name;                // الاسم (مثل "حساب_المجموع")
    std::string normalized_name;     // الاسم المطبّع (بدون تشكيل)
    AnalyzedSymbolKind kind;         // النوع
    TypeInfo type;                   // نوع البيانات
    DocumentUri uri;                 // ملف التعريف
    Range definition_range;          // نطاق التعريف الكامل
    Range name_range;                // نطاق الاسم فقط
    std::string documentation;       // التوثيق (من تعليقات ##)
    bool is_exported = false;        // هل مصدّر؟
    bool is_builtin = false;         // هل دالة مدمجة؟

    // معلومات إضافية للدوال
    struct FunctionInfo {
        std::vector<std::pair<std::string, TypeInfo>> parameters; // المعلمات
        TypeInfo return_type;                                      // نوع الإرجاع
        bool is_async = false;                                     // غير متزامن؟
        bool is_generator = false;                                 // مولد؟
    };
    std::optional<FunctionInfo> func_info; // معلومات الدالة (إذا كان دالة)

    // معلومات إضافية للأصناف
    struct ClassInfo {
        std::string parent_class;               // الصنف الأب
        std::vector<std::string> interfaces;    // الواجهات المنفذة
        std::vector<std::string> members;       // الأعضاء
    };
    std::optional<ClassInfo> class_info; // معلومات الصنف (إذا كان صنف)

    // نطاق الرمز (أين يكون مرئياً)
    int scope_depth = 0;             // عمق النطاق
    std::string scope_owner;         // الرمز الذي يملك هذا النطاق
};

/// مرجع لاستخدام رمز (ليس التعريف)
struct SymbolReference {
    DocumentUri uri;                 // الملف
    Range range;                     // النطاق
    std::string name;                // اسم الرمز المرجعي
    bool is_write = false;           // هل هو كتابة (تعديل)؟
    bool is_declaration = false;     // هل هو تعريف؟
};

// ══════════════════════════════════════════════════════════════════════════════
//  مخزن المستندات - يدير جميع المستندات المفتوحة
// ══════════════════════════════════════════════════════════════════════════════

/// معلومات مستند مفتوح
struct OpenDocument {
    DocumentUri uri;                             // عنوان المستند
    std::string content;                         // المحتوى الكامل
    int version = 0;                             // رقم الإصدار
    std::vector<std::string> lines;              // الأسطر (محسوبة مسبقاً)
    std::vector<AnalyzedSymbol> symbols;         // الرموز المحللة
    std::vector<Diagnostic> diagnostics;         // التشخيصات
    std::vector<SymbolReference> references;     // المراجع
    bool needs_reanalysis = true;                // يحتاج إعادة تحليل؟
    std::chrono::steady_clock::time_point last_change; // وقت آخر تعديل
};

/// مخزن المستندات - يدير دورة حياة المستندات المفتوحة
/// آمن للاستخدام من عدة خيوط (thread-safe)
class DocumentStore {
public:
    DocumentStore();
    ~DocumentStore();

    /// فتح مستند جديد
    void open(const TextDocumentItem& item);

    /// تحديث مستند (تغييرات تدريجية)
    void update(const DocumentUri& uri,
                const std::vector<TextDocumentContentChangeEvent>& changes,
                int version);

    /// إغلاق مستند
    void close(const DocumentUri& uri);

    /// الحصول على مستند (nullptr إذا غير موجود)
    std::shared_ptr<OpenDocument> get(const DocumentUri& uri) const;

    /// الحصول على جميع المستندات المفتوحة
    std::vector<DocumentUri> get_all_uris() const;

    /// هل المستند مفتوح؟
    bool is_open(const DocumentUri& uri) const;

    /// الحصول على السطر عند رقم سطر معين
    std::string get_line(const DocumentUri& uri, int line) const;

    /// الحصول على الكلمة عند موضع
    std::string get_word_at(const DocumentUri& uri, const Position& pos) const;

    /// تحويل موضع LSP إلى إزاحة بايت
    int position_to_offset(const DocumentUri& uri, const Position& pos) const;

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<DocumentUri, std::shared_ptr<OpenDocument>> documents_;

    /// تحديث مصفوفة الأسطر بعد تغيير المحتوى
    void recalculate_lines(OpenDocument& doc);

    /// تطبيق تغيير تدريجي على المحتوى
    void apply_incremental_change(OpenDocument& doc,
                                   const Range& range,
                                   const std::string& new_text);
};

// ══════════════════════════════════════════════════════════════════════════════
//  خط أنابيب التحليل - يشغل Lexer → Parser → SemanticAnalysis
// ══════════════════════════════════════════════════════════════════════════════

/// خط أنابيب التحليل: يأخذ نص الكود ويُنتج رموزاً محللة وتشخيصات
class AnalysisPipeline {
public:
    AnalysisPipeline();
    ~AnalysisPipeline();

    /// تحليل مستند كامل:
    ///   1. التحليل المعجمي (Lexer) → رموز
    ///   2. التحليل النحوي (Parser) → شجرة AST
    ///   3. جمع الرموز (الدوال، المتغيرات، الأصناف)
    ///   4. جمع التشخيصات (أخطاء + تحذيرات)
    struct AnalysisResult {
        std::vector<AnalyzedSymbol> symbols;     // الرموز المكتشفة
        std::vector<Diagnostic> diagnostics;     // التشخيصات
        std::vector<SymbolReference> references; // المراجع
        bool success = false;                    // هل نجح التحليل؟
    };

    AnalysisResult analyze(const std::string& content, const DocumentUri& uri);

private:
    /// جمع الرموز من شجرة AST
    void collect_symbols_from_ast(/*AST::StmtList& ast,*/
                                   const std::string& content,
                                   const DocumentUri& uri,
                                   AnalysisResult& result);

    /// اكتشاف التشخيصات الإضافية (تحذيرات، اقتراحات)
    void collect_extra_diagnostics(const std::string& content,
                                    const DocumentUri& uri,
                                    AnalysisResult& result);
};

// ══════════════════════════════════════════════════════════════════════════════
//  فهرس الرموز - يفهرس كل الرموز عبر الملفات
// ══════════════════════════════════════════════════════════════════════════════

/// فهرس الرموز: يتيح البحث السريع عن تعريفات ومراجع الرموز
class SymbolIndex {
public:
    SymbolIndex();
    ~SymbolIndex();

    /// تحديث رموز مستند (يحذف القديمة ويضيف الجديدة)
    void update_document(const DocumentUri& uri,
                          const std::vector<AnalyzedSymbol>& symbols,
                          const std::vector<SymbolReference>& references);

    /// حذف رموز مستند
    void remove_document(const DocumentUri& uri);

    /// البحث عن تعريف رمز بالاسم
    std::optional<AnalyzedSymbol> find_definition(const std::string& name,
                                                    const DocumentUri& context_uri,
                                                    const Position& context_pos) const;

    /// البحث عن كل مراجع رمز
    std::vector<SymbolReference> find_references(const std::string& name,
                                                   const DocumentUri& context_uri) const;

    /// البحث الضبابي (للإكمال التلقائي) - يدعم التشكيل والألف
    std::vector<AnalyzedSymbol> fuzzy_search(const std::string& prefix,
                                               const DocumentUri& context_uri,
                                               int max_results = 50) const;

    /// الحصول على كل الرموز في مستند معين
    std::vector<AnalyzedSymbol> get_document_symbols(const DocumentUri& uri) const;

    /// الحصول على كل الرموز في مساحة العمل
    std::vector<AnalyzedSymbol> get_workspace_symbols(const std::string& query = "") const;

    /// إضافة الدوال المدمجة (اطبع، ادخل، طول، إلخ)
    void register_builtins();

private:
    mutable std::shared_mutex mutex_;

    /// رموز كل مستند
    std::unordered_map<DocumentUri, std::vector<AnalyzedSymbol>> doc_symbols_;

    /// مراجع كل مستند
    std::unordered_map<DocumentUri, std::vector<SymbolReference>> doc_references_;

    /// الدوال المدمجة
    std::vector<AnalyzedSymbol> builtins_;

    /// فهرس عكسي: اسم → مواقع التعريف
    std::unordered_map<std::string, std::vector<AnalyzedSymbol*>> name_index_;

    /// إعادة بناء الفهرس العكسي
    void rebuild_name_index();
};

// ══════════════════════════════════════════════════════════════════════════════
//  متتبع النطاقات - يتتبع نطاق كل رمز
// ══════════════════════════════════════════════════════════════════════════════

/// حدود نطاق واحد (دالة، حلقة، شرط، صنف)
struct ScopeInfo {
    std::string name;                    // اسم النطاق (اسم الدالة مثلاً)
    AnalyzedSymbolKind kind;             // نوع النطاق
    Range range;                         // النطاق في الملف
    int depth;                           // العمق
    std::vector<std::string> variables;  // المتغيرات المعرفة في هذا النطاق
};

/// متتبع النطاقات: يحدد أي متغيرات مرئية في أي موضع
class ScopeTracker {
public:
    ScopeTracker();
    ~ScopeTracker();

    /// بناء شجرة النطاقات من رموز المستند
    void build(const std::vector<AnalyzedSymbol>& symbols);

    /// الحصول على النطاق النشط عند موضع معين
    std::optional<ScopeInfo> get_scope_at(const Position& pos) const;

    /// الحصول على كل المتغيرات المرئية عند موضع
    std::vector<AnalyzedSymbol> get_visible_symbols(
        const Position& pos,
        const std::vector<AnalyzedSymbol>& all_symbols) const;

    /// الحصول على جميع النطاقات
    const std::vector<ScopeInfo>& get_all_scopes() const { return scopes_; }

private:
    std::vector<ScopeInfo> scopes_;
};

// ══════════════════════════════════════════════════════════════════════════════
//  المحرك الرئيسي - ينظم كل شيء
// ══════════════════════════════════════════════════════════════════════════════

/// دالة نشر التشخيصات (يتم تعيينها من طبقة النقل)
using DiagnosticsPublisher = std::function<void(const DocumentUri&,
                                                 const std::vector<Diagnostic>&)>;

/// المحرك الرئيسي لخادم LSP
/// هذا هو القلب النابض - يربط بين كل المكونات
class LspEngine {
public:
    LspEngine();
    ~LspEngine();

    // ──────────────────── دورة الحياة ────────────────────
    /// تهيئة المحرك وإرجاع قدرات الخادم
    ServerCapabilities initialize(const std::string& root_uri = "");

    /// إيقاف المحرك
    void shutdown();

    // ──────────────────── مزامنة المستندات ────────────────────
    void did_open(const TextDocumentItem& item);
    void did_change(const DocumentUri& uri,
                    const std::vector<TextDocumentContentChangeEvent>& changes,
                    int version);
    void did_close(const DocumentUri& uri);

    // ──────────────────── ميزات اللغة ────────────────────
    CompletionList completion(const DocumentUri& uri, const Position& pos);
    std::optional<Hover> hover(const DocumentUri& uri, const Position& pos);
    std::optional<Location> definition(const DocumentUri& uri, const Position& pos);
    std::vector<Location> references(const DocumentUri& uri, const Position& pos);
    std::vector<DocumentSymbol> document_symbols(const DocumentUri& uri);
    SignatureHelp signature_help(const DocumentUri& uri, const Position& pos);
    SemanticTokensData semantic_tokens_full(const DocumentUri& uri);
    SemanticTokensData semantic_tokens_range(const DocumentUri& uri, const Range& range);
    std::optional<PrepareRenameResult> prepare_rename(const DocumentUri& uri, const Position& pos);
    WorkspaceEdit rename(const DocumentUri& uri, const Position& pos, const std::string& new_name);
    std::vector<InlayHint> inlay_hints(const DocumentUri& uri, const Range& range);
    std::vector<FoldingRange> folding_ranges(const DocumentUri& uri);
    std::vector<CodeAction> code_actions(const DocumentUri& uri, const Range& range,
                                          const std::vector<Diagnostic>& diagnostics);
    std::vector<TextEdit> format_document(const DocumentUri& uri, const FormattingOptions& options);
    std::vector<TextEdit> format_range(const DocumentUri& uri, const Range& range,
                                        const FormattingOptions& options);
    std::vector<SymbolInformation> workspace_symbols(const std::string& query);
    std::vector<DocumentHighlight> document_highlights(const DocumentUri& uri, const Position& pos);

    // ──────────────────── ميزات ثورية جديدة ────────────────────
    /// عدسات الكود: عرض عدد المراجع والاستدعاءات فوق كل دالة/صنف
    std::vector<CodeLens> code_lens(const DocumentUri& uri);

    /// نطاق التحديد الذكي: توسيع التحديد (كلمة → تعبير → جملة → كتلة → دالة)
    std::vector<SelectionRange> selection_range(const DocumentUri& uri,
                                                 const std::vector<Position>& positions);

    /// شجرة الاستدعاءات: تحضير عنصر من الشجرة
    std::vector<CallHierarchyItem> call_hierarchy_prepare(const DocumentUri& uri,
                                                           const Position& pos);

    /// شجرة الاستدعاءات: من يستدعي هذه الدالة؟
    std::vector<CallHierarchyIncomingCall> call_hierarchy_incoming(
        const CallHierarchyItem& item);

    /// شجرة الاستدعاءات: من تستدعيه هذه الدالة؟
    std::vector<CallHierarchyOutgoingCall> call_hierarchy_outgoing(
        const CallHierarchyItem& item);

    /// شجرة الأنواع: تحضير عنصر
    std::vector<TypeHierarchyItem> type_hierarchy_prepare(
        const DocumentUri& uri, const Position& pos);

    /// شجرة الأنواع: الأنواع الفوقية (supertypes — الآباء والسمات المنفذة)
    std::vector<TypeHierarchyItem> type_hierarchy_supertypes(
        const TypeHierarchyItem& item);

    /// شجرة الأنواع: الأنواع الفرعية (subtypes — الأبناء والمنفذون)
    std::vector<TypeHierarchyItem> type_hierarchy_subtypes(
        const TypeHierarchyItem& item);

    /// روابط المستند: جعل مسارات الاستيراد قابلة للنقر
    std::vector<DocumentLink> document_links(const DocumentUri& uri);

    /// التنسيق أثناء الكتابة: تنسيق فوري عند إدخال حرف معين
    std::vector<TextEdit> on_type_formatting(const DocumentUri& uri,
                                              const Position& pos,
                                              const std::string& ch,
                                              const FormattingOptions& options);

    // ──────────────────── إعدادات ────────────────────
    /// تعيين دالة نشر التشخيصات
    void set_diagnostics_publisher(DiagnosticsPublisher publisher);

    /// الحصول على معلومات الخادم
    ServerInfo get_server_info() const;

private:
    std::unique_ptr<DocumentStore> doc_store_;
    std::unique_ptr<AnalysisPipeline> pipeline_;
    std::unique_ptr<SymbolIndex> index_;
    DiagnosticsPublisher diagnostics_publisher_;
    std::string root_uri_;

    // ──── آلية التأخير (Debouncing) ────
    // تمنع إعادة التحليل عند كل ضغطة مفتاح
    // تنتظر 200ms بعد آخر تغيير قبل التحليل
    mutable std::mutex debounce_mutex_;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> pending_analysis_;
    std::unique_ptr<std::thread> debounce_thread_;
    std::condition_variable debounce_cv_;
    bool debounce_running_ = false;
    static constexpr int DEBOUNCE_MS = 200;

    /// تحليل مستند ونشر النتائج
    void analyze_and_publish(const DocumentUri& uri);

    /// بدء خيط التأخير
    void start_debounce_thread();

    /// جدولة تحليل مؤجل
    void schedule_analysis(const DocumentUri& uri);
};

} // namespace lsp
} // namespace sad
