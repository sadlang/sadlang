/**
 * @file test_function_decl.cpp
 * @brief (AR) اختبارات تعريف الدوال
 * @brief (EN) Function Declaration Tests
 * 
 * اختبار تعريف الدوال وتسجيلها في FunctionManager
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include <iostream>
#include <cassert>
#include <string>
#include <memory>

#include "../../include/interpreter/visitors/statement_executor.h"
#include "../../include/parser/ast/declarations.h"
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

// =========================================================================
// (AR) اختبارات تعريف الدوال / (EN) Function Declaration Tests
// =========================================================================

void test_simple_function_declaration() {
    TEST_START("test_simple_function_declaration");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // func greet() { }
    std::vector<Parameter> params;
    auto body = std::make_unique<BlockStmt>(StmtList());
    
    auto funcDecl = std::make_unique<FunctionDecl>(
        "greet",
        std::move(params),
        DataType::UNKNOWN,
        std::move(body)
    );
    
    funcDecl->accept(executor);
    
    // التحقق من تسجيل الدالة
    assert(funcMgr.hasFunction("greet", 0));
    
    TEST_PASS();
}

void test_function_with_parameters() {
    TEST_START("test_function_with_parameters");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // func add(a, b) { }
    std::vector<Parameter> params;
    params.push_back(Parameter("a", DataType::INTEGER));
    params.push_back(Parameter("b", DataType::INTEGER));
    
    auto body = std::make_unique<BlockStmt>(StmtList());
    
    auto funcDecl = std::make_unique<FunctionDecl>(
        "add",
        std::move(params),
        DataType::INTEGER,
        std::move(body)
    );
    
    funcDecl->accept(executor);
    
    // التحقق من تسجيل الدالة بمعاملين
    assert(funcMgr.hasFunction("add", 2));
    
    auto func = funcMgr.getFunction("add", 2);
    assert(func != nullptr);
    assert(func->getName() == "add");
    assert(func->getParameterCount() == 2);
    
    TEST_PASS();
}

void test_multiple_functions() {
    TEST_START("test_multiple_functions");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // func func1() { }
    {
        std::vector<Parameter> params;
        auto body = std::make_unique<BlockStmt>(StmtList());
        auto funcDecl = std::make_unique<FunctionDecl>(
            "func1",
            std::move(params),
            DataType::UNKNOWN,
            std::move(body)
        );
        funcDecl->accept(executor);
    }
    
    // func func2(x) { }
    {
        std::vector<Parameter> params;
        params.push_back(Parameter("x", DataType::INTEGER));
        auto body = std::make_unique<BlockStmt>(StmtList());
        auto funcDecl = std::make_unique<FunctionDecl>(
            "func2",
            std::move(params),
            DataType::UNKNOWN,
            std::move(body)
        );
        funcDecl->accept(executor);
    }
    
    // func func3(x, y, z) { }
    {
        std::vector<Parameter> params;
        params.push_back(Parameter("x", DataType::INTEGER));
        params.push_back(Parameter("y", DataType::INTEGER));
        params.push_back(Parameter("z", DataType::INTEGER));
        auto body = std::make_unique<BlockStmt>(StmtList());
        auto funcDecl = std::make_unique<FunctionDecl>(
            "func3",
            std::move(params),
            DataType::UNKNOWN,
            std::move(body)
        );
        funcDecl->accept(executor);
    }
    
    // التحقق من تسجيل جميع الدوال
    assert(funcMgr.hasFunction("func1", 0));
    assert(funcMgr.hasFunction("func2", 1));
    assert(funcMgr.hasFunction("func3", 3));
    assert(funcMgr.getFunctionCount() == 3);
    
    TEST_PASS();
}

void test_function_with_body() {
    TEST_START("test_function_with_body");
    
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    FunctionManager funcMgr;
    ScopeManager scopeMgr;
    StatementExecutor executor(varMgr, funcMgr, scopeMgr);
    
    // func calculate() {
    //     var x = 10;
    //     return x;
    // }
    
    StmtList bodyStmts;
    
    // var x = 10
    auto init = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "10", Position()));
    bodyStmts.push_back(std::make_unique<VarDeclStmt>("x", DataType::INTEGER, std::move(init)));
    
    // return x
    auto returnExpr = std::make_unique<VariableExpr>("x");
    bodyStmts.push_back(std::make_unique<ReturnStmt>(std::move(returnExpr)));
    
    auto body = std::make_unique<BlockStmt>(std::move(bodyStmts));
    
    std::vector<Parameter> params;
    auto funcDecl = std::make_unique<FunctionDecl>(
        "calculate",
        std::move(params),
        DataType::INTEGER,
        std::move(body)
    );
    
    funcDecl->accept(executor);
    
    // التحقق من تسجيل الدالة
    assert(funcMgr.hasFunction("calculate", 0));
    
    auto func = funcMgr.getFunction("calculate", 0);
    assert(func != nullptr);
    assert(func->hasBody());
    
    TEST_PASS();
}

// =========================================================================
// (AR) الدالة الرئيسية / (EN) Main Function
// =========================================================================

int main() {
    std::cout << "\n========================================\n";
    std::cout << " اختبارات تعريف الدوال\n";
    std::cout << " Function Declaration Tests\n";
    std::cout << "========================================\n";
    
    int passed = 0;
    int total = 0;
    
    try {
        test_simple_function_declaration(); passed++; total++;
        test_function_with_parameters(); passed++; total++;
        test_multiple_functions(); passed++; total++;
        test_function_with_body(); passed++; total++;
        
    } catch (const std::exception& e) {
        std::cout << "❌ EXCEPTION: " << e.what() << "\n";
        return 1;
    }
    
    std::cout << "\n========================================\n";
    std::cout << "✅ ALL TESTS PASSED! (" << passed << "/" << total << ")\n";
    std::cout << "========================================\n";
    
    return 0;
}
