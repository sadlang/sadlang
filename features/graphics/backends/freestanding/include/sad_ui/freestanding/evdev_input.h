/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: evdev_input.h
 * المسار: features/graphics/backends/freestanding/include/sad_ui/freestanding/evdev_input.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @brief (AR) مصدر أحداث evdev اللينكسيّ (/dev/input/event*) للوضع المستقلّ
 * @brief (EN) Linux evdev (/dev/input/event*) input source for freestanding mode
 *
 * الوصف:
 * ------
 * الوضع المستقلّ (FreestandingWindow) بلا مصدر أحداث إطلاقًا — هذا الملفّ يسدّ
 * الفجوة على لينكس بلا X11/SDL: يفتح كلّ أجهزة /dev/input/event* المتاحة
 * (non-blocking)، يفكّ struct input_event، ويحوّلها إلى استدعاءات موحَّدة:
 *
 *   EV_REL (حركة فأرة نسبيّة)  ⇒ MouseMoveCallback (موقع مؤشّر متراكم مقصوص للشاشة)
 *   EV_ABS (مؤشّر مطلق/لوح لمس) ⇒ MouseMoveCallback (بعد تدريج مدى الجهاز للشاشة)
 *   EV_KEY (BTN_LEFT/RIGHT/MIDDLE) ⇒ MouseButtonCallback
 *   EV_KEY (مفاتيح لوحة أساسيّة)   ⇒ KeyCallback (برمز UnifiedKeyCode الموحَّد)
 *
 * القرار البنيويّ (موثَّق عمدًا): هذه الطبقة «مصدر» صِرف لا يعرف شجرة الواجهة —
 * الربط بمعالِجَي النواة الموحّدَين (MouseEventProcessor/KeyboardEventProcessor)
 * مسؤوليّة المستهلك (كما يفعل backends/desktop مع أحداث SDL)، فيبقى المصدر
 * قابلًا لإعادة الاستعمال مع أيّ نافذة/معالِج دون تبعيّة دائريّة.
 *
 * القيود (فشل-مُغلق لا التفاف):
 *   • open() يفشل برسالة عربيّة إذا لم يُفتح أيّ جهاز — لا «نجاح» صامت بلا إدخال.
 *   • حركة المؤشّر تُجمَّع وتُبلَّغ عند حدود SYN_REPORT (دلالة evdev الصحيحة —
 *     حزمة الجهاز وحدة واحدة)، لا عند كلّ حدث REL جزئيّ.
 *   • المفاتيح غير المشمولة بالجدول الموحَّد تُتجاهَل صراحةً (لا رموز مُختلَقة).
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_FREESTANDING_EVDEV_INPUT_H
#define SAD_UI_FREESTANDING_EVDEV_INPUT_H

#ifdef __linux__

#include "sad_ui/mouse_processor.h"
#include "sad_ui/keyboard_processor.h"

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace sad
{
    namespace ui
    {
        namespace freestanding
        {
            namespace evdev
            {

                /// (AR) دليل أجهزة الإدخال اللينكسيّة — ثابت مسمّى (قاعدة: لا سلاسل خام)
                inline constexpr const char *EVDEV_DEVICE_DIR = "/dev/input";

                /// (AR) بادئة أسماء أجهزة evdev داخل الدليل
                inline constexpr const char *EVDEV_DEVICE_PREFIX = "event";

                /// (AR) أقصى فهرس جهاز يُستقصى بالحلقة (event0..event31)
                inline constexpr int EVDEV_MAX_DEVICES = 32;

                /**
                 * @brief (AR) مصدر أحداث evdev — يفتح الأجهزة ويبثّ أحداثًا موحَّدة
                 * @brief (EN) evdev input source — opens devices, emits unified events
                 *
                 * الاستعمال:
                 * @code
                 *   EvdevInputSource input;
                 *   if (!input.open(screenW, screenH)) { … input.error … }
                 *   input.setMouseMoveCallback(...);
                 *   input.setMouseButtonCallback(...);
                 *   input.setKeyCallback(...);
                 *   while (running) { input.poll(); window.runOneFrame(); }
                 * @endcode
                 */
                class EvdevInputSource
                {
                public:
                    EvdevInputSource() = default;
                    ~EvdevInputSource();

                    // لا نسخ — الكائن يملك واصفات ملفّات
                    EvdevInputSource(const EvdevInputSource &) = delete;
                    EvdevInputSource &operator=(const EvdevInputSource &) = delete;

                    /**
                     * @brief (AR) يفتح كلّ /dev/input/event* المتاحة — فشل-مُغلق
                     * @param screenWidth/screenHeight أبعاد الشاشة (لقصّ المؤشّر وتدريج EV_ABS)
                     * @return true إن فُتح جهاز واحد على الأقلّ؛ وإلّا false وerror تشرح بالعربيّة
                     */
                    bool open(uint32_t screenWidth, uint32_t screenHeight);

                    /// (AR) إغلاق كلّ الواصفات (آمنة الاستدعاء المتكرّر)
                    void close();

                    /// (AR) عدد الأجهزة المفتوحة فعلًا
                    std::size_t deviceCount() const { return devices_.size(); }

                    // ─── Callbacks — يعيّنها المستهلك قبل poll ─────────────────────

                    /// (AR) حركة المؤشّر (موقع مطلق بعد التجميع والقصّ) + حالة الأزرار
                    using MouseMoveCallback =
                        std::function<void(float x, float y, const MouseButtonState &buttons)>;
                    void setMouseMoveCallback(MouseMoveCallback cb) { mouseMoveCb_ = std::move(cb); }

                    /// (AR) ضغط/رفع زرّ فأرة عند الموقع الحاليّ للمؤشّر
                    using MouseButtonCallback =
                        std::function<void(MouseButton button, bool pressed, float x, float y)>;
                    void setMouseButtonCallback(MouseButtonCallback cb) { mouseButtonCb_ = std::move(cb); }

                    /// (AR) مفتاح لوحة (رمز موحَّد + اسم عرضيّ + ضغط/رفع)
                    using KeyCallback =
                        std::function<void(UnifiedKeyCode code, const std::string &name, bool pressed)>;
                    void setKeyCallback(KeyCallback cb) { keyCb_ = std::move(cb); }

                    /**
                     * @brief (AR) يجفّف كلّ الأحداث المعلَّقة من كلّ الأجهزة ويطلق الـcallbacks
                     * غير حاجز (non-blocking) — يعود فورًا إن لم يكن ثمّة أحداث.
                     */
                    void poll();

                    /**
                     * @brief (AR) يقرأ ويُهمِل كلّ الأحداث المعلَّقة من كلّ الأجهزة بلا
                     * إطلاق أيّ callback (غير حاجز — يعود فورًا عند EAGAIN).
                     * الغرض: تفريغ ما تراكم في طابور evdev بالنواة أثناء غياب الاستقصاء
                     * (تشغيل تطبيق فرعيّ من سطح المكتب) كي لا تُعاد نقرة قديمة معلَّقة
                     * بعد العودة. يُبطل أيضًا أيّ حركة مؤشّر مجمَّعة لم تُبلَّغ بعد.
                     */
                    void drainPending();

                    // ─── حالة المؤشّر ─────────────────────────────────────────────

                    float pointerX() const { return pointerX_; }
                    float pointerY() const { return pointerY_; }
                    const MouseButtonState &buttons() const { return buttons_; }

                    /// (AR) تعيين موقع المؤشّر ابتدائيًّا (يُقصّ لحدود الشاشة)
                    void setPointerPosition(float x, float y);

                    /// (AR) رسالة الخطأ العربيّة الأخيرة (فارغة = لا خطأ)
                    std::string error;

                private:
                    /// (AR) جهاز evdev مفتوح + مدى EV_ABS المُدرَّج (إن وُجد)
                    struct Device
                    {
                        int fd = -1;
                        std::string path;
                        bool hasAbsX = false;
                        bool hasAbsY = false;
                        float absMinX = 0.0f, absMaxX = 0.0f;
                        float absMinY = 0.0f, absMaxY = 0.0f;
                    };

                    std::vector<Device> devices_;
                    uint32_t screenW_ = 0;
                    uint32_t screenH_ = 0;

                    float pointerX_ = 0.0f;
                    float pointerY_ = 0.0f;
                    MouseButtonState buttons_;
                    bool movedSinceReport_ = false; ///< تجمّعت حركة منذ آخر SYN_REPORT

                    void clampPointer();
                    void flushPendingMove(); ///< إبلاغ الحركة المجمَّعة (عند SYN_REPORT)
                    void handleRelEvent(uint16_t code, int32_t value);
                    void handleAbsEvent(const Device &dev, uint16_t code, int32_t value);
                    void handleKeyEvent(uint16_t code, int32_t value);

                    MouseMoveCallback mouseMoveCb_;
                    MouseButtonCallback mouseButtonCb_;
                    KeyCallback keyCb_;
                };

            } // namespace evdev
        } // namespace freestanding
    } // namespace ui
} // namespace sad

#endif // __linux__
#endif // SAD_UI_FREESTANDING_EVDEV_INPUT_H
