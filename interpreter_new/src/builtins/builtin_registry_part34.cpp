/**
 * @file builtin_registry_part34.cpp
 * @brief (AR) دوال الأشكال المتقدمة والمعالجة البعدية
 * @brief (EN) Advanced Shapes & Post-Processing functions
 */

#include "interpreter_core.h"
#include "value.h"
#include "graphics/sad_shapes_postfx.h"

#include <vector>
#include <memory>
#include <string>

namespace Sad {
namespace Interpreter {

void registerBuiltinsPart34(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // =================================================================
    // الأشكال المتقدمة / Advanced Shapes
    // =================================================================

    // 1. shape_bezier_quad / شكل_بيزيه_تربيعي
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int rid = args.size() > 0 ? args[0]->toInt() : 0;
            float x0 = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0;
            float y0 = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            float x1 = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 0;
            float y1 = args.size() > 4 ? static_cast<float>(args[4]->toDouble()) : 0;
            float x2 = args.size() > 5 ? static_cast<float>(args[5]->toDouble()) : 0;
            float y2 = args.size() > 6 ? static_cast<float>(args[6]->toDouble()) : 0;
            int r = args.size() > 7 ? args[7]->toInt() : 255;
            int g = args.size() > 8 ? args[8]->toInt() : 255;
            int b = args.size() > 9 ? args[9]->toInt() : 255;
            float thick = args.size() > 10 ? static_cast<float>(args[10]->toDouble()) : 1.0f;
            sad::stdlib::graphics::shape_bezier_quad_impl(rid, x0, y0, x1, y1, x2, y2, r, g, b, thick);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("shape_bezier_quad", f);
        fm.registerBuiltinFunction("\xd8\xb4\xd9\x83\xd9\x84_\xd8\xa8\xd9\x8a\xd8\xb2\xd9\x8a\xd9\x87_\xd8\xaa\xd8\xb1\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a", f); // شكل_بيزيه_تربيعي
    }

    // 2. shape_bezier_cubic / شكل_بيزيه_تكعيبي
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int rid = args.size() > 0 ? args[0]->toInt() : 0;
            float x0 = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0;
            float y0 = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            float x1 = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 0;
            float y1 = args.size() > 4 ? static_cast<float>(args[4]->toDouble()) : 0;
            float x2 = args.size() > 5 ? static_cast<float>(args[5]->toDouble()) : 0;
            float y2 = args.size() > 6 ? static_cast<float>(args[6]->toDouble()) : 0;
            float x3 = args.size() > 7 ? static_cast<float>(args[7]->toDouble()) : 0;
            float y3 = args.size() > 8 ? static_cast<float>(args[8]->toDouble()) : 0;
            int r = args.size() > 9 ? args[9]->toInt() : 255;
            int g = args.size() > 10 ? args[10]->toInt() : 255;
            int b = args.size() > 11 ? args[11]->toInt() : 255;
            float thick = args.size() > 12 ? static_cast<float>(args[12]->toDouble()) : 1.0f;
            sad::stdlib::graphics::shape_bezier_cubic_impl(rid, x0, y0, x1, y1, x2, y2, x3, y3, r, g, b, thick);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("shape_bezier_cubic", f);
        fm.registerBuiltinFunction("\xd8\xb4\xd9\x83\xd9\x84_\xd8\xa8\xd9\x8a\xd8\xb2\xd9\x8a\xd9\x87_\xd8\xaa\xd9\x83\xd8\xb9\xd9\x8a\xd8\xa8\xd9\x8a", f); // شكل_بيزيه_تكعيبي
    }

    // 3. shape_polygon / شكل_مضلع
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int rid = args.size() > 0 ? args[0]->toInt() : 0;
            float cx = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0;
            float cy = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            float radius = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 10;
            int sides = args.size() > 4 ? args[4]->toInt() : 6;
            int r = args.size() > 5 ? args[5]->toInt() : 255;
            int g = args.size() > 6 ? args[6]->toInt() : 255;
            int b = args.size() > 7 ? args[7]->toInt() : 255;
            bool filled = args.size() > 8 ? args[8]->toBool() : false;
            sad::stdlib::graphics::shape_polygon_impl(rid, cx, cy, radius, sides, r, g, b, filled);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("shape_polygon", f);
        fm.registerBuiltinFunction("\xd8\xb4\xd9\x83\xd9\x84_\xd9\x85\xd8\xb6\xd9\x84\xd8\xb9", f); // شكل_مضلع
    }

    // 4. shape_star / شكل_نجمة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int rid = args.size() > 0 ? args[0]->toInt() : 0;
            float cx = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0;
            float cy = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            float outerR = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 20;
            float innerR = args.size() > 4 ? static_cast<float>(args[4]->toDouble()) : 10;
            int points = args.size() > 5 ? args[5]->toInt() : 5;
            int r = args.size() > 6 ? args[6]->toInt() : 255;
            int g = args.size() > 7 ? args[7]->toInt() : 255;
            int b = args.size() > 8 ? args[8]->toInt() : 255;
            bool filled = args.size() > 9 ? args[9]->toBool() : false;
            sad::stdlib::graphics::shape_star_impl(rid, cx, cy, outerR, innerR, points, r, g, b, filled);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("shape_star", f);
        fm.registerBuiltinFunction("\xd8\xb4\xd9\x83\xd9\x84_\xd9\x86\xd8\xac\xd9\x85\xd8\xa9", f); // شكل_نجمة
    }

    // 5. shape_arrow / شكل_سهم
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int rid = args.size() > 0 ? args[0]->toInt() : 0;
            float x0 = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0;
            float y0 = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            float x1 = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 0;
            float y1 = args.size() > 4 ? static_cast<float>(args[4]->toDouble()) : 0;
            float headSize = args.size() > 5 ? static_cast<float>(args[5]->toDouble()) : 10;
            int r = args.size() > 6 ? args[6]->toInt() : 255;
            int g = args.size() > 7 ? args[7]->toInt() : 255;
            int b = args.size() > 8 ? args[8]->toInt() : 255;
            sad::stdlib::graphics::shape_arrow_impl(rid, x0, y0, x1, y1, headSize, r, g, b);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("shape_arrow", f);
        fm.registerBuiltinFunction("\xd8\xb4\xd9\x83\xd9\x84_\xd8\xb3\xd9\x87\xd9\x85", f); // شكل_سهم
    }

    // 6. shape_arc / شكل_قوس
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int rid = args.size() > 0 ? args[0]->toInt() : 0;
            float cx = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0;
            float cy = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            float radius = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 10;
            float startAngle = args.size() > 4 ? static_cast<float>(args[4]->toDouble()) : 0;
            float endAngle = args.size() > 5 ? static_cast<float>(args[5]->toDouble()) : 3.14159f;
            int r = args.size() > 6 ? args[6]->toInt() : 255;
            int g = args.size() > 7 ? args[7]->toInt() : 255;
            int b = args.size() > 8 ? args[8]->toInt() : 255;
            float thick = args.size() > 9 ? static_cast<float>(args[9]->toDouble()) : 1.0f;
            sad::stdlib::graphics::shape_arc_impl(rid, cx, cy, radius, startAngle, endAngle, r, g, b, thick);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("shape_arc", f);
        fm.registerBuiltinFunction("\xd8\xb4\xd9\x83\xd9\x84_\xd9\x82\xd9\x88\xd8\xb3", f); // شكل_قوس
    }

    // 7. shape_pie / شكل_فطيرة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int rid = args.size() > 0 ? args[0]->toInt() : 0;
            float cx = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0;
            float cy = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            float radius = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 10;
            float startAngle = args.size() > 4 ? static_cast<float>(args[4]->toDouble()) : 0;
            float endAngle = args.size() > 5 ? static_cast<float>(args[5]->toDouble()) : 3.14159f;
            int r = args.size() > 6 ? args[6]->toInt() : 255;
            int g = args.size() > 7 ? args[7]->toInt() : 255;
            int b = args.size() > 8 ? args[8]->toInt() : 255;
            bool filled = args.size() > 9 ? args[9]->toBool() : true;
            sad::stdlib::graphics::shape_pie_impl(rid, cx, cy, radius, startAngle, endAngle, r, g, b, filled);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("shape_pie", f);
        fm.registerBuiltinFunction("\xd8\xb4\xd9\x83\xd9\x84_\xd9\x81\xd8\xb7\xd9\x8a\xd8\xb1\xd8\xa9", f); // شكل_فطيرة
    }

    // 8. shape_ring / شكل_حلقة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int rid = args.size() > 0 ? args[0]->toInt() : 0;
            float cx = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0;
            float cy = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            float innerR = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 5;
            float outerR = args.size() > 4 ? static_cast<float>(args[4]->toDouble()) : 10;
            int r = args.size() > 5 ? args[5]->toInt() : 255;
            int g = args.size() > 6 ? args[6]->toInt() : 255;
            int b = args.size() > 7 ? args[7]->toInt() : 255;
            sad::stdlib::graphics::shape_ring_impl(rid, cx, cy, innerR, outerR, r, g, b);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("shape_ring", f);
        fm.registerBuiltinFunction("\xd8\xb4\xd9\x83\xd9\x84_\xd8\xad\xd9\x84\xd9\x82\xd8\xa9", f); // شكل_حلقة
    }

    // 9. shape_grid / شكل_شبكة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int rid = args.size() > 0 ? args[0]->toInt() : 0;
            float x = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0;
            float y = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            float w = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 100;
            float h = args.size() > 4 ? static_cast<float>(args[4]->toDouble()) : 100;
            int cols = args.size() > 5 ? args[5]->toInt() : 10;
            int rows = args.size() > 6 ? args[6]->toInt() : 10;
            int r = args.size() > 7 ? args[7]->toInt() : 200;
            int g = args.size() > 8 ? args[8]->toInt() : 200;
            int b = args.size() > 9 ? args[9]->toInt() : 200;
            sad::stdlib::graphics::shape_grid_impl(rid, x, y, w, h, cols, rows, r, g, b);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("shape_grid", f);
        fm.registerBuiltinFunction("\xd8\xb4\xd9\x83\xd9\x84_\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9", f); // شكل_شبكة
    }

    // 10. shape_heart / شكل_قلب
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int rid = args.size() > 0 ? args[0]->toInt() : 0;
            float cx = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0;
            float cy = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            float size = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 20;
            int r = args.size() > 4 ? args[4]->toInt() : 255;
            int g = args.size() > 5 ? args[5]->toInt() : 0;
            int b = args.size() > 6 ? args[6]->toInt() : 0;
            bool filled = args.size() > 7 ? args[7]->toBool() : true;
            sad::stdlib::graphics::shape_heart_impl(rid, cx, cy, size, r, g, b, filled);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("shape_heart", f);
        fm.registerBuiltinFunction("\xd8\xb4\xd9\x83\xd9\x84_\xd9\x82\xd9\x84\xd8\xa8", f); // شكل_قلب
    }

    // 11. shape_checkmark / شكل_صح
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int rid = args.size() > 0 ? args[0]->toInt() : 0;
            float x = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0;
            float y = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            float size = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 20;
            int r = args.size() > 4 ? args[4]->toInt() : 0;
            int g = args.size() > 5 ? args[5]->toInt() : 255;
            int b = args.size() > 6 ? args[6]->toInt() : 0;
            float thick = args.size() > 7 ? static_cast<float>(args[7]->toDouble()) : 2.0f;
            sad::stdlib::graphics::shape_checkmark_impl(rid, x, y, size, r, g, b, thick);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("shape_checkmark", f);
        fm.registerBuiltinFunction("\xd8\xb4\xd9\x83\xd9\x84_\xd8\xb5\xd8\xad", f); // شكل_صح
    }

    // 12. shape_x / شكل_خطا
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int rid = args.size() > 0 ? args[0]->toInt() : 0;
            float x = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0;
            float y = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
            float size = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 20;
            int r = args.size() > 4 ? args[4]->toInt() : 255;
            int g = args.size() > 5 ? args[5]->toInt() : 0;
            int b = args.size() > 6 ? args[6]->toInt() : 0;
            float thick = args.size() > 7 ? static_cast<float>(args[7]->toDouble()) : 2.0f;
            sad::stdlib::graphics::shape_x_impl(rid, x, y, size, r, g, b, thick);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("shape_x", f);
        fm.registerBuiltinFunction("\xd8\xb4\xd9\x83\xd9\x84_\xd8\xae\xd8\xb7\xd8\xa7", f); // شكل_خطا
    }

    // =================================================================
    // المعالجة البعدية / Post-Processing
    // =================================================================

    // 13. postfx_create_chain / تاثير_سلسلة_جديد
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::postfx_create_chain_impl()));
        };
        fm.registerBuiltinFunction("postfx_create_chain", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xa7\xd8\xab\xd9\x8a\xd8\xb1_\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd8\xa9_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", f); // تاثير_سلسلة_جديد
    }

    // 14. postfx_destroy_chain / تاثير_سلسلة_تدمير
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::postfx_destroy_chain_impl(id);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("postfx_destroy_chain", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xa7\xd8\xab\xd9\x8a\xd8\xb1_\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd8\xa9_\xd8\xaa\xd8\xaf\xd9\x85\xd9\x8a\xd8\xb1", f); // تاثير_سلسلة_تدمير
    }

    // 15. postfx_add_bloom / تاثير_توهج
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            float threshold = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0.8f;
            float intensity = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 1.0f;
            int passes = args.size() > 3 ? args[3]->toInt() : 3;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::postfx_add_bloom_impl(id, threshold, intensity, passes)));
        };
        fm.registerBuiltinFunction("postfx_add_bloom", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xa7\xd8\xab\xd9\x8a\xd8\xb1_\xd8\xaa\xd9\x88\xd9\x87\xd8\xac", f); // تاثير_توهج
    }

    // 16. postfx_add_blur / تاثير_ضبابية
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            float radius = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 2.0f;
            int samples = args.size() > 2 ? args[2]->toInt() : 5;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::postfx_add_blur_impl(id, radius, samples)));
        };
        fm.registerBuiltinFunction("postfx_add_blur", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xa7\xd8\xab\xd9\x8a\xd8\xb1_\xd8\xb6\xd8\xa8\xd8\xa7\xd8\xa8\xd9\x8a\xd8\xa9", f); // تاثير_ضبابية
    }

    // 17. postfx_add_vignette / تاثير_فينييت
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            float intensity = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0.5f;
            float radius = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0.8f;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::postfx_add_vignette_impl(id, intensity, radius)));
        };
        fm.registerBuiltinFunction("postfx_add_vignette", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xa7\xd8\xab\xd9\x8a\xd8\xb1_\xd9\x81\xd9\x8a\xd9\x86\xd9\x8a\xd9\x8a\xd8\xaa", f); // تاثير_فينييت
    }

    // 18. postfx_add_color_grading / تاثير_تدرج_الوان
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            float brightness = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 1.0f;
            float contrast = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 1.0f;
            float saturation = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 1.0f;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::postfx_add_color_grading_impl(id, brightness, contrast, saturation)));
        };
        fm.registerBuiltinFunction("postfx_add_color_grading", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xa7\xd8\xab\xd9\x8a\xd8\xb1_\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac_\xd8\xa7\xd9\x84\xd9\x88\xd8\xa7\xd9\x86", f); // تاثير_تدرج_الوان
    }

    // 19. postfx_add_chromatic / تاثير_انحراف_لوني
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            float intensity = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0.5f;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::postfx_add_chromatic_impl(id, intensity)));
        };
        fm.registerBuiltinFunction("postfx_add_chromatic", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xa7\xd8\xab\xd9\x8a\xd8\xb1_\xd8\xa7\xd9\x86\xd8\xad\xd8\xb1\xd8\xa7\xd9\x81_\xd9\x84\xd9\x88\xd9\x86\xd9\x8a", f); // تاثير_انحراف_لوني
    }

    // 20. postfx_add_crt / تاثير_شاشة_قديمة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            float curvature = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0.3f;
            float scanlines = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0.5f;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::postfx_add_crt_impl(id, curvature, scanlines)));
        };
        fm.registerBuiltinFunction("postfx_add_crt", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xa7\xd8\xab\xd9\x8a\xd8\xb1_\xd8\xb4\xd8\xa7\xd8\xb4\xd8\xa9_\xd9\x82\xd8\xaf\xd9\x8a\xd9\x85\xd8\xa9", f); // تاثير_شاشة_قديمة
    }

    // 21. postfx_add_grayscale / تاثير_رمادي
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::postfx_add_grayscale_impl(id)));
        };
        fm.registerBuiltinFunction("postfx_add_grayscale", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xa7\xd8\xab\xd9\x8a\xd8\xb1_\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a", f); // تاثير_رمادي
    }

    // 22. postfx_add_sepia / تاثير_سيبيا
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::postfx_add_sepia_impl(id)));
        };
        fm.registerBuiltinFunction("postfx_add_sepia", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xa7\xd8\xab\xd9\x8a\xd8\xb1_\xd8\xb3\xd9\x8a\xd8\xa8\xd9\x8a\xd8\xa7", f); // تاثير_سيبيا
    }

    // 23. postfx_add_invert / تاثير_عكس
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::postfx_add_invert_impl(id)));
        };
        fm.registerBuiltinFunction("postfx_add_invert", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xa7\xd8\xab\xd9\x8a\xd8\xb1_\xd8\xb9\xd9\x83\xd8\xb3", f); // تاثير_عكس
    }

    // 24. postfx_add_pixelate / تاثير_تقطيع
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            float pixelSize = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 4.0f;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::postfx_add_pixelate_impl(id, pixelSize)));
        };
        fm.registerBuiltinFunction("postfx_add_pixelate", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xa7\xd8\xab\xd9\x8a\xd8\xb1_\xd8\xaa\xd9\x82\xd8\xb7\xd9\x8a\xd8\xb9", f); // تاثير_تقطيع
    }

    // 25. postfx_set_enabled / تاثير_تفعيل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int chainId = args.size() > 0 ? args[0]->toInt() : 0;
            int idx = args.size() > 1 ? args[1]->toInt() : 0;
            bool enabled = args.size() > 2 ? args[2]->toBool() : true;
            sad::stdlib::graphics::postfx_set_enabled_impl(chainId, idx, enabled);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("postfx_set_enabled", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xa7\xd8\xab\xd9\x8a\xd8\xb1_\xd8\xaa\xd9\x81\xd8\xb9\xd9\x8a\xd9\x84", f); // تاثير_تفعيل
    }

    // 26. postfx_set_intensity / تاثير_شدة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int chainId = args.size() > 0 ? args[0]->toInt() : 0;
            int idx = args.size() > 1 ? args[1]->toInt() : 0;
            float intensity = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 1.0f;
            sad::stdlib::graphics::postfx_set_intensity_impl(chainId, idx, intensity);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("postfx_set_intensity", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xa7\xd8\xab\xd9\x8a\xd8\xb1_\xd8\xb4\xd8\xaf\xd8\xa9", f); // تاثير_شدة
    }

    // 27. postfx_effect_count / تاثير_عدد
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int chainId = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::postfx_effect_count_impl(chainId)));
        };
        fm.registerBuiltinFunction("postfx_effect_count", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xa7\xd8\xab\xd9\x8a\xd8\xb1_\xd8\xb9\xd8\xaf\xd8\xaf", f); // تاثير_عدد
    }

    // 28. postfx_remove / تاثير_ازالة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int chainId = args.size() > 0 ? args[0]->toInt() : 0;
            int idx = args.size() > 1 ? args[1]->toInt() : 0;
            sad::stdlib::graphics::postfx_remove_impl(chainId, idx);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("postfx_remove", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xa7\xd8\xab\xd9\x8a\xd8\xb1_\xd8\xa7\xd8\xb2\xd8\xa7\xd9\x84\xd8\xa9", f); // تاثير_ازالة
    }

    // 29. postfx_clear / تاثير_مسح
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int chainId = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::postfx_clear_impl(chainId);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("postfx_clear", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xa7\xd8\xab\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xb3\xd8\xad", f); // تاثير_مسح
    }

} // registerBuiltinsPart34

} // namespace Interpreter
} // namespace Sad
