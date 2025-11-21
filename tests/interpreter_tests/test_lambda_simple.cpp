/**
 * @file test_lambda_simple.cpp
 * @brief (AR) اختبارات دوال Lambda البسيطة
 * @brief (EN) Simple Lambda Functions Tests
 * 
 * اختبارات شاملة لدوال Lambda في لغة ص
 * Comprehensive tests for Lambda functions in S language
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include <iostream>
#include <cassert>
#include <memory>
#include <vector>

#include "../../include/parser/ast/expressions.h"
#include "../../include/parser/ast/statements.h"
#include "../../include/interpreter/visitors/expression_evaluator.h"
#include "../../include/interpreter/visitors/statement_executor.h"
#include "../../include/data/managers/variable_manager.h"
#include "../../include/data/managers/function_manager.h"
#include "../../include/data/scope/scope_manager.h"
#include "../../include/lexer/token.h"

using namespace Sad;
using namespace Sad::AST;
using namespace Sad::Interpreter;
using namespace Sad::Data;
using namespace Sad::Lexer;

// =========================================================================
// (AR) اختبار 1: Lambda بسيط بدون معاملات
// (EN) Test 1: Simple Lambda without parameters
// =========================================================================

void test_lambda_no_params() {
    std::cout << "\n=== Test 1: Lambda without parameters ===\n";
    
    // var getValue = lambda() { 42 };
    // var result = getValue();
    // assert result == 42
    
    VariableManager varMgr;
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    ExpressionEvaluator evaluator(varMgr, funcMgr, scopeMgr, executor);
    
    // إنشاء Lambda: lambda() { 42 }
    std::vector<Parameter> params;  // بدون معاملات
    auto body = std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "42", Position())
    );
    auto lambdaExpr = std::make_unique<LambdaExpr>(
        std::move(params),
        std::move(body)
    );
    
    // تقييم Lambda - يجب أن يعيد اسم الدالة
    lambdaExpr->accept(evaluator);
    Value lambdaName = evaluator.getResult();
    
    std::cout << "(AR) اسم Lambda: " << lambdaName.toString() << "\n";
    std::cout << "(EN) Lambda name: " << lambdaName.toString() << "\n";
    
    assert(lambdaName.isString());
    assert(lambdaName.toString().find("__lambda_") == 0);
    
    // حفظ Lambda في متغير
    varMgr.define("getValue", lambdaName);
    
    // استدعاء Lambda: getValue()
    auto calleeVar = std::make_unique<VariableExpr>("getValue");
    std::vector<ExprPtr> args;  // بدون معاملات
    auto callExpr = std::make_unique<CallExpr>(
        std::move(calleeVar),
        std::move(args)
    );
    
    callExpr->accept(evaluator);
    Value result = evaluator.getResult();
    
    std::cout << "(AR) النتيجة: " << result.toInt() << "\n";
    std::cout << "(EN) Result: " << result.toInt() << "\n";
    
    assert(result.isInteger());
    assert(result.toInt() == 42);
    
    std::cout << "✅ PASSED: Lambda without parameters\n";
}

// =========================================================================
// (AR) اختبار 2: Lambda مع معامل واحد
// (EN) Test 2: Lambda with one parameter
// =========================================================================

void test_lambda_one_param() {
    std::cout << "\n=== Test 2: Lambda with one parameter ===\n";
    
    // var double = lambda(x) { x * 2 };
    // var result = double(5);
    // assert result == 10
    
    VariableManager varMgr;
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    ExpressionEvaluator evaluator(varMgr, funcMgr, scopeMgr, executor);
    
    // إنشاء Lambda: lambda(x) { x * 2 }
    std::vector<Parameter> params;
    params.emplace_back("x", DataType::INTEGER);
    
    // الجسم: x * 2
    auto left = std::make_unique<VariableExpr>("x");
    auto right = std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "2", Position())
    );
    auto body = std::make_unique<BinaryExpr>(
        std::move(left),
        TokenType::OP_MULTIPLY,
        std::move(right)
    );
    
    auto lambdaExpr = std::make_unique<LambdaExpr>(
        std::move(params),
        std::move(body)
    );
    
    // تقييم Lambda
    lambdaExpr->accept(evaluator);
    Value lambdaName = evaluator.getResult();
    
    std::cout << "(AR) اسم Lambda: " << lambdaName.toString() << "\n";
    std::cout << "(EN) Lambda name: " << lambdaName.toString() << "\n";
    
    // حفظ في متغير
    varMgr.define("double", lambdaName);
    
    // استدعاء: double(5)
    auto calleeVar = std::make_unique<VariableExpr>("double");
    std::vector<ExprPtr> args;
    args.push_back(std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "5", Position())
    ));
    
    auto callExpr = std::make_unique<CallExpr>(
        std::move(calleeVar),
        std::move(args)
    );
    
    callExpr->accept(evaluator);
    Value result = evaluator.getResult();
    
    std::cout << "(AR) النتيجة: " << result.toInt() << "\n";
    std::cout << "(EN) Result: " << result.toInt() << "\n";
    
    assert(result.isInteger());
    assert(result.toInt() == 10);
    
    std::cout << "✅ PASSED: Lambda with one parameter\n";
}

// =========================================================================
// (AR) اختبار 3: Lambda مع معاملين
// (EN) Test 3: Lambda with two parameters
// =========================================================================

void test_lambda_two_params() {
    std::cout << "\n=== Test 3: Lambda with two parameters ===\n";
    
    // var add = lambda(a, b) { a + b };
    // var result = add(10, 20);
    // assert result == 30
    
    VariableManager varMgr;
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    ExpressionEvaluator evaluator(varMgr, funcMgr, scopeMgr, executor);
    
    // إنشاء Lambda: lambda(a, b) { a + b }
    std::vector<Parameter> params;
    params.emplace_back("a", DataType::INTEGER);
    params.emplace_back("b", DataType::INTEGER);
    
    // الجسم: a + b
    auto left = std::make_unique<VariableExpr>("a");
    auto right = std::make_unique<VariableExpr>("b");
    auto body = std::make_unique<BinaryExpr>(
        std::move(left),
        TokenType::OP_PLUS,
        std::move(right)
    );
    
    auto lambdaExpr = std::make_unique<LambdaExpr>(
        std::move(params),
        std::move(body)
    );
    
    // تقييم Lambda
    lambdaExpr->accept(evaluator);
    Value lambdaName = evaluator.getResult();
    
    std::cout << "(AR) اسم Lambda: " << lambdaName.toString() << "\n";
    std::cout << "(EN) Lambda name: " << lambdaName.toString() << "\n";
    
    // حفظ في متغير
    varMgr.define("add", lambdaName);
    
    // استدعاء: add(10, 20)
    auto calleeVar = std::make_unique<VariableExpr>("add");
    std::vector<ExprPtr> args;
    args.push_back(std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "10", Position())
    ));
    args.push_back(std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "20", Position())
    ));
    
    auto callExpr = std::make_unique<CallExpr>(
        std::move(calleeVar),
        std::move(args)
    );
    
    callExpr->accept(evaluator);
    Value result = evaluator.getResult();
    
    std::cout << "(AR) النتيجة: " << result.toInt() << "\n";
    std::cout << "(EN) Result: " << result.toInt() << "\n";
    
    assert(result.isInteger());
    assert(result.toInt() == 30);
    
    std::cout << "✅ PASSED: Lambda with two parameters\n";
}

// =========================================================================
// (AR) اختبار 4: استدعاء Lambda مباشر (inline)
// (EN) Test 4: Direct Lambda call (inline)
// =========================================================================

void test_lambda_inline_call() {
    std::cout << "\n=== Test 4: Direct Lambda call (inline) ===\n";
    
    // (lambda(x) { x * x })(5)
    // assert result == 25
    
    VariableManager varMgr;
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    ExpressionEvaluator evaluator(varMgr, funcMgr, scopeMgr, executor);
    
    // إنشاء Lambda: lambda(x) { x * x }
    std::vector<Parameter> params;
    params.emplace_back("x", DataType::INTEGER);
    
    // الجسم: x * x
    auto left = std::make_unique<VariableExpr>("x");
    auto right = std::make_unique<VariableExpr>("x");
    auto body = std::make_unique<BinaryExpr>(
        std::move(left),
        TokenType::OP_MULTIPLY,
        std::move(right)
    );
    
    auto lambdaExpr = std::make_unique<LambdaExpr>(
        std::move(params),
        std::move(body)
    );
    
    // استدعاء مباشر: (lambda)(5)
    std::vector<ExprPtr> args;
    args.push_back(std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "5", Position())
    ));
    
    auto callExpr = std::make_unique<CallExpr>(
        std::move(lambdaExpr),
        std::move(args)
    );
    
    callExpr->accept(evaluator);
    Value result = evaluator.getResult();
    
    std::cout << "(AR) النتيجة: " << result.toInt() << "\n";
    std::cout << "(EN) Result: " << result.toInt() << "\n";
    
    assert(result.isInteger());
    assert(result.toInt() == 25);
    
    std::cout << "✅ PASSED: Direct Lambda call\n";
}

// =========================================================================
// (AR) اختبار 5: Lambda مع تعبيرات معقدة
// (EN) Test 5: Lambda with complex expressions
// =========================================================================

void test_lambda_complex_expr() {
    std::cout << "\n=== Test 5: Lambda with complex expression ===\n";
    
    // var calc = lambda(x, y) { (x + y) * 2 };
    // var result = calc(3, 7);
    // assert result == 20
    
    VariableManager varMgr;
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    ExpressionEvaluator evaluator(varMgr, funcMgr, scopeMgr, executor);
    
    // إنشاء Lambda: lambda(x, y) { (x + y) * 2 }
    std::vector<Parameter> params;
    params.emplace_back("x", DataType::INTEGER);
    params.emplace_back("y", DataType::INTEGER);
    
    // الجسم: (x + y) * 2
    auto x = std::make_unique<VariableExpr>("x");
    auto y = std::make_unique<VariableExpr>("y");
    auto sum = std::make_unique<BinaryExpr>(
        std::move(x),
        TokenType::OP_PLUS,
        std::move(y)
    );
    auto two = std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "2", Position())
    );
    auto body = std::make_unique<BinaryExpr>(
        std::move(sum),
        TokenType::OP_MULTIPLY,
        std::move(two)
    );
    
    auto lambdaExpr = std::make_unique<LambdaExpr>(
        std::move(params),
        std::move(body)
    );
    
    // تقييم وحفظ
    lambdaExpr->accept(evaluator);
    varMgr.define("calc", evaluator.getResult());
    
    // استدعاء: calc(3, 7)
    auto calleeVar = std::make_unique<VariableExpr>("calc");
    std::vector<ExprPtr> args;
    args.push_back(std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "3", Position())
    ));
    args.push_back(std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "7", Position())
    ));
    
    auto callExpr = std::make_unique<CallExpr>(
        std::move(calleeVar),
        std::move(args)
    );
    
    callExpr->accept(evaluator);
    Value result = evaluator.getResult();
    
    std::cout << "(AR) النتيجة: " << result.toInt() << "\n";
    std::cout << "(EN) Result: " << result.toInt() << "\n";
    
    assert(result.isInteger());
    assert(result.toInt() == 20);
    
    std::cout << "✅ PASSED: Lambda with complex expression\n";
}

// =========================================================================
// (AR) الدالة الرئيسية / (EN) Main Function
// =========================================================================

int main() {
    std::cout << "========================================\n";
    std::cout << "(AR) اختبارات دوال Lambda البسيطة\n";
    std::cout << "(EN) Simple Lambda Functions Tests\n";
    std::cout << "========================================\n";
    
    try {
        test_lambda_no_params();
        test_lambda_one_param();
        test_lambda_two_params();
        test_lambda_inline_call();
        test_lambda_complex_expr();
        
        std::cout << "\n========================================\n";
        std::cout << "✅ (AR) جميع الاختبارات نجحت! (5/5)\n";
        std::cout << "✅ (EN) All tests passed! (5/5)\n";
        std::cout << "========================================\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ (AR) فشل الاختبار: " << e.what() << "\n";
        std::cerr << "❌ (EN) Test failed: " << e.what() << "\n";
        return 1;
    }
}
