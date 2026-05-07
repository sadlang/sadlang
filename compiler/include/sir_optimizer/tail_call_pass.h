/**
 * @file tail_call_pass.h
 * @brief (AR) تمرير تحسين الاستدعاء الذيلي — تحويل الاستدعاءات الذيلية الذاتية إلى حلقات
 * @brief (EN) Tail Call Optimization Pass — convert self-tail-calls into loops
 *
 * @details
 * (AR) يكتشف الأنماط حيث تكون آخر عملية قبل RET هي CALL لنفس الدالة،
 *      ويحوّلها إلى قفز (BR) للكتلة الأولى مع تحديث المعاملات.
 *      هذا يمنع تجاوز المكدس في الدوال التعاودية الذيلية.
 *
 * (EN) Detects patterns where the last operation before RET is a CALL to the same
 *      function, and converts to a BR back to entry with updated arguments.
 *      Prevents stack overflow in tail-recursive functions.
 *
 * @author SadLanguage Compiler Team
 * @date January 2026
 */

#pragma once

#include "sir_optimizer/pass.h"
#include <string>
#include <vector>

namespace Sad
{
    namespace Compiler
    {
        namespace Optimizer
        {

            /**
             * @brief (AR) ممرّ تحسين الاستدعاء الذيلي
             * @brief (EN) Tail Call Optimization Pass
             */
            class TailCallPass : public OptimizationPass
            {
            public:
                TailCallPass();
                ~TailCallPass() override;

                /**
                 * @brief (AR) تنفيذ التحسين على الدالة
                 * @brief (EN) Run optimization on a function
                 * @param function الدالة المراد تحسينها
                 * @return true إذا تم تعديل الدالة
                 */
                bool runOnFunction(SIR::SIRFunction *function) override;

            private:
                /**
                 * @brief (AR) يفحص هل التعليمة CALL + RET تشكل استدعاءاً ذيلياً ذاتياً
                 * @brief (EN) Check if CALL + RET form a self-tail-call
                 * @param block الكتلة الحالية
                 * @param callIdx فهرس تعليمة CALL
                 * @param funcName اسم الدالة الحالية
                 * @return true إذا كان استدعاءً ذيلياً ذاتياً
                 */
                bool isSelfTailCall(SIR::SIRBasicBlock *block, size_t callIdx,
                                    const std::string &funcName) const;

                /**
                 * @brief (AR) يحوّل الاستدعاء الذيلي إلى قفز للكتلة الأولى
                 * @brief (EN) Convert tail call to branch back to entry block
                 * @param block الكتلة الحالية
                 * @param callIdx فهرس تعليمة CALL
                 * @param function الدالة الحالية
                 * @return true إذا نجح التحويل
                 */
                bool convertTailCallToLoop(SIR::SIRBasicBlock *block, size_t callIdx,
                                           SIR::SIRFunction *function);

                /**
                 * @brief (AR) عدد التحويلات التي تمت
                 */
                int convertedCount_;
            };

        } // namespace Optimizer
    } // namespace Compiler
} // namespace Sad
