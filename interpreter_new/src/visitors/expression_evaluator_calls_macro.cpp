/**
 * @file expression_evaluator_calls_macro.cpp
 * @brief (AR) معالجة استدعاءات الماكروز — مُستخرج من visitCallExpr
 * @brief (EN) Macro call handling — extracted from visitCallExpr
 */

#include <string>
#include "expression_evaluator.h"
#include "statement_executor.h"
#include "ast_node.h"
#include "statements.h"
#include "declarations.h"
#include "class_nodes.h"
#include "expressions.h"
#include "advanced_expr_nodes.h"
#include "class_manager.h"
#include "object_instance.h"
#include "error_manager.h"
#include "ownership_manager.h"
#include "exception.h"
#include <vector>
#include <string>

namespace Sad
{
    namespace Interpreter
    {

        using namespace Data;
        using namespace AST;
        using namespace Lexer;

        // ═══════════════════════════════════════════════════════════════
        // (AR) معالجة استدعاء الماكرو: اسم!(وسائط)
        //      الماكرو يُنفّذ في نطاق فرعي (hygiene) مع نسخ للخلف للمتغيرات
        //      يدعم: معاملات متغيرة، قيمة إرجاع، ماكروز متداخلة
        // (EN) Macro call handling: name!(args)
        //      Macro executes in child scope (hygiene) with copy-back for variables
        //      Supports: variadic params, return values, nested macros
        // ═══════════════════════════════════════════════════════════════
        void ExpressionEvaluator::handleMacroCallImpl(CallExpr &node)
        {
            auto *calleeVar = dynamic_cast<VariableExpr *>(node.callee.get());
            if (!calleeVar)
            {
                throw Interpreter::RuntimeError(
                    "(AR) خطأ: استدعاء ماكرو يجب أن يكون بالاسم. (EN) Macro call must use a name.",
                    node.position);
            }

            auto *macroDef = statementExecutor_.findMacro(calleeVar->name);
            if (!macroDef)
            {
                throw Interpreter::RuntimeError(
                    "(AR) الماكرو '" + calleeVar->name + "' غير معرّف.\n"
                                                         "تأكد من تعريف الماكرو قبل استدعائه.\n"
                                                         "(EN) Macro '" +
                        calleeVar->name + "' is not defined.\n"
                                          "Make sure the macro is defined before calling it.",
                    node.position);
            }

            // (AR) التحقق من عدد المعاملات مع دعم المعاملات المتغيرة
            // (EN) Check argument count with variadic support
            size_t requiredParams = macroDef->isVariadic
                                        ? macroDef->params.size() - 1
                                        : macroDef->params.size();

            if (macroDef->isVariadic)
            {
                if (node.arguments.size() < requiredParams)
                {
                    throw Interpreter::RuntimeError(
                        "(AR) الماكرو '" + calleeVar->name + "' يتوقع على الأقل " +
                            std::to_string(requiredParams) + " معاملات لكن حصل على " +
                            std::to_string(node.arguments.size()) + ".\n"
                                                                    "(EN) Macro '" +
                            calleeVar->name + "' expects at least " +
                            std::to_string(requiredParams) + " arguments but got " +
                            std::to_string(node.arguments.size()) + ".",
                        node.position);
                }
            }
            else
            {
                if (node.arguments.size() != macroDef->params.size())
                {
                    throw Interpreter::RuntimeError(
                        "(AR) الماكرو '" + calleeVar->name + "' يتوقع " +
                            std::to_string(macroDef->params.size()) + " معاملات لكن حصل على " +
                            std::to_string(node.arguments.size()) + ".\n"
                                                                    "(EN) Macro '" +
                            calleeVar->name + "' expects " +
                            std::to_string(macroDef->params.size()) + " arguments but got " +
                            std::to_string(node.arguments.size()) + ".",
                        node.position);
                }
            }

            // ═══════════════════════════════════════════════════════════
            // (AR) أمان النطاق (Macro Hygiene):
            //      نستخدم نطاق FUNCTION لمنع `define()` من الصعود لتعديل
            //      متغيرات النطاق الخارجي — وهذا يضمن عزل الماكرو تماماً
            // (EN) Macro Hygiene:
            //      Use FUNCTION scope so `define()` won't walk up to modify
            //      outer scope variables — ensures complete macro isolation
            // ═══════════════════════════════════════════════════════════
            // ═══════════════════════════════════════════════════════════
            // (AR) تقييم جميع الوسائط قبل دخول نطاق الماكرو
            //      هذا ضروري لمنع تداخل الأسماء عند الماكروز المتداخلة —
            //      بدونه، وسيط يحمل اسم معامل الماكرو سيقرأ القيمة الخاطئة
            // (EN) Evaluate ALL arguments BEFORE entering macro scope
            //      Essential to prevent name collision in nested macros —
            //      without this, an arg with same name as param reads wrong value
            // ═══════════════════════════════════════════════════════════
            std::vector<Value> argValues(requiredParams);
            std::vector<std::string> argVarNames(requiredParams);

            for (size_t i = 0; i < requiredParams; ++i)
            {
                // (AR) تحقق إذا كان الوسيط تعبير متغير — لدعم النسخ للخلف
                // (EN) Check if argument is a variable expression — for copy-back
                auto *varExpr = dynamic_cast<AST::VariableExpr *>(node.arguments[i].get());
                if (varExpr)
                {
                    argVarNames[i] = varExpr->name;
                }

                // (AR) تقييم الوسيط في النطاق الخارجي (قبل دخول نطاق الماكرو)
                // (EN) Evaluate argument in outer scope (before entering macro scope)
                node.arguments[i]->accept(*this);
                argValues[i] = lastResult_;
            }

            // (AR) تقييم الوسائط المتغيرة أيضاً قبل دخول النطاق
            // (EN) Evaluate variadic arguments also before entering scope
            std::vector<Value> variadicArgValues;
            if (macroDef->isVariadic)
            {
                for (size_t i = requiredParams; i < node.arguments.size(); ++i)
                {
                    node.arguments[i]->accept(*this);
                    variadicArgValues.push_back(lastResult_);
                }
            }

            // (AR) الآن ندخل نطاق الماكرو المعزول
            // (EN) Now enter the isolated macro scope
            variableManager_.enterScope(Data::ScopeType::FUNCTION, "macro:" + calleeVar->name);

            // (AR) ربط القيم المُقيّمة مسبقاً كمعاملات في نطاق الماكرو
            // (EN) Bind pre-evaluated values as parameters in macro scope
            for (size_t i = 0; i < requiredParams; ++i)
            {
                variableManager_.define(macroDef->params[i], argValues[i]);
            }

            // (AR) ربط المعامل المتغير كمصفوفة
            // (EN) Bind variadic parameter as array
            if (macroDef->isVariadic)
            {
                variableManager_.define(macroDef->params.back(), Value(variadicArgValues));
            }

            // (AR) تنفيذ جسم الماكرو — مع دعم قيمة الإرجاع (ماكرو تعبيري)
            // (EN) Execute macro body — with return value support (expression macro)
            Value macroResult;
            bool hasReturnValue = false;

            if (macroDef->body)
            {
                auto savedFlowControl = statementExecutor_.getFlowControl();
                statementExecutor_.resetFlowControl();

                macroDef->body->accept(statementExecutor_);

                // (AR) التحقق من وجود جملة ارجع — ماكرو تعبيري
                // (EN) Check for return statement — expression macro
                if (statementExecutor_.getFlowControl() == FlowControl::RETURN)
                {
                    macroResult = statementExecutor_.getReturnValue();
                    hasReturnValue = true;
                    statementExecutor_.resetFlowControl();
                }
            }

            // (AR) النسخ للخلف: جمع القيم المعدّلة قبل الخروج من النطاق
            //      ثم تطبيقها في النطاق الخارجي بعد الخروج
            // (EN) Copy-back: collect modified values before exiting scope
            //      then apply them in outer scope after exit
            std::vector<std::pair<std::string, Value>> copyBackValues;
            for (size_t i = 0; i < requiredParams; ++i)
            {
                if (!argVarNames[i].empty())
                {
                    const auto &paramName = macroDef->params[i];
                    if (variableManager_.exists(paramName))
                    {
                        copyBackValues.push_back({argVarNames[i], variableManager_.get(paramName)});
                    }
                }
            }

            // (AR) الخروج من نطاق الماكرو (أمان النطاق)
            // (EN) Exit macro scope (hygiene)
            variableManager_.exitScope();

            // (AR) تطبيق النسخ للخلف في النطاق الخارجي
            // (EN) Apply copy-back in outer scope
            for (auto &[varName, modifiedValue] : copyBackValues)
            {
                variableManager_.assign(varName, modifiedValue);
            }

            // (AR) تعيين قيمة الإرجاع إن وُجدت
            // (EN) Set return value if present
            if (hasReturnValue)
            {
                lastResult_ = macroResult;
            }
        }

    } // namespace Interpreter
} // namespace Sad
