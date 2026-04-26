/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: widget_builtins.h
 * المسار: interpreter/src/ui/widget_builtins.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * الوصف بالعربية:
 * ───────────────
 * تصريحات دوال بناء عناصر الواجهة المدمجة.
 *
 * هذه الدوال تُسجَّل عندما يكتب المبرمج `استورد رسومات` في كود ص.
 * كل دالة تنشئ WidgetBuilder يغلّف IRNode من النوع المناسب.
 *
 * المجموعة الأساسية (~30 عنصر):
 *   عناصر العرض:     نص، صورة، أيقونة
 *   الأزرار:         زر، زر_نصي، زر_أيقونة، زر_عائم
 *   التخطيط:         عمود، صف، رصة، وسط، حشوة، محاذاة، موسع، مرن، مقاس، فاصل
 *   الحاويات:        حاوية، بطاقة، هيكل، صندوق، عرض_تمرير
 *   الإدخال:         حقل_نص، مفتاح، خانة_اختيار
 *   الملاحة:         شريط_تطبيق، تنقل_سفلي
 *   الحوارات:        حوار، شريط_إشعار، تلميح
 *
 * English Description:
 * ────────────────────
 * Declarations for widget builder builtin functions.
 * Registered on `استورد رسومات`. Each creates WidgetBuilder wrapping IRNode.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_INTERPRETER_WIDGET_BUILTINS_H
#define SAD_INTERPRETER_WIDGET_BUILTINS_H

namespace Sad
{
    namespace Interpreter
    {

        class Interpreter;

        /**
         * @brief (AR) تسجيل دوال بناء عناصر الواجهة المدمجة (~30 دالة)
         * @brief (EN) Register widget builder builtin functions (~30 functions)
         *
         * تُسجَّل عند `استورد رسومات` عبر BuiltinModuleRegistry.
         * كل دالة تنشئ WidgetBuilder(IRNode) وتُرجعه كقيمة Value::OBJECT.
         */
        void registerWidgetBuiltins(Interpreter &interpreter);

    } // namespace Interpreter
} // namespace Sad

#endif // SAD_INTERPRETER_WIDGET_BUILTINS_H
