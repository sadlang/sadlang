/**
 * @file builtin_registry_part29.cpp
 * @brief (AR) دوال نظام الكيانات والمكونات (ECS) وناقل الأحداث
 * @brief (EN) ECS & Event Bus functions
 */

#include "interpreter_core.h"
#include "value.h"
#include "graphics/sad_ecs_binding.h"

#include <vector>
#include <memory>
#include <string>

namespace Sad {
namespace Interpreter {

void registerBuiltinsPart29(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // =================================================================
    // ECS — نظام الكيانات والمكونات
    // =================================================================

    // 1. ecs_world_create / كيانات_عالم_جديد
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::ecs_world_create_impl()));
        };
        fm.registerBuiltinFunction("ecs_world_create", f);
        fm.registerBuiltinFunction("\xd9\x83\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa_\xd8\xb9\xd8\xa7\xd9\x84\xd9\x85_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", f); // كيانات_عالم_جديد
    }

    // 2. ecs_world_destroy / كيانات_عالم_تدمير
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int worldId = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::ecs_world_destroy_impl(worldId);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("ecs_world_destroy", f);
        fm.registerBuiltinFunction("\xd9\x83\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa_\xd8\xb9\xd8\xa7\xd9\x84\xd9\x85_\xd8\xaa\xd8\xaf\xd9\x85\xd9\x8a\xd8\xb1", f); // كيانات_عالم_تدمير
    }

    // 3. ecs_entity_create / كيانات_كيان_جديد
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int worldId = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::ecs_entity_create_impl(worldId)));
        };
        fm.registerBuiltinFunction("ecs_entity_create", f);
        fm.registerBuiltinFunction("\xd9\x83\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa_\xd9\x83\xd9\x8a\xd8\xa7\xd9\x86_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", f); // كيانات_كيان_جديد
    }

    // 4. ecs_entity_destroy / كيانات_كيان_تدمير
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int worldId = args.size() > 0 ? args[0]->toInt() : 0;
            int entityId = args.size() > 1 ? args[1]->toInt() : 0;
            sad::stdlib::graphics::ecs_entity_destroy_impl(worldId, entityId);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("ecs_entity_destroy", f);
        fm.registerBuiltinFunction("\xd9\x83\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa_\xd9\x83\xd9\x8a\xd8\xa7\xd9\x86_\xd8\xaa\xd8\xaf\xd9\x85\xd9\x8a\xd8\xb1", f); // كيانات_كيان_تدمير
    }

    // 5. ecs_add_position / كيانات_اضف_موقع
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int w = args.size() > 0 ? args[0]->toInt() : 0;
            int e = args.size() > 1 ? args[1]->toInt() : 0;
            float x = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            float y = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 0;
            sad::stdlib::graphics::ecs_add_position_impl(w, e, x, y);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("ecs_add_position", f);
        fm.registerBuiltinFunction("\xd9\x83\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa_\xd8\xa7\xd8\xb6\xd9\x81_\xd9\x85\xd9\x88\xd9\x82\xd8\xb9", f); // كيانات_اضف_موقع
    }

    // 6. ecs_add_velocity / كيانات_اضف_سرعة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int w = args.size() > 0 ? args[0]->toInt() : 0;
            int e = args.size() > 1 ? args[1]->toInt() : 0;
            float vx = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            float vy = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 0;
            sad::stdlib::graphics::ecs_add_velocity_impl(w, e, vx, vy);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("ecs_add_velocity", f);
        fm.registerBuiltinFunction("\xd9\x83\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa_\xd8\xa7\xd8\xb6\xd9\x81_\xd8\xb3\xd8\xb1\xd8\xb9\xd8\xa9", f); // كيانات_اضف_سرعة
    }

    // 7. ecs_add_health / كيانات_اضف_صحة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int w = args.size() > 0 ? args[0]->toInt() : 0;
            int e = args.size() > 1 ? args[1]->toInt() : 0;
            float hp = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 100;
            float maxHp = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 100;
            sad::stdlib::graphics::ecs_add_health_impl(w, e, hp, maxHp);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("ecs_add_health", f);
        fm.registerBuiltinFunction("\xd9\x83\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa_\xd8\xa7\xd8\xb6\xd9\x81_\xd8\xb5\xd8\xad\xd8\xa9", f); // كيانات_اضف_صحة
    }

    // 8. ecs_add_tag / كيانات_اضف_وسم
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int w = args.size() > 0 ? args[0]->toInt() : 0;
            int e = args.size() > 1 ? args[1]->toInt() : 0;
            std::string tag = args.size() > 2 ? args[2]->toString() : "";
            sad::stdlib::graphics::ecs_add_tag_impl(w, e, tag);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("ecs_add_tag", f);
        fm.registerBuiltinFunction("\xd9\x83\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa_\xd8\xa7\xd8\xb6\xd9\x81_\xd9\x88\xd8\xb3\xd9\x85", f); // كيانات_اضف_وسم
    }

    // 9. ecs_get_x / كيانات_موقع_س
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int w = args.size() > 0 ? args[0]->toInt() : 0;
            int e = args.size() > 1 ? args[1]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<double>(sad::stdlib::graphics::ecs_get_x_impl(w, e)));
        };
        fm.registerBuiltinFunction("ecs_get_x", f);
        fm.registerBuiltinFunction("\xd9\x83\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa_\xd9\x85\xd9\x88\xd9\x82\xd8\xb9_\xd8\xb3", f); // كيانات_موقع_س
    }

    // 10. ecs_get_y / كيانات_موقع_ص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int w = args.size() > 0 ? args[0]->toInt() : 0;
            int e = args.size() > 1 ? args[1]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<double>(sad::stdlib::graphics::ecs_get_y_impl(w, e)));
        };
        fm.registerBuiltinFunction("ecs_get_y", f);
        fm.registerBuiltinFunction("\xd9\x83\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa_\xd9\x85\xd9\x88\xd9\x82\xd8\xb9_\xd8\xb5", f); // كيانات_موقع_ص
    }

    // 11. ecs_set_position / كيانات_عين_موقع
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int w = args.size() > 0 ? args[0]->toInt() : 0;
            int e = args.size() > 1 ? args[1]->toInt() : 0;
            float x = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            float y = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 0;
            sad::stdlib::graphics::ecs_set_position_impl(w, e, x, y);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("ecs_set_position", f);
        fm.registerBuiltinFunction("\xd9\x83\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa_\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x85\xd9\x88\xd9\x82\xd8\xb9", f); // كيانات_عين_موقع
    }

    // 12. ecs_world_update / كيانات_عالم_تحديث
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int w = args.size() > 0 ? args[0]->toInt() : 0;
            float dt = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0.016f;
            sad::stdlib::graphics::ecs_world_update_impl(w, dt);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("ecs_world_update", f);
        fm.registerBuiltinFunction("\xd9\x83\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa_\xd8\xb9\xd8\xa7\xd9\x84\xd9\x85_\xd8\xaa\xd8\xad\xd8\xaf\xd9\x8a\xd8\xab", f); // كيانات_عالم_تحديث
    }

    // 13. ecs_entity_count / كيانات_عدد
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int w = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::ecs_entity_count_impl(w)));
        };
        fm.registerBuiltinFunction("ecs_entity_count", f);
        fm.registerBuiltinFunction("\xd9\x83\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa_\xd8\xb9\xd8\xaf\xd8\xaf", f); // كيانات_عدد
    }

    // 14. ecs_entity_alive / كيانات_كيان_حي
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int w = args.size() > 0 ? args[0]->toInt() : 0;
            int e = args.size() > 1 ? args[1]->toInt() : 0;
            return std::make_shared<Data::Value>(sad::stdlib::graphics::ecs_entity_alive_impl(w, e));
        };
        fm.registerBuiltinFunction("ecs_entity_alive", f);
        fm.registerBuiltinFunction("\xd9\x83\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa_\xd9\x83\xd9\x8a\xd8\xa7\xd9\x86_\xd8\xad\xd9\x8a", f); // كيانات_كيان_حي
    }

    // =================================================================
    // Event Bus — ناقل الأحداث
    // =================================================================

    // 15. eventbus_create / احداث_جديد
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::eventbus_create_impl()));
        };
        fm.registerBuiltinFunction("eventbus_create", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xad\xd8\xaf\xd8\xa7\xd8\xab_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", f); // احداث_جديد
    }

    // 16. eventbus_destroy / احداث_تدمير
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::eventbus_destroy_impl(id);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("eventbus_destroy", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xad\xd8\xaf\xd8\xa7\xd8\xab_\xd8\xaa\xd8\xaf\xd9\x85\xd9\x8a\xd8\xb1", f); // احداث_تدمير
    }

    // 17. eventbus_publish / احداث_نشر
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string name = args.size() > 1 ? args[1]->toString() : "";
            sad::stdlib::graphics::eventbus_publish_impl(id, name);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("eventbus_publish", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xad\xd8\xaf\xd8\xa7\xd8\xab_\xd9\x86\xd8\xb4\xd8\xb1", f); // احداث_نشر
    }

    // 18. eventbus_clear / احداث_مسح
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::eventbus_clear_impl(id);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("eventbus_clear", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xad\xd8\xaf\xd8\xa7\xd8\xab_\xd9\x85\xd8\xb3\xd8\xad", f); // احداث_مسح
    }

    // 19. eventbus_pending_count / احداث_معلقة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::eventbus_pending_count_impl(id)));
        };
        fm.registerBuiltinFunction("eventbus_pending_count", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xad\xd8\xaf\xd8\xa7\xd8\xab_\xd9\x85\xd8\xb9\xd9\x84\xd9\x82\xd8\xa9", f); // احداث_معلقة
    }

    // 20. eventbus_process / احداث_معالجة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::eventbus_process_impl(id);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("eventbus_process", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xad\xd8\xaf\xd8\xa7\xd8\xab_\xd9\x85\xd8\xb9\xd8\xa7\xd9\x84\xd8\xac\xd8\xa9", f); // احداث_معالجة
    }

} // registerBuiltinsPart29

} // namespace Interpreter
} // namespace Sad
