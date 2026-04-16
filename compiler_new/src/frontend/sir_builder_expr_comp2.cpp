// ============================================================================
// sir_builder_expr_comp2.cpp - SetComp and Generator expression builders
// ============================================================================
#include "sir_builder.h"
#include <set>
#include <functional>
#include <iostream>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            BuildResult SIRBuilder::buildExprSetComp(AST::SetComprehensionExpr *setCompExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found SetComprehensionExpr" << std::endl;
#endif

                // (AR) المجموعة تُمثَّل كمصفوفة بعناصر فريدة (مثل المفسر)
                // (EN) Set represented as array with unique elements (like interpreter)
                std::string resultSetReg = newTempRegister();
                SIRInstruction allocInst;
                allocInst.opcode = SIROpcode::ALLOC;
                allocInst.result = SIROperand::Register(resultSetReg, SadTypeKind::Array);
                allocInst.operands.push_back(SIROperand::ConstantI64(0));
                allocInst.comment = "set comprehension result";
                if (currentBlock_)
                    currentBlock_->addInstruction(allocInst);

                // (AR) بناء التعبير القابل للتكرار
                // (EN) Build iterable expression
                auto iterResult = buildExpression(setCompExpr->iterable.get());

                // (AR) تخصيص عداد الحلقة
                // (EN) Allocate loop counter
                std::string idxReg = newTempRegister();
                SIRInstruction allocIdx;
                allocIdx.opcode = SIROpcode::ALLOC;
                allocIdx.result = SIROperand::Register(idxReg, SadTypeKind::Integer);
                allocIdx.operands.push_back(SIROperand::ConstantI64(1));
                if (currentBlock_)
                    currentBlock_->addInstruction(allocIdx);

                SIRInstruction storeZero;
                storeZero.opcode = SIROpcode::STORE;
                storeZero.operands.push_back(SIROperand::ConstantI64(0));
                storeZero.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                if (currentBlock_)
                    currentBlock_->addInstruction(storeZero);

                // (AR) كتل الحلقة
                // (EN) Loop blocks
                std::string condLabel = newLabel("sc_cond");
                std::string bodyLabel = newLabel("sc_body");
                std::string exitLabel = newLabel("sc_exit");

                auto condBlock = createBasicBlock(condLabel);
                auto bodyBlock = createBasicBlock(bodyLabel);
                auto exitBlock = createBasicBlock(exitLabel);

                if (currentBlock_)
                {
                    currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(condLabel)));
                }

                // (AR) كتلة الشرط
                // (EN) Condition block
                if (currentFunction_)
                    currentFunction_->addBasicBlock(condBlock);
                currentBlock_ = condBlock;

                std::string curIdxReg = newTempRegister();
                SIRInstruction loadIdx;
                loadIdx.opcode = SIROpcode::LOAD;
                loadIdx.result = SIROperand::Register(curIdxReg, SadTypeKind::Integer);
                loadIdx.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                if (currentBlock_)
                    currentBlock_->addInstruction(loadIdx);

                std::string lenReg = newTempRegister();
                SIRInstruction callLen;
                callLen.opcode = SIROpcode::CALL;
                callLen.result = SIROperand::Register(lenReg, SadTypeKind::Integer);
                callLen.operands.push_back(SIROperand::ConstantString("__sad_len"));
                callLen.operands.push_back(SIROperand::Register(iterResult.registerName, iterResult.type));
                if (currentBlock_)
                    currentBlock_->addInstruction(callLen);

                std::string cmpReg = newTempRegister();
                if (currentBlock_)
                {
                    currentBlock_->addInstruction(SIRInstruction::Binary(
                        SIROpcode::LT,
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Register(curIdxReg, SadTypeKind::Integer),
                        SIROperand::Register(lenReg, SadTypeKind::Integer)));
                    currentBlock_->addInstruction(SIRInstruction::BranchCond(
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Label(bodyLabel),
                        SIROperand::Label(exitLabel)));
                }

                // (AR) كتلة الجسم
                // (EN) Body block
                if (currentFunction_)
                    currentFunction_->addBasicBlock(bodyBlock);
                currentBlock_ = bodyBlock;

                enterScope();

                std::string elemReg = newTempRegister();
                SIRInstruction loadElem;
                loadElem.opcode = SIROpcode::LOAD;
                loadElem.result = SIROperand::Register(elemReg, SadTypeKind::Integer);
                loadElem.operands.push_back(SIROperand::Register(iterResult.registerName, iterResult.type));
                loadElem.operands.push_back(SIROperand::Register(curIdxReg, SadTypeKind::Integer));
                if (currentBlock_)
                    currentBlock_->addInstruction(loadElem);

                VariableInfo loopVar;
                loopVar.name = setCompExpr->variable;
                loopVar.type = SadTypeKind::Integer;
                loopVar.registerName = elemReg;
                loopVar.isMutable = false;
                loopVar.scopeLevel = currentScopeLevel_;
                addVariable(loopVar);

                // (AR) فحص الشرط (إن وجد)
                // (EN) Check condition (if present)
                bool hasCondition = (setCompExpr->condition != nullptr);
                std::string storeLabel, incLabel;

                if (hasCondition)
                {
                    storeLabel = newLabel("sc_store");
                    incLabel = newLabel("sc_inc");

                    auto condResult = buildExpression(setCompExpr->condition.get());
                    auto storeBlock2 = createBasicBlock(storeLabel);
                    auto incBlock = createBasicBlock(incLabel);

                    if (currentBlock_)
                    {
                        currentBlock_->addInstruction(SIRInstruction::BranchCond(
                            SIROperand::Register(condResult.registerName, SadTypeKind::Boolean),
                            SIROperand::Label(storeLabel),
                            SIROperand::Label(incLabel)));
                    }

                    if (currentFunction_)
                        currentFunction_->addBasicBlock(storeBlock2);
                    currentBlock_ = storeBlock2;
                }

                // (AR) بناء التعبير وإضافته للمجموعة (بدون تكرار عبر runtime)
                // (EN) Build expression and add to set (dedup via runtime)
                auto elemExprResult = buildExpression(setCompExpr->expression.get());

                SIRInstruction appendInst;
                appendInst.opcode = SIROpcode::CALL;
                appendInst.operands.push_back(SIROperand::ConstantString("__sad_set_add"));
                appendInst.operands.push_back(SIROperand::Register(resultSetReg, SadTypeKind::Array));
                appendInst.operands.push_back(SIROperand::Register(elemExprResult.registerName, elemExprResult.type));
                if (currentBlock_)
                    currentBlock_->addInstruction(appendInst);

                if (hasCondition)
                {
                    if (currentBlock_)
                    {
                        currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(incLabel)));
                    }
                    auto incBlock2 = createBasicBlock(incLabel);
                    if (currentFunction_)
                        currentFunction_->addBasicBlock(incBlock2);
                    currentBlock_ = incBlock2;
                }

                // (AR) زيادة العداد
                // (EN) Increment counter
                std::string nextIdxReg = newTempRegister();
                if (currentBlock_)
                {
                    currentBlock_->addInstruction(SIRInstruction::Binary(
                        SIROpcode::ADD_I64,
                        SIROperand::Register(nextIdxReg, SadTypeKind::Integer),
                        SIROperand::Register(curIdxReg, SadTypeKind::Integer),
                        SIROperand::ConstantI64(1)));

                    SIRInstruction storeIdx;
                    storeIdx.opcode = SIROpcode::STORE;
                    storeIdx.operands.push_back(SIROperand::Register(nextIdxReg, SadTypeKind::Integer));
                    storeIdx.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                    currentBlock_->addInstruction(storeIdx);
                    currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(condLabel)));
                }

                exitScope();

                // (AR) كتلة الخروج
                // (EN) Exit block
                if (currentFunction_)
                    currentFunction_->addBasicBlock(exitBlock);
                currentBlock_ = exitBlock;

                return BuildResult(resultSetReg, SadTypeKind::Array);
            }

            // ============================================================================
            // buildExprGenerator
            // ============================================================================
            BuildResult SIRBuilder::buildExprGenerator(AST::GeneratorExpr *genExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found GeneratorExpr" << std::endl;
#endif

                // (AR) المولّد يُقيَّم بشكل كامل كمصفوفة (مثل المفسر)
                // (EN) Generator eagerly evaluated as array (matching interpreter)
                std::string resultArrReg = newTempRegister();
                SIRInstruction allocInst;
                allocInst.opcode = SIROpcode::ALLOC;
                allocInst.result = SIROperand::Register(resultArrReg, SadTypeKind::Array);
                allocInst.operands.push_back(SIROperand::ConstantI64(0));
                allocInst.comment = "generator expression result";
                if (currentBlock_)
                    currentBlock_->addInstruction(allocInst);

                auto iterResult = buildExpression(genExpr->iterable.get());

                std::string idxReg = newTempRegister();
                SIRInstruction allocIdx;
                allocIdx.opcode = SIROpcode::ALLOC;
                allocIdx.result = SIROperand::Register(idxReg, SadTypeKind::Integer);
                allocIdx.operands.push_back(SIROperand::ConstantI64(1));
                if (currentBlock_)
                    currentBlock_->addInstruction(allocIdx);

                SIRInstruction storeZero;
                storeZero.opcode = SIROpcode::STORE;
                storeZero.operands.push_back(SIROperand::ConstantI64(0));
                storeZero.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                if (currentBlock_)
                    currentBlock_->addInstruction(storeZero);

                std::string condLabel = newLabel("gen_cond");
                std::string bodyLabel = newLabel("gen_body");
                std::string exitLabel = newLabel("gen_exit");

                auto condBlock = createBasicBlock(condLabel);
                auto bodyBlock = createBasicBlock(bodyLabel);
                auto exitBlock = createBasicBlock(exitLabel);

                if (currentBlock_)
                {
                    currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(condLabel)));
                }

                if (currentFunction_)
                    currentFunction_->addBasicBlock(condBlock);
                currentBlock_ = condBlock;

                std::string curIdxReg = newTempRegister();
                SIRInstruction loadIdx;
                loadIdx.opcode = SIROpcode::LOAD;
                loadIdx.result = SIROperand::Register(curIdxReg, SadTypeKind::Integer);
                loadIdx.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                if (currentBlock_)
                    currentBlock_->addInstruction(loadIdx);

                std::string lenReg = newTempRegister();
                SIRInstruction callLen;
                callLen.opcode = SIROpcode::CALL;
                callLen.result = SIROperand::Register(lenReg, SadTypeKind::Integer);
                callLen.operands.push_back(SIROperand::ConstantString("__sad_len"));
                callLen.operands.push_back(SIROperand::Register(iterResult.registerName, iterResult.type));
                if (currentBlock_)
                    currentBlock_->addInstruction(callLen);

                std::string cmpReg = newTempRegister();
                if (currentBlock_)
                {
                    currentBlock_->addInstruction(SIRInstruction::Binary(
                        SIROpcode::LT,
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Register(curIdxReg, SadTypeKind::Integer),
                        SIROperand::Register(lenReg, SadTypeKind::Integer)));
                    currentBlock_->addInstruction(SIRInstruction::BranchCond(
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Label(bodyLabel),
                        SIROperand::Label(exitLabel)));
                }

                if (currentFunction_)
                    currentFunction_->addBasicBlock(bodyBlock);
                currentBlock_ = bodyBlock;

                enterScope();

                std::string elemReg = newTempRegister();
                SIRInstruction loadElem;
                loadElem.opcode = SIROpcode::LOAD;
                loadElem.result = SIROperand::Register(elemReg, SadTypeKind::Integer);
                loadElem.operands.push_back(SIROperand::Register(iterResult.registerName, iterResult.type));
                loadElem.operands.push_back(SIROperand::Register(curIdxReg, SadTypeKind::Integer));
                if (currentBlock_)
                    currentBlock_->addInstruction(loadElem);

                VariableInfo loopVar;
                loopVar.name = genExpr->variable;
                loopVar.type = SadTypeKind::Integer;
                loopVar.registerName = elemReg;
                loopVar.isMutable = false;
                loopVar.scopeLevel = currentScopeLevel_;
                addVariable(loopVar);

                bool hasCondition = (genExpr->condition != nullptr);
                std::string storeLabel, incLabel;

                if (hasCondition)
                {
                    storeLabel = newLabel("gen_store");
                    incLabel = newLabel("gen_inc");

                    auto condResult = buildExpression(genExpr->condition.get());
                    auto storeBlock2 = createBasicBlock(storeLabel);
                    auto incBlock = createBasicBlock(incLabel);

                    if (currentBlock_)
                    {
                        currentBlock_->addInstruction(SIRInstruction::BranchCond(
                            SIROperand::Register(condResult.registerName, SadTypeKind::Boolean),
                            SIROperand::Label(storeLabel),
                            SIROperand::Label(incLabel)));
                    }

                    if (currentFunction_)
                        currentFunction_->addBasicBlock(storeBlock2);
                    currentBlock_ = storeBlock2;
                }

                auto elemExprResult = buildExpression(genExpr->element.get());

                SIRInstruction appendInst;
                appendInst.opcode = SIROpcode::CALL;
                appendInst.operands.push_back(SIROperand::ConstantString("__sad_array_push"));
                appendInst.operands.push_back(SIROperand::Register(resultArrReg, SadTypeKind::Array));
                appendInst.operands.push_back(SIROperand::Register(elemExprResult.registerName, elemExprResult.type));
                if (currentBlock_)
                    currentBlock_->addInstruction(appendInst);

                if (hasCondition)
                {
                    if (currentBlock_)
                    {
                        currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(incLabel)));
                    }
                    auto incBlock2 = createBasicBlock(incLabel);
                    if (currentFunction_)
                        currentFunction_->addBasicBlock(incBlock2);
                    currentBlock_ = incBlock2;
                }

                std::string nextIdxReg = newTempRegister();
                if (currentBlock_)
                {
                    currentBlock_->addInstruction(SIRInstruction::Binary(
                        SIROpcode::ADD_I64,
                        SIROperand::Register(nextIdxReg, SadTypeKind::Integer),
                        SIROperand::Register(curIdxReg, SadTypeKind::Integer),
                        SIROperand::ConstantI64(1)));

                    SIRInstruction storeIdx;
                    storeIdx.opcode = SIROpcode::STORE;
                    storeIdx.operands.push_back(SIROperand::Register(nextIdxReg, SadTypeKind::Integer));
                    storeIdx.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                    currentBlock_->addInstruction(storeIdx);
                    currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(condLabel)));
                }

                exitScope();

                if (currentFunction_)
                    currentFunction_->addBasicBlock(exitBlock);
                currentBlock_ = exitBlock;

                return BuildResult(resultArrReg, SadTypeKind::Array);
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
