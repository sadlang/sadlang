/**
 * @file channel.h
 * @brief (AR) قنوات الاتصال بين المهام المتزامنة (Goroutines) — مثل قنوات Go
 * @brief (EN) Communication channels between concurrent tasks (Goroutines) — like Go channels
 *
 * (AR) يوفر هذا الملف صنف SadChannel الذي يُنفّذ قناة اتصال آمنة بين الخيوط.
 *      يدعم القنوات المحدودة الحجم (buffered) وغير المحدودة (unbuffered).
 *      العمليات الأساسية:
 *        - أرسل(قيمة) — إرسال قيمة (يحجب إذا القناة ممتلئة)
 *        - استقبل()    — استقبال قيمة (يحجب إذا القناة فارغة)
 *        - أغلق()      — إغلاق القناة
 *        - حاول_استقبل() — محاولة استقبال بدون حجب (للاختيار select)
 *
 * (EN) Provides the SadChannel class implementing a thread-safe communication channel.
 *      Supports both buffered and unbuffered channels.
 *      Core operations:
 *        - send(value) — send a value (blocks if channel full)
 *        - receive()   — receive a value (blocks if channel empty)
 *        - close()     — close the channel
 *        - tryReceive() — non-blocking receive (for select)
 *
 * @author S Language Development Team
 * @date 2026
 */

#pragma once

#include "value.h"
#include "async_runtime.h" // (AR) ThreadPool لإطلاق goroutines / (EN) ThreadPool for goroutine launching
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <optional>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <thread>
#include <functional>
#include <utility>
#include <unordered_map>

namespace Sad
{
    namespace Interpreter
    {

        // =========================================================================
        // (AR) استثناء القناة المغلقة
        // (EN) Closed channel exception
        // =========================================================================

        class ChannelClosedError : public std::runtime_error
        {
        public:
            ChannelClosedError()
                : std::runtime_error("خطأ: محاولة إرسال على قناة مغلقة / Error: send on closed channel") {}
        };

        // =========================================================================
        // (AR) قناة اتصال بين المهام المتزامنة
        // (EN) Communication channel between concurrent tasks
        // =========================================================================

        class SadChannel
        {
        public:
            /**
             * @brief (AR) إنشاء قناة جديدة
             * @brief (EN) Create a new channel
             *
             * @param bufferSize (AR) حجم المخزن المؤقت — 0 = قناة غير محدودة (unbuffered)
             *                   (EN) Buffer size — 0 = unbuffered channel
             */
            explicit SadChannel(size_t bufferSize = 0)
                : bufferSize_(bufferSize), closed_(false) {}

            /**
             * @brief (AR) إرسال قيمة عبر القناة — يحجب إذا القناة ممتلئة
             * @brief (EN) Send a value through the channel — blocks if channel is full
             *
             * @param value (AR) القيمة المُرسلة / (EN) Value to send
             * @throws ChannelClosedError (AR) إذا كانت القناة مغلقة / (EN) If channel is closed
             */
            void send(const Data::Value &value)
            {
                std::unique_lock<std::mutex> lock(mutex_);

                if (closed_)
                {
                    throw ChannelClosedError();
                }

                // (AR) إذا كانت القناة محدودة الحجم وممتلئة، انتظر
                // (EN) If buffered and full, wait
                if (bufferSize_ > 0)
                {
                    notFull_.wait(lock, [this]()
                                  { return queue_.size() < bufferSize_ || closed_; });
                }
                else
                {
                    // (AR) قناة unbuffered — انتظر حتى يستقبل مستقبل
                    // (EN) Unbuffered — wait until a receiver is ready
                    // (AR) نُضيف للصف ثم ننتظر أن يُستهلك
                    // (EN) Push to queue then wait for it to be consumed
                }

                if (closed_)
                {
                    throw ChannelClosedError();
                }

                queue_.push(value);
                notEmpty_.notify_one();
            }

            /**
             * @brief (AR) استقبال قيمة من القناة — يحجب إذا القناة فارغة
             * @brief (EN) Receive a value from the channel — blocks if channel is empty
             *
             * @return (AR) القيمة المُستقبلة، أو لاشيء إذا القناة مغلقة وفارغة
             *         (EN) Received value, or null if channel is closed and empty
             */
            Data::Value receive()
            {
                std::unique_lock<std::mutex> lock(mutex_);

                notEmpty_.wait(lock, [this]()
                               { return !queue_.empty() || closed_; });

                if (queue_.empty() && closed_)
                {
                    // (AR) القناة مغلقة وفارغة — ارجع لاشيء
                    // (EN) Channel closed and empty — return null
                    return Data::Value();
                }

                Data::Value value = queue_.front();
                queue_.pop();
                notFull_.notify_one();
                return value;
            }

            /**
             * @brief (AR) محاولة استقبال بدون حجب — للاستخدام في اختر (select)
             * @brief (EN) Try to receive without blocking — for use in select
             *
             * @return (AR) القيمة إذا كانت متاحة، أو std::nullopt
             *         (EN) Value if available, or std::nullopt
             */
            std::optional<Data::Value> tryReceive()
            {
                std::lock_guard<std::mutex> lock(mutex_);

                if (queue_.empty())
                {
                    return std::nullopt;
                }

                Data::Value value = queue_.front();
                queue_.pop();
                notFull_.notify_one();
                return value;
            }

            /**
             * @brief (AR) محاولة إرسال بدون حجب — للاستخدام في اختر (select)
             * @brief (EN) Try to send without blocking — for use in select
             *
             * @return (AR) هل نجح الإرسال / (EN) Whether send succeeded
             */
            bool trySend(const Data::Value &value)
            {
                std::lock_guard<std::mutex> lock(mutex_);

                if (closed_)
                    return false;

                if (bufferSize_ > 0 && queue_.size() >= bufferSize_)
                {
                    return false;
                }

                queue_.push(value);
                notEmpty_.notify_one();
                return true;
            }

            /**
             * @brief (AR) إغلاق القناة — لن تقبل قيماً جديدة
             * @brief (EN) Close the channel — no new values accepted
             */
            void close()
            {
                std::lock_guard<std::mutex> lock(mutex_);
                closed_ = true;
                notEmpty_.notify_all();
                notFull_.notify_all();
            }

            /**
             * @brief (AR) هل القناة مغلقة؟ / (EN) Is the channel closed?
             */
            bool isClosed() const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                return closed_;
            }

            /**
             * @brief (AR) عدد العناصر في القناة / (EN) Number of items in channel
             */
            size_t size() const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                return queue_.size();
            }

            /**
             * @brief (AR) سعة المخزن المؤقت / (EN) Buffer capacity
             */
            size_t capacity() const
            {
                return bufferSize_;
            }

            /**
             * @brief (AR) هل القناة فارغة؟ / (EN) Is the channel empty?
             */
            bool empty() const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                return queue_.empty();
            }

            /**
             * @brief (AR) إرسال مع حد زمني (بالمللي ثانية)
             * @brief (EN) Send with timeout (in milliseconds)
             *
             * @param value (AR) القيمة المُرسلة / (EN) Value to send
             * @param timeoutMs (AR) الحد الزمني بالمللي ثانية / (EN) Timeout in milliseconds
             * @return (AR) صحيح إذا نجح الإرسال، خطأ إذا انتهى الوقت
             *         (EN) true if sent, false if timed out
             */
            bool sendTimeout(const Data::Value &value, int timeoutMs)
            {
                std::unique_lock<std::mutex> lock(mutex_);

                if (closed_)
                    return false;

                if (bufferSize_ > 0)
                {
                    if (!notFull_.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                                           [this]()
                                           { return queue_.size() < bufferSize_ || closed_; }))
                    {
                        return false; // (AR) انتهى الوقت / (EN) Timed out
                    }
                }

                if (closed_)
                    return false;

                queue_.push(value);
                notEmpty_.notify_one();
                return true;
            }

            /**
             * @brief (AR) استقبال مع حد زمني (بالمللي ثانية)
             * @brief (EN) Receive with timeout (in milliseconds)
             *
             * @param timeoutMs (AR) الحد الزمني بالمللي ثانية / (EN) Timeout in milliseconds
             * @return (AR) القيمة إذا نجح، أو std::nullopt إذا انتهى الوقت
             *         (EN) Value if received, or std::nullopt if timed out
             */
            std::optional<Data::Value> receiveTimeout(int timeoutMs)
            {
                std::unique_lock<std::mutex> lock(mutex_);

                if (!notEmpty_.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                                        [this]()
                                        { return !queue_.empty() || closed_; }))
                {
                    return std::nullopt; // (AR) انتهى الوقت / (EN) Timed out
                }

                if (queue_.empty() && closed_)
                {
                    return std::nullopt;
                }

                Data::Value value = queue_.front();
                queue_.pop();
                notFull_.notify_one();
                return value;
            }

        private:
            size_t bufferSize_;                ///< (AR) حجم المخزن المؤقت — 0 = unbuffered
            std::queue<Data::Value> queue_;    ///< (AR) صف القيم / (EN) Value queue
            mutable std::mutex mutex_;         ///< (AR) قفل التزامن / (EN) Synchronization mutex
            std::condition_variable notEmpty_; ///< (AR) إشعار عند عدم الفراغ / (EN) Notify when not empty
            std::condition_variable notFull_;  ///< (AR) إشعار عند عدم الامتلاء / (EN) Notify when not full
            bool closed_;                      ///< (AR) هل القناة مغلقة؟ / (EN) Is channel closed?
        };

        // =========================================================================
        // (AR) سجل القنوات — يربط معرّفات القنوات بكائنات SadChannel
        // (EN) Channel Registry — maps channel IDs to SadChannel objects
        // =========================================================================

        class SadChannelRegistry
        {
        public:
            static SadChannelRegistry &getInstance()
            {
                static SadChannelRegistry instance;
                return instance;
            }

            /**
             * @brief (AR) تسجيل قناة جديدة وإرجاع معرّفها
             * @brief (EN) Register a new channel and return its ID
             */
            size_t registerChannel(std::shared_ptr<SadChannel> channel)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                size_t id = nextId_++;
                channels_[id] = std::move(channel);
                return id;
            }

            /**
             * @brief (AR) الحصول على قناة بمعرّفها
             * @brief (EN) Get a channel by its ID
             */
            std::shared_ptr<SadChannel> getChannel(size_t id) const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                auto it = channels_.find(id);
                if (it == channels_.end())
                    return nullptr;
                return it->second;
            }

            /**
             * @brief (AR) إزالة قناة من السجل
             * @brief (EN) Remove a channel from the registry
             */
            void removeChannel(size_t id)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                channels_.erase(id);
            }

        private:
            SadChannelRegistry() : nextId_(1) {}
            mutable std::mutex mutex_;
            std::unordered_map<size_t, std::shared_ptr<SadChannel>> channels_;
            size_t nextId_;
        };

        // =========================================================================
        // (AR) مدير Goroutines — يدير إطلاق المهام وانتظارها
        // (EN) Goroutine Manager — manages task launching and waiting
        // =========================================================================

        class GoroutineManager
        {
        public:
            static GoroutineManager &getInstance()
            {
                static GoroutineManager instance;
                return instance;
            }

            /**
             * @brief (AR) إطلاق goroutine جديدة
             * @brief (EN) Launch a new goroutine
             *
             * @param task (AR) الدالة المراد تنفيذها / (EN) Function to execute
             */
            void launch(std::function<void()> task)
            {
                auto future = std::make_shared<std::atomic<bool>>(false);
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    activeFutures_.push_back(future);
                }
                pool_.submit([task = std::move(task), future]()
                             {
            try {
                task();
            } catch (const std::exception& e) {
                // (AR) خطأ في goroutine — نسجل الخطأ ونستمر
                // (EN) Error in goroutine — log error and continue
                std::cerr << "خطأ في goroutine: " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "خطأ غير معروف في goroutine" << std::endl;
            }
            future->store(true); });
            }

            /**
             * @brief (AR) انتظار جميع المهام النشطة
             * @brief (EN) Wait for all active goroutines to complete
             */
            void waitAll()
            {
                // (AR) ننتظر حتى تكتمل جميع المهام
                // (EN) Wait until all tasks complete
                while (true)
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    // (AR) أزل المهام المكتملة
                    activeFutures_.erase(
                        std::remove_if(activeFutures_.begin(), activeFutures_.end(),
                                       [](const auto &f)
                                       { return f->load(); }),
                        activeFutures_.end());
                    if (activeFutures_.empty())
                        break;
                    // (AR) حلقة انتظار نشطة مع تأخير صغير لتجنب استهلاك CPU
                    // (EN) Active wait loop with small delay to avoid CPU consumption
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }

            /**
             * @brief (AR) عدد المهام النشطة / (EN) Number of active goroutines
             */
            size_t activeCount() const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                size_t count = 0;
                for (const auto &f : activeFutures_)
                {
                    if (!f->load())
                        count++;
                }
                return count;
            }

        private:
            GoroutineManager()
                : pool_(std::thread::hardware_concurrency() > 0
                            ? std::thread::hardware_concurrency()
                            : 4) {}

            ThreadPool pool_;
            mutable std::mutex mutex_;
            std::vector<std::shared_ptr<std::atomic<bool>>> activeFutures_;
        };

    } // namespace Interpreter
} // namespace Sad

// ═════════════════════════════════════════════════════════════════════════════
// (AR) ملف channel.h — يحتوي أيضاً على:
//   - SadWaitGroup: مجموعة انتظار لتنسيق عدد محدد من goroutines
//   - SadMutex: قفل تبادلي للوصول المشترك الآمن
//   - SadFuture: مستقبل لإرجاع قيمة من goroutine
// (EN) channel.h also contains:
//   - SadWaitGroup: WaitGroup for coordinating a fixed number of goroutines
//   - SadMutex: Mutex for safe shared access
//   - SadFuture: Future for returning a value from a goroutine
// ═════════════════════════════════════════════════════════════════════════════

namespace Sad
{
    namespace Interpreter
    {

        // =========================================================================
        // (AR) مجموعة انتظار — تنسيق عدد محدد من المهام المتزامنة
        // (EN) WaitGroup — coordinate a specific number of concurrent tasks
        //
        // الاستخدام:
        //   متغير مج = مجموعة_انتظار()
        //   مج.أضف(3)          # نتوقع 3 مهام
        //   أطلق دالة() ... مج.أنهي() ... نهاية    (×3)
        //   مج.انتظر()          # يحجب حتى تكتمل الـ 3
        // =========================================================================

        class SadWaitGroup
        {
        public:
            SadWaitGroup() : count_(0) {}

            /**
             * @brief (AR) إضافة عدد من المهام المتوقعة
             * @brief (EN) Add expected number of tasks
             */
            void add(int delta)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                count_ += delta;
                if (count_ < 0)
                {
                    throw std::runtime_error("خطأ: عداد مجموعة الانتظار أصبح سالباً / WaitGroup counter went negative");
                }
                if (count_ == 0)
                {
                    cv_.notify_all();
                }
            }

            /**
             * @brief (AR) إنهاء مهمة واحدة (يعادل أضف(-1))
             * @brief (EN) Mark one task as done (equivalent to add(-1))
             */
            void done()
            {
                add(-1);
            }

            /**
             * @brief (AR) انتظار حتى يصل العداد إلى صفر
             * @brief (EN) Wait until counter reaches zero
             */
            void wait()
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this]()
                         { return count_ == 0; });
            }

            /**
             * @brief (AR) انتظار مع حد زمني (بالمللي ثانية)
             * @brief (EN) Wait with timeout (in milliseconds)
             * @return (AR) صحيح إذا اكتمل، خطأ إذا انتهى الوقت
             *         (EN) true if completed, false if timed out
             */
            bool waitFor(int timeoutMs)
            {
                std::unique_lock<std::mutex> lock(mutex_);
                return cv_.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                                    [this]()
                                    { return count_ == 0; });
            }

            /**
             * @brief (AR) القيمة الحالية للعداد / (EN) Current counter value
             */
            int count() const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                return count_;
            }

        private:
            int count_;
            mutable std::mutex mutex_;
            std::condition_variable cv_;
        };

        // =========================================================================
        // (AR) سجل مجموعات الانتظار
        // (EN) WaitGroup Registry
        // =========================================================================

        class SadWaitGroupRegistry
        {
        public:
            static SadWaitGroupRegistry &getInstance()
            {
                static SadWaitGroupRegistry instance;
                return instance;
            }

            size_t registerWaitGroup(std::shared_ptr<SadWaitGroup> wg)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                size_t id = nextId_++;
                waitGroups_[id] = std::move(wg);
                return id;
            }

            std::shared_ptr<SadWaitGroup> getWaitGroup(size_t id) const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                auto it = waitGroups_.find(id);
                if (it == waitGroups_.end())
                    return nullptr;
                return it->second;
            }

        private:
            SadWaitGroupRegistry() : nextId_(1) {}
            mutable std::mutex mutex_;
            std::unordered_map<size_t, std::shared_ptr<SadWaitGroup>> waitGroups_;
            size_t nextId_;
        };

        // =========================================================================
        // (AR) قفل تبادلي — للوصول المشترك الآمن بين goroutines
        // (EN) Mutex — for safe shared access between goroutines
        //
        // الاستخدام:
        //   متغير ق = قفل()
        //   ق.اقفل()
        //   # ... عمليات حصرية ...
        //   ق.افتح()
        // =========================================================================

        class SadMutex
        {
        public:
            SadMutex() : locked_(false) {}

            /**
             * @brief (AR) قفل — يحجب حتى يُتاح القفل
             * @brief (EN) Lock — blocks until lock is available
             */
            void lock()
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this]()
                         { return !locked_; });
                locked_ = true;
            }

            /**
             * @brief (AR) فتح القفل
             * @brief (EN) Unlock
             */
            void unlock()
            {
                std::lock_guard<std::mutex> lock(mutex_);
                locked_ = false;
                cv_.notify_one();
            }

            /**
             * @brief (AR) محاولة القفل بدون حجب
             * @brief (EN) Try to lock without blocking
             * @return (AR) صحيح إذا تم القفل / (EN) true if locked
             */
            bool tryLock()
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (!locked_)
                {
                    locked_ = true;
                    return true;
                }
                return false;
            }

            /**
             * @brief (AR) هل القفل مُغلق؟ / (EN) Is the mutex locked?
             */
            bool isLocked() const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                return locked_;
            }

        private:
            bool locked_;
            mutable std::mutex mutex_;
            std::condition_variable cv_;
        };

        // =========================================================================
        // (AR) سجل الأقفال
        // (EN) Mutex Registry
        // =========================================================================

        class SadMutexRegistry
        {
        public:
            static SadMutexRegistry &getInstance()
            {
                static SadMutexRegistry instance;
                return instance;
            }

            size_t registerMutex(std::shared_ptr<SadMutex> mtx)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                size_t id = nextId_++;
                mutexes_[id] = std::move(mtx);
                return id;
            }

            std::shared_ptr<SadMutex> getMutex(size_t id) const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                auto it = mutexes_.find(id);
                if (it == mutexes_.end())
                    return nullptr;
                return it->second;
            }

        private:
            SadMutexRegistry() : nextId_(1) {}
            mutable std::mutex mutex_;
            std::unordered_map<size_t, std::shared_ptr<SadMutex>> mutexes_;
            size_t nextId_;
        };

        // =========================================================================
        // (AR) مستقبل (Future) — يُستخدم SadFuture المُعرّف في async_runtime.h
        //      هنا فقط SadFutureRegistry لربط المعرّفات بكائنات المستقبل
        // (EN) Future — uses SadFuture defined in async_runtime.h
        //      Here only SadFutureRegistry to map IDs to future objects
        //
        // الاستخدام:
        //   متغير م = مستقبل()
        //   أطلق
        //       م.عيّن(حساب_ثقيل())
        //   نهاية
        //   متغير نتيجة = م.احصل()   # يحجب حتى تجهز النتيجة
        // =========================================================================

        // (AR) SadFuture مُعرّف في async_runtime.h — لا نُعيد تعريفه هنا
        // (EN) SadFuture defined in async_runtime.h — not redefined here

        // =========================================================================
        // (AR) سجل المستقبلات
        // (EN) Future Registry
        // =========================================================================

        class SadFutureRegistry
        {
        public:
            static SadFutureRegistry &getInstance()
            {
                static SadFutureRegistry instance;
                return instance;
            }

            size_t registerFuture(std::shared_ptr<SadFuture> fut)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                size_t id = nextId_++;
                futures_[id] = fut;
                return id;
            }

            std::shared_ptr<SadFuture> getFuture(size_t id) const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                auto it = futures_.find(id);
                if (it == futures_.end())
                    return nullptr;
                return it->second;
            }

        private:
            SadFutureRegistry() : nextId_(1) {}
            mutable std::mutex mutex_;
            std::unordered_map<size_t, std::shared_ptr<SadFuture>> futures_;
            size_t nextId_;
        };

    } // namespace Interpreter
} // namespace Sad
