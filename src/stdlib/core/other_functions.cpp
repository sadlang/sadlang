/**
 * @file other_functions.cpp
 * @brief تنفيذ دوال المكتبة القياسية الإضافية / Implementation of Other Standard Library Functions
 * 
 * @author S Language Development Team
 * @date December 2024
 */

#include "../../../include/stdlib/core/other_functions.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

namespace Sad {
namespace StdLib {
namespace Core {

using Data::Value;
using Data::ValueType;

/**
 * @brief دالة مساعدة للتحقق من عدد المعاملات
 * Helper function to validate argument count
 */
static bool validateArgCount(const std::vector<Value>& args, size_t min, size_t max, const std::string& funcName) {
    if (args.size() < min || args.size() > max) {
        std::cerr << "خطأ / Error: " << funcName << " يتوقع / expects " << min;
        if (min != max) {
            std::cerr << "-" << max;
        }
        std::cerr << " معامل / argument(s), حصل على / got " << args.size() << std::endl;
        return false;
    }
    return true;
}

// =============================================================================
// input() - قراءة مدخل من المستخدم / Read input from user
// =============================================================================

Value input(const std::vector<Value>& args) {
    // التحقق من عدد المعاملات (0 أو 1)
    // Validate argument count (0 or 1)
    if (!validateArgCount(args, 0, 1, "input")) {
        return Value(""); // إرجاع نص فارغ عند الخطأ / Return empty string on error
    }
    
    // إذا كان هناك محث، اطبعه
    // If there's a prompt, print it
    if (args.size() == 1) {
        std::string prompt = args[0].toString();
        std::cout << prompt;
        std::cout.flush(); // تأكد من طباعة المحث فوراً / Ensure prompt is printed immediately
    }
    
    // قراءة سطر من المستخدم
    // Read line from user
    std::string line;
    if (std::getline(std::cin, line)) {
        return Value(line);
    }
    
    // إذا فشلت القراءة، أرجع نص فارغ
    // If read fails, return empty string
    return Value("");
}

// =============================================================================
// random() - توليد رقم عشوائي / Generate random number
// =============================================================================

Value random(const std::vector<Value>& args) {
    // تهيئة مولد الأرقام العشوائية مرة واحدة
    // Initialize random number generator once
    static bool initialized = false;
    if (!initialized) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        initialized = true;
    }
    
    // التحقق من عدد المعاملات (0، 1، أو 2)
    // Validate argument count (0, 1, or 2)
    if (!validateArgCount(args, 0, 2, "random")) {
        return Value(0); // إرجاع 0 عند الخطأ / Return 0 on error
    }
    
    // random() - رقم بين 0 و RAND_MAX
    // random() - number between 0 and RAND_MAX
    if (args.size() == 0) {
        return Value(std::rand());
    }
    
    // random(max) - رقم بين 0 و max-1
    // random(max) - number between 0 and max-1
    if (args.size() == 1) {
        int max = args[0].toInt();
        if (max <= 0) {
            std::cerr << "خطأ / Error: random() الحد الأقصى يجب أن يكون موجباً / max must be positive" << std::endl;
            return Value(0);
        }
        return Value(std::rand() % max);
    }
    
    // random(min, max) - رقم بين min و max-1
    // random(min, max) - number between min and max-1
    int min = args[0].toInt();
    int max = args[1].toInt();
    
    if (min >= max) {
        std::cerr << "خطأ / Error: random() الحد الأدنى يجب أن يكون أقل من الحد الأقصى / min must be less than max" << std::endl;
        return Value(min);
    }
    
    int range = max - min;
    return Value(min + (std::rand() % range));
}

// =============================================================================
// sleep() - إيقاف التنفيذ مؤقتاً / Pause execution temporarily
// =============================================================================

Value sleep(const std::vector<Value>& args) {
    // التحقق من عدد المعاملات (1 بالضبط)
    // Validate argument count (exactly 1)
    if (!validateArgCount(args, 1, 1, "sleep")) {
        return Value(); // إرجاع void عند الخطأ / Return void on error
    }
    
    // الحصول على عدد الميلي ثانية
    // Get number of milliseconds
    int milliseconds = args[0].toInt();
    
    if (milliseconds < 0) {
        std::cerr << "خطأ / Error: sleep() المدة يجب أن تكون غير سالبة / duration must be non-negative" << std::endl;
        return Value();
    }
    
    // إيقاف التنفيذ
    // Pause execution
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    
    return Value(); // void
}

// =============================================================================
// exit() - إنهاء البرنامج / Terminate program
// =============================================================================

Value exit(const std::vector<Value>& args) {
    // التحقق من عدد المعاملات (0 أو 1)
    // Validate argument count (0 or 1)
    if (!validateArgCount(args, 0, 1, "exit")) {
        std::exit(1); // خروج مع خطأ / Exit with error
    }
    
    // الحصول على كود الخروج (افتراضي: 0)
    // Get exit code (default: 0)
    int exit_code = 0;
    if (args.size() == 1) {
        exit_code = args[0].toInt();
    }
    
    // إنهاء البرنامج
    // Terminate program
    std::exit(exit_code);
    
    // هذا السطر لن يُنفذ أبداً
    // This line will never execute
    return Value();
}

// =============================================================================
// assert() - التحقق من شرط / Assert a condition
// =============================================================================

Value assert(const std::vector<Value>& args) {
    // التحقق من عدد المعاملات (1 أو 2)
    // Validate argument count (1 or 2)
    if (!validateArgCount(args, 1, 2, "assert")) {
        return Value(); // إرجاع void عند الخطأ / Return void on error
    }
    
    // الحصول على الشرط
    // Get condition
    bool condition = args[0].toBool();
    
    // إذا كان الشرط صحيحاً، لا نفعل شيئاً
    // If condition is true, do nothing
    if (condition) {
        return Value();
    }
    
    // الشرط خاطئ - نطبع رسالة الخطأ
    // Condition is false - print error message
    std::cerr << "❌ فشل التحقق / Assertion Failed!" << std::endl;
    
    if (args.size() == 2) {
        std::string message = args[1].toString();
        std::cerr << "   الرسالة / Message: " << message << std::endl;
    }
    
    // نوقف البرنامج
    // Stop the program
    std::cerr << "   إيقاف البرنامج / Stopping program..." << std::endl;
    std::exit(1);
    
    // هذا السطر لن يُنفذ أبداً
    // This line will never execute
    return Value();
}

} // namespace Core
} // namespace StdLib
} // namespace Sad
