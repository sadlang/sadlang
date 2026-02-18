/**
 * @file builtin_registry_part8.cpp
 * @brief (AR) جزء من تسجيل الدوال المضمنة
 */

#include "builtins.h"
#include "interpreter_core.h"
#include "io/io_functions.h"
#include "object_instance.h"
#include "type_functions.h"
#include "array_functions.h"
#include "other_functions.h"
#include "string/string_functions.h"
#include "math/math_functions.h"
#include "math/advanced_math.h"
#include "system/system_functions.h"
#include "filesystem/filesystem_module.h"
#include "graphics/graphics_module.h"
#include "graphics/sad_particles.h"
#include "graphics/sad_effects.h"
#include "graphics/sad_physics.h"
#include "graphics/sad_scene3d.h"
#include "graphics/sad_ai.h"
#include "graphics/sad_camera2d.h"
#include "graphics/sad_charts.h"
#include "graphics/sad_audio.h"
#include "graphics/sad_tilemap.h"
#include "graphics/sad_islamic_art.h"
#include "graphics/sad_dhikr.h"
#include "graphics/sad_qibla.h"
#include "graphics/sad_prayer.h"
#include "graphics/sad_hijri.h"
#include "graphics/sad_android.h"
#include "mobile/mobile_module.h"
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <functional>
#include <thread>
#include <mutex>
#include <future>
#include <set>
#include <regex>
#include <numeric>
#include <bitset>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <atomic>
#include <queue>
#include <unordered_map>
#include <condition_variable>

namespace Sad {
namespace Interpreter {

using namespace StdLib;

void registerBuiltinsPart8(Interpreter& interpreter) {
    // ========================================================================
    // Async/Await & Concurrency Builtins (23 functions)
    // Simulated using std::thread, std::mutex, std::condition_variable
    // ========================================================================

    // --- Shared async state ---
    static std::atomic<int64_t> async_task_counter{1};
    static std::mutex async_tasks_mutex;
    static std::unordered_map<int64_t, std::shared_ptr<std::thread>> async_tasks;
    static std::unordered_map<int64_t, std::shared_ptr<Data::Value>> async_results;
    
    static std::atomic<int64_t> async_future_counter{1};
    static std::mutex async_futures_mutex;
    struct AsyncFuture {
        std::mutex mtx;
        std::condition_variable cv;
        std::shared_ptr<Data::Value> value;
        bool resolved = false;
    };
    static std::unordered_map<int64_t, std::shared_ptr<AsyncFuture>> async_futures;
    
    static std::atomic<int64_t> async_channel_counter{1};
    static std::mutex async_channels_mutex;
    struct AsyncChannel {
        std::mutex mtx;
        std::condition_variable cv_send, cv_recv;
        std::queue<std::shared_ptr<Data::Value>> buffer;
        int64_t capacity = 1;
        bool closed = false;
    };
    static std::unordered_map<int64_t, std::shared_ptr<AsyncChannel>> async_channels;
    
    static std::atomic<int64_t> async_mutex_counter{1};
    static std::mutex async_mutexes_mutex;
    static std::unordered_map<int64_t, std::shared_ptr<std::mutex>> async_mutexes;
    
    static std::atomic<int64_t> async_atomic_counter{1};
    static std::mutex async_atomics_mutex;
    static std::unordered_map<int64_t, std::shared_ptr<std::atomic<int64_t>>> async_atomics;

    // 1. spawn / async_spawn
    auto async_spawn_func = [&interpreter](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int64_t taskId = async_task_counter.fetch_add(1);
        std::cout << "[ASYNC-SIM] Spawned task #" << taskId << std::endl;
        return std::make_shared<Data::Value>(static_cast<int>(taskId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x85\xd9\x87\xd9\x85\xd8\xa9", async_spawn_func);
    interpreter.getFunctionManager().registerBuiltinFunction("spawn", async_spawn_func);
    interpreter.getFunctionManager().registerBuiltinFunction("async_spawn", async_spawn_func);

    // 2. await / async_await
    auto async_await_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("await requires 1 argument (task_id)");
        int64_t taskId = 0;
        if (args[0]->isInteger()) taskId = args[0]->toInt();
        else if (args[0]->isDouble()) taskId = static_cast<int64_t>(args[0]->toDouble());
        std::cout << "[ASYNC-SIM] Awaiting task #" << taskId << std::endl;
        // Check if there's a future with this ID
        {
            std::lock_guard<std::mutex> lock(async_futures_mutex);
            auto it = async_futures.find(taskId);
            if (it != async_futures.end()) {
                auto& future = it->second;
                std::unique_lock<std::mutex> flock(future->mtx);
                future->cv.wait(flock, [&]{ return future->resolved; });
                std::cout << "[ASYNC-SIM] Task #" << taskId << " completed" << std::endl;
                return future->value ? future->value : std::make_shared<Data::Value>(0);
            }
        }
        std::cout << "[ASYNC-SIM] Task #" << taskId << " completed (immediate)" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1_\xd9\x85\xd9\x87\xd9\x85\xd8\xa9", async_await_func);
    interpreter.getFunctionManager().registerBuiltinFunction("await", async_await_func);
    interpreter.getFunctionManager().registerBuiltinFunction("async_await", async_await_func);
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1", async_await_func);

    // 3. yield / async_yield
    auto async_yield_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::cout << "[ASYNC-SIM] Yielding execution" << std::endl;
        std::this_thread::yield();
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd9\x86\xd8\xaa\xd8\xac", async_yield_func);
    interpreter.getFunctionManager().registerBuiltinFunction("yield", async_yield_func);
    interpreter.getFunctionManager().registerBuiltinFunction("async_yield", async_yield_func);

    // 4. async_sleep
    auto async_sleep_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("async_sleep requires 1 argument (ms)");
        int64_t ms = 0;
        if (args[0]->isInteger()) ms = args[0]->toInt();
        else if (args[0]->isDouble()) ms = static_cast<int64_t>(args[0]->toDouble());
        std::cout << "[ASYNC-SIM] Sleeping for " << ms << "ms" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd9\x88\xd9\x85_\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86", async_sleep_func);
    interpreter.getFunctionManager().registerBuiltinFunction("async_sleep", async_sleep_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sleep_async", async_sleep_func);

    // 5. create_future
    auto create_future_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int64_t futureId = async_future_counter.fetch_add(1);
        auto future = std::make_shared<AsyncFuture>();
        {
            std::lock_guard<std::mutex> lock(async_futures_mutex);
            async_futures[futureId] = future;
        }
        std::cout << "[ASYNC-SIM] Created future #" << futureId << std::endl;
        return std::make_shared<Data::Value>(static_cast<int>(futureId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84", create_future_func);
    interpreter.getFunctionManager().registerBuiltinFunction("create_future", create_future_func);
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84", create_future_func);

    // 6. resolve_future
    auto resolve_future_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("resolve_future requires 2 args (future_id, value)");
        int64_t futureId = args[0]->toInt();
        std::lock_guard<std::mutex> lock(async_futures_mutex);
        auto it = async_futures.find(futureId);
        if (it != async_futures.end()) {
            auto& future = it->second;
            std::lock_guard<std::mutex> flock(future->mtx);
            future->value = args[1];
            future->resolved = true;
            future->cv.notify_all();
            std::cout << "[ASYNC-SIM] Resolved future #" << futureId << std::endl;
        } else {
            std::cout << "[ASYNC-SIM] Future #" << futureId << " not found" << std::endl;
        }
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd9\x88\xd9\x81_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84", resolve_future_func);
    interpreter.getFunctionManager().registerBuiltinFunction("resolve_future", resolve_future_func);
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xad\xd9\x82\xd9\x82_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84", resolve_future_func);

    // 7. get_future
    auto get_future_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("get_future requires 1 argument (future_id)");
        int64_t futureId = args[0]->toInt();
        std::lock_guard<std::mutex> lock(async_futures_mutex);
        auto it = async_futures.find(futureId);
        if (it != async_futures.end()) {
            auto& future = it->second;
            if (future->resolved && future->value) {
                std::cout << "[ASYNC-SIM] Got future #" << futureId << " value" << std::endl;
                return future->value;
            }
        }
        std::cout << "[ASYNC-SIM] Future #" << futureId << " pending (returning 0)" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xad\xd8\xb5\xd9\x84_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84", get_future_func);
    interpreter.getFunctionManager().registerBuiltinFunction("get_future", get_future_func);

    // 8. create_channel
    auto create_channel_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int64_t chanId = async_channel_counter.fetch_add(1);
        auto chan = std::make_shared<AsyncChannel>();
        if (!args.empty()) {
            chan->capacity = args[0]->isInteger() ? args[0]->toInt() : 1;
        }
        {
            std::lock_guard<std::mutex> lock(async_channels_mutex);
            async_channels[chanId] = chan;
        }
        std::cout << "[ASYNC-SIM] Created channel #" << chanId << " (capacity=" << chan->capacity << ")" << std::endl;
        return std::make_shared<Data::Value>(static_cast<int>(chanId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9", create_channel_func);
    interpreter.getFunctionManager().registerBuiltinFunction("create_channel", create_channel_func);
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9", create_channel_func);

    // 9. channel_send
    auto channel_send_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("channel_send requires 2 args (channel_id, value)");
        int64_t chanId = args[0]->toInt();
        std::lock_guard<std::mutex> lock(async_channels_mutex);
        auto it = async_channels.find(chanId);
        if (it != async_channels.end()) {
            auto& chan = it->second;
            std::lock_guard<std::mutex> clock(chan->mtx);
            chan->buffer.push(args[1]);
            chan->cv_recv.notify_one();
            std::cout << "[ASYNC-SIM] Sent to channel #" << chanId << std::endl;
        } else {
            std::cout << "[ASYNC-SIM] Channel #" << chanId << " not found" << std::endl;
        }
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd8\xb1\xd8\xb3\xd9\x84_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9", channel_send_func);
    interpreter.getFunctionManager().registerBuiltinFunction("channel_send", channel_send_func);
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd8\xb1\xd8\xb3\xd9\x84", channel_send_func);

    // 10. channel_recv
    auto channel_recv_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("channel_recv requires 1 argument (channel_id)");
        int64_t chanId = args[0]->toInt();
        std::lock_guard<std::mutex> lock(async_channels_mutex);
        auto it = async_channels.find(chanId);
        if (it != async_channels.end()) {
            auto& chan = it->second;
            std::lock_guard<std::mutex> clock(chan->mtx);
            if (!chan->buffer.empty()) {
                auto val = chan->buffer.front();
                chan->buffer.pop();
                chan->cv_send.notify_one();
                std::cout << "[ASYNC-SIM] Received from channel #" << chanId << std::endl;
                return val;
            }
            std::cout << "[ASYNC-SIM] Channel #" << chanId << " empty" << std::endl;
        }
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9", channel_recv_func);
    interpreter.getFunctionManager().registerBuiltinFunction("channel_recv", channel_recv_func);
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84", channel_recv_func);

    // 11. channel_close
    auto channel_close_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("channel_close requires 1 argument (channel_id)");
        int64_t chanId = args[0]->toInt();
        std::lock_guard<std::mutex> lock(async_channels_mutex);
        auto it = async_channels.find(chanId);
        if (it != async_channels.end()) {
            it->second->closed = true;
            async_channels.erase(it);
            std::cout << "[ASYNC-SIM] Closed channel #" << chanId << std::endl;
        }
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd8\xba\xd9\x84\xd9\x82_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9", channel_close_func);
    interpreter.getFunctionManager().registerBuiltinFunction("channel_close", channel_close_func);

    // 12. create_mutex
    auto create_mutex_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int64_t mutexId = async_mutex_counter.fetch_add(1);
        auto mtx = std::make_shared<std::mutex>();
        {
            std::lock_guard<std::mutex> lock(async_mutexes_mutex);
            async_mutexes[mutexId] = mtx;
        }
        std::cout << "[ASYNC-SIM] Created mutex #" << mutexId << std::endl;
        return std::make_shared<Data::Value>(static_cast<int>(mutexId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x82\xd9\x81\xd9\x84", create_mutex_func);
    interpreter.getFunctionManager().registerBuiltinFunction("create_mutex", create_mutex_func);
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x82\xd9\x81\xd9\x84", create_mutex_func);

    // 13. mutex_lock
    auto mutex_lock_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("mutex_lock requires 1 argument (mutex_id)");
        int64_t mutexId = args[0]->toInt();
        std::lock_guard<std::mutex> lock(async_mutexes_mutex);
        auto it = async_mutexes.find(mutexId);
        if (it != async_mutexes.end()) {
            it->second->lock();
            std::cout << "[ASYNC-SIM] Locked mutex #" << mutexId << std::endl;
        }
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd9\x81\xd9\x84", mutex_lock_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mutex_lock", mutex_lock_func);
    interpreter.getFunctionManager().registerBuiltinFunction("lock", mutex_lock_func);

    // 14. mutex_unlock
    auto mutex_unlock_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("mutex_unlock requires 1 argument (mutex_id)");
        int64_t mutexId = args[0]->toInt();
        std::lock_guard<std::mutex> lock(async_mutexes_mutex);
        auto it = async_mutexes.find(mutexId);
        if (it != async_mutexes.end()) {
            it->second->unlock();
            std::cout << "[ASYNC-SIM] Unlocked mutex #" << mutexId << std::endl;
        }
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x81\xd8\xaa\xd8\xad_\xd9\x82\xd9\x81\xd9\x84", mutex_unlock_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mutex_unlock", mutex_unlock_func);
    interpreter.getFunctionManager().registerBuiltinFunction("unlock", mutex_unlock_func);

    // 15. thread_spawn
    auto thread_spawn_func = [&interpreter](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int64_t threadId = async_task_counter.fetch_add(1);
        std::cout << "[ASYNC-SIM] Spawned thread #" << threadId << std::endl;
        return std::make_shared<Data::Value>(static_cast<int>(threadId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd8\xae\xd9\x8a\xd8\xb7", thread_spawn_func);
    interpreter.getFunctionManager().registerBuiltinFunction("thread_spawn", thread_spawn_func);
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xae\xd9\x8a\xd8\xb7", thread_spawn_func);

    // 16. thread_join
    auto thread_join_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("thread_join requires 1 argument (thread_id)");
        int64_t threadId = args[0]->toInt();
        std::cout << "[ASYNC-SIM] Joined thread #" << threadId << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x86\xd8\xb6\xd9\x85_\xd8\xae\xd9\x8a\xd8\xb7", thread_join_func);
    interpreter.getFunctionManager().registerBuiltinFunction("thread_join", thread_join_func);

    // 16b. create_atomic — إنشاء متغير ذري
    auto create_atomic_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int64_t initVal = 0;
        if (!args.empty()) initVal = args[0]->toInt();
        int64_t id = async_atomic_counter.fetch_add(1);
        auto atom = std::make_shared<std::atomic<int64_t>>(initVal);
        {
            std::lock_guard<std::mutex> lock(async_atomics_mutex);
            async_atomics[id] = atom;
        }
        std::cout << "[ASYNC-SIM] Created atomic #" << id << " (initial=" << initVal << ")" << std::endl;
        return std::make_shared<Data::Value>(static_cast<int>(id));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd8\xb0\xd8\xb1\xd9\x8a", create_atomic_func);
    interpreter.getFunctionManager().registerBuiltinFunction("create_atomic", create_atomic_func);

    // 17. atomic_load
    auto atomic_load_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("atomic_load requires 1 argument (atomic_id)");
        int64_t atomicId = args[0]->toInt();
        std::lock_guard<std::mutex> lock(async_atomics_mutex);
        auto it = async_atomics.find(atomicId);
        if (it != async_atomics.end()) {
            int64_t val = it->second->load();
            std::cout << "[ASYNC-SIM] Atomic load #" << atomicId << " = " << val << std::endl;
            return std::make_shared<Data::Value>(static_cast<int>(val));
        }
        std::cout << "[ASYNC-SIM] Atomic #" << atomicId << " not found, returning 0" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xad\xd9\x85\xd9\x84_\xd8\xb0\xd8\xb1\xd9\x8a", atomic_load_func);
    interpreter.getFunctionManager().registerBuiltinFunction("atomic_load", atomic_load_func);

    // 18. atomic_store
    auto atomic_store_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("atomic_store requires 2 args (atomic_id, value)");
        int64_t atomicId = args[0]->toInt();
        int64_t value = args[1]->toInt();
        std::lock_guard<std::mutex> lock(async_atomics_mutex);
        auto it = async_atomics.find(atomicId);
        if (it == async_atomics.end()) {
            async_atomics[atomicId] = std::make_shared<std::atomic<int64_t>>(value);
        } else {
            it->second->store(value);
        }
        std::cout << "[ASYNC-SIM] Atomic store #" << atomicId << " = " << value << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xae\xd8\xb2\xd9\x86_\xd8\xb0\xd8\xb1\xd9\x8a", atomic_store_func);
    interpreter.getFunctionManager().registerBuiltinFunction("atomic_store", atomic_store_func);

    // 19. atomic_add
    auto atomic_add_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("atomic_add requires 2 args (atomic_id, value)");
        int64_t atomicId = args[0]->toInt();
        int64_t addVal = args[1]->toInt();
        std::lock_guard<std::mutex> lock(async_atomics_mutex);
        auto it = async_atomics.find(atomicId);
        int64_t oldVal = 0;
        if (it == async_atomics.end()) {
            async_atomics[atomicId] = std::make_shared<std::atomic<int64_t>>(addVal);
        } else {
            oldVal = it->second->fetch_add(addVal);
        }
        std::cout << "[ASYNC-SIM] Atomic add #" << atomicId << " += " << addVal << " (old=" << oldVal << ")" << std::endl;
        return std::make_shared<Data::Value>(static_cast<int>(oldVal));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd8\xb6\xd9\x81_\xd8\xb0\xd8\xb1\xd9\x8a", atomic_add_func);
    interpreter.getFunctionManager().registerBuiltinFunction("atomic_add", atomic_add_func);

    // 20. compare_and_swap / CAS
    auto atomic_cas_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) throw std::runtime_error("CAS requires 3 args (atomic_id, expected, desired)");
        int64_t atomicId = args[0]->toInt();
        int64_t expected = args[1]->toInt();
        int64_t desired = args[2]->toInt();
        std::lock_guard<std::mutex> lock(async_atomics_mutex);
        auto it = async_atomics.find(atomicId);
        if (it != async_atomics.end()) {
            int64_t exp = expected;
            bool success = it->second->compare_exchange_strong(exp, desired);
            std::cout << "[ASYNC-SIM] CAS #" << atomicId << " expected=" << expected << " desired=" << desired << " success=" << success << std::endl;
            return std::make_shared<Data::Value>(success ? 1 : 0);
        }
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x82\xd8\xa7\xd8\xb1\xd9\x86_\xd9\x88\xd8\xa8\xd8\xaf\xd9\x84", atomic_cas_func);
    interpreter.getFunctionManager().registerBuiltinFunction("compare_and_swap", atomic_cas_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cas", atomic_cas_func);
    interpreter.getFunctionManager().registerBuiltinFunction("atomic_cas", atomic_cas_func);

    // 21. wait_all
    auto wait_all_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::cout << "[ASYNC-SIM] Waiting for all " << args.size() << " tasks" << std::endl;
        for (size_t i = 0; i < args.size(); i++) {
            int64_t taskId = args[i]->isInteger() ? args[i]->toInt() : static_cast<int64_t>(args[i]->toDouble());
            std::cout << "[ASYNC-SIM] Task #" << taskId << " completed (simulated)" << std::endl;
        }
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1_\xd8\xa7\xd9\x84\xd9\x83\xd9\x84", wait_all_func);
    interpreter.getFunctionManager().registerBuiltinFunction("wait_all", wait_all_func);

    // 22. wait_any
    auto wait_any_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(-1);
        int64_t taskId = args[0]->toInt();
        std::cout << "[ASYNC-SIM] First task #" << taskId << " completed (wait_any)" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1_\xd8\xa3\xd9\x8a", wait_any_func);
    interpreter.getFunctionManager().registerBuiltinFunction("wait_any", wait_any_func);

    // 23. select / channel_select
    auto channel_select_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::cout << "[ASYNC-SIM] Select on " << args.size() << " channels" << std::endl;
        if (!args.empty()) {
            int64_t chanId = args[0]->toInt();
            return std::make_shared<Data::Value>(static_cast<int>(chanId));
        }
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xae\xd8\xaa\xd8\xb1_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9", channel_select_func);
    interpreter.getFunctionManager().registerBuiltinFunction("select", channel_select_func);
    interpreter.getFunctionManager().registerBuiltinFunction("channel_select", channel_select_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) دوال البرمجة الكائنية المبنية / (EN) OOP Built-in Functions
    // ═══════════════════════════════════════════════════════════════════

    // ─── هو_مثيل(كائن، "اسم_صنف") — instanceof check ───
    // (AR) يتحقق هل الكائن من نوع الصنف المحدد (أو أي صنف أب)
    // (EN) Checks if object is instance of specified class (or any parent class)
    auto instanceof_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>(0);
        if (!args[0]->isObject()) return std::make_shared<Data::Value>(0);
        
        auto obj = args[0]->toObject();
        if (!obj) return std::make_shared<Data::Value>(0);
        
        std::string className = args[1]->toString();
        bool result = obj->isInstanceOf(className);
        return std::make_shared<Data::Value>(result ? 1 : 0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x87\xd9\x88_\xd9\x85\xd8\xab\xd9\x8a\xd9\x84", instanceof_func);  // هو_مثيل
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd8\xab\xd9\x8a\xd9\x84_\xd9\x85\xd9\x86", instanceof_func);    // مثيل_من
    interpreter.getFunctionManager().registerBuiltinFunction("instanceof", instanceof_func);

    // ─── نوع_الكائن(كائن) — get object class name ───
    // (AR) يُرجع اسم صنف الكائن كنص
    // (EN) Returns the class name of an object as string
    auto get_class_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(std::string(""));
        if (!args[0]->isObject()) {
            // (AR) نُرجع نوع القيمة إذا لم يكن كائناً
            // (EN) Return value type if not an object
            using VT = Data::ValueType;
            auto vt = args[0]->getType();
            std::string typeName;
            switch (vt) {
                case VT::INTEGER: typeName = "رقم"; break;
                case VT::DOUBLE:  typeName = "عشري"; break;
                case VT::BOOLEAN: typeName = "منطقي"; break;
                case VT::STRING:  typeName = "نص"; break;
                case VT::ARRAY:   typeName = "مصفوفة"; break;
                case VT::MAP:     typeName = "قاموس"; break;
                default:          typeName = "مجهول"; break;
            }
            return std::make_shared<Data::Value>(typeName);
        }
        auto obj = args[0]->toObject();
        if (!obj) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(obj->getClassName());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd9\x88\xd8\xb9_\xd8\xa7\xd9\x84\xd9\x83\xd8\xa7\xd8\xa6\xd9\x86", get_class_func);  // نوع_الكائن
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd9\x88\xd8\xb9", get_class_func);  // نوع
    interpreter.getFunctionManager().registerBuiltinFunction("typeof", get_class_func);
    interpreter.getFunctionManager().registerBuiltinFunction("classof", get_class_func);

    // ─── حقول_الكائن(كائن) — get object fields as map ───
    // (AR) يُرجع قاموس بحقول الكائن
    // (EN) Returns a map of object fields
    auto get_fields_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty() || !args[0]->isObject()) return std::make_shared<Data::Value>();
        auto obj = args[0]->toObject();
        if (!obj) return std::make_shared<Data::Value>();
        
        Data::Value::MapType result;
        for (auto it = obj->fields.begin(); it != obj->fields.end(); ++it) {
            if (it->first.find("__") == 0) continue;
            result[it->first] = it->second;
        }
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xad\xd9\x82\xd9\x88\xd9\x84_\xd8\xa7\xd9\x84\xd9\x83\xd8\xa7\xd8\xa6\xd9\x86", get_fields_func);  // حقول_الكائن
    interpreter.getFunctionManager().registerBuiltinFunction("fields", get_fields_func);


}

} // namespace Interpreter
} // namespace Sad
