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
#include "sad_ui/event_dispatch.h" // (rfcs#51) stopActiveEventPropagation
#include "sad_ui/types.h"
#include "sad_ui/print_tree.h" // (AR) مصدر الحقيقة الوحيد لطباعة شجرة IR (مشترك)
#include "sad_ui/nav.h"        // (AR) مكدّس التنقّل المشترك (مصدر الحقيقة: عمق + بنية + رسم حيّ)
#include "sad_ui/web/html_codegen.h"
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <cstdint>
#include "builtins/builtin_context.h"
#include "error_manager.h" // (AR) buildBilingualMessage من كتالوج الأخطاء
#include "error_catalog.h" // (AR) RenderContext

namespace Sad
{
    namespace Interpreter
    {

        namespace
        {

            // ─── جدول صفحات المفسّر لمكدّس التنقّل المشترك ───────────────────────────
            // (AR) القلب (sad::ui::nav) يخزّن «مقبضًا معتمًا» (void*) فيبقى محايدًا
            //      لأنواع المحرّك. المفسّر يربط المقبض بصفحته Data::Value هنا، فيُجذّرها
            //      (shared_ptr) طوال وجودها في المكدّس، ويُحرّرها القلبُ عند الإسقاط عبر
            //      release ⇒ لا تسريب (نظير المترجم الذي يخزّن SadWidget مباشرةً).
            std::unordered_map<std::uintptr_t, std::shared_ptr<Data::Value>> &interpPageTable()
            {
                // (AR) مُسرَّبٌ عمدًا (never-destroyed): مُدمِّر nav() الـstatic في المكتبة
                //      قد يستدعي interpPageRelease عند خروج البرنامج، وترتيب إتلاف statics
                //      عبر الوحدتين غير محدَّد. الإسراب يضمن بقاء الجدول صالحًا عند أيّ
                //      استدعاء (تشغيلًا وعند الخروج) ⇒ لا UB. الذاكرة تُستردّ بإنهاء العمليّة.
                static auto *t = new std::unordered_map<std::uintptr_t, std::shared_ptr<Data::Value>>();
                return *t;
            }
            void interpPageRelease(void *handle)
            {
                interpPageTable().erase(reinterpret_cast<std::uintptr_t>(handle));
            }
            /// (AR) يبني NavEntry يحمل مقبض الصفحة + مُحرِّرها (ملكيّة مُدارة Q5).
            [[nodiscard]] sad::ui::NavEntry interpPageEntry(const std::shared_ptr<Data::Value> &page)
            {
                static std::uintptr_t counter = 0;
                const std::uintptr_t h = ++counter;
                interpPageTable()[h] = page;
                sad::ui::NavEntry e;
                e.page = reinterpret_cast<void *>(h);
                e.data = reinterpret_cast<void *>(h);
                e.release = &interpPageRelease;
                return e;
            }
            // ═══════════════════════════════════════════════════════════════════
            // (AR) نموذج البانِي (م1-ج، توقيع البانِي): `انتقل(دالّة)` يخزّن **دالّة ص**
            //      تُنتج الصفحة عند كلّ رسم (تفاعليّة داخل الصفحة، نظير Flutter routes)،
            //      لا لقطةً ميّتة. المكتبة تخزّن مؤشّر بانٍ C معتم (PageBuilder = void*(*)
            //      (void*))؛ لكن دالّة ص تحتاج المفسّر لاستدعائها. لذا نحفظ {الدالّة،
            //      المفسّر} في جدولٍ جانبيّ مُفهرَس بمقبض، ونمرّر للمكتبة بانيًا static
            //      (interpBuilderInvoke) يقرأ الجدول، يستدعي الدالّة عبر المفسّر، ويسجّل
            //      الصفحة المبنيّة في interpPageTable مُعيدًا مقبضها ⇒ buildCurrent موحّد.
            // (EN) Builder model: انتقل(function) stores a ص function producing the page
            //      on each render (in-page interactivity). The library stores an opaque C
            //      PageBuilder; a ص function needs the interpreter, so we keep {fn,interp}
            //      in a side table keyed by handle and hand the library a static builder.
            // ═══════════════════════════════════════════════════════════════════
            struct InterpBuilderEntry
            {
                std::shared_ptr<Data::Value> fn;         ///< دالّة ص المُنتِجة للصفحة
                Sad::Interpreter::Interpreter *interp;   ///< المفسّر لاستدعائها
                std::uintptr_t pageHandle = 0;           ///< مقبض الصفحة المبنيّة (يُعاد استعماله)
            };
            std::unordered_map<std::uintptr_t, InterpBuilderEntry> &interpBuilderTable()
            {
                // (AR) مُسرَّبٌ عمدًا (نظير interpPageTable): مُدمِّر nav() الـstatic قد
                //      يستدعي interpBuilderRelease عند الخروج وترتيب إتلاف statics غير محدَّد.
                static auto *t = new std::unordered_map<std::uintptr_t, InterpBuilderEntry>();
                return *t;
            }
            void interpBuilderRelease(void *data)
            {
                // (AR) عند إسقاط إدخال البانِي (عودة/استبدال/جذر): احذف مقبض صفحته المبنيّة
                //      من interpPageTable أيضًا (لا تُبقِ الشجرة المبنيّة حيّةً) ثمّ إدخال البانِي.
                auto &tbl = interpBuilderTable();
                auto it = tbl.find(reinterpret_cast<std::uintptr_t>(data));
                if (it != tbl.end())
                {
                    if (it->second.pageHandle != 0)
                        interpPageTable().erase(it->second.pageHandle);
                    tbl.erase(it);
                }
            }
            /// (AR) البانِي static الذي تستدعيه nav().buildCurrent: يستدعي دالّة ص المخزّنة
            ///      ويُعيد مقبض الصفحة المبنيّة (مُسجَّلة في interpPageTable). فارغٌ إن تعذّر.
            void *interpBuilderInvoke(void *data)
            {
                auto &tbl = interpBuilderTable();
                auto it = tbl.find(reinterpret_cast<std::uintptr_t>(data));
                if (it == tbl.end() || !it->second.interp || !it->second.fn)
                    return nullptr;
                auto funcRef = it->second.fn->toFunction();
                if (!funcRef)
                    return nullptr;
                Data::Value built = it->second.interp->callUserFunction(funcRef->registeredName, {});
                // (AR) أعِد استعمال **مقبضٍ واحد لكلّ إدخال بانٍ** (لا مقبض جديد كلّ رسم):
                //      نخصّصه مرّةً (نطاق 1<<48 منفصل عن مقابض اللقطات) ثمّ نَدُوس محتواه
                //      بالصفحة المبنيّة الطازجة ⇒ لا نموّ غير محدود لـinterpPageTable في
                //      تطبيقٍ حيّ يستدعي buildCurrent كلّ إطار (كان يسرّب إدخالًا/رسمة).
                if (it->second.pageHandle == 0)
                {
                    static std::uintptr_t builtCounter = (static_cast<std::uintptr_t>(1) << 48);
                    it->second.pageHandle = ++builtCounter;
                }
                interpPageTable()[it->second.pageHandle] = std::make_shared<Data::Value>(std::move(built));
                return reinterpret_cast<void *>(it->second.pageHandle);
            }
            /// (AR) يبني NavEntry بانيًا (build/data/release) يلتقط دالّة ص + المفسّر (Q5).
            [[nodiscard]] sad::ui::NavEntry interpBuilderEntryOf(const std::shared_ptr<Data::Value> &fn,
                                                   Sad::Interpreter::Interpreter *interp)
            {
                static std::uintptr_t counter = 0;
                const std::uintptr_t h = ++counter;
                interpBuilderTable()[h] = InterpBuilderEntry{fn, interp};
                sad::ui::NavEntry e;
                e.build = &interpBuilderInvoke;
                e.data = reinterpret_cast<void *>(h);
                e.release = &interpBuilderRelease;
                return e;
            }
            /// (AR) يبني NavEntry مناسبًا للوسيط: **دالّة ص** ⇒ بانٍ (تفاعليّة)؛ عنصر ⇒ لقطة.
            ///      ⚠ نميّز بـisFunction() **فقط** لا isCallable(): الأخيرة true لـ
            ///      __WidgetBuilder__ (عنصرٌ مبنيّ، لا دالّة) فتُوقِع عنصرًا في مسار البانِي
            ///      ⇒ toFunction على عنصر = انهيار. (نظير درس الصفحة_الحالية.)
            [[nodiscard]] sad::ui::NavEntry interpNavEntryFor(const std::shared_ptr<Data::Value> &arg,
                                                Sad::Interpreter::Interpreter *interp)
            {
                if (arg && arg->isFunction())
                    return interpBuilderEntryOf(arg, interp);
                return interpPageEntry(arg);
            }

            /// (AR) حارس نوع وسيط التنقّل (HIGH-2، تكافؤ مع المترجم): يجب أن يكون **عنصر
            ///      واجهة** (WidgetBuilder تحديدًا) أو **دالّة بناء صفحة** (isFunction). أيّ
            ///      نوعٍ آخر (عدد/نص/لاشيء…) ليس صفحةً.
            ///      ⚠ تشديد (مراجعة Amelia النهائيّة، تكافؤ): نقبل الكائن **فقط إن كان
            ///      WidgetBuilder** لا أيّ `isObject` — لأنّ **كائن المستخدم** (مثيل صنف)
            ///      isObject=true في المفسّر لكنّه Struct في المترجم (يرفضه checkUiNavArgType
            ///      إذ Struct ∉ {Pointer, Function}). فقبول أيّ كائنٍ كان يجعل المفسّر يقبل
            ///      `انتقل(مثيل_صنف)` بينما المترجم يرفضه ⇒ تباعد. الآن كلاهما يرفض غير
            ///      العنصر. (نظير المترجم: العنصر Pointer، كائن المستخدم Struct.)
            ///      يرفضه المترجم بـSEM_TYPE_MISMATCH فنرفضه هنا بالمثل. ctx.error
            ///      [[noreturn]] (يرمي RuntimeAbort) نظير إيقاف الترجمة في المترجم.
            void checkNavArg(Sad::Interpreter::BuiltinContext &ctx,
                             const std::shared_ptr<Data::Value> &arg)
            {
                if (arg && arg->isFunction())
                    return; // دالّة بناء صفحة
                if (arg && arg->isObject())
                {
                    auto obj = arg->toObject();
                    if (obj && isWidgetBuilder(obj))
                        return; // عنصر واجهة فقط (لا كائن مستخدم)
                }
                ctx.error(::Sad::Errors::ErrorCode::SEM_TYPE_MISMATCH,
                          {{"expected", "\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1 \xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9 \xd8\xa3\xd9\x88 \xd8\xaf\xd8\xa7\xd9\x84\xd9\x91\xd8\xa9 \xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1 \xd8\xb5\xd9\x81\xd8\xad\xd8\xa9"}, // عنصر واجهة أو دالّة بناء صفحة
                           {"found", arg ? arg->toString() : "\xd9\x84\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xa1"}}); // لاشيء
            }

            /// (AR) الصفحة الحالية من القلب عبر buildCurrent (م1-ج، توحيد المسار): إن كان
            ///      الإدخال بانيًا استُدعي البانِي ⇒ يُعيد مقبض صفحةٍ مبنيّةٍ طازجة؛ وإلّا
            ///      فمقبض اللقطة. المقبض ⇒ Data::Value (أو فارغ إن لا صفحة). فيتطابق
            ///      طباعة_شجرة(الصفحة_الحالية) headless مع الرسم الحيّ ومع المترجم.
            [[nodiscard]] std::shared_ptr<Data::Value> interpCurrentPage()
            {
                const auto h = reinterpret_cast<std::uintptr_t>(sad::ui::nav().buildCurrent());
                auto &tbl = interpPageTable();
                const auto it = tbl.find(h);
                return it != tbl.end() ? it->second : std::make_shared<Data::Value>();
            }

            /**
             * @brief (AR) طباعة شجرة IRNode بشكل هرمي (للتصحيح والتحقّق) — تفويضٌ
             *        لمصدر الحقيقة الوحيد في المكتبة (features/graphics/…/print_tree.h)
             *        فلا يُكرَّر التنسيق في المحرّك ⇒ **تطابقُ مخرَجٍ بالبناء** مع
             *        المترجم (sad_print_tree يستدعي الدالة المكتبيّة نفسها). أيّ تغييرٍ
             *        للتنسيق يُجرى في المكتبة مرّةً واحدة فيسري على المحرّكين معًا.
             * @brief (EN) Hierarchical IRNode tree print — delegates to the single
             *        source of truth in the library so the format is not duplicated in
             *        the engine ⇒ byte-identical output by construction with the compiler.
             */
            void printIRNodeTree(const std::shared_ptr<sad::ui::IRNode> &node, int depth, int maxDepth)
            {
                sad::ui::printIRNodeTree(node, depth, std::cout, maxDepth);
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
                    if (isWidgetBuilder(obj))
                    {
                        auto *wb = static_cast<Sad::Interpreter::WidgetBuilder *>(obj);
                        printIRNodeTree(wb->getIRNode(), depth, maxDepth);
                        return;
                    }

                    // (AR) غير عنصر واجهة (كائن عاديّ) ⇒ لا نطبع شيئًا، مطابقةً للمترجم
                    //      (sad_print_tree يتجاهل ما لا يملك IRNode) ⇒ تكافؤ حتى في
                    //      حالة الحدّ (مثل الصفحة_الحالية على مكدّس فارغ).
                    // (EN) Non-widget object ⇒ print nothing, mirroring the compiler
                    //      (sad_print_tree ignores non-IRNode) ⇒ parity even at the edge.
                    return;
                }

                // (AR) قيمة غير كائن (فارغة/بدائيّة) ⇒ صامت، مطابقةً للمترجم (null ⇒ لا شيء).
                // (EN) Non-object value (empty/primitive) ⇒ silent, mirroring the compiler.
            }

        } // anonymous namespace

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) جسر التوحيد الكامل (Amelia LOW-1): يكشف مساعدات nav (المُعرَّفة في النطاق
        //      المجهول أعلاه) لـUIBridge ليصير مكدّس sad::ui::nav **المصدرَ الوحيد** للرسم
        //      الحيّ، فيُزال المكدّس المُوازي navigationStack_. الدوال أدناه ذات ربطٍ خارجيّ
        //      (تُعلَن في ui_bridge.h) وتفوّض للمساعدات الداخليّة في هذه الوحدة.
        // ═══════════════════════════════════════════════════════════════════════

        Data::Value navCurrentPageValue()
        {
            // (AR) الصفحة الحاليّة من nav (عبر buildCurrent: يستدعي البانِي إن وُجد ⇒ شجرةٌ
            //      طازجةٌ تفاعليّة). قيمةٌ فارغة إن كان المكدّس فارغًا (لم يُبذَر جذر).
            return *interpCurrentPage();
        }

        void navSeedRoot(const Data::Value &root, Interpreter *interp)
        {
            // (AR) ابذر جذر التطبيق في nav كإدخالٍ حاليّ (بانٍ إن دالّة ص، لقطة إن عنصر) ⇒
            //      يصير nav المكدّس الكامل شاملًا الجذر، فترسم rebuildUI من navCurrentPageValue
            //      موحَّدةً (جذرًا كانت أو صفحةً مُنتقَلًا إليها). reset لبداية نظيفة لكلّ تشغيل
            //      (نظير sad_app_run؛ يمنع تراكم مكدّسٍ static عبر تشغيلاتٍ في العمليّة الواحدة).
            sad::ui::nav().reset();
            sad::ui::nav().replace(interpNavEntryFor(std::make_shared<Data::Value>(root), interp));
            sad::ui::nav().takeDirty(); // المحتوى الابتدائيّ يُضبَط في run() مباشرةً
        }

        void registerUICoreBuiltins(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // (AR) اختصار لأسماء ثوابت نواة الواجهة
            namespace Bc = Builtins::Names::UICore;

            // ─── _محرك_واجهات(عنصر_جذر) — تشغيل التطبيق بشجرة عناصر تعريحية أو دالة بنّاء ───
            auto engine_fn = [&interpreter](Sad::Interpreter::BuiltinContext &ctx)
                -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
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
                        // (AR) من الكتالوج (RUN_UI_LAUNCH_FAILED) لا نصًّا يدويًّا — نفس
                        //      حدث فشل bridge.run المُرحَّل في تشغيل_تطبيق؛ نُبقي التدفّق.
                        std::cerr << Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                                         Sad::Errors::ErrorCode::RUN_UI_LAUNCH_FAILED, {})
                                  << std::endl;
                    }
                    return std::make_shared<Data::Value>();
                }

                if (!rootWidget.isObject() && !rootWidget.isMap())
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
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
            auto run_app_fn = [&interpreter](Sad::Interpreter::BuiltinContext &ctx)
                -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                    // "تشغيل_تطبيق() يحتاج عنصر واجهة أو دالة بنّاء"
                }
                const auto &rootWidget = *args[0];
                UIBridge bridge;
                bool success = bridge.run(rootWidget, const_cast<Interpreter *>(&interpreter));
                if (!success)
                {
                    // (AR) رسالة من الكتالوج (RUN_UI_LAUNCH_FAILED) لا نصًّا يدويًّا؛
                    //      نُبقي cerr + الاستمرار (لا ctx.error الذي يرمي) حفظًا للتدفّق.
                    std::cerr << Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                                     Sad::Errors::ErrorCode::RUN_UI_LAUNCH_FAILED, {})
                              << std::endl;
                }
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::RUN_APP), run_app_fn); // تشغيل_تطبيق

            // ─── طباعة_شجرة(عنصر) — طباعة شجرة العناصر للتصحيح ───
            auto print_tree_fn = [](Sad::Interpreter::BuiltinContext &ctx)
                -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (!args.empty())
                {
                    printWidgetTree(*args[0], 0);
                }
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::PRINT_TREE), print_tree_fn); // طباعة_شجرة

            // ─── دوال التنقل ───
            auto navigate_fn = [&interpreter](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                checkNavArg(ctx, args[0]); // حارس نوع (HIGH-2): عنصر أو دالّة
                // (AR) مصدر الحقيقة: مكدّس المكتبة. (م1-ج) دالّة ⇒ إدخال بانٍ (تفاعليّة عبر
                //      buildCurrent)؛ عنصر ⇒ لقطة. الجسر الحيّ يبقى بالتوازي (lockstep):
                //      navigateTo يخزّن نفس الوسيط (دالّة أو عنصر) فيتّسق الرسم الحيّ.
                sad::ui::nav().navigate(interpNavEntryFor(args[0], &interpreter));
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->navigateTo(*args[0]);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::NAVIGATE), navigate_fn);

            // ─── انتقل_بتحريك(صفحة, نوع_انتقال, مدة؟) — تنقل مع تحريك بصري ───
            // (AR) دالة الانتقال مع تحريك — تدعم أنواع مثل "ظهور"، "انزلاق_يمين"، "تكبير"
            // (EN) Navigate with page transition animation — supports types like "fadeIn", "slideRight"
            auto navigate_transition_fn = [&interpreter](Sad::Interpreter::BuiltinContext &ctx)
                -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                // (AR) يحتاج 2 وسائط على الأقل: الصفحة + نوع الانتقال
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                // "انتقل_بتحريك() يحتاج صفحة ونوع انتقال"

                std::string transType = args[1]->toString();
                float duration = 0.3f;
                if (args.size() >= 3)
                    duration = static_cast<float>(args[2]->toDouble());

                checkNavArg(ctx, args[0]); // حارس نوع (HIGH-2): عنصر أو دالّة
                sad::ui::nav().navigate(interpNavEntryFor(args[0], &interpreter)); // lockstep (دالّة⇒بانٍ)
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
            auto navigate_exit_transition_fn = [&interpreter](Sad::Interpreter::BuiltinContext &ctx)
                -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                // (AR) يحتاج 3 وسائط على الأقل: الصفحة + دخول + خروج
                if (args.size() < 3)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                // "انتقل_بتحريك_كامل() يحتاج صفحة ونوع دخول ونوع خروج"

                std::string entryType = args[1]->toString();
                std::string exitType = args[2]->toString();
                float duration = 0.3f;
                if (args.size() >= 4)
                    duration = static_cast<float>(args[3]->toDouble());

                checkNavArg(ctx, args[0]); // حارس نوع (HIGH-2): عنصر أو دالّة
                sad::ui::nav().navigate(interpNavEntryFor(args[0], &interpreter)); // lockstep (دالّة⇒بانٍ)
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->navigateWithExitTransition(*args[0], entryType, exitType, duration);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(
                std::string(Bc::NAVIGATE_EXIT_TRANSITION),
                navigate_exit_transition_fn); // انتقل_بتحريك_كامل

            auto back_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                // (Amelia LOW-1) بوّابة على نتيجة nav().back(): لا إعادة رسمٍ على مكدّسٍ
                //   فارغ (عودةٌ بلا صفحةٍ سابقة = لا-عمل، لا رسمٌ زائد ولا زيادة جيلٍ).
                if (sad::ui::nav().back())
                {
                    auto *bridge = UIBridge::active();
                    if (bridge)
                        bridge->navigateBack();
                }
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::BACK), back_fn);

            // ─── عودة_بتحريك(نوع_انتقال, مدة؟) — العودة مع تحريك بصري ───
            // (AR) العودة للصفحة السابقة مع تحريك انتقالي
            // (EN) Navigate back with visual page transition
            auto back_transition_fn = [](Sad::Interpreter::BuiltinContext &ctx)
                -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                std::string transType = "\xd8\xb8\xd9\x87\xd9\x88\xd8\xb1"; // ظهور (افتراضي)
                float duration = 0.3f;
                if (!args.empty())
                    transType = args[0]->toString();
                if (args.size() >= 2)
                    duration = static_cast<float>(args[1]->toDouble());

                // (Amelia LOW-1) بوّابة على نتيجة back: لا رسمَ على مكدّسٍ فارغ.
                if (sad::ui::nav().back())
                {
                    auto *bridge = UIBridge::active();
                    if (bridge)
                        bridge->navigateBackWithTransition(transType, duration);
                }
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(
                std::string(Bc::BACK_TRANSITION),
                back_transition_fn); // عودة_بتحريك

            auto root_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                sad::ui::nav().root(); // مصدر الحقيقة (عمق-فقط)؛ الجسر الحيّ بالتوازي
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->navigateToRoot();
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::BACK_TO_ROOT), root_fn);

            auto replace_fn = [&interpreter](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                checkNavArg(ctx, args[0]); // حارس نوع (HIGH-2): عنصر أو دالّة
                // (م1-ج) دالّة ⇒ بانٍ (تفاعليّة)؛ عنصر ⇒ لقطة. الجسر الحيّ بالتوازي.
                sad::ui::nav().replace(interpNavEntryFor(args[0], &interpreter));
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->replacePage(*args[0]);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::REPLACE_PAGE), replace_fn);

            // ─── دوال الثيم ───
            auto toggle_theme_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                sad::ui::toggleTheme();
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->rebuildUI();
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::TOGGLE_THEME), toggle_theme_fn);

            auto dark_mode_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                sad::ui::setTheme(sad::ui::ThemeMode::Dark);
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->rebuildUI();
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::DARK_MODE), dark_mode_fn);

            auto light_mode_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                sad::ui::setTheme(sad::ui::ThemeMode::Light);
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->rebuildUI();
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::LIGHT_MODE), light_mode_fn);

            auto is_dark_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                return std::make_shared<Data::Value>(sad::ui::isDarkMode());
            };
            fm.registerBuiltinFunction(std::string(Bc::IS_DARK), is_dark_fn);

            // ─── تحديث الحالة ───
            auto set_state_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->rebuildUI();
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::UPDATE_STATE), set_state_fn);

            // ─── أوقف_الانتشار — يوقف انتشار الحدث الجاري (rfcs#51) ───
            auto stop_prop_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                sad::ui::stopActiveEventPropagation();
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::STOP_PROPAGATION), stop_prop_fn);

            // ─── عنوان النافذة ───
            auto set_title_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isString())
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->setWindowTitle(args[0]->toString());
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::SET_TITLE), set_title_fn);

            // ─── عدد الصفحات ───
            auto page_count_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                // (AR) العمق من مكدّس المكتبة (مصدر الحقيقة) ⇒ يعمل headless ويطابق
                //      المترجم (sad_page_count). كان سابقًا من الجسر (0 بلا نافذة).
                return std::make_shared<Data::Value>(
                    static_cast<int64_t>(sad::ui::nav().depth()));
            };
            fm.registerBuiltinFunction(std::string(Bc::PAGE_COUNT), page_count_fn);

            // ─── الصفحة الحالية (حارس بنيويّ) ───
            // (AR) تُرجع الصفحة الحالية من مكدّس التنقّل المشترك (عبر جدول المقابض) ⇒
            //      طباعة_شجرة(الصفحة_الحالية()) تكشف الجذر المرسوم headless وتطابق
            //      المترجم (sad_current_page). خطة التنقّل §Q3 (حارس إلزاميّ).
            auto current_page_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                (void)ctx.args();
                return interpCurrentPage();
            };
            fm.registerBuiltinFunction(std::string(Bc::CURRENT_PAGE), current_page_fn);

            // ─── إغلاق النافذة ───
            auto close_window_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->closeWindow();
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bc::CLOSE_WINDOW), close_window_fn);

            // ─── عيّن_الحالة(دالة_تحديث) — تحديث حالة + إعادة بناء (مثل setState في Flutter) ───
            // (AR) يستدعي دالة التحديث أولاً ثم يُعيد بناء الواجهة
            // (EN) Calls update function first, then rebuilds UI (like Flutter's setState)
            auto set_state_v2_fn = [&interpreter](Sad::Interpreter::BuiltinContext &ctx)
                -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
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
                        // (AR) رسالة من الكتالوج (RUN_UI_STATE_ERROR) مع تفصيل الاستثناء
                        //      كـplaceholder؛ نُبقي cerr + الاستمرار حفظًا للتدفّق.
                        Sad::Errors::RenderContext sctx;
                        sctx.placeholders = {{"detail", e.what()}};
                        std::cerr << Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                                         Sad::Errors::ErrorCode::RUN_UI_STATE_ERROR, sctx)
                                  << std::endl;
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
            auto gen_web_fn = [&interpreter](Sad::Interpreter::BuiltinContext &ctx)
                -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
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
                        ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                        // "فشل استدعاء دالة البنّاء: ..."
                    }
                }

                // تحويل إلى IR
                UIBridge bridge;
                auto irRoot = bridge.convertToIR(actualWidget);
                if (!irRoot)
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
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
