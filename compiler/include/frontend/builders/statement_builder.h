// ======================================================================
// statement_builder.h - بنّاء العبارات / Statement Builder
// ======================================================================
// (AR) Phase 6 — Step 6: صنف منفصل يجمع 23 method لبناء العبارات:
//      buildStatement (الموزع), buildIfStatement, buildWhileLoop,
//      buildForLoop, buildForRangeLoop, buildMatchStatement,
//      buildReturnStatement, buildBreakStatement, buildContinueStatement,
//      buildAssignment, buildLocalVariable, buildDeferredClosure,
//      emitRunDeferredClosures, emitPopFunctionCleanupHandler,
//      buildStatement_Advanced/Go/Select/Exceptions/Extension/
//      Generators/Property/Test/Types.
// (EN) Phase 6 — Step 6: standalone class grouping 23 statement builders.
// ======================================================================

#pragma once

#include "../sir_types.h"
#include "../sir_instruction.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Sad { namespace AST {
    class Statement;
    class IfStmt;
    class AssignExpr;
    class VarDeclStmt;
    class ReturnStmt;
    class BreakStmt;
    class ContinueStmt;
    class ForRangeStmt;
    class MatchStmt;
    class WhileStmt;
    class ForStmt;
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
             * @brief (AR) بنّاء العبارات (23 method)
             * @brief (EN) Statement builder (23 methods)
             */
            class StatementBuilder
            {
            public:
                explicit StatementBuilder(SIRBuilder &b) : b_(b) {}

                void buildStatement(Sad::AST::Statement *stmt);

                void buildIfStatement(Sad::AST::IfStmt *ifStmt);

                void buildAssignment(Sad::AST::AssignExpr *assignment);

                void buildLocalVariable(Sad::AST::VarDeclStmt *varDecl);

                void buildReturnStatement(Sad::AST::ReturnStmt *retStmt);

                void buildBreakStatement(Sad::AST::BreakStmt *breakStmt);

                void buildContinueStatement(Sad::AST::ContinueStmt *continueStmt);

                void buildForRangeLoop(Sad::AST::ForRangeStmt *forRange);

                void buildMatchStatement(Sad::AST::MatchStmt *matchStmt);

                void buildWhileLoop(Sad::AST::WhileStmt *whileLoop);

                void buildForLoop(Sad::AST::ForStmt *forLoop);

                BuildResult buildDeferredClosure(Sad::AST::Statement *stmt);

                void emitRunDeferredClosures();

                void emitPopFunctionCleanupHandler();

                bool buildStatement_Advanced(Sad::AST::Statement *stmt);

                bool buildStatement_Go(Sad::AST::Statement *stmt);

                bool buildStatement_Select(Sad::AST::Statement *stmt);

                bool buildStatement_Exceptions(Sad::AST::Statement *stmt);

                bool buildStatement_Extension(Sad::AST::Statement *stmt);

                bool buildStatement_Generators(Sad::AST::Statement *stmt);

                bool buildStatement_Property(Sad::AST::Statement *stmt);

                bool buildStatement_Test(Sad::AST::Statement *stmt);

                bool buildStatement_Types(Sad::AST::Statement *stmt);

            private:
                SIRBuilder &b_;
            };

        } // namespace SIR
    }     // namespace Compiler
} // namespace Sad
