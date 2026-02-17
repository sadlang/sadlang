// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: definition_provider.cpp
// الوصف: مزود الانتقال للتعريف والمراجع الثوري
// ══════════════════════════════════════════════════════════════════════════════
//
// المميزات الثورية:
//   ✦ الانتقال للتعريف (F12 / Ctrl+Click)
//   ✦ الانتقال لتعريف الأعضاء بعد النقطة (كائن.عضو)
//   ✦ البحث عن كل المراجع (Shift+F12)
//   ✦ إضافة التعريف نفسه في قائمة المراجع
//   ✦ دعم الكلمات المفتاحية: الانتقال لتوثيقها
//
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"

namespace sad {
namespace lsp {

std::optional<Location> LspEngine::definition(const DocumentUri& uri, const Position& pos) {
    auto doc = doc_store_->get(uri);
    if (!doc) return std::nullopt;

    std::string word = doc_store_->get_word_at(uri, pos);
    if (word.empty()) return std::nullopt;

    // ──── التعامل مع النقطة: كائن.عضو ────
    std::string line = doc_store_->get_line(uri, pos.line);
    std::string resolved_word = word;

    // نبحث عن اسم الكائن قبل النقطة
    auto id_range = arabic::get_identifier_range(line, pos.character);
    if (id_range.first > 0) {
        int dot_pos = id_range.first - 1;
        // نتخطى المسافات
        while (dot_pos >= 0 && (line[dot_pos] == ' ' || line[dot_pos] == '\t'))
            dot_pos--;
        if (dot_pos >= 0 && line[dot_pos] == '.') {
            // هناك نقطة - نحاول إيجاد التعريف كعضو
            int obj_end = dot_pos;
            int obj_start = obj_end - 1;
            while (obj_start >= 0 && line[obj_start] != ' ' && line[obj_start] != '\t' &&
                   line[obj_start] != '(' && line[obj_start] != ',')
                obj_start--;
            obj_start++;
            if (obj_start < obj_end) {
                std::string obj_name = line.substr(obj_start, obj_end - obj_start);
                // نبحث عن تعريف الكائن لإيجاد صنفه
                auto obj_def = index_->find_definition(obj_name, uri, pos);
                if (obj_def && !obj_def->type.name.empty()) {
                    // نبحث عن العضو في جسم الصنف
                    auto symbols = index_->get_document_symbols(uri);
                    for (const auto& sym : symbols) {
                        if (sym.name == word && sym.scope_owner == obj_def->type.name) {
                            Location loc;
                            loc.uri = sym.uri;
                            loc.range = sym.name_range;
                            return loc;
                        }
                    }
                }
            }
        }
    }

    // ──── البحث العادي في الفهرس ────
    auto def = index_->find_definition(word, uri, pos);
    if (!def) return std::nullopt;

    // المدمجات ليس لها موقع في ملف
    if (def->is_builtin) return std::nullopt;

    Location loc;
    loc.uri = def->uri;
    loc.range = def->name_range;
    return loc;
}

// ══════════════════════════════════════════════════════════════════════════════
//  مزود المراجع - يجد كل الأماكن التي يُستخدم فيها رمز
// ══════════════════════════════════════════════════════════════════════════════

std::vector<Location> LspEngine::references(const DocumentUri& uri, const Position& pos) {
    std::vector<Location> locations;

    auto doc = doc_store_->get(uri);
    if (!doc) return locations;

    std::string word = doc_store_->get_word_at(uri, pos);
    if (word.empty()) return locations;

    // ──── جمع المراجع ────
    auto refs = index_->find_references(word, uri);
    for (const auto& ref : refs) {
        Location loc;
        loc.uri = ref.uri;
        loc.range = ref.range;
        locations.push_back(loc);
    }

    // ──── إضافة التعريف نفسه (include declaration) ────
    auto def = index_->find_definition(word, uri, pos);
    if (def && !def->is_builtin) {
        // نتأكد أن التعريف ليس مكرراً مع المراجع
        bool already_included = false;
        for (const auto& loc : locations) {
            if (loc.uri == def->uri &&
                loc.range.start.line == def->name_range.start.line &&
                loc.range.start.character == def->name_range.start.character) {
                already_included = true;
                break;
            }
        }
        if (!already_included) {
            Location loc;
            loc.uri = def->uri;
            loc.range = def->name_range;
            locations.insert(locations.begin(), loc); // التعريف أولاً
        }
    }

    return locations;
}

} // namespace lsp
} // namespace sad
