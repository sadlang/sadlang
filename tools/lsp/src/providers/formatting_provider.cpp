// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: formatting_provider.cpp
// الوصف: مزود التنسيق — يستخدم SadFormatter الشامل لتنسيق كود لغة ص
// ══════════════════════════════════════════════════════════════════════════════
//
// (AR) يوفر تنسيق المستند الكامل وتنسيق نطاق محدد عبر LSP، مع دعم:
//   ✦ تنسيق كامل للمستند (Ctrl+Shift+F)
//   ✦ تنسيق نطاق محدد (تحديد → تنسيق)
//   ✦ تحويل خيارات LSP إلى خيارات SadFormatter
//   ✦ تحميل ملفات الإعدادات (.تنسيق، .sad-fmt) من مجلد المشروع
//   ✦ دعم 30+ قاعدة تنسيق عبر SadFormatter
//
// (EN) Provides document and range formatting via LSP using the real
//      SadFormatter engine with 30+ rules, config files, and profiles.
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"
#include "format/sad_formatter.h"
#include <algorithm>
#include <filesystem>

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  تحويل خيارات LSP إلى خيارات SadFormatter
// ══════════════════════════════════════════════════════════════════════════════

/// (AR) تحويل خيارات التنسيق من بروتوكول LSP إلى خيارات المنسّق الشامل
/// (EN) Convert LSP FormattingOptions to SadFormatter options
static Sad::Format::FormatterOptions lsp_to_formatter_options(
    const FormattingOptions& lsp_opts)
{
    Sad::Format::FormatterOptions fmt_opts;
    fmt_opts.indentSize = lsp_opts.tab_size;
    fmt_opts.useSpaces = lsp_opts.insert_spaces;
    fmt_opts.ensureFinalNewline = lsp_opts.insert_final_newline;
    fmt_opts.trimTrailingWhitespace = lsp_opts.trim_final_newlines;
    return fmt_opts;
}

/// (AR) استخراج مسار المجلد من URI للمستند
/// (EN) Extract directory path from document URI for config loading
static std::string extract_directory_from_uri(const DocumentUri& uri) {
    std::string path = uri;
    // إزالة بادئة file:///
    if (path.rfind("file:///", 0) == 0) {
        path = path.substr(8);
    } else if (path.rfind("file://", 0) == 0) {
        path = path.substr(7);
    }
    // استبدال %20 بمسافات
    std::string decoded;
    for (size_t i = 0; i < path.size(); i++) {
        if (path[i] == '%' && i + 2 < path.size()) {
            int hex = 0;
            if (std::sscanf(path.substr(i + 1, 2).c_str(), "%x", &hex) == 1) {
                decoded += static_cast<char>(hex);
                i += 2;
                continue;
            }
        }
        decoded += path[i];
    }
    // الحصول على المجلد الأب
    // (AR) نستعمل u8path لتفسير السلسلة كـUTF-8 صراحةً؛ خلافه يحاول std::filesystem
    //      تحويلها عبر صفحة ترميز ANSI فيرمي "No mapping for Unicode" على المسارات
    //      العربية حين تكون صفحة الترميز النشطة ليست UTF-8.
    // (EN) Use u8path so the UTF-8 bytes are not run through the ANSI code page,
    //      which throws on Arabic paths when the active code page isn't UTF-8.
    std::filesystem::path fs_path = std::filesystem::u8path(decoded);
    if (fs_path.has_parent_path()) {
        return fs_path.parent_path().u8string();
    }
    return ".";
}

// ══════════════════════════════════════════════════════════════════════════════
//  تنسيق المستند الكامل — باستخدام SadFormatter
// ══════════════════════════════════════════════════════════════════════════════

std::vector<TextEdit> LspEngine::format_document(
    const DocumentUri& uri, const FormattingOptions& options)
{
    std::vector<TextEdit> edits;

    auto doc = doc_store_->get(uri);
    if (!doc) return edits;

    // تحويل خيارات LSP إلى خيارات المنسّق
    auto fmt_opts = lsp_to_formatter_options(options);

    // (AR) معالِج طلب LSP يجب ألّا يرمي أبدًا على حالة مسار/ترميز حدّية؛ أيّ فشل
    //      في تحميل الإعدادات أو التنسيق يتدهور بأمان إلى "لا تعديلات".
    // (EN) An LSP request handler must never throw on a path/encoding edge case;
    //      any config-load or format failure degrades gracefully to no edits.
    try {
        // إنشاء المنسّق وتحميل إعدادات المشروع إن وجدت
        Sad::Format::SadFormatter formatter(fmt_opts);
        std::string dir = extract_directory_from_uri(uri);
        formatter.loadConfigFromDirectory(dir);

        // تنسيق الكود
        auto result = formatter.format(doc->content);

        if (!result.success()) {
            // فشل التنسيق — نعيد قائمة فارغة
            return edits;
        }

        if (result.changed) {
            // إنشاء تعديل واحد يغطي المستند بالكامل
            auto lines = arabic::split_lines(doc->content);
            TextEdit edit;
            edit.range.start = {0, 0};
            edit.range.end = {static_cast<int>(lines.size()), 0};
            edit.new_text = result.output;
            edits.push_back(edit);
        }
    } catch (const std::exception&) {
        // تدهور بأمان: لا تعديلات بدل إسقاط الطلب
        return edits;
    }

    return edits;
}

// ══════════════════════════════════════════════════════════════════════════════
//  تنسيق نطاق محدد — باستخدام SadFormatter
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

    // ──── تنسيق المستند الكامل ثم استخراج النطاق المطلوب ────
    // هذا يضمن أن المسافة البادئة والسياق صحيحان
    auto fmt_opts = lsp_to_formatter_options(options);

    // (AR) كما في format_document: المعالِج لا يرمي على حالة مسار/ترميز حدّية.
    // (EN) As in format_document: the handler must not throw on path/encoding edges.
    std::string formatted_output;
    try {
        Sad::Format::SadFormatter formatter(fmt_opts);
        std::string dir = extract_directory_from_uri(uri);
        formatter.loadConfigFromDirectory(dir);

        auto result = formatter.format(doc->content);
        if (!result.success() || !result.changed) {
            return edits;
        }
        formatted_output = result.output;
    } catch (const std::exception&) {
        return edits; // تدهور بأمان
    }

    // تقسيم النتيجة المنسقة إلى أسطر
    auto formatted_lines = arabic::split_lines(formatted_output);

    // (AR) حارس مطابقة الأسطر: نستخرج الأسطر [start_line, end_line] من المخرَج
    //      المنسَّق بنفس الفهارس. هذا صحيح فقط إن لم يُغيّر التنسيق عددَ الأسطر
    //      الكلّيّ؛ إن أُدرج/حُذف سطرٌ فوق النطاق انزاحت الفهارس وأنتجنا تعديلًا
    //      فاسدًا. الحلّ الجذريّ: إن اختلف العدد الكلّيّ نتدهور إلى «لا تعديلات»
    //      (المحرّر يبقى متّسقًا) بدل قصّ شريحةٍ خاطئة.
    // (EN) Line-slice extraction by identical indices is only valid when the
    //      formatter preserves the total line count; otherwise indices shift and
    //      the slice is wrong. On a count change, degrade to no edits.
    if (formatted_lines.size() != lines.size())
        return edits;

    std::string original_range;
    for (int i = start_line; i <= end_line; i++) {
        if (i > start_line) original_range += "\n";
        original_range += lines[i];
    }

    // استخراج النطاق المنسق (إذا بقي عدد الأسطر متوافقاً)
    std::string formatted_range;
    int fmt_end = std::min(end_line, static_cast<int>(formatted_lines.size()) - 1);
    for (int i = start_line; i <= fmt_end && i < static_cast<int>(formatted_lines.size()); i++) {
        if (i > start_line) formatted_range += "\n";
        formatted_range += formatted_lines[i];
    }

    if (formatted_range != original_range) {
        TextEdit edit;
        edit.range.start = {start_line, 0};
        edit.range.end = {end_line, static_cast<int>(lines[end_line].size())};
        edit.new_text = formatted_range;
        edits.push_back(edit);
    }

    return edits;
}

} // namespace lsp
} // namespace sad
