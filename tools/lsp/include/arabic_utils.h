// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: arabic_utils.h
// الوصف: أدوات معالجة النص العربي للخادم
// ══════════════════════════════════════════════════════════════════════════════
//
// هذا الملف يوفر دوالاً متخصصة لمعالجة النصوص العربية في سياق LSP.
// يتعامل مع المشاكل الفريدة للغة العربية:
//
//   ✦ التشكيل: حركات مثل الفتحة والضمة والكسرة
//     يجب تجاهلها عند مقارنة الكلمات المفتاحية
//     مثال: "دَالِة" = "دالة" = "دَالَة"
//
//   ✦ الألف بأشكالها: آ أ إ ا
//     يُعامل كحرف واحد عند البحث الضبابي
//
//   ✦ التاء المربوطة والهاء: ة ه
//     يمكن التبادل بينهما في بعض السياقات
//
//   ✦ اتجاه النص (BiDi): الكود العربي يمزج بين RTL و LTR
//     يجب حساب العمود بوحدات UTF-16 وفقاً لمواصفات LSP
//
//   ✦ الياء النقطية/غير النقطية: ي ى
//     تُعامل كحرف واحد عند البحث المرن
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace sad {
namespace lsp {
namespace arabic {

// ══════════════════════════════════════════════════════════════════════════════
// ثوابت يونيكود عربية
// ══════════════════════════════════════════════════════════════════════════════

/// نطاق الحروف العربية الأساسية
constexpr uint32_t ARABIC_START = 0x0600;
constexpr uint32_t ARABIC_END = 0x06FF;

/// حركات التشكيل العربية (يجب تجاهلها عند المقارنة)
constexpr uint32_t FATHATAN   = 0x064B;  // الفتحتان ً
constexpr uint32_t DAMMATAN   = 0x064C;  // الضمتان ٌ
constexpr uint32_t KASRATAN   = 0x064D;  // الكسرتان ٍ
constexpr uint32_t FATHAH     = 0x064E;  // الفتحة َ
constexpr uint32_t DAMMAH     = 0x064F;  // الضمة ُ
constexpr uint32_t KASRAH     = 0x0650;  // الكسرة ِ
constexpr uint32_t SHADDAH    = 0x0651;  // الشدة ّ
constexpr uint32_t SUKUN      = 0x0652;  // السكون ْ
constexpr uint32_t SUPERSCRIPT_ALEF = 0x0670; // ألف خنجرية ٰ

// ══════════════════════════════════════════════════════════════════════════════
// دوال فحص الحروف
// ══════════════════════════════════════════════════════════════════════════════

/// هل هذا رمز يونيكود حرف عربي؟
bool is_arabic_letter(uint32_t codepoint);

/// هل هذا رمز يونيكود حركة تشكيل عربية؟
bool is_arabic_diacritic(uint32_t codepoint);

/// هل هذا رمز يصلح كحرف في معرّف (اسم متغير/دالة)؟
bool is_identifier_char(uint32_t codepoint);

/// هل هذا رمز يصلح كبداية معرّف؟
bool is_identifier_start(uint32_t codepoint);

/// فحص سريع على مستوى البايت: هل هذا البايت يمكن أن يكون جزءاً من معرّف؟
/// يعمل مع ASCII والحروف العربية المتعددة البايتات (continuation bytes)
bool is_identifier_char_byte(unsigned char byte);

// ══════════════════════════════════════════════════════════════════════════════
// دوال معالجة النصوص العربية
// ══════════════════════════════════════════════════════════════════════════════

/// إزالة كل حركات التشكيل من النص
/// مثال: "دَالِة" → "دالة"
std::string strip_diacritics(const std::string& text);

/// تطبيع النص العربي (توحيد أشكال الألف والياء والتاء)
/// مثال: "إذَا" → "اذا"
std::string normalize_arabic(const std::string& text);

/// مقارنة مرنة لنصين عربيين (تتجاهل التشكيل واختلافات الألف)
/// مثال: fuzzy_match("دَالِة", "دالة") → true
bool fuzzy_match_arabic(const std::string& a, const std::string& b);

/// حساب نسبة التشابه بين نصين عربيين (0.0 - 1.0)
/// تستخدم للإكمال التلقائي ومقترحات "هل تقصد...؟"
double similarity_score(const std::string& a, const std::string& b);

// ══════════════════════════════════════════════════════════════════════════════
// دوال تحويل UTF
// ══════════════════════════════════════════════════════════════════════════════

/// فك ترميز UTF-8 إلى رمز يونيكود واحد
/// تُرجع عدد البايتات المستهلكة (1-4)
int utf8_decode(const char* bytes, uint32_t& codepoint);

/// ترميز رمز يونيكود إلى UTF-8
/// تُرجع عدد البايتات المكتوبة (1-4)
int utf8_encode(uint32_t codepoint, char* output);

/// تحويل إزاحة بايت UTF-8 إلى عمود UTF-16 (وحدات LSP)
/// LSP يستخدم UTF-16 code units لحساب العمود
int utf8_offset_to_utf16_column(const std::string& line, int byte_offset);

/// تحويل عمود UTF-16 إلى إزاحة بايت UTF-8
int utf16_column_to_utf8_offset(const std::string& line, int utf16_column);

/// حساب طول النص بوحدات UTF-16
int utf16_length(const std::string& utf8_text);

/// تقسيم النص إلى أسطر (يدعم \n و \r\n و \r)
std::vector<std::string> split_lines(const std::string& text);

/// استخراج الكلمة عند موضع معين (يفهم المعرفات العربية)
/// مثال: "متغير اسم = 5" مع العمود على 'س' → "اسم"
std::string get_word_at(const std::string& line, int character);

/// استخراج المعرّف الكامل عند إزاحة بايت معينة
/// تُرجع (بداية, نهاية) بالبايتات
std::pair<int, int> get_identifier_range(const std::string& line, int byte_offset);

} // namespace arabic
} // namespace lsp
} // namespace sad
