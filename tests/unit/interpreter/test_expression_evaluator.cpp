/**
 * @file test_expression_evaluator.cpp
 * @brief (AR) اختبارات مُقيِّم التعابير
 * @brief (EN) Expression Evaluator Tests
 * 
 * اختبار شامل لجميع أنواع التعابير:
 * - القيم الحرفية (أرقام، نصوص، boolean)
 * - المتغيرات (قراءة، كتابة)
 * - العمليات الثنائية (حسابية، منطقية، مقارنة)
 * - العمليات الأحادية (سالب، نفي منطقي)
 * - المصفوفات والقواميس
 * - الفهرسة
 * 
 * الهدف: 40+ اختبار
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include <iostream>
#include <cassert>
#include <string>
#include <memory>

#include "../../include/interpreter/visitors/expression_evaluator.h"
#include "../../include/parser/ast/expressions.h"
#include "../../include/parser/ast/statements.h"
#include "../../include/lexer/token.h"
#include "../../include/data/managers/variable_manager.h"
#include "../../include/data/managers/function_manager.h"

using namespace Sad;
using namespace Sad::Interpreter;
using namespace Sad::AST;
using namespace Sad::Lexer;
using namespace Sad::Data;

// =========================================================================
// (AR) ماكروهات الاختبار / (EN) Test Macros
// =========================================================================

#define TEST_START(name) \
    std::cout << "\n========================================\n"; \
    std::cout << "TEST: " << name << "\n"; \
    std::cout << "========================================\n";

#define TEST_PASS() \
    std::cout << "✓ PASSED\n";

#define ASSERT_INT(value, expected) \
    assert(value.isInteger()); \
    assert(value.toInt() == expected);

#define ASSERT_DOUBLE(value, expected) \
    assert(value.isDouble()); \
    assert(std::abs(value.toDouble() - expected) < 0.0001);

#define ASSERT_STRING(value, expected) \
    assert(value.isString()); \
    assert(value.toString() == expected);

#define ASSERT_BOOL(value, expected) \
    assert(value.isBoolean()); \
    assert(value.toBool() == expected);

// =========================================================================
// (AR) الاختبارات الأساسية - القيم الحرفية / (EN) Basic Tests - Literals
// =========================================================================

void test_literal_integer() {
    TEST_START("test_literal_integer");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: 42
    auto expr = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "42", Position()));
    expr->accept(evaluator);
    
    Value result = evaluator.getResult();
    ASSERT_INT(result, 42);
    
    TEST_PASS();
}

void test_literal_double() {
    TEST_START("test_literal_double");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: 3.14
    auto expr = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_DOUBLE, "3.14", Position()));
    expr->accept(evaluator);
    
    Value result = evaluator.getResult();
    ASSERT_DOUBLE(result, 3.14);
    
    TEST_PASS();
}

void test_literal_string() {
    TEST_START("test_literal_string");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: "hello"
    auto expr = std::make_unique<LiteralExpr>(Token(TokenType::STRING_LITERAL, "hello", Position()));
    expr->accept(evaluator);
    
    Value result = evaluator.getResult();
    ASSERT_STRING(result, "hello");
    
    TEST_PASS();
}

void test_literal_bool_true() {
    TEST_START("test_literal_bool_true");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: true
    auto expr = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_TRUE, "true", Position()));
    expr->accept(evaluator);
    
    Value result = evaluator.getResult();
    ASSERT_BOOL(result, true);
    
    TEST_PASS();
}

void test_literal_bool_false() {
    TEST_START("test_literal_bool_false");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: false
    auto expr = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_FALSE, "false", Position()));
    expr->accept(evaluator);
    
    Value result = evaluator.getResult();
    ASSERT_BOOL(result, false);
    
    TEST_PASS();
}

void test_literal_null() {
    TEST_START("test_literal_null");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: null
    auto expr = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_NULL, "null", Position()));
    expr->accept(evaluator);
    
    Value result = evaluator.getResult();
    assert(result.isVoid());
    
    TEST_PASS();
}

// =========================================================================
// (AR) اختبارات المتغيرات / (EN) Variable Tests
// =========================================================================

void test_variable_read() {
    TEST_START("test_variable_read");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // تعريف متغير: x = 100
    varMgr.define("x", Value(100));
    
    // قراءة: x
    auto expr = std::make_unique<VariableExpr>("x", Position());
    expr->accept(evaluator);
    
    Value result = evaluator.getResult();
    ASSERT_INT(result, 100);
    
    TEST_PASS();
}

void test_variable_assign() {
    TEST_START("test_variable_assign");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // تعريف متغير
    varMgr.define("x", Value(10));
    
    // إسناد: x = 20
    auto valueExpr = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "20", Position()));
    auto assignExpr = std::make_unique<AssignExpr>("x", std::move(valueExpr), Position());
    assignExpr->accept(evaluator);
    
    // التحقق من النتيجة
    Value result = evaluator.getResult();
    ASSERT_INT(result, 20);
    
    // التحقق من أن المتغير تم تعديله
    Value varValue = varMgr.get("x");
    ASSERT_INT(varValue, 20);
    
    TEST_PASS();
}

// =========================================================================
// (AR) اختبارات العمليات الحسابية / (EN) Arithmetic Operations Tests
// =========================================================================

void test_addition_integer() {
    TEST_START("test_addition_integer");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: 10 + 20
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "20", Position()));
    auto expr = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_PLUS, std::move(right), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_INT(result, 30);
    
    TEST_PASS();
}

void test_subtraction() {
    TEST_START("test_subtraction");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: 50 - 30
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "50", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "30", Position()));
    auto expr = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_MINUS, std::move(right), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_INT(result, 20);
    
    TEST_PASS();
}

void test_multiplication() {
    TEST_START("test_multiplication");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: 7 * 6
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "7", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "6", Position()));
    auto expr = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_MULTIPLY, std::move(right), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_INT(result, 42);
    
    TEST_PASS();
}

void test_division() {
    TEST_START("test_division");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: 100 / 4
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "100", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "4", Position()));
    auto expr = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_DIVIDE, std::move(right), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_INT(result, 25);
    
    TEST_PASS();
}

void test_modulo() {
    TEST_START("test_modulo");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: 17 % 5
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "17", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "5", Position()));
    auto expr = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_MODULO, std::move(right), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_INT(result, 2);
    
    TEST_PASS();
}

void test_power() {
    TEST_START("test_power");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: 2 ** 10
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "2", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto expr = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_POWER, std::move(right), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_INT(result, 1024);
    
    TEST_PASS();
}

void test_string_concatenation() {
    TEST_START("test_string_concatenation");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: "Hello" + " World"
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::STRING_LITERAL, "Hello", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::STRING_LITERAL, " World", Position()));
    auto expr = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_PLUS, std::move(right), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_STRING(result, "Hello World");
    
    TEST_PASS();
}

void test_mixed_double_operations() {
    TEST_START("test_mixed_double_operations");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: 10 + 3.5
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_DOUBLE, "3.5", Position()));
    auto expr = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_PLUS, std::move(right), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_DOUBLE(result, 13.5);
    
    TEST_PASS();
}

// =========================================================================
// (AR) اختبارات عمليات المقارنة / (EN) Comparison Operations Tests
// =========================================================================

void test_equality() {
    TEST_START("test_equality");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: 10 == 10
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto expr = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_EQUAL, std::move(right), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_BOOL(result, true);
    
    TEST_PASS();
}

void test_inequality() {
    TEST_START("test_inequality");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: 10 != 20
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "20", Position()));
    auto expr = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_NOT_EQUAL, std::move(right), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_BOOL(result, true);
    
    TEST_PASS();
}

void test_less_than() {
    TEST_START("test_less_than");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: 5 < 10
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "5", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto expr = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_LESS, std::move(right), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_BOOL(result, true);
    
    TEST_PASS();
}

void test_greater_than() {
    TEST_START("test_greater_than");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: 15 > 10
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "15", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto expr = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_GREATER, std::move(right), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_BOOL(result, true);
    
    TEST_PASS();
}

void test_less_equal() {
    TEST_START("test_less_equal");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: 10 <= 10
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto expr = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_LESS_EQUAL, std::move(right), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_BOOL(result, true);
    
    TEST_PASS();
}

void test_greater_equal() {
    TEST_START("test_greater_equal");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: 20 >= 10
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "20", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto expr = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_GREATER_EQUAL, std::move(right), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_BOOL(result, true);
    
    TEST_PASS();
}

void test_string_equality() {
    TEST_START("test_string_equality");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: "hello" == "hello"
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::STRING_LITERAL, "hello", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::STRING_LITERAL, "hello", Position()));
    auto expr = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_EQUAL, std::move(right), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_BOOL(result, true);
    
    TEST_PASS();
}

// =========================================================================
// (AR) اختبارات العمليات المنطقية / (EN) Logical Operations Tests
// =========================================================================

void test_logical_and_true() {
    TEST_START("test_logical_and_true");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: true && true
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_TRUE, "true", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_TRUE, "true", Position()));
    auto expr = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_AND, std::move(right), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_BOOL(result, true);
    
    TEST_PASS();
}

void test_logical_and_false() {
    TEST_START("test_logical_and_false");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: true && false
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_TRUE, "true", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_FALSE, "false", Position()));
    auto expr = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_AND, std::move(right), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_BOOL(result, false);
    
    TEST_PASS();
}

void test_logical_or_true() {
    TEST_START("test_logical_or_true");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: true || false
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_TRUE, "true", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_FALSE, "false", Position()));
    auto expr = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_OR, std::move(right), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_BOOL(result, true);
    
    TEST_PASS();
}

void test_logical_or_false() {
    TEST_START("test_logical_or_false");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: false || false
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_FALSE, "false", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_FALSE, "false", Position()));
    auto expr = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_OR, std::move(right), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_BOOL(result, false);
    
    TEST_PASS();
}

// =========================================================================
// (AR) اختبارات العمليات الأحادية / (EN) Unary Operations Tests
// =========================================================================

void test_unary_minus_integer() {
    TEST_START("test_unary_minus_integer");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: -42
    auto operand = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "42", Position()));
    auto expr = std::make_unique<UnaryExpr>(TokenType::OP_MINUS, std::move(operand), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_INT(result, -42);
    
    TEST_PASS();
}

void test_unary_minus_double() {
    TEST_START("test_unary_minus_double");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: -3.14
    auto operand = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_DOUBLE, "3.14", Position()));
    auto expr = std::make_unique<UnaryExpr>(TokenType::OP_MINUS, std::move(operand), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_DOUBLE(result, -3.14);
    
    TEST_PASS();
}

void test_unary_not_true() {
    TEST_START("test_unary_not_true");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: !true
    auto operand = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_TRUE, "true", Position()));
    auto expr = std::make_unique<UnaryExpr>(TokenType::OP_NOT, std::move(operand), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_BOOL(result, false);
    
    TEST_PASS();
}

void test_unary_not_false() {
    TEST_START("test_unary_not_false");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: !false
    auto operand = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_FALSE, "false", Position()));
    auto expr = std::make_unique<UnaryExpr>(TokenType::OP_NOT, std::move(operand), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_BOOL(result, true);
    
    TEST_PASS();
}

// =========================================================================
// (AR) اختبارات المصفوفات / (EN) Array Tests
// =========================================================================

void test_array_creation_empty() {
    TEST_START("test_array_creation_empty");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: []
    std::vector<ExprPtr> elements;
    auto expr = std::make_unique<ArrayExpr>(std::move(elements), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    assert(result.isArray());
    assert(result.toArray().size() == 0);
    
    TEST_PASS();
}

void test_array_creation() {
    TEST_START("test_array_creation");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: [1, 2, 3]
    std::vector<ExprPtr> elements;
    elements.push_back(std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position())));
    elements.push_back(std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "2", Position())));
    elements.push_back(std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "3", Position())));
    auto expr = std::make_unique<ArrayExpr>(std::move(elements), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    assert(result.isArray());
    
    Value::ArrayType arr = result.toArray();
    assert(arr.size() == 3);
    ASSERT_INT(arr[0], 1);
    ASSERT_INT(arr[1], 2);
    ASSERT_INT(arr[2], 3);
    
    TEST_PASS();
}

void test_array_indexing() {
    TEST_START("test_array_indexing");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: [10, 20, 30][1]
    std::vector<ExprPtr> elements;
    elements.push_back(std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position())));
    elements.push_back(std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "20", Position())));
    elements.push_back(std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "30", Position())));
    auto array = std::make_unique<ArrayExpr>(std::move(elements), Position());
    
    auto index = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position()));
    auto expr = std::make_unique<IndexExpr>(std::move(array), std::move(index), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_INT(result, 20);
    
    TEST_PASS();
}

// =========================================================================
// (AR) اختبارات القواميس / (EN) Map Tests
// =========================================================================

void test_map_creation_empty() {
    TEST_START("test_map_creation_empty");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: {}
    std::vector<MapPair> pairs;
    auto expr = std::make_unique<MapExpr>(std::move(pairs), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    assert(result.isMap());
    assert(result.toMap().size() == 0);
    
    TEST_PASS();
}

void test_map_creation() {
    TEST_START("test_map_creation");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: {x: 10, y: 20}
    std::vector<MapPair> pairs;
    pairs.push_back(MapPair(
        std::make_unique<LiteralExpr>(Token(TokenType::STRING_LITERAL, "x", Position())),
        std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()))
    ));
    pairs.push_back(MapPair(
        std::make_unique<LiteralExpr>(Token(TokenType::STRING_LITERAL, "y", Position())),
        std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "20", Position()))
    ));
    auto expr = std::make_unique<MapExpr>(std::move(pairs), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    assert(result.isMap());
    
    Value::MapType map = result.toMap();
    assert(map.size() == 2);
    ASSERT_INT(map["x"], 10);
    ASSERT_INT(map["y"], 20);
    
    TEST_PASS();
}

void test_map_indexing() {
    TEST_START("test_map_indexing");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: {name: "Ali", age: 25}["name"]
    std::vector<MapPair> pairs;
    pairs.push_back(MapPair(
        std::make_unique<LiteralExpr>(Token(TokenType::STRING_LITERAL, "name", Position())),
        std::make_unique<LiteralExpr>(Token(TokenType::STRING_LITERAL, "Ali", Position()))
    ));
    pairs.push_back(MapPair(
        std::make_unique<LiteralExpr>(Token(TokenType::STRING_LITERAL, "age", Position())),
        std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "25", Position()))
    ));
    auto map = std::make_unique<MapExpr>(std::move(pairs), Position());
    
    auto index = std::make_unique<LiteralExpr>(Token(TokenType::STRING_LITERAL, "name", Position()));
    auto expr = std::make_unique<IndexExpr>(std::move(map), std::move(index), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_STRING(result, "Ali");
    
    TEST_PASS();
}

// =========================================================================
// (AR) اختبارات معقدة / (EN) Complex Tests
// =========================================================================

void test_nested_arithmetic() {
    TEST_START("test_nested_arithmetic");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // إنشاء: (10 + 20) * 3
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "20", Position()));
    auto sum = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_PLUS, std::move(right), Position());
    
    auto three = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "3", Position()));
    auto expr = std::make_unique<BinaryExpr>(std::move(sum), TokenType::OP_MULTIPLY, std::move(three), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_INT(result, 90);
    
    TEST_PASS();
}

void test_complex_expression_with_variables() {
    TEST_START("test_complex_expression_with_variables");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ExpressionEvaluator evaluator(varMgr, funcMgr);
    
    // تعريف: x = 5, y = 3
    varMgr.define("x", Value(5));
    varMgr.define("y", Value(3));
    
    // إنشاء: x * 2 + y
    auto x = std::make_unique<VariableExpr>("x", Position());
    auto two = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "2", Position()));
    auto mult = std::make_unique<BinaryExpr>(std::move(x), TokenType::OP_MULTIPLY, std::move(two), Position());
    
    auto y = std::make_unique<VariableExpr>("y", Position());
    auto expr = std::make_unique<BinaryExpr>(std::move(mult), TokenType::OP_PLUS, std::move(y), Position());
    
    expr->accept(evaluator);
    Value result = evaluator.getResult();
    ASSERT_INT(result, 13);  // 5 * 2 + 3 = 13
    
    TEST_PASS();
}

// =========================================================================
// (AR) الدالة الرئيسية / (EN) Main Function
// =========================================================================

int main() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << " اختبارات مُقيِّم التعابير\n";
    std::cout << " Expression Evaluator Tests\n";
    std::cout << "========================================\n";
    
    // القيم الحرفية / Literals
    test_literal_integer();
    test_literal_double();
    test_literal_string();
    test_literal_bool_true();
    test_literal_bool_false();
    test_literal_null();
    
    // المتغيرات / Variables
    test_variable_read();
    test_variable_assign();
    
    // العمليات الحسابية / Arithmetic
    test_addition_integer();
    test_subtraction();
    test_multiplication();
    test_division();
    test_modulo();
    test_power();
    test_string_concatenation();
    test_mixed_double_operations();
    
    // المقارنات / Comparisons
    test_equality();
    test_inequality();
    test_less_than();
    test_greater_than();
    test_less_equal();
    test_greater_equal();
    test_string_equality();
    
    // العمليات المنطقية / Logical
    test_logical_and_true();
    test_logical_and_false();
    test_logical_or_true();
    test_logical_or_false();
    
    // العمليات الأحادية / Unary
    test_unary_minus_integer();
    test_unary_minus_double();
    test_unary_not_true();
    test_unary_not_false();
    
    // المصفوفات / Arrays
    test_array_creation_empty();
    test_array_creation();
    test_array_indexing();
    
    // القواميس / Maps
    test_map_creation_empty();
    test_map_creation();
    test_map_indexing();
    
    // معقد / Complex
    test_nested_arithmetic();
    test_complex_expression_with_variables();
    
    std::cout << "\n========================================\n";
    std::cout << "✓ ALL TESTS PASSED! (37/37)\n";
    std::cout << "========================================\n\n";
    
    return 0;
}
