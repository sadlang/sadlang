/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: fb_demo.cpp
 * المسار: features/graphics/backends/freestanding/tools/fb_demo.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @brief (AR) عرض تجريبيّ: SadUI على /dev/fb0 (لينكس بلا X11/SDL) — ثابت وتفاعليّ
 * @brief (EN) Demo: SadUI on /dev/fb0 (Linux, no X11/SDL) — static + interactive
 *
 * الوصف:
 * ------
 * وضعان:
 *   • الوضع الثابت (افتراضيّ — الشريحة ١): شجرة IR ملوّنة تُرسم ~3 ثوانٍ ثمّ خروج.
 *   • الوضع التفاعليّ (وسيط «تفاعلي» — الشريحة ٢): زرّ SadUI كبير في الوسط؛
 *     أحداث الفأرة من evdev (/dev/input/event*) تُغذّى للمعالِجَين الموحّدَين
 *     (MouseEventProcessor/KeyboardEventProcessor)، والنقر على الزرّ يقلب خلفيّة
 *     الشاشة قلبًا جذريًّا (سوداء ⇄ فاتحة — قابل للقياس البكسليّ)، وزرّ «EXIT»
 *     (أو مفتاح Escape) يُنهي، وإلّا فمهلة ثابتة (~20 ثانية).
 *   • وضع النصّ (وسيط «نص» [مسار خطّ PSF] — الشريحة ٣): عنوان عربيّ مشكَّل
 *     (أشكال سياقيّة + لام-ألف + عكس مدًى عبر arabic_shaper داخل المُصيّر)
 *     بخطّ PSF خارجيّ، مع سقوط للخطّ المدمج عند غياب/فساد الخطّ (فشل ناعم).
 *
 * علامات الاختبار الآليّ (لا تُغيَّر — يعتمدها سكربت اختبار sad-os):
 *   نجاح التهيئة/العرض: «FB_DEMO_OK <عرض>x<ارتفاع>» على stdout
 *   كلّ نقرة مُعالَجة على زرّ القلب: «FB_CLICK_OK» على stdout
 *   وضع النصّ: «FB_TEXT_OK <عدد غليفات FE70-FEFF المرسومة فعلًا>» على stdout
 *   فشل: «FB_DEMO_FAIL: <سبب>» على stdout
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifdef __linux__

#include "sad_ui/freestanding/linuxfb.h"
#include "sad_ui/freestanding/renderer.h"
#include "sad_ui/freestanding/evdev_input.h"
#include "sad_ui/freestanding/psf_font.h"
#include "sad_ui/ir.h"
#include "sad_ui/prop_keys.h"
#include "sad_ui/mouse_processor.h"
#include "sad_ui/keyboard_processor.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace
{
    using sad::ui::EventData;
    using sad::ui::IREvent;
    using sad::ui::IREventType;
    using sad::ui::IRNode;
    using sad::ui::IRProperty;
    using sad::ui::KeyboardEventProcessor;
    using sad::ui::MouseButton;
    using sad::ui::MouseEventProcessor;
    using sad::ui::UINodeType;
    namespace fs = sad::ui::freestanding;

    // ─── علامات الاختبار الآليّ (عقد ثابت مع اختبار sad-os) ─────────────────
    constexpr const char *MARKER_OK_PREFIX = "FB_DEMO_OK";
    constexpr const char *MARKER_FAIL_PREFIX = "FB_DEMO_FAIL: ";
    constexpr const char *MARKER_CLICK_OK = "FB_CLICK_OK";
    /// (AR) علامة وضع النصّ: تليها مسافة ثمّ عدد غليفات أشكال العرض FE70–FEFF
    /// المرسومة فعلًا في إطار القياس (من عدّاد مسار الرسم الحقيقيّ في المُصيّر)
    constexpr const char *MARKER_TEXT_OK = "FB_TEXT_OK";

    // ─── وسائط سطر الأوامر ──────────────────────────────────────────────────
    /// (AR) وسيط تفعيل الوضع التفاعليّ (الاسم العربيّ القانونيّ الوحيد)
    constexpr const char *ARG_INTERACTIVE = "تفاعلي";

    /// (AR) وسيط وضع النصّ العربيّ المشكَّل (الشريحة ٣) — الوسيط التالي له
    /// (اختياريّ) مسار خطّ PSF؛ غيابه/فشله = سقوط للخطّ المدمج (فشل ناعم معلَن)
    constexpr const char *ARG_TEXT_MODE = "نص";

    // ─── مفاتيح خصائص IR غير المشمولة بـprop_keys.h (نفس مفاتيح المُرسِّم
    //     المشترك في platform_renderer.cpp — ثوابت مسمّاة لا سلاسل خام) ─────
    constexpr const char *PROP_BG_COLOR = "لون_خلفية";  // خلفية العقدة
    constexpr const char *PROP_TEXT_COLOR = "لون";      // لون النصّ
    constexpr const char *PROP_FONT_SIZE = "حجم_خط";    // حجم الخطّ
    constexpr const char *PROP_PADDING = sad::ui::props::PADDING; // SoT «حشوة» — حشو الحاوية/العمود
    constexpr const char *PROP_SPACING = "تباعد";       // التباعد بين الأبناء
    constexpr const char *PROP_BTN_TEXT = "نص";         // نصّ الزرّ (يقرأه المُرسِّم)

    // ─── لوحة ألوان العرض الثابت (بيانات العرض التجريبيّ) ───────────────────
    constexpr const char *COLOR_ROOT_BG = "#123A6E";   // أزرق داكن — خلفية الشاشة
    constexpr const char *COLOR_PANEL_1 = "#C0392B";   // أحمر قرميديّ
    constexpr const char *COLOR_PANEL_2 = "#27AE60";   // أخضر
    constexpr const char *COLOR_PANEL_3 = "#F1C40F";   // أصفر
    constexpr const char *COLOR_TEXT_LIGHT = "#FFFFFF"; // نصّ أبيض
    constexpr const char *COLOR_TEXT_DARK = "#101010";  // نصّ داكن (على الأصفر)

    // ─── نصوص العرض (لاتينيّة عمدًا: الخطّ النقطيّ المدمج يغطّيها كاملة) ────
    constexpr const char *DEMO_TITLE = "SadUI on /dev/fb0";
    constexpr const char *DEMO_PANEL_TEXT_1 = "Panel One - filled rect";
    constexpr const char *DEMO_PANEL_TEXT_2 = "Panel Two - filled rect";
    constexpr const char *DEMO_PANEL_TEXT_3 = "Panel Three - filled rect";
    constexpr const char *DEMO_FOOTER = "static frame loop ~3s, double buffered";

    // ─── معايير الرسم الثابت ────────────────────────────────────────────────
    constexpr double TITLE_FONT_SIZE = 48.0;
    constexpr double PANEL_FONT_SIZE = 24.0;
    constexpr double FOOTER_FONT_SIZE = 16.0;
    constexpr double ROOT_PADDING = 40.0;
    constexpr double ROOT_SPACING = 24.0;
    constexpr double PANEL_HEIGHT = 150.0;
    constexpr int FRAME_COUNT = 60;              // ‏60 إطارًا × 50مث ≈ 3 ثوانٍ
    constexpr int FRAME_INTERVAL_MS = 50;

    // ─── الوضع التفاعليّ: الألوان والقياسات ─────────────────────────────────
    // الخلفيّة الداكنة سوداء حقيقيّة (#000000) عمدًا: مقياس الاختبار البكسليّ
    // (نسبة غير الأسود) لا يفرّق درجات اللون — القلب أسود⇄فاتح وحده يُحدث
    // فرق نسبة جوهريًّا (~0.4) قابلًا للاشتراط ≥0.3:
    constexpr const char *COLOR_BG_DARK = "#000000";
    constexpr const char *COLOR_BG_LIGHT = "#F0F0F0";
    constexpr const char *COLOR_BTN_TOGGLE = "#2E86DE";  // أزرق — زرّ القلب
    constexpr const char *COLOR_BTN_EXIT = "#C0392B";    // أحمر — زرّ الخروج
    constexpr const char *COLOR_TITLE_NEUTRAL = "#808080"; // رماديّ مرئيّ على الخلفيّتين
    constexpr const char *BTN_TOGGLE_TEXT = "TOGGLE BACKGROUND";
    constexpr const char *BTN_EXIT_TEXT = "EXIT";
    constexpr const char *INTERACTIVE_TITLE = "SadUI evdev interactive";

    /// (AR) تعبيرا الحدثَين (العقد بين الشجرة ومُوزِّع fireEvent هنا)
    constexpr const char *EXPR_TOGGLE_BACKGROUND = "قلب_الخلفية";
    constexpr const char *EXPR_EXIT = "خروج";

    constexpr double TOGGLE_BTN_HEIGHT = 400.0; ///< يغطّي مركز الشاشة بامتياز
    constexpr double EXIT_BTN_HEIGHT = 80.0;
    constexpr double BTN_FONT_SIZE = 32.0;
    constexpr double INTERACTIVE_TITLE_FONT_SIZE = 24.0;

    /// (AR) مهلة الوضع التفاعليّ الثابتة (~20 ثانية) وخطوة الاستقصاء
    constexpr int INTERACTIVE_TIMEOUT_MS = 25000;
    constexpr int INTERACTIVE_POLL_INTERVAL_MS = 10;

    // ─── وضع النصّ العربيّ المشكَّل (الشريحة ٣) ─────────────────────────────
    /// (AR) العنوان المرجعيّ (بيانات العرض): «سلام لغة ص» —
    /// 7 غليفات عرض متوقَّعة: سلام⇒FEB3+FEFC+FEE1، لغة⇒FEDF+FED0+FE94، ص⇒FEB9
    /// (الاشتقاق الكامل موثَّق في tools/arabic_shaper_test.cpp — نفس الحالة الأخيرة)
    constexpr const char *TEXT_TITLE_AR = "سلام لغة ص";
    /// (AR) تذييل لاتينيّ للمرجعيّة البصريّة (يرسمه أيّ خطّ)
    constexpr const char *TEXT_FOOTER_LATIN = "Arabic shaped text on /dev/fb0";
    constexpr double TEXT_TITLE_FONT_SIZE = 48.0;
    /// (AR) عمر وضع النصّ: 300 إطار × 50مث = ~15 ثانية (مهلة كافية للقطة الاختبار)
    constexpr int TEXT_FRAME_COUNT = 300;

    /// (AR) رسالتا تحميل الخطّ (تصلان السجلّ التسلسليّ مع إعادة التوجيه)
    constexpr const char *MSG_FONT_LOADED_PREFIX = "FB_FONT_LOADED ";
    constexpr const char *MSG_FONT_FALLBACK_PREFIX = "FB_FONT_FALLBACK ";

    /// (AR) عقدة نصّ جاهزة (محتوى + حجم + لون)
    std::shared_ptr<IRNode> makeText(const std::string &content, double fontSize,
                                     const char *color)
    {
        auto node = IRNode::create(UINodeType::Text);
        node->setProperty(sad::ui::props::TEXT, content);
        node->setProperty(PROP_FONT_SIZE, fontSize);
        node->setProperty(PROP_TEXT_COLOR, std::string(color));
        return node;
    }

    /// (AR) لوحة ملوّنة بعرض/ارتفاع ثابتَين تحوي نصًّا
    std::shared_ptr<IRNode> makePanel(double width, double height,
                                      const char *bg, const std::string &label,
                                      const char *labelColor)
    {
        auto panel = IRNode::create(UINodeType::Container);
        panel->setProperty(sad::ui::props::WIDTH, width);
        panel->setProperty(sad::ui::props::HEIGHT, height);
        panel->setProperty(PROP_BG_COLOR, std::string(bg));
        panel->setProperty(PROP_PADDING, 20.0);
        panel->addChild(makeText(label, PANEL_FONT_SIZE, labelColor));
        return panel;
    }

    /// (AR) شجرة العرض الثابت: عمود داكن يملأ الشاشة + عنوان + 3 لوحات + تذييل
    std::shared_ptr<IRNode> buildDemoTree(uint32_t screenW, uint32_t screenH)
    {
        auto root = IRNode::create(UINodeType::Column);
        root->setProperty(sad::ui::props::WIDTH, static_cast<double>(screenW));
        root->setProperty(sad::ui::props::HEIGHT, static_cast<double>(screenH));
        root->setProperty(PROP_BG_COLOR, std::string(COLOR_ROOT_BG));
        root->setProperty(PROP_PADDING, ROOT_PADDING);
        root->setProperty(PROP_SPACING, ROOT_SPACING);

        const double panelW = static_cast<double>(screenW) - 2.0 * ROOT_PADDING;

        root->addChild(makeText(DEMO_TITLE, TITLE_FONT_SIZE, COLOR_TEXT_LIGHT));
        root->addChild(makePanel(panelW, PANEL_HEIGHT, COLOR_PANEL_1,
                                 DEMO_PANEL_TEXT_1, COLOR_TEXT_LIGHT));
        root->addChild(makePanel(panelW, PANEL_HEIGHT, COLOR_PANEL_2,
                                 DEMO_PANEL_TEXT_2, COLOR_TEXT_LIGHT));
        root->addChild(makePanel(panelW, PANEL_HEIGHT, COLOR_PANEL_3,
                                 DEMO_PANEL_TEXT_3, COLOR_TEXT_DARK));
        root->addChild(makeText(DEMO_FOOTER, FOOTER_FONT_SIZE, COLOR_TEXT_LIGHT));
        return root;
    }

    /// (AR) زرّ SadUI بعرض كامل الصفّ (يضمن تغطية مركز الشاشة أفقيًّا أيًّا كانت المحاذاة)
    std::shared_ptr<IRNode> makeButton(double width, double height,
                                       const char *bg, const char *label,
                                       const char *tapExpression)
    {
        auto btn = IRNode::create(UINodeType::Button);
        btn->setProperty(sad::ui::props::WIDTH, width);
        btn->setProperty(sad::ui::props::HEIGHT, height);
        btn->setProperty(PROP_BG_COLOR, std::string(bg));
        btn->setProperty(PROP_BTN_TEXT, std::string(label));
        btn->setProperty(PROP_FONT_SIZE, BTN_FONT_SIZE);

        IREvent tap;
        tap.type = IREventType::OnTap;
        tap.expression = tapExpression;
        btn->addEvent(tap);
        return btn;
    }

    /// (AR) شجرة الوضع التفاعليّ: خلفيّة قابلة للقلب + زرّ قلب كبير + زرّ خروج
    std::shared_ptr<IRNode> buildInteractiveTree(uint32_t screenW, uint32_t screenH)
    {
        auto root = IRNode::create(UINodeType::Column);
        root->setProperty(sad::ui::props::WIDTH, static_cast<double>(screenW));
        root->setProperty(sad::ui::props::HEIGHT, static_cast<double>(screenH));
        root->setProperty(PROP_BG_COLOR, std::string(COLOR_BG_DARK));
        root->setProperty(PROP_PADDING, ROOT_PADDING);
        root->setProperty(PROP_SPACING, ROOT_SPACING);

        const double rowW = static_cast<double>(screenW) - 2.0 * ROOT_PADDING;

        root->addChild(makeText(INTERACTIVE_TITLE, INTERACTIVE_TITLE_FONT_SIZE,
                                COLOR_TITLE_NEUTRAL));
        root->addChild(makeButton(rowW, TOGGLE_BTN_HEIGHT, COLOR_BTN_TOGGLE,
                                  BTN_TOGGLE_TEXT, EXPR_TOGGLE_BACKGROUND));
        root->addChild(makeButton(rowW, EXIT_BTN_HEIGHT, COLOR_BTN_EXIT,
                                  BTN_EXIT_TEXT, EXPR_EXIT));
        return root;
    }

    int fail(const std::string &reason)
    {
        std::cout << MARKER_FAIL_PREFIX << reason << std::endl;
        return 1;
    }

    /// (AR) مللي ثانية رتيبة (steady) — مصدر الوقت للمعالِجات الموحّدة
    uint32_t monotonicMs()
    {
        using namespace std::chrono;
        return static_cast<uint32_t>(
            duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
    }

    /**
     * @brief (AR) الوضع التفاعليّ: evdev ⇒ المعالِجان الموحّدان ⇒ نقر يقلب الخلفيّة
     * @return رمز خروج العمليّة (0 نجاح)
     */
    int runInteractive(fs::FreestandingWindow &window,
                       fs::linuxfb::LinuxFbDevice &fbDev)
    {
        const uint32_t w = fbDev.config.width;
        const uint32_t h = fbDev.config.height;

        auto root = buildInteractiveTree(w, h);
        window.setContent(root);

        // ─── مصدر evdev — فشل-مُغلق إن لم يُفتح أيّ جهاز إدخال ───
        fs::evdev::EvdevInputSource input;
        if (!input.open(w, h))
        {
            return fail(input.error);
        }

        bool running = true;
        bool darkBackground = true;

        // ─── المعالِج الموحَّد للفأرة (نفس الحقن الذي يفعله backends/desktop) ───
        MouseEventProcessor mouseProc;
        mouseProc.setHitTestCallback(
            [&window](float x, float y) -> const IRNode *
            { return window.hitTest(x, y); });
        mouseProc.setGetTimeMsCallback([]() -> uint32_t
                                       { return monotonicMs(); });
        mouseProc.setInvalidateCallback([&window]()
                                        { window.invalidate(); });
        mouseProc.setFireEventCallback(
            [&](IREventType type, const std::string &expr,
                const IRNode * /*node*/, const EventData & /*data*/)
            {
                if (type != IREventType::OnTap)
                    return;
                if (expr == EXPR_TOGGLE_BACKGROUND)
                {
                    // القلب الجذريّ: خلفيّة الشاشة كاملة سوداء ⇄ فاتحة
                    darkBackground = !darkBackground;
                    root->setProperty(PROP_BG_COLOR,
                                      std::string(darkBackground ? COLOR_BG_DARK
                                                                 : COLOR_BG_LIGHT));
                    window.invalidate();
                    std::cout << MARKER_CLICK_OK << std::endl; // endl = تدفّق فوريّ
                }
                else if (expr == EXPR_EXIT)
                {
                    running = false;
                }
            });

        // ─── المعالِج الموحَّد للوحة المفاتيح (Escape = خروج) ───
        KeyboardEventProcessor kbProc;
        kbProc.setFireEventCallback(
            [](IREventType, const std::string &, const IRNode *, const EventData &) {});
        kbProc.setGetContentRootCallback([&root]() -> const IRNode *
                                         { return root.get(); });
        kbProc.setInvalidateCallback([&window]()
                                     { window.invalidate(); });
        kbProc.setCloseWindowCallback([&running]()
                                      { running = false; });

        // ─── ربط مصدر evdev بالمعالِجَين + مؤشّر الفأرة المرسوم ───
        input.setMouseMoveCallback(
            [&](float x, float y, const sad::ui::MouseButtonState &buttons)
            {
                window.setCursorPosition(x, y);
                mouseProc.onMouseMove(x, y, buttons);
                window.invalidate(); // المؤشّر تحرّك ⇒ الإطار بحاجة رسم
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
            [&](sad::ui::UnifiedKeyCode code, const std::string &name, bool pressed)
            {
                if (pressed)
                    kbProc.onKeyDown(code, name);
                else
                    kbProc.onKeyUp(code, name);
            });

        window.setCursorPosition(input.pointerX(), input.pointerY());
        window.setCursorVisible(true);

        // أوّل إطار ثمّ إعلان الجاهزيّة (الاختبار ينتظر امتلاء الشاشة بعده):
        window.invalidate();
        if (!window.runOneFrame())
        {
            return fail("فشل رسم أوّل إطار في الوضع التفاعليّ");
        }
        std::cout << MARKER_OK_PREFIX << " " << w << "x" << h << std::endl;

        // ─── حلقة الأحداث: poll evdev + إعادة رسم عند invalidate ───
        const uint32_t startMs = monotonicMs();
        while (running && (monotonicMs() - startMs) <
                              static_cast<uint32_t>(INTERACTIVE_TIMEOUT_MS))
        {
            input.poll();
            if (!window.runOneFrame())
            {
                return fail("انقطعت حلقة الإطارات التفاعليّة");
            }
            std::this_thread::sleep_for(
                std::chrono::milliseconds(INTERACTIVE_POLL_INTERVAL_MS));
        }
        return 0;
    }

    /**
     * @brief (AR) وضع النصّ (الشريحة ٣): عنوان عربيّ مشكَّل بخطّ PSF + عدّاد إثبات
     *
     * يحمّل خطّ PSF من fontPath (إن مُرّر) وإلّا/وعند الفشل يسقط للخطّ المدمج
     * (فشل ناعم معلَن — المدمج بلا أشكال عرض فيكون العدّاد 0 ويكشفه الاختبار).
     * يبثّ «FB_TEXT_OK <عدد>» حيث العدد من عدّاد مسار الرسم الحقيقيّ في المُصيّر
     * (drawBitmapChar) لإطار القياس الأوّل — لا من إعادة حساب مستقلّة.
     */
    int runText(fs::FreestandingWindow &window,
                fs::linuxfb::LinuxFbDevice &fbDev,
                const std::string &fontPath)
    {
        const uint32_t w = fbDev.config.width;
        const uint32_t h = fbDev.config.height;
        fs::FreestandingRenderer *renderer = window.getFreestandingRenderer();

        if (!fontPath.empty())
        {
            fs::BitmapFont psfFont;
            std::string fontError;
            if (fs::psf::loadPsfFont(fontPath, psfFont, fontError))
            {
                renderer->loadBitmapFont(psfFont);
                std::cout << MSG_FONT_LOADED_PREFIX << fontPath << std::endl;
            }
            else
            {
                // فشل ناعم: نبقى على الخطّ المدمج (المحمَّل في main) ونعلن السبب
                std::cout << MSG_FONT_FALLBACK_PREFIX << fontError << std::endl;
            }
        }

        // شجرة بسيطة: خلفيّة ملوّنة تملأ الشاشة (قفزة نسبة بكسليّة قابلة للقياس)
        // + العنوان العربيّ المرجعيّ + تذييل لاتينيّ:
        auto root = IRNode::create(UINodeType::Column);
        root->setProperty(sad::ui::props::WIDTH, static_cast<double>(w));
        root->setProperty(sad::ui::props::HEIGHT, static_cast<double>(h));
        root->setProperty(PROP_BG_COLOR, std::string(COLOR_ROOT_BG));
        root->setProperty(PROP_PADDING, ROOT_PADDING);
        root->setProperty(PROP_SPACING, ROOT_SPACING);
        root->addChild(makeText(TEXT_TITLE_AR, TEXT_TITLE_FONT_SIZE, COLOR_TEXT_LIGHT));
        root->addChild(makeText(TEXT_FOOTER_LATIN, FOOTER_FONT_SIZE, COLOR_TEXT_LIGHT));
        window.setContent(root);

        // إطار القياس: تصفير العدّاد ثمّ رسم إطار واحد كامل وقراءته:
        renderer->resetPresentationGlyphCount();
        window.invalidate();
        if (!window.runOneFrame())
        {
            return fail("فشل رسم إطار القياس في وضع النصّ");
        }
        const uint32_t shapedGlyphs = renderer->presentationGlyphsDrawn();
        std::cout << MARKER_OK_PREFIX << " " << w << "x" << h << std::endl;
        std::cout << MARKER_TEXT_OK << " " << shapedGlyphs << std::endl;

        // إبقاء الشاشة حيّة مدّة كافية للقطة screendump في الاختبار:
        for (int frame = 0; frame < TEXT_FRAME_COUNT; ++frame)
        {
            window.invalidate();
            if (!window.runOneFrame())
            {
                return fail("انقطعت حلقة إطارات وضع النصّ");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_INTERVAL_MS));
        }
        return 0;
    }

    /// (AR) الوضع الثابت (الشريحة ١ كما كانت): حلقة إطارات ~3 ثوانٍ
    int runStatic(fs::FreestandingWindow &window,
                  fs::linuxfb::LinuxFbDevice &fbDev)
    {
        window.setContent(buildDemoTree(fbDev.config.width, fbDev.config.height));

        // invalidate كلّ دورة كي يُمرَّن مسار تخطيط+رسم+نسخ المخزن كاملًا كلّ إطار.
        // لا vsync على fbdev — الإيقاع الزمنيّ الثابت بديل عمليّ كافٍ لرسم ثابت:
        for (int frame = 0; frame < FRAME_COUNT; ++frame)
        {
            window.invalidate();
            if (!window.runOneFrame())
            {
                return fail("انقطعت حلقة الإطارات قبل اكتمالها");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_INTERVAL_MS));
        }
        std::cout << MARKER_OK_PREFIX << " " << fbDev.config.width << "x"
                  << fbDev.config.height << std::endl;
        return 0;
    }
} // namespace

int main(int argc, char **argv)
{
    // (AR) الوسائط: «تفاعلي» يفعّل الوضع التفاعليّ؛ «نص» يفعّل وضع النصّ المشكَّل
    // (أوّل وسيط بعده = مسار خطّ PSF اختياريّ)؛ أيّ وسيط آخر = مسار جهاز fb.
    bool interactive = false;
    bool textMode = false;
    std::string fontPath;
    std::string devicePath = fs::linuxfb::DEFAULT_FB_DEVICE_PATH;
    for (int i = 1; i < argc; ++i)
    {
        const std::string arg(argv[i]);
        if (arg == ARG_INTERACTIVE)
            interactive = true;
        else if (arg == ARG_TEXT_MODE)
            textMode = true;
        else if (textMode && fontPath.empty())
            fontPath = arg;
        else
            devicePath = arg;
    }

    fs::linuxfb::LinuxFbDevice fbDev;
    if (!fs::linuxfb::openFramebufferDevice(fbDev, devicePath))
    {
        return fail(fbDev.error);
    }

    fs::FreestandingWindow window;
    if (!window.initializeFramebuffer(fbDev.config))
    {
        fs::linuxfb::closeFramebufferDevice(fbDev);
        return fail("فشلت تهيئة المُصيّر المستقلّ على إعدادات الجهاز");
    }
    // (AR) الخطّ النقطيّ المدمج (8×16) — بدونه drawText صامت:
    window.getFreestandingRenderer()->loadBuiltinFont();

    sad::ui::PlatformWindowOptions winOpts;
    winOpts.width = static_cast<int>(fbDev.config.width);
    winOpts.height = static_cast<int>(fbDev.config.height);
    if (!window.create(winOpts))
    {
        fs::linuxfb::closeFramebufferDevice(fbDev);
        return fail("فشل إنشاء نافذة الوضع المستقلّ");
    }

    const int rc = interactive ? runInteractive(window, fbDev)
                   : textMode  ? runText(window, fbDev, fontPath)
                               : runStatic(window, fbDev);

    // (AR) خروج نظيف: تدمير النافذة ثمّ munmap/close للجهاز:
    window.destroy();
    fs::linuxfb::closeFramebufferDevice(fbDev);
    return rc;
}

#else // !__linux__

#include <iostream>

int main()
{
    // (AR) هذا العرض لينكسيّ حصرًا (fbdev) — فشل-مُغلق صريح على غيره:
    std::cout << "FB_DEMO_FAIL: fb_demo يعمل على لينكس فقط (/dev/fb0)" << std::endl;
    return 1;
}

#endif // __linux__
