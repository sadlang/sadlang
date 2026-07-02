/**
 * ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
 * …„: ui_bridge.cpp
 * ״§„…״³״§״±: interpreter/src/ui/ui_bridge.cpp
 * ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
 *
 * ״§„ˆ״µ:
 * ------
 * ״×†״° ״¬״³״± ״§„״±״¨״· ״¨† ״§„…״³״± ˆ†״¸״§… ˆ״§״¬‡״§״× ״µ (SadUI).
 *
 * ״­‚ˆ‚ ״§„†״´״± (c) 2024-2026 ״±‚ „״÷״© ״µ
 * …״±״®״µ ״×״­״× ״±״®״µ״© MIT
 * ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
 */

#include "ui_bridge.h"
#include "interpreter_core.h"
#include "object_instance.h"
#include "class_manager.h"    // (AR) لفحص isUIComponent / (EN) For isUIComponent check
#include "ui_state_manager.h" // (AR) مدير الحالة التفاعلية / (EN) Reactive state manager
#include "widget_builder.h"   // (AR) دعم WidgetBuilder → استخراج IRNode مباشرة / (EN) WidgetBuilder → extract IRNode directly

#include "sad_ui/types.h"
#include "sad_ui/ir.h"
#include "sad_ui/ir_builder.h"
#include "sad_ui/layout.h"
#include "sad_ui/state.h"

#ifdef SAD_UI_USE_SDL2
#include "sad_ui/desktop/window.h"
#include "sad_ui/desktop/renderer.h"
#include <SDL.h>
#endif

#if defined(SAD_WASM_BUILD)
#include "sad_ui/web/html_codegen.h"
#endif

#if defined(SAD_FREESTANDING)
#include "sad_ui/freestanding/renderer.h"
#endif

#if defined(SAD_PLATFORM_IOS)
#include "sad_ui/ios/swiftui_codegen.h"
#endif

#if defined(SAD_PLATFORM_MACOS)
#include "sad_ui/macos/appkit_codegen.h"
#endif

#ifdef SAD_PLATFORM_ANDROID
#include "native_ui_storage.h"
#endif

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#include <commdlg.h>
#include <shlobj.h>
#elif defined(__APPLE__)
#include <sys/sysctl.h>
#endif

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <chrono>
#include <ctime>
#include <algorithm>

namespace Sad
{
    namespace Interpreter
    {

        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
        // ״×†״° UIBridge
        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

        UIBridge::UIBridge() = default;
        UIBridge::~UIBridge()
        {
            if (activeInstance_ == this)
            {
                activeInstance_ = nullptr;
            }
        }

        // ״§„…״«„ ״§„†״´״·
        UIBridge *UIBridge::activeInstance_ = nullptr;

        // (AR) HTML ״§„…ˆ„‘״¯ ג€” ״®״²‘† ‡†״§ „״§״³״×״±״¬״§״¹‡ …† WASM
        // (EN) Generated HTML ג€” stored here for WASM retrieval
        static std::string s_lastGeneratedHtml;

        bool UIBridge::run(const Data::Value &rootWidget, Interpreter *interpreter)
        {
            interpreter_ = interpreter;
            activeInstance_ = this; // ״×״¹† ״§„…״«„ ״§„†״´״·

            // (AR) ״¯״¹… ״¯״§„״© ״§„״¨†‘״§״¡: ״¥״°״§ ƒ״§† ״§„״¬״°״± ״¯״§„״© (ˆ„״³ ƒ״§״¦† ˆ״§״¬‡״©)״ †״­״¸‡״§ ƒ״¨†‘״§״¡
            //      ˆ†״³״×״¯״¹‡״§ „„״­״µˆ„ ״¹„‰ ״§„״´״¬״±״© ״§„״£ˆ„״©
            // (EN) Builder function support: if root is a function, call it once for the initial tree;
            Data::Value actualWidget;
            if (rootWidget.isFunction())
            {
                // (Amelia LOW-2) أُزيل builderFunc_ (لا قارئ بعد التوحيد؛ الرسم من nav).
                try
                {
                    auto funcRef = rootWidget.toFunction();
                    if (funcRef)
                    {
                        // (AR) استدعِ باني الجذر مرّةً للحصول على شجرة العرض الأوّليّة.
                        actualWidget = interpreter_->callUserFunction(funcRef->registeredName, {});
                    }
                }
                catch (const std::exception &e)
                {
                    std::cerr << "״®״·״£  ״§״³״×״¯״¹״§״¡ ״¯״§„״© ״§„״¨†‘״§״¡: " << e.what() << std::endl;
                    return false;
                }
            }
            else
            {
                actualWidget = rootWidget;
            }

            // (توحيد كامل، LOW-1) ابذر الجذر الأصليّ (rootWidget، دالّةً كان أو عنصرًا)
            //   في مكدّس nav ⇒ يصير مصدرَ الرسم الحيّ. نمرّر rootWidget لا actualWidget
            //   كي يُخزَّن البانِي نفسه (تفاعليّة الجذر) لا لقطته الأولى.
            navSeedRoot(rootWidget, interpreter);
            rootWidget_ = actualWidget; // (AR) لقطةُ الجذر: محتوى العرض الأوّل + احتياطُ rebuildUI الآمن

            // ״×״­…„ ״§„״×״®״²† ״§„…״­„ …† ״§„‚״±״µ
            loadStorageFromDisk();
            // ״×״­ˆ„ ״´״¬״±״© Widget ״¥„‰ ״´״¬״±״© IR
            auto irRoot = convertToIR(actualWidget);
            if (!irRoot)
            {
                std::cerr << "״´„ ״×״­ˆ„ ״´״¬״±״© ״§„ˆ״§״¬‡״© ״¥„‰ IR" << std::endl;
                return false;
            }

#ifdef SAD_UI_USE_SDL2
            // ״¥†״´״§״¡ †״§״°״© ״³״·״­ ״§„…ƒ״×״¨
            sad::ui::desktop::DesktopWindow window;
            activeWindow_ = &window; // ״­״¸ …״₪״´״± ״§„†״§״°״© „״¥״¹״§״¯״© ״§„״¨†״§״¡
            sad::ui::desktop::WindowOptions options;

            // ‚״±״§״¡״© ״¹†ˆ״§† ״§„†״§״°״© …† IR ״¥״°״§ ˆ״¬״¯ (…† ״®״§״µ״© "״¹†ˆ״§†"  ״§„״¬״°״±)
            const auto *titleProp = irRoot->findProperty("״¹†ˆ״§†");
            if (titleProp)
            {
                if (auto *str = std::get_if<std::string>(&titleProp->value))
                {
                    options.title = *str;
                }
            }
            else
            {
                options.title = "״×״·״¨‚ ״µ";
            }

            // ‚״±״§״¡״© ״£״¨״¹״§״¯ ״§„†״§״°״© ״¥״°״§ ˆ״¬״¯״×
            const auto *widthProp = irRoot->findProperty("״¹״±״¶");
            const auto *heightProp = irRoot->findProperty("״§״±״×״§״¹");
            if (widthProp)
            {
                if (auto *num = std::get_if<int64_t>(&widthProp->value))
                {
                    options.width = static_cast<int>(*num);
                }
                else if (auto *dbl = std::get_if<double>(&widthProp->value))
                {
                    options.width = static_cast<int>(*dbl);
                }
            }
            if (heightProp)
            {
                if (auto *num = std::get_if<int64_t>(&heightProp->value))
                {
                    options.height = static_cast<int>(*num);
                }
                else if (auto *dbl = std::get_if<double>(&heightProp->value))
                {
                    options.height = static_cast<int>(*dbl);
                }
            }

            if (!window.create(options))
            {
                std::cerr << "״´„ ״¥†״´״§״¡ ״§„†״§״°״©" << std::endl;
                return false;
            }

            // ״×״¹† ״§„…״­״×ˆ‰
            window.setContent(irRoot);

            // (AR) حفظ الشجرة الأولية كمرجع لـ Reconciler — أول rebuildUI سيستخدمها للمقارنة الذكية
            // (EN) Store initial IR tree as reference for Reconciler — first rebuildUI uses it for smart diffing
            previousIR_ = irRoot;

            // (AR) ربط UIStateManager بإعادة بناء الواجهة
            //      عند تغيير أي حقل @حالة، سيُستدعى rebuildUI تلقائياً
            // (EN) Connect UIStateManager to UI rebuild
            //      When any @state field changes, rebuildUI is called automatically
            UIStateManager::instance().setRebuildCallback([this]()
                                                          { rebuildUI(); });

            // ״×״¹† callback „„״£״­״¯״§״«
            window.setOnEventCallback(
                [this](sad::ui::IREventType eventType,
                       const std::string &elementId,
                       const sad::ui::IRNode *node,
                       const sad::ui::EventData &eventData)
                {
                    handleEvent(eventType, elementId, node, eventData);
                });

            // ״×״³״¬„ callback ״§„…״₪‚״×״§״× ג€” ״³״×״¯״¹‰ ƒ„ ״¥״·״§״±
            window.setTimerUpdateCallback([this]()
                                          {
        updateTimers();
        // (AR) تحديث الحركات كل إطار — محرك موحّد
        // (EN) Update animations every frame — unified engine
        updateAnimations();
        // (AR) تنفيذ إعادة بناء تفاعلية معلّقة (حقول @حالة)
        // (EN) Flush pending reactive rebuilds (@state fields)
        UIStateManager::instance().flush();
        // (AR) حص إعادة التحميل الحي كل إطار
        // (EN) Poll hot reload every frame
        pollHotReload(); });

            // ״×״³״¬„ callback ״¥„״§״× ״§„…„״§״×
            window.setDropFileCallback([this](const std::string &filePath)
                                       { handleFileDrop(filePath); });

            // ״×״´״÷„ ״­„‚״© ״§„״£״­״¯״§״«
            window.run();

            // ״×†״¸
            activeWindow_ = nullptr;
            activeInstance_ = nullptr;
            window.destroy();

            return true;
#else
#if defined(SAD_WASM_BUILD)
            // (AR) ˆ״¶״¹ WASM ג€” ״×ˆ„״¯ HTML …† ״´״¬״±״© IR ״¨״§״³״×״®״¯״§… HtmlCodegen
            // (EN) WASM mode ג€” generate HTML from IR tree using HtmlCodegen
            {
                sad::ui::IRModule module;
                module.name = "ˆ״­״¯״©_ˆ״§״¬‡״©";
                module.root = irRoot;

                sad::ui::web::HtmlCodegenOptions htmlOpts;
                htmlOpts.title = "״×״·״¨‚ ״µ";
                htmlOpts.dir = "rtl";
                htmlOpts.lang = "ar";

                // ‚״±״§״¡״© ״¹†ˆ״§† ״§„†״§״°״© …† IR ״¥״°״§ ˆ״¬״¯
                const auto *titleProp = irRoot->findProperty("״¹†ˆ״§†");
                if (titleProp)
                {
                    if (auto *str = std::get_if<std::string>(&titleProp->value))
                    {
                        htmlOpts.title = *str;
                    }
                }

                sad::ui::web::HtmlCodegen codegen(htmlOpts);
                s_lastGeneratedHtml = codegen.generate(module);

                // ״·״¨״§״¹״© ״¹„״§…״© ״®״§״µ״© „״×״¹״± ״¹„‡״§ runtime
                std::cout << "<!--SAD_UI_HTML_START-->" << std::endl;
                std::cout << s_lastGeneratedHtml << std::endl;
                std::cout << "<!--SAD_UI_HTML_END-->" << std::endl;
            }
            return true;
#elif defined(SAD_PLATFORM_ANDROID)
            // (AR) وضع أندرويد — تخزين شجرة IR للعرض الأصلي عبر JNI
            // (EN) Android mode — store IR tree for native rendering via JNI
            {
                auto module = std::make_shared<sad::ui::IRModule>();
                module->name = "\xd9\x88\xd8\xad\xd8\xaf\xd8\xa9_\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9";
                module->root = irRoot;
                sad_native_ui_store(module);
                std::cout << "<!--SAD_NATIVE_UI_READY-->" << std::endl;
            }
            return true;
#elif defined(SAD_FREESTANDING)
            // (AR) وضع النواة/Freestanding — رسم مباشر على الـ framebuffer
            // (EN) Freestanding mode — direct framebuffer rendering (kernel/bare-metal)
            {
                sad::ui::freestanding::FreestandingRenderer renderer;

                // (AR) قراءة إعدادات framebuffer من خصائص IR (عرض, ارتفاع, عمق)
                int fbWidth = 1024, fbHeight = 768, fbBpp = 32;
                const auto *wProp = irRoot->findProperty("\xd8\xb9\xd8\xb1\xd8\xb6");                         // عرض
                const auto *hProp = irRoot->findProperty("\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9"); // ارتفاع
                if (wProp)
                {
                    if (auto *n = std::get_if<int64_t>(&wProp->value))
                        fbWidth = static_cast<int>(*n);
                    else if (auto *d = std::get_if<double>(&wProp->value))
                        fbWidth = static_cast<int>(*d);
                }
                if (hProp)
                {
                    if (auto *n = std::get_if<int64_t>(&hProp->value))
                        fbHeight = static_cast<int>(*n);
                    else if (auto *d = std::get_if<double>(&hProp->value))
                        fbHeight = static_cast<int>(*d);
                }

                // (AR) تهيئة framebuffer — يعتمد على البيئة المستهدفة (UEFI GOP, VGA, ...)
                sad::ui::freestanding::FramebufferConfig fbConfig;
                fbConfig.width = static_cast<uint32_t>(fbWidth);
                fbConfig.height = static_cast<uint32_t>(fbHeight);
                fbConfig.bpp = static_cast<uint32_t>(fbBpp);
                fbConfig.pitch = fbConfig.width * (fbConfig.bpp / 8);
                // (AR) العنوان الفعلي يُعيَّن من النواة — هنا نضع nullptr كقيمة افتراضية
                // (EN) Actual address set by kernel — nullptr as default placeholder
                fbConfig.address = nullptr;

                sad::ui::freestanding::FreestandingRenderOptions fbOpts;
                renderer.initializeFramebuffer(fbConfig, fbOpts);

                // (AR) تصيير الشجرة — في النواة الفعلية يُستدعى من حلقة الأحداث
                renderer.renderTree(irRoot.get());

                std::cout << "<!--SAD_FREESTANDING_UI_READY-->" << std::endl;
            }
            return true;
#elif defined(SAD_PLATFORM_IOS)
            // (AR) وضع iOS — توليد كود SwiftUI من شجرة IR
            // (EN) iOS mode — generate SwiftUI code from IR tree
            {
                sad::ui::IRModule module;
                module.name = "\xd9\x88\xd8\xad\xd8\xaf\xd8\xa9_\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9";
                module.root = irRoot;

                sad::ui::ios::SwiftUICodegenOptions swiftOpts;
                swiftOpts.generatePreview = true;

                sad::ui::ios::SwiftUICodegen codegen(swiftOpts);
                std::string swiftCode = codegen.generate(module);

                std::cout << "<!--SAD_SWIFTUI_START-->" << std::endl;
                std::cout << swiftCode << std::endl;
                std::cout << "<!--SAD_SWIFTUI_END-->" << std::endl;
            }
            return true;
#elif defined(SAD_PLATFORM_MACOS)
            // (AR) وضع macOS — توليد كود AppKit من شجرة IR
            // (EN) macOS mode — generate AppKit code from IR tree
            {
                sad::ui::IRModule module;
                module.name = "\xd9\x88\xd8\xad\xd8\xaf\xd8\xa9_\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9";
                module.root = irRoot;

                sad::ui::macos::AppKitCodegenOptions appkitOpts;

                // (AR) قراءة عنوان النافذة من IR إذا وُجد
                const auto *titleProp = irRoot->findProperty("\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86");
                if (titleProp)
                {
                    if (auto *str = std::get_if<std::string>(&titleProp->value))
                    {
                        appkitOpts.windowTitle = *str;
                    }
                }

                sad::ui::macos::AppKitCodegen codegen(appkitOpts);
                std::string appkitCode = codegen.generate(module);

                std::cout << "<!--SAD_APPKIT_START-->" << std::endl;
                std::cout << appkitCode << std::endl;
                std::cout << "<!--SAD_APPKIT_END-->" << std::endl;
            }
            return true;
#else
            // SDL2 ״÷״± …״¹‘„ ג€” ״·״¨״§״¹״© ״´״¬״±״© IR ‚״·
            std::cout << "=== ״´״¬״±״© IR ===" << std::endl;
            std::cout << irRoot->dump() << std::endl;
            std::cout << "=== ״§†״×‡‰ ===" << std::endl;
            std::cout << "[״×״­״°״±] SDL2 ״÷״± …״¹‘„ ג€” ״§„ˆ״§״¬‡״© „† ״×״¹״±״¶ ״±״³ˆ…״§‹" << std::endl;
            std::cout << "„״×״¹„ SDL2״ ״«״¨‘״× SDL2 ״¹„‰ ״§„†״¸״§… ״«… ״£״¹״¯ ״§„״¨†״§״¡" << std::endl;
            return true;
#endif
#endif
        }

        std::shared_ptr<sad::ui::IRNode> UIBridge::convertToIR(const Data::Value &widget)
        {
            auto result = convertNodeToIR(widget, 0);
            return result;
        }

        std::shared_ptr<sad::ui::IRNode> UIBridge::convertNodeToIR(
            const Data::Value &widget, int depth)
        {
            constexpr int MAX_DEPTH = 100;
            if (depth > MAX_DEPTH)
            {
                std::cerr << "״×״­״°״±: ˆ״µ„״× ״´״¬״±״© ״§„ˆ״§״¬‡״© „„״­״¯ ״§„״£‚״µ‰ …† ״§„״¹…‚" << std::endl;
                return nullptr;
            }

            // ג”€ג”€ג”€ ƒ״§״¦† (†״³״®״© ״µ†) ג”€ג”€ג”€
            if (widget.isObject())
            {
                auto obj = widget.toObject();
                if (!obj)
                    return nullptr;
                // =================================================================
                // (AR) دعم WidgetBuilder: إذا كان الكائن WidgetBuilder، نستخرج
                //      IRNode المضمّن مباشرة — بدون أي تحويل وسيط
                // (EN) WidgetBuilder support: extract embedded IRNode directly
                // =================================================================
                if (isWidgetBuilder(obj))
                {
                    auto *wb = static_cast<Sad::Interpreter::WidgetBuilder *>(obj);
                    auto irNode = wb->getIRNode();

                    // ═══════════════════════════════════════════════════════════
                    // (AR) تسجيل الأحداث المخزنة على WidgetBuilder كمعالجات
                    //      في eventHandlers_ ليتمكن handleEvent من استدعائها
                    //      الأحداث مخزنة كحقول بأسماء __event_عند_النقر...
                    // (EN) Register stored events from WidgetBuilder as handlers
                    //      in eventHandlers_ so handleEvent can invoke them
                    // ═══════════════════════════════════════════════════════════
                    for (auto &[key, val] : wb->fields)
                    {
                        if (key.rfind("__event_", 0) == 0)
                        {
                            // (AR) استخراج owner من حقل __owner إن وجد
                            Data::ObjectInstance *ownerPtr = nullptr;
                            auto *ownerField = wb->getField("__owner");
                            if (ownerField && ownerField->isObject())
                            {
                                ownerPtr = ownerField->toObject();
                            }

                            // (AR) توليد معرّف مستقر: nodeId + ":" + eventKey
                            //      هذا يضمن أن نفس العقدة بنفس الحدث تحصل على نفس المعرّف
                            //      عبر كل إعادات البناء — مما يمنع تغييرات وهمية في Reconciler
                            // (EN) Generate stable ID: nodeId + ":" + eventKey
                            //      Ensures same node+event gets same ID across rebuilds
                            std::string stableId = irNode->getId() + ":" + key;
                            std::string handlerId = registerHandler(val, ownerPtr, stableId);

                            // (AR) تحديث معرّف الحدث في IRNode
                            auto &events = irNode->getMutableEvents();
                            for (auto &evt : events)
                            {
                                if (evt.expression == key)
                                {
                                    evt.expression = handlerId;
                                    break;
                                }
                            }
                        }
                    }

                    // (AR) تسجيل أحداث الأبناء إن وجدت
                    registerWidgetBuilderEvents(irNode, wb);

                    return irNode;
                }

                // ‚״±״§״¡״© _†ˆ״¹
                std::string widgetType = obj->getClassName();                        // ״§״³… ״§„״µ† ƒ†ˆ״¹ ״§״×״±״§״¶
                Data::Value *typeField = obj->getField("_\xd9\x86\xd9\x88\xd8\xb9"); // _†ˆ״¹
                if (typeField && typeField->isString())
                {
                    widgetType = typeField->toString();
                }
                // ═══════════════════════════════════════════════════════════════════
                // (AR) دعم حالة_عنصر (StatefulWidget):
                //      إذا كان _نوع == "حالة"، نستدعي بناء() على الكائن
                //      لنحصل على شجرة العناصر الفعلية ونحوّلها بدلاً منه
                // (EN) StatefulWidget support:
                //      If _نوع == "حالة", call بناء() on the object
                //      to get the actual widget tree and convert that instead
                // ═══════════════════════════════════════════════════════════════════
                if (widgetType == "\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9" && interpreter_)
                { // حالة
                    try
                    {
                        // (AR) استدعاء دالة بناء() على كائن حالة_عنصر
                        // (EN) Call بناء() on the StatefulWidget object
                        auto *interp = const_cast<Sad::Interpreter::Interpreter *>(
                            reinterpret_cast<const Sad::Interpreter::Interpreter *>(interpreter_));
                        Data::Value buildResult = interp->callMethodOnObject(
                            obj, "\xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1"); // بناء

                        if (!buildResult.isVoid())
                        {
                            // (AR) حفظ مرجع كائن الحالة للإعادة بناء لاحقاً
                            // (EN) Store state object reference for later rebuilds
                            if (depth == 0)
                            {
                                rootWidget_ = widget;
                            }
                            // (AR) تحويل الشجرة المُرجعة من بناء() إلى IR
                            // (EN) Convert the tree returned from بناء() to IR
                            return convertNodeToIR(buildResult, depth + 1);
                        }
                    }
                    catch (const std::exception &e)
                    {
                        std::cerr << "[\xd8\xae\xd8\xb7\xd8\xa3] \xd8\xa7\xd8\xb3\xd8\xaa\xd8\xaf\xd8\xb9\xd8\xa7\xd8\xa1 \xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1(): "
                                  << e.what() << std::endl;
                        // [خطأ] استدعاء بناء():
                    }
                }

                // ═══════════════════════════════════════════════════════════════════
                // (AR) دعم مكونات الواجهة التصريحية (واجهة ... نهاية):
                //      إذا كان الصنف isUIComponent، نستدعي بناء() على الكائن
                //      لنحصل على شجرة العناصر التصريحية (UIWidgetExprNode → Value)
                //
                // (EN) Declarative UI component support (واجهة ... نهاية):
                //      If class isUIComponent, call بناء() on the object
                //      to get the declarative widget tree (UIWidgetExprNode → Value)
                // ═══════════════════════════════════════════════════════════════════
                if (widgetType != "\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9" && interpreter_)
                { // حالة — لم يكن حالة عنصر
                    auto *classManager = Data::ClassManager::getInstance();
                    auto *classType = classManager->getClass(widgetType);
                    if (classType && classType->isUIComponent)
                    {
                        try
                        {
                            auto *interp = const_cast<Sad::Interpreter::Interpreter *>(
                                reinterpret_cast<const Sad::Interpreter::Interpreter *>(interpreter_));
                            Data::Value buildResult = interp->callMethodOnObject(
                                obj, "\xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1"); // بناء

                            if (!buildResult.isVoid())
                            {
                                if (depth == 0)
                                {
                                    rootWidget_ = widget;
                                }
                                return convertNodeToIR(buildResult, depth + 1);
                            }
                        }
                        catch (const std::exception &e)
                        {
                            std::cerr << "[\xd8\xae\xd8\xb7\xd8\xa3] \xd8\xa7\xd8\xb3\xd8\xaa\xd8\xaf\xd8\xb9\xd8\xa7\xd8\xa1 \xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1() \xd9\x85\xd9\x83\xd9\x88\xd9\x86: "
                                      << e.what() << std::endl;
                            // [خطأ] استدعاء بناء() مكون:
                        }
                    }
                }

                // =================================================================
                // (AR) احتياط: كائن عادي ليس WidgetBuilder ولا مكون واجهة
                //      نُنشئ IRNode ونقرأ الحقول كخصائص
                // (EN) Fallback: regular object — create IRNode from fields
                // =================================================================
                {
                    sad::ui::UINodeType nodeType = stringToNodeType(widgetType);
                    auto irNode = sad::ui::IRNode::create(nodeType);
                    irNode->setId(obj->getClassName() + "_" + std::to_string(reinterpret_cast<uintptr_t>(obj)));

                    // (AR) قراءة حقول الكائن مباشرة كخصائص
                    auto fieldNames = obj->getFieldNames();
                    for (const auto &key : fieldNames)
                    {
                        if (key.empty() || key[0] == '_')
                            continue;

                        auto *val = obj->getField(key);
                        if (!val || val->isVoid())
                            continue;

                        if (val->isObject())
                        {
                            auto childNode = convertNodeToIR(*val, depth + 1);
                            if (childNode)
                                irNode->addChild(childNode);
                            continue;
                        }

                        sad::ui::IRProperty prop;
                        prop.key = key;
                        if (val->isInteger())
                            prop.value = val->toInt64();
                        else if (val->isDouble())
                            prop.value = val->toDouble();
                        else if (val->isBoolean())
                            prop.value = val->toBool();
                        else
                            prop.value = val->toString();
                        irNode->addProperty(prop);
                    }

                    return irNode;
                }
            }

            // ג”€ג”€ג”€ †״µ ״¨״³״· ג†’ ״¹‚״¯״© Text ג”€ג”€ג”€
            if (widget.isString())
            {
                auto irNode = sad::ui::IRNode::create(sad::ui::UINodeType::Text);
                sad::ui::IRProperty prop;
                prop.key = "†״µ";
                prop.value = widget.toString();
                irNode->addProperty(prop);
                return irNode;
            }

            return nullptr;
        }

        sad::ui::UINodeType UIBridge::stringToNodeType(const std::string &typeName)
        {
            // ״§״³״×״®״¯״§… ״§„״¯״§„״© ״§„…ˆ״¬ˆ״¯״©  types.h
            auto result = sad::ui::arabicNameToNodeType(typeName);
            if (result.has_value())
            {
                return result.value();
            }

            // …״­״§ˆ„״© ״×״®…† ״§„†ˆ״¹ …† ״§„״§״³…
            static const std::unordered_map<std::string, sad::ui::UINodeType> fallbackMap = {
                // ─── عناصر أساسية ───
                {"\xd9\x86\xd8\xb5", sad::ui::UINodeType::Text},
                {"\xd8\xb2\xd8\xb1", sad::ui::UINodeType::Button},
                {"\xd8\xb5\xd9\x88\xd8\xb1\xd8\xa9", sad::ui::UINodeType::Image},
                {"\xd8\xb9\xd9\x85\xd9\x88\xd8\xaf", sad::ui::UINodeType::Column},
                {"\xd8\xb5\xd9\x81", sad::ui::UINodeType::Row},
                {"\xd8\xad\xd8\xa7\xd9\x88\xd9\x8a\xd8\xa9", sad::ui::UINodeType::Container},
                {"\xd9\x87\xd9\x8a\xd9\x83\xd9\x84", sad::ui::UINodeType::Scaffold},
                {"\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9", sad::ui::UINodeType::Card},
                {"\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9", sad::ui::UINodeType::List},
                {"״£‚ˆ†״©", sad::ui::UINodeType::Icon},
                {"״§‚ˆ†״©", sad::ui::UINodeType::Icon},
                {"…†״²„‚", sad::ui::UINodeType::Slider},
                {"…״×״§״­", sad::ui::UINodeType::Toggle},
                {"״×ƒ״¯״³", sad::ui::UINodeType::Stack},
                // ג”€ג”€ג”€ ״¥״¯״®״§„ ג”€ג”€ג”€
                {"״­‚„", sad::ui::UINodeType::TextField},
                {"״­‚„_†״µ", sad::ui::UINodeType::TextField},
                {"״­‚„_״³״±", sad::ui::UINodeType::TextField},
                {"״­‚„_״¨״­״«", sad::ui::UINodeType::SearchBar},
                {"…†״·‚״©_†״µ", sad::ui::UINodeType::TextArea},
                {"…״±״¨״¹_״§״®״×״§״±", sad::ui::UINodeType::Checkbox},
                {"‚״§״¦…״©_…†״³״¯„״©", sad::ui::UINodeType::Picker},
                {"״×‚…", sad::ui::UINodeType::RatingBar},
                // ג”€ג”€ג”€ ״£״²״±״§״± ג”€ג”€ג”€
                {"״²״±_…״­״·", sad::ui::UINodeType::Button},
                {"״²״±_†״µ", sad::ui::UINodeType::Button},
                {"״²״±_״§‚ˆ†״©", sad::ui::UINodeType::Button},
                {"״²״±_״¹״§״¦…", sad::ui::UINodeType::FAB},
                {"״²״±_״×״¨״¯„", sad::ui::UINodeType::Toggle},
                // ג”€ג”€ג”€ ״×†‚„ ג”€ג”€ג”€
                {"״´״±״·_״×״·״¨‚", sad::ui::UINodeType::AppBar},
                {"״´״±״·_״³„", sad::ui::UINodeType::BottomNav},
                // ג”€ג”€ג”€ ״£†ˆ״§״¹ ״§„…ˆ״§״¯ (Material Design) ג”€ג”€ג”€
                {"״×״·״¨‚_…״§״¯״©", sad::ui::UINodeType::Container},
                {"‡ƒ„_…״§״¯״©", sad::ui::UINodeType::Scaffold},
                {"״´״±״·_״×״·״¨‚_…״§״¯״©", sad::ui::UINodeType::AppBar},
                {"״´״±״·_״×†‚„_…״§״¯״©", sad::ui::UINodeType::BottomNav},
                {"״¨״·״§‚״©_…״§״¯״©", sad::ui::UINodeType::Card},
                {"״²״±_…״±״×״¹", sad::ui::UINodeType::Button},
                {"״²״±_…״¹״¨״£", sad::ui::UINodeType::Button},
                {"״²״±_…״¹״¨״£_״¨״§‡״×", sad::ui::UINodeType::Button},
                {"״²״±_…״­״¯״¯", sad::ui::UINodeType::Button},
                {"״²״±_†״µ_…״§״¯״©", sad::ui::UINodeType::Button},
                {"״²״±_״£‚ˆ†״©_…״§״¯״©", sad::ui::UINodeType::Button},
                {"״²״±_״¹״§״¦…_…״§״¯״©", sad::ui::UINodeType::FAB},
                {"״§״µ„_…״§״¯״©", sad::ui::UINodeType::Divider},
                {"״¯״±״¬", sad::ui::UINodeType::Drawer},
                {"״´״±״·_״×״¨ˆ״¨", sad::ui::UINodeType::Tabs},
                {"״×״¨ˆ״¨", sad::ui::UINodeType::TabItem},
                // ג”€ג”€ג”€ ״×״®״·״· ג”€ג”€ג”€
                {"״´״¨ƒ״©", sad::ui::UINodeType::Grid},
                {"״§״µ„", sad::ui::UINodeType::Spacer},
                {"״®״·_״§״µ„", sad::ui::UINodeType::Divider},
                {"״×ˆ״³״·", sad::ui::UINodeType::Container},
                {"…״±†", sad::ui::UINodeType::Container},
                {"״µ†״¯ˆ‚", sad::ui::UINodeType::Box},
                // ג”€ג”€ג”€ ״­ˆ״§״±״§״× ג”€ג”€ג”€
                {"״­ˆ״§״±", sad::ui::UINodeType::Dialog},
                {"״­ˆ״§״±_״×״£ƒ״¯", sad::ui::UINodeType::Dialog},
                {"„ˆ״­״©_״³„״©", sad::ui::UINodeType::BottomSheet},
                {"״±״³״§„״©_…†״¨״«‚״©", sad::ui::UINodeType::SnackBar},
                {"״×„…״­", sad::ui::UINodeType::Tooltip},
                // ג”€ג”€ג”€ ״×‚״¯… ג”€ג”€ג”€
                {"״´״±״·_״×‚״¯…", sad::ui::UINodeType::ProgressBar},
                {"״×‚״¯…_״¯״§״¦״±", sad::ui::UINodeType::ProgressBar},
                {"״×״­…„", sad::ui::UINodeType::Skeleton},
                {"‡ƒ„_״×״­…„", sad::ui::UINodeType::Skeleton},
                {"‡ƒ„_״¹״¸…", sad::ui::UINodeType::Skeleton},
                // ג”€ג”€ג”€ ״µˆ״± ג”€ג”€ג”€
                {"״µˆ״±״©_״±…״²״©", sad::ui::UINodeType::Avatar},
                {"״´״§״±״©", sad::ui::UINodeType::Badge},
                {"״±‚״§‚״©", sad::ui::UINodeType::Chip},
                // ג”€ג”€ג”€ †״µˆ״µ ג”€ג”€ג”€
                {"״¹†ˆ״§†", sad::ui::UINodeType::Text},
                {"״×״³…״©", sad::ui::UINodeType::Text},
                {"‚״±״©", sad::ui::UINodeType::Text},
                {"״±״§״¨״·", sad::ui::UINodeType::Text},
                {"ƒˆ״¯", sad::ui::UINodeType::CodeBlock},
                {"״§‚״×״¨״§״³", sad::ui::UINodeType::Text},
                // ג”€ג”€ג”€ ‚ˆ״§״¦… ג”€ג”€ג”€
                {"״£ƒˆ״±״¯ˆ†", sad::ui::UINodeType::Expandable},
                {"‚״§״¦…״©_״´״¨ƒ״©", sad::ui::UINodeType::LazyGrid},
                {"‚״§״¦…״©_״×…״±״±", sad::ui::UINodeType::ScrollView},
                // ג”€ג”€ג”€ …״×‚״¯…״© ג”€ג”€ג”€
                {"״²״±_״±״§״¯ˆ", sad::ui::UINodeType::Radio},
                {"…†״×‚_״×״§״±״®", sad::ui::UINodeType::DatePicker},
                {"…†״×‚_„ˆ†", sad::ui::UINodeType::ColorPicker},
                {"…†״×‚_ˆ‚״×", sad::ui::UINodeType::TimePicker},
                {"״×‚ˆ…", sad::ui::UINodeType::Calendar},
                {"״¬״¯ˆ„_״¨״§†״§״×", sad::ui::UINodeType::DataTable},
                {"״¹״±״¶_״´״¬״±", sad::ui::UINodeType::TreeView},
                {"״®״·_״²…†", sad::ui::UINodeType::Timeline},
                {"״¹״±״¶_״¯ˆ״§״±", sad::ui::UINodeType::Carousel}, // ─── تخطيط Flutter الأساسي (v4) ───
                {"\xd9\x88\xd8\xb3\xd8\xb7", sad::ui::UINodeType::Center},
                {"\xd8\xaa\xd9\x88\xd8\xb3\xd9\x8a\xd8\xb7", sad::ui::UINodeType::Center},
                {"\xd8\xad\xd8\xb4\xd9\x88\xd8\xa9", sad::ui::UINodeType::Padding},
                {"\xd8\xad\xd8\xb4\xd9\x88", sad::ui::UINodeType::Padding},
                {"\xd9\x85\xd9\x82\xd8\xa7\xd8\xb3_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf", sad::ui::UINodeType::SizedBox},
                {"\xd9\x85\xd9\x88\xd8\xb3\xd8\xb9", sad::ui::UINodeType::Expanded},
                {"\xd9\x85\xd9\x88\xd8\xb3\xd9\x91\xd8\xb9", sad::ui::UINodeType::Expanded},
                {"\xd9\x85\xd8\xb1\xd9\x86", sad::ui::UINodeType::Flexible},
                {"\xd9\x85\xd8\xad\xd8\xa7\xd8\xb0\xd9\x8a", sad::ui::UINodeType::Align},
                {"\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd8\xa9_\xd8\xa2\xd9\x85\xd9\x86\xd8\xa9", sad::ui::UINodeType::SafeArea},
                {"\xd9\x83\xd8\xa7\xd8\xb4\xd9\x81_\xd8\xa5\xd9\x8a\xd9\x85\xd8\xa7\xd8\xa1\xd8\xa7\xd8\xaa", sad::ui::UINodeType::GestureDetector},
                {"\xd8\xad\xd8\xa8\xd8\xb1", sad::ui::UINodeType::InkWell},
                {"\xd9\x82\xd8\xa7\xd8\xa8\xd9\x84_\xd9\x84\xd9\x84\xd9\x86\xd9\x82\xd8\xb1", sad::ui::UINodeType::InkWell},
                {"\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd8\xb9\xd8\xb1\xd8\xb6", sad::ui::UINodeType::ListView},
                {"\xd8\xb5\xd9\x86\xd8\xaf\xd9\x88\xd9\x82_\xd9\x86\xd8\xb3\xd8\xa8\xd9\x8a", sad::ui::UINodeType::FractionallySizedBox},
                {"\xd8\xb5\xd9\x86\xd8\xaf\xd9\x88\xd9\x82_\xd9\x85\xd9\x82\xd9\x8a\xd8\xaf", sad::ui::UINodeType::ConstrainedBox},
                {"\xd9\x86\xd8\xb3\xd8\xa8\xd8\xa9_\xd8\xb9\xd8\xb1\xd8\xb6", sad::ui::UINodeType::AspectRatio},
            };

            auto it = fallbackMap.find(typeName);
            if (it != fallbackMap.end())
            {
                return it->second;
            }

            // ״§״×״±״§״¶: ״­״§ˆ״©
            return sad::ui::UINodeType::Container;
        }

        sad::ui::Color UIBridge::parseColor(const std::string &colorStr)
        {
            // ״£„ˆ״§† …״³…״§״© (‚… …† 0.0 ״¥„‰ 1.0)
            static const std::unordered_map<std::string, sad::ui::Color> namedColors = {
                {"״£״­…״±", {1.0f, 0.0f, 0.0f, 1.0f}},
                {"״£״®״¶״±", {0.0f, 1.0f, 0.0f, 1.0f}},
                {"״£״²״±‚", {0.0f, 0.0f, 1.0f, 1.0f}},
                {"״£״¨״¶", {1.0f, 1.0f, 1.0f, 1.0f}},
                {"״£״³ˆ״¯", {0.0f, 0.0f, 0.0f, 1.0f}},
                {"״±…״§״¯", {0.5f, 0.5f, 0.5f, 1.0f}},
                {"״£״µ״±", {1.0f, 1.0f, 0.0f, 1.0f}},
                {"״¨״±״×‚״§„", {1.0f, 0.65f, 0.0f, 1.0f}},
                {"״¨†״³״¬", {0.5f, 0.0f, 0.5f, 1.0f}},
                {"ˆ״±״¯", {1.0f, 0.75f, 0.8f, 1.0f}},
                {"״´״§", {0.0f, 0.0f, 0.0f, 0.0f}},
            };

            auto it = namedColors.find(colorStr);
            if (it != namedColors.end())
            {
                return it->second;
            }

            // #RRGGBB ״£ˆ #RRGGBBAA
            if (colorStr.size() >= 7 && colorStr[0] == '#')
            {
                unsigned int r, g, b, a = 255;
                if (colorStr.size() >= 9)
                {
                    std::sscanf(colorStr.c_str(), "#%02x%02x%02x%02x", &r, &g, &b, &a);
                }
                else
                {
                    std::sscanf(colorStr.c_str(), "#%02x%02x%02x", &r, &g, &b);
                }
                return {
                    static_cast<float>(r) / 255.0f,
                    static_cast<float>(g) / 255.0f,
                    static_cast<float>(b) / 255.0f,
                    static_cast<float>(a) / 255.0f};
            }

            // ״§״×״±״§״¶: ״£״³ˆ״¯
            return {0.0f, 0.0f, 0.0f, 1.0f};
        }

        const std::string &UIBridge::getLastGeneratedHtml()
        {
            return s_lastGeneratedHtml;
        }

    } // namespace Interpreter
} // namespace Sad
