/**
 * =============================================================================
 * ملف: sad_types.h
 * الوصف: تعريفات الأنواع البدائية لبيئة التشغيل بدون نظام (Freestanding)
 * المهمة: م-ج01 - بيئة تشغيل Freestanding أساسية
 * =============================================================================
 *
 * هذا الملف يُعرِّف الأنواع البدائية المستخدمة في بيئة بلا_مكتبة_قياسية.
 * لا يستورد أي شيء من libc أو libstdc++.
 *
 * الأنواع تتبع تسمية لغة ص العربية:
 *   ع8-ع64  = أعداد صحيحة بدون إشارة (unsigned)
 *   ح8-ح64  = أعداد صحيحة مع إشارة (signed)
 *   ع_م8-32 = أعداد عشرية (float/double)
 *   حجم     = حجم عنوان الذاكرة
 *   منطق    = قيمة منطقية (صحيح/خطأ)
 * =============================================================================
 */

#ifndef SAD_FREESTANDING_TYPES_H
#define SAD_FREESTANDING_TYPES_H

// ═══════════════════════════════════════════════════════════════════════════════
//                     أنواع بدائية بدون أي مكتبة قياسية
// ═══════════════════════════════════════════════════════════════════════════════

// أعداد صحيحة بدون إشارة (unsigned integers)
typedef unsigned char       ع8;    // uint8_t  — 0 إلى 255
typedef unsigned short      ع16;   // uint16_t — 0 إلى 65535
typedef unsigned int        ع32;   // uint32_t — 0 إلى 4 مليار
typedef unsigned long long  ع64;   // uint64_t — 0 إلى 18 كوينتيليون

// أعداد صحيحة مع إشارة (signed integers)
typedef signed char         ح8;    // int8_t   — -128 إلى 127
typedef signed short        ح16;   // int16_t  — -32768 إلى 32767
typedef signed int          ح32;   // int32_t
typedef signed long long    ح64;   // int64_t

// أعداد عشرية (floating point)
typedef float               ع_م32; // f32 — 32-bit IEEE 754
typedef double              ع_م64; // f64 — 64-bit IEEE 754

// حجم (size type) — يتبع حجم المعالج
#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__)
typedef unsigned long long  حجم;       // 64-bit
typedef signed long long    حجم_موقع;  // signed size (ptrdiff)
#else
typedef unsigned int        حجم;       // 32-bit
typedef signed int          حجم_موقع;  // signed size
#endif

// عنوان صحيح (uintptr_t)
typedef حجم عنوان;

// منطق (boolean)
typedef ع8 منطق;

// ثوابت منطقية
#define صحيح  ((منطق)1)
#define خطأ   ((منطق)0)

// قيمة فارغة
#define فراغ   ((void*)0)

// ═══════════════════════════════════════════════════════════════════════════════
//                            حدود الأنواع
// ═══════════════════════════════════════════════════════════════════════════════

#define ع8_أقصى    ((ع8)255)
#define ع16_أقصى   ((ع16)65535)
#define ع32_أقصى   ((ع32)4294967295U)
#define ع64_أقصى   ((ع64)18446744073709551615ULL)

#define ح8_أقصى    ((ح8)127)
#define ح8_أدنى    ((ح8)(-128))
#define ح16_أقصى   ((ح16)32767)
#define ح16_أدنى   ((ح16)(-32768))
#define ح32_أقصى   ((ح32)2147483647)
#define ح32_أدنى   ((ح32)(-2147483647 - 1))
#define ح64_أقصى   ((ح64)9223372036854775807LL)
#define ح64_أدنى   ((ح64)(-9223372036854775807LL - 1))

// ═══════════════════════════════════════════════════════════════════════════════
//                            ماكرو مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

// تحديد المعالج المستهدف
#if defined(__x86_64__) || defined(_M_X64)
    #define SAD_ARCH_X86_64 1
#elif defined(__i386__) || defined(_M_IX86)
    #define SAD_ARCH_X86    1
#elif defined(__aarch64__)
    #define SAD_ARCH_AARCH64 1
#elif defined(__arm__)
    #define SAD_ARCH_ARM    1
#elif defined(__riscv)
    #define SAD_ARCH_RISCV  1
#endif

// محاذاة الذاكرة
#if defined(__GNUC__) || defined(__clang__)
    #define SAD_ALIGNED(n)  __attribute__((aligned(n)))
    #define SAD_PACKED      __attribute__((packed))
    #define SAD_NORETURN    __attribute__((noreturn))
    #define SAD_INLINE      __attribute__((always_inline)) inline
    #define SAD_NOINLINE    __attribute__((noinline))
    #define SAD_SECTION(s)  __attribute__((section(s)))
    #define SAD_USED        __attribute__((used))
    #define SAD_WEAK        __attribute__((weak))
#elif defined(_MSC_VER)
    #define SAD_ALIGNED(n)  __declspec(align(n))
    #define SAD_PACKED
    #define SAD_NORETURN    __declspec(noreturn)
    #define SAD_INLINE      __forceinline
    #define SAD_NOINLINE    __declspec(noinline)
    #define SAD_SECTION(s)
    #define SAD_USED
    #define SAD_WEAK
#endif

// إيقاف تحسينات المترجم لدالة معينة
#if defined(__GNUC__) || defined(__clang__)
    #define SAD_NO_OPTIMIZE __attribute__((optimize("O0")))
#else
    #define SAD_NO_OPTIMIZE
#endif

#endif // SAD_FREESTANDING_TYPES_H
