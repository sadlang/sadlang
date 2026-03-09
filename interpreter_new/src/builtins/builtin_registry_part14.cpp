/**
 * @file builtin_registry_part14.cpp
 * @brief (AR) تسجيل دوال v2.0 الجديدة: الشفافية، الظل، التلميحات، القيود، الأحداث الكاملة
 * @brief (EN) Register v2.0 new APIs: opacity, shadow, tooltip, constraints, full events, etc.
 *
 * 50+ إصلاح + 10 ميزات جديدة مسجلة هنا
 */

#include "builtins.h"
#include "interpreter_core.h"
#include <memory>
#include <string>
#include <vector>

// ───── sad::ui framework headers ─────
#include "graphics/sad_ui_framework.h"

namespace Sad {
namespace Interpreter {

void registerBuiltinsPart14(Interpreter& interpreter) {
#ifdef HAS_GRAPHICS

    // ═══════════════════════════════════════════════════════════════════
    // (AR) الشفافية / (EN) Opacity
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_شفافية / widget_set_opacity ---
    auto widget_set_opacity_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_opacity(args[0]->toInt(), static_cast<float>(args[1]->toDouble()));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xb4\xd9\x81\xd8\xa7\xd9\x81\xd9\x8a\xd8\xa9", widget_set_opacity_func); // عين_شفافية
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_opacity", widget_set_opacity_func);

    // --- اقرأ_شفافية / widget_get_opacity ---
    auto widget_get_opacity_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(1.0);
        return std::make_shared<Data::Value>(static_cast<double>(sad::ui::widget_get_opacity(args[0]->toInt())));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd8\xb4\xd9\x81\xd8\xa7\xd9\x81\xd9\x8a\xd8\xa9", widget_get_opacity_func); // اقرأ_شفافية
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_opacity", widget_get_opacity_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) التفعيل/التعطيل / (EN) Enable/Disable
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_مفعل / widget_set_enabled ---
    auto widget_set_enabled_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_enabled(args[0]->toInt(), args[1]->toBool());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x85\xd9\x81\xd8\xb9\xd9\x84", widget_set_enabled_func); // عين_مفعل
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_enabled", widget_set_enabled_func);

    // --- هل_مفعل / widget_is_enabled ---
    auto widget_is_enabled_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(false);
        return std::make_shared<Data::Value>(sad::ui::widget_is_enabled(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x87\xd9\x84_\xd9\x85\xd9\x81\xd8\xb9\xd9\x84", widget_is_enabled_func); // هل_مفعل
    interpreter.getFunctionManager().registerBuiltinFunction("widget_is_enabled", widget_is_enabled_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) الظل / (EN) Shadow
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_ظل / widget_set_shadow ---
    auto widget_set_shadow_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 4) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        float radius = static_cast<float>(args[1]->toDouble());
        float ox = args.size() > 2 ? static_cast<float>(args[2]->toDouble()) : 0;
        float oy = args.size() > 3 ? static_cast<float>(args[3]->toDouble()) : 2;
        int r = args.size() > 4 ? args[4]->toInt() : 0;
        int g = args.size() > 5 ? args[5]->toInt() : 0;
        int b = args.size() > 6 ? args[6]->toInt() : 0;
        int a = args.size() > 7 ? args[7]->toInt() : 64;
        sad::ui::widget_set_shadow(id, radius, ox, oy, r, g, b, a);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xb8\xd9\x84", widget_set_shadow_func); // عين_ظل
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_shadow", widget_set_shadow_func);

    // --- اقرأ_ظل / widget_get_shadow_radius ---
    auto widget_get_shadow_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0.0);
        return std::make_shared<Data::Value>(static_cast<double>(sad::ui::widget_get_shadow_radius(args[0]->toInt())));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd8\xb8\xd9\x84", widget_get_shadow_func); // اقرأ_ظل
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_shadow_radius", widget_get_shadow_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) التلميح / (EN) Tooltip
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_تلميح / widget_set_tooltip ---
    auto widget_set_tooltip_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_tooltip(args[0]->toInt(), args[1]->toString());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad", widget_set_tooltip_func); // عين_تلميح
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_tooltip", widget_set_tooltip_func);

    // --- اقرأ_تلميح / widget_get_tooltip ---
    auto widget_get_tooltip_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(sad::ui::widget_get_tooltip(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad", widget_get_tooltip_func); // اقرأ_تلميح
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_tooltip", widget_get_tooltip_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) قيود الحجم / (EN) Size Constraints
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_حد_ادنى / widget_set_min_size ---
    auto widget_set_min_size_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) return std::make_shared<Data::Value>();
        sad::ui::widget_set_min_size(args[0]->toInt(), static_cast<float>(args[1]->toDouble()), static_cast<float>(args[2]->toDouble()));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xad\xd8\xaf_\xd8\xa7\xd8\xaf\xd9\x86\xd9\x89", widget_set_min_size_func); // عين_حد_ادنى
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_min_size", widget_set_min_size_func);

    // --- عين_حد_اقصى / widget_set_max_size ---
    auto widget_set_max_size_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) return std::make_shared<Data::Value>();
        sad::ui::widget_set_max_size(args[0]->toInt(), static_cast<float>(args[1]->toDouble()), static_cast<float>(args[2]->toDouble()));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xad\xd8\xaf_\xd8\xa7\xd9\x82\xd8\xb5\xd9\x89", widget_set_max_size_func); // عين_حد_اقصى
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_max_size", widget_set_max_size_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) الهامش / (EN) Margin
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_هامش / widget_set_margin ---
    auto widget_set_margin_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        if (args.size() >= 5) {
            float t = static_cast<float>(args[1]->toDouble());
            float r = static_cast<float>(args[2]->toDouble());
            float b = static_cast<float>(args[3]->toDouble());
            float l = static_cast<float>(args[4]->toDouble());
            sad::ui::widget_set_margin4(id, t, r, b, l);
        } else if (args.size() >= 2) {
            sad::ui::widget_set_margin(id, static_cast<float>(args[1]->toDouble()));
        }
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x87\xd8\xa7\xd9\x85\xd8\xb4", widget_set_margin_func); // عين_هامش
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_margin", widget_set_margin_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) التدرج اللوني / (EN) Gradient
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_تدرج / widget_set_gradient ---
    auto widget_set_gradient_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 7) return std::make_shared<Data::Value>();
        sad::ui::widget_set_gradient(args[0]->toInt(),
            args[1]->toInt(), args[2]->toInt(), args[3]->toInt(),
            args[4]->toInt(), args[5]->toInt(), args[6]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac", widget_set_gradient_func); // عين_تدرج
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_gradient", widget_set_gradient_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) ألوان التحويم والضغط / (EN) Hover & Press Colors
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_لون_تحويم / widget_set_hover_color ---
    auto widget_set_hover_color_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 4) return std::make_shared<Data::Value>();
        sad::ui::widget_set_hover_color(args[0]->toInt(), args[1]->toInt(), args[2]->toInt(), args[3]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x84\xd9\x88\xd9\x86_\xd8\xaa\xd8\xad\xd9\x88\xd9\x8a\xd9\x85", widget_set_hover_color_func); // عين_لون_تحويم
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_hover_color", widget_set_hover_color_func);

    // --- عين_لون_ضغط / widget_set_press_color ---
    auto widget_set_press_color_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 4) return std::make_shared<Data::Value>();
        sad::ui::widget_set_press_color(args[0]->toInt(), args[1]->toInt(), args[2]->toInt(), args[3]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x84\xd9\x88\xd9\x86_\xd8\xb6\xd8\xba\xd8\xb7", widget_set_press_color_func); // عين_لون_ضغط
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_press_color", widget_set_press_color_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) خصائص النص المتقدمة / (EN) Advanced Text Properties
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_وزن_خط / widget_set_font_weight ---
    auto widget_set_font_weight_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_font_weight(args[0]->toInt(), args[1]->toString());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x88\xd8\xb2\xd9\x86_\xd8\xae\xd8\xb7", widget_set_font_weight_func); // عين_وزن_خط
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_font_weight", widget_set_font_weight_func);

    // --- عين_مائل / widget_set_italic ---
    auto widget_set_italic_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_italic(args[0]->toInt(), args[1]->toBool());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x85\xd8\xa7\xd8\xa6\xd9\x84", widget_set_italic_func); // عين_مائل
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_italic", widget_set_italic_func);

    // --- عين_ارتفاع_سطر / widget_set_line_height ---
    auto widget_set_line_height_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_line_height(args[0]->toInt(), static_cast<float>(args[1]->toDouble()));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9_\xd8\xb3\xd8\xb7\xd8\xb1", widget_set_line_height_func); // عين_ارتفاع_سطر
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_line_height", widget_set_line_height_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) التحويلات / (EN) Transforms
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_دوران / widget_set_rotation ---
    auto widget_set_rotation_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_rotation(args[0]->toInt(), static_cast<float>(args[1]->toDouble()));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xaf\xd9\x88\xd8\xb1\xd8\xa7\xd9\x86", widget_set_rotation_func); // عين_دوران
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_rotation", widget_set_rotation_func);

    // --- عين_تحجيم / widget_set_scale ---
    auto widget_set_scale_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) return std::make_shared<Data::Value>();
        sad::ui::widget_set_scale(args[0]->toInt(), static_cast<float>(args[1]->toDouble()), static_cast<float>(args[2]->toDouble()));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xaa\xd8\xad\xd8\xac\xd9\x8a\xd9\x85", widget_set_scale_func); // عين_تحجيم
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_scale", widget_set_scale_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) الترتيب / (EN) Ordering
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_ترتيب / widget_set_z_index ---
    auto widget_set_z_index_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_z_index(args[0]->toInt(), args[1]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xaa\xd8\xb1\xd8\xaa\xd9\x8a\xd8\xa8", widget_set_z_index_func); // عين_ترتيب
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_z_index", widget_set_z_index_func);

    // --- عين_ترتيب_تاب / widget_set_tab_index ---
    auto widget_set_tab_index_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_tab_index(args[0]->toInt(), args[1]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xaa\xd8\xb1\xd8\xaa\xd9\x8a\xd8\xa8_\xd8\xaa\xd8\xa7\xd8\xa8", widget_set_tab_index_func); // عين_ترتيب_تاب
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_tab_index", widget_set_tab_index_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) الأيقونة النصية / (EN) Icon Text (multi-char)
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_ايقونة_نص / widget_set_icon_text ---
    auto widget_set_icon_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_icon_text(args[0]->toInt(), args[1]->toString());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xa7\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9_\xd9\x86\xd8\xb5", widget_set_icon_text_func); // عين_ايقونة_نص
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_icon_text", widget_set_icon_text_func);

    // --- اقرأ_ايقونة_نص / widget_get_icon_text ---
    auto widget_get_icon_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(sad::ui::widget_get_icon_text(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd8\xa7\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9_\xd9\x86\xd8\xb5", widget_get_icon_text_func); // اقرأ_ايقونة_نص
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_icon_text", widget_get_icon_text_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) التمرير / (EN) Scroll Position
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_تمرير / widget_set_scroll ---
    auto widget_set_scroll_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) return std::make_shared<Data::Value>();
        sad::ui::widget_set_scroll_position(args[0]->toInt(), static_cast<float>(args[1]->toDouble()), static_cast<float>(args[2]->toDouble()));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1", widget_set_scroll_func); // عين_تمرير
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_scroll", widget_set_scroll_func);

    // --- اقرأ_تمرير_ص / widget_get_scroll_y ---
    auto widget_get_scroll_y_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0.0);
        return std::make_shared<Data::Value>(static_cast<double>(sad::ui::widget_get_scroll_y(args[0]->toInt())));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1_\xd8\xb5", widget_get_scroll_y_func); // اقرأ_تمرير_ص
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_scroll_y", widget_get_scroll_y_func);

    // --- اقرأ_تمرير_س / widget_get_scroll_x ---
    auto widget_get_scroll_x_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0.0);
        return std::make_shared<Data::Value>(static_cast<double>(sad::ui::widget_get_scroll_x(args[0]->toInt())));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1_\xd8\xb3", widget_get_scroll_x_func); // اقرأ_تمرير_س
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_scroll_x", widget_get_scroll_x_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) نوع الفائض / (EN) Overflow
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_فائض / widget_set_overflow ---
    auto widget_set_overflow_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_overflow(args[0]->toInt(), args[1]->toString());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x81\xd8\xa7\xd8\xa6\xd8\xb6", widget_set_overflow_func); // عين_فائض
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_overflow", widget_set_overflow_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) اسم المكون / (EN) Widget Name
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_اسم / widget_set_name ---
    auto widget_set_name_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_name(args[0]->toInt(), args[1]->toString());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xa7\xd8\xb3\xd9\x85", widget_set_name_func); // عين_اسم
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_name", widget_set_name_func);

    // --- اقرأ_اسم / widget_get_name ---
    auto widget_get_name_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(sad::ui::widget_get_name(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd8\xa7\xd8\xb3\xd9\x85", widget_get_name_func); // اقرأ_اسم
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_name", widget_get_name_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) استعلام المكون / (EN) Widget Query
    // ═══════════════════════════════════════════════════════════════════

    // --- اقرأ_نوع / widget_get_type_name ---
    auto widget_get_type_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(sad::ui::widget_get_type_name(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd9\x86\xd9\x88\xd8\xb9", widget_get_type_func); // اقرأ_نوع
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_type_name", widget_get_type_func);

    // --- عدد_الأبناء / widget_get_children_count ---
    auto widget_get_children_count_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0);
        return std::make_shared<Data::Value>(sad::ui::widget_get_children_count(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd8\xaf\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xa3\xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1", widget_get_children_count_func); // عدد_الأبناء
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_children_count", widget_get_children_count_func);

    // --- اقرأ_ابن / widget_get_child_at ---
    auto widget_get_child_at_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>(-1);
        return std::make_shared<Data::Value>(sad::ui::widget_get_child_at(args[0]->toInt(), args[1]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd8\xa7\xd8\xa8\xd9\x86", widget_get_child_at_func); // اقرأ_ابن
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_child_at", widget_get_child_at_func);

    // --- احذف_ابن / widget_remove_child ---
    auto widget_remove_child_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_remove_child(args[0]->toInt(), args[1]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xad\xd8\xb0\xd9\x81_\xd8\xa7\xd8\xa8\xd9\x86", widget_remove_child_func); // احذف_ابن
    interpreter.getFunctionManager().registerBuiltinFunction("widget_remove_child", widget_remove_child_func);

    // --- ابحث_بالاسم / widget_find_by_name ---
    auto widget_find_by_name_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(-1);
        return std::make_shared<Data::Value>(sad::ui::widget_find_by_name(args[0]->toString()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xa8\xd8\xad\xd8\xab_\xd8\xa8\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85", widget_find_by_name_func); // ابحث_بالاسم
    interpreter.getFunctionManager().registerBuiltinFunction("widget_find_by_name", widget_find_by_name_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) الموضع والحجم / (EN) Position & Size
    // ═══════════════════════════════════════════════════════════════════

    // --- اقرأ_س / widget_get_x ---
    auto widget_get_x_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0.0);
        return std::make_shared<Data::Value>(static_cast<double>(sad::ui::widget_get_x(args[0]->toInt())));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd8\xb3", widget_get_x_func); // اقرأ_س
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_x", widget_get_x_func);

    // --- اقرأ_ص / widget_get_y ---
    auto widget_get_y_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0.0);
        return std::make_shared<Data::Value>(static_cast<double>(sad::ui::widget_get_y(args[0]->toInt())));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd8\xb5", widget_get_y_func); // اقرأ_ص
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_y", widget_get_y_func);

    // --- اقرأ_عرض_المكون / widget_get_width ---
    auto widget_get_width_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0.0);
        return std::make_shared<Data::Value>(static_cast<double>(sad::ui::widget_get_width(args[0]->toInt())));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd8\xb9\xd8\xb1\xd8\xb6_\xd8\xa7\xd9\x84\xd9\x85\xd9\x83\xd9\x88\xd9\x86", widget_get_width_func); // اقرأ_عرض_المكون
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_width", widget_get_width_func);

    // --- اقرأ_ارتفاع_المكون / widget_get_height ---
    auto widget_get_height_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0.0);
        return std::make_shared<Data::Value>(static_cast<double>(sad::ui::widget_get_height(args[0]->toInt())));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9_\xd8\xa7\xd9\x84\xd9\x85\xd9\x83\xd9\x88\xd9\x86", widget_get_height_func); // اقرأ_ارتفاع_المكون
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_height", widget_get_height_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) خصائص إضافية / (EN) Additional Properties
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_مجموع_صفحات / widget_set_total_pages ---
    auto widget_set_total_pages_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_total_pages(args[0]->toInt(), args[1]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9_\xd8\xb5\xd9\x81\xd8\xad\xd8\xa7\xd8\xaa", widget_set_total_pages_func); // عين_مجموع_صفحات
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_total_pages", widget_set_total_pages_func);

    // --- عين_قيمة2 / widget_set_value2 ---
    auto widget_set_value2_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_value2(args[0]->toInt(), static_cast<float>(args[1]->toDouble()));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9" "2", widget_set_value2_func); // عين_قيمة2
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_value2", widget_set_value2_func);

    // --- اقرأ_قيمة2 / widget_get_value2 ---
    auto widget_get_value2_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0.0);
        return std::make_shared<Data::Value>(static_cast<double>(sad::ui::widget_get_value2(args[0]->toInt())));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9" "2", widget_get_value2_func); // اقرأ_قيمة2
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_value2", widget_get_value2_func);

    // --- عين_نص_ادخال / widget_set_input_text ---
    auto widget_set_input_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_input_text(args[0]->toInt(), args[1]->toString());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x86\xd8\xb5_\xd8\xa7\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84", widget_set_input_text_func); // عين_نص_ادخال
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_input_text", widget_set_input_text_func);

    // --- اقرأ_نص_ادخال / widget_get_input_text ---
    auto widget_get_input_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(sad::ui::widget_get_input_text(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd9\x86\xd8\xb5_\xd8\xa7\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84", widget_get_input_text_func); // اقرأ_نص_ادخال
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_input_text", widget_get_input_text_func);

    // --- امسح_عناصر / widget_clear_items ---
    auto widget_clear_items_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>();
        sad::ui::widget_clear_items(args[0]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x85\xd8\xb3\xd8\xad_\xd8\xb9\xd9\x86\xd8\xa7\xd8\xb5\xd8\xb1", widget_clear_items_func); // امسح_عناصر
    interpreter.getFunctionManager().registerBuiltinFunction("widget_clear_items", widget_clear_items_func);

    // --- امسح_بيانات / widget_clear_data ---
    auto widget_clear_data_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>();
        sad::ui::widget_clear_data(args[0]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x85\xd8\xb3\xd8\xad_\xd8\xa8\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa", widget_clear_data_func); // امسح_بيانات
    interpreter.getFunctionManager().registerBuiltinFunction("widget_clear_data", widget_clear_data_func);

    // --- أضف_لون_بيانات / widget_add_data_color ---
    auto widget_add_data_color_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 4) return std::make_shared<Data::Value>();
        sad::ui::widget_add_data_color(args[0]->toInt(), args[1]->toInt(), args[2]->toInt(), args[3]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd8\xb6\xd9\x81_\xd9\x84\xd9\x88\xd9\x86_\xd8\xa8\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa", widget_add_data_color_func); // أضف_لون_بيانات
    interpreter.getFunctionManager().registerBuiltinFunction("widget_add_data_color", widget_add_data_color_func);

    // --- عين_مقنع / widget_set_masked ---
    auto widget_set_masked_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_masked(args[0]->toInt(), args[1]->toBool());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x85\xd9\x82\xd9\x86\xd8\xb9", widget_set_masked_func); // عين_مقنع
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_masked", widget_set_masked_func);

    // --- هل_مقنع / widget_get_masked ---
    auto widget_get_masked_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(false);
        return std::make_shared<Data::Value>(sad::ui::widget_get_masked(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x87\xd9\x84_\xd9\x85\xd9\x82\xd9\x86\xd8\xb9", widget_get_masked_func); // هل_مقنع
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_masked", widget_get_masked_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) نظام الأحداث الكامل / (EN) Full Event Data Access
    // ═══════════════════════════════════════════════════════════════════

    // --- عدد_احداث / events_count ---
    auto events_count_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0);
        return std::make_shared<Data::Value>(sad::ui::events_count(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd8\xaf\xd8\xaf_\xd8\xa7\xd8\xad\xd8\xaf\xd8\xa7\xd8\xab", events_count_func); // عدد_احداث
    interpreter.getFunctionManager().registerBuiltinFunction("events_count", events_count_func);

    // --- نوع_حدث / event_get_type ---
    auto event_get_type_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(sad::ui::event_get_type(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd9\x88\xd8\xb9_\xd8\xad\xd8\xaf\xd8\xab", event_get_type_func); // نوع_حدث
    interpreter.getFunctionManager().registerBuiltinFunction("event_get_type", event_get_type_func);

    // --- معرف_حدث / event_get_widget_id ---
    auto event_get_widget_id_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(-1);
        return std::make_shared<Data::Value>(sad::ui::event_get_widget_id(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd8\xb9\xd8\xb1\xd9\x81_\xd8\xad\xd8\xaf\xd8\xab", event_get_widget_id_func); // معرف_حدث
    interpreter.getFunctionManager().registerBuiltinFunction("event_get_widget_id", event_get_widget_id_func);

    // --- نص_حدث / event_get_text ---
    auto event_get_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(sad::ui::event_get_text(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd8\xb5_\xd8\xad\xd8\xaf\xd8\xab", event_get_text_func); // نص_حدث
    interpreter.getFunctionManager().registerBuiltinFunction("event_get_text", event_get_text_func);

    // --- رقم_حدث / event_get_number ---
    auto event_get_number_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0.0);
        return std::make_shared<Data::Value>(sad::ui::event_get_number(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd9\x82\xd9\x85_\xd8\xad\xd8\xaf\xd8\xab", event_get_number_func); // رقم_حدث
    interpreter.getFunctionManager().registerBuiltinFunction("event_get_number", event_get_number_func);

    // --- منطقي_حدث / event_get_bool ---
    auto event_get_bool_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(false);
        return std::make_shared<Data::Value>(sad::ui::event_get_bool(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a_\xd8\xad\xd8\xaf\xd8\xab", event_get_bool_func); // منطقي_حدث
    interpreter.getFunctionManager().registerBuiltinFunction("event_get_bool", event_get_bool_func);

    // --- امسح_احداث / events_clear ---
    auto events_clear_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        sad::ui::events_clear();
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x85\xd8\xb3\xd8\xad_\xd8\xa7\xd8\xad\xd8\xaf\xd8\xa7\xd8\xab", events_clear_func); // امسح_احداث
    interpreter.getFunctionManager().registerBuiltinFunction("events_clear", events_clear_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) مكون() — منشئ شامل بأسلوب فلتر التصريحي — يدعم جميع 109 نوع
    // (EN) مكون() — Universal Flutter-like declarative widget builder — all 109 types
    // ═══════════════════════════════════════════════════════════════════
    //
    // Usage / الاستخدام:
    //   مكون("عمود"،  مكون("نص"، "مرحبا")،  مكون("زر"، "اضغط"))
    //   مكون("شبكة"، 3،  مكون("نص"، "1")،  مكون("نص"، "2"))
    //   مكون("منزلق"، 50، 0، 100)
    //   مكون("شريط_تقدم"، 75)
    //
    auto ui_builder_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(-1);
        std::string type = args[0]->toString();

        int id = -1;
        size_t childStart = 1;

        // === GROUP A: Containers (0 own params) ===
        if      (type == "\xd8\xb9\xd9\x85\xd9\x88\xd8\xaf" || type == "column")     { id = sad::ui::widget_column(); }
        else if (type == "\xd8\xb5\xd9\x81" || type == "row")                          { id = sad::ui::widget_row(); }
        else if (type == "\xd8\xaa\xd9\x83\xd8\xaf\xd9\x8a\xd8\xb3" || type == "stack")       { id = sad::ui::widget_stack(); }
        else if (type == "\xd8\xaa\xd9\x88\xd8\xb3\xd9\x8a\xd8\xb7" || type == "center")      { id = sad::ui::widget_center(); }
        else if (type == "\xd8\xad\xd8\xa7\xd9\x88\xd9\x8a\xd8\xa9" || type == "container")    { id = sad::ui::widget_container(); }
        else if (type == "\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9" || type == "card")         { id = sad::ui::widget_card(); }
        else if (type == "\xd8\xa7\xd9\x84\xd8\xaa\xd9\x81\xd8\xa7\xd9\x81" || type == "wrap") { id = sad::ui::widget_wrap(); }
        else if (type == "\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd8\xb9\xd8\xb1\xd8\xb6" || type == "listview")   { id = sad::ui::widget_listview(); }
        else if (type == "\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1" || type == "scrollview")   { id = sad::ui::widget_scrollview(); }
        else if (type == "\xd8\xaa\xd9\x88\xd8\xb3\xd9\x8a\xd8\xb9" || type == "expanded")     { id = sad::ui::widget_expanded(); }
        else if (type == "\xd8\xb5\xd9\x81\xd8\xad\xd8\xa7\xd8\xaa" || type == "pageview")     { id = sad::ui::widget_pageview(); }
        else if (type == "\xd8\xaf\xd8\xb1\xd8\xac" || type == "drawer")                { id = sad::ui::widget_drawer(); }
        else if (type == "\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9_\xd9\x85\xd8\xb4\xd8\xb1\xd9\x88\xd8\xb7\xd8\xa9" || type == "modal") { id = sad::ui::widget_modal(); }
        else if (type == "\xd8\xa7\xd9\x83\xd9\x88\xd8\xb1\xd8\xaf\xd9\x8a\xd9\x88\xd9\x86" || type == "accordion")   { id = sad::ui::widget_accordion(); }
        else if (type == "\xd8\xb9\xd8\xb1\xd8\xb6_\xd9\x85\xd9\x82\xd8\xb3\xd9\x85" || type == "split_view")        { id = sad::ui::widget_split_view(); }
        else if (type == "\xd8\xaf\xd9\x88\xd8\xa7\xd8\xb1" || type == "carousel")     { id = sad::ui::widget_carousel(); }
        else if (type == "\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd8\xac\xd8\xa7\xd9\x86\xd8\xa8\xd9\x8a\xd8\xa9" || type == "sidemenu") { id = sad::ui::widget_side_menu(); }
        else if (type == "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xa7\xd8\xaf\xd9\x88\xd8\xa7\xd8\xaa" || type == "toolbar") { id = sad::ui::widget_toolbar(); }
        else if (type == "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9" || type == "statusbar") { id = sad::ui::widget_status_bar(""); }
        else if (type == "\xd9\x85\xd8\xb3\xd8\xa7\xd8\xb1" || type == "breadcrumb")   { id = sad::ui::widget_breadcrumb(); }
        else if (type == "\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\xd8\xa9_\xd8\xa7\xd8\xb2\xd8\xb1\xd8\xa7\xd8\xb1" || type == "buttongroup") { id = sad::ui::widget_button_group(); }
        else if (type == "\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\xd8\xa9_\xd8\xaa\xd8\xa8\xd8\xaf\xd9\x8a\xd9\x84" || type == "togglegroup") { id = sad::ui::widget_toggle_group(); }
        else if (type == "\xd8\xae\xd8\xb7_\xd8\xb2\xd9\x85\xd9\x86\xd9\x8a" || type == "timeline") { id = sad::ui::widget_timeline(); }
        else if (type == "\xd9\x84\xd9\x88\xd8\xad\xd8\xa9_\xd8\xb1\xd8\xb3\xd9\x85" || type == "canvas") { id = sad::ui::widget_canvas(400, 300); }

        // === GROUP B: Text-based (1 string param) ===
        else if (type == "\xd9\x86\xd8\xb5" || type == "text") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_text(t); childStart = 2;
        }
        else if (type == "\xd8\xb2\xd8\xb1" || type == "button") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_button(t); childStart = 2;
        }
        else if (type == "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86" || type == "heading") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            int lv = args.size() > 2 ? args[2]->toInt() : 1;
            id = sad::ui::widget_heading(t, lv); childStart = 3;
        }
        else if (type == "\xd9\x81\xd9\x82\xd8\xb1\xd8\xa9" || type == "paragraph") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_paragraph(t); childStart = 2;
        }
        else if (type == "\xd8\xaa\xd8\xb3\xd9\x85\xd9\x8a\xd8\xa9" || type == "label") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_label(t); childStart = 2;
        }
        else if (type == "\xd8\xb1\xd8\xa7\xd8\xa8\xd8\xb7" || type == "link") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            std::string url = args.size() > 2 ? args[2]->toString() : "";
            id = sad::ui::widget_link(t); childStart = 2;
        }
        else if (type == "\xd9\x83\xd9\x88\xd8\xaf" || type == "codeblock") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_code_block(t); childStart = 2;
        }
        else if (type == "\xd8\xa7\xd9\x82\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb3" || type == "quote") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_quote(t); childStart = 2;
        }
        else if (type == "\xd9\x85\xd8\xa7\xd8\xb1\xd9\x83\xd8\xaf\xd8\xa7\xd9\x88\xd9\x86" || type == "markdown") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_markdown(t); childStart = 2;
        }

        // === GROUP C: Input fields (1 string param) ===
        else if (type == "\xd8\xad\xd9\x82\xd9\x84" || type == "\xd8\xad\xd9\x82\xd9\x84_\xd9\x86\xd8\xb5" || type == "textfield") {
            std::string ph = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_textfield(ph); childStart = 2;
        }
        else if (type == "\xd8\xad\xd9\x82\xd9\x84_\xd8\xa8\xd8\xad\xd8\xab" || type == "searchfield") {
            std::string ph = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_search_field(ph); childStart = 2;
        }
        else if (type == "\xd8\xad\xd9\x82\xd9\x84_\xd8\xb3\xd8\xb1" || type == "passwordfield") {
            std::string ph = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_password_field(ph); childStart = 2;
        }
        else if (type == "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd8\xa9_\xd9\x86\xd8\xb5" || type == "textarea") {
            std::string ph = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_textarea(ph); childStart = 2;
        }
        else if (type == "\xd8\xad\xd9\x82\xd9\x84_\xd8\xaa\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xae" || type == "datefield") {
            std::string ph = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_date_field(); childStart = 1;
        }
        else if (type == "\xd8\xa7\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84_\xd8\xb1\xd9\x82\xd9\x85" || type == "numberinput") {
            double v = args.size() > 1 ? args[1]->toDouble() : 0;
            id = sad::ui::widget_number_input(0, 100); childStart = 1;
        }

        // === GROUP D: Toggle/Check widgets (1 optional bool param) ===
        else if (type == "\xd9\x85\xd8\xb1\xd8\xa8\xd8\xb9_\xd8\xa7\xd8\xae\xd8\xaa\xd9\x8a\xd8\xa7\xd8\xb1" || type == "checkbox") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_checkbox(t); childStart = 2;
        }
        else if (type == "\xd8\xb1\xd8\xa7\xd8\xaf\xd9\x8a\xd9\x88" || type == "radio") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_radio(t); childStart = 2;
        }
        else if (type == "\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad" || type == "switch") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_switch(t); childStart = 2;
        }

        // === GROUP E: Sliders & progress (numeric params) ===
        else if (type == "\xd9\x85\xd9\x86\xd8\xb2\xd9\x84\xd9\x82" || type == "slider") {
            double v = args.size() > 1 ? args[1]->toDouble() : 50;
            double mn = args.size() > 2 ? args[2]->toDouble() : 0;
            double mx = args.size() > 3 ? args[3]->toDouble() : 100;
            id = sad::ui::widget_slider((float)mn, (float)mx); childStart = 4;
        }
        else if (type == "\xd9\x85\xd9\x86\xd8\xb2\xd9\x84\xd9\x82_\xd9\x86\xd8\xb7\xd8\xa7\xd9\x82" || type == "rangeslider") {
            double v1 = args.size() > 1 ? args[1]->toDouble() : 20;
            double v2 = args.size() > 2 ? args[2]->toDouble() : 80;
            id = sad::ui::widget_range_slider(0, 100); childStart = 1;
        }
        else if (type == "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85" || type == "progressbar") {
            double v = args.size() > 1 ? args[1]->toDouble() : 0;
            id = sad::ui::widget_progress_bar(v); childStart = 2;
        }
        else if (type == "\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85_\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd9\x8a" || type == "circleprogress") {
            double v = args.size() > 1 ? args[1]->toDouble() : 0;
            id = sad::ui::widget_circle_progress(v); childStart = 2;
        }
        else if (type == "\xd9\x85\xd9\x82\xd9\x8a\xd8\xa7\xd8\xb3" || type == "gauge") {
            double v = args.size() > 1 ? args[1]->toDouble() : 0;
            id = sad::ui::widget_gauge((float)v, 100); childStart = 2;
        }
        else if (type == "\xd8\xaa\xd9\x82\xd9\x8a\xd9\x8a\xd9\x85" || type == "rating") {
            double v = args.size() > 1 ? args[1]->toDouble() : 0;
            double mx = args.size() > 2 ? args[2]->toDouble() : 5;
            int mx_i = args.size() > 2 ? args[2]->toInt() : 5;
            id = sad::ui::widget_rating_stars(mx_i); childStart = 3;
        }

        // === GROUP F: Buttons (1 string param) ===
        else if (type == "\xd8\xb2\xd8\xb1_\xd8\xa7\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9" || type == "iconbutton") {
            char ic = (args.size() > 1 && !args[1]->toString().empty()) ? args[1]->toString()[0] : '*';
            id = sad::ui::widget_icon_button(ic); childStart = 2;
        }
        else if (type == "\xd8\xb2\xd8\xb1_\xd8\xb9\xd8\xa7\xd8\xa6\xd9\x85" || type == "fab") {
            char ic = (args.size() > 1 && !args[1]->toString().empty()) ? args[1]->toString()[0] : '+';
            id = sad::ui::widget_floating_button(ic); childStart = 2;
        }
        else if (type == "\xd8\xb2\xd8\xb1_\xd9\x85\xd8\xad\xd9\x8a\xd8\xb7" || type == "outlinebutton") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_outline_button(t); childStart = 2;
        }
        else if (type == "\xd8\xb2\xd8\xb1_\xd9\x86\xd8\xb5\xd9\x8a" || type == "textbutton") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_text_button(t); childStart = 2;
        }
        else if (type == "\xd8\xb2\xd8\xb1_\xd8\xaa\xd8\xa8\xd8\xaf\xd9\x8a\xd9\x84" || type == "togglebutton") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_toggle_button(t); childStart = 2;
        }

        // === GROUP G: Visual widgets ===
        else if (type == "\xd8\xa7\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9" || type == "icon") {
            char ic = (args.size() > 1 && !args[1]->toString().empty()) ? args[1]->toString()[0] : '*';
            id = sad::ui::widget_icon(ic); childStart = 2;
        }
        else if (type == "\xd8\xb5\xd9\x88\xd8\xb1\xd8\xa9_\xd8\xb1\xd9\x85\xd8\xb2\xd9\x8a\xd8\xa9" || type == "avatar") {
            std::string letter = (args.size() > 1) ? args[1]->toString() : "U";
            id = sad::ui::widget_avatar(letter, 70, 130, 180); childStart = 2;
        }
        else if (type == "\xd8\xb4\xd8\xa7\xd8\xb1\xd8\xa9" || type == "badge") {
            int count = args.size() > 1 ? args[1]->toInt() : 0;
            id = sad::ui::widget_badge(count); childStart = 2;
        }
        else if (type == "\xd8\xb1\xd9\x82\xd8\xa7\xd9\x82\xd8\xa9" || type == "chip") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_chip(t); childStart = 2;
        }
        else if (type == "\xd9\x88\xd8\xb3\xd9\x85" || type == "tag") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_tag(t, 70, 130, 200); childStart = 2;
        }
        else if (type == "\xd9\x86\xd9\x82\xd8\xb7\xd8\xa9_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9" || type == "statusdot") {
            id = sad::ui::widget_status_dot(0, 200, 0); childStart = 1;
        }
        else if (type == "\xd8\xb5\xd9\x86\xd8\xaf\xd9\x88\xd9\x82_\xd9\x84\xd9\x88\xd9\x86" || type == "colorbox") {
            id = sad::ui::widget_color_box(100, 100, 200); childStart = 1;
        }
        else if (type == "\xd8\xb5\xd9\x88\xd8\xb1\xd8\xa9" || type == "image") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_image(t); childStart = 2;
        }

        // === GROUP H: Navigational ===
        else if (type == "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82" || type == "appbar") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_app_bar(t); childStart = 2;
        }
        else if (type == "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xb3\xd9\x81\xd9\x84\xd9\x8a" || type == "bottombar") {
            id = sad::ui::widget_bottom_bar(); childStart = 1;
        }
        else if (type == "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd8\xa8\xd9\x88\xd9\x8a\xd8\xa8" || type == "tabbar") {
            id = sad::ui::widget_tab_bar(); childStart = 1;
        }
        else if (type == "\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xaa\xd9\x86\xd9\x82\xd9\x84" || type == "navitem") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_nav_item(t); childStart = 2;
        }
        else if (type == "\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9" || type == "menuitem") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_menu_item(t); childStart = 2;
        }

        // === GROUP I: Alerts & dialogs ===
        else if (type == "\xd8\xaa\xd9\x86\xd8\xa8\xd9\x8a\xd9\x87" || type == "alert") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            std::string tp = args.size() > 2 ? args[2]->toString() : "info";
            id = sad::ui::widget_alert(t, tp); childStart = 3;
        }
        else if (type == "\xd8\xa7\xd8\xb4\xd8\xb9\xd8\xa7\xd8\xb1_\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa" || type == "toast") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_toast(t); childStart = 2;
        }
        else if (type == "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xa7\xd8\xb4\xd8\xb9\xd8\xa7\xd8\xb1" || type == "snackbar") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_snackbar(t); childStart = 2;
        }
        else if (type == "\xd9\x84\xd8\xa7\xd9\x81\xd8\xaa\xd8\xa9" || type == "banner") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_banner(t); childStart = 2;
        }
        else if (type == "\xd8\xad\xd9\x88\xd8\xa7\xd8\xb1" || type == "dialog") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            std::string m = args.size() > 2 ? args[2]->toString() : "";
            id = sad::ui::widget_dialog(t, m); childStart = 3;
        }
        else if (type == "\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad" || type == "tooltip") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_tooltip(t); childStart = 2;
        }

        // === GROUP J: Data display ===
        else if (type == "\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xa8\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa" || type == "listtile") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            std::string sub = args.size() > 2 ? args[2]->toString() : "";
            id = sad::ui::widget_list_tile(t, sub); childStart = 3;
        }
        else if (type == "\xd8\xb5\xd9\x81_\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb5\xd9\x8a\xd9\x84" || type == "detailrow") {
            std::string lbl = args.size() > 1 ? args[1]->toString() : "";
            std::string val = args.size() > 2 ? args[2]->toString() : "";
            id = sad::ui::widget_detail_row(lbl, val); childStart = 3;
        }
        else if (type == "\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9_\xd9\x85\xd8\xb9\xd9\x84\xd9\x88\xd9\x85\xd8\xa7\xd8\xaa" || type == "infocard") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            std::string d = args.size() > 2 ? args[2]->toString() : "";
            id = sad::ui::widget_info_card(t, d); childStart = 3;
        }
        else if (type == "\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9_\xd8\xa7\xd8\xad\xd8\xb5\xd8\xa7\xd8\xa6\xd9\x8a\xd8\xa7\xd8\xaa" || type == "statcard") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            std::string v = args.size() > 2 ? args[2]->toString() : "";
            id = sad::ui::widget_stat_card(t, v); childStart = 3;
        }
        else if (type == "\xd9\x84\xd9\x88\xd8\xad\xd8\xa9_\xd8\xaa\xd9\x88\xd8\xb3\xd9\x8a\xd8\xb9" || type == "expansion") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_expansion_panel(t); childStart = 2;
        }
        else if (type == "\xd8\xb5\xd9\x86\xd8\xaf\xd9\x88\xd9\x82_\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\xd8\xa9" || type == "groupbox") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_group_box(t); childStart = 2;
        }
        else if (type == "\xd8\xaa\xd9\x86\xd9\x88\xd9\x8a\xd9\x87" || type == "callout") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_callout(t, "info"); childStart = 2;
        }
        else if (type == "\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xb2\xd9\x85\xd9\x86\xd9\x8a" || type == "timelineitem") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            std::string d = args.size() > 2 ? args[2]->toString() : "";
            id = sad::ui::widget_timeline_item(t); childStart = 3;
        }
        else if (type == "\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xb4\xd8\xac\xd8\xb1\xd8\xa9" || type == "treeitem") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            int lv = args.size() > 2 ? args[2]->toInt() : 0;
            id = sad::ui::widget_tree_item(t, lv); childStart = 3;
        }

        // === GROUP K: Charts ===
        else if (type == "\xd8\xb1\xd8\xb3\xd9\x85_\xd8\xa7\xd8\xb9\xd9\x85\xd8\xaf\xd8\xa9" || type == "barchart") {
            id = sad::ui::widget_bar_chart(); childStart = 1;
        }
        else if (type == "\xd8\xb1\xd8\xb3\xd9\x85_\xd8\xa7\xd9\x81\xd9\x82\xd9\x8a" || type == "hbarchart") {
            id = sad::ui::widget_hbar_chart(); childStart = 1;
        }
        else if (type == "\xd8\xb1\xd8\xb3\xd9\x85_\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd9\x8a" || type == "piechart") {
            id = sad::ui::widget_pie_chart(); childStart = 1;
        }
        else if (type == "\xd8\xb1\xd8\xb3\xd9\x85_\xd8\xae\xd8\xb7\xd9\x8a" || type == "linechart") {
            id = sad::ui::widget_line_chart(); childStart = 1;
        }
        else if (type == "\xd8\xb1\xd8\xb3\xd9\x85_\xd8\xad\xd9\x84\xd9\x82\xd9\x8a" || type == "donutchart") {
            id = sad::ui::widget_donut_chart(); childStart = 1;
        }
        else if (type == "\xd8\xae\xd8\xb7_\xd9\x85\xd8\xb5\xd8\xba\xd8\xb1" || type == "sparkline") {
            id = sad::ui::widget_spark_line(); childStart = 1;
        }

        // === GROUP L: Misc ===
        else if (type == "\xd9\x81\xd8\xa7\xd8\xb5\xd9\x84" || type == "spacer") {
            float sz = args.size() > 1 ? (float)args[1]->toDouble() : 8;
            id = sad::ui::widget_spacer(sz); childStart = 2;
        }
        else if (type == "\xd8\xae\xd8\xb7_\xd9\x81\xd8\xa7\xd8\xb5\xd9\x84" || type == "divider") {
            id = sad::ui::widget_divider(); childStart = 1;
        }
        else if (type == "\xd8\xaa\xd8\xad\xd9\x85\xd9\x8a\xd9\x84" || type == "spinner") {
            id = sad::ui::widget_loading_spinner(); childStart = 1;
        }
        else if (type == "\xd9\x87\xd9\x8a\xd9\x83\xd9\x84" || type == "skeleton") {
            id = sad::ui::widget_skeleton(); childStart = 1;
        }
        else if (type == "\xd8\xaa\xd8\xb1\xd9\x82\xd9\x8a\xd9\x85" || type == "pagination") {
            int total = args.size() > 1 ? args[1]->toInt() : 5;
            id = sad::ui::widget_pagination(total); childStart = 2;
        }
        else if (type == "\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1_\xd9\x86\xd9\x82\xd8\xb7\xd9\x8a" || type == "dotindicator") {
            int total = args.size() > 1 ? args[1]->toInt() : 3;
            int cur = args.size() > 2 ? args[2]->toInt() : 0;
            id = sad::ui::widget_dot_indicator(total, cur); childStart = 2;
        }
        else if (type == "\xd9\x85\xd9\x86\xd8\xaa\xd9\x82\xd9\x8a_\xd9\x84\xd9\x88\xd9\x86" || type == "colorpicker") {
            id = sad::ui::widget_color_picker(); childStart = 1;
        }
        else if (type == "\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd9\x85\xd9\x86\xd8\xb3\xd8\xaf\xd9\x84\xd8\xa9" || type == "dropdown") {
            std::string hint = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_dropdown(hint); childStart = 2;
        }
        else if (type == "\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9" || type == "gridview") {
            int cols = args.size() > 1 ? args[1]->toInt() : 2;
            id = sad::ui::widget_gridview(cols); childStart = 2;
        }
        else if (type == "\xd8\xb5\xd9\x86\xd8\xaf\xd9\x88\xd9\x82" || type == "sizedbox") {
            float sw = args.size() > 1 ? (float)args[1]->toDouble() : 100;
            float sh = args.size() > 2 ? (float)args[2]->toDouble() : 100;
            id = sad::ui::widget_sizedbox(sw, sh); childStart = 3;
        }
        else if (type == "\xd8\xac\xd8\xaf\xd9\x88\xd9\x84" || type == "table") {
            int cols = args.size() > 1 ? args[1]->toInt() : 2;
            id = sad::ui::widget_table(cols); childStart = 2;
        }
        else if (type == "\xd9\x84\xd9\x88\xd8\xad\xd8\xa9" || type == "panel") {
            std::string t = args.size() > 1 ? args[1]->toString() : "";
            id = sad::ui::widget_panel(t); childStart = 2;
        }
        else if (type == "\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac" || type == "gradientbox") {
            id = sad::ui::widget_gradient_box(100, 149, 237, 255, 107, 107); childStart = 1;
        }
        else {
            // نوع غير معروف — نعيد -1
            return std::make_shared<Data::Value>(-1);
        }

        if (id < 0) return std::make_shared<Data::Value>(-1);

        // Auto-add remaining int args as children (Flutter-like nesting)
        for (size_t i = childStart; i < args.size(); ++i) {
            int cid = args[i]->toInt();
            if (cid >= 0) sad::ui::widget_add_child(id, cid);
        }

        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd9\x83\xd9\x88\xd9\x86", ui_builder_func); // مكون
    interpreter.getFunctionManager().registerBuiltinFunction("ui_widget", ui_builder_func);

#endif // HAS_GRAPHICS
} // registerBuiltinsPart14

} // namespace Interpreter
} // namespace Sad
