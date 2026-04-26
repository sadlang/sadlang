// ============================================================================
// (AR) تنفيذ تشخيصات تعارض الأنماط
//      هذا الملف يُنفِّذ أدواتين:
//      1. PatternFixGenerator — يُحلِّل التداخل بين نطاقين ويقترح حلولاً بالأولوية
//         التقنية: مقارنة min/max + خوارزمية بسيطة O(1) للتداخل
//      2. VisualPatternRenderer — يرسم صندوقاً بصرياً بالرسم الحرفي (box-drawing)
//         التقنية: mapping من النطاق إلى الشريط + ألوان ANSI للطرفيات
//
//      كيف يُستخدم: يُستدعى من محرك الأخطاء عند اكتشاف تعارض في جملة طابق/match
//      الاعتماديات: pattern_diagnostics.h فقط (لا اعتماد على مكونات المترجم)
//
// (EN) Implementation of pattern conflict diagnostics.
//      PatternFixGenerator: O(1) range overlap analysis with prioritized suggestions.
//      VisualPatternRenderer: box-drawing chart with ANSI colors and numeric scale.
//      Called from the error engine when a match/طابق pattern conflict is detected.
//
// المكون: shared/errors
// المسار: shared/errors/src/pattern_diagnostics.cpp
// استُبدلت به:
//   - compiler/src/diagnostics/pattern_fix_suggestions.cpp (Fix #26)
//   - compiler/src/diagnostics/visual_pattern_warning.cpp (Fix #26)
// ============================================================================

// ─── القسم 1: الرأس الذاتي ───────────────────────────────────────────────────
#include "pattern_diagnostics.h"

// ─── القسم 2: رؤوس المكتبة القياسية ─────────────────────────────────────────
#include <iomanip>

namespace Sad {
namespace Errors {
namespace Pattern {

// ============================================================================
// PatternFixSuggestion::format()
// ============================================================================

std::string PatternFixSuggestion::format() const
{
    std::ostringstream ss;
    ss << "╔═══════════════════════════════════════╗\n";
    ss << "║ 💡 " << title_ar << "\n";
    ss << "╠═══════════════════════════════════════╣\n";
    ss << "║ " << explanation_ar << "\n";

    if (!changes.empty()) {
        ss << "╠═══════════════════════════════════════╣\n";
        ss << "║ التغييرات:\n";
        for (const auto& c : changes) {
            ss << "║   • " << c.toString() << "\n";
        }
    }

    if (!is_safe) {
        ss << "╠═══════════════════════════════════════╣\n";
        ss << "║ ⚠️ تحذير: هذا الإصلاح قد يغير السلوك\n";
    }

    ss << "╚═══════════════════════════════════════╝\n";
    return ss.str();
}

// ============================================================================
// PatternFixGenerator
// ============================================================================

std::vector<PatternFixSuggestion> PatternFixGenerator::suggestFixes(
    const NumericRange& first,
    const NumericRange& second) const
{
    std::vector<PatternFixSuggestion> suggestions;

    if (!first.overlaps(second))
        return suggestions;

    // (AR) حالة: النطاقان متطابقان → إزالة مكرر
    if (first.equals(second)) {
        suggestions.push_back(makeRemoveDuplicateSuggestion(second));
        return suggestions;
    }

    // (AR) حالة: أحدهما محتوى بالكامل في الآخر → نقل للأعلى
    if (first.contains(second)) {
        auto s = makeMoveUpSuggestion(second, first);
        s.priority = 90;
        suggestions.push_back(std::move(s));
    } else if (second.contains(first)) {
        auto s = makeMoveUpSuggestion(first, second);
        s.priority = 90;
        suggestions.push_back(std::move(s));
    }

    // (AR) اقتراح التقسيم دائماً (أعلى أولوية للتداخل الجزئي)
    auto split = makeSplitSuggestion(first, second);
    split.priority = 100;
    suggestions.push_back(std::move(split));

    // (AR) ترتيب تنازلي بالأولوية
    std::sort(suggestions.begin(), suggestions.end(),
        [](const PatternFixSuggestion& a, const PatternFixSuggestion& b) {
            return a.priority > b.priority;
        });

    return suggestions;
}

PatternFixSuggestion PatternFixGenerator::makeSplitSuggestion(
    const NumericRange& first,
    const NumericRange& second) const
{
    // (AR) حساب الحدود الجديدة لإزالة التداخل
    int64_t overlap_start = std::max(first.min, second.min);
    int64_t overlap_end   = std::min(first.max, second.max);

    PatternFixSuggestion s;
    s.type = PatternFixType::SPLIT_RANGES;
    s.is_safe = true;

    s.title_ar = "تقسيم النطاقات المتداخلة";
    s.title_en = "Split overlapping ranges";

    std::ostringstream explain;
    explain << "النطاق " << first.min << ".." << first.max
            << " يتداخل مع " << second.min << ".." << second.max
            << " في المجال " << overlap_start << ".." << overlap_end;
    s.explanation_ar = explain.str();

    // (AR) التغيير المقترح للنطاق الأول
    std::string new_max1 = std::to_string(overlap_start - 1);
    std::string new_min2 = std::to_string(overlap_end + 1);

    if (first.line > 0) {
        CodeChange c1;
        c1.line     = first.line;
        c1.col_start = 0;
        c1.col_end   = 0;
        c1.old_text  = std::to_string(first.min) + ".." + std::to_string(first.max);
        c1.new_text  = std::to_string(first.min) + ".." + new_max1;
        s.changes.push_back(c1);
    }

    if (second.line > 0) {
        CodeChange c2;
        c2.line     = second.line;
        c2.col_start = 0;
        c2.col_end   = 0;
        c2.old_text  = std::to_string(second.min) + ".." + std::to_string(second.max);
        c2.new_text  = new_min2 + ".." + std::to_string(second.max);
        s.changes.push_back(c2);
    }

    return s;
}

PatternFixSuggestion PatternFixGenerator::makeMoveUpSuggestion(
    const NumericRange& inner,
    const NumericRange& /*outer*/) const
{
    PatternFixSuggestion s;
    s.type = PatternFixType::MOVE_UP;
    s.is_safe = true;
    s.title_ar = "نقل النمط المحتوى للأعلى";
    s.title_en = "Move contained pattern up";

    std::ostringstream ex;
    ex << "النطاق " << inner.min << ".." << inner.max
       << " محتوى بالكامل — ضعه قبل النطاق الأكبر";
    s.explanation_ar = ex.str();

    if (inner.line > 0) {
        CodeChange c;
        c.line     = inner.line;
        c.col_start = 0; c.col_end = 0;
        c.old_text = "(الموضع الحالي)";
        c.new_text = "(انقله قبل النطاق المحتوِي)";
        s.changes.push_back(c);
    }

    return s;
}

PatternFixSuggestion PatternFixGenerator::makeRemoveDuplicateSuggestion(
    const NumericRange& duplicate) const
{
    PatternFixSuggestion s;
    s.type = PatternFixType::REMOVE_DUPLICATE;
    s.is_safe = true;
    s.title_ar = "إزالة النمط المكرر";
    s.title_en = "Remove duplicate pattern";

    std::ostringstream ex;
    ex << "النطاق " << duplicate.min << ".." << duplicate.max << " مكرر";
    s.explanation_ar = ex.str();

    if (duplicate.line > 0) {
        CodeChange c;
        c.line     = duplicate.line;
        c.col_start = 0; c.col_end = 0;
        c.old_text = std::to_string(duplicate.min) + ".." + std::to_string(duplicate.max);
        c.new_text = "(احذف هذا النمط)";
        s.changes.push_back(c);
    }

    return s;
}

// ============================================================================
// VisualPatternRenderer — helper methods
// ============================================================================

void VisualPatternRenderer::drawHLine(std::ostringstream& ss) const
{
    ss << settings_.box_v;
    for (int i = 0; i < settings_.chart_width; ++i)
        ss << settings_.box_h;
    ss << settings_.box_v << "\n";
}

void VisualPatternRenderer::drawScale(std::ostringstream& ss, int64_t lo, int64_t hi) const
{
    // (AR) رسم مقياس الأرقام المبسَّط
    ss << settings_.box_v << "  ";
    int w = settings_.chart_width - 4;
    int steps = 5;
    for (int i = 0; i <= steps; ++i) {
        int64_t val = lo + (hi - lo) * i / steps;
        std::string lbl = std::to_string(val);
        int pos = w * i / steps;
        (void)pos;
        ss << std::setw(8) << val;
    }
    ss << " " << settings_.box_v << "\n";
}

void VisualPatternRenderer::drawTicks(std::ostringstream& ss, int64_t lo, int64_t hi) const
{
    (void)lo; (void)hi;
    ss << settings_.box_v << "  ";
    for (int i = 0; i < settings_.chart_width - 4; ++i) {
        ss << (i % 8 == 0 ? "┬" : "─");
    }
    ss << "  " << settings_.box_v << "\n";
}

std::string VisualPatternRenderer::renderBar(
    int64_t range_min, int64_t range_max,
    int64_t bar_min, int64_t bar_max,
    const std::string& symbol,
    const std::string& color) const
{
    int w = settings_.chart_width - 4;
    double span = static_cast<double>(range_max - range_min);
    if (span <= 0.0) span = 1.0;

    int start_pos = static_cast<int>((bar_min - range_min) / span * w);
    int end_pos   = static_cast<int>((bar_max - range_min) / span * w);
    start_pos = std::max(0, std::min(start_pos, w));
    end_pos   = std::max(0, std::min(end_pos, w));

    std::string bar;
    if (settings_.use_colors && !color.empty())
        bar += color;

    for (int i = 0; i < w; ++i) {
        if (i >= start_pos && i <= end_pos)
            bar += symbol;
        else
            bar += settings_.sym_empty;
    }

    if (settings_.use_colors && !color.empty())
        bar += Ansi::RESET;

    return bar;
}

// ============================================================================
// VisualPatternRenderer::renderOverlap()
// ============================================================================

std::string VisualPatternRenderer::renderOverlap(
    int64_t min1, int64_t max1, const std::string& name1,
    int64_t min2, int64_t max2, const std::string& name2,
    int line1, int line2) const
{
    std::ostringstream ss;

    int64_t lo = std::min(min1, min2);
    int64_t hi = std::max(max1, max2);

    // حساب التداخل
    int64_t ov_start = std::max(min1, min2);
    int64_t ov_end   = std::min(max1, max2);
    bool has_overlap = (ov_start <= ov_end);

    // الإطار العلوي
    ss << settings_.box_tl;
    for (int i = 0; i < settings_.chart_width; ++i) ss << settings_.box_h;
    ss << settings_.box_tr << "\n";

    // العنوان
    ss << settings_.box_v << "  ";
    if (settings_.use_colors) ss << Ansi::YELLOW << Ansi::BOLD;
    ss << "⚠️  تداخل في الأنماط";
    if (settings_.use_colors) ss << Ansi::RESET;
    int title_pad = settings_.chart_width - 22;
    for (int i = 0; i < title_pad; ++i) ss << " ";
    ss << settings_.box_v << "\n";

    // فاصل
    drawHLine(ss);

    // المقياس
    drawScale(ss, lo, hi);
    drawTicks(ss, lo, hi);

    // النمط الأول
    ss << settings_.box_v << "  ";
    std::string bar1 = renderBar(lo, hi, min1, max1,
        settings_.sym_first, settings_.use_colors ? Ansi::BLUE : "");
    ss << bar1;
    ss << " ← \"" << name1 << "\"";
    if (line1 > 0) ss << " (سطر " << line1 << ")";
    ss << "\n";

    // النمط الثاني
    ss << settings_.box_v << "  ";
    std::string bar2 = renderBar(lo, hi, min2, max2,
        settings_.sym_second, settings_.use_colors ? Ansi::GREEN : "");
    ss << bar2;
    ss << " ← \"" << name2 << "\"";
    if (line2 > 0) ss << " (سطر " << line2 << ")";
    ss << "\n";

    if (has_overlap) {
        // النطاق المتداخل
        ss << settings_.box_v << "  ";
        std::string bar_ov = renderBar(lo, hi, ov_start, ov_end,
            settings_.sym_overlap, settings_.use_colors ? Ansi::RED : "");
        ss << bar_ov;
        if (settings_.use_colors) ss << Ansi::RED << Ansi::BOLD;
        ss << " ← التداخل! (" << ov_start << ".." << ov_end << ")";
        if (settings_.use_colors) ss << Ansi::RESET;
        ss << "\n";

        drawHLine(ss);

        // اقتراح الحل
        ss << settings_.box_v << "  ";
        if (settings_.use_colors) ss << Ansi::GREEN;
        ss << "💡 الحل: استخدم أنماطاً غير متداخلة";
        if (settings_.use_colors) ss << Ansi::RESET;
        ss << "\n";
        ss << settings_.box_v << "      مثال: حالة " << min1 << ".." << (ov_start - 1)
           << ":  " << name1 << "\n";
        ss << settings_.box_v << "             حالة " << ov_start << ".." << max2
           << ":  " << name2 << "\n";
    }

    // الإطار السفلي
    ss << settings_.box_bl;
    for (int i = 0; i < settings_.chart_width; ++i) ss << settings_.box_h;
    ss << settings_.box_br << "\n";

    return ss.str();
}

} // namespace Pattern
} // namespace Errors
} // namespace Sad
