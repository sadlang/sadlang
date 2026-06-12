/**
 * @file test_decorator_integration.cpp
 * @brief (AR) اختبار تكامل المُزخرِفات مع الدوال
 * @brief (EN) Test decorator integration with functions
 * 
 * Tests full decorator+function syntax:
 * - @decorator\nfunction name() {}
 * - Multiple stacked decorators
 * - Decorators with arguments
 * - Arabic decorator names
 */

#include "../../include/lexer/lexer_core.h"
#include "../../include/parser/parser_core.h"
#include "../../include/parser/ast/declarations.h"
#include "../../include/parser/ast/expressions.h"
#include <iostream>
#include <cassert>

using namespace Sad;

void testSimpleDecorator() {
    std::cout << "=== Test 1: Simple Decorator + Function ===\n";
    std::cout << "Code: @staticmethod\\nfunction test() {}\n";
    
    std::string code = "@staticmethod\nfunction test() {}";
    Lexer::LexerCore lexer(code);
    Parser::ParserCore parser(lexer);
    
    auto statements = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "Errors found:\n";
        parser.printErrors();
        assert(false && "Parser errors found");
    }
    
    assert(!parser.hasErrors());
    assert(!statements.empty());
    assert(statements.size() == 1);
    
    // Check it's a FunctionDecl
    auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(statements[0].get());
    assert(funcDecl != nullptr);
    assert(funcDecl->name == "test");
    
    // Check decorators
    assert(!funcDecl->decorators.empty());
    assert(funcDecl->decorators.size() == 1);
    
    auto* decorator = dynamic_cast<AST::DecoratorExpr*>(funcDecl->decorators[0].get());
    assert(decorator != nullptr);
    assert(decorator->name == "staticmethod");
    assert(!decorator->hasArguments);
    
    std::cout << "  Function: " << funcDecl->name << "\n";
    std::cout << "  Decorator: " << decorator->toString() << "\n";
    std::cout << "  ✅ PASS\n\n";
}

void testMultipleDecorators() {
    std::cout << "=== Test 2: Multiple Stacked Decorators ===\n";
    std::cout << "Code: @dec1\\n@dec2\\n@dec3\\nfunction test() {}\n";
    
    std::string code = "@dec1\n@dec2\n@dec3\nfunction test() {}";
    Lexer::LexerCore lexer(code);
    Parser::ParserCore parser(lexer);
    
    auto statements = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "Errors found:\n";
        parser.printErrors();
        assert(false && "Parser errors found");
    }
    
    assert(!statements.empty());
    
    auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(statements[0].get());
    assert(funcDecl != nullptr);
    assert(funcDecl->name == "test");
    
    // Check multiple decorators
    assert(funcDecl->decorators.size() == 3);
    
    auto* dec1 = dynamic_cast<AST::DecoratorExpr*>(funcDecl->decorators[0].get());
    auto* dec2 = dynamic_cast<AST::DecoratorExpr*>(funcDecl->decorators[1].get());
    auto* dec3 = dynamic_cast<AST::DecoratorExpr*>(funcDecl->decorators[2].get());
    
    assert(dec1 != nullptr && dec1->name == "dec1");
    assert(dec2 != nullptr && dec2->name == "dec2");
    assert(dec3 != nullptr && dec3->name == "dec3");
    
    std::cout << "  Decorators: " << dec1->toString() << ", " 
              << dec2->toString() << ", " << dec3->toString() << "\n";
    std::cout << "  ✅ PASS\n\n";
}

void testDecoratorWithArguments() {
    std::cout << "=== Test 3: Decorator with Arguments ===\n";
    std::cout << "Code: @cache(100)\\nfunction expensive() {}\n";
    
    std::string code = "@cache(100)\nfunction expensive() { return 42; }";
    Lexer::LexerCore lexer(code);
    Parser::ParserCore parser(lexer);
    
    auto statements = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "Errors found:\n";
        parser.printErrors();
        assert(false && "Parser errors found");
    }
    
    assert(!statements.empty());
    
    auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(statements[0].get());
    assert(funcDecl != nullptr);
    assert(funcDecl->name == "expensive");
    
    // Check decorator with arguments
    assert(funcDecl->decorators.size() == 1);
    
    auto* decorator = dynamic_cast<AST::DecoratorExpr*>(funcDecl->decorators[0].get());
    assert(decorator != nullptr);
    assert(decorator->name == "cache");
    assert(decorator->hasArguments);
    assert(decorator->arguments.size() == 1);
    
    std::cout << "  Decorator: " << decorator->toString() << "\n";
    std::cout << "  ✅ PASS\n\n";
}

void testArabicDecorator() {
    std::cout << "=== Test 4: Arabic Decorator Name ===\n";
    std::cout << "Code: @مُزخرِف\\nدالة اختبار() {}\n";
    
    std::string code = "@مُزخرِف\nدالة اختبار() {}";
    Lexer::LexerCore lexer(code);
    Parser::ParserCore parser(lexer);
    
    auto statements = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "Errors found:\n";
        parser.printErrors();
        assert(false && "Parser errors found");
    }
    
    assert(!statements.empty());
    
    auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(statements[0].get());
    assert(funcDecl != nullptr);
    assert(funcDecl->name == "اختبار");
    
    auto* decorator = dynamic_cast<AST::DecoratorExpr*>(funcDecl->decorators[0].get());
    assert(decorator != nullptr);
    assert(decorator->name == "مُزخرِف");
    
    std::cout << "  Function: " << funcDecl->name << "\n";
    std::cout << "  Decorator: " << decorator->toString() << "\n";
    std::cout << "  ✅ PASS\n\n";
}

void testComplexDecorators() {
    std::cout << "=== Test 5: Complex Decorator Pattern ===\n";
    std::cout << "Code: @auth\\n@cache(maxsize=100, ttl=60)\\nfunction api() {}\n";
    
    std::string code = "@auth\n@cache(100, 60)\nfunction api() { return true; }";
    Lexer::LexerCore lexer(code);
    Parser::ParserCore parser(lexer);
    
    auto statements = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "Errors found:\n";
        parser.printErrors();
        assert(false && "Parser errors found");
    }
    
    assert(!statements.empty());
    
    auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(statements[0].get());
    assert(funcDecl != nullptr);
    assert(funcDecl->decorators.size() == 2);
    
    auto* dec1 = dynamic_cast<AST::DecoratorExpr*>(funcDecl->decorators[0].get());
    auto* dec2 = dynamic_cast<AST::DecoratorExpr*>(funcDecl->decorators[1].get());
    
    assert(dec1 != nullptr && dec1->name == "auth");
    assert(!dec1->hasArguments);
    
    assert(dec2 != nullptr && dec2->name == "cache");
    assert(dec2->hasArguments);
    assert(dec2->arguments.size() == 2);
    
    std::cout << "  Decorator 1: " << dec1->toString() << " (no args)\n";
    std::cout << "  Decorator 2: " << dec2->toString() << " (2 args)\n";
    std::cout << "  ✅ PASS\n\n";
}

void testFunctionWithoutDecorator() {
    std::cout << "=== Test 6: Function Without Decorator (Backwards Compatibility) ===\n";
    std::cout << "Code: function normal() {}\n";
    
    std::string code = "function normal() { return 1; }";
    Lexer::LexerCore lexer(code);
    Parser::ParserCore parser(lexer);
    
    auto statements = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "Errors found:\n";
        parser.printErrors();
        assert(false && "Parser errors found");
    }
    
    assert(!statements.empty());
    
    auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(statements[0].get());
    assert(funcDecl != nullptr);
    assert(funcDecl->name == "normal");
    assert(funcDecl->decorators.empty());
    
    std::cout << "  Function: " << funcDecl->name << " (no decorators)\n";
    std::cout << "  ✅ PASS - Backwards compatibility maintained\n\n";
}

int main() {
    std::cout << "=== Testing Decorator Integration (Task 4.4 & 4.5) ===\n\n";
    
    try {
        testSimpleDecorator();
        testMultipleDecorators();
        testDecoratorWithArguments();
        testArabicDecorator();
        testComplexDecorators();
        testFunctionWithoutDecorator();
        
        std::cout << "=== ALL INTEGRATION TESTS PASSED ===\n\n";
        std::cout << "✅ Task 4.1: AT_SIGN token          - COMPLETE\n";
        std::cout << "✅ Task 4.2: DecoratorExpr AST      - COMPLETE\n";
        std::cout << "✅ Task 4.3: parseDecorator()       - COMPLETE\n";
        std::cout << "✅ Task 4.4: Function integration   - COMPLETE\n";
        std::cout << "✅ Task 4.5: Integration tests      - COMPLETE\n";
        std::cout << "⏳ Task 4.6: Documentation          - NEXT\n\n";
        
        std::cout << "Decorator implementation ready! 🎉\n";
        
    } catch (const std::exception& e) {
        std::cerr << "❌ TEST FAILED: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
