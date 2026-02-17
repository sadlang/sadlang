// ==============================================================================
// test_arabic_text.cpp - اختبارات دعم النصوص العربية
// Arabic Text Support Tests
// ==============================================================================

#include "catch.hpp"
#include "../include/text/arabic_text.h"

using namespace sad::graphics;

// ==============================================================================
// اختبارات فك ترميز UTF-8 / UTF-8 Decoding Tests
// ==============================================================================

TEST_CASE("UTF-8: فك ترميز ASCII / Decode ASCII", "[arabic][utf8]") {
    auto result = ArabicText::DecodeUTF8Char("A", 1);
    REQUIRE(result.codepoint == 0x41);
    REQUIRE(result.bytesUsed == 1);
}

TEST_CASE("UTF-8: فك ترميز حرف عربي / Decode Arabic char", "[arabic][utf8]") {
    // ب = U+0628 = 0xD8 0xA8 in UTF-8
    const char baa[] = "\xD8\xA8";
    auto result = ArabicText::DecodeUTF8Char(baa, 2);
    REQUIRE(result.codepoint == 0x0628);
    REQUIRE(result.bytesUsed == 2);
}

TEST_CASE("UTF-8: فك ترميز 3 بايت / Decode 3-byte char", "[arabic][utf8]") {
    // ﺑ = U+FE91 (Baa Initial) = 0xEF 0xBA 0x91
    const char baa_init[] = "\xEF\xBA\x91";
    auto result = ArabicText::DecodeUTF8Char(baa_init, 3);
    REQUIRE(result.codepoint == 0xFE91);
    REQUIRE(result.bytesUsed == 3);
}

TEST_CASE("UTF-8: فك ترميز نص كامل / Decode full string", "[arabic][utf8]") {
    // "Hi مرحبا" = 'H','i',' ', م, ر, ح, ب, ا
    std::string text = "Hi \xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7";
    auto codepoints = ArabicText::DecodeUTF8(text);
    REQUIRE(codepoints.size() == 8);
    REQUIRE(codepoints[0] == 'H');
    REQUIRE(codepoints[1] == 'i');
    REQUIRE(codepoints[2] == ' ');
    REQUIRE(codepoints[3] == 0x0645);  // م
    REQUIRE(codepoints[4] == 0x0631);  // ر
    REQUIRE(codepoints[5] == 0x062D);  // ح
    REQUIRE(codepoints[6] == 0x0628);  // ب
    REQUIRE(codepoints[7] == 0x0627);  // ا
}

TEST_CASE("UTF-8: ترميز وفك ترميز / Encode-Decode roundtrip", "[arabic][utf8]") {
    std::vector<u32> original = { 0x0633, 0x0644, 0x0627, 0x0645 }; // سلام
    std::string encoded = ArabicText::EncodeUTF8(original);
    auto decoded = ArabicText::DecodeUTF8(encoded);
    REQUIRE(decoded == original);
}

TEST_CASE("UTF-8: ترميز حرف واحد / Encode single char", "[arabic][utf8]") {
    SECTION("ASCII") {
        std::string enc = ArabicText::EncodeUTF8Char(0x41);
        REQUIRE(enc == "A");
    }
    SECTION("2-byte عربي") {
        std::string enc = ArabicText::EncodeUTF8Char(0x0628);  // ب
        REQUIRE(enc.size() == 2);
        auto dec = ArabicText::DecodeUTF8Char(enc.data(), enc.size());
        REQUIRE(dec.codepoint == 0x0628);
    }
    SECTION("3-byte عرض") {
        std::string enc = ArabicText::EncodeUTF8Char(0xFE91);  // ﺑ
        REQUIRE(enc.size() == 3);
        auto dec = ArabicText::DecodeUTF8Char(enc.data(), enc.size());
        REQUIRE(dec.codepoint == 0xFE91);
    }
}

TEST_CASE("UTF-8: بيانات فارغة / Empty data", "[arabic][utf8]") {
    auto result = ArabicText::DecodeUTF8Char(nullptr, 0);
    REQUIRE(result.bytesUsed == 0);
    
    auto codepoints = ArabicText::DecodeUTF8("");
    REQUIRE(codepoints.empty());
}

TEST_CASE("UTF-8: بايت غير صالح / Invalid byte", "[arabic][utf8]") {
    const char bad[] = "\xFF";
    auto result = ArabicText::DecodeUTF8Char(bad, 1);
    REQUIRE(result.codepoint == 0xFFFD);  // حرف بديل / Replacement character
    REQUIRE(result.bytesUsed == 1);
}

// ==============================================================================
// اختبارات تصنيف الأحرف / Character Classification Tests
// ==============================================================================

TEST_CASE("تصنيف: حرف عربي / IsArabic", "[arabic][classify]") {
    REQUIRE(ArabicText::IsArabic(0x0628));   // ب
    REQUIRE(ArabicText::IsArabic(0x0639));   // ع
    REQUIRE(ArabicText::IsArabic(0x0641));   // ف
    REQUIRE(ArabicText::IsArabic(0x0635));   // ص
    REQUIRE_FALSE(ArabicText::IsArabic('A'));
    REQUIRE_FALSE(ArabicText::IsArabic('5'));
    REQUIRE_FALSE(ArabicText::IsArabic(' '));
}

TEST_CASE("تصنيف: حرف RTL / IsRTL", "[arabic][classify]") {
    REQUIRE(ArabicText::IsRTL(0x0627));      // ا (Arabic)
    REQUIRE(ArabicText::IsRTL(0x05D0));      // א (Hebrew)
    REQUIRE_FALSE(ArabicText::IsRTL('A'));
    REQUIRE_FALSE(ArabicText::IsRTL('0'));
    REQUIRE_FALSE(ArabicText::IsRTL(' '));
}

TEST_CASE("تصنيف: حرف متصل / IsArabicJoining", "[arabic][classify]") {
    REQUIRE(ArabicText::IsArabicJoining(0x0628));   // ب - يتصل
    REQUIRE(ArabicText::IsArabicJoining(0x062A));   // ت - يتصل
    REQUIRE(ArabicText::IsArabicJoining(0x0627));   // ا - يتصل من اليمين فقط
    REQUIRE_FALSE(ArabicText::IsArabicJoining(0x0640));  // ـ تطويل
    REQUIRE_FALSE(ArabicText::IsArabicJoining('A'));
}

TEST_CASE("تصنيف: تشكيل / IsDiacritic", "[arabic][classify]") {
    REQUIRE(ArabicText::IsDiacritic(0x064B));  // فتحتان
    REQUIRE(ArabicText::IsDiacritic(0x064E));  // فتحة
    REQUIRE(ArabicText::IsDiacritic(0x064F));  // ضمة
    REQUIRE(ArabicText::IsDiacritic(0x0650));  // كسرة
    REQUIRE(ArabicText::IsDiacritic(0x0651));  // شدة
    REQUIRE(ArabicText::IsDiacritic(0x0652));  // سكون
    REQUIRE_FALSE(ArabicText::IsDiacritic(0x0628));  // ب ليس تشكيلاً
}

// ==============================================================================
// اختبارات تشكيل الحروف / Arabic Shaping Tests
// ==============================================================================

TEST_CASE("تشكيل: حرف منفصل / Isolated form", "[arabic][shaping]") {
    // حرف واحد منفصل / Single isolated character
    std::vector<u32> input = { 0x0628 };  // ب
    auto shaped = ArabicText::ShapeArabic(input);
    REQUIRE(shaped.size() == 1);
    REQUIRE(shaped[0] == 0xFE8F);  // ب منفصل / Isolated Baa
}

TEST_CASE("تشكيل: حرفان متصلان / Two connected letters", "[arabic][shaping]") {
    // بت = بداية + نهاية
    std::vector<u32> input = { 0x0628, 0x062A };  // ب + ت
    auto shaped = ArabicText::ShapeArabic(input);
    REQUIRE(shaped.size() == 2);
    REQUIRE(shaped[0] == 0xFE91);  // ب بداية / Initial Baa
    REQUIRE(shaped[1] == 0xFE96);  // ت نهاية / Final Taa
}

TEST_CASE("تشكيل: ثلاثة أحرف / Three letters", "[arabic][shaping]") {
    // بسم = بداية + وسط + نهاية
    std::vector<u32> input = { 0x0628, 0x0633, 0x0645 };  // ب + س + م
    auto shaped = ArabicText::ShapeArabic(input);
    REQUIRE(shaped.size() == 3);
    REQUIRE(shaped[0] == 0xFE91);  // ب بداية / Initial Baa
    REQUIRE(shaped[1] == 0xFEB4);  // س وسط / Medial Seen
    REQUIRE(shaped[2] == 0xFEE2);  // م نهاية / Final Meem
}

TEST_CASE("تشكيل: حرف لا يتصل من اليسار / Non-left-joining letter", "[arabic][shaping]") {
    // ار = ا لا يتصل من اليسار، ر لا يتصل من اليسار
    std::vector<u32> input = { 0x0627, 0x0631 };  // ا + ر
    auto shaped = ArabicText::ShapeArabic(input);
    REQUIRE(shaped.size() == 2);
    // ا لا يتصل من اليسار فيكون منفصلاً
    REQUIRE(shaped[0] == 0xFE8D);  // ا منفصل / Isolated Alef
    // ر: ا من قبلها لا يتصل من اليسار → ر منفصل أيضاً
    REQUIRE(shaped[1] == 0xFEAD);  // ر منفصل / Isolated Raa
}

TEST_CASE("تشكيل: مع تشكيل / With diacritics", "[arabic][shaping]") {
    // بَت = ب + فتحة + ت
    std::vector<u32> input = { 0x0628, 0x064E, 0x062A };
    auto shaped = ArabicText::ShapeArabic(input);
    REQUIRE(shaped.size() == 3);
    REQUIRE(shaped[0] == 0xFE91);  // ب بداية / Initial Baa
    REQUIRE(shaped[1] == 0x064E);  // فتحة (بقيت كما هي) / Fathah unchanged
    REQUIRE(shaped[2] == 0xFE96);  // ت نهاية / Final Taa
}

TEST_CASE("تشكيل: حروف غير عربية / Non-Arabic passthrough", "[arabic][shaping]") {
    std::vector<u32> input = { 'H', 'e', 'l', 'l', 'o' };
    auto shaped = ArabicText::ShapeArabic(input);
    REQUIRE(shaped == input);  // بقيت كما هي / Unchanged
}

TEST_CASE("تشكيل: GetArabicForm مباشر / Direct GetArabicForm", "[arabic][shaping]") {
    // ب
    REQUIRE(ArabicText::GetArabicForm(0x0628, ArabicForm::Isolated) == 0xFE8F);
    REQUIRE(ArabicText::GetArabicForm(0x0628, ArabicForm::Initial)  == 0xFE91);
    REQUIRE(ArabicText::GetArabicForm(0x0628, ArabicForm::Medial)   == 0xFE92);
    REQUIRE(ArabicText::GetArabicForm(0x0628, ArabicForm::Final)    == 0xFE90);
    
    // حرف غير موجود في الجدول يرجع كما هو
    REQUIRE(ArabicText::GetArabicForm('X', ArabicForm::Isolated) == 'X');
}

// ==============================================================================
// اختبارات اتجاه النص / Text Direction Tests
// ==============================================================================

TEST_CASE("اتجاه: نص عربي RTL / Arabic text is RTL", "[arabic][direction]") {
    std::vector<u32> arabic = { 0x0645, 0x0631, 0x062D, 0x0628, 0x0627 };  // مرحبا
    REQUIRE(ArabicText::DetectDirection(arabic) == TextDirection::RTL);
}

TEST_CASE("اتجاه: نص إنجليزي LTR / English text is LTR", "[arabic][direction]") {
    std::vector<u32> english = { 'H', 'e', 'l', 'l', 'o' };
    REQUIRE(ArabicText::DetectDirection(english) == TextDirection::LTR);
}

TEST_CASE("اتجاه: نص مختلط / Mixed text direction", "[arabic][direction]") {
    // "مرحبا Hello" - يبدأ بعربي
    std::vector<u32> mixed = { 0x0645, 0x0631, 0x062D, 0x0628, 0x0627, ' ', 'H', 'e', 'l', 'l', 'o' };
    REQUIRE(ArabicText::DetectDirection(mixed) == TextDirection::RTL);
    
    // "Hello مرحبا" - يبدأ بإنجليزي
    std::vector<u32> mixed2 = { 'H', 'e', 'l', 'l', 'o', ' ', 0x0645, 0x0631, 0x062D, 0x0628, 0x0627 };
    REQUIRE(ArabicText::DetectDirection(mixed2) == TextDirection::LTR);
}

TEST_CASE("اتجاه: أرقام وفراغات / Numbers and spaces", "[arabic][direction]") {
    // أرقام ومسافات بدون حروف قوية → افتراضي LTR
    std::vector<u32> nums = { '1', '2', '3', ' ', '+', ' ', '4', '5' };
    REQUIRE(ArabicText::DetectDirection(nums) == TextDirection::LTR);
}

// ==============================================================================
// اختبارات BiDi / BiDi Reordering Tests
// ==============================================================================

TEST_CASE("BiDi: نص عربي فقط / Arabic-only text", "[arabic][bidi]") {
    // مرحبا → عكس للعرض البصري
    std::vector<u32> input = { 0x0645, 0x0631, 0x062D, 0x0628, 0x0627 };
    auto reordered = ArabicText::ReorderBiDi(input);
    REQUIRE(reordered.size() == input.size());
    // النص الأساسي RTL → يُعكس
    REQUIRE(reordered[0] == 0x0627);  // ا
    REQUIRE(reordered[4] == 0x0645);  // م
}

TEST_CASE("BiDi: نص إنجليزي فقط / English-only text", "[arabic][bidi]") {
    std::vector<u32> input = { 'H', 'e', 'l', 'l', 'o' };
    auto reordered = ArabicText::ReorderBiDi(input);
    REQUIRE(reordered == input);  // لا تغيير / No change for LTR
}

// ==============================================================================
// اختبارات نطاقات Unicode / Unicode Range Tests
// ==============================================================================

TEST_CASE("نطاقات: العربية / Arabic ranges", "[arabic][ranges]") {
    auto ranges = ArabicText::GetArabicRanges();
    REQUIRE(ranges.size() == 4);
    REQUIRE(ranges[0].first == 0x0600);
    REQUIRE(ranges[0].last == 0x06FF);
}

TEST_CASE("نطاقات: الكاملة / Full ranges", "[arabic][ranges]") {
    auto ranges = ArabicText::GetFullRanges();
    REQUIRE(ranges.size() == 5);
    REQUIRE(ranges[0].first == 0x0020);  // ASCII
    REQUIRE(ranges[0].last == 0x007E);
}

// ==============================================================================
// اختبارات تكاملية / Integration Tests
// ==============================================================================

TEST_CASE("تكامل: فك → تشكيل → ترميز / Decode → Shape → Encode", "[arabic][integration]") {
    // "بسم" in UTF-8
    std::string input = "\xD8\xA8\xD8\xB3\xD9\x85";
    
    // فك الترميز / Decode
    auto codepoints = ArabicText::DecodeUTF8(input);
    REQUIRE(codepoints.size() == 3);
    REQUIRE(codepoints[0] == 0x0628);  // ب
    REQUIRE(codepoints[1] == 0x0633);  // س
    REQUIRE(codepoints[2] == 0x0645);  // م
    
    // تشكيل / Shape
    auto shaped = ArabicText::ShapeArabic(codepoints);
    REQUIRE(shaped.size() == 3);
    REQUIRE(shaped[0] == 0xFE91);  // ب بداية
    REQUIRE(shaped[1] == 0xFEB4);  // س وسط
    REQUIRE(shaped[2] == 0xFEE2);  // م نهاية
    
    // إعادة الترميز / Re-encode
    std::string reencoded = ArabicText::EncodeUTF8(shaped);
    REQUIRE_FALSE(reencoded.empty());
    
    // التأكد من أن إعادة الفك تُعيد نفس الأشكال / Verify decode gives same shapes
    auto redecoded = ArabicText::DecodeUTF8(reencoded);
    REQUIRE(redecoded == shaped);
}

TEST_CASE("تكامل: كلمة كاملة 'صاد' / Full word 'Sad'", "[arabic][integration]") {
    // ص ا د = Sad (the language name)
    std::vector<u32> sad = { 0x0635, 0x0627, 0x062F };
    
    auto shaped = ArabicText::ShapeArabic(sad);
    REQUIRE(shaped.size() == 3);

    // ص يتصل من اليسار لـ ا → شكل بداية
    REQUIRE(shaped[0] == 0xFEBB);  // ص بداية / Initial Sad
    // ا لا يتصل من اليسار → نهاية (بعد ص)
    REQUIRE(shaped[1] == 0xFE8E);  // ا نهاية / Final Alef
    // د بعد ا (الذي لا يتصل) → منفصل
    REQUIRE(shaped[2] == 0xFEA9);  // د منفصل / Isolated Dal
}

TEST_CASE("تكامل: جملة 'لغة ص' / Sentence 'لغة ص'", "[arabic][integration]") {
    // ل غ ة    ص
    std::vector<u32> phrase = { 0x0644, 0x063A, 0x0629, 0x0020, 0x0635 };
    
    auto shaped = ArabicText::ShapeArabic(phrase);
    REQUIRE(shaped.size() == 5);
    REQUIRE(shaped[0] == 0xFEDF);  // ل بداية / Initial Lam
    REQUIRE(shaped[1] == 0xFED0);  // غ وسط / Medial Ghain
    REQUIRE(shaped[2] == 0xFE94);  // ة نهاية / Final Taa Marbuta
    REQUIRE(shaped[3] == 0x0020);  // مسافة / Space unchanged
    REQUIRE(shaped[4] == 0xFEB9);  // ص منفصل / Isolated Sad
}
