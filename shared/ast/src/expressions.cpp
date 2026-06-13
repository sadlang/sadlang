/**
 * @file expressions.cpp
 * @brief Expression AST node implementations / تنفيذ عُقد AST للتعابير
 * @author Sad Language Team
 * @date 11 November 2025
 */

#include "expressions.h"
#include "sad_type_system.h"
#include <sstream>

namespace Sad
{
    namespace AST
    {

        // (AR) مساعد محلي — اختصار للوصول إلى سجل الأنواع الموحد
        // (EN) Local helper — shorthand for accessing the unified type registry
        static Types::SadTypeRegistry &reg()
        {
            return Types::SadTypeRegistry::instance();
        }

        // =========================================================================
        // BinaryExpr Implementation
        // =========================================================================

        Types::SadTypePtr BinaryExpr::getType() const
        {
            // Type inference based on operator and operands
            // استنتاج النوع بناءً على العامل والمعاملات

            using TT = Lexer::TokenType;

            // Logical operators always return boolean
            // العوامل المنطقية تُرجع دائماً قيمة منطقية
            if (op == TT::OP_AND || op == TT::OP_OR || op == TT::OP_NOT ||
                op == TT::OP_EQUAL || op == TT::OP_NOT_EQUAL ||
                op == TT::OP_LESS || op == TT::OP_LESS_EQUAL ||
                op == TT::OP_GREATER || op == TT::OP_GREATER_EQUAL)
            {
                return reg().getBoolean();
            }

            // Arithmetic operators: if either operand is float, result is float
            // العوامل الحسابية: إذا كان أحد المعاملات float، النتيجة float
            // (S-TS-P2.5a) المحور SadTypeKind (لا الجسر القديم)
            auto leftDT = left->getTypeKind();
            auto rightDT = right->getTypeKind();

            if (leftDT == Types::SadTypeKind::Float || rightDT == Types::SadTypeKind::Float)
            {
                return reg().getFloat();
            }

            if (leftDT == Types::SadTypeKind::String || rightDT == Types::SadTypeKind::String)
            {
                return reg().getString();
            }

            // Default to integer for arithmetic
            return reg().getInteger();
        }

        // =========================================================================
        // UnaryExpr Implementation
        // =========================================================================

        Types::SadTypePtr UnaryExpr::getType() const
        {
            using TT = Lexer::TokenType;

            // Logical NOT returns boolean
            if (op == TT::OP_NOT)
            {
                return reg().getBoolean();
            }

            // Other unary operators preserve operand type
            return operand->getType();
        }

        // =========================================================================
        // TernaryExpr Implementation
        // =========================================================================

        /**
         * @brief (AR) يحسب نوع البيانات للتعبير الثلاثي الشرطي.
         *        (EN) Determines data type for ternary conditional expression.
         *
         * The ternary expression's type is determined by the common type of both branches.
         * If both branches have the same type, that type is returned.
         * Otherwise, a common type is inferred (e.g., if one is INT and other is FLOAT, returns FLOAT).
         *
         * نوع التعبير الثلاثي يُحدد بالنوع المشترك للفرعين.
         * إذا كان الفرعان من نفس النوع، يُرجع ذلك النوع.
         * وإلا، يُستنتج نوع مشترك (مثلاً، إذا كان أحدهما INT والآخر FLOAT، يُرجع FLOAT).
         */
        Types::SadTypePtr TernaryExpr::getType() const
        {
            // (S-TS-P2.5a) المحور SadTypeKind (لا الجسر القديم)
            auto trueDT = trueExpr->getTypeKind();
            auto falseDT = falseExpr->getTypeKind();

            // If both branches have same type, return it
            // إذا كان الفرعان من نفس النوع، ارجعه
            if (trueDT == falseDT)
            {
                return trueExpr->getType();
            }

            // Numeric type promotion: FLOAT takes precedence
            // ترقية النوع الرقمي: FLOAT له الأولوية
            if ((trueDT == Types::SadTypeKind::Integer && falseDT == Types::SadTypeKind::Float) ||
                (trueDT == Types::SadTypeKind::Float && falseDT == Types::SadTypeKind::Integer))
            {
                return reg().getFloat();
            }

            // If types differ and no clear promotion, default to the true branch type
            // إذا اختلفت الأنواع ولا توجد ترقية واضحة، افتراضياً نوع الفرع الصحيح
            return trueExpr->getType();
        }

        // =========================================================================
        // LiteralExpr Implementation
        // =========================================================================

        Types::SadTypePtr LiteralExpr::getType() const
        {
            using TT = Lexer::TokenType;

            switch (token.getType())
            {
            case TT::NUMBER_INTEGER:
                return reg().getInteger();

            case TT::NUMBER_DOUBLE:
                return reg().getFloat();

            case TT::STRING_LITERAL:
                return reg().getString();

            case TT::LITERAL_TRUE:
            case TT::LITERAL_FALSE:
                return reg().getBoolean();

            case TT::LITERAL_NULL:
                return reg().getVoid();

            default:
                return nullptr;
            }
        }

        // =========================================================================
        // VariableExpr Implementation
        // =========================================================================

        Types::SadTypePtr VariableExpr::getType() const
        {
            // Type will be resolved during semantic analysis
            // سيتم تحديد النوع أثناء التحليل الدلالي
            return nullptr;
        }

        // =========================================================================
        // AssignExpr Implementation
        // =========================================================================

        Types::SadTypePtr AssignExpr::getType() const
        {
            // Assignment expression returns the type of the assigned value
            return value->getType();
        }

        // =========================================================================
        // CallExpr Implementation
        // =========================================================================

        std::string CallExpr::toString() const
        {
            std::ostringstream oss;
            oss << callee->toString() << "(";

            for (size_t i = 0; i < arguments.size(); ++i)
            {
                if (i > 0)
                    oss << ", ";
                oss << arguments[i]->toString();
            }

            oss << ")";
            return oss.str();
        }

        Types::SadTypePtr CallExpr::getType() const
        {
            // Return type will be determined during semantic analysis
            // نوع الإرجاع سيتحدد أثناء التحليل الدلالي
            return nullptr;
        }

        // =========================================================================
        // IndexExpr Implementation
        // =========================================================================

        Types::SadTypePtr IndexExpr::getType() const
        {
            auto objDT = object->getTypeKind(); // (S-TS-P2.5a) المحور SadTypeKind

            // Arrays and strings return element type
            // المصفوفات والنصوص تُرجع نوع العنصر
            if (objDT == Types::SadTypeKind::Array)
            {
                // Element type will be determined during semantic analysis
                return nullptr;
            }

            if (objDT == Types::SadTypeKind::String)
            {
                return reg().getString();
            }

            if (objDT == Types::SadTypeKind::Map)
            {
                // Value type will be determined during semantic analysis
                return nullptr;
            }

            return nullptr;
        }

        // =========================================================================
        // MemberExpr Implementation
        // =========================================================================

        Types::SadTypePtr MemberExpr::getType() const
        {
            // Member type will be determined during semantic analysis
            // نوع العضو سيتحدد أثناء التحليل الدلالي
            return nullptr;
        }

        // =========================================================================
        // ArrayExpr Implementation
        // =========================================================================

        std::string ArrayExpr::toString() const
        {
            std::ostringstream oss;
            oss << "[";

            for (size_t i = 0; i < elements.size(); ++i)
            {
                if (i > 0)
                    oss << ", ";
                oss << elements[i]->toString();
            }

            oss << "]";
            return oss.str();
        }

        // =========================================================================
        // MapExpr Implementation
        // =========================================================================

        std::string MapExpr::toString() const
        {
            std::ostringstream oss;
            oss << "{";

            for (size_t i = 0; i < pairs.size(); ++i)
            {
                if (i > 0)
                    oss << ", ";
                oss << pairs[i].key->toString() << ": " << pairs[i].value->toString();
            }

            oss << "}";
            return oss.str();
        }

        // =========================================================================
        // LambdaExpr Implementation
        // =========================================================================

        std::string LambdaExpr::toString() const
        {
            std::ostringstream oss;
            oss << "(";

            for (size_t i = 0; i < parameters.size(); ++i)
            {
                if (i > 0)
                    oss << ", ";
                oss << parameters[i].name;
            }

            oss << ") => " << body->toString();
            return oss.str();
        }

        // =========================================================================
        // ListComprehensionExpr Implementation
        // =========================================================================

        std::string ListComprehensionExpr::toString() const
        {
            std::ostringstream oss;
            oss << "[" << element->toString() << " for " << variable
                << " in " << iterable->toString();

            if (condition)
            {
                oss << " if " << condition->toString();
            }

            oss << "]";
            return oss.str();
        }

        // =========================================================================
        // DictComprehensionExpr Implementation
        // =========================================================================

        std::string DictComprehensionExpr::toString() const
        {
            std::ostringstream oss;
            oss << "{" << key->toString() << ": " << value->toString()
                << " for " << variable << " in " << iterable->toString();

            if (condition)
            {
                oss << " if " << condition->toString();
            }

            oss << "}";
            return oss.str();
        }

        // =========================================================================
        // GeneratorExpr Implementation
        // =========================================================================

        std::string GeneratorExpr::toString() const
        {
            std::ostringstream oss;
            oss << "(" << element->toString() << " for " << variable
                << " in " << iterable->toString();

            if (condition)
            {
                oss << " if " << condition->toString();
            }

            oss << ")";
            return oss.str();
        }

        // =========================================================================
        // Decorator Expression / تعبير المُزخرِف
        // =========================================================================

        std::string DecoratorExpr::toString() const
        {
            std::ostringstream oss;
            oss << "@" << name;

            if (hasArguments && !arguments.empty())
            {
                oss << "(";
                for (size_t i = 0; i < arguments.size(); ++i)
                {
                    if (i > 0)
                        oss << ", ";
                    oss << arguments[i]->toString();
                }
                oss << ")";
            }

            return oss.str();
        }

        // =========================================================================
        // OptionalChainExpr Implementation / تعبير الوصول الآمن
        // =========================================================================

        Types::SadTypePtr OptionalChainExpr::getType() const
        {
            // (AR) النوع يعتمد على العضو — غير معروف حتى وقت التشغيل
            // (EN) Type depends on member — unknown until runtime
            return nullptr;
        }

        // =========================================================================
        // NullCoalesceExpr Implementation / تعبير التجميع الفارغ
        // =========================================================================

        Types::SadTypePtr NullCoalesceExpr::getType() const
        {
            // (AR) النوع يعتمد على الطرفين — غير معروف حتى وقت التشغيل
            // (EN) Type depends on both sides — unknown until runtime
            return nullptr;
        }

    } // namespace AST
} // namespace Sad
