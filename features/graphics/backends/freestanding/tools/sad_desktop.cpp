/**
 * ملف: sad_desktop.cpp
 * المسار: features/graphics/backends/freestanding/tools/sad_desktop.cpp
 *
 * @brief (AR) سطح مكتب sad-os الأوّليّ على /dev/fb0 (الشريحة ١ من مذكّرة سطح المكتب)
 * @brief (EN) Initial sad-os desktop shell on /dev/fb0 (slice 1 of the desktop memo)
 *
 * الوصف:
 * ------
 * أداة C++ بنفس مكانة fb_demo.cpp (المذكّرة الملزمة:
 * sad-os/متطلبات/تقرير_الميزات/مذكرة-تصميم-سطح-المكتب-والنسختين.md، قرار ق٥:
 * C++ الآن، والهجرة إلى ص دَين معلَن): تبني شجرة SadUI IR وتُصيّرها عبر
 * FreestandingRenderer وجسر linuxfb — خلفيّة + شريط علويّ فيه ساعة حيّة واسم
 * المضيف + عنوان عربيّ مشكَّل + نصّ إرشاديّ + زرّ ‹الطرفيّة›.
 *
 * الحلقة دائمة (بلا مهلة fb-demo) وترسم **عند التغيّر فقط** (invalidate عند
 * تغيّر دقيقة الساعة أو حركة الفأرة — خطر الأداء ٢ في المذكّرة: لا invalidate
 * كلّ دورة)، والخروج للصدَفة النصّيّة بمفتاح F2 أو نقرة زرّ ‹الطرفيّة›
 * (رمز الخروج 7 = اتّفاقيّة «اخرج للصدَفة» — init يسلّم بعدها للمسار القائم).
 *
 * علامات الاختبار الآليّ (عقد [4م] مجمَّد — لا تُغيَّر؛ علامات الشريحة ٢ إضافيّة أدناه):
 *   بعد أوّل إطار:      «DESKTOP_OK <عرض>x<ارتفاع>» على stdout
 *   غليفات أوّل إطار:   «DESKTOP_GLYPHS <عدد أشكال العرض FE70-FEFF المرسومة>»
 *   الخروج للصدَفة:     «DESKTOP_SHELL_EXIT» على stdout ثمّ رمز الخروج 7
 *   فشل:               «DESKTOP_FAIL: <سبب>» على stdout ورمز الخروج 1
 *
 * علامات مشغّل التطبيقات (الشريحة ٢ — إضافيّة، تعتمدها بوّابة [4ن]):
 *   إطلاق تطبيق:        «DESKTOP_LAUNCH <مسار>» قبل fork/exec
 *   خروج التطبيق:       «DESKTOP_APP_EXIT <رمز>» بعد waitpid
 *   استئناف السطح:      «DESKTOP_RESUMED» بعد إعادة الرسم الكامل
 *   تعذّر الإطلاق:      «DESKTOP_LAUNCH_FAIL: <سبب>» (فشل ناعم — البقاء على السطح)
 *
 * علامة الإطفاء (الشريحة ٣ — إضافيّة، تعتمدها بوّابة [4س]):
 *   طلب الإطفاء:        «DESKTOP_SHUTDOWN_REQUEST» ثمّ رمز الخروج 8 (يواصل init
 *                       لمسار الإطفاء النظيف — تعريب ٢٦ — بلا تسليم للصدَفة النصّيّة)
 *
 * الوسائط: الوسيط الأوّل (اختياريّ) = مسار خطّ PSF بأشكال العرض العربيّة —
 * غيابه/فشله سقوط ناعم للخطّ المدمج (بلا أشكال عرض ⇒ العدّاد 0 وتكشفه البوّابة).
 * الوسيط الثاني (اختياريّ) = مسار جهاز الإطار (افتراضيًّا /dev/fb0).
 *
 * حدود معلَنة (الشريحة ١): الشريط العلويّ سطر نصّ واحد يجمع الساعة واسم المضيف
 * (لا محاذاة يمين/يسار مستقلّة لكلّ طرف)؛ الساعة HH:MM بأرقام غربيّة من
 * localtime (بلا TZ في initramfs ⇒ UTC فعليًّا — قرار ق٤ يتوارث قرارات تعريب ٨)؛
 * زرّ «العرض التجريبيّ» (الشريحة ٢) يظهر فقط حين يمرَّر مسار تطبيق صالح عبر البيئة
 * (SAD_DESKTOP_APP) — غياب المتغيّر ⇒ غياب الزرّ بنيويًّا. زرّ «إطفاء» (الشريحة ٣)
 * حاضر دائمًا (الإطفاء قدرة أساسيّة لا يحرسها env) ⇒ خروج بالرمز 8 = طلب إطفاء
 * صريح يواصل به init مسار الإطفاء النظيف القائم (تعريب ٢٦) بلا تسليم للصدَفة.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 */

#ifdef __linux__

#include "sad_ui/freestanding/app_runner.h"
#include "sad_ui/freestanding/linuxfb.h"
#include "sad_ui/freestanding/renderer.h"
#include "sad_ui/ir.h"
#include "sad_ui/prop_keys.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include <sys/wait.h>
#include <unistd.h>

namespace
{
    using sad::ui::IREvent;
    using sad::ui::IREventType;
    using sad::ui::IRNode;
    using sad::ui::UINodeType;
    using sad::ui::UnifiedKeyCode;
    namespace fs = sad::ui::freestanding;

    // ─── علامات الاختبار الآليّ (عقد بوّابة [4م] — لا تُغيَّر) ─────────────
    constexpr const char *MARKER_OK_PREFIX = "DESKTOP_OK";
    constexpr const char *MARKER_GLYPHS_PREFIX = "DESKTOP_GLYPHS";
    constexpr const char *MARKER_SHELL_EXIT = "DESKTOP_SHELL_EXIT";
    constexpr const char *MARKER_FAIL_PREFIX = "DESKTOP_FAIL: ";

    // ─── علامات مشغّل التطبيقات (الشريحة ٢ — إضافيّة، عقد بوّابة [4ن]) ──────
    constexpr const char *MARKER_LAUNCH_PREFIX = "DESKTOP_LAUNCH ";
    constexpr const char *MARKER_APP_EXIT_PREFIX = "DESKTOP_APP_EXIT ";
    constexpr const char *MARKER_RESUMED = "DESKTOP_RESUMED";
    constexpr const char *MARKER_LAUNCH_FAIL_PREFIX = "DESKTOP_LAUNCH_FAIL: ";

    // ─── علامة طلب الإطفاء (الشريحة ٣ — إضافيّة، عقد بوّابة [4س]) ───────────
    constexpr const char *MARKER_SHUTDOWN_REQUEST = "DESKTOP_SHUTDOWN_REQUEST";

    /// (AR) متغيّر البيئة الحامل لمسار التطبيق المُطلَق (يمرّره /init من السجلّ) —
    /// غيابه/فراغه ⇒ لا زرّ إطلاق (غياب بنيويّ يُثبته الاختبار حضورًا وغيابًا).
    constexpr const char *ENV_APP_PATH = "SAD_DESKTOP_APP";

    /// (AR) رمز خروج الابن حين يعود execl (تعذّر استبدال صورة العمليّة) — اصطلاح شائع.
    constexpr int CHILD_EXEC_FAIL_CODE = 127;

    /// (AR) رمز الخروج «اخرج للصدَفة» — الاتّفاقيّة المتعاقد عليها مع /init في
    /// sad-os (جذر/بناء-جذر-صدفة-ص.sh): 7 = تسليم نظيف للصدَفة النصّيّة
    constexpr int EXIT_CODE_SHELL = 7;

    /// (AR) رمز الخروج «أطفئ النظام» (الشريحة ٣) — عقد مع /init: 8 = طلب إطفاء صريح
    /// ⇒ الصدَفة الفرعيّة تخرج بلا sad-repl، فيواصل init لمسار الإطفاء (تعريب ٢٦).
    constexpr int EXIT_CODE_SHUTDOWN = 8;

    /// (AR) رسالتا تحميل الخطّ (نفس نمط fb_demo — تصلان وجهة التشخيص)
    constexpr const char *MSG_FONT_LOADED_PREFIX = "DESKTOP_FONT_LOADED ";
    constexpr const char *MSG_FONT_FALLBACK_PREFIX = "DESKTOP_FONT_FALLBACK ";

    // ─── مفاتيح خصائص IR غير المشمولة بـprop_keys.h (نفس ثوابت fb_demo) ────
    constexpr const char *PROP_BG_COLOR = "لون_خلفية";
    constexpr const char *PROP_TEXT_COLOR = "لون";
    constexpr const char *PROP_FONT_SIZE = "حجم_خط";
    constexpr const char *PROP_PADDING = sad::ui::props::PADDING; // SoT: القانونيّ «حشوة»
    constexpr const char *PROP_SPACING = "تباعد";
    constexpr const char *PROP_BTN_TEXT = "نص";

    // ─── الألوان (قرار ق٢: خلفيّة زرقاء داكنة COLOR_ROOT_BG القائم) ────────
    constexpr const char *COLOR_ROOT_BG = "#123A6E";   ///< خلفيّة سطح المكتب
    constexpr const char *COLOR_TOPBAR_BG = "#0B2447"; ///< شريط علويّ أدكن
    constexpr const char *COLOR_TEXT_LIGHT = "#FFFFFF";
    constexpr const char *COLOR_TEXT_DIM = "#B8C6DB";  ///< النصّ الإرشاديّ
    constexpr const char *COLOR_BTN_TERMINAL = "#C0392B"; ///< زرّ ‹الطرفيّة›
    constexpr const char *COLOR_BTN_DEMO = "#1E824C";     ///< زرّ ‹العرض التجريبيّ› (أخضر مميّز)
    constexpr const char *COLOR_BTN_SHUTDOWN = "#7A1F1F"; ///< زرّ ‹إطفاء› (أحمر داكن مميّز)

    // ─── نصوص سطح المكتب (ثوابت مسمّاة — المذكّرة ٣.٢) ────────────────────
    /// (AR) عنوان سطح المكتب (عربيّ مشكَّل — يُثبته عدّاد الغليفات)
    constexpr const char *DESKTOP_TITLE_AR = "سطح مكتب نظام ص";
    /// (AR) النصّ الإرشاديّ (نصّ المذكّرة ٣.٢ حرفيًّا)
    constexpr const char *DESKTOP_HINT_AR =
        "اضغط زرّ ‹الطرفيّة› أو F2 للصدَفة النصّيّة";
    /// (AR) نصّ زرّ الخروج للصدَفة
    constexpr const char *BTN_TERMINAL_TEXT = "الطرفيّة";
    /// (AR) نصّ زرّ إطلاق العرض التجريبيّ (الشريحة ٢ — مذكّرة ٣.٣)
    constexpr const char *BTN_DEMO_TEXT = "العرض التجريبيّ";
    /// (AR) نصّ زرّ إطفاء النظام (الشريحة ٣ — مذكّرة ٣.٣)
    constexpr const char *BTN_SHUTDOWN_TEXT = "إطفاء";
    /// (AR) فاصل الشريط العلويّ بين الساعة واسم المضيف (حدّ معلَن: سطر واحد)
    constexpr const char *TOPBAR_SEPARATOR = "  \xC2\xB7  "; // « · »
    /// (AR) بديل اسم المضيف حين يتعذّر /proc (فشل ناعم لا صمت)
    constexpr const char *HOSTNAME_FALLBACK = "؟";
    /// (AR) مصدر اسم المضيف (واجهة النواة — القيمة نفسها من SoT ‏sad-os)
    constexpr const char *HOSTNAME_PROC_PATH = "/proc/sys/kernel/hostname";

    /// (AR) تعبير حدث الخروج للصدَفة (العقد بين الشجرة وfireEvent هنا)
    constexpr const char *EXPR_EXIT_TO_SHELL = "اخرج_للصدفة";
    /// (AR) تعبير حدث إطلاق العرض التجريبيّ (العقد بين الشجرة وfireEvent هنا)
    constexpr const char *EXPR_LAUNCH_DEMO = "شغّل_العرض";
    /// (AR) تعبير حدث إطفاء النظام (العقد بين الشجرة وfireEvent هنا)
    constexpr const char *EXPR_SHUTDOWN = "اطفئ_النظام";

    // ─── قياسات التخطيط ─────────────────────────────────────────────────
    constexpr double TOPBAR_HEIGHT = 48.0;
    constexpr double TOPBAR_PADDING = 12.0;
    constexpr double TOPBAR_FONT_SIZE = 20.0;
    constexpr double TITLE_FONT_SIZE = 48.0;
    constexpr double HINT_FONT_SIZE = 24.0;
    constexpr double BTN_FONT_SIZE = 32.0;
    constexpr double BTN_HEIGHT = 96.0;
    constexpr double ROOT_SPACING = 48.0;
    constexpr double BODY_PADDING = 64.0;

    /// (AR) صيغة الساعة (قرار ق٤: HH:MM أرقام غربيّة — يتوارث تعريب ٨)
    constexpr const char *CLOCK_FORMAT = "%02d:%02d";

    int fail(const std::string &reason)
    {
        std::cout << MARKER_FAIL_PREFIX << reason << std::endl;
        return 1;
    }

    /// (AR) اسم المضيف من واجهة النواة (سطر واحد مشذَّب) — فشل ناعم بالبديل
    std::string readHostname()
    {
        std::ifstream in(HOSTNAME_PROC_PATH);
        std::string name;
        if (!in || !std::getline(in, name) || name.empty())
            return HOSTNAME_FALLBACK;
        return name;
    }

    /// (AR) نصّ الساعة HH:MM من localtime (بلا TZ في initramfs ⇒ UTC فعليًّا)
    std::string clockText(int *outMinuteStamp)
    {
        const std::time_t now = std::time(nullptr);
        std::tm tmBuf{};
        localtime_r(&now, &tmBuf);
        if (outMinuteStamp)
            *outMinuteStamp = tmBuf.tm_hour * 60 + tmBuf.tm_min;
        char buf[8];
        std::snprintf(buf, sizeof(buf), CLOCK_FORMAT, tmBuf.tm_hour, tmBuf.tm_min);
        return std::string(buf);
    }

    /// (AR) سطر الشريط العلويّ: الساعة + فاصل + اسم المضيف (حدّ الشريحة ١)
    std::string topBarText(const std::string &clock, const std::string &hostname)
    {
        return clock + TOPBAR_SEPARATOR + hostname;
    }

    std::shared_ptr<IRNode> makeText(const std::string &content, double fontSize,
                                     const char *color)
    {
        auto node = IRNode::create(UINodeType::Text);
        node->setProperty(sad::ui::props::TEXT, content);
        node->setProperty(PROP_FONT_SIZE, fontSize);
        node->setProperty(PROP_TEXT_COLOR, std::string(color));
        return node;
    }

    /// (AR) نتاج بناء الشجرة: الجذر + عقدة نصّ الشريط (تُحدَّث كلّ دقيقة)
    struct DesktopTree
    {
        std::shared_ptr<IRNode> root;
        std::shared_ptr<IRNode> topBarLabel;
    };

    /// (AR) شجرة سطح المكتب: شريط علويّ + عنوان + نصّ إرشاديّ + [زرّ ‹العرض التجريبيّ›]
    /// + زرّ ‹الطرفيّة›. زرّ العرض يُضاف فقط حين appLaunchPath غير فارغ (الشريحة ٢).
    DesktopTree buildDesktopTree(uint32_t screenW, uint32_t screenH,
                                 const std::string &topBarInitialText,
                                 const std::string &appLaunchPath)
    {
        DesktopTree tree;

        auto root = IRNode::create(UINodeType::Column);
        root->setProperty(sad::ui::props::WIDTH, static_cast<double>(screenW));
        root->setProperty(sad::ui::props::HEIGHT, static_cast<double>(screenH));
        root->setProperty(PROP_BG_COLOR, std::string(COLOR_ROOT_BG));
        root->setProperty(PROP_SPACING, ROOT_SPACING);

        // الشريط العلويّ (ساعة + اسم مضيف):
        auto topBar = IRNode::create(UINodeType::Container);
        topBar->setProperty(sad::ui::props::WIDTH, static_cast<double>(screenW));
        topBar->setProperty(sad::ui::props::HEIGHT, TOPBAR_HEIGHT);
        topBar->setProperty(PROP_BG_COLOR, std::string(COLOR_TOPBAR_BG));
        topBar->setProperty(PROP_PADDING, TOPBAR_PADDING);
        auto topBarLabel = makeText(topBarInitialText, TOPBAR_FONT_SIZE, COLOR_TEXT_LIGHT);
        topBar->addChild(topBarLabel);
        root->addChild(topBar);

        // الجسد: عنوان + نصّ إرشاديّ + [زرّ عرض اختياريّ] + زرّ طرفيّة + زرّ إطفاء (عمود بحشو).
        // اتّساع التخطيط عند 1024×768 (الحالة القصوى بثلاثة أزرار — مُتحقَّق حسابيًّا):
        //   ارتفاع الجسد المرئيّ = screenH − TOPBAR_HEIGHT = 720، وأعلاه عند y=96
        //   (TOPBAR_HEIGHT + ROOT_SPACING). المحتوى من childY=BODY_PADDING(64):
        //     عنوان (TITLE_FONT_SIZE×1.5 = 72) + تباعد 48
        //     تلميح (HINT_FONT_SIZE×1.5 = 36) + تباعد 48
        //     زرّ العرض  (BTN_HEIGHT 96) top=364 مركز(512,412) + تباعد 48
        //     زرّ الطرفيّة (96)          top=508 مركز(512,556) + تباعد 48
        //     زرّ الإطفاء  (96)          top=652 مركز(512,700) أسفله 748 < 768 ✔
        //   الأزرار الثلاثة تتّسع كلّها داخل الشاشة بهامش 20 بكسل ⇒ لا تقليل لأيّ ثابت.
        auto body = IRNode::create(UINodeType::Column);
        body->setProperty(sad::ui::props::WIDTH, static_cast<double>(screenW));
        body->setProperty(sad::ui::props::HEIGHT,
                          static_cast<double>(screenH) - TOPBAR_HEIGHT);
        body->setProperty(PROP_PADDING, BODY_PADDING);
        body->setProperty(PROP_SPACING, ROOT_SPACING);

        body->addChild(makeText(DESKTOP_TITLE_AR, TITLE_FONT_SIZE, COLOR_TEXT_LIGHT));
        body->addChild(makeText(DESKTOP_HINT_AR, HINT_FONT_SIZE, COLOR_TEXT_DIM));

        // زرّ العرض التجريبيّ (الشريحة ٢) — فوق زرّ ‹الطرفيّة›، وحضورُه مشروط بمسار
        // تطبيق صالح مُمرَّر عبر البيئة (غيابه ⇒ غياب بنيويّ للزرّ يُثبته الاختبار):
        if (!appLaunchPath.empty())
        {
            auto demoBtn = IRNode::create(UINodeType::Button);
            demoBtn->setProperty(sad::ui::props::WIDTH,
                                 static_cast<double>(screenW) - 2.0 * BODY_PADDING);
            demoBtn->setProperty(sad::ui::props::HEIGHT, BTN_HEIGHT);
            demoBtn->setProperty(PROP_BG_COLOR, std::string(COLOR_BTN_DEMO));
            demoBtn->setProperty(PROP_BTN_TEXT, std::string(BTN_DEMO_TEXT));
            demoBtn->setProperty(PROP_FONT_SIZE, BTN_FONT_SIZE);
            IREvent demoTap;
            demoTap.type = IREventType::OnTap;
            demoTap.expression = EXPR_LAUNCH_DEMO;
            demoBtn->addEvent(demoTap);
            body->addChild(demoBtn);
        }

        auto btn = IRNode::create(UINodeType::Button);
        btn->setProperty(sad::ui::props::WIDTH,
                         static_cast<double>(screenW) - 2.0 * BODY_PADDING);
        btn->setProperty(sad::ui::props::HEIGHT, BTN_HEIGHT);
        btn->setProperty(PROP_BG_COLOR, std::string(COLOR_BTN_TERMINAL));
        btn->setProperty(PROP_BTN_TEXT, std::string(BTN_TERMINAL_TEXT));
        btn->setProperty(PROP_FONT_SIZE, BTN_FONT_SIZE);
        IREvent tap;
        tap.type = IREventType::OnTap;
        tap.expression = EXPR_EXIT_TO_SHELL;
        btn->addEvent(tap);
        body->addChild(btn);

        // زرّ الإطفاء (الشريحة ٣) — أسفل زرّ ‹الطرفيّة›، وحاضر دائمًا (الإطفاء قدرة
        // أساسيّة لا يحرسها env بخلاف زرّ العرض): OnTap = EXPR_SHUTDOWN ⇒ خروج بالرمز 8.
        auto shutdownBtn = IRNode::create(UINodeType::Button);
        shutdownBtn->setProperty(sad::ui::props::WIDTH,
                                 static_cast<double>(screenW) - 2.0 * BODY_PADDING);
        shutdownBtn->setProperty(sad::ui::props::HEIGHT, BTN_HEIGHT);
        shutdownBtn->setProperty(PROP_BG_COLOR, std::string(COLOR_BTN_SHUTDOWN));
        shutdownBtn->setProperty(PROP_BTN_TEXT, std::string(BTN_SHUTDOWN_TEXT));
        shutdownBtn->setProperty(PROP_FONT_SIZE, BTN_FONT_SIZE);
        IREvent shutdownTap;
        shutdownTap.type = IREventType::OnTap;
        shutdownTap.expression = EXPR_SHUTDOWN;
        shutdownBtn->addEvent(shutdownTap);
        body->addChild(shutdownBtn);

        root->addChild(body);
        tree.root = root;
        tree.topBarLabel = topBarLabel;
        return tree;
    }

    /**
     * @brief (AR) إطلاق تطبيق من سطح المكتب: تحقّق فشل-مُغلق ثمّ fork/exec/انتظار
     *        مع علامات معلَنة على stdout. لا fork داخل ردّ نداء الحدث — يُستدعى من
     *        الحلقة الرئيسة (تحكّم نظيف).
     * @return true إن أُطلق التطبيق فعلًا (فتلزم إعادة رسم كاملة للسطح)، false عند
     *         فشل ناعم (مسار غير صالح — البقاء على السطح بلا إطلاق).
     */
    bool launchDesktopApp(const std::string &appPath)
    {
        // تحقّق فشل-مُغلق صريح: مسار غير فارغ وقابل للتنفيذ (وإلّا علامة معلَنة):
        if (appPath.empty())
        {
            std::cout << MARKER_LAUNCH_FAIL_PREFIX << "مسار التطبيق فارغ" << std::endl;
            return false;
        }
        if (::access(appPath.c_str(), X_OK) != 0)
        {
            std::cout << MARKER_LAUNCH_FAIL_PREFIX << appPath
                      << " غير موجود أو غير قابل للتنفيذ" << std::endl;
            return false;
        }

        std::cout << MARKER_LAUNCH_PREFIX << appPath << std::endl;
        std::cout.flush();

        const pid_t pid = ::fork();
        if (pid < 0)
        {
            std::cout << MARKER_LAUNCH_FAIL_PREFIX << "تعذّر fork" << std::endl;
            return false;
        }
        if (pid == 0)
        {
            // الابن: استبدال صورة العمليّة بالتطبيق الساكن بلا وسائط.
            // ملاحظة توارث الواصفات (هامش مراجعة أميليا 2026-07-17): واصفات evdev
            // فُتحت بـO_CLOEXEC فتُغلق تلقائيًّا عند execl؛ أمّا واصف /dev/fb0 وmmap
            // فيُورَثان للابن — وهو **مقصود وغير ضارّ**: الابن يفتح fb0 بنفسه ويرسم
            // عليه، والأب محجوب في waitpid فلا رسم متزامن على نفس الجهاز (تسلسليّ
            // بالتصميم)، ثمّ يستعيد الأب السطح بإعادة رسم كاملة بعد عودة الابن.
            ::execl(appPath.c_str(), appPath.c_str(), static_cast<char *>(nullptr));
            ::_exit(CHILD_EXEC_FAIL_CODE); // execl عاد ⇒ فشل الاستبدال
        }

        // الأب: انتظار الابن ثمّ استخراج رمز الخروج وإعلانه:
        int status = 0;
        ::waitpid(pid, &status, 0);
        const int code =
            WIFEXITED(status) ? WEXITSTATUS(status)
                              : (WIFSIGNALED(status) ? 128 + WTERMSIG(status) : -1);
        std::cout << MARKER_APP_EXIT_PREFIX << code << std::endl;
        std::cout.flush();
        return true;
    }

    /**
     * @brief (AR) حلقة سطح المكتب: تبني السياسة (شجرة السطح + ساعة حيّة + إطلاق
     *        تطبيق + علامات الاختبار + الخروج) فوق آليّة الحلقة الحرّة المشتركة
     *        (fs::runFreestandingApp) — الجسر رفيع لا يكرّر منطق fb0/evdev/العرض.
     * @return رمز خروج العمليّة (EXIT_CODE_SHELL / EXIT_CODE_SHUTDOWN / 1 عند الفشل).
     */
    int runDesktop(const std::string &fontPath, const std::string &devicePath,
                   const std::string &appLaunchPath)
    {
        // ─── حالة السطح المشتركة بين ردود النداء (تُبنى في buildRoot) ───
        const std::string hostname = readHostname();
        DesktopTree tree;
        int minuteStamp = -1;

        // (AR) أعلام تُرفع من ردود النداء وتُستهلَك في onIterate (تحكّم نظيف — لا
        //      fork داخل مسار المعالجة، ولا إيقاف للحلقة إلّا من onIterate):
        bool exitRequested = false;     ///< زرّ الطرفيّة أو الإطفاء طلب الخروج
        bool shutdownRequested = false; ///< الإطفاء تحديدًا (الرمز 8 لا 7)
        bool pendingLaunch = false;     ///< نقرة ‹العرض التجريبيّ› (إطلاق مؤجّل)
        bool desktopFail = false;       ///< فشل إعادة رسم بعد عودة التطبيق
        std::string desktopFailMsg;

        fs::FreestandingAppConfig cfg;
        cfg.fontPath = fontPath;
        cfg.devicePath = devicePath;
        cfg.resetGlyphCountOnReady = true; // بوّابة [4م] تقيس غليفات الإطار الأوّل

        // خطّ العرض العربيّ: علامتا التشخيص (نفس عقد fb-demo):
        cfg.onFontResult = [](bool loaded, const std::string &detail)
        {
            if (loaded)
                std::cout << MSG_FONT_LOADED_PREFIX << detail << std::endl;
            else
                std::cout << MSG_FONT_FALLBACK_PREFIX << detail << std::endl;
        };

        // بناء شجرة السطح بالأبعاد الفعليّة (ساعة ابتدائيّة + اسم مضيف + أزرار):
        cfg.buildRoot = [&](uint32_t w, uint32_t h) -> std::shared_ptr<IRNode>
        {
            const std::string initialClock = clockText(&minuteStamp);
            tree = buildDesktopTree(w, h, topBarText(initialClock, hostname),
                                    appLaunchPath);
            return tree.root;
        };

        // بعد الإطار الأوّل: علامتا الجاهزيّة [4م] (OK ثمّ عدّاد الغليفات):
        cfg.onReady = [](const fs::AppLoopContext &ctx)
        {
            fs::FreestandingRenderer *renderer = ctx.window.getFreestandingRenderer();
            std::cout << MARKER_OK_PREFIX << " " << ctx.width << "x" << ctx.height
                      << std::endl;
            std::cout << MARKER_GLYPHS_PREFIX << " "
                      << renderer->presentationGlyphsDrawn() << std::endl;
        };

        // إرسال حدث النقر ⇒ رفع العلَم المناسب (لا إيقاف مباشر للحلقة):
        cfg.onEvent = [&](IREventType type, const std::string &expr,
                          const IRNode * /*node*/)
        {
            if (type != IREventType::OnTap)
                return;
            if (expr == EXPR_EXIT_TO_SHELL)
                exitRequested = true;
            else if (expr == EXPR_LAUNCH_DEMO)
                pendingLaunch = true;
            else if (expr == EXPR_SHUTDOWN)
            {
                exitRequested = true;
                shutdownRequested = true;
            }
        };

        // مفتاح الخروج الموثَّق (المذكّرة ٣.٢): F2 ⇒ الصدَفة النصّيّة (إيقاف الحلقة):
        cfg.onKey = [](UnifiedKeyCode code) -> bool
        { return code == UnifiedKeyCode::F2; };

        // كلّ دورة: إطلاق مؤجّل ⇒ ساعة حيّة ⇒ إيقاف إن طُلب الخروج (زرّ):
        cfg.onIterate = [&](fs::AppLoopContext &ctx) -> bool
        {
            // إطلاق تطبيق معلَّق (نقرة ‹العرض التجريبيّ›): fork/exec/انتظار ثمّ
            // تفريغ الإدخال المتراكم وإعادة رسم كاملة لاستعادة السطح (الشريحة ٢):
            if (pendingLaunch)
            {
                pendingLaunch = false;
                if (launchDesktopApp(appLaunchPath))
                {
                    ctx.input.drainPending(); // منع إعادة تشغيل نقرة قديمة (الخطر ٤)
                    fs::FreestandingRenderer *r = ctx.window.getFreestandingRenderer();
                    r->resetPresentationGlyphCount();
                    ctx.window.invalidate();
                    if (!ctx.window.runOneFrame())
                    {
                        desktopFail = true;
                        desktopFailMsg = "فشلت إعادة رسم سطح المكتب بعد خروج التطبيق";
                        return false;
                    }
                    std::cout << MARKER_RESUMED << std::endl;
                    std::cout.flush();
                }
                return !exitRequested; // دورة نظيفة (تجاوز فحص الساعة لهذه الدورة)
            }

            // الساعة الحيّة: invalidate واحدة عند تغيّر الدقيقة لا كلّ دورة:
            int nowStamp = -1;
            const std::string nowClock = clockText(&nowStamp);
            if (nowStamp != minuteStamp)
            {
                minuteStamp = nowStamp;
                tree.topBarLabel->setProperty(sad::ui::props::TEXT,
                                              topBarText(nowClock, hostname));
                ctx.window.invalidate();
            }

            return !exitRequested;
        };

        // ─── تشغيل الحلقة المشتركة ───
        std::string runError;
        const int rc = fs::runFreestandingApp(cfg, &runError);
        if (desktopFail)
            return fail(desktopFailMsg);
        if (rc != 0)
            return fail(runError);

        // طلب إطفاء صريح (زرّ ‹إطفاء› — الشريحة ٣): خروج بالرمز 8 ⇒ الصدَفة الفرعيّة
        // تخرج بلا sad-repl فيواصل init مسار الإطفاء النظيف القائم (تعريب ٢٦):
        if (shutdownRequested)
        {
            std::cout << MARKER_SHUTDOWN_REQUEST << std::endl;
            return EXIT_CODE_SHUTDOWN;
        }

        // خروج مقصود للصدَفة النصّيّة (زرّ الطرفيّة أو F2) — التسليم النظيف عقدُ init:
        std::cout << MARKER_SHELL_EXIT << std::endl;
        return EXIT_CODE_SHELL;
    }
} // namespace

int main(int argc, char **argv)
{
    // (AR) الوسائط: [1] مسار خطّ PSF (اختياريّ)، [2] مسار جهاز fb (اختياريّ)
    std::string fontPath;
    std::string devicePath = fs::linuxfb::DEFAULT_FB_DEVICE_PATH;
    if (argc > 1)
        fontPath = argv[1];
    if (argc > 2)
        devicePath = argv[2];

    // (AR) مسار التطبيق المُطلَق من البيئة (يمرّره /init من السجلّ) — غيابه/فراغه
    // ⇒ لا زرّ إطلاق (غياب بنيويّ). صلاحيّة المسار تُتحقَّق وقت الإطلاق لا وقت البناء:
    std::string appLaunchPath;
    if (const char *envApp = std::getenv(ENV_APP_PATH))
        appLaunchPath = envApp;

    // الجسر رفيع: كلّ آليّة fb0/evdev/العرض في fs::runFreestandingApp عبر runDesktop.
    return runDesktop(fontPath, devicePath, appLaunchPath);
}

#else // !__linux__

#include <iostream>

int main()
{
    // (AR) سطح المكتب لينكسيّ حصرًا (fbdev) — فشل-مُغلق صريح على غيره:
    std::cout << "DESKTOP_FAIL: sad-desktop يعمل على لينكس فقط (/dev/fb0)" << std::endl;
    return 1;
}

#endif // __linux__
