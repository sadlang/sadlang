/**
 * @file test_end_to_end.cpp
 * @brief (AR) اختبارات التكامل الشاملة لنظام الوحدات
 *        (EN) End-to-end integration tests for module system
 * 
 * @details
 * (AR) اختبار كامل يمر عبر جميع مراحل نظام الوحدات:
 *      1. تحميل الوحدة (ModuleLoader)
 *      2. حل الرموز (SymbolResolver)
 *      3. التحقق من الصحة (ModuleValidator)
 *      4. استخدام الرموز المصدرة
 * 
 * (EN) Complete test passing through all module system phases:
 *      1. Module loading (ModuleLoader)
 *      2. Symbol resolution (SymbolResolver)
 *      3. Validation (ModuleValidator)
 *      4. Using exported symbols
 * 
 * @author Sad Language Development Team
 * @date December 8, 2025
 */

#include <gtest/gtest.h>
#include "../../../include/modules/module_loader.h"
#include "../../../include/modules/module_validator.h"
#include "../../../include/modules/symbol_resolver.h"
#include "../../../include/modules/search_paths.h"
#include <fstream>
#include <filesystem>
#include <iostream>

using namespace Sad::Modules;

// =========================================================================
// Helper Functions
// =========================================================================

void createTestModule(const std::string& filename, const std::string& content) {
    std::filesystem::create_directories("test_integration");
    std::ofstream file("test_integration/" + filename);
    file << content;
    file.close();
}

void cleanup() {
    if (std::filesystem::exists("test_integration")) {
        std::filesystem::remove_all("test_integration");
    }
}

// =========================================================================
// Test 1: Complete Module Workflow
// =========================================================================

TEST(IntegrationEndToEnd, CompleteModuleWorkflow) {
    std::cout << "\n=== Test 1: Complete Module Workflow (سير عمل كامل للوحدة) ===" << std::endl;
    
    cleanup();
    
    // إنشاء وحدة حاسبة / Create calculator module
    createTestModule("calculator.s", R"(
export function add(x, y) 
    var result = x + y
    return result
نهاية

export function multiply(x, y) 
    var result = x * y
    return result
نهاية

export var PI = 3.14159
)");
    
    // إعداد مسارات البحث / Setup search paths
    auto& pathManager = SearchPathManager::getInstance();
    pathManager.clear();
    pathManager.addPath("test_integration", 100);
    
    // Step 1: تحميل الوحدة / Load module
    auto& loader = ModuleLoader::getInstance();
    loader.clear();
    
    Module* calc = loader.loadModule("calculator");
    ASSERT_NE(calc, nullptr) << "Failed to load calculator module";
    
    std::cout << "✅ Module loaded successfully: " << calc->getName() << std::endl;
    
    // Step 2: حل الرموز / Resolve symbols
    SymbolResolver resolver;
    resolver.analyzeModule(calc);
    auto symbols = resolver.getModuleSymbols(calc->getName());
    
    std::cout << "✅ Symbols resolved: " << symbols.size() << " symbols found" << std::endl;
    
    // يجب أن يكون لدينا 3 رموز: add, multiply, PI
    // Should have 3 symbols: add, multiply, PI
    ASSERT_GE(symbols.size(), 3) << "Expected at least 3 exported symbols";
    
    // Step 3: التحقق من الصحة / Validate module
    ModuleValidator validator;
    auto result = validator.validateModule(calc);
    
    std::cout << "✅ Module validated: " << (result.success ? "PASSED" : "FAILED") << std::endl;
    
    if (!result.success) {
        result.print();
    }
    
    ASSERT_TRUE(result.success) << "Module validation failed";
    
    // Step 4: التحقق من الصادرات / Check exports
    auto& exports = calc->getExports();
    
    std::cout << "✅ Checking exports..." << std::endl;
    
    bool hasAdd = false, hasMultiply = false, hasPI = false;
    
    for (const auto& [name, exp] : exports) {
        std::cout << "   - " << name;
        if (exp.type == ExportedSymbol::Type::FUNCTION) std::cout << " (function)";
        else if (exp.type == ExportedSymbol::Type::VARIABLE) std::cout << " (variable)";
        std::cout << std::endl;
        
        if (name == "add") hasAdd = true;
        if (name == "multiply") hasMultiply = true;
        if (name == "PI") hasPI = true;
    }
    
    ASSERT_TRUE(hasAdd) << "Missing export: add";
    ASSERT_TRUE(hasMultiply) << "Missing export: multiply";
    ASSERT_TRUE(hasPI) << "Missing export: PI";
    
    std::cout << "✅ PASSED: Complete module workflow successful" << std::endl;
    
    cleanup();
}

// =========================================================================
// Test 2: Module With Dependencies
// =========================================================================

TEST(IntegrationEndToEnd, ModuleWithDependencies) {
    std::cout << "\n=== Test 2: Module With Dependencies (وحدة مع اعتماديات) ===" << std::endl;
    
    cleanup();
    
    // وحدة أساسية / Base module
    createTestModule("math_utils.s", R"(
export function square(x) 
    return x * x
نهاية

export function cube(x) 
    return x * x * x
نهاية
)");
    
    // وحدة تعتمد على الأولى / Module depending on first
    createTestModule("advanced_math.s", R"(
import math_utils

export function sum_of_squares(a, b) 
    var sq_a = math_utils.square(a)
    var sq_b = math_utils.square(b)
    return sq_a + sq_b
نهاية

export function volume_cube(side) 
    return math_utils.cube(side)
نهاية
)");
    
    auto& pathManager = SearchPathManager::getInstance();
    pathManager.clear();
    pathManager.addPath("test_integration", 100);
    
    auto& loader = ModuleLoader::getInstance();
    loader.clear();
    
    // تحميل الوحدة المتقدمة (يجب أن تحمل math_utils تلقائياً)
    // Load advanced module (should auto-load math_utils)
    Module* advMath = loader.loadModule("advanced_math");
    ASSERT_NE(advMath, nullptr) << "Failed to load advanced_math module";
    
    std::cout << "✅ Module loaded: " << advMath->getName() << std::endl;
    
    // التحقق من الاعتماديات / Check dependencies
    auto deps = advMath->getDependencies();
    ASSERT_EQ(deps.size(), 1) << "Expected 1 dependency";
    ASSERT_EQ(deps[0], "math_utils") << "Expected dependency on math_utils";
    
    std::cout << "✅ Dependency resolved: " << deps[0] << std::endl;
    
    // التحقق من أن math_utils محملة / Verify math_utils is loaded
    Module* mathUtils = loader.getModule("math_utils");
    ASSERT_NE(mathUtils, nullptr) << "Dependency math_utils not loaded";
    
    std::cout << "✅ Dependency module also loaded: " << mathUtils->getName() << std::endl;
    
    // التحقق من الصادرات / Check exports
    auto& exports = advMath->getExports();
    ASSERT_GE(exports.size(), 2) << "Expected at least 2 exports";
    
    bool hasSumOfSquares = false, hasVolumeCube = false;
    for (const auto& [name, exp] : exports) {
        if (name == "sum_of_squares") hasSumOfSquares = true;
        if (name == "volume_cube") hasVolumeCube = true;
    }
    
    ASSERT_TRUE(hasSumOfSquares) << "Missing export: sum_of_squares";
    ASSERT_TRUE(hasVolumeCube) << "Missing export: volume_cube";
    
    std::cout << "✅ PASSED: Module with dependencies works correctly" << std::endl;
    
    cleanup();
}

// =========================================================================
// Test 3: Multi-Level Import Chain
// =========================================================================

TEST(IntegrationEndToEnd, MultiLevelImportChain) {
    std::cout << "\n=== Test 3: Multi-Level Import Chain (سلسلة استيراد متعددة المستويات) ===" << std::endl;
    
    cleanup();
    
    // المستوى 1: أدوات أساسية / Level 1: Base utilities
    createTestModule("base_utils.s", R"(
export function double(x) 
    return x * 2
نهاية

export function triple(x) 
    return x * 3
نهاية
)");
    
    // المستوى 2: رياضيات / Level 2: Math operations
    createTestModule("math_ops.s", R"(
import base_utils

export function quadruple(x) 
    return base_utils.double(base_utils.double(x))
نهاية

export function sextuple(x) 
    return base_utils.double(base_utils.triple(x))
نهاية
)");
    
    // المستوى 3: تطبيق / Level 3: Application
    createTestModule("app.s", R"(
import math_ops

function main() 
    var result1 = math_ops.quadruple(5)
    var result2 = math_ops.sextuple(3)
    return result1 + result2
نهاية
)");
    
    auto& pathManager = SearchPathManager::getInstance();
    pathManager.clear();
    pathManager.addPath("test_integration", 100);
    
    auto& loader = ModuleLoader::getInstance();
    loader.clear();
    
    // تحميل التطبيق (يجب أن يحمل السلسلة كاملة)
    // Load application (should load entire chain)
    Module* app = loader.loadModule("app");
    ASSERT_NE(app, nullptr) << "Failed to load app module";
    
    std::cout << "✅ App module loaded" << std::endl;
    
    // التحقق من سلسلة الاعتماديات / Verify dependency chain
    auto appDeps = app->getDependencies();
    ASSERT_EQ(appDeps.size(), 1) << "App should depend on math_ops";
    ASSERT_EQ(appDeps[0], "math_ops");
    
    std::cout << "✅ Level 3 -> Level 2 dependency: " << appDeps[0] << std::endl;
    
    Module* mathOps = loader.getModule("math_ops");
    ASSERT_NE(mathOps, nullptr) << "math_ops not loaded";
    
    auto mathDeps = mathOps->getDependencies();
    ASSERT_EQ(mathDeps.size(), 1) << "math_ops should depend on base_utils";
    ASSERT_EQ(mathDeps[0], "base_utils");
    
    std::cout << "✅ Level 2 -> Level 1 dependency: " << mathDeps[0] << std::endl;
    
    Module* baseUtils = loader.getModule("base_utils");
    ASSERT_NE(baseUtils, nullptr) << "base_utils not loaded";
    
    std::cout << "✅ Level 1 (base) loaded" << std::endl;
    
    // التحقق من أن السلسلة كاملة / Verify complete chain
    std::cout << "✅ Complete chain verified:" << std::endl;
    std::cout << "   app -> math_ops -> base_utils" << std::endl;
    
    std::cout << "✅ PASSED: Multi-level import chain works correctly" << std::endl;
    
    cleanup();
}

// =========================================================================
// Test 4: Symbol Resolution Across Modules
// =========================================================================

TEST(IntegrationEndToEnd, SymbolResolutionAcrossModules) {
    std::cout << "\n=== Test 4: Symbol Resolution Across Modules (حل الرموز عبر الوحدات) ===" << std::endl;
    
    cleanup();
    
    // وحدة مع عدة أنواع من الصادرات / Module with various export types
    createTestModule("library.s", R"(
export function func1() 
    return 1
نهاية

export function func2(x) 
    return x + 2
نهاية

export var constant = 42

export class MyClass 
    var value
    
    function constructor(v) 
        value = v
    نهاية
نهاية
)");
    
    auto& pathManager = SearchPathManager::getInstance();
    pathManager.clear();
    pathManager.addPath("test_integration", 100);
    
    auto& loader = ModuleLoader::getInstance();
    loader.clear();
    
    Module* lib = loader.loadModule("library");
    ASSERT_NE(lib, nullptr);
    
    // حل جميع الرموز / Resolve all symbols
    SymbolResolver resolver;
    resolver.analyzeModule(lib);
    auto symbols = resolver.getModuleSymbols(lib->getName());
    
    std::cout << "✅ Found " << symbols.size() << " symbols:" << std::endl;
    
    bool hasFunc1 = false, hasFunc2 = false, hasConstant = false, hasClass = false;
    
    for (const auto& sym : symbols) {
        std::cout << "   - " << sym.toString() << std::endl;
        
        if (sym.name == "func1") hasFunc1 = true;
        if (sym.name == "func2") hasFunc2 = true;
        if (sym.name == "constant") hasConstant = true;
        if (sym.name == "MyClass") hasClass = true;
    }
    
    ASSERT_TRUE(hasFunc1) << "Missing symbol: func1";
    ASSERT_TRUE(hasFunc2) << "Missing symbol: func2";
    ASSERT_TRUE(hasConstant) << "Missing symbol: constant";
    ASSERT_TRUE(hasClass) << "Missing symbol: MyClass";
    
    // اختبار completion suggestions
    auto suggestions = resolver.getCompletionSuggestions("func", lib);
    std::cout << "✅ Completion suggestions for 'func': " << suggestions.size() << std::endl;
    ASSERT_GE(suggestions.size(), 2) << "Should suggest func1 and func2";
    
    std::cout << "✅ PASSED: Symbol resolution across modules works" << std::endl;
    
    cleanup();
}

// =========================================================================
// Test 5: Validation Integration
// =========================================================================

TEST(IntegrationEndToEnd, ValidationIntegration) {
    std::cout << "\n=== Test 5: Validation Integration (دمج التحقق من الصحة) ===" << std::endl;
    
    cleanup();
    
    // وحدة صحيحة / Valid module
    createTestModule("valid.s", R"(
export function good_function() 
    return 123
نهاية
)");
    
    // وحدة بها مشاكل / Module with issues
    createTestModule("123invalid.s", R"(
function no_export() 
    return 456
نهاية
)");
    
    auto& pathManager = SearchPathManager::getInstance();
    pathManager.clear();
    pathManager.addPath("test_integration", 100);
    
    auto& loader = ModuleLoader::getInstance();
    loader.clear();
    
    // اختبار الوحدة الصحيحة / Test valid module
    Module* validMod = loader.loadModule("valid");
    ASSERT_NE(validMod, nullptr);
    
    ModuleValidator validator;
    auto validResult = validator.validateModule(validMod);
    
    std::cout << "✅ Valid module validation:" << std::endl;
    ASSERT_TRUE(validResult.success) << "Valid module should pass validation";
    ASSERT_EQ(validResult.diagnostics.size(), 0) << "Should have no errors";
    
    // اختبار الوحدة غير الصحيحة / Test invalid module
    Module* invalidMod = loader.loadModule("123invalid");
    
    // يجب أن تفشل في التحميل بسبب الاسم غير الصحيح
    // Should fail to load due to invalid name
    // لكن إذا نجح التحميل، يجب أن تفشل في التحقق
    // But if loaded, should fail validation
    
    if (invalidMod != nullptr) {
        auto invalidResult = validator.validateModule(invalidMod);
        std::cout << "✅ Invalid module validation:" << std::endl;
        invalidResult.print();
        
        // يجب أن يكون لديها أخطاء / Should have errors
        // Count errors
        size_t errorCount = 0;
        for (const auto& diag : invalidResult.diagnostics) {
            if (diag.severity == DiagnosticSeverity::ERROR) errorCount++;
        }
        ASSERT_GT(errorCount, 0) << "Invalid module should have errors";
    }
    
    std::cout << "✅ PASSED: Validation integration works correctly" << std::endl;
    
    cleanup();
}

// =========================================================================
// Main
// =========================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    // عرض البانر مع معلومات كاملة / Display banner with full info
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                                ║\n";
    std::cout << "║     🌐 اختبارات التكامل الشاملة لنظام الوحدات               ║\n";
    std::cout << "║     🌐 End-to-End Module System Integration Tests             ║\n";
    std::cout << "║                                                                ║\n";
    std::cout << "╠════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║  📅 التاريخ / Date: December 8, 2025                          ║\n";
    std::cout << "║  📦 الإصدار / Version: Phase 5 - Integration Testing         ║\n";
    std::cout << "║  🏗️  المرحلة / Phase: Module System Complete                  ║\n";
    std::cout << "║                                                                ║\n";
    std::cout << "║  [AR/EN] للتبديل بين اللغات اضغط Ctrl+L                      ║\n";
    std::cout << "║  [AR/EN] Press Ctrl+L to switch languages                     ║\n";
    std::cout << "║                                                                ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    
    int result = RUN_ALL_TESTS();
    
    // تنظيف نهائي / Final cleanup
    cleanup();
    
    return result;
}
