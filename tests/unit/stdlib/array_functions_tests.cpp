/**
 * @file array_functions_tests.cpp
 * @brief (AR) اختبارات شاملة لدوال المصفوفات / (EN) Comprehensive Array Functions Tests
 * 
 * هذا الملف يحتوي على اختبارات شاملة لجميع دوال المصفوفات الـ 10
 * This file contains comprehensive tests for all 10 array functions
 * 
 * @author S Language Development Team
 * @date November 22, 2025
 * @version 1.0
 */

#include "core/array_functions.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>

using namespace Sad::StdLib::Core;
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

void printSection(const std::string& title) {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  " << title;
    int padding = 58 - title.length();
    for (int i = 0; i < padding; i++) std::cout << " ";
    std::cout << "║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
}

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

Value makeString(const std::string& str) { return Value(str); }
Value makeInt(int num) { return Value(num); }
Value makeBool(bool b) { return Value(b); }
Value makeDouble(double d) { return Value(d); }
Value makeArray(const std::vector<Value>& arr) { return Value(arr); }

// ============================================================================
// (AR) اختبارات دالة append / (EN) Append Function Tests
// ============================================================================
void testAppend() {
    printSection("1. Testing append() Function");
    
    try {
        // Test 1: Append to integer array
        std::vector<Value> arr1 = {makeInt(1), makeInt(2), makeInt(3)};
        Value result1 = ArrayFunctions::append({makeArray(arr1), makeInt(4)});
        assertTest("append: Add integer to array", result1.toArray().size() == 4);
        
        // Test 2: Append to string array
        std::vector<Value> arr2 = {makeString("a"), makeString("b")};
        Value result2 = ArrayFunctions::append({makeArray(arr2), makeString("c")});
        assertTest("append: Add string to array", result2.toArray().size() == 3);
        
        // Test 3: Append to empty array
        std::vector<Value> arr3;
        Value result3 = ArrayFunctions::append({makeArray(arr3), makeInt(1)});
        assertTest("append: Add to empty array", result3.toArray().size() == 1);
        
        // Test 4: Append mixed type
        std::vector<Value> arr4 = {makeInt(1), makeString("test")};
        Value result4 = ArrayFunctions::append({makeArray(arr4), makeBool(true)});
        assertTest("append: Add to mixed array", result4.toArray().size() == 3);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testAppend: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة pop / (EN) Pop Function Tests
// ============================================================================
void testPop() {
    printSection("2. Testing pop() Function");
    
    try {
        // Test 1: Pop from integer array
        std::vector<Value> arr1 = {makeInt(1), makeInt(2), makeInt(3)};
        Value result1 = ArrayFunctions::pop({makeArray(arr1)});
        assertTest("pop: Remove last element", result1.toInt() == 3);
        
        // Test 2: Pop from string array
        std::vector<Value> arr2 = {makeString("a"), makeString("b"), makeString("c")};
        Value result2 = ArrayFunctions::pop({makeArray(arr2)});
        assertTest("pop: Remove last string", result2.toString() == "c");
        
        // Test 3: Pop from single element
        std::vector<Value> arr3 = {makeInt(42)};
        Value result3 = ArrayFunctions::pop({makeArray(arr3)});
        assertTest("pop: Single element", result3.toInt() == 42);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testPop: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة size / (EN) Size Function Tests
// ============================================================================
void testSize() {
    printSection("3. Testing size() Function");
    
    try {
        // Test 1: Non-empty array
        std::vector<Value> arr1 = {makeInt(1), makeInt(2), makeInt(3), makeInt(4), makeInt(5)};
        Value result1 = ArrayFunctions::size({makeArray(arr1)});
        assertTest("size: 5 elements", result1.toInt() == 5);
        
        // Test 2: Empty array
        std::vector<Value> arr2;
        Value result2 = ArrayFunctions::size({makeArray(arr2)});
        assertTest("size: Empty array", result2.toInt() == 0);
        
        // Test 3: Single element
        std::vector<Value> arr3 = {makeString("test")};
        Value result3 = ArrayFunctions::size({makeArray(arr3)});
        assertTest("size: Single element", result3.toInt() == 1);
        
        // Test 4: Mixed types
        std::vector<Value> arr4 = {makeInt(1), makeString("text"), makeBool(true), makeDouble(3.14)};
        Value result4 = ArrayFunctions::size({makeArray(arr4)});
        assertTest("size: Mixed types", result4.toInt() == 4);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testSize: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة indexOf / (EN) IndexOf Function Tests
// ============================================================================
void testIndexOf() {
    printSection("4. Testing indexOf() Function");
    
    try {
        // Test 1: Find existing element
        std::vector<Value> arr1 = {makeInt(10), makeInt(20), makeInt(30), makeInt(40)};
        Value result1 = ArrayFunctions::indexOf({makeArray(arr1), makeInt(30)});
        assertTest("indexOf: Found at index 2", result1.toInt() == 2);
        
        // Test 2: Element not found
        Value result2 = ArrayFunctions::indexOf({makeArray(arr1), makeInt(99)});
        assertTest("indexOf: Not found returns -1", result2.toInt() == -1);
        
        // Test 3: Find at beginning
        Value result3 = ArrayFunctions::indexOf({makeArray(arr1), makeInt(10)});
        assertTest("indexOf: At beginning", result3.toInt() == 0);
        
        // Test 4: Find with start position
        Value result4 = ArrayFunctions::indexOf({makeArray(arr1), makeInt(30), makeInt(1)});
        assertTest("indexOf: With start position", result4.toInt() >= 1);
        
        // Test 5: String array
        std::vector<Value> arr2 = {makeString("apple"), makeString("banana"), makeString("cherry")};
        Value result5 = ArrayFunctions::indexOf({makeArray(arr2), makeString("banana")});
        assertTest("indexOf: String found", result5.toInt() == 1);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testIndexOf: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة contains / (EN) Contains Function Tests
// ============================================================================
void testContains() {
    printSection("5. Testing contains() Function");
    
    try {
        // Test 1: Contains integer
        std::vector<Value> arr1 = {makeInt(5), makeInt(10), makeInt(15), makeInt(20)};
        Value result1 = ArrayFunctions::contains({makeArray(arr1), makeInt(10)});
        assertTest("contains: Integer present", result1.toBool() == true);
        
        // Test 2: Doesn't contain
        Value result2 = ArrayFunctions::contains({makeArray(arr1), makeInt(99)});
        assertTest("contains: Integer not present", result2.toBool() == false);
        
        // Test 3: Contains string
        std::vector<Value> arr2 = {makeString("hello"), makeString("world"), makeString("test")};
        Value result3 = ArrayFunctions::contains({makeArray(arr2), makeString("world")});
        assertTest("contains: String present", result3.toBool() == true);
        
        // Test 4: Empty array
        std::vector<Value> arr3;
        Value result4 = ArrayFunctions::contains({makeArray(arr3), makeInt(1)});
        assertTest("contains: Empty array", result4.toBool() == false);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testContains: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة reverse / (EN) Reverse Function Tests
// ============================================================================
void testReverse() {
    printSection("6. Testing reverse() Function");
    
    try {
        // Test 1: Reverse integers
        std::vector<Value> arr1 = {makeInt(1), makeInt(2), makeInt(3), makeInt(4)};
        Value result1 = ArrayFunctions::reverse({makeArray(arr1)});
        std::vector<Value> reversed1 = result1.toArray();
        assertTest("reverse: Integer array", 
                   reversed1[0].toInt() == 4 && reversed1[3].toInt() == 1);
        
        // Test 2: Reverse strings
        std::vector<Value> arr2 = {makeString("a"), makeString("b"), makeString("c")};
        Value result2 = ArrayFunctions::reverse({makeArray(arr2)});
        std::vector<Value> reversed2 = result2.toArray();
        assertTest("reverse: String array", 
                   reversed2[0].toString() == "c" && reversed2[2].toString() == "a");
        
        // Test 3: Single element
        std::vector<Value> arr3 = {makeInt(42)};
        Value result3 = ArrayFunctions::reverse({makeArray(arr3)});
        assertTest("reverse: Single element", result3.toArray().size() == 1);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testReverse: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة sort / (EN) Sort Function Tests
// ============================================================================
void testSort() {
    printSection("7. Testing sort() Function");
    
    try {
        // Test 1: Sort ascending
        std::vector<Value> arr1 = {makeInt(5), makeInt(2), makeInt(8), makeInt(1), makeInt(9)};
        Value result1 = ArrayFunctions::sort({makeArray(arr1), makeBool(true)});
        std::vector<Value> sorted1 = result1.toArray();
        assertTest("sort: Ascending order", 
                   sorted1[0].toInt() == 1 && sorted1[4].toInt() == 9);
        
        // Test 2: Sort descending
        Value result2 = ArrayFunctions::sort({makeArray(arr1), makeBool(false)});
        std::vector<Value> sorted2 = result2.toArray();
        assertTest("sort: Descending order", 
                   sorted2[0].toInt() == 9 && sorted2[4].toInt() == 1);
        
        // Test 3: Sort strings
        std::vector<Value> arr2 = {makeString("zebra"), makeString("apple"), makeString("mango")};
        Value result3 = ArrayFunctions::sort({makeArray(arr2), makeBool(true)});
        std::vector<Value> sorted3 = result3.toArray();
        assertTest("sort: String ascending", sorted3[0].toString() == "apple");
        
        // Test 4: Already sorted
        std::vector<Value> arr3 = {makeInt(1), makeInt(2), makeInt(3)};
        Value result4 = ArrayFunctions::sort({makeArray(arr3), makeBool(true)});
        assertTest("sort: Already sorted", result4.toArray().size() == 3);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testSort: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة first / (EN) First Function Tests
// ============================================================================
void testFirst() {
    printSection("8. Testing first() Function");
    
    try {
        // Test 1: First of integers
        std::vector<Value> arr1 = {makeInt(10), makeInt(20), makeInt(30)};
        Value result1 = ArrayFunctions::first({makeArray(arr1)});
        assertTest("first: Integer array", result1.toInt() == 10);
        
        // Test 2: First of strings
        std::vector<Value> arr2 = {makeString("first"), makeString("second"), makeString("third")};
        Value result2 = ArrayFunctions::first({makeArray(arr2)});
        assertTest("first: String array", result2.toString() == "first");
        
        // Test 3: Single element
        std::vector<Value> arr3 = {makeInt(42)};
        Value result3 = ArrayFunctions::first({makeArray(arr3)});
        assertTest("first: Single element", result3.toInt() == 42);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testFirst: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة last / (EN) Last Function Tests
// ============================================================================
void testLast() {
    printSection("9. Testing last() Function");
    
    try {
        // Test 1: Last of integers
        std::vector<Value> arr1 = {makeInt(10), makeInt(20), makeInt(30)};
        Value result1 = ArrayFunctions::last({makeArray(arr1)});
        assertTest("last: Integer array", result1.toInt() == 30);
        
        // Test 2: Last of strings
        std::vector<Value> arr2 = {makeString("first"), makeString("second"), makeString("last")};
        Value result2 = ArrayFunctions::last({makeArray(arr2)});
        assertTest("last: String array", result2.toString() == "last");
        
        // Test 3: Single element
        std::vector<Value> arr3 = {makeInt(42)};
        Value result3 = ArrayFunctions::last({makeArray(arr3)});
        assertTest("last: Single element", result3.toInt() == 42);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testLast: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة slice / (EN) Slice Function Tests
// ============================================================================
void testSlice() {
    printSection("10. Testing slice() Function");
    
    try {
        // Test 1: Slice from middle
        std::vector<Value> arr1 = {makeInt(0), makeInt(1), makeInt(2), makeInt(3), makeInt(4)};
        Value result1 = ArrayFunctions::slice({makeArray(arr1), makeInt(1), makeInt(4)});
        std::vector<Value> sliced1 = result1.toArray();
        assertTest("slice: Middle section", sliced1.size() == 3 && sliced1[0].toInt() == 1);
        
        // Test 2: Slice from beginning
        Value result2 = ArrayFunctions::slice({makeArray(arr1), makeInt(0), makeInt(2)});
        std::vector<Value> sliced2 = result2.toArray();
        assertTest("slice: From beginning", sliced2.size() == 2 && sliced2[0].toInt() == 0);
        
        // Test 3: Slice to end
        Value result3 = ArrayFunctions::slice({makeArray(arr1), makeInt(3)});
        std::vector<Value> sliced3 = result3.toArray();
        assertTest("slice: To end", sliced3.size() == 2);
        
        // Test 4: String array slice
        std::vector<Value> arr2 = {makeString("a"), makeString("b"), makeString("c"), makeString("d")};
        Value result4 = ArrayFunctions::slice({makeArray(arr2), makeInt(1), makeInt(3)});
        std::vector<Value> sliced4 = result4.toArray();
        assertTest("slice: String array", sliced4.size() == 2 && sliced4[0].toString() == "b");
        
        // Test 5: Empty slice
        Value result5 = ArrayFunctions::slice({makeArray(arr1), makeInt(2), makeInt(2)});
        assertTest("slice: Empty result", result5.toArray().size() == 0);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testSlice: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات التكامل / (EN) Integration Tests
// ============================================================================
void testIntegration() {
    printSection("11. Integration Tests");
    
    try {
        // Test 1: append + sort
        std::vector<Value> arr1 = {makeInt(5), makeInt(2)};
        Value temp1 = ArrayFunctions::append({makeArray(arr1), makeInt(8)});
        Value result1 = ArrayFunctions::sort({temp1, makeBool(true)});
        std::vector<Value> final1 = result1.toArray();
        assertTest("Integration: append + sort", 
                   final1.size() == 3 && final1[0].toInt() == 2);
        
        // Test 2: reverse + first
        std::vector<Value> arr2 = {makeInt(1), makeInt(2), makeInt(3)};
        Value temp2 = ArrayFunctions::reverse({makeArray(arr2)});
        Value result2 = ArrayFunctions::first({temp2});
        assertTest("Integration: reverse + first", result2.toInt() == 3);
        
        // Test 3: slice + size
        std::vector<Value> arr3 = {makeInt(0), makeInt(1), makeInt(2), makeInt(3), makeInt(4)};
        Value temp3 = ArrayFunctions::slice({makeArray(arr3), makeInt(1), makeInt(4)});
        Value result3 = ArrayFunctions::size({temp3});
        assertTest("Integration: slice + size", result3.toInt() == 3);
        
        // Test 4: contains + indexOf
        std::vector<Value> arr4 = {makeString("apple"), makeString("banana"), makeString("cherry")};
        Value contains_result = ArrayFunctions::contains({makeArray(arr4), makeString("banana")});
        if (contains_result.toBool()) {
            Value index_result = ArrayFunctions::indexOf({makeArray(arr4), makeString("banana")});
            assertTest("Integration: contains + indexOf", index_result.toInt() == 1);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testIntegration: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات حالات خاصة / (EN) Edge Cases Tests
// ============================================================================
void testEdgeCases() {
    printSection("12. Edge Cases Tests");
    
    try {
        // Test 1: Large array
        std::vector<Value> largeArr;
        for (int i = 0; i < 1000; i++) {
            largeArr.push_back(makeInt(i));
        }
        Value result1 = ArrayFunctions::size({makeArray(largeArr)});
        assertTest("Edge: Large array (1000 elements)", result1.toInt() == 1000);
        
        // Test 2: Mixed types array
        std::vector<Value> mixedArr = {
            makeInt(42),
            makeString("text"),
            makeBool(true),
            makeDouble(3.14)
        };
        Value result2 = ArrayFunctions::size({makeArray(mixedArr)});
        assertTest("Edge: Mixed types", result2.toInt() == 4);
        
        // Test 3: Nested operations
        std::vector<Value> arr3 = {makeInt(3), makeInt(1), makeInt(4), makeInt(1), makeInt(5)};
        Value temp3 = ArrayFunctions::sort({makeArray(arr3), makeBool(true)});
        Value temp4 = ArrayFunctions::slice({temp3, makeInt(1), makeInt(4)});
        Value result3 = ArrayFunctions::size({temp4});
        assertTest("Edge: Nested operations", result3.toInt() == 3);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testEdgeCases: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) الدالة الرئيسية / (EN) Main Function
// ============================================================================
int main() {
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║    Array Functions Comprehensive Test Suite               ║\n";
    std::cout << "║    اختبارات شاملة لدوال المصفوفات                        ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    
    // Run all tests
    testAppend();
    testPop();
    testSize();
    testIndexOf();
    testContains();
    testReverse();
    testSort();
    testFirst();
    testLast();
    testSlice();
    testIntegration();
    testEdgeCases();
    
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
