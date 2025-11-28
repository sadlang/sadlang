/**
 * @file exception.h
 * @brief Sad Language Exception System / نظام الاستثناءات للغة ص
 * @details Complete exception handling infrastructure with bilingual support
 *          البنية التحتية الكاملة لمعالجة الاستثناءات مع دعم ثنائي اللغة
 * 
 * Features / المميزات:
 * - Base exception class hierarchy / تسلسل الفئات الأساسية للاستثناءات
 * - Stack trace support / دعم تتبع المكدس
 * - Bilingual error messages / رسائل خطأ ثنائية اللغة
 * - Standard exception types / أنواع الاستثناءات القياسية
 * 
 * @author Sad Language Team
 * @date 2025-11-27
 * @version 1.0.0
 */

#ifndef SAD_INTERPRETER_EXCEPTION_H
#define SAD_INTERPRETER_EXCEPTION_H

#include <exception>
#include <string>
#include <vector>
#include "lexer/token.h"  // For Position struct

namespace Sad {
namespace Interpreter {

/**
 * @class SadException
 * @brief Base class for all Sad Language exceptions
 *        الفئة الأساسية لجميع استثناءات لغة ص
 * 
 * @details Provides:
 *          - Error message storage (bilingual)
 *          - Exception type identification
 *          - Stack trace collection
 *          - Source code position tracking
 * 
 *          توفر:
 *          - تخزين رسالة الخطأ (ثنائية اللغة)
 *          - تحديد نوع الاستثناء
 *          - جمع تتبع المكدس
 *          - تتبع موضع الكود المصدري
 */
class SadException : public std::exception {
protected:
    std::string message_;       ///< Error message / رسالة الخطأ
    std::string type_;          ///< Exception type name / اسم نوع الاستثناء
    std::vector<std::string> stackTrace_;  ///< Stack trace frames / إطارات تتبع المكدس
    Lexer::Position position_;  ///< Source position / موضع المصدر
    
public:
    /**
     * @brief Construct exception with message and type
     *        إنشاء استثناء مع رسالة ونوع
     * 
     * @param message Error message (bilingual preferred) / رسالة الخطأ (يفضل ثنائية اللغة)
     * @param type Exception type name / اسم نوع الاستثناء
     * @param position Source code position / موضع الكود المصدري
     * 
     * @example
     * ```cpp
     * throw SadException(
     *     "(AR) خطأ في التنفيذ / (EN) Runtime error",
     *     "RuntimeError",
     *     currentPosition
     * );
     * ```
     */
    SadException(const std::string& message, 
                 const std::string& type,
                 const Lexer::Position& position = Lexer::Position());
    
    /**
     * @brief Virtual destructor for inheritance
     *        المدمر الافتراضي للوراثة
     */
    virtual ~SadException() = default;
    
    /**
     * @brief Get error message
     *        الحصول على رسالة الخطأ
     * 
     * @return Error message string / نص رسالة الخطأ
     */
    std::string getMessage() const { return message_; }
    
    /**
     * @brief Get exception type name
     *        الحصول على اسم نوع الاستثناء
     * 
     * @return Type name (e.g., "RuntimeError") / اسم النوع
     */
    std::string getType() const { return type_; }
    
    /**
     * @brief Get stack trace frames
     *        الحصول على إطارات تتبع المكدس
     * 
     * @return Vector of stack frame descriptions / متجه من أوصاف إطارات المكدس
     */
    std::vector<std::string> getStackTrace() const { return stackTrace_; }
    
    /**
     * @brief Get source code position where exception occurred
     *        الحصول على موضع الكود المصدري حيث حدث الاستثناء
     * 
     * @return Position object / كائن الموضع
     */
    Lexer::Position getPosition() const { return position_; }
    
    /**
     * @brief Add a stack frame to the trace
     *        إضافة إطار مكدس إلى التتبع
     * 
     * @param frame Stack frame description / وصف إطار المكدس
     * 
     * @example
     * ```cpp
     * exception.addStackFrame("at calculateTotal (main.s:42:10)");
     * ```
     */
    void addStackFrame(const std::string& frame);
    
    /**
     * @brief Get formatted exception string with stack trace
     *        الحصول على نص الاستثناء المنسق مع تتبع المكدس
     * 
     * @return Formatted exception string / نص الاستثناء المنسق
     * 
     * @example Output:
     * ```
     * RuntimeError: لا يمكن القسمة على صفر / Cannot divide by zero
     *   at divide (math.s:15:12)
     *   at calculate (main.s:42:5)
     *   at main (main.s:50:1)
     * ```
     */
    virtual std::string toString() const;
    
    /**
     * @brief Override std::exception::what()
     *        تجاوز std::exception::what()
     * 
     * @return C-style error message / رسالة خطأ بنمط C
     */
    const char* what() const noexcept override;
};

// ============================================================================
// (AR) أنواع الاستثناءات القياسية
// (EN) Standard Exception Types
// ============================================================================

/**
 * @class RuntimeError
 * @brief General runtime error / خطأ عام في وقت التشغيل
 * 
 * @details Used for generic runtime errors that don't fit other categories
 *          يستخدم للأخطاء العامة في وقت التشغيل التي لا تناسب فئات أخرى
 * 
 * @example
 * ```cpp
 * throw RuntimeError(
 *     "(AR) عملية غير صالحة / (EN) Invalid operation",
 *     position
 * );
 * ```
 */
class RuntimeError : public SadException {
public:
    RuntimeError(const std::string& message, 
                 const Lexer::Position& position = Lexer::Position())
        : SadException(message, "RuntimeError", position) {}
};

/**
 * @class TypeError
 * @brief Type mismatch or invalid type error / خطأ في عدم تطابق الأنواع
 * 
 * @details Thrown when operations are performed on incompatible types
 *          يُرمى عندما تُجرى عمليات على أنواع غير متوافقة
 * 
 * @example
 * ```cpp
 * throw TypeError(
 *     "(AR) لا يمكن إضافة نص إلى رقم / (EN) Cannot add string to integer",
 *     position
 * );
 * ```
 */
class TypeError : public SadException {
public:
    TypeError(const std::string& message,
              const Lexer::Position& position = Lexer::Position())
        : SadException(message, "TypeError", position) {}
};

/**
 * @class ValueError
 * @brief Invalid value error / خطأ في قيمة غير صالحة
 * 
 * @details Thrown when a value is invalid for the given context
 *          يُرمى عندما تكون القيمة غير صالحة للسياق المعطى
 * 
 * @example
 * ```cpp
 * throw ValueError(
 *     "(AR) القيمة يجب أن تكون موجبة / (EN) Value must be positive",
 *     position
 * );
 * ```
 */
class ValueError : public SadException {
public:
    ValueError(const std::string& message,
               const Lexer::Position& position = Lexer::Position())
        : SadException(message, "ValueError", position) {}
};

/**
 * @class DivisionByZeroError
 * @brief Division by zero error / خطأ القسمة على صفر
 * 
 * @details Thrown when attempting to divide by zero
 *          يُرمى عند محاولة القسمة على صفر
 * 
 * @example
 * ```cpp
 * throw DivisionByZeroError(
 *     "(AR) لا يمكن القسمة على صفر / (EN) Cannot divide by zero",
 *     position
 * );
 * ```
 */
class DivisionByZeroError : public SadException {
public:
    DivisionByZeroError(const std::string& message,
                        const Lexer::Position& position = Lexer::Position())
        : SadException(message, "DivisionByZeroError", position) {}
    
    // Convenience constructor with default message / منشئ سهل مع رسالة افتراضية
    explicit DivisionByZeroError(const Lexer::Position& position = Lexer::Position())
        : SadException(
            "(AR) لا يمكن القسمة على صفر / (EN) Cannot divide by zero",
            "DivisionByZeroError",
            position
        ) {}
};

/**
 * @class IndexOutOfRangeError
 * @brief Array/string index out of bounds error / خطأ الفهرس خارج النطاق
 * 
 * @details Thrown when accessing an array or string with invalid index
 *          يُرمى عند الوصول إلى مصفوفة أو نص بفهرس غير صالح
 * 
 * @example
 * ```cpp
 * throw IndexOutOfRangeError(
 *     "(AR) الفهرس 10 خارج النطاق / (EN) Index 10 out of range",
 *     position
 * );
 * ```
 */
class IndexOutOfRangeError : public SadException {
public:
    IndexOutOfRangeError(const std::string& message,
                         const Lexer::Position& position = Lexer::Position())
        : SadException(message, "IndexOutOfRangeError", position) {}
    
    // Convenience constructor with index value / منشئ سهل مع قيمة الفهرس
    IndexOutOfRangeError(int index, 
                         const Lexer::Position& position = Lexer::Position())
        : SadException(
            "(AR) الفهرس " + std::to_string(index) + " خارج النطاق / "
            "(EN) Index " + std::to_string(index) + " out of range",
            "IndexOutOfRangeError",
            position
        ) {}
};

/**
 * @class NullReferenceError
 * @brief Null reference access error / خطأ الوصول إلى مرجع فارغ
 * 
 * @details Thrown when attempting to access members of a null/void value
 *          يُرمى عند محاولة الوصول إلى أعضاء قيمة فارغة
 * 
 * @example
 * ```cpp
 * throw NullReferenceError(
 *     "(AR) محاولة الوصول إلى كائن فارغ / (EN) Attempting to access null object",
 *     position
 * );
 * ```
 */
class NullReferenceError : public SadException {
public:
    NullReferenceError(const std::string& message,
                       const Lexer::Position& position = Lexer::Position())
        : SadException(message, "NullReferenceError", position) {}
    
    // Convenience constructor with default message / منشئ سهل مع رسالة افتراضية
    explicit NullReferenceError(const Lexer::Position& position = Lexer::Position())
        : SadException(
            "(AR) محاولة الوصول إلى كائن فارغ / (EN) Attempting to access null object",
            "NullReferenceError",
            position
        ) {}
};

/**
 * @class KeyError
 * @brief Dictionary key not found error / خطأ المفتاح غير موجود
 * 
 * @details Thrown when accessing a dictionary with a non-existent key
 *          يُرمى عند الوصول إلى قاموس بمفتاح غير موجود
 * 
 * @example
 * ```cpp
 * throw KeyError(
 *     "(AR) المفتاح 'الاسم' غير موجود / (EN) Key 'name' not found",
 *     position
 * );
 * ```
 */
class KeyError : public SadException {
public:
    KeyError(const std::string& message,
             const Lexer::Position& position = Lexer::Position())
        : SadException(message, "KeyError", position) {}
    
    // Convenience constructor with key name / منشئ سهل مع اسم المفتاح
    KeyError(const std::string& keyName,
             bool isConstructor,  // dummy parameter to distinguish constructors
             const Lexer::Position& position = Lexer::Position())
        : SadException(
            "(AR) المفتاح '" + keyName + "' غير موجود / "
            "(EN) Key '" + keyName + "' not found",
            "KeyError",
            position
        ) {}
};

} // namespace Interpreter
} // namespace Sad

#endif // SAD_INTERPRETER_EXCEPTION_H
