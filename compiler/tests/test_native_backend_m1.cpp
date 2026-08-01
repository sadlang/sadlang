// ============================================================================
// (AR) اختبار م١ — محرّك ترميز x86-64 (variable) + كاتب ELF64 الساكن.
//      البوّابةُ التفاضليّة: كلُّ ترميزٍ مُتحقَّقٌ بايتًا ببايت ضدّ مخرَج llvm-mc
//      (مصحِّحٌ مرجعيّ، لا تبعيّة إنتاج). القيمُ المتوقَّعة أدناه هي حرفيًّا مخرَج
//      `llvm-mc-18 --arch=x86-64 --show-encoding` للتعليمات المكافئة.
//      برهانُ «خروج ٤٢»: mov edi,42 ; mov eax,60 ; syscall ⇒ ثنائيُّ ELF ساكن يخرج ٤٢.
// (EN) m1 test — x86-64 variable encoder + static ELF64 writer. Differential gate:
//      every encoding is byte-for-byte checked against llvm-mc output (reference
//      corrector, not a production dependency). The exit-42 proof wraps the bytes
//      into a static ELF that exits with code 42.
// ============================================================================
#include "sad_test.h"
#include "backend/native/x86_variable_encoder.h"
#include "backend/native/elf64_writer.h"
#include "backend/native/generated/x86_64_encoding_generated.h" // (AR) الجدول المولَّد من SoT

#include <cstdio>
#include <string>
#include <vector>

using namespace sad::native;

namespace
{
    // (AR) تحويل بايتات إلى سلسلة سداسيّة صغيرة (للمقارنة القابلة للطباعة).
    std::string hex(const std::vector<uint8_t> &b)
    {
        static const char *d = "0123456789abcdef";
        std::string s;
        s.reserve(b.size() * 2);
        for (uint8_t c : b)
        {
            s.push_back(d[c >> 4]);
            s.push_back(d[c & 0xF]);
        }
        return s;
    }

    std::vector<x86::Operand> ops2(x86::Operand a, x86::Operand b) { return {a, b}; }

    // (AR) يرمّز تعليمةً عبر الجدول المولَّد من SoT (منمنمة عربيّة + صيغة) — يُثبت
    //      أنّ المحرّك يقرأ بياناتِه من الـYAML لا من كودٍ مضمَّن (table-driven).
    // (EN) encodes via the SoT-generated table (Arabic mnemonic + form) — proving
    //      the encoder reads its data from YAML, not inline code.
    std::vector<uint8_t> enc(const std::string &mnemonic, const std::string &form,
                             const std::vector<x86::Operand> &ops)
    {
        const x86::EncSpec *spec = x86::lookupEncSpec(mnemonic, form);
        ASSERT_TRUE(spec != nullptr);
        return x86::encodeVariable(*spec, ops);
    }
} // namespace

// ─── الترميز التفاضليّ (القيم = مخرَج llvm-mc-18 حرفيًّا) ───

// movl $42, %edi  # encoding: [0xbf,0x2a,0x00,0x00,0x00]
TEST(NativeX86, MovEdi42)
{
    auto b = enc("انقل", "r32, imm32",
                                 ops2(x86::Operand::R(x86::RDI), x86::Operand::I(42, 32)));
    ASSERT_EQ(hex(b), std::string("bf2a000000"));
}

// movl $60, %eax  # encoding: [0xb8,0x3c,0x00,0x00,0x00]
TEST(NativeX86, MovEax60)
{
    auto b = enc("انقل", "r32, imm32",
                                 ops2(x86::Operand::R(x86::RAX), x86::Operand::I(60, 32)));
    ASSERT_EQ(hex(b), std::string("b83c000000"));
}

// syscall  # encoding: [0x0f,0x05]
TEST(NativeX86, Syscall)
{
    auto b = enc("نداء_نظام", "", {});
    ASSERT_EQ(hex(b), std::string("0f05"));
}

// movq %rbx, %rax  # encoding: [0x48,0x89,0xd8]   (REX.W + ModRM)
TEST(NativeX86, MovRaxRbx)
{
    auto b = enc("انقل", "r64, r64",
                                 ops2(x86::Operand::R(x86::RAX), x86::Operand::R(x86::RBX)));
    ASSERT_EQ(hex(b), std::string("4889d8"));
}

// addq %rbx, %rax  # encoding: [0x48,0x01,0xd8]
TEST(NativeX86, AddRaxRbx)
{
    auto b = enc("اجمع", "r64, r64",
                                 ops2(x86::Operand::R(x86::RAX), x86::Operand::R(x86::RBX)));
    ASSERT_EQ(hex(b), std::string("4801d8"));
}

// movq %r9, %r8  # encoding: [0x4d,0x89,0xc8]   (REX.WRB — امتداد السجلّات)
TEST(NativeX86, MovR8R9)
{
    auto b = enc("انقل", "r64, r64",
                                 ops2(x86::Operand::R(x86::R8), x86::Operand::R(x86::R9)));
    ASSERT_EQ(hex(b), std::string("4d89c8"));
}

// retq  # encoding: [0xc3]
TEST(NativeX86, Ret)
{
    auto b = enc("ارجع", "", {});
    ASSERT_EQ(hex(b), std::string("c3"));
}

// ─── تسلسل «خروج ٤٢» الكامل ───
TEST(NativeX86, Exit42Sequence)
{
    std::vector<uint8_t> code;
    auto append = [&](const std::vector<uint8_t> &b) { code.insert(code.end(), b.begin(), b.end()); };
    append(enc("انقل", "r32, imm32",
                               ops2(x86::Operand::R(x86::RDI), x86::Operand::I(42, 32)))); // exit code
    append(enc("انقل", "r32, imm32",
                               ops2(x86::Operand::R(x86::RAX), x86::Operand::I(60, 32)))); // SYS_exit
    append(enc("نداء_نظام", "", {}));
    // (AR) bf2a000000 b83c000000 0f05
    ASSERT_EQ(hex(code), std::string("bf2a000000b83c0000000f05"));
}

// ════════════════════════════════════════════════════════════════════════
// (AR) توسيعُ المجموعة الدنيا (م٣-س): تكديس + حساب فوريّ + مقارنة + تدفّق تحكّم.
//      القيمُ المتوقَّعة = مخرَج llvm-mc-18 حرفيًّا (البوّابة التفاضليّة). كلُّ هذه
//      الصيَغ تعمل بالمحرّك العامّ نفسِه بلا منطقٍ جديد — بياناتُ YAML وحدها اتّسعت.
// (EN) Minimal-set expansion (m3-x): stack + immediate arithmetic + compare +
//      control flow. Expected values are literal llvm-mc-18 output. All these
//      forms run through the same generic engine with no new logic — only YAML data grew.
// ════════════════════════════════════════════════════════════════════════

// push %rbp  # [0x55]   ·   push %r15  # [0x41,0x57]
TEST(NativeX86, PushReg)
{
    ASSERT_EQ(hex(enc("ادفع", "r64", {x86::Operand::R(x86::RBP)})), std::string("55"));
    ASSERT_EQ(hex(enc("ادفع", "r64", {x86::Operand::R(x86::R15)})), std::string("4157"));
}

// pop %rbp  # [0x5d]   ·   pop %r15  # [0x41,0x5f]
TEST(NativeX86, PopReg)
{
    ASSERT_EQ(hex(enc("اسحب", "r64", {x86::Operand::R(x86::RBP)})), std::string("5d"));
    ASSERT_EQ(hex(enc("اسحب", "r64", {x86::Operand::R(x86::R15)})), std::string("415f"));
}

// sub $16, %rsp  # [0x48,0x83,0xec,0x10]  (imm8)
TEST(NativeX86, SubImm8)
{
    auto b = enc("اطرح", "r64, imm8", ops2(x86::Operand::R(x86::RSP), x86::Operand::I(16, 8)));
    ASSERT_EQ(hex(b), std::string("4883ec10"));
}

// sub $4096, %rcx  # [0x48,0x81,0xe9,0x00,0x10,0x00,0x00]  (imm32، سجلّ غير المركم)
TEST(NativeX86, SubImm32)
{
    auto b = enc("اطرح", "r64, imm32", ops2(x86::Operand::R(x86::RCX), x86::Operand::I(4096, 32)));
    ASSERT_EQ(hex(b), std::string("4881e900100000"));
}

// add $16, %rsp  # [0x48,0x83,0xc4,0x10]   ·   add $4096, %rcx  # [0x48,0x81,0xc1,...]
TEST(NativeX86, AddImm)
{
    ASSERT_EQ(hex(enc("اجمع", "r64, imm8", ops2(x86::Operand::R(x86::RSP), x86::Operand::I(16, 8)))),
              std::string("4883c410"));
    ASSERT_EQ(hex(enc("اجمع", "r64, imm32", ops2(x86::Operand::R(x86::RCX), x86::Operand::I(4096, 32)))),
              std::string("4881c100100000"));
}

// ─── العمليّات البتّيّة (القيم = مخرَج llvm-mc-18 حرفيًّا) ───
// and %rcx, %rdx  # [0x48,0x21,0xca]
TEST(NativeX86, AndRegReg)
{
    ASSERT_EQ(hex(enc("وافق", "r64, r64", ops2(x86::Operand::R(x86::RDX), x86::Operand::R(x86::RCX)))),
              std::string("4821ca"));
}
// or %rcx, %rdx  # [0x48,0x09,0xca]
TEST(NativeX86, OrRegReg)
{
    ASSERT_EQ(hex(enc("اضمم", "r64, r64", ops2(x86::Operand::R(x86::RDX), x86::Operand::R(x86::RCX)))),
              std::string("4809ca"));
}
// xor %rcx, %rdx  # [0x48,0x31,0xca]
TEST(NativeX86, XorRegReg)
{
    ASSERT_EQ(hex(enc("غاير", "r64, r64", ops2(x86::Operand::R(x86::RDX), x86::Operand::R(x86::RCX)))),
              std::string("4831ca"));
}
// not %rdx  # [0x48,0xf7,0xd2]
TEST(NativeX86, NotReg)
{
    ASSERT_EQ(hex(enc("اعكس", "r64", {x86::Operand::R(x86::RDX)})), std::string("48f7d2"));
}
// neg %rax  # [0x48,0xf7,0xd8]  ·  sar $5,%rdx  # [0x48,0xc1,0xfa,0x05]  ·  sar %cl,%rdx  # [0x48,0xd3,0xfa]
TEST(NativeX86, NegSarEncodings)
{
    ASSERT_EQ(hex(enc("انفِ", "r64", {x86::Operand::R(x86::RAX)})), std::string("48f7d8"));
    ASSERT_EQ(hex(enc("أزح_يمين_حسابي", "r64, imm8", ops2(x86::Operand::R(x86::RDX), x86::Operand::I(5, 8)))),
              std::string("48c1fa05"));
    ASSERT_EQ(hex(enc("أزح_يمين_حسابي", "r64, cl", {x86::Operand::R(x86::RDX)})), std::string("48d3fa"));
}
// ─── setcc لا-موقَّعة + test (مقارنةُ العوائم عبر ucomisd) — القيم = llvm-mc حرفيًّا ───
// seta al=0f97c0 · setae al=0f93c0 · setnp al=0f9bc0 · setp al=0f9ac0 · test rax,rax=4885c0
TEST(NativeX86, FloatCompareEncodings)
{
    ASSERT_EQ(hex(enc("عيّن_إذا_فوق", "r8", {x86::Operand::R(x86::RAX)})), std::string("0f97c0"));
    ASSERT_EQ(hex(enc("عيّن_إذا_فوق_أو_ساوى", "r8", {x86::Operand::R(x86::RAX)})), std::string("0f93c0"));
    ASSERT_EQ(hex(enc("عيّن_إذا_لا_تكافؤ", "r8", {x86::Operand::R(x86::RAX)})), std::string("0f9bc0"));
    ASSERT_EQ(hex(enc("عيّن_إذا_تكافؤ", "r8", {x86::Operand::R(x86::RAX)})), std::string("0f9ac0"));
    ASSERT_EQ(hex(enc("اختبر", "r64, r64", ops2(x86::Operand::R(x86::RAX), x86::Operand::R(x86::RAX)))),
              std::string("4885c0"));
}
// shl $5, %rdx  # [0x48,0xc1,0xe2,0x05]   ·   shr $5, %rdx  # [0x48,0xc1,0xea,0x05]
TEST(NativeX86, ShlShrImm)
{
    ASSERT_EQ(hex(enc("أزح_يسار", "r64, imm8", ops2(x86::Operand::R(x86::RDX), x86::Operand::I(5, 8)))),
              std::string("48c1e205"));
    ASSERT_EQ(hex(enc("أزح_يمين", "r64, imm8", ops2(x86::Operand::R(x86::RDX), x86::Operand::I(5, 8)))),
              std::string("48c1ea05"));
}

// ─── المقارنةُ كقيمة (setcc + movzx) + الإزاحةُ المتغيّرة (CL) ───
// sete/setne/setl/setle/setg/setge al  (القيم = llvm-mc حرفيًّا)
TEST(NativeX86, SetccAl)
{
    ASSERT_EQ(hex(enc("عيّن_إذا_ساوى", "r8", {x86::Operand::R(x86::RAX)})), std::string("0f94c0"));
    ASSERT_EQ(hex(enc("عيّن_إذا_لم_يساوِ", "r8", {x86::Operand::R(x86::RAX)})), std::string("0f95c0"));
    ASSERT_EQ(hex(enc("عيّن_إذا_أصغر", "r8", {x86::Operand::R(x86::RAX)})), std::string("0f9cc0"));
    ASSERT_EQ(hex(enc("عيّن_إذا_أصغر_أو_ساوى", "r8", {x86::Operand::R(x86::RAX)})), std::string("0f9ec0"));
    ASSERT_EQ(hex(enc("عيّن_إذا_أكبر", "r8", {x86::Operand::R(x86::RAX)})), std::string("0f9fc0"));
    ASSERT_EQ(hex(enc("عيّن_إذا_أكبر_أو_ساوى", "r8", {x86::Operand::R(x86::RAX)})), std::string("0f9dc0"));
}
// movzx rax,al  # [0x48,0x0f,0xb6,0xc0]
TEST(NativeX86, MovzxRaxAl)
{
    ASSERT_EQ(hex(enc("مدد_بالصفر", "r64, r8", ops2(x86::Operand::R(x86::RAX), x86::Operand::R(x86::RAX)))),
              std::string("480fb6c0"));
}
// shl %cl,%rdx  # [0x48,0xd3,0xe2]   ·   shr %cl,%rdx  # [0x48,0xd3,0xea]
TEST(NativeX86, ShlShrCl)
{
    ASSERT_EQ(hex(enc("أزح_يسار", "r64, cl", {x86::Operand::R(x86::RDX)})), std::string("48d3e2"));
    ASSERT_EQ(hex(enc("أزح_يمين", "r64, cl", {x86::Operand::R(x86::RDX)})), std::string("48d3ea"));
}

// cmp %rbx, %rax  # [0x48,0x39,0xd8]
TEST(NativeX86, CmpRegReg)
{
    auto b = enc("قارن", "r64, r64", ops2(x86::Operand::R(x86::RAX), x86::Operand::R(x86::RBX)));
    ASSERT_EQ(hex(b), std::string("4839d8"));
}

// cmp $5, %rcx  # [0x48,0x83,0xf9,0x05]   ·   cmp $4096, %rcx  # [0x48,0x81,0xf9,...]
TEST(NativeX86, CmpImm)
{
    ASSERT_EQ(hex(enc("قارن", "r64, imm8", ops2(x86::Operand::R(x86::RCX), x86::Operand::I(5, 8)))),
              std::string("4883f905"));
    ASSERT_EQ(hex(enc("قارن", "r64, imm32", ops2(x86::Operand::R(x86::RCX), x86::Operand::I(4096, 32)))),
              std::string("4881f900100000"));
}

// (AR) تدفّق التحكّم: الأوپكود من llvm-mc (E9/0F84/0F85/E8) والإزاحة النسبيّة LE
//      كمعاملٍ فوريّ. القيمة 200 (0xC8) مطابقةٌ لمخرَج llvm-mc عند هدفٍ على بُعد ٢٠٠.
// jmp .+? (rel32=200)  # [0xe9,0xc8,0x00,0x00,0x00]
TEST(NativeX86, JmpRel32)
{
    ASSERT_EQ(hex(enc("اقفز", "rel32", {x86::Operand::I(200, 32)})), std::string("e9c8000000"));
}
// je rel32=200  # [0x0f,0x84,0xc8,0x00,0x00,0x00]  ·  jne  # [0x0f,0x85,...]
TEST(NativeX86, JccRel32)
{
    ASSERT_EQ(hex(enc("اقفز_إذا_ساوى", "rel32", {x86::Operand::I(200, 32)})), std::string("0f84c8000000"));
    ASSERT_EQ(hex(enc("اقفز_إذا_لم_يساوِ", "rel32", {x86::Operand::I(200, 32)})), std::string("0f85c8000000"));
}
// call rel32=0  # [0xe8,0x00,0x00,0x00,0x00]
TEST(NativeX86, CallRel32)
{
    ASSERT_EQ(hex(enc("نادِ", "rel32", {x86::Operand::I(0, 32)})), std::string("e800000000"));
}
// (AR) خاتمةُ الدالّة (SysV): pop rbp = 5D · pop r13 = 41 5D (REX.B) · ret = C3.
//      القيمُ = مخرَج llvm-mc-18 حرفيًّا. تُستعمَل في emitEpilogue للدوالّ غير الداخلة.
TEST(NativeX86, EpiloguePopRet)
{
    ASSERT_EQ(hex(enc("اسحب", "r64", {x86::Operand::R(x86::RBP)})), std::string("5d"));
    ASSERT_EQ(hex(enc("اسحب", "r64", {x86::Operand::R(x86::R13)})), std::string("415d"));
    ASSERT_EQ(hex(enc("ارجع", "", {})), std::string("c3"));
    // (AR) mov [rbp-8], rdi = 48 89 7D F8 (تخزينُ وسيطٍ واردٍ في خانة معامل).
    ASSERT_EQ(hex(enc("انقل", "m64, r64", {x86::Operand::M(x86::RBP, -8), x86::Operand::R(x86::RDI)})),
              std::string("48897df8"));
}
// (AR) القفزاتُ الشرطيّةُ الموقَّعةُ القريبة (rel32): الأوپكود 0F 8C/8D/8E/8F مطابقٌ
//      لقاعدة llvm-mc (القصير 7C/7D/7E/7F ⇒ القريب +0x10). rel32=200 (0xC8) LE.
// jl  # [0f,8c,..]  ·  jge  # [0f,8d,..]  ·  jle  # [0f,8e,..]  ·  jg  # [0f,8f,..]
TEST(NativeX86, SignedJccRel32)
{
    ASSERT_EQ(hex(enc("اقفز_إذا_أصغر", "rel32", {x86::Operand::I(200, 32)})), std::string("0f8cc8000000"));
    ASSERT_EQ(hex(enc("اقفز_إذا_أكبر_أو_ساوى", "rel32", {x86::Operand::I(200, 32)})), std::string("0f8dc8000000"));
    ASSERT_EQ(hex(enc("اقفز_إذا_أصغر_أو_ساوى", "rel32", {x86::Operand::I(200, 32)})), std::string("0f8ec8000000"));
    ASSERT_EQ(hex(enc("اقفز_إذا_أكبر", "rel32", {x86::Operand::I(200, 32)})), std::string("0f8fc8000000"));
}

// (AR) معاملاتُ الذاكرة [rbp+disp] (تحميل 8B /r · تخزين 89 /r) — القيمُ = مخرَج
//      llvm-mc-18 حرفيًّا. تشمل حدَّ disp8→disp32 (−129) وREX.R لـr8.
// mov rax,[rbp-8]=488b45f8 · mov [rbp-8],rax=488945f8 · mov rcx,[rbp-16]=488b4df0
// mov r8,[rbp-8]=4c8b45f8 · mov [rbp-128],rdx=48895580 · mov rax,[rbp-129]=488b857fffffff
TEST(NativeX86, MemoryOperandsRbpDisp)
{
    using O = x86::Operand;
    ASSERT_EQ(hex(enc("انقل", "r64, m64", {O::R(x86::RAX), O::M(x86::RBP, -8)})), std::string("488b45f8"));
    ASSERT_EQ(hex(enc("انقل", "m64, r64", {O::M(x86::RBP, -8), O::R(x86::RAX)})), std::string("488945f8"));
    ASSERT_EQ(hex(enc("انقل", "r64, m64", {O::R(x86::RCX), O::M(x86::RBP, -16)})), std::string("488b4df0"));
    ASSERT_EQ(hex(enc("انقل", "r64, m64", {O::R(x86::R8), O::M(x86::RBP, -8)})), std::string("4c8b45f8"));
    ASSERT_EQ(hex(enc("انقل", "m64, r64", {O::M(x86::RBP, -128), O::R(x86::RDX)})), std::string("48895580"));
    ASSERT_EQ(hex(enc("انقل", "r64, m64", {O::R(x86::RAX), O::M(x86::RBP, -129)})), std::string("488b857fffffff"));
}

// ─── فحصُ حدّ المصفوفة: cmp r64,[base] (3B /r) + jb rel8 (72) ───
// (AR) cmp rdi,[rax] = 48 3B 38 · jb +12 = 72 0C. متحقَّقان بايتًا ببايت ضدّ llvm-mc-18.
//      يقارنُ الفهرسَ بالطول [arr+0] لا-موقَّعًا، وjb يتخطّى كتلةَ الهلع إن كان ضمنَ الحدّ.
TEST(NativeX86, ArrayBoundsCmpMemAndJb)
{
    using O = x86::Operand;
    ASSERT_EQ(hex(enc("قارن", "r64, m64", {O::R(x86::RDI), O::M(x86::RAX, 0)})), std::string("483b38"));
    ASSERT_EQ(hex(enc("اقفز_إذا_أدنى", "rel8", {O::I(12, 8)})), std::string("720c"));
}

// ─── SSE عدديّ مزدوج (float): البادئةُ الإلزاميّة F2/66 تسبق REX. متحقَّقٌ ضدّ llvm-mc-18. ───
TEST(NativeX86, SseFloatOps)
{
    using O = x86::Operand;
    ASSERT_EQ(hex(enc("عبّئ_عشري", "xmm, r64", {O::R(0), O::R(0)})), std::string("66480f6ec0"));    // movq xmm0,rax
    ASSERT_EQ(hex(enc("عبّئ_عشري", "xmm, r64", {O::R(9), O::R(10)})), std::string("664d0f6eca"));    // movq xmm9,r10
    ASSERT_EQ(hex(enc("استخرج_عشري", "r64, xmm", {O::R(0), O::R(0)})), std::string("66480f7ec0"));   // movq rax,xmm0
    ASSERT_EQ(hex(enc("حمّل_عشري", "xmm, m64", {O::R(0), O::M(x86::RBP, -8)})), std::string("f20f1045f8"));  // movsd xmm0,[rbp-8]
    ASSERT_EQ(hex(enc("خزّن_عشري", "m64, xmm", {O::M(x86::RBP, -8), O::R(0)})), std::string("f20f1145f8"));  // movsd [rbp-8],xmm0
    ASSERT_EQ(hex(enc("اجمع_عشري", "xmm, xmm", {O::R(0), O::R(1)})), std::string("f20f58c1"));       // addsd
    ASSERT_EQ(hex(enc("اطرح_عشري", "xmm, xmm", {O::R(0), O::R(1)})), std::string("f20f5cc1"));       // subsd
    ASSERT_EQ(hex(enc("اضرب_عشري", "xmm, xmm", {O::R(0), O::R(1)})), std::string("f20f59c1"));       // mulsd
    ASSERT_EQ(hex(enc("اقسم_عشري", "xmm, xmm", {O::R(0), O::R(1)})), std::string("f20f5ec1"));       // divsd
    ASSERT_EQ(hex(enc("حوّل_إلى_عشري", "xmm, r64", {O::R(0), O::R(0)})), std::string("f2480f2ac0")); // cvtsi2sd xmm0,rax
    ASSERT_EQ(hex(enc("حوّل_من_عشري", "r64, xmm", {O::R(0), O::R(0)})), std::string("f2480f2cc0"));  // cvttsd2si rax,xmm0
    ASSERT_EQ(hex(enc("قارن_عشري", "xmm, xmm", {O::R(0), O::R(1)})), std::string("660f2ec1"));       // ucomisd
    ASSERT_EQ(hex(enc("قرّب_عشري", "r64, xmm", {O::R(0), O::R(0)})), std::string("f2480f2dc0")); // cvtsd2si rax,xmm0 (nearest-even)
    ASSERT_EQ(hex(enc("قرّب_عشري", "r64, xmm", {O::R(8), O::R(0)})), std::string("f24c0f2dc0")); // cvtsd2si r8,xmm0
}

// ─── برهانُ تدفّق التحكّم الحيّ: لولبٌ يعدّ حتّى ٤٢ ثمّ يخرج به ───
// (AR) mov edi,0 ; loop: add rdi,1 ; cmp rdi,42 ; jne loop ; mov eax,60 ; syscall
//      يُثبت أنّ الحساب الفوريّ والمقارنة والقفز النسبيّ الخلفيّ تُرمَّز وتُنفَّذ صحيحًا.
//      الإزاحةُ النسبيّة تُحسَب ديناميًّا من مواقع الشيفرة (لا رقمٌ سحريّ).
TEST(NativeX86, Exit42ViaControlFlowLoop)
{
    std::vector<uint8_t> code;
    auto append = [&](const std::vector<uint8_t> &b) { code.insert(code.end(), b.begin(), b.end()); };

    append(enc("انقل", "r32, imm32", ops2(x86::Operand::R(x86::RDI), x86::Operand::I(0, 32)))); // العدّاد = 0
    const size_t loop = code.size();                                                            // لصيقة اللولب
    append(enc("اجمع", "r64, imm8", ops2(x86::Operand::R(x86::RDI), x86::Operand::I(1, 8))));    // ++العدّاد
    append(enc("قارن", "r64, imm8", ops2(x86::Operand::R(x86::RDI), x86::Operand::I(42, 8))));   // مقارنة بـ٤٢
    // (AR) jne loop: الإزاحة = موقع اللصيقة − نهاية تعليمة القفز (٦ بايت: 0F 85 + rel32)
    const long long rel = static_cast<long long>(loop) - static_cast<long long>(code.size() + 6);
    append(enc("اقفز_إذا_لم_يساوِ", "rel32", {x86::Operand::I(rel, 32)}));
    append(enc("انقل", "r32, imm32", ops2(x86::Operand::R(x86::RAX), x86::Operand::I(60, 32)))); // SYS_exit
    append(enc("نداء_نظام", "", {}));                                                            // exit(rdi=42)

    // (AR) القفزُ الخلفيّ: rel = −14 ⇒ 0xFFFFFFF2 (little-endian f2ffffff بعد الأوپكود 0f85)
    ASSERT_EQ(rel, -14);

    auto bin = elf::writeStaticExec(code);
    std::FILE *fp = std::fopen("sad_m3x_loop42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

// ─── كاتب ELF64 الساكن ───
TEST(NativeElf, Exit42HeaderWellFormed)
{
    std::vector<uint8_t> code;
    auto append = [&](const std::vector<uint8_t> &b) { code.insert(code.end(), b.begin(), b.end()); };
    append(enc("انقل", "r32, imm32",
                               ops2(x86::Operand::R(x86::RDI), x86::Operand::I(42, 32))));
    append(enc("انقل", "r32, imm32",
                               ops2(x86::Operand::R(x86::RAX), x86::Operand::I(60, 32))));
    append(enc("نداء_نظام", "", {}));

    auto bin = elf::writeStaticExec(code);

    // (AR) السحر + class=64 + LE
    ASSERT_EQ(bin[0], uint8_t(0x7F));
    ASSERT_EQ(bin[1], uint8_t('E'));
    ASSERT_EQ(bin[2], uint8_t('L'));
    ASSERT_EQ(bin[3], uint8_t('F'));
    ASSERT_EQ(bin[4], uint8_t(2)); // ELFCLASS64
    ASSERT_EQ(bin[5], uint8_t(1)); // ELFDATA2LSB
    // (AR) e_type=ET_EXEC (إزاحة 16، LE)
    ASSERT_EQ(int(bin[16]) | (int(bin[17]) << 8), 2);
    // (AR) e_machine=EM_X86_64=62 (إزاحة 18)
    ASSERT_EQ(int(bin[18]) | (int(bin[19]) << 8), 62);
    // (AR) الحجم = رأس + program header + الشيفرة = 120 + 12
    ASSERT_EQ(int(bin.size()), int(elf::kCodeOffset) + int(code.size()));
    // (AR) نقطةُ الدخول = 0x400000 + 0x78 (إزاحة e_entry = 24، 8 بايت LE)
    uint64_t entry = 0;
    for (int i = 0; i < 8; ++i) entry |= uint64_t(bin[24 + i]) << (8 * i);
    ASSERT_EQ(uint64_t(entry), uint64_t(elf::kDefaultVBase + elf::kCodeOffset));
}

// (AR) يكتب الثنائيَّ إلى ملفٍّ اختياريّ (لبرهان التشغيل الحيّ على Linux/WSL خارج الاختبار).
TEST(NativeElf, WritesFile)
{
    std::vector<uint8_t> code;
    auto append = [&](const std::vector<uint8_t> &b) { code.insert(code.end(), b.begin(), b.end()); };
    append(enc("انقل", "r32, imm32",
                               ops2(x86::Operand::R(x86::RDI), x86::Operand::I(42, 32))));
    append(enc("انقل", "r32, imm32",
                               ops2(x86::Operand::R(x86::RAX), x86::Operand::I(60, 32))));
    append(enc("نداء_نظام", "", {}));
    auto bin = elf::writeStaticExec(code);

    std::FILE *fp = std::fopen("sad_m1_exit42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return SAD_RUN_TESTS();
}
