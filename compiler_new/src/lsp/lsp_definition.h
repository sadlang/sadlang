/// الذهاب للتعريف وفهرس الرموز
///
/// يوفر خدمات التنقل: الذهاب للتعريف، الذهاب للتصريح،
/// وفهرس الرموز لبحث سريع.

#pragma once

#include "lsp_server.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace sad {
namespace lsp {

/// نوع الرمز
enum class SymbolKind {
    File = 1, Module = 2, Namespace = 3, Package = 4,
    Class = 5, Method = 6, Property = 7, Field = 8,
    Constructor = 9, Enum = 10, Interface = 11, Function = 12,
    Variable = 13, Constant = 14, String = 15, Number = 16,
    Boolean = 17, Array = 18, Object = 19, Key = 20,
    Null = 21, EnumMember = 22, Struct = 23, Event = 24,
    Operator = 25, TypeParameter = 26,
};

/// موقع رمز في الكود
struct SymbolLocation {
    std::string uri;
    int start_line = 0;
    int start_char = 0;
    int end_line = 0;
    int end_char = 0;
    
    std::string toJson() const;
};

/// إدخال في فهرس الرموز
struct SymbolEntry {
    std::string name;
    SymbolKind kind;
    SymbolLocation definition;
    SymbolLocation declaration;  // قد يكون فارغاً
    std::string container_name;  // اسم الحاوية (صنف، وحدة)
    std::vector<SymbolLocation> references;
    
    std::string toDocumentSymbolJson() const;
    std::string toWorkspaceSymbolJson() const;
};

/// فهرس الرموز
class SymbolIndex {
public:
    /// إضافة رمز
    void addSymbol(const SymbolEntry& entry);
    
    /// البحث عن تعريف رمز عند موقع معين
    SymbolLocation* findDefinition(const std::string& uri, int line, int character);
    
    /// البحث عن مراجع رمز
    std::vector<SymbolLocation> findReferences(const std::string& uri, int line, int character,
                                                bool include_declaration = true);
    
    /// البحث عن رموز في مستند
    std::vector<SymbolEntry> getDocumentSymbols(const std::string& uri);
    
    /// البحث عن رموز في مجلد العمل
    std::vector<SymbolEntry> searchWorkspaceSymbols(const std::string& query);
    
    /// مسح فهرس مستند
    void clearDocument(const std::string& uri);
    
    /// مسح كل الفهرس
    void clearAll();

private:
    /// رموز كل مستند (URI → رموز)
    std::unordered_map<std::string, std::vector<SymbolEntry>> document_symbols_;
    
    /// فهرس عكسي (اسم → إدخالات)
    std::unordered_map<std::string, std::vector<SymbolEntry*>> name_index_;
};

/// معالج الذهاب للتعريف
class LspDefinition {
public:
    explicit LspDefinition(LspServer& server);
    void registerHandlers();
    
    /// الحصول على فهرس الرموز
    SymbolIndex& symbolIndex() { return symbol_index_; }

private:
    std::string handleDefinition(const std::string& params);
    std::string handleDeclaration(const std::string& params);
    std::string handleTypeDefinition(const std::string& params);
    std::string handleImplementation(const std::string& params);
    std::string handleDocumentSymbol(const std::string& params);
    std::string handleWorkspaceSymbol(const std::string& params);
    
    LspServer& server_;
    SymbolIndex symbol_index_;
};

} // namespace lsp
} // namespace sad
