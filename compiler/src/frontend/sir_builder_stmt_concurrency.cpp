// ============================================================================
// sir_builder_stmt_concurrency.cpp
// ============================================================================
#include <string>
#include "sir_builder.h"
#include "module_nodes.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
#include "../../../shared/utils/include/utf8_utils.h"
#include <iostream>
#include <set>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            bool SIRBuilder::buildStatement_Go(AST::Statement *stmt)
            {
                // ========================================================================
                if (auto goStmt = dynamic_cast<Sad::AST::GoStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found GoStmt" << std::endl;
#endif
                    if (goStmt->expression)
                    {
                        // (AR) أطلق تعبير — نبني التعبير كاستدعاء دالة
                        // (EN) go expression — build expression as function call
                        auto exprResult = buildExpression(goStmt->expression.get());

                        // (AR) إصلاح: بعض تعابير الاستدعاء ذات نوع void لا تُنتج register صالح.
                        //      في هذه الحالة التعبير نفسه يكون قد أضاف تعليماته بالفعل،
                        //      لذلك لا نولّد ASYNC_SPAWN بسجل غير معرّف.
                        // (EN) Fix: some void call expressions do not produce a valid register.
                        //      In that case, expression lowering already emitted instructions,
                        //      so avoid emitting ASYNC_SPAWN with an undefined register.
                        if (!exprResult.registerName.empty())
                        {
                            std::string resultReg = newTempRegister();
                            SIRInstruction spawnInst(SIROpcode::ASYNC_SPAWN);
                            spawnInst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                            spawnInst.operands.push_back(
                                SIROperand::Register(exprResult.registerName, exprResult.type));
                            spawnInst.comment = "go expression → ASYNC_SPAWN";
                            if (currentBlock_)
                                currentBlock_->instructions.push_back(spawnInst);
                        }
                    }
                    else if (goStmt->blockBody)
                    {
                        // (AR) إصلاح جذري لكتل أطلق المتداخلة:
                        //      التنفيذ الخلفي الحالي للـ ASYNC_SPAWN يعمل بشكل متزامن (direct call)
                        //      ولا يدعم captures كاملة للمتغيرات المحلية داخل go-blocks المتداخلة.
                        //      لذلك نُخفض go-block مباشرة داخل نفس السياق للحفاظ على صحة النطاقات
                        //      ومنع توليد سجلات غير معرّفة مثل %مج_داخلي في الاختبارات المركبة.
                        // (EN) Root fix for nested go blocks:
                        //      current ASYNC_SPAWN backend behaves synchronously (direct call)
                        //      and does not fully support local captures in nested go-blocks.
                        //      So we lower go-block inline in the same scope to preserve
                        //      variable visibility and avoid undefined registers (e.g. %مج_داخلي).
                        buildStatement(goStmt->blockBody.get());
                    }
                    return true;
                }

                return false;
            }

            bool SIRBuilder::buildStatement_Select(AST::Statement *stmt)
            {

                // ========================================================================
                // (AR) SelectStmt - جملة اختر — اختيار من قنوات متعددة
                // (EN) Select statement — multiplex on multiple channels
                //
                // (AR) التنفيذ: نُحوّل إلى سلسلة فحوصات:
                //      لكل حالة: ASYNC_CHANNEL_HAS_DATA → إذا نعم: ASYNC_CHANNEL_RECV + جسم
                //      إذا لم تنجح أي حالة: جسم الافتراضي (إن وُجد)
                // (EN) Implementation: lower to check chain:
                //      for each case: ASYNC_CHANNEL_HAS_DATA → if yes: ASYNC_CHANNEL_RECV + body
                //      if no case ready: default body (if exists)
                // ========================================================================
                if (auto selectStmt = dynamic_cast<Sad::AST::SelectStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found SelectStmt with " << selectStmt->cases.size() << " cases" << std::endl;
#endif
                    // (AR) إنشاء تسمية الخروج
                    std::string exitLabel = newLabel("select_exit");
                    auto exitBlock = createBasicBlock(exitLabel);

                    for (size_t i = 0; i < selectStmt->cases.size(); ++i)
                    {
                        auto &selCase = selectStmt->cases[i];

                        // (AR) إنشاء تسميات لجسم الحالة والحالة التالية
                        std::string bodyLabel = newLabel("select_case_" + std::to_string(i));
                        std::string nextLabel = newLabel("select_next_" + std::to_string(i));
                        auto bodyBlock = createBasicBlock(bodyLabel);
                        auto nextBlock = createBasicBlock(nextLabel);

                        // (AR) بناء تعبير القناة (عادةً ق.حاول_استقبل() أو ق.استقبل())
                        // (EN) Build channel expression => check if data available
                        if (selCase->channelExpr)
                        {
                            std::string checkReg = newTempRegister();
                            bool loweredToTryRecvCompare = false;

                            // (AR) إصلاح دلالة select:
                            //      عندما تكون الحالة من الشكل ق.حاول_استقبل() يجب أن نستهلك
                            //      العنصر عند النجاح. لذلك نُبقي الاستدعاء try_recv نفسه ونقارن
                            //      الناتج مع null-sentinel (لاشيء) بدلاً من المقارنة مع 0.
                            // (EN) Fix select semantics:
                            //      for ch.try_recv() cases we must consume on success.
                            //      Keep try_recv call and compare result with null-sentinel
                            //      instead of comparing with 0.
                            if (auto m = dynamic_cast<Sad::AST::MethodCallExpr *>(selCase->channelExpr.get()))
                            {
                                const std::string &mn = m->methodName;
                                const bool isTryRecv =
                                    mn == "\xD8\xAD\xD8\xA7\xD9\x88\xD9\x84_\xD8\xA7\xD8\xB3\xD8\xAA\xD9\x82\xD8\xA8\xD9\x84" ||
                                    mn == "try_recv";
                                if (isTryRecv)
                                {
                                    auto recvResult = buildExpression(selCase->channelExpr.get());
                                    if (!recvResult.registerName.empty())
                                    {
                                        SIRInstruction cmpInst = SIRInstruction::Binary(
                                            SIROpcode::NE,
                                            SIROperand::Register(checkReg, SadTypeKind::Boolean),
                                            SIROperand::Register(recvResult.registerName, recvResult.type),
                                            SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel));
                                        cmpInst.comment = "select: try_recv != null case " + std::to_string(i);
                                        if (currentBlock_)
                                            currentBlock_->addInstruction(cmpInst);
                                        loweredToTryRecvCompare = true;
                                    }
                                }
                            }

                            if (!loweredToTryRecvCompare)
                            {
                                // (AR) المسار العام: تقييم التعبير ثم مقارنة != 0
                                auto chanResult = buildExpression(selCase->channelExpr.get());
                                SIRInstruction cmpInst = SIRInstruction::Binary(
                                    SIROpcode::NE,
                                    SIROperand::Register(checkReg, SadTypeKind::Boolean),
                                    SIROperand::Register(chanResult.registerName, chanResult.type),
                                    SIROperand::ConstantI64(0));
                                cmpInst.comment = "select: check channel case " + std::to_string(i);
                                if (currentBlock_)
                                    currentBlock_->addInstruction(cmpInst);
                            }

                            // (AR) تفرع: إذا جاهزة → الجسم، وإلا → الحالة التالية
                            SIRInstruction brInst = SIRInstruction::BranchCond(
                                SIROperand::Register(checkReg, SadTypeKind::Boolean),
                                SIROperand::Label(bodyLabel),
                                SIROperand::Label(nextLabel));
                            if (currentBlock_)
                                currentBlock_->addInstruction(brInst);
                        }
                        else
                        {
                            // (AR) لا يوجد تعبير — تخطي إلى التالي
                            if (currentBlock_)
                                currentBlock_->addInstruction(
                                    SIRInstruction::Branch(SIROperand::Label(nextLabel)));
                        }

                        // (AR) كتلة جسم الحالة
                        if (currentFunction_)
                            currentFunction_->addBasicBlock(bodyBlock);
                        currentBlock_ = bodyBlock;

                        // (AR) بناء جسم الحالة
                        for (auto &bodyStmt : selCase->body)
                        {
                            if (bodyStmt)
                                buildStatement(bodyStmt.get());
                        }

                        // (AR) قفز إلى الخروج بعد الجسم
                        if (currentBlock_ && !currentBlock_->getTerminator())
                        {
                            currentBlock_->addInstruction(
                                SIRInstruction::Branch(SIROperand::Label(exitLabel)));
                        }

                        // (AR) كتلة الحالة التالية
                        if (currentFunction_)
                            currentFunction_->addBasicBlock(nextBlock);
                        currentBlock_ = nextBlock;
                    }

                    // (AR) الحالة الافتراضية
                    if (!selectStmt->defaultBody.empty())
                    {
                        for (auto &defStmt : selectStmt->defaultBody)
                        {
                            if (defStmt)
                                buildStatement(defStmt.get());
                        }
                    }

                    // (AR) قفز إلى الخروج
                    if (currentBlock_ && !currentBlock_->getTerminator())
                    {
                        currentBlock_->addInstruction(
                            SIRInstruction::Branch(SIROperand::Label(exitLabel)));
                    }

                    // (AR) كتلة الخروج
                    if (currentFunction_)
                        currentFunction_->addBasicBlock(exitBlock);
                    currentBlock_ = exitBlock;

                    return true;
                }

                return false;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
