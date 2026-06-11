/**
 * @file ui_storage_builtins.cpp
 * @brief (AR) نظام التخزين المحلي + الحافظة
 * @brief (EN) Local storage + clipboard system
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

        void registerUIStorageBuiltins(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // (AR) اختصار لأسماء ثوابت التخزين
            namespace Bs = Builtins::Names::UIStorage;

            // ═══ الحافظة ═══

            // انسخ(نص)
            auto copy_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>(false);
                std::string text = args[0]->isString() ? args[0]->toString() : "";
                bool ok = bridge->copyToClipboard(text);
                return std::make_shared<Data::Value>(ok);
            };
            fm.registerBuiltinFunction(std::string(Bs::COPY), copy_fn);

            // الصق()
            auto paste_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(std::string(""));
                return std::make_shared<Data::Value>(bridge->pasteFromClipboard());
            };
            fm.registerBuiltinFunction(std::string(Bs::PASTE), paste_fn);

            // هل_يوجد_نص_منسوخ()
            auto has_clipboard_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(false);
                return std::make_shared<Data::Value>(bridge->hasClipboardText());
            };
            fm.registerBuiltinFunction(std::string(Bs::HAS_CLIPBOARD), has_clipboard_fn);

            // ═══ التخزين المحلي ═══

            // خزّن(مفتاح، قيمة)
            auto storage_set_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge || args.size() < 2)
                    return std::make_shared<Data::Value>();
                std::string key = args[0]->isString() ? args[0]->toString() : "";
                std::string val = args[1]->toString();
                bridge->storageSet(key, val);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bs::STORE), storage_set_fn);
            // استرجع(مفتاح)
            auto storage_get_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>(std::string(""));
                std::string key = args[0]->isString() ? args[0]->toString() : "";
                return std::make_shared<Data::Value>(bridge->storageGet(key));
            };
            fm.registerBuiltinFunction(std::string(Bs::RETRIEVE), storage_get_fn);
            // احذف_مخزن(مفتاح)
            auto storage_remove_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>();
                std::string key = args[0]->isString() ? args[0]->toString() : "";
                bridge->storageRemove(key);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bs::REMOVE_STORED), storage_remove_fn);
            // امسح_المخزن()
            auto storage_clear_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->storageClear();
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bs::CLEAR_STORE), storage_clear_fn);
        }

    } // namespace Interpreter
} // namespace Sad
