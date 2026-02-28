/**
 * @file builtin_registry_part33.cpp
 * @brief (AR) دوال التصيير ثلاثي الأبعاد وأطلس النسيج
 * @brief (EN) 3D Renderer & Texture Atlas functions
 */

#include "interpreter_core.h"
#include "value.h"
#include "graphics/sad_renderer3d_atlas.h"

#include <vector>
#include <memory>
#include <string>

namespace Sad {
namespace Interpreter {

void registerBuiltinsPart33(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // =================================================================
    // الكاميرا ثلاثية الأبعاد / 3D Camera
    // =================================================================

    // 1. camera3d_create / كاميرا3د_جديد
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            float fov = args.size() > 0 ? static_cast<float>(args[0]->toDouble()) : 60.0f;
            float nearP = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0.1f;
            float farP = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 1000.0f;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::camera3d_create_impl(fov, nearP, farP)));
        };
        fm.registerBuiltinFunction("camera3d_create", f);
        fm.registerBuiltinFunction("\xd9\x83\xd8\xa7\xd9\x85\xd9\x8a\xd8\xb1\xd8\xa7\x33\xd8\xaf_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", f); // كاميرا3د_جديد
    }

    // 2. camera3d_destroy / كاميرا3د_تدمير
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::camera3d_destroy_impl(id);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("camera3d_destroy", f);
        fm.registerBuiltinFunction("\xd9\x83\xd8\xa7\xd9\x85\xd9\x8a\xd8\xb1\xd8\xa7\x33\xd8\xaf_\xd8\xaa\xd8\xaf\xd9\x85\xd9\x8a\xd8\xb1", f); // كاميرا3د_تدمير
    }

    // 3. camera3d_set_position / كاميرا3د_موقع
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            float x = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0;
            float y = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            float z = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 0;
            sad::stdlib::graphics::camera3d_set_position_impl(id, x, y, z);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("camera3d_set_position", f);
        fm.registerBuiltinFunction("\xd9\x83\xd8\xa7\xd9\x85\xd9\x8a\xd8\xb1\xd8\xa7\x33\xd8\xaf_\xd9\x85\xd9\x88\xd9\x82\xd8\xb9", f); // كاميرا3د_موقع
    }

    // 4. camera3d_set_target / كاميرا3د_هدف
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            float x = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0;
            float y = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            float z = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 0;
            sad::stdlib::graphics::camera3d_set_target_impl(id, x, y, z);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("camera3d_set_target", f);
        fm.registerBuiltinFunction("\xd9\x83\xd8\xa7\xd9\x85\xd9\x8a\xd8\xb1\xd8\xa7\x33\xd8\xaf_\xd9\x87\xd8\xaf\xd9\x81", f); // كاميرا3د_هدف
    }

    // 5. camera3d_move_forward / كاميرا3د_تقدم
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            float amount = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 1.0f;
            sad::stdlib::graphics::camera3d_move_forward_impl(id, amount);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("camera3d_move_forward", f);
        fm.registerBuiltinFunction("\xd9\x83\xd8\xa7\xd9\x85\xd9\x8a\xd8\xb1\xd8\xa7\x33\xd8\xaf_\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85", f); // كاميرا3د_تقدم
    }

    // 6. camera3d_strafe / كاميرا3د_جانب
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            float amount = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 1.0f;
            sad::stdlib::graphics::camera3d_strafe_impl(id, amount);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("camera3d_strafe", f);
        fm.registerBuiltinFunction("\xd9\x83\xd8\xa7\xd9\x85\xd9\x8a\xd8\xb1\xd8\xa7\x33\xd8\xaf_\xd8\xac\xd8\xa7\xd9\x86\xd8\xa8", f); // كاميرا3د_جانب
    }

    // 7. camera3d_rotate / كاميرا3د_دوران
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            float yaw = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0;
            float pitch = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            sad::stdlib::graphics::camera3d_rotate_impl(id, yaw, pitch);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("camera3d_rotate", f);
        fm.registerBuiltinFunction("\xd9\x83\xd8\xa7\xd9\x85\xd9\x8a\xd8\xb1\xd8\xa7\x33\xd8\xaf_\xd8\xaf\xd9\x88\xd8\xb1\xd8\xa7\xd9\x86", f); // كاميرا3د_دوران
    }

    // =================================================================
    // الشبكات ثلاثية الأبعاد / 3D Meshes
    // =================================================================

    // 8. mesh_create_cube / شبكة_مكعب
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            float size = args.size() > 0 ? static_cast<float>(args[0]->toDouble()) : 1.0f;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::mesh_create_cube_impl(size)));
        };
        fm.registerBuiltinFunction("mesh_create_cube", f);
        fm.registerBuiltinFunction("\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9_\xd9\x85\xd9\x83\xd8\xb9\xd8\xa8", f); // شبكة_مكعب
    }

    // 9. mesh_create_sphere / شبكة_كرة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            float radius = args.size() > 0 ? static_cast<float>(args[0]->toDouble()) : 1.0f;
            int segments = args.size() > 1 ? args[1]->toInt() : 16;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::mesh_create_sphere_impl(radius, segments)));
        };
        fm.registerBuiltinFunction("mesh_create_sphere", f);
        fm.registerBuiltinFunction("\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9_\xd9\x83\xd8\xb1\xd8\xa9", f); // شبكة_كرة
    }

    // 10. mesh_create_cylinder / شبكة_اسطوانة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            float radius = args.size() > 0 ? static_cast<float>(args[0]->toDouble()) : 1.0f;
            float height = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 2.0f;
            int segments = args.size() > 2 ? args[2]->toInt() : 16;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::mesh_create_cylinder_impl(radius, height, segments)));
        };
        fm.registerBuiltinFunction("mesh_create_cylinder", f);
        fm.registerBuiltinFunction("\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9_\xd8\xa7\xd8\xb3\xd8\xb7\xd9\x88\xd8\xa7\xd9\x86\xd8\xa9", f); // شبكة_اسطوانة
    }

    // 11. mesh_create_cone / شبكة_مخروط
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            float radius = args.size() > 0 ? static_cast<float>(args[0]->toDouble()) : 1.0f;
            float height = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 2.0f;
            int segments = args.size() > 2 ? args[2]->toInt() : 16;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::mesh_create_cone_impl(radius, height, segments)));
        };
        fm.registerBuiltinFunction("mesh_create_cone", f);
        fm.registerBuiltinFunction("\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9_\xd9\x85\xd8\xae\xd8\xb1\xd9\x88\xd8\xb7", f); // شبكة_مخروط
    }

    // 12. mesh_create_plane / شبكة_مستوى
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            float width = args.size() > 0 ? static_cast<float>(args[0]->toDouble()) : 10.0f;
            float depth = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 10.0f;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::mesh_create_plane_impl(width, depth)));
        };
        fm.registerBuiltinFunction("mesh_create_plane", f);
        fm.registerBuiltinFunction("\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x88\xd9\x89", f); // شبكة_مستوى
    }

    // 13. mesh_destroy / شبكة_تدمير
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::mesh_destroy_impl(id);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("mesh_destroy", f);
        fm.registerBuiltinFunction("\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9_\xd8\xaa\xd8\xaf\xd9\x85\xd9\x8a\xd8\xb1", f); // شبكة_تدمير
    }

    // 14. mesh_triangle_count / شبكة_عدد_مثلثات
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::mesh_triangle_count_impl(id)));
        };
        fm.registerBuiltinFunction("mesh_triangle_count", f);
        fm.registerBuiltinFunction("\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9_\xd8\xb9\xd8\xaf\xd8\xaf_\xd9\x85\xd8\xab\xd9\x84\xd8\xab\xd8\xa7\xd8\xaa", f); // شبكة_عدد_مثلثات
    }

    // 15. mesh_vertex_count / شبكة_عدد_رؤوس
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::mesh_vertex_count_impl(id)));
        };
        fm.registerBuiltinFunction("mesh_vertex_count", f);
        fm.registerBuiltinFunction("\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9_\xd8\xb9\xd8\xaf\xd8\xaf_\xd8\xb1\xd8\xa4\xd9\x88\xd8\xb3", f); // شبكة_عدد_رؤوس
    }

    // =================================================================
    // المواد / Materials
    // =================================================================

    // 16. material_create / مادة_جديد
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            std::string type = args.size() > 0 ? args[0]->toString() : "standard";
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::material_create_impl(type)));
        };
        fm.registerBuiltinFunction("material_create", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa7\xd8\xaf\xd8\xa9_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", f); // مادة_جديد
    }

    // 17. material_set_color / مادة_لون
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            float r = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 1.0f;
            float g = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 1.0f;
            float b = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 1.0f;
            float a = args.size() > 4 ? static_cast<float>(args[4]->toDouble()) : 1.0f;
            sad::stdlib::graphics::material_set_color_impl(id, r, g, b, a);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("material_set_color", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa7\xd8\xaf\xd8\xa9_\xd9\x84\xd9\x88\xd9\x86", f); // مادة_لون
    }

    // 18. material_set_roughness / مادة_خشونة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            float rough = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0.5f;
            sad::stdlib::graphics::material_set_roughness_impl(id, rough);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("material_set_roughness", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa7\xd8\xaf\xd8\xa9_\xd8\xae\xd8\xb4\xd9\x88\xd9\x86\xd8\xa9", f); // مادة_خشونة
    }

    // 19. material_set_metallic / مادة_انعكاسية
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            float metallic = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0.0f;
            sad::stdlib::graphics::material_set_metallic_impl(id, metallic);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("material_set_metallic", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa7\xd8\xaf\xd8\xa9_\xd8\xa7\xd9\x86\xd8\xb9\xd9\x83\xd8\xa7\xd8\xb3\xd9\x8a\xd8\xa9", f); // مادة_انعكاسية
    }

    // 20. material_destroy / مادة_تدمير
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::material_destroy_impl(id);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("material_destroy", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa7\xd8\xaf\xd8\xa9_\xd8\xaa\xd8\xaf\xd9\x85\xd9\x8a\xd8\xb1", f); // مادة_تدمير
    }

    // =================================================================
    // الإضاءة / Lighting
    // =================================================================

    // 21. light_directional_create / ضوء_اتجاهي
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            float dx = args.size() > 0 ? static_cast<float>(args[0]->toDouble()) : 0;
            float dy = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : -1.0f;
            float dz = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            float r = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 1.0f;
            float g = args.size() > 4 ? static_cast<float>(args[4]->toDouble()) : 1.0f;
            float b = args.size() > 5 ? static_cast<float>(args[5]->toDouble()) : 1.0f;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::light_directional_create_impl(dx, dy, dz, r, g, b)));
        };
        fm.registerBuiltinFunction("light_directional_create", f);
        fm.registerBuiltinFunction("\xd8\xb6\xd9\x88\xd8\xa1_\xd8\xa7\xd8\xaa\xd8\xac\xd8\xa7\xd9\x87\xd9\x8a", f); // ضوء_اتجاهي
    }

    // 22. light_point_create / ضوء_نقطي
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            float x = args.size() > 0 ? static_cast<float>(args[0]->toDouble()) : 0;
            float y = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 5.0f;
            float z = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            float r = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 1.0f;
            float g = args.size() > 4 ? static_cast<float>(args[4]->toDouble()) : 1.0f;
            float b = args.size() > 5 ? static_cast<float>(args[5]->toDouble()) : 1.0f;
            float range = args.size() > 6 ? static_cast<float>(args[6]->toDouble()) : 50.0f;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::light_point_create_impl(x, y, z, r, g, b, range)));
        };
        fm.registerBuiltinFunction("light_point_create", f);
        fm.registerBuiltinFunction("\xd8\xb6\xd9\x88\xd8\xa1_\xd9\x86\xd9\x82\xd8\xb7\xd9\x8a", f); // ضوء_نقطي
    }

    // 23. light_set_ambient / ضوء_محيط
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            float r = args.size() > 0 ? static_cast<float>(args[0]->toDouble()) : 0.2f;
            float g = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0.2f;
            float b = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0.2f;
            sad::stdlib::graphics::light_set_ambient_impl(r, g, b);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("light_set_ambient", f);
        fm.registerBuiltinFunction("\xd8\xb6\xd9\x88\xd8\xa1_\xd9\x85\xd8\xad\xd9\x8a\xd8\xb7", f); // ضوء_محيط
    }

    // 24. light_destroy / ضوء_تدمير
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::light_destroy_impl(id);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("light_destroy", f);
        fm.registerBuiltinFunction("\xd8\xb6\xd9\x88\xd8\xa1_\xd8\xaa\xd8\xaf\xd9\x85\xd9\x8a\xd8\xb1", f); // ضوء_تدمير
    }

    // =================================================================
    // أطلس النسيج / Texture Atlas
    // =================================================================

    // 25. atlas_create / اطلس_جديد
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            std::string path = args.size() > 0 ? args[0]->toString() : "";
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::atlas_create_impl(path)));
        };
        fm.registerBuiltinFunction("atlas_create", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd9\x84\xd8\xb3_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", f); // اطلس_جديد
    }

    // 26. atlas_destroy / اطلس_تدمير
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::atlas_destroy_impl(id);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("atlas_destroy", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd9\x84\xd8\xb3_\xd8\xaa\xd8\xaf\xd9\x85\xd9\x8a\xd8\xb1", f); // اطلس_تدمير
    }

    // 27. atlas_add_region / اطلس_اضف_منطقة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string name = args.size() > 1 ? args[1]->toString() : "";
            float x = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            float y = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 0;
            float w = args.size() > 4 ? static_cast<float>(args[4]->toDouble()) : 0;
            float h = args.size() > 5 ? static_cast<float>(args[5]->toDouble()) : 0;
            sad::stdlib::graphics::atlas_add_region_impl(id, name, x, y, w, h);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("atlas_add_region", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd9\x84\xd8\xb3_\xd8\xa7\xd8\xb6\xd9\x81_\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd8\xa9", f); // اطلس_اضف_منطقة
    }

    // 28. atlas_slice_grid / اطلس_تقسيم
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string prefix = args.size() > 1 ? args[1]->toString() : "tile";
            int cols = args.size() > 2 ? args[2]->toInt() : 4;
            int rows = args.size() > 3 ? args[3]->toInt() : 4;
            sad::stdlib::graphics::atlas_slice_grid_impl(id, prefix, cols, rows);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("atlas_slice_grid", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd9\x84\xd8\xb3_\xd8\xaa\xd9\x82\xd8\xb3\xd9\x8a\xd9\x85", f); // اطلس_تقسيم
    }

    // 29. atlas_has_region / اطلس_يحتوي
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string name = args.size() > 1 ? args[1]->toString() : "";
            return std::make_shared<Data::Value>(sad::stdlib::graphics::atlas_has_region_impl(id, name));
        };
        fm.registerBuiltinFunction("atlas_has_region", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd9\x84\xd8\xb3_\xd9\x8a\xd8\xad\xd8\xaa\xd9\x88\xd9\x8a", f); // اطلس_يحتوي
    }

    // 30. atlas_region_count / اطلس_عدد_مناطق
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::atlas_region_count_impl(id)));
        };
        fm.registerBuiltinFunction("atlas_region_count", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd9\x84\xd8\xb3_\xd8\xb9\xd8\xaf\xd8\xaf_\xd9\x85\xd9\x86\xd8\xa7\xd8\xb7\xd9\x82", f); // اطلس_عدد_مناطق
    }

    // 31. atlas_define_anim / اطلس_رسم_متحرك
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string animName = args.size() > 1 ? args[1]->toString() : "";
            std::string prefix = args.size() > 2 ? args[2]->toString() : "";
            int frameCount = args.size() > 3 ? args[3]->toInt() : 1;
            float frameDur = args.size() > 4 ? static_cast<float>(args[4]->toDouble()) : 0.1f;
            sad::stdlib::graphics::atlas_define_anim_impl(id, animName, prefix, frameCount, frameDur);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("atlas_define_anim", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd9\x84\xd8\xb3_\xd8\xb1\xd8\xb3\xd9\x85_\xd9\x85\xd8\xaa\xd8\xad\xd8\xb1\xd9\x83", f); // اطلس_رسم_متحرك
    }

    // 32. atlas_anim_count / اطلس_عدد_رسوم
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::atlas_anim_count_impl(id)));
        };
        fm.registerBuiltinFunction("atlas_anim_count", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd9\x84\xd8\xb3_\xd8\xb9\xd8\xaf\xd8\xaf_\xd8\xb1\xd8\xb3\xd9\x88\xd9\x85", f); // اطلس_عدد_رسوم
    }

} // registerBuiltinsPart33

} // namespace Interpreter
} // namespace Sad
