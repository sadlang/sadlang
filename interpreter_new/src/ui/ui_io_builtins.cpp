/**
 * @file ui_io_builtins.cpp
 * @brief (AR) نظام الملفات + اختيار الملفات + لقطة الشاشة + سحب وإفلات
 * @brief (EN) File I/O + file picker + screenshot + drag & drop
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

        void registerUIIOBuiltins(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // (AR) اختصار لأسماء ثوابت الملفات
            namespace Bio = Builtins::Names::UIIO;

            // ═══ سحب وإفلات ═══
            auto drop_handler_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>();
                bridge->setDropHandler(*args[0]);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bio::ON_DROP), drop_handler_fn);

            // ═══ لقطة الشاشة ═══
            auto screenshot_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>(false);
                std::string path = args[0]->isString() ? args[0]->toString() : "screenshot.bmp";
                bool ok = bridge->takeScreenshot(path);
                return std::make_shared<Data::Value>(ok);
            };
            fm.registerBuiltinFunction(std::string(Bio::SCREENSHOT), screenshot_fn);

            // ═══ الملفات ═══

            // اقرأ_ملف(مسار) → نص
            auto read_file_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>(std::string(""));
                std::string path = args[0]->isString() ? args[0]->toString() : "";
                return std::make_shared<Data::Value>(bridge->readTextFile(path));
            };
            fm.registerBuiltinFunction(std::string(Bio::READ_FILE), read_file_fn);

            // اكتب_ملف(مسار، محتوى)
            auto write_file_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.size() < 2)
                    return std::make_shared<Data::Value>(false);
                std::string path = args[0]->isString() ? args[0]->toString() : "";
                std::string content = args[1]->toString();
                return std::make_shared<Data::Value>(bridge->writeTextFile(path, content));
            };
            fm.registerBuiltinFunction(std::string(Bio::WRITE_FILE), write_file_fn);

            // هل_ملف_موجود(مسار)
            auto file_exists_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>(false);
                std::string path = args[0]->isString() ? args[0]->toString() : "";
                return std::make_shared<Data::Value>(bridge->fileExists(path));
            };
            fm.registerBuiltinFunction(std::string(Bio::FILE_EXISTS), file_exists_fn);

            // ═══ اختيار الملفات ═══

            // اختر_ملف(فلتر) → نص
            auto file_picker_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(std::string(""));
                std::string filter = (!args.empty() && args[0]->isString()) ? args[0]->toString() : "";
                return std::make_shared<Data::Value>(bridge->openFilePicker(filter));
            };
            fm.registerBuiltinFunction(std::string(Bio::FILE_PICKER), file_picker_fn);

            // حفظ_ملف(اسم_افتراضي) → نص
            auto save_picker_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(std::string(""));
                std::string name = (!args.empty() && args[0]->isString()) ? args[0]->toString() : "";
                return std::make_shared<Data::Value>(bridge->saveFilePicker(name));
            };
            fm.registerBuiltinFunction(std::string(Bio::SAVE_PICKER), save_picker_fn);
        }

    } // namespace Interpreter
} // namespace Sad
