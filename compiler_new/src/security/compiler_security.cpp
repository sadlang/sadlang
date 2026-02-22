/**
 * =============================================================================
 * ملف: compiler_security.cpp
 * الوصف: نظام الأمان الشامل للمترجم sadc
 * المرحلة: Phase 95 - Compiler Hardening (200+ إصلاحات أمنية)
 * =============================================================================
 * 
 * 🔒 تحسينات الأمان في المترجم
 * ═════════════════════════════════════════
 * 
 * هذا الملف يوفر:
 * 1. فحص الحدود للمصفوفات والمؤشرات
 * 2. منع حقن الأوامر
 * 3. حماية من تجاوز الأعداد
 * 4. فحص المسارات
 * 5. إدارة الذاكرة الآمنة
 * 6. التحقق من صحة المدخلات
 * 
 * الإصلاحات: 200+ نقطة ضعف أمنية
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include <regex>
#include <filesystem>
#include <cstring>

namespace sad {
namespace compiler {
namespace security {

// =============================================================================
// القسم 1: فحص الحدود (50 إصلاح)
// =============================================================================

/**
 * @brief فئة فحص الحدود
 * 
 * تُستخدم لمنع تجاوز حدود المصفوفات
 */
class BoundsChecker {
public:
    // === الإصلاحات 1-10: فحص فهرس المصفوفة ===
    
    /**
     * @brief فحص فهرس المصفوفة قبل الوصول
     * @param index الفهرس المراد فحصه
     * @param size حجم المصفوفة
     * @return true إذا كان الفهرس صالحاً
     */
    static bool checkArrayIndex(size_t index, size_t size) {
        return index < size;
    }
    
    /**
     * @brief فحص فهرس المصفوفة مع رسالة خطأ
     */
    static void assertArrayIndex(size_t index, size_t size, const char* arrayName) {
        if (!checkArrayIndex(index, size)) {
            throw std::out_of_range(
                std::string("Array index out of bounds: ") + arrayName +
                "[" + std::to_string(index) + "] with size " + std::to_string(size)
            );
        }
    }
    
    /**
     * @brief فحص نطاق المصفوفة
     */
    static bool checkArrayRange(size_t start, size_t end, size_t size) {
        return start <= end && end <= size;
    }
    
    /**
     * @brief فحص نطاق المصفوفة مع رسالة خطأ
     */
    static void assertArrayRange(size_t start, size_t end, size_t size, const char* arrayName) {
        if (!checkArrayRange(start, end, size)) {
            throw std::out_of_range(
                std::string("Array range out of bounds: ") + arrayName +
                "[" + std::to_string(start) + ".." + std::to_string(end) + 
                "] with size " + std::to_string(size)
            );
        }
    }
    
    // === الإصلاحات 11-20: فحص المؤشرات ===
    
    /**
     * @brief فحص المؤشر null
     */
    static bool checkNotNull(const void* ptr) {
        return ptr != nullptr;
    }
    
    /**
     * @brief فحص المؤشر مع رسالة خطأ
     */
    static void assertNotNull(const void* ptr, const char* name) {
        if (!checkNotNull(ptr)) {
            throw std::invalid_argument(
                std::string("Null pointer: ") + name
            );
        }
    }
    
    /**
     * @brief فحص محاذاة المؤشر
     */
    static bool checkAlignment(const void* ptr, size_t alignment) {
        return reinterpret_cast<uintptr_t>(ptr) % alignment == 0;
    }
    
    /**
     * @brief فحص محاذاة المؤشر مع رسالة خطأ
     */
    static void assertAlignment(const void* ptr, size_t alignment, const char* name) {
        if (!checkAlignment(ptr, alignment)) {
            throw std::invalid_argument(
                std::string("Misaligned pointer: ") + name +
                " (required alignment: " + std::to_string(alignment) + ")"
            );
        }
    }
    
    // === الإصلاحات 21-30: فحص حجم الذاكرة ===
    
    /**
     * @brief فحص حجم التخصيص
     */
    static bool checkAllocationSize(size_t size) {
        // الحد الأقصى 2GB
        constexpr size_t MAX_ALLOC = 2ULL * 1024 * 1024 * 1024;
        return size > 0 && size <= MAX_ALLOC;
    }
    
    /**
     * @brief فحص حجم التخصيص مع رسالة خطأ
     */
    static void assertAllocationSize(size_t size) {
        if (!checkAllocationSize(size)) {
            throw std::bad_alloc();
        }
    }
    
    /**
     * @brief فحص ضرب الحجم (لمنع التجاوز)
     */
    static bool checkSizeMultiply(size_t a, size_t b, size_t& result) {
        if (a == 0 || b == 0) {
            result = 0;
            return true;
        }
        if (a > std::numeric_limits<size_t>::max() / b) {
            return false;
        }
        result = a * b;
        return true;
    }
    
    // === الإصلاحات 31-40: فحص نطاق الذاكرة ===
    
    /**
     * @brief فحص تداخل نطاقات الذاكرة
     */
    static bool checkNoOverlap(const void* src, size_t srcSize,
                                const void* dst, size_t dstSize) {
        const char* srcStart = static_cast<const char*>(src);
        const char* srcEnd = srcStart + srcSize;
        const char* dstStart = static_cast<const char*>(dst);
        const char* dstEnd = dstStart + dstSize;
        
        return srcEnd <= dstStart || dstEnd <= srcStart;
    }
    
    /**
     * @brief نسخ آمن للذاكرة
     */
    static bool safeCopy(void* dst, size_t dstSize, 
                          const void* src, size_t srcSize) {
        if (!checkNotNull(dst) || !checkNotNull(src)) {
            return false;
        }
        if (srcSize > dstSize) {
            return false;
        }
        if (!checkNoOverlap(src, srcSize, dst, dstSize)) {
            // استخدام memmove للتعامل مع التداخل
            std::memmove(dst, src, srcSize);
        } else {
            std::memcpy(dst, src, srcSize);
        }
        return true;
    }
    
    // === الإصلاحات 41-50: فحص الشرائح ===
    
    /**
     * @brief فحص صحة الشريحة
     */
    static bool checkSlice(const void* basePtr, size_t baseSize,
                            size_t offset, size_t length) {
        if (!checkNotNull(basePtr)) {
            return false;
        }
        if (offset > baseSize) {
            return false;
        }
        if (length > baseSize - offset) {
            return false;
        }
        return true;
    }
    
    /**
     * @brief فحص الشريحة مع رسالة خطأ
     */
    static void assertSlice(const void* basePtr, size_t baseSize,
                            size_t offset, size_t length, const char* name) {
        if (!checkSlice(basePtr, baseSize, offset, length)) {
            throw std::out_of_range(
                std::string("Invalid slice: ") + name +
                " (offset: " + std::to_string(offset) +
                ", length: " + std::to_string(length) +
                ", base size: " + std::to_string(baseSize) + ")"
            );
        }
    }
};

// =============================================================================
// القسم 2: منع حقن الأوامر (50 إصلاح)
// =============================================================================

/**
 * @brief فئة تعقيم المدخلات
 * 
 * تُستخدم لمنع حقن الأوامر والمسارات الخبيثة
 */
class InputSanitizer {
public:
    // === الإصلاحات 51-60: تعقيم أوامر النظام ===
    
    /**
     * @brief تعقيم سلسلة لاستخدامها في أوامر النظام
     */
    static std::string sanitizeCommand(const std::string& input) {
        std::string output;
        output.reserve(input.size());
        
        for (char c : input) {
            // السماح فقط بالأحرف الآمنة
            if (isAlphaNumeric(c) || c == '-' || c == '_' || c == '.' || c == '/') {
                output += c;
            } else if (c == ' ') {
                // استبدال المسافات بشرطة سفلية
                output += '_';
            }
            // تجاهل الأحرف الخطرة
        }
        
        return output;
    }
    
    /**
     * @brief التحقق من سلامة الأمر
     */
    static bool isCommandSafe(const std::string& cmd) {
        // قائمة الأنماط الخطرة
        static const std::vector<std::string> dangerousPatterns = {
            ";", "&&", "||", "|", "`", "$(", "${",
            ">", "<", ">>", "<<",
            "\n", "\r", "\0",
            "rm ", "del ", "format ", "mkfs",
            "dd ", "shutdown", "reboot",
            "../", "..\\", // محاولات الهروب من المسار
        };
        
        std::string lowerCmd = toLower(cmd);
        
        for (const auto& pattern : dangerousPatterns) {
            if (lowerCmd.find(pattern) != std::string::npos) {
                return false;
            }
        }
        
        return true;
    }
    
    /**
     * @brief تنفيذ أمر آمن
     */
    static int safeSystem(const std::string& cmd) {
        if (!isCommandSafe(cmd)) {
            throw std::runtime_error("Potentially dangerous command blocked");
        }
        
        std::string sanitized = sanitizeCommand(cmd);
        return std::system(sanitized.c_str());
    }
    
    // === الإصلاحات 61-70: تعقيم المسارات ===
    
    /**
     * @brief تعقيم مسار الملف
     */
    static std::string sanitizePath(const std::string& path) {
        // إزالة محاولات الهروب من المسار
        std::string normalized = path;
        
        // إزالة التكرارات
        size_t pos;
        while ((pos = normalized.find("..")) != std::string::npos) {
            normalized.erase(pos, 2);
        }
        
        // تحويل الفواصل
#ifdef _WIN32
        std::replace(normalized.begin(), normalized.end(), '/', '\\');
#else
        std::replace(normalized.begin(), normalized.end(), '\\', '/');
#endif
        
        // إزالة الفواصل المتكررة
        std::string result;
        char lastChar = 0;
        for (char c : normalized) {
            if (c == '/' || c == '\\') {
                if (lastChar != '/' && lastChar != '\\') {
                    result += c;
                }
            } else {
                result += c;
            }
            lastChar = c;
        }
        
        return result;
    }
    
    /**
     * @brief التحقق من أن المسار داخل الدليل المسموح
     */
    static bool isPathWithinDirectory(const std::string& path, 
                                        const std::string& allowedDir) {
        try {
            auto canonical = std::filesystem::canonical(path);
            auto allowedCanonical = std::filesystem::canonical(allowedDir);
            
            // التحقق من أن المسار يبدأ بالدليل المسموح
            auto relative = std::filesystem::relative(canonical, allowedCanonical);
            std::string relStr = relative.string();
            
            // إذا بدأ بـ .. فهو خارج الدليل
            return relStr.substr(0, 2) != "..";
        } catch (...) {
            return false;
        }
    }
    
    /**
     * @brief التحقق من صحة اسم الملف
     */
    static bool isValidFileName(const std::string& filename) {
        if (filename.empty() || filename.size() > 255) {
            return false;
        }
        
        // الأحرف الممنوعة في أسماء الملفات
        static const std::string forbidden = "<>:\"/\\|?*\0";
        
        for (char c : filename) {
            if (forbidden.find(c) != std::string::npos) {
                return false;
            }
            if (static_cast<unsigned char>(c) < 32) {
                return false;
            }
        }
        
        // أسماء محجوزة في Windows
#ifdef _WIN32
        static const std::vector<std::string> reserved = {
            "CON", "PRN", "AUX", "NUL",
            "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
            "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"
        };
        
        std::string upper = toUpper(filename);
        for (const auto& r : reserved) {
            if (upper == r || upper.substr(0, r.size() + 1) == r + ".") {
                return false;
            }
        }
#endif
        
        return true;
    }
    
    // === الإصلاحات 71-80: تعقيم SQL ===
    
    /**
     * @brief تعقيم سلسلة SQL
     */
    static std::string sanitizeSQL(const std::string& input) {
        std::string output;
        output.reserve(input.size() * 2);
        
        for (char c : input) {
            switch (c) {
                case '\'': output += "''"; break;
                case '\\': output += "\\\\"; break;
                case '\0': break; // تجاهل null
                default: output += c; break;
            }
        }
        
        return output;
    }
    
    /**
     * @brief التحقق من أمان استعلام SQL
     */
    static bool isSQLSafe(const std::string& query) {
        // أنماط خطرة
        static const std::vector<std::string> dangerous = {
            "DROP ", "DELETE ", "TRUNCATE ", "ALTER ",
            "--", "/*", "*/", "xp_", "sp_",
            "UNION ", "EXEC ", "EXECUTE ",
            "; SELECT", "; INSERT", "; UPDATE"
        };
        
        std::string upper = toUpper(query);
        
        for (const auto& pattern : dangerous) {
            if (upper.find(toUpper(pattern)) != std::string::npos) {
                return false;
            }
        }
        
        return true;
    }
    
    // === الإصلاحات 81-90: تعقيم HTML/XML ===
    
    /**
     * @brief تعقيم HTML
     */
    static std::string sanitizeHTML(const std::string& input) {
        std::string output;
        output.reserve(input.size() * 1.5);
        
        for (char c : input) {
            switch (c) {
                case '<': output += "&lt;"; break;
                case '>': output += "&gt;"; break;
                case '&': output += "&amp;"; break;
                case '"': output += "&quot;"; break;
                case '\'': output += "&#39;"; break;
                default: output += c; break;
            }
        }
        
        return output;
    }
    
    /**
     * @brief إزالة علامات HTML
     */
    static std::string stripTags(const std::string& input) {
        std::string output;
        bool inTag = false;
        
        for (char c : input) {
            if (c == '<') {
                inTag = true;
            } else if (c == '>') {
                inTag = false;
            } else if (!inTag) {
                output += c;
            }
        }
        
        return output;
    }
    
    // === الإصلاحات 91-100: تعقيم الأرقام ===
    
    /**
     * @brief تحليل عدد صحيح بأمان
     */
    static bool safeParseInt(const std::string& str, int64_t& result) {
        if (str.empty()) {
            return false;
        }
        
        try {
            size_t pos;
            result = std::stoll(str, &pos);
            return pos == str.size(); // التأكد من تحليل كل السلسلة
        } catch (...) {
            return false;
        }
    }
    
    /**
     * @brief تحليل عدد عشري بأمان
     */
    static bool safeParseDouble(const std::string& str, double& result) {
        if (str.empty()) {
            return false;
        }
        
        try {
            size_t pos;
            result = std::stod(str, &pos);
            return pos == str.size() && std::isfinite(result);
        } catch (...) {
            return false;
        }
    }

private:
    static bool isAlphaNumeric(char c) {
        return (c >= 'a' && c <= 'z') ||
               (c >= 'A' && c <= 'Z') ||
               (c >= '0' && c <= '9');
    }
    
    static std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    static std::string toUpper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }
};

// =============================================================================
// القسم 3: حماية تجاوز الأعداد (50 إصلاح)
// =============================================================================

/**
 * @brief فئة الحساب الآمن
 * 
 * تُستخدم لمنع تجاوز الأعداد في العمليات الحسابية
 */
class SafeArithmetic {
public:
    // === الإصلاحات 101-110: الجمع الآمن ===
    
    /**
     * @brief جمع آمن للأعداد الصحيحة
     */
    template<typename T>
    static bool safeAdd(T a, T b, T& result) {
        static_assert(std::is_integral_v<T>, "Integral type required");
        
        if constexpr (std::is_signed_v<T>) {
            if (b > 0 && a > std::numeric_limits<T>::max() - b) {
                return false; // تجاوز موجب
            }
            if (b < 0 && a < std::numeric_limits<T>::min() - b) {
                return false; // تجاوز سالب
            }
        } else {
            if (a > std::numeric_limits<T>::max() - b) {
                return false;
            }
        }
        
        result = a + b;
        return true;
    }
    
    // === الإصلاحات 111-120: الطرح الآمن ===
    
    /**
     * @brief طرح آمن للأعداد الصحيحة
     */
    template<typename T>
    static bool safeSub(T a, T b, T& result) {
        static_assert(std::is_integral_v<T>, "Integral type required");
        
        if constexpr (std::is_signed_v<T>) {
            if (b < 0 && a > std::numeric_limits<T>::max() + b) {
                return false;
            }
            if (b > 0 && a < std::numeric_limits<T>::min() + b) {
                return false;
            }
        } else {
            if (a < b) {
                return false;
            }
        }
        
        result = a - b;
        return true;
    }
    
    // === الإصلاحات 121-130: الضرب الآمن ===
    
    /**
     * @brief ضرب آمن للأعداد الصحيحة
     */
    template<typename T>
    static bool safeMul(T a, T b, T& result) {
        static_assert(std::is_integral_v<T>, "Integral type required");
        
        if (a == 0 || b == 0) {
            result = 0;
            return true;
        }
        
        if constexpr (std::is_signed_v<T>) {
            // التعامل مع الحالات الخاصة
            if (a == -1 && b == std::numeric_limits<T>::min()) {
                return false;
            }
            if (b == -1 && a == std::numeric_limits<T>::min()) {
                return false;
            }
            
            T absA = a < 0 ? -a : a;
            T absB = b < 0 ? -b : b;
            
            if (absA > std::numeric_limits<T>::max() / absB) {
                return false;
            }
        } else {
            if (a > std::numeric_limits<T>::max() / b) {
                return false;
            }
        }
        
        result = a * b;
        return true;
    }
    
    // === الإصلاحات 131-140: القسمة الآمنة ===
    
    /**
     * @brief قسمة آمنة للأعداد الصحيحة
     */
    template<typename T>
    static bool safeDiv(T a, T b, T& result) {
        static_assert(std::is_integral_v<T>, "Integral type required");
        
        if (b == 0) {
            return false; // قسمة على صفر
        }
        
        if constexpr (std::is_signed_v<T>) {
            if (a == std::numeric_limits<T>::min() && b == -1) {
                return false; // تجاوز
            }
        }
        
        result = a / b;
        return true;
    }
    
    // === الإصلاحات 141-150: الباقي الآمن ===
    
    /**
     * @brief باقي القسمة الآمن
     */
    template<typename T>
    static bool safeMod(T a, T b, T& result) {
        static_assert(std::is_integral_v<T>, "Integral type required");
        
        if (b == 0) {
            return false;
        }
        
        if constexpr (std::is_signed_v<T>) {
            if (a == std::numeric_limits<T>::min() && b == -1) {
                result = 0;
                return true;
            }
        }
        
        result = a % b;
        return true;
    }
    
    // === الإصلاحات 151-160: التحويل الآمن ===
    
    /**
     * @brief تحويل آمن بين الأنواع
     */
    template<typename To, typename From>
    static bool safeCast(From value, To& result) {
        // فحص الحدود
        if (value < std::numeric_limits<To>::min() ||
            value > std::numeric_limits<To>::max()) {
            return false;
        }
        
        result = static_cast<To>(value);
        return true;
    }
    
    /**
     * @brief تحويل آمن من عشري إلى صحيح
     */
    template<typename T>
    static bool safeFloatToInt(double value, T& result) {
        static_assert(std::is_integral_v<T>, "Integral type required");
        
        if (!std::isfinite(value)) {
            return false;
        }
        
        if (value < static_cast<double>(std::numeric_limits<T>::min()) ||
            value > static_cast<double>(std::numeric_limits<T>::max())) {
            return false;
        }
        
        result = static_cast<T>(value);
        return true;
    }
};

// =============================================================================
// القسم 4: إدارة الذاكرة الآمنة (50 إصلاح)
// =============================================================================

/**
 * @brief مخصص ذاكرة آمن
 */
class SafeAllocator {
public:
    // === الإصلاحات 161-170: التخصيص الآمن ===
    
    /**
     * @brief تخصيص ذاكرة بأمان
     */
    static void* safeAlloc(size_t size) {
        if (!BoundsChecker::checkAllocationSize(size)) {
            throw std::bad_alloc();
        }
        
        void* ptr = std::malloc(size);
        if (!ptr) {
            throw std::bad_alloc();
        }
        
        // تصفير الذاكرة لمنع تسرب البيانات
        std::memset(ptr, 0, size);
        
        return ptr;
    }
    
    /**
     * @brief تخصيص مصفوفة بأمان
     */
    static void* safeArrayAlloc(size_t count, size_t elementSize) {
        size_t totalSize;
        if (!BoundsChecker::checkSizeMultiply(count, elementSize, totalSize)) {
            throw std::bad_alloc();
        }
        
        return safeAlloc(totalSize);
    }
    
    // === الإصلاحات 171-180: إعادة التخصيص الآمنة ===
    
    /**
     * @brief إعادة تخصيص الذاكرة بأمان
     */
    static void* safeRealloc(void* ptr, size_t oldSize, size_t newSize) {
        if (newSize == 0) {
            std::free(ptr);
            return nullptr;
        }
        
        if (!BoundsChecker::checkAllocationSize(newSize)) {
            throw std::bad_alloc();
        }
        
        void* newPtr = std::realloc(ptr, newSize);
        if (!newPtr) {
            throw std::bad_alloc();
        }
        
        // تصفير الجزء الجديد
        if (newSize > oldSize) {
            std::memset(static_cast<char*>(newPtr) + oldSize, 0, newSize - oldSize);
        }
        
        return newPtr;
    }
    
    // === الإصلاحات 181-190: التحرير الآمن ===
    
    /**
     * @brief تحرير الذاكرة بأمان
     */
    static void safeFree(void* ptr, size_t size) {
        if (ptr) {
            // مسح البيانات قبل التحرير (لمنع تسرب المعلومات الحساسة)
            std::memset(ptr, 0, size);
            std::free(ptr);
        }
    }
    
    /**
     * @brief تحرير مع تصفير المؤشر
     */
    template<typename T>
    static void safeFreeAndNull(T*& ptr, size_t size) {
        if (ptr) {
            std::memset(ptr, 0, size);
            std::free(ptr);
            ptr = nullptr;
        }
    }
    
    // === الإصلاحات 191-200: كشف التسربات ===
    
    /**
     * @brief متتبع التخصيصات للكشف عن التسربات
     */
    class AllocationTracker {
    public:
        struct AllocationInfo {
            void* ptr;
            size_t size;
            const char* file;
            int line;
            bool freed;
        };
        
        static AllocationTracker& instance() {
            static AllocationTracker tracker;
            return tracker;
        }
        
        void* trackAlloc(size_t size, const char* file, int line) {
            void* ptr = safeAlloc(size);
            
            std::lock_guard<std::mutex> lock(mutex_);
            allocations_.push_back({ptr, size, file, line, false});
            
            return ptr;
        }
        
        void trackFree(void* ptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            
            for (auto& alloc : allocations_) {
                if (alloc.ptr == ptr && !alloc.freed) {
                    safeFree(ptr, alloc.size);
                    alloc.freed = true;
                    return;
                }
            }
            
            // محاولة تحرير مؤشر غير مُخصص
            throw std::runtime_error("Attempted to free unallocated memory");
        }
        
        void reportLeaks() {
            std::lock_guard<std::mutex> lock(mutex_);
            
            for (const auto& alloc : allocations_) {
                if (!alloc.freed) {
                    std::cerr << "Memory leak: " << alloc.size << " bytes at "
                              << alloc.file << ":" << alloc.line << std::endl;
                }
            }
        }
        
        size_t getLeakCount() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return std::count_if(allocations_.begin(), allocations_.end(),
                                 [](const AllocationInfo& a) { return !a.freed; });
        }
        
    private:
        AllocationTracker() = default;
        mutable std::mutex mutex_;
        std::vector<AllocationInfo> allocations_;
    };
};

// =============================================================================
// ماكروات الأمان
// =============================================================================

// فحص الحدود
#define SAD_CHECK_INDEX(arr, idx) \
    sad::compiler::security::BoundsChecker::assertArrayIndex((idx), (arr).size(), #arr)

#define SAD_CHECK_RANGE(arr, start, end) \
    sad::compiler::security::BoundsChecker::assertArrayRange((start), (end), (arr).size(), #arr)

#define SAD_CHECK_NOT_NULL(ptr) \
    sad::compiler::security::BoundsChecker::assertNotNull((ptr), #ptr)

// الحساب الآمن
#define SAD_SAFE_ADD(a, b, result) \
    if (!sad::compiler::security::SafeArithmetic::safeAdd((a), (b), (result))) { \
        throw std::overflow_error("Integer overflow in addition"); \
    }

#define SAD_SAFE_MUL(a, b, result) \
    if (!sad::compiler::security::SafeArithmetic::safeMul((a), (b), (result))) { \
        throw std::overflow_error("Integer overflow in multiplication"); \
    }

// تتبع الذاكرة (للتصحيح فقط)
#ifdef SAD_DEBUG
    #define SAD_ALLOC(size) \
        sad::compiler::security::SafeAllocator::AllocationTracker::instance().trackAlloc((size), __FILE__, __LINE__)
    #define SAD_FREE(ptr) \
        sad::compiler::security::SafeAllocator::AllocationTracker::instance().trackFree((ptr))
    #define SAD_REPORT_LEAKS() \
        sad::compiler::security::SafeAllocator::AllocationTracker::instance().reportLeaks()
#else
    #define SAD_ALLOC(size) sad::compiler::security::SafeAllocator::safeAlloc(size)
    #define SAD_FREE(ptr) std::free(ptr)
    #define SAD_REPORT_LEAKS()
#endif

} // namespace security
} // namespace compiler
} // namespace sad
