/**
 * @file ui_dialog_builtins.cpp
 * @brief (AR) نظام الحوارات + الإشعارات + الرسائل المنبثقة
 * @brief (EN) Dialog + notification + snackbar system
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

        void registerUIDialogBuiltins(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // (AR) اختصار لأسماء ثوابت الحوارات
            namespace Bd = Builtins::Names::UIDialog;

            // حوار_تنبيه(عنوان، رسالة)
            auto alert_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge || args.size() < 2)
                    return std::make_shared<Data::Value>();
                std::string title = args[0]->isString() ? args[0]->toString() : "";
                std::string msg = args[1]->isString() ? args[1]->toString() : "";
                bridge->showAlert(title, msg);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bd::ALERT), alert_fn);

            // حوار_تأكيد(عنوان، رسالة) → منطقي
            auto confirm_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge || args.size() < 2)
                    return std::make_shared<Data::Value>(false);
                std::string title = args[0]->isString() ? args[0]->toString() : "";
                std::string msg = args[1]->isString() ? args[1]->toString() : "";
                bool result = bridge->showConfirm(title, msg);
                return std::make_shared<Data::Value>(result);
            };
            fm.registerBuiltinFunction(std::string(Bd::CONFIRM), confirm_fn);

            // أظهر_رسالة(نص, [مدة])
            auto show_snackbar_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isString())
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }
                std::string message = args[0]->toString();
                int durationMs = 3000;
                if (args.size() > 1 && args[1]->isNumeric())
                {
                    durationMs = static_cast<int>(args[1]->toDouble());
                }
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->showSnackbar(message, durationMs);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bd::SHOW_SNACKBAR), show_snackbar_fn);

            // أظهر_إشعار(عنوان، محتوى)
            auto notification_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge || args.size() < 2)
                    return std::make_shared<Data::Value>();
                std::string title = args[0]->isString() ? args[0]->toString() : "";
                std::string body = args[1]->isString() ? args[1]->toString() : "";
                bridge->showNotification(title, body);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bd::NOTIFICATION), notification_fn);
        }

    } // namespace Interpreter
} // namespace Sad
