// بسم الله الرحمن الرحيم
// sad - Sad Language Interpreter
// مفسر لغة ص البرمجية العربية
// Simple main.cpp that uses the interpreter directly

#include "lexer_core.h"
#include "parser_core.h"
#include "interpreter_core.h"
#include "error_manager.h"
#include "value.h"

// CLI Commands for mobile etc.
#include "cli_commands.hpp"

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
        
    } catch (const std::exception& e) {
        std::cerr << "خطأ / Error: " << e.what() << std::endl;
        return 1;
    }
}
