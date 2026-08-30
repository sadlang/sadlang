// (AR) نقطة دخول مركز أدوات لغة ص (sad.exe — الهدفُ والمُخرَجُ والمنشورُ اسمٌ واحد)
// (EN) Entry point for Sad Tool Hub (sad.exe — target, output and published name are one)

#include "hub/tool_registry.h"
#include "hub/command_dispatcher.h"
#include "hub/help_renderer.h"
#include "utf8_args.h" // (AR) الأداة الموحَّدة في shared/utils/include/utf8_args.h

#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

namespace
{

    constexpr const char *kHubVersion = "1.0.0";

    // (AR) محاولة الحصول على المسار الكامل للتنفيذي الحالي
    // (EN) Try to get full path of current executable
    std::string getSelfPath(const char *argv0)
    {
#ifdef _WIN32
        char buf[MAX_PATH];
        DWORD n = GetModuleFileNameA(nullptr, buf, MAX_PATH);
        if (n > 0 && n < MAX_PATH)
            return std::string(buf, n);
#endif
        return std::string(argv0 ? argv0 : "sad");
    }

    // (AR) هل السلسلة تطابق أي علم help/version/list؟
    // (EN) Does the string match any help/version/list flag?
    bool isHelpFlag(const std::string &s)
    {
        return s == "--help" || s == "-h" || s == "help" || s == "/?" || s == "/h";
    }
    bool isVersionFlag(const std::string &s)
    {
        return s == "--version" || s == "-v" || s == "version";
    }
    bool isListFlag(const std::string &s)
    {
        return s == "--list" || s == "list";
    }

} // anonymous

int main(int argc, char **argv)
{
#ifdef _WIN32
    // (AR) تفعيل UTF-8 لإخراج العربية بشكل صحيح
    // (EN) Enable UTF-8 for proper Arabic output
    SetConsoleOutputCP(CP_UTF8);
#endif

    try
    {
        using namespace Sad::Hub;

        // (AR) جمع argv بـUTF-8 (يحفظ الأسماء العربية على ويندوز)
        // (EN) Collect UTF-8 argv (preserves Arabic filenames on Windows)
        auto args = Sad::HubUtil::getUtf8Args(argc, argv);
        const int n = static_cast<int>(args.size());

        std::string selfPath = getSelfPath(n > 0 ? args[0].c_str() : nullptr);

        ToolRegistry registry;
        registry.discover(selfPath);

        HelpRenderer help(registry);

        // (AR) لا معطيات → اعرض المساعدة
        // (EN) No args → show help
        if (n < 2)
        {
            std::cout << help.renderMainHelp(kHubVersion);
            return 0;
        }

        const std::string &firstArg = args[1];

        // (AR) أوامر مدمجة
        // (EN) Built-in commands
        if (isHelpFlag(firstArg))
        {
            std::cout << help.renderMainHelp(kHubVersion);
            return 0;
        }
        if (isVersionFlag(firstArg))
        {
            std::cout << help.renderVersion(kHubVersion);
            return 0;
        }
        if (isListFlag(firstArg))
        {
            std::cout << help.renderToolList();
            return 0;
        }

        // (AR) جمع باقي المعطيات
        // (EN) Collect remaining args
        std::vector<std::string> forwardArgs;
        for (int i = 2; i < n; ++i)
            forwardArgs.push_back(args[i]);

        // (AR) توزيع الأمر إلى الأداة المناسبة
        // (EN) Dispatch command to appropriate tool
        CommandDispatcher dispatcher(registry);
        DispatchResult result = dispatcher.dispatch(firstArg, forwardArgs);

        if (!result.dispatched)
        {
            // (AR) أداة غير موجودة → اعرض اقتراحاً
            // (EN) Tool not found → show suggestion
            std::cerr << help.renderUnknownCommand(firstArg);
            return 127;
        }

        return result.exitCode;
    }
    catch (const std::exception &ex)
    {
        std::cerr << "(AR) خطأ غير متوقع في sad: " << ex.what() << "\n"
                  << "(EN) unexpected error in sad: " << ex.what() << "\n";
        return 2;
    }
    catch (...)
    {
        std::cerr << "(AR) خطأ غير معروف في sad\n"
                  << "(EN) unknown error in sad\n";
        return 2;
    }
}
