// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file android_target.hpp
 * @brief هدف أندرويد - تكامل NDK لإنشاء تطبيقات أندرويد من كود لغة ص
 * 
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║                    نظام بناء تطبيقات أندرويد بلغة ص                        ║
 * ║              Android Application Build System for Sad Language             ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                              نظرة عامة
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يُمكّن مترجم لغة ص من إنشاء تطبيقات أندرويد أصلية.
 * يتكامل مع Android NDK (Native Development Kit) لترجمة كود ص
 * إلى مكتبات أصلية (.so) تعمل على معالجات ARM و x86.
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                         المعماريات المدعومة
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * ┌─────────────────┬────────────────┬──────────────────────────────────────┐
 * │   المعمارية      │    الاسم        │              الوصف                    │
 * ├─────────────────┼────────────────┼──────────────────────────────────────┤
 * │ arm64-v8a       │ ARM 64-bit     │ معظم الهواتف الحديثة (2015+)         │
 * │ armeabi-v7a     │ ARM 32-bit     │ الهواتف القديمة (دعم واسع)           │
 * │ x86_64          │ Intel 64-bit   │ المحاكيات وبعض الأجهزة اللوحية       │
 * │ x86             │ Intel 32-bit   │ المحاكيات القديمة                     │
 * └─────────────────┴────────────────┴──────────────────────────────────────┘
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                         مستويات API المدعومة
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * - الحد الأدنى: API 21 (Android 5.0 Lollipop) - يدعم 98% من الأجهزة
 * - الموصى به: API 26 (Android 8.0 Oreo) - ميزات حديثة
 * - الأحدث: API 34 (Android 14) - أحدث الميزات
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                         مثال الاستخدام بالعربية
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * @code{.sad}
 * // ملف: تطبيقي.ص
 * استورد واجهة_أندرويد
 * استورد مكونات_واجهة
 * 
 * // تعريف النشاط الرئيسي
 * نشاط رئيسي {
 *     عند_الإنشاء() {
 *         أعرض_رسالة("مرحباً من لغة ص!")
 *     }
 *     
 *     عند_الاستئناف() {
 *         // كود عند استئناف التطبيق
 *     }
 * }
 * 
 * // استخدام واجهة المستخدم التصريحية
 * واجهة شاشة_رئيسية {
 *     عمود {
 *         نص("تطبيق لغة ص")
 *         زر("اضغط هنا") {
 *             عند_النقر: || { أعرض_رسالة("تم النقر!") }
 *         }
 *     }
 * }
 * @endcode
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#ifndef SAD_TOOLS_ANDROID_TARGET_HPP
#define SAD_TOOLS_ANDROID_TARGET_HPP

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
//                    تعدادات أندرويد / Android Enumerations
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @enum AndroidArchitecture
 * @brief معماريات المعالج المدعومة لأندرويد
 * 
 * تحدد المعماريات التي يمكن لمترجم ص استهدافها.
 * كل معمارية تنتج ملف .so منفصل.
 */
enum class AndroidArchitecture {
    ARM64_V8A,      ///< معمارية ARM 64-بت (الأكثر شيوعاً حالياً)
    ARMEABI_V7A,    ///< معمارية ARM 32-بت (للتوافق مع الأجهزة القديمة)
    X86_64,         ///< معمارية Intel 64-بت (للمحاكيات)
    X86,            ///< معمارية Intel 32-بت (للمحاكيات القديمة)
    All             ///< جميع المعماريات (للتوزيع)
};

/**
 * @brief تحويل معمارية أندرويد لاسم مجلد NDK
 */
std::string architectureToNdkName(AndroidArchitecture arch);

/**
 * @brief تحويل معمارية أندرويد لاسم LLVM triple
 */
std::string architectureToLlvmTriple(AndroidArchitecture arch);

/**
 * @enum AndroidBuildType
 * @brief نوع البناء لتطبيق أندرويد
 */
enum class AndroidBuildType {
    Debug,          ///< بناء تطوير (رموز تصحيح كاملة)
    Release,        ///< بناء إصدار (محسّن)
    Profile         ///< بناء تحليل أداء (محسّن + رموز)
};

/**
 * @enum AndroidComponentType
 * @brief أنواع مكونات تطبيق أندرويد
 */
enum class AndroidComponentType {
    Activity,       ///< نشاط (شاشة)
    Service,        ///< خدمة (عمل خلفي)
    Receiver,       ///< مستقبل بث
    Provider        ///< مزود محتوى
};

// ═══════════════════════════════════════════════════════════════════════════
//                    إعدادات NDK / NDK Configuration
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct NdkConfig
 * @brief إعدادات Android NDK
 * 
 * تحتوي على جميع المعلومات اللازمة للتكامل مع NDK.
 */
struct NdkConfig {
    std::filesystem::path ndkPath;              ///< مسار NDK (مثل: C:/Android/ndk/25.2.9519653)
    std::string ndkVersion;                     ///< إصدار NDK (مثل: 25.2.9519653)
    int minApiLevel = 21;                       ///< أقل مستوى API مدعوم
    int targetApiLevel = 34;                    ///< مستوى API المستهدف
    std::vector<AndroidArchitecture> architectures; ///< المعماريات للبناء
    
    /**
     * @brief التحقق من صحة إعدادات NDK
     * @return رسالة خطأ إذا كانت الإعدادات غير صالحة
     */
    std::optional<std::string> validate() const;
    
    /**
     * @brief اكتشاف NDK تلقائياً من متغيرات البيئة
     * @return إعدادات NDK المكتشفة أو nullopt
     */
    static std::optional<NdkConfig> autoDetect();
    
    /**
     * @brief الحصول على مسار toolchain
     */
    std::filesystem::path getToolchainPath() const;
    
    /**
     * @brief الحصول على مسار sysroot
     */
    std::filesystem::path getSysrootPath() const;
    
    /**
     * @brief الحصول على مسار المترجم (clang)
     */
    std::filesystem::path getClangPath() const;
};

// ═══════════════════════════════════════════════════════════════════════════
//                    إعدادات المشروع / Project Configuration
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct AndroidManifestInfo
 * @brief معلومات ملف AndroidManifest.xml
 */
struct AndroidManifestInfo {
    std::string packageName;                    ///< اسم الحزمة (مثل: com.example.app)
    std::string applicationName;                ///< اسم التطبيق بالعربية
    std::string applicationNameEn;              ///< اسم التطبيق بالإنجليزية
    int versionCode = 1;                        ///< رمز الإصدار (للتحديثات)
    std::string versionName = "1.0.0";          ///< اسم الإصدار للعرض
    int minSdk = 21;                            ///< أقل SDK مدعوم
    int targetSdk = 34;                         ///< SDK المستهدف
    
    std::vector<std::string> permissions;       ///< الأذونات المطلوبة
    std::vector<std::string> features;          ///< الميزات المطلوبة
    
    bool supportsRtl = true;                    ///< دعم الكتابة من اليمين (للعربية)
    std::string theme;                          ///< سمة التطبيق
    
    /**
     * @brief توليد نص AndroidManifest.xml
     */
    std::string generateXml() const;
};

/**
 * @struct AndroidProjectConfig
 * @brief إعدادات مشروع أندرويد الكاملة
 */
struct AndroidProjectConfig {
    std::string projectName;                    ///< اسم المشروع
    std::filesystem::path outputDir;            ///< مجلد الإخراج
    AndroidManifestInfo manifest;               ///< معلومات Manifest
    NdkConfig ndk;                              ///< إعدادات NDK
    AndroidBuildType buildType = AndroidBuildType::Debug;
    
    bool useKotlin = false;                     ///< استخدام Kotlin للجسر
    bool useJetpackCompose = true;              ///< استخدام Jetpack Compose للواجهة
    bool enableDataBinding = true;              ///< تفعيل ربط البيانات
    
    std::vector<std::string> gradleDependencies; ///< تبعيات Gradle إضافية
    std::unordered_map<std::string, std::string> buildConfigFields; ///< حقول BuildConfig
};

// ═══════════════════════════════════════════════════════════════════════════
//                    مكون أندرويد / Android Component
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct AndroidComponent
 * @brief مكون تطبيق أندرويد (نشاط، خدمة، إلخ)
 */
struct AndroidComponent {
    std::string name;                           ///< اسم المكون
    std::string arabicName;                     ///< الاسم بالعربية
    AndroidComponentType type;                  ///< نوع المكون
    bool exported = false;                      ///< هل المكون متاح خارجياً
    std::vector<std::string> intentFilters;     ///< فلاتر Intent
    
    /**
     * @brief توليد تعريف XML للمكون
     */
    std::string toManifestXml() const;
};

// ═══════════════════════════════════════════════════════════════════════════
//                    نتيجة البناء / Build Result
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct AndroidBuildResult
 * @brief نتيجة عملية بناء تطبيق أندرويد
 */
struct AndroidBuildResult {
    bool success = false;                       ///< هل نجح البناء
    std::filesystem::path apkPath;              ///< مسار ملف APK الناتج
    std::filesystem::path aabPath;              ///< مسار ملف AAB (للنشر على Play Store)
    std::vector<std::filesystem::path> soFiles; ///< ملفات المكتبات الأصلية
    std::vector<std::string> warnings;          ///< تحذيرات البناء
    std::vector<std::string> errors;            ///< أخطاء البناء
    std::chrono::milliseconds buildTime;        ///< وقت البناء
    
    std::string toArabicSummary() const;        ///< ملخص عربي للنتيجة
};

// ═══════════════════════════════════════════════════════════════════════════
//                    هدف أندرويد / Android Target
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @class AndroidTarget
 * @brief الفئة الرئيسية لبناء تطبيقات أندرويد من كود لغة ص
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                         كيفية الاستخدام
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * @code{.cpp}
 * // إنشاء هدف أندرويد
 * AndroidTarget target;
 * 
 * // تهيئة الإعدادات
 * AndroidProjectConfig config;
 * config.projectName = "تطبيقي";
 * config.manifest.packageName = "com.example.myapp";
 * config.manifest.applicationName = "تطبيقي الأول";
 * 
 * // بناء التطبيق
 * auto result = target.build(sadSourceFiles, config);
 * if (result.success) {
 *     std::cout << "تم البناء: " << result.apkPath << std::endl;
 * }
 * @endcode
 */
class AndroidTarget {
public:
    using ProgressCallback = std::function<void(const std::string& stage, int percent)>;
    
    AndroidTarget();
    ~AndroidTarget();
    
    // ══════════════════════════════════════════════════════════════════════
    //                    التهيئة / Initialization
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief تهيئة هدف أندرويد
     * @param config إعدادات المشروع
     * @return رسالة خطأ إذا فشلت التهيئة
     */
    std::optional<std::string> initialize(const AndroidProjectConfig& config);
    
    /**
     * @brief التحقق من توفر أدوات البناء
     * @return قائمة الأدوات المفقودة
     */
    std::vector<std::string> checkPrerequisites() const;
    
    // ══════════════════════════════════════════════════════════════════════
    //                    البناء / Building
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief بناء تطبيق أندرويد من ملفات ص
     * @param sourceFiles ملفات كود ص المصدرية
     * @param config إعدادات المشروع
     * @return نتيجة البناء
     */
    AndroidBuildResult build(
        const std::vector<std::filesystem::path>& sourceFiles,
        const AndroidProjectConfig& config);
    
    /**
     * @brief بناء مكتبة أصلية فقط (بدون APK)
     * @param sourceFiles ملفات كود ص المصدرية
     * @param arch المعمارية المستهدفة
     * @return مسار ملف .so أو خطأ
     */
    std::variant<std::filesystem::path, std::string> buildNativeLibrary(
        const std::vector<std::filesystem::path>& sourceFiles,
        AndroidArchitecture arch);
    
    /**
     * @brief بناء تدريجي (فقط الملفات المتغيرة)
     */
    AndroidBuildResult incrementalBuild(
        const std::vector<std::filesystem::path>& changedFiles);
    
    // ══════════════════════════════════════════════════════════════════════
    //                    توليد المشروع / Project Generation
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief توليد مشروع Android Studio كامل
     * @param outputDir مجلد الإخراج
     * @param config إعدادات المشروع
     * @return مسار المشروع أو خطأ
     */
    std::variant<std::filesystem::path, std::string> generateProject(
        const std::filesystem::path& outputDir,
        const AndroidProjectConfig& config);
    
    /**
     * @brief توليد ملف build.gradle
     */
    std::string generateBuildGradle(const AndroidProjectConfig& config) const;
    
    /**
     * @brief توليد ملف CMakeLists.txt للكود الأصلي
     */
    std::string generateNativeCMake(const AndroidProjectConfig& config) const;
    
    // ══════════════════════════════════════════════════════════════════════
    //                    التشغيل والتثبيت / Running & Installing
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief تثبيت APK على جهاز متصل
     * @param apkPath مسار ملف APK
     * @param deviceId معرف الجهاز (اختياري)
     * @return نجاح أو رسالة خطأ
     */
    std::optional<std::string> installApk(
        const std::filesystem::path& apkPath,
        const std::string& deviceId = "");
    
    /**
     * @brief تشغيل التطبيق على جهاز
     */
    std::optional<std::string> runApp(
        const std::string& packageName,
        const std::string& activityName,
        const std::string& deviceId = "");
    
    /**
     * @brief الحصول على قائمة الأجهزة المتصلة
     */
    std::vector<std::pair<std::string, std::string>> listDevices() const;
    
    // ══════════════════════════════════════════════════════════════════════
    //                    التصحيح / Debugging
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief بدء جلسة تصحيح
     * @param deviceId معرف الجهاز
     * @param port منفذ التصحيح
     */
    std::optional<std::string> startDebugSession(
        const std::string& deviceId,
        int port = 5005);
    
    /**
     * @brief قراءة سجلات Logcat
     * @param packageName فلترة حسب اسم الحزمة
     */
    std::vector<std::string> readLogcat(const std::string& packageName) const;
    
    // ══════════════════════════════════════════════════════════════════════
    //                    الإشعارات / Callbacks
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief تعيين دالة إشعار التقدم
     */
    void setProgressCallback(ProgressCallback callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    // دوال مساعدة داخلية
    std::optional<std::string> compileToLlvmIr(
        const std::vector<std::filesystem::path>& sourceFiles);
    
    std::optional<std::string> linkNativeLibrary(
        const std::vector<std::filesystem::path>& objectFiles,
        AndroidArchitecture arch,
        const std::filesystem::path& outputPath);
    
    std::optional<std::string> packageApk(
        const AndroidProjectConfig& config,
        const std::vector<std::filesystem::path>& soFiles);
    
    std::optional<std::string> signApk(
        const std::filesystem::path& apkPath,
        const std::filesystem::path& keystorePath);
};

// ═══════════════════════════════════════════════════════════════════════════
//                    أذونات أندرويد الشائعة / Common Android Permissions
// ═══════════════════════════════════════════════════════════════════════════

namespace Permissions {
    // أذونات الكاميرا
    constexpr const char* CAMERA = "android.permission.CAMERA";
    
    // أذونات الموقع
    constexpr const char* ACCESS_FINE_LOCATION = "android.permission.ACCESS_FINE_LOCATION";
    constexpr const char* ACCESS_COARSE_LOCATION = "android.permission.ACCESS_COARSE_LOCATION";
    
    // أذونات التخزين
    constexpr const char* READ_EXTERNAL_STORAGE = "android.permission.READ_EXTERNAL_STORAGE";
    constexpr const char* WRITE_EXTERNAL_STORAGE = "android.permission.WRITE_EXTERNAL_STORAGE";
    
    // أذونات الشبكة
    constexpr const char* INTERNET = "android.permission.INTERNET";
    constexpr const char* ACCESS_NETWORK_STATE = "android.permission.ACCESS_NETWORK_STATE";
    
    // أذونات أخرى
    constexpr const char* VIBRATE = "android.permission.VIBRATE";
    constexpr const char* RECORD_AUDIO = "android.permission.RECORD_AUDIO";
    constexpr const char* BLUETOOTH = "android.permission.BLUETOOTH";
}

// ═══════════════════════════════════════════════════════════════════════════
//                    مساعدات / Helpers
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief الحصول على مسار Android SDK من البيئة
 */
std::optional<std::filesystem::path> getAndroidSdkPath();

/**
 * @brief الحصول على مسار Android NDK من البيئة
 */
std::optional<std::filesystem::path> getAndroidNdkPath();

/**
 * @brief التحقق من تثبيت أدوات البناء
 */
bool isBuildToolsInstalled(int version);

} // namespace Mobile
} // namespace Tools
} // namespace Sad

#endif // SAD_TOOLS_ANDROID_TARGET_HPP
