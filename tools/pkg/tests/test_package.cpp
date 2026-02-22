// بسم الله الرحمن الرحيم
/**
 * @file test_package.cpp
 * @brief Unit tests for Package class - اختبارات وحدة لفئة Package
 * 
 * @author Sad Language Team
 * @date January 2026
 * الحمد لله رب العالمين
 */

#include "../package.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace sad::pkg;

// Test package creation - اختبار إنشاء الحزمة
void test_package_creation() {
    std::cout << "Testing Package creation...\n";
    
    Package pkg;
    pkg.name = "test_package";
    pkg.version = Version::parse("1.2.3");
    pkg.description = "A test package";
    pkg.authors.push_back("Test Author <test@example.com>");
    pkg.license = "MIT";
    pkg.main_file = "src/main.ص";
    pkg.source_dirs.push_back("src");
    
    assert(pkg.name == "test_package");
    assert(pkg.version == Version::parse("1.2.3"));
    assert(pkg.description == "A test package");
    assert(pkg.authors.size() == 1);
    assert(pkg.license == "MIT");
    
    std::cout << "  ✓ Package creation tests passed\n";
}

// Test package with dependencies - اختبار الحزمة مع التبعيات
void test_package_dependencies() {
    std::cout << "Testing Package dependencies...\n";
    
    Package pkg;
    pkg.name = "test_package";
    pkg.version = Version::parse("1.0.0");
    
    // Add dependencies
    pkg.dependencies["http"] = "^1.0.0";
    pkg.dependencies["json"] = "~2.3.0";
    pkg.dev_dependencies["test_framework"] = "^0.5.0";
    
    assert(pkg.dependencies.size() == 2);
    assert(pkg.dev_dependencies.size() == 1);
    assert(pkg.dependencies["http"] == "^1.0.0");
    assert(pkg.dependencies["json"] == "~2.3.0");
    assert(pkg.dev_dependencies["test_framework"] == "^0.5.0");
    
    std::cout << "  ✓ Package dependencies tests passed\n";
}

// Test package save and load - اختبار حفظ وتحميل الحزمة
void test_package_save_load() {
    std::cout << "Testing Package save/load...\n";
    
    // Create and save package
    Package pkg1;
    pkg1.name = "test_save_load";
    pkg1.version = Version::parse("1.0.0");
    pkg1.description = "Test save/load";
    pkg1.authors.push_back("Test Author");
    pkg1.license = "MIT";
    pkg1.main_file = "src/main.ص";
    pkg1.source_dirs.push_back("src");
    pkg1.dependencies["http"] = "^1.0.0";
    
    const std::string test_file = "test_sad.toml";
    pkg1.save_to_file(test_file);
    
    // Load package
    Package pkg2 = Package::load_from_file(test_file);
    
    // Verify
    assert(pkg2.name == pkg1.name);
    assert(pkg2.version == pkg1.version);
    assert(pkg2.description == pkg1.description);
    assert(pkg2.authors.size() == pkg1.authors.size());
    assert(pkg2.license == pkg1.license);
    assert(pkg2.main_file == pkg1.main_file);
    assert(pkg2.dependencies.size() == pkg1.dependencies.size());
    
    // Cleanup
    std::filesystem::remove(test_file);
    
    std::cout << "  ✓ Package save/load tests passed\n";
}

// Test package validation - اختبار التحقق من صحة الحزمة
void test_package_validation() {
    std::cout << "Testing Package validation...\n";
    
    // Valid package
    Package valid_pkg;
    valid_pkg.name = "valid_package";
    valid_pkg.version = Version::parse("1.0.0");
    valid_pkg.main_file = "src/main.ص";
    assert(valid_pkg.is_valid());
    
    // Invalid package (no name)
    Package invalid_pkg1;
    invalid_pkg1.version = Version::parse("1.0.0");
    assert(!invalid_pkg1.is_valid());
    
    // Invalid package (no main file)
    Package invalid_pkg2;
    invalid_pkg2.name = "test";
    invalid_pkg2.version = Version::parse("1.0.0");
    invalid_pkg2.main_file = "";
    assert(!invalid_pkg2.is_valid());
    
    std::cout << "  ✓ Package validation tests passed\n";
}

// Test package scripts - اختبار نصوص الحزمة
void test_package_scripts() {
    std::cout << "Testing Package scripts...\n";
    
    Package pkg;
    pkg.name = "test_package";
    pkg.version = Version::parse("1.0.0");
    
    // Add scripts
    pkg.scripts["build"] = "sad build";
    pkg.scripts["test"] = "sad test";
    pkg.scripts["deploy"] = "sad deploy";
    
    assert(pkg.scripts.size() == 3);
    assert(pkg.scripts["build"] == "sad build");
    assert(pkg.scripts["test"] == "sad test");
    
    std::cout << "  ✓ Package scripts tests passed\n";
}

int main() {
    std::cout << "==============================================\n";
    std::cout << "Running Package Unit Tests\n";
    std::cout << "==============================================\n\n";
    
    try {
        test_package_creation();
        test_package_dependencies();
        test_package_save_load();
        test_package_validation();
        test_package_scripts();
        
        std::cout << "\n==============================================\n";
        std::cout << "✓ All Package tests passed!\n";
        std::cout << "==============================================\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << "\n";
        return 1;
    }
}
