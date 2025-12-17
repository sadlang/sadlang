/**
 * @file test_symbol_resolver.cpp
 * @brief (AR) اختبار محلل الرموز - Phase 3
 *        (EN) Symbol resolver test - Phase 3
 * 
 * @author Sad Language Development Team
 * @date December 2025
 */

#include "../../include/modules/symbol_resolver.h"
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
    std::filesystem::create_directories("test_symbols");
    std::ofstream file("test_symbols/" + fileName);
    file << content;
    file.close();
}

// =========================================================================
// Test 1: Symbol Extraction
// =========================================================================

void testSymbolExtraction() {
    std::cout << "\n=== Test 1: Symbol Extraction (استخراج الرموز) ===" << std::endl;
    
    // إنشاء وحدة بسيطة مع دوال وأصناف
    createTestModule("math_simple.s", R"(
export function add(a, b) 
    var result = a + b
    return result
نهاية

export function multiply(x, y) 
    var result = x * y
    return result
نهاية

function helper() 
    var value = 42
    return value
نهاية
)");
    
    auto& pathManager = SearchPathManager::getInstance();
    pathManager.clear();
    pathManager.addPath("test_symbols", 100);
    
    auto& loader = ModuleLoader::getInstance();
    loader.clear();
    
    Module* mathModule = loader.loadModule("math_simple");
    assert(mathModule != nullptr);
    
    SymbolResolver resolver;
    resolver.analyzeModule(mathModule);
    
    auto symbols = resolver.getModuleSymbols("math_simple");
    assert(symbols.size() == 3);  // add, multiply, helper
    
    auto exported = resolver.getExportedSymbols("math_simple");
    assert(exported.size() == 2);  // add, multiply (helper not exported)
    
    std::cout << "✅ PASSED: Symbol extraction works" << std::endl;
    std::cout << "Total symbols: " << symbols.size() << std::endl;
    std::cout << "Exported symbols: " << exported.size() << std::endl;
}

// =========================================================================
// Test 2: Symbol Resolution
// =========================================================================

void testSymbolResolution() {
    std::cout << "\n=== Test 2: Symbol Resolution (حل الرموز) ===" << std::endl;
    
    // إنشاء وحدتين: utils و main
    createTestModule("utils.s", R"(
export function square(n) 
    var result = n * n
    return result
نهاية

export function cube(n) 
    var result = n * n * n
    return result
نهاية
)");
    
    createTestModule("main_app.s", R"(
import utils

function calculate() 
    var value = 0
    return value
نهاية
)");
    
    auto& pathManager = SearchPathManager::getInstance();
    pathManager.clear();
    pathManager.addPath("test_symbols", 100);
    
    auto& loader = ModuleLoader::getInstance();
    loader.clear();
    
    SymbolResolver resolver;
    
    // تحميل وتحليل utils
    Module* utilsModule = loader.loadModule("utils");
    assert(utilsModule != nullptr);
    resolver.analyzeModule(utilsModule);
    
    // تحميل وتحليل main
    Module* mainModule = loader.loadModule("main_app");
    assert(mainModule != nullptr);
    resolver.analyzeModule(mainModule);
    resolver.resolveImports(mainModule, &loader);
    
    // البحث عن رمز في الوحدة الحالية
    auto calcSymbol = resolver.findSymbol("calculate", mainModule);
    assert(calcSymbol.has_value());
    assert(calcSymbol->name == "calculate");
    
    // البحث عن رمز مؤهل من وحدة أخرى
    auto squareSymbol = resolver.findQualifiedSymbol("utils.square");
    assert(squareSymbol.has_value());
    assert(squareSymbol->name == "square");
    assert(squareSymbol->isExported == true);
    
    std::cout << "✅ PASSED: Symbol resolution works" << std::endl;
    std::cout << "Found local symbol: " << calcSymbol->toString() << std::endl;
    std::cout << "Found qualified symbol: " << squareSymbol->toString() << std::endl;
}

// =========================================================================
// Test 3: Completion Suggestions
// =========================================================================

void testCompletionSuggestions() {
    std::cout << "\n=== Test 3: Completion Suggestions (اقتراحات الإكمال) ===" << std::endl;
    
    createTestModule("strings.s", R"(
export function startsWith(txt, pre) 
    var result = 0
    return result
نهاية

export function substring(txt, i1, i2) 
    var result = ""
    return result
نهاية

export function split(txt, del) 
    var result = []
    return result
نهاية

function internal_helper() 
    var value = 0
    return value
نهاية
)");
    
    auto& pathManager = SearchPathManager::getInstance();
    pathManager.clear();
    pathManager.addPath("test_symbols", 100);
    
    auto& loader = ModuleLoader::getInstance();
    loader.clear();
    
    SymbolResolver resolver;
    
    Module* stringsModule = loader.loadModule("strings");
    assert(stringsModule != nullptr);
    resolver.analyzeModule(stringsModule);
    
    // الحصول على اقتراحات تبدأ بـ "s"
    auto suggestions = resolver.getCompletionSuggestions("s", stringsModule);
    
    // يجب أن نجد: startsWith, substring, split
    assert(suggestions.size() >= 3);
    
    std::cout << "✅ PASSED: Completion suggestions work" << std::endl;
    std::cout << "Suggestions for prefix 's':" << std::endl;
    for (const auto& sugg : suggestions) {
        std::cout << "  - " << sugg.name << std::endl;
    }
}

// =========================================================================
// Test 4: Symbol Table Printing
// =========================================================================

void testSymbolTablePrinting() {
    std::cout << "\n=== Test 4: Symbol Table Printing (طباعة جدول الرموز) ===" << std::endl;
    
    createTestModule("geometry.s", R"(
export function area_circle(radius) 
    var area = 3.14159 * radius * radius
    return area
نهاية

export function area_rectangle(width, height) 
    var area = width * height
    return area
نهاية

export class Shape 
    var shapeName
نهاية
)");
    
    auto& pathManager = SearchPathManager::getInstance();
    pathManager.clear();
    pathManager.addPath("test_symbols", 100);
    
    auto& loader = ModuleLoader::getInstance();
    loader.clear();
    
    SymbolResolver resolver;
    
    Module* geoModule = loader.loadModule("geometry");
    assert(geoModule != nullptr);
    resolver.analyzeModule(geoModule);
    
    resolver.printSymbolTable();
    
    std::cout << "✅ PASSED: Symbol table printing works" << std::endl;
}

// =========================================================================
// Main Test Runner
// =========================================================================

int main() {
    std::cout << "\n╔════════════════════════════════════════╗\n";
    std::cout << "║  (AR) اختبار محلل الرموز              ║\n";
    std::cout << "║  (EN) Symbol Resolver Test            ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";
    
    try {
        testSymbolExtraction();
        testSymbolResolution();
        testCompletionSuggestions();
        testSymbolTablePrinting();
        
        std::cout << "\n╔════════════════════════════════════════╗\n";
        std::cout << "║  ✅ جميع الاختبارات نجحت!              ║\n";
        std::cout << "║  ✅ All Tests Passed!                  ║\n";
        std::cout << "╚════════════════════════════════════════╝\n";
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
