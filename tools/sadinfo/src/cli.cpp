// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: tools/sadinfo/src/cli.cpp
// (AR) معالجة وسائط سطر الأوامر وإرسالها للأمر المناسب.
//      الأوامر المدعومة في v0.1:
//        --version                        طباعة الإصدار
//        --help                           رسالة المساعدة
//        --dump-keywords [-o file] [--compact|--pretty]
//        --dump-builtins  [-o file]       (stub — لم يُنفَّذ بعد)
//        --dump-errors    [-o file]       (stub)
//        --dump-directives [-o file]      (stub)
//        --dump-all -o <dir>              (stub)
//
// (EN) Command-line argument parsing and dispatch.
// ═══════════════════════════════════════════════════════════════════════════════

#include "sadinfo.h"

#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

namespace Sad
{
    namespace Info
    {

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) رسالة المساعدة الثنائية اللغة
        // (EN) Bilingual help message
        // ───────────────────────────────────────────────────────────────────────────────
        static void printHelp()
        {
            std::cout << "sadinfo " << kToolVersion << " — أداة معلومات لغة ص / Sad language info tool\n"
                                                       "\n"
                                                       "الاستخدام / Usage:\n"
                                                       "  sadinfo <command> [options]\n"
                                                       "\n"
                                                       "الأوامر / Commands:\n"
                                                       "  --version                    طباعة الإصدار / Print version\n"
                                                       "  --help, -h                   عرض هذه الرسالة / Show this message\n"
                                                       "  --dump-keywords              إخراج الكلمات / Dump keywords (JSON or YAML)\n"
                                                       "  --dump-builtins              إخراج الدوال المدمجة / Dump built-in functions\n"
                                                       "  --dump-errors                إخراج أكواد الأخطاء / Dump error codes\n"
                                                       "  --dump-directives            إخراج التوجيهات @ / Dump @ directives\n"
                                                       "  --dump-all -o <dir>          إخراج كل شيء / Dump everything\n"
                                                       "\n"
                                                       "خيارات عامة / Common Options:\n"
                                                       "  -o, --output <path>          ملف الإخراج (افتراضياً stdout)\n"
                                                       "  --pretty                     تنسيق مقروء (افتراضي) / Pretty (default)\n"
                                                       "  --compact                    JSON مضغوط / Compact JSON\n"
                                                       "  --format json|yaml           صيغة الإخراج (افتراضياً json)\n"
                                                       "\n"
                                                       "خيارات --dump-keywords / Keyword Options:\n"
                                                       "  --lang ar|en|both            اللغة المعروضة (افتراضياً both)\n"
                                                       "  --minimal                    أبقِ word/tokenType/category فقط\n"
                                                       "  --by-category                جمّع الكلمات تحت categories.{...}\n"
                                                       "  --include-deprecated         ضمّن الكلمات المُهمَلة (90 → 91)\n"
                                                       "  --filter key=value           تصفية (يمكن تكرارها — AND)\n"
                                                       "      مفاتيح مدعومة:\n"
                                                       "        category=reserved|operator|contextual|builtin_type\n"
                                                       "        role=block_opener|block_closer|inter_block|import_keyword\n"
                                                       "        token-type=KEYWORD_IF (اسم TokenType حرفياً)\n"
                                                       "        has-aliases (بدون قيمة)\n"
                                                       "\n"
                                                       "خيارات --dump-builtins / Builtin Options:\n"
                                                       "  --lang ar|en|both            اللغة المعروضة (افتراضياً both)\n"
                                                       "  --minimal                    أبقِ id/nameAr/category/returnType فقط\n"
                                                       "  --filter key=value           تصفية (يمكن تكرارها — AND)\n"
                                                       "      مفاتيح مدعومة:\n"
                                                       "        category=printing|input|reflection|type_ctor|concurrency\n"
                                                       "        host=array|string|map|channel  (للطرق فقط)\n"
                                                       "\n"
                                                       "أمثلة / Examples:\n"
                                                       "  sadinfo --dump-keywords -o keywords.json\n"
                                                       "  sadinfo --dump-keywords --compact > keywords.min.json\n"
                                                       "  sadinfo --dump-keywords --filter category=reserved\n"
                                                       "  sadinfo --dump-keywords --filter category=reserved --filter role=block_opener\n"
                                                       "  sadinfo --dump-keywords --by-category --minimal\n"
                                                       "  sadinfo --dump-keywords --include-deprecated --format yaml\n"
                                                       "  sadinfo --dump-builtins -o builtins.json\n"
                                                       "  sadinfo --dump-builtins --filter category=concurrency\n"
                                                       "  sadinfo --dump-builtins --filter host=array --format yaml\n"
                                                       "  sadinfo --dump-builtins --minimal --lang ar\n"
                                                       "\n"
                                                       "الإصدار schema / Schema version: "
                      << kSchemaVersion << "\n";
        }

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) أنواع الأوامر المدعومة
        // (EN) Supported command types
        // ───────────────────────────────────────────────────────────────────────────────
        enum class Command
        {
            None,
            Help,
            Version,
            DumpKeywords,
            DumpBuiltins,
            DumpErrors,
            DumpDirectives,
            DumpAll,
        };

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) ربط نص الأمر بالنوع — جدول lookup خفيف
        // (EN) Map command string to enum — small lookup
        // ───────────────────────────────────────────────────────────────────────────────
        static Command parseCommand(std::string_view arg)
        {
            if (arg == "--help" || arg == "-h")
                return Command::Help;
            if (arg == "--version")
                return Command::Version;
            if (arg == "--dump-keywords")
                return Command::DumpKeywords;
            if (arg == "--dump-builtins")
                return Command::DumpBuiltins;
            if (arg == "--dump-errors")
                return Command::DumpErrors;
            if (arg == "--dump-directives")
                return Command::DumpDirectives;
            if (arg == "--dump-all")
                return Command::DumpAll;
            return Command::None;
        }

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) المدخل الرئيسي / (EN) Main entry
        // ───────────────────────────────────────────────────────────────────────────────
        int sadinfoMain(int argc, char **argv)
        {
            if (argc < 2)
            {
                printHelp();
                return 0;
            }

            // (AR) أول وسيط هو الأمر / (EN) first argument is the command
            Command cmd = parseCommand(argv[1]);

            if (cmd == Command::None)
            {
                std::cerr << "(AR) خطأ: أمر غير معروف '" << argv[1] << "'\n"
                          << "(EN) error: unknown command '" << argv[1] << "'\n"
                          << "(AR) شغّل 'sadinfo --help' لعرض الأوامر / (EN) run 'sadinfo --help'\n";
                return 2;
            }

            if (cmd == Command::Help)
            {
                printHelp();
                return 0;
            }

            if (cmd == Command::Version)
            {
                std::cout << "sadinfo " << kToolVersion
                          << " (schema " << kSchemaVersion << ")\n";
                return 0;
            }

            // ── تحليل الخيارات المشتركة لكل أوامر dump / Common dump options ──
            DumpOptions opts;
            opts.pretty = true;
            for (int i = 2; i < argc; ++i)
            {
                std::string_view a = argv[i];
                if (a == "-o" || a == "--output")
                {
                    if (i + 1 >= argc)
                    {
                        std::cerr << "(AR) خطأ: -o يتطلب مساراً / (EN) error: -o requires a path\n";
                        return 2;
                    }
                    opts.outputPath = argv[++i];
                }
                else if (a == "--pretty")
                {
                    opts.pretty = true;
                }
                else if (a == "--compact")
                {
                    opts.pretty = false;
                }
                else if (a == "--format")
                {
                    if (i + 1 >= argc)
                    {
                        std::cerr << "(AR) خطأ: --format يتطلب قيمة (json|yaml)\n"
                                  << "(EN) error: --format requires a value (json|yaml)\n";
                        return 2;
                    }
                    std::string_view fmt = argv[++i];
                    if (fmt != "json" && fmt != "yaml")
                    {
                        std::cerr << "(AR) خطأ: صيغة غير مدعومة '" << fmt << "' (المدعوم: json|yaml)\n"
                                  << "(EN) error: unsupported format '" << fmt << "' (supported: json|yaml)\n";
                        return 2;
                    }
                    opts.format = std::string(fmt);
                }
                else if (a == "--lang")
                {
                    if (i + 1 >= argc)
                    {
                        std::cerr << "(AR) خطأ: --lang يتطلب قيمة (ar|en|both)\n"
                                  << "(EN) error: --lang requires a value (ar|en|both)\n";
                        return 2;
                    }
                    std::string_view lang = argv[++i];
                    if (lang != "ar" && lang != "en" && lang != "both")
                    {
                        std::cerr << "(AR) خطأ: لغة غير مدعومة '" << lang << "' (المدعوم: ar|en|both)\n"
                                  << "(EN) error: unsupported lang '" << lang << "' (supported: ar|en|both)\n";
                        return 2;
                    }
                    opts.lang = std::string(lang);
                }
                else if (a == "--minimal")
                {
                    opts.minimal = true;
                }
                else if (a == "--by-category")
                {
                    opts.byCategory = true;
                }
                else if (a == "--include-deprecated")
                {
                    opts.includeDeprecated = true;
                }
                else if (a == "--filter")
                {
                    if (i + 1 >= argc)
                    {
                        std::cerr << "(AR) خطأ: --filter يتطلب تعبيراً key=value\n"
                                  << "(EN) error: --filter requires a key=value expression\n";
                        return 2;
                    }
                    opts.filters.emplace_back(argv[++i]);
                }
                else
                {
                    std::cerr << "(AR) خطأ: خيار غير معروف '" << a << "'\n"
                              << "(EN) error: unknown option '" << a << "'\n";
                    return 2;
                }
            }

            // ── تنفيذ الأمر / Dispatch ──
            switch (cmd)
            {
            case Command::DumpKeywords:
                return dumpKeywords(opts);
            case Command::DumpBuiltins:
                return dumpBuiltins(opts);
            case Command::DumpErrors:
                return dumpErrors(opts);
            case Command::DumpDirectives:
                return dumpDirectives(opts);
            case Command::DumpAll:
            {
                // (AR) يتطلب مجلد إخراج / (EN) requires output directory
                if (opts.outputPath.empty())
                {
                    std::cerr << "(AR) خطأ: --dump-all يتطلب -o <dir>\n"
                              << "(EN) error: --dump-all requires -o <dir>\n";
                    return 2;
                }
                std::cerr << "(AR) --dump-all قيد التطوير في v0.2 / (EN) --dump-all coming in v0.2\n";
                return 1;
            }
            default:
                // (AR) لن نصل هنا — فحص استباقي
                return 2;
            }
        }

    } // namespace Info
} // namespace Sad
