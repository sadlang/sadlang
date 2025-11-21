/**
 * @file test_type_simple.cpp
 * @brief اختبار بسيط لنظام الأنواع / Simple Type System Test
 */

#include <iostream>
#include <string>

// Include parser headers
#include "parser/parser_core.h"
#include "lexer/lexer_core.h"
#include "data/types/data_types.h"

using namespace Sad;
using namespace Sad::Parser;
using namespace Sad::Lexer;
using namespace Sad::Data;

int main() {
    std::cout << "\n========================================\n";
    std::cout << "Simple Type System Test\n";
    std::cout << "========================================\n\n";
    
    try {
        // Test 1: English typed variable
        std::string code1 = "متغير x : int = 10;";
        std::cout << "Test 1: " << code1 << "\n";
        std::cout << "  Creating lexer...\n" << std::flush;
        
        LexerCore lexer1(code1);
        std::cout << "  Creating parser...\n" << std::flush;
        
        ParserCore parser1(lexer1);
        std::cout << "  Parsing...\n" << std::flush;
        
        auto ast1 = parser1.parseProgram();
        std::cout << "  Parsing complete. AST size: " << ast1.size() << "\n" << std::flush;
        
        if (ast1.size() > 0) {
            auto* varDecl = dynamic_cast<AST::VarDeclStmt*>(ast1[0].get());
            if (varDecl) {
                std::cout << "  Variable name: " << varDecl->name << "\n";
                std::cout << "  Type: " << static_cast<int>(varDecl->type) << "\n";
                std::cout << "  ✓ PASSED\n\n";
            } else {
                std::cout << "  ✗ FAILED: Not a VarDeclStmt\n\n";
            }
        } else {
            std::cout << "  ✗ FAILED: No statements parsed\n\n";
        }
        
        // Test 2: Arabic typed variable
        std::string code2 = "متغير y : رقم = 20;";
        std::cout << "Test 2: " << code2 << "\n";
        
        LexerCore lexer2(code2);
        ParserCore parser2(lexer2);
        auto ast2 = parser2.parseProgram();
        
        if (ast2.size() > 0) {
            auto* varDecl = dynamic_cast<AST::VarDeclStmt*>(ast2[0].get());
            if (varDecl) {
                std::cout << "  Variable name: " << varDecl->name << "\n";
                std::cout << "  Type: " << static_cast<int>(varDecl->type) << "\n";
                std::cout << "  ✓ PASSED\n\n";
            } else {
                std::cout << "  ✗ FAILED: Not a VarDeclStmt\n\n";
            }
        } else {
            std::cout << "  ✗ FAILED: No statements parsed\n\n";
        }
        
        // Test 3: Untyped variable
        std::string code3 = "متغير z = 30;";
        std::cout << "Test 3: " << code3 << "\n";
        
        LexerCore lexer3(code3);
        ParserCore parser3(lexer3);
        auto ast3 = parser3.parseProgram();
        
        if (ast3.size() > 0) {
            auto* varDecl = dynamic_cast<AST::VarDeclStmt*>(ast3[0].get());
            if (varDecl) {
                std::cout << "  Variable name: " << varDecl->name << "\n";
                std::cout << "  Type: " << static_cast<int>(varDecl->type) << " (UNKNOWN = 0)\n";
                std::cout << "  ✓ PASSED\n\n";
            } else {
                std::cout << "  ✗ FAILED: Not a VarDeclStmt\n\n";
            }
        } else {
            std::cout << "  ✗ FAILED: No statements parsed\n\n";
        }
        
        std::cout << "========================================\n";
        std::cout << "All tests completed!\n";
        std::cout << "========================================\n\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "✗ Exception: " << e.what() << "\n\n";
        return 1;
    }
}
