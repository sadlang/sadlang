/// خادم LSP الرئيسي للغة ص
///
/// ينفذ بروتوكول خادم اللغة (LSP) لتوفير خدمات الذكاء
/// للمحرر مثل الإكمال التلقائي والتشخيصات والتنقل.
///
/// يتواصل عبر stdin/stdout باستخدام JSON-RPC 2.0
/// مع ترويسة Content-Length.

#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <atomic>
#include <iostream>

// التصريحات الأمامية
namespace sad {
namespace lsp {

class LspInitialize;
class LspSync;
class LspDiagnostics;
class LspCompletion;
class LspDefinition;
class LspReferences;
class LspHover;
class LspSignature;
class LspInlayHints;
class LspCodeLens;
class LspSemanticTokens;
class LspCodeActions;
class LspRename;
class LspFolding;
class LspFormatting;

/// قدرات الخادم
struct ServerCapabilities {
    bool completionProvider = true;
    bool hoverProvider = true;
    bool signatureHelpProvider = true;
    bool definitionProvider = true;
    bool referencesProvider = true;
    bool documentSymbolProvider = true;
    bool codeActionProvider = true;
    bool codeLensProvider = true;
    bool documentFormattingProvider = true;
    bool documentRangeFormattingProvider = true;
    bool documentOnTypeFormattingProvider = true;
    bool renameProvider = true;
    bool foldingRangeProvider = true;
    bool selectionRangeProvider = true;
    bool semanticTokensProvider = true;
    bool inlayHintProvider = true;
    bool callHierarchyProvider = true;
    bool typeHierarchyProvider = true;
    bool workspaceSymbolProvider = true;
    
    // مزامنة المستندات
    int textDocumentSyncKind = 2; // تزايدي
    bool openClose = true;
    bool willSave = false;
    bool save = true;
    bool saveIncludeText = true;
};

/// حالة خادم LSP
enum class ServerState {
    /// غير مُهيّأ
    Uninitialized,
    /// جارٍ التهيئة
    Initializing,
    /// جاهز
    Ready,
    /// جارٍ الإيقاف
    ShuttingDown,
    /// متوقف
    Stopped,
};

/// خادم LSP الرئيسي
///
/// يستقبل الرسائل من stdin ويرد عبر stdout.
/// يوزع الطلبات على المعالجات المتخصصة.
class LspServer {
public:
    LspServer();
    ~LspServer();

    /// بدء الخادم (حلقة الرسائل الرئيسية)
    int run();

    /// إيقاف الخادم
    void shutdown();

    /// تسجيل معالج طلب
    void registerHandler(
        const std::string& method,
        std::function<std::string(const std::string&)> handler
    );

    /// تسجيل معالج إشعار
    void registerNotificationHandler(
        const std::string& method,
        std::function<void(const std::string&)> handler
    );

    /// إرسال إشعار إلى العميل
    void sendNotification(const std::string& method, const std::string& params);

    /// إرسال طلب إلى العميل
    int sendRequest(const std::string& method, const std::string& params);

    /// حالة الخادم
    ServerState state() const { return state_; }

    /// قدرات الخادم
    const ServerCapabilities& capabilities() const { return capabilities_; }

    /// مسار مجلد العمل
    const std::string& rootUri() const { return root_uri_; }

    /// هل وضع المعلم مُفعّل
    bool teacherMode() const { return teacher_mode_; }

    /// (AR) الحصول على معالج مزامنة المستندات / (EN) Get document sync handler
    LspSync* getSync() { return sync_.get(); }

private:
    /// قراءة رسالة من stdin
    std::string readMessage();

    /// إرسال رسالة إلى stdout
    void writeMessage(const std::string& message);

    /// معالجة رسالة واردة
    void processMessage(const std::string& message);

    /// معالجة طلب
    void handleRequest(int id, const std::string& method, const std::string& params);

    /// معالجة إشعار
    void handleNotification(const std::string& method, const std::string& params);

    /// تهيئة المعالجات
    void initializeHandlers();

    /// إرسال استجابة
    void sendResponse(int id, const std::string& result);

    /// إرسال خطأ
    void sendError(int id, int code, const std::string& message);

    // الحالة
    std::atomic<ServerState> state_{ServerState::Uninitialized};
    ServerCapabilities capabilities_;
    std::string root_uri_;
    bool teacher_mode_ = false;
    std::atomic<int> next_request_id_{1};

    // المعالجات
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> 
        request_handlers_;
    std::unordered_map<std::string, std::function<void(const std::string&)>> 
        notification_handlers_;

    // المكونات الفرعية
    std::unique_ptr<LspInitialize> initialize_;
    std::unique_ptr<LspSync> sync_;
    std::unique_ptr<LspDiagnostics> diagnostics_;
    std::unique_ptr<LspCompletion> completion_;
    std::unique_ptr<LspDefinition> definition_;
    std::unique_ptr<LspReferences> references_;
    std::unique_ptr<LspHover> hover_;
    std::unique_ptr<LspSignature> signature_;
    std::unique_ptr<LspInlayHints> inlay_hints_;
    std::unique_ptr<LspCodeLens> code_lens_;
    std::unique_ptr<LspSemanticTokens> semantic_tokens_;
    std::unique_ptr<LspCodeActions> code_actions_;
    std::unique_ptr<LspRename> rename_;
    std::unique_ptr<LspFolding> folding_;
    std::unique_ptr<LspFormatting> formatting_;

    // قفل الإخراج
    std::mutex output_mutex_;
};

} // namespace lsp
} // namespace sad
