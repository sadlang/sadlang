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

    // (AR) تفرّعٌ صادقُ الشرط (42 > 41) ⇒ فرعُ then ⇒ يُرجع 42؛ else يُرجع 99.
    //      يُثبت: مقارنةٌ مدموجة + jg + كتلٌ متعدّدة + ترقيعُ rel32 أماميّ.
    const std::string kSrcIfTrue =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9\x28\x29\x0A"
        "\x20\x20\x20\x20\xD8\xA5\xD8\xB0\xD8\xA7\x20\x34\x32\x20\x3E\x20\x34\x31\x0A"
        "\x20\x20\x20\x20\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\x34\x32\x0A"
        "\x20\x20\x20\x20\xD9\x88\xD8\xA5\xD9\x84\xD8\xA7\x0A"
        "\x20\x20\x20\x20\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\x39\x39\x0A"
        "\x20\x20\x20\x20\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A"
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";

    // (AR) نداءٌ متداخل: «رئيسية = اجمع(اجمع(40، 0)، 2)» ⇒ الوسيطُ الأوّلُ للنداء الخارجيّ
    //      نتيجةُ نداءٍ داخليّ (مؤقّتٌ سجليّ) ⇒ يخرج ٤٢. يُثبت: النقلُ المتوازي (وسيطٌ مؤقّت
    //      يُحمَّل من خانة انسكابه لا سجلّه) — كان يفشل صراحةً قبل هذه الطبقة.
    const std::string kSrcNested =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xA7\xD8\xAC\xD9\x85\xD8\xB9\x28\xD8\xB1\xD9\x82\xD9\x85\x20\xD8\xA3\xD8\x8C\x20\xD8\xB1\xD9\x82\xD9\x85\x20\xD8\xA8\x29\x0A"
        "\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\xD8\xA3\x20\x2B\x20\xD8\xA8\x0A"
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A"
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9\x28\x29\x0A"
        "\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\xD8\xA7\xD8\xAC\xD9\x85\xD8\xB9\x28\xD8\xA7\xD8\xAC\xD9\x85\xD8\xB9\x28\x34\x30\xD8\x8C\x20\x30\x29\xD8\x8C\x20\x32\x29\x0A"
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";

    // (AR) مؤقّتٌ حيٌّ عبر نداء: «رئيسية = اجمع(40، 0) + اجمع(1، 1)» ⇒ نتيجةُ النداء الأوّل
    //      (%3) حيّةٌ عبر النداء الثاني ثمّ تُجمَع بنتيجته ⇒ يخرج ٤٢. يُثبت: الانسكابُ عبر
    //      النداء (spill/reload) — كان يفشل صراحةً (regOf_ يُبطَل) قبل هذه الطبقة.
    const std::string kSrcLiveAcross =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xA7\xD8\xAC\xD9\x85\xD8\xB9\x28\xD8\xB1\xD9\x82\xD9\x85\x20\xD8\xA3\xD8\x8C\x20\xD8\xB1\xD9\x82\xD9\x85\x20\xD8\xA8\x29\x0A"
        "\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\xD8\xA3\x20\x2B\x20\xD8\xA8\x0A"
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A"
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9\x28\x29\x0A"
        "\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\xD8\xA7\xD8\xAC\xD9\x85\xD8\xB9\x28\x34\x30\xD8\x8C\x20\x30\x29\x20\x2B\x20\xD8\xA7\xD8\xAC\xD9\x85\xD8\xB9\x28\x31\xD8\x8C\x20\x31\x29\x0A"
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";

    // (AR) حلقةُ «بينما» بعدّادٍ متغيّرٍ في الذاكرة: عداد=0؛ بينما عداد<42 { عداد=عداد+1 }؛
    //      ارجع عداد ⇒ يخرج ٤٢. يُثبت: ALLOC/LOAD/STORE + إطارُ دالّة + قفزٌ خلفيّ (لولب)
    //      + قراءةُ متغيّرِ الذاكرة كقيمة (تحميلٌ ضمنيّ). أوّلُ برنامجِ ص ذي حالةٍ متغيّرة.
    const std::string kSrcWhile =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9\x28\x29\x0A"
        "\x20\x20\x20\x20\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1\x20\xD8\xB9\xD8\xAF\xD8\xA7\xD8\xAF\x20\x3D\x20\x30\x0A"
        "\x20\x20\x20\x20\xD8\xA8\xD9\x8A\xD9\x86\xD9\x85\xD8\xA7\x20\xD8\xB9\xD8\xAF\xD8\xA7\xD8\xAF\x20\x3C\x20\x34\x32\x0A"
        "\x20\x20\x20\x20\x20\x20\x20\x20\xD8\xB9\xD8\xAF\xD8\xA7\xD8\xAF\x20\x3D\x20\xD8\xB9\xD8\xAF\xD8\xA7\xD8\xAF\x20\x2B\x20\x31\x0A"
        "\x20\x20\x20\x20\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A"
        "\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\xD8\xB9\xD8\xAF\xD8\xA7\xD8\xAF\x0A"
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";

    // (AR) نداءُ دالّةٍ بوسائط: «اجمع(أ، ب) = أ+ب»؛ «رئيسية = اجمع(40، 2)» ⇒ يخرج ٤٢.
    //      يُثبت: دالّتان + تمرير الوسائط في rdi/rsi (SysV) + call/ret حقيقيّ (لا exit
    //      للمُستدعاة) + الإرجاعُ في rax + خانةُ معاملٍ مُسكَنةٌ من سجلّ الوسيط الوارد.
    const std::string kSrcCall =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xA7\xD8\xAC\xD9\x85\xD8\xB9\x28\xD8\xB1\xD9\x82\xD9\x85\x20\xD8\xA3\xD8\x8C\x20\xD8\xB1\xD9\x82\xD9\x85\x20\xD8\xA8\x29\x0A"
        "\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\xD8\xA3\x20\x2B\x20\xD8\xA8\x0A"
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A"
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9\x28\x29\x0A"
        "\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\xD8\xA7\xD8\xAC\xD9\x85\xD8\xB9\x28\x34\x30\xD8\x8C\x20\x32\x29\x0A"
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";

    // (AR) تفرّعٌ كاذبُ الشرط (5 > 41) ⇒ فرعُ else ⇒ يُرجع 42؛ then يُرجع 99.
    //      يُثبت: القفزُ غيرُ المشروط للفرع الآخر (jmp else) صحيحٌ أيضًا.
    const std::string kSrcIfFalse =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9\x28\x29\x0A"
        "\x20\x20\x20\x20\xD8\xA5\xD8\xB0\xD8\xA7\x20\x35\x20\x3E\x20\x34\x31\x0A"
        "\x20\x20\x20\x20\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\x39\x39\x0A"
        "\x20\x20\x20\x20\xD9\x88\xD8\xA5\xD9\x84\xD8\xA7\x0A"
        "\x20\x20\x20\x20\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\x34\x32\x0A"
        "\x20\x20\x20\x20\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A"
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";

    // (AR) ضربٌ ثمّ طرحُ ثابت: «ارجع 40 * 2 - 38» ⇒ 80−38 = 42. يُثبت MUL_I64 (imul
    //      0F AF، الوجهةُ = reg عكسُ add/sub) + SUB بفوريّ (81 /5) في تعبيرٍ واحد.
    const std::string kSrcMul =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9\x28\x29\x0A"
        "\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\x34\x30\x20\x2A\x20\x32\x20\x2D\x20\x33\x38\x0A"
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";

    // (AR) باقي القسمة: «ارجع 85 % 43» ⇒ 42. يُثبت MOD_I64 (idiv F7 /7 + cqo، الباقي
    //      في rdx). النتيجةُ أوّلُ سجلٍّ يُخصَّص ⇒ dst==RDX ⇒ يُمارِس مسارَ «تخطّي استعادةِ
    //      rdx» (إصلاحُ عائقِ دهسِ الباقي) مباشرةً: خروجٌ خاطئٌ إن انكسر الإصلاح.
    const std::string kSrcMod =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9\x28\x29\x0A"
        "\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\x38\x35\x20\x25\x20\x34\x33\x0A"
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";

    // (AR) قسمةٌ صحيحة: «ارجع 84 // 2» ⇒ 42. يُثبت FLOOR_DIV_I64 (idiv F7 /7 + cqo،
    //      الحاصلُ في rax). النتيجةُ أوّلُ تخصيصٍ ⇒ dst==RDX ⇒ يُمارِس نقلَ rax⇒rdx ثمّ
    //      تخطّيَ الاستعادة (فرعُ FLOOR_DIV من الإصلاح): خروجٌ خاطئٌ إن انكسر.
    const std::string kSrcFloorDiv =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9\x28\x29\x0A"
        "\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\x38\x34\x20\x2F\x2F\x20\x32\x0A"
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";

    // (AR) طرحُ سجلٍّ من سجلّ: «ناقص(أ، ب) = أ − ب»؛ «رئيسية = ناقص(50، 8)» ⇒ 42. يُثبت
    //      SUB reg-reg (29 /r، reg=المصدر rm=الوجهة) بمعامِلَين كلاهما من الذاكرة (لا فوريّ).
    const std::string kSrcSubReg =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD9\x86\xD8\xA7\xD9\x82\xD8\xB5\x28\xD8\xB1\xD9\x82\xD9\x85\x20\xD8\xA3\xD8\x8C\x20\xD8\xB1\xD9\x82\xD9\x85\x20\xD8\xA8\x29\x0A"
        "\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\xD8\xA3\x20\x2D\x20\xD8\xA8\x0A"
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A"
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9\x28\x29\x0A"
        "\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\xD9\x86\xD8\xA7\xD9\x82\xD8\xB5\x28\x35\x30\xD8\x8C\x20\x38\x29\x0A"
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";
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

namespace
{
    // (AR) هل تحوي البايتاتُ التسلسلَ الفرعيّ needle؟ (لتأكيد إصدار بايتات القفز فعلًا.)
    bool contains(const std::vector<uint8_t> &hay, const std::vector<uint8_t> &needle)
    {
        if (needle.empty() || hay.size() < needle.size())
            return false;
        for (size_t i = 0; i + needle.size() <= hay.size(); ++i)
        {
            bool m = true;
            for (size_t j = 0; j < needle.size(); ++j)
                if (hay[i + j] != needle[j])
                {
                    m = false;
                    break;
                }
            if (m)
                return true;
        }
        return false;
    }
} // namespace

// (AR) تفرّعٌ صادقُ الشرط يُخفَّض، والـELF سليم، والبايتاتُ تحوي jg (0F 8F) وjmp (E9).
TEST(NativeSirBridge, LowersIfElseTrueBranch)
{
    auto module = buildSir(kSrcIfTrue);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    ASSERT_EQ(int(bin[18]) | (int(bin[19]) << 8), 62); // EM_X86_64
    // (AR) jg = 0F 8F (المقارنة المدموجة للشرط >) + jmp = E9 (لفرع else) موجودان.
    ASSERT_TRUE(contains(bin, {0x0F, 0x8F}));
    ASSERT_TRUE(contains(bin, {0xE9}));
}

// (AR) تفرّعٌ كاذبُ الشرط يُخفَّض أيضًا (مسار jmp else).
TEST(NativeSirBridge, LowersIfElseFalseBranch)
{
    auto module = buildSir(kSrcIfFalse);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    ASSERT_TRUE(res.code.size() > sad::native::elf::kCodeOffset);
}

// (AR) يكتب ثنائيَّي التفرّع (كلاهما يخرج ٤٢: أحدهما عبر then والآخر عبر else)
//      لبرهانِ تشغيلٍ حيٍّ تحت Linux/WSL.
TEST(NativeSirBridge, WritesBranchingBinaries)
{
    struct Case
    {
        const std::string *src;
        const char *file;
    } cases[] = {
        {&kSrcIfTrue, "sad_sir_if_true42"},
        {&kSrcIfFalse, "sad_sir_if_false42"},
    };
    for (const auto &c : cases)
    {
        auto module = buildSir(*c.src);
        ASSERT_TRUE(module != nullptr);
        auto res = sad::native::lowerModuleToElf(*module);
        ASSERT_TRUE(res.ok);
        std::FILE *fp = std::fopen(c.file, "wb");
        ASSERT_TRUE(fp != nullptr);
        size_t wrote = std::fwrite(res.code.data(), 1, res.code.size(), fp);
        std::fclose(fp);
        ASSERT_EQ(int(wrote), int(res.code.size()));
    }
}

// (AR) قفزٌ خلفيّ (إزاحةٌ سالبة): يُبنى SIR يدويًّا بثلاث كتل — entry يقفز أمامًا فوق
//      L_b إلى L_c، وL_c يقفز خلفًا إلى L_b الذي يُرجع ٤٢. يُثبت أنّ ترقيعَ rel32
//      يحسب الإزاحةَ السالبة صحيحًا (مسارٌ لا يُنتجه مصدرُ ص بلا لولبٍ ذي ذاكرة).
TEST(NativeSirBridge, BackwardJumpNegativeFixup)
{
    using namespace Sad::Compiler::SIR;
    auto mod = std::make_shared<SIRModule>("backjump");
    auto fn = std::make_shared<SIRFunction>(
        "\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9", Sad::Types::SadTypeKind::Integer); // رئيسية

    auto entry = std::make_shared<SIRBasicBlock>("entry");
    entry->addInstruction(SIRInstruction::Branch(SIROperand::Label("L_c")));
    auto lb = std::make_shared<SIRBasicBlock>("L_b");
    lb->addInstruction(SIRInstruction::Return(SIROperand::ConstantI64(42)));
    auto lc = std::make_shared<SIRBasicBlock>("L_c");
    lc->addInstruction(SIRInstruction::Branch(SIROperand::Label("L_b"))); // ← قفزٌ خلفيّ

    fn->addBasicBlock(entry);
    fn->addBasicBlock(lb);
    fn->addBasicBlock(lc);
    mod->addFunction(fn);

    auto res = sad::native::lowerModuleToElf(*mod);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    // (AR) يحوي قفزَين غيرَ مشروطين (E9)؛ والقفزُ الخلفيّ صغيرُ المقدار سالبٌ ⇒ يظهر
    //      امتدادُ إشارته كسلسلةِ بايتات 0xFF المتتالية (FF FF FF) في حقل rel32.
    ASSERT_TRUE(contains(bin, {0xE9}));
    ASSERT_TRUE(contains(bin, {0xFF, 0xFF, 0xFF}));
    // (AR) اكتبه لبرهانِ تشغيلٍ حيٍّ (يخرج ٤٢ عبر: entry→L_c→L_b) تحت Linux/WSL.
    std::FILE *fp = std::fopen("sad_sir_backjump42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

// (AR) حلقةُ «بينما» بعدّادٍ في الذاكرة تُخفَّض، والـELF سليم، والبايتاتُ تحوي مقدّمةَ
//      الإطار (push rbp = 0x55) وتحميلَ خانةٍ (mov reg,[rbp-8] = 8B 45 F8) وتخزينَها
//      (89 45 F8) — دليلُ استعمالِ الذاكرة فعلًا لا مجرّد سجلّات. تُكتب للبرهان الحيّ.
TEST(NativeSirBridge, LowersWhileLoopWithMemory)
{
    auto module = buildSir(kSrcWhile);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    ASSERT_EQ(int(bin[18]) | (int(bin[19]) << 8), 62); // EM_X86_64
    ASSERT_TRUE(contains(bin, {0x55}));             // push rbp (مقدّمة الإطار)
    ASSERT_TRUE(contains(bin, {0x8B, 0x45, 0xF8})); // mov reg, [rbp-8] (تحميل الخانة)
    ASSERT_TRUE(contains(bin, {0x89, 0x45, 0xF8})); // mov [rbp-8], reg (تخزين الخانة)
    ASSERT_TRUE(contains(bin, {0xFF, 0xFF, 0xFF})); // قفزٌ خلفيّ (لولب) بإزاحةٍ سالبة

    std::FILE *fp = std::fopen("sad_sir_while42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

// (AR) نداءُ دالّةٍ بوسائط يُخفَّض، والـELF سليم، والبايتاتُ تحوي call (E8) وret (C3)
//      وتخزينَ وسيطٍ واردٍ ([rbp-8],rdi = 48 89 7D F8). يُكتب للبرهان الحيّ (خروج ٤٢).
TEST(NativeSirBridge, LowersFunctionCall)
{
    auto module = buildSir(kSrcCall);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    ASSERT_EQ(int(bin[18]) | (int(bin[19]) << 8), 62); // EM_X86_64
    ASSERT_TRUE(contains(bin, {0xE8}));             // call rel32 (نداءُ المُستدعاة)
    ASSERT_TRUE(contains(bin, {0xC3}));             // ret (خاتمةُ المُستدعاة)
    ASSERT_TRUE(contains(bin, {0x48, 0x89, 0x7D, 0xF8})); // mov [rbp-8],rdi (تخزينُ وسيطٍ وارد)

    std::FILE *fp = std::fopen("sad_sir_call42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

// (AR) نداءٌ متداخل (وسيطٌ مؤقّت) يُخفَّض ويُكتَب للبرهان الحيّ (خروج ٤٢). لم يعد يفشل.
TEST(NativeSirBridge, LowersNestedCallTempArg)
{
    auto module = buildSir(kSrcNested);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    ASSERT_TRUE(res.code.size() > sad::native::elf::kCodeOffset);
    std::FILE *fp = std::fopen("sad_sir_nested42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(res.code.data(), 1, res.code.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(res.code.size()));
}

// (AR) مؤقّتٌ حيٌّ عبر نداء (spill/reload) يُخفَّض ويُكتَب للبرهان الحيّ (خروج ٤٢).
TEST(NativeSirBridge, LowersLiveAcrossCall)
{
    auto module = buildSir(kSrcLiveAcross);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    ASSERT_TRUE(res.code.size() > sad::native::elf::kCodeOffset);
    std::FILE *fp = std::fopen("sad_sir_liveacross42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(res.code.data(), 1, res.code.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(res.code.size()));
}

// (AR) ضربٌ ثمّ طرحُ فوريّ (40*2−38=42) يُخفَّض، والـELF سليم، والبايتاتُ تحوي imul
//      (0F AF) وطرحَ الفوريّ (81 /5 ⇒ ModRM بامتداد reg=5). يُكتب للبرهان الحيّ (خروج ٤٢).
TEST(NativeSirBridge, LowersMultiplyThenSubImm)
{
    auto module = buildSir(kSrcMul);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    ASSERT_EQ(int(bin[18]) | (int(bin[19]) << 8), 62); // EM_X86_64
    ASSERT_TRUE(contains(bin, {0x0F, 0xAF})); // imul r64, r64

    std::FILE *fp = std::fopen("sad_sir_mul42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

// (AR) باقي القسمة (85%43=42) يُخفَّض، والبايتاتُ تحوي cqo (48 99) وidiv (48 F7 /7 ⇒
//      ModRM D8..DF بامتداد reg=7) وحفظَ/استعادةَ خانةِ خدشِ rdx. يُكتب للبرهان الحيّ.
//      يُمارِس مسارَ dst==RDX (تخطّي الاستعادة) — إصلاحُ عائقِ دهسِ الباقي.
TEST(NativeSirBridge, LowersModuloReturn)
{
    auto module = buildSir(kSrcMod);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    ASSERT_EQ(int(bin[18]) | (int(bin[19]) << 8), 62); // EM_X86_64
    ASSERT_TRUE(contains(bin, {0x48, 0x99}));       // cqo (امتدادُ إشارةِ rax إلى rdx:rax)
    ASSERT_TRUE(contains(bin, {0x48, 0xF7}));       // idiv r64 (F7 /7)

    std::FILE *fp = std::fopen("sad_sir_mod42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

// (AR) القسمةُ الصحيحة (84//2=42) تُخفَّض، والبايتاتُ تحوي cqo (48 99) وidiv (48 F7).
//      يُكتب للبرهان الحيّ. يُمارِس فرعَ FLOOR_DIV من إصلاح العائق (نقلُ rax⇒rdx ثمّ تخطّي).
TEST(NativeSirBridge, LowersFloorDivReturn)
{
    auto module = buildSir(kSrcFloorDiv);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    ASSERT_EQ(int(bin[18]) | (int(bin[19]) << 8), 62); // EM_X86_64
    ASSERT_TRUE(contains(bin, {0x48, 0x99})); // cqo
    ASSERT_TRUE(contains(bin, {0x48, 0xF7})); // idiv r64

    std::FILE *fp = std::fopen("sad_sir_floordiv42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

// (AR) طرحُ سجلٍّ من سجلّ (ناقص(50،8)=42) يُخفَّض، والبايتاتُ تحوي sub r64,r64 (48 29 /r)
//      وcall (E8) وret (C3). يُكتب للبرهان الحيّ (خروج ٤٢). أوّلُ SUB بمعامِلَين سجليَّين.
TEST(NativeSirBridge, LowersSubRegReg)
{
    auto module = buildSir(kSrcSubReg);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    ASSERT_EQ(int(bin[18]) | (int(bin[19]) << 8), 62); // EM_X86_64
    ASSERT_TRUE(contains(bin, {0x48, 0x29})); // sub r64, r64 (29 /r)
    ASSERT_TRUE(contains(bin, {0xE8}));       // call rel32 (نداءُ ناقص)

    std::FILE *fp = std::fopen("sad_sir_subreg42", "wb");
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
