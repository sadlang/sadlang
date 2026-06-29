// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: code_actions_provider.cpp
// الوصف: مزود إجراءات الكود الثوري - إصلاحات وإعادة هيكلة ذكية
// ══════════════════════════════════════════════════════════════════════════════
//
// إجراءات الكود (Code Actions) تظهر كأيقونة المصباح 💡 في المحرر
//
// الإجراءات المدعومة:
//   ✦ إصلاح سريع: استبدال "print" بـ "اطبع"
//   ✦ إصلاح سريع: استبدال الكلمات الإنجليزية بالعربية
//   ✦ إصلاح سريع: إزالة المسافات الزائدة في آخر الأسطر
//   ✦ إصلاح سريع: حذف المتغير غير المستخدم
//   ✦ إصلاح سريع: تحويل متغير إلى ثابت
//   ✦ إعادة هيكلة: استخراج التحديد إلى متغير
//   ✦ إعادة هيكلة: استخراج التحديد إلى دالة
//   ✦ إعادة هيكلة: تغليف بـ حاول/امسك
//   ✦ توليد: منشئ (constructor) لصنف
//   ✦ إضافة توثيق لدالة أو صنف
//   ✦ ترتيب الاستيرادات تلقائياً
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"
#include <algorithm>
#include <map>

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  ثوابت الكلمات المفتاحية (UTF-8)
// ══════════════════════════════════════════════════════════════════════════════
static const std::string KW_PRINT  = "\xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9"; // اطبع
static const std::string KW_FUNC   = "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9"; // دالة
static const std::string KW_CLASS  = "\xd8\xb5\xd9\x86\xd9\x81";         // صنف
static const std::string KW_IMPORT = "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x88\xd8\xb1\xd8\xaf"; // استورد
static const std::string KW_VAR    = "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1"; // متغير
static const std::string KW_CONST  = "\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa"; // ثابت
static const std::string KW_TRY    = "\xd8\xad\xd8\xa7\xd9\x88\xd9\x84"; // حاول
static const std::string KW_CATCH  = "\xd8\xa7\xd9\x85\xd8\xb3\xd9\x83"; // امسك
static const std::string KW_CONSTR = "\xd9\x85\xd9\x86\xd8\xb4\xd8\xa6"; // منشئ

// خريطة الكلمات الإنجليزية → العربية (للإصلاح السريع)
static const std::map<std::string, std::string>& get_english_to_arabic() {
    static std::map<std::string, std::string> m = {
        {"function", "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9"},
        {"class",    "\xd8\xb5\xd9\x86\xd9\x81"},
        {"if",       "\xd8\xa5\xd8\xb0\xd8\xa7"},
        {"else",     "\xd9\x88\xd8\xa5\xd9\x84\xd8\xa7"},
        {"while",    "\xd8\xa8\xd9\x8a\xd9\x86\xd9\x85\xd8\xa7"},
        {"for",      "\xd9\x84\xd9\x83\xd9\x84"},
        {"return",   "\xd8\xa5\xd8\xb1\xd8\xac\xd8\xa7\xd8\xb9"},
        {"var",      "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1"},
        {"const",    "\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa"},
        {"try",      "\xd8\xad\xd8\xa7\xd9\x88\xd9\x84"},
        {"catch",    "\xd8\xa7\xd9\x85\xd8\xb3\xd9\x83"},
        {"import",   "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x88\xd8\xb1\xd8\xaf"},
        {"true",     "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad"},
        {"false",    "\xd8\xae\xd8\xb7\xd8\xa3"},
        {"null",     "\xd8\xb9\xd8\xaf\xd9\x85"},
        {"break",    "\xd8\xa7\xd8\xae\xd8\xb1\xd8\xac"},
        {"continue", "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x85\xd8\xb1"},
        {"print",    "\xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9"},
    };
    return m;
}

// ══════════════════════════════════════════════════════════════════════════════
//  مساعدات داخلية
// ══════════════════════════════════════════════════════════════════════════════

/// إيجاد المسافة البادئة لسطر
static std::string get_indent(const std::string& line) {
    std::string indent;
    for (char c : line) {
        if (c == ' ' || c == '\t') indent += c;
        else break;
    }
    return indent;
}

/// استخراج اسم بعد كلمة مفتاحية
static std::string extract_name_after(const std::string& line, const std::string& kw) {
    auto kw_pos = line.find(kw);
    if (kw_pos == std::string::npos) return "";
    auto name_start = kw_pos + kw.size();
    while (name_start < line.size() && (line[name_start] == ' ' || line[name_start] == '\t'))
        name_start++;
    auto name_end = name_start;
    while (name_end < line.size() && line[name_end] != '(' && line[name_end] != ' ' &&
           line[name_end] != '{' && line[name_end] != ':')
        name_end++;
    return line.substr(name_start, name_end - name_start);
}

// ══════════════════════════════════════════════════════════════════════════════
//  تنفيذ إجراءات الكود
// ══════════════════════════════════════════════════════════════════════════════

std::vector<CodeAction> LspEngine::code_actions(
    const DocumentUri& uri,
    const Range& range,
    const std::vector<Diagnostic>& diagnostics)
{
    std::vector<CodeAction> actions;

    auto doc = doc_store_->get(uri);
    if (!doc) return actions;

    auto all_lines = arabic::split_lines(doc->content);

    // ╔══════════════════════════════════════════════════════════════╗
    // ║  ١. إصلاحات سريعة بناءً على التشخيصات                     ║
    // ╚══════════════════════════════════════════════════════════════╝
    for (const auto& diag : diagnostics) {

        // ──── إصلاح تلقائي: إدراج رمز مفقود (من نظام التعافي) ────
        // تشخيصات التعافي تحمل كوداً بصيغة "ص-تعافي-INSERT:نص"
        if (diag.code.rfind("\xd8\xb5-\xd8\xaa\xd8\xb9\xd8\xa7\xd9\x81\xd9\x8a-INSERT:", 0) == 0) {
            // استخراج النص المُدرج من الكود (بعد "ص-تعافي-INSERT:")
            std::string prefix = "\xd8\xb5-\xd8\xaa\xd8\xb9\xd8\xa7\xd9\x81\xd9\x8a-INSERT:";
            std::string inserted_text = diag.code.substr(prefix.size());

            if (!inserted_text.empty()) {
                CodeAction action;
                action.title = "\xf0\x9f\x94\xa7 \xd8\xa5\xd8\xaf\xd8\xb1\xd8\xa7\xd8\xac '" + inserted_text + "' \xd8\xa7\xd9\x84\xd9\x85\xd9\x81\xd9\x82\xd9\x88\xd8\xaf\xd8\xa9"; // 🔧 إدراج 'X' المفقودة
                action.kind = CodeActionKind::QuickFix;
                action.is_preferred = true;
                action.diagnostics = {diag};

                WorkspaceEdit edit;
                TextEdit te;
                // إدراج الرمز في موضع التشخيص
                te.range.start = diag.range.start;
                te.range.end = diag.range.start; // نقطة إدراج (بدون حذف)
                te.new_text = inserted_text;
                edit.changes[uri] = {te};
                action.edit = edit;
                actions.push_back(action);
            }
        }

        // ──── إصلاح تلقائي عام من نظام التعافي ────
        if (diag.code == "\xd8\xb5-\xd8\xaa\xd8\xb9\xd8\xa7\xd9\x81\xd9\x8a-\xd9\xa0\xd9\xa0\xd9\xa1") { // ص-تعافي-٠٠١
            CodeAction action;
            action.title = "\xf0\x9f\x94\x8d " + diag.message; // 🔍 + رسالة التعافي
            action.kind = CodeActionKind::QuickFix;
            action.diagnostics = {diag};
            // لا يوجد edit تلقائي — فقط إشعار
            actions.push_back(action);
        }

        // ──── إصلاح: استبدال print بـ اطبع ────
        if (diag.code == "\xd8\xb5-\xd8\xaa\xd9\xa0\xd9\xa0\xd9\xa2" || // ص-ت٠٠٢
            (diag.message.find("print") != std::string::npos &&
             diag.message.find("اطبع") != std::string::npos)) {

            CodeAction action;
            action.title = "\xf0\x9f\x94\xa7 استبدل 'print' بـ '" + KW_PRINT + "'";
            action.kind = CodeActionKind::QuickFix;
            action.is_preferred = true;
            action.diagnostics = {diag};

            std::string line = doc_store_->get_line(uri, diag.range.start.line);
            auto pos = line.find("print");
            if (pos != std::string::npos) {
                WorkspaceEdit edit;
                TextEdit te;
                te.range.start = {diag.range.start.line, static_cast<int>(pos)};
                te.range.end = {diag.range.start.line, static_cast<int>(pos + 5)};
                te.new_text = KW_PRINT;
                edit.changes[uri] = {te};
                action.edit = edit;
            }
            actions.push_back(action);
        }

        // ──── إصلاح: استبدال كلمة إنجليزية بالعربية ────
        std::string diag_code_eng = "\xd8\xb5-\xd8\xaa\xd9\xa0\xd9\xa0\xd9\xa5"; // ص-ت٠٠٥
        if (diag.code == diag_code_eng) {
            // التشخيص يحتوي على الكلمة الإنجليزية في النطاق
            std::string line = doc_store_->get_line(uri, diag.range.start.line);
            // نستخرج الكلمة الإنجليزية من line
            for (const auto& [eng, arb] : get_english_to_arabic()) {
                auto eng_pos = line.find(eng);
                if (eng_pos != std::string::npos) {
                    CodeAction action;
                    action.title = "\xf0\x9f\x94\xa7 استبدل '" + eng + "' بـ '" + arb + "'";
                    action.kind = CodeActionKind::QuickFix;
                    action.is_preferred = true;
                    action.diagnostics = {diag};

                    WorkspaceEdit edit;
                    TextEdit te;
                    te.range.start = {diag.range.start.line, static_cast<int>(eng_pos)};
                    te.range.end = {diag.range.start.line, static_cast<int>(eng_pos + eng.size())};
                    te.new_text = arb;
                    edit.changes[uri] = {te};
                    action.edit = edit;
                    actions.push_back(action);
                }
            }
        }

        // ──── إصلاح: حذف متغير غير مستخدم ────
        std::string diag_code_unused = "\xd8\xb5-\xd8\xaa\xd9\xa0\xd9\xa0\xd9\xa1"; // ص-ت٠٠١
        if (diag.code == diag_code_unused) {
            // نحذف السطر كاملاً
            CodeAction action;
            action.title = "\xf0\x9f\x97\x91\xef\xb8\x8f حذف المتغير غير المستخدم";
            action.kind = CodeActionKind::QuickFix;
            action.diagnostics = {diag};

            int ln = diag.range.start.line;
            WorkspaceEdit edit;
            TextEdit te;
            te.range.start = {ln, 0};
            te.range.end = {ln + 1, 0}; // حذف السطر كاملاً
            te.new_text = "";
            edit.changes[uri] = {te};
            action.edit = edit;
            actions.push_back(action);

            // أو: إضافة تعليق تجاهل
            CodeAction ignore_action;
            ignore_action.title = "\xf0\x9f\x94\x87 إضافة تعليق تجاهل #nocheck";
            ignore_action.kind = CodeActionKind::QuickFix;
            ignore_action.diagnostics = {diag};

            WorkspaceEdit ignore_edit;
            TextEdit ignore_te;
            std::string orig_line = (ln < static_cast<int>(all_lines.size())) ? all_lines[ln] : "";
            ignore_te.range.start = {ln, static_cast<int>(orig_line.size())};
            ignore_te.range.end = {ln, static_cast<int>(orig_line.size())};
            ignore_te.new_text = "  # nocheck";
            ignore_edit.changes[uri] = {ignore_te};
            ignore_action.edit = ignore_edit;
            actions.push_back(ignore_action);
        }

        // ──── إصلاح: إزالة المسافات الزائدة ────
        std::string diag_code_trailing = "\xd8\xb5-\xd8\xaa\xd9\xa0\xd9\xa0\xd9\xa9"; // ص-ت٠٠٩
        if (diag.code == diag_code_trailing) {
            CodeAction action;
            action.title = "\xf0\x9f\xa7\xb9 إزالة المسافات الزائدة في آخر السطر";
            action.kind = CodeActionKind::QuickFix;
            action.is_preferred = true;
            action.diagnostics = {diag};

            int ln = diag.range.start.line;
            std::string line = (ln < static_cast<int>(all_lines.size())) ? all_lines[ln] : "";
            // إزالة المسافات من النهاية
            std::string trimmed = line;
            while (!trimmed.empty() && (trimmed.back() == ' ' || trimmed.back() == '\t'))
                trimmed.pop_back();

            WorkspaceEdit edit;
            TextEdit te;
            te.range.start = {ln, 0};
            te.range.end = {ln, static_cast<int>(line.size())};
            te.new_text = trimmed;
            edit.changes[uri] = {te};
            action.edit = edit;
            actions.push_back(action);
        }
    }

    // ╔══════════════════════════════════════════════════════════════╗
    // ║  ٢. إصلاح جماعي: تنظيف جميع المسافات الزائدة             ║
    // ╚══════════════════════════════════════════════════════════════╝
    {
        bool has_trailing = false;
        for (const auto& line : all_lines) {
            if (!line.empty() && (line.back() == ' ' || line.back() == '\t')) {
                has_trailing = true;
                break;
            }
        }
        if (has_trailing) {
            CodeAction action;
            action.title = "\xf0\x9f\xa7\xb9 إزالة جميع المسافات الزائدة في الملف";
            action.kind = CodeActionKind::SourceFixAll;

            WorkspaceEdit edit;
            for (int i = 0; i < static_cast<int>(all_lines.size()); i++) {
                const auto& line = all_lines[i];
                if (!line.empty() && (line.back() == ' ' || line.back() == '\t')) {
                    std::string trimmed = line;
                    while (!trimmed.empty() && (trimmed.back() == ' ' || trimmed.back() == '\t'))
                        trimmed.pop_back();
                    TextEdit te;
                    te.range.start = {i, 0};
                    te.range.end = {i, static_cast<int>(line.size())};
                    te.new_text = trimmed;
                    edit.changes[uri].push_back(te);
                }
            }
            action.edit = edit;
            actions.push_back(action);
        }
    }

    // ╔══════════════════════════════════════════════════════════════╗
    // ║  ٣. إجراءات سياقية على الأسطر المحددة                     ║
    // ╚══════════════════════════════════════════════════════════════╝
    for (int line_num = range.start.line; line_num <= range.end.line; line_num++) {
        if (line_num >= static_cast<int>(all_lines.size())) break;
        std::string line = all_lines[line_num];
        if (line.empty()) continue;

        // ──── إضافة توثيق لدالة ────
        if (line.find(KW_FUNC) != std::string::npos) {
            std::string prev_line = (line_num > 0) ? all_lines[line_num - 1] : "";
            if (prev_line.find("##") == std::string::npos &&
                prev_line.find("///") == std::string::npos) {

                std::string func_name = extract_name_after(line, KW_FUNC);
                std::string indent = get_indent(line);

                // نبحث عن المعاملات
                auto paren_start = line.find('(');
                auto paren_end = line.find(')');
                std::vector<std::string> params;
                if (paren_start != std::string::npos && paren_end != std::string::npos && paren_end > paren_start + 1) {
                    std::string params_str = line.substr(paren_start + 1, paren_end - paren_start - 1);
                    // تقسيم بالفاصلة
                    size_t s = 0;
                    for (size_t p = 0; p <= params_str.size(); p++) {
                        if (p == params_str.size() || params_str[p] == ',') {
                            std::string param = params_str.substr(s, p - s);
                            // إزالة المسافات
                            while (!param.empty() && param.front() == ' ') param.erase(0, 1);
                            while (!param.empty() && param.back() == ' ') param.pop_back();
                            // إزالة النوع إذا كان موجوداً (بعد :)
                            auto colon = param.find(':');
                            if (colon != std::string::npos) param = param.substr(0, colon);
                            while (!param.empty() && param.back() == ' ') param.pop_back();
                            if (!param.empty()) params.push_back(param);
                            s = p + 1;
                        }
                    }
                }

                CodeAction action;
                action.title = "\xf0\x9f\x93\x9d إضافة توثيق لدالة '" + func_name + "'";
                action.kind = CodeActionKind::Refactor;

                std::string doc_comment = indent + "## " + func_name + "\n";
                doc_comment += indent + "## الوصف: [أضف وصفاً هنا]\n";
                for (const auto& p : params) {
                    doc_comment += indent + "## @" + p + " - [وصف المعلمة]\n";
                }
                doc_comment += indent + "## @إرجاع - [أضف نوع الإرجاع]\n";

                WorkspaceEdit edit;
                TextEdit te;
                te.range.start = {line_num, 0};
                te.range.end = {line_num, 0};
                te.new_text = doc_comment;
                edit.changes[uri] = {te};
                action.edit = edit;
                actions.push_back(action);
            }
        }

        // ──── إضافة توثيق لصنف ────
        if (line.find(KW_CLASS) != std::string::npos) {
            std::string prev_line = (line_num > 0) ? all_lines[line_num - 1] : "";
            if (prev_line.find("##") == std::string::npos) {
                std::string class_name = extract_name_after(line, KW_CLASS);
                std::string indent = get_indent(line);

                CodeAction action;
                action.title = "\xf0\x9f\x93\x9d إضافة توثيق لصنف '" + class_name + "'";
                action.kind = CodeActionKind::Refactor;

                std::string doc_comment = indent + "## صنف: " + class_name + "\n";
                doc_comment += indent + "## الوصف: [أضف وصفاً هنا]\n";

                WorkspaceEdit edit;
                TextEdit te;
                te.range.start = {line_num, 0};
                te.range.end = {line_num, 0};
                te.new_text = doc_comment;
                edit.changes[uri] = {te};
                action.edit = edit;
                actions.push_back(action);
            }
        }

        // ──── تحويل متغير إلى ثابت ────
        if (line.find(KW_VAR) != std::string::npos) {
            auto var_pos = line.find(KW_VAR);
            std::string var_name = extract_name_after(line, KW_VAR);

            CodeAction action;
            action.title = "\xf0\x9f\x94\x92 تحويل '" + var_name + "' إلى ثابت";
            action.kind = CodeActionKind::Refactor;

            WorkspaceEdit edit;
            TextEdit te;
            te.range.start = {line_num, static_cast<int>(var_pos)};
            te.range.end = {line_num, static_cast<int>(var_pos + KW_VAR.size())};
            te.new_text = KW_CONST;
            edit.changes[uri] = {te};
            action.edit = edit;
            actions.push_back(action);
        }
    }

    // ╔══════════════════════════════════════════════════════════════╗
    // ║  ٤. إعادة هيكلة: استخراج إلى متغير أو دالة               ║
    // ╚══════════════════════════════════════════════════════════════╝
    // إذا كان هناك تحديد (أكثر من سطر أو جزء من سطر)
    bool has_selection = (range.start.line != range.end.line ||
                          range.start.character != range.end.character);
    if (has_selection) {
        // استخراج إلى متغير
        {
            CodeAction action;
            action.title = "\xf0\x9f\x93\xa6 استخراج التحديد إلى متغير";
            action.kind = CodeActionKind::RefactorExtract;

            // نحصل على النص المحدد
            std::string selected;
            if (range.start.line == range.end.line) {
                std::string line = (range.start.line < static_cast<int>(all_lines.size()))
                    ? all_lines[range.start.line] : "";
                int s = std::min(range.start.character, static_cast<int>(line.size()));
                int e = std::min(range.end.character, static_cast<int>(line.size()));
                selected = line.substr(s, e - s);
            } else {
                for (int ln = range.start.line; ln <= range.end.line && ln < static_cast<int>(all_lines.size()); ln++) {
                    if (!selected.empty()) selected += "\n";
                    selected += all_lines[ln];
                }
            }

            if (!selected.empty() && range.start.line == range.end.line) {
                std::string indent = get_indent(all_lines[range.start.line]);

                WorkspaceEdit edit;
                // ١. إضافة سطر التعريف قبل السطر الحالي
                TextEdit te_def;
                te_def.range.start = {range.start.line, 0};
                te_def.range.end = {range.start.line, 0};
                te_def.new_text = indent + KW_VAR + " \xd9\x86\xd8\xaa\xd9\x8a\xd8\xac\xd8\xa9 = " + selected + "\n";
                edit.changes[uri].push_back(te_def);

                // ٢. استبدال التحديد بالمتغير الجديد
                TextEdit te_rep;
                te_rep.range = range;
                te_rep.new_text = "\xd9\x86\xd8\xaa\xd9\x8a\xd8\xac\xd8\xa9"; // نتيجة
                edit.changes[uri].push_back(te_rep);

                action.edit = edit;
                actions.push_back(action);
            }
        }

        // استخراج إلى دالة (تحديد متعدد الأسطر)
        if (range.start.line != range.end.line) {
            CodeAction action;
            action.title = "\xf0\x9f\x94\xa7 استخراج التحديد إلى دالة";
            action.kind = CodeActionKind::RefactorExtract;

            std::string selected_lines;
            for (int ln = range.start.line; ln <= range.end.line && ln < static_cast<int>(all_lines.size()); ln++) {
                selected_lines += "\t" + all_lines[ln] + "\n";
            }

            WorkspaceEdit edit;
            // ١. إضافة الدالة الجديدة قبل الموضع الحالي
            TextEdit te_func;
            te_func.range.start = {range.start.line, 0};
            te_func.range.end = {range.start.line, 0};
            te_func.new_text = KW_FUNC + " \xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9_\xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xb1\xd8\xac\xd8\xa9() {\n" + selected_lines + "}\n\n";
            edit.changes[uri].push_back(te_func);

            // ٢. استبدال الأسطر المحددة باستدعاء الدالة
            TextEdit te_call;
            te_call.range = range;
            std::string indent = get_indent(all_lines[range.start.line]);
            te_call.new_text = indent + "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9_\xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xb1\xd8\xac\xd8\xa9()";
            edit.changes[uri].push_back(te_call);

            action.edit = edit;
            actions.push_back(action);
        }

        // تغليف بـ حاول/امسك
        {
            CodeAction action;
            action.title = "\xf0\x9f\x9b\xa1\xef\xb8\x8f تغليف بـ حاول / امسك";
            action.kind = CodeActionKind::Refactor;

            std::string indent = (range.start.line < static_cast<int>(all_lines.size()))
                ? get_indent(all_lines[range.start.line]) : "";

            // نجمع الأسطر المحددة
            std::string wrapped;
            for (int ln = range.start.line; ln <= range.end.line && ln < static_cast<int>(all_lines.size()); ln++) {
                wrapped += indent + "\t" + all_lines[ln] + "\n";
            }

            std::string replacement = indent + KW_TRY + " {\n" + wrapped + indent + "} " +
                KW_CATCH + " (\xd8\xae\xd8\xb7\xd8\xa3) {\n" +
                indent + "\t" + KW_PRINT + "(\xd8\xae\xd8\xb7\xd8\xa3)\n" +
                indent + "}";

            WorkspaceEdit edit;
            TextEdit te;
            te.range = range;
            te.new_text = replacement;
            edit.changes[uri] = {te};
            action.edit = edit;
            actions.push_back(action);
        }
    }

    // ╔══════════════════════════════════════════════════════════════╗
    // ║  ٥. توليد منشئ لصنف                                       ║
    // ╚══════════════════════════════════════════════════════════════╝
    for (int line_num = range.start.line; line_num <= range.end.line; line_num++) {
        if (line_num >= static_cast<int>(all_lines.size())) break;
        const auto& line = all_lines[line_num];
        if (line.find(KW_CLASS) != std::string::npos) {
            // نبحث عن خصائص الصنف
            std::string class_name = extract_name_after(line, KW_CLASS);
            std::vector<std::string> properties;

            int brace_depth = 0;
            bool in_class = false;
            for (int i = line_num; i < static_cast<int>(all_lines.size()); i++) {
                const auto& cl = all_lines[i];
                for (char c : cl) {
                    if (c == '{') { brace_depth++; in_class = true; }
                    if (c == '}') brace_depth--;
                }
                if (in_class && brace_depth == 0) break;
                // نبحث عن تعريفات خصائص (متغير أو ثابت في جسم الصنف)
                if (in_class && brace_depth == 1) {
                    if (cl.find(KW_VAR) != std::string::npos || cl.find(KW_CONST) != std::string::npos) {
                        std::string prop = (cl.find(KW_VAR) != std::string::npos)
                            ? extract_name_after(cl, KW_VAR) : extract_name_after(cl, KW_CONST);
                        if (!prop.empty()) properties.push_back(prop);
                    }
                }
            }

            // نتحقق أنه لا يوجد منشئ بالفعل
            bool has_constructor = false;
            brace_depth = 0; in_class = false;
            for (int i = line_num; i < static_cast<int>(all_lines.size()); i++) {
                const auto& cl = all_lines[i];
                for (char c : cl) {
                    if (c == '{') { brace_depth++; in_class = true; }
                    if (c == '}') brace_depth--;
                }
                if (in_class && brace_depth == 0) break;
                if (cl.find(KW_CONSTR) != std::string::npos) { has_constructor = true; break; }
            }

            if (!has_constructor && !properties.empty()) {
                CodeAction action;
                action.title = "\xf0\x9f\x8f\x97\xef\xb8\x8f توليد منشئ لصنف '" + class_name + "'";
                action.kind = CodeActionKind::Refactor;

                // بناء نص المنشئ
                std::string params;
                std::string assignments;
                for (const auto& prop : properties) {
                    if (!params.empty()) params += ", ";
                    params += prop;
                    assignments += "\t\t\xd9\x87\xd8\xb0\xd8\xa7." + prop + " = " + prop + "\n"; // هذا.x = x
                }

                // موضع الإدراج: بعد سطر فتح القوس {
                int insert_line = line_num + 1;
                for (int i = line_num; i < static_cast<int>(all_lines.size()); i++) {
                    if (all_lines[i].find('{') != std::string::npos) {
                        insert_line = i + 1;
                        break;
                    }
                }

                WorkspaceEdit edit;
                TextEdit te;
                te.range.start = {insert_line, 0};
                te.range.end = {insert_line, 0};
                te.new_text = "\t" + KW_CONSTR + "(" + params + ") {\n" + assignments + "\t}\n\n";
                edit.changes[uri] = {te};
                action.edit = edit;
                actions.push_back(action);
            }
        }
    }

    // ╔══════════════════════════════════════════════════════════════╗
    // ║  ٦. ترتيب الاستيرادات (Source Action)                      ║
    // ╚══════════════════════════════════════════════════════════════╝
    {
        std::vector<int> import_lines;
        for (int i = 0; i < static_cast<int>(all_lines.size()); i++) {
            if (all_lines[i].find(KW_IMPORT) != std::string::npos) {
                import_lines.push_back(i);
            }
        }
        if (import_lines.size() > 1) {
            CodeAction action;
            action.title = "\xf0\x9f\x93\x8b ترتيب جمل الاستيراد أبجدياً";
            action.kind = CodeActionKind::SourceOrganizeImports;

            std::vector<std::string> imports;
            for (int line_idx : import_lines) {
                imports.push_back(all_lines[line_idx]);
            }
            std::sort(imports.begin(), imports.end());

            WorkspaceEdit edit;
            for (size_t idx = 0; idx < import_lines.size(); idx++) {
                TextEdit te;
                int ln = import_lines[idx];
                te.range.start = {ln, 0};
                te.range.end = {ln, static_cast<int>(all_lines[ln].size())};
                te.new_text = imports[idx];
                edit.changes[uri].push_back(te);
            }
            action.edit = edit;
            actions.push_back(action);
        }
    }

    return actions;
}

} // namespace lsp
} // namespace sad
