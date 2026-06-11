/**
 * @file builtin_kernel_threads.cpp
 * @brief (AR) وحدة الخيوط — جدولة المهام وإدارة السياقات
 * @brief (EN) Threads module — task scheduling and context management
 *
 * @note يتطلب: builtin_common.h أو headers متخصصة
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "interpreter_core.h"
#include "builtin_registry.h"
#include "value.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <sstream>
#include <thread>
#include <unordered_map>

namespace Sad
{
    namespace Interpreter
    {

        // Self-contained stub state for threading simulation
        namespace
        {
            enum class StubThreadState
            {
                Created,
                Running,
                Blocked,
                Sleeping,
                Finished
            };

            struct StubThread
            {
                std::unique_ptr<std::thread> thread;
                StubThreadState state = StubThreadState::Created;
            };

            struct StubSemaphore
            {
                std::mutex mtx;
                std::condition_variable cv;
                uint32_t count;
                StubSemaphore(uint32_t c = 1) : count(c) {}
                void wait()
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    cv.wait(lock, [this]
                            { return count > 0; });
                    --count;
                }
                void signal()
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    ++count;
                    cv.notify_one();
                }
                bool try_wait()
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    if (count > 0)
                    {
                        --count;
                        return true;
                    }
                    return false;
                }
            };

            static std::unordered_map<uint64_t, std::unique_ptr<StubThread>> g_threads;
            static std::unordered_map<uint64_t, std::unique_ptr<std::mutex>> g_mutexes;
            static std::unordered_map<uint64_t, std::unique_ptr<StubSemaphore>> g_semaphores;
            static std::atomic<uint64_t> g_nextId{1};
        }

        void registerBuiltinsKernelThreads(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // (AR) اختصار لأسماء ثوابت الخيوط
            namespace Bth = Builtins::Names::KernelThreads;

            // (1) خيط_جديد / thread_new
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    uint64_t id = g_nextId++;
                    g_threads[id] = std::make_unique<StubThread>();
                    return std::make_shared<Data::Value>(static_cast<double>(id));
                };
                fm.registerBuiltinFunction(std::string(Bth::THREAD_NEW), f); // خيط_جديد
            }

            // (2) خيط_ابدأ / thread_start (stub — actual thread execution not yet supported)
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 2)
                        return std::make_shared<Data::Value>(false);
                    uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
                    auto it = g_threads.find(id);
                    if (it == g_threads.end())
                        return std::make_shared<Data::Value>(false);
                    // Stub: mark thread as running but don't actually spawn
                    // (full thread execution requires interpreter re-entrance support)
                    it->second->state = StubThreadState::Running;
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bth::THREAD_START), f); // خيط_ابدأ
            }

            // (3) خيط_انتظر / thread_join
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
                    auto it = g_threads.find(id);
                    if (it == g_threads.end())
                        return std::make_shared<Data::Value>(false);
                    if (it->second->thread && it->second->thread->joinable())
                        it->second->thread->join();
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bth::THREAD_WAIT), f); // خيط_انتظر
            }

            // (4) خيط_حالة / thread_state
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(std::string("غير موجود"));
                    uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
                    auto it = g_threads.find(id);
                    if (it == g_threads.end())
                        return std::make_shared<Data::Value>(std::string("غير موجود"));
                    switch (it->second->state)
                    {
                    case StubThreadState::Created:
                        return std::make_shared<Data::Value>(std::string("منشأ"));
                    case StubThreadState::Running:
                        return std::make_shared<Data::Value>(std::string("يعمل"));
                    case StubThreadState::Blocked:
                        return std::make_shared<Data::Value>(std::string("محجوب"));
                    case StubThreadState::Sleeping:
                        return std::make_shared<Data::Value>(std::string("نائم"));
                    case StubThreadState::Finished:
                        return std::make_shared<Data::Value>(std::string("منتهي"));
                    default:
                        return std::make_shared<Data::Value>(std::string("مجهول"));
                    }
                };
                fm.registerBuiltinFunction(std::string(Bth::THREAD_STATUS), f); // خيط_حالة
            }

            // (5) خيط_نوم / thread_sleep
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    int ms = static_cast<int>(args[0]->toDouble());
                    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bth::THREAD_SLEEP), f); // خيط_نوم
            }

            // (6) خيط_معرف / thread_id
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    auto id = std::this_thread::get_id();
                    std::ostringstream oss;
                    oss << id;
                    return std::make_shared<Data::Value>(oss.str());
                };
                fm.registerBuiltinFunction(std::string(Bth::THREAD_ID), f); // خيط_معرف
            }

            // (7) خيط_عدد_معالجات / thread_cpu_count
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    return std::make_shared<Data::Value>(static_cast<double>(std::thread::hardware_concurrency()));
                };
                fm.registerBuiltinFunction(std::string(Bth::THREAD_CPU_COUNT), f); // خيط_عدد_معالجات
            }

            // (8) قفل_جديد / mutex_new
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    uint64_t id = g_nextId++;
                    g_mutexes[id] = std::make_unique<std::mutex>();
                    return std::make_shared<Data::Value>(static_cast<double>(id));
                };
                fm.registerBuiltinFunction(std::string(Bth::LOCK_NEW), f); // قفل_جديد
            }

            // (9) قفل_أقفل / mutex_lock
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
                    auto it = g_mutexes.find(id);
                    if (it == g_mutexes.end())
                        return std::make_shared<Data::Value>(false);
                    it->second->lock();
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bth::LOCK_ACQUIRE), f); // قفل_أقفل
            }

            // (10) قفل_حرر / mutex_unlock
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
                    auto it = g_mutexes.find(id);
                    if (it == g_mutexes.end())
                        return std::make_shared<Data::Value>(false);
                    it->second->unlock();
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bth::LOCK_RELEASE), f); // قفل_حرر
            }

            // (11) قفل_حاول / mutex_try_lock
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
                    auto it = g_mutexes.find(id);
                    if (it == g_mutexes.end())
                        return std::make_shared<Data::Value>(false);
                    return std::make_shared<Data::Value>(it->second->try_lock());
                };
                fm.registerBuiltinFunction(std::string(Bth::LOCK_TRY), f); // قفل_حاول
            }

            // (12) قفل_احذف / mutex_destroy
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
                    return std::make_shared<Data::Value>(g_mutexes.erase(id) > 0);
                };
                fm.registerBuiltinFunction(std::string(Bth::LOCK_DELETE), f); // قفل_احذف
            }

            // (13) إشارة_جديدة / semaphore_new
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    uint32_t count = args.empty() ? 1 : static_cast<uint32_t>(args[0]->toDouble());
                    uint64_t id = g_nextId++;
                    g_semaphores[id] = std::make_unique<StubSemaphore>(count);
                    return std::make_shared<Data::Value>(static_cast<double>(id));
                };
                fm.registerBuiltinFunction(std::string(Bth::SEM_NEW), f); // إشارة_جديدة
            }

            // (14) إشارة_انتظر / semaphore_wait
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
                    auto it = g_semaphores.find(id);
                    if (it == g_semaphores.end())
                        return std::make_shared<Data::Value>(false);
                    it->second->wait();
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bth::SEM_WAIT), f); // إشارة_انتظر
            }

            // (15) إشارة_أطلق / semaphore_signal
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
                    auto it = g_semaphores.find(id);
                    if (it == g_semaphores.end())
                        return std::make_shared<Data::Value>(false);
                    it->second->signal();
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bth::SEM_POST), f); // إشارة_أطلق
            }

            // (16) إشارة_حاول / semaphore_try_wait
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
                    auto it = g_semaphores.find(id);
                    if (it == g_semaphores.end())
                        return std::make_shared<Data::Value>(false);
                    return std::make_shared<Data::Value>(it->second->try_wait());
                };
                fm.registerBuiltinFunction(std::string(Bth::SEM_TRY), f); // إشارة_حاول
            }

            // (17) خيط_قائمة / thread_list
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    std::vector<Data::Value> ids;
                    for (auto &[id, _] : g_threads)
                    {
                        ids.push_back(Data::Value(static_cast<double>(id)));
                    }
                    return std::make_shared<Data::Value>(ids);
                };
                fm.registerBuiltinFunction(std::string(Bth::THREAD_LIST), f); // خيط_قائمة
            }

            // (18) خيط_احذف / thread_destroy
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
                    return std::make_shared<Data::Value>(g_threads.erase(id) > 0);
                };
                fm.registerBuiltinFunction(std::string(Bth::THREAD_DELETE), f); // خيط_احذف
            }

            // (19) خيط_تنازل / thread_yield
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    std::this_thread::yield();
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bth::THREAD_YIELD), f); // خيط_تنازل
            }

            // (20) خيط_عدد / thread_count
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    return std::make_shared<Data::Value>(static_cast<double>(g_threads.size()));
                };
                fm.registerBuiltinFunction(std::string(Bth::THREAD_COUNT), f); // خيط_عدد
            }
        }

    } // namespace Interpreter
} // namespace Sad
