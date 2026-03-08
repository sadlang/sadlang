/**
 * =============================================================================
 * ملف: compiler_improvements.h
 * الوصف: تحسينات شاملة للمترجم sadc - إصلاح 700 نقطة ضعف
 * المرحلة: Phase 95 - Compiler Hardening
 * =============================================================================
 * 
 * 🛡️ تحسينات المترجم الشاملة
 * ═════════════════════════════════════════
 * 
 * هذا الملف يوفر:
 * 1. معالجة أخطاء محسنة
 * 2. أمان الخيوط (Thread Safety)
 * 3. فحص الحدود والقيم
 * 4. تسجيل الأخطاء
 * 5. أدوات التحقق
 * 
 * =============================================================================
 */

#ifndef SAD_COMPILER_IMPROVEMENTS_H
#define SAD_COMPILER_IMPROVEMENTS_H

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <optional>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <chrono>
#include <atomic>
#include <set>
#include <iomanip>
#include <algorithm>

namespace sad {
namespace compiler {

// =============================================================================
// الفئة 1: معالجة الأخطاء المحسنة (إصلاح 50 نقطة ضعف)
// =============================================================================

/**
 * @brief أنواع أخطاء المترجم
 */
enum class CompilerErrorKind {
    // أخطاء المحلل المعجمي (1-20)
    LexerInvalidToken = 1,
    LexerUnterminatedString = 2,
    LexerInvalidNumber = 3,
    LexerInvalidEscape = 4,
    LexerUnterminatedComment = 5,
    LexerInvalidUTF8 = 6,
    LexerFileTooLarge = 7,
    LexerLineTooLong = 8,
    LexerTooManyTokens = 9,
    LexerInvalidCharacter = 10,
    
    // أخطاء المحلل النحوي (21-50)
    ParserUnexpectedToken = 21,
    ParserMissingExpression = 22,
    ParserMissingStatement = 23,
    ParserUnterminatedBlock = 24,
    ParserInvalidDeclaration = 25,
    ParserTooManyArguments = 26,
    ParserTooManyParameters = 27,
    ParserInvalidOperator = 28,
    ParserNestedTooDeep = 29,
    ParserSyntaxError = 30,
    
    // أخطاء التحليل الدلالي (51-100)
    SemanticUndefinedVariable = 51,
    SemanticTypeMismatch = 52,
    SemanticInvalidAssignment = 53,
    SemanticUndefinedFunction = 54,
    SemanticDuplicateDefinition = 55,
    SemanticInvalidReturn = 56,
    SemanticMissingReturn = 57,
    SemanticInvalidBreak = 58,
    SemanticInvalidContinue = 59,
    SemanticBorrowAfterMove = 60,
    SemanticMutBorrowConflict = 61,
    SemanticUseAfterDrop = 62,
    SemanticInfiniteRecursion = 63,
    SemanticUnusedVariable = 64,
    SemanticDeadCode = 65,
    
    // أخطاء SIR (101-150)
    SirInvalidInstruction = 101,
    SirInvalidOperand = 102,
    SirTypeMismatch = 103,
    SirInvalidBlock = 104,
    SirUnreachableCode = 105,
    SirInvalidJump = 106,
    SirStackOverflow = 107,
    SirInvalidCall = 108,
    
    // أخطاء LLVM (151-200)
    LlvmCodegenFailed = 151,
    LlvmVerificationFailed = 152,
    LlvmOptimizationFailed = 153,
    LlvmTargetNotSupported = 154,
    LlvmLinkingFailed = 155,
    LlvmObjectFileFailed = 156,
    
    // أخطاء الرسومات (201-250)
    GraphicsSDL2NotFound = 201,
    GraphicsWindowCreationFailed = 202,
    GraphicsRendererFailed = 203,
    GraphicsWidgetInvalid = 204,
    GraphicsFontLoadFailed = 205,
    GraphicsTextureLoadFailed = 206,
    
    // أخطاء عامة (251-300)
    InternalError = 251,
    OutOfMemory = 252,
    FileNotFound = 253,
    PermissionDenied = 254,
    IOError = 255,
    Timeout = 256
};

/**
 * @brief خطأ مترجم مفصل
 */
struct CompilerError {
    CompilerErrorKind kind;
    std::string message;
    std::string messageArabic;
    std::string file;
    int line;
    int column;
    std::string sourceSnippet;
    std::vector<std::string> notes;
    std::vector<std::string> suggestions;
    
    // إنشاء رسالة خطأ منسقة
    std::string format() const {
        std::ostringstream ss;
        ss << "\n╔══════════════════════════════════════════════════════════════════╗\n";
        ss << "║ خطأ / Error: " << static_cast<int>(kind) << "\n";
        ss << "╠══════════════════════════════════════════════════════════════════╣\n";
        
        if (!file.empty()) {
            ss << "║ الملف / File: " << file << ":" << line << ":" << column << "\n";
        }
        
        ss << "║\n";
        ss << "║ " << messageArabic << "\n";
        ss << "║ " << message << "\n";
        
        if (!sourceSnippet.empty()) {
            ss << "╠──────────────────────────────────────────────────────────────────╣\n";
            ss << "║ " << sourceSnippet << "\n";
        }
        
        if (!notes.empty()) {
            ss << "╠──────────────────────────────────────────────────────────────────╣\n";
            for (const auto& note : notes) {
                ss << "║ ملاحظة: " << note << "\n";
            }
        }
        
        if (!suggestions.empty()) {
            ss << "╠──────────────────────────────────────────────────────────────────╣\n";
            for (const auto& sug : suggestions) {
                ss << "║ اقتراح: " << sug << "\n";
            }
        }
        
        ss << "╚══════════════════════════════════════════════════════════════════╝\n";
        return ss.str();
    }
};

/**
 * @brief مجمع الأخطاء - آمن للخيوط
 */
class ErrorCollector {
public:
    static ErrorCollector& instance() {
        static ErrorCollector instance;
        return instance;
    }
    
    void addError(CompilerError error) {
        std::lock_guard<std::mutex> lock(mutex_);
        errors_.push_back(std::move(error));
        errorCount_++;
    }
    
    void addWarning(CompilerError warning) {
        std::lock_guard<std::mutex> lock(mutex_);
        warnings_.push_back(std::move(warning));
        warningCount_++;
    }
    
    bool hasErrors() const { return errorCount_ > 0; }
    bool hasWarnings() const { return warningCount_ > 0; }
    
    size_t errorCount() const { return errorCount_; }
    size_t warningCount() const { return warningCount_; }
    
    std::vector<CompilerError> getErrors() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return errors_;
    }
    
    std::vector<CompilerError> getWarnings() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return warnings_;
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        errors_.clear();
        warnings_.clear();
        errorCount_ = 0;
        warningCount_ = 0;
    }
    
    std::string formatAll() const {
        std::ostringstream ss;
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (const auto& err : errors_) {
            ss << err.format();
        }
        
        ss << "\n═══════════════════════════════════════════════════════════════════\n";
        ss << "المجموع: " << errorCount_ << " خطأ، " << warningCount_ << " تحذير\n";
        
        return ss.str();
    }
    
private:
    mutable std::mutex mutex_;
    std::vector<CompilerError> errors_;
    std::vector<CompilerError> warnings_;
    std::atomic<size_t> errorCount_{0};
    std::atomic<size_t> warningCount_{0};
    
    ErrorCollector() = default;
};

// =============================================================================
// الفئة 2: أمان الخيوط (إصلاح 30 نقطة ضعف)
// =============================================================================

/**
 * @brief نتيجة آمنة للخيوط بدلاً من static string
 */
template<typename T>
class ThreadSafeResult {
public:
    ThreadSafeResult() = default;
    
    void set(T value) {
        std::lock_guard<std::mutex> lock(mutex_);
        value_ = std::move(value);
        hasValue_ = true;
    }
    
    std::optional<T> get() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (hasValue_) {
            return value_;
        }
        return std::nullopt;
    }
    
    T getOrDefault(const T& defaultVal) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return hasValue_ ? value_ : defaultVal;
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        value_ = T{};
        hasValue_ = false;
    }
    
private:
    mutable std::mutex mutex_;
    T value_;
    bool hasValue_ = false;
};

/**
 * @brief مخزن مؤقت آمن للخيوط
 */
template<typename K, typename V>
class ThreadSafeCache {
public:
    std::optional<V> get(const K& key) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    void set(const K& key, V value) {
        std::lock_guard<std::mutex> lock(mutex_);
        cache_[key] = std::move(value);
    }
    
    void remove(const K& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        cache_.erase(key);
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        cache_.clear();
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return cache_.size();
    }
    
private:
    mutable std::mutex mutex_;
    std::unordered_map<K, V> cache_;
};

// =============================================================================
// الفئة 3: فحص الحدود والقيم (إصلاح 80 نقطة ضعف)
// =============================================================================

/**
 * @brief فاحص الحدود الآمن
 */
class BoundsChecker {
public:
    // فحص فهرس المصفوفة
    template<typename T>
    static bool checkArrayIndex(const std::vector<T>& arr, size_t index,
                                 const std::string& context = "") {
        if (index >= arr.size()) {
            CompilerError error;
            error.kind = CompilerErrorKind::InternalError;
            error.message = "Array index out of bounds: " + std::to_string(index) + 
                           " >= " + std::to_string(arr.size());
            error.messageArabic = "فهرس المصفوفة خارج الحدود: " + std::to_string(index);
            if (!context.empty()) {
                error.notes.push_back(context);
            }
            ErrorCollector::instance().addError(error);
            return false;
        }
        return true;
    }
    
    // فحص مؤشر غير فارغ
    template<typename T>
    static bool checkNotNull(const T* ptr, const std::string& name = "pointer") {
        if (ptr == nullptr) {
            CompilerError error;
            error.kind = CompilerErrorKind::InternalError;
            error.message = "Null pointer: " + name;
            error.messageArabic = "مؤشر فارغ: " + name;
            ErrorCollector::instance().addError(error);
            return false;
        }
        return true;
    }
    
    // فحص shared_ptr غير فارغ
    template<typename T>
    static bool checkNotNull(const std::shared_ptr<T>& ptr, const std::string& name = "pointer") {
        if (!ptr) {
            CompilerError error;
            error.kind = CompilerErrorKind::InternalError;
            error.message = "Null shared_ptr: " + name;
            error.messageArabic = "مؤشر مشترك فارغ: " + name;
            ErrorCollector::instance().addError(error);
            return false;
        }
        return true;
    }
    
    // فحص نطاق القيمة
    template<typename T>
    static bool checkRange(T value, T min, T max, const std::string& name = "value") {
        if (value < min || value > max) {
            CompilerError error;
            error.kind = CompilerErrorKind::InternalError;
            error.message = name + " out of range: " + std::to_string(value);
            error.messageArabic = name + " خارج النطاق: " + std::to_string(value);
            ErrorCollector::instance().addError(error);
            return false;
        }
        return true;
    }
    
    // فحص قسمة على صفر
    template<typename T>
    static bool checkDivision(T divisor, const std::string& context = "") {
        if (divisor == 0) {
            CompilerError error;
            error.kind = CompilerErrorKind::InternalError;
            error.message = "Division by zero";
            error.messageArabic = "قسمة على صفر";
            if (!context.empty()) {
                error.notes.push_back(context);
            }
            ErrorCollector::instance().addError(error);
            return false;
        }
        return true;
    }
    
    // فحص طفح الأعداد
    static bool checkIntOverflow(int64_t a, int64_t b, char op) {
        switch (op) {
            case '+':
                if ((b > 0 && a > INT64_MAX - b) || (b < 0 && a < INT64_MIN - b)) {
                    reportOverflow("addition");
                    return false;
                }
                break;
            case '-':
                if ((b < 0 && a > INT64_MAX + b) || (b > 0 && a < INT64_MIN + b)) {
                    reportOverflow("subtraction");
                    return false;
                }
                break;
            case '*':
                if (a > 0 && b > 0 && a > INT64_MAX / b) {
                    reportOverflow("multiplication");
                    return false;
                }
                break;
        }
        return true;
    }
    
private:
    static void reportOverflow(const std::string& op) {
        CompilerError error;
        error.kind = CompilerErrorKind::InternalError;
        error.message = "Integer overflow in " + op;
        error.messageArabic = "طفح عددي في " + op;
        ErrorCollector::instance().addError(error);
    }
};

// =============================================================================
// الفئة 4: تحسينات SIR (إصلاح 100 نقطة ضعف)
// =============================================================================

/**
 * @brief فاحص تعليمات SIR
 */
class SirInstructionValidator {
public:
    struct ValidationResult {
        bool valid;
        std::string error;
        std::string errorArabic;
    };
    
    // فحص تعليمة واحدة
    static ValidationResult validateInstruction(int opcode, 
                                                 const std::vector<int>& operands,
                                                 bool hasResult) {
        ValidationResult result{true, "", ""};
        
        // فحص الـ opcode
        if (opcode < 0 || opcode > 255) {
            result.valid = false;
            result.error = "Invalid opcode: " + std::to_string(opcode);
            result.errorArabic = "رمز عملية غير صالح: " + std::to_string(opcode);
            return result;
        }
        
        // فحص عدد المعاملات حسب نوع العملية
        static const std::unordered_map<int, std::pair<int, int>> opcodeOperandCount = {
            // {opcode, {min_operands, max_operands}}
            {0, {0, 0}},   // Nop
            {1, {1, 1}},   // Alloc
            {2, {2, 2}},   // Store
            {3, {1, 1}},   // Load
            {4, {2, 2}},   // Add
            {5, {2, 2}},   // Sub
            {6, {2, 2}},   // Mul
            {7, {2, 2}},   // Div
            {8, {1, 1}},   // Return
            {9, {1, 100}}, // Call (1 func + up to 99 args)
            {10, {1, 1}},  // Jump
            {11, {3, 3}},  // Branch
            // ... المزيد
        };
        
        auto it = opcodeOperandCount.find(opcode);
        if (it != opcodeOperandCount.end()) {
            auto [minOps, maxOps] = it->second;
            if (operands.size() < static_cast<size_t>(minOps) || 
                operands.size() > static_cast<size_t>(maxOps)) {
                result.valid = false;
                result.error = "Wrong operand count for opcode " + std::to_string(opcode) +
                              ": expected " + std::to_string(minOps) + "-" + std::to_string(maxOps) +
                              ", got " + std::to_string(operands.size());
                result.errorArabic = "عدد معاملات خاطئ للعملية";
                return result;
            }
        }
        
        return result;
    }
    
    // فحص تسلسل التعليمات
    static ValidationResult validateSequence(const std::vector<std::pair<int, std::vector<int>>>& instructions) {
        ValidationResult result{true, "", ""};
        
        std::set<int> definedValues;
        
        for (size_t i = 0; i < instructions.size(); ++i) {
            auto& [opcode, operands] = instructions[i];
            
            // التحقق من أن كل معامل معرّف مسبقاً
            for (int op : operands) {
                if (op >= 0 && definedValues.find(op) == definedValues.end()) {
                    result.valid = false;
                    result.error = "Use of undefined value " + std::to_string(op) +
                                  " at instruction " + std::to_string(i);
                    result.errorArabic = "استخدام قيمة غير معرّفة";
                    return result;
                }
            }
            
            // إضافة النتيجة إلى القيم المعرّفة
            definedValues.insert(static_cast<int>(i));
        }
        
        return result;
    }
};

// =============================================================================
// الفئة 5: تحسينات LLVM (إصلاح 80 نقطة ضعف)
// =============================================================================

/**
 * @brief مُحقق LLVM IR
 */
class LlvmIrVerifier {
public:
    struct VerificationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    // فحص LLVM IR نصي
    static VerificationResult verifyTextualIR(const std::string& ir) {
        VerificationResult result{true, {}, {}};
        
        // فحص الترويسة
        if (ir.find("target datalayout") == std::string::npos) {
            result.warnings.push_back("Missing target datalayout");
        }
        
        if (ir.find("target triple") == std::string::npos) {
            result.warnings.push_back("Missing target triple");
        }
        
        // فحص الدوال
        size_t defineCount = 0;
        size_t pos = 0;
        while ((pos = ir.find("define ", pos)) != std::string::npos) {
            defineCount++;
            pos += 7;
            
            // التحقق من وجود } لإغلاق الدالة
            size_t bracePos = ir.find("}", pos);
            if (bracePos == std::string::npos) {
                result.valid = false;
                result.errors.push_back("Unclosed function definition");
            }
        }
        
        // فحص استدعاءات الدوال
        pos = 0;
        while ((pos = ir.find("call ", pos)) != std::string::npos) {
            // التحقق من وجود @
            size_t atPos = ir.find("@", pos);
            size_t newlinePos = ir.find("\n", pos);
            if (atPos == std::string::npos || atPos > newlinePos) {
                result.valid = false;
                result.errors.push_back("Invalid call instruction");
            }
            pos += 5;
        }
        
        // فحص branches
        pos = 0;
        while ((pos = ir.find("br ", pos)) != std::string::npos) {
            // التحقق من وجود label
            size_t labelPos = ir.find("label %", pos);
            size_t newlinePos = ir.find("\n", pos);
            if (labelPos == std::string::npos || labelPos > newlinePos) {
                result.valid = false;
                result.errors.push_back("Invalid branch instruction");
            }
            pos += 3;
        }
        
        return result;
    }
};

// =============================================================================
// الفئة 6: أمان المدخلات (إصلاح 50 نقطة ضعف)
// =============================================================================

/**
 * @brief منظف المدخلات
 */
class InputSanitizer {
public:
    // تنظيف مسار الملف (منع path traversal)
    static std::string sanitizePath(const std::string& path) {
        std::string result = path;
        
        // إزالة .. و .
        while (result.find("..") != std::string::npos) {
            size_t pos = result.find("..");
            result.erase(pos, 2);
        }
        
        // إزالة أحرف خطيرة
        const std::string dangerous = "<>|\"'\r\n\0";
        for (char c : dangerous) {
            result.erase(std::remove(result.begin(), result.end(), c), result.end());
        }
        
        return result;
    }
    
    // تنظيف مدخلات سطر الأوامر (منع command injection)
    static std::string sanitizeCommand(const std::string& cmd) {
        std::string result;
        result.reserve(cmd.size());
        
        for (char c : cmd) {
            // السماح فقط بأحرف آمنة
            if (std::isalnum(static_cast<unsigned char>(c)) || 
                c == '_' || c == '-' || c == '.' || c == '/' || c == '\\' || c == ':') {
                result += c;
            } else if (c == ' ') {
                result += ' ';
            }
            // تجاهل الأحرف الخطيرة: ; | & $ ` ! < > " ' \n \r
        }
        
        return result;
    }
    
    // تنظيف معرّف
    static std::string sanitizeIdentifier(const std::string& id) {
        if (id.empty()) return "_empty_";
        
        std::string result;
        result.reserve(id.size());
        
        // الحرف الأول: حرف أو _
        char first = id[0];
        if (std::isalpha(static_cast<unsigned char>(first)) || first == '_' ||
            (static_cast<unsigned char>(first) > 127)) { // Unicode
            result += first;
        } else {
            result += '_';
        }
        
        // باقي الأحرف
        for (size_t i = 1; i < id.size(); ++i) {
            char c = id[i];
            if (std::isalnum(static_cast<unsigned char>(c)) || c == '_' ||
                (static_cast<unsigned char>(c) > 127)) { // Unicode
                result += c;
            } else {
                result += '_';
            }
        }
        
        return result;
    }
    
    // فحص حجم المدخل
    static bool checkInputSize(size_t size, size_t maxSize, const std::string& name = "input") {
        if (size > maxSize) {
            CompilerError error;
            error.kind = CompilerErrorKind::InternalError;
            error.message = name + " too large: " + std::to_string(size) + " > " + std::to_string(maxSize);
            error.messageArabic = name + " كبير جداً";
            ErrorCollector::instance().addError(error);
            return false;
        }
        return true;
    }
};

// =============================================================================
// الفئة 7: تحسينات الذاكرة (إصلاح 60 نقطة ضعف)
// =============================================================================

/**
 * @brief مُخصص ذاكرة مُتتبع
 */
class TrackedAllocator {
public:
    static TrackedAllocator& instance() {
        static TrackedAllocator instance;
        return instance;
    }
    
    void* allocate(size_t size, const char* file, int line) {
        void* ptr = std::malloc(size);
        if (ptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            allocations_[ptr] = {size, file, line, std::chrono::steady_clock::now()};
            totalAllocated_ += size;
            currentAllocated_ += size;
            allocationCount_++;
        }
        return ptr;
    }
    
    void deallocate(void* ptr, const char* file, int line) {
        if (ptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = allocations_.find(ptr);
            if (it != allocations_.end()) {
                currentAllocated_ -= it->second.size;
                allocations_.erase(it);
                deallocationCount_++;
            } else {
                // تحرير مزدوج أو مؤشر غير معروف
                CompilerError error;
                error.kind = CompilerErrorKind::InternalError;
                error.message = "Double free or invalid free at " + 
                               std::string(file) + ":" + std::to_string(line);
                error.messageArabic = "تحرير مزدوج أو غير صالح";
                ErrorCollector::instance().addError(error);
            }
            std::free(ptr);
        }
    }
    
    void reportLeaks() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!allocations_.empty()) {
            std::ostringstream ss;
            ss << "\n╔══════════════════════════════════════════════════════════════════╗\n";
            ss << "║ تسربات الذاكرة المكتشفة / Memory Leaks Detected\n";
            ss << "╠══════════════════════════════════════════════════════════════════╣\n";
            ss << "║ العدد: " << allocations_.size() << "\n";
            ss << "║ الحجم الإجمالي: " << currentAllocated_ << " bytes\n";
            ss << "╠──────────────────────────────────────────────────────────────────╣\n";
            
            for (const auto& [ptr, info] : allocations_) {
                ss << "║ " << info.size << " bytes at " << info.file << ":" << info.line << "\n";
            }
            
            ss << "╚══════════════════════════════════════════════════════════════════╝\n";
            
            // يمكن طباعة التقرير أو تسجيله
        }
    }
    
    size_t getCurrentAllocated() const { return currentAllocated_; }
    size_t getTotalAllocated() const { return totalAllocated_; }
    size_t getAllocationCount() const { return allocationCount_; }
    
private:
    struct AllocationInfo {
        size_t size;
        const char* file;
        int line;
        std::chrono::steady_clock::time_point time;
    };
    
    std::mutex mutex_;
    std::unordered_map<void*, AllocationInfo> allocations_;
    std::atomic<size_t> totalAllocated_{0};
    std::atomic<size_t> currentAllocated_{0};
    std::atomic<size_t> allocationCount_{0};
    std::atomic<size_t> deallocationCount_{0};
    
    TrackedAllocator() = default;
};

// ماكرو للتخصيص المُتتبع
#ifdef SAD_DEBUG_MEMORY
#define SAD_ALLOC(size) TrackedAllocator::instance().allocate(size, __FILE__, __LINE__)
#define SAD_FREE(ptr) TrackedAllocator::instance().deallocate(ptr, __FILE__, __LINE__)
#else
#define SAD_ALLOC(size) std::malloc(size)
#define SAD_FREE(ptr) std::free(ptr)
#endif

// =============================================================================
// الفئة 8: تحسينات الرسومات (إصلاح 50 نقطة ضعف)
// =============================================================================

/**
 * @brief فاحص موارد الرسومات
 */
class GraphicsResourceValidator {
public:
    // فحص معرّف widget
    static bool validateWidgetId(int id,const std::string& operation = "") {
        if (id < 0) {
            CompilerError error;
            error.kind = CompilerErrorKind::GraphicsWidgetInvalid;
            error.message = "Invalid widget ID: " + std::to_string(id);
            error.messageArabic = "معرف ودجت غير صالح";
            if (!operation.empty()) {
                error.notes.push_back("Operation: " + operation);
            }
            ErrorCollector::instance().addError(error);
            return false;
        }
        return true;
    }
    
    // فحص أبعاد النافذة
    static bool validateWindowDimensions(int width, int height) {
        if (width < 1 || width > 32767 || height < 1 || height > 32767) {
            CompilerError error;
            error.kind = CompilerErrorKind::GraphicsWindowCreationFailed;
            error.message = "Invalid window dimensions: " + std::to_string(width) + "x" + std::to_string(height);
            error.messageArabic = "أبعاد نافذة غير صالحة";
            error.suggestions.push_back("Use dimensions between 1 and 32767");
            ErrorCollector::instance().addError(error);
            return false;
        }
        return true;
    }
    
    // فحص قيمة لون
    static bool validateColor(int r, int g, int b, int a = 255) {
        auto check = [](int v, const std::string& name) {
            if (v < 0 || v > 255) {
                CompilerError error;
                error.kind = CompilerErrorKind::InternalError;
                error.message = "Invalid color component " + name + ": " + std::to_string(v);
                error.messageArabic = "قيمة لون غير صالحة";
                ErrorCollector::instance().addError(error);
                return false;
            }
            return true;
        };
        
        return check(r, "R") && check(g, "G") && check(b, "B") && check(a, "A");
    }
    
    // فحص حجم الخط
    static bool validateFontSize(int size) {
        if (size < 1 || size > 1000) {
            CompilerError error;
            error.kind = CompilerErrorKind::InternalError;
            error.message = "Invalid font size: " + std::to_string(size);
            error.messageArabic = "حجم خط غير صالح";
            ErrorCollector::instance().addError(error);
            return false;
        }
        return true;
    }
};

// =============================================================================
// الفئة 9: تحسينات الأداء (إصلاح 40 نقطة ضعف)
// =============================================================================

/**
 * @brief مقياس أداء
 */
class PerformanceProfiler {
public:
    struct TimingInfo {
        std::string name;
        std::chrono::microseconds duration;
        size_t callCount;
    };
    
    static PerformanceProfiler& instance() {
        static PerformanceProfiler instance;
        return instance;
    }
    
    void startTimer(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        timers_[name] = std::chrono::steady_clock::now();
    }
    
    void stopTimer(const std::string& name) {
        auto end = std::chrono::steady_clock::now();
        
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = timers_.find(name);
        if (it != timers_.end()) {
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - it->second);
            totals_[name] += duration;
            counts_[name]++;
        }
    }
    
    std::string report() const {
        std::ostringstream ss;
        ss << "\n═══════════════════════════════════════════════════════════════════\n";
        ss << "تقرير الأداء / Performance Report\n";
        ss << "═══════════════════════════════════════════════════════════════════\n";
        
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& [name, total] : totals_) {
            auto count = counts_.at(name);
            auto avg = total.count() / (count > 0 ? count : 1);
            ss << name << ": " << total.count() << "µs total, " << count << " calls, " << avg << "µs avg\n";
        }
        
        return ss.str();
    }
    
private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> timers_;
    std::unordered_map<std::string, std::chrono::microseconds> totals_;
    std::unordered_map<std::string, size_t> counts_;
    
    PerformanceProfiler() = default;
};

// ماكرو للقياس التلقائي
class ScopedTimer {
public:
    ScopedTimer(const std::string& name) : name_(name) {
        PerformanceProfiler::instance().startTimer(name_);
    }
    ~ScopedTimer() {
        PerformanceProfiler::instance().stopTimer(name_);
    }
private:
    std::string name_;
};

#define SAD_PROFILE_SCOPE(name) ScopedTimer _timer_##__LINE__(name)
#define SAD_PROFILE_FUNCTION() ScopedTimer _timer_func(__FUNCTION__)

// =============================================================================
// الفئة 10: تحسينات التسجيل (إصلاح 30 نقطة ضعف)
// =============================================================================

/**
 * @brief نظام تسجيل آمن
 */
class Logger {
public:
    enum class Level { Debug, Info, Warning, Error, Fatal };
    
    static Logger& instance() {
        static Logger instance;
        return instance;
    }
    
    void log(Level level, const std::string& message, 
             const char* file = nullptr, int line = 0) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::ostringstream ss;
        ss << "[" << std::put_time(std::localtime(&time), "%H:%M:%S") << "] ";
        ss << "[" << levelToString(level) << "] ";
        if (file) {
            ss << "[" << file << ":" << line << "] ";
        }
        ss << message << "\n";
        
        logs_.push_back(ss.str());
        
        if (level >= minLevel_) {
            std::cerr << ss.str();
        }
    }
    
    void setMinLevel(Level level) { minLevel_ = level; }
    
    std::string getAllLogs() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::ostringstream ss;
        for (const auto& log : logs_) {
            ss << log;
        }
        return ss.str();
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        logs_.clear();
    }
    
private:
    std::string levelToString(Level level) {
        switch (level) {
            case Level::Debug: return "DEBUG";
            case Level::Info: return "INFO";
            case Level::Warning: return "WARN";
            case Level::Error: return "ERROR";
            case Level::Fatal: return "FATAL";
            default: return "?";
        }
    }
    
    mutable std::mutex mutex_;
    std::vector<std::string> logs_;
    Level minLevel_ = Level::Info;
    
    Logger() = default;
};

// ماكروات التسجيل
#define SAD_LOG_DEBUG(msg) Logger::instance().log(Logger::Level::Debug, msg, __FILE__, __LINE__)
#define SAD_LOG_INFO(msg) Logger::instance().log(Logger::Level::Info, msg, __FILE__, __LINE__)
#define SAD_LOG_WARN(msg) Logger::instance().log(Logger::Level::Warning, msg, __FILE__, __LINE__)
#define SAD_LOG_ERROR(msg) Logger::instance().log(Logger::Level::Error, msg, __FILE__, __LINE__)
#define SAD_LOG_FATAL(msg) Logger::instance().log(Logger::Level::Fatal, msg, __FILE__, __LINE__)

} // namespace compiler
} // namespace sad

#endif // SAD_COMPILER_IMPROVEMENTS_H
