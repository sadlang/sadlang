/**
 * @file ui_timer_builtins.cpp
 * @brief (AR) نظام المؤقتات — setTimeout, setInterval, clearTimer
 * @brief (EN) Timer system — setTimeout, setInterval, clearTimer
 */

#include "ui_module_builtins.h"
#include "ui_bridge.h"
#include "builtins.h"
#include "interpreter_core.h"
#include <memory>
#include <string>
#include <vector>

namespace Sad {
namespace Interpreter {

void registerUITimerBuiltins(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // بعد_مدة(دالة_رد، مدة_بالمللي_ثانية) — setTimeout
    auto timeout_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.size() < 2) return std::make_shared<Data::Value>();
        int delay = args[1]->isNumeric() ? static_cast<int>(args[1]->toDouble()) : 1000;
        int id = bridge->scheduleTimeout(*args[0], delay);
        return std::make_shared<Data::Value>(static_cast<double>(id));
    };
    fm.registerBuiltinFunction("\xd8\xa8\xd8\xb9\xd8\xaf_\xd9\x85\xd8\xaf\xd8\xa9", timeout_fn);
    fm.registerBuiltinFunction("setTimeout", timeout_fn);

    // كل_مدة(دالة_رد، مدة_بالمللي_ثانية) — setInterval
    auto interval_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.size() < 2) return std::make_shared<Data::Value>();
        int interval = args[1]->isNumeric() ? static_cast<int>(args[1]->toDouble()) : 1000;
        int id = bridge->scheduleInterval(*args[0], interval);
        return std::make_shared<Data::Value>(static_cast<double>(id));
    };
    fm.registerBuiltinFunction("\xd9\x83\xd9\x84_\xd9\x85\xd8\xaf\xd8\xa9", interval_fn);
    fm.registerBuiltinFunction("setInterval", interval_fn);

    // أوقف_مؤقت(معرّف) — clearTimer
    auto cancel_timer_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.empty()) return std::make_shared<Data::Value>();
        int id = args[0]->isNumeric() ? static_cast<int>(args[0]->toDouble()) : 0;
        bridge->cancelTimer(id);
        return std::make_shared<Data::Value>();
    };
    fm.registerBuiltinFunction("\xd8\xa3\xd9\x88\xd9\x82\xd9\x81_\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa", cancel_timer_fn);
    fm.registerBuiltinFunction("clearTimer", cancel_timer_fn);
}

} // namespace Interpreter
} // namespace Sad
