/**
 * @file async_runtime.h
 * @brief (AR) نظام غير متزامن (Async/Await) للغة ص
 * @brief (EN) Async/Await runtime system for Sad language
 *
 * (AR) يوفر نظام مهام غير متزامنة مبني على خيوط (threads) مع Future/Promise
 * (EN) Provides a task-based async system built on threads with Future/Promise
 *
 * @author S Language Development Team
 * @date 2026
 */

#pragma once

#include "value.h"
#include <future>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <vector>
#include <memory>
#include <atomic>
#include <condition_variable>
#include <unordered_map>
#include <optional>
#include <iostream>

namespace Sad
{
    namespace Interpreter
    {

        // =========================================================================
        // (AR) وعد المستقبل — يُمثّل نتيجة عملية غير متزامنة
        // (EN) SadFuture — represents the result of an async operation
        // =========================================================================

        class SadFuture
        {
        public:
            enum class State
            {
                PENDING,
                RESOLVED,
                REJECTED
            };

            SadFuture() : state_(State::PENDING) {}

            /// (AR) هل اكتملت العملية؟ / (EN) Is the operation complete?
            bool isReady() const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                return state_ != State::PENDING;
            }

            /// (AR) تعيين النتيجة / (EN) Set result (resolve)
            void resolve(const Data::Value &value)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                result_ = value;
                state_ = State::RESOLVED;
                cv_.notify_all();
            }

            /// (AR) تعيين الخطأ / (EN) Set error (reject)
            void reject(const std::string &error)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                error_ = error;
                state_ = State::REJECTED;
                cv_.notify_all();
            }

            /// (AR) انتظار النتيجة (حجب) / (EN) Wait for result (blocking)
            Data::Value await()
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this]
                         { return state_ != State::PENDING; });
                if (state_ == State::REJECTED)
                {
                    throw std::runtime_error(error_);
                }
                return result_;
            }

            /// (AR) انتظار مع مهلة / (EN) Wait with timeout
            bool awaitFor(int timeoutMs)
            {
                std::unique_lock<std::mutex> lock(mutex_);
                return cv_.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                                    [this]
                                    { return state_ != State::PENDING; });
            }

            /// (AR) تعيين النتيجة — اسم بديل لـ resolve / (EN) Set result — alias for resolve
            void setResult(const Data::Value &value)
            {
                resolve(value);
            }

            /// (AR) الحصول على النتيجة مع حد زمني / (EN) Get result with timeout
            std::optional<Data::Value> getResultFor(int timeoutMs)
            {
                std::unique_lock<std::mutex> lock(mutex_);
                if (cv_.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                                 [this]
                                 { return state_ != State::PENDING; }))
                {
                    if (state_ == State::REJECTED)
                        return std::nullopt;
                    return result_;
                }
                return std::nullopt;
            }

            State getState() const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                return state_;
            }

            Data::Value getResult() const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                return result_;
            }

            std::string getError() const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                return error_;
            }

        private:
            mutable std::mutex mutex_;
            std::condition_variable cv_;
            State state_;
            Data::Value result_;
            std::string error_;
        };

        // =========================================================================
        // (AR) حوض الخيوط — يدير مجموعة من الخيوط لتنفيذ المهام
        // (EN) Thread Pool — manages a pool of threads for task execution
        // =========================================================================

        class ThreadPool
        {
        public:
            explicit ThreadPool(size_t numThreads = 4) : stop_(false), busy_(0)
            {
                size_t n = numThreads > 0 ? numThreads : 4;
                std::unique_lock<std::mutex> lock(queueMutex_);
                for (size_t i = 0; i < n; ++i)
                {
                    addWorkerLocked();
                }
            }

            ~ThreadPool()
            {
                {
                    std::unique_lock<std::mutex> lock(queueMutex_);
                    stop_ = true;
                }
                condition_.notify_all();
                for (auto &worker : workers_)
                {
                    if (worker.joinable())
                    {
                        worker.join();
                    }
                }
            }

            /// (AR) إرسال مهمة للتنفيذ / (EN) Submit a task for execution
            void submit(std::function<void()> task)
            {
                {
                    std::unique_lock<std::mutex> lock(queueMutex_);
                    tasks_.push(std::move(task));
                    // (AR) حوض مرن: الـgoroutines قد تحجب (channel/waitgroup/mutex).
                    //      مع حوض ثابت الحجم، goroutine محجوبة تحتجز عاملاً بينما
                    //      المهام التي تُحرّرها تبقى في الطابور بلا عامل ⇒ تجويع/تعلّق
                    //      (يظهر على عدّاء قليل النوى مثل Linux CI، لا على ويندوز كثير النوى).
                    //      نضمن وجود عامل لكلّ مهمة قابلة للتقدّم بإضافة عامل عند التشبّع.
                    // (EN) Elastic pool: goroutines may block (channel/waitgroup/mutex).
                    //      With a fixed pool, a blocked goroutine holds a worker while the
                    //      tasks that would unblock it sit queued with no worker ⇒ pool
                    //      starvation deadlock (surfaces on low-core runners like Linux CI,
                    //      not on many-core Windows). Grow on saturation so no task starves.
                    if (busy_.load() + tasks_.size() > workers_.size() &&
                        workers_.size() < kMaxWorkers)
                    {
                        addWorkerLocked();
                    }
                }
                condition_.notify_one();
            }

        private:
            static constexpr size_t kMaxWorkers = 8192;

            // (AR) يجب استدعاؤها مع حيازة queueMutex_ / (EN) Call with queueMutex_ held.
            void addWorkerLocked()
            {
                workers_.emplace_back([this]
                                      {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queueMutex_);
                        condition_.wait(lock, [this]{ return stop_ || !tasks_.empty(); });
                        if (stop_ && tasks_.empty()) return;
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    busy_.fetch_add(1);
                    task();
                    busy_.fetch_sub(1);
                } });
            }

            std::vector<std::thread> workers_;
            std::queue<std::function<void()>> tasks_;
            std::mutex queueMutex_;
            std::condition_variable condition_;
            bool stop_;
            std::atomic<size_t> busy_;
        };

        // =========================================================================
        // (AR) مدير المهام غير المتزامنة
        // (EN) Async Task Manager
        // =========================================================================

        class AsyncRuntime
        {
        public:
            static AsyncRuntime &getInstance()
            {
                static AsyncRuntime instance;
                return instance;
            }

            /// (AR) إنشاء مهمة غير متزامنة / (EN) Create an async task
            std::shared_ptr<SadFuture> createTask(std::function<Data::Value()> func)
            {
                auto future = std::make_shared<SadFuture>();
                pool_.submit([future, func = std::move(func)]()
                             {
            try {
                Data::Value result = func();
                future->resolve(result);
            } catch (const std::exception& e) {
                future->reject(e.what());
            } catch (...) {
                future->reject("Unknown async error");
            } });

                std::lock_guard<std::mutex> lock(mutex_);
                activeTasks_.push_back(future);
                return future;
            }

            /// (AR) انتظار جميع المهام / (EN) Wait for all tasks
            void awaitAll()
            {
                std::lock_guard<std::mutex> lock(mutex_);
                for (auto &task : activeTasks_)
                {
                    if (!task->isReady())
                    {
                        task->await();
                    }
                }
                activeTasks_.clear();
            }

            /// (AR) عدد المهام النشطة / (EN) Number of active tasks
            size_t activeTaskCount() const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                size_t count = 0;
                for (const auto &task : activeTasks_)
                {
                    if (!task->isReady())
                        count++;
                }
                return count;
            }

        private:
            AsyncRuntime() : pool_(std::thread::hardware_concurrency() > 0
                                       ? std::thread::hardware_concurrency()
                                       : 4) {}

            ThreadPool pool_;
            mutable std::mutex mutex_;
            std::vector<std::shared_ptr<SadFuture>> activeTasks_;
        };

    } // namespace Interpreter
} // namespace Sad
