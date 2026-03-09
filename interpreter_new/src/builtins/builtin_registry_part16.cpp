/**
 * @file builtin_registry_part16.cpp
 * @brief (AR) الجزء 16: 40+ ودجت مركّبة متقدمة بأسلوب Flutter + أدوات مساعدة
 * @brief (EN) Part 16: 40+ advanced composite Flutter-style widgets + helper utilities
 *
 * كل ودجت هنا عبارة عن تركيب ذكي من الودجات الأساسية (109 في part11)
 * لا يتطلب إضافة أنواع جديدة لـ WidgetType enum
 */

#include "builtins.h"
#include "interpreter_core.h"
#include "graphics/sad_ui_framework.h"
#include <string>
#include <vector>
#include <memory>
#include <cmath>

namespace Sad {
namespace Interpreter {

// ═══════════════════════════════════════════════════════════════════════════
// (AR) دالة مساعدة لتطبيق الخصائص السطرية على أي ودجت
// (EN) Helper to apply inline properties from variadic args
// ═══════════════════════════════════════════════════════════════════════════
#ifdef HAS_GRAPHICS
static void _applyProps(int id, const std::vector<std::shared_ptr<Data::Value>>& args, size_t startIdx) {
    for (size_t i = startIdx; i < args.size(); ++i) {
        if (args[i]->getType() == Data::ValueType::STRING) {
            std::string name = args[i]->toString();
            if (sad::ui::isKnownPropertyName(name) && i + 1 < args.size()) {
                auto& val = args[i + 1];
                auto vtype = val->getType();
                if (vtype == Data::ValueType::STRING) {
                    sad::ui::widgetSetProp(id, name, val->toString());
                } else if (vtype == Data::ValueType::BOOLEAN) {
                    sad::ui::widgetSetProp(id, name, val->toBool());
                } else {
                    sad::ui::widgetSetProp(id, name, val->toDouble());
                }
                i++;
                continue;
            }
        }
        if (args[i]->isNumeric()) {
            int cid = args[i]->toInt();
            if (cid >= 0) sad::ui::widget_add_child(id, cid);
        }
    }
}
#endif // HAS_GRAPHICS

void registerBuiltinsPart16(Interpreter& interpreter) {
#ifdef HAS_GRAPHICS

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 1. سقالة / scaffold — هيكل تطبيق Flutter الأساسي
    // (EN) Scaffold: AppBar + Body + optional BottomBar
    // ═══════════════════════════════════════════════════════════════════
    auto scaffold_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = args.empty() ? "" : args[0]->toString();
        int col = sad::ui::widget_column();
        // AppBar
        int appbar = sad::ui::widget_app_bar(title);
        sad::ui::widget_add_child(col, appbar);
        // Body region (Expanded)
        int body = sad::ui::widget_expanded();
        sad::ui::widget_add_child(col, body);
        // Apply inline props to outer column
        _applyProps(col, args, 1);
        return std::make_shared<Data::Value>(col);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb3\xd9\x82\xd8\xa7\xd9\x84\xd8\xa9", scaffold_func); // سقالة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_scaffold", scaffold_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 2. منطقة_امنة / safe_area — حاوية بحشوة تلقائية
    // ═══════════════════════════════════════════════════════════════════
    auto safe_area_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int c = sad::ui::widget_container();
        sad::ui::widgetSetProp(c, "padding", 16.0);
        _applyProps(c, args, 0);
        return std::make_shared<Data::Value>(c);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd8\xa9_\xd8\xa7\xd9\x85\xd9\x86\xd8\xa9", safe_area_func); // منطقة_امنة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_safe_area", safe_area_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 3. مبطن / padding — حاوية مبطنة (padding فقط)
    // ═══════════════════════════════════════════════════════════════════
    auto padding_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        double pad = args.empty() ? 8.0 : args[0]->toDouble();
        int c = sad::ui::widget_container();
        sad::ui::widgetSetProp(c, "padding", pad);
        _applyProps(c, args, 1);
        return std::make_shared<Data::Value>(c);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xa8\xd8\xb7\xd9\x86", padding_func); // مبطن
    interpreter.getFunctionManager().registerBuiltinFunction("ui_padding", padding_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 4. مبطن4 / padding4 — حشوة لكل جانب
    // ═══════════════════════════════════════════════════════════════════
    auto padding4_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        double top = args.size() > 0 ? args[0]->toDouble() : 8;
        double right = args.size() > 1 ? args[1]->toDouble() : 8;
        double bottom = args.size() > 2 ? args[2]->toDouble() : 8;
        double left = args.size() > 3 ? args[3]->toDouble() : 8;
        int c = sad::ui::widget_container();
        sad::ui::widgetSetProp(c, "padTop", top);
        sad::ui::widgetSetProp(c, "padRight", right);
        sad::ui::widgetSetProp(c, "padBottom", bottom);
        sad::ui::widgetSetProp(c, "padLeft", left);
        _applyProps(c, args, 4);
        return std::make_shared<Data::Value>(c);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xa8\xd8\xb7\xd9\x86" "4", padding4_func); // مبطن4
    interpreter.getFunctionManager().registerBuiltinFunction("ui_padding4", padding4_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 5. محاذي / align — تحديد محاذاة المحتوى
    // ═══════════════════════════════════════════════════════════════════
    auto align_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string alignment = args.empty() ? "center" : args[0]->toString();
        int c = sad::ui::widget_container();
        sad::ui::widgetSetProp(c, "mainAxisAlignment", alignment);
        sad::ui::widgetSetProp(c, "crossAxisAlignment", alignment);
        _applyProps(c, args, 1);
        return std::make_shared<Data::Value>(c);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xad\xd8\xa7\xd8\xb0\xd9\x8a", align_func); // محاذي
    interpreter.getFunctionManager().registerBuiltinFunction("ui_align", align_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 6. نسبة_ابعاد / aspect_ratio
    // ═══════════════════════════════════════════════════════════════════
    auto aspect_ratio_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        double ratio = args.empty() ? 1.0 : args[0]->toDouble();
        int c = sad::ui::widget_container();
        double w = 200;
        sad::ui::widgetSetProp(c, "width", w);
        sad::ui::widgetSetProp(c, "height", w / ratio);
        _applyProps(c, args, 1);
        return std::make_shared<Data::Value>(c);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x86\xd8\xb3\xd8\xa8\xd8\xa9_\xd8\xa7\xd8\xa8\xd8\xb9\xd8\xa7\xd8\xaf", aspect_ratio_func); // نسبة_ابعاد
    interpreter.getFunctionManager().registerBuiltinFunction("ui_aspect_ratio", aspect_ratio_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 7. مقيد / constrained_box — حاوية بقيود min/max
    // ═══════════════════════════════════════════════════════════════════
    auto constrained_box_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        double minW = args.size() > 0 ? args[0]->toDouble() : 0;
        double maxW = args.size() > 1 ? args[1]->toDouble() : 9999;
        double minH = args.size() > 2 ? args[2]->toDouble() : 0;
        double maxH = args.size() > 3 ? args[3]->toDouble() : 9999;
        int c = sad::ui::widget_container();
        sad::ui::widgetSetProp(c, "minWidth", minW);
        sad::ui::widgetSetProp(c, "maxWidth", maxW);
        sad::ui::widgetSetProp(c, "minHeight", minH);
        sad::ui::widgetSetProp(c, "maxHeight", maxH);
        _applyProps(c, args, 4);
        return std::make_shared<Data::Value>(c);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd9\x82\xd9\x8a\xd8\xaf", constrained_box_func); // مقيد
    interpreter.getFunctionManager().registerBuiltinFunction("ui_constrained_box", constrained_box_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 8. شفافية_مكون / opacity_widget
    // ═══════════════════════════════════════════════════════════════════
    auto opacity_widget_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        double op = args.empty() ? 1.0 : args[0]->toDouble();
        int c = sad::ui::widget_container();
        sad::ui::widgetSetProp(c, "opacity", op);
        _applyProps(c, args, 1);
        return std::make_shared<Data::Value>(c);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb4\xd9\x81\xd8\xa7\xd9\x81\xd9\x8a\xd8\xa9_\xd9\x85\xd9\x83\xd9\x88\xd9\x86", opacity_widget_func); // شفافية_مكون
    interpreter.getFunctionManager().registerBuiltinFunction("ui_opacity_widget", opacity_widget_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 9. مزخرف / decorated_box — حاوية بحدود وظل وزوايا
    // ═══════════════════════════════════════════════════════════════════
    auto decorated_box_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int c = sad::ui::widget_container();
        sad::ui::widgetSetProp(c, "borderRadius", 8.0);
        sad::ui::widgetSetProp(c, "borderWidth", 1.0);
        sad::ui::widgetSetProp(c, "shadow", 4.0);
        _applyProps(c, args, 0);
        return std::make_shared<Data::Value>(c);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xb2\xd8\xae\xd8\xb1\xd9\x81", decorated_box_func); // مزخرف
    interpreter.getFunctionManager().registerBuiltinFunction("ui_decorated_box", decorated_box_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 10. قص_دائري / clip_rrect — حاوية بقص دائري
    // ═══════════════════════════════════════════════════════════════════
    auto clip_rrect_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        double radius = args.empty() ? 16.0 : args[0]->toDouble();
        int c = sad::ui::widget_container();
        sad::ui::widgetSetProp(c, "borderRadius", radius);
        _applyProps(c, args, 1);
        return std::make_shared<Data::Value>(c);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x82\xd8\xb5_\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd9\x8a", clip_rrect_func); // قص_دائري
    interpreter.getFunctionManager().registerBuiltinFunction("ui_clip_rrect", clip_rrect_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 11. مرن / flexible — ابن مرن بعامل flex
    // ═══════════════════════════════════════════════════════════════════
    auto flexible_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int flexVal = args.empty() ? 1 : args[0]->toInt();
        int c = sad::ui::widget_expanded();
        sad::ui::widgetSetProp(c, "flex", static_cast<double>(flexVal));
        _applyProps(c, args, 1);
        return std::make_shared<Data::Value>(c);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xb1\xd9\x86", flexible_func); // مرن
    interpreter.getFunctionManager().registerBuiltinFunction("ui_flexible", flexible_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 12. صورة_دائرية / circle_avatar — صورة رمزية دائرية
    // ═══════════════════════════════════════════════════════════════════
    auto circle_avatar_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string letter = args.empty() ? "?" : args[0]->toString();
        int av = sad::ui::widget_avatar(letter, 66, 133, 244);
        sad::ui::widgetSetProp(av, "borderRadius", 50.0);
        _applyProps(av, args, 1);
        return std::make_shared<Data::Value>(av);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb5\xd9\x88\xd8\xb1\xd8\xa9_\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd9\x8a\xd8\xa9", circle_avatar_func); // صورة_دائرية
    interpreter.getFunctionManager().registerBuiltinFunction("ui_circle_avatar", circle_avatar_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 13. شريط_بحث_مكون / search_bar — حقل بحث مع أيقونة
    // ═══════════════════════════════════════════════════════════════════
    auto search_bar_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string hint = args.empty() ? "\xd8\xa8\xd8\xad\xd8\xab..." : args[0]->toString();
        int row = sad::ui::widget_row();
        sad::ui::widgetSetProp(row, "padding", 8.0);
        sad::ui::widgetSetProp(row, "borderRadius", 24.0);
        sad::ui::widgetSetProp(row, "background", "\xd9\x81\xd8\xb6\xd9\x8a");
        int icn = sad::ui::widget_icon('S');
        sad::ui::widget_add_child(row, icn);
        int fld = sad::ui::widget_search_field(hint);
        sad::ui::widgetSetProp(fld, "flex", 1.0);
        sad::ui::widget_add_child(row, fld);
        _applyProps(row, args, 1);
        return std::make_shared<Data::Value>(row);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xa8\xd8\xad\xd8\xab_\xd9\x85\xd9\x83\xd9\x88\xd9\x86", search_bar_func); // شريط_بحث_مكون
    interpreter.getFunctionManager().registerBuiltinFunction("ui_search_bar", search_bar_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 14. ورقة_سفلية / bottom_sheet
    // ═══════════════════════════════════════════════════════════════════
    auto bottom_sheet_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int c = sad::ui::widget_container();
        sad::ui::widgetSetProp(c, "background", "\xd8\xa7\xd8\xa8\xd9\x8a\xd8\xb6");
        sad::ui::widgetSetProp(c, "borderRadius", 16.0);
        sad::ui::widgetSetProp(c, "shadow", 8.0);
        sad::ui::widgetSetProp(c, "padding", 16.0);
        _applyProps(c, args, 0);
        return std::make_shared<Data::Value>(c);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x88\xd8\xb1\xd9\x82\xd8\xa9_\xd8\xb3\xd9\x81\xd9\x84\xd9\x8a\xd8\xa9", bottom_sheet_func); // ورقة_سفلية
    interpreter.getFunctionManager().registerBuiltinFunction("ui_bottom_sheet", bottom_sheet_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 15. شريط_تنقل_جانبي / navigation_rail
    // ═══════════════════════════════════════════════════════════════════
    auto navigation_rail_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int col = sad::ui::widget_column();
        sad::ui::widgetSetProp(col, "width", 72.0);
        sad::ui::widgetSetProp(col, "background", "\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a_\xd9\x81\xd8\xa7\xd8\xaa\xd8\xad");
        sad::ui::widgetSetProp(col, "padding", 8.0);
        _applyProps(col, args, 0);
        return std::make_shared<Data::Value>(col);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd9\x86\xd9\x82\xd9\x84_\xd8\xac\xd8\xa7\xd9\x86\xd8\xa8\xd9\x8a", navigation_rail_func); // شريط_تنقل_جانبي
    interpreter.getFunctionManager().registerBuiltinFunction("ui_navigation_rail", navigation_rail_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 16. زر_مقسم / segmented_button
    // ═══════════════════════════════════════════════════════════════════
    auto segmented_button_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int row = sad::ui::widget_row();
        sad::ui::widgetSetProp(row, "borderRadius", 20.0);
        sad::ui::widgetSetProp(row, "borderWidth", 1.0);
        sad::ui::widgetSetProp(row, "borderColor", "\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a");
        _applyProps(row, args, 0);
        return std::make_shared<Data::Value>(row);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb2\xd8\xb1_\xd9\x85\xd9\x82\xd8\xb3\xd9\x85", segmented_button_func); // زر_مقسم
    interpreter.getFunctionManager().registerBuiltinFunction("ui_segmented_button", segmented_button_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 17-20. أنواع الرقاقات / Chip Variants
    // ═══════════════════════════════════════════════════════════════════

    // 17. رقاقة_تصفية / filter_chip
    auto filter_chip_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string label = args.empty() ? "" : args[0]->toString();
        int ch = sad::ui::widget_chip(label);
        sad::ui::widgetSetProp(ch, "borderRadius", 16.0);
        sad::ui::widgetSetProp(ch, "borderWidth", 1.0);
        _applyProps(ch, args, 1);
        return std::make_shared<Data::Value>(ch);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb1\xd9\x82\xd8\xa7\xd9\x82\xd8\xa9_\xd8\xaa\xd8\xb5\xd9\x81\xd9\x8a\xd8\xa9", filter_chip_func); // رقاقة_تصفية
    interpreter.getFunctionManager().registerBuiltinFunction("ui_filter_chip", filter_chip_func);

    // 18. رقاقة_اختيار / choice_chip
    auto choice_chip_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string label = args.empty() ? "" : args[0]->toString();
        int ch = sad::ui::widget_chip(label);
        sad::ui::widgetSetProp(ch, "borderRadius", 16.0);
        sad::ui::widgetSetProp(ch, "background", "\xd8\xa7\xd8\xb2\xd8\xb1\xd9\x82_\xd9\x81\xd8\xa7\xd8\xaa\xd8\xad");
        _applyProps(ch, args, 1);
        return std::make_shared<Data::Value>(ch);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb1\xd9\x82\xd8\xa7\xd9\x82\xd8\xa9_\xd8\xa7\xd8\xae\xd8\xaa\xd9\x8a\xd8\xa7\xd8\xb1", choice_chip_func); // رقاقة_اختيار
    interpreter.getFunctionManager().registerBuiltinFunction("ui_choice_chip", choice_chip_func);

    // 19. رقاقة_ادخال / input_chip
    auto input_chip_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string label = args.empty() ? "" : args[0]->toString();
        int ch = sad::ui::widget_chip(label);
        sad::ui::widgetSetProp(ch, "borderRadius", 16.0);
        sad::ui::widgetSetProp(ch, "borderWidth", 1.0);
        sad::ui::widgetSetProp(ch, "borderColor", "\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a");
        _applyProps(ch, args, 1);
        return std::make_shared<Data::Value>(ch);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb1\xd9\x82\xd8\xa7\xd9\x82\xd8\xa9_\xd8\xa7\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84", input_chip_func); // رقاقة_ادخال
    interpreter.getFunctionManager().registerBuiltinFunction("ui_input_chip", input_chip_func);

    // 20. رقاقة_اجراء / action_chip
    auto action_chip_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string label = args.empty() ? "" : args[0]->toString();
        int ch = sad::ui::widget_chip(label);
        sad::ui::widgetSetProp(ch, "borderRadius", 16.0);
        sad::ui::widgetSetProp(ch, "background", "\xd8\xa7\xd8\xb2\xd8\xb1\xd9\x82");
        sad::ui::widgetSetProp(ch, "color", "\xd8\xa7\xd8\xa8\xd9\x8a\xd8\xb6");
        _applyProps(ch, args, 1);
        return std::make_shared<Data::Value>(ch);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb1\xd9\x82\xd8\xa7\xd9\x82\xd8\xa9_\xd8\xa7\xd8\xac\xd8\xb1\xd8\xa7\xd8\xa1", action_chip_func); // رقاقة_اجراء
    interpreter.getFunctionManager().registerBuiltinFunction("ui_action_chip", action_chip_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 21. مجموعة_نموذج / form_group — تجميع حقول النموذج
    // ═══════════════════════════════════════════════════════════════════
    auto form_group_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = args.empty() ? "" : args[0]->toString();
        int col = sad::ui::widget_column();
        sad::ui::widgetSetProp(col, "spacing", 8.0);
        sad::ui::widgetSetProp(col, "padding", 12.0);
        if (!title.empty()) {
            int lbl = sad::ui::widget_label(title);
            sad::ui::widgetSetProp(lbl, "fontSize", 16.0);
            sad::ui::widgetSetProp(lbl, "fontWeight", "bold");
            sad::ui::widget_add_child(col, lbl);
        }
        _applyProps(col, args, 1);
        return std::make_shared<Data::Value>(col);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\xd8\xa9_\xd9\x86\xd9\x85\xd9\x88\xd8\xb0\xd8\xac", form_group_func); // مجموعة_نموذج
    interpreter.getFunctionManager().registerBuiltinFunction("ui_form_group", form_group_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 22. قسم_قائمة / list_section — عنوان قسم في قائمة
    // ═══════════════════════════════════════════════════════════════════
    auto list_section_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = args.empty() ? "" : args[0]->toString();
        int col = sad::ui::widget_column();
        sad::ui::widgetSetProp(col, "spacing", 4.0);
        int lbl = sad::ui::widget_label(title);
        sad::ui::widgetSetProp(lbl, "fontSize", 12.0);
        sad::ui::widgetSetProp(lbl, "fontWeight", "bold");
        sad::ui::widgetSetProp(lbl, "color", "\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a");
        sad::ui::widget_add_child(col, lbl);
        int div = sad::ui::widget_divider();
        sad::ui::widget_add_child(col, div);
        _applyProps(col, args, 1);
        return std::make_shared<Data::Value>(col);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x82\xd8\xb3\xd9\x85_\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9", list_section_func); // قسم_قائمة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_list_section", list_section_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 23. حالة_فارغة / empty_state — عرض حالة "لا بيانات"
    // ═══════════════════════════════════════════════════════════════════
    auto empty_state_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string msg = args.empty() ? "\xd9\x84\xd8\xa7 \xd8\xaa\xd9\x88\xd8\xac\xd8\xaf \xd8\xa8\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa" : args[0]->toString();
        int c = sad::ui::widget_center();
        int col = sad::ui::widget_column();
        sad::ui::widgetSetProp(col, "spacing", 12.0);
        int icn = sad::ui::widget_icon('!');
        sad::ui::widgetSetProp(icn, "fontSize", 48.0);
        sad::ui::widgetSetProp(icn, "color", "\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a");
        sad::ui::widget_add_child(col, icn);
        int txt = sad::ui::widget_text(msg);
        sad::ui::widgetSetProp(txt, "color", "\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a");
        sad::ui::widgetSetProp(txt, "fontSize", 16.0);
        sad::ui::widget_add_child(col, txt);
        sad::ui::widget_add_child(c, col);
        _applyProps(c, args, 1);
        return std::make_shared<Data::Value>(c);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9_\xd9\x81\xd8\xa7\xd8\xb1\xd8\xba\xd8\xa9", empty_state_func); // حالة_فارغة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_empty_state", empty_state_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 24. عنصر_خطا / error_widget
    // ═══════════════════════════════════════════════════════════════════
    auto error_widget_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string msg = args.empty() ? "\xd8\xae\xd8\xb7\xd8\xa3" : args[0]->toString();
        int c = sad::ui::widget_container();
        sad::ui::widgetSetProp(c, "background", "\xd8\xa7\xd8\xad\xd9\x85\xd8\xb1_\xd9\x81\xd8\xa7\xd8\xaa\xd8\xad");
        sad::ui::widgetSetProp(c, "padding", 12.0);
        sad::ui::widgetSetProp(c, "borderRadius", 8.0);
        int row = sad::ui::widget_row();
        sad::ui::widgetSetProp(row, "spacing", 8.0);
        int icn = sad::ui::widget_icon('X');
        sad::ui::widgetSetProp(icn, "color", "\xd8\xa7\xd8\xad\xd9\x85\xd8\xb1");
        sad::ui::widget_add_child(row, icn);
        int txt = sad::ui::widget_text(msg);
        sad::ui::widgetSetProp(txt, "color", "\xd8\xa7\xd8\xad\xd9\x85\xd8\xb1");
        sad::ui::widget_add_child(row, txt);
        sad::ui::widget_add_child(c, row);
        _applyProps(c, args, 1);
        return std::make_shared<Data::Value>(c);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xae\xd8\xb7\xd8\xa7", error_widget_func); // عنصر_خطا
    interpreter.getFunctionManager().registerBuiltinFunction("ui_error_widget", error_widget_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 25. عنصر_مؤقت / placeholder
    // ═══════════════════════════════════════════════════════════════════
    auto placeholder_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        double w = args.size() > 0 ? args[0]->toDouble() : 100;
        double h = args.size() > 1 ? args[1]->toDouble() : 100;
        int c = sad::ui::widget_container();
        sad::ui::widgetSetProp(c, "width", w);
        sad::ui::widgetSetProp(c, "height", h);
        sad::ui::widgetSetProp(c, "background", "\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a_\xd9\x81\xd8\xa7\xd8\xaa\xd8\xad");
        sad::ui::widgetSetProp(c, "borderWidth", 1.0);
        sad::ui::widgetSetProp(c, "borderColor", "\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a");
        int txt = sad::ui::widget_text("X");
        sad::ui::widgetSetProp(txt, "color", "\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a");
        sad::ui::widget_add_child(c, txt);
        _applyProps(c, args, 2);
        return std::make_shared<Data::Value>(c);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa", placeholder_func); // عنصر_مؤقت
    interpreter.getFunctionManager().registerBuiltinFunction("ui_placeholder", placeholder_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 26. بطاقة_مؤشر / kpi_card — مؤشر أداء رئيسي
    // ═══════════════════════════════════════════════════════════════════
    auto kpi_card_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string label = args.size() > 0 ? args[0]->toString() : "KPI";
        std::string value = args.size() > 1 ? args[1]->toString() : "0";
        int card = sad::ui::widget_card();
        sad::ui::widgetSetProp(card, "padding", 16.0);
        sad::ui::widgetSetProp(card, "borderRadius", 12.0);
        int col = sad::ui::widget_column();
        sad::ui::widgetSetProp(col, "spacing", 4.0);
        int lTxt = sad::ui::widget_label(label);
        sad::ui::widgetSetProp(lTxt, "fontSize", 12.0);
        sad::ui::widgetSetProp(lTxt, "color", "\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a");
        sad::ui::widget_add_child(col, lTxt);
        int vTxt = sad::ui::widget_text(value);
        sad::ui::widgetSetProp(vTxt, "fontSize", 28.0);
        sad::ui::widgetSetProp(vTxt, "fontWeight", "bold");
        sad::ui::widget_add_child(col, vTxt);
        sad::ui::widget_add_child(card, col);
        _applyProps(card, args, 2);
        return std::make_shared<Data::Value>(card);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9_\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1", kpi_card_func); // بطاقة_مؤشر
    interpreter.getFunctionManager().registerBuiltinFunction("ui_kpi_card", kpi_card_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 27. عد_تنازلي / countdown
    // ═══════════════════════════════════════════════════════════════════
    auto countdown_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int seconds = args.empty() ? 60 : args[0]->toInt();
        int mins = seconds / 60;
        int secs = seconds % 60;
        std::string display = std::to_string(mins) + ":" + (secs < 10 ? "0" : "") + std::to_string(secs);
        int txt = sad::ui::widget_text(display);
        sad::ui::widgetSetProp(txt, "fontSize", 32.0);
        sad::ui::widgetSetProp(txt, "fontWeight", "bold");
        _applyProps(txt, args, 1);
        return std::make_shared<Data::Value>(txt);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd8\xaf_\xd8\xaa\xd9\x86\xd8\xa7\xd8\xb2\xd9\x84\xd9\x8a", countdown_func); // عد_تنازلي
    interpreter.getFunctionManager().registerBuiltinFunction("ui_countdown", countdown_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 28. فاصل_ملون / divider_themed
    // ═══════════════════════════════════════════════════════════════════
    auto divider_themed_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string color = args.empty() ? "\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a" : args[0]->toString();
        int d = sad::ui::widget_divider();
        sad::ui::widgetSetProp(d, "color", color);
        _applyProps(d, args, 1);
        return std::make_shared<Data::Value>(d);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x81\xd8\xa7\xd8\xb5\xd9\x84_\xd9\x85\xd9\x84\xd9\x88\xd9\x86", divider_themed_func); // فاصل_ملون
    interpreter.getFunctionManager().registerBuiltinFunction("ui_divider_themed", divider_themed_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 29. بطاقة_تدرج / gradient_card — بطاقة بخلفية متدرجة
    // ═══════════════════════════════════════════════════════════════════
    auto gradient_card_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string c1 = args.size() > 0 ? args[0]->toString() : "\xd8\xa7\xd8\xb2\xd8\xb1\xd9\x82";
        std::string c2 = args.size() > 1 ? args[1]->toString() : "\xd8\xa8\xd9\x86\xd9\x81\xd8\xb3\xd8\xac\xd9\x8a";
        int card = sad::ui::widget_card();
        sad::ui::widgetSetProp(card, "borderRadius", 12.0);
        sad::ui::widgetSetProp(card, "padding", 16.0);
        sad::ui::widgetSetProp(card, "hasGradient", true);
        sad::ui::widgetSetProp(card, "gradientStart", c1);
        sad::ui::widgetSetProp(card, "gradientEnd", c2);
        _applyProps(card, args, 2);
        return std::make_shared<Data::Value>(card);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9_\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac", gradient_card_func); // بطاقة_تدرج
    interpreter.getFunctionManager().registerBuiltinFunction("ui_gradient_card", gradient_card_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 30. بطاقة_محددة / outlined_card — بطاقة بحدود فقط
    // ═══════════════════════════════════════════════════════════════════
    auto outlined_card_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int card = sad::ui::widget_card();
        sad::ui::widgetSetProp(card, "borderWidth", 1.0);
        sad::ui::widgetSetProp(card, "borderColor", "\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a");
        sad::ui::widgetSetProp(card, "borderRadius", 8.0);
        sad::ui::widgetSetProp(card, "shadow", 0.0);
        sad::ui::widgetSetProp(card, "padding", 12.0);
        _applyProps(card, args, 0);
        return std::make_shared<Data::Value>(card);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf\xd8\xa9", outlined_card_func); // بطاقة_محددة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_outlined_card", outlined_card_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 31. عنوان_بطولي / hero_header — عنوان كبير مع عنوان فرعي
    // ═══════════════════════════════════════════════════════════════════
    auto hero_header_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = args.size() > 0 ? args[0]->toString() : "";
        std::string subtitle = args.size() > 1 ? args[1]->toString() : "";
        int col = sad::ui::widget_column();
        sad::ui::widgetSetProp(col, "spacing", 8.0);
        sad::ui::widgetSetProp(col, "padding", 24.0);
        int h1 = sad::ui::widget_heading(title, 1);
        sad::ui::widgetSetProp(h1, "fontSize", 36.0);
        sad::ui::widget_add_child(col, h1);
        if (!subtitle.empty()) {
            int sub = sad::ui::widget_text(subtitle);
            sad::ui::widgetSetProp(sub, "fontSize", 18.0);
            sad::ui::widgetSetProp(sub, "color", "\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a");
            sad::ui::widget_add_child(col, sub);
        }
        _applyProps(col, args, 2);
        return std::make_shared<Data::Value>(col);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86_\xd8\xa8\xd8\xb7\xd9\x88\xd9\x84\xd9\x8a", hero_header_func); // عنوان_بطولي
    interpreter.getFunctionManager().registerBuiltinFunction("ui_hero_header", hero_header_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 32. نموذج_دخول / login_form — نموذج تسجيل دخول
    // ═══════════════════════════════════════════════════════════════════
    auto login_form_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = args.empty() ? "\xd8\xaa\xd8\xb3\xd8\xac\xd9\x8a\xd9\x84 \xd8\xaf\xd8\xae\xd9\x88\xd9\x84" : args[0]->toString();
        int card = sad::ui::widget_card();
        sad::ui::widgetSetProp(card, "padding", 24.0);
        sad::ui::widgetSetProp(card, "borderRadius", 12.0);
        int col = sad::ui::widget_column();
        sad::ui::widgetSetProp(col, "spacing", 16.0);
        int h = sad::ui::widget_heading(title, 2);
        sad::ui::widget_add_child(col, h);
        int email = sad::ui::widget_textfield("\xd8\xa7\xd9\x84\xd8\xa8\xd8\xb1\xd9\x8a\xd8\xaf \xd8\xa7\xd9\x84\xd8\xa5\xd9\x84\xd9\x83\xd8\xaa\xd8\xb1\xd9\x88\xd9\x86\xd9\x8a");
        sad::ui::widget_add_child(col, email);
        int pass = sad::ui::widget_password_field("\xd9\x83\xd9\x84\xd9\x85\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb1\xd9\x88\xd8\xb1");
        sad::ui::widget_add_child(col, pass);
        int btn = sad::ui::widget_button("\xd8\xaf\xd8\xae\xd9\x88\xd9\x84");
        sad::ui::widget_add_child(col, btn);
        sad::ui::widget_add_child(card, col);
        _applyProps(card, args, 1);
        return std::make_shared<Data::Value>(card);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x86\xd9\x85\xd9\x88\xd8\xb0\xd8\xac_\xd8\xaf\xd8\xae\xd9\x88\xd9\x84", login_form_func); // نموذج_دخول
    interpreter.getFunctionManager().registerBuiltinFunction("ui_login_form", login_form_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 33. مجموعة_صور / avatar_group — صف من الصور الرمزية
    // ═══════════════════════════════════════════════════════════════════
    auto avatar_group_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int count = args.empty() ? 3 : args[0]->toInt();
        int row = sad::ui::widget_row();
        sad::ui::widgetSetProp(row, "spacing", -8.0);
        for (int i = 0; i < count && i < 10; ++i) {
            char letter = 'A' + (i % 26);
            std::string s(1, letter);
            int av = sad::ui::widget_avatar(s, 66 + i*30, 133 - i*20, 244 - i*40);
            sad::ui::widgetSetProp(av, "borderRadius", 50.0);
            sad::ui::widgetSetProp(av, "borderWidth", 2.0);
            sad::ui::widgetSetProp(av, "borderColor", "\xd8\xa7\xd8\xa8\xd9\x8a\xd8\xb6");
            sad::ui::widget_add_child(row, av);
        }
        _applyProps(row, args, 1);
        return std::make_shared<Data::Value>(row);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\xd8\xa9_\xd8\xb5\xd9\x88\xd8\xb1", avatar_group_func); // مجموعة_صور
    interpreter.getFunctionManager().registerBuiltinFunction("ui_avatar_group", avatar_group_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 34. اجراء_شريط / toolbar_action — زر شريط أدوات
    // ═══════════════════════════════════════════════════════════════════
    auto toolbar_action_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string label = args.empty() ? "" : args[0]->toString();
        char icon = args.size() > 1 ? args[1]->toString()[0] : '+';
        int row = sad::ui::widget_row();
        sad::ui::widgetSetProp(row, "spacing", 4.0);
        sad::ui::widgetSetProp(row, "padding", 8.0);
        int ic = sad::ui::widget_icon(icon);
        sad::ui::widget_add_child(row, ic);
        if (!label.empty()) {
            int txt = sad::ui::widget_text(label);
            sad::ui::widget_add_child(row, txt);
        }
        _applyProps(row, args, 2);
        return std::make_shared<Data::Value>(row);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa7\xd8\xac\xd8\xb1\xd8\xa7\xd8\xa1_\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7", toolbar_action_func); // اجراء_شريط
    interpreter.getFunctionManager().registerBuiltinFunction("ui_toolbar_action", toolbar_action_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 35. مجموعة_وسوم / tag_group — مجموعة وسوم ملتفة
    // ═══════════════════════════════════════════════════════════════════
    auto tag_group_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int wrap = sad::ui::widget_wrap();
        sad::ui::widgetSetProp(wrap, "spacing", 6.0);
        _applyProps(wrap, args, 0);
        return std::make_shared<Data::Value>(wrap);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\xd8\xa9_\xd9\x88\xd8\xb3\xd9\x88\xd9\x85", tag_group_func); // مجموعة_وسوم
    interpreter.getFunctionManager().registerBuiltinFunction("ui_tag_group", tag_group_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 36. شريط_تقييم / rating_bar — صف نجوم تقييم
    // ═══════════════════════════════════════════════════════════════════
    auto rating_bar_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int maxStars = args.empty() ? 5 : args[0]->toInt();
        int filled = args.size() > 1 ? args[1]->toInt() : 0;
        int row = sad::ui::widget_row();
        sad::ui::widgetSetProp(row, "spacing", 2.0);
        for (int i = 0; i < maxStars; ++i) {
            int star = sad::ui::widget_icon(i < filled ? '*' : 'o');
            sad::ui::widgetSetProp(star, "color", i < filled ? "\xd8\xa7\xd8\xb5\xd9\x81\xd8\xb1" : "\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a");
            sad::ui::widgetSetProp(star, "fontSize", 20.0);
            sad::ui::widget_add_child(row, star);
        }
        _applyProps(row, args, 2);
        return std::make_shared<Data::Value>(row);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd9\x82\xd9\x8a\xd9\x8a\xd9\x85", rating_bar_func); // شريط_تقييم
    interpreter.getFunctionManager().registerBuiltinFunction("ui_rating_bar", rating_bar_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 37. مؤشر_تقدم_مسمى / progress_indicator — شريط تقدم بنص
    // ═══════════════════════════════════════════════════════════════════
    auto progress_indicator_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string label = args.size() > 0 ? args[0]->toString() : "";
        double value = args.size() > 1 ? args[1]->toDouble() : 0;
        int col = sad::ui::widget_column();
        sad::ui::widgetSetProp(col, "spacing", 4.0);
        if (!label.empty()) {
            int row = sad::ui::widget_row();
            int lbl = sad::ui::widget_text(label);
            sad::ui::widgetSetProp(lbl, "fontSize", 12.0);
            sad::ui::widget_add_child(row, lbl);
            int pct = sad::ui::widget_text(std::to_string(static_cast<int>(value * 100)) + "%");
            sad::ui::widgetSetProp(pct, "fontSize", 12.0);
            sad::ui::widget_add_child(row, pct);
            sad::ui::widget_add_child(col, row);
        }
        int bar = sad::ui::widget_progress_bar(static_cast<float>(value));
        sad::ui::widget_add_child(col, bar);
        _applyProps(col, args, 2);
        return std::make_shared<Data::Value>(col);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1_\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85_\xd9\x85\xd8\xb3\xd9\x85\xd9\x89", progress_indicator_func); // مؤشر_تقدم_مسمى
    interpreter.getFunctionManager().registerBuiltinFunction("ui_progress_indicator", progress_indicator_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 38. زر_منقسم / split_button — زر رئيسي + dropdown
    // ═══════════════════════════════════════════════════════════════════
    auto split_button_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string label = args.empty() ? "" : args[0]->toString();
        int row = sad::ui::widget_row();
        int btn = sad::ui::widget_button(label);
        sad::ui::widgetSetProp(btn, "borderRadius", 0.0);
        sad::ui::widget_add_child(row, btn);
        int arrow = sad::ui::widget_icon('v');
        sad::ui::widgetSetProp(arrow, "padding", 8.0);
        sad::ui::widgetSetProp(arrow, "background", "\xd8\xa7\xd8\xb2\xd8\xb1\xd9\x82");
        sad::ui::widgetSetProp(arrow, "color", "\xd8\xa7\xd8\xa8\xd9\x8a\xd8\xb6");
        sad::ui::widget_add_child(row, arrow);
        _applyProps(row, args, 1);
        return std::make_shared<Data::Value>(row);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb2\xd8\xb1_\xd9\x85\xd9\x86\xd9\x82\xd8\xb3\xd9\x85", split_button_func); // زر_منقسم
    interpreter.getFunctionManager().registerBuiltinFunction("ui_split_button", split_button_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 39. زر_عائم_ممتد / fab_extended — FAB ممتد
    // ═══════════════════════════════════════════════════════════════════
    auto fab_extended_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string label = args.empty() ? "" : args[0]->toString();
        int btn = sad::ui::widget_button(label);
        sad::ui::widgetSetProp(btn, "borderRadius", 28.0);
        sad::ui::widgetSetProp(btn, "padding", 16.0);
        sad::ui::widgetSetProp(btn, "shadow", 6.0);
        sad::ui::widgetSetProp(btn, "background", "\xd8\xa7\xd8\xb2\xd8\xb1\xd9\x82");
        sad::ui::widgetSetProp(btn, "color", "\xd8\xa7\xd8\xa8\xd9\x8a\xd8\xb6");
        _applyProps(btn, args, 1);
        return std::make_shared<Data::Value>(btn);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb2\xd8\xb1_\xd8\xb9\xd8\xa7\xd8\xa6\xd9\x85_\xd9\x85\xd9\x85\xd8\xaa\xd8\xaf", fab_extended_func); // زر_عائم_ممتد
    interpreter.getFunctionManager().registerBuiltinFunction("ui_fab_extended", fab_extended_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 40. مجموعة_رقاقات / chip_group — مجموعة رقاقات ملتفة
    // ═══════════════════════════════════════════════════════════════════
    auto chip_group_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int wrap = sad::ui::widget_wrap();
        sad::ui::widgetSetProp(wrap, "spacing", 8.0);
        _applyProps(wrap, args, 0);
        return std::make_shared<Data::Value>(wrap);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\xd8\xa9_\xd8\xb1\xd9\x82\xd8\xa7\xd9\x82\xd8\xa7\xd8\xaa", chip_group_func); // مجموعة_رقاقات
    interpreter.getFunctionManager().registerBuiltinFunction("ui_chip_group", chip_group_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 41. درج_تنقل / nav_drawer — درج تنقل جانبي
    // ═══════════════════════════════════════════════════════════════════
    auto nav_drawer_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = args.empty() ? "" : args[0]->toString();
        int col = sad::ui::widget_column();
        sad::ui::widgetSetProp(col, "width", 280.0);
        sad::ui::widgetSetProp(col, "background", "\xd8\xa7\xd8\xa8\xd9\x8a\xd8\xb6");
        sad::ui::widgetSetProp(col, "shadow", 4.0);
        if (!title.empty()) {
            int header = sad::ui::widget_container();
            sad::ui::widgetSetProp(header, "padding", 16.0);
            sad::ui::widgetSetProp(header, "background", "\xd8\xa7\xd8\xb2\xd8\xb1\xd9\x82");
            int h = sad::ui::widget_text(title);
            sad::ui::widgetSetProp(h, "color", "\xd8\xa7\xd8\xa8\xd9\x8a\xd8\xb6");
            sad::ui::widgetSetProp(h, "fontSize", 20.0);
            sad::ui::widget_add_child(header, h);
            sad::ui::widget_add_child(col, header);
        }
        _applyProps(col, args, 1);
        return std::make_shared<Data::Value>(col);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xaf\xd8\xb1\xd8\xac_\xd8\xaa\xd9\x86\xd9\x82\xd9\x84", nav_drawer_func); // درج_تنقل
    interpreter.getFunctionManager().registerBuiltinFunction("ui_nav_drawer", nav_drawer_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 42. بطاقة_صورة / image_card — بطاقة مع صورة وعنوان
    // ═══════════════════════════════════════════════════════════════════
    auto image_card_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = args.size() > 0 ? args[0]->toString() : "";
        std::string desc = args.size() > 1 ? args[1]->toString() : "";
        int card = sad::ui::widget_card();
        sad::ui::widgetSetProp(card, "borderRadius", 12.0);
        int col = sad::ui::widget_column();
        // Image placeholder
        int img = sad::ui::widget_container();
        sad::ui::widgetSetProp(img, "height", 160.0);
        sad::ui::widgetSetProp(img, "background", "\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a_\xd9\x81\xd8\xa7\xd8\xaa\xd8\xad");
        sad::ui::widget_add_child(col, img);
        // Text content
        int content = sad::ui::widget_column();
        sad::ui::widgetSetProp(content, "padding", 12.0);
        sad::ui::widgetSetProp(content, "spacing", 4.0);
        if (!title.empty()) {
            int t = sad::ui::widget_text(title);
            sad::ui::widgetSetProp(t, "fontSize", 16.0);
            sad::ui::widgetSetProp(t, "fontWeight", "bold");
            sad::ui::widget_add_child(content, t);
        }
        if (!desc.empty()) {
            int d = sad::ui::widget_text(desc);
            sad::ui::widgetSetProp(d, "fontSize", 14.0);
            sad::ui::widgetSetProp(d, "color", "\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a");
            sad::ui::widget_add_child(content, d);
        }
        sad::ui::widget_add_child(col, content);
        sad::ui::widget_add_child(card, col);
        _applyProps(card, args, 2);
        return std::make_shared<Data::Value>(card);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9_\xd8\xb5\xd9\x88\xd8\xb1\xd8\xa9", image_card_func); // بطاقة_صورة
    interpreter.getFunctionManager().registerBuiltinFunction("ui_image_card", image_card_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 43. قائمة_تحديد / selection_list — قائمة مع عناصر قابلة للتحديد
    // ═══════════════════════════════════════════════════════════════════
    auto selection_list_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int col = sad::ui::widget_column();
        sad::ui::widgetSetProp(col, "spacing", 0.0);
        sad::ui::widgetSetProp(col, "borderWidth", 1.0);
        sad::ui::widgetSetProp(col, "borderColor", "\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a");
        sad::ui::widgetSetProp(col, "borderRadius", 8.0);
        _applyProps(col, args, 0);
        return std::make_shared<Data::Value>(col);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd8\xaa\xd8\xad\xd8\xaf\xd9\x8a\xd8\xaf", selection_list_func); // قائمة_تحديد
    interpreter.getFunctionManager().registerBuiltinFunction("ui_selection_list", selection_list_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 44. مربع_رمز / icon_box — أيقونة في صندوق ملون
    // ═══════════════════════════════════════════════════════════════════
    auto icon_box_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        char icon = args.empty() ? '*' : args[0]->toString()[0];
        std::string bgColor = args.size() > 1 ? args[1]->toString() : "\xd8\xa7\xd8\xb2\xd8\xb1\xd9\x82_\xd9\x81\xd8\xa7\xd8\xaa\xd8\xad";
        int c = sad::ui::widget_container();
        sad::ui::widgetSetProp(c, "width", 48.0);
        sad::ui::widgetSetProp(c, "height", 48.0);
        sad::ui::widgetSetProp(c, "borderRadius", 12.0);
        sad::ui::widgetSetProp(c, "background", bgColor);
        int ic = sad::ui::widget_icon(icon);
        sad::ui::widgetSetProp(ic, "fontSize", 24.0);
        sad::ui::widget_add_child(c, ic);
        _applyProps(c, args, 2);
        return std::make_shared<Data::Value>(c);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xb1\xd8\xa8\xd8\xb9_\xd8\xb1\xd9\x85\xd8\xb2", icon_box_func); // مربع_رمز
    interpreter.getFunctionManager().registerBuiltinFunction("ui_icon_box", icon_box_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 45. شريحة_عرض / slide — شريحة عرض تقديمي
    // ═══════════════════════════════════════════════════════════════════
    auto slide_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = args.empty() ? "" : args[0]->toString();
        int c = sad::ui::widget_container();
        sad::ui::widgetSetProp(c, "padding", 32.0);
        sad::ui::widgetSetProp(c, "background", "\xd8\xa7\xd8\xa8\xd9\x8a\xd8\xb6");
        int col = sad::ui::widget_column();
        sad::ui::widgetSetProp(col, "spacing", 16.0);
        if (!title.empty()) {
            int h = sad::ui::widget_heading(title, 1);
            sad::ui::widgetSetProp(h, "fontSize", 32.0);
            sad::ui::widget_add_child(col, h);
        }
        sad::ui::widget_add_child(c, col);
        _applyProps(c, args, 1);
        return std::make_shared<Data::Value>(c);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xad\xd8\xa9_\xd8\xb9\xd8\xb1\xd8\xb6", slide_func); // شريحة_عرض
    interpreter.getFunctionManager().registerBuiltinFunction("ui_slide", slide_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 46. ملخص_بيانات / data_summary — صف ملخص بيانات
    // ═══════════════════════════════════════════════════════════════════
    auto data_summary_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int row = sad::ui::widget_row();
        sad::ui::widgetSetProp(row, "spacing", 16.0);
        sad::ui::widgetSetProp(row, "padding", 12.0);
        _applyProps(row, args, 0);
        return std::make_shared<Data::Value>(row);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd9\x84\xd8\xae\xd8\xb5_\xd8\xa8\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa", data_summary_func); // ملخص_بيانات
    interpreter.getFunctionManager().registerBuiltinFunction("ui_data_summary", data_summary_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 47. تنبيه_نجاح / success_alert
    // ═══════════════════════════════════════════════════════════════════
    auto success_alert_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string msg = args.empty() ? "\xd8\xaa\xd9\x85 \xd8\xa8\xd9\x86\xd8\xac\xd8\xa7\xd8\xad" : args[0]->toString();
        int al = sad::ui::widget_alert(msg, "success");
        _applyProps(al, args, 1);
        return std::make_shared<Data::Value>(al);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xaa\xd9\x86\xd8\xa8\xd9\x8a\xd9\x87_\xd9\x86\xd8\xac\xd8\xa7\xd8\xad", success_alert_func); // تنبيه_نجاح
    interpreter.getFunctionManager().registerBuiltinFunction("ui_success_alert", success_alert_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 48. تنبيه_تحذير / warning_alert
    // ═══════════════════════════════════════════════════════════════════
    auto warning_alert_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string msg = args.empty() ? "\xd8\xaa\xd8\xad\xd8\xb0\xd9\x8a\xd8\xb1" : args[0]->toString();
        int al = sad::ui::widget_alert(msg, "warning");
        _applyProps(al, args, 1);
        return std::make_shared<Data::Value>(al);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xaa\xd9\x86\xd8\xa8\xd9\x8a\xd9\x87_\xd8\xaa\xd8\xad\xd8\xb0\xd9\x8a\xd8\xb1", warning_alert_func); // تنبيه_تحذير
    interpreter.getFunctionManager().registerBuiltinFunction("ui_warning_alert", warning_alert_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 49. تنبيه_خطا / error_alert
    // ═══════════════════════════════════════════════════════════════════
    auto error_alert_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string msg = args.empty() ? "\xd8\xae\xd8\xb7\xd8\xa3" : args[0]->toString();
        int al = sad::ui::widget_alert(msg, "error");
        _applyProps(al, args, 1);
        return std::make_shared<Data::Value>(al);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xaa\xd9\x86\xd8\xa8\xd9\x8a\xd9\x87_\xd8\xae\xd8\xb7\xd8\xa7", error_alert_func); // تنبيه_خطا
    interpreter.getFunctionManager().registerBuiltinFunction("ui_error_alert", error_alert_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 50. حقل_مخصص / custom_field — حقل نص مع تسمية
    // ═══════════════════════════════════════════════════════════════════
    auto custom_field_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string label = args.size() > 0 ? args[0]->toString() : "";
        std::string hint = args.size() > 1 ? args[1]->toString() : "";
        int col = sad::ui::widget_column();
        sad::ui::widgetSetProp(col, "spacing", 4.0);
        if (!label.empty()) {
            int lbl = sad::ui::widget_label(label);
            sad::ui::widgetSetProp(lbl, "fontSize", 14.0);
            sad::ui::widgetSetProp(lbl, "fontWeight", "bold");
            sad::ui::widget_add_child(col, lbl);
        }
        int fld = sad::ui::widget_textfield(hint);
        sad::ui::widget_add_child(col, fld);
        _applyProps(col, args, 2);
        return std::make_shared<Data::Value>(col);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xad\xd9\x82\xd9\x84_\xd9\x85\xd8\xae\xd8\xb5\xd8\xb5", custom_field_func); // حقل_مخصص
    interpreter.getFunctionManager().registerBuiltinFunction("ui_custom_field", custom_field_func);

#endif // HAS_GRAPHICS
} // registerBuiltinsPart16

} // namespace Interpreter
} // namespace Sad
