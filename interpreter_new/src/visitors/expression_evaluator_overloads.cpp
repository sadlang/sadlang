/**
 * @file expression_evaluator_overloads.cpp
 * @brief (AR) تنفيذ تحويلات الكائنات وتوجيهات sizeof/atomic في ExpressionEvaluator
 * @brief (EN) Object conversions and sizeof/atomic directives in ExpressionEvaluator
 */

#include <string>
#include "expression_evaluator.h"
#include "statement_executor.h"
#include "ast_node.h" // (AR) لضمان تعريف ASTNode الكامل عند استدعاء accept / (EN) Ensure full ASTNode definition for accept() calls
#include "statements.h"
#include "declarations.h"
#include "class_nodes.h"
#include "expressions.h"
#include "advanced_expr_nodes.h" // For AwaitExpr
#include "directive_nodes.h"     // For SizeofExpr, AtomicExpr
#include "class_manager.h"
#include "object_instance.h"
#include "error_manager.h"
#include "ownership_manager.h"
#include "exception.h"
#include "async_runtime.h" // (AR) نظام التنفيذ غير المتزامن / (EN) Async runtime system
#include "suggestions.h"   // (AR) نظام الاقتراحات الذكية / (EN) Smart suggestion engine
#include <atomic>
#include <cmath>
#include <climits>
#include <iostream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <set>
#include "sad_type_system.h"

namespace Sad
{
    namespace Interpreter
    {

        using namespace Data;
        using namespace AST;
        using namespace Lexer;

        // (AR) دالة مساعدة للحصول على اسم الملف من مدير الأخطاء
        // (EN) Helper function to get filename from error manager
        static inline std::string getSourceFilename()
        {
            const auto &fn = Sad::Errors::ErrorManager::getInstance().getSourceFilename();
            return fn.empty() ? "<input>" : fn;
        }

        // =========================================================================
        // (AR) تحسين النصوص العربية / (EN) Arabic String Optimization
        // =========================================================================

        Value ExpressionEvaluator::executeOperatorOverload(const Value &left, Data::OperatorOverload &overload, const Value &right, const Lexer::Position &pos)
        {
            // (AR) التحقق من عدد المعاملات / (EN) Verify parameter count
            if (overload.parameters.size() != 1)
            {
                throw RuntimeError(
                    "(AR) العامل '" + overload.operatorSymbol + "' يجب أن يقبل معاملاً واحداً بالضبط. "
                                                                "(EN) Operator '" +
                        overload.operatorSymbol + "' must accept exactly one parameter.",
                    pos);
            }

            // (AR) إنشاء نطاق جديد لتنفيذ العامل / (EN) Create new scope for operator execution
            variableManager_.enterScope(Data::ScopeType::FUNCTION, "operator" + overload.operatorSymbol);

            // (AR) ربط 'هذا' بالكائن الأيسر / (EN) Bind 'this' to left object
            variableManager_.define("هذا", left);

            // (AR) ربط حقول الكائن الأيسر كمتغيرات محلية (محاكاة this.field → field)
            // (EN) Bind left object fields as local variables (simulate this.field → field)
            if (left.isObject())
            {
                // ═══════════════════════════════════════════════════════════════
                // (AR) كائن حقيقي — نربط حقوله من ObjectInstance مباشرة
                // (EN) Real OBJECT — bind fields from ObjectInstance directly
                // ═══════════════════════════════════════════════════════════════
                auto objPtr = left.toObject();
                if (objPtr)
                {
                    for (const auto &[fieldName, fieldValue] : objPtr->fields)
                    {
                        variableManager_.define(fieldName, fieldValue);
                    }
                }
            }
            else if (left.isMap())
            {
                // (AR) MAP قديم مع __class__ / (EN) Legacy MAP with __class__
                auto fields = left.toMap();
                for (const auto &[fieldName, fieldValue] : fields)
                {
                    if (fieldName != "__class__")
                    {
                        variableManager_.define(fieldName, fieldValue);
                    }
                }
            }

            // (AR) ربط المعامل الأيمن / (EN) Bind right operand
            variableManager_.define(overload.parameters[0].name, right);

            // (AR) تنفيذ جسم العامل / (EN) Execute operator body
            Value returnValue;
            try
            {
                overload.body->accept(statementExecutor_);

                if (statementExecutor_.getFlowControl() == FlowControl::RETURN)
                {
                    returnValue = statementExecutor_.getReturnValue();
                    statementExecutor_.resetFlowControl();
                }

                // ═══════════════════════════════════════════════════════════════
                // (AR) كتابة الحقول المحدثة إلى الكائن الأصلي (لدعم العوامل المتغيرة)
                //      مثل: عامل += قد يعدل حقول الكائن
                //
                // (EN) Write back updated fields to original object (for mutating operators)
                //      e.g.: operator += may modify object fields
                // ═══════════════════════════════════════════════════════════════
                if (left.isObject())
                {
                    auto objPtr = left.toObject();
                    if (objPtr)
                    {
                        for (auto &[fieldName, fieldValue] : objPtr->fields)
                        {
                            if (variableManager_.exists(fieldName))
                            {
                                Value updated = variableManager_.get(fieldName);
                                objPtr->fields[fieldName] = updated;
                            }
                        }
                    }
                }
            }
            catch (...)
            {
                variableManager_.exitScope();
                throw;
            }

            variableManager_.exitScope();
            return returnValue;
        }

        // =========================================================================
        // (AR) تنفيذ عامل أحادي محمل زائداً / (EN) Unary Operator Overload Execution
        // =========================================================================

        Value ExpressionEvaluator::executeUnaryOperatorOverload(const Value &operand, Data::OperatorOverload &overload, const Lexer::Position &pos)
        {
            // (AR) إنشاء نطاق جديد لتنفيذ العامل / (EN) Create new scope for operator execution
            variableManager_.enterScope(Data::ScopeType::FUNCTION, "operator" + overload.operatorSymbol);

            // (AR) ربط 'هذا' بالكائن / (EN) Bind 'this' to operand object
            variableManager_.define("هذا", operand);

            // (AR) ربط حقول الكائن كمتغيرات محلية / (EN) Bind object fields as local variables
            if (operand.isObject())
            {
                auto objPtr = operand.toObject();
                if (objPtr)
                {
                    for (const auto &[fieldName, fieldValue] : objPtr->fields)
                    {
                        variableManager_.define(fieldName, fieldValue);
                    }
                }
            }
            else if (operand.isMap())
            {
                auto fields = operand.toMap();
                for (const auto &[fieldName, fieldValue] : fields)
                {
                    if (fieldName != "__class__")
                    {
                        variableManager_.define(fieldName, fieldValue);
                    }
                }
            }

            // (AR) تنفيذ جسم العامل / (EN) Execute operator body
            Value returnValue;
            try
            {
                overload.body->accept(statementExecutor_);

                if (statementExecutor_.getFlowControl() == FlowControl::RETURN)
                {
                    returnValue = statementExecutor_.getReturnValue();
                    statementExecutor_.resetFlowControl();
                }
            }
            catch (...)
            {
                variableManager_.exitScope();
                throw;
            }

            variableManager_.exitScope();
            return returnValue;
        }

        // =========================================================================
        // (AR) تنفيذ عامل الإسناد بالفهرس []= محمل زائداً
        // (EN) Index Assignment Operator []= Overload Execution
        // =========================================================================

        Value ExpressionEvaluator::executeIndexAssignOperatorOverload(const Value &obj, Data::OperatorOverload &overload, const Value &index, const Value &value, const Lexer::Position &pos)
        {
            // (AR) التحقق من عدد المعاملات: يجب أن يكون 2 (فهرس + قيمة)
            // (EN) Verify parameter count: must be 2 (index + value)
            if (overload.parameters.size() != 2)
            {
                throw RuntimeError(
                    "(AR) العامل '[]=' يجب أن يقبل معاملين (فهرس، قيمة). "
                    "(EN) Operator '[]=' must accept two parameters (index, value).",
                    pos);
            }

            variableManager_.enterScope(Data::ScopeType::FUNCTION, "operator[]=");
            variableManager_.define("هذا", obj);

            // (AR) ربط حقول الكائن كمتغيرات محلية
            if (obj.isObject())
            {
                auto objPtr = obj.toObject();
                if (objPtr)
                {
                    for (const auto &[fieldName, fieldValue] : objPtr->fields)
                    {
                        variableManager_.define(fieldName, fieldValue);
                    }
                }
            }
            else if (obj.isMap())
            {
                auto fields = obj.toMap();
                for (const auto &[fieldName, fieldValue] : fields)
                {
                    if (fieldName != "__class__")
                    {
                        variableManager_.define(fieldName, fieldValue);
                    }
                }
            }

            // (AR) ربط الفهرس والقيمة / (EN) Bind index and value
            variableManager_.define(overload.parameters[0].name, index);
            variableManager_.define(overload.parameters[1].name, value);

            Value returnValue;
            try
            {
                overload.body->accept(statementExecutor_);

                if (statementExecutor_.getFlowControl() == FlowControl::RETURN)
                {
                    returnValue = statementExecutor_.getReturnValue();
                    statementExecutor_.resetFlowControl();
                }

                // (AR) كتابة الحقول المحدثة إلى الكائن الأصلي
                // (EN) Write back updated fields to original object
                if (obj.isObject())
                {
                    auto objPtr = obj.toObject();
                    if (objPtr)
                    {
                        for (auto &[fieldName, fieldValue] : objPtr->fields)
                        {
                            if (variableManager_.exists(fieldName))
                            {
                                Value updated = variableManager_.get(fieldName);
                                objPtr->fields[fieldName] = updated;
                            }
                        }
                    }
                }
            }
            catch (...)
            {
                variableManager_.exitScope();
                throw;
            }

            variableManager_.exitScope();
            return returnValue;
        }

        // ======================================================================
        // (AR) عوامل التحويل الضمني — Implicit Conversion Operators
        // ======================================================================

        /**
         * @brief (AR) تحويل كائن إلى نص — يبحث عن عامل نص() ويستدعيه ضمنياً
         * @brief (EN) Convert object to string — looks for نص() operator and calls implicitly
         */
        std::string ExpressionEvaluator::convertObjectToString(const Data::Value &val, const Lexer::Position &pos)
        {
            if (val.isObject() || val.isObjectLike())
            {
                std::string className = val.getClassName();
                if (!className.empty())
                {
                    auto *classMgr = Data::ClassManager::getInstance();
                    if (classMgr)
                    {
                        auto *classType = classMgr->getClass(className);
                        if (classType)
                        {
                            auto *opOverload = classType->findOperator("نص");
                            if (opOverload && opOverload->body)
                            {
                                Data::Value result = executeUnaryOperatorOverload(val, *opOverload, pos);
                                return result.toString();
                            }
                        }
                    }
                }
            }
            return val.toString();
        }

        /**
         * @brief (AR) تحويل كائن إلى منطقي — يبحث عن عامل منطقي() ويستدعيه ضمنياً
         * @brief (EN) Convert object to bool — looks for منطقي() operator and calls implicitly
         */
        bool ExpressionEvaluator::convertObjectToBool(const Data::Value &val, const Lexer::Position &pos)
        {
            if (val.isObject() || val.isObjectLike())
            {
                std::string className = val.getClassName();
                if (!className.empty())
                {
                    auto *classMgr = Data::ClassManager::getInstance();
                    if (classMgr)
                    {
                        auto *classType = classMgr->getClass(className);
                        if (classType)
                        {
                            auto *opOverload = classType->findOperator("منطقي");
                            if (opOverload && opOverload->body)
                            {
                                Data::Value result = executeUnaryOperatorOverload(val, *opOverload, pos);
                                return result.toBool();
                            }
                        }
                    }
                }
            }
            return val.toBool();
        }

        /**
         * @brief (AR) تحويل كائن إلى رقم — يبحث عن عامل رقم() ويستدعيه ضمنياً
         * @brief (EN) Convert object to number — looks for رقم() operator and calls implicitly
         */
        Data::Value ExpressionEvaluator::convertObjectToNumber(const Data::Value &val, const Lexer::Position &pos)
        {
            if (val.isObject() || val.isObjectLike())
            {
                std::string className = val.getClassName();
                if (!className.empty())
                {
                    auto *classMgr = Data::ClassManager::getInstance();
                    if (classMgr)
                    {
                        auto *classType = classMgr->getClass(className);
                        if (classType)
                        {
                            auto *opOverload = classType->findOperator("رقم");
                            if (opOverload && opOverload->body)
                            {
                                return executeUnaryOperatorOverload(val, *opOverload, pos);
                            }
                        }
                    }
                }
            }
            return val;
        }

        // ======================================================================
        // (AR) زوار التوجيهات التعبيرية @ — Expression Directive Visitors
        // ======================================================================

        /**
         * @brief (AR) @حجم(نوع) — يُرجع حجم النوع بالبايتات
         * @brief (EN) @sizeof(type) — returns size of type in bytes
         */
        void ExpressionEvaluator::visitSizeofExpr(AST::SizeofExpr &expr)
        {
            // (AR) خريطة أحجام الأنواع الأساسية
            // (EN) Basic type size map
            static const std::unordered_map<std::string, int64_t> typeSizes = {
                {"رقم", 8},     // int64_t = 8 bytes
                {"عشري", 8},    // double = 8 bytes
                {"نص", 32},     // std::string (approx)
                {"منطقي", 1},   // bool = 1 byte
                {"فراغ", 0},    // void = 0
                {"عدم", 0},     // null = 0
                {"مصفوفة", 24}, // vector (approx)
                {"خريطة", 48},  // map (approx)
                {"أي", 72},     // Value variant (approx)
                // Sized integer types
                {"u8", 1},
                {"i8", 1},
                {"u16", 2},
                {"i16", 2},
                {"u32", 4},
                {"i32", 4},
                {"u64", 8},
                {"i64", 8},
                {"usize", 8},
                {"isize", 8},
                {"ptr", 8},
            };

            auto it = typeSizes.find(expr.typeName);
            if (it != typeSizes.end())
            {
                lastResult_ = Data::Value(it->second);
            }
            else
            {
                // (AR) نوع غير معروف — نرجع 0
                // (EN) Unknown type — return 0
                lastResult_ = Data::Value(static_cast<int64_t>(0));
            }
        }

        /**
         * @brief (AR) @ذري(عملية, ...) — عمليات ذرية
         * @brief (EN) @atomic(op, ...) — atomic operations
         *
         * (AR) في المفسر: نحاكي العمليات الذرية بدون قفل حقيقي
         *      لأن المفسر أحادي الخيط. القيمة الحقيقية تكون في المترجم.
         * (EN) In interpreter: simulate atomic ops without real locking
         *      because interpreter is single-threaded. Real value is in compiler.
         */
        void ExpressionEvaluator::visitAtomicExpr(AST::AtomicExpr &expr)
        {
            const std::string &op = expr.operation;

            if (op == "تحميل" || op == "load")
            {
                // @ذري(تحميل, متغير) — قراءة ذرية
                if (!expr.operands.empty())
                {
                    expr.operands[0]->accept(*this);
                    // lastResult_ already set
                }
                else
                {
                    lastResult_ = Data::Value(static_cast<int64_t>(0));
                }
            }
            else if (op == "تخزين" || op == "store")
            {
                // @ذري(تخزين, متغير, قيمة) — كتابة ذرية
                if (expr.operands.size() >= 2)
                {
                    // (AR) نحسب القيمة
                    expr.operands[1]->accept(*this);
                    Data::Value val = lastResult_;

                    // (AR) نحصل على اسم المتغير ونعيّنه
                    if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(expr.operands[0].get()))
                    {
                        variableManager_.assign(varExpr->name, std::move(val));
                    }
                    lastResult_ = Data::Value(static_cast<int64_t>(0));
                }
            }
            else if (op == "إضافة" || op == "add")
            {
                // @ذري(إضافة, متغير, قيمة) — إضافة ذرية
                if (expr.operands.size() >= 2)
                {
                    expr.operands[0]->accept(*this);
                    Data::Value current = lastResult_;
                    expr.operands[1]->accept(*this);
                    Data::Value addend = lastResult_;

                    if (current.isInteger() && addend.isInteger())
                    {
                        int64_t newVal = current.toInt64() + addend.toInt64();
                        lastResult_ = Data::Value(newVal);

                        // (AR) تحديث المتغير
                        if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(expr.operands[0].get()))
                        {
                            variableManager_.assign(varExpr->name, Data::Value(newVal));
                        }
                    }
                }
            }
            else if (op == "طرح" || op == "sub")
            {
                // @ذري(طرح, متغير, قيمة) — طرح ذري
                if (expr.operands.size() >= 2)
                {
                    expr.operands[0]->accept(*this);
                    Data::Value current = lastResult_;
                    expr.operands[1]->accept(*this);
                    Data::Value subtrahend = lastResult_;

                    if (current.isInteger() && subtrahend.isInteger())
                    {
                        int64_t newVal = current.toInt64() - subtrahend.toInt64();
                        lastResult_ = Data::Value(newVal);

                        if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(expr.operands[0].get()))
                        {
                            variableManager_.assign(varExpr->name, Data::Value(newVal));
                        }
                    }
                }
            }
            else if (op == "مقارنة_وتبديل" || op == "compare_and_swap" || op == "cas")
            {
                // @ذري(مقارنة_وتبديل, متغير, متوقع, جديد)
                if (expr.operands.size() >= 3)
                {
                    expr.operands[0]->accept(*this);
                    Data::Value current = lastResult_;
                    expr.operands[1]->accept(*this);
                    Data::Value expected = lastResult_;
                    expr.operands[2]->accept(*this);
                    Data::Value newVal = lastResult_;

                    // (AR) مقارنة وتبديل: إذا القيمة الحالية == المتوقعة، نضع الجديدة
                    if (current.isInteger() && expected.isInteger() && newVal.isInteger())
                    {
                        if (current.toInt64() == expected.toInt64())
                        {
                            if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(expr.operands[0].get()))
                            {
                                variableManager_.assign(varExpr->name, Data::Value(newVal.toInt64()));
                            }
                            lastResult_ = Data::Value(true); // (AR) نجح / (EN) succeeded
                        }
                        else
                        {
                            lastResult_ = Data::Value(false); // (AR) فشل / (EN) failed
                        }
                    }
                }
            }
            else
            {
                // (AR) عملية ذرية غير معروفة
                lastResult_ = Data::Value(static_cast<int64_t>(0));
            }
        }

    } // namespace Interpreter
} // namespace Sad

