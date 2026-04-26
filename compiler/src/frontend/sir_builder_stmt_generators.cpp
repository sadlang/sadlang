// ============================================================================
// sir_builder_stmt_generators.cpp
// (AR) Generator statements: yield, with (context manager)
// (EN) Generator and context manager statements
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
#include <set>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            bool SIRBuilder::buildStatement_Generators(AST::Statement *stmt)
            {
                // (AR) YieldStmt - جملة yield (مولّد): اعطِ قيمة
                // (EN) Yield statement (generator)
                // ========================================================================
                if (auto yieldStmt = dynamic_cast<Sad::AST::YieldStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found YieldStmt" << std::endl;
#endif

                    BuildResult valResult;
                    if (yieldStmt->value)
                    {
                        valResult = buildExpression(yieldStmt->value.get());
                    }

                    // ================================================================
                    // (AR) إذا كان yield from — تكرار على iterable وإنتاج كل عنصر
                    // (EN) If yield from — iterate over iterable and yield each element
                    // ================================================================
                    if (yieldStmt->isYieldFrom)
                    {
                        // (AR) الحصول على طول المصفوفة/المتكرر
                        // (EN) Get length of array/iterable
                        std::string lenReg = newTempRegister();
                        {
                            SIRInstruction lenInst;
                            lenInst.opcode = SIROpcode::ARRAY_LEN;
                            lenInst.result = SIROperand::Register(lenReg, SadTypeKind::Integer);
                            lenInst.operands.push_back(SIROperand::Register(valResult.registerName, valResult.type));
                            lenInst.comment = "yield from: get iterable length";
                            if (currentBlock_)
                                currentBlock_->addInstruction(lenInst);
                        }

                        // (AR) عداد الحلقة
                        // (EN) Loop counter
                        std::string idxReg = newTempRegister();
                        {
                            SIRInstruction initInst;
                            initInst.opcode = SIROpcode::MOVE;
                            initInst.result = SIROperand::Register(idxReg, SadTypeKind::Integer);
                            initInst.operands.push_back(SIROperand::ConstantI64(0));
                            initInst.comment = "yield from: init counter";
                            if (currentBlock_)
                                currentBlock_->addInstruction(initInst);
                        }

                        // (AR) إنشاء كتل الحلقة
                        // (EN) Create loop blocks
                        std::string yieldLoopCondLabel = newLabel("yield_from_cond");
                        std::string yieldLoopBodyLabel = newLabel("yield_from_body");
                        std::string yieldLoopExitLabel = newLabel("yield_from_exit");

                        auto yieldCondBlock = createBasicBlock(yieldLoopCondLabel);
                        auto yieldBodyBlock = createBasicBlock(yieldLoopBodyLabel);
                        auto yieldExitBlock = createBasicBlock(yieldLoopExitLabel);

                        if (currentBlock_)
                        {
                            currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(yieldLoopCondLabel)));
                        }

                        // (AR) كتلة الشرط
                        // (EN) Condition block
                        if (currentFunction_)
                            currentFunction_->addBasicBlock(yieldCondBlock);
                        currentBlock_ = yieldCondBlock;

                        std::string cmpReg = newTempRegister();
                        {
                            SIRInstruction cmpInst = SIRInstruction::Binary(
                                SIROpcode::LT,
                                SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                                SIROperand::Register(idxReg, SadTypeKind::Integer),
                                SIROperand::Register(lenReg, SadTypeKind::Integer));
                            if (currentBlock_)
                                currentBlock_->addInstruction(cmpInst);
                        }
                        {
                            SIRInstruction brInst = SIRInstruction::BranchCond(
                                SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                                SIROperand::Label(yieldLoopBodyLabel),
                                SIROperand::Label(yieldLoopExitLabel));
                            if (currentBlock_)
                                currentBlock_->addInstruction(brInst);
                        }

                        // (AR) كتلة الجسم — استخراج العنصر وإنتاجه
                        // (EN) Body block — extract element and yield it
                        if (currentFunction_)
                            currentFunction_->addBasicBlock(yieldBodyBlock);
                        currentBlock_ = yieldBodyBlock;

                        std::string elemReg = newTempRegister();
                        {
                            SIRInstruction getInst;
                            getInst.opcode = SIROpcode::ARRAY_GET;
                            getInst.result = SIROperand::Register(elemReg, SadTypeKind::Pointer);
                            getInst.operands.push_back(SIROperand::Register(valResult.registerName, valResult.type));
                            getInst.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                            getInst.comment = "yield from: get element at index";
                            if (currentBlock_)
                                currentBlock_->addInstruction(getInst);
                        }

                        // (AR) إنتاج العنصر
                        // (EN) Yield the element
                        {
                            SIRInstruction yieldInst;
                            yieldInst.opcode = SIROpcode::GENERATOR_YIELD;
                            yieldInst.operands.push_back(SIROperand::Register(elemReg, SadTypeKind::Pointer));
                            yieldInst.comment = "yield from: yield delegated element";
                            if (currentBlock_)
                                currentBlock_->addInstruction(yieldInst);
                        }

                        // (AR) زيادة العداد
                        // (EN) Increment counter
                        {
                            SIRInstruction incInst = SIRInstruction::Binary(
                                SIROpcode::ADD_I64,
                                SIROperand::Register(idxReg, SadTypeKind::Integer),
                                SIROperand::Register(idxReg, SadTypeKind::Integer),
                                SIROperand::ConstantI64(1));
                            incInst.comment = "yield from: increment counter";
                            if (currentBlock_)
                                currentBlock_->addInstruction(incInst);
                        }

                        if (currentBlock_)
                        {
                            currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(yieldLoopCondLabel)));
                        }

                        // (AR) كتلة الخروج من الحلقة
                        // (EN) Loop exit block
                        if (currentFunction_)
                            currentFunction_->addBasicBlock(yieldExitBlock);
                        currentBlock_ = yieldExitBlock;

                        return true;
                    }

                    // ================================================================
                    // (AR) yield عادي — إنتاج قيمة واحدة
                    // (EN) Normal yield — produce single value
                    // ================================================================
                    SIRInstruction yieldInst;
                    yieldInst.opcode = SIROpcode::GENERATOR_YIELD;
                    // (AR) أولوية: الثابت أولاً، ثم السجل
                    // (EN) Priority: constant first, then register
                    if (valResult.isConstant && !valResult.constantValue.empty())
                    {
                        if (valResult.type == SadTypeKind::String)
                        {
                            yieldInst.operands.push_back(SIROperand::ConstantString(valResult.constantValue));
                        }
                        else if (valResult.type == SadTypeKind::Float)
                        {
                            yieldInst.operands.push_back(SIROperand::ConstantF64(std::stod(valResult.constantValue)));
                        }
                        else
                        {
                            yieldInst.operands.push_back(SIROperand::ConstantI64(std::stoll(valResult.constantValue)));
                        }
                    }
                    else if (!valResult.registerName.empty())
                    {
                        yieldInst.operands.push_back(SIROperand::Register(valResult.registerName, valResult.type));
                    }
                    else
                    {
                        // (AR) أنتج بدون قيمة — ينتج 0
                        // (EN) yield without value — yields 0
                        yieldInst.operands.push_back(SIROperand::ConstantI64(0));
                    }
                    yieldInst.comment = "yield";

                    if (currentBlock_)
                    {
                        currentBlock_->addInstruction(yieldInst);
                    }
                    return true;
                }

                // ========================================================================
                // (AR) WithStmt - جملة باستخدام (مدير سياق)
                // (EN) With statement (context manager)
                // ========================================================================
                if (auto withStmt = dynamic_cast<Sad::AST::WithStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found WithStmt" << std::endl;
#endif

                    enterScope();

                    // (AR) بناء تعبير المورد
                    // (EN) Build resource expression
                    auto resResult = buildExpression(withStmt->resource.get());

                    // (AR) تسجيل المتغير المستعار
                    // (EN) Register alias variable
                    if (!withStmt->alias.empty())
                    {
                        VariableInfo aliasVar;
                        aliasVar.name = withStmt->alias;
                        aliasVar.type = resResult.type;
                        aliasVar.registerName = resResult.registerName;
                        aliasVar.isMutable = false;
                        aliasVar.scopeLevel = currentScopeLevel_;
                        addVariable(aliasVar);
                    }

                    // (AR) استدعاء __دخول__() على المورد
                    // (EN) Call __enter__() on resource
                    {
                        SIRInstruction enterInst;
                        enterInst.opcode = SIROpcode::CALL;
                        enterInst.operands.push_back(SIROperand::Function("__sad_context_enter"));
                        enterInst.operands.push_back(SIROperand::Register(resResult.registerName, resResult.type));
                        enterInst.comment = "context manager __enter__";
                        if (currentBlock_)
                            currentBlock_->addInstruction(enterInst);
                    }

                    // ================================================================
                    // (AR) حماية الجسم بـ setjmp/longjmp لضمان استدعاء __خروج__ دائماً
                    // (EN) Protect body with setjmp/longjmp to guarantee __exit__ is always called
                    // ================================================================
                    std::string withBodyLabel = newLabel("with_body");
                    std::string withCleanupLabel = newLabel("with_cleanup");
                    std::string withRethrowLabel = newLabel("with_rethrow");
                    std::string withExitLabel = newLabel("with_exit");

                    auto withBodyBlock = createBasicBlock(withBodyLabel);
                    auto withCleanupBlock = createBasicBlock(withCleanupLabel);
                    auto withRethrowBlock = createBasicBlock(withRethrowLabel);
                    auto withExitBlock = createBasicBlock(withExitLabel);

                    // (AR) إعداد معالج الاستثناء
                    // (EN) Setup exception handler
                    std::string jmpbufReg = newTempRegister();
                    {
                        SIRInstruction allocInst;
                        allocInst.opcode = SIROpcode::CALL;
                        allocInst.result = SIROperand::Register(jmpbufReg, SadTypeKind::Pointer);
                        allocInst.operands.push_back(SIROperand::Function("__sad_alloc_jmpbuf"));
                        allocInst.comment = "allocate jmpbuf for with-statement exception safety";
                        if (currentBlock_)
                            currentBlock_->addInstruction(allocInst);
                    }
                    {
                        SIRInstruction pushInst;
                        pushInst.opcode = SIROpcode::CALL;
                        pushInst.operands.push_back(SIROperand::Function("__sad_push_handler"));
                        pushInst.operands.push_back(SIROperand::Register(jmpbufReg, SadTypeKind::Pointer));
                        pushInst.comment = "push handler for with-statement";
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
                        setjmpInst.comment = "setjmp: 0=normal, 1=exception in with body";
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
                            SIROperand::Label(withCleanupLabel),
                            SIROperand::Label(withBodyLabel));
                        if (currentBlock_)
                            currentBlock_->addInstruction(brInst);
                    }

                    // (AR) كتلة الجسم — المسار الطبيعي
                    // (EN) Body block — normal path
                    if (currentFunction_)
                        currentFunction_->addBasicBlock(withBodyBlock);
                    currentBlock_ = withBodyBlock;

                    if (withStmt->body)
                    {
                        buildStatement(withStmt->body.get());
                    }

                    // (AR) إزالة المعالج بعد نجاح الجسم
                    // (EN) Pop handler after successful body
                    {
                        SIRInstruction popInst;
                        popInst.opcode = SIROpcode::CALL;
                        popInst.operands.push_back(SIROperand::Function("__sad_pop_handler"));
                        popInst.comment = "pop handler after successful with body";
                        if (currentBlock_)
                            currentBlock_->addInstruction(popInst);
                    }

                    // (AR) استدعاء __خروج__ — المسار الطبيعي (بدون استثناء)
                    // (EN) Call __exit__ — normal path (no exception)
                    {
                        SIRInstruction exitInst;
                        exitInst.opcode = SIROpcode::CALL;
                        exitInst.operands.push_back(SIROperand::Function("__sad_context_exit"));
                        exitInst.operands.push_back(SIROperand::Register(resResult.registerName, resResult.type));
                        exitInst.operands.push_back(SIROperand::ConstantI64(0)); // no exception
                        exitInst.comment = "context manager __exit__ (normal)";
                        if (currentBlock_)
                            currentBlock_->addInstruction(exitInst);
                    }
                    if (currentBlock_)
                    {
                        currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(withExitLabel)));
                    }

                    // (AR) كتلة التنظيف — استثناء في الجسم
                    // (EN) Cleanup block — exception in body
                    if (currentFunction_)
                        currentFunction_->addBasicBlock(withCleanupBlock);
                    currentBlock_ = withCleanupBlock;

                    // (AR) إزالة المعالج
                    // (EN) Pop the handler
                    {
                        SIRInstruction popInst;
                        popInst.opcode = SIROpcode::CALL;
                        popInst.operands.push_back(SIROperand::Function("__sad_pop_handler"));
                        popInst.comment = "pop handler at with-exception cleanup";
                        if (currentBlock_)
                            currentBlock_->addInstruction(popInst);
                    }

                    // (AR) استدعاء __خروج__ مع معلومات الاستثناء
                    // (EN) Call __exit__ with exception info
                    std::string exitResultReg = newTempRegister();
                    {
                        SIRInstruction exitInst;
                        exitInst.opcode = SIROpcode::CALL;
                        exitInst.result = SIROperand::Register(exitResultReg, SadTypeKind::Integer);
                        exitInst.operands.push_back(SIROperand::Function("__sad_context_exit"));
                        exitInst.operands.push_back(SIROperand::Register(resResult.registerName, resResult.type));
                        exitInst.operands.push_back(SIROperand::ConstantI64(1)); // has exception
                        exitInst.comment = "context manager __exit__ (with exception)";
                        if (currentBlock_)
                            currentBlock_->addInstruction(exitInst);
                    }

                    // (AR) إذا أرجع __خروج__ true: تجاهل الاستثناء. وإلا: أعد رميه
                    // (EN) If __exit__ returns true: suppress exception. Otherwise: rethrow
                    std::string suppressReg = newTempRegister();
                    {
                        SIRInstruction cmpInst = SIRInstruction::Binary(
                            SIROpcode::NE,
                            SIROperand::Register(suppressReg, SadTypeKind::Boolean),
                            SIROperand::Register(exitResultReg, SadTypeKind::Integer),
                            SIROperand::ConstantI64(0));
                        if (currentBlock_)
                            currentBlock_->addInstruction(cmpInst);
                    }
                    {
                        SIRInstruction brInst = SIRInstruction::BranchCond(
                            SIROperand::Register(suppressReg, SadTypeKind::Boolean),
                            SIROperand::Label(withExitLabel),     // suppress: skip rethrow
                            SIROperand::Label(withRethrowLabel)); // rethrow
                        if (currentBlock_)
                            currentBlock_->addInstruction(brInst);
                    }

                    // (AR) كتلة إعادة الرمي — __خروج__ لم يكبت الاستثناء
                    // (EN) Rethrow block — __exit__ didn't suppress exception
                    if (currentFunction_)
                        currentFunction_->addBasicBlock(withRethrowBlock);
                    currentBlock_ = withRethrowBlock;
                    {
                        SIRInstruction rethrowInst;
                        rethrowInst.opcode = SIROpcode::CALL;
                        rethrowInst.operands.push_back(SIROperand::Function("__sad_raise_current"));
                        rethrowInst.comment = "rethrow exception not suppressed by __exit__";
                        if (currentBlock_)
                            currentBlock_->addInstruction(rethrowInst);
                    }
                    {
                        // (AR) كتلة ميتة بعد إعادة الرمي
                        // (EN) Dead block after rethrow
                        std::string deadLabel = newLabel("dead_after_with_rethrow");
                        auto deadBlock = createBasicBlock(deadLabel);
                        if (currentFunction_)
                            currentFunction_->addBasicBlock(deadBlock);
                        currentBlock_ = deadBlock;
                    }

                    // (AR) كتلة الخروج
                    // (EN) Exit block
                    if (currentFunction_)
                        currentFunction_->addBasicBlock(withExitBlock);
                    currentBlock_ = withExitBlock;

                    exitScope();
                    return true;
                }

                return false;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad