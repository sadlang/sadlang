// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: fmt_main.cpp
 * الوصف: نقطة الدخول الرئيسية لأداة تنسيق كود لغة ص
 * Main Entry Point for Sad Language Code Formatter
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * الاستخدام / Usage:
 *   sad-fmt file.ص           تنسيق ملف واحد / Format single file
 *   sad-fmt dir/             تنسيق مجلد / Format directory
 *   sad-fmt --check file.ص   التحقق فقط / Check only
 *   sad-fmt --stdin          القراءة من stdin / Read from stdin
 *
 * @author فريق لغة ص
 * @date 2026
 */

// إعلان دالة التنسيق الجديدة من sad_formatter.cpp
// Declaration of formatter function from sad_formatter.cpp
namespace Sad
{
    namespace Format
    {
        int formatter_main(int argc, char *argv[]);
    }
}

#include "utf8_args.h"

/**
 * @brief نقطة الدخول الرئيسية - Main entry point
 */
int main(int argc, char *argv[])
{
    Sad::Utils::enableUtf8Console();
    Sad::Utils::Utf8ArgvHolder _argvHolder(argc, argv);
    return Sad::Format::formatter_main(argc, argv);
}
