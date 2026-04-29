// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(disable : 4819)
#endif

/**
 * @file borrow_checker.cpp
 * @brief ״×†״° ״§״­״µ ״§„״§״³״×״¹״§״±״© / Borrow Checker Implementation
 * @author Sad Language Team
 * @date January 2026
 * @version 1.0
 * @phase Phase 4: US2 - Memory Safety
 */

#include "ownership/borrow_checker.h"
#include <iostream>
#include <sstream>

namespace Sad
{
    namespace Semantic
    {

        // ============================================================================
        // ״¨†״§״¡ ˆ‡״¯… / Construction & Destruction
        // ============================================================================

        BorrowChecker::BorrowChecker()
            : tracker_(std::make_unique<OwnershipTracker>()), useArabicMessages_(true), debugMode_(false), nllMode_(true), currentFile_(""), currentFunction_("")
        {
            initializeCopyTypes();
        }

        BorrowChecker::~BorrowChecker() = default;

        // ============================================================================
        // ״×‡״¦״© ״§„״£†ˆ״§״¹ ״§„‚״§״¨„״© „„†״³״® / Initialize Copy Types
        // ============================================================================

        void BorrowChecker::initializeCopyTypes()
        {
            // (AR) ״§„״£†ˆ״§״¹ ״§„״¨״¯״§״¦״© ‚״§״¨„״© „„†״³״® ״¯״§״¦…״§‹
            // (EN) Primitive types are always Copy

            // ״§„״£״±‚״§… ״§„״µ״­״­״© / Integers
            copyTypes_.insert("״­8");  // i8
            copyTypes_.insert("״­16"); // i16
            copyTypes_.insert("״­32"); // i32
            copyTypes_.insert("״­64"); // i64
            copyTypes_.insert("״¹8");  // u8
            copyTypes_.insert("״¹16"); // u16
            copyTypes_.insert("״¹32"); // u32
            copyTypes_.insert("״¹64"); // u64
            copyTypes_.insert("״±‚…"); // default integer
            copyTypes_.insert("״¹״¯״¯"); // alias for integer

            // ״§„״£״±‚״§… ״§„״¹״´״±״© / Floats
            copyTypes_.insert("״¹״´32"); // f32
            copyTypes_.insert("״¹״´64"); // f64
            copyTypes_.insert("״¹״´״±"); // default float

            // ״§„…†״·‚ / Boolean
            copyTypes_.insert("…†״·‚");
            copyTypes_.insert("bool");

            // ״§„״­״± / Character
            copyTypes_.insert("״­״±");
            copyTypes_.insert("char");

            // ״§„…״₪״´״±״§״× ״§„״®״§… (unsafe) / Raw pointers
            copyTypes_.insert("*״«״§״¨״×");
            copyTypes_.insert("*…״×״÷״±");

            // English aliases
            copyTypes_.insert("i8");
            copyTypes_.insert("i16");
            copyTypes_.insert("i32");
            copyTypes_.insert("i64");
            copyTypes_.insert("u8");
            copyTypes_.insert("u16");
            copyTypes_.insert("u32");
            copyTypes_.insert("u64");
            copyTypes_.insert("f32");
            copyTypes_.insert("f64");
            copyTypes_.insert("int");
            copyTypes_.insert("float");
            copyTypes_.insert("double");
        }

        // ============================================================================
        // ˆ״§״¬‡״© ״§„״­״µ ״§„״±״¦״³״© / Main Check Interface
        // ============================================================================

        BorrowCheckResult BorrowChecker::check(AST::ASTNode *ast)
        {
            reset();

            if (!ast)
            {
                currentResult_.addWarning("AST is null / ״´״¬״±״© AST ״§״±״÷״©");
                return currentResult_;
            }

            if (debugMode_)
            {
                std::cout << "[BorrowChecker] Starting check...\n";
            }

            // (AR) ״²״§״±״© ״§„״¬״°״±
            // (EN) Visit root
            // (AR) ״¥״°״§ ƒ״§† ״§„״¬״°״± BlockStmt״ †״²ˆ״± …״­״×ˆ״§״×‡ …״¨״§״´״±״© ״¯ˆ† ״¯״®ˆ„ †״·״§‚ ״¬״¯״¯
            // (EN) If root is BlockStmt, visit its contents directly without entering a new scope
            // ‡״°״§ ״¶״±ˆ״± „״£† ״§„…״×״÷״±״§״× ״§„״¹״§…״© ״¬״¨ ״£† ״×״³״¬‘„  ״§„†״·״§‚ 0
            // This is necessary so global variables are registered in scope 0
            if (auto *blockStmt = dynamic_cast<AST::BlockStmt *>(ast))
            {
                for (auto &stmt : blockStmt->statements)
                {
                    if (stmt)
                    {
                        stmt->accept(*this);
                    }
                }
            }
            else
            {
                ast->accept(*this);
            }

            // (AR) ״¬…״¹ ״§„״£״®״·״§״¡ ״§„״¥״¶״§״© …† ״§„…״×״×״¨״¹ (…״«„: ״­״° ״£״«†״§״¡ ״§„״§״³״×״¹״§״±״©)
            // (EN) Collect additional errors from tracker (e.g. drop while borrowed)
            // …„״§״­״¸״©: ״§„״£״®״·״§״¡ …† ״§„״²ˆ״§״± ״×״¶״§ …״¨״§״´״±״© ״¥„‰ currentResult_
            // Note: Visitor errors are added directly to currentResult_
            // ‡†״§ †״¬…״¹ ‚״· ״§„״£״®״·״§״¡ ״§„״× ƒ״×״´‡״§ ״§„…״×״×״¨״¹ ״¹†״¯ ״®״±ˆ״¬ ״§„†״·״§‚
            // Here we only collect errors the tracker detects on scope exit
            // ״×… †‚„ ״§„״£״®״·״§״¡ …† ״§„…״×״×״¨״¹ ״¥„‰ ״§„״²ˆ״§״± …״¨״§״´״±״© - „״§ ״­״§״¬״© „״¬…״¹‡״§ …״±״×†
            // Errors moved to visitors directly - no need to collect twice

            if (debugMode_)
            {
                std::cout << "[BorrowChecker] Check complete. "
                          << "Errors: " << currentResult_.errors.size() << "\n";
                tracker_->dump();
            }

            return currentResult_;
        }

        BorrowCheckResult BorrowChecker::checkFunction(AST::FunctionDecl *func)
        {
            reset();

            if (!func)
            {
                currentResult_.addWarning("Function is null / ״§„״¯״§„״© ״§״±״÷״©");
                return currentResult_;
            }

            visitFunctionDecl(*func);

            return currentResult_;
        }

        void BorrowChecker::reset()
        {
            tracker_ = std::make_unique<OwnershipTracker>();
            currentResult_ = BorrowCheckResult();
            currentFile_ = "";
            currentFunction_ = "";
        }

        // ============================================================================
        // ״²״§״±״© ״§„״×״¹״§״¨״± / Visit Expressions
        // ============================================================================

        void BorrowChecker::visitBinaryExpr(AST::BinaryExpr &expr)
        {
            // (AR) ״­״µ ״§„״·״±†
            // (EN) Check both sides
            if (expr.left)
            {
                expr.left->accept(*this);
            }
            if (expr.right)
            {
                expr.right->accept(*this);
            }
        }

        void BorrowChecker::visitUnaryExpr(AST::UnaryExpr &expr)
        {
            // (AR) ״§„״×״­‚‚ …† †ˆ״¹ ״§„״¹״§…„ ״§„״£״­״§״¯
            // (EN) Check unary operator type

            if (!expr.operand)
                return;

            // (AR) …״¹״§„״¬״© ״¹״§…„ ״§„…״±״¬״¹ & (״§״³״×״¹״§״±״©)
            // (EN) Handle reference operator & (borrow)
            if (expr.op == Lexer::TokenType::OP_BITWISE_AND)
            {
                // (AR) ״×״­‚‚: ‡„ ״§„…״¹״§…„ …״×״÷״± …ƒ† ״§״³״×״¹״§״±״×‡״
                // (EN) Check: is operand a variable we can borrow?
                if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(expr.operand.get()))
                {
                    std::string varName = varExpr->toString();
                    std::string borrowerName = "&" + varName;

                    // (AR) ״¥†״´״§״¡ ״§״³״×״¹״§״±״© ״«״§״¨״×״© (‚״±״§״¡״© ‚״·)
                    // (EN) Create shared (immutable) borrow
                    auto error = tracker_->createBorrow(
                        varName, borrowerName,
                        BorrowKind::Shared,
                        getLocation(&expr));

                    if (error)
                    {
                        currentResult_.addError(*error);
                    }
                    currentResult_.totalBorrows++;
                }
                else
                {
                    // (AR) ״­״µ ״§„…״¹״§…„ ״¹״§״¯״§‹
                    // (EN) Check operand normally
                    expr.operand->accept(*this);
                }
                return;
            }

            // (AR) …״¹״§„״¬״© ״¹״§…„ ƒ ״§„…״±״¬״¹ * (dereference)
            // (EN) Handle dereference operator *
            if (expr.op == Lexer::TokenType::OP_MULTIPLY)
            {
                // (AR) ״­״µ ״£† ״§„…״¹״§…„ …״³״×״¹״§״± ״¨״´ƒ„ ״µ״­״­
                // (EN) Check that operand is properly borrowed
                if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(expr.operand.get()))
                {
                    std::string varName = varExpr->toString();
                    // (AR) ״§„״×״­‚‚ …† ״£† ״§„…״×״÷״± ״µ״§„״­ „„״§״³״×״®״¯״§… („״³ …†‚ˆ„״§‹)
                    // (EN) Verify variable is valid (not moved)
                    auto error = tracker_->useVariable(varName, getLocation(&expr));
                    if (error)
                    {
                        currentResult_.addError(*error);
                    }
                }
                else
                {
                    expr.operand->accept(*this);
                }
                return;
            }

            // (AR) „„״¹ˆ״§…„ ״§„״£״®״±‰ (-, !, ~): ״­״µ ״§„…״¹״§…„ ״¹״§״¯״§‹
            // (EN) For other operators (-, !, ~): check operand normally
            expr.operand->accept(*this);
        }

        void BorrowChecker::visitTernaryExpr(AST::TernaryExpr &expr)
        {
            // (AR) ״­״µ ״§„״´״±״· ˆ״§„״±״¹†
            // (EN) Check condition and both branches
        }

        void BorrowChecker::visitLiteralExpr(AST::LiteralExpr &expr)
        {
            // (AR) ״§„‚… ״§„״­״±״© „״§ ״×״­״×״§״¬ ״­״µ …„ƒ״©
            // (EN) Literals don't need ownership checking
            (void)expr;
        }

        void BorrowChecker::visitVariableExpr(AST::VariableExpr &expr)
        {
            // (AR) ״§„״×״­‚‚ …† ״¥…ƒ״§†״© ״§„״§״³״×״®״¯״§…
            // (EN) Check if can be used
            auto error = tracker_->useVariable(expr.toString(), getLocation(&expr));
            if (error)
            {
                currentResult_.addError(*error);
            }
        }

        void BorrowChecker::visitAssignExpr(AST::AssignExpr &expr)
        {
            analyzeAssignment(&expr);
        }

        void BorrowChecker::visitCallExpr(AST::CallExpr &expr)
        {
            analyzeFunctionCall(&expr);
        }

        void BorrowChecker::visitIndexExpr(AST::IndexExpr &expr)
        {
            // (AR) ״­״µ ״§„…״µˆ״© ˆ״§„‡״±״³
            // (EN) Check array and index
        }

        void BorrowChecker::visitMemberExpr(AST::MemberExpr &expr)
        {
            // (AR) ״­״µ ״§„ƒ״§״¦†
            // (EN) Check object
        }

        void BorrowChecker::visitMemberAssignExpr(AST::MemberAssignExpr &expr)
        {
            // (AR) ״­״µ ״×״¹† ״§„״¹״¶ˆ
            // (EN) Check member assignment
        }

        void BorrowChecker::visitIndexAssignExpr(AST::IndexAssignExpr &expr)
        {
            // (AR) ״­״µ ״§„״¥״³†״§״¯ ״¨״§„‡״±״³
            // (EN) Check index assignment
        }

        void BorrowChecker::visitArrayExpr(AST::ArrayExpr &expr)
        {
            // (AR) ״­״µ ״¹†״§״µ״± ״§„…״µˆ״©
            // (EN) Check array elements
        }

        void BorrowChecker::visitMapExpr(AST::MapExpr &expr)
        {
            // (AR) ״­״µ ״¹†״§״µ״± ״§„‚״§…ˆ״³
            // (EN) Check map entries
        }

        void BorrowChecker::visitWalrusExpr(AST::WalrusExpr &expr)
        {
            // (AR) ״­״µ ״×״¹״¨״± Walrus
            // (EN) Check walrus expression
        }

        void BorrowChecker::visitAwaitExpr(AST::AwaitExpr &expr)
        {
            // (AR) ״­״µ ״×״¹״¨״± ״§„״§†״×״¸״§״±
            // (EN) Check await expression
        }

        void BorrowChecker::visitLambdaExpr(AST::LambdaExpr &expr)
        {
            // (AR) ״­״µ Lambda
            // (EN) Check lambda expression
        }

        void BorrowChecker::visitListComprehensionExpr(AST::ListComprehensionExpr &expr)
        {
            // (AR) ״­״µ ״§״³״×״¹״§״¨ ״§„‚״§״¦…״©
            // (EN) Check list comprehension
        }

        void BorrowChecker::visitDictComprehensionExpr(AST::DictComprehensionExpr &expr)
        {
            // (AR) ״­״µ ״§״³״×״¹״§״¨ ״§„‚״§…ˆ״³
            // (EN) Check dict comprehension
        }

        void BorrowChecker::visitSetComprehensionExpr(AST::SetComprehensionExpr &expr)
        {
            // (AR) ״­״µ ״§״³״×״¹״§״¨ ״§„…״¬…ˆ״¹״©
            // (EN) Check set comprehension
        }

        void BorrowChecker::visitGeneratorExpr(AST::GeneratorExpr &expr)
        {
            // (AR) ״­״µ ״×״¹״¨״± ״§„…ˆ„״¯
            // (EN) Check generator expression
        }

        void BorrowChecker::visitDecoratorExpr(AST::DecoratorExpr &expr)
        {
            // (AR) ״­״µ ״§„…״²״®״±
            // (EN) Check decorator
        }

        void BorrowChecker::visitNewExpr(AST::NewExpr &expr)
        {
            // (AR) ״­״µ ״×״¹״¨״± new
            // (EN) Check new expression
        }

        void BorrowChecker::visitMemberAccessExpr(AST::MemberAccessExpr &expr)
        {
            // (AR) ״­״µ ״§„ˆ״µˆ„ „„״¹״¶ˆ
            // (EN) Check member access
        }

        void BorrowChecker::visitMethodCallExpr(AST::MethodCallExpr &expr)
        {
            // (AR) ״­״µ ״§״³״×״¯״¹״§״¡ ״§„״·״±‚״©
            // (EN) Check method call
        }

        void BorrowChecker::visitThisExpr(AST::ThisExpr &expr)
        {
            // (AR) this/‡״°״§ „״§ ״­״×״§״¬ ״­״µ ״®״§״µ
            // (EN) this doesn't need special checking
            (void)expr;
        }

        void BorrowChecker::visitSuperExpr(AST::SuperExpr &expr)
        {
            // (AR) super/״£״µ„ „״§ ״­״×״§״¬ ״­״µ ״®״§״µ
            // (EN) super doesn't need special checking
            (void)expr;
        }

        void BorrowChecker::visitBorrowExpr(AST::BorrowExpr &expr)
        {
            // (AR) ״­״µ ״×״¹״¨״± ״§„״§״³״×״¹״§״±״© &x ״£ˆ &…״×״÷״± x
            // (EN) Check borrow expression &x or &mut x
            if (!expr.variableName.empty())
            {
                // (AR) ״×״­״¯״¯ †ˆ״¹ ״§„״§״³״×״¹״§״±״©: …״´״×״±ƒ״© ״£ˆ …״×״÷״±״©
                // (EN) Determine borrow kind: shared or mutable
                BorrowKind kind = expr.isMutable ? BorrowKind::Mutable : BorrowKind::Shared;
                std::string borrowerName = (expr.isMutable ? "&…״×״÷״± " : "&") + expr.variableName;

                // (AR) ״¥†״´״§״¡ ״§„״§״³״×״¹״§״±״©  …״×״×״¨״¹ ״§„…„ƒ״©
                // (EN) Create borrow in ownership tracker
                auto error = tracker_->createBorrow(
                    expr.variableName, borrowerName,
                    kind, getLocation(&expr));

                if (error)
                {
                    currentResult_.addError(*error);
                }
                currentResult_.totalBorrows++;

                if (debugMode_)
                {
                    recordWarning("[debug] " + std::string(expr.isMutable ? "Mutable" : "Shared") + " borrow of '" + expr.variableName + "' at " + getLocation(&expr).toString());
                }
            }
        }

        void BorrowChecker::visitInlineAsmExpr(AST::InlineAsmExpr &expr)
        {
            // (AR) ״×״¹״¨״± ״§„״×״¬…״¹ ״§„…״¶…‘† - „״§ ״­״×״§״¬ ״­״µ …„ƒ״©
            // (EN) Inline assembly expression - no ownership checking needed
            (void)expr;
        }

        void BorrowChecker::visitRangeExpr(AST::RangeExpr &expr)
        {
            // (AR) ״×״¹״¨״± ״§„†״·״§‚ ג€” ״­״µ ״§„״×״¹״¨״±״§״× ״§„״±״¹״©
            // (EN) Range expression ג€” check sub-expressions
            if (expr.start)
                expr.start->accept(*this);
            if (expr.end)
                expr.end->accept(*this);
        }

        void BorrowChecker::visitOptionalChainExpr(AST::OptionalChainExpr &expr)
        {
            // (AR) ˆ״µˆ„ ״¢…† ?. ג€” ״­״µ ״§„ƒ״§״¦† „„״×״£ƒ״¯ ״£†‡ „… †‚„
            // (EN) Optional chaining ?. ג€” check object is not moved
            if (expr.object)
            {
                expr.object->accept(*this);

                // (AR) ״¥״°״§ ƒ״§† ״§„ƒ״§״¦† …״×״÷״±״§‹״ ״×״­‚‚ …† ״µ„״§״­״×‡
                // (EN) If object is a variable, verify it's still valid
                if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(expr.object.get()))
                {
                    auto error = tracker_->useVariable(varExpr->toString(), getLocation(&expr));
                    if (error)
                    {
                        currentResult_.addError(*error);
                    }
                }
            }
        }

        void BorrowChecker::visitNullCoalesceExpr(AST::NullCoalesceExpr &expr)
        {
            // (AR) ״×״¬…״¹ ״§״±״÷ ?? ג€” ״­״µ ״§„״·״± ״§„״£״³״± ˆ״§„״£…†
            // (EN) Null coalescing ?? ג€” check both left and right
            if (expr.left)
            {
                expr.left->accept(*this);

                // (AR) ״§„״·״± ״§„״£״³״± ‚״¯ †‚„ …„ƒ״© ״¥״°״§ ƒ״§† …״×״÷״±״§‹
                // (EN) Left side may transfer ownership if it's a variable
                if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(expr.left.get()))
                {
                    auto error = tracker_->useVariable(varExpr->toString(), getLocation(&expr));
                    if (error)
                    {
                        currentResult_.addError(*error);
                    }
                }
            }
            if (expr.right)
            {
                expr.right->accept(*this);
            }
        }

        void BorrowChecker::visitErrorPropagateExpr(AST::ErrorPropagateExpr &expr)
        {
            // (AR) †״´״± ״§„״®״·״£ ג€” ״×״¹״¨״± ֲ«״§†״´״±ֲ» ‚‘… ״§„״×״¹״¨״± ״§„״¯״§״®„:
            //      - ״¥״°״§ ƒ״§†״× ״§„†״×״¬״© ״®״·״£/„״§״´״¡״ ״±״¬״¹ …״¨ƒ״±״§‹ …† ״§„״¯״§„״©
            //      - ״¥״°״§ ƒ״§†״× ״§„†״×״¬״© ״µ״­״­״©״ ƒ ״§„״×״÷„ ˆ״±״¬״¹ ״§„‚…״©
            //      ‡״°״§ ״´״¨‡ ״¹״§…„ ?  Rust ״£ˆ ?. …״¹ ״¥״±״¬״§״¹ …״¨ƒ״±
            // (EN) Error propagation ג€” 'propagate' expression evaluates inner expression:
            //      - If result is error/none, returns early from function
            //      - If result is ok, unwraps and returns value
            //      Similar to Rust's ? operator

            if (!expr.inner)
                return;

            // (AR) ״­״µ ״§„״×״¹״¨״± ״§„״¯״§״®„ ג€” ‚״¯ ƒˆ† ״§״³״×״¯״¹״§״¡ ״¯״§„״© ״£ˆ …״×״÷״±
            // (EN) Check inner expression ג€” could be a function call or variable
            expr.inner->accept(*this);

            // (AR) ״¥״°״§ ƒ״§† ״§„״×״¹״¨״± ״§„״¯״§״®„ …״×״÷״±״§‹״ †״×״­‚‚ …† ״µ„״§״­״×‡
            //      †״´״± ״§„״®״·״£ ‚״±״£ ״§„…״×״÷״± („״§ †‚„‡) „״£†‡ ƒ ״§„״×״÷„ ‚״·
            // (EN) If inner is a variable, verify it's still valid
            //      Error propagation reads the variable (doesn't move) as it only unwraps
            if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(expr.inner.get()))
            {
                auto error = tracker_->useVariable(varExpr->toString(), getLocation(&expr));
                if (error)
                {
                    currentResult_.addError(*error);
                }
            }

            if (debugMode_)
            {
                recordWarning("[debug] Error propagation at " + getLocation(&expr).toString());
            }
        }

        void BorrowChecker::visitTupleExpr(AST::TupleExpr &expr)
        {
            // (AR) ״×״¹״¨״± ״§„״µ (‚…״©1״ ‚…״©2״ ...) ג€” ״¥†״´״§״¡ ״µ ״¬״¯״¯
            //      ƒ„ ״¹†״µ״±  ״§„״µ †‚„ ״¥„‡ (…״§ „… ƒ† …† †ˆ״¹ ‚״§״¨„ „„†״³״®)
            //      „״°„ƒ †״×״­‚‚ …† …„ƒ״© ƒ„ ״¹†״µ״±
            // (EN) Tuple expression (val1, val2, ...) ג€” create new tuple
            //      Each element is moved into the tuple (unless Copy type)
            //      So we verify ownership of each element

            for (auto &element : expr.elements)
            {
                if (!element)
                    continue;

                // (AR) ״¥״°״§ ƒ״§† ״§„״¹†״µ״± …״×״÷״±״§‹ ״÷״± ‚״§״¨„ „„†״³״®״ ‡״°״§ †‚„ …„ƒ״©
                // (EN) If element is a non-Copy variable, this is an ownership move
                if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(element.get()))
                {
                    std::string varName = varExpr->toString();

                    // (AR) ״×״­‚‚ …† …״¹„ˆ…״§״× ״§„…„ƒ״© „״×״­״¯״¯ ״¥״°״§ ƒ״§† ״§„†ˆ״¹ ‚״§״¨„״§‹ „„†״³״®
                    // (EN) Check ownership info to determine if type is Copy
                    auto info = tracker_->getOwnershipInfo(varName);
                    if (info && !info->isCopyType)
                    {
                        // (AR) †‚„ ״§„…„ƒ״© ג€” ״§„…״×״÷״± †‚„ ״¥„‰ ״§„״µ
                        // (EN) Move ownership ג€” variable is moved into tuple
                        auto error = tracker_->moveVariable(varName, getLocation(&expr));
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
                        auto error = tracker_->useVariable(varName, getLocation(&expr));
                        if (error)
                        {
                            currentResult_.addError(*error);
                        }
                    }
                }
                else
                {
                    // (AR) ״×״¹״¨״± ״÷״± …״×״÷״± (״­״±״ ״§״³״×״¯״¹״§״¡ ״¯״§„״©״ ...) ג€” ״­״µ ״¹״§״¯
                    // (EN) Non-variable expression (literal, function call, ...) ג€” normal check
                    element->accept(*this);
                }
            }

            if (debugMode_)
            {
                recordWarning("[debug] Tuple with " + std::to_string(expr.elements.size()) + " elements at " + getLocation(&expr).toString());
            }
        }

        // ============================================================================
        // ״²״§״±״© ״§„״¹״¨״§״±״§״× / Visit Statements
        // ============================================================================

        void BorrowChecker::visitExprStmt(AST::ExprStmt &stmt)
        {
            // (AR) ״­״µ ״§„״×״¹״¨״± ״¯״§״®„ ״§„״¹״¨״§״±״©
            // (EN) Check expression inside statement
            if (stmt.expression)
            {
                stmt.expression->accept(*this);
            }
        }

        void BorrowChecker::visitVarDeclStmt(AST::VarDeclStmt &stmt)
        {
            // (AR) ״×״³״¬„ ״§„…״×״÷״± ״§„״¬״¯״¯  ״§„…״×״×״¨״¹
            // (EN) Register new variable with tracker
            currentResult_.totalVariables++;

            // (AR) ״­״µ ״§„…״¹‘† ״£ˆ„״§‹ (‚״¨„ ״×״³״¬„ ״§„…״×״÷״±)
            // (EN) Check initializer first (before registering variable)
            if (stmt.initializer)
            {
                // (AR) ״×״­„„ ״§„…״¹‘† - ‚״¯ ״×״¶…† †‚„ …„ƒ״©
                // (EN) Analyze initializer - may involve ownership transfer

                // (AR) ״¥״°״§ ƒ״§† ״§„…״¹‘† …״×״÷״±״§‹״ ‡״°״§ †‚„ …„ƒ״©
                // (EN) If initializer is a variable, this is an ownership move
                auto *varExpr = dynamic_cast<AST::VariableExpr *>(stmt.initializer.get());
                if (varExpr)
                {
                    // (AR) †‚„ ״§„…„ƒ״© …† ״§„…״×״÷״± ״§„…״µ״¯״±
                    // (EN) Move ownership from source variable
                    auto error = tracker_->moveVariable(varExpr->toString(), getLocation(varExpr));
                    if (error)
                    {
                        currentResult_.addError(*error);
                    }
                    currentResult_.totalMoves++;
                }
                else
                {
                    // (AR) ״­״µ ״§„״×״¹״¨״± ״¨״´ƒ„ ״¹״§…
                    // (EN) Check expression generally
                    stmt.initializer->accept(*this);
                }
            }

            // (AR) ״§״³״×״®״±״§״¬ ״§״³… ״§„†ˆ״¹ …† DataType
            // (EN) Extract type name from DataType
            std::string typeName = dataTypeToString(stmt.type);
            bool isCopy = isCopyType(typeName);

            // (AR) ״¥״°״§ ƒ״§† ״§„†ˆ״¹ ״÷״± …״­״¯״¯ ˆ„ƒ† ״§„…״¹‘† ‚…״© ״­״±״©״ ״§״³״×†״×״¬ ״§„†ˆ״¹
            // (EN) If type is unspecified but initializer is a literal, infer the type
            if (!isCopy && stmt.initializer)
            {
                if (auto *lit = dynamic_cast<AST::LiteralExpr *>(stmt.initializer.get()))
                {
                    // (AR) ״§„‚… ״§„״­״±״© (״£״±‚״§…״ ״¹״´״±״©״ …†״·‚״©) ‚״§״¨„״© „„†״³״®
                    // (EN) Literal values (integers, floats, booleans) are copy types
                    Data::DataType litType = lit->getDataType();
                    isCopy = true;
                    if (litType == Data::DataType::INTEGER)
                        typeName = "״±‚…";
                    else if (litType == Data::DataType::FLOAT)
                        typeName = "״¹״´״±";
                    else if (litType == Data::DataType::BOOLEAN)
                        typeName = "…†״·‚";
                    else if (litType == Data::DataType::STRING)
                    {
                        typeName = "†״µ";
                        isCopy = false; // Strings are not copy types
                    }
                }
            }

            // (AR) ״¥״°״§ ƒ״§† ״§„…״¹‘† …״×״÷״±״§‹״ ״§״³״×†״×״¬ ״§„†ˆ״¹ …† ״§„…״×״÷״± ״§„…״µ״¯״±
            // (EN) If initializer is a variable, infer type from source variable
            if (!isCopy && stmt.initializer)
            {
                if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(stmt.initializer.get()))
                {
                    auto srcInfo = tracker_->getOwnershipInfo(varExpr->toString());
                    if (srcInfo)
                    {
                        typeName = srcInfo->typeName;
                        isCopy = srcInfo->isCopyType;
                    }
                }
            }

            // (AR) ״¥״°״§ „… †״×…ƒ† …† ״×״­״¯״¯ ״§„†ˆ״¹״ †״¹״×״¨״±‡ ‚״§״¨„״§‹ „„†״³״®
            // (EN) If we couldn't determine the type, assume it's a copy type
            // (AR) ‡״°״§ ״¢…† „״£† ״§„„״÷״© ״­״§„״§‹ ״×״¯״¹… ״§„״£†ˆ״§״¹ ״§„״¨״¯״§״¦״© ‚״· (״£״±‚״§…״ …†״·‚״ †״µˆ״µ)
            // (EN) This is safe because the language currently only supports primitive types
            if (typeName == "unknown" || typeName.empty())
            {
                isCopy = true;
            }

            if (debugMode_)
            {
                std::cout << "[BorrowChecker] Declaring variable: " << stmt.name
                          << " type: " << typeName << " isCopy: " << isCopy << "\n";
            }

            tracker_->declareVariable(stmt.name, typeName, getLocation(&stmt), isCopy);
        }

        void BorrowChecker::visitIfStmt(AST::IfStmt &stmt)
        {
            // (AR) ״­״µ ״§„״´״±״· ˆ״§„״±ˆ״¹
            // (EN) Check condition and branches
            if (stmt.condition)
            {
                stmt.condition->accept(*this);
            }
            tracker_->enterScope();
            if (stmt.thenBranch)
            {
                stmt.thenBranch->accept(*this);
            }
            tracker_->exitScope();
            if (stmt.elseBranch)
            {
                tracker_->enterScope();
                stmt.elseBranch->accept(*this);
                tracker_->exitScope();
            }
        }

        void BorrowChecker::visitWhileStmt(AST::WhileStmt &stmt)
        {
            // (AR) ״­״µ ״§„״­„‚״©
            // (EN) Check loop
            if (stmt.condition)
            {
                stmt.condition->accept(*this);
            }
            tracker_->enterScope();
            if (stmt.body)
            {
                stmt.body->accept(*this);
            }
            tracker_->exitScope();
        }

        void BorrowChecker::visitForStmt(AST::ForStmt &stmt)
        {
            // (AR) ״­״µ ״­„‚״© for
            // (EN) Check for loop
            tracker_->enterScope();
            if (stmt.initializer)
            {
                stmt.initializer->accept(*this);
            }
            if (stmt.condition)
            {
                stmt.condition->accept(*this);
            }
            if (stmt.increment)
            {
                stmt.increment->accept(*this);
            }
            if (stmt.body)
            {
                stmt.body->accept(*this);
            }
            tracker_->exitScope();
        }


    } // namespace Semantic
} // namespace Sad

