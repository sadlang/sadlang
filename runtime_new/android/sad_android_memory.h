/**
 * @file sad_android_memory.h
 * @brief (AR) نظام إدارة الذاكرة لبيئة أندرويد الأصلية — قلب Runtime لغة ص
 * @brief (EN) Native Android memory management system — Sad language Runtime core
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                    نظام إدارة الذاكرة — لغة ص على أندرويد
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * هذا الملف يُعرّف نظام إدارة الذاكرة الأصلي الذي يعمل مباشرة على أندرويد
 * بدون أي اعتماد على Java أو Kotlin أو أي طبقة وسيطة.
 *
 * ───────────────────────────────────────────────────────────────────────────
 *                         البنية المعمارية
 * ───────────────────────────────────────────────────────────────────────────
 *
 *  ┌─────────────────────────────────────────────────────────────┐
 *  │              SadMemoryManager (مدير الذاكرة الرئيسي)        │
 *  │                                                             │
 *  │  ┌──────────────┐  ┌──────────────┐  ┌──────────────────┐  │
 *  │  │ Arena        │  │ Pool         │  │ عدّ مرجعي (RC)    │  │
 *  │  │ (الأرينا)    │  │ (المجمّع)    │  │ + كشف دورات      │  │
 *  │  │              │  │              │  │                    │  │
 *  │  │ تخصيص سريع  │  │ أحجام ثابتة │  │ كائنات طويلة     │  │
 *  │  │ أحادي الاتجاه│  │ 16/32/64/   │  │ العمر (UI،       │  │
 *  │  │ تحرير جماعي │  │ 128/256 بايت│  │ اتصالات، إلخ)    │  │
 *  │  └──────────────┘  └──────────────┘  └──────────────────┘  │
 *  │                          ↓                                  │
 *  │  ┌──────────────────────────────────────────────────────┐  │
 *  │  │              bionic mmap / malloc                     │  │
 *  │  │      (نظام تشغيل أندرويد — نواة لينكس)               │  │
 *  │  └──────────────────────────────────────────────────────┘  │
 *  └─────────────────────────────────────────────────────────────┘
 *
 * ───────────────────────────────────────────────────────────────────────────
 *                         لماذا ثلاثة مُخصصين؟
 * ───────────────────────────────────────────────────────────────────────────
 *
 * ١. الأرينا (Arena): للكائنات المؤقتة قصيرة العمر — مثل السلاسل النصية
 *    الوسيطة في الحسابات. التخصيص فوري (O(1) — فقط زيادة مؤشر).
 *    التحرير جماعي عند انتهاء النطاق. مثالي لكل إطار رسم.
 *
 * ٢. المجمّع (Pool): لأحجام الذاكرة الشائعة الثابتة. يُعيد استخدام
 *    الكتل المحررة بدون استدعاء malloc مرة أخرى. يقلل تجزئة الذاكرة.
 *
 * ٣. العدّ المرجعي (RC): للكائنات طويلة العمر كعناصر الواجهة
 *    واتصالات الشبكة. يستخدم عدّاداً ذرياً (atomic) للأمان مع الخيوط.
 *    يتضمن كاشف دورات بسيط (يعمل دورياً) للحالات النادرة.
 *
 * ───────────────────────────────────────────────────────────────────────────
 *                         ملاحظات مهمة
 * ───────────────────────────────────────────────────────────────────────────
 *
 * - جميع الدوال آمنة للخيوط (thread-safe) ما لم يُذكر خلاف ذلك
 * - تتوافق مع NDK API Level 24+ (Android 7.0+)
 * - تعمل مع bionic libc (مكتبة C الخاصة بأندرويد)
 * - لا تعتمد على أي مكتبة C++ (pure C)
 * - جميع الأسماء العامة تبدأ بـ sad_ لتجنب التعارضات
 *
 * @author فريق تطوير لغة ص
 * @version 1.0.0
 * @date مارس 2026
 */

#ifndef SAD_ANDROID_MEMORY_H
#define SAD_ANDROID_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

/* ═══════════════════════════════════════════════════════════════════════════
 *                         الترويسات المطلوبة
 * ═══════════════════════════════════════════════════════════════════════════ */

#include <stddef.h>   /* size_t, NULL */
#include <stdint.h>   /* uint8_t, int64_t, uintptr_t */
#include <stdbool.h>  /* bool */

/* ═══════════════════════════════════════════════════════════════════════════
 *                         ثوابت النظام
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) حجم الأرينا الافتراضي — 1 ميجابايت
 * @brief (EN) Default arena size — 1 megabyte
 *
 * تم اختيار هذا الحجم لأنه:
 * - كبير بما يكفي لمعظم إطارات الرسم (عادة 100-500 كيلوبايت)
 * - صغير بما يكفي لعدم استهلاك ذاكرة الجهاز
 * - يتناسب مع حجم صفحات الذاكرة في ARM64 (4KB × 256)
 */
#define SAD_ARENA_DEFAULT_SIZE  (1024 * 1024)

/**
 * @brief (AR) محاذاة الذاكرة الافتراضية — 16 بايت
 * @brief (EN) Default memory alignment — 16 bytes
 *
 * ARM64 (AArch64) يتطلب محاذاة 16 بايت للأداء الأمثل.
 * هذا يضمن أيضاً توافق NEON SIMD (128-bit).
 */
#define SAD_ALIGNMENT_DEFAULT  16

/**
 * @brief (AR) أحجام المجمّعات الثابتة
 * @brief (EN) Fixed pool sizes
 *
 * هذه الأحجام تغطي أكثر من 90% من التخصيصات الشائعة في لغة ص:
 * - 16 بايت: قيم بسيطة (SadValue بدون بيانات heap)
 * - 32 بايت: SadString قصيرة (SSO) — وهي الأكثر شيوعاً
 * - 64 بايت: SadArray صغيرة (≤ 4 عناصر)
 * - 128 بايت: كائنات متوسطة
 * - 256 بايت: كائنات كبيرة وسلاسل متوسطة
 */
#define SAD_POOL_SIZE_16    16
#define SAD_POOL_SIZE_32    32
#define SAD_POOL_SIZE_64    64
#define SAD_POOL_SIZE_128   128
#define SAD_POOL_SIZE_256   256
#define SAD_POOL_COUNT      5

/**
 * @brief (AR) عدد الكتل الابتدائية لكل مجمّع
 * @brief (EN) Initial number of blocks per pool
 */
#define SAD_POOL_INITIAL_BLOCKS  256

/**
 * @brief (AR) الحد الأقصى للذاكرة الكلية — 256 ميجابايت
 * @brief (EN) Maximum total memory — 256 megabytes
 *
 * حد أمان لمنع التطبيق من استهلاك كل ذاكرة الجهاز.
 * يمكن تعديله عبر sad_memory_set_limit().
 */
#define SAD_MEMORY_MAX_TOTAL  (256 * 1024 * 1024)

/* ═══════════════════════════════════════════════════════════════════════════
 *                    ترويسة الكائن المُدار (Object Header)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * كل كائن يُدار بالعدّ المرجعي يحتوي على ترويسة في بداية ذاكرته.
 * هذه الترويسة تُخزّن:
 * - عدد المراجع الحالي (ذري — آمن للخيوط)
 * - حجم الكائن (لإحصائيات الذاكرة)
 * - نوع الكائن (لكاشف الدورات)
 * - دالة التنظيف (destructor) — تُستدعى قبل التحرير
 *
 * ┌──────────────────────────────────────────┐
 * │ SadObjectHeader (32 بايت)                 │
 * ├──────────┬───────────┬───────────────────┤
 * │ ref_count│  size     │  type   │ flags   │
 * │ (4 بايت)│ (4 بايت) │ (2 بايت)│(2 بايت) │
 * ├──────────┴───────────┴───────────────────┤
 * │ finalizer (مؤشر دالة التنظيف) (8 بايت)  │
 * ├──────────────────────────────────────────┤
 * │ gc_next (مؤشر للقائمة المتصلة) (8 بايت) │
 * ├──────────────────────────────────────────┤
 * │ reserved (محجوز للمستقبل) (4 بايت)       │
 * └──────────────────────────────────────────┘
 * │ ← هنا تبدأ بيانات الكائن الفعلية →      │
 * └──────────────────────────────────────────┘
 */

/**
 * @brief (AR) أنواع الكائنات المُدارة — يستخدمها كاشف الدورات
 * @brief (EN) Managed object types — used by cycle detector
 */
typedef enum {
    SAD_OBJ_NONE     = 0,   /* لا نوع — ذاكرة خام */
    SAD_OBJ_STRING   = 1,   /* سلسلة نصية — لا تُشكّل دورات */
    SAD_OBJ_ARRAY    = 2,   /* مصفوفة — قد تحتوي مراجع */
    SAD_OBJ_MAP      = 3,   /* خريطة — قد تحتوي مراجع */
    SAD_OBJ_OBJECT   = 4,   /* كائن مستخدم — قد يُشكّل دورات */
    SAD_OBJ_CLOSURE  = 5,   /* إغلاق (closure) — يحتوي متغيرات مأسورة */
    SAD_OBJ_SOCKET   = 6,   /* مقبس شبكة — يحتاج إغلاق خاص */
    SAD_OBJ_FILE     = 7,   /* مقبض ملف — يحتاج إغلاق خاص */
    SAD_OBJ_UI       = 8,   /* عنصر واجهة — شجرة واجهات */
} SadObjectType;

/**
 * @brief (AR) أعلام الكائن المُدار
 * @brief (EN) Managed object flags
 */
typedef enum {
    SAD_OBJ_FLAG_NONE       = 0x0000,  /* لا أعلام */
    SAD_OBJ_FLAG_PINNED     = 0x0001,  /* مُثبّت — لا يُنقل */
    SAD_OBJ_FLAG_GC_MARKED  = 0x0002,  /* مُعلّم من كاشف الدورات */
    SAD_OBJ_FLAG_WEAK       = 0x0004,  /* مرجع ضعيف */
    SAD_OBJ_FLAG_IMMORTAL   = 0x0008,  /* لا يُحرّر أبداً (ثوابت) */
    SAD_OBJ_FLAG_FINALIZING = 0x0010,  /* قيد التنظيف — يمنع الاستدعاء المتكرر */
} SadObjectFlags;

/**
 * @brief (AR) نوع دالة التنظيف — تُستدعى قبل تحرير الكائن
 * @brief (EN) Finalizer function type — called before freeing the object
 *
 * @param data مؤشر إلى بيانات الكائن (بعد الترويسة)
 * @param size حجم بيانات الكائن بالبايت
 *
 * أمثلة استخدام:
 * - إغلاق مقبس الشبكة قبل تحرير كائن الاتصال
 * - تحرير مخزن OpenGL المؤقت قبل تحرير واجهة UI
 * - تحرير ذاكرة أصلية مرتبطة بالكائن
 */
typedef void (*SadFinalizer)(void* data, size_t size);

/**
 * @brief (AR) ترويسة الكائن المُدار — تسبق بيانات كل كائن يُدار بالعدّ المرجعي
 * @brief (EN) Managed object header — precedes data of every ref-counted object
 */
typedef struct SadObjectHeader {
    _Atomic uint32_t ref_count;    /* عدد المراجع — ذري */
    uint32_t         size;         /* حجم بيانات الكائن (بدون الترويسة) */
    uint16_t         type;         /* SadObjectType — نوع الكائن */
    uint16_t         flags;        /* SadObjectFlags — أعلام الكائن */
    SadFinalizer     finalizer;    /* دالة التنظيف (قد تكون NULL) */
    struct SadObjectHeader* gc_next; /* القائمة المتصلة لكاشف الدورات */
    uint32_t         reserved;     /* محجوز للمستقبل */
} SadObjectHeader;

/* ═══════════════════════════════════════════════════════════════════════════
 *                         الأرينا (Arena Allocator)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * الأرينا هي أسرع طريقة لتخصيص الذاكرة:
 *
 * ┌───────────────────────────────────────────────────┐
 * │ ╔═══╤═══╤════════╤═══════╤══════════════════════╗ │
 * │ ║ A │ B │   C    │  D    │    (فارغ)            ║ │
 * │ ╚═══╧═══╧════════╧═══════╧══════════════════════╝ │
 * │ ↑ base                    ↑ offset     ↑ capacity │
 * └───────────────────────────────────────────────────┘
 *
 * - التخصيص: فقط offset += size (عملية واحدة!)
 * - لا يوجد free فردي — كل شيء يُحرّر معاً عند الـ reset
 * - مثالي لكل إطار رسم: خصّص كل شيء، ارسم، ثم reset
 */

/**
 * @brief (AR) الأرينا — مُخصص ذاكرة خطّي فائق السرعة
 * @brief (EN) Arena — ultra-fast linear memory allocator
 */
typedef struct SadArena {
    uint8_t* base;       /* بداية كتلة الذاكرة */
    size_t   capacity;   /* الحجم الكلي بالبايت */
    size_t   offset;     /* الموقع الحالي (= الحجم المستخدم) */
    size_t   peak;       /* أعلى استخدام تم تسجيله — لمراقبة الأداء */
    uint32_t alloc_count;/* عدد التخصيصات منذ آخر reset */
} SadArena;

/**
 * @brief (AR) إنشاء أرينا جديدة بالحجم المحدد
 * @brief (EN) Create a new arena with the specified size
 *
 * تُخصص كتلة ذاكرة واحدة متصلة من نظام التشغيل.
 * التخصيصات اللاحقة من هذه الأرينا فورية (O(1)).
 *
 * @param capacity الحجم بالبايت (يُستحسن مضاعفات 4096 — حجم صفحة الذاكرة)
 * @return مؤشر إلى الأرينا الجديدة، أو NULL إذا فشل التخصيص
 *
 * @note يُفضّل استخدام SAD_ARENA_DEFAULT_SIZE (1MB) للاستخدام العام.
 * @note لإطارات الرسم، 256KB تكفي عادةً.
 *
 * مثال:
 * @code
 * SadArena* إطار = sad_arena_new(256 * 1024);  // 256KB لإطار رسم
 * // ... تخصيصات سريعة ...
 * sad_arena_reset(إطار);  // تحرير كل شيء دفعة واحدة
 * @endcode
 */
SadArena* sad_arena_new(size_t capacity);

/**
 * @brief (AR) تخصيص ذاكرة من الأرينا (فائق السرعة)
 * @brief (EN) Allocate memory from arena (ultra-fast)
 *
 * عملية واحدة فقط: تحريك المؤشر للأمام.
 * الذاكرة المُرجَعة مُحاذاة تلقائياً لـ SAD_ALIGNMENT_DEFAULT (16 بايت).
 *
 * @param arena الأرينا المراد التخصيص منها
 * @param size الحجم المطلوب بالبايت
 * @return مؤشر للذاكرة المُخصصة، أو NULL إذا الأرينا ممتلئة
 *
 * @warning لا يمكن تحرير هذه الذاكرة فردياً! استخدم sad_arena_reset().
 * @warning ليست آمنة للخيوط — كل خيط يجب أن يملك أرينا خاصة.
 */
void* sad_arena_alloc(SadArena* arena, size_t size);

/**
 * @brief (AR) تخصيص ذاكرة من الأرينا بمحاذاة محددة
 * @brief (EN) Allocate aligned memory from arena
 *
 * @param arena الأرينا
 * @param size الحجم المطلوب
 * @param alignment المحاذاة المطلوبة (يجب أن تكون قوة 2)
 * @return مؤشر محاذى للذاكرة، أو NULL
 */
void* sad_arena_alloc_aligned(SadArena* arena, size_t size, size_t alignment);

/**
 * @brief (AR) إعادة تعيين الأرينا — تحرير كل التخصيصات دفعة واحدة
 * @brief (EN) Reset arena — free all allocations at once
 *
 * عملية O(1) — فقط إعادة المؤشر إلى البداية.
 * الذاكرة نفسها لا تُعاد لنظام التشغيل — تبقى جاهزة لإعادة الاستخدام.
 *
 * @param arena الأرينا المراد إعادة تعيينها
 */
void sad_arena_reset(SadArena* arena);

/**
 * @brief (AR) تدمير الأرينا وإعادة ذاكرتها لنظام التشغيل
 * @brief (EN) Destroy arena and return its memory to the OS
 *
 * @param arena الأرينا المراد تدميرها
 */
void sad_arena_free(SadArena* arena);

/**
 * @brief (AR) الحصول على إحصائيات الأرينا
 * @brief (EN) Get arena statistics
 */
size_t sad_arena_used(const SadArena* arena);
size_t sad_arena_remaining(const SadArena* arena);
size_t sad_arena_peak(const SadArena* arena);

/* ═══════════════════════════════════════════════════════════════════════════
 *                     المجمّع (Pool Allocator)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * مُخصص بأحجام ثابتة. يحتفظ بقائمة حرة (free list) من الكتل المتاحة.
 *
 *  مجمّع 32 بايت:
 *  ┌────┐  ┌────┐  ┌────┐  ┌────┐
 *  │مشغ│  │حر │→ │حر │→ │حر │→ NULL
 *  └────┘  └────┘  └────┘  └────┘
 *
 * - التخصيص: O(1) — أخذ أول كتلة من القائمة الحرة
 * - التحرير: O(1) — إضافة الكتلة لبداية القائمة الحرة
 * - لا تجزئة ذاكرة (fragmentation) على الإطلاق
 */

/**
 * @brief (AR) كتلة حرة في المجمّع
 * @brief (EN) Free block in pool
 */
typedef struct SadPoolBlock {
    struct SadPoolBlock* next;  /* الكتلة الحرة التالية */
} SadPoolBlock;

/**
 * @brief (AR) مجمّع لحجم ثابت
 * @brief (EN) Pool for a fixed size
 */
typedef struct SadPool {
    SadPoolBlock*  free_list;    /* قائمة الكتل الحرة */
    uint8_t*       memory;       /* الذاكرة المخصصة للمجمّع */
    size_t         block_size;   /* حجم كل كتلة */
    size_t         total_blocks; /* العدد الكلي للكتل */
    size_t         used_blocks;  /* الكتل المستخدمة حالياً */
    _Atomic uint32_t lock;       /* قفل دوراني للخيوط */
} SadPool;

/**
 * @brief (AR) إنشاء مجمّع جديد
 * @brief (EN) Create a new pool
 *
 * @param block_size حجم كل كتلة (يجب أن يكون ≥ sizeof(void*))
 * @param num_blocks عدد الكتل الابتدائية
 * @return مؤشر للمجمّع، أو NULL إذا فشل
 */
SadPool* sad_pool_new(size_t block_size, size_t num_blocks);

/**
 * @brief (AR) تخصيص كتلة من المجمّع — O(1) آمن للخيوط
 * @brief (EN) Allocate a block from pool — O(1) thread-safe
 */
void* sad_pool_alloc(SadPool* pool);

/**
 * @brief (AR) تحرير كتلة وإعادتها للمجمّع — O(1) آمن للخيوط
 * @brief (EN) Free a block back to the pool — O(1) thread-safe
 */
void sad_pool_free(SadPool* pool, void* ptr);

/**
 * @brief (AR) تدمير المجمّع
 * @brief (EN) Destroy the pool
 */
void sad_pool_destroy(SadPool* pool);

/* ═══════════════════════════════════════════════════════════════════════════
 *                  العدّ المرجعي (Reference Counting)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * نظام العدّ المرجعي هو الطريقة الرئيسية لإدارة الكائنات طويلة العمر.
 *
 *   sad_rc_alloc() → عدد المراجع = 1
 *        ↓
 *   sad_retain()   → عدد المراجع += 1
 *   sad_release()  → عدد المراجع -= 1
 *        ↓
 *   عدد المراجع == 0 → استدعاء finalizer → تحرير الذاكرة
 *
 * كل تخصيص يسبقه SadObjectHeader تلقائياً:
 *
 *   ┌────────────────────┬──────────────────────┐
 *   │ SadObjectHeader    │ بيانات الكائن        │
 *   │ (32 بايت)         │ (حسب الطلب)          │
 *   └────────────────────┴──────────────────────┘
 *   ↑ ما يُخزّن داخلياً  ↑ ما يُرجَع للمستخدم
 */

/**
 * @brief (AR) تخصيص كائن مُدار بالعدّ المرجعي (عدد المراجع = 1)
 * @brief (EN) Allocate a ref-counted managed object (ref_count = 1)
 *
 * يُخصص ذاكرة = sizeof(SadObjectHeader) + size، ويُعيد مؤشراً
 * لبيانات الكائن (بعد الترويسة).
 *
 * @param size حجم بيانات الكائن المطلوبة
 * @param type نوع الكائن (SadObjectType)
 * @param finalizer دالة التنظيف (أو NULL إذا لا يحتاج)
 * @return مؤشر لبيانات الكائن، أو NULL إذا فشل التخصيص
 *
 * مثال:
 * @code
 * // تخصيص سلسلة نصية
 * char* بيانات = (char*)sad_rc_alloc(100, SAD_OBJ_STRING, NULL);
 * strcpy(بيانات, "مرحباً");
 *
 * // تخصيص مقبس شبكة مع دالة تنظيف
 * SadSocket* مقبس = (SadSocket*)sad_rc_alloc(sizeof(SadSocket),
 *                                             SAD_OBJ_SOCKET,
 *                                             sad_socket_finalizer);
 * @endcode
 */
void* sad_rc_alloc(size_t size, SadObjectType type, SadFinalizer finalizer);

/**
 * @brief (AR) زيادة عدد المراجع بواحد (ذري — آمن للخيوط)
 * @brief (EN) Increment reference count by one (atomic — thread-safe)
 *
 * @param ptr مؤشر للكائن (يجب أن يكون مُخصصاً بـ sad_rc_alloc)
 *
 * @note عملية ذرية — يمكن استدعاؤها من أي خيط بأمان.
 */
void sad_retain(void* ptr);

/**
 * @brief (AR) إنقاص عدد المراجع بواحد — يُحرّر الكائن إذا وصل لصفر
 * @brief (EN) Decrement reference count — frees object if it reaches zero
 *
 * إذا وصل العدد لصفر:
 * ١. يُعلّم الكائن بـ SAD_OBJ_FLAG_FINALIZING (يمنع التنظيف المتكرر)
 * ٢. يستدعي دالة التنظيف (finalizer) إن وُجدت
 * ٣. يُحرّر الذاكرة
 *
 * @param ptr مؤشر للكائن
 *
 * @warning لا تستدعي هذه الدالة مرتين على نفس الكائن بعدد مراجع 0!
 */
void sad_release(void* ptr);

/**
 * @brief (AR) الحصول على عدد المراجع الحالي (للتشخيص)
 * @brief (EN) Get current reference count (for diagnostics)
 */
uint32_t sad_rc_get_count(const void* ptr);

/**
 * @brief (AR) الحصول على ترويسة الكائن من مؤشر بياناته
 * @brief (EN) Get object header from data pointer
 *
 * @param ptr مؤشر لبيانات الكائن (المُرجَع من sad_rc_alloc)
 * @return مؤشر للترويسة
 */
SadObjectHeader* sad_rc_get_header(void* ptr);

/* ═══════════════════════════════════════════════════════════════════════════
 *                      كاشف الدورات (Cycle Detector)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * مشكلة الدورات المرجعية:
 *
 *   كائن أ ──(مرجع)──→ كائن ب
 *      ↑                   │
 *      └────(مرجع)─────────┘
 *
 * في هذه الحالة، عدد المراجع لن يصل لصفر أبداً!
 *
 * الحل: كاشف دورات يعمل دورياً (كل N ثانية أو كل N تخصيص).
 * يستخدم خوارزمية Mark-Sweep مُبسّطة:
 * ١. علّم (Mark): تجاوز كل الكائنات القابلة للوصول من الجذور
 * ٢. امسح (Sweep): حرّر الكائنات غير المُعلّمة
 */

/**
 * @brief (AR) تشغيل كاشف الدورات يدوياً
 * @brief (EN) Run cycle detector manually
 *
 * @return عدد الكائنات المُحرّرة (الدورات المكتشفة)
 */
uint32_t sad_gc_collect_cycles(void);

/**
 * @brief (AR) إضافة جذر للكائنات الحية (مثل: متغير عام، مكدس)
 * @brief (EN) Add a root for live objects
 */
void sad_gc_add_root(void* ptr);

/**
 * @brief (AR) إزالة جذر
 * @brief (EN) Remove a root
 */
void sad_gc_remove_root(void* ptr);

/* ═══════════════════════════════════════════════════════════════════════════
 *                   مدير الذاكرة الموحّد (Unified Manager)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * واجهة موحّدة تختار المُخصص الأنسب تلقائياً حسب حجم التخصيص:
 *
 *   الحجم ≤ 256 بايت → المجمّع (Pool)
 *   الحجم > 256 بايت → malloc العادي + ترويسة RC
 *   إطار رسم         → الأرينا (Arena)
 */

/**
 * @brief (AR) إحصائيات الذاكرة — لمراقبة الأداء وكشف التسريبات
 * @brief (EN) Memory statistics — for performance monitoring and leak detection
 */
typedef struct SadMemoryStats {
    size_t   total_allocated;     /* إجمالي الذاكرة المُخصصة حالياً */
    size_t   total_freed;         /* إجمالي الذاكرة المُحرّرة منذ البداية */
    size_t   peak_usage;          /* أعلى استخدام تم تسجيله */
    uint64_t alloc_count;         /* عدد عمليات التخصيص */
    uint64_t free_count;          /* عدد عمليات التحرير */
    uint64_t arena_resets;        /* عدد مرات إعادة تعيين الأرينا */
    uint32_t pool_hits;           /* عدد المرات التي خدم فيها المجمّع طلباً */
    uint32_t pool_misses;         /* عدد المرات التي فشل فيها المجمّع */
    uint32_t gc_cycles;           /* عدد دورات كاشف الدورات */
    uint32_t gc_collected;        /* عدد الكائنات المُحرّرة بواسطة GC */
    size_t   memory_limit;        /* الحد الأقصى المسموح */
} SadMemoryStats;

/**
 * @brief (AR) تهيئة نظام الذاكرة — يجب استدعاؤها مرة واحدة عند بدء التطبيق
 * @brief (EN) Initialize memory system — must be called once at app startup
 *
 * تُنشئ المجمّعات الخمسة وتُعيّن الأرينا الافتراضية.
 *
 * @return true إذا نجحت التهيئة، false إذا فشلت
 */
bool sad_memory_init(void);

/**
 * @brief (AR) إيقاف نظام الذاكرة — يُحرّر كل الذاكرة المتبقية
 * @brief (EN) Shutdown memory system — frees all remaining memory
 *
 * يُطبع تحذيراً إذا وُجد تسريب ذاكرة (كائنات لم تُحرَّر).
 */
void sad_memory_shutdown(void);

/**
 * @brief (AR) تخصيص ذاكرة عبر المدير الموحّد
 * @brief (EN) Allocate via unified manager
 *
 * يختار المُخصص الأنسب تلقائياً:
 * - ≤256 بايت: المجمّع (Pool)
 * - >256 بايت: malloc + ترويسة
 *
 * @param size الحجم المطلوب
 * @return مؤشر للذاكرة، أو NULL إذا تجاوز الحد الأقصى
 */
void* sad_alloc(size_t size);

/**
 * @brief (AR) تخصيص ذاكرة مُحاذاة
 * @brief (EN) Allocate aligned memory
 */
void* sad_alloc_aligned(size_t size, size_t alignment);

/**
 * @brief (AR) تحرير ذاكرة مُخصصة بـ sad_alloc
 * @brief (EN) Free memory allocated by sad_alloc
 */
void sad_free(void* ptr);

/**
 * @brief (AR) إعادة تخصيص ذاكرة (تكبير أو تصغير)
 * @brief (EN) Reallocate memory (grow or shrink)
 */
void* sad_realloc(void* ptr, size_t new_size);

/**
 * @brief (AR) تخصيص وتصفير (calloc)
 * @brief (EN) Allocate and zero (calloc)
 */
void* sad_calloc(size_t count, size_t size);

/**
 * @brief (AR) تعيين الحد الأقصى للذاكرة
 * @brief (EN) Set memory limit
 */
void sad_memory_set_limit(size_t max_bytes);

/**
 * @brief (AR) الحصول على إحصائيات الذاكرة الحالية
 * @brief (EN) Get current memory statistics
 */
SadMemoryStats sad_memory_get_stats(void);

/**
 * @brief (AR) طباعة تقرير الذاكرة (للتشخيص)
 * @brief (EN) Print memory report (for diagnostics)
 */
void sad_memory_dump_stats(void);

#ifdef __cplusplus
}
#endif

#endif /* SAD_ANDROID_MEMORY_H */
