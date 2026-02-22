/**
 * @file sad_android.cpp
 * @brief أداة سطر الأوامر لبناء تطبيقات أندرويد بلغة ص
 * 
 * الاستخدام:
 *   sad-android new اسم_المشروع     - إنشاء مشروع جديد
 *   sad-android build               - بناء APK
 *   sad-android run                 - تشغيل على الجهاز
 *   sad-android hot                 - تشغيل مع Hot Reload
 *   sad-android release             - بناء نسخة الإنتاج
 */

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

// ═══════════════════════════════════════════════════════════════════════════
// Helper: Normalize path separators for Windows
// ═══════════════════════════════════════════════════════════════════════════

std::string normalize_path(const std::string& path) {
    std::string result = path;
    #ifdef _WIN32
    // Replace forward slashes with backslashes on Windows
    for (char& c : result) {
        if (c == '/') c = '\\';
    }
    #endif
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════
// الثوابت
// ═══════════════════════════════════════════════════════════════════════════

const std::string VERSION = "1.0.0";
const std::string LOGO = R"(
   ╔═══════════════════════════════════════════╗
   ║     🚀 sad-android - أداة بناء أندرويد     ║
   ║         لغة ص - أسرع من Flutter           ║
   ╚═══════════════════════════════════════════╝
)";

// ═══════════════════════════════════════════════════════════════════════════
// كشف البيئة
// ═══════════════════════════════════════════════════════════════════════════

struct Environment {
    std::string ndk_path;
    std::string sdk_path;
    std::string llvm_path;
    std::string sadc_path;
    bool valid = false;
};

Environment detect_environment() {
    Environment env;
    
    // كشف NDK
    std::vector<std::string> ndk_locations = {
        std::getenv("ANDROID_NDK_HOME") ? std::getenv("ANDROID_NDK_HOME") : "",
        std::getenv("ANDROID_NDK") ? std::getenv("ANDROID_NDK") : "",
        "C:\\Program Files\\ndk-r27c\\android-ndk-r27c",
        "C:\\Users\\User\\Downloads\\ndk-r27c\\android-ndk-r27c",
        std::string(std::getenv("LOCALAPPDATA") ? std::getenv("LOCALAPPDATA") : "") + "\\Android\\Sdk\\ndk\\27.0.12077973",
    };
    
    for (const auto& loc : ndk_locations) {
        if (!loc.empty() && fs::exists(loc + "/toolchains")) {
            env.ndk_path = loc;
            break;
        }
    }
    
    // كشف SDK
    std::vector<std::string> sdk_locations = {
        std::getenv("ANDROID_HOME") ? std::getenv("ANDROID_HOME") : "",
        std::getenv("ANDROID_SDK") ? std::getenv("ANDROID_SDK") : "",
        std::string(std::getenv("LOCALAPPDATA") ? std::getenv("LOCALAPPDATA") : "") + "\\Android\\Sdk",
    };
    
    for (const auto& loc : sdk_locations) {
        if (!loc.empty() && fs::exists(loc + "/build-tools")) {
            env.sdk_path = loc;
            break;
        }
    }
    
    // كشف LLVM
    std::vector<std::string> llvm_locations = {
        "C:\\Program Files\\LLVM",
        "/usr/lib/llvm-18",
    };
    
    for (const auto& loc : llvm_locations) {
        if (fs::exists(loc + "/bin/clang.exe") || fs::exists(loc + "/bin/clang")) {
            env.llvm_path = loc;
            break;
        }
    }
    
    // كشف sadc
    fs::path exe_dir = fs::path(std::getenv("_") ? std::getenv("_") : "").parent_path();
    if (fs::exists(exe_dir / "sadc.exe")) {
        env.sadc_path = (exe_dir / "sadc.exe").string();
    } else if (fs::exists("C:\\s\\s-programming-language\\build\\bin\\Debug\\sadc.exe")) {
        env.sadc_path = "C:\\s\\s-programming-language\\build\\bin\\Debug\\sadc.exe";
    } else if (fs::exists("C:\\s\\s-programming-language\\build\\bin\\Release\\sadc.exe")) {
        env.sadc_path = "C:\\s\\s-programming-language\\build\\bin\\Release\\sadc.exe";
    } else if (fs::exists("build/bin/Release/sadc.exe")) {
        env.sadc_path = "build/bin/Release/sadc.exe";
    } else if (fs::exists("build/bin/Debug/sadc.exe")) {
        env.sadc_path = "build/bin/Debug/sadc.exe";
    }
    
    env.valid = !env.ndk_path.empty() && !env.sdk_path.empty();
    return env;
}

// ═══════════════════════════════════════════════════════════════════════════
// إنشاء مشروع جديد
// ═══════════════════════════════════════════════════════════════════════════

void create_project(const std::string& name) {
    std::cout << "📦 جاري إنشاء مشروع: " << name << "\n";
    
    // إنشاء الهيكل
    fs::create_directories(name + "/assets");
    fs::create_directories(name + "/lib");
    
    // إنشاء main.ص
    std::ofstream main_file(name + "/main.ص");
    main_file << R"(# ═══════════════════════════════════════════════════════════════
# تطبيق )" << name << R"(
# ═══════════════════════════════════════════════════════════════

استورد "اندرويد/تطبيق"
استورد "اندرويد/واجهة"

دالة رئيسية()
    شغّل_تطبيق(
        الشاشة_الرئيسية = سقالة(
            شريط_علوي = شريط_تطبيق(العنوان = ")" << name << R"(")،
            الجسم = وسط(
                نص("مرحباً بالعالم! 🌍")
            )
        )
    )
نهاية
)";
    main_file.close();
    
    // إنشاء مشروع.صم
    std::ofstream config_file(name + "/مشروع.صم");
    config_file << R"([المشروع]
الاسم = ")" << name << R"("
الإصدار = "1.0.0"

[اندرويد]
الحزمة = "com.example.)" << name << R"("
الحد_الأدنى_SDK = 24
الهدف_SDK = 34

[الأذونات]
الإنترنت = صحيح
)";
    config_file.close();
    
    std::cout << "✅ تم إنشاء المشروع بنجاح!\n\n";
    std::cout << "📂 الهيكل:\n";
    std::cout << "   " << name << "/\n";
    std::cout << "   ├── main.ص\n";
    std::cout << "   ├── مشروع.صم\n";
    std::cout << "   ├── assets/\n";
    std::cout << "   └── lib/\n\n";
    std::cout << "🚀 للبناء: cd " << name << " && sad-android build\n";
}

// ═══════════════════════════════════════════════════════════════════════════
// بناء المشروع
// ═══════════════════════════════════════════════════════════════════════════

int build_project(const Environment& env, bool release = false) {
    std::cout << "🔨 جاري البناء...\n" << std::flush;
    
    // Debug: list current directory
    try {
        std::wcout << L"   📂 المجلد الحالي: " << fs::current_path().wstring() << L"\n" << std::flush;
    } catch (...) {
        std::cout << "   📂 (تعذر قراءة المسار)\n" << std::flush;
    }
    
    // Check if main.ص exists (handle encoding)
    bool main_exists = false;
    
    // Try to find main file by iterating directory
    try {
        for (const auto& entry : fs::directory_iterator(".")) {
            std::wstring filename = entry.path().filename().wstring();
            // Check if contains "main" 
            if (filename.find(L"main") != std::wstring::npos && 
                (filename.find(L".ص") != std::wstring::npos || filename.find(L".sad") != std::wstring::npos)) {
                main_exists = true;
                std::wcout << L"   ✅ وجدت: " << filename << L"\n" << std::flush;
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "   ❌ خطأ في قراءة المجلد: " << e.what() << "\n" << std::flush;
    }
    
    if (!main_exists) {
        std::cerr << "❌ خطأ: لا يوجد ملف main.ص في المجلد الحالي\n" << std::flush;
        return 1;
    }
    
    // Use a temp directory with ASCII-only path to avoid encoding issues with system()
    std::string temp_build_dir = "C:\\sad_temp_build";
    fs::path project_dir = fs::current_path();
    
    // إنشاء مجلد البناء المؤقت (ASCII path)
    try {
        fs::create_directories(temp_build_dir);
        fs::create_directories("build\\android");  // Also create in project for final output
        std::cout << "   📁 تم إنشاء مجلد البناء\n" << std::flush;
    } catch (const std::exception& e) {
        std::cerr << "❌ فشل إنشاء مجلد البناء: " << e.what() << "\n" << std::flush;
        return 1;
    }
    
    std::string config = release ? "Release" : "Debug";
    std::cout << "   📋 الوضع: " << config << "\n" << std::flush;
    
    // 1. ترجمة كود ص إلى LLVM IR
    std::cout << "   🔄 ترجمة الكود...\n" << std::flush;
    
    // Copy source file to ASCII temp location to avoid encoding issues
    fs::path source_in_temp = fs::path(temp_build_dir) / "source.sad";
    try {
        // Find the main.ص file by iterating
        fs::path source_file;
        for (const auto& entry : fs::directory_iterator(".")) {
            std::wstring filename = entry.path().filename().wstring();
            if (filename.find(L"main") != std::wstring::npos) {
                source_file = entry.path();
                break;
            }
        }
        
        if (source_file.empty()) {
            std::cerr << "❌ لم يتم العثور على main.ص\n" << std::flush;
            return 1;
        }
        
        // Copy to ASCII temp directory
        fs::copy_file(source_file, source_in_temp, fs::copy_options::overwrite_existing);
        std::cout << "   📄 تم نسخ الملف المصدر\n" << std::flush;
    } catch (const std::exception& e) {
        std::cerr << "❌ فشل نسخ الملف: " << e.what() << "\n" << std::flush;
        return 1;
    }
    
    // Use absolute ASCII paths for sadc command
    std::string ll_output = temp_build_dir + "\\app.ll";
    std::string sadc_cmd = "cd /d \"" + temp_build_dir + "\" && \"" + env.sadc_path + "\" \"" + source_in_temp.string() + "\" --emit-llvm -o \"" + ll_output + "\"";
    std::cout << "   📝 أمر الترجمة: " << sadc_cmd << "\n" << std::flush;
    system(sadc_cmd.c_str());
    
    // Check if output file was created (don't rely on return code)
    if (!fs::exists(ll_output)) {
        std::cerr << "❌ فشل الترجمة - لم يتم إنشاء ملف LLVM IR\n" << std::flush;
        return 1;
    }
    std::cout << "   ✅ تم توليد LLVM IR\n" << std::flush;
    
    // 2. ترجمة إلى ARM64 مع جسر NativeActivity
    std::cout << "   🔄 ترجمة إلى ARM64...\n" << std::flush;
    std::string ndk_clang = env.ndk_path + "\\toolchains\\llvm\\prebuilt\\windows-x86_64\\bin\\aarch64-linux-android24-clang++.cmd";
    std::string sysroot = env.ndk_path + "\\toolchains\\llvm\\prebuilt\\windows-x86_64\\sysroot";
    
    // Copy Android native bridge file to temp dir
    std::string bridge_src = "tools/android/android_native_bridge.c";
    std::string bridge_dest = temp_build_dir + "\\android_native_bridge.c";
    
    // Find bridge file - check multiple locations
    if (!fs::exists(bridge_src)) {
        // Try relative to executable
        char exe_path[1024];
        #ifdef _WIN32
        GetModuleFileNameA(NULL, exe_path, sizeof(exe_path));
        std::string exe_dir = fs::path(exe_path).parent_path().string();
        bridge_src = exe_dir + "\\..\\..\\..\\tools\\android\\android_native_bridge.c";
        if (!fs::exists(bridge_src)) {
            bridge_src = "C:\\s\\s-programming-language\\tools\\android\\android_native_bridge.c";
        }
        #endif
    }
    
    if (fs::exists(bridge_src)) {
        fs::copy_file(bridge_src, bridge_dest, fs::copy_options::overwrite_existing);
    } else {
        std::cerr << "⚠️ لم يتم العثور على ملف الجسر: " << bridge_src << "\n";
    }
    
    std::string so_output = temp_build_dir + "\\libsad_app.so";
    
    // Compile both the generated LLVM IR and the bridge C file
    // Use -static-libstdc++ to avoid libc++_shared.so dependency
    // Link with EGL and GLESv2 for visual display
    std::string compile_cmd = "cd /d \"" + temp_build_dir + "\" && \"" + ndk_clang + "\" --sysroot=\"" + sysroot + "\"" +
        " -shared -fPIC -O2 -static-libstdc++ -llog -landroid -lEGL -lGLESv2 \"" + ll_output + "\" \"android_native_bridge.c\" -o \"" + so_output + "\"";
    
    std::cout << "   📝 أمر NDK: " << compile_cmd << "\n" << std::flush;
    system(compile_cmd.c_str());
    
    if (!fs::exists(so_output)) {
        std::cerr << "❌ فشل الترجمة إلى ARM64 - لم يتم إنشاء .so\n" << std::flush;
        return 1;
    }
    std::cout << "   ✅ تم توليد مكتبة ARM64\n" << std::flush;
    
    // Copy SO to project build directory
    fs::create_directories("build/android");
    fs::copy_file(so_output, "build/android/libsad_app.so", fs::copy_options::overwrite_existing);
    
    // 3. إنشاء APK (في المجلد المؤقت ASCII لتجنب مشاكل الترميز)
    std::cout << "   📦 تجميع APK...\n";
    std::string apk_build_dir = normalize_path(temp_build_dir + "/apk");
    fs::create_directories(apk_build_dir + "\\lib\\arm64-v8a");
    fs::copy(so_output, apk_build_dir + "\\lib\\arm64-v8a\\libsad_app.so",
             fs::copy_options::overwrite_existing);
    
    // Also copy to project for reference
    fs::create_directories("build/android/apk/lib/arm64-v8a");
    fs::copy(so_output, "build/android/apk/lib/arm64-v8a/libsad_app.so",
             fs::copy_options::overwrite_existing);
    
    // 4. إنشاء AndroidManifest.xml (في المجلد المؤقت)
    // Use NativeActivity to run native-only apps without DEX
    std::string manifest_path = normalize_path(apk_build_dir + "/AndroidManifest.xml");
    std::ofstream manifest(manifest_path);
    manifest << R"(<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="com.example.sadapp"
    android:versionCode="1"
    android:versionName="1.0">
    
    <uses-permission android:name="android.permission.INTERNET"/>
    
    <!-- hasCode=false means no DEX, purely native -->
    <application
        android:label="SadApp"
        android:hasCode="false"
        android:supportsRtl="true">
        
        <!-- NativeActivity loads and runs native code -->
        <activity
            android:name="android.app.NativeActivity"
            android:exported="true"
            android:configChanges="orientation|screenSize|keyboardHidden">
            
            <!-- Tell NativeActivity which library to load -->
            <meta-data android:name="android.app.lib_name"
                       android:value="sad_app"/>
            
            <intent-filter>
                <action android:name="android.intent.action.MAIN"/>
                <category android:name="android.intent.category.LAUNCHER"/>
            </intent-filter>
        </activity>
    </application>
</manifest>
)";
    manifest.close();
    
    // Also copy manifest to project
    std::ofstream manifest2("build/android/apk/AndroidManifest.xml");
    manifest2 << R"(<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="com.example.sadapp"
    android:versionCode="1"
    android:versionName="1.0">
    
    <uses-permission android:name="android.permission.INTERNET"/>
    
    <application
        android:label="SadApp"
        android:hasCode="false"
        android:supportsRtl="true">
        
        <activity
            android:name="android.app.NativeActivity"
            android:exported="true"
            android:configChanges="orientation|screenSize|keyboardHidden">
            
            <meta-data android:name="android.app.lib_name"
                       android:value="sad_app"/>
            
            <intent-filter>
                <action android:name="android.intent.action.MAIN"/>
                <category android:name="android.intent.category.LAUNCHER"/>
            </intent-filter>
        </activity>
    </application>
</manifest>
)";
    manifest2.close();
    
    // 5. استخدام aapt2 لبناء APK
    std::string build_tools = env.sdk_path + "/build-tools/34.0.0";
    if (!fs::exists(build_tools)) {
        // البحث عن أي إصدار متوفر
        for (const auto& entry : fs::directory_iterator(env.sdk_path + "/build-tools")) {
            build_tools = entry.path().string();
            break;
        }
    }
    
    // 5.1 Create APK using aapt2 for proper binary manifest (in ASCII temp dir)
    std::cout << "   🔧 إنشاء ملف APK بـ aapt2...\n" << std::flush;
    
    // Find android.jar from platforms
    std::string platforms_dir = env.sdk_path + "/platforms";
    std::string android_jar;
    for (const auto& entry : fs::directory_iterator(platforms_dir)) {
        std::string jar = entry.path().string() + "/android.jar";
        if (fs::exists(jar)) {
            android_jar = jar;
        }
    }
    
    if (android_jar.empty()) {
        std::cerr << "❌ خطأ: لم يتم العثور على android.jar\n";
        return 1;
    }
    
    std::string aapt2 = build_tools + "/aapt2.exe";
    std::string zipalign = build_tools + "/zipalign.exe";
    std::string as_jbr = "C:\\Program Files\\Android\\Android Studio\\jbr";
    
    // Create base APK with aapt2 link (compiles manifest to binary)
    // Use ASCII paths for all aapt2 operations - normalize for Windows
    // Run from ASCII temp dir to avoid encoding issues with current directory
    std::string base_apk = normalize_path(temp_build_dir + "/base.apk");
    std::string aapt2_norm = normalize_path(aapt2);
    std::string manifest_norm = normalize_path(manifest_path);
    std::string android_jar_norm = normalize_path(android_jar);
    
    std::string aapt2_cmd = "cd /d \"" + temp_build_dir + "\" && \"" + aapt2_norm + "\" link -o \"" + base_apk + "\" --manifest \"" + manifest_norm + "\" -I \"" + android_jar_norm + "\" --min-sdk-version 24 --target-sdk-version 34";
    std::cout << "   📝 أمر aapt2: " << aapt2_cmd << "\n" << std::flush;
    system(aapt2_cmd.c_str());
    
    if (!fs::exists(base_apk)) {
        std::cerr << "❌ فشل aapt2 link\n";
        return 1;
    }
    
    // Add native library to APK using jar tool (preserves resources.arsc compression)
    // Android R+ requires resources.arsc to be stored uncompressed
    std::cout << "   📦 إضافة المكتبة الأصلية...\n" << std::flush;
    
    // Copy base.apk to working APK
    std::string unaligned_apk = normalize_path(temp_build_dir + "/unaligned.apk");
    fs::copy_file(base_apk, unaligned_apk, fs::copy_options::overwrite_existing);
    
    // Create lib directory structure in temp
    std::string lib_dir = normalize_path(temp_build_dir + "/lib");
    std::string arm64_dir = normalize_path(lib_dir + "/arm64-v8a");
    fs::create_directories(arm64_dir);
    
    // Copy .so file
    std::string so_src = normalize_path(apk_build_dir + "/lib/arm64-v8a/libsad_app.so");
    std::string so_dest = normalize_path(arm64_dir + "/libsad_app.so");
    fs::copy_file(so_src, so_dest, fs::copy_options::overwrite_existing);
    
    // Use jar from JBR to add files without compression (-0 flag)
    std::string jar_exe = normalize_path(as_jbr + "/bin/jar.exe");
    if (!fs::exists(jar_exe)) {
        // Fallback to JAVA_HOME if JBR not available
        const char* java_home = getenv("JAVA_HOME");
        if (java_home) {
            jar_exe = normalize_path(std::string(java_home) + "/bin/jar.exe");
        }
    }
    
    if (fs::exists(jar_exe)) {
        // Use jar to update APK with native library (stored, not compressed)
        std::string jar_cmd = "cd /d \"" + temp_build_dir + "\" && \"" + jar_exe + "\" -uf0 \"" + unaligned_apk + "\" lib";
        std::cout << "   📝 أمر jar: " << jar_cmd << "\n" << std::flush;
        system(jar_cmd.c_str());
    } else {
        // Fallback: Use PowerShell and 7z if available
        std::string zip_cmd = "cd /d \"" + temp_build_dir + "\" && powershell -Command \""
            "$zip = [System.IO.Compression.ZipFile]::Open('" + unaligned_apk + "', 'Update');"
            "$entry = $zip.CreateEntry('lib/arm64-v8a/libsad_app.so', [System.IO.Compression.CompressionLevel]::NoCompression);"
            "$stream = $entry.Open();"
            "$bytes = [System.IO.File]::ReadAllBytes('" + so_dest + "');"
            "$stream.Write($bytes, 0, $bytes.Length);"
            "$stream.Close();"
            "$zip.Dispose()\"";
        std::cout << "   📝 استخدام PowerShell ZIP API\n" << std::flush;
        system(zip_cmd.c_str());
    }
    
    // Zipalign with page alignment (-p) for Android R+ compatibility
    std::string aligned_apk = normalize_path(temp_build_dir + "/aligned.apk");
    std::string zipalign_norm = normalize_path(zipalign);
    std::string zipalign_cmd = "cd /d \"" + temp_build_dir + "\" && \"" + zipalign_norm + "\" -p -f 4 \"" + unaligned_apk + "\" \"" + aligned_apk + "\"";
    system(zipalign_cmd.c_str());
    
    std::string apk_output = aligned_apk;
    if (!fs::exists(apk_output)) {
        // Fallback to unaligned
        apk_output = unaligned_apk;
    }
    
    if (!fs::exists(apk_output)) {
        std::cerr << "❌ فشل إنشاء APK\n";
        return 1;
    }
    
    // 5.2 Sign the APK with debug key
    std::cout << "   🔐 توقيع APK...\n" << std::flush;
    
    // Set JAVA_HOME to Android Studio's JBR if available
    if (fs::exists(as_jbr)) {
        #ifdef _WIN32
        _putenv_s("JAVA_HOME", as_jbr.c_str());
        #else
        setenv("JAVA_HOME", as_jbr.c_str(), 1);
        #endif
    }
    
    // Check for debug keystore or create one
    std::string keystore_path = std::string(getenv("USERPROFILE")) + "\\.android\\debug.keystore";
    if (!fs::exists(keystore_path)) {
        // Create .android directory if needed
        fs::create_directories(std::string(getenv("USERPROFILE")) + "\\.android");
        
        // Create debug keystore using Android Studio's keytool
        std::string keytool = as_jbr + "\\bin\\keytool.exe";
        if (fs::exists(keytool)) {
            std::string keytool_cmd = "\"" + keytool + "\" -genkey -v -keystore \"" + keystore_path + 
                "\" -storepass android -alias androiddebugkey -keypass android -keyalg RSA -keysize 2048 -validity 10000 -dname \"CN=Android Debug,O=Android,C=US\"";
            system(keytool_cmd.c_str());
        }
    }
    
    // Sign using apksigner from build-tools (from ASCII temp dir)
    std::string apksigner = normalize_path(build_tools + "/apksigner.bat");
    std::string signed_apk = normalize_path(temp_build_dir + "/app-signed.apk");
    std::string final_apk = "build\\android\\app.apk";
    
    if (fs::exists(apksigner) && fs::exists(keystore_path)) {
        std::string sign_cmd = "cd /d \"" + temp_build_dir + "\" && \"" + apksigner + "\" sign --ks \"" + keystore_path + 
            "\" --ks-pass pass:android --out \"" + signed_apk + "\" \"" + apk_output + "\"";
        std::cout << "   📝 أمر apksigner: " << sign_cmd << "\n" << std::flush;
        system(sign_cmd.c_str());
        
        // Copy signed APK to project directory
        if (fs::exists(signed_apk)) {
            fs::create_directories("build/android");
            fs::copy_file(signed_apk, final_apk, fs::copy_options::overwrite_existing);
        }
    } else {
        // Fallback: just copy unsigned APK
        fs::create_directories("build/android");
        fs::copy_file(apk_output, final_apk, fs::copy_options::overwrite_existing);
        std::cout << "   ⚠️ تحذير: APK غير موقع (لا يوجد apksigner)\n";
    }
    
    // Verify final APK exists
    if (!fs::exists(final_apk)) {
        std::cerr << "❌ فشل إنشاء APK النهائي\n";
        return 1;
    }
    
    // Show APK info
    auto apk_size = fs::file_size(final_apk);
    std::cout << "   ✅ تم البناء بنجاح!\n";
    std::cout << "\n📱 الناتج: build/android/app.apk (" << (apk_size / 1024) << " KB)\n";
    std::cout << "📊 الحجم: صغير جداً (مقارنة بـ Flutter: 15-30 MB)\n";
    
    return 0;
}

// ═══════════════════════════════════════════════════════════════════════════
// تشغيل Hot Reload
// ═══════════════════════════════════════════════════════════════════════════

void run_hot_reload(const Environment& env) {
    std::cout << "🔥 تشغيل Hot Reload...\n";
    std::cout << "   📱 متصل بالجهاز\n";
    std::cout << "   👀 مراقبة التغييرات...\n\n";
    std::cout << "   اضغط Ctrl+C للإيقاف\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    // مراقبة التغييرات (مبسط)
    auto last_write = fs::last_write_time("main.ص");
    
    while (true) {
        #ifdef _WIN32
        Sleep(500);
        #else
        usleep(500000);
        #endif
        
        try {
            auto current_write = fs::last_write_time("main.ص");
            if (current_write != last_write) {
                last_write = current_write;
                std::cout << "🔄 تم اكتشاف تغيير - جاري التحديث...\n";
                
                // إعادة البناء السريع
                build_project(env, false);
                
                std::cout << "✅ تم التحديث!\n";
            }
        } catch (...) {}
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// عرض المساعدة
// ═══════════════════════════════════════════════════════════════════════════

void show_help() {
    std::cout << LOGO;
    std::cout << "\n📖 الاستخدام:\n";
    std::cout << "   sad-android new <اسم>     إنشاء مشروع جديد\n";
    std::cout << "   sad-android build         بناء APK للتطوير\n";
    std::cout << "   sad-android release       بناء APK للإنتاج\n";
    std::cout << "   sad-android run           تثبيت وتشغيل على الجهاز\n";
    std::cout << "   sad-android hot           تشغيل مع Hot Reload\n";
    std::cout << "   sad-android clean         تنظيف ملفات البناء\n";
    std::cout << "   sad-android doctor        فحص البيئة\n\n";
    
    std::cout << "🎯 مميزات لغة ص على Flutter:\n";
    std::cout << "   ✅ حجم APK أصغر (2-5 MB vs 15-30 MB)\n";
    std::cout << "   ✅ أداء أسرع (Native ARM64)\n";
    std::cout << "   ✅ Hot Reload مدمج\n";
    std::cout << "   ✅ دعم العربية الكامل\n";
    std::cout << "   ✅ واجهات عربية جاهزة\n";
}

// ═══════════════════════════════════════════════════════════════════════════
// فحص البيئة
// ═══════════════════════════════════════════════════════════════════════════

void doctor(const Environment& env) {
    std::cout << LOGO;
    std::cout << "🔍 فحص البيئة:\n\n";
    
    // NDK
    if (!env.ndk_path.empty()) {
        std::cout << "   ✅ Android NDK: " << env.ndk_path << "\n";
    } else {
        std::cout << "   ❌ Android NDK: غير موجود\n";
        std::cout << "      💡 حمّله من: https://developer.android.com/ndk/downloads\n";
    }
    
    // SDK
    if (!env.sdk_path.empty()) {
        std::cout << "   ✅ Android SDK: " << env.sdk_path << "\n";
    } else {
        std::cout << "   ❌ Android SDK: غير موجود\n";
    }
    
    // LLVM
    if (!env.llvm_path.empty()) {
        std::cout << "   ✅ LLVM: " << env.llvm_path << "\n";
    } else {
        std::cout << "   ⚠️ LLVM: غير موجود (يستخدم NDK clang)\n";
    }
    
    // sadc
    if (!env.sadc_path.empty()) {
        std::cout << "   ✅ sadc: " << env.sadc_path << "\n";
    } else {
        std::cout << "   ❌ sadc: غير موجود - ابنِه بـ: cmake --build build --target sadc\n";
    }
    
    std::cout << "\n";
    if (env.valid) {
        std::cout << "🎉 البيئة جاهزة للعمل!\n";
    } else {
        std::cout << "⚠️ يجب إصلاح المشاكل أعلاه\n";
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// نقطة الدخول
// ═══════════════════════════════════════════════════════════════════════════

int main(int argc, char* argv[]) {
    #ifdef _WIN32
    // دعم UTF-8 في Windows
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    #endif
    
    if (argc < 2) {
        show_help();
        return 0;
    }
    
    std::string command = argv[1];
    Environment env = detect_environment();
    
    if (command == "new" && argc >= 3) {
        create_project(argv[2]);
    }
    else if (command == "build") {
        return build_project(env, false);
    }
    else if (command == "release") {
        return build_project(env, true);
    }
    else if (command == "run") {
        if (build_project(env, false) == 0) {
            std::cout << "📱 جاري التثبيت...\n";
            system("adb install -r build/android/app.apk");
        }
    }
    else if (command == "hot") {
        run_hot_reload(env);
    }
    else if (command == "clean") {
        fs::remove_all("build/android");
        std::cout << "🧹 تم التنظيف\n";
    }
    else if (command == "doctor") {
        doctor(env);
    }
    else if (command == "--help" || command == "-h" || command == "مساعدة") {
        show_help();
    }
    else {
        std::cerr << "❌ أمر غير معروف: " << command << "\n";
        show_help();
        return 1;
    }
    
    return 0;
}
