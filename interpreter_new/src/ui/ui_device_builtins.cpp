/**
 * @file ui_device_builtins.cpp
 * @brief (AR) معلومات الجهاز + المؤشر + الروابط + الوقت + الاهتزاز + السجل + البطارية
 * @brief (EN) Device info, cursor, URL, time, vibration, log, battery
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

void registerUIDeviceBuiltins(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // ═══ شاشة كاملة ═══
    auto fullscreen_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto* bridge = UIBridge::active();
        if (bridge) bridge->toggleFullscreen();
        return std::make_shared<Data::Value>();
    };
    fm.registerBuiltinFunction("\xd8\xb4\xd8\xa7\xd8\xb4\xd8\xa9_\xd9\x83\xd8\xa7\xd9\x85\xd9\x84\xd8\xa9", fullscreen_fn);
    fm.registerBuiltinFunction("toggleFullscreen", fullscreen_fn);

    auto is_fullscreen_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>(false);
        return std::make_shared<Data::Value>(bridge->isFullscreen());
    };
    fm.registerBuiltinFunction("\xd9\x87\xd9\x84_\xd8\xb4\xd8\xa7\xd8\xb4\xd8\xa9_\xd9\x83\xd8\xa7\xd9\x85\xd9\x84\xd8\xa9", is_fullscreen_fn);
    fm.registerBuiltinFunction("isFullscreen", is_fullscreen_fn);

    // ═══ معلومات النافذة ═══
    auto window_size_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto* bridge = UIBridge::active();
        Data::Value::ArrayType arr;
        if (!bridge) {
            arr.push_back(Data::Value(0.0));
            arr.push_back(Data::Value(0.0));
            return std::make_shared<Data::Value>(std::move(arr));
        }
        auto [w, h] = bridge->getWindowSize();
        arr.push_back(Data::Value(static_cast<double>(w)));
        arr.push_back(Data::Value(static_cast<double>(h)));
        return std::make_shared<Data::Value>(std::move(arr));
    };
    fm.registerBuiltinFunction("\xd8\xa3\xd8\xa8\xd8\xb9\xd8\xa7\xd8\xaf_\xd8\xa7\xd9\x84\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9", window_size_fn);
    fm.registerBuiltinFunction("getWindowSize", window_size_fn);

    auto mouse_pos_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto* bridge = UIBridge::active();
        Data::Value::ArrayType arr;
        if (!bridge) {
            arr.push_back(Data::Value(0.0));
            arr.push_back(Data::Value(0.0));
            return std::make_shared<Data::Value>(std::move(arr));
        }
        auto [x, y] = bridge->getMousePosition();
        arr.push_back(Data::Value(static_cast<double>(x)));
        arr.push_back(Data::Value(static_cast<double>(y)));
        return std::make_shared<Data::Value>(std::move(arr));
    };
    fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd9\x82\xd8\xb9_\xd8\xa7\xd9\x84\xd9\x81\xd8\xa3\xd8\xb1\xd8\xa9", mouse_pos_fn);
    fm.registerBuiltinFunction("getMousePosition", mouse_pos_fn);

    auto display_scale_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>(1.0);
        return std::make_shared<Data::Value>(static_cast<double>(bridge->getDisplayScale()));
    };
    fm.registerBuiltinFunction("\xd9\x85\xd9\x82\xd9\x8a\xd8\xa7\xd8\xb3_\xd8\xa7\xd9\x84\xd8\xb9\xd8\xb1\xd8\xb6", display_scale_fn);
    fm.registerBuiltinFunction("getDisplayScale", display_scale_fn);

    // ═══ معلومات الجهاز ═══
    auto os_name_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(bridge->getOSName());
    };
    fm.registerBuiltinFunction("\xd8\xa7\xd8\xb3\xd9\x85_\xd8\xa7\xd9\x84\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85", os_name_fn);
    fm.registerBuiltinFunction("getOSName", os_name_fn);

    auto cpu_count_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>(0.0);
        return std::make_shared<Data::Value>(static_cast<double>(bridge->getCPUCount()));
    };
    fm.registerBuiltinFunction("\xd8\xb9\xd8\xaf\xd8\xaf_\xd8\xa7\xd9\x84\xd9\x85\xd8\xb9\xd8\xa7\xd9\x84\xd8\xac\xd8\xa7\xd8\xaa", cpu_count_fn);
    fm.registerBuiltinFunction("getCPUCount", cpu_count_fn);

    auto ram_size_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>(0.0);
        return std::make_shared<Data::Value>(static_cast<double>(bridge->getRAMSize()));
    };
    fm.registerBuiltinFunction("\xd8\xad\xd8\xac\xd9\x85_\xd8\xa7\xd9\x84\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9", ram_size_fn);
    fm.registerBuiltinFunction("getRAMSize", ram_size_fn);

    auto lang_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(bridge->getLanguage());
    };
    fm.registerBuiltinFunction("\xd9\x84\xd8\xba\xd8\xa9_\xd8\xa7\xd9\x84\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85", lang_fn);
    fm.registerBuiltinFunction("getLanguage", lang_fn);

    // ═══ المؤشر ═══
    auto cursor_style_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.empty()) return std::make_shared<Data::Value>();
        std::string style = args[0]->isString() ? args[0]->toString() : "سهم";
        bridge->setCursorStyle(style);
        return std::make_shared<Data::Value>();
    };
    fm.registerBuiltinFunction("\xd8\xa7\xd9\x84\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1", cursor_style_fn);
    fm.registerBuiltinFunction("setCursorStyle", cursor_style_fn);

    auto show_cursor_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>();
        bool show = args.empty() || (args[0]->isBoolean() ? args[0]->toBool() : true);
        bridge->showCursor(show);
        return std::make_shared<Data::Value>();
    };
    fm.registerBuiltinFunction("\xd8\xa3\xd8\xb8\xd9\x87\xd8\xb1_\xd8\xa7\xd9\x84\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1", show_cursor_fn);
    fm.registerBuiltinFunction("showCursor", show_cursor_fn);

    // ═══ فتح رابط ═══
    auto open_url_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.empty()) return std::make_shared<Data::Value>();
        std::string url = args[0]->isString() ? args[0]->toString() : "";
        bridge->openURL(url);
        return std::make_shared<Data::Value>();
    };
    fm.registerBuiltinFunction("\xd9\x81\xd8\xaa\xd8\xad_\xd8\xb1\xd8\xa7\xd8\xa8\xd8\xb7", open_url_fn);
    fm.registerBuiltinFunction("openURL", open_url_fn);

    // ═══ وقت النظام ═══
    auto timestamp_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>(0.0);
        return std::make_shared<Data::Value>(bridge->getTimestamp());
    };
    fm.registerBuiltinFunction("\xd8\xa7\xd9\x84\xd8\xb7\xd8\xa7\xd8\xa8\xd8\xb9_\xd8\xa7\xd9\x84\xd8\xb2\xd9\x85\xd9\x86\xd9\x8a", timestamp_fn);
    fm.registerBuiltinFunction("getTimestamp", timestamp_fn);

    auto datetime_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(bridge->getDateTime());
    };
    fm.registerBuiltinFunction("\xd8\xa7\xd9\x84\xd8\xaa\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xae_\xd9\x88\xd8\xa7\xd9\x84\xd9\x88\xd9\x82\xd8\xaa", datetime_fn);
    fm.registerBuiltinFunction("getDateTime", datetime_fn);

    // ═══ اهتزاز ═══
    auto vibrate_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>();
        int ms = (!args.empty() && args[0]->isNumeric()) ? static_cast<int>(args[0]->toDouble()) : 100;
        bridge->vibrate(ms);
        return std::make_shared<Data::Value>();
    };
    fm.registerBuiltinFunction("\xd8\xa7\xd9\x87\xd8\xaa\xd8\xb2\xd8\xa7\xd8\xb2", vibrate_fn);
    fm.registerBuiltinFunction("vibrate", vibrate_fn);

    // ═══ سجل التطبيق ═══
    auto log_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.empty()) return std::make_shared<Data::Value>();
        std::string level = "معلومة";
        std::string message;
        if (args.size() >= 2) {
            level = args[0]->isString() ? args[0]->toString() : "معلومة";
            message = args[1]->toString();
        } else {
            message = args[0]->toString();
        }
        bridge->logMessage(level, message);
        return std::make_shared<Data::Value>();
    };
    fm.registerBuiltinFunction("\xd8\xb3\xd8\xac\xd9\x91\xd9\x84", log_fn);
    fm.registerBuiltinFunction("\xd8\xb3\xd8\xac\xd9\x84", log_fn);
    fm.registerBuiltinFunction("logMessage", log_fn);

    // ═══ البطارية ═══
    auto battery_level_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>(-1.0);
        return std::make_shared<Data::Value>(static_cast<double>(bridge->getBatteryLevel()));
    };
    fm.registerBuiltinFunction("\xd9\x85\xd8\xb3\xd8\xaa\xd9\x88\xd9\x89_\xd8\xa7\xd9\x84\xd8\xa8\xd8\xb7\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xa9", battery_level_fn);
    fm.registerBuiltinFunction("getBatteryLevel", battery_level_fn);

    auto charging_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>(false);
        return std::make_shared<Data::Value>(bridge->isBatteryCharging());
    };
    fm.registerBuiltinFunction("\xd9\x87\xd9\x84_\xd9\x8a\xd8\xb4\xd8\xad\xd9\x86", charging_fn);
    fm.registerBuiltinFunction("isBatteryCharging", charging_fn);
}

} // namespace Interpreter
} // namespace Sad
