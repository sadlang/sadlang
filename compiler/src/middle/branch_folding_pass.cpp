/**
 * @file branch_folding_pass.cpp
 * @brief (AR) تنفيذ تمرير طي الفروع
 * @brief (EN) Branch Folding Pass Implementation
 *
 * @details
 * (AR) يعمل على ثلاث مراحل:
 *      1. طي الفروع المشروطة الثابتة: if(true)→jump مباشر
 *      2. طي سلسلة القفزات: BR→(كتلة فارغة)→الهدف → BR→الهدف
 *      3. إزالة الكتل غير القابلة للوصول بعد الطي
 *
 * (EN) Works in three phases:
 *      1. Fold constant conditional branches
 *      2. Thread jump chains through empty blocks
 *      3. Remove unreachable blocks after folding
 *
 * @author SadLanguage Compiler Team
 * @date January 2026
 */

#include "middle/branch_folding_pass.h"
#include <algorithm>
#include <queue>
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

            BranchFoldingPass::BranchFoldingPass()
                : OptimizationPass("Branch Folding", PassType::TRANSFORMATION), foldedCount_(0), removedBlockCount_(0), threadedCount_(0)
            {
            }

            BranchFoldingPass::~BranchFoldingPass() = default;

            // ============================================================================
            // نقطة الدخول / Main Entry Point
            // ============================================================================

            bool BranchFoldingPass::runOnFunction(SIR::SIRFunction *function)
            {
                if (!function)
                    return false;

                foldedCount_ = 0;
                removedBlockCount_ = 0;
                threadedCount_ = 0;
                bool modified = false;

                // (AR) المرحلة 1: طي الفروع الثابتة
                if (foldConstantBranches(function))
                {
                    modified = true;
                }

                // (AR) المرحلة 2: طي سلسلة القفزات
                if (threadJumps(function))
                {
                    modified = true;
                }

                // (AR) المرحلة 3: إزالة الكتل الميتة
                if (removeDeadBlocks(function))
                {
                    modified = true;
                }

                return modified;
            }

            // ============================================================================
            // (AR) المرحلة 1: طي الفروع المشروطة الثابتة
            // (EN) Phase 1: Fold constant conditional branches
            // ============================================================================

            bool BranchFoldingPass::foldConstantBranches(SIR::SIRFunction *function)
            {
                bool modified = false;

                for (auto &blockPtr : function->basicBlocks)
                {
                    if (!blockPtr || blockPtr->instructions.empty())
                        continue;

                    auto &lastInst = blockPtr->instructions.back();

                    // (AR) BR_COND مع شرط ثابت: BR_COND(const, then_label, else_label)
                    if (lastInst.opcode != SIR::SIROpcode::BR_COND)
                        continue;
                    if (lastInst.operands.size() < 3)
                        continue;

                    const auto &condOp = lastInst.operands[0];

                    // (AR) الشرط ثابت صحيح أو خاطئ؟
                    bool isConstTrue = false;
                    bool isConst = false;

                    if (condOp.type == SIR::SIROperandType::CONSTANT && condOp.dataType == SIR::SadTypeKind::Integer)
                    {
                        isConst = true;
                        isConstTrue = (condOp.intValue != 0);
                    }
                    else if (condOp.type == SIR::SIROperandType::CONSTANT && condOp.dataType == SIR::SadTypeKind::Boolean)
                    {
                        isConst = true;
                        isConstTrue = condOp.boolValue;
                    }

                    if (!isConst)
                        continue;

                    // (AR) حدد الهدف المختار
                    const auto &thenLabel = lastInst.operands[1]; // then block
                    const auto &elseLabel = lastInst.operands[2]; // else block

                    const auto &target = isConstTrue ? thenLabel : elseLabel;

                    // (AR) حوّل BR_COND إلى BR غير مشروط
                    lastInst.opcode = SIR::SIROpcode::BR;
                    lastInst.operands.clear();
                    lastInst.operands.push_back(target);

                    foldedCount_++;
                    modified = true;
                    recordModification();
                }

                return modified;
            }

            // ============================================================================
            // (AR) المرحلة 2: طي سلسلة القفزات
            // (EN) Phase 2: Thread jump chains
            // ============================================================================

            bool BranchFoldingPass::threadJumps(SIR::SIRFunction *function)
            {
                bool modified = false;

                // (AR) حد أقصى للتكرار لمنع الحلقات اللانهائية / (EN) Max iterations to prevent infinite loops
                constexpr int MAX_THREAD_DEPTH = 10;

                for (auto &blockPtr : function->basicBlocks)
                {
                    if (!blockPtr || blockPtr->instructions.empty())
                        continue;

                    auto &lastInst = blockPtr->instructions.back();

                    // (AR) فقط BR غير مشروط / (EN) Only unconditional BR
                    if (lastInst.opcode != SIR::SIROpcode::BR)
                        continue;
                    if (lastInst.operands.empty())
                        continue;
                    if (lastInst.operands[0].type != SIR::SIROperandType::LABEL)
                        continue;

                    // (AR) تتبع سلسلة القفزات
                    std::string targetName = lastInst.operands[0].name;
                    int depth = 0;

                    while (depth < MAX_THREAD_DEPTH)
                    {
                        auto *targetBlock = findBlock(function, targetName);
                        if (!targetBlock)
                            break;

                        // (AR) الكتلة تحتوي فقط BR واحد؟ → واصل السلسلة
                        if (targetBlock->instructions.size() != 1)
                            break;

                        auto &targetInst = targetBlock->instructions[0];
                        if (targetInst.opcode != SIR::SIROpcode::BR)
                            break;
                        if (targetInst.operands.empty())
                            break;
                        if (targetInst.operands[0].type != SIR::SIROperandType::LABEL)
                            break;

                        std::string nextTarget = targetInst.operands[0].name;

                        // (AR) تجنب الحلقات الذاتية
                        if (nextTarget == targetName)
                            break;

                        targetName = nextTarget;
                        depth++;
                    }

                    // (AR) هل تغير الهدف؟
                    if (depth > 0 && targetName != lastInst.operands[0].name)
                    {
                        lastInst.operands[0] = SIR::SIROperand::Label(targetName);
                        threadedCount_++;
                        modified = true;
                        recordModification();
                    }
                }

                return modified;
            }

            // ============================================================================
            // (AR) المرحلة 3: إزالة الكتل الميتة
            // (EN) Phase 3: Remove dead blocks
            // ============================================================================

            bool BranchFoldingPass::removeDeadBlocks(SIR::SIRFunction *function)
            {
                if (function->basicBlocks.empty())
                    return false;

                // (AR) جمع الكتل القابلة للوصول / (EN) Collect reachable blocks
                std::unordered_set<std::string> reachable;
                collectReachable(function, reachable);

                // (AR) إزالة الكتل غير القابلة للوصول
                auto &blocks = function->basicBlocks;
                size_t originalSize = blocks.size();

                blocks.erase(
                    std::remove_if(blocks.begin(), blocks.end(),
                                   [&](const std::shared_ptr<SIR::SIRBasicBlock> &block)
                                   {
                                       if (!block)
                                           return true;
                                       return reachable.find(block->name) == reachable.end();
                                   }),
                    blocks.end());

                removedBlockCount_ = static_cast<int>(originalSize - blocks.size());
                if (removedBlockCount_ > 0)
                {
                    recordModification();
                    return true;
                }
                return false;
            }

            // ============================================================================
            // (AR) جمع الكتل القابلة للوصول (BFS من كتلة الدخول)
            // (EN) Collect reachable blocks (BFS from entry block)
            // ============================================================================

            void BranchFoldingPass::collectReachable(SIR::SIRFunction *function,
                                                     std::unordered_set<std::string> &reachable)
            {
                if (function->basicBlocks.empty())
                    return;

                // (AR) ابدأ من الكتلة الأولى (كتلة الدخول)
                std::queue<std::string> worklist;
                const auto &entryName = function->basicBlocks[0]->name;
                worklist.push(entryName);
                reachable.insert(entryName);

                while (!worklist.empty())
                {
                    std::string current = worklist.front();
                    worklist.pop();

                    auto *block = findBlock(function, current);
                    if (!block || block->instructions.empty())
                        continue;

                    // (AR) فحص آخر تعليمة (المنتهية) لتحديد الخلفاء
                    const auto &lastInst = block->instructions.back();

                    // (AR) جمع أسماء الكتل المستهدفة من المعاملات
                    for (const auto &op : lastInst.operands)
                    {
                        if (op.type == SIR::SIROperandType::LABEL)
                        {
                            if (reachable.find(op.name) == reachable.end())
                            {
                                reachable.insert(op.name);
                                worklist.push(op.name);
                            }
                        }
                    }
                }
            }

            // ============================================================================
            // (AR) البحث عن كتلة بالاسم
            // (EN) Find block by name
            // ============================================================================

            SIR::SIRBasicBlock *BranchFoldingPass::findBlock(SIR::SIRFunction *function,
                                                             const std::string &name)
            {
                for (auto &blockPtr : function->basicBlocks)
                {
                    if (blockPtr && blockPtr->name == name)
                    {
                        return blockPtr.get();
                    }
                }
                return nullptr;
            }

        } // namespace Optimizer
    } // namespace Compiler
} // namespace Sad
