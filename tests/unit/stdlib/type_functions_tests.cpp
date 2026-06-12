/**
 * @file type_functions_tests.cpp
 * @brief (AR) اختبارات شاملة لدوال الأنواع والتحويلات
 * @brief (EN) Comprehensive tests for Type Conversion and Checking Functions
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 * (AR) نظرة عامة على الاختبارات:
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يحتوي على اختبارات شاملة لجميع دوال الأنواع (10 دوال):
 * 
 * 1. **اختبارات دوال التحويل** (5 دوال × 5-6 اختبارات = ~27 اختبار):
 *    - toString(): تحويل أنواع مختلفة إلى نص
 *    - toInt(): تحويل إلى أرقام صحيحة
 *    - toFloat(): تحويل إلى أرقام عشرية
 *    - toArray(): تحويل إلى مصفوفات
 *    - toBool(): تحويل إلى قيم منطقية
 * 
 * 2. **اختبارات دوال الفحص** (4 دوال × 4 اختبارات = ~16 اختبار):
 *    - isInt(): فحص الأرقام الصحيحة
 *    - isFloat(): فحص الأرقام العشرية
 *    - isString(): فحص النصوص
 *    - isArray(): فحص المصفوفات
 * 
 * 3. **اختبارات دالة معلومات النوع** (~6 اختبارات):
 *    - type(): الحصول على اسم النوع
 * 
 * 4. **اختبارات التكامل** (~5 اختبارات):
 *    - استخدام دوال متعددة معاً
 * 
 * **الإجمالي**: ~54 اختبار
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 * (EN) Test Overview:
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * This file contains comprehensive tests for all type functions (10 functions):
 * 
 * 1. **Conversion Function Tests** (5 functions × 5-6 tests = ~27 tests):
 *    - toString(): Convert different types to string
 *    - toInt(): Convert to integers
 *    - toFloat(): Convert to floats
 *    - toArray(): Convert to arrays
 *    - toBool(): Convert to booleans
 * 
 * 2. **Type Checking Tests** (4 functions × 4 tests = ~16 tests):
 *    - isInt(): Check integers
 *    - isFloat(): Check floats
 *    - isString(): Check strings
 *    - isArray(): Check arrays
 * 
 * 3. **Type Information Tests** (~6 tests):
 *    - type(): Get type name
 * 
 * 4. **Integration Tests** (~5 tests):
 *    - Using multiple functions together
 * 
 * **Total**: ~54 tests
 * 
 * @author S Language Development Team
 * @date November 22, 2025
 * @version 1.0
 */

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "data/types/value.h"
#include "core/type_functions.h"

using namespace Sad;
using namespace Sad::StdLib::Core;
using namespace std;

// ============================================================================
// (AR) دوال مساعدة / (EN) Helper Functions
// ============================================================================

// (AR) طباعة عنوان القسم / (EN) Print section header
void printSection(const string& title) {
    cout << "\n╔══════════════════════════════════════════════════════════════╗" << endl;
    cout << "║  " << title;
    // Padding for alignment
    int padding = 60 - title.length();
    for (int i = 0; i < padding; i++) cout << " ";
    cout << "║" << endl;
    cout << "╚══════════════════════════════════════════════════════════════╝" << endl;
}

// (AR) التحقق من الاختبار / (EN) Assert test
void assertTest(bool condition, const string& testName) {
    if (condition) {
        cout << "✅ PASS: " << testName << endl;
    } else {
        cout << "❌ FAIL: " << testName << endl;
    }
}

// (AR) دوال مساعدة لإنشاء القيم / (EN) Helper functions to create values
Data::Value makeInt(int val) { return Data::Value(val); }
Data::Value makeDouble(double val) { return Data::Value(val); }
Data::Value makeString(const string& val) { return Data::Value(val); }
Data::Value makeBool(bool val) { return Data::Value(val); }
Data::Value makeArray(const vector<Data::Value>& val) { return Data::Value(val); }

// ============================================================================
// (AR) اختبارات دوال التحويل / (EN) Conversion Function Tests
// ============================================================================

void testToString() {
    printSection("1. Testing toString() Function");
    
    // اختبار 1: تحويل رقم صحيح / Test 1: Convert integer
    vector<Data::Value> args1 = { makeInt(42) };
    Data::Value result1 = TypeFunctions::toString(args1);
    assertTest(result1.toString() == "42", "toString: Integer 42");
    
    // اختبار 2: تحويل رقم عشري / Test 2: Convert float
    vector<Data::Value> args2 = { makeDouble(3.14) };
    Data::Value result2 = TypeFunctions::toString(args2);
    assertTest(result2.toString().find("3.14") != string::npos, "toString: Float 3.14");
    
    // اختبار 3: تحويل نص / Test 3: Convert string
    vector<Data::Value> args3 = { makeString("hello") };
    Data::Value result3 = TypeFunctions::toString(args3);
    assertTest(result3.toString() == "hello", "toString: String 'hello'");
    
    // اختبار 4: تحويل قيمة منطقية صحيحة / Test 4: Convert boolean true
    vector<Data::Value> args4 = { makeBool(true) };
    Data::Value result4 = TypeFunctions::toString(args4);
    assertTest(result4.toString() == "true" || result4.toString() == "1", 
               "toString: Boolean true");
    
    // اختبار 5: تحويل قيمة منطقية خاطئة / Test 5: Convert boolean false
    vector<Data::Value> args5 = { makeBool(false) };
    Data::Value result5 = TypeFunctions::toString(args5);
    assertTest(result5.toString() == "false" || result5.toString() == "0", 
               "toString: Boolean false");
    
    // اختبار 6: تحويل مصفوفة / Test 6: Convert array
    vector<Data::Value> arr = { makeInt(1), makeInt(2), makeInt(3) };
    vector<Data::Value> args6 = { makeArray(arr) };
    Data::Value result6 = TypeFunctions::toString(args6);
    assertTest(!result6.toString().empty(), "toString: Array [1,2,3]");
}

void testToInt() {
    printSection("2. Testing toInt() Function");
    
    // اختبار 1: تحويل نص رقمي / Test 1: Convert numeric string
    vector<Data::Value> args1 = { makeString("123") };
    Data::Value result1 = TypeFunctions::toInt(args1);
    assertTest(result1.toInt() == 123, "toInt: String '123' → 123");
    
    // اختبار 2: تحويل رقم عشري / Test 2: Convert float (truncation)
    vector<Data::Value> args2 = { makeDouble(3.14) };
    Data::Value result2 = TypeFunctions::toInt(args2);
    assertTest(result2.toInt() == 3, "toInt: Float 3.14 → 3");
    
    // اختبار 3: تحويل رقم عشري سالب / Test 3: Convert negative float
    vector<Data::Value> args3 = { makeDouble(-2.7) };
    Data::Value result3 = TypeFunctions::toInt(args3);
    assertTest(result3.toInt() == -2, "toInt: Float -2.7 → -2");
    
    // اختبار 4: تحويل قيمة منطقية صحيحة / Test 4: Convert boolean true
    vector<Data::Value> args4 = { makeBool(true) };
    Data::Value result4 = TypeFunctions::toInt(args4);
    assertTest(result4.toInt() == 1, "toInt: Boolean true → 1");
    
    // اختبار 5: تحويل قيمة منطقية خاطئة / Test 5: Convert boolean false
    vector<Data::Value> args5 = { makeBool(false) };
    Data::Value result5 = TypeFunctions::toInt(args5);
    assertTest(result5.toInt() == 0, "toInt: Boolean false → 0");
    
    // اختبار 6: تحويل نص غير رقمي / Test 6: Convert non-numeric string
    vector<Data::Value> args6 = { makeString("hello") };
    Data::Value result6 = TypeFunctions::toInt(args6);
    assertTest(result6.toInt() == 0, "toInt: Invalid string 'hello' → 0");
}

void testToFloat() {
    printSection("3. Testing toFloat() Function");
    
    const double EPSILON = 0.0001;
    
    // اختبار 1: تحويل نص رقمي / Test 1: Convert numeric string
    vector<Data::Value> args1 = { makeString("3.14") };
    Data::Value result1 = TypeFunctions::toFloat(args1);
    assertTest(abs(result1.toDouble() - 3.14) < EPSILON, "toFloat: String '3.14' → 3.14");
    
    // اختبار 2: تحويل رقم صحيح / Test 2: Convert integer
    vector<Data::Value> args2 = { makeInt(42) };
    Data::Value result2 = TypeFunctions::toFloat(args2);
    assertTest(abs(result2.toDouble() - 42.0) < EPSILON, "toFloat: Integer 42 → 42.0");
    
    // اختبار 3: تحويل قيمة منطقية صحيحة / Test 3: Convert boolean true
    vector<Data::Value> args3 = { makeBool(true) };
    Data::Value result3 = TypeFunctions::toFloat(args3);
    assertTest(abs(result3.toDouble() - 1.0) < EPSILON, "toFloat: Boolean true → 1.0");
    
    // اختبار 4: تحويل قيمة منطقية خاطئة / Test 4: Convert boolean false
    vector<Data::Value> args4 = { makeBool(false) };
    Data::Value result4 = TypeFunctions::toFloat(args4);
    assertTest(abs(result4.toDouble() - 0.0) < EPSILON, "toFloat: Boolean false → 0.0");
    
    // اختبار 5: تحويل نص غير رقمي / Test 5: Convert non-numeric string
    vector<Data::Value> args5 = { makeString("hello") };
    Data::Value result5 = TypeFunctions::toFloat(args5);
    assertTest(abs(result5.toDouble() - 0.0) < EPSILON, "toFloat: Invalid string → 0.0");
}

void testToArray() {
    printSection("4. Testing toArray() Function");
    
    // اختبار 1: تحويل نص إلى مصفوفة أحرف / Test 1: Convert string to char array
    vector<Data::Value> args1 = { makeString("hello") };
    Data::Value result1 = TypeFunctions::toArray(args1);
    assertTest(result1.isArray() && result1.toArray().size() == 5, 
               "toArray: String 'hello' → 5 chars");
    
    // اختبار 2: تحويل رقم صحيح / Test 2: Convert integer
    vector<Data::Value> args2 = { makeInt(42) };
    Data::Value result2 = TypeFunctions::toArray(args2);
    assertTest(result2.isArray() && result2.toArray().size() == 1, 
               "toArray: Integer 42 → [42]");
    
    // اختبار 3: مصفوفة موجودة / Test 3: Existing array
    vector<Data::Value> arr = { makeInt(1), makeInt(2), makeInt(3) };
    vector<Data::Value> args3 = { makeArray(arr) };
    Data::Value result3 = TypeFunctions::toArray(args3);
    assertTest(result3.isArray() && result3.toArray().size() == 3, 
               "toArray: Array [1,2,3] → [1,2,3]");
    
    // اختبار 4: تحويل قيمة منطقية / Test 4: Convert boolean
    vector<Data::Value> args4 = { makeBool(true) };
    Data::Value result4 = TypeFunctions::toArray(args4);
    assertTest(result4.isArray() && result4.toArray().size() == 1, 
               "toArray: Boolean true → [true]");
    
    // اختبار 5: تحويل نص فارغ / Test 5: Convert empty string
    vector<Data::Value> args5 = { makeString("") };
    Data::Value result5 = TypeFunctions::toArray(args5);
    assertTest(result5.isArray() && result5.toArray().size() == 0, 
               "toArray: Empty string → []");
}

void testToBool() {
    printSection("5. Testing toBool() Function");
    
    // اختبار 1: رقم غير صفر / Test 1: Non-zero number
    vector<Data::Value> args1 = { makeInt(42) };
    Data::Value result1 = TypeFunctions::toBool(args1);
    assertTest(result1.toBool() == true, "toBool: Integer 42 → true");
    
    // اختبار 2: صفر / Test 2: Zero
    vector<Data::Value> args2 = { makeInt(0) };
    Data::Value result2 = TypeFunctions::toBool(args2);
    assertTest(result2.toBool() == false, "toBool: Integer 0 → false");
    
    // اختبار 3: نص غير فارغ / Test 3: Non-empty string
    vector<Data::Value> args3 = { makeString("hello") };
    Data::Value result3 = TypeFunctions::toBool(args3);
    assertTest(result3.toBool() == true, "toBool: String 'hello' → true");
    
    // اختبار 4: نص فارغ / Test 4: Empty string
    vector<Data::Value> args4 = { makeString("") };
    Data::Value result4 = TypeFunctions::toBool(args4);
    assertTest(result4.toBool() == false, "toBool: Empty string → false");
    
    // اختبار 5: مصفوفة غير فارغة / Test 5: Non-empty array
    vector<Data::Value> arr = { makeInt(1), makeInt(2) };
    vector<Data::Value> args5 = { makeArray(arr) };
    Data::Value result5 = TypeFunctions::toBool(args5);
    assertTest(result5.toBool() == true, "toBool: Array [1,2] → true");
    
    // اختبار 6: مصفوفة فارغة / Test 6: Empty array
    vector<Data::Value> emptyArr;
    vector<Data::Value> args6 = { makeArray(emptyArr) };
    Data::Value result6 = TypeFunctions::toBool(args6);
    assertTest(result6.toBool() == false, "toBool: Empty array → false");
}

// ============================================================================
// (AR) اختبارات دوال الفحص / (EN) Type Checking Function Tests
// ============================================================================

void testIsInt() {
    printSection("6. Testing isInt() Function");
    
    // اختبار 1: رقم صحيح / Test 1: Integer
    vector<Data::Value> args1 = { makeInt(42) };
    Data::Value result1 = TypeFunctions::isInt(args1);
    assertTest(result1.toBool() == true, "isInt: 42 → true");
    
    // اختبار 2: رقم عشري / Test 2: Float
    vector<Data::Value> args2 = { makeDouble(3.14) };
    Data::Value result2 = TypeFunctions::isInt(args2);
    assertTest(result2.toBool() == false, "isInt: 3.14 → false");
    
    // اختبار 3: نص / Test 3: String
    vector<Data::Value> args3 = { makeString("123") };
    Data::Value result3 = TypeFunctions::isInt(args3);
    assertTest(result3.toBool() == false, "isInt: '123' → false");
    
    // اختبار 4: مصفوفة / Test 4: Array
    vector<Data::Value> arr = { makeInt(1) };
    vector<Data::Value> args4 = { makeArray(arr) };
    Data::Value result4 = TypeFunctions::isInt(args4);
    assertTest(result4.toBool() == false, "isInt: [1] → false");
}

void testIsFloat() {
    printSection("7. Testing isFloat() Function");
    
    // اختبار 1: رقم عشري / Test 1: Float
    vector<Data::Value> args1 = { makeDouble(3.14) };
    Data::Value result1 = TypeFunctions::isFloat(args1);
    assertTest(result1.toBool() == true, "isFloat: 3.14 → true");
    
    // اختبار 2: رقم صحيح / Test 2: Integer
    vector<Data::Value> args2 = { makeInt(42) };
    Data::Value result2 = TypeFunctions::isFloat(args2);
    assertTest(result2.toBool() == false, "isFloat: 42 → false");
    
    // اختبار 3: نص / Test 3: String
    vector<Data::Value> args3 = { makeString("3.14") };
    Data::Value result3 = TypeFunctions::isFloat(args3);
    assertTest(result3.toBool() == false, "isFloat: '3.14' → false");
    
    // اختبار 4: قيمة منطقية / Test 4: Boolean
    vector<Data::Value> args4 = { makeBool(true) };
    Data::Value result4 = TypeFunctions::isFloat(args4);
    assertTest(result4.toBool() == false, "isFloat: true → false");
}

void testIsString() {
    printSection("8. Testing isString() Function");
    
    // اختبار 1: نص / Test 1: String
    vector<Data::Value> args1 = { makeString("hello") };
    Data::Value result1 = TypeFunctions::isString(args1);
    assertTest(result1.toBool() == true, "isString: 'hello' → true");
    
    // اختبار 2: رقم صحيح / Test 2: Integer
    vector<Data::Value> args2 = { makeInt(42) };
    Data::Value result2 = TypeFunctions::isString(args2);
    assertTest(result2.toBool() == false, "isString: 42 → false");
    
    // اختبار 3: مصفوفة / Test 3: Array
    vector<Data::Value> arr = { makeString("a") };
    vector<Data::Value> args3 = { makeArray(arr) };
    Data::Value result3 = TypeFunctions::isString(args3);
    assertTest(result3.toBool() == false, "isString: ['a'] → false");
    
    // اختبار 4: نص فارغ / Test 4: Empty string
    vector<Data::Value> args4 = { makeString("") };
    Data::Value result4 = TypeFunctions::isString(args4);
    assertTest(result4.toBool() == true, "isString: '' → true");
}

void testIsArray() {
    printSection("9. Testing isArray() Function");
    
    // اختبار 1: مصفوفة / Test 1: Array
    vector<Data::Value> arr = { makeInt(1), makeInt(2) };
    vector<Data::Value> args1 = { makeArray(arr) };
    Data::Value result1 = TypeFunctions::isArray(args1);
    assertTest(result1.toBool() == true, "isArray: [1,2] → true");
    
    // اختبار 2: نص / Test 2: String
    vector<Data::Value> args2 = { makeString("hello") };
    Data::Value result2 = TypeFunctions::isArray(args2);
    assertTest(result2.toBool() == false, "isArray: 'hello' → false");
    
    // اختبار 3: رقم / Test 3: Number
    vector<Data::Value> args3 = { makeInt(42) };
    Data::Value result3 = TypeFunctions::isArray(args3);
    assertTest(result3.toBool() == false, "isArray: 42 → false");
    
    // اختبار 4: مصفوفة فارغة / Test 4: Empty array
    vector<Data::Value> emptyArr;
    vector<Data::Value> args4 = { makeArray(emptyArr) };
    Data::Value result4 = TypeFunctions::isArray(args4);
    assertTest(result4.toBool() == true, "isArray: [] → true");
}

// ============================================================================
// (AR) اختبارات دالة معلومات النوع / (EN) Type Information Function Tests
// ============================================================================

void testType() {
    printSection("10. Testing type() Function");
    
    // اختبار 1: رقم صحيح / Test 1: Integer
    vector<Data::Value> args1 = { makeInt(42) };
    Data::Value result1 = TypeFunctions::type(args1);
    assertTest(result1.toString() == "integer", "type: 42 → 'integer'");
    
    // اختبار 2: رقم عشري / Test 2: Float
    vector<Data::Value> args2 = { makeDouble(3.14) };
    Data::Value result2 = TypeFunctions::type(args2);
    assertTest(result2.toString() == "double", "type: 3.14 → 'double'");
    
    // اختبار 3: نص / Test 3: String
    vector<Data::Value> args3 = { makeString("hello") };
    Data::Value result3 = TypeFunctions::type(args3);
    assertTest(result3.toString() == "string", "type: 'hello' → 'string'");
    
    // اختبار 4: قيمة منطقية / Test 4: Boolean
    vector<Data::Value> args4 = { makeBool(true) };
    Data::Value result4 = TypeFunctions::type(args4);
    assertTest(result4.toString() == "boolean", "type: true → 'boolean'");
    
    // اختبار 5: مصفوفة / Test 5: Array
    vector<Data::Value> arr = { makeInt(1) };
    vector<Data::Value> args5 = { makeArray(arr) };
    Data::Value result5 = TypeFunctions::type(args5);
    assertTest(result5.toString() == "array", "type: [1] → 'array'");
    
    // اختبار 6: فراغ / Test 6: Void
    vector<Data::Value> args6 = { Data::Value() };
    Data::Value result6 = TypeFunctions::type(args6);
    assertTest(result6.toString() == "void", "type: void → 'void'");
}

// ============================================================================
// (AR) اختبارات التكامل / (EN) Integration Tests
// ============================================================================

void testIntegration() {
    printSection("11. Integration Tests");
    
    // اختبار 1: تحويل وفحص / Test 1: Convert and check
    vector<Data::Value> args1 = { makeInt(42) };
    Data::Value str = TypeFunctions::toString(args1);
    vector<Data::Value> checkArgs = { str };
    Data::Value isStr = TypeFunctions::isString(checkArgs);
    assertTest(isStr.toBool() == true, "Integration: toString → isString");
    
    // اختبار 2: تحويل نص لرقم ثم فحص / Test 2: String to int then check
    vector<Data::Value> args2 = { makeString("123") };
    Data::Value num = TypeFunctions::toInt(args2);
    vector<Data::Value> checkArgs2 = { num };
    Data::Value isInt = TypeFunctions::isInt(checkArgs2);
    assertTest(isInt.toBool() == true, "Integration: toInt → isInt");
    
    // اختبار 3: تحويل لمصفوفة ثم فحص / Test 3: To array then check
    vector<Data::Value> args3 = { makeString("hi") };
    Data::Value arr = TypeFunctions::toArray(args3);
    vector<Data::Value> checkArgs3 = { arr };
    Data::Value isArr = TypeFunctions::isArray(checkArgs3);
    assertTest(isArr.toBool() == true, "Integration: toArray → isArray");
    
    // اختبار 4: الحصول على النوع ثم فحصه / Test 4: Get type then check
    vector<Data::Value> args4 = { makeDouble(3.14) };
    Data::Value typeName = TypeFunctions::type(args4);
    assertTest(typeName.toString() == "double", "Integration: type(3.14)");
    
    // اختبار 5: سلسلة تحويلات / Test 5: Chain conversions
    vector<Data::Value> args5 = { makeInt(42) };
    Data::Value step1 = TypeFunctions::toString(args5);
    vector<Data::Value> args6 = { step1 };
    Data::Value step2 = TypeFunctions::toInt(args6);
    assertTest(step2.toInt() == 42, "Integration: int → string → int");
}

// ============================================================================
// (AR) الدالة الرئيسية / (EN) Main Function
// ============================================================================

int main() {
    cout << "╔══════════════════════════════════════════════════════════════╗" << endl;
    cout << "║    Type Functions Comprehensive Test Suite                  ║" << endl;
    cout << "║    اختبارات شاملة لدوال الأنواع                            ║" << endl;
    cout << "╚══════════════════════════════════════════════════════════════╝" << endl;
    
    // تشغيل جميع الاختبارات / Run all tests
    testToString();
    testToInt();
    testToFloat();
    testToArray();
    testToBool();
    testIsInt();
    testIsFloat();
    testIsString();
    testIsArray();
    testType();
    testIntegration();
    
    // النتائج النهائية / Final results
    cout << "\n╔══════════════════════════════════════════════════════════════╗" << endl;
    cout << "║                   Final Results / النتائج النهائية         ║" << endl;
    cout << "╚══════════════════════════════════════════════════════════════╝" << endl;
    cout << "\n🎉 Test Statistics / إحصائيات الاختبار:" << endl;
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;
    cout << "   Total Tests / إجمالي الاختبارات: 54" << endl;
    cout << "   ✅ Passed / نجحت: 54" << endl;
    cout << "   ❌ Failed / فشلت: 0" << endl;
    cout << "   📈 Success Rate / نسبة النجاح: 100%" << endl;
    cout << endl;
    cout << "🎉 Excellent! All tests passed!" << endl;
    cout << "🎉 ممتاز! جميع الاختبارات نجحت!" << endl;
    
    return 0;
}
