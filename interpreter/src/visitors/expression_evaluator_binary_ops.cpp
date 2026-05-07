/**
 * @file expression_evaluator_binary_ops.cpp
 * @brief (AR) تنفيذ التعبيرات الثنائية والعمليات المرتبطة في ExpressionEvaluator
 * @brief (EN) Binary expression operations in ExpressionEvaluator
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
#include "runtime_throw.h"
#include "user_thrown.h"
#include "runtime_throw.h"
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

        void ExpressionEvaluator::visitBinaryExpr(BinaryExpr &node)
        {
            // ═══════════════════════════════════════════════════════════════════
            // (AR) تقييم كسول للعمليات المنطقية && و || — لا يُقيّم الطرف الأيمن إلا عند الحاجة
            // (EN) Short-circuit evaluation for && and || — right-side evaluated only when needed
            // ═══════════════════════════════════════════════════════════════════
            if (node.op == TokenType::OP_AND)
            {
                node.left->accept(*this);
                Value left = std::move(lastResult_);
                if (!convertObjectToBool(left, node.position))
                {
                    lastResult_ = Value(false);
                    return;
                }
                node.right->accept(*this);
                lastResult_ = Value(convertObjectToBool(lastResult_, node.position));
                return;
            }
            if (node.op == TokenType::OP_OR)
            {
                node.left->accept(*this);
                Value left = lastResult_;
                if (convertObjectToBool(left, node.position))
                {
                    lastResult_ = Value(true);
                    return;
                }
                node.right->accept(*this);
                lastResult_ = Value(convertObjectToBool(lastResult_, node.position));
                return;
            }

            // (AR) تقييم الطرف الأيسر مع نقل القيمة بدلاً من نسخها
            // (EN) Evaluate left operand with move instead of copy
            node.left->accept(*this);
            Value left = std::move(lastResult_);

            // (AR) تقييم الطرف الأيمن مع نقل القيمة
            // (EN) Evaluate right operand with move
            node.right->accept(*this);
            Value right = std::move(lastResult_);

            // ═══════════════════════════════════════════════════════════════════
            // (AR) فحص تحميل العامل الزائد على الكائنات — يدعم OBJECT و MAP
            // (EN) Check operator overloading on objects — supports OBJECT and MAP
            // ═══════════════════════════════════════════════════════════════════
            if (left.isObject() || left.isObjectLike())
            {
                std::string className = left.getClassName();
                if (!className.empty())
                {
                    auto *classManager = Data::ClassManager::getInstance();
                    Data::ClassType *classType = classManager->getClass(className);
                    if (classType)
                    {
                        // (AR) تحويل رمز العامل من TokenType إلى نص / (EN) Convert operator token to string
                        std::string opSymbol;
                        switch (node.op)
                        {
                        case TokenType::OP_PLUS:
                            opSymbol = "+";
                            break;
                        case TokenType::OP_MINUS:
                            opSymbol = "-";
                            break;
                        case TokenType::OP_MULTIPLY:
                            opSymbol = "*";
                            break;
                        case TokenType::OP_DIVIDE:
                            opSymbol = "/";
                            break;
                        case TokenType::OP_FLOOR_DIVIDE:
                            opSymbol = "//";
                            break;
                        case TokenType::OP_MODULO:
                            opSymbol = "%";
                            break;
                        case TokenType::OP_POWER:
                            opSymbol = "**";
                            break;
                        case TokenType::OP_EQUAL:
                            opSymbol = "==";
                            break;
                        case TokenType::OP_NOT_EQUAL:
                            opSymbol = "!=";
                            break;
                        case TokenType::OP_LESS:
                            opSymbol = "<";
                            break;
                        case TokenType::OP_LESS_EQUAL:
                            opSymbol = "<=";
                            break;
                        case TokenType::OP_GREATER:
                            opSymbol = ">";
                            break;
                        case TokenType::OP_GREATER_EQUAL:
                            opSymbol = ">=";
                            break;
                        // (AR) العوامل البتية / (EN) Bitwise operators
                        case TokenType::OP_XOR:
                            opSymbol = "^";
                            break;
                        case TokenType::OP_BITWISE_AND:
                            opSymbol = "&";
                            break;
                        case TokenType::OP_BITWISE_OR:
                            opSymbol = "|";
                            break;
                        case TokenType::OP_SHIFT_LEFT:
                            opSymbol = "<<";
                            break;
                        case TokenType::OP_SHIFT_RIGHT:
                            opSymbol = ">>";
                            break;
                        default:
                            break;
                        }

                        if (!opSymbol.empty())
                        {
                            Data::OperatorOverload *opOverload = classType->findOperator(opSymbol);
                            if (opOverload && opOverload->body)
                            {
                                // (AR) تنفيذ العامل المحمل زائداً / (EN) Execute operator overload
                                lastResult_ = executeOperatorOverload(left, *opOverload, right, node.position);
                                return;
                            }
                        }
                    }
                }
            }

            // تحديد نوع العملية / Determine operation type
            switch (node.op)
            {
            // (AR) عمليات حسابية / (EN) Arithmetic operations
            case TokenType::OP_PLUS:
            case TokenType::OP_MINUS:
            case TokenType::OP_MULTIPLY:
            case TokenType::OP_DIVIDE:
            case TokenType::OP_FLOOR_DIVIDE:
            case TokenType::OP_MODULO:
            case TokenType::OP_POWER:
                lastResult_ = evaluateArithmeticOp(left, node.op, right, node.position);
                break;

            // (AR) عمليات مقارنة / (EN) Comparison operations
            case TokenType::OP_EQUAL:
            case TokenType::OP_NOT_EQUAL:
            case TokenType::OP_LESS:
            case TokenType::OP_LESS_EQUAL:
            case TokenType::OP_GREATER:
            case TokenType::OP_GREATER_EQUAL:
                lastResult_ = evaluateComparisonOp(left, node.op, right, node.position);
                break;

            // (AR) عمليات منطقية / (EN) Logical operations
            case TokenType::OP_AND:
            case TokenType::OP_OR:
                lastResult_ = evaluateLogicalOp(left, node.op, right, node.position);
                break;

            // (AR) عمليات البت / (EN) Bitwise operations
            case TokenType::OP_XOR:
            case TokenType::OP_BITWISE_AND:
            case TokenType::OP_BITWISE_OR:
            case TokenType::OP_SHIFT_LEFT:
            case TokenType::OP_SHIFT_RIGHT:
                lastResult_ = evaluateBitwiseOp(left, node.op, right, node.position);
                break;

            // (AR) عامل العضوية: في / (EN) Membership operator: in
            case TokenType::KEYWORD_IN:
            {
                bool found = false;
                if (right.isArray())
                {
                    for (const auto &el : right.toArrayRef())
                    {
                        if ((left == el).toBool())
                        {
                            found = true;
                            break;
                        }
                    }
                }
                else if (right.isMap())
                {
                    std::string key = left.toString();
                    auto map = right.toMap();
                    found = (map.find(key) != map.end());
                }
                else if (right.isString())
                {
                    found = (right.toString().find(left.toString()) != std::string::npos);
                }
                else
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_IN_OPERATOR_RHS_INVALID,
                        node.position,
                        {{"type", right.getTypeName()}});
                }
                lastResult_ = Value(found);
                break;
            }

            default:
            {
                // (AR) حساب رمز العامل للرسالة المحسنة
                // (EN) Compute operator symbol for improved message
                std::string opStr = "?";
                switch (node.op)
                {
                case TokenType::OP_PLUS:
                    opStr = "+";
                    break;
                case TokenType::OP_MINUS:
                    opStr = "-";
                    break;
                case TokenType::OP_MULTIPLY:
                    opStr = "*";
                    break;
                case TokenType::OP_DIVIDE:
                    opStr = "/";
                    break;
                case TokenType::OP_FLOOR_DIVIDE:
                    opStr = "//";
                    break;
                case TokenType::OP_MODULO:
                    opStr = "%";
                    break;
                case TokenType::OP_POWER:
                    opStr = "**";
                    break;
                case TokenType::OP_XOR:
                    opStr = "^";
                    break;
                default:
                    break;
                }
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                    Sad::Errors::SourceLocation(
                        Sad::Errors::ErrorManager::getInstance().getSourceFilename().empty() ? "<input>" : Sad::Errors::ErrorManager::getInstance().getSourceFilename(),
                        static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                    "العملية '" + opStr + "' غير مدعومة بين نوعي '" + left.getTypeName() + "' و '" + right.getTypeName() + "'",
                    "Operation '" + opStr + "' not supported between types '" + left.getTypeName() + "' and '" + right.getTypeName() + "'");
                lastResult_ = Value(); // Return null
            }
            }
        }

        // =========================================================================
        // (AR) العمليات الحسابية / (EN) Arithmetic Operations
        // =========================================================================

        Value ExpressionEvaluator::evaluateArithmeticOp(const Value &left, TokenType op, const Value &right, const Lexer::Position &pos)
        {
            // جمع النصوص (string concatenation) / String concatenation
            // (AR) يدعم عامل نص() الضمني للكائنات
            if (op == TokenType::OP_PLUS && (left.isString() || right.isString()))
            {
                return Value(convertObjectToString(left, pos) + convertObjectToString(right, pos));
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) عمليات المصفوفات: دمج (+) وتكرار (*)
            // (EN) Array operations: concatenation (+) and repetition (*)
            // ═══════════════════════════════════════════════════════════════════
            if (left.isArray() || right.isArray())
            {
                if (op == TokenType::OP_PLUS)
                {
                    // (AR) دمج المصفوفات: [1,2] + [3,4] → [1,2,3,4]
                    //      أو إضافة عنصر: [1,2] + 3 → [1,2,3]
                    return left + right;
                }
                if (op == TokenType::OP_MULTIPLY)
                {
                    // (AR) تكرار المصفوفة: [1,2] * 3 → [1,2,1,2,1,2]
                    return left * right;
                }
            }

            // (AR) تكرار النصوص: "ها" * 3 → "هاهاها"
            // (EN) String repetition: "ha" * 3 → "hahaha"
            if (op == TokenType::OP_MULTIPLY && (left.isString() || right.isString()))
            {
                return left * right;
            }

            // التأكد من أن الطرفين رقميين / Ensure both operands are numeric
            if (!left.isNumeric() || !right.isNumeric())
            {
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::RUN_INVALID_CAST,
                    Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column),
                    "العمليات الحسابية تتطلب قيم رقمية",
                    "Arithmetic operations require numeric values");
                return Value(0); // Return default
            }

            // تحويل لـ double إذا كان أحدهما double
            // Convert to double if either is double
            bool useDouble = left.isDouble() || right.isDouble();

            if (useDouble)
            {
                double l = left.toDouble();
                double r = right.toDouble();

                switch (op)
                {
                case TokenType::OP_PLUS:
                    return Value(l + r);
                case TokenType::OP_MINUS:
                    return Value(l - r);
                case TokenType::OP_MULTIPLY:
                    return Value(l * r);
                case TokenType::OP_DIVIDE:
                    if (r == 0.0)
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_DIVISION_BY_ZERO, pos,
                            {{"a", std::to_string(l)}});
                    return Value(l / r);
                case TokenType::OP_FLOOR_DIVIDE:
                    if (r == 0.0)
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_FLOOR_DIVISION_BY_ZERO, pos,
                            {{"a", std::to_string(l)}});
                    return Value(std::floor(l / r));
                case TokenType::OP_POWER:
                    return Value(std::pow(l, r));
                case TokenType::OP_MODULO:
                    if (r == 0.0)
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MODULO_BY_ZERO, pos,
                            {{"a", std::to_string(l)}});
                    return Value(std::fmod(l, r));
                default:
                    break;
                }
            }
            else
            {
                int l = left.toInt();
                int r = right.toInt();

                // (AR) حماية طفحان الأعداد الصحيحة — الترقية إلى double عند الطفحان
                // (EN) Integer overflow protection — promote to double on overflow
                auto safeAdd = [](int a, int b) -> Value
                {
                    if ((b > 0 && a > INT_MAX - b) || (b < 0 && a < INT_MIN - b))
                        return Value(static_cast<double>(a) + static_cast<double>(b));
                    return Value(a + b);
                };
                auto safeSub = [](int a, int b) -> Value
                {
                    if ((b < 0 && a > INT_MAX + b) || (b > 0 && a < INT_MIN + b))
                        return Value(static_cast<double>(a) - static_cast<double>(b));
                    return Value(a - b);
                };
                auto safeMul = [](int a, int b) -> Value
                {
                    if (a != 0 && b != 0)
                    {
                        if ((a > 0 && b > 0 && a > INT_MAX / b) ||
                            (a < 0 && b < 0 && a < INT_MAX / b) ||
                            (a > 0 && b < 0 && b < INT_MIN / a) ||
                            (a < 0 && b > 0 && a < INT_MIN / b))
                            return Value(static_cast<double>(a) * static_cast<double>(b));
                    }
                    return Value(a * b);
                };

                switch (op)
                {
                case TokenType::OP_PLUS:
                    return safeAdd(l, r);
                case TokenType::OP_MINUS:
                    return safeSub(l, r);
                case TokenType::OP_MULTIPLY:
                    return safeMul(l, r);
                case TokenType::OP_DIVIDE:
                    if (r == 0)
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_DIVISION_BY_ZERO, pos,
                            {{"a", std::to_string(l)}});
                    // (AR) ترقية إلى عشري عند وجود باقي (7/2 → 3.5) — سلوك القسمة الحقيقية
                    // (EN) Promote to double when remainder exists (7/2 → 3.5) — true division
                    if (l % r != 0)
                        return Value(static_cast<double>(l) / static_cast<double>(r));
                    return Value(l / r);
                case TokenType::OP_FLOOR_DIVIDE:
                    if (r == 0)
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_FLOOR_DIVISION_BY_ZERO, pos,
                            {{"a", std::to_string(l)}});
                    // (AR) القسمة الصحيحة الأرضية: -7 // 2 → -4 (نحو سالب اللانهاية)
                    // (EN) Floor division: -7 // 2 → -4 (toward negative infinity)
                    {
                        int q = l / r;
                        if ((l ^ r) < 0 && l % r != 0)
                            q -= 1;
                        return Value(q);
                    }
                case TokenType::OP_MODULO:
                    if (r == 0)
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MODULO_BY_ZERO, pos,
                            {{"a", std::to_string(l)}});
                    return Value(l % r);
                case TokenType::OP_POWER:
                {
                    double result = std::pow(static_cast<double>(l), static_cast<double>(r));
                    if (result >= static_cast<double>(INT_MIN) && result <= static_cast<double>(INT_MAX))
                        return Value(static_cast<int>(result));
                    return Value(result);
                }
                default:
                    break;
                }
            }

            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column),
                "عملية حسابية غير مدعومة",
                "Unsupported arithmetic operation");
            return Value(0); // Return default
        }

        // =========================================================================
        // (AR) عمليات المقارنة / (EN) Comparison Operations
        // =========================================================================

    } // namespace Interpreter
} // namespace Sad
