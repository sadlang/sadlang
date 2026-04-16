/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: ir.h
 * المسار: sad_ui/core/include/sad_ui/ir.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * الوصف:
 * ------
 * التمثيل الوسيط للواجهات (Intermediate Representation — IR).
 *
 * هذه الطبقة تقع بين شجرة UINode والأكواد المولّدة لكل منصة:
 *
 *     كود ص  →  Parser  →  UINode Tree  →  IR  →  {Compose, SwiftUI, SDL2, HTML}
 *
 * لماذا نحتاج IR؟
 * ────────────────
 * 1. **التجريد**: IR يُجرّد الفروقات بين المنصات. بدلاً من أن يعرف كل
 *    مولّد كود عن UINode مباشرة، يتعامل مع IR موحد.
 *
 * 2. **التوجيه الذكي (Smart Routing)**: IR يحتوي على معلومات عن
 *    ما إذا كان العنصر "قياسي" (يوجد مكافئ أصلي) أو "مخصص" (يحتاج Canvas).
 *    هذا هو جوهر الهندسة الهجينة الذكية.
 *
 * 3. **التحسين**: يمكن تطبيق تحسينات على IR قبل توليد الكود:
 *    - دمج المعدّلات المتكررة
 *    - حذف العناصر المخفية
 *    - تبسيط الشجرة
 *
 * بنية IR:
 * ─────────
 * - IRNode: عقدة واحدة في شجرة IR (مشابهة لـ UINode لكن أبسط)
 * - IRProperty: خاصية واحدة لعقدة IR (عرض، لون، نص...)
 * - IREvent: حدث مرتبط بعقدة (نقر، سحب...)
 * - IRStateRef: إشارة لمتغير حالة (ربط تفاعلي)
 * - RenderingHint: تلميح للمولّد (استخدم أصلي أم Canvas)
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_IR_H
#define SAD_UI_IR_H

#include "types.h"

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <optional>
#include <unordered_map>

namespace sad
{
    namespace ui
    {

        // ═══════════════════════════════════════════════════════════════════════════════
        // تلميح العرض (Rendering Hint)
        // ═══════════════════════════════════════════════════════════════════════════════

        /**
         * @brief تلميحات لمولّد الكود عن كيفية عرض العنصر
         *
         * هذا هو قلب نظام التوجيه الذكي:
         * - Native: استخدم Widget الأصلي للمنصة (Button, Text, etc.)
         * - Canvas: ارسم العنصر يدوياً على Canvas
         * - Auto: اتركه للمولّد ليقرر (الافتراضي)
         */
        enum class RenderingHint : uint8_t
        {
            Auto,   ///< المولّد يقرر تلقائياً
            Native, ///< استخدم العنصر الأصلي للمنصة
            Canvas, ///< ارسم يدوياً على Canvas
            Hybrid  ///< جزء أصلي + جزء Canvas (مثل: زر بشكل مخصص)
        };

        // ═══════════════════════════════════════════════════════════════════════════════
        // خاصية IR (IR Property)
        // ═══════════════════════════════════════════════════════════════════════════════

        /**
         * @brief خاصية واحدة لعقدة IR
         *
         * كل خاصية تحتوي على:
         * - اسم (key) — مثل "عرض"، "لون"، "نص"
         * - قيمة — يمكن أن تكون رقم، نص، لون، أو بُعد
         *
         * الخصائص تُستخرج من معدّلات UINode وتُخزن بصيغة أبسط في IR.
         */
        struct IRProperty
        {
            /// القيمة يمكن أن تكون أي نوع من هذه الأنواع
            using Value = std::variant<
                int64_t,    // عدد صحيح
                double,     // عدد عشري
                bool,       // منطقي
                std::string // نص (يشمل ألوان مشفرة، أبعاد، تعبيرات)
                >;

            std::string key; ///< اسم الخاصية
            Value value;     ///< قيمة الخاصية
        };

        // ═══════════════════════════════════════════════════════════════════════════════
        // حدث IR (IR Event)
        // ═══════════════════════════════════════════════════════════════════════════════

        /**
         * @brief حدث مرتبط بعقدة IR
         *
         * الأحداث تمثل التفاعلات (نقر، سحب، ظهور...).
         * كل حدث يحتوي على:
         * - نوعه (onTap, onLongPress...)
         * - التعبير الذي يُنفذ عند حدوث الحدث
         * - (اختياري) متغيرات الحالة التي يُعدّلها
         */
        struct IREvent
        {
            IREventType type = IREventType::Custom;                ///< نوع الحدث (enum موحد)
            std::string expression;                                ///< تعبير الاستدعاء
            std::vector<std::string> modifiedStates;               ///< متغيرات الحالة المتأثرة
            std::string customEventName;                           ///< اسم الحدث المخصص (فقط عندما type == Custom)
            EventPropagation propagation = EventPropagation::None; ///< طور الانتشار

            /// الحصول على اسم الحدث (العربي الأساسي، أو المخصص)
            const std::string &getEventName() const
            {
                if (type == IREventType::Custom)
                {
                    return customEventName;
                }
                return irEventTypeToString(type);
            }
        };

        // ═══════════════════════════════════════════════════════════════════════════════
        // تحريك IR (IR Animation) — بيانات التحريك المرتبطة بعقدة
        // ═══════════════════════════════════════════════════════════════════════════════

        /**
         * @brief (AR) نوع التحريك — يحدد ما يتغير أثناء الحركة
         * @brief (EN) Animation type — specifies what changes during animation
         *
         * أنواع مدعومة:
         *   ظهور (FadeIn)، اختفاء (FadeOut)، تكبير (ScaleUp)، تصغير (ScaleDown)
         *   انزلاق_يمين/يسار/أعلى/أسفل (SlideRight/Left/Up/Down)
         *   دوران (Rotate)، نبض (Pulse)، اهتزاز (Shake)، ارتداد (Bounce)
         *   تلاشي (CrossFade)، مخصص (Custom)
         */
        enum class AnimationType : uint8_t
        {
            FadeIn,     ///< ظهور — من شفاف إلى معتم
            FadeOut,    ///< اختفاء — من معتم إلى شفاف
            ScaleUp,    ///< تكبير — من صغير إلى الحجم الطبيعي
            ScaleDown,  ///< تصغير — من الحجم الطبيعي إلى صغير
            SlideRight, ///< انزلاق_يمين — دخول من اليسار
            SlideLeft,  ///< انزلاق_يسار — دخول من اليمين
            SlideUp,    ///< انزلاق_أعلى — دخول من الأسفل
            SlideDown,  ///< انزلاق_أسفل — دخول من الأعلى
            Rotate,     ///< دوران — تدوير العنصر
            Pulse,      ///< نبض — تكبير وتصغير متكرر
            Shake,      ///< اهتزاز — حركة أفقية سريعة
            Bounce,     ///< ارتداد — نطة لطيفة
            CrossFade,  ///< تلاشي — تبديل سلس بين حالتين
            ZoomIn,     ///< تقريب — تكبير مع ظهور تدريجي
            ZoomOut,    ///< تبعيد — تصغير مع اختفاء تدريجي
            DropIn,     ///< سقوط — نزول من أعلى مع ارتداد
            FlyOut,     ///< طيران — انزلاق للخارج مع اختفاء
            Swing,      ///< تأرجح — حركة بندولية مستمرة
            FlipX,      ///< قلب_أفقي — قلب 180° حول المحور الأفقي (محاكاة 3D عبر scale Y)
            FlipY,      ///< قلب_عمودي — قلب 180° حول المحور العمودي (محاكاة 3D عبر scale X)
            Wobble,     ///< ترنّح — اهتزاز دوراني سريع مثل جرس
            BounceIn,   ///< ارتداد_دخول — انزلاق مع ارتداد مرن على المحور X
            Blink,      ///< وميض — ظهور/اختفاء متكرر سريع
            Stretch,    ///< تمطّي — تمدد مطاطي إلى الحجم الطبيعي
            Explode,    ///< انفجار — تكبير سريع مع اختفاء (خروج انفجاري)
            Spin3D,     ///< دورة_3D — دوران مع تغيّر حجم (محاكاة عمق 3D)
            Custom      ///< مخصص — يُحدد بالخصائص
        };

        /**
         * @brief (AR) منحنى التحريك — يتحكم في سرعة التحريك عبر الزمن
         * @brief (EN) Easing curve — controls animation speed over time
         */
        enum class EasingCurve : uint8_t
        {
            Linear,    ///< خطي — سرعة ثابتة
            EaseIn,    ///< تسارع — بطيء ثم سريع
            EaseOut,   ///< تباطؤ — سريع ثم بطيء
            EaseInOut, ///< تسارع_وتباطؤ — بطيء ثم سريع ثم بطيء
            Bounce,    ///< ارتداد — ارتداد في النهاية
            Elastic,   ///< مرن — تمدد مرن في النهاية
            Spring     ///< نابض — فيزيائي نابضي
        };

        /**
         * @brief (AR) بنية تحريك كاملة مرتبطة بعقدة IR
         * @brief (EN) Complete animation config attached to an IRNode
         *
         * تُخزّن على IRNode وتُقرأ بواسطة SDL backend لتنفيذ الحركة.
         *
         * الاستخدام في كود ص:
         *   زر("اضغط").حرّك("ظهور").مدة(0.5).منحنى("مرونة").تأخير(0.2)
         *   نص("مرحبا").حرّك("انزلاق_يمين").مدة(0.3).تكرار(3)
         */
        struct IRAnimation
        {
            AnimationType type = AnimationType::FadeIn;  ///< نوع التحريك
            EasingCurve easing = EasingCurve::EaseInOut; ///< المنحنى الزمني
            float duration = 0.3f;                       ///< المدة بالثواني
            float delay = 0.0f;                          ///< التأخير قبل البدء
            int repeatCount = 1;                         ///< عدد التكرارات (0 = لانهائي)
            bool autoReverse = false;                    ///< عكس الحركة تلقائياً
            bool playOnAppear = true;                    ///< تشغيل عند الظهور

            /// قيم مخصصة للتحريك المخصص (مثل زاوية الدوران، مسافة الانزلاق)
            float customFromValue = 0.0f;
            float customToValue = 1.0f;
            std::string customProperty; ///< اسم الخاصية المتحركة (للنوع Custom)
        };

        /**
         * @brief (AR) تحويل اسم تحريك عربي/إنجليزي إلى AnimationType
         * @brief (EN) Convert Arabic/English animation name to AnimationType
         */
        inline AnimationType stringToAnimationType(const std::string &name)
        {
            // أسماء عربية
            if (name == "\xd8\xb8\xd9\x87\xd9\x88\xd8\xb1" || name == "fadeIn" || name == "fade_in")
                return AnimationType::FadeIn;
            if (name == "\xd8\xa7\xd8\xae\xd8\xaa\xd9\x81\xd8\xa7\xd8\xa1" || name == "fadeOut" || name == "fade_out")
                return AnimationType::FadeOut;
            if (name == "\xd8\xaa\xd9\x83\xd8\xa8\xd9\x8a\xd8\xb1" || name == "scaleUp" || name == "scale_up")
                return AnimationType::ScaleUp;
            if (name == "\xd8\xaa\xd8\xb5\xd8\xba\xd9\x8a\xd8\xb1" || name == "scaleDown" || name == "scale_down")
                return AnimationType::ScaleDown;
            if (name == "\xd8\xa7\xd9\x86\xd8\xb2\xd9\x84\xd8\xa7\xd9\x82_\xd9\x8a\xd9\x85\xd9\x8a\xd9\x86" || name == "slideRight" || name == "slide_right")
                return AnimationType::SlideRight;
            if (name == "\xd8\xa7\xd9\x86\xd8\xb2\xd9\x84\xd8\xa7\xd9\x82_\xd9\x8a\xd8\xb3\xd8\xa7\xd8\xb1" || name == "slideLeft" || name == "slide_left")
                return AnimationType::SlideLeft;
            if (name == "\xd8\xa7\xd9\x86\xd8\xb2\xd9\x84\xd8\xa7\xd9\x82_\xd8\xa3\xd8\xb9\xd9\x84\xd9\x89" || name == "slideUp" || name == "slide_up")
                return AnimationType::SlideUp;
            if (name == "\xd8\xa7\xd9\x86\xd8\xb2\xd9\x84\xd8\xa7\xd9\x82_\xd8\xa3\xd8\xb3\xd9\x81\xd9\x84" || name == "slideDown" || name == "slide_down")
                return AnimationType::SlideDown;
            if (name == "\xd8\xaf\xd9\x88\xd8\xb1\xd8\xa7\xd9\x86" || name == "rotate")
                return AnimationType::Rotate;
            if (name == "\xd9\x86\xd8\xa8\xd8\xb6" || name == "pulse")
                return AnimationType::Pulse;
            if (name == "\xd8\xa7\xd9\x87\xd8\xaa\xd8\xb2\xd8\xa7\xd8\xb2" || name == "shake")
                return AnimationType::Shake;
            if (name == "\xd8\xa7\xd8\xb1\xd8\xaa\xd8\xaf\xd8\xa7\xd8\xaf" || name == "bounce")
                return AnimationType::Bounce;
            if (name == "\xd8\xaa\xd9\x84\xd8\xa7\xd8\xb4\xd9\x8a" || name == "crossFade" || name == "cross_fade")
                return AnimationType::CrossFade;
            if (name == "\xd8\xaa\xd9\x82\xd8\xb1\xd9\x8a\xd8\xa8" || name == "zoomIn" || name == "zoom_in")
                return AnimationType::ZoomIn;
            if (name == "\xd8\xaa\xd8\xa8\xd8\xb9\xd9\x8a\xd8\xaf" || name == "zoomOut" || name == "zoom_out")
                return AnimationType::ZoomOut;
            if (name == "\xd8\xb3\xd9\x82\xd9\x88\xd8\xb7" || name == "dropIn" || name == "drop_in")
                return AnimationType::DropIn;
            if (name == "\xd8\xb7\xd9\x8a\xd8\xb1\xd8\xa7\xd9\x86" || name == "flyOut" || name == "fly_out")
                return AnimationType::FlyOut;
            if (name == "\xd8\xaa\xd8\xa3\xd8\xb1\xd8\xac\xd8\xad" || name == "swing")
                return AnimationType::Swing;
            // ── الأنواع الجديدة (v4) ──
            if (name == "\xd9\x82\xd9\x84\xd8\xa8_\xd8\xa3\xd9\x81\xd9\x82\xd9\x8a" || name == "flipX" || name == "flip_x")
                return AnimationType::FlipX;
            if (name == "\xd9\x82\xd9\x84\xd8\xa8_\xd8\xb9\xd9\x85\xd9\x88\xd8\xaf\xd9\x8a" || name == "flipY" || name == "flip_y")
                return AnimationType::FlipY;
            if (name == "\xd8\xaa\xd8\xb1\xd9\x86\xd9\x91\xd8\xad" || name == "\xd8\xaa\xd8\xb1\xd9\x86\xd8\xad" || name == "wobble")
                return AnimationType::Wobble;
            if (name == "\xd8\xa7\xd8\xb1\xd8\xaa\xd8\xaf\xd8\xa7\xd8\xaf_\xd8\xaf\xd8\xae\xd9\x88\xd9\x84" || name == "bounceIn" || name == "bounce_in")
                return AnimationType::BounceIn;
            if (name == "\xd9\x88\xd9\x85\xd9\x8a\xd8\xb6" || name == "blink")
                return AnimationType::Blink;
            if (name == "\xd8\xaa\xd9\x85\xd8\xb7\xd9\x91\xd9\x8a" || name == "\xd8\xaa\xd9\x85\xd8\xb7\xd9\x8a" || name == "stretch")
                return AnimationType::Stretch;
            if (name == "\xd8\xa7\xd9\x86\xd9\x81\xd8\xac\xd8\xa7\xd8\xb1" || name == "explode")
                return AnimationType::Explode;
            if (name == "\xd8\xaf\xd9\x88\xd8\xb1\xd8\xa9_3D" || name == "spin3D" || name == "spin_3d" || name == "\xd8\xaf\xd9\x88\xd8\xb1\xd8\xa9_3\xd8\xaf\xd9\x8a")
                return AnimationType::Spin3D;
            return AnimationType::Custom;
        }

        /**
         * @brief (AR) تحويل اسم منحنى عربي/إنجليزي إلى EasingCurve
         * @brief (EN) Convert Arabic/English easing name to EasingCurve
         */
        inline EasingCurve stringToEasingCurve(const std::string &name)
        {
            if (name == "\xd8\xae\xd8\xb7\xd9\x8a" || name == "linear")
                return EasingCurve::Linear;
            if (name == "\xd8\xaa\xd8\xb3\xd8\xa7\xd8\xb1\xd8\xb9" || name == "easeIn" || name == "ease_in")
                return EasingCurve::EaseIn;
            if (name == "\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb7\xd8\xa4" || name == "easeOut" || name == "ease_out")
                return EasingCurve::EaseOut;
            if (name == "\xd8\xaa\xd8\xb3\xd8\xa7\xd8\xb1\xd8\xb9_\xd9\x88\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb7\xd8\xa4" || name == "easeInOut" || name == "ease_in_out")
                return EasingCurve::EaseInOut;
            if (name == "\xd8\xa7\xd8\xb1\xd8\xaa\xd8\xaf\xd8\xa7\xd8\xaf" || name == "bounce")
                return EasingCurve::Bounce;
            if (name == "\xd9\x85\xd8\xb1\xd9\x86" || name == "elastic")
                return EasingCurve::Elastic;
            if (name == "\xd9\x86\xd8\xa7\xd8\xa8\xd8\xb6" || name == "spring")
                return EasingCurve::Spring;
            return EasingCurve::EaseInOut;
        }

        /**
         * @brief (AR) تحويل AnimationType إلى اسم عربي
         * @brief (EN) Convert AnimationType to Arabic name
         */
        inline std::string animationTypeToString(AnimationType type)
        {
            switch (type)
            {
            case AnimationType::FadeIn:
                return "\xd8\xb8\xd9\x87\xd9\x88\xd8\xb1"; // ظهور
            case AnimationType::FadeOut:
                return "\xd8\xa7\xd8\xae\xd8\xaa\xd9\x81\xd8\xa7\xd8\xa1"; // اختفاء
            case AnimationType::ScaleUp:
                return "\xd8\xaa\xd9\x83\xd8\xa8\xd9\x8a\xd8\xb1"; // تكبير
            case AnimationType::ScaleDown:
                return "\xd8\xaa\xd8\xb5\xd8\xba\xd9\x8a\xd8\xb1"; // تصغير
            case AnimationType::SlideRight:
                return "\xd8\xa7\xd9\x86\xd8\xb2\xd9\x84\xd8\xa7\xd9\x82_\xd9\x8a\xd9\x85\xd9\x8a\xd9\x86"; // انزلاق_يمين
            case AnimationType::SlideLeft:
                return "\xd8\xa7\xd9\x86\xd8\xb2\xd9\x84\xd8\xa7\xd9\x82_\xd9\x8a\xd8\xb3\xd8\xa7\xd8\xb1"; // انزلاق_يسار
            case AnimationType::SlideUp:
                return "\xd8\xa7\xd9\x86\xd8\xb2\xd9\x84\xd8\xa7\xd9\x82_\xd8\xa3\xd8\xb9\xd9\x84\xd9\x89"; // انزلاق_أعلى
            case AnimationType::SlideDown:
                return "\xd8\xa7\xd9\x86\xd8\xb2\xd9\x84\xd8\xa7\xd9\x82_\xd8\xa3\xd8\xb3\xd9\x81\xd9\x84"; // انزلاق_أسفل
            case AnimationType::Rotate:
                return "\xd8\xaf\xd9\x88\xd8\xb1\xd8\xa7\xd9\x86"; // دوران
            case AnimationType::Pulse:
                return "\xd9\x86\xd8\xa8\xd8\xb6"; // نبض
            case AnimationType::Shake:
                return "\xd8\xa7\xd9\x87\xd8\xaa\xd8\xb2\xd8\xa7\xd8\xb2"; // اهتزاز
            case AnimationType::Bounce:
                return "\xd8\xa7\xd8\xb1\xd8\xaa\xd8\xaf\xd8\xa7\xd8\xaf"; // ارتداد
            case AnimationType::CrossFade:
                return "\xd8\xaa\xd9\x84\xd8\xa7\xd8\xb4\xd9\x8a"; // تلاشي
            case AnimationType::ZoomIn:
                return "\xd8\xaa\xd9\x82\xd8\xb1\xd9\x8a\xd8\xa8"; // تقريب
            case AnimationType::ZoomOut:
                return "\xd8\xaa\xd8\xa8\xd8\xb9\xd9\x8a\xd8\xaf"; // تبعيد
            case AnimationType::DropIn:
                return "\xd8\xb3\xd9\x82\xd9\x88\xd8\xb7"; // سقوط
            case AnimationType::FlyOut:
                return "\xd8\xb7\xd9\x8a\xd8\xb1\xd8\xa7\xd9\x86"; // طيران
            case AnimationType::Swing:
                return "\xd8\xaa\xd8\xa3\xd8\xb1\xd8\xac\xd8\xad"; // تأرجح
            case AnimationType::FlipX:
                return "\xd9\x82\xd9\x84\xd8\xa8_\xd8\xa3\xd9\x81\xd9\x82\xd9\x8a"; // قلب_أفقي
            case AnimationType::FlipY:
                return "\xd9\x82\xd9\x84\xd8\xa8_\xd8\xb9\xd9\x85\xd9\x88\xd8\xaf\xd9\x8a"; // قلب_عمودي
            case AnimationType::Wobble:
                return "\xd8\xaa\xd8\xb1\xd9\x86\xd9\x91\xd8\xad"; // ترنّح
            case AnimationType::BounceIn:
                return "\xd8\xa7\xd8\xb1\xd8\xaa\xd8\xaf\xd8\xa7\xd8\xaf_\xd8\xaf\xd8\xae\xd9\x88\xd9\x84"; // ارتداد_دخول
            case AnimationType::Blink:
                return "\xd9\x88\xd9\x85\xd9\x8a\xd8\xb6"; // وميض
            case AnimationType::Stretch:
                return "\xd8\xaa\xd9\x85\xd8\xb7\xd9\x91\xd9\x8a"; // تمطّي
            case AnimationType::Explode:
                return "\xd8\xa7\xd9\x86\xd9\x81\xd8\xac\xd8\xa7\xd8\xb1"; // انفجار
            case AnimationType::Spin3D:
                return "\xd8\xaf\xd9\x88\xd8\xb1\xd8\xa9_3D"; // دورة_3D
            case AnimationType::Custom:
                return "\xd9\x85\xd8\xae\xd8\xb5\xd8\xb5"; // مخصص
            }
            return "\xd9\x85\xd8\xae\xd8\xb5\xd8\xb5"; // مخصص
        }

        /**
         * @brief (AR) تحويل EasingCurve إلى اسم عربي
         * @brief (EN) Convert EasingCurve to Arabic name
         */
        inline std::string easingCurveToString(EasingCurve curve)
        {
            switch (curve)
            {
            case EasingCurve::Linear:
                return "\xd8\xae\xd8\xb7\xd9\x8a"; // خطي
            case EasingCurve::EaseIn:
                return "\xd8\xaa\xd8\xb3\xd8\xa7\xd8\xb1\xd8\xb9"; // تسارع
            case EasingCurve::EaseOut:
                return "\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb7\xd8\xa4"; // تباطؤ
            case EasingCurve::EaseInOut:
                return "\xd8\xaa\xd8\xb3\xd8\xa7\xd8\xb1\xd8\xb9_\xd9\x88\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb7\xd8\xa4"; // تسارع_وتباطؤ
            case EasingCurve::Bounce:
                return "\xd8\xa7\xd8\xb1\xd8\xaa\xd8\xaf\xd8\xa7\xd8\xaf"; // ارتداد
            case EasingCurve::Elastic:
                return "\xd9\x85\xd8\xb1\xd9\x86"; // مرن
            case EasingCurve::Spring:
                return "\xd9\x86\xd8\xa7\xd8\xa8\xd8\xb6"; // نابض
            }
            return "\xd8\xaa\xd8\xb3\xd8\xa7\xd8\xb1\xd8\xb9_\xd9\x88\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb7\xd8\xa4"; // تسارع_وتباطؤ
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // إشارة حالة IR (IR State Reference)
        // ═══════════════════════════════════════════════════════════════════════════════

        /**
         * @brief إشارة لمتغير حالة في عقدة IR
         *
         * عندما يعتمد عنصر واجهة على متغير حالة (@حالة أو @ربط)،
         * يُخزّن ذلك هنا حتى يعرف المولّد كيف يربط العنصر بتحديثات الحالة.
         */
        struct IRStateRef
        {
            std::string stateName;        ///< اسم المتغير
            std::string propertyBinding;  ///< الخاصية المرتبطة (مثل: "نص" في نص(عداد))
            StateBindingType bindingType; ///< نوع الربط
        };

        // ═══════════════════════════════════════════════════════════════════════════════
        // عقدة IR (IRNode) — العقدة الرئيسية في شجرة IR
        // ═══════════════════════════════════════════════════════════════════════════════

        /**
         * @brief عقدة واحدة في شجرة IR
         *
         * هذه نسخة مبسطة ومُحسّنة من UINode مصممة لتوليد الكود.
         * الفرق عن UINode:
         * - المعدّلات مُحلّلة إلى خصائص ثابتة (IRProperty)
         * - الأحداث مفصولة عن الخصائص البصرية
         * - تلميح العرض (RenderingHint) يوجه المولّد
         * - إشارات الحالة محددة بوضوح
         */
        class IRNode : public std::enable_shared_from_this<IRNode>
        {
        public:
            /// إنشاء عقدة IR من نوع محدد
            explicit IRNode(UINodeType type);

            /// إنشاء سريع — يعيد مؤشر ذكي
            static std::shared_ptr<IRNode> create(UINodeType type);

            // ─── الخصائص الأساسية ────────────────────────

            /// نوع العنصر
            UINodeType getType() const { return type_; }

            /// معرّف فريد (من UINode الأصلي)
            const std::string &getId() const { return id_; }
            void setId(const std::string &id) { id_ = id; }

            /// تلميح العرض
            RenderingHint getRenderingHint() const { return hint_; }
            void setRenderingHint(RenderingHint hint) { hint_ = hint; }

            /// المنصة المُستهدفة
            TargetPlatform getTargetPlatform() const { return platform_; }
            void setTargetPlatform(TargetPlatform p) { platform_ = p; }

            // ─── الخصائص (Properties) ──────────────────

            /// إضافة خاصية
            void addProperty(const IRProperty &prop) { properties_.push_back(prop); }

            /// الحصول على كل الخصائص
            const std::vector<IRProperty> &getProperties() const { return properties_; }

            /// البحث عن خاصية بالاسم
            const IRProperty *findProperty(const std::string &key) const;

            /// تعديل أو إضافة خاصية بالاسم
            void setProperty(const std::string &key, IRProperty::Value value)
            {
                for (auto &prop : properties_)
                {
                    if (prop.key == key)
                    {
                        prop.value = value;
                        return;
                    }
                }
                IRProperty newProp;
                newProp.key = key;
                newProp.value = value;
                properties_.push_back(newProp);
            }

            // ─── الأحداث (Events) ──────────────────────

            /// إضافة حدث
            void addEvent(const IREvent &event) { events_.push_back(event); }

            /// الحصول على كل الأحداث
            const std::vector<IREvent> &getEvents() const { return events_; }

            /// الحصول على كل الأحداث (قابل للتعديل — لتحديث معرّفات المعالجات)
            std::vector<IREvent> &getMutableEvents() { return events_; }

            // ─── التحريكات (Animations) ────────────────

            /// إضافة تحريك
            void addAnimation(const IRAnimation &anim) { animations_.push_back(anim); }

            /// الحصول على كل التحريكات
            const std::vector<IRAnimation> &getAnimations() const { return animations_; }

            /// الحصول على كل التحريكات (قابل للتعديل)
            std::vector<IRAnimation> &getMutableAnimations() { return animations_; }

            /// هل يوجد تحريكات؟
            bool hasAnimations() const { return !animations_.empty(); }

            // ─── إشارات الحالة (State References) ──────

            /// إضافة إشارة حالة
            void addStateRef(const IRStateRef &ref) { stateRefs_.push_back(ref); }

            /// الحصول على كل إشارات الحالة
            const std::vector<IRStateRef> &getStateRefs() const { return stateRefs_; }

            // ─── الأبناء (Children) ────────────────────

            /// إضافة عقدة ابن
            void addChild(std::shared_ptr<IRNode> child)
            {
                child->parent_ = shared_from_this();
                children_.push_back(std::move(child));
            }

            /// الحصول على كل الأبناء
            const std::vector<std::shared_ptr<IRNode>> &getChildren() const { return children_; }

            /// عدد الأبناء
            size_t childCount() const { return children_.size(); }

            // ─── العنصر الأب (Parent) ───────────────────

            /// الحصول على الأب (قد يكون nullptr)
            std::shared_ptr<IRNode> getParent() const { return parent_.lock(); }

            /// هل لهذه العقدة أب؟
            bool hasParent() const { return !parent_.expired(); }

            /// بناء مسار الأجداد من هذه العقدة صعوداً إلى الجذر
            std::vector<IRNode *> getAncestorPath() const
            {
                std::vector<IRNode *> path;
                auto p = parent_.lock();
                while (p)
                {
                    path.push_back(p.get());
                    p = p->parent_.lock();
                }
                return path;
            }

            // ─── التصحيح ────────────────────────────────

            /// طباعة شجرة IR بصيغة نصية
            std::string dump(int indent = 0) const;

        private:
            UINodeType type_;                               ///< نوع العنصر
            std::string id_;                                ///< معرّف فريد
            RenderingHint hint_ = RenderingHint::Auto;      ///< تلميح العرض
            TargetPlatform platform_ = TargetPlatform::All; ///< المنصة المستهدفة
            std::vector<IRProperty> properties_;            ///< الخصائص
            std::vector<IREvent> events_;                   ///< الأحداث
            std::vector<IRAnimation> animations_;           ///< التحريكات
            std::vector<IRStateRef> stateRefs_;             ///< إشارات الحالة
            std::vector<std::shared_ptr<IRNode>> children_; ///< الأبناء
            std::weak_ptr<IRNode> parent_;                  ///< الأب (weak لتجنب دورات المراجع)
        };

        // ═══════════════════════════════════════════════════════════════════════════════
        // وحدة IR (IR Module) — يمثل ملف أو شاشة كاملة
        // ═══════════════════════════════════════════════════════════════════════════════

        /**
         * @brief وحدة IR تمثل ملف واحد أو شاشة كاملة
         *
         * تحتوي على:
         * - قائمة تعريفات الواجهات (مثل: واجهة شاشة_رئيسية { ... })
         * - متغيرات الحالة العامة
         * - اسم الوحدة
         */
        struct IRModule
        {
            /// اسم الوحدة (عادة اسم الملف)
            std::string name;

            /// الشجرة الجذرية لـ IR
            std::shared_ptr<IRNode> root;

            /// تعريفات الحالة (اسم → نوع)
            std::unordered_map<std::string, std::string> stateDefinitions;

            /// ملف المصدر
            std::string sourceFile;
        };

    } // namespace ui
} // namespace sad

#endif // SAD_UI_IR_H
