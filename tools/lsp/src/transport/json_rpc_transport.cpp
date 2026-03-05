// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: json_rpc_transport.cpp
// الوصف: تنفيذ طبقة النقل JSON-RPC لخادم LSP الثوري
// ══════════════════════════════════════════════════════════════════════════════
//
//  ╔══════════════════════════════════════════════════════════════════════════╗
//  ║  بروتوكول النقل:                                                       ║
//  ║                                                                        ║
//  ║  ┌─── stdin (من المحرر) ──────────────────────────────────────────────┐ ║
//  ║  │ Content-Length: 52\r\n                                             │ ║
//  ║  │ \r\n                                                               │ ║
//  ║  │ {"jsonrpc":"2.0","id":1,"method":"initialize",...}                 │ ║
//  ║  └───────────────────────────────────────────────────────────────────┘ ║
//  ║                                                                        ║
//  ║  ┌─── stdout (إلى المحرر) ────────────────────────────────────────────┐ ║
//  ║  │ Content-Length: 120\r\n                                            │ ║
//  ║  │ \r\n                                                               │ ║
//  ║  │ {"jsonrpc":"2.0","id":1,"result":{"capabilities":{...}}}          │ ║
//  ║  └───────────────────────────────────────────────────────────────────┘ ║
//  ╚══════════════════════════════════════════════════════════════════════════╝
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "json_rpc_transport.h"
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <algorithm>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

namespace sad {
namespace lsp {
namespace transport {

// ══════════════════════════════════════════════════════════════════════════════
//  المنشئ والمدمر
// ══════════════════════════════════════════════════════════════════════════════

JsonRpcTransport::JsonRpcTransport(LspEngine& engine)
    : engine_(engine)
    , running_(false)
{
#ifdef _WIN32
    // تحويل stdin/stdout إلى الوضع الثنائي لمنع تحويل \r\n
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    // فتح ملف السجل (اختياري - للتصحيح)
    const char* log_env = std::getenv("SAD_LSP_LOG");
    if (log_env && std::strlen(log_env) > 0) {
        log_file_.open(log_env, std::ios::app);
        log("═══ بدأ خادم LSP الثوري ═══");
    }
}

JsonRpcTransport::~JsonRpcTransport() {
    if (log_file_.is_open()) {
        log("═══ أُغلق خادم LSP ═══");
        log_file_.close();
    }
}

// ══════════════════════════════════════════════════════════════════════════════
//  حلقة الخادم الرئيسية
// ══════════════════════════════════════════════════════════════════════════════

void JsonRpcTransport::run() {
    running_ = true;
    log("بدأت حلقة الخادم الرئيسية...");

    while (running_) {
        // قراءة رسالة واحدة من stdin
        auto msg = read_message();
        if (!msg.has_value()) {
            // فشل القراءة (stdin مغلق أو خطأ)
            log("فشلت قراءة الرسالة - إيقاف الخادم");
            break;
        }

        // معالجة الرسالة
        try {
            handle_message(msg.value());
        } catch (const std::exception& ex) {
            log("خطأ أثناء معالجة الرسالة: " + std::string(ex.what()));
        }
    }

    log("انتهت حلقة الخادم الرئيسية");
}

void JsonRpcTransport::stop() {
    running_ = false;
}

// ══════════════════════════════════════════════════════════════════════════════
//  قراءة وإرسال رسائل LSP
// ══════════════════════════════════════════════════════════════════════════════

/// قراءة رسالة LSP من stdin
/// البروتوكول:
///   Content-Length: <n>\r\n
///   [أي ترويسات أخرى]\r\n
///   \r\n
///   <n بايت من JSON>
std::optional<json> JsonRpcTransport::read_message() {
    // 1. قراءة الترويسات (headers)
    int content_length = -1;

    while (true) {
        std::string line;
        // قراءة سطر واحد (حتى \n)
        int c;
        while ((c = std::fgetc(stdin)) != EOF) {
            if (c == '\n') break;
            line += static_cast<char>(c);
        }

        if (c == EOF) {
            return std::nullopt; // stdin مغلق
        }

        // إزالة \r إن وُجد
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        // سطر فارغ = نهاية الترويسات
        if (line.empty()) {
            break;
        }

        // تحليل ترويسة Content-Length
        const std::string prefix = "Content-Length: ";
        if (line.compare(0, prefix.size(), prefix) == 0) {
            try {
                content_length = std::stoi(line.substr(prefix.size()));
            } catch (...) {
                log("ترويسة Content-Length غير صالحة: " + line);
                return std::nullopt;
            }
        }
    }

    if (content_length <= 0) {
        log("ترويسة Content-Length مفقودة أو غير صالحة");
        return std::nullopt;
    }

    // 2. قراءة المحتوى (body)
    std::string body(content_length, '\0');
    size_t bytes_read = std::fread(&body[0], 1, content_length, stdin);
    if (bytes_read != static_cast<size_t>(content_length)) {
        log("فشلت قراءة محتوى الرسالة: قُرئ " +
            std::to_string(bytes_read) + " من " +
            std::to_string(content_length));
        return std::nullopt;
    }

    // 3. تحليل JSON
    try {
        auto parsed = json::parse(body);
        log("← استُقبلت: " + parsed.value("method", "(response)"));
        return parsed;
    } catch (const json::parse_error& ex) {
        log("خطأ في تحليل JSON: " + std::string(ex.what()));
        return std::nullopt;
    }
}

/// إرسال رسالة JSON عبر stdout
void JsonRpcTransport::send_message(const json& msg) {
    std::string body = msg.dump();
    std::string header = "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

    // يجب إرسال الترويسة والمحتوى ذرياً
    std::fwrite(header.c_str(), 1, header.size(), stdout);
    std::fwrite(body.c_str(), 1, body.size(), stdout);
    std::fflush(stdout);

    log("→ أُرسلت: " + body.substr(0, std::min(body.size(), size_t(200))));
}

/// إرسال إشعار (بدون id)
void JsonRpcTransport::send_notification(const std::string& method, const json& params) {
    json msg = {
        {"jsonrpc", "2.0"},
        {"method", method},
        {"params", params}
    };
    send_message(msg);
}

/// إرسال رد خطأ
void JsonRpcTransport::send_error(const json& id, int code, const std::string& message) {
    json msg = {
        {"jsonrpc", "2.0"},
        {"id", id},
        {"error", {
            {"code", code},
            {"message", message}
        }}
    };
    send_message(msg);
}

// ══════════════════════════════════════════════════════════════════════════════
//  معالجة الرسائل الواردة
// ══════════════════════════════════════════════════════════════════════════════

void JsonRpcTransport::handle_message(const json& msg) {
    std::string method = msg.value("method", "");

    if (msg.contains("id")) {
        // طلب (Request) - يتوقع رداً
        auto id = msg["id"];

        try {
            json result = handle_request(method, msg.value("params", json::object()));
            json response = {
                {"jsonrpc", "2.0"},
                {"id", id},
                {"result", result}
            };
            send_message(response);
        } catch (const std::exception& ex) {
            // خطأ داخلي
            send_error(id, -32603, std::string("خطأ داخلي: ") + ex.what());
        }
    } else {
        // إشعار (Notification) - لا يتوقع رداً
        try {
            handle_notification(method, msg.value("params", json::object()));
        } catch (const std::exception& ex) {
            log("خطأ في معالجة إشعار " + method + ": " + ex.what());
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
//  معالجة الطلبات (تتوقع رداً)
// ══════════════════════════════════════════════════════════════════════════════

json JsonRpcTransport::handle_request(const std::string& method, const json& params) {

    // ════════════════════════════════════════════════════
    //  دورة الحياة
    // ════════════════════════════════════════════════════

    if (method == "initialize") {
        // تهيئة الخادم
        std::string root_uri;
        if (params.contains("rootUri") && !params["rootUri"].is_null()) {
            root_uri = params["rootUri"].get<std::string>();
        }

        auto caps = engine_.initialize(root_uri);
        auto info = engine_.get_server_info();

        // ربط ناشر التشخيصات
        engine_.set_diagnostics_publisher(
            [this](const DocumentUri& uri, const std::vector<Diagnostic>& diags) {
                publish_diagnostics(uri, diags);
            }
        );

        // بناء رد initialize
        json result;
        result["capabilities"] = json::object();

        // مزامنة المستندات
        result["capabilities"]["textDocumentSync"] = {
            {"openClose", caps.text_document_sync.open_close},
            {"change", caps.text_document_sync.change}
        };

        // الإكمال التلقائي
        result["capabilities"]["completionProvider"] = {
            {"triggerCharacters", caps.completion_provider.trigger_characters},
            {"resolveProvider", caps.completion_provider.resolve_provider}
        };

        // توقيعات الدوال
        result["capabilities"]["signatureHelpProvider"] = {
            {"triggerCharacters", caps.signature_help_provider.trigger_characters},
            {"retriggerCharacters", caps.signature_help_provider.retrigger_characters}
        };

        // ميزات أساسية
        result["capabilities"]["hoverProvider"] = caps.hover_provider;
        result["capabilities"]["definitionProvider"] = caps.definition_provider;
        result["capabilities"]["declarationProvider"] = caps.declaration_provider;
        result["capabilities"]["referencesProvider"] = caps.references_provider;
        result["capabilities"]["documentHighlightProvider"] = caps.document_highlight_provider;
        result["capabilities"]["documentSymbolProvider"] = caps.document_symbol_provider;
        result["capabilities"]["workspaceSymbolProvider"] = caps.workspace_symbol_provider;
        result["capabilities"]["codeActionProvider"] = caps.code_action_provider;
        result["capabilities"]["documentFormattingProvider"] = caps.document_formatting_provider;
        result["capabilities"]["documentRangeFormattingProvider"] = caps.document_range_formatting_provider;
        result["capabilities"]["renameProvider"] = {
            {"prepareProvider", true}
        };

        // الرموز الدلالية
        result["capabilities"]["semanticTokensProvider"] = {
            {"full", caps.semantic_tokens_provider.full},
            {"range", caps.semantic_tokens_provider.range},
            {"legend", {
                {"tokenTypes", {
                    "namespace", "type", "class", "enum", "interface",
                    "struct", "typeParameter", "parameter", "variable",
                    "property", "enumMember", "event", "function",
                    "method", "macro", "keyword", "modifier", "comment",
                    "string", "number", "regexp", "operator", "decorator"
                }},
                {"tokenModifiers", {
                    "declaration", "definition", "readonly", "static",
                    "deprecated", "abstract", "async", "modification",
                    "documentation", "defaultLibrary"
                }}
            }}
        };

        // التلميحات المضمنة
        result["capabilities"]["inlayHintProvider"] = caps.inlay_hint_provider;

        // الطي
        result["capabilities"]["foldingRangeProvider"] = caps.folding_range_provider;

        // ══════════════════════════════════════════════════
        //  القدرات الثورية الجديدة - المرحلة ٣
        // ══════════════════════════════════════════════════

        // عدسات الكود
        if (caps.code_lens_provider) {
            result["capabilities"]["codeLensProvider"] = {
                {"resolveProvider", false}
            };
        }

        // نطاق التحديد الذكي
        result["capabilities"]["selectionRangeProvider"] = caps.selection_range_provider;

        // شجرة الاستدعاءات
        result["capabilities"]["callHierarchyProvider"] = caps.call_hierarchy_provider;

        // شجرة الأنواع (الوراثة)
        result["capabilities"]["typeHierarchyProvider"] = caps.type_hierarchy_provider;

        // روابط المستند
        if (caps.document_link_provider) {
            result["capabilities"]["documentLinkProvider"] = {
                {"resolveProvider", false}
            };
        }

        // التنسيق أثناء الكتابة
        if (caps.document_on_type_formatting_provider_enabled) {
            result["capabilities"]["documentOnTypeFormattingProvider"] = {
                {"firstTriggerCharacter", "}"},
                {"moreTriggerCharacter", json::array({"\n", ";"})}
            };
        }

        // معلومات الخادم
        result["serverInfo"] = {
            {"name", info.name},
            {"version", info.version}
        };

        return result;
    }

    if (method == "shutdown") {
        engine_.shutdown();
        return nullptr; // null response
    }

    // ════════════════════════════════════════════════════
    //  الإكمال التلقائي
    // ════════════════════════════════════════════════════

    if (method == "textDocument/completion") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        auto pos = parse_position(params["position"]);

        auto result = engine_.completion(uri, pos);

        json items_json = json::array();
        for (const auto& item : result.items) {
            items_json.push_back(completion_item_to_json(item));
        }

        return {
            {"isIncomplete", result.is_incomplete},
            {"items", items_json}
        };
    }

    // ════════════════════════════════════════════════════
    //  التلميحات (Hover)
    // ════════════════════════════════════════════════════

    if (method == "textDocument/hover") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        auto pos = parse_position(params["position"]);

        auto result = engine_.hover(uri, pos);
        if (!result.has_value()) {
            return nullptr;
        }

        return hover_to_json(result.value());
    }

    // ════════════════════════════════════════════════════
    //  الانتقال للتعريف
    // ════════════════════════════════════════════════════

    if (method == "textDocument/definition") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        auto pos = parse_position(params["position"]);

        auto result = engine_.definition(uri, pos);
        if (!result.has_value()) {
            return nullptr;
        }

        return location_to_json(result.value());
    }

    // ════════════════════════════════════════════════════
    //  المراجع
    // ════════════════════════════════════════════════════

    if (method == "textDocument/references") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        auto pos = parse_position(params["position"]);

        auto results = engine_.references(uri, pos);

        json arr = json::array();
        for (const auto& loc : results) {
            arr.push_back(location_to_json(loc));
        }
        return arr;
    }

    // ════════════════════════════════════════════════════
    //  تمييز الرموز في المستند
    // ════════════════════════════════════════════════════

    if (method == "textDocument/documentHighlight") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        auto pos = parse_position(params["position"]);

        auto highlights = engine_.document_highlights(uri, pos);

        json arr = json::array();
        for (const auto& dh : highlights) {
            json j;
            j["range"] = range_to_json(dh.range);
            j["kind"] = static_cast<int>(dh.kind);
            arr.push_back(j);
        }
        return arr;
    }

    // ════════════════════════════════════════════════════
    //  رموز المستند
    // ════════════════════════════════════════════════════

    if (method == "textDocument/documentSymbol") {
        auto uri = params["textDocument"]["uri"].get<std::string>();

        auto symbols = engine_.document_symbols(uri);

        json arr = json::array();
        for (const auto& sym : symbols) {
            arr.push_back(document_symbol_to_json(sym));
        }
        return arr;
    }

    // ════════════════════════════════════════════════════
    //  رموز مساحة العمل (Ctrl+T)
    // ════════════════════════════════════════════════════

    if (method == "workspace/symbol") {
        std::string query;
        if (params.contains("query")) {
            query = params["query"].get<std::string>();
        }

        auto symbols = engine_.workspace_symbols(query);

        json arr = json::array();
        for (const auto& sym : symbols) {
            json sym_json;
            sym_json["name"] = sym.name;
            sym_json["kind"] = static_cast<int>(sym.kind);
            sym_json["location"] = {
                {"uri", sym.location.uri},
                {"range", {
                    {"start", {{"line", sym.location.range.start.line}, {"character", sym.location.range.start.character}}},
                    {"end", {{"line", sym.location.range.end.line}, {"character", sym.location.range.end.character}}}
                }}
            };
            if (!sym.container_name.empty()) {
                sym_json["containerName"] = sym.container_name;
            }
            arr.push_back(sym_json);
        }
        return arr;
    }

    // ════════════════════════════════════════════════════
    //  الرموز الدلالية
    // ════════════════════════════════════════════════════

    if (method == "textDocument/semanticTokens/full") {
        auto uri = params["textDocument"]["uri"].get<std::string>();

        auto tokens = engine_.semantic_tokens_full(uri);
        return {{"data", tokens.data}};
    }

    if (method == "textDocument/semanticTokens/range") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        auto range = parse_range(params["range"]);

        auto tokens = engine_.semantic_tokens_range(uri, range);
        return {{"data", tokens.data}};
    }

    // ════════════════════════════════════════════════════
    //  توقيعات الدوال
    // ════════════════════════════════════════════════════

    if (method == "textDocument/signatureHelp") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        auto pos = parse_position(params["position"]);

        auto result = engine_.signature_help(uri, pos);

        json sigs = json::array();
        for (const auto& sig : result.signatures) {
            json sig_json;
            sig_json["label"] = sig.label;
            sig_json["documentation"] = sig.documentation;

            json params_json = json::array();
            for (const auto& p : sig.parameters) {
                params_json.push_back({
                    {"label", p.label},
                    {"documentation", p.documentation}
                });
            }
            sig_json["parameters"] = params_json;

            if (sig.active_parameter.has_value()) {
                sig_json["activeParameter"] = sig.active_parameter.value();
            }
            sigs.push_back(sig_json);
        }

        return {
            {"signatures", sigs},
            {"activeSignature", result.active_signature},
            {"activeParameter", result.active_parameter}
        };
    }

    // ════════════════════════════════════════════════════
    //  إعادة التسمية
    // ════════════════════════════════════════════════════

    if (method == "textDocument/prepareRename") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        auto pos = parse_position(params["position"]);

        auto result = engine_.prepare_rename(uri, pos);
        if (!result.has_value()) {
            return nullptr;
        }

        return {
            {"range", range_to_json(result->range)},
            {"placeholder", result->placeholder}
        };
    }

    if (method == "textDocument/rename") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        auto pos = parse_position(params["position"]);
        auto new_name = params["newName"].get<std::string>();

        auto result = engine_.rename(uri, pos, new_name);

        json changes = json::object();
        for (const auto& [doc_uri, edits] : result.changes) {
            json edits_json = json::array();
            for (const auto& edit : edits) {
                edits_json.push_back(text_edit_to_json(edit));
            }
            changes[doc_uri] = edits_json;
        }

        return {{"changes", changes}};
    }

    // ════════════════════════════════════════════════════
    //  التلميحات المضمنة
    // ════════════════════════════════════════════════════

    if (method == "textDocument/inlayHint") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        auto range = parse_range(params["range"]);

        auto hints = engine_.inlay_hints(uri, range);

        json arr = json::array();
        for (const auto& hint : hints) {
            arr.push_back(inlay_hint_to_json(hint));
        }
        return arr;
    }

    // ════════════════════════════════════════════════════
    //  الطي
    // ════════════════════════════════════════════════════

    if (method == "textDocument/foldingRange") {
        auto uri = params["textDocument"]["uri"].get<std::string>();

        auto ranges = engine_.folding_ranges(uri);

        json arr = json::array();
        for (const auto& fr : ranges) {
            arr.push_back(folding_range_to_json(fr));
        }
        return arr;
    }

    // ════════════════════════════════════════════════════
    //  إجراءات الكود
    // ════════════════════════════════════════════════════

    if (method == "textDocument/codeAction") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        auto range = parse_range(params["range"]);

        // تحليل التشخيصات من السياق
        std::vector<Diagnostic> context_diags;
        if (params.contains("context") && params["context"].contains("diagnostics")) {
            for (const auto& d : params["context"]["diagnostics"]) {
                Diagnostic diag;
                diag.range = parse_range(d["range"]);
                diag.message = d.value("message", "");
                if (d.contains("severity")) {
                    diag.severity = static_cast<DiagnosticSeverity>(d["severity"].get<int>());
                }
                diag.code = d.value("code", "");
                context_diags.push_back(diag);
            }
        }

        auto actions = engine_.code_actions(uri, range, context_diags);

        json arr = json::array();
        for (const auto& action : actions) {
            arr.push_back(code_action_to_json(action));
        }
        return arr;
    }

    // ════════════════════════════════════════════════════
    //  التنسيق
    // ════════════════════════════════════════════════════

    if (method == "textDocument/formatting") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        FormattingOptions opts;
        if (params.contains("options")) {
            opts.tab_size = params["options"].value("tabSize", 4);
            opts.insert_spaces = params["options"].value("insertSpaces", true);
            opts.trim_trailing_whitespace = params["options"].value("trimTrailingWhitespace", true);
            opts.insert_final_newline = params["options"].value("insertFinalNewline", true);
            opts.trim_final_newlines = params["options"].value("trimFinalNewlines", true);
        }

        auto edits = engine_.format_document(uri, opts);

        json arr = json::array();
        for (const auto& edit : edits) {
            arr.push_back(text_edit_to_json(edit));
        }
        return arr;
    }

    if (method == "textDocument/rangeFormatting") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        auto range = parse_range(params["range"]);
        FormattingOptions opts;
        if (params.contains("options")) {
            opts.tab_size = params["options"].value("tabSize", 4);
            opts.insert_spaces = params["options"].value("insertSpaces", true);
        }

        auto edits = engine_.format_range(uri, range, opts);

        json arr = json::array();
        for (const auto& edit : edits) {
            arr.push_back(text_edit_to_json(edit));
        }
        return arr;
    }

    // ════════════════════════════════════════════════════
    //  عدسات الكود (Code Lens)
    // ════════════════════════════════════════════════════

    if (method == "textDocument/codeLens") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        auto lenses = engine_.code_lens(uri);

        json arr = json::array();
        for (const auto& lens : lenses) {
            json j;
            j["range"] = range_to_json(lens.range);
            if (lens.command.has_value()) {
                j["command"] = {
                    {"title", lens.command->title},
                    {"command", lens.command->command}
                };
                if (!lens.command->arguments.empty()) {
                    j["command"]["arguments"] = lens.command->arguments;
                }
            }
            arr.push_back(j);
        }
        return arr;
    }

    // ════════════════════════════════════════════════════
    //  نطاق التحديد الذكي (Selection Range)
    // ════════════════════════════════════════════════════

    if (method == "textDocument/selectionRange") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        std::vector<Position> positions;
        if (params.contains("positions")) {
            for (const auto& p : params["positions"]) {
                positions.push_back(parse_position(p));
            }
        }

        auto ranges = engine_.selection_range(uri, positions);

        // تحويل SelectionRange إلى JSON مع الأب المتداخل
        std::function<json(const SelectionRange&)> sel_range_to_json;
        sel_range_to_json = [&](const SelectionRange& sr) -> json {
            json j;
            j["range"] = range_to_json(sr.range);
            if (sr.parent) {
                j["parent"] = sel_range_to_json(*sr.parent);
            }
            return j;
        };

        json arr = json::array();
        for (const auto& sr : ranges) {
            arr.push_back(sel_range_to_json(sr));
        }
        return arr;
    }

    // ════════════════════════════════════════════════════
    //  شجرة الاستدعاءات (Call Hierarchy)
    // ════════════════════════════════════════════════════

    if (method == "textDocument/prepareCallHierarchy") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        auto pos = parse_position(params["position"]);

        auto items = engine_.call_hierarchy_prepare(uri, pos);

        json arr = json::array();
        for (const auto& item : items) {
            json j;
            j["name"] = item.name;
            j["kind"] = static_cast<int>(item.kind);
            j["detail"] = item.detail;
            j["uri"] = item.uri;
            j["range"] = range_to_json(item.range);
            j["selectionRange"] = range_to_json(item.selection_range);
            arr.push_back(j);
        }
        return arr;
    }

    if (method == "callHierarchy/incomingCalls") {
        CallHierarchyItem item;
        if (params.contains("item")) {
            const auto& it = params["item"];
            item.name = it.value("name", "");
            item.kind = static_cast<SymbolKind>(it.value("kind", 12));
            item.detail = it.value("detail", "");
            item.uri = it.value("uri", "");
            if (it.contains("range")) item.range = parse_range(it["range"]);
            if (it.contains("selectionRange")) item.selection_range = parse_range(it["selectionRange"]);
        }

        auto calls = engine_.call_hierarchy_incoming(item);

        json arr = json::array();
        for (const auto& call : calls) {
            json j;
            j["from"] = {
                {"name", call.from.name},
                {"kind", static_cast<int>(call.from.kind)},
                {"detail", call.from.detail},
                {"uri", call.from.uri},
                {"range", range_to_json(call.from.range)},
                {"selectionRange", range_to_json(call.from.selection_range)}
            };
            json ranges = json::array();
            for (const auto& r : call.from_ranges) {
                ranges.push_back(range_to_json(r));
            }
            j["fromRanges"] = ranges;
            arr.push_back(j);
        }
        return arr;
    }

    if (method == "callHierarchy/outgoingCalls") {
        CallHierarchyItem item;
        if (params.contains("item")) {
            const auto& it = params["item"];
            item.name = it.value("name", "");
            item.kind = static_cast<SymbolKind>(it.value("kind", 12));
            item.detail = it.value("detail", "");
            item.uri = it.value("uri", "");
            if (it.contains("range")) item.range = parse_range(it["range"]);
            if (it.contains("selectionRange")) item.selection_range = parse_range(it["selectionRange"]);
        }

        auto calls = engine_.call_hierarchy_outgoing(item);

        json arr = json::array();
        for (const auto& call : calls) {
            json j;
            j["to"] = {
                {"name", call.to.name},
                {"kind", static_cast<int>(call.to.kind)},
                {"detail", call.to.detail},
                {"uri", call.to.uri},
                {"range", range_to_json(call.to.range)},
                {"selectionRange", range_to_json(call.to.selection_range)}
            };
            json ranges = json::array();
            for (const auto& r : call.from_ranges) {
                ranges.push_back(range_to_json(r));
            }
            j["fromRanges"] = ranges;
            arr.push_back(j);
        }
        return arr;
    }

    // ════════════════════════════════════════════════════
    //  شجرة الأنواع (Type Hierarchy)
    // ════════════════════════════════════════════════════

    if (method == "textDocument/prepareTypeHierarchy") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        auto pos = parse_position(params["position"]);

        auto items = engine_.type_hierarchy_prepare(uri, pos);

        json arr = json::array();
        for (const auto& item : items) {
            json j;
            j["name"] = item.name;
            j["kind"] = static_cast<int>(item.kind);
            j["detail"] = item.detail;
            j["uri"] = item.uri;
            j["range"] = range_to_json(item.range);
            j["selectionRange"] = range_to_json(item.selection_range);
            arr.push_back(j);
        }
        return arr;
    }

    if (method == "typeHierarchy/supertypes") {
        TypeHierarchyItem item;
        if (params.contains("item")) {
            const auto& it = params["item"];
            item.name = it.value("name", "");
            item.kind = static_cast<SymbolKind>(it.value("kind", 5));
            item.detail = it.value("detail", "");
            item.uri = it.value("uri", "");
            if (it.contains("range")) item.range = parse_range(it["range"]);
            if (it.contains("selectionRange")) item.selection_range = parse_range(it["selectionRange"]);
        }

        auto supertypes = engine_.type_hierarchy_supertypes(item);

        json arr = json::array();
        for (const auto& t : supertypes) {
            json j;
            j["name"] = t.name;
            j["kind"] = static_cast<int>(t.kind);
            j["detail"] = t.detail;
            j["uri"] = t.uri;
            j["range"] = range_to_json(t.range);
            j["selectionRange"] = range_to_json(t.selection_range);
            arr.push_back(j);
        }
        return arr;
    }

    if (method == "typeHierarchy/subtypes") {
        TypeHierarchyItem item;
        if (params.contains("item")) {
            const auto& it = params["item"];
            item.name = it.value("name", "");
            item.kind = static_cast<SymbolKind>(it.value("kind", 5));
            item.detail = it.value("detail", "");
            item.uri = it.value("uri", "");
            if (it.contains("range")) item.range = parse_range(it["range"]);
            if (it.contains("selectionRange")) item.selection_range = parse_range(it["selectionRange"]);
        }

        auto subtypes = engine_.type_hierarchy_subtypes(item);

        json arr = json::array();
        for (const auto& t : subtypes) {
            json j;
            j["name"] = t.name;
            j["kind"] = static_cast<int>(t.kind);
            j["detail"] = t.detail;
            j["uri"] = t.uri;
            j["range"] = range_to_json(t.range);
            j["selectionRange"] = range_to_json(t.selection_range);
            arr.push_back(j);
        }
        return arr;
    }

    // ════════════════════════════════════════════════════
    //  روابط المستند (Document Links)
    // ════════════════════════════════════════════════════

    if (method == "textDocument/documentLink") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        auto links = engine_.document_links(uri);

        json arr = json::array();
        for (const auto& link : links) {
            json j;
            j["range"] = range_to_json(link.range);
            if (!link.target.empty()) j["target"] = link.target;
            if (!link.tooltip.empty()) j["tooltip"] = link.tooltip;
            arr.push_back(j);
        }
        return arr;
    }

    // ════════════════════════════════════════════════════
    //  التنسيق أثناء الكتابة (On Type Formatting)
    // ════════════════════════════════════════════════════

    if (method == "textDocument/onTypeFormatting") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        auto pos = parse_position(params["position"]);
        auto ch = params.value("ch", "");
        FormattingOptions opts;
        if (params.contains("options")) {
            opts.tab_size = params["options"].value("tabSize", 4);
            opts.insert_spaces = params["options"].value("insertSpaces", true);
        }

        auto edits = engine_.on_type_formatting(uri, pos, ch, opts);

        json arr = json::array();
        for (const auto& edit : edits) {
            arr.push_back(text_edit_to_json(edit));
        }
        return arr;
    }

    // ════════════════════════════════════════════════════
    //  طريقة غير معروفة
    // ════════════════════════════════════════════════════

    log("طريقة غير معروفة: " + method);
    throw std::runtime_error("Method not found: " + method);
}

// ══════════════════════════════════════════════════════════════════════════════
//  معالجة الإشعارات (لا تتوقع رداً)
// ══════════════════════════════════════════════════════════════════════════════

void JsonRpcTransport::handle_notification(const std::string& method, const json& params) {

    if (method == "initialized") {
        // المحرر أكد التهيئة - لا شيء مطلوب
        log("المحرر أكد التهيئة (initialized)");
        return;
    }

    if (method == "exit") {
        // إنهاء الخادم
        log("استُقبل طلب الخروج (exit)");
        stop();
        return;
    }

    // ──── مزامنة المستندات ────

    if (method == "textDocument/didOpen") {
        TextDocumentItem item;
        item.uri = params["textDocument"]["uri"].get<std::string>();
        item.language_id = params["textDocument"].value("languageId", "sad");
        item.version = params["textDocument"].value("version", 0);
        item.text = params["textDocument"]["text"].get<std::string>();

        engine_.did_open(item);
        return;
    }

    if (method == "textDocument/didChange") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        int version = params["textDocument"].value("version", 0);

        std::vector<TextDocumentContentChangeEvent> changes;
        for (const auto& c : params["contentChanges"]) {
            TextDocumentContentChangeEvent change;
            if (c.contains("range")) {
                change.range = parse_range(c["range"]);
            }
            change.text = c["text"].get<std::string>();
            changes.push_back(change);
        }

        engine_.did_change(uri, changes, version);
        return;
    }

    if (method == "textDocument/didClose") {
        auto uri = params["textDocument"]["uri"].get<std::string>();
        engine_.did_close(uri);
        return;
    }

    if (method == "textDocument/didSave") {
        // حدث الحفظ - يمكن تشغيل تحليل إضافي
        log("حُفظ المستند: " + params["textDocument"]["uri"].get<std::string>());
        return;
    }

    // إشعارات أخرى نتجاهلها بصمت
    log("إشعار غير معالج: " + method);
}

// ══════════════════════════════════════════════════════════════════════════════
//  نشر التشخيصات
// ══════════════════════════════════════════════════════════════════════════════

void JsonRpcTransport::publish_diagnostics(const DocumentUri& uri,
                                            const std::vector<Diagnostic>& diagnostics) {
    json diags_json = json::array();
    for (const auto& d : diagnostics) {
        diags_json.push_back(diagnostic_to_json(d));
    }

    send_notification("textDocument/publishDiagnostics", {
        {"uri", uri},
        {"diagnostics", diags_json}
    });
}

// ══════════════════════════════════════════════════════════════════════════════
//  دوال تحويل البيانات
// ══════════════════════════════════════════════════════════════════════════════

Position JsonRpcTransport::parse_position(const json& j) {
    return {j["line"].get<int>(), j["character"].get<int>()};
}

Range JsonRpcTransport::parse_range(const json& j) {
    Range r;
    r.start = parse_position(j["start"]);
    r.end = parse_position(j["end"]);
    return r;
}

json JsonRpcTransport::position_to_json(const Position& pos) {
    return {{"line", pos.line}, {"character", pos.character}};
}

json JsonRpcTransport::range_to_json(const Range& r) {
    return {{"start", position_to_json(r.start)}, {"end", position_to_json(r.end)}};
}

json JsonRpcTransport::diagnostic_to_json(const Diagnostic& d) {
    json j;
    j["range"] = range_to_json(d.range);
    j["severity"] = static_cast<int>(d.severity);
    j["source"] = d.source;

    // استخدام الرسالة العربية إذا متوفرة، وإلا الرسالة الافتراضية
    j["message"] = d.message_ar.empty() ? d.message : d.message_ar;

    if (!d.code.empty()) {
        j["code"] = d.code;
    }

    if (!d.tags.empty()) {
        json tags = json::array();
        for (auto t : d.tags) {
            tags.push_back(static_cast<int>(t));
        }
        j["tags"] = tags;
    }

    if (!d.related.empty()) {
        json related = json::array();
        for (const auto& r : d.related) {
            related.push_back({
                {"location", {
                    {"uri", r.location.uri},
                    {"range", range_to_json(r.location.range)}
                }},
                {"message", r.message}
            });
        }
        j["relatedInformation"] = related;
    }

    return j;
}

json JsonRpcTransport::completion_item_to_json(const CompletionItem& item) {
    json j;
    j["label"] = item.label;
    j["kind"] = static_cast<int>(item.kind);

    if (!item.detail.empty()) {
        j["detail"] = item.detail;
    }

    if (!item.documentation.value.empty()) {
        j["documentation"] = {
            {"kind", item.documentation.kind},
            {"value", item.documentation.value}
        };
    }

    if (item.deprecated) {
        j["deprecated"] = true;
    }

    if (item.preselect) {
        j["preselect"] = true;
    }

    if (!item.sort_text.empty()) {
        j["sortText"] = item.sort_text;
    }

    if (!item.filter_text.empty()) {
        j["filterText"] = item.filter_text;
    }

    if (!item.insert_text.empty()) {
        j["insertText"] = item.insert_text;
    }

    if (item.insert_text_format != InsertTextFormat::PlainText) {
        j["insertTextFormat"] = static_cast<int>(item.insert_text_format);
    }

    if (item.text_edit.has_value()) {
        j["textEdit"] = text_edit_to_json(item.text_edit.value());
    }

    if (!item.additional_text_edits.empty()) {
        json edits = json::array();
        for (const auto& e : item.additional_text_edits) {
            edits.push_back(text_edit_to_json(e));
        }
        j["additionalTextEdits"] = edits;
    }

    return j;
}

json JsonRpcTransport::hover_to_json(const Hover& h) {
    json j;
    j["contents"] = {
        {"kind", h.contents.kind},
        {"value", h.contents.value}
    };
    if (h.range.has_value()) {
        j["range"] = range_to_json(h.range.value());
    }
    return j;
}

json JsonRpcTransport::location_to_json(const Location& loc) {
    return {
        {"uri", loc.uri},
        {"range", range_to_json(loc.range)}
    };
}

json JsonRpcTransport::document_symbol_to_json(const DocumentSymbol& sym) {
    json j;
    j["name"] = sym.name;
    j["kind"] = static_cast<int>(sym.kind);
    j["range"] = range_to_json(sym.range);
    j["selectionRange"] = range_to_json(sym.selection_range);

    if (!sym.detail.empty()) {
        j["detail"] = sym.detail;
    }

    if (sym.deprecated) {
        j["deprecated"] = true;
    }

    if (!sym.children.empty()) {
        json children = json::array();
        for (const auto& child : sym.children) {
            children.push_back(document_symbol_to_json(child));
        }
        j["children"] = children;
    }

    return j;
}

json JsonRpcTransport::code_action_to_json(const CodeAction& action) {
    json j;
    j["title"] = action.title;
    j["kind"] = code_action_kind_to_string(action.kind);

    if (action.is_preferred) {
        j["isPreferred"] = true;
    }

    if (!action.diagnostics.empty()) {
        json diags = json::array();
        for (const auto& d : action.diagnostics) {
            diags.push_back(diagnostic_to_json(d));
        }
        j["diagnostics"] = diags;
    }

    if (action.edit.has_value()) {
        json changes = json::object();
        for (const auto& [uri, edits] : action.edit->changes) {
            json edits_json = json::array();
            for (const auto& edit : edits) {
                edits_json.push_back(text_edit_to_json(edit));
            }
            changes[uri] = edits_json;
        }
        j["edit"] = {{"changes", changes}};
    }

    return j;
}

json JsonRpcTransport::text_edit_to_json(const TextEdit& edit) {
    return {
        {"range", range_to_json(edit.range)},
        {"newText", edit.new_text}
    };
}

json JsonRpcTransport::inlay_hint_to_json(const InlayHint& hint) {
    json j;
    j["position"] = position_to_json(hint.position);
    j["label"] = hint.label;
    j["kind"] = static_cast<int>(hint.kind);
    if (hint.padding_left) j["paddingLeft"] = true;
    if (hint.padding_right) j["paddingRight"] = true;
    return j;
}

json JsonRpcTransport::folding_range_to_json(const FoldingRange& fr) {
    json j;
    j["startLine"] = fr.start_line;
    j["endLine"] = fr.end_line;

    if (fr.start_character.has_value()) {
        j["startCharacter"] = fr.start_character.value();
    }
    if (fr.end_character.has_value()) {
        j["endCharacter"] = fr.end_character.value();
    }
    if (fr.kind.has_value()) {
        switch (fr.kind.value()) {
            case FoldingRangeKind::Comment: j["kind"] = "comment"; break;
            case FoldingRangeKind::Imports: j["kind"] = "imports"; break;
            case FoldingRangeKind::Region:  j["kind"] = "region";  break;
        }
    }
    if (!fr.collapsed_text.empty()) {
        j["collapsedText"] = fr.collapsed_text;
    }

    return j;
}

std::string JsonRpcTransport::code_action_kind_to_string(CodeActionKind kind) {
    switch (kind) {
        case CodeActionKind::QuickFix:               return "quickfix";
        case CodeActionKind::Refactor:                return "refactor";
        case CodeActionKind::RefactorExtract:         return "refactor.extract";
        case CodeActionKind::RefactorInline:          return "refactor.inline";
        case CodeActionKind::RefactorRewrite:         return "refactor.rewrite";
        case CodeActionKind::Source:                  return "source";
        case CodeActionKind::SourceOrganizeImports:   return "source.organizeImports";
        case CodeActionKind::SourceFixAll:            return "source.fixAll";
        default:                                      return "quickfix";
    }
}

// ══════════════════════════════════════════════════════════════════════════════
//  السجل
// ══════════════════════════════════════════════════════════════════════════════

void JsonRpcTransport::log(const std::string& message) {
    if (log_file_.is_open()) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        char buf[64];
        std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&time));
        log_file_ << "[" << buf << "] " << message << std::endl;
    }
}

} // namespace transport
} // namespace lsp
} // namespace sad
