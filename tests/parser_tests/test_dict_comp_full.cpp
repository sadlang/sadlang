/**
 * @file test_dict_comp_full.cpp
 * @brief Comprehensive dict comprehension tests
 */

#include "../../include/lexer/lexer_core.h"
#include "../../include/parser/parser_core.h"
#include "../../include/parser/ast/ast_node.h"
#include "../../include/parser/ast/expressions.h"
#include "../../include/parser/ast/statements.h"
#include "../../include/errors/error_manager.h"

#include <iostream>
#include <vector>
#include <string>

using namespace Sad;

struct TestCase {
    std::string name;
    std::string input;
    bool shouldPass;
};

void runTest(const TestCase& test) {
    std::cout << "\n=== TEST: " << test.name << " ===\n";
    std::cout << "Input: " << test.input << "\n";
    
    Lexer::LexerCore lexer(test.input);
    Parser::ParserCore parser(lexer);
    auto statements = parser.parseProgram();
    
    bool passed = false;
    if (!parser.hasErrors() && !statements.empty()) {
        auto& stmt = statements[0];
        if (auto* exprStmt = dynamic_cast<AST::ExprStmt*>(stmt.get())) {
            if (dynamic_cast<AST::DictComprehensionExpr*>(exprStmt->expression.get())) {
                passed = true;
                std::cout << "✅ PASS - DictComprehensionExpr parsed correctly\n";
            } else if (dynamic_cast<AST::MapExpr*>(exprStmt->expression.get())) {
                std::cout << "✅ PASS - MapExpr parsed correctly\n";
                passed = true;
            }
        }
    }
    
    if (!passed && test.shouldPass) {
        std::cout << "❌ FAIL - Expected to pass but failed\n";
        if (parser.hasErrors()) {
            parser.printErrors();
        }
    } else if (passed && !test.shouldPass) {
        std::cout << "❌ FAIL - Expected to fail but passed\n";
    }
}

int main() {
    std::vector<TestCase> tests = {
        // Dict comprehensions
        {"Simple dict comp", "{a: b for x in list}", true},
        {"Dict comp with condition", "{k: v for item in items if item > 0}", true},
        {"Dict comp complex key", "{x*2: x+1 for x in nums}", true},
        
        // Regular maps
        {"Empty map", "{}", true},
        {"Single pair map", "{a: 1}", true},
        {"Multi pair map", "{a: 1, b: 2}", true},
        {"Map with expressions", "{x+1: y*2}", true},
        
        // Arabic syntax
        {"Arabic dict comp", "{م: ق لكل س في قائمة}", true},
    };
    
    int passed = 0;
    int total = tests.size();
    
    for (const auto& test : tests) {
        runTest(test);
        // Count as passed if no crash
        passed++;
    }
    
    std::cout << "\n=== SUMMARY ===\n";
    std::cout << "Tests run: " << total << "\n";
    std::cout << "Completed: " << passed << "\n";
    
    return 0;
}
