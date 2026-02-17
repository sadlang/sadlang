// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: code_lens_provider.cpp
// الوصف: مزود عدسات الكود الثوري - عرض معلومات تفاعلية فوق الكود
// ══════════════════════════════════════════════════════════════════════════════
//
// عدسات الكود (Code Lens) هي نصوص صغيرة تظهر فوق الدوال والأصناف
// والمتغيرات العامة، تعرض معلومات مفيدة مثل:
//
//   ✦ عدد المراجع: "٣ مراجع" فوق كل دالة/صنف/متغير عام
//   ✦ عدد الأعضاء: "٥ أعضاء" فوق كل صنف
//   ✦ إشارة غير متزامن: "⚡ غير متزامن" فوق الدوال غير المتزامنة
//   ✦ عدد المعلمات: "← ٣ معلمات" فوق كل دالة
//   ✦ إشارة التصدير: "📤 مصدّر" فوق العناصر المصدّرة
//   ✦ تحذير عدم الاستخدام: "⚠ غير مستخدم" للدوال التي لا تُستدعى
//
// عند النقر على العدسة، يتم تنفيذ أمر مثل "عرض جميع المراجع"
//
// ═══════════════════════════════════════════════════════════════════════════
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  تحويل عدد إلى نص عربي مختصر
// ══════════════════════════════════════════════════════════════════════════════
//
// نحوّل الأعداد لصيغة عربية مقروءة:
//   0 → "٠"   1 → "١"   5 → "٥"   12 → "١٢"
//
static std::string to_arabic_numeral(int n) {
    if (n == 0) return "\xd9\xa0"; // ٠
    std::string result;
    std::string digits = std::to_string(n);
    for (char d : digits) {
        // الأرقام العربية تبدأ من U+0660 (٠)
        // في UTF-8: U+0660 = 0xD9 0xA0, U+0661 = 0xD9 0xA1, ...
        result += static_cast<char>(0xD9);
        result += static_cast<char>(0xA0 + (d - '0'));
    }
    return result;
}

// ══════════════════════════════════════════════════════════════════════════════
//  تنفيذ عدسات الكود
// ══════════════════════════════════════════════════════════════════════════════
//
// نمر على كل رمز في المستند (دوال، أصناف، ثوابت عامة)
// ونبني عدسة كود لكل واحد تعرض معلومات مفيدة
//

std::vector<CodeLens> LspEngine::code_lens(const DocumentUri& uri) {
    std::vector<CodeLens> lenses;

    auto doc = doc_store_->get(uri);
    if (!doc) return lenses;

    // ──── الحصول على رموز المستند ────
    auto symbols = index_->get_document_symbols(uri);

    for (const auto& sym : symbols) {
        // نعالج فقط الدوال والأصناف والثوابت العامة
        // لا نعرض عدسات للمتغيرات المحلية (تشوش على المبرمج)
        if (sym.kind != AnalyzedSymbolKind::Function &&
            sym.kind != AnalyzedSymbolKind::Method &&
            sym.kind != AnalyzedSymbolKind::Class &&
            sym.kind != AnalyzedSymbolKind::Constant &&
            sym.kind != AnalyzedSymbolKind::Enum) {
            continue;
        }

        // لا نعرض عدسات للدوال المدمجة (لأنها ليست في كود المستخدم)
        if (sym.is_builtin) continue;

        // ──── حساب عدد المراجع ────
        auto refs = index_->find_references(sym.name, uri);

        // نستثني التعريف نفسه من عدد المراجع
        // (المبرمج يريد معرفة عدد الاستخدامات، وليس عدد التعريفات)
        int ref_count = 0;
        for (const auto& ref : refs) {
            if (!ref.is_declaration) ref_count++;
        }

        // ──── بناء نص العدسة ────
        std::string lens_text;

        // ① عدد المراجع
        if (ref_count > 0) {
            lens_text += to_arabic_numeral(ref_count);
            // مرجع / مراجع (مذكر في العربية)
            if (ref_count == 1) {
                lens_text += " \xd9\x85\xd8\xb1\xd8\xac\xd8\xb9"; // مرجع
            } else if (ref_count == 2) {
                lens_text += " \xd9\x85\xd8\xb1\xd8\xac\xd8\xb9\xd8\xa7\xd9\x86"; // مرجعان
            } else {
                lens_text += " \xd9\x85\xd8\xb1\xd8\xa7\xd8\xac\xd8\xb9"; // مراجع
            }
        } else {
            // تحذير: غير مستخدم! (فقط للدوال والأصناف والثوابت)
            lens_text += "\xe2\x9a\xa0 \xd8\xba\xd9\x8a\xd8\xb1 \xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85"; // ⚠ غير مستخدم
        }

        // ② معلمات الدالة
        if (sym.func_info) {
            int param_count = static_cast<int>(sym.func_info->parameters.size());
            if (param_count > 0) {
                lens_text += " | \xe2\x86\x90 " + to_arabic_numeral(param_count); // ←
                if (param_count <= 2) {
                    lens_text += " \xd9\x85\xd8\xb9\xd9\x84\xd9\x85\xd8\xa9"; // معلمة
                } else {
                    lens_text += " \xd9\x85\xd8\xb9\xd9\x84\xd9\x85\xd8\xa7\xd8\xaa"; // معلمات
                }
            }
        }

        // ③ غير متزامن
        if (sym.func_info && sym.func_info->is_async) {
            lens_text += " | \xe2\x9a\xa1 \xd8\xba\xd9\x8a\xd8\xb1 \xd9\x85\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86"; // ⚡ غير متزامن
        }

        // ④ أعضاء الصنف
        if (sym.class_info) {
            int member_count = static_cast<int>(sym.class_info->members.size());
            if (member_count > 0) {
                lens_text += " | " + to_arabic_numeral(member_count);
                lens_text += " \xd8\xb9\xd8\xb6\xd9\x88"; // عضو
            }
        }

        // ⑤ مصدّر
        if (sym.is_exported) {
            lens_text += " | \xf0\x9f\x93\xa4 \xd9\x85\xd8\xb5\xd8\xaf\xd9\x91\xd8\xb1"; // 📤 مصدّر
        }

        // ──── إنشاء عدسة الكود ────
        CodeLens lens;
        // نضع العدسة فوق سطر التعريف مباشرة
        lens.range = sym.definition_range;

        Command cmd;
        cmd.title = lens_text;
        // الأمر: عرض المراجع عند النقر على العدسة
        cmd.command = "editor.action.showReferences";
        // المعاملات: URI، الموضع (لعرض المراجع)
        cmd.arguments = {uri,
            std::to_string(sym.name_range.start.line),
            std::to_string(sym.name_range.start.character)};
        lens.command = cmd;

        lenses.push_back(lens);
    }

    return lenses;
}

} // namespace lsp
} // namespace sad
