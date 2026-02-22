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
    bool release = false;
    bool sign = false;
    bool verbose = false;
};

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
  sad build-apk <مسار_المشروع> [خيارات]

خيارات:
  -o, --output <مسار>    مسار ملف APK الناتج
  -n, --name <اسم>       اسم التطبيق
  -p, --package <حزمة>   اسم الحزمة (مثال: com.example.myapp)
  -i, --icon <مسار>      مسار أيقونة التطبيق (PNG)
  -r, --release          بناء نسخة Release
  -s, --sign             توقيع APK
  -v, --verbose          عرض تفاصيل البناء
  -h, --help             عرض هذه المساعدة

مثال:
  sad build-apk مشروعي/
  sad build-apk مشروعي/ -o مشروعي.apk -n "تطبيقي الأول"

هيكل المشروع المطلوب:
  مشروعي/
  ├── التطبيق.ص          # نقطة الدخول الرئيسية
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
        
        // 5. بناء APK باستخدام أدوات Android SDK
        if (!buildApk()) return false;
        
        // 6. توقيع APK (إذا مطلوب)
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
        
        for (const auto& file : sourceFiles) {
            fs::path relativePath = fs::relative(file, projectPath);
            fs::path destPath = buildDir_ / "assets" / relativePath;
            
            fs::create_directories(destPath.parent_path());
            fs::copy_file(file, destPath, fs::copy_options::overwrite_existing);
            
            if (options_.verbose) {
                std::cout << "   ← " << relativePath.string() << std::endl;
            }
        }
        
        std::cout << "   تم نسخ " << sourceFiles.size() << " ملف" << std::endl;
        
        return true;
    }
    
    bool generateManifest() {
        std::cout << "📝 إنشاء AndroidManifest.xml..." << std::endl;
        
        std::ostringstream manifest;
        manifest << R"(<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package=")" << config_.packageName << R"(">
    
    <uses-permission android:name="android.permission.INTERNET" />
    
    <application
        android:label=")" << config_.name << R"("
        android:icon="@mipmap/ic_launcher"
        android:theme="@style/SadTheme"
        android:supportsRtl="true"
        android:allowBackup="true">
        
        <activity
            android:name="com.sad.runtime.SadActivity"
            android:exported="true"
            android:screenOrientation="portrait">
            
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
    
    bool buildApk() {
        std::cout << "🔨 بناء APK..." << std::endl;
        
        // تحديد مسار APK الناتج
        if (options_.outputPath.empty()) {
            outputApkPath_ = fs::path(options_.projectPath).parent_path() / 
                             (config_.name + ".apk");
        } else {
            outputApkPath_ = options_.outputPath;
        }
        
        // ملاحظة: البناء الفعلي يتطلب Android SDK
        // هنا نعرض الأوامر المطلوبة
        
        std::cout << std::endl;
        std::cout << "⚠️  لإكمال البناء، تحتاج Android SDK. نفذ الأوامر التالية:" << std::endl;
        std::cout << std::endl;
        std::cout << "   # 1. ترجمة الموارد" << std::endl;
        std::cout << "   aapt2 compile -o " << (buildDir_ / "compiled").string() 
                  << " " << (buildDir_ / "res").string() << std::endl;
        std::cout << std::endl;
        std::cout << "   # 2. ربط وإنشاء APK" << std::endl;
        std::cout << "   aapt2 link -o " << outputApkPath_.string() 
                  << " --manifest " << (buildDir_ / "AndroidManifest.xml").string() << std::endl;
        std::cout << std::endl;
        std::cout << "   # 3. إضافة ملفات المصدر" << std::endl;
        std::cout << "   zip -r " << outputApkPath_.string() 
                  << " " << (buildDir_ / "assets").string() << std::endl;
        std::cout << std::endl;
        
        // للتجربة: إنشاء ملف وهمي
        writeFile(outputApkPath_, "# APK placeholder - requires Android SDK to build\n");
        
        return true;
    }
    
    bool signApk() {
        std::cout << "🔐 توقيع APK..." << std::endl;
        
        std::cout << "   # استخدم apksigner أو jarsigner لتوقيع APK" << std::endl;
        std::cout << "   apksigner sign --ks my.keystore " << outputApkPath_.string() << std::endl;
        
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
        } else if (arg == "-r" || arg == "--release") {
            options.release = true;
        } else if (arg == "-s" || arg == "--sign") {
            options.sign = true;
        } else if (arg == "-v" || arg == "--verbose") {
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
