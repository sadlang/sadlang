/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: evdev_input.cpp
 * المسار: features/graphics/backends/freestanding/src/evdev_input.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @brief (AR) تنفيذ مصدر أحداث evdev (/dev/input/event*) للوضع المستقلّ
 * @brief (EN) Implementation of the evdev input source for freestanding mode
 *
 * التفاصيل الدلاليّة (evdev):
 *   • الجهاز يبثّ أحداثًا جزئيّة (REL_X ثم REL_Y ثم مفاتيح…) ويختمها بحدث
 *     مزامنة EV_SYN/SYN_REPORT = «الحزمة اكتملت». نُجمّع الحركة ونبلّغها عند
 *     حدود المزامنة فقط — لا حركات نصفيّة (X بلا Y).
 *   • EV_KEY بقيمة 2 = تكرار تلقائيّ (autorepeat) — يُتجاهَل: المعالِجات الموحَّدة
 *     تتوقّع ضغطًا/رفعًا حقيقيَّين.
 *   • أزرار الفأرة (BTN_LEFT/RIGHT/MIDDLE) تُبلَّغ فورًا (لا تنتظر SYN_REPORT)
 *     لكن بعد دفع أيّ حركة مجمَّعة قبلها — كي يصل الزرّ بالموقع الصحيح.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifdef __linux__

#include "sad_ui/freestanding/evdev_input.h"

#include <linux/input.h>

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace sad
{
    namespace ui
    {
        namespace freestanding
        {
            namespace evdev
            {

                namespace
                {
                    /// (AR) عدد أحداث input_event تُقرأ دفعة واحدة من كلّ جهاز
                    constexpr std::size_t READ_BATCH_EVENTS = 64;

                    /**
                     * @brief (AR) تحويل رمز مفتاح لينكس (KEY_*) إلى الرمز الموحَّد
                     * @param name يُملأ بالاسم العرضيّ عند النجاح
                     * @return الرمز الموحَّد، أو Unknown إن كان المفتاح خارج الجدول
                     */
                    UnifiedKeyCode mapLinuxKey(uint16_t code, std::string &name)
                    {
                        switch (code)
                        {
                        // ─── مفاتيح تحكّم أساسيّة ───
                        case KEY_ENTER:     name = "Enter";     return UnifiedKeyCode::Return;
                        case KEY_ESC:       name = "Escape";    return UnifiedKeyCode::Escape;
                        case KEY_BACKSPACE: name = "Backspace"; return UnifiedKeyCode::Backspace;
                        case KEY_TAB:       name = "Tab";       return UnifiedKeyCode::Tab;
                        case KEY_SPACE:     name = "Space";     return UnifiedKeyCode::Space;
                        // ─── مفاتيح الوظائف المعتمدة (F2 = «اخرج للصدَفة» في
                        //     سطح مكتب sad-os — sad_desktop.cpp؛ بقيّة مفاتيح F
                        //     خارج نطاق الشريحة الحاليّة: حدّ معلَن) ───
                        case KEY_F2:        name = "F2";        return UnifiedKeyCode::F2;
                        // ─── أسهم ───
                        case KEY_RIGHT: name = "Right"; return UnifiedKeyCode::Right;
                        case KEY_LEFT:  name = "Left";  return UnifiedKeyCode::Left;
                        case KEY_DOWN:  name = "Down";  return UnifiedKeyCode::Down;
                        case KEY_UP:    name = "Up";    return UnifiedKeyCode::Up;
                        // ─── تحرير ───
                        case KEY_HOME:     name = "Home";     return UnifiedKeyCode::Home;
                        case KEY_END:      name = "End";      return UnifiedKeyCode::End;
                        case KEY_DELETE:   name = "Delete";   return UnifiedKeyCode::Delete;
                        case KEY_PAGEUP:   name = "PageUp";   return UnifiedKeyCode::PageUp;
                        case KEY_PAGEDOWN: name = "PageDown"; return UnifiedKeyCode::PageDown;
                        // ─── حروف (ترتيب لوحة QWERTY الفيزيائيّ في رموز لينكس) ───
                        case KEY_A: name = "A"; return UnifiedKeyCode::A;
                        case KEY_B: name = "B"; return UnifiedKeyCode::B;
                        case KEY_C: name = "C"; return UnifiedKeyCode::C;
                        case KEY_D: name = "D"; return UnifiedKeyCode::D;
                        case KEY_E: name = "E"; return UnifiedKeyCode::E;
                        case KEY_F: name = "F"; return UnifiedKeyCode::F;
                        case KEY_G: name = "G"; return UnifiedKeyCode::G;
                        case KEY_H: name = "H"; return UnifiedKeyCode::H;
                        case KEY_I: name = "I"; return UnifiedKeyCode::I;
                        case KEY_J: name = "J"; return UnifiedKeyCode::J;
                        case KEY_K: name = "K"; return UnifiedKeyCode::K;
                        case KEY_L: name = "L"; return UnifiedKeyCode::L;
                        case KEY_M: name = "M"; return UnifiedKeyCode::M;
                        case KEY_N: name = "N"; return UnifiedKeyCode::N;
                        case KEY_O: name = "O"; return UnifiedKeyCode::O;
                        case KEY_P: name = "P"; return UnifiedKeyCode::P;
                        case KEY_Q: name = "Q"; return UnifiedKeyCode::Q;
                        case KEY_R: name = "R"; return UnifiedKeyCode::R;
                        case KEY_S: name = "S"; return UnifiedKeyCode::S;
                        case KEY_T: name = "T"; return UnifiedKeyCode::T;
                        case KEY_U: name = "U"; return UnifiedKeyCode::U;
                        case KEY_V: name = "V"; return UnifiedKeyCode::V;
                        case KEY_W: name = "W"; return UnifiedKeyCode::W;
                        case KEY_X: name = "X"; return UnifiedKeyCode::X;
                        case KEY_Y: name = "Y"; return UnifiedKeyCode::Y;
                        case KEY_Z: name = "Z"; return UnifiedKeyCode::Z;
                        default:
                            return UnifiedKeyCode::Unknown;
                        }
                    }
                } // namespace

                EvdevInputSource::~EvdevInputSource() { close(); }

                bool EvdevInputSource::open(uint32_t screenWidth, uint32_t screenHeight)
                {
                    close();
                    error.clear();

                    if (screenWidth == 0 || screenHeight == 0)
                    {
                        error = "أبعاد شاشة صفريّة — لا يمكن قصّ المؤشّر ولا تدريج EV_ABS";
                        return false;
                    }
                    screenW_ = screenWidth;
                    screenH_ = screenHeight;

                    // اكتشاف بسيط بالحلقة: event0..event(N-1) — الأجهزة الغائبة تُتخطّى بصمت
                    // (غيابها طبيعيّ)، وغياب *الجميع* فشلٌ ناطق أدناه.
                    for (int i = 0; i < EVDEV_MAX_DEVICES; ++i)
                    {
                        char path[64];
                        std::snprintf(path, sizeof(path), "%s/%s%d",
                                      EVDEV_DEVICE_DIR, EVDEV_DEVICE_PREFIX, i);
                        int fd = ::open(path, O_RDONLY | O_NONBLOCK | O_CLOEXEC);
                        if (fd < 0)
                            continue;

                        Device dev;
                        dev.fd = fd;
                        dev.path = path;

                        // مدى المحاور المطلقة (لوح لمس/جهاز tablet) — إن وُجدت:
                        struct input_absinfo absX
                        {
                        };
                        struct input_absinfo absY
                        {
                        };
                        if (::ioctl(fd, EVIOCGABS(ABS_X), &absX) == 0 && absX.maximum > absX.minimum)
                        {
                            dev.hasAbsX = true;
                            dev.absMinX = static_cast<float>(absX.minimum);
                            dev.absMaxX = static_cast<float>(absX.maximum);
                        }
                        if (::ioctl(fd, EVIOCGABS(ABS_Y), &absY) == 0 && absY.maximum > absY.minimum)
                        {
                            dev.hasAbsY = true;
                            dev.absMinY = static_cast<float>(absY.minimum);
                            dev.absMaxY = static_cast<float>(absY.maximum);
                        }

                        devices_.push_back(dev);
                    }

                    if (devices_.empty())
                    {
                        error = std::string("لم يُفتح أيّ جهاز إدخال في ") + EVDEV_DEVICE_DIR +
                                " — تحقّق من CONFIG_INPUT_EVDEV في النواة ومن devtmpfs";
                        return false;
                    }

                    // المؤشّر يبدأ في مركز الشاشة (موقع محايد قابل للتغيير بـsetPointerPosition)
                    pointerX_ = static_cast<float>(screenW_) / 2.0f;
                    pointerY_ = static_cast<float>(screenH_) / 2.0f;
                    return true;
                }

                void EvdevInputSource::close()
                {
                    for (auto &dev : devices_)
                    {
                        if (dev.fd >= 0)
                            ::close(dev.fd);
                    }
                    devices_.clear();
                }

                void EvdevInputSource::setPointerPosition(float x, float y)
                {
                    pointerX_ = x;
                    pointerY_ = y;
                    clampPointer();
                }

                void EvdevInputSource::clampPointer()
                {
                    const float maxX = static_cast<float>(screenW_) - 1.0f;
                    const float maxY = static_cast<float>(screenH_) - 1.0f;
                    if (pointerX_ < 0.0f)
                        pointerX_ = 0.0f;
                    if (pointerX_ > maxX)
                        pointerX_ = maxX;
                    if (pointerY_ < 0.0f)
                        pointerY_ = 0.0f;
                    if (pointerY_ > maxY)
                        pointerY_ = maxY;
                }

                void EvdevInputSource::flushPendingMove()
                {
                    if (!movedSinceReport_)
                        return;
                    movedSinceReport_ = false;
                    clampPointer();
                    if (mouseMoveCb_)
                        mouseMoveCb_(pointerX_, pointerY_, buttons_);
                }

                void EvdevInputSource::handleRelEvent(uint16_t code, int32_t value)
                {
                    switch (code)
                    {
                    case REL_X:
                        pointerX_ += static_cast<float>(value);
                        movedSinceReport_ = true;
                        break;
                    case REL_Y:
                        pointerY_ += static_cast<float>(value);
                        movedSinceReport_ = true;
                        break;
                    default:
                        // REL_WHEEL وغيرها خارج نطاق هذه الشريحة (لا عناصر تمرير بعد)
                        break;
                    }
                }

                void EvdevInputSource::handleAbsEvent(const Device &dev, uint16_t code, int32_t value)
                {
                    // تدريج مدى الجهاز [min,max] إلى مدى الشاشة [0,w-1]/[0,h-1]:
                    if (code == ABS_X && dev.hasAbsX)
                    {
                        const float ratio = (static_cast<float>(value) - dev.absMinX) /
                                            (dev.absMaxX - dev.absMinX);
                        pointerX_ = ratio * (static_cast<float>(screenW_) - 1.0f);
                        movedSinceReport_ = true;
                    }
                    else if (code == ABS_Y && dev.hasAbsY)
                    {
                        const float ratio = (static_cast<float>(value) - dev.absMinY) /
                                            (dev.absMaxY - dev.absMinY);
                        pointerY_ = ratio * (static_cast<float>(screenH_) - 1.0f);
                        movedSinceReport_ = true;
                    }
                }

                void EvdevInputSource::handleKeyEvent(uint16_t code, int32_t value)
                {
                    // value: 0 = رفع، 1 = ضغط، 2 = تكرار تلقائيّ (يُتجاهَل)
                    if (value != 0 && value != 1)
                        return;
                    const bool pressed = (value == 1);

                    // ─── أزرار الفأرة ───
                    MouseButton button = MouseButton::Unknown;
                    switch (code)
                    {
                    case BTN_LEFT:
                        button = MouseButton::Left;
                        buttons_.leftPressed = pressed;
                        break;
                    case BTN_RIGHT:
                        button = MouseButton::Right;
                        buttons_.rightPressed = pressed;
                        break;
                    case BTN_MIDDLE:
                        button = MouseButton::Middle;
                        buttons_.middlePressed = pressed;
                        break;
                    default:
                        break;
                    }
                    if (button != MouseButton::Unknown)
                    {
                        // ادفع أيّ حركة مجمَّعة أوّلًا كي يصل الزرّ بالموقع الصحيح:
                        flushPendingMove();
                        if (mouseButtonCb_)
                            mouseButtonCb_(button, pressed, pointerX_, pointerY_);
                        return;
                    }

                    // ─── مفاتيح اللوحة الأساسيّة ───
                    std::string name;
                    const UnifiedKeyCode unified = mapLinuxKey(code, name);
                    if (unified == UnifiedKeyCode::Unknown)
                        return; // خارج الجدول الموحَّد — تجاهل صريح موثَّق
                    if (keyCb_)
                        keyCb_(unified, name, pressed);
                }

                void EvdevInputSource::poll()
                {
                    struct input_event batch[READ_BATCH_EVENTS];

                    for (auto &dev : devices_)
                    {
                        if (dev.fd < 0)
                            continue;
                        for (;;)
                        {
                            const ssize_t n = ::read(dev.fd, batch, sizeof(batch));
                            if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
                                break; // لا أحداث الآن — جولة الجهاز انتهت طبيعيًّا
                            if (n <= 0)
                            {
                                // خطأ دائم (ENODEV: انتُزع الجهاز) أو نهاية دفق — إبطال
                                // الواصف كي لا يُعاد استقصاء جهاز ميّت كلّ جولة:
                                ::close(dev.fd);
                                dev.fd = -1;
                                break;
                            }
                            const std::size_t count =
                                static_cast<std::size_t>(n) / sizeof(struct input_event);
                            for (std::size_t i = 0; i < count; ++i)
                            {
                                const struct input_event &ev = batch[i];
                                switch (ev.type)
                                {
                                case EV_REL:
                                    handleRelEvent(ev.code, ev.value);
                                    break;
                                case EV_ABS:
                                    handleAbsEvent(dev, ev.code, ev.value);
                                    break;
                                case EV_KEY:
                                    handleKeyEvent(ev.code, ev.value);
                                    break;
                                case EV_SYN:
                                    if (ev.code == SYN_REPORT)
                                        flushPendingMove();
                                    break;
                                default:
                                    break;
                                }
                            }
                        }
                    }

                    // شبكة أمان: جهاز شذّ عن ختم حزمه بـSYN_REPORT — لا نُضيّع حركته:
                    flushPendingMove();
                }

                void EvdevInputSource::drainPending()
                {
                    // نفس آليّة القراءة غير الحاجزة في poll()، لكن تُقرأ الأحداث وتُهمَل
                    // بلا فكّ ولا callback: تفريغ طابور النواة المتراكم أثناء غياب
                    // الاستقصاء (منع إعادة تشغيل نقرة قديمة معلَّقة).
                    struct input_event batch[READ_BATCH_EVENTS];

                    for (auto &dev : devices_)
                    {
                        if (dev.fd < 0)
                            continue;
                        for (;;)
                        {
                            const ssize_t n = ::read(dev.fd, batch, sizeof(batch));
                            if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
                                break; // لا أحداث الآن — جولة الجهاز انتهت طبيعيًّا
                            if (n <= 0)
                            {
                                // خطأ دائم/نهاية دفق — إبطال الواصف كي لا يُعاد استقصاؤه:
                                ::close(dev.fd);
                                dev.fd = -1;
                                break;
                            }
                            // الأحداث المقروءة تُهمَل صراحةً (لا تحويل ولا إبلاغ).
                        }
                    }

                    // أيّ حركة مؤشّر مجمَّعة قبل التجفيف تُلغى (لا تُبلَّغ بعده):
                    movedSinceReport_ = false;
                }

            } // namespace evdev
        } // namespace freestanding
    } // namespace ui
} // namespace sad

#endif // __linux__
