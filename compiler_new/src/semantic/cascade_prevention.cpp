/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: cascade_prevention.cpp                                               ║
 * ║   الوصف: منع تسلسل الأخطاء                                                  ║
 * ║   المهمة: T165 - Phase 15 (US13)                                            ║
 * ║   المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                         دليل المبتدئين | Beginner's Guide
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 ما هو تسلسل الأخطاء؟ | What is Error Cascading?
 * ─────────────────────────────────────────────────────
 *
 * أحياناً خطأ واحد يسبب عشرات الأخطاء الأخرى!
 *
 * مثال:
 * ─────
 *    السطر 1: متغير س = 5  // نسيت علامة الاقتباس
 *    السطر 2: متغير ص = س + 10
 *    السطر 3: اطبع(ص)
 *
 * إذا أخطأت في السطر 1، المترجم قد يعطيك:
 *    ❌ خطأ 1: رمز غير متوقع في السطر 1
 *    ❌ خطأ 2: س غير معرّف في السطر 2  <- تسلسل!
 *    ❌ خطأ 3: ص غير معرّف في السطر 3  <- تسلسل!
 *
 * 🤔 المشكلة:
 *    الأخطاء 2 و 3 ليست حقيقية!
 *    هي نتيجة للخطأ الأول فقط
 *
 * 💡 الحل: منع التسلسل
 * ─────────────────────────────────────────────────────
 *
 *    1️⃣ نتتبع الأخطاء السابقة
 *    2️⃣ نفحص إذا الخطأ الجديد ناتج عنها
 *    3️⃣ إذا كان تسلسلاً، نتجاهله أو نخفيه
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <algorithm>
#include <sstream>

namespace sad {
namespace semantic {

// ═══════════════════════════════════════════════════════════════════════════════
//                         أنواع الأخطاء | Error Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📋 PrimaryErrorType - أنواع الأخطاء الأساسية
 */
enum class PrimaryErrorType {
    UNDEFINED_VARIABLE,     // متغير غير معرّف
    UNDEFINED_FUNCTION,     // دالة غير معرّفة
    UNDEFINED_TYPE,         // نوع غير معرّف
    UNDEFINED_CLASS,        // صنف غير معرّف
    UNDEFINED_PROPERTY,     // خاصية غير معرّفة
    UNDEFINED_METHOD,       // دالة غير معرّفة
    TYPE_MISMATCH,          // عدم تطابق الأنواع
    SYNTAX_ERROR,           // خطأ نحوي
    IMPORT_ERROR,           // خطأ استيراد
    ARGUMENT_ERROR          // خطأ معاملات
};

/**
 * 📋 CascadeReason - سبب التسلسل
 */
enum class CascadeReason {
    USES_UNDEFINED_SYMBOL,  // يستخدم رمزاً غير معرّف
    DEPENDS_ON_ERROR_TYPE,  // يعتمد على نوع خاطئ
    IN_ERROR_CONTEXT,       // داخل سياق خاطئ
    FOLLOWS_SYNTAX_ERROR,   // يتبع خطأ نحوي
    SAME_EXPRESSION         // في نفس التعبير
};

// ═══════════════════════════════════════════════════════════════════════════════
//                       معلومات الخطأ | Error Information
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📍 ErrorLocation - موقع الخطأ
 */
struct ErrorLocation {
    std::string filename;
    size_t line;
    size_t column;
    size_t scopeDepth;      // عمق النطاق
    std::string scopeName;  // اسم النطاق (دالة، صنف، إلخ)
    
    ErrorLocation()
        : line(0), column(0), scopeDepth(0) {}
    
    bool isAfter(const ErrorLocation& other) const {
        if (filename != other.filename) return false;
        if (line > other.line) return true;
        if (line == other.line && column > other.column) return true;
        return false;
    }
    
    bool inSameScope(const ErrorLocation& other) const {
        return filename == other.filename && 
               scopeName == other.scopeName &&
               scopeDepth == other.scopeDepth;
    }
};

/**
 * 📋 TrackedError - خطأ متتبع
 */
struct TrackedError {
    size_t id;
    PrimaryErrorType type;
    ErrorLocation location;
    std::string symbol;         // الرمز المتأثر (إن وجد)
    std::string message;
    bool isCascade;            // هل هذا خطأ تسلسلي؟
    size_t causedById;         // معرف الخطأ المسبب
    CascadeReason cascadeReason;
    
    TrackedError()
        : id(0)
        , type(PrimaryErrorType::UNDEFINED_VARIABLE)
        , isCascade(false)
        , causedById(0)
        , cascadeReason(CascadeReason::USES_UNDEFINED_SYMBOL)
    {}
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    نظام منع التسلسل | Cascade Prevention System
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🛡️ CascadePreventionSystem - نظام منع التسلسل
 *
 * يتتبع الأخطاء ويمنع الأخطاء التسلسلية
 */
class CascadePreventionSystem {
public:
    CascadePreventionSystem()
        : nextErrorId_(1)
        , cascadeThreshold_(3)  // الحد الأقصى للتسلسلات المسموحة
    {}
    
    /**
     * 🔍 فحص إذا كان الخطأ تسلسلياً
     */
    bool isCascadeError(
        PrimaryErrorType type,
        const std::string& symbol,
        const ErrorLocation& location
    ) {
        // فحص 1: هل الرمز غير معرّف بسبب خطأ سابق؟
        if (type == PrimaryErrorType::UNDEFINED_VARIABLE ||
            type == PrimaryErrorType::UNDEFINED_FUNCTION ||
            type == PrimaryErrorType::UNDEFINED_TYPE) {
            
            if (isSymbolAffectedByPreviousError(symbol)) {
                return true;
            }
        }
        
        // فحص 2: هل الخطأ في نفس نطاق خطأ نحوي؟
        if (isInErrorContext(location)) {
            return true;
        }
        
        // فحص 3: هل يتبع خطأ نحوي في نفس السطر؟
        if (followsSyntaxError(location)) {
            return true;
        }
        
        // فحص 4: هل وصلنا للحد الأقصى من نفس النوع؟
        if (hasReachedCascadeThreshold(type, symbol)) {
            return true;
        }
        
        return false;
    }
    
    /**
     * ➕ تسجيل خطأ جديد
     */
    TrackedError registerError(
        PrimaryErrorType type,
        const std::string& symbol,
        const ErrorLocation& location,
        const std::string& message
    ) {
        TrackedError error;
        error.id = nextErrorId_++;
        error.type = type;
        error.symbol = symbol;
        error.location = location;
        error.message = message;
        
        // فحص التسلسل
        CascadeReason reason;
        size_t causeId;
        if (detectCascade(type, symbol, location, reason, causeId)) {
            error.isCascade = true;
            error.cascadeReason = reason;
            error.causedById = causeId;
        } else {
            error.isCascade = false;
        }
        
        // تخزين الخطأ
        errors_.push_back(error);
        
        // تحديث التتبع
        updateTracking(error);
        
        return error;
    }
    
    /**
     * 📋 الحصول على الأخطاء الأساسية فقط
     */
    std::vector<TrackedError> getPrimaryErrors() const {
        std::vector<TrackedError> primary;
        for (const auto& error : errors_) {
            if (!error.isCascade) {
                primary.push_back(error);
            }
        }
        return primary;
    }
    
    /**
     * 📋 الحصول على كل الأخطاء مع علامة التسلسل
     */
    const std::vector<TrackedError>& getAllErrors() const {
        return errors_;
    }
    
    /**
     * 📊 إحصائيات
     */
    struct Stats {
        size_t totalErrors;
        size_t primaryErrors;
        size_t cascadeErrors;
        size_t preventedCascades;
    };
    
    Stats getStats() const {
        Stats stats;
        stats.totalErrors = errors_.size();
        stats.primaryErrors = 0;
        stats.cascadeErrors = 0;
        
        for (const auto& error : errors_) {
            if (error.isCascade) {
                stats.cascadeErrors++;
            } else {
                stats.primaryErrors++;
            }
        }
        
        stats.preventedCascades = preventedCount_;
        return stats;
    }
    
    /**
     * 🧹 مسح
     */
    void clear() {
        errors_.clear();
        affectedSymbols_.clear();
        errorContexts_.clear();
        syntaxErrorLocations_.clear();
        typeErrorCounts_.clear();
        nextErrorId_ = 1;
        preventedCount_ = 0;
    }
    
    /**
     * ⚙️ إعدادات
     */
    void setCascadeThreshold(size_t threshold) {
        cascadeThreshold_ = threshold;
    }
    
    /**
     * 📝 تنسيق تقرير
     */
    std::string formatReport(bool showCascades = false) const {
        std::ostringstream oss;
        
        auto stats = getStats();
        
        oss << "\n";
        oss << "╔══════════════════════════════════════════════════════════════╗\n";
        oss << "║              تقرير منع التسلسل | Cascade Report              ║\n";
        oss << "╠══════════════════════════════════════════════════════════════╣\n";
        oss << "║   إجمالي الأخطاء: " << stats.totalErrors << "\n";
        oss << "║   أخطاء أساسية: " << stats.primaryErrors << "\n";
        oss << "║   أخطاء تسلسلية: " << stats.cascadeErrors << "\n";
        oss << "║   تسلسلات تم منعها: " << stats.preventedCascades << "\n";
        oss << "╚══════════════════════════════════════════════════════════════╝\n";
        
        // عرض الأخطاء الأساسية
        oss << "\n📋 الأخطاء الأساسية:\n";
        oss << "   ─────────────────────────────────────────────────────────\n";
        
        for (const auto& error : errors_) {
            if (!error.isCascade) {
                oss << "\n   [" << error.id << "] " << error.message << "\n";
                oss << "       " << error.location.filename << ":"
                    << error.location.line << ":" << error.location.column << "\n";
                if (!error.symbol.empty()) {
                    oss << "       الرمز: " << error.symbol << "\n";
                }
            }
        }
        
        // عرض الأخطاء التسلسلية (إن طُلب)
        if (showCascades && stats.cascadeErrors > 0) {
            oss << "\n🔗 الأخطاء التسلسلية (مخفية عادةً):\n";
            oss << "   ─────────────────────────────────────────────────────────\n";
            
            for (const auto& error : errors_) {
                if (error.isCascade) {
                    oss << "\n   [" << error.id << "] " << error.message << "\n";
                    oss << "       ناتج عن الخطأ #" << error.causedById << "\n";
                    oss << "       السبب: " << cascadeReasonToString(error.cascadeReason) << "\n";
                }
            }
        }
        
        return oss.str();
    }
    
private:
    std::vector<TrackedError> errors_;
    std::unordered_set<std::string> affectedSymbols_;
    std::vector<ErrorLocation> errorContexts_;
    std::vector<ErrorLocation> syntaxErrorLocations_;
    std::unordered_map<std::string, size_t> typeErrorCounts_;
    size_t nextErrorId_;
    size_t cascadeThreshold_;
    size_t preventedCount_ = 0;
    
    /**
     * 🔍 هل الرمز متأثر بخطأ سابق؟
     */
    bool isSymbolAffectedByPreviousError(const std::string& symbol) const {
        return affectedSymbols_.find(symbol) != affectedSymbols_.end();
    }
    
    /**
     * 🔍 هل الموقع في سياق خاطئ؟
     */
    bool isInErrorContext(const ErrorLocation& location) const {
        for (const auto& ctx : errorContexts_) {
            if (location.inSameScope(ctx)) {
                return true;
            }
        }
        return false;
    }
    
    /**
     * 🔍 هل يتبع خطأ نحوي؟
     */
    bool followsSyntaxError(const ErrorLocation& location) const {
        for (const auto& syntaxLoc : syntaxErrorLocations_) {
            if (location.filename == syntaxLoc.filename &&
                location.line == syntaxLoc.line &&
                location.column > syntaxLoc.column) {
                return true;
            }
        }
        return false;
    }
    
    /**
     * 🔍 هل وصلنا للحد الأقصى؟
     */
    bool hasReachedCascadeThreshold(PrimaryErrorType type, const std::string& symbol) const {
        std::string key = std::to_string(static_cast<int>(type)) + ":" + symbol;
        auto it = typeErrorCounts_.find(key);
        if (it != typeErrorCounts_.end()) {
            return it->second >= cascadeThreshold_;
        }
        return false;
    }
    
    /**
     * 🔍 اكتشاف التسلسل
     */
    bool detectCascade(
        PrimaryErrorType type,
        const std::string& symbol,
        const ErrorLocation& location,
        CascadeReason& reason,
        size_t& causeId
    ) {
        // فحص 1: استخدام رمز غير معرّف
        if (isSymbolAffectedByPreviousError(symbol)) {
            reason = CascadeReason::USES_UNDEFINED_SYMBOL;
            causeId = findCausingError(symbol);
            preventedCount_++;
            return true;
        }
        
        // فحص 2: في سياق خاطئ
        if (isInErrorContext(location)) {
            reason = CascadeReason::IN_ERROR_CONTEXT;
            causeId = findContextError(location);
            preventedCount_++;
            return true;
        }
        
        // فحص 3: بعد خطأ نحوي
        if (followsSyntaxError(location)) {
            reason = CascadeReason::FOLLOWS_SYNTAX_ERROR;
            causeId = findSyntaxError(location);
            preventedCount_++;
            return true;
        }
        
        // فحص 4: تجاوز الحد
        if (hasReachedCascadeThreshold(type, symbol)) {
            reason = CascadeReason::SAME_EXPRESSION;
            causeId = 0;
            preventedCount_++;
            return true;
        }
        
        return false;
    }
    
    /**
     * 📝 تحديث التتبع
     */
    void updateTracking(const TrackedError& error) {
        // إضافة الرمز للمتأثرين
        if (!error.symbol.empty() && !error.isCascade) {
            affectedSymbols_.insert(error.symbol);
        }
        
        // إذا كان خطأ نحوي، أضف للقائمة
        if (error.type == PrimaryErrorType::SYNTAX_ERROR && !error.isCascade) {
            syntaxErrorLocations_.push_back(error.location);
            errorContexts_.push_back(error.location);
        }
        
        // تحديث العداد
        std::string key = std::to_string(static_cast<int>(error.type)) + ":" + error.symbol;
        typeErrorCounts_[key]++;
    }
    
    /**
     * 🔍 البحث عن الخطأ المسبب
     */
    size_t findCausingError(const std::string& symbol) const {
        for (const auto& error : errors_) {
            if (!error.isCascade && error.symbol == symbol) {
                return error.id;
            }
        }
        return 0;
    }
    
    size_t findContextError(const ErrorLocation& location) const {
        for (const auto& error : errors_) {
            if (!error.isCascade && location.inSameScope(error.location)) {
                return error.id;
            }
        }
        return 0;
    }
    
    size_t findSyntaxError(const ErrorLocation& location) const {
        for (const auto& error : errors_) {
            if (!error.isCascade && 
                error.type == PrimaryErrorType::SYNTAX_ERROR &&
                error.location.filename == location.filename &&
                error.location.line == location.line) {
                return error.id;
            }
        }
        return 0;
    }
    
    /**
     * 📝 تحويل سبب التسلسل لنص
     */
    std::string cascadeReasonToString(CascadeReason reason) const {
        switch (reason) {
            case CascadeReason::USES_UNDEFINED_SYMBOL:
                return "يستخدم رمزاً غير معرّف من خطأ سابق";
            case CascadeReason::DEPENDS_ON_ERROR_TYPE:
                return "يعتمد على نوع خاطئ";
            case CascadeReason::IN_ERROR_CONTEXT:
                return "داخل سياق فيه خطأ";
            case CascadeReason::FOLLOWS_SYNTAX_ERROR:
                return "يتبع خطأ نحوي في نفس السطر";
            case CascadeReason::SAME_EXPRESSION:
                return "خطأ متكرر في نفس التعبير";
        }
        return "غير معروف";
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        واجهة خارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════════

static std::unique_ptr<CascadePreventionSystem> globalCascade;

extern "C" {
    /**
     * 🔧 إنشاء النظام
     */
    void* sad_cascade_create() {
        return new CascadePreventionSystem();
    }
    
    /**
     * 🗑️ حذف النظام
     */
    void sad_cascade_destroy(void* system) {
        delete static_cast<CascadePreventionSystem*>(system);
    }
    
    /**
     * 🔍 فحص التسلسل
     */
    bool sad_cascade_is_cascade(
        void* system,
        int type,
        const char* symbol,
        const char* filename,
        size_t line,
        size_t column
    ) {
        auto* s = static_cast<CascadePreventionSystem*>(system);
        ErrorLocation loc;
        loc.filename = filename ? filename : "";
        loc.line = line;
        loc.column = column;
        
        return s->isCascadeError(
            static_cast<PrimaryErrorType>(type),
            symbol ? symbol : "",
            loc
        );
    }
    
    /**
     * 📝 تقرير
     */
    const char* sad_cascade_report(void* system) {
        static std::string result;
        result = static_cast<CascadePreventionSystem*>(system)->formatReport(false);
        return result.c_str();
    }
    
    /**
     * 🧹 مسح
     */
    void sad_cascade_clear(void* system) {
        static_cast<CascadePreventionSystem*>(system)->clear();
    }
}

} // namespace semantic
} // namespace sad
