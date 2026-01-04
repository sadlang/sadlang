/**
 * @file test_type_narrowing_ast.cpp
 * @brief Integration tests for Type Narrowing with AST
 * @author Sad Language Team
 * @date 2026-01-03
 */

#include "type_narrowing.h"
#include "type_guard.h"
#include "union_type.h"
#include "optional_type.h"
#include "type_registry.h"
#include "../../../include/parser/ast/expressions.h"
#include "../../../include/parser/ast/statements.h"
#include "../../../include/lexer/token.h"
#include <cassert>
#include <iostream>
#include <memory>

using namespace Sad::TypeSystem;
using namespace Sad::AST;
using namespace Sad::Lexer;

// ════════════════════════════════════════════════════════════════════════════
// Helper Functions / دوال مساعدة
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
// Test 1: Extract Guard from != null
// ════════════════════════════════════════════════════════════════════════════

void testExtractNotNullGuard() {
    printTest("Extract NotNull Guard from != null", "استخراج حارس NotNull من != null");
    
    // Create AST: value != null
    auto varExpr = std::make_unique<VariableExpr>("value", Position());
    auto nullToken = Token(TokenType::LITERAL_NULL, "null", Position());
    auto nullExpr = std::make_unique<LiteralExpr>(nullToken);
    
    auto binaryExpr = std::make_unique<BinaryExpr>(
        std::move(varExpr),
        TokenType::OP_NOT_EQUAL,
        std::move(nullExpr),
        Position()
    );
    
    // Extract guard
    TypeNarrowingAnalyzer analyzer;
    std::string varName;
    TypeGuardPtr guard = analyzer.extractGuardFromCondition(binaryExpr.get(), varName);
    
    // Verify
    assert(guard != nullptr);
    assert(varName == "value");
    assert(guard->getKind() == TypeGuardKind::NotNull);
    
    printSuccess("استخرج حارس NotNull بنجاح من 'value != null'");
}

// ════════════════════════════════════════════════════════════════════════════
// Test 2: Extract Guard from == null
// ════════════════════════════════════════════════════════════════════════════

void testExtractNullGuard() {
    printTest("Extract Null Guard from == null", "استخراج حارس Null من == null");
    
    // Create AST: value == null
    auto varExpr = std::make_unique<VariableExpr>("value", Position());
    auto nullToken = Token(TokenType::LITERAL_NULL, "null", Position());
    auto nullExpr = std::make_unique<LiteralExpr>(nullToken);
    
    auto binaryExpr = std::make_unique<BinaryExpr>(
        std::move(varExpr),
        TokenType::OP_EQUAL,
        std::move(nullExpr),
        Position()
    );
    
    // Extract guard
    TypeNarrowingAnalyzer analyzer;
    std::string varName;
    TypeGuardPtr guard = analyzer.extractGuardFromCondition(binaryExpr.get(), varName);
    
    // Verify
    assert(guard != nullptr);
    assert(varName == "value");
    assert(guard->getKind() == TypeGuardKind::Null);
    
    printSuccess("استخرج حارس Null بنجاح من 'value == null'");
}

// ════════════════════════════════════════════════════════════════════════════
// Test 3: Extract Guard from null != var (reversed order)
// ════════════════════════════════════════════════════════════════════════════

void testExtractNotNullGuardReversed() {
    printTest("Extract NotNull Guard from null != var", "استخراج حارس NotNull من null != var");
    
    // Create AST: null != value
    auto nullToken = Token(TokenType::LITERAL_NULL, "null", Position());
    auto nullExpr = std::make_unique<LiteralExpr>(nullToken);
    auto varExpr = std::make_unique<VariableExpr>("value", Position());
    
    auto binaryExpr = std::make_unique<BinaryExpr>(
        std::move(nullExpr),
        TokenType::OP_NOT_EQUAL,
        std::move(varExpr),
        Position()
    );
    
    // Extract guard
    TypeNarrowingAnalyzer analyzer;
    std::string varName;
    TypeGuardPtr guard = analyzer.extractGuardFromCondition(binaryExpr.get(), varName);
    
    // Verify
    assert(guard != nullptr);
    assert(varName == "value");
    assert(guard->getKind() == TypeGuardKind::NotNull);
    
    printSuccess("استخرج حارس NotNull بنجاح من 'null != value'");
}

// ════════════════════════════════════════════════════════════════════════════
// Test 4: Extract Guard from !var
// ════════════════════════════════════════════════════════════════════════════

void testExtractNullGuardFromNot() {
    printTest("Extract Null Guard from !var", "استخراج حارس Null من !var");
    
    // Create AST: !value
    auto varExpr = std::make_unique<VariableExpr>("value", Position());
    auto unaryExpr = std::make_unique<UnaryExpr>(
        TokenType::OP_NOT,
        std::move(varExpr),
        Position()
    );
    
    // Extract guard
    TypeNarrowingAnalyzer analyzer;
    std::string varName;
    TypeGuardPtr guard = analyzer.extractGuardFromCondition(unaryExpr.get(), varName);
    
    // Verify
    assert(guard != nullptr);
    assert(varName == "value");
    assert(guard->getKind() == TypeGuardKind::Null);
    
    printSuccess("استخرج حارس Null بنجاح من '!value'");
}

// ════════════════════════════════════════════════════════════════════════════
// Test 5: No Guard for non-null expressions
// ════════════════════════════════════════════════════════════════════════════

void testNoGuardForOtherExpressions() {
    printTest("No Guard for other expressions", "لا حارس للتعابير الأخرى");
    
    // Create AST: x + 10 (no guard)
    auto varExpr = std::make_unique<VariableExpr>("x", Position());
    auto numToken = Token(TokenType::NUMBER_INTEGER, "10", Position());
    auto numExpr = std::make_unique<LiteralExpr>(numToken);
    
    auto binaryExpr = std::make_unique<BinaryExpr>(
        std::move(varExpr),
        TokenType::OP_PLUS,
        std::move(numExpr),
        Position()
    );
    
    // Extract guard
    TypeNarrowingAnalyzer analyzer;
    std::string varName;
    TypeGuardPtr guard = analyzer.extractGuardFromCondition(binaryExpr.get(), varName);
    
    // Verify
    assert(guard == nullptr);
    assert(varName.empty());
    
    printSuccess("لا يوجد حارس للتعبير 'x + 10' (كما متوقع)");
}

// ════════════════════════════════════════════════════════════════════════════
// Test 6: Apply extracted guard to Optional type
// ════════════════════════════════════════════════════════════════════════════

void testApplyExtractedGuard() {
    printTest("Apply extracted guard to Optional", "تطبيق الحارس المستخرج على Optional");
    
    // Setup: String? type
    auto& registry = TypeRegistry::getInstance();
    auto stringType = registry.getStringType();
    auto optionalType = std::make_shared<OptionalType>(stringType);
    
    // Create AST: value != null
    auto varExpr = std::make_unique<VariableExpr>("value", Position());
    auto nullToken = Token(TokenType::LITERAL_NULL, "null", Position());
    auto nullExpr = std::make_unique<LiteralExpr>(nullToken);
    
    auto binaryExpr = std::make_unique<BinaryExpr>(
        std::move(varExpr),
        TokenType::OP_NOT_EQUAL,
        std::move(nullExpr),
        Position()
    );
    
    // Extract guard
    TypeNarrowingAnalyzer analyzer;
    std::string varName;
    TypeGuardPtr guard = analyzer.extractGuardFromCondition(binaryExpr.get(), varName);
    
    assert(guard != nullptr);
    
    // Apply guard to Optional type
    TypeNarrowingContext context;
    context.registerVariable("value", optionalType);
    context.pushScope();
    bool applied = context.applyGuard("value", guard);
    
    assert(applied);
    
    // Check narrowed type
    TypePtr narrowedType = context.getVariableType("value");
    assert(narrowedType != nullptr);
    assert(narrowedType->equals(stringType.get()));
    
    printSuccess("طُبق الحارس بنجاح: String? → String");
}

// ════════════════════════════════════════════════════════════════════════════
// Main Test Runner
// ════════════════════════════════════════════════════════════════════════════

int main() {
    printHeader("Type Narrowing AST Integration Tests", "اختبارات تكامل Type Narrowing مع AST");
    std::cout << "Phase 1.3.5.2 - Day 2\n";
    std::cout << "Testing extractGuardFromCondition with real AST nodes\n\n";
    
    try {
        testExtractNotNullGuard();
        testExtractNullGuard();
        testExtractNotNullGuardReversed();
        testExtractNullGuardFromNot();
        testNoGuardForOtherExpressions();
        testApplyExtractedGuard();
        
        std::cout << "\n╔════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║  ✓✓✓ جميع الاختبارات نجحت! (All Tests Passed!) ✓✓✓\n";
        std::cout << "╚════════════════════════════════════════════════════════════════════════╝\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ فشل الاختبار (Test Failed): " << e.what() << "\n";
        return 1;
    }
}
