/**
 * ════════════════════════════════════════════════════════════════
 * ملفّ: interpreter/src/ui/ui_module_registration.cpp
 * ════════════════════════════════════════════════════════════════
 *
 * (AR) seam 1 + التثبيت (م2-أ، sadlang-rfcs#10): تسجيل وحدات الواجهات
 *      (رسومات/_محرك_واجهات/_صوت/...) في BuiltinModuleRegistry، ونقطة التثبيت
 *      installSadUIBridge() التي يستدعيها sad-run. نُقِلت كتل التسجيل حرفيًّا من
 *      builtin_registry.cpp في القلب إلى هنا (مكتبة الجسر).
 * (EN) seam 1 + install (Phase 2-A): registers the UI modules into the
 *      BuiltinModuleRegistry and exposes installSadUIBridge() called by sad-run.
 *      The registration blocks were moved verbatim out of the core's
 *      builtin_registry.cpp into the bridge library.
 * ════════════════════════════════════════════════════════════════
 */

#include "builtin_module_registry.h"
#include "interpreter_core.h"
#include "ui_builtins.h"
#include "ui_module_builtins.h"
#include "widget_builtins.h"
#include "ui_eval_bridge_impl.h"

namespace Sad
{
    namespace Interpreter
    {
        // (AR) seam 1: تُستدعى عبر externalModuleProviders() من القلب.
        void registerSadUIModules()
        {
            auto &registry = BuiltinModuleRegistry::getInstance();

            // ─── رسومات / Graphics — عناصر واجهة المستخدم التصريحية ───
            // (AR) يُسجِّل دوال بناء العناصر (~30 دالة) + محرك الواجهات عند `استورد رسومات`
            // (EN) Registers widget builder functions (~30) + UI engine on `استورد رسومات`
            registry.registerModule({"\xd8\xb1\xd8\xb3\xd9\x88\xd9\x85\xd8\xa7\xd8\xaa", // رسومات
                                     "\xd8\xb9\xd9\x86\xd8\xa7\xd8\xb5\xd8\xb1 \xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xb5\xd8\xb1\xd9\x8a\xd8\xad\xd9\x8a\xd8\xa9",
                                     // عناصر واجهة المستخدم التصريحية
                                     [](Interpreter &interp)
                                     {
                                         // (AR) تسجيل دوال بناء العناصر (نص، زر، عمود، ...)
                                         registerWidgetBuiltins(interp);
#if !defined(SAD_NO_SDL2) || defined(SAD_WASM_BUILD) || defined(SAD_PLATFORM_ANDROID)
                                         // (AR) تسجيل محرك الواجهات (تشغيل_تطبيق، التنقل، ...)
                                         registerUIBuiltins(interp);
#endif
                                     },
                                     {}});

            // ─── _محرك_واجهات / UI Engine — محرك واجهات داخلي ───
            // يسجل جميع الدوال عند استيراد _محرك_واجهات (تحميل كسول)
            registry.registerModule({"_\xd9\x85\xd8\xad\xd8\xb1\xd9\x83_\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa7\xd8\xaa", // _محرك_واجهات
                                     "\xd9\x85\xd8\xad\xd8\xb1\xd9\x83 \xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa7\xd8\xaa \xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaf\xd8\xa7\xd8\xae\xd9\x84\xd9\x8a",
                                     // محرك واجهات المستخدم الداخلي
                                     [](Interpreter &interp)
                                     {
#if !defined(SAD_NO_SDL2) || defined(SAD_WASM_BUILD) || defined(SAD_PLATFORM_ANDROID)
                                         registerUIBuiltins(interp); // يسجل جميع الوحدات الفرعية
#endif
                                     },
                                     {}});

            // ─── وحدات الخدمات الفرعية (تحميل كسول مستقل) ───
            registry.registerModule({"_\xd8\xb5\xd9\x88\xd8\xaa",                                                 // _صوت
                                     "\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd8\xa7\xd9\x84\xd8\xb5\xd9\x88\xd8\xaa", // نظام الصوت
                                     [](Interpreter &interp)
                                     { registerUIAudioBuiltins(interp); },
                                     {}});

            registry.registerModule({"_\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa\xd8\xa7\xd8\xaa",                                                 // _مؤقتات
                                     "\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd8\xa7\xd9\x84\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa\xd8\xa7\xd8\xaa", // نظام المؤقتات
                                     [](Interpreter &interp)
                                     { registerUITimerBuiltins(interp); },
                                     {}});

            registry.registerModule({"_\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86",                                                 // _تخزين
                                     "\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86", // نظام التخزين
                                     [](Interpreter &interp)
                                     { registerUIStorageBuiltins(interp); },
                                     {}});

            registry.registerModule({"_\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9",                                                 // _شبكة
                                     "\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd8\xa7\xd9\x84\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9", // نظام الشبكة
                                     [](Interpreter &interp)
                                     { registerUINetworkBuiltins(interp); },
                                     {}});

            registry.registerModule({"_\xd8\xaa\xd8\xb4\xd9\x81\xd9\x8a\xd8\xb1",                                                 // _تشفير
                                     "\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xb4\xd9\x81\xd9\x8a\xd8\xb1", // نظام التشفير
                                     [](Interpreter &interp)
                                     { registerUICryptoBuiltins(interp); },
                                     {}});

            registry.registerModule({"_\xd9\x85\xd9\x86\xd8\xb5\xd8\xa9",                                                         // _منصة
                                     "\xd8\xae\xd8\xaf\xd9\x85\xd8\xa7\xd8\xaa \xd8\xa7\xd9\x84\xd9\x85\xd9\x86\xd8\xb5\xd8\xa9", // خدمات المنصة
                                     [](Interpreter &interp)
                                     { registerUIPlatformBuiltins(interp); },
                                     {}});
        }

        // (AR) نقطة التثبيت (يستدعيها كلّ ثنائيّ ينفّذ كود واجهات قبل إنشاء المفسّر:
        //      sad-run، wasm، profiler...). عديمة التكرار: استدعاؤها مرّتين لا يُسجّل
        //      الوحدات مرّتين ولا يدفع المزوّد مرّتين (يمنع ازدواج registerModule).
        // (EN) Install point (called by every binary that executes UI code before
        //      creating the interpreter: sad-run, wasm, profiler...). Idempotent:
        //      a second call neither re-registers modules nor double-pushes the
        //      provider, preventing duplicate registerModule entries.
        void installSadUIBridge()
        {
            static bool installed = false;
            if (installed)
                return;
            installed = true;

            static UIEvalBridgeImpl impl;
            setUIEvalBridge(&impl);
            externalModuleProviders().push_back(&registerSadUIModules);
        }
    }
}
