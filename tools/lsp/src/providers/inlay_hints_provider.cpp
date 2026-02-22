// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: inlay_hints_provider.cpp
// الوصف: مزود التلميحات المضمنة الثوري - معلومات غنية داخل الكود
// ══════════════════════════════════════════════════════════════════════════════
//
// التلميحات المضمنة (Inlay Hints) تعرض معلومات إضافية بشكل شفاف:
//
//   ✦ تلميحات الأنواع: متغير حساب = 100  →  متغير حساب: رقم = 100
//   ✦ تلميحات المعلمات: مسافة(10, 20)  →  مسافة(س: 10, ص: 20)
//   ✦ تلميحات نوع الإرجاع: دالة مجموع(أ, ب) → ... -> رقم
//   ✦ تلميحات عدد العناصر للمصفوفات والقواميس
//   ✦ الفاصلة العربية ، مدعومة
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
static const std::string KW_VAR    = "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1"; // متغير
static const std::string KW_CONST  = "\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa";         // ثابت
static const std::string KW_FUNC   = "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9";         // دالة
static const std::string KW_IF     = "\xd8\xa5\xd8\xb0\xd8\xa7";                  // إذا
static const std::string KW_WHILE  = "\xd8\xa8\xd9\x8a\xd9\x86\xd9\x85\xd8\xa7"; // بينما
static const std::string KW_FOR    = "\xd9\x84\xd9\x83\xd9\x84";                  // لكل
static const std::string KW_RETURN = "\xd8\xa3\xd8\xb1\xd8\xac\xd8\xb9";         // أرجع
static const std::string KW_PRINT  = "\xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9";         // اطبع
static const std::string KW_CLASS  = "\xd8\xb5\xd9\x86\xd9\x81";                  // صنف

/// هل الاسم كلمة مفتاحية لا نضيف لها تلميح معلمات؟
static bool is_control_keyword(const std::string& name) {
    return name == KW_IF || name == KW_WHILE || name == KW_FOR ||
           name == KW_RETURN || name == KW_FUNC || name == KW_VAR ||
           name == KW_CONST || name == KW_PRINT || name == KW_CLASS;
}

// ══════════════════════════════════════════════════════════════════════════════
//  تنفيذ التلميحات المضمنة
// ══════════════════════════════════════════════════════════════════════════════

std::vector<InlayHint> LspEngine::inlay_hints(
    const DocumentUri& uri, const Range& range)
{
    std::vector<InlayHint> hints;

    auto doc = doc_store_->get(uri);
    if (!doc) return hints;

    auto all_symbols = index_->get_document_symbols(uri);

    // ──── ١. تلميحات الأنواع للمتغيرات ونوع الإرجاع ────
    for (const auto& sym : all_symbols) {
        if (sym.name_range.start.line < range.start.line ||
            sym.name_range.start.line > range.end.line) {
            continue;
        }

        // تلميح النوع: يظهر بعد اسم المتغير/الثابت
        if ((sym.kind == AnalyzedSymbolKind::Variable ||
             sym.kind == AnalyzedSymbolKind::Constant ||
             sym.kind == AnalyzedSymbolKind::Property) &&
            !sym.type.name.empty() &&
            sym.type.name != "\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf" && // "غير_محدد"
            sym.type.is_inferred) {

            InlayHint hint;
            hint.position = sym.name_range.end;
            hint.label = ": " + sym.type.name;
            hint.kind = InlayHintKind::Type;
            hint.padding_left = false;
            hint.padding_right = true;
            hints.push_back(hint);
        }

        // تلميح نوع الإرجاع: يظهر بعد قائمة معلمات الدالة
        if ((sym.kind == AnalyzedSymbolKind::Function ||
             sym.kind == AnalyzedSymbolKind::Method) &&
            sym.func_info &&
            !sym.func_info->return_type.name.empty() &&
            sym.func_info->return_type.name != "\xd8\xb9\xd8\xaf\xd9\x85" &&
            sym.func_info->return_type.name != "\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf") {

            std::string line = doc_store_->get_line(uri, sym.definition_range.start.line);
            auto paren_pos = line.rfind(')');
            if (paren_pos != std::string::npos) {
                auto after_paren = line.substr(paren_pos + 1);
                if (after_paren.find("->") == std::string::npos &&
                    after_paren.find("\xe2\x86\x90") == std::string::npos) {
                    InlayHint hint;
                    hint.position = {
                        sym.definition_range.start.line,
                        arabic::utf8_offset_to_utf16_column(line, static_cast<int>(paren_pos + 1))
                    };
                    hint.label = " \xe2\x86\x90 " + sym.func_info->return_type.name;
                    hint.kind = InlayHintKind::Type;
                    hint.padding_left = true;
                    hint.padding_right = false;
                    hints.push_back(hint);
                }
            }
        }
    }

    // ──── ٢. تلميحات المعلمات عند استدعاء الدوال ────
    auto lines = arabic::split_lines(doc->content);
    for (int i = range.start.line; i <= range.end.line && i < static_cast<int>(lines.size()); i++) {
        const auto& line = lines[i];

        size_t pos = 0;
        while (pos < line.size()) {
            auto paren = line.find('(', pos);
            if (paren == std::string::npos || paren == 0) break;

            // قراءة اسم الدالة قبل القوس
            int func_end = static_cast<int>(paren) - 1;
            while (func_end >= 0 && (line[func_end] == ' ' || line[func_end] == '\t'))
                func_end--;
            if (func_end < 0) { pos = paren + 1; continue; }

            int func_start = func_end;
            while (func_start > 0) {
                uint32_t cp;
                int prev = func_start - 1;
                while (prev > 0 && (static_cast<unsigned char>(line[prev]) & 0xC0) == 0x80)
                    prev--;
                arabic::utf8_decode(line.c_str() + prev, cp);
                if (!arabic::is_identifier_char(cp)) break;
                func_start = prev;
            }

            std::string func_name = line.substr(func_start, func_end - func_start + 1);

            // تخطي الكلمات المفتاحية
            if (is_control_keyword(func_name)) {
                pos = paren + 1;
                continue;
            }

            // البحث في الفهرس عن معلومات الدالة
            Position search_pos{i, arabic::utf8_offset_to_utf16_column(line, func_start)};
            auto def = index_->find_definition(func_name, uri, search_pos);
            if (def && def->func_info && !def->func_info->parameters.empty()) {
                // ──── حساب مواضع كل معلمة ────
                std::vector<size_t> arg_starts;
                int depth = 0;
                arg_starts.push_back(paren + 1);

                for (size_t j = paren + 1; j < line.size(); j++) {
                    char c = line[j];
                    if (c == '(' || c == '[' || c == '{') depth++;
                    else if (c == ')' || c == ']' || c == '}') {
                        if (depth == 0) break;
                        depth--;
                    }
                    else if (depth == 0) {
                        // فاصلة عربية أو إنجليزية
                        bool is_comma = (c == ',');
                        if (!is_comma && static_cast<unsigned char>(c) == 0xD8 &&
                            j + 1 < line.size() &&
                            static_cast<unsigned char>(line[j+1]) == 0x8C) {
                            is_comma = true;
                        }
                        if (is_comma) {
                            // تخطي بايتات الفاصلة
                            size_t next = j + 1;
                            if (c != ',') next = j + 2; // الفاصلة العربية = 2 بايت
                            arg_starts.push_back(next);
                        }
                    }
                }

                // ──── إضافة تلميح لكل معلمة ────
                for (size_t a = 0; a < arg_starts.size() &&
                     a < def->func_info->parameters.size(); a++) {
                    size_t hint_pos = arg_starts[a];
                    // تخطي المسافات
                    while (hint_pos < line.size() &&
                           (line[hint_pos] == ' ' || line[hint_pos] == '\t'))
                        hint_pos++;
                    if (hint_pos >= line.size() || line[hint_pos] == ')') continue;

                    InlayHint hint;
                    hint.position = {i, arabic::utf8_offset_to_utf16_column(line, static_cast<int>(hint_pos))};
                    hint.label = def->func_info->parameters[a].first + ": ";
                    hint.kind = InlayHintKind::Parameter;
                    hint.padding_left = false;
                    hint.padding_right = false;
                    hints.push_back(hint);
                }
            }

            pos = paren + 1;
        }
    }

    return hints;
}

} // namespace lsp
} // namespace sad
