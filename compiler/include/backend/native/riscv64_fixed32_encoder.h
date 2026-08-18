// ============================================================================
// (AR) محرّك ترميز RISC-V RV64 (عائلة fixed32) — م٦ من الخلفيّة السياديّة بلا LLVM.
//      يبني كلمةَ تعليمةٍ ثابتةَ العرض (٣٢ بت) بحشو حقولٍ بتّيّة يصفها الجدول
//      language-truth/backend/riscv64/instructions.yaml (encode.fields).
//
//      الفارقُ الوحيدُ عن محرّك arm64: **الفوريُّ المُشظّى**. صيغتا S وB في RISC-V
//      تُوزّعان فوريًّا واحدًا على حقلين غيرِ متجاورين (وB تُقلِّب ترتيبَ بتّاته)، فلا
//      يكفي «خذ قيمةَ المعامل كاملةً». لذلك للحقل هنا شريحةُ بتّاتٍ اختياريّة
//      [op_hi..op_lo] من المعامل — تُرآي `from_bits` في الجدول. غيابُها = القيمةُ كاملةً،
//      وبه يصير هذا المحرّكُ مُعمَّمًا على arm64 أيضًا (توحيدُهما مؤجَّلٌ في ISSUE-130:
//      محرّكُ arm64 متحقَّقٌ بايتًا ببايت ضدّ llvm-mc، فلا يُمَسُّ في هذه الدفعة).
//
//      حارسُ المدى مبنيٌّ على **الفوريِّ كاملًا** لا على شريحته: الشريحةُ مقنَّعةٌ
//      بالبناء فلا تُخفِق، فلو فُحِصت وحدَها لمرَّ الطفحُ صامتًا — وهو بالضبط الصنفُ
//      الذي يجعل الترميزَ خاطئًا والاختبارَ أخضر.
//      header-only ⇒ لا يمسّ مكتبة المصرّف. المضيفُ little-endian (كلّ أهدافنا LE).
// (EN) RISC-V RV64 fixed32 encoder. Same table-driven engine as AArch64 plus
//      split-immediate support (S/B formats scatter one immediate across
//      non-adjacent fields). Range checking is done on the whole immediate, not
//      on a slice — a masked slice can never fail, so slice-only checks would let
//      overflow through silently.
// ============================================================================
#ifndef SAD_NATIVE_RISCV64_FIXED32_ENCODER_H
#define SAD_NATIVE_RISCV64_FIXED32_ENCODER_H

#include <cstdint>
#include <stdexcept>
#include <vector>

namespace sad
{
    namespace native
    {
        namespace riscv64
        {
            // (AR) معاملٌ مُرمَّز: سجلّ (٠..٣١) أو فوريّ. لا يحمل عرضًا — عرضُ الحقل
            //      يُحدَّد من مدى بتّاته في المواصفة.
            struct Operand
            {
                enum Kind
                {
                    Reg,
                    Imm
                } kind;
                int reg = 0;
                long long imm = 0;

                static Operand R(int n) { return Operand{Reg, n, 0}; }
                static Operand I(long long v) { return Operand{Imm, 0, v}; }
            };

            // (AR) وصفُ حقلٍ بتّيّ [lo..hi] شاملًا. إمّا ثابتٌ (is_const) أو من المعامل
            //      from_op. وإن كان op_lo >= 0 فالمأخوذُ شريحةُ بتّاتِ المعامل
            //      [op_hi..op_lo] لا قيمتُه كاملةً (الفوريُّ المُشظّى).
            struct Field
            {
                int hi;
                int lo;
                long long konst;
                bool is_const;
                int from_op;
                int op_hi = -1; // (AR) أعلى بتّة من المعامل (شامل)؛ -1 ⇒ لا تشظية
                int op_lo = -1; // (AR) أدنى بتّة من المعامل (شامل)؛ -1 ⇒ لا تشظية
                // (AR) عرضُ **الفوريِّ كاملًا** (١٢ لصيغة S، ١٣ لصيغة B) — لا عرضُ الشريحة.
                //      كان الحارسُ يقيس بـ(op_hi+1)، وهو أعلى بتّةٍ في *هذه الشريحة* لا في
                //      الفوريّ، فيرفض قيمًا سليمةً: شريحةُ imm[4:1] في B تُقاس بخمسِ بتّاتٍ
                //      فيُرفَض −20 وهو داخلَ مدى الثلاث عشرة. عرضُ الشريحةِ ليس عرضَ القيمة.
                int imm_bits = -1;
                // (AR) هل الفوريُّ **موقَّعٌ** (فيُرفَض ما خرج عن [−2^(w−1), 2^(w−1)))؟
                //      كان القبولُ «موقَّعًا أو لا-موقَّعًا» — سياسةٌ ورثتُها عن محرّك arm64
                //      حيث إزاحاتُ الفروعِ والفوريّاتُ اللا-موقَّعةُ تتشارك الحقلَ نفسَه.
                //      في RISC-V هي **خطأٌ صامت**: 2048 تسع اثنتَي عشرةَ بتًّا لا-موقَّعةً
                //      فتمرّ، ثمّ يقرؤها العتادُ موقَّعةً فتصير −2048 — إزاحةٌ مغايرةٌ
                //      تمامًا بلا أيّ إنذار. وllvm-mc يرفضها صراحةً. فصار للحقل موقفٌ
                //      مُعلَنٌ من الإشارة بدل قبولِ التأويلين معًا.
                bool imm_signed = false;
            };

            // (AR) مواصفة ترميز صيغةٍ واحدة — تُرآي `encode` (width + fields).
            struct EncSpec
            {
                int width = 32;
                std::vector<Field> fields;
            };

            // (AR) وصولٌ آمنٌ لمعاملٍ بفهرسه — كحارسِ محرّكَي x86 وarm64.
            inline const Operand &opAt(const std::vector<Operand> &ops, int idx)
            {
                if (idx < 0 || static_cast<size_t>(idx) >= ops.size())
                    throw std::out_of_range("EncSpec يشير إلى فهرس معاملٍ غير مُمرَّر / spec references an out-of-range operand index");
                return ops[static_cast<size_t>(idx)];
            }

            inline long long operandFieldValue(const Operand &op)
            {
                return op.kind == Operand::Reg ? static_cast<long long>(op.reg) : op.imm;
            }

            // (AR) هل يسع الحقلُ (بعرض fw) القيمةَ raw لا-موقَّعةً أو موقَّعةً؟
            //      يُستدعى على **الفوريِّ كاملًا**؛ انظر ترويسةَ الملفّ.
            inline bool fitsInWidth(long long raw, int fw, bool isSigned)
            {
                if (fw >= 64)
                    return true;
                if (isSigned)
                {
                    const long long sgnMin = -(1LL << (fw - 1));
                    const long long sgnMax = (1LL << (fw - 1)) - 1;
                    return raw >= sgnMin && raw <= sgnMax;
                }
                return (static_cast<unsigned long long>(raw) >> fw) == 0;
            }

            // (AR) المحرّك العامّ: يبني كلمةَ ٣٢-بت بحشو الحقول ثمّ يُخرجها little-endian.
            inline std::vector<uint8_t> encodeFixed32(const EncSpec &s, const std::vector<Operand> &ops)
            {
                if (s.width != 32)
                    throw std::invalid_argument("محرّك fixed32 لا يدعم إلّا عرض ٣٢ بت / fixed32 encoder supports width=32 only");

                uint32_t word = 0;
                for (const Field &fld : s.fields)
                {
                    if (fld.lo < 0 || fld.hi > 31 || fld.lo > fld.hi)
                        throw std::out_of_range("مدى حقلٍ غير صالح / invalid field bit range");

                    const int fw = fld.hi - fld.lo + 1;
                    long long raw = fld.is_const ? fld.konst
                                                 : operandFieldValue(opAt(ops, fld.from_op));

                    long long placed = raw;
                    if (!fld.is_const && fld.op_lo >= 0)
                    {
                        // (AR) شريحةٌ من المعامل: تُقاس صحّةُ المدى على الفوريِّ كاملًا
                        //      **قبل** التقنيع، لأنّ الشريحةَ المقنَّعةَ لا تُخفِق أبدًا.
                        if (fld.op_hi < fld.op_lo || fld.op_hi > 63)
                            throw std::out_of_range("مدى شريحةِ معاملٍ غير صالح / invalid operand slice range");
                        const int sliceW = fld.op_hi - fld.op_lo + 1;
                        if (sliceW != fw)
                            throw std::invalid_argument("عرضُ الشريحةِ لا يطابق عرضَ الحقل / operand slice width != field width");
                        const int immW = fld.imm_bits > 0 ? fld.imm_bits : (fld.op_hi + 1);
                        if (!fitsInWidth(raw, immW, fld.imm_signed))
                            throw std::out_of_range("قيمةُ فوريٍّ تتجاوز عرضَه المُرمَّز / immediate exceeds its encodable width");
                        const unsigned long long u = static_cast<unsigned long long>(raw);
                        placed = static_cast<long long>((u >> fld.op_lo) & ((1ULL << sliceW) - 1ULL));
                    }
                    else if (fw < 32 && !fitsInWidth(raw, fw, fld.imm_signed))
                    {
                        throw std::out_of_range("قيمةُ حقلٍ تتجاوز عرضَه / field value exceeds its bit width");
                    }

                    const uint32_t mask = (fw == 32) ? 0xFFFFFFFFu : ((1u << fw) - 1u);
                    word |= (static_cast<uint32_t>(static_cast<uint64_t>(placed)) & mask) << fld.lo;
                }

                std::vector<uint8_t> out;
                out.reserve(4);
                for (int i = 0; i < 4; ++i)
                    out.push_back(static_cast<uint8_t>((word >> (8 * i)) & 0xFF));
                return out;
            }

            // (AR) أرقام سجلّات RV64 (تُرآي registers.yaml).
            enum Reg
            {
                ZERO = 0, RA = 1, SP = 2, GP = 3, TP = 4,
                T0 = 5, T1 = 6, T2 = 7,
                S0 = 8, S1 = 9,
                A0 = 10, A1 = 11, A2 = 12, A3 = 13, A4 = 14, A5 = 15, A6 = 16, A7 = 17,
                S2 = 18, S3 = 19, S4 = 20, S5 = 21, S6 = 22, S7 = 23,
                S8 = 24, S9 = 25, S10 = 26, S11 = 27,
                T3 = 28, T4 = 29, T5 = 30, T6 = 31
            };

            // ================================================================
            // (AR) مُصدِراتٌ مباشرةٌ للصيغ الخمس. كلٌّ منها يبني EncSpec المُرآيَ
            //      لمدخلِ الجدول ثمّ يُنادي المحرّكَ العامّ — فيبقى منطقُ التعبئة
            //      في موضعٍ واحد، والاختلافُ بيانًا لا كودًا.
            // ================================================================
            namespace detail
            {
                inline Field K(int hi, int lo, long long v) { return Field{hi, lo, v, true, -1, -1, -1, -1, false}; }
                // (AR) حقلٌ من معاملٍ كاملًا. sgn=true للفوريّ الموقَّع (imm12 في I)،
                //      وfalse للسجلّات وللفوريّ اللا-موقَّع (imm20 في U، shamt في SLLI).
                inline Field OP(int hi, int lo, int idx, bool sgn = false)
                {
                    return Field{hi, lo, 0, false, idx, -1, -1, -1, sgn};
                }
                inline Field SL(int hi, int lo, int idx, int ohi, int olo, int immBits, bool sgn = true)
                {
                    return Field{hi, lo, 0, false, idx, ohi, olo, immBits, sgn};
                }
            } // namespace detail

            // (AR) صيغة R: funct7 rs2 rs1 funct3 rd opcode
            inline std::vector<uint8_t> emitR(int funct7, int funct3, int opcode, int rd, int rs1, int rs2)
            {
                using namespace detail;
                EncSpec s{32, {K(31, 25, funct7), OP(24, 20, 2), OP(19, 15, 1),
                               K(14, 12, funct3), OP(11, 7, 0), K(6, 0, opcode)}};
                return encodeFixed32(s, {Operand::R(rd), Operand::R(rs1), Operand::R(rs2)});
            }

            // (AR) صيغة I: imm[11:0] rs1 funct3 rd opcode
            inline std::vector<uint8_t> emitI(int funct3, int opcode, int rd, int rs1, long long imm)
            {
                using namespace detail;
                EncSpec s{32, {OP(31, 20, 2, /*sgn=*/true), OP(19, 15, 1), K(14, 12, funct3),
                               OP(11, 7, 0), K(6, 0, opcode)}};
                return encodeFixed32(s, {Operand::R(rd), Operand::R(rs1), Operand::I(imm)});
            }

            // (AR) صيغة S: imm[11:5] rs2 rs1 funct3 imm[4:0] opcode
            inline std::vector<uint8_t> emitS(int funct3, int opcode, int rs2, int rs1, long long imm)
            {
                using namespace detail;
                EncSpec s{32, {SL(31, 25, 2, 11, 5, 12), OP(24, 20, 0), OP(19, 15, 1),
                               K(14, 12, funct3), SL(11, 7, 2, 4, 0, 12), K(6, 0, opcode)}};
                return encodeFixed32(s, {Operand::R(rs2), Operand::R(rs1), Operand::I(imm)});
            }

            // (AR) صيغة B: imm[12] imm[10:5] rs2 rs1 funct3 imm[4:1] imm[11] opcode.
            //      الإزاحةُ بالبايتات؛ بتُّها الأدنى غيرُ مُرمَّزٍ فيجب أن يكون صفرًا.
            inline std::vector<uint8_t> emitB(int funct3, int rs1, int rs2, long long imm)
            {
                using namespace detail;
                if ((imm & 1LL) != 0)
                    throw std::invalid_argument("إزاحةُ فرعٍ فرديّة — بتُّ الصفرِ غيرُ مُرمَّزٍ في صيغة B / odd branch displacement");
                EncSpec s{32, {SL(31, 31, 2, 12, 12, 13), SL(30, 25, 2, 10, 5, 13), OP(24, 20, 1),
                               OP(19, 15, 0), K(14, 12, funct3), SL(11, 8, 2, 4, 1, 13),
                               SL(7, 7, 2, 11, 11, 13), K(6, 0, 0x63)}};
                return encodeFixed32(s, {Operand::R(rs1), Operand::R(rs2), Operand::I(imm)});
            }

            // (AR) صيغة U: imm[31:12] rd opcode
            inline std::vector<uint8_t> emitU(int opcode, int rd, long long imm20)
            {
                using namespace detail;
                EncSpec s{32, {OP(31, 12, 1), OP(11, 7, 0), K(6, 0, opcode)}};
                return encodeFixed32(s, {Operand::R(rd), Operand::I(imm20)});
            }

            // ── اختصاراتُ التعليمات (تُرآي مداخلَ instructions.yaml اسمًا بترميز) ──
            inline std::vector<uint8_t> ADDI(int rd, int rs1, long long imm) { return emitI(0, 0x13, rd, rs1, imm); }
            inline std::vector<uint8_t> LD(int rd, int rs1, long long imm) { return emitI(3, 0x03, rd, rs1, imm); }
            inline std::vector<uint8_t> SLLI(int rd, int rs1, int shamt)
            {
                using namespace detail;
                EncSpec s{32, {K(31, 26, 0), OP(25, 20, 2), OP(19, 15, 1),
                               K(14, 12, 1), OP(11, 7, 0), K(6, 0, 0x13)}};
                return encodeFixed32(s, {Operand::R(rd), Operand::R(rs1), Operand::I(shamt)});
            }
            inline std::vector<uint8_t> ECALL() { return emitI(0, 0x73, 0, 0, 0); }
            inline std::vector<uint8_t> ADD(int rd, int rs1, int rs2) { return emitR(0x00, 0, 0x33, rd, rs1, rs2); }
            inline std::vector<uint8_t> SUB(int rd, int rs1, int rs2) { return emitR(0x20, 0, 0x33, rd, rs1, rs2); }
            inline std::vector<uint8_t> DIVU(int rd, int rs1, int rs2) { return emitR(0x01, 5, 0x33, rd, rs1, rs2); }
            inline std::vector<uint8_t> REMU(int rd, int rs1, int rs2) { return emitR(0x01, 7, 0x33, rd, rs1, rs2); }
            inline std::vector<uint8_t> MUL(int rd, int rs1, int rs2) { return emitR(0x01, 0, 0x33, rd, rs1, rs2); }
            inline std::vector<uint8_t> LUI(int rd, long long imm20) { return emitU(0x37, rd, imm20); }
            inline std::vector<uint8_t> SD(int rs2, int rs1, long long imm) { return emitS(3, 0x23, rs2, rs1, imm); }
            inline std::vector<uint8_t> SB(int rs2, int rs1, long long imm) { return emitS(0, 0x23, rs2, rs1, imm); }
            inline std::vector<uint8_t> BEQ(int rs1, int rs2, long long imm) { return emitB(0, rs1, rs2, imm); }
            inline std::vector<uint8_t> BNE(int rs1, int rs2, long long imm) { return emitB(1, rs1, rs2, imm); }
            inline std::vector<uint8_t> BLT(int rs1, int rs2, long long imm) { return emitB(4, rs1, rs2, imm); }
            inline std::vector<uint8_t> BGE(int rs1, int rs2, long long imm) { return emitB(5, rs1, rs2, imm); }
            inline std::vector<uint8_t> BLTU(int rs1, int rs2, long long imm) { return emitB(6, rs1, rs2, imm); }
            inline std::vector<uint8_t> BGEU(int rs1, int rs2, long long imm) { return emitB(7, rs1, rs2, imm); }

            // (AR) تحميلُ عنوانٍ مطلقٍ ٣٢-بت: LUI بالعشرين العليا **مصحَّحةً** ثمّ ADDI
            //      بالاثنتَي عشرة الدنيا موقَّعةً. التصحيحُ (+0x800) لازمٌ لأنّ ADDI يمدّ
            //      الإشارةَ: إغفالُه يُنقص العنوانَ 0x1000 كلّما كان بتُّ 11 مرفوعًا.
            inline void appendAbs32(std::vector<uint8_t> &out, int rd, uint64_t addr)
            {
                // (AR) ع-٧ (مراجعةُ أميليا): كان العنوانُ يُبتَر إلى ٣٢ بتًّا صامتًا، والتقنيعُ
                //      `hi & 0xFFFFF` يُبطل حارسَ مدى LUI فلا يُخفِق أبدًا. آمنٌ عند
                //      vbase=0x400000 الحاليّ، وهو عينُ «التقنيعُ يُخفي الطفح» الذي يُدينه
                //      هذا الملفّ في سياقٍ آخر. فليُخفِقْ صراحةً بدل أن يُصدِرَ عنوانًا كاذبًا.
                if ((addr >> 32) != 0)
                    throw std::out_of_range("riscv64 appendAbs32: address exceeds 32 bits");
                const uint32_t a = static_cast<uint32_t>(addr);
                const uint32_t hi = (a + 0x800u) >> 12;
                const int32_t lo = static_cast<int32_t>(a) - static_cast<int32_t>(hi << 12);
                // (AR) hi ≤ 0xFFFFF بالبناء (a ≤ 0xFFFFFFFF ⇒ (a+0x800)>>12 ≤ 0x100000)،
                //      والحدُّ الأعلى وحدَه يستلزم التقنيع؛ ما دونَه يمرُّ بحارسِ LUI كاملًا.
                const std::vector<uint8_t> u = LUI(rd, hi & 0xFFFFFu);
                out.insert(out.end(), u.begin(), u.end());
                const std::vector<uint8_t> i = ADDI(rd, rd, lo);
                out.insert(out.end(), i.begin(), i.end());
            }
        } // namespace riscv64
    } // namespace native
} // namespace sad

#endif // SAD_NATIVE_RISCV64_FIXED32_ENCODER_H
