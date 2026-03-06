/**
 * @file sad_android_thread.h
 * @brief (AR) نظام الخيوط والتزامن لرانتايم أندرويد — لغة ص
 * @brief (EN) Threading and synchronization system for Android runtime
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                    نظام الخيوط والتزامن — لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * نظام الخيوط مبني على pthreads (مدعوم أصلياً في bionic/أندرويد).
 * يوفر:
 *  • إنشاء وإدارة الخيوط
 *  • أقفال (Mutex) — لحماية البيانات المشتركة
 *  • قنوات (Channels) — لتبادل الرسائل بين الخيوط
 *  • مجمع خيوط (Thread Pool) — لتنفيذ المهام بكفاءة
 *
 * ╔══════════════════════════════════════════════════════════════════╗
 * ║                    نموذج التزامن في لغة ص                     ║
 * ╠══════════════════════════════════════════════════════════════════╣
 * ║                                                                ║
 * ║  لغة ص تتبع نموذج "القنوات" (مستوحى من Go/Rust):             ║
 * ║                                                                ║
 * ║  خيط ١                    خيط ٢                               ║
 * ║  ┌──────────┐             ┌──────────┐                        ║
 * ║  │ حسابات  │─── قناة ──→│ معالجة   │                        ║
 * ║  │ بيانات  │   رسالة    │ النتائج  │                        ║
 * ║  └──────────┘             └──────────┘                        ║
 * ║                                                                ║
 * ║  بدلاً من مشاركة بيانات + أقفال (صعب وخطير)،               ║
 * ║  كل خيط يملك بياناته ويتواصل عبر القنوات.                    ║
 * ║                                                                ║
 * ║  القناة: طابور FIFO آمن بين الخيوط = أنبوب بيانات            ║
 * ║                                                                ║
 * ║  مثال بلغة ص:                                                 ║
 * ║  ──────────                                                   ║
 * ║  متغير قناة_رسائل = قناة_جديدة()                             ║
 * ║  خيط_جديد(دالة()                                             ║
 * ║    بينما (صحيح)                                               ║
 * ║      متغير رسالة = قناة_رسائل.استلم()                        ║
 * ║      اطبع("استلمت: " + رسالة)                                ║
 * ║    نهاية                                                      ║
 * ║  نهاية)                                                       ║
 * ║  قناة_رسائل.ارسل("مرحباً")                                   ║
 * ║                                                                ║
 * ╚══════════════════════════════════════════════════════════════════╝
 *
 * @author فريق تطوير لغة ص
 * @version 1.0.0
 */

#ifndef SAD_ANDROID_THREAD_H
#define SAD_ANDROID_THREAD_H

#include "sad_android_memory.h"
#include "sad_android_value.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §١ — ثوابت
 * ═══════════════════════════════════════════════════════════════════════════ */

/** @brief (AR) حجم المكدس الافتراضي لكل خيط (1 ميجابايت) */
#define SAD_THREAD_STACK_SIZE (1024 * 1024)

/** @brief (AR) سعة القناة الافتراضية */
#define SAD_CHANNEL_DEFAULT_CAPACITY 64

/** @brief (AR) عدد الخيوط الافتراضي في المجمع */
#define SAD_THREAD_POOL_DEFAULT_SIZE 4

/** @brief (AR) أقصى عدد خيوط */
#define SAD_THREAD_MAX_COUNT 32

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٢ — بنية القفل (Mutex)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) قفل التزامن — يحمي الوصول للبيانات المشتركة
 *
 * قفل بسيط يُغلّف pthread_mutex:
 *  • قفل() → انتظر حتى يتحرر ثم أقفل
 *  • حاول_قفل() → أقفل فوراً أو ارجع false
 *  • افتح() → حرّر القفل
 *
 * مثال:
 *   متغير قفل_المتغير = قفل_جديد()
 *   قفل_المتغير.قفل()
 *   # الكود المحمي هنا
 *   قفل_المتغير.افتح()
 */
typedef struct SadMutex {
    pthread_mutex_t handle;       /**< مقبض pthread */
    bool            initialized;  /**< هل مُهيّأ؟ */
} SadMutex;

/** @brief (AR) إنشاء قفل جديد */
bool sad_mutex_init(SadMutex* mutex);

/** @brief (AR) تدمير القفل */
void sad_mutex_destroy(SadMutex* mutex);

/** @brief (AR) قفل (ينتظر إذا مقفول) */
bool sad_mutex_lock(SadMutex* mutex);

/** @brief (AR) محاولة قفل (لا ينتظر) */
bool sad_mutex_try_lock(SadMutex* mutex);

/** @brief (AR) فتح القفل */
bool sad_mutex_unlock(SadMutex* mutex);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٣ — بنية القناة (Channel)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * القناة هي طابور FIFO آمن بين الخيوط.
 *
 * البنية الداخلية:
 * ┌────────────────────────────────────────────┐
 * │ buffer (مصفوفة دائرية من SadValue)         │
 * │ ┌────┬────┬────┬────┬────┬────┬────┬────┐  │
 * │ │    │val1│val2│val3│    │    │    │    │  │
 * │ └────┴────┴────┴────┴────┴────┴────┴────┘  │
 * │       ↑head          ↑tail                 │
 * │                                            │
 * │ mutex: يحمي الوصول المتزامن                │
 * │ cond_not_full: ينتظر المرسل إذا ممتلئة    │
 * │ cond_not_empty: ينتظر المستلم إذا فارغة   │
 * └────────────────────────────────────────────┘
 *
 * كل ارسال/استلام:
 *  ١. أقفل القناة
 *  ٢. انتظر إذا (ممتلئة/فارغة)
 *  ٣. انسخ القيمة
 *  ٤. أنبّه الطرف الآخر
 *  ٥. افتح القفل
 */
typedef struct SadChannel {
    SadValue*       buffer;           /**< مصفوفة القيم (دائرية) */
    size_t          capacity;         /**< سعة المصفوفة */
    size_t          count;            /**< عدد العناصر الحالي */
    size_t          head;             /**< رأس القراءة */
    size_t          tail;             /**< ذيل الكتابة */
    pthread_mutex_t mutex;            /**< قفل الحماية */
    pthread_cond_t  cond_not_full;    /**< شرط: ليست ممتلئة */
    pthread_cond_t  cond_not_empty;   /**< شرط: ليست فارغة */
    bool            closed;           /**< هل القناة مُغلقة؟ */
} SadChannel;

/**
 * @brief (AR) إنشاء قناة جديدة
 * @param capacity سعة القناة (0 = SAD_CHANNEL_DEFAULT_CAPACITY)
 * @return مؤشر للقناة، أو NULL عند الفشل
 */
SadChannel* sad_channel_new(size_t capacity);

/**
 * @brief (AR) إرسال قيمة عبر القناة
 * @param ch القناة
 * @param value القيمة المُراد إرسالها
 * @return true إذا نجح الإرسال، false إذا القناة مغلقة
 *
 * إذا القناة ممتلئة: الخيط ينتظر حتى يتوفر مكان.
 */
bool sad_channel_send(SadChannel* ch, SadValue value);

/**
 * @brief (AR) استلام قيمة من القناة
 * @param ch القناة
 * @param out_value مؤشر لتخزين القيمة المُستلمة
 * @return true إذا نجح الاستلام، false إذا القناة مغلقة وفارغة
 *
 * إذا القناة فارغة: الخيط ينتظر حتى يتوفر عنصر.
 */
bool sad_channel_receive(SadChannel* ch, SadValue* out_value);

/**
 * @brief (AR) محاولة إرسال بدون انتظار
 * @return true إذا نجح، false إذا ممتلئة أو مغلقة
 */
bool sad_channel_try_send(SadChannel* ch, SadValue value);

/**
 * @brief (AR) محاولة استلام بدون انتظار
 * @return true إذا نجح، false إذا فارغة أو مغلقة
 */
bool sad_channel_try_receive(SadChannel* ch, SadValue* out_value);

/**
 * @brief (AR) إغلاق القناة
 *
 * بعد الإغلاق:
 *  • الإرسال يفشل دائماً
 *  • الاستلام يستمر حتى تفريغ القناة ثم يفشل
 *  • كل الخيوط المنتظرة تُنبّه
 */
void sad_channel_close(SadChannel* ch);

/**
 * @brief (AR) تحرير القناة
 */
void sad_channel_free(SadChannel* ch);

/** @brief (AR) عدد العناصر في القناة */
size_t sad_channel_count(const SadChannel* ch);

/** @brief (AR) هل القناة مغلقة؟ */
bool sad_channel_is_closed(const SadChannel* ch);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٤ — بنية الخيط (Thread)
 * ═══════════════════════════════════════════════════════════════════════════ */

/** @brief (AR) نوع دالة الخيط */
typedef void* (*SadThreadFunc)(void* arg);

/**
 * @brief (AR) بنية الخيط
 */
typedef struct SadThread {
    pthread_t       handle;           /**< مقبض pthread */
    bool            running;          /**< هل يعمل؟ */
    bool            detached;         /**< هل منفصل؟ */
    SadString       name;             /**< اسم الخيط (للتتبع) */
} SadThread;

/**
 * @brief (AR) إنشاء وتشغيل خيط جديد
 * @param name اسم الخيط (اختياري)
 * @param func الدالة المُراد تنفيذها
 * @param arg وسيط يُمرّر للدالة
 * @return مؤشر للخيط أو NULL عند الفشل
 */
SadThread* sad_thread_new(const char* name, SadThreadFunc func, void* arg);

/**
 * @brief (AR) انتظار انتهاء الخيط
 * @param thread الخيط
 * @param out_result مؤشر لتخزين نتيجة الخيط (اختياري)
 * @return true إذا نجح الانتظار
 */
bool sad_thread_join(SadThread* thread, void** out_result);

/**
 * @brief (AR) فصل الخيط (لن ينتظره أحد)
 */
bool sad_thread_detach(SadThread* thread);

/**
 * @brief (AR) تحرير بنية الخيط
 */
void sad_thread_free(SadThread* thread);

/** @brief (AR) هل الخيط يعمل؟ */
bool sad_thread_is_running(const SadThread* thread);

/** @brief (AR) تأخير الخيط الحالي (بالمللي ثانية) */
void sad_thread_sleep(uint32_t milliseconds);

/** @brief (AR) التنازل عن المعالج لخيط آخر */
void sad_thread_yield(void);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٥ — مجمع الخيوط (Thread Pool)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * مجمع الخيوط يُعيد استخدام مجموعة ثابتة من الخيوط.
 * بدلاً من إنشاء خيط جديد لكل مهمة (مكلف!)،
 * نُضيف المهام لطابور ونترك الخيوط الموجودة تسحبها.
 *
 * ┌─────────────┐     ┌──────────────────────┐
 * │ مهمة ١     │────→│                      │
 * │ مهمة ٢     │────→│  طابور المهام        │
 * │ مهمة ٣     │────→│  ┌───┬───┬───┬───┐   │
 * │    ...      │     │  │ 1 │ 2 │ 3 │ 4 │   │
 * └─────────────┘     │  └───┴───┴───┴───┘   │
 *                     └────────────────────────┘
 *                              ↓ ↓ ↓ ↓
 *                     ┌────────────────────────┐
 *                     │ خيط١ خيط٢ خيط٣ خيط٤  │
 *                     │ يسحب  يسحب  ينتظر ... │
 *                     └────────────────────────┘
 */

/** @brief (AR) نوع مهمة في مجمع الخيوط */
typedef void (*SadTaskFunc)(void* arg);

/** @brief (AR) مهمة في الطابور */
typedef struct SadTask {
    SadTaskFunc     func;             /**< الدالة */
    void*           arg;              /**< الوسيط */
    struct SadTask* next;             /**< المهمة التالية */
} SadTask;

/**
 * @brief (AR) مجمع الخيوط
 */
typedef struct SadThreadPool {
    pthread_t*      threads;          /**< مصفوفة الخيوط */
    size_t          thread_count;     /**< عدد الخيوط */
    SadTask*        task_head;        /**< رأس طابور المهام */
    SadTask*        task_tail;        /**< ذيل طابور المهام */
    size_t          task_count;       /**< عدد المهام المنتظرة */
    pthread_mutex_t mutex;            /**< قفل الحماية */
    pthread_cond_t  cond_task;        /**< شرط: مهمة جديدة */
    bool            shutdown;         /**< هل يتم الإيقاف؟ */
} SadThreadPool;

/**
 * @brief (AR) إنشاء مجمع خيوط
 * @param thread_count عدد الخيوط (0 = SAD_THREAD_POOL_DEFAULT_SIZE)
 * @return مؤشر لمجمع الخيوط
 */
SadThreadPool* sad_thread_pool_new(size_t thread_count);

/**
 * @brief (AR) إضافة مهمة للمجمع
 * @param pool المجمع
 * @param func الدالة
 * @param arg الوسيط
 * @return true إذا نجحت الإضافة
 */
bool sad_thread_pool_submit(SadThreadPool* pool, SadTaskFunc func, void* arg);

/**
 * @brief (AR) إيقاف المجمع وانتظار انتهاء كل المهام
 */
void sad_thread_pool_shutdown(SadThreadPool* pool);

/**
 * @brief (AR) تحرير المجمع
 */
void sad_thread_pool_free(SadThreadPool* pool);

#ifdef __cplusplus
}
#endif

#endif /* SAD_ANDROID_THREAD_H */
