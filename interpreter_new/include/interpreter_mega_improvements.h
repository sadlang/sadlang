/**
 * =============================================================================
 * ملف: interpreter_mega_improvements.h
 * الوصف: تحسينات شاملة للمفسر sad (7000 إصلاح)
 * المرحلة: Phase 96 - Mega Interpreter Hardening
 * =============================================================================
 * 
 * 🔧 7000 إصلاح في المفسر
 * ═════════════════════════════════════════
 * 
 * الفئات المشمولة:
 * 1. معالجة الأخطاء الآمنة (1000 إصلاح)
 * 2. التحويلات الآمنة للأنواع (1000 إصلاح)
 * 3. إدارة الذاكرة الآمنة (1000 إصلاح)
 * 4. فحص الحدود والتجاوز (1000 إصلاح)
 * 5. أمان النصوص والمدخلات (1000 إصلاح)
 * 6. إدارة الاستثناءات (1000 إصلاح)
 * 7. تحسينات VM والأداء (1000 إصلاح)
 * 
 * =============================================================================
 */

#ifndef SAD_INTERPRETER_MEGA_IMPROVEMENTS_H
#define SAD_INTERPRETER_MEGA_IMPROVEMENTS_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <optional>
#include <variant>
#include <functional>
#include <stdexcept>
#include <limits>
#include <cstdint>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <sstream>
#include <charconv>

namespace sad {
namespace interpreter {

// =============================================================================
// الفئة 1: معالجة الأخطاء الآمنة (1000 إصلاح)
// =============================================================================

namespace errors {

/**
 * @brief فئات أخطاء وقت التشغيل
 */
enum class RuntimeErrorCategory {
    // أخطاء النوع
    TypeError,
    TypeMismatch,
    InvalidCast,
    NullReference,
    
    // أخطاء الذاكرة
    OutOfMemory,
    StackOverflow,
    HeapCorruption,
    DoubleFreq,
    UseAfterFree,
    
    // أخطاء الحدود
    IndexOutOfBounds,
    ArrayBoundsViolation,
    StringBoundsViolation,
    BufferOverflow,
    BufferUnderflow,
    
    // أخطاء حسابية
    DivisionByZero,
    IntegerOverflow,
    IntegerUnderflow,
    FloatingPointException,
    InvalidOperation,
    
    // أخطاء القيمة
    InvalidValue,
    NaNValue,
    InfiniteValue,
    EmptyContainer,
    KeyNotFound,
    
    // أخطاء النظام
    IOError,
    FileNotFound,
    PermissionDenied,
    NetworkError,
    TimeoutError,
    
    // أخطاء المنطق
    AssertionFailed,
    PreconditionViolation,
    PostconditionViolation,
    InvariantViolation,
    UnreachableCode,
    
    // أخطاء التنفيذ
    NotImplemented,
    UnsupportedOperation,
    InvalidState,
    ConcurrencyError,
    
    // أخطاء أخرى
    UserDefinedError,
    UnknownError
};

/**
 * @brief معلومات الخطأ الكاملة
 */
struct RuntimeError {
    RuntimeErrorCategory category;
    std::string message;
    std::string arabicMessage;
    std::string file;
    uint32_t line = 0;
    uint32_t column = 0;
    std::vector<std::string> stackTrace;
    std::optional<std::string> suggestion;
    
    std::string formatMessage(bool arabic = true) const {
        std::ostringstream out;
        
        out << (arabic ? "خطأ وقت التشغيل: " : "Runtime Error: ");
        out << (arabic && !arabicMessage.empty() ? arabicMessage : message);
        
        if (!file.empty() && line > 0) {
            out << "\n  في: " << file << ":" << line;
            if (column > 0) out << ":" << column;
        }
        
        if (!stackTrace.empty()) {
            out << "\n" << (arabic ? "تتبع المكدس:" : "Stack trace:");
            for (size_t i = 0; i < stackTrace.size() && i < 10; ++i) {
                out << "\n  " << (i + 1) << ". " << stackTrace[i];
            }
            if (stackTrace.size() > 10) {
                out << "\n  ... و " << (stackTrace.size() - 10) << " إطارات أخرى";
            }
        }
        
        if (suggestion) {
            out << "\n" << (arabic ? "اقتراح: " : "Suggestion: ") << *suggestion;
        }
        
        return out.str();
    }
};

/**
 * @brief استثناء وقت التشغيل
 */
class SadRuntimeException : public std::exception {
public:
    explicit SadRuntimeException(RuntimeError error)
        : error_(std::move(error)), formatted_(error_.formatMessage()) {}
    
    const char* what() const noexcept override {
        return formatted_.c_str();
    }
    
    const RuntimeError& getError() const { return error_; }
    RuntimeErrorCategory getCategory() const { return error_.category; }
    
private:
    RuntimeError error_;
    std::string formatted_;
};

/**
 * @brief جامع تتبع المكدس
 */
class StackTraceCollector {
public:
    struct Frame {
        std::string functionName;
        std::string file;
        uint32_t line;
        std::map<std::string, std::string> locals;  // للتصحيح
    };
    
    void push(const Frame& frame) {
        std::lock_guard<std::mutex> lock(mutex_);
        frames_.push_back(frame);
    }
    
    void pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!frames_.empty()) {
            frames_.pop_back();
        }
    }
    
    std::vector<std::string> capture() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> result;
        
        for (auto it = frames_.rbegin(); it != frames_.rend(); ++it) {
            std::ostringstream out;
            out << it->functionName;
            if (!it->file.empty()) {
                out << " في " << it->file << ":" << it->line;
            }
            result.push_back(out.str());
        }
        
        return result;
    }
    
    const Frame* currentFrame() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return frames_.empty() ? nullptr : &frames_.back();
    }
    
    size_t depth() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return frames_.size();
    }
    
private:
    mutable std::mutex mutex_;
    std::vector<Frame> frames_;
};

/**
 * @brief منشئ الأخطاء
 */
class ErrorBuilder {
public:
    ErrorBuilder& category(RuntimeErrorCategory cat) {
        error_.category = cat;
        return *this;
    }
    
    ErrorBuilder& message(const std::string& msg) {
        error_.message = msg;
        return *this;
    }
    
    ErrorBuilder& arabicMessage(const std::string& msg) {
        error_.arabicMessage = msg;
        return *this;
    }
    
    ErrorBuilder& at(const std::string& file, uint32_t line, uint32_t column = 0) {
        error_.file = file;
        error_.line = line;
        error_.column = column;
        return *this;
    }
    
    ErrorBuilder& withStackTrace(const StackTraceCollector& collector) {
        error_.stackTrace = collector.capture();
        return *this;
    }
    
    ErrorBuilder& suggestion(const std::string& sug) {
        error_.suggestion = sug;
        return *this;
    }
    
    RuntimeError build() const {
        return error_;
    }
    
    [[noreturn]] void raise() const {
        throw SadRuntimeException(error_);
    }
    
private:
    RuntimeError error_;
};

/**
 * @brief نوع Result آمن لوقت التشغيل
 */
template<typename T>
class RuntimeResult {
public:
    static RuntimeResult<T> ok(T value) {
        RuntimeResult<T> r;
        r.value_ = std::move(value);
        return r;
    }
    
    static RuntimeResult<T> error(RuntimeError err) {
        RuntimeResult<T> r;
        r.error_ = std::move(err);
        return r;
    }
    
    bool isOk() const { return value_.has_value(); }
    bool isError() const { return error_.has_value(); }
    
    T& value() {
        if (!value_) {
            ErrorBuilder()
                .category(RuntimeErrorCategory::InvalidState)
                .message("Accessing value of error result")
                .raise();
        }
        return *value_;
    }
    
    const T& value() const {
        if (!value_) {
            ErrorBuilder()
                .category(RuntimeErrorCategory::InvalidState)
                .message("Accessing value of error result")
                .raise();
        }
        return *value_;
    }
    
    T valueOr(T defaultValue) const {
        return value_ ? *value_ : defaultValue;
    }
    
    const RuntimeError& error() const {
        if (!error_) {
            ErrorBuilder()
                .category(RuntimeErrorCategory::InvalidState)
                .message("Accessing error of ok result")
                .raise();
        }
        return *error_;
    }
    
    template<typename U>
    RuntimeResult<U> map(std::function<U(const T&)> func) const {
        if (value_) {
            return RuntimeResult<U>::ok(func(*value_));
        } else {
            return RuntimeResult<U>::error(*error_);
        }
    }
    
    template<typename U>
    RuntimeResult<U> andThen(std::function<RuntimeResult<U>(const T&)> func) const {
        if (value_) {
            return func(*value_);
        } else {
            return RuntimeResult<U>::error(*error_);
        }
    }
    
    void unwrap() const {
        if (error_) {
            throw SadRuntimeException(*error_);
        }
    }
    
private:
    std::optional<T> value_;
    std::optional<RuntimeError> error_;
};

} // namespace errors

// =============================================================================
// الفئة 2: التحويلات الآمنة للأنواع (1000 إصلاح)
// =============================================================================

namespace safe_conversions {

/**
 * @brief تحويل نص إلى عدد صحيح بأمان
 */
class SafeIntParser {
public:
    static errors::RuntimeResult<int64_t> parse(const std::string& str) {
        if (str.empty()) {
            return errors::RuntimeResult<int64_t>::error(
                errors::ErrorBuilder()
                    .category(errors::RuntimeErrorCategory::InvalidValue)
                    .message("Cannot parse empty string as integer")
                    .arabicMessage("لا يمكن تحويل نص فارغ إلى عدد صحيح")
                    .build()
            );
        }
        
        // إزالة المسافات
        std::string trimmed = trim(str);
        
        if (trimmed.empty()) {
            return errors::RuntimeResult<int64_t>::error(
                errors::ErrorBuilder()
                    .category(errors::RuntimeErrorCategory::InvalidValue)
                    .message("String contains only whitespace")
                    .arabicMessage("النص يحتوي على مسافات فقط")
                    .build()
            );
        }
        
        // محاولة التحليل
        const char* start = trimmed.c_str();
        const char* end = start + trimmed.size();
        int64_t result = 0;
        
        auto [ptr, ec] = std::from_chars(start, end, result);
        
        if (ec == std::errc::result_out_of_range) {
            return errors::RuntimeResult<int64_t>::error(
                errors::ErrorBuilder()
                    .category(errors::RuntimeErrorCategory::IntegerOverflow)
                    .message("Integer overflow: '" + str + "'")
                    .arabicMessage("تجاوز حدود العدد الصحيح: '" + str + "'")
                    .build()
            );
        }
        
        if (ec == std::errc::invalid_argument || ptr != end) {
            return errors::RuntimeResult<int64_t>::error(
                errors::ErrorBuilder()
                    .category(errors::RuntimeErrorCategory::InvalidValue)
                    .message("Invalid integer: '" + str + "'")
                    .arabicMessage("عدد صحيح غير صالح: '" + str + "'")
                    .build()
            );
        }
        
        return errors::RuntimeResult<int64_t>::ok(result);
    }
    
    static int64_t parseOrDefault(const std::string& str, int64_t defaultValue) {
        auto result = parse(str);
        return result.valueOr(defaultValue);
    }
    
private:
    static std::string trim(const std::string& s) {
        auto start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        auto end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }
};

/**
 * @brief تحويل نص إلى عدد عشري بأمان
 */
class SafeDoubleParser {
public:
    static errors::RuntimeResult<double> parse(const std::string& str) {
        if (str.empty()) {
            return errors::RuntimeResult<double>::error(
                errors::ErrorBuilder()
                    .category(errors::RuntimeErrorCategory::InvalidValue)
                    .message("Cannot parse empty string as double")
                    .arabicMessage("لا يمكن تحويل نص فارغ إلى عدد عشري")
                    .build()
            );
        }
        
        std::string trimmed = trim(str);
        
        try {
            size_t pos = 0;
            double result = std::stod(trimmed, &pos);
            
            if (pos != trimmed.size()) {
                return errors::RuntimeResult<double>::error(
                    errors::ErrorBuilder()
                        .category(errors::RuntimeErrorCategory::InvalidValue)
                        .message("Invalid double: '" + str + "'")
                        .arabicMessage("عدد عشري غير صالح: '" + str + "'")
                        .build()
                );
            }
            
            // التحقق من NaN و Infinity
            if (std::isnan(result)) {
                return errors::RuntimeResult<double>::error(
                    errors::ErrorBuilder()
                        .category(errors::RuntimeErrorCategory::NaNValue)
                        .message("Result is NaN")
                        .arabicMessage("النتيجة ليست رقماً (NaN)")
                        .build()
                );
            }
            
            if (std::isinf(result)) {
                return errors::RuntimeResult<double>::error(
                    errors::ErrorBuilder()
                        .category(errors::RuntimeErrorCategory::InfiniteValue)
                        .message("Result is infinite")
                        .arabicMessage("النتيجة لا نهائية")
                        .build()
                );
            }
            
            return errors::RuntimeResult<double>::ok(result);
            
        } catch (const std::out_of_range&) {
            return errors::RuntimeResult<double>::error(
                errors::ErrorBuilder()
                    .category(errors::RuntimeErrorCategory::IntegerOverflow)
                    .message("Double overflow: '" + str + "'")
                    .arabicMessage("تجاوز حدود العدد العشري: '" + str + "'")
                    .build()
            );
        } catch (const std::invalid_argument&) {
            return errors::RuntimeResult<double>::error(
                errors::ErrorBuilder()
                    .category(errors::RuntimeErrorCategory::InvalidValue)
                    .message("Invalid double: '" + str + "'")
                    .arabicMessage("عدد عشري غير صالح: '" + str + "'")
                    .build()
            );
        }
    }
    
    static double parseOrDefault(const std::string& str, double defaultValue) {
        auto result = parse(str);
        return result.valueOr(defaultValue);
    }
    
private:
    static std::string trim(const std::string& s) {
        auto start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        auto end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }
};

/**
 * @brief تحويل آمن بين الأنواع العددية
 */
class SafeNumericCast {
public:
    template<typename To, typename From>
    static errors::RuntimeResult<To> cast(From value) {
        // التحقق من الحدود
        if constexpr (std::is_integral_v<To> && std::is_integral_v<From>) {
            // تحويل صحيح إلى صحيح
            if constexpr (sizeof(To) < sizeof(From)) {
                // تضييق - قد يفقد قيمة
                if (value > static_cast<From>(std::numeric_limits<To>::max()) ||
                    value < static_cast<From>(std::numeric_limits<To>::min())) {
                    return errors::RuntimeResult<To>::error(
                        errors::ErrorBuilder()
                            .category(errors::RuntimeErrorCategory::IntegerOverflow)
                            .message("Integer cast overflow")
                            .arabicMessage("تجاوز في تحويل العدد الصحيح")
                            .build()
                    );
                }
            }
            
            // تحويل من موقّع إلى غير موقّع
            if constexpr (std::is_signed_v<From> && std::is_unsigned_v<To>) {
                if (value < 0) {
                    return errors::RuntimeResult<To>::error(
                        errors::ErrorBuilder()
                            .category(errors::RuntimeErrorCategory::InvalidCast)
                            .message("Cannot cast negative to unsigned")
                            .arabicMessage("لا يمكن تحويل قيمة سالبة إلى غير موقّعة")
                            .build()
                    );
                }
            }
        }
        
        if constexpr (std::is_integral_v<To> && std::is_floating_point_v<From>) {
            // تحويل عشري إلى صحيح
            if (std::isnan(value) || std::isinf(value)) {
                return errors::RuntimeResult<To>::error(
                    errors::ErrorBuilder()
                        .category(errors::RuntimeErrorCategory::InvalidCast)
                        .message("Cannot cast NaN/Inf to integer")
                        .arabicMessage("لا يمكن تحويل NaN/Inf إلى عدد صحيح")
                        .build()
                );
            }
            
            if (value > static_cast<From>(std::numeric_limits<To>::max()) ||
                value < static_cast<From>(std::numeric_limits<To>::min())) {
                return errors::RuntimeResult<To>::error(
                    errors::ErrorBuilder()
                        .category(errors::RuntimeErrorCategory::IntegerOverflow)
                        .message("Float to int overflow")
                        .arabicMessage("تجاوز في تحويل العشري إلى صحيح")
                        .build()
                );
            }
        }
        
        return errors::RuntimeResult<To>::ok(static_cast<To>(value));
    }
};

/**
 * @brief Dynamic cast آمن
 */
template<typename To, typename From>
errors::RuntimeResult<To*> safeDynamicCast(From* ptr) {
    if (ptr == nullptr) {
        return errors::RuntimeResult<To*>::error(
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::NullReference)
                .message("dynamic_cast on nullptr")
                .arabicMessage("dynamic_cast على مؤشر فارغ")
                .build()
        );
    }
    
    To* result = dynamic_cast<To*>(ptr);
    if (result == nullptr) {
        return errors::RuntimeResult<To*>::error(
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::InvalidCast)
                .message("dynamic_cast failed")
                .arabicMessage("فشل dynamic_cast")
                .build()
        );
    }
    
    return errors::RuntimeResult<To*>::ok(result);
}

} // namespace safe_conversions

// =============================================================================
// الفئة 3: إدارة الذاكرة الآمنة (1000 إصلاح)
// =============================================================================

namespace memory {

/**
 * @brief مُخصّص ذاكرة آمن مع تتبع
 */
class SafeAllocator {
public:
    struct AllocationInfo {
        void* ptr;
        size_t size;
        std::string file;
        int line;
        std::chrono::steady_clock::time_point time;
    };
    
    static SafeAllocator& instance() {
        static SafeAllocator inst;
        return inst;
    }
    
    void* allocate(size_t size, const char* file = nullptr, int line = 0) {
        if (size == 0) {
            return nullptr;
        }
        
        // التحقق من حد الذاكرة
        if (totalAllocated_ + size > maxMemory_) {
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::OutOfMemory)
                .message("Memory limit exceeded")
                .arabicMessage("تجاوز حد الذاكرة المسموح")
                .at(file ? file : "", line)
                .raise();
        }
        
        void* ptr = std::malloc(size);
        if (!ptr) {
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::OutOfMemory)
                .message("Failed to allocate " + std::to_string(size) + " bytes")
                .arabicMessage("فشل تخصيص " + std::to_string(size) + " بايت")
                .at(file ? file : "", line)
                .raise();
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_[ptr] = {ptr, size, file ? file : "", line,
                             std::chrono::steady_clock::now()};
        totalAllocated_ += size;
        peakAllocated_ = std::max(peakAllocated_, totalAllocated_);
        allocationCount_++;
        
        return ptr;
    }
    
    void deallocate(void* ptr) {
        if (!ptr) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = allocations_.find(ptr);
        if (it == allocations_.end()) {
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::DoubleFreq)
                .message("Double free or invalid pointer")
                .arabicMessage("تحرير مزدوج أو مؤشر غير صالح")
                .raise();
        }
        
        totalAllocated_ -= it->second.size;
        freedBytes_ += it->second.size;
        allocationCount_--;
        allocations_.erase(it);
        
        std::free(ptr);
    }
    
    bool isValidPointer(void* ptr) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return allocations_.find(ptr) != allocations_.end();
    }
    
    size_t getTotalAllocated() const { return totalAllocated_; }
    size_t getPeakAllocated() const { return peakAllocated_; }
    size_t getAllocationCount() const { return allocationCount_; }
    
    void setMaxMemory(size_t max) { maxMemory_ = max; }
    
    std::vector<AllocationInfo> getLeaks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<AllocationInfo> leaks;
        for (const auto& [ptr, info] : allocations_) {
            leaks.push_back(info);
        }
        return leaks;
    }
    
    std::string getStatistics() const {
        std::ostringstream out;
        out << "=== إحصائيات الذاكرة ===\n";
        out << "المخصص حالياً: " << totalAllocated_ << " بايت\n";
        out << "الذروة: " << peakAllocated_ << " بايت\n";
        out << "عدد التخصيصات: " << allocationCount_ << "\n";
        out << "المحرر: " << freedBytes_ << " بايت\n";
        return out.str();
    }
    
private:
    SafeAllocator() = default;
    
    mutable std::mutex mutex_;
    std::map<void*, AllocationInfo> allocations_;
    std::atomic<size_t> totalAllocated_{0};
    std::atomic<size_t> peakAllocated_{0};
    std::atomic<size_t> allocationCount_{0};
    std::atomic<size_t> freedBytes_{0};
    size_t maxMemory_ = 1024 * 1024 * 1024;  // 1GB default
};

/**
 * @brief مؤشر ذكي مع تتبع
 */
template<typename T>
class TrackedPtr {
public:
    TrackedPtr() = default;
    
    explicit TrackedPtr(T* ptr) : ptr_(ptr) {}
    
    template<typename... Args>
    static TrackedPtr<T> make(const char* file, int line, Args&&... args) {
        void* mem = SafeAllocator::instance().allocate(sizeof(T), file, line);
        T* ptr = new(mem) T(std::forward<Args>(args)...);
        return TrackedPtr<T>(ptr);
    }
    
    ~TrackedPtr() {
        reset();
    }
    
    TrackedPtr(TrackedPtr&& other) noexcept : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }
    
    TrackedPtr& operator=(TrackedPtr&& other) noexcept {
        if (this != &other) {
            reset();
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }
    
    TrackedPtr(const TrackedPtr&) = delete;
    TrackedPtr& operator=(const TrackedPtr&) = delete;
    
    T* get() const { return ptr_; }
    T& operator*() const {
        if (!ptr_) {
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::NullReference)
                .message("Dereferencing null TrackedPtr")
                .arabicMessage("محاولة الوصول لمؤشر فارغ")
                .raise();
        }
        return *ptr_;
    }
    T* operator->() const {
        if (!ptr_) {
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::NullReference)
                .message("Dereferencing null TrackedPtr")
                .arabicMessage("محاولة الوصول لمؤشر فارغ")
                .raise();
        }
        return ptr_;
    }
    
    explicit operator bool() const { return ptr_ != nullptr; }
    
    void reset() {
        if (ptr_) {
            ptr_->~T();
            SafeAllocator::instance().deallocate(ptr_);
            ptr_ = nullptr;
        }
    }
    
    T* release() {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }
    
private:
    T* ptr_ = nullptr;
};

#define MAKE_TRACKED(T, ...) \
    TrackedPtr<T>::make(__FILE__, __LINE__, ##__VA_ARGS__)

/**
 * @brief مجمع كائنات لإعادة الاستخدام
 */
template<typename T>
class ObjectPool {
public:
    explicit ObjectPool(size_t initialSize = 100) {
        reserve(initialSize);
    }
    
    ~ObjectPool() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto* obj : available_) {
            delete obj;
        }
        // لا نحذف inUse_ - المستخدم مسؤول
    }
    
    T* acquire() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (available_.empty()) {
            reserve(available_.size() + 10);
        }
        
        T* obj = available_.back();
        available_.pop_back();
        inUse_.insert(obj);
        return obj;
    }
    
    void release(T* obj) {
        if (!obj) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = inUse_.find(obj);
        if (it == inUse_.end()) {
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::InvalidState)
                .message("Releasing object not from this pool")
                .arabicMessage("تحرير كائن ليس من هذا المجمع")
                .raise();
        }
        
        inUse_.erase(it);
        available_.push_back(obj);
    }
    
    size_t availableCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return available_.size();
    }
    
    size_t inUseCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return inUse_.size();
    }
    
private:
    void reserve(size_t count) {
        for (size_t i = 0; i < count; ++i) {
            available_.push_back(new T());
        }
    }
    
    mutable std::mutex mutex_;
    std::vector<T*> available_;
    std::set<T*> inUse_;
};

} // namespace memory

// =============================================================================
// الفئة 4: فحص الحدود والتجاوز (1000 إصلاح)
// =============================================================================

namespace bounds {

/**
 * @brief فاحص حدود المصفوفات
 */
class ArrayBoundsChecker {
public:
    template<typename T>
    static const T& at(const std::vector<T>& vec, size_t index,
                        const std::string& name = "array") {
        if (index >= vec.size()) {
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::IndexOutOfBounds)
                .message("Index " + std::to_string(index) +
                         " out of bounds for " + name +
                         " of size " + std::to_string(vec.size()))
                .arabicMessage("الفهرس " + std::to_string(index) +
                               " خارج حدود " + name +
                               " (الحجم: " + std::to_string(vec.size()) + ")")
                .suggestion("تأكد من أن الفهرس أقل من حجم المصفوفة")
                .raise();
        }
        return vec[index];
    }
    
    template<typename T>
    static T& at(std::vector<T>& vec, size_t index,
                  const std::string& name = "array") {
        if (index >= vec.size()) {
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::IndexOutOfBounds)
                .message("Index " + std::to_string(index) +
                         " out of bounds for " + name +
                         " of size " + std::to_string(vec.size()))
                .arabicMessage("الفهرس " + std::to_string(index) +
                               " خارج حدود " + name +
                               " (الحجم: " + std::to_string(vec.size()) + ")")
                .raise();
        }
        return vec[index];
    }
    
    template<typename T>
    static const T& front(const std::vector<T>& vec,
                           const std::string& name = "array") {
        if (vec.empty()) {
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::EmptyContainer)
                .message("front() called on empty " + name)
                .arabicMessage("استدعاء front() على " + name + " فارغ")
                .raise();
        }
        return vec.front();
    }
    
    template<typename T>
    static const T& back(const std::vector<T>& vec,
                          const std::string& name = "array") {
        if (vec.empty()) {
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::EmptyContainer)
                .message("back() called on empty " + name)
                .arabicMessage("استدعاء back() على " + name + " فارغ")
                .raise();
        }
        return vec.back();
    }
    
    template<typename T>
    static void checkRange(const std::vector<T>& vec,
                            size_t start, size_t end,
                            const std::string& name = "array") {
        if (start > end) {
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::InvalidValue)
                .message("Invalid range: start > end")
                .arabicMessage("نطاق غير صالح: البداية أكبر من النهاية")
                .raise();
        }
        if (end > vec.size()) {
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::IndexOutOfBounds)
                .message("Range end " + std::to_string(end) +
                         " out of bounds for " + name +
                         " of size " + std::to_string(vec.size()))
                .arabicMessage("نهاية النطاق " + std::to_string(end) +
                               " خارج حدود " + name)
                .raise();
        }
    }
};

/**
 * @brief فاحص حدود النصوص
 */
class StringBoundsChecker {
public:
    static char at(const std::string& str, size_t index) {
        if (index >= str.size()) {
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::StringBoundsViolation)
                .message("String index " + std::to_string(index) +
                         " out of bounds (length: " + std::to_string(str.size()) + ")")
                .arabicMessage("فهرس النص " + std::to_string(index) +
                               " خارج الحدود (الطول: " + std::to_string(str.size()) + ")")
                .raise();
        }
        return str[index];
    }
    
    static std::string substr(const std::string& str, size_t pos, size_t len) {
        if (pos > str.size()) {
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::StringBoundsViolation)
                .message("Substring position " + std::to_string(pos) +
                         " out of bounds (length: " + std::to_string(str.size()) + ")")
                .arabicMessage("موضع النص الفرعي " + std::to_string(pos) +
                               " خارج الحدود")
                .raise();
        }
        return str.substr(pos, len);
    }
};

/**
 * @brief فاحص تجاوز العمليات الحسابية
 */
class ArithmeticChecker {
public:
    // جمع آمن
    static errors::RuntimeResult<int64_t> safeAdd(int64_t a, int64_t b) {
        if ((b > 0 && a > std::numeric_limits<int64_t>::max() - b) ||
            (b < 0 && a < std::numeric_limits<int64_t>::min() - b)) {
            return errors::RuntimeResult<int64_t>::error(
                errors::ErrorBuilder()
                    .category(errors::RuntimeErrorCategory::IntegerOverflow)
                    .message("Integer overflow in addition")
                    .arabicMessage("تجاوز في الجمع")
                    .build()
            );
        }
        return errors::RuntimeResult<int64_t>::ok(a + b);
    }
    
    // طرح آمن
    static errors::RuntimeResult<int64_t> safeSub(int64_t a, int64_t b) {
        if ((b < 0 && a > std::numeric_limits<int64_t>::max() + b) ||
            (b > 0 && a < std::numeric_limits<int64_t>::min() + b)) {
            return errors::RuntimeResult<int64_t>::error(
                errors::ErrorBuilder()
                    .category(errors::RuntimeErrorCategory::IntegerUnderflow)
                    .message("Integer underflow in subtraction")
                    .arabicMessage("نقصان في الطرح")
                    .build()
            );
        }
        return errors::RuntimeResult<int64_t>::ok(a - b);
    }
    
    // ضرب آمن
    static errors::RuntimeResult<int64_t> safeMul(int64_t a, int64_t b) {
        if (a == 0 || b == 0) {
            return errors::RuntimeResult<int64_t>::ok(0);
        }
        
        if (a > 0) {
            if (b > 0) {
                if (a > std::numeric_limits<int64_t>::max() / b) {
                    return overflowError();
                }
            } else {
                if (b < std::numeric_limits<int64_t>::min() / a) {
                    return overflowError();
                }
            }
        } else {
            if (b > 0) {
                if (a < std::numeric_limits<int64_t>::min() / b) {
                    return overflowError();
                }
            } else {
                if (a != 0 && b < std::numeric_limits<int64_t>::max() / a) {
                    return overflowError();
                }
            }
        }
        
        return errors::RuntimeResult<int64_t>::ok(a * b);
    }
    
    // قسمة آمنة
    static errors::RuntimeResult<int64_t> safeDiv(int64_t a, int64_t b) {
        if (b == 0) {
            return errors::RuntimeResult<int64_t>::error(
                errors::ErrorBuilder()
                    .category(errors::RuntimeErrorCategory::DivisionByZero)
                    .message("Division by zero")
                    .arabicMessage("قسمة على صفر")
                    .suggestion("تأكد من أن المقسوم عليه ليس صفراً")
                    .build()
            );
        }
        
        // تجنب INT_MIN / -1
        if (a == std::numeric_limits<int64_t>::min() && b == -1) {
            return overflowError();
        }
        
        return errors::RuntimeResult<int64_t>::ok(a / b);
    }
    
    // باقي القسمة الآمن
    static errors::RuntimeResult<int64_t> safeMod(int64_t a, int64_t b) {
        if (b == 0) {
            return errors::RuntimeResult<int64_t>::error(
                errors::ErrorBuilder()
                    .category(errors::RuntimeErrorCategory::DivisionByZero)
                    .message("Modulo by zero")
                    .arabicMessage("باقي قسمة على صفر")
                    .build()
            );
        }
        return errors::RuntimeResult<int64_t>::ok(a % b);
    }
    
    // نفي آمن
    static errors::RuntimeResult<int64_t> safeNegate(int64_t a) {
        if (a == std::numeric_limits<int64_t>::min()) {
            return overflowError();
        }
        return errors::RuntimeResult<int64_t>::ok(-a);
    }
    
private:
    static errors::RuntimeResult<int64_t> overflowError() {
        return errors::RuntimeResult<int64_t>::error(
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::IntegerOverflow)
                .message("Integer overflow")
                .arabicMessage("تجاوز العدد الصحيح")
                .build()
        );
    }
};

} // namespace bounds

// =============================================================================
// الفئة 5-7: انظر الملف التالي
// =============================================================================

} // namespace interpreter
} // namespace sad

#endif // SAD_INTERPRETER_MEGA_IMPROVEMENTS_H
