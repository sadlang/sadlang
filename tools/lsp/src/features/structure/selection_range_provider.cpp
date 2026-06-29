// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: selection_range_provider.cpp
// الوصف: مزود نطاق التحديد الذكي - توسيع التحديد بذكاء
// ══════════════════════════════════════════════════════════════════════════════
//
// نطاق التحديد الذكي (Smart Selection) يمكّن المبرمج من توسيع
// التحديد بشكل تدريجي ومنطقي:
//
//   المستوى ١: الكلمة الحالية     → "مجموع"
//   المستوى ٢: التعبير الكامل     → "مجموع + ١"
//   المستوى ٣: الجملة الكاملة     → "متغير نتيجة = مجموع + ١"
//   المستوى ٤: الكتلة (بين {})    → كامل جسم إذا/بينما/دالة
//   المستوى ٥: الدالة/الصنف       → كامل تعريف الدالة
//   المستوى ٦: الملف كاملاً       → كل المحتوى
//
// يتم تفعيله عبر:
//   - ويندوز/لينكس: Alt+Shift+→ (توسيع) / Alt+Shift+← (تقليص)
//   - ماك: Ctrl+Shift+→ / Ctrl+Shift+←
//
// ═══════════════════════════════════════════════════════════════════════════
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة لتحديد حدود النطاقات
// ══════════════════════════════════════════════════════════════════════════════

/// إرجاع نطاق الكلمة عند موضع معين
/// نبحث عن حدود المعرّف: بداية ونهاية الكلمة
static Range get_word_range(const std::vector<std::string>& lines, const Position& pos) {
    if (pos.line >= static_cast<int>(lines.size())) {
        return {{pos.line, 0}, {pos.line, 0}};
    }
    const auto& line = lines[pos.line];

    // إيجاد بداية الكلمة: نرجع للخلف حتى نجد حرفاً غير معرّفي
    int start = std::min(pos.character, static_cast<int>(line.size()));
    while (start > 0) {
        unsigned char byte = static_cast<unsigned char>(line[start - 1]);
        // UTF-8: بايت أكبر من 0x7F أو حرف معرّفي ASCII
        if (byte >= 0x80 || std::isalnum(byte) || byte == '_') {
            start--;
        } else {
            break;
        }
    }

    // إيجاد نهاية الكلمة: نتقدم حتى نجد حرفاً غير معرّفي
    int end = std::min(pos.character, static_cast<int>(line.size()));
    while (end < static_cast<int>(line.size())) {
        unsigned char byte = static_cast<unsigned char>(line[end]);
        if (byte >= 0x80 || std::isalnum(byte) || byte == '_') {
            end++;
        } else {
            break;
        }
    }

    return {{pos.line, start}, {pos.line, end}};
}

/// إرجاع نطاق السطر كاملاً (بدون المسافات البادئة والزائدة)
static Range get_statement_range(const std::vector<std::string>& lines, int line_num) {
    if (line_num >= static_cast<int>(lines.size())) {
        return {{line_num, 0}, {line_num, 0}};
    }
    const auto& line = lines[line_num];

    // تجاهل المسافات البادئة
    int start = 0;
    while (start < static_cast<int>(line.size()) && (line[start] == ' ' || line[start] == '\t'))
        start++;

    // تجاهل المسافات الزائدة من النهاية
    int end = static_cast<int>(line.size());
    while (end > start && (line[end - 1] == ' ' || line[end - 1] == '\t' || line[end - 1] == '\r'))
        end--;

    return {{line_num, start}, {line_num, end}};
}

/// إيجاد الكتلة المحيطة (بين { و }) عند موضع معين
/// نستخدم مكدس الأقواس للعثور على الكتلة الصحيحة
static Range find_enclosing_block(const std::vector<std::string>& lines, const Position& pos) {
    // نبحث عن { المطابق للخلف
    int brace_depth = 0;
    int block_start_line = -1;
    int block_start_char = -1;

    // أولاً: البحث عن { المطابق للخلف
    for (int i = pos.line; i >= 0; i--) {
        const auto& line = lines[i];
        int end_col = (i == pos.line) ? std::min(pos.character, static_cast<int>(line.size())) : static_cast<int>(line.size());
        for (int j = end_col - 1; j >= 0; j--) {
            if (line[j] == '}') brace_depth++;
            else if (line[j] == '{') {
                if (brace_depth == 0) {
                    block_start_line = i;
                    block_start_char = j;
                    goto found_start;
                }
                brace_depth--;
            }
        }
    }
    found_start:

    if (block_start_line < 0) {
        // لم نجد كتلة - نرجع الملف كاملاً
        return {{0, 0}, {static_cast<int>(lines.size()) - 1,
                         static_cast<int>(lines.back().size())}};
    }

    // ثانياً: البحث عن } المطابق للأمام من بداية الكتلة
    brace_depth = 0;
    for (int i = block_start_line; i < static_cast<int>(lines.size()); i++) {
        const auto& line = lines[i];
        int start_col = (i == block_start_line) ? block_start_char : 0;
        for (int j = start_col; j < static_cast<int>(line.size()); j++) {
            if (line[j] == '{') brace_depth++;
            else if (line[j] == '}') {
                brace_depth--;
                if (brace_depth == 0) {
                    // وجدنا الكتلة: من سطر { إلى سطر }
                    return {{block_start_line, 0}, {i, static_cast<int>(line.size())}};
                }
            }
        }
    }

    // لم نجد } المطابق - نرجع من البداية حتى نهاية الملف
    return {{block_start_line, 0},
            {static_cast<int>(lines.size()) - 1,
             static_cast<int>(lines.back().size())}};
}

/// إيجاد الدالة/الصنف المحيط بالموضع
/// نبحث عن أقرب تعريف دالة أو صنف فوق الموضع
static Range find_enclosing_definition(const std::vector<AnalyzedSymbol>& symbols,
                                        const Position& pos,
                                        const std::vector<std::string>& lines) {
    // نبحث عن أقرب رمز (دالة أو صنف) يحتوي الموضع
    const AnalyzedSymbol* best = nullptr;
    for (const auto& sym : symbols) {
        if (sym.kind != AnalyzedSymbolKind::Function &&
            sym.kind != AnalyzedSymbolKind::Method &&
            sym.kind != AnalyzedSymbolKind::Class) {
            continue;
        }
        if (sym.is_builtin) continue;

        // نتحقق أن الرمز يحتوي الموضع
        if (sym.definition_range.contains(pos) ||
            sym.definition_range.start.line <= pos.line) {
            // نختار الأقرب (الأعمق)
            if (!best || sym.definition_range.start > best->definition_range.start) {
                best = &sym;
            }
        }
    }

    if (best) {
        return best->definition_range;
    }

    // لا يوجد تعريف محيط - نرجع الملف كاملاً
    return {{0, 0}, {static_cast<int>(lines.size()) - 1,
                     static_cast<int>(lines.back().size())}};
}

// ══════════════════════════════════════════════════════════════════════════════
//  التنفيذ الرئيسي: بناء شجرة نطاقات التحديد
// ══════════════════════════════════════════════════════════════════════════════
//
// نبني لكل موضع شجرة من النطاقات المتداخلة:
//   كلمة  ⊂  جملة  ⊂  كتلة  ⊂  دالة  ⊂  ملف
//
// حيث كل مستوى هو الأب (parent) للمستوى الأصغر
//

std::vector<SelectionRange> LspEngine::selection_range(
    const DocumentUri& uri,
    const std::vector<Position>& positions)
{
    std::vector<SelectionRange> results;

    auto doc = doc_store_->get(uri);
    if (!doc) return results;

    auto lines = arabic::split_lines(doc->content);
    if (lines.empty()) return results;

    auto symbols = index_->get_document_symbols(uri);

    for (const auto& pos : positions) {
        // ──── المستوى ٥: الملف كاملاً ────
        auto file_range = std::make_shared<SelectionRange>();
        file_range->range = {{0, 0},
                              {static_cast<int>(lines.size()) - 1,
                               static_cast<int>(lines.back().size())}};
        file_range->parent = nullptr;

        // ──── المستوى ٤: الدالة/الصنف المحيط ────
        auto def_r = find_enclosing_definition(symbols, pos, lines);
        auto def_range = std::make_shared<SelectionRange>();
        def_range->range = def_r;
        def_range->parent = file_range;

        // إذا كان نطاق التعريف = نطاق الملف، نتجاوزه
        auto parent_of_block = def_range;
        if (def_r.start == file_range->range.start && def_r.end == file_range->range.end) {
            parent_of_block = file_range;
        }

        // ──── المستوى ٣: الكتلة المحيطة { } ────
        auto block_r = find_enclosing_block(lines, pos);
        auto block_range = std::make_shared<SelectionRange>();
        block_range->range = block_r;
        block_range->parent = parent_of_block;

        auto parent_of_stmt = block_range;
        if (block_r.start == parent_of_block->range.start &&
            block_r.end == parent_of_block->range.end) {
            parent_of_stmt = parent_of_block;
        }

        // ──── المستوى ٢: الجملة (السطر) ────
        auto stmt_r = get_statement_range(lines, pos.line);
        auto stmt_range = std::make_shared<SelectionRange>();
        stmt_range->range = stmt_r;
        stmt_range->parent = parent_of_stmt;

        // ──── المستوى ١: الكلمة ────
        auto word_r = get_word_range(lines, pos);
        SelectionRange word_range;
        word_range.range = word_r;
        word_range.parent = stmt_range;

        results.push_back(word_range);
    }

    return results;
}

} // namespace lsp
} // namespace sad
