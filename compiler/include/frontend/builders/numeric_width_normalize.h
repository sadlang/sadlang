#pragma once

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) تطبيعُ القيمةِ على العرضِ المُعلَنِ — نسخةٌ واحدةٌ للمعابرِ الثلاثة
// (EN) Normalizing a value to its declared width — one copy for all three crossings
//
// (AR) 🔑 كان الاقتطاعُ مكتوبًا ثلاثَ مرّاتٍ في المُقدِّمة (التهيئةُ، وإعادةُ
//      الإسنادِ، والعائدُ) بقناعٍ مُصلَّبٍ `& 0xFF` لـ«بايت» وحدَه. ولمّا فُتحت
//      «رقم8/16/32» و«طبيعي16/32» احتاجَ كلُّ موضعٍ عرضًا مختلفًا **وتوسيعًا
//      بالإشارةِ** للموقَّع — وثلاثُ نسخٍ من ذلك تتباعدُ يومَ يُصلَحُ أحدُها.
//      فالمنطقُ هنا مرّةً واحدةً، والعرضُ والإشارةُ يُقرآن من الجدولِ المُولَّدِ
//      عن types.yaml لا من اسمِ النوعِ مكتوبًا يدًا.
//
// (EN) 🔑 Truncation was written three times in the frontend (init, reassignment,
//      return) with a hard-coded `& 0xFF` for Byte alone. Opening the sized types
//      made each site need a different width AND sign-extension for signed kinds;
//      three copies of that would drift the day one of them is fixed. The logic
//      lives here once, and width/signedness come from the generated table.
// ═══════════════════════════════════════════════════════════════════════════════

#include "../sir_builder.h"
#include "sad_type_system.h"

#include <string>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            /**
             * @brief (AR) هل يحتاجُ هذا النوعُ تطبيعًا عندَ الخزن؟
             * @brief (EN) Does storing into this kind require normalization?
             *
             * (AR) لا يحتاجُه إلّا عددٌ صحيحٌ مُعلَنُ العرضِ دونَ ٦٤ — فما عرضُه ٦٤
             *      («رقم» و«طبيعي») يملأُ الخانةَ فلا بترَ فيه، والعائمُ لا يُقتطع.
             * (EN) Only a declared sub-64-bit integer kind; 64-bit kinds fill the slot
             *      and floats are never truncated.
             */
            inline bool needsWidthNormalization(SadTypeKind declaredKind)
            {
                const int bits = Sad::Types::sadTypeKindNumericBits(declaredKind);
                return Sad::Types::sadTypeKindIsIntegerNumeric(declaredKind) &&
                       bits != Sad::Types::kSadTypeSizeUnknown && bits > 0 && bits < 64;
            }

            /**
             * @brief (AR) هل هذا النوعُ صحيحٌ زمنَ التشغيلِ فيشملُه التطبيع؟
             * @brief (EN) Is this a runtime-integer kind covered by normalization?
             *
             * (AR) قيمةُ «طبيعي» تحملُ وسمَ Integer زمنَ التشغيلِ («الخيار ب»)،
             *      فيقتطعُها المفسّر — فلا بدَّ أن يقتطعَها المترجمُ أيضًا وإلّا انفرجا.
             *      والعائمُ والنصُّ والمؤشّرُ مُستثناةٌ، مطابقةً لحارسِ المفسّرِ
             *      `getKind() == Integer`.
             * (EN) A UInt64 value carries the Integer tag at runtime (Option B), so the
             *      interpreter truncates it and the compiler must too. Float/String/
             *      Pointer are excluded, mirroring the interpreter's guard.
             */
            inline bool isRuntimeIntegerKind(SadTypeKind kind)
            {
                return Sad::Types::sadTypeKindIsIntegerNumeric(kind);
            }

            /**
             * @brief (AR) يُصدِرُ تطبيعَ سجلٍّ على العرضِ المُعلَنِ ويُرجِعُ اسمَ السجلِّ الناتج
             * @brief (EN) Emits register normalization to the declared width; returns the result register
             *
             * (AR) يُصدِرُ `AND` بقناعِ العرض، ثمّ — للموقَّعِ وحدَه — توسيعًا بالإشارة.
             *      ولا تعليمةَ `sext` في SIR، فالتوسيعُ يُبنى بالمُطابقةِ الحسابيّةِ
             *      المعروفة: ‎(س ⊕ بتّ_الإشارة) − بتّ_الإشارة‎. وبدونِه يُخزَنُ
             *      «رقم8 = ‎-1» موجبًا ٢٥٥ — فالبترُ وحدَه دلالةُ اللا-موقَّعِ لا الموقَّع.
             * (EN) Emits AND with the width mask, then — for signed kinds only —
             *      sign-extension. SIR has no `sext`, so it is built from the standard
             *      identity (x XOR signBit) - signBit. Without it `رقم8 = -1` would be
             *      stored as 255: masking alone is unsigned semantics, not signed.
             *
             * @param builder (AR) البانِي — يُستعمَلُ منه newTempRegister وcurrentBlock_
             * @param sourceRegister (AR) اسمُ السجلِّ المصدر
             * @param sourceKind (AR) نوعُ السجلِّ المصدر
             * @param declaredKind (AR) النوعُ المُعلَنُ للخانةِ المخزونِ فيها
             * @return (AR) اسمُ السجلِّ بعدَ التطبيع (أو المصدرُ نفسُه إن لم يلزم)
             */
            template <typename BuilderT>
            inline std::string emitNormalizeRegisterToDeclaredWidth(
                BuilderT &builder, const std::string &sourceRegister,
                SadTypeKind sourceKind, SadTypeKind declaredKind)
            {
                if (sourceRegister.empty() || !builder.currentBlock_ ||
                    !needsWidthNormalization(declaredKind))
                {
                    return sourceRegister;
                }

                const int bits = Sad::Types::sadTypeKindNumericBits(declaredKind);
                const long long widthMask = static_cast<long long>((1ULL << bits) - 1ULL);

                SIRInstruction andInst(SIROpcode::AND);
                std::string maskedRegister = builder.newTempRegister();
                andInst.result =
                    SIROperand::Register(maskedRegister, SadTypeKind::Integer);
                andInst.operands.push_back(SIROperand::Register(sourceRegister, sourceKind));
                andInst.operands.push_back(SIROperand::ConstantI64(widthMask));
                andInst.comment = "truncate to declared width";
                builder.currentBlock_->addInstruction(andInst);

                if (Sad::Types::sadTypeKindIsUnsignedInteger(declaredKind))
                {
                    return maskedRegister;
                }

                const long long signBit = static_cast<long long>(1ULL << (bits - 1));

                SIRInstruction xorInst(SIROpcode::XOR);
                std::string xorRegister = builder.newTempRegister();
                xorInst.result =
                    SIROperand::Register(xorRegister, SadTypeKind::Integer);
                xorInst.operands.push_back(
                    SIROperand::Register(maskedRegister, SadTypeKind::Integer));
                xorInst.operands.push_back(SIROperand::ConstantI64(signBit));
                xorInst.comment = "sign-extend step 1 of 2";
                builder.currentBlock_->addInstruction(xorInst);

                SIRInstruction subInst(SIROpcode::SUB_I64);
                std::string extendedRegister = builder.newTempRegister();
                subInst.result =
                    SIROperand::Register(extendedRegister, SadTypeKind::Integer);
                subInst.operands.push_back(
                    SIROperand::Register(xorRegister, SadTypeKind::Integer));
                subInst.operands.push_back(SIROperand::ConstantI64(signBit));
                subInst.comment = "sign-extend step 2 of 2";
                builder.currentBlock_->addInstruction(subInst);

                return extendedRegister;
            }
        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
