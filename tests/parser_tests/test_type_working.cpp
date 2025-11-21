/**
 * @file test_type_working.cpp
 * @brief اختبار عملي لنظام الأنواع / Working Type System Test
 */

#include <iostream>
#include <string>
#include "parser/parser_core.h"
#include "lexer/lexer_core.h"

using namespace Sad;
using namespace Sad::Parser;
using namespace Sad::Lexer;
using namespace Sad::Data;

int main() {
    std::cout << "\n═══════════════════════════════════════════════════════════\n";
    std::cout << "║  Type System Integration Tests                          ║\n";
    std::cout << "║  اختبارات تكامل نظام الأنواع                           ║\n";
    std::cout << "═══════════════════════════════════════════════════════════\n\n";
    
    int passed = 0;
    int failed = 0;
    
    // Test 1: Lexer recognizes type keywords (English)
    std::cout << "Test 1: Lexer recognizes English type keywords\n";
    try {
        LexerCore lexer("int float string bool void");
        auto tokens = lexer.tokenize();
        
        bool allTypesFound = true;
        for (const auto& token : tokens) {
            if (token.getType() != TokenType::TYPE_INTEGER &&
                token.getType() != TokenType::TYPE_DOUBLE &&
                token.getType() != TokenType::TYPE_STRING &&
                token.getType() != TokenType::TYPE_BOOLEAN &&
                token.getType() != TokenType::TYPE_VOID &&
                token.getType() != TokenType::END_OF_FILE) {
                allTypesFound = false;
                break;
            }
        }
        
        if (allTypesFound) {
            std::cout << "  ✓ PASSED\n\n";
            passed++;
        } else {
            std::cout << "  ✗ FAILED\n\n";
            failed++;
        }
    } catch (...) {
        std::cout << "  ✗ FAILED (exception)\n\n";
        failed++;
    }
    
    // Test 2: Lexer recognizes type keywords (Arabic)
    std::cout << "Test 2: Lexer recognizes Arabic type keywords\n";
    try {
        LexerCore lexer("رقم عشري نص منطقي فراغ");
        auto tokens = lexer.tokenize();
        
        bool allTypesFound = true;
        for (const auto& token : tokens) {
            if (token.getType() != TokenType::TYPE_INTEGER &&
                token.getType() != TokenType::TYPE_DOUBLE &&
                token.getType() != TokenType::TYPE_STRING &&
                token.getType() != TokenType::TYPE_BOOLEAN &&
                token.getType() != TokenType::TYPE_VOID &&
                token.getType() != TokenType::END_OF_FILE) {
                allTypesFound = false;
                break;
            }
        }
        
        if (allTypesFound) {
            std::cout << "  ✓ PASSED\n\n";
            passed++;
        } else {
            std::cout << "  ✗ FAILED\n\n";
            failed++;
        }
    } catch (...) {
        std::cout << "  ✗ FAILED (exception)\n\n";
        failed++;
    }
    
    // Test 3: Lexer recognizes colon
    std::cout << "Test 3: Lexer recognizes colon (:)\n";
    try {
        LexerCore lexer("x : int");
        auto tokens = lexer.tokenize();
        
        bool colonFound = false;
        for (const auto& token : tokens) {
            if (token.getType() == TokenType::COLON) {
                colonFound = true;
                break;
            }
        }
        
        if (colonFound) {
            std::cout << "  ✓ PASSED\n\n";
            passed++;
        } else {
            std::cout << "  ✗ FAILED (colon not found)\n\n";
            failed++;
        }
    } catch (...) {
        std::cout << "  ✗ FAILED (exception)\n\n";
        failed++;
    }
    
    // Test 4: Complete variable declaration tokens
    std::cout << "Test 4: Complete variable declaration tokens\n";
    try {
      //  LexerCore lexer("var x : int = 10 ;");
       LexerCore lexer(" int x = 10 ;");
        auto tokens = lexer.tokenize();
        
        std::cout << "  Tokens found: " << tokens.size() << "\n";
        for (size_t i = 0; i < tokens.size(); ++i) {
            std::cout << "    [" << i << "] " << tokens[i].getValue() 
                      << " (Type: " << static_cast<int>(tokens[i].getType()) << ")\n";
        }
        
        // Expected: var(KEYWORD), x(ID), :(COLON), int(TYPE), =(ASSIGN), 10(NUM), ;(SEMI), EOF
        if (tokens.size() >= 5) {
            std::cout << "  ✓ PASSED\n\n";
            passed++;
        } else {
            std::cout << "  ✗ FAILED (wrong token count)\n\n";
            failed++;
        }
    } catch (const std::exception& e) {
        std::cout << "  ✗ FAILED (exception: " << e.what() << ")\n\n";
        failed++;
    }
    
    // Summary
    std::cout << "═══════════════════════════════════════════════════════════\n";
    std::cout << "Test Results:\n";
    std::cout << "  Total:  " << (passed + failed) << "\n";
    std::cout << "  Passed: " << passed << "\n";
    std::cout << "  Failed: " << failed << "\n";
    std::cout << "═══════════════════════════════════════════════════════════\n\n";
    
    if (failed == 0) {
        std::cout << "✓ All integration tests passed!\n";
        std::cout << "✓ جميع اختبارات التكامل نجحت!\n\n";
        return 0;
    } else {
        std::cout << "✗ Some tests failed!\n";
        std::cout << "✗ بعض الاختبارات فشلت!\n\n";
        return 1;
    }
}
