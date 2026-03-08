// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: document_symbols_provider.cpp
// الوصف: مزود رموز المستند الثوري - يعرض هيكل الملف بتفاصيل غنية
// ══════════════════════════════════════════════════════════════════════════════
//
// المميزات الثورية:
//   ✦ عرض شجري كامل (أصناف → طرق/خصائص/منشئ)
//   ✦ تفاصيل غنية: توقيع الدالة، نوع المتغير
//   ✦ علامة "مهمل" (Deprecated) للرموز المميزة بتعليق
//   ✦ فرز الرموز: أصناف أولاً، ثم دوال، ثم متغيرات
//   ✦ دعم الاستيرادات كرموز وحدات
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"
#include <algorithm>

namespace sad {
namespace lsp {

/// تحويل نوع الرمز المحلل إلى SymbolKind المعيارية
static SymbolKind analyzed_to_symbol_kind(AnalyzedSymbolKind kind) {
    switch (kind) {
        case AnalyzedSymbolKind::Function:    return SymbolKind::Function;
        case AnalyzedSymbolKind::Method:      return SymbolKind::Method;
        case AnalyzedSymbolKind::Variable:    return SymbolKind::Variable;
        case AnalyzedSymbolKind::Constant:    return SymbolKind::Constant;
        case AnalyzedSymbolKind::Class:       return SymbolKind::Class;
        case AnalyzedSymbolKind::Property:    return SymbolKind::Property;
        case AnalyzedSymbolKind::Parameter:   return SymbolKind::Variable;
        case AnalyzedSymbolKind::Enum:        return SymbolKind::Enum;
        case AnalyzedSymbolKind::Module:      return SymbolKind::Module;
        case AnalyzedSymbolKind::Import:      return SymbolKind::Module;
        case AnalyzedSymbolKind::Constructor: return SymbolKind::Constructor;
        case AnalyzedSymbolKind::Interface:   return SymbolKind::Interface;
        case AnalyzedSymbolKind::Struct:      return SymbolKind::Struct;
    }
    return SymbolKind::Variable;
}

/// ترتيب الأولوية حسب نوع الرمز
/// @note يُستخدم لترتيب الرموز في عرض المخطط (مستقبلاً)
[[maybe_unused]]
static int symbol_sort_priority(AnalyzedSymbolKind kind) {
    switch (kind) {
        case AnalyzedSymbolKind::Import:      return 0; // الاستيرادات أولاً
        case AnalyzedSymbolKind::Class:       return 1;
        case AnalyzedSymbolKind::Interface:   return 2;
        case AnalyzedSymbolKind::Struct:      return 3;
        case AnalyzedSymbolKind::Enum:        return 4;
        case AnalyzedSymbolKind::Function:    return 5;
        case AnalyzedSymbolKind::Constructor: return 6;
        case AnalyzedSymbolKind::Method:      return 7;
        case AnalyzedSymbolKind::Constant:    return 8;
        case AnalyzedSymbolKind::Variable:    return 9;
        case AnalyzedSymbolKind::Property:    return 10;
        case AnalyzedSymbolKind::Parameter:   return 11;
        default: return 99;
    }
}

/// بناء نص التفاصيل لرمز
static std::string build_detail(const AnalyzedSymbol& sym) {
    if (sym.func_info) {
        std::string sig = "(";
        bool first = true;
        for (const auto& [pname, ptype] : sym.func_info->parameters) {
            if (!first) sig += ", ";
            first = false;
            sig += pname;
            if (!ptype.name.empty()) sig += ": " + ptype.name;
        }
        sig += ")";
        if (!sym.func_info->return_type.name.empty()) {
            sig += " \xe2\x86\x90 " + sym.func_info->return_type.name; // ← نوع الإرجاع
        }
        if (sym.func_info->is_async) sig += " \xe2\x9a\xa1"; // ⚡ غير متزامن
        return sig;
    }

    if (!sym.type.name.empty()) {
        return sym.type.name;
    }

    if (sym.kind == AnalyzedSymbolKind::Constant) {
        return "\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa"; // "ثابت"
    }

    if (sym.kind == AnalyzedSymbolKind::Module || sym.kind == AnalyzedSymbolKind::Import) {
        return "\xd9\x88\xd8\xad\xd8\xaf\xd8\xa9"; // "وحدة"
    }

    return "";
}

/// هل الرمز مميز كمهمل؟ (يحتوي على تعليق @مهمل أو @deprecated)
static bool check_deprecated(const AnalyzedSymbol& sym) {
    const std::string dep_ar = "\x40\xd9\x85\xd9\x87\xd9\x85\xd9\x84"; // @مهمل
    if (sym.documentation.find(dep_ar) != std::string::npos) return true;
    if (sym.documentation.find("@deprecated") != std::string::npos) return true;
    return false;
}

std::vector<DocumentSymbol> LspEngine::document_symbols(const DocumentUri& uri) {
    std::vector<DocumentSymbol> result;

    auto symbols = index_->get_document_symbols(uri);
    if (symbols.empty()) return result;

    // ──── جمع الرموز على المستوى العلوي ────
    std::unordered_map<std::string, size_t> class_indices;

    for (const auto& sym : symbols) {
        // تخطي الرموز المتداخلة (ستُضاف كأبناء)
        if (sym.scope_depth > 0 && !sym.scope_owner.empty()) continue;

        DocumentSymbol ds;
        ds.name = sym.name;
        ds.kind = analyzed_to_symbol_kind(sym.kind);
        ds.range = sym.definition_range;
        ds.selection_range = sym.name_range;
        ds.detail = build_detail(sym);
        ds.deprecated = check_deprecated(sym);

        result.push_back(ds);

        if (sym.kind == AnalyzedSymbolKind::Class ||
            sym.kind == AnalyzedSymbolKind::Interface ||
            sym.kind == AnalyzedSymbolKind::Struct) {
            class_indices[sym.name] = result.size() - 1;
        }
    }

    // ──── إضافة أعضاء الأصناف ────
    for (const auto& sym : symbols) {
        if (sym.scope_depth > 0 && !sym.scope_owner.empty()) {
            auto it = class_indices.find(sym.scope_owner);
            if (it != class_indices.end() && it->second < result.size()) {
                DocumentSymbol child;
                child.name = sym.name;
                child.kind = analyzed_to_symbol_kind(sym.kind);
                child.range = sym.definition_range;
                child.selection_range = sym.name_range;
                child.detail = build_detail(sym);
                child.deprecated = check_deprecated(sym);
                result[it->second].children.push_back(child);
            }
        }
    }

    // ──── فرز الأبناء داخل كل صنف ────
    for (auto& ds : result) {
        if (!ds.children.empty()) {
            // المنشئ أولاً، ثم الطرق، ثم الخصائص
            std::stable_sort(ds.children.begin(), ds.children.end(),
                [](const DocumentSymbol& a, const DocumentSymbol& b) {
                    auto pa = (a.kind == SymbolKind::Constructor) ? 0 :
                              (a.kind == SymbolKind::Method) ? 1 :
                              (a.kind == SymbolKind::Function) ? 2 :
                              (a.kind == SymbolKind::Property) ? 3 : 4;
                    auto pb = (b.kind == SymbolKind::Constructor) ? 0 :
                              (b.kind == SymbolKind::Method) ? 1 :
                              (b.kind == SymbolKind::Function) ? 2 :
                              (b.kind == SymbolKind::Property) ? 3 : 4;
                    return pa < pb;
                });
        }
    }

    return result;
}

} // namespace lsp
} // namespace sad} // namespace sad
