/**
 * @file statement_executor_control_exceptions.cpp
 * @brief (AR) تنفيذ جمل try/raise/match في StatementExecutor
 * @brief (EN) try/raise/match statement execution for StatementExecutor
 *
 * (AR) ملف مُستخرج لتقليل حجم statement_executor_control.cpp وفق CW-05
 */

#include "statement_executor.h"
#include "declarations.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
#include "error_manager.h"
#include "exception.h"
#include "object_instance.h"
#include "debug_server.h"
#include "class_manager.h"
#include "channel.h"
#include "sad_type_system.h"
#include "../../tools/profiler/include/profiler_hooks.h" // (AR) خطافات مصحح الأداء / (EN) Profiler hooks
#include <iostream>
#include <sstream>
#include <map>

namespace Sad
{
    namespace Interpreter
    {

        // (AR) دالة مساعدة للحصول على اسم الملف من مدير الأخطاء
        // (EN) Helper function to get filename from error manager
        static inline std::string getSourceFilename()
        {
            const auto &fn = Sad::Errors::ErrorManager::getInstance().getSourceFilename();
            return fn.empty() ? "<input>" : fn;
        }

        // ═══════════════════════════════════════════════════════════════
        // (AR) خطاف التصحيح — يُستدعى قبل تنفيذ كل جملة
        // (EN) Debug hook — called before each statement execution
        // ═══════════════════════════════════════════════════════════════
        static inline void debugHook(const Sad::Lexer::Position &pos)
        {
            auto *dbg = Sad::Debug::DebugServer::getInstance();
            if (dbg && dbg->isConnected())
            {
                dbg->onBeforeStatement(
                    getSourceFilename(),
                    static_cast<int>(pos.line),
                    static_cast<int>(pos.column));
                if (dbg->shouldStop())
                {
                    throw std::runtime_error("تم فصل المصحح / Debugger disconnected");
                }
            }
        }

        // =========================================================================
        // (AR) مساعد: مطابقة نوع الاستثناء مع دعم الوراثة
        // (EN) Helper: Match exception type with inheritance support
        // =========================================================================
        static bool isExceptionTypeOrSubclass(const std::string &exceptionType, const std::string &catchTypeName)
        {
            // (AR) مطابقة مباشرة / (EN) Direct match
            if (exceptionType == catchTypeName)
                return true;

            // (AR) بحث في سلسلة الوراثة / (EN) Walk inheritance chain
            auto *classMgr = Data::ClassManager::getInstance();
            auto *cls = classMgr->getClass(exceptionType);
            while (cls && cls->getBaseClass())
            {
                cls = cls->getBaseClass();
                if (cls->name == catchTypeName)
                    return true;
            }
            return false;
        }

        void StatementExecutor::visitTryStmt(AST::TryStmt &node)
        {
            // (AR) حارس RAII لضمان تنفيذ كتلة finally دائماً — حتى عند إعادة رفع الاستثناء
            // (EN) RAII guard to guarantee finally block always executes — even on re-throw
            std::exception_ptr pendingException = nullptr;

            try
            {
                // (AR) تنفيذ كتلة المحاولة / (EN) Execute try block
                node.tryBlock->accept(*this);
            }
            catch (const SadException &e)
            {
                // (AR) البحث عن بند التقاط مناسب مع مطابقة نوع الاستثناء
                // (EN) Find matching catch clause with exception type matching
                bool caught = false;
                const std::string exType = e.getType();

                for (auto &catchClause : node.catchClauses)
                {
                    // (AR) مطابقة نوع الاستثناء — UNKNOWN يطابق الكل (catch-all)
                    // (EN) Match exception type — UNKNOWN matches everything (catch-all)
                    bool typeMatches = false;

                    if (catchClause.exceptionType == Data::DataType::UNKNOWN)
                    {
                        // (AR) catch-all: يلتقط أي استثناء
                        typeMatches = true;
                    }
                    else if (catchClause.exceptionType == Data::DataType::ERROR)
                    {
                        // (AR) مطابقة نوع ERROR فقط — نقارن مع نوع الاستثناء الفعلي
                        // (EN) Match ERROR type — compare against actual exception type
                        typeMatches = (exType == "RuntimeError" || exType == "Error" ||
                                       exType == "خطأ" || exType == "خطأ_تشغيل" || exType.empty());
                    }
                    else if (catchClause.exceptionType == Data::DataType::OBJECT)
                    {
                        // (AR) مطابقة نوع كائن مخصص مع دعم الوراثة
                        // (EN) Match custom object type with inheritance support
                        if (!catchClause.exceptionTypeName.empty())
                        {
                            typeMatches = isExceptionTypeOrSubclass(exType, catchClause.exceptionTypeName);
                        }
                        else
                        {
                            typeMatches = (!catchClause.exceptionVar.empty() &&
                                           (catchClause.exceptionVar == exType ||
                                            exType.empty()));
                        }
                    }

                    if (!typeMatches)
                    {
                        continue;
                    }

                    variableManager_.enterScope(Data::ScopeType::BLOCK);

                    if (!catchClause.exceptionVar.empty())
                    {
                        // (AR) إذا كان الاستثناء يحمل كائناً أصلياً — نربطه ككائن
                        // (EN) If exception carries original object — bind as object
                        if (e.hasThrownValue())
                        {
                            variableManager_.define(catchClause.exceptionVar, e.getThrownValue());
                        }
                        else
                        {
                            variableManager_.define(catchClause.exceptionVar,
                                                    Data::Value(e.getMessage()));
                        }
                    }

                    catchClause.body->accept(*this);
                    variableManager_.exitScope();

                    caught = true;
                    break;
                }

                if (!caught)
                {
                    pendingException = std::current_exception();
                }
            }
            catch (const ExecutionError &e)
            {
                bool caught = false;

                for (auto &catchClause : node.catchClauses)
                {
                    // (AR) مطابقة نوع الاستثناء — UNKNOWN يلتقط الكل
                    // (EN) Type matching — UNKNOWN catches all
                    if (catchClause.exceptionType != Data::DataType::UNKNOWN &&
                        catchClause.exceptionType != Data::DataType::ERROR)
                    {
                        continue;
                    }

                    variableManager_.enterScope(Data::ScopeType::BLOCK);

                    if (!catchClause.exceptionVar.empty())
                    {
                        variableManager_.define(catchClause.exceptionVar,
                                                Data::Value(std::string(e.what())));
                    }

                    catchClause.body->accept(*this);
                    variableManager_.exitScope();

                    caught = true;
                    break;
                }

                if (!caught)
                {
                    pendingException = std::current_exception();
                }
            }
            catch (const std::exception &e)
            {
                bool caught = false;

                for (auto &catchClause : node.catchClauses)
                {
                    // (AR) مطابقة نوع الاستثناء — UNKNOWN يلتقط الكل
                    // (EN) Type matching — UNKNOWN catches all
                    if (catchClause.exceptionType != Data::DataType::UNKNOWN &&
                        catchClause.exceptionType != Data::DataType::ERROR)
                    {
                        continue;
                    }

                    variableManager_.enterScope(Data::ScopeType::BLOCK);

                    if (!catchClause.exceptionVar.empty())
                    {
                        variableManager_.define(catchClause.exceptionVar,
                                                Data::Value(std::string(e.what())));
                    }

                    catchClause.body->accept(*this);
                    variableManager_.exitScope();

                    caught = true;
                    break;
                }

                if (!caught)
                {
                    pendingException = std::current_exception();
                }
            }
            catch (...)
            {
                bool caught = false;

                for (auto &catchClause : node.catchClauses)
                {
                    // (AR) للاستثناءات غير المعروفة، فقط catch-all (UNKNOWN) يلتقطها
                    // (EN) For unknown exceptions, only catch-all (UNKNOWN) matches
                    if (catchClause.exceptionType != Data::DataType::UNKNOWN)
                    {
                        continue;
                    }

                    variableManager_.enterScope(Data::ScopeType::BLOCK);

                    if (!catchClause.exceptionVar.empty())
                    {
                        variableManager_.define(catchClause.exceptionVar,
                                                Data::Value("Unknown exception"));
                    }

                    catchClause.body->accept(*this);
                    variableManager_.exitScope();

                    caught = true;
                    break;
                }

                if (!caught)
                {
                    pendingException = std::current_exception();
                }
            }

            // (AR) تنفيذ كتلة finally دائماً — حتى لو لم يُلتقط الاستثناء
            // (EN) Always execute finally block — even if exception was not caught
            if (node.finallyBlock)
            {
                // (AR) حفظ حالة التحكم بالتدفق قبل finally
                // (EN) Save flow control state before finally
                auto savedFlowBeforeFinally = flowControl_;

                node.finallyBlock->accept(*this);

                // (AR) إذا غيّرت finally الحالة (مثلاً ارجع)، لها الأولوية على الاستثناء
                // (EN) If finally changed flow (e.g. return), it takes precedence over exception
                if (flowControl_ != savedFlowBeforeFinally && flowControl_ == FlowControl::RETURN)
                {
                    pendingException = nullptr; // (AR) إلغاء الاستثناء لصالح الإرجاع
                }
            }

            // (AR) إعادة رفع الاستثناء غير المُلتقط بعد تنفيذ finally
            // (EN) Re-throw uncaught exception after finally executed
            if (pendingException)
            {
                std::rethrow_exception(pendingException);
            }
        }

        void StatementExecutor::visitRaiseStmt(AST::RaiseStmt &node)
        {
            // (AR) تقييم تعبير الاستثناء / (EN) Evaluate exception expression
            Data::Value exceptionValue = evaluateExpression(*node.exception);

            // (AR) إذا كان الاستثناء كائناً (صنف مخصص) — نرمي مع اسم الصنف كنوع
            // (EN) If exception is an object (custom class) — throw with class name as type
            if (exceptionValue.isObject())
            {
                auto obj = exceptionValue.toObject();
                std::string typeName = obj ? obj->getClassName() : "UnknownObject";
                // (AR) محاولة استخراج رسالة من حقل 'رسالة' أو 'message'
                // (EN) Try to extract message from 'رسالة' or 'message' field
                std::string message = typeName;
                if (obj)
                {
                    if (obj->hasField("رسالة"))
                    {
                        auto *field = obj->getField("رسالة");
                        if (field)
                            message = field->toString();
                    }
                    else if (obj->hasField("message"))
                    {
                        auto *field = obj->getField("message");
                        if (field)
                            message = field->toString();
                    }
                    else if (obj->hasField("الرسالة"))
                    {
                        auto *field = obj->getField("الرسالة");
                        if (field)
                            message = field->toString();
                    }
                }
                // (AR) نُنشئ استثناء يحمل الكائن الأصلي — حتى يمكن ربطه في catch كـ كائن
                // (EN) Create exception carrying the original object — so catch can bind it as object
                SadException exc(message, typeName, node.position);
                exc.setThrownValue(exceptionValue);
                throw exc;
            }

            // (AR) رفع الاستثناء كخطأ تشغيل عادي — مع حمل القيمة الأصلية
            // (EN) Raise as regular runtime error — carrying the original thrown value
            RuntimeError exc(exceptionValue.toString(), node.position);
            exc.setThrownValue(exceptionValue);
            throw exc;
        }

        // =========================================================================
        // (AR) Pattern Matching Implementation / (EN) تنفيذ مطابقة الأنماط
        // =========================================================================

        void StatementExecutor::visitMatchStmt(AST::MatchStmt &node)
        {
            // (AR) تقييم القيمة المراد مطابقتها / (EN) Evaluate value to match
            Data::Value testValue = evaluateExpression(*node.value);

            // (AR) المرور على كل حالة بالترتيب / (EN) Try each case in order
            for (auto &caseClause : node.cases)
            {
                // (AR) خريطة لربط المتغيرات من النمط / (EN) Map to bind variables from pattern
                std::map<std::string, Data::Value> bindings;

                // ═══════════════════════════════════════════════════════════════
                // (AR) معالجة خاصة لأنماط التعدادات البسيطة (غير الجبرية)
                //      المشكلة: EnumVariantPattern::matches() يتوقع MAP مع __تعداد__ و__عضو__
                //      لكن قيم التعدادات البسيطة تُخزّن كـ INTEGER (مثلاً 0، 1، 2)
                //      الحل: إذا كان النمط EnumVariantPattern والقيمة ليست MAP،
                //            نبحث عن القيمة المؤهلة (مثل "تعداد.عضو") في المتغيرات ونقارن مباشرة
                // (EN) Special handling for simple (non-ADT) enum patterns
                //      Problem: EnumVariantPattern::matches() expects MAP with __تعداد__ and __عضو__
                //      But simple enum values are stored as INTEGER (e.g., 0, 1, 2)
                //      Fix: If pattern is EnumVariantPattern and value is not MAP,
                //           look up the qualified name (e.g., "Enum.Member") and compare directly
                // ═══════════════════════════════════════════════════════════════
                bool matched = false;
                auto *enumVarPat = dynamic_cast<AST::EnumVariantPattern *>(caseClause.pattern.get());
                if (enumVarPat && testValue.getKind() != Types::SadTypeKind::Map)
                {
                    // (AR) تعداد بسيط — نبحث عن قيمة "تعداد.عضو" ونقارنها مباشرة
                    // (EN) Simple enum — look up "Enum.Member" value and compare directly
                    std::string qualifiedName = enumVarPat->enumName + "." + enumVarPat->variantName;
                    const Data::Value *enumMemberVal = variableManager_.tryGet(qualifiedName);
                    if (enumMemberVal)
                    {
                        matched = (testValue == *enumMemberVal).toBool();
                    }
                }
                else
                {
                    // (AR) مطابقة عادية — تعدادات جبرية (ADT) وجميع الأنماط الأخرى
                    // (EN) Normal matching — ADT enums and all other patterns
                    matched = caseClause.pattern->matches(testValue, bindings);
                }

                // (AR) محاولة مطابقة النمط / (EN) Try to match pattern
                if (matched)
                {
                    // ═══════════════════════════════════════════════════════════════
                    // (AR) إصلاح BUG-GUARD-SCOPE: ندفع النطاق ونعرّف ربطات النمط
                    //      قبل تقييم الحارس، حتى يتمكن الحارس من رؤية المتغيرات
                    //      المربوطة (مثل: عندما ق إذا ق < 5)
                    // (EN) Fix BUG-GUARD-SCOPE: Push scope and define pattern bindings
                    //      BEFORE evaluating the guard, so the guard can see bound
                    //      variables (e.g.: when x if x < 5)
                    // ═══════════════════════════════════════════════════════════════
                    variableManager_.enterScope(Data::ScopeType::BLOCK);

                    // (AR) ربط جميع المتغيرات من النمط / (EN) Bind all variables from pattern
                    for (const auto &[name, value] : bindings)
                    {
                        variableManager_.define(name, value);
                    }

                    // (AR) النمط تطابق - الآن نتحقق من الحارس (guard) إن وجد
                    // (EN) Pattern matched - now check guard if present
                    if (caseClause.guard)
                    {
                        Data::Value guardResult = evaluateExpression(*caseClause.guard);
                        if (!guardResult.toBool())
                        {
                            // (AR) الحارس فشل — نزيل النطاق ونجرّب الحالة التالية
                            // (EN) Guard failed — pop scope and try next case
                            variableManager_.exitScope();
                            continue;
                        }
                    }

                    // (AR) النمط والحارس نجحا - ننفذ الجسم / (EN) Pattern and guard succeeded - execute body
                    // (AR) تنفيذ جسم الحالة / (EN) Execute case body
                    for (auto &stmt : caseClause.body)
                    {
                        stmt->accept(*this);

                        // (AR) إذا حدث تحكم في التدفق (return, break, continue)، نتوقف
                        // (EN) If flow control occurred (return, break, continue), stop
                        if (flowControl_ != FlowControl::NONE)
                        {
                            variableManager_.exitScope();
                            return;
                        }
                    }

                    // (AR) إزالة النطاق / (EN) Pop scope
                    variableManager_.exitScope();

                    // (AR) وجدنا تطابق، ننهي / (EN) Found match, exit
                    return;
                }
            }

            // (AR) لم يتطابق أي نمط - بدون حالة افتراضية نتجاهل فقط
            // (EN) No pattern matched - without default case, silently do nothing
            // NOTE: If a default arm ("_" or افتراضي) is wanted, it should be defined as a
            // WildcardPattern in the match cases. No error thrown for exhaustiveness.
        }

    } // namespace Interpreter
} // namespace Sad
