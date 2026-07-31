// ============================================================================
// (AR) محرّك ترميز AArch64 (عائلة fixed32) — م٣ من الخلفيّة السياديّة بلا LLVM.
//      محرّكٌ عامٌّ واحد يبني كلمةَ تعليمةٍ ثابتةَ العرض (٣٢ بت) بحشو حقولٍ بتّيّة
//      يصفها جدول language-truth/backend/arm64/instructions.yaml (encode.fields).
//      كلّ حقلٍ إمّا ثابتٌ (const) أو مأخوذٌ من معاملٍ (from: opN)؛ المنطقُ الضيّق
//      لعائلة RISC (تعبئة بتّيّة + إخراج little-endian) مكتوبٌ مرّةً هنا، والاختلافُ
//      بين التعليمات بياناتٌ (Field/EncSpec) لا كود. header-only ⇒ لا يمسّ المصرّف.
//      الصحّةُ مُتحقَّقة بايتًا ببايت ضدّ llvm-mc في test_native_backend_arm64.cpp.
// (EN) AArch64 fixed32-family encoder (m3 of the sovereign no-LLVM backend). One
//      generic engine packs a fixed-width 32-bit instruction word from the bit
//      fields described by the SoT YAML (encode.fields). Each field is either a
//      constant or drawn from an operand. RISC-specific narrow logic (bitfield
//      packing + little-endian emit) is written once; per-instruction differences
//      are data (Field/EncSpec), not code — mirroring the x86 variable encoder.
// ============================================================================
#ifndef SAD_NATIVE_ARM64_FIXED32_ENCODER_H
#define SAD_NATIVE_ARM64_FIXED32_ENCODER_H

#include <cstdint>
#include <stdexcept>
#include <vector>

namespace sad
{
    namespace native
    {
        namespace arm64
        {
            // (AR) معاملٌ مُرمَّز: سجلّ (رقم 0..31) أو فوريّ (قيمة). لا يحمل عرضًا —
            //      عرضُ الحقل يُحدَّد من مدى بتّاته في المواصفة (hi..lo).
            // (EN) An encoded operand: register (0..31) or immediate. It carries no
            //      width; the field's bit range in the spec fixes the width.
            struct Operand
            {
                enum Kind
                {
                    Reg,
                    Imm
                } kind;
                int reg = 0;       // (AR) رقم السجلّ 0..31 (x0=0 … x30=30، xzr/sp=31)
                long long imm = 0; // (AR) القيمة الفوريّة

                static Operand R(int n) { return Operand{Reg, n, 0}; }
                static Operand I(long long v) { return Operand{Imm, 0, v}; }
            };

            // (AR) وصفُ حقلٍ بتّيّ [lo..hi] شاملًا — يُرآي عنصرًا في encode.fields:
            //      إمّا قيمةٌ ثابتة (is_const) أو مأخوذةٌ من المعامل ذي الفهرس from_op.
            // (EN) A bit field [lo..hi] inclusive — mirrors one encode.fields entry:
            //      either a constant value, or drawn from operand index from_op.
            struct Field
            {
                int hi;             // (AR) أعلى بتّة (شامل)
                int lo;             // (AR) أدنى بتّة (شامل)
                long long konst;    // (AR) القيمة الثابتة (تُستعمل إن is_const)
                bool is_const;      // (AR) true ⇒ ثابت؛ false ⇒ من معامل
                int from_op;        // (AR) فهرس المعامل (يُستعمل إن !is_const؛ وإلّا -1)
            };

            // (AR) مواصفة ترميز صيغةٍ واحدة — تُرآي `encode` (width + fields) في instructions.yaml.
            struct EncSpec
            {
                int width = 32;            // (AR) عرض الكلمة بالبتّات (٣٢ لكلّ AArch64)
                std::vector<Field> fields; // (AR) حقولُ التعليمة
            };

            // (AR) وصولٌ آمنٌ لمعاملٍ بفهرسه — يحرس ضدّ مواصفةٍ تشير لفهرسٍ غير مُمرَّر
            //      (نفسُ تحصين محرّك x86؛ يمنع قراءةً خارج الحدود عند بياناتٍ مغلوطة).
            // (EN) bounds-checked operand access — same hardening as the x86 encoder.
            inline const Operand &opAt(const std::vector<Operand> &ops, int idx)
            {
                if (idx < 0 || static_cast<size_t>(idx) >= ops.size())
                    throw std::out_of_range("EncSpec يشير إلى فهرس معاملٍ غير مُمرَّر / spec references an out-of-range operand index");
                return ops[static_cast<size_t>(idx)];
            }

            // (AR) قيمةُ المعامل التي تدخل الحقل: رقمُ السجلّ إن كان سجلًّا، أو الفوريّ إن كان فوريًّا.
            inline long long operandFieldValue(const Operand &op)
            {
                return op.kind == Operand::Reg ? static_cast<long long>(op.reg) : op.imm;
            }

            // (AR) المحرّك العامّ: يبني كلمةَ ٣٢-بت بحشو الحقول ثمّ يُخرجها little-endian.
            //      كلُّ حقلٍ يُقنَّع بعرضه (hi-lo+1) فلا يطفح على جاره، ثمّ يُزاح إلى lo.
            // (EN) The generic engine: packs a 32-bit word field-by-field, then emits
            //      it little-endian. Each field is masked to its width and shifted to lo.
            inline std::vector<uint8_t> encodeFixed32(const EncSpec &s, const std::vector<Operand> &ops)
            {
                if (s.width != 32)
                    throw std::invalid_argument("محرّك fixed32 لا يدعم إلّا عرض ٣٢ بت / fixed32 encoder supports width=32 only");

                uint32_t word = 0;
                for (const Field &fld : s.fields)
                {
                    if (fld.lo < 0 || fld.hi > 31 || fld.lo > fld.hi)
                        throw std::out_of_range("مدى حقلٍ غير صالح / invalid field bit range");

                    const int fw = fld.hi - fld.lo + 1; // (AR) عرض الحقل بالبتّات (1..32)
                    long long raw = fld.is_const ? fld.konst
                                                 : operandFieldValue(opAt(ops, fld.from_op));

                    // (AR) قناعُ الحقل بعرضه: يمنع تسرّبَ بتّاتٍ عليا إلى الحقول المجاورة.
                    const uint32_t mask = (fw == 32) ? 0xFFFFFFFFu
                                                     : ((1u << fw) - 1u);
                    const uint32_t val = static_cast<uint32_t>(static_cast<uint64_t>(raw));

                    // (AR) حارسٌ ضدّ قيمةٍ تتجاوز عرضَ حقلها: تُقبَل إن وسِعها الحقلُ لا-موقَّعةً
                    //      [0, 2^fw) أو موقَّعةً [−2^(fw−1), 2^(fw−1)) — الأخيرةُ لإزاحات الفروع
                    //      السالبة (imm19/imm26) بمتمّم الاثنين؛ ما عداهما بترٌ صامتٌ يُنتج ترميزًا
                    //      خاطئًا فنرفضه بوضوح. `val & mask` يُنتج المتمّمَ الصحيحَ للسالب.
                    // (EN) accept a value that fits the field either unsigned [0,2^fw) or signed
                    //      [-2^(fw-1),2^(fw-1)) — the latter for negative branch displacements
                    //      (two's complement). Anything else would truncate silently; reject it.
                    if (fw < 32)
                    {
                        const bool fitsUnsigned = (val >> fw) == 0;
                        const long long sgnMin = -(1LL << (fw - 1));
                        const long long sgnMax = (1LL << (fw - 1)) - 1;
                        const bool fitsSigned = fld.is_const ? false : (raw >= sgnMin && raw <= sgnMax);
                        if (!fitsUnsigned && !fitsSigned)
                            throw std::out_of_range("قيمةُ حقلٍ تتجاوز عرضَه / field value exceeds its bit width");
                    }

                    word |= (val & mask) << fld.lo;
                }

                std::vector<uint8_t> out;
                out.reserve(4);
                for (int i = 0; i < 4; ++i)
                    out.push_back(static_cast<uint8_t>((word >> (8 * i)) & 0xFF));
                return out;
            }

            // ================================================================
            // (AR) مواصفاتُ الترميز (EncSpec) ليست مضمَّنةً هنا: تُولَّد من
            //      language-truth/backend/arm64/instructions.yaml إلى الهيدر
            //      arm64_encoding_generated.h (دالّة lookupEncSpec). هذا الملفّ
            //      منطقٌ صرفٌ (encodeFixed32) — تحقيقُ مبدأ «table-driven».
            // (EN) EncSpec data is generated from the SoT YAML into
            //      arm64_encoding_generated.h (lookupEncSpec). This header is pure
            //      logic (encodeFixed32) — the table-driven principle.
            // ================================================================

            // (AR) أرقام سجلّات AArch64 (تُرآي registers.yaml) — لراحة الاستدعاء.
            //      xzr وsp يتشاركان الرقم ٣١ حسب السياق (لا نُدرج إلّا XZR هنا).
            enum Reg
            {
                X0 = 0, X1 = 1, X2 = 2, X3 = 3, X4 = 4, X5 = 5, X6 = 6, X7 = 7,
                X8 = 8, X9 = 9, X10 = 10, X11 = 11, X12 = 12, X13 = 13, X14 = 14, X15 = 15,
                X16 = 16, X17 = 17, X18 = 18, X19 = 19, X20 = 20, X21 = 21, X22 = 22, X23 = 23,
                X24 = 24, X25 = 25, X26 = 26, X27 = 27, X28 = 28, X29 = 29, X30 = 30, XZR = 31
            };
        } // namespace arm64
    } // namespace native
} // namespace sad

#endif // SAD_NATIVE_ARM64_FIXED32_ENCODER_H
