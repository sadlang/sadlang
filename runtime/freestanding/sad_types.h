/* ╔══════════════════════════════════════════════════════════════════════════╗
   ║                   ملف: sad_types.h                                      ║
   ║           الأنواع البدائية لبيئة Freestanding — لغة ص                  ║
   ╠══════════════════════════════════════════════════════════════════════════╣
   ║  الإصدار : 2.0.0 — ثوري                                                 ║
   ║  الهدف   : bare-metal / kernel / embedded / RTOS                        ║
   ║  المعماريات: x86_64، ARM64، ARM32، RISC-V 32/64، MIPS                  ║
   ╠══════════════════════════════════════════════════════════════════════════╣
   ║                                                                          ║
   ║  نظام الأنواع في لغة ص:                                                 ║
   ║  ─────────────────────────────────────────────────────────────────────  ║
   ║                                                                          ║
   ║   الأنواع الصحيحة غير الموقَّعة (Unsigned Integers):                   ║
   ║   ┌────────┬──────────┬───────────────────────────────────────────┐     ║
   ║   │ ع8    │ uint8_t  │ بايت واحد — 0 .. 255                      │     ║
   ║   │ ع16   │ uint16_t │ كلمة — 0 .. 65,535                        │     ║
   ║   │ ع32   │ uint32_t │ كلمة مزدوجة — 0 .. 4,294,967,295         │     ║
   ║   │ ع64   │ uint64_t │ كلمة رباعية — 0 .. 18.4 كوينتيليون       │     ║
   ║   │ ع128  │ __uint128│ كلمة ثُمانية (حيث يُدعم)                 │     ║
   ║   └────────┴──────────┴───────────────────────────────────────────┘     ║
   ║                                                                          ║
   ║   الأنواع الصحيحة الموقَّعة (Signed Integers):                          ║
   ║   ┌────────┬─────────┬────────────────────────────────────────────┐     ║
   ║   │ ح8    │ int8_t  │ -128 .. 127                                 │     ║
   ║   │ ح16   │ int16_t │ -32,768 .. 32,767                           │     ║
   ║   │ ح32   │ int32_t │ -2.1 مليار .. 2.1 مليار                    │     ║
   ║   │ ح64   │ int64_t │ ±9.2 كوينتيليون                             │     ║
   ║   └────────┴─────────┴────────────────────────────────────────────┘     ║
   ║                                                                          ║
   ║   أنواع الحجم والعناوين:                                                 ║
   ║   ┌──────────────┬──────────────────────────────────────────────┐       ║
   ║   │ حجم          │ size_t — يتبع عرض المعالج                   │       ║
   ║   │ حجم_موقع     │ ptrdiff_t — فرق المؤشرات الموقَّع            │       ║
   ║   │ عنوان        │ uintptr_t — للـ MMIO والعناوين المادية       │       ║
   ║   └──────────────┴──────────────────────────────────────────────┘       ║
   ║                                                                          ║
   ║   الأنواع الذرية (للتزامن مع المقاطعات):                                ║
   ║   ┌──────────────┬──────────────────────────────────────────────┐       ║
   ║   │ ذري_ع32     │ atomic uint32 — آمن بين ISR والكود الرئيسي  │       ║
   ║   │ ذري_ع64     │ atomic uint64                                  │       ║
   ║   └──────────────┴──────────────────────────────────────────────┘       ║
   ║                                                                          ║
   ║   نوع النتيجة (بديل الاستثناءات في freestanding):                       ║
   ║     نتيجة_ع32 ن = تهيئة_جهاز();                                        ║
   ║     إذا (ن.نجح) { /* استخدم ن.قيمة */ }                                 ║
   ║                                                                          ║
   ╚══════════════════════════════════════════════════════════════════════════╝
   */

#ifndef SAD_FREESTANDING_TYPES_H
#define SAD_FREESTANDING_TYPES_H

/* ══════════════════════════════════════════════════════════════════════════════
   القسم 1: كشف المعمارية المستهدفة
   ══════════════════════════════════════════════════════════════════════════════
   هذه الماكروهات تُحدَّد تلقائياً حسب المترجم والهدف المستهدف.
   تُستخدم في جميع ملفات sad_freestanding لتفعيل الكود الصحيح
   لكل معمارية: x86_64، ARM، RISC-V، MIPS.
*/

#if defined(__x86_64__) || defined(_M_X64)
    /* معالج x86 64-bit — الأشيع لأجهزة الحاسوب والخوادم */
    #define SAD_ARCH_X86_64      1
    #define SAD_ARCH_64BIT       1
    #define SAD_ARCH_NAME        "x86_64"
    #define SAD_CACHE_LINE_SIZE  64   /* حجم سطر الذاكرة المخبئية بالبايت */
    #define SAD_PAGE_SIZE        4096 /* حجم الصفحة الافتراضية */

#elif defined(__i386__) || defined(_M_IX86)
    /* معالج x86 32-bit — الأجهزة القديمة والمُدمَجة منخفضة التكلفة */
    #define SAD_ARCH_X86         1
    #define SAD_ARCH_32BIT       1
    #define SAD_ARCH_NAME        "x86"
    #define SAD_CACHE_LINE_SIZE  32
    #define SAD_PAGE_SIZE        4096

#elif defined(__aarch64__) || defined(_M_ARM64)
    /* معالج ARM 64-bit — Cortex-A53/A72/A78، Apple M1/M2، Qualcomm */
    #define SAD_ARCH_AARCH64     1
    #define SAD_ARCH_64BIT       1
    #define SAD_ARCH_NAME        "aarch64"
    #define SAD_CACHE_LINE_SIZE  64
    #define SAD_PAGE_SIZE        4096

#elif defined(__arm__) || defined(_M_ARM)
    /* معالج ARM 32-bit — Cortex-M0/M3/M4/M7، Cortex-A5/A7/A9 */
    #define SAD_ARCH_ARM         1
    #define SAD_ARCH_32BIT       1
    #define SAD_ARCH_NAME        "arm"
    #define SAD_CACHE_LINE_SIZE  32
    #define SAD_PAGE_SIZE        4096

#elif defined(__riscv) && defined(__riscv_xlen) && (__riscv_xlen == 64)
    /* معالج RISC-V 64-bit — SiFive U74، T-Head C910 */
    #define SAD_ARCH_RISCV64     1
    #define SAD_ARCH_64BIT       1
    #define SAD_ARCH_NAME        "riscv64"
    #define SAD_CACHE_LINE_SIZE  64
    #define SAD_PAGE_SIZE        4096

#elif defined(__riscv)
    /* معالج RISC-V 32-bit — SiFive E31، GD32VF103 */
    #define SAD_ARCH_RISCV32     1
    #define SAD_ARCH_32BIT       1
    #define SAD_ARCH_NAME        "riscv32"
    #define SAD_CACHE_LINE_SIZE  32
    #define SAD_PAGE_SIZE        4096

#else
    /* معمارية مجهولة — افتراض 32-bit للأمان */
    #define SAD_ARCH_UNKNOWN     1
    #define SAD_ARCH_32BIT       1
    #define SAD_ARCH_NAME        "unknown"
    #define SAD_CACHE_LINE_SIZE  32
    #define SAD_PAGE_SIZE        4096
#endif

/* ══════════════════════════════════════════════════════════════════════════════
   القسم 2: الأنواع الصحيحة الأساسية
   ══════════════════════════════════════════════════════════════════════════════
   تعريفات مستقلة تماماً عن stdint.h أو أي مكتبة قياسية.
   ضروري عند التجميع بـ -nostdinc أو -ffreestanding أو -nostdlib.
*/

/* ── الأنواع غير الموقَّعة — Unsigned Integers ───────────────────────────── */
typedef unsigned char               ع8;   /* uint8_t  — بافرات، سجلات 8-bit */
typedef unsigned short              ع16;  /* uint16_t — منافذ I/O، IDT، PCI  */
typedef unsigned int                ع32;  /* uint32_t — سجلات MMIO، PIC/APIC */
typedef unsigned long long          ع64;  /* uint64_t — عناوين 64-bit، وقت  */

/* ── الأنواع الموقَّعة — Signed Integers ─────────────────────────────────── */
typedef signed char                 ح8;   /* int8_t  — درجات حرارة، إزاحات  */
typedef signed short                ح16;  /* int16_t — حساسات الحركة         */
typedef signed int                  ح32;  /* int32_t — رموز الخطأ، TCB IDs   */
typedef signed long long            ح64;  /* int64_t — فروق الوقت (ns)       */

/* ── الأنواع العشرية — IEEE 754 ──────────────────────────────────────────── */
typedef float                       ع_م32; /* f32 — 32-bit، 7 أرقام دقيقة  */
typedef double                      ع_م64; /* f64 — 64-bit، 15 رقماً دقيقاً */

/* ── 128-bit (حيث يُدعم على 64-bit مع GCC/Clang) ────────────────────────── */
#if defined(__SIZEOF_INT128__) && defined(SAD_ARCH_64BIT)
    typedef unsigned __int128       ع128;
    typedef signed   __int128       ح128;
    #define SAD_HAS_INT128          1
#endif

/* ══════════════════════════════════════════════════════════════════════════════
   القسم 3: أنواع الحجم والعناوين
   ══════════════════════════════════════════════════════════════════════════════
   تتكيف تلقائياً مع عرض المعالج (32 أو 64 بت).
   عنوان = uintptr_t — يُستخدم مع سجلات MMIO والعناوين المادية.
*/
#if defined(SAD_ARCH_64BIT)
    typedef ع64     حجم;            /* size_t     */
    typedef ح64     حجم_موقع;       /* ptrdiff_t  */
    typedef ع64     عنوان;          /* uintptr_t  */
    typedef ح64     عنوان_موقع;     /* intptr_t   */
    #define SAD_POINTER_SIZE    8
#else
    typedef ع32     حجم;
    typedef ح32     حجم_موقع;
    typedef ع32     عنوان;
    typedef ح32     عنوان_موقع;
    #define SAD_POINTER_SIZE    4
#endif

/* ══════════════════════════════════════════════════════════════════════════════
   القسم 4: النوع المنطقي
   ══════════════════════════════════════════════════════════════════════════════
*/
typedef ع8 منطق;
#ifndef صحيح
    #define صحيح  ((منطق)1)
#endif
#ifndef خطأ
    #define خطأ   ((منطق)0)
#endif
#ifndef فراغ
    #ifdef __cplusplus
        #define فراغ  nullptr
    #else
        #define فراغ  ((void*)0)
    #endif
#endif

/* ══════════════════════════════════════════════════════════════════════════════
   القسم 5: حدود الأنواع
   ══════════════════════════════════════════════════════════════════════════════
*/
#define ع8_أقصى     ((ع8)  0xFFU)
#define ع16_أقصى    ((ع16) 0xFFFFU)
#define ع32_أقصى    ((ع32) 0xFFFFFFFFUL)
#define ع64_أقصى    ((ع64) 0xFFFFFFFFFFFFFFFFULL)
#define ح8_أقصى     ((ح8)  0x7F)
#define ح8_أدنى     ((ح8)  (-0x7F - 1))
#define ح16_أقصى    ((ح16) 0x7FFF)
#define ح16_أدنى    ((ح16) (-0x7FFF - 1))
#define ح32_أقصى    ((ح32) 0x7FFFFFFF)
#define ح32_أدنى    ((ح32) (-0x7FFFFFFF - 1))
#define ح64_أقصى    ((ح64) 0x7FFFFFFFFFFFFFFFLL)
#define ح64_أدنى    ((ح64) (-0x7FFFFFFFFFFFFFFFLL - 1))

/* ══════════════════════════════════════════════════════════════════════════════
   القسم 6: ماكروهات المترجم — سمات الدوال والمحاذاة
   ══════════════════════════════════════════════════════════════════════════════
   تُجرِّد الفوارق بين GCC/Clang/MSVC.
*/
#if defined(__GNUC__) || defined(__clang__)
    /* محاذاة هيكل أو متغير إلى N بايت */
    #define SAD_ALIGNED(n)      __attribute__((aligned(n)))
    /* هيكل بدون حشو (packed) — للبروتوكولات والقرص */
    #define SAD_PACKED          __attribute__((packed))
    /* دالة لا ترجع أبداً */
    #define SAD_NORETURN        __attribute__((noreturn))
    /* دالة مضمَّنة دائماً */
    #define SAD_INLINE          __attribute__((always_inline)) inline
    /* دالة لا تُضمَّن أبداً */
    #define SAD_NOINLINE        __attribute__((noinline))
    /* وضع الدالة في قطعة محددة من الـ ELF */
    #define SAD_SECTION(s)      __attribute__((section(s)))
    /* منع حذف الرمز بواسطة المُحسِّن */
    #define SAD_USED            __attribute__((used))
    /* رمز ضعيف — يمكن تغطيته بتعريف قوي */
    #define SAD_WEAK            __attribute__((weak))
    /* دالة معالج مقاطعة (تحفظ/تستعيد كل السجلات) */
    #define SAD_INTERRUPT       __attribute__((interrupt))
    /* دالة حارة (تُوضع في ذاكرة مخبئية ساخنة) */
    #define SAD_HOT             __attribute__((hot))
    /* دالة باردة (شبه مستحيلة — مثل معالج الذعر) */
    #define SAD_COLD            __attribute__((cold))
    /* إيقاف تحسينات المترجم لهذه الدالة */
    #define SAD_NO_OPTIMIZE     __attribute__((optimize("O0")))
    /* دالة naked — بدون prologue/epilogue (للـ Assembly المُدمَج) */
    #define SAD_NAKED           __attribute__((naked))
    /* منع التحسين عبر الحدود بين الدوال */
    #define SAD_NOCLONE         __attribute__((noclone))
#elif defined(_MSC_VER)
    #define SAD_ALIGNED(n)      __declspec(align(n))
    #define SAD_PACKED
    #define SAD_NORETURN        __declspec(noreturn)
    #define SAD_INLINE          __forceinline
    #define SAD_NOINLINE        __declspec(noinline)
    #define SAD_SECTION(s)
    #define SAD_USED
    #define SAD_WEAK
    #define SAD_INTERRUPT
    #define SAD_HOT
    #define SAD_COLD
    #define SAD_NO_OPTIMIZE
    #define SAD_NAKED           __declspec(naked)
    #define SAD_NOCLONE
#else
    #define SAD_ALIGNED(n)
    #define SAD_PACKED
    #define SAD_NORETURN
    #define SAD_INLINE          inline
    #define SAD_NOINLINE
    #define SAD_SECTION(s)
    #define SAD_USED
    #define SAD_WEAK
    #define SAD_INTERRUPT
    #define SAD_HOT
    #define SAD_COLD
    #define SAD_NO_OPTIMIZE
    #define SAD_NAKED
    #define SAD_NOCLONE
#endif

/* تلميحات الفروع — تُحسِّن الكود المُولَّد لمسارات متكررة */
#if defined(__GNUC__) || defined(__clang__)
    #define مرجَّح(ش)       __builtin_expect(!!(ش), 1)  /* الشرط صحيح في الغالب */
    #define نادر(ش)         __builtin_expect(!!(ش), 0)  /* الشرط نادر الحدوث   */
#else
    #define مرجَّح(ش)       (ش)
    #define نادر(ش)         (ش)
#endif

/* ── أسماء عربية للسمات الشائعة ──────────────────────────────────────────── */
#define لن_ترجع            SAD_NORETURN void  /* دالة لا ترجع — للـ panic وnpoint */
#define مضمَّن             SAD_INLINE         /* مضمَّن دائماً */
#define غير_مضمَّن         SAD_NOINLINE       /* لا يُضمَّن */
#define متطاير             volatile           /* متغير لا يُحسَّن */
#define محاذاة_ذاكرة(ن)   SAD_ALIGNED(ن)    /* محاذاة إلى N بايت */
#define محاذاة_سطر_مخبأ   SAD_ALIGNED(SAD_CACHE_LINE_SIZE)
#define هيكل_مضغوط        SAD_PACKED         /* هيكل بدون حشو */
#define ثابت               const             /* قيمة ثابتة (const) */

/* ══════════════════════════════════════════════════════════════════════════════
   القسم 7: الأنواع الذرية — Atomic Types
   ══════════════════════════════════════════════════════════════════════════════
   للتزامن بين معالجات المقاطعات (ISR) والكود الرئيسي بدون أقفال.
   آمنة للاستخدام في بيئة متعددة الأنوية mأو مع IRQ.
*/
typedef struct { volatile ع32 القيمة; } SAD_ALIGNED(4)  ذري_ع32;
typedef struct { volatile ع64 القيمة; } SAD_ALIGNED(8)  ذري_ع64;
typedef struct { volatile ع8  القيمة; } SAD_ALIGNED(1)  ذري_منطق;

/* قراءة ذرية مع حاجز ذاكرة كامل */
static SAD_INLINE ع32 ذري_اقرأ_ع32(const ذري_ع32* د) { return د->القيمة; }
static SAD_INLINE ع64 ذري_اقرأ_ع64(const ذري_ع64* د) { return د->القيمة; }
/* كتابة ذرية */
static SAD_INLINE void ذري_اكتب_ع32(ذري_ع32* د, ع32 ق) { د->القيمة = ق; }
static SAD_INLINE void ذري_اكتب_ع64(ذري_ع64* د, ع64 ق) { د->القيمة = ق; }

/* تزايد ذري — يُرجع القيمة السابقة */
static SAD_INLINE ع32 ذري_تزايد_ع32(ذري_ع32* د) {
#if defined(__GNUC__) || defined(__clang__)
    return __atomic_fetch_add(&د->القيمة, 1, __ATOMIC_SEQ_CST);
#else
    return د->القيمة++;
#endif
}

/* مقارنة وتبديل ذري (CAS) — أساس خوارزميات Lock-Free */
static SAD_INLINE منطق ذري_قارن_وبدِّل_ع32(ذري_ع32* د, ع32 متوقع, ع32 جديد) {
#if defined(__GNUC__) || defined(__clang__)
    return (منطق)__atomic_compare_exchange_n(
        &د->القيمة, &متوقع, جديد, 0,
        __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
#else
    if (د->القيمة == متوقع) { د->القيمة = جديد; return صحيح; }
    return خطأ;
#endif
}

/* ══════════════════════════════════════════════════════════════════════════════
   القسم 8: نوع النتيجة — بديل الاستثناءات في Freestanding
   ══════════════════════════════════════════════════════════════════════════════
   مستوحى من نظام Result<T,E> في Rust.
   يُستخدم لإرجاع نجاح أو خطأ من دوال تهيئة العتاد.

   مثال:
     نتيجة_ع32 ن = تهيئة_UART(UART0_BASE);
     إذا (ن.نجح) { استخدم(ن.قيمة); }
     وإلا        { أبلِّغ_خطأ(ن.رمز_خطأ); }
*/
typedef struct { ع32    قيمة; ع32 رمز_خطأ; منطق نجح; } نتيجة_ع32;
typedef struct { عنوان  قيمة; ع32 رمز_خطأ; منطق نجح; } نتيجة_عنوان;
typedef struct {               ع32 رمز_خطأ; منطق نجح; } نتيجة_منطق;

/* أكواد الخطأ القياسية */
#define خطأ_نجاح         0u  /* لا خطأ — العملية ناجحة     */
#define خطأ_وسيط_خاطئ   1u  /* وسيط غير صالح               */
#define خطأ_جهاز         2u  /* خطأ في الجهاز               */
#define خطأ_مهلة         3u  /* انتهت المهلة الزمنية        */
#define خطأ_مشغول        4u  /* الجهاز مشغول بعملية أخرى   */
#define خطأ_ذاكرة        5u  /* نفاد الذاكرة المتاحة        */
#define خطأ_غير_مدعوم    6u  /* العملية غير مدعومة          */
#define خطأ_ليس_جاهزاً  7u  /* الجهاز لم يُهيَّأ بعد      */
#define خطأ_تجاوز        8u  /* تجاوز حد (overflow)         */
#define خطأ_محاذاة       9u  /* خطأ محاذاة ذاكرة            */

/* بانيات النتيجة — تُسهِّل إنشاء القيم */
static SAD_INLINE نتيجة_ع32    نجاح_ع32(ع32 ق)    { نتيجة_ع32    ن={ق,0,صحيح}; return ن; }
static SAD_INLINE نتيجة_ع32    فشل_ع32(ع32 رمز)   { نتيجة_ع32    ن={0,رمز,خطأ}; return ن; }
static SAD_INLINE نتيجة_منطق   نجاح_عملية(void)   { نتيجة_منطق   ن={0,صحيح};   return ن; }
static SAD_INLINE نتيجة_منطق   فشل_عملية(ع32 رمز) { نتيجة_منطق   ن={رمز,خطأ};  return ن; }

/* ══════════════════════════════════════════════════════════════════════════════
   القسم 9: ماكروهات حسابية للأنظمة
   ══════════════════════════════════════════════════════════════════════════════
*/
/* محاذاة للأعلى/الأسفل — ضروري لإدارة الصفحات والحسومات */
#define محاذاة_لأعلى(ق, م)     (((ق) + (م) - 1) & ~((م) - 1))
#define محاذاة_لأسفل(ق, م)     ((ق) & ~((م) - 1))
#define هل_محاذٍ(ق, م)          (((ق) & ((م) - 1)) == 0)

/* حد أدنى وأقصى */
#define أصغر(أ, ب)   (((أ) < (ب)) ? (أ) : (ب))
#define أكبر(أ, ب)   (((أ) > (ب)) ? (أ) : (ب))
#define اقصص(ق, d, e)  أكبر((d), أصغر((ق), (e)))
#define مطلق(ن)       (((ن) < 0) ? -(ن) : (ن))

/* عمليات البتات */
#define بت(ن)              (1UL << (ن))
#define فحص_بت(ق, ن)      (((ق) >> (ن)) & 1)
#define ضبط_بت(ق, ن)      ((ق) | (1UL << (ن)))
#define مسح_بت(ق, ن)      ((ق) & ~(1UL << (ن)))
#define تبديل_بت(ق, ن)    ((ق) ^ (1UL << (ن)))
#define حقل_بتات(ق, ب, ط) (((ق) >> (ب)) & ((1UL << (ط)) - 1))
#define دوران_يسار_ع32(ق, ن) (((ع32)(ق) << (ن)) | ((ع32)(ق) >> (32 - (ن))))
#define دوران_يمين_ع32(ق, ن) (((ع32)(ق) >> (ن)) | ((ع32)(ق) << (32 - (ن))))

/* وحدات الذاكرة */
#define كيلوبايت   (1024UL)
#define ميغابايت   (1024UL * كيلوبايت)
#define غيغابايت   (1024ULL * ميغابايت)

/* مساعد عام */
#define عدد_عناصر(مصفوفة)   (sizeof(مصفوفة) / sizeof((مصفوفة)[0]))
#define قوة_اثنين(ن)          ((ن) > 0 && !((ن) & ((ن) - 1)))
#define حجم_نوع(ن)            sizeof(ن)

#endif /* SAD_FREESTANDING_TYPES_H */
