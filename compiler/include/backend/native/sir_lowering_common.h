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

#include <cstddef>
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

            // (AR) هل التعليمةُ مقارنةُ عوائم؟ (أحدُ المعامِلَين Float.) المقارنةُ موحَّدةٌ (EQ..GE)
            //      والنوعُ في المعامِل. مقارنةُ العوائم تحتاج ucomisd/fcmp + أعلامًا لا-موقَّعةً
            //      + معالجةَ NaN ⇒ **لا تُدمَج في الفرع**: تُخفَّض كقيمةٍ (٠/١) ويختبرها الفرعُ
            //      الاحتياطيّ (test/cbnz). ⇒ findFusedComparison يستثنيها ليخفّضها مسارُ القيمة.
            inline bool isFloatCompare(const sir::SIRInstruction &inst)
            {
                if (!isComparison(inst.opcode) || inst.operands.size() != 2)
                    return false;
                return inst.operands[0].dataType == types::SadTypeKind::Float ||
                       inst.operands[1].dataType == types::SadTypeKind::Float;
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
                // (AR) الدفعة ٨: ثابتٌ منطقيّ ⇒ صحيحٌ ٠/١ (bool يعيش قيمةً ٠/١؛ boolValue لا intValue
                //      لأنّهما في اتّحادٍ). يوحّد معالجةَ الحرفيّاتِ المنطقيّة عبر المعماريّتين.
                if (op.type == sir::SIROperandType::CONSTANT &&
                    op.dataType == types::SadTypeKind::Boolean)
                {
                    out = op.boolValue ? 1 : 0;
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
                // (AR) مقارنةُ العوائم لا تُدمَج (ucomisd/fcmp + NaN) ⇒ تُترَك لمسارِ القيمة.
                if (isComparison(prev.opcode) && !isFloatCompare(prev) &&
                    prev.result && prev.result->name == condName)
                    return &prev;
                return nullptr;
            }

            // (AR) طبقةُ تحليل الحياة/الانسكاب — محايدةٌ للمعماريّة (لا سجلّاتٌ ولا بايتات، فقط
            //      قراءةُ SIR). كانت مكرَّرةً حرفيًّا في مخفّضَي x86 وARM64 لتقرير أيَّ المؤقّتات
            //      تُنسَك حولَ نداءٍ/طباعةٍ يدهسان سجلّاتِ الحوض (caller-saved). النموذجُ ينظّف
            //      الحوضَ لكلّ كتلة ⇒ الحياةُ ذاتُ الصلة محصورةٌ داخل الكتلة. مُسنِدُ «هل متغيّرُ
            //      ذاكرة» (isMem) يُمرَّر لأنّ تمييزَ المؤقّت السجليّ من متغيّر الذاكرة يعتمد على
            //      خريطةِ الخانات الخاصّة بكلّ مخفّض (byte-disp لـx86، فهرسٌ لـARM64).

            // (AR) هل يُقرأ vreg في تعليمةٍ لاحقةٍ (فهرسٌ > from) في الكتلة نفسِها؟ الميتُ لا يُنسَك.
            inline bool usedAfterInBlock(const sir::SIRBasicBlock &block, std::size_t from,
                                         const std::string &vreg)
            {
                for (std::size_t i = from + 1; i < block.instructions.size(); ++i)
                    for (const auto &op : block.instructions[i].operands)
                        if (op.type == sir::SIROperandType::REGISTER && op.name == vreg)
                            return true;
                return false;
            }

            // (AR) هل vreg وسيطٌ سجليٌّ مؤقّتٌ (لا متغيّرَ ذاكرة، بحسب isMem) لهذا النداء؟ وسائطُ
            //      المؤقّتات تُحمَّل من خانات الانسكاب ⇒ يجب نسكُها حتّى لو ماتت بعد النداء.
            template <typename IsMem>
            inline bool isPoolArgOfCall(const sir::SIRInstruction &call, const std::string &vreg,
                                        IsMem isMem)
            {
                for (std::size_t i = 1; i < call.operands.size(); ++i)
                {
                    const auto &a = call.operands[i];
                    if (a.type == sir::SIROperandType::REGISTER && a.name == vreg && !isMem(a.name))
                        return true;
                }
                return false;
            }

            // (AR) هل vreg مؤقّتٌ سجليٌّ (لا متغيّرَ ذاكرة، بحسب isMem) بين معاملات التعليمة؟ (للطباعة.)
            template <typename IsMem>
            inline bool isPoolOperandOf(const sir::SIRInstruction &inst, const std::string &vreg,
                                        IsMem isMem)
            {
                for (const auto &a : inst.operands)
                    if (a.type == sir::SIROperandType::REGISTER && a.name == vreg && !isMem(a.name))
                        return true;
                return false;
            }
        } // namespace common
    } // namespace native
} // namespace sad

#endif // SAD_NATIVE_SIR_LOWERING_COMMON_H
