// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: folding_provider.cpp
// الوصف: مزود نطاقات الطي - يمكّن المستخدم من طي/فتح كتل الكود
// ══════════════════════════════════════════════════════════════════════════════
//
// الطي (Folding) يسمح للمبرمج بإخفاء أجزاء الكود التي لا يعمل عليها.
// يدعم مزود الطي الأنواع التالية:
//
//   ✦ طي الدوال: من "دالة" إلى "نهاية" أو "}"
//   ✦ طي الأصناف: من "صنف" إلى "نهاية" أو "}"
//   ✦ طي الشروط: من "إذا" إلى "نهاية" أو "}"
//   ✦ طي الحلقات: من "بينما"/"لكل" إلى "نهاية" أو "}"
//   ✦ طي معالجة الأخطاء: من "حاول" إلى "}"
//   ✦ طي كتل التعليقات المتتالية
//   ✦ طي الاستيرادات المتتالية
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  ثوابت الكلمات المفتاحية (UTF-8)
// ══════════════════════════════════════════════════════════════════════════════
static const std::string KW_FUNC     = "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9"; // دالة
static const std::string KW_CLASS    = "\xd8\xb5\xd9\x86\xd9\x81";         // صنف
static const std::string KW_IF       = "\xd8\xa5\xd8\xb0\xd8\xa7";         // إذا
static const std::string KW_WHILE    = "\xd8\xa8\xd9\x8a\xd9\x86\xd9\x85\xd8\xa7"; // بينما
static const std::string KW_FOR      = "\xd9\x84\xd9\x83\xd9\x84";         // لكل
static const std::string KW_TRY      = "\xd8\xad\xd8\xa7\xd9\x88\xd9\x84"; // حاول
static const std::string KW_END      = "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9"; // نهاية
static const std::string KW_IMPORT   = "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x88\xd8\xb1\xd8\xaf"; // استورد

// ══════════════════════════════════════════════════════════════════════════════
//  تنفيذ نطاقات الطي
// ══════════════════════════════════════════════════════════════════════════════

std::vector<FoldingRange> LspEngine::folding_ranges(const DocumentUri& uri) {
    std::vector<FoldingRange> ranges;

    auto doc = doc_store_->get(uri);
    if (!doc) return ranges;

    auto lines = arabic::split_lines(doc->content);

    // ──── ١. طي كتل الأقواس المتعرجة { } ────
    // نستخدم مكدس (stack) لتتبع الأقواس المتعرجة
    std::vector<int> brace_stack; // أرقام أسطر فتح الأقواس

    for (int i = 0; i < static_cast<int>(lines.size()); i++) {
        const auto& line = lines[i];
        for (char c : line) {
            if (c == '{') {
                brace_stack.push_back(i);
            }
            else if (c == '}' && !brace_stack.empty()) {
                int start_line = brace_stack.back();
                brace_stack.pop_back();

                // لا نضيف طي لأقواس في نفس السطر
                if (i > start_line) {
                    FoldingRange fr;
                    fr.start_line = start_line;
                    fr.end_line = i;
                    fr.kind = FoldingRangeKind::Region;
                    // النص المختصر: نأخذ بداية السطر الأول
                    auto trimmed = lines[start_line];
                    size_t brace_pos = trimmed.find('{');
                    if (brace_pos != std::string::npos && brace_pos > 0) {
                        fr.collapsed_text = trimmed.substr(0, brace_pos) + "...";
                    }
                    ranges.push_back(fr);
                }
            }
        }
    }

    // ──── ٢. طي كتل "نهاية" ────
    // نبحث عن الكلمات المفتاحية التي تفتح كتلة ونربطها بـ "نهاية"
    struct BlockStart {
        int line;
        std::string keyword;
    };
    std::vector<BlockStart> block_stack;

    for (int i = 0; i < static_cast<int>(lines.size()); i++) {
        const auto& line = lines[i];
        std::string trimmed;
        for (char c : line) {
            if (c != ' ' && c != '\t' && c != '\r') trimmed += c;
            else if (!trimmed.empty()) break;
        }

        // فتح كتلة
        if (line.find(KW_FUNC) != std::string::npos ||
            line.find(KW_CLASS) != std::string::npos ||
            line.find(KW_IF) != std::string::npos ||
            line.find(KW_WHILE) != std::string::npos ||
            line.find(KW_FOR) != std::string::npos ||
            line.find(KW_TRY) != std::string::npos) {

            // نتأكد أن السطر لا يحتوي على "{" (تمت معالجته أعلاه)
            if (line.find('{') == std::string::npos) {
                std::string kw;
                if (line.find(KW_FUNC) != std::string::npos) kw = KW_FUNC;
                else if (line.find(KW_CLASS) != std::string::npos) kw = KW_CLASS;
                else if (line.find(KW_IF) != std::string::npos) kw = KW_IF;
                else if (line.find(KW_WHILE) != std::string::npos) kw = KW_WHILE;
                else if (line.find(KW_FOR) != std::string::npos) kw = KW_FOR;
                else if (line.find(KW_TRY) != std::string::npos) kw = KW_TRY;
                block_stack.push_back({i, kw});
            }
        }

        // إغلاق كتلة بـ "نهاية"
        if (line.find(KW_END) != std::string::npos && !block_stack.empty()) {
            auto start = block_stack.back();
            block_stack.pop_back();

            if (i > start.line) {
                FoldingRange fr;
                fr.start_line = start.line;
                fr.end_line = i;
                fr.kind = FoldingRangeKind::Region;

                // النص المختصر: الكلمة المفتاحية + "..."
                auto start_line_text = lines[start.line];
                size_t kw_end = start_line_text.find(start.keyword);
                if (kw_end != std::string::npos) {
                    kw_end += start.keyword.size();
                    // نقرأ حتى نهاية الكلمة التالية (مثل اسم الدالة)
                    while (kw_end < start_line_text.size() &&
                           (start_line_text[kw_end] == ' ' || start_line_text[kw_end] == '\t')) {
                        kw_end++;
                    }
                    while (kw_end < start_line_text.size() &&
                           start_line_text[kw_end] != '(' &&
                           start_line_text[kw_end] != '{' &&
                           start_line_text[kw_end] != ' ') {
                        kw_end++;
                    }
                    fr.collapsed_text = start_line_text.substr(0, kw_end) + "...";
                }

                ranges.push_back(fr);
            }
        }
    }

    // ──── ٣. طي كتل التعليقات المتتالية ────
    {
        int comment_start = -1;
        for (int i = 0; i < static_cast<int>(lines.size()); i++) {
            const auto& line = lines[i];
            // تخطي المسافات وفحص التعليق
            size_t j = 0;
            while (j < line.size() && (line[j] == ' ' || line[j] == '\t')) j++;

            bool is_comment = (j < line.size() && line[j] == '#');

            if (is_comment) {
                if (comment_start < 0) comment_start = i;
            } else {
                // انتهت كتلة التعليقات
                if (comment_start >= 0 && i - comment_start >= 3) { // 3 أسطر على الأقل
                    FoldingRange fr;
                    fr.start_line = comment_start;
                    fr.end_line = i - 1;
                    fr.kind = FoldingRangeKind::Comment;
                    fr.collapsed_text = "# ...";
                    ranges.push_back(fr);
                }
                comment_start = -1;
            }
        }
        // كتلة تعليقات في نهاية الملف
        if (comment_start >= 0 && static_cast<int>(lines.size()) - comment_start >= 3) {
            FoldingRange fr;
            fr.start_line = comment_start;
            fr.end_line = static_cast<int>(lines.size()) - 1;
            fr.kind = FoldingRangeKind::Comment;
            fr.collapsed_text = "# ...";
            ranges.push_back(fr);
        }
    }

    // ──── ٤. طي كتل الاستيرادات المتتالية ────
    {
        int import_start = -1;
        for (int i = 0; i < static_cast<int>(lines.size()); i++) {
            bool is_import = (lines[i].find(KW_IMPORT) != std::string::npos);

            if (is_import) {
                if (import_start < 0) import_start = i;
            } else {
                if (import_start >= 0 && i - import_start >= 2) { // سطران على الأقل
                    FoldingRange fr;
                    fr.start_line = import_start;
                    fr.end_line = i - 1;
                    fr.kind = FoldingRangeKind::Imports;
                    fr.collapsed_text = KW_IMPORT + " ...";
                    ranges.push_back(fr);
                }
                import_start = -1;
            }
        }
    }

    return ranges;
}

} // namespace lsp
} // namespace sad
