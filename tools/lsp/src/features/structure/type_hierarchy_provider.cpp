// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: type_hierarchy_provider.cpp
// الوصف: مزود شجرة الأنواع — يعرض شجرة الوراثة (الآباء والأبناء)
// ══════════════════════════════════════════════════════════════════════════════
//
// المميزات:
//   ✦ عرض شجرة الوراثة لأي صنف أو سمة
//   ✦ Supertypes: الآباء + السمات المنفذة
//   ✦ Subtypes: الأبناء + المنفذون
//   ✦ دعم الملفات المتعددة عبر فهرس الرموز
//
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"
#include <algorithm>

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  تحضير عنصر شجرة الأنواع
// ══════════════════════════════════════════════════════════════════════════════

std::vector<TypeHierarchyItem> LspEngine::type_hierarchy_prepare(
    const DocumentUri& uri, const Position& pos)
{
    std::vector<TypeHierarchyItem> result;

    auto doc = doc_store_->get(uri);
    if (!doc) return result;

    std::string word = doc_store_->get_word_at(uri, pos);
    if (word.empty()) return result;

    // البحث عن الرمز — يجب أن يكون صنف أو سمة أو بنية
    auto sym = index_->find_definition(word, uri, pos);
    if (!sym) return result;

    // نسمح فقط بأنواع OOP
    if (sym->kind != AnalyzedSymbolKind::Class &&
        sym->kind != AnalyzedSymbolKind::Interface &&
        sym->kind != AnalyzedSymbolKind::Struct) {
        return result;
    }

    TypeHierarchyItem item;
    item.name = sym->name;
    item.kind = (sym->kind == AnalyzedSymbolKind::Interface)
        ? SymbolKind::Interface
        : (sym->kind == AnalyzedSymbolKind::Struct ? SymbolKind::Struct : SymbolKind::Class);
    item.uri = sym->uri;
    item.range = sym->definition_range;
    item.selection_range = sym->name_range;

    // تفاصيل: عرض الأب إن وُجد
    if (sym->class_info && !sym->class_info->parent_class.empty()) {
        item.detail = "يرث " + sym->class_info->parent_class;
    }

    result.push_back(item);
    return result;
}

// ══════════════════════════════════════════════════════════════════════════════
//  الأنواع الفوقية (Supertypes) — الآباء والسمات المنفذة
// ══════════════════════════════════════════════════════════════════════════════

std::vector<TypeHierarchyItem> LspEngine::type_hierarchy_supertypes(
    const TypeHierarchyItem& item)
{
    std::vector<TypeHierarchyItem> result;

    // البحث عن الصنف في الفهرس
    auto sym = index_->find_definition(item.name, item.uri, item.selection_range.start);
    if (!sym || !sym->class_info) return result;

    // ──── ١. الصنف الأب ────
    if (!sym->class_info->parent_class.empty()) {
        auto parent = index_->find_definition(sym->class_info->parent_class, item.uri,
                                               Position{0, 0});
        if (parent) {
            TypeHierarchyItem parent_item;
            parent_item.name = parent->name;
            parent_item.kind = SymbolKind::Class;
            parent_item.uri = parent->uri;
            parent_item.range = parent->definition_range;
            parent_item.selection_range = parent->name_range;
            if (parent->class_info && !parent->class_info->parent_class.empty()) {
                parent_item.detail = "يرث " + parent->class_info->parent_class;
            }
            result.push_back(parent_item);
        }
    }

    // ──── ٢. السمات المنفذة ────
    if (!sym->class_info->interfaces.empty()) {
        for (const auto& trait_name : sym->class_info->interfaces) {
            auto trait = index_->find_definition(trait_name, item.uri, Position{0, 0});
            if (trait) {
                TypeHierarchyItem trait_item;
                trait_item.name = trait->name;
                trait_item.kind = SymbolKind::Interface;
                trait_item.uri = trait->uri;
                trait_item.range = trait->definition_range;
                trait_item.selection_range = trait->name_range;
                trait_item.detail = "سمة";
                result.push_back(trait_item);
            }
        }
    }

    return result;
}

// ══════════════════════════════════════════════════════════════════════════════
//  الأنواع الفرعية (Subtypes) — الأبناء والمنفذون
// ══════════════════════════════════════════════════════════════════════════════

std::vector<TypeHierarchyItem> LspEngine::type_hierarchy_subtypes(
    const TypeHierarchyItem& item)
{
    std::vector<TypeHierarchyItem> result;

    // نبحث في كل الرموز عن أصناف ترث من هذا الصنف
    auto all_symbols = index_->get_workspace_symbols("");

    for (const auto& sym : all_symbols) {
        if (sym.kind != AnalyzedSymbolKind::Class &&
            sym.kind != AnalyzedSymbolKind::Interface &&
            sym.kind != AnalyzedSymbolKind::Struct) {
            continue;
        }

        bool is_subtype = false;

        // هل يرث من هذا الصنف؟
        if (sym.class_info && sym.class_info->parent_class == item.name) {
            is_subtype = true;
        }

        // هل ينفذ هذه السمة؟
        if (sym.class_info && !sym.class_info->interfaces.empty()) {
            for (const auto& trait : sym.class_info->interfaces) {
                if (trait == item.name) {
                    is_subtype = true;
                    break;
                }
            }
        }

        if (is_subtype) {
            TypeHierarchyItem child_item;
            child_item.name = sym.name;
            child_item.kind = (sym.kind == AnalyzedSymbolKind::Interface)
                ? SymbolKind::Interface : SymbolKind::Class;
            child_item.uri = sym.uri;
            child_item.range = sym.definition_range;
            child_item.selection_range = sym.name_range;
            if (sym.class_info && !sym.class_info->parent_class.empty()) {
                child_item.detail = "يرث " + sym.class_info->parent_class;
            }
            result.push_back(child_item);
        }
    }

    return result;
}

} // namespace lsp
} // namespace sad
