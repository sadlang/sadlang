// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: call_hierarchy_provider.cpp
// الوصف: مزود شجرة الاستدعاءات - تسلسل هرمي للدوال المتصلة
// ══════════════════════════════════════════════════════════════════════════════
//
// شجرة الاستدعاءات (Call Hierarchy) تتيح للمبرمج:
//
//   ١. الاستدعاءات الواردة (Incoming Calls): من يستدعي هذه الدالة؟
//      مثال: دالة "احسب_المجموع" تُستدعى من:
//        ← دالة "معالجة_البيانات" في سطر ٤٥
//        ← دالة "التقرير_النهائي" في سطر ١٢٠
//
//   ٢. الاستدعاءات الصادرة (Outgoing Calls): ماذا تستدعي هذه الدالة؟
//      مثال: دالة "معالجة_البيانات" تستدعي:
//        → دالة "احسب_المجموع" في سطر ٥٠
//        → دالة "اطبع_النتيجة" في سطر ٥٥
//
// يتم تفعيله عبر:
//   - Shift+Alt+H (تسلسل هرمي للاستدعاءات)
//   - أو النقر بزر الفأرة الأيمن → "إظهار شجرة الاستدعاءات"
//
// ══════════════════════════════════════════════════════════════════════════════
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"
#include <algorithm>

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  تحضير شجرة الاستدعاءات: إيجاد الدالة عند الموضع المحدد
// ══════════════════════════════════════════════════════════════════════════════
//
// عندما يضع المبرمج المؤشر على اسم دالة ويطلب شجرة الاستدعاءات،
// نبحث عن الرمز (الدالة أو الطريقة) عند ذلك الموضع
//

std::vector<CallHierarchyItem> LspEngine::call_hierarchy_prepare(
    const DocumentUri& uri,
    const Position& position)
{
    std::vector<CallHierarchyItem> items;

    auto doc = doc_store_->get(uri);
    if (!doc) return items;

    auto lines = arabic::split_lines(doc->content);

    auto symbols = index_->get_document_symbols(uri);

    // استخراج الكلمة عند الموضع لمطابقتها مع الرموز
    std::string target_word;
    if (position.line < static_cast<int>(lines.size())) {
        const auto& line = lines[position.line];
        int start = std::min(position.character, static_cast<int>(line.size()));
        while (start > 0) {
            unsigned char b = static_cast<unsigned char>(line[start - 1]);
            if (b >= 0x80 || std::isalnum(b) || b == '_') start--;
            else break;
        }
        int end = std::min(position.character, static_cast<int>(line.size()));
        while (end < static_cast<int>(line.size())) {
            unsigned char b = static_cast<unsigned char>(line[end]);
            if (b >= 0x80 || std::isalnum(b) || b == '_') end++;
            else break;
        }
        if (end > start) {
            target_word = line.substr(start, end - start);
        }
    }

    if (target_word.empty()) return items;

    // البحث عن الرمز الذي يطابق الكلمة
    for (const auto& sym : symbols) {
        if (sym.name != target_word) continue;
        if (sym.kind != AnalyzedSymbolKind::Function &&
            sym.kind != AnalyzedSymbolKind::Method) {
            continue;
        }

        CallHierarchyItem item;
        item.name = sym.name;
        item.uri = uri;

        // تحديد النوع: دالة عادية أو طريقة صنف
        if (sym.kind == AnalyzedSymbolKind::Method) {
            item.kind = SymbolKind::Method;
            item.detail = "طريقة";
        } else {
            item.kind = SymbolKind::Function;
            item.detail = "دالة";
        }

        // إضافة معلومات إضافية عن الدالة
        auto refs = index_->find_references(sym.name, uri);
        int ref_count = 0;
        for (const auto& r : refs) {
            if (r.range.start.line != sym.definition_range.start.line) ref_count++;
        }

        if (ref_count > 0) {
            item.detail += " (";
            item.detail += std::to_string(ref_count);
            item.detail += " مرجع)";
        }

        item.range = sym.definition_range;
        item.selection_range = {{sym.definition_range.start.line,
                                  sym.definition_range.start.character},
                                 {sym.definition_range.start.line,
                                  sym.definition_range.start.character +
                                  static_cast<int>(sym.name.size())}};

        items.push_back(item);
        break;  // نرجع العنصر الأول فقط
    }

    return items;
}

// ══════════════════════════════════════════════════════════════════════════════
//  الاستدعاءات الواردة: من يستدعي هذه الدالة؟
// ══════════════════════════════════════════════════════════════════════════════
//
// لكل دالة، نبحث عن جميع المراجع لها في جميع الملفات المفتوحة.
// ثم لكل مرجع، نحدد أي دالة يقع فيها هذا المرجع.
// النتيجة: قائمة بالدوال التي تستدعي الدالة الهدف.
//

std::vector<CallHierarchyIncomingCall> LspEngine::call_hierarchy_incoming(
    const CallHierarchyItem& item)
{
    std::vector<CallHierarchyIncomingCall> incoming;

    auto all_uris = doc_store_->get_all_uris();

    for (const auto& doc_uri : all_uris) {
        auto doc = doc_store_->get(doc_uri);
        if (!doc) continue;

        auto symbols = index_->get_document_symbols(doc_uri);
        auto refs = index_->find_references(item.name, doc_uri);

        for (const auto& ref : refs) {
            // تجاهل التعريف نفسه
            if (doc_uri == item.uri &&
                ref.range.start.line == item.range.start.line &&
                ref.range.start.character == item.range.start.character) {
                continue;
            }

            // إيجاد الدالة التي يقع فيها هذا المرجع
            const AnalyzedSymbol* containing_func = nullptr;
            for (const auto& sym : symbols) {
                if (sym.kind != AnalyzedSymbolKind::Function &&
                    sym.kind != AnalyzedSymbolKind::Method) continue;
                if (sym.is_builtin) continue;

                // التحقق أن المرجع يقع ضمن نطاق الدالة
                Position ref_pos = {ref.range.start.line, ref.range.start.character};
                if (sym.definition_range.contains(ref_pos)) {
                    if (!containing_func ||
                        sym.definition_range.start > containing_func->definition_range.start) {
                        containing_func = &sym;
                    }
                }
            }

            if (containing_func) {
                // التحقق من عدم التكرار
                bool already_added = false;
                for (auto& inc : incoming) {
                    if (inc.from.name == containing_func->name &&
                        inc.from.uri == doc_uri) {
                        // إضافة نطاق المرجع الإضافي
                        inc.from_ranges.push_back(ref.range);
                        already_added = true;
                        break;
                    }
                }

                if (!already_added) {
                    CallHierarchyIncomingCall call;
                    call.from.name = containing_func->name;
                    call.from.uri = doc_uri;
                    call.from.kind = (containing_func->kind == AnalyzedSymbolKind::Method) ? SymbolKind::Method : SymbolKind::Function;
                    call.from.detail = (containing_func->kind == AnalyzedSymbolKind::Method) ? "طريقة" : "دالة";
                    call.from.range = containing_func->definition_range;
                    call.from.selection_range = {{containing_func->definition_range.start.line,
                                                   containing_func->definition_range.start.character},
                                                  {containing_func->definition_range.start.line,
                                                   containing_func->definition_range.start.character +
                                                   static_cast<int>(containing_func->name.size())}};

                    call.from_ranges.push_back(ref.range);
                    incoming.push_back(call);
                }
            }
        }
    }

    return incoming;
}

// ══════════════════════════════════════════════════════════════════════════════
//  الاستدعاءات الصادرة: ماذا تستدعي هذه الدالة؟
// ══════════════════════════════════════════════════════════════════════════════
//
// نفحص جسم الدالة ونبحث عن جميع أسماء الدوال الأخرى المذكورة فيه.
// لكل اسم دالة نجده، نبني عنصراً في الشجرة الهرمية.
//

std::vector<CallHierarchyOutgoingCall> LspEngine::call_hierarchy_outgoing(
    const CallHierarchyItem& item)
{
    std::vector<CallHierarchyOutgoingCall> outgoing;

    auto doc = doc_store_->get(item.uri);
    if (!doc) return outgoing;

    auto lines = arabic::split_lines(doc->content);
    auto symbols = index_->get_document_symbols(item.uri);

    // جمع أسماء جميع الدوال المعروفة
    std::vector<const AnalyzedSymbol*> all_functions;
    for (const auto& sym : symbols) {
        if (sym.kind != AnalyzedSymbolKind::Function &&
            sym.kind != AnalyzedSymbolKind::Method) continue;
        if (sym.name == item.name) continue;  // تجاهل الدالة نفسها
        all_functions.push_back(&sym);
    }

    // فحص كل سطر في نطاق الدالة المحددة
    int start_line = item.range.start.line;
    int end_line = std::min(item.range.end.line, static_cast<int>(lines.size()) - 1);

    for (int i = start_line; i <= end_line; i++) {
        const auto& line = lines[i];

        for (const auto* func : all_functions) {
            // البحث عن اسم الدالة في السطر
            size_t search_pos = 0;
            while ((search_pos = line.find(func->name, search_pos)) != std::string::npos) {
                int col = static_cast<int>(search_pos);

                // التحقق من أن هذا ليس جزءاً من كلمة أكبر
                bool valid = true;
                if (col > 0) {
                    unsigned char prev = static_cast<unsigned char>(line[col - 1]);
                    if (prev >= 0x80 || std::isalnum(prev) || prev == '_')
                        valid = false;
                }
                size_t end_pos = search_pos + func->name.size();
                if (end_pos < line.size()) {
                    unsigned char next = static_cast<unsigned char>(line[end_pos]);
                    if (next >= 0x80 || std::isalnum(next) || next == '_')
                        valid = false;
                }

                if (valid) {
                    Range ref_range = {{i, col}, {i, col + static_cast<int>(func->name.size())}};

                    // التحقق من عدم التكرار
                    bool already_added = false;
                    for (auto& out : outgoing) {
                        if (out.to.name == func->name) {
                            out.from_ranges.push_back(ref_range);
                            already_added = true;
                            break;
                        }
                    }

                    if (!already_added) {
                        CallHierarchyOutgoingCall call;
                        call.to.name = func->name;
                        call.to.uri = item.uri;
                        call.to.kind = (func->kind == AnalyzedSymbolKind::Method) ? SymbolKind::Method : SymbolKind::Function;
                        call.to.detail = (func->kind == AnalyzedSymbolKind::Method) ? "طريقة" : "دالة";
                        call.to.range = func->definition_range;
                        call.to.selection_range = {{func->definition_range.start.line,
                                                     func->definition_range.start.character},
                                                    {func->definition_range.start.line,
                                                     func->definition_range.start.character +
                                                     static_cast<int>(func->name.size())}};
                        call.from_ranges.push_back(ref_range);
                        outgoing.push_back(call);
                    }
                }

                search_pos = end_pos;
            }
        }
    }

    return outgoing;
}

} // namespace lsp
} // namespace sad
