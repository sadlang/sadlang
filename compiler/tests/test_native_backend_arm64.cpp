// ============================================================================
// (AR) اختبار م٣ — محرّك ترميز AArch64 (fixed32) + كاتب ELF64 الساكن بـe_machine.
//      البوّابةُ التفاضليّة: كلُّ ترميزٍ مُتحقَّقٌ بايتًا ببايت ضدّ مخرَج llvm-mc
//      (مصحِّحٌ مرجعيّ، لا تبعيّة إنتاج). القيمُ المتوقَّعة أدناه هي حرفيًّا مخرَج
//      `llvm-mc-18 --arch=aarch64 --show-encoding` للتعليمات المكافئة.
//      برهانُ «خروج ٤٢»: movz x0,#42 ; movz x8,#93 ; svc #0 ⇒ ELF64 (EM_AARCH64)
//      ساكنٌ يخرج ٤٢ تحت qemu-aarch64-static.
//      يُبرهِن أنّ نفس الخطّ (محرّك عامّ + جدول SoT + كاتب ELF) يخدم عائلةً ثانيةً
//      من صنفٍ مختلف (RISC، عرض ثابت) بلا تعديلٍ في المنطق المشترك — table-driven.
// (EN) m3 test — AArch64 fixed32 encoder + static ELF64 writer (e_machine param).
//      Differential gate: every encoding byte-for-byte checked against llvm-mc.
//      The exit-42 proof wraps the bytes into a static EM_AARCH64 ELF.
// ============================================================================
#include "sad_test.h"
#include "backend/native/arm64_fixed32_encoder.h"
#include "backend/native/elf64_writer.h"
#include "backend/native/generated/arm64_encoding_generated.h" // (AR) الجدول المولَّد من SoT

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

    std::vector<arm64::Operand> ops3(arm64::Operand a, arm64::Operand b, arm64::Operand c)
    {
        return {a, b, c};
    }

    // (AR) يرمّز تعليمةً عبر الجدول المولَّد من SoT (منمنمة عربيّة + صيغة) — يُثبت
    //      أنّ محرّك fixed32 يقرأ بياناتِه من الـYAML لا من كودٍ مضمَّن (table-driven).
    // (EN) encodes via the SoT-generated table — proving the fixed32 encoder reads
    //      its data from YAML, not inline code.
    std::vector<uint8_t> enc(const std::string &mnemonic, const std::string &form,
                             const std::vector<arm64::Operand> &ops)
    {
        const arm64::EncSpec *spec = arm64::lookupEncSpec(mnemonic, form);
        ASSERT_TRUE(spec != nullptr);
        return arm64::encodeFixed32(*spec, ops);
    }
} // namespace

// ─── الترميز التفاضليّ (القيم = مخرَج llvm-mc-18 aarch64 حرفيًّا، little-endian) ───

// movz x0, #42  # encoding: [0x40,0x05,0x80,0xd2]
TEST(NativeArm64, MovzX0_42)
{
    auto b = enc("انقل", "x, imm16", {arm64::Operand::R(arm64::X0), arm64::Operand::I(42)});
    ASSERT_EQ(hex(b), std::string("400580d2"));
}

// movz x8, #93  # encoding: [0xa8,0x0b,0x80,0xd2]   (رقم نداء exit في asm-generic)
TEST(NativeArm64, MovzX8_93)
{
    auto b = enc("انقل", "x, imm16", {arm64::Operand::R(arm64::X8), arm64::Operand::I(93)});
    ASSERT_EQ(hex(b), std::string("a80b80d2"));
}

// svc #0  # encoding: [0x01,0x00,0x00,0xd4]
TEST(NativeArm64, Svc0)
{
    auto b = enc("نداء_نظام", "", {});
    ASSERT_EQ(hex(b), std::string("010000d4"));
}

// add x0, x1, x2  # encoding: [0x20,0x00,0x02,0x8b]   (ثلاثة معاملات: Rd=x0, Rn=x1, Rm=x2)
TEST(NativeArm64, AddX0X1X2)
{
    auto b = enc("اجمع", "x, x, x",
                 ops3(arm64::Operand::R(arm64::X0), arm64::Operand::R(arm64::X1), arm64::Operand::R(arm64::X2)));
    ASSERT_EQ(hex(b), std::string("2000028b"));
}

// ret  # encoding: [0xc0,0x03,0x5f,0xd6]   (RET x30 ضمنيًّا)
TEST(NativeArm64, Ret)
{
    auto b = enc("ارجع", "", {});
    ASSERT_EQ(hex(b), std::string("c0035fd6"));
}

// ─── تسلسل «خروج ٤٢» الكامل ───
TEST(NativeArm64, Exit42Sequence)
{
    std::vector<uint8_t> code;
    auto append = [&](const std::vector<uint8_t> &b) { code.insert(code.end(), b.begin(), b.end()); };
    append(enc("انقل", "x, imm16", {arm64::Operand::R(arm64::X0), arm64::Operand::I(42)})); // exit code
    append(enc("انقل", "x, imm16", {arm64::Operand::R(arm64::X8), arm64::Operand::I(93)})); // NR exit
    append(enc("نداء_نظام", "", {}));
    // (AR) 400580d2 a80b80d2 010000d4
    ASSERT_EQ(hex(code), std::string("400580d2a80b80d2010000d4"));
}

// ─── كاتب ELF64 الساكن بـe_machine=EM_AARCH64 ───
TEST(NativeElfArm64, Exit42HeaderWellFormed)
{
    std::vector<uint8_t> code;
    auto append = [&](const std::vector<uint8_t> &b) { code.insert(code.end(), b.begin(), b.end()); };
    append(enc("انقل", "x, imm16", {arm64::Operand::R(arm64::X0), arm64::Operand::I(42)}));
    append(enc("انقل", "x, imm16", {arm64::Operand::R(arm64::X8), arm64::Operand::I(93)}));
    append(enc("نداء_نظام", "", {}));

    auto bin = elf::writeStaticExec(code, elf::kEmAArch64);

    // (AR) السحر + class=64 + LE
    ASSERT_EQ(bin[0], uint8_t(0x7F));
    ASSERT_EQ(bin[1], uint8_t('E'));
    ASSERT_EQ(bin[2], uint8_t('L'));
    ASSERT_EQ(bin[3], uint8_t('F'));
    ASSERT_EQ(bin[4], uint8_t(2)); // ELFCLASS64
    ASSERT_EQ(bin[5], uint8_t(1)); // ELFDATA2LSB
    // (AR) e_type=ET_EXEC (إزاحة 16، LE)
    ASSERT_EQ(int(bin[16]) | (int(bin[17]) << 8), 2);
    // (AR) e_machine=EM_AARCH64=183 (إزاحة 18) — الفارقُ الوحيد عن x86 في الرأس
    ASSERT_EQ(int(bin[18]) | (int(bin[19]) << 8), 183);
    // (AR) الحجم = رأس + program header + الشيفرة
    ASSERT_EQ(int(bin.size()), int(elf::kCodeOffset) + int(code.size()));
    // (AR) نقطةُ الدخول = 0x400000 + 0x78
    uint64_t entry = 0;
    for (int i = 0; i < 8; ++i) entry |= uint64_t(bin[24 + i]) << (8 * i);
    ASSERT_EQ(uint64_t(entry), uint64_t(elf::kDefaultVBase + elf::kCodeOffset));
}

// (AR) يكتب الثنائيَّ إلى ملفٍّ (لبرهان التشغيل الحيّ تحت qemu-aarch64 خارج الاختبار).
TEST(NativeElfArm64, WritesFile)
{
    std::vector<uint8_t> code;
    auto append = [&](const std::vector<uint8_t> &b) { code.insert(code.end(), b.begin(), b.end()); };
    append(enc("انقل", "x, imm16", {arm64::Operand::R(arm64::X0), arm64::Operand::I(42)}));
    append(enc("انقل", "x, imm16", {arm64::Operand::R(arm64::X8), arm64::Operand::I(93)}));
    append(enc("نداء_نظام", "", {}));
    auto bin = elf::writeStaticExec(code, elf::kEmAArch64);

    std::FILE *fp = std::fopen("sad_m3_arm64_exit42", "wb");
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
