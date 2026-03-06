/**
 * @file sad_android_thread.c
 * @brief (AR) تطبيق الخيوط والقنوات ومجمع الخيوط — لغة ص لأندرويد
 * @brief (EN) Threading, channels, and thread pool implementation
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                    تطبيق نظام التزامن — لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @author فريق تطوير لغة ص
 * @version 1.0.0
 */

#include "sad_android_thread.h"
#include <string.h>   /* memset, memcpy */
#include <time.h>     /* nanosleep */
#include <sched.h>    /* sched_yield */
#include <stdlib.h>   /* malloc, free */

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §١ — القفل (Mutex)
 * ═══════════════════════════════════════════════════════════════════════════ */

bool sad_mutex_init(SadMutex* mutex) {
    if (!mutex) return false;

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    /* ERRORCHECK يكشف الأقفال الميتة والأخطاء أثناء التطوير */
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

    int result = pthread_mutex_init(&mutex->handle, &attr);
    pthread_mutexattr_destroy(&attr);

    mutex->initialized = (result == 0);
    return mutex->initialized;
}

void sad_mutex_destroy(SadMutex* mutex) {
    if (!mutex || !mutex->initialized) return;
    pthread_mutex_destroy(&mutex->handle);
    mutex->initialized = false;
}

bool sad_mutex_lock(SadMutex* mutex) {
    if (!mutex || !mutex->initialized) return false;
    return pthread_mutex_lock(&mutex->handle) == 0;
}

bool sad_mutex_try_lock(SadMutex* mutex) {
    if (!mutex || !mutex->initialized) return false;
    return pthread_mutex_trylock(&mutex->handle) == 0;
}

bool sad_mutex_unlock(SadMutex* mutex) {
    if (!mutex || !mutex->initialized) return false;
    return pthread_mutex_unlock(&mutex->handle) == 0;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٢ — القناة (Channel)
 * ═══════════════════════════════════════════════════════════════════════════ */

SadChannel* sad_channel_new(size_t capacity) {
    if (capacity == 0) capacity = SAD_CHANNEL_DEFAULT_CAPACITY;

    SadChannel* ch = (SadChannel*)sad_rc_alloc(sizeof(SadChannel),
                                                SAD_OBJ_NONE, NULL);
    if (!ch) return NULL;

    ch->buffer = (SadValue*)sad_alloc(capacity * sizeof(SadValue));
    if (!ch->buffer) {
        sad_release(ch);
        return NULL;
    }

    memset(ch->buffer, 0, capacity * sizeof(SadValue));
    ch->capacity = capacity;
    ch->count = 0;
    ch->head = 0;
    ch->tail = 0;
    ch->closed = false;

    pthread_mutex_init(&ch->mutex, NULL);
    pthread_cond_init(&ch->cond_not_full, NULL);
    pthread_cond_init(&ch->cond_not_empty, NULL);

    return ch;
}

bool sad_channel_send(SadChannel* ch, SadValue value) {
    if (!ch) return false;

    pthread_mutex_lock(&ch->mutex);

    /* انتظر حتى يتوفر مكان أو تُغلق القناة */
    while (ch->count >= ch->capacity && !ch->closed) {
        /*
         * الخيط "ينام" هنا حتى يأتي إشعار بأن القناة
         * لم تعد ممتلئة (بعد أن يستلم خيط آخر عنصراً).
         */
        pthread_cond_wait(&ch->cond_not_full, &ch->mutex);
    }

    if (ch->closed) {
        pthread_mutex_unlock(&ch->mutex);
        return false;
    }

    /* أضف القيمة في نهاية المصفوفة الدائرية */
    ch->buffer[ch->tail] = sad_value_copy(&value);
    ch->tail = (ch->tail + 1) % ch->capacity;
    ch->count++;

    /* أنبّه خيطاً منتظراً للاستلام */
    pthread_cond_signal(&ch->cond_not_empty);

    pthread_mutex_unlock(&ch->mutex);
    return true;
}

bool sad_channel_receive(SadChannel* ch, SadValue* out_value) {
    if (!ch) return false;

    pthread_mutex_lock(&ch->mutex);

    /* انتظر حتى يتوفر عنصر أو تُغلق القناة */
    while (ch->count == 0 && !ch->closed) {
        pthread_cond_wait(&ch->cond_not_empty, &ch->mutex);
    }

    if (ch->count == 0) {
        /* القناة مغلقة وفارغة */
        pthread_mutex_unlock(&ch->mutex);
        return false;
    }

    /* اقرأ القيمة من رأس المصفوفة الدائرية */
    if (out_value) {
        *out_value = ch->buffer[ch->head];
    } else {
        sad_value_free(&ch->buffer[ch->head]);
    }

    memset(&ch->buffer[ch->head], 0, sizeof(SadValue));
    ch->head = (ch->head + 1) % ch->capacity;
    ch->count--;

    /* أنبّه خيطاً منتظراً للإرسال */
    pthread_cond_signal(&ch->cond_not_full);

    pthread_mutex_unlock(&ch->mutex);
    return true;
}

bool sad_channel_try_send(SadChannel* ch, SadValue value) {
    if (!ch) return false;

    if (pthread_mutex_trylock(&ch->mutex) != 0) return false;

    if (ch->closed || ch->count >= ch->capacity) {
        pthread_mutex_unlock(&ch->mutex);
        return false;
    }

    ch->buffer[ch->tail] = sad_value_copy(&value);
    ch->tail = (ch->tail + 1) % ch->capacity;
    ch->count++;

    pthread_cond_signal(&ch->cond_not_empty);
    pthread_mutex_unlock(&ch->mutex);
    return true;
}

bool sad_channel_try_receive(SadChannel* ch, SadValue* out_value) {
    if (!ch) return false;

    if (pthread_mutex_trylock(&ch->mutex) != 0) return false;

    if (ch->count == 0) {
        pthread_mutex_unlock(&ch->mutex);
        return false;
    }

    if (out_value) {
        *out_value = ch->buffer[ch->head];
    } else {
        sad_value_free(&ch->buffer[ch->head]);
    }

    memset(&ch->buffer[ch->head], 0, sizeof(SadValue));
    ch->head = (ch->head + 1) % ch->capacity;
    ch->count--;

    pthread_cond_signal(&ch->cond_not_full);
    pthread_mutex_unlock(&ch->mutex);
    return true;
}

void sad_channel_close(SadChannel* ch) {
    if (!ch) return;

    pthread_mutex_lock(&ch->mutex);
    ch->closed = true;

    /* أنبّه كل الخيوط المنتظرة — حتى تتحقق من الإغلاق */
    pthread_cond_broadcast(&ch->cond_not_full);
    pthread_cond_broadcast(&ch->cond_not_empty);

    pthread_mutex_unlock(&ch->mutex);
}

void sad_channel_free(SadChannel* ch) {
    if (!ch) return;

    /* أغلق أولاً */
    sad_channel_close(ch);

    pthread_mutex_lock(&ch->mutex);

    /* حرّر كل القيم المتبقية */
    while (ch->count > 0) {
        sad_value_free(&ch->buffer[ch->head]);
        ch->head = (ch->head + 1) % ch->capacity;
        ch->count--;
    }

    if (ch->buffer) {
        sad_free(ch->buffer);
        ch->buffer = NULL;
    }

    pthread_mutex_unlock(&ch->mutex);

    pthread_mutex_destroy(&ch->mutex);
    pthread_cond_destroy(&ch->cond_not_full);
    pthread_cond_destroy(&ch->cond_not_empty);

    sad_release(ch);
}

size_t sad_channel_count(const SadChannel* ch) {
    return ch ? ch->count : 0;
}

bool sad_channel_is_closed(const SadChannel* ch) {
    return !ch || ch->closed;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٣ — الخيوط (Thread)
 * ═══════════════════════════════════════════════════════════════════════════ */

SadThread* sad_thread_new(const char* name, SadThreadFunc func, void* arg) {
    if (!func) return NULL;

    SadThread* thread = (SadThread*)sad_rc_alloc(sizeof(SadThread),
                                                   SAD_OBJ_NONE, NULL);
    if (!thread) return NULL;

    thread->name = sad_string_from_cstr(name ? name : "خيط");
    thread->running = false;
    thread->detached = false;

    /* إعداد خصائص الخيط */
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, SAD_THREAD_STACK_SIZE);

    int result = pthread_create(&thread->handle, &attr, func, arg);
    pthread_attr_destroy(&attr);

    if (result != 0) {
        sad_string_free(&thread->name);
        sad_release(thread);
        return NULL;
    }

    thread->running = true;
    return thread;
}

bool sad_thread_join(SadThread* thread, void** out_result) {
    if (!thread || !thread->running || thread->detached) return false;

    /*
     * انتظار الخيط — يوقف الخيط الحالي حتى ينتهي الهدف.
     * بعد الانضمام، لا يمكن إعادة الانضمام.
     */
    int result = pthread_join(thread->handle, out_result);
    if (result == 0) {
        thread->running = false;
        return true;
    }
    return false;
}

bool sad_thread_detach(SadThread* thread) {
    if (!thread || !thread->running || thread->detached) return false;

    /*
     * فصل الخيط — يعمل بشكل مستقل.
     * لن يُنتظر ولن يُحرّر يدوياً.
     * الموارد تُحرّر تلقائياً عند الانتهاء.
     */
    int result = pthread_detach(thread->handle);
    if (result == 0) {
        thread->detached = true;
        return true;
    }
    return false;
}

void sad_thread_free(SadThread* thread) {
    if (!thread) return;

    if (thread->running && !thread->detached) {
        /* لا يجب تحرير خيط يعمل — انتظره أولاً */
        sad_thread_join(thread, NULL);
    }

    sad_string_free(&thread->name);
    sad_release(thread);
}

bool sad_thread_is_running(const SadThread* thread) {
    return thread && thread->running;
}

void sad_thread_sleep(uint32_t milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

void sad_thread_yield(void) {
    sched_yield();
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٤ — مجمع الخيوط (Thread Pool)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * كل خيط في المجمع ينفّذ حلقة لا نهائية:
 *  ١. أقفل الطابور
 *  ٢. إذا لا مهام: انتظر إشعار
 *  ٣. اسحب مهمة من الطابور
 *  ٤. افتح القفل
 *  ٥. نفّذ المهمة
 *  ٦. ارجع للخطوة ١
 */

/** @brief (AR) الحلقة الرئيسية لخيط المجمع */
static void* thread_pool_worker(void* arg) {
    SadThreadPool* pool = (SadThreadPool*)arg;

    while (true) {
        pthread_mutex_lock(&pool->mutex);

        /* انتظر مهمة جديدة أو إشارة إيقاف */
        while (pool->task_count == 0 && !pool->shutdown) {
            pthread_cond_wait(&pool->cond_task, &pool->mutex);
        }

        if (pool->shutdown && pool->task_count == 0) {
            pthread_mutex_unlock(&pool->mutex);
            break; /* إيقاف */
        }

        /* اسحب مهمة */
        SadTask* task = pool->task_head;
        if (task) {
            pool->task_head = task->next;
            if (!pool->task_head) {
                pool->task_tail = NULL;
            }
            pool->task_count--;
        }

        pthread_mutex_unlock(&pool->mutex);

        /* نفّذ المهمة (خارج القفل — لا نمنع خيوطاً أخرى) */
        if (task) {
            task->func(task->arg);
            sad_free(task);
        }
    }

    return NULL;
}

SadThreadPool* sad_thread_pool_new(size_t thread_count) {
    if (thread_count == 0) thread_count = SAD_THREAD_POOL_DEFAULT_SIZE;
    if (thread_count > SAD_THREAD_MAX_COUNT) thread_count = SAD_THREAD_MAX_COUNT;

    SadThreadPool* pool = (SadThreadPool*)sad_alloc(sizeof(SadThreadPool));
    if (!pool) return NULL;

    memset(pool, 0, sizeof(SadThreadPool));

    pool->threads = (pthread_t*)sad_alloc(thread_count * sizeof(pthread_t));
    if (!pool->threads) {
        sad_free(pool);
        return NULL;
    }

    pool->thread_count = thread_count;
    pool->task_head = NULL;
    pool->task_tail = NULL;
    pool->task_count = 0;
    pool->shutdown = false;

    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->cond_task, NULL);

    /* إطلاق الخيوط */
    for (size_t i = 0; i < thread_count; i++) {
        pthread_create(&pool->threads[i], NULL, thread_pool_worker, pool);
    }

    return pool;
}

bool sad_thread_pool_submit(SadThreadPool* pool, SadTaskFunc func, void* arg) {
    if (!pool || !func || pool->shutdown) return false;

    /* إنشاء مهمة */
    SadTask* task = (SadTask*)sad_alloc(sizeof(SadTask));
    if (!task) return false;

    task->func = func;
    task->arg = arg;
    task->next = NULL;

    /* إضافة للطابور */
    pthread_mutex_lock(&pool->mutex);

    if (pool->task_tail) {
        pool->task_tail->next = task;
    } else {
        pool->task_head = task;
    }
    pool->task_tail = task;
    pool->task_count++;

    /* أنبّه خيطاً منتظراً */
    pthread_cond_signal(&pool->cond_task);

    pthread_mutex_unlock(&pool->mutex);
    return true;
}

void sad_thread_pool_shutdown(SadThreadPool* pool) {
    if (!pool) return;

    pthread_mutex_lock(&pool->mutex);
    pool->shutdown = true;
    /* أنبّه كل الخيوط */
    pthread_cond_broadcast(&pool->cond_task);
    pthread_mutex_unlock(&pool->mutex);

    /* انتظر انتهاء كل الخيوط */
    for (size_t i = 0; i < pool->thread_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }
}

void sad_thread_pool_free(SadThreadPool* pool) {
    if (!pool) return;

    if (!pool->shutdown) {
        sad_thread_pool_shutdown(pool);
    }

    /* حرّر المهام المتبقية (إذا وُجدت) */
    SadTask* task = pool->task_head;
    while (task) {
        SadTask* next = task->next;
        sad_free(task);
        task = next;
    }

    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->cond_task);

    if (pool->threads) {
        sad_free(pool->threads);
    }

    sad_free(pool);
}
