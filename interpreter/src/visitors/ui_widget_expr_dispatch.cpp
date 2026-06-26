/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: interpreter/src/visitors/ui_widget_expr_dispatch.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * (AR) التطبيق الرفيع لـ ExpressionEvaluator::visitUIWidgetExpr (seam 2، م2-أ من
 *      sadlang-rfcs#10): يُفوّض إلى الجسر إن كان مثبَّتًا، وإلّا يُرجع قيمة فارغة
 *      (لا رسومات، لا انهيار). يبقى في sad_core ولا يضمّ أيّ ترويسة من sad_ui.
 *
 *      **وحدة ترجمة مستقلّة عمدًا:** هذا الرمز وحده هنا (لا حالة بذرة أخرى) كي
 *      يستطيع interpreter_test_stubs.cpp إشباعه في اختبارات comprehensive دون
 *      سحب هذا الـobj؛ ولو دُمج مع uiEvalBridge/externalModuleProviders (المُشار
 *      إليهما دائمًا من القلب) لسُحب الـobj دائمًا فوقع تعارض LNK2005 مع الـstub.
 *
 * (EN) Thin ExpressionEvaluator::visitUIWidgetExpr (seam 2): delegates to the
 *      installed bridge, else yields an empty value. Stays in sad_core; no sad_ui.
 *      Deliberately its OWN translation unit so interpreter_test_stubs can satisfy
 *      this symbol in the comprehensive tests without pulling this object — keeping
 *      it together with the always-referenced uiEvalBridge/externalModuleProviders
 *      would force the object to be pulled, causing an LNK2005 clash with the stub.
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "ui/ui_eval_bridge.h"
#include "visitors/expression_evaluator.h"
#include "ui_nodes.h"
#include "value.h"

namespace Sad
{
    namespace Interpreter
    {
        // ─── (AR) التطبيق الرفيع: يُفوّض لتعبير عنصر الواجهة إلى الجسر ───
        // (EN) Thin delegation: forwards a UI widget expression to the installed bridge.
        void ExpressionEvaluator::visitUIWidgetExpr(AST::UIWidgetExprNode &node)
        {
            if (IUIEvalBridge *b = uiEvalBridge())
            {
                b->evalWidgetExpr(*this, node);
                return;
            }
            // (AR) لم تُحمَّل الرسومات (sad-build/بلا sad_ui): قيمة فارغة بلا انهيار.
            // (EN) Graphics not loaded (sad-build / no sad_ui): empty value, no crash.
            lastResult_ = Data::Value();
        }
    }
}
