// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file ios_target.hpp
 * @brief هدف iOS - إنشاء تطبيقات iOS من كود لغة ص
 * 
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║                    نظام بناء تطبيقات iOS بلغة ص                            ║
 * ║                iOS Application Build System for Sad Language               ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                              نظرة عامة
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يُمكّن مترجم لغة ص من إنشاء تطبيقات iOS أصلية.
 * يدعم أجهزة iPhone و iPad و Apple Watch و Apple TV.
 * يتكامل مع Xcode و Swift لتوفير تجربة تطوير متكاملة.
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                         الأجهزة المدعومة
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * ┌─────────────────┬────────────────┬──────────────────────────────────────┐
 * │   النوع          │    المعمارية    │              الوصف                    │
 * ├─────────────────┼────────────────┼──────────────────────────────────────┤
 * │ iPhone          │ arm64          │ جميع أجهزة iPhone من 6s وأحدث        │
 * │ iPad            │ arm64          │ جميع أجهزة iPad من Air 2 وأحدث       │
 * │ Apple Watch     │ arm64_32       │ جميع أجهزة watchOS                   │
 * │ Apple TV        │ arm64          │ جميع أجهزة tvOS                       │
 * │ Mac Catalyst    │ arm64/x86_64   │ تشغيل تطبيقات iOS على Mac            │
 * │ Simulator       │ x86_64/arm64   │ محاكيات Xcode                        │
 * └─────────────────┴────────────────┴──────────────────────────────────────┘
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                         إصدارات iOS المدعومة
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * - الحد الأدنى: iOS 14.0 (يدعم 95% من الأجهزة)
 * - الموصى به: iOS 16.0 (ميزات SwiftUI الحديثة)
 * - الأحدث: iOS 17.0 (أحدث الميزات)
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                         مثال الاستخدام بالعربية
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * @code{.sad}
 * // ملف: تطبيقي.ص
 * استورد واجهة_آيفون
 * استورد سويفت_يوآي
 * 
 * // تعريف التطبيق
 * تطبيق رئيسي {
 *     اسم: "تطبيقي"
 *     أيقونة: "AppIcon"
 *     
 *     عند_البدء() {
 *         أعرض(شاشة_رئيسية())
 *     }
 * }
 * 
 * // واجهة SwiftUI بالعربية
 * واجهة شاشة_رئيسية {
 *     عرض() -> واجهة {
 *         تنقل {
 *             قائمة {
 *                 لكل عنصر في العناصر {
 *                     صف_عنصر(عنصر)
 *                 }
 *             }
 *             .عنوان("قائمتي")
 *         }
 *     }
 * }
 * @endcode
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#ifndef SAD_TOOLS_IOS_TARGET_HPP
#define SAD_TOOLS_IOS_TARGET_HPP

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <optional>
#include <functional>
#include <variant>

namespace Sad {
namespace Tools {
namespace Mobile {

// ═══════════════════════════════════════════════════════════════════════════
//                    تعدادات iOS / iOS Enumerations
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @enum iOSPlatform
 * @brief منصات Apple المدعومة
 */
enum class iOSPlatform {
    iOS,            ///< iPhone و iPad
    iPadOS,         ///< iPad فقط (تطبيقات مُحسّنة)
    watchOS,        ///< Apple Watch
    tvOS,           ///< Apple TV
    macCatalyst,    ///< تشغيل iOS على Mac
    visionOS        ///< Apple Vision Pro
};

/**
 * @enum iOSArchitecture
 * @brief معماريات Apple المدعومة
 */
enum class iOSArchitecture {
    ARM64,          ///< أجهزة حقيقية (iPhone, iPad, Mac M1+)
    ARM64_32,       ///< Apple Watch
    X86_64,         ///< محاكي Intel Mac
    Universal       ///< FAT binary (arm64 + x86_64)
};

/**
 * @enum iOSBuildType
 * @brief نوع بناء iOS
 */
enum class iOSBuildType {
    Debug,          ///< تطوير (رموز تصحيح)
    Release,        ///< إصدار (محسّن)
    Archive,        ///< أرشيف للتوزيع
    Profile         ///< تحليل أداء
};

/**
 * @enum iOSDistributionMethod
 * @brief طريقة توزيع التطبيق
 */
enum class iOSDistributionMethod {
    Development,    ///< تطوير (أجهزة مسجلة)
    AdHoc,          ///< توزيع محدود
    Enterprise,     ///< مؤسسة
    AppStore,       ///< متجر التطبيقات
    TestFlight      ///< اختبار بيتا
};

// ═══════════════════════════════════════════════════════════════════════════
//                    إعدادات Xcode / Xcode Configuration
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct XcodeConfig
 * @brief إعدادات Xcode
 */
struct XcodeConfig {
    std::filesystem::path xcodePath;            ///< مسار Xcode.app
    std::string xcodeVersion;                   ///< إصدار Xcode (مثل: 15.2)
    std::filesystem::path developerDir;         ///< مسار Developer directory
    std::vector<std::string> installedSdks;     ///< SDKs المثبتة
    
    /**
     * @brief اكتشاف Xcode تلقائياً
     */
    static std::optional<XcodeConfig> autoDetect();
    
    /**
     * @brief الحصول على مسار SDK
     */
    std::filesystem::path getSdkPath(iOSPlatform platform) const;
    
    /**
     * @brief الحصول على مسار clang
     */
    std::filesystem::path getClangPath() const;
    
    /**
     * @brief الحصول على مسار swift
     */
    std::filesystem::path getSwiftPath() const;
};

// ═══════════════════════════════════════════════════════════════════════════
//                    معلومات Info.plist
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct InfoPlistData
 * @brief بيانات ملف Info.plist
 */
struct InfoPlistData {
    std::string bundleIdentifier;               ///< Bundle ID (com.example.app)
    std::string bundleName;                     ///< اسم التطبيق
    std::string bundleDisplayName;              ///< الاسم المعروض (يدعم العربية)
    std::string bundleVersion;                  ///< إصدار البناء (1.0.0)
    std::string bundleShortVersion;             ///< إصدار قصير (1.0)
    
    std::string minimumOSVersion = "14.0";      ///< أقل إصدار iOS
    std::vector<std::string> deviceFamilies;    ///< عائلات الأجهزة (1=iPhone, 2=iPad)
    
    bool supportsRTL = true;                    ///< دعم الكتابة من اليمين
    std::string launchStoryboard;               ///< اسم Launch Screen
    
    // أذونات وأوصافها بالعربية
    std::string cameraUsageDescription;         ///< وصف استخدام الكاميرا
    std::string locationUsageDescription;       ///< وصف استخدام الموقع
    std::string microphoneUsageDescription;     ///< وصف استخدام الميكروفون
    std::string photoLibraryUsageDescription;   ///< وصف الوصول للصور
    
    std::vector<std::pair<std::string, std::string>> customKeys; ///< مفاتيح إضافية
    
    /**
     * @brief توليد محتوى Info.plist
     */
    std::string generatePlist() const;
};

// ═══════════════════════════════════════════════════════════════════════════
//                    إعدادات المشروع / Project Configuration
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct iOSProjectConfig
 * @brief إعدادات مشروع iOS الكاملة
 */
struct iOSProjectConfig {
    std::string projectName;                    ///< اسم المشروع
    std::filesystem::path outputDir;            ///< مجلد الإخراج
    InfoPlistData infoPlist;                    ///< بيانات Info.plist
    
    iOSPlatform platform = iOSPlatform::iOS;    ///< المنصة المستهدفة
    std::vector<iOSArchitecture> architectures; ///< المعماريات
    iOSBuildType buildType = iOSBuildType::Debug;
    
    std::string developmentTeam;                ///< Team ID للتوقيع
    std::string provisioningProfile;            ///< Provisioning Profile
    
    bool useSwiftUI = true;                     ///< استخدام SwiftUI
    bool useStoryboards = false;                ///< استخدام Storyboards
    bool useCoreData = false;                   ///< استخدام Core Data
    bool useCloudKit = false;                   ///< استخدام CloudKit
    
    std::vector<std::string> frameworks;        ///< Frameworks إضافية
    std::vector<std::string> capabilities;      ///< قدرات التطبيق
};

// ═══════════════════════════════════════════════════════════════════════════
//                    نتيجة البناء / Build Result
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct iOSBuildResult
 * @brief نتيجة بناء iOS
 */
struct iOSBuildResult {
    bool success = false;
    std::filesystem::path appPath;              ///< مسار .app
    std::filesystem::path ipaPath;              ///< مسار .ipa (للتوزيع)
    std::filesystem::path dsymPath;             ///< رموز التصحيح
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    std::chrono::milliseconds buildTime;
    
    std::string toArabicSummary() const;
};

// ═══════════════════════════════════════════════════════════════════════════
//                    هدف iOS / iOS Target
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @class iOSTarget
 * @brief الفئة الرئيسية لبناء تطبيقات iOS من كود لغة ص
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                         كيفية الاستخدام
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * @code{.cpp}
 * // إنشاء هدف iOS
 * iOSTarget target;
 * 
 * // تهيئة الإعدادات
 * iOSProjectConfig config;
 * config.projectName = "تطبيقي";
 * config.infoPlist.bundleIdentifier = "com.example.myapp";
 * config.infoPlist.bundleDisplayName = "تطبيقي الأول";
 * 
 * // بناء التطبيق
 * auto result = target.build(sadSourceFiles, config);
 * if (result.success) {
 *     std::cout << "تم البناء: " << result.appPath << std::endl;
 * }
 * @endcode
 */
class iOSTarget {
public:
    using ProgressCallback = std::function<void(const std::string& stage, int percent)>;
    
    iOSTarget();
    ~iOSTarget();
    
    // ══════════════════════════════════════════════════════════════════════
    //                    التهيئة / Initialization
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief تهيئة هدف iOS
     */
    std::optional<std::string> initialize(const iOSProjectConfig& config);
    
    /**
     * @brief التحقق من توفر أدوات البناء
     * @note يتطلب macOS و Xcode
     */
    std::vector<std::string> checkPrerequisites() const;
    
    // ══════════════════════════════════════════════════════════════════════
    //                    البناء / Building
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief بناء تطبيق iOS من ملفات ص
     */
    iOSBuildResult build(
        const std::vector<std::filesystem::path>& sourceFiles,
        const iOSProjectConfig& config);
    
    /**
     * @brief بناء للمحاكي فقط
     */
    iOSBuildResult buildForSimulator(
        const std::vector<std::filesystem::path>& sourceFiles,
        const iOSProjectConfig& config);
    
    /**
     * @brief بناء أرشيف للتوزيع
     */
    iOSBuildResult archive(
        const std::vector<std::filesystem::path>& sourceFiles,
        const iOSProjectConfig& config,
        iOSDistributionMethod method);
    
    // ══════════════════════════════════════════════════════════════════════
    //                    توليد المشروع / Project Generation
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief توليد مشروع Xcode كامل
     */
    std::variant<std::filesystem::path, std::string> generateXcodeProject(
        const std::filesystem::path& outputDir,
        const iOSProjectConfig& config);
    
    /**
     * @brief توليد كود Swift Bridge
     */
    std::string generateSwiftBridge(const iOSProjectConfig& config) const;
    
    /**
     * @brief توليد واجهة SwiftUI من كود ص
     */
    std::string generateSwiftUIView(const std::string& sadUICode) const;
    
    // ══════════════════════════════════════════════════════════════════════
    //                    المحاكي / Simulator
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief الحصول على قائمة المحاكيات
     */
    std::vector<std::pair<std::string, std::string>> listSimulators() const;
    
    /**
     * @brief تشغيل محاكي
     */
    std::optional<std::string> bootSimulator(const std::string& simulatorId);
    
    /**
     * @brief تثبيت التطبيق على محاكي
     */
    std::optional<std::string> installOnSimulator(
        const std::filesystem::path& appPath,
        const std::string& simulatorId);
    
    /**
     * @brief تشغيل التطبيق على محاكي
     */
    std::optional<std::string> launchOnSimulator(
        const std::string& bundleId,
        const std::string& simulatorId);
    
    // ══════════════════════════════════════════════════════════════════════
    //                    الأجهزة الحقيقية / Real Devices
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief الحصول على قائمة الأجهزة المتصلة
     */
    std::vector<std::pair<std::string, std::string>> listDevices() const;
    
    /**
     * @brief تثبيت على جهاز حقيقي
     */
    std::optional<std::string> installOnDevice(
        const std::filesystem::path& appPath,
        const std::string& deviceId);
    
    // ══════════════════════════════════════════════════════════════════════
    //                    التوقيع / Code Signing
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief توقيع التطبيق
     */
    std::optional<std::string> signApp(
        const std::filesystem::path& appPath,
        const std::string& identity,
        const std::string& provisioningProfile);
    
    /**
     * @brief الحصول على هويات التوقيع المتاحة
     */
    std::vector<std::string> listSigningIdentities() const;
    
    /**
     * @brief الحصول على Provisioning Profiles المتاحة
     */
    std::vector<std::pair<std::string, std::string>> listProvisioningProfiles() const;
    
    // ══════════════════════════════════════════════════════════════════════
    //                    الإشعارات / Callbacks
    // ══════════════════════════════════════════════════════════════════════
    
    void setProgressCallback(ProgressCallback callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    std::optional<std::string> compileToLlvmIr(
        const std::vector<std::filesystem::path>& sourceFiles);
    
    std::optional<std::string> generateSwiftWrapper(
        const iOSProjectConfig& config);
    
    std::optional<std::string> runXcodebuild(
        const std::filesystem::path& projectPath,
        const iOSProjectConfig& config);
};

// ═══════════════════════════════════════════════════════════════════════════
//                    Capabilities / قدرات التطبيق
// ═══════════════════════════════════════════════════════════════════════════

namespace Capabilities {
    constexpr const char* PUSH_NOTIFICATIONS = "com.apple.Push";
    constexpr const char* IN_APP_PURCHASE = "com.apple.InAppPurchase";
    constexpr const char* SIGN_IN_WITH_APPLE = "com.apple.Sign-In-with-Apple";
    constexpr const char* ICLOUD = "com.apple.iCloud";
    constexpr const char* GAME_CENTER = "com.apple.GameCenter";
    constexpr const char* HEALTHKIT = "com.apple.HealthKit";
    constexpr const char* HOMEKIT = "com.apple.HomeKit";
    constexpr const char* SIRI = "com.apple.Siri";
    constexpr const char* WALLET = "com.apple.Wallet";
    constexpr const char* BACKGROUND_MODES = "com.apple.BackgroundModes";
}

// ═══════════════════════════════════════════════════════════════════════════
//                    مساعدات / Helpers
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief التحقق من تثبيت Xcode
 */
bool isXcodeInstalled();

/**
 * @brief الحصول على إصدار Xcode
 */
std::optional<std::string> getXcodeVersion();

/**
 * @brief التحقق من قبول اتفاقية Xcode
 */
bool isXcodeLicenseAccepted();

} // namespace Mobile
} // namespace Tools
} // namespace Sad

#endif // SAD_TOOLS_IOS_TARGET_HPP
