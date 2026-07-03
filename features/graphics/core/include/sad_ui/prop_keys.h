/**
 * ═══════════════════════════════════════════════════════════════════
 * ملف: prop_keys.h
 * المسار: features/graphics/core/include/sad_ui/prop_keys.h
 * ═══════════════════════════════════════════════════════════════════
 *
 * (AR) مصدر الحقيقة الوحيد لأسماء مفاتيح خصائص عناصر الواجهة (IR keys).
 *   يستهلكه ثلاثةُ منتِجين/مستهلِكين كي لا تنحرف المفاتيح بينهم انحرافًا
 *   صامتًا (كان انحرافُها يُنتج زرًّا فارغًا وزرًّا عائمًا بلا أيقونة):
 *     1. مصنع العناصر في المفسّر (widget_builtins.cpp) — يَكتب المفتاح.
 *     2. مُرسِّم سطح المكتب (backends/desktop/renderer*.cpp) — يقرؤه.
 *     3. مُصرِّفات الويب/أندرويد/iOS/macOS — تقرؤه.
 *   القاعدة: ممنوع سلاسلُ مفاتيحَ حرفيّةٌ مباشرةٌ في أيٍّ من هؤلاء؛ استعمل
 *   هذه الثوابت حصرًا (قاعدة المشروع «لا literals منطقيّة مباشرة»).
 *
 * (EN) Single source of truth for UI IR property-key names, shared by the
 *   interpreter widget factory, the desktop renderer, and every codegen
 *   backend so the keys can never silently diverge.
 * ═══════════════════════════════════════════════════════════════════
 */

#pragma once

namespace sad
{
    namespace ui
    {
        namespace props
        {
            // (AR) نصّ العنوان القانونيّ للزرّ/الشريط/التبويب/الحوار — «عنوان».
            inline constexpr const char *TITLE = "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86";
            // (AR) محتوى النصّ القانونيّ لعنصر نص_عنصر ونظائره — «محتوى».
            inline constexpr const char *CONTENT = "\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89";
            // (AR) اسمٌ بديلٌ شائعٌ للنصّ — «نص».
            inline constexpr const char *TEXT = "\xd9\x86\xd8\xb5";
            // (AR) رمز/أيقونة الزرّ العائم وزرّ الأيقونة — «أيقونة» (بهمزة على الألف).
            inline constexpr const char *ICON = "\xd8\xa3\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9";
            // (AR) مصدر الصورة — «مصدر».
            inline constexpr const char *SOURCE = "\xd9\x85\xd8\xb5\xd8\xaf\xd8\xb1";
            // (AR) اسم الأيقونة/الرمز — «اسم».
            inline constexpr const char *NAME = "\xd8\xa7\xd8\xb3\xd9\x85";
            // (AR) مفتاح الوسيط الأساسيّ اللاتينيّ الذي يُنتجه مسار UINode
            //   (ir_builder: primaryArg ⇒ "text") — بديلٌ احتياطيّ لا قانونيّ.
            inline constexpr const char *TEXT_LATIN = "text";
        } // namespace props
    } // namespace ui
} // namespace sad
