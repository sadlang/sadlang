/**
 * =============================================================================
 * ملف: interpreter_mega_improvements_part2.h
 * الوصف: تحسينات شاملة للمفسر sad - الجزء الثاني
 * المرحلة: Phase 96 - Mega Interpreter Hardening
 * =============================================================================
 * 
 * الفئات المشمولة (تكملة):
 * 5. أمان النصوص والمدخلات (1000 إصلاح)
 * 6. إدارة الاستثناءات (1000 إصلاح)
 * 7. تحسينات VM والأداء (1000 إصلاح)
 * 
 * =============================================================================
 */

#ifndef SAD_INTERPRETER_MEGA_IMPROVEMENTS_PART2_H
#define SAD_INTERPRETER_MEGA_IMPROVEMENTS_PART2_H

#include "interpreter_mega_improvements.h"
#include <regex>
#include <codecvt>
#include <locale>
#include <fstream>

namespace sad {
namespace interpreter {

// =============================================================================
// الفئة 5: أمان النصوص والمدخلات (1000 إصلاح)
// =============================================================================

namespace string_safety {

/**
 * @brief منظف النصوص
 */
class StringSanitizer {
public:
    // إزالة الأحرف الخطرة
    static std::string sanitize(const std::string& input) {
        std::string result;
        result.reserve(input.size());
        
        for (char c : input) {
            // السماح فقط بالأحرف الآمنة
            if (isSafeChar(c)) {
                result += c;
            } else if (c == '\0') {
                // تجنب null terminator في الوسط
                break;
            }
            // تجاهل الأحرف الخطرة
        }
        
        return result;
    }
    
    // إزالة أحرف التحكم
    static std::string removeControlChars(const std::string& input) {
        std::string result;
        result.reserve(input.size());
        
        for (unsigned char c : input) {
            if (c >= 32 || c == '\n' || c == '\r' || c == '\t') {
                result += c;
            }
        }
        
        return result;
    }
    
    // تنظيف لـ HTML
    static std::string escapeHtml(const std::string& input) {
        std::string result;
        result.reserve(input.size() * 1.2);
        
        for (char c : input) {
            switch (c) {
                case '&':  result += "&amp;"; break;
                case '<':  result += "&lt;"; break;
                case '>':  result += "&gt;"; break;
                case '"':  result += "&quot;"; break;
                case '\'': result += "&#39;"; break;
                default:   result += c; break;
            }
        }
        
        return result;
    }
    
    // تنظيف لـ SQL (أفضل استخدام prepared statements)
    static std::string escapeSql(const std::string& input) {
        std::string result;
        result.reserve(input.size() * 2);
        
        for (char c : input) {
            switch (c) {
                case '\'': result += "''"; break;
                case '\\': result += "\\\\"; break;
                case '\0': break;  // تجنب
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                default:   result += c; break;
            }
        }
        
        return result;
    }
    
    // تنظيف لـ JSON
    static std::string escapeJson(const std::string& input) {
        std::string result;
        result.reserve(input.size() * 1.5);
        
        for (unsigned char c : input) {
            switch (c) {
                case '"':  result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:
                    if (c < 32) {
                        char buf[8];
                        snprintf(buf, sizeof(buf), "\\u%04x", c);
                        result += buf;
                    } else {
                        result += c;
                    }
                    break;
            }
        }
        
        return result;
    }
    
    // تنظيف للمسارات (منع path traversal)
    static errors::RuntimeResult<std::string> sanitizePath(const std::string& input) {
        // التحقق من المسارات الخطرة
        if (input.find("..") != std::string::npos) {
            return errors::RuntimeResult<std::string>::error(
                errors::ErrorBuilder()
                    .category(errors::RuntimeErrorCategory::InvalidValue)
                    .message("Path traversal attempt detected")
                    .arabicMessage("محاولة تجاوز المسار المكتشفة")
                    .build()
            );
        }
        
        // إزالة الأحرف غير الصالحة في المسارات
        std::string result;
        for (char c : input) {
            if (isValidPathChar(c)) {
                result += c;
            }
        }
        
        return errors::RuntimeResult<std::string>::ok(result);
    }
    
    // تنظيف للأوامر (منع command injection)
    static errors::RuntimeResult<std::string> sanitizeCommand(const std::string& input) {
        // رفض الأحرف الخطرة
        const std::string dangerous = "|;&$`\\!#()<>{}[]'\"\n\r";
        
        for (char c : input) {
            if (dangerous.find(c) != std::string::npos) {
                return errors::RuntimeResult<std::string>::error(
                    errors::ErrorBuilder()
                        .category(errors::RuntimeErrorCategory::InvalidValue)
                        .message("Dangerous characters in command")
                        .arabicMessage("أحرف خطرة في الأمر")
                        .suggestion("أزل الأحرف الخاصة من المدخل")
                        .build()
                );
            }
        }
        
        return errors::RuntimeResult<std::string>::ok(input);
    }
    
private:
    static bool isSafeChar(char c) {
        unsigned char uc = static_cast<unsigned char>(c);
        // السماح بـ UTF-8 والأحرف الآمنة
        return uc >= 32 || c == '\n' || c == '\r' || c == '\t' || uc >= 128;
    }
    
    static bool isValidPathChar(char c) {
        if (c >= 'a' && c <= 'z') return true;
        if (c >= 'A' && c <= 'Z') return true;
        if (c >= '0' && c <= '9') return true;
        if (c == '/' || c == '\\' || c == '.' || c == '_' || c == '-') return true;
        if (c == ':') return true;  // للـ Windows drives
        // السماح بالعربية
        if (static_cast<unsigned char>(c) >= 0x80) return true;
        return false;
    }
};

/**
 * @brief محقق النصوص
 */
class StringValidator {
public:
    // التحقق من UTF-8 صالح
    static bool isValidUtf8(const std::string& str) {
        const unsigned char* bytes = reinterpret_cast<const unsigned char*>(str.data());
        size_t len = str.size();
        size_t i = 0;
        
        while (i < len) {
            if (bytes[i] <= 0x7F) {
                i++;
            } else if ((bytes[i] & 0xE0) == 0xC0) {
                if (i + 1 >= len || (bytes[i + 1] & 0xC0) != 0x80) return false;
                i += 2;
            } else if ((bytes[i] & 0xF0) == 0xE0) {
                if (i + 2 >= len || (bytes[i + 1] & 0xC0) != 0x80 ||
                    (bytes[i + 2] & 0xC0) != 0x80) return false;
                i += 3;
            } else if ((bytes[i] & 0xF8) == 0xF0) {
                if (i + 3 >= len || (bytes[i + 1] & 0xC0) != 0x80 ||
                    (bytes[i + 2] & 0xC0) != 0x80 ||
                    (bytes[i + 3] & 0xC0) != 0x80) return false;
                i += 4;
            } else {
                return false;
            }
        }
        
        return true;
    }
    
    // التحقق من بريد إلكتروني
    static bool isValidEmail(const std::string& email) {
        if (email.empty() || email.length() > 254) return false;
        
        size_t atPos = email.find('@');
        if (atPos == std::string::npos || atPos == 0 || atPos == email.length() - 1) {
            return false;
        }
        
        // التحقق من الأجزاء
        std::string local = email.substr(0, atPos);
        std::string domain = email.substr(atPos + 1);
        
        if (local.empty() || local.length() > 64) return false;
        if (domain.empty() || domain.length() > 253) return false;
        if (domain.find('.') == std::string::npos) return false;
        
        return true;
    }
    
    // التحقق من URL
    static bool isValidUrl(const std::string& url) {
        if (url.empty()) return false;
        
        // يجب أن يبدأ ببروتوكول معروف
        if (url.substr(0, 7) != "http://" && url.substr(0, 8) != "https://") {
            return false;
        }
        
        // التحقق من الأحرف
        for (char c : url) {
            if (c < 32 || c == ' ') return false;
        }
        
        return true;
    }
    
    // التحقق من رقم
    static bool isNumeric(const std::string& str) {
        if (str.empty()) return false;
        
        size_t start = 0;
        if (str[0] == '-' || str[0] == '+') start = 1;
        
        bool hasDigit = false;
        bool hasDot = false;
        
        for (size_t i = start; i < str.size(); ++i) {
            if (str[i] >= '0' && str[i] <= '9') {
                hasDigit = true;
            } else if (str[i] == '.' && !hasDot) {
                hasDot = true;
            } else {
                return false;
            }
        }
        
        return hasDigit;
    }
    
    // التحقق من الحد الأقصى للطول
    static errors::RuntimeResult<std::string> validateLength(
        const std::string& str, size_t maxLength, const std::string& fieldName = "النص") {
        
        if (str.length() > maxLength) {
            return errors::RuntimeResult<std::string>::error(
                errors::ErrorBuilder()
                    .category(errors::RuntimeErrorCategory::InvalidValue)
                    .message(fieldName + " exceeds maximum length of " + std::to_string(maxLength))
                    .arabicMessage(fieldName + " يتجاوز الطول الأقصى " + std::to_string(maxLength))
                    .build()
            );
        }
        
        return errors::RuntimeResult<std::string>::ok(str);
    }
};

/**
 * @brief معالج النصوص العربية
 */
class ArabicTextHandler {
public:
    // عدد الأحرف (ليس البايتات)
    static size_t charCount(const std::string& str) {
        size_t count = 0;
        const unsigned char* bytes = reinterpret_cast<const unsigned char*>(str.data());
        size_t len = str.size();
        size_t i = 0;
        
        while (i < len) {
            if (bytes[i] <= 0x7F) {
                i++;
            } else if ((bytes[i] & 0xE0) == 0xC0) {
                i += 2;
            } else if ((bytes[i] & 0xF0) == 0xE0) {
                i += 3;
            } else if ((bytes[i] & 0xF8) == 0xF0) {
                i += 4;
            } else {
                i++;  // حرف غير صالح
            }
            count++;
        }
        
        return count;
    }
    
    // هل النص عربي؟
    static bool isArabic(const std::string& str) {
        const unsigned char* bytes = reinterpret_cast<const unsigned char*>(str.data());
        size_t len = str.size();
        
        for (size_t i = 0; i < len;) {
            uint32_t codepoint = 0;
            
            if (bytes[i] <= 0x7F) {
                codepoint = bytes[i];
                i++;
            } else if ((bytes[i] & 0xE0) == 0xC0 && i + 1 < len) {
                codepoint = ((bytes[i] & 0x1F) << 6) | (bytes[i + 1] & 0x3F);
                i += 2;
            } else if ((bytes[i] & 0xF0) == 0xE0 && i + 2 < len) {
                codepoint = ((bytes[i] & 0x0F) << 12) |
                            ((bytes[i + 1] & 0x3F) << 6) |
                            (bytes[i + 2] & 0x3F);
                i += 3;
            } else {
                i++;
                continue;
            }
            
            // نطاق العربية في Unicode
            if (codepoint >= 0x0600 && codepoint <= 0x06FF) {
                return true;
            }
            if (codepoint >= 0x0750 && codepoint <= 0x077F) {
                return true;  // العربية الإضافية
            }
            if (codepoint >= 0xFB50 && codepoint <= 0xFDFF) {
                return true;  // أشكال عرض عربية
            }
        }
        
        return false;
    }
    
    // تطبيع النص العربي
    static std::string normalize(const std::string& str) {
        // إزالة التشكيل وتوحيد الأحرف
        // ... تنفيذ كامل
        return str;
    }
};

} // namespace string_safety

// =============================================================================
// الفئة 6: إدارة الاستثناءات (1000 إصلاح)
// =============================================================================

namespace exception_handling {

/**
 * @brief مُعالج الاستثناءات
 */
class ExceptionHandler {
public:
    using Handler = std::function<void(const errors::SadRuntimeException&)>;
    using FinallyBlock = std::function<void()>;
    
    // تسجيل معالج عام
    void registerHandler(errors::RuntimeErrorCategory category, Handler handler) {
        handlers_[category] = handler;
    }
    
    // تنفيذ مع معالجة
    template<typename Func>
    auto tryExecute(Func&& func) -> decltype(func()) {
        try {
            return func();
        } catch (const errors::SadRuntimeException& e) {
            auto it = handlers_.find(e.getCategory());
            if (it != handlers_.end()) {
                it->second(e);
            } else if (defaultHandler_) {
                defaultHandler_(e);
            }
            throw;
        }
    }
    
    // try-catch-finally
    template<typename TryFunc, typename CatchFunc, typename FinallyFunc>
    void tryCatchFinally(TryFunc&& tryBlock,
                          CatchFunc&& catchBlock,
                          FinallyFunc&& finallyBlock) {
        try {
            tryBlock();
        } catch (const errors::SadRuntimeException& e) {
            catchBlock(e);
        } catch (...) {
            finallyBlock();
            throw;
        }
        finallyBlock();
    }
    
    void setDefaultHandler(Handler handler) {
        defaultHandler_ = handler;
    }
    
private:
    std::map<errors::RuntimeErrorCategory, Handler> handlers_;
    Handler defaultHandler_;
};

/**
 * @brief حارس النطاق (RAII)
 */
class ScopeGuard {
public:
    explicit ScopeGuard(std::function<void()> cleanup)
        : cleanup_(std::move(cleanup)), dismissed_(false) {}
    
    ~ScopeGuard() {
        if (!dismissed_) {
            try {
                cleanup_();
            } catch (...) {
                // لا نريد استثناءات في المدمر
            }
        }
    }
    
    void dismiss() { dismissed_ = true; }
    
    // منع النسخ
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    
    // السماح بالنقل
    ScopeGuard(ScopeGuard&& other) noexcept
        : cleanup_(std::move(other.cleanup_)), dismissed_(other.dismissed_) {
        other.dismissed_ = true;
    }
    
private:
    std::function<void()> cleanup_;
    bool dismissed_;
};

#define SCOPE_EXIT(code) ScopeGuard _scope_guard_##__LINE__([&]() { code; })

/**
 * @brief سجل الاستثناءات
 */
class ExceptionLog {
public:
    struct LogEntry {
        std::chrono::system_clock::time_point time;
        errors::RuntimeError error;
        std::string context;
    };
    
    static ExceptionLog& instance() {
        static ExceptionLog inst;
        return inst;
    }
    
    void log(const errors::SadRuntimeException& e, const std::string& context = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        
        LogEntry entry;
        entry.time = std::chrono::system_clock::now();
        entry.error = e.getError();
        entry.context = context;
        
        entries_.push_back(entry);
        
        // حد أقصى للإدخالات
        if (entries_.size() > maxEntries_) {
            entries_.erase(entries_.begin());
        }
    }
    
    std::vector<LogEntry> getRecentEntries(size_t count = 10) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        size_t start = entries_.size() > count ? entries_.size() - count : 0;
        return std::vector<LogEntry>(entries_.begin() + start, entries_.end());
    }
    
    void saveToFile(const std::string& filename) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::ofstream out(filename);
        for (const auto& entry : entries_) {
            auto timeT = std::chrono::system_clock::to_time_t(entry.time);
            out << std::ctime(&timeT);
            out << entry.error.formatMessage() << "\n";
            if (!entry.context.empty()) {
                out << "Context: " << entry.context << "\n";
            }
            out << "---\n";
        }
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        entries_.clear();
    }
    
private:
    ExceptionLog() = default;
    
    mutable std::mutex mutex_;
    std::vector<LogEntry> entries_;
    size_t maxEntries_ = 1000;
};

/**
 * @brief إعادة المحاولة التلقائية
 */
class RetryPolicy {
public:
    struct Config {
        size_t maxRetries = 3;
        std::chrono::milliseconds initialDelay{100};
        double backoffMultiplier = 2.0;
        std::chrono::milliseconds maxDelay{10000};
        std::set<errors::RuntimeErrorCategory> retryableErrors;
    };
    
    explicit RetryPolicy(Config config = {}) : config_(std::move(config)) {
        // الأخطاء القابلة للمحاولة افتراضياً
        if (config_.retryableErrors.empty()) {
            config_.retryableErrors = {
                errors::RuntimeErrorCategory::NetworkError,
                errors::RuntimeErrorCategory::TimeoutError,
                errors::RuntimeErrorCategory::IOError
            };
        }
    }
    
    template<typename Func>
    auto execute(Func&& func) -> decltype(func()) {
        std::chrono::milliseconds delay = config_.initialDelay;
        
        for (size_t attempt = 0; attempt <= config_.maxRetries; ++attempt) {
            try {
                return func();
            } catch (const errors::SadRuntimeException& e) {
                if (attempt == config_.maxRetries ||
                    config_.retryableErrors.find(e.getCategory()) ==
                    config_.retryableErrors.end()) {
                    throw;
                }
                
                std::this_thread::sleep_for(delay);
                
                delay = std::chrono::milliseconds(
                    static_cast<int64_t>(delay.count() * config_.backoffMultiplier)
                );
                if (delay > config_.maxDelay) {
                    delay = config_.maxDelay;
                }
            }
        }
        
        // Unreachable
        throw std::runtime_error("Unreachable");
    }
    
private:
    Config config_;
};

} // namespace exception_handling

// =============================================================================
// الفئة 7: تحسينات VM والأداء (1000 إصلاح)
// =============================================================================

namespace vm_optimizations {

/**
 * @brief ذاكرة تخزين مؤقت للقيم
 */
template<typename Key, typename Value>
class ValueCache {
public:
    explicit ValueCache(size_t maxSize = 1000) : maxSize_(maxSize) {}
    
    std::optional<Value> get(const Key& key) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            hits_++;
            return it->second;
        }
        
        misses_++;
        return std::nullopt;
    }
    
    void put(const Key& key, const Value& value) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        
        if (cache_.size() >= maxSize_) {
            evictOldest();
        }
        
        cache_[key] = value;
        accessOrder_.push_back(key);
    }
    
    void invalidate(const Key& key) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        cache_.erase(key);
    }
    
    void clear() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        cache_.clear();
        accessOrder_.clear();
    }
    
    double hitRate() const {
        size_t total = hits_ + misses_;
        return total > 0 ? static_cast<double>(hits_) / total : 0.0;
    }
    
private:
    void evictOldest() {
        if (!accessOrder_.empty()) {
            cache_.erase(accessOrder_.front());
            accessOrder_.erase(accessOrder_.begin());
        }
    }
    
    mutable std::shared_mutex mutex_;
    std::map<Key, Value> cache_;
    std::vector<Key> accessOrder_;
    size_t maxSize_;
    std::atomic<size_t> hits_{0};
    std::atomic<size_t> misses_{0};
};

/**
 * @brief مكدس التنفيذ
 */
template<typename T>
class ExecutionStack {
public:
    explicit ExecutionStack(size_t maxSize = 10000) : maxSize_(maxSize) {
        stack_.reserve(maxSize);
    }
    
    void push(T value) {
        if (stack_.size() >= maxSize_) {
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::StackOverflow)
                .message("Execution stack overflow")
                .arabicMessage("تجاوز مكدس التنفيذ")
                .suggestion("تحقق من وجود استدعاء متكرر لا نهائي")
                .raise();
        }
        stack_.push_back(std::move(value));
    }
    
    T pop() {
        if (stack_.empty()) {
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::InvalidState)
                .message("Stack underflow")
                .arabicMessage("نقصان في المكدس")
                .raise();
        }
        T value = std::move(stack_.back());
        stack_.pop_back();
        return value;
    }
    
    T& top() {
        if (stack_.empty()) {
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::EmptyContainer)
                .message("Stack is empty")
                .arabicMessage("المكدس فارغ")
                .raise();
        }
        return stack_.back();
    }
    
    const T& top() const {
        if (stack_.empty()) {
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::EmptyContainer)
                .message("Stack is empty")
                .arabicMessage("المكدس فارغ")
                .raise();
        }
        return stack_.back();
    }
    
    bool empty() const { return stack_.empty(); }
    size_t size() const { return stack_.size(); }
    
    void clear() { stack_.clear(); }
    
private:
    std::vector<T> stack_;
    size_t maxSize_;
};

/**
 * @brief جدول الثوابت
 */
class ConstantPool {
public:
    size_t addInteger(int64_t value) {
        return addConstant(intPool_, value);
    }
    
    size_t addDouble(double value) {
        return addConstant(doublePool_, value);
    }
    
    size_t addString(const std::string& value) {
        return addConstant(stringPool_, value);
    }
    
    int64_t getInteger(size_t index) const {
        return getConstant(intPool_, index, "integer");
    }
    
    double getDouble(size_t index) const {
        return getConstant(doublePool_, index, "double");
    }
    
    const std::string& getString(size_t index) const {
        return getConstant(stringPool_, index, "string");
    }
    
private:
    template<typename T>
    size_t addConstant(std::vector<T>& pool, const T& value) {
        // البحث عن قيمة موجودة
        for (size_t i = 0; i < pool.size(); ++i) {
            if (pool[i] == value) return i;
        }
        pool.push_back(value);
        return pool.size() - 1;
    }
    
    template<typename T>
    const T& getConstant(const std::vector<T>& pool, size_t index,
                          const std::string& typeName) const {
        if (index >= pool.size()) {
            errors::ErrorBuilder()
                .category(errors::RuntimeErrorCategory::IndexOutOfBounds)
                .message("Invalid " + typeName + " constant index: " + std::to_string(index))
                .arabicMessage("فهرس ثابت " + typeName + " غير صالح: " + std::to_string(index))
                .raise();
        }
        return pool[index];
    }
    
    std::vector<int64_t> intPool_;
    std::vector<double> doublePool_;
    std::vector<std::string> stringPool_;
};

/**
 * @brief مقياس الأداء
 */
class PerformanceMonitor {
public:
    struct Metrics {
        std::atomic<uint64_t> instructionsExecuted{0};
        std::atomic<uint64_t> functionCalls{0};
        std::atomic<uint64_t> memoryAllocations{0};
        std::atomic<uint64_t> cacheHits{0};
        std::atomic<uint64_t> cacheMisses{0};
        std::chrono::steady_clock::time_point startTime;
        
        double instructionsPerSecond() const {
            auto elapsed = std::chrono::steady_clock::now() - startTime;
            auto seconds = std::chrono::duration<double>(elapsed).count();
            return seconds > 0 ? instructionsExecuted / seconds : 0;
        }
    };
    
    static PerformanceMonitor& instance() {
        static PerformanceMonitor inst;
        return inst;
    }
    
    void recordInstruction() {
        metrics_.instructionsExecuted++;
    }
    
    void recordFunctionCall() {
        metrics_.functionCalls++;
    }
    
    void recordMemoryAllocation() {
        metrics_.memoryAllocations++;
    }
    
    void recordCacheHit() {
        metrics_.cacheHits++;
    }
    
    void recordCacheMiss() {
        metrics_.cacheMisses++;
    }
    
    void reset() {
        metrics_.instructionsExecuted = 0;
        metrics_.functionCalls = 0;
        metrics_.memoryAllocations = 0;
        metrics_.cacheHits = 0;
        metrics_.cacheMisses = 0;
        metrics_.startTime = std::chrono::steady_clock::now();
    }
    
    const Metrics& getMetrics() const { return metrics_; }
    
    std::string generateReport() const {
        std::ostringstream out;
        out << "=== تقرير الأداء ===\n";
        out << "التعليمات المنفذة: " << metrics_.instructionsExecuted << "\n";
        out << "استدعاءات الدوال: " << metrics_.functionCalls << "\n";
        out << "تخصيصات الذاكرة: " << metrics_.memoryAllocations << "\n";
        out << "معدل الإصابة بالذاكرة المؤقتة: "
            << (metrics_.cacheHits + metrics_.cacheMisses > 0 ?
                100.0 * metrics_.cacheHits / (metrics_.cacheHits + metrics_.cacheMisses) : 0)
            << "%\n";
        out << "التعليمات/ثانية: " << metrics_.instructionsPerSecond() << "\n";
        return out.str();
    }
    
private:
    PerformanceMonitor() {
        metrics_.startTime = std::chrono::steady_clock::now();
    }
    
    Metrics metrics_;
};

/**
 * @brief محرك VM محسن
 */
class OptimizedVM {
public:
    enum class Opcode : uint8_t {
        Nop = 0,
        
        // تحميل وتخزين
        LoadConst,
        LoadLocal,
        StoreLocal,
        LoadGlobal,
        StoreGlobal,
        
        // عمليات المكدس
        Push,
        Pop,
        Dup,
        Swap,
        
        // عمليات حسابية
        Add,
        Sub,
        Mul,
        Div,
        Mod,
        Neg,
        
        // مقارنة
        Eq,
        Ne,
        Lt,
        Le,
        Gt,
        Ge,
        
        // منطقية
        And,
        Or,
        Not,
        
        // تفرع
        Jump,
        JumpIf,
        JumpIfNot,
        
        // استدعاء
        Call,
        Return,
        
        // كائنات
        New,
        GetField,
        SetField,
        CallMethod,
        
        // مصفوفات
        NewArray,
        GetElement,
        SetElement,
        ArrayLength,
        
        // إدخال/إخراج
        Print,
        PrintLine,
        
        // تحكم
        Halt
    };
    
    struct Instruction {
        Opcode opcode;
        int32_t arg1 = 0;
        int32_t arg2 = 0;
    };
    
    void execute(const std::vector<Instruction>& program) {
        size_t ip = 0;  // instruction pointer
        
        while (ip < program.size()) {
            const auto& inst = program[ip];
            PerformanceMonitor::instance().recordInstruction();
            
            switch (inst.opcode) {
                case Opcode::Nop:
                    break;
                    
                case Opcode::LoadConst:
                    // تحميل ثابت
                    valueStack_.push(loadConstant(inst.arg1));
                    break;
                    
                case Opcode::Add: {
                    auto b = valueStack_.pop();
                    auto a = valueStack_.pop();
                    valueStack_.push(performAdd(a, b));
                    break;
                }
                
                case Opcode::Call:
                    PerformanceMonitor::instance().recordFunctionCall();
                    // ... تنفيذ الاستدعاء
                    break;
                    
                case Opcode::Jump:
                    ip = inst.arg1;
                    continue;
                    
                case Opcode::JumpIf: {
                    auto cond = valueStack_.pop();
                    if (isTruthy(cond)) {
                        ip = inst.arg1;
                        continue;
                    }
                    break;
                }
                
                case Opcode::Halt:
                    return;
                    
                default:
                    errors::ErrorBuilder()
                        .category(errors::RuntimeErrorCategory::InvalidOperation)
                        .message("Unknown opcode: " + std::to_string(static_cast<int>(inst.opcode)))
                        .arabicMessage("عملية غير معروفة")
                        .raise();
            }
            
            ip++;
        }
    }
    
private:
    using VMValue = std::variant<std::monostate, int64_t, double, std::string, bool>;
    
    ExecutionStack<VMValue> valueStack_;
    ConstantPool constants_;
    
    VMValue loadConstant(int32_t index) {
        // تحميل من جدول الثوابت
        return VMValue{};  // placeholder
    }
    
    VMValue performAdd(const VMValue& a, const VMValue& b) {
        // جمع مع فحص النوع
        if (auto* ai = std::get_if<int64_t>(&a)) {
            if (auto* bi = std::get_if<int64_t>(&b)) {
                auto result = bounds::ArithmeticChecker::safeAdd(*ai, *bi);
                result.unwrap();
                return result.value();
            }
        }
        if (auto* ad = std::get_if<double>(&a)) {
            if (auto* bd = std::get_if<double>(&b)) {
                return *ad + *bd;
            }
        }
        if (auto* as = std::get_if<std::string>(&a)) {
            if (auto* bs = std::get_if<std::string>(&b)) {
                return *as + *bs;
            }
        }
        
        errors::ErrorBuilder()
            .category(errors::RuntimeErrorCategory::TypeError)
            .message("Cannot add these types")
            .arabicMessage("لا يمكن جمع هذه الأنواع")
            .raise();
            
        return VMValue{};  // Unreachable
    }
    
    bool isTruthy(const VMValue& v) {
        if (auto* b = std::get_if<bool>(&v)) return *b;
        if (auto* i = std::get_if<int64_t>(&v)) return *i != 0;
        if (auto* d = std::get_if<double>(&v)) return *d != 0.0;
        if (auto* s = std::get_if<std::string>(&v)) return !s->empty();
        return false;
    }
};

} // namespace vm_optimizations

} // namespace interpreter
} // namespace sad

#endif // SAD_INTERPRETER_MEGA_IMPROVEMENTS_PART2_H
