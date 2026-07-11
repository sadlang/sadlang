/**
 * @file array_functions.cpp
 * @brief (AR) تنفيذ دوال معالجة المصفوفات / (EN) Implementation of Array Manipulation Functions
 *
 * This file implements all 10 array processing functions for Sad Language.
 * All functions support mixed-type arrays and proper error handling.
 *
 * @author S Language Development Team
 * @date November 22, 2025
 * @version 1.0
 */

#include "array_functions.h"
#include "sad_type_system.h"
#include <algorithm>
#include <stdexcept>
#include "builtin_error.h" // (AR) EM-CPP: حامل خطأ الكتالوج

namespace Sad
{
    namespace StdLib
    {
        namespace Core
        {

            // ============================================================================
            // (AR) الدوال المساعدة الخاصة / (EN) Private Helper Functions
            // ============================================================================

            /**
             * @brief (AR) التحقق من صحة المعاملات
             * @brief (EN) Validate function arguments
             */
            bool ArrayFunctions::validateArguments(const std::vector<Data::Value> &args,
                                                   size_t minArgs,
                                                   int maxArgs)
            {
                if (args.size() < minArgs)
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }
                if (maxArgs >= 0 && args.size() > static_cast<size_t>(maxArgs))
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }
                return true;
            }

            /**
             * @brief (AR) مقارنة قيمتين للفرز
             * @brief (EN) Compare two values for sorting
             */
            bool ArrayFunctions::compareValues(const Data::Value &a, const Data::Value &b)
            {
                // (AR) إذا كانا من نفس النوع، قارن مباشرة
                // (EN) If same type, compare directly
                if (a.getKind() == b.getKind())
                {
                    if (a.getKind() == Types::SadTypeKind::Integer)
                    {
                        // (AR) مقارنة 64-بت — كان toInt() يقتطع فيُفسد ترتيب القيم الكبيرة
                        // (EN) 64-bit comparison — toInt() truncated and misordered large values
                        return a.toInt64() < b.toInt64();
                    }
                    if (a.getKind() == Types::SadTypeKind::Float)
                    {
                        return a.toDouble() < b.toDouble();
                    }
                    if (a.getKind() == Types::SadTypeKind::String)
                    {
                        return a.toString() < b.toString();
                    }
                }

                // (AR) أنواع مختلفة: قارن كنصوص
                // (EN) Different types: compare as strings
                return a.toString() < b.toString();
            }

            // ============================================================================
            // (AR) تنفيذ الدوال العامة / (EN) Public Function Implementations
            // ============================================================================

            /**
             * @brief (AR) إضافة - إضافة عنصر للمصفوفة
             * @brief (EN) Append - Add element to array
             */
            Data::Value ArrayFunctions::append(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 2, 2);

                if (args[0].getKind() != Types::SadTypeKind::Array)
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                }

                // (AR) نسخ المصفوفة وإضافة العنصر
                // (EN) Copy array and add element
                std::vector<Data::Value> arr = args[0].toArray();
                arr.push_back(args[1]);

                return Data::Value(arr);
            }

            /**
             * @brief (AR) إزالة - إزالة آخر عنصر
             * @brief (EN) Pop - Remove last element
             */
            Data::Value ArrayFunctions::pop(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 1, 1);

                if (args[0].getKind() != Types::SadTypeKind::Array)
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                }

                std::vector<Data::Value> arr = args[0].toArray();

                if (arr.empty())
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_EMPTY_OPERATION);
                }

                // (AR) إرجاع آخر عنصر
                // (EN) Return last element
                Data::Value lastElement = arr.back();

                return lastElement;
            }

            /**
             * @brief (AR) حجم - الحصول على حجم المصفوفة
             * @brief (EN) Size - Get array size
             */
            Data::Value ArrayFunctions::size(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 1, 1);

                if (args[0].getKind() != Types::SadTypeKind::Array)
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                }

                std::vector<Data::Value> arr = args[0].toArray();
                int arraySize = static_cast<int>(arr.size());

                return Data::Value(arraySize);
            }

            /**
             * @brief (AR) فهرس - إيجاد موقع عنصر
             * @brief (EN) IndexOf - Find element index
             */
            Data::Value ArrayFunctions::indexOf(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 2, 3);

                if (args[0].getKind() != Types::SadTypeKind::Array)
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                }

                std::vector<Data::Value> arr = args[0].toArray();
                Data::Value searchValue = args[1];
                size_t startPos = 0;

                if (args.size() == 3)
                {
                    startPos = static_cast<size_t>(args[2].toInt());
                    if (startPos >= arr.size())
                    {
                        return Data::Value(-1);
                    }
                }

                // (AR) البحث عن العنصر
                // (EN) Search for element
                for (size_t i = startPos; i < arr.size(); i++)
                {
                    if ((arr[i] == searchValue).toBool())
                    {
                        return Data::Value(static_cast<int>(i));
                    }
                }

                return Data::Value(-1);
            }

            /**
             * @brief (AR) يحتوي - التحقق من وجود عنصر
             * @brief (EN) Contains - Check if contains element
             */
            Data::Value ArrayFunctions::contains(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 2, 2);

                if (args[0].getKind() != Types::SadTypeKind::Array)
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                }

                std::vector<Data::Value> arr = args[0].toArray();
                Data::Value searchValue = args[1];

                // (AR) البحث عن العنصر
                // (EN) Search for element
                for (const auto &elem : arr)
                {
                    if ((elem == searchValue).toBool())
                    {
                        return Data::Value(true);
                    }
                }

                return Data::Value(false);
            }

            /**
             * @brief (AR) قلب - قلب ترتيب العناصر
             * @brief (EN) Reverse - Reverse array order
             */
            Data::Value ArrayFunctions::reverse(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 1, 1);

                if (args[0].getKind() != Types::SadTypeKind::Array)
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                }

                std::vector<Data::Value> arr = args[0].toArray();

                // (AR) قلب المصفوفة
                // (EN) Reverse array
                std::reverse(arr.begin(), arr.end());

                return Data::Value(arr);
            }

            /**
             * @brief (AR) فرز - فرز العناصر
             * @brief (EN) Sort - Sort elements
             */
            Data::Value ArrayFunctions::sort(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 1, 2);

                if (args[0].getKind() != Types::SadTypeKind::Array)
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                }

                std::vector<Data::Value> arr = args[0].toArray();
                bool ascending = true;

                if (args.size() == 2)
                {
                    ascending = args[1].toBool();
                }

                // (AR) فرز المصفوفة
                // (EN) Sort array
                if (ascending)
                {
                    std::sort(arr.begin(), arr.end(), compareValues);
                }
                else
                {
                    std::sort(arr.begin(), arr.end(),
                              [](const Data::Value &a, const Data::Value &b)
                              {
                                  return compareValues(b, a);
                              });
                }

                return Data::Value(arr);
            }

            /**
             * @brief (AR) أول - الحصول على أول عنصر
             * @brief (EN) First - Get first element
             */
            Data::Value ArrayFunctions::first(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 1, 1);

                if (args[0].getKind() != Types::SadTypeKind::Array)
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                }

                std::vector<Data::Value> arr = args[0].toArray();

                if (arr.empty())
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_EMPTY_OPERATION);
                }

                return arr[0];
            }

            /**
             * @brief (AR) آخر - الحصول على آخر عنصر
             * @brief (EN) Last - Get last element
             */
            Data::Value ArrayFunctions::last(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 1, 1);

                if (args[0].getKind() != Types::SadTypeKind::Array)
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                }

                std::vector<Data::Value> arr = args[0].toArray();

                if (arr.empty())
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_EMPTY_OPERATION);
                }

                return arr.back();
            }

            /**
             * @brief (AR) شريحة - استخراج جزء من المصفوفة
             * @brief (EN) Slice - Extract subarray
             */
            Data::Value ArrayFunctions::slice(const std::vector<Data::Value> &args)
            {
                // (AR) التحقق من المعاملات
                // (EN) Validate arguments
                validateArguments(args, 2, 3);

                if (args[0].getKind() != Types::SadTypeKind::Array)
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                }

                std::vector<Data::Value> arr = args[0].toArray();
                int start = args[1].toInt();
                int end = static_cast<int>(arr.size());

                if (args.size() == 3)
                {
                    end = args[2].toInt();
                }

                // (AR) التحقق من الحدود
                // (EN) Check bounds
                if (start < 0)
                    start = 0;
                if (end > static_cast<int>(arr.size()))
                    end = static_cast<int>(arr.size());
                if (start >= end)
                {
                    return Data::Value(std::vector<Data::Value>());
                }

                // (AR) استخراج الجزء
                // (EN) Extract slice
                std::vector<Data::Value> result(
                    arr.begin() + start,
                    arr.begin() + end);

                return Data::Value(result);
            }

        } // namespace Core
    } // namespace StdLib
} // namespace Sad
