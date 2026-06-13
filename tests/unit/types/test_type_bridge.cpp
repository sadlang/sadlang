/**
 * @file test_type_bridge.cpp
 * @brief (AR) اختبارات وحدة لطبقة الجسر بين SadType و Value — ADR-01 Phase 1
 * @brief (EN) Unit tests for the bridge layer between SadType and Value — ADR-01 Phase 1
 *
 * ═══════════════════════════════════════════════════════════════════════════════════
 * (AR) اختبارات شاملة تغطي:
 *      ① تحويلات SadTypeKind ↔ ValueType (جميع الأنواع)
 *      ② تحويلات SadTypeKind ↔ DataType (جميع الأنواع)
 *      ③ تحويلات SadTypePtr ↔ ValueType
 *      ④ استنتاج النوع من Value (inferSadType)
 *      ⑤ فحص التوافق (isValueCompatible)
 *      ⑥ SadValue — الإنشاء والوصول للبيانات
 *      ⑦ SadValue::fromLegacy() و toLegacy() — الرحلة الكاملة ذهاباً وإياباً
 *      ⑧ أدوات sad_type_utils.h — الدوال المختصرة
 *      ⑨ حالات حافّة (null, empty arrays, nested types)
 *
 *      الهدف: 60+ اختبار — تغطية كاملة لجميع مسارات التحويل
 *
 * (EN) Comprehensive tests covering:
 *      ① SadTypeKind ↔ ValueType conversions (all types)
 *      ② SadTypeKind ↔ DataType conversions (all types)
 *      ③ SadTypePtr ↔ ValueType conversions
 *      ④ Type inference from Value (inferSadType)
 *      ⑤ Compatibility checking (isValueCompatible)
 *      ⑥ SadValue — creation and data access
 *      ⑦ SadValue::fromLegacy() and toLegacy() — full round-trip
 *      ⑧ sad_type_utils.h utilities — shortcut functions
 *      ⑨ Edge cases (null, empty arrays, nested types)
 *
 *      Target: 60+ tests — full coverage of all conversion paths
 * ═══════════════════════════════════════════════════════════════════════════════════
 *
 * @author فريق تطوير لغة ص / S Language Development Team
 * @date يناير 2026 / January 2026
 */

#include <iostream>
#include <cassert>
#include <string>
#include <memory>
#include <vector>
#include <cmath>

// (AR) إلغاء تعريفات ماكروز ويندوز قبل أي شيء
#ifdef VOID
#undef VOID
#endif
#ifdef ERROR
#undef ERROR
#endif

#include "type_bridge.h"
#include "sad_value.h"
#include "sad_type_utils.h"
#include "value.h"
#include "object_instance.h"
#include "function_ref.h"

// ═══════════════════════════════════════════════════════════════════════════════════
// (AR) ماكروهات الاختبار / (EN) Test Macros
// ═══════════════════════════════════════════════════════════════════════════════════

static int g_testCount = 0;
static int g_passCount = 0;
static int g_failCount = 0;

#define TEST_START(name) \
    g_testCount++;       \
    std::cout << "  اختبار " << g_testCount << ": " << name;

#define TEST_PASS() \
    g_passCount++;  \
    std::cout << " ✓" << std::endl;

#define TEST_FAIL(msg) \
    g_failCount++;     \
    std::cout << " ✗ — " << msg << std::endl;

// (AR) ماكرو للطباعة الآمنة — يتعامل مع enum class عبر static_cast<int>
namespace _detail
{
    template <typename T>
    auto toPrintable(const T &v) -> typename std::enable_if<std::is_enum<T>::value, int>::type
    {
        return static_cast<int>(v);
    }
    template <typename T>
    auto toPrintable(const T &v) -> typename std::enable_if<!std::is_enum<T>::value, const T &>::type
    {
        return v;
    }
}

#define ASSERT_EQ(actual, expected, msg)                                    \
    if ((actual) != (expected))                                             \
    {                                                                       \
        TEST_FAIL(msg);                                                     \
        std::cerr << "    متوقع: " << _detail::toPrintable(expected)        \
                  << " وُجد: " << _detail::toPrintable(actual) << std::endl; \
        return;                                                             \
    }

#define ASSERT_TRUE_MSG(cond, msg) \
    if (!(cond))                   \
    {                              \
        TEST_FAIL(msg);            \
        return;                    \
    }

#define ASSERT_NOT_NULL(ptr, msg) \
    if (!(ptr))                   \
    {                             \
        TEST_FAIL(msg);           \
        return;                   \
    }

using namespace Sad::Types;
using namespace Sad::Data;

// █████████████████████████████████████████████████████████████████████████████████
//
//  ① اختبارات SadTypeKind ↔ ValueType
//
// █████████████████████████████████████████████████████████████████████████████████

void test_kind_to_valuetype_primitives()
{
    TEST_START("SadTypeKind → ValueType: الأنواع البدائية");

    ASSERT_EQ(toValueType(SadTypeKind::Void), ValueType::VOID, "Void");
    ASSERT_EQ(toValueType(SadTypeKind::Integer), ValueType::INTEGER, "Integer");
    ASSERT_EQ(toValueType(SadTypeKind::Float), ValueType::DOUBLE, "Float");
    ASSERT_EQ(toValueType(SadTypeKind::Boolean), ValueType::BOOLEAN, "Boolean");
    ASSERT_EQ(toValueType(SadTypeKind::String), ValueType::STRING, "String");
    ASSERT_EQ(toValueType(SadTypeKind::Byte), ValueType::INTEGER, "Byte→INTEGER");

    TEST_PASS();
}

void test_kind_to_valuetype_composite()
{
    TEST_START("SadTypeKind → ValueType: الأنواع المركبة");

    ASSERT_EQ(toValueType(SadTypeKind::Array), ValueType::ARRAY, "Array");
    ASSERT_EQ(toValueType(SadTypeKind::Map), ValueType::MAP, "Map");
    ASSERT_EQ(toValueType(SadTypeKind::Tuple), ValueType::TUPLE, "Tuple");
    ASSERT_EQ(toValueType(SadTypeKind::Function), ValueType::FUNCTION, "Function");
    ASSERT_EQ(toValueType(SadTypeKind::Closure), ValueType::FUNCTION, "Closure→FUNCTION");
    ASSERT_EQ(toValueType(SadTypeKind::Class), ValueType::OBJECT, "Class→OBJECT");
    ASSERT_EQ(toValueType(SadTypeKind::Struct), ValueType::OBJECT, "Struct→OBJECT");

    TEST_PASS();
}

void test_kind_to_valuetype_sized_integers()
{
    TEST_START("SadTypeKind → ValueType: الأعداد بحجم محدد");

    ASSERT_EQ(toValueType(SadTypeKind::Int8), ValueType::INTEGER, "Int8→INTEGER");
    ASSERT_EQ(toValueType(SadTypeKind::Int16), ValueType::INTEGER, "Int16→INTEGER");
    ASSERT_EQ(toValueType(SadTypeKind::Int32), ValueType::INTEGER, "Int32→INTEGER");
    ASSERT_EQ(toValueType(SadTypeKind::Int64), ValueType::INTEGER, "Int64→INTEGER");
    ASSERT_EQ(toValueType(SadTypeKind::UInt8), ValueType::INTEGER, "UInt8→INTEGER");
    ASSERT_EQ(toValueType(SadTypeKind::UInt16), ValueType::INTEGER, "UInt16→INTEGER");
    ASSERT_EQ(toValueType(SadTypeKind::UInt32), ValueType::INTEGER, "UInt32→INTEGER");
    ASSERT_EQ(toValueType(SadTypeKind::UInt64), ValueType::INTEGER, "UInt64→INTEGER");
    ASSERT_EQ(toValueType(SadTypeKind::Float32), ValueType::DOUBLE, "Float32→DOUBLE");
    ASSERT_EQ(toValueType(SadTypeKind::Float64), ValueType::DOUBLE, "Float64→DOUBLE");

    TEST_PASS();
}

void test_valuetype_to_kind()
{
    TEST_START("ValueType → SadTypeKind: جميع القيم");

    ASSERT_EQ(fromValueType(ValueType::VOID), SadTypeKind::Void, "VOID");
    ASSERT_EQ(fromValueType(ValueType::INTEGER), SadTypeKind::Integer, "INTEGER");
    ASSERT_EQ(fromValueType(ValueType::DOUBLE), SadTypeKind::Float, "DOUBLE→Float");
    ASSERT_EQ(fromValueType(ValueType::STRING), SadTypeKind::String, "STRING");
    ASSERT_EQ(fromValueType(ValueType::BOOLEAN), SadTypeKind::Boolean, "BOOLEAN");
    ASSERT_EQ(fromValueType(ValueType::ARRAY), SadTypeKind::Array, "ARRAY");
    ASSERT_EQ(fromValueType(ValueType::MAP), SadTypeKind::Map, "MAP");
    ASSERT_EQ(fromValueType(ValueType::TUPLE), SadTypeKind::Tuple, "TUPLE");
    ASSERT_EQ(fromValueType(ValueType::FUNCTION), SadTypeKind::Function, "FUNCTION");
    ASSERT_EQ(fromValueType(ValueType::OBJECT), SadTypeKind::Class, "OBJECT→Class");

    TEST_PASS();
}

void test_kind_valuetype_roundtrip()
{
    TEST_START("SadTypeKind → ValueType → SadTypeKind: رحلة ذهاب وإياب");

    // (AR) الأنواع التي لها مقابل دقيق في كلا الاتجاهين
    auto roundtrip = [](SadTypeKind kind)
    {
        return fromValueType(toValueType(kind));
    };

    ASSERT_EQ(roundtrip(SadTypeKind::Void), SadTypeKind::Void, "Void roundtrip");
    ASSERT_EQ(roundtrip(SadTypeKind::Integer), SadTypeKind::Integer, "Integer roundtrip");
    ASSERT_EQ(roundtrip(SadTypeKind::Float), SadTypeKind::Float, "Float roundtrip");
    ASSERT_EQ(roundtrip(SadTypeKind::Boolean), SadTypeKind::Boolean, "Boolean roundtrip");
    ASSERT_EQ(roundtrip(SadTypeKind::String), SadTypeKind::String, "String roundtrip");
    ASSERT_EQ(roundtrip(SadTypeKind::Array), SadTypeKind::Array, "Array roundtrip");
    ASSERT_EQ(roundtrip(SadTypeKind::Map), SadTypeKind::Map, "Map roundtrip");
    ASSERT_EQ(roundtrip(SadTypeKind::Tuple), SadTypeKind::Tuple, "Tuple roundtrip");

    TEST_PASS();
}

// █████████████████████████████████████████████████████████████████████████████████
//
//  ② اختبارات نوع النتيجة Result<T, E> (S-TS-P3)
//  (AR) DataType أُزيل نهائيًّا (S-TS-P2.5b)؛ هذا القسم يختبر النوع الجبري الجديد
//       SadResultType: البناء، الأنواع الداخلية، المساواة، وقابلية الإسناد (التغايُر).
//
// █████████████████████████████████████████████████████████████████████████████████

void test_result_construct()
{
    TEST_START("Result<T,E>: البناء واستخراج الأنواع الداخلية");

    auto &reg = SadTypeRegistry::instance();
    auto res = reg.makeResult(reg.getInteger(), reg.getString()); // نتيجة<رقم، نص>

    ASSERT_NOT_NULL(res, "makeResult أرجع nullptr");
    ASSERT_EQ(res->getKind(), SadTypeKind::Result, "kind = Result");

    auto rt = std::static_pointer_cast<const SadResultType>(res);
    ASSERT_NOT_NULL(rt->getOkType(), "نوع النجاح غير موجود");
    ASSERT_NOT_NULL(rt->getErrType(), "نوع الخطأ غير موجود");
    ASSERT_EQ(rt->getOkType()->getKind(), SadTypeKind::Integer, "okType = Integer");
    ASSERT_EQ(rt->getErrType()->getKind(), SadTypeKind::String, "errType = String");

    TEST_PASS();
}

void test_result_equals_and_assign()
{
    TEST_START("Result<T,E>: المساواة وقابلية الإسناد");

    auto &reg = SadTypeRegistry::instance();
    auto r1 = reg.makeResult(reg.getInteger(), reg.getString());
    auto r2 = reg.makeResult(reg.getInteger(), reg.getString());
    auto r3 = reg.makeResult(reg.getString(), reg.getString()); // نوع نجاح مختلف

    // المساواة البنيوية: r1 == r2، r1 != r3
    ASSERT_TRUE_MSG(r1->equals(r2.get()), "نتيجتان متطابقتان لم تتساويا");
    ASSERT_TRUE_MSG(!r1->equals(r3.get()), "نتيجتان مختلفتان تساوتا خطأً");

    // قابلية الإسناد: نفس النوع يُسنَد، والمختلف لا يُسنَد
    ASSERT_TRUE_MSG(r1->isAssignableTo(r2.get()), "Result متطابق غير قابل للإسناد");
    ASSERT_TRUE_MSG(!r1->isAssignableTo(r3.get()), "Result مختلف قُبِل إسناده خطأً");

    // الإسناد إلى Any مسموح دائمًا
    ASSERT_TRUE_MSG(r1->isAssignableTo(reg.getAny().get()), "Result غير قابل للإسناد إلى أي");

    TEST_PASS();
}

// █████████████████████████████████████████████████████████████████████████████████
//
//  ③ اختبارات SadTypePtr ↔ ValueType
//
// █████████████████████████████████████████████████████████████████████████████████

void test_sadtype_ptr_to_valuetype()
{
    TEST_START("SadTypePtr → ValueType: مؤشرات ذكية");

    auto &reg = SadTypeRegistry::instance();

    ASSERT_EQ(sadTypeToValueType(reg.getInteger()), ValueType::INTEGER, "Integer ptr");
    ASSERT_EQ(sadTypeToValueType(reg.getFloat()), ValueType::DOUBLE, "Float ptr");
    ASSERT_EQ(sadTypeToValueType(reg.getString()), ValueType::STRING, "String ptr");
    ASSERT_EQ(sadTypeToValueType(reg.getBoolean()), ValueType::BOOLEAN, "Boolean ptr");
    ASSERT_EQ(sadTypeToValueType(reg.getVoid()), ValueType::VOID, "Void ptr");
    ASSERT_EQ(sadTypeToValueType(nullptr), ValueType::VOID, "nullptr→VOID");

    TEST_PASS();
}

void test_sadtype_ptr_from_valuetype()
{
    TEST_START("ValueType → SadTypePtr: إنشاء من تعداد");

    auto intType = sadTypeFromValueType(ValueType::INTEGER);
    ASSERT_NOT_NULL(intType, "Integer type");
    ASSERT_EQ(intType->getKind(), SadTypeKind::Integer, "Integer kind");

    auto strType = sadTypeFromValueType(ValueType::STRING);
    ASSERT_NOT_NULL(strType, "String type");
    ASSERT_EQ(strType->getKind(), SadTypeKind::String, "String kind");

    auto arrType = sadTypeFromValueType(ValueType::ARRAY);
    ASSERT_NOT_NULL(arrType, "Array type");
    ASSERT_EQ(arrType->getKind(), SadTypeKind::Array, "Array kind");

    TEST_PASS();
}

// █████████████████████████████████████████████████████████████████████████████████
//
//  ④ اختبارات inferSadType (استنتاج النوع من Value)
//
// █████████████████████████████████████████████████████████████████████████████████

void test_infer_primitives()
{
    TEST_START("inferSadType: الأنواع البدائية");

    auto voidType = inferSadType(Value());
    ASSERT_NOT_NULL(voidType, "void inference");
    ASSERT_EQ(voidType->getKind(), SadTypeKind::Void, "void kind");

    auto intType = inferSadType(Value(int64_t(42)));
    ASSERT_NOT_NULL(intType, "int inference");
    ASSERT_EQ(intType->getKind(), SadTypeKind::Integer, "int kind");

    auto dblType = inferSadType(Value(3.14));
    ASSERT_NOT_NULL(dblType, "double inference");
    ASSERT_EQ(dblType->getKind(), SadTypeKind::Float, "double kind");

    auto strType = inferSadType(Value(std::string("مرحبا")));
    ASSERT_NOT_NULL(strType, "string inference");
    ASSERT_EQ(strType->getKind(), SadTypeKind::String, "string kind");

    auto boolType = inferSadType(Value(true));
    ASSERT_NOT_NULL(boolType, "bool inference");
    ASSERT_EQ(boolType->getKind(), SadTypeKind::Boolean, "bool kind");

    TEST_PASS();
}

void test_infer_array()
{
    TEST_START("inferSadType: مصفوفة");

    // (AR) مصفوفة أرقام
    Value::ArrayType arr;
    arr.push_back(Value(int64_t(1)));
    arr.push_back(Value(int64_t(2)));
    arr.push_back(Value(int64_t(3)));
    auto arrType = inferSadType(Value(std::move(arr)));

    ASSERT_NOT_NULL(arrType, "array inference");
    ASSERT_EQ(arrType->getKind(), SadTypeKind::Array, "array kind");

    // (AR) فحص نوع العنصر
    auto arrCast = dynamic_cast<const SadArrayType *>(arrType.get());
    ASSERT_NOT_NULL(arrCast, "array cast");
    ASSERT_NOT_NULL(arrCast->getElementType(), "element type");
    ASSERT_EQ(arrCast->getElementType()->getKind(), SadTypeKind::Integer, "element is Integer");

    TEST_PASS();
}

void test_infer_empty_array()
{
    TEST_START("inferSadType: مصفوفة فارغة");

    Value::ArrayType emptyArr;
    auto arrType = inferSadType(Value(std::move(emptyArr)));

    ASSERT_NOT_NULL(arrType, "empty array inference");
    ASSERT_EQ(arrType->getKind(), SadTypeKind::Array, "array kind");

    // (AR) بدون عناصر — لا يمكن استنتاج نوع العنصر
    auto arrCast = dynamic_cast<const SadArrayType *>(arrType.get());
    ASSERT_NOT_NULL(arrCast, "array cast");
    // (AR) نوع العنصر يمكن أن يكون null (مصفوفة بدون نوع)

    TEST_PASS();
}

void test_infer_map()
{
    TEST_START("inferSadType: خريطة");

    Value::MapType map;
    map["اسم"] = Value(std::string("أحمد"));
    map["عمر"] = Value(int64_t(25));
    auto mapType = inferSadType(Value(std::move(map)));

    ASSERT_NOT_NULL(mapType, "map inference");
    ASSERT_EQ(mapType->getKind(), SadTypeKind::Map, "map kind");

    TEST_PASS();
}

void test_infer_tuple()
{
    TEST_START("inferSadType: صف");

    Value::TupleType tup;
    tup.push_back(Value(int64_t(1)));
    tup.push_back(Value(std::string("نص")));
    tup.push_back(Value(true));
    auto tupType = inferSadType(Value(Value::TupleTag{}, std::move(tup)));

    ASSERT_NOT_NULL(tupType, "tuple inference");
    ASSERT_EQ(tupType->getKind(), SadTypeKind::Tuple, "tuple kind");

    // (AR) فحص أنواع العناصر
    auto tupCast = dynamic_cast<const SadTupleType *>(tupType.get());
    ASSERT_NOT_NULL(tupCast, "tuple cast");
    ASSERT_EQ(tupCast->arity(), size_t(3), "tuple arity");
    ASSERT_EQ(tupCast->getElements()[0]->getKind(), SadTypeKind::Integer, "elem 0 = int");
    ASSERT_EQ(tupCast->getElements()[1]->getKind(), SadTypeKind::String, "elem 1 = string");
    ASSERT_EQ(tupCast->getElements()[2]->getKind(), SadTypeKind::Boolean, "elem 2 = bool");

    TEST_PASS();
}

// █████████████████████████████████████████████████████████████████████████████████
//
//  ⑤ اختبارات isValueCompatible
//
// █████████████████████████████████████████████████████████████████████████████████

void test_compatibility_basic()
{
    TEST_START("isValueCompatible: التوافق الأساسي");

    auto &reg = SadTypeRegistry::instance();

    // (AR) رقم متوافق مع رقم
    ASSERT_TRUE_MSG(isValueCompatible(Value(int64_t(42)), reg.getInteger()), "int compat int");

    // (AR) نص متوافق مع نص
    ASSERT_TRUE_MSG(isValueCompatible(Value(std::string("مرحبا")), reg.getString()), "str compat str");

    // (AR) منطقي متوافق مع منطقي
    ASSERT_TRUE_MSG(isValueCompatible(Value(true), reg.getBoolean()), "bool compat bool");

    // (AR) رقم متوافق مع عشري (تحويل ضمني)
    ASSERT_TRUE_MSG(isValueCompatible(Value(int64_t(42)), reg.getFloat()), "int coerces float");

    TEST_PASS();
}

void test_compatibility_null_type()
{
    TEST_START("isValueCompatible: نوع null");

    // (AR) نوع متوقع null — يسمح بكل شيء
    ASSERT_TRUE_MSG(isValueCompatible(Value(int64_t(42)), nullptr), "null expected = allow");

    TEST_PASS();
}

// █████████████████████████████████████████████████████████████████████████████████
//
//  ⑥ اختبارات SadValue — الإنشاء والوصول
//
// █████████████████████████████████████████████████████████████████████████████████

void test_sadvalue_void()
{
    TEST_START("SadValue: فراغ");

    SadValue v;
    ASSERT_TRUE_MSG(v.isVoid(), "is void");
    ASSERT_EQ(v.getKind(), SadTypeKind::Void, "kind is Void");
    ASSERT_EQ(v.toString(), std::string("لاشيء"), "toString = لاشيء");

    TEST_PASS();
}

void test_sadvalue_integer()
{
    TEST_START("SadValue: رقم صحيح");

    SadValue v(int64_t(42));
    ASSERT_TRUE_MSG(v.isInteger(), "is integer");
    ASSERT_EQ(v.toInt64(), int64_t(42), "value is 42");
    ASSERT_EQ(v.toInt(), 42, "toInt = 42");
    ASSERT_TRUE_MSG(v.isNumeric(), "is numeric");
    ASSERT_EQ(v.typeName(), std::string("رقم"), "typeName = رقم");

    TEST_PASS();
}

void test_sadvalue_float()
{
    TEST_START("SadValue: عشري");

    SadValue v(3.14);
    ASSERT_TRUE_MSG(v.isFloat(), "is float");
    ASSERT_TRUE_MSG(std::abs(v.toDouble() - 3.14) < 0.001, "value ≈ 3.14");
    ASSERT_TRUE_MSG(v.isNumeric(), "is numeric");
    ASSERT_EQ(v.typeName(), std::string("عشري"), "typeName = عشري");

    TEST_PASS();
}

void test_sadvalue_string()
{
    TEST_START("SadValue: نص");

    SadValue v(std::string("مرحبا"));
    ASSERT_TRUE_MSG(v.isString(), "is string");
    ASSERT_EQ(v.toStringRef(), std::string("مرحبا"), "value = مرحبا");
    ASSERT_EQ(v.typeName(), std::string("نص"), "typeName = نص");

    TEST_PASS();
}

void test_sadvalue_boolean()
{
    TEST_START("SadValue: منطقي");

    SadValue t(true);
    SadValue f(false);
    ASSERT_TRUE_MSG(t.isBoolean(), "true is boolean");
    ASSERT_TRUE_MSG(t.toBool(), "true = true");
    ASSERT_TRUE_MSG(!f.toBool(), "false = false");

    TEST_PASS();
}

void test_sadvalue_cstring()
{
    TEST_START("SadValue: C-string");

    SadValue v("اختبار");
    ASSERT_TRUE_MSG(v.isString(), "C-string → string");
    ASSERT_EQ(v.toStringRef(), std::string("اختبار"), "value = اختبار");

    TEST_PASS();
}

void test_sadvalue_int_promote()
{
    TEST_START("SadValue: int → int64_t ترقية");

    SadValue v(7);
    ASSERT_TRUE_MSG(v.isInteger(), "int promoted to integer");
    ASSERT_EQ(v.toInt64(), int64_t(7), "value = 7");

    TEST_PASS();
}

void test_sadvalue_array()
{
    TEST_START("SadValue: مصفوفة");

    SadValue::ArrayType arr;
    arr.push_back(SadValue(int64_t(10)));
    arr.push_back(SadValue(int64_t(20)));
    arr.push_back(SadValue(int64_t(30)));

    SadValue v(std::move(arr));
    ASSERT_TRUE_MSG(v.isArray(), "is array");
    ASSERT_EQ(v.toArrayRef().size(), size_t(3), "size = 3");
    ASSERT_EQ(v.toArrayRef()[0].toInt64(), int64_t(10), "elem 0 = 10");
    ASSERT_EQ(v.toArrayRef()[2].toInt64(), int64_t(30), "elem 2 = 30");

    TEST_PASS();
}

void test_sadvalue_map()
{
    TEST_START("SadValue: خريطة");

    SadValue::MapType map;
    map["مفتاح"] = SadValue(std::string("قيمة"));
    map["رقم"] = SadValue(int64_t(99));

    SadValue v(std::move(map));
    ASSERT_TRUE_MSG(v.isMap(), "is map");
    ASSERT_EQ(v.toMapRef().size(), size_t(2), "size = 2");

    TEST_PASS();
}

void test_sadvalue_tuple()
{
    TEST_START("SadValue: صف");

    SadValue::TupleType tup({SadValue(int64_t(1)), SadValue("نص"), SadValue(true)});
    SadValue v(std::move(tup));
    ASSERT_TRUE_MSG(v.isTuple(), "is tuple");
    ASSERT_EQ(v.toTupleRef().size(), size_t(3), "size = 3");

    TEST_PASS();
}

void test_sadvalue_equality()
{
    TEST_START("SadValue: المقارنة ==");

    ASSERT_TRUE_MSG(SadValue(int64_t(42)) == SadValue(int64_t(42)), "int == int");
    ASSERT_TRUE_MSG(SadValue("مرحبا") == SadValue("مرحبا"), "str == str");
    ASSERT_TRUE_MSG(SadValue(true) == SadValue(true), "bool == bool");
    ASSERT_TRUE_MSG(SadValue(int64_t(1)) != SadValue(int64_t(2)), "1 != 2");
    ASSERT_TRUE_MSG(SadValue(int64_t(42)) != SadValue(42.0), "int != double (different kind)");

    TEST_PASS();
}

void test_sadvalue_tobool_coercion()
{
    TEST_START("SadValue::toBool: التحويل الضمني");

    // (AR) 0 = خطأ
    ASSERT_TRUE_MSG(!SadValue(int64_t(0)).toBool(), "0 = false");
    // (AR) غير 0 = صحيح
    ASSERT_TRUE_MSG(SadValue(int64_t(1)).toBool(), "1 = true");
    // (AR) نص فارغ = خطأ
    ASSERT_TRUE_MSG(!SadValue(std::string("")).toBool(), "\"\" = false");
    // (AR) نص غير فارغ = صحيح
    ASSERT_TRUE_MSG(SadValue(std::string("أ")).toBool(), "\"أ\" = true");
    // (AR) void = خطأ
    ASSERT_TRUE_MSG(!SadValue().toBool(), "void = false");
    // (AR) 0.0 = خطأ
    ASSERT_TRUE_MSG(!SadValue(0.0).toBool(), "0.0 = false");

    TEST_PASS();
}

// █████████████████████████████████████████████████████████████████████████████████
//
//  ⑦ اختبارات SadValue::fromLegacy() و toLegacy()
//
// █████████████████████████████████████████████████████████████████████████████████

void test_roundtrip_void()
{
    TEST_START("roundtrip: Void — ذهاب وإياب");

    Value legacy;
    SadValue modern = SadValue::fromLegacy(legacy);
    ASSERT_TRUE_MSG(modern.isVoid(), "fromLegacy → void");

    Value back = modern.toLegacy();
    ASSERT_EQ(back.getType(), ValueType::VOID, "toLegacy → VOID");

    TEST_PASS();
}

void test_roundtrip_integer()
{
    TEST_START("roundtrip: رقم صحيح — ذهاب وإياب");

    Value legacy(int64_t(42));
    SadValue modern = SadValue::fromLegacy(legacy);
    ASSERT_TRUE_MSG(modern.isInteger(), "fromLegacy → integer");
    ASSERT_EQ(modern.toInt64(), int64_t(42), "value = 42");

    Value back = modern.toLegacy();
    ASSERT_EQ(back.getType(), ValueType::INTEGER, "toLegacy → INTEGER");
    ASSERT_EQ(back.toInt64(), int64_t(42), "back value = 42");

    TEST_PASS();
}

void test_roundtrip_double()
{
    TEST_START("roundtrip: عشري — ذهاب وإياب");

    Value legacy(3.14);
    SadValue modern = SadValue::fromLegacy(legacy);
    ASSERT_TRUE_MSG(modern.isFloat(), "fromLegacy → float");
    ASSERT_TRUE_MSG(std::abs(modern.toDouble() - 3.14) < 0.001, "value ≈ 3.14");

    Value back = modern.toLegacy();
    ASSERT_EQ(back.getType(), ValueType::DOUBLE, "toLegacy → DOUBLE");
    ASSERT_TRUE_MSG(std::abs(back.toDouble() - 3.14) < 0.001, "back value ≈ 3.14");

    TEST_PASS();
}

void test_roundtrip_string()
{
    TEST_START("roundtrip: نص — ذهاب وإياب");

    Value legacy(std::string("مرحبا بالعالم"));
    SadValue modern = SadValue::fromLegacy(legacy);
    ASSERT_TRUE_MSG(modern.isString(), "fromLegacy → string");
    ASSERT_EQ(modern.toStringRef(), std::string("مرحبا بالعالم"), "value retained");

    Value back = modern.toLegacy();
    ASSERT_EQ(back.getType(), ValueType::STRING, "toLegacy → STRING");
    ASSERT_EQ(back.toStringRef(), std::string("مرحبا بالعالم"), "back value retained");

    TEST_PASS();
}

void test_roundtrip_boolean()
{
    TEST_START("roundtrip: منطقي — ذهاب وإياب");

    Value legacyTrue(true);
    SadValue modernTrue = SadValue::fromLegacy(legacyTrue);
    ASSERT_TRUE_MSG(modernTrue.isBoolean(), "fromLegacy → boolean");
    ASSERT_TRUE_MSG(modernTrue.toBool(), "value = true");

    Value backTrue = modernTrue.toLegacy();
    ASSERT_EQ(backTrue.getType(), ValueType::BOOLEAN, "toLegacy → BOOLEAN");
    ASSERT_TRUE_MSG(backTrue.toBool(), "back value = true");

    TEST_PASS();
}

void test_roundtrip_array()
{
    TEST_START("roundtrip: مصفوفة — ذهاب وإياب");

    Value::ArrayType arr;
    arr.push_back(Value(int64_t(10)));
    arr.push_back(Value(int64_t(20)));
    arr.push_back(Value(int64_t(30)));

    Value legacy(std::move(arr));
    SadValue modern = SadValue::fromLegacy(legacy);
    ASSERT_TRUE_MSG(modern.isArray(), "fromLegacy → array");
    ASSERT_EQ(modern.toArrayRef().size(), size_t(3), "size = 3");
    ASSERT_EQ(modern.toArrayRef()[1].toInt64(), int64_t(20), "elem 1 = 20");

    Value back = modern.toLegacy();
    ASSERT_EQ(back.getType(), ValueType::ARRAY, "toLegacy → ARRAY");
    ASSERT_EQ(back.toArrayRef().size(), size_t(3), "back size = 3");
    ASSERT_EQ(back.toArrayRef()[1].toInt64(), int64_t(20), "back elem 1 = 20");

    TEST_PASS();
}

void test_roundtrip_map()
{
    TEST_START("roundtrip: خريطة — ذهاب وإياب");

    Value::MapType map;
    map["اسم"] = Value(std::string("أحمد"));
    map["عمر"] = Value(int64_t(25));

    Value legacy(std::move(map));
    SadValue modern = SadValue::fromLegacy(legacy);
    ASSERT_TRUE_MSG(modern.isMap(), "fromLegacy → map");
    ASSERT_EQ(modern.toMapRef().size(), size_t(2), "size = 2");

    Value back = modern.toLegacy();
    ASSERT_EQ(back.getType(), ValueType::MAP, "toLegacy → MAP");
    ASSERT_EQ(back.toMapRef().size(), size_t(2), "back size = 2");
    ASSERT_EQ(back.toMapRef().at("اسم").toStringRef(), std::string("أحمد"), "back اسم = أحمد");

    TEST_PASS();
}

void test_roundtrip_tuple()
{
    TEST_START("roundtrip: صف — ذهاب وإياب");

    Value::TupleType tup;
    tup.push_back(Value(int64_t(1)));
    tup.push_back(Value(std::string("نص")));

    Value legacy(Value::TupleTag{}, std::move(tup));
    SadValue modern = SadValue::fromLegacy(legacy);
    ASSERT_TRUE_MSG(modern.isTuple(), "fromLegacy → tuple");
    ASSERT_EQ(modern.toTupleRef().size(), size_t(2), "size = 2");

    Value back = modern.toLegacy();
    ASSERT_EQ(back.getType(), ValueType::TUPLE, "toLegacy → TUPLE");
    ASSERT_EQ(back.toTupleRef().size(), size_t(2), "back size = 2");

    TEST_PASS();
}

// █████████████████████████████████████████████████████████████████████████████████
//
//  ⑧ اختبارات sad_type_utils.h — الدوال المختصرة
//
// █████████████████████████████████████████████████████████████████████████████████

void test_utils_shortcuts()
{
    TEST_START("Utils: اختصارات الأنواع البدائية");

    ASSERT_NOT_NULL(sadInteger(), "sadInteger()");
    ASSERT_EQ(sadInteger()->getKind(), SadTypeKind::Integer, "sadInteger kind");

    ASSERT_NOT_NULL(sadFloat(), "sadFloat()");
    ASSERT_EQ(sadFloat()->getKind(), SadTypeKind::Float, "sadFloat kind");

    ASSERT_NOT_NULL(sadString(), "sadString()");
    ASSERT_EQ(sadString()->getKind(), SadTypeKind::String, "sadString kind");

    ASSERT_NOT_NULL(sadBoolean(), "sadBoolean()");
    ASSERT_EQ(sadBoolean()->getKind(), SadTypeKind::Boolean, "sadBoolean kind");

    ASSERT_NOT_NULL(sadVoid(), "sadVoid()");
    ASSERT_EQ(sadVoid()->getKind(), SadTypeKind::Void, "sadVoid kind");

    ASSERT_NOT_NULL(sadAny(), "sadAny()");
    ASSERT_EQ(sadAny()->getKind(), SadTypeKind::Any, "sadAny kind");

    TEST_PASS();
}

void test_utils_composite_shortcuts()
{
    TEST_START("Utils: اختصارات الأنواع المركبة");

    auto arr = sadArray(sadInteger());
    ASSERT_NOT_NULL(arr, "sadArray(sadInteger())");
    ASSERT_EQ(arr->getKind(), SadTypeKind::Array, "array kind");

    auto map = sadMap(sadString(), sadInteger());
    ASSERT_NOT_NULL(map, "sadMap(sadString(), sadInteger())");
    ASSERT_EQ(map->getKind(), SadTypeKind::Map, "map kind");

    auto tup = sadTuple({sadInteger(), sadString()});
    ASSERT_NOT_NULL(tup, "sadTuple({int, str})");
    ASSERT_EQ(tup->getKind(), SadTypeKind::Tuple, "tuple kind");

    auto func = sadFunction({sadInteger(), sadString()}, sadBoolean());
    ASSERT_NOT_NULL(func, "sadFunction({int, str}, bool)");
    ASSERT_EQ(func->getKind(), SadTypeKind::Function, "function kind");

    auto cls = sadClass("شخص");
    ASSERT_NOT_NULL(cls, "sadClass(شخص)");
    ASSERT_EQ(cls->getKind(), SadTypeKind::Class, "class kind");

    TEST_PASS();
}

void test_utils_sadIs()
{
    TEST_START("Utils: sadIs() و sadIsAnyOf()");

    auto intType = sadInteger();
    auto strType = sadString();

    ASSERT_TRUE_MSG(sadIs(intType, SadTypeKind::Integer), "sadIs(int, Integer)");
    ASSERT_TRUE_MSG(!sadIs(intType, SadTypeKind::String), "!sadIs(int, String)");
    ASSERT_TRUE_MSG(!sadIs(nullptr, SadTypeKind::Integer), "sadIs(null, Integer) = false");

    ASSERT_TRUE_MSG(sadIsAnyOf(intType, SadTypeKind::Integer, SadTypeKind::Float), "sadIsAnyOf int∈{int,float}");
    ASSERT_TRUE_MSG(!sadIsAnyOf(strType, SadTypeKind::Integer, SadTypeKind::Float), "!sadIsAnyOf str∈{int,float}");

    TEST_PASS();
}

void test_utils_error_messages()
{
    TEST_START("Utils: رسائل الأخطاء");

    auto msg1 = makeTypeMismatchError(sadInteger(), sadString(), "المعامل الأول");
    ASSERT_TRUE_MSG(msg1.find("رقم") != std::string::npos, "mismatch msg has رقم");
    ASSERT_TRUE_MSG(msg1.find("نص") != std::string::npos, "mismatch msg has نص");

    auto msg2 = makeUnsupportedOpError("جمع", sadBoolean());
    ASSERT_TRUE_MSG(msg2.find("جمع") != std::string::npos, "unsupported has جمع");
    ASSERT_TRUE_MSG(msg2.find("منطقي") != std::string::npos, "unsupported has منطقي");

    auto msg3 = makeConversionError(sadString(), sadInteger());
    ASSERT_TRUE_MSG(msg3.find("نص") != std::string::npos, "conversion has نص");
    ASSERT_TRUE_MSG(msg3.find("رقم") != std::string::npos, "conversion has رقم");

    TEST_PASS();
}

void test_utils_type_helpers()
{
    TEST_START("Utils: typesEqual, typeAssignableTo, describeType");

    ASSERT_TRUE_MSG(typesEqual(sadInteger(), sadInteger()), "int == int");
    ASSERT_TRUE_MSG(!typesEqual(sadInteger(), sadString()), "int != string");
    ASSERT_TRUE_MSG(typesEqual(nullptr, nullptr), "null == null");

    ASSERT_TRUE_MSG(typeAssignableTo(sadInteger(), sadInteger()), "int → int");
    ASSERT_TRUE_MSG(typeAssignableTo(nullptr, sadInteger()), "null → int (allow)");

    ASSERT_EQ(describeType(sadInteger()), std::string("رقم"), "describe int = رقم");
    ASSERT_EQ(describeType(nullptr), std::string("مجهول"), "describe null = مجهول");
    ASSERT_EQ(describeTypeEn(sadString()), std::string("String"), "describeEn string = String");

    TEST_PASS();
}

void test_utils_typeOrDefault()
{
    TEST_START("Utils: typeOrDefault");

    auto t1 = typeOrDefault(sadInteger(), sadString());
    ASSERT_EQ(t1->getKind(), SadTypeKind::Integer, "non-null returns itself");

    auto t2 = typeOrDefault(nullptr, sadString());
    ASSERT_EQ(t2->getKind(), SadTypeKind::String, "null returns default");

    TEST_PASS();
}

// █████████████████████████████████████████████████████████████████████████████████
//
//  ⑨ حالات حافّة
//
// █████████████████████████████████████████████████████████████████████████████████

void test_edge_value_arabic_type_name()
{
    TEST_START("Edge: valueToArabicTypeName");

    ASSERT_EQ(valueToArabicTypeName(Value(int64_t(42))), std::string("رقم"), "int = رقم");
    ASSERT_EQ(valueToArabicTypeName(Value(3.14)), std::string("عشري"), "double = عشري");
    ASSERT_EQ(valueToArabicTypeName(Value(std::string("test"))), std::string("نص"), "str = نص");
    ASSERT_EQ(valueToArabicTypeName(Value(true)), std::string("منطقي"), "bool = منطقي");
    ASSERT_EQ(valueToArabicTypeName(Value()), std::string("فراغ"), "void = فراغ");

    TEST_PASS();
}

void test_edge_value_english_type_name()
{
    TEST_START("Edge: valueToEnglishTypeName");

    ASSERT_EQ(valueToEnglishTypeName(Value(int64_t(42))), std::string("Integer"), "int = Integer");
    ASSERT_EQ(valueToEnglishTypeName(Value(3.14)), std::string("Float"), "double = Float");
    ASSERT_EQ(valueToEnglishTypeName(Value(std::string("test"))), std::string("String"), "str = String");

    TEST_PASS();
}

void test_edge_isNumeric_isString()
{
    TEST_START("Edge: isNumericType, isStringType, isPrimitiveType");

    ASSERT_TRUE_MSG(isNumericType(sadInteger()), "int is numeric");
    ASSERT_TRUE_MSG(isNumericType(sadFloat()), "float is numeric");
    ASSERT_TRUE_MSG(!isNumericType(sadString()), "string not numeric");
    ASSERT_TRUE_MSG(!isNumericType(nullptr), "null not numeric");

    ASSERT_TRUE_MSG(isStringType(sadString()), "string is string");
    ASSERT_TRUE_MSG(!isStringType(sadInteger()), "int not string");

    ASSERT_TRUE_MSG(isPrimitiveType(sadInteger()), "int is primitive");
    ASSERT_TRUE_MSG(isPrimitiveType(sadString()), "string is primitive");
    ASSERT_TRUE_MSG(!isPrimitiveType(sadArray()), "array not primitive");
    ASSERT_TRUE_MSG(!isPrimitiveType(nullptr), "null not primitive");

    TEST_PASS();
}

void test_edge_sadvalue_type_info()
{
    TEST_START("Edge: SadValue معلومات النوع الغنية");

    // (AR) مصفوفة مع نوع عنصر محدد
    SadValue::ArrayType arr;
    arr.push_back(SadValue(int64_t(1)));
    SadValue v(std::move(arr), sadInteger());
    ASSERT_TRUE_MSG(v.isArray(), "is array");
    ASSERT_TRUE_MSG(v.typeName().find("مصفوفة") != std::string::npos, "typeName has مصفوفة");
    ASSERT_TRUE_MSG(v.typeName().find("رقم") != std::string::npos, "typeName has رقم");

    // (AR) التوافق مع نوع مصفوفة<رقم>
    auto expectedArr = sadArray(sadInteger());
    ASSERT_TRUE_MSG(v.isCompatibleWith(expectedArr), "array<int> compat array<int>");

    TEST_PASS();
}

// █████████████████████████████████████████████████████████████████████████████████
//
//  نقطة الدخول / Entry Point
//
// █████████████████████████████████████████████████████████████████████████████████

int main()
{
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "  اختبارات طبقة الجسر — ADR-01 Phase 1 (type_bridge)" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;

    // ① SadTypeKind ↔ ValueType
    std::cout << "\n── ① SadTypeKind ↔ ValueType ──" << std::endl;
    test_kind_to_valuetype_primitives();
    test_kind_to_valuetype_composite();
    test_kind_to_valuetype_sized_integers();
    test_valuetype_to_kind();
    test_kind_valuetype_roundtrip();

    // ② نوع النتيجة Result<T,E> (S-TS-P3) — DataType أُزيل في S-TS-P2.5b
    std::cout << "\n── ② نوع النتيجة Result<T,E> ──" << std::endl;
    test_result_construct();
    test_result_equals_and_assign();

    // ③ SadTypePtr ↔ ValueType
    std::cout << "\n── ③ SadTypePtr ↔ ValueType ──" << std::endl;
    test_sadtype_ptr_to_valuetype();
    test_sadtype_ptr_from_valuetype();

    // ④ inferSadType
    std::cout << "\n── ④ inferSadType ──" << std::endl;
    test_infer_primitives();
    test_infer_array();
    test_infer_empty_array();
    test_infer_map();
    test_infer_tuple();

    // ⑤ isValueCompatible
    std::cout << "\n── ⑤ isValueCompatible ──" << std::endl;
    test_compatibility_basic();
    test_compatibility_null_type();

    // ⑥ SadValue
    std::cout << "\n── ⑥ SadValue ──" << std::endl;
    test_sadvalue_void();
    test_sadvalue_integer();
    test_sadvalue_float();
    test_sadvalue_string();
    test_sadvalue_boolean();
    test_sadvalue_cstring();
    test_sadvalue_int_promote();
    test_sadvalue_array();
    test_sadvalue_map();
    test_sadvalue_tuple();
    test_sadvalue_equality();
    test_sadvalue_tobool_coercion();

    // ⑦ Roundtrip
    std::cout << "\n── ⑦ Roundtrip (fromLegacy/toLegacy) ──" << std::endl;
    test_roundtrip_void();
    test_roundtrip_integer();
    test_roundtrip_double();
    test_roundtrip_string();
    test_roundtrip_boolean();
    test_roundtrip_array();
    test_roundtrip_map();
    test_roundtrip_tuple();

    // ⑧ Utils
    std::cout << "\n── ⑧ sad_type_utils.h ──" << std::endl;
    test_utils_shortcuts();
    test_utils_composite_shortcuts();
    test_utils_sadIs();
    test_utils_error_messages();
    test_utils_type_helpers();
    test_utils_typeOrDefault();

    // ⑨ Edge cases
    std::cout << "\n── ⑨ حالات حافّة ──" << std::endl;
    test_edge_value_arabic_type_name();
    test_edge_value_english_type_name();
    test_edge_isNumeric_isString();
    test_edge_sadvalue_type_info();

    // الملخص
    std::cout << "\n═══════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "  إجمالي: " << g_testCount << std::endl;
    std::cout << "  نجح:    " << g_passCount << std::endl;
    std::cout << "  فشل:    " << g_failCount << std::endl;
    std::cout << "  النسبة:  " << (g_testCount > 0 ? (100.0 * g_passCount / g_testCount) : 0) << "%" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;

    return g_failCount > 0 ? 1 : 0;
}
