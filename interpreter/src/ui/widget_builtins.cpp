/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: widget_builtins.cpp
 * المسار: interpreter/src/ui/widget_builtins.cpp
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
#include "builtin_registry.h"
#include "sad_ui/types.h"
#include "sad_type_system.h"

#include <memory>
#include <string>
#include <vector>
#include "builtins/builtin_context.h"

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
    [](Sad::Interpreter::BuiltinContext &ctx)                 \
        -> std::shared_ptr<Data::Value> {                                              \
        const auto &args = ctx.args(); (void)args;                                     \
        /* (AR) B-step5b: WidgetBuilder تُخصَّص بـnew وتُدار بـGC */                  \
        auto *builder = new WidgetBuilder(sad::ui::UINodeType::nodeType);              \
        /* إضافة جميع الوسائط كأبناء إذا كانت WidgetBuilder */                        \
        for (auto &arg : args) {                                                       \
            if (arg && arg->getKind() == Types::SadTypeKind::Class) {                    \
                auto *obj = arg->toObject();                                            \
                if (isWidgetBuilder(obj)) { builder->addChildBuilder(static_cast<WidgetBuilder *>(obj)); }    \
            }                                                                          \
        }                                                                              \
        return std::make_shared<Data::Value>(                                           \
            static_cast<Data::ObjectInstance *>(builder)); }

// ═══════════════════════════════════════════════════════════════════════════════
// ماكرو مساعد: إنشاء دالة بناء عنصر مع خاصية أولى
// ═══════════════════════════════════════════════════════════════════════════════
// (AR) ينشئ WidgetBuilder ويعيّن خاصية من الوسيط الأول إن وُجد
// (EN) Creates WidgetBuilder and sets first arg as property if present
#define MAKE_WIDGET_WITH_PROP_FN(nodeType, propName)          \
    [](Sad::Interpreter::BuiltinContext &ctx)                 \
        -> std::shared_ptr<Data::Value> {                                              \
        const auto &args = ctx.args(); (void)args;                                     \
        /* (AR) B-step5b: WidgetBuilder تُخصَّص بـnew وتُدار بـGC عند أول لف في Value */ \
        auto *builder = new WidgetBuilder(sad::ui::UINodeType::nodeType);              \
        if (!args.empty() && args[0]) {                                                \
            builder->setIRPropertyFromValue(propName, *args[0]);                        \
        }                                                                              \
        /* إضافة الوسائط المتبقية كأبناء إذا كانت WidgetBuilder */                     \
        for (size_t i = 1; i < args.size(); ++i) {                                     \
            if (args[i] && args[i]->getKind() == Types::SadTypeKind::Class) {            \
                auto *obj = args[i]->toObject();                                        \
                if (isWidgetBuilder(obj)) { builder->addChildBuilder(static_cast<WidgetBuilder *>(obj)); }    \
            }                                                                          \
        }                                                                              \
        return std::make_shared<Data::Value>(                                           \
            static_cast<Data::ObjectInstance *>(builder)); }

        void registerWidgetBuiltins(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // (AR) اختصار لأسماء ثوابت عناصر الواجهة
            namespace Bw = Builtins::Names::UIWidgets;

            // ═════════════════════════════════════════════════════════════════════════
            // 1. عناصر العرض الأساسية (Display Components)
            // ═════════════════════════════════════════════════════════════════════════

            // نص("محتوى") — عنصر نصي
            auto text_fn = MAKE_WIDGET_WITH_PROP_FN(Text, "\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89"); // محتوى
            fm.registerBuiltinFunction(std::string(Bw::TEXT_WIDGET), text_fn);                         // نص_عنصر
            // (AR) ملاحظة: سيتم تسجيل "نص" كدالة بناء أيضاً لكن بأولوية أقل
            //      Parser يميّز بين نص(تعبير) ← استدعاء دالة  و  نص اسم ← تصريح متغير

            // صورة("مصدر") — عنصر صورة
            auto image_fn = MAKE_WIDGET_WITH_PROP_FN(Image, "\xd9\x85\xd8\xb5\xd8\xaf\xd8\xb1"); // مصدر
            fm.registerBuiltinFunction(std::string(Bw::IMAGE), image_fn);                        // صورة

            // أيقونة("اسم") — أيقونة متجهة
            auto icon_fn = MAKE_WIDGET_WITH_PROP_FN(Icon, "\xd8\xa7\xd8\xb3\xd9\x85"); // اسم
            fm.registerBuiltinFunction(std::string(Bw::ICON), icon_fn);                // أيقونة

            // ═════════════════════════════════════════════════════════════════════════
            // 2. مكونات الإدخال (Input Components)
            // ═════════════════════════════════════════════════════════════════════════

            // زر("عنوان") — زر قابل للنقر
            auto button_fn = MAKE_WIDGET_WITH_PROP_FN(Button, "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86"); // عنوان
            fm.registerBuiltinFunction(std::string(Bw::BUTTON), button_fn);                                // زر

            // زر_عائم(أيقونة: "إضافة") — زر عائم دائري (FAB)
            auto fab_fn = MAKE_WIDGET_WITH_PROP_FN(FAB, "\xd8\xa3\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9"); // أيقونة
            fm.registerBuiltinFunction(std::string(Bw::FAB), fab_fn);                                        // زر_عائم

            // زر_نصي("نص") — زر بنص فقط بدون خلفية
            auto text_btn_fn = MAKE_WIDGET_WITH_PROP_FN(Button, "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86"); // عنوان
            fm.registerBuiltinFunction(std::string(Bw::TEXT_BUTTON), text_btn_fn);                           // زر_نصي

            // زر_أيقونة("حذف") — زر بأيقونة فقط
            auto icon_btn_fn = MAKE_WIDGET_WITH_PROP_FN(Button, "\xd8\xa3\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9"); // أيقونة
            fm.registerBuiltinFunction(std::string(Bw::ICON_BUTTON), icon_btn_fn);                                   // زر_أيقونة

            // حقل_نص(تلميح: "أدخل...") — حقل إدخال نصي
            auto textfield_fn = MAKE_WIDGET_WITH_PROP_FN(TextField, "\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad"); // تلميح
            fm.registerBuiltinFunction(std::string(Bw::TEXT_FIELD), textfield_fn);                               // حقل_نص

            // مفتاح(قيمة) — مفتاح تشغيل/إيقاف
            auto toggle_fn = MAKE_WIDGET_WITH_PROP_FN(Toggle, "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9"); // قيمة
            fm.registerBuiltinFunction(std::string(Bw::TOGGLE), toggle_fn);                        // مفتاح

            // خانة_اختيار(قيمة) — خانة اختيار
            auto checkbox_fn = MAKE_WIDGET_WITH_PROP_FN(Checkbox, "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9"); // قيمة
            fm.registerBuiltinFunction(std::string(Bw::CHECKBOX), checkbox_fn);                        // خانة_اختيار

            // منزلق(قيمة) — شريط قيمة قابل للسحب
            auto slider_fn = MAKE_WIDGET_WITH_PROP_FN(Slider, "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9"); // قيمة
            fm.registerBuiltinFunction(std::string(Bw::SLIDER), slider_fn);                        // منزلق

            // ═════════════════════════════════════════════════════════════════════════
            // 3. مكونات التخطيط (Layout Components)
            // ═════════════════════════════════════════════════════════════════════════

            // عمود — يرتب الأبناء عمودياً
            auto column_fn = MAKE_SIMPLE_WIDGET_FN(Column);
            fm.registerBuiltinFunction(std::string(Bw::COLUMN), column_fn); // عمود

            // صف — يرتب الأبناء أفقياً
            auto row_fn = MAKE_SIMPLE_WIDGET_FN(Row);
            fm.registerBuiltinFunction(std::string(Bw::ROW), row_fn); // صف

            // رصة — يكدّس الأبناء فوق بعضهم
            auto stack_fn = MAKE_SIMPLE_WIDGET_FN(Stack);
            fm.registerBuiltinFunction(std::string(Bw::STACK), stack_fn); // رصة

            // شبكة — تخطيط شبكي
            auto grid_fn = MAKE_SIMPLE_WIDGET_FN(Grid);
            fm.registerBuiltinFunction(std::string(Bw::GRID), grid_fn); // شبكة

            // وسط — يضع الابن في المنتصف
            auto center_fn = MAKE_SIMPLE_WIDGET_FN(Center);
            fm.registerBuiltinFunction(std::string(Bw::CENTER), center_fn); // وسط

            // حشوة — يضيف حشوة حول الابن
            auto padding_fn = MAKE_SIMPLE_WIDGET_FN(Padding);
            fm.registerBuiltinFunction(std::string(Bw::PADDING), padding_fn); // حشوة

            // محاذاة — يضع الابن في موضع محدد
            auto align_fn = MAKE_SIMPLE_WIDGET_FN(Align);
            fm.registerBuiltinFunction(std::string(Bw::ALIGN), align_fn); // محاذاة

            // موسع — يملأ المساحة المتاحة
            auto expanded_fn = MAKE_SIMPLE_WIDGET_FN(Expanded);
            fm.registerBuiltinFunction(std::string(Bw::EXPANDED), expanded_fn); // موسع

            // مرن — حجم مرن داخل Row/Column
            auto flexible_fn = MAKE_SIMPLE_WIDGET_FN(Flexible);
            fm.registerBuiltinFunction(std::string(Bw::FLEXIBLE), flexible_fn); // مرن

            // مقاس(عرض, ارتفاع) — صندوق بأبعاد ثابتة
            auto sizedbox_fn = [](Sad::Interpreter::BuiltinContext &ctx)
                -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                // (AR) B-step5b: WidgetBuilder خام مُدار بـGC
                auto *builder = new WidgetBuilder(sad::ui::UINodeType::SizedBox);
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
                    static_cast<Data::ObjectInstance *>(builder));
            };
            fm.registerBuiltinFunction(std::string(Bw::SIZED_BOX), sizedbox_fn); // مقاس

            // فاصل — مساحة فارغة
            auto spacer_fn = MAKE_SIMPLE_WIDGET_FN(Spacer);
            fm.registerBuiltinFunction(std::string(Bw::SPACER), spacer_fn); // فاصل

            // فاصل_خط — خط أفقي أو عمودي
            auto divider_fn = MAKE_SIMPLE_WIDGET_FN(Divider);
            fm.registerBuiltinFunction(std::string(Bw::DIVIDER), divider_fn); // فاصل_خط

            // التفاف — يلتف للسطر التالي
            auto wrap_fn = MAKE_SIMPLE_WIDGET_FN(Wrap);
            fm.registerBuiltinFunction(std::string(Bw::WRAP), wrap_fn); // التفاف

            // ═════════════════════════════════════════════════════════════════════════
            // 4. الحاويات (Container Components)
            // ═════════════════════════════════════════════════════════════════════════

            // حاوية — حاوية عامة
            auto container_fn = MAKE_SIMPLE_WIDGET_FN(Container);
            fm.registerBuiltinFunction(std::string(Bw::CONTAINER), container_fn); // حاوية

            // بطاقة — بطاقة مرتفعة مع ظل
            auto card_fn = MAKE_SIMPLE_WIDGET_FN(Card);
            fm.registerBuiltinFunction(std::string(Bw::CARD), card_fn); // بطاقة

            // هيكل — هيكل شاشة كاملة (Scaffold)
            auto scaffold_fn = MAKE_SIMPLE_WIDGET_FN(Scaffold);
            fm.registerBuiltinFunction(std::string(Bw::SCAFFOLD), scaffold_fn); // هيكل

            // صندوق — صندوق عام بدون زخرفة
            auto box_fn = MAKE_SIMPLE_WIDGET_FN(Box);
            fm.registerBuiltinFunction(std::string(Bw::BOX), box_fn); // صندوق

            // عرض_تمرير — حاوية قابلة للتمرير
            auto scrollview_fn = MAKE_SIMPLE_WIDGET_FN(ScrollView);
            fm.registerBuiltinFunction(std::string(Bw::SCROLL_VIEW), scrollview_fn); // عرض_تمرير

            // ═════════════════════════════════════════════════════════════════════════
            // 5. مكونات الملاحة (Navigation)
            // ═════════════════════════════════════════════════════════════════════════

            // شريط_تطبيق(عنوان: "عنوان") — شريط علوي
            auto appbar_fn = MAKE_WIDGET_WITH_PROP_FN(AppBar, "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86"); // عنوان
            fm.registerBuiltinFunction(std::string(Bw::APP_BAR), appbar_fn);                               // شريط_تطبيق

            // تنقل_سفلي — شريط تنقل سفلي
            auto bottomnav_fn = MAKE_SIMPLE_WIDGET_FN(BottomNav);
            fm.registerBuiltinFunction(std::string(Bw::BOTTOM_NAV), bottomnav_fn); // تنقل_سفلي

            // ═════════════════════════════════════════════════════════════════════════
            // 6. الحوارات والإشعارات (Dialogs & Notifications)
            // ═════════════════════════════════════════════════════════════════════════

            // حوار(عنوان: "...") — نافذة حوار
            auto dialog_fn = MAKE_WIDGET_WITH_PROP_FN(Dialog, "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86"); // عنوان
            fm.registerBuiltinFunction(std::string(Bw::DIALOG), dialog_fn);                                // حوار

            // شريط_إشعار(رسالة: "...") — رسالة منبثقة أسفل الشاشة
            auto snackbar_fn = MAKE_WIDGET_WITH_PROP_FN(SnackBar, "\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84\xd8\xa9"); // رسالة
            fm.registerBuiltinFunction(std::string(Bw::SNACKBAR), snackbar_fn);                                // شريط_إشعار

            // تلميح(نص: "...") — تلميح عند التمرير
            auto tooltip_fn = MAKE_WIDGET_WITH_PROP_FN(Tooltip, "\xd9\x86\xd8\xb5"); // نص
            fm.registerBuiltinFunction(std::string(Bw::TOOLTIP), tooltip_fn);        // تلميح

            // ═════════════════════════════════════════════════════════════════════════
            // 7. مكونات إضافية من الحزمة الأساسية
            // ═════════════════════════════════════════════════════════════════════════

            // شريط_تقدم(قيمة) — شريط تقدم
            auto progress_fn = MAKE_WIDGET_WITH_PROP_FN(ProgressBar, "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9"); // قيمة
            fm.registerBuiltinFunction(std::string(Bw::PROGRESS), progress_fn);                           // شريط_تقدم

            // عمود_كسول — قائمة عمودية كسولة
            auto lazycolumn_fn = MAKE_SIMPLE_WIDGET_FN(LazyColumn);
            fm.registerBuiltinFunction(std::string(Bw::LAZY_COLUMN), lazycolumn_fn); // عمود_كسول

            // صف_كسول — قائمة أفقية كسولة
            auto lazyrow_fn = MAKE_SIMPLE_WIDGET_FN(LazyRow);
            fm.registerBuiltinFunction(std::string(Bw::LAZY_ROW), lazyrow_fn); // صف_كسول

            // قائمة — قائمة عناصر
            auto list_fn = MAKE_SIMPLE_WIDGET_FN(List);
            fm.registerBuiltinFunction(std::string(Bw::LIST_VIEW), list_fn); // قائمة

            // منطقة_نص — حقل إدخال متعدد الأسطر
            auto textarea_fn = MAKE_WIDGET_WITH_PROP_FN(TextArea, "\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad"); // تلميح
            fm.registerBuiltinFunction(std::string(Bw::TEXT_AREA), textarea_fn);                               // منطقة_نص

            // درج — لوحة جانبية
            auto drawer_fn = MAKE_SIMPLE_WIDGET_FN(Drawer);
            fm.registerBuiltinFunction(std::string(Bw::DRAWER), drawer_fn); // درج

            // منطقة_آمنة — يتجنب مناطق النظام
            auto safearea_fn = MAKE_SIMPLE_WIDGET_FN(SafeArea);
            fm.registerBuiltinFunction(std::string(Bw::SAFE_AREA), safearea_fn); // منطقة_آمنة

            // سطح — سطح Material
            auto surface_fn = MAKE_SIMPLE_WIDGET_FN(Surface);
            fm.registerBuiltinFunction(std::string(Bw::SURFACE), surface_fn); // سطح
        }

#undef MAKE_SIMPLE_WIDGET_FN
#undef MAKE_WIDGET_WITH_PROP_FN

    } // namespace Interpreter
} // namespace Sad
