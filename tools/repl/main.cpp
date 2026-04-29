// بسم الله الرحمن الرحيم
/**
 * @file main.cpp
 * @brief نقطة الدخول لـ REPL لغة ص / Entry point for Sad Language REPL
 *
 * @author Sad Language Team
 * @date January 2026
 */

#include "repl_engine.h"
#include "utf8_args.h"
#include <iostream>
#include <cstring>

using namespace Sad::REPL;

/**
 * @brief عرض المساعدة / Show help
 */
void showHelp()
{
    std::cout << "========================================\n";
    std::cout << "لغة ص - REPL\n";
    std::cout << "Sad Language - REPL\n";
    std::cout << "========================================\n\n";

    std::cout << "الاستخدام / Usage:\n";
    std::cout << "  sad-repl [options]\n\n";

    std::cout << "الخيارات / Options:\n";
    std::cout << "  --help, -h           عرض هذه المساعدة / Show this help\n";
    std::cout << "  --version, -v        عرض رقم الإصدار / Show version\n";
    std::cout << "  --no-color           تعطيل الألوان / Disable colors\n";
    std::cout << "  --no-history         تعطيل التاريخ / Disable history\n";
    std::cout << "  --history-file FILE  ملف التاريخ / History file\n";
    std::cout << "  --load FILE          تحميل ملف عند البدء / Load file on start\n\n";

    std::cout << "الأوامر الخاصة / Special Commands:\n";
    std::cout << "  :help / :مساعدة      عرض المساعدة / Show help\n";
    std::cout << "  :exit / :خروج        الخروج / Exit\n";
    std::cout << "  :clear / :مسح        مسح الشاشة / Clear screen\n";
    std::cout << "  :type <expr>         عرض نوع التعبير / Show type\n";
    std::cout << "  :load <file>         تحميل ملف / Load file\n";
    std::cout << "  :history             عرض التاريخ / Show history\n";
    std::cout << "  :reset               إعادة تعيين / Reset\n";
    std::cout << "  :vars                عرض المتغيرات / Show variables\n";
    std::cout << "  :funcs               عرض الدوال / Show functions\n\n";

    std::cout << "أمثلة / Examples:\n";
    std::cout << "  >>> متغير س = 42\n";
    std::cout << "  >>> دالة مربع(ع) { إرجاع ع * ع }\n";
    std::cout << "  >>> مربع(10)\n";
    std::cout << "  100\n\n";
}

/**
 * @brief عرض رقم الإصدار / Show version
 */
void showVersion()
{
    std::cout << "sad-repl version 1.0.0\n";
    std::cout << "Sad Language REPL v1.0.0\n";
}

/**
 * @brief نقطة الدخول الرئيسية / Main entry point
 */
int main(int argc, char *argv[])
{
    // (AR) تفعيل UTF-8 على الكونسول + ربط argv بـUTF-8 لدعم العربية على ويندوز
    Sad::Utils::enableUtf8Console();
    Sad::Utils::Utf8ArgvHolder _argvHolder(argc, argv);

    // تحليل الخيارات / Parse options
    REPLConfig config;
    std::string loadFile;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h")
        {
            showHelp();
            return 0;
        }
        else if (arg == "--version" || arg == "-v")
        {
            showVersion();
            return 0;
        }
        else if (arg == "--no-color")
        {
            config.enableColor = false;
        }
        else if (arg == "--no-history")
        {
            config.enableHistory = false;
        }
        else if (arg == "--history-file" && i + 1 < argc)
        {
            config.historyFile = argv[++i];
        }
        else if (arg == "--load" && i + 1 < argc)
        {
            loadFile = argv[++i];
        }
        else
        {
            std::cerr << "خيار غير معروف / Unknown option: " << arg << "\n";
            std::cerr << "استخدم --help للمساعدة / Use --help for help\n";
            return 1;
        }
    }

    // إنشاء وتشغيل REPL / Create and run REPL
    try
    {
        REPLEngine repl(config);

        // تحميل ملف إذا طُلب / Load file if requested
        if (!loadFile.empty())
        {
            if (!repl.loadFile(loadFile))
            {
                std::cerr << "فشل تحميل الملف / Failed to load file: " << loadFile << "\n";
                return 1;
            }
        }

        // تشغيل REPL / Run REPL
        return repl.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "خطأ فادح / Fatal error: " << e.what() << "\n";
        return 1;
    }
}
