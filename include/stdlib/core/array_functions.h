/**
 * @file array_functions.h
 * @brief (AR) دوال معالجة المصفوفات / (EN) Array Manipulation Functions
 * 
 * Phase 3 of Standard Library Implementation:
 * Comprehensive array processing functions for the Sad Language interpreter.
 * 
 * Functions implemented:
 * - إضافة() / append() - Add element to array
 * - إزالة() / pop() - Remove and return last element
 * - حجم() / size() - Get array size
 * - فهرس() / indexOf() - Find element index
 * - يحتوي() / contains() - Check if array contains element
 * - قلب() / reverse() - Reverse array order
 * - فرز() / sort() - Sort array elements
 * - أول() / first() - Get first element
 * - آخر() / last() - Get last element
 * - شريحة() / slice() - Extract subarray
 * 
 * @author S Language Development Team
 * @date November 22, 2025
 * @version 1.0
 * 
 * @note
 * (AR) هذه الوحدة توفر دوال شاملة لمعالجة المصفوفات مع دعم أنواع البيانات المختلفة
 * (EN) This module provides comprehensive array processing with support for mixed data types
 * 
 * Integration Points:
 * - Include in: stdlib_manager.h and stdlib_manager.cpp
 * - Used by: Interpreter core during function registration
 * - Tests: tests/stdlib_tests/test_array_functions.cpp
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include "data/types/value.h"

namespace Sad {
namespace StdLib {
namespace Core {

/**
 * @class ArrayFunctions
 * @brief (AR) فئة دوال معالجة المصفوفات / (EN) Array Processing Functions Class
 * 
 * Provides static methods for all array operations required in Phase 3.
 * All methods support mixed-type arrays and handle edge cases properly.
 * 
 * Design Principles:
 * - Type-safe: Proper type checking for all operations
 * - Flexible: Works with arrays of any type
 * - Performance: Optimized for common use cases
 * - Safety: Bounds checking and error handling
 */
class ArrayFunctions {
public:
    // Delete default constructor - only static methods
    ArrayFunctions() = delete;
    ~ArrayFunctions() = default;

    /**
     * @brief (AR) إضافة - إضافة عنصر للمصفوفة
     * @brief (EN) Append - Add element to array
     * 
     * @param args[0] (AR) المصفوفة / (EN) Array
     * @param args[1] (AR) العنصر المراد إضافته / (EN) Element to add
     * @return Value (AR) المصفوفة الجديدة / (EN) New array with element added
     * 
     * @note Creates a new array, does not modify original
     * 
     * Example (AR):
     * @code
     * المصفوفة = [1, 2, 3]
     * المصفوفة = إضافة(المصفوفة, 4)    // Returns: [1, 2, 3, 4]
     * @endcode
     * 
     * Example (EN):
     * @code
     * arr = [1, 2, 3]
     * arr = append(arr, 4)              // Returns: [1, 2, 3, 4]
     * @endcode
     */
    static Data::Value append(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) إزالة - إزالة وإرجاع آخر عنصر
     * @brief (EN) Pop - Remove and return last element
     * 
     * @param args[0] (AR) المصفوفة / (EN) Array
     * @return Value (AR) آخر عنصر / (EN) Last element
     * 
     * @note Throws error if array is empty
     * 
     * Example (AR):
     * @code
     * المصفوفة = [1, 2, 3]
     * العنصر = إزالة(المصفوفة)    // Returns: 3, array becomes [1, 2]
     * @endcode
     * 
     * Example (EN):
     * @code
     * arr = [1, 2, 3]
     * elem = pop(arr)              // Returns: 3, array becomes [1, 2]
     * @endcode
     */
    static Data::Value pop(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) حجم - الحصول على حجم المصفوفة
     * @brief (EN) Size - Get array size
     * 
     * @param args[0] (AR) المصفوفة / (EN) Array
     * @return Value (AR) عدد العناصر / (EN) Number of elements
     * 
     * Example (AR):
     * @code
     * المصفوفة = [1, 2, 3, 4, 5]
     * الحجم = حجم(المصفوفة)    // Returns: 5
     * @endcode
     * 
     * Example (EN):
     * @code
     * arr = [1, 2, 3, 4, 5]
     * s = size(arr)            // Returns: 5
     * @endcode
     */
    static Data::Value size(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) فهرس - إيجاد موقع عنصر
     * @brief (EN) IndexOf - Find element index
     * 
     * @param args[0] (AR) المصفوفة / (EN) Array
     * @param args[1] (AR) العنصر المراد البحث عنه / (EN) Element to find
     * @param args[2] (AR) موقع البداية (اختياري، افتراضي 0) / (EN) Start position (optional, default 0)
     * @return Value (AR) موقع العنصر (-1 إذا لم يُعثر عليه) / (EN) Element index (-1 if not found)
     * 
     * Example (AR):
     * @code
     * المصفوفة = [10, 20, 30, 40]
     * الموقع = فهرس(المصفوفة, 30)    // Returns: 2
     * @endcode
     * 
     * Example (EN):
     * @code
     * arr = [10, 20, 30, 40]
     * idx = indexOf(arr, 30)         // Returns: 2
     * @endcode
     */
    static Data::Value indexOf(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) يحتوي - التحقق من وجود عنصر
     * @brief (EN) Contains - Check if array contains element
     * 
     * @param args[0] (AR) المصفوفة / (EN) Array
     * @param args[1] (AR) العنصر / (EN) Element
     * @return Value (AR) صحيح أو خطأ / (EN) Boolean result
     * 
     * Example (AR):
     * @code
     * المصفوفة = [1, 2, 3, 4]
     * الموجود = يحتوي(المصفوفة, 3)    // Returns: true
     * @endcode
     * 
     * Example (EN):
     * @code
     * arr = [1, 2, 3, 4]
     * exists = contains(arr, 3)        // Returns: true
     * @endcode
     */
    static Data::Value contains(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) قلب - قلب ترتيب العناصر
     * @brief (EN) Reverse - Reverse array order
     * 
     * @param args[0] (AR) المصفوفة / (EN) Array
     * @return Value (AR) المصفوفة المقلوبة / (EN) Reversed array
     * 
     * Example (AR):
     * @code
     * المصفوفة = [1, 2, 3, 4]
     * المقلوبة = قلب(المصفوفة)    // Returns: [4, 3, 2, 1]
     * @endcode
     * 
     * Example (EN):
     * @code
     * arr = [1, 2, 3, 4]
     * rev = reverse(arr)          // Returns: [4, 3, 2, 1]
     * @endcode
     */
    static Data::Value reverse(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) فرز - فرز عناصر المصفوفة
     * @brief (EN) Sort - Sort array elements
     * 
     * @param args[0] (AR) المصفوفة / (EN) Array
     * @param args[1] (AR) تصاعدي/تنازلي (اختياري، افتراضي true) / (EN) Ascending (optional, default true)
     * @return Value (AR) المصفوفة المفروزة / (EN) Sorted array
     * 
     * @note Works with numbers and strings
     * 
     * Example (AR):
     * @code
     * المصفوفة = [3, 1, 4, 1, 5]
     * المفروزة = فرز(المصفوفة)         // Returns: [1, 1, 3, 4, 5]
     * التنازلية = فرز(المصفوفة, false) // Returns: [5, 4, 3, 1, 1]
     * @endcode
     * 
     * Example (EN):
     * @code
     * arr = [3, 1, 4, 1, 5]
     * sorted = sort(arr)              // Returns: [1, 1, 3, 4, 5]
     * desc = sort(arr, false)         // Returns: [5, 4, 3, 1, 1]
     * @endcode
     */
    static Data::Value sort(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) أول - الحصول على أول عنصر
     * @brief (EN) First - Get first element
     * 
     * @param args[0] (AR) المصفوفة / (EN) Array
     * @return Value (AR) أول عنصر / (EN) First element
     * 
     * @note Throws error if array is empty
     * 
     * Example (AR):
     * @code
     * المصفوفة = [10, 20, 30]
     * الأول = أول(المصفوفة)    // Returns: 10
     * @endcode
     * 
     * Example (EN):
     * @code
     * arr = [10, 20, 30]
     * f = first(arr)          // Returns: 10
     * @endcode
     */
    static Data::Value first(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) آخر - الحصول على آخر عنصر
     * @brief (EN) Last - Get last element
     * 
     * @param args[0] (AR) المصفوفة / (EN) Array
     * @return Value (AR) آخر عنصر / (EN) Last element
     * 
     * @note Throws error if array is empty
     * 
     * Example (AR):
     * @code
     * المصفوفة = [10, 20, 30]
     * الأخير = آخر(المصفوفة)    // Returns: 30
     * @endcode
     * 
     * Example (EN):
     * @code
     * arr = [10, 20, 30]
     * l = last(arr)            // Returns: 30
     * @endcode
     */
    static Data::Value last(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) شريحة - استخراج جزء من المصفوفة
     * @brief (EN) Slice - Extract subarray
     * 
     * @param args[0] (AR) المصفوفة / (EN) Array
     * @param args[1] (AR) موقع البداية / (EN) Start index
     * @param args[2] (AR) موقع النهاية (اختياري، -1 = حتى النهاية) / (EN) End index (optional, -1 = to end)
     * @return Value (AR) الجزء المستخرج / (EN) Extracted subarray
     * 
     * Example (AR):
     * @code
     * المصفوفة = [1, 2, 3, 4, 5]
     * الجزء = شريحة(المصفوفة, 1, 3)    // Returns: [2, 3]
     * الباقي = شريحة(المصفوفة, 2)      // Returns: [3, 4, 5]
     * @endcode
     * 
     * Example (EN):
     * @code
     * arr = [1, 2, 3, 4, 5]
     * part = slice(arr, 1, 3)          // Returns: [2, 3]
     * rest = slice(arr, 2)             // Returns: [3, 4, 5]
     * @endcode
     */
    static Data::Value slice(const std::vector<Data::Value>& args);

private:
    /**
     * @brief (AR) التحقق من صحة المعاملات
     * @brief (EN) Validate arguments
     * 
     * @param args (AR) المعاملات / (EN) Arguments
     * @param minArgs (AR) الحد الأدنى / (EN) Minimum count
     * @param maxArgs (AR) الحد الأقصى / (EN) Maximum count
     * @return bool (AR) صحيحة أم لا / (EN) Valid or not
     */
    static bool validateArguments(const std::vector<Data::Value>& args,
                                  size_t minArgs, 
                                  int maxArgs = -1);

    /**
     * @brief (AR) مقارنة قيمتين
     * @brief (EN) Compare two values
     * 
     * @param a (AR) القيمة الأولى / (EN) First value
     * @param b (AR) القيمة الثانية / (EN) Second value
     * @return bool (AR) a < b / (EN) a < b
     */
    static bool compareValues(const Data::Value& a, const Data::Value& b);
};

} // namespace Core
} // namespace StdLib
} // namespace Sad
