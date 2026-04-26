/**
 * @file ui_core_builtins.cpp
 * @brief (AR) محرك الواجهات الأساسي — التنقل، السمة، الحالة، النافذة
 * @brief (EN) Core UI engine — navigation, theme, state, window management
 */

#include "ui_module_builtins.h"
#include "ui_bridge.h"
#include "builtins.h"
#include "interpreter_core.h"
#include "builtin_registry.h"
#include "object_instance.h"
#include "widget_builder.h"
#include "sad_ui/node.h"
#include "sad_ui/ir.h"
#include "sad_ui/types.h"
#include "sad_ui/web/html_codegen.h"
#include <memory>
#include <string>
#include <vector>
#include <iostream>

namespace Sad
{
    namespace Interpreter
    {

        namespace
        {

            /**
             * @brief (AR) طباعة شجرة العناصر التعريحية بشكل هرمي (للتصحيح والتحقق)
             * @brief (EN) Print declarative widget tree hierarchically (for debug/verification)
             */
            /**
             * @brief (AR) طباعة شجرة IRNode من WidgetBuilder بشكل هرمي
             * @brief (EN) Print IRNode tree from WidgetBuilder hierarchically
             */
            void printIRNodeTree(const std::shared_ptr<sad::ui::IRNode> &node, int depth, int maxDepth)
            {
                if (!node || depth > maxDepth)
                    return;

                std::string indent(depth * 2, ' ');

                // (AR) طباعة نوع العنصر
                std::cout << indent << sad::ui::nodeTypeToArabicName(node->getType());

                // (AR) طباعة الخصائص
                const auto &props = node->getProperties();
                if (!props.empty())
                {
                    std::cout << "(";
                    bool first = true;
                    for (const auto &prop : props)
                    {
                        if (!first)
                            std::cout << ", ";
                        first = false;
                        std::cout << prop.key << ": ";
                        std::visit([](const auto &v)
                                   {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    std::string s = v;
                    if (s.size() > 40) s = s.substr(0, 40) + "...";
                    std::cout << "\"" << s << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    std::cout << (v ? "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad" : "\xd8\xae\xd8\xb7\xd8\xa3"); // صحيح/خطأ
                } else {
                    std::cout << v;
                } }, prop.value);
                    }
                    std::cout << ")";
                }

                // (AR) طباعة الأحداث
                const auto &events = node->getEvents();
                if (!events.empty())
                {
                    std::cout << " [";
                    bool first = true;
                    for (const auto &event : events)
                    {
                        if (!first)
                            std::cout << ", ";
                        first = false;
                        std::cout << event.getEventName();
                    }
                    std::cout << "]";
                }

                // (AR) طباعة التحريكات
                const auto &anims = node->getAnimations();
                if (!anims.empty())
                {
                    std::cout << " {";
                    bool first = true;
                    for (const auto &anim : anims)
                    {
                        if (!first)
                            std::cout << ", ";
                        first = false;
                        std::cout << "\xd8\xad\xd8\xb1\xd9\x83\xd8\xa9:" // حركة:
                                  << sad::ui::animationTypeToString(anim.type)
                                  << "/" << anim.duration << "\xd8\xab"; // ث (ثانية)
                        if (anim.easing != sad::ui::EasingCurve::EaseInOut)
                        {
                            std::cout << "/" << sad::ui::easingCurveToString(anim.easing);
                        }
                        if (anim.delay > 0.0f)
                        {
                            std::cout << " \xd8\xaa\xd8\xa3\xd8\xae\xd9\x8a\xd8\xb1:" << anim.delay << "\xd8\xab"; // تأخير:Xث
                        }
                        if (anim.repeatCount != 1)
                        {
                            std::cout << " \xd8\xaa\xd9\x83\xd8\xb1\xd8\xa7\xd8\xb1:"                                 // تكرار:
                                      << (anim.repeatCount == 0 ? "\xe2\x88\x9e" : std::to_string(anim.repeatCount)); // ∞
                        }
                    }
                    std::cout << "}";
                }

                std::cout << std::endl;

                // (AR) طباعة الأبناء تكرارياً
                for (size_t i = 0; i < node->childCount(); i++)
                {
                    printIRNodeTree(node->getChildren()[i], depth + 1, maxDepth);
                }
            }

            void printWidgetTree(const Data::Value &widget, int depth, int maxDepth = 50)
            {
                if (depth > maxDepth)
                    return;

                std::string indent(depth * 2, ' ');

                if (widget.isObject())
                {
                    auto obj = widget.toObject();
                    if (!obj)
                        return;

                    // ═══════════════════════════════════════════════════════════════════
                    // (AR) دعم WidgetBuilder: إذا كان الكائن WidgetBuilder، نطبع
                    //      شجرة IRNode مباشرة بدلاً من الخرائط القديمة
                    // (EN) WidgetBuilder support: print IRNode tree directly
                    // ═══════════════════════════════════════════════════════════════════
                    if (isWidgetBuilder(obj.get()))
                    {
                        auto *wb = static_cast<Sad::Interpreter::WidgetBuilder *>(obj.get());
                        printIRNodeTree(wb->getIRNode(), depth, maxDepth);
                        return;
                    }

                    // (AR) احتياط: كائن عادي — طباعة اسم الصنف والحقول
                    // (EN) Fallback: plain object — print class name and fields
                    std::cout << indent << obj->getClassName() << std::endl;
                    return;
                }

                std::cout << indent << "[" << widget.toString() << "]" << std::endl;
            }

        } // anonymous namespace

        void registerUICoreBuiltins(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // (AR) اختصار لأسماء ثوابت نواة الواجهة
            namespace Bc = Builtins::Names::UICore;

            // ─── _محرك_واجهات(عنصر_جذر) — تشغيل التطبيق بشجرة عناصر تعريحية أو دالة بنّاء ───
            auto engine_fn = [&interpreter](const std::vector<std::shared_ptr<Data::Value>> &args)
                -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                {
                    throw std::runtime_error(
                        "\xd8\xb4\xd8\xba\xd9\x91\xd9\x84() \xd9\x8a\xd8\xad\xd8\xaa\xd8\xa7\xd8\xac "
                        "\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1 \xd8\xac\xd8\xb0\xd8\xb1");
                }
                const auto &rootWidget = *args[0];

                // (AR) دعم دالة البنّاء: إذا كان الوسيط دالة، نمرره مباشرة للجسر
                // (EN) Builder function support: if arg is a function, pass directly to bridge
                if (rootWidget.isFunction() || rootWidget.isCallable())
                {
                    UIBridge bridge;
                    bool success = bridge.run(rootWidget, const_cast<Interpreter *>(&interpreter));
                    if (!success)
                    {
                        std::cerr << "[\xd8\xaa\xd8\xad\xd8\xb0\xd9\x8a\xd8\xb1] \xd9\x81\xd8\xb4\xd9\x84 \xd8\xaa\xd8\xb4\xd8\xba\xd9\x8a\xd9\x84 \xd8\xa7\xd9\x84\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xb1\xd8\xb3\xd9\x88\xd9\x85\xd9\x8a\xd8\xa9" << std::endl;
                    }
                    return std::make_shared<Data::Value>();
                }

                if (!rootWidget.isObject() && !rootWidget.isMap())
                {
                    throw std::runtime_error(
                        "\xd8\xb4\xd8\xba\xd9\x91\xd9\x84() \xd9\x8a\xd8\xad\xd8\xaa\xd8\xa7\xd8\xac "
                        "\xd9\x83\xd8\xa7\xd8\xa6\xd9\x86 \xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1 "
                        "\xd8\xa3\xd9\x88 \xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 \xd8\xa8\xd9\x86\xd9\x91\xd8\xa7\xd8\xa1");
                }
                UIBridge bridge;
                bool success = bridge.run(rootWidget, const_cast<Interpreter *>(&interpreter));
                if (!success)
                {
                    std::cerr << "[\xd8\xaa\xd8\xad\xd8\xb0\xd9\x8a\xd8\xb1] \xd9\x81\xd8\xb4\xd9\x84 \xd8\xaa\xd8\xb4\xd8\xba\xd9\x8a\xd9\x84 \xd8\xa7\xd9\x84\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xb1\xd8\xb3\xd9\x88\xd9\x85\xd9\x8a\xd8\xa9" << std::endl;
                }
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::ENGINE), engine_fn);

            // ─── تشغيل_تطبيق(عنصر_أو_دالة) — نقطة الدخول العامة (مثل runApp في Flutter) ───
            // (AR) هذه الدالة العامة التي يستخدمها المبرمج لتشغيل تطبيق واجهة رسومية
            // (EN) Public entry point for running a GUI application (like Flutter's runApp)
            auto run_app_fn = [&interpreter](const std::vector<std::shared_ptr<Data::Value>> &args)
                -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                {
                    throw std::runtime_error(
                        "\xd8\xaa\xd8\xb4\xd8\xba\xd9\x8a\xd9\x84_\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82() "
                        "\xd9\x8a\xd8\xad\xd8\xaa\xd8\xa7\xd8\xac \xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1 "
                        "\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9 \xd8\xa3\xd9\x88 "
                        "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 \xd8\xa8\xd9\x86\xd9\x91\xd8\xa7\xd8\xa1");
                    // "تشغيل_تطبيق() يحتاج عنصر واجهة أو دالة بنّاء"
                }
                const auto &rootWidget = *args[0];
                UIBridge bridge;
                bool success = bridge.run(rootWidget, const_cast<Interpreter *>(&interpreter));
                if (!success)
                {
                    std::cerr << "[\xd8\xae\xd8\xb7\xd8\xa3] \xd9\x81\xd8\xb4\xd9\x84 "
                              << "\xd8\xaa\xd8\xb4\xd8\xba\xd9\x8a\xd9\x84 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82"
                              << std::endl;
                    // "[خطأ] فشل تشغيل التطبيق"
                }
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::RUN_APP), run_app_fn); // تشغيل_تطبيق

            // ─── طباعة_شجرة(عنصر) — طباعة شجرة العناصر للتصحيح ───
            auto print_tree_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args)
                -> std::shared_ptr<Data::Value>
            {
                if (!args.empty())
                {
                    printWidgetTree(*args[0], 0);
                }
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::PRINT_TREE), print_tree_fn); // طباعة_شجرة

            // ─── دوال التنقل ───
            auto navigate_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("\xd8\xa7\xd9\x86\xd8\xaa\xd9\x82\xd9\x84() \xd9\x8a\xd8\xad\xd8\xaa\xd8\xa7\xd8\xac \xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1 \xd8\xb5\xd9\x81\xd8\xad\xd8\xa9");
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->navigateTo(*args[0]);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::NAVIGATE), navigate_fn);

            // ─── انتقل_بتحريك(صفحة, نوع_انتقال, مدة؟) — تنقل مع تحريك بصري ───
            // (AR) دالة الانتقال مع تحريك — تدعم أنواع مثل "ظهور"، "انزلاق_يمين"، "تكبير"
            // (EN) Navigate with page transition animation — supports types like "fadeIn", "slideRight"
            auto navigate_transition_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args)
                -> std::shared_ptr<Data::Value>
            {
                // (AR) يحتاج 2 وسائط على الأقل: الصفحة + نوع الانتقال
                if (args.size() < 2)
                    throw std::runtime_error(
                        "\xd8\xa7\xd9\x86\xd8\xaa\xd9\x82\xd9\x84_\xd8\xa8\xd8\xaa\xd8\xad\xd8\xb1\xd9\x8a\xd9\x83() "
                        "\xd9\x8a\xd8\xad\xd8\xaa\xd8\xa7\xd8\xac \xd8\xb5\xd9\x81\xd8\xad\xd8\xa9 "
                        "\xd9\x88\xd9\x86\xd9\x88\xd8\xb9 \xd8\xa7\xd9\x86\xd8\xaa\xd9\x82\xd8\xa7\xd9\x84");
                // "انتقل_بتحريك() يحتاج صفحة ونوع انتقال"

                std::string transType = args[1]->toString();
                float duration = 0.3f;
                if (args.size() >= 3)
                    duration = static_cast<float>(args[2]->toDouble());

                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->navigateWithTransition(*args[0], transType, duration);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(
                std::string(Bc::NAVIGATE_TRANSITION),
                navigate_transition_fn); // انتقل_بتحريك

            // ─── انتقل_بتحريك_كامل(صفحة, دخول, خروج, مدة؟) — تنقل مع خروج + دخول ───
            // (AR) دالة انتقال مع تحريك خروج على الصفحة الحالية + تحريك دخول على الجديدة
            // (EN) Navigate with exit transition on current page + entry on new page
            auto navigate_exit_transition_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args)
                -> std::shared_ptr<Data::Value>
            {
                // (AR) يحتاج 3 وسائط على الأقل: الصفحة + دخول + خروج
                if (args.size() < 3)
                    throw std::runtime_error(
                        "\xd8\xa7\xd9\x86\xd8\xaa\xd9\x82\xd9\x84_\xd8\xa8\xd8\xaa\xd8\xad\xd8\xb1\xd9\x8a\xd9\x83_\xd9\x83\xd8\xa7\xd9\x85\xd9\x84() "
                        "\xd9\x8a\xd8\xad\xd8\xaa\xd8\xa7\xd8\xac \xd8\xb5\xd9\x81\xd8\xad\xd8\xa9 "
                        "\xd9\x88\xd9\x86\xd9\x88\xd8\xb9 \xd8\xaf\xd8\xae\xd9\x88\xd9\x84 "
                        "\xd9\x88\xd9\x86\xd9\x88\xd8\xb9 \xd8\xae\xd8\xb1\xd9\x88\xd8\xac");
                // "انتقل_بتحريك_كامل() يحتاج صفحة ونوع دخول ونوع خروج"

                std::string entryType = args[1]->toString();
                std::string exitType = args[2]->toString();
                float duration = 0.3f;
                if (args.size() >= 4)
                    duration = static_cast<float>(args[3]->toDouble());

                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->navigateWithExitTransition(*args[0], entryType, exitType, duration);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(
                std::string(Bc::NAVIGATE_EXIT_TRANSITION),
                navigate_exit_transition_fn); // انتقل_بتحريك_كامل

            auto back_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->navigateBack();
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::BACK), back_fn);

            // ─── عودة_بتحريك(نوع_انتقال, مدة؟) — العودة مع تحريك بصري ───
            // (AR) العودة للصفحة السابقة مع تحريك انتقالي
            // (EN) Navigate back with visual page transition
            auto back_transition_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args)
                -> std::shared_ptr<Data::Value>
            {
                std::string transType = "\xd8\xb8\xd9\x87\xd9\x88\xd8\xb1"; // ظهور (افتراضي)
                float duration = 0.3f;
                if (!args.empty())
                    transType = args[0]->toString();
                if (args.size() >= 2)
                    duration = static_cast<float>(args[1]->toDouble());

                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->navigateBackWithTransition(transType, duration);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(
                std::string(Bc::BACK_TRANSITION),
                back_transition_fn); // عودة_بتحريك

            auto root_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->navigateToRoot();
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::BACK_TO_ROOT), root_fn);

            auto replace_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xa8\xd8\xaf\xd9\x84() \xd9\x8a\xd8\xad\xd8\xaa\xd8\xa7\xd8\xac \xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1 \xd8\xb5\xd9\x81\xd8\xad\xd8\xa9");
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->replacePage(*args[0]);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::REPLACE_PAGE), replace_fn);

            // ─── دوال الثيم ───
            auto toggle_theme_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                sad::ui::toggleTheme();
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->rebuildUI();
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::TOGGLE_THEME), toggle_theme_fn);

            auto dark_mode_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                sad::ui::setTheme(sad::ui::ThemeMode::Dark);
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->rebuildUI();
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::DARK_MODE), dark_mode_fn);

            auto light_mode_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                sad::ui::setTheme(sad::ui::ThemeMode::Light);
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->rebuildUI();
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::LIGHT_MODE), light_mode_fn);

            auto is_dark_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                return std::make_shared<Data::Value>(sad::ui::isDarkMode());
            };
            fm.registerBuiltinFunction(std::string(Bc::IS_DARK), is_dark_fn);

            // ─── تحديث الحالة ───
            auto set_state_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->rebuildUI();
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::UPDATE_STATE), set_state_fn);

            // ─── عنوان النافذة ───
            auto set_title_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty() || !args[0]->isString())
                {
                    throw std::runtime_error("\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86_\xd8\xa7\xd9\x84\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9() \xd9\x8a\xd8\xad\xd8\xaa\xd8\xa7\xd8\xac \xd9\x86\xd8\xb5");
                }
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->setWindowTitle(args[0]->toString());
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::SET_TITLE), set_title_fn);

            // ─── عدد الصفحات ───
            auto page_count_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (bridge)
                    return std::make_shared<Data::Value>(static_cast<int64_t>(bridge->getNavigationDepth()));
                return std::make_shared<Data::Value>(static_cast<int64_t>(0));
            };
            fm.registerBuiltinFunction(std::string(Bc::PAGE_COUNT), page_count_fn);

            // ─── إغلاق النافذة ───
            auto close_window_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->closeWindow();
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::CLOSE_WINDOW), close_window_fn);

            // ─── عيّن_الحالة(دالة_تحديث) — تحديث حالة + إعادة بناء (مثل setState في Flutter) ───
            // (AR) يستدعي دالة التحديث أولاً ثم يُعيد بناء الواجهة
            // (EN) Calls update function first, then rebuilds UI (like Flutter's setState)
            auto set_state_v2_fn = [&interpreter](const std::vector<std::shared_ptr<Data::Value>> &args)
                -> std::shared_ptr<Data::Value>
            {
                // إذا مُررت دالة تحديث، نفّذها أولاً
                if (!args.empty() && (args[0]->isFunction() || args[0]->isCallable()))
                {
                    try
                    {
                        auto funcRef = args[0]->toFunction();
                        if (funcRef)
                        {
                            interpreter.callUserFunction(funcRef->registeredName, {});
                        }
                    }
                    catch (const std::exception &e)
                    {
                        std::cerr << "[\xd8\xae\xd8\xb7\xd8\xa3] \xd8\xb9\xd9\x8a\xd9\x91\xd9\x86_\xd8\xa7\xd9\x84\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9: "
                                  << e.what() << std::endl;
                        // "[خطأ] عيّن_الحالة: ..."
                    }
                }
                // إعادة بناء الواجهة
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->rebuildUI();
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::SET_STATE), set_state_v2_fn); // عيّن_الحالة

            // ─── توليد_ويب(عنصر_أو_دالة) — توليد HTML من شجرة عناصر ───
            // (AR) يحوّل شجرة عناصر ص إلى صفحة HTML كاملة
            // (EN) Converts Sad widget tree to complete HTML page
            auto gen_web_fn = [&interpreter](const std::vector<std::shared_ptr<Data::Value>> &args)
                -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                {
                    throw std::runtime_error(
                        "\xd8\xaa\xd9\x88\xd9\x84\xd9\x8a\xd8\xaf_\xd9\x88\xd9\x8a\xd8\xa8() "
                        "\xd9\x8a\xd8\xad\xd8\xaa\xd8\xa7\xd8\xac \xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1 "
                        "\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9");
                    // "توليد_ويب() يحتاج عنصر واجهة"
                }

                const auto &rootWidget = *args[0];
                Data::Value actualWidget = rootWidget;

                // (AR) إذا كان دالة بنّاء (وليس كائن واجهة)، استدعها للحصول على الشجرة
                // (EN) If builder function (not widget object), invoke to get the tree
                if (rootWidget.isFunction())
                {
                    try
                    {
                        auto funcRef = rootWidget.toFunction();
                        if (funcRef)
                        {
                            actualWidget = interpreter.callUserFunction(funcRef->registeredName, {});
                        }
                    }
                    catch (const std::exception &e)
                    {
                        throw std::runtime_error(
                            std::string("\xd9\x81\xd8\xb4\xd9\x84 \xd8\xa7\xd8\xb3\xd8\xaa\xd8\xaf\xd8\xb9\xd8\xa7\xd8\xa1 "
                                        "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xa8\xd9\x86\xd9\x91\xd8\xa7\xd8\xa1: ") +
                            e.what());
                        // "فشل استدعاء دالة البنّاء: ..."
                    }
                }

                // تحويل إلى IR
                UIBridge bridge;
                auto irRoot = bridge.convertToIR(actualWidget);
                if (!irRoot)
                {
                    throw std::runtime_error(
                        "\xd9\x81\xd8\xb4\xd9\x84 \xd8\xaa\xd8\xad\xd9\x88\xd9\x8a\xd9\x84 "
                        "\xd8\xa7\xd9\x84\xd8\xb4\xd8\xac\xd8\xb1\xd8\xa9 \xd8\xa5\xd9\x84\xd9\x89 IR");
                    // "فشل تحويل الشجرة إلى IR"
                }

                // توليد HTML
                sad::ui::IRModule module;
                module.name = "web_output";
                module.root = irRoot;

                // إعداد الخيارات
                sad::ui::web::HtmlCodegenOptions opts;
                opts.dir = "rtl";
                opts.lang = "ar";
                opts.title = "\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82 \xd8\xb5"; // "تطبيق ص"

                // قراءة العنوان من الوسيط الثاني إذا وُجد
                if (args.size() > 1 && args[1]->isString())
                {
                    opts.title = args[1]->toString();
                }

                sad::ui::web::HtmlCodegen codegen(opts);
                std::string html = codegen.generate(module);

                return std::make_shared<Data::Value>(html);
            };
            fm.registerBuiltinFunction(std::string(Bc::GEN_WEB), gen_web_fn); // توليد_ويب
        }

    } // namespace Interpreter
} // namespace Sad
