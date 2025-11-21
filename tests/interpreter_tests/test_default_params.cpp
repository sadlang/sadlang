/**
 * @file test_default_params.cpp
 * @brief (AR) اختبار Default Parameters للدوال
 * @brief (EN) Test Default Parameters for Functions
 * 
 * الوصف بالعربية:
 * =================
 * هذا الملف يحتوي على اختبارات شاملة لميزة Default Parameters:
 * - معامل واحد بقيمة افتراضية
 * - معاملات متعددة بقيم افتراضية
 * - مزيج من معاملات إجبارية واختيارية
 * - استدعاء بجميع المعاملات
 * - استدعاء ببعض المعاملات (استخدام القيم الافتراضية)
 * 
 * English Description:
 * ====================
 * This file contains comprehensive tests for Default Parameters feature:
 * - Single parameter with default value
 * - Multiple parameters with defaults
 * - Mix of required and optional parameters
 * - Call with all arguments
 * - Call with some arguments (using defaults)
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 * @version 1.0
 */

#include <iostream>
#include <string>
#include <memory>
#include <cassert>

// AST includes
#include "../../include/parser/ast/expressions.h"
#include "../../include/parser/ast/statements.h"
#include "../../include/parser/ast/declarations.h"

// Interpreter includes
#include "../../include/interpreter/visitors/expression_evaluator.h"
#include "../../include/interpreter/visitors/statement_executor.h"

// Managers
#include "../../include/data/managers/variable_manager.h"
#include "../../include/data/managers/function_manager.h"
#include "../../include/data/scope/scope_manager.h"

// Lexer (for Token/Position)
#include "../../include/lexer/token.h"

using namespace Sad;
using namespace Sad::AST;
using namespace Sad::Data;
using namespace Sad::Interpreter;
using namespace Sad::Lexer;

// =========================================================================
// Test 1: (AR) اختبار أساسي - هل default parameters معرفة؟
//         (EN) Basic test - are default parameters defined?
// =========================================================================
bool test_default_params_structure() {
    std::cout << "\nTest 1: Default Parameters Structure Check\n";
    std::cout << "============================================\n";
    
    try {
        std::cout << "(AR) التحقق من: هل Parameter.defaultValue موجود؟\n";
        std::cout << "(EN) Checking: Does Parameter.defaultValue exist?\n\n";
        
        // Create a parameter with default value
        auto defaultExpr = std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, "42", Position())
        );
        
        Parameter param("x", DataType::INTEGER, std::move(defaultExpr));
        
        if (param.defaultValue) {
            std::cout << "(AR) ✅ Parameter.defaultValue موجود!\n";
            std::cout << "(EN) ✅ Parameter.defaultValue exists!\n\n";
            
            std::cout << "(AR) التفاصيل:\n";
            std::cout << "(EN) Details:\n";
            std::cout << "  - Name: " << param.name << "\n";
            std::cout << "  - Type: " << (int)param.type << "\n";
            std::cout << "  - Has Default: " << (param.defaultValue ? "Yes" : "No") << "\n";
            
            return true;
        } else {
            std::cout << "(AR) ❌ Parameter.defaultValue فارغ!\n";
            std::cout << "(EN) ❌ Parameter.defaultValue is null!\n";
            return false;
        }
        
    } catch (const std::exception& e) {
        std::cout << "(AR) خطأ: " << e.what() << "\n";
        std::cout << "(EN) Error: " << e.what() << "\n";
        return false;
    }
}

// =========================================================================
// Test 2: (AR) اختبار Integer default بسيط
//         (EN) Test simple Integer default
// =========================================================================
bool test_integer_default_simple() {
    std::cout << "\nTest 2: Simple Default Parameter\n";
    std::cout << "==================================\n";
    
    try {
        std::cout << "(AR) اختبار: func greet(name, msg = \"Hello\")\n";
        std::cout << "(EN) Testing: func greet(name, msg = \"Hello\")\n\n";
        
        VariableManager varMgr;
        FunctionManager funcMgr;
        ScopeManager scopeMgr;
        StatementExecutor executor(varMgr, funcMgr, scopeMgr);
        ExpressionEvaluator evaluator(varMgr, funcMgr, scopeMgr, executor);
        
        // Create function: func greet(name, msg = "Hello") { return msg + ", " + name; }
        std::vector<Parameter> params;
        params.emplace_back("name", DataType::STRING);
        
        // Parameter with default
        auto defaultExpr = std::make_unique<LiteralExpr>(
            Token(TokenType::STRING, "Hello", Position())
        );
        params.emplace_back("msg", DataType::STRING, std::move(defaultExpr));
        
        // Body: return msg + ", " + name;
        auto msg = std::make_unique<VariableExpr>("msg");
        auto comma = std::make_unique<LiteralExpr>(
            Token(TokenType::STRING, ", ", Position())
        );
        auto name = std::make_unique<VariableExpr>("name");
        
        auto msgComma = std::make_unique<BinaryExpr>(
            std::move(msg), TokenType::OP_PLUS, std::move(comma)
        );
        auto fullMsg = std::make_unique<BinaryExpr>(
            std::move(msgComma), TokenType::OP_PLUS, std::move(name)
        );
        auto returnStmt = std::make_unique<ReturnStmt>(std::move(fullMsg));
        
        std::vector<StmtPtr> bodyStmts;
        bodyStmts.push_back(std::move(returnStmt));
        auto bodyBlock = std::make_unique<BlockStmt>(std::move(bodyStmts));
        
        // Create FunctionDecl
        auto funcDecl = std::make_unique<FunctionDecl>(
            "greet",
            std::move(params),
            DataType::STRING,
            std::move(bodyBlock)
        );
        
        // Convert to FunctionParameters
        std::vector<FunctionParameter> funcParams;
        funcParams.emplace_back("name", "string", false, "");
        funcParams.emplace_back("msg", "string", true, "Hello");
        
        // Register function with NEW overload
        auto funcDeclPtr = std::shared_ptr<Parser::ASTNode>(funcDecl.release());
        auto bodyPtr = std::static_pointer_cast<BlockStmt>(
            static_cast<BlockStmt*>(funcDeclPtr.get())->clone()
        );
        
        funcMgr.defineFunction("greet", funcParams, bodyPtr, funcDeclPtr);
        
        // Test 1: Call with 1 argument (use default)
        std::cout << "(AR) الاختبار 1: greet(\"Alice\")\n";
        std::cout << "(EN) Test 1: greet(\"Alice\")\n";
        
        auto callee1 = std::make_unique<VariableExpr>("greet");
        std::vector<ExprPtr> args1;
        args1.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::STRING, "Alice", Position())
        ));
        auto call1 = std::make_unique<CallExpr>(std::move(callee1), std::move(args1));
        
        call1->accept(evaluator);
        Value result1 = evaluator.getResult();
        
        std::cout << "(AR) النتيجة: " << result1.toString() << "\n";
        std::cout << "(EN) Result: " << result1.toString() << "\n";
        
        if (result1.toString() != "Hello, Alice") {
            std::cout << "(AR) ❌ فشل: متوقع 'Hello, Alice' لكن حصلنا '" << result1.toString() << "'\n";
            std::cout << "(EN) ❌ Failed: Expected 'Hello, Alice' but got '" << result1.toString() << "'\n";
            return false;
        }
        
        // Test 2: Call with 2 arguments (override default)
        std::cout << "\n(AR) الاختبار 2: greet(\"Bob\", \"Hi\")\n";
        std::cout << "(EN) Test 2: greet(\"Bob\", \"Hi\")\n";
        
        auto callee2 = std::make_unique<VariableExpr>("greet");
        std::vector<ExprPtr> args2;
        args2.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::STRING, "Bob", Position())
        ));
        args2.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::STRING, "Hi", Position())
        ));
        auto call2 = std::make_unique<CallExpr>(std::move(callee2), std::move(args2));
        
        call2->accept(evaluator);
        Value result2 = evaluator.getResult();
        
        std::cout << "(AR) النتيجة: " << result2.toString() << "\n";
        std::cout << "(EN) Result: " << result2.toString() << "\n";
        
        if (result2.toString() != "Hi, Bob") {
            std::cout << "(AR) ❌ فشل: متوقع 'Hi, Bob' لكن حصلنا '" << result2.toString() << "'\n";
            std::cout << "(EN) ❌ Failed: Expected 'Hi, Bob' but got '" << result2.toString() << "'\n";
            return false;
        }
        
        std::cout << "(AR) ✅ نجح الاختبار!\n";
        std::cout << "(EN) ✅ Test passed!\n";
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "(AR) خطأ: " << e.what() << "\n";
        std::cout << "(EN) Error: " << e.what() << "\n";
        return false;
    }
}

// =========================================================================
// Test 3: (AR) اختبار معاملات متعددة بقيم افتراضية
//         (EN) Test multiple parameters with defaults
// =========================================================================
bool test_multiple_defaults() {
    std::cout << "\nTest 3: Multiple Default Parameters\n";
    std::cout << "=====================================\n";
    
    try {
        std::cout << "(AR) اختبار: func sum(a, b = 1, c = 2)\n";
        std::cout << "(EN) Testing: func sum(a, b = 1, c = 2)\n\n";
        
        VariableManager varMgr;
        FunctionManager funcMgr;
        ScopeManager scopeMgr;
        StatementExecutor executor(varMgr, funcMgr, scopeMgr);
        ExpressionEvaluator evaluator(varMgr, funcMgr, scopeMgr, executor);
        
        // Create function: func sum(a, b = 1, c = 2) { return a + b + c; }
        std::vector<Parameter> params;
        params.emplace_back("a", DataType::INTEGER);
        
        auto defaultB = std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, "1", Position())
        );
        params.emplace_back("b", DataType::INTEGER, std::move(defaultB));
        
        auto defaultC = std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, "2", Position())
        );
        params.emplace_back("c", DataType::INTEGER, std::move(defaultC));
        
        // Body: return a + b + c;
        auto varA = std::make_unique<VariableExpr>("a");
        auto varB = std::make_unique<VariableExpr>("b");
        auto varC = std::make_unique<VariableExpr>("c");
        
        auto aPlusB = std::make_unique<BinaryExpr>(
            std::move(varA), TokenType::OP_PLUS, std::move(varB)
        );
        auto result = std::make_unique<BinaryExpr>(
            std::move(aPlusB), TokenType::OP_PLUS, std::move(varC)
        );
        auto returnStmt = std::make_unique<ReturnStmt>(std::move(result));
        
        std::vector<StmtPtr> bodyStmts;
        bodyStmts.push_back(std::move(returnStmt));
        auto bodyBlock = std::make_unique<BlockStmt>(std::move(bodyStmts));
        
        // Create FunctionDecl
        auto funcDecl = std::make_unique<FunctionDecl>(
            "sum",
            std::move(params),
            DataType::INTEGER,
            std::move(bodyBlock)
        );
        
        // Convert to FunctionParameters
        std::vector<FunctionParameter> funcParams;
        funcParams.emplace_back("a", "integer", false, "");
        funcParams.emplace_back("b", "integer", true, "1");
        funcParams.emplace_back("c", "integer", true, "2");
        
        // Register function
        auto funcDeclPtr = std::shared_ptr<Parser::ASTNode>(funcDecl.release());
        auto bodyPtr = std::static_pointer_cast<BlockStmt>(
            static_cast<BlockStmt*>(funcDeclPtr.get())->clone()
        );
        
        funcMgr.defineFunction("sum", funcParams, bodyPtr, funcDeclPtr);
        
        // Test 1: sum(10) → 10 + 1 + 2 = 13
        std::cout << "(AR) الاختبار 1: sum(10)\n";
        std::cout << "(EN) Test 1: sum(10)\n";
        
        auto callee1 = std::make_unique<VariableExpr>("sum");
        std::vector<ExprPtr> args1;
        args1.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, "10", Position())
        ));
        auto call1 = std::make_unique<CallExpr>(std::move(callee1), std::move(args1));
        
        call1->accept(evaluator);
        Value result1 = evaluator.getResult();
        
        std::cout << "(AR) النتيجة: " << result1.toInt() << "\n";
        std::cout << "(EN) Result: " << result1.toInt() << "\n";
        
        if (result1.toInt() != 13) {
            std::cout << "(AR) ❌ فشل: متوقع 13 لكن حصلنا " << result1.toInt() << "\n";
            std::cout << "(EN) ❌ Failed: Expected 13 but got " << result1.toInt() << "\n";
            return false;
        }
        
        // Test 2: sum(10, 5) → 10 + 5 + 2 = 17
        std::cout << "\n(AR) الاختبار 2: sum(10, 5)\n";
        std::cout << "(EN) Test 2: sum(10, 5)\n";
        
        auto callee2 = std::make_unique<VariableExpr>("sum");
        std::vector<ExprPtr> args2;
        args2.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, "10", Position())
        ));
        args2.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, "5", Position())
        ));
        auto call2 = std::make_unique<CallExpr>(std::move(callee2), std::move(args2));
        
        call2->accept(evaluator);
        Value result2 = evaluator.getResult();
        
        std::cout << "(AR) النتيجة: " << result2.toInt() << "\n";
        std::cout << "(EN) Result: " << result2.toInt() << "\n";
        
        if (result2.toInt() != 17) {
            std::cout << "(AR) ❌ فشل: متوقع 17 لكن حصلنا " << result2.toInt() << "\n";
            std::cout << "(EN) ❌ Failed: Expected 17 but got " << result2.toInt() << "\n";
            return false;
        }
        
        // Test 3: sum(10, 5, 3) → 10 + 5 + 3 = 18
        std::cout << "\n(AR) الاختبار 3: sum(10, 5, 3)\n";
        std::cout << "(EN) Test 3: sum(10, 5, 3)\n";
        
        auto callee3 = std::make_unique<VariableExpr>("sum");
        std::vector<ExprPtr> args3;
        args3.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, "10", Position())
        ));
        args3.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, "5", Position())
        ));
        args3.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, "3", Position())
        ));
        auto call3 = std::make_unique<CallExpr>(std::move(callee3), std::move(args3));
        
        call3->accept(evaluator);
        Value result3 = evaluator.getResult();
        
        std::cout << "(AR) النتيجة: " << result3.toInt() << "\n";
        std::cout << "(EN) Result: " << result3.toInt() << "\n";
        
        if (result3.toInt() != 18) {
            std::cout << "(AR) ❌ فشل: متوقع 18 لكن حصلنا " << result3.toInt() << "\n";
            std::cout << "(EN) ❌ Failed: Expected 18 but got " << result3.toInt() << "\n";
            return false;
        }
        
        std::cout << "(AR) ✅ نجح الاختبار!\n";
        std::cout << "(EN) ✅ Test passed!\n";
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "(AR) خطأ: " << e.what() << "\n";
        std::cout << "(EN) Error: " << e.what() << "\n";
        return false;
    }
}

// =========================================================================
// Test 4: (AR) اختبار Integer defaults
//         (EN) Test Integer default values
// =========================================================================
bool test_integer_defaults() {
    std::cout << "\nTest 4: Integer Default Parameters\n";
    std::cout << "====================================\n";
    
    try {
        std::cout << "(AR) اختبار: func calculate(x, y = 10)\n";
        std::cout << "(EN) Testing: func calculate(x, y = 10)\n\n";
        
        VariableManager varMgr;
        FunctionManager funcMgr;
        ScopeManager scopeMgr;
        StatementExecutor executor(varMgr, funcMgr, scopeMgr);
        ExpressionEvaluator evaluator(varMgr, funcMgr, scopeMgr, executor);
        
        // Create function: func calculate(x, y = 10) { return x + y; }
        std::vector<Parameter> params;
        params.emplace_back("x", DataType::INTEGER);
        
        auto defaultY = std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, "10", Position())
        );
        params.emplace_back("y", DataType::INTEGER, std::move(defaultY));
        
        // Body: return x + y;
        auto varX = std::make_unique<VariableExpr>("x");
        auto varY = std::make_unique<VariableExpr>("y");
        auto sum = std::make_unique<BinaryExpr>(
            std::move(varX), TokenType::OP_PLUS, std::move(varY)
        );
        auto returnStmt = std::make_unique<ReturnStmt>(std::move(sum));
        
        std::vector<StmtPtr> bodyStmts;
        bodyStmts.push_back(std::move(returnStmt));
        auto bodyBlock = std::make_unique<BlockStmt>(std::move(bodyStmts));
        
        // Create FunctionDecl
        auto funcDecl = std::make_unique<FunctionDecl>(
            "calculate",
            std::move(params),
            DataType::INTEGER,
            std::move(bodyBlock)
        );
        
        // Convert to FunctionParameters
        std::vector<FunctionParameter> funcParams;
        funcParams.emplace_back("x", "integer", false, "");
        funcParams.emplace_back("y", "integer", true, "10");
        
        // Register function
        auto funcDeclPtr = std::shared_ptr<Parser::ASTNode>(funcDecl.release());
        auto bodyPtr = std::static_pointer_cast<BlockStmt>(
            static_cast<BlockStmt*>(funcDeclPtr.get())->clone()
        );
        
        funcMgr.defineFunction("calculate", funcParams, bodyPtr, funcDeclPtr);
        
        // Test 1: calculate(5) → 5 + 10 = 15
        std::cout << "(AR) الاختبار: calculate(5)\n";
        std::cout << "(EN) Test: calculate(5)\n";
        
        auto callee = std::make_unique<VariableExpr>("calculate");
        std::vector<ExprPtr> args;
        args.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, "5", Position())
        ));
        auto call = std::make_unique<CallExpr>(std::move(callee), std::move(args));
        
        call->accept(evaluator);
        Value result = evaluator.getResult();
        
        std::cout << "(AR) النتيجة: " << result.toInt() << "\n";
        std::cout << "(EN) Result: " << result.toInt() << "\n";
        
        if (result.toInt() != 15) {
            std::cout << "(AR) ❌ فشل: متوقع 15 لكن حصلنا " << result.toInt() << "\n";
            std::cout << "(EN) ❌ Failed: Expected 15 but got " << result.toInt() << "\n";
            return false;
        }
        
        std::cout << "(AR) ✅ نجح الاختبار!\n";
        std::cout << "(EN) ✅ Test passed!\n";
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "(AR) خطأ: " << e.what() << "\n";
        std::cout << "(EN) Error: " << e.what() << "\n";
        return false;
    }
}

// =========================================================================
// Test 5: (AR) اختبار حالة الخطأ - معامل مطلوب مفقود
//         (EN) Test error case - missing required parameter
// =========================================================================
bool test_missing_required_param() {
    std::cout << "\nTest 5: Missing Required Parameter (Error Case)\n";
    std::cout << "=================================================\n";
    
    try {
        std::cout << "(AR) اختبار: func need(a, b) - استدعاء need(5)\n";
        std::cout << "(EN) Testing: func need(a, b) - calling need(5)\n\n";
        
        VariableManager varMgr;
        FunctionManager funcMgr;
        ScopeManager scopeMgr;
        StatementExecutor executor(varMgr, funcMgr, scopeMgr);
        ExpressionEvaluator evaluator(varMgr, funcMgr, scopeMgr, executor);
        
        // Create function: func need(a, b) { return a + b; }
        std::vector<Parameter> params;
        params.emplace_back("a", DataType::INTEGER);
        params.emplace_back("b", DataType::INTEGER);  // No default - required!
        
        // Body: return a + b;
        auto varA = std::make_unique<VariableExpr>("a");
        auto varB = std::make_unique<VariableExpr>("b");
        auto sum = std::make_unique<BinaryExpr>(
            std::move(varA), TokenType::OP_PLUS, std::move(varB)
        );
        auto returnStmt = std::make_unique<ReturnStmt>(std::move(sum));
        
        std::vector<StmtPtr> bodyStmts;
        bodyStmts.push_back(std::move(returnStmt));
        auto bodyBlock = std::make_unique<BlockStmt>(std::move(bodyStmts));
        
        // Create FunctionDecl
        auto funcDecl = std::make_unique<FunctionDecl>(
            "need",
            std::move(params),
            DataType::INTEGER,
            std::move(bodyBlock)
        );
        
        // Convert to FunctionParameters
        std::vector<FunctionParameter> funcParams;
        funcParams.emplace_back("a", "integer", false, "");
        funcParams.emplace_back("b", "integer", false, "");  // No default!
        
        // Register function
        auto funcDeclPtr = std::shared_ptr<Parser::ASTNode>(funcDecl.release());
        auto bodyPtr = std::static_pointer_cast<BlockStmt>(
            static_cast<BlockStmt*>(funcDeclPtr.get())->clone()
        );
        
        funcMgr.defineFunction("need", funcParams, bodyPtr, funcDeclPtr);
        
        // Try to call with only 1 argument (should fail)
        std::cout << "(AR) محاولة استدعاء need(5) - يجب أن يفشل!\n";
        std::cout << "(EN) Trying to call need(5) - should fail!\n";
        
        auto callee = std::make_unique<VariableExpr>("need");
        std::vector<ExprPtr> args;
        args.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, "5", Position())
        ));
        auto call = std::make_unique<CallExpr>(std::move(callee), std::move(args));
        
        bool exceptionThrown = false;
        try {
            call->accept(evaluator);
            // Should NOT reach here
        } catch (const std::exception& e) {
            exceptionThrown = true;
            std::cout << "(AR) ✅ استثناء متوقع: " << e.what() << "\n";
            std::cout << "(EN) ✅ Expected exception: " << e.what() << "\n";
        }
        
        if (!exceptionThrown) {
            std::cout << "(AR) ❌ فشل: كان يجب أن يُطلق استثناء!\n";
            std::cout << "(EN) ❌ Failed: Should have thrown an exception!\n";
            return false;
        }
        
        std::cout << "(AR) ✅ نجح الاختبار!\n";
        std::cout << "(EN) ✅ Test passed!\n";
        return true;
        
    } catch (const std::exception& e) {
        // Unexpected exception during setup
        std::cout << "(AR) خطأ غير متوقع: " << e.what() << "\n";
        std::cout << "(EN) Unexpected error: " << e.what() << "\n";
        return false;
    }
}

// =========================================================================
// Main
// =========================================================================
int main() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "Default Parameter Tests\n";
    std::cout << "========================================\n";
    
    int passed = 0;
    int total = 0;
    
    // Test 1: Structure check
    total++;
    if (test_default_params_structure()) {
        std::cout << "Test 1: Default Parameters Structure Check - PASSED ✓\n";
        passed++;
    } else {
        std::cout << "Test 1: Default Parameters Structure Check - FAILED ✗\n";
    }
    
    // Test 2: Simple default
    total++;
    if (test_simple_default()) {
        std::cout << "Test 2: Simple Default Parameter - PASSED ✓\n";
        passed++;
    } else {
        std::cout << "Test 2: Simple Default Parameter - FAILED ✗\n";
    }
    
    // Test 3: Multiple defaults
    total++;
    if (test_multiple_defaults()) {
        std::cout << "Test 3: Multiple Default Parameters - PASSED ✓\n";
        passed++;
    } else {
        std::cout << "Test 3: Multiple Default Parameters - FAILED ✗\n";
    }
    
    // Test 4: Integer defaults
    total++;
    if (test_integer_defaults()) {
        std::cout << "Test 4: Integer Default Parameters - PASSED ✓\n";
        passed++;
    } else {
        std::cout << "Test 4: Integer Default Parameters - FAILED ✗\n";
    }
    
    // Test 5: Error case
    total++;
    if (test_missing_required_param()) {
        std::cout << "Test 5: Missing Required Parameter - PASSED ✓\n";
        passed++;
    } else {
        std::cout << "Test 5: Missing Required Parameter - FAILED ✗\n";
    }
    
    // Summary
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "Summary: " << passed << "/" << total << " tests passed\n";
    std::cout << "========================================\n";
    
    if (passed == total) {
        std::cout << "\n🎉 (AR) جميع الاختبارات نجحت!\n";
        std::cout << "🎉 (EN) All tests passed!\n";
    } else {
        std::cout << "\n❌ (AR) بعض الاختبارات فشلت!\n";
        std::cout << "❌ (EN) Some tests failed!\n";
    }
    
    return (passed == total) ? 0 : 1;
}
