/**
 * @file string_functions.h
 * @brief (AR) دوال معالجة النصوص / (EN) String Manipulation Functions
 * 
 * Phase 2 of Standard Library Implementation:
 * Comprehensive string processing functions for the Sad Language interpreter.
 * 
 * Functions implemented:
 * - طول() / length() - Get string length
 * - بحث() / find() - Find substring position
 * - استبدل() / replace() - Replace substring
 * - استخراج() / substring() - Extract substring
 * - تحويل_صغير() / toLower() - Convert to lowercase
 * - تحويل_كبير() / toUpper() - Convert to uppercase
 * - قص_أطراف() / trim() - Remove leading/trailing whitespace
 * - تقسيم() / split() - Split string by delimiter
 * - دمج() / join() - Join array elements into string
 * - يبدأ_ب() / startsWith() - Check if string starts with prefix
 * - ينتهي_ب() / endsWith() - Check if string ends with suffix
 * - يحتوي_على() / contains() - Check if string contains substring
 * 
 * @author S Language Development Team
 * @date November 22, 2025
 * @version 1.0
 * 
 * @note
 * (AR) هذه الوحدة توفر دوال شاملة لمعالجة النصوص مع دعم كامل للـ Unicode والعربية
 * (EN) This module provides comprehensive string processing with full Unicode and Arabic support
 * 
 * Integration Points:
 * - Include in: stdlib_manager.h and stdlib_manager.cpp
 * - Used by: Interpreter core during function registration
 * - Tests: tests/stdlib_tests/test_string_functions.cpp
 * 
 * Unicode Support:
 * - All functions handle UTF-8 encoding correctly
 * - Arabic text is processed properly
 * - Bidirectional text is supported
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include "data/types/value.h"

namespace Sad {
namespace StdLib {
namespace String {

/**
 * @class StringFunctions
 * @brief (AR) فئة دوال معالجة النصوص / (EN) String Processing Functions Class
 * 
 * Provides static methods for all string operations required in Phase 2.
 * All methods are Unicode-aware and handle Arabic text correctly.
 * 
 * Design Principles:
 * - Unicode-aware: All functions handle UTF-8 properly
 * - RTL Support: Arabic and other RTL languages work correctly
 * - Performance: Optimized for common use cases
 * - Safety: Proper bounds checking and error handling
 */
class StringFunctions {
public:
    // Delete default constructor - only static methods
    StringFunctions() = delete;
    ~StringFunctions() = default;

    /**
     * @brief (AR) طول - الحصول على طول النص
     * @brief (EN) Length - Get the length of a string
     * 
     * @param args[0] (AR) النص المراد قياسه / (EN) String to measure
     * @return Value (AR) عدد الأحرف (Unicode-aware) / (EN) Number of characters (Unicode-aware)
     * 
     * @note Returns character count, not byte count (handles multi-byte UTF-8)
     * 
     * Example (AR):
     * @code
     * طول("مرحبا")        // Returns: 5
     * طول("Hello")       // Returns: 5
     * طول("")            // Returns: 0
     * @endcode
     * 
     * Example (EN):
     * @code
     * length("مرحبا")    // Returns: 5
     * length("Hello")    // Returns: 5
     * length("")         // Returns: 0
     * @endcode
     */
    static Data::Value length(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) بحث - البحث عن نص فرعي
     * @brief (EN) Find - Find substring position
     * 
     * @param args[0] (AR) النص الأساسي / (EN) Main string
     * @param args[1] (AR) النص المراد البحث عنه / (EN) Substring to find
     * @param args[2] (AR) موقع البداية (اختياري، افتراضي 0) / (EN) Start position (optional, default 0)
     * @return Value (AR) موقع النص (-1 إذا لم يُعثر عليه) / (EN) Position of substring (-1 if not found)
     * 
     * Example (AR):
     * @code
     * بحث("مرحبا بالعالم", "العالم")     // Returns: 7
     * بحث("مرحبا", "وداعا")             // Returns: -1
     * بحث("AAA", "A", 1)                // Returns: 1
     * @endcode
     * 
     * Example (EN):
     * @code
     * find("Hello World", "World")      // Returns: 6
     * find("Hello", "Goodbye")          // Returns: -1
     * find("AAA", "A", 1)               // Returns: 1
     * @endcode
     */
    static Data::Value find(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) استبدل - استبدال نص فرعي بآخر
     * @brief (EN) Replace - Replace substring with another
     * 
     * @param args[0] (AR) النص الأساسي / (EN) Main string
     * @param args[1] (AR) النص المراد استبداله / (EN) Substring to replace
     * @param args[2] (AR) النص البديل / (EN) Replacement string
     * @param args[3] (AR) عدد مرات الاستبدال (اختياري، -1 = الكل) / (EN) Count (-1 = all occurrences)
     * @return Value (AR) النص بعد الاستبدال / (EN) String after replacement
     * 
     * Example (AR):
     * @code
     * استبدل("مرحبا مرحبا", "مرحبا", "أهلا")          // Returns: "أهلا أهلا"
     * استبدل("مرحبا مرحبا", "مرحبا", "أهلا", 1)      // Returns: "أهلا مرحبا"
     * @endcode
     * 
     * Example (EN):
     * @code
     * replace("Hello Hello", "Hello", "Hi")          // Returns: "Hi Hi"
     * replace("Hello Hello", "Hello", "Hi", 1)       // Returns: "Hi Hello"
     * @endcode
     */
    static Data::Value replace(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) استخراج - استخراج جزء من النص
     * @brief (EN) Substring - Extract part of a string
     * 
     * @param args[0] (AR) النص الأساسي / (EN) Main string
     * @param args[1] (AR) موقع البداية / (EN) Start position
     * @param args[2] (AR) الطول (اختياري، -1 = حتى النهاية) / (EN) Length (optional, -1 = to end)
     * @return Value (AR) النص المستخرج / (EN) Extracted substring
     * 
     * Example (AR):
     * @code
     * استخراج("مرحبا بالعالم", 0, 5)    // Returns: "مرحبا"
     * استخراج("مرحبا بالعالم", 7)       // Returns: "بالعالم"
     * استخراج("مرحبا", 1, 3)           // Returns: "رحب"
     * @endcode
     * 
     * Example (EN):
     * @code
     * substring("Hello World", 0, 5)    // Returns: "Hello"
     * substring("Hello World", 6)       // Returns: "World"
     * substring("Hello", 1, 3)          // Returns: "ell"
     * @endcode
     */
    static Data::Value substring(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) تحويل_صغير - تحويل النص إلى أحرف صغيرة
     * @brief (EN) ToLower - Convert string to lowercase
     * 
     * @param args[0] (AR) النص المراد تحويله / (EN) String to convert
     * @return Value (AR) النص بأحرف صغيرة / (EN) Lowercase string
     * 
     * @note Works with English letters, preserves Arabic text as-is
     * 
     * Example (AR):
     * @code
     * تحويل_صغير("HELLO World")    // Returns: "hello world"
     * تحويل_صغير("مرحبا")          // Returns: "مرحبا" (unchanged)
     * @endcode
     * 
     * Example (EN):
     * @code
     * toLower("HELLO World")        // Returns: "hello world"
     * toLower("مرحبا")              // Returns: "مرحبا" (unchanged)
     * @endcode
     */
    static Data::Value toLower(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) تحويل_كبير - تحويل النص إلى أحرف كبيرة
     * @brief (EN) ToUpper - Convert string to uppercase
     * 
     * @param args[0] (AR) النص المراد تحويله / (EN) String to convert
     * @return Value (AR) النص بأحرف كبيرة / (EN) Uppercase string
     * 
     * @note Works with English letters, preserves Arabic text as-is
     * 
     * Example (AR):
     * @code
     * تحويل_كبير("hello World")    // Returns: "HELLO WORLD"
     * تحويل_كبير("مرحبا")          // Returns: "مرحبا" (unchanged)
     * @endcode
     * 
     * Example (EN):
     * @code
     * toUpper("hello World")        // Returns: "HELLO WORLD"
     * toUpper("مرحبا")              // Returns: "مرحبا" (unchanged)
     * @endcode
     */
    static Data::Value toUpper(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) قص_أطراف - إزالة المسافات من البداية والنهاية
     * @brief (EN) Trim - Remove leading and trailing whitespace
     * 
     * @param args[0] (AR) النص المراد قصه / (EN) String to trim
     * @return Value (AR) النص بعد القص / (EN) Trimmed string
     * 
     * @note Removes spaces, tabs, newlines from both ends
     * 
     * Example (AR):
     * @code
     * قص_أطراف("  مرحبا  ")        // Returns: "مرحبا"
     * قص_أطراف("\n\tمرحبا\t\n")    // Returns: "مرحبا"
     * @endcode
     * 
     * Example (EN):
     * @code
     * trim("  Hello  ")             // Returns: "Hello"
     * trim("\n\tHello\t\n")         // Returns: "Hello"
     * @endcode
     */
    static Data::Value trim(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) تقسيم - تقسيم النص إلى مصفوفة
     * @brief (EN) Split - Split string into array
     * 
     * @param args[0] (AR) النص المراد تقسيمه / (EN) String to split
     * @param args[1] (AR) الفاصل / (EN) Delimiter
     * @param args[2] (AR) الحد الأقصى للتقسيمات (اختياري، -1 = لا حد) / (EN) Max splits (optional, -1 = no limit)
     * @return Value (AR) مصفوفة من النصوص / (EN) Array of strings
     * 
     * Example (AR):
     * @code
     * تقسيم("واحد،اثنان،ثلاثة", "،")     // Returns: ["واحد", "اثنان", "ثلاثة"]
     * تقسيم("a-b-c-d", "-", 2)          // Returns: ["a", "b", "c-d"]
     * @endcode
     * 
     * Example (EN):
     * @code
     * split("one,two,three", ",")       // Returns: ["one", "two", "three"]
     * split("a-b-c-d", "-", 2)          // Returns: ["a", "b", "c-d"]
     * @endcode
     */
    static Data::Value split(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) دمج - دمج عناصر مصفوفة في نص
     * @brief (EN) Join - Join array elements into string
     * 
     * @param args[0] (AR) المصفوفة / (EN) Array
     * @param args[1] (AR) الفاصل (اختياري، افتراضي "") / (EN) Separator (optional, default "")
     * @return Value (AR) النص المدموج / (EN) Joined string
     * 
     * Example (AR):
     * @code
     * دمج(["واحد", "اثنان", "ثلاثة"], "،")    // Returns: "واحد،اثنان،ثلاثة"
     * دمج(["أ", "ب", "ج"])                     // Returns: "أبج"
     * @endcode
     * 
     * Example (EN):
     * @code
     * join(["one", "two", "three"], ",")      // Returns: "one,two,three"
     * join(["a", "b", "c"])                   // Returns: "abc"
     * @endcode
     */
    static Data::Value join(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) يبدأ_ب - التحقق من البداية
     * @brief (EN) StartsWith - Check if string starts with prefix
     * 
     * @param args[0] (AR) النص الأساسي / (EN) Main string
     * @param args[1] (AR) البادئة / (EN) Prefix
     * @return Value (AR) صحيح أو خطأ / (EN) Boolean result
     * 
     * Example (AR):
     * @code
     * يبدأ_ب("مرحبا بالعالم", "مرحبا")    // Returns: true
     * يبدأ_ب("مرحبا", "أهلا")             // Returns: false
     * @endcode
     * 
     * Example (EN):
     * @code
     * startsWith("Hello World", "Hello")   // Returns: true
     * startsWith("Hello", "Hi")            // Returns: false
     * @endcode
     */
    static Data::Value startsWith(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) ينتهي_ب - التحقق من النهاية
     * @brief (EN) EndsWith - Check if string ends with suffix
     * 
     * @param args[0] (AR) النص الأساسي / (EN) Main string
     * @param args[1] (AR) اللاحقة / (EN) Suffix
     * @return Value (AR) صحيح أو خطأ / (EN) Boolean result
     * 
     * Example (AR):
     * @code
     * ينتهي_ب("مرحبا بالعالم", "العالم")    // Returns: true
     * ينتهي_ب("مرحبا", "أهلا")              // Returns: false
     * @endcode
     * 
     * Example (EN):
     * @code
     * endsWith("Hello World", "World")      // Returns: true
     * endsWith("Hello", "Hi")               // Returns: false
     * @endcode
     */
    static Data::Value endsWith(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) يحتوي_على - التحقق من احتواء نص فرعي
     * @brief (EN) Contains - Check if string contains substring
     * 
     * @param args[0] (AR) النص الأساسي / (EN) Main string
     * @param args[1] (AR) النص المراد البحث عنه / (EN) Substring to find
     * @return Value (AR) صحيح أو خطأ / (EN) Boolean result
     * 
     * Example (AR):
     * @code
     * يحتوي_على("مرحبا بالعالم", "العالم")    // Returns: true
     * يحتوي_على("مرحبا", "أهلا")              // Returns: false
     * @endcode
     * 
     * Example (EN):
     * @code
     * contains("Hello World", "World")        // Returns: true
     * contains("Hello", "Goodbye")            // Returns: false
     * @endcode
     */
    static Data::Value contains(const std::vector<Data::Value>& args);

private:
    /**
     * @brief (AR) حساب طول النص بشكل صحيح مع UTF-8
     * @brief (EN) Calculate UTF-8 string length correctly
     * 
     * @param str (AR) النص / (EN) String
     * @return size_t (AR) عدد الأحرف / (EN) Character count
     * 
     * @note Handles multi-byte UTF-8 characters correctly
     */
    static size_t utf8Length(const std::string& str);

    /**
     * @brief (AR) استخراج نص فرعي مع دعم UTF-8
     * @brief (EN) Extract substring with UTF-8 support
     * 
     * @param str (AR) النص / (EN) String
     * @param start (AR) موقع البداية / (EN) Start position
     * @param length (AR) الطول / (EN) Length
     * @return std::string (AR) النص المستخرج / (EN) Extracted string
     */
    static std::string utf8Substring(const std::string& str, size_t start, size_t length);

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
};

} // namespace String
} // namespace StdLib
} // namespace Sad
