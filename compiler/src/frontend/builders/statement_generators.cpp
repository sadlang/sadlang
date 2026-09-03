// ============================================================================
// sir_builder_stmt_generators.cpp
// (AR) Generator statements: yield, with (context manager)
// (EN) Generator and context manager statements
// ============================================================================

#include <string>
#include "sir_builder.h"
#include "builders/statement_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
#include "utf8_utils.h"
#include "sad_debug_log.h"
#include "error_manager.h"
#include "error_codes.h"
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

            bool StatementBuilder::buildStatement_Generators(AST::Statement *stmt)
            {
                // (AR) YieldStmt - جملة yield (مولّد): اعطِ قيمة
                // (EN) Yield statement (generator)
                // ========================================================================
                if (auto yieldStmt = dynamic_cast<Sad::AST::YieldStmt *>(stmt))
                {
#ifndef NDEBUG
                    SAD_DEBUG_LOG_LINE("[DEBUG] Found YieldStmt");
#endif

                    // ================================================================
                    // (AR) 🔑 SEM014 — «أنتج» لا تُستخدَم إلّا داخلَ دالّةٍ مولّدة.
                    //
                    //      الرمزُ **مُعلَنٌ في `language-truth/errors/semantic.yaml`
                    //      منذُ زمنٍ ولم يُبعَثْ قطُّ من المترجّم**: كان `أنتج` خارجَ
                    //      مولّدٍ يمرُّ إلى الخلفيّة فتنهار بـ«التعليمة
                    //      (GENERATOR_YIELD) وصلت بعدد معاملات غير متوقَّع» — أي
                    //      خطأً داخليًّا يطلبُ من المستعمِلِ أن **يُبلِّغَ عن علّةِ
                    //      مترجِم**، وليست علّةَ مترجِمٍ بل خطأً في برنامجِه.
                    //      وقِيسَ أنّ المترجّمَ كان يخرجُ **بصفرٍ** بعدَها ويُسلّمُ
                    //      ثنائيًّا (وصفُ البذرة 001_toplevel_yield_compiler يوثّقه).
                    //
                    //      والمُميِّزُ `SIRFunction::isGenerator` لا استنتاجٌ من
                    //      الشكل: الدالّةُ مولّدةٌ إن أعلنَها المحلّلُ كذلك.
                    // (EN) SEM014 — yield is only valid inside a generator function.
                    //      The code has been declared in the SoT all along and was
                    //      NEVER emitted: yield outside a generator reached the
                    //      backend, which crashed with an INTERNAL error asking the
                    //      user to report a compiler bug — for a defect in their own
                    //      program — and then exited 0 with a binary.
                    // ================================================================
                    if (!b_.currentFunction_ || !b_.currentFunction_->isGenerator)
                    {
                        b_.errors_.push_back(
                            Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                                Sad::Errors::ErrorCode::SEM_YIELD_OUTSIDE_GENERATOR,
                                Sad::Errors::RenderContext{}));
                        return true;
                    }

                    BuildResult valResult;
                    if (yieldStmt->value)
                    {
                        valResult = b_.buildExpression(yieldStmt->value.get());
                    }

                    // ================================================================
                    // (AR) إذا كان yield from — تكرار على iterable وإنتاج كل عنصر
                    // (EN) If yield from — iterate over iterable and yield each element
                    // ================================================================
                    if (yieldStmt->isYieldFrom)
                    {
                        // (AR) الحصول على طول المصفوفة/المتكرر
                        // (EN) Get length of array/iterable
                        std::string lenReg = b_.newTempRegister();
                        {
                            SIRInstruction lenInst;
                            lenInst.opcode = SIROpcode::ARRAY_LEN;
                            lenInst.result = SIROperand::Register(lenReg, SadTypeKind::Integer);
                            lenInst.operands.push_back(SIROperand::Register(valResult.registerName, valResult.type));
                            lenInst.comment = "yield from: get iterable length";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(lenInst);
                        }

                        // (AR) عداد الحلقة
                        // (EN) Loop counter
                        std::string idxReg = b_.newTempRegister();
                        {
                            SIRInstruction initInst;
                            initInst.opcode = SIROpcode::MOVE;
                            initInst.result = SIROperand::Register(idxReg, SadTypeKind::Integer);
                            initInst.operands.push_back(SIROperand::ConstantI64(0));
                            initInst.comment = "yield from: init counter";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(initInst);
                        }

                        // (AR) إنشاء كتل الحلقة
                        // (EN) Create loop blocks
                        std::string yieldLoopCondLabel = b_.newLabel("yield_from_cond");
                        std::string yieldLoopBodyLabel = b_.newLabel("yield_from_body");
                        std::string yieldLoopExitLabel = b_.newLabel("yield_from_exit");

                        auto yieldCondBlock = b_.createBasicBlock(yieldLoopCondLabel);
                        auto yieldBodyBlock = b_.createBasicBlock(yieldLoopBodyLabel);
                        auto yieldExitBlock = b_.createBasicBlock(yieldLoopExitLabel);

                        if (b_.currentBlock_)
                        {
                            b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(yieldLoopCondLabel)));
                        }

                        // (AR) كتلة الشرط
                        // (EN) Condition block
                        if (b_.currentFunction_)
                            b_.currentFunction_->addBasicBlock(yieldCondBlock);
                        b_.currentBlock_ = yieldCondBlock;

                        std::string cmpReg = b_.newTempRegister();
                        {
                            SIRInstruction cmpInst = SIRInstruction::Binary(
                                SIROpcode::LT,
                                SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                                SIROperand::Register(idxReg, SadTypeKind::Integer),
                                SIROperand::Register(lenReg, SadTypeKind::Integer));
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(cmpInst);
                        }
                        {
                            SIRInstruction brInst = SIRInstruction::BranchCond(
                                SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                                SIROperand::Label(yieldLoopBodyLabel),
                                SIROperand::Label(yieldLoopExitLabel));
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(brInst);
                        }

                        // (AR) كتلة الجسم — استخراج العنصر وإنتاجه
                        // (EN) Body block — extract element and yield it
                        if (b_.currentFunction_)
                            b_.currentFunction_->addBasicBlock(yieldBodyBlock);
                        b_.currentBlock_ = yieldBodyBlock;

                        std::string elemReg = b_.newTempRegister();
                        {
                            SIRInstruction getInst;
                            getInst.opcode = SIROpcode::ARRAY_GET;
                            getInst.result = SIROperand::Register(elemReg, SadTypeKind::Pointer);
                            getInst.operands.push_back(SIROperand::Register(valResult.registerName, valResult.type));
                            getInst.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                            getInst.comment = "yield from: get element at index";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(getInst);
                        }

                        // (AR) إنتاج العنصر
                        // (EN) Yield the element
                        {
                            SIRInstruction yieldInst;
                            yieldInst.opcode = SIROpcode::GENERATOR_YIELD;
                            yieldInst.operands.push_back(SIROperand::Register(elemReg, SadTypeKind::Pointer));
                            yieldInst.comment = "yield from: yield delegated element";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(yieldInst);
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
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(incInst);
                        }

                        if (b_.currentBlock_)
                        {
                            b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(yieldLoopCondLabel)));
                        }

                        // (AR) كتلة الخروج من الحلقة
                        // (EN) Loop exit block
                        if (b_.currentFunction_)
                            b_.currentFunction_->addBasicBlock(yieldExitBlock);
                        b_.currentBlock_ = yieldExitBlock;

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

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(yieldInst);
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
                    SAD_DEBUG_LOG_LINE("[DEBUG] Found WithStmt");
#endif

                    b_.enterScope();

                    // (AR) بناء تعبير المورد
                    // (EN) Build resource expression
                    auto resResult = b_.buildExpression(withStmt->resource.get());

                    // (AR) تسجيل المتغير المستعار
                    // (EN) Register alias variable
                    if (!withStmt->alias.empty())
                    {
                        VariableInfo aliasVar;
                        aliasVar.name = withStmt->alias;
                        aliasVar.type = resResult.type;
                        aliasVar.registerName = resResult.registerName;
                        aliasVar.isMutable = false;
                        aliasVar.scopeLevel = b_.currentScopeLevel_;
                        b_.addVariable(aliasVar);
                    }

                    // (AR) استدعاء __دخول__() على المورد
                    // (EN) Call __enter__() on resource
                    {
                        SIRInstruction enterInst;
                        enterInst.opcode = SIROpcode::CALL;
                        enterInst.operands.push_back(SIROperand::Function("__sad_context_enter"));
                        enterInst.operands.push_back(SIROperand::Register(resResult.registerName, resResult.type));
                        enterInst.comment = "context manager __enter__";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(enterInst);
                    }

                    // ================================================================
                    // (AR) حماية الجسم بـ setjmp/longjmp لضمان استدعاء __خروج__ دائماً
                    // (EN) Protect body with setjmp/longjmp to guarantee __exit__ is always called
                    // ================================================================
                    std::string withBodyLabel = b_.newLabel("with_body");
                    std::string withCleanupLabel = b_.newLabel("with_cleanup");
                    std::string withRethrowLabel = b_.newLabel("with_rethrow");
                    std::string withExitLabel = b_.newLabel("with_exit");

                    auto withBodyBlock = b_.createBasicBlock(withBodyLabel);
                    auto withCleanupBlock = b_.createBasicBlock(withCleanupLabel);
                    auto withRethrowBlock = b_.createBasicBlock(withRethrowLabel);
                    auto withExitBlock = b_.createBasicBlock(withExitLabel);

                    // (AR) إعداد معالج الاستثناء
                    // (EN) Setup exception handler
                    std::string jmpbufReg = b_.newTempRegister();
                    {
                        SIRInstruction allocInst;
                        allocInst.opcode = SIROpcode::CALL;
                        allocInst.result = SIROperand::Register(jmpbufReg, SadTypeKind::Pointer);
                        allocInst.operands.push_back(SIROperand::Function("__sad_alloc_jmpbuf"));
                        allocInst.comment = "allocate jmpbuf for with-statement exception safety";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(allocInst);
                    }
                    {
                        SIRInstruction pushInst;
                        pushInst.opcode = SIROpcode::CALL;
                        pushInst.operands.push_back(SIROperand::Function("__sad_push_handler"));
                        pushInst.operands.push_back(SIROperand::Register(jmpbufReg, SadTypeKind::Pointer));
                        pushInst.comment = "push handler for with-statement";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(pushInst);
                    }
                    std::string setjmpResultReg = b_.newTempRegister();
                    {
                        SIRInstruction setjmpInst;
                        setjmpInst.opcode = SIROpcode::CALL;
                        setjmpInst.result = SIROperand::Register(setjmpResultReg, SadTypeKind::Integer);
                        setjmpInst.operands.push_back(SIROperand::Function("__sad_setjmp"));
                        setjmpInst.operands.push_back(SIROperand::Register(jmpbufReg, SadTypeKind::Pointer));
                        setjmpInst.comment = "setjmp: 0=normal, 1=exception in with body";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(setjmpInst);
                    }
                    std::string cmpReg = b_.newTempRegister();
                    {
                        SIRInstruction cmpInst = SIRInstruction::Binary(
                            SIROpcode::NE,
                            SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                            SIROperand::Register(setjmpResultReg, SadTypeKind::Integer),
                            SIROperand::ConstantI64(0));
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(cmpInst);
                    }
                    {
                        SIRInstruction brInst = SIRInstruction::BranchCond(
                            SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                            SIROperand::Label(withCleanupLabel),
                            SIROperand::Label(withBodyLabel));
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(brInst);
                    }

                    // (AR) كتلة الجسم — المسار الطبيعي
                    // (EN) Body block — normal path
                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(withBodyBlock);
                    b_.currentBlock_ = withBodyBlock;

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
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(popInst);
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
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(exitInst);
                    }
                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(withExitLabel)));
                    }

                    // (AR) كتلة التنظيف — استثناء في الجسم
                    // (EN) Cleanup block — exception in body
                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(withCleanupBlock);
                    b_.currentBlock_ = withCleanupBlock;

                    // (AR) إزالة المعالج
                    // (EN) Pop the handler
                    {
                        SIRInstruction popInst;
                        popInst.opcode = SIROpcode::CALL;
                        popInst.operands.push_back(SIROperand::Function("__sad_pop_handler"));
                        popInst.comment = "pop handler at with-exception cleanup";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(popInst);
                    }

                    // (AR) استدعاء __خروج__ مع معلومات الاستثناء
                    // (EN) Call __exit__ with exception info
                    std::string exitResultReg = b_.newTempRegister();
                    {
                        SIRInstruction exitInst;
                        exitInst.opcode = SIROpcode::CALL;
                        exitInst.result = SIROperand::Register(exitResultReg, SadTypeKind::Integer);
                        exitInst.operands.push_back(SIROperand::Function("__sad_context_exit"));
                        exitInst.operands.push_back(SIROperand::Register(resResult.registerName, resResult.type));
                        exitInst.operands.push_back(SIROperand::ConstantI64(1)); // has exception
                        exitInst.comment = "context manager __exit__ (with exception)";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(exitInst);
                    }

                    // (AR) إذا أرجع __خروج__ true: تجاهل الاستثناء. وإلا: أعد رميه
                    // (EN) If __exit__ returns true: suppress exception. Otherwise: rethrow
                    std::string suppressReg = b_.newTempRegister();
                    {
                        SIRInstruction cmpInst = SIRInstruction::Binary(
                            SIROpcode::NE,
                            SIROperand::Register(suppressReg, SadTypeKind::Boolean),
                            SIROperand::Register(exitResultReg, SadTypeKind::Integer),
                            SIROperand::ConstantI64(0));
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(cmpInst);
                    }
                    {
                        SIRInstruction brInst = SIRInstruction::BranchCond(
                            SIROperand::Register(suppressReg, SadTypeKind::Boolean),
                            SIROperand::Label(withExitLabel),     // suppress: skip rethrow
                            SIROperand::Label(withRethrowLabel)); // rethrow
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(brInst);
                    }

                    // (AR) كتلة إعادة الرمي — __خروج__ لم يكبت الاستثناء
                    // (EN) Rethrow block — __exit__ didn't suppress exception
                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(withRethrowBlock);
                    b_.currentBlock_ = withRethrowBlock;
                    {
                        SIRInstruction rethrowInst;
                        rethrowInst.opcode = SIROpcode::CALL;
                        rethrowInst.operands.push_back(SIROperand::Function("__sad_raise_current"));
                        rethrowInst.comment = "rethrow exception not suppressed by __exit__";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(rethrowInst);
                    }
                    {
                        // (AR) كتلة ميتة بعد إعادة الرمي
                        // (EN) Dead block after rethrow
                        std::string deadLabel = b_.newLabel("dead_after_with_rethrow");
                        auto deadBlock = b_.createBasicBlock(deadLabel);
                        if (b_.currentFunction_)
                            b_.currentFunction_->addBasicBlock(deadBlock);
                        b_.currentBlock_ = deadBlock;
                    }

                    // (AR) كتلة الخروج
                    // (EN) Exit block
                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(withExitBlock);
                    b_.currentBlock_ = withExitBlock;

                    b_.exitScope();
                    return true;
                }

                return false;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad