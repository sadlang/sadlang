/**
 * @file builtin_registry_part15.cpp
 * @brief (AR) تسجيل دوال v2.1 الجديدة: استعلامات، تعديل الشجرة، تمرير، محاذاة، تدرج، ظل
 * @brief (EN) Register v2.1 APIs: queries, tree manipulation, scroll, alignment, gradient, shadow
 *
 * 60+ دالة جديدة مسجلة لدعم تحسينات نظام الرسومات الشاملة
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

void registerBuiltinsPart15(Interpreter& interpreter) {

    // ═══════════════════════════════════════════════════════════════════
    // (AR) استعلامات العناصر / (EN) Widget Queries
    // ═══════════════════════════════════════════════════════════════════

    // --- عدد_عناصر / widget_count ---
    auto widget_count_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::ui::widget_count());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd8\xaf\xd8\xaf_\xd8\xb9\xd9\x86\xd8\xa7\xd8\xb5\xd8\xb1", widget_count_func); // عدد_عناصر
    interpreter.getFunctionManager().registerBuiltinFunction("widget_count", widget_count_func);

    // --- اب_عنصر / widget_get_parent ---
    auto widget_get_parent_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(-1);
        return std::make_shared<Data::Value>(sad::ui::widget_get_parent(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xa8_\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1", widget_get_parent_func); // اب_عنصر
    interpreter.getFunctionManager().registerBuiltinFunction("widget_get_parent", widget_get_parent_func);

    // --- ابحث_بالاسم / widget_find_by_name_fast ---
    auto find_by_name_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(-1);
        return std::make_shared<Data::Value>(sad::ui::widget_find_by_name_fast(args[0]->toString()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xa8\xd8\xad\xd8\xab_\xd8\xa8\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85", find_by_name_func); // ابحث_بالاسم
    interpreter.getFunctionManager().registerBuiltinFunction("widget_find_by_name", find_by_name_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) تعديل الشجرة / (EN) Tree Manipulation
    // ═══════════════════════════════════════════════════════════════════

    // --- اضف_ابن_متتبع / widget_add_child_tracked ---
    auto add_child_tracked_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_add_child_tracked(args[0]->toInt(), args[1]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xb6\xd9\x81_\xd8\xa7\xd8\xa8\xd9\x86_\xd9\x85\xd8\xaa\xd8\xaa\xd8\xa8\xd8\xb9", add_child_tracked_func); // اضف_ابن_متتبع
    interpreter.getFunctionManager().registerBuiltinFunction("widget_add_child_tracked", add_child_tracked_func);

    // --- انسخ_عنصر / widget_clone ---
    auto widget_clone_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(-1);
        return std::make_shared<Data::Value>(sad::ui::widget_clone(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x86\xd8\xb3\xd8\xae_\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1", widget_clone_func); // انسخ_عنصر
    interpreter.getFunctionManager().registerBuiltinFunction("widget_clone", widget_clone_func);

    // --- انسخ_عنصر_عميق / widget_deep_clone ---
    auto widget_deep_clone_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(-1);
        return std::make_shared<Data::Value>(sad::ui::widget_deep_clone(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x86\xd8\xb3\xd8\xae_\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xb9\xd9\x85\xd9\x8a\xd9\x82", widget_deep_clone_func); // انسخ_عنصر_عميق
    interpreter.getFunctionManager().registerBuiltinFunction("widget_deep_clone", widget_deep_clone_func);

    // --- احذف_عنصر / widget_remove ---
    auto widget_remove_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>();
        sad::ui::widget_remove(args[0]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xad\xd8\xb0\xd9\x81_\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1", widget_remove_func); // احذف_عنصر
    interpreter.getFunctionManager().registerBuiltinFunction("widget_remove", widget_remove_func);

    // --- انقل_عنصر / widget_reparent ---
    auto widget_reparent_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_reparent(args[0]->toInt(), args[1]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x86\xd9\x82\xd9\x84_\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1", widget_reparent_func); // انقل_عنصر
    interpreter.getFunctionManager().registerBuiltinFunction("widget_reparent", widget_reparent_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) التطبيق / (EN) Application
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_عنوان_تطبيق / app_set_title ---
    auto app_set_title_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::app_set_title(args[0]->toInt(), args[1]->toString());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86_\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82", app_set_title_func); // عين_عنوان_تطبيق
    interpreter.getFunctionManager().registerBuiltinFunction("app_set_title", app_set_title_func);

    // --- عنوان_تطبيق / app_get_title ---
    auto app_get_title_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(sad::ui::app_get_title(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86_\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82", app_get_title_func); // عنوان_تطبيق
    interpreter.getFunctionManager().registerBuiltinFunction("app_get_title", app_get_title_func);

    // --- معدل_اطارات / app_get_fps ---
    auto app_get_fps_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0.0);
        return std::make_shared<Data::Value>(sad::ui::app_get_fps(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd8\xb9\xd8\xaf\xd9\x84_\xd8\xa7\xd8\xb7\xd8\xa7\xd8\xb1\xd8\xa7\xd8\xaa", app_get_fps_func); // معدل_اطارات
    interpreter.getFunctionManager().registerBuiltinFunction("app_get_fps", app_get_fps_func);

    // --- زمن_اطار / app_get_delta_time ---
    auto app_get_dt_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0.0);
        return std::make_shared<Data::Value>(sad::ui::app_get_delta_time(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb2\xd9\x85\xd9\x86_\xd8\xa7\xd8\xb7\xd8\xa7\xd8\xb1", app_get_dt_func); // زمن_اطار
    interpreter.getFunctionManager().registerBuiltinFunction("app_get_delta_time", app_get_dt_func);

    // --- عرض_تطبيق / app_get_width ---
    auto app_get_w_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0);
        return std::make_shared<Data::Value>(sad::ui::app_get_width(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd8\xb1\xd8\xb6_\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82", app_get_w_func); // عرض_تطبيق
    interpreter.getFunctionManager().registerBuiltinFunction("app_get_width", app_get_w_func);

    // --- ارتفاع_تطبيق / app_get_height ---
    auto app_get_h_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0);
        return std::make_shared<Data::Value>(sad::ui::app_get_height(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9_\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82", app_get_h_func); // ارتفاع_تطبيق
    interpreter.getFunctionManager().registerBuiltinFunction("app_get_height", app_get_h_func);

    // --- عين_حجم_تطبيق / app_set_size ---
    auto app_set_size_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) return std::make_shared<Data::Value>();
        sad::ui::app_set_size(args[0]->toInt(), args[1]->toInt(), args[2]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xad\xd8\xac\xd9\x85_\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82", app_set_size_func); // عين_حجم_تطبيق
    interpreter.getFunctionManager().registerBuiltinFunction("app_set_size", app_set_size_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) التمرير / (EN) Scrolling
    // ═══════════════════════════════════════════════════════════════════

    // --- مرر / widget_scroll_by ---
    auto scroll_by_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_scroll_by(args[0]->toInt(), (float)args[1]->toDouble());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd8\xb1\xd8\xb1", scroll_by_func); // مرر
    interpreter.getFunctionManager().registerBuiltinFunction("widget_scroll_by", scroll_by_func);

    // --- مرر_الى / widget_scroll_to ---
    auto scroll_to_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::widget_scroll_to(args[0]->toInt(), (float)args[1]->toDouble());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd8\xb1\xd8\xb1_\xd8\xa7\xd9\x84\xd9\x89", scroll_to_func); // مرر_الى
    interpreter.getFunctionManager().registerBuiltinFunction("widget_scroll_to", scroll_to_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) التموضع والمحاذاة / (EN) Positioning & Alignment
    // ═══════════════════════════════════════════════════════════════════

    // --- حرك_عنصر / widget_move_to ---
    auto move_to_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) return std::make_shared<Data::Value>();
        sad::ui::widget_move_to(args[0]->toInt(), (float)args[1]->toDouble(), (float)args[2]->toDouble());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xad\xd8\xb1\xd9\x83_\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1", move_to_func); // حرك_عنصر
    interpreter.getFunctionManager().registerBuiltinFunction("widget_move_to", move_to_func);

    // --- عين_محاذاة / widget_set_alignment ---
    auto set_align_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) return std::make_shared<Data::Value>();
        sad::ui::widget_set_alignment(args[0]->toInt(), args[1]->toString(), args[2]->toString());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x85\xd8\xad\xd8\xa7\xd8\xb0\xd8\xa7\xd8\xa9", set_align_func); // عين_محاذاة
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_alignment", set_align_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) الهامش والحشوة / (EN) Margin & Padding
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_هامش / widget_set_margin ---
    auto set_margin_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        if (args.size() >= 5) {
            sad::ui::widget_set_margin4(args[0]->toInt(),
                (float)args[1]->toDouble(), (float)args[2]->toDouble(),
                (float)args[3]->toDouble(), (float)args[4]->toDouble());
        } else {
            sad::ui::widget_set_margin(args[0]->toInt(), (float)args[1]->toDouble());
        }
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x87\xd8\xa7\xd9\x85\xd8\xb4", set_margin_func); // عين_هامش
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_margin", set_margin_func);

    // --- عين_حشوة4 / widget_set_padding4 ---
    auto set_padding4_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 5) return std::make_shared<Data::Value>();
        sad::ui::widget_set_padding4(args[0]->toInt(),
            (float)args[1]->toDouble(), (float)args[2]->toDouble(),
            (float)args[3]->toDouble(), (float)args[4]->toDouble());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xad\xd8\xb4\xd9\x88\xd8\xa9" "4", set_padding4_func); // عين_حشوة4
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_padding4", set_padding4_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) الظل والتدرج / (EN) Shadow & Gradient
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_ظل_كامل / widget_set_shadow ---
    auto set_shadow_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 8) return std::make_shared<Data::Value>();
        sad::ui::widget_set_shadow(args[0]->toInt(),
            (float)args[1]->toDouble(), (float)args[2]->toDouble(), (float)args[3]->toDouble(),
            args[4]->toInt(), args[5]->toInt(), args[6]->toInt(), args[7]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xb8\xd9\x84_\xd9\x83\xd8\xa7\xd9\x85\xd9\x84", set_shadow_func); // عين_ظل_كامل
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_shadow", set_shadow_func);

    // --- عين_تدرج / widget_set_gradient ---
    auto set_gradient_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 7) return std::make_shared<Data::Value>();
        sad::ui::widget_set_gradient(args[0]->toInt(),
            args[1]->toInt(), args[2]->toInt(), args[3]->toInt(),
            args[4]->toInt(), args[5]->toInt(), args[6]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac", set_gradient_func); // عين_تدرج
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_gradient", set_gradient_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) خصائص v2.1 المتقدمة / (EN) Advanced v2.1 Properties
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_خاصية_نص / widgetSetPropString_v2 ---
    auto set_prop_str_v2_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) return std::make_shared<Data::Value>();
        sad::ui::widgetSetPropString_v2(args[0]->toInt(), args[1]->toString(), args[2]->toString());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xae\xd8\xa7\xd8\xb5\xd9\x8a\xd8\xa9_\xd9\x86\xd8\xb5", set_prop_str_v2_func); // عين_خاصية_نص
    interpreter.getFunctionManager().registerBuiltinFunction("widget_set_prop_str", set_prop_str_v2_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) دوال مساعدة Flutter-like / (EN) Flutter-like Helpers
    // ═══════════════════════════════════════════════════════════════════

    // --- عنصر_بخصائص / widget_styled — ينشئ عنصر ويعين خصائصه دفعة واحدة
    // الاستخدام: عنصر_بخصائص(المعرف، "خلفية"، "أزرق"، "حشوة"، 16، "زوايا"، 8)
    auto widget_styled_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(-1);
        int id = args[0]->toInt();
        auto* w = sad::ui::getWidget(id);
        if (!w) return std::make_shared<Data::Value>(-1);
        // المعالجة بأزواج: (اسم_خاصية، قيمة)
        for (size_t i = 1; i + 1 < args.size(); i += 2) {
            std::string prop = args[i]->toString();
            auto& val = args[i+1];
            // محاولة كنص
            if (val->isString()) {
                sad::ui::widgetSetProp(id, prop, val->toString());
            }
            // محاولة كمنطقي
            else if (val->isBoolean()) {
                sad::ui::widgetSetProp(id, prop, val->toBool());
            }
            // محاولة كرقم
            else {
                sad::ui::widgetSetProp(id, prop, val->toDouble());
            }
        }
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xa8\xd8\xae\xd8\xb5\xd8\xa7\xd8\xa6\xd8\xb5", widget_styled_func); // عنصر_بخصائص
    interpreter.getFunctionManager().registerBuiltinFunction("widget_styled", widget_styled_func);

    // --- سطر / row_builder — اختصار Flutter لبناء صف
    // الاستخدام: سطر(ابن1، ابن2، ابن3)
    auto row_builder_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_row();
        for (auto& arg : args) {
            int cid = arg->toInt();
            if (cid >= 0) sad::ui::widget_add_child(id, cid);
        }
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb3\xd8\xb7\xd8\xb1", row_builder_func); // سطر
    interpreter.getFunctionManager().registerBuiltinFunction("row_of", row_builder_func);

    // --- عامود / col_builder — اختصار Flutter لبناء عمود
    auto col_builder_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_column();
        for (auto& arg : args) {
            int cid = arg->toInt();
            if (cid >= 0) sad::ui::widget_add_child(id, cid);
        }
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd8\xa7\xd9\x85\xd9\x88\xd8\xaf", col_builder_func); // عامود
    interpreter.getFunctionManager().registerBuiltinFunction("col_of", col_builder_func);

    // --- مكدس / stack_builder — اختصار Flutter لبناء مكدس
    auto stack_builder_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_stack();
        for (auto& arg : args) {
            int cid = arg->toInt();
            if (cid >= 0) sad::ui::widget_add_child(id, cid);
        }
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd9\x83\xd8\xaf\xd8\xb3", stack_builder_func); // مكدس
    interpreter.getFunctionManager().registerBuiltinFunction("stack_of", stack_builder_func);

    // --- وسط / center_builder — اختصار Flutter للتوسيط
    auto center_builder_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_center();
        for (auto& arg : args) {
            int cid = arg->toInt();
            if (cid >= 0) sad::ui::widget_add_child(id, cid);
        }
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x88\xd8\xb3\xd8\xb7", center_builder_func); // وسط
    interpreter.getFunctionManager().registerBuiltinFunction("center_of", center_builder_func);

    // --- بطاقة / card_builder — اختصار Flutter لبناء بطاقة
    auto card_builder_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_card();
        for (auto& arg : args) {
            int cid = arg->toInt();
            if (cid >= 0) sad::ui::widget_add_child(id, cid);
        }
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9_\xd9\x85\xd9\x86", card_builder_func); // بطاقة_من
    interpreter.getFunctionManager().registerBuiltinFunction("card_of", card_builder_func);

    // --- حشوة / padding_builder — يلف عنصر بحاوية ذات حشوة
    // الاستخدام: حشوة(16، ابن)
    auto padding_builder_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>(-1);
        float p = (float)args[0]->toDouble();
        int id = sad::ui::widget_container();
        auto* w = sad::ui::getWidget(id);
        if (w) {
            w->padding = p;
            w->padTop = p; w->padRight = p; w->padBottom = p; w->padLeft = p;
        }
        for (size_t i = 1; i < args.size(); ++i) {
            int cid = args[i]->toInt();
            if (cid >= 0) sad::ui::widget_add_child(id, cid);
        }
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xad\xd8\xb4\xd9\x88\xd8\xa9", padding_builder_func); // حشوة
    interpreter.getFunctionManager().registerBuiltinFunction("padding_of", padding_builder_func);

    // --- هامش / margin_builder — يلف عنصر بحاوية ذات هامش
    auto margin_builder_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>(-1);
        float m = (float)args[0]->toDouble();
        int id = sad::ui::widget_container();
        auto* w = sad::ui::getWidget(id);
        if (w) { w->marginTop = m; w->marginRight = m; w->marginBottom = m; w->marginLeft = m; }
        for (size_t i = 1; i < args.size(); ++i) {
            int cid = args[i]->toInt();
            if (cid >= 0) sad::ui::widget_add_child(id, cid);
        }
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x87\xd8\xa7\xd9\x85\xd8\xb4", margin_builder_func); // هامش
    interpreter.getFunctionManager().registerBuiltinFunction("margin_of", margin_builder_func);

    // --- ملون / colored_builder — حاوية بلون خلفية + أبناء
    // الاستخدام: ملون("أزرق"، ابن1، ابن2)
    auto colored_builder_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(-1);
        int id = sad::ui::widget_container();
        sad::ui::widgetSetProp(id, "خلفية", args[0]->toString());
        for (size_t i = 1; i < args.size(); ++i) {
            int cid = args[i]->toInt();
            if (cid >= 0) sad::ui::widget_add_child(id, cid);
        }
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd9\x84\xd9\x88\xd9\x86", colored_builder_func); // ملون
    interpreter.getFunctionManager().registerBuiltinFunction("colored", colored_builder_func);

} // registerBuiltinsPart15

} // namespace Interpreter
} // namespace Sad
