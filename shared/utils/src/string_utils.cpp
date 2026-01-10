// ======================================================================
// string_utils.cpp - تنفيذ أدوات النصوص / String Utilities Implementation
// ======================================================================
// الوصف بالعربية:
//   تنفيذ الدوال المساعدة للتعامل مع النصوص والسلاسل الحرفية
//
// English Description:
//   Implementation of helper functions for string manipulation
// ======================================================================

#include "string_utils.h"
#include <iostream>
#include <filesystem>

namespace Sad {
namespace Utils {

// ======================================================================
// تنفيذ دالة trim
// Implementation of trim function
// ======================================================================
/**
 * الوصف: إزالة المسافات من بداية ونهاية النص
 * Description: Remove whitespace from start and end of string
 */
std::string StringUtils::trim(const std::string& str) {
    DEBUG_PRINT("استدعاء دالة trim للنص: \"" + str + "\"");
    
    // البحث عن أول حرف غير مسافة من البداية
    // Find first non-whitespace character from start
    size_t start = 0;
    while (start < str.length() && isWhitespace(str[start])) {
        start++;
    }
    
    // إذا كان النص كله مسافات، إرجاع نص فارغ
    // If entire string is whitespace, return empty string
    if (start == str.length()) {
        DEBUG_PRINT("النص يحتوي على مسافات فقط، إرجاع نص فارغ");
        return "";
    }
    
    // البحث عن آخر حرف غير مسافة من النهاية
    // Find last non-whitespace character from end
    size_t end = str.length() - 1;
    while (end > start && isWhitespace(str[end])) {
        end--;
    }
    
    // استخراج النص من start إلى end
    // Extract substring from start to end
    std::string result = str.substr(start, end - start + 1);
    DEBUG_PRINT("النتيجة بعد trim: \"" + result + "\"");
    
    return result;
}

// ======================================================================
// تنفيذ دالة trimLeft
// Implementation of trimLeft function
// ======================================================================
/**
 * الوصف: إزالة المسافات من بداية النص فقط
 * Description: Remove whitespace from start of string only
 */
std::string StringUtils::trimLeft(const std::string& str) {
    DEBUG_PRINT("استدعاء دالة trimLeft");
    
    size_t start = 0;
    while (start < str.length() && isWhitespace(str[start])) {
        start++;
    }
    
    if (start == str.length()) {
        return "";
    }
    
    return str.substr(start);
}

// ======================================================================
// تنفيذ دالة trimRight
// Implementation of trimRight function
// ======================================================================
/**
 * الوصف: إزالة المسافات من نهاية النص فقط
 * Description: Remove whitespace from end of string only
 */
std::string StringUtils::trimRight(const std::string& str) {
    DEBUG_PRINT("استدعاء دالة trimRight");
    
    if (str.empty()) {
        return "";
    }
    
    size_t end = str.length() - 1;
    while (end > 0 && isWhitespace(str[end])) {
        end--;
    }
    
    if (end == 0 && isWhitespace(str[0])) {
        return "";
    }
    
    return str.substr(0, end + 1);
}

// ======================================================================
// تنفيذ دالة split
// Implementation of split function
// ======================================================================
/**
 * الوصف: تقسيم النص إلى أجزاء باستخدام فاصل
 * Description: Split string into parts using a delimiter
 */
std::vector<std::string> StringUtils::split(const std::string& str, char delimiter) {
    DEBUG_PRINT("استدعاء دالة split بالفاصل: '" + std::string(1, delimiter) + "'");
    
    std::vector<std::string> result;
    
    // إذا كان النص فارغاً، إرجاع قائمة فارغة
    // If string is empty, return empty vector
    if (str.empty()) {
        DEBUG_PRINT("النص فارغ، إرجاع قائمة فارغة");
        return result;
    }
    
    std::stringstream ss(str);
    std::string part;
    
    // قراءة كل جزء مفصول بالفاصل المحدد
    // Read each part separated by the delimiter
    while (std::getline(ss, part, delimiter)) {
        result.push_back(part);
    }
    
    DEBUG_PRINT("تم تقسيم النص إلى " + std::to_string(result.size()) + " جزء");
    
    return result;
}

// ======================================================================
// تنفيذ دالة startsWith
// Implementation of startsWith function
// ======================================================================
/**
 * الوصف: التحقق من أن النص يبدأ بنص معين
 * Description: Check if string starts with a specific prefix
 */
bool StringUtils::startsWith(const std::string& str, const std::string& prefix) {
    DEBUG_PRINT("فحص إذا كان النص يبدأ بـ: \"" + prefix + "\"");
    
    // إذا كان البادئة أطول من النص، فبالتأكيد لا يبدأ به
    // If prefix is longer than string, it can't start with it
    if (prefix.length() > str.length()) {
        DEBUG_PRINT("البادئة أطول من النص، النتيجة: false");
        return false;
    }
    
    // مقارنة أول n حرف من النص مع البادئة
    // Compare first n characters of string with prefix
    bool result = str.compare(0, prefix.length(), prefix) == 0;
    DEBUG_PRINT("النتيجة: " + std::string(result ? "true" : "false"));
    
    return result;
}

// ======================================================================
// تنفيذ دالة endsWith
// Implementation of endsWith function
// ======================================================================
/**
 * الوصف: التحقق من أن النص ينتهي بنص معين
 * Description: Check if string ends with a specific suffix
 */
bool StringUtils::endsWith(const std::string& str, const std::string& suffix) {
    DEBUG_PRINT("فحص إذا كان النص ينتهي بـ: \"" + suffix + "\"");
    
    // إذا كانت اللاحقة أطول من النص، فبالتأكيد لا ينتهي بها
    // If suffix is longer than string, it can't end with it
    if (suffix.length() > str.length()) {
        DEBUG_PRINT("اللاحقة أطول من النص، النتيجة: false");
        return false;
    }
    
    // مقارنة آخر n حرف من النص مع اللاحقة
    // Compare last n characters of string with suffix
    bool result = str.compare(str.length() - suffix.length(), 
                             suffix.length(), 
                             suffix) == 0;
    DEBUG_PRINT("النتيجة: " + std::string(result ? "true" : "false"));
    
    return result;
}

// ======================================================================
// تنفيذ دالة toLower
// Implementation of toLower function
// ======================================================================
/**
 * الوصف: تحويل الأحرف اللاتينية إلى أحرف صغيرة
 * Description: Convert Latin characters to lowercase
 */
std::string StringUtils::toLower(const std::string& str) {
    DEBUG_PRINT("تحويل النص إلى أحرف صغيرة");
    
    std::string result = str;
    
    // تحويل كل حرف إلى صغير
    // Convert each character to lowercase
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    
    return result;
}

// ======================================================================
// تنفيذ دالة toUpper
// Implementation of toUpper function
// ======================================================================
/**
 * الوصف: تحويل الأحرف اللاتينية إلى أحرف كبيرة
 * Description: Convert Latin characters to uppercase
 */
std::string StringUtils::toUpper(const std::string& str) {
    DEBUG_PRINT("تحويل النص إلى أحرف كبيرة");
    
    std::string result = str;
    
    // تحويل كل حرف إلى كبير
    // Convert each character to uppercase
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::toupper(c); });
    
    return result;
}

// ======================================================================
// تنفيذ دالة replace
// Implementation of replace function
// ======================================================================
/**
 * الوصف: استبدال جميع حالات ظهور نص بنص آخر
 * Description: Replace all occurrences of a substring with another
 */
std::string StringUtils::replace(const std::string& str, 
                                const std::string& from, 
                                const std::string& to) {
    DEBUG_PRINT("استبدال \"" + from + "\" بـ \"" + to + "\"");
    
    // إذا كان النص المراد استبداله فارغاً، إرجاع النص الأصلي
    // If substring to replace is empty, return original string
    if (from.empty()) {
        DEBUG_PRINT("النص المراد استبداله فارغ، إرجاع النص الأصلي");
        return str;
    }
    
    std::string result = str;
    size_t pos = 0;
    int count = 0;
    
    // البحث عن كل حالة واستبدالها
    // Find and replace each occurrence
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
        count++;
    }
    
    DEBUG_PRINT("تم استبدال " + std::to_string(count) + " حالة");
    
    return result;
}

// ======================================================================
// تنفيذ دالة isBlank
// Implementation of isBlank function
// ======================================================================
/**
 * الوصف: التحقق من أن النص فارغ أو مسافات فقط
 * Description: Check if string is empty or whitespace only
 */
bool StringUtils::isBlank(const std::string& str) {
    DEBUG_PRINT("فحص إذا كان النص فارغاً أو مسافات فقط");
    
    // إذا كان النص فارغاً
    // If string is empty
    if (str.empty()) {
        return true;
    }
    
    // فحص كل حرف
    // Check each character
    for (char c : str) {
        if (!isWhitespace(c)) {
            return false;
        }
    }
    
    return true;
}

// ======================================================================
// تنفيذ دالة join
// Implementation of join function
// ======================================================================
/**
 * الوصف: دمج قائمة من النصوص بفاصل محدد
 * Description: Join a list of strings with a delimiter
 */
std::string StringUtils::join(const std::vector<std::string>& parts, 
                             const std::string& delimiter) {
    DEBUG_PRINT("دمج " + std::to_string(parts.size()) + " جزء بالفاصل: \"" + delimiter + "\"");
    
    // إذا كانت القائمة فارغة
    // If vector is empty
    if (parts.empty()) {
        DEBUG_PRINT("القائمة فارغة، إرجاع نص فارغ");
        return "";
    }
    
    std::string result;
    
    // إضافة أول عنصر
    // Add first element
    result += parts[0];
    
    // إضافة باقي العناصر مع الفاصل
    // Add remaining elements with delimiter
    for (size_t i = 1; i < parts.size(); i++) {
        result += delimiter;
        result += parts[i];
    }
    
    DEBUG_PRINT("النتيجة: \"" + result + "\"");
    
    return result;
}

// ======================================================================
// تنفيذ دالة debugPrint
// Implementation of debugPrint function
// ======================================================================
/**
 * الوصف: طباعة رسالة تنقيح مع معلومات إضافية
 * Description: Print debug message with additional info
 */
void StringUtils::debugPrint(const std::string& message, 
                            const char* file, 
                            int line) {
#ifdef SAD_DEBUG_ENABLED
    // استخراج اسم الملف فقط (بدون المسار الكامل)
    // Extract filename only (without full path)
    std::filesystem::path filePath(file);
    std::string filename = filePath.filename().string();
    
    // طباعة الرسالة بالتنسيق: [اسم_الملف:رقم_السطر] الرسالة
    // Print message in format: [filename:line] message
    std::cout << "[" << filename << ":" << line << "] " << message << std::endl;
#else
    // في حالة عدم تفعيل التنقيح، لا نفعل شيء
    // If debug is not enabled, do nothing
    (void)message;
    (void)file;
    (void)line;
#endif
}

// ======================================================================
// تنفيذ دالة isWhitespace المساعدة
// Implementation of helper isWhitespace function
// ======================================================================
/**
 * الوصف: التحقق من أن الحرف مسافة
 * Description: Check if character is whitespace
 */
bool StringUtils::isWhitespace(char c) {
    // فحص المسافة، التاب، السطر الجديد، والإرجاع
    // Check for space, tab, newline, and carriage return
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

// ======================================================================
// تنفيذ دوال دعم الأرقام العربية
// Implementation of Arabic Digit Support Functions
// ======================================================================

/**
 * الوصف: التحقق من أن الحرف رقم عربي-هندي (٠-٩)
 * Description: Check if character is an Arabic-Indic digit (٠-٩)
 * 
 * ملاحظة: الأرقام العربية في UTF-8:
 *   ٠ = 0xD9 0xA0 (U+0660)
 *   ١ = 0xD9 0xA1 (U+0661)
 *   ...
 *   ٩ = 0xD9 0xA9 (U+0669)
 */
bool StringUtils::isArabicDigit(unsigned char c) {
    // في UTF-8، الأرقام العربية تبدأ بـ 0xD9
    // البايت الثاني يكون من 0xA0 إلى 0xA9
    // In UTF-8, Arabic digits start with 0xD9
    // Second byte ranges from 0xA0 to 0xA9
    return (c >= 0xA0 && c <= 0xA9);
}

/**
 * الوصف: التحقق من أن الحرف رقم (عربي أو إنجليزي)
 * Description: Check if character is a digit (Arabic or English)
 */
bool StringUtils::isDigit(unsigned char c) {
    // فحص الأرقام الإنجليزية (0-9)
    if (c >= '0' && c <= '9') {
        return true;
    }
    
    // فحص الأرقام العربية (٠-٩)
    return isArabicDigit(c);
}

/**
 * الوصف: تحويل رقم عربي إلى رقم إنجليزي
 * Description: Convert Arabic digit to English digit
 * 
 * @example ٠ -> '0', ٥ -> '5', ٩ -> '9'
 */
char StringUtils::arabicDigitToEnglish(unsigned char c) {
    // التحقق من أن الحرف رقم عربي
    if (!isArabicDigit(c)) {
        return c; // إرجاع الحرف كما هو إذا لم يكن رقماً عربياً
    }
    
    // الأرقام العربية تبدأ من 0xA0 (٠) إلى 0xA9 (٩)
    // نطرح 0xA0 ونضيف '0' للحصول على الرقم الإنجليزي
    // Arabic digits start from 0xA0 (٠) to 0xA9 (٩)
    // Subtract 0xA0 and add '0' to get English digit
    return '0' + (c - 0xA0);
}

/**
 * الوصف: تحويل نص يحتوي على أرقام عربية إلى أرقام إنجليزية
 * Description: Convert string with Arabic digits to English digits
 */
std::string StringUtils::convertArabicDigitsToEnglish(const std::string& str) {
    DEBUG_PRINT("تحويل الأرقام العربية في النص: " + str);
    
    std::string result;
    result.reserve(str.length());
    
    for (size_t i = 0; i < str.length(); ++i) {
        unsigned char c = static_cast<unsigned char>(str[i]);
        
        // فحص إذا كان هذا بداية رقم عربي UTF-8 (0xD9)
        // Check if this is the start of an Arabic digit UTF-8 sequence (0xD9)
        if (c == 0xD9 && i + 1 < str.length()) {
            unsigned char next = static_cast<unsigned char>(str[i + 1]);
            
            // فحص إذا كان البايت التالي رقم عربي (0xA0-0xA9)
            if (isArabicDigit(next)) {
                // تحويل إلى رقم إنجليزي
                char englishDigit = arabicDigitToEnglish(next);
                result += englishDigit;
                
                // تخطي البايت التالي (تم معالجته)
                ++i;
                
                DEBUG_PRINT(std::string("تم تحويل رقم عربي إلى: ") + englishDigit);
                continue;
            }
        }
        
        // إذا لم يكن رقماً عربياً، أضف الحرف كما هو
        result += str[i];
    }
    
    DEBUG_PRINT("النتيجة بعد التحويل: " + result);
    return result;
}

} // namespace Utils
} // namespace Sad
