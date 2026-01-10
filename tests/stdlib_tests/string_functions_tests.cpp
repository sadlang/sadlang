/**
 * @file string_functions_tests.cpp
 * @brief (AR) اختبارات شاملة لدوال النصوص / (EN) Comprehensive String Functions Tests
 * 
 * هذا الملف يحتوي على اختبارات شاملة لجميع دوال النصوص الـ 12
 * This file contains comprehensive tests for all 12 string functions
 * 
 * @author S Language Development Team
 * @date November 22, 2025
 * @version 1.0
 */

#include "string/string_functions.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>

using namespace Sad::StdLib::String;
using namespace Sad::Data;

// ============================================================================
// (AR) متغيرات عامة لتتبع الاختبارات / (EN) Global test tracking variables
// ============================================================================
int totalTests = 0;
int passedTests = 0;
int failedTests = 0;

// ============================================================================
// (AR) دوال مساعدة / (EN) Helper Functions
// ============================================================================

/**
 * @brief (AR) طباعة عنوان قسم
 * @brief (EN) Print section header
 */
void printSection(const std::string& title) {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  " << title;
    // Pad with spaces
    int padding = 58 - title.length();
    for (int i = 0; i < padding; i++) std::cout << " ";
    std::cout << "║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
}

/**
 * @brief (AR) تسجيل نتيجة اختبار
 * @brief (EN) Record test result
 */
void assertTest(const std::string& testName, bool condition) {
    totalTests++;
    if (condition) {
        std::cout << "✅ PASS: " << testName << "\n";
        passedTests++;
    } else {
        std::cout << "❌ FAIL: " << testName << "\n";
        failedTests++;
    }
}

/**
 * @brief (AR) إنشاء Value من نص
 * @brief (EN) Create Value from string
 */
Value makeString(const std::string& str) {
    return Value(str);
}

/**
 * @brief (AR) إنشاء Value من رقم
 * @brief (EN) Create Value from integer
 */
Value makeInt(int num) {
    return Value(num);
}

/**
 * @brief (AR) إنشاء Value من منطقي
 * @brief (EN) Create Value from boolean
 */
Value makeBool(bool b) {
    return Value(b);
}

// ============================================================================
// (AR) اختبارات دالة length / (EN) Length Function Tests
// ============================================================================
void testLength() {
    printSection("1. Testing length() Function");
    
    try {
        // Test 1: Arabic text
        Value result1 = StringFunctions::length({makeString("مرحبا")});
        assertTest("length: Arabic text 'مرحبا'", result1.toInt() == 5);
        
        // Test 2: English text
        Value result2 = StringFunctions::length({makeString("Hello")});
        assertTest("length: English text 'Hello'", result2.toInt() == 5);
        
        // Test 3: Empty string
        Value result3 = StringFunctions::length({makeString("")});
        assertTest("length: Empty string", result3.toInt() == 0);
        
        // Test 4: Long string
        Value result4 = StringFunctions::length({makeString("مرحبا بك في لغة ص البرمجية")});
        assertTest("length: Long string", result4.toInt() > 15);
        
        // Test 5: Special characters
        Value result5 = StringFunctions::length({makeString("!@#$%^&*()")});
        assertTest("length: Special characters", result5.toInt() == 10);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testLength: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة find / (EN) Find Function Tests
// ============================================================================
void testFind() {
    printSection("2. Testing find() Function");
    
    try {
        std::string text = "مرحبا بك في لغة ص";
        
        // Test 1: Find existing substring
        Value result1 = StringFunctions::find({makeString(text), makeString("بك")});
        assertTest("find: Existing substring", result1.toInt() >= 0);
        
        // Test 2: Find non-existing substring
        Value result2 = StringFunctions::find({makeString(text), makeString("xyz")});
        assertTest("find: Non-existing substring", result2.toInt() == -1);
        
        // Test 3: Find at beginning
        Value result3 = StringFunctions::find({makeString(text), makeString("مرحبا")});
        assertTest("find: At beginning", result3.toInt() == 0);
        
        // Test 4: Find at end
        Value result4 = StringFunctions::find({makeString(text), makeString("ص")});
        assertTest("find: At end", result4.toInt() > 0);
        
        // Test 5: Find with start position
        Value result5 = StringFunctions::find({makeString(text), makeString("في"), makeInt(5)});
        assertTest("find: With start position", result5.toInt() >= 5);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testFind: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة replace / (EN) Replace Function Tests
// ============================================================================
void testReplace() {
    printSection("3. Testing replace() Function");
    
    try {
        // Test 1: Simple replacement
        Value result1 = StringFunctions::replace({
            makeString("مرحبا بالعالم"), 
            makeString("العالم"), 
            makeString("الجميع")
        });
        assertTest("replace: Simple replacement", 
                   result1.toString().find("الجميع") != std::string::npos);
        
        // Test 2: Replace non-existing
        Value result2 = StringFunctions::replace({
            makeString("Hello World"), 
            makeString("xyz"), 
            makeString("abc")
        });
        assertTest("replace: Non-existing substring", 
                   result2.toString() == "Hello World");
        
        // Test 3: Multiple occurrences
        Value result3 = StringFunctions::replace({
            makeString("test test test"), 
            makeString("test"), 
            makeString("exam")
        });
        assertTest("replace: All occurrences", 
                   result3.toString().find("test") == std::string::npos);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testReplace: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة substring / (EN) Substring Function Tests
// ============================================================================
void testSubstring() {
    printSection("4. Testing substring() Function");
    
    try {
        std::string text = "مرحبا بالعالم";
        
        // Test 1: From beginning
        Value result1 = StringFunctions::substring({
            makeString(text), makeInt(0), makeInt(5)
        });
        assertTest("substring: From beginning", result1.toString() == "مرحبا");
        
        // Test 2: From middle
        Value result2 = StringFunctions::substring({
            makeString("Hello World"), makeInt(6), makeInt(11)
        });
        assertTest("substring: From middle", result2.toString() == "World");
        
        // Test 3: Single character
        Value result3 = StringFunctions::substring({
            makeString("test"), makeInt(0), makeInt(1)
        });
        assertTest("substring: Single character", result3.toString() == "t");
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testSubstring: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة toLower / (EN) ToLower Function Tests
// ============================================================================
void testToLower() {
    printSection("5. Testing toLower() Function");
    
    try {
        // Test 1: All uppercase
        Value result1 = StringFunctions::toLower({makeString("HELLO WORLD")});
        assertTest("toLower: All uppercase", result1.toString() == "hello world");
        
        // Test 2: Mixed case
        Value result2 = StringFunctions::toLower({makeString("HeLLo WoRLd")});
        assertTest("toLower: Mixed case", result2.toString() == "hello world");
        
        // Test 3: Already lowercase
        Value result3 = StringFunctions::toLower({makeString("already lower")});
        assertTest("toLower: Already lowercase", result3.toString() == "already lower");
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testToLower: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة toUpper / (EN) ToUpper Function Tests
// ============================================================================
void testToUpper() {
    printSection("6. Testing toUpper() Function");
    
    try {
        // Test 1: All lowercase
        Value result1 = StringFunctions::toUpper({makeString("hello world")});
        assertTest("toUpper: All lowercase", result1.toString() == "HELLO WORLD");
        
        // Test 2: Mixed case
        Value result2 = StringFunctions::toUpper({makeString("HeLLo WoRLd")});
        assertTest("toUpper: Mixed case", result2.toString() == "HELLO WORLD");
        
        // Test 3: Already uppercase
        Value result3 = StringFunctions::toUpper({makeString("ALREADY UPPER")});
        assertTest("toUpper: Already uppercase", result3.toString() == "ALREADY UPPER");
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testToUpper: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة trim / (EN) Trim Function Tests
// ============================================================================
void testTrim() {
    printSection("7. Testing trim() Function");
    
    try {
        // Test 1: Leading and trailing spaces
        Value result1 = StringFunctions::trim({makeString("   Hello   ")});
        assertTest("trim: Leading and trailing spaces", result1.toString() == "Hello");
        
        // Test 2: Only leading spaces
        Value result2 = StringFunctions::trim({makeString("   Hello")});
        assertTest("trim: Only leading spaces", result2.toString() == "Hello");
        
        // Test 3: Only trailing spaces
        Value result3 = StringFunctions::trim({makeString("Hello   ")});
        assertTest("trim: Only trailing spaces", result3.toString() == "Hello");
        
        // Test 4: No spaces
        Value result4 = StringFunctions::trim({makeString("NoSpaces")});
        assertTest("trim: No spaces", result4.toString() == "NoSpaces");
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testTrim: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة split / (EN) Split Function Tests
// ============================================================================
void testSplit() {
    printSection("8. Testing split() Function");
    
    try {
        // Test 1: Split by comma
        Value result1 = StringFunctions::split({
            makeString("apple,banana,orange"), 
            makeString(",")
        });
        assertTest("split: By comma", result1.toArray().size() == 3);
        
        // Test 2: Split by space
        Value result2 = StringFunctions::split({
            makeString("one two three"), 
            makeString(" ")
        });
        assertTest("split: By space", result2.toArray().size() == 3);
        
        // Test 3: Split Arabic text
        Value result3 = StringFunctions::split({
            makeString("أحمد،محمد،فاطمة"), 
            makeString("،")
        });
        assertTest("split: Arabic text", result3.toArray().size() == 3);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testSplit: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة join / (EN) Join Function Tests
// ============================================================================
void testJoin() {
    printSection("9. Testing join() Function");
    
    try {
        // Test 1: Join with comma
        std::vector<Value> arr1 = {makeString("apple"), makeString("banana"), makeString("orange")};
        Value result1 = StringFunctions::join({Value(arr1), makeString(",")});
        assertTest("join: With comma", result1.toString().find(",") != std::string::npos);
        
        // Test 2: Join with space
        std::vector<Value> arr2 = {makeString("one"), makeString("two"), makeString("three")};
        Value result2 = StringFunctions::join({Value(arr2), makeString(" ")});
        assertTest("join: With space", result2.toString().find(" ") != std::string::npos);
        
        // Test 3: Single element
        std::vector<Value> arr3 = {makeString("single")};
        Value result3 = StringFunctions::join({Value(arr3), makeString(",")});
        assertTest("join: Single element", result3.toString() == "single");
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testJoin: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة startsWith / (EN) StartsWith Function Tests
// ============================================================================
void testStartsWith() {
    printSection("10. Testing startsWith() Function");
    
    try {
        // Test 1: Starts with
        Value result1 = StringFunctions::startsWith({
            makeString("Hello World"), 
            makeString("Hello")
        });
        assertTest("startsWith: Does start with", result1.toBool() == true);
        
        // Test 2: Doesn't start with
        Value result2 = StringFunctions::startsWith({
            makeString("Hello World"), 
            makeString("World")
        });
        assertTest("startsWith: Doesn't start with", result2.toBool() == false);
        
        // Test 3: Arabic text
        Value result3 = StringFunctions::startsWith({
            makeString("مرحبا بالعالم"), 
            makeString("مرحبا")
        });
        assertTest("startsWith: Arabic text", result3.toBool() == true);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testStartsWith: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة endsWith / (EN) EndsWith Function Tests
// ============================================================================
void testEndsWith() {
    printSection("11. Testing endsWith() Function");
    
    try {
        // Test 1: Ends with
        Value result1 = StringFunctions::endsWith({
            makeString("Hello World"), 
            makeString("World")
        });
        assertTest("endsWith: Does end with", result1.toBool() == true);
        
        // Test 2: Doesn't end with
        Value result2 = StringFunctions::endsWith({
            makeString("Hello World"), 
            makeString("Hello")
        });
        assertTest("endsWith: Doesn't end with", result2.toBool() == false);
        
        // Test 3: Arabic text
        Value result3 = StringFunctions::endsWith({
            makeString("مرحبا بالعالم"), 
            makeString("بالعالم")
        });
        assertTest("endsWith: Arabic text", result3.toBool() == true);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testEndsWith: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة contains / (EN) Contains Function Tests
// ============================================================================
void testContains() {
    printSection("12. Testing contains() Function");
    
    try {
        // Test 1: Contains
        Value result1 = StringFunctions::contains({
            makeString("Hello Beautiful World"), 
            makeString("Beautiful")
        });
        assertTest("contains: Does contain", result1.toBool() == true);
        
        // Test 2: Doesn't contain
        Value result2 = StringFunctions::contains({
            makeString("Hello World"), 
            makeString("xyz")
        });
        assertTest("contains: Doesn't contain", result2.toBool() == false);
        
        // Test 3: Arabic text
        Value result3 = StringFunctions::contains({
            makeString("مرحبا بالعالم الجميل"), 
            makeString("العالم")
        });
        assertTest("contains: Arabic text", result3.toBool() == true);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testContains: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات التكامل / (EN) Integration Tests
// ============================================================================
void testIntegration() {
    printSection("13. Integration Tests");
    
    try {
        // Test 1: trim + toLower
        Value temp1 = StringFunctions::trim({makeString("   HELLO WORLD   ")});
        Value result1 = StringFunctions::toLower({temp1});
        assertTest("Integration: trim + toLower", result1.toString() == "hello world");
        
        // Test 2: split + join
        Value temp2 = StringFunctions::split({makeString("a,b,c"), makeString(",")});
        Value result2 = StringFunctions::join({temp2, makeString("-")});
        assertTest("Integration: split + join", result2.toString().find("-") != std::string::npos);
        
        // Test 3: contains + replace
        std::string text = "Hello Friend";
        Value temp3 = StringFunctions::contains({makeString(text), makeString("Friend")});
        if (temp3.toBool()) {
            Value result3 = StringFunctions::replace({
                makeString(text), 
                makeString("Friend"), 
                makeString("World")
            });
            assertTest("Integration: contains + replace", 
                       result3.toString().find("World") != std::string::npos);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testIntegration: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) الدالة الرئيسية / (EN) Main Function
// ============================================================================
int main() {
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║    String Functions Comprehensive Test Suite              ║\n";
    std::cout << "║    اختبارات شاملة لدوال النصوص                           ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    
    // Run all tests
    testLength();
    testFind();
    testReplace();
    testSubstring();
    testToLower();
    testToUpper();
    testTrim();
    testSplit();
    testJoin();
    testStartsWith();
    testEndsWith();
    testContains();
    testIntegration();
    
    // Print final results
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                   Final Results / النتائج النهائية        ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "📊 Test Statistics / إحصائيات الاختبارات:\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "   Total Tests / إجمالي الاختبارات: " << totalTests << "\n";
    std::cout << "   ✅ Passed / نجحت: " << passedTests << "\n";
    std::cout << "   ❌ Failed / فشلت: " << failedTests << "\n";
    
    double successRate = (totalTests > 0) ? (double)passedTests / totalTests * 100.0 : 0.0;
    std::cout << "   📈 Success Rate / نسبة النجاح: " << successRate << "%\n";
    std::cout << "\n";
    
    if (failedTests == 0) {
        std::cout << "🎉 Excellent! All tests passed!\n";
        std::cout << "🎉 ممتاز! جميع الاختبارات نجحت!\n";
        return 0;
    } else {
        std::cout << "⚠️  Some tests failed, please review\n";
        std::cout << "⚠️  يوجد اختبارات فاشلة، يرجى المراجعة\n";
        return 1;
    }
}
