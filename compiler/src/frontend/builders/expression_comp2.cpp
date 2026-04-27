// ============================================================================
// sir_builder_expr_comp2.cpp - SetComp and Generator expression builders
// ============================================================================
#include "sir_builder.h"
#include "builders/expression_builder.h"
#include <set>
#include <functional>
#include <iostream>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            BuildResult ExpressionBuilder::buildExprSetComp(AST::SetComprehensionExpr *setCompExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found SetComprehensionExpr" << std::endl;
#endif

                // (AR) المجموعة تُمثَّل كمصفوفة بعناصر فريدة (مثل المفسر)
                // (EN) Set represented as array with unique elements (like interpreter)
                std::string resultSetReg = b_.newTempRegister();
                SIRInstruction allocInst;
                allocInst.opcode = SIROpcode::ALLOC;
                allocInst.result = SIROperand::Register(resultSetReg, SadTypeKind::Array);
                allocInst.operands.push_back(SIROperand::ConstantI64(0));
                allocInst.comment = "set comprehension result";
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(allocInst);

                // (AR) بناء التعبير القابل للتكرار
                // (EN) Build iterable expression
                auto iterResult = buildExpression(setCompExpr->iterable.get());

                // (AR) تخصيص عداد الحلقة
                // (EN) Allocate loop counter
                std::string idxReg = b_.newTempRegister();
                SIRInstruction allocIdx;
                allocIdx.opcode = SIROpcode::ALLOC;
                allocIdx.result = SIROperand::Register(idxReg, SadTypeKind::Integer);
                allocIdx.operands.push_back(SIROperand::ConstantI64(1));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(allocIdx);

                SIRInstruction storeZero;
                storeZero.opcode = SIROpcode::STORE;
                storeZero.operands.push_back(SIROperand::ConstantI64(0));
                storeZero.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(storeZero);

                // (AR) كتل الحلقة
                // (EN) Loop blocks
                std::string condLabel = b_.newLabel("sc_cond");
                std::string bodyLabel = b_.newLabel("sc_body");
                std::string exitLabel = b_.newLabel("sc_exit");

                auto condBlock = b_.createBasicBlock(condLabel);
                auto bodyBlock = b_.createBasicBlock(bodyLabel);
                auto exitBlock = b_.createBasicBlock(exitLabel);

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(condLabel)));
                }

                // (AR) كتلة الشرط
                // (EN) Condition block
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(condBlock);
                b_.currentBlock_ = condBlock;

                std::string curIdxReg = b_.newTempRegister();
                SIRInstruction loadIdx;
                loadIdx.opcode = SIROpcode::LOAD;
                loadIdx.result = SIROperand::Register(curIdxReg, SadTypeKind::Integer);
                loadIdx.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(loadIdx);

                std::string lenReg = b_.newTempRegister();
                SIRInstruction callLen;
                callLen.opcode = SIROpcode::CALL;
                callLen.result = SIROperand::Register(lenReg, SadTypeKind::Integer);
                callLen.operands.push_back(SIROperand::ConstantString("__sad_len"));
                callLen.operands.push_back(SIROperand::Register(iterResult.registerName, iterResult.type));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(callLen);

                std::string cmpReg = b_.newTempRegister();
                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                        SIROpcode::LT,
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Register(curIdxReg, SadTypeKind::Integer),
                        SIROperand::Register(lenReg, SadTypeKind::Integer)));
                    b_.currentBlock_->addInstruction(SIRInstruction::BranchCond(
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Label(bodyLabel),
                        SIROperand::Label(exitLabel)));
                }

                // (AR) كتلة الجسم
                // (EN) Body block
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(bodyBlock);
                b_.currentBlock_ = bodyBlock;

                b_.enterScope();

                std::string elemReg = b_.newTempRegister();
                SIRInstruction loadElem;
                loadElem.opcode = SIROpcode::LOAD;
                loadElem.result = SIROperand::Register(elemReg, SadTypeKind::Integer);
                loadElem.operands.push_back(SIROperand::Register(iterResult.registerName, iterResult.type));
                loadElem.operands.push_back(SIROperand::Register(curIdxReg, SadTypeKind::Integer));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(loadElem);

                VariableInfo loopVar;
                loopVar.name = setCompExpr->variable;
                loopVar.type = SadTypeKind::Integer;
                loopVar.registerName = elemReg;
                loopVar.isMutable = false;
                loopVar.scopeLevel = b_.currentScopeLevel_;
                b_.addVariable(loopVar);

                // (AR) فحص الشرط (إن وجد)
                // (EN) Check condition (if present)
                bool hasCondition = (setCompExpr->condition != nullptr);
                std::string storeLabel, incLabel;

                if (hasCondition)
                {
                    storeLabel = b_.newLabel("sc_store");
                    incLabel = b_.newLabel("sc_inc");

                    auto condResult = buildExpression(setCompExpr->condition.get());
                    auto storeBlock2 = b_.createBasicBlock(storeLabel);
                    auto incBlock = b_.createBasicBlock(incLabel);

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(SIRInstruction::BranchCond(
                            SIROperand::Register(condResult.registerName, SadTypeKind::Boolean),
                            SIROperand::Label(storeLabel),
                            SIROperand::Label(incLabel)));
                    }

                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(storeBlock2);
                    b_.currentBlock_ = storeBlock2;
                }

                // (AR) بناء التعبير وإضافته للمجموعة (بدون تكرار عبر runtime)
                // (EN) Build expression and add to set (dedup via runtime)
                auto elemExprResult = buildExpression(setCompExpr->expression.get());

                SIRInstruction appendInst;
                appendInst.opcode = SIROpcode::CALL;
                appendInst.operands.push_back(SIROperand::ConstantString("__sad_set_add"));
                appendInst.operands.push_back(SIROperand::Register(resultSetReg, SadTypeKind::Array));
                appendInst.operands.push_back(SIROperand::Register(elemExprResult.registerName, elemExprResult.type));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(appendInst);

                if (hasCondition)
                {
                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(incLabel)));
                    }
                    auto incBlock2 = b_.createBasicBlock(incLabel);
                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(incBlock2);
                    b_.currentBlock_ = incBlock2;
                }

                // (AR) زيادة العداد
                // (EN) Increment counter
                std::string nextIdxReg = b_.newTempRegister();
                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                        SIROpcode::ADD_I64,
                        SIROperand::Register(nextIdxReg, SadTypeKind::Integer),
                        SIROperand::Register(curIdxReg, SadTypeKind::Integer),
                        SIROperand::ConstantI64(1)));

                    SIRInstruction storeIdx;
                    storeIdx.opcode = SIROpcode::STORE;
                    storeIdx.operands.push_back(SIROperand::Register(nextIdxReg, SadTypeKind::Integer));
                    storeIdx.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                    b_.currentBlock_->addInstruction(storeIdx);
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(condLabel)));
                }

                b_.exitScope();

                // (AR) كتلة الخروج
                // (EN) Exit block
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(exitBlock);
                b_.currentBlock_ = exitBlock;

                return BuildResult(resultSetReg, SadTypeKind::Array);
            }

            // ============================================================================
            // buildExprGenerator
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprGenerator(AST::GeneratorExpr *genExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found GeneratorExpr" << std::endl;
#endif

                // (AR) المولّد يُقيَّم بشكل كامل كمصفوفة (مثل المفسر)
                // (EN) Generator eagerly evaluated as array (matching interpreter)
                std::string resultArrReg = b_.newTempRegister();
                SIRInstruction allocInst;
                allocInst.opcode = SIROpcode::ALLOC;
                allocInst.result = SIROperand::Register(resultArrReg, SadTypeKind::Array);
                allocInst.operands.push_back(SIROperand::ConstantI64(0));
                allocInst.comment = "generator expression result";
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(allocInst);

                auto iterResult = buildExpression(genExpr->iterable.get());

                std::string idxReg = b_.newTempRegister();
                SIRInstruction allocIdx;
                allocIdx.opcode = SIROpcode::ALLOC;
                allocIdx.result = SIROperand::Register(idxReg, SadTypeKind::Integer);
                allocIdx.operands.push_back(SIROperand::ConstantI64(1));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(allocIdx);

                SIRInstruction storeZero;
                storeZero.opcode = SIROpcode::STORE;
                storeZero.operands.push_back(SIROperand::ConstantI64(0));
                storeZero.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(storeZero);

                std::string condLabel = b_.newLabel("gen_cond");
                std::string bodyLabel = b_.newLabel("gen_body");
                std::string exitLabel = b_.newLabel("gen_exit");

                auto condBlock = b_.createBasicBlock(condLabel);
                auto bodyBlock = b_.createBasicBlock(bodyLabel);
                auto exitBlock = b_.createBasicBlock(exitLabel);

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(condLabel)));
                }

                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(condBlock);
                b_.currentBlock_ = condBlock;

                std::string curIdxReg = b_.newTempRegister();
                SIRInstruction loadIdx;
                loadIdx.opcode = SIROpcode::LOAD;
                loadIdx.result = SIROperand::Register(curIdxReg, SadTypeKind::Integer);
                loadIdx.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(loadIdx);

                std::string lenReg = b_.newTempRegister();
                SIRInstruction callLen;
                callLen.opcode = SIROpcode::CALL;
                callLen.result = SIROperand::Register(lenReg, SadTypeKind::Integer);
                callLen.operands.push_back(SIROperand::ConstantString("__sad_len"));
                callLen.operands.push_back(SIROperand::Register(iterResult.registerName, iterResult.type));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(callLen);

                std::string cmpReg = b_.newTempRegister();
                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                        SIROpcode::LT,
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Register(curIdxReg, SadTypeKind::Integer),
                        SIROperand::Register(lenReg, SadTypeKind::Integer)));
                    b_.currentBlock_->addInstruction(SIRInstruction::BranchCond(
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Label(bodyLabel),
                        SIROperand::Label(exitLabel)));
                }

                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(bodyBlock);
                b_.currentBlock_ = bodyBlock;

                b_.enterScope();

                std::string elemReg = b_.newTempRegister();
                SIRInstruction loadElem;
                loadElem.opcode = SIROpcode::LOAD;
                loadElem.result = SIROperand::Register(elemReg, SadTypeKind::Integer);
                loadElem.operands.push_back(SIROperand::Register(iterResult.registerName, iterResult.type));
                loadElem.operands.push_back(SIROperand::Register(curIdxReg, SadTypeKind::Integer));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(loadElem);

                VariableInfo loopVar;
                loopVar.name = genExpr->variable;
                loopVar.type = SadTypeKind::Integer;
                loopVar.registerName = elemReg;
                loopVar.isMutable = false;
                loopVar.scopeLevel = b_.currentScopeLevel_;
                b_.addVariable(loopVar);

                bool hasCondition = (genExpr->condition != nullptr);
                std::string storeLabel, incLabel;

                if (hasCondition)
                {
                    storeLabel = b_.newLabel("gen_store");
                    incLabel = b_.newLabel("gen_inc");

                    auto condResult = buildExpression(genExpr->condition.get());
                    auto storeBlock2 = b_.createBasicBlock(storeLabel);
                    auto incBlock = b_.createBasicBlock(incLabel);

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(SIRInstruction::BranchCond(
                            SIROperand::Register(condResult.registerName, SadTypeKind::Boolean),
                            SIROperand::Label(storeLabel),
                            SIROperand::Label(incLabel)));
                    }

                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(storeBlock2);
                    b_.currentBlock_ = storeBlock2;
                }

                auto elemExprResult = buildExpression(genExpr->element.get());

                SIRInstruction appendInst;
                appendInst.opcode = SIROpcode::CALL;
                appendInst.operands.push_back(SIROperand::ConstantString("__sad_array_push"));
                appendInst.operands.push_back(SIROperand::Register(resultArrReg, SadTypeKind::Array));
                appendInst.operands.push_back(SIROperand::Register(elemExprResult.registerName, elemExprResult.type));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(appendInst);

                if (hasCondition)
                {
                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(incLabel)));
                    }
                    auto incBlock2 = b_.createBasicBlock(incLabel);
                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(incBlock2);
                    b_.currentBlock_ = incBlock2;
                }

                std::string nextIdxReg = b_.newTempRegister();
                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                        SIROpcode::ADD_I64,
                        SIROperand::Register(nextIdxReg, SadTypeKind::Integer),
                        SIROperand::Register(curIdxReg, SadTypeKind::Integer),
                        SIROperand::ConstantI64(1)));

                    SIRInstruction storeIdx;
                    storeIdx.opcode = SIROpcode::STORE;
                    storeIdx.operands.push_back(SIROperand::Register(nextIdxReg, SadTypeKind::Integer));
                    storeIdx.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                    b_.currentBlock_->addInstruction(storeIdx);
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(condLabel)));
                }

                b_.exitScope();

                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(exitBlock);
                b_.currentBlock_ = exitBlock;

                return BuildResult(resultArrReg, SadTypeKind::Array);
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
