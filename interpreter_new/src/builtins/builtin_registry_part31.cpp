/**
 * @file builtin_registry_part31.cpp
 * @brief (AR) دوال التسلسل (JSON) ومدير الموارد
 * @brief (EN) Serialization (JSON) & Resource Manager functions
 */

#include "interpreter_core.h"
#include "value.h"
#include "graphics/sad_serialization_resources.h"

#include <vector>
#include <memory>
#include <string>

namespace Sad {
namespace Interpreter {

void registerBuiltinsPart31(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // =================================================================
    // التسلسل — JSON / Serialization
    // =================================================================

    // 1. json_create / جسن_جديد
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::json_create_impl()));
        };
        fm.registerBuiltinFunction("json_create", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", f); // جسن_جديد
    }

    // 2. json_destroy / جسن_تدمير
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::json_destroy_impl(id);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("json_destroy", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xaa\xd8\xaf\xd9\x85\xd9\x8a\xd8\xb1", f); // جسن_تدمير
    }

    // 3. json_write_string / جسن_اكتب_نص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string key = args.size() > 1 ? args[1]->toString() : "";
            std::string val = args.size() > 2 ? args[2]->toString() : "";
            sad::stdlib::graphics::json_write_string_impl(id, key, val);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("json_write_string", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xa7\xd9\x83\xd8\xaa\xd8\xa8_\xd9\x86\xd8\xb5", f); // جسن_اكتب_نص
    }

    // 4. json_write_number / جسن_اكتب_رقم
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string key = args.size() > 1 ? args[1]->toString() : "";
            double val = args.size() > 2 ? args[2]->toDouble() : 0.0;
            sad::stdlib::graphics::json_write_number_impl(id, key, val);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("json_write_number", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xa7\xd9\x83\xd8\xaa\xd8\xa8_\xd8\xb1\xd9\x82\xd9\x85", f); // جسن_اكتب_رقم
    }

    // 5. json_write_bool / جسن_اكتب_منطقي
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string key = args.size() > 1 ? args[1]->toString() : "";
            bool val = args.size() > 2 ? args[2]->toBool() : false;
            sad::stdlib::graphics::json_write_bool_impl(id, key, val);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("json_write_bool", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xa7\xd9\x83\xd8\xaa\xd8\xa8_\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a", f); // جسن_اكتب_منطقي
    }

    // 6. json_read_string / جسن_اقرا_نص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string key = args.size() > 1 ? args[1]->toString() : "";
            return std::make_shared<Data::Value>(sad::stdlib::graphics::json_read_string_impl(id, key));
        };
        fm.registerBuiltinFunction("json_read_string", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa7_\xd9\x86\xd8\xb5", f); // جسن_اقرا_نص
    }

    // 7. json_read_number / جسن_اقرا_رقم
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string key = args.size() > 1 ? args[1]->toString() : "";
            return std::make_shared<Data::Value>(sad::stdlib::graphics::json_read_number_impl(id, key));
        };
        fm.registerBuiltinFunction("json_read_number", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa7_\xd8\xb1\xd9\x82\xd9\x85", f); // جسن_اقرا_رقم
    }

    // 8. json_read_bool / جسن_اقرا_منطقي
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string key = args.size() > 1 ? args[1]->toString() : "";
            return std::make_shared<Data::Value>(sad::stdlib::graphics::json_read_bool_impl(id, key));
        };
        fm.registerBuiltinFunction("json_read_bool", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa7_\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a", f); // جسن_اقرا_منطقي
    }

    // 9. json_to_string / جسن_الى_نص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(sad::stdlib::graphics::json_to_string_impl(id));
        };
        fm.registerBuiltinFunction("json_to_string", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xa7\xd9\x84\xd9\x89_\xd9\x86\xd8\xb5", f); // جسن_الى_نص
    }

    // 10. json_parse / جسن_حلل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            std::string jsonStr = args.size() > 0 ? args[0]->toString() : "{}";
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::json_parse_impl(jsonStr)));
        };
        fm.registerBuiltinFunction("json_parse", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xad\xd9\x84\xd9\x84", f); // جسن_حلل
    }

    // 11. json_has_key / جسن_يحتوي
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string key = args.size() > 1 ? args[1]->toString() : "";
            return std::make_shared<Data::Value>(sad::stdlib::graphics::json_has_key_impl(id, key));
        };
        fm.registerBuiltinFunction("json_has_key", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd9\x8a\xd8\xad\xd8\xaa\xd9\x88\xd9\x8a", f); // جسن_يحتوي
    }

    // 12. json_remove / جسن_احذف
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string key = args.size() > 1 ? args[1]->toString() : "";
            sad::stdlib::graphics::json_remove_impl(id, key);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("json_remove", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xa7\xd8\xad\xd8\xb0\xd9\x81", f); // جسن_احذف
    }

    // 13. json_size / جسن_حجم
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::json_size_impl(id)));
        };
        fm.registerBuiltinFunction("json_size", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xad\xd8\xac\xd9\x85", f); // جسن_حجم
    }

    // 14. json_save_file / جسن_احفظ_ملف
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string path = args.size() > 1 ? args[1]->toString() : "";
            return std::make_shared<Data::Value>(sad::stdlib::graphics::json_save_file_impl(id, path));
        };
        fm.registerBuiltinFunction("json_save_file", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xa7\xd8\xad\xd9\x81\xd8\xb8_\xd9\x85\xd9\x84\xd9\x81", f); // جسن_احفظ_ملف
    }

    // 15. json_load_file / جسن_حمل_ملف
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            std::string path = args.size() > 0 ? args[0]->toString() : "";
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::json_load_file_impl(path)));
        };
        fm.registerBuiltinFunction("json_load_file", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xad\xd9\x85\xd9\x84_\xd9\x85\xd9\x84\xd9\x81", f); // جسن_حمل_ملف
    }

    // =================================================================
    // مدير الموارد / Resource Manager
    // =================================================================

    // 16. resource_load / مورد_تحميل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            std::string path = args.size() > 0 ? args[0]->toString() : "";
            std::string type = args.size() > 1 ? args[1]->toString() : "generic";
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::resource_load_impl(path, type)));
        };
        fm.registerBuiltinFunction("resource_load", f);
        fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd8\xb1\xd8\xaf_\xd8\xaa\xd8\xad\xd9\x85\xd9\x8a\xd9\x84", f); // مورد_تحميل
    }

    // 17. resource_unload / مورد_تفريغ
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            std::string path = args.size() > 0 ? args[0]->toString() : "";
            sad::stdlib::graphics::resource_unload_impl(path);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("resource_unload", f);
        fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd8\xb1\xd8\xaf_\xd8\xaa\xd9\x81\xd8\xb1\xd9\x8a\xd8\xba", f); // مورد_تفريغ
    }

    // 18. resource_loaded / مورد_محمل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            std::string path = args.size() > 0 ? args[0]->toString() : "";
            return std::make_shared<Data::Value>(sad::stdlib::graphics::resource_loaded_impl(path));
        };
        fm.registerBuiltinFunction("resource_loaded", f);
        fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd8\xb1\xd8\xaf_\xd9\x85\xd8\xad\xd9\x85\xd9\x84", f); // مورد_محمل
    }

    // 19. resource_count / مورد_عدد
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::resource_count_impl()));
        };
        fm.registerBuiltinFunction("resource_count", f);
        fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd8\xb1\xd8\xaf_\xd8\xb9\xd8\xaf\xd8\xaf", f); // مورد_عدد
    }

    // 20. resource_memory / مورد_ذاكرة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::resource_memory_impl()));
        };
        fm.registerBuiltinFunction("resource_memory", f);
        fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd8\xb1\xd8\xaf_\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9", f); // مورد_ذاكرة
    }

    // 21. resource_clear_all / مورد_مسح_الكل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            sad::stdlib::graphics::resource_clear_all_impl();
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("resource_clear_all", f);
        fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd8\xb1\xd8\xaf_\xd9\x85\xd8\xb3\xd8\xad_\xd8\xa7\xd9\x84\xd9\x83\xd9\x84", f); // مورد_مسح_الكل
    }

    // 22. resource_cache_ratio / مورد_نسبة_كاش
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(sad::stdlib::graphics::resource_cache_ratio_impl());
        };
        fm.registerBuiltinFunction("resource_cache_ratio", f);
        fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd8\xb1\xd8\xaf_\xd9\x86\xd8\xb3\xd8\xa8\xd8\xa9_\xd9\x83\xd8\xa7\xd8\xb4", f); // مورد_نسبة_كاش
    }

    // 23. resource_load_count / مورد_عدد_تحميلات
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::resource_load_count_impl()));
        };
        fm.registerBuiltinFunction("resource_load_count", f);
        fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd8\xb1\xd8\xaf_\xd8\xb9\xd8\xaf\xd8\xaf_\xd8\xaa\xd8\xad\xd9\x85\xd9\x8a\xd9\x84\xd8\xa7\xd8\xaa", f); // مورد_عدد_تحميلات
    }

    // 24. resource_set_limit / مورد_حد_ذاكرة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int bytes = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::resource_set_limit_impl(bytes);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("resource_set_limit", f);
        fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd8\xb1\xd8\xaf_\xd8\xad\xd8\xaf_\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9", f); // مورد_حد_ذاكرة
    }

} // registerBuiltinsPart31

} // namespace Interpreter
} // namespace Sad
