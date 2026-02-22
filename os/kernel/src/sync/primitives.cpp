// ===================================================================
// أوليات التزامن - صدOS
// primitives.cpp - Synchronization Primitives
// قفل متبادل، إشارة، قفل دوران، قفل قراءة/كتابة، متغير شرطي
// المرحلة 52 - مشروع لغة صاد
// ===================================================================

#include <cstdint>
#include <cstddef>
#include <array>

namespace sad::os::kernel::sync {

// === ثوابت التزامن ===
static constexpr uint32_t MAX_WAITERS      = 256;    // الحد الأقصى للمنتظرين
static constexpr uint32_t SPIN_LIMIT       = 1000;   // حد الدوران قبل الحجب
static constexpr uint32_t INVALID_THREAD   = UINT32_MAX;

// === عمليات ذرية مبسطة ===
namespace atomic {
    // مقارنة وتبديل ذري
    inline bool compare_and_swap(volatile uint32_t* ptr, uint32_t expected, uint32_t desired) {
        return __sync_bool_compare_and_swap(ptr, expected, desired);
    }
    // جلب وإضافة ذري  
    inline uint32_t fetch_add(volatile uint32_t* ptr, uint32_t val) {
        return __sync_fetch_and_add(ptr, val);
    }
    // جلب وطرح ذري
    inline uint32_t fetch_sub(volatile uint32_t* ptr, uint32_t val) {
        return __sync_fetch_and_sub(ptr, val);
    }
    // تحميل ذري
    inline uint32_t load(volatile uint32_t* ptr) {
        return __sync_add_and_fetch(ptr, 0);
    }
    // تخزين ذري
    inline void store(volatile uint32_t* ptr, uint32_t val) {
        __sync_lock_test_and_set(ptr, val);
    }
    // حاجز ذاكرة
    inline void memory_barrier() { __sync_synchronize(); }
}

// === قائمة انتظار الخيوط المحجوبة ===
struct WaitQueue {
    std::array<uint32_t, MAX_WAITERS> waiters;
    uint32_t head = 0;
    uint32_t tail = 0;
    uint32_t count = 0;

    // إضافة خيط للانتظار
    bool enqueue(uint32_t tid) {
        if (count >= MAX_WAITERS) return false;
        waiters[tail] = tid;
        tail = (tail + 1) % MAX_WAITERS;
        count++;
        return true;
    }

    // إيقاظ خيط واحد
    uint32_t dequeue() {
        if (count == 0) return INVALID_THREAD;
        uint32_t tid = waiters[head];
        head = (head + 1) % MAX_WAITERS;
        count--;
        return tid;
    }

    bool is_empty() const { return count == 0; }
};

// === القفل الدوراني (SpinLock) ===
// قفل خفيف يدور في حلقة حتى يتحرر - مناسب للأقسام الحرجة القصيرة
class SpinLock {
public:
    void lock() {
        while (!atomic::compare_and_swap(&locked_, 0, 1)) {
            // انتظار نشط مع تلميح للمعالج
            cpu_relax();
        }
        atomic::memory_barrier();
    }

    bool try_lock() {
        return atomic::compare_and_swap(&locked_, 0, 1);
    }

    void unlock() {
        atomic::memory_barrier();
        atomic::store(&locked_, 0);
    }

    bool is_locked() const { return atomic::load(const_cast<volatile uint32_t*>(&locked_)); }

private:
    volatile uint32_t locked_ = 0;
    void cpu_relax() { /* __asm__ volatile("pause") */ }
};

// === القفل المتبادل (Mutex) ===
// قفل يحجب الخيط إذا كان مغلقاً - مناسب للأقسام الحرجة الطويلة
class Mutex {
public:
    void lock(uint32_t caller_tid) {
        // محاولة الدوران أولاً (تحسين الأداء)
        for (uint32_t i = 0; i < SPIN_LIMIT; i++) {
            if (try_lock(caller_tid)) return;
        }
        // الحجب إذا لم ينجح الدوران
        guard_.lock();
        if (atomic::compare_and_swap(&locked_, 0, 1)) {
            owner_ = caller_tid;
            guard_.unlock();
            return;
        }
        wait_queue_.enqueue(caller_tid);
        guard_.unlock();
        // حجب الخيط (يُستأنف عند فتح القفل)
        block_current_thread(caller_tid);
    }

    bool try_lock(uint32_t caller_tid) {
        if (atomic::compare_and_swap(&locked_, 0, 1)) {
            owner_ = caller_tid;
            return true;
        }
        return false;
    }

    void unlock() {
        guard_.lock();
        owner_ = INVALID_THREAD;
        atomic::store(&locked_, 0);
        // إيقاظ أول منتظر
        uint32_t next = wait_queue_.dequeue();
        guard_.unlock();
        if (next != INVALID_THREAD) {
            wake_thread(next);
        }
    }

    uint32_t owner() const { return owner_; }

private:
    volatile uint32_t locked_ = 0;
    uint32_t owner_ = INVALID_THREAD;
    SpinLock guard_;
    WaitQueue wait_queue_;

    void block_current_thread(uint32_t) { /* حجب عبر المجدول */ }
    void wake_thread(uint32_t) { /* إيقاظ عبر المجدول */ }
};

// === الإشارة (Semaphore) ===
// عداد للتحكم في الوصول المتزامن إلى عدد محدد من الموارد
class Semaphore {
public:
    explicit Semaphore(uint32_t initial_count = 1) : count_(initial_count) {}

    // انتظار (إنقاص العداد)
    void wait(uint32_t caller_tid) {
        guard_.lock();
        if (count_ > 0) {
            count_--;
            guard_.unlock();
            return;
        }
        wait_queue_.enqueue(caller_tid);
        guard_.unlock();
        block_current_thread(caller_tid);
    }

    // إشارة (زيادة العداد)
    void signal() {
        guard_.lock();
        uint32_t next = wait_queue_.dequeue();
        if (next != INVALID_THREAD) {
            guard_.unlock();
            wake_thread(next);
        } else {
            count_++;
            guard_.unlock();
        }
    }

    uint32_t count() const { return count_; }

private:
    uint32_t count_;
    SpinLock guard_;
    WaitQueue wait_queue_;

    void block_current_thread(uint32_t) {}
    void wake_thread(uint32_t) {}
};

// === قفل القراءة/الكتابة (RWLock) ===
// يسمح بقراءة متزامنة متعددة لكن كتابة حصرية واحدة فقط
class RWLock {
public:
    // قفل للقراءة
    void read_lock(uint32_t tid) {
        guard_.lock();
        if (writer_ == INVALID_THREAD && write_waiters_.is_empty()) {
            readers_++;
            guard_.unlock();
            return;
        }
        read_waiters_.enqueue(tid);
        guard_.unlock();
        block_current_thread(tid);
    }

    void read_unlock() {
        guard_.lock();
        readers_--;
        if (readers_ == 0) {
            // إيقاظ كاتب منتظر
            uint32_t w = write_waiters_.dequeue();
            if (w != INVALID_THREAD) {
                writer_ = w;
                guard_.unlock();
                wake_thread(w);
                return;
            }
        }
        guard_.unlock();
    }

    // قفل للكتابة
    void write_lock(uint32_t tid) {
        guard_.lock();
        if (readers_ == 0 && writer_ == INVALID_THREAD) {
            writer_ = tid;
            guard_.unlock();
            return;
        }
        write_waiters_.enqueue(tid);
        guard_.unlock();
        block_current_thread(tid);
    }

    void write_unlock() {
        guard_.lock();
        writer_ = INVALID_THREAD;
        // إيقاظ جميع القراء المنتظرين أولاً
        while (!read_waiters_.is_empty()) {
            uint32_t r = read_waiters_.dequeue();
            readers_++;
            wake_thread(r);
        }
        if (readers_ == 0) {
            uint32_t w = write_waiters_.dequeue();
            if (w != INVALID_THREAD) {
                writer_ = w;
                wake_thread(w);
            }
        }
        guard_.unlock();
    }

private:
    uint32_t readers_ = 0;
    uint32_t writer_ = INVALID_THREAD;
    SpinLock guard_;
    WaitQueue read_waiters_;
    WaitQueue write_waiters_;

    void block_current_thread(uint32_t) {}
    void wake_thread(uint32_t) {}
};

// === المتغير الشرطي (ConditionVariable) ===
// يسمح للخيوط بالانتظار حتى يتحقق شرط معين
class ConditionVariable {
public:
    // انتظار مع تحرير القفل المتبادل مؤقتاً
    void wait(Mutex& mtx, uint32_t caller_tid) {
        guard_.lock();
        wait_queue_.enqueue(caller_tid);
        guard_.unlock();
        mtx.unlock();                      // تحرير القفل أثناء الانتظار
        block_current_thread(caller_tid);
        mtx.lock(caller_tid);              // إعادة القفل بعد الاستيقاظ
    }

    // إيقاظ خيط واحد منتظر
    void notify_one() {
        guard_.lock();
        uint32_t tid = wait_queue_.dequeue();
        guard_.unlock();
        if (tid != INVALID_THREAD) {
            wake_thread(tid);
        }
    }

    // إيقاظ جميع الخيوط المنتظرة
    void notify_all() {
        guard_.lock();
        while (!wait_queue_.is_empty()) {
            uint32_t tid = wait_queue_.dequeue();
            wake_thread(tid);
        }
        guard_.unlock();
    }

private:
    SpinLock guard_;
    WaitQueue wait_queue_;

    void block_current_thread(uint32_t) {}
    void wake_thread(uint32_t) {}
};

} // namespace sad::os::kernel::sync
