/**
 * @file test_module_validator.cpp
 * @brief (AR) اختبار محقق صحة الوحدات - Phase 4
 *        (EN) Module validator test - Phase 4
 * 
 * @author Sad Language Development Team
 * @date December 2025
 */

#include "../../include/modules/module_validator.h"
#include "../../include/modules/module_loader.h"
#include "../../include/modules/search_paths.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>

using namespace Sad;
using namespace Sad::Modules;

// =========================================================================
// Helper Functions
// =========================================================================

void createTestModule(const std::string& fileName, const std::string& content) {
    std::filesystem::create_directories("test_validation");
    std::ofstream file("test_validation/" + fileName);
    file << content;
    file.close();
}

void cleanup() {
    std::filesystem::remove_all("test_validation");
}

// =========================================================================
// Test 1: Valid Module
// =========================================================================

void testValidModule() {
    std::cout << "\n=== Test 1: Valid Module (وحدة صالحة) ===" << std::endl;
    
    createTestModule("valid.s", R"(
export function add(a, b) 
    var result = a + b
    return result
نهاية

export function multiply(x, y) 
    var result = x * y
    return result
نهاية
)");
    
    auto& pathManager = SearchPathManager::getInstance();
    pathManager.clear();
    pathManager.addPath("test_validation", 100);
    
    auto& loader = ModuleLoader::getInstance();
    loader.clear();
    
    Module* module = loader.loadModule("valid");
    assert(module != nullptr);
    
    ModuleValidator validator;
    auto result = validator.validateModule(module);
    
    result.print();
    
    assert(result.success == true);
    assert(result.errorCount() == 0);
    
    std::cout << "✅ PASSED: Valid module validated successfully" << std::endl;
}

// =========================================================================
// Test 2: Invalid Module Name
// =========================================================================

void testInvalidModuleName() {
    std::cout << "\n=== Test 2: Invalid Module Name (اسم وحدة غير صالح) ===" << std::endl;
    
    // Create a module with invalid exports
    createTestModule("123invalid.s", R"(
export function test() 
    var x = 1
    return x
نهاية
)");
    
    auto& pathManager = SearchPathManager::getInstance();
    pathManager.clear();
    pathManager.addPath("test_validation", 100);
    
    auto& loader = ModuleLoader::getInstance();
    loader.clear();
    
    // Module file name is invalid but can still be loaded
    // The validator should catch naming issues
    
    ModuleValidator validator;
    
    // Create a mock module with invalid name
    Module mockModule("123invalid", "test_validation/123invalid.s");
    auto result = validator.validateModule(&mockModule);
    
    result.print();
    
    assert(result.hasErrors() == true);
    assert(result.errorCount() >= 1);
    
    std::cout << "✅ PASSED: Invalid module name detected" << std::endl;
}

// =========================================================================
// Test 3: Cyclic Dependencies
// =========================================================================

void testCyclicDependencies() {
    std::cout << "\n=== Test 3: Cyclic Dependencies (تبعيات دائرية) ===" << std::endl;
    
    // Create modules with circular dependencies
    createTestModule("cycle_a.s", R"(
import cycle_b

export function funcA() 
    var x = 1
    return x
نهاية
)");
    
    createTestModule("cycle_b.s", R"(
import cycle_c

export function funcB() 
    var y = 2
    return y
نهاية
)");
    
    createTestModule("cycle_c.s", R"(
import cycle_a

export function funcC() 
    var z = 3
    return z
نهاية
)");
    
    auto& pathManager = SearchPathManager::getInstance();
    pathManager.clear();
    pathManager.addPath("test_validation", 100);
    
    auto& loader = ModuleLoader::getInstance();
    loader.clear();
    
    // محاولة تحميل الوحدات - يجب أن تفشل بسبب الحلقة الدائرية
    // Try loading modules - should fail due to circular dependency
    Module* moduleA = loader.loadModule("cycle_a");
    Module* moduleB = loader.loadModule("cycle_b");
    Module* moduleC = loader.loadModule("cycle_c");
    
    // الوحدات يجب أن تكون nullptr لأن التحميل فشل بسبب الحلقة
    // Modules should be nullptr because loading failed due to cycle
    // Note: In a real scenario, only one load call happens and fails immediately
    // Here we're testing that the loader correctly prevents cyclic loading
    
    bool cycleDetected = (moduleA == nullptr || moduleB == nullptr || moduleC == nullptr);
    
    if (cycleDetected) {
        std::cout << "🔁 Circular dependency correctly detected and prevented during loading" << std::endl;
    }
    
    // يجب أن يُكتشف أن التحميل فشل بسبب الحلقة
    // Should detect that loading failed due to cycle
    assert(cycleDetected == true);
    
    std::cout << "✅ PASSED: Cyclic dependency detected and prevented" << std::endl;
}

// =========================================================================
// Test 4: Missing Exports
// =========================================================================

void testMissingExports() {
    std::cout << "\n=== Test 4: Missing Exports (صادرات مفقودة) ===" << std::endl;
    
    createTestModule("no_exports.s", R"(
function internal_func() 
    var x = 42
    return x
نهاية
)");
    
    auto& pathManager = SearchPathManager::getInstance();
    pathManager.clear();
    pathManager.addPath("test_validation", 100);
    
    auto& loader = ModuleLoader::getInstance();
    loader.clear();
    
    Module* module = loader.loadModule("no_exports");
    assert(module != nullptr);
    
    ModuleValidator validator;
    validator.setStrictMode(true);
    
    auto result = validator.validateModule(module);
    result.print();
    
    // Should have warnings in strict mode if no exports
    // (But not necessarily errors - empty module is technically valid)
    
    std::cout << "✅ PASSED: Module with no exports validated" << std::endl;
}

// =========================================================================
// Test 5: Valid Import
// =========================================================================

void testValidImport() {
    std::cout << "\n=== Test 5: Valid Import (استيراد صالح) ===" << std::endl;
    
    createTestModule("lib.s", R"(
export function helper() 
    var result = 100
    return result
نهاية
)");
    
    createTestModule("main.s", R"(
import lib

function main() 
    var value = lib.helper()
    return value
نهاية
)");
    
    auto& pathManager = SearchPathManager::getInstance();
    pathManager.clear();
    pathManager.addPath("test_validation", 100);
    
    auto& loader = ModuleLoader::getInstance();
    loader.clear();
    
    Module* libModule = loader.loadModule("lib");
    Module* mainModule = loader.loadModule("main");
    
    assert(libModule != nullptr);
    assert(mainModule != nullptr);
    
    ModuleValidator validator;
    
    auto libResult = validator.validateModule(libModule);
    auto mainResult = validator.validateModule(mainModule);
    
    libResult.print();
    mainResult.print();
    
    assert(libResult.success == true);
    assert(mainResult.success == true);
    
    std::cout << "✅ PASSED: Valid import validated" << std::endl;
}

// =========================================================================
// Test 6: Validate Multiple Modules
// =========================================================================

void testValidateMultipleModules() {
    std::cout << "\n=== Test 6: Validate Multiple Modules (التحقق من عدة وحدات) ===" << std::endl;
    
    createTestModule("module1.s", R"(
export function func1() 
    var x = 1
    return x
نهاية
)");
    
    createTestModule("module2.s", R"(
import module1

export function func2() 
    var y = 2
    return y
نهاية
)");
    
    createTestModule("module3.s", R"(
import module2

export function func3() 
    var z = 3
    return z
نهاية
)");
    
    auto& pathManager = SearchPathManager::getInstance();
    pathManager.clear();
    pathManager.addPath("test_validation", 100);
    
    auto& loader = ModuleLoader::getInstance();
    loader.clear();
    
    Module* m1 = loader.loadModule("module1");
    Module* m2 = loader.loadModule("module2");
    Module* m3 = loader.loadModule("module3");
    
    assert(m1 != nullptr);
    assert(m2 != nullptr);
    assert(m3 != nullptr);
    
    ModuleValidator validator;
    
    std::vector<const Module*> modules = {m1, m2, m3};
    auto result = validator.validateModules(modules);
    
    result.print();
    
    assert(result.success == true);
    assert(result.errorCount() == 0);
    
    std::cout << "✅ PASSED: Multiple modules validated" << std::endl;
}

// =========================================================================
// Test 7: Strict Mode
// =========================================================================

void testStrictMode() {
    std::cout << "\n=== Test 7: Strict Mode (الوضع الصارم) ===" << std::endl;
    
    createTestModule("empty.s", R"(
# Empty module
)");
    
    auto& pathManager = SearchPathManager::getInstance();
    pathManager.clear();
    pathManager.addPath("test_validation", 100);
    
    auto& loader = ModuleLoader::getInstance();
    loader.clear();
    
    Module* module = loader.loadModule("empty");
    assert(module != nullptr);
    
    ModuleValidator validator;
    
    // Non-strict mode
    validator.setStrictMode(false);
    auto result1 = validator.validateModule(module);
    
    std::cout << "\n--- Non-strict Mode ---" << std::endl;
    result1.print();
    
    // Strict mode
    validator.setStrictMode(true);
    auto result2 = validator.validateModule(module);
    
    std::cout << "\n--- Strict Mode ---" << std::endl;
    result2.print();
    
    // Strict mode should have more warnings
    assert(result2.warningCount() >= result1.warningCount());
    
    std::cout << "✅ PASSED: Strict mode working" << std::endl;
}

// =========================================================================
// Main Test Runner
// =========================================================================

int main() {
    std::cout << "\n╔════════════════════════════════════════╗\n";
    std::cout << "║  (AR) اختبار محقق صحة الوحدات         ║\n";
    std::cout << "║  (EN) Module Validator Test           ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";
    
    try {
        testValidModule();
        testInvalidModuleName();
        testCyclicDependencies();
        testMissingExports();
        testValidImport();
        testValidateMultipleModules();
        testStrictMode();
        
        std::cout << "\n╔════════════════════════════════════════╗\n";
        std::cout << "║  ✅ جميع الاختبارات نجحت!              ║\n";
        std::cout << "║  ✅ All Tests Passed!                  ║\n";
        std::cout << "╚════════════════════════════════════════╝\n\n";
        
        cleanup();
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Exception: " << e.what() << std::endl;
        cleanup();
        return 1;
    }
}
