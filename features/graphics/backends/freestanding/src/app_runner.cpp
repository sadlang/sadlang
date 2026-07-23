/**
 * ملف: app_runner.cpp
 * المسار: features/graphics/backends/freestanding/src/app_runner.cpp
 *
 * @brief (AR) تنفيذ حلقة تشغيل تطبيق SadUI الحرّة (fb0 + evdev). آليّة الحلقة
 *             المرفوعة من sad_desktop.cpp لتُشارَك بين الجسرين (وقت التشغيل +
 *             سطح المكتب) — لا تكرار.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 */

#ifdef __linux__

#include "sad_ui/freestanding/app_runner.h"
#include "sad_ui/freestanding/psf_font.h"
#include "sad_ui/event_dispatch.h" // (AR) إرسال الأحداث بأطواره — مصدر الحقيقة المشترك
#include "sad_ui/mouse_processor.h"
#include "sad_ui/platform_renderer.h" // PlatformWindowOptions

#include <chrono>
#include <thread>

namespace sad
{
    namespace ui
    {
        namespace freestanding
        {
            namespace
            {
                /// (AR) الزمن الأحاديّ بالمِلّي ثانية — لمعالِج الفأرة (نقر/سحب).
                uint32_t monotonicMs()
                {
                    using namespace std::chrono;
                    return static_cast<uint32_t>(
                        duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
                }
            } // namespace

            int runFreestandingApp(const FreestandingAppConfig &cfg, std::string *errorOut)
            {
                const auto setError = [&](const std::string &msg) -> int
                {
                    if (errorOut)
                        *errorOut = msg;
                    return 1;
                };

                if (!cfg.buildRoot)
                    return setError("runFreestandingApp: buildRoot غير مضبوط");

                // ─── فتح جهاز الإطار (fb0) — فشل-مُغلق ───
                linuxfb::LinuxFbDevice fbDev;
                if (!linuxfb::openFramebufferDevice(fbDev, cfg.devicePath))
                    return setError(fbDev.error);

                // ─── النافذة المستقلّة فوق إعدادات الجهاز ───
                FreestandingWindow window;
                if (!window.initializeFramebuffer(fbDev.config))
                {
                    linuxfb::closeFramebufferDevice(fbDev);
                    return setError("فشلت تهيئة المُصيّر المستقلّ على إعدادات الجهاز");
                }
                // الخطّ المدمج أوّلًا (لاتينيّ/أساس) — خطّ PSF يعلوه أدناه:
                window.getFreestandingRenderer()->loadBuiltinFont();

                PlatformWindowOptions winOpts;
                winOpts.width = static_cast<int>(fbDev.config.width);
                winOpts.height = static_cast<int>(fbDev.config.height);
                if (!window.create(winOpts))
                {
                    linuxfb::closeFramebufferDevice(fbDev);
                    return setError("فشل إنشاء نافذة الوضع المستقلّ");
                }

                const uint32_t w = fbDev.config.width;
                const uint32_t h = fbDev.config.height;
                FreestandingRenderer *renderer = window.getFreestandingRenderer();

                // ─── خطّ العرض العربيّ (PSF) — سقوط ناعم للمدمج عند الفشل ───
                if (!cfg.fontPath.empty())
                {
                    BitmapFont psfFont;
                    std::string fontError;
                    if (psf::loadPsfFont(cfg.fontPath, psfFont, fontError))
                    {
                        renderer->loadBitmapFont(psfFont);
                        if (cfg.onFontResult)
                            cfg.onFontResult(true, cfg.fontPath);
                    }
                    else if (cfg.onFontResult)
                    {
                        // فشل ناعم: نبقى على الخطّ المدمج ونترك الإعلان للسياسة.
                        cfg.onFontResult(false, fontError);
                    }
                }

                // ─── بناء الشجرة (سياسة المستهلك) بالأبعاد الفعليّة ───
                std::shared_ptr<IRNode> root = cfg.buildRoot(w, h);
                if (!root)
                {
                    window.destroy();
                    linuxfb::closeFramebufferDevice(fbDev);
                    return setError("buildRoot أرجع شجرةً فارغة");
                }
                window.setContent(root);

                // ─── مصدر evdev — فشل-مُغلق إن لم يُفتح أيّ جهاز إدخال ───
                evdev::EvdevInputSource input;
                if (!input.open(w, h))
                {
                    const std::string err = input.error;
                    window.destroy();
                    linuxfb::closeFramebufferDevice(fbDev);
                    return setError(err);
                }

                AppLoopContext ctx{window, input, w, h};
                bool running = true;

                // ─── المعالِج الموحَّد للفأرة → إرسال الأحداث لسياسة المستهلك ───
                MouseEventProcessor mouseProc;
                mouseProc.setHitTestCallback(
                    [&window](float x, float y) -> const IRNode *
                    { return window.hitTest(x, y); });
                mouseProc.setGetTimeMsCallback([]() -> uint32_t
                                               { return monotonicMs(); });
                mouseProc.setInvalidateCallback([&window]()
                                                { window.invalidate(); });
                // (AR) جذر المحتوى الحيّ: يستعمله المعالج للتحقّق من بقاء العقدة
                //      المُمسَكة أثناء السحب — ردّ نداء ص قد يستبدل الشجرة
                //      (تحديث_حالة ⇒ setContent) فتتحرّر العقدة القديمة.
                mouseProc.setGetContentRootCallback(
                    [&window]() -> const IRNode *
                    { return window.getContentRoot(); });
                mouseProc.setFireEventCallback(
                    [&](IREventType type, const std::string & /*expr*/,
                        const IRNode *node, const EventData &data)
                    {
                        // (AR) كان الربط مباشرًا بـcfg.onEvent ⇒ صفر تفرّع في الوضع
                        //      الحرّ. صار يمرّ بمُرسِل المكتبة المشترك، فيسلك أطوار
                        //      الالتقاط/الهدف/الفقاعات كسطح المكتب تمامًا.
                        dispatchEvent(type, node, data,
                                      [&](IREventType t, const std::string &e,
                                          const IRNode *n, const EventData &d)
                                      {
                                          if (cfg.onEvent)
                                              cfg.onEvent(t, e, n, d); // (② rfcs#46) بيانات الحدث
                                      });
                    });

                // ─── ربط evdev: فأرة (مؤشّر مرسوم) + مفاتيح لوحة ───
                input.setMouseMoveCallback(
                    [&](float x, float y, const MouseButtonState &buttons)
                    {
                        window.setCursorPosition(x, y);
                        mouseProc.onMouseMove(x, y, buttons);
                        window.invalidate(); // المؤشّر تحرّك ⇒ رسم عند التغيّر
                    });
                input.setMouseButtonCallback(
                    [&](MouseButton button, bool pressed, float x, float y)
                    {
                        if (pressed)
                            mouseProc.onMouseDown(button, x, y);
                        else
                            mouseProc.onMouseUp(button, x, y);
                    });
                input.setKeyCallback(
                    [&](UnifiedKeyCode code, const std::string & /*name*/, bool pressed)
                    {
                        if (pressed && cfg.onKey && cfg.onKey(code))
                            running = false;
                    });

                window.setCursorPosition(input.pointerX(), input.pointerY());
                window.setCursorVisible(true);

                // ─── الإطار الأوّل: (تصفير عدّاد الغليفات إن طُلب) ثمّ عرض وإعلان الجاهزيّة ───
                if (cfg.resetGlyphCountOnReady)
                    renderer->resetPresentationGlyphCount();
                window.invalidate();
                if (!window.runOneFrame())
                {
                    window.destroy();
                    linuxfb::closeFramebufferDevice(fbDev);
                    return setError("فشل رسم أوّل إطار للتطبيق الحرّ");
                }
                if (cfg.onReady)
                    cfg.onReady(ctx);

                // ─── الحلقة الدائمة: بلا مهلة — رسم عند التغيّر فقط ───
                while (running)
                {
                    input.poll();

                    if (cfg.onIterate && !cfg.onIterate(ctx))
                        break;

                    if (!window.runOneFrame())
                    {
                        window.destroy();
                        linuxfb::closeFramebufferDevice(fbDev);
                        return setError("انقطعت حلقة إطارات التطبيق الحرّ");
                    }
                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(APP_LOOP_POLL_INTERVAL_MS));
                }

                window.destroy();
                linuxfb::closeFramebufferDevice(fbDev);
                return 0;
            }

        } // namespace freestanding
    } // namespace ui
} // namespace sad

#endif // __linux__
