/*
 * بسم الله الرحمن الرحيم
 * ================================
 * نظام Future/Promise للعمليات غير المتزامنة
 * Future/Promise System for Asynchronous Operations
 * ================================
 * 
 * نظام متقدم للبرمجة غير المتزامنة باستخدام Future و Promise
 * Advanced asynchronous programming using Future and Promise
 * 
 * الميزات / Features:
 * - Future<T> for async results
 * - Promise<T> for setting results
 * - Thread pool integration
 * - Exception handling
 * - Timeout support
 * 
 * @file runtime/thread/future.h
 * @author SadLang Compiler Team
 * @date January 2026
 * 
 * المراجع / References:
 * - thread.h:531-611 (ThreadPool class)
 * - thread.h:221-260 (Mutex class)
 * - thread.h:432-477 (ConditionVariable class)
 */

#ifndef SAD_RUNTIME_FUTURE_H
#define SAD_RUNTIME_FUTURE_H

#include "thread.h"
#include <memory>
#include <exception>
#include <stdexcept>
#include <chrono>
#include <utility>

namespace Sad {
namespace Thread {

// ============================================================================
// Forward Declarations / التعريفات المسبقة
// ============================================================================

template<typename T>
class Promise;

template<typename T>
class SharedState;

// ============================================================================
// Future State / حالة Future
// ============================================================================

/**
 * حالة Future / Future state
 */
enum class FutureState {
    Pending,        // قيد الانتظار / Pending
    Ready,          // جاهز / Ready
    Exception       // استثناء / Exception
};

// ============================================================================
// Shared State / الحالة المشتركة
// ============================================================================

/**
 * الحالة المشتركة بين Future و Promise
 * Shared state between Future and Promise
 * 
 * المراجع / References:
 * - thread.h:231 (Mutex class definition)
 * - thread.h:450 (ConditionVariable class definition)
 */
template<typename T>
class SharedState {
public:
    SharedState() 
        : state_(FutureState::Pending)
        , retrieved_(false) {
    }
    
    ~SharedState() = default;
    
    // منع النسخ / Prevent copying
    SharedState(const SharedState&) = delete;
    SharedState& operator=(const SharedState&) = delete;
    
    /**
     * تعيين القيمة / Set value
     * 
     * المتغيرات المستخدمة:
     * - mutex_: thread.h (Mutex class)
     * - state_: local variable
     * - value_: local variable
     * - cond_: thread.h (ConditionVariable class)
     */
    void set_value(T&& value) {
        LockGuard lock(mutex_);
        if (state_ != FutureState::Pending) {
            throw std::runtime_error("Promise already satisfied");
        }
        value_ = std::move(value);
        state_ = FutureState::Ready;
        cond_.notify_all();
    }
    
    /**
     * تعيين استثناء / Set exception
     * 
     * المتغيرات المستخدمة:
     * - mutex_: thread.h (Mutex class)
     * - state_: local variable
     * - exception_: local variable
     * - cond_: thread.h (ConditionVariable class)
     */
    void set_exception(std::exception_ptr exception) {
        LockGuard lock(mutex_);
        if (state_ != FutureState::Pending) {
            throw std::runtime_error("Promise already satisfied");
        }
        exception_ = exception;
        state_ = FutureState::Exception;
        cond_.notify_all();
    }
    
    /**
     * الحصول على القيمة / Get value
     * 
     * المتغيرات المستخدمة:
     * - mutex_: thread.h (Mutex class)
     * - state_: local variable
     * - retrieved_: local variable
     * - value_: local variable
     * - exception_: local variable
     * - cond_: thread.h (ConditionVariable class)
     */
    T get() {
        UniqueLock lock(mutex_);
        
        // انتظار حتى يصبح جاهزاً / Wait until ready
        cond_.wait(lock, [this] {
            return state_ != FutureState::Pending;
        });
        
        if (retrieved_) {
            throw std::runtime_error("Future already retrieved");
        }
        retrieved_ = true;
        
        if (state_ == FutureState::Exception) {
            std::rethrow_exception(exception_);
        }
        
        return std::move(value_);
    }
    
    /**
     * الانتظار / Wait
     * 
     * المتغيرات المستخدمة:
     * - mutex_: thread.h (Mutex class)
     * - state_: local variable
     * - cond_: thread.h (ConditionVariable class)
     */
    void wait() {
        UniqueLock lock(mutex_);
        cond_.wait(lock, [this] {
            return state_ != FutureState::Pending;
        });
    }
    
    /**
     * الانتظار مع مهلة زمنية / Wait with timeout
     * 
     * المتغيرات المستخدمة:
     * - mutex_: thread.h (Mutex class)
     * - state_: local variable
     * - cond_: thread.h (ConditionVariable class)
     */
    template<typename Rep, typename Period>
    bool wait_for(const std::chrono::duration<Rep, Period>& timeout) {
        UniqueLock lock(mutex_);
        return cond_.wait_for(lock, timeout, [this] {
            return state_ != FutureState::Pending;
        });
    }
    
    /**
     * التحقق من الجاهزية / Check if ready
     * 
     * المتغيرات المستخدمة:
     * - mutex_: thread.h (Mutex class)
     * - state_: local variable
     */
    bool is_ready() const {
        LockGuard lock(const_cast<Mutex&>(mutex_));
        return state_ != FutureState::Pending;
    }
    
private:
    // المتغيرات الخاصة / Private members
    // المصدر: thread.h:231 (Mutex)
    Mutex mutex_;
    
    // المصدر: thread.h:450 (ConditionVariable)  
    ConditionVariable cond_;
    
    // الحالة / State
    FutureState state_;
    
    // القيمة / Value
    T value_;
    
    // الاستثناء / Exception
    std::exception_ptr exception_;
    
    // تم الاسترجاع؟ / Retrieved?
    bool retrieved_;
};

// ============================================================================
// Future Class / فئة Future
// ============================================================================

/**
 * Future<T> - نتيجة عملية غير متزامنة
 * Future<T> - Result of asynchronous operation
 * 
 * @tparam T نوع النتيجة / Result type
 */
template<typename T>
class Future {
public:
    /**
     * إنشاء Future فارغ / Create empty Future
     */
    Future() : state_(nullptr) {}
    
    /**
     * إنشاء Future من حالة مشتركة / Create Future from shared state
     */
    explicit Future(std::shared_ptr<SharedState<T>> state)
        : state_(std::move(state)) {
    }
    
    /**
     * مُدمر / Destructor
     */
    ~Future() = default;
    
    // منع النسخ / Prevent copying
    Future(const Future&) = delete;
    Future& operator=(const Future&) = delete;
    
    // السماح بالنقل / Allow moving
    Future(Future&& other) noexcept
        : state_(std::move(other.state_)) {
    }
    
    Future& operator=(Future&& other) noexcept {
        if (this != &other) {
            state_ = std::move(other.state_);
        }
        return *this;
    }
    
    /**
     * الحصول على النتيجة / Get result
     * 
     * المتغيرات المستخدمة:
     * - state_: std::shared_ptr<SharedState<T>> (local member)
     * 
     * الدوال المستدعاة:
     * - state_->get(): SharedState::get() (defined above, line ~144)
     */
    T get() {
        if (!state_) {
            throw std::runtime_error("Future is empty");
        }
        return state_->get();
    }
    
    /**
     * الانتظار / Wait
     * 
     * المتغيرات المستخدمة:
     * - state_: std::shared_ptr<SharedState<T>> (local member)
     * 
     * الدوال المستدعاة:
     * - state_->wait(): SharedState::wait() (defined above, line ~167)
     */
    void wait() {
        if (!state_) {
            throw std::runtime_error("Future is empty");
        }
        state_->wait();
    }
    
    /**
     * الانتظار مع مهلة زمنية / Wait with timeout
     * 
     * المتغيرات المستخدمة:
     * - state_: std::shared_ptr<SharedState<T>> (local member)
     * 
     * الدوال المستدعاة:
     * - state_->wait_for(): SharedState::wait_for() (defined above, line ~179)
     */
    template<typename Rep, typename Period>
    bool wait_for(const std::chrono::duration<Rep, Period>& timeout) {
        if (!state_) {
            throw std::runtime_error("Future is empty");
        }
        return state_->wait_for(timeout);
    }
    
    /**
     * التحقق من الجاهزية / Check if ready
     * 
     * المتغيرات المستخدمة:
     * - state_: std::shared_ptr<SharedState<T>> (local member)
     * 
     * الدوال المستدعاة:
     * - state_->is_ready(): SharedState::is_ready() (defined above, line ~193)
     */
    bool is_ready() const {
        if (!state_) {
            return false;
        }
        return state_->is_ready();
    }
    
    /**
     * التحقق من الصلاحية / Check validity
     * 
     * المتغيرات المستخدمة:
     * - state_: std::shared_ptr<SharedState<T>> (local member)
     */
    bool valid() const {
        return state_ != nullptr;
    }
    
private:
    // الحالة المشتركة / Shared state
    std::shared_ptr<SharedState<T>> state_;
    
    // Promise هو صديق / Promise is friend
    friend class Promise<T>;
};

// ============================================================================
// Promise Class / فئة Promise
// ============================================================================

/**
 * Promise<T> - وعد بتقديم نتيجة
 * Promise<T> - Promise to provide a result
 * 
 * @tparam T نوع النتيجة / Result type
 */
template<typename T>
class Promise {
public:
    /**
     * إنشاء Promise / Create Promise
     */
    Promise()
        : state_(std::make_shared<SharedState<T>>())
        , future_retrieved_(false) {
    }
    
    /**
     * مُدمر / Destructor
     */
    ~Promise() = default;
    
    // منع النسخ / Prevent copying
    Promise(const Promise&) = delete;
    Promise& operator=(const Promise&) = delete;
    
    // السماح بالنقل / Allow moving
    Promise(Promise&& other) noexcept
        : state_(std::move(other.state_))
        , future_retrieved_(other.future_retrieved_) {
        other.future_retrieved_ = false;
    }
    
    Promise& operator=(Promise&& other) noexcept {
        if (this != &other) {
            state_ = std::move(other.state_);
            future_retrieved_ = other.future_retrieved_;
            other.future_retrieved_ = false;
        }
        return *this;
    }
    
    /**
     * الحصول على Future / Get Future
     * 
     * المتغيرات المستخدمة:
     * - state_: std::shared_ptr<SharedState<T>> (local member)
     * - future_retrieved_: bool (local member)
     */
    Future<T> get_future() {
        if (future_retrieved_) {
            throw std::runtime_error("Future already retrieved");
        }
        future_retrieved_ = true;
        return Future<T>(state_);
    }
    
    /**
     * تعيين القيمة / Set value
     * 
     * المتغيرات المستخدمة:
     * - state_: std::shared_ptr<SharedState<T>> (local member)
     * 
     * الدوال المستدعاة:
     * - state_->set_value(): SharedState::set_value() (defined above, line ~104)
     */
    void set_value(T&& value) {
        if (!state_) {
            throw std::runtime_error("Promise is empty");
        }
        state_->set_value(std::move(value));
    }
    
    /**
     * تعيين القيمة (نسخة) / Set value (copy)
     * 
     * المتغيرات المستخدمة:
     * - state_: std::shared_ptr<SharedState<T>> (local member)
     * 
     * الدوال المستدعاة:
     * - state_->set_value(): SharedState::set_value() (defined above, line ~104)
     */
    void set_value(const T& value) {
        T copy = value;
        set_value(std::move(copy));
    }
    
    /**
     * تعيين استثناء / Set exception
     * 
     * المتغيرات المستخدمة:
     * - state_: std::shared_ptr<SharedState<T>> (local member)
     * 
     * الدوال المستدعاة:
     * - state_->set_exception(): SharedState::set_exception() (defined above, line ~122)
     */
    void set_exception(std::exception_ptr exception) {
        if (!state_) {
            throw std::runtime_error("Promise is empty");
        }
        state_->set_exception(exception);
    }
    
private:
    // الحالة المشتركة / Shared state
    std::shared_ptr<SharedState<T>> state_;
    
    // تم استرجاع Future؟ / Future retrieved?
    bool future_retrieved_;
};

// ============================================================================
// Async Function / دالة غير متزامنة
// ============================================================================

/**
 * تشغيل دالة بشكل غير متزامن / Run function asynchronously
 * 
 * @tparam F نوع الدالة / Function type
 * @param pool تجمع الخيوط / Thread pool
 * @param func الدالة / Function
 * @return Future<R> حيث R نوع إرجاع الدالة / Future<R> where R is return type
 * 
 * المراجع / References:
 * - thread.h:531-611 (ThreadPool class)
 * - thread.h:571 (submit() method: void submit(F&& task))
 */
template<typename F>
auto async(ThreadPool& pool, F&& func) 
    -> Future<typename std::result_of<F()>::type> {
    
    using R = typename std::result_of<F()>::type;
    
    // إنشاء Promise / Create Promise
    auto promise = std::make_shared<Promise<R>>();
    
    // الحصول على Future / Get Future
    auto future = promise->get_future();
    
    // إضافة المهمة إلى ThreadPool / Submit task to ThreadPool
    // المصدر: thread.h:571 (ThreadPool::submit)
    pool.submit([promise, func = std::forward<F>(func)]() mutable {
        try {
            // تنفيذ الدالة وتعيين النتيجة / Execute function and set result
            promise->set_value(func());
        } catch (...) {
            // تعيين الاستثناء / Set exception
            promise->set_exception(std::current_exception());
        }
    });
    
    return future;
}

} // namespace Thread
} // namespace Sad

#endif // SAD_RUNTIME_FUTURE_H
