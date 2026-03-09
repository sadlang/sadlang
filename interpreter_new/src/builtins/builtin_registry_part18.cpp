// ═══════════════════════════════════════════════════════════════════════════
// builtin_registry_part18.cpp — 80 ودجت متقدمة جديدة Flutter-Style v4.0
// ═══════════════════════════════════════════════════════════════════════════
// Material Design 3 + Cupertino + Custom Advanced Widgets
// ═══════════════════════════════════════════════════════════════════════════

#include "builtins.h"
#include "interpreter_core.h"
#include "graphics/sad_ui_framework.h"
#include <string>
#include <memory>
#include <vector>
#include <cmath>

namespace Sad {
namespace Interpreter {

// ═══════════════════════════════════════════════════════════════════════════
// مساعد تطبيق الخصائص
// ═══════════════════════════════════════════════════════════════════════════
#ifdef HAS_GRAPHICS
static void _applyProps18(int id, const std::vector<std::shared_ptr<Data::Value>>& args, size_t startIdx) {
    for (size_t i = startIdx; i < args.size(); ++i) {
        if (args[i]->getType() == Data::ValueType::STRING) {
            std::string name = args[i]->toString();
            if (sad::ui::isKnownPropertyName(name) && i + 1 < args.size()) {
                auto& val = args[i + 1];
                if (val->isString()) {
                    sad::ui::widgetSetProp(id, name, val->toString());
                } else if (val->isNumeric()) {
                    sad::ui::widgetSetProp(id, name, val->toDouble());
                } else if (val->isBoolean()) {
                    sad::ui::widgetSetProp(id, name, val->toBool());
                }
                i++;
            }
        }
    }
}
#endif // HAS_GRAPHICS

void registerBuiltinsPart18(Interpreter& interpreter) {
#ifdef HAS_GRAPHICS
    using Args = const std::vector<std::shared_ptr<Data::Value>>&;

    // ═══════════════════════════════════════════════════════════════════════════
    // [1-10] Material Design 3 Components
    // ═══════════════════════════════════════════════════════════════════════════

    // 1. نص_سائل / fluid_text — نص يتكيف مع حجم الشاشة
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x86\xd8\xb5_\xd8\xb3\xd8\xa7\xd8\xa6\xd9\x84", // نص_سائل
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string txt = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            int id = sad::ui::widget_text(txt);
            sad::ui::widgetSetProp(id, "textWrap", true);
            sad::ui::widgetSetProp(id, "overflow", "ellipsis");
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 2. زر_ممتلئ / filled_button — Material 3 Filled Button
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb2\xd8\xb1_\xd9\x85\xd9\x85\xd8\xaa\xd9\x84\xd8\xa6", // زر_ممتلئ
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string txt = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "زر";
            int id = sad::ui::widget_button(txt);
            sad::ui::widgetSetProp(id, "borderRadius", 20.0);
            sad::ui::widgetSetProp(id, "elevation", 2.0);
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 3. زر_لوني / tonal_button — Material 3 Tonal Button
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb2\xd8\xb1_\xd9\x84\xd9\x88\xd9\x86\xd9\x8a", // زر_لوني
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string txt = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "زر";
            int id = sad::ui::widget_button(txt);
            sad::ui::widgetSetProp(id, "bg", "#E8DEF8");
            sad::ui::widgetSetProp(id, "color", "#1D192B");
            sad::ui::widgetSetProp(id, "borderRadius", 20.0);
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 4. بطاقة_مملوءة / filled_card — Material 3 Filled Card
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9_\xd9\x85\xd9\x85\xd9\x84\xd9\x88\xd8\xa1\xd8\xa9", // بطاقة_مملوءة
        [](Args args) -> std::shared_ptr<Data::Value> {
            int id = sad::ui::widget_card();
            sad::ui::widgetSetProp(id, "bg", "#E6E0E9");
            sad::ui::widgetSetProp(id, "borderRadius", 12.0);
            sad::ui::widgetSetProp(id, "elevation", 0.0);
            sad::ui::widgetSetProp(id, "padding", 16.0);
            _applyProps18(id, args, 0);
            return std::make_shared<Data::Value>((double)id);
        });

    // 5. بطاقة_مرفوعة / elevated_card — Material 3 Elevated Card
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9_\xd9\x85\xd8\xb1\xd9\x81\xd9\x88\xd8\xb9\xd8\xa9", // بطاقة_مرفوعة
        [](Args args) -> std::shared_ptr<Data::Value> {
            int id = sad::ui::widget_card();
            sad::ui::widgetSetProp(id, "bg", "#FFFBFE");
            sad::ui::widgetSetProp(id, "borderRadius", 12.0);
            sad::ui::widgetSetProp(id, "elevation", 4.0);
            sad::ui::widgetSetProp(id, "padding", 16.0);
            _applyProps18(id, args, 0);
            return std::make_shared<Data::Value>((double)id);
        });

    // 6. بطاقة_محددة / outlined_card — Material 3 Outlined Card
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf\xd8\xa9", // بطاقة_محددة
        [](Args args) -> std::shared_ptr<Data::Value> {
            int id = sad::ui::widget_card();
            sad::ui::widgetSetProp(id, "bg", "#FFFBFE");
            sad::ui::widgetSetProp(id, "borderRadius", 12.0);
            sad::ui::widgetSetProp(id, "borderWidth", 1.0);
            sad::ui::widgetSetProp(id, "borderColor", "#79747E");
            sad::ui::widgetSetProp(id, "padding", 16.0);
            _applyProps18(id, args, 0);
            return std::make_shared<Data::Value>((double)id);
        });

    // 7. شريحة_مرشحة / filter_chip — Material 3 Filter Chip
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xad\xd8\xa9_\xd9\x85\xd8\xb1\xd8\xb4\xd8\xad\xd8\xa9", // شريحة_مرشحة
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string txt = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "فلتر";
            int id = sad::ui::widget_chip(txt);
            sad::ui::widgetSetProp(id, "borderRadius", 8.0);
            sad::ui::widgetSetProp(id, "bg", "#E8DEF8");
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 8. شريط_بحث_موسع / search_bar — Material 3 Search Bar
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xa8\xd8\xad\xd8\xab_\xd9\x85\xd9\x88\xd8\xb3\xd8\xb9", // شريط_بحث_موسع
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string hint = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "بحث...";
            int id = sad::ui::widget_search_field(hint);
            sad::ui::widgetSetProp(id, "bg", "#E6E0E9");
            sad::ui::widgetSetProp(id, "borderRadius", 28.0);
            sad::ui::widgetSetProp(id, "padding", 16.0);
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 9. شريط_تنقل_سفلي / navigation_bar — Material 3 Navigation Bar
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd9\x86\xd9\x82\xd9\x84_\xd8\xb3\xd9\x81\xd9\x84\xd9\x8a", // شريط_تنقل_سفلي
        [](Args args) -> std::shared_ptr<Data::Value> {
            int id = sad::ui::widget_bottom_bar();
            sad::ui::widgetSetProp(id, "bg", "#E6E0E9");
            sad::ui::widgetSetProp(id, "height", 80.0);
            _applyProps18(id, args, 0);
            return std::make_shared<Data::Value>((double)id);
        });

    // 10. شريط_تطبيق_كبير / large_app_bar — Material 3 Large Top App Bar
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82_\xd9\x83\xd8\xa8\xd9\x8a\xd8\xb1", // شريط_تطبيق_كبير
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string title = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            int id = sad::ui::widget_app_bar(title);
            sad::ui::widgetSetProp(id, "height", 152.0);
            sad::ui::widgetSetProp(id, "fontSize", 32.0);
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // ═══════════════════════════════════════════════════════════════════════════
    // [11-20] Cupertino Style Components
    // ═══════════════════════════════════════════════════════════════════════════

    // 11. زر_ايفون / cupertino_button
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb2\xd8\xb1_\xd8\xa7\xd9\x8a\xd9\x81\xd9\x88\xd9\x86", // زر_ايفون
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string txt = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "زر";
            int id = sad::ui::widget_button(txt);
            sad::ui::widgetSetProp(id, "bg", "#007AFF");
            sad::ui::widgetSetProp(id, "borderRadius", 8.0);
            sad::ui::widgetSetProp(id, "fontSize", 17.0);
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 12. منزلق_ايفون / cupertino_slider
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd9\x86\xd8\xb2\xd9\x84\xd9\x82_\xd8\xa7\xd9\x8a\xd9\x81\xd9\x88\xd9\x86", // منزلق_ايفون
        [](Args args) -> std::shared_ptr<Data::Value> {
            double minV = args.size() > 0 && args[0]->isNumeric() ? args[0]->toDouble() : 0;
            double maxV = args.size() > 1 && args[1]->isNumeric() ? args[1]->toDouble() : 100;
            int id = sad::ui::widget_slider((float)minV, (float)maxV);
            sad::ui::widgetSetProp(id, "accent", "#007AFF");
            sad::ui::widgetSetProp(id, "height", 22.0);
            _applyProps18(id, args, 2);
            return std::make_shared<Data::Value>((double)id);
        });

    // 13. مفتاح_ايفون / cupertino_switch
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad_\xd8\xa7\xd9\x8a\xd9\x81\xd9\x88\xd9\x86", // مفتاح_ايفون
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string lbl = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            int id = sad::ui::widget_switch(lbl);
            sad::ui::widgetSetProp(id, "accent", "#34C759");
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 14. شريط_تبويب_ايفون / cupertino_tab_bar
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd8\xa8\xd9\x88\xd9\x8a\xd8\xa8_\xd8\xa7\xd9\x8a\xd9\x81\xd9\x88\xd9\x86", // شريط_تبويب_ايفون
        [](Args args) -> std::shared_ptr<Data::Value> {
            int id = sad::ui::widget_tab_bar();
            sad::ui::widgetSetProp(id, "bg", "#F2F2F7");
            sad::ui::widgetSetProp(id, "accent", "#007AFF");
            _applyProps18(id, args, 0);
            return std::make_shared<Data::Value>((double)id);
        });

    // 15. حقل_نص_ايفون / cupertino_textfield
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xad\xd9\x82\xd9\x84_\xd9\x86\xd8\xb5_\xd8\xa7\xd9\x8a\xd9\x81\xd9\x88\xd9\x86", // حقل_نص_ايفون
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string hint = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            int id = sad::ui::widget_textfield(hint);
            sad::ui::widgetSetProp(id, "bg", "#FFFFFF");
            sad::ui::widgetSetProp(id, "borderRadius", 10.0);
            sad::ui::widgetSetProp(id, "borderWidth", 0.5);
            sad::ui::widgetSetProp(id, "borderColor", "#C6C6C8");
            sad::ui::widgetSetProp(id, "padding", 12.0);
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 16. تنبيه_ايفون / cupertino_alert
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xaa\xd9\x86\xd8\xa8\xd9\x8a\xd9\x87_\xd8\xa7\xd9\x8a\xd9\x81\xd9\x88\xd9\x86", // تنبيه_ايفون
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string title = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "تنبيه";
            std::string msg = args.size() > 1 && args[1]->isString() ? args[1]->toString() : "";
            int id = sad::ui::widget_dialog(title, msg);
            sad::ui::widgetSetProp(id, "bg", "#FFFFFF");
            sad::ui::widgetSetProp(id, "borderRadius", 14.0);
            sad::ui::widgetSetProp(id, "blur", 20.0);
            _applyProps18(id, args, 2);
            return std::make_shared<Data::Value>((double)id);
        });

    // 17. ورقة_اجراءات / action_sheet
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x88\xd8\xb1\xd9\x82\xd8\xa9_\xd8\xa7\xd8\xac\xd8\xb1\xd8\xa7\xd8\xa1\xd8\xa7\xd8\xaa", // ورقة_اجراءات
        [](Args args) -> std::shared_ptr<Data::Value> {
            int id = sad::ui::widget_modal();
            sad::ui::widgetSetProp(id, "bg", "#FFFFFF");
            sad::ui::widgetSetProp(id, "borderRadius", 14.0);
            sad::ui::widgetSetProp(id, "padding", 16.0);
            _applyProps18(id, args, 0);
            return std::make_shared<Data::Value>((double)id);
        });

    // 18. مؤشر_نشاط / activity_indicator
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1_\xd9\x86\xd8\xb4\xd8\xa7\xd8\xb7", // مؤشر_نشاط
        [](Args args) -> std::shared_ptr<Data::Value> {
            int id = sad::ui::widget_loading_spinner();
            sad::ui::widgetSetProp(id, "accent", "#007AFF");
            _applyProps18(id, args, 0);
            return std::make_shared<Data::Value>((double)id);
        });

    // 19. منتقي_ايفون / cupertino_picker
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd9\x86\xd8\xaa\xd9\x82\xd9\x8a_\xd8\xa7\xd9\x8a\xd9\x81\xd9\x88\xd9\x86", // منتقي_ايفون
        [](Args args) -> std::shared_ptr<Data::Value> {
            int id = sad::ui::widget_dropdown("");
            sad::ui::widgetSetProp(id, "bg", "#F2F2F7");
            sad::ui::widgetSetProp(id, "borderRadius", 8.0);
            _applyProps18(id, args, 0);
            return std::make_shared<Data::Value>((double)id);
        });

    // 20. شريط_تنقل_ايفون / cupertino_nav_bar
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd9\x86\xd9\x82\xd9\x84_\xd8\xa7\xd9\x8a\xd9\x81\xd9\x88\xd9\x86", // شريط_تنقل_ايفون
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string title = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            int id = sad::ui::widget_app_bar(title);
            sad::ui::widgetSetProp(id, "bg", "#F2F2F7");
            sad::ui::widgetSetProp(id, "height", 44.0);
            sad::ui::widgetSetProp(id, "blur", 10.0);
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // ═══════════════════════════════════════════════════════════════════════════
    // [21-40] Data Display Widgets
    // ═══════════════════════════════════════════════════════════════════════════

    // 21. صف_بيانات / data_row
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb5\xd9\x81_\xd8\xa8\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa", // صف_بيانات
        [](Args args) -> std::shared_ptr<Data::Value> {
            int row = sad::ui::widget_row();
            sad::ui::widgetSetProp(row, "padding", 12.0);
            sad::ui::widgetSetProp(row, "mainAxisAlignment", "spaceBetween");
            sad::ui::widgetSetProp(row, "showDivider", true);
            _applyProps18(row, args, 0);
            return std::make_shared<Data::Value>((double)row);
        });

    // 22. خلية_رأس / header_cell
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xae\xd9\x84\xd9\x8a\xd8\xa9_\xd8\xb1\xd8\xa3\xd8\xb3", // خلية_رأس
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string txt = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            int id = sad::ui::widget_table_cell(txt);
            sad::ui::widgetSetProp(id, "fontWeight", "bold");
            sad::ui::widgetSetProp(id, "bg", "#F5F5F5");
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 23. عنصر_قائمة_غني / rich_list_tile
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd8\xba\xd9\x86\xd9\x8a", // عنصر_قائمة_غني
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string title = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            std::string sub = args.size() > 1 && args[1]->isString() ? args[1]->toString() : "";
            int id = sad::ui::widget_list_tile(title, sub);
            sad::ui::widgetSetProp(id, "padding", 16.0);
            sad::ui::widgetSetProp(id, "showDivider", true);
            _applyProps18(id, args, 2);
            return std::make_shared<Data::Value>((double)id);
        });

    // 24. بطاقة_احصائية / stat_card_v2
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9_\xd8\xa7\xd8\xad\xd8\xb5\xd8\xa7\xd8\xa6\xd9\x8a\xd8\xa9", // بطاقة_احصائية
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string label = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            std::string value = args.size() > 1 && args[1]->isString() ? args[1]->toString() : "0";
            int id = sad::ui::widget_stat_card(label, value);
            sad::ui::widgetSetProp(id, "elevation", 2.0);
            sad::ui::widgetSetProp(id, "borderRadius", 12.0);
            _applyProps18(id, args, 2);
            return std::make_shared<Data::Value>((double)id);
        });

    // 25. مخطط_شريطي_متحرك / animated_bar_chart
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xae\xd8\xb7\xd8\xb7_\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7\xd9\x8a_\xd9\x85\xd8\xaa\xd8\xad\xd8\xb1\xd9\x83", // مخطط_شريطي_متحرك
        [](Args args) -> std::shared_ptr<Data::Value> {
            int id = sad::ui::widget_bar_chart();
            sad::ui::widgetSetProp(id, "animating", true);
            sad::ui::widgetSetProp(id, "animationDuration", 500.0);
            _applyProps18(id, args, 0);
            return std::make_shared<Data::Value>((double)id);
        });

    // 26. مخطط_دائري_متحرك / animated_pie_chart
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xae\xd8\xb7\xd8\xb7_\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd9\x8a_\xd9\x85\xd8\xaa\xd8\xad\xd8\xb1\xd9\x83", // مخطط_دائري_متحرك
        [](Args args) -> std::shared_ptr<Data::Value> {
            int id = sad::ui::widget_pie_chart();
            sad::ui::widgetSetProp(id, "animating", true);
            sad::ui::widgetSetProp(id, "animationDuration", 800.0);
            _applyProps18(id, args, 0);
            return std::make_shared<Data::Value>((double)id);
        });

    // 27. عداد_رقمي / number_ticker
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf_\xd8\xb1\xd9\x82\xd9\x85\xd9\x8a", // عداد_رقمي
        [](Args args) -> std::shared_ptr<Data::Value> {
            double val = args.size() > 0 && args[0]->isNumeric() ? args[0]->toDouble() : 0;
            int id = sad::ui::widget_text(std::to_string((int)val));
            sad::ui::widgetSetProp(id, "fontSize", 48.0);
            sad::ui::widgetSetProp(id, "fontWeight", "bold");
            sad::ui::widgetSetProp(id, "animating", true);
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 28. شريط_تقدم_دائري / circular_progress
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85_\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd9\x8a", // شريط_تقدم_دائري
        [](Args args) -> std::shared_ptr<Data::Value> {
            double val = args.size() > 0 && args[0]->isNumeric() ? args[0]->toDouble() : 0;
            int id = sad::ui::widget_circle_progress((float)val);
            sad::ui::widgetSetProp(id, "width", 64.0);
            sad::ui::widgetSetProp(id, "height", 64.0);
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 29. مقياس_نصف_دائري / half_gauge
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd9\x82\xd9\x8a\xd8\xa7\xd8\xb3_\xd9\x86\xd8\xb5\xd9\x81_\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd9\x8a", // مقياس_نصف_دائري
        [](Args args) -> std::shared_ptr<Data::Value> {
            double val = args.size() > 0 && args[0]->isNumeric() ? args[0]->toDouble() : 0;
            double maxV = args.size() > 1 && args[1]->isNumeric() ? args[1]->toDouble() : 100;
            int id = sad::ui::widget_gauge((float)val, (float)maxV);
            sad::ui::widgetSetProp(id, "shape", "stadium");
            _applyProps18(id, args, 2);
            return std::make_shared<Data::Value>((double)id);
        });

    // 30. خط_زمني_عمودي / vertical_timeline
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xae\xd8\xb7_\xd8\xb2\xd9\x85\xd9\x86\xd9\x8a_\xd8\xb9\xd9\x85\xd9\x88\xd8\xaf\xd9\x8a", // خط_زمني_عمودي
        [](Args args) -> std::shared_ptr<Data::Value> {
            int id = sad::ui::widget_timeline();
            sad::ui::widgetSetProp(id, "spacing", 24.0);
            _applyProps18(id, args, 0);
            return std::make_shared<Data::Value>((double)id);
        });

    // 31. عنصر_زمني / timeline_event
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xb2\xd9\x85\xd9\x86\xd9\x8a", // عنصر_زمني
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string title = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            int id = sad::ui::widget_timeline_item(title);
            sad::ui::widgetSetProp(id, "showDivider", true);
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 32. مؤشر_خطوات / step_progress
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1_\xd8\xae\xd8\xb7\xd9\x88\xd8\xa7\xd8\xaa", // مؤشر_خطوات
        [](Args args) -> std::shared_ptr<Data::Value> {
            int total = args.size() > 0 && args[0]->isNumeric() ? (int)args[0]->toDouble() : 3;
            int current = args.size() > 1 && args[1]->isNumeric() ? (int)args[1]->toDouble() : 0;
            int id = sad::ui::widget_step_indicator(total, current);
            sad::ui::widgetSetProp(id, "accent", "#6200EE");
            _applyProps18(id, args, 2);
            return std::make_shared<Data::Value>((double)id);
        });

    // 33. شجرة_ملفات / file_tree
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb4\xd8\xac\xd8\xb1\xd8\xa9_\xd9\x85\xd9\x84\xd9\x81\xd8\xa7\xd8\xaa", // شجرة_ملفات
        [](Args args) -> std::shared_ptr<Data::Value> {
            int col = sad::ui::widget_column();
            sad::ui::widgetSetProp(col, "padding", 8.0);
            sad::ui::widgetSetProp(col, "spacing", 4.0);
            _applyProps18(col, args, 0);
            return std::make_shared<Data::Value>((double)col);
        });

    // 34. عنصر_شجرة_قابل_للتوسع / expandable_tree_item
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xb4\xd8\xac\xd8\xb1\xd8\xa9_\xd9\x82\xd8\xa7\xd8\xa8\xd9\x84", // عنصر_شجرة_قابل
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string label = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            int depth = args.size() > 1 && args[1]->isNumeric() ? (int)args[1]->toDouble() : 0;
            int id = sad::ui::widget_tree_item(label, depth);
            sad::ui::widgetSetProp(id, "focusable", true);
            _applyProps18(id, args, 2);
            return std::make_shared<Data::Value>((double)id);
        });

    // 35. قائمة_مجمعة / grouped_list
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd9\x85\xd8\xac\xd9\x85\xd8\xb9\xd8\xa9", // قائمة_مجمعة
        [](Args args) -> std::shared_ptr<Data::Value> {
            int id = sad::ui::widget_listview();
            sad::ui::widgetSetProp(id, "showDivider", true);
            _applyProps18(id, args, 0);
            return std::make_shared<Data::Value>((double)id);
        });

    // 36. رأس_مجموعة / group_header
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb1\xd8\xa3\xd8\xb3_\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\xd8\xa9", // رأس_مجموعة
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string title = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            int id = sad::ui::widget_text(title);
            sad::ui::widgetSetProp(id, "fontWeight", "bold");
            sad::ui::widgetSetProp(id, "fontSize", 14.0);
            sad::ui::widgetSetProp(id, "color", "#666666");
            sad::ui::widgetSetProp(id, "padding", 12.0);
            sad::ui::widgetSetProp(id, "bg", "#F5F5F5");
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 37. اكورديون_متعدد / multi_accordion
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa7\xd9\x83\xd9\x88\xd8\xb1\xd8\xaf\xd9\x8a\xd9\x88\xd9\x86_\xd9\x85\xd8\xaa\xd8\xb9\xd8\xaf\xd8\xaf", // اكورديون_متعدد
        [](Args args) -> std::shared_ptr<Data::Value> {
            int id = sad::ui::widget_accordion();
            sad::ui::widgetSetProp(id, "showDivider", true);
            _applyProps18(id, args, 0);
            return std::make_shared<Data::Value>((double)id);
        });

    // 38. لوحة_توسع / expand_panel
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x84\xd9\x88\xd8\xad\xd8\xa9_\xd8\xaa\xd9\x88\xd8\xb3\xd8\xb9", // لوحة_توسع
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string title = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            int id = sad::ui::widget_expansion_panel(title);
            sad::ui::widgetSetProp(id, "borderRadius", 8.0);
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 39. تفاصيل_قابلة_للطي / collapsible_details
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb5\xd9\x8a\xd9\x84_\xd9\x82\xd8\xa7\xd8\xa8\xd9\x84\xd8\xa9", // تفاصيل_قابلة
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string summary = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            int id = sad::ui::widget_expansion_panel(summary);
            sad::ui::widgetSetProp(id, "compact", true);
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 40. جدول_بيانات_متقدم / advanced_data_table
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xac\xd8\xaf\xd9\x88\xd9\x84_\xd8\xa8\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa_\xd9\x85\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85", // جدول_بيانات_متقدم
        [](Args args) -> std::shared_ptr<Data::Value> {
            int cols = args.size() > 0 && args[0]->isNumeric() ? (int)args[0]->toDouble() : 3;
            int id = sad::ui::widget_data_table(cols);
            sad::ui::widgetSetProp(id, "showDivider", true);
            sad::ui::widgetSetProp(id, "borderWidth", 1.0);
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // ═══════════════════════════════════════════════════════════════════════════
    // [41-60] Form & Input Widgets
    // ═══════════════════════════════════════════════════════════════════════════

    // 41. نموذج / form_widget
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x86\xd9\x85\xd9\x88\xd8\xb0\xd8\xac", // نموذج
        [](Args args) -> std::shared_ptr<Data::Value> {
            int col = sad::ui::widget_column();
            sad::ui::widgetSetProp(col, "spacing", 16.0);
            sad::ui::widgetSetProp(col, "padding", 20.0);
            _applyProps18(col, args, 0);
            return std::make_shared<Data::Value>((double)col);
        });

    // 42. حقل_مع_تسمية / labeled_field
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xad\xd9\x82\xd9\x84_\xd9\x85\xd8\xb9_\xd8\xaa\xd8\xb3\xd9\x85\xd9\x8a\xd8\xa9", // حقل_مع_تسمية
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string label = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            int col = sad::ui::widget_column();
            int lbl = sad::ui::widget_label(label);
            sad::ui::widgetSetProp(lbl, "fontSize", 14.0);
            sad::ui::widgetSetProp(lbl, "color", "#666666");
            sad::ui::widget_add_child(col, lbl);
            int field = sad::ui::widget_textfield("");
            sad::ui::widget_add_child(col, field);
            sad::ui::widgetSetProp(col, "spacing", 8.0);
            _applyProps18(col, args, 1);
            return std::make_shared<Data::Value>((double)col);
        });

    // 43. حقل_كلمة_مرور_مرئية / password_toggle
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xad\xd9\x82\xd9\x84_\xd9\x83\xd9\x84\xd9\x85\xd8\xa9_\xd9\x85\xd8\xb1\xd9\x88\xd8\xb1_\xd9\x85\xd8\xb1\xd8\xa6\xd9\x8a\xd8\xa9", // حقل_كلمة_مرور_مرئية
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string hint = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            int id = sad::ui::widget_password_field(hint);
            sad::ui::widgetSetProp(id, "borderRadius", 8.0);
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 44. حقل_بريد / email_field
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xad\xd9\x82\xd9\x84_\xd8\xa8\xd8\xb1\xd9\x8a\xd8\xaf", // حقل_بريد
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string hint = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "البريد الإلكتروني";
            int id = sad::ui::widget_textfield(hint);
            sad::ui::widgetSetProp(id, "inputType", "email");
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 45. حقل_هاتف / phone_field
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xad\xd9\x82\xd9\x84_\xd9\x87\xd8\xa7\xd8\xaa\xd9\x81", // حقل_هاتف
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string hint = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "رقم الهاتف";
            int id = sad::ui::widget_textfield(hint);
            sad::ui::widgetSetProp(id, "inputType", "phone");
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 46. حقل_رابط / url_field
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xad\xd9\x82\xd9\x84_\xd8\xb1\xd8\xa7\xd8\xa8\xd8\xb7", // حقل_رابط
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string hint = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "https://";
            int id = sad::ui::widget_textfield(hint);
            sad::ui::widgetSetProp(id, "inputType", "url");
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 47. منزلق_نطاق / range_slider_v2
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd9\x86\xd8\xb2\xd9\x84\xd9\x82_\xd9\x86\xd8\xb7\xd8\xa7\xd9\x82", // منزلق_نطاق
        [](Args args) -> std::shared_ptr<Data::Value> {
            double minV = args.size() > 0 && args[0]->isNumeric() ? args[0]->toDouble() : 0;
            double maxV = args.size() > 1 && args[1]->isNumeric() ? args[1]->toDouble() : 100;
            int id = sad::ui::widget_range_slider((float)minV, (float)maxV);
            _applyProps18(id, args, 2);
            return std::make_shared<Data::Value>((double)id);
        });

    // 48. منتقي_لون_متقدم / color_picker_v2
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd9\x86\xd8\xaa\xd9\x82\xd9\x8a_\xd9\x84\xd9\x88\xd9\x86_\xd9\x85\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85", // منتقي_لون_متقدم
        [](Args args) -> std::shared_ptr<Data::Value> {
            int id = sad::ui::widget_color_picker();
            sad::ui::widgetSetProp(id, "width", 200.0);
            sad::ui::widgetSetProp(id, "height", 200.0);
            _applyProps18(id, args, 0);
            return std::make_shared<Data::Value>((double)id);
        });

    // 49. تقييم_نجوم_متقدم / star_rating_v2
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xaa\xd9\x82\xd9\x8a\xd9\x8a\xd9\x85_\xd9\x86\xd8\xac\xd9\x88\xd9\x85_\xd9\x85\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85", // تقييم_نجوم_متقدم
        [](Args args) -> std::shared_ptr<Data::Value> {
            int max = args.size() > 0 && args[0]->isNumeric() ? (int)args[0]->toDouble() : 5;
            int id = sad::ui::widget_rating_stars(max);
            sad::ui::widgetSetProp(id, "accent", "#FFD700");
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 50. مجموعة_خيارات / option_group
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\xd8\xa9_\xd8\xae\xd9\x8a\xd8\xa7\xd8\xb1\xd8\xa7\xd8\xaa", // مجموعة_خيارات
        [](Args args) -> std::shared_ptr<Data::Value> {
            int col = sad::ui::widget_column();
            sad::ui::widgetSetProp(col, "spacing", 8.0);
            _applyProps18(col, args, 0);
            return std::make_shared<Data::Value>((double)col);
        });

    // 51. زر_راديو_مع_نص / radio_with_label
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb2\xd8\xb1_\xd8\xb1\xd8\xa7\xd8\xaf\xd9\x8a\xd9\x88_\xd9\x85\xd8\xb9_\xd9\x86\xd8\xb5", // زر_راديو_مع_نص
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string label = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            int id = sad::ui::widget_radio(label);
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 52. مربع_اختيار_مع_نص / checkbox_with_label
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xb1\xd8\xa8\xd8\xb9_\xd8\xa7\xd8\xae\xd8\xaa\xd9\x8a\xd8\xa7\xd8\xb1_\xd9\x85\xd8\xb9_\xd9\x86\xd8\xb5", // مربع_اختيار_مع_نص
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string label = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            int id = sad::ui::widget_checkbox(label);
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 53. قائمة_منسدلة_متعددة / multi_select_dropdown
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd9\x85\xd9\x86\xd8\xb3\xd8\xaf\xd9\x84\xd8\xa9_\xd9\x85\xd8\xaa\xd8\xb9\xd8\xaf\xd8\xaf\xd8\xa9", // قائمة_منسدلة_متعددة
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string hint = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "اختر...";
            int id = sad::ui::widget_dropdown(hint);
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 54. منتقي_تاريخ_متقدم / date_picker_v2
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd9\x86\xd8\xaa\xd9\x82\xd9\x8a_\xd8\xaa\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xae_\xd9\x85\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85", // منتقي_تاريخ_متقدم
        [](Args args) -> std::shared_ptr<Data::Value> {
            int id = sad::ui::widget_date_field();
            sad::ui::widgetSetProp(id, "borderRadius", 8.0);
            _applyProps18(id, args, 0);
            return std::make_shared<Data::Value>((double)id);
        });

    // 55. منتقي_وقت / time_picker
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd9\x86\xd8\xaa\xd9\x82\xd9\x8a_\xd9\x88\xd9\x82\xd8\xaa", // منتقي_وقت
        [](Args args) -> std::shared_ptr<Data::Value> {
            int id = sad::ui::widget_textfield("00:00");
            sad::ui::widgetSetProp(id, "inputType", "time");
            _applyProps18(id, args, 0);
            return std::make_shared<Data::Value>((double)id);
        });

    // 56. منطقة_سحب_ملفات / file_drop_zone
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd8\xa9_\xd8\xb3\xd8\xad\xd8\xa8_\xd9\x85\xd9\x84\xd9\x81\xd8\xa7\xd8\xaa", // منطقة_سحب_ملفات
        [](Args args) -> std::shared_ptr<Data::Value> {
            int id = sad::ui::widget_container();
            sad::ui::widgetSetProp(id, "width", 300.0);
            sad::ui::widgetSetProp(id, "height", 150.0);
            sad::ui::widgetSetProp(id, "borderWidth", 2.0);
            sad::ui::widgetSetProp(id, "borderStyle", "dashed");
            sad::ui::widgetSetProp(id, "borderColor", "#CCCCCC");
            sad::ui::widgetSetProp(id, "borderRadius", 8.0);
            sad::ui::widgetSetProp(id, "droppable", true);
            int txt = sad::ui::widget_text("اسحب الملفات هنا");
            sad::ui::widgetSetProp(txt, "color", "#999999");
            sad::ui::widgetSetProp(txt, "textAlign", "center");
            sad::ui::widget_add_child(id, txt);
            _applyProps18(id, args, 0);
            return std::make_shared<Data::Value>((double)id);
        });

    // 57. زر_رفع / upload_button
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb2\xd8\xb1_\xd8\xb1\xd9\x81\xd8\xb9", // زر_رفع
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string txt = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "رفع ملف";
            int id = sad::ui::widget_button(txt);
            sad::ui::widgetSetProp(id, "iconText", "📤");
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 58. ازرار_نعم_لا / yes_no_buttons
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa7\xd8\xb2\xd8\xb1\xd8\xa7\xd8\xb1_\xd9\x86\xd8\xb9\xd9\x85_\xd9\x84\xd8\xa7", // ازرار_نعم_لا
        [](Args args) -> std::shared_ptr<Data::Value> {
            int row = sad::ui::widget_row();
            sad::ui::widgetSetProp(row, "spacing", 12.0);
            int yes = sad::ui::widget_button("نعم");
            sad::ui::widgetSetProp(yes, "bg", "#4CAF50");
            int no = sad::ui::widget_button("لا");
            sad::ui::widgetSetProp(no, "bg", "#F44336");
            sad::ui::widget_add_child(row, yes);
            sad::ui::widget_add_child(row, no);
            _applyProps18(row, args, 0);
            return std::make_shared<Data::Value>((double)row);
        });

    // 59. ازرار_حفظ_الغاء / save_cancel_buttons
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa7\xd8\xb2\xd8\xb1\xd8\xa7\xd8\xb1_\xd8\xad\xd9\x81\xd8\xb8_\xd8\xa7\xd9\x84\xd8\xba\xd8\xa7\xd8\xa1", // ازرار_حفظ_الغاء
        [](Args args) -> std::shared_ptr<Data::Value> {
            int row = sad::ui::widget_row();
            sad::ui::widgetSetProp(row, "spacing", 12.0);
            sad::ui::widgetSetProp(row, "mainAxisAlignment", "end");
            int cancel = sad::ui::widget_outline_button("إلغاء");
            int save = sad::ui::widget_button("حفظ");
            sad::ui::widgetSetProp(save, "bg", "#2196F3");
            sad::ui::widget_add_child(row, cancel);
            sad::ui::widget_add_child(row, save);
            _applyProps18(row, args, 0);
            return std::make_shared<Data::Value>((double)row);
        });

    // 60. شريط_بحث_مع_فلتر / search_with_filter
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xa8\xd8\xad\xd8\xab_\xd9\x85\xd8\xb9_\xd9\x81\xd9\x84\xd8\xaa\xd8\xb1", // شريط_بحث_مع_فلتر
        [](Args args) -> std::shared_ptr<Data::Value> {
            int row = sad::ui::widget_row();
            sad::ui::widgetSetProp(row, "spacing", 8.0);
            int search = sad::ui::widget_search_field("بحث...");
            sad::ui::widgetSetProp(search, "flex", 1.0);
            int filter = sad::ui::widget_icon_button('F');
            sad::ui::widget_add_child(row, search);
            sad::ui::widget_add_child(row, filter);
            _applyProps18(row, args, 0);
            return std::make_shared<Data::Value>((double)row);
        });

    // ═══════════════════════════════════════════════════════════════════════════
    // [61-80] Special Advanced Widgets 
    // ═══════════════════════════════════════════════════════════════════════════

    // 61. صفحة_فارغة / empty_state
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb5\xd9\x81\xd8\xad\xd8\xa9_\xd9\x81\xd8\xa7\xd8\xb1\xd8\xba\xd8\xa9", // صفحة_فارغة
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string msg = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "لا توجد بيانات";
            int col = sad::ui::widget_column();
            sad::ui::widgetSetProp(col, "mainAxisAlignment", "center");
            sad::ui::widgetSetProp(col, "crossAxisAlignment", "center");
            sad::ui::widgetSetProp(col, "padding", 40.0);
            int icon = sad::ui::widget_text("📭");
            sad::ui::widgetSetProp(icon, "fontSize", 64.0);
            int txt = sad::ui::widget_text(msg);
            sad::ui::widgetSetProp(txt, "color", "#999999");
            sad::ui::widgetSetProp(txt, "fontSize", 18.0);
            sad::ui::widget_add_child(col, icon);
            sad::ui::widget_add_child(col, txt);
            _applyProps18(col, args, 1);
            return std::make_shared<Data::Value>((double)col);
        });

    // 62. شاشة_تحميل / loading_screen
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb4\xd8\xa7\xd8\xb4\xd8\xa9_\xd8\xaa\xd8\xad\xd9\x85\xd9\x8a\xd9\x84", // شاشة_تحميل
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string msg = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "جارٍ التحميل...";
            int col = sad::ui::widget_column();
            sad::ui::widgetSetProp(col, "mainAxisAlignment", "center");
            sad::ui::widgetSetProp(col, "crossAxisAlignment", "center");
            int spinner = sad::ui::widget_loading_spinner();
            int txt = sad::ui::widget_text(msg);
            sad::ui::widgetSetProp(txt, "color", "#666666");
            sad::ui::widget_add_child(col, spinner);
            sad::ui::widget_add_child(col, txt);
            _applyProps18(col, args, 1);
            return std::make_shared<Data::Value>((double)col);
        });

    // 63. صفحة_خطا / error_page
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb5\xd9\x81\xd8\xad\xd8\xa9_\xd8\xae\xd8\xb7\xd8\xa7", // صفحة_خطا
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string msg = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "حدث خطأ";
            int col = sad::ui::widget_column();
            sad::ui::widgetSetProp(col, "mainAxisAlignment", "center");
            sad::ui::widgetSetProp(col, "crossAxisAlignment", "center");
            sad::ui::widgetSetProp(col, "padding", 40.0);
            int icon = sad::ui::widget_text("⚠️");
            sad::ui::widgetSetProp(icon, "fontSize", 64.0);
            int txt = sad::ui::widget_text(msg);
            sad::ui::widgetSetProp(txt, "color", "#F44336");
            sad::ui::widgetSetProp(txt, "fontSize", 18.0);
            int btn = sad::ui::widget_button("إعادة المحاولة");
            sad::ui::widget_add_child(col, icon);
            sad::ui::widget_add_child(col, txt);
            sad::ui::widget_add_child(col, btn);
            _applyProps18(col, args, 1);
            return std::make_shared<Data::Value>((double)col);
        });

    // 64. بانر_ترحيب / welcome_banner
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa8\xd8\xa7\xd9\x86\xd8\xb1_\xd8\xaa\xd8\xb1\xd8\xad\xd9\x8a\xd8\xa8", // بانر_ترحيب
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string title = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "مرحباً";
            int container = sad::ui::widget_container();
            sad::ui::widgetSetProp(container, "gradientStart", "#667eea");
            sad::ui::widgetSetProp(container, "gradientEnd", "#764ba2");
            sad::ui::widgetSetProp(container, "hasGradient", true);
            sad::ui::widgetSetProp(container, "padding", 32.0);
            sad::ui::widgetSetProp(container, "borderRadius", 16.0);
            int txt = sad::ui::widget_heading(title, 1);
            sad::ui::widgetSetProp(txt, "color", "#FFFFFF");
            sad::ui::widget_add_child(container, txt);
            _applyProps18(container, args, 1);
            return std::make_shared<Data::Value>((double)container);
        });

    // 65. بطاقة_مستخدم / user_card
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9_\xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85", // بطاقة_مستخدم
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string name = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            int card = sad::ui::widget_card();
            sad::ui::widgetSetProp(card, "padding", 16.0);
            int row = sad::ui::widget_row();
            sad::ui::widgetSetProp(row, "spacing", 12.0);
            int avatar = sad::ui::widget_avatar(name.empty() ? "?" : name.substr(0,1), 100, 100, 255);
            int col = sad::ui::widget_column();
            int nameTxt = sad::ui::widget_text(name);
            sad::ui::widgetSetProp(nameTxt, "fontWeight", "bold");
            sad::ui::widget_add_child(col, nameTxt);
            sad::ui::widget_add_child(row, avatar);
            sad::ui::widget_add_child(row, col);
            sad::ui::widget_add_child(card, row);
            _applyProps18(card, args, 1);
            return std::make_shared<Data::Value>((double)card);
        });

    // 66. شريط_مهام / task_bar
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd9\x85\xd9\x87\xd8\xa7\xd9\x85", // شريط_مهام
        [](Args args) -> std::shared_ptr<Data::Value> {
            int row = sad::ui::widget_row();
            sad::ui::widgetSetProp(row, "bg", "#F5F5F5");
            sad::ui::widgetSetProp(row, "padding", 8.0);
            sad::ui::widgetSetProp(row, "mainAxisAlignment", "spaceBetween");
            _applyProps18(row, args, 0);
            return std::make_shared<Data::Value>((double)row);
        });

    // 67. شريط_حركة / action_bar
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xad\xd8\xb1\xd9\x83\xd8\xa9", // شريط_حركة
        [](Args args) -> std::shared_ptr<Data::Value> {
            int row = sad::ui::widget_row();
            sad::ui::widgetSetProp(row, "spacing", 8.0);
            sad::ui::widgetSetProp(row, "mainAxisAlignment", "end");
            _applyProps18(row, args, 0);
            return std::make_shared<Data::Value>((double)row);
        });

    // 68. لوحة_معلومات / info_panel
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x84\xd9\x88\xd8\xad\xd8\xa9_\xd9\x85\xd8\xb9\xd9\x84\xd9\x88\xd9\x85\xd8\xa7\xd8\xaa", // لوحة_معلومات
        [](Args args) -> std::shared_ptr<Data::Value> {
            int col = sad::ui::widget_column();
            sad::ui::widgetSetProp(col, "bg", "#E3F2FD");
            sad::ui::widgetSetProp(col, "padding", 16.0);
            sad::ui::widgetSetProp(col, "borderRadius", 8.0);
            sad::ui::widgetSetProp(col, "borderLeftWidth", 4.0);
            sad::ui::widgetSetProp(col, "borderLeftColor", "#2196F3");
            _applyProps18(col, args, 0);
            return std::make_shared<Data::Value>((double)col);
        });

    // 69. تنبيه_معلومات / info_alert
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xaa\xd9\x86\xd8\xa8\xd9\x8a\xd9\x87_\xd9\x85\xd8\xb9\xd9\x84\xd9\x88\xd9\x85\xd8\xa7\xd8\xaa", // تنبيه_معلومات
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string msg = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            int id = sad::ui::widget_alert(msg, "info");
            sad::ui::widgetSetProp(id, "bg", "#E3F2FD");
            sad::ui::widgetSetProp(id, "borderColor", "#2196F3");
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 70. تنبيه_تحذير / warning_alert
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xaa\xd9\x86\xd8\xa8\xd9\x8a\xd9\x87_\xd8\xaa\xd8\xad\xd8\xb0\xd9\x8a\xd8\xb1", // تنبيه_تحذير
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string msg = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            int id = sad::ui::widget_alert(msg, "warning");
            sad::ui::widgetSetProp(id, "bg", "#FFF3E0");
            sad::ui::widgetSetProp(id, "borderColor", "#FF9800");
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 71. عرض_شرائح / slideshow
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd8\xb1\xd8\xb6_\xd8\xb4\xd8\xb1\xd8\xa7\xd8\xa6\xd8\xad", // عرض_شرائح
        [](Args args) -> std::shared_ptr<Data::Value> {
            int id = sad::ui::widget_carousel();
            sad::ui::widgetSetProp(id, "animating", true);
            sad::ui::widgetSetProp(id, "swipeEnabled", true);
            _applyProps18(id, args, 0);
            return std::make_shared<Data::Value>((double)id);
        });

    // 72. معرض_صور / image_gallery
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xb9\xd8\xb1\xd8\xb6_\xd8\xb5\xd9\x88\xd8\xb1", // معرض_صور
        [](Args args) -> std::shared_ptr<Data::Value> {
            int cols = args.size() > 0 && args[0]->isNumeric() ? (int)args[0]->toDouble() : 3;
            int id = sad::ui::widget_gridview(cols);
            sad::ui::widgetSetProp(id, "spacing", 4.0);
            _applyProps18(id, args, 1);
            return std::make_shared<Data::Value>((double)id);
        });

    // 73. فيديو_مصغر / video_thumbnail
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x81\xd9\x8a\xd8\xaf\xd9\x8a\xd9\x88_\xd9\x85\xd8\xb5\xd8\xba\xd8\xb1", // فيديو_مصغر
        [](Args args) -> std::shared_ptr<Data::Value> {
            int container = sad::ui::widget_container();
            sad::ui::widgetSetProp(container, "bg", "#000000");
            sad::ui::widgetSetProp(container, "borderRadius", 8.0);
            int play = sad::ui::widget_text("▶");
            sad::ui::widgetSetProp(play, "fontSize", 48.0);
            sad::ui::widgetSetProp(play, "color", "#FFFFFF");
            sad::ui::widgetSetProp(play, "textAlign", "center");
            sad::ui::widget_add_child(container, play);
            _applyProps18(container, args, 0);
            return std::make_shared<Data::Value>((double)container);
        });

    // 74. شريط_وسائط / media_bar
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd9\x88\xd8\xb3\xd8\xa7\xd8\xa6\xd8\xb7", // شريط_وسائط
        [](Args args) -> std::shared_ptr<Data::Value> {
            int row = sad::ui::widget_row();
            sad::ui::widgetSetProp(row, "bg", "#212121");
            sad::ui::widgetSetProp(row, "padding", 8.0);
            sad::ui::widgetSetProp(row, "mainAxisAlignment", "spaceBetween");
            _applyProps18(row, args, 0);
            return std::make_shared<Data::Value>((double)row);
        });

    // 75. مشغل_صوت / audio_player
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xb4\xd8\xba\xd9\x84_\xd8\xb5\xd9\x88\xd8\xaa", // مشغل_صوت
        [](Args args) -> std::shared_ptr<Data::Value> {
            int row = sad::ui::widget_row();
            sad::ui::widgetSetProp(row, "spacing", 12.0);
            sad::ui::widgetSetProp(row, "padding", 12.0);
            sad::ui::widgetSetProp(row, "bg", "#F5F5F5");
            sad::ui::widgetSetProp(row, "borderRadius", 24.0);
            int play = sad::ui::widget_icon_button('>');
            int slider = sad::ui::widget_slider(0, 100);
            sad::ui::widgetSetProp(slider, "flex", 1.0);
            sad::ui::widget_add_child(row, play);
            sad::ui::widget_add_child(row, slider);
            _applyProps18(row, args, 0);
            return std::make_shared<Data::Value>((double)row);
        });

    // 76. محادثة_فقاعة / chat_bubble
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xad\xd8\xa7\xd8\xaf\xd8\xab\xd8\xa9_\xd9\x81\xd9\x82\xd8\xa7\xd8\xb9\xd8\xa9", // محادثة_فقاعة
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string msg = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            bool isMe = args.size() > 1 && args[1]->isBoolean() ? args[1]->toBool() : true;
            int container = sad::ui::widget_container();
            sad::ui::widgetSetProp(container, "bg", isMe ? "#DCF8C6" : "#FFFFFF");
            sad::ui::widgetSetProp(container, "padding", 12.0);
            sad::ui::widgetSetProp(container, "borderRadius", 16.0);
            sad::ui::widgetSetProp(container, "maxWidth", 280.0);
            int txt = sad::ui::widget_text(msg);
            sad::ui::widget_add_child(container, txt);
            _applyProps18(container, args, 2);
            return std::make_shared<Data::Value>((double)container);
        });

    // 77. قائمة_محادثات / chat_list
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd9\x85\xd8\xad\xd8\xa7\xd8\xaf\xd8\xab\xd8\xa7\xd8\xaa", // قائمة_محادثات
        [](Args args) -> std::shared_ptr<Data::Value> {
            int col = sad::ui::widget_column();
            sad::ui::widgetSetProp(col, "spacing", 8.0);
            sad::ui::widgetSetProp(col, "padding", 8.0);
            _applyProps18(col, args, 0);
            return std::make_shared<Data::Value>((double)col);
        });

    // 78. حقل_ارسال / send_field
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xad\xd9\x82\xd9\x84_\xd8\xa7\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84", // حقل_ارسال
        [](Args args) -> std::shared_ptr<Data::Value> {
            int row = sad::ui::widget_row();
            sad::ui::widgetSetProp(row, "spacing", 8.0);
            sad::ui::widgetSetProp(row, "padding", 8.0);
            int field = sad::ui::widget_textfield("اكتب رسالة...");
            sad::ui::widgetSetProp(field, "flex", 1.0);
            sad::ui::widgetSetProp(field, "borderRadius", 24.0);
            int send = sad::ui::widget_icon_button('>');
            sad::ui::widgetSetProp(send, "bg", "#25D366");
            sad::ui::widget_add_child(row, field);
            sad::ui::widget_add_child(row, send);
            _applyProps18(row, args, 0);
            return std::make_shared<Data::Value>((double)row);
        });

    // 79. اشعار_منبثق / popup_notification
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa7\xd8\xb4\xd8\xb9\xd8\xa7\xd8\xb1_\xd9\x85\xd9\x86\xd8\xa8\xd8\xab\xd9\x82", // اشعار_منبثق
        [](Args args) -> std::shared_ptr<Data::Value> {
            std::string title = args.size() > 0 && args[0]->isString() ? args[0]->toString() : "";
            std::string msg = args.size() > 1 && args[1]->isString() ? args[1]->toString() : "";
            int id = sad::ui::widget_notification(title, msg);
            sad::ui::widgetSetProp(id, "elevation", 8.0);
            sad::ui::widgetSetProp(id, "borderRadius", 12.0);
            _applyProps18(id, args, 2);
            return std::make_shared<Data::Value>((double)id);
        });

    // 80. شريط_اختصارات / shortcut_bar
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xa7\xd8\xae\xd8\xaa\xd8\xb5\xd8\xa7\xd8\xb1\xd8\xa7\xd8\xaa", // شريط_اختصارات
        [](Args args) -> std::shared_ptr<Data::Value> {
            int row = sad::ui::widget_row();
            sad::ui::widgetSetProp(row, "mainAxisAlignment", "spaceEvenly");
            sad::ui::widgetSetProp(row, "padding", 8.0);
            sad::ui::widgetSetProp(row, "bg", "#F5F5F5");
            _applyProps18(row, args, 0);
            return std::make_shared<Data::Value>((double)row);
        });
#endif // HAS_GRAPHICS
}

} // namespace Interpreter
} // namespace Sad
