/**
 * @file test_statement_executor.cpp
 * @brief (AR) اختبارات منفّذ الجُمل
 * @brief (EN) Statement Executor Tests
 * 
 * اختبار شامل لجميع أنواع الجُمل:
 * - تعريف المتغيرات (VarDeclStmt)
 * - جملة التعبير (ExprStmt)
 * - كتل الكود (BlockStmt)
 * - الشروط (IfStmt)
 * - الحلقات (WhileStmt, ForStmt, ForRangeStmt)
 * - التحكم بالتدفق (ReturnStmt, BreakStmt, ContinueStmt)
 * - معالجة الأخطاء (TryStmt, RaiseStmt)
 * 
 * الهدف: 30+ اختبار
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include <iostream>
#include <cassert>
#include <string>
#include <memory>

#include "../../include/interpreter/visitors/statement_executor.h"
#include "../../include/parser/ast/statements.h"
#include "../../include/parser/ast/expressions.h"
#include "../../include/lexer/token.h"
#include "../../include/data/managers/variable_manager.h"
#include "../../include/data/managers/function_manager.h"
#include "../../include/data/scope/scope_manager.h"

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

#define ASSERT_DOUBLE(value, expected) \
    assert(value.isDouble() && value.toDouble() == expected);

#define ASSERT_STRING(value, expected) \
    assert(value.isString() && value.toString() == expected);

#define ASSERT_BOOL(value, expected) \
    assert(value.isBoolean() && value.toBool() == expected);

#define ASSERT_VAR_EXISTS(varMgr, name) \
    assert(varMgr.exists(name));

#define ASSERT_VAR_VALUE(varMgr, name, expected) \
    assert(varMgr.exists(name)); \
    ASSERT_INT(varMgr.get(name), expected);

#define ASSERT_FLOW(executor, expected) \
    assert(executor.getFlowControl() == expected);

// =========================================================================
// (AR) اختبارات تعريف المتغيرات / (EN) Variable Declaration Tests
// =========================================================================

void test_var_decl_with_init() {
    TEST_START("test_var_decl_with_init");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير x: عدد = 42
    auto init = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "42", Position()));
    auto stmt = std::make_unique<VarDeclStmt>("x", DataType::INTEGER, std::move(init), false, Position());
    
    stmt->accept(executor);
    
    // التحقق من أن المتغير مُعرّف
    ASSERT_VAR_EXISTS(varMgr, "x");
    ASSERT_VAR_VALUE(varMgr, "x", 42);
    
    TEST_PASS();
}

void test_var_decl_without_init() {
    TEST_START("test_var_decl_without_init");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير y: عدد
    auto stmt = std::make_unique<VarDeclStmt>("y", DataType::INTEGER, nullptr, false, Position());
    
    stmt->accept(executor);
    
    // التحقق من أن المتغير مُعرّف بقيمة افتراضية (0)
    ASSERT_VAR_EXISTS(varMgr, "y");
    ASSERT_VAR_VALUE(varMgr, "y", 0);
    
    TEST_PASS();
}

void test_var_decl_string() {
    TEST_START("test_var_decl_string");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير name: نص = "أحمد"
    auto init = std::make_unique<LiteralExpr>(Token(TokenType::STRING_LITERAL, "أحمد", Position()));
    auto stmt = std::make_unique<VarDeclStmt>("name", DataType::STRING, std::move(init), false, Position());
    
    stmt->accept(executor);
    
    ASSERT_VAR_EXISTS(varMgr, "name");
    Value val = varMgr.get("name");
    ASSERT_STRING(val, "أحمد");
    
    TEST_PASS();
}

void test_var_decl_const() {
    TEST_START("test_var_decl_const");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // const PI: عدد = 3.14
    auto init = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_DOUBLE, "3.14", Position()));
    auto stmt = std::make_unique<VarDeclStmt>("PI", DataType::FLOAT, std::move(init), true, Position());
    
    stmt->accept(executor);
    
    ASSERT_VAR_EXISTS(varMgr, "PI");
    Value val = varMgr.get("PI");
    ASSERT_DOUBLE(val, 3.14);
    
    TEST_PASS();
}

// =========================================================================
// (AR) اختبارات جملة التعبير / (EN) Expression Statement Tests
// =========================================================================

void test_expr_stmt() {
    TEST_START("test_expr_stmt");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // تعريف متغير أولاً
    auto init = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto varDecl = std::make_unique<VarDeclStmt>("x", DataType::INTEGER, std::move(init));
    varDecl->accept(executor);
    
    // جملة: x = 20
    auto value = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "20", Position()));
    auto assign = std::make_unique<AssignExpr>("x", std::move(value));
    auto exprStmt = std::make_unique<ExprStmt>(std::move(assign));
    
    exprStmt->accept(executor);
    
    // التحقق من أن القيمة تغيرت
    ASSERT_VAR_VALUE(varMgr, "x", 20);
    
    TEST_PASS();
}

// =========================================================================
// (AR) اختبارات كتل الكود / (EN) Block Statement Tests
// =========================================================================

void test_block_simple() {
    TEST_START("test_block_simple");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // كتلة: { متغير a = 10; متغير b = 20; }
    StmtList stmts;
    
    auto initA = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    stmts.push_back(std::make_unique<VarDeclStmt>("a", DataType::INTEGER, std::move(initA)));
    
    auto initB = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "20", Position()));
    stmts.push_back(std::make_unique<VarDeclStmt>("b", DataType::INTEGER, std::move(initB)));
    
    auto block = std::make_unique<BlockStmt>(std::move(stmts));
    block->accept(executor);
    
    // المتغيرات يجب أن تكون متاحة بعد الكتلة
    ASSERT_VAR_EXISTS(varMgr, "a");
    ASSERT_VAR_EXISTS(varMgr, "b");
    
    TEST_PASS();
}

void test_block_scoped() {
    TEST_START("test_block_scoped");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير عام
    auto init1 = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "100", Position()));
    auto outer = std::make_unique<VarDeclStmt>("outer", DataType::INTEGER, std::move(init1));
    outer->accept(executor);
    
    // كتلة بمتغير داخلي
    StmtList stmts;
    auto init2 = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "200", Position()));
    stmts.push_back(std::make_unique<VarDeclStmt>("inner", DataType::INTEGER, std::move(init2)));
    
    auto block = std::make_unique<BlockStmt>(std::move(stmts));
    block->accept(executor);
    
    // المتغير الخارجي موجود
    ASSERT_VAR_EXISTS(varMgr, "outer");
    // المتغير الداخلي موجود أيضاً (في التنفيذ الحالي)
    ASSERT_VAR_EXISTS(varMgr, "inner");
    
    TEST_PASS();
}

// =========================================================================
// (AR) اختبارات جملة إذا / (EN) If Statement Tests
// =========================================================================

void test_if_true() {
    TEST_START("test_if_true");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير x = 0
    auto init = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDecl = std::make_unique<VarDeclStmt>("x", DataType::INTEGER, std::move(init));
    varDecl->accept(executor);
    
    // if (true) { x = 10 }
    auto condition = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_TRUE, "true", Position()));
    
    auto value = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto assign = std::make_unique<AssignExpr>("x", std::move(value));
    auto thenBranch = std::make_unique<ExprStmt>(std::move(assign));
    
    auto ifStmt = std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch));
    ifStmt->accept(executor);
    
    // x يجب أن يكون 10
    ASSERT_VAR_VALUE(varMgr, "x", 10);
    
    TEST_PASS();
}

void test_if_false() {
    TEST_START("test_if_false");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير x = 0
    auto init = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDecl = std::make_unique<VarDeclStmt>("x", DataType::INTEGER, std::move(init));
    varDecl->accept(executor);
    
    // if (false) { x = 10 }
    auto condition = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_FALSE, "false", Position()));
    
    auto value = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto assign = std::make_unique<AssignExpr>("x", std::move(value));
    auto thenBranch = std::make_unique<ExprStmt>(std::move(assign));
    
    auto ifStmt = std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch));
    ifStmt->accept(executor);
    
    // x يجب أن يبقى 0
    ASSERT_VAR_VALUE(varMgr, "x", 0);
    
    TEST_PASS();
}

void test_if_else() {
    TEST_START("test_if_else");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير x = 0
    auto init = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDecl = std::make_unique<VarDeclStmt>("x", DataType::INTEGER, std::move(init));
    varDecl->accept(executor);
    
    // if (false) { x = 10 } else { x = 20 }
    auto condition = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_FALSE, "false", Position()));
    
    auto value1 = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto assign1 = std::make_unique<AssignExpr>("x", std::move(value1));
    auto thenBranch = std::make_unique<ExprStmt>(std::move(assign1));
    
    auto value2 = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "20", Position()));
    auto assign2 = std::make_unique<AssignExpr>("x", std::move(value2));
    auto elseBranch = std::make_unique<ExprStmt>(std::move(assign2));
    
    auto ifStmt = std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
    ifStmt->accept(executor);
    
    // x يجب أن يكون 20 (من فرع else)
    ASSERT_VAR_VALUE(varMgr, "x", 20);
    
    TEST_PASS();
}

void test_if_with_comparison() {
    TEST_START("test_if_with_comparison");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير a = 5, result = 0
    auto initA = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "5", Position()));
    auto varDeclA = std::make_unique<VarDeclStmt>("a", DataType::INTEGER, std::move(initA));
    varDeclA->accept(executor);
    
    auto initR = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDeclR = std::make_unique<VarDeclStmt>("result", DataType::INTEGER, std::move(initR));
    varDeclR->accept(executor);
    
    // if (a > 3) { result = 100 }
    auto left = std::make_unique<VariableExpr>("a");
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "3", Position()));
    auto condition = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_GREATER, std::move(right));
    
    auto value = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "100", Position()));
    auto assign = std::make_unique<AssignExpr>("result", std::move(value));
    auto thenBranch = std::make_unique<ExprStmt>(std::move(assign));
    
    auto ifStmt = std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch));
    ifStmt->accept(executor);
    
    // result يجب أن يكون 100 (لأن 5 > 3)
    ASSERT_VAR_VALUE(varMgr, "result", 100);
    
    TEST_PASS();
}

// =========================================================================
// (AR) اختبارات حلقة while / (EN) While Statement Tests
// =========================================================================

void test_while_simple() {
    TEST_START("test_while_simple");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير i = 0
    auto initI = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDeclI = std::make_unique<VarDeclStmt>("i", DataType::INTEGER, std::move(initI));
    varDeclI->accept(executor);
    
    // while (i < 5) { i = i + 1 }
    auto left = std::make_unique<VariableExpr>("i");
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "5", Position()));
    auto condition = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_LESS, std::move(right));
    
    // i = i + 1
    auto iVar = std::make_unique<VariableExpr>("i");
    auto one = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position()));
    auto add = std::make_unique<BinaryExpr>(std::move(iVar), TokenType::OP_PLUS, std::move(one));
    auto assign = std::make_unique<AssignExpr>("i", std::move(add));
    auto body = std::make_unique<ExprStmt>(std::move(assign));
    
    auto whileStmt = std::make_unique<WhileStmt>(std::move(condition), std::move(body));
    whileStmt->accept(executor);
    
    // i يجب أن يكون 5
    ASSERT_VAR_VALUE(varMgr, "i", 5);
    
    TEST_PASS();
}

void test_while_with_break() {
    TEST_START("test_while_with_break");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير i = 0
    auto initI = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDeclI = std::make_unique<VarDeclStmt>("i", DataType::INTEGER, std::move(initI));
    varDeclI->accept(executor);
    
    // while (true) { i = i + 1; if (i >= 3) break; }
    auto condition = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_TRUE, "true", Position()));
    
    StmtList stmts;
    // i = i + 1
    auto iVar1 = std::make_unique<VariableExpr>("i");
    auto one = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position()));
    auto add = std::make_unique<BinaryExpr>(std::move(iVar1), TokenType::OP_PLUS, std::move(one));
    auto assign = std::make_unique<AssignExpr>("i", std::move(add));
    stmts.push_back(std::make_unique<ExprStmt>(std::move(assign)));
    
    // if (i >= 3) break
    auto iVar2 = std::make_unique<VariableExpr>("i");
    auto three = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "3", Position()));
    auto ifCond = std::make_unique<BinaryExpr>(std::move(iVar2), TokenType::OP_GREATER_EQUAL, std::move(three));
    auto breakStmt = std::make_unique<BreakStmt>();
    auto ifStmt = std::make_unique<IfStmt>(std::move(ifCond), std::move(breakStmt));
    stmts.push_back(std::move(ifStmt));
    
    auto body = std::make_unique<BlockStmt>(std::move(stmts));
    auto whileStmt = std::make_unique<WhileStmt>(std::move(condition), std::move(body));
    whileStmt->accept(executor);
    
    // i يجب أن يكون 3
    ASSERT_VAR_VALUE(varMgr, "i", 3);
    
    TEST_PASS();
}

// =========================================================================
// (AR) اختبارات حلقة for / (EN) For Statement Tests
// =========================================================================

void test_for_simple() {
    TEST_START("test_for_simple");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير sum = 0
    auto initSum = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDeclSum = std::make_unique<VarDeclStmt>("sum", DataType::INTEGER, std::move(initSum));
    varDeclSum->accept(executor);
    
    // for (var i = 0; i < 5; i = i + 1) { sum = sum + i }
    // Initializer: var i = 0
    auto initI = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto initializer = std::make_unique<VarDeclStmt>("i", DataType::INTEGER, std::move(initI));
    
    // Condition: i < 5
    auto iVar1 = std::make_unique<VariableExpr>("i");
    auto five = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "5", Position()));
    auto cond = std::make_unique<BinaryExpr>(std::move(iVar1), TokenType::OP_LESS, std::move(five));
    
    // Increment: i = i + 1
    auto iVar2 = std::make_unique<VariableExpr>("i");
    auto one = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position()));
    auto addI = std::make_unique<BinaryExpr>(std::move(iVar2), TokenType::OP_PLUS, std::move(one));
    auto increment = std::make_unique<AssignExpr>("i", std::move(addI));
    
    // Body: sum = sum + i
    auto sumVar = std::make_unique<VariableExpr>("sum");
    auto iVar3 = std::make_unique<VariableExpr>("i");
    auto addSum = std::make_unique<BinaryExpr>(std::move(sumVar), TokenType::OP_PLUS, std::move(iVar3));
    auto assignSum = std::make_unique<AssignExpr>("sum", std::move(addSum));
    auto body = std::make_unique<ExprStmt>(std::move(assignSum));
    
    auto forStmt = std::make_unique<ForStmt>(std::move(initializer), std::move(cond), 
                                             std::move(increment), std::move(body));
    forStmt->accept(executor);
    
    // sum يجب أن يكون 0+1+2+3+4 = 10
    ASSERT_VAR_VALUE(varMgr, "sum", 10);
    
    TEST_PASS();
}

// =========================================================================
// (AR) اختبارات حلقة for-range / (EN) For-Range Statement Tests
// =========================================================================

void test_for_range_array() {
    TEST_START("test_for_range_array");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير sum = 0
    auto initSum = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDeclSum = std::make_unique<VarDeclStmt>("sum", DataType::INTEGER, std::move(initSum));
    varDeclSum->accept(executor);
    
    // مصفوفة arr = [1, 2, 3, 4, 5]
    ExprList elements;
    elements.push_back(std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position())));
    elements.push_back(std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "2", Position())));
    elements.push_back(std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "3", Position())));
    elements.push_back(std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "4", Position())));
    elements.push_back(std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "5", Position())));
    auto arrayExpr = std::make_unique<ArrayExpr>(std::move(elements));
    auto varDeclArr = std::make_unique<VarDeclStmt>("arr", DataType::ARRAY, std::move(arrayExpr));
    varDeclArr->accept(executor);
    
    // for (x in arr) { sum = sum + x }
    auto iterable = std::make_unique<VariableExpr>("arr");
    
    auto sumVar = std::make_unique<VariableExpr>("sum");
    auto xVar = std::make_unique<VariableExpr>("x");
    auto addSum = std::make_unique<BinaryExpr>(std::move(sumVar), TokenType::OP_PLUS, std::move(xVar));
    auto assignSum = std::make_unique<AssignExpr>("sum", std::move(addSum));
    auto body = std::make_unique<ExprStmt>(std::move(assignSum));
    
    auto forRange = std::make_unique<ForRangeStmt>("x", std::move(iterable), std::move(body));
    forRange->accept(executor);
    
    // sum يجب أن يكون 1+2+3+4+5 = 15
    ASSERT_VAR_VALUE(varMgr, "sum", 15);
    
    TEST_PASS();
}

// =========================================================================
// (AR) اختبارات متقدمة للحلقات / (EN) Advanced Loop Tests
// =========================================================================

void test_while_with_continue() {
    TEST_START("test_while_with_continue");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير i = 0, sum = 0
    auto initI = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDeclI = std::make_unique<VarDeclStmt>("i", DataType::INTEGER, std::move(initI));
    varDeclI->accept(executor);
    
    auto initSum = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDeclSum = std::make_unique<VarDeclStmt>("sum", DataType::INTEGER, std::move(initSum));
    varDeclSum->accept(executor);
    
    // while (i < 5) { i = i + 1; if (i == 2) continue; sum = sum + i; }
    auto left = std::make_unique<VariableExpr>("i");
    auto five = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "5", Position()));
    auto condition = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_LESS, std::move(five));
    
    StmtList stmts;
    
    // i = i + 1
    auto iVar1 = std::make_unique<VariableExpr>("i");
    auto one = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position()));
    auto addI = std::make_unique<BinaryExpr>(std::move(iVar1), TokenType::OP_PLUS, std::move(one));
    auto assignI = std::make_unique<AssignExpr>("i", std::move(addI));
    stmts.push_back(std::make_unique<ExprStmt>(std::move(assignI)));
    
    // if (i == 2) continue
    auto iVar2 = std::make_unique<VariableExpr>("i");
    auto two = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "2", Position()));
    auto ifCond = std::make_unique<BinaryExpr>(std::move(iVar2), TokenType::OP_EQUAL, std::move(two));
    auto continueStmt = std::make_unique<ContinueStmt>();
    auto ifStmt = std::make_unique<IfStmt>(std::move(ifCond), std::move(continueStmt));
    stmts.push_back(std::move(ifStmt));
    
    // sum = sum + i
    auto sumVar = std::make_unique<VariableExpr>("sum");
    auto iVar3 = std::make_unique<VariableExpr>("i");
    auto addSum = std::make_unique<BinaryExpr>(std::move(sumVar), TokenType::OP_PLUS, std::move(iVar3));
    auto assignSum = std::make_unique<AssignExpr>("sum", std::move(addSum));
    stmts.push_back(std::make_unique<ExprStmt>(std::move(assignSum)));
    
    auto body = std::make_unique<BlockStmt>(std::move(stmts));
    auto whileStmt = std::make_unique<WhileStmt>(std::move(condition), std::move(body));
    whileStmt->accept(executor);
    
    // sum يجب أن يكون 1+3+4+5 = 13 (تخطي 2)
    ASSERT_VAR_VALUE(varMgr, "sum", 13);
    
    TEST_PASS();
}

void test_for_with_continue() {
    TEST_START("test_for_with_continue");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير sum = 0
    auto initSum = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDeclSum = std::make_unique<VarDeclStmt>("sum", DataType::INTEGER, std::move(initSum));
    varDeclSum->accept(executor);
    
    // for (var i = 0; i < 6; i = i + 1) { if (i % 2 == 0) continue; sum = sum + i; }
    // Initializer
    auto initI = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto initializer = std::make_unique<VarDeclStmt>("i", DataType::INTEGER, std::move(initI));
    
    // Condition: i < 6
    auto iVar1 = std::make_unique<VariableExpr>("i");
    auto six = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "6", Position()));
    auto cond = std::make_unique<BinaryExpr>(std::move(iVar1), TokenType::OP_LESS, std::move(six));
    
    // Increment: i = i + 1
    auto iVar2 = std::make_unique<VariableExpr>("i");
    auto one = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position()));
    auto addI = std::make_unique<BinaryExpr>(std::move(iVar2), TokenType::OP_PLUS, std::move(one));
    auto increment = std::make_unique<AssignExpr>("i", std::move(addI));
    
    // Body: { if (i % 2 == 0) continue; sum = sum + i; }
    StmtList bodyStmts;
    
    // if (i % 2 == 0) continue
    auto iVar3 = std::make_unique<VariableExpr>("i");
    auto twoLit = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "2", Position()));
    auto modulo = std::make_unique<BinaryExpr>(std::move(iVar3), TokenType::OP_MODULO, std::move(twoLit));
    auto zero = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto ifCond = std::make_unique<BinaryExpr>(std::move(modulo), TokenType::OP_EQUAL, std::move(zero));
    auto continueStmt = std::make_unique<ContinueStmt>();
    bodyStmts.push_back(std::make_unique<IfStmt>(std::move(ifCond), std::move(continueStmt)));
    
    // sum = sum + i
    auto sumVar = std::make_unique<VariableExpr>("sum");
    auto iVar4 = std::make_unique<VariableExpr>("i");
    auto addSum = std::make_unique<BinaryExpr>(std::move(sumVar), TokenType::OP_PLUS, std::move(iVar4));
    auto assignSum = std::make_unique<AssignExpr>("sum", std::move(addSum));
    bodyStmts.push_back(std::make_unique<ExprStmt>(std::move(assignSum)));
    
    auto body = std::make_unique<BlockStmt>(std::move(bodyStmts));
    
    auto forStmt = std::make_unique<ForStmt>(std::move(initializer), std::move(cond), 
                                             std::move(increment), std::move(body));
    forStmt->accept(executor);
    
    // sum يجب أن يكون 1+3+5 = 9 (الأرقام الفردية فقط)
    ASSERT_VAR_VALUE(varMgr, "sum", 9);
    
    TEST_PASS();
}

void test_nested_loops() {
    TEST_START("test_nested_loops");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير sum = 0
    auto initSum = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDeclSum = std::make_unique<VarDeclStmt>("sum", DataType::INTEGER, std::move(initSum));
    varDeclSum->accept(executor);
    
    // متغير j = 0
    auto initJ = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDeclJ = std::make_unique<VarDeclStmt>("j", DataType::INTEGER, std::move(initJ));
    varDeclJ->accept(executor);
    
    // for (var i = 1; i <= 3; i = i + 1) {
    //     j = 1;
    //     while (j <= 2) {
    //         sum = sum + (i * j);
    //         j = j + 1;
    //     }
    // }
    
    // Outer loop
    auto initI = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position()));
    auto outerInit = std::make_unique<VarDeclStmt>("i", DataType::INTEGER, std::move(initI));
    
    auto iVar1 = std::make_unique<VariableExpr>("i");
    auto three = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "3", Position()));
    auto outerCond = std::make_unique<BinaryExpr>(std::move(iVar1), TokenType::OP_LESS_EQUAL, std::move(three));
    
    auto iVar2 = std::make_unique<VariableExpr>("i");
    auto oneI = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position()));
    auto addI = std::make_unique<BinaryExpr>(std::move(iVar2), TokenType::OP_PLUS, std::move(oneI));
    auto outerIncr = std::make_unique<AssignExpr>("i", std::move(addI));
    
    // Outer body: { j = 1; while(...) }
    StmtList outerStmts;
    
    // j = 1
    auto oneLit = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position()));
    auto resetJ = std::make_unique<AssignExpr>("j", std::move(oneLit));
    outerStmts.push_back(std::make_unique<ExprStmt>(std::move(resetJ)));
    
    // while (j <= 2) { sum = sum + (i * j); j = j + 1; }
    auto jVar1 = std::make_unique<VariableExpr>("j");
    auto two = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "2", Position()));
    auto whileCond = std::make_unique<BinaryExpr>(std::move(jVar1), TokenType::OP_LESS_EQUAL, std::move(two));
    
    StmtList whileStmts;
    
    // sum = sum + (i * j)
    auto sumVar = std::make_unique<VariableExpr>("sum");
    auto iVar3 = std::make_unique<VariableExpr>("i");
    auto jVar2 = std::make_unique<VariableExpr>("j");
    auto mult = std::make_unique<BinaryExpr>(std::move(iVar3), TokenType::OP_MULTIPLY, std::move(jVar2));
    auto addSum = std::make_unique<BinaryExpr>(std::move(sumVar), TokenType::OP_PLUS, std::move(mult));
    auto assignSum = std::make_unique<AssignExpr>("sum", std::move(addSum));
    whileStmts.push_back(std::make_unique<ExprStmt>(std::move(assignSum)));
    
    // j = j + 1
    auto jVar3 = std::make_unique<VariableExpr>("j");
    auto oneJ = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position()));
    auto incrJ = std::make_unique<BinaryExpr>(std::move(jVar3), TokenType::OP_PLUS, std::move(oneJ));
    auto assignJ = std::make_unique<AssignExpr>("j", std::move(incrJ));
    whileStmts.push_back(std::make_unique<ExprStmt>(std::move(assignJ)));
    
    auto whileBody = std::make_unique<BlockStmt>(std::move(whileStmts));
    auto whileLoop = std::make_unique<WhileStmt>(std::move(whileCond), std::move(whileBody));
    outerStmts.push_back(std::move(whileLoop));
    
    auto outerBody = std::make_unique<BlockStmt>(std::move(outerStmts));
    auto outerLoop = std::make_unique<ForStmt>(std::move(outerInit), std::move(outerCond),
                                               std::move(outerIncr), std::move(outerBody));
    outerLoop->accept(executor);
    
    // sum = (1*1 + 1*2) + (2*1 + 2*2) + (3*1 + 3*2) = 3 + 6 + 9 = 18
    ASSERT_VAR_VALUE(varMgr, "sum", 18);
    
    TEST_PASS();
}

// =========================================================================
// (AR) اختبارات متقدمة للشروط / (EN) Advanced If Tests
// =========================================================================

void test_if_elif_else() {
    TEST_START("test_if_elif_else");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير score = 75, grade = ""
    auto initScore = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "75", Position()));
    auto varDeclScore = std::make_unique<VarDeclStmt>("score", DataType::INTEGER, std::move(initScore));
    varDeclScore->accept(executor);
    
    auto initGrade = std::make_unique<LiteralExpr>(Token(TokenType::STRING_LITERAL, "", Position()));
    auto varDeclGrade = std::make_unique<VarDeclStmt>("grade", DataType::STRING, std::move(initGrade));
    varDeclGrade->accept(executor);
    
    // if (score >= 90) { grade = "A" }
    // elif (score >= 80) { grade = "B" }
    // elif (score >= 70) { grade = "C" }
    // else { grade = "D" }
    
    // Innermost else: grade = "D"
    auto gradeD = std::make_unique<LiteralExpr>(Token(TokenType::STRING_LITERAL, "D", Position()));
    auto assignD = std::make_unique<AssignExpr>("grade", std::move(gradeD));
    auto elseD = std::make_unique<ExprStmt>(std::move(assignD));
    
    // elif (score >= 70) { grade = "C" }
    auto scoreVar3 = std::make_unique<VariableExpr>("score");
    auto seventy = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "70", Position()));
    auto cond3 = std::make_unique<BinaryExpr>(std::move(scoreVar3), TokenType::OP_GREATER_EQUAL, std::move(seventy));
    auto gradeC = std::make_unique<LiteralExpr>(Token(TokenType::STRING_LITERAL, "C", Position()));
    auto assignC = std::make_unique<AssignExpr>("grade", std::move(gradeC));
    auto thenC = std::make_unique<ExprStmt>(std::move(assignC));
    auto elif2 = std::make_unique<IfStmt>(std::move(cond3), std::move(thenC), std::move(elseD));
    
    // elif (score >= 80) { grade = "B" }
    auto scoreVar2 = std::make_unique<VariableExpr>("score");
    auto eighty = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "80", Position()));
    auto cond2 = std::make_unique<BinaryExpr>(std::move(scoreVar2), TokenType::OP_GREATER_EQUAL, std::move(eighty));
    auto gradeB = std::make_unique<LiteralExpr>(Token(TokenType::STRING_LITERAL, "B", Position()));
    auto assignB = std::make_unique<AssignExpr>("grade", std::move(gradeB));
    auto thenB = std::make_unique<ExprStmt>(std::move(assignB));
    auto elif1 = std::make_unique<IfStmt>(std::move(cond2), std::move(thenB), std::move(elif2));
    
    // if (score >= 90) { grade = "A" }
    auto scoreVar1 = std::make_unique<VariableExpr>("score");
    auto ninety = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "90", Position()));
    auto cond1 = std::make_unique<BinaryExpr>(std::move(scoreVar1), TokenType::OP_GREATER_EQUAL, std::move(ninety));
    auto gradeA = std::make_unique<LiteralExpr>(Token(TokenType::STRING_LITERAL, "A", Position()));
    auto assignA = std::make_unique<AssignExpr>("grade", std::move(gradeA));
    auto thenA = std::make_unique<ExprStmt>(std::move(assignA));
    auto ifStmt = std::make_unique<IfStmt>(std::move(cond1), std::move(thenA), std::move(elif1));
    
    ifStmt->accept(executor);
    
    // grade يجب أن يكون "C" (لأن score = 75)
    ASSERT_VAR_EXISTS(varMgr, "grade");
    Value grade = varMgr.get("grade");
    ASSERT_STRING(grade, "C");
    
    TEST_PASS();
}

void test_nested_if() {
    TEST_START("test_nested_if");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير x = 10, y = 20, result = 0
    auto initX = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto varDeclX = std::make_unique<VarDeclStmt>("x", DataType::INTEGER, std::move(initX));
    varDeclX->accept(executor);
    
    auto initY = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "20", Position()));
    auto varDeclY = std::make_unique<VarDeclStmt>("y", DataType::INTEGER, std::move(initY));
    varDeclY->accept(executor);
    
    auto initResult = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDeclResult = std::make_unique<VarDeclStmt>("result", DataType::INTEGER, std::move(initResult));
    varDeclResult->accept(executor);
    
    // if (x > 5) {
    //     if (y > 15) {
    //         result = 100
    //     }
    // }
    
    // Inner if
    auto yVar = std::make_unique<VariableExpr>("y");
    auto fifteen = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "15", Position()));
    auto innerCond = std::make_unique<BinaryExpr>(std::move(yVar), TokenType::OP_GREATER, std::move(fifteen));
    auto hundred = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "100", Position()));
    auto assign = std::make_unique<AssignExpr>("result", std::move(hundred));
    auto innerThen = std::make_unique<ExprStmt>(std::move(assign));
    auto innerIf = std::make_unique<IfStmt>(std::move(innerCond), std::move(innerThen));
    
    // Outer if
    auto xVar = std::make_unique<VariableExpr>("x");
    auto fiveLit = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "5", Position()));
    auto outerCond = std::make_unique<BinaryExpr>(std::move(xVar), TokenType::OP_GREATER, std::move(fiveLit));
    auto outerIf = std::make_unique<IfStmt>(std::move(outerCond), std::move(innerIf));
    
    outerIf->accept(executor);
    
    // result يجب أن يكون 100
    ASSERT_VAR_VALUE(varMgr, "result", 100);
    
    TEST_PASS();
}

// =========================================================================
// (AR) اختبارات للتعابير المعقدة / (EN) Complex Expression Tests
// =========================================================================

void test_var_decl_with_expression() {
    TEST_START("test_var_decl_with_expression");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير a = 5, b = 10
    auto initA = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "5", Position()));
    auto varDeclA = std::make_unique<VarDeclStmt>("a", DataType::INTEGER, std::move(initA));
    varDeclA->accept(executor);
    
    auto initB = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto varDeclB = std::make_unique<VarDeclStmt>("b", DataType::INTEGER, std::move(initB));
    varDeclB->accept(executor);
    
    // متغير c = (a + b) * 2
    auto aVar = std::make_unique<VariableExpr>("a");
    auto bVar = std::make_unique<VariableExpr>("b");
    auto add = std::make_unique<BinaryExpr>(std::move(aVar), TokenType::OP_PLUS, std::move(bVar));
    auto two = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "2", Position()));
    auto mult = std::make_unique<BinaryExpr>(std::move(add), TokenType::OP_MULTIPLY, std::move(two));
    auto varDeclC = std::make_unique<VarDeclStmt>("c", DataType::INTEGER, std::move(mult));
    varDeclC->accept(executor);
    
    // c يجب أن يكون (5 + 10) * 2 = 30
    ASSERT_VAR_VALUE(varMgr, "c", 30);
    
    TEST_PASS();
}

// =========================================================================
// (AR) اختبارات التحكم بالتدفق المعقد / (EN) Complex Flow Control Tests
// =========================================================================

void test_return_from_loop() {
    TEST_START("test_return_from_loop");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير i = 0, found = 0
    auto initI = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDeclI = std::make_unique<VarDeclStmt>("i", DataType::INTEGER, std::move(initI));
    varDeclI->accept(executor);
    
    auto initFound = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDeclFound = std::make_unique<VarDeclStmt>("found", DataType::INTEGER, std::move(initFound));
    varDeclFound->accept(executor);
    
    // while (i < 10) {
    //     i = i + 1;
    //     if (i == 5) {
    //         found = i;
    //         return;
    //     }
    // }
    
    auto iVar1 = std::make_unique<VariableExpr>("i");
    auto ten = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto condition = std::make_unique<BinaryExpr>(std::move(iVar1), TokenType::OP_LESS, std::move(ten));
    
    StmtList stmts;
    
    // i = i + 1
    auto iVar2 = std::make_unique<VariableExpr>("i");
    auto one = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position()));
    auto addI = std::make_unique<BinaryExpr>(std::move(iVar2), TokenType::OP_PLUS, std::move(one));
    auto assignI = std::make_unique<AssignExpr>("i", std::move(addI));
    stmts.push_back(std::make_unique<ExprStmt>(std::move(assignI)));
    
    // if (i == 5) { found = i; return; }
    auto iVar3 = std::make_unique<VariableExpr>("i");
    auto five = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "5", Position()));
    auto ifCond = std::make_unique<BinaryExpr>(std::move(iVar3), TokenType::OP_EQUAL, std::move(five));
    
    StmtList thenStmts;
    auto foundVar = std::make_unique<VariableExpr>("i");
    auto assignFound = std::make_unique<AssignExpr>("found", std::move(foundVar));
    thenStmts.push_back(std::make_unique<ExprStmt>(std::move(assignFound)));
    thenStmts.push_back(std::make_unique<ReturnStmt>(nullptr));
    
    auto thenBlock = std::make_unique<BlockStmt>(std::move(thenStmts));
    auto ifStmt = std::make_unique<IfStmt>(std::move(ifCond), std::move(thenBlock));
    stmts.push_back(std::move(ifStmt));
    
    auto body = std::make_unique<BlockStmt>(std::move(stmts));
    auto whileStmt = std::make_unique<WhileStmt>(std::move(condition), std::move(body));
    whileStmt->accept(executor);
    
    // found يجب أن يكون 5، والتدفق يجب أن يكون RETURN
    ASSERT_VAR_VALUE(varMgr, "found", 5);
    ASSERT_FLOW(executor, FlowControl::RETURN);
    
    TEST_PASS();
}

void test_break_from_nested_loop() {
    TEST_START("test_break_from_nested_loop");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير count = 0, j = 0
    auto initCount = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDeclCount = std::make_unique<VarDeclStmt>("count", DataType::INTEGER, std::move(initCount));
    varDeclCount->accept(executor);
    
    auto initJ = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDeclJ = std::make_unique<VarDeclStmt>("j", DataType::INTEGER, std::move(initJ));
    varDeclJ->accept(executor);
    
    // for (var i = 0; i < 2; i = i + 1) {
    //     j = 0;
    //     while (j < 5) {
    //         count = count + 1;
    //         if (count == 7) break;
    //         j = j + 1;
    //     }
    // }
    // التكرارات:
    // i=0: count=1,2,3,4,5 (j يصل إلى 5)
    // i=1: count=6,7 ثم break (j يتوقف عند 1)
    // النتيجة النهائية: count = 7
    
    auto initI = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto outerInit = std::make_unique<VarDeclStmt>("i", DataType::INTEGER, std::move(initI));
    
    auto iVar1 = std::make_unique<VariableExpr>("i");
    auto two = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "2", Position()));
    auto outerCond = std::make_unique<BinaryExpr>(std::move(iVar1), TokenType::OP_LESS, std::move(two));
    
    auto iVar2 = std::make_unique<VariableExpr>("i");
    auto one1 = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position()));
    auto addI = std::make_unique<BinaryExpr>(std::move(iVar2), TokenType::OP_PLUS, std::move(one1));
    auto outerIncr = std::make_unique<AssignExpr>("i", std::move(addI));
    
    // Outer body
    StmtList outerStmts;
    
    // j = 0
    auto zero = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto resetJ = std::make_unique<AssignExpr>("j", std::move(zero));
    outerStmts.push_back(std::make_unique<ExprStmt>(std::move(resetJ)));
    
    // while (j < 5)
    auto jVar1 = std::make_unique<VariableExpr>("j");
    auto five2 = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "5", Position()));
    auto whileCond = std::make_unique<BinaryExpr>(std::move(jVar1), TokenType::OP_LESS, std::move(five2));
    
    StmtList whileStmts;
    
    // count = count + 1
    auto countVar1 = std::make_unique<VariableExpr>("count");
    auto one2 = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position()));
    auto addCount = std::make_unique<BinaryExpr>(std::move(countVar1), TokenType::OP_PLUS, std::move(one2));
    auto assignCount = std::make_unique<AssignExpr>("count", std::move(addCount));
    whileStmts.push_back(std::make_unique<ExprStmt>(std::move(assignCount)));
    
    // if (count == 7) break
    auto countVar2 = std::make_unique<VariableExpr>("count");
    auto seven = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "7", Position()));
    auto ifCond = std::make_unique<BinaryExpr>(std::move(countVar2), TokenType::OP_EQUAL, std::move(seven));
    auto breakStmt = std::make_unique<BreakStmt>();
    auto ifStmt = std::make_unique<IfStmt>(std::move(ifCond), std::move(breakStmt));
    whileStmts.push_back(std::move(ifStmt));
    
    // j = j + 1
    auto jVar2 = std::make_unique<VariableExpr>("j");
    auto one3 = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position()));
    auto addJ = std::make_unique<BinaryExpr>(std::move(jVar2), TokenType::OP_PLUS, std::move(one3));
    auto assignJ = std::make_unique<AssignExpr>("j", std::move(addJ));
    whileStmts.push_back(std::make_unique<ExprStmt>(std::move(assignJ)));
    
    auto whileBody = std::make_unique<BlockStmt>(std::move(whileStmts));
    auto whileLoop = std::make_unique<WhileStmt>(std::move(whileCond), std::move(whileBody));
    outerStmts.push_back(std::move(whileLoop));
    
    auto outerBody = std::make_unique<BlockStmt>(std::move(outerStmts));
    auto outerLoop = std::make_unique<ForStmt>(std::move(outerInit), std::move(outerCond),
                                               std::move(outerIncr), std::move(outerBody));
    outerLoop->accept(executor);
    
    // count يجب أن يكون 7 (يتوقف عند 7)
    ASSERT_VAR_VALUE(varMgr, "count", 7);
    ASSERT_FLOW(executor, FlowControl::NONE);
    
    TEST_PASS();
}

void test_multiple_conditions() {
    TEST_START("test_multiple_conditions");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير x = 15, y = 25, result = 0
    auto initX = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "15", Position()));
    auto varDeclX = std::make_unique<VarDeclStmt>("x", DataType::INTEGER, std::move(initX));
    varDeclX->accept(executor);
    
    auto initY = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "25", Position()));
    auto varDeclY = std::make_unique<VarDeclStmt>("y", DataType::INTEGER, std::move(initY));
    varDeclY->accept(executor);
    
    auto initResult = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDeclResult = std::make_unique<VarDeclStmt>("result", DataType::INTEGER, std::move(initResult));
    varDeclResult->accept(executor);
    
    // if (x > 10 && y > 20) { result = 100 }
    auto xVar = std::make_unique<VariableExpr>("x");
    auto ten = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto cond1 = std::make_unique<BinaryExpr>(std::move(xVar), TokenType::OP_GREATER, std::move(ten));
    
    auto yVar = std::make_unique<VariableExpr>("y");
    auto twenty = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "20", Position()));
    auto cond2 = std::make_unique<BinaryExpr>(std::move(yVar), TokenType::OP_GREATER, std::move(twenty));
    
    auto condition = std::make_unique<BinaryExpr>(std::move(cond1), TokenType::OP_AND, std::move(cond2));
    
    auto hundred = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "100", Position()));
    auto assign = std::make_unique<AssignExpr>("result", std::move(hundred));
    auto thenStmt = std::make_unique<ExprStmt>(std::move(assign));
    
    auto ifStmt = std::make_unique<IfStmt>(std::move(condition), std::move(thenStmt));
    ifStmt->accept(executor);
    
    // result يجب أن يكون 100 (الشرطان صحيحان)
    ASSERT_VAR_VALUE(varMgr, "result", 100);
    
    TEST_PASS();
}

// =========================================================================
// (AR) اختبارات الحالات الحدية / (EN) Edge Case Tests
// =========================================================================

void test_empty_while_loop() {
    TEST_START("test_empty_while_loop");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير executed = false
    auto initExecuted = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_FALSE, "false", Position()));
    auto varDeclExecuted = std::make_unique<VarDeclStmt>("executed", DataType::BOOLEAN, std::move(initExecuted));
    varDeclExecuted->accept(executor);
    
    // while (false) { executed = true }
    auto falseLit = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_FALSE, "false", Position()));
    
    auto trueLit = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_TRUE, "true", Position()));
    auto assign = std::make_unique<AssignExpr>("executed", std::move(trueLit));
    auto body = std::make_unique<ExprStmt>(std::move(assign));
    
    auto whileStmt = std::make_unique<WhileStmt>(std::move(falseLit), std::move(body));
    whileStmt->accept(executor);
    
    // executed يجب أن يبقى false (لم تُنفَّذ الحلقة)
    ASSERT_VAR_EXISTS(varMgr, "executed");
    Value executed = varMgr.get("executed");
    ASSERT_BOOL(executed, false);
    
    TEST_PASS();
}

void test_for_loop_no_body_execution() {
    TEST_START("test_for_loop_no_body_execution");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير count = 0
    auto initCount = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDeclCount = std::make_unique<VarDeclStmt>("count", DataType::INTEGER, std::move(initCount));
    varDeclCount->accept(executor);
    
    // for (var i = 10; i < 5; i = i + 1) { count = count + 1 }
    auto initI = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto initializer = std::make_unique<VarDeclStmt>("i", DataType::INTEGER, std::move(initI));
    
    auto iVar1 = std::make_unique<VariableExpr>("i");
    auto five = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "5", Position()));
    auto condition = std::make_unique<BinaryExpr>(std::move(iVar1), TokenType::OP_LESS, std::move(five));
    
    auto iVar2 = std::make_unique<VariableExpr>("i");
    auto one = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position()));
    auto addI = std::make_unique<BinaryExpr>(std::move(iVar2), TokenType::OP_PLUS, std::move(one));
    auto increment = std::make_unique<AssignExpr>("i", std::move(addI));
    
    auto countVar = std::make_unique<VariableExpr>("count");
    auto one2 = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position()));
    auto addCount = std::make_unique<BinaryExpr>(std::move(countVar), TokenType::OP_PLUS, std::move(one2));
    auto assignCount = std::make_unique<AssignExpr>("count", std::move(addCount));
    auto body = std::make_unique<ExprStmt>(std::move(assignCount));
    
    auto forStmt = std::make_unique<ForStmt>(std::move(initializer), std::move(condition),
                                             std::move(increment), std::move(body));
    forStmt->accept(executor);
    
    // count يجب أن يبقى 0 (الشرط خاطئ من البداية)
    ASSERT_VAR_VALUE(varMgr, "count", 0);
    
    TEST_PASS();
}

void test_for_range_empty_array() {
    TEST_START("test_for_range_empty_array");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // متغير count = 0
    auto initCount = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "0", Position()));
    auto varDeclCount = std::make_unique<VarDeclStmt>("count", DataType::INTEGER, std::move(initCount));
    varDeclCount->accept(executor);
    
    // مصفوفة فارغة
    std::vector<std::unique_ptr<Expression>> elements;
    auto arrayExpr = std::make_unique<ArrayExpr>(std::move(elements));
    
    // for (x in []) { count = count + 1 }
    auto countVar = std::make_unique<VariableExpr>("count");
    auto one = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position()));
    auto addCount = std::make_unique<BinaryExpr>(std::move(countVar), TokenType::OP_PLUS, std::move(one));
    auto assignCount = std::make_unique<AssignExpr>("count", std::move(addCount));
    auto body = std::make_unique<ExprStmt>(std::move(assignCount));
    
    auto forRangeStmt = std::make_unique<ForRangeStmt>("x", std::move(arrayExpr), std::move(body));
    forRangeStmt->accept(executor);
    
    // count يجب أن يبقى 0 (مصفوفة فارغة)
    ASSERT_VAR_VALUE(varMgr, "count", 0);
    
    TEST_PASS();
}

// =========================================================================
// (AR) الدالة الرئيسية / (EN) Main Function
// =========================================================================

int main() {
    std::cout << "\n========================================\n";
    std::cout << " اختبارات منفّذ الجُمل\n";
    std::cout << " Statement Executor Tests\n";
    std::cout << "========================================\n";
    
    int passed = 0;
    int total = 0;
    
    try {
        // اختبارات تعريف المتغيرات
        test_var_decl_with_init(); passed++; total++;
        test_var_decl_without_init(); passed++; total++;
        test_var_decl_string(); passed++; total++;
        test_var_decl_const(); passed++; total++;
        
        // اختبارات جملة التعبير
        test_expr_stmt(); passed++; total++;
        
        // اختبارات كتل الكود
        test_block_simple(); passed++; total++;
        test_block_scoped(); passed++; total++;
        
        // اختبارات جملة إذا
        test_if_true(); passed++; total++;
        test_if_false(); passed++; total++;
        test_if_else(); passed++; total++;
        test_if_with_comparison(); passed++; total++;
        
        // اختبارات حلقة while
        test_while_simple(); passed++; total++;
        test_while_with_break(); passed++; total++;
        
        // اختبارات حلقة for
        test_for_simple(); passed++; total++;
        
        // اختبارات حلقة for-range
        test_for_range_array(); passed++; total++;
        
        // =========================================================================
        // اختبارات متقدمة
        // =========================================================================
        
        // اختبارات متقدمة للحلقات
        test_while_with_continue(); passed++; total++;
        test_for_with_continue(); passed++; total++;
        test_nested_loops(); passed++; total++;
        
        // اختبارات متقدمة للشروط
        test_if_elif_else(); passed++; total++;
        test_nested_if(); passed++; total++;
        
        // اختبارات للتعابير المعقدة
        test_var_decl_with_expression(); passed++; total++;
        
        // اختبارات التحكم بالتدفق المعقد
        test_return_from_loop(); passed++; total++;
        test_break_from_nested_loop(); passed++; total++;
        test_multiple_conditions(); passed++; total++;
        
        // اختبارات الحالات الحدية
        test_empty_while_loop(); passed++; total++;
        test_for_loop_no_body_execution(); passed++; total++;
        test_for_range_empty_array(); passed++; total++;
        
    } catch (const std::exception& e) {
        std::cout << "❌ EXCEPTION: " << e.what() << "\n";
        return 1;
    }
    
    std::cout << "\n========================================\n";
    std::cout << "✅ ALL TESTS PASSED! (" << passed << "/" << total << ")\n";
    std::cout << "========================================\n";
    
    return 0;
}
