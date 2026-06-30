/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: interpreter/include/ui/ui_eval_bridge.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * (AR) بذرة عكس الاعتماد بين القلب (sad_core) وطبقة جسر الواجهات (sad_graphics_bridge).
 *      المرحلة 2-أ من RFC «قلب موحَّد بحدود داخلية» (sadlang-rfcs#10).
 *
 *      الهدف: ألّا يعتمد `sad_core` على `sad_ui` إطلاقًا. بدل أن يستدعي القلبُ
 *      الواجهاتِ مباشرةً، يعرض القلب هذه الواجهة المجرّدة؛ وتسجّل طبقة الجسر
 *      تطبيقها فيها وقت التشغيل (يثبّته `sad-run`). فإن لم تُحمَّل الرسومات بقي
 *      المؤشّر فارغًا والقلب يعمل بلا رسومات.
 *
 * (EN) Dependency-inversion seam between the core (sad_core) and the UI bridge
 *      layer (sad_graphics_bridge). Phase 2-A of RFC sadlang-rfcs#10. The core no longer
 *      calls into sad_ui; it exposes this abstract interface and the bridge installs
 *      its implementation at runtime (wired by sad-run).
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_INTERPRETER_UI_EVAL_BRIDGE_H
#define SAD_INTERPRETER_UI_EVAL_BRIDGE_H

#include <functional>
#include <vector>

namespace Sad
{
    namespace Data
    {
        class Value;
    }
    namespace AST
    {
        class UIWidgetExprNode;
        class MethodCallExpr;
    }

    namespace Interpreter
    {
        class ExpressionEvaluator;

        // ═══════════════════════════════════════════════════════════════════
        // (AR) بذرة الزائر — يملؤها sad_graphics_bridge (seams 2 و3)
        // (EN) Evaluator seam — implemented by sad_graphics_bridge (seams 2 and 3)
        // ═══════════════════════════════════════════════════════════════════
        struct IUIEvalBridge
        {
            virtual ~IUIEvalBridge() = default;

            // (AR) seam 2: تقييم تعبير عنصر واجهة (نص()، عمود، زر()...).
            //      يضع النتيجة في القلب عبر العضو lastResult_ (صداقة).
            // (EN) seam 2: evaluate a UI widget expression; stores result on the core.
            virtual void evalWidgetExpr(ExpressionEvaluator &ev, AST::UIWidgetExprNode &node) = 0;

            // (AR) seam 3: محاولة معالجة استدعاء طريقة على WidgetBuilder
            //      (سلسلة المعدّلات .حجم().لون()...). يُرجع true إن عولِج فعلًا.
            // (EN) seam 3: try to handle a method call on a WidgetBuilder modifier chain.
            //      Returns true iff it handled the call.
            virtual bool tryWidgetMethodCall(ExpressionEvaluator &ev,
                                             const Data::Value &objectValue,
                                             AST::MethodCallExpr &node) = 0;
        };

        // (AR) يُثبَّت قبل إنشاء المفسّر من كلّ ثنائيّ ينفّذ كود واجهات (sad-run،
        //      wasm، profiler) عبر installSadGraphicsBridge()؛ القلب يقرؤه في المسار
        //      الساخن عبر uiEvalBridge(). العقد: التثبيت يحدث مرّة على الخيط الرئيسيّ
        //      قبل تنفيذ أيّ كود مستخدم.
        // (EN) Installed before interpreter creation by every binary that runs UI code
        //      (sad-run, wasm, profiler) via installSadGraphicsBridge(); read on the hot path
        //      via uiEvalBridge(). Contract: install once, on the main thread, before
        //      any user code executes.
        void setUIEvalBridge(IUIEvalBridge *bridge);
        IUIEvalBridge *uiEvalBridge();

        // ═══════════════════════════════════════════════════════════════════
        // (AR) بذرة المدمجات (seam 1) — مزوّدو وحدات خارجيّون يسجّلون أنفسهم في
        //      BuiltinModuleRegistry. القلب يستدعي كلّ مزوّد بعد وحداته الأساسية.
        // (EN) Builtins seam (seam 1) — external module providers register themselves
        //      into the BuiltinModuleRegistry; the core invokes each after its own.
        // ═══════════════════════════════════════════════════════════════════
        using ModuleProvider = std::function<void()>;
        std::vector<ModuleProvider> &externalModuleProviders();
    }
}

#endif // SAD_INTERPRETER_UI_EVAL_BRIDGE_H
