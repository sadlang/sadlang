// بسم الله الرحمن الرحيم
/**
 * @file test_version_requirement.cpp
 * @brief Unit tests for VersionRequirement class - اختبارات وحدة لفئة VersionRequirement
 * 
 * @author Sad Language Team
 * @date January 2026
 * الحمد لله رب العالمين
 */

#include "../package.h"
#include <cassert>
#include <iostream>

using namespace sad::pkg;

// Test exact version requirement - اختبار متطلب الإصدار الدقيق
void test_exact_requirement() {
    std::cout << "Testing exact version requirement...\n";
    
    VersionRequirement req = VersionRequirement::parse("1.2.3");
    
    assert(req.satisfies(Version::parse("1.2.3")));
    assert(!req.satisfies(Version::parse("1.2.4")));
    assert(!req.satisfies(Version::parse("1.3.0")));
    assert(!req.satisfies(Version::parse("2.0.0")));
    
    std::cout << "  ✓ Exact version requirement tests passed\n";
}

// Test caret (^) requirement - اختبار متطلب الإقحام (^)
void test_caret_requirement() {
    std::cout << "Testing caret (^) version requirement...\n";
    
    // ^1.2.3 means >=1.2.3 and <2.0.0
    VersionRequirement req = VersionRequirement::parse("^1.2.3");
    
    assert(!req.satisfies(Version::parse("1.2.2"))); // Too old
    assert(req.satisfies(Version::parse("1.2.3")));  // Exact match
    assert(req.satisfies(Version::parse("1.2.4")));  // Patch update OK
    assert(req.satisfies(Version::parse("1.3.0")));  // Minor update OK
    assert(req.satisfies(Version::parse("1.99.0"))); // Still OK
    assert(!req.satisfies(Version::parse("2.0.0"))); // Major update not OK
    
    // ^0.2.3 means >=0.2.3 and <0.3.0 (special case for 0.x.y)
    VersionRequirement req2 = VersionRequirement::parse("^0.2.3");
    assert(req2.satisfies(Version::parse("0.2.3")));
    assert(req2.satisfies(Version::parse("0.2.4")));
    assert(!req2.satisfies(Version::parse("0.3.0")));
    
    std::cout << "  ✓ Caret version requirement tests passed\n";
}

// Test tilde (~) requirement - اختبار متطلب التيلدة (~)
void test_tilde_requirement() {
    std::cout << "Testing tilde (~) version requirement...\n";
    
    // ~1.2.3 means >=1.2.3 and <1.3.0
    VersionRequirement req = VersionRequirement::parse("~1.2.3");
    
    assert(!req.satisfies(Version::parse("1.2.2"))); // Too old
    assert(req.satisfies(Version::parse("1.2.3")));  // Exact match
    assert(req.satisfies(Version::parse("1.2.4")));  // Patch update OK
    assert(req.satisfies(Version::parse("1.2.99"))); // Still OK
    assert(!req.satisfies(Version::parse("1.3.0"))); // Minor update not OK
    assert(!req.satisfies(Version::parse("2.0.0"))); // Major update not OK
    
    std::cout << "  ✓ Tilde version requirement tests passed\n";
}

// Test comparison operators - اختبار عوامل المقارنة
void test_comparison_requirements() {
    std::cout << "Testing comparison version requirements...\n";
    
    // Greater than
    VersionRequirement gt = VersionRequirement::parse(">1.2.3");
    assert(!gt.satisfies(Version::parse("1.2.3")));
    assert(gt.satisfies(Version::parse("1.2.4")));
    assert(gt.satisfies(Version::parse("2.0.0")));
    
    // Greater than or equal
    VersionRequirement gte = VersionRequirement::parse(">=1.2.3");
    assert(gte.satisfies(Version::parse("1.2.3")));
    assert(gte.satisfies(Version::parse("1.2.4")));
    
    // Less than
    VersionRequirement lt = VersionRequirement::parse("<2.0.0");
    assert(lt.satisfies(Version::parse("1.9.9")));
    assert(!lt.satisfies(Version::parse("2.0.0")));
    
    // Less than or equal
    VersionRequirement lte = VersionRequirement::parse("<=2.0.0");
    assert(lte.satisfies(Version::parse("2.0.0")));
    assert(!lte.satisfies(Version::parse("2.0.1")));
    
    std::cout << "  ✓ Comparison version requirement tests passed\n";
}

// Test wildcard requirement - اختبار متطلب الحرف البدل
void test_wildcard_requirement() {
    std::cout << "Testing wildcard version requirement...\n";
    
    // * matches any version
    VersionRequirement any = VersionRequirement::parse("*");
    assert(any.satisfies(Version::parse("0.0.1")));
    assert(any.satisfies(Version::parse("1.2.3")));
    assert(any.satisfies(Version::parse("999.999.999")));
    
    // 1.x matches any 1.x.x version
    VersionRequirement v1x = VersionRequirement::parse("1.x");
    assert(v1x.satisfies(Version::parse("1.0.0")));
    assert(v1x.satisfies(Version::parse("1.9.9")));
    assert(!v1x.satisfies(Version::parse("2.0.0")));
    
    std::cout << "  ✓ Wildcard version requirement tests passed\n";
}

int main() {
    std::cout << "==============================================\n";
    std::cout << "Running VersionRequirement Unit Tests\n";
    std::cout << "==============================================\n\n";
    
    try {
        test_exact_requirement();
        test_caret_requirement();
        test_tilde_requirement();
        test_comparison_requirements();
        test_wildcard_requirement();
        
        std::cout << "\n==============================================\n";
        std::cout << "✓ All VersionRequirement tests passed!\n";
        std::cout << "==============================================\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << "\n";
        return 1;
    }
}
