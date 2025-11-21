/**
 * @file test_decorator_token.cpp
 * @brief (AR) اختبار رمز @ للمُزخرِفات
 * @brief (EN) Test @ token for decorators
 */

#include "../../include/lexer/lexer_core.h"
#include <iostream>
#include <cassert>

using namespace Sad::Lexer;

int main() {
    std::cout << "Testing AT_SIGN (@) token for decorators...\n\n";
    
    // Test 1: Simple @ token
    {
        std::cout << "Test 1: Simple @ token\n";
        LexerCore lexer("@");
        auto token = lexer.nextToken();
        
        assert(token.getType() == TokenType::AT_SIGN);
        assert(token.getValue() == "@");
        std::cout << "  Token: " << token.toString() << "\n";
        std::cout << "  ✅ PASS\n\n";
    }
    
    // Test 2: @decorator pattern
    {
        std::cout << "Test 2: @decorator pattern\n";
        LexerCore lexer("@decorator");
        
        auto tok1 = lexer.nextToken();
        auto tok2 = lexer.nextToken();
        
        assert(tok1.getType() == TokenType::AT_SIGN);
        assert(tok1.getValue() == "@");
        assert(tok2.getType() == TokenType::IDENTIFIER);
        assert(tok2.getValue() == "decorator");
        
        std::cout << "  Token 1: " << tok1.toString() << "\n";
        std::cout << "  Token 2: " << tok2.toString() << "\n";
        std::cout << "  ✅ PASS\n\n";
    }
    
    // Test 3: @decorator(args)
    {
        std::cout << "Test 3: @decorator(args)\n";
        LexerCore lexer("@decorator(arg1, arg2)");
        
        auto tok1 = lexer.nextToken(); // @
        auto tok2 = lexer.nextToken(); // decorator
        auto tok3 = lexer.nextToken(); // (
        auto tok4 = lexer.nextToken(); // arg1
        auto tok5 = lexer.nextToken(); // ,
        auto tok6 = lexer.nextToken(); // arg2
        auto tok7 = lexer.nextToken(); // )
        
        assert(tok1.getType() == TokenType::AT_SIGN);
        assert(tok2.getType() == TokenType::IDENTIFIER);
        assert(tok3.getType() == TokenType::PAREN_LEFT);
        assert(tok4.getType() == TokenType::IDENTIFIER);
        assert(tok5.getType() == TokenType::COMMA);
        assert(tok6.getType() == TokenType::IDENTIFIER);
        assert(tok7.getType() == TokenType::PAREN_RIGHT);
        
        std::cout << "  ✅ PASS - All tokens correct\n\n";
    }
    
    // Test 4: Multiple decorators
    {
        std::cout << "Test 4: Multiple decorators\n";
        LexerCore lexer("@dec1\n@dec2\n@dec3");
        
        auto tok1 = lexer.nextToken(); // @
        auto tok2 = lexer.nextToken(); // dec1
        // Skip newline (lexer ignores whitespace)
        auto tok3 = lexer.nextToken(); // @
        auto tok4 = lexer.nextToken(); // dec2
        
        assert(tok1.getType() == TokenType::AT_SIGN);
        assert(tok2.getValue() == "dec1");
        assert(tok3.getType() == TokenType::AT_SIGN);
        assert(tok4.getValue() == "dec2");
        
        std::cout << "  ✅ PASS - Multiple decorators tokenized correctly\n\n";
    }
    
    // Test 5: Arabic decorator name (مُزخرِف)
    {
        std::cout << "Test 5: Arabic decorator name\n";
        LexerCore lexer("@مُزخرِف");
        
        auto tok1 = lexer.nextToken(); // @
        auto tok2 = lexer.nextToken(); // مُزخرِف
        
        assert(tok1.getType() == TokenType::AT_SIGN);
        assert(tok2.getType() == TokenType::IDENTIFIER);
        assert(tok2.getValue() == "مُزخرِف");
        
        std::cout << "  Token 1: " << tok1.toString() << "\n";
        std::cout << "  Token 2: " << tok2.toString() << "\n";
        std::cout << "  ✅ PASS - Arabic decorator name\n\n";
    }
    
    std::cout << "=== ALL TESTS PASSED ===\n";
    std::cout << "AT_SIGN token working correctly! ✅\n";
    
    return 0;
}
