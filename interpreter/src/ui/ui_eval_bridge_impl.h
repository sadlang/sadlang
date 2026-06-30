/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: interpreter/src/ui/ui_eval_bridge_impl.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * (AR) تطبيق بذرة عكس الاعتماد في طبقة الجسر (sad_graphics_bridge، م2-أ من sadlang-rfcs#10).
 *      يُجمَّع هذا الملفّ ضمن مكتبة الجسر (لا القلب)؛ يصل لأعضاء ExpressionEvaluator
 *      الخاصّة عبر الصداقة لتنفيذ منطق الواجهات (المنقول من القلب) خارجه.
 * (EN) Bridge-side implementation of the dependency-inversion seam (sad_graphics_bridge,
 *      Phase 2-A). Compiled into the bridge library, not the core.
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_INTERPRETER_UI_EVAL_BRIDGE_IMPL_H
#define SAD_INTERPRETER_UI_EVAL_BRIDGE_IMPL_H

#include "ui/ui_eval_bridge.h" // (AR) بادئة ui/ للاتّساق (تَحلّ في كلّ الأهداف)
#include "ui/sad_graphics_bridge.h"  // (AR) إعلان installSadGraphicsBridge العامّ (مصدر وحيد)

namespace Sad
{
    namespace Interpreter
    {
        // (AR) التطبيق الموحَّد للبذرتين 2 و3 (صديق ExpressionEvaluator).
        // (EN) Unified implementation of seams 2 and 3 (friend of ExpressionEvaluator).
        class UIEvalBridgeImpl : public IUIEvalBridge
        {
        public:
            void evalWidgetExpr(ExpressionEvaluator &ev, AST::UIWidgetExprNode &node) override;
            bool tryWidgetMethodCall(ExpressionEvaluator &ev,
                                     const Data::Value &objectValue,
                                     AST::MethodCallExpr &node) override;
        };

        // (AR) seam 1: يسجّل وحدات الواجهات (رسومات/_صوت/...) في BuiltinModuleRegistry.
        // (EN) seam 1: registers UI modules into the BuiltinModuleRegistry.
        void registerSadUIModules();

        // (AR) installSadGraphicsBridge() مُعلَنة في الترويسة العامّة ui/sad_graphics_bridge.h.
        // (EN) installSadGraphicsBridge() is declared in the public header ui/sad_graphics_bridge.h.
    }
}

#endif // SAD_INTERPRETER_UI_EVAL_BRIDGE_IMPL_H
