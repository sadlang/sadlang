// ======================================================================
// test_arrow_functions.cpp - اختبار Arrow Functions / Arrow Functions Test
// ======================================================================
// الوصف بالعربية:
//   اختبار شامل لـ arrow functions مع أنماط مختلفة
//
// English Description:
//   Comprehensive test for arrow functions with different patterns
// ======================================================================

#include "../../include/lexer/lexer_core.h"
#include "../../include/lexer/lexer_keywords.h"
#include <iostream>
#include <string>

using namespace Sad;

void testTokenization(const std::string& testName, const std::string& code) {
    std::cout << "========================================\n";
    std::cout << testName << "\n";
    std::cout << "Code: " << code << "\n\n";
    
    Lexer::LexerCore lexer(code);
    auto tokens = lexer.tokenize();
    
    std::cout << "Tokens (" << tokens.size() << "):\n";
    for (size_t i = 0; i < tokens.size(); i++) {
        std::cout << "  [" << i << "] Type=" 
                  << static_cast<int>(tokens[i].getType())
                  << " (" << tokens[i].getTypeName() << ")"
                  << ", Value='" << tokens[i].getValue() << "'\n";
    }
    std::cout << "\n";
}

int main() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "Arrow Functions Comprehensive Test\n";
    std::cout << "========================================\n\n";
    
    // تهيئة الكلمات المفتاحية
    // Initialize keywords table
    Lexer::KeywordTable::initialize();
    
    // Test 1: Single parameter arrow function
    testTokenization(
        "Test 1: Single parameter (x => x * 2)",
        "x => x * 2"
    );
    
    // Test 2: Multiple parameters arrow function
    testTokenization(
        "Test 2: Multiple parameters ((x, y) => x + y)",
        "(x, y) => x + y"
    );
    
    // Test 3: No parameters arrow function
    testTokenization(
        "Test 3: No parameters (() => 42)",
        "() => 42"
    );
    
    // Test 4: Typed parameters arrow function
    testTokenization(
        "Test 4: Typed parameters ((x: int, y: int) => x + y)",
        "(x: int, y: int) => x + y"
    );
    
    // Test 5: Arrow function with complex expression
    testTokenization(
        "Test 5: Complex expression ((x, y) => x * y + 10)",
        "(x, y) => x * y + 10"
    );
    
    // Test 6: Python-style lambda for comparison
    testTokenization(
        "Test 6: Python-style lambda (lambda x: x * 2)",
        "lambda x: x * 2"
    );
    
    // Test 7: Arabic lambda
    testTokenization(
        "Test 7: Arabic lambda (لامدا س: س * 2)",
        "لامدا س: س * 2"
    );
    
    // Test 8: Nested arrow function
    testTokenization(
        "Test 8: Nested arrow ((x) => (y) => x + y)",
        "(x) => (y) => x + y"
    );
    
    // Test 9: Arrow function in array
    testTokenization(
        "Test 9: Arrow in array ([x => x * 2, y => y + 1])",
        "[x => x * 2, y => y + 1]"
    );
    
    // Test 10: Arrow function with power operator
    testTokenization(
        "Test 10: Arrow with power (x => x ** 2)",
        "x => x ** 2"
    );
    
    std::cout << "========================================\n";
    std::cout << "All tokenization tests completed!\n";
    std::cout << "========================================\n\n";
    
    // Summary
    std::cout << "Summary:\n";
    std::cout << "  - Single param:      x => expr\n";
    std::cout << "  - Multiple params:   (x, y) => expr\n";
    std::cout << "  - No params:         () => expr\n";
    std::cout << "  - Typed params:      (x: int) => expr\n";
    std::cout << "  - Python lambda:     lambda x: expr\n";
    std::cout << "  - Arabic lambda:     لامدا س: تعبير\n";
    std::cout << "\n";
    
    return 0;
}
