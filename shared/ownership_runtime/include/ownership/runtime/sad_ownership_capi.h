// ═══════════════════════════════════════════════════════════════════════════════
// (AR) sad_ownership_capi.h — طبقة C-ABI لمتتبع الملكية الموحَّد.
//
// (AR) الهدف الجوهري: تمكين runtime sadc (المخرج التنفيذي للمترجم) من استدعاء
//      منطق التتبع نفسه دون ربط أي C++ symbol أو نوع. هذا يفك قفل تفعيل --prod
//      على البرامج المُترجَمة في مرحلة C.
//
// (AR) كل الدوال thread-safe عبر القفل الداخلي في C++ implementation.
//
// (EN) C-ABI surface for the runtime ownership tracker. Enables sadc-compiled
//      programs to call the same tracker without linking any C++ symbols.
//      All functions are thread-safe via the underlying C++ mutex.
//
// @phase Memory Layer Unification — Phase B / DEF-001
// ═══════════════════════════════════════════════════════════════════════════════

#ifndef SAD_OWNERSHIP_CAPI_H
#define SAD_OWNERSHIP_CAPI_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// ─────────────────────────────────────────────────────────────────────────────
// (AR) تسجيل تخصيص جديد. allocator يمكن أن يكون NULL (يُعامَل كنص فارغ).
// (EN) Register a new allocation. `allocator` may be NULL.
// ─────────────────────────────────────────────────────────────────────────────
void sad_ownership_register(void* ptr, size_t size, const char* allocator);

// ─────────────────────────────────────────────────────────────────────────────
// (AR) تسجيل تحرير. تُرجع 1 إذا نجح، 0 عند تحرير مزدوج أو مؤشر غير مسجَّل.
// (EN) Register a deallocation. Returns 1 on success, 0 on double-free or
//      unknown pointer.
// ─────────────────────────────────────────────────────────────────────────────
int  sad_ownership_deregister(void* ptr, const char* deallocator);

// ─────────────────────────────────────────────────────────────────────────────
// (AR) نقل ملكية مؤشر مسجَّل (يضع policy=TRANSFERRED).
// (EN) Transfer ownership of a registered pointer (sets policy=TRANSFERRED).
// ─────────────────────────────────────────────────────────────────────────────
void sad_ownership_transfer(void* ptr, const char* new_owner);

// ─────────────────────────────────────────────────────────────────────────────
// (AR) فحوص بسيطة. تُرجع 0/1 — مناسبة لاستهلاك C البحت.
// (EN) Simple checks. Return 0/1 for plain C consumers.
// ─────────────────────────────────────────────────────────────────────────────
int  sad_ownership_is_owned(void* ptr);
int  sad_ownership_is_freed(void* ptr);

// ─────────────────────────────────────────────────────────────────────────────
// (AR) طباعة تقرير التسربات إلى stdout (نفس تنسيق C++ API).
// (EN) Print the leak report to stdout (identical format to C++ API).
// ─────────────────────────────────────────────────────────────────────────────
void sad_ownership_print_leaks(void);

// ─────────────────────────────────────────────────────────────────────────────
// (AR) مسح كل التتبعات (استخدامها في إعادة تهيئة الاختبارات).
// (EN) Clear all tracking (useful in test resets).
// ─────────────────────────────────────────────────────────────────────────────
void sad_ownership_clear(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SAD_OWNERSHIP_CAPI_H
