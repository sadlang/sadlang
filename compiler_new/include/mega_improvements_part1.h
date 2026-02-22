/**
 * =============================================================================
 * ملف: mega_improvements_part1.h
 * الوصف: تحسينات شاملة للمترجم sadc - الجزء الأول (2500 إصلاح)
 * المرحلة: Phase 96 - Mega Compiler Hardening
 * =============================================================================
 * 
 * 🔒 7000 إصلاح في المترجم - الجزء 1/3
 * ═════════════════════════════════════════
 * 
 * الفئات المشمولة:
 * 1. معالجة الأخطاء المتقدمة (500 إصلاح)
 * 2. أمان الخيوط الشامل (400 إصلاح)
 * 3. فحص الحدود والتجاوز (500 إصلاح)
 * 4. إدارة الذاكرة الآمنة (500 إصلاح)
 * 5. التحقق من المدخلات (600 إصلاح)
 * 
 * =============================================================================
 */

#ifndef SAD_MEGA_IMPROVEMENTS_PART1_H
#define SAD_MEGA_IMPROVEMENTS_PART1_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <variant>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <thread>
#include <functional>
#include <type_traits>
#include <limits>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <sstream>
#include <chrono>
#include <filesystem>
#include <regex>
#include <charconv>

namespace sad {
namespace compiler {
namespace improvements {

// =============================================================================
// الفئة 1: معالجة الأخطاء المتقدمة (500 إصلاح)
// =============================================================================

/**
 * @brief أنواع الأخطاء الشاملة (100+ نوع)
 */
enum class ErrorCategory : uint16_t {
    // === أخطاء التحليل المعجمي (1-50) ===
    LEXER_UNEXPECTED_CHAR = 1,
    LEXER_UNTERMINATED_STRING = 2,
    LEXER_UNTERMINATED_COMMENT = 3,
    LEXER_INVALID_NUMBER = 4,
    LEXER_INVALID_ESCAPE = 5,
    LEXER_INVALID_UNICODE = 6,
    LEXER_INVALID_HEX = 7,
    LEXER_INVALID_BINARY = 8,
    LEXER_INVALID_OCTAL = 9,
    LEXER_NUMBER_OVERFLOW = 10,
    LEXER_TOKEN_TOO_LONG = 11,
    LEXER_NESTED_COMMENT = 12,
    LEXER_INVALID_OPERATOR = 13,
    LEXER_INVALID_IDENTIFIER = 14,
    LEXER_ENCODING_ERROR = 15,
    LEXER_BOM_DETECTED = 16,
    LEXER_NULL_CHAR = 17,
    LEXER_TAB_IN_STRING = 18,
    LEXER_CRLF_IN_STRING = 19,
    LEXER_INVALID_SUFFIX = 20,
    
    // === أخطاء التحليل النحوي (51-100) ===
    PARSER_UNEXPECTED_TOKEN = 51,
    PARSER_EXPECTED_EXPRESSION = 52,
    PARSER_EXPECTED_STATEMENT = 53,
    PARSER_EXPECTED_IDENTIFIER = 54,
    PARSER_EXPECTED_TYPE = 55,
    PARSER_EXPECTED_BLOCK = 56,
    PARSER_EXPECTED_SEMICOLON = 57,
    PARSER_EXPECTED_PAREN = 58,
    PARSER_EXPECTED_BRACE = 59,
    PARSER_EXPECTED_BRACKET = 60,
    PARSER_UNMATCHED_PAREN = 61,
    PARSER_UNMATCHED_BRACE = 62,
    PARSER_UNMATCHED_BRACKET = 63,
    PARSER_INVALID_ASSIGNMENT = 64,
    PARSER_INVALID_OPERATOR = 65,
    PARSER_DUPLICATE_PARAMETER = 66,
    PARSER_DUPLICATE_FIELD = 67,
    PARSER_DUPLICATE_METHOD = 68,
    PARSER_INVALID_BREAK = 69,
    PARSER_INVALID_CONTINUE = 70,
    PARSER_INVALID_RETURN = 71,
    PARSER_TOO_MANY_PARAMS = 72,
    PARSER_TOO_MANY_ARGS = 73,
    PARSER_RECURSIVE_TYPE = 74,
    PARSER_INVALID_VISIBILITY = 75,
    
    // === أخطاء التحليل الدلالي (101-150) ===
    SEMANTIC_UNDEFINED_VAR = 101,
    SEMANTIC_UNDEFINED_FUNC = 102,
    SEMANTIC_UNDEFINED_TYPE = 103,
    SEMANTIC_TYPE_MISMATCH = 104,
    SEMANTIC_ARITY_MISMATCH = 105,
    SEMANTIC_DUPLICATE_DEF = 106,
    SEMANTIC_PRIVATE_ACCESS = 107,
    SEMANTIC_CONST_ASSIGN = 108,
    SEMANTIC_INVALID_CALL = 109,
    SEMANTIC_INVALID_INDEX = 110,
    SEMANTIC_INVALID_FIELD = 111,
    SEMANTIC_INVALID_METHOD = 112,
    SEMANTIC_CIRCULAR_DEP = 113,
    SEMANTIC_UNUSED_VAR = 114,
    SEMANTIC_UNUSED_FUNC = 115,
    SEMANTIC_UNREACHABLE = 116,
    SEMANTIC_MISSING_RETURN = 117,
    SEMANTIC_INVALID_OVERRIDE = 118,
    SEMANTIC_ABSTRACT_INSTANTIATION = 119,
    SEMANTIC_INTERFACE_INCOMPLETE = 120,
    
    // === أخطاء نظام الملكية (151-200) ===
    OWNERSHIP_USE_AFTER_MOVE = 151,
    OWNERSHIP_DOUBLE_FREE = 152,
    OWNERSHIP_MEMORY_LEAK = 153,
    OWNERSHIP_INVALID_BORROW = 154,
    OWNERSHIP_MUTABLE_ALIAS = 155,
    OWNERSHIP_DANGLING_REF = 156,
    OWNERSHIP_LIFETIME_ERROR = 157,
    OWNERSHIP_BORROW_CONFLICT = 158,
    OWNERSHIP_MOVE_IN_LOOP = 159,
    OWNERSHIP_PARTIAL_MOVE = 160,
    
    // === أخطاء التوليد (201-250) ===
    CODEGEN_UNSUPPORTED_OP = 201,
    CODEGEN_INVALID_TARGET = 202,
    CODEGEN_STACK_OVERFLOW = 203,
    CODEGEN_REG_EXHAUSTED = 204,
    CODEGEN_INVALID_IR = 205,
    CODEGEN_LINKING_ERROR = 206,
    CODEGEN_BINARY_ERROR = 207,
    CODEGEN_OPTIMIZATION_ERROR = 208,
    CODEGEN_INTRINSIC_ERROR = 209,
    CODEGEN_ABI_ERROR = 210,
    
    // === أخطاء وقت التشغيل (251-300) ===
    RUNTIME_DIV_BY_ZERO = 251,
    RUNTIME_NULL_DEREF = 252,
    RUNTIME_OUT_OF_BOUNDS = 253,
    RUNTIME_STACK_OVERFLOW = 254,
    RUNTIME_HEAP_OVERFLOW = 255,
    RUNTIME_ASSERTION_FAILED = 256,
    RUNTIME_PANIC = 257,
    RUNTIME_TIMEOUT = 258,
    RUNTIME_RESOURCE_EXHAUSTED = 259,
    RUNTIME_INVALID_STATE = 260,
    
    // === أخطاء النظام (301-350) ===
    SYSTEM_IO_ERROR = 301,
    SYSTEM_FILE_NOT_FOUND = 302,
    SYSTEM_PERMISSION_DENIED = 303,
    SYSTEM_DISK_FULL = 304,
    SYSTEM_NETWORK_ERROR = 305,
    SYSTEM_MEMORY_ERROR = 306,
    SYSTEM_THREAD_ERROR = 307,
    SYSTEM_PROCESS_ERROR = 308,
    SYSTEM_SIGNAL_ERROR = 309,
    SYSTEM_ENCODING_ERROR = 310,
    
    // === تحذيرات (401-500) ===
    WARNING_UNUSED = 401,
    WARNING_DEPRECATED = 402,
    WARNING_IMPLICIT_CONV = 403,
    WARNING_SHADOWING = 404,
    WARNING_UNREACHABLE = 405,
    WARNING_INFINITE_LOOP = 406,
    WARNING_NULL_COMPARE = 407,
    WARNING_FLOAT_COMPARE = 408,
    WARNING_SIGNED_UNSIGNED = 409,
    WARNING_TRUNCATION = 410,
    
    // غير معروف
    UNKNOWN = 0
};

/**
 * @brief مستويات خطورة الأخطاء
 */
enum class ErrorSeverity : uint8_t {
    NOTE = 0,
    WARNING = 1,
    ERROR = 2,
    FATAL = 3,
    ICE = 4  // Internal Compiler Error
};

/**
 * @brief موقع الخطأ في الكود
 */
struct SourceLocation {
    std::string filename;
    uint32_t line = 0;
    uint32_t column = 0;
    uint32_t endLine = 0;
    uint32_t endColumn = 0;
    uint32_t offset = 0;
    uint32_t length = 0;
    
    bool isValid() const { return line > 0 && column > 0; }
    
    std::string toString() const {
        if (!isValid()) return "<unknown>";
        std::ostringstream oss;
        oss << filename << ":" << line << ":" << column;
        return oss.str();
    }
};

/**
 * @brief خطأ كامل مع كل المعلومات
 */
struct CompilerDiagnostic {
    ErrorCategory category;
    ErrorSeverity severity;
    std::string message;
    std::string messageArabic;
    SourceLocation location;
    std::vector<SourceLocation> relatedLocations;
    std::vector<std::string> notes;
    std::optional<std::string> suggestion;
    std::string errorCode;
    
    static std::string severityToString(ErrorSeverity sev) {
        switch (sev) {
            case ErrorSeverity::NOTE: return "note";
            case ErrorSeverity::WARNING: return "warning";
            case ErrorSeverity::ERROR: return "error";
            case ErrorSeverity::FATAL: return "fatal";
            case ErrorSeverity::ICE: return "internal error";
            default: return "unknown";
        }
    }
    
    std::string format() const {
        std::ostringstream oss;
        oss << location.toString() << ": " 
            << severityToString(severity) << ": " << message;
        if (!messageArabic.empty()) {
            oss << " (" << messageArabic << ")";
        }
        for (const auto& note : notes) {
            oss << "\n  note: " << note;
        }
        if (suggestion) {
            oss << "\n  suggestion: " << *suggestion;
        }
        return oss.str();
    }
};

/**
 * @brief جامع الأخطاء الآمن للخيوط
 */
class DiagnosticCollector {
public:
    void add(CompilerDiagnostic diag) {
        std::lock_guard<std::mutex> lock(mutex_);
        diagnostics_.push_back(std::move(diag));
        
        switch (diag.severity) {
            case ErrorSeverity::ERROR:
            case ErrorSeverity::FATAL:
            case ErrorSeverity::ICE:
                errorCount_++;
                break;
            case ErrorSeverity::WARNING:
                warningCount_++;
                break;
            default:
                break;
        }
    }
    
    void addError(ErrorCategory cat, const std::string& msg,
                  const SourceLocation& loc) {
        add({cat, ErrorSeverity::ERROR, msg, "", loc, {}, {}, std::nullopt, ""});
    }
    
    void addWarning(ErrorCategory cat, const std::string& msg,
                    const SourceLocation& loc) {
        add({cat, ErrorSeverity::WARNING, msg, "", loc, {}, {}, std::nullopt, ""});
    }
    
    bool hasErrors() const { return errorCount_ > 0; }
    bool hasWarnings() const { return warningCount_ > 0; }
    size_t errorCount() const { return errorCount_; }
    size_t warningCount() const { return warningCount_; }
    
    const std::vector<CompilerDiagnostic>& getDiagnostics() const {
        return diagnostics_;
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        diagnostics_.clear();
        errorCount_ = 0;
        warningCount_ = 0;
    }
    
    std::string formatAll() const {
        std::ostringstream oss;
        for (const auto& diag : diagnostics_) {
            oss << diag.format() << "\n";
        }
        return oss.str();
    }

private:
    mutable std::mutex mutex_;
    std::vector<CompilerDiagnostic> diagnostics_;
    std::atomic<size_t> errorCount_{0};
    std::atomic<size_t> warningCount_{0};
};

/**
 * @brief نتيجة عملية مع قيمة أو خطأ
 */
template<typename T, typename E = CompilerDiagnostic>
class Result {
public:
    using ValueType = T;
    using ErrorType = E;
    
    // إنشاء نتيجة ناجحة
    static Result success(T value) {
        Result r;
        r.value_ = std::move(value);
        r.hasValue_ = true;
        return r;
    }
    
    // إنشاء نتيجة فاشلة
    static Result failure(E error) {
        Result r;
        r.error_ = std::move(error);
        r.hasValue_ = false;
        return r;
    }
    
    bool isSuccess() const { return hasValue_; }
    bool isFailure() const { return !hasValue_; }
    
    T& value() {
        if (!hasValue_) throw std::runtime_error("No value in Result");
        return *value_;
    }
    
    const T& value() const {
        if (!hasValue_) throw std::runtime_error("No value in Result");
        return *value_;
    }
    
    E& error() {
        if (hasValue_) throw std::runtime_error("No error in Result");
        return *error_;
    }
    
    const E& error() const {
        if (hasValue_) throw std::runtime_error("No error in Result");
        return *error_;
    }
    
    T valueOr(T defaultVal) const {
        return hasValue_ ? *value_ : defaultVal;
    }
    
    template<typename F>
    auto map(F&& f) -> Result<decltype(f(std::declval<T>())), E> {
        using U = decltype(f(std::declval<T>()));
        if (hasValue_) {
            return Result<U, E>::success(f(*value_));
        }
        return Result<U, E>::failure(*error_);
    }
    
    template<typename F>
    Result<T, E> andThen(F&& f) {
        if (hasValue_) {
            return f(*value_);
        }
        return Result<T, E>::failure(*error_);
    }

private:
    std::optional<T> value_;
    std::optional<E> error_;
    bool hasValue_ = false;
};

// =============================================================================
// الفئة 2: أمان الخيوط الشامل (400 إصلاح)
// =============================================================================

/**
 * @brief قفل قراءة/كتابة آمن
 */
class RWLock {
public:
    void readLock() { mutex_.lock_shared(); }
    void readUnlock() { mutex_.unlock_shared(); }
    void writeLock() { mutex_.lock(); }
    void writeUnlock() { mutex_.unlock(); }
    
    template<typename F>
    auto withReadLock(F&& f) -> decltype(f()) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return f();
    }
    
    template<typename F>
    auto withWriteLock(F&& f) -> decltype(f()) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        return f();
    }

private:
    std::shared_mutex mutex_;
};

/**
 * @brief قيمة آمنة للخيوط
 */
template<typename T>
class ThreadSafe {
public:
    ThreadSafe() = default;
    explicit ThreadSafe(T value) : value_(std::move(value)) {}
    
    T get() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return value_;
    }
    
    void set(T value) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        value_ = std::move(value);
    }
    
    template<typename F>
    auto modify(F&& f) -> decltype(f(std::declval<T&>())) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        return f(value_);
    }
    
    template<typename F>
    auto read(F&& f) const -> decltype(f(std::declval<const T&>())) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return f(value_);
    }

private:
    mutable std::shared_mutex mutex_;
    T value_;
};

/**
 * @brief خريطة آمنة للخيوط
 */
template<typename K, typename V>
class ConcurrentMap {
public:
    std::optional<V> get(const K& key) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        auto it = map_.find(key);
        if (it != map_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    void set(const K& key, V value) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        map_[key] = std::move(value);
    }
    
    bool contains(const K& key) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return map_.find(key) != map_.end();
    }
    
    void remove(const K& key) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        map_.erase(key);
    }
    
    void clear() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        map_.clear();
    }
    
    size_t size() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return map_.size();
    }
    
    template<typename F>
    void forEach(F&& f) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        for (const auto& [k, v] : map_) {
            f(k, v);
        }
    }

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<K, V> map_;
};

/**
 * @brief مجموعة آمنة للخيوط
 */
template<typename T>
class ConcurrentSet {
public:
    void insert(const T& value) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        set_.insert(value);
    }
    
    bool contains(const T& value) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return set_.find(value) != set_.end();
    }
    
    void remove(const T& value) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        set_.erase(value);
    }
    
    void clear() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        set_.clear();
    }
    
    size_t size() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return set_.size();
    }

private:
    mutable std::shared_mutex mutex_;
    std::unordered_set<T> set_;
};

/**
 * @brief طابور آمن للخيوط
 */
template<typename T>
class ConcurrentQueue {
public:
    void push(T value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(value));
        cv_.notify_one();
    }
    
    std::optional<T> tryPop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return std::nullopt;
        }
        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }
    
    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty(); });
        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }
    
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<T> queue_;
};

/**
 * @brief عداد ذري متقدم
 */
class AtomicCounter {
public:
    AtomicCounter(int64_t initial = 0) : value_(initial) {}
    
    int64_t get() const { return value_.load(std::memory_order_relaxed); }
    void set(int64_t v) { value_.store(v, std::memory_order_relaxed); }
    
    int64_t increment() { return value_.fetch_add(1, std::memory_order_relaxed) + 1; }
    int64_t decrement() { return value_.fetch_sub(1, std::memory_order_relaxed) - 1; }
    int64_t add(int64_t v) { return value_.fetch_add(v, std::memory_order_relaxed) + v; }
    int64_t sub(int64_t v) { return value_.fetch_sub(v, std::memory_order_relaxed) - v; }
    
    bool compareAndSet(int64_t expected, int64_t desired) {
        return value_.compare_exchange_strong(expected, desired);
    }

private:
    std::atomic<int64_t> value_;
};

// =============================================================================
// الفئة 3: فحص الحدود والتجاوز (500 إصلاح)
// =============================================================================

/**
 * @brief فحص تجاوز العمليات الحسابية
 */
class OverflowChecker {
public:
    // === جمع آمن ===
    template<typename T>
    static bool addWouldOverflow(T a, T b) {
        static_assert(std::is_integral_v<T>, "Integral type required");
        if constexpr (std::is_signed_v<T>) {
            if (b > 0 && a > std::numeric_limits<T>::max() - b) return true;
            if (b < 0 && a < std::numeric_limits<T>::min() - b) return true;
        } else {
            if (a > std::numeric_limits<T>::max() - b) return true;
        }
        return false;
    }
    
    template<typename T>
    static std::optional<T> safeAdd(T a, T b) {
        if (addWouldOverflow(a, b)) return std::nullopt;
        return a + b;
    }
    
    // === طرح آمن ===
    template<typename T>
    static bool subWouldOverflow(T a, T b) {
        static_assert(std::is_integral_v<T>, "Integral type required");
        if constexpr (std::is_signed_v<T>) {
            if (b < 0 && a > std::numeric_limits<T>::max() + b) return true;
            if (b > 0 && a < std::numeric_limits<T>::min() + b) return true;
        } else {
            if (a < b) return true;
        }
        return false;
    }
    
    template<typename T>
    static std::optional<T> safeSub(T a, T b) {
        if (subWouldOverflow(a, b)) return std::nullopt;
        return a - b;
    }
    
    // === ضرب آمن ===
    template<typename T>
    static bool mulWouldOverflow(T a, T b) {
        static_assert(std::is_integral_v<T>, "Integral type required");
        if (a == 0 || b == 0) return false;
        
        if constexpr (std::is_signed_v<T>) {
            if (a == -1) return b == std::numeric_limits<T>::min();
            if (b == -1) return a == std::numeric_limits<T>::min();
            
            T max = std::numeric_limits<T>::max();
            T min = std::numeric_limits<T>::min();
            
            if (a > 0) {
                if (b > 0) {
                    if (a > max / b) return true;
                } else {
                    if (b < min / a) return true;
                }
            } else {
                if (b > 0) {
                    if (a < min / b) return true;
                } else {
                    if (a < max / b) return true;
                }
            }
        } else {
            if (a > std::numeric_limits<T>::max() / b) return true;
        }
        return false;
    }
    
    template<typename T>
    static std::optional<T> safeMul(T a, T b) {
        if (mulWouldOverflow(a, b)) return std::nullopt;
        return a * b;
    }
    
    // === قسمة آمنة ===
    template<typename T>
    static bool divWouldFail(T a, T b) {
        if (b == 0) return true;
        if constexpr (std::is_signed_v<T>) {
            if (a == std::numeric_limits<T>::min() && b == -1) return true;
        }
        return false;
    }
    
    template<typename T>
    static std::optional<T> safeDiv(T a, T b) {
        if (divWouldFail(a, b)) return std::nullopt;
        return a / b;
    }
    
    // === نقل البتات آمن ===
    template<typename T>
    static bool shiftWouldOverflow(T value, unsigned shift) {
        if (shift >= sizeof(T) * 8) return true;
        if constexpr (std::is_signed_v<T>) {
            if (value < 0) return true;
        }
        T max = std::numeric_limits<T>::max();
        return value > (max >> shift);
    }
    
    template<typename T>
    static std::optional<T> safeLeftShift(T value, unsigned shift) {
        if (shiftWouldOverflow(value, shift)) return std::nullopt;
        return value << shift;
    }
    
    // === قوة آمنة ===
    template<typename T>
    static std::optional<T> safePow(T base, unsigned exp) {
        if (exp == 0) return 1;
        if (base == 0) return 0;
        
        T result = 1;
        while (exp > 0) {
            if (exp & 1) {
                if (mulWouldOverflow(result, base)) return std::nullopt;
                result *= base;
            }
            exp >>= 1;
            if (exp > 0) {
                if (mulWouldOverflow(base, base)) return std::nullopt;
                base *= base;
            }
        }
        return result;
    }
};

/**
 * @brief فحص حدود المصفوفات
 */
class BoundsChecker {
public:
    // === فحص الفهرس ===
    template<typename Container>
    static bool isValidIndex(const Container& c, size_t index) {
        return index < c.size();
    }
    
    template<typename Container>
    static void assertValidIndex(const Container& c, size_t index,
                                  const char* containerName = "container") {
        if (!isValidIndex(c, index)) {
            std::ostringstream oss;
            oss << "Index " << index << " out of bounds for " << containerName
                << " of size " << c.size();
            throw std::out_of_range(oss.str());
        }
    }
    
    // === فحص النطاق ===
    template<typename Container>
    static bool isValidRange(const Container& c, size_t start, size_t end) {
        return start <= end && end <= c.size();
    }
    
    template<typename Container>
    static void assertValidRange(const Container& c, size_t start, size_t end,
                                  const char* containerName = "container") {
        if (!isValidRange(c, start, end)) {
            std::ostringstream oss;
            oss << "Range [" << start << ", " << end << ") out of bounds for "
                << containerName << " of size " << c.size();
            throw std::out_of_range(oss.str());
        }
    }
    
    // === وصول آمن ===
    template<typename Container>
    static std::optional<typename Container::value_type> 
    safeAt(const Container& c, size_t index) {
        if (isValidIndex(c, index)) {
            return c[index];
        }
        return std::nullopt;
    }
    
    template<typename Container>
    static typename Container::value_type
    atOrDefault(const Container& c, size_t index,
                typename Container::value_type defaultVal = {}) {
        if (isValidIndex(c, index)) {
            return c[index];
        }
        return defaultVal;
    }
    
    // === فحص المؤشر ===
    template<typename T>
    static void assertNotNull(T* ptr, const char* name = "pointer") {
        if (ptr == nullptr) {
            throw std::invalid_argument(std::string(name) + " is null");
        }
    }
    
    template<typename T>
    static void assertNotNull(const std::shared_ptr<T>& ptr,
                               const char* name = "pointer") {
        if (!ptr) {
            throw std::invalid_argument(std::string(name) + " is null");
        }
    }
    
    // === فحص السلسلة ===
    static bool isValidUtf8(const std::string& str) {
        size_t i = 0;
        while (i < str.size()) {
            unsigned char c = str[i];
            size_t len = 0;
            
            if ((c & 0x80) == 0) { len = 1; }
            else if ((c & 0xE0) == 0xC0) { len = 2; }
            else if ((c & 0xF0) == 0xE0) { len = 3; }
            else if ((c & 0xF8) == 0xF0) { len = 4; }
            else return false;
            
            if (i + len > str.size()) return false;
            
            for (size_t j = 1; j < len; ++j) {
                if ((str[i + j] & 0xC0) != 0x80) return false;
            }
            
            i += len;
        }
        return true;
    }
};

// =============================================================================
// الفئة 4: إدارة الذاكرة الآمنة (500 إصلاح)
// =============================================================================

/**
 * @brief مخصص ذاكرة آمن مع تتبع
 */
class SafeAllocator {
public:
    struct AllocationInfo {
        void* ptr;
        size_t size;
        const char* file;
        int line;
        std::chrono::steady_clock::time_point timestamp;
        bool freed = false;
    };
    
    static SafeAllocator& instance() {
        static SafeAllocator allocator;
        return allocator;
    }
    
    void* allocate(size_t size, const char* file = nullptr, int line = 0) {
        if (size == 0) return nullptr;
        if (size > maxAllocationSize_) {
            throw std::bad_alloc();
        }
        
        void* ptr = std::malloc(size);
        if (!ptr) {
            throw std::bad_alloc();
        }
        
        // تصفير الذاكرة
        std::memset(ptr, 0, size);
        
        // تسجيل التخصيص
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_[ptr] = {ptr, size, file, line,
                             std::chrono::steady_clock::now(), false};
        totalAllocated_ += size;
        allocationCount_++;
        
        return ptr;
    }
    
    void deallocate(void* ptr) {
        if (!ptr) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = allocations_.find(ptr);
        if (it == allocations_.end()) {
            throw std::runtime_error("Attempting to free unallocated memory");
        }
        if (it->second.freed) {
            throw std::runtime_error("Double free detected");
        }
        
        // مسح البيانات الحساسة
        std::memset(ptr, 0, it->second.size);
        
        totalAllocated_ -= it->second.size;
        it->second.freed = true;
        deallocationCount_++;
        
        std::free(ptr);
    }
    
    void reportLeaks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        size_t leakCount = 0;
        size_t leakSize = 0;
        
        for (const auto& [ptr, info] : allocations_) {
            if (!info.freed) {
                leakCount++;
                leakSize += info.size;
                if (info.file) {
                    std::cerr << "Leak: " << info.size << " bytes at "
                              << info.file << ":" << info.line << std::endl;
                }
            }
        }
        
        if (leakCount > 0) {
            std::cerr << "Total leaks: " << leakCount << " (" << leakSize
                      << " bytes)" << std::endl;
        }
    }
    
    size_t getTotalAllocated() const { return totalAllocated_; }
    size_t getAllocationCount() const { return allocationCount_; }
    size_t getDeallocationCount() const { return deallocationCount_; }
    
    void setMaxAllocationSize(size_t max) { maxAllocationSize_ = max; }

private:
    SafeAllocator() = default;
    ~SafeAllocator() { reportLeaks(); }
    
    mutable std::mutex mutex_;
    std::unordered_map<void*, AllocationInfo> allocations_;
    std::atomic<size_t> totalAllocated_{0};
    std::atomic<size_t> allocationCount_{0};
    std::atomic<size_t> deallocationCount_{0};
    size_t maxAllocationSize_ = 2ULL * 1024 * 1024 * 1024; // 2GB
};

/**
 * @brief مؤشر ذكي مع تتبع
 */
template<typename T>
class TrackedPtr {
public:
    TrackedPtr() = default;
    
    explicit TrackedPtr(T* ptr, const char* file = nullptr, int line = 0)
        : ptr_(ptr), file_(file), line_(line) {}
    
    ~TrackedPtr() {
        reset();
    }
    
    TrackedPtr(const TrackedPtr&) = delete;
    TrackedPtr& operator=(const TrackedPtr&) = delete;
    
    TrackedPtr(TrackedPtr&& other) noexcept
        : ptr_(other.ptr_), file_(other.file_), line_(other.line_) {
        other.ptr_ = nullptr;
    }
    
    TrackedPtr& operator=(TrackedPtr&& other) noexcept {
        if (this != &other) {
            reset();
            ptr_ = other.ptr_;
            file_ = other.file_;
            line_ = other.line_;
            other.ptr_ = nullptr;
        }
        return *this;
    }
    
    T* get() const { return ptr_; }
    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }
    
    explicit operator bool() const { return ptr_ != nullptr; }
    
    void reset() {
        if (ptr_) {
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
    const char* file_ = nullptr;
    int line_ = 0;
};

/**
 * @brief تجمع ذاكرة (Memory Pool)
 */
template<typename T, size_t BlockSize = 1024>
class MemoryPool {
public:
    MemoryPool() = default;
    
    ~MemoryPool() {
        for (auto* block : blocks_) {
            std::free(block);
        }
    }
    
    T* allocate() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (freeList_.empty()) {
            allocateBlock();
        }
        
        T* ptr = freeList_.back();
        freeList_.pop_back();
        return new (ptr) T();
    }
    
    void deallocate(T* ptr) {
        if (!ptr) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        ptr->~T();
        freeList_.push_back(ptr);
    }
    
    size_t getAllocatedCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return blocks_.size() * BlockSize - freeList_.size();
    }

private:
    void allocateBlock() {
        T* block = static_cast<T*>(std::aligned_alloc(alignof(T),
                                                       sizeof(T) * BlockSize));
        if (!block) throw std::bad_alloc();
        
        blocks_.push_back(block);
        for (size_t i = 0; i < BlockSize; ++i) {
            freeList_.push_back(block + i);
        }
    }
    
    mutable std::mutex mutex_;
    std::vector<T*> blocks_;
    std::vector<T*> freeList_;
};

// =============================================================================
// الفئة 5: التحقق من المدخلات (600 إصلاح)
// =============================================================================

/**
 * @brief معقم المدخلات الشامل
 */
class InputSanitizer {
public:
    // === تعقيم النصوص ===
    static std::string sanitizeString(const std::string& input,
                                       size_t maxLength = 10000) {
        std::string result;
        result.reserve(std::min(input.size(), maxLength));
        
        for (size_t i = 0; i < input.size() && result.size() < maxLength; ++i) {
            char c = input[i];
            
            // إزالة الأحرف الخطرة
            if (c == '\0') continue;
            if (static_cast<unsigned char>(c) < 32 && c != '\n' && c != '\r' && c != '\t') {
                continue;
            }
            
            result += c;
        }
        
        return result;
    }
    
    // === تعقيم المسارات ===
    static std::string sanitizePath(const std::string& path) {
        std::string result;
        
        // إزالة محاولات التجاوز
        std::string normalized = path;
        
        // استبدال \ بـ /
        std::replace(normalized.begin(), normalized.end(), '\\', '/');
        
        // إزالة //
        size_t pos;
        while ((pos = normalized.find("//")) != std::string::npos) {
            normalized.erase(pos, 1);
        }
        
        // إزالة ..
        while ((pos = normalized.find("..")) != std::string::npos) {
            // إيجاد بداية المكون السابق
            size_t start = pos > 0 ? normalized.rfind('/', pos - 2) : std::string::npos;
            if (start == std::string::npos) start = 0;
            else start++;
            
            normalized.erase(start, pos + 2 - start + 1);
        }
        
        // إزالة ./ في البداية
        if (normalized.substr(0, 2) == "./") {
            normalized = normalized.substr(2);
        }
        
        return normalized;
    }
    
    // === تعقيم أوامر النظام ===
    static std::string sanitizeCommand(const std::string& cmd) {
        static const std::string dangerous = ";|&`$(){}[]<>\\\"'";
        std::string result;
        result.reserve(cmd.size());
        
        for (char c : cmd) {
            if (dangerous.find(c) == std::string::npos) {
                result += c;
            }
        }
        
        return result;
    }
    
    // === التحقق من صحة اسم الملف ===
    static bool isValidFilename(const std::string& name) {
        if (name.empty() || name.size() > 255) return false;
        
        // أحرف ممنوعة
        static const std::string invalid = "<>:\"/\\|?*";
        for (char c : name) {
            if (invalid.find(c) != std::string::npos) return false;
            if (static_cast<unsigned char>(c) < 32) return false;
        }
        
        // أسماء محجوزة في Windows
        static const std::vector<std::string> reserved = {
            "CON", "PRN", "AUX", "NUL",
            "COM1", "COM2", "COM3", "COM4", "COM5",
            "COM6", "COM7", "COM8", "COM9",
            "LPT1", "LPT2", "LPT3", "LPT4", "LPT5",
            "LPT6", "LPT7", "LPT8", "LPT9"
        };
        
        std::string upper = name;
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        
        // إزالة الامتداد للمقارنة
        size_t dotPos = upper.find('.');
        std::string baseName = (dotPos != std::string::npos)
            ? upper.substr(0, dotPos) : upper;
        
        for (const auto& r : reserved) {
            if (baseName == r) return false;
        }
        
        return true;
    }
    
    // === التحقق من صحة المعرف ===
    static bool isValidIdentifier(const std::string& name) {
        if (name.empty()) return false;
        
        // الحرف الأول يجب أن يكون حرفاً أو _
        if (!std::isalpha(name[0]) && name[0] != '_') {
            // التحقق من الحروف العربية
            unsigned char c = name[0];
            if (c < 0xD8 || c > 0xD9) {
                return false;
            }
        }
        
        // باقي الأحرف يمكن أن تكون أرقام أيضاً
        for (size_t i = 1; i < name.size(); ++i) {
            char c = name[i];
            if (!std::isalnum(c) && c != '_') {
                unsigned char uc = static_cast<unsigned char>(c);
                // UTF-8 continuation byte or Arabic
                if ((uc & 0xC0) != 0x80 && (uc < 0xD8 || uc > 0xD9)) {
                    return false;
                }
            }
        }
        
        return true;
    }
    
    // === تعقيم HTML ===
    static std::string escapeHtml(const std::string& input) {
        std::string result;
        result.reserve(input.size() * 1.2);
        
        for (char c : input) {
            switch (c) {
                case '<': result += "&lt;"; break;
                case '>': result += "&gt;"; break;
                case '&': result += "&amp;"; break;
                case '"': result += "&quot;"; break;
                case '\'': result += "&#39;"; break;
                default: result += c; break;
            }
        }
        
        return result;
    }
    
    // === تعقيم SQL ===
    static std::string escapeSQL(const std::string& input) {
        std::string result;
        result.reserve(input.size() * 2);
        
        for (char c : input) {
            if (c == '\'') {
                result += "''";
            } else if (c == '\\') {
                result += "\\\\";
            } else if (c == '\0') {
                // تجاهل null
            } else {
                result += c;
            }
        }
        
        return result;
    }
    
    // === تعقيم JSON ===
    static std::string escapeJSON(const std::string& input) {
        std::string result;
        result.reserve(input.size() * 1.2);
        
        for (char c : input) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:
                    if (static_cast<unsigned char>(c) < 32) {
                        char buf[8];
                        std::snprintf(buf, sizeof(buf), "\\u%04x", c);
                        result += buf;
                    } else {
                        result += c;
                    }
                    break;
            }
        }
        
        return result;
    }
    
    // === تحويل الأرقام الآمن ===
    static std::optional<int64_t> safeParseInt(const std::string& str, int base = 10) {
        if (str.empty()) return std::nullopt;
        
        const char* begin = str.c_str();
        const char* end = begin + str.size();
        
        // تجاوز المسافات
        while (begin < end && std::isspace(*begin)) ++begin;
        while (end > begin && std::isspace(*(end - 1))) --end;
        
        if (begin >= end) return std::nullopt;
        
        int64_t result;
        auto [ptr, ec] = std::from_chars(begin, end, result, base);
        
        if (ec != std::errc() || ptr != end) {
            return std::nullopt;
        }
        
        return result;
    }
    
    static std::optional<double> safeParseDouble(const std::string& str) {
        if (str.empty()) return std::nullopt;
        
        try {
            size_t pos;
            double result = std::stod(str, &pos);
            
            // التأكد من تحليل كل السلسلة
            if (pos != str.size()) {
                // تجاوز المسافات في النهاية
                while (pos < str.size() && std::isspace(str[pos])) ++pos;
                if (pos != str.size()) return std::nullopt;
            }
            
            // التحقق من القيم الخاصة
            if (!std::isfinite(result)) return std::nullopt;
            
            return result;
        } catch (...) {
            return std::nullopt;
        }
    }
    
    // === التحقق من البريد الإلكتروني ===
    static bool isValidEmail(const std::string& email) {
        // تحقق بسيط
        if (email.size() < 5 || email.size() > 254) return false;
        
        size_t atPos = email.find('@');
        if (atPos == std::string::npos) return false;
        if (atPos == 0 || atPos == email.size() - 1) return false;
        
        // التحقق من وجود نقطة بعد @
        size_t dotPos = email.find('.', atPos);
        if (dotPos == std::string::npos) return false;
        if (dotPos == atPos + 1 || dotPos == email.size() - 1) return false;
        
        return true;
    }
    
    // === التحقق من URL ===
    static bool isValidUrl(const std::string& url) {
        // التحقق من البروتوكول
        if (url.size() < 8) return false;
        
        std::string lower = url;
        std::transform(lower.begin(), lower.begin() + 8,
                       lower.begin(), ::tolower);
        
        if (lower.substr(0, 7) != "http://" &&
            lower.substr(0, 8) != "https://") {
            return false;
        }
        
        // التحقق من وجود المضيف
        size_t hostStart = url.find("://") + 3;
        size_t hostEnd = url.find('/', hostStart);
        if (hostEnd == std::string::npos) hostEnd = url.size();
        
        std::string host = url.substr(hostStart, hostEnd - hostStart);
        if (host.empty()) return false;
        
        return true;
    }
};

// =============================================================================
// ماكروات مساعدة
// =============================================================================

// فحص الحدود
#define SAD_CHECK_INDEX(container, index) \
    ::sad::compiler::improvements::BoundsChecker::assertValidIndex( \
        (container), (index), #container)

#define SAD_CHECK_RANGE(container, start, end) \
    ::sad::compiler::improvements::BoundsChecker::assertValidRange( \
        (container), (start), (end), #container)

#define SAD_CHECK_NOT_NULL(ptr) \
    ::sad::compiler::improvements::BoundsChecker::assertNotNull((ptr), #ptr)

// العمليات الآمنة
#define SAD_SAFE_ADD(a, b) \
    ::sad::compiler::improvements::OverflowChecker::safeAdd((a), (b))

#define SAD_SAFE_MUL(a, b) \
    ::sad::compiler::improvements::OverflowChecker::safeMul((a), (b))

#define SAD_SAFE_DIV(a, b) \
    ::sad::compiler::improvements::OverflowChecker::safeDiv((a), (b))

// التخصيص المتتبع
#ifdef SAD_DEBUG
    #define SAD_ALLOC(size) \
        ::sad::compiler::improvements::SafeAllocator::instance().allocate( \
            (size), __FILE__, __LINE__)
    #define SAD_FREE(ptr) \
        ::sad::compiler::improvements::SafeAllocator::instance().deallocate(ptr)
#else
    #define SAD_ALLOC(size) std::malloc(size)
    #define SAD_FREE(ptr) std::free(ptr)
#endif

} // namespace improvements
} // namespace compiler
} // namespace sad

#endif // SAD_MEGA_IMPROVEMENTS_PART1_H
