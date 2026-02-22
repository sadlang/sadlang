/*
 * بسم الله الرحمن الرحيم
 * ================================
 * نظام إدارة الخيوط / Thread Management System
 * ================================
 * 
 * نظام متقدم لإدارة الخيوط والتزامن للغة Sad
 * Advanced threading and synchronization system for Sad language
 * 
 * الميزات / Features:
 * - Thread creation and management
 * - Thread pools for efficient execution
 * - Synchronization primitives (mutex, rwlock, semaphore)
 * - Condition variables
 * - Thread-local storage
 * - Lock-free atomic operations
 * 
 * @file runtime/thread/thread.h
 * @author SadLang Compiler Team
 * @date December 2025
 */

#ifndef SAD_RUNTIME_THREAD_H
#define SAD_RUNTIME_THREAD_H

#include <cstdint>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <queue>
#include <memory>
#include <chrono>

namespace Sad {
namespace Thread {

// ============================================================================
// Thread ID and Handle / معرف الخيط والمقبض
// ============================================================================

/**
 * معرف الخيط / Thread ID
 */
using ThreadId = uint64_t;

/**
 * مقبض الخيط / Thread handle
 */
using ThreadHandle = std::thread::native_handle_type;

// ============================================================================
// Thread States / حالات الخيط
// ============================================================================

/**
 * حالة الخيط / Thread state
 */
enum class ThreadState {
    Created,        // تم الإنشاء
    Running,        // قيد التشغيل
    Blocked,        // محجوب
    Sleeping,       // نائم
    Waiting,        // في انتظار
    Terminated      // منتهي
};

/**
 * أولوية الخيط / Thread priority
 */
enum class ThreadPriority {
    Lowest,         // الأدنى
    Low,            // منخفضة
    Normal,         // عادية
    High,           // عالية
    Highest         // الأعلى
};

// ============================================================================
// Thread Configuration / إعدادات الخيط
// ============================================================================

/**
 * إعدادات الخيط / Thread configuration
 */
struct ThreadConfig {
    size_t stack_size;          // حجم الـ stack
    ThreadPriority priority;    // الأولوية
    int cpu_affinity;           // ربط بمعالج محدد (-1 = any)
    bool detached;              // خيط منفصل؟
    const char* name;           // اسم الخيط
    
    /**
     * الإعدادات الافتراضية / Default configuration
     */
    static ThreadConfig default_config() {
        return ThreadConfig{
            1024 * 1024,            // stack_size: 1 MB
            ThreadPriority::Normal, // priority
            -1,                     // cpu_affinity: any
            false,                  // detached
            nullptr                 // name
        };
    }
};

// ============================================================================
// Thread Class / فئة الخيط
// ============================================================================

/**
 * خيط تنفيذ / Thread of execution
 */
class Thread {
public:
    /**
     * دالة الخيط / Thread function
     */
    using Function = std::function<void()>;
    
    /**
     * إنشاء خيط جديد / Create new thread
     * 
     * @param func دالة التنفيذ
     * @param config الإعدادات
     */
    Thread(Function func, const ThreadConfig* config = nullptr);
    
    /**
     * مُدمر / Destructor
     */
    ~Thread();
    
    // منع النسخ / Prevent copying
    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;
    
    // السماح بالنقل / Allow moving
    Thread(Thread&& other) noexcept;
    Thread& operator=(Thread&& other) noexcept;
    
    /**
     * بدء تشغيل الخيط / Start thread
     */
    bool start();
    
    /**
     * الانتظار حتى الانتهاء / Wait for completion
     */
    void join();
    
    /**
     * فصل الخيط / Detach thread
     */
    void detach();
    
    /**
     * طلب الإيقاف / Request stop
     */
    void request_stop();
    
    /**
     * التحقق من طلب الإيقاف / Check if stop requested
     */
    bool stop_requested() const { return stop_flag_.load(); }
    
    /**
     * الحصول على المعرف / Get ID
     */
    ThreadId get_id() const { return id_; }
    
    /**
     * الحصول على الحالة / Get state
     */
    ThreadState get_state() const { return state_.load(); }
    
    /**
     * الحصول على المقبض / Get handle
     */
    ThreadHandle get_handle() const;
    
    /**
     * التحقق من قابلية الانضمام / Check if joinable
     */
    bool joinable() const { return thread_ && thread_->joinable(); }
    
    /**
     * الحصول على الخيط الحالي / Get current thread ID
     */
    static ThreadId current_thread_id();
    
    /**
     * النوم لفترة / Sleep for duration
     */
    static void sleep_for(std::chrono::milliseconds duration);
    
    /**
     * التنازل عن المعالج / Yield processor
     */
    static void yield();
    
private:
    ThreadId id_;                           // المعرف
    std::unique_ptr<std::thread> thread_;   // الخيط الفعلي
    Function function_;                     // دالة التنفيذ
    ThreadConfig config_;                   // الإعدادات
    std::atomic<ThreadState> state_;        // الحالة
    std::atomic<bool> stop_flag_;           // علم الإيقاف
    
    /**
     * دالة التشغيل الداخلية / Internal run function
     */
    void run();
    
    /**
     * توليد معرف فريد / Generate unique ID
     */
    static ThreadId generate_id();
};

// ============================================================================
// Mutex / القفل المتبادل
// ============================================================================

/**
 * قفل متبادل للتزامن / Mutual exclusion lock
 */
class Mutex {
public:
    Mutex() = default;
    ~Mutex() = default;
    
    // منع النسخ والنقل / Prevent copying and moving
    Mutex(const Mutex&) = delete;
    Mutex& operator=(const Mutex&) = delete;
    Mutex(Mutex&&) = delete;
    Mutex& operator=(Mutex&&) = delete;
    
    /**
     * قفل / Lock
     */
    void lock() { mutex_.lock(); }
    
    /**
     * محاولة القفل / Try lock
     */
    bool try_lock() { return mutex_.try_lock(); }
    
    /**
     * فتح القفل / Unlock
     */
    void unlock() { mutex_.unlock(); }
    
    /**
     * الحصول على القفل الأساسي / Get native mutex
     */
    std::mutex& native() { return mutex_; }
    
private:
    std::mutex mutex_;
};

/**
 * قفل تلقائي / Automatic lock guard
 */
class LockGuard {
public:
    explicit LockGuard(Mutex& mutex) : mutex_(mutex) {
        mutex_.lock();
    }
    
    ~LockGuard() {
        mutex_.unlock();
    }
    
    // منع النسخ والنقل / Prevent copying and moving
    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;
    
private:
    Mutex& mutex_;
};

/**
 * قفل فريد / Unique lock
 */
class UniqueLock {
public:
    explicit UniqueLock(Mutex& mutex) 
        : mutex_(&mutex), owns_lock_(false) {
        lock();
    }
    
    ~UniqueLock() {
        if (owns_lock_) {
            unlock();
        }
    }
    
    // منع النسخ / Prevent copying
    UniqueLock(const UniqueLock&) = delete;
    UniqueLock& operator=(const UniqueLock&) = delete;
    
    // السماح بالنقل / Allow moving
    UniqueLock(UniqueLock&& other) noexcept
        : mutex_(other.mutex_), owns_lock_(other.owns_lock_) {
        other.mutex_ = nullptr;
        other.owns_lock_ = false;
    }
    
    void lock() {
        if (!owns_lock_) {
            mutex_->lock();
            owns_lock_ = true;
        }
    }
    
    bool try_lock() {
        if (!owns_lock_) {
            owns_lock_ = mutex_->try_lock();
        }
        return owns_lock_;
    }
    
    void unlock() {
        if (owns_lock_) {
            mutex_->unlock();
            owns_lock_ = false;
        }
    }
    
    bool owns_lock() const { return owns_lock_; }
    
    Mutex* mutex() const { return mutex_; }
    
private:
    Mutex* mutex_;
    bool owns_lock_;
};

// ============================================================================
// Read-Write Lock / قفل القراءة والكتابة
// ============================================================================

/**
 * قفل القراءة والكتابة / Read-write lock
 */
class RWLock {
public:
    RWLock() : readers_(0), writer_(false) {}
    ~RWLock() = default;
    
    // منع النسخ والنقل / Prevent copying and moving
    RWLock(const RWLock&) = delete;
    RWLock& operator=(const RWLock&) = delete;
    
    /**
     * قفل للقراءة / Lock for reading
     */
    void lock_read();
    
    /**
     * محاولة قفل القراءة / Try lock for reading
     */
    bool try_lock_read();
    
    /**
     * فتح قفل القراءة / Unlock reading
     */
    void unlock_read();
    
    /**
     * قفل للكتابة / Lock for writing
     */
    void lock_write();
    
    /**
     * محاولة قفل الكتابة / Try lock for writing
     */
    bool try_lock_write();
    
    /**
     * فتح قفل الكتابة / Unlock writing
     */
    void unlock_write();
    
private:
    std::mutex mutex_;
    std::condition_variable cond_;
    int readers_;
    bool writer_;
};

/**
 * قفل قراءة تلقائي / Automatic read lock
 */
class ReadLockGuard {
public:
    explicit ReadLockGuard(RWLock& lock) : lock_(lock) {
        lock_.lock_read();
    }
    
    ~ReadLockGuard() {
        lock_.unlock_read();
    }
    
private:
    RWLock& lock_;
};

/**
 * قفل كتابة تلقائي / Automatic write lock
 */
class WriteLockGuard {
public:
    explicit WriteLockGuard(RWLock& lock) : lock_(lock) {
        lock_.lock_write();
    }
    
    ~WriteLockGuard() {
        lock_.unlock_write();
    }
    
private:
    RWLock& lock_;
};

// ============================================================================
// Condition Variable / متغير الشرط
// ============================================================================

/**
 * متغير شرط للتزامن / Condition variable for synchronization
 */
class ConditionVariable {
public:
    ConditionVariable() = default;
    ~ConditionVariable() = default;
    
    // منع النسخ والنقل / Prevent copying and moving
    ConditionVariable(const ConditionVariable&) = delete;
    ConditionVariable& operator=(const ConditionVariable&) = delete;
    
    /**
     * الانتظار / Wait
     */
    void wait(UniqueLock& lock);
    
    /**
     * الانتظار مع شرط / Wait with predicate
     */
    template<typename Predicate>
    void wait(UniqueLock& lock, Predicate pred) {
        while (!pred()) {
            wait(lock);
        }
    }
    
    /**
     * الانتظار لفترة / Wait for duration
     */
    template<typename Rep, typename Period>
    bool wait_for(UniqueLock& lock, 
                  const std::chrono::duration<Rep, Period>& duration) {
        return cond_.wait_for(
            std::unique_lock<std::mutex>(lock.mutex()->native()), 
            duration
        ) == std::cv_status::no_timeout;
    }
    
    /**
     * إيقاظ خيط واحد / Notify one thread
     */
    void notify_one() { cond_.notify_one(); }
    
    /**
     * إيقاظ جميع الخيوط / Notify all threads
     */
    void notify_all() { cond_.notify_all(); }
    
private:
    std::condition_variable cond_;
};

// ============================================================================
// Semaphore / العداد الثنائي
// ============================================================================

/**
 * عداد ثنائي / Semaphore
 */
class Semaphore {
public:
    explicit Semaphore(int initial_count = 0)
        : count_(initial_count) {}
    
    ~Semaphore() = default;
    
    // منع النسخ والنقل / Prevent copying and moving
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    
    /**
     * انتظار (تقليل العداد) / Wait (decrement)
     */
    void wait();
    
    /**
     * محاولة الانتظار / Try wait
     */
    bool try_wait();
    
    /**
     * إشارة (زيادة العداد) / Signal (increment)
     */
    void signal();
    
    /**
     * الحصول على العداد / Get count
     */
    int get_count() const { return count_.load(); }
    
private:
    std::atomic<int> count_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

// ============================================================================
// Thread Pool / تجمع الخيوط
// ============================================================================

/**
 * تجمع خيوط للتنفيذ الفعال / Thread pool for efficient execution
 */
class ThreadPool {
public:
    /**
     * مهمة / Task
     */
    using Task = std::function<void()>;
    
    /**
     * إنشاء تجمع خيوط / Create thread pool
     * 
     * @param num_threads عدد الخيوط
     */
    explicit ThreadPool(size_t num_threads = std::thread::hardware_concurrency());
    
    /**
     * مُدمر / Destructor
     */
    ~ThreadPool();
    
    // منع النسخ والنقل / Prevent copying and moving
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    
    /**
     * إضافة مهمة / Submit task
     */
    template<typename F>
    void submit(F&& task) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            tasks_.emplace(std::forward<F>(task));
        }
        cond_.notify_one();
    }
    
    /**
     * انتظار جميع المهام / Wait for all tasks
     */
    void wait_all();
    
    /**
     * إيقاف التجمع / Shutdown pool
     */
    void shutdown();
    
    /**
     * الحصول على عدد الخيوط / Get thread count
     */
    size_t get_thread_count() const { return workers_.size(); }
    
    /**
     * الحصول على عدد المهام المعلقة / Get pending tasks
     */
    size_t get_pending_tasks() const;
    
private:
    std::vector<std::thread> workers_;      // الخيوط العاملة
    std::queue<Task> tasks_;                // قائمة المهام
    mutable std::mutex mutex_;              // قفل الحماية
    std::condition_variable cond_;          // متغير الشرط
    std::atomic<bool> stop_;                // علم الإيقاف
    std::atomic<size_t> active_tasks_;      // المهام النشطة
    
    /**
     * دالة العامل / Worker function
     */
    void worker();
};

// ============================================================================
// Thread-Local Storage / التخزين المحلي للخيط
// ============================================================================

/**
 * تخزين محلي للخيط / Thread-local storage
 */
template<typename T>
class ThreadLocal {
public:
    ThreadLocal() = default;
    
    /**
     * الحصول على القيمة / Get value
     */
    T& get() {
        return value_;
    }
    
    /**
     * تعيين القيمة / Set value
     */
    void set(const T& value) {
        value_ = value;
    }
    
private:
    static thread_local T value_;
};

template<typename T>
thread_local T ThreadLocal<T>::value_{};

// ============================================================================
// Atomic Operations / العمليات الذرية
// ============================================================================

/**
 * عمليات ذرية / Atomic operations
 */
template<typename T>
class Atomic {
public:
    Atomic(T value = T{}) : value_(value) {}
    
    /**
     * قراءة / Load
     */
    T load() const {
        return value_.load(std::memory_order_acquire);
    }
    
    /**
     * كتابة / Store
     */
    void store(T value) {
        value_.store(value, std::memory_order_release);
    }
    
    /**
     * تبادل / Exchange
     */
    T exchange(T value) {
        return value_.exchange(value, std::memory_order_acq_rel);
    }
    
    /**
     * مقارنة وتبادل / Compare and swap
     */
    bool compare_exchange(T& expected, T desired) {
        return value_.compare_exchange_strong(
            expected, desired,
            std::memory_order_acq_rel
        );
    }
    
    /**
     * إضافة / Add
     */
    T fetch_add(T arg) {
        return value_.fetch_add(arg, std::memory_order_acq_rel);
    }
    
    /**
     * طرح / Subtract
     */
    T fetch_sub(T arg) {
        return value_.fetch_sub(arg, std::memory_order_acq_rel);
    }
    
private:
    std::atomic<T> value_;
};

} // namespace Thread
} // namespace Sad

#endif // SAD_RUNTIME_THREAD_H
