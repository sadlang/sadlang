// ============================================================================
// sir_builder_control_flow.cpp
// (AR) Loops: buildWhileLoop, buildForLoop, buildForRangeLoop
// (EN) Loop control flow: while, for, for-range loops
// ============================================================================

#include <string>
#include "sir_builder.h"
#include "builders/statement_builder.h"
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

            // ============================================================================
            // buildForRangeLoop - ״¨†״§״¡ ״­„‚״© „ƒ„  (foreach)
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:405
            // ״§„״×ˆ‚״¹ / Signature: void buildForRangeLoop(AST::ForRangeStmt* forRange);
            //
            // ״§„…״¹״§…„״§״× / Parameters:
            // - forRange: AST::ForRangeStmt* = Sad::AST::ForRangeStmt* (statements.h:228)
            //
            // ForRangeStmt Members (statements.h:228-235):
            // - variable: std::string (line 230) - ״§״³… …״×״÷״± ״§„״­„‚״©
            // - valueVar: std::string (line 231) - …״×״÷״± ״§„‚…״© „„‚ˆ״§…״³ (״§״®״×״§״±)
            // - iterable: ExprPtr (line 232) - ״§„״×״¹״¨״± ״§„‚״§״¨„ „„״×ƒ״±״§״±
            // - body: StmtPtr (line 233) - ״¬״³… ״§„״­„‚״©
            // ============================================================================
            void StatementBuilder::buildForRangeLoop(AST::ForRangeStmt *forRange)
            {
                if (!forRange)
                {
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildForRangeLoop: starting, variable='"
                          << forRange->variable << "'" << std::endl;
#endif

                // ========================================================================
                // (AR) …״³״§״± ״®״§״µ: ״¥״°״§ ƒ״§† ״§„״×״¹״¨״± †״·״§‚״§‹ (RangeExpr) †ˆ„‘״¯ ״­„‚״© while ״¨״³״·״©
                // (EN) Special path: if iterable is RangeExpr, generate simple while loop
                // ========================================================================
                if (auto *rangeExpr = dynamic_cast<Sad::AST::RangeExpr *>(forRange->iterable.get()))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildForRangeLoop: detected RangeExpr, using while-style loop" << std::endl;
#endif

                    b_.enterScope();

                    // (AR) ״¨†״§״¡ ״¨״¯״§״© ˆ†‡״§״© ״§„†״·״§‚
                    auto startResult = b_.buildExpression(rangeExpr->start.get());
                    auto endResult = b_.buildExpression(rangeExpr->end.get());

                    // ================================================================
                    // (AR) [Fix #47] ״×״­״¯״¯ ״§״×״¬״§‡ ״§„…״¯‰ ג€” ״µ״¹ˆ״¯ ״£ˆ ״×†״§״²„:
                    //      „ƒ„ ״¹ …† 1 ״§„‰ 5 ג†’ ״µ״¹ˆ״¯ (LE, +1)
                    //      „ƒ„ ״¹ …† 5 ״§„‰ 1 ג†’ ״×†״§״²„ (GE, -1)
                    //      „ƒ„ ״¹ …† ״³ ״§„‰ ״µ ג†’ ״­״¯״¯  ˆ‚״× ״§„״×״´״÷„
                    //      ״¨״¯ˆ† ‡״°״§: „ƒ„ ״¹ …† 5 ״§„‰ 1 „״§ †״° ״§„״­„‚״© ״£״¨״¯״§‹
                    //      „״£† ״§„״´״±״· 5 <= 1 ״®״·״£ …† ״£ˆ„ ״×ƒ״±״§״±
                    // (EN) [Fix #47] Determine range direction ג€” ascending or descending:
                    //      for i from 1 to 5 ג†’ ascending (LE, +1)
                    //      for i from 5 to 1 ג†’ descending (GE, -1)
                    //      for i from x to y ג†’ determined at runtime
                    //      Without this: for i from 5 to 1 never executes because 5<=1 is false
                    // ================================================================
                    bool isDescending = false;
                    bool isStaticDirection = false;

                    if (startResult.isConstant && endResult.isConstant)
                    {
                        try
                        {
                            int64_t startVal = std::stoll(startResult.constantValue);
                            int64_t endVal = std::stoll(endResult.constantValue);
                            isDescending = (startVal > endVal);
                            isStaticDirection = true;
                        }
                        catch (...)
                        {
                        }
                    }

                    // (AR) ״¥†״´״§״¡ ״§„ƒ״×„ ״§„״£״³״§״³״©
                    std::string condL = b_.newLabel("range_cond");
                    std::string bodyL = b_.newLabel("range_body");
                    std::string incL = b_.newLabel("range_inc");
                    std::string exitL = b_.newLabel("range_exit");

                    auto condB = b_.createBasicBlock(condL);
                    auto bodyB = b_.createBasicBlock(bodyL);
                    auto incB = b_.createBasicBlock(incL);
                    auto exitB = b_.createBasicBlock(exitL);

                    // (AR) ƒ״×„ ״¥״¶״§״© „„…״¯‰ ״§„״¯†״§…ƒ ג€” ״×״­״¯״¯ ״§„״§״×״¬״§‡  ˆ‚״× ״§„״×״´״÷„
                    // (EN) Extra blocks for dynamic range ג€” determine direction at runtime
                    std::shared_ptr<SIRBasicBlock> stepAscB, stepDescB;
                    std::string stepAscL, stepDescL;
                    if (!isStaticDirection)
                    {
                        stepAscL = b_.newLabel("step_asc");
                        stepDescL = b_.newLabel("step_desc");
                        stepAscB = b_.createBasicBlock(stepAscL);
                        stepDescB = b_.createBasicBlock(stepDescL);
                    }

                    if (b_.currentFunction_)
                    {
                        if (!isStaticDirection)
                        {
                            b_.currentFunction_->addBasicBlock(stepAscB);
                            b_.currentFunction_->addBasicBlock(stepDescB);
                        }
                        b_.currentFunction_->addBasicBlock(condB);
                        b_.currentFunction_->addBasicBlock(bodyB);
                        b_.currentFunction_->addBasicBlock(incB);
                        b_.currentFunction_->addBasicBlock(exitB);
                    }

                    // (AR) ״×״®״µ״µ alloca „…״×״÷״± ״§„״­„‚״©
                    std::string loopVarAlloc = "%" + forRange->variable;
                    {
                        SIRInstruction allocInst(SIROpcode::ALLOC);
                        allocInst.result = SIROperand::Register(loopVarAlloc, SadTypeKind::Integer);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(allocInst);
                    }
                    // (AR) ״¯״§„״© …״³״§״¹״¯״©: ״×״­ˆ„ BuildResult ״¥„‰ SIROperand …״¹ …״±״§״¹״§״© ״§„״«ˆ״§״¨״×
                    // (EN) Helper: convert BuildResult to SIROperand, handling constants
                    auto resultToOperand = [](const BuildResult &r) -> SIROperand
                    {
                        if (r.isConstant && !r.constantValue.empty())
                        {
                            if (r.type == SadTypeKind::Integer)
                            {
                                return SIROperand::ConstantI64(std::stoll(r.constantValue));
                            }
                            else if (r.type == SadTypeKind::Float)
                            {
                                return SIROperand::ConstantF64(std::stod(r.constantValue));
                            }
                        }
                        return SIROperand::Register(r.registerName, r.type);
                    };

                    // (AR) ״×‡״¦״© ״§„…״×״÷״± ״¨‚…״© ״§„״¨״¯״§״©
                    {
                        SIRInstruction storeInit(SIROpcode::STORE);
                        storeInit.operands.push_back(resultToOperand(startResult));
                        storeInit.operands.push_back(SIROperand::Register(loopVarAlloc, SadTypeKind::Integer));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(storeInit);
                    }

                    // (AR) ״×״®״µ״µ alloca „†‡״§״© ״§„†״·״§‚
                    std::string endAlloc = b_.newTempRegister();
                    {
                        SIRInstruction allocEnd(SIROpcode::ALLOC);
                        allocEnd.result = SIROperand::Register(endAlloc, SadTypeKind::Integer);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(allocEnd);
                    }
                    {
                        SIRInstruction storeEnd(SIROpcode::STORE);
                        storeEnd.operands.push_back(resultToOperand(endResult));
                        storeEnd.operands.push_back(SIROperand::Register(endAlloc, SadTypeKind::Integer));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(storeEnd);
                    }

                    // (AR) ״×״³״¬„ …״×״÷״± ״§„״­„‚״©
                    VariableInfo varInfo;
                    varInfo.name = forRange->variable;
                    varInfo.registerName = loopVarAlloc;
                    varInfo.type = SadTypeKind::Integer;
                    varInfo.isMutable = true;
                    b_.addVariable(varInfo);

                    // ================================================================
                    // (AR) [Fix #47] ״×״®״µ״µ alloca „״®״·ˆ״© ״§„…״¯‰ ˆ״×‡״¦״×‡״§:
                    //      ״«״§״¨״× ״µ״¹ˆ״¯: step = 1
                    //      ״«״§״¨״× ״×†״§״²„: step = -1
                    //      ״¯†״§…ƒ: ״­״µ start <= end  ˆ‚״× ״§„״×״´״÷„
                    // ================================================================
                    std::string stepAlloc = b_.newTempRegister();
                    {
                        SIRInstruction allocStep(SIROpcode::ALLOC);
                        allocStep.result = SIROperand::Register(stepAlloc, SadTypeKind::Integer);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(allocStep);
                    }

                    if (isStaticDirection)
                    {
                        // (AR) ״§„״§״×״¬״§‡ …״¹״±ˆ ˆ‚״× ״§„״×״±״¬…״© ג€” †״®״²† ״§„״®״·ˆ״© …״¨״§״´״±״©
                        SIRInstruction storeStep(SIROpcode::STORE);
                        storeStep.operands.push_back(SIROperand::ConstantI64(isDescending ? -1 : 1));
                        storeStep.operands.push_back(SIROperand::Register(stepAlloc, SadTypeKind::Integer));
                        storeStep.comment = isDescending ? "step = -1 (descending)" : "step = 1 (ascending)";
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(storeStep);

                        // ״§„‚״² ״¥„‰ ״§„״´״±״·
                        SIRInstruction br = SIRInstruction::Branch(SIROperand::Label(condL));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(br);
                    }
                    else
                    {
                        // (AR) ״§„״§״×״¬״§‡ ״÷״± …״¹״±ˆ ג€” †״­״µ start <= end  ˆ‚״× ״§„״×״´״÷„
                        //      ˆ†‚״² „ƒ״×„״© step_asc ״£ˆ step_desc „״×״¹† ״§„״®״·ˆ״©
                        // (EN) Unknown direction ג€” check start <= end at runtime
                        //      branch to step_asc or step_desc to set step value
                        std::string loadS = b_.newTempRegister();
                        {
                            SIRInstruction ls(SIROpcode::LOAD);
                            ls.result = SIROperand::Register(loadS, SadTypeKind::Integer);
                            ls.operands.push_back(SIROperand::Register(loopVarAlloc, SadTypeKind::Integer));
                            if (b_.currentBlock_)
                                b_.currentBlock_->instructions.push_back(ls);
                        }
                        std::string loadE = b_.newTempRegister();
                        {
                            SIRInstruction le(SIROpcode::LOAD);
                            le.result = SIROperand::Register(loadE, SadTypeKind::Integer);
                            le.operands.push_back(SIROperand::Register(endAlloc, SadTypeKind::Integer));
                            if (b_.currentBlock_)
                                b_.currentBlock_->instructions.push_back(le);
                        }
                        std::string dirCmp = b_.newTempRegister();
                        {
                            SIRInstruction cmp = SIRInstruction::Binary(
                                SIROpcode::LE, SIROperand::Register(dirCmp, SadTypeKind::Boolean),
                                SIROperand::Register(loadS, SadTypeKind::Integer),
                                SIROperand::Register(loadE, SadTypeKind::Integer));
                            cmp.comment = "start <= end? (direction check)";
                            if (b_.currentBlock_)
                                b_.currentBlock_->instructions.push_back(cmp);
                        }
                        {
                            SIRInstruction brDir = SIRInstruction::BranchCond(
                                SIROperand::Register(dirCmp, SadTypeKind::Boolean),
                                SIROperand::Label(stepAscL), SIROperand::Label(stepDescL));
                            if (b_.currentBlock_)
                                b_.currentBlock_->instructions.push_back(brDir);
                        }

                        // -- ƒ״×„״© step_asc: ״×״®״²† step = 1 --
                        b_.currentBlock_ = stepAscB;
                        {
                            SIRInstruction store(SIROpcode::STORE);
                            store.operands.push_back(SIROperand::ConstantI64(1));
                            store.operands.push_back(SIROperand::Register(stepAlloc, SadTypeKind::Integer));
                            store.comment = "step = 1 (ascending)";
                            b_.currentBlock_->instructions.push_back(store);
                        }
                        {
                            SIRInstruction br = SIRInstruction::Branch(SIROperand::Label(condL));
                            b_.currentBlock_->instructions.push_back(br);
                        }

                        // -- ƒ״×„״© step_desc: ״×״®״²† step = -1 --
                        b_.currentBlock_ = stepDescB;
                        {
                            SIRInstruction store(SIROpcode::STORE);
                            store.operands.push_back(SIROperand::ConstantI64(-1));
                            store.operands.push_back(SIROperand::Register(stepAlloc, SadTypeKind::Integer));
                            store.comment = "step = -1 (descending)";
                            b_.currentBlock_->instructions.push_back(store);
                        }
                        {
                            SIRInstruction br = SIRInstruction::Branch(SIROperand::Label(condL));
                            b_.currentBlock_->instructions.push_back(br);
                        }
                    }

                    // ---- ƒ״×„״© ״§„״´״±״· ----
                    b_.currentBlock_ = condB;
                    std::string loadedVar = b_.newTempRegister();
                    {
                        SIRInstruction loadV(SIROpcode::LOAD);
                        loadV.result = SIROperand::Register(loadedVar, SadTypeKind::Integer);
                        loadV.operands.push_back(SIROperand::Register(loopVarAlloc, SadTypeKind::Integer));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(loadV);
                    }
                    std::string loadedEnd = b_.newTempRegister();
                    {
                        SIRInstruction loadE(SIROpcode::LOAD);
                        loadE.result = SIROperand::Register(loadedEnd, SadTypeKind::Integer);
                        loadE.operands.push_back(SIROperand::Register(endAlloc, SadTypeKind::Integer));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(loadE);
                    }
                    std::string cmpReg = b_.newTempRegister();
                    {
                        // ================================================================
                        // (AR) [Fix #47] ״§„״´״±״· ״¹״×…״¯ ״¹„‰ ״§״×״¬״§‡ ״§„…״¯‰:
                        //      ״µ״¹ˆ״¯ (״«״§״¨״×): LE (״£״µ״÷״± ״£ˆ ״³״§ˆ)
                        //      ״×†״§״²„ (״«״§״¨״×): GE (״£ƒ״¨״± ״£ˆ ״³״§ˆ)
                        //      ״¯†״§…ƒ: †״­״µ step > 0 ˆ†״®״×״§״± LE ״£ˆ GE
                        //       ״§„״­״§„״© ״§„״¯†״§…ƒ״© ††״´״¦ ƒ״×„״× ״´״±״· …†״µ„״×†
                        // (EN) [Fix #47] Condition depends on range direction:
                        //      ascending (static): LE
                        //      descending (static): GE
                        //      dynamic: check step > 0, choose LE or GE
                        // ================================================================
                        if (isStaticDirection)
                        {
                            SIROpcode cmpOp = isDescending ? SIROpcode::GE : SIROpcode::LE;
                            SIRInstruction cmp = SIRInstruction::Binary(
                                cmpOp, SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                                SIROperand::Register(loadedVar, SadTypeKind::Integer),
                                SIROperand::Register(loadedEnd, SadTypeKind::Integer));
                            cmp.comment = isDescending ? "i >= end (descending)" : "i <= end (ascending)";
                            if (b_.currentBlock_)
                                b_.currentBlock_->instructions.push_back(cmp);
                        }
                        else
                        {
                            // (AR) ״¯†״§…ƒ: †‚״±״£ step״ ״¥״°״§ step > 0 ג†’ LE״ ˆ״¥„״§ ג†’ GE
                            // (EN) Dynamic: read step, if step > 0 ג†’ LE, else ג†’ GE
                            std::string loadStep = b_.newTempRegister();
                            {
                                SIRInstruction ls(SIROpcode::LOAD);
                                ls.result = SIROperand::Register(loadStep, SadTypeKind::Integer);
                                ls.operands.push_back(SIROperand::Register(stepAlloc, SadTypeKind::Integer));
                                b_.currentBlock_->instructions.push_back(ls);
                            }
                            std::string stepPos = b_.newTempRegister();
                            {
                                SIRInstruction sp = SIRInstruction::Binary(
                                    SIROpcode::GT, SIROperand::Register(stepPos, SadTypeKind::Boolean),
                                    SIROperand::Register(loadStep, SadTypeKind::Integer),
                                    SIROperand::ConstantI64(0));
                                sp.comment = "step > 0?";
                                b_.currentBlock_->instructions.push_back(sp);
                            }
                            // (AR) ƒ״×„ ״´״±״· ״±״¹״©: le_cond ˆ ge_cond
                            std::string leCL = b_.newLabel("le_cond");
                            std::string geCL = b_.newLabel("ge_cond");
                            std::string mergeCL = b_.newLabel("cond_merge");
                            auto leCB = b_.createBasicBlock(leCL);
                            auto geCB = b_.createBasicBlock(geCL);
                            auto mergeCB = b_.createBasicBlock(mergeCL);
                            if (b_.currentFunction_)
                            {
                                b_.currentFunction_->addBasicBlock(leCB);
                                b_.currentFunction_->addBasicBlock(geCB);
                                b_.currentFunction_->addBasicBlock(mergeCB);
                            }
                            {
                                SIRInstruction br = SIRInstruction::BranchCond(
                                    SIROperand::Register(stepPos, SadTypeKind::Boolean),
                                    SIROperand::Label(leCL), SIROperand::Label(geCL));
                                b_.currentBlock_->instructions.push_back(br);
                            }
                            // -- LE ƒ״×„״© --
                            b_.currentBlock_ = leCB;
                            std::string leRes = b_.newTempRegister();
                            {
                                SIRInstruction cmp = SIRInstruction::Binary(
                                    SIROpcode::LE, SIROperand::Register(leRes, SadTypeKind::Boolean),
                                    SIROperand::Register(loadedVar, SadTypeKind::Integer),
                                    SIROperand::Register(loadedEnd, SadTypeKind::Integer));
                                b_.currentBlock_->instructions.push_back(cmp);
                            }
                            {
                                SIRInstruction br = SIRInstruction::Branch(SIROperand::Label(mergeCL));
                                b_.currentBlock_->instructions.push_back(br);
                            }
                            // -- GE ƒ״×„״© --
                            b_.currentBlock_ = geCB;
                            std::string geRes = b_.newTempRegister();
                            {
                                SIRInstruction cmp = SIRInstruction::Binary(
                                    SIROpcode::GE, SIROperand::Register(geRes, SadTypeKind::Boolean),
                                    SIROperand::Register(loadedVar, SadTypeKind::Integer),
                                    SIROperand::Register(loadedEnd, SadTypeKind::Integer));
                                b_.currentBlock_->instructions.push_back(cmp);
                            }
                            {
                                SIRInstruction br = SIRInstruction::Branch(SIROperand::Label(mergeCL));
                                b_.currentBlock_->instructions.push_back(br);
                            }
                            // -- ״¯…״¬: PHI „״§״®״×״§״± ״§„†״×״¬״© ״§„״µ״­״­״© --
                            b_.currentBlock_ = mergeCB;
                            {
                                SIRInstruction phi(SIROpcode::PHI);
                                phi.result = SIROperand::Register(cmpReg, SadTypeKind::Boolean);
                                phi.operands.push_back(SIROperand::Register(leRes, SadTypeKind::Boolean));
                                phi.operands.push_back(SIROperand::Label(leCL));
                                phi.operands.push_back(SIROperand::Register(geRes, SadTypeKind::Boolean));
                                phi.operands.push_back(SIROperand::Label(geCL));
                                phi.comment = "range direction PHI";
                                b_.currentBlock_->instructions.push_back(phi);
                            }
                        }
                    }
                    {
                        SIRInstruction brCond = SIRInstruction::BranchCond(
                            SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                            SIROperand::Label(bodyL), SIROperand::Label(exitL));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(brCond);
                    }

                    // ---- ƒ״×„״© ״§„״¬״³… ----
                    b_.currentBlock_ = bodyB;
                    if (forRange->body)
                    {
                        buildStatement(forRange->body.get());
                    }
                    {
                        SIRInstruction brInc = SIRInstruction::Branch(SIROperand::Label(incL));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(brInc);
                    }

                    // ---- ƒ״×„״© ״§„״²״§״¯״© ----
                    b_.currentBlock_ = incB;
                    std::string loadedInc = b_.newTempRegister();
                    {
                        SIRInstruction loadI(SIROpcode::LOAD);
                        loadI.result = SIROperand::Register(loadedInc, SadTypeKind::Integer);
                        loadI.operands.push_back(SIROperand::Register(loopVarAlloc, SadTypeKind::Integer));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(loadI);
                    }
                    // ================================================================
                    // (AR) [Fix #47] ״§״³״×״®״¯״§… step ״¨״¯„ +1 ״§„״«״§״¨״×:
                    //      ״µ״¹ˆ״¯ ג†’ i = i + 1
                    //      ״×†״§״²„ ג†’ i = i + (-1) = i - 1
                    //      ״¯†״§…ƒ ג†’ i = i + step (״­״« step = 1 ״£ˆ -1)
                    // (EN) [Fix #47] Use step instead of hardcoded +1:
                    //      ascending ג†’ i = i + 1
                    //      descending ג†’ i = i + (-1) = i - 1
                    //      dynamic ג†’ i = i + step (where step = 1 or -1)
                    // ================================================================
                    std::string newVal = b_.newTempRegister();
                    if (isStaticDirection)
                    {
                        SIRInstruction addInst = SIRInstruction::Binary(
                            SIROpcode::ADD_I64, SIROperand::Register(newVal, SadTypeKind::Integer),
                            SIROperand::Register(loadedInc, SadTypeKind::Integer),
                            SIROperand::ConstantI64(isDescending ? -1 : 1));
                        addInst.comment = isDescending ? "i = i - 1 (descending)" : "i = i + 1 (ascending)";
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(addInst);
                    }
                    else
                    {
                        // (AR) ״¯†״§…ƒ: †‚״±״£ step ˆ†״¶‡
                        std::string loadStep = b_.newTempRegister();
                        {
                            SIRInstruction ls(SIROpcode::LOAD);
                            ls.result = SIROperand::Register(loadStep, SadTypeKind::Integer);
                            ls.operands.push_back(SIROperand::Register(stepAlloc, SadTypeKind::Integer));
                            if (b_.currentBlock_)
                                b_.currentBlock_->instructions.push_back(ls);
                        }
                        SIRInstruction addInst = SIRInstruction::Binary(
                            SIROpcode::ADD_I64, SIROperand::Register(newVal, SadTypeKind::Integer),
                            SIROperand::Register(loadedInc, SadTypeKind::Integer),
                            SIROperand::Register(loadStep, SadTypeKind::Integer));
                        addInst.comment = "i = i + step (dynamic direction)";
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(addInst);
                    }
                    {
                        SIRInstruction storeNew(SIROpcode::STORE);
                        storeNew.operands.push_back(SIROperand::Register(newVal, SadTypeKind::Integer));
                        storeNew.operands.push_back(SIROperand::Register(loopVarAlloc, SadTypeKind::Integer));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(storeNew);
                    }
                    {
                        SIRInstruction brBack = SIRInstruction::Branch(SIROperand::Label(condL));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(brBack);
                    }

                    // ---- ƒ״×„״© ״§„״®״±ˆ״¬ ----
                    b_.currentBlock_ = exitB;
                    b_.exitScope();
                    return;
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 1: ״¯״®ˆ„ †״·״§‚ ״¬״¯״¯ „„״­„‚״©
                // (EN) Step 1: Enter new scope for loop
                // ========================================================================
                b_.enterScope();

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 2: ״¨†״§״¡ ״§„״×״¹״¨״± ״§„‚״§״¨„ „„״×ƒ״±״§״±
                // (EN) Step 2: Build iterable expression
                // ״§„…״µ״¯״±: ForRangeStmt::iterable (statements.h:232)
                // ========================================================================
                auto iterableResult = b_.buildExpression(forRange->iterable.get());

                if (iterableResult.registerName.empty())
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildForRangeLoop: failed to build iterable!" << std::endl;
#endif
                    b_.errors_.push_back("Error: Failed to build iterable expression in for-range");
                    b_.exitScope();
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildForRangeLoop: iterable reg='"
                          << iterableResult.registerName << "'" << std::endl;
#endif

                // ========================================================================
                // (AR) …״³״§״± ״®״§״µ: ״¥״°״§ ƒ״§† ״§„״×״¹״¨״± ‚†״§״© ג€” ״­„‚״© ״§״³״×‚״¨״§„ …† ״§„‚†״§״©
                // (EN) Special path: if iterable is a channel ג€” channel receive loop
                //
                // (AR) ״¨†״© ״§„״­„‚״©:
                //   [chan_cond]  ג€” ״­״µ ˆ״¬ˆ״¯ ״¨״§†״§״×  ״§„‚†״§״©
                //   [chan_check] ג€” ״¥״°״§ „״§ ״¨״§†״§״×״ ״­״µ ״¥״÷„״§‚ ״§„‚†״§״© ג†’ ״®״±ˆ״¬ ״£ˆ ״§†״×״¸״§״±
                //   [chan_body]  ג€” ״§״³״×‚״¨״§„ + ״×†״° ״§„״¬״³…
                //   [chan_exit]  ג€” ״¨״¹״¯ ״§†״×‡״§״¡ ״§„‚†״§״©
                // ========================================================================
                if (iterableResult.className == "__channel__")
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildForRangeLoop: detected channel iterable" << std::endl;
#endif

                    // (AR) ״¥†״´״§״¡ ״§„ƒ״×„ ״§„״£״³״§״³״© „״­„‚״© ״§„‚†״§״©
                    // (EN) Create basic blocks for channel loop
                    std::string condLabel = b_.newLabel("chan_cond");
                    std::string checkLabel = b_.newLabel("chan_check_closed");
                    std::string bodyLabel = b_.newLabel("chan_body");
                    std::string exitLabel = b_.newLabel("chan_exit");

                    auto condBlock = b_.createBasicBlock(condLabel);
                    auto checkBlock = b_.createBasicBlock(checkLabel);
                    auto bodyBlock = b_.createBasicBlock(bodyLabel);
                    auto exitBlock = b_.createBasicBlock(exitLabel);

                    if (b_.currentFunction_)
                    {
                        b_.currentFunction_->addBasicBlock(condBlock);
                        b_.currentFunction_->addBasicBlock(checkBlock);
                        b_.currentFunction_->addBasicBlock(bodyBlock);
                        b_.currentFunction_->addBasicBlock(exitBlock);
                    }

                    // (AR) تخصيص متغير الحلقة وتحديد نوعه من b_.channelTypeMap_
                    // (EN) Allocate loop variable and infer its type from b_.channelTypeMap_
                    std::string loopVarAllocName = "%" + forRange->variable;
                    SadTypeKind chanElemType = SadTypeKind::Integer;
                    {
                        // (AR) البحث عن نوع عنصر القناة من اسم السجل أو اسم المتغير الأصلي
                        // (EN) Look up the channel element type by register or source variable name
                        auto ctIt = b_.channelTypeMap_.find(iterableResult.registerName);
                        if (ctIt != b_.channelTypeMap_.end())
                        {
                            chanElemType = ctIt->second;
                        }
                        else if (auto *iterVar = dynamic_cast<Sad::AST::VariableExpr *>(
                                     forRange->iterable.get()))
                        {
                            ctIt = b_.channelTypeMap_.find(iterVar->name);
                            if (ctIt != b_.channelTypeMap_.end())
                            {
                                chanElemType = ctIt->second;
                            }
                        }
                    }
                    {
                        SIRInstruction allocLoop(SIROpcode::ALLOC);
                        allocLoop.result = SIROperand::Register(loopVarAllocName, chanElemType);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(allocLoop);
                    }

                    // (AR) تسجيل متغير الحلقة بالنوع المستنتج
                    // (EN) Register the loop variable with the inferred type
                    VariableInfo chanVarInfo;
                    chanVarInfo.name = forRange->variable;
                    chanVarInfo.registerName = loopVarAllocName;
                    chanVarInfo.type = chanElemType;
                    chanVarInfo.isMutable = true;
                    b_.addVariable(chanVarInfo);

                    // (AR) ״§„‚״² ״¥„‰ ƒ״×„״© ״§„״´״±״·
                    // (EN) Jump to condition block
                    {
                        SIRInstruction br = SIRInstruction::Branch(SIROperand::Label(condLabel));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(br);
                    }

                    // ---- ƒ״×„״© ״§„״´״±״·: ‡„ ˆ״¬״¯ ״¨״§†״§״×״ ----
                    // ---- Condition: does channel have data? ----
                    b_.currentBlock_ = condBlock;
                    SIROperand chanOp = SIROperand::Register(iterableResult.registerName, iterableResult.type);

                    std::string hasDataReg = b_.newTempRegister();
                    {
                        SIRInstruction hasDataInst(SIROpcode::ASYNC_CHANNEL_HAS_DATA);
                        hasDataInst.result = SIROperand::Register(hasDataReg, SadTypeKind::Integer);
                        hasDataInst.operands.push_back(chanOp);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(hasDataInst);
                    }

                    // (AR) …‚״§״±†״©: has_data != 0 ג†’ …†״·‚
                    // (EN) Compare: has_data != 0 ג†’ bool
                    std::string hasDataBool = b_.newTempRegister();
                    {
                        SIRInstruction cmp = SIRInstruction::Binary(
                            SIROpcode::NE, SIROperand::Register(hasDataBool, SadTypeKind::Boolean),
                            SIROperand::Register(hasDataReg, SadTypeKind::Integer),
                            SIROperand::ConstantI64(0));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(cmp);
                    }

                    // BR_COND: has_data ג†’ body, else ג†’ check_closed
                    {
                        SIRInstruction brCond = SIRInstruction::BranchCond(
                            SIROperand::Register(hasDataBool, SadTypeKind::Boolean),
                            SIROperand::Label(bodyLabel),
                            SIROperand::Label(checkLabel));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(brCond);
                    }

                    // ---- ƒ״×„״© ״­״µ ״§„״¥״÷„״§‚ ----
                    // ---- Check closed block ----
                    b_.currentBlock_ = checkBlock;

                    std::string isClosedReg = b_.newTempRegister();
                    {
                        SIRInstruction isClosedInst(SIROpcode::ASYNC_CHANNEL_IS_CLOSED);
                        isClosedInst.result = SIROperand::Register(isClosedReg, SadTypeKind::Integer);
                        isClosedInst.operands.push_back(chanOp);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(isClosedInst);
                    }

                    std::string isClosedBool = b_.newTempRegister();
                    {
                        SIRInstruction cmp = SIRInstruction::Binary(
                            SIROpcode::NE, SIROperand::Register(isClosedBool, SadTypeKind::Boolean),
                            SIROperand::Register(isClosedReg, SadTypeKind::Integer),
                            SIROperand::ConstantI64(0));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(cmp);
                    }

                    // (AR) ״¥״°״§ …״÷„‚״© ג†’ ״®״±ˆ״¬״ ˆ״¥„״§ ג†’ ״¹ˆ״¯״© „„״´״±״· (״§†״×״¸״§״±)
                    // (EN) If closed ג†’ exit, else ג†’ back to cond (wait)
                    {
                        SIRInstruction brCond = SIRInstruction::BranchCond(
                            SIROperand::Register(isClosedBool, SadTypeKind::Boolean),
                            SIROperand::Label(exitLabel),
                            SIROperand::Label(condLabel));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(brCond);
                    }

                    // ---- ƒ״×„״© ״§„״¬״³…: ״§״³״×‚״¨״§„ + ״×†״° ----
                    // ---- Body: receive + execute ----
                    b_.currentBlock_ = bodyBlock;

                    std::string recvReg = b_.newTempRegister();
                    {
                        SIRInstruction recvInst(SIROpcode::ASYNC_CHANNEL_RECV);
                        recvInst.result = SIROperand::Register(recvReg, chanElemType);
                        recvInst.operands.push_back(chanOp);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(recvInst);
                    }

                    // (AR) ״×״®״²† ״§„‚…״© ״§„…״³״×‚״¨„״©  …״×״÷״± ״§„״­„‚״©
                    // (EN) Store received value into loop variable
                    {
                        SIRInstruction storeElem(SIROpcode::STORE);
                        storeElem.operands.push_back(SIROperand::Register(recvReg, chanElemType));
                        storeElem.operands.push_back(SIROperand::Register(loopVarAllocName, chanElemType));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(storeElem);
                    }

                    // (AR) ״×״³״¬„ ״³״§‚ ״§„״­„‚״© „״¯״¹… ״×ˆ‚/״§״³״×…״±
                    // (EN) Register loop context for break/continue
                    LoopContext chanLoopCtx;
                    chanLoopCtx.continueLabel = condLabel;
                    chanLoopCtx.breakLabel = exitLabel;
                    b_.enterLoop(chanLoopCtx);

                    // (AR) ״¨†״§״¡ ״¬״³… ״§„״­„‚״©
                    // (EN) Build loop body
                    if (forRange->body)
                    {
                        buildStatement(forRange->body.get());
                    }

                    b_.exitLoop();

                    // (AR) ״§„״¹ˆ״¯״© ״¥„‰ ״§„״´״±״·
                    // (EN) Jump back to condition
                    {
                        SIRInstruction br = SIRInstruction::Branch(SIROperand::Label(condLabel));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(br);
                    }

                    // ---- ƒ״×„״© ״§„״®״±ˆ״¬ ----
                    b_.currentBlock_ = exitBlock;
                    b_.exitScope();

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildForRangeLoop: channel iteration completed" << std::endl;
#endif
                    return;
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 3: ״¥†״´״§״¡ ״§„ƒ״×„ ״§„״£״³״§״³״© (…״³״§״± ״§„…״µˆ״©)
                // (EN) Step 3: Create basic blocks (array path)
                // ========================================================================
                std::string condLabel = b_.newLabel("foreach_cond");
                std::string bodyLabel = b_.newLabel("foreach_body");
                std::string incLabel = b_.newLabel("foreach_inc");
                std::string exitLabel = b_.newLabel("foreach_exit");

                auto condBlock = b_.createBasicBlock(condLabel);
                auto bodyBlock = b_.createBasicBlock(bodyLabel);
                auto incBlock = b_.createBasicBlock(incLabel);
                auto exitBlock = b_.createBasicBlock(exitLabel);

                // (AR) ״¥״¶״§״© ״§„ƒ״×„ ״¥„‰ ״§„״¯״§„״© ״§„״­״§„״©
                // (EN) Add blocks to current function
                if (b_.currentFunction_)
                {
                    b_.currentFunction_->addBasicBlock(condBlock);
                    b_.currentFunction_->addBasicBlock(bodyBlock);
                    b_.currentFunction_->addBasicBlock(incBlock);
                    b_.currentFunction_->addBasicBlock(exitBlock);
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 4: ״¥†״´״§״¡ …״×״÷״± ״§„״¹״¯״§״¯ ״¹״¨״± ALLOC+STORE (†…״· ״÷״±-SSA ״¢…†)
                // (EN) Step 4: Create index counter via ALLOC+STORE (safe non-SSA pattern)
                // (AR) †״³״×״®״¯… †…״· ״§„״×״®״µ״µ ״¹„‰ ״§„…ƒ״¯״³ …״«„ ״§„…״×״÷״±״§״× ״§„…״­„״© „״×״¬†״¨
                //      …״´ƒ„״© ״×״¹״± ״§„״³״¬„ ״§„ˆ״§״­״¯  ƒ״×„ …״×״¹״¯״¯״© (non-SSA counter).
                // (EN) We use stack allocation like local variables to avoid single-def
                //      register being "defined" in multiple blocks (non-SSA counter).
                // ========================================================================
                std::string idxSuffix = condLabel.substr(condLabel.find_last_of('_') + 1);
                std::string indexAllocName = "%_foreach_idx_" + idxSuffix;

                // ALLOC the counter slot
                {
                    SIRInstruction allocIdx(SIROpcode::ALLOC);
                    allocIdx.result = SIROperand::Register(indexAllocName, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(allocIdx);
                }

                // STORE 0 into the counter slot (initialize to 0)
                {
                    SIRInstruction storeZero(SIROpcode::STORE);
                    storeZero.operands.push_back(SIROperand::ConstantI64(0));
                    storeZero.operands.push_back(SIROperand::Register(indexAllocName, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(storeZero);
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 5: ״¥†״´״§״¡ …״×״÷״± ״§„״­„‚״© ˆ״×״³״¬„‡  ״§„†״·״§‚
                // (EN) Step 5: Create loop variable and register in scope
                // ״§„…״µ״¯״±: ForRangeStmt::variable (statements.h:230)
                // ========================================================================
                // (AR) [״¥״µ„״§״­] ״§״³״×״®״¯״§… ״§״³… ״±״¯ „…״×״÷״± ״§„״­„‚״© „״×״¬†״¨ ״×״¶״§״±״¨ ALLOCAs
                //      ״¹†״¯ ˆ״¬ˆ״¯ ״¹״¯״© ״­„‚״§״× „ƒ„ ״¨†״³ ״§״³… ״§„…״×״÷״± (…״«„ ״¹).
                //      ״¨״¯ˆ† ״§„„״§״­‚״©: namedValues["%״¹"] ƒ״×״¨ ˆ‚‡ ״¹״¯״© …״±״§״×  codegen
                //      ˆ״¹…„״§״× LOAD/STORE ״¯״§״®„ ״§„״­„‚״© ״×‚״±״£/״×ƒ״×״¨  alloca ״§„״­„‚״© ״§„״®״·״£.
                // (EN) [Fix] Use unique name for loop variable to avoid alloca collision
                //      when multiple foreach loops use the same variable name (e.g. ״¹).
                //      Without suffix: namedValues["%״¹"] gets overwritten in codegen
                //      and LOAD/STORE inside the loop body reads/writes wrong alloca.
                std::string loopVarAllocName = "%" + forRange->variable + "_" + idxSuffix;

                // ALLOC the loop variable slot
                // (AR) †״³״×״®״¯… †ˆ״¹ ״§„״¹†״µ״± ״§„״¹„ ״¥† ƒ״§† …״¹״±ˆ״§‹ (…״«„ STRING „„†״µˆ״µ)
                //      ‡״°״§ ״¶…† ״¥†״´״§״¡ alloca ptr „„†״µˆ״µ ״¨״¯„״§‹ …† alloca i64
                //      ……״§ …†״¹ ״×״­ˆ„ ptrtoint ״¹†״¯ ״§„״×״®״²† ˆ״­״§״¸ ״¹„‰ ״§„…״₪״´״± ƒ€ ptr
                // (EN) Use actual element type when known (e.g., STRING for text arrays)
                //      This ensures alloca ptr for strings instead of alloca i64
                {
                    SadTypeKind loopVarType = (iterableResult.elementType != SadTypeKind::Void)
                                                  ? iterableResult.elementType
                                                  : SadTypeKind::Integer;
                    SIRInstruction allocLoop(SIROpcode::ALLOC);
                    allocLoop.result = SIROperand::Register(loopVarAllocName, loopVarType);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(allocLoop);
                }

                // (AR) ״×״³״¬„ …״×״÷״± ״§„״­„‚״© (sir_builder.h:144 - VariableInfo)
                //      †״³״×״®״¯… †ˆ״¹ ״§„״¹†״µ״± ״§„…״³״×†״×״¬ ״¥† ˆ״¬״¯ „״×״µ״­״­ ״§„״·״¨״§״¹״©
                // (EN) Register loop variable using b_.addVariable (sir_builder.h:591)
                //      Use inferred element type when available for correct printing
                VariableInfo varInfo;
                varInfo.name = forRange->variable;
                varInfo.registerName = loopVarAllocName;
                varInfo.type = (iterableResult.elementType != SadTypeKind::Void) ? iterableResult.elementType : SadTypeKind::Integer;
                varInfo.isMutable = true;
                if (!iterableResult.elementClassName.empty())
                {
                    varInfo.className = iterableResult.elementClassName;
                }

                b_.addVariable(varInfo);

                if (!varInfo.className.empty())
                {
                    b_.classInstanceTypes_[forRange->variable] = varInfo.className;
                    b_.classInstanceTypes_[loopVarAllocName] = varInfo.className;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildForRangeLoop: registered loop var '"
                          << forRange->variable << "' alloc=" << loopVarAllocName << std::endl;
#endif

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 6: ‚״² ״÷״± ״´״±״· ״¥„‰ ƒ״×„״© ״§„״´״±״·
                // (EN) Step 6: Unconditional jump to condition block
                // ========================================================================
                SIROperand condLabelOp = SIROperand::Label(condLabel);
                SIRInstruction brCondBlockInst = SIRInstruction::Branch(condLabelOp);

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->instructions.push_back(brCondBlockInst);
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 7: ״¨†״§״¡ ״§„״´״±״· (index < length)
                // (EN) Step 7: Build condition (index < length)
                // ========================================================================
                b_.currentBlock_ = condBlock;

                SIROperand iterOp = SIROperand::Register(iterableResult.registerName, iterableResult.type);

                // LOAD current counter value
                std::string loadedIdxCond = b_.newTempRegister();
                {
                    SIRInstruction loadIdx(SIROpcode::LOAD);
                    loadIdx.result = SIROperand::Register(loadedIdxCond, SadTypeKind::Integer);
                    loadIdx.operands.push_back(SIROperand::Register(indexAllocName, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(loadIdx);
                }

                // ARRAY_LEN
                std::string lengthReg = b_.newTempRegister();
                SIROperand lengthOp = SIROperand::Register(lengthReg, SadTypeKind::Integer);
                {
                    SIRInstruction lenInst(SIROpcode::ARRAY_LEN);
                    lenInst.result = lengthOp;
                    lenInst.operands.push_back(iterOp);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(lenInst);
                }

                // LT: loadedIdxCond < length
                std::string condReg = b_.newTempRegister();
                SIROperand condResultOp = SIROperand::Register(condReg, SadTypeKind::Boolean);
                {
                    SIRInstruction cmpInst = SIRInstruction::Binary(
                        SIROpcode::LT, condResultOp,
                        SIROperand::Register(loadedIdxCond, SadTypeKind::Integer),
                        lengthOp);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(cmpInst);
                }

                // BR_COND ג†’ body / exit
                SIROperand bodyLabelOp = SIROperand::Label(bodyLabel);
                SIROperand exitLabelOp = SIROperand::Label(exitLabel);
                {
                    SIRInstruction brCondInst = SIRInstruction::BranchCond(
                        condResultOp, bodyLabelOp, exitLabelOp);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(brCondInst);
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 8: ״¨†״§״¡ ״¬״³… ״§„״­„‚״©
                // (EN) Step 8: Build loop body
                // ========================================================================
                b_.currentBlock_ = bodyBlock;

                // LOAD current counter value for ARRAY_GET
                std::string loadedIdxBody = b_.newTempRegister();
                {
                    SIRInstruction loadIdxB(SIROpcode::LOAD);
                    loadIdxB.result = SIROperand::Register(loadedIdxBody, SadTypeKind::Integer);
                    loadIdxB.operands.push_back(SIROperand::Register(indexAllocName, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(loadIdxB);
                }

                // ARRAY_GET: loopVar = iterable[loadedIdx]
                // (AR) †״³״×״®״¯… †ˆ״¹ ״§„״¹†״µ״± ״§„…״³״×†״×״¬ …† ״§„…״µˆ״© ״¥† ˆ״¬״¯
                //      ‡״°״§ ״­„ …״´ƒ„״© ״×ƒ״±״§״± ״§„†״µˆ״µ ״§„״× ƒ״§†״× ״×״­…‘„ ƒ״£״±‚״§…
                // (EN) Use inferred element type from array if available
                //      This fixes string iteration being loaded as numbers
                SadTypeKind elemType = SadTypeKind::Integer;
                if (iterableResult.elementType != SadTypeKind::Void)
                {
                    elemType = iterableResult.elementType;
                }
                std::string elemReg = b_.newTempRegister();
                {
                    SIRInstruction loadElem(SIROpcode::ARRAY_GET);
                    loadElem.result = SIROperand::Register(elemReg, elemType);
                    loadElem.operands.push_back(iterOp);
                    loadElem.operands.push_back(SIROperand::Register(loadedIdxBody, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(loadElem);
                }

                // STORE element into loop variable slot
                // (AR) †״³״×״®״¯… †ˆ״¹ ״§„״¹†״µ״± ״§„״¹„ „„״×״®״²† „״×״¬†״¨ ptrtoint ״¹†״¯ ״×״®״²† †״µˆ״µ
                // (EN) Use actual element type for store to avoid ptrtoint when storing strings
                {
                    SIRInstruction storeElem(SIROpcode::STORE);
                    storeElem.operands.push_back(SIROperand::Register(elemReg, elemType));
                    storeElem.operands.push_back(SIROperand::Register(loopVarAllocName, elemType));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(storeElem);
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 8.5: ״×״³״¬„ ״³״§‚ ״§„״­„‚״© „״¯״¹… break/continue
                //      continueLabel = ƒ״×„״© ״§„״²״§״¯״© (foreach_inc) ג€” continue ״²״¯ ״§„״¹״¯״§״¯ ״«… ״­״µ ״§„״´״±״·
                //      breakLabel = ƒ״×„״© ״§„״®״±ˆ״¬ (foreach_exit) ג€” break ‚״² „„״®״±ˆ״¬
                // (EN) Step 8.5: Register loop context for break/continue support
                //      continueLabel = increment block (foreach_inc) ג€” continue increments then checks condition
                //      breakLabel = exit block (foreach_exit) ג€” break jumps to exit
                // ========================================================================
                LoopContext foreachLoopCtx;
                foreachLoopCtx.continueLabel = incLabel;
                foreachLoopCtx.breakLabel = exitLabel;
                b_.enterLoop(foreachLoopCtx);

                // (AR) ״¨†״§״¡ ״¬״³… ״§„״­„‚״©
                // (EN) Build loop body
                if (forRange->body)
                {
                    buildStatement(forRange->body.get());
                }

                // (AR) ״§„״®״±ˆ״¬ …† ״³״§‚ ״§„״­„‚״© (break/continue)
                // (EN) Exit loop context (break/continue)
                b_.exitLoop();

                // (AR) ‚״² ״¥„‰ ƒ״×„״© ״§„״²״§״¯״©
                // (EN) Jump to increment block
                SIROperand incLabelOp = SIROperand::Label(incLabel);
                {
                    SIRInstruction brIncInst = SIRInstruction::Branch(incLabelOp);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(brIncInst);
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 9: ״¨†״§״¡ ״§„״²״§״¯״© (index = index + 1)
                // (EN) Step 9: Build increment (index = index + 1)
                // ========================================================================
                b_.currentBlock_ = incBlock;

                // LOAD counter current value
                std::string loadedIdxInc = b_.newTempRegister();
                {
                    SIRInstruction loadIdxI(SIROpcode::LOAD);
                    loadIdxI.result = SIROperand::Register(loadedIdxInc, SadTypeKind::Integer);
                    loadIdxI.operands.push_back(SIROperand::Register(indexAllocName, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(loadIdxI);
                }

                // ADD: newIdx = loadedIdx + 1
                std::string newIdxReg = b_.newTempRegister();
                {
                    SIRInstruction incInst = SIRInstruction::Binary(
                        SIROpcode::ADD_I64,
                        SIROperand::Register(newIdxReg, SadTypeKind::Integer),
                        SIROperand::Register(loadedIdxInc, SadTypeKind::Integer),
                        SIROperand::ConstantI64(1));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(incInst);
                }

                // STORE newIdx back into counter slot
                {
                    SIRInstruction storeIdx(SIROpcode::STORE);
                    storeIdx.operands.push_back(SIROperand::Register(newIdxReg, SadTypeKind::Integer));
                    storeIdx.operands.push_back(SIROperand::Register(indexAllocName, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(storeIdx);
                }

                // (AR) ‚״² „„״¹ˆ״¯״© ״¥„‰ ״§„״´״±״·
                // (EN) Jump back to condition
                {
                    SIRInstruction brBackInst = SIRInstruction::Branch(condLabelOp);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(brBackInst);
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 10: ״§„״§״³״×…״±״§״± ״¨״¹״¯ ״§„״­„‚״©
                // (EN) Step 10: Continue after loop
                // ========================================================================
                b_.currentBlock_ = exitBlock;

                // (AR) ״§„״®״±ˆ״¬ …† †״·״§‚ ״§„״­„‚״©
                // (EN) Exit loop scope
                b_.exitScope();

#ifndef NDEBUG
                std::cout << "[DEBUG] buildForRangeLoop: completed" << std::endl;
#endif
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
