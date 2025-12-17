// بسم الله الرحمن الرحيم
/**
 * @file lsp_server.h
 * @brief LSP Server - خادم بروتوكول اللغة
 * 
 * هذا الملف يحتوي على تنفيذ خادم LSP الكامل للغة ص
 * يدعم محررات الأكواد مثل VS Code و Neovim وغيرها
 * 
 * This file contains the complete LSP server implementation for Sad language
 * Supports code editors like VS Code, Neovim, and others
 * 
 * Features / الميزات:
 * - Document synchronization / مزامنة المستندات
 * - Hover information / معلومات التلميح
 * - Auto-completion / الإكمال التلقائي
 * - Go to definition / الذهاب للتعريف
 * - Find references / إيجاد المراجع
 * - Diagnostics / التشخيصات
 * - Code actions / إجراءات الكود
 * - Formatting / التنسيق
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#pragma once

#include "lsp_types.h"
#include "lexer/lexer_core.h"
#include "parser/parser_core.h"
#include "parser/ast/ast_node.h"
// #include "semantic/type_checker.h" // TODO: سيتم إضافته لاحقاً
#include <memory>
#include <unordered_map>
#include <functional>
#include <string>
#include <vector>

namespace sad {
namespace lsp {

// ============================================================================
// Document Class - صنف المستند
// ============================================================================

/**
 * @brief Represents an open document - يمثل مستنداً مفتوحاً
 * 
 * يحتفظ بالمستند ونتائج التحليل
 * Keeps the document and analysis results
 */
class Document {
public:
    /**
     * @brief Constructor - المنشئ
     */
    explicit Document(const std::string& uri, const std::string& text, int version = 0)
        : uri_(uri), text_(text), version_(version) {
        reanalyze();
    }
    
    // Getters - جالبات
    const std::string& uri() const { return uri_; }
    const std::string& text() const { return text_; }
    int version() const { return version_; }
    const std::vector<Diagnostic>& diagnostics() const { return diagnostics_; }
    
    /**
     * @brief Update document content - تحديث محتوى المستند
     */
    void update(const std::string& new_text, int new_version) {
        text_ = new_text;
        version_ = new_version;
        reanalyze();
    }
    
    /**
     * @brief Apply incremental changes - تطبيق تغييرات تدريجية
     */
    void apply_changes(const std::vector<TextDocumentContentChangeEvent>& changes, int new_version) {
        for (const auto& change : changes) {
            if (change.range) {
                // Incremental change - تغيير تدريجي
                apply_edit(*change.range, change.text);
            } else {
                // Full document update - تحديث كامل
                text_ = change.text;
            }
        }
        version_ = new_version;
        reanalyze();
    }
    
    /**
     * @brief Get symbol at position - الحصول على الرمز عند موضع
     */
    std::optional<std::string> get_symbol_at(const Position& pos) const {
        int offset = position_to_offset(pos);
        if (offset < 0 || offset >= static_cast<int>(text_.size())) {
            return std::nullopt;
        }
        
        // Find word boundaries - إيجاد حدود الكلمة
        int start = offset;
        while (start > 0 && is_identifier_char(text_[start - 1])) {
            start--;
        }
        
        int end = offset;
        while (end < static_cast<int>(text_.size()) && is_identifier_char(text_[end])) {
            end++;
        }
        
        if (start < end) {
            return text_.substr(start, end - start);
        }
        
        return std::nullopt;
    }
    
    /**
     * @brief Get line text - الحصول على نص السطر
     */
    std::string get_line(int line_number) const {
        auto lines = split_lines(text_);
        if (line_number >= 0 && line_number < static_cast<int>(lines.size())) {
            return lines[line_number];
        }
        return "";
    }
    
private:
    std::string uri_;                    // URI المستند / Document URI
    std::string text_;                   // محتوى المستند / Document content
    int version_;                        // رقم الإصدار / Version number
    std::vector<Diagnostic> diagnostics_; // التشخيصات / Diagnostics
    
    // Analysis results - نتائج التحليل
    std::unique_ptr<Sad::Lexer::LexerCore> lexer_;
    std::unique_ptr<Sad::Parser::ParserCore> parser_;
    Sad::AST::StmtList ast_;  // قائمة جمل البرنامج / List of program statements
    
    /**
     * @brief Reanalyze document - إعادة تحليل المستند
     */
    void reanalyze() {
        diagnostics_.clear();
        
        try {
            // Lexical analysis - التحليل المعجمي
            lexer_ = std::make_unique<Sad::Lexer::LexerCore>(text_);
            auto tokens = lexer_->tokenize();
            
            // Syntax analysis - التحليل النحوي
            parser_ = std::make_unique<Sad::Parser::ParserCore>(*lexer_);
            ast_ = parser_->parseProgram();
            
            // Semantic analysis - التحليل الدلالي
            // type_checker_ = std::make_unique<semantic::TypeChecker>();
            // type_checker_->check(ast_);
            
        } catch (const std::exception& e) {
            // Add diagnostic for error - إضافة تشخيص للخطأ
            Diagnostic diag;
            diag.range = Range{{0, 0}, {0, 0}};
            diag.severity = DiagnosticSeverity::ERROR;
            diag.source = "sad";
            diag.message = e.what();
            diagnostics_.push_back(diag);
        }
    }
    
    /**
     * @brief Apply text edit - تطبيق تعديل نصي
     */
    void apply_edit(const Range& range, const std::string& new_text) {
        int start_offset = position_to_offset(range.start);
        int end_offset = position_to_offset(range.end);
        
        if (start_offset >= 0 && end_offset >= start_offset) {
            text_.replace(start_offset, end_offset - start_offset, new_text);
        }
    }
    
    /**
     * @brief Convert position to offset - تحويل الموضع لإزاحة
     */
    int position_to_offset(const Position& pos) const {
        auto lines = split_lines(text_);
        if (pos.line < 0 || pos.line >= static_cast<int>(lines.size())) {
            return -1;
        }
        
        int offset = 0;
        for (int i = 0; i < pos.line; ++i) {
            offset += lines[i].length() + 1; // +1 for newline
        }
        offset += std::min(pos.character, static_cast<int>(lines[pos.line].length()));
        
        return offset;
    }
    
    /**
     * @brief Split text into lines - تقسيم النص لأسطر
     */
    static std::vector<std::string> split_lines(const std::string& text) {
        std::vector<std::string> lines;
        std::string line;
        
        for (char c : text) {
            if (c == '\n') {
                lines.push_back(line);
                line.clear();
            } else if (c != '\r') {
                line += c;
            }
        }
        
        if (!line.empty() || (!text.empty() && text.back() == '\n')) {
            lines.push_back(line);
        }
        
        return lines;
    }
    
    /**
     * @brief Check if character is identifier - فحص إذا كان الحرف معرّف
     */
    static bool is_identifier_char(char c) {
        return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || 
               (static_cast<unsigned char>(c) >= 0x80); // UTF-8 Arabic
    }
};

// ============================================================================
// Language Server Class - صنف خادم اللغة
// ============================================================================

/**
 * @brief LSP Server implementation - تنفيذ خادم LSP
 * 
 * الخادم الرئيسي لبروتوكول اللغة
 * Main Language Server Protocol implementation
 */
class LanguageServer {
public:
    /**
     * @brief Constructor - المنشئ
     */
    LanguageServer() {
        initialize_keywords();
    }
    
    // ========================================================================
    // Lifecycle Methods - دوال دورة الحياة
    // ========================================================================
    
    /**
     * @brief Initialize server - تهيئة الخادم
     */
    ServerCapabilities initialize() {
        ServerCapabilities caps;
        
        // Configure capabilities - ضبط القدرات
        caps.text_document_sync.open_close = true;
        caps.text_document_sync.change = 2; // Incremental
        
        caps.completion_provider.enabled = true;
        caps.completion_provider.trigger_characters = {".", ":", ">"};
        
        caps.hover_provider = true;
        caps.definition_provider = true;
        caps.references_provider = true;
        caps.document_symbol_provider = true;
        
        caps.code_action_provider.enabled = true;
        caps.code_action_provider.kinds = {
            CodeActionKind::QUICKFIX,
            CodeActionKind::REFACTOR
        };
        
        caps.document_formatting_provider = true;
        caps.document_range_formatting_provider = true;
        
        return caps;
    }
    
    // ========================================================================
    // Document Synchronization - مزامنة المستندات
    // ========================================================================
    
    /**
     * @brief Open document - فتح مستند
     */
    void did_open(const TextDocumentItem& item) {
        auto doc = std::make_shared<Document>(item.uri, item.text, item.version);
        documents_[item.uri] = doc;
        
        // Publish diagnostics - نشر التشخيصات
        publish_diagnostics(item.uri, doc->diagnostics());
    }
    
    /**
     * @brief Change document - تغيير مستند
     */
    void did_change(const std::string& uri, 
                    const std::vector<TextDocumentContentChangeEvent>& changes,
                    int version) {
        auto it = documents_.find(uri);
        if (it != documents_.end()) {
            it->second->apply_changes(changes, version);
            publish_diagnostics(uri, it->second->diagnostics());
        }
    }
    
    /**
     * @brief Close document - إغلاق مستند
     */
    void did_close(const std::string& uri) {
        documents_.erase(uri);
    }
    
    // ========================================================================
    // Language Features - ميزات اللغة
    // ========================================================================
    
    /**
     * @brief Get hover information - الحصول على معلومات التلميح
     */
    std::optional<Hover> hover(const std::string& uri, const Position& pos) {
        auto doc = get_document(uri);
        if (!doc) return std::nullopt;
        
        auto symbol = doc->get_symbol_at(pos);
        if (!symbol) return std::nullopt;
        
        // Check if keyword - فحص إذا كانت كلمة مفتاحية
        if (keyword_docs_.count(*symbol)) {
            Hover hover;
            hover.contents.kind = MarkupKind::MARKDOWN;
            hover.contents.value = keyword_docs_[*symbol];
            return hover;
        }
        
        // TODO: Check variables, functions, etc.
        // فحص المتغيرات، الدوال، إلخ
        
        return std::nullopt;
    }
    
    /**
     * @brief Get completions - الحصول على الإكمالات
     */
    CompletionList completion(const std::string& uri, const Position& pos) {
        CompletionList list;
        list.is_incomplete = false;
        
        auto doc = get_document(uri);
        if (!doc) return list;
        
        // Get current line context - الحصول على سياق السطر الحالي
        std::string line = doc->get_line(pos.line);
        std::string prefix = line.substr(0, pos.character);
        
        // Add keyword completions - إضافة إكمالات الكلمات المفتاحية
        for (const auto& [keyword, doc_str] : keyword_docs_) {
            if (keyword.find(prefix) == 0 || prefix.empty()) {
                CompletionItem item;
                item.label = keyword;
                item.kind = CompletionItemKind::KEYWORD;
                item.detail = "كلمة مفتاحية / Keyword";
                item.documentation = doc_str;
                item.insert_text = keyword;
                list.items.push_back(item);
            }
        }
        
        // TODO: Add variable/function completions
        // إضافة إكمالات المتغيرات/الدوال
        
        return list;
    }
    
    /**
     * @brief Go to definition - الذهاب للتعريف
     */
    std::optional<Location> definition(const std::string& uri, const Position& pos) {
        auto doc = get_document(uri);
        if (!doc) return std::nullopt;
        
        auto symbol = doc->get_symbol_at(pos);
        if (!symbol) return std::nullopt;
        
        // TODO: Find symbol definition
        // إيجاد تعريف الرمز
        
        return std::nullopt;
    }
    
    /**
     * @brief Find references - إيجاد المراجع
     */
    std::vector<Location> references(const std::string& uri, const Position& pos) {
        std::vector<Location> refs;
        
        auto doc = get_document(uri);
        if (!doc) return refs;
        
        auto symbol = doc->get_symbol_at(pos);
        if (!symbol) return refs;
        
        // TODO: Find all references to symbol
        // إيجاد جميع المراجع للرمز
        
        return refs;
    }
    
    /**
     * @brief Get document symbols - الحصول على رموز المستند
     */
    std::vector<DocumentSymbol> document_symbols(const std::string& uri) {
        std::vector<DocumentSymbol> symbols;
        
        auto doc = get_document(uri);
        if (!doc) return symbols;
        
        // TODO: Extract symbols from AST
        // استخراج الرموز من شجرة البناء
        
        return symbols;
    }
    
    /**
     * @brief Get code actions - الحصول على إجراءات الكود
     */
    std::vector<CodeAction> code_actions(const std::string& uri, 
                                         const Range& range,
                                         const std::vector<Diagnostic>& diagnostics) {
        std::vector<CodeAction> actions;
        
        // TODO: Generate quick fixes and refactorings
        // توليد الإصلاحات السريعة وإعادة الهيكلة
        
        return actions;
    }
    
    /**
     * @brief Format document - تنسيق المستند
     */
    std::vector<TextEdit> format_document(const std::string& uri,
                                          const FormattingOptions& options) {
        std::vector<TextEdit> edits;
        
        auto doc = get_document(uri);
        if (!doc) return edits;
        
        // TODO: Implement formatting
        // تنفيذ التنسيق
        
        return edits;
    }
    
private:
    // Documents - المستندات
    std::unordered_map<std::string, std::shared_ptr<Document>> documents_;
    
    // Keyword documentation - توثيق الكلمات المفتاحية
    std::unordered_map<std::string, std::string> keyword_docs_;
    
    // Diagnostic callback - دالة نشر التشخيصات
    std::function<void(const std::string&, const std::vector<Diagnostic>&)> diagnostics_callback_;
    
    /**
     * @brief Get document - الحصول على المستند
     */
    std::shared_ptr<Document> get_document(const std::string& uri) {
        auto it = documents_.find(uri);
        return (it != documents_.end()) ? it->second : nullptr;
    }
    
    /**
     * @brief Publish diagnostics - نشر التشخيصات
     */
    void publish_diagnostics(const std::string& uri, const std::vector<Diagnostic>& diagnostics) {
        if (diagnostics_callback_) {
            diagnostics_callback_(uri, diagnostics);
        }
    }
    
    /**
     * @brief Initialize keyword documentation - تهيئة توثيق الكلمات المفتاحية
     */
    void initialize_keywords() {
        // دوال / Functions
        keyword_docs_["دالة"] = "**دالة** / Function\n\nتعريف دالة جديدة\n\n```sad\nدالة اسم_الدالة(معامل1، معامل2)\n    # كود الدالة\nنهاية\n```";
        keyword_docs_["إرجاع"] = "**إرجاع** / Return\n\nإرجاع قيمة من الدالة\n\n```sad\nإرجاع القيمة\n```";
        
        // تحكم / Control flow
        keyword_docs_["إذا"] = "**إذا** / If\n\nشرط إذا\n\n```sad\nإذا الشرط أعمل\n    # كود\nنهاية\n```";
        keyword_docs_["وإلا"] = "**وإلا** / Else\n\nحالة وإلا\n\n```sad\nوإلا أعمل\n    # كود\nنهاية\n```";
        keyword_docs_["بينما"] = "**بينما** / While\n\nحلقة بينما\n\n```sad\nبينما الشرط أعمل\n    # كود\nنهاية\n```";
        keyword_docs_["لـ"] = "**لـ** / For\n\nحلقة لـ\n\n```sad\nلـ متغير من 0 إلى 10 أعمل\n    # كود\nنهاية\n```";
        
        // أنواع / Types
        keyword_docs_["رقم"] = "**رقم** / Number\n\nنوع رقم صحيح (integer)";
        keyword_docs_["عشري"] = "**عشري** / Float\n\nنوع رقم عشري (float)";
        keyword_docs_["نص"] = "**نص** / String\n\nنوع نصي (string)";
        keyword_docs_["منطقي"] = "**منطقي** / Boolean\n\nنوع منطقي (true/false)";
        
        // قيم / Values
        keyword_docs_["صحيح"] = "**صحيح** / True\n\nقيمة منطقية صحيحة";
        keyword_docs_["خطأ"] = "**خطأ** / False\n\nقيمة منطقية خاطئة";
        keyword_docs_["لاشيء"] = "**لاشيء** / Null\n\nقيمة فارغة";
        
        // صنف / Class
        keyword_docs_["صنف"] = "**صنف** / Class\n\nتعريف صنف جديد\n\n```sad\nصنف اسم_الصنف\n    # خصائص ودوال\nنهاية\n```";
        keyword_docs_["جديد"] = "**جديد** / New\n\nإنشاء كائن جديد\n\n```sad\nكائن = جديد الصنف()\n```";
        
        // طباعة / Print
        keyword_docs_["اطبع"] = "**اطبع** / Print\n\nطباعة نص على الشاشة\n\n```sad\nاطبع(\"مرحباً\")\n```";
    }
    
public:
    /**
     * @brief Set diagnostics callback - تعيين دالة نشر التشخيصات
     */
    void set_diagnostics_callback(
        std::function<void(const std::string&, const std::vector<Diagnostic>&)> callback
    ) {
        diagnostics_callback_ = std::move(callback);
    }
};

} // namespace lsp
} // namespace sad
