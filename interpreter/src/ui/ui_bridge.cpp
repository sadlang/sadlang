/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: ui_bridge.cpp
 * المسار: interpreter/src/ui/ui_bridge.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * الوصف:
 * ------
 * تنفيذ جسر الربط بين المفسر ونظام واجهات ص (SadUI).
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
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
#include "sad_ui/window_control.h" // (Amelia H1) تصفير المتحكّم بعد التشغيل (تناظر مع sad_app_run)

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

        // ═══════════════════════════════════════════════════════════════════════════════
        // تنفيذ UIBridge
        // ═══════════════════════════════════════════════════════════════════════════════

        UIBridge::UIBridge() = default;
        UIBridge::~UIBridge()
        {
            if (activeInstance_ == this)
            {
                activeInstance_ = nullptr;
            }
        }

        // المثيل النشط
        UIBridge *UIBridge::activeInstance_ = nullptr;

        // (AR) HTML المولّد — يُخزَّن هنا لاسترجاعه من WASM
        // (EN) Generated HTML — stored here for WASM retrieval
        static std::string s_lastGeneratedHtml;

        bool UIBridge::run(const Data::Value &rootWidget, Interpreter *interpreter)
        {
            interpreter_ = interpreter;
            activeInstance_ = this; // تعيين المثيل النشط

            // (AR) دعم دالة البنّاء: إذا كان الجذر دالة (وليس كائن واجهة)، نحفظها كبنّاء
            //      ونستدعيها للحصول على الشجرة الأولية
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
                    std::cerr << "خطأ في استدعاء دالة البنّاء: " << e.what() << std::endl;
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

            // تحميل التخزين المحلي من القرص
            loadStorageFromDisk();
            // تحويل شجرة Widget إلى شجرة IR
            auto irRoot = convertToIR(actualWidget);
            if (!irRoot)
            {
                std::cerr << "فشل تحويل شجرة الواجهة إلى IR" << std::endl;
                return false;
            }

#ifdef SAD_UI_USE_SDL2
            // إنشاء نافذة سطح المكتب
            sad::ui::desktop::DesktopWindow window;
            activeWindow_ = &window; // حفظ مؤشر النافذة لإعادة البناء
            sad::ui::desktop::WindowOptions options;

            // قراءة عنوان النافذة من IR إذا وُجد (من خاصية "عنوان" في الجذر)
            const auto *titleProp = irRoot->findProperty("عنوان");
            if (titleProp)
            {
                if (auto *str = std::get_if<std::string>(&titleProp->value))
                {
                    options.title = *str;
                }
            }
            else
            {
                options.title = "تطبيق ص";
            }

            // قراءة أبعاد النافذة إذا وُجدت
            const auto *widthProp = irRoot->findProperty("عرض");
            const auto *heightProp = irRoot->findProperty("ارتفاع");
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
                std::cerr << "فشل إنشاء النافذة" << std::endl;
                return false;
            }

            // تعيين المحتوى
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

            // تعيين callback للأحداث
            window.setOnEventCallback(
                [this](sad::ui::IREventType eventType,
                       const std::string &elementId,
                       const sad::ui::IRNode *node,
                       const sad::ui::EventData &eventData)
                {
                    handleEvent(eventType, elementId, node, eventData);
                });

            // تسجيل callback المؤقتات — يُستدعى كل إطار
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

            // تسجيل callback إفلات الملفات
            window.setDropFileCallback([this](const std::string &filePath)
                                       { handleFileDrop(filePath); });

            // تشغيل حلقة الأحداث
            window.run();

            // تنظيف
            activeWindow_ = nullptr;
            activeInstance_ = nullptr;
            window.destroy();
            // (Amelia H1) صفّر المتحكّم المشترك بعد التشغيل (تناظرًا مع sad_app_run في
            //   وقت تشغيل المترجم) لئلّا يتسرّب طلب عنوان/إغلاق مُعلَّق إلى تشغيلٍ لاحق في
            //   العمليّة نفسها (مصدر حقيقة واحد ⇒ سلوك تصفيرٍ واحد للمحرّكين).
            sad::ui::windowController().reset();

            return true;
#else
#if defined(SAD_WASM_BUILD)
            // (AR) وضع WASM — توليد HTML من شجرة IR باستخدام HtmlCodegen
            // (EN) WASM mode — generate HTML from IR tree using HtmlCodegen
            {
                sad::ui::IRModule module;
                module.name = "وحدة_واجهة";
                module.root = irRoot;

                sad::ui::web::HtmlCodegenOptions htmlOpts;
                htmlOpts.title = "تطبيق ص";
                htmlOpts.dir = "rtl";
                htmlOpts.lang = "ar";

                // قراءة عنوان النافذة من IR إذا وُجد
                const auto *titleProp = irRoot->findProperty("عنوان");
                if (titleProp)
                {
                    if (auto *str = std::get_if<std::string>(&titleProp->value))
                    {
                        htmlOpts.title = *str;
                    }
                }

                sad::ui::web::HtmlCodegen codegen(htmlOpts);
                s_lastGeneratedHtml = codegen.generate(module);

                // طباعة علامة خاصة ليتعرف عليها runtime
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
            // SDL2 غير مفعّل — طباعة شجرة IR فقط
            std::cout << "=== شجرة IR ===" << std::endl;
            std::cout << irRoot->dump() << std::endl;
            std::cout << "=== انتهى ===" << std::endl;
            std::cout << "[تحذير] SDL2 غير مفعّل — الواجهة لن تُعرض رسومياً" << std::endl;
            std::cout << "لتفعيل SDL2، ثبّت SDL2 على النظام ثم أعد البناء" << std::endl;
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
                std::cerr << "تحذير: وصلت شجرة الواجهة للحد الأقصى من العمق" << std::endl;
                return nullptr;
            }

            // ─── كائن (نسخة صنف) ───
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

                // قراءة _نوع
                std::string widgetType = obj->getClassName();                        // اسم الصنف كنوع افتراضي
                Data::Value *typeField = obj->getField("_\xd9\x86\xd9\x88\xd8\xb9"); // _نوع
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

            // ─── نص بسيط → عقدة Text ───
            if (widget.isString())
            {
                auto irNode = sad::ui::IRNode::create(sad::ui::UINodeType::Text);
                sad::ui::IRProperty prop;
                prop.key = "نص";
                prop.value = widget.toString();
                irNode->addProperty(prop);
                return irNode;
            }

            return nullptr;
        }

        sad::ui::UINodeType UIBridge::stringToNodeType(const std::string &typeName)
        {
            // (AR) البحثُ من مصدر الحقيقة وحدَه: arabicNameToNodeType مبنيّةٌ على
            //   جدولٍ مولَّدٍ من language-truth/ui_nodes.yaml (الاسمُ القانونيّ +
            //   أسماءُ المصانع + دَينُ الهجرة). كانت هنا خريطةُ «تخمين» يدويّةٌ
            //   بنحو ٩٩ اسمًا حرفيًّا فانحرفت: `توسيط` و`مرن` كانتا تُحلّان إلى
            //   Container بدل Center/Flexible لأنّ مدخلةً سابقةً تحجُب اللاحقة.
            // (EN) Single SoT-generated lookup; the hand-written fallback map is
            //   gone (it silently shadowed correct entries).
            auto result = sad::ui::arabicNameToNodeType(typeName);
            if (result.has_value())
            {
                return result.value();
            }

            // افتراضي: حاوية
            return sad::ui::UINodeType::Container;
        }

        sad::ui::Color UIBridge::parseColor(const std::string &colorStr)
        {
            // ألوان مُسماة (قيم من 0.0 إلى 1.0)
            static const std::unordered_map<std::string, sad::ui::Color> namedColors = {
                {"أحمر", {1.0f, 0.0f, 0.0f, 1.0f}},
                {"أخضر", {0.0f, 1.0f, 0.0f, 1.0f}},
                {"أزرق", {0.0f, 0.0f, 1.0f, 1.0f}},
                {"أبيض", {1.0f, 1.0f, 1.0f, 1.0f}},
                {"أسود", {0.0f, 0.0f, 0.0f, 1.0f}},
                {"رمادي", {0.5f, 0.5f, 0.5f, 1.0f}},
                {"أصفر", {1.0f, 1.0f, 0.0f, 1.0f}},
                {"برتقالي", {1.0f, 0.65f, 0.0f, 1.0f}},
                {"بنفسجي", {0.5f, 0.0f, 0.5f, 1.0f}},
                {"وردي", {1.0f, 0.75f, 0.8f, 1.0f}},
                {"شفاف", {0.0f, 0.0f, 0.0f, 0.0f}},
            };

            auto it = namedColors.find(colorStr);
            if (it != namedColors.end())
            {
                return it->second;
            }

            // #RRGGBB أو #RRGGBBAA
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

            // افتراضي: أسود
            return {0.0f, 0.0f, 0.0f, 1.0f};
        }

        const std::string &UIBridge::getLastGeneratedHtml()
        {
            return s_lastGeneratedHtml;
        }

    } // namespace Interpreter
} // namespace Sad
