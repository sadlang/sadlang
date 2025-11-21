/**
 * @file test_yield_token.cpp
 * @brief (AR) اختبار رمز الـ yield
 * @brief (EN) Test yield keyword tokenization
 * 
 * Tests:
 * 1. yield keyword recognition
 * 2. Arabic keyword (اعطِ)
 * 3. yield with value
 * 4. yield from pattern
 */

#include "../../include/lexer/lexer_core.h"
#include <iostream>
#include <cassert>

using namespace Sad;

void testSimpleYield() {
    std::cout << "Test 1: Simple 'yield' keyword\n";
    
    std::string code = "yield";
    Lexer::LexerCore lexer(code);
    
    auto tok = lexer.nextToken();
    
    assert(tok.getType() == Lexer::TokenType::KEYWORD_YIELD);
    assert(tok.getValue() == "yield");
    
    std::cout << "  Token: " << tok.toString() << "\n";
    std::cout << "  ✅ PASS\n\n";
}

void testYieldWithValue() {
    std::cout << "Test 2: 'yield value' pattern\n";
    
    std::string code = "yield 42";
    Lexer::LexerCore lexer(code);
    
    auto tok1 = lexer.nextToken(); // yield
    auto tok2 = lexer.nextToken(); // 42
    
    assert(tok1.getType() == Lexer::TokenType::KEYWORD_YIELD);
    assert(tok2.getType() == Lexer::TokenType::NUMBER_INTEGER);
    assert(tok2.getValue() == "42");
    
    std::cout << "  Tokens: " << tok1.toString() << " " << tok2.toString() << "\n";
    std::cout << "  ✅ PASS\n\n";
}

void testYieldFrom() {
    std::cout << "Test 3: 'yield from' pattern\n";
    
    std::string code = "yield from iterable";
    Lexer::LexerCore lexer(code);
    
    auto tok1 = lexer.nextToken(); // yield
    auto tok2 = lexer.nextToken(); // from (identifier)
    auto tok3 = lexer.nextToken(); // iterable
    
    assert(tok1.getType() == Lexer::TokenType::KEYWORD_YIELD);
    assert(tok2.getType() == Lexer::TokenType::IDENTIFIER);
    assert(tok2.getValue() == "from");
    assert(tok3.getType() == Lexer::TokenType::IDENTIFIER);
    assert(tok3.getValue() == "iterable");
    
    std::cout << "  Tokens: " << tok1.toString() << " " 
              << tok2.toString() << " " << tok3.toString() << "\n";
    std::cout << "  ✅ PASS (Note: 'from' is identifier, not keyword)\n\n";
}

void testArabicYield() {
    std::cout << "Test 4: Arabic 'اعطِ' keyword\n";
    
    std::string code = "اعطِ";
    Lexer::LexerCore lexer(code);
    
    auto tok = lexer.nextToken();
    
    assert(tok.getType() == Lexer::TokenType::KEYWORD_YIELD);
    assert(tok.getValue() == "اعطِ");
    
    std::cout << "  Token: " << tok.toString() << "\n";
    std::cout << "  ✅ PASS\n\n";
}

void testYieldInFunction() {
    std::cout << "Test 5: 'yield' in function context\n";
    
    std::string code = "function gen() { yield 1; }";
    Lexer::LexerCore lexer(code);
    
    auto tok1 = lexer.nextToken(); // function
    auto tok2 = lexer.nextToken(); // gen
    auto tok3 = lexer.nextToken(); // (
    auto tok4 = lexer.nextToken(); // )
    auto tok5 = lexer.nextToken(); // {
    auto tok6 = lexer.nextToken(); // yield
    auto tok7 = lexer.nextToken(); // 1
    
    assert(tok1.getType() == Lexer::TokenType::KEYWORD_FUNCTION);
    assert(tok6.getType() == Lexer::TokenType::KEYWORD_YIELD);
    assert(tok7.getType() == Lexer::TokenType::NUMBER_INTEGER);
    
    std::cout << "  Found 'yield' at position: " 
              << tok6.getPosition().line << ":" 
              << tok6.getPosition().column << "\n";
    std::cout << "  ✅ PASS\n\n";
}

int main() {
    std::cout << "=== Testing YIELD Token (Task 5.1) ===\n\n";
    
    try {
        testSimpleYield();
        testYieldWithValue();
        testYieldFrom();
        testArabicYield();
        testYieldInFunction();
        
        std::cout << "=== ALL YIELD TOKEN TESTS PASSED ===\n";
        std::cout << "✅ KEYWORD_YIELD token verified\n";
        std::cout << "✅ English 'yield' works\n";
        std::cout << "✅ Arabic 'اعطِ' works\n";
        std::cout << "✅ 'yield from' pattern recognized\n\n";
        std::cout << "Ready to implement YieldStmt AST node!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "❌ TEST FAILED: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
