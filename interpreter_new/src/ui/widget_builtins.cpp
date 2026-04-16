/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: widget_builtins.cpp
 * المسار: interpreter_new/src/ui/widget_builtins.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * الوصف بالعربية:
 * ───────────────
 * تنفيذ دوال بناء عناصر الواجهة المدمجة — الحزمة الأساسية (~30 عنصر).
 *
 * عند `استورد رسومات` في كود ص، تُسجَّل هذه الدوال في FunctionManager.
 * كل دالة:
 *   1. تُنشئ WidgetBuilder من النوع المناسب (UINodeType)
 *   2. تُعيّن الخصائص الأولية من الوسائط (محتوى النص، عنوان الزر، ...)
 *   3. تُرجع shared_ptr<WidgetBuilder> ملفوفاً في Value::OBJECT
 *
 * مثال المسار:
 *   نص("مرحبا") في كود ص
 *     → Parser: FunctionCallNode("نص", ["مرحبا"])
 *     → Interpreter: calls widget_text_fn
 *     → widget_text_fn: WidgetBuilder(Text) + setProperty("محتوى", "مرحبا")
 *     → return Value(shared_ptr<WidgetBuilder>)
 *
 * English Description:
 * ────────────────────
 * Widget builder builtin functions — core package (~30 widgets).
 * Registered via `استورد رسومات`. Each creates WidgetBuilder(IRNode).
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "widget_builtins.h"
#include "widget_builder.h"
#include "builtins.h"
#include "interpreter_core.h"
#include "sad_ui/types.h"
#include "sad_type_system.h"

#include <memory>
#include <string>
#include <vector>

namespace Sad
{
    namespace Interpreter
    {

// ═══════════════════════════════════════════════════════════════════════════════
// ماكرو مساعد: إنشاء دالة بناء عنصر بسيط (بدون وسائط إلزامية)
// ═══════════════════════════════════════════════════════════════════════════════
// (AR) ينشئ WidgetBuilder من النوع المحدد ويُرجعه
// (EN) Creates WidgetBuilder of specified type and returns it
#define MAKE_SIMPLE_WIDGET_FN(nodeType)                       \
    [](const std::vector<std::shared_ptr<Data::Value>> &args) \
        -> std::shared_ptr<Data::Value> {                                              \
        auto builder = std::make_shared<WidgetBuilder>(sad::ui::UINodeType::nodeType);  \
        /* إضافة جميع الوسائط كأبناء إذا كانت WidgetBuilder */                        \
        for (auto &arg : args) {                                                       \
            if (arg && arg->getKind() == Types::SadTypeKind::Class) {                    \
                auto obj = arg->toObject();                                            \
                if (isWidgetBuilder(obj.get())) { builder->addChildBuilder(std::static_pointer_cast<WidgetBuilder>(obj)); }    \
            }                                                                          \
        }                                                                              \
        return std::make_shared<Data::Value>(                                           \
            std::static_pointer_cast<Data::ObjectInstance>(builder)); }

// ═══════════════════════════════════════════════════════════════════════════════
// ماكرو مساعد: إنشاء دالة بناء عنصر مع خاصية أولى
// ═══════════════════════════════════════════════════════════════════════════════
// (AR) ينشئ WidgetBuilder ويعيّن خاصية من الوسيط الأول إن وُجد
// (EN) Creates WidgetBuilder and sets first arg as property if present
#define MAKE_WIDGET_WITH_PROP_FN(nodeType, propName)          \
    [](const std::vector<std::shared_ptr<Data::Value>> &args) \
        -> std::shared_ptr<Data::Value> {                                              \
        auto builder = std::make_shared<WidgetBuilder>(sad::ui::UINodeType::nodeType);  \
        if (!args.empty() && args[0]) {                                                \
            builder->setIRPropertyFromValue(propName, *args[0]);                        \
        }                                                                              \
        /* إضافة الوسائط المتبقية كأبناء إذا كانت WidgetBuilder */                     \
        for (size_t i = 1; i < args.size(); ++i) {                                     \
            if (args[i] && args[i]->getKind() == Types::SadTypeKind::Class) {            \
                auto obj = args[i]->toObject();                                        \
                if (isWidgetBuilder(obj.get())) { builder->addChildBuilder(std::static_pointer_cast<WidgetBuilder>(obj)); }    \
            }                                                                          \
        }                                                                              \
        return std::make_shared<Data::Value>(                                           \
            std::static_pointer_cast<Data::ObjectInstance>(builder)); }

        void registerWidgetBuiltins(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // ═════════════════════════════════════════════════════════════════════════
            // 1. عناصر العرض الأساسية (Display Components)
            // ═════════════════════════════════════════════════════════════════════════

            // نص("محتوى") — عنصر نصي
            auto text_fn = MAKE_WIDGET_WITH_PROP_FN(Text, "\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89"); // محتوى
            fm.registerBuiltinFunction("\xd9\x86\xd8\xb5_\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1", text_fn);  // نص_عنصر (لتجنب التصادم مع نص النوع)
            // (AR) ملاحظة: سيتم تسجيل "نص" كدالة بناء أيضاً لكن بأولوية أقل
            //      Parser يميّز بين نص(تعبير) ← استدعاء دالة  و  نص اسم ← تصريح متغير

            // صورة("مصدر") — عنصر صورة
            auto image_fn = MAKE_WIDGET_WITH_PROP_FN(Image, "\xd9\x85\xd8\xb5\xd8\xaf\xd8\xb1"); // مصدر
            fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xb1\xd8\xa9", image_fn);            // صورة

            // أيقونة("اسم") — أيقونة متجهة
            auto icon_fn = MAKE_WIDGET_WITH_PROP_FN(Icon, "\xd8\xa7\xd8\xb3\xd9\x85");               // اسم
            fm.registerBuiltinFunction("\xd8\xa3\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9", icon_fn); // أيقونة

            // ═════════════════════════════════════════════════════════════════════════
            // 2. مكونات الإدخال (Input Components)
            // ═════════════════════════════════════════════════════════════════════════

            // زر("عنوان") — زر قابل للنقر
            auto button_fn = MAKE_WIDGET_WITH_PROP_FN(Button, "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86"); // عنوان
            fm.registerBuiltinFunction("\xd8\xb2\xd8\xb1", button_fn);                                     // زر

            // زر_عائم(أيقونة: "إضافة") — زر عائم دائري (FAB)
            auto fab_fn = MAKE_WIDGET_WITH_PROP_FN(FAB, "\xd8\xa3\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9"); // أيقونة
            fm.registerBuiltinFunction("\xd8\xb2\xd8\xb1_\xd8\xb9\xd8\xa7\xd8\xa6\xd9\x85", fab_fn);         // زر_عائم

            // زر_نصي("نص") — زر بنص فقط بدون خلفية
            auto text_btn_fn = MAKE_WIDGET_WITH_PROP_FN(Button, "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86"); // عنوان
            fm.registerBuiltinFunction("\xd8\xb2\xd8\xb1_\xd9\x86\xd8\xb5\xd9\x8a", text_btn_fn);            // زر_نصي

            // زر_أيقونة("حذف") — زر بأيقونة فقط
            auto icon_btn_fn = MAKE_WIDGET_WITH_PROP_FN(Button, "\xd8\xa3\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9");      // أيقونة
            fm.registerBuiltinFunction("\xd8\xb2\xd8\xb1_\xd8\xa3\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9", icon_btn_fn); // زر_أيقونة

            // حقل_نص(تلميح: "أدخل...") — حقل إدخال نصي
            auto textfield_fn = MAKE_WIDGET_WITH_PROP_FN(TextField, "\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad"); // تلميح
            fm.registerBuiltinFunction("\xd8\xad\xd9\x82\xd9\x84_\xd9\x86\xd8\xb5", textfield_fn);               // حقل_نص

            // مفتاح(قيمة) — مفتاح تشغيل/إيقاف
            auto toggle_fn = MAKE_WIDGET_WITH_PROP_FN(Toggle, "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9"); // قيمة
            fm.registerBuiltinFunction("\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad", toggle_fn);     // مفتاح

            // خانة_اختيار(قيمة) — خانة اختيار
            auto checkbox_fn = MAKE_WIDGET_WITH_PROP_FN(Checkbox, "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9");                                    // قيمة
            fm.registerBuiltinFunction("\xd8\xae\xd8\xa7\xd9\x86\xd8\xa9_\xd8\xa7\xd8\xae\xd8\xaa\xd9\x8a\xd8\xa7\xd8\xb1", checkbox_fn); // خانة_اختيار

            // منزلق(قيمة) — شريط قيمة قابل للسحب
            auto slider_fn = MAKE_WIDGET_WITH_PROP_FN(Slider, "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9"); // قيمة
            fm.registerBuiltinFunction("\xd9\x85\xd9\x86\xd8\xb2\xd9\x84\xd9\x82", slider_fn);     // منزلق

            // ═════════════════════════════════════════════════════════════════════════
            // 3. مكونات التخطيط (Layout Components)
            // ═════════════════════════════════════════════════════════════════════════

            // عمود — يرتب الأبناء عمودياً
            auto column_fn = MAKE_SIMPLE_WIDGET_FN(Column);
            fm.registerBuiltinFunction("\xd8\xb9\xd9\x85\xd9\x88\xd8\xaf", column_fn); // عمود

            // صف — يرتب الأبناء أفقياً
            auto row_fn = MAKE_SIMPLE_WIDGET_FN(Row);
            fm.registerBuiltinFunction("\xd8\xb5\xd9\x81", row_fn); // صف

            // رصة — يكدّس الأبناء فوق بعضهم
            auto stack_fn = MAKE_SIMPLE_WIDGET_FN(Stack);
            fm.registerBuiltinFunction("\xd8\xb1\xd8\xb5\xd8\xa9", stack_fn); // رصة

            // شبكة — تخطيط شبكي
            auto grid_fn = MAKE_SIMPLE_WIDGET_FN(Grid);
            fm.registerBuiltinFunction("\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9", grid_fn); // شبكة

            // وسط — يضع الابن في المنتصف
            auto center_fn = MAKE_SIMPLE_WIDGET_FN(Center);
            fm.registerBuiltinFunction("\xd9\x88\xd8\xb3\xd8\xb7", center_fn); // وسط

            // حشوة — يضيف حشوة حول الابن
            auto padding_fn = MAKE_SIMPLE_WIDGET_FN(Padding);
            fm.registerBuiltinFunction("\xd8\xad\xd8\xb4\xd9\x88\xd8\xa9", padding_fn); // حشوة

            // محاذاة — يضع الابن في موضع محدد
            auto align_fn = MAKE_SIMPLE_WIDGET_FN(Align);
            fm.registerBuiltinFunction("\xd9\x85\xd8\xad\xd8\xa7\xd8\xb0\xd8\xa7\xd8\xa9", align_fn); // محاذاة
            fm.registerBuiltinFunction("\xd9\x85\xd8\xad\xd8\xa7\xd8\xb0\xd9\x8a", align_fn);         // محاذي

            // موسع — يملأ المساحة المتاحة
            auto expanded_fn = MAKE_SIMPLE_WIDGET_FN(Expanded);
            fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd8\xb3\xd8\xb9", expanded_fn);         // موسع
            fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd8\xb3\xd9\x91\xd8\xb9", expanded_fn); // موسّع

            // مرن — حجم مرن داخل Row/Column
            auto flexible_fn = MAKE_SIMPLE_WIDGET_FN(Flexible);
            fm.registerBuiltinFunction("\xd9\x85\xd8\xb1\xd9\x86", flexible_fn); // مرن

            // مقاس(عرض, ارتفاع) — صندوق بأبعاد ثابتة
            auto sizedbox_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args)
                -> std::shared_ptr<Data::Value>
            {
                auto builder = std::make_shared<WidgetBuilder>(sad::ui::UINodeType::SizedBox);
                // (AR) معالجة وسائط عرض وارتفاع
                if (args.size() >= 1 && args[0])
                {
                    builder->setIRPropertyFromValue("\xd8\xb9\xd8\xb1\xd8\xb6", *args[0]); // عرض
                }
                if (args.size() >= 2 && args[1])
                {
                    builder->setIRPropertyFromValue("\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9", *args[1]); // ارتفاع
                }
                return std::make_shared<Data::Value>(
                    std::static_pointer_cast<Data::ObjectInstance>(builder));
            };
            fm.registerBuiltinFunction("\xd9\x85\xd9\x82\xd8\xa7\xd8\xb3", sizedbox_fn);                                  // مقاس
            fm.registerBuiltinFunction("\xd9\x85\xd9\x82\xd8\xa7\xd8\xb3_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf", sizedbox_fn); // مقاس_محدد

            // فاصل — مساحة فارغة
            auto spacer_fn = MAKE_SIMPLE_WIDGET_FN(Spacer);
            fm.registerBuiltinFunction("\xd9\x81\xd8\xa7\xd8\xb5\xd9\x84", spacer_fn); // فاصل

            // فاصل_خط — خط أفقي أو عمودي
            auto divider_fn = MAKE_SIMPLE_WIDGET_FN(Divider);
            fm.registerBuiltinFunction("\xd9\x81\xd8\xa7\xd8\xb5\xd9\x84_\xd8\xae\xd8\xb7", divider_fn); // فاصل_خط

            // التفاف — يلتف للسطر التالي
            auto wrap_fn = MAKE_SIMPLE_WIDGET_FN(Wrap);
            fm.registerBuiltinFunction("\xd8\xa7\xd9\x84\xd8\xaa\xd9\x81\xd8\xa7\xd9\x81", wrap_fn); // التفاف

            // ═════════════════════════════════════════════════════════════════════════
            // 4. الحاويات (Container Components)
            // ═════════════════════════════════════════════════════════════════════════

            // حاوية — حاوية عامة
            auto container_fn = MAKE_SIMPLE_WIDGET_FN(Container);
            fm.registerBuiltinFunction("\xd8\xad\xd8\xa7\xd9\x88\xd9\x8a\xd8\xa9", container_fn); // حاوية

            // بطاقة — بطاقة مرتفعة مع ظل
            auto card_fn = MAKE_SIMPLE_WIDGET_FN(Card);
            fm.registerBuiltinFunction("\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9", card_fn); // بطاقة

            // هيكل — هيكل شاشة كاملة (Scaffold)
            auto scaffold_fn = MAKE_SIMPLE_WIDGET_FN(Scaffold);
            fm.registerBuiltinFunction("\xd9\x87\xd9\x8a\xd9\x83\xd9\x84", scaffold_fn); // هيكل

            // صندوق — صندوق عام بدون زخرفة
            auto box_fn = MAKE_SIMPLE_WIDGET_FN(Box);
            fm.registerBuiltinFunction("\xd8\xb5\xd9\x86\xd8\xaf\xd9\x88\xd9\x82", box_fn); // صندوق

            // عرض_تمرير — حاوية قابلة للتمرير
            auto scrollview_fn = MAKE_SIMPLE_WIDGET_FN(ScrollView);
            fm.registerBuiltinFunction("\xd8\xb9\xd8\xb1\xd8\xb6_\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1", scrollview_fn); // عرض_تمرير

            // ═════════════════════════════════════════════════════════════════════════
            // 5. مكونات الملاحة (Navigation)
            // ═════════════════════════════════════════════════════════════════════════

            // شريط_تطبيق(عنوان: "عنوان") — شريط علوي
            auto appbar_fn = MAKE_WIDGET_WITH_PROP_FN(AppBar, "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86");                      // عنوان
            fm.registerBuiltinFunction("\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82", appbar_fn); // شريط_تطبيق

            // تنقل_سفلي — شريط تنقل سفلي
            auto bottomnav_fn = MAKE_SIMPLE_WIDGET_FN(BottomNav);
            fm.registerBuiltinFunction("\xd8\xaa\xd9\x86\xd9\x82\xd9\x84_\xd8\xb3\xd9\x81\xd9\x84\xd9\x8a", bottomnav_fn); // تنقل_سفلي

            // ═════════════════════════════════════════════════════════════════════════
            // 6. الحوارات والإشعارات (Dialogs & Notifications)
            // ═════════════════════════════════════════════════════════════════════════

            // حوار(عنوان: "...") — نافذة حوار
            auto dialog_fn = MAKE_WIDGET_WITH_PROP_FN(Dialog, "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86"); // عنوان
            fm.registerBuiltinFunction("\xd8\xad\xd9\x88\xd8\xa7\xd8\xb1", dialog_fn);                     // حوار

            // شريط_إشعار(رسالة: "...") — رسالة منبثقة أسفل الشاشة
            auto snackbar_fn = MAKE_WIDGET_WITH_PROP_FN(SnackBar, "\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84\xd8\xa9");                    // رسالة
            fm.registerBuiltinFunction("\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xa5\xd8\xb4\xd8\xb9\xd8\xa7\xd8\xb1", snackbar_fn); // شريط_إشعار

            // تلميح(نص: "...") — تلميح عند التمرير
            auto tooltip_fn = MAKE_WIDGET_WITH_PROP_FN(Tooltip, "\xd9\x86\xd8\xb5");            // نص
            fm.registerBuiltinFunction("\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad", tooltip_fn); // تلميح

            // ═════════════════════════════════════════════════════════════════════════
            // 7. مكونات إضافية من الحزمة الأساسية
            // ═════════════════════════════════════════════════════════════════════════

            // شريط_تقدم(قيمة) — شريط تقدم
            auto progress_fn = MAKE_WIDGET_WITH_PROP_FN(ProgressBar, "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9");                 // قيمة
            fm.registerBuiltinFunction("\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85", progress_fn); // شريط_تقدم

            // عمود_كسول — قائمة عمودية كسولة
            auto lazycolumn_fn = MAKE_SIMPLE_WIDGET_FN(LazyColumn);
            fm.registerBuiltinFunction("\xd8\xb9\xd9\x85\xd9\x88\xd8\xaf_\xd9\x83\xd8\xb3\xd9\x88\xd9\x84", lazycolumn_fn); // عمود_كسول

            // صف_كسول — قائمة أفقية كسولة
            auto lazyrow_fn = MAKE_SIMPLE_WIDGET_FN(LazyRow);
            fm.registerBuiltinFunction("\xd8\xb5\xd9\x81_\xd9\x83\xd8\xb3\xd9\x88\xd9\x84", lazyrow_fn); // صف_كسول

            // قائمة — قائمة عناصر
            auto list_fn = MAKE_SIMPLE_WIDGET_FN(List);
            fm.registerBuiltinFunction("\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9", list_fn); // قائمة

            // منطقة_نص — حقل إدخال متعدد الأسطر
            auto textarea_fn = MAKE_WIDGET_WITH_PROP_FN(TextArea, "\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad");    // تلميح
            fm.registerBuiltinFunction("\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd8\xa9_\xd9\x86\xd8\xb5", textarea_fn); // منطقة_نص

            // درج — لوحة جانبية
            auto drawer_fn = MAKE_SIMPLE_WIDGET_FN(Drawer);
            fm.registerBuiltinFunction("\xd8\xaf\xd8\xb1\xd8\xac", drawer_fn); // درج

            // منطقة_آمنة — يتجنب مناطق النظام
            auto safearea_fn = MAKE_SIMPLE_WIDGET_FN(SafeArea);
            fm.registerBuiltinFunction("\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd8\xa9_\xd8\xa2\xd9\x85\xd9\x86\xd8\xa9", safearea_fn); // منطقة_آمنة

            // سطح — سطح Material
            auto surface_fn = MAKE_SIMPLE_WIDGET_FN(Surface);
            fm.registerBuiltinFunction("\xd8\xb3\xd8\xb7\xd8\xad", surface_fn); // سطح
        }

#undef MAKE_SIMPLE_WIDGET_FN
#undef MAKE_WIDGET_WITH_PROP_FN

    } // namespace Interpreter
} // namespace Sad
