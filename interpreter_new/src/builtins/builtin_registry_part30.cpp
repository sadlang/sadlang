/**
 * @file builtin_registry_part30.cpp
 * @brief (AR) دوال آلة الحالة ومحلل الأداء
 * @brief (EN) State Machine & Profiler functions
 */

#include "interpreter_core.h"
#include "value.h"
#include "graphics/sad_state_profiler.h"

#include <vector>
#include <memory>
#include <string>

namespace Sad {
namespace Interpreter {

void registerBuiltinsPart30(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // =================================================================
    // آلة الحالة / State Machine
    // =================================================================

    // 1. statemachine_create / حالة_جديد
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            std::string initial = args.size() > 0 ? args[0]->toString() : "idle";
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::statemachine_create_impl(initial)));
        };
        fm.registerBuiltinFunction("statemachine_create", f);
        fm.registerBuiltinFunction("\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", f); // حالة_جديد
    }

    // 2. statemachine_destroy / حالة_تدمير
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::statemachine_destroy_impl(id);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("statemachine_destroy", f);
        fm.registerBuiltinFunction("\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9_\xd8\xaa\xd8\xaf\xd9\x85\xd9\x8a\xd8\xb1", f); // حالة_تدمير
    }

    // 3. statemachine_add_state / حالة_اضف
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string name = args.size() > 1 ? args[1]->toString() : "";
            sad::stdlib::graphics::statemachine_add_state_impl(id, name);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("statemachine_add_state", f);
        fm.registerBuiltinFunction("\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9_\xd8\xa7\xd8\xb6\xd9\x81", f); // حالة_اضف
    }

    // 4. statemachine_add_transition / حالة_اضف_انتقال
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string from = args.size() > 1 ? args[1]->toString() : "";
            std::string to = args.size() > 2 ? args[2]->toString() : "";
            std::string trigger = args.size() > 3 ? args[3]->toString() : "";
            sad::stdlib::graphics::statemachine_add_transition_impl(id, from, to, trigger);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("statemachine_add_transition", f);
        fm.registerBuiltinFunction("\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9_\xd8\xa7\xd8\xb6\xd9\x81_\xd8\xa7\xd9\x86\xd8\xaa\xd9\x82\xd8\xa7\xd9\x84", f); // حالة_اضف_انتقال
    }

    // 5. statemachine_trigger / حالة_شغل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string trigger = args.size() > 1 ? args[1]->toString() : "";
            return std::make_shared<Data::Value>(sad::stdlib::graphics::statemachine_trigger_impl(id, trigger));
        };
        fm.registerBuiltinFunction("statemachine_trigger", f);
        fm.registerBuiltinFunction("\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9_\xd8\xb4\xd8\xba\xd9\x84", f); // حالة_شغل
    }

    // 6. statemachine_current / حالة_حالية
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(sad::stdlib::graphics::statemachine_current_impl(id));
        };
        fm.registerBuiltinFunction("statemachine_current", f);
        fm.registerBuiltinFunction("\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9_\xd8\xad\xd8\xa7\xd9\x84\xd9\x8a\xd8\xa9", f); // حالة_حالية
    }

    // 7. statemachine_state_count / حالة_عدد
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::statemachine_state_count_impl(id)));
        };
        fm.registerBuiltinFunction("statemachine_state_count", f);
        fm.registerBuiltinFunction("\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9_\xd8\xb9\xd8\xaf\xd8\xaf", f); // حالة_عدد
    }

    // 8. statemachine_reset / حالة_اعادة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::statemachine_reset_impl(id);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("statemachine_reset", f);
        fm.registerBuiltinFunction("\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9_\xd8\xa7\xd8\xb9\xd8\xa7\xd8\xaf\xd8\xa9", f); // حالة_اعادة
    }

    // =================================================================
    // محلل الأداء / Profiler
    // =================================================================

    // 9. profiler_create / محلل_جديد
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::profiler_create_impl()));
        };
        fm.registerBuiltinFunction("profiler_create", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xad\xd9\x84\xd9\x84_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", f); // محلل_جديد
    }

    // 10. profiler_destroy / محلل_تدمير
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::profiler_destroy_impl(id);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("profiler_destroy", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xad\xd9\x84\xd9\x84_\xd8\xaa\xd8\xaf\xd9\x85\xd9\x8a\xd8\xb1", f); // محلل_تدمير
    }

    // 11. profiler_begin / محلل_بدء
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string name = args.size() > 1 ? args[1]->toString() : "sample";
            sad::stdlib::graphics::profiler_begin_impl(id, name);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("profiler_begin", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xad\xd9\x84\xd9\x84_\xd8\xa8\xd8\xaf\xd8\xa1", f); // محلل_بدء
    }

    // 12. profiler_end / محلل_نهاية
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(sad::stdlib::graphics::profiler_end_impl(id));
        };
        fm.registerBuiltinFunction("profiler_end", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xad\xd9\x84\xd9\x84_\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9", f); // محلل_نهاية
    }

    // 13. profiler_fps / محلل_اطارات
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(sad::stdlib::graphics::profiler_fps_impl(id));
        };
        fm.registerBuiltinFunction("profiler_fps", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xad\xd9\x84\xd9\x84_\xd8\xa7\xd8\xb7\xd8\xa7\xd8\xb1\xd8\xa7\xd8\xaa", f); // محلل_اطارات
    }

    // 14. profiler_frame_time / محلل_وقت_اطار
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(sad::stdlib::graphics::profiler_frame_time_impl(id));
        };
        fm.registerBuiltinFunction("profiler_frame_time", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xad\xd9\x84\xd9\x84_\xd9\x88\xd9\x82\xd8\xaa_\xd8\xa7\xd8\xb7\xd8\xa7\xd8\xb1", f); // محلل_وقت_اطار
    }

    // 15. profiler_memory / محلل_ذاكرة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::profiler_memory_impl(id)));
        };
        fm.registerBuiltinFunction("profiler_memory", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xad\xd9\x84\xd9\x84_\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9", f); // محلل_ذاكرة
    }

    // 16. profiler_peak_memory / محلل_ذروة_ذاكرة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::profiler_peak_memory_impl(id)));
        };
        fm.registerBuiltinFunction("profiler_peak_memory", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xad\xd9\x84\xd9\x84_\xd8\xb0\xd8\xb1\xd9\x88\xd8\xa9_\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9", f); // محلل_ذروة_ذاكرة
    }

    // 17. profiler_reset / محلل_اعادة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::profiler_reset_impl(id);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("profiler_reset", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xad\xd9\x84\xd9\x84_\xd8\xa7\xd8\xb9\xd8\xa7\xd8\xaf\xd8\xa9", f); // محلل_اعادة
    }

} // registerBuiltinsPart30

} // namespace Interpreter
} // namespace Sad
