/**
 * @file advanced_math.cpp
 * @brief Implementation of Advanced Math Functions
 * @date February 4, 2026
 */

#include "advanced_math.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>

using namespace Sad::Data;

namespace Sad {
namespace StdLib {
namespace Math {

// Natural logarithm / لوغاريتم طبيعي
Value AdvancedMath::log(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::invalid_argument("log requires 1 argument");
    }
    
    double value = args[0].toDouble();
    if (value <= 0) {
        throw std::domain_error("log requires positive number");
    }
    
    return Value(std::log(value));
}

// Base-10 logarithm / لوغاريتم عشري
Value AdvancedMath::log10(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::invalid_argument("log10 requires 1 argument");
    }
    
    double value = args[0].toDouble();
    if (value <= 0) {
        throw std::domain_error("log10 requires positive number");
    }
    
    return Value(std::log10(value));
}

// Base-2 logarithm / لوغاريتم ثنائي
Value AdvancedMath::log2(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::invalid_argument("log2 requires 1 argument");
    }
    
    double value = args[0].toDouble();
    if (value <= 0) {
        throw std::domain_error("log2 requires positive number");
    }
    
    return Value(std::log2(value));
}

// Exponential e^x / أسّي
Value AdvancedMath::exp(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::invalid_argument("exp requires 1 argument");
    }
    
    double value = args[0].toDouble();
    return Value(std::exp(value));
}

// Minimum of multiple values / أصغر قيمة
Value AdvancedMath::min(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::invalid_argument("min requires at least 1 argument");
    }
    
    double minVal = args[0].toDouble();
    for (size_t i = 1; i < args.size(); i++) {
        minVal = std::min(minVal, args[i].toDouble());
    }
    
    return Value(minVal);
}

// Maximum of multiple values / أكبر قيمة
Value AdvancedMath::max(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::invalid_argument("max requires at least 1 argument");
    }
    
    double maxVal = args[0].toDouble();
    for (size_t i = 1; i < args.size(); i++) {
        maxVal = std::max(maxVal, args[i].toDouble());
    }
    
    return Value(maxVal);
}

// Clamp value between min and max / تقييد القيمة
Value AdvancedMath::clamp(const std::vector<Value>& args) {
    if (args.size() < 3) {
        throw std::invalid_argument("clamp requires 3 arguments: value, min, max");
    }
    
    double value = args[0].toDouble();
    double minVal = args[1].toDouble();
    double maxVal = args[2].toDouble();
    
    if (minVal > maxVal) {
        throw std::invalid_argument("min must be <= max");
    }
    
    return Value(std::clamp(value, minVal, maxVal));
}

// Truncate / اقتطاع
Value AdvancedMath::trunc(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::invalid_argument("trunc requires 1 argument");
    }
    
    double value = args[0].toDouble();
    return Value(std::trunc(value));
}

// Modulo / باقي القسمة
Value AdvancedMath::mod(const std::vector<Value>& args) {
    if (args.size() < 2) {
        throw std::invalid_argument("mod requires 2 arguments");
    }
    
    int a = args[0].toInt();
    int b = args[1].toInt();
    
    if (b == 0) {
        throw std::domain_error("modulo by zero");
    }
    
    return Value(a % b);
}

// Sign of number / إشارة الرقم
Value AdvancedMath::sign(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::invalid_argument("sign requires 1 argument");
    }
    
    double value = args[0].toDouble();
    
    if (value > 0) return Value(1);
    if (value < 0) return Value(-1);
    return Value(0);
}

// Pi constant / ثابت باي
Value AdvancedMath::pi() {
    return Value(3.14159265358979323846);
}

// e constant / ثابت إي
Value AdvancedMath::e() {
    return Value(2.71828182845904523536);
}

} // namespace Math
} // namespace StdLib
} // namespace Sad
