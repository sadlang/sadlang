// ======================================================================
// call_builder.h - بنّاء استدعاءات الدوال / Function Call Builder
// ======================================================================
// (AR) Phase 6 — Step 4: صنف منفصل يجمع 7 methods لبناء استدعاءات الدوال:
//      buildFunctionCall (الرئيسية), buildMacroCallExpansion,
//      buildCallableObjectInvoke, buildSuperConstructorCall,
//      buildCallArgumentsList, fillDefaultCallArguments, buildMethodCall.
//      يحمل مرجعاً إلى SIRBuilder للوصول للحقول المشتركة والـhelpers.
// (EN) Phase 6 — Step 4: standalone class grouping 7 builders for
//      function call construction. Holds reference to SIRBuilder.
// ======================================================================

#pragma once

#include "../sir_types.h"
#include "../sir_instruction.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

// (AR) إعلانات مسبقة لأنواع AST الحقيقية (FunctionCallNode هو alias لـCallExpr)
// (EN) Forward decls of real AST types (FunctionCallNode is alias for CallExpr)
namespace Sad { namespace AST {
    class CallExpr;
    class MethodCallExpr;
}}

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            class SIRBuilder;
            struct BuildResult;

            /**
             * @brief (AR) بنّاء استدعاءات الدوال (7 methods)
             * @brief (EN) Builder for function calls (7 methods)
             */
            class CallBuilder
            {
            public:
                explicit CallBuilder(SIRBuilder &b) : b_(b) {}

                BuildResult buildFunctionCall(Sad::AST::CallExpr *call);

                std::optional<BuildResult> buildMacroCallExpansion(Sad::AST::CallExpr *call, const std::string &funcName);

                std::optional<BuildResult> buildCallableObjectInvoke(Sad::AST::CallExpr *call, const std::string &funcName);

                std::optional<BuildResult> buildSuperConstructorCall(Sad::AST::CallExpr *call, const std::string &funcName);

                bool buildCallArgumentsList(Sad::AST::CallExpr *call, std::vector<SIROperand> &outArgOperands, std::vector<BuildResult> &outArgResults);

                void fillDefaultCallArguments(Sad::AST::CallExpr *call, const std::string &funcName, std::vector<SIROperand> &argOperands, std::vector<BuildResult> &argResults);

                BuildResult buildMethodCall(Sad::AST::MethodCallExpr *methodCallExpr);

            private:
                SIRBuilder &b_;

                // (AR) تطابق المحرّكين — طبع قيمة موسومة لتعداد جبريّ:
                //      «اطبع(مربع(٩))» كان المترجم يطبع فيه عنوانَ بنية الحالة (بينما المفسّر
                //      يطبع «شكل.مربع(٩)») — فراغٌ/عنوانٌ صامت يخالف بوّابة تطابق المحرّكين.
                //      إن كان `argExpr` بناءَ حالةٍ جبريّةٍ مباشرًا (CallExpr على اسم حالةٍ في
                //      adtEnumTable_، غير مظلَّلٍ بدالّة/متغيّر/إغلاق مُصرَّح — نظير حسم التظليل)
                //      يبني هذا سلسلة «تعداد.حالة(حقل، …)» من حمولة البنية المبنيّة سلفًا
                //      (ENUM_GET_PAYLOAD ⇒ STRING_CONCAT، بلا إعادة تقييم) ويُعيد true.
                // (EN) Engine parity — printing a tagged algebraic-enum value: «print(مربع(9))»
                //      used to print the variant struct's address (while the interpreter prints
                //      «شكل.مربع(9)») — a silent address/blank violating the engine-parity gate.
                //      If `argExpr` is a direct algebraic-enum constructor (a CallExpr on a variant
                //      name in adtEnumTable_, not shadowed by a declared function/variable/closure —
                //      mirroring the shadow decision), this builds the «Enum.Variant(f, …)» string
                //      from the already-built struct payload (ENUM_GET_PAYLOAD ⇒ STRING_CONCAT, no
                //      re-evaluation) and returns true.
                bool tryBuildAdtVariantDisplay(Sad::AST::Expr *argExpr,
                                               const BuildResult &argRes,
                                               const SIROperand &argOp,
                                               SIROperand &outOp,
                                               BuildResult &outRes);

                // (AR) مُساعد مشترك: يبني وسائط الاستدعاء ويُصدر CLOSURE_CALL على مؤشّر
                //      إغلاق سبق بناؤه (callee غير مباشر: IndexExpr/LambdaExpr). يُعيد سجلّ
                //      النتيجة بنوع Integer، أو BuildResult فارغة إن كان مؤشّر الإغلاق فارغًا.
                // (EN) Shared helper: builds call arguments and emits CLOSURE_CALL on an
                //      already-built closure pointer (indirect callee: IndexExpr/LambdaExpr).
                //      Returns the Integer-typed result register, or an empty BuildResult
                //      if the closure pointer is empty.
                BuildResult emitClosureCallFromCallee(const BuildResult &closureResult,
                                                      Sad::AST::CallExpr *call,
                                                      const std::string &comment);
            };

        } // namespace SIR
    }     // namespace Compiler
} // namespace Sad
