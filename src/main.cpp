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
#include "../include/parser/ast/ast_node.h"
#include "../include/interpreter/core/interpreter_core.h"
#include "../include/errors/error_manager.h"  // (AR) نظام إدارة الأخطاء / (EN) Error management system

// LLVM Pipeline Integration / تكامل خط أنابيب LLVM
// ✅ ENABLED: LLVM Pipeline is now ready (Phase 1.1.4 complete - SIRBuilder fixed)
// Source: compiler/pipeline/llvm/include/llvm_compiler_pipeline.h:32-36
#include "../compiler/pipeline/llvm/include/llvm_compiler_pipeline.h"
#include "../compiler/include/compiler_options.h"  // CompilerOptions definition

// تعريف معلومات الإصدار / Version information
#define SAD_VERSION_MAJOR 1
#define SAD_VERSION_MINOR 0
#define SAD_VERSION_PATCH 0
#define SAD_VERSION_STRING "1.0.0"

// ======================================================================
// النطاق / Namespace
// ======================================================================
using namespace Sad::Utils;
using namespace Sad::Parser;
using namespace Sad::AST;

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
    std::cout << "  --llvm, -c <file.s>           ترجمة باستخدام LLVM (سريع)" << std::endl;
    std::cout << "                                Compile using LLVM (fast)" << std::endl;
    std::cout << "  --interpret, -i <file.s>      تنفيذ باستخدام المفسر (افتراضي)" << std::endl;
    std::cout << "                                Interpret using VM (default)" << std::endl;
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
// دالة ترجمة البرنامج باستخدام LLVM / Compile Program using LLVM
// ======================================================================
/**
 * @brief ترجمة البرنامج إلى executable باستخدام LLVM Pipeline
 *        Compile program to executable using LLVM Pipeline
 * 
 * Source: llvm_compiler_pipeline.h:195-207
 * - LLVMCompilerPipeline() constructor
 * - compileFile(const std::string& sourceFile) method
 * - CompilationResult structure (lines 114-141)
 * 
 * @param filename اسم الملف / Filename
 * @return int كود الخروج (0 = نجاح، غير 0 = خطأ)
 *             Exit code (0 = success, non-zero = error)
 */
int compileLLVM(const std::string& filename) {
    DEBUG_PRINT("LLVM compilation started: " + filename);
    
    try {
        // ✅ STEP 1: Create LLVM Compiler Pipeline
        //    Source: llvm_compiler_pipeline.h:195 - LLVMCompilerPipeline()
        //    Source: compiler_options.h:59-124 - CompilerOptions members
        Sad::Compiler::CompilerOptions options;
        options.optimization_level = sad::OptimizationLevel::O2;  // compiler_options.h:67
        options.emit_debug_info = false;  // compiler_options.h:124
        
        Sad::Compiler::LLVM::LLVMCompilerPipeline pipeline(options);
        
        // ✅ STEP 2: Initialize Pipeline
        //    Source: llvm_compiler_pipeline.h:201 - initialize()
        if (!pipeline.initialize()) {
            std::cerr << "❌ Error: Failed to initialize LLVM pipeline" << std::endl;
            return 1;
        }
        
        // ✅ STEP 3: Compile File
        //    Source: llvm_compiler_pipeline.h:207 - compileFile()
        auto result = pipeline.compileFile(filename);
        
        // ✅ STEP 4: Check Result
        //    Source: llvm_compiler_pipeline.h:114-141 - CompilationResult
        if (!result.success) {
            std::cerr << "❌ Compilation Failed: " << result.errorMessage << std::endl;
            for (const auto& error : result.errors) {
                std::cerr << "  • " << error << std::endl;
            }
            return 1;
        }
        
        // ✅ STEP 5: Emit Executable
        //    Source: llvm_compiler_pipeline.h:230 - emitExecutable()
        std::string outputFile = filename.substr(0, filename.find_last_of('.')) + ".exe";
        if (!pipeline.emitExecutable(outputFile)) {
            std::cerr << "❌ Error: Failed to emit executable" << std::endl;
            return 1;
        }
        
        std::cout << "✅ Compilation successful: " << outputFile << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "❌ Exception: " << e.what() << std::endl;
        return 1;
    }
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
        StmtList ast;
        try {
            Sad::Parser::ParserCore parser(lexer);
            ast = parser.parseProgram();
            
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
        } catch (const std::exception& parseEx) {
            std::cerr << std::endl;
            std::cerr << "========================================" << std::endl;
            std::cerr << "❌ خطأ حرج في التحليل النحوي" << std::endl;
            std::cerr << "❌ Critical parsing error" << std::endl;
            std::cerr << "========================================" << std::endl;
            std::cerr << "الرسالة: " << parseEx.what() << std::endl;
            std::cerr << "Message: " << parseEx.what() << std::endl;
            std::cerr << "========================================" << std::endl;
            std::cerr << std::endl;
            return 1;
        } catch (...) {
            std::cerr << std::endl;
            std::cerr << "========================================" << std::endl;
            std::cerr << "❌ خطأ غير معروف في التحليل النحوي" << std::endl;
            std::cerr << "❌ Unknown parsing error" << std::endl;
            std::cerr << "========================================" << std::endl;
            std::cerr << std::endl;
            return 1;
        }
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
        
        // (AR) استخراج exit code من قيمة الإرجاع (إذا كانت من main)
        // (EN) Extract exit code from return value (if from main)
        int exitCode = 0;
        if (!result.result.isVoid() && result.result.isInteger()) {
            exitCode = result.result.toInt();
            
            if (exitCode != 0) {
                std::cout << "(AR) رمز الخروج من main: " << exitCode
                         << " / (EN) Exit code from main: " << exitCode << std::endl;
            }
        }
        
        return exitCode;
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
    if (arg == "--version" || arg == "-v"|| arg == "إصدار"|| arg == "اصدار") {
        showVersion();
        return 0;
    }
    
    // خيار --help أو -h
    // Option --help or -h
    if (arg == "--help" || arg == "-h"|| arg == "مساعدة") {
        showHelp();
        return 0;
    }
    
    // خيار --llvm أو -c (Compile with LLVM)
    // Option --llvm or -c
    if (arg == "--llvm" || arg == "-c" || arg == "--compile"|| arg == "--ترجم") {
        if (argc < 3) {
            std::cerr << "خطأ: يجب تحديد ملف للترجمة" << std::endl;
            std::cerr << "Error: File must be specified for compilation" << std::endl;
            return 1;
        }
        
        std::string filename = argv[2];
        
        // التحقق من امتداد الملف
        if (!checkFileExtension(filename)) {
            return 1;
        }
        
        // ترجمة باستخدام LLVM
        // Source: compileLLVM() function defined above
        return compileLLVM(filename);
    }
    
    // معالجة الملف (الوضع الافتراضي: Interpreter)
    // Process file (default mode: Interpreter)
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
        std::cerr << std::endl;
        std::cerr << "========================================" << std::endl;
        std::cerr << "❌ خطأ حرج في التنفيذ / Critical Execution Error" << std::endl;
        std::cerr << "========================================" << std::endl;
        std::cerr << "الرسالة / Message: " << e.what() << std::endl;
        std::cerr << "========================================" << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << std::endl;
        std::cerr << "========================================" << std::endl;
        std::cerr << "❌ خطأ غير معروف / Unknown Error" << std::endl;
        std::cerr << "========================================" << std::endl;
        return 1;
    }
    
    return 0;
}
