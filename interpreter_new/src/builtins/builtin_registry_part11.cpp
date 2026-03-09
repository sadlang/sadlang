/**
 * @file builtin_registry_part11.cpp
 * @brief (AR) تسجيل دوال إنشاء مكونات واجهة المستخدم (109 ودجت) — إطار sad::ui
 * @brief (EN) Register UI widget factory functions (109 widgets) — sad::ui framework
 */

#include "builtins.h"
#include "interpreter_core.h"
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

// ───── sad::ui framework headers ─────
#include "graphics/sad_ui_framework.h"
#include "ui/sad_ui_callbacks.h"

namespace Sad {
namespace Interpreter {

#ifdef HAS_GRAPHICS

// (AR) مساعد: أضف الأبناء تلقائياً للأسلوب التصريحي مثل فلتر + دعم الخصائص السطرية v3.0
// (EN) Helper: auto-add child widgets + inline property support (Flutter-like declarative syntax)
// يدعم: عمود("تباعد"، 10، "خلفية"، "ابيض"، child1، child2)
// ويدعم: نص_واجهة("مرحبا"، "لون"، "احمر"، "حجم_خط"، 20)
static void _autoChildren(int parentId, const std::vector<std::shared_ptr<Data::Value>>& args, size_t startIdx = 0) {
    for (size_t i = startIdx; i < args.size(); ++i) {
        // [v3.0] دعم الخصائص السطرية: إذا كان الوسيط سلسلة نصية واسم خاصية معروف
        if (args[i]->getType() == Data::ValueType::STRING) {
            std::string name = args[i]->toString();
            if (sad::ui::isKnownPropertyName(name) && i + 1 < args.size()) {
                // [v3.1] أحداث: إذا كان الاسم حدث معروف → سجل الإرجاع
                if (sad::callbacks::isEventProperty(name)) {
                    auto& val = args[i + 1];
                    // القيمة هي اسم الدالة (نص)
                    if (val->getType() == Data::ValueType::STRING) {
                        sad::callbacks::registerCallback(parentId, name, val->toString());
                    }
                    i++; // تخطي القيمة
                    continue;
                }
                auto& val = args[i + 1];
                auto vtype = val->getType();
                if (vtype == Data::ValueType::STRING) {
                    sad::ui::widgetSetProp(parentId, name, val->toString());
                } else if (vtype == Data::ValueType::BOOLEAN) {
                    sad::ui::widgetSetProp(parentId, name, val->toBool());
                } else {
                    sad::ui::widgetSetProp(parentId, name, val->toDouble());
                }
                i++; // تخطي القيمة
                continue;
            }
        }
        // معاملة الأرقام كمعرّفات أبناء
        if (args[i]->isNumeric()) {
            int cid = args[i]->toInt();
            if (cid >= 0) sad::ui::widget_add_child(parentId, cid);
        }
    }
}
#endif // HAS_GRAPHICS (helper)

void registerBuiltinsPart11(Interpreter& interpreter) {
#ifdef HAS_GRAPHICS

    // ═══════════════════════════════════════════════════════════════════
    // (AR) المكونات الأساسية (9) / (EN) Original 9 Widgets
    // ═══════════════════════════════════════════════════════════════════

    // --- عمود / column ---
    auto ui_column_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_column();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x85\xd9\x88\xd8\xaf", ui_column_func); // عمود
    interpreter.getFunctionManager().registerBuiltinFunction("ui_column", ui_column_func);

    // --- صف / row ---
    auto ui_row_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_row();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb5\xd9\x81", ui_row_func); // صف
    interpreter.getFunctionManager().registerBuiltinFunction("ui_row", ui_row_func);

    // --- نص / text ---
    auto ui_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string content = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_text(content);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd8\xb5_\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9", ui_text_func); // نص_واجهة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_text", ui_text_func);

    // --- زر / button ---
    auto ui_button_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string text = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_button(text);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb2\xd8\xb1", ui_button_func); // زر
    interpreter.getFunctionManager().registerBuiltinFunction("ui_button", ui_button_func);

    // --- حقل_نص / textfield ---
    auto ui_textfield_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string hint = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_textfield(hint);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xad\xd9\x82\xd9\x84_\xd9\x86\xd8\xb5", ui_textfield_func); // حقل_نص
    interpreter.getFunctionManager().registerBuiltinFunction("ui_textfield", ui_textfield_func);

    // --- حاوية / container ---
    auto ui_container_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_container();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xad\xd8\xa7\xd9\x88\xd9\x8a\xd8\xa9", ui_container_func); // حاوية
    interpreter.getFunctionManager().registerBuiltinFunction("ui_container", ui_container_func);

    // --- فراغ / spacer ---
    auto ui_spacer_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        float size = args.empty() ? 8.0f : static_cast<float>(args[0]->toDouble());
        int id = sad::ui::widget_spacer(size);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x81\xd8\xb1\xd8\xa7\xd8\xba", ui_spacer_func); // فراغ
    interpreter.getFunctionManager().registerBuiltinFunction("ui_spacer", ui_spacer_func);

    // --- بطاقة / card ---
    auto ui_card_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_card();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9", ui_card_func); // بطاقة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_card", ui_card_func);

    // --- فاصل / divider ---
    auto ui_divider_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_divider();
        _autoChildren(id, args, 0);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x81\xd8\xa7\xd8\xb5\xd9\x84", ui_divider_func); // فاصل
    interpreter.getFunctionManager().registerBuiltinFunction("ui_divider", ui_divider_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) التخطيط (10) / (EN) Layout Widgets (10)
    // ═══════════════════════════════════════════════════════════════════

    // --- مكدس / stack ---
    auto ui_stack_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_stack();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd9\x83\xd8\xaf\xd8\xb3", ui_stack_func); // مكدس
    interpreter.getFunctionManager().registerBuiltinFunction("ui_stack", ui_stack_func);

    // --- وسط / center ---
    auto ui_center_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_center();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x88\xd8\xb3\xd8\xb7", ui_center_func); // وسط
    interpreter.getFunctionManager().registerBuiltinFunction("ui_center", ui_center_func);

    // --- التفاف / wrap ---
    auto ui_wrap_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_wrap();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x84\xd8\xaa\xd9\x81\xd8\xa7\xd9\x81", ui_wrap_func); // التفاف
    interpreter.getFunctionManager().registerBuiltinFunction("ui_wrap", ui_wrap_func);

    // --- شبكة / gridview ---
    auto ui_gridview_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int cols = args.empty() ? 2 : args[0]->toInt();
        int id = sad::ui::widget_gridview(cols);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9", ui_gridview_func); // شبكة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_gridview", ui_gridview_func);

    // --- قائمة / listview ---
    auto ui_listview_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_listview();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9", ui_listview_func); // قائمة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_listview", ui_listview_func);

    // --- تمرير / scrollview ---
    auto ui_scrollview_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_scrollview();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1", ui_scrollview_func); // تمرير
    interpreter.getFunctionManager().registerBuiltinFunction("ui_scrollview", ui_scrollview_func);

    // --- صندوق_بحجم / sizedbox ---
    auto ui_sizedbox_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        float w = args.size() > 0 ? static_cast<float>(args[0]->toDouble()) : 0;
        float h = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 0;
        return std::make_shared<Data::Value>(sad::ui::widget_sizedbox(w, h));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb5\xd9\x86\xd8\xaf\xd9\x88\xd9\x82_\xd8\xa8\xd8\xad\xd8\xac\xd9\x85", ui_sizedbox_func); // صندوق_بحجم
    interpreter.getFunctionManager().registerBuiltinFunction("ui_sizedbox", ui_sizedbox_func);

    // --- توسيع / expanded ---
    auto ui_expanded_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_expanded();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xaa\xd9\x88\xd8\xb3\xd9\x8a\xd8\xb9", ui_expanded_func); // توسيع
    interpreter.getFunctionManager().registerBuiltinFunction("ui_expanded", ui_expanded_func);

    // --- صفحات / pageview ---
    auto ui_pageview_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_pageview();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb5\xd9\x81\xd8\xad\xd8\xa7\xd8\xaa", ui_pageview_func); // صفحات
    interpreter.getFunctionManager().registerBuiltinFunction("ui_pageview", ui_pageview_func);

    // --- جدول / table ---
    auto ui_table_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int cols = args.empty() ? 2 : args[0]->toInt();
        int id = sad::ui::widget_table(cols);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xac\xd8\xaf\xd9\x88\xd9\x84", ui_table_func); // جدول
    interpreter.getFunctionManager().registerBuiltinFunction("ui_table", ui_table_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) الأزرار (6) / (EN) Buttons (6)
    // ═══════════════════════════════════════════════════════════════════

    // --- زر_أيقونة / icon_button ---
    auto ui_icon_button_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        char icon = args.empty() ? '*' : args[0]->toString()[0];
        int id = sad::ui::widget_icon_button(icon);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb2\xd8\xb1_\xd8\xa3\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9", ui_icon_button_func); // زر_أيقونة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_icon_button", ui_icon_button_func);

    // --- زر_عائم / floating_button ---
    auto ui_floating_button_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        char icon = args.empty() ? '+' : args[0]->toString()[0];
        int id = sad::ui::widget_floating_button(icon);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb2\xd8\xb1_\xd8\xb9\xd8\xa7\xd8\xa6\xd9\x85", ui_floating_button_func); // زر_عائم
    interpreter.getFunctionManager().registerBuiltinFunction("ui_floating_button", ui_floating_button_func);

    // --- زر_محدد / outline_button ---
    auto ui_outline_button_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string text = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_outline_button(text);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb2\xd8\xb1_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf", ui_outline_button_func); // زر_محدد
    interpreter.getFunctionManager().registerBuiltinFunction("ui_outline_button", ui_outline_button_func);

    // --- زر_نص / text_button ---
    auto ui_text_button_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string text = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_text_button(text);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb2\xd8\xb1_\xd9\x86\xd8\xb5", ui_text_button_func); // زر_نص
    interpreter.getFunctionManager().registerBuiltinFunction("ui_text_button", ui_text_button_func);

    // --- زر_تبديل / toggle_button ---
    auto ui_toggle_button_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string text = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_toggle_button(text);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb2\xd8\xb1_\xd8\xaa\xd8\xa8\xd8\xaf\xd9\x8a\xd9\x84", ui_toggle_button_func); // زر_تبديل
    interpreter.getFunctionManager().registerBuiltinFunction("ui_toggle_button", ui_toggle_button_func);

    // --- مجموعة_أزرار / button_group ---
    auto ui_button_group_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_button_group();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\xd8\xa9_\xd8\xa3\xd8\xb2\xd8\xb1\xd8\xa7\xd8\xb1", ui_button_group_func); // مجموعة_أزرار
    interpreter.getFunctionManager().registerBuiltinFunction("ui_button_group", ui_button_group_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) عناصر الإدخال (14) / (EN) Input Controls (14)
    // ═══════════════════════════════════════════════════════════════════

    // --- مربع_اختيار / checkbox ---
    auto ui_checkbox_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string label = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_checkbox(label);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd8\xb1\xd8\xa8\xd8\xb9_\xd8\xa7\xd8\xae\xd8\xaa\xd9\x8a\xd8\xa7\xd8\xb1", ui_checkbox_func); // مربع_اختيار
    interpreter.getFunctionManager().registerBuiltinFunction("ui_checkbox", ui_checkbox_func);

    // --- راديو / radio ---
    auto ui_radio_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string label = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_radio(label);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xa7\xd8\xaf\xd9\x8a\xd9\x88", ui_radio_func); // راديو
    interpreter.getFunctionManager().registerBuiltinFunction("ui_radio", ui_radio_func);

    // --- مفتاح / switch ---
    auto ui_switch_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string label = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_switch(label);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad", ui_switch_func); // مفتاح
    interpreter.getFunctionManager().registerBuiltinFunction("ui_switch", ui_switch_func);

    // --- منزلق / slider ---
    auto ui_slider_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        float minv = args.size() > 0 ? static_cast<float>(args[0]->toDouble()) : 0;
        float maxv = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 100;
        int id = sad::ui::widget_slider(minv, maxv);
        _autoChildren(id, args, 2);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd9\x86\xd8\xb2\xd9\x84\xd9\x82", ui_slider_func); // منزلق
    interpreter.getFunctionManager().registerBuiltinFunction("ui_slider", ui_slider_func);

    // --- قائمة_منسدلة / dropdown ---
    auto ui_dropdown_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string hint = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_dropdown(hint);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd9\x85\xd9\x86\xd8\xb3\xd8\xaf\xd9\x84\xd8\xa9", ui_dropdown_func); // قائمة_منسدلة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_dropdown", ui_dropdown_func);

    // --- إدخال_رقم / number_input ---
    auto ui_number_input_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        float minv = args.size() > 0 ? static_cast<float>(args[0]->toDouble()) : 0;
        float maxv = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 100;
        int id = sad::ui::widget_number_input(minv, maxv);
        _autoChildren(id, args, 2);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa5\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84_\xd8\xb1\xd9\x82\xd9\x85", ui_number_input_func); // إدخال_رقم
    interpreter.getFunctionManager().registerBuiltinFunction("ui_number_input", ui_number_input_func);

    // --- حقل_بحث / search_field ---
    auto ui_search_field_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string hint = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_search_field(hint);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xad\xd9\x82\xd9\x84_\xd8\xa8\xd8\xad\xd8\xab", ui_search_field_func); // حقل_بحث
    interpreter.getFunctionManager().registerBuiltinFunction("ui_search_field", ui_search_field_func);

    // --- حقل_كلمة_سر / password_field ---
    auto ui_password_field_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string hint = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_password_field(hint);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xad\xd9\x82\xd9\x84_\xd9\x83\xd9\x84\xd9\x85\xd8\xa9_\xd8\xb3\xd8\xb1", ui_password_field_func); // حقل_كلمة_سر
    interpreter.getFunctionManager().registerBuiltinFunction("ui_password_field", ui_password_field_func);

    // --- منطقة_نص / textarea ---
    auto ui_textarea_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string hint = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_textarea(hint);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd8\xa9_\xd9\x86\xd8\xb5", ui_textarea_func); // منطقة_نص
    interpreter.getFunctionManager().registerBuiltinFunction("ui_textarea", ui_textarea_func);

    // --- حقل_تاريخ / date_field ---
    auto ui_date_field_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_date_field();
        _autoChildren(id, args, 0);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xad\xd9\x82\xd9\x84_\xd8\xaa\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xae", ui_date_field_func); // حقل_تاريخ
    interpreter.getFunctionManager().registerBuiltinFunction("ui_date_field", ui_date_field_func);

    // --- نطاق_منزلق / range_slider ---
    auto ui_range_slider_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        float minv = args.size() > 0 ? static_cast<float>(args[0]->toDouble()) : 0;
        float maxv = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 100;
        int id = sad::ui::widget_range_slider(minv, maxv);
        _autoChildren(id, args, 2);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd8\xb7\xd8\xa7\xd9\x82_\xd9\x85\xd9\x86\xd8\xb2\xd9\x84\xd9\x82", ui_range_slider_func); // نطاق_منزلق
    interpreter.getFunctionManager().registerBuiltinFunction("ui_range_slider", ui_range_slider_func);

    // --- مجموعة_تبديل / toggle_group ---
    auto ui_toggle_group_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_toggle_group();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\xd8\xa9_\xd8\xaa\xd8\xa8\xd8\xaf\xd9\x8a\xd9\x84", ui_toggle_group_func); // مجموعة_تبديل
    interpreter.getFunctionManager().registerBuiltinFunction("ui_toggle_group", ui_toggle_group_func);

    // --- منتقي_لون / color_picker ---
    auto ui_color_picker_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_color_picker();
        _autoChildren(id, args, 0);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd9\x86\xd8\xaa\xd9\x82\xd9\x8a_\xd9\x84\xd9\x88\xd9\x86", ui_color_picker_func); // منتقي_لون
    interpreter.getFunctionManager().registerBuiltinFunction("ui_color_picker", ui_color_picker_func);

    // --- تقييم_نجوم / rating_stars ---
    auto ui_rating_stars_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int maxStars = args.empty() ? 5 : args[0]->toInt();
        int id = sad::ui::widget_rating_stars(maxStars);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xaa\xd9\x82\xd9\x8a\xd9\x8a\xd9\x85_\xd9\x86\xd8\xac\xd9\x88\xd9\x85", ui_rating_stars_func); // تقييم_نجوم
    interpreter.getFunctionManager().registerBuiltinFunction("ui_rating_stars", ui_rating_stars_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) عرض نصوص (7) / (EN) Display Text (7)
    // ═══════════════════════════════════════════════════════════════════

    // --- تسمية / label ---
    auto ui_label_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string text = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_label(text);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x85\xd9\x8a\xd8\xa9", ui_label_func); // تسمية
    interpreter.getFunctionManager().registerBuiltinFunction("ui_label", ui_label_func);

    // --- عنوان / heading ---
    auto ui_heading_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string text = args.empty() ? "" : args[0]->toString();
        int level = args.size() > 1 ? args[1]->toInt() : 1;
        int id = sad::ui::widget_heading(text, level);
        _autoChildren(id, args, 2);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86", ui_heading_func); // عنوان
    interpreter.getFunctionManager().registerBuiltinFunction("ui_heading", ui_heading_func);

    // --- فقرة / paragraph ---
    auto ui_paragraph_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string text = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_paragraph(text);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x81\xd9\x82\xd8\xb1\xd8\xa9", ui_paragraph_func); // فقرة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_paragraph", ui_paragraph_func);

    // --- رابط / link ---
    auto ui_link_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string text = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_link(text);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xa7\xd8\xa8\xd8\xb7", ui_link_func); // رابط
    interpreter.getFunctionManager().registerBuiltinFunction("ui_link", ui_link_func);

    // --- كود / code_block ---
    auto ui_code_block_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string code = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_code_block(code);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x83\xd9\x88\xd8\xaf", ui_code_block_func); // كود
    interpreter.getFunctionManager().registerBuiltinFunction("ui_code_block", ui_code_block_func);

    // --- اقتباس / quote ---
    auto ui_quote_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string text = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_quote(text);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb3", ui_quote_func); // اقتباس
    interpreter.getFunctionManager().registerBuiltinFunction("ui_quote", ui_quote_func);

    // --- ماركداون / markdown ---
    auto ui_markdown_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string text = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_markdown(text);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd8\xa7\xd8\xb1\xd9\x83\xd8\xaf\xd8\xa7\xd9\x88\xd9\x86", ui_markdown_func); // ماركداون
    interpreter.getFunctionManager().registerBuiltinFunction("ui_markdown", ui_markdown_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) عرض مرئي (10) / (EN) Display Visual (10)
    // ═══════════════════════════════════════════════════════════════════

    // --- أيقونة / icon ---
    auto ui_icon_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        char icon = args.empty() ? '*' : args[0]->toString()[0];
        int id = sad::ui::widget_icon(icon);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9", ui_icon_func); // أيقونة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_icon", ui_icon_func);

    // --- صورة_رمزية / avatar ---
    auto ui_avatar_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string letter = args.empty() ? "?" : args[0]->toString();
        int r = args.size() > 1 ? args[1]->toInt() : 66;
        int g = args.size() > 2 ? args[2]->toInt() : 133;
        int b = args.size() > 3 ? args[3]->toInt() : 244;
        int id = sad::ui::widget_avatar(letter, r, g, b);
        _autoChildren(id, args, 4);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xb1\xd8\xa9_\xd8\xb1\xd9\x85\xd8\xb2\xd9\x8a\xd8\xa9", ui_avatar_func); // صورة_رمزية
    interpreter.getFunctionManager().registerBuiltinFunction("ui_avatar", ui_avatar_func);

    // --- شارة / badge ---
    auto ui_badge_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int count = args.empty() ? 0 : args[0]->toInt();
        int id = sad::ui::widget_badge(count);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb4\xd8\xa7\xd8\xb1\xd8\xa9", ui_badge_func); // شارة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_badge", ui_badge_func);

    // --- رقاقة / chip ---
    auto ui_chip_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string label = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_chip(label);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd9\x82\xd8\xa7\xd9\x82\xd8\xa9", ui_chip_func); // رقاقة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_chip", ui_chip_func);

    // --- وسم / tag ---
    auto ui_tag_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string label = args.empty() ? "" : args[0]->toString();
        int r = args.size() > 1 ? args[1]->toInt() : 66;
        int g = args.size() > 2 ? args[2]->toInt() : 133;
        int b = args.size() > 3 ? args[3]->toInt() : 244;
        int id = sad::ui::widget_tag(label, r, g, b);
        _autoChildren(id, args, 4);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x88\xd8\xb3\xd9\x85", ui_tag_func); // وسم
    interpreter.getFunctionManager().registerBuiltinFunction("ui_tag", ui_tag_func);

    // --- نقطة_حالة / status_dot ---
    auto ui_status_dot_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int r = args.size() > 0 ? args[0]->toInt() : 76;
        int g = args.size() > 1 ? args[1]->toInt() : 175;
        int b = args.size() > 2 ? args[2]->toInt() : 80;
        int id = sad::ui::widget_status_dot(r, g, b);
        _autoChildren(id, args, 3);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd9\x82\xd8\xb7\xd8\xa9_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9", ui_status_dot_func); // نقطة_حالة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_status_dot", ui_status_dot_func);

    // --- صندوق_لون / color_box ---
    auto ui_color_box_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int r = args.size() > 0 ? args[0]->toInt() : 200;
        int g = args.size() > 1 ? args[1]->toInt() : 200;
        int b = args.size() > 2 ? args[2]->toInt() : 200;
        int id = sad::ui::widget_color_box(r, g, b);
        _autoChildren(id, args, 3);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb5\xd9\x86\xd8\xaf\xd9\x88\xd9\x82_\xd9\x84\xd9\x88\xd9\x86", ui_color_box_func); // صندوق_لون
    interpreter.getFunctionManager().registerBuiltinFunction("ui_color_box", ui_color_box_func);

    // --- صندوق_تدرج / gradient_box ---
    auto ui_gradient_box_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int r1 = args.size() > 0 ? args[0]->toInt() : 66;
        int g1 = args.size() > 1 ? args[1]->toInt() : 133;
        int b1 = args.size() > 2 ? args[2]->toInt() : 244;
        int r2 = args.size() > 3 ? args[3]->toInt() : 255;
        int g2 = args.size() > 4 ? args[4]->toInt() : 152;
        int b2 = args.size() > 5 ? args[5]->toInt() : 0;
        int id = sad::ui::widget_gradient_box(r1, g1, b1, r2, g2, b2);
        _autoChildren(id, args, 6);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb5\xd9\x86\xd8\xaf\xd9\x88\xd9\x82_\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac", ui_gradient_box_func); // صندوق_تدرج
    interpreter.getFunctionManager().registerBuiltinFunction("ui_gradient_box", ui_gradient_box_func);

    // --- صورة / image ---
    auto ui_image_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string placeholder = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_image(placeholder);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xb1\xd8\xa9", ui_image_func); // صورة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_image", ui_image_func);

    // --- فاصل_مرئي / separator ---
    auto ui_separator_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_separator();
        _autoChildren(id, args, 0);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x81\xd8\xa7\xd8\xb5\xd9\x84_\xd9\x85\xd8\xb1\xd8\xa6\xd9\x8a", ui_separator_func); // فاصل_مرئي
    interpreter.getFunctionManager().registerBuiltinFunction("ui_separator", ui_separator_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) التقدم (4) / (EN) Progress (4)
    // ═══════════════════════════════════════════════════════════════════

    // --- شريط_تقدم / progress_bar ---
    auto ui_progress_bar_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        float value = args.empty() ? 0 : static_cast<float>(args[0]->toDouble());
        int id = sad::ui::widget_progress_bar(value);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85", ui_progress_bar_func); // شريط_تقدم
    interpreter.getFunctionManager().registerBuiltinFunction("ui_progress_bar", ui_progress_bar_func);

    // --- تقدم_دائري / circle_progress ---
    auto ui_circle_progress_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        float value = args.empty() ? 0 : static_cast<float>(args[0]->toDouble());
        int id = sad::ui::widget_circle_progress(value);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85_\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd9\x8a", ui_circle_progress_func); // تقدم_دائري
    interpreter.getFunctionManager().registerBuiltinFunction("ui_circle_progress", ui_circle_progress_func);

    // --- تحميل / loading_spinner ---
    auto ui_loading_spinner_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_loading_spinner();
        _autoChildren(id, args, 0);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xaa\xd8\xad\xd9\x85\xd9\x8a\xd9\x84", ui_loading_spinner_func); // تحميل
    interpreter.getFunctionManager().registerBuiltinFunction("ui_loading_spinner", ui_loading_spinner_func);

    // --- هيكل / skeleton ---
    auto ui_skeleton_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_skeleton();
        _autoChildren(id, args, 0);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x87\xd9\x8a\xd9\x83\xd9\x84", ui_skeleton_func); // هيكل
    interpreter.getFunctionManager().registerBuiltinFunction("ui_skeleton", ui_skeleton_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) التنقل (10) / (EN) Navigation Widgets (10)
    // ═══════════════════════════════════════════════════════════════════

    // --- شريط_تطبيق / app_bar ---
    auto ui_app_bar_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_app_bar(title);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82", ui_app_bar_func); // شريط_تطبيق
    interpreter.getFunctionManager().registerBuiltinFunction("ui_app_bar", ui_app_bar_func);

    // --- شريط_سفلي / bottom_bar ---
    auto ui_bottom_bar_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_bottom_bar();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xb3\xd9\x81\xd9\x84\xd9\x8a", ui_bottom_bar_func); // شريط_سفلي
    interpreter.getFunctionManager().registerBuiltinFunction("ui_bottom_bar", ui_bottom_bar_func);

    // --- شريط_تبويب / tab_bar ---
    auto ui_tab_bar_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_tab_bar();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd8\xa8\xd9\x88\xd9\x8a\xd8\xa8", ui_tab_bar_func); // شريط_تبويب
    interpreter.getFunctionManager().registerBuiltinFunction("ui_tab_bar", ui_tab_bar_func);

    // --- درج / drawer ---
    auto ui_drawer_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_drawer();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xaf\xd8\xb1\xd8\xac", ui_drawer_func); // درج
    interpreter.getFunctionManager().registerBuiltinFunction("ui_drawer", ui_drawer_func);

    // --- مسار_تنقل / breadcrumb ---
    auto ui_breadcrumb_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_breadcrumb();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd8\xb3\xd8\xa7\xd8\xb1_\xd8\xaa\xd9\x86\xd9\x82\xd9\x84", ui_breadcrumb_func); // مسار_تنقل
    interpreter.getFunctionManager().registerBuiltinFunction("ui_breadcrumb", ui_breadcrumb_func);

    // --- عنصر_تنقل / nav_item ---
    auto ui_nav_item_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string label = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_nav_item(label);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xaa\xd9\x86\xd9\x82\xd9\x84", ui_nav_item_func); // عنصر_تنقل
    interpreter.getFunctionManager().registerBuiltinFunction("ui_nav_item", ui_nav_item_func);

    // --- عنصر_قائمة / menu_item ---
    auto ui_menu_item_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string label = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_menu_item(label);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9", ui_menu_item_func); // عنصر_قائمة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_menu_item", ui_menu_item_func);

    // --- شريط_أدوات / toolbar ---
    auto ui_toolbar_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_toolbar();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xa3\xd8\xaf\xd9\x88\xd8\xa7\xd8\xaa", ui_toolbar_func); // شريط_أدوات
    interpreter.getFunctionManager().registerBuiltinFunction("ui_toolbar", ui_toolbar_func);

    // --- شريط_حالة / status_bar ---
    auto ui_status_bar_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string text = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_status_bar(text);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9", ui_status_bar_func); // شريط_حالة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_status_bar", ui_status_bar_func);

    // --- قائمة_جانبية / side_menu ---
    auto ui_side_menu_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_side_menu();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd8\xac\xd8\xa7\xd9\x86\xd8\xa8\xd9\x8a\xd8\xa9", ui_side_menu_func); // قائمة_جانبية
    interpreter.getFunctionManager().registerBuiltinFunction("ui_side_menu", ui_side_menu_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) التغذية الراجعة (10) / (EN) Feedback/Dialogs (10)
    // ═══════════════════════════════════════════════════════════════════

    // --- حوار / dialog ---
    auto ui_dialog_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = args.size() > 0 ? args[0]->toString() : "";
        std::string msg = args.size() > 1 ? args[1]->toString() : "";
        int id = sad::ui::widget_dialog(title, msg);
        _autoChildren(id, args, 2);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xad\xd9\x88\xd8\xa7\xd8\xb1", ui_dialog_func); // حوار
    interpreter.getFunctionManager().registerBuiltinFunction("ui_dialog", ui_dialog_func);

    // --- تنبيه / alert ---
    auto ui_alert_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string msg = args.size() > 0 ? args[0]->toString() : "";
        std::string type = args.size() > 1 ? args[1]->toString() : "info";
        int id = sad::ui::widget_alert(msg, type);
        _autoChildren(id, args, 2);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xaa\xd9\x86\xd8\xa8\xd9\x8a\xd9\x87", ui_alert_func); // تنبيه
    interpreter.getFunctionManager().registerBuiltinFunction("ui_alert", ui_alert_func);

    // --- إشعار_مؤقت / toast ---
    auto ui_toast_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string msg = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_toast(msg);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa5\xd8\xb4\xd8\xb9\xd8\xa7\xd8\xb1_\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa", ui_toast_func); // إشعار_مؤقت
    interpreter.getFunctionManager().registerBuiltinFunction("ui_toast", ui_toast_func);

    // --- شريط_رسالة / snackbar ---
    auto ui_snackbar_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string msg = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_snackbar(msg);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84\xd8\xa9", ui_snackbar_func); // شريط_رسالة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_snackbar", ui_snackbar_func);

    // --- لافتة / banner ---
    auto ui_banner_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string msg = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_banner(msg);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x84\xd8\xa7\xd9\x81\xd8\xaa\xd8\xa9", ui_banner_func); // لافتة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_banner", ui_banner_func);

    // --- نافذة_منبثقة / modal ---
    auto ui_modal_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_modal();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9_\xd9\x85\xd9\x86\xd8\xa8\xd8\xab\xd9\x82\xd8\xa9", ui_modal_func); // نافذة_منبثقة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_modal", ui_modal_func);

    // --- فقاعة / popover ---
    auto ui_popover_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_popover();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x81\xd9\x82\xd8\xa7\xd8\xb9\xd8\xa9", ui_popover_func); // فقاعة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_popover", ui_popover_func);

    // --- إشعار / notification ---
    auto ui_notification_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = args.size() > 0 ? args[0]->toString() : "";
        std::string msg = args.size() > 1 ? args[1]->toString() : "";
        int id = sad::ui::widget_notification(title, msg);
        _autoChildren(id, args, 2);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa5\xd8\xb4\xd8\xb9\xd8\xa7\xd8\xb1", ui_notification_func); // إشعار
    interpreter.getFunctionManager().registerBuiltinFunction("ui_notification", ui_notification_func);

    // --- حوار_تأكيد / confirm_dialog ---
    auto ui_confirm_dialog_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = args.size() > 0 ? args[0]->toString() : "";
        std::string msg = args.size() > 1 ? args[1]->toString() : "";
        int id = sad::ui::widget_confirm_dialog(title, msg);
        _autoChildren(id, args, 2);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xad\xd9\x88\xd8\xa7\xd8\xb1_\xd8\xaa\xd8\xa3\xd9\x83\xd9\x8a\xd8\xaf", ui_confirm_dialog_func); // حوار_تأكيد
    interpreter.getFunctionManager().registerBuiltinFunction("ui_confirm_dialog", ui_confirm_dialog_func);

    // --- تلميح / tooltip ---
    auto ui_tooltip_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string text = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_tooltip(text);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad", ui_tooltip_func); // تلميح
    interpreter.getFunctionManager().registerBuiltinFunction("ui_tooltip", ui_tooltip_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) بطاقات ولوحات (7) / (EN) Cards & Panels (7)
    // ═══════════════════════════════════════════════════════════════════

    // --- بطاقة_معلومات / info_card ---
    auto ui_info_card_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = args.size() > 0 ? args[0]->toString() : "";
        std::string body = args.size() > 1 ? args[1]->toString() : "";
        int id = sad::ui::widget_info_card(title, body);
        _autoChildren(id, args, 2);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9_\xd9\x85\xd8\xb9\xd9\x84\xd9\x88\xd9\x85\xd8\xa7\xd8\xaa", ui_info_card_func); // بطاقة_معلومات
    interpreter.getFunctionManager().registerBuiltinFunction("ui_info_card", ui_info_card_func);

    // --- بطاقة_إحصاء / stat_card ---
    auto ui_stat_card_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string label = args.size() > 0 ? args[0]->toString() : "";
        std::string value = args.size() > 1 ? args[1]->toString() : "";
        int id = sad::ui::widget_stat_card(label, value);
        _autoChildren(id, args, 2);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9_\xd8\xa5\xd8\xad\xd8\xb5\xd8\xa7\xd8\xa1", ui_stat_card_func); // بطاقة_إحصاء
    interpreter.getFunctionManager().registerBuiltinFunction("ui_stat_card", ui_stat_card_func);

    // --- لوحة_توسيع / expansion_panel ---
    auto ui_expansion_panel_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_expansion_panel(title);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x84\xd9\x88\xd8\xad\xd8\xa9_\xd8\xaa\xd9\x88\xd8\xb3\xd9\x8a\xd8\xb9", ui_expansion_panel_func); // لوحة_توسيع
    interpreter.getFunctionManager().registerBuiltinFunction("ui_expansion_panel", ui_expansion_panel_func);

    // --- أكورديون / accordion ---
    auto ui_accordion_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_accordion();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd9\x83\xd9\x88\xd8\xb1\xd8\xaf\xd9\x8a\xd9\x88\xd9\x86", ui_accordion_func); // أكورديون
    interpreter.getFunctionManager().registerBuiltinFunction("ui_accordion", ui_accordion_func);

    // --- لوحة / panel ---
    auto ui_panel_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_panel(title);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x84\xd9\x88\xd8\xad\xd8\xa9", ui_panel_func); // لوحة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_panel", ui_panel_func);

    // --- صندوق_مجموعة / group_box ---
    auto ui_group_box_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_group_box(title);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb5\xd9\x86\xd8\xaf\xd9\x88\xd9\x82_\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\xd8\xa9", ui_group_box_func); // صندوق_مجموعة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_group_box", ui_group_box_func);

    // --- وسيلة_إيضاح / callout ---
    auto ui_callout_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string text = args.size() > 0 ? args[0]->toString() : "";
        std::string type = args.size() > 1 ? args[1]->toString() : "info";
        int id = sad::ui::widget_callout(text, type);
        _autoChildren(id, args, 2);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x88\xd8\xb3\xd9\x8a\xd9\x84\xd8\xa9_\xd8\xa5\xd9\x8a\xd8\xb6\xd8\xa7\xd8\xad", ui_callout_func); // وسيلة_إيضاح
    interpreter.getFunctionManager().registerBuiltinFunction("ui_callout", ui_callout_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) البيانات (10) / (EN) Data Widgets (10)
    // ═══════════════════════════════════════════════════════════════════

    // --- عنصر_قائمة_مفصل / list_tile ---
    auto ui_list_tile_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = args.size() > 0 ? args[0]->toString() : "";
        std::string subtitle = args.size() > 1 ? args[1]->toString() : "";
        int id = sad::ui::widget_list_tile(title, subtitle);
        _autoChildren(id, args, 2);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd9\x85\xd9\x81\xd8\xb5\xd9\x84", ui_list_tile_func); // عنصر_قائمة_مفصل
    interpreter.getFunctionManager().registerBuiltinFunction("ui_list_tile", ui_list_tile_func);

    // --- جدول_بيانات / data_table ---
    auto ui_data_table_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int cols = args.empty() ? 2 : args[0]->toInt();
        int id = sad::ui::widget_data_table(cols);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xac\xd8\xaf\xd9\x88\xd9\x84_\xd8\xa8\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa", ui_data_table_func); // جدول_بيانات
    interpreter.getFunctionManager().registerBuiltinFunction("ui_data_table", ui_data_table_func);

    // --- صف_جدول / table_row ---
    auto ui_table_row_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_table_row();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb5\xd9\x81_\xd8\xac\xd8\xaf\xd9\x88\xd9\x84", ui_table_row_func); // صف_جدول
    interpreter.getFunctionManager().registerBuiltinFunction("ui_table_row", ui_table_row_func);

    // --- خلية_جدول / table_cell ---
    auto ui_table_cell_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string text = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_table_cell(text);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xae\xd9\x84\xd9\x8a\xd8\xa9_\xd8\xac\xd8\xaf\xd9\x88\xd9\x84", ui_table_cell_func); // خلية_جدول
    interpreter.getFunctionManager().registerBuiltinFunction("ui_table_cell", ui_table_cell_func);

    // --- صف_تفاصيل / detail_row ---
    auto ui_detail_row_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string key = args.size() > 0 ? args[0]->toString() : "";
        std::string val = args.size() > 1 ? args[1]->toString() : "";
        int id = sad::ui::widget_detail_row(key, val);
        _autoChildren(id, args, 2);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb5\xd9\x81_\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb5\xd9\x8a\xd9\x84", ui_detail_row_func); // صف_تفاصيل
    interpreter.getFunctionManager().registerBuiltinFunction("ui_detail_row", ui_detail_row_func);

    // --- خط_زمني / timeline ---
    auto ui_timeline_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_timeline();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xae\xd8\xb7_\xd8\xb2\xd9\x85\xd9\x86\xd9\x8a", ui_timeline_func); // خط_زمني
    interpreter.getFunctionManager().registerBuiltinFunction("ui_timeline", ui_timeline_func);

    // --- عنصر_خط_زمني / timeline_item ---
    auto ui_timeline_item_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = args.empty() ? "" : args[0]->toString();
        int id = sad::ui::widget_timeline_item(title);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xae\xd8\xb7_\xd8\xb2\xd9\x85\xd9\x86\xd9\x8a", ui_timeline_item_func); // عنصر_خط_زمني
    interpreter.getFunctionManager().registerBuiltinFunction("ui_timeline_item", ui_timeline_item_func);

    // --- عنصر_شجرة / tree_item ---
    auto ui_tree_item_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string label = args.size() > 0 ? args[0]->toString() : "";
        int depth = args.size() > 1 ? args[1]->toInt() : 0;
        int id = sad::ui::widget_tree_item(label, depth);
        _autoChildren(id, args, 2);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xb4\xd8\xac\xd8\xb1\xd8\xa9", ui_tree_item_func); // عنصر_شجرة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_tree_item", ui_tree_item_func);

    // --- خطوات / stepper ---
    auto ui_stepper_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int steps = args.empty() ? 3 : args[0]->toInt();
        int id = sad::ui::widget_stepper(steps);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xae\xd8\xb7\xd9\x88\xd8\xa7\xd8\xaa", ui_stepper_func); // خطوات
    interpreter.getFunctionManager().registerBuiltinFunction("ui_stepper", ui_stepper_func);

    // --- مؤشر_خطوة / step_indicator ---
    auto ui_step_indicator_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int total = args.size() > 0 ? args[0]->toInt() : 3;
        int current = args.size() > 1 ? args[1]->toInt() : 0;
        int id = sad::ui::widget_step_indicator(total, current);
        _autoChildren(id, args, 2);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1_\xd8\xae\xd8\xb7\xd9\x88\xd8\xa9", ui_step_indicator_func); // مؤشر_خطوة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_step_indicator", ui_step_indicator_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) الرسوم البيانية (7) / (EN) Charts (7)
    // ═══════════════════════════════════════════════════════════════════

    // --- رسم_أعمدة / bar_chart ---
    auto ui_bar_chart_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_bar_chart();
        _autoChildren(id, args, 0);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd9\x85_\xd8\xa3\xd8\xb9\xd9\x85\xd8\xaf\xd8\xa9", ui_bar_chart_func); // رسم_أعمدة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_bar_chart", ui_bar_chart_func);

    // --- رسم_أعمدة_أفقي / hbar_chart ---
    auto ui_hbar_chart_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_hbar_chart();
        _autoChildren(id, args, 0);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd9\x85_\xd8\xa3\xd8\xb9\xd9\x85\xd8\xaf\xd8\xa9_\xd8\xa3\xd9\x81\xd9\x82\xd9\x8a", ui_hbar_chart_func); // رسم_أعمدة_أفقي
    interpreter.getFunctionManager().registerBuiltinFunction("ui_hbar_chart", ui_hbar_chart_func);

    // --- رسم_دائري / pie_chart ---
    auto ui_pie_chart_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_pie_chart();
        _autoChildren(id, args, 0);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd9\x85_\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd9\x8a", ui_pie_chart_func); // رسم_دائري
    interpreter.getFunctionManager().registerBuiltinFunction("ui_pie_chart", ui_pie_chart_func);

    // --- رسم_خطي / line_chart ---
    auto ui_line_chart_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_line_chart();
        _autoChildren(id, args, 0);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd9\x85_\xd8\xae\xd8\xb7\xd9\x8a", ui_line_chart_func); // رسم_خطي
    interpreter.getFunctionManager().registerBuiltinFunction("ui_line_chart", ui_line_chart_func);

    // --- خط_مؤشر / spark_line ---
    auto ui_spark_line_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_spark_line();
        _autoChildren(id, args, 0);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xae\xd8\xb7_\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1", ui_spark_line_func); // خط_مؤشر
    interpreter.getFunctionManager().registerBuiltinFunction("ui_spark_line", ui_spark_line_func);

    // --- مقياس / gauge ---
    auto ui_gauge_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        float value = args.size() > 0 ? static_cast<float>(args[0]->toDouble()) : 0;
        float maxv = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 100;
        int id = sad::ui::widget_gauge(value, maxv);
        _autoChildren(id, args, 2);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd9\x82\xd9\x8a\xd8\xa7\xd8\xb3", ui_gauge_func); // مقياس
    interpreter.getFunctionManager().registerBuiltinFunction("ui_gauge", ui_gauge_func);

    // --- رسم_حلقي / donut_chart ---
    auto ui_donut_chart_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_donut_chart();
        _autoChildren(id, args, 0);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd9\x85_\xd8\xad\xd9\x84\xd9\x82\xd9\x8a", ui_donut_chart_func); // رسم_حلقي
    interpreter.getFunctionManager().registerBuiltinFunction("ui_donut_chart", ui_donut_chart_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) ترقيم الصفحات (2) / (EN) Pagination (2)
    // ═══════════════════════════════════════════════════════════════════

    // --- ترقيم / pagination ---
    auto ui_pagination_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int totalPages = args.empty() ? 1 : args[0]->toInt();
        int id = sad::ui::widget_pagination(totalPages);
        _autoChildren(id, args, 1);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xaa\xd8\xb1\xd9\x82\xd9\x8a\xd9\x85", ui_pagination_func); // ترقيم
    interpreter.getFunctionManager().registerBuiltinFunction("ui_pagination", ui_pagination_func);

    // --- مؤشر_نقاط / dot_indicator ---
    auto ui_dot_indicator_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int total = args.size() > 0 ? args[0]->toInt() : 3;
        int current = args.size() > 1 ? args[1]->toInt() : 0;
        int id = sad::ui::widget_dot_indicator(total, current);
        _autoChildren(id, args, 2);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1_\xd9\x86\xd9\x82\xd8\xa7\xd8\xb7", ui_dot_indicator_func); // مؤشر_نقاط
    interpreter.getFunctionManager().registerBuiltinFunction("ui_dot_indicator", ui_dot_indicator_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) متقدم (3) / (EN) Advanced (3)
    // ═══════════════════════════════════════════════════════════════════

    // --- لوح_رسم / canvas ---
    auto ui_canvas_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        float w = args.size() > 0 ? static_cast<float>(args[0]->toDouble()) : 200;
        float h = args.size() > 1 ? static_cast<float>(args[1]->toDouble()) : 200;
        int id = sad::ui::widget_canvas(w, h);
        _autoChildren(id, args, 2);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x84\xd9\x88\xd8\xad_\xd8\xb1\xd8\xb3\xd9\x85", ui_canvas_func); // لوح_رسم
    interpreter.getFunctionManager().registerBuiltinFunction("ui_canvas", ui_canvas_func);

    // --- تقسيم / split_view ---
    auto ui_split_view_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_split_view();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xaa\xd9\x82\xd8\xb3\xd9\x8a\xd9\x85", ui_split_view_func); // تقسيم
    interpreter.getFunctionManager().registerBuiltinFunction("ui_split_view", ui_split_view_func);

    // --- عرض_دوار / carousel ---
    auto ui_carousel_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int id = sad::ui::widget_carousel();
        _autoChildren(id, args);
        return std::make_shared<Data::Value>(id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd8\xb1\xd8\xb6_\xd8\xaf\xd9\x88\xd8\xa7\xd8\xb1", ui_carousel_func); // عرض_دوار
    interpreter.getFunctionManager().registerBuiltinFunction("ui_carousel", ui_carousel_func);

#endif // HAS_GRAPHICS
} // registerBuiltinsPart11

} // namespace Interpreter
} // namespace Sad
