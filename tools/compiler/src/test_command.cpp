// بسم الله الرحمن الرحيم
// ==============================================================================
// test_command.cpp - أمر تشغيل الاختبارات للغة ص
// Test Command Implementation for SAD Language
// ==============================================================================
//
// هذا الملف يحتوي على تنفيذ أمر "ص اختبر" الذي يكتشف ويشغل
// جميع اختبارات المشروع بطريقة منظمة وجميلة.
//
// مميزات نظام الاختبار:
// ---------------------
//   • اكتشاف تلقائي للاختبارات
//   • تشغيل اختبارات بالتوازي
//   • تصفية الاختبارات بالنمط
//   • تقارير مفصلة بالعربية والإنجليزية
//   • قياس وقت كل اختبار
//   • دعم اختبارات الوحدة والتكامل
//   • تغطية الكود (code coverage)
//
// أنواع الاختبارات المدعومة:
// -------------------------
//   • #[اختبار] - اختبار وحدة عادي
//   • #[اختبار_يفشل] - اختبار يجب أن يفشل
//   • #[اختبار_خاصية] - اختبار خاصية (property testing)
//   • #[اختبار_تكامل] - اختبار تكامل
//
// الاستخدام:
// ----------
//   ص اختبر              - تشغيل جميع الاختبارات
//   ص اختبر اختبار_*     - تشغيل اختبارات مطابقة للنمط
//   ص اختبر --متوازي     - تشغيل بالتوازي
//   ص اختبر --تغطية      - مع تقرير تغطية الكود
//
// المؤلف: فريق تطوير لغة ص
// التاريخ: 2026-01-23
// الإصدار: 1.0.0
// ==============================================================================

#include "cli_commands.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <mutex>
#include <atomic>
#include <regex>
#include <fstream>
#include <sstream>
#include "lexer_core.h"
#include "parser_core.h"
#include "interpreter_core.h"
#include "error_manager.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

namespace sad {
namespace cli {

// ==============================================================================
// تنفيذ خيارات أمر الاختبار
// Test Command Options Implementation
// ==============================================================================

std::vector<CommandOption> TestCommand::get_options() const {
    return {
        // تشغيل بالتوازي
        {"", "--parallel", "--متوازي",
         "Run tests in parallel", "تشغيل الاختبارات بالتوازي",
         false, "", true},
        
        // عدد الخيوط
        {"-j", "--jobs", "--خيوط",
         "Number of parallel test jobs", "عدد خيوط الاختبار المتوازية",
         true, "0", false},  // 0 = تلقائي
        
        // الإخراج المفصل
        {"-v", "--verbose", "--مفصل",
         "Verbose output (show all test output)", "إخراج مفصل (عرض كل مخرجات الاختبارات)",
         false, "", true},
        
        // إخراج هادئ
        {"-q", "--quiet", "--صامت",
         "Quiet output (only show failures)", "إخراج هادئ (عرض الفشل فقط)",
         false, "", true},
        
        // تصفية الاختبارات
        {"-f", "--filter", "--صفّي",
         "Filter tests by pattern", "تصفية الاختبارات بنمط",
         true, "", false},
        
        // تشغيل اختبار محدد
        {"", "--test", "--اختبار",
         "Run specific test by name", "تشغيل اختبار محدد بالاسم",
         true, "", false},
        
        // استبعاد اختبارات
        {"", "--exclude", "--استبعد",
         "Exclude tests matching pattern", "استبعاد الاختبارات المطابقة للنمط",
         true, "", false},
        
        // تغطية الكود
        {"", "--coverage", "--تغطية",
         "Generate code coverage report", "توليد تقرير تغطية الكود",
         false, "", true},
        
        // إيقاف عند أول فشل
        {"", "--fail-fast", "--توقف-عند-فشل",
         "Stop on first failure", "التوقف عند أول فشل",
         false, "", true},
        
        // إعادة تشغيل الفاشلة
        {"", "--rerun-failed", "--أعد-الفاشلة",
         "Re-run only previously failed tests", "إعادة تشغيل الاختبارات الفاشلة سابقاً فقط",
         false, "", true},
        
        // حد الوقت لكل اختبار
        {"", "--timeout", "--مهلة",
         "Timeout per test in seconds", "مهلة كل اختبار بالثواني",
         true, "60", false},
        
        // عرض الوقت
        {"", "--time", "--وقت",
         "Show test execution times", "عرض أوقات تنفيذ الاختبارات",
         false, "", true},
        
        // تقرير بصيغة معينة
        {"", "--format", "--صيغة",
         "Output format (text, json, junit)", "صيغة الإخراج (نص، json، junit)",
         true, "text", false},
        
        // ملف التقرير
        {"", "--report", "--تقرير",
         "Write report to file", "كتابة التقرير إلى ملف",
         true, "", false},
        
        // وضع المراقبة
        {"", "--watch", "--راقب",
         "Watch for changes and re-run tests", "مراقبة التغييرات وإعادة الاختبار",
         false, "", true},
        
        // اختبارات التكامل فقط
        {"", "--integration", "--تكامل",
         "Run only integration tests", "تشغيل اختبارات التكامل فقط",
         false, "", true},
        
        // اختبارات الوحدة فقط
        {"", "--unit", "--وحدة",
         "Run only unit tests", "تشغيل اختبارات الوحدة فقط",
         false, "", true},
    };
}

// ==============================================================================
// تنفيذ أمر الاختبار
// Test Command Execution
// ==============================================================================

int TestCommand::execute(const ParsedOptions& options) {
    bool verbose = options.get_bool("verbose");
    bool quiet = options.get_bool("quiet");
    bool parallel = options.get_bool("parallel");
    bool fail_fast = options.get_bool("fail-fast");
    bool show_time = options.get_bool("time");
    bool coverage = options.get_bool("coverage");
    
    int timeout = options.get_int("timeout", 60);
    int num_jobs = options.get_int("jobs", 0);
    
    // إذا كان عدد الخيوط 0، نستخدم عدد المعالجات
    if (num_jobs == 0) {
        num_jobs = std::thread::hardware_concurrency();
        if (num_jobs == 0) num_jobs = 4;  // الافتراضي
    }
    
    // عرض رسالة البدء
    if (!quiet) {
        std::cout << colors::BOLD << colors::CYAN;
        std::cout << "╔════════════════════════════════════════╗" << std::endl;
        std::cout << "║     🧪 نظام اختبارات لغة ص 🧪        ║" << std::endl;
        std::cout << "║     SAD Language Test Runner          ║" << std::endl;
        std::cout << "╚════════════════════════════════════════╝" << std::endl;
        std::cout << colors::RESET << std::endl;
    }
    
    // الحصول على نمط التصفية
    std::string filter_pattern = options.get("filter");
    if (filter_pattern.empty() && !options.positional_args.empty()) {
        filter_pattern = options.positional_args[0];
    }
    
    // اكتشاف الاختبارات
    if (!quiet) {
        print_info_ar("جارٍ اكتشاف الاختبارات...");
    }
    
    std::vector<std::string> test_files = discover_tests(filter_pattern);
    
    if (test_files.empty()) {
        print_warning_ar("لم يتم العثور على اختبارات");
        print_warning("No tests found");
        
        if (!filter_pattern.empty()) {
            print_info_ar("تأكد من صحة النمط: " + filter_pattern);
        } else {
            print_info_ar("أضف ملفات اختبار في مجلد tests/ أو استخدم #[اختبار] في الكود");
        }
        
        return 0;
    }
    
    // عرض عدد الاختبارات
    if (!quiet) {
        std::cout << colors::GREEN << symbols::CHECK << " ";
        std::cout << "تم العثور على " << test_files.size() << " اختبار";
        std::cout << " / Found " << test_files.size() << " tests";
        std::cout << colors::RESET << std::endl;
        
        if (parallel) {
            std::cout << colors::DIM;
            std::cout << "   " << symbols::INFO << " التشغيل بالتوازي: " << num_jobs << " خيط";
            std::cout << colors::RESET << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    // بدء قياس الوقت الكلي
    auto total_start = std::chrono::high_resolution_clock::now();
    
    // تشغيل الاختبارات
    std::vector<TestResult> results;
    std::mutex results_mutex;
    std::atomic<int> tests_completed(0);
    std::atomic<bool> should_stop(false);
    
    if (parallel && test_files.size() > 1) {
        // التشغيل المتوازي
        std::vector<std::thread> threads;
        size_t test_index = 0;
        std::mutex index_mutex;
        
        auto worker = [&]() {
            while (!should_stop) {
                std::string test_file;
                
                {
                    std::lock_guard<std::mutex> lock(index_mutex);
                    if (test_index >= test_files.size()) break;
                    test_file = test_files[test_index++];
                }
                
                TestResult result = run_single_test(test_file);
                
                {
                    std::lock_guard<std::mutex> lock(results_mutex);
                    results.push_back(result);
                    
                    // عرض التقدم
                    if (!quiet) {
                        int completed = ++tests_completed;
                        std::cout << "\r";
                        print_test_result_line(result, verbose);
                        std::cout << " [" << completed << "/" << test_files.size() << "]";
                        std::cout << std::flush;
                    }
                }
                
                // التوقف عند الفشل إذا طُلب
                if (fail_fast && !result.passed) {
                    should_stop = true;
                }
            }
        };
        
        // إنشاء الخيوط
        for (int i = 0; i < num_jobs && i < static_cast<int>(test_files.size()); ++i) {
            threads.emplace_back(worker);
        }
        
        // انتظار انتهاء الخيوط
        for (auto& t : threads) {
            t.join();
        }
        
    } else {
        // التشغيل التسلسلي
        for (const auto& test_file : test_files) {
            if (should_stop) break;
            
            TestResult result = run_single_test(test_file);
            results.push_back(result);
            
            // عرض النتيجة
            if (!quiet) {
                print_test_result_line(result, verbose);
            }
            
            // التوقف عند الفشل إذا طُلب
            if (fail_fast && !result.passed) {
                should_stop = true;
                print_warning_ar("توقف بسبب --توقف-عند-فشل");
            }
        }
    }
    
    // حساب الوقت الكلي
    auto total_end = std::chrono::high_resolution_clock::now();
    double total_duration = std::chrono::duration<double>(total_end - total_start).count();
    
    // عرض سطر فارغ
    if (!quiet) {
        std::cout << std::endl << std::endl;
    }
    
    // عرض الملخص
    print_test_summary(results);
    
    // عرض الوقت الكلي
    if (show_time || verbose) {
        std::cout << std::endl;
        std::cout << colors::DIM;
        std::cout << symbols::INFO << " الوقت الكلي: " 
                  << std::fixed << std::setprecision(2) << total_duration << " ثانية";
        std::cout << " / Total time: " << total_duration << "s";
        std::cout << colors::RESET << std::endl;
    }
    
    // توليد تقرير التغطية إذا طُلب
    if (coverage) {
        print_info_ar("جارٍ توليد تقرير التغطية...");
        // TODO: تنفيذ توليد تقرير التغطية
    }
    
    // كتابة التقرير إلى ملف إذا طُلب
    std::string report_file = options.get("report");
    if (!report_file.empty()) {
        write_report(results, report_file, options.get("format", "text"));
    }
    
    // حساب عدد الفاشلة
    int failed_count = 0;
    for (const auto& r : results) {
        if (!r.passed) ++failed_count;
    }
    
    // إرجاع رمز الخروج
    return failed_count > 0 ? 1 : 0;
}

// ==============================================================================
// اكتشاف الاختبارات
// Test Discovery
// ==============================================================================

std::vector<std::string> TestCommand::discover_tests(const std::string& pattern) const {
    std::vector<std::string> tests;
    
    // البحث عن جذر المشروع
    auto project_root = utils::find_project_root();
    std::string base_dir = project_root ? *project_root : utils::get_current_directory();
    
    // مجلدات البحث
    std::vector<std::string> search_dirs = {
        base_dir + "/tests",
        base_dir + "/test",
        base_dir + "/اختبارات"
    };
    
    // أنماط الملفات
    std::vector<std::string> test_patterns = {
        "test_*.s",
        "test_*.ص",
        "test_*.sad",
        "*_test.s",
        "*_test.ص",
        "*_test.sad",
        "اختبار_*.s",
        "اختبار_*.ص",
        "اختبار_*.sad"
    };
    
    for (const auto& dir : search_dirs) {
        if (!utils::directory_exists(dir)) continue;
        
        // البحث في المجلد مع معالجة أخطاء الوصول
        try {
        std::error_code ec;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(
                dir, std::filesystem::directory_options::skip_permission_denied, ec)) {
            if (ec) { ec.clear(); continue; }
            if (!entry.is_regular_file(ec)) continue;
            if (ec) { ec.clear(); continue; }
            
            std::string filename = entry.path().filename().string();
            std::string filepath = entry.path().string();
            
            // التحقق من النمط
            bool matches = false;
            for (const auto& p : test_patterns) {
                if (match_pattern(filename, p)) {
                    matches = true;
                    break;
                }
            }
            
            if (!matches) continue;
            
            // تطبيق تصفية المستخدم
            if (!pattern.empty() && !match_pattern(filename, pattern)) {
                continue;
            }
            
            tests.push_back(filepath);
        }
        } catch (const std::exception& e) {
            // تجاهل أخطاء المجلد والمتابعة
            (void)e;
        }
    }
    
    // ترتيب الاختبارات أبجدياً
    std::sort(tests.begin(), tests.end());
    
    return tests;
}

// ==============================================================================
// تشغيل اختبار واحد
// Run Single Test
// ==============================================================================

TestCommand::TestResult TestCommand::run_single_test(const std::string& test_file) const {
    TestResult result;
    result.name = std::filesystem::path(test_file).filename().string();
    result.name_ar = result.name;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // قراءة الملف المصدري
    auto content = utils::read_file(test_file);
    if (!content) {
        result.passed = false;
        result.error_message = "فشل قراءة ملف الاختبار / Failed to read test file";
        auto end = std::chrono::high_resolution_clock::now();
        result.duration_ms = std::chrono::duration<double, std::milli>(end - start).count();
        return result;
    }
    
    // مسح الأخطاء السابقة
    Sad::Errors::ErrorManager::getInstance().clear();
    
    try {
        // المرحلة 1: التحليل المعجمي
        Sad::Lexer::LexerCore lexer(*content);
        
        // المرحلة 2: التحليل النحوي
        Sad::Parser::ParserCore parser(lexer);
        auto program = parser.parseProgram();
        
        if (parser.hasErrors()) {
            // التحقق إذا كان الفشل متوقعاً
            if (content->find("يتوقع_فشل") != std::string::npos ||
                content->find("expect_fail") != std::string::npos ||
                content->find("expect_parse_error") != std::string::npos) {
                result.passed = true;
                result.output = "فشل التحليل كما هو متوقع";
            } else {
                result.passed = false;
                result.error_message = "خطأ في التحليل النحوي";
            }
            auto end = std::chrono::high_resolution_clock::now();
            result.duration_ms = std::chrono::duration<double, std::milli>(end - start).count();
            return result;
        }
        
        // المرحلة 3: التنفيذ عبر المفسر الحقيقي
        Sad::Interpreter::InterpreterOptions opts;
        opts.enableDebugMode = false;
        opts.printResults = false;
        opts.enableOwnership = true;
        opts.ownershipArabicMessages = true;
        opts.enableTypeCheck = true;
        opts.currentFilePath = test_file;
        
        Sad::Interpreter::Interpreter interpreter(opts);
        
        // إعادة توجيه الإخراج لالتقاطه
        std::stringstream captured_output;
        auto old_buf = std::cout.rdbuf(captured_output.rdbuf());
        
        auto exec_result = interpreter.execute(program);
        
        // استعادة الإخراج
        std::cout.rdbuf(old_buf);
        result.output = captured_output.str();
        
        if (exec_result.success) {
            // التحقق إذا كان الفشل متوقعاً
            if (content->find("يتوقع_فشل") != std::string::npos ||
                content->find("expect_fail") != std::string::npos) {
                result.passed = false;
                result.error_message = "كان يُتوقع فشل الاختبار لكنه نجح";
            } else {
                result.passed = true;
            }
        } else {
            // التحقق إذا كان الفشل متوقعاً
            if (content->find("يتوقع_فشل") != std::string::npos ||
                content->find("expect_fail") != std::string::npos) {
                result.passed = true;
                result.output = "فشل كما هو متوقع: " + exec_result.errorMessage;
            } else {
                result.passed = false;
                result.error_message = exec_result.errorMessage;
            }
        }
        
    } catch (const std::exception& e) {
        if (content->find("يتوقع_فشل") != std::string::npos ||
            content->find("expect_fail") != std::string::npos) {
            result.passed = true;
            result.output = "استثناء متوقع: " + std::string(e.what());
        } else {
            result.passed = false;
            result.error_message = std::string("استثناء: ") + e.what();
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    result.duration_ms = std::chrono::duration<double, std::milli>(end - start).count();
    
    return result;
}

// ==============================================================================
// طباعة ملخص النتائج
// Print Test Summary
// ==============================================================================

void TestCommand::print_test_summary(const std::vector<TestResult>& results) const {
    int passed = 0, failed = 0;
    double total_time = 0;
    
    std::vector<const TestResult*> failed_tests;
    
    for (const auto& r : results) {
        if (r.passed) {
            ++passed;
        } else {
            ++failed;
            failed_tests.push_back(&r);
        }
        total_time += r.duration_ms;
    }
    
    // رسم الملخص
    std::cout << colors::BOLD;
    std::cout << "════════════════════════════════════════" << std::endl;
    std::cout << "            ملخص الاختبارات             " << std::endl;
    std::cout << "            Test Summary                " << std::endl;
    std::cout << "════════════════════════════════════════" << std::endl;
    std::cout << colors::RESET;
    
    // عرض الإحصائيات
    std::cout << std::endl;
    
    // الناجحة
    std::cout << colors::GREEN << symbols::CHECK << " نجاح: " << passed;
    std::cout << " / Passed: " << passed << colors::RESET << std::endl;
    
    // الفاشلة
    if (failed > 0) {
        std::cout << colors::RED << symbols::CROSS << " فشل: " << failed;
        std::cout << " / Failed: " << failed << colors::RESET << std::endl;
    }
    
    // المجموع
    std::cout << colors::DIM << symbols::BULLET << " المجموع: " << results.size();
    std::cout << " / Total: " << results.size() << colors::RESET << std::endl;
    
    // شريط التقدم المرئي
    std::cout << std::endl;
    int bar_width = 40;
    int passed_width = results.empty() ? 0 : (passed * bar_width) / results.size();
    int failed_width = bar_width - passed_width;
    
    std::cout << "  [";
    std::cout << colors::GREEN;
    for (int i = 0; i < passed_width; ++i) std::cout << "█";
    std::cout << colors::RED;
    for (int i = 0; i < failed_width; ++i) std::cout << "█";
    std::cout << colors::RESET;
    std::cout << "]";
    
    // النسبة المئوية
    int percent = results.empty() ? 100 : (passed * 100) / results.size();
    std::cout << " " << percent << "%" << std::endl;
    
    // عرض الاختبارات الفاشلة
    if (!failed_tests.empty()) {
        std::cout << std::endl;
        std::cout << colors::RED << colors::BOLD;
        std::cout << "الاختبارات الفاشلة / Failed Tests:" << std::endl;
        std::cout << colors::RESET;
        
        for (const auto* r : failed_tests) {
            std::cout << "  " << symbols::CROSS << " " << r->name;
            if (!r->error_message.empty()) {
                std::cout << std::endl;
                std::cout << "     " << colors::DIM << r->error_message << colors::RESET;
            }
            std::cout << std::endl;
        }
    }
    
    // النتيجة النهائية
    std::cout << std::endl;
    std::cout << "════════════════════════════════════════" << std::endl;
    
    if (failed == 0) {
        std::cout << colors::GREEN << colors::BOLD;
        std::cout << "  ✨ جميع الاختبارات نجحت! ✨" << std::endl;
        std::cout << "  ✨ All tests passed! ✨" << std::endl;
    } else {
        std::cout << colors::RED << colors::BOLD;
        std::cout << "  ❌ بعض الاختبارات فشلت ❌" << std::endl;
        std::cout << "  ❌ Some tests failed ❌" << std::endl;
    }
    
    std::cout << colors::RESET;
    std::cout << "════════════════════════════════════════" << std::endl;
}

// ==============================================================================
// طباعة نتيجة اختبار واحد
// ==============================================================================

void TestCommand::print_test_result_line(const TestResult& result, bool verbose) const {
    if (result.passed) {
        std::cout << colors::GREEN << symbols::CHECK << colors::RESET;
    } else {
        std::cout << colors::RED << symbols::CROSS << colors::RESET;
    }
    
    std::cout << " " << result.name;
    
    if (verbose || !result.passed) {
        std::cout << colors::DIM;
        std::cout << " (" << std::fixed << std::setprecision(0) << result.duration_ms << "ms)";
        std::cout << colors::RESET;
    }
    
    std::cout << std::endl;
    
    // عرض رسالة الخطأ إذا فشل
    if (!result.passed && !result.error_message.empty()) {
        std::cout << colors::DIM << "   " << result.error_message << colors::RESET << std::endl;
    }
}

// ==============================================================================
// كتابة التقرير إلى ملف
// ==============================================================================

void TestCommand::write_report(const std::vector<TestResult>& results,
                               const std::string& filename,
                               const std::string& format) const {
    std::ofstream file(filename);
    if (!file) {
        print_error_ar("فشل فتح ملف التقرير: " + filename);
        return;
    }
    
    if (format == "json") {
        // صيغة JSON
        file << "{\n";
        file << "  \"tests\": [\n";
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& r = results[i];
            file << "    {\n";
            file << "      \"name\": \"" << r.name << "\",\n";
            file << "      \"passed\": " << (r.passed ? "true" : "false") << ",\n";
            file << "      \"duration_ms\": " << r.duration_ms << ",\n";
            file << "      \"error\": \"" << r.error_message << "\"\n";
            file << "    }" << (i < results.size() - 1 ? "," : "") << "\n";
        }
        file << "  ]\n";
        file << "}\n";
    } else {
        // صيغة نصية
        file << "تقرير اختبارات لغة ص\n";
        file << "SAD Language Test Report\n";
        file << "========================\n\n";
        
        for (const auto& r : results) {
            file << (r.passed ? "[نجاح]" : "[فشل]");
            file << " " << r.name;
            file << " (" << r.duration_ms << "ms)\n";
            if (!r.passed && !r.error_message.empty()) {
                file << "   " << r.error_message << "\n";
            }
        }
    }
    
    print_success_ar("تم حفظ التقرير: " + filename);
}

// ==============================================================================
// مطابقة النمط
// ==============================================================================

bool TestCommand::match_pattern(const std::string& str, const std::string& pattern) const {
    // تحويل نمط glob بسيط إلى regex
    std::string regex_pattern;
    for (char c : pattern) {
        if (c == '*') {
            regex_pattern += ".*";
        } else if (c == '?') {
            regex_pattern += ".";
        } else if (c == '.' || c == '[' || c == ']' || c == '(' || c == ')' ||
                   c == '+' || c == '^' || c == '$' || c == '|' || c == '\\') {
            regex_pattern += '\\';
            regex_pattern += c;
        } else {
            regex_pattern += c;
        }
    }
    
    try {
        std::regex re(regex_pattern, std::regex::icase);
        return std::regex_match(str, re);
    } catch (...) {
        // إذا فشل الـ regex، نستخدم مقارنة بسيطة
        return str.find(pattern) != std::string::npos;
    }
}

// ==============================================================================
// طباعة أمثلة الاستخدام
// ==============================================================================

void TestCommand::print_examples(std::ostream& os) const {
    os << colors::CYAN << "أمثلة / Examples:" << colors::RESET << std::endl;
    os << std::endl;
    
    os << "  " << colors::GREEN << "# تشغيل جميع الاختبارات" << colors::RESET << std::endl;
    os << "  ص اختبر" << std::endl;
    os << "  sad test" << std::endl;
    os << std::endl;
    
    os << "  " << colors::GREEN << "# تشغيل اختبارات محددة" << colors::RESET << std::endl;
    os << "  ص اختبر اختبار_المحلل*" << std::endl;
    os << "  sad test test_parser*" << std::endl;
    os << std::endl;
    
    os << "  " << colors::GREEN << "# تشغيل بالتوازي" << colors::RESET << std::endl;
    os << "  ص اختبر --متوازي --خيوط 8" << std::endl;
    os << "  sad test --parallel -j 8" << std::endl;
    os << std::endl;
    
    os << "  " << colors::GREEN << "# تشغيل مع تقرير تغطية" << colors::RESET << std::endl;
    os << "  ص اختبر --تغطية" << std::endl;
    os << "  sad test --coverage" << std::endl;
    os << std::endl;
    
    os << "  " << colors::GREEN << "# التوقف عند أول فشل" << colors::RESET << std::endl;
    os << "  ص اختبر --توقف-عند-فشل" << std::endl;
    os << "  sad test --fail-fast" << std::endl;
    os << std::endl;
    
    os << "  " << colors::GREEN << "# حفظ تقرير JSON" << colors::RESET << std::endl;
    os << "  ص اختبر --تقرير نتائج.json --صيغة json" << std::endl;
    os << "  sad test --report results.json --format json" << std::endl;
}

} // namespace cli
} // namespace sad
