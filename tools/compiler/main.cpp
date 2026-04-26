// بسم الله الرحمن الرحيم
// sadc - Sad Compiler Main Entry Point
// نقطة الدخول الرئيسية لمترجم Sad
// Phase 7.2: Compiler Driver

#include "compiler_driver.h"
#include "utf8_utils.h"
#include <iostream>
#include <cstdlib>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

/**
 * @brief Main entry point / نقطة الدخول الرئيسية
 * 
 * (AR) برنامج sadc - المترجم الرئيسي للغة Sad
 *      يدعم أسماء الملفات العربية عبر Windows Unicode API
 * 
 * أمثلة:
 * - sadc program.ص -o program
 * - sadc program.ص -O3 -o program_optimized
 * - sadc program.ص --emit-llvm -o program.ll
 * - sadc program.ص -c -o program.o
 * - sadc *.o -o program
 * 
 * (EN) sadc program - Main Sad language compiler
 *      Supports Arabic filenames via Windows Unicode API
 * 
 * Examples:
 * - sadc program.ص -o program
 * - sadc program.ص -O3 -o program_optimized
 * - sadc program.ص --emit-llvm -o program.ll
 * - sadc program.ص -c -o program.o
 * - sadc *.o -o program
 */
int main(int argc, char* argv[]) {
#ifdef _WIN32
    // (AR) الحصول على معاملات سطر الأوامر بترميز UTF-8 لدعم أسماء الملفات العربية
    // (EN) Get UTF-8 command line args to support Arabic filenames
    SetConsoleOutputCP(CP_UTF8);
    auto utf8_args = sad::utf8::get_utf8_args();
    std::vector<char*> new_argv;
    for (auto& s : utf8_args) new_argv.push_back(s.data());
    argc = static_cast<int>(new_argv.size());
    argv = new_argv.data();
#endif

    // ════════════════════════════════════════════════════════════════════════
    // (AR) معالجة أمر build android
    //      الاستخدام: sadc build android [خيارات] <ملف.ص>
    // (EN) Handle build android command
    //      Usage: sadc build android [options] <file.sad>
    // ════════════════════════════════════════════════════════════════════════
    if (argc >= 3) {
        std::string arg1 = argv[1];
        std::string arg2 = argv[2];
        
        // دعم الأمر بالعربية والإنجليزية
        if ((arg1 == "build" || arg1 == u8"بناء") &&
            (arg2 == "android" || arg2 == u8"أندرويد" || arg2 == u8"اندرويد")) {
            return sad::driver::handleBuildAndroidCommand(argc, argv);
        }
    }

    // ════════════════════════════════════════════════════════════════════════
    // (AR) معالجة أمر ui generate
    //      الاستخدام: sadc ui generate <منصة> [خيارات]
    // (EN) Handle UI generation command
    //      Usage: sadc ui generate <platform> [options]
    // ════════════════════════════════════════════════════════════════════════
    if (argc >= 2) {
        std::string arg1 = argv[1];
        if (arg1 == "ui" || arg1 == u8"واجهة") {
            return sad::driver::handleUICommand(argc, argv);
        }
    }

    // ════════════════════════════════════════════════════════════════════════
    // (AR) معالجة أوامر إدارة الحزم
    //      الاستخدام: sadc pkg <أمر> [خيارات]
    // (EN) Handle package management commands
    //      Usage: sadc pkg <command> [options]
    // ════════════════════════════════════════════════════════════════════════
    if (argc >= 2) {
        std::string arg1 = argv[1];
        if (arg1 == "pkg" || arg1 == u8"حزم") {
            return sad::driver::handlePkgCommand(argc, argv);
        }
    }

    // Create compiler driver
    sad::driver::CompilerDriver driver;
    
    // Run compilation
    int exit_code = driver.run(argc, argv);
    
    // Return exit code
    return exit_code;
}
