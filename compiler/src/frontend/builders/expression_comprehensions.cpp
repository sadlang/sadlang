// ============================================================================
// sir_builder_expr_comprehensions.cpp
// ============================================================================
// Comprehension expression builders (list, dict, set comprehensions + generator)
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
            BuildResult ExpressionBuilder::buildExprListComp(AST::ListComprehensionExpr *listCompExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found ListComprehensionExpr" << std::endl;
#endif

                // (AR) تخصيص مصفوفة النتيجة
                // (EN) Allocate result array
                std::string resultArrReg = b_.newTempRegister();
                SIRInstruction allocInst;
                allocInst.opcode = SIROpcode::ALLOC;
                allocInst.result = SIROperand::Register(resultArrReg, SadTypeKind::Array);
                allocInst.operands.push_back(SIROperand::ConstantI64(0));
                allocInst.comment = "list comprehension result";

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(allocInst);
                }

                // (AR) بناء التعبير القابل للتكرار
                // (EN) Build iterable expression
                auto iterResult = buildExpression(listCompExpr->iterable.get());

                // (AR) إنشاء حلقة للتكرار (تُترجم إلى حلقة عداد)
                // (EN) Create iteration loop (lowered to counter loop)
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
                std::string condLabel = b_.newLabel("lc_cond");
                std::string bodyLabel = b_.newLabel("lc_body");
                std::string exitLabel = b_.newLabel("lc_exit");

                auto condBlock = b_.createBasicBlock(condLabel);
                auto bodyBlock = b_.createBasicBlock(bodyLabel);
                auto exitBlock = b_.createBasicBlock(exitLabel);

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(condLabel)));
                }

                // (AR) كتلة الشرط — مقارنة العداد مع طول القائمة
                // (EN) Condition block — compare counter to list length
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(condBlock);
                b_.currentBlock_ = condBlock;

                // (AR) تحميل العداد الحالي
                // (EN) Load current counter
                std::string curIdxReg = b_.newTempRegister();
                SIRInstruction loadIdx;
                loadIdx.opcode = SIROpcode::LOAD;
                loadIdx.result = SIROperand::Register(curIdxReg, SadTypeKind::Integer);
                loadIdx.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(loadIdx);

                // (AR) للتبسيط: نستخدم استدعاء طول runtime
                // (EN) Simplified: use runtime length call
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

                // (AR) تحميل العنصر الحالي وتسجيل متغير الحلقة
                // (EN) Load current element and register loop variable
                std::string elemReg = b_.newTempRegister();
                SIRInstruction loadElem;
                loadElem.opcode = SIROpcode::LOAD;
                loadElem.result = SIROperand::Register(elemReg, SadTypeKind::Integer);
                loadElem.operands.push_back(SIROperand::Register(iterResult.registerName, iterResult.type));
                loadElem.operands.push_back(SIROperand::Register(curIdxReg, SadTypeKind::Integer));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(loadElem);

                VariableInfo loopVar;
                loopVar.name = listCompExpr->variable;
                loopVar.type = SadTypeKind::Integer;
                loopVar.registerName = elemReg;
                loopVar.isMutable = false;
                loopVar.scopeLevel = b_.currentScopeLevel_;
                b_.addVariable(loopVar);

                // (AR) فحص الشرط (إن وجد)
                // (EN) Check condition (if present)
                bool hasCondition = (listCompExpr->condition != nullptr);
                std::string storeLabel, incLabel;

                if (hasCondition)
                {
                    storeLabel = b_.newLabel("lc_store");
                    incLabel = b_.newLabel("lc_inc");

                    auto condResult = buildExpression(listCompExpr->condition.get());
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

                // (AR) بناء تعبير العنصر وإضافته للمصفوفة
                // (EN) Build element expression and append to array
                auto elemExprResult = buildExpression(listCompExpr->element.get());

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
                    // (AR) كتلة زيادة العداد
                    // (EN) Increment block
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

                return BuildResult(resultArrReg, SadTypeKind::Array);
            }

            // ============================================================================
            // buildExprDictComp
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprDictComp(AST::DictComprehensionExpr *dictCompExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found DictComprehensionExpr" << std::endl;
#endif

                // (AR) تخصيص خريطة النتيجة
                // (EN) Allocate result map
                std::string resultMapReg = b_.newTempRegister();
                SIRInstruction allocInst;
                allocInst.opcode = SIROpcode::ALLOC;
                allocInst.result = SIROperand::Register(resultMapReg, SadTypeKind::Struct);
                allocInst.operands.push_back(SIROperand::ConstantI64(0));
                allocInst.comment = "dict comprehension result";
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(allocInst);

                // (AR) بناء التعبير القابل للتكرار
                // (EN) Build iterable expression
                auto iterResult = buildExpression(dictCompExpr->iterable.get());

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
                std::string condLabel = b_.newLabel("dc_cond");
                std::string bodyLabel = b_.newLabel("dc_body");
                std::string exitLabel = b_.newLabel("dc_exit");

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

                // (AR) تحميل العنصر وتسجيل متغير الحلقة
                // (EN) Load element and register loop variable
                std::string elemReg = b_.newTempRegister();
                SIRInstruction loadElem;
                loadElem.opcode = SIROpcode::LOAD;
                loadElem.result = SIROperand::Register(elemReg, SadTypeKind::Integer);
                loadElem.operands.push_back(SIROperand::Register(iterResult.registerName, iterResult.type));
                loadElem.operands.push_back(SIROperand::Register(curIdxReg, SadTypeKind::Integer));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(loadElem);

                VariableInfo loopVar;
                loopVar.name = dictCompExpr->variable;
                loopVar.type = SadTypeKind::Integer;
                loopVar.registerName = elemReg;
                loopVar.isMutable = false;
                loopVar.scopeLevel = b_.currentScopeLevel_;
                b_.addVariable(loopVar);

                // (AR) فحص الشرط (إن وجد)
                // (EN) Check condition (if present)
                bool hasCondition = (dictCompExpr->condition != nullptr);
                std::string storeLabel, incLabel;

                if (hasCondition)
                {
                    storeLabel = b_.newLabel("dc_store");
                    incLabel = b_.newLabel("dc_inc");

                    auto condResult = buildExpression(dictCompExpr->condition.get());
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

                // (AR) بناء تعبيرات المفتاح والقيمة وإضافتها للخريطة
                // (EN) Build key and value expressions and add to map
                auto keyResult = buildExpression(dictCompExpr->key.get());
                auto valResult = buildExpression(dictCompExpr->value.get());

                SIRInstruction setInst;
                setInst.opcode = SIROpcode::CALL;
                setInst.operands.push_back(SIROperand::ConstantString("__sad_dict_set"));
                setInst.operands.push_back(SIROperand::Register(resultMapReg, SadTypeKind::Struct));
                setInst.operands.push_back(SIROperand::Register(keyResult.registerName, keyResult.type));
                setInst.operands.push_back(SIROperand::Register(valResult.registerName, valResult.type));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(setInst);

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

                return BuildResult(resultMapReg, SadTypeKind::Struct);
            }

            // ============================================================================
            // buildExprSetComp
            // ============================================================================
        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
