/**
 * @file math_functions_tests.cpp
 * @brief (AR) اختبارات شاملة للدوال الرياضية / (EN) Comprehensive Math Functions Tests
 * 
 * هذا الملف يحتوي على اختبارات شاملة لجميع الدوال الرياضية الـ 12
 * This file contains comprehensive tests for all 12 math functions
 * 
 * @author S Language Development Team
 * @date November 22, 2025
 * @version 1.0
 */

#include "math/math_functions.h"
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <string>

using namespace Sad::StdLib::Math;
using namespace Sad::Data;

// ============================================================================
// (AR) متغيرات عامة لتتبع الاختبارات / (EN) Global test tracking variables
// ============================================================================
int totalTests = 0;
int passedTests = 0;
int failedTests = 0;

const double EPSILON = 0.0001; // For floating point comparisons

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

bool areClose(double a, double b, double epsilon = EPSILON) {
    return std::abs(a - b) < epsilon;
}

Value makeInt(int num) { return Value(num); }
Value makeDouble(double d) { return Value(d); }
Value makeBool(bool b) { return Value(b); }

// ============================================================================
// (AR) اختبارات دالة sqrt / (EN) Sqrt Function Tests
// ============================================================================
void testSqrt() {
    printSection("1. Testing sqrt() Function");
    
    try {
        // Test 1: Perfect square
        Value result1 = MathFunctions::sqrt({makeDouble(16.0)});
        assertTest("sqrt: Perfect square (16)", areClose(result1.toDouble(), 4.0));
        
        // Test 2: Non-perfect square
        Value result2 = MathFunctions::sqrt({makeDouble(2.0)});
        assertTest("sqrt: Non-perfect square (2)", areClose(result2.toDouble(), 1.4142, 0.001));
        
        // Test 3: Square root of 0
        Value result3 = MathFunctions::sqrt({makeDouble(0.0)});
        assertTest("sqrt: Zero", areClose(result3.toDouble(), 0.0));
        
        // Test 4: Large number
        Value result4 = MathFunctions::sqrt({makeDouble(100.0)});
        assertTest("sqrt: Large number (100)", areClose(result4.toDouble(), 10.0));
        
        // Test 5: Small decimal
        Value result5 = MathFunctions::sqrt({makeDouble(0.25)});
        assertTest("sqrt: Small decimal (0.25)", areClose(result5.toDouble(), 0.5));
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testSqrt: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة power / (EN) Power Function Tests
// ============================================================================
void testPower() {
    printSection("2. Testing power() Function");
    
    try {
        // Test 1: Integer powers
        Value result1 = MathFunctions::power({makeDouble(2.0), makeDouble(3.0)});
        assertTest("power: 2^3 = 8", areClose(result1.toDouble(), 8.0));
        
        // Test 2: Power of 0
        Value result2 = MathFunctions::power({makeDouble(5.0), makeDouble(0.0)});
        assertTest("power: n^0 = 1", areClose(result2.toDouble(), 1.0));
        
        // Test 3: Power of 1
        Value result3 = MathFunctions::power({makeDouble(7.0), makeDouble(1.0)});
        assertTest("power: n^1 = n", areClose(result3.toDouble(), 7.0));
        
        // Test 4: Negative exponent
        Value result4 = MathFunctions::power({makeDouble(2.0), makeDouble(-2.0)});
        assertTest("power: 2^-2 = 0.25", areClose(result4.toDouble(), 0.25));
        
        // Test 5: Fractional exponent
        Value result5 = MathFunctions::power({makeDouble(4.0), makeDouble(0.5)});
        assertTest("power: 4^0.5 = 2", areClose(result5.toDouble(), 2.0));
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testPower: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة abs / (EN) Abs Function Tests
// ============================================================================
void testAbs() {
    printSection("3. Testing abs() Function");
    
    try {
        // Test 1: Negative integer
        Value result1 = MathFunctions::abs({makeInt(-15)});
        assertTest("abs: Negative integer (-15)", result1.toInt() == 15);
        
        // Test 2: Positive integer
        Value result2 = MathFunctions::abs({makeInt(20)});
        assertTest("abs: Positive integer (20)", result2.toInt() == 20);
        
        // Test 3: Zero
        Value result3 = MathFunctions::abs({makeInt(0)});
        assertTest("abs: Zero", result3.toInt() == 0);
        
        // Test 4: Negative double
        Value result4 = MathFunctions::abs({makeDouble(-3.14)});
        assertTest("abs: Negative double (-3.14)", areClose(result4.toDouble(), 3.14));
        
        // Test 5: Large negative
        Value result5 = MathFunctions::abs({makeInt(-1000)});
        assertTest("abs: Large negative (-1000)", result5.toInt() == 1000);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testAbs: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة square / (EN) Square Function Tests
// ============================================================================
void testSquare() {
    printSection("4. Testing square() Function");
    
    try {
        // Test 1: Positive integer
        Value result1 = MathFunctions::square({makeInt(5)});
        assertTest("square: 5^2 = 25", result1.toInt() == 25);
        
        // Test 2: Negative integer
        Value result2 = MathFunctions::square({makeInt(-4)});
        assertTest("square: (-4)^2 = 16", result2.toInt() == 16);
        
        // Test 3: Zero
        Value result3 = MathFunctions::square({makeInt(0)});
        assertTest("square: 0^2 = 0", result3.toInt() == 0);
        
        // Test 4: Decimal
        Value result4 = MathFunctions::square({makeDouble(2.5)});
        assertTest("square: 2.5^2 = 6.25", areClose(result4.toDouble(), 6.25));
        
        // Test 5: Large number
        Value result5 = MathFunctions::square({makeInt(10)});
        assertTest("square: 10^2 = 100", result5.toInt() == 100);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testSquare: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة max / (EN) Max Function Tests
// ============================================================================
void testMax() {
    printSection("5. Testing max() Function");
    
    try {
        // Test 1: Integers
        Value result1 = MathFunctions::max({makeInt(5), makeInt(10), makeInt(3), makeInt(20), makeInt(8)});
        assertTest("max: Multiple integers", result1.toInt() == 20);
        
        // Test 2: Two numbers
        Value result2 = MathFunctions::max({makeInt(15), makeInt(7)});
        assertTest("max: Two numbers", result2.toInt() == 15);
        
        // Test 3: Negative numbers
        Value result3 = MathFunctions::max({makeInt(-5), makeInt(-10), makeInt(-3)});
        assertTest("max: Negative numbers", result3.toInt() == -3);
        
        // Test 4: Mixed positive and negative
        Value result4 = MathFunctions::max({makeInt(-5), makeInt(10), makeInt(-20), makeInt(3)});
        assertTest("max: Mixed signs", result4.toInt() == 10);
        
        // Test 5: Doubles
        Value result5 = MathFunctions::max({makeDouble(3.5), makeDouble(2.7), makeDouble(4.2)});
        assertTest("max: Doubles", areClose(result5.toDouble(), 4.2));
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testMax: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة min / (EN) Min Function Tests
// ============================================================================
void testMin() {
    printSection("6. Testing min() Function");
    
    try {
        // Test 1: Integers
        Value result1 = MathFunctions::min({makeInt(5), makeInt(10), makeInt(3), makeInt(20), makeInt(8)});
        assertTest("min: Multiple integers", result1.toInt() == 3);
        
        // Test 2: Two numbers
        Value result2 = MathFunctions::min({makeInt(15), makeInt(7)});
        assertTest("min: Two numbers", result2.toInt() == 7);
        
        // Test 3: Negative numbers
        Value result3 = MathFunctions::min({makeInt(-5), makeInt(-10), makeInt(-3)});
        assertTest("min: Negative numbers", result3.toInt() == -10);
        
        // Test 4: Mixed positive and negative
        Value result4 = MathFunctions::min({makeInt(-5), makeInt(10), makeInt(-20), makeInt(3)});
        assertTest("min: Mixed signs", result4.toInt() == -20);
        
        // Test 5: Doubles
        Value result5 = MathFunctions::min({makeDouble(3.5), makeDouble(2.7), makeDouble(4.2)});
        assertTest("min: Doubles", areClose(result5.toDouble(), 2.7));
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testMin: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة round / (EN) Round Function Tests
// ============================================================================
void testRound() {
    printSection("7. Testing round() Function");
    
    try {
        // Test 1: Round up
        Value result1 = MathFunctions::round({makeDouble(3.7)});
        assertTest("round: 3.7 -> 4", result1.toInt() == 4);
        
        // Test 2: Round down
        Value result2 = MathFunctions::round({makeDouble(3.2)});
        assertTest("round: 3.2 -> 3", result2.toInt() == 3);
        
        // Test 3: Exactly .5
        Value result3 = MathFunctions::round({makeDouble(3.5)});
        assertTest("round: 3.5 -> 4", result3.toInt() == 4);
        
        // Test 4: Negative number
        Value result4 = MathFunctions::round({makeDouble(-2.7)});
        assertTest("round: -2.7 -> -3", result4.toInt() == -3);
        
        // Test 5: Integer input
        Value result5 = MathFunctions::round({makeDouble(5.0)});
        assertTest("round: Integer input", result5.toInt() == 5);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testRound: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة floor / (EN) Floor Function Tests
// ============================================================================
void testFloor() {
    printSection("8. Testing floor() Function");
    
    try {
        // Test 1: Positive decimal
        Value result1 = MathFunctions::floor({makeDouble(3.9)});
        assertTest("floor: 3.9 -> 3", result1.toInt() == 3);
        
        // Test 2: Positive small decimal
        Value result2 = MathFunctions::floor({makeDouble(3.1)});
        assertTest("floor: 3.1 -> 3", result2.toInt() == 3);
        
        // Test 3: Negative number
        Value result3 = MathFunctions::floor({makeDouble(-2.3)});
        assertTest("floor: -2.3 -> -3", result3.toInt() == -3);
        
        // Test 4: Integer
        Value result4 = MathFunctions::floor({makeDouble(5.0)});
        assertTest("floor: Integer input", result4.toInt() == 5);
        
        // Test 5: Zero
        Value result5 = MathFunctions::floor({makeDouble(0.0)});
        assertTest("floor: Zero", result5.toInt() == 0);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testFloor: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة ceil / (EN) Ceil Function Tests
// ============================================================================
void testCeil() {
    printSection("9. Testing ceil() Function");
    
    try {
        // Test 1: Positive decimal
        Value result1 = MathFunctions::ceil({makeDouble(3.1)});
        assertTest("ceil: 3.1 -> 4", result1.toInt() == 4);
        
        // Test 2: Positive large decimal
        Value result2 = MathFunctions::ceil({makeDouble(3.9)});
        assertTest("ceil: 3.9 -> 4", result2.toInt() == 4);
        
        // Test 3: Negative number
        Value result3 = MathFunctions::ceil({makeDouble(-2.7)});
        assertTest("ceil: -2.7 -> -2", result3.toInt() == -2);
        
        // Test 4: Integer
        Value result4 = MathFunctions::ceil({makeDouble(5.0)});
        assertTest("ceil: Integer input", result4.toInt() == 5);
        
        // Test 5: Zero
        Value result5 = MathFunctions::ceil({makeDouble(0.0)});
        assertTest("ceil: Zero", result5.toInt() == 0);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testCeil: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة sin / (EN) Sin Function Tests
// ============================================================================
void testSin() {
    printSection("10. Testing sin() Function");
    
    const double PI = 3.14159265358979323846;
    
    try {
        // Test 1: sin(0)
        Value result1 = MathFunctions::sin({makeDouble(0.0)});
        assertTest("sin: sin(0) = 0", areClose(result1.toDouble(), 0.0));
        
        // Test 2: sin(π/2) ≈ 1
        Value result2 = MathFunctions::sin({makeDouble(PI / 2.0)});
        assertTest("sin: sin(π/2) ≈ 1", areClose(result2.toDouble(), 1.0));
        
        // Test 3: sin(π) ≈ 0
        Value result3 = MathFunctions::sin({makeDouble(PI)});
        assertTest("sin: sin(π) ≈ 0", areClose(result3.toDouble(), 0.0));
        
        // Test 4: sin(π/6) ≈ 0.5
        Value result4 = MathFunctions::sin({makeDouble(PI / 6.0)});
        assertTest("sin: sin(π/6) ≈ 0.5", areClose(result4.toDouble(), 0.5));
        
        // Test 5: sin(-π/2) ≈ -1
        Value result5 = MathFunctions::sin({makeDouble(-PI / 2.0)});
        assertTest("sin: sin(-π/2) ≈ -1", areClose(result5.toDouble(), -1.0));
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testSin: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة cos / (EN) Cos Function Tests
// ============================================================================
void testCos() {
    printSection("11. Testing cos() Function");
    
    const double PI = 3.14159265358979323846;
    
    try {
        // Test 1: cos(0)
        Value result1 = MathFunctions::cos({makeDouble(0.0)});
        assertTest("cos: cos(0) = 1", areClose(result1.toDouble(), 1.0));
        
        // Test 2: cos(π/2) ≈ 0
        Value result2 = MathFunctions::cos({makeDouble(PI / 2.0)});
        assertTest("cos: cos(π/2) ≈ 0", areClose(result2.toDouble(), 0.0));
        
        // Test 3: cos(π) ≈ -1
        Value result3 = MathFunctions::cos({makeDouble(PI)});
        assertTest("cos: cos(π) ≈ -1", areClose(result3.toDouble(), -1.0));
        
        // Test 4: cos(π/3) ≈ 0.5
        Value result4 = MathFunctions::cos({makeDouble(PI / 3.0)});
        assertTest("cos: cos(π/3) ≈ 0.5", areClose(result4.toDouble(), 0.5));
        
        // Test 5: cos(-π) ≈ -1
        Value result5 = MathFunctions::cos({makeDouble(-PI)});
        assertTest("cos: cos(-π) ≈ -1", areClose(result5.toDouble(), -1.0));
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testCos: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات دالة tan / (EN) Tan Function Tests
// ============================================================================
void testTan() {
    printSection("12. Testing tan() Function");
    
    const double PI = 3.14159265358979323846;
    
    try {
        // Test 1: tan(0)
        Value result1 = MathFunctions::tan({makeDouble(0.0)});
        assertTest("tan: tan(0) = 0", areClose(result1.toDouble(), 0.0));
        
        // Test 2: tan(π/4) ≈ 1
        Value result2 = MathFunctions::tan({makeDouble(PI / 4.0)});
        assertTest("tan: tan(π/4) ≈ 1", areClose(result2.toDouble(), 1.0));
        
        // Test 3: tan(π) ≈ 0
        Value result3 = MathFunctions::tan({makeDouble(PI)});
        assertTest("tan: tan(π) ≈ 0", areClose(result3.toDouble(), 0.0));
        
        // Test 4: tan(-π/4) ≈ -1
        Value result4 = MathFunctions::tan({makeDouble(-PI / 4.0)});
        assertTest("tan: tan(-π/4) ≈ -1", areClose(result4.toDouble(), -1.0));
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in testTan: " << e.what() << "\n";
        failedTests++;
    }
}

// ============================================================================
// (AR) اختبارات التكامل / (EN) Integration Tests
// ============================================================================
void testIntegration() {
    printSection("13. Integration Tests");
    
    try {
        // Test 1: Pythagorean theorem: c = √(a² + b²)
        Value a_squared = MathFunctions::square({makeInt(3)});
        Value b_squared = MathFunctions::square({makeInt(4)});
        double sum = a_squared.toDouble() + b_squared.toDouble();
        Value c = MathFunctions::sqrt({makeDouble(sum)});
        assertTest("Integration: Pythagorean (3,4,5)", areClose(c.toDouble(), 5.0));
        
        // Test 2: abs(min(...))
        Value min_val = MathFunctions::min({makeInt(-5), makeInt(-10), makeInt(-3)});
        Value abs_min = MathFunctions::abs({makeInt(min_val.toInt())});
        assertTest("Integration: abs(min(...))", abs_min.toInt() == 10);
        
        // Test 3: round(sqrt(power(...)))
        Value powered = MathFunctions::power({makeDouble(2.0), makeDouble(4.0)});
        Value rooted = MathFunctions::sqrt({powered});
        Value rounded = MathFunctions::round({rooted});
        assertTest("Integration: round(sqrt(2^4))", rounded.toInt() == 4);
        
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
    std::cout << "║    Math Functions Comprehensive Test Suite                ║\n";
    std::cout << "║    اختبارات شاملة للدوال الرياضية                        ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    
    // Run all tests
    testSqrt();
    testPower();
    testAbs();
    testSquare();
    testMax();
    testMin();
    testRound();
    testFloor();
    testCeil();
    testSin();
    testCos();
    testTan();
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
