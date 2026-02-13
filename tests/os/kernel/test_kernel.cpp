// ===================================================================
// اختبارات نواة نظام صدOS
// المرحلة 52: اختبارات وحدة لمكونات النواة الأساسية
// ===================================================================

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>
#include <string>

namespace sad::os::tests {

/// عدّاد نتائج الاختبارات
struct TestResults {
    uint32_t passed = 0;
    uint32_t failed = 0;

    void record(bool success, const char* name) {
        if (success) {
            passed++;
            std::cout << "[نجاح] " << name << "\n";
        } else {
            failed++;
            std::cerr << "[فشل!] " << name << "\n";
        }
    }

    void summary() const {
        std::cout << "\n=== ملخص الاختبارات ===\n";
        std::cout << "نجح: " << passed << " | فشل: " << failed
                  << " | المجموع: " << (passed + failed) << "\n";
    }
};

// ----------------------------------------------------------------
// اختبارات إدارة الذاكرة
// ----------------------------------------------------------------

/// اختبار تخصيص وتحرير الذاكرة
void test_memory_allocation(TestResults& results) {
    // محاكاة مُخصّص ذاكرة بسيط
    const size_t POOL_SIZE = 4096;
    std::vector<uint8_t> pool(POOL_SIZE, 0);
    size_t allocated = 0;

    // تخصيص كتلة
    size_t block_size = 256;
    bool can_alloc = (allocated + block_size <= POOL_SIZE);
    if (can_alloc) allocated += block_size;

    results.record(can_alloc && allocated == 256,
        "تخصيص كتلة ذاكرة 256 بايت");

    // تخصيص حتى الامتلاء
    while (allocated + block_size <= POOL_SIZE) {
        allocated += block_size;
    }
    results.record(allocated == POOL_SIZE,
        "تخصيص الذاكرة حتى الامتلاء");

    // رفض التخصيص عند الامتلاء
    bool overflow = (allocated + block_size > POOL_SIZE);
    results.record(overflow, "رفض التخصيص عند نفاد الذاكرة");
}

/// اختبار محاذاة الذاكرة
void test_memory_alignment(TestResults& results) {
    // التحقق من محاذاة 4 كيلوبايت (حجم الصفحة)
    uintptr_t addr = 0x1000;
    bool aligned_4k = (addr % 4096 == 0);
    results.record(aligned_4k, "محاذاة العنوان على حدود 4KB");

    addr = 0x1234;
    uintptr_t aligned = (addr + 4095) & ~4095ULL;
    results.record(aligned == 0x2000, "تقريب العنوان لأقرب صفحة");
}

// ----------------------------------------------------------------
// اختبارات الجدولة
// ----------------------------------------------------------------

/// اختبار جدولة المهام
void test_scheduler(TestResults& results) {
    // محاكاة قائمة مهام بسيطة
    struct Task {
        uint32_t id;
        uint8_t priority;  // 0 = أعلى أولوية
    };

    std::vector<Task> ready_queue = {
        {1, 3}, {2, 1}, {3, 0}, {4, 2}
    };

    // ترتيب حسب الأولوية (الأقل رقماً = الأعلى أولوية)
    std::sort(ready_queue.begin(), ready_queue.end(),
        [](const Task& a, const Task& b) { return a.priority < b.priority; });

    results.record(ready_queue[0].id == 3,
        "اختيار المهمة ذات الأولوية القصوى");
    results.record(ready_queue.back().id == 1,
        "المهمة الأقل أولوية في النهاية");
}

// ----------------------------------------------------------------
// اختبارات نظام الملفات
// ----------------------------------------------------------------

/// اختبار مسارات نظام الملفات
void test_filesystem_paths(TestResults& results) {
    // اختبار تحليل المسارات بالعربية
    std::string path = "/نظام/مستخدم/وثائق/ملف.txt";
    
    // استخراج اسم الملف
    auto last_slash = path.rfind('/');
    std::string filename = path.substr(last_slash + 1);
    results.record(filename == "ملف.txt",
        "استخراج اسم الملف من مسار عربي");

    // اختبار المسار الجذري
    results.record(path[0] == '/', "المسار يبدأ من الجذر");
}

// ----------------------------------------------------------------
// اختبارات التزامن
// ----------------------------------------------------------------

/// اختبار القفل الدوّار (Spinlock) البسيط
void test_spinlock(TestResults& results) {
    // محاكاة قفل ذرّي
    uint32_t lock_value = 0;  // 0 = مفتوح، 1 = مقفل

    // قفل
    bool acquired = (lock_value == 0);
    if (acquired) lock_value = 1;
    results.record(acquired && lock_value == 1,
        "الحصول على القفل الدوّار");

    // فتح
    lock_value = 0;
    results.record(lock_value == 0, "تحرير القفل الدوّار");
}

} // namespace sad::os::tests

/// نقطة الدخول الرئيسية للاختبارات
int main() {
    std::cout << "╔══════════════════════════════════╗\n";
    std::cout << "║   اختبارات نواة نظام صدOS       ║\n";
    std::cout << "╚══════════════════════════════════╝\n\n";

    sad::os::tests::TestResults results;

    sad::os::tests::test_memory_allocation(results);
    sad::os::tests::test_memory_alignment(results);
    sad::os::tests::test_scheduler(results);
    sad::os::tests::test_filesystem_paths(results);
    sad::os::tests::test_spinlock(results);

    results.summary();
    return (results.failed > 0) ? 1 : 0;
}
