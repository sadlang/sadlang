/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: interpreter/src/visitors/ui_eval_bridge_core.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * (AR) الجانب القلبيّ من بذرة عكس الاعتماد (sadlang-rfcs#10، م2-أ):
 *      تخزين مؤشّر بذرة الزائر (ذرّيّ) + قائمة مزوّدي الوحدات الخارجيّين.
 *      هذا الملفّ **يبقى في sad_core** ولا يضمّ أيّ ترويسة من sad_ui.
 *
 *      ملاحظة: التطبيق الرفيع لـ visitUIWidgetExpr في ملفّ منفصل
 *      (ui_widget_expr_dispatch.cpp) عمدًا — كي يُشبِع interpreter_test_stubs ذلك
 *      الرمز في اختبارات comprehensive دون سحب هذا الملفّ (الذي يُسحب دائمًا لأنّ
 *      القلب يشير إلى uiEvalBridge/externalModuleProviders)، فلا يقع LNK2005.
 *
 * (EN) Core side of the dependency-inversion seam (RFC sadlang-rfcs#10, Phase 2-A):
 *      holds the (atomic) evaluator-bridge pointer + external module-provider list.
 *      Stays in sad_core; includes no sad_ui header.
 *
 *      Note: the thin visitUIWidgetExpr lives in a SEPARATE TU
 *      (ui_widget_expr_dispatch.cpp) on purpose, so interpreter_test_stubs can
 *      satisfy that symbol in the comprehensive tests WITHOUT pulling this object
 *      (which is always pulled because the core references uiEvalBridge/
 *      externalModuleProviders), avoiding an LNK2005 multiply-defined error.
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "ui/ui_eval_bridge.h"

#include <atomic>

namespace Sad
{
    namespace Interpreter
    {
        // ─── (AR) مؤشّر بذرة الزائر (يُثبَّت من sad-run قبل أيّ خيط) ───
        // (AR) ذرّيّ لأنّ uiEvalBridge() يُقرأ في المسار الساخن من goroutines
        //      محتملة بينما setUIEvalBridge() قد يكتب؛ relaxed كافٍ (قيمة مؤشّر واحدة
        //      تُكتب مرّة عند التثبيت قبل تنفيذ أيّ كود مستخدم).
        // (EN) Atomic: uiEvalBridge() is read on the OOP hot path from possible
        //      goroutines while setUIEvalBridge() may write. Relaxed suffices (single
        //      install before any user code runs).
        namespace
        {
            std::atomic<IUIEvalBridge *> g_uiEvalBridge{nullptr};
        }

        void setUIEvalBridge(IUIEvalBridge *bridge)
        {
            g_uiEvalBridge.store(bridge, std::memory_order_relaxed);
        }

        IUIEvalBridge *uiEvalBridge()
        {
            return g_uiEvalBridge.load(std::memory_order_relaxed);
        }

        // ─── (AR) قائمة مزوّدي الوحدات الخارجيّين (seam 1) ───
        std::vector<ModuleProvider> &externalModuleProviders()
        {
            static std::vector<ModuleProvider> providers;
            return providers;
        }
    }
}
