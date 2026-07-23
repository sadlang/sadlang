/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: app_runner.h
 * المسار: features/graphics/backends/freestanding/include/sad_ui/freestanding/app_runner.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @brief (AR) حلقة تشغيل تطبيق SadUI في الوضع الحرّ (fb0 + evdev) — منزل واحد
 *             لآليّة الحلقة يتشاركه جسرُ وقت التشغيل (sad_app_run) وأداةُ سطح
 *             المكتب (sad_desktop). يفتح جهاز الإطار والإدخال، يبني الشجرة عبر
 *             ردّ نداء، ويدير حلقة العرض «عند التغيّر» + الإدخال، ويُرسِل الأحداث.
 * @brief (EN) SadUI freestanding (fb0 + evdev) application loop — the single home
 *             of the loop mechanics shared by the runtime bridge (sad_app_run)
 *             and the desktop tool (sad_desktop). Opens the framebuffer/input,
 *             builds the tree via a callback, runs the invalidate-driven render
 *             + input loop, and dispatches events.
 *
 * القرار المعماريّ: آليّة الحلقة (فتح fb0/evdev، توصيل معالِج الفأرة، الإطار
 * الأوّل، حلقة العرض) تعيش هنا مرّةً واحدة؛ سياسة كلّ مستهلك (بناء الشجرة،
 * تحديث الساعة، إطلاق تطبيق، علامات الاختبار، الخروج) تُمرَّر عبر ردود نداء —
 * فلا تكرار للمنطق ويبقى الجسران رفيعين.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_FREESTANDING_APP_RUNNER_H
#define SAD_UI_FREESTANDING_APP_RUNNER_H

#ifdef __linux__

#include "sad_ui/freestanding/renderer.h"
#include "sad_ui/freestanding/linuxfb.h"
#include "sad_ui/freestanding/evdev_input.h"
#include "sad_ui/ir.h"
#include "sad_ui/keyboard_processor.h" // UnifiedKeyCode

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace sad
{
    namespace ui
    {
        namespace freestanding
        {

            /// (AR) خطوة استقصاء الحلقة الدائمة (evdev + فحص السياسة) بالمِلّي ثانية.
            ///      الرسم لا يقع إلّا عند invalidate (رسم عند التغيّر — لا كلّ دورة).
            inline constexpr int APP_LOOP_POLL_INTERVAL_MS = 50;

            /**
             * @brief (AR) سياق يُمرَّر لردود نداء الحلقة: يمنح سياسةَ المستهلك وصولًا
             *        للنافذة ومصدر الإدخال الحيّين دون تكرار آليّة الحلقة.
             */
            struct AppLoopContext
            {
                FreestandingWindow &window;      ///< النافذة الحيّة (invalidate/runOneFrame/setContent)
                evdev::EvdevInputSource &input;  ///< مصدر الإدخال (drainPending عند الحاجة)
                uint32_t width = 0;              ///< عرض الشاشة الفعليّ
                uint32_t height = 0;             ///< ارتفاع الشاشة الفعليّ
            };

            /**
             * @brief (AR) إعداد تشغيل تطبيق حرّ — كلّ ما تختلف فيه السياسة بين مستهلك
             *        وآخر يُمرَّر هنا كردود نداء؛ آليّة الحلقة نفسها في runFreestandingApp.
             */
            struct FreestandingAppConfig
            {
                /// (AR) مسار خطّ PSF بأشكال العرض العربيّة (اختياريّ — سقوط ناعم للمدمج).
                std::string fontPath;

                /// (AR) مسار جهاز الإطار (افتراضيًّا /dev/fb0).
                std::string devicePath = linuxfb::DEFAULT_FB_DEVICE_PATH;

                /**
                 * @brief (AR) نتيجة تحميل خطّ العرض (اختياريّ). loaded=true ⇒ detail
                 *        هو المسار المحمَّل؛ false ⇒ detail رسالة الفشل (نبقى على المدمج).
                 *        يستعمله سطح المكتب لعلامتَي DESKTOP_FONT_LOADED/FALLBACK.
                 */
                std::function<void(bool loaded, const std::string &detail)> onFontResult;

                /**
                 * @brief (AR) يبني (أو يُرجع) شجرة الجذر بعد معرفة أبعاد الشاشة الفعليّة.
                 *        إلزاميّ؛ إرجاع nullptr يُفشل التشغيل برسالة خطأ. جسرُ وقت
                 *        التشغيل يُرجع جذرًا جاهزًا؛ سطحُ المكتب يبني شجرته بالأبعاد.
                 */
                std::function<std::shared_ptr<IRNode>(uint32_t w, uint32_t h)> buildRoot;

                /**
                 * @brief (AR) يُستدعى بعد عرض الإطار الأوّل (مثلًا: علامات OK/GLYPHS).
                 */
                std::function<void(const AppLoopContext &)> onReady;

                /**
                 * @brief (AR) ردّ نداء لكلّ دورة (بعد poll، قبل runOneFrame). يُرجع false
                 *        لإيقاف الحلقة. يُستعمل للمحتوى الحيّ (الساعة) والإجراءات
                 *        المؤجّلة (إطلاق تطبيق). غيابه ⇒ الحلقة تستمرّ دائمًا.
                 */
                std::function<bool(AppLoopContext &)> onIterate;

                /**
                 * @brief (AR) إرسال حدث: يُطلَق حين يحمل العنصر المُصاب أحداثًا (نقر).
                 *        expr = IREvent.expression (تعبير المفسّر النصّيّ) لتمييز
                 *        الأزرار في سطح المكتب؛ جسرُ وقت التشغيل يتجاهله ويرسل بالعقدة.
                 */
                std::function<void(IREventType, const std::string &expr, const IRNode *, const EventData &)> onEvent; // (② rfcs#46) يحمل بيانات الحدث

                /**
                 * @brief (AR) ردّ نداء ضغط مفتاح. يُرجع true لطلب الخروج (مثلًا F2).
                 */
                std::function<bool(UnifiedKeyCode)> onKey;

                /// (AR) تصفير عدّاد غليفات العرض قبل إطار القياس الأوّل (لعلامة GLYPHS).
                bool resetGlyphCountOnReady = false;
            };

            /**
             * @brief (AR) يفتح fb0 + evdev، يبني الشجرة، ويدير حلقة العرض + الإدخال.
             * @brief (EN) Opens fb0 + evdev, builds the tree, runs the render+input loop.
             *
             * المنزل الوحيد لآليّة الحلقة الحرّة — يعيد استعماله كلٌّ من sad_app_run
             * (تطبيقات ص المُترجَمة) وsad_desktop (سطح المكتب).
             *
             * @param cfg      إعداد التشغيل (ردود النداء + المسارات)
             * @param errorOut عند الفشل: رسالة عربيّة تشرح السبب (قد تكون nullptr)
             * @return 0 عند خروجٍ نظيف؛ قيمة غير صفريّة عند فشل التهيئة/العرض.
             */
            int runFreestandingApp(const FreestandingAppConfig &cfg, std::string *errorOut);

        } // namespace freestanding
    } // namespace ui
} // namespace sad

#endif // __linux__
#endif // SAD_UI_FREESTANDING_APP_RUNNER_H
