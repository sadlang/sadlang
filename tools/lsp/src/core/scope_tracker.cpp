// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: scope_tracker.cpp
// الوصف: متتبع النطاقات - يحدد أي متغيرات مرئية في أي موضع
// ══════════════════════════════════════════════════════════════════════════════
//
// متتبع النطاقات يبني شجرة من النطاقات المتداخلة:
//   ✦ النطاق العام (المستوى العلوي)
//   ✦ نطاق الدالة
//   ✦ نطاق الصنف
//   ✦ نطاق الحلقة/الشرط
//
// عند طلب المتغيرات المرئية في موضع معين، يصعد من النطاق
// المحلي إلى الأعلى ويجمع كل المتغيرات المرئية.
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include <algorithm>

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  المنشئ والمدمر
// ══════════════════════════════════════════════════════════════════════════════

ScopeTracker::ScopeTracker() = default;
ScopeTracker::~ScopeTracker() = default;

// ══════════════════════════════════════════════════════════════════════════════
//  بناء شجرة النطاقات من الرموز المحللة
// ══════════════════════════════════════════════════════════════════════════════

void ScopeTracker::build(const std::vector<AnalyzedSymbol>& symbols) {
    scopes_.clear();

    // نبني نطاقاً لكل دالة وصنف
    for (const auto& sym : symbols) {
        if (sym.kind == AnalyzedSymbolKind::Function ||
            sym.kind == AnalyzedSymbolKind::Method ||
            sym.kind == AnalyzedSymbolKind::Constructor) {

            ScopeInfo scope;
            scope.name = sym.name;
            scope.kind = sym.kind;
            scope.range = sym.definition_range;
            scope.depth = sym.scope_depth;

            // جمع المتغيرات التي تنتمي لهذا النطاق
            for (const auto& other : symbols) {
                if (other.scope_owner == sym.name &&
                    (other.kind == AnalyzedSymbolKind::Variable ||
                     other.kind == AnalyzedSymbolKind::Constant ||
                     other.kind == AnalyzedSymbolKind::Parameter)) {
                    scope.variables.push_back(other.name);
                }
            }

            scopes_.push_back(scope);
        }
        else if (sym.kind == AnalyzedSymbolKind::Class) {
            ScopeInfo scope;
            scope.name = sym.name;
            scope.kind = sym.kind;
            scope.range = sym.definition_range;
            scope.depth = sym.scope_depth;

            // جمع أعضاء الصنف
            for (const auto& other : symbols) {
                if (other.scope_owner == sym.name) {
                    scope.variables.push_back(other.name);
                }
            }

            scopes_.push_back(scope);
        }
    }

    // ترتيب النطاقات حسب بدايتها
    std::sort(scopes_.begin(), scopes_.end(),
              [](const ScopeInfo& a, const ScopeInfo& b) {
                  if (a.range.start.line != b.range.start.line)
                      return a.range.start.line < b.range.start.line;
                  return a.range.start.character < b.range.start.character;
              });
}

// ══════════════════════════════════════════════════════════════════════════════
//  الحصول على النطاق النشط عند موضع
// ══════════════════════════════════════════════════════════════════════════════

std::optional<ScopeInfo> ScopeTracker::get_scope_at(const Position& pos) const {
    // نبحث عن أعمق نطاق يحتوي على هذا الموضع
    const ScopeInfo* best = nullptr;

    for (const auto& scope : scopes_) {
        bool in_scope = false;

        // التحقق مما إذا كان الموضع داخل النطاق
        if (pos.line > scope.range.start.line && pos.line < scope.range.end.line) {
            in_scope = true;
        } else if (pos.line == scope.range.start.line &&
                   pos.character >= scope.range.start.character) {
            in_scope = true;
        } else if (pos.line == scope.range.end.line &&
                   pos.character <= scope.range.end.character) {
            in_scope = true;
        }

        if (in_scope) {
            if (!best || scope.depth > best->depth) {
                best = &scope;
            }
        }
    }

    if (best) return *best;
    return std::nullopt;
}

// ══════════════════════════════════════════════════════════════════════════════
//  الحصول على كل المتغيرات المرئية
// ══════════════════════════════════════════════════════════════════════════════

std::vector<AnalyzedSymbol> ScopeTracker::get_visible_symbols(
    const Position& pos,
    const std::vector<AnalyzedSymbol>& all_symbols) const
{
    std::vector<AnalyzedSymbol> visible;

    // النطاق النشط
    auto current_scope = get_scope_at(pos);

    for (const auto& sym : all_symbols) {
        bool is_visible = false;

        // ──── الرموز العامة (المستوى العلوي) مرئية دائماً ────
        if (sym.scope_depth == 0 && sym.scope_owner.empty()) {
            is_visible = true;
        }

        // ──── الرموز المدمجة مرئية دائماً ────
        if (sym.is_builtin) {
            is_visible = true;
        }

        // ──── الرموز المحلية: مرئية إذا كانت في نفس النطاق أو نطاق أب ────
        if (current_scope.has_value()) {
            // في نفس النطاق
            if (sym.scope_owner == current_scope->name) {
                is_visible = true;
            }

            // إذا كنا داخل صنف، أعضاء الصنف مرئية
            if (current_scope->kind == AnalyzedSymbolKind::Class ||
                current_scope->kind == AnalyzedSymbolKind::Method) {
                // نبحث عن الصنف الحاوي
                for (const auto& scope : scopes_) {
                    if (scope.kind == AnalyzedSymbolKind::Class &&
                        sym.scope_owner == scope.name) {
                        is_visible = true;
                        break;
                    }
                }
            }
        }

        // ──── الرموز المعرفة قبل الموضع الحالي ────
        if (sym.definition_range.start.line < pos.line ||
            (sym.definition_range.start.line == pos.line &&
             sym.definition_range.start.character <= pos.character)) {
            // الرمز معرف قبل الموضع الحالي
            if (sym.scope_depth == 0) {
                is_visible = true;
            }
        }

        if (is_visible) {
            visible.push_back(sym);
        }
    }

    return visible;
}

} // namespace lsp
} // namespace sad
