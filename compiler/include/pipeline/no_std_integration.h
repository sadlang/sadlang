/**
 * @file no_std_integration.h
 * @brief (AR) تكامل وضع بلا مكتبة قياسية مع خط الترجمة
 *        (EN) Integration of no_std mode with compilation pipeline
 * 
 * @details
 * (AR) هذا الملف يربط NoStdModeManager بخط الترجمة الرئيسي.
 *      عند اكتشاف سمة #![بلا_مكتبة_قياسية] في الكود المصدري ←
 *      يتم تفعيل وضع bare-metal تلقائياً.
 * 
 * (EN) This file connects NoStdModeManager to the main compilation pipeline.
 *      When #![no_std] attribute is detected in source code →
 *      bare-metal mode is automatically activated.
 * 
 * --- م-ب02: دمج NoStdModeManager في خط الأنابيب الرئيسي ---
 */

#ifndef SAD_PIPELINE_NO_STD_INTEGRATION_H
#define SAD_PIPELINE_NO_STD_INTEGRATION_H

#include <string>
#include <vector>
#include <memory>

// إعلانات مسبقة
namespace sad { namespace compiler { namespace pipeline {
    class NoStdModeManager;
    struct NoStdConfig;
    struct ValidationError;
}}}

namespace sad {
namespace compiler {
namespace pipeline {

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) نتيجة فحص المصدر / (EN) Source Scan Result
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct NoStdScanResult
 * @brief (AR) نتيجة فحص الكود المصدري لسمات no_std
 *        (EN) Result of scanning source code for no_std attributes
 */
struct NoStdScanResult {
    bool hasNoStd = false;          // (AR) وُجدت سمة بلا_مكتبة_قياسية
    bool hasNoMain = false;         // (AR) وُجدت سمة بلا_رئيسية
    bool hasAbortOnPanic = false;   // (AR) وُجدت سمة إيقاف_عند_ذعر
    
    // (AR) سمات الدوال المكتشفة: [{سمة, اسم_الدالة}]
    std::vector<std::pair<std::string, std::string>> functionAttributes;
};

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) واجهة التكامل / (EN) Integration Interface
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class NoStdIntegration
 * @brief (AR) طبقة التكامل بين NoStdModeManager وخط الترجمة
 *        (EN) Integration layer between NoStdModeManager and compilation pipeline
 * 
 * @details
 * (AR) الاستخدام المتوقع في خط الترجمة:
 * @code
 *   // 1. فحص المصدر (قبل أو بعد التحليل النحوي)
 *   auto scanResult = NoStdIntegration::scanSourceForNoStd(sourceCode);
 *   
 *   // 2. إنشاء المدير إذا وُجدت سمة no_std
 *   auto manager = NoStdIntegration::createManagerFromScan(scanResult, targetTriple);
 *   
 *   // 3. التحقق من الصحة
 *   auto errors = NoStdIntegration::validateNoStdRequirements(*manager);
 *   
 *   // 4. الحصول على كود البدء (لإضافته لـ LLVM Module)
 *   std::string startupIR = manager->generateStartupCode();
 * @endcode
 */
class NoStdIntegration {
public:
    // ═════════════════════════════════════════════════════════════════════════
    // (AR) الخطوة 1: فحص المصدر / (EN) Step 1: Source scanning
    // ═════════════════════════════════════════════════════════════════════════
    
    /**
     * @brief (AR) فحص الكود المصدري لاكتشاف سمات no_std
     *        (EN) Scan source code to detect no_std attributes
     * 
     * @param sourceCode (AR) الكود المصدري بلغة ص (EN) Sad source code
     * @return (AR) نتيجة الفحص (EN) Scan result
     * 
     * @details
     * (AR) يبحث عن الأنماط التالية في بداية الملف:
     *      - #![بلا_مكتبة_قياسية] أو #![no_std]
     *      - #![بلا_رئيسية] أو #![no_main]
     *      - #![إيقاف_عند_ذعر] أو #![abort_on_panic]
     *      ويبحث عن سمات الدوال:
     *      - #[نقطة_دخول] أو #[entry_point]
     *      - #[معالج_ذعر] أو #[panic_handler]
     *      - #[معالج_تخصيص] أو #[alloc_error_handler]
     */
    static NoStdScanResult scanSourceForNoStd(const std::string& sourceCode);
    
    // ═════════════════════════════════════════════════════════════════════════
    // (AR) الخطوة 2: إنشاء المدير / (EN) Step 2: Create manager
    // ═════════════════════════════════════════════════════════════════════════
    
    /**
     * @brief (AR) إنشاء NoStdModeManager من نتيجة الفحص
     *        (EN) Create NoStdModeManager from scan result
     * 
     * @param scanResult (AR) نتيجة فحص المصدر (EN) Source scan result
     * @param targetTriple (AR) الهدف الثلاثي (EN) Target triple
     * @return (AR) مدير no_std (nullptr إذا لم يكن مطلوباً)
     */
    static std::unique_ptr<NoStdModeManager> createManagerFromScan(
        const NoStdScanResult& scanResult,
        const std::string& targetTriple = "");
    
    // ═════════════════════════════════════════════════════════════════════════
    // (AR) الخطوة 3: التحقق / (EN) Step 3: Validation
    // ═════════════════════════════════════════════════════════════════════════
    
    /**
     * @brief (AR) التحقق من متطلبات no_std
     *        (EN) Validate no_std requirements
     * 
     * @param manager (AR) مدير no_std (EN) No_std manager
     * @return (AR) قائمة أخطاء التحقق (فارغة = نجاح)
     */
    static std::vector<ValidationError> validateNoStdRequirements(
        NoStdModeManager& manager);
    
    // ═════════════════════════════════════════════════════════════════════════
    // (AR) الخطوة 4: إعداد خيارات البناء / (EN) Step 4: Build options
    // ═════════════════════════════════════════════════════════════════════════
    
    /**
     * @brief (AR) تطبيق إعدادات no_std على خيارات البناء
     *        (EN) Apply no_std settings to build options
     * 
     * @param manager (AR) مدير no_std (EN) No_std manager
     * @param[out] excludedLibs (AR) المكتبات المستبعدة (EN) Excluded libraries
     * @param[out] linkerFlags (AR) إعلام الرابط الإضافية (EN) Extra linker flags
     */
    static void applyBuildOptions(
        const NoStdModeManager& manager,
        std::vector<std::string>& excludedLibs,
        std::vector<std::string>& linkerFlags);
    
    /**
     * @brief (AR) هل الهدف يحتاج no_std تلقائياً؟
     *        (EN) Does target automatically require no_std?
     */
    static bool isBaremetalTarget(const std::string& targetTriple);
};

} // namespace pipeline
} // namespace compiler
} // namespace sad

#endif // SAD_PIPELINE_NO_STD_INTEGRATION_H
