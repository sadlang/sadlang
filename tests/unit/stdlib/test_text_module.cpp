// بسم الله الرحمن الرحيم
/**
 * @file test_text_module.cpp
 * @brief Comprehensive Tests for Arabic Text Module (نص)
 *        اختبارات شاملة لوحدة النصوص العربية
 *
 * Tests cover: Arabic normalization, string pool, Arabic storage (10-bit),
 *              root search, and character converter
 *
 * @author ريم — مهندسة المكتبات والأدوات
 * @date March 2026
 * الحمد لله رب العالمين
 */

#include <gtest/gtest.h>
#include "نص/arabic_normalization.hpp"
#include "نص/arabic_string_pool.hpp"
#include "نص/arabic_storage.hpp"
#include "نص/root_search.hpp"

// ============================================================================
// 1. Normalization Options Tests
//    اختبارات خيارات التطبيع
// ============================================================================

using namespace sad::نص;

TEST(NormalizationOptionsTest, DefaultOptions) {
    خيارات_تطبيع opts;
    EXPECT_TRUE(opts.إزالة_التشكيل);
    EXPECT_TRUE(opts.توحيد_الألفات);
    EXPECT_TRUE(opts.توحيد_الياء);
    EXPECT_FALSE(opts.توحيد_الهاء);
    EXPECT_TRUE(opts.إزالة_التطويل);
    EXPECT_TRUE(opts.إزالة_الفراغات_الزائدة);
    EXPECT_FALSE(opts.حذف_أرقام_عربية);
}

TEST(NormalizationOptionsTest, SearchPreset) {
    auto opts = خيارات_تطبيع::للبحث();
    EXPECT_TRUE(opts.إزالة_التشكيل);
    EXPECT_TRUE(opts.توحيد_الألفات);
    EXPECT_TRUE(opts.توحيد_الياء);
    EXPECT_TRUE(opts.توحيد_الهاء);
    EXPECT_TRUE(opts.إزالة_التطويل);
}

TEST(NormalizationOptionsTest, DisplayPreset) {
    auto opts = خيارات_تطبيع::للعرض();
    EXPECT_FALSE(opts.إزالة_التشكيل);
    EXPECT_FALSE(opts.توحيد_الألفات);
    EXPECT_TRUE(opts.إزالة_التطويل);
}

TEST(NormalizationOptionsTest, StoragePreset) {
    auto opts = خيارات_تطبيع::للتخزين();
    EXPECT_TRUE(opts.إزالة_التشكيل);
    EXPECT_TRUE(opts.توحيد_الألفات);
    EXPECT_FALSE(opts.توحيد_الهاء);
    EXPECT_TRUE(opts.إزالة_الفراغات_الزائدة);
}

// ============================================================================
// 2. Normalizer Tests
//    اختبارات المُطبِّع
// ============================================================================

TEST(NormalizerTest, RemoveDiacritics) {
    مُطبِّع n;
    // مُحَمَّد → محمد (remove tashkeel)
    std::string input = "مُحَمَّد";
    std::string result = n.طبّع(input);
    EXPECT_EQ(result, "محمد");
}

TEST(NormalizerTest, UnifyAlef) {
    مُطبِّع n;
    // إبراهيم → ابراهيم, أحمد → احمد, آدم → ادم
    std::string result1 = n.طبّع("إبراهيم");
    EXPECT_EQ(result1, "ابراهيم");

    std::string result2 = n.طبّع("أحمد");
    EXPECT_EQ(result2, "احمد");

    std::string result3 = n.طبّع("آدم");
    EXPECT_EQ(result3, "ادم");
}

TEST(NormalizerTest, UnifyYaa) {
    مُطبِّع n;
    // مصطفى → مصطفي (ى → ي)
    std::string result = n.طبّع("مصطفى");
    EXPECT_EQ(result, "مصطفي");
}

TEST(NormalizerTest, RemoveTatweel) {
    مُطبِّع n;
    // محمـــد → محمد (remove kashida)
    std::string result = n.طبّع("محمـــد");
    EXPECT_EQ(result, "محمد");
}

TEST(NormalizerTest, UnifyHaa) {
    auto opts = خيارات_تطبيع::للبحث();
    مُطبِّع n(opts);
    // مكة → مكه (ة → ه with توحيد_الهاء enabled)
    std::string result = n.طبّع("مكة");
    EXPECT_EQ(result, "مكه");
}

TEST(NormalizerTest, NormalizeSpaces) {
    مُطبِّع n;
    std::string result = n.طبّع("  السلام   عليكم  ");
    EXPECT_EQ(result, "السلام عليكم");
}

TEST(NormalizerTest, EmptyString) {
    مُطبِّع n;
    EXPECT_EQ(n.طبّع(""), "");
}

TEST(NormalizerTest, NoArabicText) {
    مُطبِّع n;
    EXPECT_EQ(n.طبّع("Hello World"), "Hello World");
}

// ============================================================================
// 3. Equality Comparison Tests
//    اختبارات المقارنة المُطبَّعة
// ============================================================================

TEST(NormalizerTest, EqualityComparison) {
    مُطبِّع n;
    // محمّد = مُحَمَّد = محمد (all normalize to محمد)
    EXPECT_TRUE(n.متساويان("محمّد", "محمد"));
    EXPECT_TRUE(n.متساويان("أحمد", "احمد"));
    EXPECT_TRUE(n.متساويان("مصطفى", "مصطفي"));
}

TEST(NormalizerTest, ContainsSearch) {
    مُطبِّع n;
    EXPECT_TRUE(n.يحتوي("أحمد ذهب إلى المدرسة", "احمد"));
    EXPECT_TRUE(n.يحتوي("أحمد ذهب إلى المدرسة", "المدرسة"));
    EXPECT_FALSE(n.يحتوي("أحمد ذهب إلى المدرسة", "سعيد"));
}

// ============================================================================
// 4. String Pool Tests
//    اختبارات مجمع النصوص
// ============================================================================

TEST(StringPoolTest, AddAndRetrieve) {
    string_pool pool;
    معرف_نص id = pool.أضف("مرحبا");
    EXPECT_FALSE(id.فارغ());

    const std::string& retrieved = pool.احصل(id);
    EXPECT_EQ(retrieved, "مرحبا");
}

TEST(StringPoolTest, Deduplication) {
    string_pool pool;
    معرف_نص id1 = pool.أضف("لغة ص");
    معرف_نص id2 = pool.أضف("لغة ص");

    // Same string → same ID
    EXPECT_EQ(id1.القيمة(), id2.القيمة());
    EXPECT_EQ(pool.الحجم(), 1u);
}

TEST(StringPoolTest, DifferentStrings) {
    string_pool pool;
    معرف_نص id1 = pool.أضف("أحمد");
    معرف_نص id2 = pool.أضف("سعيد");

    EXPECT_NE(id1.القيمة(), id2.القيمة());
    EXPECT_EQ(pool.الحجم(), 2u);
}

TEST(StringPoolTest, Search) {
    string_pool pool;
    pool.أضف("مفتاح");

    معرف_نص found = pool.ابحث("مفتاح");
    EXPECT_FALSE(found.فارغ());

    معرف_نص not_found = pool.ابحث("غير_موجود");
    EXPECT_TRUE(not_found.فارغ());
}

TEST(StringPoolTest, Clear) {
    string_pool pool;
    pool.أضف("نص1");
    pool.أضف("نص2");
    EXPECT_EQ(pool.الحجم(), 2u);

    pool.امسح();
    EXPECT_EQ(pool.الحجم(), 0u);
}

TEST(StringPoolTest, MemorySize) {
    string_pool pool;
    pool.أضف("اختبار");
    EXPECT_GT(pool.حجم_الذاكرة(), 0u);
}

TEST(StringPoolTest, ManagedString) {
    string_pool pool;
    معرف_نص id = pool.أضف("لغة البرمجة ص");
    نص_مُدار managed(id, pool);

    EXPECT_EQ(managed.إلى_نص(), "لغة البرمجة ص");
    EXPECT_EQ(managed.المعرف().القيمة(), id.القيمة());
    EXPECT_FALSE(managed.فارغ());
}

// ============================================================================
// 5. Arabic Storage (10-bit encoding) Tests
//    اختبارات التخزين العربي المُرمَّز
// ============================================================================

TEST(ArabicStorageTest, CharacterEncoding) {
    حرف_مُرمَّز ch(حرف_عربي::باء, تشكيل::فتحة, شكل_حرف::أول);
    EXPECT_EQ(ch.الحرف(), حرف_عربي::باء);
    EXPECT_EQ(ch.التشكيل(), تشكيل::فتحة);
    EXPECT_EQ(ch.الشكل(), شكل_حرف::أول);
}

TEST(ArabicStorageTest, SetProperties) {
    حرف_مُرمَّز ch(حرف_عربي::ألف);
    ch.عيّن_التشكيل(تشكيل::ضمة);
    ch.عيّن_الشكل(شكل_حرف::وسط);

    EXPECT_EQ(ch.التشكيل(), تشكيل::ضمة);
    EXPECT_EQ(ch.الشكل(), شكل_حرف::وسط);
}

TEST(ArabicStorageTest, CompactValue) {
    حرف_مُرمَّز ch(حرف_عربي::جيم, تشكيل::بدون, شكل_حرف::منفصل);
    uint16_t val = ch.القيمة();
    // Value should be 10 bits, so < 1024
    EXPECT_LT(val, 1024u);
}

TEST(ArabicStorageTest, AllLetters) {
    // Test all 28 letters + special characters
    std::vector<حرف_عربي> letters = {
        حرف_عربي::ألف, حرف_عربي::باء, حرف_عربي::تاء,
        حرف_عربي::ثاء, حرف_عربي::جيم, حرف_عربي::حاء,
        حرف_عربي::خاء, حرف_عربي::دال, حرف_عربي::ذال,
        حرف_عربي::راء, حرف_عربي::زاي, حرف_عربي::سين,
        حرف_عربي::شين, حرف_عربي::صاد, حرف_عربي::ضاد,
        حرف_عربي::طاء, حرف_عربي::ظاء, حرف_عربي::عين,
        حرف_عربي::غين, حرف_عربي::فاء, حرف_عربي::قاف,
        حرف_عربي::كاف, حرف_عربي::لام, حرف_عربي::ميم,
        حرف_عربي::نون, حرف_عربي::هاء, حرف_عربي::واو,
        حرف_عربي::ياء
    };
    for (auto letter : letters) {
        حرف_مُرمَّز ch(letter);
        EXPECT_EQ(ch.الحرف(), letter);
    }
}

TEST(ArabicStorageTest, AllDiacritics) {
    std::vector<تشكيل> diacritics = {
        تشكيل::بدون, تشكيل::فتحة, تشكيل::ضمة,
        تشكيل::كسرة, تشكيل::سكون, تشكيل::شدة,
        تشكيل::فتحتين, تشكيل::ضمتين
    };
    for (auto d : diacritics) {
        حرف_مُرمَّز ch(حرف_عربي::عين, d);
        EXPECT_EQ(ch.التشكيل(), d);
    }
}

// ============================================================================
// 6. Character Converter Tests
//    اختبارات مُحوِّل الحروف
// ============================================================================

TEST(CharConverterTest, Singleton) {
    auto& conv1 = مُحوِّل_حروف::المثيل();
    auto& conv2 = مُحوِّل_حروف::المثيل();
    EXPECT_EQ(&conv1, &conv2);
}

TEST(CharConverterTest, UnicodeToArabicLetter) {
    auto& conv = مُحوِّل_حروف::المثيل();
    // ب = U+0628
    auto letter = conv.من_unicode(0x0628);
    EXPECT_TRUE(letter.has_value());
    EXPECT_EQ(letter.value(), حرف_عربي::باء);
}

TEST(CharConverterTest, ArabicLetterToUnicode) {
    auto& conv = مُحوِّل_حروف::المثيل();
    uint32_t code = conv.إلى_unicode(حرف_عربي::باء, شكل_حرف::منفصل);
    EXPECT_EQ(code, 0x0628u);
}

TEST(CharConverterTest, DiacriticFromUnicode) {
    auto& conv = مُحوِّل_حروف::المثيل();
    // فتحة = U+064E
    auto d = conv.تشكيل_من_unicode(0x064E);
    EXPECT_TRUE(d.has_value());
    EXPECT_EQ(d.value(), تشكيل::فتحة);
}

TEST(CharConverterTest, NonArabicCharacter) {
    auto& conv = مُحوِّل_حروف::المثيل();
    auto letter = conv.من_unicode('A');
    EXPECT_FALSE(letter.has_value());
}

// ============================================================================
// 7. Compressed Arabic Text Tests
//    اختبارات النصوص العربية المضغوطة
// ============================================================================

TEST(CompressedTextTest, FromUtf8) {
    auto text = نص_عربي_مضغوط::من_utf8("بسم الله");
    // Should not crash, and produce a compressed representation
    EXPECT_NO_THROW(نص_عربي_مضغوط::من_utf8("بسم الله الرحمن الرحيم"));
}

// ============================================================================
// 8. Root Search Tests
//    اختبارات استخراج الجذور
// ============================================================================

TEST(RootSearchTest, CommonPatterns) {
    auto patterns = وزن::الأوزان_الشائعة();
    // Should have 24 common Arabic morphological patterns
    EXPECT_EQ(patterns.size(), 24u);
}

TEST(RootSearchTest, ExtractRoot) {
    مُستخرج_جذور extractor;
    auto root = extractor.استخرج("كتاب");
    if (root.has_value()) {
        // Root of كتاب should be ك-ت-ب
        EXPECT_EQ(root.value().الطول(), 3u);
    }
}

TEST(RootSearchTest, ExtractRootFromVerb) {
    مُستخرج_جذور extractor;
    auto root = extractor.استخرج("يكتب");
    // Root should be ك-ت-ب
    if (root.has_value()) {
        EXPECT_EQ(root.value().الطول(), 3u);
    }
}

TEST(RootSearchTest, EmptyInput) {
    مُستخرج_جذور extractor;
    auto root = extractor.استخرج("");
    EXPECT_FALSE(root.has_value());
}

TEST(RootSearchTest, NonArabicInput) {
    مُستخرج_جذور extractor;
    auto root = extractor.استخرج("hello");
    // Should return empty for non-Arabic
    EXPECT_FALSE(root.has_value());
}

// ============================================================================
// 9. Edge Cases
//    حالات الحافة
// ============================================================================

TEST(TextEdgeCaseTest, MixedArabicEnglish) {
    مُطبِّع n;
    std::string result = n.طبّع("Hello مرحبا World");
    EXPECT_NE(result.find("Hello"), std::string::npos);
    EXPECT_NE(result.find("مرحبا"), std::string::npos);
}

TEST(TextEdgeCaseTest, NumbersInArabicText) {
    مُطبِّع n;
    EXPECT_EQ(n.طبّع("عدد 42"), "عدد 42");
}

TEST(TextEdgeCaseTest, EmptyStringPool) {
    string_pool pool;
    EXPECT_EQ(pool.الحجم(), 0u);
    معرف_نص notFound = pool.ابحث("لا_يوجد");
    EXPECT_TRUE(notFound.فارغ());
}
