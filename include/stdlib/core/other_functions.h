/**
 * @file other_functions.h
 * @brief دوال المكتبة القياسية الإضافية - Other Standard Library Functions
 * 
 * =============================================================================
 * 📦 نظرة عامة / Overview
 * =============================================================================
 * 
 * العربية:
 * --------
 * هذا الملف يحتوي على الدوال الإضافية للمكتبة القياسية للغة ص (Sad Language).
 * يشمل دوال للإدخال، التوقيت، التحكم في البرنامج، والتحقق من الشروط.
 * 
 * الدوال المتوفرة:
 * - input()   : قراءة مدخل من المستخدم
 * - random()  : توليد رقم عشوائي
 * - sleep()   : إيقاف التنفيذ مؤقتاً
 * - exit()    : إنهاء البرنامج
 * - assert()  : التحقق من شرط
 * 
 * English:
 * --------
 * This file contains additional standard library functions for S Language.
 * It includes functions for input, timing, program control, and assertions.
 * 
 * Available functions:
 * - input()   : Read input from user
 * - random()  : Generate random number
 * - sleep()   : Pause execution temporarily
 * - exit()    : Terminate program
 * - assert()  : Assert a condition
 * 
 * =============================================================================
 * 🎯 الاستخدام / Usage
 * =============================================================================
 * 
 * مثال 1 - قراءة مدخل / Example 1 - Reading Input:
 * @code
 * # العربية
 * الاسم = input("ما اسمك؟ ")
 * اطبع("مرحباً " + الاسم)
 * 
 * # English
 * name = input("What's your name? ")
 * print("Hello " + name)
 * @endcode
 * 
 * مثال 2 - رقم عشوائي / Example 2 - Random Number:
 * @code
 * # العربية
 * رقم = random(1, 10)        # رقم بين 1 و 10
 * اطبع("الرقم العشوائي: " + toString(رقم))
 * 
 * # English
 * num = random(1, 10)         # number between 1 and 10
 * print("Random number: " + toString(num))
 * @endcode
 * 
 * مثال 3 - التوقيت / Example 3 - Sleep:
 * @code
 * # العربية
 * اطبع("انتظر 2 ثانية...")
 * sleep(2000)                # 2000 ميلي ثانية
 * اطبع("انتهى الانتظار!")
 * 
 * # English
 * print("Wait 2 seconds...")
 * sleep(2000)                # 2000 milliseconds
 * print("Done waiting!")
 * @endcode
 * 
 * مثال 4 - التحقق / Example 4 - Assert:
 * @code
 * # العربية
 * العمر = 25
 * assert(العمر > 0, "العمر يجب أن يكون موجباً")
 * 
 * # English
 * age = 25
 * assert(age > 0, "Age must be positive")
 * @endcode
 * 
 * =============================================================================
 * 
 * @author S Language Development Team
 * @date December 2024
 * @version 1.0.0
 */

#ifndef SAD_STDLIB_OTHER_FUNCTIONS_H
#define SAD_STDLIB_OTHER_FUNCTIONS_H

#include "data/types/value.h"
#include <vector>
#include <string>

namespace Sad {
namespace StdLib {
namespace Core {

using Data::Value;
using Data::ValueType;

/**
 * @brief قراءة مدخل من المستخدم / Read input from user
 * 
 * العربية:
 * --------
 * تقرأ سطراً كاملاً من المستخدم عبر stdin.
 * يمكن تحديد رسالة محث اختيارية.
 * 
 * المعاملات:
 * - prompt (اختياري): رسالة المحث التي تُعرض قبل القراءة
 * 
 * الإرجاع:
 * - نص (string): السطر المقروء من المستخدم
 * - نص فارغ إذا فشلت القراءة
 * 
 * English:
 * --------
 * Reads a complete line from the user via stdin.
 * Can specify an optional prompt message.
 * 
 * Parameters:
 * - prompt (optional): Prompt message displayed before reading
 * 
 * Returns:
 * - string: Line read from user
 * - Empty string if read fails
 * 
 * @param args Vector of arguments [prompt (optional)]
 * @return Value String value containing user input
 * 
 * @code
 * # استخدام بدون محث / Use without prompt
 * نص = input()
 * 
 * # استخدام مع محث / Use with prompt
 * الاسم = input("أدخل اسمك: ")
 * name = input("Enter your name: ")
 * @endcode
 * 
 * @note يقرأ حتى نهاية السطر (حتى \n)
 * @note Reads until end of line (until \n)
 */
Value input(const std::vector<Value>& args);

/**
 * @brief توليد رقم عشوائي / Generate random number
 * 
 * العربية:
 * --------
 * يولد رقماً عشوائياً ضمن نطاق محدد.
 * يدعم ثلاث صيغ مختلفة للاستخدام.
 * 
 * الصيغ:
 * 1. random()         : رقم بين 0 و RAND_MAX
 * 2. random(max)      : رقم بين 0 و max-1
 * 3. random(min, max) : رقم بين min و max-1
 * 
 * المعاملات:
 * - min (اختياري): الحد الأدنى (شامل)
 * - max (اختياري): الحد الأقصى (غير شامل)
 * 
 * الإرجاع:
 * - عدد صحيح (integer): رقم عشوائي ضمن النطاق
 * 
 * English:
 * --------
 * Generates a random number within a specified range.
 * Supports three different usage formats.
 * 
 * Formats:
 * 1. random()         : number between 0 and RAND_MAX
 * 2. random(max)      : number between 0 and max-1
 * 3. random(min, max) : number between min and max-1
 * 
 * Parameters:
 * - min (optional): Minimum value (inclusive)
 * - max (optional): Maximum value (exclusive)
 * 
 * Returns:
 * - integer: Random number within range
 * 
 * @param args Vector of arguments [min (optional), max (optional)]
 * @return Value Integer value containing random number
 * 
 * @code
 * # أمثلة / Examples
 * رقم1 = random()              # 0 إلى RAND_MAX
 * رقم2 = random(10)            # 0 إلى 9
 * رقم3 = random(5, 15)         # 5 إلى 14
 * 
 * num1 = random()              # 0 to RAND_MAX
 * num2 = random(10)            # 0 to 9
 * num3 = random(5, 15)         # 5 to 14
 * @endcode
 * 
 * @note يستخدم rand() من <cstdlib>
 * @note Uses rand() from <cstdlib>
 * @note النطاق الأقصى غير شامل (exclusive)
 * @note Maximum value is exclusive
 */
Value random(const std::vector<Value>& args);

/**
 * @brief إيقاف التنفيذ مؤقتاً / Pause execution temporarily
 * 
 * العربية:
 * --------
 * يوقف تنفيذ البرنامج لمدة محددة بالميلي ثانية.
 * مفيد للتحكم في توقيت البرنامج والانتظار بين العمليات.
 * 
 * المعاملات:
 * - milliseconds: عدد الميلي ثانية للانتظار (1000 ميلي ثانية = 1 ثانية)
 * 
 * الإرجاع:
 * - void: لا يُرجع قيمة
 * 
 * English:
 * --------
 * Pauses program execution for a specified duration in milliseconds.
 * Useful for timing control and waiting between operations.
 * 
 * Parameters:
 * - milliseconds: Number of milliseconds to wait (1000 ms = 1 second)
 * 
 * Returns:
 * - void: Returns no value
 * 
 * @param args Vector of arguments [milliseconds]
 * @return Value Void value
 * 
 * @code
 * # العربية
 * اطبع("بداية")
 * sleep(1000)              # انتظر ثانية واحدة
 * اطبع("بعد ثانية")
 * sleep(2500)              # انتظر 2.5 ثانية
 * اطبع("نهاية")
 * 
 * # English
 * print("Start")
 * sleep(1000)              # Wait 1 second
 * print("After 1 second")
 * sleep(2500)              # Wait 2.5 seconds
 * print("End")
 * @endcode
 * 
 * @note يستخدم std::this_thread::sleep_for
 * @note Uses std::this_thread::sleep_for
 * @warning قد يؤثر على أداء البرنامج في حالة الاستخدام المفرط
 * @warning May affect program performance if used excessively
 */
Value sleep(const std::vector<Value>& args);

/**
 * @brief إنهاء البرنامج / Terminate program
 * 
 * العربية:
 * --------
 * ينهي تنفيذ البرنامج فوراً مع كود خروج محدد.
 * يمكن استخدامه للخروج من البرنامج في حالات معينة.
 * 
 * المعاملات:
 * - exit_code (اختياري): كود الخروج (افتراضي: 0)
 *   * 0 = خروج عادي (نجاح)
 *   * غير 0 = خروج مع خطأ
 * 
 * الإرجاع:
 * - void: لا يُرجع (ينهي البرنامج)
 * 
 * English:
 * --------
 * Terminates program execution immediately with specified exit code.
 * Can be used to exit program under certain conditions.
 * 
 * Parameters:
 * - exit_code (optional): Exit code (default: 0)
 *   * 0 = Normal exit (success)
 *   * Non-zero = Exit with error
 * 
 * Returns:
 * - void: Does not return (terminates program)
 * 
 * @param args Vector of arguments [exit_code (optional)]
 * @return Value Void value (but program terminates)
 * 
 * @code
 * # العربية
 * اذا (خطأ) {
 *     اطبع("حدث خطأ!")
 *     exit(1)              # خروج مع كود خطأ
 * }
 * exit()                   # خروج عادي
 * 
 * # English
 * if (error) {
 *     print("Error occurred!")
 *     exit(1)              # Exit with error code
 * }
 * exit()                   # Normal exit
 * @endcode
 * 
 * @note يستخدم std::exit()
 * @note Uses std::exit()
 * @warning ينهي البرنامج فوراً بدون تنظيف
 * @warning Terminates program immediately without cleanup
 */
Value exit(const std::vector<Value>& args);

/**
 * @brief التحقق من شرط / Assert a condition
 * 
 * العربية:
 * --------
 * يتحقق من صحة شرط معين. إذا كان الشرط خاطئاً، يطبع رسالة خطأ ويوقف البرنامج.
 * مفيد جداً في تطوير البرامج للتأكد من الشروط المتوقعة.
 * 
 * المعاملات:
 * - condition: الشرط للتحقق منه (boolean)
 * - message (اختياري): رسالة الخطأ عند فشل الشرط
 * 
 * الإرجاع:
 * - void: لا يُرجع قيمة إذا نجح الشرط
 * - يوقف البرنامج إذا فشل الشرط
 * 
 * English:
 * --------
 * Verifies that a condition is true. If false, prints error message and stops program.
 * Very useful in development to ensure expected conditions.
 * 
 * Parameters:
 * - condition: Condition to verify (boolean)
 * - message (optional): Error message if condition fails
 * 
 * Returns:
 * - void: Returns nothing if condition succeeds
 * - Stops program if condition fails
 * 
 * @param args Vector of arguments [condition, message (optional)]
 * @return Value Void value if assertion passes
 * 
 * @code
 * # العربية
 * العمر = 25
 * assert(العمر > 0, "العمر يجب أن يكون موجباً")
 * assert(العمر < 150, "العمر غير منطقي")
 * 
 * قائمة = [1, 2, 3]
 * assert(length(قائمة) > 0, "القائمة فارغة")
 * 
 * # English
 * age = 25
 * assert(age > 0, "Age must be positive")
 * assert(age < 150, "Age is unrealistic")
 * 
 * list = [1, 2, 3]
 * assert(length(list) > 0, "List is empty")
 * @endcode
 * 
 * @note مفيد للتصحيح والتطوير
 * @note Useful for debugging and development
 * @warning يوقف البرنامج عند فشل التحقق
 * @warning Stops program when assertion fails
 */
Value assert(const std::vector<Value>& args);

} // namespace Core
} // namespace StdLib
} // namespace Sad

#endif // SAD_STDLIB_OTHER_FUNCTIONS_H
