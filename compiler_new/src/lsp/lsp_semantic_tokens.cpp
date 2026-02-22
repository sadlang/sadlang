/// تنفيذ الرموز الدلالية

#include "lsp_semantic_tokens.h"
#include <sstream>

namespace sad {
namespace lsp {

/// أسماء أنواع الرموز الدلالية
static const char* TOKEN_TYPE_NAMES[] = {
    "namespace", "type", "class", "enum", "interface",
    "struct", "typeParameter", "parameter", "variable",
    "property", "enumMember", "event", "function",
    "method", "macro", "keyword", "modifier",
    "comment", "string", "number", "regexp",
    "operator", "decorator"
};

/// أسماء معدِّلات الرموز
static const char* TOKEN_MODIFIER_NAMES[] = {
    "declaration", "definition", "readonly", "static",
    "deprecated", "abstract", "async", "modification",
    "documentation", "defaultLibrary"
};

LspSemanticTokens::LspSemanticTokens(LspServer& server)
    : server_(server) {}

void LspSemanticTokens::registerHandlers() {
    server_.registerHandler("textDocument/semanticTokens/full",
        [this](const std::string& p) { return handleSemanticTokensFull(p); });
    server_.registerHandler("textDocument/semanticTokens/full/delta",
        [this](const std::string& p) { return handleSemanticTokensDelta(p); });
    server_.registerHandler("textDocument/semanticTokens/range",
        [this](const std::string& p) { return handleSemanticTokensRange(p); });
}

std::string LspSemanticTokens::buildLegendJson() {
    std::ostringstream ss;
    ss << "{\"tokenTypes\":[";
    for (int i = 0; i < static_cast<int>(SemanticTokenType::COUNT); i++) {
        if (i > 0) ss << ",";
        ss << "\"" << TOKEN_TYPE_NAMES[i] << "\"";
    }
    ss << "],\"tokenModifiers\":[";
    for (int i = 0; i < 10; i++) {
        if (i > 0) ss << ",";
        ss << "\"" << TOKEN_MODIFIER_NAMES[i] << "\"";
    }
    ss << "]}";
    return ss.str();
}

std::string LspSemanticTokens::handleSemanticTokensFull(const std::string& params) {
    // TODO: تحليل المستند بالكامل وإرجاع الرموز الدلالية
    resultId_++;
    std::ostringstream ss;
    ss << "{\"resultId\":\"" << resultId_ << "\",\"data\":[]}";
    return ss.str();
}

std::string LspSemanticTokens::handleSemanticTokensDelta(const std::string& params) {
    // TODO: حساب الفرق بين الإصدارات
    resultId_++;
    std::ostringstream ss;
    ss << "{\"resultId\":\"" << resultId_ << "\",\"edits\":[]}";
    return ss.str();
}

std::string LspSemanticTokens::handleSemanticTokensRange(const std::string& params) {
    // TODO: إرجاع الرموز لنطاق محدد فقط
    return "{\"data\":[]}";
}

std::string LspSemanticTokens::encodeTokens(const std::vector<SemanticToken>& tokens) {
    /// ترميز الرموز بالتنسيق النسبي: [deltaLine, deltaStart, length, type, modifiers]
    std::ostringstream ss;
    ss << "[";
    int prevLine = 0, prevChar = 0;
    for (size_t i = 0; i < tokens.size(); i++) {
        if (i > 0) ss << ",";
        int deltaLine = tokens[i].line - prevLine;
        int deltaChar = (deltaLine == 0) ? tokens[i].startChar - prevChar : tokens[i].startChar;
        ss << deltaLine << "," << deltaChar << ","
           << tokens[i].length << ","
           << static_cast<int>(tokens[i].type) << ","
           << tokens[i].modifiers;
        prevLine = tokens[i].line;
        prevChar = tokens[i].startChar;
    }
    ss << "]";
    return ss.str();
}

} // namespace lsp
} // namespace sad
