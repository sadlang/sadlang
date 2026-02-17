// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: formatting_provider.cpp
// الوصف: مزود التنسيق الثوري - يُنسِّق كود لغة ص بشكل جميل وموحّد
// ══════════════════════════════════════════════════════════════════════════════
//
// المميزات الثورية:
//   ✦ تنسيق المستند الكامل (Ctrl+Shift+F)
//   ✦ تنسيق نطاق محدد (تحديد → تنسيق) - تنفيذ حقيقي
//   ✦ مسافة بادئة ذكية (4 مسافات أو tab)
//   ✦ مسافات حول العوامل: = + - * / == != < > <= >=
//   ✦ مسافة بعد الفواصل وقبل الأقواس المتعرجة
//   ✦ إزالة المسافات الزائدة في نهاية الأسطر
//   ✦ تنظيم الأسطر الفارغة المتتالية (حد أقصى 1)
//   ✦ سطران فارغان بين تعريفات الدوال والأصناف
//   ✦ سطر جديد في نهاية الملف
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"
#include <algorithm>

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة للتنسيق
// ══════════════════════════════════════════════════════════════════════════════

static const std::string KW_FUNC   = "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9";
static const std::string KW_CLASS  = "\xd8\xb5\xd9\x86\xd9\x81";
static const std::string KW_ELSE   = "\xd9\x88\xd8\xa5\xd9\x84\xd8\xa7";
static const std::string KW_CATCH  = "\xd8\xa7\xd9\x85\xd8\xb3\xd9\x83";
static const std::string KW_FINALLY = "\xd8\xa3\xd8\xae\xd9\x8a\xd8\xb1\xd8\xa7\xd9\x8b";
static const std::string KW_END   = "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9";
static const std::string KW_DO    = "\xd8\xa3\xd8\xb9\xd9\x85\xd9\x84";

/// إزالة المسافات البادئة
static std::string trim_leading(const std::string& line) {
    size_t start = 0;
    while (start < line.size() && (line[start] == ' ' || line[start] == '\t'))
        start++;
    return line.substr(start);
}

/// إزالة المسافات الزائدة من نهاية السطر
static std::string trim_trailing(const std::string& line) {
    size_t end = line.size();
    while (end > 0 && (line[end-1] == ' ' || line[end-1] == '\t' || line[end-1] == '\r'))
        end--;
    return line.substr(0, end);
}

/// هل السطر يفتح كتلة جديدة؟
static bool opens_block(const std::string& trimmed) {
    if (trimmed.empty()) return false;
    if (trimmed.back() == '{') return true;
    if (trimmed.size() >= KW_DO.size() &&
        trimmed.substr(trimmed.size() - KW_DO.size()) == KW_DO)
        return true;
    return false;
}

/// هل السطر يغلق كتلة؟
static bool closes_block(const std::string& trimmed) {
    if (trimmed.empty()) return false;
    if (trimmed[0] == '}') return true;
    if (trimmed.find(KW_END) == 0) return true;
    if (trimmed.find(KW_ELSE) == 0) return true;
    if (trimmed.find(KW_CATCH) == 0) return true;
    if (trimmed.find(KW_FINALLY) == 0) return true;
    return false;
}

/// هل يغلق ويفتح (وإلا، امسك)؟
static bool closes_and_opens(const std::string& trimmed) {
    if (trimmed.find(KW_ELSE) == 0) return true;
    if (trimmed.find(KW_CATCH) == 0) return true;
    return false;
}

/// هل هو تعريف دالة أو صنف على مستوى عالٍ؟
static bool is_top_level_definition(const std::string& trimmed) {
    return trimmed.find(KW_FUNC) == 0 || trimmed.find(KW_CLASS) == 0;
}

/// إنشاء نص المسافة البادئة
static std::string make_indent(int level, const FormattingOptions& options) {
    if (options.insert_spaces) {
        return std::string(level * options.tab_size, ' ');
    }
    return std::string(level, '\t');
}

/// إضافة مسافات حول العوامل في سطر (مع تخطي النصوص والتعليقات)
static std::string format_operators(const std::string& line) {
    std::string result;
    result.reserve(line.size() + 20);

    bool in_string = false;
    char string_char = 0;
    bool in_comment = false;

    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];

        // التعامل مع التعليقات
        if (!in_string && c == '#') {
            in_comment = true;
            result += line.substr(i);
            break;
        }

        // التعامل مع النصوص
        if (!in_comment) {
            if (!in_string && (c == '"' || c == '\'')) {
                in_string = true;
                string_char = c;
                result += c;
                continue;
            }
            if (in_string && c == string_char && (i == 0 || line[i-1] != '\\')) {
                in_string = false;
                result += c;
                continue;
            }
        }

        if (in_string || in_comment) {
            result += c;
            continue;
        }

        // ──── العوامل المركبة (حرفان) ────
        char next = (i + 1 < line.size()) ? line[i + 1] : 0;

        if ((c == '=' && next == '=') || (c == '!' && next == '=') ||
            (c == '<' && next == '=') || (c == '>' && next == '=') ||
            (c == '+' && next == '=') || (c == '-' && next == '=') ||
            (c == '*' && next == '=') || (c == '/' && next == '=')) {
            // نضمن مسافة قبل وبعد
            if (!result.empty() && result.back() != ' ') result += ' ';
            result += c;
            result += next;
            i++;
            if (i + 1 < line.size() && line[i + 1] != ' ') result += ' ';
            continue;
        }

        // ──── سهم الإرجاع -> ────
        if (c == '-' && next == '>') {
            if (!result.empty() && result.back() != ' ') result += ' ';
            result += "->";
            i++;
            if (i + 1 < line.size() && line[i + 1] != ' ') result += ' ';
            continue;
        }

        // ──── العوامل الأحادية ────
        if (c == '=' && next != '=') {
            if (!result.empty() && result.back() != ' ') result += ' ';
            result += '=';
            if (i + 1 < line.size() && line[i + 1] != ' ' && line[i + 1] != '=')
                result += ' ';
            continue;
        }

        // ──── + - * / % مع مسافات ────
        if ((c == '+' || c == '*' || c == '/' || c == '%') &&
            next != '=' && c != '/' /* ليس تعليقاً */) {
            // نتأكد أنه ليس أحادياً
            bool is_binary = !result.empty() && result.back() != '(' &&
                              result.back() != ',' && result.back() != '=';
            if (is_binary) {
                if (!result.empty() && result.back() != ' ') result += ' ';
                result += c;
                if (i + 1 < line.size() && line[i + 1] != ' ') result += ' ';
                continue;
            }
        }

        // ──── فاصلة: مسافة بعدها ────
        if (c == ',' || (static_cast<unsigned char>(c) == 0xD8 && i + 1 < line.size() &&
            static_cast<unsigned char>(line[i+1]) == 0x8C)) {
            result += c;
            if (c == ',') {
                if (i + 1 < line.size() && line[i + 1] != ' ') result += ' ';
            }
            continue;
        }

        result += c;
    }

    return result;
}

// ══════════════════════════════════════════════════════════════════════════════
//  تنسيق أسطر مع حساب المسافة البادئة
// ══════════════════════════════════════════════════════════════════════════════

static std::string format_lines(const std::vector<std::string>& lines,
                                 int start, int end,
                                 int initial_indent,
                                 const FormattingOptions& options) {
    std::string formatted;
    int indent_level = initial_indent;
    bool prev_was_empty = false;
    bool prev_was_definition = false;

    for (int i = start; i <= end && i < static_cast<int>(lines.size()); i++) {
        std::string trimmed = trim_trailing(trim_leading(lines[i]));

        // ──── الأسطر الفارغة ────
        if (trimmed.empty()) {
            if (!prev_was_empty) {
                formatted += "\n";
                prev_was_empty = true;
            }
            prev_was_definition = false;
            continue;
        }
        prev_was_empty = false;

        // ──── سطر فارغ إضافي قبل تعريفات الدوال والأصناف ────
        if (is_top_level_definition(trimmed) && prev_was_definition && indent_level == 0) {
            // نضيف سطراً فارغاً إضافياً (فاصل بين تعريفات)
            if (!formatted.empty() && formatted.back() != '\n') formatted += "\n";
        }

        // ──── حساب المسافة البادئة ────
        if (closes_block(trimmed) && !closes_and_opens(trimmed)) {
            indent_level = std::max(0, indent_level - 1);
        }
        if (closes_and_opens(trimmed)) {
            indent_level = std::max(0, indent_level - 1);
        }

        // ──── تنسيق العوامل ────
        std::string formatted_line = format_operators(trimmed);

        // ──── بناء السطر المنسق ────
        formatted += make_indent(indent_level, options);
        formatted += formatted_line;
        formatted += "\n";

        prev_was_definition = is_top_level_definition(trimmed);

        // ──── فتح كتلة ────
        if (opens_block(trimmed) || closes_and_opens(trimmed)) {
            indent_level++;
        }
    }

    return formatted;
}

// ══════════════════════════════════════════════════════════════════════════════
//  تنسيق المستند الكامل
// ══════════════════════════════════════════════════════════════════════════════

std::vector<TextEdit> LspEngine::format_document(
    const DocumentUri& uri, const FormattingOptions& options)
{
    std::vector<TextEdit> edits;

    auto doc = doc_store_->get(uri);
    if (!doc) return edits;

    auto lines = arabic::split_lines(doc->content);
    if (lines.empty()) return edits;

    std::string formatted = format_lines(lines, 0, static_cast<int>(lines.size()) - 1, 0, options);

    // ──── إزالة الأسطر الفارغة الزائدة في النهاية ────
    if (options.trim_final_newlines) {
        while (formatted.size() > 1 && formatted.back() == '\n' &&
               formatted[formatted.size()-2] == '\n') {
            formatted.pop_back();
        }
    }

    // ──── التأكد من وجود سطر جديد في النهاية ────
    if (options.insert_final_newline && !formatted.empty() && formatted.back() != '\n') {
        formatted += '\n';
    }

    if (formatted != doc->content) {
        TextEdit edit;
        edit.range.start = {0, 0};
        edit.range.end = {static_cast<int>(lines.size()), 0};
        edit.new_text = formatted;
        edits.push_back(edit);
    }

    return edits;
}

// ══════════════════════════════════════════════════════════════════════════════
//  تنسيق نطاق محدد - تنفيذ حقيقي
// ══════════════════════════════════════════════════════════════════════════════

std::vector<TextEdit> LspEngine::format_range(
    const DocumentUri& uri, const Range& range, const FormattingOptions& options)
{
    std::vector<TextEdit> edits;

    auto doc = doc_store_->get(uri);
    if (!doc) return edits;

    auto lines = arabic::split_lines(doc->content);
    if (lines.empty()) return edits;

    int start_line = std::max(0, range.start.line);
    int end_line = std::min(static_cast<int>(lines.size()) - 1, range.end.line);

    // ──── حساب المسافة البادئة الأولية ────
    // نمسح الأسطر قبل النطاق لحساب indent_level
    int initial_indent = 0;
    for (int i = 0; i < start_line; i++) {
        std::string trimmed = trim_trailing(trim_leading(lines[i]));
        if (trimmed.empty()) continue;
        if (closes_block(trimmed) && !closes_and_opens(trimmed))
            initial_indent = std::max(0, initial_indent - 1);
        if (closes_and_opens(trimmed))
            initial_indent = std::max(0, initial_indent - 1);
        if (opens_block(trimmed) || closes_and_opens(trimmed))
            initial_indent++;
    }

    std::string formatted = format_lines(lines, start_line, end_line, initial_indent, options);

    // إزالة السطر الفارغ الأخير الزائد
    if (!formatted.empty() && formatted.back() == '\n') {
        formatted.pop_back();
    }

    // بناء النص الأصلي للمقارنة
    std::string original;
    for (int i = start_line; i <= end_line; i++) {
        if (i > start_line) original += "\n";
        original += lines[i];
    }

    if (formatted != original) {
        TextEdit edit;
        edit.range.start = {start_line, 0};
        edit.range.end = {end_line, static_cast<int>(lines[end_line].size())};
        edit.new_text = formatted;
        edits.push_back(edit);
    }

    return edits;
}

} // namespace lsp
} // namespace sad
