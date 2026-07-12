// ============================================================================
// sir_builder_stmt_concurrency.cpp
// ============================================================================
#include <string>
#include "sir_builder.h"
#include "builders/statement_builder.h"
#include "module_nodes.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
#include "utf8_utils.h"
#include <iostream>
#include <set>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            bool StatementBuilder::buildStatement_Go(AST::Statement *stmt)
            {
                // ========================================================================
                if (auto goStmt = dynamic_cast<Sad::AST::GoStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found GoStmt" << std::endl;
#endif
                    if (goStmt->expression)
                    {
                        // (AR) أطلق تعبير — نبني التعبير (استدعاء دالة/لامدا). التنفيذ الخلفي
                        //      الحالي لعمل «أطلق» متزامن (كما في مسار go-block أدناه)، وبناء
                        //      التعبير هنا يُصدر الاستدعاء فينفّذه فعليًّا.
                        //      إصلاح جذري (0xC0000005): لا نُصدر ASYNC_SPAWN على *سجل نتيجة*
                        //      الاستدعاء — فنتيجة نداء void تُحلّ إلى مؤشّر null، فيولّد الخلفيّ
                        //      sad_rt_thread_spawn(null, null) الذي يُطلق خيط OS حقيقيًّا بدالة
                        //      بداية null/قمامة ⇒ انهيار وصول غير حتميّ. العمل نُفّذ متزامنًا
                        //      بالفعل، مطابقةً لمسار go-block أدناه.
                        // (EN) go expression — build the expression (function/lambda call).
                        //      The current backend runs go-work synchronously (like the
                        //      go-block path below); building the expression already emits and
                        //      performs the call.
                        //      Root fix (0xC0000005): do NOT emit ASYNC_SPAWN on the call's
                        //      *result* register — a void call's result resolves to a null
                        //      pointer, so the backend emits sad_rt_thread_spawn(null, null),
                        //      spawning a real OS thread whose start routine is null/garbage →
                        //      non-deterministic access violation. The work is already done
                        //      synchronously, mirroring the go-block path below.
                        b_.buildExpression(goStmt->expression.get());
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

            bool StatementBuilder::buildStatement_Select(AST::Statement *stmt)
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
                    std::string exitLabel = b_.newLabel("select_exit");
                    auto exitBlock = b_.createBasicBlock(exitLabel);

                    for (size_t i = 0; i < selectStmt->cases.size(); ++i)
                    {
                        auto &selCase = selectStmt->cases[i];

                        // (AR) إنشاء تسميات لجسم الحالة والحالة التالية
                        std::string bodyLabel = b_.newLabel("select_case_" + std::to_string(i));
                        std::string nextLabel = b_.newLabel("select_next_" + std::to_string(i));
                        auto bodyBlock = b_.createBasicBlock(bodyLabel);
                        auto nextBlock = b_.createBasicBlock(nextLabel);

                        // (AR) بناء تعبير القناة (عادةً ق.حاول_استقبل() أو ق.استقبل())
                        // (EN) Build channel expression => check if data available
                        if (selCase->channelExpr)
                        {
                            std::string checkReg = b_.newTempRegister();
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
                                    auto recvResult = b_.buildExpression(selCase->channelExpr.get());
                                    if (!recvResult.registerName.empty())
                                    {
                                        SIRInstruction cmpInst = SIRInstruction::Binary(
                                            SIROpcode::NE,
                                            SIROperand::Register(checkReg, SadTypeKind::Boolean),
                                            SIROperand::Register(recvResult.registerName, recvResult.type),
                                            SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel));
                                        cmpInst.comment = "select: try_recv != null case " + std::to_string(i);
                                        if (b_.currentBlock_)
                                            b_.currentBlock_->addInstruction(cmpInst);
                                        loweredToTryRecvCompare = true;
                                    }
                                }
                            }

                            if (!loweredToTryRecvCompare)
                            {
                                // (AR) المسار العام: تقييم التعبير ثم مقارنة != 0
                                auto chanResult = b_.buildExpression(selCase->channelExpr.get());
                                SIRInstruction cmpInst = SIRInstruction::Binary(
                                    SIROpcode::NE,
                                    SIROperand::Register(checkReg, SadTypeKind::Boolean),
                                    SIROperand::Register(chanResult.registerName, chanResult.type),
                                    SIROperand::ConstantI64(0));
                                cmpInst.comment = "select: check channel case " + std::to_string(i);
                                if (b_.currentBlock_)
                                    b_.currentBlock_->addInstruction(cmpInst);
                            }

                            // (AR) تفرع: إذا جاهزة → الجسم، وإلا → الحالة التالية
                            SIRInstruction brInst = SIRInstruction::BranchCond(
                                SIROperand::Register(checkReg, SadTypeKind::Boolean),
                                SIROperand::Label(bodyLabel),
                                SIROperand::Label(nextLabel));
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(brInst);
                        }
                        else
                        {
                            // (AR) لا يوجد تعبير — تخطي إلى التالي
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(
                                    SIRInstruction::Branch(SIROperand::Label(nextLabel)));
                        }

                        // (AR) كتلة جسم الحالة
                        if (b_.currentFunction_)
                            b_.currentFunction_->addBasicBlock(bodyBlock);
                        b_.currentBlock_ = bodyBlock;

                        // (AR) بناء جسم الحالة
                        for (auto &bodyStmt : selCase->body)
                        {
                            if (bodyStmt)
                                buildStatement(bodyStmt.get());
                        }

                        // (AR) قفز إلى الخروج بعد الجسم
                        if (b_.currentBlock_ && !b_.currentBlock_->getTerminator())
                        {
                            b_.currentBlock_->addInstruction(
                                SIRInstruction::Branch(SIROperand::Label(exitLabel)));
                        }

                        // (AR) كتلة الحالة التالية
                        if (b_.currentFunction_)
                            b_.currentFunction_->addBasicBlock(nextBlock);
                        b_.currentBlock_ = nextBlock;
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
                    if (b_.currentBlock_ && !b_.currentBlock_->getTerminator())
                    {
                        b_.currentBlock_->addInstruction(
                            SIRInstruction::Branch(SIROperand::Label(exitLabel)));
                    }

                    // (AR) كتلة الخروج
                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(exitBlock);
                    b_.currentBlock_ = exitBlock;

                    return true;
                }

                return false;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
