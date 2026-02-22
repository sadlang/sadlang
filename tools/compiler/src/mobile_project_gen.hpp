// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file mobile_project_gen.hpp
 * @brief مولّد مشاريع الهاتف - إنشاء هيكل مشروع متكامل للهاتف
 * 
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║               مولّد مشاريع تطبيقات الهاتف بلغة ص                           ║
 * ║           Mobile Project Generator for Sad Language                        ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                              نظرة عامة
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * هذا المولّد ينشئ مشاريع تطبيقات هاتف كاملة من كود لغة ص الواحد.
 * يدعم إنشاء مشاريع لـ:
 * 
 * - Android (Gradle + Kotlin/Java)
 * - iOS (Xcode + Swift)
 * - مشترك (Shared code between platforms)
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                         أنواع المشاريع
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * 1. مشروع أندرويد فقط:
 *    - هيكل Gradle قياسي
 *    - تكامل NDK للكود الأصلي
 *    - Jetpack Compose للواجهة
 * 
 * 2. مشروع iOS فقط:
 *    - هيكل Xcode قياسي
 *    - تكامل Swift/C للكود الأصلي
 *    - SwiftUI للواجهة
 * 
 * 3. مشروع متعدد المنصات:
 *    - كود مشترك بلغة ص
 *    - طبقة platform-specific لكل منصة
 *    - واجهة موحدة
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                         مثال الاستخدام
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * من سطر الأوامر:
 * @code{.bash}
 * # إنشاء مشروع أندرويد
 * ص هاتف جديد --اندرويد تطبيقي
 * 
 * # إنشاء مشروع iOS
 * ص هاتف جديد --آيفون تطبيقي
 * 
 * # إنشاء مشروع متعدد المنصات
 * ص هاتف جديد --الكل تطبيقي
 * @endcode
 * 
 * من كود ص:
 * @code{.sad}
 * استورد مولد_مشروع
 * 
 * // إنشاء مشروع متعدد المنصات
 * دع مشروع = مولد_مشروع.جديد()
 * مشروع.اسم = "تطبيقي"
 * مشروع.معرف = "com.example.myapp"
 * مشروع.منصات = [اندرويد، آيفون]
 * 
 * مشروع.انشئ("./المشاريع/")
 * @endcode
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#ifndef SAD_TOOLS_MOBILE_PROJECT_GEN_HPP
#define SAD_TOOLS_MOBILE_PROJECT_GEN_HPP

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <optional>
#include <functional>
#include <variant>
#include <unordered_map>

namespace Sad {
namespace Tools {
namespace Mobile {

// ═══════════════════════════════════════════════════════════════════════════
//                    تعدادات / Enumerations
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @enum MobilePlatform
 * @brief المنصات المدعومة
 */
enum class MobilePlatform {
    Android,        ///< أندرويد فقط
    iOS,            ///< iOS فقط
    Both,           ///< كلا المنصتين
    CrossPlatform   ///< مشترك مع أكواد خاصة بكل منصة
};

/**
 * @enum ProjectTemplate
 * @brief قوالب المشاريع المتاحة
 */
enum class ProjectTemplate {
    Empty,              ///< مشروع فارغ (الحد الأدنى)
    HelloWorld,         ///< تطبيق مرحباً بالعالم
    Navigation,         ///< تطبيق متعدد الشاشات
    ListDetail,         ///< قائمة مع تفاصيل
    TabBased,           ///< تطبيق بأقسام (tabs)
    Login,              ///< تطبيق مع شاشة تسجيل دخول
    MapBased,           ///< تطبيق خرائط
    CameraApp,          ///< تطبيق كاميرا
    ECommerce,          ///< تطبيق تجارة إلكترونية
    SocialMedia,        ///< تطبيق تواصل اجتماعي
    Game2D,             ///< لعبة ثنائية الأبعاد
    Custom              ///< قالب مخصص
};

/**
 * @enum UIFramework
 * @brief أُطر واجهة المستخدم
 */
enum class UIFramework {
    // أندرويد
    JetpackCompose,     ///< Jetpack Compose (الموصى به)
    AndroidViews,       ///< Views التقليدية
    
    // iOS
    SwiftUI,            ///< SwiftUI (الموصى به)
    UIKit,              ///< UIKit التقليدي
    
    // مشترك
    SadUI               ///< نظام واجهة لغة ص الموحد
};

// ═══════════════════════════════════════════════════════════════════════════
//                    بيانات المشروع / Project Data
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct ProjectMetadata
 * @brief البيانات الوصفية للمشروع
 */
struct ProjectMetadata {
    std::string name;                       ///< اسم المشروع (بالإنجليزية)
    std::string arabicName;                 ///< اسم المشروع بالعربية
    std::string packageId;                  ///< معرف الحزمة (com.example.app)
    std::string description;                ///< وصف التطبيق
    std::string arabicDescription;          ///< الوصف بالعربية
    std::string author;                     ///< المطور
    std::string version = "1.0.0";          ///< الإصدار
    std::string minSdkVersion;              ///< أقل إصدار SDK
    std::string targetSdkVersion;           ///< إصدار SDK المستهدف
    std::vector<std::string> keywords;      ///< كلمات مفتاحية
    
    // معلومات إضافية
    std::string website;                    ///< موقع التطبيق
    std::string supportEmail;               ///< بريد الدعم
    std::string privacyPolicy;              ///< رابط سياسة الخصوصية
};

/**
 * @struct ProjectFeatures
 * @brief ميزات المشروع
 */
struct ProjectFeatures {
    bool useCamera = false;                 ///< استخدام الكاميرا
    bool useLocation = false;               ///< استخدام الموقع
    bool useNotifications = false;          ///< الإشعارات
    bool useDatabase = false;               ///< قاعدة بيانات محلية
    bool useNetworking = true;              ///< الاتصال بالشبكة
    bool useAuthentication = false;         ///< المصادقة
    bool useBiometrics = false;             ///< البصمة/الوجه
    bool useInAppPurchase = false;          ///< المشتريات داخل التطبيق
    bool useAnalytics = false;              ///< تحليلات الاستخدام
    bool useCrashReporting = false;         ///< تقارير الأعطال
    bool useMultilingual = true;            ///< تعدد اللغات (عربي/إنجليزي)
    bool useRTLSupport = true;              ///< دعم الكتابة من اليمين
    bool useDarkMode = true;                ///< الوضع الداكن
    bool useAccessibility = true;           ///< دعم ذوي الاحتياجات
};

/**
 * @struct ScreenDefinition
 * @brief تعريف شاشة في التطبيق
 */
struct ScreenDefinition {
    std::string name;                       ///< اسم الشاشة (بالإنجليزية)
    std::string arabicName;                 ///< اسم الشاشة بالعربية
    std::string description;                ///< وصف الشاشة
    bool isStartScreen = false;             ///< هل هي الشاشة الأولى
    std::vector<std::string> components;    ///< مكونات الشاشة
    std::vector<std::string> navigatesTo;   ///< الشاشات التي يمكن الانتقال إليها
};

// ═══════════════════════════════════════════════════════════════════════════
//                    إعدادات المشروع / Project Configuration
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct MobileProjectConfig
 * @brief إعدادات مشروع الهاتف الكاملة
 */
struct MobileProjectConfig {
    ProjectMetadata metadata;               ///< البيانات الوصفية
    ProjectFeatures features;               ///< الميزات
    MobilePlatform platform;                ///< المنصة المستهدفة
    ProjectTemplate templateType;           ///< نوع القالب
    UIFramework uiFramework;                ///< إطار الواجهة
    
    std::vector<ScreenDefinition> screens;  ///< تعريفات الشاشات
    
    // إعدادات Android
    int androidMinSdk = 21;                 ///< أقل Android SDK
    int androidTargetSdk = 34;              ///< Android SDK المستهدف
    bool useKotlin = true;                  ///< استخدام Kotlin
    
    // إعدادات iOS
    std::string iosMinVersion = "14.0";     ///< أقل إصدار iOS
    std::string developmentTeam;            ///< فريق التطوير Apple
    
    // مسارات
    std::filesystem::path outputDirectory;  ///< مجلد الإخراج
    
    // خيارات إضافية
    bool includeTests = true;               ///< تضمين اختبارات
    bool includeCI = false;                 ///< تضمين CI/CD
    bool includeDocumentation = true;       ///< تضمين توثيق
    bool includeGitIgnore = true;           ///< تضمين .gitignore
    bool includeReadme = true;              ///< تضمين README
    
    /**
     * @brief التحقق من صحة الإعدادات
     */
    std::optional<std::string> validate() const;
};

// ═══════════════════════════════════════════════════════════════════════════
//                    نتيجة التوليد / Generation Result
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct GeneratedFile
 * @brief ملف مُولَّد
 */
struct GeneratedFile {
    std::filesystem::path relativePath;     ///< المسار النسبي
    std::string content;                    ///< المحتوى
    bool isDirectory = false;               ///< هل هو مجلد
};

/**
 * @struct ProjectGenerationResult
 * @brief نتيجة توليد المشروع
 */
struct ProjectGenerationResult {
    bool success = false;
    std::filesystem::path projectPath;      ///< مسار المشروع
    std::vector<GeneratedFile> files;       ///< الملفات المُولَّدة
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    std::string nextSteps;                  ///< الخطوات التالية
    
    std::string toArabicSummary() const;
};

// ═══════════════════════════════════════════════════════════════════════════
//                    مولّد المشروع / Project Generator
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @class MobileProjectGenerator
 * @brief مولّد مشاريع تطبيقات الهاتف
 */
class MobileProjectGenerator {
public:
    using ProgressCallback = std::function<void(const std::string& stage, int percent)>;
    
    MobileProjectGenerator();
    ~MobileProjectGenerator();
    
    // ══════════════════════════════════════════════════════════════════════
    //                    التوليد الرئيسي / Main Generation
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief توليد مشروع جديد
     * @param config إعدادات المشروع
     * @return نتيجة التوليد
     */
    ProjectGenerationResult generate(const MobileProjectConfig& config);
    
    /**
     * @brief توليد من قالب مُعرَّف مسبقاً
     */
    ProjectGenerationResult generateFromTemplate(
        ProjectTemplate templateType,
        const ProjectMetadata& metadata,
        const std::filesystem::path& outputDir);
    
    // ══════════════════════════════════════════════════════════════════════
    //                    توليد مكونات محددة / Component Generation
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief توليد هيكل مشروع Android
     */
    std::vector<GeneratedFile> generateAndroidStructure(
        const MobileProjectConfig& config);
    
    /**
     * @brief توليد هيكل مشروع iOS
     */
    std::vector<GeneratedFile> generateiOSStructure(
        const MobileProjectConfig& config);
    
    /**
     * @brief توليد الكود المشترك
     */
    std::vector<GeneratedFile> generateSharedCode(
        const MobileProjectConfig& config);
    
    /**
     * @brief توليد شاشة جديدة
     */
    std::vector<GeneratedFile> generateScreen(
        const ScreenDefinition& screen,
        MobilePlatform platform,
        UIFramework framework);
    
    // ══════════════════════════════════════════════════════════════════════
    //                    توليد الملفات / File Generation
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief توليد ملف Gradle الرئيسي
     */
    std::string generateRootBuildGradle(const MobileProjectConfig& config);
    
    /**
     * @brief توليد ملف build.gradle للتطبيق
     */
    std::string generateAppBuildGradle(const MobileProjectConfig& config);
    
    /**
     * @brief توليد AndroidManifest.xml
     */
    std::string generateAndroidManifest(const MobileProjectConfig& config);
    
    /**
     * @brief توليد Info.plist
     */
    std::string generateInfoPlist(const MobileProjectConfig& config);
    
    /**
     * @brief توليد README.md
     */
    std::string generateReadme(const MobileProjectConfig& config);
    
    /**
     * @brief توليد .gitignore
     */
    std::string generateGitIgnore(const MobileProjectConfig& config);
    
    // ══════════════════════════════════════════════════════════════════════
    //                    القوالب / Templates
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief الحصول على قائمة القوالب المتاحة
     */
    static std::vector<std::pair<ProjectTemplate, std::string>> getAvailableTemplates();
    
    /**
     * @brief الحصول على وصف قالب
     */
    static std::string getTemplateDescription(ProjectTemplate templateType);
    
    /**
     * @brief الحصول على وصف قالب بالعربية
     */
    static std::string getTemplateArabicDescription(ProjectTemplate templateType);
    
    // ══════════════════════════════════════════════════════════════════════
    //                    الإشعارات / Callbacks
    // ══════════════════════════════════════════════════════════════════════
    
    void setProgressCallback(ProgressCallback callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    void createDirectoryStructure(
        const std::filesystem::path& basePath,
        const std::vector<GeneratedFile>& files);
    
    void writeFiles(
        const std::filesystem::path& basePath,
        const std::vector<GeneratedFile>& files);
};

// ═══════════════════════════════════════════════════════════════════════════
//                    منشئ المشروع السريع / Quick Project Builder
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @class QuickProjectBuilder
 * @brief منشئ مشاريع سريع بواجهة fluent
 * 
 * @code{.cpp}
 * auto result = QuickProjectBuilder::create("تطبيقي")
 *     .withPackageId("com.example.myapp")
 *     .withPlatform(MobilePlatform::Both)
 *     .withTemplate(ProjectTemplate::Navigation)
 *     .withCamera()
 *     .withLocation()
 *     .build("./projects/");
 * @endcode
 */
class QuickProjectBuilder {
public:
    static QuickProjectBuilder create(const std::string& name);
    
    QuickProjectBuilder& withArabicName(const std::string& name);
    QuickProjectBuilder& withPackageId(const std::string& packageId);
    QuickProjectBuilder& withPlatform(MobilePlatform platform);
    QuickProjectBuilder& withTemplate(ProjectTemplate templateType);
    QuickProjectBuilder& withUIFramework(UIFramework framework);
    
    // إضافة ميزات
    QuickProjectBuilder& withCamera();
    QuickProjectBuilder& withLocation();
    QuickProjectBuilder& withNotifications();
    QuickProjectBuilder& withDatabase();
    QuickProjectBuilder& withAuthentication();
    QuickProjectBuilder& withAnalytics();
    
    // إضافة شاشات
    QuickProjectBuilder& addScreen(const ScreenDefinition& screen);
    
    // بناء المشروع
    ProjectGenerationResult build(const std::filesystem::path& outputDir);

private:
    QuickProjectBuilder(const std::string& name);
    MobileProjectConfig config_;
};

} // namespace Mobile
} // namespace Tools
} // namespace Sad

#endif // SAD_TOOLS_MOBILE_PROJECT_GEN_HPP
