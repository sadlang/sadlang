// ============================================================================
// sir_builder_loops.cpp - حلقات while و for
// ============================================================================
// (AR) هذا الملف يحتوي على:
//      - buildWhileLoop: بناء حلقة بينما
//      - buildForLoop: بناء حلقة لكل (for)
// (EN) This file contains:
//      - buildWhileLoop: Build while loop
//      - buildForLoop: Build for loop
// ============================================================================
// تم استخراج هذا الملف من sir_builder_control_flow.cpp وفقاً لقاعدة CW-05
// ============================================================================

#include <string>
#include "sir_builder.h"
#include "builders/statement_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
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
            void StatementBuilder::buildWhileLoop(AST::WhileStmt *whileLoop)
            {
                if (!whileLoop)
                {
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildWhileLoop: starting" << std::endl;
#endif

                // ========================================================================
                // (AR) الخطوة 1: إنشاء الكتل الأساسية
                // (EN) Step 1: Create basic blocks
                // المصدر: sir_builder.h:501 - b_.createBasicBlock()
                // المصدر: sir_builder.h:520 - b_.newLabel()
                // ========================================================================
                std::string condLabel = b_.newLabel("while_cond");
                std::string bodyLabel = b_.newLabel("while_body");
                std::string exitLabel = b_.newLabel("while_exit");

                auto condBlock = b_.createBasicBlock(condLabel);
                auto bodyBlock = b_.createBasicBlock(bodyLabel);
                auto exitBlock = b_.createBasicBlock(exitLabel);

                // (AR) إضافة الكتل إلى الدالة الحالية
                // (EN) Add blocks to current function
                if (b_.currentFunction_)
                {
                    b_.currentFunction_->addBasicBlock(condBlock);
                    b_.currentFunction_->addBasicBlock(bodyBlock);
                    b_.currentFunction_->addBasicBlock(exitBlock);
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildWhileLoop: created blocks cond=" << condLabel
                          << ", body=" << bodyLabel << ", exit=" << exitLabel << std::endl;
#endif

                // ========================================================================
                // (AR) الخطوة 2: قفز غير شرطي إلى كتلة الشرط
                // (EN) Step 2: Unconditional jump to condition block
                // المصدر: sir_instruction.h:178-183 - SIRInstruction::Branch()
                // ========================================================================
                SIROperand condLabelOp = SIROperand::Label(condLabel);
                SIRInstruction brCondBlockInst = SIRInstruction::Branch(condLabelOp);

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->instructions.push_back(brCondBlockInst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildWhileLoop: added BR to condition block" << std::endl;
#endif
                }

                // ========================================================================
                // (AR) الخطوة 3: بناء الشرط
                // (EN) Step 3: Build condition expression
                // المصدر: WhileStmt::condition (statements.h:149)
                // ========================================================================
                b_.currentBlock_ = condBlock;
                auto condResult = b_.buildExpression(whileLoop->condition.get());

                if (condResult.registerName.empty())
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildWhileLoop: condition build failed!" << std::endl;
#endif
                    b_.errors_.push_back("Error: Failed to build while condition");
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildWhileLoop: condition reg=" << condResult.registerName << std::endl;
#endif

                // (AR) تحويل تلقائي لـ __op_tobool__ إذا كان الشرط كائناً (مثل buildIfStatement)
                // (EN) Auto-convert __op_tobool__ if condition is an object (like buildIfStatement)
                {
                    std::string condClassName = condResult.className;
                    if (condClassName.empty() && !condResult.registerName.empty())
                    {
                        auto it = b_.classInstanceTypes_.find(condResult.registerName);
                        if (it != b_.classInstanceTypes_.end())
                            condClassName = it->second;
                    }
                    if (!condClassName.empty())
                    {
                        // (AR) بحث في سلسلة الوراثة عن __op_tobool__
                        // (EN) Search inheritance chain for __op_tobool__
                        std::string searchClass = condClassName;
                        std::string toboolName;
                        bool foundToBool = false;
                        while (!searchClass.empty())
                        {
                            toboolName = searchClass + ".__op_tobool__";
                            if (b_.functionTable_.find(toboolName) != b_.functionTable_.end())
                            {
                                foundToBool = true;
                                break;
                            }
                            auto classInfo = b_.module_->getClass(searchClass);
                            if (classInfo && !classInfo->parentClass.empty())
                                searchClass = classInfo->parentClass;
                            else
                                break;
                        }
                        if (foundToBool)
                        {
                            std::string boolReg = b_.newTempRegister();
                            SIRInstruction callInst;
                            callInst.opcode = SIROpcode::OBJECT_CALL;
                            callInst.result = SIROperand::Register(boolReg, SadTypeKind::Boolean);
                            callInst.operands.push_back(SIROperand::Register(condResult.registerName, condResult.type));
                            callInst.operands.push_back(SIROperand::ConstantString("__op_tobool__"));
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(callInst);
                            condResult = BuildResult(boolReg, SadTypeKind::Boolean);
                        }
                    }
                }

                // ========================================================================
                // (AR) الخطوة 4: توليد تعليمة القفز الشرطي
                // (EN) Step 4: Generate conditional branch instruction
                // المصدر: sir_instruction.h:190-197 - SIRInstruction::BranchCond()
                // ========================================================================
                // (AR) إذا كان الشرط ثابتاً منطقياً (صحيح/خطأ)، نستخدم ConstantBool بدلاً من Register
                //      لتجنب توليد سجل غير معرّف في LLVM IR
                // (EN) If condition is a boolean constant (true/false), use ConstantBool instead of Register
                //      to avoid generating an undefined register in LLVM IR
                SIROperand condOp;
                if (condResult.isConstant && condResult.type == SadTypeKind::Boolean)
                {
                    condOp = SIROperand::ConstantBool(condResult.constantValue == "true" || condResult.constantValue == "1");
                }
                else if (condResult.isConstant && condResult.type == SadTypeKind::Integer)
                {
                    condOp = SIROperand::ConstantI64(std::stoll(condResult.constantValue));
                }
                else
                {
                    condOp = SIROperand::Register(condResult.registerName, condResult.type);
                }
                SIROperand bodyLabelOp = SIROperand::Label(bodyLabel);
                SIROperand exitLabelOp = SIROperand::Label(exitLabel);

                SIRInstruction brCondInst = SIRInstruction::BranchCond(condOp, bodyLabelOp, exitLabelOp);

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->instructions.push_back(brCondInst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildWhileLoop: added BR_COND (body/exit)" << std::endl;
#endif
                }

                // ========================================================================
                // (AR) الخطوة 4.5: تسجيل سياق الحلقة لدعم break/continue
                //      continueLabel = كتلة الشرط (while_cond) — continue يقفز للشرط
                //      breakLabel = كتلة الخروج (while_exit) — break يقفز للخروج
                // (EN) Step 4.5: Register loop context for break/continue support
                //      continueLabel = condition block (while_cond) — continue jumps to condition
                //      breakLabel = exit block (while_exit) — break jumps to exit
                // ========================================================================
                LoopContext whileLoopCtx;
                whileLoopCtx.continueLabel = condLabel;
                whileLoopCtx.breakLabel = exitLabel;
                b_.enterLoop(whileLoopCtx);

                // ========================================================================
                // (AR) الخطوة 5: بناء جسم الحلقة
                // (EN) Step 5: Build loop body
                // المصدر: WhileStmt::body (statements.h:150)
                // ========================================================================
                b_.currentBlock_ = bodyBlock;
                if (whileLoop->body)
                {
                    buildStatement(whileLoop->body.get());
                }

                // (AR) الخروج من سياق الحلقة بعد بناء الجسم
                // (EN) Exit loop context after building body
                b_.exitLoop();

                // ========================================================================
                // (AR) الخطوة 6: قفز للعودة إلى كتلة الشرط
                // (EN) Step 6: Jump back to condition block
                // (AR) لا نضيف القفز إذا كان الجسم قد انتهى بـ RET أو BR أو BR_COND
                // (EN) Don't add branch if the body already ends with RET or BR or BR_COND
                // ========================================================================
                SIRInstruction brBackInst = SIRInstruction::Branch(condLabelOp);

                if (b_.currentBlock_ && !b_.currentBlock_->instructions.empty())
                {
                    const auto &lastInst = b_.currentBlock_->instructions.back();
                    bool hasTerminator = (lastInst.opcode == SIROpcode::RET ||
                                          lastInst.opcode == SIROpcode::RET_VOID ||
                                          lastInst.opcode == SIROpcode::BR ||
                                          lastInst.opcode == SIROpcode::BR_COND);
                    if (!hasTerminator)
                    {
                        b_.currentBlock_->instructions.push_back(brBackInst);
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildWhileLoop: added BR back to condition" << std::endl;
#endif
                    }
                    else
                    {
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildWhileLoop: body block already has terminator, skipping BR" << std::endl;
#endif
                    }
                }
                else if (b_.currentBlock_)
                {
                    b_.currentBlock_->instructions.push_back(brBackInst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildWhileLoop: added BR back to condition (empty block)" << std::endl;
#endif
                }

                // ========================================================================
                // (AR) الخطوة 7: الاستمرار بعد الحلقة
                // (EN) Step 7: Continue after loop
                // ========================================================================
                b_.currentBlock_ = exitBlock;
#ifndef NDEBUG
                std::cout << "[DEBUG] buildWhileLoop: completed, now at exit block" << std::endl;
#endif
            }

            // ============================================================================
            // buildForLoop - بناء حلقة for
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:401
            // التوقيع / Signature: void buildForLoop(AST::ForStmt* forLoop);
            //
            // المعاملات / Parameters:
            // - forLoop: AST::ForStmt* = Sad::AST::ForStmt* (sir_builder.h:401)
            //
            // ForStmt Members (statements.h:187-253):
            // - initializer: StmtPtr (line 193) - optional
            // - condition: ExprPtr (line 194) - optional
            // - increment: ExprPtr (line 195) - optional
            // - body: StmtPtr (line 196)
            //
            // المتغيرات المستخدمة / Used variables:
            // - b_.currentBlock_: sir_builder.h:582 (shared_ptr<SIRBasicBlock>)
            // - b_.currentScopeLevel_: sir_builder.h:599 (int)
            //
            // الدوال المستدعاة / Called functions:
            // - buildStatement: sir_builder.h:372
            // - b_.buildExpression: sir_builder.h:432
            // - b_.createBasicBlock: sir_builder.h:501
            // - b_.newLabel: sir_builder.h:520
            // - b_.enterScope: sir_builder.h:587
            // - b_.exitScope: sir_builder.h:589
            // ============================================================================
            void StatementBuilder::buildForLoop(AST::ForStmt *forLoop)
            {
                if (!forLoop)
                {
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildForLoop: starting" << std::endl;
#endif

                // ========================================================================
                // (AR) الخطوة 1: دخول نطاق جديد للحلقة
                // (EN) Step 1: Enter new scope for loop
                // المصدر: sir_builder.h:587 - b_.enterScope()
                // ========================================================================
                b_.enterScope();

                // ========================================================================
                // (AR) الخطوة 2: تنفيذ initializer إذا وُجد
                // (EN) Step 2: Execute initializer if exists
                // المصدر: ForStmt::initializer (statements.h:193)
                // ========================================================================
                if (forLoop->initializer)
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildForLoop: building initializer" << std::endl;
#endif
                    buildStatement(forLoop->initializer.get());
                }

                // ========================================================================
                // (AR) الخطوة 3: إنشاء الكتل الأساسية
                // (EN) Step 3: Create basic blocks
                // ========================================================================
                std::string condLabel = b_.newLabel("for_cond");
                std::string bodyLabel = b_.newLabel("for_body");
                std::string incLabel = b_.newLabel("for_inc");
                std::string exitLabel = b_.newLabel("for_exit");

                auto condBlock = b_.createBasicBlock(condLabel);
                auto bodyBlock = b_.createBasicBlock(bodyLabel);
                auto incBlock = b_.createBasicBlock(incLabel);
                auto exitBlock = b_.createBasicBlock(exitLabel);

                // (AR) إضافة الكتل إلى الدالة الحالية
                // (EN) Add blocks to current function
                if (b_.currentFunction_)
                {
                    b_.currentFunction_->addBasicBlock(condBlock);
                    b_.currentFunction_->addBasicBlock(bodyBlock);
                    b_.currentFunction_->addBasicBlock(incBlock);
                    b_.currentFunction_->addBasicBlock(exitBlock);
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildForLoop: created blocks cond=" << condLabel
                          << ", body=" << bodyLabel << ", inc=" << incLabel
                          << ", exit=" << exitLabel << std::endl;
#endif

                // ========================================================================
                // (AR) الخطوة 4: قفز غير شرطي إلى كتلة الشرط
                // (EN) Step 4: Unconditional jump to condition block
                // ========================================================================
                SIROperand condLabelOp = SIROperand::Label(condLabel);
                SIRInstruction brCondBlockInst = SIRInstruction::Branch(condLabelOp);

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->instructions.push_back(brCondBlockInst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildForLoop: added BR to condition block" << std::endl;
#endif
                }

                // ========================================================================
                // (AR) الخطوة 5: بناء الشرط
                // (EN) Step 5: Build condition
                // المصدر: ForStmt::condition (statements.h:194)
                // ========================================================================
                b_.currentBlock_ = condBlock;

                SIROperand bodyLabelOp = SIROperand::Label(bodyLabel);
                SIROperand exitLabelOp = SIROperand::Label(exitLabel);

                if (forLoop->condition)
                {
                    auto condResult = b_.buildExpression(forLoop->condition.get());

                    if (!condResult.registerName.empty())
                    {
                        // (AR) توليد BR شرطي لـ body أو exit
                        // (EN) Generate conditional BR to body or exit
                        // (AR) إذا كان الشرط ثابتاً منطقياً، نستخدم ConstantBool بدلاً من Register
                        // (EN) If condition is a boolean constant, use ConstantBool instead of Register
                        SIROperand condOp;
                        if (condResult.isConstant && condResult.type == SadTypeKind::Boolean)
                        {
                            condOp = SIROperand::ConstantBool(condResult.constantValue == "true" || condResult.constantValue == "1");
                        }
                        else if (condResult.isConstant && condResult.type == SadTypeKind::Integer)
                        {
                            condOp = SIROperand::ConstantI64(std::stoll(condResult.constantValue));
                        }
                        else
                        {
                            condOp = SIROperand::Register(condResult.registerName, condResult.type);
                        }
                        SIRInstruction brCondInst = SIRInstruction::BranchCond(condOp, bodyLabelOp, exitLabelOp);

                        if (b_.currentBlock_)
                        {
                            b_.currentBlock_->instructions.push_back(brCondInst);
#ifndef NDEBUG
                            std::cout << "[DEBUG] buildForLoop: added BR_COND (body/exit)" << std::endl;
#endif
                        }
                    }
                }
                else
                {
                    // (AR) لا يوجد شرط - قفز غير شرطي لـ body (حلقة لا نهائية)
                    // (EN) No condition - unconditional jump to body (infinite loop)
                    SIRInstruction brBodyInst = SIRInstruction::Branch(bodyLabelOp);

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(brBodyInst);
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildForLoop: no condition, added BR to body" << std::endl;
#endif
                    }
                }

                // ========================================================================
                // (AR) الخطوة 5.5: تسجيل سياق الحلقة لدعم break/continue
                //      continueLabel = كتلة الزيادة (for_inc) — continue يقفز للزيادة أولاً
                //      breakLabel = كتلة الخروج (for_exit) — break يقفز للخروج مباشرة
                //      ملاحظة: في for، continue يجب أن ينفّذ الزيادة ثم يعود للشرط
                // (EN) Step 5.5: Register loop context for break/continue support
                //      continueLabel = increment block (for_inc) — continue goes to increment first
                //      breakLabel = exit block (for_exit) — break jumps to exit directly
                //      Note: In for, continue must execute increment then go to condition
                // ========================================================================
                LoopContext forLoopCtx;
                forLoopCtx.continueLabel = incLabel;
                forLoopCtx.breakLabel = exitLabel;
                b_.enterLoop(forLoopCtx);

                // ========================================================================
                // (AR) الخطوة 6: بناء جسم الحلقة
                // (EN) Step 6: Build loop body
                // المصدر: ForStmt::body (statements.h:196)
                // ========================================================================
                b_.currentBlock_ = bodyBlock;
                if (forLoop->body)
                {
                    buildStatement(forLoop->body.get());
                }

                // (AR) الخروج من سياق الحلقة بعد بناء الجسم
                // (EN) Exit loop context after building body
                b_.exitLoop();

                // (AR) قفز إلى كتلة الزيادة
                // (EN) Jump to increment block
                SIROperand incLabelOp = SIROperand::Label(incLabel);
                SIRInstruction brIncInst = SIRInstruction::Branch(incLabelOp);

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->instructions.push_back(brIncInst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildForLoop: added BR to increment block" << std::endl;
#endif
                }

                // ========================================================================
                // (AR) الخطوة 7: بناء الزيادة
                // (EN) Step 7: Build increment
                // المصدر: ForStmt::increment (statements.h:195)
                // ========================================================================
                b_.currentBlock_ = incBlock;
                if (forLoop->increment)
                {
                    b_.buildExpression(forLoop->increment.get());
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildForLoop: built increment expression" << std::endl;
#endif
                }

                // (AR) قفز للعودة إلى كتلة الشرط
                // (EN) Jump back to condition block
                SIRInstruction brBackInst = SIRInstruction::Branch(condLabelOp);

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->instructions.push_back(brBackInst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildForLoop: added BR back to condition" << std::endl;
#endif
                }

                // ========================================================================
                // (AR) الخطوة 8: الاستمرار بعد الحلقة
                // (EN) Step 8: Continue after loop
                // ========================================================================
                b_.currentBlock_ = exitBlock;

                // (AR) الخروج من نطاق الحلقة
                // (EN) Exit loop scope
                b_.exitScope();

#ifndef NDEBUG
                std::cout << "[DEBUG] buildForLoop: completed, now at exit block" << std::endl;
#endif
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
