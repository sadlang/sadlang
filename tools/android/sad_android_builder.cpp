// ============================================================================
// sad_android_builder.cpp — أداة بناء APK / APK Builder Tool
// ============================================================================
// @brief (AR) أداة لبناء مشاريع Android من ملفات ص (.ص)
//   - اكتشاف Android SDK/NDK تلقائياً
//   - بناء عبر Gradle (assembleDebug/Release)
//   - توقيع APK (zipalign + apksigner)
//   - نشر على الجهاز عبر ADB
//
// @brief (EN) Android APK builder tool for Sad language projects
//   - Auto-detect Android SDK/NDK
//   - Build via Gradle (assembleDebug/Release)
//   - Sign APK (zipalign + apksigner)
//   - Deploy to device via ADB
//
// الاستخدام / Usage:
//   sad_android_builder build [--release] [--project-dir <path>]
//   sad_android_builder sign <apk-path> --keystore <ks> --alias <alias>
//   sad_android_builder install <apk-path>
//   sad_android_builder run --package <pkg> --activity <act>
//   sad_android_builder info
// ============================================================================

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace fs = std::filesystem;

// ============================================================================
// اكتشاف البيئة / Environment Detection
// ============================================================================

struct AndroidEnvironment {
    std::string sdkPath;
    std::string ndkPath;
    std::string buildToolsPath;
    std::string platformPath;
    bool valid = false;

    std::string getZipalign() const {
        if (buildToolsPath.empty()) return "";
#ifdef _WIN32
        return buildToolsPath + "\\zipalign.exe";
#else
        return buildToolsPath + "/zipalign";
#endif
    }

    std::string getApksigner() const {
        if (buildToolsPath.empty()) return "";
#ifdef _WIN32
        return buildToolsPath + "\\apksigner.bat";
#else
        return buildToolsPath + "/apksigner";
#endif
    }

    std::string getAdb() const {
        if (sdkPath.empty()) return "adb";
#ifdef _WIN32
        return sdkPath + "\\platform-tools\\adb.exe";
#else
        return sdkPath + "/platform-tools/adb";
#endif
    }
};

// (AR) اكتشاف Android SDK تلقائياً
// (EN) Auto-detect Android SDK
AndroidEnvironment detectEnvironment() {
    AndroidEnvironment env;

    // ANDROID_HOME أو ANDROID_SDK_ROOT
    const char* sdkEnv = std::getenv("ANDROID_HOME");
    if (!sdkEnv) sdkEnv = std::getenv("ANDROID_SDK_ROOT");

    if (sdkEnv && fs::exists(sdkEnv)) {
        env.sdkPath = sdkEnv;
    } else {
        // مسارات افتراضية
#ifdef _WIN32
        std::string userProfile = std::getenv("USERPROFILE") ? std::getenv("USERPROFILE") : "";
        std::vector<std::string> candidates = {
            userProfile + "\\AppData\\Local\\Android\\Sdk",
            "C:\\Android\\Sdk",
            "C:\\Users\\Public\\Android\\Sdk"
        };
#else
        std::string home = std::getenv("HOME") ? std::getenv("HOME") : "";
        std::vector<std::string> candidates = {
            home + "/Android/Sdk",
            home + "/Library/Android/sdk",
            "/usr/local/android-sdk"
        };
#endif
        for (const auto& p : candidates) {
            if (fs::exists(p)) {
                env.sdkPath = p;
                break;
            }
        }
    }

    if (env.sdkPath.empty()) {
        return env;
    }

    // NDK
    const char* ndkEnv = std::getenv("ANDROID_NDK");
    if (ndkEnv && fs::exists(ndkEnv)) {
        env.ndkPath = ndkEnv;
    } else {
        fs::path ndkDir = fs::path(env.sdkPath) / "ndk";
        if (fs::exists(ndkDir)) {
            // أحدث إصدار
            std::string latest;
            for (auto& entry : fs::directory_iterator(ndkDir)) {
                if (entry.is_directory()) {
                    std::string name = entry.path().filename().string();
                    if (latest.empty() || name > latest) {
                        latest = name;
                    }
                }
            }
            if (!latest.empty()) {
                env.ndkPath = (ndkDir / latest).string();
            }
        }
    }

    // Build Tools (أحدث إصدار)
    fs::path btDir = fs::path(env.sdkPath) / "build-tools";
    if (fs::exists(btDir)) {
        std::string latest;
        for (auto& entry : fs::directory_iterator(btDir)) {
            if (entry.is_directory()) {
                std::string name = entry.path().filename().string();
                if (latest.empty() || name > latest) {
                    latest = name;
                }
            }
        }
        if (!latest.empty()) {
            env.buildToolsPath = (btDir / latest).string();
        }
    }

    env.valid = true;
    return env;
}

// ============================================================================
// تنفيذ الأوامر / Command Execution
// ============================================================================

// (AR) تنفيذ أمر خارجي وإرجاع كود الخروج
int runCommand(const std::string& cmd) {
    std::cout << "[بناء] تنفيذ: " << cmd << std::endl;
    return std::system(cmd.c_str());
}

// ============================================================================
// أمر: build — بناء المشروع / Build Project  
// ============================================================================
int cmdBuild(const std::string& projectDir, bool release) {
    fs::path projPath = fs::absolute(projectDir);

    if (!fs::exists(projPath)) {
        std::cerr << "خطأ: مجلد المشروع غير موجود: " << projPath.string() << std::endl;
        return 1;
    }

    // التحقق من وجود ملفات Gradle
#ifdef _WIN32
    std::string gradlew = (projPath / "gradlew.bat").string();
#else
    std::string gradlew = (projPath / "gradlew").string();
#endif

    if (!fs::exists(gradlew)) {
        std::cerr << "خطأ: لم يتم العثور على gradlew في: " << projPath.string() << std::endl;
        std::cerr << "تأكد من تشغيل 'sad-android create' أولاً" << std::endl;
        return 1;
    }

    // التحقق من ملفات .ص
    fs::path sadDir = projPath / "sad_sources";
    if (fs::exists(sadDir)) {
        int sadCount = 0;
        for (auto& entry : fs::recursive_directory_iterator(sadDir)) {
            std::string ext = entry.path().extension().string();
            if (ext == ".ص" || ext == ".sad") {
                sadCount++;
            }
        }
        std::cout << "[بناء] تم العثور على " << sadCount << " ملف مصدري (.ص)" << std::endl;
    }

    // بناء Gradle
    std::string buildType = release ? "assembleRelease" : "assembleDebug";
    std::string cmd;
#ifdef _WIN32
    cmd = "cd /d \"" + projPath.string() + "\" && \"" + gradlew + "\" " + buildType;
#else
    cmd = "cd \"" + projPath.string() + "\" && chmod +x gradlew && ./gradlew " + buildType;
#endif

    std::cout << "[بناء] بدء البناء (" << buildType << ")..." << std::endl;
    int result = runCommand(cmd);

    if (result == 0) {
        // البحث عن APK الناتج
        std::string apkSubdir = release ? "release" : "debug";
        fs::path apkDir = projPath / "app" / "build" / "outputs" / "apk" / apkSubdir;
        if (fs::exists(apkDir)) {
            for (auto& entry : fs::directory_iterator(apkDir)) {
                if (entry.path().extension() == ".apk") {
                    std::cout << "[بناء] ✓ تم إنشاء APK: " << entry.path().string() << std::endl;
                }
            }
        }
        std::cout << "[بناء] ✓ اكتمل البناء بنجاح" << std::endl;
    } else {
        std::cerr << "[بناء] ✗ فشل البناء (كود الخروج: " << result << ")" << std::endl;
    }

    return result;
}

// ============================================================================
// أمر: sign — توقيع APK / Sign APK
// ============================================================================
int cmdSign(const std::string& apkPath, const std::string& keystorePath,
            const std::string& alias, const std::string& storePass) {
    
    if (!fs::exists(apkPath)) {
        std::cerr << "خطأ: ملف APK غير موجود: " << apkPath << std::endl;
        return 1;
    }

    if (!fs::exists(keystorePath)) {
        std::cerr << "خطأ: ملف keystore غير موجود: " << keystorePath << std::endl;
        return 1;
    }

    auto env = detectEnvironment();
    if (!env.valid || env.buildToolsPath.empty()) {
        std::cerr << "خطأ: لم يتم العثور على Android SDK build-tools" << std::endl;
        std::cerr << "تأكد من تعيين ANDROID_HOME" << std::endl;
        return 1;
    }

    // الخطوة 1: zipalign
    fs::path apkFile(apkPath);
    std::string alignedPath = apkFile.parent_path().string() + "/" +
                              apkFile.stem().string() + "-aligned.apk";

    std::string zipalignCmd = "\"" + env.getZipalign() + "\" -v -p 4 \"" +
                              apkPath + "\" \"" + alignedPath + "\"";
    std::cout << "[توقيع] الخطوة 1: محاذاة APK..." << std::endl;
    int result = runCommand(zipalignCmd);
    if (result != 0) {
        std::cerr << "[توقيع] ✗ فشلت محاذاة APK" << std::endl;
        return result;
    }

    // الخطوة 2: apksigner
    std::string signedPath = apkFile.parent_path().string() + "/" +
                             apkFile.stem().string() + "-signed.apk";

    std::string signCmd = "\"" + env.getApksigner() + "\" sign" +
                          " --ks \"" + keystorePath + "\"" +
                          " --ks-key-alias " + alias;
    if (!storePass.empty()) {
        signCmd += " --ks-pass pass:" + storePass;
    }
    signCmd += " --out \"" + signedPath + "\" \"" + alignedPath + "\"";

    std::cout << "[توقيع] الخطوة 2: توقيع APK..." << std::endl;
    result = runCommand(signCmd);
    if (result != 0) {
        std::cerr << "[توقيع] ✗ فشل التوقيع" << std::endl;
        return result;
    }

    std::cout << "[توقيع] ✓ تم التوقيع: " << signedPath << std::endl;
    return 0;
}

// ============================================================================
// أمر: install — تثبيت APK على الجهاز / Install APK
// ============================================================================
int cmdInstall(const std::string& apkPath) {
    if (!fs::exists(apkPath)) {
        std::cerr << "خطأ: ملف APK غير موجود: " << apkPath << std::endl;
        return 1;
    }

    auto env = detectEnvironment();
    std::string adb = env.getAdb();

    std::cout << "[تثبيت] تثبيت APK على الجهاز..." << std::endl;
    std::string cmd = "\"" + adb + "\" install -r \"" + apkPath + "\"";
    int result = runCommand(cmd);

    if (result == 0) {
        std::cout << "[تثبيت] ✓ تم التثبيت بنجاح" << std::endl;
    } else {
        std::cerr << "[تثبيت] ✗ فشل التثبيت — تأكد من اتصال الجهاز" << std::endl;
    }

    return result;
}

// ============================================================================
// أمر: run — تشغيل التطبيق على الجهاز / Run App
// ============================================================================
int cmdRun(const std::string& packageName, const std::string& activityName) {
    auto env = detectEnvironment();
    std::string adb = env.getAdb();

    std::string activity = activityName.empty() ? ".MainActivity" : activityName;
    std::string component = packageName + "/" + activity;

    std::cout << "[تشغيل] تشغيل: " << component << std::endl;
    std::string cmd = "\"" + adb + "\" shell am start -n " + component;
    int result = runCommand(cmd);

    if (result == 0) {
        std::cout << "[تشغيل] ✓ تم تشغيل التطبيق" << std::endl;
    } else {
        std::cerr << "[تشغيل] ✗ فشل التشغيل" << std::endl;
    }

    return result;
}

// ============================================================================
// أمر: info — معلومات البيئة / Environment Info
// ============================================================================
int cmdInfo() {
    auto env = detectEnvironment();

    std::cout << "=== معلومات بيئة Android ===" << std::endl;

    if (env.valid) {
        std::cout << "  SDK:         " << env.sdkPath << std::endl;
        std::cout << "  NDK:         " << (env.ndkPath.empty() ? "(غير موجود)" : env.ndkPath) << std::endl;
        std::cout << "  Build Tools: " << (env.buildToolsPath.empty() ? "(غير موجود)" : env.buildToolsPath) << std::endl;
        std::cout << "  zipalign:    " << (fs::exists(env.getZipalign()) ? "✓" : "✗") << std::endl;
        std::cout << "  apksigner:   " << (fs::exists(env.getApksigner()) ? "✓" : "✗") << std::endl;
        std::cout << "  adb:         " << (fs::exists(env.getAdb()) ? "✓" : "✗") << std::endl;
    } else {
        std::cout << "  ✗ لم يتم العثور على Android SDK" << std::endl;
        std::cout << "  عيّن ANDROID_HOME أو ANDROID_SDK_ROOT" << std::endl;
    }

    // Java
    const char* javaHome = std::getenv("JAVA_HOME");
    std::cout << "  JAVA_HOME:   " << (javaHome ? javaHome : "(غير معيّن)") << std::endl;

    return 0;
}

// ============================================================================
// المساعدة / Help
// ============================================================================
void printHelp() {
    std::cout << R"(
أداة بناء APK — لغة ص / Sad APK Builder Tool
===============================================

الأوامر:
  build [--release] [--project-dir <path>]
      بناء مشروع Android (افتراضي: debug)

  sign <apk> --keystore <ks> --alias <alias> [--pass <password>]
      توقيع ملف APK

  install <apk>
      تثبيت APK على جهاز متصل عبر ADB

  run --package <pkg> [--activity <act>]
      تشغيل التطبيق على الجهاز

  info
      عرض معلومات بيئة Android SDK/NDK

  help
      عرض هذه المساعدة

أمثلة:
  sad_android_builder build --project-dir my_app
  sad_android_builder build --release --project-dir my_app
  sad_android_builder sign app-release.apk --keystore my.jks --alias mykey
  sad_android_builder install app-debug.apk
  sad_android_builder run --package com.example.myapp
  sad_android_builder info
)" << std::endl;
}

// ============================================================================
// نقطة الدخول / Entry Point
// ============================================================================
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp();
        return 0;
    }

    std::string command = argv[1];

    if (command == "help" || command == "--help" || command == "-h" || command == "مساعدة") {
        printHelp();
        return 0;
    }

    if (command == "info" || command == "معلومات") {
        return cmdInfo();
    }

    if (command == "build" || command == "ابنِ" || command == "بناء") {
        bool release = false;
        std::string projectDir = ".";
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--release") {
                release = true;
            } else if (arg == "--project-dir" && i + 1 < argc) {
                projectDir = argv[++i];
            }
        }
        return cmdBuild(projectDir, release);
    }

    if (command == "sign" || command == "وقّع") {
        if (argc < 3) {
            std::cerr << "خطأ: يجب تحديد ملف APK" << std::endl;
            return 1;
        }
        std::string apkPath = argv[2];
        std::string keystore, alias, pass;
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if ((arg == "--keystore" || arg == "--ks") && i + 1 < argc) {
                keystore = argv[++i];
            } else if (arg == "--alias" && i + 1 < argc) {
                alias = argv[++i];
            } else if (arg == "--pass" && i + 1 < argc) {
                pass = argv[++i];
            }
        }
        if (keystore.empty() || alias.empty()) {
            std::cerr << "خطأ: يجب تحديد --keystore و --alias" << std::endl;
            return 1;
        }
        return cmdSign(apkPath, keystore, alias, pass);
    }

    if (command == "install" || command == "ثبّت") {
        if (argc < 3) {
            std::cerr << "خطأ: يجب تحديد ملف APK" << std::endl;
            return 1;
        }
        return cmdInstall(argv[2]);
    }

    if (command == "run" || command == "شغّل") {
        std::string package, activity;
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--package" && i + 1 < argc) {
                package = argv[++i];
            } else if (arg == "--activity" && i + 1 < argc) {
                activity = argv[++i];
            }
        }
        if (package.empty()) {
            std::cerr << "خطأ: يجب تحديد --package" << std::endl;
            return 1;
        }
        return cmdRun(package, activity);
    }

    std::cerr << "أمر غير معروف: " << command << std::endl;
    printHelp();
    return 1;
}
