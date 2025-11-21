/**
 * @file test_type_system.cpp
 * @brief اختبارات نظام الأنواع / Type System Tests
 * 
 * (AR) يحتوي على اختبارات لتحليل تصريحات الأنواع في المتغيرات والدوال.
 * (EN) Contains tests for parsing type declarations in variables and functions.
 */

#include <iostream>
#include <string>
#include <memory>
#include <cassert>

// Include parser headers
#include "parser/parser_core.h"
#include "lexer/lexer_core.h"
#include "data/types/data_types.h"

using namespace Sad;
using namespace Sad::Parser;
using namespace Sad::Lexer;
using namespace Sad::Data;

// =========================================================================
// Test Helper Functions
// =========================================================================

/**
 * @brief (AR) يختبر تحليل كود ويعيد AST.
 *        (EN) Tests parsing code and returns AST.
 */
AST::StmtList parseCode(const std::string& code) {
    try {
        LexerCore lexer(code);
        ParserCore parser(lexer);
        auto result = parser.parseProgram();
        
        // Check for parser errors
        if (parser.hasErrors()) {
            std::cerr << "Parser errors detected in code: " << code << std::endl;
        }
        
        return result;
    } catch (const std::exception& e) {
        std::cerr << "Exception in parseCode: " << e.what() << std::endl;
        std::cerr << "Code: " << code << std::endl;
        throw;
    }
}

/**
 * @brief (AR) يطبع نتيجة اختبار.
 *        (EN) Prints test result.
 */
void printTestResult(const std::string& testName, bool passed) {
    if (passed) {
        std::cout << "  Running: " << testName << " ... ✓ PASSED\n";
    } else {
        std::cout << "  Running: " << testName << " ... ✗ FAILED\n";
    }
}

// =========================================================================
// TypedVariableTests / اختبارات المتغيرات المكتوبة
// =========================================================================

namespace TypedVariableTests {

/**
 * @brief (AR) اختبار متغير بنوع صريح بالإنجليزية.
 *        (EN) Test variable with explicit English type.
 */
bool testEnglishTypedVariable() {
    std::string code = "var x : int = 10;";
    
    try {
        auto ast = parseCode(code);
        
        // Should have 1 statement
        if (ast.size() != 1) return false;
        
        // Should be VarDeclStmt
        auto* varDecl = dynamic_cast<AST::VarDeclStmt*>(ast[0].get());
        if (!varDecl) return false;
        
        // Check name
        if (varDecl->name != "x") return false;
        
        // Check type (should be INTEGER)
        if (varDecl->type != DataType::INTEGER) return false;
        
        // Check initializer exists
        if (!varDecl->initializer) return false;
        
        return true;
    } catch (...) {
        return false;
    }
}

/**
 * @brief (AR) اختبار متغير بنوع صريح بالعربية.
 *        (EN) Test variable with explicit Arabic type.
 */
bool testArabicTypedVariable() {
    std::string code = "var ع : رقم = 20;";
    
    try {
        auto ast = parseCode(code);
        
        // Should have 1 statement
        if (ast.size() != 1) return false;
        
        // Should be VarDeclStmt
        auto* varDecl = dynamic_cast<AST::VarDeclStmt*>(ast[0].get());
        if (!varDecl) return false;
        
        // Check name
        if (varDecl->name != "ع") return false;
        
        // Check type (should be INTEGER)
        if (varDecl->type != DataType::INTEGER) return false;
        
        return true;
    } catch (...) {
        return false;
    }
}

/**
 * @brief (AR) اختبار متغير بدون نوع صريح (UNKNOWN).
 *        (EN) Test variable without explicit type (UNKNOWN).
 */
bool testUntypedVariable() {
    std::string code = "var y = 30;";
    
    try {
        auto ast = parseCode(code);
        
        auto* varDecl = dynamic_cast<AST::VarDeclStmt*>(ast[0].get());
        if (!varDecl) return false;
        
        // Type should be UNKNOWN (no type annotation)
        if (varDecl->type != DataType::UNKNOWN) return false;
        
        return true;
    } catch (...) {
        return false;
    }
}

/**
 * @brief (AR) اختبار متغير نصي.
 *        (EN) Test string variable.
 */
bool testStringVariable() {
    std::string code = "var name : string = \"Ahmed\";";
    
    try {
        auto ast = parseCode(code);
        
        auto* varDecl = dynamic_cast<AST::VarDeclStmt*>(ast[0].get());
        if (!varDecl) return false;
        
        // Check type (should be STRING)
        if (varDecl->type != DataType::STRING) return false;
        
        return true;
    } catch (...) {
        return false;
    }
}

/**
 * @brief (AR) اختبار متغير منطقي.
 *        (EN) Test boolean variable.
 */
bool testBooleanVariable() {
    std::string code = "var flag : bool = true;";
    
    try {
        auto ast = parseCode(code);
        
        auto* varDecl = dynamic_cast<AST::VarDeclStmt*>(ast[0].get());
        if (!varDecl) return false;
        
        // Check type (should be BOOLEAN)
        if (varDecl->type != DataType::BOOLEAN) return false;
        
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace TypedVariableTests

// =========================================================================
// TypedFunctionTests / اختبارات الدوال المكتوبة
// =========================================================================

namespace TypedFunctionTests {

/**
 * @brief (AR) اختبار دالة بمعاملات مكتوبة.
 *        (EN) Test function with typed parameters.
 */
bool testTypedParameters() {
    std::string code = "function add(a : int, b : int) { return a + b; }";
    
    try {
        auto ast = parseCode(code);
        
        auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(ast[0].get());
        if (!funcDecl) return false;
        
        // Check function name
        if (funcDecl->name != "add") return false;
        
        // Check parameters count
        if (funcDecl->parameters.size() != 2) return false;
        
        // Check first parameter
        if (funcDecl->parameters[0].name != "a") return false;
        if (funcDecl->parameters[0].type != DataType::INTEGER) return false;
        
        // Check second parameter
        if (funcDecl->parameters[1].name != "b") return false;
        if (funcDecl->parameters[1].type != DataType::INTEGER) return false;
        
        return true;
    } catch (...) {
        return false;
    }
}

/**
 * @brief (AR) اختبار دالة بنوع إرجاع صريح.
 *        (EN) Test function with explicit return type.
 */
bool testReturnType() {
    std::string code = "function square(x : int) : int { return x * x; }";
    
    try {
        auto ast = parseCode(code);
        
        auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(ast[0].get());
        if (!funcDecl) return false;
        
        // Check return type
        if (funcDecl->returnType != DataType::INTEGER) return false;
        
        return true;
    } catch (...) {
        return false;
    }
}

/**
 * @brief (AR) اختبار دالة بمعاملات مختلطة (مكتوبة وغير مكتوبة).
 *        (EN) Test function with mixed parameters (typed and untyped).
 */
bool testMixedParameters() {
    std::string code = "function test(a, b : int, c : float) { }";
    
    try {
        auto ast = parseCode(code);
        
        auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(ast[0].get());
        if (!funcDecl) return false;
        
        // Check parameters count
        if (funcDecl->parameters.size() != 3) return false;
        
        // First parameter: no type (UNKNOWN)
        if (funcDecl->parameters[0].type != DataType::UNKNOWN) return false;
        
        // Second parameter: int
        if (funcDecl->parameters[1].type != DataType::INTEGER) return false;
        
        // Third parameter: float
        if (funcDecl->parameters[2].type != DataType::FLOAT) return false;
        
        return true;
    } catch (...) {
        return false;
    }
}

/**
 * @brief (AR) اختبار دالة عربية بالكامل.
 *        (EN) Test fully Arabic function.
 */
bool testArabicFunction() {
    std::string code = "function ضرب(س : رقم، ص : رقم) : رقم { return س * ص; }";
    
    try {
        auto ast = parseCode(code);
        
        auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(ast[0].get());
        if (!funcDecl) return false;
        
        // Check function name
        if (funcDecl->name != "ضرب") return false;
        
        // Check parameters
        if (funcDecl->parameters.size() != 2) return false;
        if (funcDecl->parameters[0].name != "س") return false;
        if (funcDecl->parameters[0].type != DataType::INTEGER) return false;
        
        // Check return type
        if (funcDecl->returnType != DataType::INTEGER) return false;
        
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace TypedFunctionTests

// =========================================================================
// Main Test Runner
// =========================================================================

int main() {
    try {
        std::cout << "\n";
        std::cout << "═══════════════════════════════════════════════════════════\n";
        std::cout << "║  Type System Tests / اختبارات نظام الأنواع           ║\n";
        std::cout << "═══════════════════════════════════════════════════════════\n\n";
        
        int totalTests = 0;
        int passedTests = 0;
        
        // TypedVariableTests
        std::cout << "─────────────────────────────────────────────────────────\n";
        std::cout << "  Test Group: TypedVariableTests\n";
        std::cout << "─────────────────────────────────────────────────────────\n";
        
        bool test1 = TypedVariableTests::testEnglishTypedVariable();
        printTestResult("TypedVariableTests.EnglishTypedVariable", test1);
        totalTests++; if (test1) passedTests++;
    
    bool test2 = TypedVariableTests::testArabicTypedVariable();
    printTestResult("TypedVariableTests.ArabicTypedVariable", test2);
    totalTests++; if (test2) passedTests++;
    
    bool test3 = TypedVariableTests::testUntypedVariable();
    printTestResult("TypedVariableTests.UntypedVariable", test3);
    totalTests++; if (test3) passedTests++;
    
    bool test4 = TypedVariableTests::testStringVariable();
    printTestResult("TypedVariableTests.StringVariable", test4);
    totalTests++; if (test4) passedTests++;
    
    bool test5 = TypedVariableTests::testBooleanVariable();
    printTestResult("TypedVariableTests.BooleanVariable", test5);
    totalTests++; if (test5) passedTests++;
    
    // TypedFunctionTests
    std::cout << "\n─────────────────────────────────────────────────────────\n";
    std::cout << "  Test Group: TypedFunctionTests\n";
    std::cout << "─────────────────────────────────────────────────────────\n";
    
    bool test6 = TypedFunctionTests::testTypedParameters();
    printTestResult("TypedFunctionTests.TypedParameters", test6);
    totalTests++; if (test6) passedTests++;
    
    bool test7 = TypedFunctionTests::testReturnType();
    printTestResult("TypedFunctionTests.ReturnType", test7);
    totalTests++; if (test7) passedTests++;
    
    bool test8 = TypedFunctionTests::testMixedParameters();
    printTestResult("TypedFunctionTests.MixedParameters", test8);
    totalTests++; if (test8) passedTests++;
    
    bool test9 = TypedFunctionTests::testArabicFunction();
    printTestResult("TypedFunctionTests.ArabicFunction", test9);
    totalTests++; if (test9) passedTests++;
    
    // Summary
    std::cout << "\n========================================\n";
    std::cout << "Test Results / نتائج الاختبارات\n";
    std::cout << "========================================\n\n";
    std::cout << "Total Tests:   " << totalTests << "\n";
    std::cout << "Passed:        " << passedTests << "\n";
    std::cout << "Failed:        " << (totalTests - passedTests) << "\n\n";
    
    if (passedTests == totalTests) {
        std::cout << "✓ All tests passed! / جميع الاختبارات نجحت!\n\n";
        return 0;
    } else {
        std::cout << "✗ Some tests failed! / بعض الاختبارات فشلت!\n\n";
        return 1;
    }
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Fatal error in main: " << e.what() << "\n\n";
        return 2;
    } catch (...) {
        std::cerr << "\n✗ Unknown fatal error in main\n\n";
        return 2;
    }
}
