/**
 * @file other_functions.cpp
 * @brief تنفيذ دوال المكتبة القياسية الإضافية / Implementation of Other Standard Library Functions
 *
 * @author S Language Development Team
 * @date December 2024
 */

#include "other_functions.h"
#include "user_thrown.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <random>

#ifdef _WIN32
#include <windows.h>
#endif
// (AR) EM-CPP: حامل خطأ الكتالوج — مطلوب على كل المنصّات (Sad::Errors::BuiltinError
//      يُستعمل دون شرط منصّة). كان محبوسًا خطأً داخل كتلة _WIN32 فكسر بناء Linux.
// (EN) Catalog error carrier — needed on all platforms (Sad::Errors::BuiltinError
//      is used unconditionally). It was wrongly trapped inside the _WIN32 block,
//      breaking the Linux build.
#include "builtin_error.h"

namespace Sad
{
    namespace StdLib
    {
        namespace Core
        {

            using Data::Value;

            /**
             * @brief دالة مساعدة للتحقق من عدد المعاملات
             * Helper function to validate argument count
             */
            static bool validateArgCount(const std::vector<Value> &args, size_t min, size_t max, const std::string &funcName)
            {
                if (args.size() < min || args.size() > max)
                {
                    std::cerr << "خطأ / Error: " << funcName << " يتوقع / expects " << min;
                    if (min != max)
                    {
                        std::cerr << "-" << max;
                    }
                    std::cerr << " معامل / argument(s), حصل على / got " << args.size() << std::endl;
                    return false;
                }
                return true;
            }

            // =============================================================================
            // Helper: Strip UTF-8 BOM / إزالة BOM من UTF-8
            // PowerShell on Windows prepends EF BB BF when piping to stdin
            // =============================================================================
            static std::string stripUtf8Bom(const std::string &str)
            {
                if (str.size() >= 3 &&
                    static_cast<unsigned char>(str[0]) == 0xEF &&
                    static_cast<unsigned char>(str[1]) == 0xBB &&
                    static_cast<unsigned char>(str[2]) == 0xBF)
                {
                    return str.substr(3);
                }
                return str;
            }

            // =============================================================================
            // Helper: Read line from stdin with Windows support
            // On Windows console, uses ReadConsoleW to avoid SetConsoleCP(CP_UTF8) bugs
            // On pipes/files, uses std::getline and strips UTF-8 BOM
            // =============================================================================
            static bool readLineFromStdin(std::string &result)
            {
#ifdef _WIN32
                HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
                DWORD mode;
                if (hStdin != INVALID_HANDLE_VALUE && GetConsoleMode(hStdin, &mode))
                {
                    // Interactive console: use ReadConsoleW to avoid CP_UTF8 bugs
                    std::wstring wline;
                    wchar_t wch;
                    DWORD charsRead;
                    while (ReadConsoleW(hStdin, &wch, 1, &charsRead, NULL) && charsRead > 0)
                    {
                        if (wch == L'\n')
                            break;
                        if (wch == L'\r')
                            continue;
                        wline += wch;
                    }
                    if (charsRead == 0 && wline.empty())
                        return false;
                    if (wline.empty())
                    {
                        result = "";
                        return true;
                    }
                    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wline.c_str(), (int)wline.size(), NULL, 0, NULL, NULL);
                    result.resize(utf8Len);
                    WideCharToMultiByte(CP_UTF8, 0, wline.c_str(), (int)wline.size(), &result[0], utf8Len, NULL, NULL);
                    return true;
                }
#endif
                if (std::getline(std::cin, result))
                {
                    result = stripUtf8Bom(result);
                    return true;
                }
                return false;
            }

            // =============================================================================
            // input() - قراءة مدخل من المستخدم / Read input from user
            // =============================================================================

            Value input(const std::vector<Value> &args)
            {
                // التحقق من عدد المعاملات (0 أو 1)
                // Validate argument count (0 or 1)
                if (!validateArgCount(args, 0, 1, "input"))
                {
                    return Value(""); // إرجاع نص فارغ عند الخطأ / Return empty string on error
                }

                // إذا كان هناك محث، اطبعه
                // If there's a prompt, print it
                if (args.size() == 1)
                {
                    std::string prompt = args[0].toString();
                    std::cout << prompt;
                    std::cout.flush(); // تأكد من طباعة المحث فوراً / Ensure prompt is printed immediately
                }

                // قراءة سطر من المستخدم (مع دعم Windows الصحيح)
                // Read line from user (with proper Windows support)
                std::string line;
                if (readLineFromStdin(line))
                {
                    return Value(line);
                }

                // إذا فشلت القراءة، أرجع نص فارغ
                // If read fails, return empty string
                return Value("");
            }

            // =============================================================================
            // random() - توليد رقم عشوائي / Generate random number
            // =============================================================================

            Value random(const std::vector<Value> &args)
            {
                // (AR) استخدام مولد أرقام عشوائية عالي الجودة (thread-safe)
                // (EN) Use high-quality random number generator (thread-safe)
                static thread_local std::mt19937 rng(std::random_device{}());

                // التحقق من عدد المعاملات (0، 1، أو 2)
                // Validate argument count (0, 1, or 2)
                if (!validateArgCount(args, 0, 2, "random"))
                {
                    return Value(0); // إرجاع 0 عند الخطأ / Return 0 on error
                }

                // random() - عدد عشري بين 0.0 و 1.0
                // random() - double between 0.0 and 1.0
                if (args.size() == 0)
                {
                    std::uniform_real_distribution<double> dist(0.0, 1.0);
                    return Value(dist(rng));
                }

                // random(max) - رقم بين 0 و max-1
                // random(max) - number between 0 and max-1
                if (args.size() == 1)
                {
                    int max = args[0].toInt();
                    if (max <= 0)
                    {
                        throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                    }
                    std::uniform_int_distribution<int> dist(0, max - 1);
                    return Value(dist(rng));
                }

                // random(min, max) - رقم بين min و max-1
                // random(min, max) - number between min and max-1
                int min = args[0].toInt();
                int max = args[1].toInt();

                if (min >= max)
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                }

                std::uniform_int_distribution<int> dist(min, max - 1);
                return Value(dist(rng));
            }

            // =============================================================================
            // sleep() - إيقاف التنفيذ مؤقتاً / Pause execution temporarily
            // =============================================================================

            Value sleep(const std::vector<Value> &args)
            {
                // التحقق من عدد المعاملات (1 بالضبط)
                // Validate argument count (exactly 1)
                if (!validateArgCount(args, 1, 1, "sleep"))
                {
                    return Value(); // إرجاع void عند الخطأ / Return void on error
                }

                // الحصول على عدد الميلي ثانية
                // Get number of milliseconds
                int milliseconds = args[0].toInt();

                if (milliseconds < 0)
                {
                    std::cerr << "خطأ / Error: sleep() المدة يجب أن تكون غير سالبة / duration must be non-negative" << std::endl;
                    return Value();
                }

                // إيقاف التنفيذ
                // Pause execution
                std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));

                return Value(); // void
            }

            // =============================================================================
            // exit() - إنهاء البرنامج / Terminate program
            // =============================================================================

            Value exit(const std::vector<Value> &args)
            {
                // التحقق من عدد المعاملات (0 أو 1)
                // Validate argument count (0 or 1)
                if (!validateArgCount(args, 0, 1, "exit"))
                {
                    throw Sad::Interpreter::ExitException(1);
                }

                // الحصول على كود الخروج (افتراضي: 0)
                // Get exit code (default: 0)
                int exit_code = 0;
                if (args.size() == 1)
                {
                    exit_code = args[0].toInt();
                }

                // (AR) إلقاء استثناء خروج نظيف بدلاً من std::exit() لضمان تنظيف RAII
                // (EN) Throw clean exit exception instead of std::exit() to ensure RAII cleanup
                throw Sad::Interpreter::ExitException(exit_code);

                // هذا السطر لن يُنفذ أبداً
                // This line will never execute
                return Value();
            }

            // =============================================================================
            // assert() - التحقق من شرط / Assert a condition
            // =============================================================================

            Value assert(const std::vector<Value> &args)
            {
                // التحقق من عدد المعاملات (1 أو 2)
                // Validate argument count (1 or 2)
                if (!validateArgCount(args, 1, 2, "assert"))
                {
                    return Value(); // إرجاع void عند الخطأ / Return void on error
                }

                // الحصول على الشرط
                // Get condition
                bool condition = args[0].toBool();

                // إذا كان الشرط صحيحاً، لا نفعل شيئاً
                // If condition is true, do nothing
                if (condition)
                {
                    return Value();
                }

                // الشرط خاطئ - نطبع رسالة الخطأ
                // Condition is false - print error message
                std::cerr << "❌ فشل التحقق / Assertion Failed!" << std::endl;

                if (args.size() == 2)
                {
                    std::string message = args[1].toString();
                    std::cerr << "   الرسالة / Message: " << message << std::endl;
                }

                // نوقف البرنامج
                // Stop the program
                std::cerr << "   إيقاف البرنامج / Stopping program..." << std::endl;
                std::exit(1);

                // هذا السطر لن يُنفذ أبداً
                // This line will never execute
                return Value();
            }

        } // namespace Core
    } // namespace StdLib
} // namespace Sad
