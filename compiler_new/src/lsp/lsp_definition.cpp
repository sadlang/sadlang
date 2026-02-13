/// تنفيذ الذهاب للتعريف وفهرس الرموز

#include "lsp_definition.h"
#include <sstream>
#include <algorithm>

namespace sad {
namespace lsp {

/// تحويل موقع إلى JSON
std::string SymbolLocation::toJson() const {
    std::ostringstream json;
    json << "{\"uri\":\"" << uri << "\","
         << "\"range\":{\"start\":{\"line\":" << start_line
         << ",\"character\":" << start_char
         << "},\"end\":{\"line\":" << end_line
         << ",\"character\":" << end_char << "}}}";
    return json.str();
}

/// تحويل رمز مستند إلى JSON
std::string SymbolEntry::toDocumentSymbolJson() const {
    std::ostringstream json;
    json << "{\"name\":\"" << name << "\","
         << "\"kind\":" << static_cast<int>(kind) << ","
         << "\"range\":{\"start\":{\"line\":" << definition.start_line
         << ",\"character\":" << definition.start_char
         << "},\"end\":{\"line\":" << definition.end_line
         << ",\"character\":" << definition.end_char << "}},"
         << "\"selectionRange\":{\"start\":{\"line\":" << definition.start_line
         << ",\"character\":" << definition.start_char
         << "},\"end\":{\"line\":" << definition.start_line
         << ",\"character\":" << (definition.start_char + static_cast<int>(name.size()))
         << "}}}";
    return json.str();
}

/// تحويل رمز مجلد عمل إلى JSON
std::string SymbolEntry::toWorkspaceSymbolJson() const {
    std::ostringstream json;
    json << "{\"name\":\"" << name << "\","
         << "\"kind\":" << static_cast<int>(kind) << ","
         << "\"location\":" << definition.toJson();
    if (!container_name.empty()) {
        json << ",\"containerName\":\"" << container_name << "\"";
    }
    json << "}";
    return json.str();
}

// ─── SymbolIndex ───

/// إضافة رمز للفهرس
void SymbolIndex::addSymbol(const SymbolEntry& entry) {
    auto& symbols = document_symbols_[entry.definition.uri];
    symbols.push_back(entry);
    name_index_[entry.name].push_back(&symbols.back());
}

/// البحث عن تعريف رمز
SymbolLocation* SymbolIndex::findDefinition(const std::string& uri, int line, int character) {
    // TODO: تحليل النص عند الموقع لتحديد اسم الرمز
    // ثم البحث في الفهرس
    return nullptr;
}

/// البحث عن مراجع
std::vector<SymbolLocation> SymbolIndex::findReferences(
    const std::string& uri, int line, int character, bool include_declaration) {
    // TODO: تحليل الرمز عند الموقع والبحث عن مراجعه
    return {};
}

/// رموز المستند
std::vector<SymbolEntry> SymbolIndex::getDocumentSymbols(const std::string& uri) {
    auto it = document_symbols_.find(uri);
    if (it != document_symbols_.end()) {
        return it->second;
    }
    return {};
}

/// بحث رموز مجلد العمل
std::vector<SymbolEntry> SymbolIndex::searchWorkspaceSymbols(const std::string& query) {
    std::vector<SymbolEntry> results;
    for (const auto& [uri, symbols] : document_symbols_) {
        for (const auto& sym : symbols) {
            if (sym.name.find(query) != std::string::npos) {
                results.push_back(sym);
            }
        }
    }
    return results;
}

/// مسح مستند
void SymbolIndex::clearDocument(const std::string& uri) {
    document_symbols_.erase(uri);
    // TODO: تنظيف name_index_ أيضاً
}

/// مسح الكل
void SymbolIndex::clearAll() {
    document_symbols_.clear();
    name_index_.clear();
}

// ─── LspDefinition ───

LspDefinition::LspDefinition(LspServer& server) : server_(server) {}

/// تسجيل المعالجات
void LspDefinition::registerHandlers() {
    server_.registerHandler("textDocument/definition",
        [this](const std::string& p) { return handleDefinition(p); });
    server_.registerHandler("textDocument/declaration",
        [this](const std::string& p) { return handleDeclaration(p); });
    server_.registerHandler("textDocument/typeDefinition",
        [this](const std::string& p) { return handleTypeDefinition(p); });
    server_.registerHandler("textDocument/implementation",
        [this](const std::string& p) { return handleImplementation(p); });
    server_.registerHandler("textDocument/documentSymbol",
        [this](const std::string& p) { return handleDocumentSymbol(p); });
    server_.registerHandler("workspace/symbol",
        [this](const std::string& p) { return handleWorkspaceSymbol(p); });
}

/// معالجة الذهاب للتعريف
std::string LspDefinition::handleDefinition(const std::string& params) {
    // TODO: تحليل الموقع، إيجاد الرمز، إرجاع موقع التعريف
    return "null";
}

/// معالجة الذهاب للتصريح
std::string LspDefinition::handleDeclaration(const std::string& params) {
    return "null";
}

/// معالجة الذهاب لتعريف النوع
std::string LspDefinition::handleTypeDefinition(const std::string& params) {
    return "null";
}

/// معالجة الذهاب للتنفيذ
std::string LspDefinition::handleImplementation(const std::string& params) {
    return "null";
}

/// معالجة رموز المستند
std::string LspDefinition::handleDocumentSymbol(const std::string& params) {
    // TODO: استخراج URI وإرجاع رموز المستند
    return "[]";
}

/// معالجة بحث رموز مجلد العمل
std::string LspDefinition::handleWorkspaceSymbol(const std::string& params) {
    // TODO: استخراج الاستعلام والبحث
    return "[]";
}

} // namespace lsp
} // namespace sad
