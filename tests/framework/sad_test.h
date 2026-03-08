/**
 * @file sad_test.h
 * @brief (AR) إطار اختبارات موحد للغة "ص" — الملف الرئيسي
 * @brief (EN) Unified testing framework for Sad Language — main header
 * 
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║  إطار اختبارات لغة ص الموحد / Sad Test Framework                          ║
 * ║                                                                           ║
 * ║  يدعم:                                                                    ║
 * ║  • xUnit style — للاختبارات الوحدوية (TEST, ASSERT_*)                    ║
 * ║  • BDD style — لاختبارات التكامل (describe, it, expect)                  ║
 * ║  • إحصائيات مفصلة + XML/JSON output                                      ║
 * ║  • دعم UTF-8 وأسماء اختبارات عربية                                       ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 * 
 * @example
 * ```cpp
 * #include "sad_test.h"
 * 
 * // xUnit Style:
 * TEST(Lexer, يحلل_الكلمات_المفتاحية) {
 *     auto tokens = lex("دالة");
 *     ASSERT_EQ(tokens[0].type, KEYWORD_FUNCTION);
 * }
 * 
 * // BDD Style:
 * describe("المحلل النحوي", []() {
 *     it("يحلل دالة بسيطة", []() {
 *         auto ast = parse("دالة جمع() نهاية");
 *         expect(ast).to_not_be_null();
 *         expect(ast->type).to_equal(NodeType::Function);
 *     });
 * });
 * ```
 * 
 * @author عمر — مهندس البنية التحتية / Omar — Infrastructure Engineer
 * @date 2026-03-06
 */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <chrono>
#include <memory>
#include <map>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <stdexcept>
#include <type_traits>

// ══════════════════════════════════════════════════════════════════════════════
// إعدادات المنصة / Platform Configuration
// ══════════════════════════════════════════════════════════════════════════════
#ifdef _WIN32
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
    // (AR) إزالة تعريفات Windows المتعارضة مع لغة ص
    #undef VOID
    #undef ERROR
    #undef TRUE
    #undef FALSE
    #undef DELETE
    #undef IN
    #undef OUT
    #undef CONST
    #undef interface
    #undef RGB
    #undef NEAR
    #undef FAR
    #undef OPTIONAL
    #undef FATAL
    
    inline void sad_enable_colors() {
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode;
        GetConsoleMode(h, &mode);
        SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        SetConsoleOutputCP(65001); // UTF-8
    }
#else
    inline void sad_enable_colors() {}
#endif

// ══════════════════════════════════════════════════════════════════════════════
// ANSI Colors / الألوان
// ══════════════════════════════════════════════════════════════════════════════
namespace SadTest {
namespace Color {
    constexpr const char* RESET   = "\033[0m";
    constexpr const char* RED     = "\033[31m";
    constexpr const char* GREEN   = "\033[32m";
    constexpr const char* YELLOW  = "\033[33m";
    constexpr const char* BLUE    = "\033[34m";
    constexpr const char* MAGENTA = "\033[35m";
    constexpr const char* CYAN    = "\033[36m";
    constexpr const char* BOLD    = "\033[1m";
    constexpr const char* DIM     = "\033[2m";
}

// ══════════════════════════════════════════════════════════════════════════════
// نتيجة اختبار فردي / Individual Test Result
// ══════════════════════════════════════════════════════════════════════════════
enum class TestStatus {
    PASSED,
    FAILED,
    SKIPPED,
    PENDING
};

struct TestResult {
    std::string suite;      // مجموعة الاختبار
    std::string name;       // اسم الاختبار
    TestStatus status;
    double durationMs;
    std::string errorMessage;
    std::string file;
    int line;
};

// ══════════════════════════════════════════════════════════════════════════════
// استثناء فشل الاختبار / Test Assertion Failure Exception
// ══════════════════════════════════════════════════════════════════════════════
class AssertionError : public std::runtime_error {
public:
    AssertionError(const std::string& msg, const char* file, int line)
        : std::runtime_error(msg), file_(file), line_(line) {}
    
    const char* file() const { return file_; }
    int line() const { return line_; }

private:
    const char* file_;
    int line_;
};

// ══════════════════════════════════════════════════════════════════════════════
// محرك الاختبارات الرئيسي (Singleton) / Main Test Runner
// ══════════════════════════════════════════════════════════════════════════════
class TestRunner {
public:
    static TestRunner& instance() {
        static TestRunner inst;
        return inst;
    }

    // ──────────────────────────────────────────────────────────────────────────
    // تسجيل الاختبارات / Test Registration
    // ──────────────────────────────────────────────────────────────────────────
    void registerTest(const std::string& suite, const std::string& name,
                      std::function<void()> fn, const char* file, int line) {
        tests_.push_back({suite, name, fn, file, line, false});
    }

    void registerSkippedTest(const std::string& suite, const std::string& name,
                             const char* file, int line) {
        tests_.push_back({suite, name, nullptr, file, line, true});
    }

    // ──────────────────────────────────────────────────────────────────────────
    // BDD describe/it
    // ──────────────────────────────────────────────────────────────────────────
    void describe(const std::string& description, std::function<void()> block) {
        currentSuite_ = description;
        block();
        currentSuite_.clear();
    }

    void it(const std::string& description, std::function<void()> fn,
            const char* file, int line) {
        registerTest(currentSuite_, description, fn, file, line);
    }

    void xit(const std::string& description, const char* file, int line) {
        registerSkippedTest(currentSuite_, description, file, line);
    }

    // ──────────────────────────────────────────────────────────────────────────
    // تشغيل الاختبارات / Run Tests
    // ──────────────────────────────────────────────────────────────────────────
    int run(int argc = 0, char** argv = nullptr) {
        sad_enable_colors();
        parseArgs(argc, argv);

        auto startAll = std::chrono::high_resolution_clock::now();

        std::string lastSuite;
        for (auto& test : tests_) {
            // فلترة بالاسم إذا تم تحديده
            if (!filter_.empty() && test.name.find(filter_) == std::string::npos &&
                test.suite.find(filter_) == std::string::npos) {
                continue;
            }

            // طباعة رأس المجموعة الجديدة
            if (test.suite != lastSuite) {
                lastSuite = test.suite;
                std::cout << Color::BOLD << Color::BLUE
                          << "\n┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n"
                          << "┃ " << test.suite << "\n"
                          << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛"
                          << Color::RESET << "\n";
            }

            runSingleTest(test);
        }

        auto endAll = std::chrono::high_resolution_clock::now();
        totalTimeMs_ = std::chrono::duration<double, std::milli>(endAll - startAll).count();

        return printSummary();
    }

    // ──────────────────────────────────────────────────────────────────────────
    // تصدير النتائج / Export Results
    // ──────────────────────────────────────────────────────────────────────────
    void exportXML(const std::string& path) {
        std::ofstream out(path);
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        out << "<testsuites tests=\"" << total_ << "\" failures=\"" << failed_ 
            << "\" time=\"" << totalTimeMs_ / 1000.0 << "\">\n";
        
        std::string lastSuite;
        for (auto& r : results_) {
            if (r.suite != lastSuite) {
                if (!lastSuite.empty()) out << "  </testsuite>\n";
                lastSuite = r.suite;
                out << "  <testsuite name=\"" << escapeXML(r.suite) << "\">\n";
            }
            out << "    <testcase name=\"" << escapeXML(r.name) << "\" "
                << "time=\"" << r.durationMs / 1000.0 << "\"";
            if (r.status == TestStatus::FAILED) {
                out << ">\n      <failure message=\"" << escapeXML(r.errorMessage) 
                    << "\" />\n    </testcase>\n";
            } else if (r.status == TestStatus::SKIPPED) {
                out << ">\n      <skipped />\n    </testcase>\n";
            } else {
                out << " />\n";
            }
        }
        if (!lastSuite.empty()) out << "  </testsuite>\n";
        out << "</testsuites>\n";
    }

    void exportJSON(const std::string& path) {
        std::ofstream out(path);
        out << "{\n  \"total\": " << total_ << ",\n  \"passed\": " << passed_
            << ",\n  \"failed\": " << failed_ << ",\n  \"skipped\": " << skipped_
            << ",\n  \"timeMs\": " << totalTimeMs_ << ",\n  \"tests\": [\n";
        
        for (size_t i = 0; i < results_.size(); ++i) {
            auto& r = results_[i];
            out << "    {\"suite\": \"" << escapeJSON(r.suite) << "\", "
                << "\"name\": \"" << escapeJSON(r.name) << "\", "
                << "\"status\": \"" << statusToString(r.status) << "\", "
                << "\"timeMs\": " << r.durationMs << "}";
            if (i < results_.size() - 1) out << ",";
            out << "\n";
        }
        out << "  ]\n}\n";
    }

private:
    TestRunner() = default;

    struct TestEntry {
        std::string suite;
        std::string name;
        std::function<void()> fn;
        const char* file;
        int line;
        bool skip;
    };

    std::vector<TestEntry> tests_;
    std::vector<TestResult> results_;
    std::string currentSuite_;
    std::string filter_;
    bool verbose_ = false;

    int total_ = 0, passed_ = 0, failed_ = 0, skipped_ = 0;
    double totalTimeMs_ = 0;

    void parseArgs(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "-v" || arg == "--verbose") verbose_ = true;
            else if (arg.rfind("--filter=", 0) == 0) filter_ = arg.substr(9);
            else if (arg.rfind("-f=", 0) == 0) filter_ = arg.substr(3);
        }
    }

    void runSingleTest(TestEntry& test) {
        total_++;
        std::cout << Color::CYAN << "  [" << total_ << "] " << Color::RESET 
                  << test.name << " ... " << std::flush;

        TestResult result;
        result.suite = test.suite;
        result.name = test.name;
        result.file = test.file ? test.file : "";
        result.line = test.line;

        if (test.skip) {
            result.status = TestStatus::SKIPPED;
            result.durationMs = 0;
            skipped_++;
            std::cout << Color::YELLOW << "SKIPPED" << Color::RESET << "\n";
        } else {
            auto start = std::chrono::high_resolution_clock::now();
            try {
                test.fn();
                auto end = std::chrono::high_resolution_clock::now();
                result.durationMs = std::chrono::duration<double, std::milli>(end - start).count();
                result.status = TestStatus::PASSED;
                passed_++;
                std::cout << Color::GREEN << "PASSED" << Color::RESET
                          << Color::DIM << " (" << std::fixed << std::setprecision(1) 
                          << result.durationMs << "ms)" << Color::RESET << "\n";
            } catch (const AssertionError& e) {
                auto end = std::chrono::high_resolution_clock::now();
                result.durationMs = std::chrono::duration<double, std::milli>(end - start).count();
                result.status = TestStatus::FAILED;
                result.errorMessage = e.what();
                failed_++;
                std::cout << Color::RED << "FAILED" << Color::RESET << "\n";
                std::cerr << Color::RED << "    ✗ " << e.what() << Color::RESET << "\n";
                std::cerr << Color::DIM << "      at " << e.file() << ":" << e.line() 
                          << Color::RESET << "\n";
            } catch (const std::exception& e) {
                auto end = std::chrono::high_resolution_clock::now();
                result.durationMs = std::chrono::duration<double, std::milli>(end - start).count();
                result.status = TestStatus::FAILED;
                result.errorMessage = std::string("Exception: ") + e.what();
                failed_++;
                std::cout << Color::RED << "FAILED (exception)" << Color::RESET << "\n";
                std::cerr << Color::RED << "    ✗ " << e.what() << Color::RESET << "\n";
            } catch (...) {
                auto end = std::chrono::high_resolution_clock::now();
                result.durationMs = std::chrono::duration<double, std::milli>(end - start).count();
                result.status = TestStatus::FAILED;
                result.errorMessage = "Unknown exception";
                failed_++;
                std::cout << Color::RED << "FAILED (unknown)" << Color::RESET << "\n";
            }
        }
        results_.push_back(result);
    }

    int printSummary() {
        std::cout << "\n" << Color::BOLD
                  << "╔════════════════════════════════════════════════════════════╗\n"
                  << "║  نتائج الاختبارات / Test Results                          ║\n"
                  << "╚════════════════════════════════════════════════════════════╝\n"
                  << Color::RESET;

        std::cout << Color::CYAN   << "  المجموع / Total:   " << Color::RESET << total_ << "\n";
        std::cout << Color::GREEN  << "  نجح / Passed:      " << Color::RESET << passed_ << "\n";
        std::cout << Color::RED    << "  فشل / Failed:      " << Color::RESET << failed_ << "\n";
        std::cout << Color::YELLOW << "  تخطي / Skipped:    " << Color::RESET << skipped_ << "\n";
        std::cout << "  الوقت / Time:      " << std::fixed << std::setprecision(1) 
                  << totalTimeMs_ << "ms\n";

        double coverage = total_ > 0 ? (passed_ * 100.0 / total_) : 0;
        std::cout << "  نسبة النجاح:       " << std::setprecision(1) << coverage << "%\n";

        if (failed_ > 0) {
            std::cout << "\n" << Color::RED << Color::BOLD 
                      << "  الاختبارات الفاشلة / Failed Tests:" << Color::RESET << "\n";
            for (auto& r : results_) {
                if (r.status == TestStatus::FAILED) {
                    std::cout << Color::RED << "    ✗ " << r.suite << " :: " << r.name 
                              << Color::RESET << "\n";
                }
            }
            std::cout << "\n";
            return 1;
        }

        std::cout << "\n" << Color::GREEN << Color::BOLD 
                  << "  ✓ جميع الاختبارات نجحت! / All tests passed!" 
                  << Color::RESET << "\n\n";
        return 0;
    }

    static std::string escapeXML(const std::string& s) {
        std::string out;
        for (char c : s) {
            switch (c) {
                case '&':  out += "&amp;";  break;
                case '<':  out += "&lt;";   break;
                case '>':  out += "&gt;";   break;
                case '"':  out += "&quot;"; break;
                case '\'': out += "&apos;"; break;
                default:   out += c;
            }
        }
        return out;
    }

    static std::string escapeJSON(const std::string& s) {
        std::string out;
        for (char c : s) {
            switch (c) {
                case '"':  out += "\\\""; break;
                case '\\': out += "\\\\"; break;
                case '\n': out += "\\n";  break;
                case '\r': out += "\\r";  break;
                case '\t': out += "\\t";  break;
                default:   out += c;
            }
        }
        return out;
    }

    static std::string statusToString(TestStatus s) {
        switch (s) {
            case TestStatus::PASSED:  return "passed";
            case TestStatus::FAILED:  return "failed";
            case TestStatus::SKIPPED: return "skipped";
            case TestStatus::PENDING: return "pending";
        }
        return "unknown";
    }
};

// ══════════════════════════════════════════════════════════════════════════════
// BDD Expect API
// ══════════════════════════════════════════════════════════════════════════════
template<typename T>
class Expectation {
public:
    Expectation(const T& actual, const char* file, int line)
        : actual_(actual), file_(file), line_(line), negated_(false) {}

    Expectation& to_not() {
        negated_ = true;
        return *this;
    }

    void to_equal(const T& expected) {
        bool eq = (actual_ == expected);
        if (negated_) eq = !eq;
        if (!eq) {
            std::ostringstream ss;
            ss << "Expected " << (negated_ ? "NOT " : "") << expected << " but got " << actual_;
            throw AssertionError(ss.str(), file_, line_);
        }
    }

    void to_be_true() {
        bool result = static_cast<bool>(actual_);
        if (negated_) result = !result;
        if (!result) {
            throw AssertionError(negated_ ? "Expected false but got true" : "Expected true but got false",
                                file_, line_);
        }
    }

    void to_be_false() {
        negated_ = !negated_;
        to_be_true();
    }

    void to_be_null() {
        bool isNull = (actual_ == nullptr);
        if (negated_) isNull = !isNull;
        if (!isNull) {
            throw AssertionError(negated_ ? "Expected NOT null" : "Expected null", file_, line_);
        }
    }

    void to_not_be_null() {
        negated_ = true;
        to_be_null();
    }

    template<typename U>
    void to_be_greater_than(const U& val) {
        bool gt = (actual_ > val);
        if (negated_) gt = !gt;
        if (!gt) {
            std::ostringstream ss;
            ss << "Expected " << actual_ << (negated_ ? " NOT" : "") << " > " << val;
            throw AssertionError(ss.str(), file_, line_);
        }
    }

    template<typename U>
    void to_be_less_than(const U& val) {
        bool lt = (actual_ < val);
        if (negated_) lt = !lt;
        if (!lt) {
            std::ostringstream ss;
            ss << "Expected " << actual_ << (negated_ ? " NOT" : "") << " < " << val;
            throw AssertionError(ss.str(), file_, line_);
        }
    }

private:
    T actual_;
    const char* file_;
    int line_;
    bool negated_;
};

// تخصص للنصوص
template<>
class Expectation<std::string> {
public:
    Expectation(const std::string& actual, const char* file, int line)
        : actual_(actual), file_(file), line_(line), negated_(false) {}

    Expectation& to_not() { negated_ = true; return *this; }

    void to_equal(const std::string& expected) {
        bool eq = (actual_ == expected);
        if (negated_) eq = !eq;
        if (!eq) {
            std::ostringstream ss;
            ss << "Expected " << (negated_ ? "NOT " : "") << "\"" << expected 
               << "\" but got \"" << actual_ << "\"";
            throw AssertionError(ss.str(), file_, line_);
        }
    }

    void to_contain(const std::string& substr) {
        bool contains = (actual_.find(substr) != std::string::npos);
        if (negated_) contains = !contains;
        if (!contains) {
            std::ostringstream ss;
            ss << "Expected \"" << actual_ << "\" to " << (negated_ ? "NOT " : "") 
               << "contain \"" << substr << "\"";
            throw AssertionError(ss.str(), file_, line_);
        }
    }

    void to_be_empty() {
        bool empty = actual_.empty();
        if (negated_) empty = !empty;
        if (!empty) {
            throw AssertionError(negated_ ? "Expected NOT empty" : "Expected empty", file_, line_);
        }
    }

    void to_not_be_empty() { negated_ = true; to_be_empty(); }

private:
    std::string actual_;
    const char* file_;
    int line_;
    bool negated_;
};

// ══════════════════════════════════════════════════════════════════════════════
// Macros — xUnit Style
// ══════════════════════════════════════════════════════════════════════════════

// تسجيل اختبار تلقائي
#define SAD_TEST_CONCAT_(a, b) a##b
#define SAD_TEST_CONCAT(a, b) SAD_TEST_CONCAT_(a, b)

#define TEST(Suite, Name) \
    static void SAD_TEST_CONCAT(test_##Suite##_##Name, _fn)(); \
    static struct SAD_TEST_CONCAT(test_##Suite##_##Name, _reg) { \
        SAD_TEST_CONCAT(test_##Suite##_##Name, _reg)() { \
            SadTest::TestRunner::instance().registerTest( \
                #Suite, #Name, SAD_TEST_CONCAT(test_##Suite##_##Name, _fn), \
                __FILE__, __LINE__); \
        } \
    } SAD_TEST_CONCAT(test_##Suite##_##Name, _inst); \
    static void SAD_TEST_CONCAT(test_##Suite##_##Name, _fn)()

#define SKIP_TEST(Suite, Name) \
    static struct SAD_TEST_CONCAT(skip_##Suite##_##Name, _reg) { \
        SAD_TEST_CONCAT(skip_##Suite##_##Name, _reg)() { \
            SadTest::TestRunner::instance().registerSkippedTest( \
                #Suite, #Name, __FILE__, __LINE__); \
        } \
    } SAD_TEST_CONCAT(skip_##Suite##_##Name, _inst);

// ══════════════════════════════════════════════════════════════════════════════
// Macros — Assertions
// ══════════════════════════════════════════════════════════════════════════════

#define ASSERT_TRUE(cond) \
    do { if (!(cond)) throw SadTest::AssertionError("ASSERT_TRUE failed: " #cond, __FILE__, __LINE__); } while(0)

#define ASSERT_FALSE(cond) \
    do { if (cond) throw SadTest::AssertionError("ASSERT_FALSE failed: " #cond, __FILE__, __LINE__); } while(0)

#define ASSERT_EQ(actual, expected) \
    do { if ((actual) != (expected)) { \
        std::ostringstream ss; \
        ss << "ASSERT_EQ failed: " << (actual) << " != " << (expected); \
        throw SadTest::AssertionError(ss.str(), __FILE__, __LINE__); \
    }} while(0)

#define ASSERT_NE(actual, expected) \
    do { if ((actual) == (expected)) { \
        std::ostringstream ss; \
        ss << "ASSERT_NE failed: " << (actual) << " == " << (expected); \
        throw SadTest::AssertionError(ss.str(), __FILE__, __LINE__); \
    }} while(0)

#define ASSERT_LT(a, b) \
    do { if (!((a) < (b))) { \
        std::ostringstream ss; \
        ss << "ASSERT_LT failed: " << (a) << " >= " << (b); \
        throw SadTest::AssertionError(ss.str(), __FILE__, __LINE__); \
    }} while(0)

#define ASSERT_LE(a, b) \
    do { if (!((a) <= (b))) { \
        std::ostringstream ss; \
        ss << "ASSERT_LE failed: " << (a) << " > " << (b); \
        throw SadTest::AssertionError(ss.str(), __FILE__, __LINE__); \
    }} while(0)

#define ASSERT_GT(a, b) \
    do { if (!((a) > (b))) { \
        std::ostringstream ss; \
        ss << "ASSERT_GT failed: " << (a) << " <= " << (b); \
        throw SadTest::AssertionError(ss.str(), __FILE__, __LINE__); \
    }} while(0)

#define ASSERT_GE(a, b) \
    do { if (!((a) >= (b))) { \
        std::ostringstream ss; \
        ss << "ASSERT_GE failed: " << (a) << " < " << (b); \
        throw SadTest::AssertionError(ss.str(), __FILE__, __LINE__); \
    }} while(0)

#define ASSERT_NULL(ptr) \
    do { if ((ptr) != nullptr) throw SadTest::AssertionError("ASSERT_NULL failed", __FILE__, __LINE__); } while(0)

#define ASSERT_NOT_NULL(ptr) \
    do { if ((ptr) == nullptr) throw SadTest::AssertionError("ASSERT_NOT_NULL failed", __FILE__, __LINE__); } while(0)

#define ASSERT_NEAR(actual, expected, tolerance) \
    do { if (std::abs((actual) - (expected)) > (tolerance)) { \
        std::ostringstream ss; \
        ss << "ASSERT_NEAR failed: |" << (actual) << " - " << (expected) << "| > " << (tolerance); \
        throw SadTest::AssertionError(ss.str(), __FILE__, __LINE__); \
    }} while(0)

#define ASSERT_THROWS(expr, ExceptionType) \
    do { \
        bool caught = false; \
        try { expr; } catch (const ExceptionType&) { caught = true; } catch (...) {} \
        if (!caught) throw SadTest::AssertionError("ASSERT_THROWS failed: no " #ExceptionType " thrown", __FILE__, __LINE__); \
    } while(0)

#define ASSERT_NO_THROW(expr) \
    do { \
        try { expr; } catch (...) { \
            throw SadTest::AssertionError("ASSERT_NO_THROW failed: exception thrown", __FILE__, __LINE__); \
        } \
    } while(0)

#define FAIL(msg) throw SadTest::AssertionError(msg, __FILE__, __LINE__)

// ══════════════════════════════════════════════════════════════════════════════
// Macros — BDD Style
// ══════════════════════════════════════════════════════════════════════════════

#define describe(desc, block) SadTest::TestRunner::instance().describe(desc, block)

#define it(desc, fn) SadTest::TestRunner::instance().it(desc, fn, __FILE__, __LINE__)

#define xit(desc) SadTest::TestRunner::instance().xit(desc, __FILE__, __LINE__)

#define expect(value) SadTest::Expectation(value, __FILE__, __LINE__)

// ══════════════════════════════════════════════════════════════════════════════
// Run macro for main
// ══════════════════════════════════════════════════════════════════════════════

#define SAD_RUN_TESTS() SadTest::TestRunner::instance().run(argc, argv)

} // namespace SadTest
