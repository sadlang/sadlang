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

// ─── FP عدديّ مزدوج (float): متحقَّقٌ بايتًا ببايت ضدّ llvm-mc-18 (aarch64). d0/d1 = 0/1. ───
TEST(NativeArm64, FpDoubleOps)
{
    using O = arm64::Operand;
    ASSERT_EQ(hex(enc("عبّئ_عشري", "d, x", {O::R(0), O::R(9)})), std::string("2001679e"));   // fmov d0,x9
    ASSERT_EQ(hex(enc("استخرج_عشري", "x, d", {O::R(9), O::R(0)})), std::string("0900669e"));  // fmov x9,d0
    ASSERT_EQ(hex(enc("حوّل_إلى_عشري", "d, x", {O::R(0), O::R(9)})), std::string("2001629e")); // scvtf d0,x9
    ASSERT_EQ(hex(enc("حوّل_من_عشري", "x, d", {O::R(9), O::R(0)})), std::string("0900789e"));  // fcvtzs x9,d0
    ASSERT_EQ(hex(enc("اجمع_عشري", "d, d, d", ops3(O::R(0), O::R(0), O::R(1)))), std::string("0028611e")); // fadd
    ASSERT_EQ(hex(enc("اطرح_عشري", "d, d, d", ops3(O::R(0), O::R(0), O::R(1)))), std::string("0038611e")); // fsub
    ASSERT_EQ(hex(enc("اضرب_عشري", "d, d, d", ops3(O::R(0), O::R(0), O::R(1)))), std::string("0008611e")); // fmul
    ASSERT_EQ(hex(enc("اقسم_عشري", "d, d, d", ops3(O::R(0), O::R(0), O::R(1)))), std::string("0018611e")); // fdiv
    ASSERT_EQ(hex(enc("قرّب_عشري", "x, d", {O::R(9), O::R(0)})), std::string("0900609e")); // fcvtns x9,d0 (nearest-even)
    ASSERT_EQ(hex(enc("قرّب_عشري", "x, d", {O::R(11), O::R(0)})), std::string("0b00609e")); // fcvtns x11,d0
}

// ret  # encoding: [0xc0,0x03,0x5f,0xd6]   (RET x30 ضمنيًّا)
TEST(NativeArm64, Ret)
{
    auto b = enc("ارجع", "", {});
    ASSERT_EQ(hex(b), std::string("c0035fd6"));
}

// sub x9, x16, x17  # encoding: [0x09,0x02,0x11,0xcb]   (طرحٌ ثلاثيّ)
TEST(NativeArm64, SubX9X16X17)
{
    auto b = enc("اطرح", "x, x, x",
                 ops3(arm64::Operand::R(arm64::X9), arm64::Operand::R(arm64::X16), arm64::Operand::R(arm64::X17)));
    ASSERT_EQ(hex(b), std::string("090211cb"));
}

// mul x9, x16, x17  # encoding: [0x09,0x7e,0x11,0x9b]   (MADD مع Ra=XZR)
TEST(NativeArm64, MulX9X16X17)
{
    auto b = enc("اضرب", "x, x, x",
                 ops3(arm64::Operand::R(arm64::X9), arm64::Operand::R(arm64::X16), arm64::Operand::R(arm64::X17)));
    ASSERT_EQ(hex(b), std::string("097e119b"));
}

// sdiv x9, x16, x17  # encoding: [0x09,0x0e,0xd1,0x9a]   (قسمةٌ صحيحةٌ موقَّعة)
TEST(NativeArm64, SdivX9X16X17)
{
    auto b = enc("اقسم", "x, x, x",
                 ops3(arm64::Operand::R(arm64::X9), arm64::Operand::R(arm64::X16), arm64::Operand::R(arm64::X17)));
    ASSERT_EQ(hex(b), std::string("090ed19a"));
}

// msub x9, x9, x17, x16  # encoding: [0x29,0xc1,0x11,0x9b]   (Ra − Xn×Xm ⇒ للباقي)
TEST(NativeArm64, MsubX9X9X17X16)
{
    auto b = enc("اطرح_الضرب", "x, x, x, x",
                 {arm64::Operand::R(arm64::X9), arm64::Operand::R(arm64::X9),
                  arm64::Operand::R(arm64::X17), arm64::Operand::R(arm64::X16)});
    ASSERT_EQ(hex(b), std::string("29c1119b"));
}

// mov x0, x9 = orr x0, xzr, x9  # encoding: [0xe0,0x03,0x09,0xaa]   (نسخُ سجلّ)
TEST(NativeArm64, MovX0X9)
{
    auto b = enc("انسخ", "x, x", {arm64::Operand::R(arm64::X0), arm64::Operand::R(arm64::X9)});
    ASSERT_EQ(hex(b), std::string("e00309aa"));
}

// cmp x9, x10 = subs xzr,x9,x10  # encoding: [0x3f,0x01,0x0a,0xeb]
TEST(NativeArm64, CmpX9X10)
{
    auto b = enc("قارن", "x, x", {arm64::Operand::R(arm64::X9), arm64::Operand::R(arm64::X10)});
    ASSERT_EQ(hex(b), std::string("3f010aeb"));
}

// b.eq #8 (imm19=2 تعليمتان)  # encoding: [0x40,0x00,0x00,0x54]
TEST(NativeArm64, BeqForward2)
{
    auto b = enc("اقفز_إذا_ساوى", "rel19", {arm64::Operand::I(2)});
    ASSERT_EQ(hex(b), std::string("40000054"));
}

// b.ne #-8 (imm19=-2)  # encoding: [0xc1,0xff,0xff,0x54]   (يُثبت الإزاحة السالبة/التقنيع)
TEST(NativeArm64, BneBackward2)
{
    auto b = enc("اقفز_إذا_لم_يساوِ", "rel19", {arm64::Operand::I(-2)});
    ASSERT_EQ(hex(b), std::string("c1ffff54"));
}

// b #8 (imm26=2)  # encoding: [0x02,0x00,0x00,0x14]
TEST(NativeArm64, BForward2)
{
    auto b = enc("اقفز", "rel26", {arm64::Operand::I(2)});
    ASSERT_EQ(hex(b), std::string("02000014"));
}

// sub sp, sp, #16  # encoding: [0xff,0x43,0x00,0xd1]
TEST(NativeArm64, SubSp16)
{
    auto b = enc("اطرح", "sp, imm12", {arm64::Operand::I(16)});
    ASSERT_EQ(hex(b), std::string("ff4300d1"));
}

// str x9, [sp, #8] (imm12=1 مقيسٌ بـ8)  # encoding: [0xe9,0x07,0x00,0xf9]
TEST(NativeArm64, StrX9Sp8)
{
    auto b = enc("اخزن", "x, sp, imm12", {arm64::Operand::R(arm64::X9), arm64::Operand::I(1)});
    ASSERT_EQ(hex(b), std::string("e90700f9"));
}

// ldr x9, [sp, #8] (imm12=1)  # encoding: [0xe9,0x07,0x40,0xf9]
TEST(NativeArm64, LdrX9Sp8)
{
    auto b = enc("احمل", "x, sp, imm12", {arm64::Operand::R(arm64::X9), arm64::Operand::I(1)});
    ASSERT_EQ(hex(b), std::string("e90740f9"));
}

// ─── التعليمات الجديدة (نداء + طباعة): كلٌّ = مخرَج llvm-mc-18 حرفيًّا ───

// bl #4 (imm26=1، إزاحةُ تعليمةٍ أمامًا)  # encoding: [0x01,0x00,0x00,0x94]
TEST(NativeArm64, Bl4)
{
    auto b = enc("نادِ", "rel26", {arm64::Operand::I(1)});
    ASSERT_EQ(hex(b), std::string("01000094"));
}

// movk x1, #0x1234, lsl #16 (hw=1)  # encoding: [0x81,0x46,0xa2,0xf2]
TEST(NativeArm64, MovkX1Lsl16)
{
    auto b = enc("ثبّت", "x, imm16, lsl",
                 ops3(arm64::Operand::R(arm64::X1), arm64::Operand::I(0x1234), arm64::Operand::I(1)));
    ASSERT_EQ(hex(b), std::string("8146a2f2"));
}

// add x12, sp, #8 (فوريّ، Rn=sp=31)  # encoding: [0xec,0x23,0x00,0x91]
TEST(NativeArm64, AddImmX12Sp8)
{
    auto b = enc("اجمع", "x, x, imm12",
                 ops3(arm64::Operand::R(arm64::X12), arm64::Operand::R(arm64::XZR), arm64::Operand::I(8)));
    ASSERT_EQ(hex(b), std::string("ec230091"));
}

// sub x12, x12, #1 (فوريّ عامّ)  # encoding: [0x8c,0x05,0x00,0xd1]
TEST(NativeArm64, SubImmX12_1)
{
    auto b = enc("اطرح", "x, x, imm12",
                 ops3(arm64::Operand::R(arm64::X12), arm64::Operand::R(arm64::X12), arm64::Operand::I(1)));
    ASSERT_EQ(hex(b), std::string("8c0500d1"));
}

// strb w14, [x12]  # encoding: [0x8e,0x01,0x00,0x39]
TEST(NativeArm64, StrbW14X12)
{
    auto b = enc("اخزن_بايت", "w, x", {arm64::Operand::R(arm64::X14), arm64::Operand::R(arm64::X12)});
    ASSERT_EQ(hex(b), std::string("8e010039"));
}

// cbnz x9, #-8 (imm19=-2، قفزٌ خلفيّ للولب itoa)  # encoding: [0xc9,0xff,0xff,0xb5]
TEST(NativeArm64, CbnzX9Back)
{
    auto b = enc("اقفز_إن_ليس_صفرًا", "x, rel19",
                 {arm64::Operand::R(arm64::X9), arm64::Operand::I(-2)});
    ASSERT_EQ(hex(b), std::string("c9ffffb5"));
}

// ─── العمليّات البتّيّة (القيم = مخرَج llvm-mc-18 حرفيًّا) ───
// and x9, x16, x17  # [0x09,0x02,0x11,0x8a]
TEST(NativeArm64, AndX9X16X17)
{
    auto b = enc("وافق", "x, x, x",
                 ops3(arm64::Operand::R(arm64::X9), arm64::Operand::R(arm64::X16), arm64::Operand::R(arm64::X17)));
    ASSERT_EQ(hex(b), std::string("0902118a"));
}
// orr x9, x16, x17  # [0x09,0x02,0x11,0xaa]
TEST(NativeArm64, OrrX9X16X17)
{
    auto b = enc("اضمم", "x, x, x",
                 ops3(arm64::Operand::R(arm64::X9), arm64::Operand::R(arm64::X16), arm64::Operand::R(arm64::X17)));
    ASSERT_EQ(hex(b), std::string("090211aa"));
}
// eor x9, x16, x17  # [0x09,0x02,0x11,0xca]
TEST(NativeArm64, EorX9X16X17)
{
    auto b = enc("غاير", "x, x, x",
                 ops3(arm64::Operand::R(arm64::X9), arm64::Operand::R(arm64::X16), arm64::Operand::R(arm64::X17)));
    ASSERT_EQ(hex(b), std::string("090211ca"));
}
// mvn x9, x17  # [0xe9,0x03,0x31,0xaa]   (= orn x9,xzr,x17)
TEST(NativeArm64, MvnX9X17)
{
    auto b = enc("اعكس", "x, x", {arm64::Operand::R(arm64::X9), arm64::Operand::R(arm64::X17)});
    ASSERT_EQ(hex(b), std::string("e90331aa"));
}
// lslv x9, x16, x17  # [0x09,0x22,0xd1,0x9a]   ·   lsrv x9, x16, x17  # [0x09,0x26,0xd1,0x9a]
TEST(NativeArm64, LslvLsrv)
{
    ASSERT_EQ(hex(enc("أزح_يسار", "x, x, x",
                 ops3(arm64::Operand::R(arm64::X9), arm64::Operand::R(arm64::X16), arm64::Operand::R(arm64::X17)))),
              std::string("0922d19a"));
    ASSERT_EQ(hex(enc("أزح_يمين", "x, x, x",
                 ops3(arm64::Operand::R(arm64::X9), arm64::Operand::R(arm64::X16), arm64::Operand::R(arm64::X17)))),
              std::string("0926d19a"));
}
// asrv x9, x16, x17  # [0x09,0x2a,0xd1,0x9a]  (إزاحةٌ حسابيّةٌ يمينًا، تمدّ الإشارة)
TEST(NativeArm64, Asrv)
{
    ASSERT_EQ(hex(enc("أزح_يمين_حسابي", "x, x, x",
                 ops3(arm64::Operand::R(arm64::X9), arm64::Operand::R(arm64::X16), arm64::Operand::R(arm64::X17)))),
              std::string("092ad19a"));
}
// ─── مقارنةُ العوائم fcmp Dn,Dm — القيمة = llvm-mc حرفيًّا ───
// fcmp d0,d1 = 0x1E612000 (LE 0020611e) · fcmp d2,d3 = 0x1E632040 (LE 40206 3 1e)
TEST(NativeArm64, Fcmp)
{
    using O = arm64::Operand;
    ASSERT_EQ(hex(enc("قارن_عشري", "d, d", {O::R(0), O::R(1)})), std::string("0020611e"));
    ASSERT_EQ(hex(enc("قارن_عشري", "d, d", {O::R(2), O::R(3)})), std::string("4020631e"));
}

// ─── المقارنةُ كقيمة (cset، حقلُ الشرط المقلوب) — القيم = llvm-mc حرفيًّا ───
// cset x9,eq(field=1)=0x9A9F17E9 · ne(0)=..07 · lt(10)=..a7 · le(12)=..c7 · gt(13)=..d7 · ge(11)=..b7
TEST(NativeArm64, CsetConditions)
{
    ASSERT_EQ(hex(enc("عيّن_إذا", "x, cond", {arm64::Operand::R(arm64::X9), arm64::Operand::I(1)})),  std::string("e9179f9a"));
    ASSERT_EQ(hex(enc("عيّن_إذا", "x, cond", {arm64::Operand::R(arm64::X9), arm64::Operand::I(0)})),  std::string("e9079f9a"));
    ASSERT_EQ(hex(enc("عيّن_إذا", "x, cond", {arm64::Operand::R(arm64::X9), arm64::Operand::I(10)})), std::string("e9a79f9a"));
    ASSERT_EQ(hex(enc("عيّن_إذا", "x, cond", {arm64::Operand::R(arm64::X9), arm64::Operand::I(12)})), std::string("e9c79f9a"));
    ASSERT_EQ(hex(enc("عيّن_إذا", "x, cond", {arm64::Operand::R(arm64::X9), arm64::Operand::I(13)})), std::string("e9d79f9a"));
    ASSERT_EQ(hex(enc("عيّن_إذا", "x, cond", {arm64::Operand::R(arm64::X9), arm64::Operand::I(11)})), std::string("e9b79f9a"));
}

// ─── المصفوفات: str/ldr بقاعدةِ سجلٍّ + add-shifted (index×8) — القيم = llvm-mc حرفيًّا ───
// str x9, [x16, #16] (imm12=2 مقيسٌ بـ8، Rn=x16)  # encoding: [0x09,0x0a,0x00,0xf9]
TEST(NativeArm64, StrBaseX9X16_16)
{
    auto b = enc("اخزن", "x, x, imm12",
                 ops3(arm64::Operand::R(arm64::X9), arm64::Operand::R(arm64::X16), arm64::Operand::I(2)));
    ASSERT_EQ(hex(b), std::string("090a00f9"));
}

// ldr x9, [x16, #16] (imm12=2، Rn=x16)  # encoding: [0x09,0x0a,0x40,0xf9]
TEST(NativeArm64, LdrBaseX9X16_16)
{
    auto b = enc("احمل", "x, x, imm12",
                 ops3(arm64::Operand::R(arm64::X9), arm64::Operand::R(arm64::X16), arm64::Operand::I(2)));
    ASSERT_EQ(hex(b), std::string("090a40f9"));
}

// add x16, x16, x17, lsl #3 (عنونةُ عنصرٍ: data + index×8)  # encoding: [0x10,0x0e,0x11,0x8b]
TEST(NativeArm64, AddLsl3X16X16X17)
{
    auto b = enc("اجمع", "x, x, x, lsl3",
                 ops3(arm64::Operand::R(arm64::X16), arm64::Operand::R(arm64::X16), arm64::Operand::R(arm64::X17)));
    ASSERT_EQ(hex(b), std::string("100e118b"));
}

// b.lo #16 (تخطّي هلعِ حدّ المصفوفة: idx<len لا-موقَّعًا؛ imm19=4)  # encoding: [0x83,0x00,0x00,0x54]
TEST(NativeArm64, BloSkipImm19_4)
{
    auto b = enc("اقفز_إذا_أدنى", "rel19", {arm64::Operand::I(4)});
    ASSERT_EQ(hex(b), std::string("83000054"));
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
