/**
 * @file io_functions.h
 * @brief (AR) دوال الإدخال والإخراج / (EN) Input/Output Functions
 * 
 * Phase 1 of Standard Library Implementation:
 * Core I/O functions for the Sad Language interpreter.
 * 
 * Functions implemented:
 * - طبع() / print() - Output without newline
 * - طبع_سطر() / println() - Output with newline
 * - أدخل() / input() - Read user input as string
 * - قراءة_سطر() / readLine() - Read full line from input
 * - مسح_الشاشة() / clear() - Clear console screen
 * 
 * @author S Language Development Team
 * @date November 22, 2025
 * @version 1.0
 * 
 * @note
 * (AR) هذه الوحدة توفر الدوال الأساسية لعمليات الإدخال والإخراج
 * (EN) This module provides fundamental I/O operations for the interpreter
 * 
 * Integration Points:
 * - Include in: stdlib_manager.h and stdlib_manager.cpp
 * - Used by: Interpreter core during function registration
 * - Tests: tests/stdlib_tests/test_io_functions.cpp
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include "value.h"

namespace Sad {
namespace StdLib {
namespace IO {

/**
 * @class IOFunctions
 * @brief (AR) فئة دوال الإدخال والإخراج / (EN) I/O Functions Class
 * 
 * Provides static methods for all I/O operations required in Phase 1.
 * All methods are thread-safe for console operations.
 */
class IOFunctions {
public:
    // Delete default constructor - only static methods
    IOFunctions() = delete;
    ~IOFunctions() = default;

    /**
     * @brief (AR) طبع (طباعة) قيمة بدون إضافة سطر جديد
     * @brief (EN) Print a value without adding a newline
     * 
     * @param args (AR) قائمة القيم المراد طباعتها / (EN) Values to print
     * @return Value (AR) قيمة فارغة دائماً / (EN) Always returns VOID
     * 
     * @note Supports multiple arguments - prints them space-separated
     * 
     * Example (AR):
     * @code
     * طبع("مرحبا")           // Output: مرحبا
     * طبع(42)               // Output: 42
     * طبع("الرقم: ", 100)   // Output: الرقم:  100
     * @endcode
     * 
     * Example (EN):
     * @code
     * print("Hello")        // Output: Hello
     * print(42)             // Output: 42
     * print("Value: ", 100) // Output: Value:  100
     * @endcode
     */
    static Data::Value print(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) طبع_سطر (طباعة سطر) - طباعة قيمة مع إضافة سطر جديد
     * @brief (EN) Println - Print a value with a newline
     * 
     * @param args (AR) قائمة القيم المراد طباعتها / (EN) Values to print
     * @return Value (AR) قيمة فارغة دائماً / (EN) Always returns VOID
     * 
     * @note Automatically adds newline at the end
     * 
     * Example (AR):
     * @code
     * طبع_سطر("السطر الأول")
     * طبع_سطر("السطر الثاني")
     * @endcode
     * 
     * Example (EN):
     * @code
     * println("First line")
     * println("Second line")
     * @endcode
     */
    static Data::Value println(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) أدخل - قراءة إدخال من المستخدم
     * @brief (EN) Input - Read user input as string
     * 
     * @param args (AR) قائمة معاملات اختيارية (رسالة موجهة) / (EN) Optional arguments (prompt message)
     * @return Value (AR) نص الإدخال من المستخدم / (EN) User input as string
     * 
     * @note If an argument is provided, it's displayed as a prompt before reading
     * 
     * Example (AR):
     * @code
     * الإجابة = أدخل("اكتب اسمك: ")
     * @endcode
     * 
     * Example (EN):
     * @code
     * answer = input("Enter your name: ")
     * @endcode
     */
    static Data::Value input(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) قراءة_سطر - قراءة سطر كامل من الإدخال
     * @brief (EN) ReadLine - Read a full line from input
     * 
     * @param args (AR) معاملات اختيارية (رسالة موجهة) / (EN) Optional prompt message
     * @return Value (AR) نص السطر المقروء / (EN) Read line as string
     * 
     * @note Similar to input() but explicitly designed for line reading
     * 
     * Example (AR):
     * @code
     * السطر = قراءة_سطر("أدخل السطر: ")
     * @endcode
     * 
     * Example (EN):
     * @code
     * line = readLine("Enter line: ")
     * @endcode
     */
    static Data::Value readLine(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) مسح_الشاشة - مسح شاشة الكونسول
     * @brief (EN) Clear - Clear the console screen
     * 
     * @param args (AR) بدون معاملات / (EN) No arguments
     * @return Value (AR) قيمة فارغة دائماً / (EN) Always returns VOID
     * 
     * @note Clears the entire console screen and positions cursor at top-left
     * 
     * Example (AR):
     * @code
     * مسح_الشاشة()
     * طبع_سطر("شاشة نظيفة")
     * @endcode
     * 
     * Example (EN):
     * @code
     * clear()
     * println("Clean screen")
     * @endcode
     */
    static Data::Value clear(const std::vector<Data::Value>& args);

    /**
     * @brief (AR) تحقق من صحة معاملات الإدخال
     * @brief (EN) Validate input arguments count
     * 
     * @param args (AR) القائمة المراد التحقق منها / (EN) Arguments to validate
     * @param minArgs (AR) الحد الأدنى / (EN) Minimum argument count
     * @param maxArgs (AR) الحد الأقصى / (EN) Maximum argument count (-1 = unlimited)
     * @return bool (AR) صحيح إذا كانت صحيحة / (EN) True if valid
     * 
     * @throws std::invalid_argument (AR) إذا كانت المعاملات غير صحيحة / (EN) If arguments invalid
     */
    static bool validateArguments(const std::vector<Data::Value>& args,
                                  size_t minArgs, 
                                  int maxArgs = -1);

private:
    /**
     * @brief (AR) معالجة تسلسلات الهروب في النصوص
     * @brief (EN) Process escape sequences in strings
     * 
     * @param input (AR) النص الأصلي / (EN) Original string
     * @return std::string (AR) النص بعد المعالجة / (EN) Processed string
     * 
     * @note Converts \n, \t, \r, \\, etc. to their actual characters
     */
    static std::string processEscapeSequences(const std::string& input);

    /**
     * @brief (AR) المساعد الداخلي للطباعة
     * @brief (EN) Internal helper for printing a single value
     * 
     * @param value (AR) القيمة المراد طباعتها / (EN) Value to print
     */
    static std::string valueToString(const Data::Value& value, int depth = 0);

    /**
     * @brief (AR) تحويل قيمة إلى نص قابل للطباعة
     * @brief (EN) Convert a value to printable string representation
     * 
     * @param value (AR) القيمة / (EN) The value
     * @return std::string (AR) النص المُنسّق / (EN) Formatted string
     * 
     * @note Handles arrays and maps specially with proper formatting
     */
    static std::string formatValue(const Data::Value& value);
};

} // namespace IO
} // namespace StdLib
} // namespace Sad

