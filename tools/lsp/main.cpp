// بسم الله الرحمن الرحيم
/**
 * @file main.cpp
 * @brief LSP Server Main - البرنامج الرئيسي لخادم LSP
 * 
 * نقطة الدخول الرئيسية لخادم بروتوكول اللغة (LSP)
 * يستقبل الرسائل من المحرر عبر stdin ويرسل الردود عبر stdout
 * 
 * Main entry point for Language Server Protocol (LSP) server
 * Receives messages from editor via stdin and sends responses via stdout
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#include "lsp_server.h"
#include "lsp_types.h"
#include "firebase_module_loader.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>

// JSON library - استخدام nlohmann/json
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace sad::lsp;

// ============================================================================
// JSON-RPC Protocol Handler - معالج بروتوكول JSON-RPC
// ============================================================================

/**
 * @brief JSON-RPC message handler - معالج رسائل JSON-RPC
 */
class JSONRPCHandler {
public:
    /**
     * @brief Constructor - المنشئ
     */
    explicit JSONRPCHandler(LanguageServer& server) 
        : server_(server), running_(true), 
          module_loader_(FirebaseModuleLoader(FirebaseConfig())) {
        
        // Log to file for debugging - السجل في ملف للتصحيح
        log_file_.open("lsp_server.log", std::ios::app);
        log("LSP Server started - بدء خادم اللغة");
        log("Firebase Module Loader initialized - تم تهيئة محمل وحدات Firebase");
    }
    
    /**
     * @brief Destructor - المدمر
     */
    ~JSONRPCHandler() {
        if (log_file_.is_open()) {
            log("LSP Server stopped - إيقاف خادم اللغة");
            log_file_.close();
        }
    }
    
    /**
     * @brief Run the server - تشغيل الخادم
     */
    void run() {
        log("Server loop starting - بدء حلقة الخادم");
        
        while (running_ && std::cin.good()) {
            try {
                // Read message - قراءة الرسالة
                auto message = read_message();
                if (!message) {
                    log("Failed to read message - فشل قراءة الرسالة");
                    continue;
                }
                
                log("Received message: " + message->dump());
                
                // Handle message - معالجة الرسالة
                auto response = handle_message(*message);
                
                // Send response - إرسال الرد
                if (response) {
                    log("Sending response: " + response->dump());
                    send_message(*response);
                }
                
            } catch (const std::exception& e) {
                log(std::string("Error: ") + e.what());
                send_error(-1, -32603, "Internal error: " + std::string(e.what()));
            }
        }
        
        log("Server loop ended - انتهت حلقة الخادم");
    }
    
private:
    LanguageServer& server_;
    bool running_;
    FirebaseModuleLoader module_loader_;
    std::ofstream log_file_;
    
    /**
     * @brief Log message - تسجيل رسالة
     */
    void log(const std::string& message) {
        if (log_file_.is_open()) {
            auto now = std::time(nullptr);
            log_file_ << "[" << std::ctime(&now) << "] " << message << std::endl;
            log_file_.flush();
        }
    }
    
    /**
     * @brief Read LSP message - قراءة رسالة LSP
     * 
     * يقرأ رسالة بصيغة LSP من stdin
     * Reads an LSP-formatted message from stdin
     * 
     * Format:
     * Content-Length: <length>\r\n
     * \r\n
     * <json content>
     */
    std::optional<json> read_message() {
        std::string header;
        
        // Read headers - قراءة الرؤوس
        while (std::getline(std::cin, header) && header != "\r") {
            if (header.back() == '\r') {
                header.pop_back();
            }
            
            // Parse Content-Length - تحليل طول المحتوى
            if (header.find("Content-Length:") == 0) {
                size_t length = std::stoul(header.substr(15));
                
                // Skip empty line - تخطي السطر الفارغ
                std::getline(std::cin, header);
                
                // Read content - قراءة المحتوى
                std::vector<char> buffer(length + 1);
                std::cin.read(buffer.data(), length);
                buffer[length] = '\0';
                
                // Parse JSON - تحليل JSON
                try {
                    return json::parse(buffer.data());
                } catch (const json::parse_error& e) {
                    log(std::string("JSON parse error: ") + e.what());
                    return std::nullopt;
                }
            }
        }
        
        return std::nullopt;
    }
    
    /**
     * @brief Send LSP message - إرسال رسالة LSP
     */
    void send_message(const json& message) {
        std::string content = message.dump();
        
        // Send headers - إرسال الرؤوس
        std::cout << "Content-Length: " << content.length() << "\r\n";
        std::cout << "\r\n";
        
        // Send content - إرسال المحتوى
        std::cout << content;
        std::cout.flush();
    }
    
    /**
     * @brief Send error response - إرسال رد خطأ
     */
    void send_error(int id, int code, const std::string& message) {
        json response = {
            {"jsonrpc", "2.0"},
            {"id", id},
            {"error", {
                {"code", code},
                {"message", message}
            }}
        };
        send_message(response);
    }
    
    /**
     * @brief Handle JSON-RPC message - معالجة رسالة JSON-RPC
     */
    std::optional<json> handle_message(const json& message) {
        // Check JSON-RPC version - فحص نسخة JSON-RPC
        if (!message.contains("jsonrpc") || message["jsonrpc"] != "2.0") {
            log("Invalid JSON-RPC version");
            return std::nullopt;
        }
        
        // Get method - الحصول على الطريقة
        if (!message.contains("method")) {
            log("No method in message");
            return std::nullopt;
        }
        
        std::string method = message["method"];
        int id = message.value("id", -1);
        
        log("Handling method: " + method);
        
        // Handle notifications (no response) - معالجة الإشعارات (بدون رد)
        if (id == -1) {
            handle_notification(method, message.value("params", json::object()));
            return std::nullopt;
        }
        
        // Handle requests (with response) - معالجة الطلبات (مع رد)
        try {
            auto result = handle_request(method, message.value("params", json::object()));
            
            json response = {
                {"jsonrpc", "2.0"},
                {"id", id},
                {"result", result}
            };
            
            return response;
            
        } catch (const std::exception& e) {
            log(std::string("Request error: ") + e.what());
            json response = {
                {"jsonrpc", "2.0"},
                {"id", id},
                {"error", {
                    {"code", -32603},
                    {"message", std::string("Internal error: ") + e.what()}
                }}
            };
            return response;
        }
    }
    
    /**
     * @brief Handle request - معالجة طلب
     */
    json handle_request(const std::string& method, const json& params) {
        if (method == "initialize") {
            return handle_initialize(params);
        }
        else if (method == "shutdown") {
            return handle_shutdown();
        }
        else if (method == "textDocument/hover") {
            return handle_hover(params);
        }
        else if (method == "textDocument/completion") {
            return handle_completion(params);
        }
        else if (method == "textDocument/definition") {
            return handle_definition(params);
        }
        else if (method == "textDocument/references") {
            return handle_references(params);
        }
        else if (method == "textDocument/documentSymbol") {
            return handle_document_symbols(params);
        }
        else if (method == "textDocument/codeAction") {
            return handle_code_action(params);
        }
        else if (method == "textDocument/formatting") {
            return handle_formatting(params);
        }
        // Firebase Module Commands - أوامر وحدات Firebase
        else if (method == "modules/list") {
            return handle_list_modules();
        }
        else if (method == "modules/search") {
            return handle_search_modules(params);
        }
        else if (method == "modules/info") {
            return handle_get_module_info(params);
        }
        else if (method == "modules/load") {
            return handle_load_module(params);
        }
        else if (method == "modules/cache/stats") {
            return handle_cache_stats();
        }
        else if (method == "modules/cache/clear") {
            return handle_cache_clear();
        }
        else {
            log("Unknown method: " + method);
            throw std::runtime_error("Method not found: " + method);
        }
    }
    
    /**
     * @brief Handle notification - معالجة إشعار
     */
    void handle_notification(const std::string& method, const json& params) {
        if (method == "initialized") {
            log("Client initialized");
        }
        else if (method == "exit") {
            log("Exit notification received");
            running_ = false;
        }
        else if (method == "textDocument/didOpen") {
            handle_did_open(params);
        }
        else if (method == "textDocument/didChange") {
            handle_did_change(params);
        }
        else if (method == "textDocument/didClose") {
            handle_did_close(params);
        }
        else {
            log("Unknown notification: " + method);
        }
    }
    
    // ========================================================================
    // Request Handlers - معالجات الطلبات
    // ========================================================================
    
    /**
     * @brief Handle initialize - معالجة التهيئة
     */
    json handle_initialize(const json& params) {
        log("Initializing server");
        
        auto capabilities = server_.initialize();
        
        return {
            {"capabilities", {
                {"textDocumentSync", {
                    {"openClose", capabilities.text_document_sync.open_close},
                    {"change", capabilities.text_document_sync.change}
                }},
                {"completionProvider", {
                    {"resolveProvider", false},
                    {"triggerCharacters", capabilities.completion_provider.trigger_characters}
                }},
                {"hoverProvider", capabilities.hover_provider},
                {"definitionProvider", capabilities.definition_provider},
                {"referencesProvider", capabilities.references_provider},
                {"documentSymbolProvider", capabilities.document_symbol_provider},
                {"codeActionProvider", {
                    {"codeActionKinds", {"quickfix", "refactor"}}
                }},
                {"documentFormattingProvider", capabilities.document_formatting_provider}
            }},
            {"serverInfo", {
                {"name", "Sad Language Server"},
                {"version", "1.0.0"}
            }}
        };
    }
    
    /**
     * @brief Handle shutdown - معالجة الإيقاف
     */
    json handle_shutdown() {
        log("Shutting down server");
        return nullptr;
    }
    
    /**
     * @brief Handle hover - معالجة التلميح
     */
    json handle_hover(const json& params) {
        std::string uri = params["textDocument"]["uri"];
        Position pos{
            params["position"]["line"],
            params["position"]["character"]
        };
        
        auto hover = server_.hover(uri, pos);
        if (!hover) {
            return nullptr;
        }
        
        return {
            {"contents", {
                {"kind", hover->contents.kind == MarkupKind::MARKDOWN ? "markdown" : "plaintext"},
                {"value", hover->contents.value}
            }}
        };
    }
    
    /**
     * @brief Handle completion - معالجة الإكمال
     */
    json handle_completion(const json& params) {
        std::string uri = params["textDocument"]["uri"];
        Position pos{
            params["position"]["line"],
            params["position"]["character"]
        };
        
        auto completion_list = server_.completion(uri, pos);
        
        json items = json::array();
        for (const auto& item : completion_list.items) {
            items.push_back({
                {"label", item.label},
                {"kind", static_cast<int>(item.kind)},
                {"detail", item.detail.value_or("")},
                {"documentation", item.documentation.value_or("")},
                {"insertText", item.insert_text.value_or(item.label)}
            });
        }
        
        return {
            {"isIncomplete", completion_list.is_incomplete},
            {"items", items}
        };
    }
    
    /**
     * @brief Handle definition - معالجة الذهاب للتعريف
     */
    json handle_definition(const json& params) {
        std::string uri = params["textDocument"]["uri"];
        Position pos{
            params["position"]["line"],
            params["position"]["character"]
        };
        
        auto location = server_.definition(uri, pos);
        if (!location) {
            return nullptr;
        }
        
        return {
            {"uri", location->uri},
            {"range", {
                {"start", {
                    {"line", location->range.start.line},
                    {"character", location->range.start.character}
                }},
                {"end", {
                    {"line", location->range.end.line},
                    {"character", location->range.end.character}
                }}
            }}
        };
    }
    
    /**
     * @brief Handle references - معالجة إيجاد المراجع
     */
    json handle_references(const json& params) {
        std::string uri = params["textDocument"]["uri"];
        Position pos{
            params["position"]["line"],
            params["position"]["character"]
        };
        
        auto locations = server_.references(uri, pos);
        
        json result = json::array();
        for (const auto& loc : locations) {
            result.push_back({
                {"uri", loc.uri},
                {"range", {
                    {"start", {
                        {"line", loc.range.start.line},
                        {"character", loc.range.start.character}
                    }},
                    {"end", {
                        {"line", loc.range.end.line},
                        {"character", loc.range.end.character}
                    }}
                }}
            });
        }
        
        return result;
    }
    
    /**
     * @brief Handle document symbols - معالجة رموز المستند
     */
    json handle_document_symbols(const json& params) {
        std::string uri = params["textDocument"]["uri"];
        auto symbols = server_.document_symbols(uri);
        
        json result = json::array();
        for (const auto& symbol : symbols) {
            result.push_back(symbol_to_json(symbol));
        }
        
        return result;
    }
    
    /**
     * @brief Handle code action - معالجة إجراء الكود
     */
    json handle_code_action(const json& params) {
        std::string uri = params["textDocument"]["uri"];
        Range range{
            Position{
                params["range"]["start"]["line"],
                params["range"]["start"]["character"]
            },
            Position{
                params["range"]["end"]["line"],
                params["range"]["end"]["character"]
            }
        };
        
        std::vector<Diagnostic> diagnostics;
        if (params.contains("context") && params["context"].contains("diagnostics")) {
            // TODO: Parse diagnostics
        }
        
        auto actions = server_.code_actions(uri, range, diagnostics);
        
        json result = json::array();
        for (const auto& action : actions) {
            json action_json = {
                {"title", action.title},
                {"kind", action_kind_to_string(action.kind)}
            };
            
            if (action.edit) {
                // TODO: Convert WorkspaceEdit to JSON
            }
            
            result.push_back(action_json);
        }
        
        return result;
    }
    
    /**
     * @brief Handle formatting - معالجة التنسيق
     */
    json handle_formatting(const json& params) {
        std::string uri = params["textDocument"]["uri"];
        FormattingOptions options;
        
        if (params.contains("options")) {
            options.tab_size = params["options"].value("tabSize", 4);
            options.insert_spaces = params["options"].value("insertSpaces", true);
        }
        
        auto edits = server_.format_document(uri, options);
        
        json result = json::array();
        for (const auto& edit : edits) {
            result.push_back({
                {"range", {
                    {"start", {
                        {"line", edit.range.start.line},
                        {"character", edit.range.start.character}
                    }},
                    {"end", {
                        {"line", edit.range.end.line},
                        {"character", edit.range.end.character}
                    }}
                }},
                {"newText", edit.new_text}
            });
        }
        
        return result;
    }
    
    // ========================================================================
    // Notification Handlers - معالجات الإشعارات
    // ========================================================================
    
    /**
     * @brief Handle didOpen - معالجة فتح مستند
     */
    void handle_did_open(const json& params) {
        TextDocumentItem item;
        item.uri = params["textDocument"]["uri"];
        item.language_id = params["textDocument"]["languageId"];
        item.version = params["textDocument"]["version"];
        item.text = params["textDocument"]["text"];
        
        server_.did_open(item);
        
        log("Document opened: " + item.uri);
    }
    
    /**
     * @brief Handle didChange - معالجة تغيير مستند
     */
    void handle_did_change(const json& params) {
        std::string uri = params["textDocument"]["uri"];
        int version = params["textDocument"]["version"];
        
        std::vector<TextDocumentContentChangeEvent> changes;
        for (const auto& change : params["contentChanges"]) {
            TextDocumentContentChangeEvent event;
            event.text = change["text"];
            
            if (change.contains("range")) {
                Range range{
                    Position{
                        change["range"]["start"]["line"],
                        change["range"]["start"]["character"]
                    },
                    Position{
                        change["range"]["end"]["line"],
                        change["range"]["end"]["character"]
                    }
                };
                event.range = range;
            }
            
            changes.push_back(event);
        }
        
        server_.did_change(uri, changes, version);
        
        log("Document changed: " + uri);
    }
    
    /**
     * @brief Handle didClose - معالجة إغلاق مستند
     */
    void handle_did_close(const json& params) {
        std::string uri = params["textDocument"]["uri"];
        server_.did_close(uri);
        
        log("Document closed: " + uri);
    }
    
    // ========================================================================
    // Helper Functions - دوال مساعدة
    // ========================================================================
    
    /**
     * @brief Convert DocumentSymbol to JSON - تحويل رمز المستند إلى JSON
     */
    json symbol_to_json(const DocumentSymbol& symbol) {
        json result = {
            {"name", symbol.name},
            {"kind", static_cast<int>(symbol.kind)},
            {"range", {
                {"start", {
                    {"line", symbol.range.start.line},
                    {"character", symbol.range.start.character}
                }},
                {"end", {
                    {"line", symbol.range.end.line},
                    {"character", symbol.range.end.character}
                }}
            }},
            {"selectionRange", {
                {"start", {
                    {"line", symbol.selection_range.start.line},
                    {"character", symbol.selection_range.start.character}
                }},
                {"end", {
                    {"line", symbol.selection_range.end.line},
                    {"character", symbol.selection_range.end.character}
                }}
            }}
        };
        
        if (symbol.detail) {
            result["detail"] = *symbol.detail;
        }
        
        if (!symbol.children.empty()) {
            json children = json::array();
            for (const auto& child : symbol.children) {
                children.push_back(symbol_to_json(child));
            }
            result["children"] = children;
        }
        
        return result;
    }
    
    /**
     * @brief Convert CodeActionKind to string - تحويل نوع إجراء الكود إلى نص
     */
    std::string action_kind_to_string(CodeActionKind kind) {
        switch (kind) {
            case CodeActionKind::QUICKFIX: return "quickfix";
            case CodeActionKind::REFACTOR: return "refactor";
            case CodeActionKind::REFACTOR_EXTRACT: return "refactor.extract";
            case CodeActionKind::REFACTOR_INLINE: return "refactor.inline";
            case CodeActionKind::REFACTOR_REWRITE: return "refactor.rewrite";
            case CodeActionKind::SOURCE: return "source";
            case CodeActionKind::SOURCE_ORGANIZE_IMPORTS: return "source.organizeImports";
            default: return "quickfix";
        }
    }
    
    // ========================================================================
    // Firebase Module Loader Functions - دوال محمل وحدات Firebase
    // ========================================================================
    
    /**
     * @brief List all available modules - قائمة بجميع الوحدات المتاحة
     */
    json handle_list_modules() {
        log("Listing all modules from Firebase");
        return module_loader_.load_available_modules();
    }
    
    /**
     * @brief Search for modules - البحث عن الوحدات
     */
    json handle_search_modules(const json& params) {
        if (!params.contains("query")) {
            throw std::runtime_error("Missing 'query' parameter");
        }
        
        std::string query = params["query"].get<std::string>();
        log("Searching modules with query: " + query);
        return module_loader_.search_modules(query);
    }
    
    /**
     * @brief Get module information - الحصول على معلومات الوحدة
     */
    json handle_get_module_info(const json& params) {
        if (!params.contains("moduleId")) {
            throw std::runtime_error("Missing 'moduleId' parameter");
        }
        
        std::string module_id = params["moduleId"].get<std::string>();
        log("Getting info for module: " + module_id);
        return module_loader_.get_module(module_id);
    }
    
    /**
     * @brief Load module from Firebase - تحميل الوحدة من Firebase
     */
    json handle_load_module(const json& params) {
        if (!params.contains("moduleId")) {
            throw std::runtime_error("Missing 'moduleId' parameter");
        }
        
        std::string module_id = params["moduleId"].get<std::string>();
        std::string version = params.value("version", "");
        
        log("Loading module: " + module_id + (version.empty() ? "" : "@" + version));
        
        try {
            auto content = module_loader_.load_module_content(module_id, version);
            
            // content is a json object
            std::string content_str = content.value("content", "");
            
            log("Successfully loaded module: " + module_id);
            
            return json{
                {"success", true},
                {"moduleId", module_id},
                {"content", content_str},
                {"contentLength", content_str.length()}
            };
            
        } catch (const std::exception& e) {
            log(std::string("Error loading module: ") + e.what());
            return json{{"success", false}, {"error", e.what()}};
        }
    }
    
    /**
     * @brief Get cache statistics - الحصول على إحصائيات التخزين المؤقت
     */
    json handle_cache_stats() {
        log("Getting cache statistics");
        
        try {
            return module_loader_.get_cache_stats();
        } catch (const std::exception& e) {
            log(std::string("Error getting cache stats: ") + e.what());
            return json{{"error", e.what()}};
        }
    }
    
    /**
     * @brief Clear cache - حذف التخزين المؤقت
     */
    json handle_cache_clear() {
        log("Clearing module cache");
        
        try {
            module_loader_.clear_cache();
            log("Cache cleared successfully");
            return json{{"success", true}, {"message", "Cache cleared"}};
        } catch (const std::exception& e) {
            log(std::string("Error clearing cache: ") + e.what());
            return json{{"success", false}, {"error", e.what()}};
        }
    }
};

// ============================================================================
// Main Entry Point - نقطة الدخول الرئيسية
// ============================================================================

/**
 * @brief Main function - الدالة الرئيسية
 */
int main(int argc, char* argv[]) {
    try {
        // Create language server - إنشاء خادم اللغة
        LanguageServer server;
        
        // Set diagnostics callback - تعيين دالة نشر التشخيصات
        server.set_diagnostics_callback(
            [](const std::string& uri, const std::vector<Diagnostic>& diagnostics) {
                // Publish diagnostics to client - نشر التشخيصات للعميل
                json params = {
                    {"uri", uri},
                    {"diagnostics", json::array()}
                };
                
                for (const auto& diag : diagnostics) {
                    params["diagnostics"].push_back({
                        {"range", {
                            {"start", {
                                {"line", diag.range.start.line},
                                {"character", diag.range.start.character}
                            }},
                            {"end", {
                                {"line", diag.range.end.line},
                                {"character", diag.range.end.character}
                            }}
                        }},
                        {"severity", static_cast<int>(diag.severity)},
                        {"message", diag.message},
                        {"source", diag.source.empty() ? "sad" : diag.source}
                    });
                }
                
                json notification = {
                    {"jsonrpc", "2.0"},
                    {"method", "textDocument/publishDiagnostics"},
                    {"params", params}
                };
                
                // Send notification - إرسال الإشعار
                std::string content = notification.dump();
                std::cout << "Content-Length: " << content.length() << "\r\n\r\n";
                std::cout << content;
                std::cout.flush();
            }
        );
        
        // Create handler and run - إنشاء المعالج والتشغيل
        JSONRPCHandler handler(server);
        handler.run();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
