/*
 * إطار عمل الاختبار / Test Framework
 * ====================================
 * 
 * إطار اختبار بسيط وفعال للمكونات LLVM
 * Simple and effective test framework for LLVM components
 */

#ifndef SAD_TEST_FRAMEWORK_H
#define SAD_TEST_FRAMEWORK_H

#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <sstream>
#include <chrono>
#include <exception>

namespace sad {
namespace testing {

/**
 * نتيجة الاختبار / Test Result
 */
enum class TestResult {
    Passed,     // نجح / Passed
    Failed,     // فشل / Failed
    Skipped     // تم التخطي / Skipped
};

/**
 * معلومات الاختبار / Test Information
 */
struct TestInfo {
    std::string name;                   // اسم الاختبار / Test name
    std::string suite;                  // مجموعة الاختبار / Test suite
    TestResult result;                  // النتيجة / Result
    std::string error_message;          // رسالة الخطأ / Error message
    double duration_ms;                 // المدة بالميلي ثانية / Duration in ms
    std::string file;                   // ملف المصدر / Source file
    int line;                           // رقم السطر / Line number
    
    TestInfo() 
        : result(TestResult::Passed), 
          duration_ms(0.0),
          line(0) {}
};

/**
 * دالة اختبار / Test Function
 */
using TestFunction = std::function<void()>;

/**
 * حالة اختبار / Test Case
 */
struct TestCase {
    std::string name;           // الاسم / Name
    std::string suite;          // المجموعة / Suite
    TestFunction function;      // الدالة / Function
    std::string file;           // الملف / File
    int line;                   // السطر / Line
    bool enabled;               // مُفعّل / Enabled
    
    TestCase(const std::string& n, const std::string& s, 
             TestFunction f, const std::string& file_path, 
             int line_num, bool is_enabled = true)
        : name(n), suite(s), function(f), 
          file(file_path), line(line_num), enabled(is_enabled) {}
};

/**
 * مدير الاختبارات / Test Manager
 */
class TestManager {
public:
    /**
     * الحصول على المثيل الوحيد / Get singleton instance
     */
    static TestManager& instance() {
        static TestManager manager;
        return manager;
    }
    
    /**
     * تسجيل اختبار / Register a test
     */
    void registerTest(const std::string& suite,
                     const std::string& name,
                     TestFunction function,
                     const std::string& file,
                     int line,
                     bool enabled = true) {
        tests_.emplace_back(name, suite, function, file, line, enabled);
    }
    
    /**
     * تشغيل جميع الاختبارات / Run all tests
     */
    int runAllTests(bool verbose = true) {
        return runTests("", verbose);
    }
    
    /**
     * تشغيل اختبارات مجموعة معينة / Run tests from specific suite
     */
    int runTests(const std::string& suite_filter, bool verbose = true) {
        results_.clear();
        
        int passed = 0;
        int failed = 0;
        int skipped = 0;
        
        if (verbose) {
            std::cout << "\n========================================" << std::endl;
            std::cout << "تشغيل الاختبارات / Running Tests" << std::endl;
            std::cout << "========================================\n" << std::endl;
        }
        
        for (const auto& test : tests_) {
            if (!test.enabled) {
                skipped++;
                continue;
            }
            
            if (!suite_filter.empty() && test.suite != suite_filter) {
                continue;
            }
            
            TestInfo info;
            info.name = test.name;
            info.suite = test.suite;
            info.file = test.file;
            info.line = test.line;
            
            if (verbose) {
                std::cout << "[تشغيل / RUN ] " << test.suite 
                         << "." << test.name << std::flush;
            }
            
            auto start_time = std::chrono::high_resolution_clock::now();
            
            try {
                test.function();
                info.result = TestResult::Passed;
                passed++;
                
                auto end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                    end_time - start_time);
                info.duration_ms = duration.count() / 1000.0;
                
                if (verbose) {
                    std::cout << " [نجح / OK] (" 
                             << info.duration_ms << " ms)" << std::endl;
                }
            } catch (const std::exception& e) {
                info.result = TestResult::Failed;
                info.error_message = e.what();
                failed++;
                
                auto end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                    end_time - start_time);
                info.duration_ms = duration.count() / 1000.0;
                
                if (verbose) {
                    std::cout << " [فشل / FAIL]" << std::endl;
                    std::cout << "  الخطأ / Error: " << e.what() << std::endl;
                    std::cout << "  الموقع / Location: " << test.file 
                             << ":" << test.line << std::endl;
                }
            }
            
            results_.push_back(info);
        }
        
        if (verbose) {
            std::cout << "\n========================================" << std::endl;
            std::cout << "النتائج / Results" << std::endl;
            std::cout << "========================================" << std::endl;
            std::cout << "الكل / Total:     " << (passed + failed + skipped) << std::endl;
            std::cout << "نجح / Passed:    " << passed << std::endl;
            std::cout << "فشل / Failed:    " << failed << std::endl;
            std::cout << "تخطى / Skipped:   " << skipped << std::endl;
            std::cout << "========================================\n" << std::endl;
        }
        
        return failed;
    }
    
    /**
     * الحصول على النتائج / Get results
     */
    const std::vector<TestInfo>& getResults() const {
        return results_;
    }
    
    /**
     * إعادة تعيين / Reset
     */
    void reset() {
        tests_.clear();
        results_.clear();
    }
    
private:
    TestManager() = default;
    TestManager(const TestManager&) = delete;
    TestManager& operator=(const TestManager&) = delete;
    
    std::vector<TestCase> tests_;
    std::vector<TestInfo> results_;
};

/**
 * مُسجل الاختبار / Test Registrar
 */
class TestRegistrar {
public:
    TestRegistrar(const std::string& suite,
                 const std::string& name,
                 TestFunction function,
                 const std::string& file,
                 int line,
                 bool enabled = true) {
        TestManager::instance().registerTest(
            suite, name, function, file, line, enabled);
    }
};

/**
 * استثناء فشل الاختبار / Test Failure Exception
 */
class TestFailure : public std::exception {
public:
    TestFailure(const std::string& message, 
                const std::string& file, 
                int line)
        : message_(buildMessage(message, file, line)) {}
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
private:
    std::string buildMessage(const std::string& msg,
                            const std::string& file,
                            int line) {
        std::ostringstream oss;
        oss << msg << " (في / at " << file << ":" << line << ")";
        return oss.str();
    }
    
    std::string message_;
};

} // namespace testing
} // namespace sad

/**
 * ماكرو لتعريف اختبار / Macro to define a test
 */
#define TEST(suite_name, test_name) \
    void suite_name##_##test_name##_TestBody(); \
    static sad::testing::TestRegistrar \
        suite_name##_##test_name##_registrar( \
            #suite_name, #test_name, \
            suite_name##_##test_name##_TestBody, \
            __FILE__, __LINE__); \
    void suite_name##_##test_name##_TestBody()

/**
 * ماكرو لتعريف اختبار معطّل / Macro to define a disabled test
 */
#define TEST_DISABLED(suite_name, test_name) \
    void suite_name##_##test_name##_TestBody(); \
    static sad::testing::TestRegistrar \
        suite_name##_##test_name##_registrar( \
            #suite_name, #test_name, \
            suite_name##_##test_name##_TestBody, \
            __FILE__, __LINE__, false); \
    void suite_name##_##test_name##_TestBody()

/**
 * ماكرو EXPECT_TRUE
 */
#define EXPECT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            throw sad::testing::TestFailure( \
                "Expected true but got false: " #condition, \
                __FILE__, __LINE__); \
        } \
    } while (0)

/**
 * ماكرو EXPECT_FALSE
 */
#define EXPECT_FALSE(condition) \
    do { \
        if (condition) { \
            throw sad::testing::TestFailure( \
                "Expected false but got true: " #condition, \
                __FILE__, __LINE__); \
        } \
    } while (0)

/**
 * ماكرو EXPECT_EQ
 */
#define EXPECT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            std::ostringstream oss; \
            oss << "Expected: " << (expected) \
                << ", Actual: " << (actual); \
            throw sad::testing::TestFailure( \
                oss.str(), __FILE__, __LINE__); \
        } \
    } while (0)

/**
 * ماكرو EXPECT_NE
 */
#define EXPECT_NE(not_expected, actual) \
    do { \
        if ((not_expected) == (actual)) { \
            std::ostringstream oss; \
            oss << "Expected not equal to: " << (not_expected) \
                << ", but got: " << (actual); \
            throw sad::testing::TestFailure( \
                oss.str(), __FILE__, __LINE__); \
        } \
    } while (0)

/**
 * ماكرو EXPECT_NULL
 */
#define EXPECT_NULL(pointer) \
    do { \
        if ((pointer) != nullptr) { \
            throw sad::testing::TestFailure( \
                "Expected null pointer but got non-null: " #pointer, \
                __FILE__, __LINE__); \
        } \
    } while (0)

/**
 * ماكرو EXPECT_NOT_NULL
 */
#define EXPECT_NOT_NULL(pointer) \
    do { \
        if ((pointer) == nullptr) { \
            throw sad::testing::TestFailure( \
                "Expected non-null pointer but got null: " #pointer, \
                __FILE__, __LINE__); \
        } \
    } while (0)

/**
 * ماكرو EXPECT_THROW
 */
#define EXPECT_THROW(statement, exception_type) \
    do { \
        bool caught = false; \
        try { \
            statement; \
        } catch (const exception_type&) { \
            caught = true; \
        } catch (...) { \
        } \
        if (!caught) { \
            throw sad::testing::TestFailure( \
                "Expected exception " #exception_type " was not thrown", \
                __FILE__, __LINE__); \
        } \
    } while (0)

/**
 * ماكرو EXPECT_NO_THROW
 */
#define EXPECT_NO_THROW(statement) \
    do { \
        try { \
            statement; \
        } catch (...) { \
            throw sad::testing::TestFailure( \
                "Unexpected exception was thrown: " #statement, \
                __FILE__, __LINE__); \
        } \
    } while (0)

#endif // SAD_TEST_FRAMEWORK_H
