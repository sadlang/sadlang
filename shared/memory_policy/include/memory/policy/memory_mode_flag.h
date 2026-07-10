// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file memory_mode_flag.h
 * @brief معالج أعلام وضع الذاكرة من سطر الأوامر
 * 
 * يوفر تحليل أعلام سطر الأوامر لتحديد وضع الذاكرة:
 *   --تطوير / --dev     : وضع التطوير مع GC
 *   --إنتاج / --prod    : وضع الإنتاج مع ملكية صارمة
 *   --تعلم / --learn    : وضع التعلم مع شروحات إضافية
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#ifndef SAD_MEMORY_MODE_FLAG_H
#define SAD_MEMORY_MODE_FLAG_H

#include "memory/policy/gc_mode.h"
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <functional>

namespace Sad {
namespace Memory {

// =============================================================================
//                    أنواع البيانات
// =============================================================================

/**
 * @brief نتيجة تحليل الأعلام
 */
struct FlagParseResult {
    bool success = false;                   // هل نجح التحليل؟
    MemoryModeSettings settings;            // الإعدادات المستخرجة
    std::vector<std::string> errors;        // أخطاء التحليل
    std::vector<std::string> warnings;      // تحذيرات
    std::vector<std::string> remainingArgs; // الأعلام غير المعالجة

    // (AR) هل طلب المستخدم وضع بلا-مكتبة-قياسية (--freestanding/--no-std/--نواة…)؟
    //      المحلّل هنا هو مالك أسماء هذه الأعلام (مصدر حقيقتها) وهو يبتلعها،
    //      فيرفع هذا المؤشّر ليُفعّل المشغّل وضع الترجمة الحرّة دون تكرار الأسماء.
    // (EN) Did the user request no-std mode? This parser owns (and consumes) the
    //      flag spellings, so it raises this indicator for the driver instead of
    //      the driver re-listing flag strings.
    bool noStdRequested = false;
};

/**
 * @brief تعريف علم سطر الأوامر
 */
struct FlagDefinition {
    std::string longNameArabic;     // --تطوير
    std::string longNameEnglish;    // --development
    std::string shortName;          // -d
    std::string description;        // الوصف
    bool hasValue;                  // هل يأخذ قيمة؟
    std::string defaultValue;       // القيمة الافتراضية
};

// =============================================================================
//                    معالج الأعلام
// =============================================================================

/**
 * @brief معالج أعلام وضع الذاكرة
 * 
 * يحلل أعلام سطر الأوامر المتعلقة بإدارة الذاكرة ويُرجع
 * إعدادات MemoryModeSettings المناسبة.
 */
class MemoryModeFlag {
public:
    /**
     * @brief إنشاء معالج جديد
     */
    MemoryModeFlag();
    
    ~MemoryModeFlag();
    
    // ==========================================================================
    //                    التحليل
    // ==========================================================================
    
    /**
     * @brief تحليل أعلام سطر الأوامر
     * @param argc عدد الأعلام
     * @param argv مصفوفة الأعلام
     * @return نتيجة التحليل
     */
    FlagParseResult parse(int argc, char* argv[]);
    
    /**
     * @brief تحليل من متجه نصوص
     * @param args متجه الأعلام
     * @return نتيجة التحليل
     */
    FlagParseResult parse(const std::vector<std::string>& args);
    
    /**
     * @brief تحليل علم واحد
     * @param flag العلم
     * @return الإعدادات إذا كان العلم معروفاً
     */
    std::optional<MemoryModeSettings> parseFlag(const std::string& flag) const;
    
    // ==========================================================================
    //                    المساعدة
    // ==========================================================================
    
    /**
     * @brief إنشاء نص المساعدة
     * @param arabic باللغة العربية؟
     * @return نص المساعدة
     */
    std::string generateHelp(bool arabic = true) const;
    
    /**
     * @brief إنشاء نص المساعدة المختصر
     */
    std::string generateShortHelp(bool arabic = true) const;
    
    /**
     * @brief طباعة المساعدة
     */
    void printHelp(bool arabic = true) const;
    
    // ==========================================================================
    //                    التحقق
    // ==========================================================================
    
    /**
     * @brief هل العلم من أعلام الذاكرة؟
     * @param flag العلم للتحقق
     * @return صحيح إذا كان علم ذاكرة
     */
    bool isMemoryFlag(const std::string& flag) const;
    
    /**
     * @brief الحصول على الأعلام المدعومة
     * @return قائمة الأعلام
     */
    std::vector<FlagDefinition> getSupportedFlags() const;

private:
    std::unordered_map<std::string, std::function<void(MemoryModeSettings&, const std::string&)>> flagHandlers_;
    std::vector<FlagDefinition> flagDefinitions_;
    
    void initializeFlags();
    void handleModeFlag(MemoryModeSettings& settings, const std::string& mode) const;
    void handleGCFlag(MemoryModeSettings& settings, const std::string& strategy) const;
    void handleOwnershipFlag(MemoryModeSettings& settings, const std::string& level) const;
};

// =============================================================================
//                    دوال مساعدة
// =============================================================================

/**
 * @brief استخراج أعلام الذاكرة من قائمة الأعلام الكاملة
 * @param args جميع الأعلام
 * @return زوج (أعلام_الذاكرة، الأعلام_الباقية)
 */
std::pair<std::vector<std::string>, std::vector<std::string>> 
extractMemoryFlags(const std::vector<std::string>& args);

/**
 * @brief تطبيق الإعدادات من متغيرات البيئة
 * @param settings الإعدادات للتعديل
 * @return صحيح إذا تم تطبيق إعدادات
 */
bool applyEnvironmentSettings(MemoryModeSettings& settings);

/**
 * @brief قراءة الإعدادات من ملف تكوين
 * @param configPath مسار ملف التكوين
 * @return الإعدادات أو فارغ إذا فشل
 */
std::optional<MemoryModeSettings> readConfigFile(const std::string& configPath);

} // namespace Memory
} // namespace Sad

#endif // SAD_MEMORY_MODE_FLAG_H
