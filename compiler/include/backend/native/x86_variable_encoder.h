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
            // (AR) معاملٌ مُرمَّز: سجلّ (رقم 0..15) أو فوريّ (قيمة + عرض بتّات).
            struct Operand
            {
                enum Kind
                {
                    Reg,
                    Imm
                } kind;
                int reg = 0;             // (AR) رقم السجلّ 0..15 (rax=0 … r15=15)
                long long imm = 0;       // (AR) القيمة الفوريّة
                int bits = 0;            // (AR) عرض الفوريّ (يُستعمل عند الترميز)

                static Operand R(int n) { return Operand{Reg, n, 0, 0}; }
                static Operand I(long long v, int b) { return Operand{Imm, 0, v, b}; }
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

                // (3) ModRM (mod=11 سجلّ مباشر)
                if (s.modrm.present)
                {
                    int regf = s.modrm.reg_ext >= 0 ? s.modrm.reg_ext
                                                    : (opAt(ops, s.modrm.reg_op).reg & 7);
                    int rmf = opAt(ops, s.modrm.rm_op).reg & 7;
                    out.push_back(static_cast<uint8_t>((3 << 6) | ((regf & 7) << 3) | (rmf & 7)));
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
            // (AR) المجموعة الدنيا — تُرآي language-truth/backend/x86_64/instructions.yaml.
            //      (لاحقًا تُقرأ هذه المواصفات من YAML مباشرةً؛ م١ يثبت المحرّك.)
            // ================================================================

            // mov r32, imm32  ⇒  B8+rd id   (بلا REX لسجلّات 0..7)
            inline EncSpec mov_r32_imm32()
            {
                EncSpec s;
                s.opcode = {0xB8};
                s.opcode_reg_add = true;
                s.opcode_reg_op = 0;
                s.imm_op = 1;
                s.imm_bits = 32;
                return s;
            }

            // mov r/m64, r64  ⇒  REX.W 89 /r   (reg=المصدر op1، rm=الوجهة op0)
            inline EncSpec mov_rm64_r64()
            {
                EncSpec s;
                s.rex_w = true;
                s.opcode = {0x89};
                s.modrm.present = true;
                s.modrm.reg_op = 1;
                s.modrm.rm_op = 0;
                return s;
            }

            // add r/m64, r64  ⇒  REX.W 01 /r
            inline EncSpec add_rm64_r64()
            {
                EncSpec s;
                s.rex_w = true;
                s.opcode = {0x01};
                s.modrm.present = true;
                s.modrm.reg_op = 1;
                s.modrm.rm_op = 0;
                return s;
            }

            // syscall  ⇒  0F 05
            inline EncSpec syscall_()
            {
                EncSpec s;
                s.opcode = {0x0F, 0x05};
                return s;
            }

            // ret  ⇒  C3
            inline EncSpec ret_()
            {
                EncSpec s;
                s.opcode = {0xC3};
                return s;
            }

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
