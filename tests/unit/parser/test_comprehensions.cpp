// ======================================================================
// test_comprehensions.cpp - اختبار Comprehensions / Comprehensions Test
// ======================================================================
// الوصف بالعربية:
//   اختبار شامل لـ list و dict comprehensions
//
// English Description:
//   Comprehensive test for list and dict comprehensions
// ======================================================================

#include "../../include/lexer/lexer_core.h"
#include "../../include/lexer/lexer_keywords.h"
#include <iostream>
#include <string>

using namespace Sad;

void testComprehension(const std::string& testName, const std::string& code) {
    std::cout << "========================================\n";
    std::cout << testName << "\n";
    std::cout << "Code: " << code << "\n\n";
    
    try {
        Lexer::LexerCore lexer(code);
        auto tokens = lexer.tokenize();
        
        std::cout << "Tokens (" << tokens.size() << "):\n";
        for (size_t i = 0; i < tokens.size() && i < 20; i++) {
            std::cout << "  [" << i << "] " 
                      << tokens[i].getTypeName()
                      << " = '" << tokens[i].getValue() << "'\n";
        }
        if (tokens.size() > 20) {
            std::cout << "  ... (" << (tokens.size() - 20) << " more tokens)\n";
        }
        
        std::cout << "\n✅ Tokenization successful!\n\n";
    } catch (const std::exception& e) {
        std::cout << "❌ Error: " << e.what() << "\n\n";
    }
}

int main() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "Comprehensions Test Suite\n";
    std::cout << "========================================\n\n";
    
    // تهيئة الكلمات المفتاحية
    Lexer::KeywordTable::initialize();
    
    // ========== CATEGORY 1: Basic List Comprehensions ==========
    std::cout << "=== CATEGORY 1: Basic List Comprehensions ===\n\n";
    
    // Test 1.1: Simple list comprehension
    testComprehension(
        "Test 1.1: Simple list comprehension",
        "[x for x in numbers]"
    );
    
    // Test 1.2: List comprehension with transformation
    testComprehension(
        "Test 1.2: List comprehension with transformation",
        "[x * 2 for x in numbers]"
    );
    
    // Test 1.3: List comprehension with condition
    testComprehension(
        "Test 1.3: List comprehension with condition",
        "[x for x in numbers if x > 0]"
    );
    
    // Test 1.4: List comprehension with complex expression
    testComprehension(
        "Test 1.4: Complex expression",
        "[x * 2 + 1 for x in numbers if x % 2 == 0]"
    );
    
    // Test 1.5: List comprehension with power operator
    testComprehension(
        "Test 1.5: With power operator",
        "[x ** 2 for x in numbers]"
    );
    
    // ========== CATEGORY 2: Arabic List Comprehensions ==========
    std::cout << "=== CATEGORY 2: Arabic List Comprehensions ===\n\n";
    
    // Test 2.1: Arabic simple
    testComprehension(
        "Test 2.1: Arabic simple",
        "[س لكل س في أرقام]"
    );
    
    // Test 2.2: Arabic with condition
    testComprehension(
        "Test 2.2: Arabic with condition",
        "[س * 2 لكل س في أرقام إذا س > 0]"
    );
    
    // ========== CATEGORY 3: Nested List Comprehensions ==========
    std::cout << "=== CATEGORY 3: Nested List Comprehensions ===\n\n";
    
    // Test 3.1: Nested comprehension
    testComprehension(
        "Test 3.1: Nested comprehension",
        "[[y for y in row] for row in matrix]"
    );
    
    // Test 3.2: Flattened nested
    testComprehension(
        "Test 3.2: Flattened nested (simulated)",
        "[x + y for x in list1 for y in list2]"
    );
    
    // ========== CATEGORY 4: Dict Comprehensions ==========
    std::cout << "=== CATEGORY 4: Dict Comprehensions ===\n\n";
    
    // Test 4.1: Simple dict comprehension
    testComprehension(
        "Test 4.1: Simple dict comprehension",
        "{k: v for k, v in items}"
    );
    
    // Test 4.2: Dict comprehension with transformation
    testComprehension(
        "Test 4.2: Dict with transformation",
        "{k: v * 2 for k, v in items}"
    );
    
    // Test 4.3: Dict comprehension with condition
    testComprehension(
        "Test 4.3: Dict with condition",
        "{k: v for k, v in items if v > 0}"
    );
    
    // Test 4.4: Dict from range
    testComprehension(
        "Test 4.4: Dict from range",
        "{x: x ** 2 for x, _ in enumerate(range(10))}"
    );
    
    // ========== CATEGORY 5: Arabic Dict Comprehensions ==========
    std::cout << "=== CATEGORY 5: Arabic Dict Comprehensions ===\n\n";
    
    // Test 5.1: Arabic dict
    testComprehension(
        "Test 5.1: Arabic dict",
        "{م: ق لكل م, ق في عناصر}"
    );
    
    // Test 5.2: Arabic dict with condition
    testComprehension(
        "Test 5.2: Arabic dict with condition",
        "{م: ق لكل م, ق في عناصر إذا ق > 0}"
    );
    
    // ========== CATEGORY 6: Complex Comprehensions ==========
    std::cout << "=== CATEGORY 6: Complex Comprehensions ===\n\n";
    
    // Test 6.1: Comprehension with function call
    testComprehension(
        "Test 6.1: With function call",
        "[f(x) for x in numbers]"
    );
    
    // Test 6.2: Comprehension with method call
    testComprehension(
        "Test 6.2: With method call",
        "[x.upper() for x in strings]"
    );
    
    // Test 6.3: Comprehension with multiple conditions
    testComprehension(
        "Test 6.3: Multiple conditions",
        "[x for x in numbers if x > 0 and x < 100]"
    );
    
    // Test 6.4: Dict with complex key
    testComprehension(
        "Test 6.4: Dict with complex key",
        "{x * 2: x ** 2 for x, _ in pairs}"
    );
    
    // ========== CATEGORY 7: Edge Cases ==========
    std::cout << "=== CATEGORY 7: Edge Cases ===\n\n";
    
    // Test 7.1: Empty result comprehension
    testComprehension(
        "Test 7.1: All filtered out",
        "[x for x in numbers if x > 1000]"
    );
    
    // Test 7.2: Comprehension in assignment
    testComprehension(
        "Test 7.2: In assignment",
        "var result = [x * 2 for x in numbers];"
    );
    
    // Test 7.3: Comprehension as function argument
    testComprehension(
        "Test 7.3: As function argument",
        "print([x for x in numbers]);"
    );
    
    // Test 7.4: Typed comprehension variable
    testComprehension(
        "Test 7.4: With type annotation (if supported)",
        "[x: int for x in numbers]"
    );
    
    std::cout << "========================================\n";
    std::cout << "All comprehension tests completed!\n";
    std::cout << "========================================\n\n";
    
    // Summary
    std::cout << "Summary of Supported Syntax:\n";
    std::cout << "  List Comprehension:\n";
    std::cout << "    [expr for var in iterable]\n";
    std::cout << "    [expr for var in iterable if condition]\n";
    std::cout << "  Dict Comprehension:\n";
    std::cout << "    {k: v for k, v in iterable}\n";
    std::cout << "    {k: v for k, v in iterable if condition}\n";
    std::cout << "  Arabic Support:\n";
    std::cout << "    [س لكل س في قائمة]\n";
    std::cout << "    {م: ق لكل م, ق في عناصر}\n";
    std::cout << "\n";
    
    return 0;
}
