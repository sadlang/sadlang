// ============================================================================
// sir_builder.cpp - بناء SIR من AST / SIR Builder from AST
// ============================================================================
// المؤلف / Author: Sad Compiler Team
// التاريخ / Date: January 5, 2026
// الإصدار / Version: 2.0 (إعادة كتابة كاملة من الصفر)
//
// ملاحظة هامة / Important Note:
// هذا الملف مُعاد كتابته بالكامل من الصفر بما يتوافق 100% مع:
// This file is completely rewritten from scratch to comply 100% with:
// - STRICT_CODING_RULES.md
// - sir_builder.h
// - sir_types.h
// - sir_instruction.h
// - sir_module.h
// - AST headers (ast_node.h, expressions.h, statements.h, declarations.h)
// ============================================================================

#include <string>
#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            BuildResult SIRBuilder::buildDeferredClosure(AST::Statement *stmt)
            {
                if (!stmt)
                {
                    return BuildResult();
                }

                // (AR) نعيد استخدام نظام اللامدا/الإغلاق الحالي لكن بجسم جملة خام
                //      لأن defer يحتاج التقاط المتغيرات وقت التنفيذ داخل الحلقة أو قبل الخروج.
                //      القائمة الثابتة من AST لا تكفي لهذه الحالة.
                // (EN) Reuse the existing lambda/closure system with a raw statement body
                //      because defer must capture runtime values inside loops or before exit.
                std::string lambdaName = "__defer_" + std::to_string(nextTempRegister_++);

                std::set<std::string> freeVars;
                std::set<std::string> boundNames;
                collectFreeVarsStmt(stmt, boundNames, freeVars);

                std::vector<CaptureInfo> captures;
                for (const auto &freeVar : freeVars)
                {
                    auto *varPtr = lookupVariable(freeVar);
                    if (!varPtr)
                    {
                        continue;
                    }

                    CaptureInfo captureInfo;
                    captureInfo.varName = freeVar;
                    captureInfo.registerName = varPtr->registerName;
                    captureInfo.type = varPtr->type;
                    captures.push_back(captureInfo);
                }

                if (!captures.empty())
                {
                    closureCaptures_[lambdaName] = captures;
                }

                std::vector<SIRParameter> sirParams;
                sirParams.push_back(SIRParameter("__env", SadTypeKind::Integer));

                auto lambdaFunc = std::make_shared<SIRFunction>(lambdaName, SadTypeKind::Void);
                lambdaFunc->addParameter(sirParams[0]);

                auto savedCtx = saveContext();

                currentFunction_ = lambdaFunc;
                auto entryBlock = createBasicBlock("defer_entry");
                lambdaFunc->addBasicBlock(entryBlock);
                currentBlock_ = entryBlock;

                enterScope();

                for (size_t i = 0; i < captures.size(); ++i)
                {
                    std::string loadReg = newTempRegister();
                    SIRInstruction envLoadInst;
                    envLoadInst.opcode = SIROpcode::ENV_LOAD;
                    envLoadInst.result = SIROperand::Register(loadReg, captures[i].type);
                    envLoadInst.operands.push_back(SIROperand::Register("%__env", SadTypeKind::Integer));
                    envLoadInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(i)));
                    if (currentBlock_)
                    {
                        currentBlock_->addInstruction(envLoadInst);
                    }

                    std::string allocaName = "%__defer_cap_" + captures[i].varName + "_" + std::to_string(i);
                    SIRInstruction storeInit;
                    storeInit.opcode = SIROpcode::STORE;
                    storeInit.operands.push_back(SIROperand::Register(loadReg, captures[i].type));
                    storeInit.operands.push_back(SIROperand::Register(allocaName, captures[i].type));
                    storeInit.comment = "init deferred capture from env[" + std::to_string(i) + "]";
                    if (currentBlock_)
                    {
                        currentBlock_->addInstruction(storeInit);
                    }

                    VariableInfo capVar;
                    capVar.name = captures[i].varName;
                    capVar.type = captures[i].type;
                    capVar.registerName = allocaName;
                    capVar.isMutable = true;
                    capVar.scopeLevel = currentScopeLevel_;
                    capVar.isCaptured = true;
                    capVar.captureIndex = static_cast<int>(i);
                    capVar.envRegister = "%__env";
                    addVariable(capVar);
                }

                buildStatement(stmt);

                if (currentBlock_)
                {
                    bool needsRetVoid = currentBlock_->instructions.empty();
                    if (!needsRetVoid)
                    {
                        auto lastOpcode = currentBlock_->instructions.back().opcode;
                        needsRetVoid = (lastOpcode != SIROpcode::RET &&
                                        lastOpcode != SIROpcode::RET_VOID &&
                                        lastOpcode != SIROpcode::CORO_RETURN &&
                                        lastOpcode != SIROpcode::BR &&
                                        lastOpcode != SIROpcode::BR_COND);
                    }

                    if (needsRetVoid)
                    {
                        SIRInstruction retVoid;
                        retVoid.opcode = SIROpcode::RET_VOID;
                        currentBlock_->addInstruction(retVoid);
                    }
                }

                exitScope();

                if (module_)
                {
                    module_->addFunction(lambdaFunc);
                }

                FunctionInfo lambdaInfo;
                lambdaInfo.name = lambdaName;
                lambdaInfo.returnType = SadTypeKind::Void;
                lambdaInfo.parameters = sirParams;
                functionTable_[lambdaName] = lambdaInfo;

                restoreContext(std::move(savedCtx));

                std::string closureReg = newTempRegister();
                SIRInstruction closureInst;
                closureInst.opcode = SIROpcode::CLOSURE_CREATE;
                closureInst.result = SIROperand::Register(closureReg, SadTypeKind::Function);
                closureInst.operands.push_back(SIROperand::Function(lambdaName));

                for (const auto &capture : captures)
                {
                    VariableInfo *capVar = lookupVariable(capture.varName);
                    if (capVar)
                    {
                        std::string capLoadReg = newTempRegister();
                        SIRInstruction capLoadInst;
                        capLoadInst.opcode = SIROpcode::LOAD;
                        capLoadInst.result = SIROperand::Register(capLoadReg, capVar->type);
                        capLoadInst.operands.push_back(SIROperand::Register(capVar->registerName, capVar->type));
                        if (currentBlock_)
                        {
                            currentBlock_->addInstruction(capLoadInst);
                        }
                        closureInst.operands.push_back(SIROperand::Register(capLoadReg, capVar->type));
                    }
                    else
                    {
                        closureInst.operands.push_back(SIROperand::Register(capture.registerName, capture.type));
                    }
                }

                if (currentBlock_)
                {
                    currentBlock_->addInstruction(closureInst);
                }

                BuildResult result(closureReg, SadTypeKind::Function);
                result.constantValue = lambdaName;
                result.closureLambdaName = lambdaName;
                result.isConstant = false;
                return result;
            }

            void SIRBuilder::emitRunDeferredClosures()
            {
                if (!currentFunction_ || !currentBlock_ || currentDeferStackReg_.empty() || currentDeferExecutedFlagReg_.empty())
                {
                    return;
                }

                std::string flagLoadReg = newTempRegister();
                SIRInstruction loadFlag;
                loadFlag.opcode = SIROpcode::LOAD;
                loadFlag.result = SIROperand::Register(flagLoadReg, SadTypeKind::Integer);
                loadFlag.operands.push_back(SIROperand::Register(currentDeferExecutedFlagReg_, SadTypeKind::Integer));
                loadFlag.comment = "load defer executed flag";
                currentBlock_->addInstruction(loadFlag);

                std::string flagCmpReg = newTempRegister();
                SIRInstruction cmpFlag = SIRInstruction::Binary(
                    SIROpcode::NE,
                    SIROperand::Register(flagCmpReg, SadTypeKind::Boolean),
                    SIROperand::Register(flagLoadReg, SadTypeKind::Integer),
                    SIROperand::ConstantI64(0));
                cmpFlag.comment = "defer already executed?";
                currentBlock_->addInstruction(cmpFlag);

                std::string runLabel = newLabel("defer_run");
                std::string skipLabel = newLabel("defer_skip");
                auto runBlock = createBasicBlock(runLabel);
                auto skipBlock = createBasicBlock(skipLabel);
                currentFunction_->addBasicBlock(runBlock);
                currentFunction_->addBasicBlock(skipBlock);

                currentBlock_->addInstruction(SIRInstruction::BranchCond(
                    SIROperand::Register(flagCmpReg, SadTypeKind::Boolean),
                    SIROperand::Label(skipLabel),
                    SIROperand::Label(runLabel)));

                currentBlock_ = runBlock;

                SIRInstruction storeFlag;
                storeFlag.opcode = SIROpcode::STORE;
                storeFlag.operands.push_back(SIROperand::ConstantI64(1));
                storeFlag.operands.push_back(SIROperand::Register(currentDeferExecutedFlagReg_, SadTypeKind::Integer));
                storeFlag.comment = "mark deferred closures executed";
                currentBlock_->addInstruction(storeFlag);

                std::string lenReg = newTempRegister();
                SIRInstruction lenInst;
                lenInst.opcode = SIROpcode::ARRAY_LEN;
                lenInst.result = SIROperand::Register(lenReg, SadTypeKind::Integer);
                lenInst.operands.push_back(SIROperand::Register(currentDeferStackReg_, SadTypeKind::Array));
                lenInst.comment = "load defer stack length";
                currentBlock_->addInstruction(lenInst);

                std::string idxAllocaReg = newTempRegister();
                SIRInstruction allocIdx;
                allocIdx.opcode = SIROpcode::ALLOC;
                allocIdx.result = SIROperand::Register(idxAllocaReg, SadTypeKind::Integer);
                allocIdx.comment = "alloca defer reverse index";
                currentBlock_->addInstruction(allocIdx);

                std::string startIdxReg = newTempRegister();
                SIRInstruction initIdx = SIRInstruction::Binary(
                    SIROpcode::SUB_I64,
                    SIROperand::Register(startIdxReg, SadTypeKind::Integer),
                    SIROperand::Register(lenReg, SadTypeKind::Integer),
                    SIROperand::ConstantI64(1));
                initIdx.comment = "start defer index = len - 1";
                currentBlock_->addInstruction(initIdx);

                SIRInstruction storeIdx;
                storeIdx.opcode = SIROpcode::STORE;
                storeIdx.operands.push_back(SIROperand::Register(startIdxReg, SadTypeKind::Integer));
                storeIdx.operands.push_back(SIROperand::Register(idxAllocaReg, SadTypeKind::Integer));
                storeIdx.comment = "store initial defer index";
                currentBlock_->addInstruction(storeIdx);

                std::string condLabel = newLabel("defer_cond");
                std::string bodyLabel = newLabel("defer_body");
                auto condBlock = createBasicBlock(condLabel);
                auto bodyBlock = createBasicBlock(bodyLabel);
                currentFunction_->addBasicBlock(condBlock);
                currentFunction_->addBasicBlock(bodyBlock);
                currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(condLabel)));

                currentBlock_ = condBlock;

                std::string idxLoadReg = newTempRegister();
                SIRInstruction loadIdxInst;
                loadIdxInst.opcode = SIROpcode::LOAD;
                loadIdxInst.result = SIROperand::Register(idxLoadReg, SadTypeKind::Integer);
                loadIdxInst.operands.push_back(SIROperand::Register(idxAllocaReg, SadTypeKind::Integer));
                loadIdxInst.comment = "load defer loop index";
                currentBlock_->addInstruction(loadIdxInst);

                std::string condReg = newTempRegister();
                SIRInstruction condInst = SIRInstruction::Binary(
                    SIROpcode::GE,
                    SIROperand::Register(condReg, SadTypeKind::Boolean),
                    SIROperand::Register(idxLoadReg, SadTypeKind::Integer),
                    SIROperand::ConstantI64(0));
                condInst.comment = "defer loop index >= 0";
                currentBlock_->addInstruction(condInst);
                currentBlock_->addInstruction(SIRInstruction::BranchCond(
                    SIROperand::Register(condReg, SadTypeKind::Boolean),
                    SIROperand::Label(bodyLabel),
                    SIROperand::Label(skipLabel)));

                currentBlock_ = bodyBlock;

                std::string closureReg = newTempRegister();
                SIRInstruction getInst;
                getInst.opcode = SIROpcode::ARRAY_GET;
                getInst.result = SIROperand::Register(closureReg, SadTypeKind::Function);
                getInst.operands.push_back(SIROperand::Register(currentDeferStackReg_, SadTypeKind::Array));
                getInst.operands.push_back(SIROperand::Register(idxLoadReg, SadTypeKind::Integer));
                getInst.comment = "defer stack get closure";
                currentBlock_->addInstruction(getInst);

                std::string callResultReg = newTempRegister();
                SIRInstruction callInst;
                callInst.opcode = SIROpcode::CLOSURE_CALL;
                callInst.result = SIROperand::Register(callResultReg, SadTypeKind::Void);
                callInst.operands.push_back(SIROperand::Register(closureReg, SadTypeKind::Function));
                callInst.comment = "execute deferred closure";
                currentBlock_->addInstruction(callInst);

                std::string nextIdxReg = newTempRegister();
                SIRInstruction decInst = SIRInstruction::Binary(
                    SIROpcode::SUB_I64,
                    SIROperand::Register(nextIdxReg, SadTypeKind::Integer),
                    SIROperand::Register(idxLoadReg, SadTypeKind::Integer),
                    SIROperand::ConstantI64(1));
                decInst.comment = "defer index--";
                currentBlock_->addInstruction(decInst);

                SIRInstruction storeNextIdx;
                storeNextIdx.opcode = SIROpcode::STORE;
                storeNextIdx.operands.push_back(SIROperand::Register(nextIdxReg, SadTypeKind::Integer));
                storeNextIdx.operands.push_back(SIROperand::Register(idxAllocaReg, SadTypeKind::Integer));
                storeNextIdx.comment = "store next defer index";
                currentBlock_->addInstruction(storeNextIdx);
                currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(condLabel)));

                currentBlock_ = skipBlock;
            }

            void SIRBuilder::emitPopFunctionCleanupHandler()
            {
                if (!currentFunctionCleanupHandlerActive_ || !currentBlock_)
                {
                    return;
                }

                SIRInstruction popInst;
                popInst.opcode = SIROpcode::CALL;
                popInst.operands.push_back(SIROperand::Function("__sad_pop_handler"));
                popInst.comment = "pop function-level defer cleanup handler";
                currentBlock_->addInstruction(popInst);
            }

            // ============================================================================
            // buildStatement - بناء جملة (موزع للأنواع المختلفة)
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:372
            // التوقيع / Signature: void buildStatement(AST::Statement* stmt);
            //
            // المعاملات / Parameters:
            // - stmt: AST::Statement* = Sad::AST::Statement* (sir_builder.h:75)
            //
            // Statement Base Class (ast_node.h:88):
            // - جميع الجمل ترث من هذا الصنف
            // - All statements inherit from this class
            //
            // Statement Types (statements.h):
            // - IfStmt (line 104): condition, thenBranch, elseBranch
            // - WhileStmt (line 143): condition, body
            // - ForStmt (line 187): initializer, condition, increment, body
            // - ReturnStmt (line 266): value
            // - BreakStmt (line 360): no members
            // - ContinueStmt (line 392): no members
            // - ExprStmt (line 38): expression
            // - VarDeclStmt (line 74): name, type, initializer, isConst
            //
            // الدوال المستدعاة / Called functions:
            // - buildIfStatement: sir_builder.h:385
            // - buildWhileLoop: sir_builder.h:393
            // - buildForLoop: sir_builder.h:401
            // - buildReturnStatement: sir_builder.h:409
            // - buildBreakStatement: sir_builder.h:417
            // - buildContinueStatement: sir_builder.h:425
            // ============================================================================
        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
