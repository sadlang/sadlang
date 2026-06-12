/**
 * @file test_dict_simple.cpp
 * @brief Simple dict comprehension test
 */

#include "../../include/lexer/lexer_core.h"
#include "../../include/parser/parser_core.h"
#include "../../include/parser/ast/ast_node.h"
#include "../../include/parser/ast/expressions.h"
#include "../../include/parser/ast/statements.h"
#include "../../include/errors/error_manager.h"

#include <iostream>

using namespace Sad;

int main() {
    std::string input = "{a: b for x in list}";
    std::cout << "Testing: " << input << "\n\n";
    
    // Tokenize
    Lexer::LexerCore lexer(input);
    
    std::cout << "=== TOKENS ===\n";
    while (true) {
        auto token = lexer.nextToken();
        std::cout << "Token: " << token.toString() << "\n";
        if (token.getType() == Lexer::TokenType::END_OF_FILE) break;
    }
    
    // Reset lexer
    Lexer::LexerCore lexer2(input);
    
    // Parse
    std::cout << "\n=== PARSING ===\n";
    Parser::ParserCore parser(lexer2);
    auto statements = parser.parseProgram();
    
    std::cout << "\n=== RESULT ===\n";
    std::cout << "Statements: " << statements.size() << "\n";
    
    if (!statements.empty()) {
        auto& stmt = statements[0];
        if (auto* exprStmt = dynamic_cast<AST::ExprStmt*>(stmt.get())) {
            std::cout << "✅ ExprStmt\n";
            if (auto* dictComp = dynamic_cast<AST::DictComprehensionExpr*>(exprStmt->expression.get())) {
                std::cout << "✅✅ DictComprehensionExpr\n";
                std::cout << "Variable: " << dictComp->variable << "\n";
            } else {
                std::cout << "❌ Not DictComprehensionExpr\n";
            }
        } else {
            std::cout << "❌ Not ExprStmt\n";
        }
    }
    
    if (parser.hasErrors()) {
        std::cout << "\n=== ERRORS ===\n";
        parser.printErrors();
    }
    
    return 0;
}
