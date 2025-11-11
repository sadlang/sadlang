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
#include "../include/utils/string_utils.h"

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
 * 
 * @throws قد يرمي استثناءات من المفسر / May throw interpreter exceptions
 * 
 * @note حالياً يطبع الكود فقط (للاختبار)
 *       Currently only prints code (for testing)
 *       سيتم إضافة المحلل المعجمي والمفسر لاحقاً
 *       Lexer and interpreter will be added later
 */
int executeProgram(const std::string& filename, const std::string& code) {
    DEBUG_PRINT("بدء تنفيذ البرنامج: " + filename);
    
    std::cout << "========================================" << std::endl;
    std::cout << "تنفيذ البرنامج / Executing Program" << std::endl;
    std::cout << "الملف / File: " << filename << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    // TODO: سيتم إضافة المحلل المعجمي هنا
    // TODO: Add lexer here
    // Lexer lexer(code);
    // auto tokens = lexer.tokenize();
    
    // TODO: سيتم إضافة المحلل النحوي هنا
    // TODO: Add parser here
    // Parser parser(tokens);
    // auto ast = parser.parse();
    
    // TODO: سيتم إضافة المفسر هنا
    // TODO: Add interpreter here
    // Interpreter interpreter(ast);
    // interpreter.execute();
    
    // حالياً: طباعة الكود فقط للاختبار
    // Currently: Just print code for testing
    std::cout << "محتوى البرنامج / Program Content:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << code << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << std::endl;
    
    std::cout << "ملاحظة: المفسر قيد التطوير" << std::endl;
    std::cout << "Note: Interpreter is under development" << std::endl;
    std::cout << std::endl;
    
    DEBUG_PRINT("انتهى تنفيذ البرنامج بنجاح");
    
    return 0;
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
