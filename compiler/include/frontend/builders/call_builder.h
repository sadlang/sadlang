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
            };

        } // namespace SIR
    }     // namespace Compiler
} // namespace Sad
