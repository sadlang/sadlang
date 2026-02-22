/**
 * @file test_utils_modules_comprehensive.cpp
 * @brief (AR) اختبارات شاملة للأدوات المساعدة ونظام الوحدات
 *        (EN) Comprehensive Utils & Module System Tests
 * 
 * ════════════════════════════════════════════════════════════════════════
 *  هذا الملف يحتوي على ~95 اختبار يغطي:
 *  ───────────────────────────────────────
 *  1. StringUtils - أدوات النصوص (trim, split, join, replace, ...)
 *  2. StringUtils Arabic Support - دعم الأرقام والنصوص العربية
 *  3. UTF-8 Utilities - تحويلات الترميز (على ويندوز)
 *  4. Module System - نظام الوحدات (Module, ModuleMetadata, ExportedSymbol)
 *  5. DependencyGraph - رسم الاعتماديات (الترتيب الطوبولوجي، الدورات)
 *  6. ModuleCache - التخزين المؤقت للوحدات
 *  7. SearchPathManager - مدير مسارات البحث
 *  8. Edge Cases - حالات الحدود والأخطاء
 * ════════════════════════════════════════════════════════════════════════
 * 
 *  يغطي هذا الملف اثنين من أهم الأنظمة الفرعية في لغة ص:
 *  
 *  📌 نظام الأدوات المساعدة (Utils):
 *     - معالجة النصوص العربية والإنجليزية
 *     - تحويل الأرقام العربية-الهندية (٠١٢٣٤٥٦٧٨٩) إلى إنجليزية
 *     - دعم ترميز UTF-8 الكامل على جميع المنصات
 *     - دوال القص، التقسيم، الدمج، الاستبدال
 *  
 *  📌 نظام الوحدات (Module System):
 *     - إنشاء وإدارة الوحدات (الملفات المُحلَّلة)
 *     - تحليل الاعتماديات بين الوحدات
 *     - الترتيب الطوبولوجي لتحميل الوحدات بترتيب صحيح
 *     - كشف التبعيات الدائرية (Circular Dependencies)
 *     - التخزين المؤقت لتسريع إعادة التحميل
 *     - إدارة مسارات البحث عن الوحدات
 * 
 * @author Sad Language Development Team
 * @date 2026-02-17
 */

#include "sad_test_framework.h"

// ──────────────────────────────────────────────────────────────────────
// مكونات الأدوات المساعدة / Utility Components
// ──────────────────────────────────────────────────────────────────────
#include "string_utils.h"

// ──────────────────────────────────────────────────────────────────────
// مكونات نظام الوحدات / Module System Components
// ──────────────────────────────────────────────────────────────────────
#include "module.h"
#include "dependency_graph.h"
#include "module_cache.h"
#include "search_paths.h"

// ──────────────────────────────────────────────────────────────────────
// مكتبات C++ القياسية / Standard C++ Libraries
// ──────────────────────────────────────────────────────────────────────
#include <filesystem>
#include <algorithm>

using namespace Sad::Utils;
using namespace Sad::Modules;

// ══════════════════════════════════════════════════════════════════════
//  نقطة الدخول الرئيسية / Main Entry Point
// ══════════════════════════════════════════════════════════════════════
int main() {
    SAD_TEST_INIT();

    // ══════════════════════════════════════════════════════════════════
    //  المجموعة 1: أدوات النصوص - القص (Trim)
    //  ─────────────────────────────────────────────
    //  تختبر دالة trim() التي تزيل المسافات الزائدة من بداية
    //  ونهاية النص. هذه الدالة أساسية لتنظيف مدخلات المستخدم
    //  ومعالجة النصوص قبل التحليل المعجمي.
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("StringUtils.Trim / أدوات_النصوص.القص");

    SAD_TEST("SU01: قص المسافات من الطرفين - trim() مع مسافات عادية", {
        // (AR) نص محاط بمسافات - يجب إزالتها من البداية والنهاية
        // (EN) String surrounded by spaces - should be removed from start and end
        std::string input = "  hello  ";
        std::string result = StringUtils::trim(input);
        SAD_ASSERT_EQ(result, std::string("hello"));
    });

    SAD_TEST("SU02: قص نص بدون مسافات - trim() لا يغيّر النص", {
        // (AR) النص بدون مسافات زائدة - يجب أن يبقى كما هو بدون تغيير
        // (EN) String without extra spaces - should remain unchanged
        std::string input = "hello";
        std::string result = StringUtils::trim(input);
        SAD_ASSERT_EQ(result, std::string("hello"));
    });

    SAD_TEST("SU03: قص نص فارغ - trim() على نص فارغ", {
        // (AR) حالة حدية: النص الفارغ يجب أن يُرجع نصاً فارغاً
        // (EN) Edge case: empty string should return empty string
        std::string input = "";
        std::string result = StringUtils::trim(input);
        SAD_ASSERT_TRUE(result.empty());
    });

    SAD_TEST("SU04: قص مسافات فقط - trim() على مسافات بدون محتوى", {
        // (AR) نص يتكون من مسافات فقط - يجب أن يُرجع نصاً فارغاً
        // (EN) String of only spaces - should return empty string
        std::string input = "     ";
        std::string result = StringUtils::trim(input);
        SAD_ASSERT_TRUE(result.empty());
    });

    SAD_TEST("SU05: قص نص عربي محاط بمسافات", {
        // (AR) اختبار القص مع نص عربي - يجب أن يعمل بشكل صحيح مع UTF-8
        // (EN) Test trim with Arabic text - should work correctly with UTF-8
        std::string input = "  \xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7  "; // "  مرحبا  "
        std::string result = StringUtils::trim(input);
        SAD_ASSERT_EQ(result, std::string("\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7")); // "مرحبا"
    });

    SAD_TEST("SU06: trimLeft - إزالة المسافات من البداية فقط", {
        // (AR) يجب أن يُزيل المسافات من اليسار فقط ويحافظ على اليمين
        // (EN) Should remove spaces from left only, keeping right spaces
        std::string input = "  hello  ";
        std::string result = StringUtils::trimLeft(input);
        SAD_ASSERT_EQ(result, std::string("hello  "));
    });

    SAD_TEST("SU07: trimRight - إزالة المسافات من النهاية فقط", {
        // (AR) يجب أن يُزيل المسافات من اليمين فقط ويحافظ على اليسار
        // (EN) Should remove spaces from right only, keeping left spaces
        std::string input = "  hello  ";
        std::string result = StringUtils::trimRight(input);
        SAD_ASSERT_EQ(result, std::string("  hello"));
    });

    SAD_TEST("SU08: trim مع تابات وأسطر جديدة", {
        // (AR) القص يجب أن يشمل جميع أنواع المسافات البيضاء
        // (EN) Trim should handle all whitespace types
        std::string input = "\t\n hello \t\n";
        std::string result = StringUtils::trim(input);
        SAD_ASSERT_EQ(result, std::string("hello"));
    });

    // ══════════════════════════════════════════════════════════════════
    //  المجموعة 2: أدوات النصوص - التقسيم (Split)
    //  ─────────────────────────────────────────────
    //  تختبر دالة split() التي تقسم النص إلى أجزاء باستخدام
    //  فاصل محدد. مهمة جداً لتحليل بيانات CSV وقراءة الأوامر.
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("StringUtils.Split / أدوات_النصوص.التقسيم");

    SAD_TEST("SU09: تقسيم نص بسيط بفاصلة", {
        // (AR) تقسيم "a,b,c" بفاصلة يجب أن يعطي 3 أجزاء
        // (EN) Splitting "a,b,c" by comma should give 3 parts
        auto parts = StringUtils::split("a,b,c", ',');
        SAD_ASSERT_EQ((int)parts.size(), 3);
        SAD_ASSERT_EQ(parts[0], std::string("a"));
        SAD_ASSERT_EQ(parts[1], std::string("b"));
        SAD_ASSERT_EQ(parts[2], std::string("c"));
    });

    SAD_TEST("SU10: تقسيم بدون فاصل موجود في النص", {
        // (AR) إذا لم يكن الفاصل موجوداً، يرجع النص كاملاً في عنصر واحد
        // (EN) If delimiter not found, returns whole string in one element
        auto parts = StringUtils::split("hello", ',');
        SAD_ASSERT_EQ((int)parts.size(), 1);
        SAD_ASSERT_EQ(parts[0], std::string("hello"));
    });

    SAD_TEST("SU11: تقسيم نص فارغ", {
        // (AR) حالة حدية: النص الفارغ يجب أن يُرجع قائمة فارغة أو عنصر فارغ
        // (EN) Edge case: empty string should return empty list or one empty element
        auto parts = StringUtils::split("", ',');
        // ملاحظة: بعض التنفيذات تُرجع عنصراً فارغاً واحداً
        // Note: some implementations return one empty element
        SAD_ASSERT_TRUE(parts.size() <= 1);
    });

    SAD_TEST("SU12: تقسيم بمسافة كفاصل", {
        // (AR) التقسيم بمسافة - حالة استخدام شائعة لتحليل الأوامر
        // (EN) Split by space - common use case for command parsing
        auto parts = StringUtils::split("hello world test", ' ');
        SAD_ASSERT_EQ((int)parts.size(), 3);
        SAD_ASSERT_EQ(parts[0], std::string("hello"));
        SAD_ASSERT_EQ(parts[2], std::string("test"));
    });

    SAD_TEST("SU13: تقسيم نص عربي بفاصل", {
        // (AR) التقسيم يجب أن يعمل مع النصوص العربية (UTF-8)
        // (EN) Split should work with Arabic text (UTF-8)
        // "أحمد.محمد" بنقطة كفاصل
        std::string input = "\xD8\xA3\xD8\xAD\xD9\x85\xD8\xAF.\xD9\x85\xD8\xAD\xD9\x85\xD8\xAF";
        auto parts = StringUtils::split(input, '.');
        SAD_ASSERT_EQ((int)parts.size(), 2);
    });

    SAD_TEST("SU14: تقسيم بفواصل متتالية", {
        // (AR) فواصل متتالية تنتج عناصر فارغة بينها
        // (EN) Consecutive delimiters produce empty elements between them
        auto parts = StringUtils::split("a,,b", ',');
        SAD_ASSERT_GE((int)parts.size(), 2);
    });

    // ══════════════════════════════════════════════════════════════════
    //  المجموعة 3: أدوات النصوص - البحث والمقارنة
    //  ─────────────────────────────────────────────
    //  تختبر startsWith, endsWith, isBlank - دوال أساسية
    //  للتحقق من أنماط النصوص وتصنيف الملفات.
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("StringUtils.Matching / أدوات_النصوص.المطابقة");

    SAD_TEST("SU15: startsWith - بداية صحيحة", {
        // (AR) التحقق أن النص يبدأ ببادئة معينة
        // (EN) Verify string starts with specific prefix
        SAD_ASSERT_TRUE(StringUtils::startsWith("hello world", "hello"));
    });

    SAD_TEST("SU16: startsWith - بداية خاطئة", {
        // (AR) التحقق أن النص لا يبدأ ببادئة مختلفة
        // (EN) Verify string does not start with different prefix
        SAD_ASSERT_FALSE(StringUtils::startsWith("hello world", "world"));
    });

    SAD_TEST("SU17: startsWith - بادئة فارغة", {
        // (AR) أي نص يبدأ ببادئة فارغة (حالة حدية)
        // (EN) Any string starts with empty prefix (edge case)
        SAD_ASSERT_TRUE(StringUtils::startsWith("hello", ""));
    });

    SAD_TEST("SU18: endsWith - نهاية صحيحة", {
        // (AR) التحقق أن النص ينتهي بلاحقة صحيحة
        // (EN) Verify string ends with correct suffix
        SAD_ASSERT_TRUE(StringUtils::endsWith("hello.s", ".s"));
    });

    SAD_TEST("SU19: endsWith - نهاية خاطئة", {
        // (AR) التحقق أن النص لا ينتهي بلاحقة مختلفة
        // (EN) Verify string does not end with different suffix
        SAD_ASSERT_FALSE(StringUtils::endsWith("hello.s", ".cpp"));
    });

    SAD_TEST("SU20: endsWith - نص أقصر من اللاحقة", {
        // (AR) النص أقصر من اللاحقة - يجب أن يُرجع false
        // (EN) String shorter than suffix - should return false
        SAD_ASSERT_FALSE(StringUtils::endsWith("hi", "hello"));
    });

    SAD_TEST("SU21: isBlank - نص فارغ", {
        // (AR) النص الفارغ يُعتبر blank
        // (EN) Empty string is considered blank
        SAD_ASSERT_TRUE(StringUtils::isBlank(""));
    });

    SAD_TEST("SU22: isBlank - مسافات فقط", {
        // (AR) مسافات فقط تُعتبر blank
        // (EN) Only spaces are considered blank
        SAD_ASSERT_TRUE(StringUtils::isBlank("   "));
    });

    SAD_TEST("SU23: isBlank - نص غير فارغ", {
        // (AR) نص يحتوي على محتوى ليس blank
        // (EN) String with content is not blank
        SAD_ASSERT_FALSE(StringUtils::isBlank("hello"));
    });

    SAD_TEST("SU24: isBlank - نص عربي", {
        // (AR) نص عربي ليس blank حتى لو كان حرفاً واحداً
        // (EN) Arabic text is not blank even if it's a single character
        SAD_ASSERT_FALSE(StringUtils::isBlank("\xD8\xA3")); // أ
    });

    // ══════════════════════════════════════════════════════════════════
    //  المجموعة 4: أدوات النصوص - التحويل والاستبدال
    //  ─────────────────────────────────────────────
    //  تختبر toLower, toUpper, replace, join - دوال التحويل
    //  والمعالجة المتقدمة للنصوص.
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("StringUtils.Transform / أدوات_النصوص.التحويل");

    SAD_TEST("SU25: toLower - تحويل لأحرف صغيرة", {
        // (AR) تحويل كل الأحرف اللاتينية إلى صغيرة
        // (EN) Convert all Latin characters to lowercase
        std::string result = StringUtils::toLower("HELLO");
        SAD_ASSERT_EQ(result, std::string("hello"));
    });

    SAD_TEST("SU26: toLower - نص مختلط", {
        // (AR) النص المختلط يُحوَّل بالكامل
        // (EN) Mixed case is fully converted
        std::string result = StringUtils::toLower("HeLLo WoRLd");
        SAD_ASSERT_EQ(result, std::string("hello world"));
    });

    SAD_TEST("SU27: toLower - نص عربي لا يتأثر", {
        // (AR) الأحرف العربية لا تتأثر بتحويل الحالة
        // (EN) Arabic characters are not affected by case conversion
        std::string arabic = "\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7"; // مرحبا
        std::string result = StringUtils::toLower(arabic);
        SAD_ASSERT_EQ(result, arabic);
    });

    SAD_TEST("SU28: toUpper - تحويل لأحرف كبيرة", {
        // (AR) تحويل كل الأحرف اللاتينية إلى كبيرة
        // (EN) Convert all Latin characters to uppercase
        std::string result = StringUtils::toUpper("hello");
        SAD_ASSERT_EQ(result, std::string("HELLO"));
    });

    SAD_TEST("SU29: replace - استبدال بسيط", {
        // (AR) استبدال كلمة بأخرى في النص
        // (EN) Replace one word with another in the text
        std::string result = StringUtils::replace("hello world", "world", "test");
        SAD_ASSERT_EQ(result, std::string("hello test"));
    });

    SAD_TEST("SU30: replace - استبدال متعدد", {
        // (AR) استبدال جميع ظهورات الكلمة وليس الأولى فقط
        // (EN) Replace all occurrences, not just the first
        std::string result = StringUtils::replace("aaa", "a", "bb");
        SAD_ASSERT_EQ(result, std::string("bbbbbb"));
    });

    SAD_TEST("SU31: replace - لا يوجد تطابق", {
        // (AR) إذا لم يُوجد النص المراد استبداله، لا يتغيّر شيء
        // (EN) If substring not found, nothing changes
        std::string result = StringUtils::replace("hello", "xyz", "abc");
        SAD_ASSERT_EQ(result, std::string("hello"));
    });

    SAD_TEST("SU32: join - دمج نصوص بفاصل", {
        // (AR) دمج قائمة من النصوص بفاصلة
        // (EN) Join list of strings with comma
        std::vector<std::string> parts({"a", "b", "c"});
        std::string result = StringUtils::join(parts, ",");
        SAD_ASSERT_EQ(result, std::string("a,b,c"));
    });

    SAD_TEST("SU33: join - دمج عنصر واحد", {
        // (AR) دمج عنصر واحد يُرجع العنصر نفسه بدون فاصل
        // (EN) Joining single element returns it without delimiter
        std::vector<std::string> parts = {"solo"};
        std::string result = StringUtils::join(parts, ",");
        SAD_ASSERT_EQ(result, std::string("solo"));
    });

    SAD_TEST("SU34: join - دمج قائمة فارغة", {
        // (AR) دمج قائمة فارغة يُرجع نصاً فارغاً
        // (EN) Joining empty list returns empty string
        std::vector<std::string> parts;
        std::string result = StringUtils::join(parts, ",");
        SAD_ASSERT_TRUE(result.empty());
    });

    SAD_TEST("SU35: join - دمج بفاصل طويل", {
        // (AR) الفاصل يمكن أن يكون نصاً كاملاً وليس حرفاً واحداً
        // (EN) Delimiter can be a full string, not just a single character
        std::vector<std::string> parts({"hello", "world"});
        std::string result = StringUtils::join(parts, " -- ");
        SAD_ASSERT_EQ(result, std::string("hello -- world"));
    });

    // ══════════════════════════════════════════════════════════════════
    //  المجموعة 5: دعم الأرقام العربية-الهندية
    //  ─────────────────────────────────────────────
    //  تختبر تحويل الأرقام العربية-الهندية (٠١٢٣٤٥٦٧٨٩)
    //  إلى الأرقام الإنجليزية (0123456789). هذا ضروري لأن
    //  لغة ص تدعم كتابة الأرقام بالطريقة العربية.
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("StringUtils.ArabicDigits / أدوات_النصوص.الأرقام_العربية");

    SAD_TEST("SU36: تحويل الأرقام العربية ٠١٢٣٤٥٦٧٨٩", {
        // (AR) تحويل جميع الأرقام العربية-الهندية إلى الإنجليزية
        // (EN) Convert all Arabic-Indic digits to English
        std::string arabic = "\xD9\xA0\xD9\xA1\xD9\xA2\xD9\xA3\xD9\xA4\xD9\xA5\xD9\xA6\xD9\xA7\xD9\xA8\xD9\xA9"; // ٠١٢٣٤٥٦٧٨٩
        std::string result = StringUtils::convertArabicDigitsToEnglish(arabic);
        SAD_ASSERT_EQ(result, std::string("0123456789"));
    });

    SAD_TEST("SU37: تحويل نص مختلط عربي وإنجليزي", {
        // (AR) النص المختلط: الأرقام العربية تُحوَّل والباقي يبقى كما هو
        // (EN) Mixed text: Arabic digits converted, rest stays the same
        // "العدد ٥٣" -> "العدد 53"
        std::string input = "\xD8\xA7\xD9\x84\xD8\xB9\xD8\xAF\xD8\xAF \xD9\xA5\xD9\xA3";
        std::string result = StringUtils::convertArabicDigitsToEnglish(input);
        SAD_ASSERT_CONTAINS(result, "53");
    });

    SAD_TEST("SU38: تحويل نص بدون أرقام عربية - لا تغيير", {
        // (AR) نص يحتوي فقط على أرقام إنجليزية - لا يتغيّر
        // (EN) Text with only English digits - unchanged
        std::string input = "hello 123";
        std::string result = StringUtils::convertArabicDigitsToEnglish(input);
        SAD_ASSERT_EQ(result, std::string("hello 123"));
    });

    SAD_TEST("SU39: تحويل نص فارغ - لا خطأ", {
        // (AR) النص الفارغ يجب أن يعمل بدون أخطاء
        // (EN) Empty string should work without errors
        std::string result = StringUtils::convertArabicDigitsToEnglish("");
        SAD_ASSERT_TRUE(result.empty());
    });

    SAD_TEST("SU40: فحص isArabicDigit مع رقم عربي", {
        // (AR) الفحص يجب أن يتعرف على بايتات الأرقام العربية
        // (EN) Check should recognize Arabic digit bytes
        // ملاحظة: الأرقام العربية-الهندية تُمثَّل ببايتين في UTF-8
        // Note: Arabic-Indic digits are represented by 2 bytes in UTF-8
        // ٥ = 0xD9 0xA5
        // isArabicDigit يتحقق من البايت الثاني بعد 0xD9
        SAD_ASSERT_TRUE(StringUtils::isArabicDigit(0xA5)); // بايت ثاني من ٥
    });

    SAD_TEST("SU41: فحص isDigit مع رقم إنجليزي", {
        // (AR) isDigit يجب أن يتعرف على الأرقام الإنجليزية
        // (EN) isDigit should recognize English digits
        SAD_ASSERT_TRUE(StringUtils::isDigit('5'));
        SAD_ASSERT_TRUE(StringUtils::isDigit('0'));
        SAD_ASSERT_TRUE(StringUtils::isDigit('9'));
    });

    SAD_TEST("SU42: فحص isDigit مع غير رقم", {
        // (AR) isDigit يجب أن يُرجع false لغير الأرقام
        // (EN) isDigit should return false for non-digits
        SAD_ASSERT_FALSE(StringUtils::isDigit('a'));
        SAD_ASSERT_FALSE(StringUtils::isDigit(' '));
    });

    // ══════════════════════════════════════════════════════════════════
    //  المجموعة 6: حالات الحدود لأدوات النصوص
    //  ─────────────────────────────────────────────
    //  اختبارات خاصة لحالات الحدود والمدخلات غير العادية.
    //  هذه الحالات مهمة لضمان stability النظام.
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("StringUtils.EdgeCases / أدوات_النصوص.حالات_الحدود");

    SAD_TEST("SU43: trim على نص طويل جداً", {
        // (AR) نص طويل مع مسافات - يجب أن يعمل بكفاءة
        // (EN) Long string with spaces - should work efficiently
        std::string input(1000, ' ');
        input += "content";
        input += std::string(1000, ' ');
        std::string result = StringUtils::trim(input);
        SAD_ASSERT_EQ(result, std::string("content"));
    });

    SAD_TEST("SU44: split على نص بفاصل واحد فقط", {
        // (AR) فاصل واحد فقط بدون محتوى - يجب أن يُرجع عنصرين فارغين
        // (EN) Single delimiter only - should return two empty elements
        auto parts = StringUtils::split(",", ',');
        SAD_ASSERT_GE((int)parts.size(), 1);
    });

    SAD_TEST("SU45: replace بنص فارغ", {
        // (AR) استبدال النص المصدر بنص فارغ (حذف)
        // (EN) Replace source text with empty string (deletion)
        std::string result = StringUtils::replace("hello world", "world", "");
        SAD_ASSERT_EQ(result, std::string("hello "));
    });

    SAD_TEST("SU46: startsWith مع نص مطابق تماماً", {
        // (AR) النص مطابق تماماً للبادئة - يجب أن يُرجع true
        // (EN) String exactly matches prefix - should return true
        SAD_ASSERT_TRUE(StringUtils::startsWith("hello", "hello"));
    });

    SAD_TEST("SU47: endsWith مع نص مطابق تماماً", {
        // (AR) النص مطابق تماماً للاحقة - يجب أن يُرجع true
        // (EN) String exactly matches suffix - should return true
        SAD_ASSERT_TRUE(StringUtils::endsWith("hello", "hello"));
    });

    SAD_TEST("SU48: toLower على نص فارغ", {
        // (AR) تحويل حالة نص فارغ - يجب أن يُرجع نصاً فارغاً
        // (EN) Case conversion on empty string - should return empty string
        std::string result = StringUtils::toLower("");
        SAD_ASSERT_TRUE(result.empty());
    });

    SAD_TEST("SU49: toUpper على نص فارغ", {
        // (AR) نفس الاختبار السابق ولكن لـ toUpper
        // (EN) Same test as above but for toUpper
        std::string result = StringUtils::toUpper("");
        SAD_ASSERT_TRUE(result.empty());
    });

    SAD_TEST("SU50: replace مع from فارغ", {
        // (AR) استبدال بنص مصدر فارغ - سلوك محدد
        // (EN) Replace with empty source text - defined behavior
        // ملاحظة: هذا سلوك حدي - لا يجب أن يُسبب crash
        // Note: This is boundary behavior - should not crash
        SAD_ASSERT_NO_THROW({
            StringUtils::replace("hello", "", "x");
        });
    });

    // ══════════════════════════════════════════════════════════════════
    //  المجموعة 7: نظام الوحدات - إنشاء الوحدات
    //  ─────────────────────────────────────────────
    //  تختبر إنشاء وحدات (Module) جديدة والتحقق من خصائصها
    //  الأساسية: الاسم، المسار، البيانات الوصفية.
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Module.Creation / الوحدات.الإنشاء");

    SAD_TEST("MO01: إنشاء وحدة بالاسم فقط", {
        // (AR) إنشاء وحدة بتمرير الاسم فقط
        // (EN) Create module with name only
        Module mod("test_module");
        SAD_ASSERT_EQ(mod.getName(), std::string("test_module"));
    });

    SAD_TEST("MO02: إنشاء وحدة بالاسم والمسار", {
        // (AR) إنشاء وحدة بتمرير الاسم ومسار الملف
        // (EN) Create module with name and file path
        Module mod("mymod", "/path/to/mymod.s");
        SAD_ASSERT_EQ(mod.getName(), std::string("mymod"));
        SAD_ASSERT_EQ(mod.getFilePath(), std::string("/path/to/mymod.s"));
    });

    SAD_TEST("MO03: إنشاء وحدة بمسار فارغ", {
        // (AR) الوحدة بمسار فارغ - تعمل ولكن بدون ملف مربوط
        // (EN) Module with empty path - works but no bound file
        Module mod("in_memory", "");
        SAD_ASSERT_EQ(mod.getName(), std::string("in_memory"));
        SAD_ASSERT_TRUE(mod.getFilePath().empty());
    });

    SAD_TEST("MO04: إنشاء وحدة باسم عربي", {
        // (AR) أسماء الوحدات بالعربية مدعومة
        // (EN) Arabic module names are supported
        Module mod("\xD8\xAD\xD8\xB3\xD8\xA7\xD8\xA8"); // حساب
        SAD_ASSERT_EQ(mod.getName(), std::string("\xD8\xAD\xD8\xB3\xD8\xA7\xD8\xA8"));
    });

    SAD_TEST("MO05: البيانات الوصفية - الحصول على Metadata", {
        // (AR) التحقق من أن البيانات الوصفية تحتوي على الاسم الصحيح
        // (EN) Verify metadata contains correct name
        Module mod("meta_test", "/some/path.s");
        const auto& meta = mod.getMetadata();
        SAD_ASSERT_EQ(meta.name, std::string("meta_test"));
        SAD_ASSERT_EQ(meta.filePath, std::string("/some/path.s"));
    });

    // ══════════════════════════════════════════════════════════════════
    //  المجموعة 8: نظام الوحدات - الاعتماديات والتصدير
    //  ─────────────────────────────────────────────
    //  تختبر إضافة اعتماديات (imports) ورموز مُصدَّرة (exports)
    //  للوحدة. جوهر نظام الوحدات يعتمد على هذه العمليات.
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Module.Dependencies / الوحدات.الاعتماديات");

    SAD_TEST("MO06: إضافة اعتمادية واحدة", {
        // (AR) إضافة وحدة كاعتمادية ثم التحقق من وجودها
        // (EN) Add a module as dependency then verify it exists
        Module mod("main");
        mod.addDependency("utils");
        auto deps = mod.getDependencies();
        SAD_ASSERT_EQ((int)deps.size(), 1);
        SAD_ASSERT_EQ(deps[0], std::string("utils"));
    });

    SAD_TEST("MO07: إضافة عدة اعتماديات", {
        // (AR) إضافة عدة وحدات كاعتماديات
        // (EN) Add multiple modules as dependencies
        Module mod("app");
        mod.addDependency("math");
        mod.addDependency("io");
        mod.addDependency("string");
        auto deps = mod.getDependencies();
        SAD_ASSERT_EQ((int)deps.size(), 3);
    });

    SAD_TEST("MO08: وحدة بدون اعتماديات", {
        // (AR) وحدة جديدة بدون اعتماديات - القائمة فارغة
        // (EN) New module without dependencies - empty list
        Module mod("standalone");
        auto deps = mod.getDependencies();
        SAD_ASSERT_TRUE(deps.empty());
    });

    SAD_TEST("MO09: إضافة رمز مُصدَّر - دالة", {
        // (AR) تصدير دالة من الوحدة
        // (EN) Export a function from the module
        Module mod("lib");
        mod.addExport("calculate", ExportedSymbol::Type::FUNCTION, nullptr);
        const auto& exports = mod.getExports();
        SAD_ASSERT_EQ((int)exports.size(), 1);
        SAD_ASSERT_TRUE(exports.count("calculate") > 0);
    });

    SAD_TEST("MO10: إضافة رموز مُصدَّرة متعددة", {
        // (AR) تصدير عدة رموز مختلفة الأنواع
        // (EN) Export multiple symbols of different types
        Module mod("multi_lib");
        mod.addExport("func1", ExportedSymbol::Type::FUNCTION, nullptr);
        mod.addExport("MyClass", ExportedSymbol::Type::CLASS, nullptr);
        mod.addExport("VERSION", ExportedSymbol::Type::CONSTANT, nullptr);
        const auto& exports = mod.getExports();
        SAD_ASSERT_EQ((int)exports.size(), 3);
    });

    SAD_TEST("MO11: البحث عن رمز مُصدَّر موجود", {
        // (AR) البحث عن رمز مُصدَّر بالاسم - يجب أن يُوجد
        // (EN) Search for exported symbol by name - should be found
        Module mod("lookup_lib");
        mod.addExport("myFunc", ExportedSymbol::Type::FUNCTION, nullptr);
        auto* sym = mod.findExport("myFunc");
        SAD_ASSERT_NOT_NULL(sym);
        SAD_ASSERT_EQ(sym->name, std::string("myFunc"));
    });

    SAD_TEST("MO12: البحث عن رمز مُصدَّر غير موجود", {
        // (AR) البحث عن رمز غير مُصدَّر - يجب أن يُرجع nullptr
        // (EN) Search for non-exported symbol - should return nullptr
        Module mod("small_lib");
        auto* sym = mod.findExport("nonexistent");
        SAD_ASSERT_TRUE(sym == nullptr);
    });

    // ══════════════════════════════════════════════════════════════════
    //  المجموعة 9: رسم الاعتماديات - الأساسيات
    //  ─────────────────────────────────────────────
    //  تختبر DependencyGraph - البنية الأساسية لتتبع وحل
    //  الاعتماديات بين الوحدات. يستخدم الترتيب الطوبولوجي.
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("DependencyGraph.Basics / رسم_الاعتماديات.الأساسيات");

    SAD_TEST("DG01: إنشاء رسم فارغ", {
        // (AR) رسم الاعتماديات الجديد يكون فارغاً
        // (EN) New dependency graph should be empty
        DependencyGraph graph;
        auto order = graph.resolveLoadOrder();
        SAD_ASSERT_TRUE(order.empty());
    });

    SAD_TEST("DG02: إضافة وحدة واحدة بدون اعتماديات", {
        // (AR) وحدة واحدة بدون اعتماديات - تُرجع الوحدة نفسها
        // (EN) Single module without deps - returns itself
        DependencyGraph graph;
        graph.addModule("main", {});
        auto order = graph.resolveLoadOrder();
        SAD_ASSERT_EQ((int)order.size(), 1);
        SAD_ASSERT_EQ(order[0], std::string("main"));
    });

    SAD_TEST("DG03: اعتمادية خطية بسيطة A -> B", {
        // (AR) A يعتمد على B، لذا B يُحمَّل أولاً
        // (EN) A depends on B, so B loads first
        DependencyGraph graph;
        graph.addModule("A", {"B"});
        graph.addModule("B", {});
        auto order = graph.resolveLoadOrder();
        SAD_ASSERT_EQ((int)order.size(), 2);
        // B يجب أن يأتي قبل A
        // B should come before A
        int posA = -1; int posB = -1;
        for (int i = 0; i < (int)order.size(); i++) {
            if (order[i] == "A") posA = i;
            if (order[i] == "B") posB = i;
        }
        SAD_ASSERT_TRUE(posB < posA);
    });

    SAD_TEST("DG04: ثلاث وحدات A -> B -> C", {
        // (AR) سلسلة اعتماديات: يُحمَّل C أولاً ثم B ثم A
        // (EN) Chain of deps: C loads first, then B, then A
        DependencyGraph graph;
        graph.addModule("A", {"B"});
        graph.addModule("B", {"C"});
        graph.addModule("C", {});
        auto order = graph.resolveLoadOrder();
        SAD_ASSERT_EQ((int)order.size(), 3);
        // ترتيب التحميل: C, B, A
        int posA = -1; int posB = -1; int posC = -1;
        for (int i = 0; i < (int)order.size(); i++) {
            if (order[i] == "A") posA = i;
            if (order[i] == "B") posB = i;
            if (order[i] == "C") posC = i;
        }
        SAD_ASSERT_TRUE(posC < posB);
        SAD_ASSERT_TRUE(posB < posA);
    });

    SAD_TEST("DG05: اعتماديات متعددة A -> {B, C}", {
        // (AR) وحدة تعتمد على وحدتين - كلاهما يُحمَّل قبلها
        // (EN) Module depends on two - both load before it
        DependencyGraph graph;
        graph.addModule("A", {"B", "C"});
        graph.addModule("B", {});
        graph.addModule("C", {});
        auto order = graph.resolveLoadOrder();
        SAD_ASSERT_EQ((int)order.size(), 3);
        int posA = -1;
        for (int i = 0; i < (int)order.size(); i++) {
            if (order[i] == "A") posA = i;
        }
        // A يجب أن يكون آخر المُحمَّلين
        // A should be loaded last
        SAD_ASSERT_EQ(posA, 2);
    });

    // ══════════════════════════════════════════════════════════════════
    //  المجموعة 10: كشف التبعيات الدائرية
    //  ─────────────────────────────────────────────
    //  من أهم اختبارات نظام الوحدات - كشف الدورات الدائرية
    //  التي تمنع التحميل الصحيح للوحدات (مثل A يستورد B
    //  و B يستورد A).
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("DependencyGraph.CircularDetection / رسم_الاعتماديات.الدورات");

    SAD_TEST("DG06: لا توجد دورة في رسم بسيط", {
        // (AR) رسم خطي بدون دورات - يجب أن يُرجع false
        // (EN) Linear graph without cycles - should return false
        DependencyGraph graph;
        graph.addModule("A", {"B"});
        graph.addModule("B", {});
        SAD_ASSERT_FALSE(graph.hasCircularDependency());
    });

    SAD_TEST("DG07: دورة مباشرة A <-> B", {
        // (AR) دورة مباشرة: A يعتمد على B و B يعتمد على A
        // (EN) Direct cycle: A depends on B and B depends on A
        DependencyGraph graph;
        graph.addModule("A", {"B"});
        graph.addModule("B", {"A"});
        SAD_ASSERT_TRUE(graph.hasCircularDependency());
    });

    SAD_TEST("DG08: دورة ذاتية - وحدة تعتمد على نفسها", {
        // (AR) وحدة تستورد نفسها - دورة ذاتية
        // (EN) Module imports itself - self-cycle
        DependencyGraph graph;
        graph.addModule("A", {"A"});
        SAD_ASSERT_TRUE(graph.hasCircularDependency());
    });

    SAD_TEST("DG09: دورة غير مباشرة A -> B -> C -> A", {
        // (AR) دورة من 3 وحدات
        // (EN) Cycle of 3 modules
        DependencyGraph graph;
        graph.addModule("A", {"B"});
        graph.addModule("B", {"C"});
        graph.addModule("C", {"A"});
        SAD_ASSERT_TRUE(graph.hasCircularDependency());
    });

    SAD_TEST("DG10: الحصول على مسار الدورة", {
        // (AR) الحصول على قائمة الوحدات في المسار الدائري
        // (EN) Get list of modules in circular path
        DependencyGraph graph;
        graph.addModule("A", {"B"});
        graph.addModule("B", {"A"});
        graph.hasCircularDependency(); // يجب استدعاؤها أولاً
        auto path = graph.getCircularPath();
        // يجب أن يحتوي على A و B
        SAD_ASSERT_TRUE(path.size() >= 2);
    });

    // ══════════════════════════════════════════════════════════════════
    //  المجموعة 11: رسم الاعتماديات - العمليات المتقدمة
    //  ─────────────────────────────────────────────
    //  اختبارات متقدمة لنظام الاعتماديات: تتبع حالة التحميل،
    //  الحصول على dependents، مسح الرسم.
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("DependencyGraph.Advanced / رسم_الاعتماديات.متقدم");

    SAD_TEST("DG11: وضع علامة تحميل على وحدة", {
        // (AR) بعد markAsLoaded، isLoaded يُرجع true
        // (EN) After markAsLoaded, isLoaded returns true
        DependencyGraph graph;
        graph.addModule("A", {});
        SAD_ASSERT_FALSE(graph.isLoaded("A"));
        graph.markAsLoaded("A");
        SAD_ASSERT_TRUE(graph.isLoaded("A"));
    });

    SAD_TEST("DG12: الحصول على اعتماديات وحدة", {
        // (AR) getDependencies يُرجع القائمة الصحيحة
        // (EN) getDependencies returns correct list
        DependencyGraph graph;
        graph.addModule("app", {"math", "io"});
        graph.addModule("math", {});
        graph.addModule("io", {});
        auto deps = graph.getDependencies("app");
        SAD_ASSERT_EQ((int)deps.size(), 2);
    });

    SAD_TEST("DG13: الحصول على الوحدات المُعتمِدة (dependents)", {
        // (AR) getDependents تُرجع الوحدات التي تعتمد على وحدة معينة
        // (EN) getDependents returns modules that depend on a specific module
        DependencyGraph graph;
        graph.addModule("app1", {"utils"});
        graph.addModule("app2", {"utils"});
        graph.addModule("utils", {});
        auto dependents = graph.getDependents("utils");
        SAD_ASSERT_EQ((int)dependents.size(), 2);
    });

    SAD_TEST("DG14: مسح الرسم بالكامل", {
        // (AR) بعد clear()، الرسم يكون فارغاً
        // (EN) After clear(), graph should be empty
        DependencyGraph graph;
        graph.addModule("A", {"B"});
        graph.addModule("B", {});
        graph.clear();
        auto order = graph.resolveLoadOrder();
        SAD_ASSERT_TRUE(order.empty());
    });

    SAD_TEST("DG15: رسم معقد - شجرة ربّاعية", {
        // (AR) رسم على شكل شجرة: root -> {A, B, C, D} -> ...
        // (EN) Tree-shaped graph: root -> {A, B, C, D} -> ...
        DependencyGraph graph;
        graph.addModule("root", {"A", "B", "C", "D"});
        graph.addModule("A", {"shared"});
        graph.addModule("B", {"shared"});
        graph.addModule("C", {});
        graph.addModule("D", {"C"});
        graph.addModule("shared", {});
        
        SAD_ASSERT_FALSE(graph.hasCircularDependency());
        auto order = graph.resolveLoadOrder();
        SAD_ASSERT_EQ((int)order.size(), 6);
        
        // shared يجب أن يُحمَّل قبل A و B
        int posShared = -1; int posRoot = -1;
        for (int i = 0; i < (int)order.size(); i++) {
            if (order[i] == "shared") posShared = i;
            if (order[i] == "root") posRoot = i;
        }
        SAD_ASSERT_TRUE(posShared < posRoot);
    });

    // ══════════════════════════════════════════════════════════════════
    //  المجموعة 12: التخزين المؤقت للوحدات (ModuleCache)
    //  ─────────────────────────────────────────────
    //  تختبر نظام الكاش الذي يُخزّن الوحدات المُحلَّلة
    //  لتجنب إعادة تحليلها عند كل استيراد.
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("ModuleCache / التخزين_المؤقت");

    SAD_TEST("MC01: الحصول على instance - Singleton", {
        // (AR) ModuleCache يجب أن يكون Singleton
        // (EN) ModuleCache should be Singleton
        auto& cache1 = ModuleCache::getInstance();
        auto& cache2 = ModuleCache::getInstance();
        SAD_ASSERT_EQ(&cache1, &cache2);
    });

    SAD_TEST("MC02: إضافة واسترجاع وحدة من الكاش", {
        // (AR) put ثم get يجب أن يُرجع الوحدة
        // (EN) put then get should return the module
        auto& cache = ModuleCache::getInstance();
        cache.clear();
        
        auto mod = std::make_unique<Module>("cached_mod", "/path.s");
        cache.put("cached_mod", std::move(mod));
        
        Module* retrieved = cache.get("cached_mod");
        SAD_ASSERT_NOT_NULL(retrieved);
        SAD_ASSERT_EQ(retrieved->getName(), std::string("cached_mod"));
    });

    SAD_TEST("MC03: التحقق من وجود وحدة - has()", {
        // (AR) has() يُرجع true للوحدة المُخزنَّة
        // (EN) has() returns true for cached module
        auto& cache = ModuleCache::getInstance();
        cache.clear();
        
        cache.put("exists", std::make_unique<Module>("exists"));
        SAD_ASSERT_TRUE(cache.has("exists"));
        SAD_ASSERT_FALSE(cache.has("not_exists"));
    });

    SAD_TEST("MC04: استرجاع وحدة غير موجودة - nullptr", {
        // (AR) get() لوحدة غير موجودة يُرجع nullptr
        // (EN) get() for non-existent module returns nullptr
        auto& cache = ModuleCache::getInstance();
        cache.clear();
        
        Module* mod = cache.get("nonexistent");
        SAD_ASSERT_TRUE(mod == nullptr);
    });

    SAD_TEST("MC05: مسح الكاش - clear()", {
        // (AR) بعد clear() جميع الوحدات تُحذف
        // (EN) After clear() all modules are removed
        auto& cache = ModuleCache::getInstance();
        cache.put("to_clear", std::make_unique<Module>("to_clear"));
        SAD_ASSERT_TRUE(cache.has("to_clear"));
        
        cache.clear();
        SAD_ASSERT_FALSE(cache.has("to_clear"));
    });

    SAD_TEST("MC06: إعادة تخزين وحدة بنفس الاسم", {
        // (AR) إضافة وحدة بنفس اسم وحدة موجودة - تُستبدل
        // (EN) Adding module with same name as existing - replaces it
        auto& cache = ModuleCache::getInstance();
        cache.clear();
        
        cache.put("overwrite", std::make_unique<Module>("overwrite", "/path1.s"));
        cache.put("overwrite", std::make_unique<Module>("overwrite", "/path2.s"));
        
        Module* mod = cache.get("overwrite");
        SAD_ASSERT_NOT_NULL(mod);
        SAD_ASSERT_EQ(mod->getFilePath(), std::string("/path2.s"));
    });

    // ══════════════════════════════════════════════════════════════════
    //  المجموعة 13: مدير مسارات البحث (SearchPathManager)
    //  ─────────────────────────────────────────────
    //  تختبر SearchPathManager الذي يُدير قائمة المسارات
    //  التي يبحث فيها النظام عن الوحدات المُستوردة.
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("SearchPathManager / مسارات_البحث");

    SAD_TEST("SP01: الحصول على instance - Singleton", {
        // (AR) SearchPathManager يجب أن يكون Singleton
        // (EN) SearchPathManager should be Singleton
        auto& spm1 = SearchPathManager::getInstance();
        auto& spm2 = SearchPathManager::getInstance();
        SAD_ASSERT_EQ(&spm1, &spm2);
    });

    SAD_TEST("SP02: إضافة مسار بحث", {
        // (AR) إضافة مسار - لا يُسبب خطأ
        // (EN) Add path - should not cause error
        auto& spm = SearchPathManager::getInstance();
        SAD_ASSERT_NO_THROW({
            spm.addPath("/test/path", 50);
        });
    });

    SAD_TEST("SP03: إضافة مسارات متعددة بأولويات مختلفة", {
        // (AR) مسارات بأولويات مختلفة - الأعلى يُفحص أولاً
        // (EN) Paths with different priorities - higher checked first
        auto& spm = SearchPathManager::getInstance();
        SAD_ASSERT_NO_THROW({
            spm.addPath("/high/priority", 100);
            spm.addPath("/low/priority", 10);
        });
    });

    // ══════════════════════════════════════════════════════════════════
    //  المجموعة 14: ExportedSymbol & ModuleMetadata
    //  ─────────────────────────────────────────────
    //  اختبارات تفصيلية لبُنى البيانات الأساسية في نظام الوحدات.
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Module.Structures / الوحدات.البِنى");

    SAD_TEST("ES01: نوع ExportedSymbol - FUNCTION", {
        // (AR) التحقق من نوع الرمز المُصدَّر
        // (EN) Verify exported symbol type
        ExportedSymbol sym;
        sym.name = "myFunc";
        sym.type = ExportedSymbol::Type::FUNCTION;
        SAD_ASSERT_EQ(sym.name, std::string("myFunc"));
        SAD_ASSERT_EQ((int)sym.type, (int)ExportedSymbol::Type::FUNCTION);
    });

    SAD_TEST("ES02: نوع ExportedSymbol - CLASS", {
        // (AR) تصدير صنف
        // (EN) Export a class
        ExportedSymbol sym;
        sym.name = "MyClass";
        sym.type = ExportedSymbol::Type::CLASS;
        SAD_ASSERT_EQ((int)sym.type, (int)ExportedSymbol::Type::CLASS);
    });

    SAD_TEST("ES03: نوع ExportedSymbol - VARIABLE", {
        // (AR) تصدير متغير
        // (EN) Export a variable
        ExportedSymbol sym;
        sym.name = "count";
        sym.type = ExportedSymbol::Type::VARIABLE;
        SAD_ASSERT_EQ((int)sym.type, (int)ExportedSymbol::Type::VARIABLE);
    });

    SAD_TEST("ES04: نوع ExportedSymbol - CONSTANT", {
        // (AR) تصدير ثابت
        // (EN) Export a constant
        ExportedSymbol sym;
        sym.name = "PI";
        sym.type = ExportedSymbol::Type::CONSTANT;
        SAD_ASSERT_EQ((int)sym.type, (int)ExportedSymbol::Type::CONSTANT);
    });

    SAD_TEST("MM01: ModuleMetadata - بيانات وصفية افتراضية", {
        // (AR) البيانات الوصفية الافتراضية
        // (EN) Default metadata values
        ModuleMetadata meta;
        meta.name = "test";
        meta.version = "1.0.0";
        SAD_ASSERT_EQ(meta.name, std::string("test"));
        SAD_ASSERT_EQ(meta.version, std::string("1.0.0"));
    });

    SAD_TEST("MM02: ModuleMetadata - معلومات الملف", {
        // (AR) التحقق من تخزين معلومات الملف بشكل صحيح
        // (EN) Verify file information stored correctly
        ModuleMetadata meta;
        meta.name = "filemod";
        meta.filePath = "/path/to/file.s";
        meta.fileSize = 1024;
        SAD_ASSERT_EQ(meta.filePath, std::string("/path/to/file.s"));
        SAD_ASSERT_EQ((int)meta.fileSize, 1024);
    });

    SAD_TEST("MM03: ModuleMetadata - toString()", {
        // (AR) toString يجب أن يُعيد نصاً يحتوي على الاسم
        // (EN) toString should return string containing name
        ModuleMetadata meta;
        meta.name = "display_test";
        std::string str = meta.toString();
        SAD_ASSERT_CONTAINS(str, "display_test");
    });

    // ══════════════════════════════════════════════════════════════════
    //  المجموعة 15: حالات الحدود المتقدمة
    //  ─────────────────────────────────────────────
    //  اختبارات لحالات خاصة ونادرة قد تحدث في الاستخدام
    //  الفعلي للنظام. تضمن الاستقرار تحت الضغط.
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("EdgeCases.Advanced / حالات_حدود_متقدمة");

    SAD_TEST("EC01: Module - AST فارغ", {
        // (AR) وحدة فارغة بدون AST - يجب أن تكون القائمة فارغة
        // (EN) Empty module without AST - list should be empty
        Module mod("empty_ast");
        SAD_ASSERT_TRUE(mod.getAST().empty());
    });

    SAD_TEST("EC02: Module - exports فارغة", {
        // (AR) وحدة بدون exports - القائمة فارغة
        // (EN) Module without exports - list empty
        Module mod("no_exports");
        SAD_ASSERT_TRUE(mod.getExports().empty());
    });

    SAD_TEST("EC03: DependencyGraph - وحدة واحدة بخمس اعتماديات", {
        // (AR) وحدة تعتمد على 5 وحدات
        // (EN) Module depends on 5 modules
        DependencyGraph graph;
        graph.addModule("big_app", {"a", "b", "c", "d", "e"});
        graph.addModule("a", {});
        graph.addModule("b", {});
        graph.addModule("c", {});
        graph.addModule("d", {});
        graph.addModule("e", {});
        
        SAD_ASSERT_FALSE(graph.hasCircularDependency());
        auto order = graph.resolveLoadOrder();
        SAD_ASSERT_EQ((int)order.size(), 6);
    });

    SAD_TEST("EC04: DependencyGraph - اعتماديات معين مشتركة", {
        // (AR) عدة وحدات تشترك في نفس الاعتمادية
        // (EN) Multiple modules share same dependency (diamond)
        //      A -> B, A -> C, B -> D, C -> D
        DependencyGraph graph;
        graph.addModule("A", {"B", "C"});
        graph.addModule("B", {"D"});
        graph.addModule("C", {"D"});
        graph.addModule("D", {});
        
        SAD_ASSERT_FALSE(graph.hasCircularDependency());
        auto order = graph.resolveLoadOrder();
        SAD_ASSERT_EQ((int)order.size(), 4);
        
        // D يُحمَّل أولاً
        SAD_ASSERT_EQ(order[0], std::string("D"));
    });

    SAD_TEST("EC05: Module cache - عدة وحدات", {
        // (AR) تخزين واسترجاع عدة وحدات في الكاش
        // (EN) Store and retrieve multiple modules from cache
        auto& cache = ModuleCache::getInstance();
        cache.clear();
        
        for (int i = 0; i < 10; i++) {
            std::string name = "mod_" + std::to_string(i);
            cache.put(name, std::make_unique<Module>(name));
        }
        
        for (int i = 0; i < 10; i++) {
            std::string name = "mod_" + std::to_string(i);
            SAD_ASSERT_TRUE(cache.has(name));
            SAD_ASSERT_NOT_NULL(cache.get(name));
        }
    });

    SAD_TEST("EC06: StringUtils - سلسلة عمليات متتالية", {
        // (AR) اختبار تسلسل عمليات: trim ثم toLower ثم split
        // (EN) Test chaining operations: trim then toLower then split
        std::string input = "  Hello,World,Test  ";
        std::string trimmed = StringUtils::trim(input);
        std::string lowered = StringUtils::toLower(trimmed);
        auto parts = StringUtils::split(lowered, ',');
        
        SAD_ASSERT_EQ((int)parts.size(), 3);
        SAD_ASSERT_EQ(parts[0], std::string("hello"));
        SAD_ASSERT_EQ(parts[1], std::string("world"));
        SAD_ASSERT_EQ(parts[2], std::string("test"));
    });

    SAD_TEST("EC07: StringUtils - split ثم join يُعيد النص الأصلي", {
        // (AR) split ثم join يجب أن يُعيد النص الأصلي (round-trip)
        // (EN) split then join should reconstruct original text (round-trip)
        std::string original = "a,b,c,d";
        auto parts = StringUtils::split(original, ',');
        std::string reconstructed = StringUtils::join(parts, ",");
        SAD_ASSERT_EQ(reconstructed, original);
    });

    SAD_TEST("EC08: DependencyGraph - clear ثم إعادة بناء", {
        // (AR) مسح الرسم ثم إعادة إضافة وحدات جديدة
        // (EN) Clear graph then add new modules
        DependencyGraph graph;
        graph.addModule("old", {});
        graph.clear();
        
        graph.addModule("new1", {"new2"});
        graph.addModule("new2", {});
        
        SAD_ASSERT_FALSE(graph.hasCircularDependency());
        auto order = graph.resolveLoadOrder();
        SAD_ASSERT_EQ((int)order.size(), 2);
    });

    SAD_TEST("EC09: Module - toString()", {
        // (AR) toString لوحدة يجب أن يحتوي على اسمها
        // (EN) toString for module should contain its name
        Module mod("display_mod", "/path.s");
        std::string str = mod.toString();
        SAD_ASSERT_FALSE(str.empty());
    });

    SAD_TEST("EC10: DependencyGraph - 10 وحدات سلسلة طويلة", {
        // (AR) سلسلة طويلة من 10 وحدات متتالية
        // (EN) Long chain of 10 sequential modules
        DependencyGraph graph;
        for (int i = 0; i < 10; i++) {
            std::string name = "mod_" + std::to_string(i);
            if (i < 9) {
                std::string dep = "mod_" + std::to_string(i + 1);
                graph.addModule(name, {dep});
            } else {
                graph.addModule(name, {});
            }
        }
        
        SAD_ASSERT_FALSE(graph.hasCircularDependency());
        auto order = graph.resolveLoadOrder();
        SAD_ASSERT_EQ((int)order.size(), 10);
        
        // mod_9 (بدون deps) يُحمَّل أولاً
        // mod_0 (root) يُحمَّل أخيراً
        SAD_ASSERT_EQ(order[0], std::string("mod_9"));
        SAD_ASSERT_EQ(order[9], std::string("mod_0"));
    });

    // ══════════════════════════════════════════════════════════════════
    //  عرض النتائج النهائية
    // ══════════════════════════════════════════════════════════════════
    SAD_SUMMARY();
}
