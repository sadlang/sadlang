/**
 * =============================================================================
 * ملف: sad_allocator.cpp
 * الوصف: تنفيذ واجهة المُخصّص العام
 * المهمة: م-ج02 - واجهة المُخصّص العام
 * =============================================================================
 *
 * يوفر:
 *   - تسجيل المُخصّص العام
 *   - واجهة التخصيص المُبسّطة (sad_alloc/sad_dealloc/sad_realloc)
 *   - مُخصّص افتراضي فارغ يُذعر عند الاستدعاء
 * =============================================================================
 */

#include "sad_allocator.h"
#include "sad_panic.h"
#include "sad_core.h"

extern "C" {

// ═══════════════════════════════════════════════════════════════════════════════
//                     المُخصّص الافتراضي (يُذعر عند الاستخدام)
// ═══════════════════════════════════════════════════════════════════════════════

static void* panic_allocate(SadAllocator* /*self*/, تخطيط_ذاكرة /*layout*/) {
    ذعر("محاولة تخصيص ذاكرة بدون تسجيل مُخصّص عام! "
         "استخدم sad_set_global_allocator() أولاً");
    return فراغ; // لن يصل هنا
}

static void panic_deallocate(SadAllocator* /*self*/, void* /*ptr*/, تخطيط_ذاكرة /*layout*/) {
    ذعر("محاولة تحرير ذاكرة بدون تسجيل مُخصّص عام!");
}

static void* panic_reallocate(SadAllocator* /*self*/, void* /*ptr*/,
                              تخطيط_ذاكرة /*old_layout*/, حجم /*new_size*/) {
    ذعر("محاولة إعادة تخصيص ذاكرة بدون تسجيل مُخصّص عام!");
    return فراغ;
}

// المُخصّص الافتراضي — يُذعر عند أي استدعاء
static SadAllocator g_default_panic_allocator = {
    panic_allocate,
    panic_deallocate,
    panic_reallocate,
    فراغ
};

// المُخصّص العام الحالي — يبدأ بالافتراضي
static SadAllocator* g_global_allocator = &g_default_panic_allocator;

// ═══════════════════════════════════════════════════════════════════════════════
//                           تسجيل المُخصّص
// ═══════════════════════════════════════════════════════════════════════════════

void sad_set_global_allocator(SadAllocator* allocator) {
    if (allocator) {
        g_global_allocator = allocator;
    } else {
        g_global_allocator = &g_default_panic_allocator;
    }
}

SadAllocator* sad_get_global_allocator(void) {
    return g_global_allocator;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                           واجهة مُبسّطة
// ═══════════════════════════════════════════════════════════════════════════════

void* sad_alloc(حجم size) {
    تخطيط_ذاكرة layout = تخطيط_بسيط(size);
    void* ptr = g_global_allocator->allocate(g_global_allocator, layout);
    
    if (!ptr) {
        ذعر("فشل تخصيص الذاكرة — نفدت الذاكرة");
    }
    
    return ptr;
}

void* sad_alloc_aligned(حجم size, حجم alignment) {
    تخطيط_ذاكرة layout = تخطيط(size, alignment);
    void* ptr = g_global_allocator->allocate(g_global_allocator, layout);
    
    if (!ptr) {
        ذعر("فشل تخصيص الذاكرة المُحاذاة — نفدت الذاكرة");
    }
    
    return ptr;
}

void sad_dealloc(void* ptr, حجم size) {
    if (!ptr) return;
    تخطيط_ذاكرة layout = تخطيط_بسيط(size);
    g_global_allocator->deallocate(g_global_allocator, ptr, layout);
}

void* sad_realloc(void* ptr, حجم old_size, حجم new_size) {
    تخطيط_ذاكرة old_layout = تخطيط_بسيط(old_size);
    void* new_ptr = g_global_allocator->reallocate(
        g_global_allocator, ptr, old_layout, new_size);
    
    if (!new_ptr && new_size > 0) {
        ذعر("فشل إعادة تخصيص الذاكرة — نفدت الذاكرة");
    }
    
    return new_ptr;
}

void* sad_alloc_zeroed(حجم size) {
    void* ptr = sad_alloc(size);
    sad_memzero(ptr, size);
    return ptr;
}

} // extern "C"
