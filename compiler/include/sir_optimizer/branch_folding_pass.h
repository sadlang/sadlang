/**
 * @file branch_folding_pass.h
 * @brief (AR) تمرير طي الفروع — يزيل الفروع المشروطة الثابتة والكتل غير القابلة للوصول
 * @brief (EN) Branch Folding Pass — removes constant conditional branches and unreachable blocks
 *
 * @details
 * (AR) التحسينات المدعومة:
 *      1. BR_COND(true, then, else) → BR(then) — الفرع ثابت صحيح
 *      2. BR_COND(false, then, else) → BR(else) — الفرع ثابت خاطئ
 *      3. إزالة الكتل التي لا يصل إليها أي فرع (dead blocks)
 *      4. طي سلسلة القفزات: BR→BB(BR only)→BB2 يصبح BR→BB2
 *
 * (EN) Supported optimizations:
 *      1. BR_COND(true, then, else) → BR(then)
 *      2. BR_COND(false, then, else) → BR(else)
 *      3. Remove blocks unreachable from entry
 *      4. Thread jumps: BR→BB(BR only)→BB2 becomes BR→BB2
 *
 * @author SadLanguage Compiler Team
 * @date January 2026
 */

#pragma once

#include "pass.h"
#include <unordered_set>

namespace Sad
{
    namespace Compiler
    {
        namespace Optimizer
        {

            class BranchFoldingPass : public OptimizationPass
            {
            public:
                BranchFoldingPass();
                ~BranchFoldingPass();

                bool runOnFunction(SIR::SIRFunction *function) override;

            private:
                // (AR) تبسيط الفروع المشروطة الثابتة / (EN) Simplify constant conditional branches
                bool foldConstantBranches(SIR::SIRFunction *function);

                // (AR) طي سلسلة القفزات / (EN) Thread jump chains
                bool threadJumps(SIR::SIRFunction *function);

                // (AR) إزالة الكتل الميتة / (EN) Remove dead blocks
                bool removeDeadBlocks(SIR::SIRFunction *function);

                // (AR) جمع الكتل القابلة للوصول / (EN) Collect reachable blocks
                void collectReachable(SIR::SIRFunction *function,
                                      std::unordered_set<std::string> &reachable);

                // (AR) البحث عن كتلة بالاسم / (EN) Find block by name
                SIR::SIRBasicBlock *findBlock(SIR::SIRFunction *function, const std::string &name);

                int foldedCount_;
                int removedBlockCount_;
                int threadedCount_;
            };

        } // namespace Optimizer
    } // namespace Compiler
} // namespace Sad
