// بسم الله الرحمن الرحيم
/**
 * @file test_version.cpp
 * @brief Unit tests for Version class - اختبارات وحدة لفئة Version
 * 
 * @author Sad Language Team
 * @date January 2026
 * الحمد لله رب العالمين
 */

#include "../package.h"
#include <cassert>
#include <iostream>
#include <stdexcept>

using namespace sad::pkg;

// Test version parsing - اختبار تحليل الإصدار
void test_version_parse() {
    std::cout << "Testing Version::parse()...\n";
    
    // Basic version
    Version v1 = Version::parse("1.2.3");
    assert(v1.major == 1 && v1.minor == 2 && v1.patch == 3);
    
    // Version with prerelease
    Version v2 = Version::parse("2.0.0-alpha.1");
    assert(v2.major == 2 && v2.minor == 0 && v2.patch == 0);
    assert(v2.prerelease == "alpha.1");
    
    // Version with build metadata
    Version v3 = Version::parse("1.0.0+build.123");
    assert(v3.major == 1 && v3.build == "build.123");
    
    // Version with both
    Version v4 = Version::parse("3.1.4-beta.2+20240107");
    assert(v4.major == 3 && v4.prerelease == "beta.2" && v4.build == "20240107");
    
    // Test invalid version
    try {
        Version::parse("invalid");
        assert(false && "Should throw exception");
    } catch (const std::invalid_argument&) {
        // Expected
    }
    
    std::cout << "  ✓ Version parsing tests passed\n";
}

// Test version comparison - اختبار مقارنة الإصدارات
void test_version_comparison() {
    std::cout << "Testing Version comparison...\n";
    
    Version v1 = Version::parse("1.0.0");
    Version v2 = Version::parse("1.0.1");
    Version v3 = Version::parse("1.1.0");
    Version v4 = Version::parse("2.0.0");
    Version v5 = Version::parse("1.0.0-alpha");
    
    // Less than
    assert(v1 < v2);
    assert(v2 < v3);
    assert(v3 < v4);
    assert(v5 < v1); // Prerelease < stable
    
    // Greater than
    assert(v2 > v1);
    assert(v3 > v2);
    assert(v4 > v3);
    assert(v1 > v5);
    
    // Equal
    Version v6 = Version::parse("1.0.0");
    assert(v1 == v6);
    assert(!(v1 != v6));
    
    // Less than or equal
    assert(v1 <= v2);
    assert(v1 <= v6);
    
    // Greater than or equal
    assert(v2 >= v1);
    assert(v1 >= v6);
    
    std::cout << "  ✓ Version comparison tests passed\n";
}

// Test version to_string - اختبار تحويل الإصدار إلى نص
void test_version_to_string() {
    std::cout << "Testing Version::to_string()...\n";
    
    Version v1(1, 2, 3);
    assert(v1.to_string() == "1.2.3");
    
    Version v2(2, 0, 0, "alpha.1");
    assert(v2.to_string() == "2.0.0-alpha.1");
    
    Version v3(1, 0, 0, "", "build.123");
    assert(v3.to_string() == "1.0.0+build.123");
    
    Version v4(3, 1, 4, "beta.2", "20240107");
    assert(v4.to_string() == "3.1.4-beta.2+20240107");
    
    std::cout << "  ✓ Version to_string tests passed\n";
}

int main() {
    std::cout << "==============================================\n";
    std::cout << "Running Version Unit Tests\n";
    std::cout << "==============================================\n\n";
    
    try {
        test_version_parse();
        test_version_comparison();
        test_version_to_string();
        
        std::cout << "\n==============================================\n";
        std::cout << "✓ All Version tests passed!\n";
        std::cout << "==============================================\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << "\n";
        return 1;
    }
}
