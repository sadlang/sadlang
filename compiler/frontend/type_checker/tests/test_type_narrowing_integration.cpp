/**
 * @file test_type_narrowing_integration.cpp
 * @brief Integration tests for Type Narrowing with Type Checker
 * @details Tests the integration of TypeNarrowingAnalyzer with TypeChecker
 * 
 * Phase 1.3.5.3: Semantic Analyzer Integration
 * 
 * @author Sad Language Team
 * @date 2026-01-03
 */

#include "../include/type_checker.h"
#include "../../../../compiler/type_system/include/union_type.h"
#include "../../../../compiler/type_system/include/optional_type.h"
#include "../../../../compiler/type_system/include/type_registry.h"
#include "../../../../include/parser/ast/expressions.h"
#include "../../../../include/parser/ast/statements.h"
#include <cassert>
#include <iostream>
#include <memory>

using namespace Sad::TypeChecker;
using namespace Sad::TypeSystem;
using namespace Sad::AST;
using namespace Sad::Lexer;

// ════════════════════════════════════════════════════════════════════════════
// Helper Functions
// ════════════════════════════════════════════════════════════════════════════

void printHeader(const std::string& titleEn, const std::string& titleAr) {
    std::cout << "\n╔════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  " << titleAr << "\n";
    std::cout << "║  " << titleEn << "\n";
    std::cout << "╚════════════════════════════════════════════════════════════════════════╝\n";
}

void printTest(const std::string& nameEn, const std::string& nameAr) {
    std::cout << "\nاختبار (Test): " << nameAr << " / " << nameEn << "\n";
}

void printSuccess(const std::string& msg) {
    std::cout << "✓ نجح (Success): " << msg << "\n";
}

// ════════════════════════════════════════════════════════════════════════════
// Test 1: TypeChecker has Type Narrowing enabled by default
// ════════════════════════════════════════════════════════════════════════════

void testTypeNarrowingEnabledByDefault() {
    printTest("Type Narrowing Enabled by Default", "Type Narrowing مفعّل افتراضياً");
    
    auto context = std::make_shared<TypeContext>();
    TypeChecker checker(context);
    
    assert(checker.isTypeNarrowingEnabled());
    
    printSuccess("Type Narrowing مفعّل افتراضياً في TypeChecker");
}

// ════════════════════════════════════════════════════════════════════════════
// Test 2: Can enable/disable Type Narrowing
// ════════════════════════════════════════════════════════════════════════════

void testEnableDisableTypeNarrowing() {
    printTest("Enable/Disable Type Narrowing", "تفعيل/تعطيل Type Narrowing");
    
    auto context = std::make_shared<TypeContext>();
    TypeChecker checker(context);
    
    // افتراضياً مفعّل / Default: enabled
    assert(checker.isTypeNarrowingEnabled());
    
    // تعطيل / Disable
    checker.setTypeNarrowingEnabled(false);
    assert(!checker.isTypeNarrowingEnabled());
    
    // تفعيل / Enable
    checker.setTypeNarrowingEnabled(true);
    assert(checker.isTypeNarrowingEnabled());
    
    printSuccess("يمكن تفعيل/تعطيل Type Narrowing بنجاح");
}

// ════════════════════════════════════════════════════════════════════════════
// Test 3: checkIfStmt works without Type Narrowing
// ════════════════════════════════════════════════════════════════════════════

void testCheckIfStmtWithoutNarrowing() {
    printTest("checkIfStmt without Type Narrowing", "checkIfStmt بدون Type Narrowing");
    
    auto context = std::make_shared<TypeContext>();
    TypeChecker checker(context);
    
    // تعطيل Type Narrowing / Disable Type Narrowing
    checker.setTypeNarrowingEnabled(false);
    
    // Create simple if statement: if (true) { }
    auto trueToken = Token(TokenType::LITERAL_TRUE, "true", Position());
    auto condition = std::make_unique<LiteralExpr>(trueToken);
    
    StmtList emptyStmts;
    auto thenBranch = std::make_unique<BlockStmt>(std::move(emptyStmts), Position());
    
    auto ifStmt = std::make_unique<IfStmt>(
        std::move(condition),
        std::move(thenBranch),
        nullptr,
        Position()
    );
    
    // التحقق من if statement / Check if statement
    bool result = checker.checkStmt(ifStmt.get());
    
    assert(result);
    assert(!checker.hasErrors());
    
    printSuccess("checkIfStmt يعمل بشكل صحيح بدون Type Narrowing");
}

// ════════════════════════════════════════════════════════════════════════════
// Test 4: checkIfStmt works with Type Narrowing (no guard)
// ════════════════════════════════════════════════════════════════════════════

void testCheckIfStmtWithNarrowingNoGuard() {
    printTest("checkIfStmt with Type Narrowing (no guard)", 
              "checkIfStmt مع Type Narrowing (بدون حارس)");
    
    auto context = std::make_shared<TypeContext>();
    TypeChecker checker(context);
    
    // Type Narrowing مفعّل افتراضياً / Type Narrowing enabled by default
    assert(checker.isTypeNarrowingEnabled());
    
    // Create if statement with non-guard condition: if (5 > 3) { }
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "5", Position()));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "3", Position()));
    auto condition = std::make_unique<BinaryExpr>(
        std::move(left),
        TokenType::OP_GREATER,
        std::move(right),
        Position()
    );
    
    StmtList emptyStmts2;
    auto thenBranch = std::make_unique<BlockStmt>(std::move(emptyStmts2), Position());
    
    auto ifStmt = std::make_unique<IfStmt>(
        std::move(condition),
        std::move(thenBranch),
        nullptr,
        Position()
    );
    
    // التحقق من if statement / Check if statement
    bool result = checker.checkStmt(ifStmt.get());
    
    assert(result);
    // قد يكون هناك أخطاء نوع أخرى لكن لا يجب أن يتعطل / May have other type errors but shouldn't crash
    
    printSuccess("checkIfStmt يعمل مع Type Narrowing عندما لا يوجد حارس");
}

// ════════════════════════════════════════════════════════════════════════════
// Test 5: checkIfStmt with Type Narrowing (with guard)
// ════════════════════════════════════════════════════════════════════════════

void testCheckIfStmtWithNarrowingWithGuard() {
    printTest("checkIfStmt with Type Narrowing (with guard)", 
              "checkIfStmt مع Type Narrowing (مع حارس)");
    
    auto context = std::make_shared<TypeContext>();
    TypeChecker checker(context);
    
    // Type Narrowing مفعّل افتراضياً / Type Narrowing enabled by default
    assert(checker.isTypeNarrowingEnabled());
    
    // Create if statement: if (value != null) { }
    auto varExpr = std::make_unique<VariableExpr>("value", Position());
    auto nullExpr = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_NULL, "null", Position()));
    
    auto condition = std::make_unique<BinaryExpr>(
        std::move(varExpr),
        TokenType::OP_NOT_EQUAL,
        std::move(nullExpr),
        Position()
    );
    
    StmtList emptyStmts3;
    auto thenBranch = std::make_unique<BlockStmt>(std::move(emptyStmts3), Position());
    
    auto ifStmt = std::make_unique<IfStmt>(
        std::move(condition),
        std::move(thenBranch),
        nullptr,
        Position()
    );
    
    // التحقق من if statement / Check if statement
    bool result = checker.checkStmt(ifStmt.get());
    
    // سينجح حتى لو لم يكن "value" معرّفاً / Will succeed even if "value" undefined
    // لأننا نختبر التكامل فقط / Because we're testing integration only
    assert(result);
    
    printSuccess("checkIfStmt يعمل مع Type Narrowing عندما يوجد حارس");
}

// ════════════════════════════════════════════════════════════════════════════
// Test 6: checkIfStmt with else branch
// ════════════════════════════════════════════════════════════════════════════

void testCheckIfStmtWithElseBranch() {
    printTest("checkIfStmt with else branch and Type Narrowing", 
              "checkIfStmt مع فرع else وType Narrowing");
    
    auto context = std::make_shared<TypeContext>();
    TypeChecker checker(context);
    
    // Create if-else statement: if (value != null) { } else { }
    auto varExpr = std::make_unique<VariableExpr>("value", Position());
    auto nullExpr = std::make_unique<LiteralExpr>(Token(TokenType::LITERAL_NULL, "null", Position()));
    
    auto condition = std::make_unique<BinaryExpr>(
        std::move(varExpr),
        TokenType::OP_NOT_EQUAL,
        std::move(nullExpr),
        Position()
    );
    
    StmtList emptyStmts3;
    auto thenBranch = std::make_unique<BlockStmt>(std::move(emptyStmts3), Position());
    StmtList emptyStmts4;
    auto elseBranch = std::make_unique<BlockStmt>(std::move(emptyStmts4), Position());
    
    auto ifStmt = std::make_unique<IfStmt>(
        std::move(condition),
        std::move(thenBranch),
        std::move(elseBranch),
        Position()
    );
    
    // التحقق من if statement / Check if statement
    bool result = checker.checkStmt(ifStmt.get());
    
    assert(result);
    
    printSuccess("checkIfStmt يعمل مع فرع else وType Narrowing");
}

// ════════════════════════════════════════════════════════════════════════════
// Main Test Runner
// ════════════════════════════════════════════════════════════════════════════

int main() {
    printHeader("Type Narrowing Integration Tests", 
                "اختبارات تكامل Type Narrowing مع Type Checker");
    std::cout << "Phase 1.3.5.3 - Semantic Analyzer Integration\n";
    std::cout << "Testing TypeNarrowingAnalyzer integration with TypeChecker\n\n";
    
    try {
        testTypeNarrowingEnabledByDefault();
        testEnableDisableTypeNarrowing();
        testCheckIfStmtWithoutNarrowing();
        testCheckIfStmtWithNarrowingNoGuard();
        testCheckIfStmtWithNarrowingWithGuard();
        testCheckIfStmtWithElseBranch();
        
        std::cout << "\n╔════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║  ✓✓✓ جميع الاختبارات نجحت! (All Tests Passed!) ✓✓✓\n";
        std::cout << "╚════════════════════════════════════════════════════════════════════════╝\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ فشل الاختبار (Test Failed): " << e.what() << "\n";
        return 1;
    }
}
