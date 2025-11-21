/**
 * @file test_dict_comp.cpp
 * @brief (AR) اختبارات الاستيعاب القاموسي
 * @brief (EN) Dictionary Comprehension Tests
 * 
 * اختبارات لميزة Dictionary Comprehensions في لغة ص
 * Tests for Dictionary Comprehensions feature in S language
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
// (AR) اختبار 1: Dict comprehension بسيط
// (EN) Test 1: Simple dict comprehension
// =========================================================================

void test_simple_dict_comp() {
    std::cout << "\n=== Test 1: Simple dict comprehension ===\n";
    
    // var numbers = [1, 2, 3];
    // var squares = {x: x * x for x in numbers};
    // assert squares == {1: 1, 2: 4, 3: 9}
    
    VariableManager varMgr;
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
    
    // إنشاء dict comprehension: {x: x * x for x in numbers}
    // Key: x (as string)
    auto keyExpr = std::make_unique<VariableExpr>("x");
    
    // Value: x * x
    auto x1 = std::make_unique<VariableExpr>("x");
    auto x2 = std::make_unique<VariableExpr>("x");
    auto valueExpr = std::make_unique<BinaryExpr>(
        std::move(x1),
        TokenType::OP_MULTIPLY,
        std::move(x2)
    );
    
    // Iterable: numbers
    auto iterable = std::make_unique<VariableExpr>("numbers");
    
    // Dict Comprehension
    auto dictComp = std::make_unique<DictComprehensionExpr>(
        std::move(keyExpr),
        std::move(valueExpr),
        "x",
        std::move(iterable),
        nullptr  // no condition
    );
    
    // تقييم
    dictComp->accept(evaluator);
    Value result = evaluator.getResult();
    
    std::cout << "(AR) النتيجة: {";
    std::cout << "(EN) Result: {";
    
    assert(result.isMap());
    assert(result.size() == 3);
    
    // التحقق من القيم
    assert(result["1"].toInt() == 1);
    assert(result["2"].toInt() == 4);
    assert(result["3"].toInt() == 9);
    
    std::cout << "1: 1, 2: 4, 3: 9}\n";
    
    std::cout << "✅ PASSED: Simple dict comprehension\n";
}

// =========================================================================
// (AR) اختبار 2: Dict comprehension مع شرط
// (EN) Test 2: Dict comprehension with filter
// =========================================================================

void test_dict_comp_with_filter() {
    std::cout << "\n=== Test 2: Dict comprehension with filter ===\n";
    
    // var numbers = [1, 2, 3, 4, 5, 6];
    // var evens = {x: x * 10 for x in numbers if x % 2 == 0};
    // assert evens == {2: 20, 4: 40, 6: 60}
    
    VariableManager varMgr;
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
    
    // Key: x
    auto keyExpr = std::make_unique<VariableExpr>("x");
    
    // Value: x * 10
    auto x_val = std::make_unique<VariableExpr>("x");
    auto ten = std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "10", Position())
    );
    auto valueExpr = std::make_unique<BinaryExpr>(
        std::move(x_val),
        TokenType::OP_MULTIPLY,
        std::move(ten)
    );
    
    // Iterable: numbers
    auto iterable = std::make_unique<VariableExpr>("numbers");
    
    // Condition: x % 2 == 0
    auto x_cond = std::make_unique<VariableExpr>("x");
    auto two = std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "2", Position())
    );
    auto modulo = std::make_unique<BinaryExpr>(
        std::move(x_cond),
        TokenType::OP_MODULO,
        std::move(two)
    );
    auto zero = std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "0", Position())
    );
    auto condition = std::make_unique<BinaryExpr>(
        std::move(modulo),
        TokenType::OP_EQUAL,
        std::move(zero)
    );
    
    // Dict Comprehension
    auto dictComp = std::make_unique<DictComprehensionExpr>(
        std::move(keyExpr),
        std::move(valueExpr),
        "x",
        std::move(iterable),
        std::move(condition)
    );
    
    // تقييم
    dictComp->accept(evaluator);
    Value result = evaluator.getResult();
    
    std::cout << "(AR) النتيجة (الأعداد الزوجية فقط): {";
    std::cout << "(EN) Result (even numbers only): {";
    
    assert(result.isMap());
    assert(result.size() == 3);
    assert(result["2"].toInt() == 20);
    assert(result["4"].toInt() == 40);
    assert(result["6"].toInt() == 60);
    
    std::cout << "2: 20, 4: 40, 6: 60}\n";
    
    std::cout << "✅ PASSED: Dict comprehension with filter\n";
}

// =========================================================================
// (AR) اختبار 3: Dict comprehension مع تعبيرات معقدة
// (EN) Test 3: Dict comprehension with complex expressions
// =========================================================================

void test_dict_comp_complex() {
    std::cout << "\n=== Test 3: Dict comprehension with complex expressions ===\n";
    
    // var numbers = [1, 2, 3];
    // var result = {x * 2: (x + 1) * 10 for x in numbers};
    // assert result == {2: 20, 4: 30, 6: 40}
    
    VariableManager varMgr;
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
    
    // Key: x * 2
    auto x_key = std::make_unique<VariableExpr>("x");
    auto two_key = std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "2", Position())
    );
    auto keyExpr = std::make_unique<BinaryExpr>(
        std::move(x_key),
        TokenType::OP_MULTIPLY,
        std::move(two_key)
    );
    
    // Value: (x + 1) * 10
    auto x_val = std::make_unique<VariableExpr>("x");
    auto one = std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "1", Position())
    );
    auto sum = std::make_unique<BinaryExpr>(
        std::move(x_val),
        TokenType::OP_PLUS,
        std::move(one)
    );
    auto ten = std::make_unique<LiteralExpr>(
        Token(TokenType::NUMBER_INTEGER, "10", Position())
    );
    auto valueExpr = std::make_unique<BinaryExpr>(
        std::move(sum),
        TokenType::OP_MULTIPLY,
        std::move(ten)
    );
    
    // Iterable: numbers
    auto iterable = std::make_unique<VariableExpr>("numbers");
    
    // Dict Comprehension
    auto dictComp = std::make_unique<DictComprehensionExpr>(
        std::move(keyExpr),
        std::move(valueExpr),
        "x",
        std::move(iterable),
        nullptr
    );
    
    // تقييم
    dictComp->accept(evaluator);
    Value result = evaluator.getResult();
    
    std::cout << "(AR) النتيجة: {";
    std::cout << "(EN) Result: {";
    
    assert(result.isMap());
    assert(result.size() == 3);
    assert(result["2"].toInt() == 20);
    assert(result["4"].toInt() == 30);
    assert(result["6"].toInt() == 40);
    
    std::cout << "2: 20, 4: 30, 6: 40}\n";
    
    std::cout << "✅ PASSED: Complex dict comprehension\n";
}

// =========================================================================
// (AR) الدالة الرئيسية / (EN) Main Function
// =========================================================================

int main() {
    std::cout << "========================================\n";
    std::cout << "(AR) اختبارات الاستيعاب القاموسي\n";
    std::cout << "(EN) Dictionary Comprehension Tests\n";
    std::cout << "========================================\n";
    
    try {
        test_simple_dict_comp();
        test_dict_comp_with_filter();
        test_dict_comp_complex();
        
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
