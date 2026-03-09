/**
 * @file builtin_registry_part12.cpp
 * @brief (AR) تسجيل دوال خصائص المكونات، الشجرة، النص، الأزرار، حقول الإدخال
 * @brief (EN) Register widget property setters, tree ops, text/button/textfield APIs
 */

#include "builtins.h"
#include "interpreter_core.h"
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

// ───── sad::ui framework headers ─────
#include "graphics/sad_ui_framework.h"

namespace Sad {
namespace Interpreter {

void registerBuiltinsPart12(Interpreter& interpreter) {
#ifdef HAS_GRAPHICS

    // ═══════════════════════════════════════════════════════════════════
    // (AR) عمليات شجرة المكونات / (EN) Widget Tree Operations
    // ═══════════════════════════════════════════════════════════════════

    // --- أضف_ابن / widget_add_child ---
    auto widget_add_child_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        int parentId = args[0]->toInt();
        int childId  = args[1]->toInt();
        sad::ui::widget_add_child(parentId, childId);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd8\xb6\xd9\x81_\xd8\xa7\xd8\xa8\xd9\x86", widget_add_child_func); // أضف_ابن
    interpreter.getFunctionManager().registerBuiltinFunction("widget_add_child", widget_add_child_func);

    // --- امسح_الأبناء / widget_clear_children ---
    auto widget_clear_children_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>();
        int parentId = args[0]->toInt();
        sad::ui::widget_clear_children(parentId);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x85\xd8\xb3\xd8\xad_\xd8\xa7\xd9\x84\xd8\xa3\xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1", widget_clear_children_func); // امسح_الأبناء
    interpreter.getFunctionManager().registerBuiltinFunction("widget_clear_children", widget_clear_children_func);

    // --- عين_أبناء / widget_set_children ---
    auto widget_set_children_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        int parentId = args[0]->toInt();
        // Remaining args are child IDs
        std::vector<int> childIds;
        for (size_t i = 1; i < args.size(); i++) {
            childIds.push_back(args[i]->toInt());
        }
        sad::ui::widgetSetChildren(parentId, childIds);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xa3\xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1", widget_set_children_func); // عين_أبناء
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_children", widget_set_children_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) خصائص المكونات الأساسية / (EN) Widget Properties
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_خلفية / widget_set_bg ---
    auto widget_set_bg_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 4) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        int r = args[1]->toInt(), g = args[2]->toInt(), b = args[3]->toInt();
        sad::ui::widget_set_bg(id, r, g, b);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9", widget_set_bg_func); // عين_خلفية
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_bg", widget_set_bg_func);

    // --- عين_حشوة / widget_set_padding ---
    auto widget_set_padding_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        if (args.size() >= 5) {
            float t = static_cast<float>(args[1]->toDouble());
            float r = static_cast<float>(args[2]->toDouble());
            float b = static_cast<float>(args[3]->toDouble());
            float l = static_cast<float>(args[4]->toDouble());
            sad::ui::widget_set_padding4(id, t, r, b, l);
        } else if (args.size() >= 2) {
            float all = static_cast<float>(args[1]->toDouble());
            sad::ui::widget_set_padding(id, all);
        }
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xad\xd8\xb4\xd9\x88\xd8\xa9", widget_set_padding_func); // عين_حشوة
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_padding", widget_set_padding_func);

    // --- عين_ارتفاع / widget_set_height ---
    auto widget_set_height_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        float h = static_cast<float>(args[1]->toDouble());
        sad::ui::widget_set_height(id, h);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9", widget_set_height_func); // عين_ارتفاع
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_height", widget_set_height_func);

    // --- عين_عرض / widget_set_width ---
    auto widget_set_width_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        float w = static_cast<float>(args[1]->toDouble());
        sad::ui::widget_set_width(id, w);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xb9\xd8\xb1\xd8\xb6", widget_set_width_func); // عين_عرض
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_width", widget_set_width_func);

    // --- عين_مرونة / widget_set_flex ---
    auto widget_set_flex_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        float flex = static_cast<float>(args[1]->toDouble());
        sad::ui::widget_set_flex(id, flex);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x85\xd8\xb1\xd9\x88\xd9\x86\xd8\xa9", widget_set_flex_func); // عين_مرونة
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_flex", widget_set_flex_func);

    // --- عين_استدارة / widget_set_border_radius ---
    auto widget_set_border_radius_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        float radius = static_cast<float>(args[1]->toDouble());
        sad::ui::widget_set_border_radius(id, radius);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xaf\xd8\xa7\xd8\xb1\xd8\xa9", widget_set_border_radius_func); // عين_استدارة
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_border_radius", widget_set_border_radius_func);

    // --- عين_تباعد / widget_set_spacing ---
    auto widget_set_spacing_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        float gap = static_cast<float>(args[1]->toDouble());
        sad::ui::widget_set_spacing(id, gap);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xaf", widget_set_spacing_func); // عين_تباعد
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_spacing", widget_set_spacing_func);

    // --- عين_مرئي / widget_set_visible ---
    auto widget_set_visible_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        bool v = args[1]->toBool();
        sad::ui::widget_set_visible(id, v);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x85\xd8\xb1\xd8\xa6\xd9\x8a", widget_set_visible_func); // عين_مرئي
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_visible", widget_set_visible_func);

    // --- عين_حدود / widget_set_border ---
    auto widget_set_border_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 5) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        float width = static_cast<float>(args[1]->toDouble());
        int r = args[2]->toInt(), g = args[3]->toInt(), b = args[4]->toInt();
        sad::ui::widget_set_border(id, width, r, g, b);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xad\xd8\xaf\xd9\x88\xd8\xaf", widget_set_border_func); // عين_حدود
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_border", widget_set_border_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) خصائص النص / (EN) Text Properties
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_حجم_نص / text_set_size ---
    auto text_set_size_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        float size = static_cast<float>(args[1]->toDouble());
        sad::ui::text_set_size(id, size);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xad\xd8\xac\xd9\x85_\xd9\x86\xd8\xb5", text_set_size_func); // عين_حجم_نص
    interpreter.getFunctionManager().registerBuiltinFunction("text_set_size", text_set_size_func);

    // --- عين_لون_نص / text_set_color ---
    auto text_set_color_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 4) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        int r = args[1]->toInt(), g = args[2]->toInt(), b = args[3]->toInt();
        sad::ui::text_set_color(id, r, g, b);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x84\xd9\x88\xd9\x86_\xd9\x86\xd8\xb5", text_set_color_func); // عين_لون_نص
    interpreter.getFunctionManager().registerBuiltinFunction("text_set_color", text_set_color_func);

    // --- عين_محتوى / text_set_content ---
    auto text_set_content_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        std::string content = args[1]->toString();
        sad::ui::text_set_content(id, content);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89", text_set_content_func); // عين_محتوى
    interpreter.getFunctionManager().registerBuiltinFunction("text_set_content", text_set_content_func);

    // --- اقرأ_محتوى / text_get_content ---
    auto text_get_content_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(std::string(""));
        int id = args[0]->toInt();
        return std::make_shared<Data::Value>(sad::ui::text_get_content(id));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89", text_get_content_func); // اقرأ_محتوى
    interpreter.getFunctionManager().registerBuiltinFunction("text_get_content", text_get_content_func);

    // --- عين_محاذاة / text_set_align ---
    auto text_set_align_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        std::string align = args[1]->toString();
        sad::ui::text_set_align(id, align);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x85\xd8\xad\xd8\xa7\xd8\xb0\xd8\xa7\xd8\xa9", text_set_align_func); // عين_محاذاة
    interpreter.getFunctionManager().registerBuiltinFunction("text_set_align", text_set_align_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) خصائص الأزرار / (EN) Button Properties
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_لون_زر / button_set_color ---
    auto button_set_color_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 4) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        int r = args[1]->toInt(), g = args[2]->toInt(), b = args[3]->toInt();
        sad::ui::button_set_color(id, r, g, b);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x84\xd9\x88\xd9\x86_\xd8\xb2\xd8\xb1", button_set_color_func); // عين_لون_زر
    interpreter.getFunctionManager().registerBuiltinFunction("button_set_color", button_set_color_func);

    // --- هل_ضغط_الزر / button_was_clicked ---
    auto button_was_clicked_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(false);
        // Support both int ID and string name
        if (args[0]->getType() == Data::ValueType::STRING) {
            return std::make_shared<Data::Value>(sad::ui::button_was_clicked(args[0]->toString()));
        }
        return std::make_shared<Data::Value>(sad::ui::button_was_clicked(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x87\xd9\x84_\xd8\xb6\xd8\xba\xd8\xb7_\xd8\xa7\xd9\x84\xd8\xb2\xd8\xb1", button_was_clicked_func); // هل_ضغط_الزر
    interpreter.getFunctionManager().registerBuiltinFunction("button_was_clicked", button_was_clicked_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) حقول الإدخال / (EN) TextField
    // ═══════════════════════════════════════════════════════════════════

    // --- اقرأ_حقل / textfield_get_text ---
    auto textfield_get_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(std::string(""));
        if (args[0]->getType() == Data::ValueType::STRING) {
            return std::make_shared<Data::Value>(sad::ui::textfield_get_text(args[0]->toString()));
        }
        return std::make_shared<Data::Value>(sad::ui::textfield_get_text(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd8\xad\xd9\x82\xd9\x84", textfield_get_text_func); // اقرأ_حقل
    interpreter.getFunctionManager().registerBuiltinFunction("textfield_get_text", textfield_get_text_func);

    // --- عين_نص_حقل / textfield_set_text ---
    auto textfield_set_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        std::string text = args[1]->toString();
        sad::ui::textfield_set_text(id, text);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x86\xd8\xb5_\xd8\xad\xd9\x82\xd9\x84", textfield_set_text_func); // عين_نص_حقل
    interpreter.getFunctionManager().registerBuiltinFunction("textfield_set_text", textfield_set_text_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) خصائص موسعة / (EN) Extended Widget Properties
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_محدد / widget_set_checked ---
    auto widget_set_checked_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_checked(args[0]->toInt(), args[1]->toBool());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf", widget_set_checked_func); // عين_محدد
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_checked", widget_set_checked_func);

    // --- اقرأ_محدد / widget_get_checked ---
    auto widget_get_checked_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(false);
        if (args[0]->getType() == Data::ValueType::STRING) {
            return std::make_shared<Data::Value>(sad::ui::widget_get_checked(args[0]->toString()));
        }
        return std::make_shared<Data::Value>(sad::ui::widget_get_checked(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf", widget_get_checked_func); // اقرأ_محدد
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_checked", widget_get_checked_func);

    // --- عين_قيمة / widget_set_value ---
    auto widget_set_value_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_value(args[0]->toInt(), static_cast<float>(args[1]->toDouble()));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9", widget_set_value_func); // عين_قيمة
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_value", widget_set_value_func);

    // --- اقرأ_قيمة / widget_get_value ---
    auto widget_get_value_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0.0);
        if (args[0]->getType() == Data::ValueType::STRING) {
            return std::make_shared<Data::Value>(static_cast<double>(sad::ui::widget_get_value(args[0]->toString())));
        }
        return std::make_shared<Data::Value>(static_cast<double>(sad::ui::widget_get_value(args[0]->toInt())));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9", widget_get_value_func); // اقرأ_قيمة
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_value", widget_get_value_func);

    // --- عين_مختار / widget_set_selected ---
    auto widget_set_selected_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_selected(args[0]->toInt(), args[1]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x85\xd8\xae\xd8\xaa\xd8\xa7\xd8\xb1", widget_set_selected_func); // عين_مختار
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_selected", widget_set_selected_func);

    // --- اقرأ_مختار / widget_get_selected ---
    auto widget_get_selected_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0);
        return std::make_shared<Data::Value>(sad::ui::widget_get_selected(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd9\x85\xd8\xae\xd8\xaa\xd8\xa7\xd8\xb1", widget_get_selected_func); // اقرأ_مختار
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_selected", widget_get_selected_func);

    // --- عين_موسع / widget_set_expanded ---
    auto widget_set_expanded_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_expanded(args[0]->toInt(), args[1]->toBool());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x85\xd9\x88\xd8\xb3\xd8\xb9", widget_set_expanded_func); // عين_موسع
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_expanded", widget_set_expanded_func);

    // --- اقرأ_موسع / widget_get_expanded ---
    auto widget_get_expanded_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(false);
        return std::make_shared<Data::Value>(sad::ui::widget_get_expanded(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd9\x85\xd9\x88\xd8\xb3\xd8\xb9", widget_get_expanded_func); // اقرأ_موسع
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_expanded", widget_get_expanded_func);

    // --- أضف_عنصر / widget_add_item ---
    auto widget_add_item_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_add_item(args[0]->toInt(), args[1]->toString());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd8\xb6\xd9\x81_\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1", widget_add_item_func); // أضف_عنصر
    interpreter.getFunctionManager().registerBuiltinFunction("widget_add_item", widget_add_item_func);

    // --- أضف_بيانات / widget_add_data ---
    auto widget_add_data_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_add_data(args[0]->toInt(), static_cast<float>(args[1]->toDouble()));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd8\xb6\xd9\x81_\xd8\xa8\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa", widget_add_data_func); // أضف_بيانات
    interpreter.getFunctionManager().registerBuiltinFunction("widget_add_data", widget_add_data_func);

    // --- عين_أيقونة / widget_set_icon ---
    auto widget_set_icon_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        char icon = args[1]->toString().empty() ? '*' : args[1]->toString()[0];
        sad::ui::widget_set_icon(args[0]->toInt(), icon);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xa3\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9", widget_set_icon_func); // عين_أيقونة
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_icon", widget_set_icon_func);

    // --- عين_نشط / widget_set_active ---
    auto widget_set_active_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_active(args[0]->toInt(), args[1]->toBool());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x86\xd8\xb4\xd8\xb7", widget_set_active_func); // عين_نشط
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_active", widget_set_active_func);

    // --- عين_عنوان_فرعي / widget_set_subtitle ---
    auto widget_set_subtitle_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_subtitle(args[0]->toInt(), args[1]->toString());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86_\xd9\x81\xd8\xb1\xd8\xb9\xd9\x8a", widget_set_subtitle_func); // عين_عنوان_فرعي
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_subtitle", widget_set_subtitle_func);

    // --- عين_مستوى / widget_set_level ---
    auto widget_set_level_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_level(args[0]->toInt(), args[1]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x88\xd9\x89", widget_set_level_func); // عين_مستوى
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_level", widget_set_level_func);

    // --- عين_أعمدة / widget_set_columns ---
    auto widget_set_columns_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_columns(args[0]->toInt(), args[1]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xa3\xd8\xb9\xd9\x85\xd8\xaf\xd8\xa9", widget_set_columns_func); // عين_أعمدة
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_columns", widget_set_columns_func);

    // --- عين_صفحة / widget_set_page ---
    auto widget_set_page_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_set_page(args[0]->toInt(), args[1]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xb5\xd9\x81\xd8\xad\xd8\xa9", widget_set_page_func); // عين_صفحة
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_page", widget_set_page_func);

    // --- عين_لون_مميز / widget_set_accent_color ---
    auto widget_set_accent_color_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 4) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        int r = args[1]->toInt(), g = args[2]->toInt(), b = args[3]->toInt();
        sad::ui::widget_set_accent_color(id, r, g, b);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x84\xd9\x88\xd9\x86_\xd9\x85\xd9\x85\xd9\x8a\xd8\xb2", widget_set_accent_color_func); // عين_لون_مميز
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_accent_color", widget_set_accent_color_func);

    // --- أظهر_حوار / widget_show_dialog ---
    auto widget_show_dialog_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_show_dialog(args[0]->toInt(), args[1]->toBool());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd8\xb8\xd9\x87\xd8\xb1_\xd8\xad\xd9\x88\xd8\xa7\xd8\xb1", widget_show_dialog_func); // أظهر_حوار
    interpreter.getFunctionManager().registerBuiltinFunction("widget_show_dialog", widget_show_dialog_func);

    // --- هل_الحوار_ظاهر / widget_is_dialog_visible ---
    auto widget_is_dialog_visible_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(false);
        return std::make_shared<Data::Value>(sad::ui::widget_is_dialog_visible(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x87\xd9\x84_\xd8\xa7\xd9\x84\xd8\xad\xd9\x88\xd8\xa7\xd8\xb1_\xd8\xb8\xd8\xa7\xd9\x87\xd8\xb1", widget_is_dialog_visible_func); // هل_الحوار_ظاهر
    interpreter.getFunctionManager().registerBuiltinFunction("widget_is_dialog_visible", widget_is_dialog_visible_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) تنظيف / (EN) Cleanup
    // ═══════════════════════════════════════════════════════════════════

    // --- نظف_الكل / widget_cleanup_all ---
    auto widget_cleanup_all_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        sad::ui::widgetCleanupAll();
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd8\xb8\xd9\x81_\xd8\xa7\xd9\x84\xd9\x83\xd9\x84", widget_cleanup_all_func); // نظف_الكل
    interpreter.getFunctionManager().registerBuiltinFunction("widget_cleanup_all", widget_cleanup_all_func);

    // --- نظف_شجرة / widget_cleanup_tree ---
    auto widget_cleanup_tree_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>();
        sad::ui::widgetCleanupTree(args[0]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd8\xb8\xd9\x81_\xd8\xb4\xd8\xac\xd8\xb1\xd8\xa9", widget_cleanup_tree_func); // نظف_شجرة
    interpreter.getFunctionManager().registerBuiltinFunction("widget_cleanup_tree", widget_cleanup_tree_func);

    // --- عين_خاصية_نص / widget_set_prop_str ---
    auto widget_set_prop_str_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        std::string propName = args[1]->toString();
        std::string strValue = args[2]->toString();
        sad::ui::widgetSetProp(id, propName, strValue);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xae\xd8\xa7\xd8\xb5\xd9\x8a\xd8\xa9_\xd9\x86\xd8\xb5", widget_set_prop_str_func); // عين_خاصية_نص
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_prop_str", widget_set_prop_str_func);

    // --- عين_خاصية_رقم / widget_set_prop_num ---
    auto widget_set_prop_num_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        std::string propName = args[1]->toString();
        double numValue = args[2]->toDouble();
        sad::ui::widgetSetProp(id, propName, numValue);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xae\xd8\xa7\xd8\xb5\xd9\x8a\xd8\xa9_\xd8\xb1\xd9\x82\xd9\x85", widget_set_prop_num_func); // عين_خاصية_رقم
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_prop_num", widget_set_prop_num_func);

    // --- عين_خاصية_منطقي / widget_set_prop_bool ---
    auto widget_set_prop_bool_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) return std::make_shared<Data::Value>();
        int id = args[0]->toInt();
        std::string propName = args[1]->toString();
        bool boolValue = args[2]->toBool();
        sad::ui::widgetSetProp(id, propName, boolValue);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xae\xd8\xa7\xd8\xb5\xd9\x8a\xd8\xa9_\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a", widget_set_prop_bool_func); // عين_خاصية_منطقي
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_prop_bool", widget_set_prop_bool_func);

#endif // HAS_GRAPHICS
} // registerBuiltinsPart12

} // namespace Interpreter
} // namespace Sad
