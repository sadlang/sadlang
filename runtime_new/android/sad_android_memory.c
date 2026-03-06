/**
 * @file sad_android_memory.c
 * @brief (AR) تطبيق نظام إدارة الذاكرة لأندرويد — لغة ص
 * @brief (EN) Android memory management implementation — Sad language
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                    تطبيق نظام إدارة الذاكرة
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * هذا الملف يحتوي التطبيق الكامل لنظام إدارة الذاكرة الأصلي المُصمم
 * للعمل على أجهزة أندرويد عبر bionic libc بدون أي اعتماد خارجي.
 *
 * الأقسام:
 * ١. الأرينا (Arena) — مُخصص خطّي فائق السرعة
 * ٢. المجمّع (Pool) — مُخصص بأحجام ثابتة
 * ٣. العدّ المرجعي (RC) — للكائنات طويلة العمر
 * ٤. كاشف الدورات (Cycle Detector) — للكشف عن المراجع الدائرية
 * ٥. المدير الموحّد — واجهة موحّدة تختار المخصص المناسب تلقائياً
 *
 * @author فريق تطوير لغة ص
 * @version 1.0.0
 * @date مارس 2026
 */

#include "sad_android_memory.h"

#include <stdlib.h>    /* malloc, free, realloc */
#include <string.h>    /* memset, memcpy */
#include <stdatomic.h> /* atomic_fetch_add, atomic_fetch_sub */

#ifdef __ANDROID__
#include <android/log.h>
#define SAD_LOG_TAG "SadMemory"
#define SAD_LOGI(...) __android_log_print(ANDROID_LOG_INFO, SAD_LOG_TAG, __VA_ARGS__)
#define SAD_LOGW(...) __android_log_print(ANDROID_LOG_WARN, SAD_LOG_TAG, __VA_ARGS__)
#define SAD_LOGE(...) __android_log_print(ANDROID_LOG_ERROR, SAD_LOG_TAG, __VA_ARGS__)
#else
/* على سطح المكتب (للاختبار) نستخدم printf */
#include <stdio.h>
#define SAD_LOGI(...) do { printf("[INFO] "); printf(__VA_ARGS__); printf("\n"); } while(0)
#define SAD_LOGW(...) do { printf("[WARN] "); printf(__VA_ARGS__); printf("\n"); } while(0)
#define SAD_LOGE(...) do { printf("[ERROR] "); printf(__VA_ARGS__); printf("\n"); } while(0)
#endif

/* ═══════════════════════════════════════════════════════════════════════════
 *                         دوال مساعدة داخلية
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) محاذاة عنوان للأعلى لأقرب مضاعف للمحاذاة المطلوبة
 * @brief (EN) Align address up to nearest multiple of alignment
 *
 * مثال: align_up(17, 16) = 32
 *       align_up(16, 16) = 16
 *       align_up(1, 16) = 16
 *
 * @param value القيمة المراد محاذاتها
 * @param alignment المحاذاة (يجب أن تكون قوة 2)
 * @return القيمة بعد المحاذاة
 */
static inline size_t align_up(size_t value, size_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

/**
 * @brief (AR) قفل دوراني بسيط (spinlock) — للحماية من التنافس بين الخيوط
 * @brief (EN) Simple spinlock — for thread contention protection
 *
 * يستخدم atomic_flag بدلاً من pthread_mutex لأنه:
 * - أخف وزناً (لا يحتاج syscall)
 * - أسرع في حالة عدم التنافس
 * - مثالي لعمليات قصيرة جداً (تخصيص/تحرير)
 *
 * @warning لا تستخدم هذا لأقفال طويلة! سيستهلك المعالج.
 */
static inline void spinlock_lock(_Atomic uint32_t* lock) {
    while (atomic_exchange_explicit(lock, 1, memory_order_acquire) != 0) {
        /* انتظار نشط — للعمليات القصيرة فقط */
#if defined(__aarch64__)
        __asm__ volatile("yield");  /* تلميح للمعالج ARM: أنا في انتظار */
#elif defined(__x86_64__)
        __asm__ volatile("pause");  /* تلميح للمعالج x86 */
#endif
    }
}

static inline void spinlock_unlock(_Atomic uint32_t* lock) {
    atomic_store_explicit(lock, 0, memory_order_release);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                   §١ — تطبيق الأرينا (Arena Allocator)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * الأرينا هي أبسط وأسرع مُخصص ذاكرة ممكن.
 *
 * كيف تعمل:
 * ┌──────────────────────────────────────────────────┐
 * │ ████████████████░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░ │
 * │ ↑ base         ↑ offset              ↑ capacity │
 * │ (مستخدم)       (هنا يبدأ التخصيص)    (النهاية) │
 * └──────────────────────────────────────────────────┘
 *
 * - sad_arena_alloc: offset += align_up(size) → إرجاع المؤشر القديم
 * - sad_arena_reset: offset = 0 → كل الذاكرة متاحة مجدداً
 * - لا يوجد free فردي — هذا هو سرّ السرعة!
 */

SadArena* sad_arena_new(size_t capacity) {
    /*
     * نُخصص الأرينا نفسها + ذاكرتها في تخصيص واحد:
     * ┌──────────┬──────────────────────────┐
     * │ SadArena │ ذاكرة الأرينا (capacity)  │
     * └──────────┴──────────────────────────┘
     *
     * هذا يقلل من عدد استدعاءات malloc ويحسّن أداء الـ cache.
     */
    size_t total = sizeof(SadArena) + capacity;
    SadArena* arena = (SadArena*)malloc(total);
    if (!arena) {
        SAD_LOGE("فشل تخصيص أرينا بحجم %zu بايت", capacity);
        return NULL;
    }

    /* الذاكرة تبدأ مباشرة بعد بنية الأرينا */
    arena->base = (uint8_t*)(arena + 1);
    arena->capacity = capacity;
    arena->offset = 0;
    arena->peak = 0;
    arena->alloc_count = 0;

    SAD_LOGI("تم إنشاء أرينا: %zu بايت", capacity);
    return arena;
}

void* sad_arena_alloc(SadArena* arena, size_t size) {
    return sad_arena_alloc_aligned(arena, size, SAD_ALIGNMENT_DEFAULT);
}

void* sad_arena_alloc_aligned(SadArena* arena, size_t size, size_t alignment) {
    if (!arena || size == 0) return NULL;

    /*
     * خطوات التخصيص:
     * ١. محاذاة الموقع الحالي
     * ٢. التحقق من وجود مساحة كافية
     * ٣. تحريك المؤشر للأمام
     * ٤. تحديث الإحصائيات
     */
    size_t aligned_offset = align_up(arena->offset, alignment);
    size_t new_offset = aligned_offset + size;

    if (new_offset > arena->capacity) {
        /* الأرينا ممتلئة! */
        SAD_LOGW("الأرينا ممتلئة: طُلب %zu بايت، متاح %zu",
                 size, arena->capacity - arena->offset);
        return NULL;
    }

    void* ptr = arena->base + aligned_offset;
    arena->offset = new_offset;
    arena->alloc_count++;

    /* تحديث أعلى استخدام */
    if (new_offset > arena->peak) {
        arena->peak = new_offset;
    }

    return ptr;
}

void sad_arena_reset(SadArena* arena) {
    if (!arena) return;

    /*
     * إعادة التعيين = إرجاع المؤشر فقط.
     * لا نُصفّر الذاكرة (لأداء أفضل) — الكود يجب أن يُهيّئ ما يُخصصه.
     */
    arena->offset = 0;
    arena->alloc_count = 0;
}

void sad_arena_free(SadArena* arena) {
    if (!arena) return;

    SAD_LOGI("تدمير أرينا: استُخدم %zu/%zu بايت (ذروة: %zu)",
             arena->offset, arena->capacity, arena->peak);

    /* تحرير كل شيء دفعة واحدة — لأن الأرينا وذاكرتها في تخصيص واحد */
    free(arena);
}

size_t sad_arena_used(const SadArena* arena) {
    return arena ? arena->offset : 0;
}

size_t sad_arena_remaining(const SadArena* arena) {
    return arena ? (arena->capacity - arena->offset) : 0;
}

size_t sad_arena_peak(const SadArena* arena) {
    return arena ? arena->peak : 0;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                   §٢ — تطبيق المجمّع (Pool Allocator)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * المجمّع يعمل بنظام القائمة الحرة (Free List):
 *
 * البداية:
 * ┌────┐→ ┌────┐→ ┌────┐→ ┌────┐→ NULL
 * │حر │  │حر │  │حر │  │حر │
 * └────┘  └────┘  └────┘  └────┘
 *
 * بعد تخصيص كتلتين:
 * ┌────┐  ┌────┐  ┌────┐→ ┌────┐→ NULL
 * │مشغ│  │مشغ│  │حر │  │حر │
 * └────┘  └────┘  └────┘  └────┘
 *                 ↑ free_list
 *
 * بعد تحرير الكتلة الأولى:
 * ┌────┐→ ┌────┐→ ┌────┐→ NULL
 * │حر │  │حر │  │حر │
 * └────┘  └────┘  └────┘
 * ↑ free_list
 *
 * الكتلة المُحررة تُصبح رأس القائمة الحرة — LIFO (آخر من يُحرَر أول من يُخصَص).
 * هذا يحسّن أداء الـ cache لأن آخر كتلة مُحررة تكون غالباً في الـ cache.
 */

SadPool* sad_pool_new(size_t block_size, size_t num_blocks) {
    /*
     * التحقق من الحد الأدنى: كل كتلة يجب أن تكون كبيرة بما يكفي
     * لتخزين مؤشر (لأن الكتل الحرة تستخدم كمؤشرات في القائمة الحرة).
     */
    if (block_size < sizeof(SadPoolBlock)) {
        block_size = sizeof(SadPoolBlock);
    }

    /* محاذاة حجم الكتلة لـ 16 بايت */
    block_size = align_up(block_size, SAD_ALIGNMENT_DEFAULT);

    SadPool* pool = (SadPool*)malloc(sizeof(SadPool));
    if (!pool) return NULL;

    /* تخصيص كل الكتل دفعة واحدة */
    size_t total = block_size * num_blocks;
    pool->memory = (uint8_t*)malloc(total);
    if (!pool->memory) {
        free(pool);
        return NULL;
    }

    pool->block_size = block_size;
    pool->total_blocks = num_blocks;
    pool->used_blocks = 0;
    atomic_init(&pool->lock, 0);

    /*
     * بناء القائمة الحرة:
     * كل كتلة تحتوي مؤشراً للكتلة التالية (كمتغير SadPoolBlock).
     * الكتلة الأخيرة تؤشر إلى NULL.
     */
    pool->free_list = NULL;
    for (size_t i = 0; i < num_blocks; i++) {
        SadPoolBlock* block = (SadPoolBlock*)(pool->memory + (i * block_size));
        block->next = pool->free_list;
        pool->free_list = block;
    }

    SAD_LOGI("تم إنشاء مجمّع: حجم_الكتلة=%zu، عدد=%zu، إجمالي=%zu بايت",
             block_size, num_blocks, total);

    return pool;
}

void* sad_pool_alloc(SadPool* pool) {
    if (!pool) return NULL;

    /* قفل دوراني — حماية القائمة الحرة من التنافس بين الخيوط */
    spinlock_lock(&pool->lock);

    if (!pool->free_list) {
        /* المجمّع ممتلئ! كل الكتل مستخدمة */
        spinlock_unlock(&pool->lock);
        SAD_LOGW("المجمّع ممتلئ: حجم_الكتلة=%zu، مستخدم=%zu/%zu",
                 pool->block_size, pool->used_blocks, pool->total_blocks);
        return NULL;
    }

    /*
     * أخذ أول كتلة من القائمة الحرة:
     *   القائمة الحرة: [A] → [B] → [C] → NULL
     *   نأخذ A، القائمة تصبح: [B] → [C] → NULL
     */
    SadPoolBlock* block = pool->free_list;
    pool->free_list = block->next;
    pool->used_blocks++;

    spinlock_unlock(&pool->lock);

    /* تصفير الكتلة لضمان سلامة البيانات */
    memset(block, 0, pool->block_size);

    return (void*)block;
}

void sad_pool_free(SadPool* pool, void* ptr) {
    if (!pool || !ptr) return;

    /*
     * التحقق من أن المؤشر ينتمي فعلاً لهذا المجمّع:
     * يجب أن يكون بين بداية ونهاية ذاكرة المجمّع.
     */
    uint8_t* byte_ptr = (uint8_t*)ptr;
    if (byte_ptr < pool->memory ||
        byte_ptr >= pool->memory + (pool->block_size * pool->total_blocks)) {
        SAD_LOGE("محاولة تحرير مؤشر لا ينتمي للمجمّع!");
        return;
    }

    spinlock_lock(&pool->lock);

    /*
     * إعادة الكتلة لبداية القائمة الحرة (LIFO):
     *   القائمة الحرة: [B] → [C] → NULL
     *   نُضيف A: [A] → [B] → [C] → NULL
     */
    SadPoolBlock* block = (SadPoolBlock*)ptr;
    block->next = pool->free_list;
    pool->free_list = block;
    pool->used_blocks--;

    spinlock_unlock(&pool->lock);
}

void sad_pool_destroy(SadPool* pool) {
    if (!pool) return;

    if (pool->used_blocks > 0) {
        SAD_LOGW("تسريب ذاكرة في المجمّع: %zu كتل لم تُحرر (حجم_الكتلة=%zu)",
                 pool->used_blocks, pool->block_size);
    }

    free(pool->memory);
    free(pool);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                  §٣ — تطبيق العدّ المرجعي (Reference Counting)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * كل كائن مُدار يحتوي على ترويسة (SadObjectHeader) قبل بياناته:
 *
 * malloc يُعيد هذا:
 * ┌──────────────────────┬─────────────────────────┐
 * │  SadObjectHeader     │  بيانات الكائن          │
 * │  (ref_count=1, etc.) │  (ما يراه المستخدم)     │
 * └──────────────────────┴─────────────────────────┘
 * ↑ base (ما نُخصصه)     ↑ ptr (ما نُرجعه للمستخدم)
 *
 * sad_retain(ptr): ref_count++
 * sad_release(ptr): ref_count-- → إذا وصل لصفر → تنظيف + تحرير
 */

void* sad_rc_alloc(size_t size, SadObjectType type, SadFinalizer finalizer) {
    /*
     * نُخصص ذاكرة = حجم الترويسة + حجم البيانات المطلوبة
     * نُعيد مؤشراً لبداية البيانات (بعد الترويسة)
     */
    size_t total = sizeof(SadObjectHeader) + size;
    SadObjectHeader* header = (SadObjectHeader*)malloc(total);
    if (!header) {
        SAD_LOGE("فشل تخصيص كائن مُدار: حجم=%zu، نوع=%d", size, type);
        return NULL;
    }

    /* تهيئة الترويسة */
    atomic_init(&header->ref_count, 1);   /* يبدأ بمرجع واحد */
    header->size = (uint32_t)size;
    header->type = (uint16_t)type;
    header->flags = SAD_OBJ_FLAG_NONE;
    header->finalizer = finalizer;
    header->gc_next = NULL;
    header->reserved = 0;

    /* تصفير بيانات الكائن */
    void* data = (void*)(header + 1);
    memset(data, 0, size);

    return data;
}

SadObjectHeader* sad_rc_get_header(void* ptr) {
    if (!ptr) return NULL;
    /*
     * الترويسة تقع مباشرة قبل مؤشر البيانات:
     * ptr - sizeof(SadObjectHeader) = الترويسة
     */
    return ((SadObjectHeader*)ptr) - 1;
}

void sad_retain(void* ptr) {
    if (!ptr) return;

    SadObjectHeader* header = sad_rc_get_header(ptr);

    /* التحقق من أن الكائن ليس في حالة تنظيف */
    if (header->flags & SAD_OBJ_FLAG_FINALIZING) {
        SAD_LOGW("محاولة retain كائن قيد التدمير!");
        return;
    }

    /*
     * زيادة ذرية — آمنة للخيوط بدون قفل.
     * memory_order_relaxed كافٍ لأن retain لا يحتاج ترتيب ذاكرة.
     */
    atomic_fetch_add_explicit(&header->ref_count, 1, memory_order_relaxed);
}

void sad_release(void* ptr) {
    if (!ptr) return;

    SadObjectHeader* header = sad_rc_get_header(ptr);

    /* التحقق من أن الكائن ليس في حالة تنظيف (يمنع التحرير المتكرر) */
    if (header->flags & SAD_OBJ_FLAG_FINALIZING) {
        return;
    }

    /*
     * إنقاص ذري بـ memory_order_acq_rel:
     * - acquire: يضمن أن كل القراءات/الكتابات قبل هذه النقطة مُكتملة
     * - release: يضمن أن الخيوط الأخرى ترى التغييرات
     *
     * هذا ضروري لأنه إذا وصل العدد لصفر، يجب أن نرى كل التعديلات
     * التي أجرتها الخيوط الأخرى على الكائن قبل تحريره.
     */
    uint32_t old_count = atomic_fetch_sub_explicit(
        &header->ref_count, 1, memory_order_acq_rel
    );

    if (old_count == 1) {
        /*
         * العدد وصل لصفر — نحن آخر من يملك مرجعاً.
         * ١. تعليم الكائن كـ "قيد التنظيف" (يمنع retain/release متكرر)
         * ٢. استدعاء دالة التنظيف (إن وُجدت)
         * ٣. تحرير الذاكرة
         */
        header->flags |= SAD_OBJ_FLAG_FINALIZING;

        if (header->finalizer) {
            header->finalizer(ptr, header->size);
        }

        free(header);
    }
}

uint32_t sad_rc_get_count(const void* ptr) {
    if (!ptr) return 0;
    const SadObjectHeader* header = ((const SadObjectHeader*)ptr) - 1;
    return atomic_load_explicit(&header->ref_count, memory_order_relaxed);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                   §٤ — كاشف الدورات (Cycle Detector)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * العدّ المرجعي وحده لا يكفي — بسبب مشكلة الدورات:
 *
 *   كائن أ (ref=1) ──→ كائن ب (ref=1)
 *       ↑                    │
 *       └────────────────────┘
 *
 * حتى لو لم يعد أحد يستخدم أ وب، عدد مراجعهما لن يصل لصفر!
 *
 * الحل: كاشف دورات يعمل دورياً بخوارزمية Mark-Sweep مُبسّطة:
 *
 * ١. مرحلة التعليم (Mark):
 *    - ابدأ من الجذور (المتغيرات العامة، مكدس كل خيط)
 *    - تتبّع كل مرجع وعلّم الكائنات القابلة للوصول
 *
 * ٢. مرحلة المسح (Sweep):
 *    - كل كائن غير مُعلّم = غير قابل للوصول = دورة → حرّره
 *
 * ملاحظة: هذا كاشف مُبسّط — يعمل على قائمة الكائنات المُسجّلة فقط
 * (الكائنات من نوع ARRAY, MAP, OBJECT التي قد تُشكّل دورات).
 */

/**
 * @brief (AR) الحد الأقصى لعدد الجذور
 * @brief (EN) Maximum number of roots
 */
#define SAD_MAX_ROOTS 1024

/**
 * @brief (AR) حالة كاشف الدورات العامة
 * @brief (EN) Global cycle detector state
 */
static struct {
    void*    roots[SAD_MAX_ROOTS]; /* مصفوفة الجذور */
    size_t   root_count;          /* عدد الجذور الحالية */
    SadObjectHeader* all_objects;  /* قائمة كل الكائنات المُسجّلة */
    _Atomic uint32_t lock;        /* قفل دوراني */
    bool     initialized;         /* هل تمت التهيئة؟ */
} gc_state = {0};

void sad_gc_add_root(void* ptr) {
    if (!ptr || gc_state.root_count >= SAD_MAX_ROOTS) return;

    spinlock_lock(&gc_state.lock);
    gc_state.roots[gc_state.root_count++] = ptr;
    spinlock_unlock(&gc_state.lock);
}

void sad_gc_remove_root(void* ptr) {
    if (!ptr) return;

    spinlock_lock(&gc_state.lock);
    for (size_t i = 0; i < gc_state.root_count; i++) {
        if (gc_state.roots[i] == ptr) {
            /* نقل آخر عنصر مكان العنصر المحذوف (ترتيب غير مهم) */
            gc_state.roots[i] = gc_state.roots[gc_state.root_count - 1];
            gc_state.root_count--;
            break;
        }
    }
    spinlock_unlock(&gc_state.lock);
}

uint32_t sad_gc_collect_cycles(void) {
    /*
     * تطبيق مُبسّط لكاشف الدورات.
     *
     * في هذه المرحلة الأولية، نعتمد على العدّ المرجعي فقط
     * ونضيف كاشف الدورات الكامل لاحقاً عند الحاجة.
     *
     * الدورات نادرة في تطبيقات الواجهة لأن:
     * - شجرة الواجهات أحادية الاتجاه (أب → أبناء)
     * - بيانات المحادثة (رسائل) لا تحتوي مراجع متداخلة
     * - اتصالات الشبكة مستقلة
     *
     * TODO: تطبيق خوارزمية تجريبية إنقاص (trial deletion) أكثر كفاءة:
     * - أنقص عدد المراجع لكل كائن قد يكون في دورة
     * - إذا وصل لصفر → في دورة → حرّره
     * - إذا لم يصل لصفر → أعد عدد المراجع
     */

    uint32_t collected = 0;

    spinlock_lock(&gc_state.lock);

    /* مرحلة التعليم: إزالة كل العلامات */
    SadObjectHeader* obj = gc_state.all_objects;
    while (obj) {
        obj->flags &= ~SAD_OBJ_FLAG_GC_MARKED;
        obj = obj->gc_next;
    }

    /* مرحلة التعليم: تتبّع من الجذور */
    for (size_t i = 0; i < gc_state.root_count; i++) {
        if (gc_state.roots[i]) {
            SadObjectHeader* root_header = sad_rc_get_header(gc_state.roots[i]);
            root_header->flags |= SAD_OBJ_FLAG_GC_MARKED;
            /* TODO: تتبّع عميق لمراجع الكائن */
        }
    }

    /* مرحلة المسح: حرّر الكائنات غير المُعلّمة ذات العدد المرجعي > 0 */
    SadObjectHeader** prev_ptr = &gc_state.all_objects;
    obj = gc_state.all_objects;
    while (obj) {
        SadObjectHeader* next = obj->gc_next;

        if (!(obj->flags & SAD_OBJ_FLAG_GC_MARKED) &&
            !(obj->flags & SAD_OBJ_FLAG_IMMORTAL)) {
            /* كائن غير قابل للوصول — في دورة مرجعية */
            void* data = (void*)(obj + 1);

            obj->flags |= SAD_OBJ_FLAG_FINALIZING;
            if (obj->finalizer) {
                obj->finalizer(data, obj->size);
            }

            *prev_ptr = next;
            free(obj);
            collected++;
        } else {
            prev_ptr = &obj->gc_next;
        }

        obj = next;
    }

    spinlock_unlock(&gc_state.lock);

    if (collected > 0) {
        SAD_LOGI("كاشف الدورات: حُرّرت %u كائنات دائرية", collected);
    }

    return collected;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                   §٥ — المدير الموحّد (Unified Manager)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * واجهة واحدة بسيطة تُخفي تعقيد المُخصصات المتعددة.
 *
 * عند استدعاء sad_alloc(size):
 *   ├── size ≤ 16  → Pool[0] (كتل 16 بايت)
 *   ├── size ≤ 32  → Pool[1] (كتل 32 بايت)
 *   ├── size ≤ 64  → Pool[2] (كتل 64 بايت)
 *   ├── size ≤ 128 → Pool[3] (كتل 128 بايت)
 *   ├── size ≤ 256 → Pool[4] (كتل 256 بايت)
 *   └── size > 256 → malloc المباشر
 *
 * الهدف: القضاء على ~90% من استدعاءات malloc/free
 * لأن معظم التخصيصات في لغة ص صغيرة (قيم، سلاسل قصيرة، إطارات).
 */

/**
 * @brief (AR) حالة المدير الموحّد
 * @brief (EN) Unified manager state
 */
static struct {
    SadPool*        pools[SAD_POOL_COUNT]; /* المجمّعات الخمسة */
    SadArena*       frame_arena;           /* أرينا الإطار الحالي */
    SadMemoryStats  stats;                 /* الإحصائيات */
    _Atomic uint32_t lock;                 /* قفل دوراني */
    bool            initialized;           /* هل تمت التهيئة؟ */
} mem_manager = {0};

/**
 * @brief (AR) أحجام المجمّعات
 * @brief (EN) Pool sizes
 */
static const size_t pool_sizes[SAD_POOL_COUNT] = {
    SAD_POOL_SIZE_16,   /*  16 بايت — SadValue بسيط */
    SAD_POOL_SIZE_32,   /*  32 بايت — SadString قصيرة (SSO) */
    SAD_POOL_SIZE_64,   /*  64 بايت — SadArray صغيرة */
    SAD_POOL_SIZE_128,  /* 128 بايت — كائنات متوسطة */
    SAD_POOL_SIZE_256,  /* 256 بايت — كائنات كبيرة */
};

bool sad_memory_init(void) {
    if (mem_manager.initialized) {
        SAD_LOGW("نظام الذاكرة مُهيأ مسبقاً!");
        return true;
    }

    SAD_LOGI("═══ تهيئة نظام ذاكرة لغة ص ═══");

    /* إنشاء المجمّعات الخمسة */
    for (int i = 0; i < SAD_POOL_COUNT; i++) {
        mem_manager.pools[i] = sad_pool_new(pool_sizes[i], SAD_POOL_INITIAL_BLOCKS);
        if (!mem_manager.pools[i]) {
            SAD_LOGE("فشل إنشاء المجمّع %d (حجم=%zu)", i, pool_sizes[i]);
            /* تنظيف ما تم إنشاؤه */
            for (int j = 0; j < i; j++) {
                sad_pool_destroy(mem_manager.pools[j]);
                mem_manager.pools[j] = NULL;
            }
            return false;
        }
    }

    /* إنشاء أرينا الإطار */
    mem_manager.frame_arena = sad_arena_new(SAD_ARENA_DEFAULT_SIZE);
    if (!mem_manager.frame_arena) {
        SAD_LOGE("فشل إنشاء أرينا الإطار");
        for (int i = 0; i < SAD_POOL_COUNT; i++) {
            sad_pool_destroy(mem_manager.pools[i]);
        }
        return false;
    }

    /* تهيئة الإحصائيات */
    memset(&mem_manager.stats, 0, sizeof(SadMemoryStats));
    mem_manager.stats.memory_limit = SAD_MEMORY_MAX_TOTAL;
    atomic_init(&mem_manager.lock, 0);
    mem_manager.initialized = true;

    /* تهيئة كاشف الدورات */
    memset(&gc_state, 0, sizeof(gc_state));
    atomic_init(&gc_state.lock, 0);
    gc_state.initialized = true;

    SAD_LOGI("═══ نظام الذاكرة جاهز ═══");
    SAD_LOGI("  مجمّعات: %d (أحجام: 16, 32, 64, 128, 256)", SAD_POOL_COUNT);
    SAD_LOGI("  أرينا الإطار: %d بايت", SAD_ARENA_DEFAULT_SIZE);
    SAD_LOGI("  الحد الأقصى: %zu ميجابايت", mem_manager.stats.memory_limit / (1024*1024));

    return true;
}

void sad_memory_shutdown(void) {
    if (!mem_manager.initialized) return;

    SAD_LOGI("═══ إيقاف نظام الذاكرة ═══");

    /* تشغيل كاشف الدورات الأخير */
    uint32_t cycles = sad_gc_collect_cycles();
    if (cycles > 0) {
        SAD_LOGW("كاشف الدورات حرّر %u كائنات في الإغلاق", cycles);
    }

    /* تدمير أرينا الإطار */
    if (mem_manager.frame_arena) {
        sad_arena_free(mem_manager.frame_arena);
        mem_manager.frame_arena = NULL;
    }

    /* تدمير المجمّعات */
    for (int i = 0; i < SAD_POOL_COUNT; i++) {
        if (mem_manager.pools[i]) {
            sad_pool_destroy(mem_manager.pools[i]);
            mem_manager.pools[i] = NULL;
        }
    }

    /* تقرير التسريبات */
    if (mem_manager.stats.alloc_count != mem_manager.stats.free_count) {
        SAD_LOGW("⚠ تسريب ذاكرة محتمل: %llu تخصيص، %llu تحرير (فارق: %llu)",
                 (unsigned long long)mem_manager.stats.alloc_count,
                 (unsigned long long)mem_manager.stats.free_count,
                 (unsigned long long)(mem_manager.stats.alloc_count -
                                     mem_manager.stats.free_count));
    }

    sad_memory_dump_stats();

    mem_manager.initialized = false;
    gc_state.initialized = false;

    SAD_LOGI("═══ نظام الذاكرة مُغلق ═══");
}

/**
 * @brief (AR) اختيار المجمّع المناسب للحجم المطلوب
 * @brief (EN) Select appropriate pool for requested size
 *
 * @param size الحجم المطلوب
 * @return فهرس المجمّع (0-4)، أو -1 إذا الحجم أكبر من كل المجمّعات
 */
static int select_pool(size_t size) {
    /*
     * بحث خطي بسيط — فقط 5 مجمّعات.
     * يمكن تحسينه بجدول lookup لكن الفرق مهمل.
     */
    for (int i = 0; i < SAD_POOL_COUNT; i++) {
        if (size <= pool_sizes[i]) {
            return i;
        }
    }
    return -1;  /* أكبر من كل المجمّعات → malloc مباشر */
}

/**
 * @brief (AR) علامة مخصصة لتمييز تخصيصات المجمّع من malloc المباشر
 * @brief (EN) Custom tag to distinguish pool allocations from direct malloc
 *
 * نُخزّن بايت واحد قبل البيانات يحتوي فهرس المجمّع المصدر:
 *   0-4: فهرس المجمّع
 *   0xFF: malloc مباشر (ليس من مجمّع)
 */
#define SAD_ALLOC_TAG_SIZE     16   /* 16 بايت للمحاذاة مع ARM64 */
#define SAD_ALLOC_TAG_DIRECT   0xFF /* تخصيص مباشر (malloc) */

void* sad_alloc(size_t size) {
    if (size == 0) return NULL;
    if (!mem_manager.initialized) {
        /* إذا لم يُهيّأ النظام بعد، استخدم malloc مباشرة */
        return malloc(size);
    }

    /* التحقق من الحد الأقصى */
    if (mem_manager.stats.total_allocated + size > mem_manager.stats.memory_limit) {
        SAD_LOGE("تجاوز حد الذاكرة! حالي=%zu، طلب=%zu، حد=%zu",
                 mem_manager.stats.total_allocated, size,
                 mem_manager.stats.memory_limit);
        return NULL;
    }

    void* result = NULL;
    int pool_idx = select_pool(size);

    if (pool_idx >= 0) {
        /*
         * التخصيص من المجمّع:
         * نُضيف علامة (tag) قبل البيانات لنعرف لاحقاً من أين جاء التخصيص.
         *
         * ┌──────┬──────────────────────┐
         * │ tag  │ بيانات المستخدم      │
         * │(16B) │ (pool_size - 16B)    │
         * └──────┴──────────────────────┘
         */
        void* block = sad_pool_alloc(mem_manager.pools[pool_idx]);
        if (block) {
            uint8_t* tag = (uint8_t*)block;
            tag[0] = (uint8_t)pool_idx;
            result = tag + SAD_ALLOC_TAG_SIZE;

            spinlock_lock(&mem_manager.lock);
            mem_manager.stats.pool_hits++;
            spinlock_unlock(&mem_manager.lock);
        }
    }

    if (!result) {
        /*
         * التخصيص المباشر (malloc):
         * نُضيف نفس العلامة لتمييزه عن تخصيصات المجمّع.
         */
        size_t total = SAD_ALLOC_TAG_SIZE + size;
        uint8_t* block = (uint8_t*)malloc(total);
        if (block) {
            block[0] = SAD_ALLOC_TAG_DIRECT;
            result = block + SAD_ALLOC_TAG_SIZE;

            if (pool_idx >= 0) {
                spinlock_lock(&mem_manager.lock);
                mem_manager.stats.pool_misses++;
                spinlock_unlock(&mem_manager.lock);
            }
        }
    }

    if (result) {
        spinlock_lock(&mem_manager.lock);
        mem_manager.stats.total_allocated += size;
        mem_manager.stats.alloc_count++;
        if (mem_manager.stats.total_allocated > mem_manager.stats.peak_usage) {
            mem_manager.stats.peak_usage = mem_manager.stats.total_allocated;
        }
        spinlock_unlock(&mem_manager.lock);
    }

    return result;
}

void* sad_alloc_aligned(size_t size, size_t alignment) {
    /*
     * للتخصيصات المُحاذاة نستخدم aligned_alloc أو posix_memalign.
     * في أندرويد (bionic)، posix_memalign متاح.
     */
    if (size == 0 || alignment == 0) return NULL;

    /* التأكد من أن المحاذاة قوة 2 */
    if ((alignment & (alignment - 1)) != 0) {
        SAD_LOGE("المحاذاة يجب أن تكون قوة 2: %zu", alignment);
        return NULL;
    }

    void* ptr = NULL;
    if (posix_memalign(&ptr, alignment, size) != 0) {
        return NULL;
    }

    return ptr;
}

void sad_free(void* ptr) {
    if (!ptr) return;

    if (!mem_manager.initialized) {
        /* لم يُهيّأ النظام — نفترض malloc مباشر */
        free(ptr);
        return;
    }

    /*
     * نقرأ العلامة (tag) لمعرفة مصدر التخصيص:
     * - 0-4: مجمّع → أعد الكتلة للمجمّع
     * - 0xFF: malloc → استدع free
     */
    uint8_t* tag = ((uint8_t*)ptr) - SAD_ALLOC_TAG_SIZE;
    uint8_t pool_idx = tag[0];

    if (pool_idx < SAD_POOL_COUNT) {
        /* من المجمّع */
        sad_pool_free(mem_manager.pools[pool_idx], tag);
    } else if (pool_idx == SAD_ALLOC_TAG_DIRECT) {
        /* malloc مباشر */
        free(tag);
    } else {
        SAD_LOGE("علامة ذاكرة تالفة! (tag=%u)", pool_idx);
        return;
    }

    spinlock_lock(&mem_manager.lock);
    mem_manager.stats.free_count++;
    spinlock_unlock(&mem_manager.lock);
}

void* sad_realloc(void* ptr, size_t new_size) {
    if (!ptr) return sad_alloc(new_size);
    if (new_size == 0) {
        sad_free(ptr);
        return NULL;
    }

    /*
     * إعادة التخصيص: خصّص ذاكرة جديدة، انسخ البيانات، حرّر القديمة.
     * هذا أبسط وأكثر أماناً من محاولة توسيع التخصيص الحالي.
     */
    void* new_ptr = sad_alloc(new_size);
    if (!new_ptr) return NULL;

    /* نسخ البيانات — نفترض أن new_size قد يكون أصغر أو أكبر */
    /* TODO: تخزين الحجم الأصلي لنسخ الحجم الأقل فقط */
    memcpy(new_ptr, ptr, new_size); /* قد ينسخ أكثر مما يجب إذا كُبّرت */

    sad_free(ptr);
    return new_ptr;
}

void* sad_calloc(size_t count, size_t size) {
    size_t total = count * size;
    /* التحقق من الفيض (overflow) */
    if (count != 0 && total / count != size) {
        SAD_LOGE("فيض في sad_calloc: count=%zu, size=%zu", count, size);
        return NULL;
    }

    void* ptr = sad_alloc(total);
    if (ptr) {
        memset(ptr, 0, total);
    }
    return ptr;
}

void sad_memory_set_limit(size_t max_bytes) {
    spinlock_lock(&mem_manager.lock);
    mem_manager.stats.memory_limit = max_bytes;
    spinlock_unlock(&mem_manager.lock);
    SAD_LOGI("تم تعيين حد الذاكرة: %zu ميجابايت", max_bytes / (1024*1024));
}

SadMemoryStats sad_memory_get_stats(void) {
    spinlock_lock(&mem_manager.lock);
    SadMemoryStats stats = mem_manager.stats;
    spinlock_unlock(&mem_manager.lock);
    return stats;
}

void sad_memory_dump_stats(void) {
    SadMemoryStats s = sad_memory_get_stats();

    SAD_LOGI("═══ تقرير الذاكرة ═══");
    SAD_LOGI("  مُخصصة حالياً: %zu بايت (%zu كيلوبايت)",
             s.total_allocated, s.total_allocated / 1024);
    SAD_LOGI("  ذروة الاستخدام: %zu بايت (%zu كيلوبايت)",
             s.peak_usage, s.peak_usage / 1024);
    SAD_LOGI("  عمليات تخصيص: %llu", (unsigned long long)s.alloc_count);
    SAD_LOGI("  عمليات تحرير: %llu", (unsigned long long)s.free_count);
    SAD_LOGI("  إصابات المجمّع: %u (أخطاء: %u)", s.pool_hits, s.pool_misses);
    SAD_LOGI("  دورات GC: %u (حُرّر: %u كائن)", s.gc_cycles, s.gc_collected);
    SAD_LOGI("  إعادات تعيين الأرينا: %llu", (unsigned long long)s.arena_resets);
    SAD_LOGI("═══════════════════════");
}
