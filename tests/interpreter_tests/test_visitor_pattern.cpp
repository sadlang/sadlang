/**
 * @file test_visitor_pattern.cpp
 * @brief (AR) اختبار نمط الزائر للـ AST
 * @brief (EN) Test Visitor pattern for AST
 * 
 * يختبر أن نمط الvoid test_visit_map_expr() {
    TEST_START("test_visit_map_expr");
  void test_visit_expr_void test_visit_var_decl() {
    TEST_START("test_visit_var_decl");
    
    // Create: let x = 10;
    auto init = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto decl = std::make_unique<VarDeclStmt>(
        "x",
        Data::DataType::INTEGER,
        std::move(init),
        false,  // not const
        Position()
    );   TEST_START("test_visit_expr_stmt");
    
    // Create: 42;
    auto expr = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "42", Position()));
    auto stmt = std::make_unique<ExprStmt>(std::move(expr));  // Create: {x: 10, y: 20}
    std::vector<MapPair> pairs;
    pairs.push_back(MapPair{
        std::make_unique<LiteralExpr>(Token(TokenType::IDENTIFIER, "x", Position())),
        std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()))
    });
    pairs.push_back(MapPair{
        std::make_unique<LiteralExpr>(Token(TokenType::IDENTIFIER, "y", Position())),
        std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "20", Position()))
    });
    
    auto map = std::make_unique<MapExpr>(std::move(pairs), Position());صحيح مع عقد AST المختلفة.
 * Tests that the Visitor pattern works correctly with different AST nodes.
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include "interpreter/ast_printer.h"
#include "parser/ast/expressions.h"
#include "parser/ast/statements.h"
#include "parser/ast/declarations.h"
#include "lexer/token.h"
#include <iostream>
#include <cassert>
#include <memory>

using namespace Sad;
using namespace Sad::AST;
using namespace Sad::Interpreter;
using namespace Sad::Lexer;

// ============================================================================
// Test Helpers / مساعدات الاختبار
// ============================================================================

int g_testsRun = 0;
int g_testsPassed = 0;
int g_testsFailed = 0;

void TEST_START(const std::string& testName) {
    g_testsRun++;
    std::cout << "  Testing: " << testName << " ... ";
}

void TEST_PASS() {
    g_testsPassed++;
    std::cout << "✅ PASS\n";
}

void TEST_FAIL(const std::string& reason) {
    g_testsFailed++;
    std::cout << "❌ FAIL: " << reason << "\n";
}

// ============================================================================
// Expression Tests / اختبارات التعابير
// ============================================================================

void test_visit_literal() {
    TEST_START("test_visit_literal");
    
    Token token(TokenType::NUMBER_INTEGER, "42", Position());
    auto lit = std::make_unique<LiteralExpr>(token);
    
    ASTPrinter printer;
    lit->accept(printer);
    
    std::string output = printer.getOutput();
    assert(output.find("Literal(42)") != std::string::npos);
    
    TEST_PASS();
}

void test_visit_variable() {
    TEST_START("test_visit_variable");
    
    auto var = std::make_unique<VariableExpr>("myVar", Position());
    
    ASTPrinter printer;
    var->accept(printer);
    
    std::string output = printer.getOutput();
    assert(output.find("Variable(myVar)") != std::string::npos);
    
    TEST_PASS();
}

void test_visit_binary_expr() {
    TEST_START("test_visit_binary_expr");
    
    // Create: 10 + 20
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "20", Position()));
    auto binary = std::make_unique<BinaryExpr>(
        std::move(left),
        TokenType::OP_PLUS,
        std::move(right),
        Position()
    );
    
    ASTPrinter printer;
    binary->accept(printer);
    
    std::string output = printer.getOutput();
    assert(output.find("BinaryExpr(+)") != std::string::npos);
    assert(output.find("Literal(10)") != std::string::npos);
    assert(output.find("Literal(20)") != std::string::npos);
    
    TEST_PASS();
}

void test_visit_unary_expr() {
    TEST_START("test_visit_unary_expr");
    
    // Create: -42
    auto operand = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "42", Position()));
    auto unary = std::make_unique<UnaryExpr>(
        TokenType::OP_MINUS,
        std::move(operand),
        Position()
    );
    
    ASTPrinter printer;
    unary->accept(printer);
    
    std::string output = printer.getOutput();
    assert(output.find("UnaryExpr(-)") != std::string::npos);
    assert(output.find("Literal(42)") != std::string::npos);
    
    TEST_PASS();
}

void test_visit_array_expr() {
    TEST_START("test_visit_array_expr");
    
    // Create: [1, 2, 3]
    ExprList elements;
    elements.push_back(std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position())));
    elements.push_back(std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "2", Position())));
    elements.push_back(std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "3", Position())));
    
    auto arr = std::make_unique<ArrayExpr>(std::move(elements), Position());
    
    ASTPrinter printer;
    arr->accept(printer);
    
    std::string output = printer.getOutput();
    assert(output.find("Array[3 elements]") != std::string::npos);
    assert(output.find("Literal(1)") != std::string::npos);
    assert(output.find("Literal(2)") != std::string::npos);
    assert(output.find("Literal(3)") != std::string::npos);
    
    TEST_PASS();
}

void test_visit_map_expr() {
    TEST_START("test_visit_map_expr");
    
    // Create: {x: 10, y: 20}
    std::vector<MapPair> pairs;
    pairs.push_back(MapPair(
        std::make_unique<LiteralExpr>(Token(TokenType::STRING_LITERAL, "x", Position())),
        std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()))
    ));
    pairs.push_back(MapPair(
        std::make_unique<LiteralExpr>(Token(TokenType::STRING_LITERAL, "y", Position())),
        std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "20", Position()))
    ));
    
    auto map = std::make_unique<MapExpr>(std::move(pairs), Position());
    
    ASTPrinter printer;
    map->accept(printer);
    
    std::string output = printer.getOutput();
    assert(output.find("Map[2 entries]") != std::string::npos);
    assert(output.find("Entry:") != std::string::npos);
    
    TEST_PASS();
}

void test_visit_call_expr() {
    TEST_START("test_visit_call_expr");
    
    // Create: func(10, 20)
    auto callee = std::make_unique<VariableExpr>("func", Position());
    
    ExprList args;
    args.push_back(std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position())));
    args.push_back(std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "20", Position())));
    
    auto call = std::make_unique<CallExpr>(
        std::move(callee),
        std::move(args),
        Position()
    );
    
    ASTPrinter printer;
    call->accept(printer);
    
    std::string output = printer.getOutput();
    assert(output.find("Call") != std::string::npos);
    assert(output.find("Callee:") != std::string::npos);
    assert(output.find("Variable(func)") != std::string::npos);
    assert(output.find("Arguments:") != std::string::npos);
    
    TEST_PASS();
}

// ============================================================================
// Statement Tests / اختبارات العبارات
// ============================================================================

void test_visit_expr_stmt() {
    TEST_START("test_visit_expr_stmt");
    
    // Create: 42;
    auto expr = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "42", Position()));
    auto stmt = std::make_unique<ExprStmt>(std::move(expr));
    
    ASTPrinter printer;
    stmt->accept(printer);
    
    std::string output = printer.getOutput();
    assert(output.find("ExprStmt") != std::string::npos);
    assert(output.find("Literal(42)") != std::string::npos);
    
    TEST_PASS();
}

void test_visit_var_decl() {
    TEST_START("test_visit_var_decl");
    
    // Create: let x = 10;
    auto init = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    auto decl = std::make_unique<VarDeclStmt>(
        "x",
        Data::DataType::INTEGER,
        std::move(init),
        false,  // isConst
        Position()
    );
    
    ASTPrinter printer;
    decl->accept(printer);
    
    std::string output = printer.getOutput();
    assert(output.find("VarDecl(x)") != std::string::npos);
    assert(output.find("Literal(10)") != std::string::npos);
    
    TEST_PASS();
}

void test_visit_block_stmt() {
    TEST_START("test_visit_block_stmt");
    
    // Create: { 1; 2; 3; }
    StmtList stmts;
    stmts.push_back(std::make_unique<ExprStmt>(
        std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position()))
    ));
    stmts.push_back(std::make_unique<ExprStmt>(
        std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "2", Position()))
    ));
    stmts.push_back(std::make_unique<ExprStmt>(
        std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "3", Position()))
    ));
    
    auto block = std::make_unique<BlockStmt>(std::move(stmts), Position());
    
    ASTPrinter printer;
    block->accept(printer);
    
    std::string output = printer.getOutput();
    assert(output.find("Block") != std::string::npos);
    assert(output.find("Literal(1)") != std::string::npos);
    assert(output.find("Literal(2)") != std::string::npos);
    assert(output.find("Literal(3)") != std::string::npos);
    
    TEST_PASS();
}

void test_visit_if_stmt() {
    TEST_START("test_visit_if_stmt");
    
    // Create: if (x > 0) { 1; } else { 2; }
    auto condition = std::make_unique<VariableExpr>("x", Position());
    
    auto thenBranch = std::make_unique<ExprStmt>(
        std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "1", Position()))
    );
    
    auto elseBranch = std::make_unique<ExprStmt>(
        std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "2", Position()))
    );
    
    auto ifStmt = std::make_unique<IfStmt>(
        std::move(condition),
        std::move(thenBranch),
        std::move(elseBranch),
        Position()
    );
    
    ASTPrinter printer;
    ifStmt->accept(printer);
    
    std::string output = printer.getOutput();
    assert(output.find("If") != std::string::npos);
    assert(output.find("Condition:") != std::string::npos);
    assert(output.find("Variable(x)") != std::string::npos);
    assert(output.find("Then:") != std::string::npos);
    assert(output.find("Else:") != std::string::npos);
    
    TEST_PASS();
}

void test_visit_while_stmt() {
    TEST_START("test_visit_while_stmt");
    
    // Create: while (true) { x; }
    auto condition = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_TRUE, "true", Position()));
    
    auto body = std::make_unique<ExprStmt>(
        std::make_unique<VariableExpr>("x", Position())
    );
    
    auto whileStmt = std::make_unique<WhileStmt>(
        std::move(condition),
        std::move(body),
        Position()
    );
    
    ASTPrinter printer;
    whileStmt->accept(printer);
    
    std::string output = printer.getOutput();
    assert(output.find("While") != std::string::npos);
    assert(output.find("Condition:") != std::string::npos);
    assert(output.find("Body:") != std::string::npos);
    assert(output.find("Variable(x)") != std::string::npos);
    
    TEST_PASS();
}

void test_visit_return_stmt() {
    TEST_START("test_visit_return_stmt");
    
    // Create: return 42;
    auto value = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "42", Position()));
    auto returnStmt = std::make_unique<ReturnStmt>(std::move(value), Position());
    
    ASTPrinter printer;
    returnStmt->accept(printer);
    
    std::string output = printer.getOutput();
    assert(output.find("Return") != std::string::npos);
    assert(output.find("Literal(42)") != std::string::npos);
    
    TEST_PASS();
}

// ============================================================================
// Complex Tests / اختبارات معقدة
// ============================================================================

void test_nested_expressions() {
    TEST_START("test_nested_expressions");
    
    // Create: (10 + 20) * 30
    auto left = std::make_unique<BinaryExpr>(
        std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position())),
        TokenType::OP_PLUS,
        std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "20", Position())),
        Position()
    );
    
    auto expr = std::make_unique<BinaryExpr>(
        std::move(left),
        TokenType::OP_MULTIPLY,
        std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "30", Position())),
        Position()
    );
    
    ASTPrinter printer;
    expr->accept(printer);
    
    std::string output = printer.getOutput();
    assert(output.find("BinaryExpr(*)") != std::string::npos);
    assert(output.find("BinaryExpr(+)") != std::string::npos);
    
    TEST_PASS();
}

// ============================================================================
// Main Test Runner / منفذ الاختبارات الرئيسي
// ============================================================================

int main() {
    std::cout << "╔════════════════════════════════════════════════════╗\n";
    std::cout << "║  VISITOR PATTERN TEST SUITE                      ║\n";
    std::cout << "║  (AR) مجموعة اختبار نمط الزائر                  ║\n";
    std::cout << "╚════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "=== Expression Visits ===\n";
    test_visit_literal();
    test_visit_variable();
    test_visit_binary_expr();
    test_visit_unary_expr();
    test_visit_array_expr();
    test_visit_map_expr();
    test_visit_call_expr();
    
    std::cout << "\n=== Statement Visits ===\n";
    test_visit_expr_stmt();
    test_visit_var_decl();
    test_visit_block_stmt();
    test_visit_if_stmt();
    test_visit_while_stmt();
    test_visit_return_stmt();
    
    std::cout << "\n=== Complex Tests ===\n";
    test_nested_expressions();
    
    // Print summary
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════╗\n";
    if (g_testsFailed == 0) {
        std::cout << "║   ✅ ALL TESTS PASSED!                            ║\n";
        std::cout << "║   ✅ جميع الاختبارات نجحت!                       ║\n";
    } else {
        std::cout << "║   ❌ SOME TESTS FAILED!                           ║\n";
        std::cout << "║   ❌ بعض الاختبارات فشلت!                        ║\n";
    }
    std::cout << "╚════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "📊 Results:\n";
    std::cout << "  ✅ Passed: " << g_testsPassed << "\n";
    std::cout << "  ❌ Failed: " << g_testsFailed << "\n";
    std::cout << "  📈 Total: " << g_testsRun << "\n";
    
    if (g_testsRun > 0) {
        double successRate = (g_testsPassed * 100.0) / g_testsRun;
        std::cout << "  📊 Success Rate: " << successRate << "%\n";
    }
    
    return (g_testsFailed == 0) ? 0 : 1;
}
