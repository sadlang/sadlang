/**
 * @file test_comprehension_simple.cpp
 * @brief (AR) اختبارات الاستيعاب القائمي البسيط
 * @brief (EN) Simple List Comprehension Tests
 * 
 * اختبارات لميزة List Comprehensions في لغة ص
 * Tests for List Comprehensions feature in S language
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
// (AR) اختبار 1: List comprehension بسيط
// (EN) Test 1: Simple list comprehension
// =========================================================================

void test_simple_comprehension() {
    std::cout << "\n=== Test 1: Simple list comprehension ===\n";
    
    // var numbers = [1, 2, 3, 4, 5];
    // var doubled = [x * 2 for x in numbers];
    // assert doubled == [2, 4, 6, 8, 10]
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    ExpressionEvaluator evaluator(varMgr, funcMgr, scopeMgr, executor);
    
    // إنشاء المصفوفة الأصلية: [1, 2, 3, 4, 5]
    std::vector<ExprPtr> numberExprs;
    for (int i = 1; i <= 5; ++i) {
        numberExprs.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, std::to_string(i), Position())
        ));
    }
    auto arrayExpr = std::make_unique<ArrayExpr>(std::move(numberExprs));
    
    // تقييم وتخزين في متغير
    arrayExpr->accept(evaluator);
    varMgr.define("numbers", evaluator.getResult());
    
    std::cout << "(AR) المصفوفة الأصلية: [1, 2, 3, 4, 5]\n";
    std::cout << "(EN) Original array: [1, 2, 3, 4, 5]\n";
    
    // إنشاء comprehension: [x * 2 for x in numbers]
    // Element: x * 2
    auto x_elem = std::make_unique<VariableExpr>("x");
    auto two = std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "2", Position())
    );
    auto element = std::make_unique<BinaryExpr>(
        std::move(x_elem),
        TokenType::OP_MULTIPLY,
        std::move(two)
    );
    
    // Iterable: numbers (variable)
    auto iterable = std::make_unique<VariableExpr>("numbers");
    
    // Comprehension
    auto compExpr = std::make_unique<ListComprehensionExpr>(
        std::move(element),
        "x",  // variable name
        std::move(iterable),
        nullptr  // no condition
    );
    
    // تقييم
    compExpr->accept(evaluator);
    Value result = evaluator.getResult();
    
    std::cout << "(AR) النتيجة: [";
    std::cout << "(EN) Result: [";
    for (size_t i = 0; i < result.size(); ++i) {
        std::cout << result[i].toInt();
        if (i < result.size() - 1) std::cout << ", ";
    }
    std::cout << "]\n";
    
    assert(result.isArray());
    assert(result.size() == 5);
    assert(result[0].toInt() == 2);
    assert(result[1].toInt() == 4);
    assert(result[2].toInt() == 6);
    assert(result[3].toInt() == 8);
    assert(result[4].toInt() == 10);
    
    std::cout << "✅ PASSED: Simple list comprehension\n";
}

// =========================================================================
// (AR) اختبار 2: List comprehension مع شرط
// (EN) Test 2: List comprehension with condition
// =========================================================================

void test_comprehension_with_filter() {
    std::cout << "\n=== Test 2: List comprehension with filter ===\n";
    
    // var numbers = [1, 2, 3, 4, 5, 6];
    // var evens = [x for x in numbers if x % 2 == 0];
    // assert evens == [2, 4, 6]
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    ExpressionEvaluator evaluator(varMgr, funcMgr, scopeMgr, executor);
    
    // إنشاء المصفوفة: [1, 2, 3, 4, 5, 6]
    std::vector<ExprPtr> numberExprs;
    for (int i = 1; i <= 6; ++i) {
        numberExprs.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, std::to_string(i), Position())
        ));
    }
    auto arrayExpr = std::make_unique<ArrayExpr>(std::move(numberExprs));
    arrayExpr->accept(evaluator);
    varMgr.define("numbers", evaluator.getResult());
    
    std::cout << "(AR) المصفوفة الأصلية: [1, 2, 3, 4, 5, 6]\n";
    std::cout << "(EN) Original array: [1, 2, 3, 4, 5, 6]\n";
    
    // إنشاء comprehension: [x for x in numbers if x % 2 == 0]
    // Element: x
    auto element = std::make_unique<VariableExpr>("x");
    
    // Iterable: numbers
    auto iterable = std::make_unique<VariableExpr>("numbers");
    
    // Condition: x % 2 == 0
    auto x_cond = std::make_unique<VariableExpr>("x");
    auto two_mod = std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "2", Position())
    );
    auto modulo = std::make_unique<BinaryExpr>(
        std::move(x_cond),
        TokenType::OP_MODULO,
        std::move(two_mod)
    );
    auto zero = std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "0", Position())
    );
    auto condition = std::make_unique<BinaryExpr>(
        std::move(modulo),
        TokenType::OP_EQUAL,
        std::move(zero)
    );
    
    // Comprehension
    auto compExpr = std::make_unique<ListComprehensionExpr>(
        std::move(element),
        "x",
        std::move(iterable),
        std::move(condition)
    );
    
    // تقييم
    compExpr->accept(evaluator);
    Value result = evaluator.getResult();
    
    std::cout << "(AR) النتيجة (الأعداد الزوجية): [";
    std::cout << "(EN) Result (even numbers): [";
    for (size_t i = 0; i < result.size(); ++i) {
        std::cout << result[i].toInt();
        if (i < result.size() - 1) std::cout << ", ";
    }
    std::cout << "]\n";
    
    assert(result.isArray());
    assert(result.size() == 3);
    assert(result[0].toInt() == 2);
    assert(result[1].toInt() == 4);
    assert(result[2].toInt() == 6);
    
    std::cout << "✅ PASSED: List comprehension with filter\n";
}

// =========================================================================
// (AR) اختبار 3: List comprehension مع تعبيرات معقدة
// (EN) Test 3: List comprehension with complex expressions
// =========================================================================

void test_comprehension_complex() {
    std::cout << "\n=== Test 3: List comprehension with complex expression ===\n";
    
    // var numbers = [1, 2, 3];
    // var result = [(x * 2) + 1 for x in numbers];
    // assert result == [3, 5, 7]
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    ExpressionEvaluator evaluator(varMgr, funcMgr, scopeMgr, executor);
    
    // إنشاء المصفوفة: [1, 2, 3]
    std::vector<ExprPtr> numberExprs;
    for (int i = 1; i <= 3; ++i) {
        numberExprs.push_back(std::make_unique<LiteralExpr>(
            Token(TokenType::NUMBER_INTEGER, std::to_string(i), Position())
        ));
    }
    auto arrayExpr = std::make_unique<ArrayExpr>(std::move(numberExprs));
    arrayExpr->accept(evaluator);
    varMgr.define("numbers", evaluator.getResult());
    
    std::cout << "(AR) المصفوفة الأصلية: [1, 2, 3]\n";
    std::cout << "(EN) Original array: [1, 2, 3]\n";
    
    // إنشاء comprehension: [(x * 2) + 1 for x in numbers]
    // Element: (x * 2) + 1
    auto x1 = std::make_unique<VariableExpr>("x");
    auto two = std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "2", Position())
    );
    auto mult = std::make_unique<BinaryExpr>(
        std::move(x1),
        TokenType::OP_MULTIPLY,
        std::move(two)
    );
    auto one = std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "1", Position())
    );
    auto element = std::make_unique<BinaryExpr>(
        std::move(mult),
        TokenType::OP_PLUS,
        std::move(one)
    );
    
    // Iterable: numbers
    auto iterable = std::make_unique<VariableExpr>("numbers");
    
    // Comprehension
    auto compExpr = std::make_unique<ListComprehensionExpr>(
        std::move(element),
        "x",
        std::move(iterable),
        nullptr
    );
    
    // تقييم
    compExpr->accept(evaluator);
    Value result = evaluator.getResult();
    
    std::cout << "(AR) النتيجة: [";
    std::cout << "(EN) Result: [";
    for (size_t i = 0; i < result.size(); ++i) {
        std::cout << result[i].toInt();
        if (i < result.size() - 1) std::cout << ", ";
    }
    std::cout << "]\n";
    
    assert(result.isArray());
    assert(result.size() == 3);
    assert(result[0].toInt() == 3);
    assert(result[1].toInt() == 5);
    assert(result[2].toInt() == 7);
    
    std::cout << "✅ PASSED: Complex list comprehension\n";
}

// =========================================================================
// (AR) الدالة الرئيسية / (EN) Main Function
// =========================================================================

int main() {
    std::cout << "========================================\n";
    std::cout << "(AR) اختبارات الاستيعاب القائمي\n";
    std::cout << "(EN) List Comprehension Tests\n";
    std::cout << "========================================\n";
    
    try {
        test_simple_comprehension();
        test_comprehension_with_filter();
        test_comprehension_complex();
        
        std::cout << "\n========================================\n";
        std::cout << "✅ (AR) جميع الاختبارات نجحت! (3/3)\n";
        std::cout << "✅ (EN) All tests passed! (3/3)\n";
        std::cout << "========================================\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ (AR) فشل الاختبار: " << e.what() << "\n";
        std::cerr << "❌ (EN) Test failed: " << e.what() << "\n";
        return 1;
    }
}
