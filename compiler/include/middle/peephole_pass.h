/**
 * @file peephole_pass.h
 * @brief (AR) تمرير التحسين النقطي — يطبّق تبسيطات جبرية محلية على تعليمات SIR
 * @brief (EN) Peephole Optimization Pass — applies local algebraic simplifications to SIR instructions
 *
 * @details
 * (AR) يفحص كل تعليمة ويطبّق قواعد تبسيط بسيطة:
 *      - هوية: x+0→x, x*1→x, x-0→x, x/1→x
 *      - تصفير: x*0→0, x&0→0, x^x→0
 *      - عكس مزدوج: !!x→x, --x→x (سالب السالب)
 *      - تحويلات عكسية: i64→f64→i64→x (إذا لم تتغير القيمة)
 *      - كتل فارغة: BR إلى كتلة تحتوي BR فقط → BR المباشر
 *
 * (EN) Examines each instruction and applies simple simplification rules:
 *      - Identity: x+0→x, x*1→x, x-0→x, x/1→x
 *      - Zero: x*0→0, x&0→0, x^x→0
 *      - Double negation: !!x→x, --x→x
 *      - Round-trip casts: i64→f64→i64 eliminated if lossless
 *      - Empty blocks: BR to block with only BR → direct BR
 *
 * @author SadLanguage Compiler Team
 * @date January 2026
 */

#pragma once

#include "pass.h"

namespace Sad
{
    namespace Compiler
    {
        namespace Optimizer
        {

            class PeepholePass : public OptimizationPass
            {
            public:
                PeepholePass();
                ~PeepholePass();

                bool runOnFunction(SIR::SIRFunction *function) override;

            private:
                // (AR) معالجة كتلة واحدة / (EN) Process single basic block
                bool processBlock(SIR::SIRBasicBlock *block);

                // (AR) قواعد التبسيط / (EN) Simplification rules
                bool tryIdentitySimplification(SIR::SIRInstruction &inst);
                bool tryZeroSimplification(SIR::SIRInstruction &inst);
                bool tryDoubleNegation(SIR::SIRInstruction &inst, SIR::SIRBasicBlock *block);
                bool tryRedundantCast(SIR::SIRInstruction &inst, SIR::SIRBasicBlock *block);

                // (AR) مساعدات / (EN) Helpers
                bool isZeroConstant(const SIR::SIROperand &op) const;
                bool isOneConstant(const SIR::SIROperand &op) const;
                void replaceWithOperand(SIR::SIRInstruction &inst, const SIR::SIROperand &op);
                void replaceWithZero(SIR::SIRInstruction &inst);
                SIR::SIRInstruction *findDefiningInstruction(const std::string &regName, SIR::SIRBasicBlock *block);

                int simplificationCount_;
            };

        } // namespace Optimizer
    } // namespace Compiler
} // namespace Sad
