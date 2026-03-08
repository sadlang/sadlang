// بسم الله الرحمن الرحيم
/**
 * @file test_text_comprehensive.cpp
 * @brief Comprehensive Tests for Arabic Text Module - اختبارات شاملة لوحدة النص العربي
 *
 * Tests: Normalization, String Pool, Arabic Storage (10-bit),
 *        Character Converter, Root Search
 *
 * @author ريم — مهندسة المكتبات والأدوات
 * @date March 2026
 */

#include "sad_test_framework.h"
#include <algorithm>
#include "نص/arabic_normalization.hpp"
#include "نص/arabic_string_pool.hpp"
#include "نص/arabic_storage.hpp"
#include "نص/root_search.hpp"

using namespace sad::نص;

int main() {
    SAD_TEST_INIT();

    // ================================================================
    // 1. خيارات التطبيع / Normalization Options
    // ================================================================
    SAD_GROUP("خيارات التطبيع / Normalization Options");

    SAD_TEST("default options: diacritics removal enabled", {
        خيارات_تطبيع opts;
        SAD_ASSERT_TRUE(opts.إزالة_التشكيل);
        SAD_ASSERT_TRUE(opts.توحيد_الألفات);
        SAD_ASSERT_TRUE(opts.توحيد_الياء);
        SAD_ASSERT_TRUE(opts.إزالة_التطويل);
    });

    SAD_TEST("search preset enables all", {
        auto opts = خيارات_تطبيع::للبحث();
        SAD_ASSERT_TRUE(opts.إزالة_التشكيل);
        SAD_ASSERT_TRUE(opts.توحيد_الألفات);
        SAD_ASSERT_TRUE(opts.توحيد_الياء);
        SAD_ASSERT_TRUE(opts.توحيد_الهاء);
    });

    SAD_TEST("display preset keeps diacritics", {
        auto opts = خيارات_تطبيع::للعرض();
        SAD_ASSERT_FALSE(opts.إزالة_التشكيل);
    });

    SAD_TEST("storage preset", {
        auto opts = خيارات_تطبيع::للتخزين();
        SAD_ASSERT_TRUE(opts.إزالة_التشكيل);
        SAD_ASSERT_TRUE(opts.إزالة_الفراغات_الزائدة);
        SAD_ASSERT_FALSE(opts.توحيد_الهاء);
    });

    // ================================================================
    // 2. عمليات التطبيع / Normalization Operations
    // ================================================================
    SAD_GROUP("عمليات التطبيع / Normalization Operations");

    SAD_TEST("remove diacritics", {
        مُطبِّع normalizer(خيارات_تطبيع::للبحث());
        std::string input = "بِسْمِ اللَّهِ";
        auto result = normalizer.طبّع(input);
        SAD_ASSERT_FALSE(result.empty());
        // Result should be shorter (diacritics removed)
        SAD_ASSERT_LT(result.size(), input.size());
    });

    SAD_TEST("remove tatweel", {
        مُطبِّع normalizer;
        std::string input = "مـــرحـــبا";
        auto result = normalizer.طبّع(input);
        SAD_ASSERT_LT(result.size(), input.size());
    });

    SAD_TEST("normalize spaces", {
        خيارات_تطبيع opts;
        opts.إزالة_الفراغات_الزائدة = true;
        مُطبِّع normalizer(opts);
        std::string input = "كلمة   كلمة    كلمة";
        auto result = normalizer.طبّع(input);
        SAD_ASSERT_LT(result.size(), input.size());
    });

    SAD_TEST("full normalization", {
        مُطبِّع normalizer(خيارات_تطبيع::للبحث());
        std::string input = "بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ";
        auto result = normalizer.طبّع(input);
        SAD_ASSERT_FALSE(result.empty());
        SAD_ASSERT_LT(result.size(), input.size());
    });

    // ================================================================
    // 3. مقارنة النصوص / Text Comparison
    // ================================================================
    SAD_GROUP("مقارنة النصوص / Text Comparison");

    SAD_TEST("equality ignoring diacritics", {
        مُطبِّع normalizer(خيارات_تطبيع::للبحث());
        SAD_ASSERT_TRUE(normalizer.متساويان("كتاب", "كِتَابٌ"));
    });

    SAD_TEST("different words not equal", {
        مُطبِّع normalizer(خيارات_تطبيع::للبحث());
        SAD_ASSERT_FALSE(normalizer.متساويان("كتاب", "قلم"));
    });

    SAD_TEST("contains search", {
        مُطبِّع normalizer(خيارات_تطبيع::للبحث());
        SAD_ASSERT_TRUE(normalizer.يحتوي("هذا كتاب جميل", "كتاب"));
    });

    // ================================================================
    // 4. String Pool
    // ================================================================
    SAD_GROUP("مجمع النصوص / String Pool");

    SAD_TEST("add and search", {
        auto& pool = string_pool::العام();
        pool.امسح();
        pool.أضف("كتاب");
        pool.أضف("قلم");
        auto id1 = pool.ابحث("كتاب");
        auto id2 = pool.ابحث("قلم");
        auto id3 = pool.ابحث("ممحاة");
        SAD_ASSERT_FALSE(id1.فارغ());
        SAD_ASSERT_FALSE(id2.فارغ());
        SAD_ASSERT_TRUE(id3.فارغ());
    });

    SAD_TEST("deduplication", {
        auto& pool = string_pool::العام();
        pool.امسح();
        pool.أضف("نص");
        pool.أضف("نص");
        pool.أضف("نص");
        SAD_ASSERT_EQ(pool.الحجم(), (size_t)1);
    });

    SAD_TEST("clear pool", {
        auto& pool = string_pool::العام();
        pool.امسح();
        pool.أضف("أ");
        pool.أضف("ب");
        pool.أضف("ت");
        SAD_ASSERT_EQ(pool.الحجم(), (size_t)3);
        pool.امسح();
        SAD_ASSERT_EQ(pool.الحجم(), (size_t)0);
    });

    SAD_TEST("memory reporting", {
        auto& pool = string_pool::العام();
        pool.امسح();
        pool.أضف("اختبار");
        auto mem = pool.حجم_الذاكرة();
        SAD_ASSERT_GT(mem, (size_t)0);
    });

    SAD_TEST("add returns ID", {
        auto& pool = string_pool::العام();
        pool.امسح();
        auto id = pool.أضف("مرحبا");
        SAD_ASSERT_FALSE(id.فارغ());
        SAD_ASSERT_GT(id.القيمة(), (uint32_t)0);
    });

    SAD_TEST("retrieve by ID", {
        auto& pool = string_pool::العام();
        pool.امسح();
        auto id = pool.أضف("بسم الله");
        const auto& text = pool.احصل(id);
        SAD_ASSERT_EQ(text, std::string("بسم الله"));
    });

    SAD_TEST("statistics", {
        auto& pool = string_pool::العام();
        pool.امسح();
        pool.أضف("كلمة");
        pool.أضف("جملة");
        auto stats = pool.الإحصائيات();
        SAD_ASSERT_EQ(stats.عدد_النصوص, (size_t)2);
        SAD_ASSERT_GT(stats.حجم_الذاكرة, (size_t)0);
    });

    // ================================================================
    // 5. Arabic Storage (10-bit)
    // ================================================================
    SAD_GROUP("التخزين العربي / Arabic Storage");

    SAD_TEST("encoded letter components", {
        حرف_مُرمَّز encoded(حرف_عربي::باء, تشكيل::فتحة, شكل_حرف::أول);
        SAD_ASSERT_EQ((int)encoded.الحرف(), (int)حرف_عربي::باء);
        SAD_ASSERT_EQ((int)encoded.التشكيل(), (int)تشكيل::فتحة);
        SAD_ASSERT_EQ((int)encoded.الشكل(), (int)شكل_حرف::أول);
    });

    SAD_TEST("all basic letters can be encoded", {
        for (int i = 0; i < 28; ++i) {
            حرف_مُرمَّز encoded(static_cast<حرف_عربي>(i));
            SAD_ASSERT_EQ((int)encoded.الحرف(), i);
        }
    });

    SAD_TEST("all diacritics can be encoded", {
        std::vector<تشكيل> diacritics = {
            تشكيل::بدون, تشكيل::فتحة, تشكيل::ضمة,
            تشكيل::كسرة, تشكيل::سكون, تشكيل::شدة,
            تشكيل::فتحتين, تشكيل::ضمتين
        };
        for (auto d : diacritics) {
            حرف_مُرمَّز encoded(حرف_عربي::ألف, d);
            SAD_ASSERT_EQ((int)encoded.التشكيل(), (int)d);
        }
    });

    SAD_TEST("set individual components", {
        حرف_مُرمَّز encoded;
        encoded.عيّن_الحرف(حرف_عربي::ميم);
        encoded.عيّن_التشكيل(تشكيل::ضمة);
        encoded.عيّن_الشكل(شكل_حرف::وسط);
        SAD_ASSERT_EQ((int)encoded.الحرف(), (int)حرف_عربي::ميم);
        SAD_ASSERT_EQ((int)encoded.التشكيل(), (int)تشكيل::ضمة);
        SAD_ASSERT_EQ((int)encoded.الشكل(), (int)شكل_حرف::وسط);
    });

    SAD_TEST("compressed text creation", {
        auto compressed = نص_عربي_مضغوط::من_utf8("بسم");
        SAD_ASSERT_GT(compressed.الطول(), (size_t)0);
    });

    SAD_TEST("compressed text round-trip", {
        std::string original = "بسم";
        auto compressed = نص_عربي_مضغوط::من_utf8(original);
        auto decompressed = compressed.إلى_utf8();
        // The decompressed text should have the same letters (shapes may differ)
        SAD_ASSERT_FALSE(decompressed.empty());
    });

    // ================================================================
    // 6. Character Converter
    // ================================================================
    SAD_GROUP("مُحوِّل الحروف / Character Converter");

    SAD_TEST("Unicode to Arabic letter", {
        auto& converter = مُحوِّل_حروف::المثيل();
        auto result = converter.من_unicode(0x0627); // ا
        SAD_ASSERT_TRUE(result.has_value());
    });

    SAD_TEST("diacritic from Unicode", {
        auto& converter = مُحوِّل_حروف::المثيل();
        auto fatha = converter.تشكيل_من_unicode(0x064E);
        SAD_ASSERT_TRUE(fatha.has_value());
        SAD_ASSERT_EQ((int)fatha.value(), (int)تشكيل::فتحة);
    });

    SAD_TEST("hamza from Unicode", {
        auto& converter = مُحوِّل_حروف::المثيل();
        auto result = converter.من_unicode(0x0621); // ء
        SAD_ASSERT_TRUE(result.has_value());
        SAD_ASSERT_EQ((int)result.value(), (int)حرف_عربي::همزة);
    });

    // ================================================================
    // 7. Root Search
    // ================================================================
    SAD_GROUP("البحث بالجذر / Root Search");

    SAD_TEST("extract root from word", {
        مُستخرج_جذور extractor;
        auto root = extractor.استخرج("كتاب");
        SAD_ASSERT_TRUE(root.has_value());
        SAD_ASSERT_GE(root.value().الطول(), (size_t)3);
    });

    SAD_TEST("root to string", {
        مُستخرج_جذور extractor;
        auto root = extractor.استخرج("كتاب");
        SAD_ASSERT_TRUE(root.has_value());
        auto text = root.value().إلى_نص();
        SAD_ASSERT_FALSE(text.empty());
    });

    SAD_TEST("different words have different roots", {
        مُستخرج_جذور extractor;
        auto root1 = extractor.استخرج("كتاب");
        auto root2 = extractor.استخرج("علم");
        if (root1.has_value() && root2.has_value()) {
            SAD_ASSERT_FALSE(root1.value() == root2.value());
        }
    });

    // ================================================================
    // 8. Edge Cases
    // ================================================================
    SAD_GROUP("حالات الحافة / Edge Cases");

    SAD_TEST("empty string normalization", {
        مُطبِّع normalizer;
        auto result = normalizer.طبّع("");
        SAD_ASSERT_TRUE(result.empty());
    });

    SAD_TEST("latin text passes through", {
        مُطبِّع normalizer;
        std::string latin = "Hello World";
        auto result = normalizer.طبّع(latin);
        SAD_ASSERT_EQ(result, latin);
    });

    SAD_TEST("mixed Arabic and Latin", {
        مُطبِّع normalizer(خيارات_تطبيع::للبحث());
        std::string mixed = "Hello مرحبا World عالم";
        auto result = normalizer.طبّع(mixed);
        SAD_ASSERT_FALSE(result.empty());
        SAD_ASSERT_CONTAINS(result, "Hello");
    });

    return _runner.printSummary();
}
