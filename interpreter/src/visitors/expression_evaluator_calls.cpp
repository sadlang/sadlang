/**
 * @file expression_evaluator.cpp
 * @brief (AR) تنفيذ مُقيِّم التعابير
 * @brief (EN) Expression Evaluator Implementation
 *
 * @author S Language Development Team
 * @date November 21, 2025
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
#include "class_manager.h"
#include "object_instance.h"
#include "error_manager.h"
#include "ownership_manager.h"
#include "exception.h"
#include "async_runtime.h"                               // (AR) نظام التنفيذ غير المتزامن / (EN) Async runtime system
#include "suggestions.h"                                 // (AR) نظام الاقتراحات الذكية / (EN) Smart suggestion engine
#include "profiler_hooks.h" // (AR) خطافات مصحح الأداء / (EN) Profiler hooks
#include <atomic>
#include <cmath>
#include <climits>
#include <iostream>
#include "sad_type_system.h"
#include <map>

// (AR) دالة مساعدة للحصول على اقتراح وحدة الاستيراد — مُعرَّفة في builtin_registry.cpp
// (EN) Helper to get import module suggestion — defined in builtin_registry.cpp
namespace Sad
{
    namespace Interpreter
    {
        std::string suggestModuleForUnimportedFunction(const std::string &funcName);
    }
}
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <set>

#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int
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
        // (AR) تقييم التعابير الأحادية / (EN) Unary Expression Evaluation
        // =========================================================================

        void ExpressionEvaluator::visitUnaryExpr(UnaryExpr &node)
        {
            // تقييم المعامل / Evaluate operand
            node.operand->accept(*this);
            Value operand = lastResult_;

            // ═══════════════════════════════════════════════════════════════════
            // (AR) فحص تحميل العامل الأحادي الزائد على الكائنات
            // (EN) Check unary operator overloading on objects
            // ═══════════════════════════════════════════════════════════════════
            if (operand.isObject() || operand.isObjectLike())
            {
                std::string opSymbol;
                switch (node.op)
                {
                case TokenType::OP_MINUS:
                    opSymbol = "u-";
                    break; // (AR) سالب أحادي / (EN) unary minus
                case TokenType::OP_NOT:
                    opSymbol = "!";
                    break;
                case TokenType::OP_PLUS:
                    opSymbol = "u+";
                    break; // (AR) موجب أحادي / (EN) unary plus
                default:
                    break;
                }
                if (!opSymbol.empty())
                {
                    std::string className = operand.getClassName();
                    if (!className.empty())
                    {
                        auto *classManager = Data::ClassManager::getInstance();
                        Data::ClassType *classType = classManager->getClass(className);
                        if (classType)
                        {
                            Data::OperatorOverload *opOverload = classType->findOperator(opSymbol);
                            if (opOverload && opOverload->body)
                            {
                                lastResult_ = executeUnaryOperatorOverload(operand, *opOverload, node.position);
                                return;
                            }
                            // (AR) بديل: جرب "-" العادي كأحادي إذا لم يُعرف "u-"
                            // (EN) Fallback: try plain "-" as unary if "u-" not defined
                            if (opSymbol == "u-")
                            {
                                opOverload = classType->findOperator("-");
                                if (opOverload && opOverload->body && opOverload->parameters.empty())
                                {
                                    lastResult_ = executeUnaryOperatorOverload(operand, *opOverload, node.position);
                                    return;
                                }
                            }
                        }
                    }
                }
            }

            switch (node.op)
            {
            case TokenType::OP_MINUS: // -x
                if (operand.isInteger())
                {
                    lastResult_ = Value(-operand.toInt());
                }
                else if (operand.isDouble())
                {
                    lastResult_ = Value(-operand.toDouble());
                }
                else
                {
                    Sad::Errors::ErrorManager::getInstance().reportError(
                        Sad::Errors::ErrorCode::RUN_INVALID_CAST,
                        Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                        "السالب يتطلب قيمة رقمية",
                        "Negation requires numeric value");
                    lastResult_ = Value(0);
                }
                break;

            case TokenType::OP_NOT: // !flag
                // (AR) يدعم عامل منطقي() الضمني للكائنات
                lastResult_ = Value(!convertObjectToBool(operand, node.position));
                break;

            case TokenType::OP_PLUS: // +x (no-op)
                if (!operand.isNumeric())
                {
                    Sad::Errors::ErrorManager::getInstance().reportError(
                        Sad::Errors::ErrorCode::RUN_INVALID_CAST,
                        Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                        "الموجب يتطلب قيمة رقمية",
                        "Positive requires numeric value");
                    lastResult_ = Value(0);
                    break;
                }
                lastResult_ = operand;
                break;

            case TokenType::OP_INCREMENT:
            { // ++x
                // (AR) الزيادة: ++x يزيد القيمة بـ 1 ويُرجع القيمة الجديدة
                // (EN) Increment: ++x adds 1 and returns new value
                Value newVal;
                if (operand.isInteger())
                {
                    newVal = Value(operand.toInt() + 1);
                }
                else if (operand.isDouble())
                {
                    newVal = Value(operand.toDouble() + 1.0);
                }
                else
                {
                    throw Interpreter::SadException(
                        "(AR) العامل ++ يتطلب قيمة رقمية، ولكن القيمة من نوع '" + operand.getTypeName() + "'. "
                                                                                                          "(EN) ++ requires numeric value, but got type '" +
                            operand.getTypeName() + "'.",
                        "TypeError", node.position);
                }
                // (AR) تحديث المتغير إذا كان المعامل متغيراً
                // (EN) Update variable if operand is variable
                if (auto *varExpr = dynamic_cast<VariableExpr *>(node.operand.get()))
                {
                    variableManager_.assign(varExpr->name, newVal);
                }
                lastResult_ = newVal;
                break;
            }

            case TokenType::OP_DECREMENT:
            { // --x
                // (AR) النقصان: --x يُنقص القيمة بـ 1 ويُرجع القيمة الجديدة
                // (EN) Decrement: --x subtracts 1 and returns new value
                Value newVal;
                if (operand.isInteger())
                {
                    newVal = Value(operand.toInt() - 1);
                }
                else if (operand.isDouble())
                {
                    newVal = Value(operand.toDouble() - 1.0);
                }
                else
                {
                    throw Interpreter::SadException(
                        "(AR) العامل -- يتطلب قيمة رقمية، ولكن القيمة من نوع '" + operand.getTypeName() + "'. "
                                                                                                          "(EN) -- requires numeric value, but got type '" +
                            operand.getTypeName() + "'.",
                        "TypeError", node.position);
                }
                // (AR) تحديث المتغير إذا كان المعامل متغيراً
                // (EN) Update variable if operand is variable
                if (auto *varExpr = dynamic_cast<VariableExpr *>(node.operand.get()))
                {
                    variableManager_.assign(varExpr->name, newVal);
                }
                lastResult_ = newVal;
                break;
            }

            case TokenType::OP_BITWISE_NOT:
            { // ~x
                // (AR) القلب البتّي: ~x يقلب جميع البتات
                // (EN) Bitwise NOT: ~x flips all bits
                if (operand.isInteger())
                {
                    lastResult_ = Value(~operand.toInt());
                }
                else
                {
                    throw Interpreter::SadException(
                        "(AR) ~ يتطلب قيمة صحيحة / (EN) ~ requires integer value",
                        "TypeError", node.position);
                }
                break;
            }

            default:
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                    Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                    "عملية أحادية غير مدعومة",
                    "Unsupported unary operation");
                lastResult_ = Value(0);
            }
        }

        // =========================================================================
        // (AR) تقييم التعبير الثلاثي الشرطي / (EN) Ternary Expression Evaluation
        // =========================================================================

        /**
         * @brief (AR) يُقيّم التعبير الثلاثي الشرطي: شرط ? صحيح : خطأ
         *        (EN) Evaluates ternary conditional expression: condition ? true : false
         *
         * Evaluates the condition, then returns either true_expression or false_expression
         * based on the condition result.
         *
         * يُقيّم الشرط، ثم يُرجع إما تعبير_صحيح أو تعبير_خطأ بناءً على نتيجة الشرط.
         *
         * @example
         * x > 0 ? "positive" : "negative"
         * age >= 18 ? "adult" : "minor"
         * العمر >= 18 ؟ "بالغ" : "قاصر"
         */
        void ExpressionEvaluator::visitTernaryExpr(TernaryExpr &node)
        {
            // Evaluate condition / تقييم الشرط
            node.condition->accept(*this);
            Value condition = lastResult_;

            // Based on condition, evaluate either true or false branch
            // بناءً على الشرط، قيّم إما الفرع الصحيح أو الخاطئ
            if (condition.toBool())
            {
                // Condition is true, evaluate true branch
                // الشرط صحيح، قيّم الفرع الصحيح
                node.trueExpr->accept(*this);
            }
            else
            {
                // Condition is false, evaluate false branch
                // الشرط خاطئ، قيّم الفرع الخاطئ
                node.falseExpr->accept(*this);
            }

            // lastResult_ already contains the result from the evaluated branch
            // lastResult_ يحتوي بالفعل على النتيجة من الفرع المُقيّم
        }

        // =========================================================================
        // (AR) تقييم المصفوفات / (EN) Array Evaluation
        // =========================================================================

        void ExpressionEvaluator::visitArrayExpr(ArrayExpr &node)
        {
            Value::ArrayType arr;

            // تقييم كل عنصر / Evaluate each element
            for (auto &elem : node.elements)
            {
                elem->accept(*this);
                arr.push_back(lastResult_);
            }

            lastResult_ = Value(arr);
        }

        // =========================================================================
        // (AR) تقييم القواميس / (EN) Map Evaluation
        // =========================================================================

        void ExpressionEvaluator::visitMapExpr(MapExpr &node)
        {
            Value::MapType map;

            // تقييم كل زوج مفتاح-قيمة / Evaluate each key-value pair
            for (auto &pair : node.pairs)
            {
                // (AR) التحقق من spread / (EN) Check for spread
                if (pair.isSpread())
                {
                    // (AR) spread: دمج الخريطة المُنتشرة مع الخريطة الحالية
                    // (EN) spread: merge the spread map into current map
                    pair.value->accept(*this);
                    Value spreadVal = lastResult_;
                    if (spreadVal.isMap())
                    {
                        for (const auto &[k, v] : spreadVal.toMapRef())
                        {
                            map[k] = v;
                        }
                    }
                    continue;
                }

                // تقييم المفتاح / Evaluate key
                pair.key->accept(*this);
                std::string key = lastResult_.toString();

                // تقييم القيمة / Evaluate value
                pair.value->accept(*this);
                Value value = lastResult_;

                map[key] = value;
            }

            lastResult_ = Value(map);
        }

        // =========================================================================
        // (AR) تقييم الفهرسة / (EN) Index Evaluation
        // =========================================================================

        void ExpressionEvaluator::visitIndexExpr(IndexExpr &node)
        {
            // تقييم الكائن المفهرس / Evaluate indexed object
            node.object->accept(*this);
            Value obj = lastResult_;

            // تقييم الفهرس / Evaluate index
            node.index->accept(*this);
            Value index = lastResult_;

            if (obj.isArray())
            {
                // فهرسة مصفوفة / Array indexing
                // (AR) قبول الأعداد العشرية التي تمثل أعداداً صحيحة (مثل 4.0) أو اقتطاعها (مثل 4.5 → 4)
                // (EN) Accept doubles that represent whole numbers (e.g. 4.0) or truncate them (e.g. 4.5 → 4)
                if (!index.isNumeric())
                {
                    Sad::Errors::ErrorManager::getInstance().reportError(
                        Sad::Errors::ErrorCode::RUN_INVALID_CAST,
                        Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                        "فهرس المصفوفة يجب أن يكون رقماً",
                        "Array index must be a number");
                    lastResult_ = Value();
                    return;
                }

                int idx = index.isInteger() ? index.toInt() : static_cast<int>(index.toDouble());
                const Value::ArrayType &arr = obj.toArrayRef();

                // (AR) دعم الفهرسة السالبة على نمط بايثون: -1 = آخر عنصر
                // (EN) Support Python-style negative indexing: -1 = last element
                if (idx < 0)
                    idx = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_calls_size") + idx;

                if (idx < 0 || idx >= ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_calls_size"))
                {
                    int sz = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_calls_size");
                    throw IndexOutOfRangeError(
                        "(AR) الفهرس " + std::to_string(idx) + " خارج النطاق. الفهارس الصالحة: 0 إلى " + std::to_string(sz - 1) + " (أو -" + std::to_string(sz) + " إلى -1). الحجم: " + std::to_string(sz) + " / " +
                            "(EN) Index " + std::to_string(idx) + " out of range. Valid indices: 0 to " + std::to_string(sz - 1) + " (or -" + std::to_string(sz) + " to -1). Size: " + std::to_string(sz),
                        node.position);
                }

                lastResult_ = arr[idx];
            }
            else if (obj.isMap())
            {
                // فهرسة قاموس / Map indexing
                // (AR) في حالة عدم وجود المفتاح، نُرجع عدم (null) بدلاً من خطأ
                // (EN) If key doesn't exist, return null instead of throwing error
                std::string key = index.toString();
                const Value::MapType &map = obj.toMapRef();

                auto it = map.find(key);
                if (it == map.end())
                {
                    lastResult_ = Value(); // null / عدم
                    return;
                }

                lastResult_ = it->second;
            }
            else if (obj.isString())
            {
                // (AR) فهرسة نص بأحرف UTF-8
                // (EN) String indexing by UTF-8 characters
                if (!index.isInteger())
                {
                    throw RuntimeError(
                        "(AR) فهرس النص يجب أن يكون رقم صحيح. (EN) String index must be integer.",
                        node.position);
                }
                std::string str = obj.toString();
                // (AR) تحويل النص إلى قائمة أحرف UTF-8
                std::vector<std::string> chars;
                for (size_t i = 0; i < str.size();)
                {
                    unsigned char c = static_cast<unsigned char>(str[i]);
                    size_t charLen = 1;
                    if (c >= 0xF0)
                        charLen = 4;
                    else if (c >= 0xE0)
                        charLen = 3;
                    else if (c >= 0xC0)
                        charLen = 2;
                    if (i + charLen > str.size())
                        charLen = 1;
                    chars.push_back(str.substr(i, charLen));
                    i += charLen;
                }
                int idx = index.toInt();
                if (idx < 0)
                    idx = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(chars.size(), "expression_evaluator_calls_size") + idx;
                if (idx < 0 || idx >= ::Sad::Security::SafeArithmetic::assertSafeCast<int>(chars.size(), "expression_evaluator_calls_size"))
                {
                    throw RuntimeError(
                        "(AR) فهرس النص " + std::to_string(idx) + " خارج النطاق (الطول: " + std::to_string(chars.size()) + "). " +
                            "(EN) String index " + std::to_string(idx) + " out of range (length: " + std::to_string(chars.size()) + ").",
                        node.position);
                }
                lastResult_ = Value(chars[idx]);
            }
            else if (obj.isTuple())
            {
                // (AR) فهرسة صف بفهرس رقمي / (EN) Tuple indexing by numeric index
                if (!index.isNumeric())
                {
                    throw RuntimeError(
                        "(AR) فهرس الصف يجب أن يكون رقماً. (EN) Tuple index must be a number.",
                        node.position);
                }

                int idx = index.isInteger() ? index.toInt() : static_cast<int>(index.toDouble());
                const auto &tupleElements = obj.toTupleRef();
                int sz = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(tupleElements.size(), "expression_evaluator_calls_size");

                // (AR) دعم الفهرسة السالبة / (EN) Support negative indexing
                if (idx < 0)
                    idx = sz + idx;

                if (idx < 0 || idx >= sz)
                {
                    throw IndexOutOfRangeError(
                        "(AR) فهرس الصف " + std::to_string(idx) + " خارج النطاق (الحجم: " + std::to_string(sz) + "). " +
                            "(EN) Tuple index " + std::to_string(idx) + " out of range (size: " + std::to_string(sz) + ").",
                        node.position);
                }

                lastResult_ = tupleElements[idx];
            }
            else if (obj.isObject() || obj.isObjectLike())
            {
                // ═══════════════════════════════════════════════════════════════
                // (AR) فحص تحميل العامل [] الزائد على الكائنات
                // (EN) Check operator[] overloading on objects
                // ═══════════════════════════════════════════════════════════════
                std::string className = obj.getClassName();
                if (!className.empty())
                {
                    auto *classManager = Data::ClassManager::getInstance();
                    Data::ClassType *classType = classManager->getClass(className);
                    if (classType)
                    {
                        Data::OperatorOverload *opOverload = classType->findOperator("[]");
                        if (opOverload && opOverload->body)
                        {
                            lastResult_ = executeOperatorOverload(obj, *opOverload, index, node.position);
                            return;
                        }
                    }
                }
                // (AR) لم يُعرَّف عامل [] — إبلاغ خطأ
                // (EN) No operator[] defined — report error
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                    Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                    "الكائن لا يدعم الفهرسة — عرّف 'عامل []' في الصنف",
                    "Object does not support indexing — define 'operator []' in the class");
                lastResult_ = Value();
            }
            else
            {
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                    Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                    "الفهرسة تعمل فقط على المصفوفات والقواميس والنصوص والصفوف",
                    "Indexing works only on arrays, maps, strings, and tuples");
                lastResult_ = Value();
            }
        }

        // =========================================================================
        // (AR) تقييم الشريحة / (EN) Slice Expression Evaluation
        // =========================================================================

        void ExpressionEvaluator::visitSliceExpr(SliceExpr &node)
        {
            // (AR) تقييم الكائن / (EN) Evaluate the object
            node.object->accept(*this);
            Value obj = lastResult_;

            // (AR) تقييم المؤشرات (إن وُجدت) / (EN) Evaluate indices (if present)
            auto evalOptional = [&](ExprPtr &expr) -> int
            {
                if (!expr)
                    return -1; // sentinel
                expr->accept(*this);
                Value v = lastResult_;
                return v.isInteger() ? v.toInt() : static_cast<int>(v.toDouble());
            };

            if (obj.isArray())
            {
                const Value::ArrayType &arr = obj.toArrayRef();
                int size = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_calls_size");

                // (AR) حساب البداية / (EN) Compute start
                int start = 0;
                if (node.start)
                {
                    node.start->accept(*this);
                    start = lastResult_.isInteger() ? lastResult_.toInt() : static_cast<int>(lastResult_.toDouble());
                    if (start < 0)
                        start = std::max(0, size + start);
                    if (start > size)
                        start = size;
                }

                // (AR) حساب النهاية / (EN) Compute end
                int end = size;
                if (node.end)
                {
                    node.end->accept(*this);
                    end = lastResult_.isInteger() ? lastResult_.toInt() : static_cast<int>(lastResult_.toDouble());
                    if (end < 0)
                        end = std::max(0, size + end);
                    if (end > size)
                        end = size;
                }

                // (AR) حساب الخطوة / (EN) Compute step
                int step = 1;
                if (node.step)
                {
                    node.step->accept(*this);
                    step = lastResult_.isInteger() ? lastResult_.toInt() : static_cast<int>(lastResult_.toDouble());
                    if (step == 0)
                    {
                        throw RuntimeError(
                            "(AR) خطوة الشريحة لا يمكن أن تكون صفر. (EN) Slice step cannot be zero.",
                            node.position);
                    }
                }

                // (AR) بناء الشريحة / (EN) Build slice
                Value::ArrayType result;
                if (step > 0)
                {
                    for (int i = start; i < end; i += step)
                    {
                        result.push_back(arr[i]);
                    }
                }
                else
                {
                    for (int i = start; i > end; i += step)
                    {
                        if (i >= 0 && i < size)
                            result.push_back(arr[i]);
                    }
                }
                lastResult_ = Value(std::move(result));
            }
            else if (obj.isString())
            {
                std::string str = obj.toString();
                // (AR) تحويل إلى أحرف UTF-8 / (EN) Convert to UTF-8 chars
                std::vector<std::string> chars;
                for (size_t i = 0; i < str.size();)
                {
                    unsigned char c = static_cast<unsigned char>(str[i]);
                    size_t charLen = 1;
                    if (c >= 0xF0)
                        charLen = 4;
                    else if (c >= 0xE0)
                        charLen = 3;
                    else if (c >= 0xC0)
                        charLen = 2;
                    if (i + charLen > str.size())
                        charLen = 1;
                    chars.push_back(str.substr(i, charLen));
                    i += charLen;
                }
                int size = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(chars.size(), "expression_evaluator_calls_size");

                int start = 0;
                if (node.start)
                {
                    node.start->accept(*this);
                    start = lastResult_.isInteger() ? lastResult_.toInt() : static_cast<int>(lastResult_.toDouble());
                    if (start < 0)
                        start = std::max(0, size + start);
                    if (start > size)
                        start = size;
                }
                int end = size;
                if (node.end)
                {
                    node.end->accept(*this);
                    end = lastResult_.isInteger() ? lastResult_.toInt() : static_cast<int>(lastResult_.toDouble());
                    if (end < 0)
                        end = std::max(0, size + end);
                    if (end > size)
                        end = size;
                }

                std::string result;
                for (int i = start; i < end; i++)
                {
                    if (i >= 0 && i < size)
                        result += chars[i];
                }
                lastResult_ = Value(std::move(result));
            }
            else
            {
                throw RuntimeError(
                    "(AR) الشريحة تعمل فقط على المصفوفات والنصوص. (EN) Slicing works only on arrays and strings.",
                    node.position);
            }
        }

        // =========================================================================
        // (AR) تقييم استدعاء الدالة / (EN) Function Call Evaluation
        // =========================================================================


    } // namespace Interpreter
} // namespace Sad

