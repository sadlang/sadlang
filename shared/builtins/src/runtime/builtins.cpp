/**
 * @file builtins.cpp
 * @brief (AR) تنفيذ الدوال المضمنة / (EN) Implementation of Built-in Functions
 *
 * This file provides adapters that connect the built-in functions interface
 * with the actual implementations in various stdlib modules.
 *
 * @author S Language Development Team
 * @date November 22, 2025
 */

#include <string>
#include "builtins.h"
#include "io/io_functions.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include "sad_type_system.h"

#ifdef _WIN32
#include <windows.h>
#include "builtin_error.h" // (AR) EM-CPP: حامل خطأ الكتالوج
// Undefine Windows VOID macro to avoid conflict with ValueType::VOID
#ifdef VOID
#undef VOID
#endif
#endif

namespace Sad
{
    namespace StdLib
    {
        namespace BuiltinFunctions
        {

            using namespace IO;

            // =========================================================================
            // (AR) دوال الإدخال والإخراج / (EN) I/O Functions
            // =========================================================================

            std::shared_ptr<Data::Value> print(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                // Convert from ValuePtr to Value
                std::vector<Data::Value> convertedArgs;
                for (const auto &arg : args)
                {
                    if (arg)
                        convertedArgs.push_back(*arg);
                }

                // Call the actual implementation
                Data::Value result = IOFunctions::print(convertedArgs);

                // Return as shared_ptr
                return std::make_shared<Data::Value>(result);
            }

            std::shared_ptr<Data::Value> println(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                std::vector<Data::Value> convertedArgs;
                for (const auto &arg : args)
                {
                    if (arg)
                        convertedArgs.push_back(*arg);
                }

                Data::Value result = IOFunctions::println(convertedArgs);
                return std::make_shared<Data::Value>(result);
            }

            std::shared_ptr<Data::Value> input(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                std::vector<Data::Value> convertedArgs;
                for (const auto &arg : args)
                {
                    if (arg)
                        convertedArgs.push_back(*arg);
                }

                Data::Value result = IOFunctions::input(convertedArgs);
                return std::make_shared<Data::Value>(result);
            }

            std::shared_ptr<Data::Value> readLine(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                std::vector<Data::Value> convertedArgs;
                for (const auto &arg : args)
                {
                    if (arg)
                        convertedArgs.push_back(*arg);
                }

                Data::Value result = IOFunctions::readLine(convertedArgs);
                return std::make_shared<Data::Value>(result);
            }

            std::shared_ptr<Data::Value> clear(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                std::vector<Data::Value> convertedArgs;
                for (const auto &arg : args)
                {
                    if (arg)
                        convertedArgs.push_back(*arg);
                }

                Data::Value result = IOFunctions::clear(convertedArgs);
                return std::make_shared<Data::Value>(result);
            }

            // =========================================================================
            // (AR) دوال المصفوفات والقوائم / (EN) Array/List Functions
            // =========================================================================

            std::shared_ptr<Data::Value> length(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                if (args.empty())
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }

                auto &value = args[0];
                if (!value)
                {
                    return std::make_shared<Data::Value>(0);
                }

                // Use size() method which works for array, string, and map
                return std::make_shared<Data::Value>((int)value->size());
            }

            std::shared_ptr<Data::Value> append(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                if (args.size() < 2)
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }

                auto &array = args[0];
                auto &value = args[1];

                if (!array || !array->isArray())
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                }

                if (value)
                {
                    array->append(*value);
                }
                else
                {
                    array->append(Data::Value());
                }
                return std::make_shared<Data::Value>(); // VOID
            }

            std::shared_ptr<Data::Value> remove(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                if (args.size() < 2)
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }

                auto &array = args[0];
                auto &index = args[1];

                if (!array || !array->isArray())
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                }

                if (!index || !index->isInteger())
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                }

                int idx = index->toInt();

                if (idx >= 0 && idx < (int)array->size())
                {
                    // (AR) إزالة العنصر بالفهرس من المصفوفة مباشرةً
                    // (EN) Remove element at index from array directly
                    array->remove(static_cast<size_t>(idx));
                }
                else
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_INDEX_OUT_OF_RANGE);
                }

                return std::make_shared<Data::Value>(); // VOID
            }

            // =========================================================================
            // (AR) دوال النصوص / (EN) String Functions
            // =========================================================================

            std::shared_ptr<Data::Value> str_len(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                if (args.empty() || !args[0])
                {
                    return std::make_shared<Data::Value>(0);
                }

                // (AR) حساب طول النص بوحدات أحرف يونيكود (وليس بايتات)
                // (EN) Count Unicode characters (not bytes) for correct Arabic string length
                const std::string &str = args[0]->toString();
                int charCount = 0;
                const unsigned char *bytes = reinterpret_cast<const unsigned char *>(str.data());
                size_t len = str.size();
                for (size_t i = 0; i < len;)
                {
                    unsigned char b = bytes[i];
                    if (b < 0x80)
                    {
                        i += 1;
                    }
                    else if ((b & 0xE0) == 0xC0)
                    {
                        i += 2;
                    }
                    else if ((b & 0xF0) == 0xE0)
                    {
                        i += 3;
                    }
                    else if ((b & 0xF8) == 0xF0)
                    {
                        i += 4;
                    }
                    else
                    {
                        i += 1;
                    } // (AR) بايت غير صالح — تخطي
                    charCount++;
                }
                return std::make_shared<Data::Value>(charCount);
            }

            std::shared_ptr<Data::Value> upper(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                if (args.empty() || !args[0])
                {
                    return std::make_shared<Data::Value>(std::string(""));
                }

                std::string str = args[0]->toString();
#ifdef _WIN32
                // (AR) استخدام Windows API لتحويل يونيكود صحيح (يشمل العربية واللاتينية)
                // (EN) Use Windows API for correct Unicode case conversion
                int wlen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
                if (wlen > 1)
                {
                    std::wstring wstr(wlen - 1, 0);
                    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], wlen);
                    CharUpperW(&wstr[0]);
                    int ulen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
                    std::string result(ulen - 1, 0);
                    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], ulen, nullptr, nullptr);
                    return std::make_shared<Data::Value>(result);
                }
#endif
                // (AR) احتياطي: ASCII فقط إذا لم يكن ويندوز
                std::transform(str.begin(), str.end(), str.begin(), ::toupper);
                return std::make_shared<Data::Value>(str);
            }

            std::shared_ptr<Data::Value> lower(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                if (args.empty() || !args[0])
                {
                    return std::make_shared<Data::Value>(std::string(""));
                }

                std::string str = args[0]->toString();
#ifdef _WIN32
                // (AR) استخدام Windows API لتحويل يونيكود صحيح
                int wlen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
                if (wlen > 1)
                {
                    std::wstring wstr(wlen - 1, 0);
                    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], wlen);
                    CharLowerW(&wstr[0]);
                    int ulen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
                    std::string result(ulen - 1, 0);
                    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], ulen, nullptr, nullptr);
                    return std::make_shared<Data::Value>(result);
                }
#endif
                std::transform(str.begin(), str.end(), str.begin(), ::tolower);
                return std::make_shared<Data::Value>(str);
            }

            /**
             * @brief (AR) تقسيم نص إلى مصفوفة من النصوص
             *        (EN) Split string into array of strings
             *
             * @param args (AR) المعاملات: [النص, الفاصل]
             *             (EN) Arguments: [text, delimiter]
             * @return (AR) مصفوفة من النصوص المقسّمة
             *         (EN) Array of split strings
             *
             * @example Examples / أمثلة:
             * split("مرحبا بكم", " ") -> ["مرحبا", "بكم"]
             * split("a,b,c", ",") -> ["a", "b", "c"]
             * split("hello", "") -> ["h", "e", "l", "l", "o"]
             */
            std::shared_ptr<Data::Value> split(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                // (AR) التحقق من عدد المعاملات
                // (EN) Validate argument count
                if (args.size() != 2)
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }

                // (AR) التحقق من أن المعامل الأول نص
                // (EN) Validate first argument is string
                if (!args[0]->isString())
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                }

                // (AR) التحقق من أن المعامل الثاني نص
                // (EN) Validate second argument is string
                if (!args[1]->isString())
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                }

                std::string text = args[0]->toString();
                std::string delimiter = args[1]->toString();
                std::vector<Data::Value> result;

                // (AR) حالة خاصة: فاصل فارغ - تقسيم إلى أحرف UTF-8
                // (EN) Special case: empty delimiter - split into UTF-8 characters
                if (delimiter.empty())
                {
                    size_t i = 0;
                    while (i < text.size())
                    {
                        unsigned char c = static_cast<unsigned char>(text[i]);
                        size_t charLen = 1;
                        if ((c & 0x80) == 0)
                            charLen = 1;
                        else if ((c & 0xE0) == 0xC0)
                            charLen = 2;
                        else if ((c & 0xF0) == 0xE0)
                            charLen = 3;
                        else if ((c & 0xF8) == 0xF0)
                            charLen = 4;

                        if (i + charLen <= text.size())
                        {
                            result.push_back(Data::Value(text.substr(i, charLen)));
                        }
                        i += charLen;
                    }
                    return std::make_shared<Data::Value>(result);
                }

                // (AR) تقسيم النص باستخدام الفاصل
                // (EN) Split text using delimiter
                size_t start = 0;
                size_t end = text.find(delimiter);

                while (end != std::string::npos)
                {
                    result.push_back(Data::Value(text.substr(start, end - start)));
                    start = end + delimiter.length();
                    end = text.find(delimiter, start);
                }

                // (AR) إضافة الجزء الأخير
                // (EN) Add last part
                result.push_back(Data::Value(text.substr(start)));

                return std::make_shared<Data::Value>(result);
            }

            /**
             * @brief (AR) دمج مصفوفة إلى نص واحد
             *        (EN) Join array elements into a single string
             *
             * @param args (AR) المعاملات: [المصفوفة, الفاصل]
             *             (EN) Arguments: [array, delimiter]
             * @return (AR) نص مدموج
             *         (EN) Joined string
             *
             * @example Examples / أمثلة:
             * join(["مرحبا", "بكم"], " ") -> "مرحبا بكم"
             * join(["a", "b", "c"], ",") -> "a,b,c"
             * join([1, 2, 3], "-") -> "1-2-3"
             */
            std::shared_ptr<Data::Value> join(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                // (AR) التحقق من عدد المعاملات
                // (EN) Validate argument count
                if (args.size() != 2)
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }

                // (AR) التحقق من أن المعامل الأول مصفوفة
                // (EN) Validate first argument is array
                if (!args[0]->isArray())
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                }

                // (AR) التحقق من أن المعامل الثاني نص
                // (EN) Validate second argument is string
                if (!args[1]->isString())
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                }

                const auto &array = args[0]->toArray();
                std::string delimiter = args[1]->toString();
                std::string result;

                // (AR) دمج عناصر المصفوفة
                // (EN) Join array elements
                for (size_t i = 0; i < array.size(); ++i)
                {
                    result += array[i].toString();

                    // (AR) إضافة الفاصل بين العناصر (ليس بعد العنصر الأخير)
                    // (EN) Add delimiter between elements (not after last element)
                    if (i < array.size() - 1)
                    {
                        result += delimiter;
                    }
                }

                return std::make_shared<Data::Value>(result);
            }

            // =========================================================================
            // (AR) الدوال الرياضية / (EN) Math Functions
            // =========================================================================

            std::shared_ptr<Data::Value> abs(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                if (args.empty() || !args[0])
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }

                if (args[0]->isInteger())
                {
                    return std::make_shared<Data::Value>(std::abs(args[0]->toInt()));
                }

                if (args[0]->isDouble())
                {
                    return std::make_shared<Data::Value>(std::abs(args[0]->toDouble()));
                }

                throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
            }

            std::shared_ptr<Data::Value> max(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                if (args.size() < 2)
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }

                Data::Value result = *args[0];

                for (size_t i = 1; i < args.size(); ++i)
                {
                    if (args[i])
                    {
                        if ((args[i]->isInteger() || args[i]->isDouble()) &&
                            (result.isInteger() || result.isDouble()))
                        {
                            double currentVal = args[i]->isInteger() ? static_cast<double>(args[i]->toInt()) : args[i]->toDouble();
                            double resultVal = result.isInteger() ? static_cast<double>(result.toInt()) : result.toDouble();
                            if (currentVal > resultVal)
                            {
                                result = *args[i];
                            }
                        }
                    }
                }

                return std::make_shared<Data::Value>(result);
            }

            std::shared_ptr<Data::Value> min(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                if (args.size() < 2)
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }

                Data::Value result = *args[0];

                for (size_t i = 1; i < args.size(); ++i)
                {
                    if (args[i])
                    {
                        if ((args[i]->isInteger() || args[i]->isDouble()) &&
                            (result.isInteger() || result.isDouble()))
                        {
                            double currentVal = args[i]->isInteger() ? static_cast<double>(args[i]->toInt()) : args[i]->toDouble();
                            double resultVal = result.isInteger() ? static_cast<double>(result.toInt()) : result.toDouble();
                            if (currentVal < resultVal)
                            {
                                result = *args[i];
                            }
                        }
                    }
                }

                return std::make_shared<Data::Value>(result);
            }

            std::shared_ptr<Data::Value> sum(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                double total = 0.0;
                bool hasDouble = false;

                for (const auto &arg : args)
                {
                    if (arg)
                    {
                        if (arg->isInteger())
                        {
                            total += arg->toInt();
                        }
                        else if (arg->isDouble())
                        {
                            total += arg->toDouble();
                            hasDouble = true;
                        }
                    }
                }

                if (hasDouble)
                {
                    return std::make_shared<Data::Value>(total);
                }
                return std::make_shared<Data::Value>(static_cast<int>(total));
            }

            std::shared_ptr<Data::Value> sqrt(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                if (args.empty() || !args[0])
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }

                double value = 0;
                if (args[0]->isInteger())
                {
                    value = args[0]->toInt();
                }
                else if (args[0]->isDouble())
                {
                    value = args[0]->toDouble();
                }

                return std::make_shared<Data::Value>(std::sqrt(value));
            }

            // =========================================================================
            // (AR) دوال النوع والتحويل / (EN) Type & Conversion Functions
            // =========================================================================

            std::shared_ptr<Data::Value> type_of(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                if (args.empty() || !args[0])
                {
                    return std::make_shared<Data::Value>(std::string("\xd9\x81\xd8\xb1\xd8\xa7\xd8\xba")); // فراغ
                }

                switch (args[0]->getKind())
                {
                case Types::SadTypeKind::Integer:
                    return std::make_shared<Data::Value>(std::string("\xd8\xb1\xd9\x82\xd9\x85")); // رقم
                case Types::SadTypeKind::Float:
                    return std::make_shared<Data::Value>(std::string("\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a")); // عشري
                case Types::SadTypeKind::String:
                    return std::make_shared<Data::Value>(std::string("\xd9\x86\xd8\xb5")); // نص
                case Types::SadTypeKind::Boolean:
                    return std::make_shared<Data::Value>(std::string("\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a")); // منطقي
                case Types::SadTypeKind::Array:
                    return std::make_shared<Data::Value>(std::string("\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9")); // مصفوفة
                case Types::SadTypeKind::Map:
                    return std::make_shared<Data::Value>(std::string("\xd8\xae\xd8\xb1\xd9\x8a\xd8\xb7\xd8\xa9")); // خريطة
                case Types::SadTypeKind::Class:
                    return std::make_shared<Data::Value>(std::string("\xd9\x83\xd8\xa7\xd8\xa6\xd9\x86")); // كائن
                case Types::SadTypeKind::Function:
                    return std::make_shared<Data::Value>(std::string("\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9")); // دالة
                case Types::SadTypeKind::Void:
                    return std::make_shared<Data::Value>(std::string("\xd9\x81\xd8\xb1\xd8\xa7\xd8\xba")); // فراغ
                case Types::SadTypeKind::Null:
                    return std::make_shared<Data::Value>(std::string("\xd8\xb9\xd8\xaf\xd9\x85")); // عدم — S-TS-P1
                default:
                    return std::make_shared<Data::Value>(std::string("\xd9\x85\xd8\xac\xd9\x87\xd9\x88\xd9\x84")); // مجهول
                }
            }

            std::shared_ptr<Data::Value> to_int(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                if (args.empty() || !args[0])
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }

                if (args[0]->isInteger())
                {
                    return std::make_shared<Data::Value>(args[0]->toInt());
                }

                if (args[0]->isDouble())
                {
                    return std::make_shared<Data::Value>((int)args[0]->toDouble());
                }

                if (args[0]->isBoolean())
                {
                    return std::make_shared<Data::Value>(args[0]->toBool() ? 1 : 0);
                }

                if (args[0]->isString())
                {
                    const std::string &s = args[0]->toString();
                    try
                    {
                        return std::make_shared<Data::Value>(std::stoi(s));
                    }
                    catch (...)
                    {
                        throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                    }
                }

                throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            }

            std::shared_ptr<Data::Value> to_float(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                if (args.empty() || !args[0])
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }

                if (args[0]->isDouble())
                {
                    return std::make_shared<Data::Value>(args[0]->toDouble());
                }

                if (args[0]->isInteger())
                {
                    return std::make_shared<Data::Value>((double)args[0]->toInt());
                }

                if (args[0]->isBoolean())
                {
                    return std::make_shared<Data::Value>(args[0]->toBool() ? 1.0 : 0.0);
                }

                if (args[0]->isString())
                {
                    const std::string &s = args[0]->toString();
                    try
                    {
                        return std::make_shared<Data::Value>(std::stod(s));
                    }
                    catch (...)
                    {
                        throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                    }
                }

                throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            }

            std::shared_ptr<Data::Value> to_string(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                if (args.empty() || !args[0])
                {
                    return std::make_shared<Data::Value>(std::string(""));
                }

                return std::make_shared<Data::Value>(args[0]->toString());
            }

            // =========================================================================
            // (AR) دوال المدى والتكرار / (EN) Range & Iteration Functions
            // =========================================================================

            /**
             * @brief (AR) إنشاء مصفوفة من الأرقام
             *        (EN) Generate array of numbers
             *
             * @param args (AR) المعاملات: [start, stop] أو [start, stop, step]
             *             (EN) Arguments: [start, stop] or [start, stop, step]
             *             (AR) أو [stop] فقط (يبدأ من 0)
             *             (EN) Or [stop] only (starts from 0)
             * @return (AR) مصفوفة من الأرقام
             *         (EN) Array of numbers
             *
             * @example Examples / أمثلة:
             * range(5) -> [0, 1, 2, 3, 4]
             * range(2, 5) -> [2, 3, 4]
             * range(0, 10, 2) -> [0, 2, 4, 6, 8]
             * range(5, 0, -1) -> [5, 4, 3, 2, 1]
             */
            std::shared_ptr<Data::Value> range(const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                // (AR) التحقق من عدد المعاملات
                // (EN) Validate argument count
                if (args.empty() || args.size() > 3)
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }

                int start = 0;
                int stop = 0;
                int step = 1;

                // (AR) تحليل المعاملات
                // (EN) Parse arguments
                if (args.size() == 1)
                {
                    // range(stop)
                    stop = args[0]->toInt();
                }
                else if (args.size() == 2)
                {
                    // range(start, stop)
                    start = args[0]->toInt();
                    stop = args[1]->toInt();
                }
                else
                {
                    // range(start, stop, step)
                    start = args[0]->toInt();
                    stop = args[1]->toInt();
                    step = args[2]->toInt();

                    // (AR) التحقق من أن step ليس صفراً
                    // (EN) Validate step is not zero
                    if (step == 0)
                    {
                        throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                    }
                }

                std::vector<Data::Value> result;

                // (AR) إنشاء المصفوفة
                // (EN) Generate array
                if (step > 0)
                {
                    // (AR) step موجب - من start إلى stop
                    // (EN) Positive step - from start to stop
                    for (int i = start; i < stop; i += step)
                    {
                        result.push_back(Data::Value(i));
                    }
                }
                else
                {
                    // (AR) step سالب - من start إلى stop بالعكس
                    // (EN) Negative step - from start to stop in reverse
                    for (int i = start; i > stop; i += step)
                    {
                        result.push_back(Data::Value(i));
                    }
                }

                return std::make_shared<Data::Value>(result);
            }

        } // namespace BuiltinFunctions
    } // namespace StdLib
} // namespace Sad
