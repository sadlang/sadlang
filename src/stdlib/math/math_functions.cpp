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

#include "stdlib/math/math_functions.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <limits>

namespace Sad {
namespace StdLib {
namespace Math {

// ============================================================================
// (AR) الدوال المساعدة الخاصة / (EN) Private Helper Functions
// ============================================================================

/**
 * @brief (AR) التحقق من صحة المعاملات
 * @brief (EN) Validate function arguments
 */
bool MathFunctions::validateArguments(const std::vector<Data::Value>& args,
                                     size_t minArgs, 
                                     int maxArgs) {
    if (args.size() < minArgs) {
        throw std::invalid_argument("Too few arguments");
    }
    if (maxArgs >= 0 && args.size() > static_cast<size_t>(maxArgs)) {
        throw std::invalid_argument("Too many arguments");
    }
    return true;
}

/**
 * @brief (AR) تحويل القيمة إلى رقم حقيقي
 * @brief (EN) Convert value to double
 */
double MathFunctions::toDouble(const Data::Value& value) {
    if (value.getType() == Data::ValueType::INTEGER) {
        return static_cast<double>(value.toInt());
    }
    if (value.getType() == Data::ValueType::DOUBLE) {
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
Data::Value MathFunctions::sqrt(const std::vector<Data::Value>& args) {
    validateArguments(args, 1, 1);
    
    double num = toDouble(args[0]);
    
    if (num < 0) {
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
Data::Value MathFunctions::power(const std::vector<Data::Value>& args) {
    validateArguments(args, 2, 2);
    
    double base = toDouble(args[0]);
    double exponent = toDouble(args[1]);
    
    double result = std::pow(base, exponent);
    
    // (AR) التحقق من النتيجة
    // (EN) Check result validity
    if (std::isnan(result) || std::isinf(result)) {
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
Data::Value MathFunctions::abs(const std::vector<Data::Value>& args) {
    validateArguments(args, 1, 1);
    
    if (args[0].getType() == Data::ValueType::INTEGER) {
        int num = args[0].toInt();
        return Data::Value(std::abs(num));
    }
    
    if (args[0].getType() == Data::ValueType::DOUBLE) {
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
Data::Value MathFunctions::max(const std::vector<Data::Value>& args) {
    validateArguments(args, 1, -1);
    
    double maxValue = toDouble(args[0]);
    bool allIntegers = (args[0].getType() == Data::ValueType::INTEGER);
    
    for (size_t i = 1; i < args.size(); i++) {
        double current = toDouble(args[i]);
        if (current > maxValue) {
            maxValue = current;
        }
        if (args[i].getType() != Data::ValueType::INTEGER) {
            allIntegers = false;
        }
    }
    
    // (AR) إرجاع رقم صحيح إذا كانت كل المدخلات أرقام صحيحة
    // (EN) Return integer if all inputs were integers
    if (allIntegers) {
        return Data::Value(static_cast<int>(maxValue));
    }
    
    return Data::Value(maxValue);
}

/**
 * @brief (AR) أصغر - إيجاد أصغر قيمة
 * @brief (EN) Min - Find minimum value
 */
Data::Value MathFunctions::min(const std::vector<Data::Value>& args) {
    validateArguments(args, 1, -1);
    
    double minValue = toDouble(args[0]);
    bool allIntegers = (args[0].getType() == Data::ValueType::INTEGER);
    
    for (size_t i = 1; i < args.size(); i++) {
        double current = toDouble(args[i]);
        if (current < minValue) {
            minValue = current;
        }
        if (args[i].getType() != Data::ValueType::INTEGER) {
            allIntegers = false;
        }
    }
    
    // (AR) إرجاع رقم صحيح إذا كانت كل المدخلات أرقام صحيحة
    // (EN) Return integer if all inputs were integers
    if (allIntegers) {
        return Data::Value(static_cast<int>(minValue));
    }
    
    return Data::Value(minValue);
}

/**
 * @brief (AR) تقريب - تقريب لأقرب عدد صحيح
 * @brief (EN) Round - Round to nearest integer
 */
Data::Value MathFunctions::round(const std::vector<Data::Value>& args) {
    validateArguments(args, 1, 1);
    
    double num = toDouble(args[0]);
    int result = static_cast<int>(std::round(num));
    
    return Data::Value(result);
}

/**
 * @brief (AR) أرضية - التقريب للأسفل
 * @brief (EN) Floor - Round down
 */
Data::Value MathFunctions::floor(const std::vector<Data::Value>& args) {
    validateArguments(args, 1, 1);
    
    double num = toDouble(args[0]);
    int result = static_cast<int>(std::floor(num));
    
    return Data::Value(result);
}

/**
 * @brief (AR) سقف - التقريب للأعلى
 * @brief (EN) Ceil - Round up
 */
Data::Value MathFunctions::ceil(const std::vector<Data::Value>& args) {
    validateArguments(args, 1, 1);
    
    double num = toDouble(args[0]);
    int result = static_cast<int>(std::ceil(num));
    
    return Data::Value(result);
}

/**
 * @brief (AR) مربع - حساب مربع العدد
 * @brief (EN) Square - Calculate square of number
 */
Data::Value MathFunctions::square(const std::vector<Data::Value>& args) {
    validateArguments(args, 1, 1);
    
    double num = toDouble(args[0]);
    double result = num * num;
    
    // (AR) إرجاع رقم صحيح إذا كان المدخل رقم صحيح والنتيجة تناسب int
    // (EN) Return integer if input was integer and result fits in int
    if (args[0].getType() == Data::ValueType::INTEGER) {
        int intNum = args[0].toInt();
        long long intResult = static_cast<long long>(intNum) * intNum;
        if (intResult <= std::numeric_limits<int>::max() && 
            intResult >= std::numeric_limits<int>::min()) {
            return Data::Value(static_cast<int>(intResult));
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
Data::Value MathFunctions::sin(const std::vector<Data::Value>& args) {
    validateArguments(args, 1, 1);
    
    double angle = toDouble(args[0]);
    double result = std::sin(angle);
    
    return Data::Value(result);
}

/**
 * @brief (AR) جيب_تمام - حساب جيب تمام الزاوية (بالراديان)
 * @brief (EN) Cos - Calculate cosine (in radians)
 */
Data::Value MathFunctions::cos(const std::vector<Data::Value>& args) {
    validateArguments(args, 1, 1);
    
    double angle = toDouble(args[0]);
    double result = std::cos(angle);
    
    return Data::Value(result);
}

/**
 * @brief (AR) ظل - حساب ظل الزاوية (بالراديان)
 * @brief (EN) Tan - Calculate tangent (in radians)
 */
Data::Value MathFunctions::tan(const std::vector<Data::Value>& args) {
    validateArguments(args, 1, 1);
    
    double angle = toDouble(args[0]);
    double result = std::tan(angle);
    
    // (AR) التحقق من النتيجة
    // (EN) Check result validity
    if (std::isnan(result) || std::isinf(result)) {
        throw std::runtime_error(
            "(AR) نتيجة غير صالحة (ربما الزاوية = 90 درجة) / "
            "(EN) Invalid result (possibly angle = 90 degrees)");
    }
    
    return Data::Value(result);
}

} // namespace Math
} // namespace StdLib
} // namespace Sad
