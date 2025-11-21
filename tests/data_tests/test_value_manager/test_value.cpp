/**
 * @file test_value.cpp
 * @brief (AR) اختبارات شاملة لفئة Value
 * @brief (EN) Comprehensive tests for Value class
 * 
 * Tests all Value operations:
 * - Construction and type checking
 * - Type conversion
 * - Arithmetic operators
 * - Comparison operators
 * - Logical operators
 * - Increment/decrement
 * - Compound assignment
 * - Error handling
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include "../../include/data/types/value.h"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace Sad::Data;

// Test counters
int tests_passed = 0;
int tests_failed = 0;

#define TEST(name) \
    void name(); \
    void name##_runner() { \
        try { \
            name(); \
            tests_passed++; \
            std::cout << "  ✅ PASS: " << #name << "\n"; \
        } catch (const std::exception& e) { \
            tests_failed++; \
            std::cout << "  ❌ FAIL: " << #name << " - " << e.what() << "\n"; \
        } \
    } \
    void name()

// ========================================
// Construction Tests / اختبارات الإنشاء
// ========================================

TEST(test_construct_void) {
    Value v;
    assert(v.isVoid());
    assert(v.getType() == ValueType::VOID);
}

TEST(test_construct_integer) {
    Value v(42);
    assert(v.isInteger());
    assert(v.toInt() == 42);
}

TEST(test_construct_double) {
    Value v(3.14);
    assert(v.isDouble());
    assert(std::abs(v.toDouble() - 3.14) < 0.001);
}

TEST(test_construct_string) {
    Value v("مرحبا");
    assert(v.isString());
    assert(v.toString() == "مرحبا");
}

TEST(test_construct_boolean) {
    Value v1(true);
    Value v2(false);
    assert(v1.isBoolean());
    assert(v2.isBoolean());
    assert(v1.toBool() == true);
    assert(v2.toBool() == false);
}

// ========================================
// Type Conversion Tests / اختبارات تحويل النوع
// ========================================

TEST(test_int_to_double) {
    Value v(10);
    assert(std::abs(v.toDouble() - 10.0) < 0.001);
}

TEST(test_double_to_int) {
    Value v(3.7);
    assert(v.toInt() == 3);
}

TEST(test_bool_to_int) {
    Value v1(true);
    Value v2(false);
    assert(v1.toInt() == 1);
    assert(v2.toInt() == 0);
}

TEST(test_int_to_string) {
    Value v(42);
    assert(v.toString() == "42");
}

TEST(test_bool_to_string) {
    Value v1(true);
    Value v2(false);
    assert(v1.toString() == "true");
    assert(v2.toString() == "false");
}

TEST(test_string_to_bool) {
    Value v1("hello");
    Value v2("");
    assert(v1.toBool() == true);
    assert(v2.toBool() == false);
}

TEST(test_numeric_to_bool) {
    Value v1(0);
    Value v2(42);
    Value v3(0.0);
    Value v4(3.14);
    assert(v1.toBool() == false);
    assert(v2.toBool() == true);
    assert(v3.toBool() == false);
    assert(v4.toBool() == true);
}

// ========================================
// Arithmetic Operators / العوامل الحسابية
// ========================================

TEST(test_add_integers) {
    Value v1(10);
    Value v2(20);
    Value result = v1 + v2;
    assert(result.isInteger());
    assert(result.toInt() == 30);
}

TEST(test_add_doubles) {
    Value v1(1.5);
    Value v2(2.5);
    Value result = v1 + v2;
    assert(result.isDouble());
    assert(std::abs(result.toDouble() - 4.0) < 0.001);
}

TEST(test_add_int_and_double) {
    Value v1(10);
    Value v2(2.5);
    Value result = v1 + v2;
    assert(result.isDouble());
    assert(std::abs(result.toDouble() - 12.5) < 0.001);
}

TEST(test_add_strings) {
    Value v1("Hello ");
    Value v2("World");
    Value result = v1 + v2;
    assert(result.isString());
    assert(result.toString() == "Hello World");
}

TEST(test_string_concatenation_with_number) {
    Value v1("Value: ");
    Value v2(42);
    Value result = v1 + v2;
    assert(result.isString());
    assert(result.toString() == "Value: 42");
}

TEST(test_subtract_integers) {
    Value v1(50);
    Value v2(20);
    Value result = v1 - v2;
    assert(result.toInt() == 30);
}

TEST(test_multiply_integers) {
    Value v1(6);
    Value v2(7);
    Value result = v1 * v2;
    assert(result.toInt() == 42);
}

TEST(test_divide_integers) {
    Value v1(20);
    Value v2(4);
    Value result = v1 / v2;
    assert(result.toInt() == 5);
}

TEST(test_divide_doubles) {
    Value v1(10.0);
    Value v2(4.0);
    Value result = v1 / v2;
    assert(std::abs(result.toDouble() - 2.5) < 0.001);
}

TEST(test_modulus) {
    Value v1(17);
    Value v2(5);
    Value result = v1 % v2;
    assert(result.toInt() == 2);
}

TEST(test_unary_minus) {
    Value v1(42);
    Value v2(3.14);
    Value result1 = -v1;
    Value result2 = -v2;
    assert(result1.toInt() == -42);
    assert(std::abs(result2.toDouble() + 3.14) < 0.001);
}

// ========================================
// Comparison Operators / عوامل المقارنة
// ========================================

TEST(test_equality_integers) {
    Value v1(42);
    Value v2(42);
    Value v3(43);
    assert((v1 == v2).toBool() == true);
    assert((v1 == v3).toBool() == false);
}

TEST(test_equality_mixed_numeric) {
    Value v1(42);
    Value v2(42.0);
    assert((v1 == v2).toBool() == true);
}

TEST(test_equality_strings) {
    Value v1("hello");
    Value v2("hello");
    Value v3("world");
    assert((v1 == v2).toBool() == true);
    assert((v1 == v3).toBool() == false);
}

TEST(test_inequality) {
    Value v1(10);
    Value v2(20);
    assert((v1 != v2).toBool() == true);
    assert((v1 != v1).toBool() == false);
}

TEST(test_less_than) {
    Value v1(10);
    Value v2(20);
    assert((v1 < v2).toBool() == true);
    assert((v2 < v1).toBool() == false);
}

TEST(test_greater_than) {
    Value v1(20);
    Value v2(10);
    assert((v1 > v2).toBool() == true);
    assert((v2 > v1).toBool() == false);
}

TEST(test_less_equal) {
    Value v1(10);
    Value v2(20);
    Value v3(10);
    assert((v1 <= v2).toBool() == true);
    assert((v1 <= v3).toBool() == true);
    assert((v2 <= v1).toBool() == false);
}

TEST(test_greater_equal) {
    Value v1(20);
    Value v2(10);
    Value v3(20);
    assert((v1 >= v2).toBool() == true);
    assert((v1 >= v3).toBool() == true);
    assert((v2 >= v1).toBool() == false);
}

TEST(test_string_comparison) {
    Value v1("apple");
    Value v2("banana");
    assert((v1 < v2).toBool() == true);
    assert((v2 > v1).toBool() == true);
}

// ========================================
// Logical Operators / العوامل المنطقية
// ========================================

TEST(test_logical_and) {
    Value v1(true);
    Value v2(false);
    assert((v1 && v1).toBool() == true);
    assert((v1 && v2).toBool() == false);
    assert((v2 && v2).toBool() == false);
}

TEST(test_logical_or) {
    Value v1(true);
    Value v2(false);
    assert((v1 || v1).toBool() == true);
    assert((v1 || v2).toBool() == true);
    assert((v2 || v2).toBool() == false);
}

TEST(test_logical_not) {
    Value v1(true);
    Value v2(false);
    assert((!v1).toBool() == false);
    assert((!v2).toBool() == true);
}

TEST(test_logical_with_numbers) {
    Value v1(0);
    Value v2(42);
    assert((v1 || v2).toBool() == true);
    assert((v1 && v2).toBool() == false);
}

// ========================================
// Increment/Decrement / الزيادة/النقصان
// ========================================

TEST(test_pre_increment) {
    Value v(10);
    ++v;
    assert(v.toInt() == 11);
}

TEST(test_post_increment) {
    Value v(10);
    Value old = v++;
    assert(old.toInt() == 10);
    assert(v.toInt() == 11);
}

TEST(test_pre_decrement) {
    Value v(10);
    --v;
    assert(v.toInt() == 9);
}

TEST(test_post_decrement) {
    Value v(10);
    Value old = v--;
    assert(old.toInt() == 10);
    assert(v.toInt() == 9);
}

TEST(test_increment_double) {
    Value v(3.14);
    ++v;
    assert(std::abs(v.toDouble() - 4.14) < 0.001);
}

// ========================================
// Compound Assignment / التعيين المركب
// ========================================

TEST(test_add_assign) {
    Value v(10);
    v += Value(5);
    assert(v.toInt() == 15);
}

TEST(test_subtract_assign) {
    Value v(10);
    v -= Value(3);
    assert(v.toInt() == 7);
}

TEST(test_multiply_assign) {
    Value v(5);
    v *= Value(3);
    assert(v.toInt() == 15);
}

TEST(test_divide_assign) {
    Value v(20);
    v /= Value(4);
    assert(v.toInt() == 5);
}

// ========================================
// Error Handling / معالجة الأخطاء
// ========================================

TEST(test_division_by_zero) {
    Value v1(10);
    Value v2(0);
    bool caught = false;
    try {
        Value result = v1 / v2;
    } catch (const std::runtime_error& e) {
        caught = true;
        std::string msg = e.what();
        assert(msg.find("Division by zero") != std::string::npos ||
               msg.find("القسمة على صفر") != std::string::npos);
    }
    assert(caught);
}

TEST(test_modulus_by_zero) {
    Value v1(10);
    Value v2(0);
    bool caught = false;
    try {
        Value result = v1 % v2;
    } catch (const std::runtime_error& e) {
        caught = true;
    }
    assert(caught);
}

TEST(test_invalid_operation) {
    Value v1("hello");
    Value v2(42);
    bool caught = false;
    try {
        Value result = v1 - v2;  // Can't subtract number from string
    } catch (const std::runtime_error& e) {
        caught = true;
    }
    assert(caught);
}

// ========================================
// Debug Methods / دوال التصحيح
// ========================================

TEST(test_debug_string) {
    Value v(42);
    std::string debug = v.debugString();
    assert(debug.find("INTEGER") != std::string::npos);
    assert(debug.find("42") != std::string::npos);
}

TEST(test_type_names) {
    Value v1;
    Value v2(42);
    Value v3(3.14);
    Value v4("test");
    Value v5(true);
    
    assert(v1.getTypeName() == "VOID");
    assert(v2.getTypeName() == "INTEGER");
    assert(v3.getTypeName() == "DOUBLE");
    assert(v4.getTypeName() == "STRING");
    assert(v5.getTypeName() == "BOOLEAN");
}

// ========================================
// Main Test Runner / مدير الاختبارات
// ========================================

int main() {
    std::cout << "\n╔════════════════════════════════════════════════════╗\n";
    std::cout << "║   VALUE CLASS COMPREHENSIVE TEST SUITE           ║\n";
    std::cout << "║   (AR) مجموعة اختبار شاملة لفئة Value             ║\n";
    std::cout << "╚════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "=== Construction Tests ===\n";
    test_construct_void_runner();
    test_construct_integer_runner();
    test_construct_double_runner();
    test_construct_string_runner();
    test_construct_boolean_runner();
    
    std::cout << "\n=== Type Conversion Tests ===\n";
    test_int_to_double_runner();
    test_double_to_int_runner();
    test_bool_to_int_runner();
    test_int_to_string_runner();
    test_bool_to_string_runner();
    test_string_to_bool_runner();
    test_numeric_to_bool_runner();
    
    std::cout << "\n=== Arithmetic Operators ===\n";
    test_add_integers_runner();
    test_add_doubles_runner();
    test_add_int_and_double_runner();
    test_add_strings_runner();
    test_string_concatenation_with_number_runner();
    test_subtract_integers_runner();
    test_multiply_integers_runner();
    test_divide_integers_runner();
    test_divide_doubles_runner();
    test_modulus_runner();
    test_unary_minus_runner();
    
    std::cout << "\n=== Comparison Operators ===\n";
    test_equality_integers_runner();
    test_equality_mixed_numeric_runner();
    test_equality_strings_runner();
    test_inequality_runner();
    test_less_than_runner();
    test_greater_than_runner();
    test_less_equal_runner();
    test_greater_equal_runner();
    test_string_comparison_runner();
    
    std::cout << "\n=== Logical Operators ===\n";
    test_logical_and_runner();
    test_logical_or_runner();
    test_logical_not_runner();
    test_logical_with_numbers_runner();
    
    std::cout << "\n=== Increment/Decrement ===\n";
    test_pre_increment_runner();
    test_post_increment_runner();
    test_pre_decrement_runner();
    test_post_decrement_runner();
    test_increment_double_runner();
    
    std::cout << "\n=== Compound Assignment ===\n";
    test_add_assign_runner();
    test_subtract_assign_runner();
    test_multiply_assign_runner();
    test_divide_assign_runner();
    
    std::cout << "\n=== Error Handling ===\n";
    test_division_by_zero_runner();
    test_modulus_by_zero_runner();
    test_invalid_operation_runner();
    
    std::cout << "\n=== Debug Methods ===\n";
    test_debug_string_runner();
    test_type_names_runner();
    
    // Print summary
    std::cout << "\n╔════════════════════════════════════════════════════╗\n";
    if (tests_failed == 0) {
        std::cout << "║   ✅ ALL TESTS PASSED!                            ║\n";
        std::cout << "║   ✅ جميع الاختبارات نجحت!                       ║\n";
    } else {
        std::cout << "║   ❌ SOME TESTS FAILED                            ║\n";
        std::cout << "║   ❌ بعض الاختبارات فشلت                         ║\n";
    }
    std::cout << "╚════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "📊 Results / النتائج:\n";
    std::cout << "  ✅ Passed: " << tests_passed << "\n";
    std::cout << "  ❌ Failed: " << tests_failed << "\n";
    std::cout << "  📈 Total:  " << (tests_passed + tests_failed) << "\n";
    std::cout << "  📊 Success Rate: " 
              << (100.0 * tests_passed / (tests_passed + tests_failed)) << "%\n\n";
    
    return tests_failed > 0 ? 1 : 0;
}
