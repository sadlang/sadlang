// ============================================================================
// sir_builder_stmt_exceptions.cpp
// (AR) Exception handling: try/catch/finally, raise
// (EN) Exception handling statements
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

            bool StatementBuilder::buildStatement_Exceptions(AST::Statement *stmt)
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

                    // (AR) ادفع خانة استنتاج نوع المرميّ لهذا «حاول». كلُّ «ارمي» لفظيّة
                    //      داخل الجسم (لا في «حاول» متداخل) ستسجّل نوعها فيها.
                    // (EN) Push a thrown-type inference slot for this «try». Each lexical
                    //      «throw» in the body (not in a nested «try») records its type here.
                    b_.tryThrownStack_.push_back({});
                    SIRBuilderContext::ThrownInfo inferredThrow;

                    // (AR) إنشاء كتل: try، catch dispatch، finally، exit
                    // (EN) Create blocks: try, catch dispatch, finally, exit
                    std::string tryLabel = b_.newLabel("try_body");
                    std::string catchSetupLabel = b_.newLabel("catch_setup");
                    std::string finallyLabel = b_.newLabel("finally_body");
                    std::string exitLabel = b_.newLabel("try_exit");

                    auto tryBlock = b_.createBasicBlock(tryLabel);
                    auto catchSetupBlock = b_.createBasicBlock(catchSetupLabel);
                    auto finallyBlock = b_.createBasicBlock(finallyLabel);
                    auto exitBlock = b_.createBasicBlock(exitLabel);

                    // (AR) كتل إضافية لـ finally الموثوق
                    // (EN) Extra blocks for reliable finally
                    std::string finallyRethrowLabel, catchSecondaryLabel;
                    std::shared_ptr<SIRBasicBlock> finallyRethrowBlock = nullptr;
                    std::shared_ptr<SIRBasicBlock> catchSecondaryBlock = nullptr;
                    if (hasFinally)
                    {
                        finallyRethrowLabel = b_.newLabel("finally_rethrow");
                        catchSecondaryLabel = b_.newLabel("catch_secondary");
                        finallyRethrowBlock = b_.createBasicBlock(finallyRethrowLabel);
                        catchSecondaryBlock = b_.createBasicBlock(catchSecondaryLabel);
                    }

                    // (AR) إنشاء كتل لكل catch clause — type check و body
                    // (EN) Create blocks for each catch clause — type check and body
                    std::vector<std::string> catchCheckLabels;
                    std::vector<std::string> catchBodyLabels;
                    std::vector<std::shared_ptr<SIRBasicBlock>> catchCheckBlocks;
                    std::vector<std::shared_ptr<SIRBasicBlock>> catchBodyBlocks;
                    // (AR) كتلة "لا تطابق" — إعادة رمي إذا لم يطابق أي catch
                    // (EN) "no match" block — rethrow if no clause matched
                    std::string noMatchLabel = b_.newLabel("catch_no_match");
                    auto noMatchBlock = b_.createBasicBlock(noMatchLabel);

                    for (size_t i = 0; i < numClauses; ++i)
                    {
                        std::string checkLabel = b_.newLabel("catch_check_" + std::to_string(i));
                        std::string bodyLabel = b_.newLabel("catch_body_" + std::to_string(i));
                        catchCheckLabels.push_back(checkLabel);
                        catchBodyLabels.push_back(bodyLabel);
                        catchCheckBlocks.push_back(b_.createBasicBlock(checkLabel));
                        catchBodyBlocks.push_back(b_.createBasicBlock(bodyLabel));
                    }

                    // ================================================================
                    // (AR) الإعداد: jmpbuf + setjmp
                    // (EN) Setup: jmpbuf + setjmp
                    // ================================================================
                    std::string jmpbufReg = b_.newTempRegister();
                    {
                        SIRInstruction allocInst;
                        allocInst.opcode = SIROpcode::CALL;
                        allocInst.result = SIROperand::Register(jmpbufReg, SadTypeKind::Pointer);
                        allocInst.operands.push_back(SIROperand::Function("__sad_alloc_jmpbuf"));
                        allocInst.comment = "allocate jmpbuf for try/catch";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(allocInst);
                    }
                    {
                        SIRInstruction pushInst;
                        pushInst.opcode = SIROpcode::CALL;
                        pushInst.operands.push_back(SIROperand::Function("__sad_push_handler"));
                        pushInst.operands.push_back(SIROperand::Register(jmpbufReg, SadTypeKind::Pointer));
                        pushInst.comment = "push exception handler";
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
                        setjmpInst.comment = "setjmp: 0=normal, 1=exception";
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
                            SIROperand::Label(catchSetupLabel),
                            SIROperand::Label(tryLabel));
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(brInst);
                    }

                    // ================================================================
                    // (AR) كتلة try
                    // (EN) Try block
                    // ================================================================
                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(tryBlock);
                    b_.currentBlock_ = tryBlock;

                    // ================================================================
                    // (AR) إذا hasFinally: إنشاء سجلات لحفظ قيمة الإرجاع ودفع SIRBuilderContext::FinallyContext
                    //      هذا يضمن تشغيل finally حتى عند وجود ارجع داخل try أو catch
                    //      يُستخدم في buildReturnStatement للتحويل إلى branch بدل RETURN مباشرة
                    // (EN) If hasFinally: create return-value registers and push SIRBuilderContext::FinallyContext
                    //      Guarantees finally runs even when return is inside try or catch block
                    //      Used in buildReturnStatement to branch instead of direct RETURN
                    // ================================================================
                    std::string finallyRetI64Reg, finallyRetPtrReg, finallyRetTypeReg, finallyHasRetReg;
                    if (hasFinally)
                    {
                        // (AR) تسميات فريدة لهذه السجلات لتجنب التعارض في الدوال الكبيرة
                        // (EN) Unique names to avoid conflicts in large functions
                        std::string uniq = std::to_string(b_.nextLabel_);
                        finallyRetI64Reg = std::string("%") + kSlotNamespaceSeparator + "fri64" + kSlotNamespaceSeparator + uniq;
                        finallyRetPtrReg = std::string("%") + kSlotNamespaceSeparator + "frptr" + kSlotNamespaceSeparator + uniq;
                        finallyRetTypeReg = std::string("%") + kSlotNamespaceSeparator + "frtype" + kSlotNamespaceSeparator + uniq;
                        finallyHasRetReg = std::string("%") + kSlotNamespaceSeparator + "frhas" + kSlotNamespaceSeparator + uniq;

                        // ALLOC i64 لقيم الأرقام
                        {
                            SIRInstruction alI;
                            alI.opcode = SIROpcode::ALLOC;
                            alI.result = SIROperand::Register(finallyRetI64Reg, SadTypeKind::Integer);
                            alI.comment = "alloca i64 for finally return value (integer)";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(alI);
                        }
                        // ALLOC ptr لقيم النصوص
                        {
                            SIRInstruction alP;
                            alP.opcode = SIROpcode::ALLOC;
                            alP.result = SIROperand::Register(finallyRetPtrReg, SadTypeKind::String);
                            alP.comment = "alloca ptr for finally return value (string/object)";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(alP);
                        }
                        // ALLOC i64 لنوع القيمة
                        {
                            SIRInstruction alT;
                            alT.opcode = SIROpcode::ALLOC;
                            alT.result = SIROperand::Register(finallyRetTypeReg, SadTypeKind::Integer);
                            alT.comment = "alloca i64 for finally return type code";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(alT);
                        }
                        // ALLOC i64 لعلامة "تم ارجع"
                        {
                            SIRInstruction alH;
                            alH.opcode = SIROpcode::ALLOC;
                            alH.result = SIROperand::Register(finallyHasRetReg, SadTypeKind::Integer);
                            alH.comment = "alloca i64 for finally has_return flag";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(alH);
                        }
                        // تهيئة has_return = 0 و type = 0 (void)
                        {
                            SIRInstruction stH;
                            stH.opcode = SIROpcode::STORE;
                            stH.operands.push_back(SIROperand::ConstantI64(0));
                            stH.operands.push_back(SIROperand::Register(finallyHasRetReg, SadTypeKind::Integer));
                            stH.comment = "init finally has_return = 0";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(stH);
                        }
                        {
                            SIRInstruction stT;
                            stT.opcode = SIROpcode::STORE;
                            stT.operands.push_back(SIROperand::ConstantI64(0));
                            stT.operands.push_back(SIROperand::Register(finallyRetTypeReg, SadTypeKind::Integer));
                            stT.comment = "init finally return type = 0 (void)";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(stT);
                        }
                        // دفع SIRBuilderContext::FinallyContext إلى المكدس
                        b_.finallyStack_.push_back({finallyLabel, exitLabel,
                                                 finallyRetI64Reg, finallyRetPtrReg,
                                                 finallyRetTypeReg, finallyHasRetReg});
                    }

                    // (AR) الحاجز ٧: علّم دخول «حاول» فعليّة (عدّاد مستقلّ عن معالِج
                    //      تنظيف الدالّة) ليعرف حاجزُ الهلع الجوهريّ أنّ الالتقاط ممكن.
                    // (EN) Barrier 7: mark entry into a real «try» (a counter independent of
                    //      the function-cleanup handler) so the intrinsic-panic guard knows a
                    //      catch is possible.
                    {
                        SIRInstruction tryEnter;
                        tryEnter.opcode = SIROpcode::CALL;
                        tryEnter.operands.push_back(SIROperand::Function("__sad_try_enter"));
                        tryEnter.comment = "barrier7: enter active try";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(tryEnter);
                    }

                    // (AR) عمق «حاول» اللفظيّ يشمل جسمَ هذا «حاول» فتعرف جُمَلُ الخروج
                    //      غير المحلّيّ (ارجع/قف/أكمل) داخله كم مرّة تبعث __sad_try_exit.
                    // (EN) The lexical «try» depth now includes this «try»'s body, so non-local
                    //      exits (return/break/continue) inside it know how many __sad_try_exit
                    //      to emit.
                    b_.currentTryDepth_++;

                    b_.enterScope();
                    if (tryStmt->tryBlock)
                    {
                        buildStatement(tryStmt->tryBlock.get());
                    }
                    b_.exitScope();

                    // (AR) انتهى جسمُ «حاول» لفظيًّا ⇒ اخرج من عمقه (كتلُ «امسك» تلي بعمقٍ أدنى).
                    // (EN) The «try» body is lexically done ⇒ leave its depth (catch blocks that
                    //      follow are at the lower depth).
                    b_.currentTryDepth_--;

                    // (AR) خروج «حاول» على المسار الناجح (لا استثناء)
                    // (EN) Leave the «try» on the successful (no-exception) path
                    {
                        SIRInstruction tryExit;
                        tryExit.opcode = SIROpcode::CALL;
                        tryExit.operands.push_back(SIROperand::Function("__sad_try_exit"));
                        tryExit.comment = "barrier7: exit active try (success)";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(tryExit);
                    }

                    // (AR) التقط استنتاج نوع المرميّ ثمّ انبثق الخانة (الجسم اكتمل بناؤه؛
                    //      «حاول» المتداخلة أدارت خاناتها). يُستعمَل في ربط متغيّر «امسك».
                    // (EN) Capture the thrown-type inference then pop the slot (the body is
                    //      fully built; nested «try»s managed their own slots). Used when
                    //      binding the «catch» variable below.
                    if (!b_.tryThrownStack_.empty())
                    {
                        inferredThrow = b_.tryThrownStack_.back();
                        b_.tryThrownStack_.pop_back();
                    }

                    {
                        SIRInstruction popInst;
                        popInst.opcode = SIROpcode::CALL;
                        popInst.operands.push_back(SIROperand::Function("__sad_pop_handler"));
                        popInst.comment = "pop handler after successful try";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(popInst);
                    }
                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(SIRInstruction::Branch(
                            SIROperand::Label(hasFinally ? finallyLabel : exitLabel)));
                    }

                    // ================================================================
                    // (AR) كتلة catch_setup — إزالة المعالج الأول + إعداد معالج ثانوي لـ finally
                    // (EN) catch_setup — pop primary handler + setup secondary for finally
                    // ================================================================
                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(catchSetupBlock);
                    b_.currentBlock_ = catchSetupBlock;

                    // (AR) خروج «حاول» على مسار الاستثناء (وصلنا هنا عبر setjmp!=0)
                    // (EN) Leave the «try» on the exception path (reached via setjmp!=0)
                    {
                        SIRInstruction tryExit;
                        tryExit.opcode = SIROpcode::CALL;
                        tryExit.operands.push_back(SIROperand::Function("__sad_try_exit"));
                        tryExit.comment = "barrier7: exit active try (exception)";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(tryExit);
                    }

                    {
                        SIRInstruction popInst;
                        popInst.opcode = SIROpcode::CALL;
                        popInst.operands.push_back(SIROperand::Function("__sad_pop_handler"));
                        popInst.comment = "pop primary handler at catch entry";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(popInst);
                    }

                    if (hasFinally)
                    {
                        // (AR) دفع معالج ثانوي يحمي catch — لضمان تنفيذ finally
                        // (EN) Push secondary handler protecting catch — guarantees finally runs
                        std::string jmpbuf2Reg = b_.newTempRegister();
                        {
                            SIRInstruction allocInst;
                            allocInst.opcode = SIROpcode::CALL;
                            allocInst.result = SIROperand::Register(jmpbuf2Reg, SadTypeKind::Pointer);
                            allocInst.operands.push_back(SIROperand::Function("__sad_alloc_jmpbuf"));
                            allocInst.comment = "allocate secondary jmpbuf for finally protection";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(allocInst);
                        }
                        {
                            SIRInstruction pushInst;
                            pushInst.opcode = SIROpcode::CALL;
                            pushInst.operands.push_back(SIROperand::Function("__sad_push_handler"));
                            pushInst.operands.push_back(SIROperand::Register(jmpbuf2Reg, SadTypeKind::Pointer));
                            pushInst.comment = "push secondary handler for finally";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(pushInst);
                        }
                        std::string setjmp2Reg = b_.newTempRegister();
                        {
                            SIRInstruction setjmpInst;
                            setjmpInst.opcode = SIROpcode::CALL;
                            setjmpInst.result = SIROperand::Register(setjmp2Reg, SadTypeKind::Integer);
                            setjmpInst.operands.push_back(SIROperand::Function("__sad_setjmp"));
                            setjmpInst.operands.push_back(SIROperand::Register(jmpbuf2Reg, SadTypeKind::Pointer));
                            setjmpInst.comment = "secondary setjmp: exception inside catch → finally_rethrow";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(setjmpInst);
                        }
                        std::string cmp2Reg = b_.newTempRegister();
                        {
                            SIRInstruction cmpInst = SIRInstruction::Binary(
                                SIROpcode::NE,
                                SIROperand::Register(cmp2Reg, SadTypeKind::Boolean),
                                SIROperand::Register(setjmp2Reg, SadTypeKind::Integer),
                                SIROperand::ConstantI64(0));
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(cmpInst);
                        }
                        {
                            // (AR) إذا استثناء داخل catch → finally_rethrow
                            // (EN) If exception inside catch → finally_rethrow
                            SIRInstruction brInst = SIRInstruction::BranchCond(
                                SIROperand::Register(cmp2Reg, SadTypeKind::Boolean),
                                SIROperand::Label(finallyRethrowLabel),
                                SIROperand::Label(numClauses > 0 ? catchCheckLabels[0] : noMatchLabel));
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(brInst);
                        }
                    }
                    else
                    {
                        // (AR) بدون finally — اذهب مباشرة لفحص الأنواع
                        // (EN) No finally — go directly to type checks
                        if (b_.currentBlock_)
                        {
                            b_.currentBlock_->addInstruction(SIRInstruction::Branch(
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

                        if (b_.currentFunction_)
                            b_.currentFunction_->addBasicBlock(catchCheckBlocks[i]);
                        b_.currentBlock_ = catchCheckBlocks[i];

                        if (clause.exceptionType == Sad::Types::SadTypeKind::Unknown)
                        {
                            // (AR) catch-all: يطابق أي استثناء — اذهب مباشرة للجسم
                            // (EN) Catch-all: matches any exception — go directly to body
                            if (b_.currentBlock_)
                            {
                                b_.currentBlock_->addInstruction(SIRInstruction::Branch(
                                    SIROperand::Label(catchBodyLabels[i])));
                            }
                        }
                        else if (clause.exceptionType == Sad::Types::SadTypeKind::Error)
                        {
                            // (AR) مطابقة نوع ERROR — قارن مع "خطأ"
                            // (EN) Match ERROR type — compare with "خطأ"
                            std::string excTypeReg = b_.newTempRegister();
                            {
                                SIRInstruction getTypeInst;
                                getTypeInst.opcode = SIROpcode::CALL;
                                getTypeInst.result = SIROperand::Register(excTypeReg, SadTypeKind::String);
                                getTypeInst.operands.push_back(SIROperand::Function("__sad_get_exception_type"));
                                getTypeInst.comment = "get exception type for matching";
                                if (b_.currentBlock_)
                                    b_.currentBlock_->addInstruction(getTypeInst);
                            }
                            std::string matchReg = b_.newTempRegister();
                            {
                                SIRInstruction cmpInst;
                                cmpInst.opcode = SIROpcode::CALL;
                                cmpInst.result = SIROperand::Register(matchReg, SadTypeKind::Integer);
                                cmpInst.operands.push_back(SIROperand::Function("__sad_str_equals"));
                                cmpInst.operands.push_back(SIROperand::Register(excTypeReg, SadTypeKind::String));
                                cmpInst.operands.push_back(SIROperand::ConstantString("\xd8\xae\xd8\xb7\xd8\xa3")); // "خطأ"
                                cmpInst.comment = "compare exception type with ERROR";
                                if (b_.currentBlock_)
                                    b_.currentBlock_->addInstruction(cmpInst);
                            }
                            std::string nextTarget = (i + 1 < numClauses) ? catchCheckLabels[i + 1] : noMatchLabel;
                            {
                                SIRInstruction brInst = SIRInstruction::BranchCond(
                                    SIROperand::Register(matchReg, SadTypeKind::Boolean),
                                    SIROperand::Label(catchBodyLabels[i]),
                                    SIROperand::Label(nextTarget));
                                if (b_.currentBlock_)
                                    b_.currentBlock_->addInstruction(brInst);
                            }
                        }
                        else if (clause.exceptionType == Sad::Types::SadTypeKind::Class)
                        {
                            // (AR) مطابقة نوع كائن مخصص — قارن مع اسم الصنف
                            // (EN) Match custom object type — compare with class name
                            std::string typeName = clause.exceptionTypeName;
                            if (typeName.empty())
                                typeName = clause.exceptionVar;

                            std::string excTypeReg = b_.newTempRegister();
                            {
                                SIRInstruction getTypeInst;
                                getTypeInst.opcode = SIROpcode::CALL;
                                getTypeInst.result = SIROperand::Register(excTypeReg, SadTypeKind::String);
                                getTypeInst.operands.push_back(SIROperand::Function("__sad_get_exception_type"));
                                getTypeInst.comment = "get exception type for object matching";
                                if (b_.currentBlock_)
                                    b_.currentBlock_->addInstruction(getTypeInst);
                            }
                            std::string matchReg = b_.newTempRegister();
                            {
                                SIRInstruction cmpInst;
                                cmpInst.opcode = SIROpcode::CALL;
                                cmpInst.result = SIROperand::Register(matchReg, SadTypeKind::Integer);
                                cmpInst.operands.push_back(SIROperand::Function("__sad_str_equals"));
                                cmpInst.operands.push_back(SIROperand::Register(excTypeReg, SadTypeKind::String));
                                cmpInst.operands.push_back(SIROperand::ConstantString(typeName));
                                cmpInst.comment = "compare exception type with " + typeName;
                                if (b_.currentBlock_)
                                    b_.currentBlock_->addInstruction(cmpInst);
                            }
                            std::string nextTarget = (i + 1 < numClauses) ? catchCheckLabels[i + 1] : noMatchLabel;
                            {
                                SIRInstruction brInst = SIRInstruction::BranchCond(
                                    SIROperand::Register(matchReg, SadTypeKind::Boolean),
                                    SIROperand::Label(catchBodyLabels[i]),
                                    SIROperand::Label(nextTarget));
                                if (b_.currentBlock_)
                                    b_.currentBlock_->addInstruction(brInst);
                            }
                        }
                        else
                        {
                            // (AR) نوع غير معروف — يعامل كـ catch-all
                            // (EN) Unknown type — treated as catch-all
                            if (b_.currentBlock_)
                            {
                                b_.currentBlock_->addInstruction(SIRInstruction::Branch(
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

                        if (b_.currentFunction_)
                            b_.currentFunction_->addBasicBlock(catchBodyBlocks[i]);
                        b_.currentBlock_ = catchBodyBlocks[i];

                        b_.enterScope();

                        if (!clause.exceptionVar.empty())
                        {
                            // ────────────────────────────────────────────────
                            // (AR) ربط متغيّر «امسك» بنوعه الساكن الصحيح فيطابق نوع()
                            //      المفسّرَ ويعمل وصول الحقل. الأولويّة: نوع البند
                            //      المصرَّح (`امسك صنف خ`) ثمّ استنتاج جسم «حاول».
                            //      • كائن ⇒ ربط مباشر لمؤشّر الكائن (msg العامّ) + اسم الصنف.
                            //      • رقم/منطقيّ ⇒ __sad_get_exception_value (i64 من value العامّ).
                            //      • عشريّ ⇒ __sad_get_exception_valuef (double).
                            //      • نصّ/تراجُع ⇒ __sad_get_exception (السلوك السابق).
                            // (EN) Bind the «catch» variable with its correct static type so
                            //      نوع() matches the interpreter and field access works.
                            //      Priority: the clause's declared type, then try-body
                            //      inference. object ⇒ direct object-pointer bind (from the
                            //      msg global) + class name; int/bool ⇒ i64 value global;
                            //      float ⇒ double; string/fallback ⇒ prior behaviour.
                            SadTypeKind bindKind = SadTypeKind::String;
                            std::string bindClassName;
                            std::string getFn = "__sad_get_exception";
                            bool isObject = false;

                            // (AR) دالّة صغيرة: صنِّف النوع المستنتَج/المصرَّح إلى ربط
                            // (EN) small helper: classify a declared/inferred kind into a binding
                            auto classify = [&](SadTypeKind k, const std::string &cls) -> bool
                            {
                                // (AR) الكائن يُرجعه «جديد» بنوع Struct (⇒ «كائن»)؛ نطابقه بالضبط
                                // (EN) an object is produced by «new» as Struct (⇒ «كائن»); mirror it
                                if (k == SadTypeKind::Struct || k == SadTypeKind::Class)
                                {
                                    bindKind = SadTypeKind::Struct;
                                    bindClassName = cls;
                                    isObject = true;
                                    getFn = "__sad_get_exception";
                                    return true;
                                }
                                if (k == SadTypeKind::Integer || k == SadTypeKind::Boolean)
                                {
                                    bindKind = k;
                                    getFn = "__sad_get_exception_value";
                                    return true;
                                }
                                if (k == SadTypeKind::Float)
                                {
                                    bindKind = SadTypeKind::Float;
                                    getFn = "__sad_get_exception_valuef";
                                    return true;
                                }
                                if (k == SadTypeKind::String)
                                {
                                    bindKind = SadTypeKind::String;
                                    getFn = "__sad_get_exception";
                                    return true;
                                }
                                return false;
                            };

                            // (AR) الأولويّة: نوع البند المصرَّح، ثمّ استنتاج جسم «حاول»
                            // (EN) Priority: the clause's declared type, then try-body inference
                            if ((clause.exceptionType == Sad::Types::SadTypeKind::Class ||
                                 clause.exceptionType == Sad::Types::SadTypeKind::Struct) &&
                                !clause.exceptionTypeName.empty())
                            {
                                classify(SadTypeKind::Struct, clause.exceptionTypeName);
                            }
                            else if (!classify(clause.exceptionType, clause.exceptionTypeName))
                            {
                                if (inferredThrow.sawThrow && !inferredThrow.mixed)
                                    classify(inferredThrow.kind, inferredThrow.className);
                                // (AR) وإلّا: يبقى التراجُع الآمن (نصّ) — لا انهيار
                                // (EN) else: safe fallback (string) — no crash
                            }

                            std::string exReg = b_.newTempRegister();
                            {
                                SIRInstruction loadExInst;
                                loadExInst.opcode = SIROpcode::CALL;
                                loadExInst.result = SIROperand::Register(exReg, bindKind);
                                loadExInst.operands.push_back(SIROperand::Function(getFn));
                                loadExInst.comment = "load caught exception into " + clause.exceptionVar;
                                if (b_.currentBlock_)
                                    b_.currentBlock_->addInstruction(loadExInst);
                            }

                            // (AR) نمط المتغيّر المحلّيّ الموحَّد: ALLOC «%الاسم» + STORE القيمة
                            //      (مطابق للكائنات العاديّة التي تُخزَّن كذلك). الكائن يُسجَّل صنفه.
                            // (EN) Uniform local-variable pattern: ALLOC «%name» + STORE the value
                            //      (identical to normal objects, which are stored the same way).
                            //      An object additionally registers its class name.
                            std::string exAllocReg = "%" + clause.exceptionVar;
                            {
                                SIRInstruction allocInst;
                                allocInst.opcode = SIROpcode::ALLOC;
                                allocInst.result = SIROperand::Register(exAllocReg, bindKind);
                                if (b_.currentBlock_)
                                    b_.currentBlock_->addInstruction(allocInst);
                            }
                            {
                                SIRInstruction storeInst;
                                storeInst.opcode = SIROpcode::STORE;
                                storeInst.operands.push_back(SIROperand::Register(exReg, bindKind));
                                storeInst.operands.push_back(SIROperand::Register(exAllocReg, bindKind));
                                if (b_.currentBlock_)
                                    b_.currentBlock_->addInstruction(storeInst);
                            }

                            VariableInfo exVar;
                            exVar.name = clause.exceptionVar;
                            exVar.type = bindKind;
                            exVar.registerName = exAllocReg;
                            exVar.isMutable = false;
                            exVar.scopeLevel = b_.currentScopeLevel_;
                            if (isObject)
                            {
                                exVar.className = bindClassName;
                                b_.classInstanceTypes_[clause.exceptionVar] = bindClassName;
                            }
                            b_.addVariable(exVar);
                        }

                        if (clause.body)
                        {
                            buildStatement(clause.body.get());
                        }

                        b_.exitScope();

                        // (AR) إزالة المعالج الثانوي قبل الخروج (إذا وجد)
                        // (EN) Pop secondary handler before exit (if exists)
                        if (hasFinally)
                        {
                            SIRInstruction popInst;
                            popInst.opcode = SIROpcode::CALL;
                            popInst.operands.push_back(SIROperand::Function("__sad_pop_handler"));
                            popInst.comment = "pop secondary handler after catch body";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(popInst);
                        }

                        if (b_.currentBlock_)
                        {
                            b_.currentBlock_->addInstruction(SIRInstruction::Branch(
                                SIROperand::Label(catchExitDest)));
                        }
                    }

                    // ================================================================
                    // (AR) كتلة "لا تطابق" — إعادة رمي الاستثناء
                    // (EN) "No match" block — rethrow exception
                    // ================================================================
                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(noMatchBlock);
                    b_.currentBlock_ = noMatchBlock;

                    if (hasFinally)
                    {
                        // (AR) إزالة المعالج الثانوي ثم تنفيذ finally ثم إعادة الرمي
                        // (EN) Pop secondary handler, run finally, then rethrow
                        {
                            SIRInstruction popInst;
                            popInst.opcode = SIROpcode::CALL;
                            popInst.operands.push_back(SIROperand::Function("__sad_pop_handler"));
                            popInst.comment = "pop secondary handler before rethrow";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(popInst);
                        }
                        // (AR) تنفيذ finally ثم إعادة رمي
                        // (EN) Run finally then rethrow
                        if (tryStmt->finallyBlock)
                        {
                            b_.enterScope();
                            buildStatement(tryStmt->finallyBlock.get());
                            b_.exitScope();
                        }
                        // (AR) إعادة رمي الاستثناء الحالي
                        // (EN) Re-throw current exception
                        {
                            SIRInstruction rethrowInst;
                            rethrowInst.opcode = SIROpcode::CALL;
                            rethrowInst.operands.push_back(SIROperand::Function("__sad_raise_current"));
                            rethrowInst.comment = "rethrow unmatched exception after finally";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(rethrowInst);
                        }
                        // (AR) كتلة ميتة
                        // (EN) Dead block
                        std::string deadLabel = b_.newLabel("dead_after_nomatch");
                        auto deadBlock = b_.createBasicBlock(deadLabel);
                        if (b_.currentFunction_)
                            b_.currentFunction_->addBasicBlock(deadBlock);
                        b_.currentBlock_ = deadBlock;
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
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(rethrowInst);
                        }
                        std::string deadLabel = b_.newLabel("dead_after_nomatch");
                        auto deadBlock = b_.createBasicBlock(deadLabel);
                        if (b_.currentFunction_)
                            b_.currentFunction_->addBasicBlock(deadBlock);
                        b_.currentBlock_ = deadBlock;
                    }

                    // ================================================================
                    // (AR) كتلة finally_rethrow — استثناء داخل catch + تنفيذ finally + إعادة رمي
                    // (EN) finally_rethrow — exception inside catch + run finally + rethrow
                    // ================================================================
                    if (hasFinally && finallyRethrowBlock)
                    {
                        if (b_.currentFunction_)
                            b_.currentFunction_->addBasicBlock(finallyRethrowBlock);
                        b_.currentBlock_ = finallyRethrowBlock;

                        // (AR) إزالة المعالج الثانوي
                        // (EN) Pop secondary handler
                        {
                            SIRInstruction popInst;
                            popInst.opcode = SIROpcode::CALL;
                            popInst.operands.push_back(SIROperand::Function("__sad_pop_handler"));
                            popInst.comment = "pop secondary handler in finally_rethrow";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(popInst);
                        }

                        // (AR) تنفيذ كود finally
                        // (EN) Execute finally code
                        if (tryStmt->finallyBlock)
                        {
                            b_.enterScope();
                            buildStatement(tryStmt->finallyBlock.get());
                            b_.exitScope();
                        }

                        // (AR) إعادة رمي الاستثناء
                        // (EN) Rethrow exception
                        {
                            SIRInstruction rethrowInst;
                            rethrowInst.opcode = SIROpcode::CALL;
                            rethrowInst.operands.push_back(SIROperand::Function("__sad_raise_current"));
                            rethrowInst.comment = "rethrow exception after finally";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(rethrowInst);
                        }
                        // (AR) كتلة ميتة
                        // (EN) Dead block
                        std::string deadLabel = b_.newLabel("dead_after_finally_rethrow");
                        auto deadRethrowBlock = b_.createBasicBlock(deadLabel);
                        if (b_.currentFunction_)
                            b_.currentFunction_->addBasicBlock(deadRethrowBlock);
                        b_.currentBlock_ = deadRethrowBlock;
                    }

                    // ================================================================
                    // (AR) كتلة finally — المسار العادي
                    //      يُدخل هذه الكتلة من: نهاية try الناجح، نهاية catch عادي
                    //      لكن ليس من ارجع داخل try/catch (تلك تذهب مباشرة لـ finallyLabel أيضاً)
                    //      بعد finally: فحص has_return → إرجاع القيمة المحفوظة أو الاستمرار للخروج
                    // (EN) Finally block — normal path
                    //      Entered from: successful try end, normal catch end
                    //      Return intercepted by SIRBuilderContext::FinallyContext also branches here
                    //      After finally: check has_return → return saved value or continue to exit
                    // ================================================================
                    if (hasFinally)
                    {
                        // (AR) إزالة SIRBuilderContext::FinallyContext من المكدس قبل بناء جسم finally
                        //      هذا يضمن أن ارجع داخل finally نفسه لا يعود لـ finally مرة أخرى
                        // (EN) Pop SIRBuilderContext::FinallyContext before building finally body
                        //      Ensures return inside finally itself does NOT loop back
                        if (!b_.finallyStack_.empty() && b_.finallyStack_.back().finallyLabel == finallyLabel)
                        {
                            b_.finallyStack_.pop_back();
                        }

                        if (b_.currentFunction_)
                            b_.currentFunction_->addBasicBlock(finallyBlock);
                        b_.currentBlock_ = finallyBlock;

                        if (tryStmt->finallyBlock)
                        {
                            b_.enterScope();
                            buildStatement(tryStmt->finallyBlock.get());
                            b_.exitScope();
                        }

                        // ================================================================
                        // (AR) بعد تنفيذ finally: فحص has_return
                        //      إذا has_return == 1 → أرجع القيمة المحفوظة (مسار ارجع)
                        //      إذا has_return == 0 → انتقل للخروج الطبيعي
                        // (EN) After finally executes: check has_return flag
                        //      If has_return == 1 → return the saved value (intercepted return path)
                        //      If has_return == 0 → branch to normal exit
                        // ================================================================
                        if (b_.currentBlock_ && !finallyHasRetReg.empty())
                        {
                            // (AR) تحميل علامة has_return
                            // (EN) Load has_return flag
                            std::string hasRetLoadReg = b_.newTempRegister();
                            {
                                SIRInstruction ld;
                                ld.opcode = SIROpcode::LOAD;
                                ld.result = SIROperand::Register(hasRetLoadReg, SadTypeKind::Integer);
                                ld.operands.push_back(SIROperand::Register(finallyHasRetReg, SadTypeKind::Integer));
                                ld.comment = "load finally has_return flag";
                                b_.currentBlock_->addInstruction(ld);
                            }
                            std::string hasCmpReg = b_.newTempRegister();
                            {
                                SIRInstruction cmp = SIRInstruction::Binary(
                                    SIROpcode::NE,
                                    SIROperand::Register(hasCmpReg, SadTypeKind::Boolean),
                                    SIROperand::Register(hasRetLoadReg, SadTypeKind::Integer),
                                    SIROperand::ConstantI64(0));
                                cmp.comment = "check: has_return != 0?";
                                b_.currentBlock_->addInstruction(cmp);
                            }

                            // (AR) إنشاء كتل: مسار الإرجاع ومسار الاستمرار الطبيعي
                            // (EN) Create blocks: return path and normal continuation path
                            std::string frRetLabel = b_.newLabel("finally_ret_start");
                            std::string frContLabel = b_.newLabel("finally_cont_normal");
                            auto frRetBlock = b_.createBasicBlock(frRetLabel);
                            auto frContBlock = b_.createBasicBlock(frContLabel);

                            {
                                SIRInstruction br = SIRInstruction::BranchCond(
                                    SIROperand::Register(hasCmpReg, SadTypeKind::Boolean),
                                    SIROperand::Label(frRetLabel),
                                    SIROperand::Label(frContLabel));
                                b_.currentBlock_->addInstruction(br);
                            }

                            // ════════════════════════════════════════════════════════
                            // (AR) كتلة مسار الإرجاع: فحص النوع وإرجاع القيمة المناسبة
                            //      typeCode: 2=نص/مؤشر، 0=فراغ، 1/3=رقم/عشري
                            // (EN) Return path block: check type and return proper value
                            //      typeCode: 2=string/ptr, 0=void, 1/3=integer/float
                            // ════════════════════════════════════════════════════════
                            if (b_.currentFunction_)
                                b_.currentFunction_->addBasicBlock(frRetBlock);
                            b_.currentBlock_ = frRetBlock;

                            // (AR) تحميل نوع القيمة المحفوظة
                            // (EN) Load saved return type code
                            std::string typeLoadReg = b_.newTempRegister();
                            {
                                SIRInstruction ld;
                                ld.opcode = SIROpcode::LOAD;
                                ld.result = SIROperand::Register(typeLoadReg, SadTypeKind::Integer);
                                ld.operands.push_back(SIROperand::Register(finallyRetTypeReg, SadTypeKind::Integer));
                                ld.comment = "load finally return type code";
                                b_.currentBlock_->addInstruction(ld);
                            }

                            // (AR) فحص: type == 2 (نص/مؤشر)؟
                            // (EN) Check: type == 2 (string/ptr)?
                            std::string isPtrReg = b_.newTempRegister();
                            {
                                SIRInstruction cmp = SIRInstruction::Binary(
                                    SIROpcode::EQ,
                                    SIROperand::Register(isPtrReg, SadTypeKind::Boolean),
                                    SIROperand::Register(typeLoadReg, SadTypeKind::Integer),
                                    SIROperand::ConstantI64(2));
                                cmp.comment = "check: type == 2 (ptr/string)?";
                                b_.currentBlock_->addInstruction(cmp);
                            }

                            std::string retPtrLabel = b_.newLabel("finally_ret_ptr");
                            std::string retNonPtrLabel = b_.newLabel("finally_ret_nonptr");
                            auto retPtrBlock = b_.createBasicBlock(retPtrLabel);
                            auto retNonPtrBlock = b_.createBasicBlock(retNonPtrLabel);

                            {
                                SIRInstruction br = SIRInstruction::BranchCond(
                                    SIROperand::Register(isPtrReg, SadTypeKind::Boolean),
                                    SIROperand::Label(retPtrLabel),
                                    SIROperand::Label(retNonPtrLabel));
                                b_.currentBlock_->addInstruction(br);
                            }

                            // ── مسار إرجاع نص/مؤشر (type == 2) ──
                            if (b_.currentFunction_)
                                b_.currentFunction_->addBasicBlock(retPtrBlock);
                            b_.currentBlock_ = retPtrBlock;
                            {
                                emitPopFunctionCleanupHandler();

                                std::string loadedPtr = b_.newTempRegister();
                                SIRInstruction ld;
                                ld.opcode = SIROpcode::LOAD;
                                ld.result = SIROperand::Register(loadedPtr, SadTypeKind::String);
                                ld.operands.push_back(SIROperand::Register(finallyRetPtrReg, SadTypeKind::String));
                                ld.comment = "load finally return string/ptr";
                                b_.currentBlock_->addInstruction(ld);

                                SIRInstruction retI;
                                retI.opcode = SIROpcode::RET;
                                retI.operands.push_back(SIROperand::Register(loadedPtr, SadTypeKind::String));
                                retI.comment = "return string/ptr value from finally";
                                b_.currentBlock_->addInstruction(retI);
                            }

                            // ── مسار إرجاع رقم/فراغ (type != 2) ──
                            if (b_.currentFunction_)
                                b_.currentFunction_->addBasicBlock(retNonPtrBlock);
                            b_.currentBlock_ = retNonPtrBlock;

                            // (AR) فحص: type == 0 (فراغ)؟
                            // (EN) Check: type == 0 (void)?
                            std::string isVoidReg = b_.newTempRegister();
                            {
                                SIRInstruction cmp = SIRInstruction::Binary(
                                    SIROpcode::EQ,
                                    SIROperand::Register(isVoidReg, SadTypeKind::Boolean),
                                    SIROperand::Register(typeLoadReg, SadTypeKind::Integer),
                                    SIROperand::ConstantI64(0));
                                cmp.comment = "check: type == 0 (void)?";
                                b_.currentBlock_->addInstruction(cmp);
                            }

                            std::string retVoidLabel2 = b_.newLabel("finally_ret_void");
                            std::string retI64Label2 = b_.newLabel("finally_ret_i64");
                            auto retVoidBlock = b_.createBasicBlock(retVoidLabel2);
                            auto retI64Block = b_.createBasicBlock(retI64Label2);

                            {
                                SIRInstruction br = SIRInstruction::BranchCond(
                                    SIROperand::Register(isVoidReg, SadTypeKind::Boolean),
                                    SIROperand::Label(retVoidLabel2),
                                    SIROperand::Label(retI64Label2));
                                b_.currentBlock_->addInstruction(br);
                            }

                            // ── مسار إرجاع فراغ ──
                            if (b_.currentFunction_)
                                b_.currentFunction_->addBasicBlock(retVoidBlock);
                            b_.currentBlock_ = retVoidBlock;
                            {
                                emitPopFunctionCleanupHandler();

                                SIRInstruction retV;
                                retV.opcode = SIROpcode::RET_VOID;
                                retV.comment = "return void from finally";
                                b_.currentBlock_->addInstruction(retV);
                            }

                            // ── مسار إرجاع رقم (type == 1 أو 3) ──
                            if (b_.currentFunction_)
                                b_.currentFunction_->addBasicBlock(retI64Block);
                            b_.currentBlock_ = retI64Block;
                            {
                                emitPopFunctionCleanupHandler();

                                std::string loadedI64 = b_.newTempRegister();
                                SIRInstruction ld;
                                ld.opcode = SIROpcode::LOAD;
                                ld.result = SIROperand::Register(loadedI64, SadTypeKind::Integer);
                                ld.operands.push_back(SIROperand::Register(finallyRetI64Reg, SadTypeKind::Integer));
                                ld.comment = "load finally return i64/float value";
                                b_.currentBlock_->addInstruction(ld);

                                SIRInstruction retI;
                                retI.opcode = SIROpcode::RET;
                                retI.operands.push_back(SIROperand::Register(loadedI64, SadTypeKind::Integer));
                                retI.comment = "return i64 value from finally";
                                b_.currentBlock_->addInstruction(retI);
                            }

                            // ════════════════════════════════════════════════════════
                            // (AR) كتلة الاستمرار الطبيعي: اذهب للخروج (has_return == 0)
                            // (EN) Normal continuation block: go to exit (has_return == 0)
                            // ════════════════════════════════════════════════════════
                            if (b_.currentFunction_)
                                b_.currentFunction_->addBasicBlock(frContBlock);
                            b_.currentBlock_ = frContBlock;
                            b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(exitLabel)));
                        }
                        else
                        {
                            // (AR) لا سجلات finally (بدون ارجع) — مسار عادي للخروج
                            // (EN) No finally registers (no return) — normal exit path
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(exitLabel)));
                        }
                    }

                    // (AR) كتلة الخروج
                    // (EN) Exit block
                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(exitBlock);
                    b_.currentBlock_ = exitBlock;
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
                        exResult = b_.buildExpression(raiseStmt->exception.get());
                    }

                    // (AR) استدعاء دالة runtime لرفع الاستثناء مع النوع
                    // (EN) Call runtime function to raise exception with type
                    // (AR) الصيغة: __sad_raise(type, msg) — إذا كان كائناً نستخدم اسم الصنف، وإلا "خطأ"
                    // (EN) Format: __sad_raise(type, msg) — use class name if object, else "خطأ"
                    SIRInstruction raiseInst;
                    raiseInst.opcode = SIROpcode::CALL;
                    raiseInst.operands.push_back(SIROperand::Function("__sad_raise"));

                    // (AR) تحديد نوع القيمة المرميّة: NewExpr ⇒ صنف؛ متغيّر يحمل كائنًا ⇒
                    //      صنف باسمه؛ وإلّا نوع BuildResult (رقم/عشريّ/منطقيّ/نصّ).
                    // (EN) Determine the thrown value's kind: NewExpr ⇒ Class; a variable
                    //      holding an object ⇒ Class by its name; else the BuildResult type
                    //      (number/float/bool/string).
                    std::string exceptionTypeName = "\xd8\xae\xd8\xb7\xd8\xa3"; // "خطأ" default
                    SadTypeKind thrownKind = raiseStmt->exception ? exResult.type : SadTypeKind::String;
                    std::string thrownClass;
                    if (raiseStmt->exception)
                    {
                        if (auto *newExpr = dynamic_cast<Sad::AST::NewExpr *>(raiseStmt->exception.get()))
                        {
                            exceptionTypeName = newExpr->className;
                            thrownKind = SadTypeKind::Class;
                            thrownClass = newExpr->className;
                        }
                        else if (!exResult.className.empty())
                        {
                            exceptionTypeName = exResult.className;
                            thrownKind = SadTypeKind::Class;
                            thrownClass = exResult.className;
                        }
                    }
                    raiseInst.operands.push_back(SIROperand::ConstantString(exceptionTypeName));

                    // (AR) سجّل النوع المرميّ في خانة استنتاج «حاول» العليا (إن وُجدت)
                    //      لربط متغيّر «امسك» بنوعه الساكن الصحيح لاحقًا.
                    // (EN) Record the thrown kind into the top «try» inference slot (if any)
                    //      so the «catch» variable can be bound with its correct static type.
                    if (!b_.tryThrownStack_.empty())
                    {
                        auto &slot = b_.tryThrownStack_.back();
                        if (!slot.sawThrow)
                        {
                            slot.sawThrow = true;
                            slot.kind = thrownKind;
                            slot.className = thrownClass;
                        }
                        else if (slot.kind != thrownKind || slot.className != thrownClass)
                        {
                            slot.mixed = true;
                        }
                    }

                    // (AR) الوسيط الثاني: القيمة المرميّة — بنوع SIR الصحيح فتُخزَّن الخلفيّةُ
                    //      المؤشّرَ (نصّ/كائن) في msg العامّ، والقيمةَ العدديّة في value العامّ
                    //      (لا تخزين i64 في مؤشّر ⇒ لا انهيار). المؤشّرات تُمرَّر كسجلّ.
                    // (EN) Second arg: the thrown value — with its correct SIR type so the
                    //      backend stores a pointer (string/object) into the msg global and a
                    //      scalar into the value global (no i64-into-ptr ⇒ no crash). Pointer
                    //      payloads are passed as a register.
                    if (raiseStmt->exception)
                    {
                        if (!exResult.registerName.empty())
                        {
                            raiseInst.operands.push_back(SIROperand::Register(exResult.registerName, exResult.type));
                        }
                        else if (exResult.isConstant)
                        {
                            switch (exResult.type)
                            {
                            case SadTypeKind::Integer:
                                raiseInst.operands.push_back(
                                    SIROperand::ConstantI64(std::stoll(exResult.constantValue)));
                                break;
                            case SadTypeKind::Float:
                                raiseInst.operands.push_back(
                                    SIROperand::ConstantF64(std::stod(exResult.constantValue)));
                                break;
                            case SadTypeKind::Boolean:
                                raiseInst.operands.push_back(SIROperand::ConstantBool(
                                    exResult.constantValue == "true" ||
                                    exResult.constantValue == "1" ||
                                    exResult.constantValue == "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad")); // "صحيح"
                                break;
                            case SadTypeKind::String:
                            default:
                                raiseInst.operands.push_back(
                                    SIROperand::ConstantString(exResult.constantValue));
                                break;
                            }
                        }
                    }
                    raiseInst.comment = "raise exception (type + value)";

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(raiseInst);
                    }

                    // (AR) بعد الرفع، لا يمكن الوصول لهذه النقطة — كتلة للكود الميت
                    // (EN) After raise, this point is unreachable — dead code block
                    // (AR) لا نصدر RET_VOID لأن __sad_raise يستدعي longjmp فلا يعود أبداً
                    // (EN) No RET_VOID because __sad_raise calls longjmp and never returns

                    // (AR) كتلة جديدة للكود بعد raise (كود ميت)
                    // (EN) New block for code after raise (dead code)
                    std::string afterRaiseLabel = b_.newLabel("after_raise");
                    auto afterRaiseBlock = b_.createBasicBlock(afterRaiseLabel);
                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(afterRaiseBlock);
                    b_.currentBlock_ = afterRaiseBlock;
                    return true;
                }

                // ========================================================================

                return false;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad