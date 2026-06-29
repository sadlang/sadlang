// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: signature_help_provider.cpp
// الوصف: مزود مساعدة التوقيع الثوري - يعرض معلمات الدالة أثناء الكتابة
// ══════════════════════════════════════════════════════════════════════════════
//
// المميزات الثورية:
//   ✦ عرض توقيع الدالة مع تمييز المعلمة النشطة
//   ✦ دعم التوقيعات المتعددة (overloads)
//   ✦ توثيق markdown لكل معلمة
//   ✦ دعم الاستدعاءات المتداخلة: دالة١(دالة٢(|))
//   ✦ دعم الفاصلة العربية (،) والإنجليزية (,)
//   ✦ عرض نوع الإرجاع
//   ✦ عرض معلومات غير متزامن / مولّد
//
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"
#include "sad_type_system.h" // (CW-06) أسماء الأنواع من مصدر الحقيقة (sadTypeKindArabicName)

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  أسماء أنواع لا تُعرض في توقيع الإرجاع — من مصدر الحقيقة لا مهرَّدة
//  (AR) «فراغ» (Void) = لا قيمة، و«غير_محدد» = استنتاج عاجز؛ كلاهما يُخفى من
//        سهم الإرجاع. نشتقّ «فراغ» من sadTypeKindArabicName(Void) (CW-19/CW-10)
//        كي لا ينحرف الحارس بعد توحيد الأسماء (كان «عدم» المهرَّد فلم يعد يطابق).
//  (EN) Names hidden from the return arrow, sourced from the SoT (no magic).
// ══════════════════════════════════════════════════════════════════════════════
static const std::string SIG_RET_VOID =
    Sad::Types::sadTypeKindArabicName(Sad::Types::SadTypeKind::Void); // فراغ
static const std::string SIG_RET_UNRESOLVED =
    "\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf"; // غير_محدد

SignatureHelp LspEngine::signature_help(const DocumentUri& uri, const Position& pos) {
    SignatureHelp help;

    auto doc = doc_store_->get(uri);
    if (!doc) return help;

    std::string line = doc_store_->get_line(uri, pos.line);
    if (line.empty()) return help;

    // ──── تحليل سياق الاستدعاء (يدعم التداخل) ────
    // نبني مكدس من سياقات الاستدعاء
    struct CallContext {
        std::string func_name;
        int param_index;        // فهرس المعلمة النشطة
        int open_paren_col;     // موضع قوس الفتح
        bool is_constructor = false; // هل هو استدعاء باني (جديد صنف())
    };
    std::vector<CallContext> call_stack;

    int col = std::min(pos.character, static_cast<int>(line.size()));
    int paren_depth = 0;
    int current_param = 0;

    // نمسح من الموضع الحالي للخلف
    for (int i = col - 1; i >= 0; i--) {
        if (i >= static_cast<int>(line.size())) continue;
        char c = line[i];

        if (c == ')') {
            paren_depth++;
        }
        else if (c == '(') {
            if (paren_depth == 0) {
                // وجدنا قوس الفتح - نقرأ اسم الدالة
                int func_end = i;
                int func_start = func_end - 1;
                // نتخطى المسافات
                while (func_start >= 0 && (line[func_start] == ' ' || line[func_start] == '\t'))
                    func_start--;
                // نقرأ الاسم
                int name_end = func_start + 1;
                while (func_start >= 0 && line[func_start] != ' ' &&
                       line[func_start] != '\t' && line[func_start] != '(' &&
                       line[func_start] != ',' && line[func_start] != '=' &&
                       line[func_start] != '.') {
                    func_start--;
                }
                func_start++;

                if (func_start < name_end) {
                    CallContext ctx;
                    ctx.func_name = line.substr(func_start, name_end - func_start);
                    ctx.param_index = current_param;
                    ctx.open_paren_col = i;

                    // ──── دعم الباني: جديد صنف(|) ────
                    // إذا كان قبل الاسم الكلمة "جديد"، نحفظ اسم الصنف
                    // جديد = \xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf
                    static const std::string kw_new = "\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf";
                    if (func_start >= static_cast<int>(kw_new.size()) + 1) {
                        int before = func_start - 1;
                        while (before >= 0 && (line[before] == ' ' || line[before] == '\t'))
                            before--;
                        int kw_end = before + 1;
                        int kw_start = kw_end - static_cast<int>(kw_new.size());
                        if (kw_start >= 0 && line.substr(kw_start, kw_new.size()) == kw_new) {
                            ctx.is_constructor = true;
                        }
                    }

                    call_stack.push_back(ctx);
                }
                // ننتقل للمستوى الأعلى
                current_param = 0;
                continue;
            }
            paren_depth--;
        }
        else if (paren_depth == 0) {
            // الفاصلة العربية أو الإنجليزية
            if (c == ',') {
                current_param++;
            }
            // الفاصلة العربية (، = 0xD8 0x8C)
            else if (i + 1 < static_cast<int>(line.size()) &&
                     static_cast<unsigned char>(c) == 0xD8 &&
                     static_cast<unsigned char>(line[i+1]) == 0x8C) {
                current_param++;
            }
        }
    }

    if (call_stack.empty()) return help;

    // ──── بناء التوقيعات لأقرب سياق استدعاء ────
    const auto& ctx = call_stack.front(); // أعمق سياق (أقرب للمؤشر)

    // ──── البحث عن تعريف الدالة أو الباني ────
    std::optional<AnalyzedSymbol> def_opt;

    if (ctx.is_constructor) {
        // جديد صنف(...) — نبحث عن الباني في الصنف
        auto class_def = index_->find_definition(ctx.func_name, uri, pos);
        if (class_def && class_def->kind == AnalyzedSymbolKind::Class) {
            // نبحث عن باني في أعضاء الصنف
            auto symbols = index_->get_document_symbols(class_def->uri);
            for (const auto& sym : symbols) {
                if (sym.kind == AnalyzedSymbolKind::Constructor &&
                    sym.scope_owner == ctx.func_name) {
                    def_opt = sym;
                    break;
                }
            }
            if (!def_opt) {
                // لا باني صريح — نعرض معلومات الصنف
                def_opt = class_def;
            }
        }
    } else {
        def_opt = index_->find_definition(ctx.func_name, uri, pos);
    }

    if (!def_opt || !def_opt->func_info) return help;
    const auto& def = *def_opt;

    SignatureInformation sig;

    // ──── بناء التوقيع بتنسيق غني ────
    std::string label;

    // مؤشرات: غير_متزامن / مولّد
    if (def.func_info->is_async) {
        label += "\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86 "; // غير_متزامن
    }
    if (def.func_info->is_generator) {
        label += "\xd9\x85\xd9\x88\xd9\x84\xd9\x91\xd8\xaf "; // مولّد
    }

    label += def.name + "(";

    bool first = true;
    for (const auto& [pname, ptype] : def.func_info->parameters) {
        if (!first) label += ", ";
        first = false;

        std::string param_label = pname;
        if (!ptype.name.empty() && ptype.name != "\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf") {
            param_label += ": " + ptype.name;
        }

        // حفظ نطاق المعلمة في التسمية (لتمييزها)
        ParameterInformation pi;
        pi.label = param_label;

        // توثيق المعلمة
        std::string param_doc = "**" + pname + "**";
        if (!ptype.name.empty() && ptype.name != "\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf") {
            param_doc += " (`" + ptype.name + "`)";
        }
        pi.documentation = param_doc;
        sig.parameters.push_back(pi);

        label += param_label;
    }
    label += ")";

    // نوع الإرجاع
    if (!def.func_info->return_type.name.empty() &&
        def.func_info->return_type.name != SIG_RET_VOID &&       // فراغ (Void: لا قيمة)
        def.func_info->return_type.name != SIG_RET_UNRESOLVED) { // غير_محدد
        label += " \xe2\x86\x90 " + def.func_info->return_type.name; // ←
    }

    sig.label = label;
    sig.active_parameter = ctx.param_index;

    // توثيق الدالة
    if (!def.documentation.empty()) {
        sig.documentation = def.documentation;
    } else {
        // توثيق تلقائي
        std::string auto_doc;
        if (def.is_builtin) {
            auto_doc = "\xf0\x9f\x94\xa7 **دالة مدمجة**\n\n";
        }
        auto_doc += "`" + def.name + "` - ";
        auto_doc += std::to_string(def.func_info->parameters.size());
        auto_doc += " \xd9\x85\xd8\xb9\xd9\x84\xd9\x85\xd8\xa9"; // معلمة
        sig.documentation = auto_doc;
    }

    help.signatures.push_back(sig);
    help.active_signature = 0;
    help.active_parameter = ctx.param_index;

    return help;
}

} // namespace lsp
} // namespace sad
