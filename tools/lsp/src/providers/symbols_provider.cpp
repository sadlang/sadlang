// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: symbols_provider.cpp
// الوصف: مزود رموز مساحة العمل - Ctrl+T للبحث عن أي رمز
// ══════════════════════════════════════════════════════════════════════════════
//
// يوفر هذا الملف ميزة البحث عن الرموز في مساحة العمل كاملة:
//   ✦ عند ضغط Ctrl+T أو Ctrl+P ثم # في VS Code
//   ✦ يبحث في كل الملفات المفتوحة عن الدوال والأصناف والمتغيرات
//   ✦ يدعم البحث الضبابي والبحث بالتشكيل والبحث بدون تشكيل
//   ✦ يعرض أيقونات مناسبة لكل نوع رمز
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  تحويل نوع الرمز من AnalyzedSymbolKind إلى SymbolKind الخاص ببروتوكول LSP
// ══════════════════════════════════════════════════════════════════════════════

static SymbolKind to_lsp_symbol_kind(AnalyzedSymbolKind kind) {
    switch (kind) {
        case AnalyzedSymbolKind::Function:  return SymbolKind::Function;
        case AnalyzedSymbolKind::Class:     return SymbolKind::Class;
        case AnalyzedSymbolKind::Variable:  return SymbolKind::Variable;
        case AnalyzedSymbolKind::Constant:  return SymbolKind::Constant;
        case AnalyzedSymbolKind::Method:    return SymbolKind::Method;
        case AnalyzedSymbolKind::Property:  return SymbolKind::Property;
        case AnalyzedSymbolKind::Enum:      return SymbolKind::Enum;
        case AnalyzedSymbolKind::Module:    return SymbolKind::Module;
        default:                            return SymbolKind::Variable;
    }
}

// ══════════════════════════════════════════════════════════════════════════════
//  تنفيذ البحث عن الرموز في مساحة العمل
// ══════════════════════════════════════════════════════════════════════════════

std::vector<SymbolInformation> LspEngine::workspace_symbols(const std::string& query) {
    std::vector<SymbolInformation> results;

    // الحصول على الرموز من جميع الملفات المفتوحة
    auto all_docs = doc_store_->get_all_uris();

    for (const auto& uri : all_docs) {
        auto symbols = index_->get_document_symbols(uri);

        for (const auto& sym : symbols) {
            // إذا كان هناك استعلام: فلترة بالبحث الضبابي
            if (!query.empty()) {
                std::string norm_query = arabic::normalize_arabic(query);
                std::string norm_name = arabic::normalize_arabic(sym.name);
                if (!arabic::fuzzy_match_arabic(norm_query, norm_name)) {
                    continue;
                }
            }

            // لا نعرض المتغيرات المحلية في بحث مساحة العمل
            if (sym.kind == AnalyzedSymbolKind::Variable && !sym.is_exported && sym.scope_owner.empty()) {
                // نعرض فقط المتغيرات العامة (غير المحلية)
                // المحلية هي التي ليست مصدّرة وليست ضمن صنف
                // سنعرضها إذا كان البحث مطابقاً تماماً
                if (!query.empty() && sym.name != query) continue;
            }

            SymbolInformation info;
            info.name = sym.name;
            info.kind = to_lsp_symbol_kind(sym.kind);

            // إضافة وصف النوع
            if (sym.func_info) {
                std::string sig = sym.name + "(";
                bool first = true;
                for (const auto& [pname, ptype] : sym.func_info->parameters) {
                    if (!first) sig += ", ";
                    first = false;
                    sig += pname;
                }
                sig += ")";
                info.name = sig;
            }

            // الموقع
            info.location.uri = uri;
            info.location.range = sym.definition_range;

            // اسم الحاوية (الصنف أو الملف)
            info.container_name = sym.scope_owner;

            results.push_back(info);
        }
    }

    return results;
}

} // namespace lsp
} // namespace sad

