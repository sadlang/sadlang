// بسم الله الرحمن الرحيم
/**
 * @file lsp_types.h
 * @brief LSP Protocol Types - أنواع بروتوكول خادم اللغة
 * 
 * هذا الملف يحتوي على التعريفات الأساسية لبروتوكول LSP (Language Server Protocol)
 * لدعم محررات الأكواد مثل VS Code و Vim و Emacs وغيرها
 * 
 * This file contains the core LSP (Language Server Protocol) type definitions
 * to support code editors like VS Code, Vim, Emacs, and others
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#pragma once

#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <memory>
#include <unordered_map>

namespace sad {
namespace lsp {

// ============================================================================
// Basic Types - الأنواع الأساسية
// ============================================================================

/**
 * @brief Position in a text document - موضع في مستند نصي
 * 
 * يمثل موقع في المستند بالسطر والعمود (zero-based)
 * Represents a position in the document by line and character (zero-based)
 */
struct Position {
    int line = 0;        // رقم السطر (يبدأ من 0) / Line number (0-based)
    int character = 0;   // رقم العمود (يبدأ من 0) / Character offset (0-based)
    
    bool operator==(const Position& other) const {
        return line == other.line && character == other.character;
    }
    
    bool operator<(const Position& other) const {
        if (line != other.line) return line < other.line;
        return character < other.character;
    }
};

/**
 * @brief Range in a text document - نطاق في مستند نصي
 * 
 * يمثل نطاقاً بين موضعين في المستند
 * Represents a range between two positions in the document
 */
struct Range {
    Position start;  // بداية النطاق / Range start
    Position end;    // نهاية النطاق / Range end
    
    bool contains(const Position& pos) const {
        return !(pos < start) && pos < end;
    }
    
    bool is_empty() const {
        return start == end;
    }
};

/**
 * @brief Location in a document - موقع في مستند
 * 
 * يجمع بين URI المستند والنطاق
 * Combines document URI with a range
 */
struct Location {
    std::string uri;     // URI المستند / Document URI (file:///path/to/file)
    Range range;         // النطاق / Range in document
};

/**
 * @brief Diagnostic severity - شدة التشخيص
 */
enum class DiagnosticSeverity {
    ERROR = 1,       // خطأ / Error
    WARNING = 2,     // تحذير / Warning
    INFORMATION = 3, // معلومة / Information
    HINT = 4         // تلميح / Hint
};

/**
 * @brief Diagnostic message - رسالة تشخيص
 * 
 * تمثل خطأ أو تحذير في الكود
 * Represents an error or warning in the code
 */
struct Diagnostic {
    Range range;                           // النطاق المتأثر / Affected range
    DiagnosticSeverity severity;           // الشدة / Severity
    std::string code;                      // كود الخطأ / Error code
    std::string source;                    // المصدر (sad) / Source (sad)
    std::string message;                   // رسالة الخطأ / Error message
    std::optional<std::string> message_ar; // رسالة بالعربية / Arabic message
};

// ============================================================================
// Document Types - أنواع المستندات
// ============================================================================

/**
 * @brief Text document identifier - معرّف المستند النصي
 */
struct TextDocumentIdentifier {
    std::string uri;  // URI المستند / Document URI
};

/**
 * @brief Versioned text document - مستند نصي بإصدار
 */
struct VersionedTextDocumentIdentifier : TextDocumentIdentifier {
    int version;  // رقم الإصدار / Version number
};

/**
 * @brief Text document item - عنصر مستند نصي
 */
struct TextDocumentItem {
    std::string uri;          // URI المستند / Document URI
    std::string language_id;  // معرّف اللغة (sad) / Language ID (sad)
    int version;              // رقم الإصدار / Version number
    std::string text;         // محتوى المستند / Document content
};

/**
 * @brief Text document content change - تغيير في محتوى المستند
 */
struct TextDocumentContentChangeEvent {
    std::optional<Range> range;  // النطاق (أو nullopt للمستند كاملاً) / Range (or nullopt for full document)
    std::string text;            // النص الجديد / New text
};

// ============================================================================
// Completion Types - أنواع الإكمال التلقائي
// ============================================================================

/**
 * @brief Completion item kind - نوع عنصر الإكمال
 */
enum class CompletionItemKind {
    TEXT = 1,          // نص / Text
    METHOD = 2,        // دالة / Method
    FUNCTION = 3,      // دالة / Function
    CONSTRUCTOR = 4,   // منشئ / Constructor
    FIELD = 5,         // حقل / Field
    VARIABLE = 6,      // متغير / Variable
    CLASS = 7,         // صنف / Class
    INTERFACE = 8,     // واجهة / Interface
    MODULE = 9,        // وحدة / Module
    PROPERTY = 10,     // خاصية / Property
    UNIT = 11,         // وحدة / Unit
    VALUE = 12,        // قيمة / Value
    ENUM = 13,         // تعداد / Enum
    KEYWORD = 14,      // كلمة مفتاحية / Keyword
    SNIPPET = 15,      // مقطع / Snippet
    COLOR = 16,        // لون / Color
    FILE = 17,         // ملف / File
    REFERENCE = 18,    // مرجع / Reference
    FOLDER = 19,       // مجلد / Folder
    ENUM_MEMBER = 20,  // عضو تعداد / Enum member
    CONSTANT = 21,     // ثابت / Constant
    STRUCT = 22,       // بنية / Struct
    EVENT = 23,        // حدث / Event
    OPERATOR = 24,     // عامل / Operator
    TYPE_PARAMETER = 25 // معامل نوع / Type parameter
};

/**
 * @brief Completion item - عنصر إكمال تلقائي
 */
struct CompletionItem {
    std::string label;                      // العنوان / Label shown in completion list
    CompletionItemKind kind;                // نوع العنصر / Item kind
    std::optional<std::string> detail;      // تفاصيل / Detail string
    std::optional<std::string> documentation; // التوثيق / Documentation
    std::optional<std::string> insert_text; // النص المدرج / Text to insert
    std::optional<std::string> filter_text; // نص الفلترة / Text for filtering
    std::optional<std::string> sort_text;   // نص الترتيب / Text for sorting
};

/**
 * @brief Completion list - قائمة الإكمال التلقائي
 */
struct CompletionList {
    bool is_incomplete;                     // هل القائمة غير كاملة / Is list incomplete
    std::vector<CompletionItem> items;      // العناصر / Completion items
};

// ============================================================================
// Hover Types - أنواع التلميح
// ============================================================================

/**
 * @brief Markup kind - نوع التنسيق
 */
enum class MarkupKind {
    PLAINTEXT,  // نص عادي / Plain text
    MARKDOWN    // Markdown
};

/**
 * @brief Markup content - محتوى منسق
 */
struct MarkupContent {
    MarkupKind kind;     // نوع التنسيق / Markup kind
    std::string value;   // المحتوى / Content
};

/**
 * @brief Hover information - معلومات التلميح
 */
struct Hover {
    MarkupContent contents;          // المحتوى / Content
    std::optional<Range> range;      // النطاق / Range
};

// ============================================================================
// Symbol Types - أنواع الرموز
// ============================================================================

/**
 * @brief Symbol kind - نوع الرمز
 */
enum class SymbolKind {
    FILE = 1,          // ملف / File
    MODULE = 2,        // وحدة / Module
    NAMESPACE = 3,     // نطاق اسم / Namespace
    PACKAGE = 4,       // حزمة / Package
    CLASS = 5,         // صنف / Class
    METHOD = 6,        // دالة / Method
    PROPERTY = 7,      // خاصية / Property
    FIELD = 8,         // حقل / Field
    CONSTRUCTOR = 9,   // منشئ / Constructor
    ENUM = 10,         // تعداد / Enum
    INTERFACE = 11,    // واجهة / Interface
    FUNCTION = 12,     // دالة / Function
    VARIABLE = 13,     // متغير / Variable
    CONSTANT = 14,     // ثابت / Constant
    STRING = 15,       // نص / String
    NUMBER = 16,       // رقم / Number
    BOOLEAN = 17,      // منطقي / Boolean
    ARRAY = 18,        // مصفوفة / Array
    OBJECT = 19,       // كائن / Object
    KEY = 20,          // مفتاح / Key
    NULL_VALUE = 21,   // فارغ / Null
    ENUM_MEMBER = 22,  // عضو تعداد / Enum member
    STRUCT = 23,       // بنية / Struct
    EVENT = 24,        // حدث / Event
    OPERATOR = 25,     // عامل / Operator
    TYPE_PARAMETER = 26 // معامل نوع / Type parameter
};

/**
 * @brief Document symbol - رمز في المستند
 */
struct DocumentSymbol {
    std::string name;                               // الاسم / Name
    std::optional<std::string> detail;              // التفاصيل / Detail
    SymbolKind kind;                                // النوع / Kind
    Range range;                                    // النطاق الكامل / Full range
    Range selection_range;                          // نطاق الاختيار / Selection range
    std::vector<DocumentSymbol> children;           // الرموز الفرعية / Child symbols
};

// ============================================================================
// Code Action Types - أنواع إجراءات الكود
// ============================================================================

/**
 * @brief Code action kind - نوع إجراء الكود
 */
enum class CodeActionKind {
    QUICKFIX,           // إصلاح سريع / Quick fix
    REFACTOR,           // إعادة هيكلة / Refactor
    REFACTOR_EXTRACT,   // استخراج / Extract
    REFACTOR_INLINE,    // توسيع مباشر / Inline
    REFACTOR_REWRITE,   // إعادة كتابة / Rewrite
    SOURCE,             // إجراء على المصدر / Source action
    SOURCE_ORGANIZE_IMPORTS // ترتيب الاستيرادات / Organize imports
};

/**
 * @brief Text edit - تعديل نصي
 */
struct TextEdit {
    Range range;        // النطاق / Range to replace
    std::string new_text; // النص الجديد / New text
};

/**
 * @brief Workspace edit - تعديل مساحة العمل
 */
struct WorkspaceEdit {
    std::unordered_map<std::string, std::vector<TextEdit>> changes; // التعديلات لكل ملف / Changes per file
};

/**
 * @brief Code action - إجراء كود
 */
struct CodeAction {
    std::string title;                      // العنوان / Title
    CodeActionKind kind;                    // النوع / Kind
    std::optional<WorkspaceEdit> edit;      // التعديل / Edit to apply
    std::vector<Diagnostic> diagnostics;    // التشخيصات المرتبطة / Related diagnostics
};

// ============================================================================
// Formatting Types - أنواع التنسيق
// ============================================================================

/**
 * @brief Formatting options - خيارات التنسيق
 */
struct FormattingOptions {
    int tab_size = 4;              // حجم التاب / Tab size
    bool insert_spaces = true;     // استخدام المسافات / Use spaces instead of tabs
    bool trim_trailing_whitespace = true;  // إزالة المسافات الزائدة / Trim trailing whitespace
    bool insert_final_newline = true;      // إضافة سطر جديد نهائي / Insert final newline
    bool trim_final_newlines = true;       // إزالة الأسطر الفارغة النهائية / Trim final newlines
};

// ============================================================================
// Server Capabilities - قدرات الخادم
// ============================================================================

/**
 * @brief Server capabilities - قدرات خادم اللغة
 * 
 * تحدد الميزات المدعومة من قبل خادم اللغة
 * Defines the features supported by the language server
 */
struct ServerCapabilities {
    // Text document sync - مزامنة المستند
    struct {
        bool open_close = true;              // فتح/إغلاق / Open/close
        int change = 2;                      // التغييرات (2 = incremental) / Changes (2 = incremental)
    } text_document_sync;
    
    // Completion - الإكمال التلقائي
    struct {
        bool enabled = true;                 // مفعّل / Enabled
        std::vector<std::string> trigger_characters = {".", ":", ">"};  // محفزات / Trigger characters
    } completion_provider;
    
    // Hover - التلميح
    bool hover_provider = true;
    
    // Go to definition - الذهاب للتعريف
    bool definition_provider = true;
    
    // Find references - إيجاد المراجع
    bool references_provider = true;
    
    // Document symbols - رموز المستند
    bool document_symbol_provider = true;
    
    // Code actions - إجراءات الكود
    struct {
        bool enabled = true;
        std::vector<CodeActionKind> kinds;   // الأنواع المدعومة / Supported kinds
    } code_action_provider;
    
    // Formatting - التنسيق
    bool document_formatting_provider = true;
    bool document_range_formatting_provider = true;
};

// ============================================================================
// Helper Functions - دوال مساعدة
// ============================================================================

/**
 * @brief Convert severity to string - تحويل الشدة لنص
 */
inline std::string severity_to_string(DiagnosticSeverity severity) {
    switch (severity) {
        case DiagnosticSeverity::ERROR: return "error";
        case DiagnosticSeverity::WARNING: return "warning";
        case DiagnosticSeverity::INFORMATION: return "info";
        case DiagnosticSeverity::HINT: return "hint";
        default: return "unknown";
    }
}

/**
 * @brief Convert kind to string - تحويل النوع لنص
 */
inline std::string completion_kind_to_string(CompletionItemKind kind) {
    switch (kind) {
        case CompletionItemKind::FUNCTION: return "function";
        case CompletionItemKind::VARIABLE: return "variable";
        case CompletionItemKind::CLASS: return "class";
        case CompletionItemKind::KEYWORD: return "keyword";
        case CompletionItemKind::CONSTANT: return "constant";
        default: return "text";
    }
}

} // namespace lsp
} // namespace sad
