// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: on_type_formatting_provider.cpp
// الوصف: مزود التنسيق أثناء الكتابة - تنسيق تلقائي فوري
// ══════════════════════════════════════════════════════════════════════════════
//
// التنسيق أثناء الكتابة (On Type Formatting) يقوم بتنسيق الكود
// تلقائياً فور كتابة أحرف معينة:
//
//   ● عند كتابة }:
//      - ضبط المسافة البادئة تلقائياً لتطابق { المقابل
//      - مثال: إذا كانت { في المستوى ٤، نضع } في المستوى ٤ أيضاً
//
//   ● عند كتابة \n (سطر جديد):
//      - إضافة المسافة البادئة تلقائياً
//      - بعد { → مسافة إضافية
//      - بعد كلمات مفتاحية (إذا/بينما/لكل) → مسافة إضافية
//
//   ● عند كتابة ; (فاصلة منقوطة):
//      - حذف المسافات الزائدة من نهاية السطر
//      - توحيد المسافات بين العوامل
//
// ══════════════════════════════════════════════════════════════════════════════
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة لحساب المسافات البادئة
// ══════════════════════════════════════════════════════════════════════════════

/// حساب مستوى المسافة البادئة لسطر معين
/// نحسب عدد المسافات في بداية السطر ونقسمها على حجم الوحدة
static int get_indent_level(const std::string& line, int tab_size) {
    int spaces = 0;
    for (char c : line) {
        if (c == ' ') spaces++;
        else if (c == '\t') spaces += tab_size;
        else break;
    }
    return spaces / tab_size;
}

/// إنشاء سلسلة المسافة البادئة
/// حسب الإعدادات: مسافات أو تبويب
static std::string make_indent(int level, bool insert_spaces, int tab_size) {
    if (level <= 0) return "";
    if (insert_spaces) {
        return std::string(level * tab_size, ' ');
    } else {
        return std::string(level, '\t');
    }
}

/// إيجاد { المطابق لـ } الحالي
/// نرجع مستوى المسافة البادئة للسطر الذي يحتوي {
static int find_matching_brace_indent(const std::vector<std::string>& lines,
                                       int close_brace_line, int tab_size) {
    int depth = 0;
    for (int i = close_brace_line; i >= 0; i--) {
        const auto& line = lines[i];
        // نمسح السطر من اليمين لليسار
        for (int j = static_cast<int>(line.size()) - 1; j >= 0; j--) {
            if (line[j] == '}') depth++;
            else if (line[j] == '{') {
                depth--;
                if (depth == 0) {
                    // وجدنا { المطابق - نرجع مستوى المسافة البادئة لسطره
                    return get_indent_level(line, tab_size);
                }
            }
        }
    }
    return 0;  // لم نجد { المطابق
}

/// التحقق مما إذا كان السطر ينتهي بكلمة مفتاحية تحتاج كتلة
/// مثل: إذا، بينما، لكل، والا، دالة
static bool line_needs_indent_after(const std::string& line) {
    // تقليم المسافات من نهاية السطر
    std::string trimmed = line;
    while (!trimmed.empty() && (trimmed.back() == ' ' || trimmed.back() == '\t' || trimmed.back() == '\r'))
        trimmed.pop_back();

    if (trimmed.empty()) return false;

    // إذا انتهى السطر بـ { يحتاج مسافة إضافية
    if (trimmed.back() == '{') return true;

    // إذا انتهى بـ : (مثل حالة: في التبديل)
    if (trimmed.back() == ':') return true;

    return false;
}

// ══════════════════════════════════════════════════════════════════════════════
//  التنفيذ الرئيسي: التنسيق عند كتابة حرف معين
// ══════════════════════════════════════════════════════════════════════════════

std::vector<TextEdit> LspEngine::on_type_formatting(
    const DocumentUri& uri,
    const Position& position,
    const std::string& ch,
    const FormattingOptions& options)
{
    std::vector<TextEdit> edits;

    auto doc = doc_store_->get(uri);
    if (!doc) return edits;

    auto lines = arabic::split_lines(doc->content);
    if (lines.empty()) return edits;

    int tab_size = options.tab_size;
    bool insert_spaces = options.insert_spaces;

    // ════════════════════════════════════════════════════════
    //  عند كتابة } - ضبط المسافة البادئة
    // ════════════════════════════════════════════════════════
    if (ch == "}") {
        int current_line = position.line;
        if (current_line >= static_cast<int>(lines.size())) return edits;

        const auto& line = lines[current_line];

        // حساب المسافة البادئة المطلوبة (نفس مستوى { المقابل)
        int target_indent = find_matching_brace_indent(lines, current_line, tab_size);
        int current_indent = get_indent_level(line, tab_size);

        if (target_indent != current_indent) {
            // إيجاد نهاية المسافة البادئة الحالية
            int indent_end = 0;
            while (indent_end < static_cast<int>(line.size()) &&
                   (line[indent_end] == ' ' || line[indent_end] == '\t'))
                indent_end++;

            // استبدال المسافة البادئة الحالية بالصحيحة
            TextEdit edit;
            edit.range = {{current_line, 0}, {current_line, indent_end}};
            edit.new_text = make_indent(target_indent, insert_spaces, tab_size);
            edits.push_back(edit);
        }
    }

    // ════════════════════════════════════════════════════════
    //  عند كتابة سطر جديد - المسافة البادئة التلقائية
    // ════════════════════════════════════════════════════════
    else if (ch == "\n") {
        int current_line = position.line;
        // السطر السابق (الذي كتبنا بعده Enter)
        int prev_line = current_line - 1;
        if (prev_line < 0 || prev_line >= static_cast<int>(lines.size())) return edits;

        const auto& prev = lines[prev_line];
        int prev_indent = get_indent_level(prev, tab_size);

        // تحديد المسافة البادئة المطلوبة
        int target_indent = prev_indent;

        // إذا كان السطر السابق ينتهي بـ { أو :، نزيد المسافة
        if (line_needs_indent_after(prev)) {
            target_indent++;
        }

        // إذا كان السطر الحالي يبدأ بـ }، نُنقص المسافة
        if (current_line < static_cast<int>(lines.size())) {
            std::string current = lines[current_line];
            std::string trimmed_current = current;
            size_t first = trimmed_current.find_first_not_of(" \t");
            if (first != std::string::npos && trimmed_current[first] == '}') {
                target_indent = std::max(0, target_indent - 1);
            }
        }

        // إنشاء المسافة البادئة
        std::string indent = make_indent(target_indent, insert_spaces, tab_size);

        if (!indent.empty() && current_line < static_cast<int>(lines.size())) {
            const auto& current = lines[current_line];
            // حساب المسافة البادئة الموجودة حالياً
            int existing_indent_end = 0;
            while (existing_indent_end < static_cast<int>(current.size()) &&
                   (current[existing_indent_end] == ' ' || current[existing_indent_end] == '\t'))
                existing_indent_end++;

            TextEdit edit;
            edit.range = {{current_line, 0}, {current_line, existing_indent_end}};
            edit.new_text = indent;
            edits.push_back(edit);
        }
    }

    // ════════════════════════════════════════════════════════
    //  عند كتابة ; - تنظيف السطر
    // ════════════════════════════════════════════════════════
    else if (ch == ";") {
        int current_line = position.line;
        if (current_line >= static_cast<int>(lines.size())) return edits;

        const auto& line = lines[current_line];

        // حذف المسافات الزائدة قبل ;
        // مثال: "متغير س = ٥  ;" → "متغير س = ٥;"
        int semicol_pos = -1;
        for (int j = static_cast<int>(line.size()) - 1; j >= 0; j--) {
            if (line[j] == ';') {
                semicol_pos = j;
                break;
            }
        }

        if (semicol_pos > 0) {
            int space_start = semicol_pos;
            while (space_start > 0 && (line[space_start - 1] == ' ' || line[space_start - 1] == '\t')) {
                space_start--;
            }

            if (space_start < semicol_pos) {
                TextEdit edit;
                edit.range = {{current_line, space_start}, {current_line, semicol_pos}};
                edit.new_text = "";  // حذف المسافات الزائدة
                edits.push_back(edit);
            }
        }

        // حذف المسافات الزائدة من نهاية السطر (بعد ;)
        if (!line.empty()) {
            int end = static_cast<int>(line.size());
            int clean_end = end;
            while (clean_end > 0 && (line[clean_end - 1] == ' ' || line[clean_end - 1] == '\t'))
                clean_end--;

            if (clean_end < end) {
                TextEdit edit;
                edit.range = {{current_line, clean_end}, {current_line, end}};
                edit.new_text = "";
                edits.push_back(edit);
            }
        }
    }

    return edits;
}

} // namespace lsp
} // namespace sad
