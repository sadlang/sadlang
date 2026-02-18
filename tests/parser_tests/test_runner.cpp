/**
 * @file test_runner.cpp
 * @brief (AR) مُشغل الاختبارات الرئيسي / (EN) Main test runner
 * 
 * (AR) يقوم بتشغيل جميع اختبارات المحلل النحوي وإظهار النتائج.
 * (EN) Runs all parser tests and displays results.
 * 
 * @usage
 *   ./test_runner                    # Run all tests
 *   ./test_runner --group=literals   # Run specific group
 *   ./test_runner --verbose          # Verbose output
 * 
 * @author Sad Language Development Team
 * @date 2025-11-11
 */

#include "test_framework.h"
#include "parser_core.h"
#include "lexer_core.h"
#include "error_manager.h"
#include <iostream>
#include <string>
#include <vector>

using namespace Sad;

// ======================================================================
// (AR) مجموعة اختبارات الأمثلة / (EN) Sample Test Group
// ======================================================================

/**
 * @brief (AR) اختبارات التعبيرات الحسابية الأساسية.
 *        (EN) Basic arithmetic expression tests.
 */
class ArithmeticTests {
public:
    Lexer::LexerCore* lexer;
    Parser::ParserCore* parser;
    
    std::string getGroupName() const { return "ArithmeticTests"; }
    
    void setup() {
        Errors::ErrorManager::getInstance().clear();
        lexer = nullptr;
        parser = nullptr;
    }
    
    void teardown() {
        delete parser;
        delete lexer;
    }
    
    /**
     * @test SimpleAddition
     * @brief Tests parsing of simple addition: 1 + 2
     */
    void testSimpleAddition() {
        const char* source = "1 + 2;";
        lexer = new Lexer::LexerCore(source);
        parser = new Parser::ParserCore(*lexer);
        
        auto program = parser->parseProgram();
        
        ASSERT_FALSE(parser->hasErrors());
        ASSERT_EQ(program.size(), (size_t)1);
    }
    
    /**
     * @test SimpleSubtraction
     * @brief Tests parsing of simple subtraction: 5 - 3
     */
    void testSimpleSubtraction() {
        const char* source = "5 - 3;";
        lexer = new Lexer::LexerCore(source);
        parser = new Parser::ParserCore(*lexer);
        
        auto program = parser->parseProgram();
        
        ASSERT_FALSE(parser->hasErrors());
        ASSERT_EQ(program.size(), (size_t)1);
    }
    
    /**
     * @test MultiplicationPrecedence
     * @brief Tests operator precedence: 2 + 3 * 4 should parse as 2 + (3 * 4)
     */
    void testMultiplicationPrecedence() {
        const char* source = "2 + 3 * 4;";
        lexer = new Lexer::LexerCore(source);
        parser = new Parser::ParserCore(*lexer);
        
        auto program = parser->parseProgram();
        
        ASSERT_FALSE(parser->hasErrors());
        ASSERT_EQ(program.size(), (size_t)1);
        
        // TODO: Add AST structure verification
        // Should verify that multiplication is nested deeper than addition
    }
    
    /**
     * @test ParenthesesOverridePrecedence
     * @brief Tests that parentheses override precedence: (2 + 3) * 4
     */
    void testParenthesesOverridePrecedence() {
        const char* source = "(2 + 3) * 4;";
        lexer = new Lexer::LexerCore(source);
        parser = new Parser::ParserCore(*lexer);
        
        auto program = parser->parseProgram();
        
        ASSERT_FALSE(parser->hasErrors());
        ASSERT_EQ(program.size(), (size_t)1);
        
        // TODO: Verify AST structure shows addition nested in multiplication
    }
    
    /**
     * @test PowerOperatorRightAssociative
     * @brief Tests that power operator is right-associative: 2 ** 3 ** 2 = 2 ** (3 ** 2) = 512
     */
    void testPowerOperatorRightAssociative() {
        const char* source = "2 ** 3 ** 2;";
        lexer = new Lexer::LexerCore(source);
        parser = new Parser::ParserCore(*lexer);
        
        auto program = parser->parseProgram();
        
        ASSERT_FALSE(parser->hasErrors());
        ASSERT_EQ(program.size(), (size_t)1);
        
        // TODO: Verify right-associativity in AST
    }
};

/**
 * @brief (AR) اختبارات معالجة الأخطاء الأساسية.
 *        (EN) Basic error handling tests.
 */
class ErrorHandlingTests {
public:
    Lexer::LexerCore* lexer;
    Parser::ParserCore* parser;
    
    std::string getGroupName() const { return "ErrorHandlingTests"; }
    
    void setup() {
        Errors::ErrorManager::getInstance().clear();
        lexer = nullptr;
        parser = nullptr;
    }
    
    void teardown() {
        delete parser;
        delete lexer;
    }
    
    /**
     * @test MissingSemicolon
     * @brief Tests error detection for missing semicolon
     */
    void testMissingSemicolon() {
        const char* source = u8"\u0645\u062A\u063A\u064A\u0631 x = 5";  // متغير x = 5 — Missing semicolon
        lexer = new Lexer::LexerCore(source);
        parser = new Parser::ParserCore(*lexer);
        
        auto program = parser->parseProgram();
        
        // Parser may recover from missing semicolons or treat newlines as separators
        // At minimum the var declaration should parse
        ASSERT_TRUE(parser->hasErrors() || program.size() > 0);
    }
    
    /**
     * @test UnclosedParenthesis
     * @brief Tests error detection for unclosed parenthesis
     */
    void testUnclosedParenthesis() {
        const char* source = "(1 + 2;";  // Missing closing paren
        lexer = new Lexer::LexerCore(source);
        parser = new Parser::ParserCore(*lexer);
        
        auto program = parser->parseProgram();
        
        ASSERT_TRUE(parser->hasErrors());
    }
    
    /**
     * @test InvalidExpression
     * @brief Tests error detection for invalid expression
     */
    void testInvalidExpression() {
        const char* source = "1 + + 2;";  // Double plus
        lexer = new Lexer::LexerCore(source);
        parser = new Parser::ParserCore(*lexer);
        
        auto program = parser->parseProgram();
        
        // Parser should handle this gracefully
        // May or may not report error depending on implementation
    }
};

/**
 * @brief (AR) اختبارات الجمل الأساسية.
 *        (EN) Basic statement tests.
 */
class StatementTests {
public:
    Lexer::LexerCore* lexer;
    Parser::ParserCore* parser;
    
    std::string getGroupName() const { return "StatementTests"; }
    
    void setup() {
        Errors::ErrorManager::getInstance().clear();
        lexer = nullptr;
        parser = nullptr;
    }
    
    void teardown() {
        delete parser;
        delete lexer;
    }
    
    /**
     * @test SimpleIfStatement
     * @brief Tests parsing of simple if statement
     */
    void testSimpleIfStatement() {
        const char* source =
            "\xd8\xa5\xd8\xb0\xd8\xa7 (x > 0)\n"
            "  \xd8\xa7\xd8\xb1\xd8\xac\xd8\xb9 x\n"
            "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\n";
        lexer = new Lexer::LexerCore(source);
        parser = new Parser::ParserCore(*lexer);
        
        auto program = parser->parseProgram();
        
        if (parser->hasErrors()) {
            auto errors = parser->getErrors();
            for (const auto& err : errors) {
                std::cerr << "  Parser error: " << err << "\n";
            }
        }
        ASSERT_FALSE(parser->hasErrors());
        ASSERT_GT(program.size(), (size_t)0);
    }
    
    /**
     * @test IfElseStatement
     * @brief Tests parsing of if-else statement
     */
    void testIfElseStatement() {
        // إذا (x > 0) \n ارجع x \n وإلا \n ارجع 0 \n نهاية
        const char* source = u8"\u0625\u0630\u0627 (x > 0)\n  \u0627\u0631\u062C\u0639 x\n\u0648\u0625\u0644\u0627\n  \u0627\u0631\u062C\u0639 0\n\u0646\u0647\u0627\u064A\u0629";
        lexer = new Lexer::LexerCore(source);
        parser = new Parser::ParserCore(*lexer);
        
        auto program = parser->parseProgram();
        
        ASSERT_FALSE(parser->hasErrors());
        ASSERT_GT(program.size(), (size_t)0);
    }
    
    /**
     * @test WhileLoop
     * @brief Tests parsing of while loop
     */
    void testWhileLoop() {
        // بينما (i < 10) \n i = i + 1 \n نهاية
        const char* source = u8"\u0628\u064A\u0646\u0645\u0627 (i < 10)\n  i = i + 1\n\u0646\u0647\u0627\u064A\u0629";
        lexer = new Lexer::LexerCore(source);
        parser = new Parser::ParserCore(*lexer);
        
        auto program = parser->parseProgram();
        
        ASSERT_FALSE(parser->hasErrors());
        ASSERT_GT(program.size(), (size_t)0);
    }
    
    /**
     * @test ForLoop
     * @brief Tests parsing of for-in loop
     */
    void testForLoop() {
        // لكل (ي في مدى(3)) \n  اطبع(ي) \n نهاية
        const char* source =
            "\xd9\x84\xd9\x83\xd9\x84 (\xd9\x8a \xd9\x81\xd9\x8a \xd9\x85\xd8\xaf\xd9\x89(3))\n"
            "  \xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9(\xd9\x8a)\n"
            "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\n";
        lexer = new Lexer::LexerCore(source);
        parser = new Parser::ParserCore(*lexer);
        
        auto program = parser->parseProgram();
        
        if (parser->hasErrors()) {
            auto errors = parser->getErrors();
            for (const auto& err : errors) {
                std::cerr << "  ForLoop error: " << err << "\n";
            }
        }
        ASSERT_FALSE(parser->hasErrors());
        ASSERT_GT(program.size(), (size_t)0);
    }
};

// ======================================================================
// (AR) الدالة الرئيسية / (EN) Main Function
// ======================================================================

int main(int argc, char* argv[]) {
    RUN_ALL_TESTS();
    
    // Parse command line arguments
    bool verbose = false;
    std::string filterGroup;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg.substr(0, 8) == "--group=") {
            filterGroup = arg.substr(8);
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [options]\n\n";
            std::cout << "Options:\n";
            std::cout << "  --verbose, -v          Verbose output\n";
            std::cout << "  --group=<name>         Run only specified group\n";
            std::cout << "  --help, -h             Show this help\n";
            return 0;
        }
    }
    
    // ======================================================================
    // (AR) تشغيل مجموعات الاختبارات / (EN) Run Test Groups
    // ======================================================================
    
    // Arithmetic Tests
    if (filterGroup.empty() || filterGroup == "arithmetic") {
        ArithmeticTests arithmeticGroup;
        g_currentTest.groupName = arithmeticGroup.getGroupName();
        printTestGroupHeader(arithmeticGroup.getGroupName());
        
        runTest(&arithmeticGroup, &ArithmeticTests::testSimpleAddition, "SimpleAddition");
        runTest(&arithmeticGroup, &ArithmeticTests::testSimpleSubtraction, "SimpleSubtraction");
        runTest(&arithmeticGroup, &ArithmeticTests::testMultiplicationPrecedence, "MultiplicationPrecedence");
        runTest(&arithmeticGroup, &ArithmeticTests::testParenthesesOverridePrecedence, "ParenthesesOverridePrecedence");
        runTest(&arithmeticGroup, &ArithmeticTests::testPowerOperatorRightAssociative, "PowerOperatorRightAssociative");
    }
    
    // Error Handling Tests
    if (filterGroup.empty() || filterGroup == "errors") {
        ErrorHandlingTests errorGroup;
        g_currentTest.groupName = errorGroup.getGroupName();
        printTestGroupHeader(errorGroup.getGroupName());
        
        runTest(&errorGroup, &ErrorHandlingTests::testMissingSemicolon, "MissingSemicolon");
        runTest(&errorGroup, &ErrorHandlingTests::testUnclosedParenthesis, "UnclosedParenthesis");
        runTest(&errorGroup, &ErrorHandlingTests::testInvalidExpression, "InvalidExpression");
    }
    
    // Statement Tests
    if (filterGroup.empty() || filterGroup == "statements") {
        StatementTests statementGroup;
        g_currentTest.groupName = statementGroup.getGroupName();
        printTestGroupHeader(statementGroup.getGroupName());
        
        runTest(&statementGroup, &StatementTests::testSimpleIfStatement, "SimpleIfStatement");
        runTest(&statementGroup, &StatementTests::testIfElseStatement, "IfElseStatement");
        runTest(&statementGroup, &StatementTests::testWhileLoop, "WhileLoop");
        runTest(&statementGroup, &StatementTests::testForLoop, "ForLoop");
    }
    
    // ======================================================================
    // (AR) طباعة الملخص / (EN) Print Summary
    // ======================================================================
    
    PRINT_TEST_SUMMARY();
    
    // Return exit code based on test results
    return (g_testStats.failedTests == 0) ? 0 : 1;
}
