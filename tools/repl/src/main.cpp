// بسم الله الرحمن الرحيم
/**
 * @file main.cpp
 * @brief نقطة الدخول لـ REPL لغة ص / Entry point for Sad Language REPL
 *
 * @author Sad Language Team
 * @date January 2026
 */

#include "repl_engine.h"
#include "repl_sot_generated.h" // (AR) كتالوج «مصدر حقيقة الأدوات» — رسائل/أخطاء CLI / (EN) Tool-SoT catalog
#include "utf8_args.h"
#include <iostream>
#include <cstring>

using namespace Sad::REPL;

// (AR) خطّ زينة الترويسة (زخرفة لا محتوى) / (EN) banner rule (decoration, not content)
static const char *const kRule = "========================================";

/**
 * @brief عرض المساعدة / Show help
 * @note (AR) الترويسة والأوصاف من كتالوج SoT؛ وقسم «الأوامر الخاصّة» يُرسَم من
 *       جدول الأوامر المولَّد (kCommands) فلا يُكرَّر يدوياً. أسماء الأعلام
 *       (--help…) وأمثلة كود ص حرفيّاتٌ تقنيّة لا نصوصُ واجهة تُترجَم.
 */
void showHelp()
{
    std::cout << kRule << "\n";
    std::cout << SoT::kDisplayNameAr << "\n";
    std::cout << SoT::kDisplayNameEn << "\n";
    std::cout << kRule << "\n\n";

    // (AR) الاستخدام / (EN) Usage
    std::cout << SoT::messageBoth(SoT::Message::USAGE_LABEL) << ":\n";
    std::cout << "  sad-repl [options]\n\n";

    // (AR) الخيارات / (EN) Options
    std::cout << SoT::messageBoth(SoT::Message::CLI_OPTIONS_HEADER) << ":\n";
    std::cout << "  --help, -h           " << SoT::messageBoth(SoT::Message::CLI_OPT_HELP) << "\n";
    std::cout << "  --version, -v        " << SoT::messageBoth(SoT::Message::CLI_OPT_VERSION) << "\n";
    std::cout << "  --no-color           " << SoT::messageBoth(SoT::Message::CLI_OPT_NO_COLOR) << "\n";
    std::cout << "  --no-history         " << SoT::messageBoth(SoT::Message::CLI_OPT_NO_HISTORY) << "\n";
    std::cout << "  --history-file FILE  " << SoT::messageBoth(SoT::Message::CLI_OPT_HISTORY_FILE) << "\n";
    std::cout << "  --load FILE          " << SoT::messageBoth(SoT::Message::CLI_OPT_LOAD) << "\n";
    std::cout << "  --rich               " << SoT::messageBoth(SoT::Message::CLI_OPT_RICH) << "\n\n";

    // (AR) الأوامر الخاصّة — تُرسَم من كتالوج الأوامر (مصدرٌ واحد، بلا تكرار)
    // (EN) Special commands — rendered from the command catalog (single source)
    std::cout << SoT::messageBoth(SoT::Message::CLI_SPECIAL_COMMANDS_HEADER) << ":\n";
    for (std::size_t i = 0; i < SoT::kCommandsCount; ++i)
    {
        const SoT::CommandEntry &c = SoT::kCommands[i];
        std::cout << "  :" << c.name << " / :" << c.arabicName << "  —  "
                  << c.descAr << SoT::kBilingualSeparator << c.descEn << "\n";
    }
    std::cout << "\n";

    // (AR) أمثلة كود ص (توضيحيّة — ليست نصوص واجهة) / (EN) ص code samples (illustrative)
    std::cout << SoT::messageBoth(SoT::Message::CLI_EXAMPLES_HEADER) << ":\n";
    std::cout << "  >>> متغير س = 42\n";
    std::cout << "  >>> دالة مربع(ع) ارجع ع * ع نهاية\n";
    std::cout << "  >>> مربع(10)\n";
    std::cout << "  100\n\n";
}

/**
 * @brief عرض رقم الإصدار / Show version
 * @note (AR) رقم الإصدار واسم الأداة من الكتالوج؛ «sad-repl» اسم التنفيذيّ (حرفيّ).
 */
void showVersion()
{
    std::cout << "sad-repl version " << SoT::kVersion << "\n";
    std::cout << SoT::kDisplayNameEn << " v" << SoT::kVersion << "\n";
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
        else if (arg == "--rich")
        {
            // (AR) تفعيل محرّر السطر التفاعليّ (يُطبَّق فقط على طرفيّة تفاعليّة)
            // (EN) enable the interactive line editor (only takes effect on a TTY)
            config.enableLineEditor = true;
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
            // (AR) خطأ «خيار غير معروف» وتلميحه من كتالوج SoT / (EN) unknown-option error + hint from SoT
            std::cerr << SoT::errorMessage(SoT::Error::UNKNOWN_OPTION, arg) << "\n";
            if (const auto *e = SoT::findError(SoT::Error::UNKNOWN_OPTION); e && e->hintAr)
            {
                std::cerr << e->hintAr << SoT::kBilingualSeparator << e->hintEn << "\n";
            }
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
                std::cerr << SoT::errorMessage(SoT::Error::LOAD_FAILED, loadFile) << "\n";
                return 1;
            }
        }

        // تشغيل REPL / Run REPL
        return repl.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << SoT::errorMessage(SoT::Error::FATAL, e.what()) << "\n";
        return 1;
    }
}
