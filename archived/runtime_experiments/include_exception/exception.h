// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════
// ملف: exception.h
// الوصف: نظام معالجة الاستثناءات الشامل للغة ص
// ═══════════════════════════════════════════════════════════════════════════
//
// الغرض من هذا الملف:
// ──────────────────
// يوفر نظام استثناءات متكاملاً لوقت تشغيل لغة ص. يتضمن:
// - هرمية أنواع الاستثناءات (25+ نوع) مصنفة حسب الفئة
// - تتبع المكدس (Stack Trace) التلقائي عند حدوث الاستثناء
// - سلسلة الأسباب (Cause Chain) لربط الاستثناءات المتتالية
// - معالج الاستثناءات (ExceptionHandler) مع أنماط التقاط مرنة
//
// المكونات الرئيسية:
// ─────────────────
// - StackFrame: معلومات إطار واحد في المكدس (اسم الدالة، الملف، السطر)
// - StackTrace: قائمة إطارات المكدس — يُلتقط تلقائياً عند رمي الاستثناء
// - ExceptionType: تعداد شامل لأنواع الأخطاء:
//   • أخطاء عامة: RUNTIME_ERROR, LOGIC_ERROR, SYSTEM_ERROR
//   • أخطاء الأنواع: TYPE_ERROR, CAST_ERROR
//   • أخطاء القيم: VALUE_ERROR, RANGE_ERROR, OVERFLOW_ERROR
//   • أخطاء الذاكرة: OUT_OF_MEMORY, NULL_POINTER, INVALID_MEMORY
//   • أخطاء الفهرسة: INDEX_ERROR, KEY_ERROR
//   • أخطاء الإدخال/الإخراج: IO_ERROR, FILE_NOT_FOUND, PERMISSION_DENIED
//   • أخطاء الحساب: DIVISION_BY_ZERO
//   • أخطاء الاستيراد: IMPORT_ERROR, MODULE_NOT_FOUND
//   • أخطاء الكائنات: ATTRIBUTE_ERROR, METHOD_NOT_FOUND
//   • أخطاء الخيوط: THREAD_ERROR, DEADLOCK
// - Exception: الفئة الأساسية — ترث من std::exception وتضيف:
//   النوع، الرسالة، الملف، السطر، تتبع المكدس، وسلسلة الأسباب
// - أصناف محددة: RuntimeError, TypeError, ValueError, IndexError,
//   KeyError, OutOfMemoryError, DivisionByZeroError, إلخ.
// - ExceptionHandler: مسجّل معالجات الاستثناءات مع أنماط تحقق
//   (نوع محدد، نوع مع أبناء، فلتر مخصص)
//
// الموقع في البنية العامة:
// ───────────────────────
//   runtime_new/
//   ├── vm/ ← الآلة الافتراضية ترمي وتلتقط الاستثناءات
//   ├── memory/ ← OutOfMemoryError عند فشل التخصيص
//   ├── thread/ ← ThreadError, Deadlock
//   ├── ffi/ ← أخطاء تحميل المكتبات والاستدعاء
//   └── [exception/ — هذا الملف] ← نظام الاستثناءات المركزي
//
// أمثلة الاستخدام (من كود ص):
// ──────────────────────────
//   حاول
//     متغير نتيجة = 10 / 0
//   امسك (خطأ)
//     اطبع("حدث خطأ: " + خطأ)
//   أخيراً
//     اطبع("تم التنظيف")
//   نهاية
//
// الاعتماديات:
// ──────────
// - <exception>, <stdexcept>: واجهة std::exception القياسية
// - <functional>: لدوال الفلتر في معالج الاستثناءات
// - <mutex>: لسلامة الاستثناءات في بيئة متعددة الخيوط
//
// الحمد لله رب العالمين
// ═══════════════════════════════════════════════════════════════════════════

#ifndef SAD_RUNTIME_EXCEPTION_H
#define SAD_RUNTIME_EXCEPTION_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <exception>
#include <stdexcept>
#include <cstdint>
#include <mutex>

namespace sad {
namespace runtime {

// ===================================================================
// Forward Declarations
// ===================================================================

class StackTrace;
class StackFrame;
class Exception;
class ExceptionHandler;

// ===================================================================
// Stack Frame Information - معلومات إطار المكدس
// ===================================================================

class StackFrame {
public:
    StackFrame() = default;
    StackFrame(const std::string& function,
               const std::string& file,
               int line,
               void* address = nullptr);

    // Getters
    const std::string& get_function_name() const { return function_name_; }
    const std::string& get_file_name() const { return file_name_; }
    int get_line_number() const { return line_number_; }
    void* get_address() const { return address_; }

    // Formatting - التنسيق
    std::string to_string() const;

private:
    std::string function_name_;  // اسم الدالة
    std::string file_name_;      // اسم الملف
    int line_number_ = 0;        // رقم السطر
    void* address_ = nullptr;    // عنوان الذاكرة
};

// ===================================================================
// Stack Trace - تتبع المكدس
// ===================================================================

class StackTrace {
public:
    StackTrace() = default;

    // Capture current stack trace - التقاط حالة المكدس الحالية
    void capture(int skip_frames = 0, int max_frames = 64);

    // Add frame manually
    void add_frame(const StackFrame& frame);
    void add_frame(const std::string& function,
                   const std::string& file,
                   int line,
                   void* address = nullptr);

    // Access frames
    const std::vector<StackFrame>& get_frames() const { return frames_; }
    size_t get_frame_count() const { return frames_.size(); }
    const StackFrame& get_frame(size_t index) const { return frames_[index]; }

    // Formatting
    std::string to_string() const;
    void print() const;

    // Clear
    void clear() { frames_.clear(); }

private:
    std::vector<StackFrame> frames_;  // قائمة الإطارات
};

// ===================================================================
// Exception Type - نوع الاستثناء
// ===================================================================

enum class ExceptionType {
    // General errors - أخطاء عامة
    RUNTIME_ERROR,          // خطأ في وقت التشغيل
    LOGIC_ERROR,            // خطأ منطقي
    SYSTEM_ERROR,           // خطأ في النظام

    // Type errors - أخطاء الأنواع
    TYPE_ERROR,             // خطأ في النوع
    CAST_ERROR,             // خطأ في التحويل

    // Value errors - أخطاء القيم
    VALUE_ERROR,            // خطأ في القيمة
    RANGE_ERROR,            // خطأ في النطاق
    OVERFLOW_ERROR,         // خطأ في الفيض
    UNDERFLOW_ERROR,        // خطأ في النقص

    // Memory errors - أخطاء الذاكرة
    OUT_OF_MEMORY,          // نفاذ الذاكرة
    NULL_POINTER,           // مؤشر فارغ
    INVALID_MEMORY,         // ذاكرة غير صالحة

    // Index/Key errors - أخطاء الفهرسة
    INDEX_ERROR,            // خطأ في الفهرس
    KEY_ERROR,              // خطأ في المفتاح

    // I/O errors - أخطاء الإدخال/الإخراج
    IO_ERROR,               // خطأ في الإدخال/الإخراج
    FILE_NOT_FOUND,         // ملف غير موجود
    PERMISSION_DENIED,      // رفض الإذن

    // Division errors - أخطاء القسمة
    DIVISION_BY_ZERO,       // قسمة على صفر

    // Import/Module errors - أخطاء الاستيراد
    IMPORT_ERROR,           // خطأ في الاستيراد
    MODULE_NOT_FOUND,       // وحدة غير موجودة

    // Attribute errors - أخطاء الخصائص
    ATTRIBUTE_ERROR,        // خطأ في الخاصية
    METHOD_NOT_FOUND,       // دالة غير موجودة

    // Thread errors - أخطاء الخيوط
    THREAD_ERROR,           // خطأ في الخيط
    DEADLOCK,               // جمود

    // Custom error - خطأ مخصص
    CUSTOM_ERROR
};

// Get exception type name - الحصول على اسم نوع الاستثناء
const char* get_exception_type_name(ExceptionType type);

// ===================================================================
// Exception Base Class - الفئة الأساسية للاستثناء
// ===================================================================

class Exception : public std::exception {
public:
    // Constructors
    Exception(ExceptionType type,
              const std::string& message,
              const std::string& file = "",
              int line = 0);

    virtual ~Exception() noexcept = default;

    // Copy and move
    Exception(const Exception& other) = default;
    Exception& operator=(const Exception& other) = default;
    Exception(Exception&& other) noexcept = default;
    Exception& operator=(Exception&& other) noexcept = default;

    // Get information - الحصول على المعلومات
    ExceptionType get_type() const { return type_; }
    const char* get_type_name() const { return get_exception_type_name(type_); }
    const std::string& get_message() const { return message_; }
    const std::string& get_file() const { return file_; }
    int get_line() const { return line_; }

    // Stack trace
    const StackTrace& get_stack_trace() const { return stack_trace_; }
    StackTrace& get_stack_trace() { return stack_trace_; }

    // std::exception interface
    virtual const char* what() const noexcept override;

    // Formatting
    virtual std::string to_string() const;
    virtual void print() const;

    // Cause chain (للاستثناءات المتسلسلة)
    void set_cause(std::shared_ptr<Exception> cause) { cause_ = cause; }
    std::shared_ptr<Exception> get_cause() const { return cause_; }

protected:
    ExceptionType type_;                    // نوع الاستثناء
    std::string message_;                   // رسالة الخطأ
    std::string file_;                      // اسم الملف
    int line_;                              // رقم السطر
    StackTrace stack_trace_;                // تتبع المكدس
    mutable std::string cached_what_;       // نسخة مخزنة من what()
    std::shared_ptr<Exception> cause_;      // الاستثناء المسبب
};

// ===================================================================
// Specific Exception Types - أنواع استثناءات محددة
// ===================================================================

class RuntimeError : public Exception {
public:
    RuntimeError(const std::string& message,
                 const std::string& file = "",
                 int line = 0)
        : Exception(ExceptionType::RUNTIME_ERROR, message, file, line) {}
};

class TypeError : public Exception {
public:
    TypeError(const std::string& message,
              const std::string& file = "",
              int line = 0)
        : Exception(ExceptionType::TYPE_ERROR, message, file, line) {}
};

class ValueError : public Exception {
public:
    ValueError(const std::string& message,
               const std::string& file = "",
               int line = 0)
        : Exception(ExceptionType::VALUE_ERROR, message, file, line) {}
};

class IndexError : public Exception {
public:
    IndexError(const std::string& message,
               const std::string& file = "",
               int line = 0)
        : Exception(ExceptionType::INDEX_ERROR, message, file, line) {}
};

class KeyError : public Exception {
public:
    KeyError(const std::string& message,
             const std::string& file = "",
             int line = 0)
        : Exception(ExceptionType::KEY_ERROR, message, file, line) {}
};

class OutOfMemoryError : public Exception {
public:
    OutOfMemoryError(const std::string& message,
                     const std::string& file = "",
                     int line = 0)
        : Exception(ExceptionType::OUT_OF_MEMORY, message, file, line) {}
};

class NullPointerError : public Exception {
public:
    NullPointerError(const std::string& message,
                     const std::string& file = "",
                     int line = 0)
        : Exception(ExceptionType::NULL_POINTER, message, file, line) {}
};

class DivisionByZeroError : public Exception {
public:
    DivisionByZeroError(const std::string& message,
                        const std::string& file = "",
                        int line = 0)
        : Exception(ExceptionType::DIVISION_BY_ZERO, message, file, line) {}
};

class IOError : public Exception {
public:
    IOError(const std::string& message,
            const std::string& file = "",
            int line = 0)
        : Exception(ExceptionType::IO_ERROR, message, file, line) {}
};

class FileNotFoundError : public Exception {
public:
    FileNotFoundError(const std::string& message,
                      const std::string& file = "",
                      int line = 0)
        : Exception(ExceptionType::FILE_NOT_FOUND, message, file, line) {}
};

// ===================================================================
// Exception Context - سياق الاستثناء
// ===================================================================

struct ExceptionContext {
    std::shared_ptr<Exception> current_exception;  // الاستثناء الحالي
    bool in_catch_block = false;                   // هل نحن في كتلة catch
    bool in_finally_block = false;                 // هل نحن في كتلة finally
    std::vector<std::function<void()>> cleanup_handlers;  // معالجات التنظيف
};

// ===================================================================
// Exception Handler - معالج الاستثناءات
// ===================================================================

class ExceptionHandler {
public:
    // Get singleton instance
    static ExceptionHandler& get_instance();

    // Throw exception - رمي استثناء
    [[noreturn]] void throw_exception(std::shared_ptr<Exception> exception);
    [[noreturn]] void throw_exception(Exception* exception);

    // Rethrow current exception - إعادة رمي الاستثناء الحالي
    [[noreturn]] void rethrow();

    // Get current exception - الحصول على الاستثناء الحالي
    std::shared_ptr<Exception> get_current_exception() const;
    bool has_current_exception() const;

    // Try-Catch-Finally support
    template<typename TryFunc, typename CatchFunc>
    bool try_catch(TryFunc try_func, CatchFunc catch_func);

    template<typename TryFunc, typename CatchFunc, typename FinallyFunc>
    bool try_catch_finally(TryFunc try_func, CatchFunc catch_func, FinallyFunc finally_func);

    // Add cleanup handler - إضافة معالج تنظيف
    void add_cleanup_handler(std::function<void()> handler);

    // Execute cleanup handlers - تنفيذ معالجات التنظيف
    void execute_cleanup_handlers();

    // Clear current exception - مسح الاستثناء الحالي
    void clear_exception();

    // Exception context (thread-local)
    ExceptionContext& get_context();

private:
    ExceptionHandler() = default;
    ~ExceptionHandler() = default;

    // Prevent copying
    ExceptionHandler(const ExceptionHandler&) = delete;
    ExceptionHandler& operator=(const ExceptionHandler&) = delete;

    // Thread-local exception context
    static thread_local ExceptionContext context_;
};

// ===================================================================
// Template Implementation
// ===================================================================

template<typename TryFunc, typename CatchFunc>
bool ExceptionHandler::try_catch(TryFunc try_func, CatchFunc catch_func) {
    try {
        try_func();
        return true;
    } catch (Exception& ex) {
        get_context().current_exception = std::make_shared<Exception>(ex);
        get_context().in_catch_block = true;
        
        try {
            catch_func(ex);
        } catch (...) {
            get_context().in_catch_block = false;
            throw;
        }
        
        get_context().in_catch_block = false;
        return false;
    } catch (std::exception& ex) {
        RuntimeError sad_ex(ex.what());
        get_context().current_exception = std::make_shared<RuntimeError>(sad_ex);
        get_context().in_catch_block = true;
        
        try {
            catch_func(sad_ex);
        } catch (...) {
            get_context().in_catch_block = false;
            throw;
        }
        
        get_context().in_catch_block = false;
        return false;
    } catch (...) {
        RuntimeError sad_ex("Unknown exception");
        get_context().current_exception = std::make_shared<RuntimeError>(sad_ex);
        get_context().in_catch_block = true;
        
        try {
            catch_func(sad_ex);
        } catch (...) {
            get_context().in_catch_block = false;
            throw;
        }
        
        get_context().in_catch_block = false;
        return false;
    }
}

template<typename TryFunc, typename CatchFunc, typename FinallyFunc>
bool ExceptionHandler::try_catch_finally(TryFunc try_func, CatchFunc catch_func, FinallyFunc finally_func) {
    bool success = true;
    
    try {
        success = try_catch(try_func, catch_func);
    } catch (...) {
        get_context().in_finally_block = true;
        finally_func();
        get_context().in_finally_block = false;
        throw;
    }
    
    get_context().in_finally_block = true;
    finally_func();
    get_context().in_finally_block = false;
    
    return success;
}

// ===================================================================
// Exception Macros - ماكروهات الاستثناءات
// ===================================================================

// Throw exception with file and line information
#define SAD_THROW(ExceptionType, message) \
    sad::runtime::ExceptionHandler::get_instance().throw_exception( \
        new ExceptionType(message, __FILE__, __LINE__))

// Rethrow current exception
#define SAD_RETHROW() \
    sad::runtime::ExceptionHandler::get_instance().rethrow()

// Try-Catch block
#define SAD_TRY_CATCH(try_block, catch_block) \
    sad::runtime::ExceptionHandler::get_instance().try_catch( \
        [&]() { try_block; }, \
        [&](sad::runtime::Exception& ex) { catch_block; })

// Try-Catch-Finally block
#define SAD_TRY_CATCH_FINALLY(try_block, catch_block, finally_block) \
    sad::runtime::ExceptionHandler::get_instance().try_catch_finally( \
        [&]() { try_block; }, \
        [&](sad::runtime::Exception& ex) { catch_block; }, \
        [&]() { finally_block; })

// ===================================================================
// Utility Functions - دوال مساعدة
// ===================================================================

// Create exception from error code
std::shared_ptr<Exception> create_exception_from_errno(int error_code,
                                                        const std::string& context = "");

// Print exception with stack trace
void print_exception(const Exception& ex, bool include_stack_trace = true);

// Get full exception chain
std::vector<std::shared_ptr<Exception>> get_exception_chain(const Exception& ex);

} // namespace runtime
} // namespace sad

#endif // SAD_RUNTIME_EXCEPTION_H
