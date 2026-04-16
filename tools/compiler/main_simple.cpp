// بسم الله الرحمن الرحيم
// sad - Sad Language Interpreter
// مفسر لغة ص البرمجية العربية
// Simple main.cpp that uses the interpreter directly

#include "lexer_core.h"
#include "parser_core.h"
#include "interpreter_core.h"
#include "error_manager.h"
#include "value.h"
#include "../../interpreter_new/include/exception.h"
#include "../../interpreter_new/include/debug/debug_server.h"

// الآلة الافتراضية / Bytecode VM
#include "sad_vm_compiler.h"
#include "sad_vm_executor.h"
#include "sad_vm_debug.h"

// CLI Commands for mobile etc.
#include "cli_commands.hpp"

#include "../../shared/utils/include/utf8_utils.h"
#include "../../tools/profiler/include/profiler_core.h"   // (AR) مصحح الأداء / (EN) Profiler
#include "../../tools/profiler/include/profiler_hooks.h"  // (AR) خطافات المصحح / (EN) Profiler hooks
#include "../../shared/hot_reload/include/hot_reload_engine.h"  // (AR) محرك إعادة التحميل الساخن / (EN) Hot Reload Engine

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <clocale>
#include <vector>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#endif

// طباعة المساعدة
// Print help
void print_help(const char* program_name) {
    std::cout << "لغة ص - Sad Programming Language v1.0.0\n"
              << "الاستخدام / Usage: " << program_name << " <file.s>\n\n"
              << "الأوامر / Commands:\n"
              << "  <file.s>      تنفيذ ملف / Execute file\n"
              << "  --help, -h    عرض المساعدة / Show help\n"
              << "  --version, -v عرض الإصدار / Show version\n"
              << "  --ownership   تفعيل نظام الملكية / Enable ownership system\n"
              << "  --ملكية       تفعيل نظام الملكية (عربي) / Enable ownership (Arabic)\n"
              << "  --vm, --آلة    تنفيذ عبر الآلة الافتراضية / Execute via Bytecode VM\n"
              << "  --vm-trace    تتبع تعليمات الآلة / Trace VM instructions\n"
              << "  --vm-disasm   فك البايت كود / Disassemble bytecode\n"
              << "  --debug-server خادم التصحيح (DAP) / Debug server mode (DAP)\n"
              << "  --profile     تنميط الأداء / Profile performance\n"
              << "  --تنميط       تنميط الأداء (عربي) / Profile performance (Arabic)\n"
              << "  --hot-reload  إعادة التحميل الساخن / Hot reload mode\n"
              << "  --مراقبة      مراقبة التغييرات (عربي) / Watch mode (Arabic)\n"
              << "\n"
              << "  واجهات <ملف>  توليد واجهات رسومية / Generate UI code\n"
              << "    --منصة=X    المنصة: desktop|android|ios|web\n"
              << "    --سطح-المكتب  سطح المكتب SDL2 / Desktop (SDL2)\n"
              << "    --اندرويد    أندرويد Jetpack Compose\n"
              << "    --ايفون      iOS SwiftUI\n"
              << "    --ويب       HTML/CSS/JS\n"
              << std::endl;
}

// قراءة ملف
// Read file
std::string read_file(const std::string& filename) {
    // (AR) استخدام utf8_open_ifstream لدعم أسماء الملفات العربية
    // (EN) Use utf8 open for Arabic filename support
    auto file = sad::utf8::open_ifstream(filename);
    if (!file.is_open()) {
        throw std::runtime_error("لم يتم العثور على الملف: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    // إعداد دعم UTF-8 للعربية / Setup UTF-8 support for Arabic
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    // (AR) ضبط الـ locale لدعم UTF-8 في std::cout
    // (EN) Set locale for UTF-8 support in std::cout
    setlocale(LC_ALL, ".UTF-8");
    // (AR) الحصول على معاملات UTF-8 لدعم أسماء الملفات العربية
    // (EN) Get UTF-8 args for Arabic filename support
    auto utf8_args = sad::utf8::get_utf8_args();
    std::vector<char*> new_argv;
    for (auto& s : utf8_args) new_argv.push_back(s.data());
    argc = static_cast<int>(new_argv.size());
    argv = new_argv.data();
#else
    std::setlocale(LC_ALL, "");
#endif
    
    // Check arguments
    if (argc < 2) {
        print_help(argv[0]);
        return 1;
    }
    
    std::string arg = argv[1];
    
    // Check for help/version
    if (arg == "--help" || arg == "-h") {
        print_help(argv[0]);
        return 0;
    }
    
    if (arg == "--version" || arg == "-v") {
        std::cout << "sad version 1.0.0-beta.1\n";
        return 0;
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // التحقق من أوامر CLI (هاتف/mobile, مساعدة/help, نسخة/version)
    // Check for CLI commands before falling through to file execution
    // ═══════════════════════════════════════════════════════════════════════
    if (arg == "mobile" || arg == "\xD9\x87\xD8\xA7\xD8\xAA\xD9\x81" /* هاتف */ ||
        arg == "build"  || arg == "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1" /* بناء */ ||
        arg == "test"   || arg == "\xD8\xA7\xD8\xAE\xD8\xAA\xD8\xA8\xD8\xB1" /* اختبر */ ||
        arg == "run"    || arg == "\xD8\xB4\xD8\xBA\xD9\x91\xD9\x84" /* شغّل */ ||
        arg == "new"    || arg == "\xD8\xAC\xD8\xAF\xD9\x8A\xD8\xAF" /* جديد */ ||
        arg == "help"   || arg == "\xD9\x85\xD8\xB3\xD8\xA7\xD8\xB9\xD8\xAF\xD8\xA9" /* مساعدة */ ||
        arg == "version"|| arg == "\xD9\x86\xD8\xB3\xD8\xAE\xD8\xA9" /* نسخة */ ||
        arg == "format" || arg == "\xD9\x86\xD8\xB3\xD9\x91\xD9\x82" /* نسّق */ ||
        arg == "check"  || arg == "\xD8\xAA\xD8\xAD\xD9\x82\xD9\x82" /* تحقق */ ||
        arg == "doc"    || arg == "\xD9\x88\xD8\xAB\xD9\x91\xD9\x82" /* وثّق */ ||
        arg == "add"    || arg == "\xD8\xA3\xD8\xB6\xD9\x81" /* أضف */ ||
        arg == "publish"|| arg == "\xD8\xA7\xD9\x86\xD8\xB4\xD8\xB1" /* انشر */) {
        
        sad::cli::CommandManager manager;
        return manager.run(argc, argv);
    }

    // Execute file
    try {
        // Parse CLI flags
        bool enableOwnership = false;
        bool debugOwnership = false;
        bool enableTypeCheck = false;
        bool debugTypeCheck = false;
        bool strictTypeCheck = false;
        bool enableSecurity = false;
        bool debugSecurity = false;
        bool strictSecurity = false;
        bool enableDebug = false;
        bool showOptStats = false;
        bool useVM = false;
        bool vmTrace = false;
        bool vmDisasm = false;
        bool useDebugServer = false;
        bool enableProfile = false;
        bool enableHotReload = false;
        std::string profileOutput;
        std::string profileFormat = "text";
        int profileTop = 20;
        std::string filename;
        for (int i = 1; i < argc; ++i) {
            std::string a = argv[i];
            if (a == "--ownership" || a == "--\xD9\x85\xD9\x84\xD9\x83\xD9\x8A\xD8\xA9") {
                enableOwnership = true;
            } else if (a == "--debug-ownership" || a == "--\xD8\xAA\xD8\xAA\xD8\xA8\xD8\xB9-\xD9\x85\xD9\x84\xD9\x83\xD9\x8A\xD8\xA9") {
                enableOwnership = true;
                debugOwnership = true;
            } else if (a == "--type-check" || a == "--\xD9\x81\xD8\xAD\xD8\xB5-\xD8\xA3\xD9\x86\xD9\x88\xD8\xA7\xD8\xB9") {
                enableTypeCheck = true;
            } else if (a == "--debug-types" || a == "--\xD8\xAA\xD9\x86\xD9\x82\xD9\x8A\xD8\xAD-\xD8\xA3\xD9\x86\xD9\x88\xD8\xA7\xD8\xB9") {
                enableTypeCheck = true;
                debugTypeCheck = true;
            } else if (a == "--strict-types" || a == "--\xD8\xA3\xD9\x86\xD9\x88\xD8\xA7\xD8\xB9-\xD8\xB5\xD8\xA7\xD8\xB1\xD9\x85\xD8\xA9") {
                enableTypeCheck = true;
                strictTypeCheck = true;
            } else if (a == "--security" || a == "--\xD8\xA3\xD9\x85\xD8\xA7\xD9\x86") {
                enableSecurity = true;
            } else if (a == "--debug-security" || a == "--\xD8\xAA\xD9\x86\xD9\x82\xD9\x8A\xD8\xAD-\xD8\xA3\xD9\x85\xD8\xA7\xD9\x86") {
                enableSecurity = true;
                debugSecurity = true;
            } else if (a == "--strict-security" || a == "--\xD8\xA3\xD9\x85\xD8\xA7\xD9\x86-\xD8\xB5\xD8\xA7\xD8\xB1\xD9\x85") {
                enableSecurity = true;
                strictSecurity = true;
            } else if (a == "--debug") {
                enableDebug = true;
            } else if (a == "--vm" || a == "--\xD8\xA2\xD9\x84\xD8\xA9") {
                useVM = true;
            } else if (a == "--vm-trace" || a == "--\xD8\xAA\xD8\xAA\xD8\xA8\xD8\xB9-\xD8\xA2\xD9\x84\xD8\xA9") {
                useVM = true;
                vmTrace = true;
            } else if (a == "--vm-disasm" || a == "--\xD9\x81\xD9\x83-\xD8\xA8\xD8\xA7\xD9\x8A\xD8\xAA\xD9\x83\xD9\x88\xD8\xAF") {
                useVM = true;
                vmDisasm = true;
            } else if (a == "--opt-stats" || a == "-v") {
                showOptStats = true;
            } else if (a == "--debug-server") {
                useDebugServer = true;
            } else if (a == "--profile" || a == "--\xD8\xAA\xD9\x86\xD9\x85\xD9\x8A\xD8\xB7" /* --تنميط */) {
                enableProfile = true;
            } else if (a == "--hot-reload" || a == "--\xD9\x85\xD8\xB1\xD8\xA7\xD9\x82\xD8\xA8\xD8\xA9" /* --مراقبة */) {
                enableHotReload = true;
            } else if (a.rfind("--profile-format=", 0) == 0) {
                profileFormat = a.substr(17);
            } else if (a.rfind("--profile-output=", 0) == 0) {
                profileOutput = a.substr(17);
            } else if (a.rfind("--profile-top=", 0) == 0) {
                try { profileTop = std::stoi(a.substr(14)); } catch(...) {}
            } else if (a[0] != '-') {
                filename = a;
            }
        }
        if (filename.empty()) {
            print_help(argv[0]);
            return 1;
        }
        
        // Read source file
        std::string source = read_file(filename);
        
        // Clear error manager
        Sad::Errors::ErrorManager::getInstance().clear();
        
        // (AR) حفظ كود المصدر لعرض الأسطر في رسائل الخطأ
        // (EN) Store source code for displaying lines in error messages
        Sad::Errors::ErrorManager::getInstance().setSourceCode(source, filename);
        
        // Lexer - create lexer and tokenize
        Sad::Lexer::LexerCore lexer(source);
        
        // Parser - parse directly from lexer
        Sad::Parser::ParserCore parser(lexer);
        auto program = parser.parseProgram();
        
        // Check for parser errors
        if (parser.hasErrors()) {
            parser.printErrors();
            return 1;
        }
        
        // ===================================================================
        // وضع الآلة الافتراضية: ترجمة AST إلى بايت كود وتنفيذه
        // VM Mode: Compile AST to bytecode and execute
        // ===================================================================
        if (useVM) {
            // الترجمة / Compile
            sad::vm::مُترجم_بايت_كود compiler;
            auto chunk = compiler.ترجم(program, filename);
            
            if (compiler.يوجد_أخطاء()) {
                std::cerr << "أخطاء ترجمة البايت كود / Bytecode compilation errors:" << std::endl;
                for (const auto& err : compiler.الأخطاء()) {
                    std::cerr << "  " << err << std::endl;
                }
                return 1;
            }
            
            // فك التجميع (اختياري) / Disassemble (optional)
            if (vmDisasm) {
                sad::vm::مفكك_البايت_كود disasm;
                disasm.فكّك_وحدة(chunk);
                std::cout << "\n═══ بدء التنفيذ / Starting execution ═══\n" << std::endl;
            }
            
            // التنفيذ / Execute
            sad::vm::آلة_افتراضية vm;
            if (vmTrace) vm.عيّن_وضع_التتبع(true);
            
            auto result = vm.نفّذ(chunk);
            
            if (result.الحالة != sad::vm::حالة_التنفيذ::نجاح) {
                std::cerr << result.رسالة_الخطأ << std::endl;
                return 1;
            }
            
            if (enableDebug) {
                std::cout << "\n[آلة افتراضية] عدد التعليمات: " << result.عدد_التعليمات
                          << " | الزمن: " << result.زمن_التنفيذ_مللي << " مللي" << std::endl;
            }
            
            return 0;
        }
        
        // ===================================================================
        // وضع خادم التصحيح: DAP debug server mode
        // Debug Server Mode: JSON protocol over stdin/stdout
        // ===================================================================
        if (useDebugServer) {
            Sad::Interpreter::InterpreterOptions options;
            options.enableDebugMode = false;
            options.printResults = false;
            options.currentFilePath = filename;
            
            Sad::Interpreter::Interpreter interpreter(options);
            
            // (AR) إنشاء خادم التصحيح وربطه بالمفسر
            // (EN) Create debug server and connect to interpreter
            Sad::Debug::DebugServer debugServer;
            debugServer.setInterpreterRefs(
                &interpreter.getVariableManager(),
                &interpreter.getFunctionManager(),
                &interpreter.getScopeManager()
            );
            debugServer.setInterpreter(&interpreter);
            Sad::Debug::DebugServer::setInstance(&debugServer);
            
            // (AR) تشغيل خادم التصحيح (سيقرأ الأوامر في خيط منفصل)
            // (EN) Start debug server (will read commands in separate thread)
            debugServer.run(filename);
            
            // (AR) تنفيذ البرنامج — الخطافات ستوقف التنفيذ عند نقاط التوقف
            // (EN) Execute program — hooks will pause at breakpoints
            auto result = interpreter.execute(program);
            
            // (AR) إرسال حدث الانتهاء
            // (EN) Send terminated event
            debugServer.sendEvent("terminated");
            debugServer.sendEvent("exited", "{\"exitCode\":" + std::to_string(result.success ? 0 : 1) + "}");
            
            return result.success ? 0 : 1;
        }
        
        // ===================================================================
        // الوضع العادي: تنفيذ شجرة AST مباشرة
        // Normal Mode: Tree-walking interpreter
        // ===================================================================
        
        // Interpreter - create and execute
        Sad::Interpreter::InterpreterOptions options;
        options.enableDebugMode = enableDebug || showOptStats;
        options.printResults = false;
        options.enableOwnership = enableOwnership;
        options.ownershipArabicMessages = true;
        options.ownershipDebugMode = debugOwnership;
        options.enableTypeCheck = enableTypeCheck;
        options.typeCheckDebugMode = debugTypeCheck;
        options.typeCheckStrictMode = strictTypeCheck;
        options.enableSecurity = enableSecurity;
        options.securityStrictMode = strictSecurity;
        options.securityDebugMode = debugSecurity;
        options.currentFilePath = filename;  // (AR) مسار الملف الحالي لنظام الاستيراد / (EN) Current file path for import system
        options.enableHotReload = enableHotReload;
        
        Sad::Interpreter::Interpreter interpreter(options);
        
        // ═══════════════════════════════════════════════════════════════
        // (AR) إعداد مصحح الأداء إذا طُلب التنميط
        // (EN) Setup profiler if profiling was requested
        // ═══════════════════════════════════════════════════════════════
        std::unique_ptr<Sad::Tools::ProfilerCore> profiler;
        if (enableProfile) {
            Sad::Tools::ProfilerOptions profOpts;
            profOpts.topFunctionsCount = profileTop;
            profOpts.colorOutput = true;
            profOpts.trackCallTree = true;
            profOpts.trackMemory = true;
            if (profileFormat == "json") profOpts.format = Sad::Tools::ReportFormat::JSON;
            else if (profileFormat == "html") profOpts.format = Sad::Tools::ReportFormat::HTML;
            else if (profileFormat == "flame") profOpts.format = Sad::Tools::ReportFormat::FLAMEGRAPH;
            else if (profileFormat == "csv") profOpts.format = Sad::Tools::ReportFormat::CSV;
            else profOpts.format = Sad::Tools::ReportFormat::TEXT;
            if (!profileOutput.empty()) profOpts.outputFile = profileOutput;
            
            profiler = std::make_unique<Sad::Tools::ProfilerCore>(profOpts);
            Sad::Tools::setGlobalProfiler(profiler.get());
            profiler->start();
        }
        
        // Execute program
        auto result = interpreter.execute(program);
        
        // ═══════════════════════════════════════════════════════════════
        // (AR) وضع إعادة التحميل الساخن: مراقبة الملف وإعادة التنفيذ
        // (EN) Hot Reload Mode: watch file and re-execute on changes
        // ═══════════════════════════════════════════════════════════════
        if (enableHotReload && result.success) {
            Sad::HotReload::HotReloadOptions hrOpts;
            hrOpts.preserveState = true;
            hrOpts.showNotifications = true;
            
            Sad::HotReload::HotReloadEngine hotEngine(hrOpts);
            hotEngine.attach(&interpreter, filename);
            
            std::cout << "\033[36m🔥 وضع المراقبة نشط — اضغط Ctrl+C للخروج\033[0m\n";
            std::cout << "\033[36m🔥 Hot Reload active — press Ctrl+C to exit\033[0m\n";
            
            hotEngine.start();
            
            // (AR) حلقة انتظار — المراقبة تعمل في خيط منفصل
            // (EN) Wait loop — file watching runs in background thread
            while (true) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        
        // ═══════════════════════════════════════════════════════════════
        // (AR) إيقاف المصحح وتوليد التقرير
        // (EN) Stop profiler and generate report
        // ═══════════════════════════════════════════════════════════════
        if (enableProfile && profiler) {
            profiler->stop();
            std::string report = profiler->generateReport(profiler->getOptions().format);
            if (!profileOutput.empty()) {
                std::ofstream outFile(profileOutput);
                if (outFile.is_open()) {
                    outFile << report;
                    std::cerr << "\n\033[32m✅ تم حفظ تقرير الأداء في: " << profileOutput << "\033[0m\n";
                } else {
                    std::cerr << "\033[91m❌ فشل فتح ملف الإخراج: " << profileOutput << "\033[0m\n";
                    std::cout << report;
                }
            } else {
                std::cout << report;
            }
            Sad::Tools::setGlobalProfiler(nullptr);
        }
        
        if (!result.success) {
            // Print enriched diagnostics from ErrorManager
            if (Sad::Errors::ErrorManager::getInstance().hasErrors()) {
                Sad::Errors::ErrorManager::getInstance().printAll();
            } else {
                // (AR) عرض خطأ وقت التشغيل مع سطر الكود المصدري
                // (EN) Display runtime error with source code line
                std::cerr << "\033[91m\033[1m❌ خطأ في التنفيذ / Runtime Error:\033[0m " << result.errorMessage << std::endl;
                
                // (AR) محاولة استخراج رقم السطر من رسالة الخطأ (at LINE:COL)
                // (EN) Try to extract line number from error message (at LINE:COL)
                std::string msg = result.errorMessage;
                size_t atPos = msg.rfind("at ");
                if (atPos != std::string::npos) {
                    std::string posStr = msg.substr(atPos + 3);
                    size_t colonPos = posStr.find(':');
                    if (colonPos != std::string::npos) {
                        try {
                            int lineNum = std::stoi(posStr.substr(0, colonPos));
                            int colNum = std::stoi(posStr.substr(colonPos + 1));
                            const std::string& src = Sad::Errors::ErrorManager::getInstance().getSourceCode();
                            if (!src.empty() && lineNum > 0) {
                                // (AR) استخراج السطر من الكود المصدري
                                // (EN) Extract line from source code
                                std::istringstream srcStream(src);
                                std::string srcLine;
                                int currentLine = 0;
                                while (std::getline(srcStream, srcLine)) {
                                    currentLine++;
                                    if (currentLine == lineNum) {
                                        std::cerr << "\033[36m" << "  " << lineNum << " │ " << srcLine << "\033[0m" << std::endl;
                                        std::cerr << "  " << std::string(std::to_string(lineNum).length(), ' ') << " │ "
                                                  << std::string(colNum > 0 ? colNum - 1 : 0, ' ') << "\033[91m\033[1m^ هنا\033[0m" << std::endl;
                                        break;
                                    }
                                }
                            }
                        } catch(...) {}
                    }
                }
            }
            return 1;
        }
        
        return 0;
        
    } catch (const Sad::Interpreter::ExitException& exitEx) {
        // (AR) خروج نظيف من البرنامج
        // (EN) Clean program exit
        return exitEx.getExitCode();
    } catch (const std::exception& e) {
        std::cerr << "\033[91m\033[1m❌ خطأ / Error:\033[0m " << e.what() << std::endl;
        return 1;
    }
}
