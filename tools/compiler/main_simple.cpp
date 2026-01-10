// بسم الله الرحمن الرحيم
// sad - Sad Language Interpreter
// مفسر لغة ص البرمجية العربية
// Simple main.cpp that uses the interpreter directly

#include "lexer_core.h"
#include "parser_core.h"
#include "interpreter_core.h"
#include "error_manager.h"
#include "value.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <clocale>

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
              << std::endl;
}

// قراءة ملف
// Read file
std::string read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("لم يتم العثور على الملف: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    // Setup UTF-8 support
    // setup_utf8();  // Disabled - causes issues with normal output
    
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
    
    // Execute file
    try {
        // Read source file
        std::string source = read_file(arg);
        
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
        options.enableDebugMode = false;
        options.printResults = false;
        
        Sad::Interpreter::Interpreter interpreter(options);
        
        // Register built-in functions (if available)
        // Sad::Interpreter::registerBuiltinFunctions(interpreter);
        
        // Execute program
        auto result = interpreter.execute(program);
        
        if (!result.success) {
            std::cerr << "خطأ في التنفيذ / Runtime Error: " << result.errorMessage << std::endl;
            return 1;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "خطأ / Error: " << e.what() << std::endl;
        return 1;
    }
}
