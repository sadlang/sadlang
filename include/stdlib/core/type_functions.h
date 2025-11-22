/**
 * @file type_functions.h
 * @brief (AR) ملف رأسي لدوال الأنواع والتحويلات / (EN) Header file for Type Conversion and Checking Functions
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 * (AR) الوصف العام:
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يحتوي على تعريفات جميع دوال الأنواع في لغة ص، والتي تشمل:
 * 
 * 1. **دوال التحويل** (5 دوال):
 *    - تحويل أي قيمة إلى نص، رقم صحيح، رقم عشري، مصفوفة، أو قيمة منطقية
 *    - معالجة آمنة للأخطاء مع قيم افتراضية معقولة
 * 
 * 2. **دوال الفحص** (4 دوال):
 *    - فحص نوع القيمة (هل هي رقم صحيح، عشري، نص، أو مصفوفة)
 *    - إرجاع قيمة منطقية (true/false)
 * 
 * 3. **دالة معلومات النوع** (1 دالة):
 *    - الحصول على اسم النوع كنص
 * 
 * **الميزات الرئيسية:**
 * - دعم ثنائي اللغة (عربي/إنجليزي)
 * - تحويلات آمنة مع معالجة الأخطاء
 * - توثيق شامل لكل دالة
 * - أمثلة استخدام واضحة
 * - دعم جميع أنواع البيانات في اللغة
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 * (EN) General Description:
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * This file contains definitions for all type-related functions in S Language, including:
 * 
 * 1. **Conversion Functions** (5 functions):
 *    - Convert any value to string, integer, float, array, or boolean
 *    - Safe error handling with reasonable default values
 * 
 * 2. **Type Checking Functions** (4 functions):
 *    - Check value type (is it integer, float, string, or array)
 *    - Returns boolean value (true/false)
 * 
 * 3. **Type Information Function** (1 function):
 *    - Get type name as string
 * 
 * **Key Features:**
 * - Bilingual support (Arabic/English)
 * - Safe conversions with error handling
 * - Comprehensive documentation for each function
 * - Clear usage examples
 * - Support for all data types in the language
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * @example استخدام دوال التحويل / Using Conversion Functions
 * @code{.s}
 * # تحويل إلى نص / Convert to string
 * رقم = 42
 * نص = لنص(رقم)           # "42"
 * نص2 = toString(3.14)    # "3.14"
 * 
 * # تحويل إلى رقم صحيح / Convert to integer
 * عدد = لرقم_صحيح("123")  # 123
 * عدد2 = toInt(3.14)       # 3
 * 
 * # تحويل إلى رقم عشري / Convert to float
 * عشري = لرقم_عشري("3.14") # 3.14
 * عشري2 = toFloat(42)      # 42.0
 * 
 * # تحويل إلى منطقي / Convert to boolean
 * منطقي = لمنطقي(1)        # صحيح
 * منطقي2 = toBool("")      # خطأ
 * @endcode
 * 
 * @example استخدام دوال الفحص / Using Type Checking Functions
 * @code{.s}
 * # فحص الأنواع / Check types
 * إذا هل_صحيح(قيمة) {
 *     اطبع("القيمة رقم صحيح")
 * }
 * 
 * إذا isFloat(قيمة) {
 *     اطبع("القيمة رقم عشري")
 * }
 * 
 * # الحصول على اسم النوع / Get type name
 * نوع_المتغير = النوع(قيمة)
 * اطبع("نوع القيمة:", نوع_المتغير)
 * @endcode
 * 
 * @author S Language Development Team
 * @date November 22, 2025
 * @version 1.0
 */

#ifndef SAD_STDLIB_TYPE_FUNCTIONS_H
#define SAD_STDLIB_TYPE_FUNCTIONS_H

#include "data/types/value.h"
#include <vector>
#include <string>

namespace Sad {
namespace StdLib {
namespace Core {

/**
 * @class TypeFunctions
 * @brief (AR) فئة تحتوي على جميع دوال الأنواع والتحويلات
 * @brief (EN) Class containing all type conversion and checking functions
 */
class TypeFunctions {
public:
    // ========================================================================
    // (AR) دوال التحويل / (EN) Conversion Functions
    // ========================================================================
    
    /**
     * @brief (AR) تحويل أي قيمة إلى نص
     * @brief (EN) Convert any value to string
     * 
     * @details (AR)
     * تقوم هذه الدالة بتحويل أي نوع من البيانات إلى نص. تدعم جميع الأنواع:
     * - الأرقام الصحيحة: 42 → "42"
     * - الأرقام العشرية: 3.14 → "3.14"
     * - النصوص: تبقى كما هي
     * - القيم المنطقية: صحيح → "true" / "صحيح"، خطأ → "false" / "خطأ"
     * - المصفوفات: [1,2,3] → "[1, 2, 3]"
     * - الفراغ: لاشيء → "null" / "لاشيء"
     * 
     * @details (EN)
     * Converts any data type to string. Supports all types:
     * - Integers: 42 → "42"
     * - Floats: 3.14 → "3.14"
     * - Strings: remain as-is
     * - Booleans: true → "true"/"صحيح", false → "false"/"خطأ"
     * - Arrays: [1,2,3] → "[1, 2, 3]"
     * - Void: null → "null"/"لاشيء"
     * 
     * @param args (AR) قائمة المعاملات تحتوي على قيمة واحدة للتحويل
     * @param args (EN) Argument list containing one value to convert
     * @return Data::Value (AR) النص الناتج من التحويل
     * @return Data::Value (EN) Resulting string from conversion
     * 
     * @example
     * @code{.s}
     * نص1 = لنص(42)           # "42"
     * نص2 = toString(3.14)    # "3.14"
     * نص3 = لنص([1, 2, 3])    # "[1, 2, 3]"
     * نص4 = toString(صحيح)    # "true"
     * @endcode
     * 
     * @note (AR) هذه الدالة آمنة ولا ترمي أخطاء
     * @note (EN) This function is safe and doesn't throw errors
     */
    static Data::Value toString(const std::vector<Data::Value>& args);
    
    /**
     * @brief (AR) تحويل قيمة إلى رقم صحيح
     * @brief (EN) Convert value to integer
     * 
     * @details (AR)
     * تحول القيمة إلى رقم صحيح:
     * - النصوص: "123" → 123، "3.14" → 3
     * - الأرقام العشرية: 3.14 → 3، -2.7 → -2 (قطع الكسور)
     * - القيم المنطقية: صحيح → 1، خطأ → 0
     * - النصوص غير الصالحة: "hello" → 0
     * 
     * @details (EN)
     * Converts value to integer:
     * - Strings: "123" → 123, "3.14" → 3
     * - Floats: 3.14 → 3, -2.7 → -2 (truncation)
     * - Booleans: true → 1, false → 0
     * - Invalid strings: "hello" → 0
     * 
     * @param args (AR) قائمة المعاملات تحتوي على قيمة واحدة للتحويل
     * @param args (EN) Argument list containing one value to convert
     * @return Data::Value (AR) الرقم الصحيح الناتج
     * @return Data::Value (EN) Resulting integer
     * 
     * @example
     * @code{.s}
     * عدد1 = لرقم_صحيح("123")   # 123
     * عدد2 = toInt(3.14)        # 3
     * عدد3 = لرقم_صحيح(صحيح)   # 1
     * عدد4 = toInt("hello")     # 0
     * @endcode
     */
    static Data::Value toInt(const std::vector<Data::Value>& args);
    
    /**
     * @brief (AR) تحويل قيمة إلى رقم عشري
     * @brief (EN) Convert value to floating-point number
     * 
     * @details (AR)
     * تحول القيمة إلى رقم عشري:
     * - النصوص: "3.14" → 3.14، "42" → 42.0
     * - الأرقام الصحيحة: 42 → 42.0
     * - القيم المنطقية: صحيح → 1.0، خطأ → 0.0
     * - النصوص غير الصالحة: "hello" → 0.0
     * 
     * @details (EN)
     * Converts value to float:
     * - Strings: "3.14" → 3.14, "42" → 42.0
     * - Integers: 42 → 42.0
     * - Booleans: true → 1.0, false → 0.0
     * - Invalid strings: "hello" → 0.0
     * 
     * @param args (AR) قائمة المعاملات تحتوي على قيمة واحدة للتحويل
     * @param args (EN) Argument list containing one value to convert
     * @return Data::Value (AR) الرقم العشري الناتج
     * @return Data::Value (EN) Resulting float
     * 
     * @example
     * @code{.s}
     * عشري1 = لرقم_عشري("3.14") # 3.14
     * عشري2 = toFloat(42)        # 42.0
     * عشري3 = لرقم_عشري(صحيح)   # 1.0
     * @endcode
     */
    static Data::Value toFloat(const std::vector<Data::Value>& args);
    
    /**
     * @brief (AR) تحويل قيمة إلى مصفوفة
     * @brief (EN) Convert value to array
     * 
     * @details (AR)
     * تحول القيمة إلى مصفوفة:
     * - المصفوفات: تبقى كما هي
     * - النصوص: "hello" → ['h', 'e', 'l', 'l', 'o']
     * - الأرقام: 123 → [123] (مصفوفة بعنصر واحد)
     * - القيم المنطقية: صحيح → [true]
     * 
     * @details (EN)
     * Converts value to array:
     * - Arrays: remain as-is
     * - Strings: "hello" → ['h', 'e', 'l', 'l', 'o']
     * - Numbers: 123 → [123] (single element array)
     * - Booleans: true → [true]
     * 
     * @param args (AR) قائمة المعاملات تحتوي على قيمة واحدة للتحويل
     * @param args (EN) Argument list containing one value to convert
     * @return Data::Value (AR) المصفوفة الناتجة
     * @return Data::Value (EN) Resulting array
     * 
     * @example
     * @code{.s}
     * مصف1 = لمصفوفة("hello")    # ['h','e','l','l','o']
     * مصف2 = toArray(42)         # [42]
     * مصف3 = لمصفوفة([1,2,3])    # [1,2,3]
     * @endcode
     */
    static Data::Value toArray(const std::vector<Data::Value>& args);
    
    /**
     * @brief (AR) تحويل قيمة إلى قيمة منطقية
     * @brief (EN) Convert value to boolean
     * 
     * @details (AR)
     * تحول القيمة إلى قيمة منطقية حسب القواعد التالية:
     * - الأرقام: 0 → خطأ، أي رقم آخر → صحيح
     * - النصوص: "" → خطأ، أي نص آخر → صحيح
     * - المصفوفات: [] → خطأ، أي مصفوفة أخرى → صحيح
     * - الفراغ: لاشيء → خطأ
     * 
     * @details (EN)
     * Converts value to boolean using these rules:
     * - Numbers: 0 → false, any other → true
     * - Strings: "" → false, any other → true
     * - Arrays: [] → false, any other → true
     * - Void: null → false
     * 
     * @param args (AR) قائمة المعاملات تحتوي على قيمة واحدة للتحويل
     * @param args (EN) Argument list containing one value to convert
     * @return Data::Value (AR) القيمة المنطقية الناتجة
     * @return Data::Value (EN) Resulting boolean
     * 
     * @example
     * @code{.s}
     * منطقي1 = لمنطقي(1)        # صحيح
     * منطقي2 = toBool(0)        # خطأ
     * منطقي3 = لمنطقي("")      # خطأ
     * منطقي4 = toBool("hello")  # صحيح
     * @endcode
     */
    static Data::Value toBool(const std::vector<Data::Value>& args);
    
    // ========================================================================
    // (AR) دوال فحص الأنواع / (EN) Type Checking Functions
    // ========================================================================
    
    /**
     * @brief (AR) فحص ما إذا كانت القيمة رقماً صحيحاً
     * @brief (EN) Check if value is an integer
     * 
     * @param args (AR) قائمة المعاملات تحتوي على قيمة واحدة للفحص
     * @param args (EN) Argument list containing one value to check
     * @return Data::Value (AR) صحيح إذا كانت القيمة رقماً صحيحاً، وإلا خطأ
     * @return Data::Value (EN) true if value is integer, false otherwise
     * 
     * @example
     * @code{.s}
     * هل_صحيح(42)      # صحيح
     * isInt(3.14)      # خطأ
     * هل_صحيح("123")  # خطأ
     * @endcode
     */
    static Data::Value isInt(const std::vector<Data::Value>& args);
    
    /**
     * @brief (AR) فحص ما إذا كانت القيمة رقماً عشرياً
     * @brief (EN) Check if value is a floating-point number
     * 
     * @param args (AR) قائمة المعاملات تحتوي على قيمة واحدة للفحص
     * @param args (EN) Argument list containing one value to check
     * @return Data::Value (AR) صحيح إذا كانت القيمة رقماً عشرياً، وإلا خطأ
     * @return Data::Value (EN) true if value is float, false otherwise
     * 
     * @example
     * @code{.s}
     * هل_عشري(3.14)    # صحيح
     * isFloat(42)      # خطأ
     * هل_عشري("3.14")  # خطأ
     * @endcode
     */
    static Data::Value isFloat(const std::vector<Data::Value>& args);
    
    /**
     * @brief (AR) فحص ما إذا كانت القيمة نصاً
     * @brief (EN) Check if value is a string
     * 
     * @param args (AR) قائمة المعاملات تحتوي على قيمة واحدة للفحص
     * @param args (EN) Argument list containing one value to check
     * @return Data::Value (AR) صحيح إذا كانت القيمة نصاً، وإلا خطأ
     * @return Data::Value (EN) true if value is string, false otherwise
     * 
     * @example
     * @code{.s}
     * هل_نص("hello")   # صحيح
     * isString(42)     # خطأ
     * هل_نص([1,2,3])   # خطأ
     * @endcode
     */
    static Data::Value isString(const std::vector<Data::Value>& args);
    
    /**
     * @brief (AR) فحص ما إذا كانت القيمة مصفوفة
     * @brief (EN) Check if value is an array
     * 
     * @param args (AR) قائمة المعاملات تحتوي على قيمة واحدة للفحص
     * @param args (EN) Argument list containing one value to check
     * @return Data::Value (AR) صحيح إذا كانت القيمة مصفوفة، وإلا خطأ
     * @return Data::Value (EN) true if value is array, false otherwise
     * 
     * @example
     * @code{.s}
     * هل_مصفوفة([1,2,3]) # صحيح
     * isArray(42)        # خطأ
     * هل_مصفوفة("hello") # خطأ
     * @endcode
     */
    static Data::Value isArray(const std::vector<Data::Value>& args);
    
    // ========================================================================
    // (AR) دالة معلومات النوع / (EN) Type Information Function
    // ========================================================================
    
    /**
     * @brief (AR) الحصول على اسم نوع القيمة
     * @brief (EN) Get the type name of a value
     * 
     * @details (AR)
     * ترجع هذه الدالة اسم نوع القيمة كنص:
     * - "integer" أو "رقم_صحيح"
     * - "double" أو "رقم_عشري"
     * - "string" أو "نص"
     * - "boolean" أو "منطقي"
     * - "array" أو "مصفوفة"
     * - "void" أو "فراغ"
     * 
     * @details (EN)
     * Returns the type name of value as string:
     * - "integer" or "رقم_صحيح"
     * - "double" or "رقم_عشري"
     * - "string" or "نص"
     * - "boolean" or "منطقي"
     * - "array" or "مصفوفة"
     * - "void" or "فراغ"
     * 
     * @param args (AR) قائمة المعاملات تحتوي على قيمة واحدة للفحص
     * @param args (EN) Argument list containing one value to check
     * @return Data::Value (AR) اسم النوع كنص
     * @return Data::Value (EN) Type name as string
     * 
     * @example
     * @code{.s}
     * النوع(42)         # "integer"
     * type(3.14)        # "double"
     * النوع("hello")    # "string"
     * type([1,2,3])     # "array"
     * @endcode
     */
    static Data::Value type(const std::vector<Data::Value>& args);

private:
    /**
     * @brief (AR) دالة مساعدة للتحقق من عدد المعاملات
     * @brief (EN) Helper function to validate argument count
     */
    static void validateArgCount(const std::vector<Data::Value>& args, 
                                 size_t expected, 
                                 const std::string& funcName);
};

} // namespace Core
} // namespace StdLib
} // namespace Sad

#endif // SAD_STDLIB_TYPE_FUNCTIONS_H
