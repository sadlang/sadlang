/**
 * @file apk_builder.cpp
 * @brief أداة بناء APK من ملفات .ص فقط
 * 
 * هذه الأداة تأخذ مجلد مشروع يحتوي ملفات .ص وتنتج APK
 * 
 * الاستخدام:
 *   sad build-apk <مسار_المشروع> [خيارات]
 * 
 * خيارات:
 *   --output, -o     مسار ملف APK الناتج
 *   --name, -n       اسم التطبيق
 *   --package, -p    اسم الحزمة (com.example.app)
 *   --icon, -i       مسار أيقونة التطبيق
 *   --release, -r    بناء نسخة Release
 *   --sign, -s       توقيع APK
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <map>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#include <algorithm>
#endif

namespace fs = std::filesystem;

// ═══════════════════════════════════════════════════════════════════════════
//                              الهيكل الرئيسي
// ═══════════════════════════════════════════════════════════════════════════

struct ProjectConfig {
    std::string name = "تطبيق ص";
    std::string packageName = "com.sad.app";
    std::string version = "1.0.0";
    int versionCode = 1;
    int minSdk = 24;
    int targetSdk = 34;
    std::string iconPath;
    std::string mainFile = "التطبيق.ص";
    std::vector<std::string> sourceFiles;
};

struct BuildOptions {
    std::string projectPath;
    std::string outputPath;
    std::string sdkPath;        // مسار Android SDK
    std::string ndkPath;        // مسار NDK
    std::string nativeLibPath;  // مسار المكتبة الأصلية (libsad_app.so)
    std::string keystorePath;   // مسار keystore للتوقيع
    std::string keystorePass = "android";
    bool release = false;
    bool sign = false;
    bool verbose = false;
    bool dryRun = false;        // عرض الأوامر فقط
    bool buildNative = false;   // بناء المكتبة الأصلية من المصدر
};

// ═══════════════════════════════════════════════════════════════════════════
//                              كشف Android SDK
// ═══════════════════════════════════════════════════════════════════════════

std::string findAndroidSdk() {
    // 1. متغير البيئة
    const char* envSdk = std::getenv("ANDROID_HOME");
    if (envSdk && fs::exists(envSdk)) return envSdk;
    
    envSdk = std::getenv("ANDROID_SDK_ROOT");
    if (envSdk && fs::exists(envSdk)) return envSdk;
    
#ifdef _WIN32
    // 2. المسارات الافتراضية على Windows
    const char* localAppData = std::getenv("LOCALAPPDATA");
    if (localAppData) {
        std::string defaultPath = std::string(localAppData) + "\\Android\\Sdk";
        if (fs::exists(defaultPath)) return defaultPath;
    }
    
    const char* userProfile = std::getenv("USERPROFILE");
    if (userProfile) {
        std::string defaultPath = std::string(userProfile) + "\\AppData\\Local\\Android\\Sdk";
        if (fs::exists(defaultPath)) return defaultPath;
    }
#else
    // Linux/Mac
    const char* home = std::getenv("HOME");
    if (home) {
        std::string defaultPath = std::string(home) + "/Android/Sdk";
        if (fs::exists(defaultPath)) return defaultPath;
        
        defaultPath = std::string(home) + "/Library/Android/sdk";
        if (fs::exists(defaultPath)) return defaultPath;
    }
#endif
    
    return "";
}

std::string findLatestBuildTools(const std::string& sdkPath) {
    fs::path btPath = fs::path(sdkPath) / "build-tools";
    if (!fs::exists(btPath)) return "";
    
    std::string latest;
    for (const auto& entry : fs::directory_iterator(btPath)) {
        if (entry.is_directory()) {
            std::string name = entry.path().filename().string();
            if (name > latest) latest = name;
        }
    }
    
    return latest.empty() ? "" : (btPath / latest).string();
}

std::string findLatestPlatform(const std::string& sdkPath) {
    fs::path platPath = fs::path(sdkPath) / "platforms";
    if (!fs::exists(platPath)) return "";
    
    std::string latest;
    for (const auto& entry : fs::directory_iterator(platPath)) {
        if (entry.is_directory()) {
            std::string name = entry.path().filename().string();
            if (name > latest) latest = name;
        }
    }
    
    return latest.empty() ? "" : (platPath / latest).string();
}

std::string findLatestNdk(const std::string& sdkPath) {
    fs::path ndkPath = fs::path(sdkPath) / "ndk";
    if (!fs::exists(ndkPath)) return "";
    
    std::string latest;
    for (const auto& entry : fs::directory_iterator(ndkPath)) {
        if (entry.is_directory()) {
            std::string name = entry.path().filename().string();
            if (name > latest) latest = name;
        }
    }
    
    return latest.empty() ? "" : (ndkPath / latest).string();
}

int runCommand(const std::string& cmd, bool verbose = false) {
    if (verbose) {
        std::cout << "   $ " << cmd << std::endl;
    }
    
#ifdef _WIN32
    // على Windows المسارات التي تحتوي مسافات تسبب مشاكل مع system
    // التفاف الأمر في علامات اقتباس إضافية ضروري جداً
    std::string fullCmd = "cmd /c \" " + cmd + " \"";
    if (verbose) {
        std::cout << "   [Windows] = " << fullCmd << std::endl;
    }
    
    // أفضل طريقة لحل مشاكل الاقتباس في Windows
    // هي إنشاء ملف batch مؤقت وتنفيذه
    fs::path tempBat = fs::temp_directory_path() / "sad_cmd.bat";
    
    std::ofstream out(tempBat);
    if (out) {
        out << "@echo off\n";
        out << cmd << "\n";
        out.close();
    }
    
    int ret = std::system(tempBat.string().c_str());
    return ret;
#else
    return std::system(cmd.c_str());
#endif
}

// ═══════════════════════════════════════════════════════════════════════════
//                              دوال مساعدة
// ═══════════════════════════════════════════════════════════════════════════

void setUTF8Console() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

void printUsage() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════════╗
║                   أداة بناء APK للغة ص                           ║
╚═══════════════════════════════════════════════════════════════════╝

الاستخدام:
  sad-apk <مسار_المشروع> [خيارات]

خيارات:
  -o, --output <مسار>       مسار ملف APK الناتج
  -n, --name <اسم>          اسم التطبيق
  -p, --package <حزمة>      اسم الحزمة (مثال: com.example.myapp)
  -i, --icon <مسار>         مسار أيقونة التطبيق (PNG)
  -r, --release             بناء نسخة Release
  -s, --sign                توقيع APK
  -k, --keystore <مسار>     مسار ملف keystore للتوقيع
  --keystore-pass <pass>    كلمة مرور keystore
  --sdk <مسار>              مسار Android SDK (اختياري)
  --lib <مسار>              مسار المكتبة الأصلية libsad_app.so
  --dry-run                 عرض الأوامر بدون تنفيذها
  -v, --verbose             عرض تفاصيل البناء
  -h, --help                عرض هذه المساعدة

أمثلة:
  sad-apk مشروعي/
  sad-apk مشروعي/ -o مشروعي.apk -s
  sad-apk مشروعي/ --sdk "C:\Android\Sdk" -v
  sad-apk مشروعي/ --lib build_test_ui/libsad_flutter.so

هيكل المشروع المطلوب:
  مشروعي/
  ├── التطبيق.ص          # نقطة الدخول الرئيسية
  ├── libsad_app.so      # (اختياري) المكتبة الأصلية
  ├── مشروع.صد           # (اختياري) إعدادات المشروع
  ├── الأيقونة.png       # (اختياري) أيقونة التطبيق
  └── *.ص                # ملفات المصدر الأخرى

)" << std::endl;
}

std::string readFile(const fs::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return "";
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

bool writeFile(const fs::path& path, const std::string& content) {
    fs::create_directories(path.parent_path());
    std::ofstream file(path, std::ios::binary);
    if (!file) return false;
    file << content;
    return true;
}

// تحليل ملف إعدادات المشروع (مشروع.صد)
ProjectConfig parseProjectConfig(const fs::path& configPath) {
    ProjectConfig config;
    
    if (!fs::exists(configPath)) {
        return config;
    }
    
    std::string content = readFile(configPath);
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // تجاهل التعليقات والأسطر الفارغة
        if (line.empty() || line[0] == '#') continue;
        
        // تحليل key = value
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = line.substr(0, eqPos);
            std::string value = line.substr(eqPos + 1);
            
            // إزالة المسافات
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t\""));
            value.erase(value.find_last_not_of(" \t\"") + 1);
            
            if (key == "الاسم" || key == "name") {
                config.name = value;
            } else if (key == "الحزمة" || key == "package") {
                config.packageName = value;
            } else if (key == "الإصدار" || key == "version") {
                config.version = value;
            } else if (key == "الأيقونة" || key == "icon") {
                config.iconPath = value;
            } else if (key == "الملف_الرئيسي" || key == "main") {
                config.mainFile = value;
            }
        }
    }
    
    return config;
}

// جمع ملفات .ص من المشروع
std::vector<fs::path> collectSourceFiles(const fs::path& projectPath) {
    std::vector<fs::path> files;
    
    printf("Scanning directory: %s\n", projectPath.string().c_str());
    fflush(stdout);
    
    try {
        for (const auto& entry : fs::recursive_directory_iterator(projectPath)) {
            if (entry.is_regular_file()) {
                // Use wstring on Windows for proper Unicode handling
#ifdef _WIN32
                std::wstring wext = entry.path().extension().wstring();
                bool isSadFile = (wext == L".sad");
                
                // Check for Arabic ص (U+0635)
                if (!isSadFile && wext.length() >= 2) {
                    if (wext[1] == L'\x0635') {
                        isSadFile = true;
                    }
                }
                
                if (isSadFile) {
                    files.push_back(entry.path());
                    // Use wprintf for Unicode filenames on Windows
                    wprintf(L"  Found: %ls\n", entry.path().filename().wstring().c_str());
                    fflush(stdout);
                }
#else
                auto ext = entry.path().extension().string();
                bool isSadFile = (ext == ".sad");
                
                // Check UTF-8 bytes for Arabic ص
                if (!isSadFile && ext.length() >= 3) {
                    unsigned char b0 = ext[1];
                    unsigned char b1 = ext.length() > 2 ? ext[2] : 0;
                    if (b0 == 0xD8 && b1 == 0xB5) {
                        isSadFile = true;
                    }
                }
                
                if (isSadFile) {
                    files.push_back(entry.path());
                    printf("  Found: %s\n", entry.path().filename().string().c_str());
                    fflush(stdout);
                }
#endif
            }
        }
    } catch (const std::exception& e) {
        printf("Error scanning: %s\n", e.what());
        fflush(stdout);
    }
    
    return files;
}

// ═══════════════════════════════════════════════════════════════════════════
//                              بناء APK
// ═══════════════════════════════════════════════════════════════════════════

class ApkBuilder {
public:
    ApkBuilder(const BuildOptions& options, const ProjectConfig& config)
        : options_(options), config_(config) {}
    
    bool build() {
        std::cout << "═══════════════════════════════════════════════════════════" << std::endl;
        std::cout << "          بناء تطبيق: " << config_.name << std::endl;
        std::cout << "═══════════════════════════════════════════════════════════" << std::endl;
        
        // 1. إنشاء مجلد البناء المؤقت
        if (!createBuildDir()) return false;
        
        // 2. نسخ ملفات .ص إلى assets
        if (!copySourceFiles()) return false;
        
        // 3. إنشاء AndroidManifest.xml
        if (!generateManifest()) return false;
        
        // 4. نسخ الموارد
        if (!copyResources()) return false;
        
        // 5. نسخ أو بناء المكتبة الأصلية
        if (!copyNativeLib()) return false;
        
        // 6. بناء APK باستخدام أدوات Android SDK
        if (!buildApk()) return false;
        
        // 7. توقيع APK (إذا مطلوب)
        if (options_.sign && !signApk()) return false;
        
        std::cout << std::endl;
        std::cout << "✅ تم البناء بنجاح: " << outputApkPath_.string() << std::endl;
        
        return true;
    }
    
private:
    BuildOptions options_;
    ProjectConfig config_;
    fs::path buildDir_;
    fs::path outputApkPath_;
    
    bool createBuildDir() {
        buildDir_ = fs::temp_directory_path() / "sad_apk_build";
        fs::remove_all(buildDir_);
        fs::create_directories(buildDir_);
        fs::create_directories(buildDir_ / "assets");
        fs::create_directories(buildDir_ / "res" / "values");
        fs::create_directories(buildDir_ / "lib");
        
        if (options_.verbose) {
            std::cout << "📁 مجلد البناء: " << buildDir_.string() << std::endl;
        }
        
        return true;
    }
    
    bool copySourceFiles() {
        std::cout << "📄 نسخ ملفات المصدر..." << std::endl;
        
        fs::path projectPath = options_.projectPath;
        auto sourceFiles = collectSourceFiles(projectPath);
        
        int copied = 0;
        for (const auto& file : sourceFiles) {
            try {
                fs::path relativePath = fs::relative(file, projectPath);
                
                // تحويل الامتداد إلى .sad لجميع ملفات المصدر (لتفادي مشاكل aapt و std::filesystem على Windows مع الحروف العربية)
                relativePath.replace_extension(".sad");
                
                fs::path destPath = buildDir_ / "assets" / relativePath;
                
                fs::create_directories(destPath.parent_path());
                fs::copy_file(file, destPath, fs::copy_options::overwrite_existing);
                copied++;
                
                if (options_.verbose) {
#ifdef _WIN32
                    std::wcout << L"   ← " << relativePath.wstring() << std::endl;
#else
                    std::cout << "   ← " << relativePath.string() << std::endl;
#endif
                }
            } catch (const std::exception& e) {
                if (options_.verbose) {
                    std::cerr << "   ⚠️ تجاهل ملف: " << e.what() << std::endl;
                }
            }
        }
        
        std::cout << "   تم نسخ " << copied << " ملف" << std::endl;
        
        return true;
    }
    
    bool generateManifest() {
        std::cout << "📝 إنشاء AndroidManifest.xml..." << std::endl;
        
        std::ostringstream manifest;
        manifest << R"(<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package=")" << config_.packageName << R"("
    android:versionCode=")" << config_.versionCode << R"("
    android:versionName=")" << config_.version << R"(">
    
    <uses-sdk android:minSdkVersion=")" << config_.minSdk << R"(" 
              android:targetSdkVersion=")" << config_.targetSdk << R"(" />
    
    <uses-permission android:name="android.permission.INTERNET" />
    
    <application
        android:label=")" << config_.name << R"("
        android:hasCode="false"
        android:supportsRtl="true"
        android:allowBackup="true">
        
        <activity
            android:name="android.app.NativeActivity"
            android:exported="true"
            android:configChanges="orientation|keyboardHidden|screenSize">
            
            <meta-data android:name="android.app.lib_name"
                       android:value="sad_app" />
            
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
        </activity>
        
    </application>
    
</manifest>
)";
        
        writeFile(buildDir_ / "AndroidManifest.xml", manifest.str());
        return true;
    }
    
    bool copyResources() {
        std::cout << "🎨 إعداد الموارد..." << std::endl;
        
        // strings.xml
        std::ostringstream strings;
        strings << R"(<?xml version="1.0" encoding="utf-8"?>
<resources>
    <string name="app_name">)" << config_.name << R"(</string>
</resources>
)";
        writeFile(buildDir_ / "res" / "values" / "strings.xml", strings.str());
        
        // styles.xml
        std::string styles = R"(<?xml version="1.0" encoding="utf-8"?>
<resources>
    <style name="SadTheme" parent="android:Theme.Material.Light.NoActionBar">
        <item name="android:colorPrimary">#1976D2</item>
        <item name="android:colorPrimaryDark">#1565C0</item>
        <item name="android:colorAccent">#FF9800</item>
    </style>
</resources>
)";
        writeFile(buildDir_ / "res" / "values" / "styles.xml", styles);
        
        return true;
    }
    
    bool copyNativeLib() {
        std::cout << "📚 إعداد المكتبة الأصلية..." << std::endl;
        
        // أنشئ مجلدات جميع المعماريات
        std::vector<std::string> abis = {"arm64-v8a", "armeabi-v7a", "x86_64", "x86"};
        for (const auto& abi : abis) {
            fs::create_directories(buildDir_ / "lib" / abi);
        }
        
        // إذا حُدد مسار مكتبة مباشرة
        if (!options_.nativeLibPath.empty()) {
            fs::path libPath = options_.nativeLibPath;
            if (fs::exists(libPath)) {
                // نسخ للمعمارية المطلوبة (افتراضي: arm64-v8a)
                fs::path dest = buildDir_ / "lib" / "arm64-v8a" / "libsad_app.so";
                fs::copy_file(libPath, dest, fs::copy_options::overwrite_existing);
                std::cout << "   ✓ نُسخت المكتبة: " << libPath.filename().string() << std::endl;
                return true;
            } else {
                std::cerr << "   ⚠️ المكتبة غير موجودة: " << libPath.string() << std::endl;
            }
        }
        
        // البحث عن مكتبة مترجمة مسبقًا في مسارات معروفة
        std::vector<fs::path> searchPaths = {
            fs::path(options_.projectPath) / "lib" / "arm64-v8a" / "libsad_app.so",
            fs::path(options_.projectPath) / "libsad_app.so",
            fs::path(options_.projectPath) / ".." / "libsad_flutter.so",
        };
        
        // أضف مسار build_test_ui إذا كان موجودًا
        fs::path workspaceRoot = fs::path(options_.projectPath).parent_path();
        searchPaths.push_back(workspaceRoot / "build_test_ui" / "libsad_flutter.so");
        searchPaths.push_back(workspaceRoot / "build_test_ui" / "lib" / "arm64-v8a" / "libsad_app.so");
        
        for (const auto& searchPath : searchPaths) {
            if (fs::exists(searchPath)) {
                fs::path dest = buildDir_ / "lib" / "arm64-v8a" / "libsad_app.so";
                fs::copy_file(searchPath, dest, fs::copy_options::overwrite_existing);
                std::cout << "   ✓ وُجدت واستُخدمت: " << searchPath.filename().string() << std::endl;
                return true;
            }
        }
        
        // إذا لم تُوجد مكتبة — تحذير وليس خطأ (يمكن إضافتها لاحقًا)
        std::cout << "   ⚠️ لم تُوجد مكتبة أصلية — APK لن يعمل بدونها!" << std::endl;
        std::cout << "   💡 استخدم --lib <path> لتحديد مسار libsad_app.so" << std::endl;
        std::cout << "   💡 أو ضع libsad_app.so في مجلد المشروع" << std::endl;
        
        return true; // استمر بدون المكتبة
    }
    
    bool buildApk() {
        std::cout << "🔨 بناء APK..." << std::endl;
        
        // كشف Android SDK
        std::string sdkPath = options_.sdkPath.empty() ? findAndroidSdk() : options_.sdkPath;
        if (sdkPath.empty()) {
            std::cerr << "❌ لم يتم العثور على Android SDK!" << std::endl;
            std::cerr << "   عيّن ANDROID_HOME أو استخدم --sdk <path>" << std::endl;
            return false;
        }
        std::cout << "   📱 SDK: " << sdkPath << std::endl;
        
        // العثور على build-tools و platform
        std::string btPath = findLatestBuildTools(sdkPath);
        std::string platPath = findLatestPlatform(sdkPath);
        
        if (btPath.empty()) {
            std::cerr << "❌ لم يتم العثور على build-tools!" << std::endl;
            return false;
        }
        if (platPath.empty()) {
            std::cerr << "❌ لم يتم العثور على platform!" << std::endl;
            return false;
        }
        
        std::cout << "   🔧 Build Tools: " << fs::path(btPath).filename().string() << std::endl;
        std::cout << "   📦 Platform: " << fs::path(platPath).filename().string() << std::endl;
        
        // أدوات البناء
#ifdef _WIN32
        std::string aapt = "\"" + btPath + "\\aapt.exe\"";
        std::string zipalign = "\"" + btPath + "\\zipalign.exe\"";
        std::string apksigner = "\"" + btPath + "\\apksigner.bat\"";
        std::string androidJar = "\"" + platPath + "\\android.jar\"";
#else
        std::string aapt = btPath + "/aapt";
        std::string zipalign = btPath + "/zipalign";
        std::string apksigner = btPath + "/apksigner";
        std::string androidJar = platPath + "/android.jar";
#endif
        
        // تحديد مسار APK الناتج
        std::string outputName = "app"; // اسم افتراضي آمن
        // استخدم اسم المشروع إذا كان ASCII فقط
        bool isAscii = true;
        for (char c : config_.name) {
            if (static_cast<unsigned char>(c) > 127) { isAscii = false; break; }
        }
        if (isAscii) outputName = config_.name;
        
        if (options_.outputPath.empty()) {
            outputApkPath_ = fs::path(options_.projectPath) / (outputName + ".apk");
        } else {
            outputApkPath_ = options_.outputPath;
        }
        
        std::cout << "   📄 Output: " << outputApkPath_.string() << std::endl;
        
        fs::path unsignedApk = buildDir_ / "app_unsigned.apk";
        fs::path alignedApk = buildDir_ / "app_aligned.apk";
        
        // تغيير المجلد الحالي إلى مجلد البناء
        fs::path cwd = fs::current_path();
        fs::current_path(buildDir_);
        
        // 1. إنشاء APK باستخدام aapt
        std::cout << "   📦 تجميع الموارد..." << std::endl;
        std::ostringstream cmd;
        cmd << "\"" << aapt << "\" package -f"
            << " -M AndroidManifest.xml"
            << " -I " << androidJar
            << " -F app_unsigned.apk";
        
        if (options_.dryRun) {
            std::cout << "   $ " << cmd.str() << std::endl;
        } else {
            int ret = runCommand(cmd.str(), options_.verbose);
            if (ret != 0) {
                std::cerr << "❌ فشل aapt package" << std::endl;
                return false;
            }
        }
        
        // 2. إضافة assets إلى APK
        std::cout << "   📁 إضافة الملفات..." << std::endl;
        
        // إضافة assets (نحن الآن في مجلد البناء)
        fs::path assetsPath = buildDir_ / "assets";
        try {
            if (fs::exists(assetsPath) && !fs::is_empty(assetsPath)) {
                for (const auto& entry : fs::recursive_directory_iterator(assetsPath)) {
                    if (entry.is_regular_file()) {
                        fs::path relPath = fs::relative(entry.path(), buildDir_);
                        std::string unixPath = relPath.string();
                        std::replace(unixPath.begin(), unixPath.end(), '\\', '/');
                        
                        cmd.str("");
                        cmd << "\"" << aapt << "\" add app_unsigned.apk \"" << unixPath << "\"";
                        if (!options_.dryRun) {
                            int ret = runCommand(cmd.str(), options_.verbose);
                            if (ret != 0 && options_.verbose) {
                                std::cerr << "   ⚠️ فشل إضافة: " << unixPath << std::endl;
                            }
                        } else {
                            std::cout << "   $ " << cmd.str() << std::endl;
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "   ⚠️ خطأ أثناء قراءة assets: " << e.what() << std::endl;
        }
        
        // إضافة المكتبة الأصلية
        fs::path libPath = buildDir_ / "lib";
        try {
            if (fs::exists(libPath)) {
                for (const auto& abiDir : fs::directory_iterator(libPath)) {
                    if (abiDir.is_directory()) {
                        for (const auto& lib : fs::directory_iterator(abiDir.path())) {
                            if (lib.is_regular_file() && lib.path().extension() == ".so") {
                                fs::path relPath = fs::relative(lib.path(), buildDir_);
                                std::string unixPath = relPath.string();
                                std::replace(unixPath.begin(), unixPath.end(), '\\', '/');
                                
                                cmd.str("");
                                cmd << "\"" << aapt << "\" add app_unsigned.apk \"" << unixPath << "\"";
                                if (!options_.dryRun) {
                                    int ret = runCommand(cmd.str(), options_.verbose);
                                    if (ret == 0) {
                                        std::cout << "   ✓ أُضيفت: " << unixPath << std::endl;
                                    } else if (options_.verbose) {
                                        std::cerr << "   ⚠️ فشل إضافة المكتبة: " << unixPath << std::endl;
                                    }
                                } else {
                                    std::cout << "   $ " << cmd.str() << std::endl;
                                }
                            }
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "   ⚠️ خطأ أثناء قراءة lib: " << e.what() << std::endl;
        }
        
        // 3. Zipalign (لا نزال في مجلد البناء)
        std::cout << "   🔧 محاذاة APK..." << std::endl;
        cmd.str("");
        cmd << "\"" << zipalign << "\" -f 4 app_unsigned.apk app_aligned.apk";
        
        if (options_.dryRun) {
            std::cout << "   $ " << cmd.str() << std::endl;
        } else {
            int ret = runCommand(cmd.str(), options_.verbose);
            if (ret != 0) {
                std::cerr << "❌ فشل zipalign" << std::endl;
                // الرجوع للمجلد الأصلي
                fs::current_path(cwd);
                return false;
            }
        }
        
        // 4. توقيع APK
        if (options_.sign) {
            std::cout << "   🔐 توقيع APK..." << std::endl;
            
            std::string keystore = options_.keystorePath;
            if (keystore.empty()) {
                // إنشاء keystore مؤقت للتطوير
                keystore = (buildDir_ / "debug.keystore").string();
                if (!fs::exists(keystore)) {
                    std::cout << "   ⚠️ لا يوجد keystore، سيتم إنشاء واحد للتطوير..." << std::endl;
                    // يمكن استخدام keytool لإنشاء keystore
                }
            }
            
            // الرجوع للمجلد الأصلي لأن outputApkPath_ قد يكون مساراً نسبياً
            fs::current_path(cwd);
            
            cmd.str("");
            cmd << "\"" << apksigner << "\" sign"
                << " --ks \"" << keystore << "\""
                << " --ks-pass pass:" << options_.keystorePass
                << " --key-pass pass:" << options_.keystorePass
                << " --out \"" << outputApkPath_.string() << "\""
                << " \"" << (buildDir_ / "app_aligned.apk").string() << "\"";
            
            if (options_.dryRun) {
                std::cout << "   $ " << cmd.str() << std::endl;
            } else {
                int ret = runCommand(cmd.str(), options_.verbose);
                if (ret != 0) {
                    std::cerr << "❌ فشل توقيع APK" << std::endl;
                    return false;
                }
            }
        } else {
            // الرجوع للمجلد الأصلي قبل النسخ النهائي
            fs::current_path(cwd);
            
            // نسخ APK غير الموقع
            if (options_.dryRun) {
                std::cout << "   📋 سيتم نسخ: app_aligned.apk -> " << outputApkPath_.string() << std::endl;
            } else {
                fs::copy_file(buildDir_ / "app_aligned.apk", outputApkPath_, fs::copy_options::overwrite_existing);
            }
        }
        
        // التأكد من الرجوع للمجلد الأصلي
        fs::current_path(cwd);
        
        return true;
    }
    
    bool signApk() {
        // التوقيع يتم داخل buildApk الآن
        return true;
    }
};

// ═══════════════════════════════════════════════════════════════════════════
//                              نقطة الدخول
// ═══════════════════════════════════════════════════════════════════════════

int main(int argc, char* argv[]) {
    // Debug: early output test
    printf("sad-apk starting...\n");
    fflush(stdout);
    
    setUTF8Console();
    
    printf("argc=%d\n", argc);
    fflush(stdout);
    
    if (argc < 2) {
        printUsage();
        return 1;
    }
    
    printf("Parsing args...\n");
    fflush(stdout);
    
    BuildOptions options;
    
    // تحليل المعاملات
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage();
            return 0;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) options.outputPath = argv[++i];
        } else if (arg == "--sdk") {
            if (i + 1 < argc) options.sdkPath = argv[++i];
        } else if (arg == "--lib") {
            if (i + 1 < argc) options.nativeLibPath = argv[++i];
        } else if (arg == "--keystore" || arg == "-k") {
            if (i + 1 < argc) options.keystorePath = argv[++i];
        } else if (arg == "--keystore-pass") {
            if (i + 1 < argc) options.keystorePass = argv[++i];
        } else if (arg == "-r" || arg == "--release") {
            options.release = true;
        } else if (arg == "-s" || arg == "--sign") {
            options.sign = true;
        } else if (arg == "-v" || arg == "--verbose") {
            options.verbose = true;
        } else if (arg == "--dry-run") {
            options.dryRun = true;
            options.verbose = true;
        } else if (arg[0] != '-') {
            options.projectPath = arg;
        }
    }
    
    // التحقق من مسار المشروع
    printf("Checking project path: %s\n", options.projectPath.c_str());
    fflush(stdout);
    
    if (options.projectPath.empty()) {
        fprintf(stderr, "Error: project path empty\n");
        return 1;
    }
    
    printf("Checking if path exists...\n");
    fflush(stdout);
    
    if (!fs::exists(options.projectPath)) {
        fprintf(stderr, "Error: path not found: %s\n", options.projectPath.c_str());
        return 1;
    }
    
    printf("Path exists, loading config...\n");
    fflush(stdout);
    
    // قراءة إعدادات المشروع
    // Try Arabic name first, then English fallback
    fs::path configPath = fs::path(options.projectPath) / "project.sad";
    printf("Looking for config: %s\n", configPath.string().c_str());
    fflush(stdout);
    
    if (!fs::exists(configPath)) {
        // Try Arabic name
        configPath = fs::path(options.projectPath) / "sad.config";
        printf("Trying fallback: %s\n", configPath.string().c_str());
        fflush(stdout);
    }
    
    ProjectConfig config = parseProjectConfig(configPath);
    printf("Config loaded, building...\n");
    fflush(stdout);
    
    // البناء
    ApkBuilder builder(options, config);
    if (!builder.build()) {
        return 1;
    }
    
    return 0;
}
