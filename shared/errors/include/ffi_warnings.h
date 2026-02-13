/**
 * @file ffi_warnings.h
 * @brief (AR) نظام التحذيرات الذكية لـ FFI - كشف تسريبات الذاكرة
 *        (EN) Smart FFI Warning System - Memory Leak Detection
 * 
 * @details
 * (AR) هذا الملف يحتوي على نظام التحذيرات الذكية لـ FFI.
 *      يكتشف:
 *      ┌────────────────────────────────────────────────────────────┐
 *      │ 1. تسريب الذاكرة (نتيجة مملوكة غير مُحررة)                 │
 *      │ 2. استخدام بعد التحرير                                     │
 *      │ 3. تحرير مزدوج                                             │
 *      │ 4. عدم تطابق دالة الحجز والتحرير                           │
 *      │ 5. تمرير null لمعامل غير اختياري                          │
 *      │ 6. نقل ملكية متغير مستعار                                  │
 *      └────────────────────────────────────────────────────────────┘
 * 
 *      الرسائل بصيغة:
 *      @code
 *      تحذير[ص-FFI-001]: تسريب محتمل للذاكرة
 *          --> ملف.ص:15:8
 *          |
 *       15 |     ثابت ptr = malloc(100)
 *          |            ^^^ النتيجة مملوكة لكن لم تُحرر
 *          |
 *      = ملاحظة: استخدم free(ptr) أو غلاف_آمن
 *      @endcode
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2025-01-18
 * @version 1.0.0
 */

#ifndef SAD_SHARED_ERRORS_FFI_WARNINGS_H
#define SAD_SHARED_ERRORS_FFI_WARNINGS_H

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <memory>
#include <functional>

namespace sad {
namespace errors {

// ============================================================================
//                    (AR) رموز التحذيرات
//                    (EN) Warning Codes
// ============================================================================

/**
 * @enum FFIWarningCode
 * @brief (AR) رموز تحذيرات FFI
 *        (EN) FFI Warning Codes
 */
enum class FFIWarningCode {
    // (AR) تحذيرات تسريب الذاكرة (001-010)
    // (EN) Memory leak warnings (001-010)
    POTENTIAL_LEAK          = 1,    ///< ص-FFI-001: تسريب محتمل
    UNFREED_ALLOCATION      = 2,    ///< ص-FFI-002: تخصيص غير مُحرر
    OWNED_RESULT_IGNORED    = 3,    ///< ص-FFI-003: نتيجة مملوكة مُتجاهلة
    MISSING_CLEANUP         = 4,    ///< ص-FFI-004: تنظيف مفقود
    CONDITIONAL_LEAK        = 5,    ///< ص-FFI-005: تسريب شرطي
    
    // (AR) تحذيرات استخدام غير صحيح (011-020)
    // (EN) Incorrect usage warnings (011-020)
    USE_AFTER_FREE          = 11,   ///< ص-FFI-011: استخدام بعد التحرير
    DOUBLE_FREE             = 12,   ///< ص-FFI-012: تحرير مزدوج
    INVALID_FREE            = 13,   ///< ص-FFI-013: تحرير غير صالح
    MISMATCHED_DEALLOC      = 14,   ///< ص-FFI-014: عدم تطابق التحرير
    
    // (AR) تحذيرات المؤشرات (021-030)
    // (EN) Pointer warnings (021-030)
    NULL_PASSED_TO_NONNULL  = 21,   ///< ص-FFI-021: null لمعامل غير اختياري
    UNCHECKED_NULL_RESULT   = 22,   ///< ص-FFI-022: نتيجة null غير مفحوصة
    DANGLING_POINTER        = 23,   ///< ص-FFI-023: مؤشر معلق
    
    // (AR) تحذيرات الملكية (031-040)
    // (EN) Ownership warnings (031-040)
    BORROWED_TRANSFERRED    = 31,   ///< ص-FFI-031: نقل مستعار
    OWNERSHIP_UNCLEAR       = 32,   ///< ص-FFI-032: ملكية غير واضحة
    ESCAPING_LOCAL          = 33,   ///< ص-FFI-033: هروب متغير محلي
    
    // (AR) تحذيرات متنوعة (041-050)
    // (EN) Miscellaneous warnings (041-050)
    UNSAFE_FFI_CALL         = 41,   ///< ص-FFI-041: استدعاء FFI غير آمن
    DEPRECATED_FFI          = 42,   ///< ص-FFI-042: FFI مُهمل
    THREAD_SAFETY           = 43,   ///< ص-FFI-043: مشكلة أمان خيوط
};

/**
 * @enum WarningSeverity
 * @brief (AR) شدة التحذير
 *        (EN) Warning severity
 */
enum class WarningSeverity {
    NOTE,       ///< (AR) ملاحظة / (EN) Note
    HINT,       ///< (AR) تلميح / (EN) Hint
    WARNING,    ///< (AR) تحذير / (EN) Warning
    ERROR       ///< (AR) خطأ / (EN) Error (promoted from warning)
};

// ============================================================================
//                    (AR) بنيات البيانات
//                    (EN) Data Structures
// ============================================================================

/**
 * @struct SourceLocation
 * @brief (AR) موقع في الكود المصدري
 *        (EN) Location in source code
 */
struct SourceLocation {
    std::string filename;
    size_t line = 0;
    size_t column = 0;
    std::string lineContent;        ///< (AR) محتوى السطر / (EN) Line content
    size_t highlightStart = 0;      ///< (AR) بداية التمييز / (EN) Highlight start
    size_t highlightLength = 0;     ///< (AR) طول التمييز / (EN) Highlight length
};

/**
 * @struct FFIWarning
 * @brief (AR) تحذير FFI كامل
 *        (EN) Complete FFI warning
 */
struct FFIWarning {
    FFIWarningCode code;
    WarningSeverity severity = WarningSeverity::WARNING;
    
    std::string arabicMessage;          ///< (AR) الرسالة بالعربية
    std::string englishMessage;         ///< (AR) الرسالة بالإنجليزية
    
    SourceLocation location;
    std::vector<SourceLocation> relatedLocations;  ///< (AR) مواقع ذات صلة
    
    std::vector<std::string> notes;     ///< (AR) ملاحظات إضافية
    std::vector<std::string> suggestions; ///< (AR) اقتراحات للإصلاح
    
    std::optional<std::string> fixHint;  ///< (AR) إصلاح مقترح / (EN) Suggested fix
};

/**
 * @struct FFIWarningConfig
 * @brief (AR) إعدادات التحذيرات
 *        (EN) Warning configuration
 */
struct FFIWarningConfig {
    bool enableAll = true;
    std::map<FFIWarningCode, bool> enabledWarnings;
    std::map<FFIWarningCode, WarningSeverity> severityOverrides;
    
    bool arabicMessages = true;         ///< (AR) رسائل عربية
    bool showNotes = true;              ///< (AR) إظهار الملاحظات
    bool showSuggestions = true;        ///< (AR) إظهار الاقتراحات
    bool showRelatedLocations = true;   ///< (AR) إظهار المواقع ذات الصلة
    bool colorOutput = true;            ///< (AR) إخراج ملون
};

// ============================================================================
//                    (AR) فئة مُنشئ التحذيرات
//                    (EN) Warning Builder Class
// ============================================================================

/**
 * @class FFIWarningBuilder
 * @brief (AR) بناء تحذيرات FFI بطريقة سلسة
 *        (EN) Build FFI warnings fluently
 */
class FFIWarningBuilder {
public:
    FFIWarningBuilder(FFIWarningCode code);
    
    FFIWarningBuilder& withArabicMessage(const std::string& msg);
    FFIWarningBuilder& withEnglishMessage(const std::string& msg);
    FFIWarningBuilder& atLocation(const SourceLocation& loc);
    FFIWarningBuilder& atLocation(const std::string& file, size_t line, size_t col);
    FFIWarningBuilder& withLineContent(const std::string& content, size_t start, size_t len);
    FFIWarningBuilder& withRelatedLocation(const SourceLocation& loc);
    FFIWarningBuilder& withNote(const std::string& note);
    FFIWarningBuilder& withSuggestion(const std::string& suggestion);
    FFIWarningBuilder& withFix(const std::string& fix);
    FFIWarningBuilder& withSeverity(WarningSeverity severity);
    
    FFIWarning build() const;
    
private:
    FFIWarning warning_;
};

// ============================================================================
//                    (AR) فئة منسق التحذيرات
//                    (EN) Warning Formatter Class
// ============================================================================

/**
 * @class FFIWarningFormatter
 * @brief (AR) تنسيق التحذيرات للعرض
 *        (EN) Format warnings for display
 */
class FFIWarningFormatter {
public:
    explicit FFIWarningFormatter(const FFIWarningConfig& config);
    
    /**
     * @brief (AR) تنسيق تحذير واحد
     *        (EN) Format single warning
     */
    std::string format(const FFIWarning& warning) const;
    
    /**
     * @brief (AR) تنسيق قائمة تحذيرات
     *        (EN) Format warning list
     */
    std::string formatAll(const std::vector<FFIWarning>& warnings) const;
    
    /**
     * @brief (AR) الحصول على رمز التحذير كنص
     *        (EN) Get warning code as string
     */
    static std::string codeToString(FFIWarningCode code);
    
    /**
     * @brief (AR) الحصول على وصف الرمز
     *        (EN) Get code description
     */
    static std::string codeDescription(FFIWarningCode code, bool arabic);
    
private:
    FFIWarningConfig config_;
    
    std::string formatLocation(const SourceLocation& loc) const;
    std::string formatCodePointer(const SourceLocation& loc) const;
    std::string severityToString(WarningSeverity severity) const;
    std::string colorize(const std::string& text, WarningSeverity severity) const;
};

// ============================================================================
//                    (AR) فئة كاشف التسريبات
//                    (EN) Leak Detector Class
// ============================================================================

/**
 * @class FFILeakDetector
 * @brief (AR) كشف تسريبات الذاكرة في كود FFI
 *        (EN) Detect memory leaks in FFI code
 * 
 * @details
 * (AR) يحلل هذا الفئة تدفق البيانات لاكتشاف:
 *      - تخصيصات غير مُحررة
 *      - نتائج مملوكة مُتجاهلة
 *      - مسارات تنفيذ بدون تنظيف
 */
class FFILeakDetector {
public:
    /**
     * @struct AllocationInfo
     * @brief (AR) معلومات التخصيص
     *        (EN) Allocation information
     */
    struct AllocationInfo {
        std::string varName;
        std::string allocatorFunc;
        SourceLocation location;
        bool isFreed = false;
        std::optional<SourceLocation> freeLocation;
        std::optional<std::string> expectedDeallocator;
    };
    
    /**
     * @brief (AR) تسجيل تخصيص
     *        (EN) Register allocation
     */
    void registerAllocation(
        const std::string& varName,
        const std::string& allocator,
        const SourceLocation& location
    );
    
    /**
     * @brief (AR) تسجيل تحرير
     *        (EN) Register deallocation
     */
    void registerDeallocation(
        const std::string& varName,
        const std::string& deallocator,
        const SourceLocation& location
    );
    
    /**
     * @brief (AR) التحقق من نطاق (نهاية دالة)
     *        (EN) Check scope (end of function)
     */
    std::vector<FFIWarning> checkScope();
    
    /**
     * @brief (AR) مسح السجلات
     *        (EN) Clear records
     */
    void clear();
    
    /**
     * @brief (AR) الحصول على التخصيصات غير المُحررة
     *        (EN) Get unfreed allocations
     */
    std::vector<AllocationInfo> getUnfreedAllocations() const;
    
private:
    std::map<std::string, AllocationInfo> allocations_;
    
    FFIWarning createLeakWarning(const AllocationInfo& info);
    FFIWarning createMismatchWarning(const AllocationInfo& info, const std::string& actualDeallocator);
};

// ============================================================================
//                    (AR) فئة مُجمع التحذيرات
//                    (EN) Warning Collector Class
// ============================================================================

/**
 * @class FFIWarningCollector
 * @brief (AR) تجميع وإدارة التحذيرات
 *        (EN) Collect and manage warnings
 */
class FFIWarningCollector {
public:
    explicit FFIWarningCollector(const FFIWarningConfig& config = {});
    
    /**
     * @brief (AR) إضافة تحذير
     *        (EN) Add warning
     */
    void add(const FFIWarning& warning);
    
    /**
     * @brief (AR) إضافة تحذير باستخدام البناء
     *        (EN) Add warning using builder
     */
    void add(FFIWarningBuilder&& builder);
    
    /**
     * @brief (AR) الحصول على جميع التحذيرات
     *        (EN) Get all warnings
     */
    const std::vector<FFIWarning>& getWarnings() const { return warnings_; }
    
    /**
     * @brief (AR) الحصول على تحذيرات بشدة معينة
     *        (EN) Get warnings by severity
     */
    std::vector<FFIWarning> getBySeverity(WarningSeverity severity) const;
    
    /**
     * @brief (AR) الحصول على تحذيرات بنوع معين
     *        (EN) Get warnings by code
     */
    std::vector<FFIWarning> getByCode(FFIWarningCode code) const;
    
    /**
     * @brief (AR) التحقق من وجود أخطاء
     *        (EN) Check if has errors
     */
    bool hasErrors() const;
    
    /**
     * @brief (AR) عدد التحذيرات
     *        (EN) Warning count
     */
    size_t count() const { return warnings_.size(); }
    
    /**
     * @brief (AR) مسح التحذيرات
     *        (EN) Clear warnings
     */
    void clear() { warnings_.clear(); }
    
    /**
     * @brief (AR) طباعة جميع التحذيرات
     *        (EN) Print all warnings
     */
    void printAll(std::ostream& out) const;
    
private:
    FFIWarningConfig config_;
    std::vector<FFIWarning> warnings_;
    FFIWarningFormatter formatter_;
    
    bool isEnabled(FFIWarningCode code) const;
    WarningSeverity getSeverity(FFIWarningCode code) const;
};

// ============================================================================
//                    (AR) دوال مساعدة
//                    (EN) Helper Functions
// ============================================================================

/**
 * @brief (AR) إنشاء تحذير تسريب
 *        (EN) Create leak warning
 */
FFIWarning createLeakWarning(
    const std::string& varName,
    const std::string& allocator,
    const SourceLocation& location
);

/**
 * @brief (AR) إنشاء تحذير استخدام بعد التحرير
 *        (EN) Create use-after-free warning
 */
FFIWarning createUseAfterFreeWarning(
    const std::string& varName,
    const SourceLocation& useLocation,
    const SourceLocation& freeLocation
);

/**
 * @brief (AR) إنشاء تحذير تحرير مزدوج
 *        (EN) Create double-free warning
 */
FFIWarning createDoubleFreeWarning(
    const std::string& varName,
    const SourceLocation& firstFree,
    const SourceLocation& secondFree
);

/**
 * @brief (AR) إنشاء تحذير عدم تطابق
 *        (EN) Create mismatch warning
 */
FFIWarning createMismatchWarning(
    const std::string& varName,
    const std::string& allocator,
    const std::string& deallocator,
    const std::string& expectedDeallocator,
    const SourceLocation& location
);

} // namespace errors
} // namespace sad

#endif // SAD_SHARED_ERRORS_FFI_WARNINGS_H
