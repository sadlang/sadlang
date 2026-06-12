/**
 * @file test_default_params_simple.cpp
 * @brief (AR) اختبار Default Parameters البسيط (String-based)
 * @brief (EN) Simple Default Parameters Test (String-based)
 * 
 * اختبارات للقيم الافتراضية باستخدام التحليل النصي (string parsing)
 * Tests for default values using string parsing approach
 * 
 * @author S Language Development Team
 * @date November 21, 2025
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
// Helper function to create simple test environment
// =========================================================================
void registerSimpleFunction(
    FunctionManager& funcMgr,
    const std::string& name,
    const std::vector<FunctionParameter>& params,
    std::shared_ptr<BlockStmt> body
) {
    // Cast BlockStmt to ASTNode using reinterpret (safe since BlockStmt extends ASTNode)
    std::shared_ptr<Parser::ASTNode> bodyNode = std::reinterpret_pointer_cast<Parser::ASTNode>(body);
    funcMgr.defineFunction(name, params, bodyNode);
}

// =========================================================================
// Test 1: (AR) اختبار بنية Parameter.defaultValue
//         (EN) Test Parameter.defaultValue structure
// =========================================================================
bool test_parameter_structure() {
    std::cout << "\nTest 1: Parameter Default Value Structure\n";
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
            std::cout << "  - Has Default: Yes\n";
            
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
// Test 2: (AR) اختبار قيمة افتراضية Integer بسيطة
//         (EN) Test simple Integer default value
// =========================================================================
bool test_integer_default() {
    std::cout << "\nTest 2: Simple Integer Default Value\n";
    std::cout << "======================================\n";
    
    try {
        std::cout << "(AR) اختبار: func add(x, y = 10) { return x + y; }\n";
        std::cout << "(EN) Testing: func add(x, y = 10) { return x + y; }\n\n";
        
        ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
        FunctionManager funcMgr;
        ScopeManager scopeMgr;
        StatementExecutor executor(varMgr, funcMgr, scopeMgr);
        ExpressionEvaluator evaluator(varMgr, funcMgr, scopeMgr, executor);
        
        // Create function body: return x + y;
        auto varX = std::make_unique<VariableExpr>("x");
        auto varY = std::make_unique<VariableExpr>("y");
        auto sum = std::make_unique<BinaryExpr>(
            std::move(varX), TokenType::OP_PLUS, std::move(varY)
        );
        auto returnStmt = std::make_unique<ReturnStmt>(std::move(sum));
        
        std::vector<StmtPtr> bodyStmts;
        bodyStmts.push_back(std::move(returnStmt));
        auto bodyBlock = std::make_shared<BlockStmt>(std::move(bodyStmts));
        
        // Define function parameters (string-based defaults)
        std::vector<FunctionParameter> params;
        params.emplace_back("x", "integer", false, "");        // Required
        params.emplace_back("y", "integer", true, "10");       // Default = 10
        
        // Register function
        registerSimpleFunction(funcMgr, "add", params, bodyBlock);
        
        // Test: add(5) should return 15 (5 + 10)
        std::cout << "(AR) الاختبار: add(5)\n";
        std::cout << "(EN) Test: add(5)\n";
        
        auto callee = std::make_unique<VariableExpr>("add");
        std::vector<ExprPtr> args;
        args.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, "5", Position())
        ));
        auto callExpr = std::make_unique<CallExpr>(std::move(callee), std::move(args));
        
        callExpr->accept(evaluator);
        Value result = evaluator.getResult();
        
        std::cout << "(AR) النتيجة: " << result.toInt() << "\n";
        std::cout << "(EN) Result: " << result.toInt() << "\n";
        
        if (result.toInt() != 15) {
            std::cout << "(AR) ❌ فشل: متوقع 15 لكن حصلنا " << result.toInt() << "\n";
            std::cout << "(EN) ❌ Failed: Expected 15 but got " << result.toInt() << "\n";
            return false;
        }
        
        std::cout << "(AR) ✅ نجح!\n";
        std::cout << "(EN) ✅ Passed!\n";
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "(AR) خطأ: " << e.what() << "\n";
        std::cout << "(EN) Error: " << e.what() << "\n";
        return false;
    }
}

// =========================================================================
// Test 3: (AR) اختبار معاملات متعددة بقيم افتراضية
//         (EN) Test multiple default parameters
// =========================================================================
bool test_multiple_defaults() {
    std::cout << "\nTest 3: Multiple Default Parameters\n";
    std::cout << "=====================================\n";
    
    try {
        std::cout << "(AR) اختبار: func sum(a, b = 1, c = 2)\n";
        std::cout << "(EN) Testing: func sum(a, b = 1, c = 2)\n\n";
        
        ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
        FunctionManager funcMgr;
        ScopeManager scopeMgr;
        StatementExecutor executor(varMgr, funcMgr, scopeMgr);
        ExpressionEvaluator evaluator(varMgr, funcMgr, scopeMgr, executor);
        
        // Create function body: return a + b + c;
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
        auto bodyBlock = std::make_shared<BlockStmt>(std::move(bodyStmts));
        
        // Define function parameters
        std::vector<FunctionParameter> params;
        params.emplace_back("a", "integer", false, "");   // Required
        params.emplace_back("b", "integer", true, "1");   // Default = 1
        params.emplace_back("c", "integer", true, "2");   // Default = 2
        
        // Register function
        registerSimpleFunction(funcMgr, "sum", params, bodyBlock);
        
        // Test 1: sum(10) → 10 + 1 + 2 = 13
        std::cout << "(AR) الاختبار 1: sum(10) → متوقع 13\n";
        std::cout << "(EN) Test 1: sum(10) → expect 13\n";
        
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
            std::cout << "(AR) ❌ فشل: متوقع 13\n";
            std::cout << "(EN) ❌ Failed: Expected 13\n";
            return false;
        }
        
        // Test 2: sum(10, 5) → 10 + 5 + 2 = 17
        // Create fresh evaluator with new StatementExecutor for test 2
        ScopeManager scopeMgr;
    VariableManager varMgr2(scopeMgr);
        ScopeManager scopeMgr2;  // NEW: Fresh scope manager
        StatementExecutor executor2(varMgr2, funcMgr, scopeMgr2);  // NEW: Fresh executor
        ExpressionEvaluator evaluator2(varMgr2, funcMgr, scopeMgr2, executor2);
        
        std::cout << "\n(AR) الاختبار 2: sum(10, 5) → متوقع 17\n";
        std::cout << "(EN) Test 2: sum(10, 5) → expect 17\n";
        
        auto callee2 = std::make_unique<VariableExpr>("sum");
        std::vector<ExprPtr> args2;
        args2.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, "10", Position())
        ));
        args2.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, "5", Position())
        ));
        auto call2 = std::make_unique<CallExpr>(std::move(callee2), std::move(args2));
        
        call2->accept(evaluator2);  // Use evaluator2
        Value result2 = evaluator2.getResult();
        
        std::cout << "(AR) النتيجة: " << result2.toInt() << "\n";
        std::cout << "(EN) Result: " << result2.toInt() << "\n";
        
        if (result2.toInt() != 17) {
            std::cout << "(AR) ❌ فشل: متوقع 17\n";
            std::cout << "(EN) ❌ Failed: Expected 17\n";
            return false;
        }
        
        std::cout << "(AR) ✅ جميع الاختبارات نجحت!\n";
        std::cout << "(EN) ✅ All tests passed!\n";
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "(AR) خطأ: " << e.what() << "\n";
        std::cout << "(EN) Error: " << e.what() << "\n";
        return false;
    }
}

// =========================================================================
// Test 4: (AR) اختبار تجاوز القيمة الافتراضية
//         (EN) Test overriding default value
// =========================================================================
bool test_override_default() {
    std::cout << "\nTest 4: Override Default Value\n";
    std::cout << "================================\n";
    
    try {
        std::cout << "(AR) اختبار: func multiply(x, factor = 2)\n";
        std::cout << "(EN) Testing: func multiply(x, factor = 2)\n\n";
        
        ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
        FunctionManager funcMgr;
        ScopeManager scopeMgr;
        StatementExecutor executor(varMgr, funcMgr, scopeMgr);
        ExpressionEvaluator evaluator(varMgr, funcMgr, scopeMgr, executor);
        
        // Create function body: return x * factor;
        auto varX = std::make_unique<VariableExpr>("x");
        auto varFactor = std::make_unique<VariableExpr>("factor");
        auto product = std::make_unique<BinaryExpr>(
            std::move(varX), TokenType::OP_MULTIPLY, std::move(varFactor)
        );
        auto returnStmt = std::make_unique<ReturnStmt>(std::move(product));
        
        std::vector<StmtPtr> bodyStmts;
        bodyStmts.push_back(std::move(returnStmt));
        auto bodyBlock = std::make_shared<BlockStmt>(std::move(bodyStmts));
        
        // Define function parameters
        std::vector<FunctionParameter> params;
        params.emplace_back("x", "integer", false, "");
        params.emplace_back("factor", "integer", true, "2");  // Default = 2
        
        // Register function
        registerSimpleFunction(funcMgr, "multiply", params, bodyBlock);
        
        // Test 1: multiply(5) → 5 * 2 = 10
        std::cout << "(AR) الاختبار 1: multiply(5) → متوقع 10\n";
        std::cout << "(EN) Test 1: multiply(5) → expect 10\n";
        
        auto callee1 = std::make_unique<VariableExpr>("multiply");
        std::vector<ExprPtr> args1;
        args1.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, "5", Position())
        ));
        auto call1 = std::make_unique<CallExpr>(std::move(callee1), std::move(args1));
        
        call1->accept(evaluator);
        Value result1 = evaluator.getResult();
        
        std::cout << "(AR) النتيجة: " << result1.toInt() << "\n";
        std::cout << "(EN) Result: " << result1.toInt() << "\n";
        
        if (result1.toInt() != 10) {
            std::cout << "(AR) ❌ فشل\n";
            std::cout << "(EN) ❌ Failed\n";
            return false;
        }
        
        // Test 2: multiply(5, 3) → 5 * 3 = 15 (override default)
        // Create fresh evaluator with new StatementExecutor for test 2
        ScopeManager scopeMgr;
    VariableManager varMgr2(scopeMgr);
        ScopeManager scopeMgr2;  // NEW: Fresh scope manager
        StatementExecutor executor2(varMgr2, funcMgr, scopeMgr2);  // NEW: Fresh executor
        ExpressionEvaluator evaluator2(varMgr2, funcMgr, scopeMgr2, executor2);
        
        std::cout << "\n(AR) الاختبار 2: multiply(5, 3) → متوقع 15 (تجاوز القيمة الافتراضية)\n";
        std::cout << "(EN) Test 2: multiply(5, 3) → expect 15 (override default)\n";
        
        auto callee2 = std::make_unique<VariableExpr>("multiply");
        std::vector<ExprPtr> args2;
        args2.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, "5", Position())
        ));
        args2.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, "3", Position())
        ));
        auto call2 = std::make_unique<CallExpr>(std::move(callee2), std::move(args2));
        
        call2->accept(evaluator2);  // Use evaluator2
        Value result2 = evaluator2.getResult();
        
        std::cout << "(AR) النتيجة: " << result2.toInt() << "\n";
        std::cout << "(EN) Result: " << result2.toInt() << "\n";
        
        if (result2.toInt() != 15) {
            std::cout << "(AR) ❌ فشل\n";
            std::cout << "(EN) ❌ Failed\n";
            return false;
        }
        
        std::cout << "(AR) ✅ جميع الاختبارات نجحت!\n";
        std::cout << "(EN) ✅ All tests passed!\n";
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "(AR) خطأ: " << e.what() << "\n";
        std::cout << "(EN) Error: " << e.what() << "\n";
        return false;
    }
}

// =========================================================================
// Test 5: (AR) اختبار خطأ - معامل مطلوب مفقود
//         (EN) Test error - missing required parameter
// =========================================================================
bool test_missing_required() {
    std::cout << "\nTest 5: Missing Required Parameter (Error Case)\n";
    std::cout << "=================================================\n";
    
    try {
        std::cout << "(AR) اختبار: func need(a, b) - استدعاء need(5)\n";
        std::cout << "(EN) Testing: func need(a, b) - calling need(5)\n\n";
        
        ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
        FunctionManager funcMgr;
        ScopeManager scopeMgr;
        StatementExecutor executor(varMgr, funcMgr, scopeMgr);
        ExpressionEvaluator evaluator(varMgr, funcMgr, scopeMgr, executor);
        
        // Create function body: return a + b;
        auto varA = std::make_unique<VariableExpr>("a");
        auto varB = std::make_unique<VariableExpr>("b");
        auto sum = std::make_unique<BinaryExpr>(
            std::move(varA), TokenType::OP_PLUS, std::move(varB)
        );
        auto returnStmt = std::make_unique<ReturnStmt>(std::move(sum));
        
        std::vector<StmtPtr> bodyStmts;
        bodyStmts.push_back(std::move(returnStmt));
        auto bodyBlock = std::make_shared<BlockStmt>(std::move(bodyStmts));
        
        // Define function parameters (both required!)
        std::vector<FunctionParameter> params;
        params.emplace_back("a", "integer", false, "");  // Required
        params.emplace_back("b", "integer", false, "");  // Required (no default!)
        
        // Register function
        registerSimpleFunction(funcMgr, "need", params, bodyBlock);
        
        // Try to call with only 1 argument (should throw exception)
        std::cout << "(AR) محاولة استدعاء need(5) - يجب أن يفشل!\n";
        std::cout << "(EN) Trying to call need(5) - should fail!\n";
        
        auto callee = std::make_unique<VariableExpr>("need");
        std::vector<ExprPtr> args;
        args.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, "5", Position())
        ));
        auto callExpr = std::make_unique<CallExpr>(std::move(callee), std::move(args));
        
        bool exceptionThrown = false;
        try {
            callExpr->accept(evaluator);
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
        
        std::cout << "(AR) ✅ نجح!\n";
        std::cout << "(EN) ✅ Passed!\n";
        return true;
        
    } catch (const std::exception& e) {
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
    std::cout << "Simple Default Parameter Tests\n";
    std::cout << "(String-based defaults)\n";
    std::cout << "========================================\n";
    
    int passed = 0;
    int total = 0;
    
    // Test 1: Structure
    total++;
    if (test_parameter_structure()) {
        std::cout << "Test 1: Parameter Structure - PASSED ✓\n";
        passed++;
    } else {
        std::cout << "Test 1: Parameter Structure - FAILED ✗\n";
    }
    
    // Test 2: Integer default
    total++;
    if (test_integer_default()) {
        std::cout << "Test 2: Integer Default - PASSED ✓\n";
        passed++;
    } else {
        std::cout << "Test 2: Integer Default - FAILED ✗\n";
    }
    
    // Test 3: Multiple defaults
    total++;
    if (test_multiple_defaults()) {
        std::cout << "Test 3: Multiple Defaults - PASSED ✓\n";
        passed++;
    } else {
        std::cout << "Test 3: Multiple Defaults - FAILED ✗\n";
    }
    
    // Test 4: Override default
    total++;
    if (test_override_default()) {
        std::cout << "Test 4: Override Default - PASSED ✓\n";
        passed++;
    } else {
        std::cout << "Test 4: Override Default - FAILED ✗\n";
    }
    
    // Test 5: Missing required
    total++;
    if (test_missing_required()) {
        std::cout << "Test 5: Missing Required - PASSED ✓\n";
        passed++;
    } else {
        std::cout << "Test 5: Missing Required - FAILED ✗\n";
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
        std::cout << "\n❌ (AR) بعض الاختبارات فشلت\n";
        std::cout << "❌ (EN) Some tests failed\n";
    }
    
    return (passed == total) ? 0 : 1;
}
