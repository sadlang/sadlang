/**
 * @file builtin_kernel_memory.cpp
 * @brief (AR) وحدة الذاكرة — إدارة صفحات الذاكرة والكومة والتخصيص
 * @brief (EN) Memory module — page management, heap, allocation
 *
 * @note يتطلب: builtin_common.h أو headers متخصصة
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "interpreter_core.h"
#include "builtin_registry.h"
namespace Kmem = Sad::Builtins::Names::KernelMemory;
#include "value.h"

#include <chrono>
#include <cstring>
#include <sstream>
#include <unordered_map>

namespace Sad {
namespace Interpreter {

// Self-contained stub state for memory simulation in interpreter
// The real allocator (runtime) is for compiled code, not interpreted
namespace {
    struct StubAllocStats {
        size_t total_allocated = 0;
        size_t total_freed = 0;
        size_t current_usage = 0;
        size_t allocation_count = 0;
        size_t deallocation_count = 0;
        size_t peak_usage = 0;
    };
    struct StubGCStats {
        size_t total_collections = 0;
        size_t live_objects = 0;
        size_t total_freed_objects = 0;
        size_t total_freed_bytes = 0;
        size_t heap_size = 0;
        double max_pause_ms = 0.0;
    };
    static StubAllocStats g_allocStats;
    static StubGCStats g_gcStats;
    static size_t g_gcThreshold = 1024 * 1024;
    static bool g_gcEnabled = true;
    static uintptr_t g_nextAddr = 0x10000;
    static std::unordered_map<uintptr_t, size_t> g_allocMap;
}

void registerBuiltinsKernelMemory(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // (1) ذاكرة_خصص / memory_alloc
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx)
            -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.empty())
                // (AR) عودةٌ بقيمةٍ زائفةٍ صامتة ⇒ إجابةٌ معقولةُ الشكلِ خاطئةٌ يقينًا.
                ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            size_t size = static_cast<size_t>(args[0]->toDouble());
            uintptr_t addr = g_nextAddr;
            g_nextAddr += size;
            g_allocMap[addr] = size;
            g_allocStats.allocation_count++;
            g_allocStats.total_allocated += size;
            g_allocStats.current_usage += size;
            if (g_allocStats.current_usage > g_allocStats.peak_usage)
                g_allocStats.peak_usage = g_allocStats.current_usage;
            return std::make_shared<Data::Value>(static_cast<double>(addr));
        };
        fm.registerBuiltinFunction(std::string(Kmem::MEM_0), f); // ذاكرة_خصص
    }

    // (2) ذاكرة_حرر / memory_free
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx)
            -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.empty())
                // (AR) عودةٌ بقيمةٍ زائفةٍ صامتة ⇒ إجابةٌ معقولةُ الشكلِ خاطئةٌ يقينًا.
                ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            uintptr_t addr = static_cast<uintptr_t>(args[0]->toDouble());
            auto it = g_allocMap.find(addr);
            if (it != g_allocMap.end()) {
                g_allocStats.deallocation_count++;
                g_allocStats.total_freed += it->second;
                if (g_allocStats.current_usage >= it->second)
                    g_allocStats.current_usage -= it->second;
                g_allocMap.erase(it);
            }
            return std::make_shared<Data::Value>(true);
        };
        fm.registerBuiltinFunction(std::string(Kmem::MEM_1), f); // ذاكرة_حرر
    }

    // (3) ذاكرة_إحصائيات / memory_stats
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx)
            -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            std::ostringstream oss;
            oss << "{"
                << "\"إجمالي_المخصص\": " << g_allocStats.total_allocated << ", "
                << "\"إجمالي_المحرر\": " << g_allocStats.total_freed << ", "
                << "\"المستخدم_حالياً\": " << g_allocStats.current_usage << ", "
                << "\"عدد_التخصيصات\": " << g_allocStats.allocation_count << ", "
                << "\"عدد_التحريرات\": " << g_allocStats.deallocation_count << ", "
                << "\"ذروة_الاستخدام\": " << g_allocStats.peak_usage
                << "}";
            return std::make_shared<Data::Value>(oss.str());
        };
        fm.registerBuiltinFunction(std::string(Kmem::MEM_2), f); // ذاكرة_إحصائيات
    }

    // (4) ذاكرة_حجم / memory_size
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx)
            -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            return std::make_shared<Data::Value>(static_cast<double>(g_allocStats.current_usage));
        };
        fm.registerBuiltinFunction(std::string(Kmem::MEM_3), f); // ذاكرة_حجم
    }

    // (5) ذاكرة_ذروة / memory_peak
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx)
            -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            return std::make_shared<Data::Value>(static_cast<double>(g_allocStats.peak_usage));
        };
        fm.registerBuiltinFunction(std::string(Kmem::MEM_4), f); // ذاكرة_ذروة
    }

    // (6) ذاكرة_صفر / memory_zero (stub)
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx)
            -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.size() < 2)
                // (AR) عودةٌ بقيمةٍ زائفةٍ صامتة ⇒ إجابةٌ معقولةُ الشكلِ خاطئةٌ يقينًا.
                ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            // Stub: in real impl would zero memory at address
            return std::make_shared<Data::Value>(true);
        };
        fm.registerBuiltinFunction(std::string(Kmem::MEM_5), f); // ذاكرة_صفر
    }

    // (7) انسخ_ذاكرة / memory_copy (stub)
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx)
            -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.size() < 3)
                // (AR) عودةٌ بقيمةٍ زائفةٍ صامتة ⇒ إجابةٌ معقولةُ الشكلِ خاطئةٌ يقينًا.
                ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            // Stub: in real impl would copy memory
            return std::make_shared<Data::Value>(true);
        };
        fm.registerBuiltinFunction(std::string(Kmem::MEM_6), f); // انسخ_ذاكرة
    }

    // (8) ذاكرة_أعد_حجم / memory_resize
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx)
            -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.size() < 2)
                // (AR) عودةٌ بقيمةٍ زائفةٍ صامتة ⇒ إجابةٌ معقولةُ الشكلِ خاطئةٌ يقينًا.
                ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            size_t newSize = static_cast<size_t>(args[1]->toDouble());
            uintptr_t addr = g_nextAddr;
            g_nextAddr += newSize;
            g_allocMap[addr] = newSize;
            g_allocStats.allocation_count++;
            g_allocStats.total_allocated += newSize;
            g_allocStats.current_usage += newSize;
            if (g_allocStats.current_usage > g_allocStats.peak_usage)
                g_allocStats.peak_usage = g_allocStats.current_usage;
            return std::make_shared<Data::Value>(static_cast<double>(addr));
        };
        fm.registerBuiltinFunction(std::string(Kmem::MEM_7), f); // ذاكرة_أعد_حجم
    }

    // (9) ذاكرة_محاذاة / memory_aligned_alloc
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx)
            -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.size() < 2)
                // (AR) عودةٌ بقيمةٍ زائفةٍ صامتة ⇒ إجابةٌ معقولةُ الشكلِ خاطئةٌ يقينًا.
                ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            size_t size = static_cast<size_t>(args[0]->toDouble());
            size_t alignment = static_cast<size_t>(args[1]->toDouble());
            uintptr_t addr = (g_nextAddr + alignment - 1) & ~(alignment - 1);
            g_nextAddr = addr + size;
            g_allocMap[addr] = size;
            g_allocStats.allocation_count++;
            g_allocStats.total_allocated += size;
            g_allocStats.current_usage += size;
            if (g_allocStats.current_usage > g_allocStats.peak_usage)
                g_allocStats.peak_usage = g_allocStats.current_usage;
            return std::make_shared<Data::Value>(static_cast<double>(addr));
        };
        fm.registerBuiltinFunction(std::string(Kmem::MEM_8), f); // ذاكرة_محاذاة
    }

    // (10) ذاكرة_تسرب / memory_check_leaks
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx)
            -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            bool hasLeaks = (g_allocStats.allocation_count > g_allocStats.deallocation_count);
            if (hasLeaks) {
                std::ostringstream oss;
                oss << "تسرب: " << (g_allocStats.allocation_count - g_allocStats.deallocation_count)
                    << " كتلة غير محررة (" << g_allocStats.current_usage << " بايت)";
                return std::make_shared<Data::Value>(oss.str());
            }
            return std::make_shared<Data::Value>(std::string("لا يوجد تسرب"));
        };
        fm.registerBuiltinFunction(std::string(Kmem::MEM_9), f); // ذاكرة_تسرب
    }

    // (11) قمامة_اجمع / gc_collect
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx)
            -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            g_gcStats.total_collections++;
            return std::make_shared<Data::Value>(true);
        };
        fm.registerBuiltinFunction(std::string(Kmem::MEM_10), f); // قمامة_اجمع
    }

    // (12) قمامة_إحصائيات / gc_stats
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx)
            -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            std::ostringstream oss;
            oss << "{"
                << "\"عدد_الجمعات\": " << g_gcStats.total_collections << ", "
                << "\"الكائنات_الحية\": " << g_gcStats.live_objects << ", "
                << "\"الكائنات_المحررة\": " << g_gcStats.total_freed_objects << ", "
                << "\"الذاكرة_المحررة\": " << g_gcStats.total_freed_bytes << ", "
                << "\"حجم_الكومة\": " << g_gcStats.heap_size << ", "
                << "\"أقصى_توقف_مللي\": " << g_gcStats.max_pause_ms
                << "}";
            return std::make_shared<Data::Value>(oss.str());
        };
        fm.registerBuiltinFunction(std::string(Kmem::MEM_11), f); // قمامة_إحصائيات
    }

    // (13) قمامة_فعل / gc_enable
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx)
            -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            g_gcEnabled = true;
            return std::make_shared<Data::Value>(true);
        };
        fm.registerBuiltinFunction(std::string(Kmem::MEM_12), f); // قمامة_فعل
    }

    // (14) قمامة_عطل / gc_disable
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx)
            -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            g_gcEnabled = false;
            return std::make_shared<Data::Value>(true);
        };
        fm.registerBuiltinFunction(std::string(Kmem::MEM_13), f); // قمامة_عطل
    }

    // (15) قمامة_عتبة / gc_threshold
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx)
            -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.empty()) {
                return std::make_shared<Data::Value>(static_cast<double>(g_gcThreshold));
            }
            g_gcThreshold = static_cast<size_t>(args[0]->toDouble());
            return std::make_shared<Data::Value>(static_cast<double>(g_gcThreshold));
        };
        fm.registerBuiltinFunction(std::string(Kmem::MEM_14), f); // قمامة_عتبة
    }

    // (16) قمامة_كائنات_حية / gc_live_objects
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx)
            -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            return std::make_shared<Data::Value>(static_cast<double>(g_gcStats.live_objects));
        };
        fm.registerBuiltinFunction(std::string(Kmem::MEM_15), f); // قمامة_كائنات_حية
    }

    // (17) قمامة_حجم_كومة / gc_heap_size
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx)
            -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            return std::make_shared<Data::Value>(static_cast<double>(g_gcStats.heap_size));
        };
        fm.registerBuiltinFunction(std::string(Kmem::MEM_16), f); // قمامة_حجم_كومة
    }

    // (18) قمامة_أقصى_توقف / gc_max_pause
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx)
            -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            return std::make_shared<Data::Value>(g_gcStats.max_pause_ms);
        };
        fm.registerBuiltinFunction(std::string(Kmem::MEM_17), f); // قمامة_أقصى_توقف
    }

    // (19) ذاكرة_عدد_تخصيصات / memory_alloc_count
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx)
            -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            return std::make_shared<Data::Value>(static_cast<double>(g_allocStats.allocation_count));
        };
        fm.registerBuiltinFunction(std::string(Kmem::MEM_18), f); // ذاكرة_عدد_تخصيصات
    }

    // (20) ذاكرة_إجمالي / memory_total
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx)
            -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            return std::make_shared<Data::Value>(static_cast<double>(g_allocStats.total_allocated));
        };
        fm.registerBuiltinFunction(std::string(Kmem::MEM_19), f); // ذاكرة_إجمالي
    }
}

} // namespace Interpreter
} // namespace Sad
