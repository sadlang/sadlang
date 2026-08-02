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

    // (AR) اقتطاعُ عشريٍّ لصحيح عبر معاملِ دالّةٍ عشريّ (يهزم طيَّ الثوابت + نوعُه Float صريحًا):
    //      «حوّل(عشري س) ارجع رقم(س)؛ رئيسية ارجع حوّل(42.0)» ⇒ ٤٢. F64_TO_I64 (movq+cvttsd2si).
    const std::string kSrcFloatArith =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xAD\xD9\x88\xD9\x91\xD9\x84\x28\xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A\x20\xD8\xB3\x29\x0A\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\xD8\xB1\xD9\x82\xD9\x85\x28\xD8\xB3\x29\x0A\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9\x28\x29\x0A\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\xD8\xAD\xD9\x88\xD9\x91\xD9\x84\x28\x34\x32\x2E\x30\x29\x0A\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";

    // (AR) طباعةُ عشريّ: «ع = 40.5 + 1.0؛ اطبع_سطر(ع)» ⇒ «41.5». ADD_F64 (addsd) + المُنسِّق.
    const std::string kSrcFloatPrint =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9\x28\x29\x0A\x20\x20\x20\x20\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1\x20\xD8\xB9\x20\x3D\x20\x34\x30\x2E\x35\x20\x2B\x20\x31\x2E\x30\x0A\x20\x20\x20\x20\xD8\xA7\xD8\xB7\xD8\xA8\xD8\xB9\x5F\xD8\xB3\xD8\xB7\xD8\xB1\x28\xD8\xB9\x29\x0A\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\x30\x0A\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";

    // (AR) مصفوفةٌ مختلطةٌ int/float معلَّبة ⇒ «3.5» ثمّ «1». ARRAY_SET/GET(Any) + طباعةٌ مبوَّبة.
    const std::string kSrcBoxedMixed =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9\x28\x29\x0A\x20\x20\x20\x20\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1\x20\xD9\x85\x20\x3D\x20\x5B\x31\xD8\x8C\x20\x33\x2E\x35\xD8\x8C\x20\x32\x5D\x0A\x20\x20\x20\x20\xD8\xA7\xD8\xB7\xD8\xA8\xD8\xB9\x5F\xD8\xB3\xD8\xB7\xD8\xB1\x28\xD9\x85\x5B\x31\x5D\x29\x0A\x20\x20\x20\x20\xD8\xA7\xD8\xB7\xD8\xA8\xD8\xB9\x5F\xD8\xB3\xD8\xB7\xD8\xB1\x28\xD9\x85\x5B\x30\x5D\x29\x0A\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\x30\x0A\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";

    // (AR) نصٌّ معلَّب في مصفوفةٍ مختلطة: «م = [1، "نص"، 2]؛ اطبع_سطر(م[1])» ⇒ «نص». الوسمُ Str
    //      والحمولةُ عنوانُ واصفٍ {len،bytes} في rodata؛ الطباعةُ المبوَّبةُ تكتب البايتات زمنَ التشغيل.
    const std::string kSrcBoxedStr =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9()\x0A    \xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x85 = [1\xD8\x8C \x22\xD9\x86\xD8\xB5\x22\xD8\x8C 2]\x0A    \xD8\xA7\xD8\xB7\xD8\xA8\xD8\xB9_\xD8\xB3\xD8\xB7\xD8\xB1(\xD9\x85[1])\x0A    \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 0\x0A\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";

    // (AR) طفحانُ الكسر: «2.9999998» ⇒ «3.0» (ترحيلُ الحمل — إصلاحُ عائق أميليا).
    const std::string kSrcFloatCarry =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9\x20\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9\x28\x29\x0A\x20\x20\x20\x20\xD8\xA7\xD8\xB7\xD8\xA8\xD8\xB9\x5F\xD8\xB3\xD8\xB7\xD8\xB1\x28\x32\x2E\x39\x39\x39\x39\x39\x39\x38\x29\x0A\x20\x20\x20\x20\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9\x20\x30\x0A\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";

    // (AR) إتمامُ الحساب والتحكّم: دالّةٌ فارغة (RET_VOID)⇒«7»، سالبٌ أحاديّ (NEG)+طباعةُ السالب⇒«-42»،
    //      إزاحةٌ حسابيّة (SAR: ‎-16>>2‎)⇒«-4». قيمٌ زمنَ تشغيلٍ (معاملات) لتجنّبِ الطيّ الثابت.
    const std::string kSrcArithComplete =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD9\x86\xD9\x81(\xD8\xB1\xD9\x82\xD9\x85 \xD8\xB3)\x0A    \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 -\xD8\xB3\x0A\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD8\xB2\xD8\xAD(\xD8\xB1\xD9\x82\xD9\x85 \xD8\xB3)\x0A    \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 \xD8\xB3 >> 2\x0A\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD8\xA7\xD8\xB7\xD8\xA8\xD8\xB9_\xD8\xB1\xD8\xB3\xD8\xA7\xD9\x84\xD8\xA9()\x0A    \xD8\xA7\xD8\xB7\xD8\xA8\xD8\xB9_\xD8\xB3\xD8\xB7\xD8\xB1(7)\x0A\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9()\x0A    \xD8\xA7\xD8\xB7\xD8\xA8\xD8\xB9_\xD8\xB1\xD8\xB3\xD8\xA7\xD9\x84\xD8\xA9()\x0A    \xD8\xA7\xD8\xB7\xD8\xA8\xD8\xB9_\xD8\xB3\xD8\xB7\xD8\xB1(\xD9\x86\xD9\x81(42))\x0A    \xD8\xA7\xD8\xB7\xD8\xA8\xD8\xB9_\xD8\xB3\xD8\xB7\xD8\xB1(\xD8\xB2\xD8\xAD(0 - 16))\x0A    \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 0\x0A\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";

    // (AR) مقارنةُ العوائم (المعامِلان Float صريحًا عبر معاملَي دالّةٍ عشريَّين، يهزمان الطيَّ الثابت):
    //      فوق(أ،ب)=أ>ب · دون(أ،ب)=أ<ب · مطابق(أ،ب)=أ==ب. رئيسيّة تطبع فوق(3.5،2.0)=«1»،
    //      دون(3.5،2.0)=«0»، مطابق(2.0،2.0)=«1». يُثبت ucomisd/fcmp + seta/setae/cset + الفرعَ
    //      الاحتياطيَّ (test/jnz على x86، cmp-xzr/b.ne على ARM64؛ العوائمُ لا تُدمَج في الفرع).
    const std::string kSrcFloatCompare =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD9\x81\xD9\x88\xD9\x82(\xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A \xD8\xA3\xD8\x8C \xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A \xD8\xA8)\x0A    \xD8\xA5\xD8\xB0\xD8\xA7 \xD8\xA3 > \xD8\xA8\x0A        \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 1\x0A    \xD9\x88\xD8\xA5\xD9\x84\xD8\xA7\x0A        \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 0\x0A    \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD8\xAF\xD9\x88\xD9\x86(\xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A \xD8\xA3\xD8\x8C \xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A \xD8\xA8)\x0A    \xD8\xA5\xD8\xB0\xD8\xA7 \xD8\xA3 < \xD8\xA8\x0A        \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 1\x0A    \xD9\x88\xD8\xA5\xD9\x84\xD8\xA7\x0A        \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 0\x0A    \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD9\x85\xD8\xB7\xD8\xA7\xD8\xA8\xD9\x82(\xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A \xD8\xA3\xD8\x8C \xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A \xD8\xA8)\x0A    \xD8\xA5\xD8\xB0\xD8\xA7 \xD8\xA3 == \xD8\xA8\x0A        \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 1\x0A    \xD9\x88\xD8\xA5\xD9\x84\xD8\xA7\x0A        \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 0\x0A    \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9()\x0A    \xD8\xA7\xD8\xB7\xD8\xA8\xD8\xB9_\xD8\xB3\xD8\xB7\xD8\xB1(\xD9\x81\xD9\x88\xD9\x82(3.5\xD8\x8C 2.0))\x0A    \xD8\xA7\xD8\xB7\xD8\xA8\xD8\xB9_\xD8\xB3\xD8\xB7\xD8\xB1(\xD8\xAF\xD9\x88\xD9\x86(3.5\xD8\x8C 2.0))\x0A    \xD8\xA7\xD8\xB7\xD8\xA8\xD8\xB9_\xD8\xB3\xD8\xB7\xD8\xB1(\xD9\x85\xD8\xB7\xD8\xA7\xD8\xA8\xD9\x82(2.0\xD8\x8C 2.0))\x0A    \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 0\x0A\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";

    // (AR) مقارنةُ العوائم في سياقِ **قيمة** (لا فرع): «متغير س = أ > ب؛ اطبع_سطر(س)» — يتحقّق
    //      أنّ نتيجةَ المقارنةِ موسومةٌ Boolean (لا Float) فتُطبَع «1»/«0» عبر itoa لا «0.000000».
    //      قيمة(3.5،2.0)⇒«1»، قيمة(2.0،3.5)⇒«0». (سدُّ ثغرةِ سياقِ القيمة التي رصدتها أميليا.)
    const std::string kSrcFloatCmpValue =
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD9\x82\xD9\x8A\xD9\x85\xD8\xA9(\xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A \xD8\xA3\xD8\x8C \xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A \xD8\xA8)\x0A    \xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xB3 = \xD8\xA3 > \xD8\xA8\x0A    \xD8\xA7\xD8\xB7\xD8\xA8\xD8\xB9_\xD8\xB3\xD8\xB7\xD8\xB1(\xD8\xB3)\x0A\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9()\x0A    \xD9\x82\xD9\x8A\xD9\x85\xD8\xA9(3.5\xD8\x8C 2.0)\x0A    \xD9\x82\xD9\x8A\xD9\x85\xD8\xA9(2.0\xD8\x8C 3.5)\x0A    \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 0\x0A\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\x0A";

    // (AR) لولبُ مدًى باتّجاهٍ **ديناميّ** (النهايةُ متغيّرٌ لا حرفيّ) ⇒ الأمامُ يُصدِر عقدةَ PHI
    //      (statement_for_range.cpp:411 «range direction PHI»): الوحيدُ الذي يُمارِس PHI في اللغة.
    //      «لكل ع من 0 الى نهاية» يجمع ١ لكلّ تكرار ⇒ يُرجع عدَّ التكرارات (برهانٌ حيٌّ للحاملِ العابر للكتل).
    const std::string kSrcRangePhi =
        "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9\x20\xd8\xb1\xd8\xa6\xd9\x8a\xd8\xb3\xd9\x8a\xd8\xa9\x28\x29\x0a"
        "\x20\x20\x20\x20\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1\x20\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\x20\x3d\x20\x35\x0a"
        "\x20\x20\x20\x20\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1\x20\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\x20\x3d\x20\x30\x0a"
        "\x20\x20\x20\x20\xd9\x84\xd9\x83\xd9\x84\x20\xd8\xb9\x20\xd9\x85\xd9\x86\x20\x30\x20\xd8\xa7\xd9\x84\xd9\x89\x20\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\x0a"
        "\x20\x20\x20\x20\x20\x20\x20\x20\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\x20\x3d\x20\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\x20\x2b\x20\x31\x0a"
        "\x20\x20\x20\x20\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\x0a"
        "\x20\x20\x20\x20\xd8\xa7\xd8\xb1\xd8\xac\xd8\xb9\x20\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\x0a"
        "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\x0a";
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

// (AR) اقتطاعُ عشريّ→صحيح (F64_TO_I64 = movq+cvttsd2si) ⇒ خروج ٤٢. أوّلُ تحويلِ عشريّ أصليّ.
TEST(NativeSirBridge, FloatArithToInt)
{
    auto module = buildSir(kSrcFloatArith);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    ASSERT_TRUE(contains(bin, {0xF2, 0x48, 0x0F, 0x2C})); // cvttsd2si r64,xmm
    std::FILE *fp = std::fopen("sad_sir_float42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

// (AR) طباعةُ عشريّ (addsd + المُنسِّق) ⇒ «41.5». أوّلُ مخرَجٍ عشريٍّ مرئيّ.
TEST(NativeSirBridge, PrintsFloat)
{
    auto module = buildSir(kSrcFloatPrint);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    ASSERT_TRUE(contains(bin, {0xF2, 0x0F, 0x58})); // addsd xmm,xmm
    ASSERT_TRUE(contains(bin, {0x0F, 0x2A}));       // cvtsi2sd (أوپكودُه، أيًّا كان REX)
    std::FILE *fp = std::fopen("sad_sir_printfloat", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

// (AR) مصفوفةٌ مختلطةٌ معلَّبة (int/float) ⇒ «3.5» ثمّ «1». مسارُ التعليب الكامل بلا LLVM.
TEST(NativeSirBridge, BoxedMixedArray)
{
    auto module = buildSir(kSrcBoxedMixed);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    ASSERT_TRUE(res.code.size() > sad::native::elf::kCodeOffset);
    std::FILE *fp = std::fopen("sad_sir_boxed", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(res.code.data(), 1, res.code.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(res.code.size()));
}

// (AR) نصٌّ معلَّب في مصفوفةٍ مختلطة ⇒ «نص». مسارُ التعليب النصّيّ (واصفُ rodata) بلا LLVM.
TEST(NativeSirBridge, BoxedStringArray)
{
    auto module = buildSir(kSrcBoxedStr);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    ASSERT_TRUE(res.code.size() > sad::native::elf::kCodeOffset);
    std::FILE *fp = std::fopen("sad_sir_boxed_str", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(res.code.data(), 1, res.code.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(res.code.size()));
}

// (AR) إتمامُ الحساب والتحكّم: RET_VOID (دالّة فارغة) + NEG (سالب أحاديّ) + طباعةُ السالب + SAR.
//      يُخفَّض ويُكتب للبرهان الحيّ (متوقَّع «7» ثمّ «-42» ثمّ «-4»).
TEST(NativeSirBridge, ArithControlCompleteness)
{
    auto module = buildSir(kSrcArithComplete);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    ASSERT_TRUE(res.code.size() > sad::native::elf::kCodeOffset);
    std::FILE *fp = std::fopen("sad_sir_arith", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(res.code.data(), 1, res.code.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(res.code.size()));
}

// (AR) مقارنةُ العوائم x86: يبني+يخفّض+يكتب sad_sir_fcmp (البرهانُ الحيّ: «1»/«0»/«1»).
TEST(NativeSirBridge, FloatComparison)
{
    auto module = buildSir(kSrcFloatCompare);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    ASSERT_TRUE(res.code.size() > sad::native::elf::kCodeOffset);
    std::FILE *fp = std::fopen("sad_sir_fcmp", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(res.code.data(), 1, res.code.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(res.code.size()));
}

// (AR) مقارنةُ العوائم في سياقِ قيمةٍ x86: sad_sir_fcmpv (البرهانُ الحيّ: «1»/«0» عبر itoa).
TEST(NativeSirBridge, FloatComparisonValueContext)
{
    auto module = buildSir(kSrcFloatCmpValue);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    ASSERT_TRUE(res.code.size() > sad::native::elf::kCodeOffset);
    std::FILE *fp = std::fopen("sad_sir_fcmpv", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(res.code.data(), 1, res.code.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(res.code.size()));
}

// (AR) هل يحوي المُنتَجُ عقدةَ PHI؟ (لضمانِ أنّ الاختبارَ يُمارِس مسارَ PHI فعلًا لا يمرُّ صامتًا).
static bool moduleHasPhi(const Sad::Compiler::SIR::SIRModule &m)
{
    for (const auto &fn : m.getFunctions())
        for (const auto &bb : fn->getBasicBlocks())
            for (const auto &inst : bb->instructions)
                if (inst.opcode == Sad::Compiler::SIR::SIROpcode::PHI)
                    return true;
    return false;
}

// (AR) لولبُ مدًى ديناميّ الاتّجاه x86: يبني+يخفّض+يكتب sad_sir_rangephi. أوّلُ برهانٍ حيٍّ لـPHI:
//      الأمامُ يُصدِر عقدةَ PHI (نؤكّده)، والخلفيّةُ تحملها بخانةِ إطارٍ (خزنُ السَّلَف + قراءةُ الدامج
//      عبر مسارِ #360 الاحتياطيّ). لو انكسر الحاملُ لاختلَّ عدُّ اللولب.
TEST(NativeSirBridge, LowersDynamicRangePhi)
{
    auto module = buildSir(kSrcRangePhi);
    ASSERT_TRUE(module != nullptr);
    ASSERT_TRUE(moduleHasPhi(*module)); // (AR) تأكيدٌ أنّ المصدرَ يُمارِس PHI فعلًا
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    ASSERT_TRUE(contains(bin, {0x89, 0x45})); // (AR) mov [rbp-x], reg — تخزينُ حاملِ PHI عند السَّلَف
    ASSERT_TRUE(contains(bin, {0x48, 0x85})); // (AR) test r64,r64 — قراءةُ الحامل في فرعِ الدمج (#360)
    std::FILE *fp = std::fopen("sad_sir_rangephi", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

// (AR) طفحانُ الكسر (ترحيلُ الحمل): «2.9999998» ⇒ «3.0». إصلاحُ عائق أميليا.
TEST(NativeSirBridge, FloatCarryRounding)
{
    auto module = buildSir(kSrcFloatCarry);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    ASSERT_TRUE(res.ok);
    std::FILE *fp = std::fopen("sad_sir_carry", "wb");
    ASSERT_TRUE(fp != nullptr);
    std::fwrite(res.code.data(), 1, res.code.size(), fp);
    std::fclose(fp);
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

// ── المصفوفات (كومةُ mmap + SadArray الخماسيّ): NEW/GET/SET/LEN على x86-64 ──

// (AR) مصفوفةٌ حرفيّةٌ مفهرسةٌ مباشرةً: «ارجع [10، 20، 42][2]» ⇒ ARRAY_NEW (mmap كومة) +
//      MOVE×3 + ARRAY_SET×3 + ARRAY_GET ⇒ يخرج ٤٢. أوّلُ برنامجِ ص ذي كومةِ mmap ومصفوفةٍ.
TEST(NativeSirBridge, LowersArrayLiteralIndex)
{
    //  ارجع [10، 20، 42][2]   (، = D8 8C)
    auto module = buildSir(mkReturn("[10\xD8\x8C 20\xD8\x8C 42][2]"));
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("array lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    ASSERT_TRUE(contains(bin, {0x0F, 0x05})); // syscall (mmap)
    std::FILE *fp = std::fopen("sad_sir_array42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

// (AR) طولُ مصفوفةٍ: «ارجع طول([1، 2، 3]) * 14» ⇒ ARRAY_LEN (الحقل ٠) × ١٤ = ٤٢. يُثبت ARRAY_LEN.
TEST(NativeSirBridge, LowersArrayLength)
{
    //  ارجع طول([1، 2، 3]) * 14      طول = D8 B7 D9 88 D9 84
    auto module = buildSir(mkReturn("\xD8\xB7\xD9\x88\xD9\x84([1\xD8\x8C 2\xD8\x8C 3]) * 14"));
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("array-len lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    std::FILE *fp = std::fopen("sad_sir_arraylen42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

// (AR) فحصُ الحدّ: «ارجع [1، 2، 3][5]» ⇒ الفهرسُ ٥ ≥ الطولِ ٣ ⇒ يُخفَّضُ بنجاحٍ (فحصٌ زمنَ
//      التشغيل لا خطأُ ترجمة) ويخرجُ الثنائيُّ بالرمز ١٣٤ (هلعُ الحدّ) لا ٤٢. برهانٌ حيٌّ في .sh.
TEST(NativeSirBridge, ArrayOutOfBoundsTraps)
{
    auto module = buildSir(mkReturn("[1\xD8\x8C 2\xD8\x8C 3][5]"));
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("array-oob lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok); // (AR) يُخفَّضُ بنجاح؛ التجاوزُ يُمسَك زمنَ التشغيل لا ترجمةً
    const auto &bin = res.code;
    ASSERT_TRUE(contains(bin, {0x0F, 0x05})); // syscall (mmap + exit-الهلع)
    std::FILE *fp = std::fopen("sad_sir_arrayoob", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

// (AR) الإلحاق (مسارُ النموّ): «م=[40]؛ م.أضف(2)؛ ارجع م[0]+م[1]» ⇒ السعةُ ١→٢ عبر mmap
//      جديدٍ + نسخِ خانةٍ، ثمّ تخزينُ ٢ ⇒ [40،2] ⇒ ٤٢. يُثبت نموَّ cap ولولبَ النسخ.
static const char *kSrcAppendGrow =
    "دالة رئيسية()\n"
    "    متغير م = [40]\n"
    "    م.أضف(2)\n"
    "    ارجع م[0] + م[1]\n"
    "نهاية\n";
// (AR) الإلحاق (نموّ + بلا نموّ): «م=[6]؛ ثلاثُ إضافاتٍ» ⇒ [6](cap1)→نموّ(cap2)→نموّ(cap4،len3)
//      →بلا نموّ(len4). يُثبت مسارَ jl (L<C ⇒ تخزينٌ بلا mmap). م[3]=6 ⇒ ٦×٧=٤٢.
static const char *kSrcAppendNoGrow =
    "دالة رئيسية()\n"
    "    متغير م = [6]\n"
    "    م.أضف(6)\n"
    "    م.أضف(6)\n"
    "    م.أضف(6)\n"
    "    ارجع م[3] * 7\n"
    "نهاية\n";

TEST(NativeSirBridge, LowersArrayAppendGrow)
{
    auto module = buildSir(kSrcAppendGrow);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("append-grow lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(contains(bin, {0x0F, 0x05})); // syscall (mmap للنموّ)
    std::FILE *fp = std::fopen("sad_sir_append42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

TEST(NativeSirBridge, LowersArrayAppendNoGrow)
{
    auto module = buildSir(kSrcAppendNoGrow);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("append-nogrow lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    std::FILE *fp = std::fopen("sad_sir_appendnogrow42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(res.code.data(), 1, res.code.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(res.code.size()));
}

// ════════════════════════════════════════════════════════════════════════════
// (AR) الدفعة ٤ (الصفوف + امتدادُ المصفوفات): TUPLE_NEW/GET/LEN (توجيهٌ لبنية المصفوفة)
//      + ARRAY_REMOVE (حذفٌ في المكان) + ARRAY_CONCAT (دمجٌ ساكنٌ tags=null) + ARRAY_ZIP
//      (حلقةٌ ذاتُ mmap داخليٍّ لكلّ زوج). كلُّها تُعيد استعمالَ ترميزاتٍ قائمةٍ (لا ترميزَ جديد).
// ════════════════════════════════════════════════════════════════════════════

// (AR) صفٌّ مفهرَس: «ص=(40، 2)؛ ارجع ص[0]+ص[1]» ⇒ TUPLE_NEW (mmap، يشاركُ بنيةَ المصفوفة) +
//      قراءةٌ بالفهرس ⇒ ٤٢. TUPLE_GET/TUPLE_LEN موجَّهان لنفسِ شيفرة ARRAY_GET/ARRAY_LEN المُبرهَنة
//      حيًّا (صحّةٌ بالبناء)؛ باعثُ TUPLE_GET الأماميُّ الوحيد (تفكيكُ الصفّ) فيه فجوةُ STORE-nonslot
//      غيرُ متعلّقةٍ بهذه الدفعة، وTUPLE_LEN بلا باعثٍ أماميّ (ميّتٌ سطحيًّا).
static const char *kSrcTuple =
    "دالة رئيسية()\n"
    "    متغير ص = (40، 2)\n"
    "    ارجع ص[0] + ص[1]\n"
    "نهاية\n";
// (AR) حذفٌ بفهرسٍ في المكان: «م=[10، 99، 32]؛ إزالة_عنصر(م، 1)؛ ارجع م[0]+م[1]» ⇒ إزاحةُ 32 لمكان
//      99 (حذفُ الفهرس ١) ⇒ 10+32=٤٢. الدالّةُ الحرّةُ إزالة_عنصر تبعث BUILTIN_ARRAY_REMOVE بمعاملَين.
//      الفهرسُ محسوبٌ (٢-١) ⇒ مؤقّتٌ في سجلّ حوضٍ يمارس مسارَ قراءةِ الفهرس (لا مسارَ الثابت) —
//      يمسك فخَّ ترتيبِ قراءةِ المعاملين على ARM64.
static const char *kSrcArrayRemove =
    "دالة رئيسية()\n"
    "    متغير م = [10، 99، 32]\n"
    "    متغير ف = 3\n"
    "    إزالة_عنصر(م، ف - 2)\n"
    "    ارجع م[0] + م[1]\n"
    "نهاية\n";
// (AR) دمجٌ: «م=[40]+[2]؛ ارجع م[0]+م[1]» ⇒ مصفوفةٌ [40،2] بـmmap واحد ⇒ ٤٢.
static const char *kSrcArrayConcat =
    "دالة رئيسية()\n"
    "    متغير م = [40] + [2]\n"
    "    ارجع م[0] + م[1]\n"
    "نهاية\n";
// (AR) زاوج + طول: «طول(زاوج([1،2،3]،[4،5]))*21» ⇒ الطول=min(3،2)=2 ⇒ 2×21=٤٢. يُثبت الحلقةَ+التخصيص.
static const char *kSrcArrayZip =
    "دالة رئيسية()\n"
    "    ارجع طول(زاوج([1، 2، 3]، [4، 5])) * 21\n"
    "نهاية\n";

TEST(NativeSirBridge, LowersTupleConstruct)
{
    auto module = buildSir(kSrcTuple);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("tuple lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(contains(bin, {0x0F, 0x05})); // syscall (mmap للصفّ)
    std::FILE *fp = std::fopen("sad_sir_tuple42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

TEST(NativeSirBridge, LowersArrayRemove)
{
    auto module = buildSir(kSrcArrayRemove);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("array-remove lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    std::FILE *fp = std::fopen("sad_sir_remove42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(res.code.data(), 1, res.code.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(res.code.size()));
}

TEST(NativeSirBridge, LowersArrayConcat)
{
    auto module = buildSir(kSrcArrayConcat);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("array-concat lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(contains(bin, {0x0F, 0x05})); // syscall (mmap للناتج)
    std::FILE *fp = std::fopen("sad_sir_concat42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

TEST(NativeSirBridge, LowersArrayZipLength)
{
    auto module = buildSir(kSrcArrayZip);
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("array-zip lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(contains(bin, {0x0F, 0x05})); // syscall (mmap الناتج + الأزواج)
    std::FILE *fp = std::fopen("sad_sir_zip42", "wb");
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

// (AR) مصفوفةٌ حرفيّةٌ مفهرسةٌ على ARM64: «ارجع [10، 20، 42][2]» ⇒ ARRAY_NEW (mmap svc) +
//      ARRAY_SET×3 + ARRAY_GET ⇒ يخرج ٤٢ على qemu. يُثبت كومةَ mmap + str/ldr بقاعدةِ سجلّ.
TEST(Arm64SirBridge, LowersArrayLiteralIndex)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(mkReturn("[10\xD8\x8C 20\xD8\x8C 42][2]"), "sad_arm64_array42", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) اقتطاعُ عشريّ→صحيح على ARM64 (fmov+fcvtzs) ⇒ ٤٢ على qemu.
TEST(Arm64SirBridge, FloatArithToInt)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcFloatArith, "sad_arm64_float42", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) طباعةُ عشريّ على ARM64 (fadd + المُنسِّق) ⇒ «41.5» على qemu.
TEST(Arm64SirBridge, PrintsFloat)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcFloatPrint, "sad_arm64_printfloat", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) مصفوفةٌ مختلطةٌ معلَّبة على ARM64 ⇒ «3.5» ثمّ «1» على qemu. مرآةُ x86 للتعليب.
TEST(Arm64SirBridge, BoxedMixedArray)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcBoxedMixed, "sad_arm64_boxed", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) نصٌّ معلَّب على ARM64 ⇒ «نص» على qemu. مرآةُ x86 للتعليب النصّيّ (واصفُ rodata).
TEST(Arm64SirBridge, BoxedStringArray)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcBoxedStr, "sad_arm64_boxed_str", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) إتمامُ الحساب والتحكّم على ARM64 ⇒ «7»/«-42»/«-4» على qemu. مرآةُ x86 (RET_VOID/NEG/SAR/سالب).
TEST(Arm64SirBridge, ArithControlCompleteness)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcArithComplete, "sad_arm64_arith", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) مقارنةُ العوائم ARM64: يبني+يخفّض+يكتب sad_arm64_fcmp (البرهانُ الحيّ: «1»/«0»/«1»).
TEST(Arm64SirBridge, FloatComparison)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcFloatCompare, "sad_arm64_fcmp", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) مقارنةُ العوائم في سياقِ قيمةٍ ARM64: sad_arm64_fcmpv (البرهانُ الحيّ: «1»/«0»).
TEST(Arm64SirBridge, FloatComparisonValueContext)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcFloatCmpValue, "sad_arm64_fcmpv", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) طفحانُ الكسر على ARM64: «2.9999998» ⇒ «3.0». إصلاحُ عائق أميليا.
TEST(Arm64SirBridge, FloatCarryRounding)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcFloatCarry, "sad_arm64_carry", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) طولُ مصفوفةٍ على ARM64: «ارجع طول([1، 2، 3]) * 14» ⇒ ARRAY_LEN × ١٤ = ٤٢. يُثبت ARRAY_LEN.
TEST(Arm64SirBridge, LowersArrayLength)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(mkReturn("\xD8\xB7\xD9\x88\xD9\x84([1\xD8\x8C 2\xD8\x8C 3]) * 14"),
                       "sad_arm64_arraylen42", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) فحصُ الحدّ على ARM64: «ارجع [1، 2، 3][5]» ⇒ الفهرسُ خارجَ الحدّ ⇒ يخرجُ بالرمز ١٣٤ على qemu.
TEST(Arm64SirBridge, ArrayOutOfBoundsTraps)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(mkReturn("[1\xD8\x8C 2\xD8\x8C 3][5]"), "sad_arm64_arrayoob", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) الإلحاق على ARM64 (مسارُ النموّ): «م=[40]؛ م.أضف(2)؛ ارجع م[0]+م[1]» ⇒ ٤٢ على qemu.
TEST(Arm64SirBridge, LowersArrayAppendGrow)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcAppendGrow, "sad_arm64_append42", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) الإلحاق على ARM64 (نموّ + بلا نموّ): «م=[6]؛ ثلاثُ إضافات؛ م[3]×7» ⇒ ٤٢ على qemu.
TEST(Arm64SirBridge, LowersArrayAppendNoGrow)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcAppendNoGrow, "sad_arm64_appendnogrow42", &ok, &sz);
    ASSERT_TRUE(ok);
}

// (AR) الدفعة ٤ على ARM64 (مرآةُ x86): صفٌّ + حذفٌ + دمجٌ + زاوج. تُبرهَن حيًّا على qemu بخروج ٤٢.
TEST(Arm64SirBridge, LowersTupleConstruct)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcTuple, "sad_arm64_tuple42", &ok, &sz);
    ASSERT_TRUE(ok);
}

TEST(Arm64SirBridge, LowersArrayRemove)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcArrayRemove, "sad_arm64_remove42", &ok, &sz);
    ASSERT_TRUE(ok);
}

TEST(Arm64SirBridge, LowersArrayConcat)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcArrayConcat, "sad_arm64_concat42", &ok, &sz);
    ASSERT_TRUE(ok);
}

TEST(Arm64SirBridge, LowersArrayZipLength)
{
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcArrayZip, "sad_arm64_zip42", &ok, &sz);
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

// (AR) لولبُ مدًى ديناميّ الاتّجاه ARM64: يخفّض PHI عبر خانةِ إطارٍ (strSlot عند السَّلَف + قراءةٌ
//      في فرعِ الدمج عبر مسارِ #360 الاحتياطيّ cmp-xzr/b.ne). برهانٌ حيّ qemu (عدُّ اللولب).
TEST(Arm64SirBridge, LowersDynamicRangePhi)
{
    auto module = buildSir(kSrcRangePhi);
    ASSERT_TRUE(module != nullptr);
    ASSERT_TRUE(moduleHasPhi(*module)); // (AR) تأكيدٌ أنّ المصدرَ يُمارِس PHI فعلًا
    bool ok = false;
    size_t sz = 0;
    lowerArm64AndWrite(kSrcRangePhi, "sad_arm64_rangephi", &ok, &sz);
    ASSERT_TRUE(ok);
}

// ════════════════════════════════════════════════════════════════════════════
// (AR) نواةُ الكومة الأصليّة (الدفعة ٢): ALLOC_HEAP/FREE/PTR_ADD/PTR_CAST/MEMSET/MEMCPY.
//      SIR مبنيٌّ يدويًّا (كـBackwardJumpNegativeFixup) لأنّ الأوپكوداتِ العاريةَ لا يولّدها
//      الأمامُ (الشقيقُ FFI حيّ، والعاري ميّت) ⇒ نُمارِسها مباشرةً. البرنامجُ يُثبت الصحّةَ حيًّا:
//        va=خانة؛ خزّن 42→va؛ vb=خانة؛ vh=حجز(64)؛ pa=&va؛ pb=&vb؛ ph=(مؤشّر)vh؛
//        عبّئ(ph,7,8) [كتابةٌ للكومة]؛ pc=(مؤشّر)pa؛ pd=pb+0؛ عبّئ(pd,0,8)؛ انسخ(pd,pc,1)؛
//        حرّر(vh)؛ r=vb؛ ارجع r.
//      المخرَجُ ٤٢ ⇒ النسخُ نقل البايتَ صحيحًا، والكومةُ خُصِّصت وكُتِبت وحُرِّرت بلا انهيار.
// ════════════════════════════════════════════════════════════════════════════
namespace
{
    std::shared_ptr<Sad::Compiler::SIR::SIRModule> buildHeapMemModule()
    {
        using namespace Sad::Compiler::SIR;
        using Sad::Types::SadTypeKind;
        auto mod = std::make_shared<SIRModule>("heapmem");
        auto fn = std::make_shared<SIRFunction>(
            "\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9", SadTypeKind::Integer); // رئيسية
        auto e = std::make_shared<SIRBasicBlock>("entry");
        auto R = [](const char *n, SadTypeKind t) { return SIROperand::Register(n, t); };
        auto I = [](int64_t v) { return SIROperand::ConstantI64(v); };
        const SadTypeKind kInt = SadTypeKind::Integer;
        const SadTypeKind kPtr = SadTypeKind::Pointer;
        e->addInstruction(SIRInstruction::Alloc(R("va", kInt), kInt, I(8)));
        e->addInstruction(SIRInstruction::Store(I(42), R("va", kInt)));
        e->addInstruction(SIRInstruction::Alloc(R("vb", kInt), kInt, I(8)));
        e->addInstruction(SIRInstruction(SIROpcode::ALLOC_HEAP, R("vh", kPtr), {I(64)}));
        e->addInstruction(SIRInstruction(SIROpcode::ADDR, R("pa", kPtr), {R("va", kInt)}));
        e->addInstruction(SIRInstruction(SIROpcode::ADDR, R("pb", kPtr), {R("vb", kInt)}));
        e->addInstruction(SIRInstruction(SIROpcode::PTR_CAST, R("ph", kPtr), {R("vh", kPtr)}));
        e->addInstruction(SIRInstruction(SIROpcode::MEMSET, std::nullopt, {R("ph", kPtr), I(7), I(8)}));
        e->addInstruction(SIRInstruction(SIROpcode::PTR_CAST, R("pc", kPtr), {R("pa", kPtr)}));
        e->addInstruction(SIRInstruction(SIROpcode::PTR_ADD, R("pd", kPtr), {R("pb", kPtr), I(0)}));
        e->addInstruction(SIRInstruction(SIROpcode::MEMSET, std::nullopt, {R("pd", kPtr), I(0), I(8)}));
        e->addInstruction(SIRInstruction(SIROpcode::MEMCPY, std::nullopt, {R("pd", kPtr), R("pc", kPtr), I(1)}));
        e->addInstruction(SIRInstruction(SIROpcode::FREE, std::nullopt, {R("vh", kPtr)}));
        e->addInstruction(SIRInstruction::Load(R("r", kInt), R("vb", kInt)));
        e->addInstruction(SIRInstruction::Return(R("r", kInt)));
        fn->addBasicBlock(e);
        mod->addFunction(fn);
        return mod;
    }

    // (AR) وحدةُ الدفعة ٣ (النصوصُ الديناميّة، الطبقتان ٠+١): تمارِس STRING_NEW/BOOL_TO_STRING/
    //      STRING_LEN/STRING_CMP/STRING_TO_I64 بقراءةٍ عكسيّةٍ تُرجِع ٤٢:
    //        n=TO_I64("40")=40 ؛ c1=CMP(NEW("k"),"k")=1 ؛ c0=CMP("a","b")=0 ؛
    //        (LEN("abcd") − LEN("abc"))=1 ؛ (LEN("ص") − 1)=0 [UTF-8 مضاعفٌ ⇒ نقطةُ رمزٍ واحدة] ؛
    //        (CMP(BOOL_TO_STRING(1),"صحيح") − 1)=0  ⇒  40+1+0+1+0+0 = 42.
    std::shared_ptr<Sad::Compiler::SIR::SIRModule> buildStringModule()
    {
        using namespace Sad::Compiler::SIR;
        using Sad::Types::SadTypeKind;
        auto mod = std::make_shared<SIRModule>("dynstr");
        auto fn = std::make_shared<SIRFunction>(
            "\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9", SadTypeKind::Integer); // رئيسية
        auto e = std::make_shared<SIRBasicBlock>("entry");
        auto R = [](const char *n, SadTypeKind t) { return SIROperand::Register(n, t); };
        auto I = [](int64_t v) { return SIROperand::ConstantI64(v); };
        auto S = [](const char *s) { return SIROperand::ConstantString(s); };
        auto F = [](double v) { return SIROperand::ConstantF64(v); };
        const SadTypeKind kInt = SadTypeKind::Integer;
        const SadTypeKind kStr = SadTypeKind::String;
        const SadTypeKind kFloat = SadTypeKind::Float;
        // n = TO_I64("40") = 40
        e->addInstruction(SIRInstruction(SIROpcode::STRING_TO_I64, R("n", kInt), {S("40")}));
        // c1 = CMP(NEW("k"), "k") = 1 ؛ t = n + c1 = 41
        e->addInstruction(SIRInstruction(SIROpcode::STRING_NEW, R("k1", kStr), {S("k")}));
        e->addInstruction(SIRInstruction(SIROpcode::STRING_CMP, R("c1", kInt), {R("k1", kStr), S("k")}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("t", kInt), {R("n", kInt), R("c1", kInt)}));
        // c0 = CMP("a","b") = 0 ؛ t += c0
        e->addInstruction(SIRInstruction(SIROpcode::STRING_CMP, R("c0", kInt), {S("a"), S("b")}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("t", kInt), {R("t", kInt), R("c0", kInt)}));
        // d = LEN("abcd") − LEN("abc") = 1 ؛ t += d
        e->addInstruction(SIRInstruction(SIROpcode::STRING_LEN, R("l4", kInt), {S("abcd")}));
        e->addInstruction(SIRInstruction(SIROpcode::STRING_LEN, R("l3", kInt), {S("abc")}));
        e->addInstruction(SIRInstruction(SIROpcode::SUB_I64, R("d", kInt), {R("l4", kInt), R("l3", kInt)}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("t", kInt), {R("t", kInt), R("d", kInt)}));
        // e2 = LEN("ص") − 1 = 0 (UTF-8: بايتان ⇒ نقطةُ رمزٍ واحدة) ؛ t += e2
        e->addInstruction(SIRInstruction(SIROpcode::STRING_LEN, R("ls", kInt), {S("\xD8\xB5")}));
        e->addInstruction(SIRInstruction(SIROpcode::SUB_I64, R("e2", kInt), {R("ls", kInt), I(1)}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("t", kInt), {R("t", kInt), R("e2", kInt)}));
        // f = CMP(BOOL_TO_STRING(1), "صحيح") − 1 = 0 ؛ t += f
        e->addInstruction(SIRInstruction(SIROpcode::BOOL_TO_STRING, R("bt", kStr), {I(1)}));
        e->addInstruction(SIRInstruction(SIROpcode::STRING_CMP, R("cb", kInt),
                                         {R("bt", kStr), S("\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD")})); // صحيح
        e->addInstruction(SIRInstruction(SIROpcode::SUB_I64, R("f", kInt), {R("cb", kInt), I(1)}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("t", kInt), {R("t", kInt), R("f", kInt)}));
        // I64_TO_STRING(-7)="-7" (المسارُ السالب) ⇒ round-trip TO_I64=-7 + LEN=2 ؛ net = -7+2+5 = 0
        e->addInstruction(SIRInstruction(SIROpcode::I64_TO_STRING, R("sr", kStr), {I(-7)}));
        e->addInstruction(SIRInstruction(SIROpcode::STRING_TO_I64, R("g", kInt), {R("sr", kStr)}));
        e->addInstruction(SIRInstruction(SIROpcode::STRING_LEN, R("h", kInt), {R("sr", kStr)}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("z", kInt), {R("g", kInt), R("h", kInt)}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("z2", kInt), {R("z", kInt), I(5)}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("t", kInt), {R("t", kInt), R("z2", kInt)}));
        // CONCAT(I64_TO_STRING(2), "5") = "25" (المعاملُ الأوّلُ نصٌّ محسوبٌ على الكومة) ⇒ TO_I64=25،
        // LEN=2 ؛ net = 25 - 2 - 23 = 0 → يُبقي ٤٢
        e->addInstruction(SIRInstruction(SIROpcode::I64_TO_STRING, R("two", kStr), {I(2)}));
        e->addInstruction(SIRInstruction(SIROpcode::STRING_CONCAT, R("cc", kStr), {R("two", kStr), S("5")}));
        e->addInstruction(SIRInstruction(SIROpcode::STRING_TO_I64, R("cv", kInt), {R("cc", kStr)}));
        e->addInstruction(SIRInstruction(SIROpcode::STRING_LEN, R("cl", kInt), {R("cc", kStr)}));
        e->addInstruction(SIRInstruction(SIROpcode::SUB_I64, R("cz", kInt), {R("cv", kInt), R("cl", kInt)}));
        e->addInstruction(SIRInstruction(SIROpcode::SUB_I64, R("cz2", kInt), {R("cz", kInt), I(23)}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("t", kInt), {R("t", kInt), R("cz2", kInt)}));
        // SUBSTR("12345",1,3)="234" (ASCII) ⇒ TO_I64=234، LEN=3 ؛ net = 234-3-231 = 0
        e->addInstruction(SIRInstruction(SIROpcode::STRING_SUBSTR, R("ss", kStr), {S("12345"), I(1), I(3)}));
        e->addInstruction(SIRInstruction(SIROpcode::STRING_TO_I64, R("sv", kInt), {R("ss", kStr)}));
        e->addInstruction(SIRInstruction(SIROpcode::STRING_LEN, R("sl", kInt), {R("ss", kStr)}));
        e->addInstruction(SIRInstruction(SIROpcode::SUB_I64, R("sz", kInt), {R("sv", kInt), R("sl", kInt)}));
        e->addInstruction(SIRInstruction(SIROpcode::SUB_I64, R("sz2", kInt), {R("sz", kInt), I(231)}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("t", kInt), {R("t", kInt), R("sz2", kInt)}));
        // SUBSTR("صاد",1,2)="اد" (UTF-8: تخطّي نقطةِ رمزٍ = بايتان، أخذُ نقطتين = ٤ بايت) ⇒ LEN=2 ؛ net=0
        e->addInstruction(SIRInstruction(SIROpcode::STRING_SUBSTR, R("us", kStr),
                                         {S("\xD8\xB5\xD8\xA7\xD8\xAF"), I(1), I(2)})); // صاد
        e->addInstruction(SIRInstruction(SIROpcode::STRING_LEN, R("ul", kInt), {R("us", kStr)}));
        e->addInstruction(SIRInstruction(SIROpcode::SUB_I64, R("uz", kInt), {R("ul", kInt), I(2)}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("t", kInt), {R("t", kInt), R("uz", kInt)}));
        // FIND("abcde","cd")=2 (فهرسُ نقطةِ رمز) ؛ FIND("abc","z")=-1 ؛ net = (2) + (-1) + (-1) = 0
        e->addInstruction(SIRInstruction(SIROpcode::STRING_FIND, R("fi", kInt), {S("abcde"), S("cd")}));
        e->addInstruction(SIRInstruction(SIROpcode::STRING_FIND, R("fn", kInt), {S("abc"), S("z")}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("fz", kInt), {R("fi", kInt), R("fn", kInt)}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("fz2", kInt), {R("fz", kInt), R("fn", kInt)}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("t", kInt), {R("t", kInt), R("fz2", kInt)}));
        // FIND UTF-8: FIND("صاد","اد")=1 (فهرسُ نقطةِ رمز، لا بايت) ؛ net = (1 - 1) = 0
        e->addInstruction(SIRInstruction(SIROpcode::STRING_FIND, R("uf", kInt),
                                         {S("\xD8\xB5\xD8\xA7\xD8\xAF"), S("\xD8\xA7\xD8\xAF")})); // "صاد","اد"
        e->addInstruction(SIRInstruction(SIROpcode::SUB_I64, R("ufz", kInt), {R("uf", kInt), I(1)}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("t", kInt), {R("t", kInt), R("ufz", kInt)}));
        // REPLACE("1X3","X","2")="123" (وجود) ⇒ TO_I64=123 ؛ net = 123-123 = 0
        e->addInstruction(SIRInstruction(SIROpcode::STRING_REPLACE, R("rp", kStr), {S("1X3"), S("X"), S("2")}));
        e->addInstruction(SIRInstruction(SIROpcode::STRING_TO_I64, R("rv", kInt), {R("rp", kStr)}));
        e->addInstruction(SIRInstruction(SIROpcode::SUB_I64, R("rz", kInt), {R("rv", kInt), I(123)}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("t", kInt), {R("t", kInt), R("rz", kInt)}));
        // REPLACE("abc","z","Q")="abc" (غياب ⇒ نسخةُ s) ⇒ LEN=3 ؛ net = 3-3 = 0
        e->addInstruction(SIRInstruction(SIROpcode::STRING_REPLACE, R("rn", kStr), {S("abc"), S("z"), S("Q")}));
        e->addInstruction(SIRInstruction(SIROpcode::STRING_LEN, R("rnl", kInt), {R("rn", kStr)}));
        e->addInstruction(SIRInstruction(SIROpcode::SUB_I64, R("rnz", kInt), {R("rnl", kInt), I(3)}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("t", kInt), {R("t", kInt), R("rnz", kInt)}));
        // ── الطبقة ٥: الزوجُ العشريّ ──
        // F64_TO_STRING(42.0)="42.0" (حذفُ صفرٍ زائد؛ '.' يليها) ⇒ STRING_TO_I64="42" (يقف عند النقطة) ؛ net = 42-42 = 0
        e->addInstruction(SIRInstruction(SIROpcode::F64_TO_STRING, R("fs0", kStr), {F(42.0)}));
        e->addInstruction(SIRInstruction(SIROpcode::STRING_TO_I64, R("fv0", kInt), {R("fs0", kStr)}));
        e->addInstruction(SIRInstruction(SIROpcode::SUB_I64, R("fz0", kInt), {R("fv0", kInt), I(42)}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("t", kInt), {R("t", kInt), R("fz0", kInt)}));
        // ذهابٌ وإيابٌ موجب: F64_TO_STRING(2.5)="2.5" → STRING_TO_F64=2.5 → ×2.0=5.0 → F64_TO_I64=5 ؛ net = 5-5 = 0
        // (يثبتُ نجاةَ الكسر «.5» في المسارَين معًا)
        e->addInstruction(SIRInstruction(SIROpcode::F64_TO_STRING, R("s25", kStr), {F(2.5)}));
        e->addInstruction(SIRInstruction(SIROpcode::STRING_TO_F64, R("f25", kFloat), {R("s25", kStr)}));
        e->addInstruction(SIRInstruction(SIROpcode::MUL_F64, R("d25", kFloat), {R("f25", kFloat), F(2.0)}));
        e->addInstruction(SIRInstruction(SIROpcode::F64_TO_I64, R("i25", kInt), {R("d25", kFloat)}));
        e->addInstruction(SIRInstruction(SIROpcode::SUB_I64, R("z25", kInt), {R("i25", kInt), I(5)}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("t", kInt), {R("t", kInt), R("z25", kInt)}));
        // ذهابٌ وإيابٌ سالب: F64_TO_STRING(-1.5)="-1.5" → STRING_TO_F64=-1.5 → ×-2.0=3.0 → F64_TO_I64=3 ؛ net = 3-3 = 0
        // (يثبتُ الإشارةَ السالبةَ في المُنسِّق والمحلِّل معًا)
        e->addInstruction(SIRInstruction(SIROpcode::F64_TO_STRING, R("sn", kStr), {F(-1.5)}));
        e->addInstruction(SIRInstruction(SIROpcode::STRING_TO_F64, R("fn2", kFloat), {R("sn", kStr)}));
        e->addInstruction(SIRInstruction(SIROpcode::MUL_F64, R("dn", kFloat), {R("fn2", kFloat), F(-2.0)}));
        e->addInstruction(SIRInstruction(SIROpcode::F64_TO_I64, R("in", kInt), {R("dn", kFloat)}));
        e->addInstruction(SIRInstruction(SIROpcode::SUB_I64, R("zn", kInt), {R("in", kInt), I(3)}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("t", kInt), {R("t", kInt), R("zn", kInt)}));
        // محلِّلٌ مباشرٌ (حرفيٌّ لا من المُنسِّق): STRING_TO_F64("7.25")=7.25 → ×4.0=29.0 → F64_TO_I64=29 ؛ net = 29-29 = 0
        e->addInstruction(SIRInstruction(SIROpcode::STRING_TO_F64, R("fl", kFloat), {S("7.25")}));
        e->addInstruction(SIRInstruction(SIROpcode::MUL_F64, R("dl", kFloat), {R("fl", kFloat), F(4.0)}));
        e->addInstruction(SIRInstruction(SIROpcode::F64_TO_I64, R("il", kInt), {R("dl", kFloat)}));
        e->addInstruction(SIRInstruction(SIROpcode::SUB_I64, R("zl", kInt), {R("il", kInt), I(29)}));
        e->addInstruction(SIRInstruction(SIROpcode::ADD_I64, R("t", kInt), {R("t", kInt), R("zl", kInt)}));
        e->addInstruction(SIRInstruction::Return(R("t", kInt)));
        fn->addBasicBlock(e);
        mod->addFunction(fn);
        return mod;
    }
} // namespace

// (AR) x86-64: نواةُ الكومة تُخفَّض، الـELF سليم، والبايتاتُ تحوي syscall (mmap/munmap = 0F 05)
//      وتحميلَ بايتٍ (mov r8b,[rsi] = 8A) — الترميزُ الجديد. تُكتب لبرهانِ خروجِ ٤٢ حيًّا.
TEST(NativeSirBridge, LowersHeapMemCore)
{
    auto module = buildHeapMemModule();
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    ASSERT_EQ(int(bin[18]) | (int(bin[19]) << 8), 62); // EM_X86_64
    ASSERT_TRUE(contains(bin, {0x0F, 0x05})); // syscall (mmap للحجز + munmap للتحرير)
    ASSERT_TRUE(contains(bin, {0x8A}));       // mov r8b,[rsi] — حِملُ بايتٍ في حلقةِ النسخ (0x8A)

    std::FILE *fp = std::fopen("sad_sir_heapmem42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

// (AR) AArch64: نفسُ نواةِ الكومة تُخفَّض لـARM64 (svc للحجز/التحرير + ldrb لحلقةِ النسخ).
//      e_machine=EM_AARCH64=183. تُكتب لبرهانِ خروجِ ٤٢ على qemu-aarch64.
TEST(Arm64SirBridge, LowersHeapMemCore)
{
    auto module = buildHeapMemModule();
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElfArm64(*module);
    if (!res.ok)
        std::printf("arm64 lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    ASSERT_EQ(int(bin[18]) | (int(bin[19]) << 8), 183); // EM_AARCH64
    ASSERT_TRUE(contains(bin, {0x40, 0x39})); // ldrb Wt,[Xn] (0x394000xx ⇒ بايتا 0x40,0x39)

    std::FILE *fp = std::fopen("sad_arm64_heapmem42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

// (AR) x86-64: النصوصُ الديناميّة (الطبقتان ٠+١) تُخفَّض، الـELF سليم، والبايتاتُ تحوي imul
//      (0F AF، من تطبيقِ الإشارة/الضربِ في STRING_TO_I64) وحِملَ بايتٍ (0x8A، مسحُ LEN/CMP).
//      تُكتب لبرهانِ خروجِ ٤٢ حيًّا (يمارِس NEW/BOOL_TO_STRING/LEN/CMP/TO_I64 مجتمعةً).
TEST(NativeSirBridge, LowersDynamicStrings)
{
    auto module = buildStringModule();
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElf(*module);
    if (!res.ok)
        std::printf("lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    ASSERT_EQ(int(bin[18]) | (int(bin[19]) << 8), 62); // EM_X86_64
    ASSERT_TRUE(contains(bin, {0x0F, 0xAF})); // imul r64,r64 (تطبيقُ الإشارة/الضربِ في TO_I64)
    ASSERT_TRUE(contains(bin, {0x0F, 0xB6})); // movzx r64,m8 — حِملُ بايتٍ ممدَّدٍ بالصفر (مسحُ LEN/CMP/TO_I64)
    ASSERT_TRUE(contains(bin, {0x0F, 0x05})); // syscall — mmap لتخصيصِ مخزنِ I64_TO_STRING على الكومة
    ASSERT_TRUE(contains(bin, {0xF2, 0x0F, 0x5E})); // divsd xmm,xmm — قسمةُ الكسر في STRING_TO_F64 (الطبقة ٥)

    std::FILE *fp = std::fopen("sad_sir_dynstr42", "wb");
    ASSERT_TRUE(fp != nullptr);
    size_t wrote = std::fwrite(bin.data(), 1, bin.size(), fp);
    std::fclose(fp);
    ASSERT_EQ(int(wrote), int(bin.size()));
}

// (AR) AArch64: نفسُ وحدةِ النصوص تُخفَّض لـARM64 (ldrb لمسحِ LEN/CMP + mul لتطبيق الإشارة).
//      e_machine=EM_AARCH64=183. تُكتب لبرهانِ خروجِ ٤٢ على qemu-aarch64.
TEST(Arm64SirBridge, LowersDynamicStrings)
{
    auto module = buildStringModule();
    ASSERT_TRUE(module != nullptr);
    auto res = sad::native::lowerModuleToElfArm64(*module);
    if (!res.ok)
        std::printf("arm64 lowering error: %s\n", res.message().c_str());
    ASSERT_TRUE(res.ok);
    const auto &bin = res.code;
    ASSERT_TRUE(bin.size() > sad::native::elf::kCodeOffset);
    ASSERT_EQ(int(bin[18]) | (int(bin[19]) << 8), 183); // EM_AARCH64
    ASSERT_TRUE(contains(bin, {0x40, 0x39})); // ldrb Wt,[Xn] (مسحُ LEN/CMP)
    ASSERT_TRUE(contains(bin, {0x62, 0x9E})); // scvtf Dd,Xn (نصفُ الكلمةِ الأعلى LE) — تراكمُ STRING_TO_F64 (الطبقة ٥)

    std::FILE *fp = std::fopen("sad_arm64_dynstr42", "wb");
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
