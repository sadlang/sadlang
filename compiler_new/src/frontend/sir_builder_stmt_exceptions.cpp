// ============================================================================
// sir_builder_stmt_exceptions.cpp
// (AR) Exception handling: try/catch/finally, raise
// (EN) Exception handling statements
// ============================================================================

#include <string>
#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
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

            bool SIRBuilder::buildStatement_Exceptions(AST::Statement *stmt)
            {
                // ========================================================================
                // (AR) TryStmt - جملة حاول/التقط (statements.h:TryStmt)
                // (EN) Try-catch statement
                // ========================================================================
                if (auto tryStmt = dynamic_cast<Sad::AST::TryStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found TryStmt" << std::endl;
#endif

                    bool hasFinally = (tryStmt->finallyBlock != nullptr);
                    size_t numClauses = tryStmt->catchClauses.size();

                    // (AR) إنشاء كتل: try، catch dispatch، finally، exit
                    // (EN) Create blocks: try, catch dispatch, finally, exit
                    std::string tryLabel = newLabel("try_body");
                    std::string catchSetupLabel = newLabel("catch_setup");
                    std::string finallyLabel = newLabel("finally_body");
                    std::string exitLabel = newLabel("try_exit");

                    auto tryBlock = createBasicBlock(tryLabel);
                    auto catchSetupBlock = createBasicBlock(catchSetupLabel);
                    auto finallyBlock = createBasicBlock(finallyLabel);
                    auto exitBlock = createBasicBlock(exitLabel);

                    // (AR) كتل إضافية لـ finally الموثوق
                    // (EN) Extra blocks for reliable finally
                    std::string finallyRethrowLabel, catchSecondaryLabel;
                    std::shared_ptr<SIRBasicBlock> finallyRethrowBlock = nullptr;
                    std::shared_ptr<SIRBasicBlock> catchSecondaryBlock = nullptr;
                    if (hasFinally)
                    {
                        finallyRethrowLabel = newLabel("finally_rethrow");
                        catchSecondaryLabel = newLabel("catch_secondary");
                        finallyRethrowBlock = createBasicBlock(finallyRethrowLabel);
                        catchSecondaryBlock = createBasicBlock(catchSecondaryLabel);
                    }

                    // (AR) إنشاء كتل لكل catch clause — type check و body
                    // (EN) Create blocks for each catch clause — type check and body
                    std::vector<std::string> catchCheckLabels;
                    std::vector<std::string> catchBodyLabels;
                    std::vector<std::shared_ptr<SIRBasicBlock>> catchCheckBlocks;
                    std::vector<std::shared_ptr<SIRBasicBlock>> catchBodyBlocks;
                    // (AR) كتلة "لا تطابق" — إعادة رمي إذا لم يطابق أي catch
                    // (EN) "no match" block — rethrow if no clause matched
                    std::string noMatchLabel = newLabel("catch_no_match");
                    auto noMatchBlock = createBasicBlock(noMatchLabel);

                    for (size_t i = 0; i < numClauses; ++i)
                    {
                        std::string checkLabel = newLabel("catch_check_" + std::to_string(i));
                        std::string bodyLabel = newLabel("catch_body_" + std::to_string(i));
                        catchCheckLabels.push_back(checkLabel);
                        catchBodyLabels.push_back(bodyLabel);
                        catchCheckBlocks.push_back(createBasicBlock(checkLabel));
                        catchBodyBlocks.push_back(createBasicBlock(bodyLabel));
                    }

                    // ================================================================
                    // (AR) الإعداد: jmpbuf + setjmp
                    // (EN) Setup: jmpbuf + setjmp
                    // ================================================================
                    std::string jmpbufReg = newTempRegister();
                    {
                        SIRInstruction allocInst;
                        allocInst.opcode = SIROpcode::CALL;
                        allocInst.result = SIROperand::Register(jmpbufReg, SadTypeKind::Pointer);
                        allocInst.operands.push_back(SIROperand::Function("__sad_alloc_jmpbuf"));
                        allocInst.comment = "allocate jmpbuf for try/catch";
                        if (currentBlock_)
                            currentBlock_->addInstruction(allocInst);
                    }
                    {
                        SIRInstruction pushInst;
                        pushInst.opcode = SIROpcode::CALL;
                        pushInst.operands.push_back(SIROperand::Function("__sad_push_handler"));
                        pushInst.operands.push_back(SIROperand::Register(jmpbufReg, SadTypeKind::Pointer));
                        pushInst.comment = "push exception handler";
                        if (currentBlock_)
                            currentBlock_->addInstruction(pushInst);
                    }
                    std::string setjmpResultReg = newTempRegister();
                    {
                        SIRInstruction setjmpInst;
                        setjmpInst.opcode = SIROpcode::CALL;
                        setjmpInst.result = SIROperand::Register(setjmpResultReg, SadTypeKind::Integer);
                        setjmpInst.operands.push_back(SIROperand::Function("__sad_setjmp"));
                        setjmpInst.operands.push_back(SIROperand::Register(jmpbufReg, SadTypeKind::Pointer));
                        setjmpInst.comment = "setjmp: 0=normal, 1=exception";
                        if (currentBlock_)
                            currentBlock_->addInstruction(setjmpInst);
                    }
                    std::string cmpReg = newTempRegister();
                    {
                        SIRInstruction cmpInst = SIRInstruction::Binary(
                            SIROpcode::NE,
                            SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                            SIROperand::Register(setjmpResultReg, SadTypeKind::Integer),
                            SIROperand::ConstantI64(0));
                        if (currentBlock_)
                            currentBlock_->addInstruction(cmpInst);
                    }
                    {
                        SIRInstruction brInst = SIRInstruction::BranchCond(
                            SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                            SIROperand::Label(catchSetupLabel),
                            SIROperand::Label(tryLabel));
                        if (currentBlock_)
                            currentBlock_->addInstruction(brInst);
                    }

                    // ================================================================
                    // (AR) كتلة try
                    // (EN) Try block
                    // ================================================================
                    if (currentFunction_)
                        currentFunction_->addBasicBlock(tryBlock);
                    currentBlock_ = tryBlock;

                    // ================================================================
                    // (AR) إذا hasFinally: إنشاء سجلات لحفظ قيمة الإرجاع ودفع FinallyContext
                    //      هذا يضمن تشغيل finally حتى عند وجود ارجع داخل try أو catch
                    //      يُستخدم في buildReturnStatement للتحويل إلى branch بدل RETURN مباشرة
                    // (EN) If hasFinally: create return-value registers and push FinallyContext
                    //      Guarantees finally runs even when return is inside try or catch block
                    //      Used in buildReturnStatement to branch instead of direct RETURN
                    // ================================================================
                    std::string finallyRetI64Reg, finallyRetPtrReg, finallyRetTypeReg, finallyHasRetReg;
                    if (hasFinally)
                    {
                        // (AR) تسميات فريدة لهذه السجلات لتجنب التعارض في الدوال الكبيرة
                        // (EN) Unique names to avoid conflicts in large functions
                        std::string uniq = std::to_string(nextLabel_);
                        finallyRetI64Reg = "%__fri64_" + uniq;
                        finallyRetPtrReg = "%__frptr_" + uniq;
                        finallyRetTypeReg = "%__frtype_" + uniq;
                        finallyHasRetReg = "%__frhas_" + uniq;

                        // ALLOC i64 لقيم الأرقام
                        {
                            SIRInstruction alI;
                            alI.opcode = SIROpcode::ALLOC;
                            alI.result = SIROperand::Register(finallyRetI64Reg, SadTypeKind::Integer);
                            alI.comment = "alloca i64 for finally return value (integer)";
                            if (currentBlock_)
                                currentBlock_->addInstruction(alI);
                        }
                        // ALLOC ptr لقيم النصوص
                        {
                            SIRInstruction alP;
                            alP.opcode = SIROpcode::ALLOC;
                            alP.result = SIROperand::Register(finallyRetPtrReg, SadTypeKind::String);
                            alP.comment = "alloca ptr for finally return value (string/object)";
                            if (currentBlock_)
                                currentBlock_->addInstruction(alP);
                        }
                        // ALLOC i64 لنوع القيمة
                        {
                            SIRInstruction alT;
                            alT.opcode = SIROpcode::ALLOC;
                            alT.result = SIROperand::Register(finallyRetTypeReg, SadTypeKind::Integer);
                            alT.comment = "alloca i64 for finally return type code";
                            if (currentBlock_)
                                currentBlock_->addInstruction(alT);
                        }
                        // ALLOC i64 لعلامة "تم ارجع"
                        {
                            SIRInstruction alH;
                            alH.opcode = SIROpcode::ALLOC;
                            alH.result = SIROperand::Register(finallyHasRetReg, SadTypeKind::Integer);
                            alH.comment = "alloca i64 for finally has_return flag";
                            if (currentBlock_)
                                currentBlock_->addInstruction(alH);
                        }
                        // تهيئة has_return = 0 و type = 0 (void)
                        {
                            SIRInstruction stH;
                            stH.opcode = SIROpcode::STORE;
                            stH.operands.push_back(SIROperand::ConstantI64(0));
                            stH.operands.push_back(SIROperand::Register(finallyHasRetReg, SadTypeKind::Integer));
                            stH.comment = "init finally has_return = 0";
                            if (currentBlock_)
                                currentBlock_->addInstruction(stH);
                        }
                        {
                            SIRInstruction stT;
                            stT.opcode = SIROpcode::STORE;
                            stT.operands.push_back(SIROperand::ConstantI64(0));
                            stT.operands.push_back(SIROperand::Register(finallyRetTypeReg, SadTypeKind::Integer));
                            stT.comment = "init finally return type = 0 (void)";
                            if (currentBlock_)
                                currentBlock_->addInstruction(stT);
                        }
                        // دفع FinallyContext إلى المكدس
                        finallyStack_.push_back({finallyLabel, exitLabel,
                                                 finallyRetI64Reg, finallyRetPtrReg,
                                                 finallyRetTypeReg, finallyHasRetReg});
                    }

                    enterScope();
                    if (tryStmt->tryBlock)
                    {
                        buildStatement(tryStmt->tryBlock.get());
                    }
                    exitScope();

                    {
                        SIRInstruction popInst;
                        popInst.opcode = SIROpcode::CALL;
                        popInst.operands.push_back(SIROperand::Function("__sad_pop_handler"));
                        popInst.comment = "pop handler after successful try";
                        if (currentBlock_)
                            currentBlock_->addInstruction(popInst);
                    }
                    if (currentBlock_)
                    {
                        currentBlock_->addInstruction(SIRInstruction::Branch(
                            SIROperand::Label(hasFinally ? finallyLabel : exitLabel)));
                    }

                    // ================================================================
                    // (AR) كتلة catch_setup — إزالة المعالج الأول + إعداد معالج ثانوي لـ finally
                    // (EN) catch_setup — pop primary handler + setup secondary for finally
                    // ================================================================
                    if (currentFunction_)
                        currentFunction_->addBasicBlock(catchSetupBlock);
                    currentBlock_ = catchSetupBlock;

                    {
                        SIRInstruction popInst;
                        popInst.opcode = SIROpcode::CALL;
                        popInst.operands.push_back(SIROperand::Function("__sad_pop_handler"));
                        popInst.comment = "pop primary handler at catch entry";
                        if (currentBlock_)
                            currentBlock_->addInstruction(popInst);
                    }

                    if (hasFinally)
                    {
                        // (AR) دفع معالج ثانوي يحمي catch — لضمان تنفيذ finally
                        // (EN) Push secondary handler protecting catch — guarantees finally runs
                        std::string jmpbuf2Reg = newTempRegister();
                        {
                            SIRInstruction allocInst;
                            allocInst.opcode = SIROpcode::CALL;
                            allocInst.result = SIROperand::Register(jmpbuf2Reg, SadTypeKind::Pointer);
                            allocInst.operands.push_back(SIROperand::Function("__sad_alloc_jmpbuf"));
                            allocInst.comment = "allocate secondary jmpbuf for finally protection";
                            if (currentBlock_)
                                currentBlock_->addInstruction(allocInst);
                        }
                        {
                            SIRInstruction pushInst;
                            pushInst.opcode = SIROpcode::CALL;
                            pushInst.operands.push_back(SIROperand::Function("__sad_push_handler"));
                            pushInst.operands.push_back(SIROperand::Register(jmpbuf2Reg, SadTypeKind::Pointer));
                            pushInst.comment = "push secondary handler for finally";
                            if (currentBlock_)
                                currentBlock_->addInstruction(pushInst);
                        }
                        std::string setjmp2Reg = newTempRegister();
                        {
                            SIRInstruction setjmpInst;
                            setjmpInst.opcode = SIROpcode::CALL;
                            setjmpInst.result = SIROperand::Register(setjmp2Reg, SadTypeKind::Integer);
                            setjmpInst.operands.push_back(SIROperand::Function("__sad_setjmp"));
                            setjmpInst.operands.push_back(SIROperand::Register(jmpbuf2Reg, SadTypeKind::Pointer));
                            setjmpInst.comment = "secondary setjmp: exception inside catch → finally_rethrow";
                            if (currentBlock_)
                                currentBlock_->addInstruction(setjmpInst);
                        }
                        std::string cmp2Reg = newTempRegister();
                        {
                            SIRInstruction cmpInst = SIRInstruction::Binary(
                                SIROpcode::NE,
                                SIROperand::Register(cmp2Reg, SadTypeKind::Boolean),
                                SIROperand::Register(setjmp2Reg, SadTypeKind::Integer),
                                SIROperand::ConstantI64(0));
                            if (currentBlock_)
                                currentBlock_->addInstruction(cmpInst);
                        }
                        {
                            // (AR) إذا استثناء داخل catch → finally_rethrow
                            // (EN) If exception inside catch → finally_rethrow
                            SIRInstruction brInst = SIRInstruction::BranchCond(
                                SIROperand::Register(cmp2Reg, SadTypeKind::Boolean),
                                SIROperand::Label(finallyRethrowLabel),
                                SIROperand::Label(numClauses > 0 ? catchCheckLabels[0] : noMatchLabel));
                            if (currentBlock_)
                                currentBlock_->addInstruction(brInst);
                        }
                    }
                    else
                    {
                        // (AR) بدون finally — اذهب مباشرة لفحص الأنواع
                        // (EN) No finally — go directly to type checks
                        if (currentBlock_)
                        {
                            currentBlock_->addInstruction(SIRInstruction::Branch(
                                SIROperand::Label(numClauses > 0 ? catchCheckLabels[0] : noMatchLabel)));
                        }
                    }

                    // ================================================================
                    // (AR) كتل فحص النوع لكل catch clause
                    // (EN) Type check blocks for each catch clause
                    // ================================================================
                    for (size_t i = 0; i < numClauses; ++i)
                    {
                        auto &clause = tryStmt->catchClauses[i];

                        if (currentFunction_)
                            currentFunction_->addBasicBlock(catchCheckBlocks[i]);
                        currentBlock_ = catchCheckBlocks[i];

                        if (clause.exceptionType == Sad::Data::DataType::UNKNOWN)
                        {
                            // (AR) catch-all: يطابق أي استثناء — اذهب مباشرة للجسم
                            // (EN) Catch-all: matches any exception — go directly to body
                            if (currentBlock_)
                            {
                                currentBlock_->addInstruction(SIRInstruction::Branch(
                                    SIROperand::Label(catchBodyLabels[i])));
                            }
                        }
                        else if (clause.exceptionType == Sad::Data::DataType::ERROR)
                        {
                            // (AR) مطابقة نوع ERROR — قارن مع "خطأ"
                            // (EN) Match ERROR type — compare with "خطأ"
                            std::string excTypeReg = newTempRegister();
                            {
                                SIRInstruction getTypeInst;
                                getTypeInst.opcode = SIROpcode::CALL;
                                getTypeInst.result = SIROperand::Register(excTypeReg, SadTypeKind::String);
                                getTypeInst.operands.push_back(SIROperand::Function("__sad_get_exception_type"));
                                getTypeInst.comment = "get exception type for matching";
                                if (currentBlock_)
                                    currentBlock_->addInstruction(getTypeInst);
                            }
                            std::string matchReg = newTempRegister();
                            {
                                SIRInstruction cmpInst;
                                cmpInst.opcode = SIROpcode::CALL;
                                cmpInst.result = SIROperand::Register(matchReg, SadTypeKind::Integer);
                                cmpInst.operands.push_back(SIROperand::Function("__sad_str_equals"));
                                cmpInst.operands.push_back(SIROperand::Register(excTypeReg, SadTypeKind::String));
                                cmpInst.operands.push_back(SIROperand::ConstantString("\xd8\xae\xd8\xb7\xd8\xa3")); // "خطأ"
                                cmpInst.comment = "compare exception type with ERROR";
                                if (currentBlock_)
                                    currentBlock_->addInstruction(cmpInst);
                            }
                            std::string nextTarget = (i + 1 < numClauses) ? catchCheckLabels[i + 1] : noMatchLabel;
                            {
                                SIRInstruction brInst = SIRInstruction::BranchCond(
                                    SIROperand::Register(matchReg, SadTypeKind::Boolean),
                                    SIROperand::Label(catchBodyLabels[i]),
                                    SIROperand::Label(nextTarget));
                                if (currentBlock_)
                                    currentBlock_->addInstruction(brInst);
                            }
                        }
                        else if (clause.exceptionType == Sad::Data::DataType::OBJECT)
                        {
                            // (AR) مطابقة نوع كائن مخصص — قارن مع اسم الصنف
                            // (EN) Match custom object type — compare with class name
                            std::string typeName = clause.exceptionTypeName;
                            if (typeName.empty())
                                typeName = clause.exceptionVar;

                            std::string excTypeReg = newTempRegister();
                            {
                                SIRInstruction getTypeInst;
                                getTypeInst.opcode = SIROpcode::CALL;
                                getTypeInst.result = SIROperand::Register(excTypeReg, SadTypeKind::String);
                                getTypeInst.operands.push_back(SIROperand::Function("__sad_get_exception_type"));
                                getTypeInst.comment = "get exception type for object matching";
                                if (currentBlock_)
                                    currentBlock_->addInstruction(getTypeInst);
                            }
                            std::string matchReg = newTempRegister();
                            {
                                SIRInstruction cmpInst;
                                cmpInst.opcode = SIROpcode::CALL;
                                cmpInst.result = SIROperand::Register(matchReg, SadTypeKind::Integer);
                                cmpInst.operands.push_back(SIROperand::Function("__sad_str_equals"));
                                cmpInst.operands.push_back(SIROperand::Register(excTypeReg, SadTypeKind::String));
                                cmpInst.operands.push_back(SIROperand::ConstantString(typeName));
                                cmpInst.comment = "compare exception type with " + typeName;
                                if (currentBlock_)
                                    currentBlock_->addInstruction(cmpInst);
                            }
                            std::string nextTarget = (i + 1 < numClauses) ? catchCheckLabels[i + 1] : noMatchLabel;
                            {
                                SIRInstruction brInst = SIRInstruction::BranchCond(
                                    SIROperand::Register(matchReg, SadTypeKind::Boolean),
                                    SIROperand::Label(catchBodyLabels[i]),
                                    SIROperand::Label(nextTarget));
                                if (currentBlock_)
                                    currentBlock_->addInstruction(brInst);
                            }
                        }
                        else
                        {
                            // (AR) نوع غير معروف — يعامل كـ catch-all
                            // (EN) Unknown type — treated as catch-all
                            if (currentBlock_)
                            {
                                currentBlock_->addInstruction(SIRInstruction::Branch(
                                    SIROperand::Label(catchBodyLabels[i])));
                            }
                        }
                    }

                    // ================================================================
                    // (AR) كتل أجسام catch
                    // (EN) Catch body blocks
                    // ================================================================
                    std::string catchExitDest = hasFinally ? finallyLabel : exitLabel;

                    for (size_t i = 0; i < numClauses; ++i)
                    {
                        auto &clause = tryStmt->catchClauses[i];

                        if (currentFunction_)
                            currentFunction_->addBasicBlock(catchBodyBlocks[i]);
                        currentBlock_ = catchBodyBlocks[i];

                        enterScope();

                        if (!clause.exceptionVar.empty())
                        {
                            std::string exAllocReg = "%" + clause.exceptionVar;
                            std::string exReg = newTempRegister();

                            {
                                SIRInstruction allocInst;
                                allocInst.opcode = SIROpcode::ALLOC;
                                allocInst.result = SIROperand::Register(exAllocReg, SadTypeKind::String);
                                if (currentBlock_)
                                    currentBlock_->addInstruction(allocInst);
                            }
                            {
                                SIRInstruction loadExInst;
                                loadExInst.opcode = SIROpcode::CALL;
                                loadExInst.result = SIROperand::Register(exReg, SadTypeKind::String);
                                loadExInst.operands.push_back(SIROperand::Function("__sad_get_exception"));
                                loadExInst.comment = "load caught exception into " + clause.exceptionVar;
                                if (currentBlock_)
                                    currentBlock_->addInstruction(loadExInst);
                            }
                            {
                                SIRInstruction storeInst;
                                storeInst.opcode = SIROpcode::STORE;
                                storeInst.operands.push_back(SIROperand::Register(exReg, SadTypeKind::String));
                                storeInst.operands.push_back(SIROperand::Register(exAllocReg, SadTypeKind::String));
                                if (currentBlock_)
                                    currentBlock_->addInstruction(storeInst);
                            }

                            VariableInfo exVar;
                            exVar.name = clause.exceptionVar;
                            exVar.type = SadTypeKind::String;
                            exVar.registerName = exAllocReg;
                            exVar.isMutable = false;
                            exVar.scopeLevel = currentScopeLevel_;
                            addVariable(exVar);
                        }

                        if (clause.body)
                        {
                            buildStatement(clause.body.get());
                        }

                        exitScope();

                        // (AR) إزالة المعالج الثانوي قبل الخروج (إذا وجد)
                        // (EN) Pop secondary handler before exit (if exists)
                        if (hasFinally)
                        {
                            SIRInstruction popInst;
                            popInst.opcode = SIROpcode::CALL;
                            popInst.operands.push_back(SIROperand::Function("__sad_pop_handler"));
                            popInst.comment = "pop secondary handler after catch body";
                            if (currentBlock_)
                                currentBlock_->addInstruction(popInst);
                        }

                        if (currentBlock_)
                        {
                            currentBlock_->addInstruction(SIRInstruction::Branch(
                                SIROperand::Label(catchExitDest)));
                        }
                    }

                    // ================================================================
                    // (AR) كتلة "لا تطابق" — إعادة رمي الاستثناء
                    // (EN) "No match" block — rethrow exception
                    // ================================================================
                    if (currentFunction_)
                        currentFunction_->addBasicBlock(noMatchBlock);
                    currentBlock_ = noMatchBlock;

                    if (hasFinally)
                    {
                        // (AR) إزالة المعالج الثانوي ثم تنفيذ finally ثم إعادة الرمي
                        // (EN) Pop secondary handler, run finally, then rethrow
                        {
                            SIRInstruction popInst;
                            popInst.opcode = SIROpcode::CALL;
                            popInst.operands.push_back(SIROperand::Function("__sad_pop_handler"));
                            popInst.comment = "pop secondary handler before rethrow";
                            if (currentBlock_)
                                currentBlock_->addInstruction(popInst);
                        }
                        // (AR) تنفيذ finally ثم إعادة رمي
                        // (EN) Run finally then rethrow
                        if (tryStmt->finallyBlock)
                        {
                            enterScope();
                            buildStatement(tryStmt->finallyBlock.get());
                            exitScope();
                        }
                        // (AR) إعادة رمي الاستثناء الحالي
                        // (EN) Re-throw current exception
                        {
                            SIRInstruction rethrowInst;
                            rethrowInst.opcode = SIROpcode::CALL;
                            rethrowInst.operands.push_back(SIROperand::Function("__sad_raise_current"));
                            rethrowInst.comment = "rethrow unmatched exception after finally";
                            if (currentBlock_)
                                currentBlock_->addInstruction(rethrowInst);
                        }
                        // (AR) كتلة ميتة
                        // (EN) Dead block
                        std::string deadLabel = newLabel("dead_after_nomatch");
                        auto deadBlock = createBasicBlock(deadLabel);
                        if (currentFunction_)
                            currentFunction_->addBasicBlock(deadBlock);
                        currentBlock_ = deadBlock;
                    }
                    else
                    {
                        // (AR) بدون finally — إعادة رمي مباشرة
                        // (EN) Without finally — direct rethrow
                        {
                            SIRInstruction rethrowInst;
                            rethrowInst.opcode = SIROpcode::CALL;
                            rethrowInst.operands.push_back(SIROperand::Function("__sad_raise_current"));
                            rethrowInst.comment = "rethrow unmatched exception";
                            if (currentBlock_)
                                currentBlock_->addInstruction(rethrowInst);
                        }
                        std::string deadLabel = newLabel("dead_after_nomatch");
                        auto deadBlock = createBasicBlock(deadLabel);
                        if (currentFunction_)
                            currentFunction_->addBasicBlock(deadBlock);
                        currentBlock_ = deadBlock;
                    }

                    // ================================================================
                    // (AR) كتلة finally_rethrow — استثناء داخل catch + تنفيذ finally + إعادة رمي
                    // (EN) finally_rethrow — exception inside catch + run finally + rethrow
                    // ================================================================
                    if (hasFinally && finallyRethrowBlock)
                    {
                        if (currentFunction_)
                            currentFunction_->addBasicBlock(finallyRethrowBlock);
                        currentBlock_ = finallyRethrowBlock;

                        // (AR) إزالة المعالج الثانوي
                        // (EN) Pop secondary handler
                        {
                            SIRInstruction popInst;
                            popInst.opcode = SIROpcode::CALL;
                            popInst.operands.push_back(SIROperand::Function("__sad_pop_handler"));
                            popInst.comment = "pop secondary handler in finally_rethrow";
                            if (currentBlock_)
                                currentBlock_->addInstruction(popInst);
                        }

                        // (AR) تنفيذ كود finally
                        // (EN) Execute finally code
                        if (tryStmt->finallyBlock)
                        {
                            enterScope();
                            buildStatement(tryStmt->finallyBlock.get());
                            exitScope();
                        }

                        // (AR) إعادة رمي الاستثناء
                        // (EN) Rethrow exception
                        {
                            SIRInstruction rethrowInst;
                            rethrowInst.opcode = SIROpcode::CALL;
                            rethrowInst.operands.push_back(SIROperand::Function("__sad_raise_current"));
                            rethrowInst.comment = "rethrow exception after finally";
                            if (currentBlock_)
                                currentBlock_->addInstruction(rethrowInst);
                        }
                        // (AR) كتلة ميتة
                        // (EN) Dead block
                        std::string deadLabel = newLabel("dead_after_finally_rethrow");
                        auto deadRethrowBlock = createBasicBlock(deadLabel);
                        if (currentFunction_)
                            currentFunction_->addBasicBlock(deadRethrowBlock);
                        currentBlock_ = deadRethrowBlock;
                    }

                    // ================================================================
                    // (AR) كتلة finally — المسار العادي
                    //      يُدخل هذه الكتلة من: نهاية try الناجح، نهاية catch عادي
                    //      لكن ليس من ارجع داخل try/catch (تلك تذهب مباشرة لـ finallyLabel أيضاً)
                    //      بعد finally: فحص has_return → إرجاع القيمة المحفوظة أو الاستمرار للخروج
                    // (EN) Finally block — normal path
                    //      Entered from: successful try end, normal catch end
                    //      Return intercepted by FinallyContext also branches here
                    //      After finally: check has_return → return saved value or continue to exit
                    // ================================================================
                    if (hasFinally)
                    {
                        // (AR) إزالة FinallyContext من المكدس قبل بناء جسم finally
                        //      هذا يضمن أن ارجع داخل finally نفسه لا يعود لـ finally مرة أخرى
                        // (EN) Pop FinallyContext before building finally body
                        //      Ensures return inside finally itself does NOT loop back
                        if (!finallyStack_.empty() && finallyStack_.back().finallyLabel == finallyLabel)
                        {
                            finallyStack_.pop_back();
                        }

                        if (currentFunction_)
                            currentFunction_->addBasicBlock(finallyBlock);
                        currentBlock_ = finallyBlock;

                        if (tryStmt->finallyBlock)
                        {
                            enterScope();
                            buildStatement(tryStmt->finallyBlock.get());
                            exitScope();
                        }

                        // ================================================================
                        // (AR) بعد تنفيذ finally: فحص has_return
                        //      إذا has_return == 1 → أرجع القيمة المحفوظة (مسار ارجع)
                        //      إذا has_return == 0 → انتقل للخروج الطبيعي
                        // (EN) After finally executes: check has_return flag
                        //      If has_return == 1 → return the saved value (intercepted return path)
                        //      If has_return == 0 → branch to normal exit
                        // ================================================================
                        if (currentBlock_ && !finallyHasRetReg.empty())
                        {
                            // (AR) تحميل علامة has_return
                            // (EN) Load has_return flag
                            std::string hasRetLoadReg = newTempRegister();
                            {
                                SIRInstruction ld;
                                ld.opcode = SIROpcode::LOAD;
                                ld.result = SIROperand::Register(hasRetLoadReg, SadTypeKind::Integer);
                                ld.operands.push_back(SIROperand::Register(finallyHasRetReg, SadTypeKind::Integer));
                                ld.comment = "load finally has_return flag";
                                currentBlock_->addInstruction(ld);
                            }
                            std::string hasCmpReg = newTempRegister();
                            {
                                SIRInstruction cmp = SIRInstruction::Binary(
                                    SIROpcode::NE,
                                    SIROperand::Register(hasCmpReg, SadTypeKind::Boolean),
                                    SIROperand::Register(hasRetLoadReg, SadTypeKind::Integer),
                                    SIROperand::ConstantI64(0));
                                cmp.comment = "check: has_return != 0?";
                                currentBlock_->addInstruction(cmp);
                            }

                            // (AR) إنشاء كتل: مسار الإرجاع ومسار الاستمرار الطبيعي
                            // (EN) Create blocks: return path and normal continuation path
                            std::string frRetLabel = newLabel("finally_ret_start");
                            std::string frContLabel = newLabel("finally_cont_normal");
                            auto frRetBlock = createBasicBlock(frRetLabel);
                            auto frContBlock = createBasicBlock(frContLabel);

                            {
                                SIRInstruction br = SIRInstruction::BranchCond(
                                    SIROperand::Register(hasCmpReg, SadTypeKind::Boolean),
                                    SIROperand::Label(frRetLabel),
                                    SIROperand::Label(frContLabel));
                                currentBlock_->addInstruction(br);
                            }

                            // ════════════════════════════════════════════════════════
                            // (AR) كتلة مسار الإرجاع: فحص النوع وإرجاع القيمة المناسبة
                            //      typeCode: 2=نص/مؤشر، 0=فراغ، 1/3=رقم/عشري
                            // (EN) Return path block: check type and return proper value
                            //      typeCode: 2=string/ptr, 0=void, 1/3=integer/float
                            // ════════════════════════════════════════════════════════
                            if (currentFunction_)
                                currentFunction_->addBasicBlock(frRetBlock);
                            currentBlock_ = frRetBlock;

                            // (AR) تحميل نوع القيمة المحفوظة
                            // (EN) Load saved return type code
                            std::string typeLoadReg = newTempRegister();
                            {
                                SIRInstruction ld;
                                ld.opcode = SIROpcode::LOAD;
                                ld.result = SIROperand::Register(typeLoadReg, SadTypeKind::Integer);
                                ld.operands.push_back(SIROperand::Register(finallyRetTypeReg, SadTypeKind::Integer));
                                ld.comment = "load finally return type code";
                                currentBlock_->addInstruction(ld);
                            }

                            // (AR) فحص: type == 2 (نص/مؤشر)؟
                            // (EN) Check: type == 2 (string/ptr)?
                            std::string isPtrReg = newTempRegister();
                            {
                                SIRInstruction cmp = SIRInstruction::Binary(
                                    SIROpcode::EQ,
                                    SIROperand::Register(isPtrReg, SadTypeKind::Boolean),
                                    SIROperand::Register(typeLoadReg, SadTypeKind::Integer),
                                    SIROperand::ConstantI64(2));
                                cmp.comment = "check: type == 2 (ptr/string)?";
                                currentBlock_->addInstruction(cmp);
                            }

                            std::string retPtrLabel = newLabel("finally_ret_ptr");
                            std::string retNonPtrLabel = newLabel("finally_ret_nonptr");
                            auto retPtrBlock = createBasicBlock(retPtrLabel);
                            auto retNonPtrBlock = createBasicBlock(retNonPtrLabel);

                            {
                                SIRInstruction br = SIRInstruction::BranchCond(
                                    SIROperand::Register(isPtrReg, SadTypeKind::Boolean),
                                    SIROperand::Label(retPtrLabel),
                                    SIROperand::Label(retNonPtrLabel));
                                currentBlock_->addInstruction(br);
                            }

                            // ── مسار إرجاع نص/مؤشر (type == 2) ──
                            if (currentFunction_)
                                currentFunction_->addBasicBlock(retPtrBlock);
                            currentBlock_ = retPtrBlock;
                            {
                                emitPopFunctionCleanupHandler();

                                std::string loadedPtr = newTempRegister();
                                SIRInstruction ld;
                                ld.opcode = SIROpcode::LOAD;
                                ld.result = SIROperand::Register(loadedPtr, SadTypeKind::String);
                                ld.operands.push_back(SIROperand::Register(finallyRetPtrReg, SadTypeKind::String));
                                ld.comment = "load finally return string/ptr";
                                currentBlock_->addInstruction(ld);

                                SIRInstruction retI;
                                retI.opcode = SIROpcode::RET;
                                retI.operands.push_back(SIROperand::Register(loadedPtr, SadTypeKind::String));
                                retI.comment = "return string/ptr value from finally";
                                currentBlock_->addInstruction(retI);
                            }

                            // ── مسار إرجاع رقم/فراغ (type != 2) ──
                            if (currentFunction_)
                                currentFunction_->addBasicBlock(retNonPtrBlock);
                            currentBlock_ = retNonPtrBlock;

                            // (AR) فحص: type == 0 (فراغ)؟
                            // (EN) Check: type == 0 (void)?
                            std::string isVoidReg = newTempRegister();
                            {
                                SIRInstruction cmp = SIRInstruction::Binary(
                                    SIROpcode::EQ,
                                    SIROperand::Register(isVoidReg, SadTypeKind::Boolean),
                                    SIROperand::Register(typeLoadReg, SadTypeKind::Integer),
                                    SIROperand::ConstantI64(0));
                                cmp.comment = "check: type == 0 (void)?";
                                currentBlock_->addInstruction(cmp);
                            }

                            std::string retVoidLabel2 = newLabel("finally_ret_void");
                            std::string retI64Label2 = newLabel("finally_ret_i64");
                            auto retVoidBlock = createBasicBlock(retVoidLabel2);
                            auto retI64Block = createBasicBlock(retI64Label2);

                            {
                                SIRInstruction br = SIRInstruction::BranchCond(
                                    SIROperand::Register(isVoidReg, SadTypeKind::Boolean),
                                    SIROperand::Label(retVoidLabel2),
                                    SIROperand::Label(retI64Label2));
                                currentBlock_->addInstruction(br);
                            }

                            // ── مسار إرجاع فراغ ──
                            if (currentFunction_)
                                currentFunction_->addBasicBlock(retVoidBlock);
                            currentBlock_ = retVoidBlock;
                            {
                                emitPopFunctionCleanupHandler();

                                SIRInstruction retV;
                                retV.opcode = SIROpcode::RET_VOID;
                                retV.comment = "return void from finally";
                                currentBlock_->addInstruction(retV);
                            }

                            // ── مسار إرجاع رقم (type == 1 أو 3) ──
                            if (currentFunction_)
                                currentFunction_->addBasicBlock(retI64Block);
                            currentBlock_ = retI64Block;
                            {
                                emitPopFunctionCleanupHandler();

                                std::string loadedI64 = newTempRegister();
                                SIRInstruction ld;
                                ld.opcode = SIROpcode::LOAD;
                                ld.result = SIROperand::Register(loadedI64, SadTypeKind::Integer);
                                ld.operands.push_back(SIROperand::Register(finallyRetI64Reg, SadTypeKind::Integer));
                                ld.comment = "load finally return i64/float value";
                                currentBlock_->addInstruction(ld);

                                SIRInstruction retI;
                                retI.opcode = SIROpcode::RET;
                                retI.operands.push_back(SIROperand::Register(loadedI64, SadTypeKind::Integer));
                                retI.comment = "return i64 value from finally";
                                currentBlock_->addInstruction(retI);
                            }

                            // ════════════════════════════════════════════════════════
                            // (AR) كتلة الاستمرار الطبيعي: اذهب للخروج (has_return == 0)
                            // (EN) Normal continuation block: go to exit (has_return == 0)
                            // ════════════════════════════════════════════════════════
                            if (currentFunction_)
                                currentFunction_->addBasicBlock(frContBlock);
                            currentBlock_ = frContBlock;
                            currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(exitLabel)));
                        }
                        else
                        {
                            // (AR) لا سجلات finally (بدون ارجع) — مسار عادي للخروج
                            // (EN) No finally registers (no return) — normal exit path
                            if (currentBlock_)
                                currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(exitLabel)));
                        }
                    }

                    // (AR) كتلة الخروج
                    // (EN) Exit block
                    if (currentFunction_)
                        currentFunction_->addBasicBlock(exitBlock);
                    currentBlock_ = exitBlock;
                    return true;
                }

                // ========================================================================
                // (AR) RaiseStmt - جملة رفع استثناء: ارفع خطأ("...")
                // (EN) Raise/throw statement
                // ========================================================================
                if (auto raiseStmt = dynamic_cast<Sad::AST::RaiseStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found RaiseStmt" << std::endl;
#endif

                    // (AR) بناء تعبير الاستثناء
                    // (EN) Build the exception expression
                    BuildResult exResult;
                    if (raiseStmt->exception)
                    {
                        exResult = buildExpression(raiseStmt->exception.get());
                    }

                    // (AR) استدعاء دالة runtime لرفع الاستثناء مع النوع
                    // (EN) Call runtime function to raise exception with type
                    // (AR) الصيغة: __sad_raise(type, msg) — إذا كان كائناً نستخدم اسم الصنف، وإلا "خطأ"
                    // (EN) Format: __sad_raise(type, msg) — use class name if object, else "خطأ"
                    SIRInstruction raiseInst;
                    raiseInst.opcode = SIROpcode::CALL;
                    raiseInst.operands.push_back(SIROperand::Function("__sad_raise"));

                    // (AR) تحديد نوع الاستثناء: إذا كان NewExpr نستخدم اسم الصنف
                    // (EN) Determine exception type: if NewExpr, use class name
                    std::string exceptionTypeName = "\xd8\xae\xd8\xb7\xd8\xa3"; // "خطأ" default
                    if (raiseStmt->exception)
                    {
                        if (auto *newExpr = dynamic_cast<Sad::AST::NewExpr *>(raiseStmt->exception.get()))
                        {
                            exceptionTypeName = newExpr->className;
                        }
                    }
                    raiseInst.operands.push_back(SIROperand::ConstantString(exceptionTypeName));

                    // (AR) الوسيط الثاني: رسالة الاستثناء
                    // (EN) Second arg: exception message
                    if (exResult.isConstant && exResult.type == SadTypeKind::String)
                    {
                        raiseInst.operands.push_back(SIROperand::ConstantString(exResult.constantValue));
                    }
                    else if (!exResult.registerName.empty())
                    {
                        raiseInst.operands.push_back(SIROperand::Register(exResult.registerName, exResult.type));
                    }
                    raiseInst.comment = "raise exception (type + message)";

                    if (currentBlock_)
                    {
                        currentBlock_->addInstruction(raiseInst);
                    }

                    // (AR) بعد الرفع، لا يمكن الوصول لهذه النقطة — كتلة للكود الميت
                    // (EN) After raise, this point is unreachable — dead code block
                    // (AR) لا نصدر RET_VOID لأن __sad_raise يستدعي longjmp فلا يعود أبداً
                    // (EN) No RET_VOID because __sad_raise calls longjmp and never returns

                    // (AR) كتلة جديدة للكود بعد raise (كود ميت)
                    // (EN) New block for code after raise (dead code)
                    std::string afterRaiseLabel = newLabel("after_raise");
                    auto afterRaiseBlock = createBasicBlock(afterRaiseLabel);
                    if (currentFunction_)
                        currentFunction_->addBasicBlock(afterRaiseBlock);
                    currentBlock_ = afterRaiseBlock;
                    return true;
                }

                // ========================================================================

                return false;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad