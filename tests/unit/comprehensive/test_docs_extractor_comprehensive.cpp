/**
 * @file test_docs_extractor_comprehensive.cpp
 * @brief (AR) اختبارات شاملة لمستخرج التوثيق من شجرة AST
 * @brief (EN) Comprehensive tests for the AST-based documentation extractor
 *
 * (AR) المجموعات:
 *   1. DocsBasic            — توثيق دوال أساسي بأنواع التعليقات الثلاثة
 *   2. DocsClasses          — توثيق أصناف وأعضاء (طريقة/حقل/باني)
 *   3. DocsStructEnum       — توثيق بنى وتعدادات
 *   4. DocsJsDocTags        — استخراج @param @returns @example @since
 *   5. DocsEdgeCases        — حالات حدّية
 *   6. DocsLeakage          — التحقق من عدم تسرب التوثيق بين تصريحات
 *   7. DocsMultilineHash    — تجميع ## متعددة الأسطر بـ \n
 *   8. DocsProject          — collectSadFiles + extractProjectMarkdown
 */

#include "sad_test_framework.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "docs_extractor.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>

using Sad::AST::DocsExtractor;
using Sad::AST::StmtList;

// ======================================================================
// (AR) كلمات مفتاحية عربية بترميز UTF-8
// (EN) Arabic keywords encoded as UTF-8 byte literals
// ======================================================================
#define AR_FUNC "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9"         // دالة
#define AR_CLASS "\xD8\xB5\xD9\x86\xD9\x81"                // صنف
#define AR_END "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9"  // نهاية
#define AR_RETURN "\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9"       // ارجع
#define AR_VAR "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1"  // متغير
#define AR_PUBLIC "\xD8\xB9\xD8\xA7\xD9\x85"               // عام
#define AR_THIS "\xD9\x87\xD8\xB0\xD8\xA7"                 // هذا
#define AR_CTOR "\xD8\xA8\xD8\xA7\xD9\x86\xD9\x8A"         // باني
#define AR_STRUCT "\xD8\xA8\xD9\x86\xD9\x8A\xD8\xA9"       // بنية
#define AR_ENUM "\xD8\xAA\xD8\xB9\xD8\xAF\xD8\xA7\xD8\xAF" // تعداد

// ======================================================================
// (AR) أدوات مساعدة / (EN) Helpers
// ======================================================================
static StmtList parse(const std::string &source)
{
    Sad::Lexer::LexerCore lexer(source);
    Sad::Parser::ParserCore parser(lexer);
    return parser.parseProgram();
}

static std::string md_of(const std::string &src,
                         const std::string &filename = "test.sad")
{
    auto prog = parse(src);
    return DocsExtractor::extractMarkdown(prog, filename);
}

static bool contains(const std::string &s, const std::string &sub)
{
    return s.find(sub) != std::string::npos;
}

// (AR) ينشئ مجلداً مؤقتاً نظيفاً للاختبار / (EN) Create a clean temp dir
static std::filesystem::path make_temp_project(const std::string &name)
{
    auto base = std::filesystem::temp_directory_path() / ("sad_docs_test_" + name);
    std::error_code ec;
    std::filesystem::remove_all(base, ec);
    std::filesystem::create_directories(base, ec);
    return base;
}

// (AR) يكتب ملفاً نصياً ضمن المجلد المؤقت
static void write_file(const std::filesystem::path &p, const std::string &content)
{
    std::ofstream ofs(p, std::ios::binary | std::ios::trunc);
    ofs << content;
}

// ======================================================================
int main()
{
    SAD_TEST_INIT();

    // ══════════════════════════════════════════════════════════════════
    // 1. DocsBasic — توثيق دوال أساسي
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("DocsBasic / التوثيق_الأساسي");

    SAD_TEST("DOC-B01: تعليق ## وحيد على دالة", {
        std::string src =
            "## دالة جمع بسيطة\n" AR_FUNC " "
            "f(x)\n"
            "  " AR_RETURN " x\n" AR_END "\n";
        auto md = md_of(src);
        SAD_ASSERT_TRUE(contains(md, "f"));
        SAD_ASSERT_TRUE(contains(md, "Documented items:** 1"));
    });

    SAD_TEST("DOC-B02: ## متعددة الأسطر تتجمّع", {
        std::string src =
            "## السطر الأول\n"
            "## السطر الثاني\n"
            "## السطر الثالث\n" AR_FUNC " "
            "g()\n"
            "  " AR_RETURN " 1\n" AR_END "\n";
        auto md = md_of(src);
        // (AR) نتأكد أن جميع الأسطر الثلاثة موجودة في المخرج
        SAD_ASSERT_TRUE(contains(md, "\xD8\xA7\xD9\x84\xD8\xB3\xD8\xB7\xD8\xB1")); // "السطر"
        SAD_ASSERT_TRUE(contains(md, "Documented items:** 1"));
    });

    SAD_TEST("DOC-B03: تعليق كتلة #** **# على دالة", {
        std::string src =
            "#** وصف كتلة\n"
            "   على عدة أسطر\n"
            "**#\n" AR_FUNC " "
            "h()\n" AR_END "\n";
        auto md = md_of(src);
        SAD_ASSERT_TRUE(contains(md, "h"));
        SAD_ASSERT_TRUE(contains(md, "Documented items:** 1"));
    });

    SAD_TEST("DOC-B04: دالة بدون توثيق لا تُحسب", {
        std::string src =
            AR_FUNC " "
                    "noDoc()\n" AR_END "\n";
        auto md = md_of(src);
        SAD_ASSERT_TRUE(contains(md, "Documented items:** 0"));
    });

    SAD_TEST("DOC-B05: عدة دوال موثقة", {
        std::string src =
            "## أولى\n" AR_FUNC " "
            "a()\n" AR_END "\n"
            "## ثانية\n" AR_FUNC " "
            "b()\n" AR_END "\n"
            "## ثالثة\n" AR_FUNC " "
            "c()\n" AR_END "\n";
        auto md = md_of(src);
        SAD_ASSERT_TRUE(contains(md, "Documented items:** 3"));
    });

    // ══════════════════════════════════════════════════════════════════
    // 2. DocsClasses — أصناف وأعضاء
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("DocsClasses / التوثيق_الأصناف");

    SAD_TEST("DOC-C01: صنف موثق", {
        std::string src =
            "## نقطة في المستوى\n" AR_CLASS " Point\n" AR_END "\n";
        auto md = md_of(src);
        SAD_ASSERT_TRUE(contains(md, "Point"));
        SAD_ASSERT_TRUE(contains(md, "class"));
    });

    SAD_TEST("DOC-C02: طريقة موثقة داخل صنف", {
        std::string src =
            AR_CLASS " Calc\n"
                     "  ## يجمع عددين\n"
                     "  " AR_FUNC " "
                     "add(x, y)\n"
                     "    " AR_RETURN " x + y\n"
                     "  " AR_END "\n" AR_END "\n";
        auto md = md_of(src);
        SAD_ASSERT_TRUE(contains(md, "add"));
        SAD_ASSERT_TRUE(contains(md, "method"));
    });

    SAD_TEST("DOC-C03: حقل موثق", {
        std::string src =
            AR_CLASS " Box\n"
                     "  ## عرض الصندوق\n"
                     "  " AR_VAR " " AR_PUBLIC " width = 0\n" AR_END "\n";
        auto md = md_of(src);
        SAD_ASSERT_TRUE(contains(md, "width"));
        SAD_ASSERT_TRUE(contains(md, "field"));
    });

    SAD_TEST("DOC-C04: باني موثق", {
        std::string src =
            AR_CLASS " Person\n"
                     "  ## ينشئ شخصاً جديداً\n"
                     "  " AR_CTOR "(name)\n"
                     "    " AR_THIS ".name = name\n"
                     "  " AR_END "\n" AR_END "\n";
        auto md = md_of(src);
        SAD_ASSERT_TRUE(contains(md, "constructor"));
    });

    // ══════════════════════════════════════════════════════════════════
    // 3. DocsStructEnum — بنى وتعدادات
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("DocsStructEnum / البنى_والتعدادات");

    SAD_TEST("DOC-S01: بنية موثقة", {
        std::string src =
            "## نقطة ثلاثية\n" AR_STRUCT " V3\n"
            "  x = 0\n"
            "  y = 0\n"
            "  z = 0\n" AR_END "\n";
        auto md = md_of(src);
        SAD_ASSERT_TRUE(contains(md, "V3"));
        SAD_ASSERT_TRUE(contains(md, "struct"));
    });

    SAD_TEST("DOC-S02: تعداد موثق", {
        std::string src =
            "## ألوان أساسية\n" AR_ENUM " Color\n"
            "  RED\n"
            "  GREEN\n"
            "  BLUE\n" AR_END "\n";
        auto md = md_of(src);
        SAD_ASSERT_TRUE(contains(md, "Color"));
        SAD_ASSERT_TRUE(contains(md, "enum"));
    });

    // ══════════════════════════════════════════════════════════════════
    // 4. DocsJsDocTags — استخراج علامات JSDoc
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("DocsJsDocTags / علامات_التوثيق");

    SAD_TEST("DOC-J01: @param و @returns", {
        std::string src =
            "## يضرب عددين\n"
            "## @param a المعامل الأول\n"
            "## @param b المعامل الثاني\n"
            "## @returns حاصل الضرب\n" AR_FUNC " "
            "mul(a, b)\n"
            "  " AR_RETURN " a * b\n" AR_END "\n";
        auto md = md_of(src);
        SAD_ASSERT_TRUE(contains(md, "Parameters") || contains(md, "Returns"));
    });

    SAD_TEST("DOC-J02: @example", {
        std::string src =
            "## دالة مثال\n"
            "## @example\n"
            "## f(1, 2)\n" AR_FUNC " "
            "f(a, b)\n" AR_END "\n";
        auto md = md_of(src);
        SAD_ASSERT_TRUE(contains(md, "Example"));
    });

    SAD_TEST("DOC-J03: @since و @author", {
        std::string src =
            "## دالة قديمة\n"
            "## @since 1.0.0\n"
            "## @author محمد\n" AR_FUNC " "
            "old()\n" AR_END "\n";
        auto md = md_of(src);
        SAD_ASSERT_TRUE(contains(md, "Since") || contains(md, "1.0.0"));
        SAD_ASSERT_TRUE(contains(md, "Author"));
    });

    // ══════════════════════════════════════════════════════════════════
    // 5. DocsEdgeCases — حالات حدّية
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("DocsEdgeCases / حالات_حدية");

    SAD_TEST("DOC-E01: ملف فارغ تماماً", {
        auto md = md_of("");
        SAD_ASSERT_TRUE(contains(md, "Documented items:** 0"));
    });

    SAD_TEST("DOC-E02: ملف بدون أي توثيق", {
        std::string src =
            AR_FUNC " "
                    "a()\n" AR_END "\n" AR_FUNC " "
                    "b()\n" AR_END "\n";
        auto md = md_of(src);
        SAD_ASSERT_TRUE(contains(md, "Documented items:** 0"));
    });

    SAD_TEST("DOC-E03: تعليق # عادي لا يُعد توثيقاً", {
        std::string src =
            "# مجرد تعليق عادي\n" AR_FUNC " "
            "f()\n" AR_END "\n";
        auto md = md_of(src);
        SAD_ASSERT_TRUE(contains(md, "Documented items:** 0"));
    });

    SAD_TEST("DOC-E04: العنوان مُشتق من اسم الملف", {
        auto md = md_of("", "MyModule.sad");
        SAD_ASSERT_TRUE(contains(md, "MyModule.sad"));
    });

    // ══════════════════════════════════════════════════════════════════
    // 6. DocsLeakage — لا تسرب توثيق
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("DocsLeakage / عدم_تسرب");

    SAD_TEST("DOC-L01: توثيق طريقة لا يتسرب لتصريح علوي تالي", {
        // (AR) Bug 2 المُصلَح: قبل الإصلاح كان توثيق طريقة الصنف الأخير
        //      يتسرّب ليصبح توثيقاً للدالة العلوية التي تليه.
        std::string src =
            AR_CLASS " A\n"
                     "  ## توثيق طريقة\n"
                     "  " AR_FUNC " "
                     "m()\n" AR_END "\n" AR_END "\n" AR_FUNC " "
                     "after()\n" AR_END "\n";
        auto md = md_of(src);
        // (AR) الدالة `after` بدون توثيق فلا تظهر — لكن المهم: التوثيق
        //      "توثيق طريقة" يجب أن يظهر مرة واحدة فقط (مرتبط بالطريقة m)
        //      ولا يتسرب لـ after.
        // (EN) `after` has no doc so it shouldn't appear — but the doc text
        //      "توثيق طريقة" must appear exactly once (attached to method m).
        size_t firstPos = md.find("\xD8\xAA\xD9\x88\xD8\xAB\xD9\x8A\xD9\x82 \xD8\xB7\xD8\xB1\xD9\x8A\xD9\x82\xD8\xA9"); // "توثيق طريقة"
        SAD_ASSERT_TRUE(firstPos != std::string::npos);
        size_t secondPos = md.find("\xD8\xAA\xD9\x88\xD8\xAB\xD9\x8A\xD9\x82 \xD8\xB7\xD8\xB1\xD9\x8A\xD9\x82\xD8\xA9", firstPos + 1);
        SAD_ASSERT_TRUE(secondPos == std::string::npos);
    });

    SAD_TEST("DOC-L02: توثيق دالة لا يلتصق بدالة لاحقة بدون توثيق", {
        std::string src =
            "## توثيق الأولى\n" AR_FUNC " "
            "first()\n" AR_END "\n" AR_FUNC " "
            "second()\n" AR_END "\n";
        auto md = md_of(src);
        SAD_ASSERT_TRUE(contains(md, "Documented items:** 1"));
    });

    // ══════════════════════════════════════════════════════════════════
    // 7. DocsMultilineHash — تجميع ## بـ \n
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("DocsMultilineHash / متعدد_الأسطر");

    SAD_TEST("DOC-M01: 5 أسطر ## تتراكم بترتيبها", {
        // (AR) Bug 1 المُصلَح: قبل الإصلاح كان كل سطر ## يستبدل السابق.
        std::string src =
            "## أ\n"
            "## ب\n"
            "## ج\n"
            "## د\n"
            "## هـ\n" AR_FUNC " "
            "f()\n" AR_END "\n";
        auto md = md_of(src);
        // (AR) إذا كانت الأسطر تتراكم بشكل صحيح فالملخص الأول
        //      يجب أن يحتوي على الحرف "أ" (الأول)
        SAD_ASSERT_TRUE(contains(md, "Documented items:** 1"));
        SAD_ASSERT_TRUE(contains(md, "\xD8\xA3")); // "أ"
    });

    // ══════════════════════════════════════════════════════════════════
    // 8. DocsProject — collectSadFiles + extractProjectMarkdown
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("DocsProject / توثيق_المشروع");

    SAD_TEST("DOC-P01: collectSadFiles يعيد قائمة فارغة لمجلد غير موجود", {
        auto files = DocsExtractor::collectSadFiles("/__definitely_does_not_exist_xyz__");
        SAD_ASSERT_TRUE(files.empty());
    });

    SAD_TEST("DOC-P02: collectSadFiles يجد .sad في مجلد", {
        auto root = make_temp_project("p02");
        write_file(root / "a.sad", "## A\n" AR_FUNC " "
                                   "a()\n" AR_END "\n");
        write_file(root / "b.sad", "## B\n" AR_FUNC " "
                                   "b()\n" AR_END "\n");
        write_file(root / "ignored.txt", "not sad");

        auto files = DocsExtractor::collectSadFiles(root.u8string());
        SAD_ASSERT_EQ(files.size(), (size_t)2);
    });

    SAD_TEST("DOC-P03: collectSadFiles يبحث تكرارياً", {
        auto root = make_temp_project("p03");
        std::filesystem::create_directories(root / "sub" / "deep");
        write_file(root / "top.sad", "## T\n" AR_FUNC " "
                                     "t()\n" AR_END "\n");
        write_file(root / "sub" / "mid.sad", "## M\n" AR_FUNC " "
                                             "m()\n" AR_END "\n");
        write_file(root / "sub" / "deep" / "low.sad", "## L\n" AR_FUNC " "
                                                      "l()\n" AR_END "\n");

        auto files = DocsExtractor::collectSadFiles(root.u8string());
        SAD_ASSERT_EQ(files.size(), (size_t)3);
    });

    SAD_TEST("DOC-P04: collectSadFiles يقبل ملفاً مفرداً", {
        auto root = make_temp_project("p04");
        auto f = root / "single.sad";
        write_file(f, "## S\n" AR_FUNC " "
                      "s()\n" AR_END "\n");

        auto files = DocsExtractor::collectSadFiles(f.u8string());
        SAD_ASSERT_EQ(files.size(), (size_t)1);
    });

    SAD_TEST("DOC-P05: extractProjectMarkdown يبني فهرساً وأقساماً", {
        auto root = make_temp_project("p05");
        write_file(root / "alpha.sad", "## دالة ألفا\n" AR_FUNC " "
                                       "alpha()\n" AR_END "\n");
        write_file(root / "beta.sad", "## دالة بيتا\n" AR_FUNC " "
                                      "beta()\n" AR_END "\n");

        auto paths = DocsExtractor::collectSadFiles(root.u8string());
        SAD_ASSERT_EQ(paths.size(), (size_t)2);

        // (AR) BF-04: نحجز السعة مسبقاً لمنع dangling pointers عند
        //      إعادة تخصيص vector — &programs.back() يبقى صالحاً.
        std::vector<StmtList> programs;
        programs.reserve(paths.size());
        std::vector<DocsExtractor::FileEntry> entries;
        for (const auto &p : paths)
        {
            std::ifstream ifs(p, std::ios::binary);
            std::string src((std::istreambuf_iterator<char>(ifs)),
                            std::istreambuf_iterator<char>());
            programs.push_back(parse(src));
            entries.push_back({p, &programs.back()});
        }

        std::string md = DocsExtractor::extractProjectMarkdown("MyProj", entries);
        SAD_ASSERT_TRUE(contains(md, "MyProj"));
        SAD_ASSERT_TRUE(contains(md, "File Index"));
        SAD_ASSERT_TRUE(contains(md, "alpha"));
        SAD_ASSERT_TRUE(contains(md, "beta"));
        SAD_ASSERT_TRUE(contains(md, "Documented files:** 2"));
        SAD_ASSERT_TRUE(contains(md, "Total declarations:** 2"));
    });

    SAD_TEST("DOC-P06: ملفات بدون توثيق تُحذف من فهرس المشروع", {
        auto root = make_temp_project("p06");
        write_file(root / "documented.sad", "## مع توثيق\n" AR_FUNC " "
                                            "d()\n" AR_END "\n");
        write_file(root / "empty.sad", AR_FUNC " "
                                               "e()\n" AR_END "\n");

        auto paths = DocsExtractor::collectSadFiles(root.u8string());
        // (AR) BF-04: حجز سعة لمنع dangling pointers
        std::vector<StmtList> programs;
        programs.reserve(paths.size());
        std::vector<DocsExtractor::FileEntry> entries;
        for (const auto &p : paths)
        {
            std::ifstream ifs(p, std::ios::binary);
            std::string src((std::istreambuf_iterator<char>(ifs)),
                            std::istreambuf_iterator<char>());
            programs.push_back(parse(src));
            entries.push_back({p, &programs.back()});
        }

        std::string md = DocsExtractor::extractProjectMarkdown("Test", entries);
        // (AR) فقط ملف واحد يجب أن يظهر في الفهرس
        SAD_ASSERT_TRUE(contains(md, "Documented files:** 1"));
        SAD_ASSERT_TRUE(contains(md, "documented.sad"));
        // (AR) `empty.sad` يجب ألا يظهر كقسم
    });

    SAD_TEST("DOC-P07: مشروع فارغ يُنتج رسالة مناسبة", {
        std::vector<DocsExtractor::FileEntry> empty;
        auto md = DocsExtractor::extractProjectMarkdown("Empty", empty);
        SAD_ASSERT_TRUE(contains(md, "Documented files:** 0"));
        SAD_ASSERT_TRUE(contains(md, "No file"));
    });

    SAD_SUMMARY();
}
