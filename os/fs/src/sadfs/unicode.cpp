// ===================================================================
// صدFS - دعم أسماء الملفات العربية ويونيكود
// المرحلة 52: تطبيع يونيكود وترتيب RTL والتشكيل
// ===================================================================

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>

namespace sad::os::fs {

/// نموذج تطبيع يونيكود
enum class NormalizationForm : uint8_t {
    NFC  = 0,  // التركيب القانوني (الافتراضي)
    NFD  = 1,  // التفكيك القانوني
    NFKC = 2,  // التركيب المتوافق
    NFKD = 3   // التفكيك المتوافق
};

/// اتجاه النص
enum class TextDirection : uint8_t {
    LTR     = 0,  // من اليسار لليمين
    RTL     = 1,  // من اليمين لليسار (العربية، العبرية)
    Mixed   = 2   // مختلط (عربي وإنجليزي)
};

/// نوع حرف التشكيل العربي
enum class TashkeelType : uint8_t {
    None    = 0,  // بدون تشكيل
    Fathah  = 1,  // فتحة  (َ)
    Dammah  = 2,  // ضمة   (ُ)
    Kasrah  = 3,  // كسرة  (ِ)
    Sukoon  = 4,  // سكون  (ْ)
    Shaddah = 5,  // شدة   (ّ)
    Tanween = 6   // تنوين (ً، ٌ، ٍ)
};

/// نتيجة تحليل اسم ملف عربي
struct ArabicFilenameInfo {
    std::u32string normalized;       // الاسم بعد التطبيع
    std::u32string without_tashkeel; // بدون تشكيل (للبحث)
    TextDirection direction;          // اتجاه النص
    bool has_tashkeel;                // هل يحتوي تشكيلاً؟
    bool is_valid;                    // هل الاسم صالح؟
};

// === نطاقات حروف يونيكود العربية ===
namespace arabic_ranges {
    constexpr char32_t ARABIC_START      = 0x0600; // بداية الحروف العربية
    constexpr char32_t ARABIC_END        = 0x06FF; // نهاية الحروف العربية
    constexpr char32_t TASHKEEL_START    = 0x064B; // بداية علامات التشكيل
    constexpr char32_t TASHKEEL_END      = 0x065F; // نهاية علامات التشكيل
    constexpr char32_t ARABIC_EXT_A_START = 0x08A0; // التمديد العربي أ
    constexpr char32_t ARABIC_EXT_A_END   = 0x08FF;
    constexpr char32_t ARABIC_PRES_A_START = 0xFB50; // أشكال العرض أ
    constexpr char32_t ARABIC_PRES_A_END   = 0xFDFF;
    constexpr char32_t ARABIC_PRES_B_START = 0xFE70; // أشكال العرض ب
    constexpr char32_t ARABIC_PRES_B_END   = 0xFEFF;
}

// === معالج يونيكود لأسماء الملفات ===

/// معالج أسماء الملفات بيونيكود - تطبيع وترتيب وتشكيل
class UnicodeFilenames {
public:
    /// تطبيع اسم ملف حسب النموذج المطلوب
    std::u32string normalize(const std::u32string& name,
                             NormalizationForm form = NormalizationForm::NFC) const {
        std::u32string result = name;
        switch (form) {
            case NormalizationForm::NFD:
            case NormalizationForm::NFKD:
                result = decompose(result);   // تفكيك الحروف المركبة
                result = reorder_marks(result); // إعادة ترتيب علامات التشكيل
                break;
            case NormalizationForm::NFC:
            case NormalizationForm::NFKC:
                result = decompose(result);
                result = reorder_marks(result);
                result = compose(result);      // إعادة تركيب الحروف
                break;
        }
        return result;
    }

    /// إزالة التشكيل من نص عربي (للبحث والمقارنة)
    std::u32string strip_tashkeel(const std::u32string& text) const {
        std::u32string result;
        result.reserve(text.size());
        for (char32_t ch : text) {
            if (!is_tashkeel(ch)) result.push_back(ch);
        }
        return result;
    }

    /// تحديد اتجاه النص (RTL أو LTR أو مختلط)
    TextDirection detect_direction(const std::u32string& text) const {
        bool has_rtl = false, has_ltr = false;
        for (char32_t ch : text) {
            if (is_arabic(ch)) has_rtl = true;
            else if (ch >= 'A' && ch <= 'z') has_ltr = true;
        }
        if (has_rtl && has_ltr) return TextDirection::Mixed;
        if (has_rtl) return TextDirection::RTL;
        return TextDirection::LTR;
    }

    /// تحليل شامل لاسم ملف عربي
    ArabicFilenameInfo analyze(const std::u32string& filename) const {
        ArabicFilenameInfo info{};
        info.normalized = normalize(filename);
        info.without_tashkeel = strip_tashkeel(info.normalized);
        info.direction = detect_direction(filename);
        info.has_tashkeel = (info.normalized.size() != info.without_tashkeel.size());
        info.is_valid = validate_filename(filename);
        return info;
    }

    /// مقارنة اسمي ملفات مع تجاهل التشكيل
    bool compare_ignore_tashkeel(const std::u32string& a,
                                  const std::u32string& b) const {
        return strip_tashkeel(normalize(a)) == strip_tashkeel(normalize(b));
    }

    /// ترتيب أسماء ملفات عربية أبجدياً
    void sort_arabic(std::vector<std::u32string>& names) const {
        std::sort(names.begin(), names.end(),
            [this](const std::u32string& a, const std::u32string& b) {
                return collation_key(a) < collation_key(b);
            });
    }

private:
    /// هل الحرف عربي؟
    bool is_arabic(char32_t ch) const {
        using namespace arabic_ranges;
        return (ch >= ARABIC_START && ch <= ARABIC_END) ||
               (ch >= ARABIC_EXT_A_START && ch <= ARABIC_EXT_A_END) ||
               (ch >= ARABIC_PRES_A_START && ch <= ARABIC_PRES_A_END) ||
               (ch >= ARABIC_PRES_B_START && ch <= ARABIC_PRES_B_END);
    }

    /// هل الحرف علامة تشكيل؟
    bool is_tashkeel(char32_t ch) const {
        return ch >= arabic_ranges::TASHKEEL_START &&
               ch <= arabic_ranges::TASHKEEL_END;
    }

    /// التحقق من صحة اسم الملف
    bool validate_filename(const std::u32string& name) const {
        if (name.empty() || name.size() > 255) return false;
        // منع الأحرف الخاصة غير المسموحة في أسماء الملفات
        for (char32_t ch : name) {
            if (ch == '/' || ch == '\0' || ch == '\\') return false;
        }
        return true;
    }

    /// تفكيك الحروف المركبة إلى مكوناتها
    std::u32string decompose(const std::u32string& text) const {
        return text; // في التنفيذ الكامل: جداول تفكيك يونيكود
    }

    /// إعادة ترتيب علامات التشكيل حسب فئة الدمج
    std::u32string reorder_marks(const std::u32string& text) const {
        return text; // ترتيب حسب Canonical Combining Class
    }

    /// إعادة تركيب الحروف بعد التفكيك
    std::u32string compose(const std::u32string& text) const {
        return text; // في التنفيذ الكامل: خوارزمية تركيب يونيكود
    }

    /// إنشاء مفتاح ترتيب للمقارنة الأبجدية العربية
    std::u32string collation_key(const std::u32string& text) const {
        return strip_tashkeel(normalize(text));
    }
};

} // namespace sad::os::fs
