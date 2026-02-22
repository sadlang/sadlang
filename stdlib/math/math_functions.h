/**
 * @file math_functions.h
 * @brief (AR) ملف رأسي لدوال الرياضيات / (EN) Header file for Mathematical Functions
 * 
 * This header declares all mathematical functions for Sad Language standard library.
 * Includes basic arithmetic, trigonometric, and utility math functions.
 * 
 * @author S Language Development Team
 * @date November 22, 2025
 * @version 1.0
 */

#ifndef SAD_STDLIB_MATH_FUNCTIONS_H
#define SAD_STDLIB_MATH_FUNCTIONS_H

#include "value.h"
#include <vector>

namespace Sad {
namespace StdLib {
namespace Math {

/**
 * @class MathFunctions
 * @brief (AR) فئة تحتوي على جميع الدوال الرياضية
 * @brief (EN) Class containing all mathematical functions
 */
class MathFunctions {
public:
    // ========================================================================
    // (AR) دوال رياضية أساسية / (EN) Basic Math Functions
    // ========================================================================
    
    /**
     * @brief (AR) جذر - حساب الجذر التربيعي
     * @brief (EN) Sqrt - Calculate square root
     * 
     * @param args[0] (AR) رقم (رقم أو رقم حقيقي) / (EN) number (رقم or رقم_حقيقي)
     * @return (AR) الجذر التربيعي / (EN) Square root
     * 
     * @example (AR)
     * ```
     * رقم_حقيقي نتيجة = جذر(16)  // نتيجة = 4.0
     * رقم_حقيقي نتيجة2 = جذر(2)   // نتيجة2 = 1.414...
     * ```
     * 
     * @example (EN)
     * ```
     * real result = sqrt(16)  // result = 4.0
     * real result2 = sqrt(2)  // result2 = 1.414...
     * ```
     */
    static Data::Value sqrt(const std::vector<Data::Value>& args);
    
    /**
     * @brief (AR) أس - رفع عدد لأس معين
     * @brief (EN) Power - Raise number to power
     * 
     * @param args[0] (AR) الأساس / (EN) base
     * @param args[1] (AR) الأس / (EN) exponent
     * @return (AR) الأساس مرفوع للأس / (EN) base raised to exponent
     * 
     * @example (AR)
     * ```
     * رقم نتيجة = أس(2، 3)     // نتيجة = 8
     * رقم_حقيقي نتيجة2 = أس(2.5، 2)  // نتيجة2 = 6.25
     * ```
     * 
     * @example (EN)
     * ```
     * int result = power(2, 3)      // result = 8
     * real result2 = power(2.5, 2)  // result2 = 6.25
     * ```
     */
    static Data::Value power(const std::vector<Data::Value>& args);
    
    /**
     * @brief (AR) مطلق - القيمة المطلقة
     * @brief (EN) Abs - Absolute value
     * 
     * @param args[0] (AR) رقم / (EN) number
     * @return (AR) القيمة المطلقة / (EN) Absolute value
     * 
     * @example (AR)
     * ```
     * رقم نتيجة = مطلق(-5)    // نتيجة = 5
     * رقم_حقيقي نتيجة2 = مطلق(-3.14)  // نتيجة2 = 3.14
     * ```
     * 
     * @example (EN)
     * ```
     * int result = abs(-5)       // result = 5
     * real result2 = abs(-3.14)  // result2 = 3.14
     * ```
     */
    static Data::Value abs(const std::vector<Data::Value>& args);
    
    /**
     * @brief (AR) أكبر - إيجاد أكبر قيمة
     * @brief (EN) Max - Find maximum value
     * 
     * @param args (AR) قائمة من الأرقام / (EN) list of numbers
     * @return (AR) أكبر قيمة / (EN) Maximum value
     * 
     * @example (AR)
     * ```
     * رقم نتيجة = أكبر(5، 10، 3، 8)  // نتيجة = 10
     * رقم_حقيقي نتيجة2 = أكبر(1.5، 2.7، 1.9)  // نتيجة2 = 2.7
     * ```
     * 
     * @example (EN)
     * ```
     * int result = max(5, 10, 3, 8)       // result = 10
     * real result2 = max(1.5, 2.7, 1.9)   // result2 = 2.7
     * ```
     */
    static Data::Value max(const std::vector<Data::Value>& args);
    
    /**
     * @brief (AR) أصغر - إيجاد أصغر قيمة
     * @brief (EN) Min - Find minimum value
     * 
     * @param args (AR) قائمة من الأرقام / (EN) list of numbers
     * @return (AR) أصغر قيمة / (EN) Minimum value
     * 
     * @example (AR)
     * ```
     * رقم نتيجة = أصغر(5، 10، 3، 8)  // نتيجة = 3
     * رقم_حقيقي نتيجة2 = أصغر(1.5، 2.7، 1.9)  // نتيجة2 = 1.5
     * ```
     * 
     * @example (EN)
     * ```
     * int result = min(5, 10, 3, 8)       // result = 3
     * real result2 = min(1.5, 2.7, 1.9)   // result2 = 1.5
     * ```
     */
    static Data::Value min(const std::vector<Data::Value>& args);
    
    /**
     * @brief (AR) تقريب - تقريب لأقرب عدد صحيح
     * @brief (EN) Round - Round to nearest integer
     * 
     * @param args[0] (AR) رقم حقيقي / (EN) real number
     * @return (AR) الرقم مقرباً / (EN) Rounded number
     * 
     * @example (AR)
     * ```
     * رقم نتيجة1 = تقريب(3.2)  // نتيجة1 = 3
     * رقم نتيجة2 = تقريب(3.7)  // نتيجة2 = 4
     * رقم نتيجة3 = تقريب(3.5)  // نتيجة3 = 4
     * ```
     * 
     * @example (EN)
     * ```
     * int result1 = round(3.2)  // result1 = 3
     * int result2 = round(3.7)  // result2 = 4
     * int result3 = round(3.5)  // result3 = 4
     * ```
     */
    static Data::Value round(const std::vector<Data::Value>& args);
    
    /**
     * @brief (AR) أرضية - التقريب للأسفل
     * @brief (EN) Floor - Round down
     * 
     * @param args[0] (AR) رقم حقيقي / (EN) real number
     * @return (AR) الرقم مقرباً للأسفل / (EN) Number rounded down
     * 
     * @example (AR)
     * ```
     * رقم نتيجة1 = أرضية(3.2)   // نتيجة1 = 3
     * رقم نتيجة2 = أرضية(3.9)   // نتيجة2 = 3
     * رقم نتيجة3 = أرضية(-2.5)  // نتيجة3 = -3
     * ```
     * 
     * @example (EN)
     * ```
     * int result1 = floor(3.2)   // result1 = 3
     * int result2 = floor(3.9)   // result2 = 3
     * int result3 = floor(-2.5)  // result3 = -3
     * ```
     */
    static Data::Value floor(const std::vector<Data::Value>& args);
    
    /**
     * @brief (AR) سقف - التقريب للأعلى
     * @brief (EN) Ceil - Round up
     * 
     * @param args[0] (AR) رقم حقيقي / (EN) real number
     * @return (AR) الرقم مقرباً للأعلى / (EN) Number rounded up
     * 
     * @example (AR)
     * ```
     * رقم نتيجة1 = سقف(3.2)   // نتيجة1 = 4
     * رقم نتيجة2 = سقف(3.9)   // نتيجة2 = 4
     * رقم نتيجة3 = سقف(-2.5)  // نتيجة3 = -2
     * ```
     * 
     * @example (EN)
     * ```
     * int result1 = ceil(3.2)   // result1 = 4
     * int result2 = ceil(3.9)   // result2 = 4
     * int result3 = ceil(-2.5)  // result3 = -2
     * ```
     */
    static Data::Value ceil(const std::vector<Data::Value>& args);
    
    /**
     * @brief (AR) مربع - حساب مربع العدد
     * @brief (EN) Square - Calculate square of number
     * 
     * @param args[0] (AR) رقم / (EN) number
     * @return (AR) مربع العدد / (EN) Square of number
     * 
     * @example (AR)
     * ```
     * رقم نتيجة = مربع(5)     // نتيجة = 25
     * رقم_حقيقي نتيجة2 = مربع(2.5)  // نتيجة2 = 6.25
     * ```
     * 
     * @example (EN)
     * ```
     * int result = square(5)       // result = 25
     * real result2 = square(2.5)   // result2 = 6.25
     * ```
     */
    static Data::Value square(const std::vector<Data::Value>& args);
    
    // ========================================================================
    // (AR) دوال مثلثية / (EN) Trigonometric Functions
    // ========================================================================
    
    /**
     * @brief (AR) جيب - حساب جيب الزاوية (بالراديان)
     * @brief (EN) Sin - Calculate sine (in radians)
     * 
     * @param args[0] (AR) زاوية بالراديان / (EN) angle in radians
     * @return (AR) جيب الزاوية / (EN) Sine of angle
     * 
     * @example (AR)
     * ```
     * رقم_حقيقي نتيجة = جيب(0)           // نتيجة = 0
     * رقم_حقيقي نتيجة2 = جيب(3.14159/2)  // نتيجة2 ≈ 1
     * ```
     * 
     * @example (EN)
     * ```
     * real result = sin(0)           // result = 0
     * real result2 = sin(3.14159/2)  // result2 ≈ 1
     * ```
     */
    static Data::Value sin(const std::vector<Data::Value>& args);
    
    /**
     * @brief (AR) جيب_تمام - حساب جيب تمام الزاوية (بالراديان)
     * @brief (EN) Cos - Calculate cosine (in radians)
     * 
     * @param args[0] (AR) زاوية بالراديان / (EN) angle in radians
     * @return (AR) جيب تمام الزاوية / (EN) Cosine of angle
     * 
     * @example (AR)
     * ```
     * رقم_حقيقي نتيجة = جيب_تمام(0)        // نتيجة = 1
     * رقم_حقيقي نتيجة2 = جيب_تمام(3.14159) // نتيجة2 ≈ -1
     * ```
     * 
     * @example (EN)
     * ```
     * real result = cos(0)        // result = 1
     * real result2 = cos(3.14159) // result2 ≈ -1
     * ```
     */
    static Data::Value cos(const std::vector<Data::Value>& args);
    
    /**
     * @brief (AR) ظل - حساب ظل الزاوية (بالراديان)
     * @brief (EN) Tan - Calculate tangent (in radians)
     * 
     * @param args[0] (AR) زاوية بالراديان / (EN) angle in radians
     * @return (AR) ظل الزاوية / (EN) Tangent of angle
     * 
     * @example (AR)
     * ```
     * رقم_حقيقي نتيجة = ظل(0)           // نتيجة = 0
     * رقم_حقيقي نتيجة2 = ظل(3.14159/4)  // نتيجة2 ≈ 1
     * ```
     * 
     * @example (EN)
     * ```
     * real result = tan(0)           // result = 0
     * real result2 = tan(3.14159/4)  // result2 ≈ 1
     * ```
     */
    static Data::Value tan(const std::vector<Data::Value>& args);

    // ========================================================================
    // (AR) دوال مثلثية عكسية / (EN) Inverse Trigonometric Functions
    // ========================================================================
    static Data::Value asin(const std::vector<Data::Value>& args);
    static Data::Value acos(const std::vector<Data::Value>& args);
    static Data::Value atan(const std::vector<Data::Value>& args);
    static Data::Value atan2(const std::vector<Data::Value>& args);

    // ========================================================================
    // (AR) دوال لوغاريتمية وأسية / (EN) Logarithmic & Exponential Functions
    // ========================================================================
    static Data::Value log(const std::vector<Data::Value>& args);
    static Data::Value log2(const std::vector<Data::Value>& args);
    static Data::Value log10(const std::vector<Data::Value>& args);
    static Data::Value exp(const std::vector<Data::Value>& args);

    // ========================================================================
    // (AR) ثوابت رياضية / (EN) Math Constants
    // ========================================================================
    static Data::Value pi(const std::vector<Data::Value>& args);
    static Data::Value e(const std::vector<Data::Value>& args);

    // ========================================================================
    // (AR) دوال عشوائية / (EN) Random Functions
    // ========================================================================
    static Data::Value random(const std::vector<Data::Value>& args);
    static Data::Value randomInt(const std::vector<Data::Value>& args);

private:
    /**
     * @brief (AR) التحقق من صحة المعاملات
     * @brief (EN) Validate function arguments
     */
    static bool validateArguments(const std::vector<Data::Value>& args,
                                  size_t minArgs, 
                                  int maxArgs = -1);
    
    /**
     * @brief (AR) تحويل القيمة إلى رقم حقيقي
     * @brief (EN) Convert value to double
     */
    static double toDouble(const Data::Value& value);
};

} // namespace Math
} // namespace StdLib
} // namespace Sad

#endif // SAD_STDLIB_MATH_FUNCTIONS_H
