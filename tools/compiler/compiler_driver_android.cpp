// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════
// compiler_driver_android.cpp - دعم بناء أندرويد في sadc
// Android Build Support for sadc
// ═══════════════════════════════════════════════════════════════════════════
//
// الوصف (عربي):
// --------------
// هذا الملف يوفر الدعم لأمر:
//   sadc build android <ملف.ص>
//
// يقوم بـ:
// 1. ترجمة كود ص إلى LLVM IR مع target=android
// 2. توليد AndroidManifest.xml من القالب
// 3. إنشاء هيكل مشروع Android
// 4. تجميع إلى مكتبة أصلية (.so)
// 5. بناء APK
//
// Description (English):
// ----------------------
// This file provides support for the command:
//   sadc build android <file.sad>
//
// It performs:
// 1. Compile Sad code to LLVM IR with target=android
// 2. Generate AndroidManifest.xml from template  
// 3. Create Android project structure
// 4. Compile to native library (.so)
// 5. Build APK
//
// ═══════════════════════════════════════════════════════════════════════════

#include "compiler_driver.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

namespace sad {
namespace driver {

// ============================================================================
// Android Build Configuration / إعدادات بناء أندرويد
// ============================================================================

struct AndroidBuildConfig {
    std::string appName = u8"تطبيق_ص";                    // اسم التطبيق
    std::string packageName = "com.sadlang.app";          // اسم الحزمة
    std::string versionName = "1.0.0";                    // رقم الإصدار
    int versionCode = 1;                                  // كود الإصدار
    int minSdkVersion = 26;                               // Android 8.0
    int targetSdkVersion = 34;                            // Android 14
    std::string orientation = "portrait";                 // اتجاه الشاشة
    bool debugBuild = true;                               // بناء تطوير
    std::string ndkPath;                                  // مسار NDK
    std::string sdkPath;                                  // مسار SDK
    std::string outputDir = "build_android";              // مجلد الإخراج
    std::vector<std::string> sourceFiles;                 // ملفات المصدر
    std::string iconPath;                                 // مسار الأيقونة
    
    // (AR) قراءة من سطر الأوامر
    static AndroidBuildConfig fromCommandLine(int argc, char* argv[], int startIndex);
};

// ============================================================================
// NDK Detection / اكتشاف NDK
// ============================================================================

std::string findAndroidNdk() {
    // 1. متغير البيئة ANDROID_NDK_HOME
    if (const char* ndk = std::getenv("ANDROID_NDK_HOME")) {
        if (fs::exists(ndk)) return ndk;
    }
    
    // 2. متغير البيئة ANDROID_NDK
    if (const char* ndk = std::getenv("ANDROID_NDK")) {
        if (fs::exists(ndk)) return ndk;
    }
    
    // 3. البحث في Android SDK
    if (const char* sdk = std::getenv("ANDROID_SDK_ROOT")) {
        fs::path ndkBundle = fs::path(sdk) / "ndk-bundle";
        if (fs::exists(ndkBundle)) return ndkBundle.string();
        
        // البحث في مجلد ndk لأحدث إصدار
        fs::path ndkDir = fs::path(sdk) / "ndk";
        if (fs::exists(ndkDir)) {
            std::string latestNdk;
            for (const auto& entry : fs::directory_iterator(ndkDir)) {
                if (entry.is_directory()) {
                    latestNdk = entry.path().string();
                }
            }
            if (!latestNdk.empty()) return latestNdk;
        }
    }
    
    // 4. المسارات الشائعة على Windows
    #ifdef _WIN32
    std::vector<std::string> commonPaths = {
        "C:\\Android\\ndk",
        "C:\\android-ndk",
    };
    
    // LOCALAPPDATA
    if (const char* local = std::getenv("LOCALAPPDATA")) {
        commonPaths.push_back(std::string(local) + "\\Android\\Sdk\\ndk");
    }
    
    for (const auto& path : commonPaths) {
        if (fs::exists(path)) {
            for (const auto& entry : fs::directory_iterator(path)) {
                if (entry.is_directory()) return entry.path().string();
            }
        }
    }
    #else
    // Linux/macOS paths
    std::vector<std::string> commonPaths = {
        "/opt/android-ndk",
        "/usr/local/android-ndk",
    };
    
    if (const char* home = std::getenv("HOME")) {
        commonPaths.push_back(std::string(home) + "/Android/Sdk/ndk");
        commonPaths.push_back(std::string(home) + "/android-ndk");
    }
    
    for (const auto& path : commonPaths) {
        if (fs::exists(path)) {
            for (const auto& entry : fs::directory_iterator(path)) {
                if (entry.is_directory()) return entry.path().string();
            }
        }
    }
    #endif
    
    return "";
}

// ============================================================================
// Template Generation / توليد القوالب
// ============================================================================

std::string generateAndroidManifest(const AndroidBuildConfig& config) {
    std::ostringstream ss;
    ss << R"(<?xml version="1.0" encoding="utf-8"?>
<!-- تم إنشاؤه تلقائياً بواسطة مترجم ص / Generated by Sad Compiler -->
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package=")" << config.packageName << R"("
    android:versionCode=")" << config.versionCode << R"("
    android:versionName=")" << config.versionName << R"(">

    <uses-sdk
        android:minSdkVersion=")" << config.minSdkVersion << R"("
        android:targetSdkVersion=")" << config.targetSdkVersion << R"(" />

    <!-- الصلاحيات / Permissions -->
    <uses-permission android:name="android.permission.INTERNET" />
    <uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />

    <!-- دعم OpenGL ES 3.0 -->
    <uses-feature android:glEsVersion="0x00030000" android:required="true" />

    <application
        android:label=")" << config.appName << R"("
        android:icon="@mipmap/ic_launcher"
        android:hasCode="false"
        android:theme="@android:style/Theme.NoTitleBar.Fullscreen">

        <activity
            android:name="android.app.NativeActivity"
            android:exported="true"
            android:configChanges="orientation|screenSize|keyboardHidden"
            android:screenOrientation=")" << config.orientation << R"(">

            <meta-data
                android:name="android.app.lib_name"
                android:value="sad_app" />

            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
        </activity>
    </application>
</manifest>
)";
    return ss.str();
}

std::string generateBuildGradle(const AndroidBuildConfig& config) {
    std::ostringstream ss;
    ss << R"(// تم إنشاؤه تلقائياً بواسطة مترجم ص / Generated by Sad Compiler
plugins {
    id 'com.android.application'
}

android {
    namespace ')" << config.packageName << R"('
    compileSdk )" << config.targetSdkVersion << R"(

    defaultConfig {
        applicationId ')" << config.packageName << R"('
        minSdk )" << config.minSdkVersion << R"(
        targetSdk )" << config.targetSdkVersion << R"(
        versionCode )" << config.versionCode << R"(
        versionName ')" << config.versionName << R"('

        ndk {
            abiFilters 'arm64-v8a'
        }
        
        externalNativeBuild {
            cmake {
                cppFlags '-std=c++17 -fexceptions -frtti'
                arguments '-DANDROID_STL=c++_shared'
            }
        }
    }

    buildTypes {
        release {
            minifyEnabled false
        }
        debug {
            debuggable true
            jniDebuggable true
        }
    }

    externalNativeBuild {
        cmake {
            path file('CMakeLists.txt')
            version '3.22.1'
        }
    }
}
)";
    return ss.str();
}

std::string generateNativeCMake(const AndroidBuildConfig& config) {
    std::ostringstream ss;
    ss << R"(# تم إنشاؤه تلقائياً بواسطة مترجم ص / Generated by Sad Compiler
cmake_minimum_required(VERSION 3.22.1)
project(")" << config.packageName << R"(" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# البحث عن مكتبات Android
find_library(log-lib log)
find_library(android-lib android)
find_library(EGL-lib EGL)
find_library(GLESv3-lib GLESv3)

# المكتبة الأصلية
add_library(sad_app SHARED
    src/main/cpp/sad_generated.cpp
    src/main/cpp/android_main.cpp
)

target_include_directories(sad_app PRIVATE
    ${ANDROID_NDK}/sources/android/native_app_glue
    src/main/cpp/include
)

target_compile_definitions(sad_app PRIVATE
    SAD_ANDROID=1
    SAD_PLATFORM_ANDROID=1
)

# Native App Glue
add_library(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)

target_include_directories(native_app_glue PUBLIC
    ${ANDROID_NDK}/sources/android/native_app_glue
)

target_link_libraries(sad_app
    ${log-lib}
    ${android-lib}
    ${EGL-lib}
    ${GLESv3-lib}
    native_app_glue
)
)";
    return ss.str();
}

// ============================================================================
// Build Android App / بناء تطبيق أندرويد
// ============================================================================

int buildAndroidApp(const AndroidBuildConfig& config, DiagnosticEngine& diag) {
    std::cout << u8"╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << u8"║           بناء تطبيق أندرويد بلغة ص                            ║\n";
    std::cout << u8"║           Sad Android App Builder                              ║\n";
    std::cout << u8"╚═══════════════════════════════════════════════════════════════╝\n\n";
    
    // 1. التحقق من ملفات المصدر
    if (config.sourceFiles.empty()) {
        diag.report_error(u8"لا توجد ملفات مصدر / No source files specified");
        return 1;
    }
    
    std::cout << u8"[1/5] التحقق من الملفات...\n";
    for (const auto& file : config.sourceFiles) {
        // Convert to filesystem path with UTF-8 support
        fs::path filePath = fs::u8path(file);
        
        std::error_code ec;
        bool exists = fs::exists(filePath, ec);
        
        if (!exists) {
            diag.report_error(u8"الملف غير موجود: " + file);
            return 1;
        }
        std::cout << u8"  ✓ " << file << "\n";
    }
    
    // 2. اكتشاف NDK
    std::cout << u8"\n[2/5] اكتشاف Android NDK...\n";
    std::string ndkPath = config.ndkPath;
    if (ndkPath.empty()) {
        ndkPath = findAndroidNdk();
    }
    
    bool hasNdk = !ndkPath.empty();
    if (hasNdk) {
        std::cout << u8"  ✓ NDK: " << ndkPath << "\n";
    } else {
        std::cout << u8"  ⚠ لم يتم العثور على Android NDK\n";
        std::cout << u8"    يمكنك تثبيته لاحقاً وتعيين ANDROID_NDK_HOME\n";
        std::cout << u8"    سيتم إنشاء هيكل المشروع بدون تجميع...\n";
    }
    
    // 3. إنشاء هيكل المشروع
    std::cout << u8"\n[3/5] إنشاء هيكل المشروع...\n";
    
    fs::path outputDir = config.outputDir;
    fs::create_directories(outputDir);
    fs::create_directories(outputDir / "src" / "main" / "cpp");
    fs::create_directories(outputDir / "src" / "main" / "cpp" / "include");
    fs::create_directories(outputDir / "src" / "main" / "res" / "mipmap-hdpi");
    fs::create_directories(outputDir / "libs" / "arm64-v8a");
    
    std::cout << u8"  ✓ " << outputDir.string() << "/\n";
    std::cout << u8"      ├─ src/main/cpp/\n";
    std::cout << u8"      ├─ src/main/res/\n";
    std::cout << u8"      └─ libs/arm64-v8a/\n";
    
    // 4. توليد ملفات المشروع
    std::cout << u8"\n[4/5] توليد ملفات المشروع...\n";
    
    // AndroidManifest.xml
    {
        std::ofstream file(outputDir / "src" / "main" / "AndroidManifest.xml");
        file << generateAndroidManifest(config);
        std::cout << u8"  ✓ AndroidManifest.xml\n";
    }
    
    // build.gradle
    {
        std::ofstream file(outputDir / "build.gradle");
        file << generateBuildGradle(config);
        std::cout << u8"  ✓ build.gradle\n";
    }
    
    // CMakeLists.txt
    {
        std::ofstream file(outputDir / "CMakeLists.txt");
        file << generateNativeCMake(config);
        std::cout << u8"  ✓ CMakeLists.txt\n";
    }
    
    // settings.gradle
    {
        std::ofstream file(outputDir / "settings.gradle");
        file << "rootProject.name = '" << config.appName << "'\n";
        file << "include ':app'\n";
        std::cout << u8"  ✓ settings.gradle\n";
    }
    
    // 5. تعليمات البناء
    std::cout << u8"\n[5/5] اكتمل إعداد المشروع!\n";
    std::cout << u8"\n═══════════════════════════════════════════════════════════════\n";
    std::cout << u8"الخطوات التالية:\n";
    std::cout << u8"\n1. ترجم ملفات ص إلى كود أندرويد:\n";
    std::cout << u8"   sadc --target=aarch64-linux-android26 ";
    for (const auto& f : config.sourceFiles) {
        std::cout << f << " ";
    }
    std::cout << u8"-o " << config.outputDir << "/src/main/cpp/sad_generated.cpp\n";
    
    std::cout << u8"\n2. افتح المشروع في Android Studio:\n";
    std::cout << u8"   File → Open → " << fs::absolute(outputDir).string() << "\n";
    
    std::cout << u8"\n3. أو ابنِ من سطر الأوامر:\n";
    std::cout << u8"   cd " << config.outputDir << "\n";
    std::cout << u8"   ./gradlew assembleDebug\n";
    
    std::cout << u8"\n4. ثبّت على الجهاز:\n";
    std::cout << u8"   adb install -r app/build/outputs/apk/debug/app-debug.apk\n";
    std::cout << u8"═══════════════════════════════════════════════════════════════\n";
    
    return 0;
}

// ============================================================================
// Command Handler / معالج الأمر
// ============================================================================

/**
 * @brief handleBuildAndroidCommand - معالجة أمر build android
 * 
 * @param argc عدد الوسائط
 * @param argv مصفوفة الوسائط
 * @return كود الخروج (0 = نجاح)
 */
int handleBuildAndroidCommand(int argc, char* argv[]) {
    AndroidBuildConfig config;
    DiagnosticEngine diag;
    
    // Parse arguments starting from "build android ..."
    // sadc build android [options] <file.ص>
    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--name" && i + 1 < argc) {
            config.appName = argv[++i];
        }
        else if (arg == "--package" && i + 1 < argc) {
            config.packageName = argv[++i];
        }
        else if (arg == "--version" && i + 1 < argc) {
            config.versionName = argv[++i];
        }
        else if (arg == "--output" || arg == "-o") {
            if (i + 1 < argc) {
                config.outputDir = argv[++i];
            }
        }
        else if (arg == "--ndk" && i + 1 < argc) {
            config.ndkPath = argv[++i];
        }
        else if (arg == "--min-sdk" && i + 1 < argc) {
            config.minSdkVersion = std::stoi(argv[++i]);
        }
        else if (arg == "--release") {
            config.debugBuild = false;
        }
        else if (arg == "--help" || arg == "-h") {
            std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                 أداة بناء أندرويد - لغة ص                        ║
║                 Sad Android Builder                             ║
╚═══════════════════════════════════════════════════════════════╝

الاستخدام / Usage:
  sadc build android [خيارات] <ملف.ص>

الخيارات / Options:
  --name <اسم>       اسم التطبيق (افتراضي: تطبيق_ص)
  --package <اسم>    اسم الحزمة (افتراضي: com.sadlang.app)
  --version <رقم>    رقم الإصدار (افتراضي: 1.0.0)
  -o, --output <مسار>  مجلد الإخراج (افتراضي: build_android)
  --ndk <مسار>       مسار Android NDK
  --min-sdk <رقم>    الحد الأدنى لإصدار Android (افتراضي: 26)
  --release          بناء إصدار للنشر
  --help             عرض هذه المساعدة

أمثلة / Examples:
  sadc build android تطبيقي.ص
  sadc build android --name "الدردشة" --package com.chat.app chat.sad
)";
            return 0;
        }
        else if (!arg.empty() && arg[0] != '-') {
            config.sourceFiles.push_back(arg);
        }
    }
    
    return buildAndroidApp(config, diag);
}

} // namespace driver
} // namespace sad
