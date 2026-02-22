/**
 * =============================================================================
 * ملف: sad_bump_allocator.cpp
 * الوصف: تنفيذ مُخصّص Bump البسيط
 * المهمة: م-ج02 - واجهة المُخصّص العام
 * =============================================================================
 *
 * الخوارزمية:
 *   1. محاذاة المؤشر للحد المطلوب
 *   2. تقدم المؤشر بحجم التخصيص
 *   3. تحقق من عدم تجاوز النهاية
 *
 * التحرير: يتجاهل التحرير الفردي (ينقص العداد فقط).
 * sad_bump_reset() يُحرّر كل شيء دفعة واحدة.
 * =============================================================================
 */

#include "sad_bump_allocator.h"
#include "sad_core.h"

extern "C" {

// ═══════════════════════════════════════════════════════════════════════════════
//                           دوال التخصيص الداخلية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * محاذاة عنوان للأعلى
 */
static inline عنوان align_up(عنوان addr, حجم alignment) {
    return (addr + alignment - 1) & ~(alignment - 1);
}

/**
 * تخصيص Bump
 */
static void* bump_allocate(SadAllocator* self, تخطيط_ذاكرة layout) {
    حالة_bump* state = (حالة_bump*)self->user_data;
    
    // محاذاة المؤشر الحالي
    عنوان aligned = align_up((عنوان)state->مؤشر, layout.المحاذاة);
    عنوان new_ptr = aligned + layout.الحجم;
    
    // تحقق من المساحة المتبقية
    if (new_ptr > (عنوان)state->نهاية) {
        return فراغ; // نفدت الذاكرة
    }
    
    // تقدم المؤشر
    state->مؤشر = (ع8*)new_ptr;
    state->عدد_التخصيصات++;
    
    return (void*)aligned;
}

/**
 * تحرير Bump — يتجاهل التحرير الفردي
 * ينقص العداد فقط
 */
static void bump_deallocate(SadAllocator* self, void* /*ptr*/, تخطيط_ذاكرة /*layout*/) {
    حالة_bump* state = (حالة_bump*)self->user_data;
    
    if (state->عدد_التخصيصات > 0) {
        state->عدد_التخصيصات--;
    }
    
    // إذا تحررت كل التخصيصات — إعادة ضبط تلقائي
    if (state->عدد_التخصيصات == 0) {
        state->مؤشر = state->بداية;
    }
}

/**
 * إعادة تخصيص Bump
 * يُخصّص مكان جديد وينسخ البيانات (لا يُحرّر القديم)
 */
static void* bump_reallocate(SadAllocator* self, void* ptr,
                             تخطيط_ذاكرة old_layout, حجم new_size) {
    // تخصيص مكان جديد
    تخطيط_ذاكرة new_layout = تخطيط(new_size, old_layout.المحاذاة);
    void* new_ptr = bump_allocate(self, new_layout);
    
    if (!new_ptr) return فراغ;
    
    // نسخ البيانات القديمة
    حجم copy_size = old_layout.الحجم;
    if (new_size < copy_size) copy_size = new_size;
    
    sad_memcpy(new_ptr, ptr, copy_size);
    
    return new_ptr;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                           الواجهة العامة
// ═══════════════════════════════════════════════════════════════════════════════

void sad_bump_init(SadAllocator* allocator, حالة_bump* state,
                   void* memory, حجم size) {
    // تهيئة الحالة
    state->بداية = (ع8*)memory;
    state->نهاية = (ع8*)memory + size;
    state->مؤشر = (ع8*)memory;
    state->عدد_التخصيصات = 0;
    
    // ربط الدوال بالمُخصّص
    allocator->allocate = bump_allocate;
    allocator->deallocate = bump_deallocate;
    allocator->reallocate = bump_reallocate;
    allocator->user_data = state;
}

void sad_bump_reset(حالة_bump* state) {
    state->مؤشر = state->بداية;
    state->عدد_التخصيصات = 0;
}

حجم sad_bump_used(const حالة_bump* state) {
    return (حجم)(state->مؤشر - state->بداية);
}

حجم sad_bump_remaining(const حالة_bump* state) {
    return (حجم)(state->نهاية - state->مؤشر);
}

} // extern "C"
