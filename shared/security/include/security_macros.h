// ============================================================================
// security_macros.h — مايكروات مساعدة للأمان
// (AR) مايكروات مختصرة لاستخدام BoundsChecker, SafeArithmetic, SafeAllocator.
//      تتبع الذاكرة يُفعّل فقط عند تعريف SAD_DEBUG.
// (EN) Convenience macros wrapping BoundsChecker / SafeArithmetic / SafeAllocator.
//      Allocation tracking is gated on SAD_DEBUG.
// ============================================================================

#ifndef SAD_SHARED_SECURITY_MACROS_H
#define SAD_SHARED_SECURITY_MACROS_H

#include "bounds_checker.h"
#include "safe_allocator.h"
#include "safe_arithmetic.h"

#include <stdexcept>

// (AR) فحص الحدود
// (EN) Bounds checks
#define SAD_CHECK_INDEX(arr, idx) \
    ::Sad::Security::BoundsChecker::assertArrayIndex((idx), (arr).size(), #arr)

#define SAD_CHECK_RANGE(arr, start, end) \
    ::Sad::Security::BoundsChecker::assertArrayRange((start), (end), (arr).size(), #arr)

#define SAD_CHECK_NOT_NULL(ptr) \
    ::Sad::Security::BoundsChecker::assertNotNull((ptr), #ptr)

// (AR) الحساب الآمن
// (EN) Safe arithmetic
#define SAD_SAFE_ADD(a, b, result)                                         \
    do                                                                     \
    {                                                                      \
        if (!::Sad::Security::SafeArithmetic::safeAdd((a), (b), (result))) \
        {                                                                  \
            throw std::overflow_error("Integer overflow in addition");     \
        }                                                                  \
    } while (0)

#define SAD_SAFE_MUL(a, b, result)                                           \
    do                                                                       \
    {                                                                        \
        if (!::Sad::Security::SafeArithmetic::safeMul((a), (b), (result)))   \
        {                                                                    \
            throw std::overflow_error("Integer overflow in multiplication"); \
        }                                                                    \
    } while (0)

// (AR) تتبع الذاكرة (وضع التصحيح فقط)
// (EN) Memory tracking (debug builds only)
#ifdef SAD_DEBUG
#define SAD_ALLOC(size) \
    ::Sad::Security::SafeAllocator::AllocationTracker::instance().trackAlloc((size), __FILE__, __LINE__)
#define SAD_FREE(ptr) \
    ::Sad::Security::SafeAllocator::AllocationTracker::instance().trackFree((ptr))
#define SAD_REPORT_LEAKS() \
    ::Sad::Security::SafeAllocator::AllocationTracker::instance().reportLeaks()
#else
#define SAD_ALLOC(size) ::Sad::Security::SafeAllocator::safeAlloc(size)
#define SAD_FREE(ptr) std::free(ptr)
#define SAD_REPORT_LEAKS() ((void)0)
#endif

#endif // SAD_SHARED_SECURITY_MACROS_H
