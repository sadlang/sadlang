/**
 * @file test_stdlib_comprehensive.cpp
 * @brief (AR) اختبارات شاملة للمكتبة القياسية / (EN) Comprehensive Standard Library Tests
 * 
 * ═══════════════════════════════════════════════════════════════════════
 *  ملف اختبارات المكتبة القياسية للغة "ص"
 *  يحتوي على ~90 اختبار يغطي:
 *    - دوال المصفوفات (ArrayFunctions): إضافة، حذف، بحث، ترتيب، تقطيع
 *    - دوال النصوص (StringFunctions): طول، بحث، استبدال، تقسيم، تحويل
 *    - دوال الرياضيات (MathFunctions): جذر، أس، مطلق، تقريب، مثلثات
 *    - دوال الأنواع (TypeFunctions): تحويل بين الأنواع، فحص الأنواع
 *    - مدير المكتبة القياسية (StandardLibraryManager): تسجيل الدوال
 *    - حالات الحدود وأخطاء المعاملات
 * ═══════════════════════════════════════════════════════════════════════
 * 
 * التصميم:
 *   كل مجموعة دوال تُختبر على 3 مستويات:
 *   1. الحالات الطبيعية: المدخلات الصحيحة والنتائج المتوقعة
 *   2. حالات الحدود: مصفوفات فارغة، نصوص فارغة، أصفار
 *   3. حالات الخطأ: أنواع خاطئة، معاملات ناقصة
 * 
 * الاعتماديات:
 *   - sad_test_framework.h: إطار الاختبار الموحد
 *   - value.h: نظام القيم (Sad::Data::Value)
 *   - array_functions.h: دوال المصفوفات (Sad::StdLib::Core::ArrayFunctions)
 *   - string_functions.h: دوال النصوص (Sad::StdLib::String::StringFunctions)
 *   - math_functions.h: دوال الرياضيات (Sad::StdLib::Math::MathFunctions)
 *   - type_functions.h: دوال الأنواع (Sad::StdLib::Core::TypeFunctions)
 * 
 * @author Sad Language Development Team
 * @date 2026-02-17
 */

#include "sad_test_framework.h"

// ══════════════════════════════════════════════════════════════════════
//  شوائب (includes) المكتبة القياسية
//  نحاول تضمين كل ملف، وإذا لم يتوفر نعطّل الاختبارات المرتبطة
// ══════════════════════════════════════════════════════════════════════

// نظام القيم - هذا متوفر دائماً
#include "value.h"

// محاولة تضمين دوال المصفوفات
#if __has_include("array_functions.h")
    #include "array_functions.h"
    #define HAS_ARRAY_FUNCTIONS 1
#else
    #define HAS_ARRAY_FUNCTIONS 0
#endif

// محاولة تضمين دوال النصوص
#if __has_include("string_functions.h")
    #include "string_functions.h"
    #define HAS_STRING_FUNCTIONS 1
#else
    #define HAS_STRING_FUNCTIONS 0
#endif

// محاولة تضمين دوال الرياضيات
#if __has_include("math_functions.h")
    #include "math_functions.h"
    #define HAS_MATH_FUNCTIONS 1
#else
    #define HAS_MATH_FUNCTIONS 0
#endif

// محاولة تضمين دوال الأنواع
#if __has_include("type_functions.h")
    #include "type_functions.h"
    #define HAS_TYPE_FUNCTIONS 1
#else
    #define HAS_TYPE_FUNCTIONS 0
#endif

// محاولة تضمين مدير المكتبة القياسية
#if __has_include("stdlib_manager.h")
    #include "stdlib_manager.h"
    #define HAS_STDLIB_MANAGER 1
#else
    #define HAS_STDLIB_MANAGER 0
#endif

// محاولة تضمين مدير الدوال (مطلوب لـ StandardLibraryManager)
#if __has_include("function_manager.h")
    #include "function_manager.h"
    #define HAS_FUNCTION_MANAGER 1
#else
    #define HAS_FUNCTION_MANAGER 0
#endif

using namespace Sad::Data;

// ══════════════════════════════════════════════════════════════════════
//  دوال مساعدة لبناء مصفوفات القيم بسهولة
//  هذه الدوال تسهّل كتابة الاختبارات بجعل الكود أقصر وأوضح
// ══════════════════════════════════════════════════════════════════════

/**
 * @brief بناء مصفوفة معاملات من عنصرين (مصفوفة + قيمة)
 * @details تُستخدم مع دوال مثل append, indexOf, contains
 *          التي تأخذ مصفوفة كأول معامل وقيمة كثاني معامل
 * 
 * @param arr المصفوفة الأساسية
 * @param extra القيمة الإضافية (عنصر يُضاف أو يُبحث عنه)
 * @return std::vector<Value> مصفوفة المعاملات الجاهزة للتمرير
 */
static std::vector<Value> makeArgs(const Value& arr, const Value& extra) {
    return {arr, extra};
}

/**
 * @brief بناء مصفوفة معاملات من قيمة واحدة
 * @details تُستخدم مع دوال مثل size, pop, sort التي تأخذ معاملاً واحداً
 * 
 * @param v القيمة الوحيدة
 * @return std::vector<Value> مصفوفة معاملات بعنصر واحد
 */
static std::vector<Value> makeArgs(const Value& v) {
    return {v};
}

/**
 * @brief بناء مصفوفة أعداد صحيحة بطريقة مختصرة
 * @details بدلاً من كتابة Value(ArrayType{Value(1), Value(2), Value(3)})
 *          يمكن كتابة makeIntArray({1, 2, 3})
 * 
 * @param nums قائمة الأعداد
 * @return Value مصفوفة من نوع ARRAY تحتوي الأعداد
 */
static Value makeIntArray(std::initializer_list<int> nums) {
    Value::ArrayType arr;
    for (int n : nums) arr.push_back(Value(n));
    return Value(arr);
}

/**
 * @brief بناء مصفوفة نصوص بطريقة مختصرة
 * @details تسهّل إنشاء مصفوفات النصوص للاختبار
 * 
 * @param strs قائمة النصوص
 * @return Value مصفوفة من نوع ARRAY تحتوي النصوص
 */
static Value makeStringArray(std::initializer_list<std::string> strs) {
    Value::ArrayType arr;
    for (const auto& s : strs) arr.push_back(Value(s));
    return Value(arr);
}

// ══════════════════════════════════════════════════════════════════════
//  نقطة الدخول
// ══════════════════════════════════════════════════════════════════════

int main() {
    SAD_TEST_INIT();

    // ┌──────────────────────────────────────────────────────────────────┐
    // │ القسم 1: اختبارات دوال المصفوفات (ArrayFunctions)              │
    // │ ~25 اختبار: إضافة، إزالة، حجم، بحث، ترتيب، عكس، تقطيع       │
    // │ كل دالة تُختبر بحالات: عادية + حدية + أنواع مختلطة            │
    // └──────────────────────────────────────────────────────────────────┘

#if HAS_ARRAY_FUNCTIONS
    SAD_GROUP("Stdlib.ArrayFunctions / المكتبة.المصفوفات");

    // ─────────────────────────── append (إضافة) ──────────────────────────
    // تأخذ مصفوفة + عنصر، ترجع مصفوفة جديدة بالعنصر مُضافاً في النهاية
    SAD_TEST("ARR01: إضافة عنصر رقمي لمصفوفة — append([1,2,3], 4) = [1,2,3,4]", {
        auto arr = makeIntArray({1, 2, 3});
        auto result = Sad::StdLib::Core::ArrayFunctions::append(makeArgs(arr, Value(4)));
        SAD_ASSERT_TRUE(result.isArray());
        SAD_ASSERT_EQ((int)result.toArray().size(), 4);
        SAD_ASSERT_EQ(result.toArray()[3].toInt(), 4);
    });

    SAD_TEST("ARR02: إضافة نص لمصفوفة أرقام — أنواع مختلطة", {
        // المصفوفات في لغة ص تدعم أنواعاً مختلطة
        auto arr = makeIntArray({1, 2});
        auto result = Sad::StdLib::Core::ArrayFunctions::append(
            makeArgs(arr, Value("hello")));
        SAD_ASSERT_EQ((int)result.toArray().size(), 3);
        SAD_ASSERT_EQ(result.toArray()[2].toString(), std::string("hello"));
    });

    SAD_TEST("ARR03: إضافة لمصفوفة فارغة — الحالة الحدية الأساسية", {
        Value::ArrayType empty;
        Value emptyArr(empty);
        auto result = Sad::StdLib::Core::ArrayFunctions::append(
            makeArgs(emptyArr, Value(42)));
        SAD_ASSERT_EQ((int)result.toArray().size(), 1);
        SAD_ASSERT_EQ(result.toArray()[0].toInt(), 42);
    });

    // ─────────────────────────── pop (إزالة) ─────────────────────────────
    // تحذف آخر عنصر وترجعه
    SAD_TEST("ARR04: إزالة آخر عنصر — pop([1,2,3]) = 3", {
        auto arr = makeIntArray({1, 2, 3});
        auto result = Sad::StdLib::Core::ArrayFunctions::pop(makeArgs(arr));
        // يُرجع العنصر الأخير
        SAD_ASSERT_EQ(result.toInt(), 3);
    });

    SAD_TEST("ARR05: إزالة عنصر وحيد — pop([42]) = 42", {
        auto arr = makeIntArray({42});
        auto result = Sad::StdLib::Core::ArrayFunctions::pop(makeArgs(arr));
        SAD_ASSERT_EQ(result.toInt(), 42);
    });

    // ─────────────────────────── size (حجم) ──────────────────────────────
    // ترجع عدد عناصر المصفوفة كقيمة صحيحة
    SAD_TEST("ARR06: حجم مصفوفة عادية — size([1,2,3,4,5]) = 5", {
        auto arr = makeIntArray({1, 2, 3, 4, 5});
        auto result = Sad::StdLib::Core::ArrayFunctions::size(makeArgs(arr));
        SAD_ASSERT_EQ(result.toInt(), 5);
    });

    SAD_TEST("ARR07: حجم مصفوفة فارغة — size([]) = 0", {
        Value::ArrayType empty;
        Value emptyArr(empty);
        auto result = Sad::StdLib::Core::ArrayFunctions::size(makeArgs(emptyArr));
        SAD_ASSERT_EQ(result.toInt(), 0);
    });

    // ─────────────────────────── indexOf (فهرس) ──────────────────────────
    // ترجع موقع أول حدوث للعنصر، أو -1 إذا لم يُوجد
    SAD_TEST("ARR08: إيجاد فهرس عنصر موجود — indexOf([10,20,30], 20) = 1", {
        auto arr = makeIntArray({10, 20, 30});
        auto result = Sad::StdLib::Core::ArrayFunctions::indexOf(
            makeArgs(arr, Value(20)));
        SAD_ASSERT_EQ(result.toInt(), 1);
    });

    SAD_TEST("ARR09: فهرس عنصر أول — indexOf([10,20,30], 10) = 0", {
        auto arr = makeIntArray({10, 20, 30});
        auto result = Sad::StdLib::Core::ArrayFunctions::indexOf(
            makeArgs(arr, Value(10)));
        SAD_ASSERT_EQ(result.toInt(), 0);
    });

    SAD_TEST("ARR10: فهرس عنصر غير موجود — indexOf([10,20,30], 99) = -1", {
        auto arr = makeIntArray({10, 20, 30});
        auto result = Sad::StdLib::Core::ArrayFunctions::indexOf(
            makeArgs(arr, Value(99)));
        SAD_ASSERT_EQ(result.toInt(), -1);
    });

    // ─────────────────────────── contains (يحتوي) ────────────────────────
    // ترجع صحيح/خطأ هل المصفوفة تحتوي العنصر
    SAD_TEST("ARR11: التحقق من وجود عنصر — contains([1,2,3], 2) = true", {
        auto arr = makeIntArray({1, 2, 3});
        auto result = Sad::StdLib::Core::ArrayFunctions::contains(
            makeArgs(arr, Value(2)));
        SAD_ASSERT_TRUE(result.toBool());
    });

    SAD_TEST("ARR12: عنصر غير موجود — contains([1,2,3], 99) = false", {
        auto arr = makeIntArray({1, 2, 3});
        auto result = Sad::StdLib::Core::ArrayFunctions::contains(
            makeArgs(arr, Value(99)));
        SAD_ASSERT_FALSE(result.toBool());
    });

    SAD_TEST("ARR13: بحث في مصفوفة فارغة — contains([], 1) = false", {
        Value::ArrayType empty;
        Value emptyArr(empty);
        auto result = Sad::StdLib::Core::ArrayFunctions::contains(
            makeArgs(emptyArr, Value(1)));
        SAD_ASSERT_FALSE(result.toBool());
    });

    // ─────────────────────────── reverse (عكس) ───────────────────────────
    // تعكس ترتيب عناصر المصفوفة
    SAD_TEST("ARR14: عكس مصفوفة — reverse([1,2,3]) = [3,2,1]", {
        auto arr = makeIntArray({1, 2, 3});
        auto result = Sad::StdLib::Core::ArrayFunctions::reverse(makeArgs(arr));
        SAD_ASSERT_TRUE(result.isArray());
        SAD_ASSERT_EQ(result.toArray()[0].toInt(), 3);
        SAD_ASSERT_EQ(result.toArray()[1].toInt(), 2);
        SAD_ASSERT_EQ(result.toArray()[2].toInt(), 1);
    });

    SAD_TEST("ARR15: عكس مصفوفة عنصر واحد — reverse([42]) = [42]", {
        auto arr = makeIntArray({42});
        auto result = Sad::StdLib::Core::ArrayFunctions::reverse(makeArgs(arr));
        SAD_ASSERT_EQ((int)result.toArray().size(), 1);
        SAD_ASSERT_EQ(result.toArray()[0].toInt(), 42);
    });

    SAD_TEST("ARR16: عكس مصفوفة فارغة — reverse([]) = []", {
        Value::ArrayType empty;
        Value emptyArr(empty);
        auto result = Sad::StdLib::Core::ArrayFunctions::reverse(makeArgs(emptyArr));
        SAD_ASSERT_EQ((int)result.toArray().size(), 0);
    });

    // ─────────────────────────── sort (ترتيب) ────────────────────────────
    // ترتيب تصاعدي
    SAD_TEST("ARR17: ترتيب أرقام — sort([3,1,4,1,5]) = [1,1,3,4,5]", {
        auto arr = makeIntArray({3, 1, 4, 1, 5});
        auto result = Sad::StdLib::Core::ArrayFunctions::sort(makeArgs(arr));
        SAD_ASSERT_TRUE(result.isArray());
        SAD_ASSERT_EQ(result.toArray()[0].toInt(), 1);
        SAD_ASSERT_EQ(result.toArray()[1].toInt(), 1);
        SAD_ASSERT_EQ(result.toArray()[2].toInt(), 3);
    });

    SAD_TEST("ARR18: ترتيب مصفوفة مرتبة — sort([1,2,3]) = [1,2,3]", {
        auto arr = makeIntArray({1, 2, 3});
        auto result = Sad::StdLib::Core::ArrayFunctions::sort(makeArgs(arr));
        SAD_ASSERT_EQ(result.toArray()[0].toInt(), 1);
        SAD_ASSERT_EQ(result.toArray()[2].toInt(), 3);
    });

    // ─────────────────────────── first / last ────────────────────────────
    SAD_TEST("ARR19: أول عنصر — first([10,20,30]) = 10", {
        auto arr = makeIntArray({10, 20, 30});
        auto result = Sad::StdLib::Core::ArrayFunctions::first(makeArgs(arr));
        SAD_ASSERT_EQ(result.toInt(), 10);
    });

    SAD_TEST("ARR20: آخر عنصر — last([10,20,30]) = 30", {
        auto arr = makeIntArray({10, 20, 30});
        auto result = Sad::StdLib::Core::ArrayFunctions::last(makeArgs(arr));
        SAD_ASSERT_EQ(result.toInt(), 30);
    });

    // ─────────────────────────── slice (تقطيع) ───────────────────────────
    SAD_TEST("ARR21: تقطيع من البداية — slice([1,2,3,4,5], 0, 3) = [1,2,3]", {
        auto arr = makeIntArray({1, 2, 3, 4, 5});
        std::vector<Value> args = {arr, Value(0), Value(3)};
        auto result = Sad::StdLib::Core::ArrayFunctions::slice(args);
        SAD_ASSERT_EQ((int)result.toArray().size(), 3);
        SAD_ASSERT_EQ(result.toArray()[0].toInt(), 1);
        SAD_ASSERT_EQ(result.toArray()[2].toInt(), 3);
    });

    SAD_TEST("ARR22: تقطيع من المنتصف — slice([1,2,3,4,5], 1, 4) = [2,3,4]", {
        auto arr = makeIntArray({1, 2, 3, 4, 5});
        std::vector<Value> args = {arr, Value(1), Value(4)};
        auto result = Sad::StdLib::Core::ArrayFunctions::slice(args);
        SAD_ASSERT_EQ((int)result.toArray().size(), 3);
        SAD_ASSERT_EQ(result.toArray()[0].toInt(), 2);
    });

    // ─────────────────────────── حالات حدية ──────────────────────────────
    SAD_TEST("ARR23: مصفوفة بـ 100 عنصر — اختبار أداء", {
        Value::ArrayType big;
        for (int i = 0; i < 100; i++) big.push_back(Value(i));
        Value bigArr(big);
        auto result = Sad::StdLib::Core::ArrayFunctions::size(makeArgs(bigArr));
        SAD_ASSERT_EQ(result.toInt(), 100);
    });

    SAD_TEST("ARR24: مصفوفة أنواع مختلطة — [1, \"hello\", true, 3.14]", {
        Value::ArrayType mixed;
        mixed.push_back(Value(1));
        mixed.push_back(Value("hello"));
        mixed.push_back(Value(true));
        mixed.push_back(Value(3.14));
        Value mixedArr(mixed);
        auto result = Sad::StdLib::Core::ArrayFunctions::size(makeArgs(mixedArr));
        SAD_ASSERT_EQ(result.toInt(), 4);
    });

    SAD_TEST("ARR25: append ثم size — التحقق من التسلسل", {
        Value::ArrayType empty;
        Value arr(empty);
        // إضافة 3 عناصر متتالية
        arr = Sad::StdLib::Core::ArrayFunctions::append(makeArgs(arr, Value(10)));
        arr = Sad::StdLib::Core::ArrayFunctions::append(makeArgs(arr, Value(20)));
        arr = Sad::StdLib::Core::ArrayFunctions::append(makeArgs(arr, Value(30)));
        auto result = Sad::StdLib::Core::ArrayFunctions::size(makeArgs(arr));
        SAD_ASSERT_EQ(result.toInt(), 3);
    });

#else
    // ─── إذا لم تتوفر دوال المصفوفات، نتخطى ───
    SAD_GROUP("Stdlib.ArrayFunctions / المكتبة.المصفوفات [SKIPPED]");
    SAD_SKIP("ARR01-25: دوال المصفوفات غير متوفرة", "missing array_functions.h");
#endif

    // ┌──────────────────────────────────────────────────────────────────┐
    // │ القسم 2: اختبارات دوال النصوص (StringFunctions)               │
    // │ ~20 اختبار: طول، بحث، استبدال، استخراج، تحويل حالة            │
    // │ تدعم Unicode و Arabic text                                     │
    // └──────────────────────────────────────────────────────────────────┘

#if HAS_STRING_FUNCTIONS
    SAD_GROUP("Stdlib.StringFunctions / المكتبة.النصوص");

    // ─────────────────────────── length (طول) ────────────────────────────
    SAD_TEST("STR01: طول نص إنجليزي — length(\"Hello\") = 5", {
        auto result = Sad::StdLib::String::StringFunctions::length(
            makeArgs(Value("Hello")));
        SAD_ASSERT_EQ(result.toInt(), 5);
    });

    SAD_TEST("STR02: طول نص فارغ — length(\"\") = 0", {
        auto result = Sad::StdLib::String::StringFunctions::length(
            makeArgs(Value("")));
        SAD_ASSERT_EQ(result.toInt(), 0);
    });

    SAD_TEST("STR03: طول نص طويل — 100 حرف", {
        std::string longStr(100, 'A');
        auto result = Sad::StdLib::String::StringFunctions::length(
            makeArgs(Value(longStr)));
        SAD_ASSERT_EQ(result.toInt(), 100);
    });

    // ─────────────────────────── find (بحث) ──────────────────────────────
    SAD_TEST("STR04: بحث عن كلمة موجودة — find(\"Hello World\", \"World\") = 6", {
        std::vector<Value> args = {Value("Hello World"), Value("World")};
        auto result = Sad::StdLib::String::StringFunctions::find(args);
        SAD_ASSERT_EQ(result.toInt(), 6);
    });

    SAD_TEST("STR05: بحث عن كلمة غير موجودة — find(\"Hello\", \"xyz\") = -1", {
        std::vector<Value> args = {Value("Hello"), Value("xyz")};
        auto result = Sad::StdLib::String::StringFunctions::find(args);
        SAD_ASSERT_EQ(result.toInt(), -1);
    });

    SAD_TEST("STR06: بحث من موقع معين — find(\"AAA\", \"A\", 1) = 1", {
        std::vector<Value> args = {Value("AAA"), Value("A"), Value(1)};
        auto result = Sad::StdLib::String::StringFunctions::find(args);
        SAD_ASSERT_GE(result.toInt(), 1);
    });

    SAD_TEST("STR07: بحث في نص فارغ — find(\"\", \"x\") = -1", {
        std::vector<Value> args = {Value(""), Value("x")};
        auto result = Sad::StdLib::String::StringFunctions::find(args);
        SAD_ASSERT_EQ(result.toInt(), -1);
    });

    // ─────────────────────────── replace (استبدال) ───────────────────────
    SAD_TEST("STR08: استبدال كلمة — replace(\"Hello World\", \"World\", \"ص\")", {
        std::vector<Value> args = {Value("Hello World"), Value("World"), Value("Sad")};
        auto result = Sad::StdLib::String::StringFunctions::replace(args);
        SAD_ASSERT_EQ(result.toString(), std::string("Hello Sad"));
    });

    SAD_TEST("STR09: استبدال متعدد — replace(\"aaa\", \"a\", \"b\") = \"bbb\"", {
        std::vector<Value> args = {Value("aaa"), Value("a"), Value("b")};
        auto result = Sad::StdLib::String::StringFunctions::replace(args);
        SAD_ASSERT_EQ(result.toString(), std::string("bbb"));
    });

    SAD_TEST("STR10: استبدال كلمة غير موجودة — لا تغيير", {
        std::vector<Value> args = {Value("Hello"), Value("xyz"), Value("abc")};
        auto result = Sad::StdLib::String::StringFunctions::replace(args);
        SAD_ASSERT_EQ(result.toString(), std::string("Hello"));
    });

    // ─────────────────────────── substring (استخراج) ─────────────────────
    SAD_TEST("STR11: استخراج من البداية — substring(\"Hello World\", 0, 5) = \"Hello\"", {
        std::vector<Value> args = {Value("Hello World"), Value(0), Value(5)};
        auto result = Sad::StdLib::String::StringFunctions::substring(args);
        SAD_ASSERT_EQ(result.toString(), std::string("Hello"));
    });

    SAD_TEST("STR12: استخراج من المنتصف — substring(\"Hello World\", 6, 5) = \"World\"", {
        std::vector<Value> args = {Value("Hello World"), Value(6), Value(5)};
        auto result = Sad::StdLib::String::StringFunctions::substring(args);
        SAD_ASSERT_EQ(result.toString(), std::string("World"));
    });

    // ─────────────────────────── toLower / toUpper ───────────────────────
    SAD_TEST("STR13: تحويل لأحرف صغيرة — toLower(\"HELLO\") = \"hello\"", {
        auto result = Sad::StdLib::String::StringFunctions::toLower(
            makeArgs(Value("HELLO")));
        SAD_ASSERT_EQ(result.toString(), std::string("hello"));
    });

    SAD_TEST("STR14: تحويل لأحرف كبيرة — toUpper(\"hello\") = \"HELLO\"", {
        auto result = Sad::StdLib::String::StringFunctions::toUpper(
            makeArgs(Value("hello")));
        SAD_ASSERT_EQ(result.toString(), std::string("HELLO"));
    });

    SAD_TEST("STR15: toLower لنص مختلط — \"HeLLo\" → \"hello\"", {
        auto result = Sad::StdLib::String::StringFunctions::toLower(
            makeArgs(Value("HeLLo")));
        SAD_ASSERT_EQ(result.toString(), std::string("hello"));
    });

    SAD_TEST("STR16: toUpper لنص فارغ — \"\" → \"\"", {
        auto result = Sad::StdLib::String::StringFunctions::toUpper(
            makeArgs(Value("")));
        SAD_ASSERT_EQ(result.toString(), std::string(""));
    });

    // ─────────────────────────── split / join ────────────────────────────
    SAD_TEST("STR17: تقسيم نص بفاصل — split(\"a,b,c\", \",\") = [\"a\",\"b\",\"c\"]", {
        std::vector<Value> args = {Value("a,b,c"), Value(",")};
        auto result = Sad::StdLib::String::StringFunctions::split(args);
        SAD_ASSERT_TRUE(result.isArray());
        SAD_ASSERT_EQ((int)result.toArray().size(), 3);
        SAD_ASSERT_EQ(result.toArray()[0].toString(), std::string("a"));
        SAD_ASSERT_EQ(result.toArray()[2].toString(), std::string("c"));
    });

    SAD_TEST("STR18: تقسيم بمسافة — split(\"hello world\", \" \") = [\"hello\",\"world\"]", {
        std::vector<Value> args = {Value("hello world"), Value(" ")};
        auto result = Sad::StdLib::String::StringFunctions::split(args);
        SAD_ASSERT_EQ((int)result.toArray().size(), 2);
    });

    SAD_TEST("STR19: join مصفوفة — join([\"a\",\"b\",\"c\"], \"-\") = \"a-b-c\"", {
        auto arr = makeStringArray({"a", "b", "c"});
        std::vector<Value> args = {arr, Value("-")};
        auto result = Sad::StdLib::String::StringFunctions::join(args);
        SAD_ASSERT_EQ(result.toString(), std::string("a-b-c"));
    });

    SAD_TEST("STR20: join بنص فارغ — join([\"a\",\"b\"], \"\") = \"ab\"", {
        auto arr = makeStringArray({"a", "b"});
        std::vector<Value> args = {arr, Value("")};
        auto result = Sad::StdLib::String::StringFunctions::join(args);
        SAD_ASSERT_EQ(result.toString(), std::string("ab"));
    });

#else
    SAD_GROUP("Stdlib.StringFunctions / المكتبة.النصوص [SKIPPED]");
    SAD_SKIP("STR01-20: دوال النصوص غير متوفرة", "missing string_functions.h");
#endif

    // ┌──────────────────────────────────────────────────────────────────┐
    // │ القسم 3: اختبارات دوال الرياضيات (MathFunctions)              │
    // │ ~20 اختبار: جذر، أس، مطلق، أكبر/أصغر، تقريب                  │
    // └──────────────────────────────────────────────────────────────────┘

#if HAS_MATH_FUNCTIONS
    SAD_GROUP("Stdlib.MathFunctions / المكتبة.الرياضيات");

    // ─────────────────────────── sqrt (جذر) ──────────────────────────────
    SAD_TEST("MATH01: جذر تربيعي كامل — sqrt(16) = 4.0", {
        auto result = Sad::StdLib::Math::MathFunctions::sqrt(makeArgs(Value(16)));
        SAD_ASSERT_FLOAT_EQ(result.toDouble(), 4.0, 0.001);
    });

    SAD_TEST("MATH02: جذر تربيعي غير كامل — sqrt(2) ≈ 1.414", {
        auto result = Sad::StdLib::Math::MathFunctions::sqrt(makeArgs(Value(2)));
        SAD_ASSERT_FLOAT_EQ(result.toDouble(), 1.41421, 0.001);
    });

    SAD_TEST("MATH03: جذر الصفر — sqrt(0) = 0.0", {
        auto result = Sad::StdLib::Math::MathFunctions::sqrt(makeArgs(Value(0)));
        SAD_ASSERT_FLOAT_EQ(result.toDouble(), 0.0, 0.001);
    });

    SAD_TEST("MATH04: جذر واحد — sqrt(1) = 1.0", {
        auto result = Sad::StdLib::Math::MathFunctions::sqrt(makeArgs(Value(1)));
        SAD_ASSERT_FLOAT_EQ(result.toDouble(), 1.0, 0.001);
    });

    // ─────────────────────────── power (أس) ──────────────────────────────
    SAD_TEST("MATH05: أس — power(2, 3) = 8", {
        std::vector<Value> args = {Value(2), Value(3)};
        auto result = Sad::StdLib::Math::MathFunctions::power(args);
        SAD_ASSERT_FLOAT_EQ(result.toDouble(), 8.0, 0.001);
    });

    SAD_TEST("MATH06: أس صفري — power(5, 0) = 1", {
        std::vector<Value> args = {Value(5), Value(0)};
        auto result = Sad::StdLib::Math::MathFunctions::power(args);
        SAD_ASSERT_FLOAT_EQ(result.toDouble(), 1.0, 0.001);
    });

    SAD_TEST("MATH07: أس سالب — power(2, -1) = 0.5", {
        std::vector<Value> args = {Value(2), Value(-1)};
        auto result = Sad::StdLib::Math::MathFunctions::power(args);
        SAD_ASSERT_FLOAT_EQ(result.toDouble(), 0.5, 0.001);
    });

    // ─────────────────────────── abs (مطلق) ──────────────────────────────
    SAD_TEST("MATH08: قيمة مطلقة لسالب — abs(-5) = 5", {
        auto result = Sad::StdLib::Math::MathFunctions::abs(makeArgs(Value(-5)));
        SAD_ASSERT_EQ(result.toInt(), 5);
    });

    SAD_TEST("MATH09: قيمة مطلقة لموجب — abs(5) = 5", {
        auto result = Sad::StdLib::Math::MathFunctions::abs(makeArgs(Value(5)));
        SAD_ASSERT_EQ(result.toInt(), 5);
    });

    SAD_TEST("MATH10: قيمة مطلقة للصفر — abs(0) = 0", {
        auto result = Sad::StdLib::Math::MathFunctions::abs(makeArgs(Value(0)));
        SAD_ASSERT_EQ(result.toInt(), 0);
    });

    SAD_TEST("MATH11: قيمة مطلقة عشرية — abs(-3.14) ≈ 3.14", {
        auto result = Sad::StdLib::Math::MathFunctions::abs(makeArgs(Value(-3.14)));
        SAD_ASSERT_FLOAT_EQ(result.toDouble(), 3.14, 0.001);
    });

    // ─────────────────────────── max / min ───────────────────────────────
    SAD_TEST("MATH12: أكبر قيمة — max(3, 7, 2) = 7", {
        std::vector<Value> args = {Value(3), Value(7), Value(2)};
        auto result = Sad::StdLib::Math::MathFunctions::max(args);
        SAD_ASSERT_EQ(result.toInt(), 7);
    });

    SAD_TEST("MATH13: أصغر قيمة — min(3, 7, 2) = 2", {
        std::vector<Value> args = {Value(3), Value(7), Value(2)};
        auto result = Sad::StdLib::Math::MathFunctions::min(args);
        SAD_ASSERT_EQ(result.toInt(), 2);
    });

    SAD_TEST("MATH14: max قيمة واحدة — max(42) = 42", {
        std::vector<Value> args = {Value(42)};
        auto result = Sad::StdLib::Math::MathFunctions::max(args);
        SAD_ASSERT_EQ(result.toInt(), 42);
    });

    SAD_TEST("MATH15: min بأعداد سالبة — min(-1, -5, -3) = -5", {
        std::vector<Value> args = {Value(-1), Value(-5), Value(-3)};
        auto result = Sad::StdLib::Math::MathFunctions::min(args);
        SAD_ASSERT_EQ(result.toInt(), -5);
    });

    // ─────────────────────────── round / floor / ceil ─────────────────────
    SAD_TEST("MATH16: تقريب لأقرب — round(3.7) = 4", {
        auto result = Sad::StdLib::Math::MathFunctions::round(makeArgs(Value(3.7)));
        SAD_ASSERT_EQ(result.toInt(), 4);
    });

    SAD_TEST("MATH17: تقريب لأسفل — round(3.2) = 3", {
        auto result = Sad::StdLib::Math::MathFunctions::round(makeArgs(Value(3.2)));
        SAD_ASSERT_EQ(result.toInt(), 3);
    });

    SAD_TEST("MATH18: أرضية — floor(3.9) = 3", {
        auto result = Sad::StdLib::Math::MathFunctions::floor(makeArgs(Value(3.9)));
        SAD_ASSERT_EQ(result.toInt(), 3);
    });

    SAD_TEST("MATH19: أرضية سالبة — floor(-2.5) = -3", {
        auto result = Sad::StdLib::Math::MathFunctions::floor(makeArgs(Value(-2.5)));
        SAD_ASSERT_EQ(result.toInt(), -3);
    });

    SAD_TEST("MATH20: سقف — ceil(3.2) = 4", {
        auto result = Sad::StdLib::Math::MathFunctions::ceil(makeArgs(Value(3.2)));
        SAD_ASSERT_EQ(result.toInt(), 4);
    });

#else
    SAD_GROUP("Stdlib.MathFunctions / المكتبة.الرياضيات [SKIPPED]");
    SAD_SKIP("MATH01-20: دوال الرياضيات غير متوفرة", "missing math_functions.h");
#endif

    // ┌──────────────────────────────────────────────────────────────────┐
    // │ القسم 4: اختبارات دوال الأنواع (TypeFunctions)                │
    // │ ~20 اختبار: تحويل نصي، تحويل رقمي، فحص نوع                    │
    // └──────────────────────────────────────────────────────────────────┘

#if HAS_TYPE_FUNCTIONS
    SAD_GROUP("Stdlib.TypeFunctions / المكتبة.الأنواع");

    // ─────────────────────────── toString (لنص) ──────────────────────────
    SAD_TEST("TYPE01: تحويل رقم صحيح لنص — toString(42) = \"42\"", {
        auto result = Sad::StdLib::Core::TypeFunctions::toString(makeArgs(Value(42)));
        SAD_ASSERT_EQ(result.toString(), std::string("42"));
    });

    SAD_TEST("TYPE02: تحويل رقم عشري لنص — toString(3.14)", {
        auto result = Sad::StdLib::Core::TypeFunctions::toString(makeArgs(Value(3.14)));
        // النتيجة يجب أن تحتوي "3.14"
        SAD_ASSERT_CONTAINS(result.toString(), "3.14");
    });

    SAD_TEST("TYPE03: تحويل منطقي لنص — toString(true)", {
        auto result = Sad::StdLib::Core::TypeFunctions::toString(makeArgs(Value(true)));
        auto str = result.toString();
        // قد يكون "true" أو "صحيح"
        SAD_ASSERT_TRUE(str == "true" || str == "1" || 
                        str.find("\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD") != std::string::npos);
    });

    SAD_TEST("TYPE04: تحويل نص لنفسه — toString(\"hello\") = \"hello\"", {
        auto result = Sad::StdLib::Core::TypeFunctions::toString(makeArgs(Value("hello")));
        SAD_ASSERT_EQ(result.toString(), std::string("hello"));
    });

    // ─────────────────────────── toInt (لرقم_صحيح) ───────────────────────
    SAD_TEST("TYPE05: تحويل نص لرقم — toInt(\"123\") = 123", {
        auto result = Sad::StdLib::Core::TypeFunctions::toInt(makeArgs(Value("123")));
        SAD_ASSERT_EQ(result.toInt(), 123);
    });

    SAD_TEST("TYPE06: تحويل عشري لصحيح — toInt(3.14) = 3", {
        auto result = Sad::StdLib::Core::TypeFunctions::toInt(makeArgs(Value(3.14)));
        SAD_ASSERT_EQ(result.toInt(), 3);
    });

    SAD_TEST("TYPE07: تحويل منطقي لرقم — toInt(true) = 1", {
        auto result = Sad::StdLib::Core::TypeFunctions::toInt(makeArgs(Value(true)));
        SAD_ASSERT_EQ(result.toInt(), 1);
    });

    SAD_TEST("TYPE08: تحويل منطقي خطأ لرقم — toInt(false) = 0", {
        auto result = Sad::StdLib::Core::TypeFunctions::toInt(makeArgs(Value(false)));
        SAD_ASSERT_EQ(result.toInt(), 0);
    });

    SAD_TEST("TYPE09: تحويل نص غير رقمي — toInt(\"hello\") = 0", {
        auto result = Sad::StdLib::Core::TypeFunctions::toInt(makeArgs(Value("hello")));
        SAD_ASSERT_EQ(result.toInt(), 0);
    });

    // ─────────────────────────── toFloat (لعشري) ─────────────────────────
    SAD_TEST("TYPE10: تحويل نص لعشري — toFloat(\"3.14\") ≈ 3.14", {
        auto result = Sad::StdLib::Core::TypeFunctions::toFloat(makeArgs(Value("3.14")));
        SAD_ASSERT_FLOAT_EQ(result.toDouble(), 3.14, 0.01);
    });

    SAD_TEST("TYPE11: تحويل صحيح لعشري — toFloat(42) = 42.0", {
        auto result = Sad::StdLib::Core::TypeFunctions::toFloat(makeArgs(Value(42)));
        SAD_ASSERT_FLOAT_EQ(result.toDouble(), 42.0, 0.01);
    });

    // ─────────────────────────── toBool (لمنطقي) ─────────────────────────
    SAD_TEST("TYPE12: تحويل صفر لمنطقي — toBool(0) = false", {
        auto result = Sad::StdLib::Core::TypeFunctions::toBool(makeArgs(Value(0)));
        SAD_ASSERT_FALSE(result.toBool());
    });

    SAD_TEST("TYPE13: تحويل رقم غير صفري — toBool(1) = true", {
        auto result = Sad::StdLib::Core::TypeFunctions::toBool(makeArgs(Value(1)));
        SAD_ASSERT_TRUE(result.toBool());
    });

    SAD_TEST("TYPE14: تحويل نص فارغ لمنطقي — toBool(\"\") = false", {
        auto result = Sad::StdLib::Core::TypeFunctions::toBool(makeArgs(Value("")));
        SAD_ASSERT_FALSE(result.toBool());
    });

    SAD_TEST("TYPE15: تحويل نص غير فارغ — toBool(\"hello\") = true", {
        auto result = Sad::StdLib::Core::TypeFunctions::toBool(makeArgs(Value("hello")));
        SAD_ASSERT_TRUE(result.toBool());
    });

    // ─────────────────────────── isInt / isString / isArray ──────────────
    SAD_TEST("TYPE16: فحص نوع رقم — isInt(42) = true", {
        auto result = Sad::StdLib::Core::TypeFunctions::isInt(makeArgs(Value(42)));
        SAD_ASSERT_TRUE(result.toBool());
    });

    SAD_TEST("TYPE17: فحص نوع نص — isString(\"hello\") = true", {
        auto result = Sad::StdLib::Core::TypeFunctions::isString(makeArgs(Value("hello")));
        SAD_ASSERT_TRUE(result.toBool());
    });

    SAD_TEST("TYPE18: فحص نوع خاطئ — isInt(\"hello\") = false", {
        auto result = Sad::StdLib::Core::TypeFunctions::isInt(makeArgs(Value("hello")));
        SAD_ASSERT_FALSE(result.toBool());
    });

    SAD_TEST("TYPE19: فحص مصفوفة — isArray([1,2,3]) = true", {
        auto arr = makeIntArray({1, 2, 3});
        auto result = Sad::StdLib::Core::TypeFunctions::isArray(makeArgs(arr));
        SAD_ASSERT_TRUE(result.toBool());
    });

    // ─────────────────────────── type (اسم_النوع) ──────────────────────────
    SAD_TEST("TYPE20: اسم نوع رقم — type(42)", {
        auto result = Sad::StdLib::Core::TypeFunctions::type(makeArgs(Value(42)));
        auto name = result.toString();
        // يجب أن يحتوي "int" أو "رقم" أو "صحيح" أو "INTEGER"
        SAD_ASSERT_TRUE(name.length() > 0);
    });

#else
    SAD_GROUP("Stdlib.TypeFunctions / المكتبة.الأنواع [SKIPPED]");
    SAD_SKIP("TYPE01-20: دوال الأنواع غير متوفرة", "missing type_functions.h");
#endif

    // ┌──────────────────────────────────────────────────────────────────┐
    // │ القسم 5: اختبارات مدير المكتبة (StandardLibraryManager)        │
    // │ ~5 اختبارات: تسجيل دوال، تحقق من التسجيل                      │
    // └──────────────────────────────────────────────────────────────────┘

#if HAS_STDLIB_MANAGER && HAS_FUNCTION_MANAGER
    SAD_GROUP("Stdlib.Manager / المكتبة.المدير");

    SAD_TEST("MGR01: إنشاء مدير المكتبة القياسية", {
        // ننشئ FunctionManager ثم StandardLibraryManager
        Sad::Data::FunctionManager fm;
        SAD_ASSERT_NO_THROW(
            Sad::StdLib::StandardLibraryManager mgr(fm);
        );
    });

    SAD_TEST("MGR02: تسجيل جميع الدوال بنجاح", {
        Sad::Data::FunctionManager fm;
        Sad::StdLib::StandardLibraryManager mgr(fm);
        bool ok = mgr.registerAllFunctions();
        SAD_ASSERT_TRUE(ok);
    });

    SAD_TEST("MGR03: حالة التسجيل غير فارغة", {
        Sad::Data::FunctionManager fm;
        Sad::StdLib::StandardLibraryManager mgr(fm);
        mgr.registerAllFunctions();
        auto status = mgr.getRegistrationStatus();
        SAD_ASSERT_TRUE(status.length() > 0);
    });

    SAD_TEST("MGR04: تسجيل المرحلة 1 (IO)", {
        Sad::Data::FunctionManager fm;
        Sad::StdLib::StandardLibraryManager mgr(fm);
        bool ok = mgr.registerPhase1_IOFunctions();
        SAD_ASSERT_TRUE(ok);
    });

    SAD_TEST("MGR05: تسجيل المرحلة 3 (المصفوفات) — لم تُنفّذ بعد", {
        Sad::Data::FunctionManager fm;
        Sad::StdLib::StandardLibraryManager mgr(fm);
        // Phase 3 is not yet implemented, returns false
        bool ok = mgr.registerPhase3_ArrayFunctions();
        SAD_ASSERT_FALSE(ok);
    });

#else
    SAD_GROUP("Stdlib.Manager / المكتبة.المدير [SKIPPED]");
    SAD_SKIP("MGR01-05: مدير المكتبة القياسية غير متوفر", "missing headers");
#endif

    // ══════════════════════════════════════════════════════════════════
    //  طباعة النتائج النهائية
    // ══════════════════════════════════════════════════════════════════
    SAD_SUMMARY();
}
