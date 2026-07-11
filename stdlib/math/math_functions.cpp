/**
 * @file math_functions.cpp
 * @brief (AR) تنفيذ الدوال الرياضية / (EN) Implementation of Mathematical Functions
 *
 * This file implements all 12 mathematical functions for Sad Language.
 * Uses C++ <cmath> library for accurate calculations.
 *
 * @author S Language Development Team
 * @date November 22, 2025
 * @version 1.0
 */

#include "math/math_functions.h"
#include "sad_type_system.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <limits>
#include <climits>
#include <cstdint> // (AR) ISSUE-063: حساب مربع() بـint64_t / (EN) ISSUE-063: int64_t square()
#include <random>

namespace Sad
{
    namespace StdLib
    {
        namespace Math
        {

            // ============================================================================
            // (AR) الدوال المساعدة الخاصة / (EN) Private Helper Functions
            // ============================================================================

            /**
             * @brief (AR) التحقق من صحة المعاملات
             * @brief (EN) Validate function arguments
             */
            bool MathFunctions::validateArguments(const std::vector<Data::Value> &args,
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

            /**
             * @brief (AR) تحويل القيمة إلى رقم حقيقي
             * @brief (EN) Convert value to double
             */
            double MathFunctions::toDouble(const Data::Value &value)
            {
                if (value.getKind() == Types::SadTypeKind::Integer)
                {
                    return static_cast<double>(value.toInt());
                }
                if (value.getKind() == Types::SadTypeKind::Float)
                {
                    return value.toDouble();
                }
                throw std::invalid_argument(
                    "(AR) يجب أن يكون المعامل رقماً / "
                    "(EN) Argument must be a number");
            }

            // ============================================================================
            // (AR) دوال رياضية أساسية / (EN) Basic Math Functions
            // ============================================================================

            /**
             * @brief (AR) جذر - حساب الجذر التربيعي
             * @brief (EN) Sqrt - Calculate square root
             */
            Data::Value MathFunctions::sqrt(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 1, 1);

                double num = toDouble(args[0]);

                if (num < 0)
                {
                    throw std::invalid_argument(
                        "(AR) لا يمكن حساب جذر عدد سالب / "
                        "(EN) Cannot calculate square root of negative number");
                }

                double result = std::sqrt(num);
                return Data::Value(result);
            }

            /**
             * @brief (AR) أس - رفع عدد لأس معين
             * @brief (EN) Power - Raise number to power
             */
            Data::Value MathFunctions::power(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 2, 2);

                double base = toDouble(args[0]);
                double exponent = toDouble(args[1]);

                double result = std::pow(base, exponent);

                // (AR) التحقق من النتيجة
                // (EN) Check result validity
                if (std::isnan(result) || std::isinf(result))
                {
                    throw std::runtime_error(
                        "(AR) نتيجة غير صالحة / "
                        "(EN) Invalid result");
                }

                return Data::Value(result);
            }

            /**
             * @brief (AR) مطلق - القيمة المطلقة
             * @brief (EN) Abs - Absolute value
             */
            Data::Value MathFunctions::abs(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 1, 1);

                if (args[0].getKind() == Types::SadTypeKind::Integer)
                {
                    int num = args[0].toInt();
                    return Data::Value(std::abs(num));
                }

                if (args[0].getKind() == Types::SadTypeKind::Float)
                {
                    double num = args[0].toDouble();
                    return Data::Value(std::fabs(num));
                }

                throw std::invalid_argument(
                    "(AR) المعامل يجب أن يكون رقماً / "
                    "(EN) Argument must be a number");
            }

            /**
             * @brief (AR) أكبر - إيجاد أكبر قيمة
             * @brief (EN) Max - Find maximum value
             */
            Data::Value MathFunctions::max(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 1, -1);

                double maxValue = toDouble(args[0]);
                bool allIntegers = (args[0].getKind() == Types::SadTypeKind::Integer);

                for (size_t i = 1; i < args.size(); i++)
                {
                    double current = toDouble(args[i]);
                    if (current > maxValue)
                    {
                        maxValue = current;
                    }
                    if (args[i].getKind() != Types::SadTypeKind::Integer)
                    {
                        allIntegers = false;
                    }
                }

                // (AR) إرجاع رقم صحيح إذا كانت كل المدخلات أرقام صحيحة
                // (EN) Return integer if all inputs were integers
                if (allIntegers)
                {
                    return Data::Value(static_cast<int>(maxValue));
                }

                return Data::Value(maxValue);
            }

            /**
             * @brief (AR) أصغر - إيجاد أصغر قيمة
             * @brief (EN) Min - Find minimum value
             */
            Data::Value MathFunctions::min(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 1, -1);

                double minValue = toDouble(args[0]);
                bool allIntegers = (args[0].getKind() == Types::SadTypeKind::Integer);

                for (size_t i = 1; i < args.size(); i++)
                {
                    double current = toDouble(args[i]);
                    if (current < minValue)
                    {
                        minValue = current;
                    }
                    if (args[i].getKind() != Types::SadTypeKind::Integer)
                    {
                        allIntegers = false;
                    }
                }

                // (AR) إرجاع رقم صحيح إذا كانت كل المدخلات أرقام صحيحة
                // (EN) Return integer if all inputs were integers
                if (allIntegers)
                {
                    return Data::Value(static_cast<int>(minValue));
                }

                return Data::Value(minValue);
            }

            /**
             * @brief (AR) تقريب - تقريب لأقرب عدد صحيح
             * @brief (EN) Round - Round to nearest integer
             */
            Data::Value MathFunctions::round(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 1, 1);

                double num = toDouble(args[0]);
                double rounded = std::round(num);
                // (AR) إرجاع كعدد صحيح إذا كان ضمن النطاق، وإلا كعشري
                // (EN) Return as int if within range, otherwise as double
                if (rounded >= INT_MIN && rounded <= INT_MAX)
                {
                    return Data::Value(static_cast<int>(rounded));
                }
                return Data::Value(rounded);
            }

            /**
             * @brief (AR) أرضية - التقريب للأسفل
             * @brief (EN) Floor - Round down
             */
            Data::Value MathFunctions::floor(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 1, 1);

                double num = toDouble(args[0]);
                double floored = std::floor(num);
                if (floored >= INT_MIN && floored <= INT_MAX)
                {
                    return Data::Value(static_cast<int>(floored));
                }
                return Data::Value(floored);
            }

            /**
             * @brief (AR) سقف - التقريب للأعلى
             * @brief (EN) Ceil - Round up
             */
            Data::Value MathFunctions::ceil(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 1, 1);

                double num = toDouble(args[0]);
                double ceiled = std::ceil(num);
                if (ceiled >= INT_MIN && ceiled <= INT_MAX)
                {
                    return Data::Value(static_cast<int>(ceiled));
                }
                return Data::Value(ceiled);
            }

            /**
             * @brief (AR) مربع - حساب مربع العدد
             * @brief (EN) Square - Calculate square of number
             */
            Data::Value MathFunctions::square(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 1, 1);

                double num = toDouble(args[0]);
                double result = num * num;

                // (AR) ISSUE-063: صحيح×صحيح=صحيح ما دام يسَع 64-بت — كان الحساب int32
                //      فيرتدّ «مربع(50000)» إلى double ويطبع «2500000000.0» بينما الدلالة
                //      المقصودة (والمترجم) صحيح i64. نحسب بـint64 ونرتدّ إلى double فقط
                //      عند فيض i64 الحقيقيّ.
                // (EN) ISSUE-063: int×int=int as long as it fits 64-bit — the old int32
                //      arithmetic made square(50000) fall back to double ("2500000000.0")
                //      while the intended (and compiler) semantics is an i64 integer.
                //      Compute in int64 and only fall back to double on true i64 overflow.
                if (args[0].getKind() == Types::SadTypeKind::Integer)
                {
                    // (AR) أكبر قاعدة يسَع مربّعُها في i64 = floor(sqrt(INT64_MAX))
                    // (EN) Largest base whose square fits in i64 = floor(sqrt(INT64_MAX))
                    static constexpr int64_t kMaxInt64SquareBase = 3037000499LL;
                    const int64_t intNum = args[0].toInt64();
                    if (intNum >= -kMaxInt64SquareBase && intNum <= kMaxInt64SquareBase)
                    {
                        return Data::Value(intNum * intNum);
                    }
                }

                return Data::Value(result);
            }

            // ============================================================================
            // (AR) دوال مثلثية / (EN) Trigonometric Functions
            // ============================================================================

            /**
             * @brief (AR) جيب - حساب جيب الزاوية (بالراديان)
             * @brief (EN) Sin - Calculate sine (in radians)
             */
            Data::Value MathFunctions::sin(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 1, 1);

                double angle = toDouble(args[0]);
                double result = std::sin(angle);

                return Data::Value(result);
            }

            /**
             * @brief (AR) جيب_تمام - حساب جيب تمام الزاوية (بالراديان)
             * @brief (EN) Cos - Calculate cosine (in radians)
             */
            Data::Value MathFunctions::cos(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 1, 1);

                double angle = toDouble(args[0]);
                double result = std::cos(angle);

                return Data::Value(result);
            }

            /**
             * @brief (AR) ظل - حساب ظل الزاوية (بالراديان)
             * @brief (EN) Tan - Calculate tangent (in radians)
             */
            Data::Value MathFunctions::tan(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 1, 1);

                double angle = toDouble(args[0]);
                double result = std::tan(angle);

                // (AR) التحقق من النتيجة
                // (EN) Check result validity
                if (std::isnan(result) || std::isinf(result))
                {
                    throw std::runtime_error(
                        "(AR) نتيجة غير صالحة (ربما الزاوية = 90 درجة) / "
                        "(EN) Invalid result (possibly angle = 90 degrees)");
                }

                return Data::Value(result);
            }

            // ============================================================================
            // (AR) دوال مثلثية عكسية / (EN) Inverse Trigonometric Functions
            // ============================================================================

            Data::Value MathFunctions::asin(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 1, 1);
                double num = toDouble(args[0]);
                if (num < -1.0 || num > 1.0)
                {
                    throw std::runtime_error(
                        "(AR) قيمة asin يجب أن تكون بين -1 و 1 / "
                        "(EN) asin argument must be between -1 and 1");
                }
                return Data::Value(std::asin(num));
            }

            Data::Value MathFunctions::acos(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 1, 1);
                double num = toDouble(args[0]);
                if (num < -1.0 || num > 1.0)
                {
                    throw std::runtime_error(
                        "(AR) قيمة acos يجب أن تكون بين -1 و 1 / "
                        "(EN) acos argument must be between -1 and 1");
                }
                return Data::Value(std::acos(num));
            }

            Data::Value MathFunctions::atan(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 1, 1);
                return Data::Value(std::atan(toDouble(args[0])));
            }

            Data::Value MathFunctions::atan2(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 2, 2);
                return Data::Value(std::atan2(toDouble(args[0]), toDouble(args[1])));
            }

            // ============================================================================
            // (AR) دوال لوغاريتمية وأسية / (EN) Logarithmic & Exponential Functions
            // ============================================================================

            Data::Value MathFunctions::log(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 1, 1);
                double num = toDouble(args[0]);
                if (num <= 0)
                {
                    throw std::runtime_error(
                        "(AR) لا يمكن حساب اللوغاريتم لعدد غير موجب / "
                        "(EN) Cannot compute logarithm of non-positive number");
                }
                return Data::Value(std::log(num));
            }

            Data::Value MathFunctions::log2(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 1, 1);
                double num = toDouble(args[0]);
                if (num <= 0)
                {
                    throw std::runtime_error(
                        "(AR) لا يمكن حساب log2 لعدد غير موجب / "
                        "(EN) Cannot compute log2 of non-positive number");
                }
                return Data::Value(std::log2(num));
            }

            Data::Value MathFunctions::log10(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 1, 1);
                double num = toDouble(args[0]);
                if (num <= 0)
                {
                    throw std::runtime_error(
                        "(AR) لا يمكن حساب log10 لعدد غير موجب / "
                        "(EN) Cannot compute log10 of non-positive number");
                }
                return Data::Value(std::log10(num));
            }

            Data::Value MathFunctions::exp(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 1, 1);
                double result = std::exp(toDouble(args[0]));
                if (std::isinf(result))
                {
                    throw std::runtime_error(
                        "(AR) النتيجة كبيرة جداً (overflow) / "
                        "(EN) Result too large (overflow)");
                }
                return Data::Value(result);
            }

            // ============================================================================
            // (AR) ثوابت رياضية / (EN) Math Constants
            // ============================================================================

            Data::Value MathFunctions::pi(const std::vector<Data::Value> &args)
            {
                return Data::Value(3.14159265358979323846);
            }

            Data::Value MathFunctions::e(const std::vector<Data::Value> &args)
            {
                return Data::Value(2.71828182845904523536);
            }

            // ============================================================================
            // (AR) دوال عشوائية / (EN) Random Functions
            // ============================================================================

            Data::Value MathFunctions::random(const std::vector<Data::Value> &args)
            {
                // (AR) إرجاع رقم عشوائي بين 0.0 و 1.0
                static std::mt19937 gen(std::random_device{}());
                static std::uniform_real_distribution<double> dist(0.0, 1.0);
                return Data::Value(dist(gen));
            }

            Data::Value MathFunctions::randomInt(const std::vector<Data::Value> &args)
            {
                validateArguments(args, 2, 2);
                int minVal = static_cast<int>(toDouble(args[0]));
                int maxVal = static_cast<int>(toDouble(args[1]));
                if (minVal > maxVal)
                    std::swap(minVal, maxVal);
                static std::mt19937 gen(std::random_device{}());
                std::uniform_int_distribution<int> dist(minVal, maxVal);
                return Data::Value(dist(gen));
            }

        } // namespace Math
    } // namespace StdLib
} // namespace Sad
