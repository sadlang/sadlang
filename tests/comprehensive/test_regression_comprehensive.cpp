/**
 * @file test_regression_comprehensive.cpp
 * @brief (AR) اختبارات الانحدار الشاملة — تشغيل ملفات .ص عبر المفسر
 * @brief (EN) Comprehensive Regression Tests — Run .ص files via interpreter
 *
 * ═══════════════════════════════════════════════════════════════════════
 *  يشغّل 30 ملف اختبار .ص من tests/regression/ عبر sad.exe
 *  ويتحقق من مخرجات PASS/FAIL لكل اختبار.
 *
 *  الأخطاء المعروفة (XFAIL) تُعامل خصيصاً:
 *    - إذا فشل اختبار XFAIL → PASS (الخطأ لا يزال موجوداً كما هو متوقع)
 *    - إذا نجح اختبار XFAIL → يُعلّم أن الخطأ قد أُصلح
 *
 *  ■ التغطية: 30 اختبار انحدار (22 من مشاكل.md + 8 إضافية)
 *    - P0 (عاجل): 10 أخطاء
 *    - P1 (مهم):  14 اختبار
 *    - P2 (تحسين): 6 اختبارات
 * ═══════════════════════════════════════════════════════════════════════
 *
 * @note يتطلب بناء sad.exe أولاً
 * @note المسارات تُمرر من CMake عبر -DSAD_EXE_PATH و -DREGRESSION_DIR
 *
 * @author فريق تطوير لغة ص — سعيد (مهندس المترجم)
 */

#include "sad_test_framework.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>

#ifdef _WIN32
    #define popen _popen
    #define pclose _pclose
    #include <windows.h>
#else
    #include <unistd.h>
#endif

// ══════════════════════════════════════════════════════════════════════
//  تحويل UTF-8 ↔ UTF-16 على Windows
// ══════════════════════════════════════════════════════════════════════
#ifdef _WIN32
static std::wstring utf8_to_wide(const std::string& utf8) {
    if (utf8.empty()) return {};
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)utf8.size(), nullptr, 0);
    std::wstring wide(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)utf8.size(), &wide[0], len);
    return wide;
}

static std::string wide_to_utf8(const std::wstring& wide) {
    if (wide.empty()) return {};
    int len = WideCharToMultiByte(CP_UTF8, 0, wide.data(), (int)wide.size(), nullptr, 0, nullptr, nullptr);
    std::string utf8(len, 0);
    WideCharToMultiByte(CP_UTF8, 0, wide.data(), (int)wide.size(), &utf8[0], len, nullptr, nullptr);
    return utf8;
}
#endif

// ══════════════════════════════════════════════════════════════════════
//  مسارات تُمرر من CMake
// ══════════════════════════════════════════════════════════════════════
#ifndef SAD_EXE_PATH
    #define SAD_EXE_PATH ""
#endif

#ifndef REGRESSION_DIR
    #define REGRESSION_DIR ""
#endif

// ══════════════════════════════════════════════════════════════════════
//  بنية اختبار الانحدار / Regression Test Definition
// ══════════════════════════════════════════════════════════════════════
struct RegressionTest {
    std::string filename;      // اسم الملف / filename
    std::string description;   // الوصف / description
    std::string priority;      // P0, P1, P2
    bool xfail;                // هل الفشل متوقع / expected to fail
    int timeout_seconds;       // مهلة التنفيذ / execution timeout
};

// ══════════════════════════════════════════════════════════════════════
//  قائمة الاختبارات / Test Registry
// ══════════════════════════════════════════════════════════════════════
static const std::vector<RegressionTest> REGRESSION_TESTS = {
    // P0 - عاجل / Critical
    {"test_p01_array_key_access.ص",     "arr[i][key] داخل الدوال",           "P0", true,  10},
    {"test_p02_while_update.ص",         "تحديث المتغير في بينما",            "P0", false, 10},  // Green test: الخطأ #2 مُصلح
    {"test_p03_length_depth.ص",         "الطول() بعمق استدعاء ≥ 2",           "P0", true,  10},
    {"test_p09_reserved_words.ص",       "كلمات محجوزة غير موثقة",             "P0", true,  10},
    {"test_p10_void_return.ص",          "ارجع بدون قيمة",                    "P0", true,  10},
    {"test_p11_custom_class_fields.ص",  "أصناف مخصصة كحقول",                 "P0", true,  10},
    {"test_p12_integer_division.ص",     "القسمة الصحيحة تنتج عشري",           "P0", true,  10},
    {"test_p16_compound_field.ص",       "+= على حقل كائن في مصفوفة",         "P0", true,  10},
    {"test_p18_this_method_save.ص",     "هذا.method() لا يحفظ التغييرات",     "P0", true,  10},
    {"test_p19_hex_literals.ص",         "0xFF غير مدعومة",                    "P0", true,  10},

    // P1 - مهم / Important
    {"test_p04_hamza_var.ص",            "متغيرات الهمزة إ",                   "P1", true,  10},
    {"test_p08_break.ص",               "توقف واستمر في الحلقات",              "P1", true,  10},
    {"test_p13_class_prefix.ص",         "صنف كبادئة للمعرفات",                "P1", true,  10},
    {"test_p14_cascading_errors.ص",     "خطأ واحد يسبب سلسلة",               "P1", true,  10},
    {"test_p17_undocumented_reserved.ص","جديد/سجل محجوزة بدون توثيق",         "P1", true,  10},
    {"test_p20_super_reserved.ص",       "أساس محجوزة كـ KEYWORD_SUPER",       "P1", true,  10},
    {"test_p23_match_range_pattern.ص",  "RangePattern في طابق/عندما",          "P1", false, 10},
    {"test_p24_json_unified_builtin.ص", "JSON unified entry points",            "P1", false, 10},
    {"test_p25_xml_unified_builtin.ص",  "XML unified entry points",             "P1", false, 10},
    {"test_p26_json_invalid_input.ص",   "JSON invalid input throws",             "P1", false, 10},
    {"test_p27_xml_invalid_input.ص",    "XML invalid input throws",              "P1", false, 10},
    {"test_p28_filesystem_smoke.ص",     "Filesystem smoke read/write",           "P1", false, 10},
    {"test_p29_filesystem_invalid_input.ص", "Filesystem invalid input throws",   "P1", false, 10},
    {"test_p30_database_availability.ص", "Database availability contract",        "P1", false, 10},
    {"test_p31_json_unicode_parse.ص",  "JSON Unicode escape parsing",           "P1", false, 10},
    {"test_p32_json_unicode_stringify.ص", "JSON Unicode escape stringify",       "P1", false, 10},

    // P2 - تحسين / Enhancement
    {"test_p05_mixed_chars.ص",          "خلط عربي-لاتيني في المعرفات",        "P2", false, 10},
    {"test_p06_large_numbers.ص",        "الأعداد الكبيرة والتدوين العلمي",     "P2", false, 10},
    {"test_p07_array_pluseq.ص",         "+= لإضافة عناصر للمصفوفة",           "P2", true,  10},
    {"test_p15_utf8_windows.ص",         "UTF-8 في طرفية Windows",            "P2", false, 10},
    {"test_p21_template_reserved.ص",    "قالب محجوزة",                        "P2", true,  10},
    {"test_p22_true_literal.ص",         "صحيح/خطأ كقيم منطقية",              "P2", false, 10},
};

// ══════════════════════════════════════════════════════════════════════
//  دوال مساعدة / Helper Functions
// ══════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) تشغيل أمر خارجي والتقاط المخرجات — يدعم UTF-8/Unicode بالكامل
 * @brief (EN) Execute an external command and capture stdout — full Unicode support via CreateProcessW
 */
static std::pair<std::string, int> executeCommand(const std::string& cmd, int timeout_seconds) {
    std::string output;

#ifdef _WIN32
    // ── إنشاء أنبوب لالتقاط stdout+stderr ──
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = 1;

    HANDLE hReadPipe = nullptr, hWritePipe = nullptr;
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        return {"ERROR: CreatePipe failed", -1};
    }
    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);

    // ── بناء سطر الأمر بـ UTF-16 ──
    std::wstring wcmd = utf8_to_wide("cmd /c \"" + cmd + " 2>&1\"");

    STARTUPINFOW si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = hWritePipe;
    si.hStdError  = hWritePipe;
    si.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);

    PROCESS_INFORMATION pi{};
    BOOL ok = CreateProcessW(
        nullptr, &wcmd[0], nullptr, nullptr,
        1 /*bInheritHandles*/, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);

    CloseHandle(hWritePipe);

    if (!ok) {
        CloseHandle(hReadPipe);
        return {"ERROR: CreateProcessW failed (" + std::to_string(GetLastError()) + ")", -1};
    }

    // ── قراءة المخرجات مع مراقبة المهلة ──
    auto start = std::chrono::steady_clock::now();
    DWORD bytesRead = 0;
    char buf[4096];

    while (true) {
        // تحقق من المهلة
        auto elapsed = std::chrono::steady_clock::now() - start;
        auto secs = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
        if (secs > timeout_seconds) {
            TerminateProcess(pi.hProcess, 1);
            CloseHandle(hReadPipe);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return {"TIMEOUT: exceeded " + std::to_string(timeout_seconds) + "s", 124};
        }

        BOOL success = ReadFile(hReadPipe, buf, sizeof(buf) - 1, &bytesRead, nullptr);
        if (!success || bytesRead == 0) break;
        buf[bytesRead] = '\0';
        output += buf;
    }

    WaitForSingleObject(pi.hProcess, 5000);
    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(hReadPipe);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return {output, static_cast<int>(exitCode)};

#else
    // على Linux/macOS نستخدم timeout + popen
    std::string full_cmd = "timeout " + std::to_string(timeout_seconds) + " " + cmd + " 2>&1";

    auto start = std::chrono::steady_clock::now();
    FILE* pipe = popen(full_cmd.c_str(), "r");
    if (!pipe) {
        return {"ERROR: Failed to open pipe", -1};
    }

    std::array<char, 4096> buffer;
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        output += buffer.data();

        auto elapsed = std::chrono::steady_clock::now() - start;
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
        if (seconds > timeout_seconds) {
            pclose(pipe);
            return {"TIMEOUT: exceeded " + std::to_string(timeout_seconds) + "s", 124};
        }
    }

    int status = pclose(pipe);
    int exit_code = WEXITSTATUS(status);
    return {output, exit_code};
#endif
}

/**
 * @brief (AR) عدّ أسطر PASS و FAIL في المخرجات
 * @brief (EN) Count PASS and FAIL lines in output
 */
static std::pair<int, int> countPassFail(const std::string& output) {
    int pass_count = 0;
    int fail_count = 0;
    
    std::istringstream stream(output);
    std::string line;
    while (std::getline(stream, line)) {
        // تحقق من بداية السطر بـ PASS: أو FAIL:
        if (line.find("PASS:") != std::string::npos || line.find("PASS ") == 0) {
            pass_count++;
        }
        if (line.find("FAIL:") != std::string::npos || line.find("FAIL ") == 0) {
            fail_count++;
        }
    }
    
    return {pass_count, fail_count};
}

/**
 * @brief (AR) بناء مسار المفسر تلقائياً
 * @brief (EN) Auto-detect interpreter path
 */
static std::string findSadExe() {
    std::string configured = SAD_EXE_PATH;
    if (!configured.empty()) {
        std::ifstream test(configured);
        if (test.good()) return configured;
    }
    
    // محاولة المسارات الشائعة
    std::vector<std::string> candidates = {
#ifdef _WIN32
        "sad.exe",
        "../bin/Debug/sad.exe",
        "../../bin/Debug/sad.exe",
        "../../../build/bin/Debug/sad.exe",
#else
        "./sad",
        "../bin/sad",
        "../../bin/sad",
        "../../../build/bin/sad",
#endif
    };
    
    for (auto& path : candidates) {
        std::ifstream test(path);
        if (test.good()) return path;
    }
    
    return "";
}

/**
 * @brief (AR) بناء مسار مجلد الاختبارات
 * @brief (EN) Auto-detect regression test directory
 */
static std::string findRegressionDir() {
    std::string configured = REGRESSION_DIR;
    if (!configured.empty()) {
        return configured;
    }
    
    std::vector<std::string> candidates = {
        "../../../tests/regression",
        "../../tests/regression",
        "../tests/regression",
        "tests/regression",
    };
    
    for (auto& path : candidates) {
        std::string check = path + "/test_p01_array_key_access.ص";
#ifdef _WIN32
        std::ifstream test(utf8_to_wide(check));
#else
        std::ifstream test(check);
#endif
        if (test.good()) return path;
    }
    
    return "";
}

// ══════════════════════════════════════════════════════════════════════
//  نقطة الدخول / Main
// ══════════════════════════════════════════════════════════════════════
int main() {
    SAD_TEST_INIT();
    
    // ── البحث عن المسارات ──
    std::string sad_exe = findSadExe();
    std::string regression_dir = findRegressionDir();
    
    // ── التحقق من المتطلبات ──
    SAD_GROUP("Regression.Prerequisites / المتطلبات");
    
    SAD_TEST("PRE01: sad.exe موجود / Interpreter found", {
        SAD_ASSERT_FALSE(sad_exe.empty());
    });
    
    SAD_TEST("PRE02: مجلد الاختبارات موجود / Regression dir found", {
        SAD_ASSERT_FALSE(regression_dir.empty());
    });
    
    if (sad_exe.empty() || regression_dir.empty()) {
        if (sad_exe.empty()) {
            std::cerr << "خطأ: لم يُعثر على sad.exe — تأكد من بناء المشروع أولاً\n";
            std::cerr << "ERROR: sad.exe not found — build the project first\n";
        }
        if (regression_dir.empty()) {
            std::cerr << "خطأ: لم يُعثر على مجلد tests/regression/\n";
            std::cerr << "ERROR: tests/regression/ directory not found\n";
        }
        SAD_SUMMARY();
    }

    // ── إحصائيات ──
    int total_run = 0;
    int total_xfail_expected = 0;
    int total_xfail_fixed = 0;
    int total_pass = 0;
    int total_unexpected_fail = 0;
    int total_timeout = 0;

    // ══════════════════════════════════════════════════════════════════
    //  P0 — اختبارات عاجلة / Critical Priority Tests
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Regression.P0_Critical / أخطاء_عاجلة");
    
    for (const auto& test : REGRESSION_TESTS) {
        if (test.priority != "P0") continue;
        
        std::string test_name = test.priority + ": " + test.description;
        total_run++;
        
        SAD_TEST(test_name.c_str(), {
            std::string filepath = regression_dir + "/" + test.filename;
            std::string cmd = "\"" + sad_exe + "\" \"" + filepath + "\"";
            
            auto [output, exit_code] = executeCommand(cmd, test.timeout_seconds);
            auto [passes, fails] = countPassFail(output);
            
            if (exit_code == 124) {
                // Timeout
                total_timeout++;
                if (test.xfail) {
                    // مهلة متوقعة (مثل حلقة لا نهائية)
                    total_xfail_expected++;
                    // XFAIL timeout — OK
                } else {
                    SAD_ASSERT_TRUE(false);  // تجاوز غير متوقع
                }
            } else if (fails > 0 || exit_code != 0) {
                // فشل
                if (test.xfail) {
                    total_xfail_expected++;
                    // XFAIL — الفشل متوقع
                } else {
                    total_unexpected_fail++;
                    std::string msg = "فشل غير متوقع: " + test.filename + 
                                     "\nPASS=" + std::to_string(passes) + 
                                     " FAIL=" + std::to_string(fails) +
                                     "\nOutput:\n" + output.substr(0, 500);
                    SAD_ASSERT_TRUE(false);
                }
            } else {
                // نجاح
                if (test.xfail) {
                    total_xfail_fixed++;
                    // الخطأ أُصلح!
                }
                total_pass++;
            }
        });
    }

    // ══════════════════════════════════════════════════════════════════
    //  P1 — اختبارات مهمة / Important Priority Tests
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Regression.P1_Important / أخطاء_مهمة");
    
    for (const auto& test : REGRESSION_TESTS) {
        if (test.priority != "P1") continue;
        
        std::string test_name = test.priority + ": " + test.description;
        total_run++;
        
        SAD_TEST(test_name.c_str(), {
            std::string filepath = regression_dir + "/" + test.filename;
            std::string cmd = "\"" + sad_exe + "\" \"" + filepath + "\"";
            
            auto [output, exit_code] = executeCommand(cmd, test.timeout_seconds);
            auto [passes, fails] = countPassFail(output);
            
            if (exit_code == 124) {
                total_timeout++;
                if (test.xfail) {
                    total_xfail_expected++;
                } else {
                    SAD_ASSERT_TRUE(false);
                }
            } else if (fails > 0 || exit_code != 0) {
                if (test.xfail) {
                    total_xfail_expected++;
                } else {
                    total_unexpected_fail++;
                    SAD_ASSERT_TRUE(false);
                }
            } else {
                if (test.xfail) total_xfail_fixed++;
                total_pass++;
            }
        });
    }

    // ══════════════════════════════════════════════════════════════════
    //  P2 — اختبارات تحسينية / Enhancement Priority Tests
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Regression.P2_Enhancement / تحسينات");
    
    for (const auto& test : REGRESSION_TESTS) {
        if (test.priority != "P2") continue;
        
        std::string test_name = test.priority + ": " + test.description;
        total_run++;
        
        SAD_TEST(test_name.c_str(), {
            std::string filepath = regression_dir + "/" + test.filename;
            std::string cmd = "\"" + sad_exe + "\" \"" + filepath + "\"";
            
            auto [output, exit_code] = executeCommand(cmd, test.timeout_seconds);
            auto [passes, fails] = countPassFail(output);
            
            if (exit_code == 124) {
                total_timeout++;
                if (test.xfail) {
                    total_xfail_expected++;
                } else {
                    SAD_ASSERT_TRUE(false);
                }
            } else if (fails > 0 || exit_code != 0) {
                if (test.xfail) {
                    total_xfail_expected++;
                } else {
                    total_unexpected_fail++;
                    SAD_ASSERT_TRUE(false);
                }
            } else {
                if (test.xfail) total_xfail_fixed++;
                total_pass++;
            }
        });
    }

    // ══════════════════════════════════════════════════════════════════
    //  ملخص الإحصائيات / Statistics Summary
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Regression.Summary / ملخص");
    
    SAD_TEST("STATS: إحصائيات الانحدار / Regression Statistics", {
        std::cout << "\n";
        std::cout << "  ═══════════════════════════════════════════════\n";
        std::cout << "  إحصائيات اختبارات الانحدار\n";
        std::cout << "  ═══════════════════════════════════════════════\n";
        std::cout << "  إجمالي الاختبارات : " << total_run << "\n";
        std::cout << "  نجح              : " << total_pass << "\n";
        std::cout << "  فشل متوقع (XFAIL): " << total_xfail_expected << "\n";
        std::cout << "  أُصلح (كان XFAIL): " << total_xfail_fixed << "\n";
        std::cout << "  تجاوز مهلة       : " << total_timeout << "\n";
        std::cout << "  فشل غير متوقع   : " << total_unexpected_fail << "\n";
        std::cout << "  ═══════════════════════════════════════════════\n";
        
        if (total_xfail_fixed > 0) {
            std::cout << "  🎉 " << total_xfail_fixed << " خطأ تم إصلاحه! حدّث XFAIL في الاختبارات\n";
        }
        
        // هذا الاختبار ينجح دائماً — الإحصائيات فقط
        SAD_ASSERT_TRUE(true);
    });
    
    SAD_SUMMARY();
}
