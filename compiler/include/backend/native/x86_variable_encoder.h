// ============================================================================
// (AR) محرّك ترميز x86-64 (عائلة variable) — م١ من الخلفيّة السياديّة بلا LLVM.
//      محرّكٌ عامٌّ واحد يقرأ مواصفةَ ترميزٍ (تُرآي encode في
//      language-truth/backend/x86_64/instructions.yaml) ويُخرج بايتات. المنطقُ
//      الضيّق لعائلة x86 (بادئة REX + ModMR) مكتوبٌ مرّةً هنا؛ الاختلافُ بين
//      التعليمات بياناتٌ (EncSpec) لا كود. header-only ⇒ لا يمسّ مكتبة المصرّف.
//      الصحّةُ مُتحقَّقة بايتًا ببايت ضدّ llvm-mc في test_native_backend_m1.cpp.
// (EN) x86-64 variable-family encoder (m1 of the sovereign no-LLVM backend). One
//      generic engine reads an encoding spec (mirroring `encode` in the SoT YAML)
//      and emits bytes. The x86-specific narrow logic (REX prefix + ModRM) is
//      written once; per-instruction differences are data (EncSpec), not code.
// ============================================================================
#ifndef SAD_NATIVE_X86_VARIABLE_ENCODER_H
#define SAD_NATIVE_X86_VARIABLE_ENCODER_H

#include <cstdint>
#include <stdexcept>
#include <vector>

namespace sad
{
    namespace native
    {
        namespace x86
        {
            // (AR) معاملٌ مُرمَّز: سجلّ (رقم 0..15)، فوريّ (قيمة + عرض)، أو ذاكرةٌ بقاعدةٍ
            //      وإزاحة [base+disp] (لخانات الإطار [rbp±إزاحة]).
            struct Operand
            {
                enum Kind
                {
                    Reg,
                    Imm,
                    Mem
                } kind;
                int reg = 0;             // (AR) رقم السجلّ (Reg) أو سجلّ القاعدة (Mem) 0..15
                long long imm = 0;       // (AR) القيمة الفوريّة (Imm) أو الإزاحة (Mem)
                int bits = 0;            // (AR) عرض الفوريّ (يُستعمل عند الترميز)

                static Operand R(int n) { return Operand{Reg, n, 0, 0}; }
                static Operand I(long long v, int b) { return Operand{Imm, 0, v, b}; }
                // (AR) [base + disp] — معاملُ ذاكرةٍ بإزاحةٍ عن سجلّ قاعدة.
                static Operand M(int base, long long disp) { return Operand{Mem, base, disp, 0}; }
            };

            // (AR) وصولٌ آمنٌ لمعاملٍ بفهرسه — يحرس ضدّ مواصفةٍ تشير لفهرسٍ غير مُمرَّر
            //      (فخّ توسّعٍ عند قراءة المواصفات من YAML لاحقًا؛ توصية أميليا).
            // (EN) bounds-checked operand access — guards against a spec referencing
            //      an unpassed operand index (a forward-looking hardening for YAML specs).
            inline const Operand &opAt(const std::vector<Operand> &ops, int idx)
            {
                if (idx < 0 || static_cast<size_t>(idx) >= ops.size())
                    throw std::out_of_range("EncSpec يشير إلى فهرس معاملٍ غير مُمرَّر / spec references an out-of-range operand index");
                return ops[static_cast<size_t>(idx)];
            }

            // (AR) وصف حقل ModRM: امتدادُ أوپكود ثابت (reg_ext ‎/0../7‎) أو معاملٌ (reg_op)،
            //      والوجهةُ في rm_op. mod=11 (سجلّ مباشر) في هذه المجموعة الدنيا.
            struct ModRM
            {
                bool present = false;
                int reg_ext = -1; // (AR) ≥0 ⇒ امتداد أوپكود ثابت؛ -1 ⇒ من reg_op
                int reg_op = -1;  // (AR) فهرس المعامل الذي يملأ حقل reg
                int rm_op = -1;   // (AR) فهرس المعامل الذي يملأ حقل rm
            };

            // (AR) مواصفة ترميز صيغةٍ واحدة — تُرآي `encode` في instructions.yaml.
            struct EncSpec
            {
                bool rex_w = false;                // (AR) بادئة REX.W (عمليّة 64-بت)
                std::vector<uint8_t> opcode;       // (AR) بايت/بايتات الأوپكود
                bool opcode_reg_add = false;       // (AR) يُضاف رقمُ السجلّ لآخر بايت أوپكود
                int opcode_reg_op = -1;            // (AR) أيّ معاملٍ يُضاف رقمُه
                ModRM modrm;                       // (AR) وصف ModRM (إن وُجد)
                int imm_op = -1;                   // (AR) فهرس المعامل الفوريّ (-1 ⇒ لا فوريّ)
                int imm_bits = 0;                  // (AR) عرض الفوريّ بالبتّات
            };

            // (AR) المحرّك العامّ: يبني البايتات من المواصفة والمعاملات.
            //      REX = 0100 WRXB — يُصدَر إن W أو أيّ سجلّ ≥ 8 (R من reg، B من rm/الأوپكود).
            // (EN) The generic engine: builds bytes from spec + operands.
            inline std::vector<uint8_t> encodeVariable(const EncSpec &s, const std::vector<Operand> &ops)
            {
                std::vector<uint8_t> out;

                // (1) بادئة REX
                int W = s.rex_w ? 1 : 0;
                int R = 0, X = 0, B = 0;
                if (s.modrm.present)
                {
                    if (s.modrm.reg_op >= 0 && opAt(ops, s.modrm.reg_op).reg >= 8)
                        R = 1;
                    if (s.modrm.rm_op >= 0 && opAt(ops, s.modrm.rm_op).reg >= 8)
                        B = 1;
                }
                if (s.opcode_reg_add && s.opcode_reg_op >= 0 && opAt(ops, s.opcode_reg_op).reg >= 8)
                    B = 1;
                if (W || R || X || B)
                    out.push_back(static_cast<uint8_t>(0x40 | (W << 3) | (R << 2) | (X << 1) | B));

                // (2) الأوپكود (+ رقم السجلّ في آخر بايت إن opcode_reg_add)
                for (size_t i = 0; i < s.opcode.size(); ++i)
                {
                    uint8_t b = s.opcode[i];
                    if (s.opcode_reg_add && i + 1 == s.opcode.size())
                        b = static_cast<uint8_t>(b + (opAt(ops, s.opcode_reg_op).reg & 7));
                    out.push_back(b);
                }

                // (3) ModRM — mod=11 لسجلٍّ مباشر، أو mod=01/10 لمعاملِ ذاكرةٍ [base+disp]
                //     (+ بايت SIB حين القاعدة rsp/r12، وبايتاتُ الإزاحة). للقاعدة rbp/r13
                //     لا يجوز mod=00 (يعني إزاحةً مطلقة/RIP) ⇒ نفرض mod=01 حتّى لإزاحةٍ صفر.
                if (s.modrm.present)
                {
                    int regf = s.modrm.reg_ext >= 0 ? s.modrm.reg_ext
                                                    : (opAt(ops, s.modrm.reg_op).reg & 7);
                    const Operand &rm = opAt(ops, s.modrm.rm_op);
                    if (rm.kind == Operand::Mem)
                    {
                        const int baseLow = rm.reg & 7;
                        const bool needSib = (baseLow == 4); // rsp/r12 ⇒ يلزم SIB
                        const bool baseIsBp = (baseLow == 5); // rbp/r13 ⇒ يمنع mod=00
                        int mod;
                        if (rm.imm == 0 && !baseIsBp)
                            mod = 0;
                        else if (rm.imm >= -128 && rm.imm <= 127)
                            mod = 1;
                        else
                            mod = 2;
                        out.push_back(static_cast<uint8_t>((mod << 6) | ((regf & 7) << 3) | baseLow));
                        if (needSib) // scale=00 · index=100(بلا) · base=rsp(100)
                            out.push_back(static_cast<uint8_t>(0x24));
                        if (mod == 1)
                            out.push_back(static_cast<uint8_t>(rm.imm & 0xFF));
                        else if (mod == 2)
                        {
                            // (AR) حرسٌ دفاعيّ: disp32 موقَّع — إزاحةٌ أوسعُ تُبتَر صامتةً.
                            if (rm.imm < -2147483648LL || rm.imm > 2147483647LL)
                                throw std::out_of_range("إزاحةُ معاملِ ذاكرةٍ خارج مدى disp32 / memory displacement out of disp32 range");
                            for (int i = 0; i < 4; ++i)
                                out.push_back(static_cast<uint8_t>((static_cast<unsigned long long>(rm.imm) >> (8 * i)) & 0xFF));
                        }
                    }
                    else
                    {
                        int rmf = rm.reg & 7;
                        out.push_back(static_cast<uint8_t>((3 << 6) | ((regf & 7) << 3) | (rmf & 7)));
                    }
                }

                // (4) الفوريّ (ترتيب البايتات little-endian)
                if (s.imm_op >= 0)
                {
                    unsigned long long v = static_cast<unsigned long long>(opAt(ops, s.imm_op).imm);
                    for (int i = 0; i < s.imm_bits / 8; ++i)
                        out.push_back(static_cast<uint8_t>((v >> (8 * i)) & 0xFF));
                }

                return out;
            }

            // ================================================================
            // (AR) مواصفاتُ الترميز (EncSpec) لم تعُد مضمَّنةً هنا: تُولَّد من
            //      language-truth/backend/x86_64/instructions.yaml إلى الهيدر
            //      x86_64_encoding_generated.h (دالّة lookupEncSpec). هذا الملفّ
            //      منطقٌ صرفٌ (encodeVariable) — تحقيقُ مبدأ «table-driven».
            // (EN) EncSpec data is no longer inline here; it is generated from the
            //      SoT YAML into x86_64_encoding_generated.h (lookupEncSpec). This
            //      header is pure logic (encodeVariable) — the table-driven principle.
            // ================================================================

            // (AR) أرقام سجلّات x86-64 (تُرآي registers.yaml) — لراحة الاستدعاء.
            enum Reg
            {
                RAX = 0, RCX = 1, RDX = 2, RBX = 3,
                RSP = 4, RBP = 5, RSI = 6, RDI = 7,
                R8 = 8, R9 = 9, R10 = 10, R11 = 11,
                R12 = 12, R13 = 13, R14 = 14, R15 = 15
            };
        } // namespace x86
    } // namespace native
} // namespace sad

#endif // SAD_NATIVE_X86_VARIABLE_ENCODER_H
