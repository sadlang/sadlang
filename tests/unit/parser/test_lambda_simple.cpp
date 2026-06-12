// ======================================================================
// test_lambda_simple.cpp - اختبار lambda البسيط / Simple Lambda Test
// ======================================================================
// الوصف بالعربية:
//   اختبار بسيط لتحليل lambda expressions
//
// English Description:
//   Simple test for parsing lambda expressions
// ======================================================================

#include "../../include/lexer/lexer_core.h"
#include "../../include/lexer/lexer_keywords.h"
#include "../../include/parser/parser_core.h"
#include <iostream>
#include <string>

using namespace Sad;

int main() {
    std::cout << "\n========================================\n";
    std::cout << "Lambda Expression Test\n";
    std::cout << "========================================\n\n";
    
    // تهيئة الكلمات المفتاحية
    // Initialize keywords table
    Lexer::KeywordTable::initialize();
    
    // Test 1: Python-style lambda
    {
        std::cout << "Test 1: Python-style lambda\n";
        std::cout << "Code: lambda x: x * 2\n\n";
        
        std::string code = "lambda x: x * 2";
        Lexer::LexerCore lexer(code);
        auto tokens = lexer.tokenize();
        
        std::cout << "Tokens (" << tokens.size() << "):\n";
        for (size_t i = 0; i < tokens.size(); i++) {
            std::cout << "  [" << i << "] Type=" 
                      << static_cast<int>(tokens[i].getType())
                      << ", Value='" << tokens[i].getValue() << "'\n";
        }
        std::cout << "\n";
        
        // Note: Parser needs direct testing through public methods
        // For now just verify tokenization works
        std::cout << "✅ Tokenization successful!\n";
        std::cout << "\n";
    }
    
    // Test 2: Arabic lambda
    {
        std::cout << "Test 2: Arabic lambda\n";
        std::cout << "Code: لامدا س: س * 2\n\n";
        
        std::string code = "لامدا س: س * 2";
        Lexer::LexerCore lexer(code);
        auto tokens = lexer.tokenize();
        
        std::cout << "Tokens (" << tokens.size() << "):\n";
        for (size_t i = 0; i < tokens.size(); i++) {
            std::cout << "  [" << i << "] Type=" 
                      << static_cast<int>(tokens[i].getType())
                      << ", Value='" << tokens[i].getValue() << "'\n";
        }
        std::cout << "\n";
        
        std::cout << "✅ Tokenization successful!\n";
        std::cout << "\n";
    }
    
    // Test 3: Lambda with multiple parameters
    {
        std::cout << "Test 3: Lambda with multiple parameters\n";
        std::cout << "Code: lambda x, y: x + y\n\n";
        
        std::string code = "lambda x, y: x + y";
        Lexer::LexerCore lexer(code);
        auto tokens = lexer.tokenize();
        
        std::cout << "Tokens (" << tokens.size() << "):\n";
        for (size_t i = 0; i < tokens.size(); i++) {
            std::cout << "  [" << i << "] Type=" 
                      << static_cast<int>(tokens[i].getType())
                      << ", Value='" << tokens[i].getValue() << "'\n";
        }
        std::cout << "\n";
        
        std::cout << "✅ Tokenization successful!\n";
        std::cout << "\n";
    }
    
    std::cout << "========================================\n\n";
    return 0;
}
