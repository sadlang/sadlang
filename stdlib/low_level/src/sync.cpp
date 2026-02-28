/*
 * ============================================================================
 * تنفيذ التزامن - لغة ص
 * Synchronization Implementation - Sad Language
 * ============================================================================
 */

#include "sync.h"

#ifdef _MSC_VER
#include <intrin.h>
#pragma intrinsic(_InterlockedExchange64, _InterlockedCompareExchange64)
#pragma intrinsic(_InterlockedExchangeAdd64, _InterlockedAnd64)
#pragma intrinsic(_InterlockedOr64, _InterlockedXor64)
#pragma intrinsic(_InterlockedExchange, _InterlockedCompareExchange)
#pragma intrinsic(_InterlockedExchangeAdd, _mm_pause, _ReadWriteBarrier)
#endif

namespace Sad {
namespace LowLevel {

// ============================================================================
// عمليات ذرية / Atomic Operations
// ============================================================================

uint64_t Atomic::exchange(volatile uint64_t* target, uint64_t value) {
#ifdef _MSC_VER
    return static_cast<uint64_t>(_InterlockedExchange64(
        reinterpret_cast<volatile long long*>(target), static_cast<long long>(value)));
#else
    return __atomic_exchange_n(target, value, __ATOMIC_SEQ_CST);
#endif
}

bool Atomic::compareAndSwap(volatile uint64_t* target, uint64_t expected, uint64_t desired) {
#ifdef _MSC_VER
    return _InterlockedCompareExchange64(
        reinterpret_cast<volatile long long*>(target),
        static_cast<long long>(desired),
        static_cast<long long>(expected)) == static_cast<long long>(expected);
#else
    return __atomic_compare_exchange_n(target, &expected, desired,
                                        false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
#endif
}

uint64_t Atomic::fetchAdd(volatile uint64_t* target, uint64_t value) {
#ifdef _MSC_VER
    return static_cast<uint64_t>(_InterlockedExchangeAdd64(
        reinterpret_cast<volatile long long*>(target), static_cast<long long>(value)));
#else
    return __atomic_fetch_add(target, value, __ATOMIC_SEQ_CST);
#endif
}

uint64_t Atomic::fetchSub(volatile uint64_t* target, uint64_t value) {
#ifdef _MSC_VER
    return static_cast<uint64_t>(_InterlockedExchangeAdd64(
        reinterpret_cast<volatile long long*>(target), -static_cast<long long>(value)));
#else
    return __atomic_fetch_sub(target, value, __ATOMIC_SEQ_CST);
#endif
}

uint64_t Atomic::fetchAnd(volatile uint64_t* target, uint64_t mask) {
#ifdef _MSC_VER
    return static_cast<uint64_t>(_InterlockedAnd64(
        reinterpret_cast<volatile long long*>(target), static_cast<long long>(mask)));
#else
    return __atomic_fetch_and(target, mask, __ATOMIC_SEQ_CST);
#endif
}

uint64_t Atomic::fetchOr(volatile uint64_t* target, uint64_t mask) {
#ifdef _MSC_VER
    return static_cast<uint64_t>(_InterlockedOr64(
        reinterpret_cast<volatile long long*>(target), static_cast<long long>(mask)));
#else
    return __atomic_fetch_or(target, mask, __ATOMIC_SEQ_CST);
#endif
}

uint64_t Atomic::fetchXor(volatile uint64_t* target, uint64_t mask) {
#ifdef _MSC_VER
    return static_cast<uint64_t>(_InterlockedXor64(
        reinterpret_cast<volatile long long*>(target), static_cast<long long>(mask)));
#else
    return __atomic_fetch_xor(target, mask, __ATOMIC_SEQ_CST);
#endif
}

uint64_t Atomic::load(const volatile uint64_t* target) {
#ifdef _MSC_VER
    uint64_t value = *target;
    _ReadWriteBarrier();
    return value;
#else
    return __atomic_load_n(target, __ATOMIC_SEQ_CST);
#endif
}

void Atomic::store(volatile uint64_t* target, uint64_t value) {
#ifdef _MSC_VER
    _ReadWriteBarrier();
    *target = value;
    _ReadWriteBarrier();
#else
    __atomic_store_n(target, value, __ATOMIC_SEQ_CST);
#endif
}

void Atomic::memoryBarrier() {
#ifdef _MSC_VER
    _ReadWriteBarrier();
    __faststorefence();
#else
    __asm__ volatile("mfence" ::: "memory");
#endif
}

void Atomic::readBarrier() {
#ifdef _MSC_VER
    _ReadWriteBarrier();
#else
    __asm__ volatile("lfence" ::: "memory");
#endif
}

void Atomic::writeBarrier() {
#ifdef _MSC_VER
    _ReadWriteBarrier();
#else
    __asm__ volatile("sfence" ::: "memory");
#endif
}

void Atomic::pause() {
#ifdef _MSC_VER
    _mm_pause();
#else
    __asm__ volatile("pause" ::: "memory");
#endif
}

uint32_t Atomic::exchange32(volatile uint32_t* target, uint32_t value) {
#ifdef _MSC_VER
    return static_cast<uint32_t>(_InterlockedExchange(
        reinterpret_cast<volatile long*>(target), static_cast<long>(value)));
#else
    return __atomic_exchange_n(target, value, __ATOMIC_SEQ_CST);
#endif
}

bool Atomic::compareAndSwap32(volatile uint32_t* target, uint32_t expected, uint32_t desired) {
#ifdef _MSC_VER
    return _InterlockedCompareExchange(
        reinterpret_cast<volatile long*>(target),
        static_cast<long>(desired),
        static_cast<long>(expected)) == static_cast<long>(expected);
#else
    return __atomic_compare_exchange_n(target, &expected, desired,
                                        false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
#endif
}

uint32_t Atomic::fetchAdd32(volatile uint32_t* target, uint32_t value) {
#ifdef _MSC_VER
    return static_cast<uint32_t>(_InterlockedExchangeAdd(
        reinterpret_cast<volatile long*>(target), static_cast<long>(value)));
#else
    return __atomic_fetch_add(target, value, __ATOMIC_SEQ_CST);
#endif
}

uint32_t Atomic::fetchSub32(volatile uint32_t* target, uint32_t value) {
#ifdef _MSC_VER
    return static_cast<uint32_t>(_InterlockedExchangeAdd(
        reinterpret_cast<volatile long*>(target), -static_cast<long>(value)));
#else
    return __atomic_fetch_sub(target, value, __ATOMIC_SEQ_CST);
#endif
}

// ============================================================================
// قفل الدوران / Spinlock
// ============================================================================

Spinlock::Spinlock() : lock_(0), savedFlags_(0) {}

void Spinlock::lock() {
    // حفظ وتعطيل المقاطعات / Save and disable interrupts
    uint64_t flags = 0;
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile("pushfq; pop %0; cli" : "=r"(flags));
#elif defined(_MSC_VER)
    flags = __readeflags();
    // _disable() requires kernel mode — stubbed for user-mode build
#endif

    // محاولة الحصول على القفل / Try to acquire lock
    while (Atomic::exchange(&lock_, 1) != 0) {
        // دوران مع إشارة انتظار / Spin with pause hint
        while (Atomic::load(&lock_) != 0) {
            Atomic::pause();
        }
    }

    savedFlags_ = flags;
}

bool Spinlock::tryLock() {
    uint64_t flags = 0;
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile("pushfq; pop %0; cli" : "=r"(flags));
#elif defined(_MSC_VER)
    flags = __readeflags();
    // _disable() requires kernel mode — stubbed for user-mode build
#endif

    if (Atomic::exchange(&lock_, 1) == 0) {
        savedFlags_ = flags;
        return true;
    }

    // إعادة المقاطعات / Restore interrupts
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile("push %0; popfq" :: "r"(flags));
#elif defined(_MSC_VER)
    __writeeflags(flags);
#endif

    return false;
}

void Spinlock::unlock() {
    uint64_t flags = savedFlags_;

    Atomic::memoryBarrier();
    Atomic::store(&lock_, 0);

    // إعادة المقاطعات / Restore interrupts
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile("push %0; popfq" :: "r"(flags));
#elif defined(_MSC_VER)
    __writeeflags(flags);
#endif
}

bool Spinlock::isLocked() const {
    return Atomic::load(&lock_) != 0;
}

// ============================================================================
// قفل القراءة والكتابة / Read-Write Lock
// ============================================================================

RWLock::RWLock() : readers_(0) {}

void RWLock::readLock() {
    while (true) {
        // انتظار حتى لا يوجد كاتب / Wait until no writer
        while (Atomic::load(reinterpret_cast<const volatile uint64_t*>(&readers_)) ==
               static_cast<uint64_t>(-1LL)) {
            Atomic::pause();
        }

        // زيادة عدد القراء / Increment reader count
        int64_t current = static_cast<int64_t>(
            Atomic::fetchAdd(reinterpret_cast<volatile uint64_t*>(&readers_), 1));

        // إذا كان كاتب أخذ القفل بينما كنا نحاول / If writer took lock meanwhile
        if (current < 0) {
            Atomic::fetchSub(reinterpret_cast<volatile uint64_t*>(&readers_), 1);
            Atomic::pause();
            continue;
        }

        break;
    }
}

void RWLock::readUnlock() {
    Atomic::fetchSub(reinterpret_cast<volatile uint64_t*>(&readers_), 1);
}

void RWLock::writeLock() {
    // انتظار حتى لا يوجد قراء أو كتاب / Wait until no readers or writers
    while (true) {
        uint64_t expected = 0;
        if (Atomic::compareAndSwap(
                reinterpret_cast<volatile uint64_t*>(&readers_),
                expected, static_cast<uint64_t>(-1LL))) {
            break;
        }
        Atomic::pause();
    }
}

void RWLock::writeUnlock() {
    Atomic::store(reinterpret_cast<volatile uint64_t*>(&readers_), 0);
}

uint32_t RWLock::getReaderCount() const {
    int64_t val = static_cast<int64_t>(
        Atomic::load(reinterpret_cast<const volatile uint64_t*>(&readers_)));
    return (val >= 0) ? static_cast<uint32_t>(val) : 0;
}

// ============================================================================
// القفل التبادلي / Mutex
// ============================================================================

Mutex::Mutex() : locked_(0), owner_(0), waiters_(0) {}

void Mutex::lock() {
    // في سياق النواة: نحاول الحصول على القفل، وإلا ندخل قائمة الانتظار
    // In kernel context: try to acquire, else enter wait queue

    while (true) {
        if (Atomic::compareAndSwap(&locked_, 0, 1)) {
            owner_ = 0; // في نظام حقيقي: معرف الخيط الحالي / Real: current thread ID
            break;
        }

        Atomic::fetchAdd32(&waiters_, 1);

        // في نظام حقيقي هنا يتم تعليق الخيط / In real OS: suspend thread here
        while (Atomic::load(&locked_) != 0) {
            Atomic::pause();
        }

        Atomic::fetchSub32(&waiters_, 1);
    }
}

bool Mutex::tryLock() {
    if (Atomic::compareAndSwap(&locked_, 0, 1)) {
        owner_ = 0;
        return true;
    }
    return false;
}

void Mutex::unlock() {
    owner_ = 0;
    Atomic::memoryBarrier();
    Atomic::store(&locked_, 0);

    // في نظام حقيقي: إيقاظ خيط من قائمة الانتظار / In real OS: wake waiter
}

bool Mutex::isLocked() const {
    return Atomic::load(&locked_) != 0;
}

// ============================================================================
// السيمافور / Semaphore
// ============================================================================

Semaphore::Semaphore(uint32_t initialCount) : count_(initialCount), maxCount_(initialCount) {}

void Semaphore::wait() {
    while (true) {
        lock_.lock();
        if (count_ > 0) {
            count_--;
            lock_.unlock();
            return;
        }
        lock_.unlock();

        // في نظام حقيقي: تعليق الخيط / In real OS: suspend thread
        Atomic::pause();
    }
}

bool Semaphore::tryWait() {
    lock_.lock();
    if (count_ > 0) {
        count_--;
        lock_.unlock();
        return true;
    }
    lock_.unlock();
    return false;
}

void Semaphore::signal() {
    lock_.lock();
    count_++;
    lock_.unlock();

    // في نظام حقيقي: إيقاظ خيط منتظر / In real OS: wake waiting thread
}

uint32_t Semaphore::getValue() const {
    return count_;
}

// ============================================================================
// الحاجز / Barrier
// ============================================================================

Barrier::Barrier(uint32_t threadCount) : threshold_(threadCount), count_(0), generation_(0) {}

void Barrier::wait() {
    lock_.lock();

    uint64_t myGeneration = generation_;
    count_++;

    if (count_ >= threshold_) {
        // آخر خيط يصل: إعادة ضبط والانتقال للجيل التالي
        // Last thread to arrive: reset and advance generation
        count_ = 0;
        generation_++;
        lock_.unlock();
        return;
    }

    lock_.unlock();

    // باقي الخيوط تنتظر / Other threads wait
    while (Atomic::load(&generation_) == myGeneration) {
        Atomic::pause();
    }
}

void Barrier::reset() {
    lock_.lock();
    count_ = 0;
    generation_++;
    lock_.unlock();
}

// ============================================================================
// القفل التكراري / Recursive Lock
// ============================================================================

RecursiveLock::RecursiveLock() : owner_(0), lockCount_(0) {}

void RecursiveLock::lock() {
    // في نظام حقيقي: نستخدم معرف الخيط الحالي / In real OS: use current thread id
    uint64_t self = 0; // placeholder

    if (Atomic::load(&owner_) == self && lockCount_ > 0) {
        // نفس الخيط: زيادة العداد / Same thread: increment count
        lockCount_++;
        return;
    }

    spinlock_.lock();
    owner_ = self;
    lockCount_ = 1;
}

bool RecursiveLock::tryLock() {
    uint64_t self = 0;

    if (Atomic::load(&owner_) == self && lockCount_ > 0) {
        lockCount_++;
        return true;
    }

    if (spinlock_.tryLock()) {
        owner_ = self;
        lockCount_ = 1;
        return true;
    }
    return false;
}

void RecursiveLock::unlock() {
    lockCount_--;
    if (lockCount_ == 0) {
        owner_ = 0;
        spinlock_.unlock();
    }
}

// ============================================================================
// مرة واحدة / Once
// ============================================================================

Once::Once() : done_(false) {}

void Once::callOnce(void (*func)()) {
    if (done_) return;

    lock_.lock();
    if (!done_) {
        func();
        done_ = true;
    }
    lock_.unlock();
}

} // namespace LowLevel
} // namespace Sad
