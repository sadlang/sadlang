/**
 * @file sad_test_framework.h
 * @brief (AR) إطار اختبارات شامل للغة "ص" / (EN) Comprehensive test framework for Sad Language
 * 
 * (AR) إطار اختبار موحد لجميع اختبارات المشروع (600+ اختبار)
 *      يدعم: مجموعات، assertions، إحصائيات، XML output، ألوان
 * 
 * (EN) Unified test framework for all project tests (600+ tests)
 *      Supports: groups, assertions, statistics, XML output, colors
 * 
 * @author Sad Language Development Team
 * @date 2026-02-17
 */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <cmath>
#include <chrono>
#include <fstream>
#include <iomanip>

// ======================================================================
// Colors / الألوان
// ======================================================================
#ifdef _WIN32
    #include <windows.h>
    // (AR) إزالة الماكروهات من windows.h التي تتعارض مع كود لغة ص
    // (EN) Undefine Windows macros that conflict with Sad Language code
    #undef VOID
    #undef ERROR
    #undef FATAL
    #undef NEAR
    #undef FAR
    #undef DELETE
    #undef IN
    #undef OUT
    #undef OPTIONAL
    #undef interface
    #undef CONST
    #undef TRUE
    #undef FALSE
    #undef RGB
    #define SAD_ENABLE_COLORS() \
        { HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE); DWORD m; \
          GetConsoleMode(h, &m); SetConsoleMode(h, m | ENABLE_VIRTUAL_TERMINAL_PROCESSING); }
#else
    #define SAD_ENABLE_COLORS()
#endif

#define C_RESET   "\033[0m"
#define C_RED     "\033[31m"
#define C_GREEN   "\033[32m"
#define C_YELLOW  "\033[33m"
#define C_BLUE    "\033[34m"
#define C_MAGENTA "\033[35m"
#define C_CYAN    "\033[36m"
#define C_BOLD    "\033[1m"

namespace SadTest {

// ======================================================================
// إحصائيات الاختبار / Test Statistics
// ======================================================================
struct TestResult {
    std::string group;
    std::string name;
    bool passed;
    double durationMs;
    std::string errorMessage;
};

class TestRunner {
public:
    static TestRunner& instance() {
        static TestRunner inst;
        return inst;
    }

    void beginGroup(const std::string& name) {
        currentGroup_ = name;
        std::cout << C_BOLD << C_BLUE
                  << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
                  << "  " << name << "\n"
                  << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
                  << C_RESET << "\n";
    }

    void runTest(const std::string& name, std::function<void()> fn) {
        total_++;
        std::cout << C_CYAN << "  [" << total_ << "] " << C_RESET << name << " ... " << std::flush;
        
        auto start = std::chrono::high_resolution_clock::now();
        TestResult result;
        result.group = currentGroup_;
        result.name = name;
        
        try {
            fn();
            auto end = std::chrono::high_resolution_clock::now();
            result.durationMs = std::chrono::duration<double, std::milli>(end - start).count();
            result.passed = true;
            passed_++;
            std::cout << C_GREEN << "PASSED" << C_RESET 
                      << " (" << std::fixed << std::setprecision(1) << result.durationMs << "ms)\n";
        } catch (const std::exception& e) {
            auto end = std::chrono::high_resolution_clock::now();
            result.durationMs = std::chrono::duration<double, std::milli>(end - start).count();
            result.passed = false;
            result.errorMessage = e.what();
            failed_++;
            failedNames_.push_back(currentGroup_ + "." + name);
            std::cout << C_RED << "FAILED" << C_RESET << "\n";
            std::cerr << C_RED << "    " << e.what() << C_RESET << "\n";
        } catch (...) {
            auto end = std::chrono::high_resolution_clock::now();
            result.durationMs = std::chrono::duration<double, std::milli>(end - start).count();
            result.passed = false;
            result.errorMessage = "Unknown exception";
            failed_++;
            failedNames_.push_back(currentGroup_ + "." + name);
            std::cout << C_RED << "FAILED (unknown exception)" << C_RESET << "\n";
        }
        results_.push_back(result);
    }

    int printSummary() {
        double totalTime = 0;
        for (auto& r : results_) totalTime += r.durationMs;

        std::cout << "\n" << C_BOLD
                  << "╔════════════════════════════════════════════════════╗\n"
                  << "║  نتائج الاختبارات / Test Results                  ║\n"
                  << "╚════════════════════════════════════════════════════╝\n"
                  << C_RESET;
        
        std::cout << C_CYAN  << "  المجموع / Total:   " << C_RESET << total_ << "\n";
        std::cout << C_GREEN << "  نجح / Passed:      " << C_RESET << passed_ << "\n";
        std::cout << C_RED   << "  فشل / Failed:      " << C_RESET << failed_ << "\n";
        std::cout << C_YELLOW << "  تخطي / Skipped:    " << C_RESET << skipped_ << "\n";
        std::cout << "  الوقت / Time:      " << std::fixed << std::setprecision(1) << totalTime << "ms\n";

        if (failed_ > 0) {
            std::cout << "\n" << C_RED << C_BOLD << "  الاختبارات الفاشلة / Failed Tests:" << C_RESET << "\n";
            for (auto& n : failedNames_) {
                std::cout << C_RED << "    ✗ " << n << C_RESET << "\n";
            }
        }

        std::cout << "\n" << C_BOLD;
        if (failed_ == 0) {
            std::cout << C_GREEN << "  ✅ جميع الاختبارات نجحت! / All " << total_ << " tests passed!" << C_RESET << "\n";
        } else {
            double pct = (passed_ * 100.0) / total_;
            std::cout << C_RED << "  ❌ نسبة النجاح / Pass Rate: " 
                      << std::fixed << std::setprecision(1) << pct << "%" << C_RESET << "\n";
        }
        std::cout << "\n";

        // Write JUnit XML (for CI)
        writeJUnitXML();

        return failed_ > 0 ? 1 : 0;
    }

    void skip(const std::string& name, const std::string& reason = "") {
        skipped_++;
        total_++;
        std::cout << C_YELLOW << "  [" << total_ << "] " << name << " ... SKIPPED";
        if (!reason.empty()) std::cout << " (" << reason << ")";
        std::cout << C_RESET << "\n";
        
        TestResult r;
        r.group = currentGroup_;
        r.name = name;
        r.passed = true;
        r.durationMs = 0;
        results_.push_back(r);
    }

    int getTotal() const { return total_; }
    int getPassed() const { return passed_; }
    int getFailed() const { return failed_; }

private:
    TestRunner() = default;

    void writeJUnitXML() {
        std::ofstream f("test-results.xml");
        if (!f) return;
        
        f << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        f << "<testsuites tests=\"" << total_ << "\" failures=\"" << failed_ 
          << "\" skipped=\"" << skipped_ << "\">\n";
        
        std::string lastGroup;
        for (auto& r : results_) {
            if (r.group != lastGroup) {
                if (!lastGroup.empty()) f << "  </testsuite>\n";
                f << "  <testsuite name=\"" << r.group << "\">\n";
                lastGroup = r.group;
            }
            f << "    <testcase name=\"" << r.name << "\" time=\"" 
              << (r.durationMs / 1000.0) << "\"";
            if (!r.passed) {
                f << ">\n      <failure message=\"" << r.errorMessage << "\"/>\n    </testcase>\n";
            } else {
                f << "/>\n";
            }
        }
        if (!lastGroup.empty()) f << "  </testsuite>\n";
        f << "</testsuites>\n";
    }

    std::string currentGroup_;
    int total_ = 0;
    int passed_ = 0;
    int failed_ = 0;
    int skipped_ = 0;
    std::vector<std::string> failedNames_;
    std::vector<TestResult> results_;
};

// ======================================================================
// Assertion Macros / ماكروات التأكيد
// ======================================================================

#define SAD_ASSERT_TRUE(cond) \
    do { if (!(cond)) { \
        std::ostringstream _ss; \
        _ss << "ASSERT_TRUE failed: " #cond " at " << __FILE__ << ":" << __LINE__; \
        throw std::runtime_error(_ss.str()); \
    }} while(0)

#define SAD_ASSERT_FALSE(cond) \
    do { if (cond) { \
        std::ostringstream _ss; \
        _ss << "ASSERT_FALSE failed: " #cond " at " << __FILE__ << ":" << __LINE__; \
        throw std::runtime_error(_ss.str()); \
    }} while(0)

#define SAD_ASSERT_EQ(a, b) \
    do { auto _a = (a); auto _b = (b); if (_a != _b) { \
        std::ostringstream _ss; \
        _ss << "ASSERT_EQ failed: " #a " == " #b "\n  Expected: " << _b << "\n  Actual:   " << _a \
            << "\n  at " << __FILE__ << ":" << __LINE__; \
        throw std::runtime_error(_ss.str()); \
    }} while(0)

#define SAD_ASSERT_NE(a, b) \
    do { auto _a = (a); auto _b = (b); if (_a == _b) { \
        std::ostringstream _ss; \
        _ss << "ASSERT_NE failed: " #a " != " #b " (both: " << _a << ")" \
            << "\n  at " << __FILE__ << ":" << __LINE__; \
        throw std::runtime_error(_ss.str()); \
    }} while(0)

#define SAD_ASSERT_GT(a, b) \
    do { auto _a = (a); auto _b = (b); if (!(_a > _b)) { \
        std::ostringstream _ss; \
        _ss << "ASSERT_GT failed: " #a " > " #b "\n  Left: " << _a << ", Right: " << _b \
            << "\n  at " << __FILE__ << ":" << __LINE__; \
        throw std::runtime_error(_ss.str()); \
    }} while(0)

#define SAD_ASSERT_LT(a, b) \
    do { auto _a = (a); auto _b = (b); if (!(_a < _b)) { \
        std::ostringstream _ss; \
        _ss << "ASSERT_LT failed: " #a " < " #b "\n  Left: " << _a << ", Right: " << _b \
            << "\n  at " << __FILE__ << ":" << __LINE__; \
        throw std::runtime_error(_ss.str()); \
    }} while(0)

#define SAD_ASSERT_GE(a, b) \
    do { auto _a = (a); auto _b = (b); if (!(_a >= _b)) { \
        std::ostringstream _ss; \
        _ss << "ASSERT_GE failed: " #a " >= " #b "\n  Left: " << _a << ", Right: " << _b \
            << "\n  at " << __FILE__ << ":" << __LINE__; \
        throw std::runtime_error(_ss.str()); \
    }} while(0)

#define SAD_ASSERT_LE(a, b) \
    do { auto _a = (a); auto _b = (b); if (!(_a <= _b)) { \
        std::ostringstream _ss; \
        _ss << "ASSERT_LE failed: " #a " <= " #b "\n  Left: " << _a << ", Right: " << _b \
            << "\n  at " << __FILE__ << ":" << __LINE__; \
        throw std::runtime_error(_ss.str()); \
    }} while(0)

#define SAD_ASSERT_CONTAINS(haystack, needle) \
    do { std::string _s = (haystack); std::string _sub = (needle); \
    if (_s.find(_sub) == std::string::npos) { \
        std::ostringstream _ss; \
        _ss << "ASSERT_CONTAINS failed: \"" << _s << "\" does not contain \"" << _sub << "\"" \
            << "\n  at " << __FILE__ << ":" << __LINE__; \
        throw std::runtime_error(_ss.str()); \
    }} while(0)

#define SAD_ASSERT_NOT_NULL(ptr) \
    do { if ((ptr) == nullptr) { \
        std::ostringstream _ss; \
        _ss << "ASSERT_NOT_NULL failed: " #ptr " is null at " << __FILE__ << ":" << __LINE__; \
        throw std::runtime_error(_ss.str()); \
    }} while(0)

#define SAD_ASSERT_THROWS(expr, exc_type) \
    do { bool _threw = false; try { expr; } catch (const exc_type&) { _threw = true; } \
    if (!_threw) { \
        std::ostringstream _ss; \
        _ss << "ASSERT_THROWS failed: " #expr " did not throw " #exc_type \
            << " at " << __FILE__ << ":" << __LINE__; \
        throw std::runtime_error(_ss.str()); \
    }} while(0)

#define SAD_ASSERT_NO_THROW(expr) \
    do { try { expr; } catch (const std::exception& e) { \
        std::ostringstream _ss; \
        _ss << "ASSERT_NO_THROW failed: " #expr " threw: " << e.what() \
            << " at " << __FILE__ << ":" << __LINE__; \
        throw std::runtime_error(_ss.str()); \
    }} while(0)

#define SAD_ASSERT_FLOAT_EQ(a, b, eps) \
    do { auto _a = (a); auto _b = (b); if (std::abs(_a - _b) > (eps)) { \
        std::ostringstream _ss; \
        _ss << "ASSERT_FLOAT_EQ failed: " #a " ≈ " #b \
            << "\n  Expected: " << _b << "\n  Actual: " << _a \
            << "\n  Diff: " << std::abs(_a - _b) << " > " << (eps) \
            << "\n  at " << __FILE__ << ":" << __LINE__; \
        throw std::runtime_error(_ss.str()); \
    }} while(0)

// ======================================================================
// Convenience Macros / ماكروات مساعدة
// ======================================================================

#define SAD_TEST_INIT() \
    SAD_ENABLE_COLORS(); \
    auto& _runner = SadTest::TestRunner::instance();

#define SAD_GROUP(name) _runner.beginGroup(name);

#define SAD_TEST(name, ...) \
    _runner.runTest(name, [&]() { __VA_ARGS__ });

#define SAD_SKIP(name, reason) _runner.skip(name, reason);

#define SAD_SUMMARY() return _runner.printSummary();

} // namespace SadTest
