// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: references_provider.cpp
// الوصف: مزود تمييز الرموز الثوري (Document Highlights)
// ══════════════════════════════════════════════════════════════════════════════
//
// عند وضع المؤشر على رمز، يُميّز كل أماكن استخدامه في الملف:
//   ✦ تمييز التعريف (Write - كتابة)
//   ✦ تمييز الاستخدامات (Read - قراءة)
//   ✦ تمييز داخل نصوص الكلمات المفتاحية (Text)
//   ✦ تمييز ذكي: الإسناد = كتابة، القراءة = قراءة
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"

namespace sad {
namespace lsp {

std::vector<DocumentHighlight> LspEngine::document_highlights(
    const DocumentUri& uri, const Position& pos)
{
    std::vector<DocumentHighlight> result;

    auto doc = doc_store_->get(uri);
    if (!doc) return result;

    // ──── استخراج الاسم تحت المؤشر ────
    auto lines = arabic::split_lines(doc->content);
    if (pos.line < 0 || pos.line >= static_cast<int>(lines.size()))
        return result;

    auto [start_col, end_col] = arabic::get_identifier_range(lines[pos.line], pos.character);
    if (start_col == end_col) return result;

    std::string name = lines[pos.line].substr(start_col, end_col - start_col);
    if (name.empty()) return result;

    // ──── البحث في كل أسطر المستند عن هذا الاسم ────
    auto symbols = index_->get_document_symbols(uri);

    // هل هذا الرمز موجود في الفهرس؟
    const AnalyzedSymbol* found_sym = nullptr;
    for (const auto& sym : symbols) {
        if (sym.name == name) {
            found_sym = &sym;
            break;
        }
    }

    // ──── التعريف ────
    if (found_sym) {
        DocumentHighlight dh;
        dh.range = found_sym->name_range;
        dh.kind = DocumentHighlightKind::Write; // التعريف = كتابة
        result.push_back(dh);
    }

    // ──── البحث في الأسطر عن كل الاستخدامات ────
    for (int line_idx = 0; line_idx < static_cast<int>(lines.size()); line_idx++) {
        const auto& line = lines[line_idx];
        size_t search_pos = 0;

        while (search_pos < line.size()) {
            size_t found_pos = line.find(name, search_pos);
            if (found_pos == std::string::npos) break;

            // تحقق أن هذا ليس جزءاً من كلمة أكبر
            bool valid_start = (found_pos == 0) ||
                               !arabic::is_identifier_char_byte(static_cast<unsigned char>(line[found_pos - 1]));
            bool valid_end = (found_pos + name.size() >= line.size()) ||
                             !arabic::is_identifier_char_byte(static_cast<unsigned char>(line[found_pos + name.size()]));

            if (valid_start && valid_end) {
                // تخطي إذا هذا هو نفس موقع التعريف
                if (found_sym &&
                    line_idx == found_sym->name_range.start.line &&
                    static_cast<int>(found_pos) == found_sym->name_range.start.character) {
                    search_pos = found_pos + name.size();
                    continue;
                }

                DocumentHighlight dh;
                dh.range.start = {line_idx, static_cast<int>(found_pos)};
                dh.range.end = {line_idx, static_cast<int>(found_pos + name.size())};

                // تحديد نوع التمييز: هل هناك = بعد الاسم (كتابة)؟
                bool is_write = false;
                size_t after = found_pos + name.size();
                // تخطي المسافات
                while (after < line.size() && (line[after] == ' ' || line[after] == '\t'))
                    after++;
                if (after < line.size() && line[after] == '=' &&
                    (after + 1 >= line.size() || line[after + 1] != '=')) {
                    is_write = true;
                }

                dh.kind = is_write ? DocumentHighlightKind::Write
                                   : DocumentHighlightKind::Read;
                result.push_back(dh);
            }

            search_pos = found_pos + name.size();
        }
    }

    return result;
}

} // namespace lsp
} // namespace sad