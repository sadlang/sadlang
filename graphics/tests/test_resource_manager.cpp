// ==============================================================================
// test_resource_manager.cpp - اختبارات مدير الموارد / Resource Manager Tests
// ==============================================================================
// ملاحظة: هذه الاختبارات لا تحتاج GPU لأنها تختبر المنطق الداخلي فقط
// Note: These tests don't require GPU - they test internal logic only
// ==============================================================================
#include "catch.hpp"
#include "resources/resource_manager.h"

using namespace sad::graphics;

// ============================================================================
// اختبارات ResourceStats / ResourceStats Tests  
// ============================================================================

TEST_CASE("ResourceStats from manager are valid", "[resources][stats]") {
    auto& mgr = ResourceManager::GetInstance();
    auto stats = mgr.GetStats();
    // مدير الموارد يُرجع إحصائيات صالحة / Manager returns valid stats
    REQUIRE(stats.memoryLimit > 0);
    REQUIRE(stats.memoryUsed >= 0);
}

TEST_CASE("ResourceStats cache hit ratio - no loads", "[resources][stats]") {
    ResourceStats stats;
    stats.cacheHits = 0;
    stats.cacheMisses = 0;
    REQUIRE(stats.GetCacheHitRatio() == Approx(0.0f));
}

TEST_CASE("ResourceStats cache hit ratio - all hits", "[resources][stats]") {
    ResourceStats stats;
    stats.cacheHits = 100;
    stats.cacheMisses = 0;
    REQUIRE(stats.GetCacheHitRatio() == Approx(1.0f));
}

TEST_CASE("ResourceStats cache hit ratio - mixed", "[resources][stats]") {
    ResourceStats stats;
    stats.cacheHits = 3;
    stats.cacheMisses = 1;
    REQUIRE(stats.GetCacheHitRatio() == Approx(0.75f));
}

// ============================================================================
// اختبارات ResourceManager singleton / Singleton Tests
// ============================================================================

TEST_CASE("ResourceManager singleton returns same instance", "[resources][singleton]") {
    auto& mgr1 = ResourceManager::GetInstance();
    auto& mgr2 = ResourceManager::GetInstance();
    REQUIRE(&mgr1 == &mgr2);
}

// ============================================================================
// اختبارات مسارات البحث / Search Path Tests
// ============================================================================

TEST_CASE("ResourceManager add and get search paths", "[resources][paths]") {
    auto& mgr = ResourceManager::GetInstance();
    // مسح المسارات الحالية / Clear current paths
    // نضيف مساراً اختبارياً / Add test path
    mgr.AddSearchPath("test_assets");
    auto paths = mgr.GetSearchPaths();
    bool found = false;
    for (const auto& p : paths) {
        if (p == "test_assets") {
            found = true;
            break;
        }
    }
    REQUIRE(found);
}

// ============================================================================
// اختبارات حد الذاكرة / Memory Limit Tests
// ============================================================================

TEST_CASE("ResourceManager set memory limit", "[resources][memory]") {
    auto& mgr = ResourceManager::GetInstance();
    size_t limit = 1024ULL * 1024 * 64; // 64 MB
    mgr.SetMemoryLimit(limit);
    auto stats = mgr.GetStats();
    REQUIRE(stats.memoryLimit == limit);
}

// ============================================================================
// اختبارات تحميل الموارد غير الموجودة / Missing Resource Tests
// ============================================================================

TEST_CASE("ResourceManager LoadTexture with nonexistent file returns nullptr", "[resources][load]") {
    auto& mgr = ResourceManager::GetInstance();
    auto tex = mgr.LoadTexture("nonexistent_file_12345.png");
    REQUIRE(tex == nullptr);
}

TEST_CASE("ResourceManager LoadFont with nonexistent file returns nullptr", "[resources][load]") {
    auto& mgr = ResourceManager::GetInstance();
    auto font = mgr.LoadFont("nonexistent_font_12345.ttf", 16);
    REQUIRE(font == nullptr);
}
