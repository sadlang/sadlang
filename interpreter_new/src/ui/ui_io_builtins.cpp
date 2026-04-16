/**
 * @file ui_io_builtins.cpp
 * @brief (AR) نظام الملفات + اختيار الملفات + لقطة الشاشة + سحب وإفلات
 * @brief (EN) File I/O + file picker + screenshot + drag & drop
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

void registerUIIOBuiltins(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // ═══ سحب وإفلات ═══
    auto drop_handler_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.empty()) return std::make_shared<Data::Value>();
        bridge->setDropHandler(*args[0]);
        return std::make_shared<Data::Value>();
    };
    fm.registerBuiltinFunction("\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xa5\xd8\xb3\xd9\x82\xd8\xa7\xd8\xb7", drop_handler_fn);
    fm.registerBuiltinFunction("onFileDrop", drop_handler_fn);

    // ═══ لقطة الشاشة ═══
    auto screenshot_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.empty()) return std::make_shared<Data::Value>(false);
        std::string path = args[0]->isString() ? args[0]->toString() : "screenshot.bmp";
        bool ok = bridge->takeScreenshot(path);
        return std::make_shared<Data::Value>(ok);
    };
    fm.registerBuiltinFunction("\xd9\x84\xd9\x82\xd8\xb7\xd8\xa9_\xd8\xb4\xd8\xa7\xd8\xb4\xd8\xa9", screenshot_fn);
    fm.registerBuiltinFunction("takeScreenshot", screenshot_fn);

    // ═══ الملفات ═══

    // اقرأ_ملف(مسار) → نص
    auto read_file_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.empty()) return std::make_shared<Data::Value>(std::string(""));
        std::string path = args[0]->isString() ? args[0]->toString() : "";
        return std::make_shared<Data::Value>(bridge->readTextFile(path));
    };
    fm.registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd9\x85\xd9\x84\xd9\x81", read_file_fn);
    fm.registerBuiltinFunction("readTextFile", read_file_fn);

    // اكتب_ملف(مسار، محتوى)
    auto write_file_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.size() < 2) return std::make_shared<Data::Value>(false);
        std::string path = args[0]->isString() ? args[0]->toString() : "";
        std::string content = args[1]->toString();
        return std::make_shared<Data::Value>(bridge->writeTextFile(path, content));
    };
    fm.registerBuiltinFunction("\xd8\xa7\xd9\x83\xd8\xaa\xd8\xa8_\xd9\x85\xd9\x84\xd9\x81", write_file_fn);
    fm.registerBuiltinFunction("writeTextFile", write_file_fn);

    // هل_ملف_موجود(مسار)
    auto file_exists_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.empty()) return std::make_shared<Data::Value>(false);
        std::string path = args[0]->isString() ? args[0]->toString() : "";
        return std::make_shared<Data::Value>(bridge->fileExists(path));
    };
    fm.registerBuiltinFunction("\xd9\x87\xd9\x84_\xd9\x85\xd9\x84\xd9\x81_\xd9\x85\xd9\x88\xd8\xac\xd9\x88\xd8\xaf", file_exists_fn);
    fm.registerBuiltinFunction("fileExists", file_exists_fn);

    // ═══ اختيار الملفات ═══

    // اختر_ملف(فلتر) → نص
    auto file_picker_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>(std::string(""));
        std::string filter = (!args.empty() && args[0]->isString()) ? args[0]->toString() : "";
        return std::make_shared<Data::Value>(bridge->openFilePicker(filter));
    };
    fm.registerBuiltinFunction("\xd8\xa7\xd8\xae\xd8\xaa\xd8\xb1_\xd9\x85\xd9\x84\xd9\x81", file_picker_fn);
    fm.registerBuiltinFunction("openFilePicker", file_picker_fn);

    // حفظ_ملف(اسم_افتراضي) → نص
    auto save_picker_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>(std::string(""));
        std::string name = (!args.empty() && args[0]->isString()) ? args[0]->toString() : "";
        return std::make_shared<Data::Value>(bridge->saveFilePicker(name));
    };
    fm.registerBuiltinFunction("\xd8\xad\xd9\x81\xd8\xb8_\xd9\x85\xd9\x84\xd9\x81", save_picker_fn);
    fm.registerBuiltinFunction("saveFilePicker", save_picker_fn);
}

} // namespace Interpreter
} // namespace Sad
