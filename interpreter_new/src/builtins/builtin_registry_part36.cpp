/**
 * @file builtin_registry_part36.cpp
 * @brief (AR) وحدة الخيوط — محاكاة نظام الخيوط
 * @brief (EN) Threading module — self-contained stub simulation
 */

#include "interpreter_core.h"
#include "value.h"

#include <sstream>
#include <unordered_map>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <functional>

namespace Sad {
namespace Interpreter {

// Self-contained stub state for threading simulation
namespace {
    enum class StubThreadState { Created, Running, Blocked, Sleeping, Finished };

    struct StubThread {
        std::unique_ptr<std::thread> thread;
        StubThreadState state = StubThreadState::Created;
    };

    struct StubSemaphore {
        std::mutex mtx;
        std::condition_variable cv;
        uint32_t count;
        StubSemaphore(uint32_t c = 1) : count(c) {}
        void wait() {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this]{ return count > 0; });
            --count;
        }
        void signal() {
            std::lock_guard<std::mutex> lock(mtx);
            ++count;
            cv.notify_one();
        }
        bool try_wait() {
            std::lock_guard<std::mutex> lock(mtx);
            if (count > 0) { --count; return true; }
            return false;
        }
    };

    static std::unordered_map<uint64_t, std::unique_ptr<StubThread>> g_threads;
    static std::unordered_map<uint64_t, std::unique_ptr<std::mutex>> g_mutexes;
    static std::unordered_map<uint64_t, std::unique_ptr<StubSemaphore>> g_semaphores;
    static std::atomic<uint64_t> g_nextId{1};
}

void registerBuiltinsPart36(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // (1) خيط_جديد / thread_new
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            uint64_t id = g_nextId++;
            g_threads[id] = std::make_unique<StubThread>();
            return std::make_shared<Data::Value>(static_cast<double>(id));
        };
        fm.registerBuiltinFunction("\xd8\xae\xd9\x8a\xd8\xb7_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", f); // خيط_جديد
        fm.registerBuiltinFunction("thread_new", f);
    }

    // (2) خيط_ابدأ / thread_start (stub — actual thread execution not yet supported)
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.size() < 2) return std::make_shared<Data::Value>(false);
            uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
            auto it = g_threads.find(id);
            if (it == g_threads.end()) return std::make_shared<Data::Value>(false);
            // Stub: mark thread as running but don't actually spawn
            // (full thread execution requires interpreter re-entrance support)
            it->second->state = StubThreadState::Running;
            return std::make_shared<Data::Value>(true);
        };
        fm.registerBuiltinFunction("\xd8\xae\xd9\x8a\xd8\xb7_\xd8\xa7\xd8\xa8\xd8\xaf\xd8\xa3", f); // خيط_ابدأ
        fm.registerBuiltinFunction("thread_start", f);
    }

    // (3) خيط_انتظر / thread_join
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(false);
            uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
            auto it = g_threads.find(id);
            if (it == g_threads.end()) return std::make_shared<Data::Value>(false);
            if (it->second->thread && it->second->thread->joinable())
                it->second->thread->join();
            return std::make_shared<Data::Value>(true);
        };
        fm.registerBuiltinFunction("\xd8\xae\xd9\x8a\xd8\xb7_\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1", f); // خيط_انتظر
        fm.registerBuiltinFunction("thread_join", f);
    }

    // (4) خيط_حالة / thread_state
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(std::string("غير موجود"));
            uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
            auto it = g_threads.find(id);
            if (it == g_threads.end()) return std::make_shared<Data::Value>(std::string("غير موجود"));
            switch (it->second->state) {
                case StubThreadState::Created:  return std::make_shared<Data::Value>(std::string("منشأ"));
                case StubThreadState::Running:  return std::make_shared<Data::Value>(std::string("يعمل"));
                case StubThreadState::Blocked:  return std::make_shared<Data::Value>(std::string("محجوب"));
                case StubThreadState::Sleeping: return std::make_shared<Data::Value>(std::string("نائم"));
                case StubThreadState::Finished: return std::make_shared<Data::Value>(std::string("منتهي"));
                default: return std::make_shared<Data::Value>(std::string("مجهول"));
            }
        };
        fm.registerBuiltinFunction("\xd8\xae\xd9\x8a\xd8\xb7_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9", f); // خيط_حالة
        fm.registerBuiltinFunction("thread_state", f);
    }

    // (5) خيط_نوم / thread_sleep
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(false);
            int ms = static_cast<int>(args[0]->toDouble());
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            return std::make_shared<Data::Value>(true);
        };
        fm.registerBuiltinFunction("\xd8\xae\xd9\x8a\xd8\xb7_\xd9\x86\xd9\x88\xd9\x85", f); // خيط_نوم
        fm.registerBuiltinFunction("thread_sleep", f);
    }

    // (6) خيط_معرف / thread_id
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            auto id = std::this_thread::get_id();
            std::ostringstream oss;
            oss << id;
            return std::make_shared<Data::Value>(oss.str());
        };
        fm.registerBuiltinFunction("\xd8\xae\xd9\x8a\xd8\xb7_\xd9\x85\xd8\xb9\xd8\xb1\xd9\x81", f); // خيط_معرف
        fm.registerBuiltinFunction("thread_id", f);
    }

    // (7) خيط_عدد_معالجات / thread_cpu_count
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            return std::make_shared<Data::Value>(static_cast<double>(std::thread::hardware_concurrency()));
        };
        fm.registerBuiltinFunction("\xd8\xae\xd9\x8a\xd8\xb7_\xd8\xb9\xd8\xaf\xd8\xaf_\xd9\x85\xd8\xb9\xd8\xa7\xd9\x84\xd8\xac\xd8\xa7\xd8\xaa", f); // خيط_عدد_معالجات
        fm.registerBuiltinFunction("thread_cpu_count", f);
    }

    // (8) قفل_جديد / mutex_new
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            uint64_t id = g_nextId++;
            g_mutexes[id] = std::make_unique<std::mutex>();
            return std::make_shared<Data::Value>(static_cast<double>(id));
        };
        fm.registerBuiltinFunction("\xd9\x82\xd9\x81\xd9\x84_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", f); // قفل_جديد
        fm.registerBuiltinFunction("mutex_new", f);
    }

    // (9) قفل_أقفل / mutex_lock
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(false);
            uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
            auto it = g_mutexes.find(id);
            if (it == g_mutexes.end()) return std::make_shared<Data::Value>(false);
            it->second->lock();
            return std::make_shared<Data::Value>(true);
        };
        fm.registerBuiltinFunction("\xd9\x82\xd9\x81\xd9\x84_\xd8\xa3\xd9\x82\xd9\x81\xd9\x84", f); // قفل_أقفل
        fm.registerBuiltinFunction("mutex_lock", f);
    }

    // (10) قفل_حرر / mutex_unlock
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(false);
            uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
            auto it = g_mutexes.find(id);
            if (it == g_mutexes.end()) return std::make_shared<Data::Value>(false);
            it->second->unlock();
            return std::make_shared<Data::Value>(true);
        };
        fm.registerBuiltinFunction("\xd9\x82\xd9\x81\xd9\x84_\xd8\xad\xd8\xb1\xd8\xb1", f); // قفل_حرر
        fm.registerBuiltinFunction("mutex_unlock", f);
    }

    // (11) قفل_حاول / mutex_try_lock
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(false);
            uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
            auto it = g_mutexes.find(id);
            if (it == g_mutexes.end()) return std::make_shared<Data::Value>(false);
            return std::make_shared<Data::Value>(it->second->try_lock());
        };
        fm.registerBuiltinFunction("\xd9\x82\xd9\x81\xd9\x84_\xd8\xad\xd8\xa7\xd9\x88\xd9\x84", f); // قفل_حاول
        fm.registerBuiltinFunction("mutex_try_lock", f);
    }

    // (12) قفل_احذف / mutex_destroy
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(false);
            uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
            return std::make_shared<Data::Value>(g_mutexes.erase(id) > 0);
        };
        fm.registerBuiltinFunction("\xd9\x82\xd9\x81\xd9\x84_\xd8\xa7\xd8\xad\xd8\xb0\xd9\x81", f); // قفل_احذف
        fm.registerBuiltinFunction("mutex_destroy", f);
    }

    // (13) إشارة_جديدة / semaphore_new
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            uint32_t count = args.empty() ? 1 : static_cast<uint32_t>(args[0]->toDouble());
            uint64_t id = g_nextId++;
            g_semaphores[id] = std::make_unique<StubSemaphore>(count);
            return std::make_shared<Data::Value>(static_cast<double>(id));
        };
        fm.registerBuiltinFunction("\xd8\xa5\xd8\xb4\xd8\xa7\xd8\xb1\xd8\xa9_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf\xd8\xa9", f); // إشارة_جديدة
        fm.registerBuiltinFunction("semaphore_new", f);
    }

    // (14) إشارة_انتظر / semaphore_wait
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(false);
            uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
            auto it = g_semaphores.find(id);
            if (it == g_semaphores.end()) return std::make_shared<Data::Value>(false);
            it->second->wait();
            return std::make_shared<Data::Value>(true);
        };
        fm.registerBuiltinFunction("\xd8\xa5\xd8\xb4\xd8\xa7\xd8\xb1\xd8\xa9_\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1", f); // إشارة_انتظر
        fm.registerBuiltinFunction("semaphore_wait", f);
    }

    // (15) إشارة_أطلق / semaphore_signal
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(false);
            uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
            auto it = g_semaphores.find(id);
            if (it == g_semaphores.end()) return std::make_shared<Data::Value>(false);
            it->second->signal();
            return std::make_shared<Data::Value>(true);
        };
        fm.registerBuiltinFunction("\xd8\xa5\xd8\xb4\xd8\xa7\xd8\xb1\xd8\xa9_\xd8\xa3\xd8\xb7\xd9\x84\xd9\x82", f); // إشارة_أطلق
        fm.registerBuiltinFunction("semaphore_signal", f);
    }

    // (16) إشارة_حاول / semaphore_try_wait
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(false);
            uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
            auto it = g_semaphores.find(id);
            if (it == g_semaphores.end()) return std::make_shared<Data::Value>(false);
            return std::make_shared<Data::Value>(it->second->try_wait());
        };
        fm.registerBuiltinFunction("\xd8\xa5\xd8\xb4\xd8\xa7\xd8\xb1\xd8\xa9_\xd8\xad\xd8\xa7\xd9\x88\xd9\x84", f); // إشارة_حاول
        fm.registerBuiltinFunction("semaphore_try_wait", f);
    }

    // (17) خيط_قائمة / thread_list
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            std::vector<Data::Value> ids;
            for (auto& [id, _] : g_threads) {
                ids.push_back(Data::Value(static_cast<double>(id)));
            }
            return std::make_shared<Data::Value>(ids);
        };
        fm.registerBuiltinFunction("\xd8\xae\xd9\x8a\xd8\xb7_\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9", f); // خيط_قائمة
        fm.registerBuiltinFunction("thread_list", f);
    }

    // (18) خيط_احذف / thread_destroy
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(false);
            uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
            return std::make_shared<Data::Value>(g_threads.erase(id) > 0);
        };
        fm.registerBuiltinFunction("\xd8\xae\xd9\x8a\xd8\xb7_\xd8\xa7\xd8\xad\xd8\xb0\xd9\x81", f); // خيط_احذف
        fm.registerBuiltinFunction("thread_destroy", f);
    }

    // (19) خيط_تنازل / thread_yield
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            std::this_thread::yield();
            return std::make_shared<Data::Value>(true);
        };
        fm.registerBuiltinFunction("\xd8\xae\xd9\x8a\xd8\xb7_\xd8\xaa\xd9\x86\xd8\xa7\xd8\xb2\xd9\x84", f); // خيط_تنازل
        fm.registerBuiltinFunction("thread_yield", f);
    }

    // (20) خيط_عدد / thread_count
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            return std::make_shared<Data::Value>(static_cast<double>(g_threads.size()));
        };
        fm.registerBuiltinFunction("\xd8\xae\xd9\x8a\xd8\xb7_\xd8\xb9\xd8\xaf\xd8\xaf", f); // خيط_عدد
        fm.registerBuiltinFunction("thread_count", f);
    }
}

} // namespace Interpreter
} // namespace Sad
