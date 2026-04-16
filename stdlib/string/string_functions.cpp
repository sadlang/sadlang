/**
 * @file string_functions.cpp
 * @brief (AR) تنفيذ دوال معالجة النصوص / (EN) Implementation of String Manipulation Functions
 *
 * This file implements all 12 string processing functions for Sad Language.
 * All functions handle UTF-8 encoding and Arabic text correctly.
 *
 * @author S Language Development Team
 * @date November 22, 2025
 * @version 1.0
 */

#include "string/string_functions.h"
#include "sad_type_system.h"
#include <algorithm>
#include <sstream>
#include <cctype>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
// (AR) إلغاء ماكرو VOID الخاص بويندوز لتجنب التعارض
// (EN) Undef Windows VOID macro to avoid conflict with ValueType::VOID
#ifdef VOID
#undef VOID
#endif
#endif

namespace Sad
{
    namespace StdLib
    {
        namespace String
        {

            // ============================================================================
            // (AR) الدوال المساعدة الخاصة / (EN) Private Helper Functions
            // ============================================================================

            /**
             * @brief (AR) حساب طول النص مع دعم UTF-8
             * @brief (EN) Calculate string length with UTF-8 support
             *
             * Counts actual characters, not bytes. Handles multi-byte UTF-8 properly.
             */
            size_t StringFunctions::utf8Length(const std::string &str)
            {
                size_t length = 0;
                size_t strLen = str.length();
                for (size_t i = 0; i < strLen;)
                {
                    unsigned char c = str[i];
                    size_t charBytes = 1;
                    // (AR) حساب عدد البايتات في الحرف مع التحقق من الحدود
                    // (EN) Count bytes in character with bounds checking
                    if ((c & 0x80) == 0)
                    { // 1-byte character (ASCII)
                        charBytes = 1;
                    }
                    else if ((c & 0xE0) == 0xC0)
                    { // 2-byte character
                        charBytes = 2;
                    }
                    else if ((c & 0xF0) == 0xE0)
                    { // 3-byte character (Arabic, etc.)
                        charBytes = 3;
                    }
                    else if ((c & 0xF8) == 0xF0)
                    { // 4-byte character
                        charBytes = 4;
                    }
                    // (AR) التحقق من أن البايتات المتبقية كافية
                    // (EN) Ensure remaining bytes are sufficient
                    if (i + charBytes > strLen)
                    {
                        break; // Truncated UTF-8 sequence
                    }
                    i += charBytes;
                    length++;
                }
                return length;
            }

            /**
             * @brief (AR) استخراج نص فرعي مع دعم UTF-8
             * @brief (EN) Extract substring with UTF-8 support
             */
            std::string StringFunctions::utf8Substring(const std::string &str, size_t start, size_t length)
            {
                if (start >= utf8Length(str))
                {
                    return "";
                }

                // (AR) تحويل موقع الحرف إلى موقع البايت
                // (EN) Convert character position to byte position
                size_t byteStart = 0;
                size_t charCount = 0;

                for (size_t i = 0; i < str.length() && charCount < start;)
                {
                    unsigned char c = str[i];
                    if ((c & 0x80) == 0)
                    {
                        i += 1;
                    }
                    else if ((c & 0xE0) == 0xC0)
                    {
                        i += 2;
                    }
                    else if ((c & 0xF0) == 0xE0)
                    {
                        i += 3;
                    }
                    else if ((c & 0xF8) == 0xF0)
                    {
                        i += 4;
                    }
                    else
                    {
                        i += 1;
                    }
                    charCount++;
                    byteStart = i;
                }

                // (AR) إذا كان الطول -1، نأخذ حتى النهاية
                // (EN) If length is -1, take until end
                if (length == static_cast<size_t>(-1))
                {
                    return str.substr(byteStart);
                }

                // (AR) حساب عدد البايتات المقابلة للطول المطلوب
                // (EN) Calculate byte count for requested length
                size_t byteLength = 0;
                charCount = 0;

                for (size_t i = byteStart; i < str.length() && charCount < length;)
                {
                    unsigned char c = str[i];
                    size_t charBytes = 1;

                    if ((c & 0x80) == 0)
                    {
                        charBytes = 1;
                    }
                    else if ((c & 0xE0) == 0xC0)
                    {
                        charBytes = 2;
                    }
                    else if ((c & 0xF0) == 0xE0)
                    {
                        charBytes = 3;
                    }
                    else if ((c & 0xF8) == 0xF0)
                    {
                        charBytes = 4;
                    }

                    i += charBytes;
                    byteLength += charBytes;
                    charCount++;
                }

                return str.substr(byteStart, byteLength);
            }

            /**
             * @brief (AR) التحقق من صحة المعاملات
             * @brief (EN) Validate function arguments
             */
            bool StringFunctions::validateArguments(const std::vector<Data::Value> &args,
                                                    size_t minArgs,
                                                    int maxArgs)
            {
                if (args.size() < minArgs)
                {
                    throw std::invalid_argument("Too few arguments");
                }
                if (maxArgs >= 0 && args.size() > static_cast<size_t>(maxArgs))
                {
                    throw std::invalid_argument("Too many arguments");
                }
                return true;
            }

            // ============================================================================
            // (AR) تنفيذ الدوال العامة / (EN) Public Function Implementations
            // ============================================================================

            /**
             * @brief (AR) طول - الحصول على طول النص
             * @brief (EN) Length - Get string length
             */
            Data::Value StringFunctions::length(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 1, 1);

                // (AR) الحصول على النص
                // (EN) Get string
                std::string str = args[0].toString();

                // (AR) حساب الطول مع دعم UTF-8
                // (EN) Calculate length with UTF-8 support
                int len = static_cast<int>(utf8Length(str));

                return Data::Value(len);
            }

            /**
             * @brief (AR) بحث - البحث عن نص فرعي
             * @brief (EN) Find - Find substring position
             */
            Data::Value StringFunctions::find(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات (نحتاج 2 على الأقل، 3 كحد أقصى)
                // (EN) Validate arguments (need at least 2, max 3)
                validateArguments(args, 2, 3);

                std::string mainStr = args[0].toString();
                std::string subStr = args[1].toString();
                size_t startBytePos = 0;

                if (args.size() == 3)
                {
                    // (AR) تحويل موقع الحرف إلى موقع البايت (UTF-8)
                    // (EN) Convert character position to byte position (UTF-8)
                    size_t charIdx = static_cast<size_t>(args[2].toInt());
                    size_t byteIdx = 0;
                    size_t charCount = 0;
                    while (byteIdx < mainStr.length() && charCount < charIdx)
                    {
                        unsigned char c = mainStr[byteIdx];
                        if ((c & 0x80) == 0)
                            byteIdx += 1;
                        else if ((c & 0xE0) == 0xC0)
                            byteIdx += 2;
                        else if ((c & 0xF0) == 0xE0)
                            byteIdx += 3;
                        else if ((c & 0xF8) == 0xF0)
                            byteIdx += 4;
                        else
                            byteIdx += 1;
                        charCount++;
                    }
                    startBytePos = byteIdx;
                }

                // (AR) البحث عن النص الفرعي
                // (EN) Search for substring
                size_t pos = mainStr.find(subStr, startBytePos);

                if (pos == std::string::npos)
                {
                    return Data::Value(-1);
                }

                // (AR) تحويل موقع البايت إلى موقع الحرف (UTF-8)
                // (EN) Convert byte position to character position (UTF-8)
                size_t charPos = utf8Length(mainStr.substr(0, pos));

                return Data::Value(static_cast<int>(charPos));
            }

            /**
             * @brief (AR) استبدل - استبدال نص فرعي بآخر
             * @brief (EN) Replace - Replace substring with another
             */
            Data::Value StringFunctions::replace(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 3, 4);

                std::string str = args[0].toString();
                std::string oldText = args[1].toString();
                std::string newText = args[2].toString();
                int count = -1; // -1 = replace all

                if (args.size() == 4)
                {
                    count = args[3].toInt();
                }

                if (oldText.empty())
                {
                    return Data::Value(str);
                }

                std::string result = str;
                size_t pos = 0;
                int replacements = 0;

                while ((pos = result.find(oldText, pos)) != std::string::npos)
                {
                    result.replace(pos, oldText.length(), newText);
                    pos += newText.length();
                    replacements++;

                    if (count > 0 && replacements >= count)
                    {
                        break;
                    }
                }

                return Data::Value(result);
            }

            /**
             * @brief (AR) استخراج - استخراج جزء من النص
             * @brief (EN) Substring - Extract part of string
             */
            Data::Value StringFunctions::substring(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 2, 3);

                std::string str = args[0].toString();
                int startInt = args[1].toInt();
                if (startInt < 0)
                {
                    throw std::invalid_argument(
                        "(AR) خطأ: فهرس البداية لا يمكن أن يكون سالباً / (EN) Start index cannot be negative");
                }
                size_t start = static_cast<size_t>(startInt);
                size_t length = static_cast<size_t>(-1); // -1 = to end

                if (args.size() == 3)
                {
                    int lenInt = args[2].toInt();
                    if (lenInt < 0)
                    {
                        throw std::invalid_argument(
                            "(AR) خطأ: الطول لا يمكن أن يكون سالباً / (EN) Length cannot be negative");
                    }
                    length = static_cast<size_t>(lenInt);
                }

                // (AR) استخراج النص مع دعم UTF-8
                // (EN) Extract substring with UTF-8 support
                std::string result = utf8Substring(str, start, length);

                return Data::Value(result);
            }

            /**
             * @brief (AR) تحويل_صغير - تحويل إلى أحرف صغيرة
             * @brief (EN) ToLower - Convert to lowercase
             */
            Data::Value StringFunctions::toLower(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 1, 1);

                std::string str = args[0].toString();
#ifdef _WIN32
                // (AR) تحويل Unicode باستخدام Windows API
                // (EN) Unicode conversion using Windows API
                int wlen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
                if (wlen > 1)
                {
                    std::wstring wstr(wlen - 1, 0);
                    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], wlen);
                    CharLowerW(&wstr[0]);
                    int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
                    std::string result(len - 1, 0);
                    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], len, nullptr, nullptr);
                    return Data::Value(result);
                }
#endif
                // (AR) احتياطي: تحويل ASCII فقط
                // (EN) Fallback: ASCII-only conversion
                std::string result = str;
                std::transform(result.begin(), result.end(), result.begin(),
                               [](unsigned char c)
                               { return std::tolower(c); });
                return Data::Value(result);
            }

            /**
             * @brief (AR) تحويل_كبير - تحويل إلى أحرف كبيرة
             * @brief (EN) ToUpper - Convert to uppercase
             */
            Data::Value StringFunctions::toUpper(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 1, 1);

                std::string str = args[0].toString();
#ifdef _WIN32
                // (AR) تحويل Unicode باستخدام Windows API
                // (EN) Unicode conversion using Windows API
                int wlen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
                if (wlen > 1)
                {
                    std::wstring wstr(wlen - 1, 0);
                    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], wlen);
                    CharUpperW(&wstr[0]);
                    int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
                    std::string result(len - 1, 0);
                    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], len, nullptr, nullptr);
                    return Data::Value(result);
                }
#endif
                // (AR) احتياطي: تحويل ASCII فقط
                // (EN) Fallback: ASCII-only conversion
                std::string result = str;
                std::transform(result.begin(), result.end(), result.begin(),
                               [](unsigned char c)
                               { return std::toupper(c); });
                return Data::Value(result);
            }

            /**
             * @brief (AR) قص_أطراف - إزالة المسافات
             * @brief (EN) Trim - Remove whitespace
             */
            Data::Value StringFunctions::trim(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 1, 1);

                std::string str = args[0].toString();

                // (AR) إزالة المسافات من البداية
                // (EN) Remove leading whitespace
                size_t start = str.find_first_not_of(" \t\n\r");
                if (start == std::string::npos)
                {
                    return Data::Value(""); // All whitespace
                }

                // (AR) إزالة المسافات من النهاية
                // (EN) Remove trailing whitespace
                size_t end = str.find_last_not_of(" \t\n\r");

                return Data::Value(str.substr(start, end - start + 1));
            }

            /**
             * @brief (AR) تقسيم - تقسيم النص إلى مصفوفة
             * @brief (EN) Split - Split string into array
             */
            Data::Value StringFunctions::split(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 2, 3);

                std::string str = args[0].toString();
                std::string delimiter = args[1].toString();
                int maxSplits = -1;

                if (args.size() == 3)
                {
                    maxSplits = args[2].toInt();
                }

                std::vector<Data::Value> parts;

                // (AR) إذا كان maxSplits == 0 ارجع النص كاملاً في مصفوفة واحدة
                // (EN) If maxSplits == 0, return the whole string as a single-element array
                if (maxSplits == 0)
                {
                    parts.push_back(Data::Value(str));
                    return Data::Value(parts);
                }

                if (delimiter.empty())
                {
                    // (AR) إذا كان الفاصل فارغاً، نفصل كل حرف
                    // (EN) If delimiter is empty, split by character
                    for (size_t i = 0; i < utf8Length(str); i++)
                    {
                        parts.push_back(Data::Value(utf8Substring(str, i, 1)));
                    }
                }
                else
                {
                    size_t pos = 0;
                    size_t found;
                    int splits = 0;

                    while ((found = str.find(delimiter, pos)) != std::string::npos)
                    {
                        parts.push_back(Data::Value(str.substr(pos, found - pos)));
                        pos = found + delimiter.length();
                        splits++;

                        if (maxSplits > 0 && splits >= maxSplits)
                        {
                            break;
                        }
                    }

                    // (AR) إضافة الجزء الأخير
                    // (EN) Add last part
                    parts.push_back(Data::Value(str.substr(pos)));
                }

                return Data::Value(parts);
            }

            /**
             * @brief (AR) دمج - دمج عناصر مصفوفة
             * @brief (EN) Join - Join array elements
             */
            Data::Value StringFunctions::join(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 1, 2);

                if (args[0].getKind() != Types::SadTypeKind::Array)
                {
                    throw std::invalid_argument("First argument must be an array");
                }

                std::vector<Data::Value> arr = args[0].toArray();
                std::string separator = "";

                if (args.size() == 2)
                {
                    separator = args[1].toString();
                }

                std::string result;
                for (size_t i = 0; i < arr.size(); i++)
                {
                    if (i > 0)
                    {
                        result += separator;
                    }
                    result += arr[i].toString();
                }

                return Data::Value(result);
            }

            /**
             * @brief (AR) يبدأ_ب - التحقق من البداية
             * @brief (EN) StartsWith - Check prefix
             */
            Data::Value StringFunctions::startsWith(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 2, 2);

                std::string str = args[0].toString();
                std::string prefix = args[1].toString();

                if (prefix.length() > str.length())
                {
                    return Data::Value(false);
                }

                bool result = (str.substr(0, prefix.length()) == prefix);
                return Data::Value(result);
            }

            /**
             * @brief (AR) ينتهي_ب - التحقق من النهاية
             * @brief (EN) EndsWith - Check suffix
             */
            Data::Value StringFunctions::endsWith(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 2, 2);

                std::string str = args[0].toString();
                std::string suffix = args[1].toString();

                if (suffix.length() > str.length())
                {
                    return Data::Value(false);
                }

                bool result = (str.substr(str.length() - suffix.length()) == suffix);
                return Data::Value(result);
            }

            /**
             * @brief (AR) يحتوي_على - التحقق من الاحتواء
             * @brief (EN) Contains - Check if contains substring
             */
            Data::Value StringFunctions::contains(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 2, 2);

                std::string str = args[0].toString();
                std::string sub = args[1].toString();

                bool result = (str.find(sub) != std::string::npos);
                return Data::Value(result);
            }

        } // namespace String
    } // namespace StdLib
} // namespace Sad
