/**
 * @file builtin_registry_part32.cpp
 * @brief (AR) دوال الإطار المخزني (Framebuffer) بدون GPU
 * @brief (EN) OS Framebuffer functions (GPU-Free)
 */

#include "interpreter_core.h"
#include "value.h"
#include "graphics/sad_framebuffer_binding.h"

#include <vector>
#include <memory>
#include <string>

namespace Sad {
namespace Interpreter {

void registerBuiltinsPart32(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // =================================================================
    // الإطار المخزني / Framebuffer
    // =================================================================

    // 1. framebuffer_create / اطار_جديد
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int w = args.size() > 0 ? args[0]->toInt() : 640;
            int h = args.size() > 1 ? args[1]->toInt() : 480;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::framebuffer_create_impl(w, h)));
        };
        fm.registerBuiltinFunction("framebuffer_create", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd8\xa7\xd8\xb1_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", f); // اطار_جديد
    }

    // 2. framebuffer_destroy / اطار_تدمير
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::framebuffer_destroy_impl(id);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("framebuffer_destroy", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd8\xa7\xd8\xb1_\xd8\xaa\xd8\xaf\xd9\x85\xd9\x8a\xd8\xb1", f); // اطار_تدمير
    }

    // 3. framebuffer_width / اطار_عرض
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::framebuffer_width_impl(id)));
        };
        fm.registerBuiltinFunction("framebuffer_width", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd8\xa7\xd8\xb1_\xd8\xb9\xd8\xb1\xd8\xb6", f); // اطار_عرض
    }

    // 4. framebuffer_height / اطار_ارتفاع
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::framebuffer_height_impl(id)));
        };
        fm.registerBuiltinFunction("framebuffer_height", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd8\xa7\xd8\xb1_\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9", f); // اطار_ارتفاع
    }

    // 5. framebuffer_clear / اطار_مسح
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            int r = args.size() > 1 ? args[1]->toInt() : 0;
            int g = args.size() > 2 ? args[2]->toInt() : 0;
            int b = args.size() > 3 ? args[3]->toInt() : 0;
            sad::stdlib::graphics::framebuffer_clear_impl(id, r, g, b);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("framebuffer_clear", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd8\xa7\xd8\xb1_\xd9\x85\xd8\xb3\xd8\xad", f); // اطار_مسح
    }

    // 6. framebuffer_set_pixel / اطار_بكسل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            int x = args.size() > 1 ? args[1]->toInt() : 0;
            int y = args.size() > 2 ? args[2]->toInt() : 0;
            int r = args.size() > 3 ? args[3]->toInt() : 255;
            int g = args.size() > 4 ? args[4]->toInt() : 255;
            int b = args.size() > 5 ? args[5]->toInt() : 255;
            sad::stdlib::graphics::framebuffer_set_pixel_impl(id, x, y, r, g, b);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("framebuffer_set_pixel", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd8\xa7\xd8\xb1_\xd8\xa8\xd9\x83\xd8\xb3\xd9\x84", f); // اطار_بكسل
    }

    // 7. framebuffer_get_pixel / اطار_لون_بكسل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            int x = args.size() > 1 ? args[1]->toInt() : 0;
            int y = args.size() > 2 ? args[2]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::framebuffer_get_pixel_impl(id, x, y)));
        };
        fm.registerBuiltinFunction("framebuffer_get_pixel", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd8\xa7\xd8\xb1_\xd9\x84\xd9\x88\xd9\x86_\xd8\xa8\xd9\x83\xd8\xb3\xd9\x84", f); // اطار_لون_بكسل
    }

    // 8. framebuffer_line / اطار_خط
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            int x0 = args.size() > 1 ? args[1]->toInt() : 0;
            int y0 = args.size() > 2 ? args[2]->toInt() : 0;
            int x1 = args.size() > 3 ? args[3]->toInt() : 0;
            int y1 = args.size() > 4 ? args[4]->toInt() : 0;
            int r = args.size() > 5 ? args[5]->toInt() : 255;
            int g = args.size() > 6 ? args[6]->toInt() : 255;
            int b = args.size() > 7 ? args[7]->toInt() : 255;
            sad::stdlib::graphics::framebuffer_line_impl(id, x0, y0, x1, y1, r, g, b);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("framebuffer_line", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd8\xa7\xd8\xb1_\xd8\xae\xd8\xb7", f); // اطار_خط
    }

    // 9. framebuffer_rect / اطار_مستطيل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            int x = args.size() > 1 ? args[1]->toInt() : 0;
            int y = args.size() > 2 ? args[2]->toInt() : 0;
            int w = args.size() > 3 ? args[3]->toInt() : 10;
            int h = args.size() > 4 ? args[4]->toInt() : 10;
            int r = args.size() > 5 ? args[5]->toInt() : 255;
            int g = args.size() > 6 ? args[6]->toInt() : 255;
            int b = args.size() > 7 ? args[7]->toInt() : 255;
            bool filled = args.size() > 8 ? args[8]->toBool() : false;
            sad::stdlib::graphics::framebuffer_rect_impl(id, x, y, w, h, r, g, b, filled);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("framebuffer_rect", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd8\xa7\xd8\xb1_\xd9\x85\xd8\xb3\xd8\xaa\xd8\xb7\xd9\x8a\xd9\x84", f); // اطار_مستطيل
    }

    // 10. framebuffer_circle / اطار_دائرة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            int cx = args.size() > 1 ? args[1]->toInt() : 0;
            int cy = args.size() > 2 ? args[2]->toInt() : 0;
            int radius = args.size() > 3 ? args[3]->toInt() : 10;
            int r = args.size() > 4 ? args[4]->toInt() : 255;
            int g = args.size() > 5 ? args[5]->toInt() : 255;
            int b = args.size() > 6 ? args[6]->toInt() : 255;
            bool filled = args.size() > 7 ? args[7]->toBool() : false;
            sad::stdlib::graphics::framebuffer_circle_impl(id, cx, cy, radius, r, g, b, filled);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("framebuffer_circle", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd8\xa7\xd8\xb1_\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd8\xa9", f); // اطار_دائرة
    }

    // 11. framebuffer_swap / اطار_تبديل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::framebuffer_swap_impl(id);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("framebuffer_swap", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd8\xa7\xd8\xb1_\xd8\xaa\xd8\xa8\xd8\xaf\xd9\x8a\xd9\x84", f); // اطار_تبديل
    }

    // 12. framebuffer_double_buffer / اطار_تخزين_مزدوج
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            bool enable = args.size() > 1 ? args[1]->toBool() : true;
            sad::stdlib::graphics::framebuffer_double_buffer_impl(id, enable);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("framebuffer_double_buffer", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd8\xa7\xd8\xb1_\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd9\x85\xd8\xb2\xd8\xaf\xd9\x88\xd8\xac", f); // اطار_تخزين_مزدوج
    }

    // 13. framebuffer_pixel_count / اطار_عدد_بكسلات
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::framebuffer_pixel_count_impl(id)));
        };
        fm.registerBuiltinFunction("framebuffer_pixel_count", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd8\xa7\xd8\xb1_\xd8\xb9\xd8\xaf\xd8\xaf_\xd8\xa8\xd9\x83\xd8\xb3\xd9\x84\xd8\xa7\xd8\xaa", f); // اطار_عدد_بكسلات
    }

    // 14. framebuffer_fill_region / اطار_ملء_منطقة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            int x = args.size() > 1 ? args[1]->toInt() : 0;
            int y = args.size() > 2 ? args[2]->toInt() : 0;
            int w = args.size() > 3 ? args[3]->toInt() : 10;
            int h = args.size() > 4 ? args[4]->toInt() : 10;
            int r = args.size() > 5 ? args[5]->toInt() : 255;
            int g = args.size() > 6 ? args[6]->toInt() : 255;
            int b = args.size() > 7 ? args[7]->toInt() : 255;
            sad::stdlib::graphics::framebuffer_fill_region_impl(id, x, y, w, h, r, g, b);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("framebuffer_fill_region", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd8\xa7\xd8\xb1_\xd9\x85\xd9\x84\xd8\xa1_\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd8\xa9", f); // اطار_ملء_منطقة
    }

    // 15. framebuffer_blit / اطار_نسخ
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int srcId = args.size() > 0 ? args[0]->toInt() : 0;
            int dstId = args.size() > 1 ? args[1]->toInt() : 0;
            int sx = args.size() > 2 ? args[2]->toInt() : 0;
            int sy = args.size() > 3 ? args[3]->toInt() : 0;
            int sw = args.size() > 4 ? args[4]->toInt() : 0;
            int sh = args.size() > 5 ? args[5]->toInt() : 0;
            int dx = args.size() > 6 ? args[6]->toInt() : 0;
            int dy = args.size() > 7 ? args[7]->toInt() : 0;
            sad::stdlib::graphics::framebuffer_blit_impl(srcId, dstId, sx, sy, sw, sh, dx, dy);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("framebuffer_blit", f);
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb7\xd8\xa7\xd8\xb1_\xd9\x86\xd8\xb3\xd8\xae", f); // اطار_نسخ
    }

} // registerBuiltinsPart32

} // namespace Interpreter
} // namespace Sad
