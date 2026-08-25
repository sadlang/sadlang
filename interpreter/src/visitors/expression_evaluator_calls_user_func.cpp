/**
 * @file expression_evaluator_calls_user_func.cpp
 * @brief (AR) تنفيذ جسم دالة المستخدم — مُستخرج من visitCallExpr
 * @brief (EN) User function body execution — extracted from visitCallExpr
 *
 * (AR) يحتوي على: حقن المتغيرات الملتقطة، تقييم القيم الافتراضية، فحص الأنواع،
 *      العقود البرمجية (يتطلب/يضمن)، قيود الأنواع (حيث)، تنفيذ الجسم،
 *      الدوال غير المتزامنة، المولّدات، وتحديث الإغلاقات.
 * (EN) Contains: capture injection, default value evaluation, type checking,
 *      design by contract (requires/ensures), where constraints, body execution,
 *      async functions, generators, and closure update.
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
#include "tagged_enum_keys.h" // (AR) مفاتيح القيمة الموسومة (SoT) / (EN) tagged-value keys (SoT)
#include "error_manager.h"
#include "runtime_throw.h"
#include "ownership_manager.h"
// (AR) SEM045 (RFC عقد الغياب): مابِع الصرامة المشترك strictnessFromOwnershipMode
// (EN) SEM045 (absence-contract RFC): shared strictness mapper
#include "null_safety/null_safety_analyzer.h"
#include "visitors/sem045_report.h" // (AR) باب إبلاغ SEM045 الواحد / (EN) single SEM045 reporting door
#include "runtime_throw.h"
#include "user_thrown.h"
#include "async_runtime.h"
#include "profiler_hooks.h"
#include <atomic>
#include <cmath>
#include <climits>
#include <iostream>
#include "sad_type_system.h"
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <set>

namespace Sad
{
    namespace Interpreter
    {

        using namespace Data;
        using namespace AST;
        using namespace Lexer;

        // (AR) دالة مساعدة للحصول على اسم الملف من مدير الأخطاء
        // (EN) Helper function to get filename from error manager
        static inline std::string getUserFuncSourceFilename()
        {
            const auto &fn = Sad::Errors::ErrorManager::getInstance().getSourceFilename();
            return fn.empty() ? "<input>" : fn;
        }

        // ═══════════════════════════════════════════════════════════════
        // (AR) تنفيذ جسم دالة المستخدم بعد resolve الدالة وتقييم الوسائط
        // (EN) Execute resolved user function body after function resolution
        // ═══════════════════════════════════════════════════════════════
        void ExpressionEvaluator::executeUserFunctionBody(
            CallExpr &node,
            const std::string &funcName,
            std::shared_ptr<FunctionDefinition> func,
            std::vector<Value> &arguments)
        {
            // ═══════════════════════════════════════════════════════════
            // (AR) خطاف مصحح الأداء — دالة مستخدم
            // (EN) Profiler hook — user function
            // ═══════════════════════════════════════════════════════════
            Sad::Tools::ProfileGuard _userProfGuard(
                funcName, Sad::Tools::getGlobalProfiler(), false, "",
                node.position.line);

            // (AR) إنشاء نطاق جديد للدالة / (EN) Create new scope for function
            variableManager_.enterScope(Data::ScopeType::FUNCTION, funcName);

            // ═════════════════════════════════════════════════════════════════
            // (AR) حقن ثوابت القالب المعلَّقة (const-generics) كمتغيرات في نطاق الدالة.
            //      يُملأ pendingTemplateConstBindings_ في visitCallExpr عند كشف
            //      TemplateInstantiation. نستهلكه (move) هنا مرة واحدة فقط.
            // (EN) Inject pending template const-param bindings (const-generics) into
            //      the function scope. Populated in visitCallExpr when a
            //      TemplateInstantiation is detected; consumed once here.
            // ═════════════════════════════════════════════════════════════════
            if (!pendingTemplateConstBindings_.empty())
            {
                auto bindings = std::move(pendingTemplateConstBindings_);
                pendingTemplateConstBindings_.clear();
                for (const auto &[constName, constVal] : bindings)
                {
                    variableManager_.define(constName, constVal);
                }
            }

            // (AR) حقن المتغيرات الملتقطة (للإغلاقات/لامدا) / (EN) Inject captured variables (for closures/lambda)
            if (func->hasCaptures())
            {
                for (const auto &[capName, capVal] : func->getCaptures())
                {
                    variableManager_.define(capName, capVal);
                }
            }

            // (AR) الحصول على المعاملات / (EN) Get parameters
            const auto &params = func->getParameters();

            // (AR) محاولة الحصول على FunctionDecl الأصلي للوصول للـ Parameters
            // (EN) Try to get original FunctionDecl to access Parameters
            auto funcDeclNode = func->getFunctionDecl();
            FunctionDecl *astFuncDecl = nullptr;

            if (funcDeclNode)
            {
                astFuncDecl = dynamic_cast<FunctionDecl *>(funcDeclNode.get());
            }

            // (AR) تقييم القيم الافتراضية للمعاملات المفقودة
            // (EN) Evaluate default values for missing arguments
            while (arguments.size() < params.size())
            {
                size_t i = arguments.size();
                const auto &param = params[i];

                if (!param.hasDefaultValue)
                {
                    variableManager_.exitScope();
                    {
                        Sad::Errors::RenderContext _rc;
                        _rc.placeholders = {{"function", funcName}, {"param", param.name}};
                        Sad::Errors::ErrorManager::getInstance().reportFromCatalog(
                            ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                            Sad::Errors::SourceLocation(getUserFuncSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)), _rc);
                    }
                    lastResult_ = Value();
                    return;
                }

                Data::Value defaultVal;

                if (astFuncDecl && i < astFuncDecl->parameters.size())
                {
                    const auto &astParam = astFuncDecl->parameters[i];

                    if (astParam.defaultValue)
                    {
                        astParam.defaultValue->accept(*this);
                        defaultVal = lastResult_;
                    }
                    else
                    {
                        variableManager_.exitScope();
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                            node.position,
                            {{"param", param.name}, {"function", funcName}});
                    }
                }
                else
                {
                    // (AR) Fallback: تحويل القيمة الافتراضية من string إلى Value
                    // (EN) Fallback: Convert default value from string to Value
                    const std::string &defaultStr = param.defaultValue;

                    if (defaultStr.empty())
                    {
                        defaultVal = Data::Value();
                    }
                    else if (defaultStr == "true" || defaultStr == "صحيح")
                    {
                        defaultVal = Data::Value(true);
                    }
                    else if (defaultStr == "false" || defaultStr == "خطأ")
                    {
                        defaultVal = Data::Value(false);
                    }
                    else if (defaultStr[0] == '"' || defaultStr[0] == '\'')
                    {
                        std::string strVal = (defaultStr.length() >= 2)
                                                 ? defaultStr.substr(1, defaultStr.length() - 2)
                                                 : "";
                        defaultVal = Data::Value(strVal);
                    }
                    else
                    {
                        try
                        {
                            if (defaultStr.find('.') != std::string::npos)
                            {
                                defaultVal = Data::Value(std::stod(defaultStr));
                            }
                            else
                            {
                                // (AR) stoll لا stoi — القيم الافتراضيّة الكبيرة كانت تُقتطع
                                // (EN) stoll not stoi — large default values used to truncate
                                defaultVal = Data::Value(static_cast<int64_t>(std::stoll(defaultStr)));
                            }
                        }
                        catch (...)
                        {
                            defaultVal = Data::Value(defaultStr);
                        }
                    }
                }

                arguments.push_back(defaultVal);
            }

            // (AR) تعريف المعاملات كمتغيرات محلية / (EN) Define parameters as local variables
            for (size_t i = 0; i < params.size(); ++i)
            {
                // ═══════════════════════════════════════════════════════════════
                // (AR) 🔑 «بايت» معاملًا — معبَرٌ رابعٌ كان يكذبُ العقد. الاقتطاعُ
                //      إلى u8 كان مسدودًا عند التصريحِ وإعادةِ الإسنادِ وحدَهما،
                //      فكان `دالة ت(بايت ب)` تستقبل 300 وتطبعها 300 في المحرّكَين
                //      معًا. والاقتطاعُ **قبلَ** `define` لا بعدَه: القيمةُ المخزَّنةُ
                //      هي ما يقرؤه الجسمُ كلُّه، فلو قُنِّعت بعدَ التعريفِ لبقيت
                //      نسخةٌ غيرُ مقنَّعةٍ لحظةً — ولوجدَها أيُّ قارئٍ يسبق التقنيع.
                //      والحارسُ على `getKind()==Integer` مطابقةً لنظيرَيه في
                //      `visitVarDecl` و`visitAssignExpr`: عشريٌّ أو نصٌّ يعبرُ إلى
                //      معاملِ بايتٍ شأنُه شأنُ فحصِ الأنواعِ أدناه، لا شأنُ القناع.
                // (EN) Byte as a parameter — the fourth crossing that broke the
                //      contract. u8 truncation was sealed only at declaration and
                //      reassignment, so a `بايت` parameter received 300 and printed
                //      300 in BOTH engines. Masked BEFORE define, not after: the
                //      stored value is what the whole body reads. The getKind()==
                //      Integer guard mirrors visitVarDecl and visitAssignExpr.
                // ═══════════════════════════════════════════════════════════════
                if (astFuncDecl && i < astFuncDecl->parameters.size() &&
                    astFuncDecl->parameters[i].type == Types::SadTypeKind::Byte &&
                    arguments[i].getKind() == Types::SadTypeKind::Integer)
                {
                    arguments[i] = Data::Value(arguments[i].toInt64() & 0xFF);
                }

                variableManager_.define(params[i].name, arguments[i]);

                // ═══════════════════════════════════════════════════════════════
                // (AR) [طبقة طبيعي64 — الخطوة ٥] تسجيل النوع الساكن المُصرَّح للمعامل
                //      (طبيعي64/بايت…) كي يراه resolveStaticType، فتُطابق المقارنة
                //      اللا-موقَّعة ما يفعله المترجم (نوع المعامل في SIR). بلا هذا
                //      كان المفسّر يقارن معاملات طبيعي64 موقَّعةً بينما المترجم لا-موقَّعة
                //      ⇒ انفراج تكافؤ (رصده أميليا). النوع من عقدة الـAST مباشرةً.
                // (EN) [طبيعي64 layer — Step 5] Register the parameter's declared static
                //      type (طبيعي64/بايت…) so resolveStaticType sees it, making unsigned
                //      comparison mirror the compiler (SIR param type). Without this the
                //      interpreter compared طبيعي64 params signed while the compiler used
                //      unsigned ⇒ parity divergence (found by Amelia). Type from the AST node.
                if (astFuncDecl && i < astFuncDecl->parameters.size() &&
                    astFuncDecl->parameters[i].type != Types::SadTypeKind::Unknown)
                {
                    variableManager_.setDeclaredType(params[i].name, astFuncDecl->parameters[i].type);
                }

                // ═══════════════════════════════════════════════════════════════
                // (AR) SEM045 (RFC عقد الغياب — أ١): «فراغ» لا يعبر إلى معاملٍ
                //      مصنَّف — نفس عقد خانتَي التصريح وإعادة الإسناد، على محور
                //      الصرامة D6. الحارس يحكم على القيمة لا مصدرها، والاختياريّ
                //      `ت؟` مشمول (مجاله {ت، عدم} لا فراغ). عند الالتقاط تُتخطّى
                //      فحوصُ النوع العامّة أدناه (لا تشخيص مزدوج لعبورٍ واحد).
                // (EN) SEM045 (absence-contract RFC, stage أ١): Void must not cross
                //      into a typed parameter slot — same D6 contract as the
                //      declaration/reassignment guards. Skips the generic type
                //      checks below once handled (no double diagnostic).
                // ═══════════════════════════════════════════════════════════════
                bool voidParamHandled = false;
                if (arguments[i].getKind() == Types::SadTypeKind::Void)
                {
                    std::string typedParamName;
                    if (!params[i].typeName.empty())
                    {
                        typedParamName = params[i].typeName;
                    }
                    else if (astFuncDecl && i < astFuncDecl->parameters.size())
                    {
                        const auto &astParam = astFuncDecl->parameters[i];
                        // (AR) المسنَدُ الموحَّد kindIsGuarded (البابُ الواحد) بدل
                        //      سلسلةِ الاستثناءاتِ المنسوخة.
                        // (EN) The unified kindIsGuarded predicate (single door)
                        //      instead of the copied exemption chain.
                        if (astParam.type != Types::SadTypeKind::Unknown &&
                            astParam.sadType &&
                            Sad::Interpreter::Sem045::kindIsGuarded(
                                astParam.sadType->getKind()))
                        {
                            typedParamName = astParam.sadType->arabicName();
                        }
                    }
                    if (!typedParamName.empty())
                    {
                        voidParamHandled = true;
                        Sad::Interpreter::Sem045::reportVoidCrossing(
                            params[i].name, typedParamName, node.position,
                            Sad::NullSafety::strictnessFromOwnershipMode(
                                statementExecutor_.getMemoryPolicy().ownershipMode));
                    }
                }

                // ═══════════════════════════════════════════════════════════════
                // (AR) التحقق من نوع المعامل في وقت التشغيل لأنواع الأصناف
                // (EN) Runtime type checking for class-typed parameters
                // ═══════════════════════════════════════════════════════════════
                if (!voidParamHandled && !params[i].typeName.empty())
                {
                    const std::string &expectedClass = params[i].typeName;
                    const Value &argVal = arguments[i];

                    if (argVal.isObject())
                    {
                        auto objPtr = argVal.toObject();
                        if (objPtr && objPtr->getClassName() != expectedClass)
                        {
                            auto *classManager = Data::ClassManager::getInstance();
                            auto *expectedClassType = classManager->getClass(expectedClass);
                            if (!expectedClassType || !objPtr->isInstanceOf(expectedClassType))
                            {
                                ::Sad::Errors::throwRuntime(
                                    ::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED,
                                    node.position,
                                    {{"actual", objPtr->getClassName()}, {"expected", expectedClass}});
                            }
                        }
                    }
                    else if (argVal.isObjectLike())
                    {
                        std::string actualClass = argVal.getClassName();
                        if (actualClass != expectedClass)
                        {
                            ::Sad::Errors::throwRuntime(
                                ::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED,
                                node.position,
                                {{"actual", actualClass}, {"expected", expectedClass}});
                        }
                    }
                    // (AR) [أ-م٣] قيمة تعداد جبريّ موسومة (خريطة بوسم __تعداد__): نوعها هو
                    //      التعداد المُخزَّن في الوسم، فتُقبَل لمعاملٍ من نوع ذلك التعداد.
                    // (EN) [A-M3] A tagged ADT value (Map with the __تعداد__ tag): its type is
                    //      the enum recorded in the tag, so accept it for a parameter of that enum.
                    else if (argVal.getKind() == Types::SadTypeKind::Map &&
                             argVal.hasKey(AST::TaggedEnumKeys::ENUM) &&
                             argVal[AST::TaggedEnumKeys::ENUM].toString() == expectedClass)
                    {
                        // (AR) مقبول: نوع القيمة الموسومة يطابق نوع المعامل التعداديّ.
                        // (EN) Accepted: tagged value's enum type matches the parameter type.
                    }
                    else
                    {
                        // (AR) لتشخيصٍ أنفع: إن كانت القيمة موسومةً لتعدادٍ آخر، اذكر اسم تعدادها
                        //      الفعليّ بدل «MAP» (تحسين أميليا 🔵٤).
                        // (EN) For a more useful diagnostic: if the value is a tagged value of a
                        //      different enum, report its actual enum name instead of «MAP» (Amelia 🔵4).
                        std::string actualType = argVal.getTypeName();
                        if (argVal.getKind() == Types::SadTypeKind::Map &&
                            argVal.hasKey(AST::TaggedEnumKeys::ENUM))
                        {
                            actualType = argVal[AST::TaggedEnumKeys::ENUM].toString();
                        }
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED,
                            node.position,
                            {{"actual", actualType}, {"expected", expectedClass}});
                    }
                }

                // ═══════════════════════════════════════════════════════════════
                // (AR) فحص توافقية الأنواع البدائية عبر النظام الموحد
                // (EN) Primitive type compatibility check via unified type system
                // ═══════════════════════════════════════════════════════════════
                if (!voidParamHandled && params[i].typeName.empty() && astFuncDecl &&
                    i < astFuncDecl->parameters.size())
                {
                    const auto &astParam = astFuncDecl->parameters[i];
                    // (AR) لا نُحذّر إلا للمعاملات المُصرَّح نوعها صراحةً. المعامل غير المُقيَّد
                    //      (مثل `دالة f(ق)`) نوعه القديم Unknown، لكن sadType يُشتقّ منه عبر
                    //      fromValueType(Unknown)=فراغ، فكان يُطلق تحذيرًا كاذبًا على كل وسيط
                    //      غير-فراغ ويكسر التكافؤ المزدوج (المترجم لا يفحص). [إصلاح X04/P09]
                    // (EN) Only warn for EXPLICITLY-typed params. An unconstrained param (e.g.
                    //      `fn f(x)`) has legacy type=Unknown, but its sadType is derived via
                    //      fromValueType(Unknown)=Void, which spuriously warned on any non-void
                    //      arg and broke dual parity (compiler has no such check). [Fix X04/P09]
                    if (astParam.type != Types::SadTypeKind::Unknown &&
                        astParam.sadType &&
                        astParam.sadType->getKind() != Types::SadTypeKind::Unknown &&
                        astParam.sadType->getKind() != Types::SadTypeKind::Any &&
                        astParam.sadType->getKind() != Types::SadTypeKind::Void)
                    {
                        auto argSadType = Types::SadType::fromValueType(arguments[i].getType());
                        if (argSadType && !argSadType->isAssignableTo(astParam.sadType.get()))
                        {
                            std::cerr << "[تحذير نوع] سطر " << node.position.line
                                      << ": المعامل '" << params[i].name
                                      << "' يتوقع '" << astParam.sadType->arabicName()
                                      << "' لكن حصل على '" << argSadType->arabicName()
                                      << "'" << std::endl;

                            Sad::Errors::SourceLocation loc(
                                "", node.position.line, node.position.column);
                            Sad::Errors::ErrorManager::getInstance().reportWarning(
                                Sad::Errors::ErrorCode::SEM_TYPE_MISMATCH,
                                loc,
                                "معامل '" + params[i].name + "' يتوقع '" +
                                    astParam.sadType->arabicName() + "' لكن حصل على '" +
                                    argSadType->arabicName() + "'",
                                "Parameter '" + params[i].name + "' expects '" +
                                    astParam.sadType->englishName() + "' but got '" +
                                    argSadType->englishName() + "'");
                        }
                    }
                }
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) فحص العقود البرمجية: الشروط المسبقة (يتطلب)
            // (EN) Design by Contract: check preconditions (requires)
            // ═══════════════════════════════════════════════════════════════════
            if (astFuncDecl && !astFuncDecl->preconditions.empty())
            {
                for (size_t pc = 0; pc < astFuncDecl->preconditions.size(); ++pc)
                {
                    auto &precond = astFuncDecl->preconditions[pc];
                    if (precond)
                    {
                        precond->accept(*this);
                        Data::Value condResult = lastResult_;
                        if (condResult.isBoolean() && !condResult.toBool())
                        {
                            variableManager_.exitScope();
                            std::string condStr = precond->toString();
                            ::Sad::Errors::throwRuntime(
                                ::Sad::Errors::ErrorCode::RUN_CONTRACT_PRECOND_FAILED,
                                node.position,
                                {{"index", std::to_string(pc + 1)},
                                 {"function", funcName},
                                 {"expr", condStr}});
                        }
                    }
                }
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) فحص قيود الأنواع (حيث) — تحقق من سمات الوسائط
            // (EN) Where constraints check — verify argument traits
            // ═══════════════════════════════════════════════════════════════════
            if (astFuncDecl && !astFuncDecl->whereConstraints.empty())
            {
                auto *classManager = Data::ClassManager::getInstance();

                for (const auto &constraint : astFuncDecl->whereConstraints)
                {
                    int paramIdx = -1;
                    for (size_t pi = 0; pi < params.size(); ++pi)
                    {
                        if (params[pi].name == constraint.paramName)
                        {
                            paramIdx = static_cast<int>(pi);
                            break;
                        }
                    }

                    size_t startIdx = (paramIdx >= 0) ? static_cast<size_t>(paramIdx) : 0;
                    size_t endIdx = (paramIdx >= 0) ? static_cast<size_t>(paramIdx + 1) : std::min(arguments.size(), params.size());

                    for (const auto &traitName : constraint.traits)
                    {
                        for (size_t ai = startIdx; ai < endIdx && ai < arguments.size(); ++ai)
                        {
                            const auto &argVal = arguments[ai];
                            bool satisfied = true;
                            std::string reason;

                            if (traitName == "\xD8\xB1\xD9\x82\xD9\x85\xD9\x8A" || traitName == "Numeric" || traitName == "numeric")
                            {
                                if (!argVal.isNumeric())
                                {
                                    satisfied = false;
                                    reason = "\xD9\x84\xD9\x8A\xD8\xB3 \xD8\xB1\xD9\x82\xD9\x85\xD8\xA7\xD9\x8B";
                                }
                            }
                            else if (traitName == "\xD9\x86\xD8\xB5\xD9\x8A" || traitName == "Textual" || traitName == "textual")
                            {
                                if (!argVal.isString())
                                {
                                    satisfied = false;
                                    reason = "\xD9\x84\xD9\x8A\xD8\xB3 \xD9\x86\xD8\xB5\xD8\xA7\xD9\x8B";
                                }
                            }
                            else if (traitName == "\xD9\x82\xD8\xA7\xD8\xA8\xD9\x84_\xD9\x84\xD9\x84\xD9\x85\xD9\x82\xD8\xA7\xD8\xB1\xD9\x86\xD8\xA9" || traitName == "Comparable" || traitName == "comparable")
                            {
                                if (!argVal.isNumeric() && !argVal.isString())
                                {
                                    satisfied = false;
                                    reason = "\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x82\xD8\xA7\xD8\xA8\xD9\x84 \xD9\x84\xD9\x84\xD9\x85\xD9\x82\xD8\xA7\xD8\xB1\xD9\x86\xD8\xA9";
                                }
                            }
                            else if (traitName == "\xD9\x85\xD9\x86\xD8\xB7\xD9\x82\xD9\x8A" || traitName == "Boolean" || traitName == "boolean")
                            {
                                if (!argVal.isBoolean())
                                {
                                    satisfied = false;
                                    reason = "\xD9\x84\xD9\x8A\xD8\xB3 \xD9\x85\xD9\x86\xD8\xB7\xD9\x82\xD9\x8A\xD8\xA7\xD9\x8B";
                                }
                            }
                            else if (traitName == "\xD9\x82\xD8\xA7\xD8\xA8\xD9\x84_\xD9\x84\xD9\x84\xD8\xB7\xD8\xA8\xD8\xA7\xD8\xB9\xD8\xA9" || traitName == "Printable" || traitName == "printable")
                            {
                                if (argVal.isVoid())
                                {
                                    satisfied = false;
                                    reason = "\xD9\x84\xD8\xA7\xD8\xB4\xD9\x8A\xD8\xA1";
                                }
                            }
                            else if (classManager && classManager->hasTrait(traitName))
                            {
                                std::string argClassName = argVal.getClassName();
                                if (argClassName.empty())
                                {
                                    satisfied = false;
                                    reason = "\xD9\x84\xD9\x8A\xD8\xB3 \xD9\x83\xD8\xA7\xD8\xA6\xD9\x86\xD8\xA7\xD9\x8B \xD9\x85\xD9\x86 \xD8\xA3\xD9\x8A \xD8\xB5\xD9\x86\xD9\x81";
                                }
                                else if (!classManager->classImplementsTrait(argClassName, traitName))
                                {
                                    satisfied = false;
                                    reason = "\xD8\xA7\xD9\x84\xD8\xB5\xD9\x86\xD9\x81 '" + argClassName +
                                             "' \xD9\x84\xD8\xA7 \xD9\x8A\xD9\x86\xD9\x81\xD8\xB0 \xD8\xA7\xD9\x84\xD8\xB3\xD9\x85\xD8\xA9";
                                }
                            }

                            if (!satisfied)
                            {
                                variableManager_.exitScope();
                                ::Sad::Errors::throwRuntime(
                                    ::Sad::Errors::ErrorCode::RUN_CONTRACT_WHERE_FAILED,
                                    node.position,
                                    {{"function", funcName},
                                     {"constraint", traitName + " on " + params[ai].name + " (" + reason + ")"}});
                            }
                        }
                    }
                }
            }

            auto bodyNode = func->getBody();

            // ═══════════════════════════════════════════════════════════════
            // (AR) تعيين نوع الإرجاع الموحد للدالة الحالية — ويُستعاد نوعُ المُستدعي
            //      عند الخروج (بأيّ مسار: عودةٌ عاديّة أو استثناء). بلا استعادةٍ كان
            //      نوعُ المُنادَى يتسرّب إلى المُستدعي، فيُبلَّغ عن تحذيرٍ كاذب في كلّ
            //      `ارجع` بعد نداءِ دالّةٍ ذاتِ نوعٍ مختلف.
            // (EN) Set the callee's return type, restoring the caller's on every exit
            //      path (normal or throwing). Without the restore the callee's type
            //      leaked into the caller, producing a bogus mismatch warning on any
            //      `return` that followed a call to a differently-typed function.
            // ═══════════════════════════════════════════════════════════════
            struct SadReturnTypeGuard
            {
                StatementExecutor &exec;
                Types::SadTypePtr previous;
                bool engaged;
                ~SadReturnTypeGuard()
                {
                    if (engaged)
                        exec.setCurrentSadReturnType(previous);
                }
            } _sadReturnTypeGuard{statementExecutor_,
                                  statementExecutor_.getCurrentSadReturnType(),
                                  astFuncDecl && astFuncDecl->sadReturnType};

            if (_sadReturnTypeGuard.engaged)
            {
                statementExecutor_.setCurrentSadReturnType(astFuncDecl->sadReturnType);
            }

            auto bodyStmt = dynamic_cast<AST::Statement *>(bodyNode.get());

            if (bodyStmt)
            {
                // (AR) التحقق إذا كانت دالة مولّدة / (EN) Check if generator function
                if (func->isGenerator())
                {
                    bool wasInGenerator = statementExecutor_.isInGenerator();
                    statementExecutor_.setGeneratorMode(true);
                    statementExecutor_.clearGeneratorYieldValues();

                    try
                    {
                        statementExecutor_.executeFunctionBody(*bodyStmt);
                    }
                    catch (...)
                    {
                        statementExecutor_.clearGeneratorYieldValues();
                        statementExecutor_.setGeneratorMode(wasInGenerator);
                        variableManager_.exitScope();
                        throw;
                    }

                    const auto &yields = statementExecutor_.getGeneratorYieldValues();
                    std::vector<Data::Value> arr(yields.begin(), yields.end());
                    lastResult_ = Data::Value(arr);

                    statementExecutor_.clearGeneratorYieldValues();
                    statementExecutor_.setGeneratorMode(wasInGenerator);
                }
                else
                {
                    // ═══════════════════════════════════════════════════════════
                    // (AR) التحقق إذا كانت دالة غير متزامنة (async)
                    // (EN) Check if async function
                    // ═══════════════════════════════════════════════════════════
                    if (func->isAsync())
                    {
                        auto &runtime = AsyncRuntime::getInstance();

                        std::unordered_map<std::string, Data::Value> capturedVars;
                        for (size_t i = 0; i < params.size(); ++i)
                        {
                            capturedVars[params[i].name] = arguments[i];
                        }
                        if (func->hasCaptures())
                        {
                            for (const auto &[capName, capVal] : func->getCaptures())
                            {
                                capturedVars[capName] = capVal;
                            }
                        }

                        Value asyncResult;
                        try
                        {
                            asyncResult = statementExecutor_.executeFunctionBody(*bodyStmt);
                        }
                        catch (const std::exception &e)
                        {
                            Data::Value::MapType futureMap;
                            futureMap["__future__"] = Data::Value(true);
                            futureMap["__resolved__"] = Data::Value(false);
                            futureMap["__rejected__"] = Data::Value(true);
                            futureMap["__error__"] = Data::Value(std::string(e.what()));
                            variableManager_.exitScope();
                            lastResult_ = Data::Value(futureMap);
                            return;
                        }

                        Data::Value::MapType futureMap;
                        futureMap["__future__"] = Data::Value(true);
                        futureMap["__resolved__"] = Data::Value(true);
                        futureMap["__rejected__"] = Data::Value(false);
                        futureMap["__value__"] = asyncResult;

                        variableManager_.exitScope();
                        lastResult_ = Data::Value(futureMap);
                        return;
                    }

                    // (AR) دالة عادية / (EN) Regular function
                    try
                    {
                        lastResult_ = statementExecutor_.executeFunctionBodyWithFuncName(*bodyStmt, funcName);
                    }
                    catch (...)
                    {
                        variableManager_.exitScope();
                        throw;
                    }
                }
            }
            else
            {
                // (AR) هذه دالة Lambda / (EN) This is Lambda
                auto bodyExpr = dynamic_cast<AST::Expression *>(bodyNode.get());

                if (!bodyExpr)
                {
                    variableManager_.exitScope();
                    Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::SEM_EMPTY_FUNCTION_BODY, Sad::Errors::SourceLocation(getUserFuncSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)));
                    lastResult_ = Value();
                    return;
                }

                try
                {
                    bodyExpr->accept(*this);
                }
                catch (...)
                {
                    variableManager_.exitScope();
                    throw;
                }
            }

            // ═══════════════════════════════════════════════════════════════
            // (AR) تحديث المتغيرات الملتقطة بعد تنفيذ الإغلاق
            // (EN) Update captured variables after closure execution
            // ═══════════════════════════════════════════════════════════════
            if (func->hasCaptures())
            {
                std::unordered_map<std::string, Data::Value> updatedCaptures;
                for (const auto &[capName, capVal] : func->getCaptures())
                {
                    const Value *currentVal = variableManager_.tryGet(capName);
                    if (currentVal)
                    {
                        updatedCaptures[capName] = *currentVal;
                    }
                    else
                    {
                        updatedCaptures[capName] = capVal;
                    }
                }
                func->setCaptures(updatedCaptures);
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) فحص العقود البرمجية: الشروط اللاحقة (يضمن)
            // (EN) Design by Contract: check postconditions (ensures)
            // ═══════════════════════════════════════════════════════════════════
            if (astFuncDecl && !astFuncDecl->postconditions.empty())
            {
                Data::Value returnVal = lastResult_;
                variableManager_.define("\xD8\xA7\xD9\x84\xD9\x86\xD8\xAA\xD9\x8A\xD8\xAC\xD8\xA9", returnVal);
                for (size_t pc = 0; pc < astFuncDecl->postconditions.size(); ++pc)
                {
                    auto &postcond = astFuncDecl->postconditions[pc];
                    if (postcond)
                    {
                        postcond->accept(*this);
                        Data::Value condResult = lastResult_;
                        if (condResult.isBoolean() && !condResult.toBool())
                        {
                            std::string condStr = postcond->toString();
                            variableManager_.exitScope();
                            ::Sad::Errors::throwRuntime(
                                ::Sad::Errors::ErrorCode::RUN_CONTRACT_POSTCOND_FAILED,
                                node.position,
                                {{"index", std::to_string(pc + 1)},
                                 {"function", funcName},
                                 {"expr", condStr + " (returned: " + returnVal.toString() + ")"}});
                        }
                    }
                }
                lastResult_ = returnVal;
            }

            // (AR) الخروج من نطاق الدالة / (EN) Exit function scope
            variableManager_.exitScope();
        }

    } // namespace Interpreter
} // namespace Sad
