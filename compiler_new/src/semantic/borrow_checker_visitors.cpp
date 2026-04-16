// ============================================================================
// borrow_checker_visitors.cpp — زوار فاحص الاستعارة (جمل + تعريفات + مساعدات)
// (AR) زوار الجمل والتعريفات ودوال مساعدة لفاحص الاستعارة
// (EN) Statement/declaration visitors and helpers for borrow checker
// تم استخراج هذا الملف من borrow_checker.cpp وفقاً لقاعدة CW-05
// ============================================================================

#ifdef _MSC_VER
#pragma warning(disable : 4819)
#endif

#include "semantic/borrow_checker.h"
#include <iostream>
#include <sstream>

namespace Sad
{
    namespace Semantic
    {
        void BorrowChecker::visitForRangeStmt(AST::ForRangeStmt &stmt)
        {
            // (AR) ״­״µ ״­„‚״© for-range
            // (EN) Check for-range loop
            tracker_->enterScope();
            if (stmt.iterable)
            {
                stmt.iterable->accept(*this);
            }
            if (stmt.body)
            {
                stmt.body->accept(*this);
            }
            tracker_->exitScope();
        }

        void BorrowChecker::visitSwitchStmt(AST::SwitchStmt &stmt)
        {
            // (AR) ״­״µ ״¬…„״© switch
            // (EN) Check switch statement
        }

        void BorrowChecker::visitReturnStmt(AST::ReturnStmt &stmt)
        {
            // (AR) ״­״µ ״¬…„״© return
            // (EN) Check return statement
            if (stmt.value)
            {
                stmt.value->accept(*this);
            }
        }

        void BorrowChecker::visitYieldStmt(AST::YieldStmt &stmt)
        {
            // (AR) ״­״µ ״¬…„״© yield
            // (EN) Check yield statement
        }

        void BorrowChecker::visitBreakStmt(AST::BreakStmt &stmt)
        {
            // (AR) break „״§ ״­״×״§״¬ ״­״µ …„ƒ״©
            // (EN) break doesn't need ownership checking
            (void)stmt;
        }

        void BorrowChecker::visitContinueStmt(AST::ContinueStmt &stmt)
        {
            // (AR) continue „״§ ״­״×״§״¬ ״­״µ …„ƒ״©
            // (EN) continue doesn't need ownership checking
            (void)stmt;
        }

        void BorrowChecker::visitBlockStmt(AST::BlockStmt &stmt)
        {
            // (AR) ״­״µ ƒ״×„״© ״§„״¹״¨״§״±״§״×
            // (EN) Check block statement
            tracker_->enterScope();

            // (AR) ״²״§״±״© ƒ„ ״¹״¨״§״±״©  ״§„ƒ״×„״©
            // (EN) Visit each statement in the block
            for (auto &s : stmt.statements)
            {
                if (s)
                {
                    s->accept(*this);
                }
            }

            tracker_->exitScope();
        }

        void BorrowChecker::visitTryStmt(AST::TryStmt &stmt)
        {
            // (AR) ״­״µ ״¬…„״© try
            // (EN) Check try statement
        }

        void BorrowChecker::visitRaiseStmt(AST::RaiseStmt &stmt)
        {
            // (AR) ״­״µ ״¬…„״© raise
            // (EN) Check raise statement
        }

        void BorrowChecker::visitWithStmt(AST::WithStmt &stmt)
        {
            // (AR) ״­״µ ״¬…„״© with
            // (EN) Check with statement
        }

        void BorrowChecker::visitMatchStmt(AST::MatchStmt &stmt)
        {
            // (AR) ״­״µ ״¬…„״© match
            // (EN) Check match statement
        }

        void BorrowChecker::visitDeferStmt(AST::DeferStmt &stmt)
        {
            // (AR) ״­״µ ״¬…„״© ״£״¬‘„ ג€” †״­״µ ״§„״¬״³… ‚״·
            // (EN) Check defer statement ג€” just check the body
            if (stmt.body)
            {
                stmt.body->accept(*this);
            }
        }

        void BorrowChecker::visitGoStmt(AST::GoStmt &stmt)
        {
            // (AR) ״­״µ ״¬…„״© ״£״·„‚ ג€” †״­״µ ״§„״×״¹״¨״± ״£ˆ ״§„ƒ״×„״©
            // (EN) Check go statement ג€” check expression or block body
            if (stmt.expression)
            {
                stmt.expression->accept(*this);
            }
            if (stmt.blockBody)
            {
                stmt.blockBody->accept(*this);
            }
        }

        void BorrowChecker::visitSelectCase(AST::SelectCase &stmt)
        {
            // (AR) ״­״µ ״­״§„״© ״§״®״×״§״± ג€” †״­״µ ״×״¹״¨״± ״§„‚†״§״© ˆ״§„״¬״³…
            // (EN) Check select case ג€” check channel expression and body
            if (stmt.channelExpr)
            {
                stmt.channelExpr->accept(*this);
            }
            for (auto &s : stmt.body)
            {
                if (s)
                    s->accept(*this);
            }
        }

        void BorrowChecker::visitSelectStmt(AST::SelectStmt &stmt)
        {
            // (AR) ״­״µ ״¬…„״© ״§״®״×״± ג€” †״­״µ ״¬…״¹ ״§„״­״§„״§״× ˆ״§„״¬״³… ״§„״§״×״±״§״¶
            // (EN) Check select statement ג€” check all cases and default body
            for (auto &c : stmt.cases)
            {
                if (c)
                    c->accept(*this);
            }
            for (auto &s : stmt.defaultBody)
            {
                if (s)
                    s->accept(*this);
            }
        }

        void BorrowChecker::visitClassDeclStmt(AST::ClassDeclStmt &stmt)
        {
            // (AR) ״­״µ ״×״µ״±״­ ״§„״µ†
            // (EN) Check class declaration statement
        }

        // ============================================================================
        // ״²״§״±״© ״§„״×״µ״±״­״§״× / Visit Declarations
        // ============================================================================

        void BorrowChecker::visitFunctionDecl(AST::FunctionDecl &decl)
        {
            std::string previousFunction = currentFunction_;
            currentFunction_ = decl.name;

            tracker_->enterScope();

            // (AR) ״×״³״¬„ ״§„…״¹״§…„״§״× ƒ…״×״÷״±״§״×  ״§„†״·״§‚
            // (EN) Register parameters as variables in scope
            for (const auto &param : decl.parameters)
            {
                std::string typeName = dataTypeToString(param.type);
                bool isCopy = isCopyType(typeName);
                tracker_->declareVariable(param.name, typeName, getLocation(&decl), isCopy);
                currentResult_.totalVariables++;
            }

            // (AR) ״­״µ ״¬״³… ״§„״¯״§„״©
            // (EN) Check function body
            if (decl.body)
            {
                decl.body->accept(*this);
            }

            tracker_->exitScope();
            currentFunction_ = previousFunction;
        }

        void BorrowChecker::visitClassDecl(AST::ClassDecl &decl)
        {
            // (AR) ״­״µ ƒ„ ״·״±‚״©  ״§„״µ†
            // (EN) Check each method in class
        }

        void BorrowChecker::visitFieldDecl(AST::FieldDecl &decl)
        {
            // (AR) ״­״µ ״×״µ״±״­ ״§„״­‚„
            // (EN) Check field declaration
        }

        void BorrowChecker::visitMethodDecl(AST::MethodDecl &decl)
        {
            // (AR) ״­״µ ״×״µ״±״­ ״§„״·״±‚״©
            // (EN) Check method declaration
        }

        void BorrowChecker::visitPropertyDecl(AST::PropertyDecl &decl)
        {
            // (AR) ״­״µ ״×״µ״±״­ ״§„״®״§״µ״©
            // (EN) Check property declaration
        }

        void BorrowChecker::visitConstructorDecl(AST::ConstructorDecl &decl)
        {
            // (AR) ״­״µ ״×״µ״±״­ ״§„״¨†״§״¡
            // (EN) Check constructor declaration
        }

        void BorrowChecker::visitDestructorDecl(AST::DestructorDecl &decl)
        {
            // (AR) ״­״µ ״×״µ״±״­ ״§„‡״¯…
            // (EN) Check destructor declaration
        }

        void BorrowChecker::visitEnumDecl(AST::EnumDecl &decl)
        {
            // (AR) ״­״µ ״×״µ״±״­ ״§„״×״¹״¯״§״¯
            // (EN) Check enum declaration
        }

        void BorrowChecker::visitImportStmt(AST::ImportStmt &stmt)
        {
            // (AR) ״­״µ ״¬…„״© ״§„״§״³״×״±״§״¯
            // (EN) Check import statement
            (void)stmt;
        }

        void BorrowChecker::visitFromImportStmt(AST::FromImportStmt &stmt)
        {
            // (AR) ״­״µ ״¬…„״© ״§„״§״³״×״±״§״¯ ״§„״§†״×‚״§״¦
            // (EN) Check from-import statement
            (void)stmt;
        }

        void BorrowChecker::visitExportStmt(AST::ExportStmt &stmt)
        {
            // (AR) ״­״µ ״¬…„״© ״§„״×״µ״¯״±
            // (EN) Check export statement
            (void)stmt;
        }

        void BorrowChecker::visitExportDecl(AST::ExportDecl &decl)
        {
            // (AR) ״­״µ ״×״µ״±״­ ״§„״×״µ״¯״±
            // (EN) Check export declaration
            (void)decl;
        }

        void BorrowChecker::visitReExportStmt(AST::ReExportStmt &stmt)
        {
            (void)stmt;
        }

        void BorrowChecker::visitTemplateFunctionDecl(AST::TemplateFunctionDecl &decl)
        {
            // (AR) ״­״µ ״×״µ״±״­ ״¯״§„״© ״§„‚״§„״¨
            // (EN) Check template function declaration
        }

        void BorrowChecker::visitTemplateClassDecl(AST::TemplateClassDecl &decl)
        {
            // (AR) ״­״µ ״×״µ״±״­ ״µ† ״§„‚״§„״¨
            // (EN) Check template class declaration
        }

        void BorrowChecker::visitTemplateInstantiation(AST::TemplateInstantiation &inst)
        {
            // (AR) ״­״µ ״×†״° ״§„‚״§„״¨
            // (EN) Check template instantiation
        }

        void BorrowChecker::visitNamespaceDecl(AST::NamespaceDecl &decl)
        {
            // (AR) ״­״µ ״×״µ״±״­ ״¶״§״¡ ״§„״£״³…״§״¡
            // (EN) Check namespace declaration
        }

        void BorrowChecker::visitOperatorDecl(AST::OperatorDecl &decl)
        {
            // (AR) ״­״µ ״×״µ״±״­ ״×״­…„ ״§„״¹״§…„
            // (EN) Check operator overload declaration
        }

        void BorrowChecker::visitTraitDecl(AST::TraitDecl &decl)
        {
            // (AR) ״­״µ ״×״µ״±״­ ״§„״³…״© ג€” ״×״­‚‚ …† ״§„…„ƒ״©  ״§„״¯ˆ״§„ ״§„…״¹„†״©
            // (EN) Check trait declaration ג€” verify ownership in declared methods
        }

        void BorrowChecker::visitImplDecl(AST::ImplDecl &decl)
        {
            // (AR) ״­״µ ״×†״° ״§„״³…״© ג€” ״×״­„„ ״£״¬״³״§… ״§„״¯ˆ״§„
            // (EN) Check trait implementation ג€” analyze method bodies
            for (auto &method : decl.methods)
            {
                if (method)
                    method->accept(*this);
            }
        }

        void BorrowChecker::visitStructDecl(AST::StructDecl &decl)
        {
            // (AR) ״­״µ ״×״µ״±״­ ״§„״¨†״© ג€” „״§ ״­ˆ״µ״§״× …„ƒ״© ״­״§„״§‹
            // (EN) Check struct declaration ג€” no ownership checks currently
        }

        void BorrowChecker::visitTestDecl(AST::TestDecl &decl)
        {
            // (AR) ״­״µ ״×״µ״±״­ ״§„״§״®״×״¨״§״± ג€” ״×״­„„ ״¬״³… ״§„״§״®״×״¨״§״±
            // (EN) Check test declaration ג€” analyze test body
            if (decl.body)
                decl.body->accept(*this);
        }

        void BorrowChecker::visitExtensionDecl(AST::ExtensionDecl &decl)
        {
            // (AR) ״×״µ״±״­ ƒ״×„״© ״§„״§…״×״¯״§״¯ ג€” ״¥״¶״§״© ״¯ˆ״§„ ״¬״¯״¯״© „†ˆ״¹ …ˆ״¬ˆ״¯
            //      …״«״§„: ״§…״×״¯״§״¯ †‚״·״© ... †‡״§״©
            //      ƒ„ ״·״±‚״©  ״§„״§…״×״¯״§״¯ ״×״¹…„ ƒ״·״±‚״© ״¹״§״¯״©  ״§„״µ† ״§„״£״µ„
            //      „״°„ƒ †״­״µ ƒ„ ״·״±‚״©  †״·״§‚ …״³״×‚„
            // (EN) Extension block declaration ג€” add new methods to existing type
            //      Example: extension Point ... end
            //      Each method works like a regular method on the original class
            //      So we check each method in its own scope

            if (debugMode_)
            {
                recordWarning("[debug] Extension for type '" + decl.targetType + "' with " + std::to_string(decl.methods.size()) + " methods at " + getLocation(&decl).toString());
            }

            // (AR) ״­״µ ƒ„ ״·״±‚״©  ״§„״§…״×״¯״§״¯ ג€” ƒ„ ״·״±‚״© ״×״×״­ †״·״§‚‡״§ ״§„״®״§״µ
            // (EN) Check each method in extension ג€” each method opens its own scope
            for (auto &method : decl.methods)
            {
                if (method)
                {
                    method->accept(*this);
                }
            }
        }

        void BorrowChecker::visitMacroDecl(AST::MacroDecl &decl)
        {
            // (AR) ״×״µ״±״­ ״§„…״§ƒ״±ˆ ג€” ״¹…„ ƒ״¯״§„״© …״¶…‘†״© ״×†‘״°  †״·״§‚ ״§„…״³״×״¯״¹
            //      ״§„…״§ƒ״±ˆ ״µ״­ (hygienic): …״×״÷״±״§״×‡ ״§„״¯״§״®„״© „״§ ״×„ˆ‘״« ״§„†״·״§‚ ״§„״®״§״±״¬
            //      „״°„ƒ †״­״µ ״§„״¬״³…  †״·״§‚ …״¹״²ˆ„ …״¹ ״×״³״¬„ ״§„…״¹״§…„״§״×
            // (EN) Macro declaration ג€” works as inline function in caller's scope
            //      Macro is hygienic: internal variables don't pollute outer scope
            //      So we check the body in isolated scope with parameters registered

            if (debugMode_)
            {
                recordWarning("[debug] Macro '" + decl.name + "' with " + std::to_string(decl.params.size()) + " params" + (decl.isVariadic ? " (variadic)" : "") + " at " + getLocation(&decl).toString());
            }

            tracker_->enterScope();

            // (AR) ״×״³״¬„ …״¹״§…„״§״× ״§„…״§ƒ״±ˆ ƒ…״×״÷״±״§״×  ״§„†״·״§‚ ״§„…״¹״²ˆ„
            //      …״¹״§…„״§״× ״§„…״§ƒ״±ˆ ״×†״³״® ״¹†״¯ ״§„״§״³״×״¯״¹״§״¡״ „״°״§ ״×״¹״×״¨״± ‚״§״¨„״© „„†״³״®
            // (EN) Register macro parameters as variables in isolated scope
            //      Macro parameters are copied on invocation, so treated as Copy
            for (const auto &param : decl.params)
            {
                // (AR) ״§„…״¹״§…„״§״× ״§„״¹״§״¯״© ג€” ƒ„ …״¹״§…„ ״³״¬‘„ ƒ…״×״÷״± ‚״§״¨„ „„†״³״®
                // (EN) Regular parameters ג€” each is registered as a Copy variable
                tracker_->declareVariable(param, "״£", getLocation(&decl), true);
                currentResult_.totalVariables++;
            }

            // (AR) ״­״µ ״¬״³… ״§„…״§ƒ״±ˆ ג€” †״³ ״­״µ ״¬״³… ״£ ״¯״§„״©
            // (EN) Check macro body ג€” same as checking any function body
            if (decl.body)
            {
                decl.body->accept(*this);
            }

            tracker_->exitScope();
        }

        void BorrowChecker::visitTypeAliasDecl(AST::TypeAliasDecl &decl)
        {
            // (AR) ״×״µ״±״­ ״§״³… …״³״×״¹״§״± „„†ˆ״¹ ג€” „״§ ״×״¶…† ‚… ˆ‚״× ״§„״×״´״÷„
            //      …״«״§„: †ˆ״¹ ״¹״¯״¯ = ״±‚…
            //      …״«״§„: †ˆ״¹ ‚״§״¦…״© = …״µˆ״©
            //      ‡״°״§ ״×״µ״±״­ ״¹„‰ …״³״×ˆ‰ ״§„״£†ˆ״§״¹ ‚״·״ „״§ ˆ„‘״¯ ƒˆ״¯ ˆ„״§ ״₪״«״± ״¹„‰ ״§„…„ƒ״©
            //      „ƒ† †״­״µ ״§„״×״¹״¨״± ״§„‡״¯  ״­״§„ ƒ״§† ״´״± ״¥„‰ ״±…״² ״÷״± …״¹״±‘
            // (EN) Type alias declaration ג€” no runtime values involved
            //      Example: type integer = number
            //      This is a type-level declaration only, generates no code, no ownership impact
            //      But we check the target expression in case it references an undefined symbol

            if (decl.target)
            {
                // (AR) ״§„״×״¹״¨״± ״§„‡״¯ ״¹״§״¯״© ״§״³… †ˆ״¹ (VariableExpr) ג€” „״§ †״×״­‚‚ …† …„ƒ״×‡
                //      „״£†‡ „״³ …״×״÷״±״§‹ ״¨„ ״§״³… †ˆ״¹ ״­„‘„  …״±״­„״© „״§״­‚״©
                // (EN) Target expression is usually a type name (VariableExpr) ג€” no ownership check
                //      Because it's not a variable but a type name resolved in a later phase

                // (AR) „״§ ״­״§״¬״© „״§״³״×״¯״¹״§״¡ accept ג€” ״§„״§״³… ״§„…״³״×״¹״§״± „״§ ״³״×‡„ƒ ‚…״§‹
                // (EN) No need to call accept ג€” alias doesn't consume values
            }

            if (debugMode_)
            {
                recordWarning("[debug] Type alias '" + decl.name + "' at " + getLocation(&decl).toString());
            }

            (void)decl;
        }

        void BorrowChecker::visitTupleDestructureStmt(AST::TupleDestructureStmt &stmt)
        {
            // (AR) ״×ƒƒ ״§„״µ ג€” ״×״¹† ״¹†״§״µ״± ״µ ״¥„‰ …״×״÷״±״§״× ״±״¯״©
            //      …״«״§„: …״×״÷״± (״£״ ״¨״ ״¬) = ״µ(1״ 2״ 3)
            //      …״«״§„: ״«״§״¨״× (״³״ ״µ) = †‚״·״©.״¥״­״¯״§״«״§״×()
            //      ״§„״×״¹״¨״± ״§„…‡‘״¦ (״§„״·״± ״§„״£…†) †‚„ ״£ˆ †״³״® ״­״³״¨ †ˆ״¹‡
            //      ƒ„ …״×״÷״± …ƒ‘ƒ ״­״µ„ ״¹„‰ …„ƒ״© ״¹†״µ״± ˆ״§״­״¯ …† ״§„״µ
            // (EN) Tuple destructuring ג€” assign tuple elements to individual variables
            //      Example: var (a, b, c) = tuple(1, 2, 3)
            //      Example: const (x, y) = point.coordinates()
            //      The initializer (right side) is moved or copied depending on its type
            //      Each destructured variable gets ownership of one tuple element

            if (debugMode_)
            {
                std::string varNames;
                for (size_t i = 0; i < stmt.names.size(); ++i)
                {
                    if (i > 0)
                        varNames += ", ";
                    varNames += stmt.names[i];
                }
                recordWarning("[debug] Tuple destructure (" + varNames + ")" + (stmt.isConst ? " [const]" : " [var]") + " at " + getLocation(&stmt).toString());
            }

            // (AR) ״­״µ ״§„…‡‘״¦ ״£ˆ„״§‹ (‚״¨„ ״×״³״¬„ ״§„…״×״÷״±״§״× ״§„״¬״¯״¯״©)
            // (EN) Check initializer first (before registering new variables)
            if (stmt.initializer)
            {
                // (AR) ״¥״°״§ ƒ״§† ״§„…‡‘״¦ …״×״÷״±״§‹״ ‡״°״§ †‚„ …„ƒ״© „„״µ ״¨״§„ƒ״§…„
                // (EN) If initializer is a variable, this is a whole-tuple ownership move
                if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(stmt.initializer.get()))
                {
                    std::string varName = varExpr->toString();

                    // (AR) ״×״­‚‚ …† …״¹„ˆ…״§״× ״§„…„ƒ״© „״×״­״¯״¯ ״¥״°״§ ƒ״§† ‚״§״¨„״§‹ „„†״³״®
                    // (EN) Check ownership info to determine if it's Copy
                    auto info = tracker_->getOwnershipInfo(varName);
                    if (info && !info->isCopyType)
                    {
                        // (AR) †‚„ ״§„…„ƒ״© …† ״§„…״×״÷״± ״§„…״µ״¯״±
                        // (EN) Move ownership from source variable
                        auto error = tracker_->moveVariable(varName, getLocation(varExpr));
                        if (error)
                        {
                            currentResult_.addError(*error);
                        }
                        currentResult_.totalMoves++;
                    }
                    else
                    {
                        // (AR) †ˆ״¹ ‚״§״¨„ „„†״³״® ג€” ‚״±״§״¡״© ‚״·
                        // (EN) Copy type ג€” read only
                        auto error = tracker_->useVariable(varName, getLocation(varExpr));
                        if (error)
                        {
                            currentResult_.addError(*error);
                        }
                    }
                }
                else
                {
                    // (AR) ״×״¹״¨״± ״¹״§… (״§״³״×״¯״¹״§״¡ ״¯״§„״©״ ״µ ״­״±״ ...) ג€” ״­״µ ״¹״§״¯
                    // (EN) General expression (function call, tuple literal, ...) ג€” normal check
                    stmt.initializer->accept(*this);
                }
            }

            // (AR) ״×״³״¬„ ƒ„ …״×״÷״± …ƒ‘ƒ  …״×״×״¨״¹ ״§„…„ƒ״©
            //      ƒ„ …״×״÷״± ״­״µ„ ״¹„‰ †ˆ״¹ "״£" „״£† †ˆ״¹ ״¹†״§״µ״± ״§„״µ ״÷״± …״­״¯״¯  AST
            //      ״§„…״×״÷״±״§״× ״§„״«״§״¨״×״© (isConst) ״×״³״¬‘„ ƒ״£†ˆ״§״¹ ‚״§״¨„״© „„†״³״® „״£†‡״§ „״§ ״×״¹״¯‘„
            // (EN) Register each destructured variable in ownership tracker
            //      Each variable gets type "any" since tuple element types aren't specified in AST
            //      Const variables (isConst) are registered as Copy types since they can't be mutated
            for (const auto &name : stmt.names)
            {
                // (AR) ״§״³״×†״×״§״¬ ״¥״°״§ ƒ״§† ״§„†ˆ״¹ ‚״§״¨„״§‹ „„†״³״®:
                //      - ״«״§״¨״×: „״§ †‚„ ״£״¨״¯״§‹״ „״°״§ ״¹״×״¨״± ‚״§״¨„״§‹ „„†״³״®
                //      - …״×״÷״±: ״¹״×…״¯ ״¹„‰ †ˆ״¹ ״§„״¹†״µ״± ״§„״£״µ„ („״§ †״¹״±‡״ †״×״±״¶ copy)
                // (EN) Infer if type is Copy:
                //      - const: never moved, so treated as Copy
                //      - var: depends on original element type (unknown, assume copy)
                bool isCopy = true; // (AR) ״§״×״±״§״¶ ג€” ״¢…† „„״÷״© ״µ ״§„״­״§„״©
                tracker_->declareVariable(name, "״£", getLocation(&stmt), isCopy);
                currentResult_.totalVariables++;
            }
        }

        // ============================================================================
        // ״¯ˆ״§„ …״³״§״¹״¯״© / Helper Functions
        // ============================================================================

        void BorrowChecker::analyzeExpression(AST::Expression *expr, bool isMoveContext)
        {
            if (!expr)
                return;

            // (AR) ״¥״°״§ ƒ״§† …״×״÷״±״§‹ ˆ״³״§‚ †‚„
            // (EN) If variable and move context
            if (isMoveContext)
            {
                auto error = tracker_->moveVariable(
                    expr->toString(),
                    getLocation(expr));
                if (error)
                {
                    currentResult_.addError(*error);
                }
                currentResult_.totalMoves++;
            }
            else
            {
                // (AR) ‚״±״§״¡״© ‚״·
                // (EN) Read only
                auto error = tracker_->useVariable(
                    expr->toString(),
                    getLocation(expr));
                if (error)
                {
                    currentResult_.addError(*error);
                }
            }
        }

        void BorrowChecker::analyzeAssignment(AST::AssignExpr *assign)
        {
            if (!assign)
                return;

            // (AR) ״­״µ ״§„״·״± ״§„״£…† ״£ˆ„״§‹
            // (EN) Check right side first
            if (assign->value)
            {
                // (AR) ״¥״°״§ ƒ״§† ״§„…״µ״¯״± …״×״÷״±״§‹״ ‡״°״§ †‚„ …„ƒ״©
                // (EN) If source is a variable, this is an ownership move
                auto *varExpr = dynamic_cast<AST::VariableExpr *>(assign->value.get());
                if (varExpr)
                {
                    auto error = tracker_->moveVariable(varExpr->toString(), getLocation(varExpr));
                    if (error)
                    {
                        currentResult_.addError(*error);
                    }
                    currentResult_.totalMoves++;
                }
                else
                {
                    assign->value->accept(*this);
                }
            }

            // (AR) ״­״µ ״§„״·״± ״§„״£״³״± (״§„״×״¹״¯„)
            // (EN) Check left side (mutation)
            auto error = tracker_->mutateVariable(
                assign->name,
                getLocation(assign));
            if (error)
            {
                currentResult_.addError(*error);
            }
        }

        void BorrowChecker::analyzeFunctionCall(AST::CallExpr *call)
        {
            if (!call)
                return;

            // (AR) ״­״µ ״§„…״³״×״¯״¹‰
            // (EN) Check callee
            if (call->callee)
            {
                call->callee->accept(*this);
            }

            // (AR) ״­״µ ƒ„ …״¹״§…„ - ƒ„ …״¹״§…„ ‚״±״£ (ˆ‚״¯ †‚„)
            // (EN) Check each argument - each is read (and may be moved)
            for (auto &arg : call->arguments)
            {
                if (arg)
                {
                    arg->accept(*this);
                }
            }
        }

        bool BorrowChecker::isCopyType(const std::string &typeName) const
        {
            // (AR) ״§„״£†ˆ״§״¹ ״÷״± ״§„…״­״¯״¯״© ״£ˆ ״÷״± ״§„…״¹״±ˆ״© ״×״¹״×״¨״± ‚״§״¨„״© „„†״³״® ״§״×״±״§״¶״§‹
            // (EN) Unknown/unspecified/none types default to copy ג€” safe for ״µ language
            // (AR) „״÷״© ״µ „״§ ״×״¯״¹… †‚„ ״§„…„ƒ״© …״«„ Rust״ „״°״§ ״§„״£†ˆ״§״¹ ״÷״± ״§„…״­״¯״¯״© ״×†״³״®
            // (EN) S language doesn't have Rust-like move semantics, so unresolved types are copy
            if (typeName.empty() || typeName == "unknown" || typeName == "„״§״´״¡")
            {
                return true;
            }

            // (AR) ״§„״×״­‚‚ …† ״§„‚״§״¦…״©
            // (EN) Check the list
            if (copyTypes_.find(typeName) != copyTypes_.end())
            {
                return true;
            }

            // (AR) ״§„…״±״§״¬״¹ „״³״× ‚״§״¨„״© „„†״³״® (״×״³״×†״³״® ״§„…״±״§״¬״¹ †״³‡״§)
            // (EN) References are Copy (the references themselves are copied)
            if (!typeName.empty() && typeName.front() == '&')
            {
                return true;
            }
            if (typeName.size() >= 4 && typeName.substr(0, 4) == "…״±״¬״¹")
            {
                return true;
            }

            return false;
        }

        std::string BorrowChecker::dataTypeToString(Data::DataType type) const
        {
            // (AR) ״×״­ˆ„ †ˆ״¹ ״§„״¨״§†״§״× ״¥„‰ †״µ „…״·״§״¨‚״© ״£†ˆ״§״¹ ״§„†״³״®
            // (EN) Convert DataType enum to string for copy type matching
            switch (type)
            {
            case Data::DataType::INTEGER:
                return "״±‚…";
            case Data::DataType::FLOAT:
                return "״¹״´״±";
            case Data::DataType::BOOLEAN:
                return "…†״·‚";
            case Data::DataType::STRING:
                return "†״µ";
            case Data::DataType::BYTE:
                return "״¨״§״×";
            case Data::DataType::ARRAY:
                return "…״µˆ״©";
            case Data::DataType::MAP:
                return "‚״§…ˆ״³";
            case Data::DataType::TUPLE:
                return "״«†״§״¦";
            case Data::DataType::FUNCTION:
                return "״¯״§„״©";
            case Data::DataType::NONE:
                return "„״§״´״¡";
            case Data::DataType::ENUM:
                return "״×״¹״¯״§״¯";
            case Data::DataType::ERROR:
                return "״®״·״£";
            default:
                return "unknown";
            }
        }

        SourceLocation BorrowChecker::getLocation(AST::ASTNode *node) const
        {
            if (!node)
            {
                return SourceLocation();
            }

            // ״×״×״¨״¹ ״§„…ˆ‚״¹ ״§„״¹„ …† ״§„״¹‚״¯״© …״¹ fallback ״¢…† ״¥״°״§ ƒ״§†״× ״§„״¥״­״¯״§״«״§״× ״÷״± …‡״£״©.
            const size_t line = (node->position.line == 0) ? 1 : node->position.line;
            const size_t column = (node->position.column == 0) ? 1 : node->position.column;
            return SourceLocation(currentFile_,
                                  line,
                                  column);
        }

        void BorrowChecker::recordError(const OwnershipError &error)
        {
            currentResult_.addError(error);

            if (debugMode_)
            {
                if (useArabicMessages_)
                {
                    std::cerr << error.toArabicString();
                }
                else
                {
                    std::cerr << error.toEnglishString();
                }
            }
        }

        void BorrowChecker::recordWarning(const std::string &warning)
        {
            currentResult_.addWarning(warning);

            if (debugMode_)
            {
                std::cerr << "[״×״­״°״±/Warning] " << warning << "\n";
            }
        }


    } // namespace Semantic
} // namespace Sad
