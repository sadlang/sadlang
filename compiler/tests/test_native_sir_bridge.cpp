// ============================================================================
// (AR) اختبار جسر SIR→x86-64 أصليّ — أوّل ترجمة «ص → شيفرة آلة» حقيقيّة بلا LLVM.
//      يبني SIR من مصدر ص فعليّ (Lexer→Parser→SIRBuilder في الوضع الحرّ)، يخفّضه
//      إلى بايتات x86-64 عبر المحرّك الجدوليّ، ويلفّه في ELF64 ساكن. الاختبارُ
//      يؤكّد نجاحَ التخفيض وسلامةَ الـELF، ويكتب الثنائيَّ لبرهانِ تشغيلٍ حيٍّ
//      (خروج ٤٢) تحت Linux/WSL عبر scripts/native_backend/prove_sir_bridge.sh.
// (EN) SIR→native x86-64 bridge test — the first real "S-lang → machine code"
//      compile with no LLVM. Builds SIR from actual ص source, lowers it to bytes,
//      wraps in a static ELF, asserts success + well-formedness, and writes the
//      binary for a live exit-42 proof under Linux/WSL.
// ============================================================================
#include "sad_test.h"

#include "lexer_core.h"
#include "parser_core.h"
#include "frontend/sir_builder.h"
#include "frontend/sir_module.h"

#include "backend/native/sir_native_lowering.h"

#include <cstdio>
#include <memory>
#include <string>

namespace
{
    // (AR) يبني SIRModule من مصدر ص في الوضع الحرّ (يُسقط سِقالةَ defer/الاستثناء).
    std::shared_ptr<Sad::Compiler::SIR::SIRModule> buildSir(const std::string &src)
    {
        Sad::Lexer::LexerCore lexer(src);
        Sad::Parser::ParserCore parser(lexer);
        auto ast = parser.parseProgram();
        Sad::Compiler::SIR::SIRBuilder builder;
        builder.setModuleMode(true);
        builder.setFreestanding(true);
        return builder.buildModule(&ast);
    }

    // دالة رئيسية()\n    ارجع 40 + 2\n نهاية
    const std::string kSrcAdd =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9()\n"
        "    \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 40 + 2\n"
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n";

    // دالة رئيسية()\n    ارجع 42\n نهاية
    const std::string kSrcConst =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9()\n"
        "    \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 42\n"
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n";
} // namespace

// (AR) المصدرُ الحسابيّ يُبنى ويُخفَّض بنجاح، والـELF سليمٌ (EM_X86_64).
TEST(NativeSirBridge, LowersAdditionReturn)
{
    auto module = buildSir(kSrcAdd);
    ASSERT_TRUE(module != nullptr);

    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);

    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    // (AR) سحرُ ELF + class64 + LE + e_machine=EM_X86_64=62
    ASSERT_EQ(bin[0], uint8_t(0x7F));
    ASSERT_EQ(bin[4], uint8_t(2));
    ASSERT_EQ(int(bin[18]) | (int(bin[19]) << 8), 62);
    // (AR) الشيفرةُ تنتهي بـsyscall (0F 05) — مخرَج exit
    ASSERT_EQ(bin[bin.size() - 2], uint8_t(0x0F));
    ASSERT_EQ(bin[bin.size() - 1], uint8_t(0x05));
}

// (AR) الإرجاعُ الثابت المباشر يُخفَّض أيضًا (مسار RET بمعاملٍ ثابت أو سجلٍّ منقول).
TEST(NativeSirBridge, LowersConstantReturn)
{
    auto module = buildSir(kSrcConst);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    ASSERT_TRUE(res.code.size() > sad::native::elf::kCodeOffset);
}

// (AR) يكتب ثنائيَّ الجمع إلى ملفٍّ لبرهانِ تشغيلٍ حيٍّ (خروج ٤٢) تحت Linux/WSL.
TEST(NativeSirBridge, WritesRunnableBinary)
{
    auto module = buildSir(kSrcAdd);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    ASSERT_TRUE(res.ok);

    std::FILE *fp = std::fopen("sad_sir_bridge_exit42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(res.code.data(), 1, res.code.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(res.code.size()));
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return SAD_RUN_TESTS();
}
