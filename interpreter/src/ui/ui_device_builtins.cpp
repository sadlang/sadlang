/**
 * @file ui_device_builtins.cpp
 * @brief (AR) معلومات الجهاز + المؤشر + الروابط + الوقت + الاهتزاز + السجل + البطارية
 * @brief (EN) Device info, cursor, URL, time, vibration, log, battery
 */

#include "ui_module_builtins.h"
#include "ui_bridge.h"
#include "builtins.h"
#include "interpreter_core.h"
#include "builtin_registry.h"
#include <memory>
#include <string>
#include <vector>
#include "builtins/builtin_context.h"

namespace Sad
{
    namespace Interpreter
    {

        void registerUIDeviceBuiltins(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // (AR) اختصار لأسماء ثوابت الأجهزة
            namespace Bdev = Builtins::Names::UIDevice;

            // ═══ شاشة كاملة ═══
            auto fullscreen_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->toggleFullscreen();
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bdev::FULLSCREEN), fullscreen_fn);

            auto is_fullscreen_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(false);
                return std::make_shared<Data::Value>(bridge->isFullscreen());
            };
            fm.registerBuiltinFunction(std::string(Bdev::IS_FULLSCREEN), is_fullscreen_fn);

            // ═══ معلومات النافذة ═══
            auto window_size_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                auto *bridge = UIBridge::active();
                Data::Value::ArrayType arr;
                if (!bridge)
                {
                    arr.push_back(Data::Value(0.0));
                    arr.push_back(Data::Value(0.0));
                    return std::make_shared<Data::Value>(std::move(arr));
                }
                auto [w, h] = bridge->getWindowSize();
                arr.push_back(Data::Value(static_cast<double>(w)));
                arr.push_back(Data::Value(static_cast<double>(h)));
                return std::make_shared<Data::Value>(std::move(arr));
            };
            fm.registerBuiltinFunction(std::string(Bdev::WINDOW_SIZE), window_size_fn);

            auto mouse_pos_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                auto *bridge = UIBridge::active();
                Data::Value::ArrayType arr;
                if (!bridge)
                {
                    arr.push_back(Data::Value(0.0));
                    arr.push_back(Data::Value(0.0));
                    return std::make_shared<Data::Value>(std::move(arr));
                }
                auto [x, y] = bridge->getMousePosition();
                arr.push_back(Data::Value(static_cast<double>(x)));
                arr.push_back(Data::Value(static_cast<double>(y)));
                return std::make_shared<Data::Value>(std::move(arr));
            };
            fm.registerBuiltinFunction(std::string(Bdev::MOUSE_POS), mouse_pos_fn);

            auto display_scale_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(1.0);
                return std::make_shared<Data::Value>(static_cast<double>(bridge->getDisplayScale()));
            };
            fm.registerBuiltinFunction(std::string(Bdev::DISPLAY_SCALE), display_scale_fn);

            // ═══ معلومات الجهاز ═══
            auto os_name_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(std::string(""));
                return std::make_shared<Data::Value>(bridge->getOSName());
            };
            fm.registerBuiltinFunction(std::string(Bdev::DEVICE_OS_NAME), os_name_fn);

            auto cpu_count_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(0.0);
                return std::make_shared<Data::Value>(static_cast<double>(bridge->getCPUCount()));
            };
            fm.registerBuiltinFunction(std::string(Bdev::CPU_COUNT), cpu_count_fn);

            auto ram_size_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(0.0);
                return std::make_shared<Data::Value>(static_cast<double>(bridge->getRAMSize()));
            };
            fm.registerBuiltinFunction(std::string(Bdev::RAM_SIZE), ram_size_fn);

            auto lang_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(std::string(""));
                return std::make_shared<Data::Value>(bridge->getLanguage());
            };
            fm.registerBuiltinFunction(std::string(Bdev::SYS_LANG), lang_fn);

            // ═══ المؤشر ═══
            auto cursor_style_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>();
                std::string style = args[0]->isString() ? args[0]->toString() : "سهم";
                bridge->setCursorStyle(style);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bdev::CURSOR_STYLE), cursor_style_fn);

            auto show_cursor_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>();
                bool show = args.empty() || (args[0]->isBoolean() ? args[0]->toBool() : true);
                bridge->showCursor(show);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bdev::SHOW_CURSOR), show_cursor_fn);

            // ═══ فتح رابط ═══
            auto open_url_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>();
                std::string url = args[0]->isString() ? args[0]->toString() : "";
                bridge->openURL(url);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bdev::OPEN_URL), open_url_fn);

            // ═══ وقت النظام ═══
            auto timestamp_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(0.0);
                return std::make_shared<Data::Value>(bridge->getTimestamp());
            };
            fm.registerBuiltinFunction(std::string(Bdev::DEVICE_TIMESTAMP), timestamp_fn);

            auto datetime_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(std::string(""));
                return std::make_shared<Data::Value>(bridge->getDateTime());
            };
            fm.registerBuiltinFunction(std::string(Bdev::DATETIME), datetime_fn);

            // ═══ اهتزاز ═══
            auto vibrate_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>();
                int ms = (!args.empty() && args[0]->isNumeric()) ? static_cast<int>(args[0]->toDouble()) : 100;
                bridge->vibrate(ms);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bdev::VIBRATE), vibrate_fn);

            // ═══ سجل التطبيق ═══
            auto log_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>();
                std::string level = "معلومة";
                std::string message;
                if (args.size() >= 2)
                {
                    level = args[0]->isString() ? args[0]->toString() : "معلومة";
                    message = args[1]->toString();
                }
                else
                {
                    message = args[0]->toString();
                }
                bridge->logMessage(level, message);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bdev::LOG_FN), log_fn);

            // ═══ البطارية ═══
            auto battery_level_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(-1.0);
                return std::make_shared<Data::Value>(static_cast<double>(bridge->getBatteryLevel()));
            };
            fm.registerBuiltinFunction(std::string(Bdev::BATTERY_LEVEL), battery_level_fn);

            auto charging_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(false);
                return std::make_shared<Data::Value>(bridge->isBatteryCharging());
            };
            fm.registerBuiltinFunction(std::string(Bdev::IS_CHARGING), charging_fn);
        }

    } // namespace Interpreter
} // namespace Sad
