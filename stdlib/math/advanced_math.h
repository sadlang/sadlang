/**
 * @file advanced_math.h
 * @brief Advanced Math Functions for Sad Language  
 * @date February 4, 2026
 */

#ifndef SAD_ADVANCED_MATH_H
#define SAD_ADVANCED_MATH_H

#include "value.h"
#include <vector>

namespace Sad {
namespace StdLib {
namespace Math {

class AdvancedMath {
public:
    // Logarithmic functions
    static Data::Value log(const std::vector<Data::Value>& args);      // Natural log
    static Data::Value log10(const std::vector<Data::Value>& args);    // Base-10 log
    static Data::Value log2(const std::vector<Data::Value>& args);     // Base-2 log
    
    // Exponential
    static Data::Value exp(const std::vector<Data::Value>& args);      // e^x
    
    // Min/Max with multiple arguments
    static Data::Value min(const std::vector<Data::Value>& args);      // Minimum
    static Data::Value max(const std::vector<Data::Value>& args);      // Maximum
    
    // Clamping
    static Data::Value clamp(const std::vector<Data::Value>& args);    // Clamp value
    
    // Rounding variants
    static Data::Value trunc(const std::vector<Data::Value>& args);    // Truncate
    
    // Modulo
    static Data::Value mod(const std::vector<Data::Value>& args);      // Modulo operation
    
    // Sign
    static Data::Value sign(const std::vector<Data::Value>& args);     // Sign of number
    
    // Constants
    static Data::Value pi();    // π
    static Data::Value e();     // e
};

} // namespace Math
} // namespace StdLib
} // namespace Sad

#endif // SAD_ADVANCED_MATH_H
