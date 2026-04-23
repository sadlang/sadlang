/**
 * @file ui_timer_builtins.cpp
 * @brief (AR) نظام المؤقتات — setTimeout, setInterval, clearTimer
 * @brief (EN) Timer system — setTimeout, setInterval, clearTimer
 */

#include "ui_module_builtins.h"
#include "ui_bridge.h"
#include "builtins.h"
#include "interpreter_core.h"
#include "builtin_registry.h"
#include <memory>
#include <string>
#include <vector>

namespace Sad
{
    namespace Interpreter
    {

        void registerUITimerBuiltins(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // (AR) اختصار لأسماء ثوابت المؤقتات
            namespace Btm = Builtins::Names::UITimer;

            // بعد_مدة(دالة_رد، مدة_بالمللي_ثانية) — setTimeout
            auto timeout_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.size() < 2)
                    return std::make_shared<Data::Value>();
                int delay = args[1]->isNumeric() ? static_cast<int>(args[1]->toDouble()) : 1000;
                int id = bridge->scheduleTimeout(*args[0], delay);
                return std::make_shared<Data::Value>(static_cast<double>(id));
            };
            fm.registerBuiltinFunction(std::string(Btm::TIMEOUT), timeout_fn);

            // كل_مدة(دالة_رد، مدة_بالمللي_ثانية) — setInterval
            auto interval_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.size() < 2)
                    return std::make_shared<Data::Value>();
                int interval = args[1]->isNumeric() ? static_cast<int>(args[1]->toDouble()) : 1000;
                int id = bridge->scheduleInterval(*args[0], interval);
                return std::make_shared<Data::Value>(static_cast<double>(id));
            };
            fm.registerBuiltinFunction(std::string(Btm::INTERVAL), interval_fn);

            // أوقف_مؤقت(معرّف) — clearTimer
            auto cancel_timer_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>();
                int id = args[0]->isNumeric() ? static_cast<int>(args[0]->toDouble()) : 0;
                bridge->cancelTimer(id);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Btm::CANCEL_TIMER), cancel_timer_fn);
        }

    } // namespace Interpreter
} // namespace Sad
