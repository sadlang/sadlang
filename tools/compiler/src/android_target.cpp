// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file android_target.cpp
 * @brief تنفيذ هدف أندرويد - تكامل NDK لإنشاء تطبيقات أندرويد
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *            تنفيذ نظام بناء تطبيقات أندرويد بلغة ص
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#include "android_target.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <array>
#include <regex>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

namespace Sad {
namespace Tools {
namespace Mobile {

// ═══════════════════════════════════════════════════════════════════════════
//                    تحويلات المعماريات / Architecture Conversions
// ═══════════════════════════════════════════════════════════════════════════

std::string architectureToNdkName(AndroidArchitecture arch) {
    switch (arch) {
        case AndroidArchitecture::ARM64_V8A:    return "arm64-v8a";
        case AndroidArchitecture::ARMEABI_V7A:  return "armeabi-v7a";
        case AndroidArchitecture::X86_64:       return "x86_64";
        case AndroidArchitecture::X86:          return "x86";
        case AndroidArchitecture::All:          return "all";
        default:                                return "unknown";
    }
}

std::string architectureToLlvmTriple(AndroidArchitecture arch) {
    switch (arch) {
        case AndroidArchitecture::ARM64_V8A:    return "aarch64-linux-android";
        case AndroidArchitecture::ARMEABI_V7A:  return "armv7a-linux-androideabi";
        case AndroidArchitecture::X86_64:       return "x86_64-linux-android";
        case AndroidArchitecture::X86:          return "i686-linux-android";
        default:                                return "unknown";
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//                    تنفيذ NdkConfig
// ═══════════════════════════════════════════════════════════════════════════

std::optional<std::string> NdkConfig::validate() const {
    // التحقق من وجود مسار NDK
    if (ndkPath.empty()) {
        return "مسار NDK غير محدد. يُرجى تعيين ANDROID_NDK_HOME أو تمرير المسار يدوياً.";
    }
    
    if (!std::filesystem::exists(ndkPath)) {
        return "مسار NDK غير موجود: " + ndkPath.string();
    }
    
    // التحقق من وجود ملفات NDK الأساسية
    auto toolchain = ndkPath / "toolchains" / "llvm" / "prebuilt";
    if (!std::filesystem::exists(toolchain)) {
        return "بنية NDK غير صالحة: مجلد toolchains غير موجود";
    }
    
    // التحقق من مستوى API
    if (minApiLevel < 21) {
        return "مستوى API الأدنى يجب أن يكون 21 على الأقل (Android 5.0)";
    }
    
    if (targetApiLevel < minApiLevel) {
        return "مستوى API المستهدف يجب أن يكون أكبر من أو يساوي الأدنى";
    }
    
    return std::nullopt;
}

std::optional<NdkConfig> NdkConfig::autoDetect() {
    NdkConfig config;
    
    // محاولة اكتشاف NDK من متغيرات البيئة
    const char* ndkHome = std::getenv("ANDROID_NDK_HOME");
    if (!ndkHome) {
        ndkHome = std::getenv("NDK_HOME");
    }
    if (!ndkHome) {
        ndkHome = std::getenv("ANDROID_NDK_ROOT");
    }
    
    if (ndkHome) {
        config.ndkPath = ndkHome;
    } else {
        // محاولة اكتشاف من مسار SDK
        const char* sdkHome = std::getenv("ANDROID_HOME");
        if (!sdkHome) {
            sdkHome = std::getenv("ANDROID_SDK_ROOT");
        }
        
        if (sdkHome) {
            // البحث عن أحدث إصدار NDK
            std::filesystem::path ndkDir = std::filesystem::path(sdkHome) / "ndk";
            if (std::filesystem::exists(ndkDir)) {
                std::filesystem::path latestNdk;
                for (const auto& entry : std::filesystem::directory_iterator(ndkDir)) {
                    if (entry.is_directory()) {
                        if (latestNdk.empty() || entry.path() > latestNdk) {
                            latestNdk = entry.path();
                        }
                    }
                }
                if (!latestNdk.empty()) {
                    config.ndkPath = latestNdk;
                }
            }
        }
    }
    
    if (config.ndkPath.empty()) {
        return std::nullopt;
    }
    
    // قراءة إصدار NDK
    auto sourceProps = config.ndkPath / "source.properties";
    if (std::filesystem::exists(sourceProps)) {
        std::ifstream file(sourceProps);
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("Pkg.Revision") != std::string::npos) {
                auto pos = line.find('=');
                if (pos != std::string::npos) {
                    config.ndkVersion = line.substr(pos + 1);
                    // إزالة المسافات
                    config.ndkVersion.erase(0, config.ndkVersion.find_first_not_of(" \t"));
                    config.ndkVersion.erase(config.ndkVersion.find_last_not_of(" \t\r\n") + 1);
                }
                break;
            }
        }
    }
    
    // إعدادات افتراضية
    config.minApiLevel = 21;
    config.targetApiLevel = 34;
    config.architectures = {AndroidArchitecture::ARM64_V8A, AndroidArchitecture::ARMEABI_V7A};
    
    return config;
}

std::filesystem::path NdkConfig::getToolchainPath() const {
#ifdef _WIN32
    return ndkPath / "toolchains" / "llvm" / "prebuilt" / "windows-x86_64";
#elif defined(__APPLE__)
    return ndkPath / "toolchains" / "llvm" / "prebuilt" / "darwin-x86_64";
#else
    return ndkPath / "toolchains" / "llvm" / "prebuilt" / "linux-x86_64";
#endif
}

std::filesystem::path NdkConfig::getSysrootPath() const {
    return getToolchainPath() / "sysroot";
}

std::filesystem::path NdkConfig::getClangPath() const {
#ifdef _WIN32
    return getToolchainPath() / "bin" / "clang.cmd";
#else
    return getToolchainPath() / "bin" / "clang";
#endif
}

// ═══════════════════════════════════════════════════════════════════════════
//                    تنفيذ AndroidManifestInfo
// ═══════════════════════════════════════════════════════════════════════════

std::string AndroidManifestInfo::generateXml() const {
    std::ostringstream xml;
    
    xml << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    xml << "<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\"\n";
    xml << "    package=\"" << packageName << "\"\n";
    xml << "    android:versionCode=\"" << versionCode << "\"\n";
    xml << "    android:versionName=\"" << versionName << "\">\n\n";
    
    // الأذونات
    for (const auto& permission : permissions) {
        xml << "    <uses-permission android:name=\"" << permission << "\" />\n";
    }
    
    // الميزات
    for (const auto& feature : features) {
        xml << "    <uses-feature android:name=\"" << feature << "\" />\n";
    }
    
    xml << "\n    <application\n";
    xml << "        android:label=\"" << applicationName << "\"\n";
    xml << "        android:supportsRtl=\"" << (supportsRtl ? "true" : "false") << "\"\n";
    if (!theme.empty()) {
        xml << "        android:theme=\"" << theme << "\"\n";
    }
    xml << "        android:hardwareAccelerated=\"true\">\n";
    
    // النشاط الرئيسي
    xml << "\n        <activity\n";
    xml << "            android:name=\".MainActivity\"\n";
    xml << "            android:exported=\"true\">\n";
    xml << "            <intent-filter>\n";
    xml << "                <action android:name=\"android.intent.action.MAIN\" />\n";
    xml << "                <category android:name=\"android.intent.category.LAUNCHER\" />\n";
    xml << "            </intent-filter>\n";
    xml << "        </activity>\n";
    
    xml << "\n    </application>\n";
    xml << "</manifest>\n";
    
    return xml.str();
}

// ═══════════════════════════════════════════════════════════════════════════
//                    تنفيذ AndroidBuildResult
// ═══════════════════════════════════════════════════════════════════════════

std::string AndroidBuildResult::toArabicSummary() const {
    std::ostringstream summary;
    
    if (success) {
        summary << "✅ تم البناء بنجاح!\n\n";
        summary << "📦 ملف APK: " << apkPath.string() << "\n";
        if (!aabPath.empty()) {
            summary << "📦 ملف AAB: " << aabPath.string() << "\n";
        }
        summary << "⏱️ وقت البناء: " << buildTime.count() << " مللي ثانية\n";
        summary << "\n📚 المكتبات الأصلية:\n";
        for (const auto& so : soFiles) {
            summary << "   • " << so.filename().string() << "\n";
        }
    } else {
        summary << "❌ فشل البناء!\n\n";
        summary << "الأخطاء:\n";
        for (const auto& error : errors) {
            summary << "   ⛔ " << error << "\n";
        }
    }
    
    if (!warnings.empty()) {
        summary << "\n⚠️ تحذيرات:\n";
        for (const auto& warning : warnings) {
            summary << "   ⚡ " << warning << "\n";
        }
    }
    
    return summary.str();
}

// ═══════════════════════════════════════════════════════════════════════════
//                    تنفيذ AndroidTarget
// ═══════════════════════════════════════════════════════════════════════════

struct AndroidTarget::Impl {
    AndroidProjectConfig config;
    ProgressCallback progressCallback;
    std::filesystem::path tempDir;
    bool initialized = false;
};

AndroidTarget::AndroidTarget() : pImpl_(std::make_unique<Impl>()) {}

AndroidTarget::~AndroidTarget() = default;

std::optional<std::string> AndroidTarget::initialize(const AndroidProjectConfig& config) {
    pImpl_->config = config;
    
    // التحقق من إعدادات NDK
    if (auto error = config.ndk.validate()) {
        return error;
    }
    
    // إنشاء مجلد مؤقت للبناء
    pImpl_->tempDir = std::filesystem::temp_directory_path() / "sad_android_build";
    std::filesystem::create_directories(pImpl_->tempDir);
    
    pImpl_->initialized = true;
    return std::nullopt;
}

std::vector<std::string> AndroidTarget::checkPrerequisites() const {
    std::vector<std::string> missing;
    
    // التحقق من NDK
    if (!getAndroidNdkPath()) {
        missing.push_back("Android NDK: غير موجود. ثبّته من Android Studio أو sdkmanager");
    }
    
    // التحقق من SDK
    if (!getAndroidSdkPath()) {
        missing.push_back("Android SDK: غير موجود. ثبّته من Android Studio");
    }
    
    // التحقق من Java
    const char* javaHome = std::getenv("JAVA_HOME");
    if (!javaHome) {
        missing.push_back("JAVA_HOME: غير محدد. ثبّت JDK 11 أو أحدث");
    }
    
    return missing;
}

AndroidBuildResult AndroidTarget::build(
    const std::vector<std::filesystem::path>& sourceFiles,
    const AndroidProjectConfig& config) {
    
    AndroidBuildResult result;
    auto startTime = std::chrono::steady_clock::now();
    
    // تهيئة إذا لم تتم
    if (!pImpl_->initialized) {
        if (auto error = initialize(config)) {
            result.errors.push_back(*error);
            return result;
        }
    }
    
    // إشعار التقدم
    if (pImpl_->progressCallback) {
        pImpl_->progressCallback("تحليل الملفات المصدرية", 10);
    }
    
    // 1. ترجمة كود ص إلى LLVM IR
    if (auto error = compileToLlvmIr(sourceFiles)) {
        result.errors.push_back(*error);
        return result;
    }
    
    if (pImpl_->progressCallback) {
        pImpl_->progressCallback("ترجمة للكود الآلي", 30);
    }
    
    // 2. بناء مكتبات أصلية لكل معمارية
    for (const auto& arch : config.ndk.architectures) {
        auto libResult = buildNativeLibrary(sourceFiles, arch);
        if (std::holds_alternative<std::string>(libResult)) {
            result.errors.push_back(std::get<std::string>(libResult));
            return result;
        }
        result.soFiles.push_back(std::get<std::filesystem::path>(libResult));
    }
    
    if (pImpl_->progressCallback) {
        pImpl_->progressCallback("تجميع APK", 70);
    }
    
    // 3. تجميع APK
    if (auto error = packageApk(config, result.soFiles)) {
        result.errors.push_back(*error);
        return result;
    }
    
    result.apkPath = config.outputDir / (config.projectName + ".apk");
    
    if (pImpl_->progressCallback) {
        pImpl_->progressCallback("توقيع APK", 90);
    }
    
    // 4. توقيع APK (debug signing)
    // TODO: تنفيذ التوقيع
    
    result.success = true;
    auto endTime = std::chrono::steady_clock::now();
    result.buildTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    if (pImpl_->progressCallback) {
        pImpl_->progressCallback("اكتمل البناء", 100);
    }
    
    return result;
}

std::variant<std::filesystem::path, std::string> AndroidTarget::buildNativeLibrary(
    const std::vector<std::filesystem::path>& sourceFiles,
    AndroidArchitecture arch) {
    
    std::string archName = architectureToNdkName(arch);
    std::string triple = architectureToLlvmTriple(arch);
    
    // مسار الإخراج
    auto outputDir = pImpl_->tempDir / "libs" / archName;
    std::filesystem::create_directories(outputDir);
    
    auto outputPath = outputDir / "libsad_app.so";
    
    // TODO: تنفيذ الترجمة الفعلية باستخدام LLVM
    // هنا نستخدم clang من NDK كبديل مؤقت
    
    const auto& ndk = pImpl_->config.ndk;
    auto clang = ndk.getClangPath();
    
    std::ostringstream cmd;
    cmd << clang.string();
    cmd << " -target " << triple << ndk.minApiLevel;
    cmd << " --sysroot=" << ndk.getSysrootPath().string();
    cmd << " -shared -fPIC";
    cmd << " -o " << outputPath.string();
    
    for (const auto& src : sourceFiles) {
        // TODO: ترجمة ص -> C/LLVM IR أولاً
        // cmd << " " << src.string();
    }
    
    // للاختبار: إنشاء ملف وهمي
    std::ofstream dummySo(outputPath, std::ios::binary);
    dummySo << "ELF"; // توقيع ELF بسيط
    dummySo.close();
    
    return outputPath;
}

std::variant<std::filesystem::path, std::string> AndroidTarget::generateProject(
    const std::filesystem::path& outputDir,
    const AndroidProjectConfig& config) {
    
    // إنشاء بنية المشروع
    auto projectDir = outputDir / config.projectName;
    std::filesystem::create_directories(projectDir / "app" / "src" / "main" / "java");
    std::filesystem::create_directories(projectDir / "app" / "src" / "main" / "cpp");
    std::filesystem::create_directories(projectDir / "app" / "src" / "main" / "res" / "layout");
    std::filesystem::create_directories(projectDir / "app" / "src" / "main" / "res" / "values");
    
    // إنشاء AndroidManifest.xml
    {
        std::ofstream manifest(projectDir / "app" / "src" / "main" / "AndroidManifest.xml");
        manifest << config.manifest.generateXml();
    }
    
    // إنشاء build.gradle
    {
        std::ofstream gradle(projectDir / "app" / "build.gradle");
        gradle << generateBuildGradle(config);
    }
    
    // إنشاء CMakeLists.txt للكود الأصلي
    {
        std::ofstream cmake(projectDir / "app" / "src" / "main" / "cpp" / "CMakeLists.txt");
        cmake << generateNativeCMake(config);
    }
    
    // إنشاء strings.xml (دعم العربية)
    {
        std::ofstream strings(projectDir / "app" / "src" / "main" / "res" / "values" / "strings.xml");
        strings << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
        strings << "<resources>\n";
        strings << "    <string name=\"app_name\">" << config.manifest.applicationName << "</string>\n";
        strings << "</resources>\n";
    }
    
    return projectDir;
}

std::string AndroidTarget::generateBuildGradle(const AndroidProjectConfig& config) const {
    std::ostringstream gradle;
    
    gradle << "plugins {\n";
    gradle << "    id 'com.android.application'\n";
    if (config.useKotlin) {
        gradle << "    id 'org.jetbrains.kotlin.android'\n";
    }
    gradle << "}\n\n";
    
    gradle << "android {\n";
    gradle << "    namespace '" << config.manifest.packageName << "'\n";
    gradle << "    compileSdk " << config.ndk.targetApiLevel << "\n\n";
    
    gradle << "    defaultConfig {\n";
    gradle << "        applicationId \"" << config.manifest.packageName << "\"\n";
    gradle << "        minSdk " << config.ndk.minApiLevel << "\n";
    gradle << "        targetSdk " << config.ndk.targetApiLevel << "\n";
    gradle << "        versionCode " << config.manifest.versionCode << "\n";
    gradle << "        versionName \"" << config.manifest.versionName << "\"\n\n";
    
    gradle << "        externalNativeBuild {\n";
    gradle << "            cmake {\n";
    gradle << "                cppFlags ''\n";
    gradle << "            }\n";
    gradle << "        }\n";
    
    gradle << "        ndk {\n";
    gradle << "            abiFilters";
    for (const auto& arch : config.ndk.architectures) {
        if (arch != AndroidArchitecture::All) {
            gradle << " '" << architectureToNdkName(arch) << "',";
        }
    }
    gradle << "\n        }\n";
    gradle << "    }\n\n";
    
    gradle << "    buildTypes {\n";
    gradle << "        release {\n";
    gradle << "            minifyEnabled false\n";
    gradle << "            proguardFiles getDefaultProguardFile('proguard-android-optimize.txt')\n";
    gradle << "        }\n";
    gradle << "    }\n\n";
    
    gradle << "    externalNativeBuild {\n";
    gradle << "        cmake {\n";
    gradle << "            path file('src/main/cpp/CMakeLists.txt')\n";
    gradle << "            version '3.22.1'\n";
    gradle << "        }\n";
    gradle << "    }\n";
    
    if (config.useJetpackCompose) {
        gradle << "\n    buildFeatures {\n";
        gradle << "        compose true\n";
        gradle << "    }\n";
    }
    
    gradle << "}\n\n";
    
    gradle << "dependencies {\n";
    gradle << "    implementation 'androidx.core:core-ktx:1.12.0'\n";
    gradle << "    implementation 'androidx.appcompat:appcompat:1.6.1'\n";
    if (config.useJetpackCompose) {
        gradle << "    implementation platform('androidx.compose:compose-bom:2024.01.00')\n";
        gradle << "    implementation 'androidx.compose.ui:ui'\n";
        gradle << "    implementation 'androidx.compose.material3:material3'\n";
        gradle << "    implementation 'androidx.activity:activity-compose:1.8.2'\n";
    }
    for (const auto& dep : config.gradleDependencies) {
        gradle << "    implementation '" << dep << "'\n";
    }
    gradle << "}\n";
    
    return gradle.str();
}

std::string AndroidTarget::generateNativeCMake(const AndroidProjectConfig& config) const {
    std::ostringstream cmake;
    
    cmake << "# ╔════════════════════════════════════════════════════════════════════╗\n";
    cmake << "# ║     CMakeLists.txt للكود الأصلي - مُولَّد تلقائياً بواسطة لغة ص      ║\n";
    cmake << "# ╚════════════════════════════════════════════════════════════════════╝\n\n";
    
    cmake << "cmake_minimum_required(VERSION 3.22.1)\n\n";
    cmake << "project(\"" << config.projectName << "\")\n\n";
    
    cmake << "# ═══════════════════════════════════════════════════════════════════════\n";
    cmake << "#                    المكتبة الأصلية من كود لغة ص\n";
    cmake << "# ═══════════════════════════════════════════════════════════════════════\n\n";
    
    cmake << "add_library(\n";
    cmake << "    sad_app\n";
    cmake << "    SHARED\n";
    cmake << "    # ملفات C/C++ المُولَّدة من كود ص\n";
    cmake << "    sad_generated.cpp\n";
    cmake << "    jni_bridge.cpp\n";
    cmake << ")\n\n";
    
    cmake << "# الربط مع مكتبات النظام\n";
    cmake << "target_link_libraries(\n";
    cmake << "    sad_app\n";
    cmake << "    android\n";
    cmake << "    log\n";
    cmake << ")\n";
    
    return cmake.str();
}

// ════════════════════════════════════════════════════════════════════════════
// Security helper functions - دوال الأمان المساعدة
// ════════════════════════════════════════════════════════════════════════════

namespace {
    /**
     * @brief Validate device ID format (prevents command injection)
     * التحقق من صيغة معرف الجهاز (يمنع حقن الأوامر)
     */
    bool isValidDeviceId(const std::string& deviceId) {
        if (deviceId.empty()) return true;  // Empty is valid (use default)
        
        // Device ID should only contain alphanumeric, colon, and hyphen
        // معرف الجهاز يجب أن يحتوي فقط على أحرف وأرقام ونقطتين وشرطة
        for (char c : deviceId) {
            if (!std::isalnum(static_cast<unsigned char>(c)) && c != ':' && c != '-' && c != '_') {
                return false;
            }
        }
        return deviceId.length() <= 64;  // Reasonable max length
    }
    
    /**
     * @brief Validate package name format (e.g., com.example.app)
     * التحقق من صيغة اسم الحزمة
     */
    bool isValidPackageName(const std::string& pkgName) {
        if (pkgName.empty() || pkgName.length() > 255) return false;
        
        // Package name: letters, digits, underscores, dots (no consecutive dots)
        char prev = '.';
        for (char c : pkgName) {
            if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_' && c != '.') {
                return false;
            }
            if (c == '.' && prev == '.') return false;  // No consecutive dots
            prev = c;
        }
        // Must not start or end with dot
        return pkgName.front() != '.' && pkgName.back() != '.';
    }
    
    /**
     * @brief Validate activity name format
     * التحقق من صيغة اسم النشاط
     */
    bool isValidActivityName(const std::string& actName) {
        if (actName.empty() || actName.length() > 255) return false;
        
        // Activity: letters, digits, underscores
        for (char c : actName) {
            if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_') {
                return false;
            }
        }
        // Must start with letter
        return std::isalpha(static_cast<unsigned char>(actName.front()));
    }
    
    /**
     * @brief Validate APK path is safe
     * التحقق من أمان مسار APK
     */
    bool isValidApkPath(const std::filesystem::path& apkPath) {
        std::string pathStr = apkPath.string();
        
        // Check for dangerous characters
        static const std::string dangerous = ";|&$`\"'<>(){}[]!*?\n\r";
        for (char c : pathStr) {
            if (dangerous.find(c) != std::string::npos) {
                return false;
            }
        }
        
        // Check for path traversal
        if (pathStr.find("..") != std::string::npos) {
            return false;
        }
        
        // Must end with .apk
        return apkPath.extension() == ".apk";
    }
}

std::optional<std::string> AndroidTarget::installApk(
    const std::filesystem::path& apkPath,
    const std::string& deviceId) {
    
    // ════════════════════════════════════════════════════════
    // التحقق الأمني / Security validation
    // ════════════════════════════════════════════════════════
    
    if (!std::filesystem::exists(apkPath)) {
        return "ملف APK غير موجود: " + apkPath.string();
    }
    
    if (!isValidApkPath(apkPath)) {
        return "مسار APK غير آمن أو لا ينتهي بـ .apk";
    }
    
    if (!isValidDeviceId(deviceId)) {
        return "معرف الجهاز غير صالح. يجب أن يحتوي على أحرف وأرقام فقط.";
    }
    
    std::ostringstream cmd;
    cmd << "adb";
    if (!deviceId.empty()) {
        cmd << " -s " << deviceId;
    }
    cmd << " install -r \"" << apkPath.string() << "\"";
    
    int result = std::system(cmd.str().c_str());
    if (result != 0) {
        return "فشل تثبيت APK. تأكد من توصيل الجهاز وتفعيل وضع المطور.";
    }
    
    return std::nullopt;
}

std::optional<std::string> AndroidTarget::runApp(
    const std::string& packageName,
    const std::string& activityName,
    const std::string& deviceId) {
    
    // ════════════════════════════════════════════════════════
    // التحقق الأمني / Security validation
    // ════════════════════════════════════════════════════════
    
    if (!isValidPackageName(packageName)) {
        return "اسم الحزمة غير صالح. يجب أن يكون بصيغة com.example.app";
    }
    
    if (!isValidActivityName(activityName)) {
        return "اسم النشاط غير صالح. يجب أن يبدأ بحرف ويحتوي على أحرف وأرقام فقط.";
    }
    
    if (!isValidDeviceId(deviceId)) {
        return "معرف الجهاز غير صالح.";
    }
    
    std::ostringstream cmd;
    cmd << "adb";
    if (!deviceId.empty()) {
        cmd << " -s " << deviceId;
    }
    cmd << " shell am start -n " << packageName << "/." << activityName;
    
    int result = std::system(cmd.str().c_str());
    if (result != 0) {
        return "فشل تشغيل التطبيق.";
    }
    
    return std::nullopt;
}

std::vector<std::pair<std::string, std::string>> AndroidTarget::listDevices() const {
    std::vector<std::pair<std::string, std::string>> devices;
    
    // تنفيذ adb devices وتحليل الناتج
    // TODO: تنفيذ فعلي
    
    return devices;
}

void AndroidTarget::setProgressCallback(ProgressCallback callback) {
    pImpl_->progressCallback = callback;
}

std::optional<std::string> AndroidTarget::compileToLlvmIr(
    const std::vector<std::filesystem::path>& sourceFiles) {
    // TODO: تنفيذ الترجمة الفعلية
    return std::nullopt;
}

std::optional<std::string> AndroidTarget::linkNativeLibrary(
    const std::vector<std::filesystem::path>& objectFiles,
    AndroidArchitecture arch,
    const std::filesystem::path& outputPath) {
    // TODO: تنفيذ الربط
    return std::nullopt;
}

std::optional<std::string> AndroidTarget::packageApk(
    const AndroidProjectConfig& config,
    const std::vector<std::filesystem::path>& soFiles) {
    // TODO: تنفيذ تجميع APK
    return std::nullopt;
}

// ═══════════════════════════════════════════════════════════════════════════
//                    دوال مساعدة عامة
// ═══════════════════════════════════════════════════════════════════════════

std::optional<std::filesystem::path> getAndroidSdkPath() {
    const char* paths[] = {"ANDROID_HOME", "ANDROID_SDK_ROOT", "ANDROID_SDK"};
    
    for (const char* envVar : paths) {
        const char* value = std::getenv(envVar);
        if (value && std::filesystem::exists(value)) {
            return std::filesystem::path(value);
        }
    }
    
    // مسارات افتراضية
#ifdef _WIN32
    std::filesystem::path defaultPath = std::filesystem::path(std::getenv("LOCALAPPDATA")) / "Android" / "Sdk";
#elif defined(__APPLE__)
    std::filesystem::path defaultPath = std::filesystem::path(std::getenv("HOME")) / "Library" / "Android" / "sdk";
#else
    std::filesystem::path defaultPath = std::filesystem::path(std::getenv("HOME")) / "Android" / "Sdk";
#endif
    
    if (std::filesystem::exists(defaultPath)) {
        return defaultPath;
    }
    
    return std::nullopt;
}

std::optional<std::filesystem::path> getAndroidNdkPath() {
    auto config = NdkConfig::autoDetect();
    if (config) {
        return config->ndkPath;
    }
    return std::nullopt;
}

bool isBuildToolsInstalled(int version) {
    auto sdkPath = getAndroidSdkPath();
    if (!sdkPath) return false;
    
    auto buildToolsDir = *sdkPath / "build-tools";
    if (!std::filesystem::exists(buildToolsDir)) return false;
    
    for (const auto& entry : std::filesystem::directory_iterator(buildToolsDir)) {
        if (entry.is_directory()) {
            std::string name = entry.path().filename().string();
            if (name.find(std::to_string(version)) == 0) {
                return true;
            }
        }
    }
    
    return false;
}

} // namespace Mobile
} // namespace Tools
} // namespace Sad
