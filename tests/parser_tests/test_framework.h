/**
 * @file test_framework.h
 * @brief (AR) إطار اختبار بسيط للغة "ص" / (EN) Simple test framework for Sad Language
 * 
 * (AR) إطار اختبار خفيف الوزن مبني على C++ مع دعم:
 *      - مجموعات الاختبارات (Test Groups)
 *      - Setup/Teardown
 *      - Assertions متنوعة
 *      - إخراج ملون
 *      - إحصائيات تفصيلية
 * 
 * (EN) Lightweight C++ test framework with support for:
 *      - Test Groups
 *      - Setup/Teardown
 *      - Various Assertions
 *      - Colored Output
 *      - Detailed Statistics
 * 
 * @example
 * @code{.cpp}
 * TEST_GROUP(MyTests) {
 *     void setup() { /* Initialize  }
 *     void teardown() { /* Cleanup  }
 *     
 *     TEST(FirstTest) {
 *         ASSERT_TRUE(1 + 1 == 2);
 *     }
 * };
 * 
 * RUN_TEST_GROUP(MyTests);
 * @endcode
 * 
 * @author Sad Language Development Team
 * @date 2025-11-11
 * @license MIT License
 */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <cmath>

// ======================================================================
// (AR) الألوان / (EN) Colors
// ======================================================================

#ifdef _WIN32
    #include <windows.h>
    #define ENABLE_COLORS() \
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); \
        DWORD mode; \
        GetConsoleMode(hConsole, &mode); \
        SetConsoleMode(hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#else
    #define ENABLE_COLORS() /* No-op on Unix */
#endif

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"

// ======================================================================
// (AR) الإحصائيات / (EN) Statistics
// ======================================================================

struct TestStats {
    int totalTests = 0;
    int passedTests = 0;
    int failedTests = 0;
    std::vector<std::string> failedTestNames;
    
    void reset() {
        totalTests = 0;
        passedTests = 0;
        failedTests = 0;
        failedTestNames.clear();
    }
    
    void print() const {
        std::cout << "\n" << COLOR_BOLD << "========================================\n";
        std::cout << "Test Results / نتائج الاختبارات\n";
        std::cout << "========================================" << COLOR_RESET << "\n\n";
        
        std::cout << COLOR_CYAN << "Total Tests:   " << COLOR_RESET << totalTests << "\n";
        std::cout << COLOR_GREEN << "Passed:        " << COLOR_RESET << passedTests << "\n";
        std::cout << COLOR_RED << "Failed:        " << COLOR_RESET << failedTests << "\n";
        
        if (failedTests > 0) {
            std::cout << "\n" << COLOR_RED << COLOR_BOLD << "Failed Tests:" << COLOR_RESET << "\n";
            for (const auto& name : failedTestNames) {
                std::cout << "  ❌ " << name << "\n";
            }
        }
        
        std::cout << "\n" << COLOR_BOLD;
        if (failedTests == 0) {
            std::cout << COLOR_GREEN << "✅ All tests passed! / جميع الاختبارات نجحت!" << COLOR_RESET << "\n";
        } else {
            double percentage = (passedTests * 100.0) / totalTests;
            std::cout << COLOR_YELLOW << "⚠️  Success Rate: " << std::fixed 
                      << percentage << "%" << COLOR_RESET << "\n";
        }
        std::cout << "\n";
    }
};

static TestStats g_testStats;

// ======================================================================
// (AR) معلومات الاختبار / (EN) Test Info
// ======================================================================

struct TestInfo {
    std::string groupName;
    std::string testName;
    std::string fileName;
    int lineNumber;
    
    std::string fullName() const {
        return groupName + "." + testName;
    }
};

static TestInfo g_currentTest;

// ======================================================================
// (AR) Assertions / (EN) Assertions
// ======================================================================

#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << COLOR_RED << "❌ Assertion Failed: " << COLOR_RESET \
                      << "ASSERT_TRUE(" #condition ")\n" \
                      << "  File: " << __FILE__ << ":" << __LINE__ << "\n"; \
            throw std::runtime_error("Assertion failed"); \
        } \
    } while (0)

#define ASSERT_FALSE(condition) \
    do { \
        if (condition) { \
            std::cerr << COLOR_RED << "❌ Assertion Failed: " << COLOR_RESET \
                      << "ASSERT_FALSE(" #condition ")\n" \
                      << "  File: " << __FILE__ << ":" << __LINE__ << "\n"; \
            throw std::runtime_error("Assertion failed"); \
        } \
    } while (0)

#define ASSERT_EQ(actual, expected) \
    do { \
        auto _actual = (actual); \
        auto _expected = (expected); \
        if (_actual != _expected) { \
            std::cerr << COLOR_RED << "❌ Assertion Failed: " << COLOR_RESET \
                      << "ASSERT_EQ(" #actual ", " #expected ")\n" \
                      << "  Expected: " << _expected << "\n" \
                      << "  Actual:   " << _actual << "\n" \
                      << "  File: " << __FILE__ << ":" << __LINE__ << "\n"; \
            throw std::runtime_error("Assertion failed"); \
        } \
    } while (0)

#define ASSERT_NE(actual, expected) \
    do { \
        auto _actual = (actual); \
        auto _expected = (expected); \
        if (_actual == _expected) { \
            std::cerr << COLOR_RED << "❌ Assertion Failed: " << COLOR_RESET \
                      << "ASSERT_NE(" #actual ", " #expected ")\n" \
                      << "  Both values: " << _actual << "\n" \
                      << "  File: " << __FILE__ << ":" << __LINE__ << "\n"; \
            throw std::runtime_error("Assertion failed"); \
        } \
    } while (0)

#define ASSERT_GT(actual, expected) \
    do { \
        auto _actual = (actual); \
        auto _expected = (expected); \
        if (!(_actual > _expected)) { \
            std::cerr << COLOR_RED << "❌ Assertion Failed: " << COLOR_RESET \
                      << "ASSERT_GT(" #actual ", " #expected ")\n" \
                      << "  Expected: > " << _expected << "\n" \
                      << "  Actual:   " << _actual << "\n" \
                      << "  File: " << __FILE__ << ":" << __LINE__ << "\n"; \
            throw std::runtime_error("Assertion failed"); \
        } \
    } while (0)

#define ASSERT_LT(actual, expected) \
    do { \
        auto _actual = (actual); \
        auto _expected = (expected); \
        if (!(_actual < _expected)) { \
            std::cerr << COLOR_RED << "❌ Assertion Failed: " << COLOR_RESET \
                      << "ASSERT_LT(" #actual ", " #expected ")\n" \
                      << "  Expected: < " << _expected << "\n" \
                      << "  Actual:   " << _actual << "\n" \
                      << "  File: " << __FILE__ << ":" << __LINE__ << "\n"; \
            throw std::runtime_error("Assertion failed"); \
        } \
    } while (0)

#define ASSERT_GE(actual, expected) \
    do { \
        auto _actual = (actual); \
        auto _expected = (expected); \
        if (!(_actual >= _expected)) { \
            std::cerr << COLOR_RED << "❌ Assertion Failed: " << COLOR_RESET \
                      << "ASSERT_GE(" #actual ", " #expected ")\n" \
                      << "  Expected: >= " << _expected << "\n" \
                      << "  Actual:   " << _actual << "\n" \
                      << "  File: " << __FILE__ << ":" << __LINE__ << "\n"; \
            throw std::runtime_error("Assertion failed"); \
        } \
    } while (0)

#define ASSERT_LE(actual, expected) \
    do { \
        auto _actual = (actual); \
        auto _expected = (expected); \
        if (!(_actual <= _expected)) { \
            std::cerr << COLOR_RED << "❌ Assertion Failed: " << COLOR_RESET \
                      << "ASSERT_LE(" #actual ", " #expected ")\n" \
                      << "  Expected: <= " << _expected << "\n" \
                      << "  Actual:   " << _actual << "\n" \
                      << "  File: " << __FILE__ << ":" << __LINE__ << "\n"; \
            throw std::runtime_error("Assertion failed"); \
        } \
    } while (0)

#define ASSERT_NOT_NULL(pointer) \
    do { \
        if ((pointer) == nullptr) { \
            std::cerr << COLOR_RED << "❌ Assertion Failed: " << COLOR_RESET \
                      << "ASSERT_NOT_NULL(" #pointer ")\n" \
                      << "  Pointer is null\n" \
                      << "  File: " << __FILE__ << ":" << __LINE__ << "\n"; \
            throw std::runtime_error("Assertion failed"); \
        } \
    } while (0)

#define ASSERT_NULL(pointer) \
    do { \
        if ((pointer) != nullptr) { \
            std::cerr << COLOR_RED << "❌ Assertion Failed: " << COLOR_RESET \
                      << "ASSERT_NULL(" #pointer ")\n" \
                      << "  Pointer is not null\n" \
                      << "  File: " << __FILE__ << ":" << __LINE__ << "\n"; \
            throw std::runtime_error("Assertion failed"); \
        } \
    } while (0)

#define ASSERT_FLOAT_EQ(actual, expected, epsilon) \
    do { \
        auto _actual = (actual); \
        auto _expected = (expected); \
        auto _epsilon = (epsilon); \
        if (std::abs(_actual - _expected) > _epsilon) { \
            std::cerr << COLOR_RED << "❌ Assertion Failed: " << COLOR_RESET \
                      << "ASSERT_FLOAT_EQ(" #actual ", " #expected ", " #epsilon ")\n" \
                      << "  Expected: " << _expected << " (±" << _epsilon << ")\n" \
                      << "  Actual:   " << _actual << "\n" \
                      << "  Diff:     " << std::abs(_actual - _expected) << "\n" \
                      << "  File: " << __FILE__ << ":" << __LINE__ << "\n"; \
            throw std::runtime_error("Assertion failed"); \
        } \
    } while (0)

#define ASSERT_CONTAINS(str, substring) \
    do { \
        std::string _str = (str); \
        std::string _sub = (substring); \
        if (_str.find(_sub) == std::string::npos) { \
            std::cerr << COLOR_RED << "❌ Assertion Failed: " << COLOR_RESET \
                      << "ASSERT_CONTAINS(" #str ", " #substring ")\n" \
                      << "  String: \"" << _str << "\"\n" \
                      << "  Does not contain: \"" << _sub << "\"\n" \
                      << "  File: " << __FILE__ << ":" << __LINE__ << "\n"; \
            throw std::runtime_error("Assertion failed"); \
        } \
    } while (0)

#define ASSERT_THROWS(expression, exception_type) \
    do { \
        bool _threw = false; \
        try { \
            expression; \
        } catch (const exception_type&) { \
            _threw = true; \
        } catch (...) { \
            std::cerr << COLOR_RED << "❌ Assertion Failed: " << COLOR_RESET \
                      << "ASSERT_THROWS(" #expression ", " #exception_type ")\n" \
                      << "  Threw wrong exception type\n" \
                      << "  File: " << __FILE__ << ":" << __LINE__ << "\n"; \
            throw; \
        } \
        if (!_threw) { \
            std::cerr << COLOR_RED << "❌ Assertion Failed: " << COLOR_RESET \
                      << "ASSERT_THROWS(" #expression ", " #exception_type ")\n" \
                      << "  Did not throw exception\n" \
                      << "  File: " << __FILE__ << ":" << __LINE__ << "\n"; \
            throw std::runtime_error("Assertion failed"); \
        } \
    } while (0)

#define ASSERT_NO_THROW(expression) \
    do { \
        try { \
            expression; \
        } catch (const std::exception& e) { \
            std::cerr << COLOR_RED << "❌ Assertion Failed: " << COLOR_RESET \
                      << "ASSERT_NO_THROW(" #expression ")\n" \
                      << "  Threw exception: " << e.what() << "\n" \
                      << "  File: " << __FILE__ << ":" << __LINE__ << "\n"; \
            throw; \
        } catch (...) { \
            std::cerr << COLOR_RED << "❌ Assertion Failed: " << COLOR_RESET \
                      << "ASSERT_NO_THROW(" #expression ")\n" \
                      << "  Threw unknown exception\n" \
                      << "  File: " << __FILE__ << ":" << __LINE__ << "\n"; \
            throw; \
        } \
    } while (0)

// ======================================================================
// (AR) ماكرو الاختبار / (EN) Test Macros
// ======================================================================

#define TEST_GROUP(GroupName) \
    class TestGroup_##GroupName { \
    public: \
        static std::string getGroupName() { return #GroupName; } \
        virtual void setup() {} \
        virtual void teardown() {}

#define TEST(TestName) \
        void test_##TestName() { \
            g_currentTest.testName = #TestName; \
            g_currentTest.fileName = __FILE__; \
            g_currentTest.lineNumber = __LINE__; \
            std::cout << COLOR_CYAN << "  Running: " << COLOR_RESET \
                      << g_currentTest.fullName() << " ... " << std::flush;

#define END_TEST \
        }

#define END_TEST_GROUP \
    };

// ======================================================================
// (AR) تشغيل الاختبارات / (EN) Run Tests
// ======================================================================

#define RUN_TEST_GROUP(GroupName) \
    do { \
        std::cout << COLOR_BOLD << COLOR_BLUE \
                  << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n" \
                  << "Test Group: " << #GroupName << "\n" \
                  << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" \
                  << COLOR_RESET << "\n\n"; \
        TestGroup_##GroupName group; \
        g_currentTest.groupName = #GroupName; \
        runTestGroup(&group); \
    } while (0)

template<typename T>
void runTestGroup(T* group) {
    // Get all test methods via reflection (manually for now)
    // This is a simplified version - in real implementation,
    // we would use a registry pattern
    
    std::vector<std::function<void()>> tests;
    
    // Manual test registration would go here
    // For now, tests are called directly in main()
}

// ======================================================================
// (AR) دالة تشغيل اختبار واحد / (EN) Run Single Test
// ======================================================================

template<typename TestGroup>
void runTest(TestGroup* group, void (TestGroup::*testMethod)(), 
             const std::string& testName) {
    g_testStats.totalTests++;
    g_currentTest.testName = testName;
    
    std::cout << COLOR_CYAN << "  Running: " << COLOR_RESET 
              << g_currentTest.fullName() << " ... " << std::flush;
    
    try {
        group->setup();
        (group->*testMethod)();
        group->teardown();
        
        std::cout << COLOR_GREEN << "✓ PASSED" << COLOR_RESET << "\n";
        g_testStats.passedTests++;
    } catch (const std::exception& e) {
        std::cout << COLOR_RED << "✗ FAILED" << COLOR_RESET << "\n";
        std::cerr << "  Exception: " << e.what() << "\n";
        g_testStats.failedTests++;
        g_testStats.failedTestNames.push_back(g_currentTest.fullName());
        
        try {
            group->teardown();
        } catch (...) {
            std::cerr << COLOR_YELLOW << "  Warning: teardown() threw exception\n" 
                      << COLOR_RESET;
        }
    } catch (...) {
        std::cout << COLOR_RED << "✗ FAILED" << COLOR_RESET << "\n";
        std::cerr << "  Unknown exception\n";
        g_testStats.failedTests++;
        g_testStats.failedTestNames.push_back(g_currentTest.fullName());
        
        try {
            group->teardown();
        } catch (...) {
            std::cerr << COLOR_YELLOW << "  Warning: teardown() threw exception\n" 
                      << COLOR_RESET;
        }
    }
}

// ======================================================================
// (AR) الماكرو الرئيسي / (EN) Main Macro
// ======================================================================

#define RUN_ALL_TESTS() \
    do { \
        ENABLE_COLORS(); \
        std::cout << COLOR_BOLD << COLOR_MAGENTA \
                  << "\n╔════════════════════════════════════════╗\n" \
                  << "║  Sad Language Parser Tests             ║\n" \
                  << "║  اختبارات المحلل النحوي للغة \"ص\"       ║\n" \
                  << "╚════════════════════════════════════════╝\n" \
                  << COLOR_RESET << "\n"; \
        g_testStats.reset(); \
    } while (0)

#define PRINT_TEST_SUMMARY() \
    do { \
        g_testStats.print(); \
    } while (0)

// ======================================================================
// (AR) دوال مساعدة / (EN) Helper Functions
// ======================================================================

/**
 * @brief (AR) يطبع رأس مجموعة اختبارات.
 *        (EN) Prints test group header.
 */
inline void printTestGroupHeader(const std::string& groupName) {
    std::cout << COLOR_BOLD << COLOR_BLUE 
              << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
              << "Test Group: " << groupName << "\n"
              << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
              << COLOR_RESET << "\n\n";
}

/**
 * @brief (AR) ينشئ رسالة خطأ مفصلة.
 *        (EN) Creates detailed error message.
 */
inline std::string createErrorMessage(const std::string& expected, 
                                      const std::string& actual,
                                      const std::string& file, 
                                      int line) {
    std::stringstream ss;
    ss << "\n  Expected: " << expected << "\n"
       << "  Actual:   " << actual << "\n"
       << "  Location: " << file << ":" << line << "\n";
    return ss.str();
}

