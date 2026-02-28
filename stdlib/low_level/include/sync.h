/*
 * ============================================================================
 * وحدة التزامن - لغة ص
 * Synchronization Primitives Module - Sad Language
 * ============================================================================
 *
 * @brief (AR) أدوات التزامن لنظام التشغيل: أقفال دوران، أقفال تبادلية،
 *              عمليات ذرية، متغيرات شرطية، حواجز
 *
 * @brief (EN) OS synchronization tools: spinlocks, mutexes,
 *              atomic operations, condition variables, barriers
 * ============================================================================
 */

#ifndef SAD_LOW_LEVEL_SYNC_H
#define SAD_LOW_LEVEL_SYNC_H

#include <cstdint>
#include <cstddef>
#include <string>

namespace Sad {
namespace LowLevel {

// ============================================================================
// عمليات ذرية / Atomic Operations
// ============================================================================

/**
 * @brief (AR) عمليات ذرية منخفضة المستوى
 * @brief (EN) Low-level atomic operations
 */
class Atomic {
public:
    /// @brief (AR) تبادل ذري / (EN) Atomic exchange
    static uint64_t exchange(volatile uint64_t* target, uint64_t value);

    /// @brief (AR) مقارنة وتبادل / (EN) Compare and swap
    static bool compareAndSwap(volatile uint64_t* target, uint64_t expected, uint64_t desired);

    /// @brief (AR) جمع ذري / (EN) Atomic add
    static uint64_t fetchAdd(volatile uint64_t* target, uint64_t value);

    /// @brief (AR) طرح ذري / (EN) Atomic subtract
    static uint64_t fetchSub(volatile uint64_t* target, uint64_t value);

    /// @brief (AR) AND ذري / (EN) Atomic AND
    static uint64_t fetchAnd(volatile uint64_t* target, uint64_t mask);

    /// @brief (AR) OR ذري / (EN) Atomic OR
    static uint64_t fetchOr(volatile uint64_t* target, uint64_t mask);

    /// @brief (AR) XOR ذري / (EN) Atomic XOR
    static uint64_t fetchXor(volatile uint64_t* target, uint64_t mask);

    /// @brief (AR) قراءة ذرية / (EN) Atomic load
    static uint64_t load(const volatile uint64_t* target);

    /// @brief (AR) كتابة ذرية / (EN) Atomic store
    static void store(volatile uint64_t* target, uint64_t value);

    /// @brief (AR) حاجز ذاكرة كامل / (EN) Full memory barrier/fence
    static void memoryBarrier();

    /// @brief (AR) حاجز قراءة / (EN) Read barrier
    static void readBarrier();

    /// @brief (AR) حاجز كتابة / (EN) Write barrier
    static void writeBarrier();

    /// @brief (AR) إشارة توقف للمعالج (في حلقات انتظار) / (EN) CPU pause hint (for spin loops)
    static void pause();

    // نسخ 32-بت / 32-bit versions
    static uint32_t exchange32(volatile uint32_t* target, uint32_t value);
    static bool compareAndSwap32(volatile uint32_t* target, uint32_t expected, uint32_t desired);
    static uint32_t fetchAdd32(volatile uint32_t* target, uint32_t value);
    static uint32_t fetchSub32(volatile uint32_t* target, uint32_t value);
};

// ============================================================================
// قفل الدوران / Spinlock
// ============================================================================

/**
 * @brief (AR) قفل دوران بسيط - مناسب للأقسام الحرجة القصيرة
 *              يعطل المقاطعات لمنع الجمود في سياق النواة
 *
 * @brief (EN) Simple spinlock - suitable for short critical sections
 *              Disables interrupts to prevent deadlock in kernel context
 */
class Spinlock {
public:
    Spinlock();

    /// @brief (AR) الحصول على القفل / (EN) Acquire lock
    void lock();

    /// @brief (AR) محاولة الحصول على القفل / (EN) Try to acquire lock
    bool tryLock();

    /// @brief (AR) تحرير القفل / (EN) Release lock
    void unlock();

    /// @brief (AR) هل القفل مغلق / (EN) Is locked
    bool isLocked() const;

private:
    volatile uint64_t lock_;
    volatile uint64_t savedFlags_;  // أعلام المقاطعات المحفوظة / Saved interrupt flags
};

/// @brief (AR) حارس RAII لقفل الدوران / (EN) RAII guard for spinlock
class SpinlockGuard {
public:
    explicit SpinlockGuard(Spinlock& lock) : lock_(lock) { lock_.lock(); }
    ~SpinlockGuard() { lock_.unlock(); }

    SpinlockGuard(const SpinlockGuard&) = delete;
    SpinlockGuard& operator=(const SpinlockGuard&) = delete;

private:
    Spinlock& lock_;
};

// ============================================================================
// قفل القراءة والكتابة / Read-Write Lock
// ============================================================================

/**
 * @brief (AR) قفل قراءة-كتابة: يسمح بقراءات متعددة متزامنة
 *              ولكن كتابة واحدة حصرية
 *
 * @brief (EN) Read-write lock: allows multiple concurrent readers
 *              but exclusive write access
 */
class RWLock {
public:
    RWLock();

    /// @brief (AR) الحصول على قفل القراءة / (EN) Acquire read lock
    void readLock();

    /// @brief (AR) تحرير قفل القراءة / (EN) Release read lock
    void readUnlock();

    /// @brief (AR) الحصول على قفل الكتابة / (EN) Acquire write lock
    void writeLock();

    /// @brief (AR) تحرير قفل الكتابة / (EN) Release write lock
    void writeUnlock();

    /// @brief (AR) عدد القراء الحاليين / (EN) Current reader count
    uint32_t getReaderCount() const;

private:
    Spinlock lock_;             // قفل داخلي / Internal lock
    volatile int64_t readers_;  // عدد القراء / Reader count (-1 = writer holds lock)
};

/// @brief (AR) حارس RAII للقراءة / (EN) RAII read guard
class ReadGuard {
public:
    explicit ReadGuard(RWLock& rwlock) : rwlock_(rwlock) { rwlock_.readLock(); }
    ~ReadGuard() { rwlock_.readUnlock(); }
    ReadGuard(const ReadGuard&) = delete;
    ReadGuard& operator=(const ReadGuard&) = delete;
private:
    RWLock& rwlock_;
};

/// @brief (AR) حارس RAII للكتابة / (EN) RAII write guard
class WriteGuard {
public:
    explicit WriteGuard(RWLock& rwlock) : rwlock_(rwlock) { rwlock_.writeLock(); }
    ~WriteGuard() { rwlock_.writeUnlock(); }
    WriteGuard(const WriteGuard&) = delete;
    WriteGuard& operator=(const WriteGuard&) = delete;
private:
    RWLock& rwlock_;
};

// ============================================================================
// القفل التبادلي / Mutex
// ============================================================================

/**
 * @brief (AR) قفل تبادلي مع دعم النوم - للأقسام الحرجة الطويلة
 *              يستخدم قائمة انتظار للخيوط المحجوبة
 *
 * @brief (EN) Mutex with sleep support - for longer critical sections
 *              Uses a wait queue for blocked threads
 */
class Mutex {
public:
    Mutex();

    /// @brief (AR) الحصول على القفل / (EN) Acquire lock
    void lock();

    /// @brief (AR) محاولة الحصول / (EN) Try to acquire
    bool tryLock();

    /// @brief (AR) تحرير القفل / (EN) Release lock
    void unlock();

    /// @brief (AR) هل مغلق / (EN) Is locked
    bool isLocked() const;

    /// @brief (AR) معرف الخيط المالك / (EN) Owner thread ID
    uint64_t getOwner() const { return owner_; }

private:
    Spinlock spinlock_;         // حماية الحالة الداخلية / Protects internal state
    volatile uint64_t locked_;  // هل مغلق / Is locked
    volatile uint64_t owner_;   // معرف الخيط المالك / Owner thread ID
    volatile uint32_t waiters_; // عدد المنتظرين / Waiter count
};

/// @brief (AR) حارس RAII للقفل التبادلي / (EN) RAII mutex guard
class MutexGuard {
public:
    explicit MutexGuard(Mutex& mutex) : mutex_(mutex) { mutex_.lock(); }
    ~MutexGuard() { mutex_.unlock(); }
    MutexGuard(const MutexGuard&) = delete;
    MutexGuard& operator=(const MutexGuard&) = delete;
private:
    Mutex& mutex_;
};

// ============================================================================
// السيمافور / Semaphore
// ============================================================================

/**
 * @brief (AR) سيمافور (عداد) - للتحكم بالوصول إلى عدد محدود من الموارد
 * @brief (EN) Counting semaphore - controls access to limited resources
 */
class Semaphore {
public:
    /// @brief (AR) إنشاء بعدد أولي / (EN) Create with initial count
    explicit Semaphore(uint32_t initialCount);

    /// @brief (AR) انتظار (P / down) / (EN) Wait (P / down)
    void wait();

    /// @brief (AR) محاولة انتظار / (EN) Try wait (non-blocking)
    bool tryWait();

    /// @brief (AR) إشارة (V / up) / (EN) Signal (V / up)
    void signal();

    /// @brief (AR) القيمة الحالية / (EN) Current value
    uint32_t getValue() const;

private:
    Spinlock lock_;
    volatile uint32_t count_;
    volatile uint32_t maxCount_;
};

// ============================================================================
// الحاجز / Barrier
// ============================================================================

/**
 * @brief (AR) حاجز - تزامن عدة خيوط عند نقطة واحدة
 * @brief (EN) Barrier - synchronize multiple threads at a single point
 */
class Barrier {
public:
    /// @brief (AR) إنشاء بعدد الخيوط / (EN) Create with thread count
    explicit Barrier(uint32_t threadCount);

    /// @brief (AR) انتظار حتى يصل الجميع / (EN) Wait until all threads arrive
    void wait();

    /// @brief (AR) إعادة ضبط / (EN) Reset
    void reset();

private:
    Spinlock lock_;
    volatile uint32_t threshold_;   // العدد المطلوب / Required count
    volatile uint32_t count_;       // العدد الحالي / Current count
    volatile uint64_t generation_;  // الجيل (لإعادة الاستخدام) / Generation
};

// ============================================================================
// القفل التكراري / Recursive Lock
// ============================================================================

/**
 * @brief (AR) قفل يمكن لنفس الخيط قفله عدة مرات
 * @brief (EN) Lock that can be acquired multiple times by the same thread
 */
class RecursiveLock {
public:
    RecursiveLock();

    void lock();
    bool tryLock();
    void unlock();
    uint32_t getLockCount() const { return lockCount_; }

private:
    Spinlock spinlock_;
    volatile uint64_t owner_;
    volatile uint32_t lockCount_;
};

// ============================================================================
// مرة واحدة / Once
// ============================================================================

/**
 * @brief (AR) تنفيذ دالة مرة واحدة فقط (thread-safe)
 * @brief (EN) Execute a function exactly once (thread-safe)
 */
class Once {
public:
    Once();

    /// @brief (AR) تنفيذ الدالة مرة واحدة / (EN) Call function once
    void callOnce(void (*func)());

    /// @brief (AR) هل تم التنفيذ / (EN) Has been called
    bool isDone() const { return done_; }

private:
    Spinlock lock_;
    volatile bool done_;
};

} // namespace LowLevel
} // namespace Sad

#endif // SAD_LOW_LEVEL_SYNC_H
