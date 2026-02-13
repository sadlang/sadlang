/// الإكمال التلقائي LSP
///
/// يوفر اقتراحات الإكمال التلقائي بما في ذلك:
/// - الكلمات المفتاحية العربية
/// - الدوال والمتغيرات المحلية
/// - أعضاء الأصناف
/// - الوحدات المتاحة للاستيراد

#pragma once

#include "lsp_server.h"
#include <string>
#include <vector>

namespace sad {
namespace lsp {

/// نوع عنصر الإكمال
enum class CompletionItemKind {
    Text = 1, Method = 2, Function = 3, Constructor = 4,
    Field = 5, Variable = 6, Class = 7, Interface = 8,
    Module = 9, Property = 10, Unit = 11, Value = 12,
    Enum = 13, Keyword = 14, Snippet = 15, Color = 16,
    File = 17, Reference = 18, Folder = 19, EnumMember = 20,
    Constant = 21, Struct = 22, Event = 23, Operator = 24,
    TypeParameter = 25,
};

/// عنصر إكمال
struct CompletionItem {
    std::string label;
    CompletionItemKind kind = CompletionItemKind::Text;
    std::string detail;
    std::string documentation;
    std::string insert_text;
    bool is_snippet = false;
    std::string sort_text;
    std::string filter_text;
    bool deprecated = false;
    
    /// تحويل إلى JSON
    std::string toJson() const;
};

/// معالج الإكمال التلقائي
class LspCompletion {
public:
    explicit LspCompletion(LspServer& server);
    void registerHandlers();

private:
    std::string handleCompletion(const std::string& params);
    std::string handleCompletionResolve(const std::string& params);
    
    /// إكمالات الكلمات المفتاحية العربية
    std::vector<CompletionItem> getKeywordCompletions(const std::string& prefix);
    
    /// إكمالات القصاصات
    std::vector<CompletionItem> getSnippetCompletions(const std::string& prefix);
    
    LspServer& server_;
    std::vector<CompletionItem> keyword_items_;
    std::vector<CompletionItem> snippet_items_;
    
    void initKeywords();
    void initSnippets();
};

} // namespace lsp
} // namespace sad
