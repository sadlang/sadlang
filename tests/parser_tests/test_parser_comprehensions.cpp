// ======================================================================
// test_parser_comprehensions.cpp - اختبار Parser للـ Comprehensions
// ======================================================================
// الوصف بالعربية:
//   اختبار شامل لـ Parser مع list و dict comprehensions
//
// English Description:
//   Comprehensive Parser test for list and dict comprehensions
// ======================================================================

#include "../../include/lexer/lexer_core.h"
#include "../../include/lexer/lexer_keywords.h"
#include "../../include/parser/parser_core.h"
#include "../../include/parser/ast/ast_node.h"
#include "../../include/parser/ast/expressions.h"
#include "../../include/parser/ast/statements.h"
#include "../../include/parser/ast/declarations.h"
#include <iostream>
#include <string>
#include <memory>

using namespace Sad;

int testsPassed = 0;
int testsFailed = 0;

void testParser(const std::string& testName, const std::string& code, bool shouldSucceed = true) {
    std::cout << "========================================\n";
    std::cout << testName << "\n";
    std::cout << "Code: " << code << "\n\n";
    
    try {
        // Create lexer and parser
        Lexer::LexerCore lexer(code);
        Parser::ParserCore parser(lexer);
        
        // Parse the program
        auto statements = parser.parseProgram();
        
        if (!statements.empty()) {
            std::cout << "✅ Parsing successful!\n";
            std::cout << "   Statements parsed: " << statements.size() << "\n";
            
            // Analyze the first statement
            auto& stmt = statements[0];
            std::cout << "   Statement type: ";
            
            // Try to cast to different statement types
            if (auto* exprStmt = dynamic_cast<AST::ExprStmt*>(stmt.get())) {
                std::cout << "ExprStmt\n";
                
                // Check if it's a list comprehension
                if (auto* listComp = dynamic_cast<AST::ListComprehensionExpr*>(exprStmt->expression.get())) {
                    std::cout << "   ✅ Detected: ListComprehensionExpr\n";
                    std::cout << "   Variable: " << listComp->variable << "\n";
                }
                // Check if it's a dict comprehension
                else if (auto* dictComp = dynamic_cast<AST::DictComprehensionExpr*>(exprStmt->expression.get())) {
                    std::cout << "   ✅ Detected: DictComprehensionExpr\n";
                    std::cout << "   Key variable: " << dictComp->keyVariable << "\n";
                    std::cout << "   Value variable: " << dictComp->valueVariable << "\n";
                }
                // Check if it's an array
                else if (auto* array = dynamic_cast<AST::ArrayExpr*>(exprStmt->expression.get())) {
                    std::cout << "   ✅ Detected: ArrayExpr\n";
                    std::cout << "   Elements: " << array->elements.size() << "\n";
                }
                // Check if it's a map
                else if (auto* map = dynamic_cast<AST::MapExpr*>(exprStmt->expression.get())) {
                    std::cout << "   ✅ Detected: MapExpr\n";
                    std::cout << "   Pairs: " << map->pairs.size() << "\n";
                }
                else {
                    std::cout << "Other expression type\n";
                }
            }
            else if (auto* varDecl = dynamic_cast<AST::VarDeclStmt*>(stmt.get())) {
                std::cout << "VarDeclStmt\n";
                std::cout << "   Variable: " << varDecl->name << "\n";
                
                // Check the initializer
                if (auto* listComp = dynamic_cast<AST::ListComprehensionExpr*>(varDecl->initializer.get())) {
                    std::cout << "   ✅ Initializer: ListComprehensionExpr\n";
                }
                else if (auto* dictComp = dynamic_cast<AST::DictComprehensionExpr*>(varDecl->initializer.get())) {
                    std::cout << "   ✅ Initializer: DictComprehensionExpr\n";
                }
            }
            else {
                std::cout << "Other statement type\n";
            }
            
            if (shouldSucceed) {
                testsPassed++;
            } else {
                std::cout << "⚠️  Expected to fail but succeeded!\n";
                testsFailed++;
            }
        } else {
            std::cout << "⚠️  Parsing returned empty program\n";
            if (shouldSucceed) {
                testsFailed++;
            } else {
                testsPassed++;
            }
        }
    } catch (const std::exception& e) {
        std::cout << "❌ Parser error: " << e.what() << "\n";
        if (shouldSucceed) {
            testsFailed++;
        } else {
            std::cout << "   (Expected failure)\n";
            testsPassed++;
        }
    }
    
    std::cout << "\n";
}

int main() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "Parser Comprehensions Test Suite\n";
    std::cout << "========================================\n\n";
    
    // Initialize keywords table
    Lexer::KeywordTable::initialize();
    
    // ========== CATEGORY 1: Basic List Comprehensions ==========
    std::cout << "=== CATEGORY 1: Basic List Comprehensions ===\n\n";
    
    // Test 1.1: Simple list comprehension
    testParser(
        "Test 1.1: Simple list comprehension",
        "[x for x in numbers];"
    );
    
    // Test 1.2: List comprehension with transformation
    testParser(
        "Test 1.2: With transformation",
        "[x * 2 for x in numbers];"
    );
    
    // Test 1.3: List comprehension with condition
    testParser(
        "Test 1.3: With condition",
        "[x for x in numbers if x > 0];"
    );
    
    // Test 1.4: Complex expression
    testParser(
        "Test 1.4: Complex expression",
        "[x * 2 + 1 for x in numbers if x % 2 == 0];"
    );
    
    // Test 1.5: With power operator
    testParser(
        "Test 1.5: Power operator",
        "[x ** 2 for x in numbers];"
    );
    
    // ========== CATEGORY 2: List Comprehensions in Variables ==========
    std::cout << "=== CATEGORY 2: List Comprehensions in Variables ===\n\n";
    
    // Test 2.1: Variable assignment
    testParser(
        "Test 2.1: Variable assignment",
        "var result = [x * 2 for x in numbers];"
    );
    
    // Test 2.2: Const assignment
    testParser(
        "Test 2.2: Const assignment",
        "const squares = [x ** 2 for x in range(10)];"
    );
    
    // Test 2.3: Typed variable
    testParser(
        "Test 2.3: Typed variable",
        "var evens : Array<int> = [x for x in numbers if x % 2 == 0];"
    );
    
    // ========== CATEGORY 3: Dict Comprehensions ==========
    std::cout << "=== CATEGORY 3: Dict Comprehensions ===\n\n";
    
    // Test 3.1: Simple dict comprehension
    testParser(
        "Test 3.1: Simple dict comprehension",
        "{k: v for k, v in items};"
    );
    
    // Test 3.2: With transformation
    testParser(
        "Test 3.2: Dict with transformation",
        "{k: v * 2 for k, v in items};"
    );
    
    // Test 3.3: With condition
    testParser(
        "Test 3.3: Dict with condition",
        "{k: v for k, v in items if v > 0};"
    );
    
    // Test 3.4: Complex keys and values
    testParser(
        "Test 3.4: Complex dict",
        "{k * 2: v ** 2 for k, v in pairs};"
    );
    
    // ========== CATEGORY 4: Dict Comprehensions in Variables ==========
    std::cout << "=== CATEGORY 4: Dict Comprehensions in Variables ===\n\n";
    
    // Test 4.1: Variable assignment
    testParser(
        "Test 4.1: Variable assignment",
        "var mapping = {k: v for k, v in items};"
    );
    
    // Test 4.2: Typed variable
    testParser(
        "Test 4.2: Typed variable",
        "var squares : Map<int, int> = {x: x ** 2 for x, _ in enumerate(range(10))};"
    );
    
    // ========== CATEGORY 5: Arabic Syntax ==========
    std::cout << "=== CATEGORY 5: Arabic Syntax ===\n\n";
    
    // Test 5.1: Arabic list comprehension
    testParser(
        "Test 5.1: Arabic list",
        "[س لكل س في أرقام];"
    );
    
    // Test 5.2: Arabic with condition
    testParser(
        "Test 5.2: Arabic with condition",
        "[س * 2 لكل س في أرقام إذا س > 0];"
    );
    
    // Test 5.3: Arabic dict
    testParser(
        "Test 5.3: Arabic dict",
        "{م: ق لكل م, ق في عناصر};"
    );
    
    // ========== CATEGORY 6: Regular Arrays/Maps (Not Comprehensions) ==========
    std::cout << "=== CATEGORY 6: Regular Arrays/Maps ===\n\n";
    
    // Test 6.1: Regular array
    testParser(
        "Test 6.1: Regular array",
        "[1, 2, 3, 4, 5];"
    );
    
    // Test 6.2: Regular map
    testParser(
        "Test 6.2: Regular map",
        "{\"name\": \"Alice\", \"age\": 30};"
    );
    
    // Test 6.3: Empty array
    testParser(
        "Test 6.3: Empty array",
        "[];"
    );
    
    // Test 6.4: Empty map
    testParser(
        "Test 6.4: Empty map",
        "{};"
    );
    
    // ========== CATEGORY 7: Edge Cases ==========
    std::cout << "=== CATEGORY 7: Edge Cases ===\n\n";
    
    // Test 7.1: Comprehension as function argument
    testParser(
        "Test 7.1: As function argument",
        "print([x for x in numbers]);"
    );
    
    // Test 7.2: Nested comprehension (simulated)
    testParser(
        "Test 7.3: Multiple statements",
        "var a = [x for x in list1]; var b = [y for y in list2];"
    );
    
    // ========== CATEGORY 8: Lambda + Comprehensions ==========
    std::cout << "=== CATEGORY 8: Lambda + Comprehensions ===\n\n";
    
    // Test 8.1: Lambda in comprehension
    testParser(
        "Test 8.1: Lambda in comprehension",
        "[lambda x: x * 2 for x in numbers];"
    );
    
    // Test 8.2: Arrow function in comprehension
    testParser(
        "Test 8.2: Arrow function in comprehension",
        "[(x) => x * 2 for x in numbers];"
    );
    
    // ========== SUMMARY ==========
    std::cout << "========================================\n";
    std::cout << "Test Results Summary\n";
    std::cout << "========================================\n";
    std::cout << "✅ Tests passed: " << testsPassed << "\n";
    std::cout << "❌ Tests failed: " << testsFailed << "\n";
    std::cout << "📊 Success rate: " 
              << (testsPassed * 100 / (testsPassed + testsFailed)) << "%\n";
    std::cout << "========================================\n\n";
    
    return (testsFailed == 0) ? 0 : 1;
}
