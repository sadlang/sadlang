// ============================================================================
// test_jit_cache.cpp - اختبارات نظام الذاكرة المؤقتة
// JIT Cache System Tests
// ============================================================================

#include "../include/jit_cache.h"
#include <iostream>
#include <cassert>
#include <chrono>
#include <thread>

using namespace Sad::JIT;

// ============================================================================
// مساعدات الاختبار / Test Helpers
// ============================================================================

int test_count = 0;      // عدد الاختبارات / Test count
int passed_count = 0;    // عدد النجاحات / Passed count
int failed_count = 0;    // عدد الإخفاقات / Failed count

#define TEST(name) \
    void test_##name(); \
    void run_test_##name() { \
        test_count++; \
        std::cout << "Running: " << #name << "... "; \
        try { \
            test_##name(); \
            passed_count++; \
            std::cout << "✅ PASSED\n"; \
        } catch (const std::exception& e) { \
            failed_count++; \
            std::cout << "❌ FAILED: " << e.what() << "\n"; \
        } \
    } \
    void test_##name()

#define ASSERT(condition) \
    if (!(condition)) { \
        throw std::runtime_error("Assertion failed: " #condition); \
    }

#define ASSERT_EQ(a, b) \
    if ((a) != (b)) { \
        throw std::runtime_error("Assertion failed: " #a " != " #b); \
    }

// ============================================================================
// الاختبارات / Tests
// ============================================================================

// اختبار الإنشاء والحجم / Test Creation and Size
TEST(cache_creation) {
    JITCache cache(10, CacheEvictionPolicy::LRU); // 10 MB
    
    ASSERT_EQ(cache.getCount(), 0);
    ASSERT_EQ(cache.getCurrentSize(), 0);
    ASSERT(!cache.isFull());
}

// اختبار الإضافة والحصول / Test Put and Get
TEST(cache_put_get) {
    JITCache cache(10, CacheEvictionPolicy::LRU);
    
    // إنشاء عنصر / Create entry
    CacheEntry entry;
    entry.function_name = "test_func";
    entry.compiled_code = reinterpret_cast<void*>(0x12345);
    entry.code_size_bytes = 1024;
    entry.optimization_level = 2;
    
    // إضافة / Put
    ASSERT(cache.put(entry));
    ASSERT_EQ(cache.getCount(), 1);
    
    // الحصول / Get
    CacheEntry* retrieved = cache.get("test_func");
    ASSERT(retrieved != nullptr);
    ASSERT_EQ(retrieved->function_name, "test_func");
    ASSERT_EQ(retrieved->code_size_bytes, 1024);
}

// اختبار الإصابة والإخفاق / Test Hit and Miss
TEST(cache_hit_miss) {
    JITCache cache(10, CacheEvictionPolicy::LRU);
    
    CacheEntry entry;
    entry.function_name = "func1";
    entry.code_size_bytes = 100;
    cache.put(entry);
    
    // إصابة / Hit
    ASSERT(cache.get("func1") != nullptr);
    
    // إخفاق / Miss
    ASSERT(cache.get("non_existent") == nullptr);
    
    // التحقق من نسبة الإصابة / Check hit rate
    double hit_rate = cache.getHitRate();
    ASSERT(hit_rate == 50.0); // 1 hit, 1 miss = 50%
}

// اختبار الإخلاء LRU / Test LRU Eviction
TEST(cache_eviction_lru) {
    JITCache cache(1, CacheEvictionPolicy::LRU); // 1 MB only
    
    // إضافة عنصرين كبيرين / Add two large entries
    CacheEntry entry1;
    entry1.function_name = "func1";
    entry1.code_size_bytes = 600 * 1024; // 600 KB
    cache.put(entry1);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    CacheEntry entry2;
    entry2.function_name = "func2";
    entry2.code_size_bytes = 600 * 1024; // 600 KB
    cache.put(entry2); // يجب أن يُخلي func1 / Should evict func1
    
    // func1 يجب أن يكون مُخلى / func1 should be evicted
    ASSERT(cache.get("func1") == nullptr);
    ASSERT(cache.get("func2") != nullptr);
}

// اختبار الإخلاء LFU / Test LFU Eviction
TEST(cache_eviction_lfu) {
    JITCache cache(1, CacheEvictionPolicy::LFU);
    
    CacheEntry entry1;
    entry1.function_name = "func1";
    entry1.code_size_bytes = 400 * 1024;
    cache.put(entry1);
    
    CacheEntry entry2;
    entry2.function_name = "func2";
    entry2.code_size_bytes = 400 * 1024;
    cache.put(entry2);
    
    // الوصول لـ func1 عدة مرات / Access func1 multiple times
    cache.get("func1");
    cache.get("func1");
    cache.get("func1");
    
    // إضافة عنصر ثالث / Add third entry
    CacheEntry entry3;
    entry3.function_name = "func3";
    entry3.code_size_bytes = 400 * 1024;
    cache.put(entry3); // يجب أن يُخلي func2 (أقل استخداماً) / Should evict func2 (least frequently used)
    
    ASSERT(cache.get("func1") != nullptr);
    ASSERT(cache.get("func2") == nullptr); // الأقل استخداماً / Least used
    ASSERT(cache.get("func3") != nullptr);
}

// اختبار المسح / Test Clear
TEST(cache_clear) {
    JITCache cache(10, CacheEvictionPolicy::LRU);
    
    // إضافة عدة عناصر / Add multiple entries
    for (int i = 0; i < 5; i++) {
        CacheEntry entry;
        entry.function_name = "func" + std::to_string(i);
        entry.code_size_bytes = 100;
        cache.put(entry);
    }
    
    ASSERT_EQ(cache.getCount(), 5);
    
    // مسح / Clear
    cache.clear();
    
    ASSERT_EQ(cache.getCount(), 0);
    ASSERT_EQ(cache.getCurrentSize(), 0);
}

// اختبار الحذف / Test Remove
TEST(cache_remove) {
    JITCache cache(10, CacheEvictionPolicy::LRU);
    
    CacheEntry entry;
    entry.function_name = "test";
    entry.code_size_bytes = 100;
    cache.put(entry);
    
    ASSERT(cache.contains("test"));
    ASSERT(cache.remove("test"));
    ASSERT(!cache.contains("test"));
    ASSERT(!cache.remove("test")); // حذف مرة أخرى يجب أن يفشل / Second remove should fail
}

// اختبار النسبة المئوية للاستخدام / Test Usage Percentage
TEST(cache_usage_percentage) {
    JITCache cache(1, CacheEvictionPolicy::LRU); // 1 MB = 1024 KB
    
    CacheEntry entry;
    entry.function_name = "test";
    entry.code_size_bytes = 512 * 1024; // 512 KB = 50%
    cache.put(entry);
    
    double usage = cache.getUsagePercentage();
    ASSERT(usage >= 49.0 && usage <= 51.0); // تقريباً 50% / Approximately 50%
}

// اختبار العناصر الساخنة / Test Hot Entries
TEST(cache_hot_entries) {
    JITCache cache(10, CacheEvictionPolicy::PRIORITY);
    
    // إضافة عناصر / Add entries
    for (int i = 0; i < 5; i++) {
        CacheEntry entry;
        entry.function_name = "func" + std::to_string(i);
        entry.code_size_bytes = 100;
        entry.is_hot = (i % 2 == 0); // العناصر الزوجية ساخنة / Even entries are hot
        cache.put(entry);
    }
    
    auto hot_entries = cache.getHotEntries();
    ASSERT_EQ(hot_entries.size(), 3); // 0, 2, 4 ساخنة / hot
}

// اختبار الأولوية / Test Priority
TEST(cache_priority_calculation) {
    CacheEntry entry;
    entry.function_name = "test";
    entry.code_size_bytes = 1024;
    entry.hit_count = 10;
    
    double priority = entry.getPriority();
    ASSERT(priority > 0.0); // يجب أن تكون الأولوية موجبة / Priority should be positive
}

// اختبار تسجيل الوصول / Test Record Access
TEST(cache_record_access) {
    CacheEntry entry;
    entry.function_name = "test";
    entry.hit_count = 0;
    
    entry.recordAccess();
    ASSERT_EQ(entry.hit_count, 1);
    
    entry.recordAccess();
    ASSERT_EQ(entry.hit_count, 2);
}

// اختبار تسجيل التنفيذ / Test Record Execution
TEST(cache_record_execution) {
    CacheEntry entry;
    entry.function_name = "test";
    entry.execution_count = 0;
    entry.total_execution_time_ms = 0.0;
    
    entry.recordExecution(10.5); // 10.5 ms
    ASSERT_EQ(entry.execution_count, 1);
    ASSERT_EQ(entry.avg_execution_time_ms, 10.5);
    
    entry.recordExecution(20.5); // 20.5 ms
    ASSERT_EQ(entry.execution_count, 2);
    ASSERT_EQ(entry.avg_execution_time_ms, 15.5); // (10.5 + 20.5) / 2
}

// اختبار JSON / Test JSON Export
TEST(cache_json_export) {
    JITCache cache(10, CacheEvictionPolicy::LRU);
    
    CacheEntry entry;
    entry.function_name = "test";
    entry.code_size_bytes = 100;
    cache.put(entry);
    
    std::string json = cache.toJSON();
    ASSERT(json.find("entries") != std::string::npos);
    ASSERT(json.find("hits") != std::string::npos);
    ASSERT(json.find("misses") != std::string::npos);
}

// اختبار إعادة تعيين الإحصائيات / Test Reset Statistics
TEST(cache_reset_statistics) {
    JITCache cache(10, CacheEvictionPolicy::LRU);
    
    CacheEntry entry;
    entry.function_name = "test";
    cache.put(entry);
    
    cache.get("test"); // إصابة / Hit
    cache.get("none");  // إخفاق / Miss
    
    ASSERT(cache.getHitRate() > 0);
    
    cache.resetStatistics();
    
    // بعد إعادة التعيين، لا توجد إصابات / After reset, no hits
    ASSERT_EQ(cache.getHitRate(), 0.0);
}

// اختبار التزامن (Thread Safety) / Test Thread Safety
TEST(cache_thread_safety) {
    JITCache cache(10, CacheEvictionPolicy::LRU);
    
    // إضافة عنصر / Add entry
    CacheEntry entry;
    entry.function_name = "shared";
    entry.code_size_bytes = 100;
    cache.put(entry);
    
    // الوصول من عدة خيوط / Access from multiple threads
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([&cache]() {
            for (int j = 0; j < 100; j++) {
                cache.get("shared");
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // يجب أن يكون العنصر موجوداً / Entry should still exist
    ASSERT(cache.get("shared") != nullptr);
}

// ============================================================================
// تشغيل جميع الاختبارات / Run All Tests
// ============================================================================

int main() {
    std::cout << "\n========================================\n";
    std::cout << " JIT Cache Tests\n";
    std::cout << " اختبارات الذاكرة المؤقتة لـ JIT\n";
    std::cout << "========================================\n\n";
    
    run_test_cache_creation();
    run_test_cache_put_get();
    run_test_cache_hit_miss();
    run_test_cache_eviction_lru();
    run_test_cache_eviction_lfu();
    run_test_cache_clear();
    run_test_cache_remove();
    run_test_cache_usage_percentage();
    run_test_cache_hot_entries();
    run_test_cache_priority_calculation();
    run_test_cache_record_access();
    run_test_cache_record_execution();
    run_test_cache_json_export();
    run_test_cache_reset_statistics();
    run_test_cache_thread_safety();
    
    std::cout << "\n========================================\n";
    std::cout << " Test Results / نتائج الاختبارات\n";
    std::cout << "========================================\n";
    std::cout << "Total Tests:   " << test_count << "\n";
    std::cout << "Passed:        " << passed_count << " ✅\n";
    std::cout << "Failed:        " << failed_count << " ❌\n";
    std::cout << "Success Rate:  " << (100.0 * passed_count / test_count) << "%\n";
    std::cout << "========================================\n\n";
    
    return failed_count == 0 ? 0 : 1;
}

// ============================================================================
// نهاية الملف / End of File
// ============================================================================
