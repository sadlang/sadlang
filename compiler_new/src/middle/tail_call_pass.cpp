/**
 * @file tail_call_pass.cpp
 * @brief (AR) تنفيذ تمرير تحسين الاستدعاء الذيلي
 * @brief (EN) Tail Call Optimization Pass Implementation
 *
 * @details
 * (AR) يبحث عن نمط CALL(self) + RET في نهاية الكتلة:
 *      1. يفحص كل كتلة أساسية بحثاً عن CALL يليه RET مباشرة
 *      2. يتحقق أن CALL يستدعي نفس الدالة الحالية
 *      3. يتحقق أن نتيجة CALL هي نفسها معامل RET (أو RET_VOID)
 *      4. يستبدل CALL+RET بتحديث المعاملات + BR للكتلة الأولى
 *
 * (EN) Looks for CALL(self) + RET pattern at block end:
 *      1. Scan each basic block for CALL followed directly by RET
 *      2. Verify CALL target is the current function
 *      3. Verify RET uses CALL's result (or is RET_VOID)
 *      4. Replace with parameter update + BR to entry block
 *
 * @author SadLanguage Compiler Team
 * @date January 2026
 */

#include "middle/tail_call_pass.h"
#include <iostream>

namespace Sad
{
    namespace Compiler
    {
        namespace Optimizer
        {

            // ============================================================================
            // البناء / Constructor
            // ============================================================================

            TailCallPass::TailCallPass()
                : OptimizationPass("Tail Call Optimization", PassType::TRANSFORMATION), convertedCount_(0)
            {
            }

            TailCallPass::~TailCallPass() = default;

            // ============================================================================
            // نقطة الدخول / Main Entry Point
            // ============================================================================

            bool TailCallPass::runOnFunction(SIR::SIRFunction *function)
            {
                if (!function)
                    return false;
                if (function->basicBlocks.empty())
                    return false;

                const std::string &funcName = function->getName();
                convertedCount_ = 0;
                bool modified = false;

                // (AR) فحص كل كتلة بحثاً عن أنماط الاستدعاء الذيلي
                for (auto &block : function->basicBlocks)
                {
                    if (!block)
                        continue;
                    auto &insts = block->instructions;
                    if (insts.size() < 2)
                        continue;

                    // (AR) افحص من النهاية: نبحث عن CALL + RET أو CALL + RET_VOID
                    for (size_t i = 0; i + 1 < insts.size(); i++)
                    {
                        if (isSelfTailCall(block.get(), i, funcName))
                        {
                            if (convertTailCallToLoop(block.get(), i, function))
                            {
                                modified = true;
                                convertedCount_++;
                                recordModification();
                                break; // (AR) الكتلة تغيرت، انتقل للكتلة التالية
                            }
                        }
                    }
                }

                return modified;
            }

            // ============================================================================
            // (AR) فحص هل يشكّل CALL+RET استدعاءً ذيلياً ذاتياً
            // (EN) Check if CALL+RET form a self-tail-call
            // ============================================================================

            bool TailCallPass::isSelfTailCall(SIR::SIRBasicBlock *block, size_t callIdx,
                                              const std::string &funcName) const
            {
                auto &insts = block->instructions;
                if (callIdx >= insts.size())
                    return false;

                auto &callInst = insts[callIdx];

                // (AR) يجب أن تكون CALL عادي
                if (callInst.opcode != SIR::SIROpcode::CALL)
                    return false;

                // (AR) يجب أن يستدعي نفس الدالة
                if (callInst.operands.empty())
                    return false;
                if (callInst.operands[0].name != funcName)
                    return false;

                // (AR) التعليمة التالية يجب أن تكون RET أو RET_VOID
                size_t retIdx = callIdx + 1;
                if (retIdx >= insts.size())
                    return false;

                auto &retInst = insts[retIdx];

                if (retInst.opcode == SIR::SIROpcode::RET_VOID)
                {
                    // (AR) CALL ذيلي بدون قيمة مُرجعة — مقبول
                    return true;
                }

                if (retInst.opcode == SIR::SIROpcode::RET)
                {
                    // (AR) RET يجب أن يستخدم نتيجة CALL
                    if (!callInst.hasResult() || !callInst.result)
                        return false;
                    if (retInst.operands.empty())
                        return false;

                    // (AR) تحقق أن RET يُعيد نتيجة CALL مباشرة
                    if (retInst.operands[0].type == SIR::SIROperandType::REGISTER &&
                        retInst.operands[0].name == callInst.result->name)
                    {
                        return true;
                    }
                }

                return false;
            }

            // ============================================================================
            // (AR) تحويل الاستدعاء الذيلي إلى حلقة
            // (EN) Convert tail call to loop (branch back to entry)
            // ============================================================================

            bool TailCallPass::convertTailCallToLoop(SIR::SIRBasicBlock *block,
                                                     size_t callIdx,
                                                     SIR::SIRFunction *function)
            {
                auto &insts = block->instructions;
                if (callIdx + 1 >= insts.size())
                    return false;
                if (function->basicBlocks.empty())
                    return false;

                auto &callInst = insts[callIdx];
                const auto &params = function->parameters;

                // (AR) الكتلة الأولى هي نقطة الدخول
                const std::string &entryLabel = function->basicBlocks[0]->name;

                // (AR) أنشئ تعليمات MOVE لتحديث المعاملات
                std::vector<SIR::SIRInstruction> newInsts;

                // (AR) المعامل الأول في CALL هو اسم الدالة، الباقي وسائط
                for (size_t pi = 0; pi < params.size() && (pi + 1) < callInst.operands.size(); pi++)
                {
                    SIR::SIRInstruction moveInst;
                    moveInst.opcode = SIR::SIROpcode::MOVE;
                    moveInst.result = SIR::SIROperand::Register(params[pi].name, params[pi].type);
                    moveInst.operands.push_back(callInst.operands[pi + 1]);
                    newInsts.push_back(moveInst);
                }

                // (AR) أضف BR للكتلة الأولى (entry)
                SIR::SIRInstruction brInst;
                brInst.opcode = SIR::SIROpcode::BR;
                brInst.operands.push_back(SIR::SIROperand::Label(entryLabel));
                newInsts.push_back(brInst);

                // (AR) احذف CALL + RET واستبدلهما بالتعليمات الجديدة
                insts.erase(insts.begin() + callIdx, insts.begin() + callIdx + 2);
                insts.insert(insts.begin() + callIdx, newInsts.begin(), newInsts.end());

                return true;
            }

        } // namespace Optimizer
    } // namespace Compiler
} // namespace Sad
