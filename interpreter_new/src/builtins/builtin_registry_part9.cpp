/**
 * @file builtin_registry_part9.cpp
 * @brief (AR) تسجيل دوال الرسومات المضمنة - نوافذ، رسام، أشكال، عناصر واجهة، إدخال، صوت
 * @brief (EN) Graphics builtin function registrations - windows, renderer, shapes, widgets, input, audio
 */

#include "builtins.h"
#include "interpreter_core.h"
#include "graphics/graphics_module.h"
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

namespace Sad {
namespace Interpreter {

void registerBuiltinsPart9(Interpreter& interpreter) {
#ifdef HAS_GRAPHICS
    // ═══════════════════════════════════════════════════════════════════
    // (AR) إدارة النوافذ / (EN) Window Management
    // ═══════════════════════════════════════════════════════════════════

    // --- نافذة_جديد / window_new ---
    auto window_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) {
            throw std::runtime_error("(AR) \xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf: 3 \xd9\x85\xd8\xb9\xd8\xa7\xd9\x85\xd9\x84\xd8\xa7\xd8\xaa \xd9\x85\xd8\xb7\xd9\x84\xd9\x88\xd8\xa8\xd8\xa9 / (EN) window_new: 3 arguments required (title, width, height)");
        }
        std::string title = args[0]->toString();
        int width = args[1]->toInt();
        int height = args[2]->toInt();
        int windowId = sad::stdlib::graphics::window_create_impl(title, width, height);
        return std::make_shared<Data::Value>(windowId);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", window_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf\xd8\xa9", window_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_new", window_create_func);

    // --- نافذة_عرض / window_show ---
    auto window_show_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("window_show: Window argument required");
        sad::stdlib::graphics::window_show_impl(args[0]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9_\xd8\xb9\xd8\xb1\xd8\xb6", window_show_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_show", window_show_func);

    // --- نافذة_إخفاء / window_hide ---
    auto window_hide_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("window_hide: Window argument required");
        sad::stdlib::graphics::window_hide_impl(args[0]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9_\xd8\xa5\xd8\xae\xd9\x81\xd8\xa7\xd8\xa1", window_hide_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_hide", window_hide_func);

    // --- نافذة_إغلاق / window_close ---
    auto window_close_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("window_close: Window argument required");
        sad::stdlib::graphics::window_close_impl(args[0]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9_\xd8\xa5\xd8\xba\xd9\x84\xd8\xa7\xd9\x82", window_close_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_close", window_close_func);

    // --- نافذة_يجب_الإغلاق / window_should_close ---
    auto window_should_close_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("window_should_close: Window argument required");
        bool shouldClose = sad::stdlib::graphics::window_should_close_impl(args[0]->toInt());
        return std::make_shared<Data::Value>(shouldClose);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9_\xd9\x8a\xd8\xac\xd8\xa8_\xd8\xa7\xd9\x84\xd8\xa5\xd8\xba\xd9\x84\xd8\xa7\xd9\x82", window_should_close_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_should_close", window_should_close_func);

    // --- نافذة_استقبال_أحداث / window_poll_events ---
    auto window_poll_events_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("window_poll_events: Window argument required");
        sad::stdlib::graphics::window_poll_events_impl(args[0]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9_\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd8\xa7\xd9\x84_\xd8\xa3\xd8\xad\xd8\xaf\xd8\xa7\xd8\xab", window_poll_events_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_poll_events", window_poll_events_func);

    // --- نافذة_تبديل / window_swap_buffers ---
    auto window_swap_buffers_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("window_swap_buffers: Window argument required");
        sad::stdlib::graphics::window_swap_buffers_impl(args[0]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9_\xd8\xaa\xd8\xa8\xd8\xaf\xd9\x8a\xd9\x84", window_swap_buffers_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_swap_buffers", window_swap_buffers_func);

    // --- نافذة_عرض_الحجم / window_get_width ---
    auto window_get_width_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("window_get_width: Window argument required");
        int w = sad::stdlib::graphics::window_get_width_impl(args[0]->toInt());
        return std::make_shared<Data::Value>(w);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9_\xd8\xa7\xd9\x84\xd8\xb9\xd8\xb1\xd8\xb6", window_get_width_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_get_width", window_get_width_func);

    // --- نافذة_ارتفاع_الحجم / window_get_height ---
    auto window_get_height_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("window_get_height: Window argument required");
        int h = sad::stdlib::graphics::window_get_height_impl(args[0]->toInt());
        return std::make_shared<Data::Value>(h);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9_\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9", window_get_height_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_get_height", window_get_height_func);

    // --- نافذة_تعيين_عنوان / window_set_title ---
    auto window_set_title_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("window_set_title: 2 arguments required (windowId, title)");
        sad::stdlib::graphics::window_set_title_impl(args[0]->toInt(), args[1]->toString());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9_\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86", window_set_title_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_set_title", window_set_title_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) إدارة الرسام / (EN) Renderer Management
    // ═══════════════════════════════════════════════════════════════════

    // --- رسام_ثنائي_جديد / renderer_new ---
    auto renderer_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("renderer_new: Window argument required");
        int rendererId = sad::stdlib::graphics::renderer_create_impl(args[0]->toInt());
        return std::make_shared<Data::Value>(rendererId);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd8\xab\xd9\x86\xd8\xa7\xd8\xa6\xd9\x8a_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", renderer_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_new", renderer_create_func);

    // --- رسام_بدء_إطار / renderer_begin_frame ---
    auto renderer_begin_frame_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("renderer_begin_frame: Renderer argument required");
        sad::stdlib::graphics::renderer_begin_frame_impl(args[0]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd8\xa8\xd8\xaf\xd8\xa1_\xd8\xa5\xd8\xb7\xd8\xa7\xd8\xb1", renderer_begin_frame_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_begin_frame", renderer_begin_frame_func);

    // --- رسام_إنهاء_إطار / renderer_end_frame ---
    auto renderer_end_frame_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("renderer_end_frame: Renderer argument required");
        sad::stdlib::graphics::renderer_end_frame_impl(args[0]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd8\xa5\xd9\x86\xd9\x87\xd8\xa7\xd8\xa1_\xd8\xa5\xd8\xb7\xd8\xa7\xd8\xb1", renderer_end_frame_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_end_frame", renderer_end_frame_func);

    // --- رسام_مسح / renderer_clear ---
    auto renderer_clear_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 5) throw std::runtime_error("renderer_clear: 5 arguments required (renderer, r, g, b, a)");
        sad::stdlib::graphics::renderer_clear_impl(args[0]->toInt(), args[1]->toInt(), args[2]->toInt(), args[3]->toInt(), args[4]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd9\x85\xd8\xb3\xd8\xad", renderer_clear_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_clear", renderer_clear_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) رسم الأشكال / (EN) Drawing Shapes
    // ═══════════════════════════════════════════════════════════════════

    // --- رسام_خط / renderer_draw_line ---
    auto renderer_draw_line_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 9) throw std::runtime_error("renderer_draw_line: 9 arguments required (renderer, x1, y1, x2, y2, r, g, b, a)");
        sad::stdlib::graphics::renderer_draw_line_impl(
            args[0]->toInt(),
            static_cast<float>(args[1]->toDouble()), static_cast<float>(args[2]->toDouble()),
            static_cast<float>(args[3]->toDouble()), static_cast<float>(args[4]->toDouble()),
            args[5]->toInt(), args[6]->toInt(), args[7]->toInt(), args[8]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd8\xae\xd8\xb7", renderer_draw_line_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_draw_line", renderer_draw_line_func);

    // --- رسام_مستطيل / renderer_draw_rect ---
    auto renderer_draw_rect_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 10) throw std::runtime_error("renderer_draw_rect: 10 arguments required (renderer, x, y, w, h, r, g, b, a, filled)");
        sad::stdlib::graphics::renderer_draw_rect_impl(
            args[0]->toInt(),
            static_cast<float>(args[1]->toDouble()), static_cast<float>(args[2]->toDouble()),
            static_cast<float>(args[3]->toDouble()), static_cast<float>(args[4]->toDouble()),
            args[5]->toInt(), args[6]->toInt(), args[7]->toInt(), args[8]->toInt(),
            args[9]->toBool());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd9\x85\xd8\xb3\xd8\xaa\xd8\xb7\xd9\x8a\xd9\x84", renderer_draw_rect_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_draw_rect", renderer_draw_rect_func);

    // --- رسام_دائرة / renderer_draw_circle ---
    auto renderer_draw_circle_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 9) throw std::runtime_error("renderer_draw_circle: 9 arguments required (renderer, x, y, radius, r, g, b, a, filled)");
        sad::stdlib::graphics::renderer_draw_circle_impl(
            args[0]->toInt(),
            static_cast<float>(args[1]->toDouble()), static_cast<float>(args[2]->toDouble()),
            static_cast<float>(args[3]->toDouble()),
            args[4]->toInt(), args[5]->toInt(), args[6]->toInt(), args[7]->toInt(),
            args[8]->toBool());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd8\xa9", renderer_draw_circle_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_draw_circle", renderer_draw_circle_func);

    // --- رسام_نص / renderer_draw_text ---
    auto renderer_draw_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 9) throw std::runtime_error("renderer_draw_text: 9 arguments required (renderer, text, x, y, size, r, g, b, a)");
        sad::stdlib::graphics::renderer_draw_text_impl(
            args[0]->toInt(), args[1]->toString(),
            static_cast<float>(args[2]->toDouble()), static_cast<float>(args[3]->toDouble()),
            static_cast<float>(args[4]->toDouble()),
            args[5]->toInt(), args[6]->toInt(), args[7]->toInt(), args[8]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd9\x86\xd8\xb5", renderer_draw_text_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_draw_text", renderer_draw_text_func);

    // --- رسام_مثلث / renderer_draw_triangle ---
    auto renderer_draw_triangle_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 12) throw std::runtime_error("renderer_draw_triangle: 12 arguments required");
        sad::stdlib::graphics::renderer_draw_triangle_impl(
            args[0]->toInt(),
            static_cast<float>(args[1]->toDouble()), static_cast<float>(args[2]->toDouble()),
            static_cast<float>(args[3]->toDouble()), static_cast<float>(args[4]->toDouble()),
            static_cast<float>(args[5]->toDouble()), static_cast<float>(args[6]->toDouble()),
            args[7]->toInt(), args[8]->toInt(), args[9]->toInt(), args[10]->toInt(),
            args[11]->toBool());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd9\x85\xd8\xab\xd9\x84\xd8\xab", renderer_draw_triangle_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_draw_triangle", renderer_draw_triangle_func);

    // --- رسام_مستطيل_دائري / renderer_draw_rounded_rect ---
    auto renderer_draw_rounded_rect_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 11) throw std::runtime_error("renderer_draw_rounded_rect: 11 arguments required");
        sad::stdlib::graphics::renderer_draw_rounded_rect_impl(
            args[0]->toInt(),
            static_cast<float>(args[1]->toDouble()), static_cast<float>(args[2]->toDouble()),
            static_cast<float>(args[3]->toDouble()), static_cast<float>(args[4]->toDouble()),
            static_cast<float>(args[5]->toDouble()),
            args[6]->toInt(), args[7]->toInt(), args[8]->toInt(), args[9]->toInt(),
            args[10]->toBool());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd9\x85\xd8\xb3\xd8\xaa\xd8\xb7\xd9\x8a\xd9\x84_\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd9\x8a", renderer_draw_rounded_rect_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_draw_rounded_rect", renderer_draw_rounded_rect_func);

    // --- رسام_قطع_ناقص / renderer_draw_ellipse ---
    auto renderer_draw_ellipse_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 10) throw std::runtime_error("renderer_draw_ellipse: 10 arguments required");
        sad::stdlib::graphics::renderer_draw_ellipse_impl(
            args[0]->toInt(),
            static_cast<float>(args[1]->toDouble()), static_cast<float>(args[2]->toDouble()),
            static_cast<float>(args[3]->toDouble()), static_cast<float>(args[4]->toDouble()),
            args[5]->toInt(), args[6]->toInt(), args[7]->toInt(), args[8]->toInt(),
            args[9]->toBool());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd9\x82\xd8\xb7\xd8\xb9_\xd9\x86\xd8\xa7\xd9\x82\xd8\xb5", renderer_draw_ellipse_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_draw_ellipse", renderer_draw_ellipse_func);

    // --- رسام_قوس / renderer_draw_arc ---
    auto renderer_draw_arc_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 9) throw std::runtime_error("renderer_draw_arc: 9 arguments required");
        sad::stdlib::graphics::renderer_draw_arc_impl(
            args[0]->toInt(),
            static_cast<float>(args[1]->toDouble()), static_cast<float>(args[2]->toDouble()),
            static_cast<float>(args[3]->toDouble()),
            static_cast<float>(args[4]->toDouble()), static_cast<float>(args[5]->toDouble()),
            args[6]->toInt(), args[7]->toInt(), args[8]->toInt(), args[9]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd9\x82\xd9\x88\xd8\xb3", renderer_draw_arc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_draw_arc", renderer_draw_arc_func);

    // --- رسام_مستطيل_محدد / renderer_draw_rect_outline ---
    auto renderer_draw_rect_outline_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 14) throw std::runtime_error("renderer_draw_rect_outline: 14 arguments required");
        sad::stdlib::graphics::renderer_draw_rect_outline_impl(
            args[0]->toInt(),
            static_cast<float>(args[1]->toDouble()), static_cast<float>(args[2]->toDouble()),
            static_cast<float>(args[3]->toDouble()), static_cast<float>(args[4]->toDouble()),
            args[5]->toInt(), args[6]->toInt(), args[7]->toInt(), args[8]->toInt(),
            args[9]->toInt(), args[10]->toInt(), args[11]->toInt(), args[12]->toInt(),
            static_cast<float>(args[13]->toDouble()));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd9\x85\xd8\xb3\xd8\xaa\xd8\xb7\xd9\x8a\xd9\x84_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf", renderer_draw_rect_outline_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_draw_rect_outline", renderer_draw_rect_outline_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) عناصر الواجهة / (EN) UI Widgets
    // ═══════════════════════════════════════════════════════════════════

    // --- تسمية_جديدة / label_new ---
    auto label_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 5) throw std::runtime_error("label_new: 5 arguments required (text, x, y, width, height)");
        int labelId = sad::stdlib::graphics::label_create_impl(
            args[0]->toString(),
            static_cast<float>(args[1]->toDouble()), static_cast<float>(args[2]->toDouble()),
            static_cast<float>(args[3]->toDouble()), static_cast<float>(args[4]->toDouble()));
        return std::make_shared<Data::Value>(labelId);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x85\xd9\x8a\xd8\xa9_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf\xd8\xa9", label_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("label_new", label_create_func);

    // --- تسمية_تعيين_نص / label_set_text ---
    auto label_set_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("label_set_text: 2 arguments required");
        sad::stdlib::graphics::label_set_text_impl(args[0]->toInt(), args[1]->toString());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x85\xd9\x8a\xd8\xa9_\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd9\x86\xd8\xb5", label_set_text_func);
    interpreter.getFunctionManager().registerBuiltinFunction("label_set_text", label_set_text_func);

    // --- تسمية_الحصول_على_نص / label_get_text ---
    auto label_get_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("label_get_text: Label argument required");
        std::string text = sad::stdlib::graphics::label_get_text_impl(args[0]->toInt());
        return std::make_shared<Data::Value>(text);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x85\xd9\x8a\xd8\xa9_\xd8\xa7\xd9\x84\xd8\xad\xd8\xb5\xd9\x88\xd9\x84_\xd8\xb9\xd9\x84\xd9\x89_\xd9\x86\xd8\xb5", label_get_text_func);
    interpreter.getFunctionManager().registerBuiltinFunction("label_get_text", label_get_text_func);

    // --- زر_جديد / button_new ---
    auto button_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 5) throw std::runtime_error("button_new: 5 arguments required (text, x, y, width, height)");
        int buttonId = sad::stdlib::graphics::button_create_impl(
            args[0]->toString(),
            static_cast<float>(args[1]->toDouble()), static_cast<float>(args[2]->toDouble()),
            static_cast<float>(args[3]->toDouble()), static_cast<float>(args[4]->toDouble()));
        return std::make_shared<Data::Value>(buttonId);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb2\xd8\xb1_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", button_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("button_new", button_create_func);

    // --- زر_تم_النقر / button_is_clicked ---
    auto button_is_clicked_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("button_is_clicked: Button argument required");
        bool clicked = sad::stdlib::graphics::button_is_clicked_impl(args[0]->toInt());
        return std::make_shared<Data::Value>(clicked);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb2\xd8\xb1_\xd8\xaa\xd9\x85_\xd8\xa7\xd9\x84\xd9\x86\xd9\x82\xd8\xb1", button_is_clicked_func);
    interpreter.getFunctionManager().registerBuiltinFunction("button_is_clicked", button_is_clicked_func);

    // --- زر_تعيين_نص / button_set_text ---
    auto button_set_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("button_set_text: 2 arguments required");
        sad::stdlib::graphics::button_set_text_impl(args[0]->toInt(), args[1]->toString());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb2\xd8\xb1_\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd9\x86\xd8\xb5", button_set_text_func);
    interpreter.getFunctionManager().registerBuiltinFunction("button_set_text", button_set_text_func);

    // --- حقل_نص_جديد / textinput_new ---
    auto textinput_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 5) throw std::runtime_error("textinput_new: 5 arguments required");
        int inputId = sad::stdlib::graphics::textinput_create_impl(
            args[0]->toString(),
            static_cast<float>(args[1]->toDouble()), static_cast<float>(args[2]->toDouble()),
            static_cast<float>(args[3]->toDouble()), static_cast<float>(args[4]->toDouble()));
        return std::make_shared<Data::Value>(inputId);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xad\xd9\x82\xd9\x84_\xd9\x86\xd8\xb5_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", textinput_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("textinput_new", textinput_create_func);

    // --- حقل_نص_الحصول_على_قيمة / textinput_get_value ---
    auto textinput_get_value_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("textinput_get_value: TextInput argument required");
        std::string value = sad::stdlib::graphics::textinput_get_value_impl(args[0]->toInt());
        return std::make_shared<Data::Value>(value);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xad\xd9\x82\xd9\x84_\xd9\x86\xd8\xb5_\xd8\xa7\xd9\x84\xd8\xad\xd8\xb5\xd9\x88\xd9\x84_\xd8\xb9\xd9\x84\xd9\x89_\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9", textinput_get_value_func);
    interpreter.getFunctionManager().registerBuiltinFunction("textinput_get_value", textinput_get_value_func);

    // --- حقل_نص_تعيين_قيمة / textinput_set_value ---
    auto textinput_set_value_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("textinput_set_value: 2 arguments required");
        sad::stdlib::graphics::textinput_set_value_impl(args[0]->toInt(), args[1]->toString());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xad\xd9\x82\xd9\x84_\xd9\x86\xd8\xb5_\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9", textinput_set_value_func);
    interpreter.getFunctionManager().registerBuiltinFunction("textinput_set_value", textinput_set_value_func);

    // --- مربع_اختيار_جديد / checkbox_new ---
    auto checkbox_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 5) throw std::runtime_error("checkbox_new: 5 arguments required");
        int checkboxId = sad::stdlib::graphics::checkbox_create_impl(
            args[0]->toString(),
            static_cast<float>(args[1]->toDouble()), static_cast<float>(args[2]->toDouble()),
            static_cast<float>(args[3]->toDouble()), static_cast<float>(args[4]->toDouble()));
        return std::make_shared<Data::Value>(checkboxId);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd8\xb1\xd8\xa8\xd8\xb9_\xd8\xa7\xd8\xae\xd8\xaa\xd9\x8a\xd8\xa7\xd8\xb1_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", checkbox_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("checkbox_new", checkbox_create_func);

    // --- مربع_اختيار_محدد / checkbox_is_checked ---
    auto checkbox_is_checked_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("checkbox_is_checked: Checkbox argument required");
        bool checked = sad::stdlib::graphics::checkbox_is_checked_impl(args[0]->toInt());
        return std::make_shared<Data::Value>(checked);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd8\xb1\xd8\xa8\xd8\xb9_\xd8\xa7\xd8\xae\xd8\xaa\xd9\x8a\xd8\xa7\xd8\xb1_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf", checkbox_is_checked_func);
    interpreter.getFunctionManager().registerBuiltinFunction("checkbox_is_checked", checkbox_is_checked_func);

    // --- مربع_اختيار_تعيين_حالة / checkbox_set_checked ---
    auto checkbox_set_checked_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("checkbox_set_checked: 2 arguments required");
        sad::stdlib::graphics::checkbox_set_checked_impl(args[0]->toInt(), args[1]->toBool());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd8\xb1\xd8\xa8\xd8\xb9_\xd8\xa7\xd8\xae\xd8\xaa\xd9\x8a\xd8\xa7\xd8\xb1_\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9", checkbox_set_checked_func);
    interpreter.getFunctionManager().registerBuiltinFunction("checkbox_set_checked", checkbox_set_checked_func);

    // --- عنصر_رسم / widget_draw ---
    auto widget_draw_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("widget_draw: 2 arguments required (widget, renderer)");
        sad::stdlib::graphics::widget_draw_impl(args[0]->toInt(), args[1]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xb1\xd8\xb3\xd9\x85", widget_draw_func);
    interpreter.getFunctionManager().registerBuiltinFunction("widget_draw", widget_draw_func);

    // --- عنصر_تحديث / widget_update ---
    auto widget_update_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("widget_update: 2 arguments required (widget, deltaTime)");
        sad::stdlib::graphics::widget_update_impl(args[0]->toInt(), static_cast<float>(args[1]->toDouble()));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xaa\xd8\xad\xd8\xaf\xd9\x8a\xd8\xab", widget_update_func);
    interpreter.getFunctionManager().registerBuiltinFunction("widget_update", widget_update_func);

    // --- عنصر_حدث_فأرة / widget_mouse_event ---
    auto widget_mouse_event_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 4) throw std::runtime_error("widget_mouse_event: 4 arguments required (widget, x, y, pressed)");
        sad::stdlib::graphics::widget_mouse_event_impl(args[0]->toInt(), args[1]->toInt(), args[2]->toInt(), args[3]->toBool());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xad\xd8\xaf\xd8\xab_\xd9\x81\xd8\xa3\xd8\xb1\xd8\xa9", widget_mouse_event_func);
    interpreter.getFunctionManager().registerBuiltinFunction("widget_mouse_event", widget_mouse_event_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) معالجة المدخلات / (EN) Input Handling
    // ═══════════════════════════════════════════════════════════════════

    // --- مفتاح_مضغوط / key_is_pressed ---
    auto key_is_pressed_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("key_is_pressed: keycode argument required");
        bool pressed = sad::stdlib::graphics::key_is_pressed_impl(args[0]->toInt());
        return std::make_shared<Data::Value>(pressed);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad_\xd9\x85\xd8\xb6\xd8\xba\xd9\x88\xd8\xb7", key_is_pressed_func);
    interpreter.getFunctionManager().registerBuiltinFunction("key_is_pressed", key_is_pressed_func);

    // --- الفأرة_س / mouse_get_x ---
    auto mouse_get_x_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int x = sad::stdlib::graphics::mouse_get_x_impl();
        return std::make_shared<Data::Value>(x);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x84\xd9\x81\xd8\xa3\xd8\xb1\xd8\xa9_\xd8\xb3", mouse_get_x_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mouse_get_x", mouse_get_x_func);

    // --- الفأرة_ص / mouse_get_y ---
    auto mouse_get_y_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int y = sad::stdlib::graphics::mouse_get_y_impl();
        return std::make_shared<Data::Value>(y);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x84\xd9\x81\xd8\xa3\xd8\xb1\xd8\xa9_\xd8\xb5", mouse_get_y_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mouse_get_y", mouse_get_y_func);

    // --- زر_الفأرة_مضغوط / mouse_button_pressed ---
    auto mouse_button_pressed_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("mouse_button_pressed: button argument required (1=left, 2=middle, 3=right)");
        bool pressed = sad::stdlib::graphics::mouse_button_pressed_impl(args[0]->toInt());
        return std::make_shared<Data::Value>(pressed);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb2\xd8\xb1_\xd8\xa7\xd9\x84\xd9\x81\xd8\xa3\xd8\xb1\xd8\xa9_\xd9\x85\xd8\xb6\xd8\xba\xd9\x88\xd8\xb7", mouse_button_pressed_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mouse_button_pressed", mouse_button_pressed_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) إدارة النسيج والصور / (EN) Texture / Image Management
    // ═══════════════════════════════════════════════════════════════════

    // --- نسيج_تحميل / texture_load ---
    auto texture_load_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("texture_load: file path argument required");
        int texId = sad::stdlib::graphics::texture_load_impl(args[0]->toString());
        return std::make_shared<Data::Value>(texId);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd8\xb3\xd9\x8a\xd8\xac_\xd8\xaa\xd8\xad\xd9\x85\xd9\x8a\xd9\x84", texture_load_func);
    interpreter.getFunctionManager().registerBuiltinFunction("texture_load", texture_load_func);

    // --- نسيج_رسم / texture_draw ---
    auto texture_draw_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 6) throw std::runtime_error("texture_draw: 6 arguments required (renderer, texture, x, y, w, h)");
        sad::stdlib::graphics::texture_draw_impl(
            args[0]->toInt(), args[1]->toInt(),
            static_cast<float>(args[2]->toDouble()), static_cast<float>(args[3]->toDouble()),
            static_cast<float>(args[4]->toDouble()), static_cast<float>(args[5]->toDouble()));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd8\xb3\xd9\x8a\xd8\xac_\xd8\xb1\xd8\xb3\xd9\x85", texture_draw_func);
    interpreter.getFunctionManager().registerBuiltinFunction("texture_draw", texture_draw_func);

    // --- نسيج_تحرير / texture_unload ---
    auto texture_unload_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("texture_unload: texture argument required");
        sad::stdlib::graphics::texture_unload_impl(args[0]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd8\xb3\xd9\x8a\xd8\xac_\xd8\xaa\xd8\xad\xd8\xb1\xd9\x8a\xd8\xb1", texture_unload_func);
    interpreter.getFunctionManager().registerBuiltinFunction("texture_unload", texture_unload_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) نظام الصوت / (EN) Audio System
    // ═══════════════════════════════════════════════════════════════════

    // --- صوت_تهيئة / audio_init ---
    auto audio_init_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        bool ok = sad::stdlib::graphics::audio_init_impl();
        return std::make_shared<Data::Value>(ok);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xaa\xd9\x87\xd9\x8a\xd8\xa6\xd8\xa9", audio_init_func);
    interpreter.getFunctionManager().registerBuiltinFunction("audio_init", audio_init_func);

    // --- صوت_تحميل / sound_load ---
    auto sound_load_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("sound_load: 2 arguments required (filePath, name)");
        int soundId = sad::stdlib::graphics::sound_load_impl(args[0]->toString(), args[1]->toString());
        return std::make_shared<Data::Value>(soundId);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xaa\xd8\xad\xd9\x85\xd9\x8a\xd9\x84", sound_load_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sound_load", sound_load_func);

    // --- صوت_تشغيل / sound_play ---
    auto sound_play_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("sound_play: sound argument required");
        int channelId = sad::stdlib::graphics::sound_play_impl(args[0]->toInt());
        return std::make_shared<Data::Value>(channelId);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xaa\xd8\xb4\xd8\xba\xd9\x8a\xd9\x84", sound_play_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sound_play", sound_play_func);

    // --- صوت_إيقاف / sound_stop ---
    auto sound_stop_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("sound_stop: channel argument required");
        sad::stdlib::graphics::sound_stop_impl(args[0]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xa5\xd9\x8a\xd9\x82\xd8\xa7\xd9\x81", sound_stop_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sound_stop", sound_stop_func);

    // --- صوت_مستوى / sound_set_volume ---
    auto sound_set_volume_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("sound_set_volume: 2 arguments required (channel, volume)");
        sad::stdlib::graphics::sound_set_volume_impl(args[0]->toInt(), static_cast<float>(args[1]->toDouble()));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x88\xd9\x89", sound_set_volume_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sound_set_volume", sound_set_volume_func);

    // --- صوت_مستوى_رئيسي / audio_set_master_volume ---
    auto audio_set_master_volume_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("audio_set_master_volume: volume argument required");
        sad::stdlib::graphics::audio_set_master_volume_impl(static_cast<float>(args[0]->toDouble()));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x88\xd9\x89_\xd8\xb1\xd8\xa6\xd9\x8a\xd8\xb3\xd9\x8a", audio_set_master_volume_func);
    interpreter.getFunctionManager().registerBuiltinFunction("audio_set_master_volume", audio_set_master_volume_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) التحويلات / (EN) Transformations
    // ═══════════════════════════════════════════════════════════════════

    // --- رسام_حفظ_تحويل / renderer_push_transform ---
    auto renderer_push_transform_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("renderer_push_transform: Renderer argument required");
        sad::stdlib::graphics::renderer_push_transform_impl(args[0]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd8\xad\xd9\x81\xd8\xb8_\xd8\xaa\xd8\xad\xd9\x88\xd9\x8a\xd9\x84", renderer_push_transform_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_push_transform", renderer_push_transform_func);

    // --- رسام_استعادة_تحويل / renderer_pop_transform ---
    auto renderer_pop_transform_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("renderer_pop_transform: Renderer argument required");
        sad::stdlib::graphics::renderer_pop_transform_impl(args[0]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xb9\xd8\xa7\xd8\xaf\xd8\xa9_\xd8\xaa\xd8\xad\xd9\x88\xd9\x8a\xd9\x84", renderer_pop_transform_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_pop_transform", renderer_pop_transform_func);

    // --- رسام_إزاحة / renderer_translate ---
    auto renderer_translate_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) throw std::runtime_error("renderer_translate: 3 arguments required (renderer, x, y)");
        sad::stdlib::graphics::renderer_translate_impl(args[0]->toInt(),
            static_cast<float>(args[1]->toDouble()), static_cast<float>(args[2]->toDouble()));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd8\xa5\xd8\xb2\xd8\xa7\xd8\xad\xd8\xa9", renderer_translate_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_translate", renderer_translate_func);

    // --- رسام_دوران / renderer_rotate ---
    auto renderer_rotate_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("renderer_rotate: 2 arguments required (renderer, angle)");
        sad::stdlib::graphics::renderer_rotate_impl(args[0]->toInt(), static_cast<float>(args[1]->toDouble()));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd8\xaf\xd9\x88\xd8\xb1\xd8\xa7\xd9\x86", renderer_rotate_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_rotate", renderer_rotate_func);

    // --- رسام_تحجيم / renderer_scale ---
    auto renderer_scale_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) throw std::runtime_error("renderer_scale: 3 arguments required (renderer, sx, sy)");
        sad::stdlib::graphics::renderer_scale_impl(args[0]->toInt(),
            static_cast<float>(args[1]->toDouble()), static_cast<float>(args[2]->toDouble()));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd8\xaa\xd8\xad\xd8\xac\xd9\x8a\xd9\x85", renderer_scale_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_scale", renderer_scale_func);

    // --- رسام_إعادة_تحويل / renderer_reset_transform ---
    auto renderer_reset_transform_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("renderer_reset_transform: Renderer argument required");
        sad::stdlib::graphics::renderer_reset_transform_impl(args[0]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd8\xa5\xd8\xb9\xd8\xa7\xd8\xaf\xd8\xa9_\xd8\xaa\xd8\xad\xd9\x88\xd9\x8a\xd9\x84", renderer_reset_transform_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_reset_transform", renderer_reset_transform_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) إعدادات الرسام / (EN) Renderer Settings
    // ═══════════════════════════════════════════════════════════════════

    // --- رسام_لون_الرسم / renderer_set_draw_color ---
    auto renderer_set_draw_color_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 5) throw std::runtime_error("renderer_set_draw_color: 5 arguments required (renderer, r, g, b, a)");
        sad::stdlib::graphics::renderer_set_draw_color_impl(args[0]->toInt(),
            args[1]->toInt(), args[2]->toInt(), args[3]->toInt(), args[4]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd9\x84\xd9\x88\xd9\x86_\xd8\xa7\xd9\x84\xd8\xb1\xd8\xb3\xd9\x85", renderer_set_draw_color_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_set_draw_color", renderer_set_draw_color_func);

    // --- رسام_سمك_الخط / renderer_set_line_thickness ---
    auto renderer_set_line_thickness_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("renderer_set_line_thickness: 2 arguments required (renderer, thickness)");
        sad::stdlib::graphics::renderer_set_line_thickness_impl(args[0]->toInt(), static_cast<float>(args[1]->toDouble()));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd8\xb3\xd9\x85\xd9\x83_\xd8\xa7\xd9\x84\xd8\xae\xd8\xb7", renderer_set_line_thickness_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_set_line_thickness", renderer_set_line_thickness_func);

    // --- رسام_منفذ_العرض / renderer_set_viewport ---
    auto renderer_set_viewport_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 5) throw std::runtime_error("renderer_set_viewport: 5 arguments required (renderer, x, y, w, h)");
        sad::stdlib::graphics::renderer_set_viewport_impl(args[0]->toInt(),
            args[1]->toInt(), args[2]->toInt(), args[3]->toInt(), args[4]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd9\x85\xd9\x86\xd9\x81\xd8\xb0_\xd8\xa7\xd9\x84\xd8\xb9\xd8\xb1\xd8\xb6", renderer_set_viewport_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_set_viewport", renderer_set_viewport_func);

    // --- رسام_إسقاط / renderer_set_projection ---
    auto renderer_set_projection_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 5) throw std::runtime_error("renderer_set_projection: 5 arguments required (renderer, left, right, bottom, top)");
        sad::stdlib::graphics::renderer_set_projection_impl(args[0]->toInt(),
            static_cast<float>(args[1]->toDouble()), static_cast<float>(args[2]->toDouble()),
            static_cast<float>(args[3]->toDouble()), static_cast<float>(args[4]->toDouble()));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x85_\xd8\xa5\xd8\xb3\xd9\x82\xd8\xa7\xd8\xb7", renderer_set_projection_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_set_projection", renderer_set_projection_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) أدوات الوقت / (EN) Time Utilities
    // ═══════════════════════════════════════════════════════════════════

    // --- الوقت_الحالي / get_ticks ---
    auto get_ticks_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int ticks = sad::stdlib::graphics::get_ticks_impl();
        return std::make_shared<Data::Value>(ticks);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x84\xd9\x88\xd9\x82\xd8\xaa_\xd8\xa7\xd9\x84\xd8\xad\xd8\xa7\xd9\x84\xd9\x8a", get_ticks_func);
    interpreter.getFunctionManager().registerBuiltinFunction("get_ticks", get_ticks_func);

#endif // HAS_GRAPHICS
} // end registerBuiltinsPart9

} // namespace Interpreter
} // namespace Sad
