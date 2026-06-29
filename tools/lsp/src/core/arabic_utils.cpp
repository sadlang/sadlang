// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: arabic_utils.cpp
// الوصف: تنفيذ أدوات معالجة النص العربي
// ══════════════════════════════════════════════════════════════════════════════
// يوفر الدوال الأساسية لمعالجة النصوص العربية في سياق LSP:
//   - فك/ترميز UTF-8/UTF-16
//   - إزالة التشكيل والتطبيع
//   - البحث الضبابي للإكمال التلقائي
//   - تحويل مواضع الأعمدة بين UTF-8 و UTF-16
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "arabic_utils.h"
#include <algorithm>
#include <cstring>

namespace sad {
namespace lsp {
namespace arabic {

// ══════════════════════════════════════════════════════════════════════════════
//  فك ترميز UTF-8
// ══════════════════════════════════════════════════════════════════════════════

int utf8_decode(const char* bytes, uint32_t& codepoint) {
    // رمز ASCII عادي (0xxxxxxx)
    auto b = static_cast<unsigned char>(bytes[0]);
    if (b < 0x80) {
        codepoint = b;
        return 1;
    }
    // رمز ببايتين (110xxxxx 10xxxxxx)
    if ((b & 0xE0) == 0xC0) {
        codepoint = (b & 0x1F) << 6;
        codepoint |= (static_cast<unsigned char>(bytes[1]) & 0x3F);
        return 2;
    }
    // رمز بثلاث بايتات (1110xxxx 10xxxxxx 10xxxxxx)
    if ((b & 0xF0) == 0xE0) {
        codepoint = (b & 0x0F) << 12;
        codepoint |= (static_cast<unsigned char>(bytes[1]) & 0x3F) << 6;
        codepoint |= (static_cast<unsigned char>(bytes[2]) & 0x3F);
        return 3;
    }
    // رمز بأربع بايتات (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
    if ((b & 0xF8) == 0xF0) {
        codepoint = (b & 0x07) << 18;
        codepoint |= (static_cast<unsigned char>(bytes[1]) & 0x3F) << 12;
        codepoint |= (static_cast<unsigned char>(bytes[2]) & 0x3F) << 6;
        codepoint |= (static_cast<unsigned char>(bytes[3]) & 0x3F);
        return 4;
    }
    // بايت غير صالح
    codepoint = 0xFFFD; // رمز الاستبدال
    return 1;
}

int utf8_encode(uint32_t codepoint, char* output) {
    if (codepoint < 0x80) {
        output[0] = static_cast<char>(codepoint);
        return 1;
    }
    if (codepoint < 0x800) {
        output[0] = static_cast<char>(0xC0 | (codepoint >> 6));
        output[1] = static_cast<char>(0x80 | (codepoint & 0x3F));
        return 2;
    }
    if (codepoint < 0x10000) {
        output[0] = static_cast<char>(0xE0 | (codepoint >> 12));
        output[1] = static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        output[2] = static_cast<char>(0x80 | (codepoint & 0x3F));
        return 3;
    }
    output[0] = static_cast<char>(0xF0 | (codepoint >> 18));
    output[1] = static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
    output[2] = static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
    output[3] = static_cast<char>(0x80 | (codepoint & 0x3F));
    return 4;
}

// ══════════════════════════════════════════════════════════════════════════════
//  فحص الحروف العربية
// ══════════════════════════════════════════════════════════════════════════════

bool is_arabic_letter(uint32_t cp) {
    // الحروف العربية الأساسية: U+0621 - U+064A
    if (cp >= 0x0621 && cp <= 0x064A) return true;
    // العربية الموسعة: U+0671 - U+06FF
    if (cp >= 0x0671 && cp <= 0x06FF) return true;
    // العربية الموسعة-A: U+FB50 - U+FDFF
    if (cp >= 0xFB50 && cp <= 0xFDFF) return true;
    // أشكال العرض العربية-B: U+FE70 - U+FEFF
    if (cp >= 0xFE70 && cp <= 0xFEFF) return true;
    return false;
}

bool is_arabic_diacritic(uint32_t cp) {
    // حركات التشكيل: U+064B - U+065F
    if (cp >= 0x064B && cp <= 0x065F) return true;
    // ألف خنجرية
    if (cp == 0x0670) return true;
    // علامات التشكيل الموسعة
    if (cp >= 0x06D6 && cp <= 0x06ED) return true;
    return false;
}

bool is_identifier_start(uint32_t cp) {
    // حرف عربي
    if (is_arabic_letter(cp)) return true;
    // حرف لاتيني
    if ((cp >= 'a' && cp <= 'z') || (cp >= 'A' && cp <= 'Z')) return true;
    // شرطة سفلية
    if (cp == '_') return true;
    return false;
}

bool is_identifier_char(uint32_t cp) {
    if (is_identifier_start(cp)) return true;
    // أرقام
    if (cp >= '0' && cp <= '9') return true;
    // أرقام عربية-هندية
    if (cp >= 0x0660 && cp <= 0x0669) return true;
    // حركات التشكيل (مسموح بها داخل المعرف)
    if (is_arabic_diacritic(cp)) return true;
    return false;
}

bool is_identifier_char_byte(unsigned char byte) {
    // ASCII: حروف لاتينية أو شرطة سفلية أو أرقام
    if ((byte >= 'a' && byte <= 'z') || (byte >= 'A' && byte <= 'Z') ||
        byte == '_' || (byte >= '0' && byte <= '9')) {
        return true;
    }
    // بايتات UTF-8 المتعددة (الحروف العربية تبدأ بـ 0xC0+ أو 0x80+ continuation)
    // أي بايت >= 0x80 يعني أنه جزء من حرف UTF-8 متعدد البايتات (عربي غالباً)
    if (byte >= 0x80) return true;
    return false;
}

// ══════════════════════════════════════════════════════════════════════════════
//  إزالة التشكيل والتطبيع
// ══════════════════════════════════════════════════════════════════════════════

std::string strip_diacritics(const std::string& text) {
    std::string result;
    result.reserve(text.size());

    size_t i = 0;
    while (i < text.size()) {
        uint32_t cp;
        int len = utf8_decode(text.c_str() + i, cp);
        // تجاهل حركات التشكيل
        if (!is_arabic_diacritic(cp)) {
            result.append(text.c_str() + i, len);
        }
        i += len;
    }
    return result;
}

std::string normalize_arabic(const std::string& text) {
    // أولاً: إزالة التشكيل
    std::string stripped = strip_diacritics(text);

    // ثانياً: توحيد أشكال الألف
    std::string result;
    result.reserve(stripped.size());

    size_t i = 0;
    while (i < stripped.size()) {
        uint32_t cp;
        int len = utf8_decode(stripped.c_str() + i, cp);

        // توحيد أشكال الألف → ا
        if (cp == 0x0622 || cp == 0x0623 || cp == 0x0625 || cp == 0x0627) {
            // آ أ إ ا → ا (ألف بدون همزة)
            char buf[4];
            int n = utf8_encode(0x0627, buf);
            result.append(buf, n);
        }
        // توحيد ى → ي
        else if (cp == 0x0649) {
            char buf[4];
            int n = utf8_encode(0x064A, buf);
            result.append(buf, n);
        }
        // توحيد ة → ه
        else if (cp == 0x0629) {
            char buf[4];
            int n = utf8_encode(0x0647, buf);
            result.append(buf, n);
        }
        else {
            result.append(stripped.c_str() + i, len);
        }
        i += len;
    }
    return result;
}

bool fuzzy_match_arabic(const std::string& a, const std::string& b) {
    return normalize_arabic(a) == normalize_arabic(b);
}

double similarity_score(const std::string& a, const std::string& b) {
    // تطبيع النصين
    std::string na = normalize_arabic(a);
    std::string nb = normalize_arabic(b);

    if (na.empty() || nb.empty()) return 0.0;
    if (na == nb) return 1.0;

    // حساب أطول تتابع مشترك (LCS) مبسط على مستوى البايتات
    // هذا تقريبي لكنه سريع وكافٍ للإكمال التلقائي
    size_t matches = 0;
    size_t ia = 0, ib = 0;

    while (ia < na.size() && ib < nb.size()) {
        if (na[ia] == nb[ib]) {
            matches++;
            ia++;
            ib++;
        } else {
            // جرب التقدم في الأقصر
            if (na.size() < nb.size()) {
                ib++;
            } else {
                ia++;
            }
        }
    }

    double max_len = static_cast<double>(std::max(na.size(), nb.size()));
    return static_cast<double>(matches) / max_len;
}

// ══════════════════════════════════════════════════════════════════════════════
//  تحويل مواضع UTF-8 ↔ UTF-16
// ══════════════════════════════════════════════════════════════════════════════

int utf8_offset_to_utf16_column(const std::string& line, int byte_offset) {
    int utf16_col = 0;
    int i = 0;

    while (i < byte_offset && i < static_cast<int>(line.size())) {
        uint32_t cp;
        int len = utf8_decode(line.c_str() + i, cp);

        // رموز خارج BMP تحتاج وحدتين UTF-16 (surrogate pair)
        if (cp >= 0x10000) {
            utf16_col += 2;
        } else {
            utf16_col += 1;
        }
        i += len;
    }
    return utf16_col;
}

int utf16_column_to_utf8_offset(const std::string& line, int utf16_column) {
    int utf16_col = 0;
    int i = 0;

    while (utf16_col < utf16_column && i < static_cast<int>(line.size())) {
        uint32_t cp;
        int len = utf8_decode(line.c_str() + i, cp);

        if (cp >= 0x10000) {
            utf16_col += 2;
        } else {
            utf16_col += 1;
        }
        i += len;
    }
    return i;
}

int utf16_length(const std::string& utf8_text) {
    return utf8_offset_to_utf16_column(utf8_text, static_cast<int>(utf8_text.size()));
}

// ══════════════════════════════════════════════════════════════════════════════
//  دوال النصوص
// ══════════════════════════════════════════════════════════════════════════════

std::vector<std::string> split_lines(const std::string& text) {
    std::vector<std::string> lines;
    std::string current;

    for (size_t i = 0; i < text.size(); i++) {
        if (text[i] == '\n') {
            lines.push_back(current);
            current.clear();
        } else if (text[i] == '\r') {
            lines.push_back(current);
            current.clear();
            // تخطي \n بعد \r (Windows line endings)
            if (i + 1 < text.size() && text[i + 1] == '\n') {
                i++;
            }
        } else {
            current += text[i];
        }
    }
    // السطر الأخير (قد لا ينتهي بـ \n)
    lines.push_back(current);
    return lines;
}

std::string get_word_at(const std::string& line, int character) {
    // تحويل عمود UTF-16 إلى إزاحة بايت
    int byte_off = utf16_column_to_utf8_offset(line, character);

    auto range = get_identifier_range(line, byte_off);
    if (range.first < range.second) {
        return line.substr(range.first, range.second - range.first);
    }
    return "";
}

std::pair<int, int> get_identifier_range(const std::string& line, int byte_offset) {
    if (line.empty() || byte_offset < 0) return {0, 0};

    // إذا الإزاحة خارج النص، ارجع للنهاية
    if (byte_offset >= static_cast<int>(line.size())) {
        byte_offset = static_cast<int>(line.size()) - 1;
        if (byte_offset < 0) return {0, 0};
    }

    // ارجع للبايت الأول من الرمز الحالي (قد نكون في منتصف UTF-8)
    while (byte_offset > 0 && (static_cast<unsigned char>(line[byte_offset]) & 0xC0) == 0x80) {
        byte_offset--;
    }

    // تحقق أن الرمز الحالي هو جزء من معرّف
    uint32_t cp;
    utf8_decode(line.c_str() + byte_offset, cp);
    if (!is_identifier_char(cp)) return {byte_offset, byte_offset};

    // ابحث عن بداية المعرّف (ارجع للخلف)
    int start = byte_offset;
    while (start > 0) {
        // ارجع بايت واحد
        int prev = start - 1;
        while (prev > 0 && (static_cast<unsigned char>(line[prev]) & 0xC0) == 0x80) {
            prev--;
        }
        uint32_t prev_cp;
        utf8_decode(line.c_str() + prev, prev_cp);
        if (!is_identifier_char(prev_cp)) break;
        start = prev;
    }

    // ابحث عن نهاية المعرّف (تقدم للأمام)
    int end = byte_offset;
    while (end < static_cast<int>(line.size())) {
        uint32_t end_cp;
        int len = utf8_decode(line.c_str() + end, end_cp);
        if (!is_identifier_char(end_cp)) break;
        end += len;
    }

    return {start, end};
}

} // namespace arabic
} // namespace lsp
} // namespace sad
