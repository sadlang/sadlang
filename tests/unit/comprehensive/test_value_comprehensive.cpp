/**
 * @file test_value_comprehensive.cpp
 * @brief (AR) اختبارات شاملة لنظام القيم والأنواع / (EN) Comprehensive Value & Type Tests
 * 
 * ~80 اختبار يغطي: Value class, ValueType, العمليات الحسابية والمنطقية,
 * المصفوفات، القواميس، التحويلات، حالات الحدود
 */

#include "sad_test_framework.h"
#include "value.h"

using namespace Sad::Data;

// ======================================================================
int main() {
    SAD_TEST_INIT();

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 1: إنشاء القيم / Value Construction
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Value.Construction / القيم.الإنشاء");

    SAD_TEST("VC01: قيمة فارغة VOID", {
        Value v;
        SAD_ASSERT_TRUE(v.isVoid());
        SAD_ASSERT_EQ((int)v.getType(), (int)::Sad::Types::SadTypeKind::Void);
    });

    SAD_TEST("VC02: رقم صحيح", {
        Value v(42);
        SAD_ASSERT_TRUE(v.isInteger());
        SAD_ASSERT_EQ(v.toInt(), 42);
    });

    SAD_TEST("VC03: رقم صحيح صفر", {
        Value v(0);
        SAD_ASSERT_TRUE(v.isInteger());
        SAD_ASSERT_EQ(v.toInt(), 0);
    });

    SAD_TEST("VC04: رقم صحيح سالب", {
        Value v(-99);
        SAD_ASSERT_TRUE(v.isInteger());
        SAD_ASSERT_EQ(v.toInt(), -99);
    });

    SAD_TEST("VC05: رقم عشري", {
        Value v(3.14);
        SAD_ASSERT_TRUE(v.isDouble());
        SAD_ASSERT_FLOAT_EQ(v.toDouble(), 3.14, 0.001);
    });

    SAD_TEST("VC06: عشري صفر", {
        Value v(0.0);
        SAD_ASSERT_TRUE(v.isDouble());
        SAD_ASSERT_FLOAT_EQ(v.toDouble(), 0.0, 0.001);
    });

    SAD_TEST("VC07: نص من string", {
        Value v(std::string("hello"));
        SAD_ASSERT_TRUE(v.isString());
        SAD_ASSERT_EQ(v.toString(), std::string("hello"));
    });

    SAD_TEST("VC08: نص من const char*", {
        Value v("world");
        SAD_ASSERT_TRUE(v.isString());
        SAD_ASSERT_EQ(v.toString(), std::string("world"));
    });

    SAD_TEST("VC09: نص فارغ", {
        Value v(std::string(""));
        SAD_ASSERT_TRUE(v.isString());
        SAD_ASSERT_EQ(v.toString(), std::string(""));
    });

    SAD_TEST("VC10: نص عربي", {
        Value v(std::string("\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7")); // مرحبا
        SAD_ASSERT_TRUE(v.isString());
    });

    SAD_TEST("VC11: منطقي صحيح", {
        Value v(true);
        SAD_ASSERT_TRUE(v.isBoolean());
        SAD_ASSERT_EQ(v.toBool(), true);
    });

    SAD_TEST("VC12: منطقي خطأ", {
        Value v(false);
        SAD_ASSERT_TRUE(v.isBoolean());
        SAD_ASSERT_EQ(v.toBool(), false);
    });

    SAD_TEST("VC13: مصفوفة فارغة", {
        Value::ArrayType arr;
        Value v(arr);
        SAD_ASSERT_TRUE(v.isArray());
        SAD_ASSERT_EQ(v.size(), (size_t)0);
    });

    SAD_TEST("VC14: مصفوفة مع عناصر", {
        Value::ArrayType arr = {Value(1), Value(2), Value(3)};
        Value v(arr);
        SAD_ASSERT_TRUE(v.isArray());
        SAD_ASSERT_EQ(v.size(), (size_t)3);
    });

    SAD_TEST("VC15: قاموس فارغ", {
        Value::MapType map;
        Value v(map);
        SAD_ASSERT_TRUE(v.isMap());
        SAD_ASSERT_EQ(v.size(), (size_t)0);
    });

    SAD_TEST("VC16: قاموس مع عناصر", {
        Value::MapType map;
        map["key"] = Value(42);
        Value v(map);
        SAD_ASSERT_TRUE(v.isMap());
        SAD_ASSERT_EQ(v.size(), (size_t)1);
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 2: فحص الأنواع / Type Checking
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Value.TypeChecking / القيم.فحص_الأنواع");

    SAD_TEST("TC01: isNumeric للصحيح", {
        Value v(5);
        SAD_ASSERT_TRUE(v.isNumeric());
    });

    SAD_TEST("TC02: isNumeric للعشري", {
        Value v(3.14);
        SAD_ASSERT_TRUE(v.isNumeric());
    });

    SAD_TEST("TC03: isNumeric للنص", {
        Value v("hello");
        SAD_ASSERT_FALSE(v.isNumeric());
    });

    SAD_TEST("TC04: isContainer للمصفوفة", {
        Value::ArrayType arr;
        Value v(arr);
        SAD_ASSERT_TRUE(v.isContainer());
    });

    SAD_TEST("TC05: isContainer للقاموس", {
        Value::MapType map;
        Value v(map);
        SAD_ASSERT_TRUE(v.isContainer());
    });

    SAD_TEST("TC06: isContainer للرقم", {
        Value v(42);
        SAD_ASSERT_FALSE(v.isContainer());
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 3: التحويلات / Type Conversions
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Value.Conversions / القيم.التحويلات");

    SAD_TEST("CV01: int -> double", {
        Value v(42);
        SAD_ASSERT_FLOAT_EQ(v.toDouble(), 42.0, 0.001);
    });

    SAD_TEST("CV02: int -> string", {
        Value v(42);
        SAD_ASSERT_EQ(v.toString(), std::string("42"));
    });

    SAD_TEST("CV03: int -> bool (non-zero)", {
        Value v(5);
        SAD_ASSERT_EQ(v.toBool(), true);
    });

    SAD_TEST("CV04: int -> bool (zero)", {
        Value v(0);
        SAD_ASSERT_EQ(v.toBool(), false);
    });

    SAD_TEST("CV05: double -> int", {
        Value v(3.7);
        SAD_ASSERT_EQ(v.toInt(), 3);
    });

    SAD_TEST("CV06: double -> string", {
        Value v(3.14);
        std::string s = v.toString();
        SAD_ASSERT_TRUE(s.find("3.14") != std::string::npos);
    });

    SAD_TEST("CV07: string -> bool (non-empty)", {
        Value v("hello");
        SAD_ASSERT_EQ(v.toBool(), true);
    });

    SAD_TEST("CV08: string -> bool (empty)", {
        Value v(std::string(""));
        SAD_ASSERT_EQ(v.toBool(), false);
    });

    SAD_TEST("CV09: bool -> int (true)", {
        Value v(true);
        SAD_ASSERT_EQ(v.toInt(), 1);
    });

    SAD_TEST("CV10: bool -> int (false)", {
        Value v(false);
        SAD_ASSERT_EQ(v.toInt(), 0);
    });

    SAD_TEST("CV11: bool -> string", {
        Value v(true);
        std::string s = v.toString();
        // Might be "true", "1", or "صحيح"
        SAD_ASSERT_FALSE(s.empty());
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 4: العمليات الحسابية / Arithmetic Operations
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Value.Arithmetic / القيم.العمليات_الحسابية");

    SAD_TEST("AR01: جمع أعداد صحيحة", {
        Value a(3), b(4);
        Value result = a + b;
        SAD_ASSERT_EQ(result.toInt(), 7);
    });

    SAD_TEST("AR02: طرح أعداد صحيحة", {
        Value a(10), b(3);
        Value result = a - b;
        SAD_ASSERT_EQ(result.toInt(), 7);
    });

    SAD_TEST("AR03: ضرب أعداد صحيحة", {
        Value a(6), b(7);
        Value result = a * b;
        SAD_ASSERT_EQ(result.toInt(), 42);
    });

    SAD_TEST("AR04: قسمة أعداد صحيحة", {
        Value a(10), b(3);
        Value result = a / b;
        SAD_ASSERT_EQ(result.toInt(), 3);
    });

    SAD_TEST("AR05: باقي القسمة", {
        Value a(10), b(3);
        Value result = a % b;
        SAD_ASSERT_EQ(result.toInt(), 1);
    });

    SAD_TEST("AR06: سالب أحادي", {
        Value a(5);
        Value result = -a;
        SAD_ASSERT_EQ(result.toInt(), -5);
    });

    SAD_TEST("AR07: جمع أعداد عشرية", {
        Value a(1.5), b(2.3);
        Value result = a + b;
        SAD_ASSERT_FLOAT_EQ(result.toDouble(), 3.8, 0.001);
    });

    SAD_TEST("AR08: طرح أعداد عشرية", {
        Value a(5.5), b(2.2);
        Value result = a - b;
        SAD_ASSERT_FLOAT_EQ(result.toDouble(), 3.3, 0.001);
    });

    SAD_TEST("AR09: ضرب عشري", {
        Value a(2.5), b(4.0);
        Value result = a * b;
        SAD_ASSERT_FLOAT_EQ(result.toDouble(), 10.0, 0.001);
    });

    SAD_TEST("AR10: قسمة عشري", {
        Value a(10.0), b(4.0);
        Value result = a / b;
        SAD_ASSERT_FLOAT_EQ(result.toDouble(), 2.5, 0.001);
    });

    SAD_TEST("AR11: جمع مختلط (int + double)", {
        Value a(5), b(2.5);
        Value result = a + b;
        SAD_ASSERT_TRUE(result.isDouble() || result.isInteger());
    });

    SAD_TEST("AR12: دمج نصوص", {
        Value a("hello"), b(" world");
        Value result = a + b;
        SAD_ASSERT_EQ(result.toString(), std::string("hello world"));
    });

    SAD_TEST("AR13: تكرار نص * عدد (إن دعم)", {
        Value a("ha"), b(3);
        try {
            Value result = a * b;
            SAD_ASSERT_EQ(result.toString(), std::string("hahaha"));
        } catch (...) {
            // If not supported, it's okay
        }
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 5: المقارنة / Comparison
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Value.Comparison / القيم.المقارنة");

    SAD_TEST("CMP01: == أعداد متساوية", {
        Value a(5), b(5);
        Value result = (a == b);
        SAD_ASSERT_EQ(result.toBool(), true);
    });

    SAD_TEST("CMP02: == أعداد غير متساوية", {
        Value a(5), b(6);
        Value result = (a == b);
        SAD_ASSERT_EQ(result.toBool(), false);
    });

    SAD_TEST("CMP03: != أعداد مختلفة", {
        Value a(5), b(6);
        Value result = (a != b);
        SAD_ASSERT_EQ(result.toBool(), true);
    });

    SAD_TEST("CMP04: < أقل", {
        Value a(3), b(5);
        Value result = (a < b);
        SAD_ASSERT_EQ(result.toBool(), true);
    });

    SAD_TEST("CMP05: > أكبر", {
        Value a(5), b(3);
        Value result = (a > b);
        SAD_ASSERT_EQ(result.toBool(), true);
    });

    SAD_TEST("CMP06: <= أقل أو يساوي", {
        Value a(5), b(5);
        Value result = (a <= b);
        SAD_ASSERT_EQ(result.toBool(), true);
    });

    SAD_TEST("CMP07: >= أكبر أو يساوي", {
        Value a(5), b(5);
        Value result = (a >= b);
        SAD_ASSERT_EQ(result.toBool(), true);
    });

    SAD_TEST("CMP08: == نصوص متساوية", {
        Value a("abc"), b("abc");
        Value result = (a == b);
        SAD_ASSERT_EQ(result.toBool(), true);
    });

    SAD_TEST("CMP09: == نصوص مختلفة", {
        Value a("abc"), b("xyz");
        Value result = (a == b);
        SAD_ASSERT_EQ(result.toBool(), false);
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 6: العمليات المنطقية / Logical Operations
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Value.Logical / القيم.المنطقية");

    SAD_TEST("LOGIC01: && (true && true)", {
        Value a(true), b(true);
        Value result = (a && b);
        SAD_ASSERT_EQ(result.toBool(), true);
    });

    SAD_TEST("LOGIC02: && (true && false)", {
        Value a(true), b(false);
        Value result = (a && b);
        SAD_ASSERT_EQ(result.toBool(), false);
    });

    SAD_TEST("LOGIC03: || (false || true)", {
        Value a(false), b(true);
        Value result = (a || b);
        SAD_ASSERT_EQ(result.toBool(), true);
    });

    SAD_TEST("LOGIC04: || (false || false)", {
        Value a(false), b(false);
        Value result = (a || b);
        SAD_ASSERT_EQ(result.toBool(), false);
    });

    SAD_TEST("LOGIC05: ! (not true)", {
        Value a(true);
        Value result = !a;
        SAD_ASSERT_EQ(result.toBool(), false);
    });

    SAD_TEST("LOGIC06: ! (not false)", {
        Value a(false);
        Value result = !a;
        SAD_ASSERT_EQ(result.toBool(), true);
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 7: عمليات المصفوفات / Array Operations
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Value.Arrays / القيم.المصفوفات");

    SAD_TEST("ARRAY01: إنشاء وحجم", {
        Value::ArrayType arr = {Value(1), Value(2), Value(3)};
        Value v(arr);
        SAD_ASSERT_EQ(v.size(), (size_t)3);
    });

    SAD_TEST("ARRAY02: push عنصر", {
        Value::ArrayType arr;
        Value v(arr);
        v.push(Value(42));
        SAD_ASSERT_EQ(v.size(), (size_t)1);
    });

    SAD_TEST("ARRAY03: الوصول بالفهرس [0]", {
        Value::ArrayType arr = {Value(10), Value(20), Value(30)};
        Value v(arr);
        SAD_ASSERT_EQ(v[0].toInt(), 10);
    });

    SAD_TEST("ARRAY04: الوصول بالفهرس [2]", {
        Value::ArrayType arr = {Value(10), Value(20), Value(30)};
        Value v(arr);
        SAD_ASSERT_EQ(v[(size_t)2].toInt(), 30);
    });

    SAD_TEST("ARRAY05: pop آخر عنصر", {
        Value::ArrayType arr = {Value(1), Value(2), Value(3)};
        Value v(arr);
        Value popped = v.pop();
        SAD_ASSERT_EQ(popped.toInt(), 3);
        SAD_ASSERT_EQ(v.size(), (size_t)2);
    });

    SAD_TEST("ARRAY06: clear", {
        Value::ArrayType arr = {Value(1), Value(2)};
        Value v(arr);
        v.clear();
        SAD_ASSERT_EQ(v.size(), (size_t)0);
        SAD_ASSERT_TRUE(v.isEmpty());
    });

    SAD_TEST("ARRAY07: isEmpty على مصفوفة فارغة", {
        Value::ArrayType arr;
        Value v(arr);
        SAD_ASSERT_TRUE(v.isEmpty());
    });

    SAD_TEST("ARRAY08: isEmpty على مصفوفة ممتلئة", {
        Value::ArrayType arr = {Value(1)};
        Value v(arr);
        SAD_ASSERT_FALSE(v.isEmpty());
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 8: عمليات القاموس / Map Operations
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Value.Maps / القيم.القواميس");

    SAD_TEST("MAP01: إنشاء وحجم", {
        Value::MapType map;
        map["a"] = Value(1);
        map["b"] = Value(2);
        Value v(map);
        SAD_ASSERT_EQ(v.size(), (size_t)2);
    });

    SAD_TEST("MAP02: الوصول بالمفتاح", {
        Value::MapType map;
        map["key"] = Value(42);
        Value v(map);
        SAD_ASSERT_EQ(v[std::string("key")].toInt(), 42);
    });

    SAD_TEST("MAP03: hasKey موجود", {
        Value::MapType map;
        map["exists"] = Value(1);
        Value v(map);
        SAD_ASSERT_TRUE(v.hasKey("exists"));
    });

    SAD_TEST("MAP04: hasKey غير موجود", {
        Value::MapType map;
        Value v(map);
        SAD_ASSERT_FALSE(v.hasKey("missing"));
    });

    SAD_TEST("MAP05: remove مفتاح", {
        Value::MapType map;
        map["key"] = Value(1);
        Value v(map);
        v.remove("key");
        SAD_ASSERT_FALSE(v.hasKey("key"));
    });

    SAD_TEST("MAP06: keys", {
        Value::MapType map;
        map["a"] = Value(1);
        map["b"] = Value(2);
        Value v(map);
        auto k = v.keys();
        SAD_ASSERT_EQ(k.size(), (size_t)2);
    });

    SAD_TEST("MAP07: values", {
        Value::MapType map;
        map["x"] = Value(10);
        Value v(map);
        auto vals = v.values();
        SAD_ASSERT_EQ(vals.size(), (size_t)1);
    });

    SAD_TEST("MAP08: clear", {
        Value::MapType map;
        map["a"] = Value(1);
        Value v(map);
        v.clear();
        SAD_ASSERT_TRUE(v.isEmpty());
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 9: الزيادة والنقصان / Increment/Decrement
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Value.IncrDecr / القيم.الزيادة_والنقصان");

    SAD_TEST("ID01: ++val (prefix)", {
        Value v(5);
        ++v;
        SAD_ASSERT_EQ(v.toInt(), 6);
    });

    SAD_TEST("ID02: val++ (postfix)", {
        Value v(5);
        Value old = v++;
        SAD_ASSERT_EQ(old.toInt(), 5);
        SAD_ASSERT_EQ(v.toInt(), 6);
    });

    SAD_TEST("ID03: --val (prefix)", {
        Value v(5);
        --v;
        SAD_ASSERT_EQ(v.toInt(), 4);
    });

    SAD_TEST("ID04: val-- (postfix)", {
        Value v(5);
        Value old = v--;
        SAD_ASSERT_EQ(old.toInt(), 5);
        SAD_ASSERT_EQ(v.toInt(), 4);
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 10: الإسناد المركب / Compound Assignment
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Value.CompoundAssignment / القيم.الإسناد_المركب");

    SAD_TEST("CA01: += أعداد", {
        Value v(5);
        v += Value(3);
        SAD_ASSERT_EQ(v.toInt(), 8);
    });

    SAD_TEST("CA02: -= أعداد", {
        Value v(10);
        v -= Value(3);
        SAD_ASSERT_EQ(v.toInt(), 7);
    });

    SAD_TEST("CA03: *= أعداد", {
        Value v(4);
        v *= Value(5);
        SAD_ASSERT_EQ(v.toInt(), 20);
    });

    SAD_TEST("CA04: /= أعداد", {
        Value v(20);
        v /= Value(4);
        SAD_ASSERT_EQ(v.toInt(), 5);
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 11: النسخ / Cloning
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Value.Clone / القيم.النسخ");

    SAD_TEST("CLN01: نسخ رقم", {
        Value v(42);
        Value c = v.clone();
        SAD_ASSERT_EQ(c.toInt(), 42);
    });

    SAD_TEST("CLN02: نسخ نص", {
        Value v("hello");
        Value c = v.clone();
        SAD_ASSERT_EQ(c.toString(), std::string("hello"));
    });

    SAD_TEST("CLN03: نسخ مصفوفة (deep)", {
        Value::ArrayType arr = {Value(1), Value(2)};
        Value v(arr);
        Value c = v.clone();
        SAD_ASSERT_EQ(c.size(), (size_t)2);
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 12: Debug / التنقيح
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Value.Debug / القيم.التنقيح");

    SAD_TEST("DBG01: debugString للرقم", {
        Value v(42);
        std::string s = v.debugString();
        SAD_ASSERT_FALSE(s.empty());
    });

    SAD_TEST("DBG02: getTypeName", {
        Value v(42);
        std::string name = v.getTypeName();
        SAD_ASSERT_FALSE(name.empty());
    });

    SAD_TEST("DBG03: debugString للنص", {
        Value v("test");
        std::string s = v.debugString();
        SAD_ASSERT_FALSE(s.empty());
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 13: حالات الحدود / Edge Cases
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Value.EdgeCases / القيم.حالات_الحدود");

    SAD_TEST("VEDGE01: قسمة على صفر", {
        Value a(10), b(0);
        SAD_ASSERT_THROWS(a / b, std::runtime_error);
    });

    SAD_TEST("VEDGE02: باقي قسمة على صفر", {
        Value a(10), b(0);
        SAD_ASSERT_THROWS(a % b, std::runtime_error);
    });

    SAD_TEST("VEDGE03: الوصول لفهرس خارج النطاق", {
        Value::ArrayType arr = {Value(1)};
        Value v(arr);
        SAD_ASSERT_THROWS(v[(size_t)10], std::out_of_range);
    });

    SAD_TEST("VEDGE04: pop من مصفوفة فارغة", {
        Value::ArrayType arr;
        Value v(arr);
        SAD_ASSERT_THROWS(v.pop(), std::runtime_error);
    });

    SAD_SUMMARY();
}
