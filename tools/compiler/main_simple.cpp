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

// الآلة الافتراضية / Bytecode VM
#include "sad_vm_compiler.h"
#include "sad_vm_executor.h"
#include "sad_vm_debug.h"

// CLI Commands for mobile etc.
#include "cli_commands.hpp"

// UI Pipeline / خط أنابيب الواجهات الرسومية
#include "ui/sad_ui_pipeline.hpp"

#include "../../shared/utils/include/utf8_utils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <clocale>
#include <vector>

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
        std::cout << "sad version 1.0.0\n";
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
    
    // ═══════════════════════════════════════════════════════════════════════
    // أمر الواجهات الرسومية: sad واجهات <ملف> [--منصة=desktop|android|ios|web]
    // UI command: sad ui <file> [--platform=desktop|android|ios|web]
    // ═══════════════════════════════════════════════════════════════════════
    if (arg == "ui" || arg == "\xD9\x88\xD8\xA7\xD8\xAC\xD9\x87\xD8\xA7\xD8\xAA") {
        if (argc < 3) {
            std::cerr << "الاستخدام / Usage: sad واجهات <ملف.ص> [--منصة=سطح-المكتب]\n";
            std::cerr << "المنصات / Platforms: desktop|android|ios|web|سطح-المكتب|اندرويد|ايفون|ويب\n";
            return 1;
        }
        
        std::string uiFile;
        std::string platform = "desktop";
        bool verbose = false;
        
        for (int i = 2; i < argc; ++i) {
            std::string a = argv[i];
            if (a.find("--platform=") == 0 || a.find("--منصة=") == 0) {
                auto eq = a.find('=');
                if (eq != std::string::npos) platform = a.substr(eq + 1);
            } else if (a == "--desktop" || a == "--سطح-المكتب") {
                platform = "desktop";
            } else if (a == "--android" || a == "--اندرويد") {
                platform = "android";
            } else if (a == "--ios" || a == "--ايفون") {
                platform = "ios";
            } else if (a == "--web" || a == "--ويب") {
                platform = "web";
            } else if (a == "-v" || a == "--verbose") {
                verbose = true;
            } else if (a[0] != '-') {
                uiFile = a;
            }
        }
        
        if (uiFile.empty()) {
            std::cerr << "خطأ: لم يُحدَّد ملف المصدر / Error: No source file specified\n";
            return 1;
        }
        
        // قراءة الملف
        std::string source = read_file(uiFile);
        
        // تحديد المنصة
        sad::ui::ir::TargetPlatform tp = sad::ui::ir::TargetPlatform::Desktop;
        if (platform == "android" || platform == "اندرويد" || platform == "هاتف") {
            tp = sad::ui::ir::TargetPlatform::Android;
        } else if (platform == "ios" || platform == "ايفون") {
            tp = sad::ui::ir::TargetPlatform::iOS;
        } else if (platform == "web" || platform == "ويب") {
            tp = sad::ui::ir::TargetPlatform::Web;
        }
        
        // إعداد خط الأنابيب
        sad::ui::ir::SadUIPipeline pipeline;
        sad::ui::ir::PipelineConfig config;
        config.platform = tp;
        config.validate = true;
        config.optimize = true;
        config.applyTheme = true;
        config.verbose = verbose;
        config.theme.isRTL = true;
        config.theme.isDarkMode = false;
        config.theme.primaryColor = {0, 122, 255, 1.0f};
        config.theme.secondaryColor = {52, 199, 89, 1.0f};
        config.theme.backgroundColor = {255, 255, 255, 1.0f};
        config.theme.surfaceColor = {242, 242, 247, 1.0f};
        config.theme.textPrimary = {0, 0, 0, 1.0f};
        config.theme.textSecondary = {142, 142, 147, 1.0f};
        
        // اسم التطبيق من اسم الملف
        std::filesystem::path fp(uiFile);
        std::string appName = fp.stem().u8string();
        config.outputDir = "build_" + appName;
        
        pipeline.setConfig(config);
        
        auto result = pipeline.buildFromSource(source, appName);
        
        // طباعة السجل
        for (const auto& msg : result.log) {
            std::cout << "  [UI] " << msg << "\n";
        }
        
        if (!result.success) {
            std::cerr << "✗ فشل توليد الواجهات / UI generation failed\n";
            for (const auto& err : result.codeGenResult.errors) {
                std::cerr << "  " << err << "\n";
            }
            return 1;
        }
        
        // كتابة الملفات
        std::filesystem::create_directories(config.outputDir);
        int written = 0;
        for (const auto& f : result.codeGenResult.files) {
            auto path = std::filesystem::path(config.outputDir) / f.path;
            std::filesystem::create_directories(path.parent_path());
            std::ofstream out(path, std::ios::binary);
            if (out.is_open()) {
                out << f.content;
                out.close();
                written++;
                if (verbose) std::cout << "  ✓ " << f.path << "\n";
            }
        }
        
        std::cout << "\n✓ تم توليد " << written << " ملف في " << config.outputDir << "\n";
        if (!result.codeGenResult.buildCommand.empty()) {
            std::cout << "  أمر البناء:\n    " << result.codeGenResult.buildCommand << "\n";
        }
        
        return 0;
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
        
        Sad::Interpreter::Interpreter interpreter(options);
        
        // Register built-in functions (if available)
        // Sad::Interpreter::registerBuiltinFunctions(interpreter);
        
        // Execute program
        auto result = interpreter.execute(program);
        
        if (!result.success) {
            // Print enriched diagnostics from ErrorManager
            if (Sad::Errors::ErrorManager::getInstance().hasErrors()) {
                Sad::Errors::ErrorManager::getInstance().printAll();
            } else {
                std::cerr << "خطأ في التنفيذ / Runtime Error: " << result.errorMessage << std::endl;
            }
            return 1;
        }
        
        return 0;
        
    } catch (const Sad::Interpreter::ExitException& exitEx) {
        // (AR) خروج نظيف من البرنامج
        // (EN) Clean program exit
        return exitEx.getExitCode();
    } catch (const std::exception& e) {
        std::cerr << "خطأ / Error: " << e.what() << std::endl;
        return 1;
    }
}
