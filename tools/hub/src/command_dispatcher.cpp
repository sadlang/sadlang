// (AR) تنفيذ موزِّع الأوامر — يُشغّل subprocess للأداة المطلوبة
// (EN) Command dispatcher implementation — runs subprocess for requested tool

#include "hub/command_dispatcher.h"

#include <cstdlib>
#include <sstream>

// 🔑 (AR) ترويسةُ المنصّةِ تُضَمُّ هنا — خارجَ `namespace Sad` — لا في فرعِ
//     `#else` أسفلُ داخلَه: ضمٌّ داخلَ فضاءِ أسماءٍ يُصادِر الترويسةَ إليه
//     بأسرِها، وحارسُ الضمِّ يجعلها النسخةَ الوحيدةَ في وحدةِ الترجمة، فيغيب
//     `::getpid` ونظائرُه عن النطاقِ العامّ بحسبِ ترتيبِ الضمِّ وحدَه.
// 🔑 (EN) Platform headers belong here, outside namespace Sad — never in the
//     #else branch inside it: an include within a namespace captures the
//     header into it, and its include guard then makes that the only copy
//     in the translation unit.
#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

namespace Sad
{
    namespace Hub
    {

        // ============================================================================
        CommandDispatcher::CommandDispatcher(const ToolRegistry &registry)
            : registry_(registry) {}

        // ============================================================================
        DispatchResult CommandDispatcher::dispatch(
            const std::string &commandName,
            const std::vector<std::string> &args) const
        {
            DispatchResult res{0, "", false};

            auto tool = registry_.find(commandName);
            if (!tool.has_value())
            {
                res.exitCode = 127; // (AR) أمر غير موجود (تقليد bash)
                                    // (EN) Command not found (bash convention)
                res.error = "(AR) أمر غير معروف: '" + commandName + "'\n"
                                                                    "(EN) unknown command: '" +
                            commandName + "'";
                return res;
            }

            if (tool->isBuiltin)
            {
                res.error = "(AR) الأداة المدمجة '" + commandName +
                            "' يجب معالجتها قبل dispatcher\n"
                            "(EN) builtin '" +
                            commandName +
                            "' must be handled before dispatcher";
                res.exitCode = 1;
                return res;
            }

            res.dispatched = true;
            res.exitCode = runSubprocess(tool->executable, args);
            return res;
        }

        // ============================================================================
        // (AR) تشغيل العملية الفرعية على ويندوز عبر CreateProcess
        //      نتجنّب system() لأنها تُحلّل عبر cmd.exe وتُسبب مشاكل اقتباس
        // (EN) Run subprocess on Windows via CreateProcess
        //      Avoid system() because it goes through cmd.exe with quoting issues
        // ============================================================================

#ifdef _WIN32

        namespace
        {

            // (AR) إقتباس وسيط لسطر الأوامر بطريقة ويندوز (CommandLineToArgvW backwards)
            // (EN) Quote a single argument Windows-style (reverse of CommandLineToArgvW)
            std::string quoteArg(const std::string &arg)
            {
                if (!arg.empty() &&
                    arg.find_first_of(" \t\n\v\"") == std::string::npos)
                {
                    return arg;
                }
                std::string out = "\"";
                int backslashes = 0;
                for (char c : arg)
                {
                    if (c == '\\')
                    {
                        ++backslashes;
                    }
                    else if (c == '"')
                    {
                        // (AR) ضاعف الـbackslashes ثم اهرب الاقتباس
                        // (EN) double backslashes then escape the quote
                        out.append(backslashes * 2 + 1, '\\');
                        out += '"';
                        backslashes = 0;
                    }
                    else
                    {
                        out.append(backslashes, '\\');
                        backslashes = 0;
                        out += c;
                    }
                }
                out.append(backslashes * 2, '\\');
                out += '"';
                return out;
            }

            // (AR) تحويل UTF-8 إلى wchar_t لاستعمالها مع CreateProcessW
            // (EN) Convert UTF-8 to wchar_t for CreateProcessW
            std::wstring utf8ToWide(const std::string &s)
            {
                if (s.empty())
                    return L"";
                int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(),
                                              static_cast<int>(s.size()), nullptr, 0);
                if (len <= 0)
                    return L"";
                std::wstring out(static_cast<size_t>(len), L'\0');
                MultiByteToWideChar(CP_UTF8, 0, s.c_str(),
                                    static_cast<int>(s.size()), out.data(), len);
                return out;
            }

        } // anonymous

        int CommandDispatcher::runSubprocess(
            const std::string &executable,
            const std::vector<std::string> &args) const
        {
            // (AR) بناء سطر أوامر مُقتَبس بشكل صحيح بـ UTF-8 ثم تحويله إلى UTF-16
            //      هذا يضمن نقل أسماء ملفات عربية بأمان إلى الأداة الفرعية.
            // (EN) Build properly-quoted UTF-8 command line then convert to UTF-16
            //      to safely pass Arabic filenames to the child tool.
            std::ostringstream cmdLine;
            cmdLine << quoteArg(executable);
            for (const auto &a : args)
            {
                cmdLine << ' ' << quoteArg(a);
            }
            std::wstring wcmd = utf8ToWide(cmdLine.str());
            std::wstring wexe = utf8ToWide(executable);

            STARTUPINFOW si{};
            si.cb = sizeof(si);
            PROCESS_INFORMATION pi{};

            // (AR) CreateProcessW يحتاج buffer قابل للتعديل لسطر الأوامر
            // (EN) CreateProcessW needs a writable buffer for command line
            std::vector<wchar_t> cmdBuf(wcmd.begin(), wcmd.end());
            cmdBuf.push_back(L'\0');

            BOOL ok = CreateProcessW(
                wexe.c_str(),  // lpApplicationName (UTF-16)
                cmdBuf.data(), // lpCommandLine (UTF-16)
                nullptr, nullptr,
                TRUE, // bInheritHandles (للسماح بـ stdin/stdout)
                0,
                nullptr, nullptr,
                &si, &pi);

            if (!ok)
            {
                return 127;
            }

            WaitForSingleObject(pi.hProcess, INFINITE);
            DWORD exitCode = 1;
            GetExitCodeProcess(pi.hProcess, &exitCode);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return static_cast<int>(exitCode);
        }

#else // !_WIN32

        int CommandDispatcher::runSubprocess(
            const std::string &executable,
            const std::vector<std::string> &args) const
        {
            // (AR) تنفيذ POSIX عبر fork/exec
            // (EN) POSIX implementation via fork/exec
            pid_t pid = fork();
            if (pid < 0)
                return 127;
            if (pid == 0)
            {
                // child
                std::vector<char *> argv;
                argv.push_back(const_cast<char *>(executable.c_str()));
                for (const auto &a : args)
                    argv.push_back(const_cast<char *>(a.c_str()));
                argv.push_back(nullptr);
                execv(executable.c_str(), argv.data());
                _exit(127);
            }
            int status = 0;
            waitpid(pid, &status, 0);
            return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
        }

#endif

    } // namespace Hub
} // namespace Sad
