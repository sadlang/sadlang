/**
 * @file test_scope_manager.cpp
 * @brief (AR) اختبارات شاملة لمدير النطاقات
 * @brief (EN) Comprehensive tests for Scope Manager
 * 
 * Tests:
 * - Scope creation and destruction
 * - Scope stack management (push/pop)
 * - Variable declaration and lookup
 * - Scope depth tracking
 * - Scope type queries
 * - Variable shadowing
 * - Error handling
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include "../../include/data/scope/scope_manager.h"
#include <iostream>
#include <cassert>

using namespace Sad::Data;

// Test counters
int tests_passed = 0;
int tests_failed = 0;

#define TEST(name) \
    void name(); \
    void name##_runner() { \
        try { \
            name(); \
            tests_passed++; \
            std::cout << "  ✅ PASS: " << #name << "\n"; \
        } catch (const std::exception& e) { \
            tests_failed++; \
            std::cout << "  ❌ FAIL: " << #name << " - " << e.what() << "\n"; \
        } \
    } \
    void name()

// ========================================
// Basic Scope Tests / اختبارات النطاق الأساسية
// ========================================

TEST(test_create_scope_manager) {
    ScopeManager mgr;
    assert(mgr.getCurrentScope() != nullptr);
    assert(mgr.getGlobalScope() != nullptr);
    assert(mgr.isGlobalScope());
    assert(mgr.getCurrentDepth() == 0);
}

TEST(test_global_scope_properties) {
    ScopeManager mgr;
    Scope* global = mgr.getGlobalScope();
    assert(global->isGlobal());
    assert(global->getType() == ScopeType::GLOBAL);
    assert(global->getName() == "global");
    assert(global->getParent() == nullptr);
    assert(global->getDepth() == 0);
}

TEST(test_push_function_scope) {
    ScopeManager mgr;
    mgr.pushScope(ScopeType::FUNCTION, "testFunc");
    
    assert(!mgr.isGlobalScope());
    assert(mgr.getCurrentDepth() == 1);
    assert(mgr.isInFunction());
    assert(mgr.getCurrentScope()->getType() == ScopeType::FUNCTION);
    assert(mgr.getCurrentScope()->getName() == "testFunc");
}

TEST(test_push_block_scope) {
    ScopeManager mgr;
    mgr.pushScope(ScopeType::BLOCK, "if_block");
    
    assert(mgr.getCurrentDepth() == 1);
    assert(mgr.getCurrentScope()->getType() == ScopeType::BLOCK);
}

TEST(test_push_loop_scope) {
    ScopeManager mgr;
    mgr.pushScope(ScopeType::LOOP, "for_loop");
    
    assert(mgr.getCurrentDepth() == 1);
    assert(mgr.isInLoop());
    assert(mgr.getCurrentScope()->isLoop());
}

// ========================================
// Scope Stack Tests / اختبارات مكدس النطاقات
// ========================================

TEST(test_push_pop_scope) {
    ScopeManager mgr;
    assert(mgr.getCurrentDepth() == 0);
    
    mgr.pushScope(ScopeType::FUNCTION, "func1");
    assert(mgr.getCurrentDepth() == 1);
    
    mgr.popScope();
    assert(mgr.getCurrentDepth() == 0);
    assert(mgr.isGlobalScope());
}

TEST(test_nested_scopes) {
    ScopeManager mgr;
    
    mgr.pushScope(ScopeType::FUNCTION, "main");
    assert(mgr.getCurrentDepth() == 1);
    
    mgr.pushScope(ScopeType::BLOCK, "if_block");
    assert(mgr.getCurrentDepth() == 2);
    
    mgr.pushScope(ScopeType::LOOP, "for_loop");
    assert(mgr.getCurrentDepth() == 3);
    
    mgr.popScope();
    assert(mgr.getCurrentDepth() == 2);
    
    mgr.popScope();
    assert(mgr.getCurrentDepth() == 1);
    
    mgr.popScope();
    assert(mgr.getCurrentDepth() == 0);
}

TEST(test_scope_count) {
    ScopeManager mgr;
    assert(mgr.getScopeCount() == 1); // Global scope
    
    mgr.pushScope(ScopeType::FUNCTION);
    assert(mgr.getScopeCount() == 2);
    
    mgr.pushScope(ScopeType::BLOCK);
    assert(mgr.getScopeCount() == 3);
    
    mgr.popScope();
    assert(mgr.getScopeCount() == 2);
    
    mgr.popScope();
    assert(mgr.getScopeCount() == 1);
}

TEST(test_cannot_pop_global_scope) {
    ScopeManager mgr;
    bool caught = false;
    
    try {
        mgr.popScope();
    } catch (const std::runtime_error& e) {
        caught = true;
        std::string msg = e.what();
        assert(msg.find("global") != std::string::npos ||
               msg.find("العام") != std::string::npos);
    }
    
    assert(caught);
}

// ========================================
// Variable Declaration Tests / اختبارات تعريف المتغيرات
// ========================================

TEST(test_declare_variable_in_global) {
    ScopeManager mgr;
    mgr.declareVariable("x");
    
    assert(mgr.isVariableDeclared("x"));
    assert(mgr.getCurrentScope()->hasVariable("x"));
}

TEST(test_declare_variable_in_function) {
    ScopeManager mgr;
    
    mgr.pushScope(ScopeType::FUNCTION, "func");
    mgr.declareVariable("localVar");
    
    assert(mgr.isVariableDeclared("localVar"));
    assert(mgr.getCurrentScope()->hasVariable("localVar"));
}

TEST(test_variable_not_declared) {
    ScopeManager mgr;
    assert(!mgr.isVariableDeclared("undeclared"));
}

TEST(test_cannot_redeclare_in_same_scope) {
    ScopeManager mgr;
    mgr.declareVariable("x");
    
    bool caught = false;
    try {
        mgr.declareVariable("x");
    } catch (const std::runtime_error& e) {
        caught = true;
        std::string msg = e.what();
        assert(msg.find("already declared") != std::string::npos ||
               msg.find("معرّف مسبقاً") != std::string::npos);
    }
    
    assert(caught);
}

// ========================================
// Variable Shadowing Tests / اختبارات تظليل المتغيرات
// ========================================

TEST(test_variable_shadowing) {
    ScopeManager mgr;
    
    // Declare in global scope
    mgr.declareVariable("x");
    assert(mgr.isVariableDeclared("x"));
    
    // Shadow in function scope
    mgr.pushScope(ScopeType::FUNCTION, "func");
    mgr.declareVariable("x");  // Should be allowed (shadowing)
    
    assert(mgr.isVariableDeclared("x"));
    assert(mgr.getCurrentScope()->hasVariable("x"));
    assert(mgr.getGlobalScope()->hasVariable("x"));
}

TEST(test_variable_lookup_chain) {
    ScopeManager mgr;
    
    // Global variable
    mgr.declareVariable("global_var");
    
    // Function scope
    mgr.pushScope(ScopeType::FUNCTION, "func");
    mgr.declareVariable("func_var");
    
    // Can see both
    assert(mgr.isVariableDeclared("global_var"));
    assert(mgr.isVariableDeclared("func_var"));
    
    // Block scope
    mgr.pushScope(ScopeType::BLOCK, "block");
    mgr.declareVariable("block_var");
    
    // Can see all three
    assert(mgr.isVariableDeclared("global_var"));
    assert(mgr.isVariableDeclared("func_var"));
    assert(mgr.isVariableDeclared("block_var"));
    
    mgr.popScope();
    
    // Now can't see block_var
    assert(mgr.isVariableDeclared("global_var"));
    assert(mgr.isVariableDeclared("func_var"));
    assert(!mgr.isVariableDeclared("block_var"));
}

TEST(test_find_variable_scope) {
    ScopeManager mgr;
    
    mgr.declareVariable("global_var");
    
    mgr.pushScope(ScopeType::FUNCTION, "func");
    mgr.declareVariable("func_var");
    
    // Find scopes
    Scope* globalVarScope = mgr.findVariableScope("global_var");
    Scope* funcVarScope = mgr.findVariableScope("func_var");
    Scope* notFoundScope = mgr.findVariableScope("nonexistent");
    
    assert(globalVarScope == mgr.getGlobalScope());
    assert(funcVarScope == mgr.getCurrentScope());
    assert(notFoundScope == nullptr);
}

// ========================================
// Scope Type Query Tests / اختبارات استعلام نوع النطاق
// ========================================

TEST(test_is_in_function) {
    ScopeManager mgr;
    assert(!mgr.isInFunction());
    
    mgr.pushScope(ScopeType::FUNCTION, "func");
    assert(mgr.isInFunction());
    
    mgr.pushScope(ScopeType::BLOCK, "block");
    assert(mgr.isInFunction());  // Still in function
    
    mgr.popScope();
    mgr.popScope();
    assert(!mgr.isInFunction());
}

TEST(test_is_in_loop) {
    ScopeManager mgr;
    assert(!mgr.isInLoop());
    
    mgr.pushScope(ScopeType::FUNCTION, "func");
    assert(!mgr.isInLoop());
    
    mgr.pushScope(ScopeType::LOOP, "for_loop");
    assert(mgr.isInLoop());
    
    mgr.pushScope(ScopeType::BLOCK, "block_inside_loop");
    assert(mgr.isInLoop());  // Still in loop
    
    mgr.popScope();
    mgr.popScope();
    assert(!mgr.isInLoop());
}

TEST(test_find_nearest_scope) {
    ScopeManager mgr;
    
    mgr.pushScope(ScopeType::FUNCTION, "func");
    mgr.pushScope(ScopeType::LOOP, "loop");
    mgr.pushScope(ScopeType::BLOCK, "block");
    
    Scope* funcScope = mgr.findNearestScope(ScopeType::FUNCTION);
    Scope* loopScope = mgr.findNearestScope(ScopeType::LOOP);
    Scope* globalScope = mgr.findNearestScope(ScopeType::GLOBAL);
    Scope* classScope = mgr.findNearestScope(ScopeType::CLASS);
    
    assert(funcScope != nullptr);
    assert(funcScope->getName() == "func");
    
    assert(loopScope != nullptr);
    assert(loopScope->getName() == "loop");
    
    assert(globalScope != nullptr);
    assert(globalScope == mgr.getGlobalScope());
    
    assert(classScope == nullptr);  // No class scope in stack
}

// ========================================
// Complex Scenario Tests / اختبارات سيناريوهات معقدة
// ========================================

TEST(test_realistic_function_with_nested_scopes) {
    ScopeManager mgr;
    
    // Global variables
    mgr.declareVariable("PI");
    mgr.declareVariable("E");
    
    // Function scope
    mgr.pushScope(ScopeType::FUNCTION, "calculate");
    mgr.declareVariable("result");
    mgr.declareVariable("temp");
    
    // If block inside function
    mgr.pushScope(ScopeType::BLOCK, "if_block");
    mgr.declareVariable("condition_var");
    
    // Can see: PI, E, result, temp, condition_var
    assert(mgr.isVariableDeclared("PI"));
    assert(mgr.isVariableDeclared("result"));
    assert(mgr.isVariableDeclared("condition_var"));
    assert(mgr.getCurrentDepth() == 2);
    
    mgr.popScope(); // Exit if block
    
    // Loop inside function
    mgr.pushScope(ScopeType::LOOP, "for_loop");
    mgr.declareVariable("i");
    mgr.declareVariable("sum");
    
    // Can see: PI, E, result, temp, i, sum
    // Can't see: condition_var (was in if block)
    assert(mgr.isVariableDeclared("PI"));
    assert(mgr.isVariableDeclared("result"));
    assert(mgr.isVariableDeclared("i"));
    assert(!mgr.isVariableDeclared("condition_var"));
    
    mgr.popScope(); // Exit loop
    mgr.popScope(); // Exit function
    
    // Back to global
    assert(mgr.isGlobalScope());
    assert(mgr.isVariableDeclared("PI"));
    assert(!mgr.isVariableDeclared("result"));
    assert(!mgr.isVariableDeclared("i"));
}

TEST(test_lambda_scope) {
    ScopeManager mgr;
    
    mgr.declareVariable("outer");
    
    mgr.pushScope(ScopeType::FUNCTION, "main");
    mgr.declareVariable("main_var");
    
    // Lambda inside function
    mgr.pushScope(ScopeType::LAMBDA, "lambda_1");
    mgr.declareVariable("lambda_var");
    
    // Lambda can see outer, main_var, lambda_var
    assert(mgr.isVariableDeclared("outer"));
    assert(mgr.isVariableDeclared("main_var"));
    assert(mgr.isVariableDeclared("lambda_var"));
    assert(mgr.isInFunction());  // Lambda is still "in function"
}

// ========================================
// Debug & Info Tests / اختبارات التصحيح والمعلومات
// ========================================

TEST(test_scope_debug_string) {
    ScopeManager mgr;
    mgr.pushScope(ScopeType::FUNCTION, "testFunc");
    
    std::string debug = mgr.getCurrentScope()->debugString();
    assert(debug.find("FUNCTION") != std::string::npos);
    assert(debug.find("testFunc") != std::string::npos);
    assert(debug.find("depth=1") != std::string::npos);
}

TEST(test_scope_type_names) {
    ScopeManager mgr;
    
    assert(mgr.getGlobalScope()->getTypeName() == "GLOBAL");
    
    mgr.pushScope(ScopeType::FUNCTION);
    assert(mgr.getCurrentScope()->getTypeName() == "FUNCTION");
    mgr.popScope();
    
    mgr.pushScope(ScopeType::BLOCK);
    assert(mgr.getCurrentScope()->getTypeName() == "BLOCK");
    mgr.popScope();
    
    mgr.pushScope(ScopeType::LOOP);
    assert(mgr.getCurrentScope()->getTypeName() == "LOOP");
    mgr.popScope();
    
    mgr.pushScope(ScopeType::LAMBDA);
    assert(mgr.getCurrentScope()->getTypeName() == "LAMBDA");
    mgr.popScope();
    
    mgr.pushScope(ScopeType::CLASS);
    assert(mgr.getCurrentScope()->getTypeName() == "CLASS");
}

// ========================================
// Main Test Runner / مدير الاختبارات
// ========================================

int main() {
    std::cout << "\n╔════════════════════════════════════════════════════╗\n";
    std::cout << "║   SCOPE MANAGER COMPREHENSIVE TEST SUITE         ║\n";
    std::cout << "║   (AR) مجموعة اختبار شاملة لمدير النطاقات        ║\n";
    std::cout << "╚════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "=== Basic Scope Tests ===\n";
    test_create_scope_manager_runner();
    test_global_scope_properties_runner();
    test_push_function_scope_runner();
    test_push_block_scope_runner();
    test_push_loop_scope_runner();
    
    std::cout << "\n=== Scope Stack Tests ===\n";
    test_push_pop_scope_runner();
    test_nested_scopes_runner();
    test_scope_count_runner();
    test_cannot_pop_global_scope_runner();
    
    std::cout << "\n=== Variable Declaration Tests ===\n";
    test_declare_variable_in_global_runner();
    test_declare_variable_in_function_runner();
    test_variable_not_declared_runner();
    test_cannot_redeclare_in_same_scope_runner();
    
    std::cout << "\n=== Variable Shadowing Tests ===\n";
    test_variable_shadowing_runner();
    test_variable_lookup_chain_runner();
    test_find_variable_scope_runner();
    
    std::cout << "\n=== Scope Type Query Tests ===\n";
    test_is_in_function_runner();
    test_is_in_loop_runner();
    test_find_nearest_scope_runner();
    
    std::cout << "\n=== Complex Scenario Tests ===\n";
    test_realistic_function_with_nested_scopes_runner();
    test_lambda_scope_runner();
    
    std::cout << "\n=== Debug & Info Tests ===\n";
    test_scope_debug_string_runner();
    test_scope_type_names_runner();
    
    // Print summary
    std::cout << "\n╔════════════════════════════════════════════════════╗\n";
    if (tests_failed == 0) {
        std::cout << "║   ✅ ALL TESTS PASSED!                            ║\n";
        std::cout << "║   ✅ جميع الاختبارات نجحت!                       ║\n";
    } else {
        std::cout << "║   ❌ SOME TESTS FAILED                            ║\n";
        std::cout << "║   ❌ بعض الاختبارات فشلت                         ║\n";
    }
    std::cout << "╚════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "📊 Results / النتائج:\n";
    std::cout << "  ✅ Passed: " << tests_passed << "\n";
    std::cout << "  ❌ Failed: " << tests_failed << "\n";
    std::cout << "  📈 Total:  " << (tests_passed + tests_failed) << "\n";
    std::cout << "  📊 Success Rate: " 
              << (100.0 * tests_passed / (tests_passed + tests_failed)) << "%\n\n";
    
    return tests_failed > 0 ? 1 : 0;
}
