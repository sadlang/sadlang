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
#include <filesystem>

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  مساعد: استخراج مسار الملف من عبارة استيراد
//  يتعامل مع: استورد "ملف" أو من "ملف" استورد ...
// ══════════════════════════════════════════════════════════════════════════════
static std::string extract_import_path(const std::string& line, int cursor_col) {
    // نبحث عن نص بين علامتي تنصيص يحتوي المؤشر
    size_t pos = 0;
    while (pos < line.size()) {
        size_t q1 = line.find('"', pos);
        if (q1 == std::string::npos) break;
        size_t q2 = line.find('"', q1 + 1);
        if (q2 == std::string::npos) break;

        if (static_cast<int>(q1) <= cursor_col && cursor_col <= static_cast<int>(q2)) {
            return line.substr(q1 + 1, q2 - q1 - 1);
        }
        pos = q2 + 1;
    }
    return "";
}

// ══════════════════════════════════════════════════════════════════════════════
//  مساعد: تحويل مسار استيراد إلى URI فعلي
// ══════════════════════════════════════════════════════════════════════════════
static std::string resolve_import_uri(const std::string& import_path,
                                       const std::string& current_uri) {
    // استخراج مجلد الملف الحالي
    std::string dir;
    if (current_uri.substr(0, 8) == "file:///") {
        dir = current_uri.substr(8);
    } else {
        dir = current_uri;
    }
    // استبدال %20 بمسافة
    std::string decoded;
    for (size_t i = 0; i < dir.size(); i++) {
        if (dir[i] == '%' && i + 2 < dir.size() && dir[i+1] == '2' && dir[i+2] == '0') {
            decoded += ' ';
            i += 2;
        } else {
            decoded += dir[i];
        }
    }
    dir = decoded;

    // إيجاد المجلد الأب
    auto last_sep = dir.find_last_of("/\\");
    if (last_sep != std::string::npos) {
        dir = dir.substr(0, last_sep);
    }

    // بناء المسار الكامل مع امتداد .ص
    namespace fs = std::filesystem;
    fs::path base(dir);
    fs::path target = base / (import_path + ".\xd8\xb5"); // .ص

    if (fs::exists(target)) {
        return "file:///" + target.generic_string();
    }
    // نجرب بدون إضافة امتداد (ربما المسار كامل)
    target = base / import_path;
    if (fs::exists(target)) {
        return "file:///" + target.generic_string();
    }
    return "";
}

std::optional<Location> LspEngine::definition(const DocumentUri& uri, const Position& pos) {
    auto doc = doc_store_->get(uri);
    if (!doc) return std::nullopt;

    // ──── التعامل مع الاستيراد: استورد "ملف" أو من "ملف" استورد ────
    // إذا كان المؤشر على مسار ملف في عبارة استيراد، ننتقل لذلك الملف
    {
        std::string line = doc_store_->get_line(uri, pos.line);
        // استورد = \xd8\xa7\xd8\xb3\xd8\xaa\xd9\x88\xd8\xb1\xd8\xaf
        // من = \xd9\x85\xd9\x86
        static const std::string kw_import = "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x88\xd8\xb1\xd8\xaf";
        static const std::string kw_from = "\xd9\x85\xd9\x86";
        bool is_import_line = (line.find(kw_import) != std::string::npos) ||
                              (line.find(kw_from) != std::string::npos);
        if (is_import_line) {
            std::string import_path = extract_import_path(line, pos.character);
            if (!import_path.empty()) {
                std::string resolved = resolve_import_uri(import_path, uri);
                if (!resolved.empty()) {
                    Location loc;
                    loc.uri = resolved;
                    loc.range = {{0, 0}, {0, 0}};
                    return loc;
                }
            }
        }
    }

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
