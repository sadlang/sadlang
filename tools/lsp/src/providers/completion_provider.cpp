// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: completion_provider.cpp
// الوصف: مزود الإكمال التلقائي الثوري - اقتراحات ذكية سياقية
// ══════════════════════════════════════════════════════════════════════════════
//
// المميزات الثورية:
//   ✦ إكمال تلقائي ذكي يعتمد على الفهرس الحقيقي
//   ✦ إكمال سياقي: يفهم ماذا يكتب المبرمج ويقترح وفقاً لذلك
//   ✦ دعم الكلمات المفتاحية العربية (دالة، إذا، بينما، إلخ)
//   ✦ إكمال أعضاء الأصناف بعد النقطة
//   ✦ إكمال داخل جسم الصنف (منشئ، طريقة)
//   ✦ إكمال بعد الكلمات المفتاحية (بعد إذا → شروط، بعد لكل → متغيرات)
//   ✦ إكمال postfix: .اطبع → اطبع(التعبير)
//   ✦ بحث ضبابي يتجاهل التشكيل وأشكال الألف
//   ✦ قصاصات كود (snippets) غنية ومفيدة
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"
#include "lexer_keywords.h"
#include <algorithm>

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  الكلمات المفتاحية مع قصاصات الكود
// ══════════════════════════════════════════════════════════════════════════════

struct KeywordSnippet {
    std::string keyword;        // الكلمة المفتاحية
    std::string label;          // العنوان المعروض
    std::string detail;         // شرح مختصر
    std::string insert_text;    // النص المدرج
    bool is_snippet;            // هل هي قصاصة (snippet)؟
    CompletionItemKind kind;    // نوع عنصر الإكمال
};

static const std::vector<KeywordSnippet>& get_keyword_snippets() {
    static std::vector<KeywordSnippet> snippets = {
        // ──── تصريحات ────
        {
            "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9", // دالة
            "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 (تعريف دالة)",
            "تعريف دالة جديدة",
            "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 ${1:\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85}(${2}) {\n\t${0}\n}",
            true, CompletionItemKind::Keyword
        },
        {
            "\xd8\xb5\xd9\x86\xd9\x81", // صنف
            "\xd8\xb5\xd9\x86\xd9\x81 (تعريف صنف)",
            "تعريف صنف جديد",
            "\xd8\xb5\xd9\x86\xd9\x81 ${1:\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85} {\n\t${0}\n}",
            true, CompletionItemKind::Keyword
        },
        // ──── بنى التحكم ────
        {
            "\xd8\xa5\xd8\xb0\xd8\xa7", // إذا
            "\xd8\xa5\xd8\xb0\xd8\xa7 (جملة شرطية)",
            "جملة شرطية",
            "\xd8\xa5\xd8\xb0\xd8\xa7 (${1:\xd8\xa7\xd9\x84\xd8\xb4\xd8\xb1\xd8\xb7}) {\n\t${0}\n}",
            true, CompletionItemKind::Keyword
        },
        {
            "\xd9\x88\xd8\xa5\xd9\x84\xd8\xa7", // وإلا
            "\xd9\x88\xd8\xa5\xd9\x84\xd8\xa7",
            "فرع وإلا",
            "\xd9\x88\xd8\xa5\xd9\x84\xd8\xa7 {\n\t${0}\n}",
            true, CompletionItemKind::Keyword
        },
        {
            "\xd8\xa8\xd9\x8a\xd9\x86\xd9\x85\xd8\xa7", // بينما
            "\xd8\xa8\xd9\x8a\xd9\x86\xd9\x85\xd8\xa7 (حلقة بينما)",
            "حلقة بينما شرط",
            "\xd8\xa8\xd9\x8a\xd9\x86\xd9\x85\xd8\xa7 (${1:\xd8\xa7\xd9\x84\xd8\xb4\xd8\xb1\xd8\xb7}) {\n\t${0}\n}",
            true, CompletionItemKind::Keyword
        },
        {
            "\xd9\x84\xd9\x83\xd9\x84", // لكل
            "\xd9\x84\xd9\x83\xd9\x84 (حلقة لكل)",
            "حلقة لكل عنصر في مجموعة",
            "\xd9\x84\xd9\x83\xd9\x84 ${1:\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1} \xd9\x81\xd9\x8a ${2:\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9} {\n\t${0}\n}",
            true, CompletionItemKind::Keyword
        },
        // ──── كلمات مفتاحية بسيطة ────
        {"\xd8\xa5\xd8\xb1\xd8\xac\xd8\xa7\xd8\xb9", "\xd8\xa5\xd8\xb1\xd8\xac\xd8\xa7\xd8\xb9", "إرجاع قيمة من دالة",
         "\xd8\xa5\xd8\xb1\xd8\xac\xd8\xa7\xd8\xb9", false, CompletionItemKind::Keyword},
        {"\xd8\xa7\xd8\xae\xd8\xb1\xd8\xac", "\xd8\xa7\xd8\xae\xd8\xb1\xd8\xac", "الخروج من حلقة",
         "\xd8\xa7\xd8\xae\xd8\xb1\xd8\xac", false, CompletionItemKind::Keyword},
        {"\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x85\xd8\xb1", "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x85\xd8\xb1", "الانتقال للدورة التالية",
         "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x85\xd8\xb1", false, CompletionItemKind::Keyword},
        {"\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1", "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1", "تعريف متغير",
         "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1 ${1:\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85} = ${0}", true, CompletionItemKind::Keyword},
        {"\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa", "\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa", "تعريف ثابت",
         "\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa ${1:\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85} = ${0}", true, CompletionItemKind::Keyword},
        // ──── معالجة الأخطاء ────
        {
            "\xd8\xad\xd8\xa7\xd9\x88\xd9\x84", // حاول
            "\xd8\xad\xd8\xa7\xd9\x88\xd9\x84 / \xd8\xa7\xd9\x85\xd8\xb3\xd9\x83 (معالجة أخطاء)",
            "كتلة معالجة الأخطاء",
            "\xd8\xad\xd8\xa7\xd9\x88\xd9\x84 {\n\t${1}\n} \xd8\xa7\xd9\x85\xd8\xb3\xd9\x83 (${2:\xd8\xae\xd8\xb7\xd8\xa3}) {\n\t${0}\n}",
            true, CompletionItemKind::Keyword
        },
        // ──── الاستيرادات ────
        {
            "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x88\xd8\xb1\xd8\xaf", // استورد
            "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x88\xd8\xb1\xd8\xaf (استيراد وحدة)",
            "استيراد وحدة أو مكتبة",
            "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x88\xd8\xb1\xd8\xaf \"${1}\"",
            true, CompletionItemKind::Keyword
        },
        // ──── تطابق الأنماط ────
        {
            "\xd8\xaa\xd8\xb7\xd8\xa7\xd8\xa8\xd9\x82", // تطابق
            "\xd8\xaa\xd8\xb7\xd8\xa7\xd8\xa8\xd9\x82 (مطابقة أنماط)",
            "مطابقة الأنماط (switch/match)",
            "\xd8\xaa\xd8\xb7\xd8\xa7\xd8\xa8\xd9\x82 (${1:\xd8\xa7\xd9\x84\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9}) {\n\t\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9 ${2} {\n\t\t${0}\n\t}\n}",
            true, CompletionItemKind::Keyword
        },
        // ──── غير متزامن ────
        {
            "\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86", // غير_متزامن
            "\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86 (دالة غير متزامنة)",
            "تعريف دالة غير متزامنة",
            "\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86 \xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 ${1:\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85}(${2}) {\n\t${0}\n}",
            true, CompletionItemKind::Keyword
        },
        // ──── أنواع البيانات ────
        {"\xd8\xb1\xd9\x82\xd9\x85", "\xd8\xb1\xd9\x82\xd9\x85 (نوع)", "نوع عدد صحيح", "\xd8\xb1\xd9\x82\xd9\x85", false, CompletionItemKind::TypeParameter},
        {"\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a", "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a (نوع)", "نوع عدد عشري", "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a", false, CompletionItemKind::TypeParameter},
        {"\xd9\x86\xd8\xb5", "\xd9\x86\xd8\xb5 (نوع)", "نوع نص", "\xd9\x86\xd8\xb5", false, CompletionItemKind::TypeParameter},
        {"\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a", "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a (نوع)", "نوع منطقي", "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a", false, CompletionItemKind::TypeParameter},
        {"\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9", "\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9 (نوع)", "نوع مصفوفة", "\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9", false, CompletionItemKind::TypeParameter},
        {"\xd9\x82\xd8\xa7\xd9\x85\xd9\x88\xd8\xb3", "\xd9\x82\xd8\xa7\xd9\x85\xd9\x88\xd8\xb3 (نوع)", "نوع قاموس (خريطة)", "\xd9\x82\xd8\xa7\xd9\x85\xd9\x88\xd8\xb3", false, CompletionItemKind::TypeParameter},
        // ──── القيم ────
        {"\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "القيمة المنطقية صحيح (true)", "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", false, CompletionItemKind::Constant},
        {"\xd8\xae\xd8\xb7\xd8\xa3", "\xd8\xae\xd8\xb7\xd8\xa3", "القيمة المنطقية خطأ (false)", "\xd8\xae\xd8\xb7\xd8\xa3", false, CompletionItemKind::Constant},
        {"\xd8\xb9\xd8\xaf\xd9\x85", "\xd8\xb9\xd8\xaf\xd9\x85", "القيمة الفارغة (null)", "\xd8\xb9\xd8\xaf\xd9\x85", false, CompletionItemKind::Constant},
        // ──── وصول ────
        {"\xd8\xb9\xd8\xa7\xd9\x85", "\xd8\xb9\xd8\xa7\xd9\x85", "وصول عام (public)", "\xd8\xb9\xd8\xa7\xd9\x85", false, CompletionItemKind::Keyword},
        {"\xd8\xae\xd8\xa7\xd8\xb5", "\xd8\xae\xd8\xa7\xd8\xb5", "وصول خاص (private)", "\xd8\xae\xd8\xa7\xd8\xb5", false, CompletionItemKind::Keyword},
        {"\xd9\x85\xd8\xad\xd9\x85\xd9\x8a", "\xd9\x85\xd8\xad\xd9\x85\xd9\x8a", "وصول محمي (protected)", "\xd9\x85\xd8\xad\xd9\x85\xd9\x8a", false, CompletionItemKind::Keyword},
    };
    return snippets;
}

// ══════════════════════════════════════════════════════════════════════════════
//  قصاصات سياقية - تظهر فقط في سياقات معينة
// ══════════════════════════════════════════════════════════════════════════════

/// قصاصات داخل جسم الصنف
static std::vector<CompletionItem> get_class_body_completions() {
    std::vector<CompletionItem> items;

    // منشئ
    {
        CompletionItem item;
        item.label = "\xd9\x85\xd9\x86\xd8\xb4\xd8\xa6 (constructor)"; // منشئ
        item.kind = CompletionItemKind::Constructor;
        item.detail = "تعريف منشئ الصنف";
        item.insert_text = "\xd9\x85\xd9\x86\xd8\xb4\xd8\xa6(${1}) {\n\t${0}\n}";
        item.insert_text_format = InsertTextFormat::Snippet;
        item.sort_text = "0_منشئ";
        items.push_back(item);
    }
    // طريقة
    {
        CompletionItem item;
        item.label = "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 (طريقة)"; // دالة
        item.kind = CompletionItemKind::Method;
        item.detail = "تعريف طريقة جديدة في الصنف";
        item.insert_text = "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 ${1:\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85}(${2}) {\n\t${0}\n}";
        item.insert_text_format = InsertTextFormat::Snippet;
        item.sort_text = "0_دالة";
        items.push_back(item);
    }
    return items;
}

/// فحص: هل نحن داخل جسم صنف؟
static bool is_inside_class_body(const std::string& content, int current_line) {
    auto lines = arabic::split_lines(content);
    std::string kw_class = "\xd8\xb5\xd9\x86\xd9\x81"; // صنف

    int brace_depth = 0;
    for (int i = current_line; i >= 0; i--) {
        const auto& line = lines[i];
        for (auto it = line.rbegin(); it != line.rend(); ++it) {
            if (*it == '}') brace_depth++;
            else if (*it == '{') {
                brace_depth--;
                if (brace_depth < 0) {
                    // نحن داخل كتلة مفتوحة - نفحص إذا كان صنف
                    if (line.find(kw_class) != std::string::npos) return true;
                    return false;
                }
            }
        }
    }
    return false;
}

// ══════════════════════════════════════════════════════════════════════════════
//  تنفيذ الإكمال التلقائي الثوري في LspEngine
// ══════════════════════════════════════════════════════════════════════════════

CompletionList LspEngine::completion(const DocumentUri& uri, const Position& pos) {
    CompletionList list;
    list.is_incomplete = false;

    auto doc = doc_store_->get(uri);
    if (!doc) return list;

    // الحصول على الكلمة الحالية (ما يكتبه المستخدم)
    std::string current_word = doc_store_->get_word_at(uri, pos);
    std::string current_line = doc_store_->get_line(uri, pos.line);

    // ──── ١. التحقق من سياق النقطة (إكمال أعضاء) ────
    bool is_member_access = false;
    std::string object_name;
    if (!current_line.empty()) {
        // نبحث عن نقطة قبل الموضع الحالي
        int col = pos.character;
        int dot_pos = -1;
        for (int i = col - 1; i >= 0; i--) {
            if (i < static_cast<int>(current_line.size()) && current_line[i] == '.') {
                dot_pos = i;
                break;
            }
        }
        if (dot_pos >= 0) {
            is_member_access = true;
            // قراءة اسم الكائن قبل النقطة
            int obj_end = dot_pos;
            int obj_start = obj_end - 1;
            while (obj_start >= 0 && current_line[obj_start] != ' ' &&
                   current_line[obj_start] != '\t' && current_line[obj_start] != '(' &&
                   current_line[obj_start] != ',') {
                obj_start--;
            }
            obj_start++;
            if (obj_start < obj_end) {
                object_name = current_line.substr(obj_start, obj_end - obj_start);
            }
        }
    }

    // ──── ٢. إكمال أعضاء الصنف ────
    if (is_member_access && !object_name.empty()) {
        // نبحث عن نوع الكائن في الفهرس
        auto def = index_->find_definition(object_name, uri, pos);
        if (def && def->class_info) {
            // نبحث عن أعضاء الصنف
            auto class_symbols = index_->get_document_symbols(uri);
            for (const auto& sym : class_symbols) {
                if (sym.scope_owner == def->name) {
                    CompletionItem item;
                    item.label = sym.name;
                    item.detail = sym.type.name;
                    item.documentation = {"markdown", sym.documentation};
                    item.insert_text = sym.name;

                    if (sym.kind == AnalyzedSymbolKind::Method) {
                        item.kind = CompletionItemKind::Method;
                        if (sym.func_info) {
                            item.insert_text = sym.name + "(${1})";
                            item.insert_text_format = InsertTextFormat::Snippet;
                        }
                    } else {
                        item.kind = CompletionItemKind::Property;
                    }

                    list.items.push_back(item);
                }
            }
            return list; // لا نضيف شيئاً آخر في سياق الأعضاء
        }

        // Postfix completions: عند كتابة شيء.اطبع → اطبع(شيء)
        std::string prefix_after_dot = current_word;
        if (!prefix_after_dot.empty()) {
            // اقتراح postfix: اطبع
            std::string kw_print = "\xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9"; // اطبع
            if (arabic::fuzzy_match_arabic(arabic::normalize_arabic(prefix_after_dot),
                                            arabic::normalize_arabic(kw_print))) {
                CompletionItem item;
                item.label = ".اطبع → اطبع(" + object_name + ")";
                item.kind = CompletionItemKind::Snippet;
                item.detail = "تحويل إلى استدعاء اطبع";
                // نحتاج استبدال object.اطبع بـ اطبع(object)
                TextEdit te;
                te.range.start = {pos.line, static_cast<int>(current_line.find(object_name))};
                te.range.end = {pos.line, pos.character};
                te.new_text = kw_print + "(" + object_name + ")";
                item.text_edit = te;
                item.sort_text = "0_postfix";
                list.items.push_back(item);
            }
        }

        return list;
    }

    // ──── ٣. إكمال سياقي: داخل جسم صنف ────
    if (is_inside_class_body(doc->content, pos.line)) {
        auto class_items = get_class_body_completions();
        for (auto& item : class_items) {
            if (current_word.empty() ||
                arabic::fuzzy_match_arabic(
                    arabic::normalize_arabic(current_word),
                    arabic::normalize_arabic(item.label))) {
                list.items.push_back(item);
            }
        }
    }

    // ──── ٤. إكمال الكلمات المفتاحية ────
    std::string normalized_word = arabic::normalize_arabic(current_word);
    for (const auto& snippet : get_keyword_snippets()) {
        std::string norm_kw = arabic::normalize_arabic(snippet.keyword);
        if (current_word.empty() ||
            arabic::fuzzy_match_arabic(normalized_word, norm_kw)) {
            CompletionItem item;
            item.label = snippet.label;
            item.kind = snippet.kind;
            item.detail = snippet.detail;
            item.insert_text = snippet.insert_text;
            item.insert_text_format = snippet.is_snippet ? InsertTextFormat::Snippet : InsertTextFormat::PlainText;
            item.sort_text = "0_" + snippet.keyword; // أولوية عالية
            list.items.push_back(item);
        }
    }

    // ──── ٥. إكمال من الفهرس (دوال، متغيرات، أصناف) ────
    if (!current_word.empty()) {
        auto matches = index_->fuzzy_search(current_word, uri, 30);
        for (const auto& sym : matches) {
            CompletionItem item;
            item.label = sym.name;

            switch (sym.kind) {
                case AnalyzedSymbolKind::Function:
                    item.kind = CompletionItemKind::Function;
                    if (sym.func_info) {
                        std::string params;
                        int idx = 1;
                        for (const auto& [pname, ptype] : sym.func_info->parameters) {
                            if (!params.empty()) params += ", ";
                            params += "${" + std::to_string(idx++) + ":" + pname + "}";
                        }
                        item.insert_text = sym.name + "(" + params + ")";
                        item.insert_text_format = InsertTextFormat::Snippet;
                        item.detail = sym.type.name;
                    }
                    break;
                case AnalyzedSymbolKind::Variable:
                    item.kind = CompletionItemKind::Variable;
                    item.detail = sym.type.name;
                    break;
                case AnalyzedSymbolKind::Constant:
                    item.kind = CompletionItemKind::Constant;
                    item.detail = sym.type.name;
                    break;
                case AnalyzedSymbolKind::Class:
                    item.kind = CompletionItemKind::Class;
                    break;
                case AnalyzedSymbolKind::Method:
                    item.kind = CompletionItemKind::Method;
                    break;
                case AnalyzedSymbolKind::Property:
                    item.kind = CompletionItemKind::Property;
                    break;
                case AnalyzedSymbolKind::Enum:
                    item.kind = CompletionItemKind::Enum;
                    break;
                case AnalyzedSymbolKind::Module:
                    item.kind = CompletionItemKind::Module;
                    break;
                default:
                    item.kind = CompletionItemKind::Text;
                    break;
            }

            // وصف markdown غني
            if (!sym.documentation.empty()) {
                item.documentation = {"markdown", sym.documentation};
            } else if (sym.func_info) {
                // بناء توثيق تلقائي
                std::string auto_doc = "```sad\n";
                auto_doc += "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 " + sym.name + "(";
                bool first = true;
                for (const auto& [pname, ptype] : sym.func_info->parameters) {
                    if (!first) auto_doc += ", ";
                    first = false;
                    auto_doc += pname;
                    if (!ptype.name.empty() && ptype.name != "\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf") {
                        auto_doc += ": " + ptype.name;
                    }
                }
                auto_doc += ")\n```";
                item.documentation = {"markdown", auto_doc};
            }

            if (sym.is_builtin) {
                item.sort_text = "1_" + sym.name; // المدمجات بعد الكلمات المفتاحية
            } else {
                item.sort_text = "2_" + sym.name;
            }

            list.items.push_back(item);
        }
    }

    return list;
}

} // namespace lsp
} // namespace sad
