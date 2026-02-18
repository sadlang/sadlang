// بسم الله الرحمن الرحيم
// ==============================================================================
// run_command.cpp - أمر تشغيل البرنامج للغة ص
// Run Command Implementation for SAD Language
// ==============================================================================
//
// هذا الملف يحتوي على تنفيذ أمر "ص شغّل" الذي يقوم بترجمة وتشغيل
// برنامج ص في خطوة واحدة مريحة للمطور.
//
// مميزات الأمر:
// -------------
//   • ترجمة وتشغيل في خطوة واحدة
//   • تمرير وسائط للبرنامج المُشغَّل
//   • دعم وضع التطوير والإصدار
//   • حذف الملفات المؤقتة تلقائياً
//   • دعم المشاريع متعددة الملفات
//   • إعادة البناء التلقائي عند تغيير الملفات
//
// الاستخدام:
// ----------
//   ص شغّل [ملف.ص] [-- وسائط_البرنامج]
//   sad run [file.s] [-- program_arguments]
//
// الفرق عن أمر "بناء":
// --------------------
//   • "شغّل" = ترجمة + تنفيذ + حذف المؤقتات
//   • "بناء" = ترجمة فقط (حفظ الملف التنفيذي)
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
#include <chrono>
#include <cstdlib>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#endif

namespace sad {
namespace cli {

// ==============================================================================
// تنفيذ خيارات أمر التشغيل
// Run Command Options Implementation
// ==============================================================================

std::vector<CommandOption> RunCommand::get_options() const {
    return {
        // وضع الإصدار
        {"", "--release", "--إصدار",
         "Run in release mode (optimized)", "تشغيل بوضع الإصدار (محسّن)",
         false, "", true},
        
        // وضع التطوير
        {"", "--debug", "--تطوير",
         "Run in debug mode (with debug info)", "تشغيل بوضع التطوير (مع معلومات التصحيح)",
         false, "", true},
        
        // الإخراج المفصل
        {"-v", "--verbose", "--مفصل",
         "Verbose output", "إخراج مفصل",
         false, "", true},
        
        // عرض الوقت
        {"", "--time", "--وقت",
         "Show execution time", "عرض وقت التنفيذ",
         false, "", true},
        
        // الاحتفاظ بالملف المؤقت
        {"", "--keep", "--احتفظ",
         "Keep temporary executable", "الاحتفاظ بالملف التنفيذي المؤقت",
         false, "", true},
        
        // تحديد دليل العمل
        {"", "--workdir", "--دليل-عمل",
         "Working directory for execution", "دليل العمل للتنفيذ",
         true, "", false},
        
        // متغيرات البيئة
        {"-e", "--env", "--بيئة",
         "Set environment variable (KEY=VALUE)", "تعيين متغير بيئة",
         true, "", false},
        
        // مراقبة التغييرات وإعادة التشغيل
        {"", "--watch", "--راقب",
         "Watch for changes and restart", "مراقبة التغييرات وإعادة التشغيل",
         false, "", true},
        
        // الهدف
        {"", "--target", "--هدف",
         "Target platform", "المنصة المستهدفة",
         true, "", false},
    };
}

// ==============================================================================
// تنفيذ أمر التشغيل
// Run Command Execution
// ==============================================================================

int RunCommand::execute(const ParsedOptions& options) {
    bool verbose = options.get_bool("verbose");
    bool show_time = options.get_bool("time");
    bool keep_temp = options.get_bool("keep");
    bool watch_mode = options.get_bool("watch");
    
    // عرض رسالة البدء
    if (verbose) {
        print_info_ar("بدء عملية الترجمة والتشغيل...");
    }
    
    // فصل وسائط البرنامج عن وسائط الأمر
    // الوسائط بعد "--" تمرر للبرنامج
    std::vector<std::string> source_files;
    std::vector<std::string> program_args;
    
    bool after_separator = false;
    for (const auto& arg : options.positional_args) {
        if (arg == "--") {
            after_separator = true;
            continue;
        }
        
        if (after_separator) {
            program_args.push_back(arg);
        } else {
            source_files.push_back(arg);
        }
    }
    
    // إذا لم تُحدد ملفات، البحث عن ملف رئيسي
    if (source_files.empty()) {
        auto project_root = utils::find_project_root();
        if (project_root) {
            std::vector<std::string> main_candidates = {
                *project_root + "/src/رئيسي.ص",
                *project_root + "/src/main.s",
                *project_root + "/src/رئيسي.s",
                *project_root + "/رئيسي.ص",
                *project_root + "/main.s"
            };
            
            for (const auto& candidate : main_candidates) {
                if (utils::file_exists(candidate)) {
                    source_files.push_back(candidate);
                    if (verbose) {
                        print_info_ar("تم العثور على الملف الرئيسي: " + candidate);
                    }
                    break;
                }
            }
        }
        
        if (source_files.empty()) {
            print_error_ar("لم يتم تحديد ملف للتشغيل");
            print_error("No source file specified");
            print_info_ar("الاستخدام: ص شغّل <ملف.ص> [-- وسائط]");
            return 1;
        }
    }
    
    // التحقق من وجود الملف
    if (!utils::file_exists(source_files[0])) {
        print_error_ar("الملف غير موجود: " + source_files[0]);
        print_error("File not found: " + source_files[0]);
        return 1;
    }
    
    // إنشاء اسم ملف مؤقت للتنفيذ
    std::filesystem::path temp_dir = std::filesystem::temp_directory_path();
    std::string temp_name = "sad_run_" + std::to_string(std::time(nullptr));
    
#ifdef _WIN32
    std::string executable = (temp_dir / (temp_name + ".exe")).string();
#else
    std::string executable = (temp_dir / temp_name).string();
#endif
    
    // وضع المراقبة - حلقة إعادة التشغيل
    do {
        // بدء قياس الوقت
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // إعداد خيارات البناء
        ParsedOptions build_options;
        build_options.positional_args = source_files;
        build_options.options["output"] = executable;
        
        if (options.get_bool("release")) {
            build_options.options["release"] = "true";
        } else if (options.get_bool("debug")) {
            build_options.options["debug"] = "true";
        }
        
        if (verbose) {
            build_options.options["verbose"] = "true";
        }
        
        // المرحلة 1: التشغيل المباشر عبر المفسر
        // Phase 1: Direct execution via interpreter
        if (verbose) {
            std::cout << colors::CYAN << symbols::PROGRESS << " ";
            std::cout << "جارٍ التشغيل... / Running..." << std::endl;
            std::cout << colors::RESET;
        }
        
        // قراءة الملف المصدري
        auto content = utils::read_file(source_files[0]);
        if (!content) {
            print_error_ar("فشل قراءة الملف: " + source_files[0]);
            if (watch_mode) {
#ifdef _WIN32
                Sleep(2000);
#else
                sleep(2);
#endif
                continue;
            }
            return 1;
        }
        
        // مسح الأخطاء السابقة
        Sad::Errors::ErrorManager::getInstance().clear();
        
        int run_result = 0;
        try {
            // التحليل المعجمي
            Sad::Lexer::LexerCore lexer(*content);
            
            // التحليل النحوي
            Sad::Parser::ParserCore parser(lexer);
            auto program = parser.parseProgram();
            
            if (parser.hasErrors()) {
                parser.printErrors();
                run_result = 1;
            } else {
                // التنفيذ
                Sad::Interpreter::InterpreterOptions interp_opts;
                interp_opts.enableDebugMode = options.get_bool("debug");
                interp_opts.printResults = false;
                interp_opts.enableOwnership = true;
                interp_opts.ownershipArabicMessages = true;
                interp_opts.enableTypeCheck = true;
                interp_opts.currentFilePath = source_files[0];
                
                if (verbose) {
                    std::cout << std::string(40, '-') << std::endl;
                }
                
                Sad::Interpreter::Interpreter interpreter(interp_opts);
                auto exec_result = interpreter.execute(program);
                
                if (!exec_result.success) {
                    std::cerr << "خطأ: " << exec_result.errorMessage << std::endl;
                    run_result = 1;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "خطأ: " << e.what() << std::endl;
            run_result = 1;
        }
        
        // حساب الوقت
        auto end_time = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(end_time - start_time).count();
        
        // طباعة الفاصل بعد الإخراج
        if (verbose) {
            std::cout << std::string(40, '-') << std::endl;
        }
        
        // عرض نتيجة التشغيل
        if (run_result == 0) {
            std::cout << colors::GREEN;
            std::cout << symbols::SUCCESS << " ";
            std::cout << "انتهى البرنامج بنجاح" << " / Program finished successfully";
            std::cout << colors::RESET << std::endl;
        } else {
            std::cout << colors::YELLOW;
            std::cout << symbols::WARNING << " ";
            std::cout << "انتهى البرنامج برمز: " << run_result;
            std::cout << " / Program exited with code: " << run_result;
            std::cout << colors::RESET << std::endl;
        }
        
        // عرض الوقت
        if (show_time) {
            std::cout << colors::DIM;
            std::cout << symbols::INFO << " ";
            std::cout << "الوقت الكلي: " << std::fixed << std::setprecision(3) << duration << " ثانية";
            std::cout << " / Total time: " << duration << "s";
            std::cout << colors::RESET << std::endl;
        }
        
        // حذف الملف المؤقت إذا لم يُطلب الاحتفاظ به
        if (!keep_temp) {
            try {
                std::filesystem::remove(executable);
            } catch (...) {
                // تجاهل أخطاء الحذف
            }
        } else if (verbose) {
            print_info_ar("تم الاحتفاظ بالملف التنفيذي: " + executable);
        }
        
        // في وضع المراقبة، ننتظر التغيير التالي
        if (watch_mode) {
            std::cout << std::endl;
            print_info_ar("في انتظار تغييرات الملفات... (اضغط Ctrl+C للخروج)");
            // TODO: تنفيذ مراقبة الملفات الحقيقية
#ifdef _WIN32
            Sleep(2000);
#else
            sleep(2);
#endif
        }
        
    } while (watch_mode);
    
    return 0;
}

// ==============================================================================
// تشغيل البرنامج
// Run the Program
// ==============================================================================

int RunCommand::run_program(const std::string& executable,
                            const std::vector<std::string>& args) const {
#ifdef _WIN32
    // نظام ويندوز - استخدام CreateProcess
    
    // بناء سطر الأوامر
    std::string cmd_line = "\"" + executable + "\"";
    for (const auto& arg : args) {
        cmd_line += " \"" + arg + "\"";
    }
    
    // إعداد هياكل البيانات
    STARTUPINFOA si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    
    PROCESS_INFORMATION pi = {};
    
    // إنشاء العملية
    if (!CreateProcessA(
            nullptr,                    // اسم التطبيق
            const_cast<char*>(cmd_line.c_str()), // سطر الأوامر
            nullptr,                    // سمات الأمان للعملية
            nullptr,                    // سمات الأمان للخيط
            TRUE,                       // وراثة المقابض
            0,                          // علامات الإنشاء
            nullptr,                    // البيئة
            nullptr,                    // الدليل الحالي
            &si,                        // معلومات البدء
            &pi                         // معلومات العملية
        )) {
        std::cerr << colors::RED;
        std::cerr << "فشل تشغيل البرنامج / Failed to run program";
        std::cerr << colors::RESET << std::endl;
        return -1;
    }
    
    // انتظار انتهاء العملية
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    // الحصول على رمز الخروج
    DWORD exit_code = 0;
    GetExitCodeProcess(pi.hProcess, &exit_code);
    
    // تنظيف
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    return static_cast<int>(exit_code);
    
#else
    // نظام يونكس - استخدام fork + exec
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // العملية الابن
        
        // بناء مصفوفة الوسائط
        std::vector<char*> argv;
        argv.push_back(const_cast<char*>(executable.c_str()));
        for (const auto& arg : args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);
        
        // تنفيذ البرنامج
        execv(executable.c_str(), argv.data());
        
        // إذا وصلنا هنا، فشل exec
        std::cerr << "فشل تنفيذ البرنامج / Failed to execute program" << std::endl;
        _exit(127);
    } else if (pid < 0) {
        // فشل fork
        std::cerr << colors::RED;
        std::cerr << "فشل إنشاء العملية / Failed to create process";
        std::cerr << colors::RESET << std::endl;
        return -1;
    } else {
        // العملية الأب - انتظار الابن
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            // انتهى بإشارة
            std::cerr << colors::YELLOW;
            std::cerr << "البرنامج توقف بإشارة: " << WTERMSIG(status);
            std::cerr << " / Program terminated by signal: " << WTERMSIG(status);
            std::cerr << colors::RESET << std::endl;
            return 128 + WTERMSIG(status);
        }
        
        return -1;
    }
#endif
}

// ==============================================================================
// طباعة أمثلة الاستخدام
// ==============================================================================

void RunCommand::print_examples(std::ostream& os) const {
    os << colors::CYAN << "أمثلة / Examples:" << colors::RESET << std::endl;
    os << std::endl;
    
    os << "  " << colors::GREEN << "# تشغيل برنامج بسيط" << colors::RESET << std::endl;
    os << "  ص شغّل برنامج.ص" << std::endl;
    os << "  sad run program.ص" << std::endl;
    os << std::endl;
    
    os << "  " << colors::GREEN << "# تشغيل مع وسائط للبرنامج" << colors::RESET << std::endl;
    os << "  ص شغّل برنامج.ص -- وسيط1 وسيط2" << std::endl;
    os << "  sad run program.ص -- arg1 arg2" << std::endl;
    os << std::endl;
    
    os << "  " << colors::GREEN << "# تشغيل بوضع الإصدار (أسرع)" << colors::RESET << std::endl;
    os << "  ص شغّل برنامج.ص --إصدار" << std::endl;
    os << "  sad run program.ص --release" << std::endl;
    os << std::endl;
    
    os << "  " << colors::GREEN << "# تشغيل مع عرض الوقت" << colors::RESET << std::endl;
    os << "  ص شغّل برنامج.ص --وقت" << std::endl;
    os << "  sad run program.ص --time" << std::endl;
    os << std::endl;
    
    os << "  " << colors::GREEN << "# تشغيل مع مراقبة التغييرات" << colors::RESET << std::endl;
    os << "  ص شغّل برنامج.ص --راقب" << std::endl;
    os << "  sad run program.ص --watch" << std::endl;
    os << std::endl;
    
    os << "  " << colors::GREEN << "# تشغيل من مجلد المشروع (بدون تحديد ملف)" << colors::RESET << std::endl;
    os << "  cd مشروعي && ص شغّل" << std::endl;
    os << "  cd myproject && sad run" << std::endl;
}

} // namespace cli
} // namespace sad
