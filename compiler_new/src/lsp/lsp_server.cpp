/// تنفيذ خادم LSP الرئيسي للغة ص
///
/// يعالج حلقة الرسائل الرئيسية ويوزع الطلبات
/// على المعالجات المتخصصة.

#include "lsp_server.h"
#include "lsp_initialize.h"
#include "lsp_sync.h"
#include "lsp_diagnostics.h"
#include "lsp_completion.h"
#include "lsp_definition.h"
#include "lsp_references.h"
#include "lsp_hover.h"
#include "lsp_signature.h"
#include "lsp_inlay_hints.h"
#include "lsp_codelens.h"
#include "lsp_semantic_tokens.h"
#include "lsp_code_actions.h"
#include "lsp_rename.h"
#include "lsp_folding.h"
#include "lsp_formatting.h"

#include <sstream>
#include <cstdlib>
#include <cstring>

// مكتبة JSON بسيطة — نستخدم تحليلاً يدوياً مبسطاً
// في الإنتاج يُفضل استخدام nlohmann/json أو rapidjson

namespace sad {
namespace lsp {

LspServer::LspServer() {
    // إنشاء المكونات الفرعية
    initialize_ = std::make_unique<LspInitialize>(*this);
    sync_ = std::make_unique<LspSync>(*this);
    diagnostics_ = std::make_unique<LspDiagnostics>(*this);
    completion_ = std::make_unique<LspCompletion>(*this);
    definition_ = std::make_unique<LspDefinition>(*this);
    references_ = std::make_unique<LspReferences>(*this);
    hover_ = std::make_unique<LspHover>(*this);
    signature_ = std::make_unique<LspSignature>(*this);
    inlay_hints_ = std::make_unique<LspInlayHints>(*this);
    code_lens_ = std::make_unique<LspCodeLens>(*this);
    semantic_tokens_ = std::make_unique<LspSemanticTokens>(*this);
    code_actions_ = std::make_unique<LspCodeActions>(*this);
    rename_ = std::make_unique<LspRename>(*this);
    folding_ = std::make_unique<LspFolding>(*this);
    formatting_ = std::make_unique<LspFormatting>(*this);
    
    initializeHandlers();
}

LspServer::~LspServer() = default;

/// بدء حلقة الرسائل الرئيسية
int LspServer::run() {
    while (state_ != ServerState::Stopped) {
        try {
            std::string message = readMessage();
            if (message.empty()) {
                // نهاية الإدخال
                break;
            }
            processMessage(message);
        } catch (const std::exception& e) {
            // تسجيل الخطأ في stderr
            std::cerr << "خطأ في معالجة الرسالة: " << e.what() << std::endl;
        }
    }
    return 0;
}

/// إيقاف الخادم
void LspServer::shutdown() {
    state_ = ServerState::ShuttingDown;
    // تنظيف الموارد
    state_ = ServerState::Stopped;
}

/// تسجيل معالج طلب
void LspServer::registerHandler(
    const std::string& method,
    std::function<std::string(const std::string&)> handler) {
    request_handlers_[method] = std::move(handler);
}

/// تسجيل معالج إشعار
void LspServer::registerNotificationHandler(
    const std::string& method,
    std::function<void(const std::string&)> handler) {
    notification_handlers_[method] = std::move(handler);
}

/// إرسال إشعار إلى العميل
void LspServer::sendNotification(const std::string& method, const std::string& params) {
    std::string message = "{\"jsonrpc\":\"2.0\",\"method\":\"" + method + "\"";
    if (!params.empty()) {
        message += ",\"params\":" + params;
    }
    message += "}";
    writeMessage(message);
}

/// إرسال طلب إلى العميل
int LspServer::sendRequest(const std::string& method, const std::string& params) {
    int id = next_request_id_++;
    std::string message = "{\"jsonrpc\":\"2.0\",\"id\":" + std::to_string(id) +
                         ",\"method\":\"" + method + "\"";
    if (!params.empty()) {
        message += ",\"params\":" + params;
    }
    message += "}";
    writeMessage(message);
    return id;
}

/// قراءة رسالة من stdin
std::string LspServer::readMessage() {
    // قراءة ترويسة Content-Length
    std::string line;
    int content_length = -1;

    while (std::getline(std::cin, line)) {
        // إزالة \r إن وجد
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line.empty()) {
            // نهاية الترويسات
            break;
        }

        // البحث عن Content-Length
        if (line.substr(0, 16) == "Content-Length: ") {
            content_length = std::atoi(line.substr(16).c_str());
        }
    }

    if (content_length <= 0) {
        return "";
    }

    // قراءة المحتوى
    std::string content(content_length, '\0');
    std::cin.read(&content[0], content_length);

    if (std::cin.gcount() != content_length) {
        return "";
    }

    return content;
}

/// إرسال رسالة إلى stdout
void LspServer::writeMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(output_mutex_);
    std::cout << "Content-Length: " << message.size() << "\r\n\r\n" << message;
    std::cout.flush();
}

/// معالجة رسالة واردة
void LspServer::processMessage(const std::string& message) {
    // تحليل JSON بسيط — نبحث عن id و method
    // في الإنتاج يُستخدم محلل JSON كامل
    
    // البحث عن "method"
    size_t method_pos = message.find("\"method\"");
    std::string method;
    if (method_pos != std::string::npos) {
        size_t colon = message.find(':', method_pos);
        size_t quote1 = message.find('"', colon + 1);
        size_t quote2 = message.find('"', quote1 + 1);
        if (quote1 != std::string::npos && quote2 != std::string::npos) {
            method = message.substr(quote1 + 1, quote2 - quote1 - 1);
        }
    }

    // البحث عن "id"
    int id = -1;
    size_t id_pos = message.find("\"id\"");
    if (id_pos != std::string::npos) {
        size_t colon = message.find(':', id_pos);
        if (colon != std::string::npos) {
            id = std::atoi(message.c_str() + colon + 1);
        }
    }

    // البحث عن "params"
    std::string params;
    size_t params_pos = message.find("\"params\"");
    if (params_pos != std::string::npos) {
        size_t colon = message.find(':', params_pos);
        if (colon != std::string::npos) {
            // نأخذ البقية — سيتم تحليلها في المعالج
            params = message.substr(colon + 1);
            // إزالة القوس الأخير للرسالة
            if (!params.empty() && params.back() == '}') {
                params.pop_back();
            }
            // إزالة المسافات البادئة
            size_t first = params.find_first_not_of(" \t\n\r");
            if (first != std::string::npos) {
                params = params.substr(first);
            }
        }
    }

    if (id >= 0 && !method.empty()) {
        // طلب
        handleRequest(id, method, params);
    } else if (!method.empty()) {
        // إشعار
        handleNotification(method, params);
    }
    // الاستجابات يتم تجاهلها حالياً
}

/// معالجة طلب
void LspServer::handleRequest(int id, const std::string& method, const std::string& params) {
    auto it = request_handlers_.find(method);
    if (it != request_handlers_.end()) {
        try {
            std::string result = it->second(params);
            sendResponse(id, result);
        } catch (const std::exception& e) {
            sendError(id, -32603, std::string("خطأ داخلي: ") + e.what());
        }
    } else {
        sendError(id, -32601, "الطريقة غير موجودة: " + method);
    }
}

/// معالجة إشعار
void LspServer::handleNotification(const std::string& method, const std::string& params) {
    if (method == "exit") {
        state_ = ServerState::Stopped;
        return;
    }

    auto it = notification_handlers_.find(method);
    if (it != notification_handlers_.end()) {
        try {
            it->second(params);
        } catch (const std::exception& e) {
            std::cerr << "خطأ في معالج الإشعار " << method << ": " << e.what() << std::endl;
        }
    }
}

/// تهيئة المعالجات
void LspServer::initializeHandlers() {
    // تسجيل معالجات المكونات الفرعية
    initialize_->registerHandlers();
    sync_->registerHandlers();
    diagnostics_->registerHandlers();
    completion_->registerHandlers();
    definition_->registerHandlers();
    references_->registerHandlers();
    hover_->registerHandlers();
    signature_->registerHandlers();
    inlay_hints_->registerHandlers();
    code_lens_->registerHandlers();
    semantic_tokens_->registerHandlers();
    code_actions_->registerHandlers();
    rename_->registerHandlers();
    folding_->registerHandlers();
    formatting_->registerHandlers();
}

/// إرسال استجابة
void LspServer::sendResponse(int id, const std::string& result) {
    std::string message = "{\"jsonrpc\":\"2.0\",\"id\":" + std::to_string(id) +
                         ",\"result\":" + result + "}";
    writeMessage(message);
}

/// إرسال خطأ
void LspServer::sendError(int id, int code, const std::string& message) {
    std::string error_msg = "{\"jsonrpc\":\"2.0\",\"id\":" + std::to_string(id) +
                           ",\"error\":{\"code\":" + std::to_string(code) +
                           ",\"message\":\"" + message + "\"}}";
    writeMessage(error_msg);
}

} // namespace lsp
} // namespace sad
