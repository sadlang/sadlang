// ============================================================================
// (AR) أدواتٌ مشتركةٌ لمخفّضات SIR الأصليّة (x86-64 وAArch64، ولاحقًا RISC-V/…): تحليلُ
//      SIR محايدُ المعماريّة (لا بايتاتٌ ولا سجلّات) — تمييزُ المقارنات، إدماجُ المقارنة
//      المُغذِّية لفرعٍ شرطيّ، وقراءةُ الثابت الصحيح. كانت هذه الدوالُّ الثلاثُ مكرَّرةً
//      حرفيًّا في X86SirLowering وArm64SirLowering؛ توحيدُها هنا يمنع الانجرافَ بينهما
//      ويؤسّس للواجهة المشتركة (طورٌ تالٍ: مُصدِرٌ مجرَّد يقود الخطَّ المشترك). النموذجُ
//      «كلٌّ في سجلّ» لكلّ الأهداف يجعل الإدماجَ (المقارنةُ آخرَ تعليمةٍ قبل المُنهي)
//      قاعدةً مشتركة، فلا حاجةَ لـsetcc/movzx (x86) ولا لسجلّ شرطٍ وسيط (ARM64).
// (EN) Shared architecture-neutral SIR analysis for the native lowerers (x86-64,
//      AArch64, and later RISC-V/…): classify comparisons, fuse a comparison that
//      feeds a conditional branch, and read an integer constant. These three were
//      duplicated verbatim in both lowerers; unifying them here prevents drift and
//      seeds the shared interface (a later phase abstracts the emitter).
// ============================================================================
#ifndef SAD_NATIVE_SIR_LOWERING_COMMON_H
#define SAD_NATIVE_SIR_LOWERING_COMMON_H

#include "frontend/sir_instruction.h"
#include "frontend/sir_types.h"

#include <string>

namespace sad
{
    namespace native
    {
        namespace common
        {
            namespace sir = ::Sad::Compiler::SIR;
            namespace types = ::Sad::Types;

            // (AR) هل الأوپكود مقارنةٌ عدديّةٌ موقَّعة؟ (تُدمَج في BR_COND التالي عبر المعماريّات.)
            inline bool isComparison(sir::SIROpcode op)
            {
                using OP = sir::SIROpcode;
                return op == OP::EQ || op == OP::NE || op == OP::LT ||
                       op == OP::LE || op == OP::GT || op == OP::GE;
            }

            // (AR) هل المعاملُ ثابتٌ صحيح؟ يُعيد قيمتَه (الشرطُ نفسُه في كلّ الأهداف).
            inline bool isConstInt(const sir::SIROperand &op, long long &out)
            {
                if (op.type == sir::SIROperandType::CONSTANT &&
                    op.dataType == types::SadTypeKind::Integer)
                {
                    out = op.intValue;
                    return true;
                }
                return false;
            }

            // (AR) يجد مقارنةً في الكتلة نتيجتُها condName تُغذّي BR_COND المُنهيَ لها، شرطَ أن
            //      تكون آخرَ تعليمةٍ قبل المُنهي (نمطُ البانِي المعتاد) — وإلّا لا إدماج. محايدٌ
            //      للمعماريّة: يخفّضها كلُّ هدفٍ بتسلسلِ (مقارنة؛ فرعٌ شرطيّ then؛ فرعٌ else).
            inline const sir::SIRInstruction *findFusedComparison(const sir::SIRBasicBlock &block,
                                                                  const std::string &condName)
            {
                const auto &is = block.instructions;
                if (is.size() < 2)
                    return nullptr;
                const sir::SIRInstruction &prev = is[is.size() - 2];
                if (isComparison(prev.opcode) && prev.result && prev.result->name == condName)
                    return &prev;
                return nullptr;
            }
        } // namespace common
    } // namespace native
} // namespace sad

#endif // SAD_NATIVE_SIR_LOWERING_COMMON_H
