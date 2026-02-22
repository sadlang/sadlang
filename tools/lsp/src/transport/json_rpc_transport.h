// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: json_rpc_transport.h
// الوصف: طبقة النقل JSON-RPC لخادم LSP الثوري
// ══════════════════════════════════════════════════════════════════════════════
//
// هذه طبقة النقل المسؤولة عن:
//   ✦ قراءة رسائل JSON-RPC من stdin (من المحرر)
//   ✦ إرسال ردود JSON-RPC عبر stdout (إلى المحرر)
//   ✦ تحويل الرسائل إلى/من استدعاءات LspEngine
//   ✦ إدارة دورة حياة الخادم (initialize → work → shutdown → exit)
//
// بروتوكول النقل:
//   كل رسالة تتكون من رأس (header) ومحتوى (body):
//   Content-Length: <طول>\r\n
//   \r\n
//   <محتوى JSON>
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "lsp_engine.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <optional>
#include <functional>

namespace sad {
namespace lsp {
namespace transport {

using json = nlohmann::json;

// ══════════════════════════════════════════════════════════════════════════════
//  صنف طبقة النقل JSON-RPC
// ══════════════════════════════════════════════════════════════════════════════

/// طبقة النقل JSON-RPC:
/// تربط بين المحرر (VS Code) والمحرك (LspEngine)
/// عبر بروتوكول JSON-RPC 2.0 على stdin/stdout
class JsonRpcTransport {
public:
    /// المنشئ: يأخذ مرجعاً لمحرك LSP
    explicit JsonRpcTransport(LspEngine& engine);

    /// المدمر: ينظف الموارد
    ~JsonRpcTransport();

    /// تشغيل حلقة الخادم الرئيسية
    /// يقرأ رسائل من stdin ويعالجها حتى يتم إيقاف الخادم
    void run();

    /// إيقاف الخادم (من داخل معالج الرسائل)
    void stop();

private:
    LspEngine& engine_;
    bool running_;
    std::ofstream log_file_;

    // ──── قراءة وإرسال الرسائل ────

    /// قراءة رسالة LSP واحدة من stdin
    /// تُرجع nullopt عند فشل القراءة أو نهاية الإدخال
    std::optional<json> read_message();

    /// إرسال رسالة JSON عبر stdout
    void send_message(const json& msg);

    /// إرسال إشعار (بدون id)
    void send_notification(const std::string& method, const json& params);

    /// إرسال رد خطأ
    void send_error(const json& id, int code, const std::string& message);

    // ──── معالجة الرسائل ────

    /// معالجة رسالة واردة (طلب أو إشعار)
    void handle_message(const json& msg);

    /// معالجة طلب (يحتوي id - يتوقع رداً)
    json handle_request(const std::string& method, const json& params);

    /// معالجة إشعار (بدون id - لا يتوقع رداً)
    void handle_notification(const std::string& method, const json& params);

    // ──── تحويل البيانات ────
    // دوال مساعدة لتحويل أنواع LSP إلى/من JSON

    Position parse_position(const json& j);
    Range parse_range(const json& j);
    json position_to_json(const Position& pos);
    json range_to_json(const Range& r);
    json diagnostic_to_json(const Diagnostic& d);
    json completion_item_to_json(const CompletionItem& item);
    json hover_to_json(const Hover& h);
    json location_to_json(const Location& loc);
    json document_symbol_to_json(const DocumentSymbol& sym);
    json code_action_to_json(const CodeAction& action);
    json text_edit_to_json(const TextEdit& edit);
    json inlay_hint_to_json(const InlayHint& hint);
    json folding_range_to_json(const FoldingRange& fr);
    std::string code_action_kind_to_string(CodeActionKind kind);

    // ──── نشر التشخيصات ────
    /// تُستدعى من المحرك عند اكتشاف أخطاء
    void publish_diagnostics(const DocumentUri& uri, const std::vector<Diagnostic>& diagnostics);

    // ──── السجل (اختياري) ────
    void log(const std::string& message);
};

} // namespace transport
} // namespace lsp
} // namespace sad
