// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: document_links_provider.cpp
// الوصف: مزود الروابط في المستند - روابط قابلة للنقر لمسارات الاستيراد
// ══════════════════════════════════════════════════════════════════════════════
//
// روابط المستند (Document Links) تجعل مسارات الاستيراد قابلة للنقر:
//
//   استورد "رياضيات/حساب"     ← النقر يفتح ملف حساب.sad
//   استورد "أدوات/نصوص"       ← النقر يفتح ملف نصوص.sad
//   # ملاحظة: راجع ملف حساب.sad  ← النقر يفتح الملف المذكور
//
// الميزات:
//   ● اكتشاف تلقائي لجمل الاستيراد (استورد/import)
//   ● اكتشاف مسارات الملفات في التعليقات
//   ● تلميح عند التمرير فوق الرابط يوضح المسار الكامل
//   ● دعم المسارات النسبية والمطلقة
//
// ══════════════════════════════════════════════════════════════════════════════
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"
#include <regex>

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة لاكتشاف الروابط
// ══════════════════════════════════════════════════════════════════════════════

/// استخراج مسار ملف من جملة استيراد
/// الأنماط المدعومة:
///   استورد "مسار/ملف"
///   استورد 'مسار/ملف'
///   import "path/file"
///   import 'path/file'
struct ImportMatch {
    int start_col;   // بداية المسار (بدون علامات الاقتباس)
    int end_col;     // نهاية المسار (بدون علامات الاقتباس)
    std::string path; // المسار المستخرج
};

/// إيجاد جمل الاستيراد في سطر واحد
static std::vector<ImportMatch> find_imports_in_line(const std::string& line) {
    std::vector<ImportMatch> matches;

    // البحث عن أنماط الاستيراد العربية والإنجليزية
    // استورد "..." أو استورد '...'
    // import "..." أو import '...'
    // تحميل "..." أو تحميل '...'
    // ادخل "..." أو ادخل '...'

    // قائمة الكلمات المفتاحية للاستيراد
    const std::vector<std::string> import_keywords = {
        "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x88\xd8\xb1\xd8\xaf",  // استورد
        "\xd8\xaa\xd8\xad\xd9\x85\xd9\x8a\xd9\x84",            // تحميل
        "\xd8\xa7\xd8\xaf\xd8\xae\xd9\x84",                      // ادخل
        "import",
        "require",
        "include"
    };

    for (const auto& kw : import_keywords) {
        size_t pos = line.find(kw);
        if (pos == std::string::npos) continue;

        // البحث عن علامة الاقتباس بعد الكلمة المفتاحية
        size_t after_kw = pos + kw.size();
        // تجاوز المسافات
        while (after_kw < line.size() && (line[after_kw] == ' ' || line[after_kw] == '\t'))
            after_kw++;

        if (after_kw >= line.size()) continue;

        char quote_char = line[after_kw];
        if (quote_char != '"' && quote_char != '\'') continue;

        // إيجاد علامة الاقتباس المغلقة
        size_t path_start = after_kw + 1;
        size_t path_end = line.find(quote_char, path_start);
        if (path_end == std::string::npos) continue;

        ImportMatch m;
        m.start_col = static_cast<int>(path_start);
        m.end_col = static_cast<int>(path_end);
        m.path = line.substr(path_start, path_end - path_start);
        matches.push_back(m);
    }

    return matches;
}

/// البحث عن مسارات ملفات .sad في التعليقات
/// مثال: # راجع ملف حساب.sad
static std::vector<ImportMatch> find_file_paths_in_comments(const std::string& line) {
    std::vector<ImportMatch> matches;

    // التحقق أن السطر يبدأ بتعليق
    std::string trimmed = line;
    size_t first_non_space = trimmed.find_first_not_of(" \t");
    if (first_non_space == std::string::npos) return matches;

    bool is_comment = false;
    if (trimmed[first_non_space] == '#' ||
        trimmed.substr(first_non_space, 2) == "//" ||
        trimmed.substr(first_non_space, 2) == "/*") {
        is_comment = true;
    }

    if (!is_comment) return matches;

    // البحث عن أنماط ملفات .sad
    const std::string sad_ext = ".sad";
    size_t search_pos = 0;
    while ((search_pos = line.find(sad_ext, search_pos)) != std::string::npos) {
        // الرجوع للخلف لإيجاد بداية اسم الملف
        int file_end = static_cast<int>(search_pos + sad_ext.size());
        int file_start = static_cast<int>(search_pos);
        while (file_start > 0) {
            unsigned char c = static_cast<unsigned char>(line[file_start - 1]);
            if (c >= 0x80 || std::isalnum(c) || c == '_' || c == '/' || c == '\\' || c == '.') {
                file_start--;
            } else {
                break;
            }
        }

        if (file_start < static_cast<int>(search_pos)) {
            ImportMatch m;
            m.start_col = file_start;
            m.end_col = file_end;
            m.path = line.substr(file_start, file_end - file_start);
            matches.push_back(m);
        }

        search_pos = file_end;
    }

    return matches;
}

// ══════════════════════════════════════════════════════════════════════════════
//  التنفيذ الرئيسي: اكتشاف الروابط في المستند
// ══════════════════════════════════════════════════════════════════════════════
//
// نمسح المستند سطراً سطراً ونبحث عن:
//   ١. جمل الاستيراد (استورد/import/تحميل/ادخل)
//   ٢. مسارات ملفات .sad في التعليقات
//
// لكل مسار نجده، ننشئ DocumentLink يمكن النقر عليه
//

std::vector<DocumentLink> LspEngine::document_links(const DocumentUri& uri) {
    std::vector<DocumentLink> links;

    auto doc = doc_store_->get(uri);
    if (!doc) return links;

    auto lines = arabic::split_lines(doc->content);

    // استخراج مسار المجلد من عنوان URI للمستند الحالي
    // نستخدمه لتحويل المسارات النسبية إلى مسارات مطلقة
    std::string base_path;
    size_t last_slash = uri.rfind('/');
    if (last_slash != std::string::npos) {
        base_path = uri.substr(0, last_slash + 1);
    }

    for (int i = 0; i < static_cast<int>(lines.size()); i++) {
        const auto& line = lines[i];

        // ──── ١. البحث عن جمل الاستيراد ────
        auto imports = find_imports_in_line(line);
        for (const auto& imp : imports) {
            DocumentLink link;
            link.range = {{i, imp.start_col}, {i, imp.end_col}};

            // بناء عنوان URI الهدف
            std::string target_path = imp.path;
            // إضافة امتداد .sad إذا لم يكن موجوداً
            if (target_path.find(".sad") == std::string::npos) {
                target_path += ".sad";
            }
            // تحويل الفواصل المائلة العكسية إلى أمامية
            for (auto& ch : target_path) {
                if (ch == '\\') ch = '/';
            }

            link.target = base_path + target_path;
            link.tooltip = "\xd8\xa7\xd9\x81\xd8\xaa\xd8\xad "  // "افتح "
                          + target_path
                          + " \xf0\x9f\x93\x82";  // " 📂"
            links.push_back(link);
        }

        // ──── ٢. البحث عن مسارات الملفات في التعليقات ────
        auto file_paths = find_file_paths_in_comments(line);
        for (const auto& fp : file_paths) {
            DocumentLink link;
            link.range = {{i, fp.start_col}, {i, fp.end_col}};

            std::string target_path = fp.path;
            for (auto& ch : target_path) {
                if (ch == '\\') ch = '/';
            }

            link.target = base_path + target_path;
            link.tooltip = "\xd8\xa7\xd9\x86\xd8\xaa\xd9\x82\xd9\x84 \xd8\xa5\xd9\x84\xd9\x89 "  // "انتقل إلى "
                          + target_path
                          + " \xf0\x9f\x94\x97";  // " 🔗"
            links.push_back(link);
        }
    }

    return links;
}

} // namespace lsp
} // namespace sad
