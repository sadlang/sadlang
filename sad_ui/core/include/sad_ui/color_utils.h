/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: color_utils.h
 * المسار: sad_ui/core/include/sad_ui/color_utils.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * أدوات تحويل الألوان العربية لجميع المنصات.
 *
 * توفر:
 * - arabicColorToHex: تحويل "أزرق" → "#2196F3"
 * - arabicColorToKotlin: تحويل "أزرق" → "Color(0xFF2196F3)"
 * - arabicColorToSwift: تحويل "أزرق" → "Color.blue"
 * - arabicColorToCSS: تحويل "أزرق" → "#2196F3"
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_COLOR_UTILS_H
#define SAD_UI_COLOR_UTILS_H

#include <string>
#include <unordered_map>
#include <optional>
#include <vector>

namespace sad {
namespace ui {

/**
 * @brief جدول الألوان: اسم عربي/إنجليزي → قيمة HEX
 */
inline const std::unordered_map<std::string, std::string>& getColorTable() {
    static const std::unordered_map<std::string, std::string> colors = {
        // الألوان الأساسية (عربي)
        {"أبيض", "#FFFFFF"},
        {"أسود", "#000000"},
        {"أحمر", "#F44336"},
        {"أخضر", "#4CAF50"},
        {"أزرق", "#2196F3"},
        {"أصفر", "#FFEB3B"},
        {"برتقالي", "#FF9800"},
        {"بنفسجي", "#9C27B0"},
        {"زهري", "#E91E63"},
        {"رمادي", "#9E9E9E"},
        {"بني", "#795548"},
        {"سماوي", "#00BCD4"},
        
        // درجات الألوان (عربي)
        {"أزرق_فاتح", "#03A9F4"},
        {"أزرق_داكن", "#1565C0"},
        {"أخضر_فاتح", "#8BC34A"},
        {"أخضر_داكن", "#2E7D32"},
        {"رمادي_فاتح", "#E0E0E0"},
        {"رمادي_داكن", "#616161"},
        
        // ألوان Material Design (عربي)
        {"رئيسي", "#1E88E5"},
        {"ثانوي", "#388E3C"},
        {"تأكيد", "#43A047"},
        {"خطر", "#E53935"},
        {"تحذير", "#FB8C00"},
        {"معلومات", "#1E88E5"},
        
        // الألوان الإنجليزية
        {"white", "#FFFFFF"},
        {"black", "#000000"},
        {"red", "#F44336"},
        {"green", "#4CAF50"},
        {"blue", "#2196F3"},
        {"yellow", "#FFEB3B"},
        {"orange", "#FF9800"},
        {"purple", "#9C27B0"},
        {"pink", "#E91E63"},
        {"gray", "#9E9E9E"},
        {"grey", "#9E9E9E"},
        {"brown", "#795548"},
        {"cyan", "#00BCD4"},
        {"teal", "#009688"},
        {"amber", "#FFC107"},
        {"indigo", "#3F51B5"},
        {"lime", "#CDDC39"},
        
        // Material colors (English)
        {"primary", "#1E88E5"},
        {"secondary", "#388E3C"},
        {"success", "#43A047"},
        {"error", "#E53935"},
        {"warning", "#FB8C00"},
        {"info", "#1E88E5"},
        {"danger", "#E53935"},
    };
    return colors;
}

/**
 * @brief تحويل اسم اللون العربي/الإنجليزي إلى HEX
 * @param colorName اسم اللون (مثل "أزرق" أو "blue")
 * @return قيمة HEX أو std::nullopt إذا غير موجود
 */
inline std::optional<std::string> arabicColorToHex(const std::string& colorName) {
    const auto& table = getColorTable();
    auto it = table.find(colorName);
    if (it != table.end()) {
        return it->second;
    }
    
    // إذا كانت القيمة hex بالفعل
    if (!colorName.empty() && colorName[0] == '#') {
        return colorName;
    }
    
    return std::nullopt;
}

/**
 * @brief تحويل اسم اللون إلى Kotlin/Compose Color
 * @param colorName اسم اللون
 * @return كود Kotlin للون
 */
inline std::string arabicColorToKotlin(const std::string& colorName) {
    auto hex = arabicColorToHex(colorName);
    if (hex) {
        // تحويل #RRGGBB إلى Color(0xFFRRGGBB)
        std::string hexValue = hex->substr(1); // إزالة #
        return "Color(0xFF" + hexValue + ")";
    }
    return "Color.Unspecified";
}

/**
 * @brief تحويل اسم اللون إلى Swift Color
 * @param colorName اسم اللون
 * @return كود Swift للون
 */
inline std::string arabicColorToSwift(const std::string& colorName) {
    // الألوان المسماة في SwiftUI
    static const std::unordered_map<std::string, std::string> swiftNames = {
        {"أبيض", "Color.white"},
        {"أسود", "Color.black"},
        {"أحمر", "Color.red"},
        {"أخضر", "Color.green"},
        {"أزرق", "Color.blue"},
        {"أصفر", "Color.yellow"},
        {"برتقالي", "Color.orange"},
        {"بنفسجي", "Color.purple"},
        {"زهري", "Color.pink"},
        {"رمادي", "Color.gray"},
        {"سماوي", "Color.cyan"},
        {"white", "Color.white"},
        {"black", "Color.black"},
        {"red", "Color.red"},
        {"green", "Color.green"},
        {"blue", "Color.blue"},
        {"yellow", "Color.yellow"},
        {"orange", "Color.orange"},
        {"purple", "Color.purple"},
        {"pink", "Color.pink"},
        {"gray", "Color.gray"},
        {"cyan", "Color.cyan"},
    };
    
    auto it = swiftNames.find(colorName);
    if (it != swiftNames.end()) {
        return it->second;
    }
    
    // تحويل HEX
    auto hex = arabicColorToHex(colorName);
    if (hex) {
        std::string hexValue = hex->substr(1);
        return "Color(hex: 0x" + hexValue + ")";
    }
    
    return "Color.primary";
}

/**
 * @brief تحويل اسم اللون إلى CSS
 * @param colorName اسم اللون
 * @return قيمة CSS للون
 */
inline std::string arabicColorToCSS(const std::string& colorName) {
    auto hex = arabicColorToHex(colorName);
    if (hex) {
        return *hex;
    }
    return "#1E88E5"; // اللون الافتراضي
}

/**
 * @brief تحويل نص تدرج عربي إلى CSS gradient
 * يدعم:
 *   "تدرج_خطي(أحمر، أزرق)"
 *   "تدرج_خطي(أحمر، أزرق، 45)"             ← زاوية
 *   "تدرج_دائري(أبيض، أزرق)"
 *   "تدرج_مخروطي(أحمر، أصفر، أخضر، أزرق)"
 * @param gradStr نص التدرج
 * @return CSS gradient string أو فارغ إذا ليس تدرجاً
 */
inline std::string arabicGradientToCSS(const std::string& gradStr) {
    // تدرج_خطي(...)
    auto parse = [](const std::string& s, const std::string& prefix) -> std::string {
        auto pos = s.find(prefix);
        if (pos == std::string::npos) return "";
        auto open = s.find('(', pos);
        auto close = s.rfind(')');
        if (open == std::string::npos || close == std::string::npos || close <= open) return "";
        return s.substr(open + 1, close - open - 1);
    };

    auto splitArgs = [](const std::string& args) -> std::vector<std::string> {
        std::vector<std::string> result;
        std::string current;
        for (char c : args) {
            if (c == ',' || c == '\xd8\x8c') { // ، (Arabic comma) is 2 bytes
                // trim
                auto s = current;
                while (!s.empty() && s.front() == ' ') s.erase(s.begin());
                while (!s.empty() && s.back() == ' ') s.pop_back();
                if (!s.empty()) result.push_back(s);
                current.clear();
            } else {
                current += c;
            }
        }
        // last
        while (!current.empty() && current.front() == ' ') current.erase(current.begin());
        while (!current.empty() && current.back() == ' ') current.pop_back();
        if (!current.empty()) result.push_back(current);
        return result;
    };

    // Handle Arabic comma (U+060C = 0xD8 0x8C)
    auto splitArgsUTF8 = [&](const std::string& args) -> std::vector<std::string> {
        std::vector<std::string> result;
        std::string current;
        for (size_t i = 0; i < args.size(); ++i) {
            unsigned char c = static_cast<unsigned char>(args[i]);
            // Check for Arabic comma (0xD8 0x8C)
            if (i + 1 < args.size() && c == 0xD8 && static_cast<unsigned char>(args[i+1]) == 0x8C) {
                auto s = current;
                while (!s.empty() && s.front() == ' ') s.erase(s.begin());
                while (!s.empty() && s.back() == ' ') s.pop_back();
                if (!s.empty()) result.push_back(s);
                current.clear();
                ++i; // skip second byte
            } else if (c == ',') {
                auto s = current;
                while (!s.empty() && s.front() == ' ') s.erase(s.begin());
                while (!s.empty() && s.back() == ' ') s.pop_back();
                if (!s.empty()) result.push_back(s);
                current.clear();
            } else {
                current += args[i];
            }
        }
        while (!current.empty() && current.front() == ' ') current.erase(current.begin());
        while (!current.empty() && current.back() == ' ') current.pop_back();
        if (!current.empty()) result.push_back(current);
        return result;
    };

    auto colorOrHex = [](const std::string& name) -> std::string {
        auto h = arabicColorToHex(name);
        if (h) return *h;
        if (!name.empty() && name[0] == '#') return name;
        return name; // pass through
    };

    // تدرج_خطي
    std::string args = parse(gradStr, "\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac_\xd8\xae\xd8\xb7\xd9\x8a"); // تدرج_خطي
    if (args.empty()) args = parse(gradStr, "linear-gradient");
    if (!args.empty()) {
        auto parts = splitArgsUTF8(args);
        if (parts.size() >= 2) {
            // Check if last arg is a number (angle)
            std::string angle = "180deg";
            int colorStart = 0;
            try {
                int a = std::stoi(parts.back());
                angle = std::to_string(a) + "deg";
                parts.pop_back();
            } catch (...) {}
            std::string css = "linear-gradient(" + angle;
            for (auto& p : parts) css += ", " + colorOrHex(p);
            css += ")";
            return css;
        }
    }

    // تدرج_دائري
    args = parse(gradStr, "\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac_\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd9\x8a"); // تدرج_دائري
    if (args.empty()) args = parse(gradStr, "radial-gradient");
    if (!args.empty()) {
        auto parts = splitArgsUTF8(args);
        if (parts.size() >= 2) {
            std::string css = "radial-gradient(circle";
            for (auto& p : parts) css += ", " + colorOrHex(p);
            css += ")";
            return css;
        }
    }

    // تدرج_مخروطي
    args = parse(gradStr, "\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac_\xd9\x85\xd8\xae\xd8\xb1\xd9\x88\xd8\xb7\xd9\x8a"); // تدرج_مخروطي
    if (args.empty()) args = parse(gradStr, "conic-gradient");
    if (!args.empty()) {
        auto parts = splitArgsUTF8(args);
        if (parts.size() >= 2) {
            std::string css = "conic-gradient(from 0deg";
            for (auto& p : parts) css += ", " + colorOrHex(p);
            css += ")";
            return css;
        }
    }

    return ""; // ليس تدرجاً
}

} // namespace ui
} // namespace sad

#endif // SAD_UI_COLOR_UTILS_H
