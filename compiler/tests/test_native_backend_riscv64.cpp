// ============================================================================
// (AR) اختبار م٦ — محرّك ترميز RISC-V RV64 (fixed32 بفوريٍّ مُشظّى) + ELF64 بـEM_RISCV.
//      البوّابةُ تفاضليّة: كلُّ ترميزٍ أدناه **مخرَجُ llvm-mc حرفيًّا** لتعليمةٍ مكافئة
//      (`llvm-mc-18 --arch=riscv64 --mattr=+m --show-encoding`) — مصحِّحٌ مرجعيٌّ لا
//      تبعيّةَ إنتاج.
//
//      ⚠️ نصفُ هذه الحالاتِ موجودٌ لسببٍ محدَّد: الطبعةُ الأولى من الاختبار كانت
//      تسعَ حالاتٍ **كلُّها خضراء على محرّكٍ معطوب**. كان حارسُ المدى يقيس الفوريَّ
//      بعرضِ *الشريحة* لا بعرضِ *القيمة*، فيرفض −٢٠ في صيغة B (شريحةُ imm[4:1]
//      خمسُ بتّاتٍ ⇒ مداها [−16,15]) وهو داخلَ مدى الثلاثَ عشرةَ بتًّا. لم يظهر
//      العطبُ لأنّ قيمَ الاختبار كانت أصغرَ من أن تلمس الحدَّ (bne −8 يسع خمسَ
//      بتّاتٍ صدفةً). فحالاتُ الحدود أدناه ليست زينةً: هي ما يجعل الأخضرَ يعني شيئًا.
// (EN) m6 test — RISC-V RV64 encoder (fixed32 with split immediates) + ELF64.
//      Boundary cases are deliberate: the first nine-case version passed on a
//      broken guard that range-checked each *slice* instead of the whole
//      immediate; every value was too small to reach the bound.
// ============================================================================
#include "sad_test.h"
#include "backend/native/riscv64_fixed32_encoder.h"
#include "backend/native/elf64_writer.h"

#include <string>
#include <vector>

using namespace sad::native;

namespace
{
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
} // namespace

// ── صيغةُ I: فوريٌّ متّصلٌ في [31:20] ──────────────────────────────────────
TEST(NativeRiscv64, FormatI)
{
    using namespace riscv64;
    ASSERT_EQ(hex(ADDI(A0, ZERO, 5)), std::string("13055000"));  // li a0, 5
    ASSERT_EQ(hex(SLLI(T0, T0, 3)), std::string("93923200"));    // slli t0, t0, 3
    ASSERT_EQ(hex(ECALL()), std::string("73000000"));            // ecall
}

// ── صيغةُ R: ثلاثةُ سجلّاتٍ (ومنها امتدادُ M للقسمة والباقي) ─────────────────
TEST(NativeRiscv64, FormatR)
{
    using namespace riscv64;
    ASSERT_EQ(hex(REMU(T1, T0, T2)), std::string("33f37202"));   // remu t1, t0, t2
}

// ── صيغةُ U: فوريُّ العشرين العليا ────────────────────────────────────────
TEST(NativeRiscv64, FormatU)
{
    using namespace riscv64;
    ASSERT_EQ(hex(LUI(A1, 1024)), std::string("b7054000"));      // lui a1, 1024
}

// ── صيغةُ S: فوريٌّ **مُشظّى** على حقلين غيرِ متجاورين ──────────────────────
TEST(NativeRiscv64, FormatS_SplitImmediate)
{
    using namespace riscv64;
    ASSERT_EQ(hex(SD(A0, SP, 24)), std::string("233ca100"));     // sd a0, 24(sp)
    ASSERT_EQ(hex(SB(A2, A3, -3)), std::string("a38ec6fe"));     // sb a2, -3(a3)
}

// ── صيغةُ B: فوريٌّ مُشظّى **ومُقلَّبُ البتّات** وبتُّه الأدنى غيرُ مُرمَّز ────────
TEST(NativeRiscv64, FormatB_ScrambledImmediate)
{
    using namespace riscv64;
    ASSERT_EQ(hex(BNE(T0, ZERO, -8)), std::string("e39c02fe"));  // bnez t0, -8
    ASSERT_EQ(hex(BLT(A0, A1, 16)), std::string("6348b500"));    // blt a0, a1, 16
}

// ── حدودُ المدى: الحالاتُ التي كان الحارسُ الخاطئُ يرفضها أو يبتُرها ─────────
TEST(NativeRiscv64, ImmediateRangeBoundaries)
{
    using namespace riscv64;
    // (AR) −٢٠ سليمةٌ في ثلاثَ عشرةَ بتًّا وكانت تُرفَض بقياسِ الشريحةِ الخماسيّة.
    ASSERT_EQ(hex(BNE(T0, ZERO, -20)), std::string("e39602fe"));
    ASSERT_EQ(hex(BEQ(T4, ZERO, 16)), std::string("63880e00"));
    // (AR) طرفا مدى صيغة B (±٢^١٢ بخطوةِ بايتين).
    ASSERT_EQ(hex(BNE(T0, ZERO, -4088)), std::string("63940280"));
    ASSERT_EQ(hex(BLT(A0, A1, 4094)), std::string("e34fb57e"));
    // (AR) طرفا مدى صيغة S (١٢ بتًّا موقَّعة).
    ASSERT_EQ(hex(SD(A0, SP, -2048)), std::string("2330a180"));
    ASSERT_EQ(hex(SB(A2, A3, 2047)), std::string("a38fc67e"));
}

// ── الرفضُ الصريحُ عند تجاوزِ المدى: البترُ الصامتُ يُنتج ترميزًا خاطئًا ────────
TEST(NativeRiscv64, RejectsOutOfRangeImmediate)
{
    using namespace riscv64;
    bool threw = false;
    try
    {
        SD(A0, SP, 2048); // (AR) خارجَ الاثنَي عشرَ بتًّا الموقَّعة بواحد
    }
    catch (const std::out_of_range &)
    {
        threw = true;
    }
    ASSERT_TRUE(threw);

    // (AR) وإزاحةُ فرعٍ فرديّةٌ مستحيلةُ الترميز (البتُّ الأدنى غيرُ مُرمَّز) ⇒ رفضٌ لا تقريب.
    threw = false;
    try
    {
        BNE(T0, ZERO, 3);
    }
    catch (const std::invalid_argument &)
    {
        threw = true;
    }
    ASSERT_TRUE(threw);
}

// ── العنوانُ المطلقُ ٣٢-بت: LUI+ADDI مع تصحيحِ مدِّ الإشارة ─────────────────
TEST(NativeRiscv64, Abs32AddressCorrectsSignExtension)
{
    using namespace riscv64;
    // (AR) 0x400078: البتُّ ١١ من الجزء الأدنى صفرٌ ⇒ لا تصحيح.
    std::vector<uint8_t> a;
    appendAbs32(a, A1, 0x400078u);
    ASSERT_EQ(hex(a), std::string("b7054000") + "93858507"); // lui a1,1024 ; addi a1,a1,120

    // (AR) 0x400800: البتُّ ١١ مرفوعٌ ⇒ ADDI يمدُّ الإشارةَ فيجب رفعُ العشرين العليا
    //      واحدًا. بلا التصحيحِ ينقص العنوانُ 0x1000 — عطبٌ لا يظهر إلّا في التشغيل.
    std::vector<uint8_t> b;
    appendAbs32(b, A1, 0x400800u);
    // lui a1, 0x401 ; addi a1, a1, -2048
    ASSERT_EQ(hex(b), std::string("b7154000") + "93850580"); // lui a1,1025 ; addi a1,a1,-2048
}

// ── ELF64 بـe_machine الخاصّ بـRISC-V (يُرآي abi/riscv64-linux.yaml) ────────
TEST(NativeRiscv64, Elf64MachineIsRiscv)
{
    ASSERT_EQ(static_cast<int>(elf::kEmRiscv64), 243);
    std::vector<uint8_t> code = riscv64::ECALL();
    std::vector<uint8_t> img = elf::writeStaticExec(code, elf::kEmRiscv64);
    ASSERT_TRUE(img.size() > 64);
    ASSERT_EQ(static_cast<int>(img[0]), 0x7F);
    ASSERT_EQ(static_cast<int>(img[1]), 'E');
    // (AR) e_machine كلمةٌ في الإزاحة ١٨ (little-endian).
    ASSERT_EQ(static_cast<int>(img[18]) | (static_cast<int>(img[19]) << 8), 243);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return SAD_RUN_TESTS();
}
