// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: uri_utils.cpp
// الوصف: أدوات تحويل URIs لمسارات الملفات
// ══════════════════════════════════════════════════════════════════════════════

#include <string>
#include <algorithm>

namespace sad {
namespace lsp {
namespace uri {

/// تحويل URI إلى مسار ملف محلي
/// مثال: "file:///c%3A/project/main.sad" → "c:/project/main.sad"
std::string uri_to_path(const std::string& uri) {
    std::string path = uri;

    // إزالة البادئة file:///
    const std::string prefix = "file:///";
    if (path.substr(0, prefix.size()) == prefix) {
        path = path.substr(prefix.size());
    } else if (path.substr(0, 8) == "file:///") {
        path = path.substr(7); // يونكس: file:///path → /path
    }

    // فك ترميز النسبة المئوية (%XX)
    std::string decoded;
    decoded.reserve(path.size());
    for (size_t i = 0; i < path.size(); i++) {
        if (path[i] == '%' && i + 2 < path.size()) {
            char hex[3] = { path[i+1], path[i+2], 0 };
            decoded += static_cast<char>(std::strtol(hex, nullptr, 16));
            i += 2;
        } else {
            decoded += path[i];
        }
    }

    // توحيد الفواصل
    std::replace(decoded.begin(), decoded.end(), '\\', '/');

    return decoded;
}

/// تحويل مسار ملف محلي إلى URI
/// مثال: "c:/project/main.sad" → "file:///c%3A/project/main.sad"
std::string path_to_uri(const std::string& path) {
    std::string result = path;

    // توحيد الفواصل
    std::replace(result.begin(), result.end(), '\\', '/');

    // ترميز النسبة المئوية للأحرف الخاصة
    std::string encoded;
    encoded.reserve(result.size() * 2);

    for (char c : result) {
        if (c == ':') {
            encoded += "%3A";
        } else if (c == ' ') {
            encoded += "%20";
        } else {
            encoded += c;
        }
    }

    // إضافة البادئة
    if (!encoded.empty() && encoded[0] != '/') {
        return "file:///" + encoded;
    }
    return "file://" + encoded;
}

} // namespace uri
} // namespace lsp
} // namespace sad
