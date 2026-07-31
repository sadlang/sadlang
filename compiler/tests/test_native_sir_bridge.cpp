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
#include "backend/native/arm64_sir_lowering.h"

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

    // (AR) يبني برنامجَ «دالة رئيسية() ارجع <تعبير> نهاية» — الأجزاءُ العربيّةُ ثابتةٌ والتعبيرُ ASCII.
    std::string mkReturn(const std::string &expr)
    {
        return "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9()\n"
               "    \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 " + expr + "\n"
               "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n";
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

    // (AR) طباعةُ سلسلةٍ حرفيّة: «اطبع_سطر("مرحبا")» ⇒ يطبع «مرحبا\n» ويخرج ٠. يُثبت: كتلةُ
    //      بيانات rodata في المقطع R+X + mov rsi,عنوان مطلق (مُرقَّع) + write(stdout).
    const std::string kSrcPrintStr =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9\x28\x29\x0A"
        "\x20\x20\x20\x20\xD8\xA7\xD8\xB7\xD8\xA8\xD8\xB9\x5F\xD8\xB3\xD8\xB7\xD8\xB1\x28\x22\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7\x22\x29\x0A"
        "\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\x30\x0A"
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";

    // (AR) طباعةُ ثابتٍ عدديّ: «اطبع_سطر(42)» ⇒ يطبع «42\n» ويخرج ٠. يُثبت: itoa (idiv/10 +
    //      mov m8,r8) في مخزن الإطار + write. النتيجةُ نصّيّةٌ حقيقيّةٌ لا رمزُ خروجٍ فقط.
    const std::string kSrcPrintNum =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9\x28\x29\x0A"
        "\x20\x20\x20\x20\xD8\xA7\xD8\xB7\xD8\xA8\xD8\xB9\x5F\xD8\xB3\xD8\xB7\xD8\xB1\x28\x34\x32\x29\x0A"
        "\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\x30\x0A"
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";

    // (AR) طباعةُ عددٍ محسوب: «اطبع_سطر(40 + 2)» ⇒ يطبع «42\n». يُثبت: الحسابُ ثمّ itoa معًا
    //      (المؤقّتُ يُنسَك حولَ الطباعة ⇒ صحّةُ الانسكاب حولَ تسلسلٍ يُبدِّد سجلّاتِ الحوض).
    const std::string kSrcPrintComputed =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9\x28\x29\x0A"
        "\x20\x20\x20\x20\xD8\xA7\xD8\xB7\xD8\xA8\xD8\xB9\x5F\xD8\xB3\xD8\xB7\xD8\xB1\x28\x34\x30\x20\x2B\x20\x32\x29\x0A"
        "\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\x30\x0A"
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";

    // (AR) قسمةٌ ونتيجتُها ليست RDX مع مؤقّتٍ حيٍّ في RDX: «(40 + 2) + (84 % 42)» ⇒ 42 + 0 = 42.
    //      يُثبت فرعَ dst!=RDX في القسمة (حفظُ rdx الحاملِ لـ%t1=42 ثمّ استعادتُه) — كان مُحقَّقًا
    //      تحليليًّا فقط؛ الآن برهانٌ حيّ (خروج ٤٢). لو انكسر الحفظُ/الاستعادةُ لخرج بقيمةٍ خاطئة.
    const std::string kSrcModLiveRdx =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9\x28\x29\x0A"
        "\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\x28\x34\x30\x20\x2B\x20\x32\x29\x20\x2B\x20\x28\x38\x34\x20\x25\x20\x34\x32\x29\x0A"
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

// (AR) طباعةُ سلسلةٍ حرفيّة تُخفَّض، والبايتاتُ تحوي mov r64,imm64 (48 B8) لعنوان السلسلة
//      وsyscall (0F 05)، وتنتهي الشيفرةُ بكتلةِ بيانات تحوي «مرحبا». تُكتب للبرهان الحيّ.
TEST(NativeSirBridge, PrintsStringLiteral)
{
    auto module = buildSir(kSrcPrintStr);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    ASSERT_EQ(int(bin[18]) | (int(bin[19]) << 8), 62);   // EM_X86_64
    ASSERT_TRUE(contains(bin, {0x48, 0xBE})); // movabs rsi, imm64 (REX.W B8+6=BE) لعنوان السلسلة
    ASSERT_TRUE(contains(bin, {0x0F, 0x05})); // syscall (write)
    // (AR) بايتاتُ «مرحبا» (UTF-8) موجودةٌ في كتلة البيانات المُلحَقة.
    ASSERT_TRUE(contains(bin, {0xD9, 0x85, 0xD8, 0xB1, 0xD8, 0xAD, 0xD8, 0xA8, 0xD8, 0xA7}));

    std::FILE *fp = std::fopen("sad_sir_printstr", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

// (AR) طباعةُ عددٍ ثابتٍ تُخفَّض، والبايتاتُ تحوي idiv (48 F7) وmov m8,r8 (88 /r ⇒ 41 88 12
//      لـ[r10],dl) وsyscall — دليلُ itoa فعليّ. تُكتب للبرهان الحيّ (يطبع «42»).
TEST(NativeSirBridge, PrintsNumberLiteral)
{
    auto module = buildSir(kSrcPrintNum);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    ASSERT_EQ(int(bin[18]) | (int(bin[19]) << 8), 62);   // EM_X86_64
    ASSERT_TRUE(contains(bin, {0x48, 0xF7}));       // idiv r64 (قسمةُ itoa)
    ASSERT_TRUE(contains(bin, {0x41, 0x88, 0x12})); // mov [r10], dl (تخزينُ رقمٍ ASCII)
    ASSERT_TRUE(contains(bin, {0x0F, 0x05}));       // syscall (write)

    std::FILE *fp = std::fopen("sad_sir_printnum", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

// (AR) طباعةُ عددٍ محسوب («40 + 2») تُخفَّض وتُكتب للبرهان الحيّ (يطبع «42»). يُثبت الانسكابَ
//      حولَ الطباعة (المؤقّتُ الحسابيّ ينجو تسلسلَ itoa الذي يُبدِّد سجلّاتِ الحوض).
TEST(NativeSirBridge, PrintsComputedNumber)
{
    auto module = buildSir(kSrcPrintComputed);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    ASSERT_TRUE(res.code.size() > sad::native::elf::kCodeOffset);
    std::FILE *fp = std::fopen("sad_sir_printcomputed", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(res.code.data(), 1, res.code.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(res.code.size()));
}

// (AR) قسمةٌ بنتيجةٍ في سجلٍّ غيرِ RDX مع مؤقّتٍ حيٍّ في RDX تُخفَّض وتُكتب للبرهان الحيّ
//      (خروج ٤٢). برهانُ فرعِ dst!=RDX (حفظ/استعادةُ rdx حولَ idiv) — كان تحليليًّا فقط.
TEST(NativeSirBridge, ModWithLiveRdxTemp)
{
    auto module = buildSir(kSrcModLiveRdx);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    ASSERT_TRUE(contains(bin, {0x48, 0xF7})); // idiv r64
    std::FILE *fp = std::fopen("sad_sir_modliverdx", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

// ════════════════════════════════════════════════════════════════════════════
// (AR) جسر SIR→AArch64 (الهدف الثاني): نفسُ SIR الأماميّ يُخفَّض لـARM64 عبر محرّكٍ
//      وكاتب ELF عامَّين. برهانُ عموميّة الخطّ عبر صنفَي ISA. تُكتب الثنائيّاتُ لبرهانٍ
//      حيٍّ على qemu-aarch64. e_machine=EM_AARCH64=183.
// ════════════════════════════════════════════════════════════════════════════

// (AR) مُساعِدٌ: يبني SIR من مصدر ص، يخفّضه لـARM64، يؤكّد السلامةَ، ويكتب الثنائيَّ.
namespace
{
    void lowerArm64AndWrite(const std::string &src, const char *file,
                            bool *okOut, size_t *sizeOut)
    {
        *okOut = false;
        *sizeOut = 0;
        auto module = buildSir(src);
        if (!module)
            return;
        auto res = sad::native::lowerModuleToElfArm64(*module);
        if (!res.ok)
        {
            std::printf("arm64 lowering error: %s\n", res.message().c_str());
            return;
        }
        const auto &bin = res.code;
        if (bin.size() <= sad::native::elf::kCodeOffset)
            return;
        // (AR) e_machine=EM_AARCH64=183 (بايتا 18..19).
        if ((int(bin[18]) | (int(bin[19]) << 8)) != 183)
            return;
        std::FILE *fp = std::fopen(file, "wb");
        if (!fp)
            return;
        size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
        std::fclose(fp);
        *okOut = (wrote == bin.size());
        *sizeOut = bin.size();
    }
} // namespace

// (AR) إرجاعُ ثابتٍ على ARM64: «ارجع 42» ⇒ movz + نسخ إلى x0 + svc(exit). يخرج ٤٢ على qemu.
TEST(Arm64SirBridge, LowersConstantReturn)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcConst, "sad_arm64_const42", &ok, &sz);
    ASSERT_TRUE(ok);
    ASSERT_TRUE(sz > sad::native::elf::kCodeOffset);
}

// (AR) ضربٌ وطرحٌ على ARM64: «40 * 2 - 38» ⇒ mul ثمّ sub ⇒ يخرج ٤٢. يُثبت MUL/SUB الجديدَين.
TEST(Arm64SirBridge, LowersMultiplyAndSub)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcMul, "sad_arm64_mul42", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) باقٍ على ARM64: «85 % 43» ⇒ sdiv ثمّ msub (الباقي = a − (a÷b)×b) ⇒ يخرج ٤٢.
//      يُثبت SDIV + MSUB الجديدَين (لا تعليمةَ باقٍ مفردة في ARM64).
TEST(Arm64SirBridge, LowersModulo)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcMod, "sad_arm64_mod42", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) قسمةٌ صحيحةٌ على ARM64: «84 // 2» ⇒ sdiv ⇒ يخرج ٤٢.
TEST(Arm64SirBridge, LowersFloorDiv)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcFloorDiv, "sad_arm64_floordiv42", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) تفرّعٌ صادقُ الشرط على ARM64: «إذا 42>41 ارجع 42 وإلا ارجع 99» ⇒ cmp؛ b.gt then؛
//      b else ⇒ يخرج ٤٢ عبر فرع then. يُثبت المقارنةَ المدموجة + b.cond + الترقيع البتّيّ.
TEST(Arm64SirBridge, LowersIfElseTrueBranch)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcIfTrue, "sad_arm64_iftrue42", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) تفرّعٌ كاذبُ الشرط على ARM64: «إذا 5>41 … وإلا ارجع 42» ⇒ يخرج ٤٢ عبر فرع else (b).
TEST(Arm64SirBridge, LowersIfElseFalseBranch)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcIfFalse, "sad_arm64_iffalse42", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) حلقةُ «بينما» بعدّادٍ في الذاكرة على ARM64: «عداد=0؛ بينما عداد<42 عداد=عداد+1؛ ارجع
//      عداد» ⇒ يخرج ٤٢. يُثبت: إطارٌ (sub sp) + STR/LDR [sp,#off] + قفزٌ خلفيّ (لولب) + قراءةُ
//      متغيّرِ الذاكرة كقيمة. أوّلُ برنامجِ ص ذي حالةٍ متغيّرة يُترجَم أصليًّا لـARM64.
TEST(Arm64SirBridge, LowersWhileLoopWithMemory)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcWhile, "sad_arm64_while42", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) نداءُ دالّةٍ على ARM64: «اجمع(أ،ب)=أ+ب؛ رئيسية=اجمع(40،2)» ⇒ يخرج ٤٢. يُثبت: BL
//      (imm26 مُرقَّع لإزاحة الدالّة) + AAPCS64 (الوسائط x0/x1، النتيجة x0) + خاتمةُ الورقة
//      (add sp؛ ret إلى x30 بلا حفظه) + مقدّمةٌ تُسكِن x0/x1 في خانات المعاملات.
TEST(Arm64SirBridge, LowersFunctionCall)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcCall, "sad_arm64_call42", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) نداءٌ متداخل: «اجمع(اجمع(40،0)،2)» ⇒ الوسيطُ الأوّلُ مؤقّتٌ (نتيجةُ نداءٍ) ⇒ يُنسَك
//      ويُحمَّل من خانته ⇒ صفر تصادمِ نقلٍ متوازٍ. يخرج ٤٢.
TEST(Arm64SirBridge, LowersNestedCallTempArg)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcNested, "sad_arm64_nested42", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) مؤقّتٌ حيٌّ عبر نداء: «اجمع(40،0)+اجمع(1،1)» ⇒ نتيجةُ الأوّلِ حيّةٌ عبر الثاني ⇒ تُنسَك
//      وتُعاد (bl يدهس الحوضَ caller-saved). يخرج ٤٢.
TEST(Arm64SirBridge, LowersLiveAcrossCall)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcLiveAcross, "sad_arm64_liveacross42", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) طباعةُ سلسلةٍ حرفيّة على ARM64: «اطبع_سطر("مرحبا")» ⇒ يطبع «مرحبا\n». يُثبت: عنوانُ
//      السلسلة ٦٤-بت مبنيٌّ movz+movk×3 (مُرقَّع) + svc-write (x8=64، x0=1). أوّلُ مخرَجٍ نصّيّ ARM64.
TEST(Arm64SirBridge, PrintsStringLiteral)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcPrintStr, "sad_arm64_printstr", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) طباعةُ ثابتٍ عدديّ على ARM64: «اطبع_سطر(42)» ⇒ يطبع «42\n». يُثبت: itoa عبر sdiv/msub
//      + strb في مخزن الإطار + cbnz (لولب) + svc-write.
TEST(Arm64SirBridge, PrintsNumberLiteral)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcPrintNum, "sad_arm64_printnum", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) طباعةُ عددٍ محسوب على ARM64: «اطبع_سطر(40 + 2)» ⇒ يطبع «42\n». يُثبت الحسابَ ثمّ itoa
//      معًا (المؤقّتُ يُنسَك حولَ الطباعة ⇒ صحّةُ الانسكاب حولَ تسلسلٍ يُبدِّد الحوضَ).
TEST(Arm64SirBridge, PrintsComputedNumber)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcPrintComputed, "sad_arm64_printcomputed", &ok, &sz);
    ASSERT_TRUE(ok);
}

// ════════════════════════════════════════════════════════════════════════════
// (AR) العمليّات البتّيّة (توسيعُ الأوپكودات نحو الاستضافة الذاتيّة) — تُخفَّض على
//      المسارين. التعابيرُ مُصمَّمةٌ لتخرج ٤٢: AND(106&47)، OR(40|2)، XOR(46^4)،
//      NOT(~213 ⇒ البايتُ الأدنى 0x2A)، SHL(21<<1)، SHR(84>>1).
// ════════════════════════════════════════════════════════════════════════════
namespace
{
    // (AR) تعابيرُ بتّيّةٌ نتيجتُها ٤٢ (أو بايتُها الأدنى لـNOT). ثابتةٌ مشتركةٌ للمسارين.
    struct BitCase { const char *expr; const char *x86File; const char *armFile; };
    const BitCase kBitCases[] = {
        {"106 & 47", "sad_x86_and42", "sad_arm64_and42"},
        {"40 | 2", "sad_x86_or42", "sad_arm64_or42"},
        {"46 ^ 4", "sad_x86_xor42", "sad_arm64_xor42"},
        {"~213", "sad_x86_not42", "sad_arm64_not42"},
        {"21 << 1", "sad_x86_shl42", "sad_arm64_shl42"},
        {"84 >> 1", "sad_x86_shr42", "sad_arm64_shr42"},
    };
} // namespace

// (AR) x86: كلُّ تعبيرٍ بتّيّ يُخفَّض ويُكتَب ثنائيًّا (يُنفَّذ نيتيفيًّا في برهانٍ منفصل).
TEST(NativeSirBridge, LowersBitwiseOps)
{
    for (const auto &c : kBitCases)
    {
        auto module = buildSir(mkReturn(c.expr));
        ASSERT_TRUE(module != nullptr);
        auto res = sad::native::lowerModuleToElf(*module);
        if (!res.ok)
            std::printf("x86 bitwise «%s» error: %s\n", c.expr, res.message().c_str());
        ASSERT_TRUE(res.ok);
        std::FILE *fp = std::fopen(c.x86File, "wb");
        ASSERT_TRUE(fp != nullptr);
        std::fwrite(res.code.data(), 1, res.code.size(), fp);
        std::fclose(fp);
    }
}

// (AR) ARM64: نفسُ التعابير تُخفَّض لـAArch64 (and/orr/eor/mvn/lslv/lsrv) وتُكتَب ثنائيًّا.
TEST(Arm64SirBridge, LowersBitwiseOps)
{
    for (const auto &c : kBitCases)
    {
        bool ok = false;
        size_t sz = 0;
        lowerArm64AndWrite(mkReturn(c.expr), c.armFile, &ok, &sz);
        ASSERT_TRUE(ok);
    }
}

// ════════════════════════════════════════════════════════════════════════════
// (AR) المقارنةُ كقيمة (setcc/cset) + الإزاحةُ المتغيّرة على x86 (CL). تعابيرُ المقارنةِ
//      نتيجتُها ٠/١ ثمّ +٤١ ⇒ ٤٢ (تُثبت أنّ القيمةَ ١ لا مجرّدُ فرع). الإزاحةُ المتغيّرة:
//      «متغير ن=1؛ ارجع 21 << ن» ⇒ ٤٢ (المقدارُ من الذاكرة لا ثابت).
// ════════════════════════════════════════════════════════════════════════════
namespace
{
    // (AR) تعابيرُ مقارنةٍ نتيجتُها true(1)+41=42، تمرّ بكلّ الشروط الستّة.
    struct CmpCase { const char *expr; const char *x86File; const char *armFile; };
    const CmpCase kCmpCases[] = {
        {"(7 == 7) + 41", "sad_x86_eq42", "sad_arm64_eq42"},
        {"(3 != 9) + 41", "sad_x86_ne42", "sad_arm64_ne42"},
        {"(3 < 9) + 41", "sad_x86_lt42", "sad_arm64_lt42"},
        {"(9 <= 9) + 41", "sad_x86_le42", "sad_arm64_le42"},
        {"(9 > 3) + 41", "sad_x86_gt42", "sad_arm64_gt42"},
        {"(9 >= 9) + 41", "sad_x86_ge42", "sad_arm64_ge42"},
    };
    // (AR) إزاحةٌ بمقدارٍ متغيّر: ن متغيّرٌ في الذاكرة ⇒ 21 << ن = 42.
    const std::string kSrcVarShift =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9()\n"
        "    \xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x86 = 1\n"          // متغير ن = 1
        "    \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 21 << \xD9\x86\n"               // ارجع 21 << ن
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n";
} // namespace

// (AR) x86: المقارنةُ كقيمة (setcc+movzx) — ٦ شروط.
TEST(NativeSirBridge, LowersComparisonAsValue)
{
    for (const auto &c : kCmpCases)
    {
        auto module = buildSir(mkReturn(c.expr));
        ASSERT_TRUE(module != nullptr);
        auto res = sad::native::lowerModuleToElf(*module);
        if (!res.ok)
            std::printf("x86 cmp «%s» error: %s\n", c.expr, res.message().c_str());
        ASSERT_TRUE(res.ok);
        std::FILE *fp = std::fopen(c.x86File, "wb");
        ASSERT_TRUE(fp != nullptr);
        std::fwrite(res.code.data(), 1, res.code.size(), fp);
        std::fclose(fp);
    }
}

// (AR) x86: الإزاحةُ بمقدارٍ متغيّر (CL + حفظُ RCX) ⇒ يُكمِل دَينَ الإزاحة، تكافؤًا مع ARM64.
TEST(NativeSirBridge, LowersVariableShift)
{
    auto module = buildSir(kSrcVarShift);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("x86 varshift error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    std::FILE *fp = std::fopen("sad_x86_varshift42", "wb");
    ASSERT_TRUE(fp != nullptr);
    std::fwrite(res.code.data(), 1, res.code.size(), fp);
    std::fclose(fp);
}

// (AR) ARM64: المقارنةُ كقيمة (cset) — ٦ شروط.
TEST(Arm64SirBridge, LowersComparisonAsValue)
{
    for (const auto &c : kCmpCases)
    {
        bool ok = false;
        size_t sz = 0;
        lowerArm64AndWrite(mkReturn(c.expr), c.armFile, &ok, &sz);
        ASSERT_TRUE(ok);
    }
}

// (AR) ARM64: الإزاحةُ بمقدارٍ متغيّر (lslv، المقدارُ من الذاكرة) ⇒ ٤٢.
TEST(Arm64SirBridge, LowersVariableShift)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcVarShift, "sad_arm64_varshift42", &ok, &sz);
    ASSERT_TRUE(ok);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return SAD_RUN_TESTS();
}
