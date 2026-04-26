// ============================================================================
// sir_builder_control_branch.cpp
// (AR) Branches: return, break, continue, assignment, localvar, if
// (EN) Branch control flow: return, break, continue, assignment, local variables, if/else
// ============================================================================

#include <string>
#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "../../../shared/utils/include/utf8_utils.h"
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
            // buildReturnStatement - ״¨†״§״¡ ״¬…„״© return
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:409
            // ״§„״×ˆ‚״¹ / Signature: void buildReturnStatement(AST::ReturnStmt* retStmt);
            //
            // ״§„…״¹״§…„״§״× / Parameters:
            // - retStmt: AST::ReturnStmt* = Sad::AST::ReturnStmt* (sir_builder.h:409)
            //
            // ReturnStmt Members (statements.h:266):
            // - value: ExprPtr (line 268) - optional, can be nullptr
            //
            // ״§„״¯ˆ״§„ ״§„…״³״×״¯״¹״§״© / Called functions:
            // - buildExpression: sir_builder.h:432
            // ============================================================================
            void SIRBuilder::buildReturnStatement(AST::ReturnStmt *retStmt)
            {
                if (!retStmt)
                {
                    return;
                }

                // ================================================================
                // (AR) FIX X06: بناء تعبير الإرجاع قبل defer
                //      المشكلة: emitRunDeferredClosures() يغير currentBlock_ الى defer_skip،
                //      وعند بناء التعبير بعده، namedValues تشير الى alloca من نطاق مختلف.
                //      مثال: في match، متغير "ضريبة" لكل case له alloca منفصل. عند بناء
                //      ارجع "..." + ضريبة في defer_skip، يجد المترجم آخر alloca لـ ضريبة
                //      (من آخر case) بدلاً من alloca الـ case الحالي، فتكون القيمة 0.
                //      الحل: بناء التعبير في body block + تخزينه في alloca فريد لضمان SSA.
                // (EN) FIX X06: Build return expression BEFORE defer
                //      Problem: emitRunDeferredClosures() changes currentBlock_ to defer_skip,
                //      and building the expression after it uses wrong alloca from another scope.
                //      In match: each case has its own "var" alloca. Building "..." + var
                //      in defer_skip finds the LAST case's alloca (value=0) not current case's.
                //      Solution: Build in current body block + store in unique temp alloca.
                // ================================================================
                std::string prebuiltRetReg;
                SadTypeKind prebuiltRetType = SadTypeKind::Void;
                bool hasPrebuiltRet = false;

                if (retStmt->value && !currentDeferStackReg_.empty() && finallyStack_.empty() &&
                    (!currentFunction_ || !currentFunction_->isCoroutine))
                {
                    BuildResult preResult = buildExpression(retStmt->value.get());
                    if (!preResult.registerName.empty() && !preResult.isConstant)
                    {
                        // (AR) تخزين القيمة في alloca مؤقت قبل defer لضمان SSA correctness
                        // (EN) Store value in temp alloca before defer for SSA correctness
                        std::string tempAllocaReg = newTempRegister();
                        SadTypeKind storeType = preResult.type;

                        SIRInstruction allocInst;
                        allocInst.opcode = SIROpcode::ALLOC;
                        allocInst.result = SIROperand::Register(tempAllocaReg, storeType);
                        allocInst.comment = "temp alloca for return value (FIX X06: before defer)";
                        if (currentBlock_)
                            currentBlock_->addInstruction(allocInst);

                        SIRInstruction storeInst;
                        storeInst.opcode = SIROpcode::STORE;
                        storeInst.operands.push_back(SIROperand::Register(preResult.registerName, storeType));
                        storeInst.operands.push_back(SIROperand::Register(tempAllocaReg, storeType));
                        storeInst.comment = "store return value before defer cleanup (FIX X06)";
                        if (currentBlock_)
                            currentBlock_->addInstruction(storeInst);

                        prebuiltRetReg = tempAllocaReg;
                        prebuiltRetType = storeType;
                        hasPrebuiltRet = true;
                    }
                }

                // ================================================================
                // (AR) ״×†״° ״§„״¬…„ ״§„…״₪״¬„״© (״£״¬‘„/defer) ‚״¨„ RET ״¨״×״±״×״¨ LIFO
                // (EN) Execute deferred statements (defer) before RET in LIFO order
                // ================================================================
                if (!currentDeferStackReg_.empty())
                {
                    emitRunDeferredClosures();
                }
                else if (!deferredStatements_.empty())
                {
                    for (auto it = deferredStatements_.rbegin(); it != deferredStatements_.rend(); ++it)
                    {
                        if (*it)
                            buildStatement(*it);
                    }
                }

                // ================================================================
                // (AR) ״­״µ ״³״§‚ finally: ״¥״°״§ ƒ†״§ ״¯״§״®„ try/catch …״¹ ״£״®״±״§‹
                //      „״§ †״µ״¯״± RET …״¨״§״´״±״© ג€” †״­״¸ ״§„‚…״© ˆ†‚״² „€ finally „״¶…״§† ״×†״°‡
                //      ״§„״£†ˆ״§״¹ ״§„…״¯״¹ˆ…״©: 0=״±״§״÷״ 1=״±‚…/…†״·‚״ 2=†״µ/…״₪״´״±״ 3=״¹״´״±
                //      ‡״°״§ ״§„״­״µ ״¬״¨ ״£† ƒˆ† ״¨״¹״¯ defer ˆ‚״¨„ ƒ„ ״´״¡ ״¢״®״±
                // (EN) Check finally context: if inside try/catch with finally block
                //      Don't emit direct RET ג€” save value and branch to finally to guarantee execution
                //      Supported types: 0=void, 1=integer/bool, 2=string/ptr, 3=float
                //      This check must come after defer processing and before everything else
                // ================================================================
                if (!finallyStack_.empty())
                {
                    auto &ctx = finallyStack_.back();

                    if (retStmt->value)
                    {
                        // (AR) ״¨†״§״¡ ‚…״© ״§„״¥״±״¬״§״¹
                        // (EN) Build return value expression
                        BuildResult valResult = buildExpression(retStmt->value.get());

                        // (AR) ״¥״°״§ ƒ״§†״× ״§„‚…״©  alloca (…״×״÷״± …״­„) ג†’ †״­״×״§״¬ LOAD ״£ˆ„״§‹
                        // (EN) If value is in alloca (local variable) ג†’ need LOAD first
                        std::string actualReg = valResult.registerName;
                        SadTypeKind actualType = valResult.type;

                        if (!valResult.isConstant && !actualReg.empty() &&
                            !valResult.isFieldAccess && !valResult.isDirectValue &&
                            actualType != SadTypeKind::String && actualType != SadTypeKind::Function &&
                            valResult.className.empty())
                        {
                            std::string vn = actualReg;
                            if (!vn.empty() && vn[0] == '%')
                                vn = vn.substr(1);
                            if (lookupVariable(vn))
                            {
                                std::string loadedReg = newTempRegister();
                                SIRInstruction loadInst;
                                loadInst.opcode = SIROpcode::LOAD;
                                loadInst.result = SIROperand::Register(loadedReg, actualType);
                                loadInst.operands.push_back(SIROperand::Register(actualReg, actualType));
                                loadInst.comment = "load variable for finally return";
                                if (currentBlock_)
                                    currentBlock_->addInstruction(loadInst);
                                actualReg = loadedReg;
                            }
                        }

                        // (AR) ״×״­״¯״¯ typeCode ˆ״×״®״²† ״§„‚…״©  ״§„״³״¬„ ״§„…†״§״³״¨
                        // (EN) Determine typeCode and store value in appropriate register
                        int typeCode = 0;
                        if (actualType == SadTypeKind::Integer || actualType == SadTypeKind::Boolean)
                        {
                            typeCode = 1;
                            SIRInstruction stI;
                            stI.opcode = SIROpcode::STORE;
                            if (valResult.isConstant && !valResult.constantValue.empty())
                            {
                                if (actualType == SadTypeKind::Integer)
                                    stI.operands.push_back(SIROperand::ConstantI64(std::stoll(valResult.constantValue)));
                                else
                                    stI.operands.push_back(SIROperand::ConstantBool(valResult.constantValue == "true"));
                            }
                            else
                                stI.operands.push_back(SIROperand::Register(actualReg, actualType));
                            stI.operands.push_back(SIROperand::Register(ctx.retValI64Reg, SadTypeKind::Integer));
                            stI.comment = "store integer finally return value";
                            if (currentBlock_)
                                currentBlock_->addInstruction(stI);
                        }
                        else if (actualType == SadTypeKind::String || actualType == SadTypeKind::Pointer)
                        {
                            typeCode = 2;
                            SIRInstruction stP;
                            stP.opcode = SIROpcode::STORE;
                            if (valResult.isConstant && actualType == SadTypeKind::String)
                                stP.operands.push_back(SIROperand::ConstantString(valResult.constantValue));
                            else
                                stP.operands.push_back(SIROperand::Register(actualReg, actualType));
                            stP.operands.push_back(SIROperand::Register(ctx.retValPtrReg, SadTypeKind::String));
                            stP.comment = "store string/ptr finally return value";
                            if (currentBlock_)
                                currentBlock_->addInstruction(stP);
                        }
                        else if (actualType == SadTypeKind::Float)
                        {
                            typeCode = 3;
                            SIRInstruction stF;
                            stF.opcode = SIROpcode::STORE;
                            if (valResult.isConstant)
                                stF.operands.push_back(SIROperand::ConstantF64(std::stod(valResult.constantValue)));
                            else
                                stF.operands.push_back(SIROperand::Register(actualReg, actualType));
                            stF.operands.push_back(SIROperand::Register(ctx.retValI64Reg, SadTypeKind::Float));
                            stF.comment = "store float finally return value";
                            if (currentBlock_)
                                currentBlock_->addInstruction(stF);
                        }
                        else if (!valResult.registerName.empty())
                        {
                            // (AR) †ˆ״¹ ״÷״± …״¹״±ˆ ג€” †״¹״§…„‡… ƒ״±‚…
                            // (EN) Unknown type ג€” treat as integer
                            typeCode = 1;
                            SIRInstruction stU;
                            stU.opcode = SIROpcode::STORE;
                            stU.operands.push_back(SIROperand::Register(actualReg, actualType));
                            stU.operands.push_back(SIROperand::Register(ctx.retValI64Reg, SadTypeKind::Integer));
                            stU.comment = "store unknown-type finally return value as i64";
                            if (currentBlock_)
                                currentBlock_->addInstruction(stU);
                        }

                        // (AR) ״×״®״²† typeCode  ״³״¬„ ״§„†ˆ״¹
                        // (EN) Store typeCode in type register
                        {
                            SIRInstruction stTC;
                            stTC.opcode = SIROpcode::STORE;
                            stTC.operands.push_back(SIROperand::ConstantI64(typeCode));
                            stTC.operands.push_back(SIROperand::Register(ctx.retTypeReg, SadTypeKind::Integer));
                            stTC.comment = "store finally return type code = " + std::to_string(typeCode);
                            if (currentBlock_)
                                currentBlock_->addInstruction(stTC);
                        }
                    }
                    // (AR) ״§״±״¬״¹ ״¨״¯ˆ† ‚…״© ג†’ type = 0 (void) ג€” …״¨״¯״¦״§‹ 0 …† ״§„״×‡״¦״©
                    // (EN) Return without value ג†’ type = 0 (void) ג€” already 0 from initialization

                    // (AR) ״×״¹† ״¹„״§…״© has_return = 1 „״¥״¹„״§… finally ״¨ˆ״¬ˆ״¯ ״§״±״¬״¹ …†״×״¸״±
                    // (EN) Set has_return = 1 to notify finally that a return is pending
                    {
                        SIRInstruction stHR;
                        stHR.opcode = SIROpcode::STORE;
                        stHR.operands.push_back(SIROperand::ConstantI64(1));
                        stHR.operands.push_back(SIROperand::Register(ctx.hasReturnReg, SadTypeKind::Integer));
                        stHR.comment = "set finally has_return = 1 (״§״±״¬״¹ intercepted by finally)";
                        if (currentBlock_)
                            currentBlock_->addInstruction(stHR);
                    }

                    // (AR) ״§„‚״² ״¥„‰ ƒ״×„״© finally ״¨״¯„״§‹ …† ״¥״µ״¯״§״± RET …״¨״§״´״±
                    // (EN) Branch to finally block instead of emitting direct RET
                    if (currentBlock_)
                        currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(ctx.finallyLabel)));

                    return; // (AR) „״§ †״µ״¯״± RET ג€” finally ״³״µ״¯״±‡ ״¨״¹״¯ ״×†״°‡
                }

                // (AR) ״¥״°״§ ƒ†״§ ״¯״§״®„ ƒˆ״±ˆ״×†״ †״³״×״®״¯… CORO_RETURN ״¨״¯„״§‹ …† RET
                // (EN) Inside a coroutine, use CORO_RETURN instead of RET
                if (currentFunction_ && currentFunction_->isCoroutine && retStmt->value)
                {
                    emitPopFunctionCleanupHandler();
                    BuildResult valueResult = buildExpression(retStmt->value.get());
                    SIRInstruction coroRet;
                    coroRet.opcode = SIROpcode::CORO_RETURN;
                    if (valueResult.isConstant && !valueResult.constantValue.empty())
                    {
                        switch (valueResult.type)
                        {
                        case SadTypeKind::Integer:
                            coroRet.operands.push_back(SIROperand::ConstantI64(std::stoll(valueResult.constantValue)));
                            break;
                        case SadTypeKind::Float:
                            coroRet.operands.push_back(SIROperand::ConstantF64(std::stod(valueResult.constantValue)));
                            break;
                        case SadTypeKind::String:
                            coroRet.operands.push_back(SIROperand::ConstantString(valueResult.constantValue));
                            break;
                        default:
                            coroRet.operands.push_back(SIROperand::Register(valueResult.registerName, valueResult.type));
                        }
                    }
                    else
                    {
                        coroRet.operands.push_back(SIROperand::Register(valueResult.registerName, valueResult.type));
                    }
                    if (currentBlock_)
                        currentBlock_->addInstruction(coroRet);
                    return;
                }

                // (AR) ReturnStmt::value: ExprPtr (statements.h:268)
                // (EN) Build return instruction
                if (retStmt->value)
                {
                    // (AR) ״¨†״§״¡ ״×״¹״¨״± ״§„‚…״© ״§„…״±״¬״¹״©
                    // (EN) Build return value expression
                    // (AR) FIX X06: استخدام القيمة المُحسوبة مسبقاً إذا توفرت
                    // (EN) FIX X06: use prebuilt return value if available
                    BuildResult valueResult;
                    if (hasPrebuiltRet)
                    {
                        // (AR) تحميل القيمة من الـ alloca المؤقت (محسوب في body block قبل defer)
                        // (EN) Load value from temp alloca (computed in body block before defer)
                        std::string loadedRetReg = newTempRegister();
                        SIRInstruction loadRetInst;
                        loadRetInst.opcode = SIROpcode::LOAD;
                        loadRetInst.result = SIROperand::Register(loadedRetReg, prebuiltRetType);
                        loadRetInst.operands.push_back(SIROperand::Register(prebuiltRetReg, prebuiltRetType));
                        loadRetInst.comment = "load prebuilt return value (FIX X06)";
                        if (currentBlock_)
                            currentBlock_->addInstruction(loadRetInst);
                        valueResult.registerName = loadedRetReg;
                        valueResult.type = prebuiltRetType;
                        valueResult.isDirectValue = true;
                    }
                    else
                    {
                        valueResult = buildExpression(retStmt->value.get());
                    }

                    // ================================================================
                    // (AR) ״×״×״¨״¹ †ˆ״¹ ״§„״µ† ״§„…״±״¬״¹:
                    //      ״¥״°״§ ƒ״§†״× ״§„‚…״© ״§„…״±״¬״¹״© ƒ״§״¦† („״¯‡״§ className)״ †״³״¬‘„ ״°„ƒ 
                    //      functionTable_ ״­״×‰ ״×…ƒ† buildFunctionCall „״§״­‚״§‹ …† …״¹״±״© ״£†
                    //      ‡״°‡ ״§„״¯״§„״© ״×״±״¬״¹ ƒ״§״¦†״§‹ …† ״µ† …״¹‘†.
                    //      ‡״°״§ ״¶״±ˆ״± „״×״×״¨״¹ †ˆ״¹ ״§„ƒ״§״¦† ״¹״¨״± ״§״³״×״¯״¹״§״¡״§״× ״§„״¯ˆ״§„.
                    //      …״«״§„: ״¯״§„״© ״§״µ†״¹_†‚״·״©() ג†’ ״§״±״¬״¹ ״¬״¯״¯ †‚״·״©(1,2) ג†’ returnClassName = "†‚״·״©"
                    //      ״¨״¯ˆ† ‡״°״§: …״×״÷״± † = ״§״µ†״¹_†‚״·״©() ג†’ †.״³ ״×״¹״·„ „״£† ״§„…״×״±״¬… „״§ ״¹״± ״£† † ƒ״§״¦†
                    // (EN) Track return class type:
                    //      If the returned value is an object (has className), record it in
                    //      functionTable_ so buildFunctionCall can later know this function
                    //      returns an object of a specific class.
                    //      Without this: var p = makePoint() ג†’ p.x crashes because compiler
                    //      doesn't know p is an object.
                    // ================================================================
                    if (!valueResult.className.empty() && currentFunction_)
                    {
                        auto ftIt = functionTable_.find(currentFunction_->name);
                        if (ftIt != functionTable_.end())
                        {
                            ftIt->second.returnClassName = valueResult.className;
                        }
                    }
                    // (AR) ״£״¶״§‹: ״¥״°״§ ƒ״§† ״§„״×״¹״¨״± ״§„…״±״¬״¹ ‡ˆ ״¬״¯״¯ ClassName() …״¨״§״´״±״©
                    // (EN) Also: if the return expression is directly new ClassName()
                    if (auto *newExpr = dynamic_cast<Sad::AST::NewExpr *>(retStmt->value.get()))
                    {
                        if (currentFunction_)
                        {
                            auto ftIt = functionTable_.find(currentFunction_->name);
                            if (ftIt != functionTable_.end() && ftIt->second.returnClassName.empty())
                            {
                                ftIt->second.returnClassName = newExpr->className;
                            }
                        }
                    }
                    // (AR) ˆ״£״¶״§‹: ״¥״°״§ ƒ״§† ״§„…״×״÷״± ״§„…״±״¬״¹ …״³״¬‘„  classInstanceTypes_
                    // (EN) Also: if returned variable is tracked in classInstanceTypes_
                    if (valueResult.className.empty() && currentFunction_)
                    {
                        // (AR) ״§„״×״­‚‚ …† classInstanceTypes_ ״¨״§„״§״³… ״¨״¯ˆ† %
                        // (EN) Check classInstanceTypes_ by name without %
                        std::string varName = valueResult.registerName;
                        if (!varName.empty() && varName[0] == '%')
                        {
                            varName = varName.substr(1);
                        }
                        auto ciIt = classInstanceTypes_.find(varName);
                        if (ciIt != classInstanceTypes_.end())
                        {
                            auto ftIt = functionTable_.find(currentFunction_->name);
                            if (ftIt != functionTable_.end() && ftIt->second.returnClassName.empty())
                            {
                                ftIt->second.returnClassName = ciIt->second;
                            }
                        }
                    }

                    // (AR) ״×ˆ„״¯ ״×״¹„…״© RET …״¹ ״§„‚…״©
                    // (EN) Generate RET instruction with value
                    SIRInstruction retInst;
                    retInst.opcode = SIROpcode::RET;

                    // (AR) ״¥״°״§ ƒ״§†״× ״§„‚…״© ״«״§״¨״×״©״ ״£״±״¬״¹‡״§ …״¨״§״´״±״© (״¬״¨ ״£† ƒˆ† ‡״°״§ ‚״¨„ ״­״µ %)
                    // (EN) If value is constant, return it directly (must check before % check)
                    if (valueResult.isConstant && !valueResult.constantValue.empty())
                    {
                        // (AR) ״§„‚…״© ״«״§״¨״×״©
                        // (EN) Value is constant
                        switch (valueResult.type)
                        {
                        case SadTypeKind::Integer:
                            retInst.operands.push_back(SIROperand::ConstantI64(std::stoll(valueResult.constantValue)));
                            break;
                        case SadTypeKind::Float:
                            retInst.operands.push_back(SIROperand::ConstantF64(std::stod(valueResult.constantValue)));
                            break;
                        case SadTypeKind::Boolean:
                            retInst.operands.push_back(SIROperand::ConstantBool(valueResult.constantValue == "true"));
                            break;
                        case SadTypeKind::String:
                            retInst.operands.push_back(SIROperand::ConstantString(valueResult.constantValue));
                            break;
                        // ================================================================
                        // (AR) ״¥״±״¬״§״¹ ״¨†״© ״¥״÷„״§‚ (Closure)
                        //      †״¸״§… ״§„״¥״÷„״§‚״§״× ״§„״¬״¯״¯: buildLambdaExpr †״´״¦ CLOSURE_CREATE
                        //      ˆ״±״¬״¹ ״³״¬„״§‹ ״­‚‚״§‹ (isConstant=false)
                        //      „† †״µ„ ‡†״§ (״­״§„״© Function …״¹ isConstant=true) ״¥„״§ ״¥״°״§
                        //      ƒ״§†״× ״¯״§„״© ״¹״§״¯״© ……״±״±״© ƒ‚…״© ג€” †״±״¬״¹‡״§ ƒ€ Function operand
                        // (EN) Return closure struct
                        //      New closure system: buildLambdaExpr creates CLOSURE_CREATE
                        //      and returns real register (isConstant=false)
                        //      We only reach here (Function with isConstant=true) for
                        //      regular functions passed as values ג€” return as Function operand
                        // ================================================================
                        case SadTypeKind::Function:
                        {
                            retInst.operands.push_back(SIROperand::Function(valueResult.constantValue));
                            break;
                        }
                        default:
                            retInst.operands.push_back(SIROperand::Register(valueResult.registerName, valueResult.type));
                        }
                    }
                    // ================================================================
                    // (AR) ״­״µ: ‡„ ״§„‚…״©  alloca ״¹„ (…״×״÷״± …״­„ ״£ˆ …״¹״§…„ ״¯״§„״©)״
                    //      ״¥״°״§ ƒ״§† ״§״³… ״§„״³״¬„ ״·״§״¨‚ …״×״÷״±״§‹ …״¹״±ˆ״§‹ ג†’ LOAD …״·„ˆ״¨
                    //      ˆ״¥„״§ ג†’ ״§„״³״¬„ ‚…״© …״¨״§״´״±״© (…† ״¹…„״© ״­״³״§״¨״©״ ״§״³״×״¯״¹״§״¡״ ״¥„״®)
                    // (EN) Check: is the value in an actual alloca (local var or param)?
                    //      If register name matches a known variable ג†’ LOAD needed
                    //      Otherwise ג†’ register is a direct value (from binary op, call, etc)
                    // ================================================================
                    else if (!valueResult.registerName.empty() && valueResult.registerName[0] == '%' && !valueResult.isFieldAccess && !valueResult.isDirectValue && valueResult.type != SadTypeKind::String && valueResult.type != SadTypeKind::Function && valueResult.className.empty())
                    {
                        // (AR) ״×״­‚‚: ‡„ ״§„״³״¬„ ״´״± ״¥„‰ …״×״÷״± …״­„ (alloca)״
                        //      ״§„…״×״÷״±״§״× ״§„…״­„״©: %variableName (״÷״± ״±‚…״©)
                        //      ״§„״³״¬„״§״× ״§„…״₪‚״×״©: %N (״±‚…״©) ג€” „״§ ״×״­״×״§״¬ LOAD
                        // (EN) Check: does register refer to a local variable (alloca)?
                        //      Local variables: %variableName (non-numeric)
                        //      Temp registers: %N (numeric) ג€” no LOAD needed
                        std::string varName = valueResult.registerName.substr(1); // ״¥״²״§„״© %
                        VariableInfo *maybeVar = lookupVariable(varName);
                        bool isAllocaVar = maybeVar != nullptr;

                        // (AR) ״×״­‚‚ ״¥״¶״§: ״¥״°״§ „… ƒ† …״×״÷״±״§‹ „ƒ†‡ ״¨״§״¯״¦״© inlining
                        //      …״«„ %_inl0_variable ג†’ †״­״§ˆ„ ״§„״¨״­״« ״¨״§„״§״³… ״¨״¹״¯ ״§„״¨״§״¯״¦״©
                        // (EN) Extra check: if not a variable but has inlining prefix
                        //      like %_inl0_variable ג†’ try looking up after prefix
                        if (!isAllocaVar && varName.size() > 4 && varName.substr(0, 4) == "_inl")
                        {
                            size_t underscorePos = varName.find('_', 4);
                            if (underscorePos != std::string::npos && underscorePos + 1 < varName.size())
                            {
                                std::string originalName = varName.substr(underscorePos + 1);
                                VariableInfo *inlinedVar = lookupVariable(originalName);
                                isAllocaVar = inlinedVar != nullptr;
                            }
                        }

                        if (isAllocaVar)
                        {
                            // (AR) ״§„‚…״©  ״¹†ˆ״§† alloca …״­„ ג€” †״­״×״§״¬ „״×״­…„‡״§
                            // (EN) Value is in local alloca address ג€” need to load it
                            std::string loadedReg = newTempRegister();

                            SIRInstruction loadInst;
                            loadInst.opcode = SIROpcode::LOAD;
                            loadInst.result = SIROperand::Register(loadedReg, valueResult.type);
                            loadInst.operands.push_back(SIROperand::Register(valueResult.registerName, valueResult.type));

                            if (currentBlock_)
                            {
                                currentBlock_->addInstruction(loadInst);
                            }

                            SIROperand retOperand = SIROperand::Register(loadedReg, valueResult.type);
                            retInst.operands.push_back(retOperand);
                        }
                        else
                        {
                            // (AR) ״³״¬„ …״₪‚״× (‚…״© …״¨״§״´״±״©) ג€” „״§ †״­״×״§״¬ LOAD
                            // (EN) Temp register (direct value) ג€” no LOAD needed
                            SIROperand retOperand = SIROperand::Register(valueResult.registerName, valueResult.type);
                            retInst.operands.push_back(retOperand);
                        }
                    }
                    else
                    {
                        // (AR) ‚…״©  ״³״¬„ …״₪‚״×
                        // (EN) Value in temporary register
                        SIROperand retOperand = SIROperand::Register(valueResult.registerName, valueResult.type);
                        retInst.operands.push_back(retOperand);
                    }

                    if (currentBlock_)
                    {
                        if (finallyStack_.empty())
                        {
                            emitPopFunctionCleanupHandler();
                        }
                        currentBlock_->addInstruction(retInst);
                    }
                }
                else
                {
                    // (AR) ״×ˆ„״¯ ״×״¹„…״© RET_VOID
                    // (EN) Generate RET_VOID instruction
                    SIRInstruction retInst;
                    retInst.opcode = SIROpcode::RET_VOID;
                    if (currentBlock_)
                    {
                        if (finallyStack_.empty())
                        {
                            emitPopFunctionCleanupHandler();
                        }
                        currentBlock_->addInstruction(retInst);
                    }
                }
            }

            // ============================================================================
            // buildBreakStatement - ״¨†״§״¡ ״¬…„״© break
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:417
            // ״§„״×ˆ‚״¹ / Signature: void buildBreakStatement(AST::BreakStmt* breakStmt);
            //
            // BreakStmt (statements.h:360):
            // - „״§ ״×ˆ״¬״¯ ״£״¹״¶״§״¡ ״¥״¶״§״©
            // - No additional members
            // ============================================================================
            void SIRBuilder::buildBreakStatement(AST::BreakStmt *breakStmt)
            {
                if (!breakStmt)
                {
                    return;
                }

                // ========================================================================
                // (AR) ״¬…„״© break: ״§„‚״² ״¥„‰ †‡״§״© ״§„״­„‚״© ״§„״­״§„״©
                //      †״³״×״®״¯… …ƒ״¯״³ ״§„״­„‚״§״× (loopStack_) „„״­״µˆ„ ״¹„‰ ״×״³…״© ƒ״×„״© ״§„״®״±ˆ״¬
                //      ״«… †ˆ„‘״¯ ״×״¹„…״© ‚״² ״÷״± ״´״±״· (BR) ״¥„‰ ״×„ƒ ״§„ƒ״×„״©
                //
                // (EN) break statement: Jump to the end of current loop
                //      We use the loop stack (loopStack_) to get the exit block label
                //      Then generate an unconditional branch (BR) to that block
                // ========================================================================
                LoopContext *loop = getCurrentLoop();
                if (!loop)
                {
                    errors_.push_back("(AR) ״®״·״£: ״¬…„״© '‚' ״®״§״±״¬ ״­„‚״©. (EN) Error: 'break' outside of loop.");
                    return;
                }

                // (AR) ״×ˆ„״¯ ‚״² ״÷״± ״´״±״· ״¥„‰ ƒ״×„״© ״®״±ˆ״¬ ״§„״­„‚״©
                // (EN) Generate unconditional branch to loop exit block
                SIROperand exitLabel = SIROperand::Label(loop->breakLabel);
                SIRInstruction brInst = SIRInstruction::Branch(exitLabel);

                if (currentBlock_)
                {
                    currentBlock_->instructions.push_back(brInst);
                }

                // (AR) ״¥†״´״§״¡ ƒ״×„״© ״¬״¯״¯״© „„ƒˆ״¯ ״¨״¹״¯ break (ƒˆ״¯ …״×)
                //      ‡״°״§ ״¶״±ˆ״± „״£† LLVM ״×״·„״¨ ״£† ƒ„ ƒ״×„״© ״×†״×‡ ״¨…†‡ ˆ״§״­״¯ ‚״·
                // (EN) Create new block for code after break (dead code)
                //      Required because LLVM needs each block to end with exactly one terminator
                std::string afterBreakLabel = newLabel("after_break");
                auto afterBreakBlock = createBasicBlock(afterBreakLabel);
                if (currentFunction_)
                {
                    currentFunction_->addBasicBlock(afterBreakBlock);
                }
                currentBlock_ = afterBreakBlock;
            }

            // ============================================================================
            // buildContinueStatement - ״¨†״§״¡ ״¬…„״© continue
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:425
            // ״§„״×ˆ‚״¹ / Signature: void buildContinueStatement(AST::ContinueStmt* continueStmt);
            //
            // ContinueStmt (statements.h:392):
            // - „״§ ״×ˆ״¬״¯ ״£״¹״¶״§״¡ ״¥״¶״§״©
            // - No additional members
            // ============================================================================
            void SIRBuilder::buildContinueStatement(AST::ContinueStmt *continueStmt)
            {
                if (!continueStmt)
                {
                    return;
                }

                // ========================================================================
                // (AR) ״¬…„״© continue: ״§„‚״² ״¥„‰ ״¨״¯״§״© ״×ƒ״±״§״± ״§„״­„‚״© ״§„״×״§„
                //      -  ״­„‚״© while: †‚״² ״¥„‰ ƒ״×„״© ״§„״´״±״· (while_cond)
                //      -  ״­„‚״© for: †‚״² ״¥„‰ ƒ״×„״© ״§„״²״§״¯״© (for_inc)
                //        ״«… ״§„״²״§״¯״© ״³״×‚״² ״¨״¯ˆ״±‡״§ „„״´״±״·
                //      †״³״×״®״¯… continueLabel …† …ƒ״¯״³ ״§„״­„‚״§״×
                //
                // (EN) continue statement: Jump to next loop iteration
                //      - In while loop: jump to condition block (while_cond)
                //      - In for loop: jump to increment block (for_inc)
                //        then increment jumps to condition
                //      We use continueLabel from the loop stack
                // ========================================================================
                LoopContext *loop = getCurrentLoop();
                if (!loop)
                {
                    errors_.push_back("(AR) ״®״·״£: ״¬…„״© '״£ƒ…„' ״®״§״±״¬ ״­„‚״©. (EN) Error: 'continue' outside of loop.");
                    return;
                }

                // (AR) ״×ˆ„״¯ ‚״² ״÷״± ״´״±״· ״¥„‰ ƒ״×„״© ״§״³״×…״±״§״± ״§„״­„‚״©
                // (EN) Generate unconditional branch to loop continue block
                SIROperand continueLabel = SIROperand::Label(loop->continueLabel);
                SIRInstruction brInst = SIRInstruction::Branch(continueLabel);

                if (currentBlock_)
                {
                    currentBlock_->instructions.push_back(brInst);
                }

                // (AR) ״¥†״´״§״¡ ƒ״×„״© ״¬״¯״¯״© „„ƒˆ״¯ ״¨״¹״¯ continue (ƒˆ״¯ …״×)
                //      ‡״°״§ ״¶״±ˆ״± „״£† LLVM ״×״·„״¨ ״£† ƒ„ ƒ״×„״© ״×†״×‡ ״¨…†‡ ˆ״§״­״¯ ‚״·
                // (EN) Create new block for code after continue (dead code)
                //      Required because LLVM needs each block to end with exactly one terminator
                std::string afterContinueLabel = newLabel("after_continue");
                auto afterContinueBlock = createBasicBlock(afterContinueLabel);
                if (currentFunction_)
                {
                    currentFunction_->addBasicBlock(afterContinueBlock);
                }
                currentBlock_ = afterContinueBlock;
            }

            // ============================================================================
            // buildAssignment - ״¨†״§״¡ ״¥״³†״§״¯ …״×״÷״±
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:429
            // ״§„״×ˆ‚״¹ / Signature: void buildAssignment(AST::AssignExpr* assignment);
            //
            // ״§„…״¹״§…„״§״× / Parameters:
            // - assignment: AST::AssignExpr* = Sad::AST::AssignExpr* (sir_builder.h:429)
            //
            // AssignExpr Members (expressions.h:247):
            // - name: std::string (line 249)
            // - value: ExprPtr (line 250)
            //
            // ״§„״¯ˆ״§„ ״§„…״³״×״¯״¹״§״© / Called functions:
            // - buildExpression: sir_builder.h:432
            // - lookupVariable: sir_builder.h:597
            // ============================================================================

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad

