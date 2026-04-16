// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════
// compiler_driver_android_linker.cpp - ربط ملفات الكائن لأندرويد
// Android Object File Linking
// ═══════════════════════════════════════════════════════════════════════════
//
// الوصف (عربي):
// --------------
// هذا الملف يوفر دالة ربط ملفات الكائن (.o) لإنتاج ملفات أندرويد
// (مكتبات مشتركة .so أو ملفات تنفيذية).
//
// المحتويات:
//   - link_android_executable() — ربط ملف كائن باستخدام NDK clang
//     مع runtime مخصص لأندرويد يستخدم __android_log_print
//
// ملف android.cpp المرافق يحتوي على أوامر بناء المشروع:
//   - buildAndroidApp()          — إنشاء هيكل مشروع أندرويد كامل
//   - handleBuildAndroidCommand() — معالجة أمر "sadc build android"
//
// Description (English):
// ----------------------
// This file provides Android object file linking to produce .so libraries
// or executables using NDK clang with Android-specific runtime.
//
// Contents:
//   - link_android_executable() — links object file using NDK clang
//     with Android-specific runtime using __android_log_print
//
// The companion android.cpp file handles project scaffolding:
//   - buildAndroidApp()          — creates full Android project structure
//   - handleBuildAndroidCommand() — handles "sadc build android" command
//
// ═══════════════════════════════════════════════════════════════════════════

#include "compiler_driver.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

namespace sad
{
    namespace driver
    {
        // ============================================================================
        // (AR) ربط ملف كائن لأندرويد باستخدام NDK clang
        // ============================================================================
        // هذه الدالة تقوم بالترجمة المتقاطعة لأندرويد:
        //   1. تبحث عن NDK clang (ليس clang العادي)
        //   2. تستخدم --target و --sysroot لأندرويد
        //   3. تولّد ملف runtime مخصص لأندرويد (يستخدم __android_log_print)
        //   4. تربط مع مكتبات أندرويد: -llog -landroid -lm -lc
        //   5. تنتج .so (مكتبة مشتركة) لتحميلها في NativeActivity
        // ============================================================================
        // (EN) Link object file for Android using NDK clang
        // Cross-compilation to Android ARM64 with proper sysroot and libraries.
        // ============================================================================

        bool CompilerDriver::link_android_executable(const std::string &obj_path,
                                                     const std::string &output_file,
                                                     llvm::Module *module)
        {
            // ================================================================
            // (AR) الخطوة 1: البحث عن NDK clang
            // (EN) Step 1: Find Android NDK clang
            // ================================================================
            auto ndk_clang_opt = find_android_ndk_clang();

            if (!ndk_clang_opt)
            {
                diagnostics_.report_fatal(
                    "لم يتم العثور على Android NDK clang\n"
                    "Android NDK clang not found\n"
                    "ثبّت Android NDK وعيّن ANDROID_NDK_HOME\n"
                    "Install Android NDK and set ANDROID_NDK_HOME\n"
                    "تحميل: https://developer.android.com/ndk/downloads");
                return false;
            }

            std::string ndk_clang = *ndk_clang_opt;
            std::string sysroot = get_android_sysroot(ndk_clang);
            std::string target_triple = options_.target.to_string();
            int api_level = options_.target.get_android_api_level();

            if (options_.verbose)
            {
                std::cerr << "\n  ══════════════════════════════════════════════\n";
                std::cerr << "  ربط لأندرويد / Linking for Android\n";
                std::cerr << "  الهدف / Target: " << target_triple << "\n";
                std::cerr << "  مستوى API / API Level: " << api_level << "\n";
                std::cerr << "  NDK clang: " << ndk_clang << "\n";
                std::cerr << "  Sysroot: " << sysroot << "\n";
                std::cerr << "  ══════════════════════════════════════════════\n";
            }

            // ================================================================
            // (AR) الخطوة 2: تحديد نوع الإخراج
            // (EN) Step 2: Determine output type
            // ================================================================
            // (AR) أندرويد يستخدم NativeActivity — الإخراج الافتراضي .so
            //      إلا إذا طلب المستخدم ملف تنفيذي صراحةً
            // (EN) Android uses NativeActivity — default output is .so
            //      unless user explicitly requested an executable
            bool output_shared = true;
            std::string actual_output = output_file;

            // (AR) إذا انتهى الاسم بـ .so فهو مكتبة مشتركة
            //      وإلا نضيف .so إذا لم ينتهِ بامتداد آخر
            if (output_file.size() >= 3 && output_file.substr(output_file.size() - 3) == ".so")
            {
                output_shared = true;
            }
            else if (output_file.find('.') == std::string::npos)
            {
                // (AR) لا امتداد — ربما يريد ملف تنفيذي (للاختبار على الجهاز عبر adb)
                output_shared = false;
            }

            // ================================================================
            // (AR) الخطوة 3: إنشاء ملف runtime لأندرويد
            // (EN) Step 3: Create Android-specific runtime file
            // ================================================================
            auto temp_runtime = get_temp_file(".c");
            temp_files_.push_back(temp_runtime);

            std::ofstream rt_file(temp_runtime);
            if (rt_file.is_open())
            {
                rt_file << R"(
/* ============================================================================
 * Sad Language - Android Runtime / مكتبة وقت التشغيل لأندرويد
 * ============================================================================
 * هذا الملف يُنشأ تلقائياً بواسطة مترجم Sad لتوفير الدوال
 * الأساسية اللازمة لتشغيل البرنامج على أندرويد.
 * ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __ANDROID__
  #include <android/log.h>
  #define SAD_LOG_TAG "SadLang"
  #define SAD_LOGI(...) __android_log_print(ANDROID_LOG_INFO, SAD_LOG_TAG, __VA_ARGS__)
  #define SAD_LOGE(...) __android_log_print(ANDROID_LOG_ERROR, SAD_LOG_TAG, __VA_ARGS__)
#else
  #define SAD_LOGI(...) fprintf(stdout, __VA_ARGS__)
  #define SAD_LOGE(...) fprintf(stderr, __VA_ARGS__)
#endif

/* ============================================================================
 * دوال الإدخال / Input Functions (محدودة على أندرويد)
 * ============================================================================ */

const char* sad_llvm_input(void) {
    /* (AR) على أندرويد، الإدخال يأتي من واجهة المستخدم وليس stdin */
    static char buffer[4096];
    buffer[0] = '\0';
    #ifndef __ANDROID__
    if (fgets(buffer, sizeof(buffer), stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') buffer[len-1] = '\0';
        return buffer;
    }
    #endif
    SAD_LOGI("sad_llvm_input: تحذير - الإدخال غير متاح على أندرويد بدون واجهة\n");
    return "";
}

long long sad_llvm_input_int(void) {
    SAD_LOGI("sad_llvm_input_int: تحذير - الإدخال غير متاح على أندرويد بدون واجهة\n");
    return 0;
}

double sad_llvm_input_float(void) {
    SAD_LOGI("sad_llvm_input_float: تحذير - الإدخال غير متاح على أندرويد بدون واجهة\n");
    return 0.0;
}

/* ============================================================================
 * دوال الذاكرة / Memory Functions
 * ============================================================================ */

void* sad_llvm_alloc(unsigned long long size) {
    const unsigned long long MAX_ALLOC = 1ULL << 30;
    if (size == 0 || size > MAX_ALLOC) {
        SAD_LOGE("[sad] تحذير: طلب تخصيص غير صالح: %llu بايت\n", size);
        return NULL;
    }
    void* ptr = malloc((size_t)size);
    if (!ptr) {
        SAD_LOGE("[sad] خطأ: فشل تخصيص %llu بايت\n", size);
    }
    return ptr;
}

void sad_llvm_free(void* ptr) {
    if (ptr) free(ptr);
}

/* ============================================================================
 * دوال النصوص / String Functions
 * ============================================================================ */

void* sad_llvm_string_new(const char* data, unsigned long long length) {
    if (!data || length > (1ULL << 30)) return NULL;
    char* str = (char*)malloc((size_t)(length + 1));
    if (!str) return NULL;
    memcpy(str, data, (size_t)length);
    str[length] = '\0';
    return str;
}

void* sad_llvm_string_from_cstr(const char* cstr) {
    if (!cstr) return NULL;
    size_t len = strlen(cstr);
    char* str = (char*)malloc(len + 1);
    if (!str) return NULL;
    memcpy(str, cstr, len + 1);
    return str;
}

unsigned long long sad_llvm_string_length(void* str) {
    if (!str) return 0;
    return (unsigned long long)strlen((const char*)str);
}

void sad_llvm_print_string(void* str) {
    if (str) {
        #ifdef __ANDROID__
        __android_log_print(ANDROID_LOG_INFO, "SadLang", "%s", (const char*)str);
        #else
        printf("%s", (const char*)str);
        #endif
    }
}

/* ============================================================================
 * دوال الأمان / Security Functions
 * ============================================================================ */

#include <time.h>

void sad_security_assert(int condition, const char* msg) {
    if (!condition) {
        SAD_LOGE("[ASSERT FAILED] %s\n", msg ? msg : "assertion failed");
        abort();
    }
}

void sad_security_panic(const char* msg) {
    SAD_LOGE("[PANIC] %s\n", msg ? msg : "panic");
    abort();
}

long long sad_security_hash(const char* str) {
    if (!str) return 0;
    unsigned long long hash = 14695981039346656037ULL;
    while (*str) {
        hash ^= (unsigned char)*str++;
        hash *= 1099511628211ULL;
    }
    return (long long)hash;
}

long long sad_security_timestamp(void) {
    return (long long)time(NULL);
}

long long sad_security_secure_random(long long min_val, long long max_val) {
    static int seeded = 0;
    if (!seeded) { srand((unsigned int)time(NULL)); seeded = 1; }
    if (min_val >= max_val) return min_val;
    long long range = max_val - min_val + 1;
    return min_val + (long long)(rand() % (int)range);
}

)";
                rt_file.close();

                if (options_.verbose)
                {
                    std::cerr << "  استخدام runtime أندرويد مؤقت: " << temp_runtime.string() << "\n";
                }
            }

            // ================================================================
            // (AR) الخطوة 4: بناء أمر الربط
            // (EN) Step 4: Build link command
            // ================================================================
            std::string command = "\"" + ndk_clang + "\"";

            // (AR) تحديد الهدف (target triple) ومسار sysroot
            command += " --target=" + target_triple;
            command += " --sysroot=\"" + sysroot + "\"";

            // (AR) إضافة ملف الكائن
            command += " \"" + obj_path + "\"";

            // (AR) إضافة ملف runtime الأندرويد
            command += " \"" + temp_runtime.string() + "\"";

            // (AR) تحديد ملف الإخراج
            command += " -o \"" + actual_output + "\"";

            // (AR) أعلام المكتبة المشتركة
            if (output_shared)
            {
                command += " -shared -fPIC";
                command += " -Wl,--build-id";     // معرف بناء لأندرويد
                command += " -Wl,--no-undefined"; // اكتشاف الرموز المفقودة
                command += " -Wl,-soname,lib" + std::filesystem::path(actual_output).stem().string() + ".so";
            }

            // (AR) مكتبات أندرويد الأساسية
            command += " -llog";     // __android_log_print
            command += " -landroid"; // NativeActivity, AAssetManager
            command += " -lm";       // دوال الرياضيات
            command += " -lc";       // مكتبة C القياسية
            command += " -ldl";      // dlopen/dlsym

            // (AR) تعطيل التحذيرات غير الضرورية
            command += " -w";

            // (AR) إضافة مسارات ومكتبات المستخدم
            for (const auto &path : options_.library_paths)
            {
                command += " -L\"" + path + "\"";
            }
            for (const auto &lib : options_.libraries)
            {
                command += " -l" + lib;
            }

            // (AR) الربط الثابت إذا طُلب
            if (options_.link_static)
            {
                command += " -static";
            }

            if (options_.verbose)
            {
                std::cerr << "\n  أمر ربط أندرويد / Android link command:\n  " << command << "\n\n";
            }

// ================================================================
// (AR) الخطوة 5: تنفيذ أمر الربط
// (EN) Step 5: Execute link command
// ================================================================
#ifdef _WIN32
            int result = std::system(("\"" + command + "\"").c_str());
#else
            int result = std::system(command.c_str());
#endif

            if (result != 0)
            {
                diagnostics_.report_fatal(
                    "فشل ربط ملف أندرويد (رمز الخطأ: " + std::to_string(result) + ")\n"
                                                                                  "Android linking failed (error code: " +
                    std::to_string(result) + ")\n"
                                             "تأكد من:\n"
                                             "  1. تثبيت Android NDK بشكل صحيح\n"
                                             "  2. تعيين ANDROID_NDK_HOME\n"
                                             "  3. أن الهدف " +
                    target_triple + " مدعوم في NDK\n"
                                    "Make sure:\n"
                                    "  1. Android NDK is properly installed\n"
                                    "  2. ANDROID_NDK_HOME is set\n"
                                    "  3. Target " +
                    target_triple + " is supported by NDK");
                return false;
            }

            // (AR) طباعة معلومات النجاح
            if (options_.verbose)
            {
                std::cerr << "  ✓ تم ربط ملف أندرويد بنجاح: " << actual_output << "\n";
                if (output_shared)
                {
                    std::cerr << "  نوع الإخراج: مكتبة مشتركة (.so)\n";
                    std::cerr << "  لاستخدامها: انسخها إلى مجلد jniLibs/arm64-v8a/ في مشروع أندرويد\n";
                }
                else
                {
                    std::cerr << "  نوع الإخراج: ملف تنفيذي\n";
                    std::cerr << "  للاختبار: adb push " << actual_output << " /data/local/tmp/\n";
                }
            }

            return true;
        }

    } // namespace driver
} // namespace sad
