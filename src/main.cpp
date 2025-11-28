// ======================================================================
// main.cpp - نقطة الدخول الرئيسية للغة ص / Main Entry Point for Sad Language
// ======================================================================
// الوصف بالعربية:
//   نقطة الدخول الرئيسية لمفسر لغة ص البرمجية
//   يستقبل ملف البرنامج (.s) ويقوم بتنفيذه
//
// English Description:
//   Main entry point for Sad language interpreter
//   Receives program file (.s) and executes it
//
// الاستخدام / Usage:
//   sad <filename.s>              - تنفيذ ملف برنامج / Execute a program file
//   sad --version                 - عرض معلومات الإصدار / Show version info
//   sad --help                    - عرض المساعدة / Show help
//
// الملاحظات / Notes:
//   - امتداد الملفات يجب أن يكون .s
//     File extension must be .s
//   - الملفات يجب أن تكون UTF-8 مع BOM
//     Files must be UTF-8 encoded with BOM
// ======================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include "../include/utils/string_utils.h"
#include "../include/lexer/lexer_core.h"
#include "../include/parser/parser_core.h"
#include "../include/interpreter/core/interpreter_core.h"
#include "../include/errors/error_manager.h"  // (AR) نظام إدارة الأخطاء / (EN) Error management system

// تعريف معلومات الإصدار / Version information
#define SAD_VERSION_MAJOR 1
#define SAD_VERSION_MINOR 0
#define SAD_VERSION_PATCH 0
#define SAD_VERSION_STRING "1.0.0"

// ======================================================================
// النطاق / Namespace
// ======================================================================
using namespace Sad::Utils;

// ======================================================================
// دالة عرض معلومات الإصدار / Show Version Info
// ======================================================================
/**
 * @brief عرض معلومات الإصدار والبناء
 *        Display version and build information
 * 
 * @note تعرض:
 *       - رقم الإصدار / Version number
 *       - معلومات المترجم / Compiler info
 *       - تاريخ البناء / Build date
 */
void showVersion() {
    DEBUG_PRINT("عرض معلومات الإصدار");
    
    std::cout << "========================================" << std::endl;
    std::cout << "لغة ص - Sad Programming Language" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "الإصدار / Version: " << SAD_VERSION_STRING << std::endl;
    
    #ifdef _MSC_VER
        std::cout << "المترجم / Compiler: MSVC " << _MSC_VER << std::endl;
    #elif defined(__GNUC__)
        std::cout << "المترجم / Compiler: GCC " << __GNUC__ << "." << __GNUC_MINOR__ << std::endl;
    #elif defined(__clang__)
        std::cout << "المترجم / Compiler: Clang " << __clang_major__ << "." << __clang_minor__ << std::endl;
    #endif
    
    std::cout << "تاريخ البناء / Build Date: " << __DATE__ << " " << __TIME__ << std::endl;
    
    #ifdef SAD_DEBUG_ENABLED
        std::cout << "وضع التنقيح / Debug Mode: مفعّل / Enabled" << std::endl;
    #else
        std::cout << "وضع التنقيح / Debug Mode: معطّل / Disabled" << std::endl;
    #endif
    
    std::cout << "========================================" << std::endl;
    std::cout << "صُنع بـ ❤️ للمطورين العرب" << std::endl;
    std::cout << "Made with ❤️ for Arabic developers" << std::endl;
    std::cout << "========================================" << std::endl;
}

// ======================================================================
// دالة عرض المساعدة / Show Help
// ======================================================================
/**
 * @brief عرض رسالة المساعدة مع الأوامر المتاحة
 *        Display help message with available commands
 * 
 * @note تعرض:
 *       - كيفية الاستخدام / Usage instructions
 *       - الخيارات المتاحة / Available options
 *       - أمثلة / Examples
 */
void showHelp() {
    DEBUG_PRINT("عرض رسالة المساعدة");
    
    std::cout << "========================================" << std::endl;
    std::cout << "مساعدة لغة ص / Sad Language Help" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "الاستخدام / Usage:" << std::endl;
    std::cout << "  sad <filename.s>              تنفيذ ملف برنامج" << std::endl;
    std::cout << "                                Execute a program file" << std::endl;
    std::cout << std::endl;
    
    std::cout << "الخيارات / Options:" << std::endl;
    std::cout << "  --version, -v                 عرض معلومات الإصدار" << std::endl;
    std::cout << "                                Show version information" << std::endl;
    std::cout << "  --help, -h                    عرض هذه المساعدة" << std::endl;
    std::cout << "                                Show this help message" << std::endl;
    std::cout << std::endl;
    
    std::cout << "أمثلة / Examples:" << std::endl;
    std::cout << "  sad program.s                 تنفيذ برنامج.s" << std::endl;
    std::cout << "                                Execute program.s" << std::endl;
    std::cout << "  sad examples/hello.s          تنفيذ مثال مرحبا" << std::endl;
    std::cout << "                                Execute hello example" << std::endl;
    std::cout << std::endl;
    
    std::cout << "المتطلبات / Requirements:" << std::endl;
    std::cout << "  - امتداد الملف يجب أن يكون .s" << std::endl;
    std::cout << "    File extension must be .s" << std::endl;
    std::cout << "  - الملفات يجب أن تكون UTF-8 مع BOM" << std::endl;
    std::cout << "    Files must be UTF-8 encoded with BOM" << std::endl;
    std::cout << std::endl;
    
    std::cout << "للمزيد من المعلومات، زر:" << std::endl;
    std::cout << "For more information, visit:" << std::endl;
    std::cout << "  https://sadlang.org/docs" << std::endl;
    std::cout << "========================================" << std::endl;
}

// ======================================================================
// دالة التحقق من امتداد الملف / Check File Extension
// ======================================================================
/**
 * @brief التحقق من أن الملف له الامتداد الصحيح (.s)
 *        Check if file has correct extension (.s)
 * 
 * @param filename اسم الملف / Filename to check
 * @return bool true إذا كان الامتداد صحيح / true if extension is correct
 * 
 * @throws لا يرمي استثناءات / Does not throw exceptions
 * 
 * @example
 *   bool valid = checkFileExtension("program.s");  // true
 *   bool invalid = checkFileExtension("program.txt");  // false
 */
bool checkFileExtension(const std::string& filename) {
    DEBUG_PRINT("فحص امتداد الملف: " + filename);
    
    // التحقق من أن الملف ينتهي بـ .s
    // Check if file ends with .s
    bool isValid = StringUtils::endsWith(filename, ".s");
    
    if (!isValid) {
        std::cerr << "خطأ: الملف يجب أن ينتهي بـ .s" << std::endl;
        std::cerr << "Error: File must have .s extension" << std::endl;
        DEBUG_PRINT("امتداد الملف غير صحيح");
    }
    
    return isValid;
}

// ======================================================================
// دالة قراءة ملف البرنامج / Read Program File
// ======================================================================
/**
 * @brief قراءة محتوى ملف البرنامج
 *        Read program file contents
 * 
 * @param filename اسم الملف المراد قراءته / Filename to read
 * @param content[out] محتوى الملف / File contents (output parameter)
 * @return bool true إذا تمت القراءة بنجاح / true if read successfully
 * 
 * @throws لا يرمي استثناءات / Does not throw exceptions
 * 
 * @example
 *   std::string code;
 *   if (readProgramFile("test.s", code)) {
 *       // معالجة الكود / Process code
 *   }
 * 
 * @note يتحقق من:
 *       - وجود الملف / File existence
 *       - إمكانية القراءة / Read permissions
 */
bool readProgramFile(const std::string& filename, std::string& content) {
    DEBUG_PRINT("قراءة ملف البرنامج: " + filename);
    
    // فتح الملف للقراءة
    // Open file for reading
    std::ifstream file(filename, std::ios::binary);
    
    // التحقق من نجاح فتح الملف
    // Check if file opened successfully
    if (!file.is_open()) {
        std::cerr << "خطأ: لا يمكن فتح الملف: " << filename << std::endl;
        std::cerr << "Error: Cannot open file: " << filename << std::endl;
        DEBUG_PRINT("فشل فتح الملف");
        return false;
    }
    
    // قراءة محتوى الملف بالكامل
    // Read entire file contents
    std::stringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    
    file.close();
    
    DEBUG_PRINT("تمت قراءة " + std::to_string(content.length()) + " حرف من الملف");
    
    // التحقق من أن الملف ليس فارغاً
    // Check if file is not empty
    if (content.empty()) {
        std::cerr << "تحذير: الملف فارغ" << std::endl;
        std::cerr << "Warning: File is empty" << std::endl;
        DEBUG_PRINT("الملف فارغ");
        return false;
    }
    
    return true;
}

// ======================================================================
// دالة تنفيذ البرنامج / Execute Program
// ======================================================================
/**
 * @brief تنفيذ كود البرنامج
 *        Execute program code
 * 
 * @param filename اسم الملف / Filename
 * @param code كود البرنامج / Program code
 * @return int كود الخروج (0 = نجاح، غير 0 = خطأ)
 *             Exit code (0 = success, non-zero = error)
 */
int executeProgram(const std::string& filename, const std::string& code) {
    DEBUG_PRINT("بدء تنفيذ البرنامج: " + filename);
    
    // (AR) تنظيف الأخطاء السابقة / (EN) Clear previous errors
    Sad::Errors::ErrorManager::getInstance().clear();
    
    try {
        std::cout << "========================================" << std::endl;
        std::cout << "تنفيذ البرنامج / Executing Program" << std::endl;
        std::cout << "الملف / File: " << filename << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;
        
        // الخطوة 1: التحليل المعجمي / Step 1: Lexical Analysis
        std::cout << "[1/4] التحليل المعجمي / Lexical Analysis..." << std::endl;
        Sad::Lexer::LexerCore lexer(code);
        std::cout << "      تم إنشاف محلل معجمي" << std::endl;
        std::cout << "      Lexer created" << std::endl;
        std::cout << std::endl;
        
        // الخطوة 2: التحليل النحوي / Step 2: Syntactic Analysis
        std::cout << "[2/4] التحليل النحوي / Syntactic Analysis..." << std::endl;
        Sad::Parser::ParserCore parser(lexer);
        auto ast = parser.parseProgram();
        
        // (AR) فحص الأخطاء بعد التحليل النحوي / (EN) Check for parsing errors
        if (Sad::Errors::ErrorManager::getInstance().hasErrors()) {
            std::cout << std::endl;
            std::cout << "========================================" << std::endl;
            std::cout << "❌ فشل التحليل النحوي / Parsing Failed" << std::endl;
            std::cout << "========================================" << std::endl;
            std::cout << std::endl;
            
            // (AR) طباعة جميع الأخطاء بشكل جميل / (EN) Print all errors beautifully
            Sad::Errors::ErrorManager::getInstance().printAll();
            
            return 1;
        }
        
        std::cout << "      تم بناء شجرة AST" << std::endl;
        std::cout << "      AST built" << std::endl;
        std::cout << std::endl;
        std::cout << "      عدد العقد في شجرة AST: " << ast.size() << std::endl;
        std::cout << "      عدد جمل البرنامج: " << ast.size() << std::endl;
       
        std::cout <<"ast print content :"<<std::endl;
        for (const auto& stmt : ast) {
            std::cout << stmt->toString() << std::endl;
        }
        // الخطوة 3: إعداد المفسر / Step 3: Interpreter Setup
        std::cout << "[3/4] إعداد المفسر / Interpreter Setup..." << std::endl;
        Sad::Interpreter::Interpreter interpreter;
        std::cout << "      تم إنشاء المفسر" << std::endl;
        std::cout << "      Interpreter created" << std::endl;
        std::cout << std::endl;
        
        // الخطوة 4: التنفيذ / Step 4: Execution
        std::cout << "[4/4] التنفيذ / Execution..." << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;
        
        auto result = interpreter.execute(ast);
        
        // (AR) فحص الأخطاء التنفيذية / (EN) Check for runtime errors
        if (!result.success || Sad::Errors::ErrorManager::getInstance().hasErrors()) {
            std::cout << std::endl;
            std::cout << "========================================" << std::endl;
            std::cout << "❌ فشل التنفيذ / Execution Failed" << std::endl;
            std::cout << "========================================" << std::endl;
            std::cout << std::endl;
            
            // Print error message from result if available
            if (!result.success && !result.errorMessage.empty()) {
                std::cerr << result.errorMessage << std::endl;
            }
            
            // (AR) طباعة جميع الأخطاء بشكل جميل / (EN) Print all errors beautifully
            if (Sad::Errors::ErrorManager::getInstance().hasErrors()) {
                Sad::Errors::ErrorManager::getInstance().printAll();
            }
            
            return 1;
        }
        
        std::cout << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "✅ اكتمل التنفيذ بنجاح / Execution completed" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;
        
        DEBUG_PRINT("انتهى تنفيذ البرنامج بنجاح");
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << std::endl;
        std::cerr << "========================================" << std::endl;
        std::cerr << "❌ خطأ في التنفيذ / Execution Error" << std::endl;
        std::cerr << "========================================" << std::endl;
        std::cerr << "الرسالة / Message: " << e.what() << std::endl;
        std::cerr << std::endl;
        
        DEBUG_PRINT("خطأ: " + std::string(e.what()));
        
        return 1;
    }
}

// ======================================================================
// الدالة الرئيسية / Main Function
// ======================================================================
/**
 * @brief نقطة الدخول الرئيسية للبرنامج
 *        Main entry point of the program
 * 
 * @param argc عدد المعاملات / Number of arguments
 * @param argv قائمة المعاملات / List of arguments
 * @return int كود الخروج / Exit code
 *             0 = نجاح / success
 *             1 = خطأ / error
 * 
 * @example
 *   sad program.s        // تنفيذ برنامج
 *   sad --version        // عرض الإصدار
 *   sad --help           // عرض المساعدة
 */
int main(int argc, char* argv[]) {
    DEBUG_PRINT("بدء برنامج لغة ص");
    
    // ضبط الترميز لدعم UTF-8 على Windows
    // Set encoding for UTF-8 support on Windows
    #ifdef _WIN32
        // تفعيل دعم UTF-8 في الكونسول
        // Enable UTF-8 support in console
        system("chcp 65001 > nul");
    #endif
    
    // إذا لم يتم تمرير أي معاملات، عرض المساعدة
    // If no arguments provided, show help
    if (argc < 2) {
        std::cerr << "خطأ: لم يتم تحديد ملف للتنفيذ" << std::endl;
        std::cerr << "Error: No file specified for execution" << std::endl;
        std::cerr << std::endl;
        showHelp();
        return 1;
    }
    
    // معالجة الخيارات
    // Process options
    std::string arg = argv[1];
    
    // خيار --version أو -v
    // Option --version or -v
    if (arg == "--version" || arg == "-v") {
        showVersion();
        return 0;
    }
    
    // خيار --help أو -h
    // Option --help or -h
    if (arg == "--help" || arg == "-h") {
        showHelp();
        return 0;
    }
    
    // معالجة الملف
    // Process file
    std::string filename = arg;
    // التحقق من امتداد الملف
    // Check file extension
    if (!checkFileExtension(filename)) {
        return 1;
    }
    
    // قراءة محتوى الملف
    // Read file contents
    std::string code;
    if (!readProgramFile(filename, code)) {
        return 1;
    }
    
    // تنفيذ البرنامج
    // Execute program
    try {
        int exitCode = executeProgram(filename, code);
        
        if (exitCode == 0) {
            std::cout << "========================================" << std::endl;
            std::cout << "تم التنفيذ بنجاح / Execution Successful" << std::endl;
            std::cout << "========================================" << std::endl;
        }
        
        return exitCode;
    }
    catch (const std::exception& e) {
        std::cerr << "========================================" << std::endl;
        std::cerr << "خطأ في التنفيذ / Execution Error" << std::endl;
        std::cerr << "========================================" << std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << "========================================" << std::endl;
        return 1;
    }
    
    return 0;
}
