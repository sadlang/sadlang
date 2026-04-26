// ============================================================================
// sir_builder_loops.cpp - حلقات while و for
// ============================================================================
// (AR) هذا الملف يحتوي على:
//      - buildWhileLoop: بناء حلقة بينما
//      - buildForLoop: بناء حلقة لكل (for)
// (EN) This file contains:
//      - buildWhileLoop: Build while loop
//      - buildForLoop: Build for loop
// ============================================================================
// تم استخراج هذا الملف من sir_builder_control_flow.cpp وفقاً لقاعدة CW-05
// ============================================================================

#include <string>
#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <set>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            void SIRBuilder::buildWhileLoop(AST::WhileStmt *whileLoop)
            {
                if (!whileLoop)
                {
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildWhileLoop: starting" << std::endl;
#endif

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 1: ״¥†״´״§״¡ ״§„ƒ״×„ ״§„״£״³״§״³״©
                // (EN) Step 1: Create basic blocks
                // ״§„…״µ״¯״±: sir_builder.h:501 - createBasicBlock()
                // ״§„…״µ״¯״±: sir_builder.h:520 - newLabel()
                // ========================================================================
                std::string condLabel = newLabel("while_cond");
                std::string bodyLabel = newLabel("while_body");
                std::string exitLabel = newLabel("while_exit");

                auto condBlock = createBasicBlock(condLabel);
                auto bodyBlock = createBasicBlock(bodyLabel);
                auto exitBlock = createBasicBlock(exitLabel);

                // (AR) ״¥״¶״§״© ״§„ƒ״×„ ״¥„‰ ״§„״¯״§„״© ״§„״­״§„״©
                // (EN) Add blocks to current function
                if (currentFunction_)
                {
                    currentFunction_->addBasicBlock(condBlock);
                    currentFunction_->addBasicBlock(bodyBlock);
                    currentFunction_->addBasicBlock(exitBlock);
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildWhileLoop: created blocks cond=" << condLabel
                          << ", body=" << bodyLabel << ", exit=" << exitLabel << std::endl;
#endif

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 2: ‚״² ״÷״± ״´״±״· ״¥„‰ ƒ״×„״© ״§„״´״±״·
                // (EN) Step 2: Unconditional jump to condition block
                // ״§„…״µ״¯״±: sir_instruction.h:178-183 - SIRInstruction::Branch()
                // ========================================================================
                SIROperand condLabelOp = SIROperand::Label(condLabel);
                SIRInstruction brCondBlockInst = SIRInstruction::Branch(condLabelOp);

                if (currentBlock_)
                {
                    currentBlock_->instructions.push_back(brCondBlockInst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildWhileLoop: added BR to condition block" << std::endl;
#endif
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 3: ״¨†״§״¡ ״§„״´״±״·
                // (EN) Step 3: Build condition expression
                // ״§„…״µ״¯״±: WhileStmt::condition (statements.h:149)
                // ========================================================================
                currentBlock_ = condBlock;
                auto condResult = buildExpression(whileLoop->condition.get());

                if (condResult.registerName.empty())
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildWhileLoop: condition build failed!" << std::endl;
#endif
                    errors_.push_back("Error: Failed to build while condition");
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildWhileLoop: condition reg=" << condResult.registerName << std::endl;
#endif

                // (AR) ״×״­ˆ„ ״×„‚״§״¦ „€ __op_tobool__ ״¥״°״§ ƒ״§† ״§„״´״±״· ƒ״§״¦†״§‹ (…״«„ buildIfStatement)
                // (EN) Auto-convert __op_tobool__ if condition is an object (like buildIfStatement)
                {
                    std::string condClassName = condResult.className;
                    if (condClassName.empty() && !condResult.registerName.empty())
                    {
                        auto it = classInstanceTypes_.find(condResult.registerName);
                        if (it != classInstanceTypes_.end())
                            condClassName = it->second;
                    }
                    if (!condClassName.empty())
                    {
                        // (AR) ״¨״­״«  ״³„״³„״© ״§„ˆ״±״§״«״© ״¹† __op_tobool__
                        // (EN) Search inheritance chain for __op_tobool__
                        std::string searchClass = condClassName;
                        std::string toboolName;
                        bool foundToBool = false;
                        while (!searchClass.empty())
                        {
                            toboolName = searchClass + ".__op_tobool__";
                            if (functionTable_.find(toboolName) != functionTable_.end())
                            {
                                foundToBool = true;
                                break;
                            }
                            auto classInfo = module_->getClass(searchClass);
                            if (classInfo && !classInfo->parentClass.empty())
                                searchClass = classInfo->parentClass;
                            else
                                break;
                        }
                        if (foundToBool)
                        {
                            std::string boolReg = newTempRegister();
                            SIRInstruction callInst;
                            callInst.opcode = SIROpcode::OBJECT_CALL;
                            callInst.result = SIROperand::Register(boolReg, SadTypeKind::Boolean);
                            callInst.operands.push_back(SIROperand::Register(condResult.registerName, condResult.type));
                            callInst.operands.push_back(SIROperand::ConstantString("__op_tobool__"));
                            if (currentBlock_)
                                currentBlock_->addInstruction(callInst);
                            condResult = BuildResult(boolReg, SadTypeKind::Boolean);
                        }
                    }
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 4: ״×ˆ„״¯ ״×״¹„…״© ״§„‚״² ״§„״´״±״·
                // (EN) Step 4: Generate conditional branch instruction
                // ״§„…״µ״¯״±: sir_instruction.h:190-197 - SIRInstruction::BranchCond()
                // ========================================================================
                // (AR) ״¥״°״§ ƒ״§† ״§„״´״±״· ״«״§״¨״×״§‹ …†״·‚״§‹ (״µ״­״­/״®״·״£)״ †״³״×״®״¯… ConstantBool ״¨״¯„״§‹ …† Register
                //      „״×״¬†״¨ ״×ˆ„״¯ ״³״¬„ ״÷״± …״¹״±‘  LLVM IR
                // (EN) If condition is a boolean constant (true/false), use ConstantBool instead of Register
                //      to avoid generating an undefined register in LLVM IR
                SIROperand condOp;
                if (condResult.isConstant && condResult.type == SadTypeKind::Boolean)
                {
                    condOp = SIROperand::ConstantBool(condResult.constantValue == "true" || condResult.constantValue == "1");
                }
                else if (condResult.isConstant && condResult.type == SadTypeKind::Integer)
                {
                    condOp = SIROperand::ConstantI64(std::stoll(condResult.constantValue));
                }
                else
                {
                    condOp = SIROperand::Register(condResult.registerName, condResult.type);
                }
                SIROperand bodyLabelOp = SIROperand::Label(bodyLabel);
                SIROperand exitLabelOp = SIROperand::Label(exitLabel);

                SIRInstruction brCondInst = SIRInstruction::BranchCond(condOp, bodyLabelOp, exitLabelOp);

                if (currentBlock_)
                {
                    currentBlock_->instructions.push_back(brCondInst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildWhileLoop: added BR_COND (body/exit)" << std::endl;
#endif
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 4.5: ״×״³״¬„ ״³״§‚ ״§„״­„‚״© „״¯״¹… break/continue
                //      continueLabel = ƒ״×„״© ״§„״´״±״· (while_cond) ג€” continue ‚״² „„״´״±״·
                //      breakLabel = ƒ״×„״© ״§„״®״±ˆ״¬ (while_exit) ג€” break ‚״² „„״®״±ˆ״¬
                // (EN) Step 4.5: Register loop context for break/continue support
                //      continueLabel = condition block (while_cond) ג€” continue jumps to condition
                //      breakLabel = exit block (while_exit) ג€” break jumps to exit
                // ========================================================================
                LoopContext whileLoopCtx;
                whileLoopCtx.continueLabel = condLabel;
                whileLoopCtx.breakLabel = exitLabel;
                enterLoop(whileLoopCtx);

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 5: ״¨†״§״¡ ״¬״³… ״§„״­„‚״©
                // (EN) Step 5: Build loop body
                // ״§„…״µ״¯״±: WhileStmt::body (statements.h:150)
                // ========================================================================
                currentBlock_ = bodyBlock;
                if (whileLoop->body)
                {
                    buildStatement(whileLoop->body.get());
                }

                // (AR) ״§„״®״±ˆ״¬ …† ״³״§‚ ״§„״­„‚״© ״¨״¹״¯ ״¨†״§״¡ ״§„״¬״³…
                // (EN) Exit loop context after building body
                exitLoop();

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 6: ‚״² „„״¹ˆ״¯״© ״¥„‰ ƒ״×„״© ״§„״´״±״·
                // (EN) Step 6: Jump back to condition block
                // (AR) „״§ †״¶ ״§„‚״² ״¥״°״§ ƒ״§† ״§„״¬״³… ‚״¯ ״§†״×‡‰ ״¨€ RET ״£ˆ BR ״£ˆ BR_COND
                // (EN) Don't add branch if the body already ends with RET or BR or BR_COND
                // ========================================================================
                SIRInstruction brBackInst = SIRInstruction::Branch(condLabelOp);

                if (currentBlock_ && !currentBlock_->instructions.empty())
                {
                    const auto &lastInst = currentBlock_->instructions.back();
                    bool hasTerminator = (lastInst.opcode == SIROpcode::RET ||
                                          lastInst.opcode == SIROpcode::RET_VOID ||
                                          lastInst.opcode == SIROpcode::BR ||
                                          lastInst.opcode == SIROpcode::BR_COND);
                    if (!hasTerminator)
                    {
                        currentBlock_->instructions.push_back(brBackInst);
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildWhileLoop: added BR back to condition" << std::endl;
#endif
                    }
                    else
                    {
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildWhileLoop: body block already has terminator, skipping BR" << std::endl;
#endif
                    }
                }
                else if (currentBlock_)
                {
                    currentBlock_->instructions.push_back(brBackInst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildWhileLoop: added BR back to condition (empty block)" << std::endl;
#endif
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 7: ״§„״§״³״×…״±״§״± ״¨״¹״¯ ״§„״­„‚״©
                // (EN) Step 7: Continue after loop
                // ========================================================================
                currentBlock_ = exitBlock;
#ifndef NDEBUG
                std::cout << "[DEBUG] buildWhileLoop: completed, now at exit block" << std::endl;
#endif
            }

            // ============================================================================
            // buildForLoop - ״¨†״§״¡ ״­„‚״© for
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:401
            // ״§„״×ˆ‚״¹ / Signature: void buildForLoop(AST::ForStmt* forLoop);
            //
            // ״§„…״¹״§…„״§״× / Parameters:
            // - forLoop: AST::ForStmt* = Sad::AST::ForStmt* (sir_builder.h:401)
            //
            // ForStmt Members (statements.h:187-253):
            // - initializer: StmtPtr (line 193) - optional
            // - condition: ExprPtr (line 194) - optional
            // - increment: ExprPtr (line 195) - optional
            // - body: StmtPtr (line 196)
            //
            // ״§„…״×״÷״±״§״× ״§„…״³״×״®״¯…״© / Used variables:
            // - currentBlock_: sir_builder.h:582 (shared_ptr<SIRBasicBlock>)
            // - currentScopeLevel_: sir_builder.h:599 (int)
            //
            // ״§„״¯ˆ״§„ ״§„…״³״×״¯״¹״§״© / Called functions:
            // - buildStatement: sir_builder.h:372
            // - buildExpression: sir_builder.h:432
            // - createBasicBlock: sir_builder.h:501
            // - newLabel: sir_builder.h:520
            // - enterScope: sir_builder.h:587
            // - exitScope: sir_builder.h:589
            // ============================================================================
            void SIRBuilder::buildForLoop(AST::ForStmt *forLoop)
            {
                if (!forLoop)
                {
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildForLoop: starting" << std::endl;
#endif

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 1: ״¯״®ˆ„ †״·״§‚ ״¬״¯״¯ „„״­„‚״©
                // (EN) Step 1: Enter new scope for loop
                // ״§„…״µ״¯״±: sir_builder.h:587 - enterScope()
                // ========================================================================
                enterScope();

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 2: ״×†״° initializer ״¥״°״§ ˆ״¬״¯
                // (EN) Step 2: Execute initializer if exists
                // ״§„…״µ״¯״±: ForStmt::initializer (statements.h:193)
                // ========================================================================
                if (forLoop->initializer)
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildForLoop: building initializer" << std::endl;
#endif
                    buildStatement(forLoop->initializer.get());
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 3: ״¥†״´״§״¡ ״§„ƒ״×„ ״§„״£״³״§״³״©
                // (EN) Step 3: Create basic blocks
                // ========================================================================
                std::string condLabel = newLabel("for_cond");
                std::string bodyLabel = newLabel("for_body");
                std::string incLabel = newLabel("for_inc");
                std::string exitLabel = newLabel("for_exit");

                auto condBlock = createBasicBlock(condLabel);
                auto bodyBlock = createBasicBlock(bodyLabel);
                auto incBlock = createBasicBlock(incLabel);
                auto exitBlock = createBasicBlock(exitLabel);

                // (AR) ״¥״¶״§״© ״§„ƒ״×„ ״¥„‰ ״§„״¯״§„״© ״§„״­״§„״©
                // (EN) Add blocks to current function
                if (currentFunction_)
                {
                    currentFunction_->addBasicBlock(condBlock);
                    currentFunction_->addBasicBlock(bodyBlock);
                    currentFunction_->addBasicBlock(incBlock);
                    currentFunction_->addBasicBlock(exitBlock);
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildForLoop: created blocks cond=" << condLabel
                          << ", body=" << bodyLabel << ", inc=" << incLabel
                          << ", exit=" << exitLabel << std::endl;
#endif

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 4: ‚״² ״÷״± ״´״±״· ״¥„‰ ƒ״×„״© ״§„״´״±״·
                // (EN) Step 4: Unconditional jump to condition block
                // ========================================================================
                SIROperand condLabelOp = SIROperand::Label(condLabel);
                SIRInstruction brCondBlockInst = SIRInstruction::Branch(condLabelOp);

                if (currentBlock_)
                {
                    currentBlock_->instructions.push_back(brCondBlockInst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildForLoop: added BR to condition block" << std::endl;
#endif
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 5: ״¨†״§״¡ ״§„״´״±״·
                // (EN) Step 5: Build condition
                // ״§„…״µ״¯״±: ForStmt::condition (statements.h:194)
                // ========================================================================
                currentBlock_ = condBlock;

                SIROperand bodyLabelOp = SIROperand::Label(bodyLabel);
                SIROperand exitLabelOp = SIROperand::Label(exitLabel);

                if (forLoop->condition)
                {
                    auto condResult = buildExpression(forLoop->condition.get());

                    if (!condResult.registerName.empty())
                    {
                        // (AR) ״×ˆ„״¯ BR ״´״±״· „€ body ״£ˆ exit
                        // (EN) Generate conditional BR to body or exit
                        // (AR) ״¥״°״§ ƒ״§† ״§„״´״±״· ״«״§״¨״×״§‹ …†״·‚״§‹״ †״³״×״®״¯… ConstantBool ״¨״¯„״§‹ …† Register
                        // (EN) If condition is a boolean constant, use ConstantBool instead of Register
                        SIROperand condOp;
                        if (condResult.isConstant && condResult.type == SadTypeKind::Boolean)
                        {
                            condOp = SIROperand::ConstantBool(condResult.constantValue == "true" || condResult.constantValue == "1");
                        }
                        else if (condResult.isConstant && condResult.type == SadTypeKind::Integer)
                        {
                            condOp = SIROperand::ConstantI64(std::stoll(condResult.constantValue));
                        }
                        else
                        {
                            condOp = SIROperand::Register(condResult.registerName, condResult.type);
                        }
                        SIRInstruction brCondInst = SIRInstruction::BranchCond(condOp, bodyLabelOp, exitLabelOp);

                        if (currentBlock_)
                        {
                            currentBlock_->instructions.push_back(brCondInst);
#ifndef NDEBUG
                            std::cout << "[DEBUG] buildForLoop: added BR_COND (body/exit)" << std::endl;
#endif
                        }
                    }
                }
                else
                {
                    // (AR) „״§ ˆ״¬״¯ ״´״±״· - ‚״² ״÷״± ״´״±״· „€ body (״­„‚״© „״§ †‡״§״¦״©)
                    // (EN) No condition - unconditional jump to body (infinite loop)
                    SIRInstruction brBodyInst = SIRInstruction::Branch(bodyLabelOp);

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(brBodyInst);
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildForLoop: no condition, added BR to body" << std::endl;
#endif
                    }
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 5.5: ״×״³״¬„ ״³״§‚ ״§„״­„‚״© „״¯״¹… break/continue
                //      continueLabel = ƒ״×„״© ״§„״²״§״¯״© (for_inc) ג€” continue ‚״² „„״²״§״¯״© ״£ˆ„״§‹
                //      breakLabel = ƒ״×„״© ״§„״®״±ˆ״¬ (for_exit) ג€” break ‚״² „„״®״±ˆ״¬ …״¨״§״´״±״©
                //      …„״§״­״¸״©:  for״ continue ״¬״¨ ״£† †‘״° ״§„״²״§״¯״© ״«… ״¹ˆ״¯ „„״´״±״·
                // (EN) Step 5.5: Register loop context for break/continue support
                //      continueLabel = increment block (for_inc) ג€” continue goes to increment first
                //      breakLabel = exit block (for_exit) ג€” break jumps to exit directly
                //      Note: In for, continue must execute increment then go to condition
                // ========================================================================
                LoopContext forLoopCtx;
                forLoopCtx.continueLabel = incLabel;
                forLoopCtx.breakLabel = exitLabel;
                enterLoop(forLoopCtx);

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 6: ״¨†״§״¡ ״¬״³… ״§„״­„‚״©
                // (EN) Step 6: Build loop body
                // ״§„…״µ״¯״±: ForStmt::body (statements.h:196)
                // ========================================================================
                currentBlock_ = bodyBlock;
                if (forLoop->body)
                {
                    buildStatement(forLoop->body.get());
                }

                // (AR) ״§„״®״±ˆ״¬ …† ״³״§‚ ״§„״­„‚״© ״¨״¹״¯ ״¨†״§״¡ ״§„״¬״³…
                // (EN) Exit loop context after building body
                exitLoop();

                // (AR) ‚״² ״¥„‰ ƒ״×„״© ״§„״²״§״¯״©
                // (EN) Jump to increment block
                SIROperand incLabelOp = SIROperand::Label(incLabel);
                SIRInstruction brIncInst = SIRInstruction::Branch(incLabelOp);

                if (currentBlock_)
                {
                    currentBlock_->instructions.push_back(brIncInst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildForLoop: added BR to increment block" << std::endl;
#endif
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 7: ״¨†״§״¡ ״§„״²״§״¯״©
                // (EN) Step 7: Build increment
                // ״§„…״µ״¯״±: ForStmt::increment (statements.h:195)
                // ========================================================================
                currentBlock_ = incBlock;
                if (forLoop->increment)
                {
                    buildExpression(forLoop->increment.get());
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildForLoop: built increment expression" << std::endl;
#endif
                }

                // (AR) ‚״² „„״¹ˆ״¯״© ״¥„‰ ƒ״×„״© ״§„״´״±״·
                // (EN) Jump back to condition block
                SIRInstruction brBackInst = SIRInstruction::Branch(condLabelOp);

                if (currentBlock_)
                {
                    currentBlock_->instructions.push_back(brBackInst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildForLoop: added BR back to condition" << std::endl;
#endif
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 8: ״§„״§״³״×…״±״§״± ״¨״¹״¯ ״§„״­„‚״©
                // (EN) Step 8: Continue after loop
                // ========================================================================
                currentBlock_ = exitBlock;

                // (AR) ״§„״®״±ˆ״¬ …† †״·״§‚ ״§„״­„‚״©
                // (EN) Exit loop scope
                exitScope();

#ifndef NDEBUG
                std::cout << "[DEBUG] buildForLoop: completed, now at exit block" << std::endl;
#endif
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
