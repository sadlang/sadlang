/**
 * @file ui_storage_builtins.cpp
 * @brief (AR) نظام التخزين المحلي + الحافظة
 * @brief (EN) Local storage + clipboard system
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

void registerUIStorageBuiltins(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // ═══ الحافظة ═══

    // انسخ(نص)
    auto copy_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.empty()) return std::make_shared<Data::Value>(false);
        std::string text = args[0]->isString() ? args[0]->toString() : "";
        bool ok = bridge->copyToClipboard(text);
        return std::make_shared<Data::Value>(ok);
    };
    fm.registerBuiltinFunction("\xd8\xa7\xd9\x86\xd8\xb3\xd8\xae", copy_fn);
    fm.registerBuiltinFunction("copyToClipboard", copy_fn);

    // الصق()
    auto paste_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(bridge->pasteFromClipboard());
    };
    fm.registerBuiltinFunction("\xd8\xa7\xd9\x84\xd8\xb5\xd9\x82", paste_fn);
    fm.registerBuiltinFunction("pasteFromClipboard", paste_fn);

    // هل_يوجد_نص_منسوخ()
    auto has_clipboard_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>(false);
        return std::make_shared<Data::Value>(bridge->hasClipboardText());
    };
    fm.registerBuiltinFunction("\xd9\x87\xd9\x84_\xd9\x8a\xd9\x88\xd8\xac\xd8\xaf_\xd9\x86\xd8\xb5_\xd9\x85\xd9\x86\xd8\xb3\xd9\x88\xd8\xae", has_clipboard_fn);
    fm.registerBuiltinFunction("hasClipboardText", has_clipboard_fn);

    // ═══ التخزين المحلي ═══

    // خزّن(مفتاح، قيمة)
    auto storage_set_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.size() < 2) return std::make_shared<Data::Value>();
        std::string key = args[0]->isString() ? args[0]->toString() : "";
        std::string val = args[1]->toString();
        bridge->storageSet(key, val);
        return std::make_shared<Data::Value>();
    };
    fm.registerBuiltinFunction("\xd8\xae\xd8\xb2\xd9\x91\xd9\x86", storage_set_fn);
    fm.registerBuiltinFunction("\xd8\xae\xd8\xb2\xd9\x86", storage_set_fn);
    fm.registerBuiltinFunction("storageSet", storage_set_fn);

    // استرجع(مفتاح)
    auto storage_get_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.empty()) return std::make_shared<Data::Value>(std::string(""));
        std::string key = args[0]->isString() ? args[0]->toString() : "";
        return std::make_shared<Data::Value>(bridge->storageGet(key));
    };
    fm.registerBuiltinFunction("\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xb1\xd8\xac\xd8\xb9", storage_get_fn);
    fm.registerBuiltinFunction("storageGet", storage_get_fn);

    // احذف_مخزن(مفتاح)
    auto storage_remove_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.empty()) return std::make_shared<Data::Value>();
        std::string key = args[0]->isString() ? args[0]->toString() : "";
        bridge->storageRemove(key);
        return std::make_shared<Data::Value>();
    };
    fm.registerBuiltinFunction("\xd8\xa7\xd8\xad\xd8\xb0\xd9\x81_\xd9\x85\xd8\xae\xd8\xb2\xd9\x86", storage_remove_fn);
    fm.registerBuiltinFunction("storageRemove", storage_remove_fn);

    // امسح_المخزن()
    auto storage_clear_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto* bridge = UIBridge::active();
        if (bridge) bridge->storageClear();
        return std::make_shared<Data::Value>();
    };
    fm.registerBuiltinFunction("\xd8\xa7\xd9\x85\xd8\xb3\xd8\xad_\xd8\xa7\xd9\x84\xd9\x85\xd8\xae\xd8\xb2\xd9\x86", storage_clear_fn);
    fm.registerBuiltinFunction("storageClear", storage_clear_fn);
}

} // namespace Interpreter
} // namespace Sad
