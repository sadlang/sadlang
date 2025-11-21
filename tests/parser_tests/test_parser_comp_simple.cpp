/**
 * @file test_parser_comp_simple.cpp
 * @brief (AR) اختبار شامل لـ parser الخاص بـ list/dict comprehensions.
 *        (EN) Comprehensive parser test for list/dict comprehensions.
 * 
 * Tests actual parser output and AST node types.
 */

#include "../../include/lexer/lexer_core.h"
#include "../../include/parser/parser_core.h"
#include "../../include/parser/ast/ast_node.h"
#include "../../include/parser/ast/expressions.h"
#include "../../include/parser/ast/statements.h"
#include "../../include/parser/ast/declarations.h"
#include "../../include/errors/error_manager.h"

#include <iostream>
#include <string>
#include <vector>

using namespace Sad;
using namespace Sad::Lexer;
using namespace Sad::Parser;

/**
 * @brief Helper function to test parsing
 */
bool testParse(const std::string& input, const std::string& testName) {
    std::cout << "\n=== " << testName << " ===" << std::endl;
    std::cout << "Input: " << input << std::endl;
    
    try {
        // Tokenize
        LexerCore lexer(input);
        
        // Parse
        ParserCore parser(lexer);
        auto statements = parser.parseProgram();
        
        if (statements.empty()) {
            std::cerr << "❌ Parsing failed: no statements\n";
            return false;
        }
        
        std::cout << "✅ Parsed successfully! Statements: " << statements.size() << "\n";
        
        // Analyze first statement
        auto& stmt = statements[0];
        
        if (auto* exprStmt = dynamic_cast<AST::ExprStmt*>(stmt.get())) {
            std::cout << "   Statement type: ExprStmt\n";
            
            if (auto* listComp = dynamic_cast<AST::ListComprehensionExpr*>(exprStmt->expression.get())) {
                std::cout << "   ✅ Expression: ListComprehensionExpr\n";
                std::cout << "   Variable: " << listComp->variable << "\n";
                return true;
            }
            else if (auto* dictComp = dynamic_cast<AST::DictComprehensionExpr*>(exprStmt->expression.get())) {
                std::cout << "   ✅ Expression: DictComprehensionExpr\n";
                std::cout << "   Variable: " << dictComp->variable << "\n";
                return true;
            }
            else if (auto* array = dynamic_cast<AST::ArrayExpr*>(exprStmt->expression.get())) {
                std::cout << "   ✅ Expression: ArrayExpr (elements: " << array->elements.size() << ")\n";
                return true;
            }
            else if (auto* map = dynamic_cast<AST::MapExpr*>(exprStmt->expression.get())) {
                std::cout << "   ✅ Expression: MapExpr (pairs: " << map->pairs.size() << ")\n";
                return true;
            }
        }
        else if (auto* varDecl = dynamic_cast<AST::VarDeclStmt*>(stmt.get())) {
            std::cout << "   Statement type: VarDeclStmt\n";
            std::cout << "   Variable: " << varDecl->name << "\n";
            
            if (auto* listComp = dynamic_cast<AST::ListComprehensionExpr*>(varDecl->initializer.get())) {
                std::cout << "   ✅ Initializer: ListComprehensionExpr\n";
                return true;
            }
            else if (auto* dictComp = dynamic_cast<AST::DictComprehensionExpr*>(varDecl->initializer.get())) {
                std::cout << "   ✅ Initializer: DictComprehensionExpr\n";
                return true;
            }
        }
        
        std::cerr << "❌ Unexpected AST node type\n";
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception: " << e.what() << "\n";
        return false;
    }
}

int main() {
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║      COMPREHENSIVE PARSER TEST - COMPREHENSIONS         ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";
    
    int passed = 0;
    int total = 0;
    
    // Category 1: Basic List Comprehensions
    std::cout << "\n━━━ Category 1: Basic List Comprehensions ━━━\n";
    
    total++; if (testParse("[x for x in list]", "Test 1.1: Simple list comprehension")) passed++;
    total++; if (testParse("[x * 2 for x in numbers]", "Test 1.2: List comprehension with expression")) passed++;
    total++; if (testParse("[x for x in items if x > 5]", "Test 1.3: List comprehension with filter")) passed++;
    total++; if (testParse("[x + y for x in nums if x % 2 == 0]", "Test 1.4: Complex expression and filter")) passed++;
    
    // Category 2: List Comprehensions in Variables
    std::cout << "\n━━━ Category 2: List Comprehensions in Variables ━━━\n";
    
    total++; if (testParse("var result = [x * 2 for x in data]", "Test 2.1: Variable with list comprehension")) passed++;
    total++; if (testParse("var filtered = [item for item in list if item > 0]", "Test 2.2: Variable with filtered comprehension")) passed++;
    
    // Category 3: Dict Comprehensions
    std::cout << "\n━━━ Category 3: Dict Comprehensions ━━━\n";
    
    total++; if (testParse("{k: v for item in pairs}", "Test 3.1: Simple dict comprehension")) passed++;
    total++; if (testParse("{key: value * 2 for item in items}", "Test 3.2: Dict comprehension with expression")) passed++;
    total++; if (testParse("{k: v for item in data if item > 10}", "Test 3.3: Dict comprehension with filter")) passed++;
    
    // Category 4: Dict Comprehensions in Variables
    std::cout << "\n━━━ Category 4: Dict Comprehensions in Variables ━━━\n";
    
    total++; if (testParse("var mapping = {k: v * 2 for item in items}", "Test 4.1: Variable with dict comprehension")) passed++;
    
    // Category 5: Arabic Syntax
    std::cout << "\n━━━ Category 5: Arabic Syntax ━━━\n";
    
    total++; if (testParse("[س لكل س في أرقام]", "Test 5.1: Arabic list comprehension")) passed++;
    total++; if (testParse("{م: ق لكل عنصر في بيانات}", "Test 5.2: Arabic dict comprehension")) passed++;
    
    // Category 6: Regular Arrays and Maps (should NOT be comprehensions)
    std::cout << "\n━━━ Category 6: Regular Arrays and Maps ━━━\n";
    
    total++; if (testParse("[1, 2, 3, 4]", "Test 6.1: Regular array")) passed++;
    total++; if (testParse("{a: 1, b: 2}", "Test 6.2: Regular map")) passed++;
    total++; if (testParse("[x, y, z]", "Test 6.3: Array of identifiers")) passed++;
    
    // Summary
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    TEST SUMMARY                        ║\n";
    std::cout << "╠════════════════════════════════════════════════════════╣\n";
    std::cout << "║  Total Tests: " << total << "                                          ║\n";
    std::cout << "║  Passed:      " << passed << "                                          ║\n";
    std::cout << "║  Failed:      " << (total - passed) << "                                          ║\n";
    std::cout << "║  Pass Rate:   " << (passed * 100.0 / total) << "%                                    ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";
    
    return (passed == total) ? 0 : 1;
}
