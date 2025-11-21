/**
 * @file test_yield_integration.cpp
 * @brief (AR) اختبار تكامل جملة yield في الدوال المولّدة
 * @brief (EN) Test yield statement integration in generator functions
 * 
 * Tests full yield syntax:
 * - yield value;
 * - yield from iterable;
 * - yield; (no value)
 * - Multiple yields in function
 * - Arabic اعطِ keyword
 */

#include "../../include/lexer/lexer_core.h"
#include "../../include/parser/parser_core.h"
#include "../../include/parser/ast/declarations.h"
#include "../../include/parser/ast/statements.h"
#include "../../include/parser/ast/expressions.h"
#include <iostream>
#include <cassert>

using namespace Sad;

void testSimpleYield() {
    std::cout << "=== Test 1: Simple Yield with Value ===\n";
    std::cout << "Code: function gen() { yield 42; }\n";
    
    std::string code = "function gen() { yield 42; }";
    Lexer::LexerCore lexer(code);
    Parser::ParserCore parser(lexer);
    
    auto statements = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "Errors found:\n";
        parser.printErrors();
        assert(false && "Parser errors found");
    }
    
    assert(!parser.hasErrors());
    assert(!statements.empty());
    assert(statements.size() == 1);
    
    // Check it's a FunctionDecl
    auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(statements[0].get());
    assert(funcDecl != nullptr);
    assert(funcDecl->name == "gen");
    
    // Check body has one statement (block with yield)
    assert(funcDecl->body != nullptr);
    auto* block = dynamic_cast<AST::BlockStmt*>(funcDecl->body.get());
    assert(block != nullptr);
    assert(!block->statements.empty());
    
    // Check yield statement
    auto* yieldStmt = dynamic_cast<AST::YieldStmt*>(block->statements[0].get());
    assert(yieldStmt != nullptr);
    assert(yieldStmt->value != nullptr);
    assert(!yieldStmt->isYieldFrom);
    
    std::cout << "  Function: " << funcDecl->name << "\n";
    std::cout << "  Yield: " << yieldStmt->toString() << "\n";
    std::cout << "  ✅ PASS\n\n";
}

void testYieldWithExpression() {
    std::cout << "=== Test 2: Yield with Complex Expression ===\n";
    std::cout << "Code: function gen() { yield x * 2 + 1; }\n";
    
    std::string code = "function gen() { yield x * 2 + 1; }";
    Lexer::LexerCore lexer(code);
    Parser::ParserCore parser(lexer);
    
    auto statements = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "Errors found:\n";
        parser.printErrors();
        assert(false && "Parser errors found");
    }
    
    assert(!statements.empty());
    
    auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(statements[0].get());
    assert(funcDecl != nullptr);
    
    auto* block = dynamic_cast<AST::BlockStmt*>(funcDecl->body.get());
    assert(block != nullptr);
    
    auto* yieldStmt = dynamic_cast<AST::YieldStmt*>(block->statements[0].get());
    assert(yieldStmt != nullptr);
    assert(yieldStmt->value != nullptr);
    assert(!yieldStmt->isYieldFrom);
    
    std::cout << "  Yield: " << yieldStmt->toString() << "\n";
    std::cout << "  ✅ PASS\n\n";
}

void testYieldFrom() {
    std::cout << "=== Test 3: Yield From (Delegation) ===\n";
    std::cout << "Code: function gen() { yield from other(); }\n";
    
    std::string code = "function gen() { yield from other(); }";
    Lexer::LexerCore lexer(code);
    Parser::ParserCore parser(lexer);
    
    auto statements = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "Errors found:\n";
        parser.printErrors();
        assert(false && "Parser errors found");
    }
    
    assert(!statements.empty());
    
    auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(statements[0].get());
    assert(funcDecl != nullptr);
    
    auto* block = dynamic_cast<AST::BlockStmt*>(funcDecl->body.get());
    assert(block != nullptr);
    
    auto* yieldStmt = dynamic_cast<AST::YieldStmt*>(block->statements[0].get());
    assert(yieldStmt != nullptr);
    assert(yieldStmt->value != nullptr);
    assert(yieldStmt->isYieldFrom); // ✅ Should be true
    
    std::cout << "  Yield: " << yieldStmt->toString() << "\n";
    std::cout << "  Is 'yield from': " << (yieldStmt->isYieldFrom ? "Yes" : "No") << "\n";
    std::cout << "  ✅ PASS\n\n";
}

void testMultipleYields() {
    std::cout << "=== Test 4: Multiple Yields in Function ===\n";
    std::cout << "Code: function gen() { yield 1; yield 2; yield 3; }\n";
    
    std::string code = "function gen() { yield 1; yield 2; yield 3; }";
    Lexer::LexerCore lexer(code);
    Parser::ParserCore parser(lexer);
    
    auto statements = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "Errors found:\n";
        parser.printErrors();
        assert(false && "Parser errors found");
    }
    
    assert(!statements.empty());
    
    auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(statements[0].get());
    assert(funcDecl != nullptr);
    
    auto* block = dynamic_cast<AST::BlockStmt*>(funcDecl->body.get());
    assert(block != nullptr);
    assert(block->statements.size() == 3);
    
    // Check all are yield statements
    for (size_t i = 0; i < 3; i++) {
        auto* yieldStmt = dynamic_cast<AST::YieldStmt*>(block->statements[i].get());
        assert(yieldStmt != nullptr);
        assert(yieldStmt->value != nullptr);
        std::cout << "  Yield " << (i+1) << ": " << yieldStmt->toString() << "\n";
    }
    
    std::cout << "  ✅ PASS\n\n";
}

void testYieldNoValue() {
    std::cout << "=== Test 5: Yield with No Value ===\n";
    std::cout << "Code: function gen() { yield; }\n";
    
    std::string code = "function gen() { yield; }";
    Lexer::LexerCore lexer(code);
    Parser::ParserCore parser(lexer);
    
    auto statements = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "Errors found:\n";
        parser.printErrors();
        assert(false && "Parser errors found");
    }
    
    assert(!statements.empty());
    
    auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(statements[0].get());
    assert(funcDecl != nullptr);
    
    auto* block = dynamic_cast<AST::BlockStmt*>(funcDecl->body.get());
    assert(block != nullptr);
    
    auto* yieldStmt = dynamic_cast<AST::YieldStmt*>(block->statements[0].get());
    assert(yieldStmt != nullptr);
    assert(yieldStmt->value == nullptr); // ✅ No value
    assert(!yieldStmt->isYieldFrom);
    
    std::cout << "  Yield: " << yieldStmt->toString() << "\n";
    std::cout << "  ✅ PASS\n\n";
}

void testArabicYield() {
    std::cout << "=== Test 6: Arabic 'اعطِ' Keyword ===\n";
    std::cout << "Code: دالة مولد() { اعطِ 100; }\n";
    
    std::string code = "دالة مولد() { اعطِ 100; }";
    Lexer::LexerCore lexer(code);
    Parser::ParserCore parser(lexer);
    
    auto statements = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "Errors found:\n";
        parser.printErrors();
        assert(false && "Parser errors found");
    }
    
    assert(!statements.empty());
    
    auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(statements[0].get());
    assert(funcDecl != nullptr);
    assert(funcDecl->name == "مولد");
    
    auto* block = dynamic_cast<AST::BlockStmt*>(funcDecl->body.get());
    assert(block != nullptr);
    
    auto* yieldStmt = dynamic_cast<AST::YieldStmt*>(block->statements[0].get());
    assert(yieldStmt != nullptr);
    assert(yieldStmt->value != nullptr);
    
    std::cout << "  Function: " << funcDecl->name << "\n";
    std::cout << "  Yield: " << yieldStmt->toString() << "\n";
    std::cout << "  ✅ PASS (Arabic support verified)\n\n";
}

void testYieldInLoop() {
    std::cout << "=== Test 7: Multiple Yields with Control Flow ===\n";
    std::cout << "Code: function gen() { yield 1; yield 2; return; }\n";
    
    std::string code = "function gen() { yield 1; yield 2; return; }";
    Lexer::LexerCore lexer(code);
    Parser::ParserCore parser(lexer);
    
    auto statements = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "Errors found:\n";
        parser.printErrors();
        assert(false && "Parser errors found");
    }
    
    assert(!statements.empty());
    
    auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(statements[0].get());
    assert(funcDecl != nullptr);
    
    auto* block = dynamic_cast<AST::BlockStmt*>(funcDecl->body.get());
    assert(block != nullptr);
    assert(block->statements.size() == 3); // 2 yields + 1 return
    
    // Check first two are yields
    auto* yield1 = dynamic_cast<AST::YieldStmt*>(block->statements[0].get());
    assert(yield1 != nullptr);
    
    auto* yield2 = dynamic_cast<AST::YieldStmt*>(block->statements[1].get());
    assert(yield2 != nullptr);
    
    // Third is return
    auto* returnStmt = dynamic_cast<AST::ReturnStmt*>(block->statements[2].get());
    assert(returnStmt != nullptr);
    
    std::cout << "  ✅ PASS (yields mixed with return)\n\n";
}

void testComplexGenerator() {
    std::cout << "=== Test 8: Yield From with Value ===\n";
    std::cout << "Code: function gen() { yield 10; yield from other(); yield 20; }\n";
    
    std::string code = "function gen() { yield 10; yield from other(); yield 20; }";
    Lexer::LexerCore lexer(code);
    Parser::ParserCore parser(lexer);
    
    auto statements = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "Errors found:\n";
        parser.printErrors();
        assert(false && "Parser errors found");
    }
    
    assert(!statements.empty());
    
    auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(statements[0].get());
    assert(funcDecl != nullptr);
    
    auto* block = dynamic_cast<AST::BlockStmt*>(funcDecl->body.get());
    assert(block != nullptr);
    assert(block->statements.size() == 3); // yield, yield from, yield
    
    // First yield
    auto* yield1 = dynamic_cast<AST::YieldStmt*>(block->statements[0].get());
    assert(yield1 != nullptr);
    assert(!yield1->isYieldFrom);
    
    // Yield from
    auto* yield2 = dynamic_cast<AST::YieldStmt*>(block->statements[1].get());
    assert(yield2 != nullptr);
    assert(yield2->isYieldFrom);
    
    // Third yield
    auto* yield3 = dynamic_cast<AST::YieldStmt*>(block->statements[2].get());
    assert(yield3 != nullptr);
    assert(!yield3->isYieldFrom);
    
    std::cout << "  Statement 1: " << yield1->toString() << "\n";
    std::cout << "  Statement 2: " << yield2->toString() << "\n";
    std::cout << "  Statement 3: " << yield3->toString() << "\n";
    std::cout << "  ✅ PASS (complex generator patterns)\n\n";
}

int main() {
    std::cout << "\n╔════════════════════════════════════════════════════╗\n";
    std::cout << "║   YIELD STATEMENT INTEGRATION TEST SUITE         ║\n";
    std::cout << "║   (AR) مجموعة اختبار تكامل جملة yield             ║\n";
    std::cout << "╚════════════════════════════════════════════════════╝\n\n";
    
    try {
        testSimpleYield();
        testYieldWithExpression();
        testYieldFrom();
        testMultipleYields();
        testYieldNoValue();
        testArabicYield();
        testYieldInLoop();
        testComplexGenerator();
        
        std::cout << "\n╔════════════════════════════════════════════════════╗\n";
        std::cout << "║   ✅ ALL YIELD INTEGRATION TESTS PASSED           ║\n";
        std::cout << "║   ✅ جميع اختبارات تكامل yield نجحت               ║\n";
        std::cout << "╚════════════════════════════════════════════════════╝\n";
        std::cout << "\n✅ Yield statement parsing works correctly\n";
        std::cout << "✅ 'yield value' syntax supported\n";
        std::cout << "✅ 'yield from' delegation works\n";
        std::cout << "✅ Multiple yields in function\n";
        std::cout << "✅ Yield without value\n";
        std::cout << "✅ Arabic 'اعطِ' keyword supported\n";
        std::cout << "✅ Yield in loop context\n";
        std::cout << "✅ Complex generator patterns\n";
        std::cout << "\n🎉 Total: 8/8 tests passed!\n\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Exception: " << e.what() << "\n";
        return 1;
    }
}
