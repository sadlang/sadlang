// ======================================================================
// expression_builder.h - بنّاء التعابير / Expression Builder
// ======================================================================
// (AR) Phase 6 — Step 7: صنف منفصل يجمع 30 method لبناء التعابير:
//      buildExpression (الموزع) + 4 أساسية (Literal/Variable/ShortCircuit/Unary)
//      + buildBinaryOp + 4 collections + 4 comprehensions + 2 functional
//      + 3 index/ternary + 3 lowlevel + 2 members + 4 nullsafety + 2 objects.
// (EN) Phase 6 — Step 7: standalone class grouping 30 expression builders.
// ======================================================================

#pragma once

#include "../sir_types.h"
#include "../sir_instruction.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Sad { namespace AST {
    class ArrayExpr;
    class AtomicExpr;
    class BinaryExpr;
    class DictComprehensionExpr;
    class ErrorPropagateExpr;
    class Expression;
    class GeneratorExpr;
    class IndexAssignExpr;
    class IndexExpr;
    class InlineAsmExpr;
    class LambdaExpr;
    class ListComprehensionExpr;
    class LiteralExpr;
    class MapExpr;
    class MemberAccessExpr;
    class MemberAssignExpr;
    class MemberExpr;
    class NewExpr;
    class NullCoalesceExpr;
    class OptionalChainExpr;
    class RangeExpr;
    class SetComprehensionExpr;
    class SizeofExpr;
    class SliceExpr;
    class TernaryExpr;
    class TupleExpr;
    class UnaryExpr;
    class VariableExpr;
    class WalrusExpr;
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
             * @brief (AR) بنّاء التعابير (30 method)
             * @brief (EN) Expression builder (30 methods)
             */
            class ExpressionBuilder
            {
            public:
                explicit ExpressionBuilder(SIRBuilder &b) : b_(b) {}

                BuildResult buildExpression(Sad::AST::Expression *expr);

                BuildResult buildLiteral(Sad::AST::LiteralExpr *literal);

                BuildResult buildVariableAccess(Sad::AST::VariableExpr *varNode);

                BuildResult buildShortCircuitLogical(Sad::AST::BinaryExpr *binOp);

                BuildResult buildUnaryOp(Sad::AST::UnaryExpr *unOp);

                BuildResult buildBinaryOp(Sad::AST::BinaryExpr *binOp);

                BuildResult buildExprArray(Sad::AST::ArrayExpr *expr);

                BuildResult buildExprTuple(Sad::AST::TupleExpr *expr);

                BuildResult buildExprMap(Sad::AST::MapExpr *expr);

                BuildResult buildExprWalrus(Sad::AST::WalrusExpr *expr);

                BuildResult buildExprListComp(Sad::AST::ListComprehensionExpr *expr);

                BuildResult buildExprDictComp(Sad::AST::DictComprehensionExpr *expr);

                BuildResult buildExprSetComp(Sad::AST::SetComprehensionExpr *expr);

                BuildResult buildExprGenerator(Sad::AST::GeneratorExpr *expr);

                BuildResult buildExprLambda(Sad::AST::LambdaExpr *expr);

                BuildResult buildExprRange(Sad::AST::RangeExpr *expr);

                BuildResult buildExprTernary(Sad::AST::TernaryExpr *expr);

                BuildResult buildExprIndex(Sad::AST::IndexExpr *expr);

                BuildResult buildExprIndexAssign(Sad::AST::IndexAssignExpr *expr);

                BuildResult buildExprInlineAsm(Sad::AST::InlineAsmExpr *expr);

                BuildResult buildExprSizeof(Sad::AST::SizeofExpr *expr);

                BuildResult buildExprAtomic(Sad::AST::AtomicExpr *expr);

                BuildResult buildExprMember(Sad::AST::MemberExpr *expr);

                BuildResult buildExprMemberAssign(Sad::AST::MemberAssignExpr *expr);

                BuildResult buildExprOptionalChain(Sad::AST::OptionalChainExpr *expr);

                BuildResult buildExprNullCoalesce(Sad::AST::NullCoalesceExpr *expr);

                BuildResult buildExprSlice(Sad::AST::SliceExpr *expr);

                BuildResult buildExprErrorPropagate(Sad::AST::ErrorPropagateExpr *expr);

                BuildResult buildNewObject(Sad::AST::NewExpr *expr);

                BuildResult buildMemberAccess(Sad::AST::MemberAccessExpr *expr);

            private:
                SIRBuilder &b_;
            };

        } // namespace SIR
    }     // namespace Compiler
} // namespace Sad
