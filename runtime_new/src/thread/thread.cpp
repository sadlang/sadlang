/*
 * بسم الله الرحمن الرحيم
 * ================================
 * تنفيذ نظام إدارة الخيوط / Thread Management Implementation
 * ================================
 */

#include "thread/thread.h"
#include <algorithm>

namespace Sad {
namespace Thread {

// ============================================================================
// Thread Implementation / تنفيذ الخيط
// ============================================================================

static std::atomic<ThreadId> g_next_thread_id{1};

Thread::Thread(Function func, const ThreadConfig* config)
    : id_(generate_id())
    , function_(std::move(func))
    , config_(config ? *config : ThreadConfig::default_config())
    , state_(ThreadState::Created)
    , stop_flag_(false)
{
}

Thread::~Thread() {
    if (thread_ && thread_->joinable()) {
        thread_->join();
    }
}

Thread::Thread(Thread&& other) noexcept
    : id_(other.id_)
    , thread_(std::move(other.thread_))
    , function_(std::move(other.function_))
    , config_(other.config_)
    , state_(other.state_.load())
    , stop_flag_(other.stop_flag_.load())
{
    other.id_ = 0;
}

Thread& Thread::operator=(Thread&& other) noexcept {
    if (this != &other) {
        if (thread_ && thread_->joinable()) {
            thread_->join();
        }
        
        id_ = other.id_;
        thread_ = std::move(other.thread_);
        function_ = std::move(other.function_);
        config_ = other.config_;
        state_ = other.state_.load();
        stop_flag_ = other.stop_flag_.load();
        
        other.id_ = 0;
    }
    return *this;
}

bool Thread::start() {
    if (thread_ || state_ != ThreadState::Created) {
        return false;
    }
    
    try {
        thread_ = std::make_unique<std::thread>(&Thread::run, this);
        
        if (config_.detached) {
            thread_->detach();
        }
        
        return true;
    }
    catch (...) {
        return false;
    }
}

void Thread::join() {
    if (thread_ && thread_->joinable()) {
        thread_->join();
    }
}

void Thread::detach() {
    if (thread_ && thread_->joinable()) {
        thread_->detach();
    }
}

void Thread::request_stop() {
    stop_flag_.store(true, std::memory_order_release);
}

ThreadHandle Thread::get_handle() const {
    return thread_ ? thread_->native_handle() : ThreadHandle{};
}

ThreadId Thread::current_thread_id() {
    return std::hash<std::thread::id>{}(std::this_thread::get_id());
}

void Thread::sleep_for(std::chrono::milliseconds duration) {
    std::this_thread::sleep_for(duration);
}

void Thread::yield() {
    std::this_thread::yield();
}

void Thread::run() {
    state_.store(ThreadState::Running, std::memory_order_release);
    
    try {
        if (function_) {
            function_();
        }
    }
    catch (...) {
        // معالجة الاستثناءات / Handle exceptions
    }
    
    state_.store(ThreadState::Terminated, std::memory_order_release);
}

ThreadId Thread::generate_id() {
    return g_next_thread_id.fetch_add(1, std::memory_order_relaxed);
}

// ============================================================================
// RWLock Implementation / تنفيذ قفل القراءة والكتابة
// ============================================================================

void RWLock::lock_read() {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this] { return !writer_; });
    ++readers_;
}

bool RWLock::try_lock_read() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (writer_) {
        return false;
    }
    ++readers_;
    return true;
}

void RWLock::unlock_read() {
    std::unique_lock<std::mutex> lock(mutex_);
    --readers_;
    if (readers_ == 0) {
        cond_.notify_all();
    }
}

void RWLock::lock_write() {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this] { return !writer_ && readers_ == 0; });
    writer_ = true;
}

bool RWLock::try_lock_write() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (writer_ || readers_ > 0) {
        return false;
    }
    writer_ = true;
    return true;
}

void RWLock::unlock_write() {
    std::unique_lock<std::mutex> lock(mutex_);
    writer_ = false;
    cond_.notify_all();
}

// ============================================================================
// ConditionVariable Implementation / تنفيذ متغير الشرط
// ============================================================================

void ConditionVariable::wait(UniqueLock& lock) {
    if (!lock.owns_lock()) {
        return;
    }
    
    std::unique_lock<std::mutex> std_lock(lock.mutex()->native(), std::adopt_lock);
    cond_.wait(std_lock);
    std_lock.release();
}

// ============================================================================
// Semaphore Implementation / تنفيذ العداد الثنائي
// ============================================================================

void Semaphore::wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this] { return count_.load() > 0; });
    count_.fetch_sub(1, std::memory_order_release);
}

bool Semaphore::try_wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (count_.load() > 0) {
        count_.fetch_sub(1, std::memory_order_release);
        return true;
    }
    return false;
}

void Semaphore::signal() {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        count_.fetch_add(1, std::memory_order_release);
    }
    cond_.notify_one();
}

// ============================================================================
// ThreadPool Implementation / تنفيذ تجمع الخيوط
// ============================================================================

ThreadPool::ThreadPool(size_t num_threads)
    : stop_(false)
    , active_tasks_(0)
{
    workers_.reserve(num_threads);
    
    for (size_t i = 0; i < num_threads; ++i) {
        workers_.emplace_back(&ThreadPool::worker, this);
    }
}

ThreadPool::~ThreadPool() {
    shutdown();
}

void ThreadPool::wait_all() {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this] {
        return tasks_.empty() && active_tasks_.load() == 0;
    });
}

void ThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        stop_.store(true, std::memory_order_release);
    }
    
    cond_.notify_all();
    
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    
    workers_.clear();
}

size_t ThreadPool::get_pending_tasks() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return tasks_.size();
}

void ThreadPool::worker() {
    while (true) {
        Task task;
        
        {
            std::unique_lock<std::mutex> lock(mutex_);
            
            cond_.wait(lock, [this] {
                return stop_.load() || !tasks_.empty();
            });
            
            if (stop_.load() && tasks_.empty()) {
                return;
            }
            
            if (!tasks_.empty()) {
                task = std::move(tasks_.front());
                tasks_.pop();
                active_tasks_.fetch_add(1, std::memory_order_release);
            }
        }
        
        if (task) {
            try {
                task();
            }
            catch (...) {
                // معالجة الاستثناءات / Handle exceptions
            }
            
            active_tasks_.fetch_sub(1, std::memory_order_release);
            cond_.notify_all();
        }
    }
}

} // namespace Thread
} // namespace Sad
