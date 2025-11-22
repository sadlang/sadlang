/**
 * @file test_io_simple.cpp
 * @brief (AR) اختبارات بسيطة لدوال الإدخال والإخراج / (EN) Simple tests for I/O Functions
 * 
 * Phase 1 Unit Tests (Simple, no Catch2 dependency):
 * Basic testing of I/O functions with manual assertions
 * 
 * @author S Language Development Team
 * @date November 22, 2025
 * @version 1.0
 */

#include "stdlib/io/io_functions.h"
#include "data/types/value.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <string>
#include <vector>
#include <functional>

using namespace Sad;
using namespace Sad::Data;
using namespace Sad::StdLib::IO;

// Test statistics
int tests_run = 0;
int tests_passed = 0;
int tests_failed = 0;

// Helper macro for testing
#define ASSERT_EQUAL(actual, expected, msg) \
    do { \
        tests_run++; \
        if ((actual) == (expected)) { \
            tests_passed++; \
            std::cout << "  ✓ " << msg << std::endl; \
        } else { \
            tests_failed++; \
            std::cout << "  ✗ FAILED: " << msg << std::endl; \
        } \
    } while(0)

#define TEST_START(name) \
    std::cout << "\nTesting: " << name << std::endl;

// Helper function to capture output
std::string captureOutput(std::function<void()> func) {
    std::streambuf* old_cout = std::cout.rdbuf();
    std::ostringstream capture;
    std::cout.rdbuf(capture.rdbuf());
    
    try {
        func();
    } catch (...) {
        std::cout.rdbuf(old_cout);
        throw;
    }
    
    std::cout.rdbuf(old_cout);
    return capture.str();
}

int main() {
    std::cout << "========================================\n";
    std::cout << "(AR) اختبارات المكتبة القياسية - المرحلة 1\n";
    std::cout << "(EN) Standard Library Tests - Phase 1\n";
    std::cout << "========================================\n";
    
    // ====================================================================
    // Test: print() function
    // ====================================================================
    TEST_START("print() with integer");
    {
        std::vector<Value> args = {Value(42)};
        Value result = IOFunctions::print(args);
        ASSERT_EQUAL(result.isVoid(), true, "print() returns VOID");
    }
    
    TEST_START("print() with string");
    {
        std::vector<Value> args = {Value("Hello")};
        Value result = IOFunctions::print(args);
        ASSERT_EQUAL(result.isVoid(), true, "print() returns VOID for string");
    }
    
    TEST_START("print() with boolean true");
    {
        std::vector<Value> args = {Value(true)};
        Value result = IOFunctions::print(args);
        ASSERT_EQUAL(result.isVoid(), true, "print() returns VOID for boolean");
    }
    
    TEST_START("print() with multiple arguments");
    {
        std::vector<Value> args = {Value("Value:"), Value(100)};
        Value result = IOFunctions::print(args);
        ASSERT_EQUAL(result.isVoid(), true, "print() returns VOID for multiple args");
    }
    
    // ====================================================================
    // Test: println() function
    // ====================================================================
    TEST_START("println() with integer");
    {
        std::vector<Value> args = {Value(42)};
        Value result = IOFunctions::println(args);
        ASSERT_EQUAL(result.isVoid(), true, "println() returns VOID");
    }
    
    TEST_START("println() with multiple arguments");
    {
        std::vector<Value> args = {Value("Line:"), Value(1)};
        Value result = IOFunctions::println(args);
        ASSERT_EQUAL(result.isVoid(), true, "println() returns VOID for multiple args");
    }
    
    // ====================================================================
    // Test: input() function
    // ====================================================================
    TEST_START("input() returns a string Value");
    {
        // Note: Can't fully test without stdin mock, but verify return type
        std::vector<Value> args = {};
        // Skip actual input test due to stdin dependency
        std::cout << "  ⊘ Skipped (requires stdin mocking)" << std::endl;
    }
    
    // ====================================================================
    // Test: readLine() function
    // ====================================================================
    TEST_START("readLine() returns a string Value");
    {
        // Note: Can't fully test without stdin mock
        std::cout << "  ⊘ Skipped (requires stdin mocking)" << std::endl;
    }
    
    // ====================================================================
    // Test: clear() function
    // ====================================================================
    TEST_START("clear() function");
    {
        std::vector<Value> args = {};
        Value result = IOFunctions::clear(args);
        ASSERT_EQUAL(result.isVoid(), true, "clear() returns VOID");
    }
    
    TEST_START("clear() with arguments (should ignore)");
    {
        std::vector<Value> args = {Value(1), Value(2)};
        Value result = IOFunctions::clear(args);
        ASSERT_EQUAL(result.isVoid(), true, "clear() ignores arguments");
    }
    
    // ====================================================================
    // Test: validateArguments() function
    // ====================================================================
    TEST_START("validateArguments() with valid count");
    {
        std::vector<Value> args = {Value(1), Value(2)};
        bool valid = IOFunctions::validateArguments(args, 2, 3);
        ASSERT_EQUAL(valid, true, "validateArguments accepts valid count");
    }
    
    TEST_START("validateArguments() below minimum");
    {
        std::vector<Value> args = {Value(1)};
        bool threw = false;
        try {
            IOFunctions::validateArguments(args, 2, 3);
        } catch (...) {
            threw = true;
        }
        ASSERT_EQUAL(threw, true, "validateArguments throws when below minimum");
    }
    
    TEST_START("validateArguments() above maximum");
    {
        std::vector<Value> args = {Value(1), Value(2), Value(3), Value(4)};
        bool threw = false;
        try {
            IOFunctions::validateArguments(args, 1, 3);
        } catch (...) {
            threw = true;
        }
        ASSERT_EQUAL(threw, true, "validateArguments throws when above maximum");
    }
    
    TEST_START("validateArguments() unlimited maximum");
    {
        std::vector<Value> args = {Value(1), Value(2), Value(3), Value(4)};
        bool valid = IOFunctions::validateArguments(args, 1, -1);
        ASSERT_EQUAL(valid, true, "validateArguments accepts unlimited (-1) max");
    }
    
    // ====================================================================
    // Print Test Summary
    // ====================================================================
    std::cout << "\n========================================\n";
    std::cout << "(AR) ملخص النتائج / Test Summary\n";
    std::cout << "========================================\n";
    std::cout << "Tests Run:    " << tests_run << "\n";
    std::cout << "Tests Passed: " << tests_passed << " ✓\n";
    std::cout << "Tests Failed: " << tests_failed << (tests_failed > 0 ? " ✗" : "") << "\n";
    
    if (tests_failed == 0) {
        std::cout << "\n(AR) جميع الاختبارات نجحت!\n";
        std::cout << "(EN) All tests passed!\n";
        return 0;
    } else {
        std::cout << "\n(AR) بعض الاختبارات فشلت\n";
        std::cout << "(EN) Some tests failed\n";
        return 1;
    }
}
