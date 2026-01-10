// ======================================================================
// string_utils.h - أدوات النصوص / String Utilities
// ======================================================================
// الوصف بالعربية:
//   مجموعة من الدوال المساعدة للتعامل مع النصوص والسلاسل الحرفية
//   تتضمن عمليات مثل القص، التقسيم، البحث، والتحويل
//
// English Description:
//   Collection of helper functions for string and text manipulation
//   Includes operations like trimming, splitting, searching, and conversion
//
// الملاحظات / Notes:
//   - جميع الدوال ثابتة (static) ولا تحتاج إلى إنشاء كائن
//     All functions are static and don't require object instantiation
//   - تدعم UTF-8 للنصوص العربية
//     Supports UTF-8 for Arabic text
// ======================================================================

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace Sad {
namespace Utils {

// ======================================================================
// فئة أدوات النصوص / StringUtils Class
// ======================================================================
/**
 * @class StringUtils
 * @brief فئة تحتوي على دوال مساعدة للتعامل مع النصوص
 *        Class containing helper functions for string manipulation
 */
class StringUtils {
public:
    // ==================================================================
    // دالة إزالة المسافات من البداية والنهاية
    // Trim whitespace from start and end
    // ==================================================================
    /**
     * @brief إزالة المسافات والأسطر الفارغة من بداية ونهاية النص
     *        Remove whitespace and empty lines from start and end of string
     * 
     * @param str النص المراد معالجته / String to process
     * @return std::string النص بعد إزالة المسافات / Trimmed string
     * 
     * @example
     *   std::string text = "  مرحبا  ";
     *   std::string result = StringUtils::trim(text);
     *   // result = "مرحبا"
     * 
     * @note تزيل المسافات، التابات، والأسطر الجديدة
     *       Removes spaces, tabs, and newlines
     */
    static std::string trim(const std::string& str);
    
    // ==================================================================
    // دالة إزالة المسافات من البداية فقط
    // Trim whitespace from start only
    // ==================================================================
    /**
     * @brief إزالة المسافات من بداية النص فقط
     *        Remove whitespace from start of string only
     * 
     * @param str النص المراد معالجته / String to process
     * @return std::string النص بعد إزالة المسافات من البداية / Left-trimmed string
     * 
     * @example
     *   std::string text = "  مرحبا  ";
     *   std::string result = StringUtils::trimLeft(text);
     *   // result = "مرحبا  "
     */
    static std::string trimLeft(const std::string& str);
    
    // ==================================================================
    // دالة إزالة المسافات من النهاية فقط
    // Trim whitespace from end only
    // ==================================================================
    /**
     * @brief إزالة المسافات من نهاية النص فقط
     *        Remove whitespace from end of string only
     * 
     * @param str النص المراد معالجته / String to process
     * @return std::string النص بعد إزالة المسافات من النهاية / Right-trimmed string
     * 
     * @example
     *   std::string text = "  مرحبا  ";
     *   std::string result = StringUtils::trimRight(text);
     *   // result = "  مرحبا"
     */
    static std::string trimRight(const std::string& str);
    
    // ==================================================================
    // دالة تقسيم النص
    // Split string
    // ==================================================================
    /**
     * @brief تقسيم النص إلى أجزاء باستخدام فاصل محدد
     *        Split string into parts using a delimiter
     * 
     * @param str النص المراد تقسيمه / String to split
     * @param delimiter الفاصل المستخدم للتقسيم / Delimiter character
     * @return std::vector<std::string> قائمة الأجزاء / List of parts
     * 
     * @example
     *   std::string text = "أحمد،محمد،علي";
     *   auto parts = StringUtils::split(text, '،');
     *   // parts = ["أحمد", "محمد", "علي"]
     * 
     * @note إذا كان النص فارغاً، ترجع قائمة فارغة
     *       If string is empty, returns empty vector
     */
    static std::vector<std::string> split(const std::string& str, char delimiter);
    
    // ==================================================================
    // دالة التحقق من بداية النص
    // Check if string starts with prefix
    // ==================================================================
    /**
     * @brief التحقق من أن النص يبدأ بنص معين
     *        Check if string starts with a specific prefix
     * 
     * @param str النص المراد فحصه / String to check
     * @param prefix النص المراد البحث عنه في البداية / Prefix to search for
     * @return bool true إذا كان النص يبدأ بالبادئة / true if starts with prefix
     * 
     * @example
     *   std::string text = "مرحبا بالعالم";
     *   bool result = StringUtils::startsWith(text, "مرحبا");
     *   // result = true
     * 
     * @note حساسة لحالة الأحرف / Case-sensitive
     */
    static bool startsWith(const std::string& str, const std::string& prefix);
    
    // ==================================================================
    // دالة التحقق من نهاية النص
    // Check if string ends with suffix
    // ==================================================================
    /**
     * @brief التحقق من أن النص ينتهي بنص معين
     *        Check if string ends with a specific suffix
     * 
     * @param str النص المراد فحصه / String to check
     * @param suffix النص المراد البحث عنه في النهاية / Suffix to search for
     * @return bool true إذا كان النص ينتهي باللاحقة / true if ends with suffix
     * 
     * @example
     *   std::string filename = "برنامج.s";
     *   bool result = StringUtils::endsWith(filename, ".s");
     *   // result = true
     * 
     * @note حساسة لحالة الأحرف / Case-sensitive
     */
    static bool endsWith(const std::string& str, const std::string& suffix);
    
    // ==================================================================
    // دالة التحويل إلى أحرف صغيرة
    // Convert to lowercase
    // ==================================================================
    /**
     * @brief تحويل جميع الأحرف اللاتينية إلى أحرف صغيرة
     *        Convert all Latin characters to lowercase
     * 
     * @param str النص المراد تحويله / String to convert
     * @return std::string النص بأحرف صغيرة / Lowercase string
     * 
     * @example
     *   std::string text = "Hello";
     *   std::string result = StringUtils::toLower(text);
     *   // result = "hello"
     * 
     * @note لا تؤثر على الأحرف العربية
     *       Does not affect Arabic characters
     */
    static std::string toLower(const std::string& str);
    
    // ==================================================================
    // دالة التحويل إلى أحرف كبيرة
    // Convert to uppercase
    // ==================================================================
    /**
     * @brief تحويل جميع الأحرف اللاتينية إلى أحرف كبيرة
     *        Convert all Latin characters to uppercase
     * 
     * @param str النص المراد تحويله / String to convert
     * @return std::string النص بأحرف كبيرة / Uppercase string
     * 
     * @example
     *   std::string text = "hello";
     *   std::string result = StringUtils::toUpper(text);
     *   // result = "HELLO"
     * 
     * @note لا تؤثر على الأحرف العربية
     *       Does not affect Arabic characters
     */
    static std::string toUpper(const std::string& str);
    
    // ==================================================================
    // دالة استبدال نص بآخر
    // Replace substring
    // ==================================================================
    /**
     * @brief استبدال جميع حالات ظهور نص بنص آخر
     *        Replace all occurrences of a substring with another
     * 
     * @param str النص الأصلي / Original string
     * @param from النص المراد استبداله / Substring to replace
     * @param to النص البديل / Replacement substring
     * @return std::string النص بعد الاستبدال / String after replacement
     * 
     * @example
     *   std::string text = "مرحبا مرحبا";
     *   std::string result = StringUtils::replace(text, "مرحبا", "أهلا");
     *   // result = "أهلا أهلا"
     * 
     * @note يستبدل جميع الحالات وليس الأولى فقط
     *       Replaces all occurrences, not just the first
     */
    static std::string replace(const std::string& str, 
                               const std::string& from, 
                               const std::string& to);
    
    // ==================================================================
    // دالة التحقق من نص فارغ أو مسافات فقط
    // Check if string is empty or whitespace only
    // ==================================================================
    /**
     * @brief التحقق من أن النص فارغ أو يحتوي على مسافات فقط
     *        Check if string is empty or contains only whitespace
     * 
     * @param str النص المراد فحصه / String to check
     * @return bool true إذا كان فارغاً أو مسافات فقط / true if empty or whitespace only
     * 
     * @example
     *   bool r1 = StringUtils::isBlank("");        // true
     *   bool r2 = StringUtils::isBlank("   ");     // true
     *   bool r3 = StringUtils::isBlank("نص");      // false
     */
    static bool isBlank(const std::string& str);
    
    // ==================================================================
    // دالة الدمج
    // Join strings
    // ==================================================================
    /**
     * @brief دمج قائمة من النصوص بفاصل محدد
     *        Join a list of strings with a delimiter
     * 
     * @param parts قائمة النصوص المراد دمجها / List of strings to join
     * @param delimiter الفاصل بين النصوص / Delimiter between strings
     * @return std::string النص المدمج / Joined string
     * 
     * @example
     *   std::vector<std::string> parts = {"أحمد", "محمد", "علي"};
     *   std::string result = StringUtils::join(parts, "، ");
     *   // result = "أحمد، محمد، علي"
     */
    static std::string join(const std::vector<std::string>& parts, 
                           const std::string& delimiter);
    
    // ==================================================================
    // دالة طباعة التنقيح
    // Debug print function
    // ==================================================================
    /**
     * @brief طباعة رسالة تنقيح مع معلومات إضافية (اسم الملف ورقم السطر)
     *        Print debug message with additional info (filename and line number)
     * 
     * @param message الرسالة المراد طباعتها / Message to print
     * @param file اسم الملف (يتم توفيره تلقائياً بواسطة __FILE__)
     *             File name (automatically provided by __FILE__)
     * @param line رقم السطر (يتم توفيره تلقائياً بواسطة __LINE__)
     *             Line number (automatically provided by __LINE__)
     * 
     * @example
     *   debugPrint("بدء التحليل المعجمي");
     *   // Output: [lexer.cpp:42] بدء التحليل المعجمي
     * 
     * @note يتم تفعيلها فقط عند تعريف SAD_DEBUG_ENABLED
     *       Only active when SAD_DEBUG_ENABLED is defined
     */
    static void debugPrint(const std::string& message, 
                          const char* file = __builtin_FILE(), 
                          int line = __builtin_LINE());

    // ==================================================================
    // دوال دعم الأرقام العربية / Arabic Digit Support Functions
    // ==================================================================
    /**
     * @brief التحقق من أن الحرف رقم عربي-هندي (٠-٩)
     *        Check if character is an Arabic-Indic digit (٠-٩)
     * 
     * @param c الحرف المراد فحصه / Character to check
     * @return bool true إذا كان رقماً عربياً / true if Arabic digit
     * 
     * @note الأرقام العربية UTF-8: ٠ = U+0660 to ٩ = U+0669
     *       Arabic digits UTF-8: ٠ = U+0660 to ٩ = U+0669
     */
    static bool isArabicDigit(unsigned char c);
    
    /**
     * @brief التحقق من أن الحرف رقم (عربي أو إنجليزي)
     *        Check if character is a digit (Arabic or English)
     * 
     * @param c الحرف المراد فحصه / Character to check
     * @return bool true إذا كان رقماً / true if digit
     */
    static bool isDigit(unsigned char c);
    
    /**
     * @brief تحويل رقم عربي إلى رقم إنجليزي
     *        Convert Arabic digit to English digit
     * 
     * @param c الحرف المراد تحويله / Character to convert
     * @return char الرقم الإنجليزي المقابل / Corresponding English digit
     * 
     * @example
     *   char result = arabicDigitToEnglish('٥');
     *   // result = '5'
     */
    static char arabicDigitToEnglish(unsigned char c);
    
    /**
     * @brief تحويل نص يحتوي على أرقام عربية إلى أرقام إنجليزية
     *        Convert string with Arabic digits to English digits
     * 
     * @param str النص المراد تحويله / String to convert
     * @return std::string النص بأرقام إنجليزية / String with English digits
     * 
     * @example
     *   std::string result = convertArabicDigitsToEnglish("العدد ٥٣");
     *   // result = "العدد 53"
     */
    static std::string convertArabicDigitsToEnglish(const std::string& str);

private:
    // ==================================================================
    // دالة مساعدة للتحقق من المسافات
    // Helper function to check whitespace
    // ==================================================================
    /**
     * @brief التحقق من أن الحرف مسافة أو تاب أو سطر جديد
     *        Check if character is space, tab, or newline
     * 
     * @param c الحرف المراد فحصه / Character to check
     * @return bool true إذا كان مسافة / true if whitespace
     */
    static bool isWhitespace(char c);
};

// ======================================================================
// ماكرو مساعد لطباعة التنقيح
// Helper macro for debug printing
// ======================================================================
/**
 * @brief ماكرو لتسهيل استدعاء دالة debugPrint
 *        Macro to simplify calling debugPrint function
 * 
 * @example
 *   DEBUG_PRINT("معالجة الرمز: " + token.getValue());
 */
#ifdef SAD_DEBUG_ENABLED
    #define DEBUG_PRINT(msg) Sad::Utils::StringUtils::debugPrint(msg, __FILE__, __LINE__)
#else
    #define DEBUG_PRINT(msg) ((void)0)
#endif

} // namespace Utils
} // namespace Sad
