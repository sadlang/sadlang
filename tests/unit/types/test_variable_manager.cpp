/**
 * @file test_variable_manager.cpp
 * @brief (AR) اختبارات شاملة لمدير المتغيرات
 * @brief (EN) Comprehensive tests for Variable Manager
 * 
 * الاختبارات المشمولة / Tests Covered:
 * ========================================
 * 1. (AR) عمليات المتغيرات الأساسية (تعريف، تعيين، قراءة)
 *    (EN) Basic variable operations (define, assign, get)
 * 
 * 2. (AR) فحص الوجود والحذف
 *    (EN) Existence checking and removal
 * 
 * 3. (AR) تظليل المتغيرات (Variable Shadowing)
 *    (EN) Variable shadowing
 * 
 * 4. (AR) إدارة النطاقات (دخول وخروج)
 *    (EN) Scope management (enter and exit)
 * 
 * 5. (AR) معالجة الأخطاء (إعادة التعريف، متغيرات غير موجودة)
 *    (EN) Error handling (redefinition, undefined variables)
 * 
 * 6. (AR) استعلامات ومعلومات (عدد المتغيرات، الأسماء)
 *    (EN) Queries and information (count, names)
 * 
 * 7. (AR) سيناريوهات معقدة (دوال متداخلة، حلقات)
 *    (EN) Complex scenarios (nested functions, loops)
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include "../../include/data/managers/variable_manager.h"
#include <iostream>
#include <cassert>
#include <algorithm>

using namespace Sad::Data;

// (AR) عدادات الاختبارات / (EN) Test counters
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
// (AR) اختبارات العمليات الأساسية
// (EN) Basic Operations Tests
// ========================================

TEST(test_create_variable_manager) {
    // (AR) إنشاء مدير متغيرات
    // (EN) Create variable manager
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    // assert(varMgr.getScopeManager() != nullptr);  // getScopeManager() now returns reference
    assert(varMgr.getScopeManager().isGlobalScope());
    assert(varMgr.getVariableCount() == 0);
}

TEST(test_define_integer_variable) {
    // (AR) تعريف متغير عدد صحيح
    // (EN) Define integer variable
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    varMgr.define("x", Value(42));
    
    assert(varMgr.exists("x"));
    assert(varMgr.get("x").toInt() == 42);
}

TEST(test_define_double_variable) {
    // (AR) تعريف متغير عدد عشري
    // (EN) Define double variable
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    varMgr.define("pi", Value(3.14159));
    
    assert(varMgr.exists("pi"));
    assert(std::abs(varMgr.get("pi").toDouble() - 3.14159) < 0.0001);
}

TEST(test_define_string_variable) {
    // (AR) تعريف متغير نصي
    // (EN) Define string variable
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    varMgr.define("name", Value("أحمد"));
    
    assert(varMgr.exists("name"));
    assert(varMgr.get("name").toString() == "أحمد");
}

TEST(test_define_boolean_variable) {
    // (AR) تعريف متغير منطقي
    // (EN) Define boolean variable
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    varMgr.define("flag", Value(true));
    
    assert(varMgr.exists("flag"));
    assert(varMgr.get("flag").toBool() == true);
}

TEST(test_assign_to_existing_variable) {
    // (AR) تعيين قيمة لمتغير موجود
    // (EN) Assign to existing variable
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    varMgr.define("x", Value(10));
    assert(varMgr.get("x").toInt() == 10);
    
    varMgr.assign("x", Value(20));
    assert(varMgr.get("x").toInt() == 20);
}

TEST(test_assign_different_types) {
    // (AR) تعيين أنواع مختلفة لنفس المتغير
    // (EN) Assign different types to same variable
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    varMgr.define("var", Value(42));
    assert(varMgr.get("var").isInteger());
    
    varMgr.assign("var", Value(3.14));
    assert(varMgr.get("var").isDouble());
    
    varMgr.assign("var", Value("text"));
    assert(varMgr.get("var").isString());
}

// ========================================
// (AR) اختبارات فحص الوجود والحذف
// (EN) Existence and Removal Tests
// ========================================

TEST(test_exists_returns_false_for_undefined) {
    // (AR) exists يرجع false للمتغيرات غير المعرفة
    // (EN) exists returns false for undefined variables
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    assert(!varMgr.exists("undefined_var"));
}

TEST(test_exists_returns_true_for_defined) {
    // (AR) exists يرجع true للمتغيرات المعرفة
    // (EN) exists returns true for defined variables
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    varMgr.define("x", Value(10));
    assert(varMgr.exists("x"));
}

TEST(test_remove_variable) {
    // (AR) حذف متغير
    // (EN) Remove variable
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    varMgr.define("temp", Value(99));
    assert(varMgr.exists("temp"));
    
    bool removed = varMgr.remove("temp");
    assert(removed);
    assert(!varMgr.exists("temp"));
}

TEST(test_remove_nonexistent_returns_false) {
    // (AR) حذف متغير غير موجود يرجع false
    // (EN) Removing nonexistent variable returns false
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    bool removed = varMgr.remove("nonexistent");
    assert(!removed);
}

// ========================================
// (AR) اختبارات معالجة الأخطاء
// (EN) Error Handling Tests
// ========================================

TEST(test_cannot_redefine_in_same_scope) {
    // (AR) لا يمكن إعادة تعريف متغير في نفس النطاق
    // (EN) Cannot redefine variable in same scope
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    varMgr.define("x", Value(10));
    
    bool caught = false;
    try {
        varMgr.define("x", Value(20));
    } catch (const std::runtime_error& e) {
        caught = true;
        std::string msg = e.what();
        assert(msg.find("already defined") != std::string::npos ||
               msg.find("معرّف مسبقاً") != std::string::npos);
    }
    
    assert(caught);
}

TEST(test_cannot_assign_to_undefined) {
    // (AR) لا يمكن التعيين لمتغير غير معرف
    // (EN) Cannot assign to undefined variable
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    bool caught = false;
    try {
        varMgr.assign("undefined", Value(10));
    } catch (const std::runtime_error& e) {
        caught = true;
        std::string msg = e.what();
        assert(msg.find("not defined") != std::string::npos ||
               msg.find("غير معرّف") != std::string::npos);
    }
    
    assert(caught);
}

TEST(test_cannot_get_undefined) {
    // (AR) لا يمكن قراءة متغير غير معرف
    // (EN) Cannot get undefined variable
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    bool caught = false;
    try {
        Value val = varMgr.get("undefined");
    } catch (const std::runtime_error& e) {
        caught = true;
    }
    
    assert(caught);
}

// ========================================
// (AR) اختبارات إدارة النطاقات
// (EN) Scope Management Tests
// ========================================

TEST(test_enter_and_exit_scope) {
    // (AR) دخول والخروج من نطاق
    // (EN) Enter and exit scope
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    assert(varMgr.getScopeManager().isGlobalScope());
    
    varMgr.enterScope(ScopeType::FUNCTION, "test");
    assert(!varMgr.getScopeManager().isGlobalScope());
    
    varMgr.exitScope();
    assert(varMgr.getScopeManager().isGlobalScope());
}

TEST(test_variables_in_different_scopes) {
    // (AR) متغيرات في نطاقات مختلفة
    // (EN) Variables in different scopes
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    // (AR) متغير عام
    // (EN) Global variable
    varMgr.define("global_var", Value(100));
    
    // (AR) دخول نطاق دالة
    // (EN) Enter function scope
    varMgr.enterScope(ScopeType::FUNCTION, "func");
    varMgr.define("local_var", Value(200));
    
    // (AR) كلا المتغيرين موجودان
    // (EN) Both variables exist
    assert(varMgr.exists("global_var"));
    assert(varMgr.exists("local_var"));
    
    // (AR) خروج من النطاق
    // (EN) Exit scope
    varMgr.exitScope();
    
    // (AR) المتغير العام موجود، المتغير المحلي اختفى
    // (EN) Global variable exists, local variable gone
    assert(varMgr.exists("global_var"));
    assert(!varMgr.exists("local_var"));
}

TEST(test_scope_cleanup_on_exit) {
    // (AR) تنظيف النطاق عند الخروج
    // (EN) Scope cleanup on exit
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    assert(varMgr.getVariableCount() == 0);
    
    varMgr.enterScope(ScopeType::FUNCTION, "func");
    varMgr.define("a", Value(1));
    varMgr.define("b", Value(2));
    varMgr.define("c", Value(3));
    
    assert(varMgr.getVariableCount() == 3);
    
    varMgr.exitScope();
    
    assert(varMgr.getVariableCount() == 0);
}

// ========================================
// (AR) اختبارات تظليل المتغيرات
// (EN) Variable Shadowing Tests
// ========================================

TEST(test_variable_shadowing) {
    // (AR) تظليل المتغيرات
    // (EN) Variable shadowing
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    // (AR) تعريف x في النطاق العام
    // (EN) Define x in global scope
    varMgr.define("x", Value(10));
    assert(varMgr.get("x").toInt() == 10);
    
    // (AR) دخول نطاق جديد وتظليل x
    // (EN) Enter new scope and shadow x
    varMgr.enterScope(ScopeType::FUNCTION, "func");
    varMgr.define("x", Value(20));  // (AR) تظليل / (EN) Shadowing
    
    // (AR) القيمة الجديدة في النطاق الداخلي
    // (EN) New value in inner scope
    assert(varMgr.get("x").toInt() == 20);
    
    // (AR) خروج من النطاق
    // (EN) Exit scope
    varMgr.exitScope();
    
    // (AR) القيمة الأصلية عادت
    // (EN) Original value restored
    assert(varMgr.get("x").toInt() == 10);
}

TEST(test_assign_updates_nearest_scope) {
    // (AR) assign يحدّث أقرب نطاق
    // (EN) assign updates nearest scope
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    varMgr.define("x", Value(10));
    
    varMgr.enterScope(ScopeType::FUNCTION, "func");
    varMgr.define("x", Value(20));
    
    // (AR) تحديث x في النطاق الحالي
    // (EN) Update x in current scope
    varMgr.assign("x", Value(30));
    assert(varMgr.get("x").toInt() == 30);
    
    varMgr.exitScope();
    
    // (AR) x العام لم يتغير
    // (EN) Global x unchanged
    assert(varMgr.get("x").toInt() == 10);
}

TEST(test_nested_shadowing) {
    // (AR) تظليل متداخل
    // (EN) Nested shadowing
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    varMgr.define("x", Value(1));  // (AR) عام / (EN) Global
    
    varMgr.enterScope(ScopeType::FUNCTION, "func1");
    varMgr.define("x", Value(2));  // (AR) دالة 1 / (EN) Function 1
    
    varMgr.enterScope(ScopeType::BLOCK, "block");
    varMgr.define("x", Value(3));  // (AR) كتلة / (EN) Block
    assert(varMgr.get("x").toInt() == 3);
    
    varMgr.exitScope();  // (AR) خروج من الكتلة / (EN) Exit block
    assert(varMgr.get("x").toInt() == 2);
    
    varMgr.exitScope();  // (AR) خروج من الدالة / (EN) Exit function
    assert(varMgr.get("x").toInt() == 1);
}

// ========================================
// (AR) اختبارات الاستعلامات والمعلومات
// (EN) Query and Information Tests
// ========================================

TEST(test_get_variable_count) {
    // (AR) الحصول على عدد المتغيرات
    // (EN) Get variable count
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    assert(varMgr.getVariableCount() == 0);
    
    varMgr.define("a", Value(1));
    assert(varMgr.getVariableCount() == 1);
    
    varMgr.define("b", Value(2));
    assert(varMgr.getVariableCount() == 2);
    
    varMgr.define("c", Value(3));
    assert(varMgr.getVariableCount() == 3);
}

TEST(test_get_total_variable_count) {
    // (AR) الحصول على العدد الكلي للمتغيرات
    // (EN) Get total variable count
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    varMgr.define("global1", Value(1));
    varMgr.define("global2", Value(2));
    
    varMgr.enterScope(ScopeType::FUNCTION, "func");
    varMgr.define("local1", Value(3));
    varMgr.define("local2", Value(4));
    
    // (AR) المجموع = 2 عام + 2 محلي = 4
    // (EN) Total = 2 global + 2 local = 4
    assert(varMgr.getTotalVariableCount() == 4);
}

TEST(test_get_variable_names) {
    // (AR) الحصول على أسماء المتغيرات
    // (EN) Get variable names
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    varMgr.define("x", Value(1));
    varMgr.define("y", Value(2));
    varMgr.define("z", Value(3));
    
    auto names = varMgr.getVariableNames();
    assert(names.size() == 3);
    
    // (AR) التحقق من وجود الأسماء
    // (EN) Check names exist
    assert(std::find(names.begin(), names.end(), "x") != names.end());
    assert(std::find(names.begin(), names.end(), "y") != names.end());
    assert(std::find(names.begin(), names.end(), "z") != names.end());
}

TEST(test_clear_all_variables) {
    // (AR) مسح جميع المتغيرات
    // (EN) Clear all variables
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    varMgr.define("a", Value(1));
    varMgr.define("b", Value(2));
    
    varMgr.enterScope(ScopeType::FUNCTION, "func");
    varMgr.define("c", Value(3));
    
    assert(varMgr.getTotalVariableCount() == 3);
    
    varMgr.clear();
    
    assert(varMgr.getTotalVariableCount() == 0);
    assert(varMgr.getScopeManager().isGlobalScope());
}

// ========================================
// (AR) اختبارات سيناريوهات معقدة
// (EN) Complex Scenario Tests
// ========================================

TEST(test_realistic_function_scenario) {
    // (AR) سيناريو دالة واقعي
    // (EN) Realistic function scenario
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    // (AR) متغيرات عامة
    // (EN) Global variables
    varMgr.define("PI", Value(3.14159));
    varMgr.define("E", Value(2.71828));
    
    // (AR) دالة calculate
    // (EN) Function calculate
    varMgr.enterScope(ScopeType::FUNCTION, "calculate");
    varMgr.define("result", Value(0.0));
    varMgr.define("temp", Value(1.0));
    
    // (AR) يمكن الوصول للمتغيرات العامة
    // (EN) Can access global variables
    assert(varMgr.exists("PI"));
    assert(varMgr.exists("result"));
    
    // (AR) كتلة if داخل الدالة
    // (EN) If block inside function
    varMgr.enterScope(ScopeType::BLOCK, "if_block");
    varMgr.define("condition", Value(true));
    
    assert(varMgr.exists("PI"));
    assert(varMgr.exists("result"));
    assert(varMgr.exists("condition"));
    
    varMgr.exitScope();  // (AR) خروج من if
    
    // (AR) حلقة for داخل الدالة
    // (EN) For loop inside function
    varMgr.enterScope(ScopeType::LOOP, "for_loop");
    varMgr.define("i", Value(0));
    varMgr.define("sum", Value(0));
    
    assert(varMgr.exists("i"));
    assert(!varMgr.exists("condition"));  // (AR) اختفى / (EN) Gone
    
    varMgr.exitScope();  // (AR) خروج من الحلقة
    varMgr.exitScope();  // (AR) خروج من الدالة
    
    // (AR) فقط المتغيرات العامة باقية
    // (EN) Only global variables remain
    assert(varMgr.exists("PI"));
    assert(!varMgr.exists("result"));
    assert(!varMgr.exists("i"));
}

TEST(test_multiple_functions_with_same_variable_names) {
    // (AR) دوال متعددة بنفس أسماء المتغيرات
    // (EN) Multiple functions with same variable names
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    // (AR) دالة 1
    // (EN) Function 1
    varMgr.enterScope(ScopeType::FUNCTION, "func1");
    varMgr.define("x", Value(10));
    varMgr.define("y", Value(20));
    assert(varMgr.get("x").toInt() == 10);
    varMgr.exitScope();
    
    // (AR) دالة 2 بنفس الأسماء
    // (EN) Function 2 with same names
    varMgr.enterScope(ScopeType::FUNCTION, "func2");
    varMgr.define("x", Value(30));
    varMgr.define("y", Value(40));
    assert(varMgr.get("x").toInt() == 30);
    varMgr.exitScope();
    
    // (AR) لا تعارض، كل دالة لها نطاقها الخاص
    // (EN) No conflict, each function has its own scope
}

TEST(test_lambda_scope) {
    // (AR) نطاق لامدا
    // (EN) Lambda scope
    ScopeManager scopeMgr;
    VariableManager varMgr(scopeMgr);
    
    varMgr.define("outer", Value(100));
    
    varMgr.enterScope(ScopeType::FUNCTION, "main");
    varMgr.define("main_var", Value(200));
    
    // (AR) لامدا داخل الدالة
    // (EN) Lambda inside function
    varMgr.enterScope(ScopeType::LAMBDA, "lambda_1");
    varMgr.define("lambda_var", Value(300));
    
    // (AR) يمكن الوصول لجميع المتغيرات الخارجية
    // (EN) Can access all outer variables
    assert(varMgr.exists("outer"));
    assert(varMgr.exists("main_var"));
    assert(varMgr.exists("lambda_var"));
    
    varMgr.exitScope();
    varMgr.exitScope();
}

// ========================================
// (AR) مدير الاختبارات الرئيسي
// (EN) Main Test Runner
// ========================================

int main() {
    std::cout << "\n╔════════════════════════════════════════════════════╗\n";
    std::cout << "║  VARIABLE MANAGER COMPREHENSIVE TEST SUITE       ║\n";
    std::cout << "║  (AR) مجموعة اختبار شاملة لمدير المتغيرات       ║\n";
    std::cout << "╚════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "=== (AR) العمليات الأساسية / (EN) Basic Operations ===\n";
    test_create_variable_manager_runner();
    test_define_integer_variable_runner();
    test_define_double_variable_runner();
    test_define_string_variable_runner();
    test_define_boolean_variable_runner();
    test_assign_to_existing_variable_runner();
    test_assign_different_types_runner();
    
    std::cout << "\n=== (AR) فحص الوجود والحذف / (EN) Existence & Removal ===\n";
    test_exists_returns_false_for_undefined_runner();
    test_exists_returns_true_for_defined_runner();
    test_remove_variable_runner();
    test_remove_nonexistent_returns_false_runner();
    
    std::cout << "\n=== (AR) معالجة الأخطاء / (EN) Error Handling ===\n";
    test_cannot_redefine_in_same_scope_runner();
    test_cannot_assign_to_undefined_runner();
    test_cannot_get_undefined_runner();
    
    std::cout << "\n=== (AR) إدارة النطاقات / (EN) Scope Management ===\n";
    test_enter_and_exit_scope_runner();
    test_variables_in_different_scopes_runner();
    test_scope_cleanup_on_exit_runner();
    
    std::cout << "\n=== (AR) تظليل المتغيرات / (EN) Variable Shadowing ===\n";
    test_variable_shadowing_runner();
    test_assign_updates_nearest_scope_runner();
    test_nested_shadowing_runner();
    
    std::cout << "\n=== (AR) الاستعلامات / (EN) Queries ===\n";
    test_get_variable_count_runner();
    test_get_total_variable_count_runner();
    test_get_variable_names_runner();
    test_clear_all_variables_runner();
    
    std::cout << "\n=== (AR) سيناريوهات معقدة / (EN) Complex Scenarios ===\n";
    test_realistic_function_scenario_runner();
    test_multiple_functions_with_same_variable_names_runner();
    test_lambda_scope_runner();
    
    // (AR) طباعة الملخص / (EN) Print summary
    std::cout << "\n╔════════════════════════════════════════════════════╗\n";
    if (tests_failed == 0) {
        std::cout << "║   ✅ ALL TESTS PASSED!                            ║\n";
        std::cout << "║   ✅ جميع الاختبارات نجحت!                       ║\n";
    } else {
        std::cout << "║   ❌ SOME TESTS FAILED                            ║\n";
        std::cout << "║   ❌ بعض الاختبارات فشلت                         ║\n";
    }
    std::cout << "╚════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "📊 (AR) النتائج / (EN) Results:\n";
    std::cout << "  ✅ (AR) نجح / (EN) Passed: " << tests_passed << "\n";
    std::cout << "  ❌ (AR) فشل / (EN) Failed: " << tests_failed << "\n";
    std::cout << "  📈 (AR) المجموع / (EN) Total: " << (tests_passed + tests_failed) << "\n";
    std::cout << "  📊 (AR) معدل النجاح / (EN) Success Rate: " 
              << (100.0 * tests_passed / (tests_passed + tests_failed)) << "%\n\n";
    
    return tests_failed > 0 ? 1 : 0;
}
