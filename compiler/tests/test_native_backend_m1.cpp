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
