/**
 * @file test_if_debug.cpp
 * @brief Debug test for IF statement
 */

#include "test_framework.h"
#include "../../include/parser/parser_core.h"
#include "../../include/lexer/lexer_core.h"
#include <iostream>

using namespace Sad;

int main() {
    const char* source = R"(
        if (x > 0) {
            return x;
        }
    )";
    
    std::cout << "Testing IF statement..." << std::endl;
    std::cout << "Source: " << source << std::endl;
    
    Lexer::LexerCore lexer(source);
    Parser::ParserCore parser(lexer);
    
    auto program = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "\n❌ Parser has errors:" << std::endl;
        auto errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cout << "  - " << error << std::endl;
        }
    } else {
        std::cout << "\n✅ Parsed successfully!" << std::endl;
        std::cout << "Program size: " << program.size() << std::endl;
    }
    
    return 0;
}
