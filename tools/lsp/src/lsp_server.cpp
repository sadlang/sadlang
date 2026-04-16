// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: lsp_server.cpp
 * الوصف: خادم بروتوكول اللغة للغة ص - Language Server Protocol Server
 * المرحلة: Phase 8 - US6 - أدوات التطوير
 * المهمة: T092
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يحتوي على التنفيذ الأساسي لخادم LSP للغة ص.
 * 
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║  ما هو بروتوكول خادم اللغة (LSP)؟                                        ║
 * ╠═══════════════════════════════════════════════════════════════════════════╣
 * ║  بروتوكول موحد للتواصل بين محررات الكود وخوادم اللغة.                    ║
 * ║  يسمح لأي محرر يدعم LSP بالاستفادة من:                                   ║
 * ║  - الإكمال التلقائي (Auto-completion)                                    ║
 * ║  - الانتقال للتعريف (Go to Definition)                                   ║
 * ║  - معلومات التمرير (Hover Information)                                   ║
 * ║  - التشخيصات والأخطاء (Diagnostics)                                      ║
 * ║  - إعادة التسمية (Rename)                                                ║
 * ║  - التنسيق (Formatting)                                                  ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * بنية الخادم:
 * 
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │                             خادم اللغة ص                                   │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐        │
 * │  │  المحلل     │  │  المفهرس    │  │  المشخص     │  │  المنسق     │        │
 * │  │  اللغوي    │  │  الدلالي    │  │  Diagnostics│  │  Formatter  │        │
 * │  │  Parser    │  │  Indexer    │  │             │  │             │        │
 * │  └─────────────┘  └─────────────┘  └─────────────┘  └─────────────┘        │
 * │         │                │                │                │               │
 * │         └────────────────┴────────────────┴────────────────┘               │
 * │                                   │                                        │
 * │                          ┌────────┴────────┐                               │
 * │                          │   مدير المستندات │                               │
 * │                          │ Document Manager │                               │
 * │                          └─────────────────┘                               │
 * └─────────────────────────────────────────────────────────────────────────────┘
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * تدفق الرسائل:
 * 
 * المحرر (VS Code/Vim/etc.)
 *    │
 *    ▼
 * ┌──────────────────┐
 * │  JSON-RPC عبر   │
 * │  stdin/stdout   │
 * └──────────────────┘
 *    │
 *    ▼
 * معالج الرسائل
 *    │
 *    ├── initialize      → تهيئة الخادم
 *    ├── completion      → الإكمال التلقائي
 *    ├── hover           → معلومات التمرير
 *    ├── definition      → الانتقال للتعريف
 *    ├── diagnostics     → التشخيصات
 *    └── shutdown        → إيقاف الخادم
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0.0
 * 
 * الحمد لله رب العالمين
 */

#include "../lsp_server.h"
#include "../lsp_types.h"
#include "completion.hpp"
#include "diagnostics.hpp"
#include "hover.hpp"
#include "goto_definition.hpp"
#include "doc_comment.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <filesystem>

namespace sad {
namespace lsp {

namespace fs = std::filesystem;

// ═══════════════════════════════════════════════════════════════════════════════
// ثوابت الخادم - Server Constants
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * اسم الخادم
 */
const std::string SERVER_NAME = "خادم لغة ص";

/**
 * إصدار الخادم
 */
const std::string SERVER_VERSION = "1.0.0";

/**
 * امتدادات ملفات لغة ص
 */
// (AR) الامتداد الوحيد المدعوم هو .ص / (EN) Only supported extension is .ص
const std::vector<std::string> SAD_EXTENSIONS = {".\xd8\xb5"};


// ═══════════════════════════════════════════════════════════════════════════════
// هياكل البيانات الداخلية - Internal Data Structures
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct معلومات_الرمز
 * @brief معلومات رمز في الكود (دالة، متغير، صنف، إلخ)
 */
struct SymbolInfo {
    std::string name;                // اسم الرمز
    SymbolKind kind;                 // نوع الرمز
    Range range;                     // نطاق التعريف
    Range selection_range;           // نطاق الاختيار
    std::string detail;              // تفاصيل إضافية
    std::string documentation;       // توثيق
    std::vector<SymbolInfo> children; // الرموز الفرعية
};

/**
 * @struct معلومات_المستند
 * @brief معلومات مستند مفتوح
 */
struct DocumentInfo {
    std::string uri;                              // معرف المستند
    std::string content;                          // محتوى المستند
    int version = 0;                              // رقم الإصدار
    std::vector<std::string> lines;               // أسطر المستند
    std::vector<SymbolInfo> symbols;              // الرموز المعرفة
    std::vector<Diagnostic> diagnostics;          // التشخيصات
    bool needs_parsing = true;                    // هل يحتاج تحليل؟
    std::chrono::system_clock::time_point last_modified; // آخر تعديل
};


// ═══════════════════════════════════════════════════════════════════════════════
// صنف خادم اللغة - Language Server Class
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class LanguageServerImpl
 * @brief التنفيذ الداخلي لخادم اللغة
 */
class LanguageServerImpl {
public:
    /**
     * @brief المنشئ
     */
    LanguageServerImpl() {
        // تهيئة مزودي الخدمات
        completion_provider_ = std::make_unique<CompletionProvider>();
        diagnostics_provider_ = std::make_unique<DiagnosticsProvider>();
        hover_provider_ = std::make_unique<HoverProvider>();
        definition_provider_ = std::make_unique<GotoDefinitionProvider>();
        
        log_info("تم تهيئة خادم لغة ص");
    }
    
    /**
     * @brief التهيئة
     * @param params معلمات التهيئة
     * @return قدرات الخادم
     */
    ServerCapabilities initialize(const InitializeParams& params) {
        log_info("تهيئة الخادم...");
        
        // تخزين مسار مساحة العمل
        if (!params.root_uri.empty()) {
            workspace_root_ = uri_to_path(params.root_uri);
            log_info("مساحة العمل: " + workspace_root_);
        }
        
        // بناء قدرات الخادم
        ServerCapabilities caps;
        
        // دعم مزامنة المستندات
        caps.text_document_sync.open_close = true;
        caps.text_document_sync.change = 2; // دعم التغييرات التزايدية
        
        // دعم الإكمال التلقائي
        caps.completion_provider.trigger_characters = {".", ":", "\"", "'"};
        caps.completion_provider.resolve_provider = true;
        
        // دعم معلومات التمرير
        caps.hover_provider = true;
        
        // دعم الانتقال للتعريف
        caps.definition_provider = true;
        
        // دعم إيجاد المراجع
        caps.references_provider = true;
        
        // دعم رموز المستند
        caps.document_symbol_provider = true;
        
        // دعم التنسيق
        caps.document_formatting_provider = true;
        
        // دعم إجراءات الكود
        caps.code_action_provider = true;
        
        log_info("تم تهيئة الخادم بنجاح");
        return caps;
    }
    
    /**
     * @brief فتح مستند
     */
    void did_open(const TextDocumentItem& item) {
        std::unique_lock lock(documents_mutex_);
        
        DocumentInfo doc;
        doc.uri = item.uri;
        doc.content = item.text;
        doc.version = item.version;
        doc.lines = split_lines(item.text);
        doc.needs_parsing = true;
        doc.last_modified = std::chrono::system_clock::now();
        
        documents_[item.uri] = std::move(doc);
        lock.unlock();
        
        // تحليل المستند
        parse_document(item.uri);
        
        log_info("فتح مستند: " + item.uri);
    }
    
    /**
     * @brief تغيير مستند
     */
    void did_change(const DidChangeTextDocumentParams& params) {
        std::unique_lock lock(documents_mutex_);
        
        auto it = documents_.find(params.text_document.uri);
        if (it == documents_.end()) {
            log_warning("مستند غير موجود: " + params.text_document.uri);
            return;
        }
        
        auto& doc = it->second;
        
        // تطبيق التغييرات
        for (const auto& change : params.content_changes) {
            if (change.range.has_value()) {
                // تغيير تزايدي
                apply_incremental_change(doc, change.range.value(), change.text);
            } else {
                // استبدال كامل
                doc.content = change.text;
                doc.lines = split_lines(change.text);
            }
        }
        
        doc.version = params.text_document.version;
        doc.needs_parsing = true;
        doc.last_modified = std::chrono::system_clock::now();
        
        lock.unlock();
        
        // إعادة تحليل المستند
        parse_document(params.text_document.uri);
    }
    
    /**
     * @brief إغلاق مستند
     */
    void did_close(const std::string& uri) {
        std::unique_lock lock(documents_mutex_);
        documents_.erase(uri);
        lock.unlock();
        
        log_info("إغلاق مستند: " + uri);
    }
    
    /**
     * @brief الإكمال التلقائي
     */
    CompletionList completion(const std::string& uri, const Position& pos) {
        std::shared_lock lock(documents_mutex_);
        
        auto it = documents_.find(uri);
        if (it == documents_.end()) {
            return CompletionList{};
        }
        
        const auto& doc = it->second;
        lock.unlock();
        
        return completion_provider_->get_completions(doc, pos);
    }
    
    /**
     * @brief معلومات التمرير
     */
    std::optional<Hover> hover(const std::string& uri, const Position& pos) {
        std::shared_lock lock(documents_mutex_);
        
        auto it = documents_.find(uri);
        if (it == documents_.end()) {
            return std::nullopt;
        }
        
        const auto& doc = it->second;
        lock.unlock();
        
        return hover_provider_->get_hover(doc, pos);
    }
    
    /**
     * @brief الانتقال للتعريف
     */
    std::optional<Location> definition(const std::string& uri, const Position& pos) {
        std::shared_lock lock(documents_mutex_);
        
        auto it = documents_.find(uri);
        if (it == documents_.end()) {
            return std::nullopt;
        }
        
        const auto& doc = it->second;
        lock.unlock();
        
        return definition_provider_->get_definition(doc, pos, documents_);
    }
    
    /**
     * @brief إيجاد المراجع
     */
    std::vector<Location> references(const std::string& uri, const Position& pos) {
        std::shared_lock lock(documents_mutex_);
        
        auto it = documents_.find(uri);
        if (it == documents_.end()) {
            return {};
        }
        
        const auto& doc = it->second;
        lock.unlock();
        
        return definition_provider_->get_references(doc, pos, documents_);
    }
    
    /**
     * @brief رموز المستند
     */
    std::vector<DocumentSymbol> document_symbols(const std::string& uri) {
        std::shared_lock lock(documents_mutex_);
        
        auto it = documents_.find(uri);
        if (it == documents_.end()) {
            return {};
        }
        
        const auto& doc = it->second;
        lock.unlock();
        
        std::vector<DocumentSymbol> result;
        for (const auto& sym : doc.symbols) {
            result.push_back(symbol_to_document_symbol(sym));
        }
        
        return result;
    }
    
    /**
     * @brief التشخيصات
     */
    std::vector<Diagnostic> diagnostics(const std::string& uri) {
        std::shared_lock lock(documents_mutex_);
        
        auto it = documents_.find(uri);
        if (it == documents_.end()) {
            return {};
        }
        
        return it->second.diagnostics;
    }
    
    /**
     * @brief تنسيق المستند
     */
    std::vector<TextEdit> format_document(const std::string& uri, const FormattingOptions& options) {
        std::shared_lock lock(documents_mutex_);
        
        auto it = documents_.find(uri);
        if (it == documents_.end()) {
            return {};
        }
        
        const auto& doc = it->second;
        lock.unlock();
        
        // NOTE: التنسيق الفعلي مُنفّذ في LspEngine::format_document
        // انظر: formatting_provider.cpp لتفاصيل التنسيق الكاملة
        // هذه الدالة stub للتوافق مع API قديم
        return {};
    }
    
    /**
     * @brief إجراءات الكود
     */
    std::vector<CodeAction> code_actions(
        const std::string& uri,
        const Range& range,
        const std::vector<Diagnostic>& diags) {
        
        std::vector<CodeAction> actions;
        
        // إضافة إجراءات للتشخيصات
        for (const auto& diag : diags) {
            auto fix = get_quick_fix(diag);
            if (fix.has_value()) {
                actions.push_back(fix.value());
            }
        }
        
        return actions;
    }
    
    /**
     * @brief إيقاف الخادم
     */
    void shutdown() {
        log_info("إيقاف الخادم...");
        
        // تنظيف الموارد
        std::unique_lock lock(documents_mutex_);
        documents_.clear();
        lock.unlock();
        
        log_info("تم إيقاف الخادم");
    }

private:
    std::string workspace_root_;                              // مسار مساحة العمل
    std::unordered_map<std::string, DocumentInfo> documents_; // المستندات المفتوحة
    mutable std::shared_mutex documents_mutex_;               // قفل المستندات
    
    // مزودي الخدمات
    std::unique_ptr<CompletionProvider> completion_provider_;
    std::unique_ptr<DiagnosticsProvider> diagnostics_provider_;
    std::unique_ptr<HoverProvider> hover_provider_;
    std::unique_ptr<GotoDefinitionProvider> definition_provider_;
    
    /**
     * @brief تحليل المستند
     */
    void parse_document(const std::string& uri) {
        std::unique_lock lock(documents_mutex_);
        
        auto it = documents_.find(uri);
        if (it == documents_.end()) return;
        
        auto& doc = it->second;
        if (!doc.needs_parsing) return;
        
        // مسح البيانات القديمة
        doc.symbols.clear();
        doc.diagnostics.clear();
        
        // تحليل الرموز
        parse_symbols(doc);
        
        // تحليل التشخيصات
        auto diags = diagnostics_provider_->analyze(doc);
        doc.diagnostics = std::move(diags);
        
        doc.needs_parsing = false;
    }
    
    /**
     * @brief تحويل نص توثيقي خام إلى Markdown غني باستخدام DocCommentParser
     * @param raw_text النص الخام المستخرج من التعليق
     * @return نص Markdown غني مع وسوم مُنسقة، أو النص الخام إذا لم تُوجد وسوم
     */
    std::string format_doc_as_markdown(const std::string& raw_text) {
        if (raw_text.empty()) return "";
        
        auto doc = Sad::AST::DocCommentParser::parse(raw_text);
        if (doc.isEmpty()) return raw_text;
        
        std::string md = doc.toMarkdown();
        // إزالة الأسطر الفارغة في النهاية
        while (!md.empty() && (md.back() == '\n' || md.back() == ' ')) {
            md.pop_back();
        }
        return md.empty() ? raw_text : md;
    }

    /**
     * @brief استخراج تعليق توثيقي من الأسطر السابقة لرمز
     * @param lines أسطر المستند
     * @param symbol_line رقم سطر الرمز (0-based)
     * @return نص التوثيق بصيغة Markdown غني، أو سلسلة فارغة إذا لا يوجد
     *
     * يدعم نمطين:
     * 1. تعليق سطر واحد أو أكثر: ## نص التوثيق
     * 2. تعليق كتلة: #** نص التوثيق **#
     */
    std::string extract_doc_comment(const std::vector<std::string>& lines, size_t symbol_line) {
        if (symbol_line == 0) return "";
        
        // (AR) تتبع الأسطر الفارغة بين التعليق والرمز
        int scan_line = static_cast<int>(symbol_line) - 1;
        
        // تخطي الأسطر الفارغة بين التعليق والرمز (أسطر فارغة واحدة أو أقل)
        while (scan_line >= 0) {
            std::string trimmed = lines[scan_line];
            // إزالة المسافات البيضاء من البداية والنهاية
            size_t start = trimmed.find_first_not_of(" \t\r\n");
            if (start == std::string::npos) {
                // سطر فارغ — نتخطاه (سطر واحد فقط)
                scan_line--;
                break;
            }
            break;
        }
        
        if (scan_line < 0) return "";
        
        // التحقق مما إذا كان السطر ينتهي بـ **# (نهاية تعليق كتلة توثيقي)
        {
            std::string trimmed = lines[scan_line];
            size_t start = trimmed.find_first_not_of(" \t");
            if (start != std::string::npos) {
                trimmed = trimmed.substr(start);
            }
            
            // البحث عن **# في نهاية السطر
            // (AR) يمكن أن يكون التعليق على سطر واحد: #** نص **#
            // (AR) أو على عدة أسطر تنتهي بـ **#
            if (trimmed.find("**#") != std::string::npos) {
                // تعليق كتلة توثيقي — نبحث عن بدايته #**
                std::string block_text;
                int block_start = scan_line;
                
                // البحث لأعلى عن #**
                for (int j = scan_line; j >= 0; j--) {
                    if (lines[j].find("#**") != std::string::npos) {
                        block_start = j;
                        break;
                    }
                }
                
                // تجميع نص الكتلة
                for (int j = block_start; j <= scan_line; j++) {
                    std::string l = lines[j];
                    // إزالة #** من البداية
                    size_t pos = l.find("#**");
                    if (pos != std::string::npos) {
                        l = l.substr(pos + std::string("#**").length());
                    }
                    // إزالة **# من النهاية
                    pos = l.find("**#");
                    if (pos != std::string::npos) {
                        l = l.substr(0, pos);
                    }
                    // إزالة * بادئة سطور التعليق
                    size_t s = l.find_first_not_of(" \t");
                    if (s != std::string::npos && l[s] == '*') {
                        l = l.substr(s + 1);
                    }
                    // إزالة المسافات البيضاء
                    s = l.find_first_not_of(" \t");
                    if (s != std::string::npos) {
                        l = l.substr(s);
                    }
                    if (!l.empty()) {
                        if (!block_text.empty()) block_text += "\n";
                        block_text += l;
                    }
                }
                
                return format_doc_as_markdown(block_text);
            }
        }
        
        // التحقق من تعليقات ## (سطر واحد أو أكثر)
        {
            std::vector<std::string> doc_lines;
            
            for (int j = scan_line; j >= 0; j--) {
                std::string trimmed = lines[j];
                size_t start = trimmed.find_first_not_of(" \t");
                if (start == std::string::npos) break; // سطر فارغ — توقف
                trimmed = trimmed.substr(start);
                
                // التحقق من أن السطر يبدأ بـ ##
                // (AR) ## تعليق توثيقي (وليس #* أو # عادي)
                if (trimmed.length() >= 2 && trimmed[0] == '#' && trimmed[1] == '#') {
                    // إزالة ## والمسافات
                    std::string text = trimmed.substr(2);
                    size_t s = text.find_first_not_of(" \t");
                    if (s != std::string::npos) {
                        text = text.substr(s);
                    }
                    doc_lines.push_back(text);
                } else {
                    break; // ليس تعليق توثيقي — توقف
                }
            }
            
            if (!doc_lines.empty()) {
                // عكس الترتيب (جمعنا من الأسفل للأعلى)
                std::reverse(doc_lines.begin(), doc_lines.end());
                std::string result;
                for (size_t k = 0; k < doc_lines.size(); k++) {
                    if (k > 0) result += "\n";
                    result += doc_lines[k];
                }
                return format_doc_as_markdown(result);
            }
        }
        
        return "";
    }
    
    /**
     * @brief تحليل الرموز في المستند
     */
    void parse_symbols(DocumentInfo& doc) {
        // التعبيرات النمطية للرموز في لغة ص
        
        // دالة: دالة اسم_الدالة(معلمات)
        std::regex func_regex(R"(دالة\s+(\w+)\s*\()");
        
        // متغير: نوع اسم_المتغير = قيمة
        std::regex var_regex(R"((رقم|نص|منطقي|قائمة|خريطة)\s+(\w+)\s*=)");
        
        // صنف: صنف اسم_الصنف
        std::regex class_regex(R"(صنف\s+(\w+))");
        
        // ثابت: ثابت اسم = قيمة
        std::regex const_regex(R"(ثابت\s+(\w+)\s*=)");
        
        for (size_t i = 0; i < doc.lines.size(); ++i) {
            const auto& line = doc.lines[i];
            std::smatch match;
            
            // البحث عن الدوال
            if (std::regex_search(line, match, func_regex)) {
                SymbolInfo sym;
                sym.name = match[1].str();
                sym.kind = SymbolKind::FUNCTION;
                sym.range.start = {static_cast<int>(i), static_cast<int>(match.position())};
                sym.range.end = find_block_end(doc.lines, i);
                sym.selection_range = sym.range;
                sym.detail = "دالة";
                sym.documentation = extract_doc_comment(doc.lines, i);
                doc.symbols.push_back(sym);
            }
            
            // البحث عن المتغيرات
            if (std::regex_search(line, match, var_regex)) {
                SymbolInfo sym;
                sym.name = match[2].str();
                sym.kind = SymbolKind::VARIABLE;
                sym.range.start = {static_cast<int>(i), static_cast<int>(match.position())};
                sym.range.end = {static_cast<int>(i), static_cast<int>(line.length())};
                sym.selection_range = sym.range;
                sym.detail = match[1].str();
                sym.documentation = extract_doc_comment(doc.lines, i);
                doc.symbols.push_back(sym);
            }
            
            // البحث عن الأصناف
            if (std::regex_search(line, match, class_regex)) {
                SymbolInfo sym;
                sym.name = match[1].str();
                sym.kind = SymbolKind::CLASS;
                sym.range.start = {static_cast<int>(i), static_cast<int>(match.position())};
                sym.range.end = find_block_end(doc.lines, i);
                sym.selection_range = sym.range;
                sym.detail = "صنف";
                sym.documentation = extract_doc_comment(doc.lines, i);
                doc.symbols.push_back(sym);
            }
            
            // البحث عن الثوابت
            if (std::regex_search(line, match, const_regex)) {
                SymbolInfo sym;
                sym.name = match[1].str();
                sym.kind = SymbolKind::CONSTANT;
                sym.range.start = {static_cast<int>(i), static_cast<int>(match.position())};
                sym.range.end = {static_cast<int>(i), static_cast<int>(line.length())};
                sym.selection_range = sym.range;
                sym.detail = "ثابت";
                sym.documentation = extract_doc_comment(doc.lines, i);
                doc.symbols.push_back(sym);
            }
        }
    }
    
    /**
     * @brief إيجاد نهاية الكتلة
     */
    Position find_block_end(const std::vector<std::string>& lines, size_t start) {
        // البحث عن "نهاية" للدالة أو الصنف
        for (size_t i = start + 1; i < lines.size(); ++i) {
            if (lines[i].find("نهاية") != std::string::npos) {
                return {static_cast<int>(i), static_cast<int>(lines[i].length())};
            }
        }
        return {static_cast<int>(start), 0};
    }
    
    /**
     * @brief تقسيم النص إلى أسطر
     */
    std::vector<std::string> split_lines(const std::string& text) {
        std::vector<std::string> lines;
        std::istringstream stream(text);
        std::string line;
        while (std::getline(stream, line)) {
            lines.push_back(line);
        }
        return lines;
    }
    
    /**
     * @brief تطبيق تغيير تزايدي
     */
    void apply_incremental_change(DocumentInfo& doc, const Range& range, const std::string& new_text) {
        // تحويل النطاق إلى مواقع في النص
        size_t start_offset = 0;
        for (int i = 0; i < range.start.line && i < static_cast<int>(doc.lines.size()); ++i) {
            start_offset += doc.lines[i].length() + 1; // +1 للسطر الجديد
        }
        start_offset += range.start.character;
        
        size_t end_offset = 0;
        for (int i = 0; i < range.end.line && i < static_cast<int>(doc.lines.size()); ++i) {
            end_offset += doc.lines[i].length() + 1;
        }
        end_offset += range.end.character;
        
        // تطبيق التغيير
        doc.content = doc.content.substr(0, start_offset) 
                    + new_text 
                    + doc.content.substr(end_offset);
        
        // إعادة تقسيم الأسطر
        doc.lines = split_lines(doc.content);
    }
    
    /**
     * @brief تحويل URI إلى مسار
     */
    std::string uri_to_path(const std::string& uri) {
        // إزالة "file://" أو "file:///"
        std::string path = uri;
        if (path.find("file:///") == 0) {
            path = path.substr(8);
        } else if (path.find("file://") == 0) {
            path = path.substr(7);
        }
        
        // (AR) فك ترميز URL — تحويل %XX إلى الحرف المقابل
        // (EN) URL decode — convert %XX to corresponding character
        std::string decoded;
        decoded.reserve(path.size());
        for (size_t i = 0; i < path.size(); ++i) {
            if (path[i] == '%' && i + 2 < path.size()) {
                char hi = path[i + 1];
                char lo = path[i + 2];
                auto hexVal = [](char c) -> int {
                    if (c >= '0' && c <= '9') return c - '0';
                    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
                    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
                    return -1;
                };
                int h = hexVal(hi), l = hexVal(lo);
                if (h >= 0 && l >= 0) {
                    decoded += static_cast<char>((h << 4) | l);
                    i += 2;
                    continue;
                }
            }
            decoded += path[i];
        }
        
        // (AR) على Windows: تحويل / إلى \\ 
        // (EN) On Windows: convert / to backslash
#ifdef _WIN32
        for (auto& c : decoded) {
            if (c == '/') c = '\\';
        }
#endif
        
        return decoded;
    }
    
    /**
     * @brief تحويل SymbolInfo إلى DocumentSymbol
     */
    DocumentSymbol symbol_to_document_symbol(const SymbolInfo& sym) {
        DocumentSymbol ds;
        ds.name = sym.name;
        ds.kind = sym.kind;
        ds.range = sym.range;
        ds.selection_range = sym.selection_range;
        ds.detail = sym.detail;
        
        for (const auto& child : sym.children) {
            ds.children.push_back(symbol_to_document_symbol(child));
        }
        
        return ds;
    }
    
    /**
     * @brief الحصول على إصلاح سريع
     */
    std::optional<CodeAction> get_quick_fix(const Diagnostic& diag) {
        CodeAction action;
        action.title = "إصلاح: " + diag.message;
        action.kind = CodeActionKind::QUICK_FIX;
        
        // (AR) الإصلاحات السريعة حسب نوع الخطأ
        // (EN) Quick fixes based on error type
        std::string msg = diag.message;
        
        // (AR) متغير غير معرّف — اقتراح إضافة "متغير"
        // (EN) Undefined variable — suggest adding "متغير"
        if (msg.find("غير معرف") != std::string::npos || 
            msg.find("undefined") != std::string::npos) {
            action.title = "إضافة تعريف المتغير";
        }
        // (AR) فاصلة منقوصة
        // (EN) Missing separator
        else if (msg.find("نهاية") != std::string::npos ||
                 msg.find("expected") != std::string::npos) {
            action.title = "إضافة 'نهاية' المفقودة";
        }
        // (AR) استيراد مفقود
        // (EN) Missing import
        else if (msg.find("استورد") != std::string::npos ||
                 msg.find("import") != std::string::npos) {
            action.title = "إضافة الاستيراد المفقود";
        }
        
        return action;
    }
    
    // دوال التسجيل
    void log_info(const std::string& msg) {
        std::cerr << "[INFO] " << msg << std::endl;
    }
    
    void log_warning(const std::string& msg) {
        std::cerr << "[WARN] " << msg << std::endl;
    }
    
    void log_error(const std::string& msg) {
        std::cerr << "[ERROR] " << msg << std::endl;
    }
};


// ═══════════════════════════════════════════════════════════════════════════════
// تنفيذ واجهة LanguageServer
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief تنفيذ خادم اللغة العام
 */
class LanguageServer::Impl {
public:
    LanguageServerImpl impl;
};

LanguageServer::LanguageServer() : pimpl_(std::make_unique<Impl>()) {}
LanguageServer::~LanguageServer() = default;

ServerCapabilities LanguageServer::initialize() {
    InitializeParams params;
    return pimpl_->impl.initialize(params);
}

void LanguageServer::did_open(const TextDocumentItem& item) {
    pimpl_->impl.did_open(item);
}

void LanguageServer::did_change(const std::string& uri, int version, const std::vector<TextDocumentContentChangeEvent>& changes) {
    DidChangeTextDocumentParams params;
    params.text_document.uri = uri;
    params.text_document.version = version;
    params.content_changes = changes;
    pimpl_->impl.did_change(params);
}

void LanguageServer::did_close(const std::string& uri) {
    pimpl_->impl.did_close(uri);
}

CompletionList LanguageServer::completion(const std::string& uri, const Position& pos) {
    return pimpl_->impl.completion(uri, pos);
}

std::optional<Hover> LanguageServer::hover(const std::string& uri, const Position& pos) {
    return pimpl_->impl.hover(uri, pos);
}

std::optional<Location> LanguageServer::definition(const std::string& uri, const Position& pos) {
    return pimpl_->impl.definition(uri, pos);
}

std::vector<Location> LanguageServer::references(const std::string& uri, const Position& pos) {
    return pimpl_->impl.references(uri, pos);
}

std::vector<DocumentSymbol> LanguageServer::document_symbols(const std::string& uri) {
    return pimpl_->impl.document_symbols(uri);
}

std::vector<TextEdit> LanguageServer::format_document(const std::string& uri, const FormattingOptions& options) {
    return pimpl_->impl.format_document(uri, options);
}

std::vector<CodeAction> LanguageServer::code_actions(const std::string& uri, const Range& range, const std::vector<Diagnostic>& diags) {
    return pimpl_->impl.code_actions(uri, range, diags);
}

void LanguageServer::shutdown() {
    pimpl_->impl.shutdown();
}

} // namespace lsp
} // namespace sad
