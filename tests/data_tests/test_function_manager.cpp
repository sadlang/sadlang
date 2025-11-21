/**
 * @file test_function_manager.cpp
 * @brief (AR) اختبار شامل لمدير الدوال
 * @brief (EN) Comprehensive test for Function Manager
 * 
 * يختبر جميع وظائف مدير الدوال:
 * Tests all Function Manager functionalities:
 * - Function definition (user-defined and built-in)
 * - Function lookup by name and parameter count
 * - Function overloading support
 * - Parameter matching (required vs optional)
 * - Error handling
 * - Query operations
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include "data/managers/function_manager.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>

using namespace Sad::Data;
using namespace Sad::Parser;

// ============================================================================
// Test Helpers / مساعدات الاختبار
// ============================================================================

int g_testsRun = 0;
int g_testsPassed = 0;
int g_testsFailed = 0;

void TEST_START(const std::string& testName) {
    g_testsRun++;
    std::cout << "  Testing: " << testName << " ... ";
}

void TEST_PASS() {
    g_testsPassed++;
    std::cout << "✅ PASS\n";
}

void TEST_FAIL(const std::string& reason) {
    g_testsFailed++;
    std::cout << "❌ FAIL: " << reason << "\n";
}

// (AR) مساعد لإنشاء جسم دالة وهمي (nullptr لأننا لا نحتاج AST حقيقي في الاختبارات)
// (EN) Helper to create dummy function body (nullptr since we don't need real AST in tests)
std::shared_ptr<ASTNode> createDummyBody() {
    return nullptr; // (AR) جسم وهمي / (EN) Dummy body
}

// ============================================================================
// Basic Operations Tests / اختبارات العمليات الأساسية
// ============================================================================

void test_create_function_manager() {
    TEST_START("test_create_function_manager");
    
    FunctionManager funcMgr;
    assert(funcMgr.getFunctionCount() == 0);
    assert(funcMgr.getFunctionNames().empty());
    
    TEST_PASS();
}

void test_define_simple_function() {
    TEST_START("test_define_simple_function");
    
    FunctionManager funcMgr;
    
    std::vector<FunctionParameter> params = {
        FunctionParameter("x"),
        FunctionParameter("y")
    };
    
    funcMgr.defineFunction("add", params, createDummyBody());
    
    assert(funcMgr.getFunctionCount() == 1);
    assert(funcMgr.hasFunction("add"));
    assert(funcMgr.hasFunction("add", 2));
    
    TEST_PASS();
}

void test_define_function_with_no_parameters() {
    TEST_START("test_define_function_with_no_parameters");
    
    FunctionManager funcMgr;
    
    std::vector<FunctionParameter> params; // Empty
    funcMgr.defineFunction("hello", params, createDummyBody());
    
    assert(funcMgr.getFunctionCount() == 1);
    assert(funcMgr.hasFunction("hello", 0));
    
    TEST_PASS();
}

void test_define_function_with_many_parameters() {
    TEST_START("test_define_function_with_many_parameters");
    
    FunctionManager funcMgr;
    
    std::vector<FunctionParameter> params = {
        FunctionParameter("a"),
        FunctionParameter("b"),
        FunctionParameter("c"),
        FunctionParameter("d"),
        FunctionParameter("e")
    };
    
    funcMgr.defineFunction("complexFunc", params, createDummyBody());
    
    assert(funcMgr.hasFunction("complexFunc", 5));
    
    TEST_PASS();
}

void test_define_built_in_function() {
    TEST_START("test_define_built_in_function");
    
    FunctionManager funcMgr;
    
    std::vector<FunctionParameter> params = {
        FunctionParameter("text")
    };
    
    auto printImpl = []() {
        // Dummy implementation
    };
    
    funcMgr.defineBuiltInFunction("print", params, printImpl);
    
    assert(funcMgr.hasFunction("print", 1));
    
    auto func = funcMgr.getFunction("print", 1);
    assert(func != nullptr);
    assert(func->getType() == FunctionType::BUILT_IN);
    
    TEST_PASS();
}

// ============================================================================
// Function Lookup Tests / اختبارات البحث عن الدوال
// ============================================================================

void test_get_function_returns_nullptr_for_undefined() {
    TEST_START("test_get_function_returns_nullptr_for_undefined");
    
    FunctionManager funcMgr;
    
    auto func = funcMgr.getFunction("undefined", 0);
    assert(func == nullptr);
    
    TEST_PASS();
}

void test_get_function_returns_correct_function() {
    TEST_START("test_get_function_returns_correct_function");
    
    FunctionManager funcMgr;
    
    std::vector<FunctionParameter> params = {
        FunctionParameter("x")
    };
    
    funcMgr.defineFunction("square", params, createDummyBody());
    
    auto func = funcMgr.getFunction("square", 1);
    assert(func != nullptr);
    assert(func->getName() == "square");
    assert(func->getParameterCount() == 1);
    
    TEST_PASS();
}

void test_has_function_returns_false_for_undefined() {
    TEST_START("test_has_function_returns_false_for_undefined");
    
    FunctionManager funcMgr;
    
    assert(!funcMgr.hasFunction("undefined"));
    assert(!funcMgr.hasFunction("undefined", 0));
    
    TEST_PASS();
}

void test_has_function_returns_true_for_defined() {
    TEST_START("test_has_function_returns_true_for_defined");
    
    FunctionManager funcMgr;
    
    std::vector<FunctionParameter> params = {
        FunctionParameter("n")
    };
    
    funcMgr.defineFunction("factorial", params, createDummyBody());
    
    assert(funcMgr.hasFunction("factorial"));
    assert(funcMgr.hasFunction("factorial", 1));
    assert(!funcMgr.hasFunction("factorial", 2));
    
    TEST_PASS();
}

// ============================================================================
// Function Overloading Tests / اختبارات تعدد التوقيعات
// ============================================================================

void test_function_overloading() {
    TEST_START("test_function_overloading");
    
    FunctionManager funcMgr;
    
    // (AR) دالة بمعامل واحد
    // (EN) Function with one parameter
    std::vector<FunctionParameter> params1 = {
        FunctionParameter("x")
    };
    funcMgr.defineFunction("compute", params1, createDummyBody());
    
    // (AR) دالة بمعاملين
    // (EN) Function with two parameters
    std::vector<FunctionParameter> params2 = {
        FunctionParameter("x"),
        FunctionParameter("y")
    };
    funcMgr.defineFunction("compute", params2, createDummyBody());
    
    assert(funcMgr.getFunctionCount() == 2);
    assert(funcMgr.hasFunction("compute", 1));
    assert(funcMgr.hasFunction("compute", 2));
    
    auto func1 = funcMgr.getFunction("compute", 1);
    auto func2 = funcMgr.getFunction("compute", 2);
    
    assert(func1 != nullptr);
    assert(func2 != nullptr);
    assert(func1->getParameterCount() == 1);
    assert(func2->getParameterCount() == 2);
    
    TEST_PASS();
}

void test_get_function_overloads() {
    TEST_START("test_get_function_overloads");
    
    FunctionManager funcMgr;
    
    // Define 3 overloads
    std::vector<FunctionParameter> params0;
    funcMgr.defineFunction("test", params0, createDummyBody());
    
    std::vector<FunctionParameter> params1 = { FunctionParameter("a") };
    funcMgr.defineFunction("test", params1, createDummyBody());
    
    std::vector<FunctionParameter> params2 = { 
        FunctionParameter("a"), 
        FunctionParameter("b") 
    };
    funcMgr.defineFunction("test", params2, createDummyBody());
    
    auto overloads = funcMgr.getFunctionOverloads("test");
    assert(overloads.size() == 3);
    
    TEST_PASS();
}

// ============================================================================
// Parameter Matching Tests / اختبارات مطابقة المعاملات
// ============================================================================

void test_function_with_default_parameters() {
    TEST_START("test_function_with_default_parameters");
    
    FunctionManager funcMgr;
    
    std::vector<FunctionParameter> params = {
        FunctionParameter("x"),
        FunctionParameter("y", "", true, "10"),  // Has default value
        FunctionParameter("z", "", true, "20")   // Has default value
    };
    
    funcMgr.defineFunction("func", params, createDummyBody());
    
    auto func = funcMgr.getFunction("func", 1);  // Only required param
    assert(func != nullptr);
    assert(func->matchesParameterCount(1));
    assert(func->matchesParameterCount(2));
    assert(func->matchesParameterCount(3));
    assert(!func->matchesParameterCount(0));
    assert(!func->matchesParameterCount(4));
    
    TEST_PASS();
}

void test_parameter_count_matching() {
    TEST_START("test_parameter_count_matching");
    
    FunctionManager funcMgr;
    
    std::vector<FunctionParameter> params = {
        FunctionParameter("a"),
        FunctionParameter("b", "", true, "5")
    };
    
    funcMgr.defineFunction("calc", params, createDummyBody());
    
    // (AR) يجب أن يقبل 1 أو 2 معاملات
    // (EN) Should accept 1 or 2 arguments
    assert(funcMgr.hasFunction("calc", 1));
    assert(funcMgr.hasFunction("calc", 2));
    assert(!funcMgr.hasFunction("calc", 0));
    assert(!funcMgr.hasFunction("calc", 3));
    
    TEST_PASS();
}

// ============================================================================
// Error Handling Tests / اختبارات معالجة الأخطاء
// ============================================================================

void test_cannot_define_function_without_name() {
    TEST_START("test_cannot_define_function_without_name");
    
    FunctionManager funcMgr;
    std::vector<FunctionParameter> params;
    
    try {
        funcMgr.defineFunction("", params, createDummyBody());
        TEST_FAIL("Should throw error for empty name");
    } catch (const std::runtime_error&) {
        TEST_PASS();
    }
}

void test_cannot_redefine_function_with_same_signature() {
    TEST_START("test_cannot_redefine_function_with_same_signature");
    
    FunctionManager funcMgr;
    
    std::vector<FunctionParameter> params = {
        FunctionParameter("x")
    };
    
    funcMgr.defineFunction("duplicate", params, createDummyBody());
    
    try {
        funcMgr.defineFunction("duplicate", params, createDummyBody());
        TEST_FAIL("Should throw error for duplicate function");
    } catch (const std::runtime_error&) {
        TEST_PASS();
    }
}

// ============================================================================
// Removal Tests / اختبارات الحذف
// ============================================================================

void test_remove_function() {
    TEST_START("test_remove_function");
    
    FunctionManager funcMgr;
    
    std::vector<FunctionParameter> params = {
        FunctionParameter("x")
    };
    
    funcMgr.defineFunction("temp", params, createDummyBody());
    assert(funcMgr.hasFunction("temp"));
    
    size_t removed = funcMgr.removeFunction("temp");
    assert(removed == 1);
    assert(!funcMgr.hasFunction("temp"));
    
    TEST_PASS();
}

void test_remove_specific_overload() {
    TEST_START("test_remove_specific_overload");
    
    FunctionManager funcMgr;
    
    // Define 2 overloads
    std::vector<FunctionParameter> params1 = { FunctionParameter("a") };
    funcMgr.defineFunction("overloaded", params1, createDummyBody());
    
    std::vector<FunctionParameter> params2 = { 
        FunctionParameter("a"), 
        FunctionParameter("b") 
    };
    funcMgr.defineFunction("overloaded", params2, createDummyBody());
    
    assert(funcMgr.getFunctionCount() == 2);
    
    // Remove only the 1-parameter version
    size_t removed = funcMgr.removeFunction("overloaded", 1);
    assert(removed == 1);
    assert(!funcMgr.hasFunction("overloaded", 1));
    assert(funcMgr.hasFunction("overloaded", 2));
    assert(funcMgr.getFunctionCount() == 1);
    
    TEST_PASS();
}

void test_remove_all_overloads() {
    TEST_START("test_remove_all_overloads");
    
    FunctionManager funcMgr;
    
    // Define 3 overloads
    for (int i = 0; i < 3; i++) {
        std::vector<FunctionParameter> params;
        for (int j = 0; j <= i; j++) {
            params.push_back(FunctionParameter("p" + std::to_string(j)));
        }
        funcMgr.defineFunction("multi", params, createDummyBody());
    }
    
    assert(funcMgr.getFunctionCount() == 3);
    
    // Remove all
    size_t removed = funcMgr.removeFunction("multi", -1);
    assert(removed == 3);
    assert(!funcMgr.hasFunction("multi"));
    assert(funcMgr.getFunctionCount() == 0);
    
    TEST_PASS();
}

void test_remove_nonexistent_returns_zero() {
    TEST_START("test_remove_nonexistent_returns_zero");
    
    FunctionManager funcMgr;
    
    size_t removed = funcMgr.removeFunction("notexists");
    assert(removed == 0);
    
    TEST_PASS();
}

// ============================================================================
// Query Tests / اختبارات الاستعلام
// ============================================================================

void test_get_function_names() {
    TEST_START("test_get_function_names");
    
    FunctionManager funcMgr;
    
    std::vector<FunctionParameter> params;
    
    funcMgr.defineFunction("alpha", params, createDummyBody());
    funcMgr.defineFunction("beta", params, createDummyBody());
    funcMgr.defineFunction("gamma", params, createDummyBody());
    
    auto names = funcMgr.getFunctionNames();
    assert(names.size() == 3);
    
    // (AR) التحقق من الترتيب الأبجدي
    // (EN) Check alphabetical order
    assert(names[0] == "alpha");
    assert(names[1] == "beta");
    assert(names[2] == "gamma");
    
    TEST_PASS();
}

void test_get_function_count() {
    TEST_START("test_get_function_count");
    
    FunctionManager funcMgr;
    
    assert(funcMgr.getFunctionCount() == 0);
    
    std::vector<FunctionParameter> params;
    funcMgr.defineFunction("f1", params, createDummyBody());
    assert(funcMgr.getFunctionCount() == 1);
    
    funcMgr.defineFunction("f2", params, createDummyBody());
    assert(funcMgr.getFunctionCount() == 2);
    
    // Add overload
    std::vector<FunctionParameter> params1 = { FunctionParameter("x") };
    funcMgr.defineFunction("f1", params1, createDummyBody());
    assert(funcMgr.getFunctionCount() == 3);  // Counts overloads
    
    TEST_PASS();
}

void test_clear_all_functions() {
    TEST_START("test_clear_all_functions");
    
    FunctionManager funcMgr;
    
    std::vector<FunctionParameter> params;
    funcMgr.defineFunction("func1", params, createDummyBody());
    funcMgr.defineFunction("func2", params, createDummyBody());
    funcMgr.defineFunction("func3", params, createDummyBody());
    
    assert(funcMgr.getFunctionCount() == 3);
    
    funcMgr.clear();
    
    assert(funcMgr.getFunctionCount() == 0);
    assert(funcMgr.getFunctionNames().empty());
    
    TEST_PASS();
}

// ============================================================================
// Function Definition Tests / اختبارات تعريف الدالة
// ============================================================================

void test_function_signature() {
    TEST_START("test_function_signature");
    
    FunctionManager funcMgr;
    
    std::vector<FunctionParameter> params = {
        FunctionParameter("x"),
        FunctionParameter("y", "int"),
        FunctionParameter("z", "double", true, "3.14")
    };
    
    funcMgr.defineFunction("test", params, createDummyBody());
    
    auto func = funcMgr.getFunction("test", 2);
    assert(func != nullptr);
    
    std::string sig = func->getSignature();
    assert(!sig.empty());
    assert(sig.find("test") != std::string::npos);
    assert(sig.find("x") != std::string::npos);
    
    TEST_PASS();
}

void test_function_type() {
    TEST_START("test_function_type");
    
    FunctionManager funcMgr;
    
    std::vector<FunctionParameter> params;
    
    // User-defined function
    funcMgr.defineFunction("userFunc", params, createDummyBody());
    auto userFunc = funcMgr.getFunction("userFunc", 0);
    assert(userFunc->getType() == FunctionType::USER_DEFINED);
    
    // Built-in function
    funcMgr.defineBuiltInFunction("builtIn", params, [](){});
    auto builtIn = funcMgr.getFunction("builtIn", 0);
    assert(builtIn->getType() == FunctionType::BUILT_IN);
    
    TEST_PASS();
}

// ============================================================================
// Complex Scenarios / سيناريوهات معقدة
// ============================================================================

void test_multiple_functions_with_overloads() {
    TEST_START("test_multiple_functions_with_overloads");
    
    FunctionManager funcMgr;
    
    // Function 'add' with 2 overloads
    std::vector<FunctionParameter> add2 = { 
        FunctionParameter("a"), 
        FunctionParameter("b") 
    };
    funcMgr.defineFunction("add", add2, createDummyBody());
    
    std::vector<FunctionParameter> add3 = { 
        FunctionParameter("a"), 
        FunctionParameter("b"),
        FunctionParameter("c")
    };
    funcMgr.defineFunction("add", add3, createDummyBody());
    
    // Function 'multiply' with 2 overloads
    funcMgr.defineFunction("multiply", add2, createDummyBody());
    funcMgr.defineFunction("multiply", add3, createDummyBody());
    
    assert(funcMgr.getFunctionCount() == 4);
    assert(funcMgr.getFunctionNames().size() == 2);
    
    auto addOverloads = funcMgr.getFunctionOverloads("add");
    assert(addOverloads.size() == 2);
    
    auto mulOverloads = funcMgr.getFunctionOverloads("multiply");
    assert(mulOverloads.size() == 2);
    
    TEST_PASS();
}

void test_realistic_program_scenario() {
    TEST_START("test_realistic_program_scenario");
    
    FunctionManager funcMgr;
    
    // Define common functions
    std::vector<FunctionParameter> noParams;
    funcMgr.defineFunction("main", noParams, createDummyBody());
    
    std::vector<FunctionParameter> oneParam = { FunctionParameter("x") };
    funcMgr.defineBuiltInFunction("print", oneParam, [](){});
    funcMgr.defineBuiltInFunction("input", noParams, [](){});
    
    std::vector<FunctionParameter> twoParams = { 
        FunctionParameter("a"), 
        FunctionParameter("b") 
    };
    funcMgr.defineFunction("add", twoParams, createDummyBody());
    funcMgr.defineFunction("subtract", twoParams, createDummyBody());
    funcMgr.defineFunction("multiply", twoParams, createDummyBody());
    funcMgr.defineFunction("divide", twoParams, createDummyBody());
    
    // Verify all functions exist
    assert(funcMgr.hasFunction("main", 0));
    assert(funcMgr.hasFunction("print", 1));
    assert(funcMgr.hasFunction("input", 0));
    assert(funcMgr.hasFunction("add", 2));
    assert(funcMgr.hasFunction("subtract", 2));
    assert(funcMgr.hasFunction("multiply", 2));
    assert(funcMgr.hasFunction("divide", 2));
    
    assert(funcMgr.getFunctionCount() == 7);
    
    TEST_PASS();
}

// ============================================================================
// Main Test Runner / منفذ الاختبارات الرئيسي
// ============================================================================

int main() {
    std::cout << "╔════════════════════════════════════════════════════╗\n";
    std::cout << "║  FUNCTION MANAGER COMPREHENSIVE TEST SUITE       ║\n";
    std::cout << "║  (AR) مجموعة اختبار شاملة لمدير الدوال          ║\n";
    std::cout << "╚════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "=== Basic Operations ===\n";
    test_create_function_manager();
    test_define_simple_function();
    test_define_function_with_no_parameters();
    test_define_function_with_many_parameters();
    test_define_built_in_function();
    
    std::cout << "\n=== Function Lookup ===\n";
    test_get_function_returns_nullptr_for_undefined();
    test_get_function_returns_correct_function();
    test_has_function_returns_false_for_undefined();
    test_has_function_returns_true_for_defined();
    
    std::cout << "\n=== Function Overloading ===\n";
    test_function_overloading();
    test_get_function_overloads();
    
    std::cout << "\n=== Parameter Matching ===\n";
    test_function_with_default_parameters();
    test_parameter_count_matching();
    
    std::cout << "\n=== Error Handling ===\n";
    test_cannot_define_function_without_name();
    test_cannot_redefine_function_with_same_signature();
    
    std::cout << "\n=== Removal Operations ===\n";
    test_remove_function();
    test_remove_specific_overload();
    test_remove_all_overloads();
    test_remove_nonexistent_returns_zero();
    
    std::cout << "\n=== Query Operations ===\n";
    test_get_function_names();
    test_get_function_count();
    test_clear_all_functions();
    
    std::cout << "\n=== Function Definitions ===\n";
    test_function_signature();
    test_function_type();
    
    std::cout << "\n=== Complex Scenarios ===\n";
    test_multiple_functions_with_overloads();
    test_realistic_program_scenario();
    
    // Print summary
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════╗\n";
    if (g_testsFailed == 0) {
        std::cout << "║   ✅ ALL TESTS PASSED!                            ║\n";
        std::cout << "║   ✅ جميع الاختبارات نجحت!                       ║\n";
    } else {
        std::cout << "║   ❌ SOME TESTS FAILED!                           ║\n";
        std::cout << "║   ❌ بعض الاختبارات فشلت!                        ║\n";
    }
    std::cout << "╚════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "📊 Results:\n";
    std::cout << "  ✅ Passed: " << g_testsPassed << "\n";
    std::cout << "  ❌ Failed: " << g_testsFailed << "\n";
    std::cout << "  📈 Total: " << g_testsRun << "\n";
    
    if (g_testsRun > 0) {
        double successRate = (g_testsPassed * 100.0) / g_testsRun;
        std::cout << "  📊 Success Rate: " << successRate << "%\n";
    }
    
    return (g_testsFailed == 0) ? 0 : 1;
}
