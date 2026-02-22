// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file ios_target.cpp
 * @brief تنفيذ هدف iOS - إنشاء تطبيقات iOS من كود لغة ص
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                تنفيذ نظام بناء تطبيقات iOS بلغة ص
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#include "ios_target.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <array>
#include <regex>

namespace Sad {
namespace Tools {
namespace Mobile {

// ═══════════════════════════════════════════════════════════════════════════
//                    تنفيذ XcodeConfig
// ═══════════════════════════════════════════════════════════════════════════

std::optional<XcodeConfig> XcodeConfig::autoDetect() {
#ifndef __APPLE__
    // iOS development requires macOS
    return std::nullopt;
#else
    XcodeConfig config;
    
    // الحصول على مسار Xcode من xcode-select
    FILE* pipe = popen("xcode-select -p", "r");
    if (!pipe) return std::nullopt;
    
    char buffer[256];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe)) {
        result += buffer;
    }
    pclose(pipe);
    
    // إزالة السطر الجديد
    result.erase(result.find_last_not_of("\n\r") + 1);
    
    if (result.empty()) return std::nullopt;
    
    config.developerDir = result;
    
    // استخراج مسار Xcode.app
    auto pos = result.find("/Contents/Developer");
    if (pos != std::string::npos) {
        config.xcodePath = result.substr(0, pos);
    }
    
    // الحصول على إصدار Xcode
    pipe = popen("xcodebuild -version | head -1", "r");
    if (pipe) {
        if (fgets(buffer, sizeof(buffer), pipe)) {
            std::string version = buffer;
            auto vpos = version.find("Xcode ");
            if (vpos != std::string::npos) {
                config.xcodeVersion = version.substr(vpos + 6);
                config.xcodeVersion.erase(config.xcodeVersion.find_last_not_of("\n\r ") + 1);
            }
        }
        pclose(pipe);
    }
    
    // اكتشاف SDKs المثبتة
    pipe = popen("xcodebuild -showsdks | grep -E 'iphoneos|iphonesimulator|watchos|appletvos'", "r");
    if (pipe) {
        while (fgets(buffer, sizeof(buffer), pipe)) {
            config.installedSdks.push_back(buffer);
        }
        pclose(pipe);
    }
    
    return config;
#endif
}

std::filesystem::path XcodeConfig::getSdkPath(iOSPlatform platform) const {
    std::string sdkName;
    
    switch (platform) {
        case iOSPlatform::iOS:
        case iOSPlatform::iPadOS:
            sdkName = "iphoneos";
            break;
        case iOSPlatform::watchOS:
            sdkName = "watchos";
            break;
        case iOSPlatform::tvOS:
            sdkName = "appletvos";
            break;
        case iOSPlatform::macCatalyst:
            sdkName = "macosx";
            break;
        case iOSPlatform::visionOS:
            sdkName = "xros";
            break;
    }
    
    return developerDir / "Platforms" / (sdkName + ".platform") / "Developer" / "SDKs" / (sdkName + ".sdk");
}

std::filesystem::path XcodeConfig::getClangPath() const {
    return developerDir / "Toolchains" / "XcodeDefault.xctoolchain" / "usr" / "bin" / "clang";
}

std::filesystem::path XcodeConfig::getSwiftPath() const {
    return developerDir / "Toolchains" / "XcodeDefault.xctoolchain" / "usr" / "bin" / "swift";
}

// ═══════════════════════════════════════════════════════════════════════════
//                    تنفيذ InfoPlistData
// ═══════════════════════════════════════════════════════════════════════════

std::string InfoPlistData::generatePlist() const {
    std::ostringstream plist;
    
    plist << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    plist << "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n";
    plist << "<plist version=\"1.0\">\n";
    plist << "<dict>\n";
    
    // معلومات أساسية
    plist << "    <key>CFBundleIdentifier</key>\n";
    plist << "    <string>" << bundleIdentifier << "</string>\n";
    
    plist << "    <key>CFBundleName</key>\n";
    plist << "    <string>" << bundleName << "</string>\n";
    
    plist << "    <key>CFBundleDisplayName</key>\n";
    plist << "    <string>" << bundleDisplayName << "</string>\n";
    
    plist << "    <key>CFBundleVersion</key>\n";
    plist << "    <string>" << bundleVersion << "</string>\n";
    
    plist << "    <key>CFBundleShortVersionString</key>\n";
    plist << "    <string>" << bundleShortVersion << "</string>\n";
    
    plist << "    <key>CFBundleExecutable</key>\n";
    plist << "    <string>$(EXECUTABLE_NAME)</string>\n";
    
    plist << "    <key>CFBundlePackageType</key>\n";
    plist << "    <string>APPL</string>\n";
    
    // الحد الأدنى لإصدار iOS
    plist << "    <key>MinimumOSVersion</key>\n";
    plist << "    <string>" << minimumOSVersion << "</string>\n";
    
    // عائلات الأجهزة
    plist << "    <key>UIDeviceFamily</key>\n";
    plist << "    <array>\n";
    for (const auto& family : deviceFamilies) {
        plist << "        <integer>" << family << "</integer>\n";
    }
    plist << "    </array>\n";
    
    // دعم RTL للعربية
    if (supportsRTL) {
        plist << "    <key>CFBundleDevelopmentRegion</key>\n";
        plist << "    <string>ar</string>\n";
        
        plist << "    <key>CFBundleLocalizations</key>\n";
        plist << "    <array>\n";
        plist << "        <string>ar</string>\n";
        plist << "        <string>en</string>\n";
        plist << "    </array>\n";
    }
    
    // Launch Screen
    if (!launchStoryboard.empty()) {
        plist << "    <key>UILaunchStoryboardName</key>\n";
        plist << "    <string>" << launchStoryboard << "</string>\n";
    }
    
    // أذونات مع أوصاف عربية
    if (!cameraUsageDescription.empty()) {
        plist << "    <key>NSCameraUsageDescription</key>\n";
        plist << "    <string>" << cameraUsageDescription << "</string>\n";
    }
    
    if (!locationUsageDescription.empty()) {
        plist << "    <key>NSLocationWhenInUseUsageDescription</key>\n";
        plist << "    <string>" << locationUsageDescription << "</string>\n";
    }
    
    if (!microphoneUsageDescription.empty()) {
        plist << "    <key>NSMicrophoneUsageDescription</key>\n";
        plist << "    <string>" << microphoneUsageDescription << "</string>\n";
    }
    
    if (!photoLibraryUsageDescription.empty()) {
        plist << "    <key>NSPhotoLibraryUsageDescription</key>\n";
        plist << "    <string>" << photoLibraryUsageDescription << "</string>\n";
    }
    
    // مفاتيح إضافية
    for (const auto& [key, value] : customKeys) {
        plist << "    <key>" << key << "</key>\n";
        plist << "    <string>" << value << "</string>\n";
    }
    
    // إعدادات الواجهة
    plist << "    <key>UIRequiresFullScreen</key>\n";
    plist << "    <true/>\n";
    
    plist << "    <key>UISupportedInterfaceOrientations</key>\n";
    plist << "    <array>\n";
    plist << "        <string>UIInterfaceOrientationPortrait</string>\n";
    plist << "        <string>UIInterfaceOrientationLandscapeLeft</string>\n";
    plist << "        <string>UIInterfaceOrientationLandscapeRight</string>\n";
    plist << "    </array>\n";
    
    plist << "</dict>\n";
    plist << "</plist>\n";
    
    return plist.str();
}

// ═══════════════════════════════════════════════════════════════════════════
//                    تنفيذ iOSBuildResult
// ═══════════════════════════════════════════════════════════════════════════

std::string iOSBuildResult::toArabicSummary() const {
    std::ostringstream summary;
    
    if (success) {
        summary << "✅ تم بناء تطبيق iOS بنجاح!\n\n";
        summary << "📱 ملف التطبيق: " << appPath.string() << "\n";
        if (!ipaPath.empty()) {
            summary << "📦 ملف IPA: " << ipaPath.string() << "\n";
        }
        if (!dsymPath.empty()) {
            summary << "🔍 رموز التصحيح: " << dsymPath.string() << "\n";
        }
        summary << "⏱️ وقت البناء: " << buildTime.count() << " مللي ثانية\n";
    } else {
        summary << "❌ فشل بناء تطبيق iOS!\n\n";
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
//                    تنفيذ iOSTarget
// ═══════════════════════════════════════════════════════════════════════════

struct iOSTarget::Impl {
    iOSProjectConfig config;
    XcodeConfig xcodeConfig;
    ProgressCallback progressCallback;
    std::filesystem::path tempDir;
    bool initialized = false;
};

iOSTarget::iOSTarget() : pImpl_(std::make_unique<Impl>()) {}

iOSTarget::~iOSTarget() = default;

std::optional<std::string> iOSTarget::initialize(const iOSProjectConfig& config) {
#ifndef __APPLE__
    return "تطوير iOS يتطلب نظام macOS و Xcode";
#endif
    
    pImpl_->config = config;
    
    // اكتشاف Xcode
    auto xcodeConfig = XcodeConfig::autoDetect();
    if (!xcodeConfig) {
        return "لم يتم العثور على Xcode. يرجى تثبيت Xcode من App Store.";
    }
    pImpl_->xcodeConfig = *xcodeConfig;
    
    // إنشاء مجلد مؤقت
    pImpl_->tempDir = std::filesystem::temp_directory_path() / "sad_ios_build";
    std::filesystem::create_directories(pImpl_->tempDir);
    
    pImpl_->initialized = true;
    return std::nullopt;
}

std::vector<std::string> iOSTarget::checkPrerequisites() const {
    std::vector<std::string> missing;
    
#ifndef __APPLE__
    missing.push_back("macOS: تطوير iOS يتطلب نظام macOS");
    return missing;
#endif
    
    if (!isXcodeInstalled()) {
        missing.push_back("Xcode: غير مثبت. ثبّته من App Store.");
    }
    
    if (!isXcodeLicenseAccepted()) {
        missing.push_back("اتفاقية Xcode: شغّل 'sudo xcodebuild -license accept'");
    }
    
    return missing;
}

iOSBuildResult iOSTarget::build(
    const std::vector<std::filesystem::path>& sourceFiles,
    const iOSProjectConfig& config) {
    
    iOSBuildResult result;
    auto startTime = std::chrono::steady_clock::now();
    
#ifndef __APPLE__
    result.errors.push_back("تطوير iOS يتطلب نظام macOS");
    return result;
#endif
    
    if (!pImpl_->initialized) {
        if (auto error = initialize(config)) {
            result.errors.push_back(*error);
            return result;
        }
    }
    
    if (pImpl_->progressCallback) {
        pImpl_->progressCallback("تحليل كود ص", 10);
    }
    
    // 1. ترجمة كود ص
    if (auto error = compileToLlvmIr(sourceFiles)) {
        result.errors.push_back(*error);
        return result;
    }
    
    if (pImpl_->progressCallback) {
        pImpl_->progressCallback("توليد جسر Swift", 30);
    }
    
    // 2. توليد جسر Swift
    if (auto error = generateSwiftWrapper(config)) {
        result.errors.push_back(*error);
        return result;
    }
    
    if (pImpl_->progressCallback) {
        pImpl_->progressCallback("بناء بـ xcodebuild", 50);
    }
    
    // 3. توليد مشروع Xcode
    auto projectResult = generateXcodeProject(pImpl_->tempDir, config);
    if (std::holds_alternative<std::string>(projectResult)) {
        result.errors.push_back(std::get<std::string>(projectResult));
        return result;
    }
    auto projectPath = std::get<std::filesystem::path>(projectResult);
    
    // 4. بناء باستخدام xcodebuild
    if (auto error = runXcodebuild(projectPath, config)) {
        result.errors.push_back(*error);
        return result;
    }
    
    result.appPath = config.outputDir / (config.projectName + ".app");
    result.success = true;
    
    auto endTime = std::chrono::steady_clock::now();
    result.buildTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    if (pImpl_->progressCallback) {
        pImpl_->progressCallback("اكتمل البناء", 100);
    }
    
    return result;
}

iOSBuildResult iOSTarget::buildForSimulator(
    const std::vector<std::filesystem::path>& sourceFiles,
    const iOSProjectConfig& config) {
    
    iOSProjectConfig simConfig = config;
    simConfig.architectures = {iOSArchitecture::X86_64, iOSArchitecture::ARM64};
    
    return build(sourceFiles, simConfig);
}

std::variant<std::filesystem::path, std::string> iOSTarget::generateXcodeProject(
    const std::filesystem::path& outputDir,
    const iOSProjectConfig& config) {
    
    auto projectDir = outputDir / (config.projectName + ".xcodeproj");
    std::filesystem::create_directories(projectDir);
    
    auto srcDir = outputDir / config.projectName;
    std::filesystem::create_directories(srcDir);
    
    // إنشاء Info.plist
    {
        std::ofstream plist(srcDir / "Info.plist");
        plist << config.infoPlist.generatePlist();
    }
    
    // إنشاء ملف Swift الرئيسي
    {
        std::ofstream appFile(srcDir / (config.projectName + "App.swift"));
        appFile << generateSwiftBridge(config);
    }
    
    // إنشاء project.pbxproj
    // TODO: توليد ملف المشروع الفعلي
    {
        std::ofstream pbxproj(projectDir / "project.pbxproj");
        pbxproj << "// Project file generated by Sad Language Compiler\n";
        // TODO: توليد محتوى pbxproj الفعلي
    }
    
    return outputDir;
}

std::string iOSTarget::generateSwiftBridge(const iOSProjectConfig& config) const {
    std::ostringstream swift;
    
    swift << "// ╔════════════════════════════════════════════════════════════════════╗\n";
    swift << "// ║    " << config.projectName << " - مُولَّد تلقائياً بواسطة لغة ص             ║\n";
    swift << "// ╚════════════════════════════════════════════════════════════════════╝\n\n";
    
    swift << "import SwiftUI\n\n";
    
    swift << "// MARK: - جسر C للكود الأصلي من لغة ص\n";
    swift << "#if canImport(SadRuntime)\n";
    swift << "import SadRuntime\n";
    swift << "#endif\n\n";
    
    swift << "@main\n";
    swift << "struct " << config.projectName << "App: App {\n";
    swift << "    init() {\n";
    swift << "        // تهيئة runtime لغة ص\n";
    swift << "        // sad_runtime_init()\n";
    swift << "    }\n\n";
    
    swift << "    var body: some Scene {\n";
    swift << "        WindowGroup {\n";
    swift << "            ContentView()\n";
    swift << "                .environment(\\.layoutDirection, .rightToLeft)\n";
    swift << "        }\n";
    swift << "    }\n";
    swift << "}\n\n";
    
    swift << "// MARK: - الواجهة الرئيسية\n";
    swift << "struct ContentView: View {\n";
    swift << "    var body: some View {\n";
    swift << "        NavigationView {\n";
    swift << "            VStack {\n";
    swift << "                Text(\"مرحباً من لغة ص!\")\n";
    swift << "                    .font(.largeTitle)\n";
    swift << "                    .padding()\n";
    swift << "            }\n";
    swift << "            .navigationTitle(\"" << config.infoPlist.bundleDisplayName << "\")\n";
    swift << "        }\n";
    swift << "    }\n";
    swift << "}\n";
    
    return swift.str();
}

std::string iOSTarget::generateSwiftUIView(const std::string& sadUICode) const {
    // TODO: تحليل كود UI بلغة ص وتوليد SwiftUI
    std::ostringstream swift;
    
    swift << "struct SadGeneratedView: View {\n";
    swift << "    var body: some View {\n";
    swift << "        // TODO: View generated from Sad code\n";
    swift << "        Text(\"Generated View\")\n";
    swift << "    }\n";
    swift << "}\n";
    
    return swift.str();
}

std::vector<std::pair<std::string, std::string>> iOSTarget::listSimulators() const {
    std::vector<std::pair<std::string, std::string>> simulators;
    
#ifdef __APPLE__
    FILE* pipe = popen("xcrun simctl list devices -j", "r");
    if (pipe) {
        // TODO: تحليل JSON
        pclose(pipe);
    }
#endif
    
    return simulators;
}

std::optional<std::string> iOSTarget::bootSimulator(const std::string& simulatorId) {
#ifndef __APPLE__
    return "المحاكيات متاحة فقط على macOS";
#endif
    
    // التحقق من صحة معرف المحاكي (UUID format)
    // UUID: 8-4-4-4-12 hex chars
    bool validId = true;
    for (char c : simulatorId) {
        if (!std::isxdigit(static_cast<unsigned char>(c)) && c != '-') {
            validId = false;
            break;
        }
    }
    
    if (!validId || simulatorId.length() < 32) {
        return "معرف المحاكي غير صالح";
    }
    
    std::string cmd = "xcrun simctl boot " + simulatorId;
    int result = std::system(cmd.c_str());
    
    if (result != 0) {
        return "فشل تشغيل المحاكي";
    }
    
    return std::nullopt;
}

std::optional<std::string> iOSTarget::installOnSimulator(
    const std::filesystem::path& appPath,
    const std::string& simulatorId) {
    
#ifndef __APPLE__
    return "التثبيت على المحاكي متاح فقط على macOS";
#endif
    
    // التحقق من صحة معرف المحاكي
    for (char c : simulatorId) {
        if (!std::isxdigit(static_cast<unsigned char>(c)) && c != '-') {
            return "معرف المحاكي غير صالح";
        }
    }
    
    // التحقق من وجود المسار
    if (!std::filesystem::exists(appPath)) {
        return "مسار التطبيق غير موجود";
    }
    
    // التحقق من أن المسار هو .app
    if (appPath.extension() != ".app") {
        return "الملف ليس تطبيق iOS (.app)";
    }
    
    // تنظيف المسار من الأحرف الخطرة
    std::string safePath = appPath.string();
    const std::string dangerous = ";|&`$(){}[]<>!";
    for (char c : dangerous) {
        if (safePath.find(c) != std::string::npos) {
            return "مسار التطبيق يحتوي على أحرف غير آمنة";
        }
    }
    
    std::string cmd = "xcrun simctl install " + simulatorId + " \"" + safePath + "\"";
    int result = std::system(cmd.c_str());
    
    if (result != 0) {
        return "فشل تثبيت التطبيق على المحاكي";
    }
    
    return std::nullopt;
}

void iOSTarget::setProgressCallback(ProgressCallback callback) {
    pImpl_->progressCallback = callback;
}

std::optional<std::string> iOSTarget::compileToLlvmIr(
    const std::vector<std::filesystem::path>& sourceFiles) {
    // TODO: تنفيذ الترجمة
    return std::nullopt;
}

std::optional<std::string> iOSTarget::generateSwiftWrapper(
    const iOSProjectConfig& config) {
    // TODO: توليد wrapper
    return std::nullopt;
}

std::optional<std::string> iOSTarget::runXcodebuild(
    const std::filesystem::path& projectPath,
    const iOSProjectConfig& config) {
    // TODO: تنفيذ xcodebuild
    return std::nullopt;
}

// ═══════════════════════════════════════════════════════════════════════════
//                    دوال مساعدة
// ═══════════════════════════════════════════════════════════════════════════

bool isXcodeInstalled() {
#ifndef __APPLE__
    return false;
#else
    return std::system("xcode-select -p > /dev/null 2>&1") == 0;
#endif
}

std::optional<std::string> getXcodeVersion() {
#ifndef __APPLE__
    return std::nullopt;
#else
    FILE* pipe = popen("xcodebuild -version | head -1 | cut -d' ' -f2", "r");
    if (!pipe) return std::nullopt;
    
    char buffer[64];
    std::string result;
    if (fgets(buffer, sizeof(buffer), pipe)) {
        result = buffer;
        result.erase(result.find_last_not_of("\n\r") + 1);
    }
    pclose(pipe);
    
    return result.empty() ? std::nullopt : std::make_optional(result);
#endif
}

bool isXcodeLicenseAccepted() {
#ifndef __APPLE__
    return false;
#else
    return std::system("xcodebuild -checkFirstLaunchStatus > /dev/null 2>&1") == 0;
#endif
}

} // namespace Mobile
} // namespace Tools
} // namespace Sad
