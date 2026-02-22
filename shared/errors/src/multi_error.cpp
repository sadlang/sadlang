/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: multi_error.cpp                                                      ║
 * ║   الوصف: نظام جمع الأخطاء المتعددة                                          ║
 * ║   المهمة: T164 - Phase 15 (US13)                                            ║
 * ║   المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                         دليل المبتدئين | Beginner's Guide
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 لماذا نجمع أخطاء متعددة؟ | Why Collect Multiple Errors?
 * ─────────────────────────────────────────────────────────────
 *
 * المترجم التقليدي يتوقف عند أول خطأ:
 *
 *    الكود:
 *    ─────
 *    متغير س = "نص" + 5        // خطأ 1: جمع نص مع عدد
 *    متغير ص = متغير_غير_موجود  // خطأ 2: متغير غير معرّف
 *    متغير ع = [1, 2, 3        // خطأ 3: قوس مفقود
 *
 *    ❌ المترجم التقليدي: يُظهر فقط الخطأ الأول
 *    ✅ نظامنا: يُظهر الأخطاء الثلاثة معاً!
 *
 * 💡 الفوائد:
 * ─────────────────────────────────────────────────────────────
 *
 *    1️⃣ توفير الوقت: تصلح كل الأخطاء مرة واحدة
 *    2️⃣ فهم أفضل: ترى الصورة الكاملة للمشاكل
 *    3️⃣ تعلم أسرع: تفهم أنماط الأخطاء المتكررة
 *
 * 🔧 كيف يعمل النظام؟
 * ─────────────────────────────────────────────────────────────
 *
 *    ┌──────────┐    ┌──────────┐    ┌──────────┐
 *    │ الكود    │ -> │ المحلل   │ -> │ الأخطاء  │
 *    └──────────┘    └──────────┘    └──────────┘
 *                          │
 *                    ┌─────┴─────┐
 *                    │ جامع      │
 *                    │ الأخطاء   │
 *                    └───────────┘
 *                          │
 *              ┌───────────┼───────────┐
 *              ▼           ▼           ▼
 *          [خطأ 1]    [خطأ 2]    [خطأ 3]
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <sstream>
#include <algorithm>
#include <chrono>

namespace sad {
namespace errors {

// ═══════════════════════════════════════════════════════════════════════════════
//                         مستويات الخطورة | Severity Levels
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📊 ErrorSeverity - مستوى خطورة الخطأ
 */
enum class ErrorSeverity {
    /**
     * 💡 HINT - تلميح
     * 
     * ليس خطأ فعلياً، لكنه اقتراح للتحسين
     * مثال: "يمكنك استخدام const بدلاً من var"
     */
    HINT,
    
    /**
     * ℹ️ INFO - معلومة
     * 
     * معلومة مفيدة، لا تؤثر على التشغيل
     * مثال: "هذه الدالة لم تُستخدم"
     */
    INFO,
    
    /**
     * ⚠️ WARNING - تحذير
     * 
     * قد يسبب مشاكل، لكن الكود سيعمل
     * مثال: "قسمة على صفر محتملة"
     */
    WARNING,
    
    /**
     * ❌ ERROR - خطأ
     * 
     * يجب إصلاحه، الكود لن يعمل
     * مثال: "متغير غير معرّف"
     */
    ERROR,
    
    /**
     * 🔥 FATAL - خطأ قاتل
     * 
     * لا يمكن الاستمرار بعده
     * مثال: "الملف غير موجود"
     */
    FATAL
};

/**
 * 🔤 تحويل المستوى إلى نص
 */
std::string severityToString(ErrorSeverity severity, bool arabic = true) {
    if (arabic) {
        switch (severity) {
            case ErrorSeverity::HINT: return "تلميح";
            case ErrorSeverity::INFO: return "معلومة";
            case ErrorSeverity::WARNING: return "تحذير";
            case ErrorSeverity::ERROR: return "خطأ";
            case ErrorSeverity::FATAL: return "خطأ قاتل";
        }
    } else {
        switch (severity) {
            case ErrorSeverity::HINT: return "hint";
            case ErrorSeverity::INFO: return "info";
            case ErrorSeverity::WARNING: return "warning";
            case ErrorSeverity::ERROR: return "error";
            case ErrorSeverity::FATAL: return "fatal";
        }
    }
    return "";
}

/**
 * 🎨 رمز emoji للمستوى
 */
std::string severityEmoji(ErrorSeverity severity) {
    switch (severity) {
        case ErrorSeverity::HINT: return "💡";
        case ErrorSeverity::INFO: return "ℹ️";
        case ErrorSeverity::WARNING: return "⚠️";
        case ErrorSeverity::ERROR: return "❌";
        case ErrorSeverity::FATAL: return "🔥";
    }
    return "";
}

// ═══════════════════════════════════════════════════════════════════════════════
//                         فئات الأخطاء | Error Categories
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📋 ErrorCategory - تصنيف الخطأ
 */
enum class ErrorCategory {
    LEXER,      // أخطاء التحليل المعجمي
    PARSER,     // أخطاء التحليل النحوي
    SEMANTIC,   // أخطاء دلالية
    TYPE,       // أخطاء الأنواع
    RUNTIME,    // أخطاء وقت التشغيل
    IO,         // أخطاء الملفات
    INTERNAL    // أخطاء داخلية
};

// ═══════════════════════════════════════════════════════════════════════════════
//                         هيكل الخطأ | Error Structure
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📍 SourceLocation - موقع في الكود المصدري
 */
struct SourceLocation {
    std::string filename;
    size_t line;
    size_t column;
    size_t endLine;
    size_t endColumn;
    
    SourceLocation()
        : line(1), column(1), endLine(1), endColumn(1) {}
    
    SourceLocation(const std::string& file, size_t l, size_t c)
        : filename(file), line(l), column(c), endLine(l), endColumn(c) {}
    
    bool operator==(const SourceLocation& other) const {
        return filename == other.filename && line == other.line && column == other.column;
    }
};

/**
 * 🔗 RelatedInfo - معلومات مرتبطة
 */
struct RelatedInfo {
    std::string message;
    SourceLocation location;
    
    RelatedInfo() = default;
    RelatedInfo(const std::string& msg, const SourceLocation& loc)
        : message(msg), location(loc) {}
};

/**
 * 📋 CompilerError - خطأ المترجم الكامل
 */
struct CompilerError {
    // معلومات أساسية
    std::string code;            // رمز الخطأ (E001)
    std::string message;         // الرسالة بالعربية
    std::string messageEn;       // الرسالة بالإنجليزية
    ErrorSeverity severity;
    ErrorCategory category;
    SourceLocation location;
    
    // السياق
    std::string sourceLine;      // سطر الكود
    std::vector<RelatedInfo> related;  // معلومات مرتبطة
    
    // البيانات الوصفية
    std::string timestamp;
    size_t errorId;
    
    CompilerError()
        : severity(ErrorSeverity::ERROR)
        , category(ErrorCategory::SEMANTIC)
        , errorId(0)
    {}
    
    /**
     * 📝 تنسيق الخطأ للعرض
     */
    std::string format(bool useArabic = true, bool showDetails = true) const {
        std::ostringstream oss;
        
        // الرأس
        oss << severityEmoji(severity) << " ";
        oss << "[" << code << "] ";
        oss << (useArabic ? message : messageEn) << "\n";
        
        // الموقع
        oss << "   ";
        if (!location.filename.empty()) {
            oss << location.filename << ":";
        }
        oss << location.line << ":" << location.column << "\n";
        
        // سطر الكود
        if (showDetails && !sourceLine.empty()) {
            oss << "\n";
            oss << "   " << location.line << " │ " << sourceLine << "\n";
            oss << "     │ ";
            for (size_t i = 1; i < location.column; i++) oss << " ";
            oss << "^";
            if (location.endColumn > location.column) {
                for (size_t i = location.column; i < location.endColumn - 1; i++) oss << "~";
            }
            oss << "\n";
        }
        
        // المعلومات المرتبطة
        if (showDetails && !related.empty()) {
            oss << "\n";
            for (const auto& rel : related) {
                oss << "   📎 " << rel.message << "\n";
                if (!rel.location.filename.empty()) {
                    oss << "      " << rel.location.filename << ":";
                    oss << rel.location.line << ":" << rel.location.column << "\n";
                }
            }
        }
        
        return oss.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                     جامع الأخطاء | Error Collector
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📦 MultiErrorCollector - جامع الأخطاء المتعددة
 *
 * يجمع ويرتب ويزيل المكرر من الأخطاء
 */
class MultiErrorCollector {
public:
    MultiErrorCollector()
        : nextErrorId_(1)
        , maxErrors_(100)
        , failOnFirstError_(false)
    {}
    
    /**
     * ➕ إضافة خطأ
     */
    void addError(CompilerError error) {
        // تعيين المعرف
        error.errorId = nextErrorId_++;
        
        // تعيين الوقت
        error.timestamp = getCurrentTimestamp();
        
        // فحص التكرار
        if (isDuplicate(error)) {
            return;
        }
        
        // فحص الحد الأقصى
        if (errors_.size() >= maxErrors_) {
            return;
        }
        
        // إضافة للقائمة
        errors_.push_back(error);
        
        // تحديث العدادات
        updateCounts(error.severity);
    }
    
    /**
     * ➕ إضافة خطأ بسيط
     */
    void add(
        const std::string& code,
        const std::string& message,
        const std::string& messageEn,
        ErrorSeverity severity,
        ErrorCategory category,
        const SourceLocation& location,
        const std::string& sourceLine = ""
    ) {
        CompilerError error;
        error.code = code;
        error.message = message;
        error.messageEn = messageEn;
        error.severity = severity;
        error.category = category;
        error.location = location;
        error.sourceLine = sourceLine;
        
        addError(error);
    }
    
    /**
     * 🔍 الحصول على كل الأخطاء
     */
    const std::vector<CompilerError>& getErrors() const {
        return errors_;
    }
    
    /**
     * 🔍 الحصول على أخطاء بمستوى معين
     */
    std::vector<CompilerError> getErrorsBySeverity(ErrorSeverity severity) const {
        std::vector<CompilerError> filtered;
        for (const auto& error : errors_) {
            if (error.severity == severity) {
                filtered.push_back(error);
            }
        }
        return filtered;
    }
    
    /**
     * 🔍 الحصول على أخطاء لملف معين
     */
    std::vector<CompilerError> getErrorsForFile(const std::string& filename) const {
        std::vector<CompilerError> filtered;
        for (const auto& error : errors_) {
            if (error.location.filename == filename) {
                filtered.push_back(error);
            }
        }
        return filtered;
    }
    
    /**
     * 📊 هل هناك أخطاء؟
     */
    bool hasErrors() const {
        return errorCount_ > 0;
    }
    
    /**
     * 📊 هل هناك أخطاء قاتلة؟
     */
    bool hasFatalErrors() const {
        return fatalCount_ > 0;
    }
    
    /**
     * 📊 عدد الأخطاء
     */
    size_t errorCount() const { return errorCount_; }
    size_t warningCount() const { return warningCount_; }
    size_t totalCount() const { return errors_.size(); }
    
    /**
     * 🧹 مسح الأخطاء
     */
    void clear() {
        errors_.clear();
        seenLocations_.clear();
        errorCount_ = 0;
        warningCount_ = 0;
        fatalCount_ = 0;
        nextErrorId_ = 1;
    }
    
    /**
     * 📝 تنسيق كل الأخطاء
     */
    std::string formatAll(bool useArabic = true) const {
        std::ostringstream oss;
        
        // الرأس
        oss << "\n";
        oss << "╔══════════════════════════════════════════════════════════════╗\n";
        if (useArabic) {
            oss << "║                    نتائج الترجمة | Compilation Results        ║\n";
        } else {
            oss << "║                    Compilation Results                        ║\n";
        }
        oss << "╠══════════════════════════════════════════════════════════════╣\n";
        
        // الملخص
        oss << "║   ";
        if (errorCount_ > 0) oss << "❌ " << errorCount_ << " أخطاء  ";
        if (warningCount_ > 0) oss << "⚠️ " << warningCount_ << " تحذيرات  ";
        if (errorCount_ == 0 && warningCount_ == 0) oss << "✅ لا أخطاء";
        oss << "\n";
        oss << "╚══════════════════════════════════════════════════════════════╝\n";
        
        // تجميع حسب الملف
        std::unordered_map<std::string, std::vector<const CompilerError*>> byFile;
        for (const auto& error : errors_) {
            byFile[error.location.filename].push_back(&error);
        }
        
        // عرض الأخطاء
        for (const auto& [filename, fileErrors] : byFile) {
            oss << "\n";
            oss << "📁 " << (filename.empty() ? "<بدون ملف>" : filename) << "\n";
            oss << "   ─────────────────────────────────────────────────────────\n";
            
            for (const auto* error : fileErrors) {
                oss << "\n" << error->format(useArabic, true);
            }
        }
        
        return oss.str();
    }
    
    /**
     * 📊 تقرير ملخص
     */
    std::string getSummary(bool useArabic = true) const {
        std::ostringstream oss;
        
        if (useArabic) {
            if (errors_.empty()) {
                oss << "✅ الترجمة نجحت بدون أخطاء\n";
            } else {
                oss << "نتيجة الترجمة: ";
                if (errorCount_ > 0) oss << errorCount_ << " أخطاء";
                if (errorCount_ > 0 && warningCount_ > 0) oss << "، ";
                if (warningCount_ > 0) oss << warningCount_ << " تحذيرات";
                oss << "\n";
            }
        } else {
            if (errors_.empty()) {
                oss << "✅ Compilation succeeded with no errors\n";
            } else {
                oss << "Compilation result: ";
                if (errorCount_ > 0) oss << errorCount_ << " errors";
                if (errorCount_ > 0 && warningCount_ > 0) oss << ", ";
                if (warningCount_ > 0) oss << warningCount_ << " warnings";
                oss << "\n";
            }
        }
        
        return oss.str();
    }
    
    /**
     * ⚙️ إعداد الحد الأقصى
     */
    void setMaxErrors(size_t max) { maxErrors_ = max; }
    void setFailOnFirstError(bool fail) { failOnFirstError_ = fail; }
    
private:
    std::vector<CompilerError> errors_;
    std::unordered_set<std::string> seenLocations_;
    size_t nextErrorId_;
    size_t maxErrors_;
    bool failOnFirstError_;
    
    // العدادات
    size_t errorCount_ = 0;
    size_t warningCount_ = 0;
    size_t fatalCount_ = 0;
    
    /**
     * 🔍 فحص التكرار
     */
    bool isDuplicate(const CompilerError& error) const {
        std::string key = error.location.filename + ":" +
                         std::to_string(error.location.line) + ":" +
                         std::to_string(error.location.column) + ":" +
                         error.code;
        return seenLocations_.find(key) != seenLocations_.end();
    }
    
    /**
     * 📊 تحديث العدادات
     */
    void updateCounts(ErrorSeverity severity) {
        switch (severity) {
            case ErrorSeverity::ERROR:
                errorCount_++;
                break;
            case ErrorSeverity::WARNING:
                warningCount_++;
                break;
            case ErrorSeverity::FATAL:
                fatalCount_++;
                errorCount_++;
                break;
            default:
                break;
        }
        
        // تخزين الموقع
        const auto& loc = errors_.back().location;
        std::string key = loc.filename + ":" +
                         std::to_string(loc.line) + ":" +
                         std::to_string(loc.column) + ":" +
                         errors_.back().code;
        seenLocations_.insert(key);
    }
    
    /**
     * ⏰ الحصول على الوقت الحالي
     */
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        char buffer[32];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
        return buffer;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                 مولد رموز الأخطاء | Error Code Generator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔢 ErrorCodeGenerator - مولد رموز الأخطاء
 */
class ErrorCodeGenerator {
public:
    /**
     * 📋 توليد رمز للخطأ
     */
    static std::string generate(ErrorCategory category, int number) {
        std::ostringstream oss;
        
        // البادئة حسب الفئة
        switch (category) {
            case ErrorCategory::LEXER:   oss << "L"; break;
            case ErrorCategory::PARSER:  oss << "P"; break;
            case ErrorCategory::SEMANTIC: oss << "S"; break;
            case ErrorCategory::TYPE:    oss << "T"; break;
            case ErrorCategory::RUNTIME: oss << "R"; break;
            case ErrorCategory::IO:      oss << "I"; break;
            case ErrorCategory::INTERNAL: oss << "X"; break;
        }
        
        // الرقم بثلاث خانات
        oss << std::setfill('0') << std::setw(3) << number;
        
        return oss.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        واجهة خارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════════

// الجامع العالمي
static std::unique_ptr<MultiErrorCollector> globalCollector;

extern "C" {
    /**
     * 🔧 إنشاء جامع
     */
    void* sad_multi_error_create() {
        return new MultiErrorCollector();
    }
    
    /**
     * 🗑️ حذف جامع
     */
    void sad_multi_error_destroy(void* collector) {
        delete static_cast<MultiErrorCollector*>(collector);
    }
    
    /**
     * ➕ إضافة خطأ
     */
    void sad_multi_error_add(
        void* collector,
        const char* code,
        const char* message,
        const char* messageEn,
        int severity,
        int category,
        const char* filename,
        size_t line,
        size_t column
    ) {
        auto* c = static_cast<MultiErrorCollector*>(collector);
        SourceLocation loc(filename ? filename : "", line, column);
        c->add(
            code ? code : "",
            message ? message : "",
            messageEn ? messageEn : "",
            static_cast<ErrorSeverity>(severity),
            static_cast<ErrorCategory>(category),
            loc
        );
    }
    
    /**
     * 📊 عدد الأخطاء
     */
    size_t sad_multi_error_count(void* collector) {
        return static_cast<MultiErrorCollector*>(collector)->errorCount();
    }
    
    /**
     * 📝 تنسيق الأخطاء
     */
    const char* sad_multi_error_format(void* collector) {
        static std::string result;
        result = static_cast<MultiErrorCollector*>(collector)->formatAll(true);
        return result.c_str();
    }
    
    /**
     * 🧹 مسح الأخطاء
     */
    void sad_multi_error_clear(void* collector) {
        static_cast<MultiErrorCollector*>(collector)->clear();
    }
}

} // namespace errors
} // namespace sad
