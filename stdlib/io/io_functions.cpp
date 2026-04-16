/**
 * @file io_functions.cpp
 * @brief (AR) تطبيق دوال الإدخال والإخراج / (EN) Implementation of I/O Functions
 *
 * Phase 1 of Standard Library Implementation:
 * Complete implementation of core I/O operations.
 *
 * @author S Language Development Team
 * @date November 22, 2025
 * @version 1.0
 */

#include <string>
#include "io/io_functions.h"
#include "object_instance.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include "sad_type_system.h"

#ifdef _WIN32
#include <windows.h>
#endif
#ifdef _WIN32
#include <windows.h>
#include <io.h>
// Windows.h defines VOID macro which conflicts with ValueType::VOID
#ifdef VOID
#undef VOID
#endif
#endif

namespace Sad
{
    namespace StdLib
    {
        namespace IO
        {

            // ====================================================================
            // Helper Methods
            // ====================================================================

            /**
             * @brief (AR) إزالة BOM (علامة ترتيب البايت) UTF-8 من بداية النص
             * @brief (EN) Strip UTF-8 BOM (Byte Order Mark) from the beginning of a string
             *
             * PowerShell on Windows prepends a UTF-8 BOM (EF BB BF) when piping
             * strings to stdin. This corrupts string-to-number conversions like لرقم().
             */
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

            /**
             * @brief (AR) قراءة سطر من stdin مع دعم Windows الصحيح
             * @brief (EN) Read a line from stdin with proper Windows support
             *
             * (AR) مشكلة ConPTY (طرفية VS Code):
             * ConPTY يُفعّل ENABLE_VIRTUAL_TERMINAL_INPUT (0x0200) مما يجعل
             * تسلسلات هروب ANSI (مثل رد موقع المؤشر \x1b[row;colR) تظهر كإدخال عادي.
             * هذا يسبب قراءة بيانات وهمية بدلاً من انتظار إدخال المستخدم.
             *
             * (EN) ConPTY (VS Code terminal) problem:
             * ConPTY enables ENABLE_VIRTUAL_TERMINAL_INPUT (0x0200) which causes
             * ANSI escape sequences (like cursor position response \x1b[row;colR)
             * to appear as regular stdin input. This causes phantom data reads.
             *
             * الحل: تعطيل VT input مؤقتاً + تنظيف المخازن المؤقتة + ضبط وضع القراءة
             * Fix: Temporarily disable VT input + flush buffers + set proper read mode
             */
            static bool readLineFromStdin(std::string &result)
            {
                // (AR) ضمان ظهور أي نص طُبع (مثل رسالة التوجيه) قبل القراءة
                // (EN) Ensure any printed text (like prompt) is flushed before reading
                fflush(stdout);
                std::cout.flush();

#ifdef _WIN32
                HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
                DWORD oldMode = 0;
                bool isConsole = (hStdin != INVALID_HANDLE_VALUE && GetConsoleMode(hStdin, &oldMode));

                if (isConsole)
                {
                    // (AR) تعطيل ENABLE_VIRTUAL_TERMINAL_INPUT لمنع تسلسلات الهروب من الظهور كإدخال
                    // (EN) Disable ENABLE_VIRTUAL_TERMINAL_INPUT to prevent escape sequences as input
                    DWORD newMode = (oldMode & ~0x0200u);
                    // (AR) ضمان: معالجة إدخال + قراءة سطر كامل + صدى الكتابة
                    // (EN) Ensure: processed input + line input + echo
                    newMode |= (ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
                    SetConsoleMode(hStdin, newMode);

                    // (AR) انتظار قصير ليصل أي escape sequence متأخر من ConPTY
                    // (EN) Brief wait for any late ConPTY escape sequences to arrive
                    Sleep(50);

                    // (AR) تنظيف كل أحداث الإدخال المُعلّقة (بما فيها بقايا escape sequences)
                    // (EN) Drain all pending input events (including leftover escape sequences)
                    DWORD numEvents = 0;
                    while (GetNumberOfConsoleInputEvents(hStdin, &numEvents) && numEvents > 0)
                    {
                        INPUT_RECORD irBuf[256];
                        DWORD eventsRead = 0;
                        ReadConsoleInputW(hStdin, irBuf, (numEvents < 256 ? numEvents : 256), &eventsRead);
                        if (eventsRead == 0)
                            break;
                    }

                    // (AR) تنظيف مخزن C Runtime
                    // (EN) Flush C runtime's stdin buffer
                    fflush(stdin);
                }
#endif

                bool ok = false;
                if (std::getline(std::cin, result))
                {
                    result = stripUtf8Bom(result);
                    ok = true;
                }

#ifdef _WIN32
                if (isConsole)
                {
                    // (AR) استعادة وضع الطرفية الأصلي
                    // (EN) Restore original console mode
                    SetConsoleMode(hStdin, oldMode);
                }
#endif

                return ok;
            }

            std::string IOFunctions::processEscapeSequences(const std::string &input)
            {
                /**
                 * (AR) معالجة تسلسلات الهروب في النصوص / (EN) Process escape sequences in strings
                 * Converts \n, \t, \r, \\, \", etc. to their actual characters
                 */
                std::string result;
                result.reserve(input.size());

                for (size_t i = 0; i < input.size(); ++i)
                {
                    if (input[i] == '\\' && i + 1 < input.size())
                    {
                        char next = input[i + 1];
                        switch (next)
                        {
                        case 'n':
                            result += '\n';
                            i++;
                            break; // Newline
                        case 't':
                            result += '\t';
                            i++;
                            break; // Tab
                        case 'r':
                            result += '\r';
                            i++;
                            break; // Carriage return
                        case '\\':
                            result += '\\';
                            i++;
                            break; // Backslash
                        case '"':
                            result += '"';
                            i++;
                            break; // Quote
                        case '\'':
                            result += '\'';
                            i++;
                            break; // Single quote
                        case '0':
                            result += '\0';
                            i++;
                            break; // Null character
                        default:
                            result += input[i];
                            break; // Not an escape, keep backslash
                        }
                    }
                    else
                    {
                        result += input[i];
                    }
                }

                return result;
            }

            // (AR) تعريف static callback لتحويل الكائنات عبر عامل نص()
            // (EN) Static callback definition for object conversion via نص() operator
            IOFunctions::ObjectToStringCallback IOFunctions::objectToStringCallback_ = nullptr;

            void IOFunctions::setObjectToStringCallback(ObjectToStringCallback callback)
            {
                objectToStringCallback_ = std::move(callback);
            }

            std::string IOFunctions::valueToString(const Data::Value &value, int depth)
            {
                // (AR) حماية من التكرار اللانهائي عند وجود كائنات دورية
                // (EN) Guard against infinite recursion with cyclic objects
                if (depth > 10)
                {
                    return "(...)";
                }

                using VT = Types::SadTypeKind;

                switch (value.getKind())
                {
                case VT::Void:
                    return "\u0644\u0627\u0634\u064a\u0621"; // لاشيء

                case VT::Integer:
                    return std::to_string(value.toInt64());

                case VT::Float:
                {
                    std::ostringstream oss;
                    // Show enough precision for doubles, remove trailing zeros
                    oss << std::fixed << std::setprecision(6) << value.toDouble();
                    std::string result = oss.str();

                    // Remove trailing zeros after decimal point
                    if (result.find('.') != std::string::npos)
                    {
                        result.erase(result.find_last_not_of('0') + 1, std::string::npos);
                        if (result.back() == '.')
                        {
                            result.pop_back();
                        }
                    }
                    return result;
                }

                case VT::String:
                    // (AR) لا نعالج escape sequences هنا لأن المحلل المعجمي قام بذلك بالفعل
                    // (EN) Don't process escape sequences here - Lexer already handled them
                    // Regular strings: Lexer converted \n → newline character
                    // Raw strings: Lexer kept backslashes literally (r"\n" stays as '\' + 'n')
                    return value.toString();

                case VT::Boolean:
                    return value.toBool() ? "صحيح" : "خطأ"; // Arabic: "true" : "false"

                case VT::Array:
                {
                    std::ostringstream oss;
                    oss << "[";
                    try
                    {
                        auto arr = value.toArray();
                        for (size_t i = 0; i < arr.size(); ++i)
                        {
                            if (i > 0)
                                oss << ", ";
                            oss << valueToString(arr[i], depth + 1);
                        }
                    }
                    catch (...)
                    {
                        oss << "...";
                    }
                    oss << "]";
                    return oss.str();
                }

                case VT::Map:
                {
                    std::ostringstream oss;
                    oss << "{";
                    try
                    {
                        auto map = value.toMap();
                        bool first = true;
                        for (const auto &[key, val] : map)
                        {
                            if (!first)
                                oss << ", ";
                            oss << "\"" << key << "\": " << valueToString(val, depth + 1);
                            first = false;
                        }
                    }
                    catch (...)
                    {
                        oss << "...";
                    }
                    oss << "}";
                    return oss.str();
                }

                // ═══════════════════════════════════════════════════════════════
                // (AR) كائن (صنف) — يطبع اسم الصنف وحقوله
                //      إذا كان الكائن يملك حقل __نص_عرض__ يستخدمه
                //      وإلا يطبع الصنف{الحقول} بالشكل الافتراضي
                // (EN) Object (class instance) — prints class name and fields
                //      If object has __نص_عرض__ field, uses it
                //      Otherwise prints ClassName{fields} as default
                // ═══════════════════════════════════════════════════════════════
                case VT::Class:
                {
                    try
                    {
                        // (AR) أولاً: استدعاء عامل نص() عبر callback إذا كان مسجلاً
                        // (EN) First: call نص() operator via callback if registered
                        if (objectToStringCallback_)
                        {
                            std::string result = objectToStringCallback_(value);
                            if (!result.empty())
                            {
                                return result;
                            }
                        }

                        auto objPtr = value.toObject();
                        if (!objPtr)
                            return "(كائن فارغ)";

                        // (AR) التحقق من وجود تمثيل نصي مخصص
                        // (EN) Check for custom string representation
                        auto customIt = objPtr->fields.find("__نص_عرض__");
                        if (customIt != objPtr->fields.end() && customIt->second.isString())
                        {
                            return customIt->second.toString();
                        }

                        std::ostringstream oss;
                        oss << objPtr->getClassName() << "{";
                        bool first = true;
                        for (const auto &[key, val] : objPtr->fields)
                        {
                            if (key.find("__") == 0)
                                continue; // (AR) تخطي الحقول الداخلية
                            if (!first)
                                oss << ", ";
                            oss << key << ": " << valueToString(val, depth + 1);
                            first = false;
                        }
                        oss << "}";
                        return oss.str();
                    }
                    catch (...)
                    {
                        return "(كائن)";
                    }
                }

                case VT::Function:
                {
                    // (AR) مرجع دالة — يطبع معلومات الدالة
                    // (EN) Function reference — print function info
                    try
                    {
                        auto funcRef = value.toFunction();
                        if (funcRef)
                        {
                            return funcRef->toString();
                        }
                    }
                    catch (...)
                    {
                    }
                    return "<دالة>";
                }

                default:
                    return "(unknown)";
                }
            }

            std::string IOFunctions::formatValue(const Data::Value &value)
            {
                // For now, formatting is same as toString
                // Can be extended for more complex formatting in future
                return valueToString(value);
            }

            bool IOFunctions::validateArguments(const std::vector<Data::Value> &args,
                                                size_t minArgs,
                                                int maxArgs)
            {
                if (args.size() < minArgs)
                {
                    std::ostringstream error;
                    error << "(AR) عدد معاملات غير صحيح: متوقع "
                          << minArgs;
                    if (maxArgs > 0)
                    {
                        error << " إلى " << maxArgs;
                    }
                    error << " لكن حصلنا على " << args.size()
                          << " | (EN) Incorrect argument count: expected "
                          << minArgs;
                    if (maxArgs > 0)
                    {
                        error << " to " << maxArgs;
                    }
                    error << " but got " << args.size();
                    throw std::invalid_argument(error.str());
                }

                if (maxArgs > 0 && args.size() > static_cast<size_t>(maxArgs))
                {
                    std::ostringstream error;
                    error << "(AR) عدد معاملات غير صحيح: متوقع أقصى "
                          << maxArgs << " لكن حصلنا على " << args.size()
                          << " | (EN) Incorrect argument count: expected maximum "
                          << maxArgs << " but got " << args.size();
                    throw std::invalid_argument(error.str());
                }

                return true;
            }

            // ====================================================================
            // Public I/O Functions
            // ====================================================================

            Data::Value IOFunctions::print(const std::vector<Data::Value> &args)
            {
                /**
                 * (AR) وظيفة طبع () تطبع جميع المعاملات مفصولة بمسافة بدون إضافة سطر جديد
                 * (EN) Function print() outputs all arguments space-separated without newline
                 */

                try
                {
                    for (size_t i = 0; i < args.size(); ++i)
                    {
                        if (i > 0)
                        {
#ifdef __EMSCRIPTEN__
                            // (AR) في WASM نستخدم std::cout للتوافق مع OutputCapture
                            // (EN) In WASM use std::cout for OutputCapture compatibility
                            std::cout << " ";
#else
                            // (AR) استخدام fwrite لتجنب مشاكل ترميز Unicode على Windows
                            // (EN) Use fwrite to avoid Unicode encoding issues on Windows
                            fwrite(" ", 1, 1, stdout);
#endif
                        }
                        std::string str = valueToString(args[i]);
#ifdef __EMSCRIPTEN__
                        std::cout << str;
#else
                        fwrite(str.c_str(), 1, str.size(), stdout);
#endif
                    }
#ifdef __EMSCRIPTEN__
                    std::cout << std::flush;
#else
                    fflush(stdout);
#endif
                }
                catch (const std::exception &e)
                {
                    throw std::runtime_error(
                        std::string("(AR) خطأ في طبع() / (EN) Error in print(): ") + e.what());
                }

                return Data::Value(); // Return VOID
            }

            Data::Value IOFunctions::println(const std::vector<Data::Value> &args)
            {
                /**
                 * (AR) وظيفة طبع_سطر () تطبع جميع المعاملات مفصولة بمسافة وتضيف سطر جديد
                 * (EN) Function println() outputs all arguments space-separated with newline
                 */

                try
                {
                    for (size_t i = 0; i < args.size(); ++i)
                    {
                        if (i > 0)
                        {
#ifdef __EMSCRIPTEN__
                            std::cout << " ";
#else
                            fwrite(" ", 1, 1, stdout);
#endif
                        }
                        std::string str = valueToString(args[i]);
#ifdef __EMSCRIPTEN__
                        std::cout << str;
#else
                        fwrite(str.c_str(), 1, str.size(), stdout);
#endif
                    }
                    // (AR) إضافة سطر جديد
                    // (EN) Add newline
#ifdef __EMSCRIPTEN__
                    std::cout << "\n"
                              << std::flush;
#else
                    fwrite("\n", 1, 1, stdout);
                    fflush(stdout);
#endif
                }
                catch (const std::exception &e)
                {
                    throw std::runtime_error(
                        std::string("(AR) خطأ في طبع_سطر() / (EN) Error in println(): ") + e.what());
                }

                return Data::Value(); // Return VOID
            }

            Data::Value IOFunctions::input(const std::vector<Data::Value> &args)
            {
                /**
                 * (AR) وظيفة أدخل () تقرأ إدخال المستخدم كنص
                 * (EN) Function input() reads user input as string
                 *
                 * إذا تم توفير معامل، يتم طباعته كرسالة موجهة قبل قراءة الإدخال
                 * If an argument is provided, it's displayed as prompt before reading
                 *
                 * (AR) ملاحظة: على Windows، نستخدم ReadConsoleW للإدخال التفاعلي
                 *       لتجنب مشاكل SetConsoleCP(CP_UTF8) مع std::getline
                 *       كما نزيل BOM الذي يضيفه PowerShell عند الأنبوب
                 * (EN) Note: On Windows, we use ReadConsoleW for interactive input
                 *       to avoid SetConsoleCP(CP_UTF8) issues with std::getline
                 *       We also strip the UTF-8 BOM that PowerShell adds when piping
                 */

                try
                {
                    // If prompt provided, display it
                    if (!args.empty())
                    {
                        std::cout << valueToString(args[0]);
                        std::cout.flush();
                    }

                    std::string input_line;
                    if (readLineFromStdin(input_line))
                    {
                        return Data::Value(input_line);
                    }
                    else
                    {
                        // EOF or error
                        return Data::Value("");
                    }
                }
                catch (const std::exception &e)
                {
                    throw std::runtime_error(
                        std::string("(AR) خطأ في أدخل() / (EN) Error in input(): ") + e.what());
                }
            }

            Data::Value IOFunctions::readLine(const std::vector<Data::Value> &args)
            {
                /**
                 * (AR) وظيفة قراءة_سطر () - نفس وظيفة أدخل () بشكل أساسي
                 * (EN) Function readLine() - essentially the same as input()
                 *
                 * توفر واجهة أكثر وضوحاً للقراءة من المدخلات
                 * Provides a clearer interface for reading from input
                 */

                try
                {
                    // If prompt provided, display it
                    if (!args.empty())
                    {
                        std::cout << valueToString(args[0]);
                        std::cout.flush();
                    }

                    std::string line;
                    if (readLineFromStdin(line))
                    {
                        return Data::Value(line);
                    }
                    else
                    {
                        // EOF or error
                        return Data::Value("");
                    }
                }
                catch (const std::exception &e)
                {
                    throw std::runtime_error(
                        std::string("(AR) خطأ في قراءة_سطر() / (EN) Error in readLine(): ") + e.what());
                }
            }

            Data::Value IOFunctions::clear(const std::vector<Data::Value> &args)
            {
                /**
                 * (AR) وظيفة مسح_الشاشة () - مسح شاشة الكونسول
                 * (EN) Function clear() - clear the console screen
                 *
                 * تعمل على معظم الأنظمة (Windows, Linux, macOS)
                 * Works on most systems (Windows, Linux, macOS)
                 */

                try
                {
#ifdef _WIN32
                    // (AR) استخدام Windows Console API بدلاً من std::system لتجنب ثغرات الحقن
                    // (EN) Use Windows Console API instead of std::system to avoid injection vulnerabilities
                    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                    if (hConsole != INVALID_HANDLE_VALUE)
                    {
                        CONSOLE_SCREEN_BUFFER_INFO csbi;
                        if (GetConsoleScreenBufferInfo(hConsole, &csbi))
                        {
                            DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
                            COORD homeCoords = {0, 0};
                            DWORD count;
                            FillConsoleOutputCharacterW(hConsole, L' ', cellCount, homeCoords, &count);
                            FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoords, &count);
                            SetConsoleCursorPosition(hConsole, homeCoords);
                        }
                    }
#else
                    // Unix/Linux/macOS — ANSI escape
                    std::cout << "\033[2J\033[H";
#endif
                    std::cout.flush();
                }
                catch (const std::exception &e)
                {
                    throw std::runtime_error(
                        std::string("(AR) خطأ في مسح_الشاشة() / (EN) Error in clear(): ") + e.what());
                }

                return Data::Value(); // Return VOID
            }

        } // namespace IO
    } // namespace StdLib
} // namespace Sad
