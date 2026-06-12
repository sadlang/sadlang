/**
 * @file test_lexer_colon.cpp
 * @brief اختبار تعرف lexer على : / Test lexer recognizes :
 */

#include <iostream>
#include <string>
#include "lexer/lexer_core.h"

using namespace Sad;
using namespace Sad::Lexer;

int main() {
    std::cout << "\n========================================\n";
    std::cout << "Lexer Colon Test\n";
    std::cout << "========================================\n\n";
    
    std::string code = " int = 10;";
    std::cout << "Code: " << code << "\n\n";
    
    LexerCore lexer(code);
    auto tokens = lexer.tokenize();
    
    std::cout << "Tokens (" << tokens.size() << "):\n";
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << "  [" << i << "] Type=" << static_cast<int>(tokens[i].getType()) 
                  << ", Value='" << tokens[i].getValue() << "'\n";
    }
    
    std::cout << "\n========================================\n\n";
    
    return 0;
}
