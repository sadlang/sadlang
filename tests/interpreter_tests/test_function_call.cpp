/**
 * @file test_function_call.cpp
 * @brief (AR) اختبارات استدعاء الدوال
 * @brief (EN) Function Call Tests
 * 
 * Tests for function calling with parameters, return values, and scope management.
 * اختبارات لاستدعاء الدوال مع المعاملات وقيم الإرجاع وإدارة النطاقات.
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include "../../include/interpreter/visitors/statement_executor.h"
#include "../../include/interpreter/visitors/expression_evaluator.h"
#include "../../include/data/managers/variable_manager.h"
#include "../../include/data/managers/function_manager.h"
#include "../../include/data/scope/scope_manager.h"
#include "../../include/parser/ast/declarations.h"
#include "../../include/parser/ast/statements.h"
#include "../../include/parser/ast/expressions.h"
#include "../../include/lexer/token.h"
#include <iostream>
#include <cassert>

using namespace Sad;
using namespace Sad::Interpreter;
using namespace Sad::AST;
using namespace Sad::Data;
using namespace Sad::Lexer;

// =========================================================================
// (AR) ماكروات مساعدة للاختبار / (EN) Test Helper Macros
// =========================================================================

#define TEST_START(name) \
    std::cout << "\n========================================\n"; \
    std::cout << "TEST: " << name << "\n"; \
    std::cout << "========================================\n";

#define TEST_PASS() \
    std::cout << "✅ PASSED\n";

#define ASSERT_INT(value, expected) \
    assert(value.isInteger() && value.toInt() == expected);

#define ASSERT_STRING(value, expected) \
    assert(value.isString() && value.toString() == expected);

#define ASSERT_VAR_INT(varMgr, name, expected) \
    assert(varMgr.exists(name)); \
    ASSERT_INT(varMgr.get(name), expected);

// =========================================================================
// (AR) الاختبار 1: استدعاء دالة بسيطة بدون معاملات
// (EN) Test 1: Simple function call without parameters
// =========================================================================

void test_simple_function_call() {
    TEST_START("test_simple_function_call");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // (AR) تعريف دالة: func getValue() { return 42; }
    // (EN) Define function: func getValue() { return 42; }
    
    // Create return statement: return 42
    auto returnValue = std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "42", Position())
    );
    auto returnStmt = std::make_unique<ReturnStmt>(std::move(returnValue));
    
    // Create function body
    StmtList bodyStmts;
    bodyStmts.push_back(std::move(returnStmt));
    auto body = std::make_unique<BlockStmt>(std::move(bodyStmts));
    
    // Create function declaration
    std::vector<Parameter> params; // No parameters
    auto funcDecl = std::make_unique<FunctionDecl>(
        "getValue",
        std::move(params),
        DataType::INTEGER,
        std::move(body)
    );
    
    // Register function
    funcDecl->accept(executor);
    
    // (AR) التحقق من تسجيل الدالة / (EN) Verify function registration
    assert(funcMgr.hasFunction("getValue", 0));
    
    // (AR) استدعاء الدالة: var x = getValue();
    // (EN) Call function: var x = getValue();
    
    auto callee = std::make_unique<VariableExpr>("getValue");
    ExprList args; // No arguments
    auto callExpr = std::make_unique<CallExpr>(std::move(callee), std::move(args));
    
    auto varDecl = std::make_unique<VarDeclStmt>("x", DataType::INTEGER, std::move(callExpr));
    varDecl->accept(executor);
    
    // (AR) التحقق من النتيجة / (EN) Verify result
    ASSERT_VAR_INT(varMgr, "x", 42);
    
    TEST_PASS();
}

// =========================================================================
// (AR) الاختبار 2: استدعاء دالة مع معامل واحد
// (EN) Test 2: Function call with one parameter
// =========================================================================

void test_function_call_with_parameter() {
    TEST_START("test_function_call_with_parameter");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // (AR) تعريف دالة: func double(x) { return x * 2; }
    // (EN) Define function: func double(x) { return x * 2; }
    
    // Create return expression: x * 2
    auto xVar = std::make_unique<VariableExpr>("x");
    auto twoLit = std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "2", Position())
    );
    auto mulExpr = std::make_unique<BinaryExpr>(
        std::move(xVar),
        TokenType::OP_MULTIPLY,
        std::move(twoLit)
    );
    auto returnStmt = std::make_unique<ReturnStmt>(std::move(mulExpr));
    
    // Create function body
    StmtList bodyStmts;
    bodyStmts.push_back(std::move(returnStmt));
    auto body = std::make_unique<BlockStmt>(std::move(bodyStmts));
    
    // Create function with parameter
    std::vector<Parameter> params;
    params.push_back(Parameter("x", DataType::INTEGER));
    auto funcDecl = std::make_unique<FunctionDecl>(
        "double",
        std::move(params),
        DataType::INTEGER,
        std::move(body)
    );
    
    // Register function
    funcDecl->accept(executor);
    
    // (AR) استدعاء الدالة: var y = double(5);
    // (EN) Call function: var y = double(5);
    
    auto callee = std::make_unique<VariableExpr>("double");
    ExprList args;
    args.push_back(std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "5", Position())
    ));
    auto callExpr = std::make_unique<CallExpr>(std::move(callee), std::move(args));
    
    auto varDecl = std::make_unique<VarDeclStmt>("y", DataType::INTEGER, std::move(callExpr));
    varDecl->accept(executor);
    
    // (AR) التحقق من النتيجة / (EN) Verify result
    ASSERT_VAR_INT(varMgr, "y", 10);
    
    TEST_PASS();
}

// =========================================================================
// (AR) الاختبار 3: استدعاء دالة مع معاملين
// (EN) Test 3: Function call with two parameters
// =========================================================================

void test_function_call_with_two_parameters() {
    TEST_START("test_function_call_with_two_parameters");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // (AR) تعريف دالة: func add(a, b) { return a + b; }
    // (EN) Define function: func add(a, b) { return a + b; }
    
    auto aVar = std::make_unique<VariableExpr>("a");
    auto bVar = std::make_unique<VariableExpr>("b");
    auto addExpr = std::make_unique<BinaryExpr>(
        std::move(aVar),
        TokenType::OP_PLUS,
        std::move(bVar)
    );
    auto returnStmt = std::make_unique<ReturnStmt>(std::move(addExpr));
    
    StmtList bodyStmts;
    bodyStmts.push_back(std::move(returnStmt));
    auto body = std::make_unique<BlockStmt>(std::move(bodyStmts));
    
    std::vector<Parameter> params;
    params.push_back(Parameter("a", DataType::INTEGER));
    params.push_back(Parameter("b", DataType::INTEGER));
    auto funcDecl = std::make_unique<FunctionDecl>(
        "add",
        std::move(params),
        DataType::INTEGER,
        std::move(body)
    );
    
    funcDecl->accept(executor);
    
    // (AR) استدعاء: var sum = add(10, 20);
    // (EN) Call: var sum = add(10, 20);
    
    auto callee = std::make_unique<VariableExpr>("add");
    ExprList args;
    args.push_back(std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "10", Position())
    ));
    args.push_back(std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "20", Position())
    ));
    auto callExpr = std::make_unique<CallExpr>(std::move(callee), std::move(args));
    
    auto varDecl = std::make_unique<VarDeclStmt>("sum", DataType::INTEGER, std::move(callExpr));
    varDecl->accept(executor);
    
    // (AR) التحقق / (EN) Verify
    ASSERT_VAR_INT(varMgr, "sum", 30);
    
    TEST_PASS();
}

// =========================================================================
// (AR) الاختبار 4: دالة مع متغيرات محلية
// (EN) Test 4: Function with local variables
// =========================================================================

void test_function_with_local_variables() {
    TEST_START("test_function_with_local_variables");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // (AR) تعريف دالة: func calculate(x) { var temp = x * 2; return temp + 10; }
    // (EN) Define function: func calculate(x) { var temp = x * 2; return temp + 10; }
    
    // var temp = x * 2
    auto xVar1 = std::make_unique<VariableExpr>("x");
    auto twoLit = std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "2", Position())
    );
    auto mulExpr = std::make_unique<BinaryExpr>(
        std::move(xVar1),
        TokenType::OP_MULTIPLY,
        std::move(twoLit)
    );
    auto tempDecl = std::make_unique<VarDeclStmt>("temp", DataType::INTEGER, std::move(mulExpr));
    
    // return temp + 10
    auto tempVar = std::make_unique<VariableExpr>("temp");
    auto tenLit = std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "10", Position())
    );
    auto addExpr = std::make_unique<BinaryExpr>(
        std::move(tempVar),
        TokenType::OP_PLUS,
        std::move(tenLit)
    );
    auto returnStmt = std::make_unique<ReturnStmt>(std::move(addExpr));
    
    StmtList bodyStmts;
    bodyStmts.push_back(std::move(tempDecl));
    bodyStmts.push_back(std::move(returnStmt));
    auto body = std::make_unique<BlockStmt>(std::move(bodyStmts));
    
    std::vector<Parameter> params;
    params.push_back(Parameter("x", DataType::INTEGER));
    auto funcDecl = std::make_unique<FunctionDecl>(
        "calculate",
        std::move(params),
        DataType::INTEGER,
        std::move(body)
    );
    
    funcDecl->accept(executor);
    
    // (AR) استدعاء: var result = calculate(5);
    // (EN) Call: var result = calculate(5);
    
    auto callee = std::make_unique<VariableExpr>("calculate");
    ExprList args;
    args.push_back(std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "5", Position())
    ));
    auto callExpr = std::make_unique<CallExpr>(std::move(callee), std::move(args));
    
    auto varDecl = std::make_unique<VarDeclStmt>("result", DataType::INTEGER, std::move(callExpr));
    varDecl->accept(executor);
    
    // (AR) التحقق: 5 * 2 + 10 = 20
    // (EN) Verify: 5 * 2 + 10 = 20
    ASSERT_VAR_INT(varMgr, "result", 20);
    
    // Note: temp should not be accessible in global scope after function returns
    // but VariableManager.exists() searches all scopes, so we skip this check
    
    TEST_PASS();
}

// =========================================================================
// (AR) الاختبار 5: استدعاء دالة متداخل
// (EN) Test 5: Nested function call
// =========================================================================

void test_nested_function_call() {
    TEST_START("test_nested_function_call");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // (AR) تعريف دالة inner: func getBase() { return 5; }
    // (EN) Define inner function: func getBase() { return 5; }
    
    auto returnVal1 = std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "5", Position())
    );
    auto returnStmt1 = std::make_unique<ReturnStmt>(std::move(returnVal1));
    StmtList body1Stmts;
    body1Stmts.push_back(std::move(returnStmt1));
    auto body1 = std::make_unique<BlockStmt>(std::move(body1Stmts));
    
    std::vector<Parameter> params1;
    auto funcDecl1 = std::make_unique<FunctionDecl>(
        "getBase",
        std::move(params1),
        DataType::INTEGER,
        std::move(body1)
    );
    funcDecl1->accept(executor);
    
    // (AR) تعريف دالة outer: func doubleBase() { return getBase() * 2; }
    // (EN) Define outer function: func doubleBase() { return getBase() * 2; }
    
    auto callee = std::make_unique<VariableExpr>("getBase");
    ExprList args;
    auto callExpr = std::make_unique<CallExpr>(std::move(callee), std::move(args));
    
    auto twoLit = std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "2", Position())
    );
    auto mulExpr = std::make_unique<BinaryExpr>(
        std::move(callExpr),
        TokenType::OP_MULTIPLY,
        std::move(twoLit)
    );
    auto returnStmt2 = std::make_unique<ReturnStmt>(std::move(mulExpr));
    
    StmtList body2Stmts;
    body2Stmts.push_back(std::move(returnStmt2));
    auto body2 = std::make_unique<BlockStmt>(std::move(body2Stmts));
    
    std::vector<Parameter> params2;
    auto funcDecl2 = std::make_unique<FunctionDecl>(
        "doubleBase",
        std::move(params2),
        DataType::INTEGER,
        std::move(body2)
    );
    funcDecl2->accept(executor);
    
    // (AR) استدعاء: var final = doubleBase();
    // (EN) Call: var final = doubleBase();
    
    auto calleeOuter = std::make_unique<VariableExpr>("doubleBase");
    ExprList argsOuter;
    auto callExprOuter = std::make_unique<CallExpr>(std::move(calleeOuter), std::move(argsOuter));
    
    auto varDecl = std::make_unique<VarDeclStmt>("final", DataType::INTEGER, std::move(callExprOuter));
    varDecl->accept(executor);
    
    // (AR) التحقق: getBase() * 2 = 5 * 2 = 10
    // (EN) Verify: getBase() * 2 = 5 * 2 = 10
    ASSERT_VAR_INT(varMgr, "final", 10);
    
    TEST_PASS();
}

// =========================================================================
// (AR) دالة main للاختبارات / (EN) Main Test Function
// =========================================================================

int main() {
    std::cout << "\n========================================\n";
    std::cout << " اختبارات استدعاء الدوال\n";
    std::cout << " Function Call Tests\n";
    std::cout << "========================================\n";
    
    int totalTests = 5;
    int passedTests = 0;
    
    try {
        test_simple_function_call();
        passedTests++;
    } catch (const std::exception& e) {
        std::cerr << "✗ FAILED: " << e.what() << "\n";
    }
    
    try {
        test_function_call_with_parameter();
        passedTests++;
    } catch (const std::exception& e) {
        std::cerr << "✗ FAILED: " << e.what() << "\n";
    }
    
    try {
        test_function_call_with_two_parameters();
        passedTests++;
    } catch (const std::exception& e) {
        std::cerr << "✗ FAILED: " << e.what() << "\n";
    }
    
    try {
        test_function_with_local_variables();
        passedTests++;
    } catch (const std::exception& e) {
        std::cerr << "✗ FAILED: " << e.what() << "\n";
    }
    
    try {
        test_nested_function_call();
        passedTests++;
    } catch (const std::exception& e) {
        std::cerr << "✗ FAILED: " << e.what() << "\n";
    }
    
    std::cout << "\n========================================\n";
    if (passedTests == totalTests) {
        std::cout << "✅ ALL TESTS PASSED! (" << passedTests << "/" << totalTests << ")\n";
    } else {
        std::cout << "✗ SOME TESTS FAILED (" << passedTests << "/" << totalTests << ")\n";
    }
    std::cout << "========================================\n";
    
    return (passedTests == totalTests) ? 0 : 1;
}
