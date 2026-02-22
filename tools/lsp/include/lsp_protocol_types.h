// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: lsp_protocol_types.h
// الوصف: جميع أنواع بروتوكول LSP الكاملة
// ══════════════════════════════════════════════════════════════════════════════
//
// هذا الملف يحتوي على التعريفات الكاملة لبروتوكول خادم اللغة (LSP 3.17)
// وفقاً للمواصفات الرسمية من مايكروسوفت.
//
// يشمل جميع الأنواع اللازمة لدعم كل ميزات المحرر:
//   ✦ المزامنة (Sync)           - فتح/تعديل/إغلاق المستندات
//   ✦ التشخيصات (Diagnostics)   - أخطاء وتحذيرات فورية
//   ✦ الإكمال (Completion)       - اقتراحات ذكية أثناء الكتابة
//   ✦ التلميحات (Hover)          - معلومات عند التمرير
//   ✦ التعريف (Definition)       - انتقال لمكان التعريف
//   ✦ المراجع (References)       - إيجاد كل الاستخدامات
//   ✦ الرموز الدلالية (Semantic) - تلوين ذكي للنحو العربي
//   ✦ توقيعات الدوال (Signature) - معلمات الدالة أثناء الكتابة
//   ✦ إعادة التسمية (Rename)     - تغيير اسم رمز في كل الأماكن
//   ✦ تلميحات مضمنة (Inlay)      - أنواع المتغيرات المستنتجة
//   ✦ الطي (Folding)             - طي/فتح كتل الكود
//   ✦ إجراءات الكود (Actions)     - إصلاحات سريعة
//   ✦ التنسيق (Formatting)       - تنسيق الكود تلقائياً
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#pragma once

#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <memory>
#include <unordered_map>
#include <functional>

namespace sad {
namespace lsp {

// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║                    الأنواع الأساسية - Basic Types                       ║
// ╚═══════════════════════════════════════════════════════════════════════════╝

/// معرّف المستند (URI) - يحدد ملفاً بشكل فريد
using DocumentUri = std::string;

/// معرّف الطلب - يمكن أن يكون رقماً أو نصاً
using RequestId = std::variant<int, std::string>;

/// موضع في مستند نصي (سطر + عمود) - يبدأ من الصفر
struct Position {
    int line = 0;         // رقم السطر (يبدأ من 0)
    int character = 0;    // رقم العمود بوحدات UTF-16

    bool operator==(const Position& o) const { return line == o.line && character == o.character; }
    bool operator!=(const Position& o) const { return !(*this == o); }
    bool operator<(const Position& o) const {
        return line < o.line || (line == o.line && character < o.character);
    }
    bool operator<=(const Position& o) const { return *this == o || *this < o; }
    bool operator>(const Position& o) const { return !(*this <= o); }
    bool operator>=(const Position& o) const { return !(*this < o); }
};

/// نطاق بين موضعين في المستند
struct Range {
    Position start;   // بداية النطاق
    Position end;     // نهاية النطاق (حصرية)

    bool contains(const Position& p) const { return start <= p && p < end; }
    bool is_empty() const { return start == end; }
    bool overlaps(const Range& o) const { return start < o.end && o.start < end; }
};

/// موقع في مستند = URI + نطاق
struct Location {
    DocumentUri uri;
    Range range;
};

/// رابط لموقع - مثل Location لكن مع نطاق الأصل أيضاً
struct LocationLink {
    std::optional<Range> origin_selection_range; // نطاق الأصل (اختياري)
    DocumentUri target_uri;
    Range target_range;
    Range target_selection_range;
};

// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║                 أنواع المستندات - Document Types                        ║
// ╚═══════════════════════════════════════════════════════════════════════════╝

/// معرّف مستند نصي
struct TextDocumentIdentifier {
    DocumentUri uri;
};

/// معرّف مستند مع رقم إصدار
struct VersionedTextDocumentIdentifier {
    DocumentUri uri;
    int version;
};

/// عنصر مستند نصي (عند الفتح)
struct TextDocumentItem {
    DocumentUri uri;
    std::string language_id;
    int version;
    std::string text;
};

/// حدث تغيير في المحتوى
struct TextDocumentContentChangeEvent {
    std::optional<Range> range;          // النطاق المتغير (أو nullopt للمستند كاملاً)
    std::optional<int> range_length;     // طول النطاق (مهمل)
    std::string text;                    // النص الجديد
};

/// تعديل نصي
struct TextEdit {
    Range range;
    std::string new_text;
};

// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║               التشخيصات - Diagnostics                                   ║
// ╚═══════════════════════════════════════════════════════════════════════════╝

/// شدة التشخيص
enum class DiagnosticSeverity {
    Error = 1,         // خطأ فادح
    Warning = 2,       // تحذير
    Information = 3,   // معلومة
    Hint = 4           // تلميح
};

/// وسم التشخيص (مهمل، غير ضروري)
enum class DiagnosticTag {
    Unnecessary = 1,   // كود غير ضروري (يظهر باهتاً)
    Deprecated = 2     // مهمل (يظهر بخط يتوسطه)
};

/// معلومات إضافية مرتبطة بالتشخيص
struct DiagnosticRelatedInformation {
    Location location;
    std::string message;
};

/// رسالة تشخيص (خطأ/تحذير/تلميح)
struct Diagnostic {
    Range range;                                            // النطاق المتأثر
    DiagnosticSeverity severity = DiagnosticSeverity::Error; // الشدة
    std::string code;                                       // رمز الخطأ (مثل "ص-001")
    std::string source = "sad";                             // المصدر
    std::string message;                                    // الرسالة (إنجليزي)
    std::string message_ar;                                 // الرسالة (عربي)
    std::vector<DiagnosticTag> tags;                         // الوسوم
    std::vector<DiagnosticRelatedInformation> related;       // معلومات مرتبطة
};

// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║               الإكمال التلقائي - Completion                             ║
// ╚═══════════════════════════════════════════════════════════════════════════╝

/// نوع عنصر الإكمال
enum class CompletionItemKind {
    Text = 1, Method = 2, Function = 3, Constructor = 4,
    Field = 5, Variable = 6, Class = 7, Interface = 8,
    Module = 9, Property = 10, Unit = 11, Value = 12,
    Enum = 13, Keyword = 14, Snippet = 15, Color = 16,
    File = 17, Reference = 18, Folder = 19, EnumMember = 20,
    Constant = 21, Struct = 22, Event = 23, Operator = 24,
    TypeParameter = 25
};

/// تنسيق النص المدرج
enum class InsertTextFormat {
    PlainText = 1,   // نص عادي
    Snippet = 2      // مقطع كود (يحتوي ${1:placeholder})
};

/// محتوى منسق (Markdown أو نص عادي)
struct MarkupContent {
    std::string kind;    // "plaintext" أو "markdown"
    std::string value;   // المحتوى
};

/// عنصر إكمال تلقائي واحد
struct CompletionItem {
    std::string label;                              // النص المعروض
    CompletionItemKind kind = CompletionItemKind::Text;
    std::string detail;                             // تفاصيل مختصرة
    MarkupContent documentation;                    // التوثيق الكامل
    bool deprecated = false;                        // هل مهمل؟
    bool preselect = false;                         // هل محدد مسبقاً؟
    std::string sort_text;                          // نص الترتيب
    std::string filter_text;                        // نص الفلترة
    std::string insert_text;                        // النص المدرج
    InsertTextFormat insert_text_format = InsertTextFormat::PlainText;
    std::optional<TextEdit> text_edit;              // تعديل نصي بديل
    std::vector<TextEdit> additional_text_edits;    // تعديلات إضافية (مثل إضافة استيراد)
    std::vector<std::string> commit_characters;     // أحرف التأكيد
};

/// قائمة الإكمال التلقائي
struct CompletionList {
    bool is_incomplete = false;              // هل القائمة غير مكتملة؟
    std::vector<CompletionItem> items;       // العناصر
};

// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║               التلميحات - Hover                                         ║
// ╚═══════════════════════════════════════════════════════════════════════════╝

/// معلومات التلميح عند التمرير فوق عنصر
struct Hover {
    MarkupContent contents;              // المحتوى المعروض
    std::optional<Range> range;          // نطاق العنصر
};

// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║               توقيعات الدوال - Signature Help                           ║
// ╚═══════════════════════════════════════════════════════════════════════════╝

/// معلومات معلمة واحدة
struct ParameterInformation {
    std::string label;                   // اسم المعلمة
    std::string documentation;           // وصف المعلمة
};

/// توقيع دالة واحد
struct SignatureInformation {
    std::string label;                              // التوقيع الكامل
    std::string documentation;                      // الوصف
    std::vector<ParameterInformation> parameters;   // المعلمات
    std::optional<int> active_parameter;             // المعلمة النشطة
};

/// معلومات توقيعات الدوال
struct SignatureHelp {
    std::vector<SignatureInformation> signatures;    // التوقيعات المتاحة
    int active_signature = 0;                        // التوقيع النشط
    int active_parameter = 0;                        // المعلمة النشطة
};

// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║               رموز المستند - Document Symbols                           ║
// ╚═══════════════════════════════════════════════════════════════════════════╝

/// نوع الرمز
enum class SymbolKind {
    File = 1, Module = 2, Namespace = 3, Package = 4,
    Class = 5, Method = 6, Property = 7, Field = 8,
    Constructor = 9, Enum = 10, Interface = 11, Function = 12,
    Variable = 13, Constant = 14, String = 15, Number = 16,
    Boolean = 17, Array = 18, Object = 19, Key = 20,
    Null = 21, EnumMember = 22, Struct = 23, Event = 24,
    Operator = 25, TypeParameter = 26
};

/// نوع تمييز المستند (قراءة / كتابة / نص)
enum class DocumentHighlightKind {
    Text = 1,
    Read = 2,
    Write = 3
};

/// تمييز رمز في المستند
struct DocumentHighlight {
    Range range;
    DocumentHighlightKind kind = DocumentHighlightKind::Text;
};

/// رمز في المستند (تدعم التداخل/الأبناء)
struct DocumentSymbol {
    std::string name;                           // اسم الرمز
    std::string detail;                         // تفاصيل (مثل نوع الإرجاع)
    SymbolKind kind;                            // النوع
    bool deprecated = false;                    // هل مهمل؟
    Range range;                                // النطاق الكامل
    Range selection_range;                      // نطاق الاسم فقط
    std::vector<DocumentSymbol> children;       // رموز فرعية
};

/// رمز في مساحة العمل (workspace/symbol)
struct SymbolInformation {
    std::string name;                           // اسم الرمز
    SymbolKind kind;                            // النوع
    Location location;                          // الموقع (ملف + نطاق)
    std::string container_name;                 // اسم الحاوية (الصنف أو الوحدة)
};

// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║            الرموز الدلالية - Semantic Tokens                            ║
// ╚═══════════════════════════════════════════════════════════════════════════╝
//
// الرموز الدلالية توفر تلويناً ذكياً يتجاوز التلوين النحوي التقليدي.
// بالنسبة للغة ص، هذا يعني:
//   - تلوين الكلمات المفتاحية العربية بشكل صحيح
//   - تمييز الدوال المدمجة عن المعرفة من المستخدم
//   - تلوين أنواع البيانات العربية
//   - تمييز الثوابت عن المتغيرات
//

/// أنواع الرموز الدلالية المدعومة
/// ترتيبها مهم - يتوافق مع الفهرس المرسل للمحرر
enum class SemanticTokenType {
    Namespace = 0,    // فضاء الأسماء
    Type = 1,         // نوع بيانات
    Class = 2,        // صنف
    Enum = 3,         // تعداد
    Interface = 4,    // واجهة
    Struct = 5,       // بنية
    TypeParameter = 6,// معلمة نوع
    Parameter = 7,    // معلمة دالة
    Variable = 8,     // متغير
    Property = 9,     // خاصية
    EnumMember = 10,  // عضو تعداد
    Event = 11,       // حدث
    Function = 12,    // دالة
    Method = 13,      // طريقة
    Macro = 14,       // ماكرو
    Keyword = 15,     // كلمة مفتاحية
    Modifier = 16,    // معدّل (عام، خاص، محمي)
    Comment = 17,     // تعليق
    String = 18,      // نص
    Number = 19,      // رقم
    Regexp = 20,      // تعبير نظامي
    Operator = 21,    // عامل
    Decorator = 22    // مزخرف (@)
};

/// معدلات الرموز الدلالية (يمكن الجمع بينها)
enum class SemanticTokenModifier {
    Declaration = 0,      // تعريف
    Definition = 1,       // تنفيذ
    Readonly = 2,         // للقراءة فقط (ثابت)
    Static = 3,           // ساكن
    Deprecated = 4,       // مهمل
    Abstract = 5,         // مجرد
    Async = 6,            // غير متزامن
    Modification = 7,     // تعديل على المتغير
    Documentation = 8,    // توثيق
    DefaultLibrary = 9    // مكتبة قياسية
};

/// بيانات الرموز الدلالية (مشفرة كمصفوفة أرقام)
struct SemanticTokensData {
    std::vector<uint32_t> data;  // [deltaLine, deltaStart, length, tokenType, tokenModifiers]...
};

// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║            إعادة التسمية - Rename                                        ║
// ╚═══════════════════════════════════════════════════════════════════════════╝

/// نتيجة التحقق من صلاحية إعادة التسمية
struct PrepareRenameResult {
    Range range;                 // نطاق الرمز القابل للتسمية
    std::string placeholder;     // الاسم الحالي
};

/// تعديل مساحة العمل (عدة ملفات)
struct WorkspaceEdit {
    std::unordered_map<DocumentUri, std::vector<TextEdit>> changes;
};

// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║            التلميحات المضمنة - Inlay Hints                              ║
// ╚═══════════════════════════════════════════════════════════════════════════╝
//
// التلميحات المضمنة تعرض معلومات إضافية داخل الكود مثل:
//   - أنواع المتغيرات المستنتجة: متغير س = 5  →  متغير س: رقم = 5
//   - أسماء المعلمات: دالة(42, "أحمد")  →  دالة(عمر: 42, اسم: "أحمد")
//

/// نوع التلميح المضمن
enum class InlayHintKind {
    Type = 1,       // نوع البيانات
    Parameter = 2   // اسم المعلمة
};

/// تلميح مضمن واحد
struct InlayHint {
    Position position;             // الموضع
    std::string label;             // النص المعروض
    InlayHintKind kind;            // النوع
    bool padding_left = false;     // مسافة قبل
    bool padding_right = false;    // مسافة بعد
};

// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║            الطي - Folding Ranges                                        ║
// ╚═══════════════════════════════════════════════════════════════════════════╝

/// نوع الطي
enum class FoldingRangeKind {
    Comment,    // تعليق
    Imports,    // استيرادات
    Region      // منطقة (كتلة كود)
};

/// نطاق قابل للطي
struct FoldingRange {
    int start_line;                            // سطر البداية
    std::optional<int> start_character;        // عمود البداية (اختياري)
    int end_line;                              // سطر النهاية
    std::optional<int> end_character;          // عمود النهاية (اختياري)
    std::optional<FoldingRangeKind> kind;       // نوع الطي
    std::string collapsed_text;                // نص مختصر عند الطي
};

// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║            إجراءات الكود - Code Actions                                  ║
// ╚═══════════════════════════════════════════════════════════════════════════╝

/// نوع إجراء الكود
enum class CodeActionKind {
    QuickFix,           // إصلاح سريع
    Refactor,           // إعادة هيكلة
    RefactorExtract,    // استخراج
    RefactorInline,     // توسيع مباشر
    RefactorRewrite,    // إعادة كتابة
    Source,             // إجراء المصدر
    SourceOrganizeImports, // ترتيب الاستيرادات
    SourceFixAll        // إصلاح الكل
};

/// إجراء كود
struct CodeAction {
    std::string title;                          // العنوان
    CodeActionKind kind;                        // النوع
    std::vector<Diagnostic> diagnostics;        // التشخيصات المرتبطة
    bool is_preferred = false;                  // هل هو الإجراء المفضل؟
    std::optional<WorkspaceEdit> edit;          // التعديلات
};

// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║            التنسيق - Formatting                                          ║
// ╚═══════════════════════════════════════════════════════════════════════════╝

/// خيارات التنسيق
struct FormattingOptions {
    int tab_size = 4;
    bool insert_spaces = true;
    bool trim_trailing_whitespace = true;
    bool insert_final_newline = true;
    bool trim_final_newlines = true;
};

// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║        عدسات الكود - Code Lens                                           ║
// ╚═══════════════════════════════════════════════════════════════════════════╝
//
// عدسات الكود هي نصوص تفاعلية تظهر فوق الدوال والأصناف:
//   "٣ مراجع | ٢ استدعاء"   ← فوق كل دالة
//   "٥ أعضاء"               ← فوق كل صنف
// يمكن النقر عليها لتنفيذ أمر (مثل عرض المراجع)
//

/// أمر قابل للتنفيذ (مرتبط بعدسة الكود)
struct Command {
    std::string title;                           // النص المعروض
    std::string command;                         // معرّف الأمر
    std::vector<std::string> arguments;          // المعاملات
};

/// عدسة كود واحدة
struct CodeLens {
    Range range;                                 // الموضع (فوق السطر)
    std::optional<Command> command;              // الأمر عند النقر
};

// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║        نطاق التحديد الذكي - Selection Range                              ║
// ╚═══════════════════════════════════════════════════════════════════════════╝
//
// يمكّن المبرمج من توسيع التحديد بشكل ذكي:
//   كلمة → تعبير → جملة → كتلة → دالة → ملف
// عبر Alt+Shift+→ (أو Ctrl+Shift+→ على ماك)
//

/// نطاق تحديد ذكي (شجرة متداخلة)
struct SelectionRange {
    Range range;                                 // النطاق الحالي
    std::shared_ptr<SelectionRange> parent;      // النطاق الأوسع (الأب)
};

// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║        شجرة الاستدعاءات - Call Hierarchy                                  ║
// ╚═══════════════════════════════════════════════════════════════════════════╝
//
// تعرض من يستدعي هذه الدالة ومن تستدعيه:
//   Callers (من يستدعيني) ← Shift+Alt+H
//   Callees (من أستدعيه) ← الفرع الآخر
//

/// عنصر في شجرة الاستدعاءات
struct CallHierarchyItem {
    std::string name;                            // اسم الدالة
    SymbolKind kind;                             // النوع
    std::string detail;                          // تفاصيل (التوقيع)
    DocumentUri uri;                             // الملف
    Range range;                                 // النطاق الكامل
    Range selection_range;                       // نطاق الاسم
};

/// استدعاء وارد (من يستدعي هذه الدالة)
struct CallHierarchyIncomingCall {
    CallHierarchyItem from;                      // المستدعي
    std::vector<Range> from_ranges;              // مواقع الاستدعاء
};

/// استدعاء صادر (من تستدعيه هذه الدالة)
struct CallHierarchyOutgoingCall {
    CallHierarchyItem to;                        // المُستدعى
    std::vector<Range> from_ranges;              // مواقع الاستدعاء في جسم الدالة
};

// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║        روابط المستند - Document Links                                     ║
// ╚═══════════════════════════════════════════════════════════════════════════╝
//
// تجعل مسارات الاستيراد قابلة للنقر:
//   استورد "وحدتي/رياضيات"  ← يمكن Ctrl+Click للانتقال
//

/// رابط في المستند
struct DocumentLink {
    Range range;                                 // نطاق الرابط
    std::string target;                          // عنوان الهدف (URI)
    std::string tooltip;                         // التلميح عند التمرير
};

// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║            قدرات الخادم - Server Capabilities                            ║
// ╚═══════════════════════════════════════════════════════════════════════════╝

/// جميع القدرات التي يدعمها الخادم
struct ServerCapabilities {
    // مزامنة المستندات
    struct TextDocumentSyncOptions {
        bool open_close = true;
        int change = 2; // 0=None, 1=Full, 2=Incremental
    } text_document_sync;

    // الإكمال التلقائي
    struct CompletionOptions {
        std::vector<std::string> trigger_characters = {".", ":", "(", "\xd8\x8c"}; // . : ( ،
        bool resolve_provider = true;
    } completion_provider;

    // توقيعات الدوال
    struct SignatureHelpOptions {
        std::vector<std::string> trigger_characters = {"(", "\xd8\x8c", ","};  // ( ، ,
        std::vector<std::string> retrigger_characters = {",", "\xd8\x8c"};     // , ،
    } signature_help_provider;

    // التنسيق أثناء الكتابة
    struct DocumentOnTypeFormattingOptions {
        std::string first_trigger_character = "}";
        std::vector<std::string> more_trigger_characters = {"\n", ";"};
    } document_on_type_formatting_provider;

    bool hover_provider = true;
    bool definition_provider = true;
    bool declaration_provider = true;
    bool references_provider = true;
    bool document_highlight_provider = true;
    bool document_symbol_provider = true;
    bool workspace_symbol_provider = true;
    bool code_action_provider = true;
    bool document_formatting_provider = true;
    bool document_range_formatting_provider = true;
    bool rename_provider = true;
    bool selection_range_provider = true;             // نطاق التحديد الذكي
    bool code_lens_provider = true;                   // عدسات الكود
    bool call_hierarchy_provider = true;              // شجرة الاستدعاءات
    bool document_link_provider = true;               // روابط المستند
    bool document_on_type_formatting_provider_enabled = true; // تنسيق أثناء الكتابة

    // الرموز الدلالية
    struct SemanticTokensOptions {
        bool full = true;
        bool range = true;
        bool delta = false;
        std::vector<std::string> token_types;
        std::vector<std::string> token_modifiers;
    } semantic_tokens_provider;

    // التلميحات المضمنة
    bool inlay_hint_provider = true;

    // الطي
    bool folding_range_provider = true;
};

// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║            معلومات الخادم - Server Info                                  ║
// ╚═══════════════════════════════════════════════════════════════════════════╝

struct ServerInfo {
    std::string name = "\xd8\xae\xd8\xa7\xd8\xaf\xd9\x85 \xd9\x84\xd8\xba\xd8\xa9 \xd8\xb5 \xd8\xa7\xd9\x84\xd8\xab\xd9\x88\xd8\xb1\xd9\x8a"; // خادم لغة ص الثوري
    std::string version = "3.0.0";               // الإصدار
};

} // namespace lsp
} // namespace sad
