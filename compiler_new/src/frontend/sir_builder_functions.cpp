// ============================================================================
// sir_builder_functions.cpp - بناء الدوال والمتغيرات العامة
// ============================================================================
// (AR) هذا الملف يحتوي على:
//      - buildFunction: بناء دالة SIR كاملة من عقدة AST
//      - buildGlobalVariable: بناء متغير عام في وحدة SIR
// (EN) This file contains:
//      - buildFunction: Build a complete SIR function from AST node
//      - buildGlobalVariable: Build a global variable in SIR module
// ============================================================================
// تم استخراج هذا الملف من sir_builder_module.cpp وفقاً لقاعدة CW-05
// (حد أقصى 800 سطر لكل ملف)
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

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            // ============================================================================
            // buildFunction - ״¨†״§״¡ ״¯״§„״© ƒ״§…„״©
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:348
            // ״§„״×ˆ‚״¹ / Signature: void buildFunction(AST::FunctionDeclNode* funcDecl);
            //
            // ״§„…״¹״§…„״§״× / Parameters:
            // - funcDecl: AST::FunctionDeclNode* = Sad::AST::FunctionDecl* (sir_builder.h:58)
            //
            // FunctionDecl Members (declarations.h:19-64):
            // - name: std::string (line 43)
            // - parameters: std::vector<Parameter> (line 44)
            // - returnType: Data::DataType (line 45)
            // - body: StmtPtr (line 46)
            // - isBuiltin: bool (line 47)
            //
            // Parameter Structure (declarations.h:24-41):
            // - name: std::string
            // - type: Data::DataType
            // - defaultValue: ExprPtr (optional)
            //
            // ״§„״¯ˆ״§„ ״§„…״³״×״¯״¹״§״© / Called functions:
            // - std::make_shared<SIRFunction>: standard library
            // - SIRFunction::addParameter: sir_module.h:289
            // - astTypeToSIRType: sir_builder.h:713
            // - createBasicBlock: sir_builder.h:501
            // - SIRFunction::addBasicBlock: sir_module.h:321
            // - buildStatement: sir_builder.h:372
            // - module_->addFunction: sir_module.h:569
            // ============================================================================
            void SIRBuilder::buildFunction(AST::FunctionDeclNode *funcDecl)
            {
                if (!funcDecl)
                {
                    return;
                }

                // (AR) ״×״­ˆ„ †ˆ״¹ ״§„״¥״±״¬״§״¹ …† DataType ״¥„‰ SadTypeKind
                // (EN) Convert return type from DataType to SadTypeKind
                // astTypeToSIRType: sir_builder.h:713
                SadTypeKind returnType;

                // ================================================================
                // (AR) ״§„…״±״­„״© 1.75 ג€” ״×״³״¬„ ״£†ˆ״§״¹ ״§„״£״µ†״§ „„…״¹״§…„״§״× ‚״¨„ ״§״³״×†״×״§״¬ †ˆ״¹ ״§„״¥״±״¬״§״¹
                //      ‡״°״§ ״³…״­ „€ inferReturnTypeFromBody ״¨…״¹״±״© ״£† ״·_״£.״§״³… ‡ˆ ״­‚„ STRING
                //      ״¹†״¯…״§ ƒˆ† ״·_״£ …״¹״§…„״§‹ …״±״± ״¥„‡ ƒ״§״¦† …† ״µ† ״·״§„״¨
                // (EN) Phase 1.75 ג€” Register class types for parameters before return type inference
                //      This allows inferReturnTypeFromBody to know that ״·_״£.״§״³… is a STRING field
                //      when ״·_״£ is a parameter that receives an object of class ״·״§„״¨
                // ================================================================
                std::vector<std::string> tempRegisteredParams; // (AR) „„״×†״¸ „״§״­‚״§‹
                {
                    auto pctIt = paramClassTypes_.find(funcDecl->name);
                    if (pctIt != paramClassTypes_.end())
                    {
                        for (const auto &[paramName, className] : pctIt->second)
                        {
                            // (AR) †״×״­‚‚ …† ״¹״¯… ˆ״¬ˆ״¯ ״×״³״¬„ ״³״§״¨‚ „״×״¬†״¨ ״§„ƒ״×״§״¨״© ˆ‚‡
                            // (EN) Check for existing registration to avoid overwriting
                            if (classInstanceTypes_.find(paramName) == classInstanceTypes_.end())
                            {
                                classInstanceTypes_[paramName] = className;
                                tempRegisteredParams.push_back(paramName);
                            }
                        }
                    }
                }

                // (AR) ״¥״°״§ ƒ״§† †ˆ״¹ ״§„״¥״±״¬״§״¹ ״÷״± …״­״¯״¯ (UNKNOWN/NONE)״ †״³״×†״×״¬‡ …† ״¬״³… ״§„״¯״§„״©
                // (EN) If return type is unspecified (UNKNOWN/NONE), infer it from function body
                if (funcDecl->returnType == Data::DataType::UNKNOWN ||
                    funcDecl->returnType == Data::DataType::NONE)
                {
                    // (AR) ״§״³״×†״×״§״¬ ״§„†ˆ״¹ …† ״¬״³… ״§„״¯״§„״© …״¹ …״¹„ˆ…״§״× ״§„…״¹״§…„״§״×
                    // (EN) Infer type from function body with parameter information
                    returnType = inferReturnTypeFromBody(funcDecl->body.get(), funcDecl);
                }
                else
                {
                    returnType = astTypeToSIRType(funcDecl->returnType);
                }

                // (AR) ״¥†״´״§״¡ ״¯״§„״© SIR ״¬״¯״¯״© (sir_module.h:235 - SIRFunction constructor)
                // (EN) Create new SIR function
                auto sirFunction = std::make_shared<SIRFunction>(funcDecl->name, returnType);
#ifdef SIR_BUILDER_DEBUG
                std::cerr << "[SIR-DBG] buildFunction: '" << funcDecl->name
                          << "' inferred retType=" << static_cast<int>(returnType) << std::endl;
#endif

                // (AR) ״¥״°״§ ƒ״§†״× ״¯״§„״© ״÷״± …״×״²״§…†״©״ ״§״¬״¹„‡״§ ƒˆ״±ˆ״×†
                // (EN) If async function, mark as coroutine
                if (funcDecl->is_async)
                {
                    sirFunction->isCoroutine = true;
                    // (AR) ״§„ƒˆ״±ˆ״×† ״±״¬״¹ …״₪״´״±״§‹ (handle) ״¨״¯„״§‹ …† ״§„‚…״© …״¨״§״´״±״©
                    // (EN) Coroutine returns a pointer (handle) instead of direct value
                    sirFunction->returnType = SadTypeKind::Pointer;
#ifdef SIR_BUILDER_DEBUG
                    std::cerr << "[CORO] Function '" << funcDecl->name << "' marked as coroutine" << std::endl;
#endif
                }

                // (AR) ״¥״°״§ ƒ״§†״× ״¯״§„״© …ˆ„‘״¯״ ״§״¬״¹„‡״§ ƒˆ״±ˆ״×† ״£״¶״§‹
                // (EN) If generator function, also mark as coroutine
                if (funcDecl->isGenerator)
                {
                    sirFunction->isCoroutine = true;
                    sirFunction->isGenerator = true;
                    // (AR) ״§„…ˆ„‘״¯ ״±״¬״¹ …״₪״´״±״§‹ (handle) ג€” ״§„…״³״×‡„ƒ ״¬…״¹ ״§„‚…
                    // (EN) Generator returns a pointer (handle) ג€” consumer collects values
                    sirFunction->returnType = SadTypeKind::Pointer;
#ifdef SIR_BUILDER_DEBUG
                    std::cerr << "[GEN] Function '" << funcDecl->name << "' marked as generator" << std::endl;
#endif
                }

                // (AR) ״×״¹† ״§״³… ״§„״±״¨״· ״§„״®״§״±״¬ (FFI) ״¥״°״§ ƒ״§† …״­״¯״¯״§‹
                // (EN) Set FFI link name if specified
                if (!funcDecl->linkName.empty())
                {
                    sirFunction->linkName = funcDecl->linkName;
                }

                // (AR) ״¥״¶״§״© ״§„…״¹״§…„״§״× (declarations.h:44 - parameters: vector<Parameter>)
                // (EN) Add parameters
                // (AR) †״³״×״®״¯… ״§„״£†ˆ״§״¹ ״§„…״³״×†״×״¬״© …† functionTable_ (״§„…״±״­„״© 1.7)
                //      ״¨״¯„״§‹ …† ״§„״£†ˆ״§״¹ ״§„״®״§… …† AST ״¹†״¯…״§ ƒˆ† ״§„†ˆ״¹ UNKNOWN
                // (EN) Use inferred types from functionTable_ (Phase 1.7)
                //      instead of raw AST types when type is UNKNOWN
                auto ftIt = functionTable_.find(funcDecl->name);
                for (size_t i = 0; i < funcDecl->parameters.size(); i++)
                {
                    const auto &param = funcDecl->parameters[i];
                    SadTypeKind paramType = astTypeToSIRType(param.type);

                    // (AR) ״¥״°״§ ƒ״§† ״§„†ˆ״¹ I64 (…† UNKNOWN) ˆfunctionTable_ ״­״×ˆ †ˆ״¹״§‹ …״³״×†״×״¬״§‹ ״£״¶„
                    // (EN) If type is I64 (from UNKNOWN) and functionTable_ has a better inferred type
                    if (paramType == SadTypeKind::Integer &&
                        param.type == Data::DataType::UNKNOWN &&
                        ftIt != functionTable_.end() &&
                        i < ftIt->second.parameters.size() &&
                        ftIt->second.parameters[i].type != SadTypeKind::Integer)
                    {
                        paramType = ftIt->second.parameters[i].type;
#ifdef SIR_BUILDER_DEBUG
                        std::cerr << "[SIR-DBG] buildFunction '" << funcDecl->name
                                  << "' param[" << i << "] '" << param.name
                                  << "': UNKNOWN->I64 overridden to "
                                  << static_cast<int>(paramType) << std::endl;
#endif
                    }

                    SIRParameter sirParam(param.name, paramType);
                    sirFunction->addParameter(sirParam);
                }

                // (AR) ״¥״°״§ „… ƒ† ‡†״§ƒ ״¬״³… „„״¯״§„״© (builtin or declaration only), „״§ †״¨† ״§„״¬״³…
                // (EN) If no function body (builtin or declaration only), don't build body
                if (!funcDecl->body)
                {
                    module_->addFunction(sirFunction);
                    return;
                }

                // (AR) ״×״¹† ״§„״¯״§„״© ״§„״­״§„״©
                // (EN) Set current function
                currentFunction_ = sirFunction;

                // (AR) ״¥†״´״§״¡ †״·״§‚ ״¬״¯״¯ „„״¯״§„״©
                // (EN) Create new scope for function
                enterScope();

                // (AR) ״×״³״¬„ …״¹״§…„״§״× ״§„״¯״§„״©  ״§„†״·״§‚
                // (EN) Register function parameters in scope
                // (AR) †״³״×״®״¯… ״§„״£†ˆ״§״¹ ״§„…״³״×†״×״¬״© (…״·״§״¨‚״© „…״§ ״×… ״¥״¶״§״×‡ „„״¯״§„״© ״£״¹„״§‡)
                // (EN) Use inferred types (matching what was added to function above)
                for (size_t i = 0; i < funcDecl->parameters.size(); i++)
                {
                    const auto &param = funcDecl->parameters[i];
                    SadTypeKind paramType = astTypeToSIRType(param.type);

                    // (AR) †״³ ״§„…†״·‚: ״§״³״×״®״¯״§… ״§„†ˆ״¹ ״§„…״³״×†״×״¬ ״¹†״¯…״§ ƒˆ† UNKNOWN
                    // (EN) Same logic: use inferred type when UNKNOWN
                    if (paramType == SadTypeKind::Integer &&
                        param.type == Data::DataType::UNKNOWN &&
                        ftIt != functionTable_.end() &&
                        i < ftIt->second.parameters.size() &&
                        ftIt->second.parameters[i].type != SadTypeKind::Integer)
                    {
                        paramType = ftIt->second.parameters[i].type;
                    }

                    VariableInfo paramInfo;
                    paramInfo.name = param.name;
                    paramInfo.type = paramType;
                    paramInfo.registerName = "%" + param.name;
                    paramInfo.isGlobal = false;
                    // (AR) …״¹״§…„״§״× ״§„״¯ˆ״§„ ‚״§״¨„״© „„״×״¹״¯„ ג€” ״×…״«„ †״³״®״© …״­„״© …† ״§„‚…״© ״§„……״±״±״©
                    //      …״«״§„: ״¯״§„״© ״­„‚״©(״¹) ג†’ ״¹ = ״¹ - 1 ״¯״§״®„ ״¨†…״§ ״¬״¨ ״£† ״¹…„
                    //      ״¨״¯ˆ† ‡״°״§: buildAssignment ״×״¬״§‡„ ״§„״×״¹† ״¨״µ…״× ג†’ ״­„‚״© „״§†‡״§״¦״©
                    // (EN) Function parameters are mutable ג€” they represent a local copy of the passed value
                    //      Example: func loop(x) ג†’ x = x - 1 inside while must work
                    //      Without this: buildAssignment silently skips assignment ג†’ infinite loop
                    paramInfo.isMutable = true;
                    paramInfo.isParameter = true;
                    paramInfo.scopeLevel = static_cast<int>(scopeStack_.size());

                    // ═══════════════════════════════════════════════════════════════
                    // (AR) نقل نوع عنصر المصفوفة من FunctionInfo إلى VariableInfo
                    //      Phase 1.7 حدّد elementType من فحص ArrayExpr في مواقع الاستدعاء
                    //      بدون هذا: forEach على معامل مصفوفة نصوص يُعامل العناصر كأرقام
                    // (EN) Propagate array element type from FunctionInfo to VariableInfo
                    //      Phase 1.7 inferred elementType from ArrayExpr at call sites
                    //      Without this: forEach over string array param treats elements as integers
                    // ═══════════════════════════════════════════════════════════════
                    if (ftIt != functionTable_.end() && i < ftIt->second.parameters.size())
                    {
                        if (ftIt->second.parameters[i].elementType != SadTypeKind::Void)
                        {
                            paramInfo.elementType = ftIt->second.parameters[i].elementType;
                        }
                    }

                    addVariable(paramInfo);
                }

                // (AR) ״¥†״´״§״¡ basic block „„״¯״®ˆ„ (createBasicBlock: sir_builder.h:501)
                // (EN) Create entry basic block
                auto entryBlock = createBasicBlock(kEntryBlockName);

                // (AR) ״¥״¶״§״© block „„״¯״§„״© (sir_module.h:321 - addBasicBlock)
                // (EN) Add block to function
                sirFunction->addBasicBlock(entryBlock);
                currentBlock_ = entryBlock;

                // (AR) ״×†״¸ …ƒ״¯״³ ״§„״×״£״¬„ (defer) ״§„״³״§״¨‚ ˆ״¨״¯״¡ …ƒ״¯״³ ״¬״¯״¯ „‡״°‡ ״§„״¯״§„״©
                // (EN) Clear previous defer stack and start fresh for this function
                auto savedDeferredStatements = std::move(deferredStatements_);
                deferredStatements_.clear();
                auto savedDeferStackReg = currentDeferStackReg_;
                auto savedDeferExecutedFlagReg = currentDeferExecutedFlagReg_;
                bool savedCleanupHandlerState = currentFunctionCleanupHandlerActive_;

                currentDeferStackReg_ = "%__defer_stack_" + std::to_string(nextLabel_++);
                currentDeferExecutedFlagReg_ = "%__defer_done_" + std::to_string(nextLabel_++);
                currentFunctionCleanupHandlerActive_ = true;

                {
                    SIRInstruction newArrInst;
                    newArrInst.opcode = SIROpcode::ARRAY_NEW;
                    newArrInst.result = SIROperand::Register(currentDeferStackReg_, SadTypeKind::Array);
                    newArrInst.operands.push_back(SIROperand::ConstantI64(8));
                    newArrInst.operands.push_back(SIROperand::ConstantI64(0));
                    newArrInst.comment = "runtime defer stack for function";
                    currentBlock_->addInstruction(newArrInst);
                }

                {
                    SIRInstruction allocDoneInst;
                    allocDoneInst.opcode = SIROpcode::ALLOC;
                    allocDoneInst.result = SIROperand::Register(currentDeferExecutedFlagReg_, SadTypeKind::Integer);
                    allocDoneInst.comment = "alloca defer executed flag";
                    currentBlock_->addInstruction(allocDoneInst);

                    SIRInstruction initDoneInst;
                    initDoneInst.opcode = SIROpcode::STORE;
                    initDoneInst.operands.push_back(SIROperand::ConstantI64(0));
                    initDoneInst.operands.push_back(SIROperand::Register(currentDeferExecutedFlagReg_, SadTypeKind::Integer));
                    initDoneInst.comment = "init defer executed flag = 0";
                    currentBlock_->addInstruction(initDoneInst);
                }

                std::string functionCleanupJmpbufReg = "%__defer_jmpbuf_" + std::to_string(nextLabel_++);
                std::string functionCleanupSetjmpReg = newTempRegister();
                std::string functionCleanupCmpReg = newTempRegister();
                std::string functionBodyLabel = newLabel("function_body");
                std::string functionCleanupLabel = newLabel("function_defer_cleanup");
                auto functionBodyBlock = createBasicBlock(functionBodyLabel);
                auto functionCleanupBlock = createBasicBlock(functionCleanupLabel);
                currentFunction_->addBasicBlock(functionBodyBlock);
                currentFunction_->addBasicBlock(functionCleanupBlock);

                {
                    SIRInstruction allocJmpbufInst;
                    allocJmpbufInst.opcode = SIROpcode::CALL;
                    allocJmpbufInst.result = SIROperand::Register(functionCleanupJmpbufReg, SadTypeKind::Pointer);
                    allocJmpbufInst.operands.push_back(SIROperand::Function("__sad_alloc_jmpbuf"));
                    allocJmpbufInst.comment = "allocate function-level defer cleanup jmpbuf";
                    currentBlock_->addInstruction(allocJmpbufInst);

                    SIRInstruction pushHandlerInst;
                    pushHandlerInst.opcode = SIROpcode::CALL;
                    pushHandlerInst.operands.push_back(SIROperand::Function("__sad_push_handler"));
                    pushHandlerInst.operands.push_back(SIROperand::Register(functionCleanupJmpbufReg, SadTypeKind::Pointer));
                    pushHandlerInst.comment = "push function-level defer cleanup handler";
                    currentBlock_->addInstruction(pushHandlerInst);

                    SIRInstruction setjmpInst;
                    setjmpInst.opcode = SIROpcode::CALL;
                    setjmpInst.result = SIROperand::Register(functionCleanupSetjmpReg, SadTypeKind::Integer);
                    setjmpInst.operands.push_back(SIROperand::Function("__sad_setjmp"));
                    setjmpInst.operands.push_back(SIROperand::Register(functionCleanupJmpbufReg, SadTypeKind::Pointer));
                    setjmpInst.comment = "setjmp for function-level defer cleanup";
                    currentBlock_->addInstruction(setjmpInst);

                    SIRInstruction cmpInst = SIRInstruction::Binary(
                        SIROpcode::NE,
                        SIROperand::Register(functionCleanupCmpReg, SadTypeKind::Boolean),
                        SIROperand::Register(functionCleanupSetjmpReg, SadTypeKind::Integer),
                        SIROperand::ConstantI64(0));
                    cmpInst.comment = "function-level defer cleanup: did exception escape?";
                    currentBlock_->addInstruction(cmpInst);

                    currentBlock_->addInstruction(SIRInstruction::BranchCond(
                        SIROperand::Register(functionCleanupCmpReg, SadTypeKind::Boolean),
                        SIROperand::Label(functionCleanupLabel),
                        SIROperand::Label(functionBodyLabel)));
                }

                currentBlock_ = functionBodyBlock;

                // (AR) ״¨†״§״¡ ״¬״³… ״§„״¯״§„״© (declarations.h:46 - body: StmtPtr)
                // (EN) Build function body

                // ================================================================
                // (AR) ״§„״¹‚ˆ״¯ ״§„״¨״±…״¬״©: ״¨†״§״¡ ״´״±ˆ״· ״×״·„״¨ (preconditions)
                //      ״¥״°״§ ״´„ ״§„״´״±״·״ †״±״¹ ״§״³״×״«†״§״¡ ״¹״¨״± __sad_raise …ƒ† ״§„״×‚״§״·‡ ״¨€ ״­״§ˆ„/״§…״³ƒ
                //      …״«״§„: ״×״·„״¨ ״¨ != 0 ג†’ ״¥״°״§ (!(״¨ != 0)) { __sad_raise("״®״·״£", "״´„...") }
                // (EN) Design by Contract: build preconditions (requires)
                //      If condition fails, raise exception via __sad_raise (catchable by try/catch)
                //      Example: requires b != 0 ג†’ if (!(b != 0)) { __sad_raise("״®״·״£", "״´„...") }
                // ================================================================
                if (!funcDecl->preconditions.empty())
                {
                    for (const auto &precond : funcDecl->preconditions)
                    {
                        if (!precond)
                            continue;
                        auto condResult = buildExpression(precond.get());
                        if (condResult.registerName.empty())
                            continue;

                        // (AR) ״¥†״´״§״¡ ƒ״×„: fail (״´„) + pass (†״¬״§״­)
                        std::string failLabel = newLabel("precond_fail");
                        std::string passLabel = newLabel("precond_pass");
                        auto failBlock = createBasicBlock(failLabel);
                        auto passBlock = createBasicBlock(passLabel);
                        if (currentFunction_)
                        {
                            currentFunction_->addBasicBlock(failBlock);
                            currentFunction_->addBasicBlock(passBlock);
                        }

                        // (AR) ״§„‚״²: ״¥״°״§ ״§„״´״±״· ״µ״­״­ ג†’ pass״ ˆ״¥„״§ ג†’ fail
                        SIRInstruction brInst = SIRInstruction::BranchCond(
                            SIROperand::Register(condResult.registerName, condResult.type),
                            SIROperand::Label(passLabel),
                            SIROperand::Label(failLabel));
                        if (currentBlock_)
                            currentBlock_->instructions.push_back(brInst);

                        // (AR) ƒ״×„״© ״§„״´„: ״±״¹ ״§״³״×״«†״§״¡ ״¹״¨״± __sad_raise (…ƒ† ״§„״×‚״§״·‡ ״¨€ ״­״§ˆ„/״§…״³ƒ)
                        // (EN) Fail block: raise exception via __sad_raise (catchable by try/catch)
                        currentBlock_ = failBlock;
                        {
                            // (AR) [Fix BF-04] استبدال exit(1) بـ __sad_raise لجعل العقود قابلة للالتقاط
                            //      الرسالة تحتوي "فشل العقد" للتوافق مع سلوك المفسر
                            // (EN) [Fix BF-04] Replace exit(1) with __sad_raise for catchable contracts
                            //      Message contains "فشل العقد" for interpreter parity
                            std::string errMsg = "\xd9\x81\xd8\xb4\xd9\x84 \xd8\xa7\xd9\x84\xd8\xb9\xd9\x82\xd8\xaf \xd9\x81\xd9\x8a \xd8\xa7\xd9\x84\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 " + funcDecl->name;
                            SIRInstruction raiseInst;
                            raiseInst.opcode = SIROpcode::CALL;
                            raiseInst.operands.push_back(SIROperand::Function("__sad_raise"));
                            raiseInst.operands.push_back(SIROperand::ConstantString("\xd8\xae\xd8\xb7\xd8\xa3")); // "״®״·״£"
                            raiseInst.operands.push_back(SIROperand::ConstantString(errMsg));
                            raiseInst.comment = "precondition failure ג€” raise catchable exception";
                            currentBlock_->addInstruction(raiseInst);

                            // (AR) ƒ״×„״© …״×״© ״¨״¹״¯ raise (longjmp „״§ ״¹ˆ״¯)
                            // (EN) Dead block after raise (longjmp never returns)
                            std::string deadLabel = newLabel("precond_dead");
                            auto deadBlock = createBasicBlock(deadLabel);
                            if (currentFunction_)
                                currentFunction_->addBasicBlock(deadBlock);
                            currentBlock_ = deadBlock;
                        }

                        // (AR) ƒ״×„״© ״§„†״¬״§״­: ״§„…״×״§״¨״¹״©
                        currentBlock_ = passBlock;
                    }
                }

                if (funcDecl->body)
                {
                    // buildStatement: sir_builder.h:372
                    buildStatement(funcDecl->body.get());
                }

                auto bodyContinuationBlock = currentBlock_;

                currentBlock_ = functionCleanupBlock;
                emitPopFunctionCleanupHandler();
                emitRunDeferredClosures();
                {
                    SIRInstruction rethrowInst;
                    rethrowInst.opcode = SIROpcode::CALL;
                    rethrowInst.operands.push_back(SIROperand::Function("__sad_raise_current"));
                    rethrowInst.comment = "run defer then rethrow escaping function exception";
                    currentBlock_->addInstruction(rethrowInst);
                }
                {
                    std::string deadLabel = newLabel("function_cleanup_dead");
                    auto deadBlock = createBasicBlock(deadLabel);
                    currentFunction_->addBasicBlock(deadBlock);
                    currentBlock_ = deadBlock;
                }

                currentBlock_ = bodyContinuationBlock;

                // ================================================================
                // (AR) ״×†״° ״§„״¬…„ ״§„…״₪״¬„״© ״§„…״×״¨‚״© („†‡״§״© ״§„״¯״§„״© ״§„״·״¨״¹״© ״¨״¯ˆ† return)
                //      …‡…: †״×״­‚‚ ״£ˆ„״§‹ ״£† ״§„ƒ״×„״© ״§„״­״§„״© „״§ ״×״­״×ˆ ״¹„‰ terminator
                //      ״¥״°״§ ƒ״§† ״¬״³… ״§„״¯״§„״© †״×‡ ״¨€ return״ ״¥† buildReturnStatement
                //      ‚״¯ †‘״° ״§„״¬…„ ״§„…״₪״¬„״© ״¨״§„״¹„ ˆ״£״¶״§ RET. ״¥״¶״§״© ƒˆ״¯ ״¢״®״± ״¨״¹״¯
                //      RET ״×״³״¨״¨  ״®״·״£ LLVM: "Terminator in middle of basic block"
                // (EN) Execute remaining deferred statements (for normal function end without return)
                //      Important: first check that current block doesn't have a terminator.
                //      If function body ends with return, buildReturnStatement already
                //      executed deferred statements and emitted RET. Adding code after
                //      RET causes LLVM error: "Terminator in middle of basic block"
                // ================================================================
                bool blockAlreadyTerminated = false;
                if (currentBlock_ && !currentBlock_->instructions.empty())
                {
                    auto lastOp = currentBlock_->instructions.back().opcode;
                    blockAlreadyTerminated = (lastOp == SIROpcode::RET ||
                                              lastOp == SIROpcode::RET_VOID ||
                                              lastOp == SIROpcode::BR ||
                                              lastOp == SIROpcode::BR_COND ||
                                              lastOp == SIROpcode::CORO_RETURN);
                }

                if (!blockAlreadyTerminated && !currentDeferStackReg_.empty())
                {
                    emitRunDeferredClosures();
                    emitPopFunctionCleanupHandler();
                }

                if (!blockAlreadyTerminated && currentDeferStackReg_.empty() && !deferredStatements_.empty())
                {
                    for (auto it = deferredStatements_.rbegin(); it != deferredStatements_.rend(); ++it)
                    {
                        if (*it)
                        {
                            buildStatement(*it);
                        }
                    }
                }

                // (AR) ״§״³״×״¹״§״¯״© …ƒ״¯״³ ״§„״×״£״¬„ ״§„״³״§״¨‚ („„״¯ˆ״§„ ״§„…״×״¯״§״®„״©)
                // (EN) Restore previous defer stack (for nested functions)
                deferredStatements_ = std::move(savedDeferredStatements);
                currentDeferStackReg_ = savedDeferStackReg;
                currentDeferExecutedFlagReg_ = savedDeferExecutedFlagReg;
                currentFunctionCleanupHandlerActive_ = savedCleanupHandlerState;

                // (AR) ״§„״®״±ˆ״¬ …† †״·״§‚ ״§„״¯״§„״©
                // (EN) Exit function scope
                exitScope();

                // (AR) ״×†״¸ ״×״³״¬„״§״× classInstanceTypes_ ״§„…״₪‚״×״© „„…״¹״§…„״§״× (״§„…״±״­„״© 1.75)
                //      †״²„ ‚״· …״§ ״£״¶†״§‡ …״₪‚״×״§‹ ג€” ״§„״×״³״¬„״§״× ״§„״£״µ„״© (…† ״¬״¯״¯ ClassName) ״×״¨‚‰
                // (EN) Clean up temporary classInstanceTypes_ registrations for params (Phase 1.75)
                //      Only remove what we temporarily added ג€” original registrations (from new ClassName) stay
                for (const auto &pName : tempRegisteredParams)
                {
                    classInstanceTypes_.erase(pName);
                }

                // (AR) ״§„״×״£ƒ״¯ …† ˆ״¬ˆ״¯ terminator  †‡״§״© ״§„״¯״§„״©
                // (EN) Ensure function has a terminator at the end
                // (AR) ״§„ƒˆ״±ˆ״×†״§״× „״§ ״×״­״×״§״¬ terminator ״¥״¶״§ - ״§„״®״§״×…״© ״×״×ƒ„ ״¨״°„ƒ
                // (EN) Coroutines don't need extra terminators - epilogue handles it
                if (!sirFunction->isCoroutine)
                {
                    // ״¥״°״§ ƒ״§†״× ״§„״¯״§„״© void ˆ„״§ ˆ״¬״¯ return ״µ״±״­״ †״¶ RET_VOID
                    // If function is void and has no explicit return, add RET_VOID
                    if (currentBlock_ && !currentBlock_->instructions.empty())
                    {
                        const auto &lastInst = currentBlock_->instructions.back();
                        if (lastInst.opcode != SIROpcode::RET && lastInst.opcode != SIROpcode::RET_VOID && lastInst.opcode != SIROpcode::CORO_RETURN)
                        {
                            // (AR) „״§ ˆ״¬״¯ return - †״¶ ˆ״§״­״¯״§‹
                            // (EN) No return - add one
                            if (returnType == SadTypeKind::Void)
                            {
                                SIRInstruction retInst;
                                retInst.opcode = SIROpcode::RET_VOID;
                                currentBlock_->addInstruction(retInst);
                            }
                            else
                            {
                                // (AR) „„״¯ˆ״§„ ״÷״± void״ †״¶ return ״¨‚…״© ״§״×״±״§״¶״©
                                // (EN) For non-void functions, add return with default value
                                SIRInstruction retInst;
                                retInst.opcode = SIROpcode::RET;
                                if (returnType == SadTypeKind::String)
                                {
                                    retInst.operands.push_back(SIROperand::ConstantString(""));
                                }
                                else
                                {
                                    retInst.operands.push_back(SIROperand::ConstantI64(0));
                                }
                                currentBlock_->addInstruction(retInst);
                            }
                        }
                    }
                    else if (currentBlock_ && currentBlock_->instructions.empty())
                    {
                        // (AR) ״§„״¯״§„״© ״§״±״÷״© - †״¶ return
                        // (EN) Empty function - add return
                        if (returnType == SadTypeKind::Void)
                        {
                            SIRInstruction retInst;
                            retInst.opcode = SIROpcode::RET_VOID;
                            currentBlock_->addInstruction(retInst);
                        }
                        else
                        {
                            SIRInstruction retInst;
                            retInst.opcode = SIROpcode::RET;
                            if (returnType == SadTypeKind::String)
                            {
                                retInst.operands.push_back(SIROperand::ConstantString(""));
                            }
                            else
                            {
                                retInst.operands.push_back(SIROperand::ConstantI64(0));
                            }
                            currentBlock_->addInstruction(retInst);
                        }
                    }
                }

                // (AR) ״¥״¶״§״© ״§„״¯״§„״© „„ˆ״­״¯״© (sir_module.h:569 - addFunction)
                // (EN) Add function to module
                module_->addFunction(sirFunction);

                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                // (AR) ״×״³״¬„/״×״­״¯״« ״§„״¯״§„״©  ״¬״¯ˆ„ ״§„״¯ˆ״§„ „„״¨״­״« ״¹†‡״§ ״¹†״¯ ״§„״§״³״×״¯״¹״§״¡
                // (EN) Register/update function in functionTable_ for call resolution
                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                {
                    FunctionInfo funcInfo;
                    funcInfo.name = funcDecl->name;
                    funcInfo.returnType = returnType;
                    funcInfo.parameters = sirFunction->getParameters();
                    funcInfo.sirFunction = sirFunction;
                    funcInfo.isGenerator = sirFunction->isGenerator;
                    // (AR) حفظ مرجع AST لمعالجة القيم الافتراضية عند الاستدعاء
                    // (EN) Save AST reference for default parameter values at call sites
                    funcInfo.astDecl = funcDecl;

                    // ================================================================
                    // (AR) [Fix #52] تتبع اسم اللامدا المُرجعة:
                    //      إذا كانت الدالة تُرجع إغلاقاً (CLOSURE_CREATE)، نستخرج
                    //      اسم اللامدا من تعليمة RET الأخيرة ونحفظه في returnLambdaName.
                    //      هذا يسمح بتتبع نوع الإرجاع الصحيح عند استدعاء الإغلاق
                    //      عبر متغير (مثل: ل = صانع() ثم ل() )
                    // (EN) [Fix #52] Track returned lambda name:
                    //      If function returns a closure (CLOSURE_CREATE), extract
                    //      the lambda name from the last RET instruction and save
                    //      in returnLambdaName. Enables correct return type tracking
                    //      when calling the closure via a variable.
                    // ================================================================
                    if (returnType == SadTypeKind::Function)
                    {
                        for (const auto &block : sirFunction->basicBlocks)
                        {
                            for (const auto &inst : block->instructions)
                            {
                                if (inst.opcode == SIROpcode::RET && !inst.operands.empty())
                                {
                                    const std::string &retReg = inst.operands[0].name;
                                    // (AR) البحث عن CLOSURE_CREATE التي أنتجت هذا السجل
                                    // (EN) Find CLOSURE_CREATE that produced this register
                                    for (const auto &b2 : sirFunction->basicBlocks)
                                    {
                                        for (const auto &i2 : b2->instructions)
                                        {
                                            if (i2.opcode == SIROpcode::CLOSURE_CREATE &&
                                                i2.result.has_value() &&
                                                i2.result->name == retReg &&
                                                !i2.operands.empty())
                                            {
                                                // (AR) المعامل الأول هو مرجع الدالة (اسم اللامدا)
                                                // (EN) First operand is function reference (lambda name)
                                                funcInfo.returnLambdaName = i2.operands[0].name;
                                            }
                                        }
                                    }
                                    break;
                                }
                            }
                            if (!funcInfo.returnLambdaName.empty())
                                break;
                        }
                    }

                    functionTable_[funcDecl->name] = funcInfo;
                }

                // (AR) ״¥״¹״§״¯״© ״×״¹† ״§„״¯״§„״© ״§„״­״§„״©
                // (EN) Reset current function
                currentFunction_ = nullptr;
                currentBlock_ = nullptr;
            }

            // ============================================================================
            // buildGlobalVariable - ״¨†״§״¡ …״×״÷״± ״¹״§…
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:356
            // ״§„״×ˆ‚״¹ / Signature: void buildGlobalVariable(AST::VariableDeclNode* varDecl);
            //
            // ״§„…״¹״§…„״§״× / Parameters:
            // - varDecl: AST::VariableDeclNode* = Sad::AST::VarDeclStmt* (sir_builder.h:59)
            //
            // VarDeclStmt Members (statements.h:74-100):
            // - name: std::string (line 76)
            // - type: Data::DataType (line 77)
            // - initializer: ExprPtr (line 78)
            // - isConst: bool (line 79)
            //
            // ״§„״¯ˆ״§„ ״§„…״³״×״¯״¹״§״© / Called functions:
            // - astTypeToSIRType: sir_builder.h:713
            // - module_->addGlobalVariable: sir_module.h:591
            // ============================================================================
            void SIRBuilder::buildGlobalVariable(AST::VariableDeclNode *varDecl)
            {
                if (!varDecl)
                {
                    return;
                }

                // (AR) ״×״­ˆ„ ״§„†ˆ״¹ (astTypeToSIRType: sir_builder.h:713)
                // (EN) Convert type
                SadTypeKind varType = astTypeToSIRType(varDecl->type);
                // (AR) إذا كان النوع UNKNOWN (افتراضي Integer)، نستنتجه من المُهيئ
                //      هذا ضروري لمتغيرات الفضاء مثل: متغير PI = 3.14159
                //      حيث لا يُحدد النوع صراحة ويجب استنتاجه من القيمة الحرفية
                // (EN) If type is UNKNOWN (defaults to Integer), infer from initializer
                //      Needed for namespace vars like: var PI = 3.14159
                //      where type is not explicit and must be inferred from literal value
                if (varDecl->type == Data::DataType::UNKNOWN && varDecl->initializer)
                {
                    if (auto *litExpr = dynamic_cast<Sad::AST::LiteralExpr *>(varDecl->initializer.get()))
                    {
                        Lexer::TokenType tokType = litExpr->token.getType();
                        if (tokType == Lexer::TokenType::NUMBER_DOUBLE)
                            varType = SadTypeKind::Float;
                        else if (tokType == Lexer::TokenType::STRING_LITERAL)
                            varType = SadTypeKind::String;
                        else if (tokType == Lexer::TokenType::LITERAL_TRUE ||
                                 tokType == Lexer::TokenType::LITERAL_FALSE)
                            varType = SadTypeKind::Boolean;
                    }
                }
                // (AR) ״¥†״´״§״¡ …״×״÷״± ״¹״§… (SIRGlobalVariable constructor: sir_module.h:96)
                // (EN) Create global variable
                auto sirGlobal = std::make_shared<SIRGlobalVariable>(varDecl->name, varType);

                // (AR) …״¹״§„״¬״© ״§„‚…״© ״§„״£ˆ„״© ״¥״°״§ ƒ״§†״× ״«״§״¨״×״§‹ ״­״±״§‹
                // (EN) Handle initializer if it's a literal constant
                if (varDecl->initializer)
                {
                    if (auto *litExpr = dynamic_cast<Sad::AST::LiteralExpr *>(varDecl->initializer.get()))
                    {
                        const auto &token = litExpr->token;
                        std::string value = token.getValue();
                        Lexer::TokenType tokenType = token.getType();

                        if (tokenType == Lexer::TokenType::NUMBER_INTEGER)
                        {
                            // (AR) ״×״­ˆ„ ״§„״£״¹״¯״§״¯ ״§„״³״× ״¹״´״±״©/״§„״«…״§†״©/״§„״«†״§״¦״© ״¥„‰ ״¹״´״±״©
                            // (EN) Normalize hex/octal/binary literals to decimal
                            if (value.size() > 2 && value[0] == '0')
                            {
                                char prefix = value[1];
                                if (prefix == 'x' || prefix == 'X')
                                {
                                    value = std::to_string(static_cast<int64_t>(std::stoull(value, nullptr, 16)));
                                }
                                else if (prefix == 'o' || prefix == 'O')
                                {
                                    value = std::to_string(static_cast<int64_t>(std::stoull(value.substr(2), nullptr, 8)));
                                }
                                else if (prefix == 'b' || prefix == 'B')
                                {
                                    value = std::to_string(static_cast<int64_t>(std::stoull(value.substr(2), nullptr, 2)));
                                }
                            }
                            sirGlobal->initialValue = value;
                        }
                        else if (tokenType == Lexer::TokenType::NUMBER_DOUBLE)
                        {
                            sirGlobal->initialValue = value;
                        }
                        else if (tokenType == Lexer::TokenType::LITERAL_TRUE)
                        {
                            sirGlobal->initialValue = "1";
                        }
                        else if (tokenType == Lexer::TokenType::LITERAL_FALSE)
                        {
                            sirGlobal->initialValue = "0";
                        }
                        else if (tokenType == Lexer::TokenType::STRING_LITERAL)
                        {
                            sirGlobal->initialValue = value;
                        }
                    }
                }

                // (AR) ״¥״¶״§״© ״§„…״×״÷״± ״§„״¹״§… „„ˆ״­״¯״© (sir_module.h:591 - addGlobalVariable)
                // (EN) Add global variable to module
                module_->addGlobalVariable(sirGlobal);
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
