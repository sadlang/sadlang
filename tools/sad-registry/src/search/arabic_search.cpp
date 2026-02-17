// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: arabic_search.cpp
 * الوصف: تنفيذ محرك البحث العربي لمستودع حزم لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * هذا الملف يحتوي على التنفيذ الكامل لمحرك البحث العربي الذكي.
 *
 * المعالجة اللغوية العربية المُنفَّذة:
 *
 *   ١. إزالة التشكيل (Diacritics Removal):
 *      - الحركات في يونيكود تقع في المدى U+0610 إلى U+065F
 *      - كل حرف عربي قد يتبعه بايتات تشكيل (2-3 بايت UTF-8)
 *      - نحذف هذه البايتات للحصول على النص الأساسي
 *
 *   ٢. تطبيع الحروف (Normalization):
 *      - توحيد الهمزات: أ/إ/آ/ٱ → ا
 *      - توحيد التاء المربوطة: ة → ه
 *      - توحيد الألف المقصورة: ى → ي
 *      - تحويل الحروف اللاتينية إلى صغيرة
 *
 *   ٣. استخراج الجذور (Root Extraction):
 *      - خوارزمية تجريد مبسطة خاصة بأسماء الحزم البرمجية
 *      - إزالة السوابق: ال، و، ب، ك، ل، ف، س، م، ت، ن
 *      - إزالة اللواحق: ون، ين، ات، ة، ية، ي، ها، هم
 *      - المتبقي هو الجذر التقريبي
 *
 *   ٤. القواميس ثنائية اللغة:
 *      - قاموس مدمج يربط المصطلحات البرمجية العربية بمقابلاتها الإنجليزية
 *      - مثال: "شبكة" ↔ "network", "http"
 *      - مثال: "بيانات" ↔ "data", "database"
 *
 *   ٥. الترتيب الذكي (Relevance Scoring):
 *      - مطابقة الاسم الكاملة → أعلى درجة (1.0)
 *      - مطابقة الاسم العربي → درجة عالية (0.95)
 *      - مطابقة كلمة في الوصف → درجة متوسطة (0.5)
 *      - مطابقة الجذر → درجة أقل (0.3)
 *      - مكافأة شعبية (حسب التنزيلات)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "arabic_search.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <cstring>

namespace sad {
namespace registry {

// ═══════════════════════════════════════════════════════════════════════════════
// ثوابت يونيكود العربية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ملاحظة مهمة حول ترميز UTF-8 والعربية:
 *
 * الأحرف العربية في UTF-8 تُرمَّز ببايتين:
 *   - البايت الأول: 0xD8 أو 0xD9 أو 0xDA أو 0xDB
 *   - البايت الثاني: يحدد الحرف المحدد
 *
 * أمثلة:
 *   ا = D8 A7    ب = D8 A8    ت = D8 AA    ث = D8 AB
 *   ج = D8 AC    ح = D8 AD    خ = D8 AE    د = D8 AF
 *   ر = D8 B1    ز = D8 B2    س = D8 B3    ش = D8 B4
 *   ص = D8 B5    ض = D8 B6    ط = D8 B7    ظ = D8 B8
 *   ع = D8 B9    غ = D8 BA    ف = D9 81    ق = D9 82
 *   ك = D9 83    ل = D9 84    م = D9 85    ن = D9 86
 *   ه = D9 87    و = D9 88    ي = D9 8A
 *
 * حركات التشكيل (D9 8B - D9 92):
 *   فتحة = D9 8E    كسرة = D9 90    ضمة = D9 8F
 *   سكون = D9 92    شدة = D9 91     تنوين فتح = D9 8B
 *   تنوين كسر = D9 8D  تنوين ضم = D9 8C
 *
 * أحرف خاصة:
 *   أ = D8 A3    إ = D8 A5    آ = D8 A2    ة = D8 A9
 *   ى = D9 89    ؤ = D8 A4    ئ = D8 A6    ٱ = D9 B1
 */

// ═══════════════════════════════════════════════════════════════════════════════
// المُنشئ والتهيئة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * المُنشئ - يمكن أن يعمل بدون قاعدة بيانات (وضع الذاكرة فقط)
 */
ArabicSearchEngine::ArabicSearchEngine(std::shared_ptr<Database> db)
    : db_(db) {
}

/**
 * تهيئة المحرك:
 *   ١. بناء القواميس ثنائية اللغة والمرادفات
 *   ٢. بناء الفهرس من قاعدة البيانات (إن وُجدت)
 */
bool ArabicSearchEngine::initialize() {
    // بناء القواميس المدمجة
    build_dictionaries();

    // بناء الفهرس من قاعدة البيانات إن توفرت
    if (db_) {
        rebuild_index();
    }

    return true;
}

// ═══════════════════════════════════════════════════════════════════════════════
// المعالجة اللغوية العربية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief إزالة التشكيل من نص عربي
 *
 * التشكيل في يونيكود (UTF-8) يُرمَّز كبايتات إضافية بعد الحرف:
 *   - تبدأ بـ 0xD9 ثم بايت في المدى 0x8B-0x92 (الحركات الأساسية)
 *   - أو تبدأ بـ 0xD9 ثم 0x80 (تطويلة)
 *   - أو بايتات في المدى U+0610-U+061A (علامات أخرى)
 *
 * نمر على النص بايتاً بايتاً ونتخطى بايتات التشكيل.
 */
std::string ArabicSearchEngine::remove_diacritics(const std::string& text) {
    std::string result;
    result.reserve(text.size());

    size_t i = 0;
    while (i < text.size()) {
        unsigned char c = static_cast<unsigned char>(text[i]);

        // ─────────────────────────────────────────────────────────────
        // حالة ١: بايت ASCII عادي (0x00-0x7F) → نسخ مباشر
        // ─────────────────────────────────────────────────────────────
        if (c < 0x80) {
            result += text[i];
            i++;
            continue;
        }

        // ─────────────────────────────────────────────────────────────
        // حالة ٢: حرف بـ بايتين (0xC0-0xDF + continuation)
        // الحروف العربية والتشكيل تقع هنا
        // ─────────────────────────────────────────────────────────────
        if (c >= 0xC0 && c < 0xE0 && i + 1 < text.size()) {
            unsigned char c2 = static_cast<unsigned char>(text[i + 1]);

            // حساب نقطة الرمز Unicode: ((c & 0x1F) << 6) | (c2 & 0x3F)
            uint32_t codepoint = ((c & 0x1F) << 6) | (c2 & 0x3F);

            // التشكيل العربي: U+0610 إلى U+065F
            // وأيضاً U+0670 (ألف خنجرية)
            if ((codepoint >= 0x0610 && codepoint <= 0x065F) ||
                codepoint == 0x0670) {
                // تخطي - هذا تشكيل
                i += 2;
                continue;
            }

            // ليس تشكيل → نسخ البايتين
            result += text[i];
            result += text[i + 1];
            i += 2;
            continue;
        }

        // ─────────────────────────────────────────────────────────────
        // حالة ٣: حرف بـ ٣ بايتات (0xE0-0xEF + 2 continuation)
        // ─────────────────────────────────────────────────────────────
        if (c >= 0xE0 && c < 0xF0 && i + 2 < text.size()) {
            unsigned char c2 = static_cast<unsigned char>(text[i + 1]);
            unsigned char c3 = static_cast<unsigned char>(text[i + 2]);

            uint32_t codepoint = ((c & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);

            // علامات التجميع في المدى U+0610-U+065F (لو رُمِّزت بـ 3 بايتات)
            // والتعديلات في FE70-FEFF (أشكال العرض العربية)
            if (codepoint >= 0x0610 && codepoint <= 0x065F) {
                i += 3;
                continue;
            }

            result += text[i];
            result += text[i + 1];
            result += text[i + 2];
            i += 3;
            continue;
        }

        // ─────────────────────────────────────────────────────────────
        // حالة ٤: حرف بـ ٤ بايتات (0xF0-0xF7)
        // ─────────────────────────────────────────────────────────────
        if (c >= 0xF0 && c < 0xF8 && i + 3 < text.size()) {
            result += text[i];
            result += text[i + 1];
            result += text[i + 2];
            result += text[i + 3];
            i += 4;
            continue;
        }

        // بايت غير متوقع → نسخ
        result += text[i];
        i++;
    }

    return result;
}

/**
 * @brief تطبيع النص العربي
 *
 * الخطوات:
 *   ١. إزالة التشكيل
 *   ٢. استبدال أشكال الهمزة: أ/إ/آ → ا
 *   ٣. استبدال التاء المربوطة: ة → ه
 *   ٤. استبدال الألف المقصورة: ى → ي
 *   ٥. تحويل الأحرف اللاتينية إلى صغيرة
 *   ٦. إزالة المسافات الزائدة
 */
std::string ArabicSearchEngine::normalize(const std::string& text) {
    // الخطوة ١: إزالة التشكيل
    std::string clean = remove_diacritics(text);

    // الخطوة ٢-٤: استبدال الأحرف
    std::string result;
    result.reserve(clean.size());

    size_t i = 0;
    while (i < clean.size()) {
        unsigned char c = static_cast<unsigned char>(clean[i]);

        // ASCII
        if (c < 0x80) {
            // تحويل اللاتيني إلى صغير
            if (c >= 'A' && c <= 'Z') {
                result += (char)(c + 32);
            } else {
                result += clean[i];
            }
            i++;
            continue;
        }

        // حرف عربي (بايتان)
        if (c >= 0xC0 && c < 0xE0 && i + 1 < clean.size()) {
            unsigned char c2 = static_cast<unsigned char>(clean[i + 1]);
            uint32_t cp = ((c & 0x1F) << 6) | (c2 & 0x3F);

            // أ (U+0623) → ا (U+0627)
            if (cp == 0x0623 || cp == 0x0625 || cp == 0x0622) {
                // ا = D8 A7
                result += (char)0xD8;
                result += (char)0xA7;
                i += 2;
                continue;
            }

            // ة (U+0629) → ه (U+0647)
            if (cp == 0x0629) {
                // ه = D9 87
                result += (char)0xD9;
                result += (char)0x87;
                i += 2;
                continue;
            }

            // ى (U+0649) → ي (U+064A)
            if (cp == 0x0649) {
                // ي = D9 8A
                result += (char)0xD9;
                result += (char)0x8A;
                i += 2;
                continue;
            }

            // حرف عربي عادي → نسخ
            result += clean[i];
            result += clean[i + 1];
            i += 2;
            continue;
        }

        // ٣ بايتات أو أكثر → نسخ
        if (c >= 0xE0 && c < 0xF0 && i + 2 < clean.size()) {
            result += clean[i];
            result += clean[i + 1];
            result += clean[i + 2];
            i += 3;
            continue;
        }

        if (c >= 0xF0 && c < 0xF8 && i + 3 < clean.size()) {
            result += clean[i];
            result += clean[i + 1];
            result += clean[i + 2];
            result += clean[i + 3];
            i += 4;
            continue;
        }

        result += clean[i];
        i++;
    }

    return result;
}

/**
 * @brief استخراج الجذر العربي من كلمة
 *
 * خوارزمية تجريد مبسطة مصممة لأسماء الحزم البرمجية:
 *
 *   الخطوة ١: تطبيع الكلمة (إزالة تشكيل + توحيد أشكال)
 *   الخطوة ٢: إزالة السوابق المعروفة:
 *     - "ال" (أداة التعريف)
 *     - "و" ، "ب" ، "ك" ، "ل" ، "ف" (حروف جر ومعاطفة)
 *     - "م" ، "ت" ، "ن" (أحرف المضارعة)
 *     - "است" (سابقة الفعل المزيد)
 *   الخطوة ٣: إزالة اللواحق المعروفة:
 *     - "ون" ، "ين" (جمع مذكر سالم)
 *     - "ات" (جمع مؤنث سالم)
 *     - "ية" ، "ة" (تاء التأنيث والنسبة)
 *     - "ي" ، "ها" ، "هم" ، "هن" (ضمائر متصلة)
 *   الخطوة ٤: إرجاع ما تبقى (لا يقل عن حرفين)
 */
std::string ArabicSearchEngine::extract_root(const std::string& word) {
    std::string w = normalize(word);

    // لا حاجة لاستخراج جذر من كلمة قصيرة جداً
    // الأحرف العربية في UTF-8 = بايتان لكل حرف
    if (w.size() < 4) return w; // أقل من حرفين عربيين

    // ─────────────────────────────────────────────────────────────────
    // السوابق العربية بالـ UTF-8
    // ─────────────────────────────────────────────────────────────────

    // "ال" = D8 A7 D9 84 (4 بايت)
    const std::string al = "\xD8\xA7\xD9\x84";
    // "و" = D9 88
    const std::string waw = "\xD9\x88";
    // "ب" = D8 A8
    const std::string ba = "\xD8\xA8";
    // "ك" = D9 83
    const std::string kaf = "\xD9\x83";
    // "ل" = D9 84
    const std::string lam = "\xD9\x84";
    // "ف" = D9 81
    const std::string fa = "\xD9\x81";
    // "م" = D9 85
    const std::string meem = "\xD9\x85";
    // "ت" = D8 AA
    const std::string ta = "\xD8\xAA";
    // "است" = D8 A7 D8 B3 D8 AA (6 بايت)
    const std::string ista = "\xD8\xA7\xD8\xB3\xD8\xAA";

    // إزالة "ال" التعريف
    if (w.size() > 6 && w.substr(0, 4) == al) {
        w = w.substr(4);
    }

    // إزالة حروف الجر المفردة (إذا كانت الكلمة طويلة بما يكفي)
    std::vector<std::string> single_prefixes = {waw, ba, kaf, lam, fa};
    for (const auto& prefix : single_prefixes) {
        if (w.size() > 6 && w.substr(0, prefix.size()) == prefix) {
            w = w.substr(prefix.size());
            break;
        }
    }

    // إزالة "است" (استفعل)
    if (w.size() > 10 && w.substr(0, ista.size()) == ista) {
        w = w.substr(ista.size());
    }

    // ─────────────────────────────────────────────────────────────────
    // اللواحق العربية بالـ UTF-8
    // ─────────────────────────────────────────────────────────────────

    // "ون" = D9 88 D9 86
    const std::string suffix_oon = "\xD9\x88\xD9\x86";
    // "ين" = D9 8A D9 86
    const std::string suffix_een = "\xD9\x8A\xD9\x86";
    // "ات" = D8 A7 D8 AA
    const std::string suffix_aat = "\xD8\xA7\xD8\xAA";
    // "يه" = D9 8A D9 87 (ية بعد التطبيع)
    const std::string suffix_iyya = "\xD9\x8A\xD9\x87";
    // "ه" = D9 87 (ة بعد التطبيع)
    const std::string suffix_ha = "\xD9\x87";
    // "ي" = D9 8A
    const std::string suffix_ya = "\xD9\x8A";

    struct SuffixInfo {
        std::string suffix;
        size_t min_remaining; // الحد الأدنى للطول بعد الإزالة (بالبايت)
    };

    // الترتيب مهم: الأطول أولاً
    std::vector<SuffixInfo> suffixes = {
        {suffix_oon,  4},   // "ون" → 4 بايت = حرفان عربيان
        {suffix_een,  4},   // "ين"
        {suffix_aat,  4},   // "ات"
        {suffix_iyya, 4},   // "ية"
        {suffix_ha,   4},   // "ة/ه"
        {suffix_ya,   4},   // "ي"
    };

    for (const auto& s : suffixes) {
        if (w.size() > s.suffix.size() + s.min_remaining) {
            if (w.size() >= s.suffix.size() &&
                w.substr(w.size() - s.suffix.size()) == s.suffix) {
                w = w.substr(0, w.size() - s.suffix.size());
                break;
            }
        }
    }

    return w;
}

/**
 * @brief التحقق مما إذا كان النص يحتوي على أحرف عربية
 *
 * الأحرف العربية في UTF-8 تبدأ بالبايت 0xD8 أو 0xD9
 * المدى: U+0600 إلى U+06FF (العربية الأساسية)
 */
bool ArabicSearchEngine::is_arabic(const std::string& text) {
    for (size_t i = 0; i + 1 < text.size(); i++) {
        unsigned char c = static_cast<unsigned char>(text[i]);
        unsigned char c2 = static_cast<unsigned char>(text[i + 1]);

        // الأحرف العربية: D8 A0-BF, D9 80-BF
        if ((c == 0xD8 && c2 >= 0xA0 && c2 <= 0xBF) ||
            (c == 0xD9 && c2 >= 0x80 && c2 <= 0xBF)) {
            return true;
        }
    }
    return false;
}

/**
 * @brief تقسيم نص إلى كلمات
 *
 * يقسم بالمسافات وعلامات الترقيم.
 * يُهمل الكلمات الفارغة والقصيرة جداً (أقل من بايتين).
 */
std::vector<std::string> ArabicSearchEngine::tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::string current;

    for (size_t i = 0; i < text.size(); i++) {
        unsigned char c = static_cast<unsigned char>(text[i]);

        // فاصل: مسافة، تبويب، سطر جديد، بعض علامات الترقيم
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r' ||
            c == ',' || c == '.' || c == ';' || c == ':' ||
            c == '!' || c == '?' || c == '(' || c == ')' ||
            c == '[' || c == ']' || c == '{' || c == '}' ||
            c == '/' || c == '\\' || c == '-' || c == '_') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current += text[i];
        }
    }

    if (!current.empty()) {
        tokens.push_back(current);
    }

    return tokens;
}

// ═══════════════════════════════════════════════════════════════════════════════
// القواميس ثنائية اللغة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief بناء القواميس المدمجة
 *
 * هذا هو القاموس الأساسي الذي يربط المصطلحات البرمجية العربية
 * بمقابلاتها الإنجليزية. مُحسَّن لحزم لغة ص.
 *
 * التصنيفات:
 *   - الشبكات والاتصالات
 *   - قواعد البيانات والتخزين
 *   - الأمان والتشفير
 *   - واجهات المستخدم
 *   - الأدوات والمساعدات
 *   - الرياضيات والحسابات
 *   - النظام والملفات
 *   - الاختبار والجودة
 *   - النصوص والمعالجة
 */
void ArabicSearchEngine::build_dictionaries() {
    // ─────────────────────────────────────────────────────────────────
    // ١. الشبكات والاتصالات
    // ─────────────────────────────────────────────────────────────────
    ar_en_dict_["\xD8\xB4\xD8\xA8\xD9\x83\xD8\xA9"] = // شبكة
        {"network", "net", "http", "web", "internet"};
    ar_en_dict_["\xD8\xAE\xD8\xA7\xD8\xAF\xD9\x85"] = // خادم
        {"server", "backend", "service"};
    ar_en_dict_["\xD8\xB9\xD9\x85\xD9\x8A\xD9\x84"] = // عميل
        {"client", "frontend"};
    ar_en_dict_["\xD8\xA7\xD8\xAA\xD8\xB5\xD8\xA7\xD9\x84"] = // اتصال
        {"connection", "socket", "tcp", "udp"};
    ar_en_dict_["\xD8\xB7\xD9\x84\xD8\xA8"] = // طلب
        {"request", "req", "http"};
    ar_en_dict_["\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xAC\xD8\xA7\xD8\xA8\xD8\xA9"] = // استجابة
        {"response", "res"};

    // ─────────────────────────────────────────────────────────────────
    // ٢. قواعد البيانات والتخزين
    // ─────────────────────────────────────────────────────────────────
    ar_en_dict_["\xD8\xA8\xD9\x8A\xD8\xA7\xD9\x86\xD8\xA7\xD8\xAA"] = // بيانات
        {"data", "database", "db", "storage"};
    ar_en_dict_["\xD9\x82\xD8\xA7\xD8\xB9\xD8\xAF\xD8\xA9"] = // قاعدة
        {"database", "db", "sql"};
    ar_en_dict_["\xD8\xAA\xD8\xAE\xD8\xB2\xD9\x8A\xD9\x86"] = // تخزين
        {"storage", "cache", "store"};
    ar_en_dict_["\xD9\x85\xD9\x84\xD9\x81"] = // ملف
        {"file", "io", "fs"};

    // ─────────────────────────────────────────────────────────────────
    // ٣. الأمان والتشفير
    // ─────────────────────────────────────────────────────────────────
    ar_en_dict_["\xD8\xA3\xD9\x85\xD8\xA7\xD9\x86"] = // أمان
        {"security", "crypto", "auth"};
    ar_en_dict_["\xD8\xAA\xD8\xB4\xD9\x81\xD9\x8A\xD8\xB1"] = // تشفير
        {"encryption", "crypto", "cipher"};
    ar_en_dict_["\xD9\x85\xD8\xB5\xD8\xA7\xD8\xAF\xD9\x82\xD8\xA9"] = // مصادقة
        {"authentication", "auth", "login"};

    // ─────────────────────────────────────────────────────────────────
    // ٤. واجهات المستخدم
    // ─────────────────────────────────────────────────────────────────
    ar_en_dict_["\xD9\x88\xD8\xA7\xD8\xAC\xD9\x87\xD8\xA9"] = // واجهة
        {"ui", "gui", "interface", "frontend"};
    ar_en_dict_["\xD9\x86\xD8\xA7\xD9\x81\xD8\xB0\xD8\xA9"] = // نافذة
        {"window", "dialog", "gui"};
    ar_en_dict_["\xD8\xB2\xD8\xB1"] = // زر
        {"button", "ui", "widget"};

    // ─────────────────────────────────────────────────────────────────
    // ٥. الرياضيات والحساب
    // ─────────────────────────────────────────────────────────────────
    ar_en_dict_["\xD8\xB1\xD9\x8A\xD8\xA7\xD8\xB6\xD9\x8A\xD8\xA7\xD8\xAA"] = // رياضيات
        {"math", "mathematics", "numeric"};
    ar_en_dict_["\xD8\xAD\xD8\xB3\xD8\xA7\xD8\xA8"] = // حساب
        {"calculation", "math", "compute"};

    // ─────────────────────────────────────────────────────────────────
    // ٦. النصوص والمعالجة
    // ─────────────────────────────────────────────────────────────────
    ar_en_dict_["\xD9\x86\xD8\xB5"] = // نص
        {"text", "string", "str"};
    ar_en_dict_["\xD8\xAC\xD9\x8A\xD8\xB3\xD9\x88\xD9\x86"] = // جيسون
        {"json", "serialization"};
    ar_en_dict_["\xD8\xAA\xD9\x86\xD8\xB3\xD9\x8A\xD9\x82"] = // تنسيق
        {"format", "formatting", "parser"};
    ar_en_dict_["\xD9\x85\xD8\xAD\xD9\x84\xD9\x84"] = // محلل
        {"parser", "analyzer"};

    // ─────────────────────────────────────────────────────────────────
    // ٧. النظام
    // ─────────────────────────────────────────────────────────────────
    ar_en_dict_["\xD9\x86\xD8\xB8\xD8\xA7\xD9\x85"] = // نظام
        {"system", "os"};
    ar_en_dict_["\xD8\xB0\xD8\xA7\xD9\x83\xD8\xB1\xD8\xA9"] = // ذاكرة
        {"memory", "ram", "buffer"};
    ar_en_dict_["\xD9\x85\xD8\xB3\xD8\xA7\xD8\xB1"] = // مسار
        {"path", "filepath", "directory"};

    // ─────────────────────────────────────────────────────────────────
    // ٨. الاختبار والجودة
    // ─────────────────────────────────────────────────────────────────
    ar_en_dict_["\xD8\xA7\xD8\xAE\xD8\xAA\xD8\xA8\xD8\xA7\xD8\xB1"] = // اختبار
        {"test", "testing", "unittest"};
    ar_en_dict_["\xD8\xAA\xD8\xAD\xD9\x82\xD9\x82"] = // تحقق
        {"validation", "verify", "check"};

    // ─────────────────────────────────────────────────────────────────
    // ٩. أدوات عامة
    // ─────────────────────────────────────────────────────────────────
    ar_en_dict_["\xD8\xA3\xD8\xAF\xD8\xA7\xD8\xA9"] = // أداة
        {"tool", "utility", "utils"};
    ar_en_dict_["\xD9\x85\xD9\x83\xD8\xAA\xD8\xA8\xD8\xA9"] = // مكتبة
        {"library", "lib"};
    ar_en_dict_["\xD8\xB3\xD8\xAC\xD9\x84"] = // سجل
        {"log", "logging", "logger"};
    ar_en_dict_["\xD8\xA5\xD8\xB9\xD8\xAF\xD8\xA7\xD8\xAF"] = // إعداد
        {"config", "configuration", "settings"};
    ar_en_dict_["\xD8\xAA\xD8\xA7\xD8\xB1\xD9\x8A\xD8\xAE"] = // تاريخ
        {"date", "time", "datetime"};
    ar_en_dict_["\xD9\x84\xD9\x88\xD9\x86"] = // لون
        {"color", "colour"};
    ar_en_dict_["\xD8\xB5\xD9\x88\xD8\xB1\xD8\xA9"] = // صورة
        {"image", "picture", "img"};
    ar_en_dict_["\xD8\xB5\xD9\x88\xD8\xAA"] = // صوت
        {"audio", "sound"};

    // ─────────────────────────────────────────────────────────────────
    // بناء القاموس العكسي (إنجليزي → عربي)
    // ─────────────────────────────────────────────────────────────────
    for (const auto& [arabic, english_list] : ar_en_dict_) {
        for (const auto& en : english_list) {
            en_ar_dict_[en].push_back(arabic);
        }
    }

    // ─────────────────────────────────────────────────────────────────
    // المرادفات العربية
    // ─────────────────────────────────────────────────────────────────
    // حاسوب = كمبيوتر = حاسب
    synonyms_["\xD8\xAD\xD8\xA7\xD8\xB3\xD9\x88\xD8\xA8"] = // حاسوب
        {"\xD9\x83\xD9\x85\xD8\xA8\xD9\x8A\xD9\x88\xD8\xAA\xD8\xB1", // كمبيوتر
         "\xD8\xAD\xD8\xA7\xD8\xB3\xD8\xA8"}; // حاسب

    // برنامج = تطبيق
    synonyms_["\xD8\xA8\xD8\xB1\xD9\x86\xD8\xA7\xD9\x85\xD8\xAC"] = // برنامج
        {"\xD8\xAA\xD8\xB7\xD8\xA8\xD9\x8A\xD9\x82"}; // تطبيق
}

// ═══════════════════════════════════════════════════════════════════════════════
// الفهرسة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief فهرسة حزمة واحدة
 *
 * الخطوات:
 *   ١. إنشاء IndexedPackage يحتوي على النص المطبَّع
 *   ٢. تقسيم الأسماء والأوصاف إلى كلمات
 *   ٣. تطبيع كل كلمة واستخراج جذرها
 *   ٤. إضافة الكلمات للفهرس العكسي
 */
void ArabicSearchEngine::index_package(
        int64_t package_id,
        const std::string& name,
        const std::string& arabic_name,
        const std::string& description,
        const std::string& description_ar) {

    IndexedPackage pkg;
    pkg.id = package_id;
    pkg.name = name;
    pkg.arabic_name = arabic_name;
    pkg.description = description;
    pkg.description_ar = description_ar;

    // تطبيع الأسماء والأوصاف
    pkg.name_normalized = normalize(name);
    pkg.arabic_name_normalized = normalize(arabic_name);
    pkg.description_normalized = normalize(description);
    pkg.description_ar_normalized = normalize(description_ar);

    // ─────────────────────────────────────────────────────────────────
    // استخراج الكلمات المفتاحية
    // ─────────────────────────────────────────────────────────────────

    // من الاسم اللاتيني
    auto name_words = tokenize(pkg.name_normalized);
    for (const auto& w : name_words) {
        pkg.keywords.insert(w);
    }

    // من الاسم العربي
    auto arabic_words = tokenize(pkg.arabic_name_normalized);
    for (const auto& w : arabic_words) {
        pkg.keywords.insert(w);
        // استخراج الجذر
        std::string root = extract_root(w);
        if (!root.empty() && root != w) {
            pkg.roots.insert(root);
        }
    }

    // من الوصف الإنجليزي
    auto desc_words = tokenize(pkg.description_normalized);
    for (const auto& w : desc_words) {
        if (w.size() > 2) pkg.keywords.insert(w); // تجاهل الكلمات القصيرة جداً
    }

    // من الوصف العربي
    auto desc_ar_words = tokenize(pkg.description_ar_normalized);
    for (const auto& w : desc_ar_words) {
        pkg.keywords.insert(w);
        std::string root = extract_root(w);
        if (!root.empty() && root != w) {
            pkg.roots.insert(root);
        }
    }

    // إضافة المقابلات ثنائية اللغة
    // إذا كان الاسم العربي في القاموس → أضف المقابلات الإنجليزية
    for (const auto& w : arabic_words) {
        auto it = ar_en_dict_.find(w);
        if (it != ar_en_dict_.end()) {
            for (const auto& en : it->second) {
                pkg.keywords.insert(en);
            }
        }
    }

    // إذا كان الاسم الإنجليزي في القاموس → أضف المقابلات العربية
    for (const auto& w : name_words) {
        auto it = en_ar_dict_.find(w);
        if (it != en_ar_dict_.end()) {
            for (const auto& ar : it->second) {
                std::string ar_norm = normalize(ar);
                pkg.keywords.insert(ar_norm);
            }
        }
    }

    // ─────────────────────────────────────────────────────────────────
    // تحديث الفهارس
    // ─────────────────────────────────────────────────────────────────

    // حذف القديم إن وُجد
    remove_package(package_id);

    // إضافة للفهرس الرئيسي
    index_[package_id] = pkg;

    // إضافة للفهرس العكسي
    add_to_inverted_index(package_id, pkg.keywords);

    // إضافة لفهرس الجذور
    for (const auto& root : pkg.roots) {
        root_index_[root].insert(package_id);
    }
}

/**
 * @brief حذف حزمة من جميع الفهارس
 */
void ArabicSearchEngine::remove_package(int64_t package_id) {
    auto it = index_.find(package_id);
    if (it == index_.end()) return;

    // حذف من الفهرس العكسي
    for (const auto& kw : it->second.keywords) {
        auto inv_it = inverted_index_.find(kw);
        if (inv_it != inverted_index_.end()) {
            inv_it->second.erase(package_id);
            if (inv_it->second.empty()) {
                inverted_index_.erase(inv_it);
            }
        }
    }

    // حذف من فهرس الجذور
    for (const auto& root : it->second.roots) {
        auto root_it = root_index_.find(root);
        if (root_it != root_index_.end()) {
            root_it->second.erase(package_id);
            if (root_it->second.empty()) {
                root_index_.erase(root_it);
            }
        }
    }

    // حذف من الفهرس الرئيسي
    index_.erase(it);
}

/**
 * @brief إعادة بناء الفهرس من قاعدة البيانات
 */
void ArabicSearchEngine::rebuild_index() {
    if (!db_) return;

    // مسح الفهارس الحالية
    index_.clear();
    inverted_index_.clear();
    root_index_.clear();

    // جلب جميع الحزم وفهرستها
    auto packages = db_->get_popular_packages(10000); // جلب الكل

    for (const auto& pkg : packages) {
        index_package(pkg.id, pkg.name, pkg.arabic_name,
                      pkg.description, pkg.description_ar);
    }
}

/**
 * @brief إضافة كلمات حزمة إلى الفهرس العكسي
 */
void ArabicSearchEngine::add_to_inverted_index(
        int64_t package_id,
        const std::set<std::string>& keywords) {
    for (const auto& kw : keywords) {
        inverted_index_[kw].insert(package_id);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// البحث
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief البحث عن حزم
 *
 * خطوات البحث:
 *   ١. تطبيع نص البحث
 *   ٢. تقسيم النص إلى كلمات
 *   ٣. توسيع الكلمات (إضافة مرادفات ومقابلات ثنائية اللغة)
 *   ٤. جمع الحزم المرشحة من الفهرس العكسي وفهرس الجذور
 *   ٥. حساب درجة الصلة لكل مرشح
 *   ٦. ترتيب النتائج تنازلياً
 *   ٧. قص النتائج حسب max_results
 */
std::vector<ArabicSearchResult> ArabicSearchEngine::search(
        const std::string& query,
        int max_results) {

    // الخطوة ١: تطبيع الاستعلام
    std::string normalized_query = normalize(query);

    // الخطوة ٢: تقسيم إلى كلمات
    std::vector<std::string> query_terms = tokenize(normalized_query);
    if (query_terms.empty()) return {};

    // الخطوة ٣: توسيع الكلمات
    std::vector<std::string> expanded_terms = query_terms;

    for (const auto& term : query_terms) {
        // إضافة الجذر
        if (is_arabic(term)) {
            std::string root = extract_root(term);
            if (!root.empty() && root != term) {
                expanded_terms.push_back(root);
            }
        }

        // إضافة المقابلات ثنائية اللغة
        auto ar_it = ar_en_dict_.find(term);
        if (ar_it != ar_en_dict_.end()) {
            for (const auto& en : ar_it->second) {
                expanded_terms.push_back(en);
            }
        }

        auto en_it = en_ar_dict_.find(term);
        if (en_it != en_ar_dict_.end()) {
            for (const auto& ar : en_it->second) {
                expanded_terms.push_back(normalize(ar));
            }
        }

        // إضافة المرادفات
        auto syn_it = synonyms_.find(term);
        if (syn_it != synonyms_.end()) {
            for (const auto& syn : syn_it->second) {
                expanded_terms.push_back(normalize(syn));
            }
        }
    }

    // الخطوة ٤: جمع الحزم المرشحة
    std::set<int64_t> candidate_ids;

    // من الفهرس العكسي
    for (const auto& term : expanded_terms) {
        auto it = inverted_index_.find(term);
        if (it != inverted_index_.end()) {
            candidate_ids.insert(it->second.begin(), it->second.end());
        }

        // بحث جزئي (يحتوي على)
        for (const auto& [kw, ids] : inverted_index_) {
            if (kw.find(term) != std::string::npos ||
                term.find(kw) != std::string::npos) {
                candidate_ids.insert(ids.begin(), ids.end());
            }
        }
    }

    // من فهرس الجذور
    for (const auto& term : expanded_terms) {
        if (is_arabic(term)) {
            std::string root = extract_root(term);
            auto it = root_index_.find(root);
            if (it != root_index_.end()) {
                candidate_ids.insert(it->second.begin(), it->second.end());
            }
        }
    }

    // الخطوة ٥: حساب الصلة وبناء النتائج
    std::vector<ArabicSearchResult> results;

    for (int64_t id : candidate_ids) {
        auto it = index_.find(id);
        if (it == index_.end()) continue;

        const auto& pkg = it->second;

        ArabicSearchResult result;
        result.package_id = pkg.id;
        result.name = pkg.name;
        result.arabic_name = pkg.arabic_name;
        result.description = pkg.description;
        result.description_ar = pkg.description_ar;
        result.latest_version = pkg.latest_version;
        result.downloads = pkg.downloads;

        // حساب درجة الصلة
        result.relevance_score = compute_relevance(expanded_terms, pkg);

        if (result.relevance_score > 0.01) {
            results.push_back(result);
        }
    }

    // الخطوة ٦: ترتيب بالصلة (تنازلياً)
    std::sort(results.begin(), results.end(),
              [](const ArabicSearchResult& a, const ArabicSearchResult& b) {
                  return a.relevance_score > b.relevance_score;
              });

    // الخطوة ٧: قص النتائج
    if ((int)results.size() > max_results) {
        results.resize(max_results);
    }

    return results;
}

/**
 * @brief حساب درجة الصلة بين استعلام وحزمة
 *
 * معادلة الصلة:
 *   score = (name_match * 1.0) +
 *           (arabic_name_match * 0.95) +
 *           (keyword_match * 0.5) +
 *           (root_match * 0.3) +
 *           (partial_match * 0.2) +
 *           (popularity_bonus * 0.1)
 *
 * حيث:
 *   name_match:      ١ إذا طابق الاستعلام اسم الحزمة بالضبط
 *   arabic_name:     ١ إذا طابق الاسم العربي
 *   keyword_match:   نسبة الكلمات المطابقة في الكلمات المفتاحية
 *   root_match:      نسبة الجذور المطابقة
 *   partial_match:   نسبة المطابقة الجزئية
 *   popularity:      log10(downloads + 1) / 10
 */
double ArabicSearchEngine::compute_relevance(
        const std::vector<std::string>& query_terms,
        const IndexedPackage& pkg) {

    double score = 0.0;
    int total_terms = (int)query_terms.size();
    if (total_terms == 0) return 0.0;

    int keyword_matches = 0;
    int root_matches = 0;
    int partial_matches = 0;

    for (const auto& term : query_terms) {
        // مطابقة كاملة مع الاسم
        if (pkg.name_normalized == term) {
            score += 1.0;
        }
        // مطابقة كاملة مع الاسم العربي
        else if (pkg.arabic_name_normalized == term) {
            score += 0.95;
        }
        // مطابقة في الكلمات المفتاحية
        else if (pkg.keywords.count(term) > 0) {
            keyword_matches++;
        }
        // مطابقة الجذر
        else if (pkg.roots.count(term) > 0) {
            root_matches++;
        }
        else {
            // بحث جزئي (الكلمة جزء من كلمة مفتاحية أو العكس)
            for (const auto& kw : pkg.keywords) {
                if (kw.find(term) != std::string::npos ||
                    term.find(kw) != std::string::npos) {
                    partial_matches++;
                    break;
                }
            }
        }
    }

    // إضافة نقاط المطابقات
    if (total_terms > 0) {
        score += (double)keyword_matches / total_terms * 0.5;
        score += (double)root_matches / total_terms * 0.3;
        score += (double)partial_matches / total_terms * 0.2;
    }

    // مكافأة الشعبية (logarithmic)
    if (pkg.downloads > 0) {
        score += std::log10(static_cast<double>(pkg.downloads) + 1.0) / 10.0;
    }

    return std::min(score, 2.0); // الحد الأقصى ٢.٠
}

// ═══════════════════════════════════════════════════════════════════════════════
// استعلام المرادفات والمقابلات
// ═══════════════════════════════════════════════════════════════════════════════

std::vector<std::string> ArabicSearchEngine::get_synonyms(const std::string& term) {
    auto it = synonyms_.find(normalize(term));
    if (it != synonyms_.end()) return it->second;
    return {};
}

std::vector<std::string> ArabicSearchEngine::get_english_equivalents(const std::string& arabic_term) {
    auto it = ar_en_dict_.find(normalize(arabic_term));
    if (it != ar_en_dict_.end()) return it->second;
    return {};
}

std::vector<std::string> ArabicSearchEngine::get_arabic_equivalents(const std::string& english_term) {
    std::string lower = english_term;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    auto it = en_ar_dict_.find(lower);
    if (it != en_ar_dict_.end()) return it->second;
    return {};
}

} // namespace registry
} // namespace sad
