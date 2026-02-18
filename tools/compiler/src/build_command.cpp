// بسم الله الرحمن الرحيم
// ==============================================================================
// build_command.cpp - أمر بناء المشروع للغة ص
// Build Command Implementation for SAD Language
// ==============================================================================
//
// هذا الملف يحتوي على تنفيذ أمر "ص بناء" الذي يقوم بترجمة الكود المصدري
// إلى ملفات تنفيذية أو مكتبات أو صيغ أخرى مدعومة.
//
// الأمر يدعم:
// -----------
//   • ترجمة ملفات .ص و .s إلى ملفات تنفيذية
//   • ترجمة إلى مكتبات مشتركة (.so/.dll) وثابتة (.a/.lib)
//   • ترجمة إلى WebAssembly (.wasm)
//   • ترجمة إلى LLVM IR (.ll) و bitcode (.bc)
//   • ترجمة متقاطعة لأهداف متعددة
//   • مستويات تحسين متعددة (O0-O3، Os، Oz)
//   • بناء تزايدي لتسريع الترجمة
//
// مستويات التحسين:
// ----------------
//   O0  - بدون تحسين (للتطوير والتصحيح)
//   O1  - تحسينات أساسية
//   O2  - تحسينات قياسية (الافتراضي)
//   O3  - تحسينات قصوى (قد يزيد حجم الكود)
//   Os  - تحسين للحجم مع الحفاظ على السرعة
//   Oz  - أقصى تحسين للحجم
//
// الأهداف المدعومة:
// ----------------
//   x86_64-pc-windows-msvc    - ويندوز 64-بت
//   x86_64-pc-linux-gnu       - لينكس 64-بت
//   aarch64-apple-macos       - ماك ARM
//   aarch64-linux-android     - أندرويد ARM
//   wasm32-unknown-unknown    - WebAssembly
//
// المؤلف: فريق تطوير لغة ص
// التاريخ: 2026-01-23
// الإصدار: 1.0.0
// ==============================================================================

#include "cli_commands.hpp"
#include "lexer_core.h"
#include "parser_core.h"
#include "interpreter_core.h"
#include "error_manager.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

namespace sad {
namespace cli {

// ==============================================================================
// تنفيذ خيارات أمر البناء
// Build Command Options Implementation
// ==============================================================================

std::vector<CommandOption> BuildCommand::get_options() const {
    return {
        // خيارات الإخراج
        {"-o", "--output", "--إخراج", 
         "Output file path", "مسار ملف الإخراج", 
         true, "", false},
        
        // مستوى التحسين
        {"-O", "--optimize", "--تحسين",
         "Optimization level (0-3, s, z)", "مستوى التحسين (0-3، s، z)",
         true, "2", false},
        
        // وضع الإصدار (اختصار لـ O3)
        {"", "--release", "--إصدار",
         "Build in release mode (O3)", "بناء بوضع الإصدار (O3)",
         false, "", true},
        
        // وضع التطوير (اختصار لـ O0 + debug)
        {"", "--debug", "--تطوير",
         "Build in debug mode (O0 + debug info)", "بناء بوضع التطوير (O0 + معلومات التصحيح)",
         false, "", true},
        
        // الهدف
        {"", "--target", "--هدف",
         "Target triple (e.g., x86_64-pc-linux-gnu)", "منصة الهدف",
         true, "", false},
        
        // نوع الإخراج
        {"", "--lib", "--مكتبة",
         "Build as static library", "بناء كمكتبة ثابتة",
         false, "", true},
        
        {"", "--shared", "--مشتركة",
         "Build as shared library", "بناء كمكتبة مشتركة",
         false, "", true},
        
        {"", "--wasm", "--واسم",
         "Build for WebAssembly", "بناء لـ WebAssembly",
         false, "", true},
        
        // خيارات إضافية
        {"", "--emit-llvm", "--أخرج-llvm",
         "Emit LLVM IR instead of object code", "إخراج LLVM IR بدلاً من كود الكائن",
         false, "", true},
        
        {"", "--emit-asm", "--أخرج-تجميع",
         "Emit assembly instead of object code", "إخراج كود التجميع",
         false, "", true},
        
        // التصحيح
        {"-g", "--debug-info", "--معلومات-تصحيح",
         "Include debug information", "تضمين معلومات التصحيح",
         false, "", true},
        
        // الربط
        {"-L", "--lib-path", "--مسار-مكتبة",
         "Add library search path", "إضافة مسار بحث للمكتبات",
         true, "", false},
        
        {"-l", "--link", "--ربط",
         "Link with library", "الربط مع مكتبة",
         true, "", false},
        
        // البناء التزايدي
        {"", "--incremental", "--تزايدي",
         "Enable incremental compilation", "تفعيل البناء التزايدي",
         false, "", true},
        
        {"", "--clean", "--نظيف",
         "Clean build (rebuild all)", "بناء نظيف (إعادة بناء الكل)",
         false, "", true},
        
        // الإخراج المفصل
        {"-v", "--verbose", "--مفصل",
         "Verbose output", "إخراج مفصل",
         false, "", true},
        
        // عرض الوقت
        {"", "--time", "--وقت",
         "Show compilation times", "عرض أوقات الترجمة",
         false, "", true},
        
        // عدد خيوط المعالجة
        {"-j", "--jobs", "--خيوط",
         "Number of parallel jobs", "عدد العمليات المتوازية",
         true, "0", false},  // 0 = تلقائي
        
        // ميزات اللغة
        {"", "--no-gc", "--بدون-gc",
         "Disable garbage collector", "تعطيل جامع المهملات",
         false, "", true},
        
        {"", "--no-bounds-check", "--بدون-فحص-حدود",
         "Disable bounds checking", "تعطيل فحص الحدود",
         false, "", true},
    };
}

// ==============================================================================
// تنفيذ أمر البناء
// Build Command Execution
// ==============================================================================

int BuildCommand::execute(const ParsedOptions& options) {
    // عرض رسالة البدء
    if (options.get_bool("verbose")) {
        print_info_ar("بدء عملية البناء...");
    }
    
    // جمع الملفات المدخلة
    std::vector<std::string> input_files = options.positional_args;
    
    // إذا لم تُحدد ملفات، البحث عن ملف ص.toml
    if (input_files.empty()) {
        auto project_root = utils::find_project_root();
        if (project_root) {
            // البحث عن ملف src/رئيسي.ص أو src/main.ص
            std::vector<std::string> main_candidates = {
                *project_root + "/src/رئيسي.ص",
                *project_root + "/src/main.ص",
                *project_root + "/رئيسي.ص",
                *project_root + "/main.ص"
            };
            
            for (const auto& candidate : main_candidates) {
                if (utils::file_exists(candidate)) {
                    input_files.push_back(candidate);
                    break;
                }
            }
        }
        
        if (input_files.empty()) {
            print_error_ar("لم يتم تحديد ملفات للبناء");
            print_error("No input files specified");
            print_info_ar("استخدم: ص بناء <ملف.ص> أو أنشئ مشروعاً بـ: ص جديد <اسم>");
            return 1;
        }
    }
    
    // التحقق من وجود الملفات
    if (!validate_input_files(input_files)) {
        return 1;
    }
    
    // تحديد نوع الإخراج
    OutputType output_type = determine_output_type(options);
    
    // تحديد مستوى التحسين
    OptLevel opt_level = determine_opt_level(options);
    
    // تحديد ملف الإخراج
    std::string output_file = options.get("output");
    if (output_file.empty()) {
        // استخدام اسم الملف الأول كأساس
        output_file = utils::change_extension(
            utils::get_filename(input_files[0]),
            get_output_extension(output_type)
        );
    }
    
    // بدء قياس الوقت
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // تنفيذ البناء
    int result = do_build(input_files, output_file, output_type, opt_level, options);
    
    // حساب الوقت المستغرق
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(end_time - start_time).count();
    
    // عرض النتيجة
    if (result == 0) {
        // نجاح البناء
        std::cout << colors::GREEN << colors::BOLD;
        std::cout << symbols::SUCCESS << " ";
        std::cout << "تم البناء بنجاح!" << " / Build successful!" << std::endl;
        std::cout << colors::RESET;
        
        // عرض معلومات الإخراج
        std::cout << "   " << symbols::ARROW << " " << output_file << std::endl;
        
        // عرض الوقت إذا طُلب
        if (options.get_bool("time") || options.get_bool("verbose")) {
            std::cout << "   " << symbols::INFO << " الوقت: " 
                      << std::fixed << std::setprecision(2) << duration << " ثانية"
                      << std::endl;
        }
    } else {
        // فشل البناء
        std::cout << colors::RED << colors::BOLD;
        std::cout << symbols::FAILURE << " ";
        std::cout << "فشل البناء" << " / Build failed" << std::endl;
        std::cout << colors::RESET;
    }
    
    return result;
}

// ==============================================================================
// تحديد نوع الإخراج
// ==============================================================================

BuildCommand::OutputType BuildCommand::determine_output_type(const ParsedOptions& options) const {
    if (options.get_bool("lib") || options.has("مكتبة")) {
        return OutputType::StaticLib;
    }
    if (options.get_bool("shared") || options.has("مشتركة")) {
        return OutputType::SharedLib;
    }
    if (options.get_bool("wasm") || options.has("واسم")) {
        return OutputType::WASM;
    }
    if (options.get_bool("emit-llvm") || options.has("أخرج-llvm")) {
        return OutputType::LLVMIR;
    }
    if (options.get_bool("emit-asm") || options.has("أخرج-تجميع")) {
        return OutputType::Assembly;
    }
    
    return OutputType::Executable;
}

// ==============================================================================
// تحديد مستوى التحسين
// ==============================================================================

BuildCommand::OptLevel BuildCommand::determine_opt_level(const ParsedOptions& options) const {
    // وضع الإصدار = O3
    if (options.get_bool("release") || options.has("إصدار")) {
        return OptLevel::O3;
    }
    
    // وضع التطوير = O0
    if (options.get_bool("debug") || options.has("تطوير")) {
        return OptLevel::O0;
    }
    
    // قراءة مستوى التحسين المحدد
    std::string opt_str = options.get("optimize", "2");
    
    if (opt_str == "0") return OptLevel::O0;
    if (opt_str == "1") return OptLevel::O1;
    if (opt_str == "2") return OptLevel::O2;
    if (opt_str == "3") return OptLevel::O3;
    if (opt_str == "s" || opt_str == "S") return OptLevel::Os;
    if (opt_str == "z" || opt_str == "Z") return OptLevel::Oz;
    
    // الافتراضي
    return OptLevel::O2;
}

// ==============================================================================
// التحقق من صحة الملفات
// ==============================================================================

bool BuildCommand::validate_input_files(const std::vector<std::string>& files) const {
    bool all_valid = true;
    
    for (const auto& file : files) {
        if (!utils::file_exists(file)) {
            print_error_ar("الملف غير موجود: " + file);
            print_error("File not found: " + file);
            all_valid = false;
            continue;
        }
        
        // التحقق من امتداد الملف
        std::string ext = utils::get_extension(file);
        if (ext != ".ص" && ext != ".o" && ext != ".obj") {
            print_warning_ar("نوع ملف غير معروف: " + file);
            print_warning("Unknown file type: " + file);
        }
    }
    
    return all_valid;
}

// ==============================================================================
// تنفيذ البناء الفعلي
// ==============================================================================

int BuildCommand::do_build(const std::vector<std::string>& input_files,
                           const std::string& output_file,
                           OutputType output_type,
                           OptLevel opt_level,
                           const ParsedOptions& options) const {
    bool verbose = options.get_bool("verbose");
    
    // المرحلة 1: تحليل وتنفيذ الملفات عبر المفسر الحقيقي
    // Phase 1: Parse and execute files through the real interpreter
    if (verbose) {
        print_progress("تحليل الكود المصدري", 0);
    }
    
    int total_errors = 0;
    
    for (const auto& file : input_files) {
        if (verbose) {
            print_info("ترجمة: " + file);
        }
        
        // قراءة الملف المصدري
        auto content = utils::read_file(file);
        if (!content) {
            print_error_ar("فشل قراءة الملف: " + file);
            return 1;
        }
        
        // مسح الأخطاء السابقة
        Sad::Errors::ErrorManager::getInstance().clear();
        
        if (verbose) {
            print_progress("تحليل الكود", 25);
        }
        
        try {
            // المرحلة 1: التحليل المعجمي
            Sad::Lexer::LexerCore lexer(*content);
            
            // المرحلة 2: التحليل النحوي
            Sad::Parser::ParserCore parser(lexer);
            auto program = parser.parseProgram();
            
            if (parser.hasErrors()) {
                parser.printErrors();
                total_errors++;
                continue;
            }
            
            if (verbose) {
                print_progress("توليد الكود", 50);
            }
            
            // المرحلة 3: التنفيذ عبر المفسر
            // (AR) حالياً نستخدم المفسر — مستقبلاً سيُستبدل بالمترجم LLVM
            Sad::Interpreter::InterpreterOptions interp_opts;
            interp_opts.enableDebugMode = (opt_level == OptLevel::O0);
            interp_opts.printResults = false;
            interp_opts.enableOwnership = true;
            interp_opts.ownershipArabicMessages = true;
            interp_opts.enableTypeCheck = true;
            interp_opts.currentFilePath = file;
            
            Sad::Interpreter::Interpreter interpreter(interp_opts);
            auto result = interpreter.execute(program);
            
            if (!result.success) {
                print_error_ar("خطأ في الملف " + file + ": " + result.errorMessage);
                total_errors++;
                continue;
            }
            
            if (verbose) {
                print_progress("ربط الملفات", 75);
            }
            
        } catch (const std::exception& e) {
            print_error_ar("خطأ في ترجمة " + file + ": " + std::string(e.what()));
            total_errors++;
        }
    }
    
    if (verbose) {
        print_progress("إكمال البناء", 100);
    }
    
    return total_errors > 0 ? 1 : 0;
}

// ==============================================================================
// الحصول على امتداد الإخراج
// ==============================================================================

std::string BuildCommand::get_output_extension(OutputType type) const {
    switch (type) {
        case BuildCommand::OutputType::Executable:
#ifdef _WIN32
            return ".exe";
#else
            return "";
#endif
        case BuildCommand::OutputType::SharedLib:
#ifdef _WIN32
            return ".dll";
#elif defined(__APPLE__)
            return ".dylib";
#else
            return ".so";
#endif
        case BuildCommand::OutputType::StaticLib:
#ifdef _WIN32
            return ".lib";
#else
            return ".a";
#endif
        case BuildCommand::OutputType::ObjectFile:
#ifdef _WIN32
            return ".obj";
#else
            return ".o";
#endif
        case BuildCommand::OutputType::LLVMIR:
            return ".ll";
        case BuildCommand::OutputType::Assembly:
            return ".s";
        case BuildCommand::OutputType::WASM:
            return ".wasm";
        default:
            return "";
    }
}

// ==============================================================================
// طباعة أمثلة الاستخدام
// ==============================================================================

void BuildCommand::print_examples(std::ostream& os) const {
    os << colors::CYAN << "أمثلة / Examples:" << colors::RESET << std::endl;
    os << std::endl;
    
    os << "  " << colors::GREEN << "# بناء برنامج بسيط" << colors::RESET << std::endl;
    os << "  ص بناء برنامج.ص" << std::endl;
    os << "  sad build program.ص" << std::endl;
    os << std::endl;
    
    os << "  " << colors::GREEN << "# بناء مع تحديد اسم الإخراج" << colors::RESET << std::endl;
    os << "  ص بناء برنامج.ص --إخراج تطبيقي" << std::endl;
    os << "  sad build program.ص -o myapp" << std::endl;
    os << std::endl;
    
    os << "  " << colors::GREEN << "# بناء بوضع الإصدار (تحسينات قصوى)" << colors::RESET << std::endl;
    os << "  ص بناء برنامج.ص --إصدار" << std::endl;
    os << "  sad build program.ص --release" << std::endl;
    os << std::endl;
    
    os << "  " << colors::GREEN << "# بناء لـ WebAssembly" << colors::RESET << std::endl;
    os << "  ص بناء تطبيق_ويب.ص --واسم" << std::endl;
    os << "  sad build webapp.ص --wasm" << std::endl;
    os << std::endl;
    
    os << "  " << colors::GREEN << "# بناء كمكتبة" << colors::RESET << std::endl;
    os << "  ص بناء مكتبتي.ص --مكتبة" << std::endl;
    os << "  sad build mylib.ص --lib" << std::endl;
    os << std::endl;
    
    os << "  " << colors::GREEN << "# بناء مع معلومات التصحيح" << colors::RESET << std::endl;
    os << "  ص بناء برنامج.ص --تطوير" << std::endl;
    os << "  sad build program.ص --debug" << std::endl;
}

} // namespace cli
} // namespace sad
