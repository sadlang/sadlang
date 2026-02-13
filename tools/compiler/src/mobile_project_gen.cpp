// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file mobile_project_gen.cpp
 * @brief تنفيذ مولّد مشاريع الهاتف
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *              تنفيذ مولّد مشاريع تطبيقات الهاتف بلغة ص
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#include "mobile_project_gen.hpp"
#include <fstream>
#include <sstream>

namespace Sad {
namespace Tools {
namespace Mobile {

// ═══════════════════════════════════════════════════════════════════════════
//                    تنفيذ MobileProjectConfig
// ═══════════════════════════════════════════════════════════════════════════

std::optional<std::string> MobileProjectConfig::validate() const {
    if (metadata.name.empty()) {
        return "اسم المشروع مطلوب";
    }
    
    if (metadata.packageId.empty()) {
        return "معرف الحزمة (package ID) مطلوب";
    }
    
    // التحقق من صيغة معرف الحزمة
    if (metadata.packageId.find('.') == std::string::npos) {
        return "معرف الحزمة يجب أن يحتوي على نقطة (مثل: com.example.app)";
    }
    
    if (outputDirectory.empty()) {
        return "مجلد الإخراج مطلوب";
    }
    
    return std::nullopt;
}

// ═══════════════════════════════════════════════════════════════════════════
//                    تنفيذ ProjectGenerationResult
// ═══════════════════════════════════════════════════════════════════════════

std::string ProjectGenerationResult::toArabicSummary() const {
    std::ostringstream summary;
    
    if (success) {
        summary << "✅ تم إنشاء المشروع بنجاح!\n\n";
        summary << "📁 مسار المشروع: " << projectPath.string() << "\n\n";
        summary << "📄 الملفات المُنشأة: " << files.size() << " ملف\n";
        
        // تصنيف الملفات
        int dirs = 0, sources = 0, configs = 0, others = 0;
        for (const auto& f : files) {
            if (f.isDirectory) dirs++;
            else {
                std::string ext = f.relativePath.extension().string();
                if (ext == ".ص" || ext == ".swift" || ext == ".kt" || ext == ".java" || ext == ".cpp")
                    sources++;
                else if (ext == ".xml" || ext == ".json" || ext == ".gradle" || ext == ".plist")
                    configs++;
                else
                    others++;
            }
        }
        
        summary << "   • مجلدات: " << dirs << "\n";
        summary << "   • ملفات مصدرية: " << sources << "\n";
        summary << "   • ملفات إعدادات: " << configs << "\n";
        summary << "   • ملفات أخرى: " << others << "\n";
        
        if (!nextSteps.empty()) {
            summary << "\n📋 الخطوات التالية:\n" << nextSteps;
        }
    } else {
        summary << "❌ فشل إنشاء المشروع!\n\n";
        for (const auto& error : errors) {
            summary << "   ⛔ " << error << "\n";
        }
    }
    
    if (!warnings.empty()) {
        summary << "\n⚠️ تحذيرات:\n";
        for (const auto& w : warnings) {
            summary << "   ⚡ " << w << "\n";
        }
    }
    
    return summary.str();
}

// ═══════════════════════════════════════════════════════════════════════════
//                    تنفيذ MobileProjectGenerator
// ═══════════════════════════════════════════════════════════════════════════

struct MobileProjectGenerator::Impl {
    ProgressCallback progressCallback;
};

MobileProjectGenerator::MobileProjectGenerator() : pImpl_(std::make_unique<Impl>()) {}
MobileProjectGenerator::~MobileProjectGenerator() = default;

ProjectGenerationResult MobileProjectGenerator::generate(const MobileProjectConfig& config) {
    ProjectGenerationResult result;
    
    // التحقق من صحة الإعدادات
    if (auto error = config.validate()) {
        result.errors.push_back(*error);
        return result;
    }
    
    if (pImpl_->progressCallback) {
        pImpl_->progressCallback("تحليل الإعدادات", 5);
    }
    
    std::vector<GeneratedFile> allFiles;
    
    // توليد الكود المشترك
    if (pImpl_->progressCallback) {
        pImpl_->progressCallback("توليد الكود المشترك", 20);
    }
    auto sharedFiles = generateSharedCode(config);
    allFiles.insert(allFiles.end(), sharedFiles.begin(), sharedFiles.end());
    
    // توليد Android
    if (config.platform == MobilePlatform::Android || 
        config.platform == MobilePlatform::Both ||
        config.platform == MobilePlatform::CrossPlatform) {
        
        if (pImpl_->progressCallback) {
            pImpl_->progressCallback("توليد مشروع أندرويد", 40);
        }
        auto androidFiles = generateAndroidStructure(config);
        allFiles.insert(allFiles.end(), androidFiles.begin(), androidFiles.end());
    }
    
    // توليد iOS
    if (config.platform == MobilePlatform::iOS || 
        config.platform == MobilePlatform::Both ||
        config.platform == MobilePlatform::CrossPlatform) {
        
        if (pImpl_->progressCallback) {
            pImpl_->progressCallback("توليد مشروع iOS", 60);
        }
        auto iosFiles = generateiOSStructure(config);
        allFiles.insert(allFiles.end(), iosFiles.begin(), iosFiles.end());
    }
    
    // إنشاء الملفات على القرص
    if (pImpl_->progressCallback) {
        pImpl_->progressCallback("كتابة الملفات", 80);
    }
    
    result.projectPath = config.outputDirectory / config.metadata.name;
    createDirectoryStructure(result.projectPath, allFiles);
    writeFiles(result.projectPath, allFiles);
    
    result.files = allFiles;
    result.success = true;
    
    // الخطوات التالية
    std::ostringstream steps;
    steps << "1. افتح المشروع في المحرر المناسب:\n";
    
    if (config.platform == MobilePlatform::Android || config.platform == MobilePlatform::Both) {
        steps << "   • أندرويد: افتح '" << config.metadata.name << "/android' في Android Studio\n";
    }
    if (config.platform == MobilePlatform::iOS || config.platform == MobilePlatform::Both) {
        steps << "   • iOS: افتح '" << config.metadata.name << "/ios' في Xcode\n";
    }
    
    steps << "2. قم بتثبيت التبعيات:\n";
    if (config.platform == MobilePlatform::Android || config.platform == MobilePlatform::Both) {
        steps << "   • أندرويد: سيقوم Gradle بذلك تلقائياً\n";
    }
    if (config.platform == MobilePlatform::iOS || config.platform == MobilePlatform::Both) {
        steps << "   • iOS: pod install (إذا كنت تستخدم CocoaPods)\n";
    }
    
    steps << "3. شغّل التطبيق على المحاكي أو الجهاز\n";
    steps << "4. ابدأ التطوير في ملفات ص في مجلد 'shared/'\n";
    
    result.nextSteps = steps.str();
    
    if (pImpl_->progressCallback) {
        pImpl_->progressCallback("اكتمل", 100);
    }
    
    return result;
}

std::vector<GeneratedFile> MobileProjectGenerator::generateAndroidStructure(
    const MobileProjectConfig& config) {
    
    std::vector<GeneratedFile> files;
    std::string pkgPath = config.metadata.packageId;
    std::replace(pkgPath.begin(), pkgPath.end(), '.', '/');
    
    // المجلدات
    files.push_back({"android/app/src/main/java/" + pkgPath, "", true});
    files.push_back({"android/app/src/main/cpp", "", true});
    files.push_back({"android/app/src/main/res/layout", "", true});
    files.push_back({"android/app/src/main/res/values", "", true});
    files.push_back({"android/app/src/main/res/drawable", "", true});
    
    // build.gradle (root)
    files.push_back({"android/build.gradle", generateRootBuildGradle(config), false});
    
    // build.gradle (app)
    files.push_back({"android/app/build.gradle", generateAppBuildGradle(config), false});
    
    // settings.gradle
    std::ostringstream settings;
    settings << "// إعدادات المشروع - مُولَّد بواسطة لغة ص\n";
    settings << "pluginManagement {\n";
    settings << "    repositories {\n";
    settings << "        google()\n";
    settings << "        mavenCentral()\n";
    settings << "        gradlePluginPortal()\n";
    settings << "    }\n";
    settings << "}\n";
    settings << "dependencyResolutionManagement {\n";
    settings << "    repositories {\n";
    settings << "        google()\n";
    settings << "        mavenCentral()\n";
    settings << "    }\n";
    settings << "}\n\n";
    settings << "rootProject.name = \"" << config.metadata.name << "\"\n";
    settings << "include ':app'\n";
    files.push_back({"android/settings.gradle", settings.str(), false});
    
    // AndroidManifest.xml
    files.push_back({"android/app/src/main/AndroidManifest.xml", 
                     generateAndroidManifest(config), false});
    
    // MainActivity (Kotlin)
    if (config.useKotlin) {
        std::ostringstream mainActivity;
        mainActivity << "// ╔════════════════════════════════════════════════════════════════════╗\n";
        mainActivity << "// ║    " << config.metadata.name << " - مُولَّد تلقائياً بواسطة لغة ص             ║\n";
        mainActivity << "// ╚════════════════════════════════════════════════════════════════════╝\n\n";
        mainActivity << "package " << config.metadata.packageId << "\n\n";
        mainActivity << "import android.os.Bundle\n";
        mainActivity << "import androidx.activity.ComponentActivity\n";
        mainActivity << "import androidx.activity.compose.setContent\n";
        mainActivity << "import androidx.compose.foundation.layout.*\n";
        mainActivity << "import androidx.compose.material3.*\n";
        mainActivity << "import androidx.compose.runtime.*\n";
        mainActivity << "import androidx.compose.ui.Alignment\n";
        mainActivity << "import androidx.compose.ui.Modifier\n";
        mainActivity << "import androidx.compose.ui.text.style.TextAlign\n";
        mainActivity << "import androidx.compose.ui.unit.dp\n";
        mainActivity << "import androidx.compose.ui.unit.sp\n\n";
        
        mainActivity << "class MainActivity : ComponentActivity() {\n";
        mainActivity << "    override fun onCreate(savedInstanceState: Bundle?) {\n";
        mainActivity << "        super.onCreate(savedInstanceState)\n";
        mainActivity << "        setContent {\n";
        mainActivity << "            MaterialTheme {\n";
        mainActivity << "                MainScreen()\n";
        mainActivity << "            }\n";
        mainActivity << "        }\n";
        mainActivity << "    }\n";
        mainActivity << "}\n\n";
        
        mainActivity << "@Composable\n";
        mainActivity << "fun MainScreen() {\n";
        mainActivity << "    var counter by remember { mutableIntStateOf(0) }\n\n";
        mainActivity << "    Surface(\n";
        mainActivity << "        modifier = Modifier.fillMaxSize(),\n";
        mainActivity << "        color = MaterialTheme.colorScheme.background\n";
        mainActivity << "    ) {\n";
        mainActivity << "        Column(\n";
        mainActivity << "            modifier = Modifier\n";
        mainActivity << "                .fillMaxSize()\n";
        mainActivity << "                .padding(24.dp),\n";
        mainActivity << "            verticalArrangement = Arrangement.Center,\n";
        mainActivity << "            horizontalAlignment = Alignment.CenterHorizontally\n";
        mainActivity << "        ) {\n";
        mainActivity << "            Text(\n";
        mainActivity << "                text = \"\\uD83C\\uDF1F مرحباً من لغة ص!\",\n";
        mainActivity << "                fontSize = 28.sp,\n";
        mainActivity << "                textAlign = TextAlign.Center,\n";
        mainActivity << "                color = MaterialTheme.colorScheme.primary\n";
        mainActivity << "            )\n";
        mainActivity << "            Spacer(modifier = Modifier.height(16.dp))\n";
        mainActivity << "            Text(\n";
        mainActivity << "                text = \"العداد: $counter\",\n";
        mainActivity << "                fontSize = 22.sp\n";
        mainActivity << "            )\n";
        mainActivity << "            Spacer(modifier = Modifier.height(24.dp))\n";
        mainActivity << "            Button(onClick = { counter++ }) {\n";
        mainActivity << "                Text(text = \"+1\", fontSize = 18.sp)\n";
        mainActivity << "            }\n";
        mainActivity << "            Spacer(modifier = Modifier.height(12.dp))\n";
        mainActivity << "            OutlinedButton(onClick = { counter = 0 }) {\n";
        mainActivity << "                Text(text = \"إعادة تعيين\", fontSize = 16.sp)\n";
        mainActivity << "            }\n";
        mainActivity << "        }\n";
        mainActivity << "    }\n";
        mainActivity << "}\n";
        
        files.push_back({"android/app/src/main/java/" + pkgPath + "/MainActivity.kt", 
                         mainActivity.str(), false});
    }
    
    // strings.xml (مع دعم العربية)
    std::ostringstream strings;
    strings << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    strings << "<resources>\n";
    strings << "    <string name=\"app_name\">" << config.metadata.arabicName << "</string>\n";
    strings << "</resources>\n";
    files.push_back({"android/app/src/main/res/values/strings.xml", strings.str(), false});
    
    // colors.xml
    std::ostringstream colors;
    colors << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    colors << "<resources>\n";
    colors << "    <color name=\"primary\">#1976D2</color>\n";
    colors << "    <color name=\"primary_dark\">#1565C0</color>\n";
    colors << "    <color name=\"accent\">#FF5722</color>\n";
    colors << "</resources>\n";
    files.push_back({"android/app/src/main/res/values/colors.xml", colors.str(), false});
    
    // themes.xml
    std::ostringstream themes;
    themes << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    themes << "<resources>\n";
    themes << "    <style name=\"AppTheme\" parent=\"android:Theme.Material.Light.NoActionBar\" />\n";
    themes << "</resources>\n";
    files.push_back({"android/app/src/main/res/values/themes.xml", themes.str(), false});
    
    // CMakeLists.txt للكود الأصلي
    std::ostringstream cmake;
    cmake << "# ╔════════════════════════════════════════════════════════════════════╗\n";
    cmake << "# ║     CMakeLists.txt للكود الأصلي - مُولَّد تلقائياً بواسطة لغة ص      ║\n";
    cmake << "# ╚════════════════════════════════════════════════════════════════════╝\n\n";
    cmake << "cmake_minimum_required(VERSION 3.22.1)\n";
    cmake << "project(\"" << config.metadata.name << "\")\n\n";
    cmake << "add_library(sad_app SHARED\n";
    cmake << "    sad_bridge.cpp\n";
    cmake << ")\n\n";
    cmake << "target_link_libraries(sad_app\n";
    cmake << "    android\n";
    cmake << "    log\n";
    cmake << ")\n";
    files.push_back({"android/app/src/main/cpp/CMakeLists.txt", cmake.str(), false});
    
    // جسر JNI
    std::ostringstream jniBridge;
    jniBridge << "// ╔════════════════════════════════════════════════════════════════════╗\n";
    jniBridge << "// ║       جسر JNI للربط بين Java والكود الأصلي من لغة ص               ║\n";
    jniBridge << "// ╚════════════════════════════════════════════════════════════════════╝\n\n";
    jniBridge << "#include <jni.h>\n";
    jniBridge << "#include <string>\n";
    jniBridge << "#include <android/log.h>\n\n";
    jniBridge << "#define LOG_TAG \"SadApp\"\n";
    jniBridge << "#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)\n\n";
    jniBridge << "extern \"C\" {\n\n";
    jniBridge << "// دالة تهيئة runtime لغة ص\n";
    jniBridge << "JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {\n";
    jniBridge << "    LOGI(\"تم تحميل مكتبة لغة ص\");\n";
    jniBridge << "    return JNI_VERSION_1_6;\n";
    jniBridge << "}\n\n";
    jniBridge << "} // extern \"C\"\n";
    files.push_back({"android/app/src/main/cpp/sad_bridge.cpp", jniBridge.str(), false});
    
    // gradle.properties
    std::ostringstream gradleProps;
    gradleProps << "# خصائص Gradle - مُولَّد بواسطة لغة ص\n";
    gradleProps << "org.gradle.jvmargs=-Xmx2048m\n";
    gradleProps << "org.gradle.parallel=true\n";
    gradleProps << "android.useAndroidX=true\n";
    gradleProps << "kotlin.code.style=official\n";
    files.push_back({"android/gradle.properties", gradleProps.str(), false});
    
    return files;
}

std::vector<GeneratedFile> MobileProjectGenerator::generateiOSStructure(
    const MobileProjectConfig& config) {
    
    std::vector<GeneratedFile> files;
    
    // المجلدات
    files.push_back({"ios/" + config.metadata.name, "", true});
    files.push_back({"ios/" + config.metadata.name + ".xcodeproj", "", true});
    files.push_back({"ios/" + config.metadata.name + "/Assets.xcassets", "", true});
    
    // Info.plist
    files.push_back({"ios/" + config.metadata.name + "/Info.plist", 
                     generateInfoPlist(config), false});
    
    // App.swift
    std::ostringstream appSwift;
    appSwift << "// ╔════════════════════════════════════════════════════════════════════╗\n";
    appSwift << "// ║    " << config.metadata.name << " - مُولَّد تلقائياً بواسطة لغة ص             ║\n";
    appSwift << "// ╚════════════════════════════════════════════════════════════════════╝\n\n";
    appSwift << "import SwiftUI\n\n";
    appSwift << "@main\n";
    appSwift << "struct " << config.metadata.name << "App: App {\n";
    appSwift << "    var body: some Scene {\n";
    appSwift << "        WindowGroup {\n";
    appSwift << "            ContentView()\n";
    appSwift << "                .environment(\\.layoutDirection, .rightToLeft)\n";
    appSwift << "        }\n";
    appSwift << "    }\n";
    appSwift << "}\n";
    files.push_back({"ios/" + config.metadata.name + "/" + config.metadata.name + "App.swift", 
                     appSwift.str(), false});
    
    // ContentView.swift
    std::ostringstream contentView;
    contentView << "// الواجهة الرئيسية\n";
    contentView << "import SwiftUI\n\n";
    contentView << "struct ContentView: View {\n";
    contentView << "    var body: some View {\n";
    contentView << "        NavigationView {\n";
    contentView << "            VStack(spacing: 20) {\n";
    contentView << "                Text(\"مرحباً من لغة ص!\")\n";
    contentView << "                    .font(.largeTitle)\n";
    contentView << "                    .fontWeight(.bold)\n";
    contentView << "                \n";
    contentView << "                Text(\"" << config.metadata.arabicName << "\")\n";
    contentView << "                    .font(.title2)\n";
    contentView << "                    .foregroundColor(.secondary)\n";
    contentView << "            }\n";
    contentView << "            .padding()\n";
    contentView << "            .navigationTitle(\"" << config.metadata.arabicName << "\")\n";
    contentView << "        }\n";
    contentView << "    }\n";
    contentView << "}\n\n";
    contentView << "#Preview {\n";
    contentView << "    ContentView()\n";
    contentView << "}\n";
    files.push_back({"ios/" + config.metadata.name + "/ContentView.swift", 
                     contentView.str(), false});
    
    return files;
}

std::vector<GeneratedFile> MobileProjectGenerator::generateSharedCode(
    const MobileProjectConfig& config) {
    
    std::vector<GeneratedFile> files;
    
    // مجلد الكود المشترك
    files.push_back({"shared/src", "", true});
    files.push_back({"shared/tests", "", true});
    
    // ملف ص الرئيسي
    std::ostringstream mainSad;
    mainSad << "// ╔════════════════════════════════════════════════════════════════════╗\n";
    mainSad << "// ║    " << config.metadata.arabicName << " - الكود المشترك                              ║\n";
    mainSad << "// ║    مُولَّد تلقائياً بواسطة مولّد مشاريع لغة ص                        ║\n";
    mainSad << "// ╚════════════════════════════════════════════════════════════════════╝\n\n";
    
    mainSad << "// استيراد المكتبات الأساسية\n";
    mainSad << "استورد أساسي\n";
    mainSad << "استورد واجهة\n\n";
    
    mainSad << "// ═══════════════════════════════════════════════════════════════════════\n";
    mainSad << "//                    نموذج البيانات\n";
    mainSad << "// ═══════════════════════════════════════════════════════════════════════\n\n";
    
    mainSad << "/// نموذج بيانات التطبيق الرئيسي\n";
    mainSad << "بنية حالة_التطبيق {\n";
    mainSad << "    عنوان: نص = \"" << config.metadata.arabicName << "\"\n";
    mainSad << "    محمّل: منطقي = خطأ\n";
    mainSad << "}\n\n";
    
    mainSad << "// ═══════════════════════════════════════════════════════════════════════\n";
    mainSad << "//                    منطق التطبيق\n";
    mainSad << "// ═══════════════════════════════════════════════════════════════════════\n\n";
    
    mainSad << "/// دالة التهيئة الرئيسية\n";
    mainSad << "دالة هيئ() {\n";
    mainSad << "    اطبع(\"مرحباً من لغة ص!\")\n";
    mainSad << "}\n\n";
    
    mainSad << "/// الدالة الرئيسية\n";
    mainSad << "دالة رئيسي() {\n";
    mainSad << "    هيئ()\n";
    mainSad << "}\n";
    
    files.push_back({"shared/src/main.sad", mainSad.str(), false});
    
    // ملف الإعدادات
    std::ostringstream configSad;
    configSad << "// إعدادات التطبيق\n\n";
    configSad << "ثابت اسم_التطبيق = \"" << config.metadata.arabicName << "\"\n";
    configSad << "ثابت معرف_الحزمة = \"" << config.metadata.packageId << "\"\n";
    configSad << "ثابت الإصدار = \"" << config.metadata.version << "\"\n";
    files.push_back({"shared/src/config.sad", configSad.str(), false});
    
    // README.md
    files.push_back({"README.md", generateReadme(config), false});
    
    // .gitignore
    if (config.includeGitIgnore) {
        files.push_back({".gitignore", generateGitIgnore(config), false});
    }
    
    return files;
}

std::string MobileProjectGenerator::generateRootBuildGradle(const MobileProjectConfig& config) {
    std::ostringstream gradle;
    
    gradle << "// ╔════════════════════════════════════════════════════════════════════╗\n";
    gradle << "// ║    build.gradle الرئيسي - مُولَّد تلقائياً بواسطة لغة ص             ║\n";
    gradle << "// ╚════════════════════════════════════════════════════════════════════╝\n\n";
    
    gradle << "plugins {\n";
    gradle << "    id 'com.android.application' version '8.2.0' apply false\n";
    if (config.useKotlin) {
        gradle << "    id 'org.jetbrains.kotlin.android' version '1.9.21' apply false\n";
    }
    gradle << "}\n";
    
    return gradle.str();
}

std::string MobileProjectGenerator::generateAppBuildGradle(const MobileProjectConfig& config) {
    std::ostringstream gradle;
    
    gradle << "// ╔════════════════════════════════════════════════════════════════════╗\n";
    gradle << "// ║    build.gradle للتطبيق - مُولَّد تلقائياً بواسطة لغة ص            ║\n";
    gradle << "// ╚════════════════════════════════════════════════════════════════════╝\n\n";
    
    gradle << "plugins {\n";
    gradle << "    id 'com.android.application'\n";
    if (config.useKotlin) {
        gradle << "    id 'org.jetbrains.kotlin.android'\n";
    }
    gradle << "}\n\n";
    
    gradle << "android {\n";
    gradle << "    namespace '" << config.metadata.packageId << "'\n";
    gradle << "    compileSdk " << config.androidTargetSdk << "\n\n";
    
    gradle << "    defaultConfig {\n";
    gradle << "        applicationId \"" << config.metadata.packageId << "\"\n";
    gradle << "        minSdk " << config.androidMinSdk << "\n";
    gradle << "        targetSdk " << config.androidTargetSdk << "\n";
    gradle << "        versionCode 1\n";
    gradle << "        versionName \"" << config.metadata.version << "\"\n";
    gradle << "    }\n\n";
    
    gradle << "    compileOptions {\n";
    gradle << "        sourceCompatibility JavaVersion.VERSION_17\n";
    gradle << "        targetCompatibility JavaVersion.VERSION_17\n";
    gradle << "    }\n\n";
    
    if (config.useKotlin) {
        gradle << "    kotlinOptions {\n";
        gradle << "        jvmTarget = '17'\n";
        gradle << "    }\n\n";
    }
    
    gradle << "    buildTypes {\n";
    gradle << "        release {\n";
    gradle << "            minifyEnabled false\n";
    gradle << "        }\n";
    gradle << "    }\n\n";
    
    if (config.uiFramework == UIFramework::JetpackCompose) {
        gradle << "    buildFeatures {\n";
        gradle << "        compose true\n";
        gradle << "    }\n";
        gradle << "    composeOptions {\n";
        gradle << "        kotlinCompilerExtensionVersion '1.5.6'\n";
        gradle << "    }\n";
    }
    
    gradle << "}\n\n";
    
    gradle << "dependencies {\n";
    gradle << "    implementation 'androidx.core:core-ktx:1.12.0'\n";
    
    if (config.uiFramework == UIFramework::JetpackCompose) {
        gradle << "    implementation 'androidx.activity:activity-compose:1.8.2'\n";
        gradle << "    implementation platform('androidx.compose:compose-bom:2024.01.00')\n";
        gradle << "    implementation 'androidx.compose.ui:ui'\n";
        gradle << "    implementation 'androidx.compose.foundation:foundation'\n";
        gradle << "    implementation 'androidx.compose.material3:material3'\n";
        gradle << "    implementation 'androidx.compose.ui:ui-tooling-preview'\n";
        gradle << "    debugImplementation 'androidx.compose.ui:ui-tooling'\n";
    }
    
    gradle << "}\n";
    
    return gradle.str();
}

std::string MobileProjectGenerator::generateAndroidManifest(const MobileProjectConfig& config) {
    std::ostringstream manifest;
    
    manifest << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    manifest << "<!-- AndroidManifest.xml - مُولَّد تلقائياً بواسطة لغة ص -->\n";
    manifest << "<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\">\n\n";
    
    // الأذونات
    if (config.features.useNetworking) {
        manifest << "    <uses-permission android:name=\"android.permission.INTERNET\" />\n";
    }
    if (config.features.useCamera) {
        manifest << "    <uses-permission android:name=\"android.permission.CAMERA\" />\n";
    }
    if (config.features.useLocation) {
        manifest << "    <uses-permission android:name=\"android.permission.ACCESS_FINE_LOCATION\" />\n";
    }
    
    manifest << "\n    <application\n";
    manifest << "        android:label=\"" << config.metadata.arabicName << "\"\n";
    manifest << "        android:supportsRtl=\"" << (config.features.useRTLSupport ? "true" : "false") << "\"\n";
    manifest << "        android:theme=\"@style/AppTheme\">\n\n";
    
    manifest << "        <activity\n";
    manifest << "            android:name=\"." << "MainActivity\"\n";
    manifest << "            android:exported=\"true\">\n";
    manifest << "            <intent-filter>\n";
    manifest << "                <action android:name=\"android.intent.action.MAIN\" />\n";
    manifest << "                <category android:name=\"android.intent.category.LAUNCHER\" />\n";
    manifest << "            </intent-filter>\n";
    manifest << "        </activity>\n\n";
    
    manifest << "    </application>\n";
    manifest << "</manifest>\n";
    
    return manifest.str();
}

std::string MobileProjectGenerator::generateInfoPlist(const MobileProjectConfig& config) {
    std::ostringstream plist;
    
    plist << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    plist << "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n";
    plist << "<!-- Info.plist - مُولَّد تلقائياً بواسطة لغة ص -->\n";
    plist << "<plist version=\"1.0\">\n";
    plist << "<dict>\n";
    
    plist << "    <key>CFBundleIdentifier</key>\n";
    plist << "    <string>" << config.metadata.packageId << "</string>\n";
    
    plist << "    <key>CFBundleName</key>\n";
    plist << "    <string>" << config.metadata.name << "</string>\n";
    
    plist << "    <key>CFBundleDisplayName</key>\n";
    plist << "    <string>" << config.metadata.arabicName << "</string>\n";
    
    plist << "    <key>CFBundleVersion</key>\n";
    plist << "    <string>" << config.metadata.version << "</string>\n";
    
    plist << "    <key>MinimumOSVersion</key>\n";
    plist << "    <string>" << config.iosMinVersion << "</string>\n";
    
    if (config.features.useRTLSupport) {
        plist << "    <key>CFBundleDevelopmentRegion</key>\n";
        plist << "    <string>ar</string>\n";
    }
    
    // أوصاف الأذونات بالعربية
    if (config.features.useCamera) {
        plist << "    <key>NSCameraUsageDescription</key>\n";
        plist << "    <string>يحتاج التطبيق للوصول إلى الكاميرا</string>\n";
    }
    
    if (config.features.useLocation) {
        plist << "    <key>NSLocationWhenInUseUsageDescription</key>\n";
        plist << "    <string>يحتاج التطبيق لمعرفة موقعك</string>\n";
    }
    
    plist << "</dict>\n";
    plist << "</plist>\n";
    
    return plist.str();
}

std::string MobileProjectGenerator::generateReadme(const MobileProjectConfig& config) {
    std::ostringstream readme;
    
    readme << "# " << config.metadata.arabicName << "\n\n";
    readme << "> " << config.metadata.name << " - " << config.metadata.description << "\n\n";
    
    readme << "## نظرة عامة\n\n";
    readme << "مشروع تطبيق هاتف مُنشأ بلغة ص.\n\n";
    
    readme << "## البنية\n\n";
    readme << "```\n";
    readme << config.metadata.name << "/\n";
    readme << "├── shared/           # الكود المشترك بلغة ص\n";
    readme << "│   ├── src/          # ملفات المصدر\n";
    readme << "│   └── tests/        # الاختبارات\n";
    
    if (config.platform == MobilePlatform::Android || config.platform == MobilePlatform::Both) {
        readme << "├── android/          # مشروع أندرويد\n";
    }
    if (config.platform == MobilePlatform::iOS || config.platform == MobilePlatform::Both) {
        readme << "├── ios/              # مشروع iOS\n";
    }
    readme << "└── README.md\n";
    readme << "```\n\n";
    
    readme << "## البدء\n\n";
    readme << "### المتطلبات\n\n";
    readme << "- مترجم لغة ص (الإصدار 1.0 أو أحدث)\n";
    
    if (config.platform == MobilePlatform::Android || config.platform == MobilePlatform::Both) {
        readme << "- Android Studio (للأندرويد)\n";
        readme << "- Android SDK " << config.androidMinSdk << "+\n";
    }
    if (config.platform == MobilePlatform::iOS || config.platform == MobilePlatform::Both) {
        readme << "- Xcode 15+ (لـ iOS)\n";
        readme << "- macOS 13+\n";
    }
    
    readme << "\n### التشغيل\n\n";
    readme << "```bash\n";
    readme << "# بناء المشروع\n";
    readme << "ص بناء\n\n";
    
    if (config.platform == MobilePlatform::Android || config.platform == MobilePlatform::Both) {
        readme << "# تشغيل على أندرويد\n";
        readme << "ص هاتف شغّل --اندرويد\n\n";
    }
    if (config.platform == MobilePlatform::iOS || config.platform == MobilePlatform::Both) {
        readme << "# تشغيل على iOS (يتطلب Mac)\n";
        readme << "ص هاتف شغّل --آيفون\n";
    }
    readme << "```\n\n";
    
    readme << "## الترخيص\n\n";
    readme << "جميع الحقوق محفوظة © 2026\n";
    
    return readme.str();
}

std::string MobileProjectGenerator::generateGitIgnore(const MobileProjectConfig& config) {
    std::ostringstream gitignore;
    
    gitignore << "# ملفات مُولَّدة بواسطة لغة ص\n";
    gitignore << "*.o\n";
    gitignore << "*.so\n";
    gitignore << "*.dylib\n";
    gitignore << "build/\n\n";
    
    if (config.platform == MobilePlatform::Android || config.platform == MobilePlatform::Both) {
        gitignore << "# أندرويد\n";
        gitignore << "android/.gradle/\n";
        gitignore << "android/build/\n";
        gitignore << "android/app/build/\n";
        gitignore << "android/local.properties\n";
        gitignore << "*.apk\n";
        gitignore << "*.aab\n\n";
    }
    
    if (config.platform == MobilePlatform::iOS || config.platform == MobilePlatform::Both) {
        gitignore << "# iOS\n";
        gitignore << "ios/DerivedData/\n";
        gitignore << "ios/build/\n";
        gitignore << "*.xcuserstate\n";
        gitignore << "*.ipa\n";
        gitignore << "Pods/\n\n";
    }
    
    gitignore << "# محرر\n";
    gitignore << ".vscode/\n";
    gitignore << ".idea/\n";
    gitignore << "*.swp\n";
    gitignore << ".DS_Store\n";
    
    return gitignore.str();
}

std::vector<std::pair<ProjectTemplate, std::string>> MobileProjectGenerator::getAvailableTemplates() {
    return {
        {ProjectTemplate::Empty, "Empty / فارغ"},
        {ProjectTemplate::HelloWorld, "Hello World / مرحباً بالعالم"},
        {ProjectTemplate::Navigation, "Navigation / متعدد الشاشات"},
        {ProjectTemplate::ListDetail, "List-Detail / قائمة وتفاصيل"},
        {ProjectTemplate::TabBased, "Tabs / أقسام"},
        {ProjectTemplate::Login, "Login / تسجيل دخول"},
        {ProjectTemplate::MapBased, "Map / خرائط"},
        {ProjectTemplate::CameraApp, "Camera / كاميرا"},
        {ProjectTemplate::ECommerce, "E-Commerce / تجارة إلكترونية"},
        {ProjectTemplate::Game2D, "2D Game / لعبة ثنائية"}
    };
}

std::string MobileProjectGenerator::getTemplateArabicDescription(ProjectTemplate templateType) {
    switch (templateType) {
        case ProjectTemplate::Empty:
            return "مشروع فارغ بالحد الأدنى من الملفات";
        case ProjectTemplate::HelloWorld:
            return "تطبيق بسيط يعرض 'مرحباً بالعالم'";
        case ProjectTemplate::Navigation:
            return "تطبيق متعدد الشاشات مع نظام تنقل";
        case ProjectTemplate::ListDetail:
            return "تطبيق قائمة مع شاشة تفاصيل";
        case ProjectTemplate::TabBased:
            return "تطبيق بأقسام (tabs) في الأسفل";
        case ProjectTemplate::Login:
            return "تطبيق مع شاشة تسجيل دخول ومصادقة";
        case ProjectTemplate::MapBased:
            return "تطبيق خرائط مع تحديد الموقع";
        case ProjectTemplate::CameraApp:
            return "تطبيق كاميرا مع معرض صور";
        case ProjectTemplate::ECommerce:
            return "تطبيق تجارة إلكترونية مع سلة مشتريات";
        case ProjectTemplate::Game2D:
            return "لعبة ثنائية الأبعاد بسيطة";
        default:
            return "قالب مخصص";
    }
}

void MobileProjectGenerator::setProgressCallback(ProgressCallback callback) {
    pImpl_->progressCallback = callback;
}

void MobileProjectGenerator::createDirectoryStructure(
    const std::filesystem::path& basePath,
    const std::vector<GeneratedFile>& files) {
    
    for (const auto& file : files) {
        if (file.isDirectory) {
            std::filesystem::create_directories(basePath / file.relativePath);
        } else {
            std::filesystem::create_directories((basePath / file.relativePath).parent_path());
        }
    }
}

void MobileProjectGenerator::writeFiles(
    const std::filesystem::path& basePath,
    const std::vector<GeneratedFile>& files) {
    
    for (const auto& file : files) {
        if (!file.isDirectory && !file.content.empty()) {
            std::ofstream out(basePath / file.relativePath);
            out << file.content;
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//                    تنفيذ QuickProjectBuilder
// ═══════════════════════════════════════════════════════════════════════════

QuickProjectBuilder::QuickProjectBuilder(const std::string& name) {
    config_.metadata.name = name;
    config_.metadata.arabicName = name;
    config_.platform = MobilePlatform::Both;
    config_.templateType = ProjectTemplate::HelloWorld;
    config_.uiFramework = UIFramework::SadUI;
}

QuickProjectBuilder QuickProjectBuilder::create(const std::string& name) {
    return QuickProjectBuilder(name);
}

QuickProjectBuilder& QuickProjectBuilder::withArabicName(const std::string& name) {
    config_.metadata.arabicName = name;
    return *this;
}

QuickProjectBuilder& QuickProjectBuilder::withPackageId(const std::string& packageId) {
    config_.metadata.packageId = packageId;
    return *this;
}

QuickProjectBuilder& QuickProjectBuilder::withPlatform(MobilePlatform platform) {
    config_.platform = platform;
    return *this;
}

QuickProjectBuilder& QuickProjectBuilder::withTemplate(ProjectTemplate templateType) {
    config_.templateType = templateType;
    return *this;
}

QuickProjectBuilder& QuickProjectBuilder::withUIFramework(UIFramework framework) {
    config_.uiFramework = framework;
    return *this;
}

QuickProjectBuilder& QuickProjectBuilder::withCamera() {
    config_.features.useCamera = true;
    return *this;
}

QuickProjectBuilder& QuickProjectBuilder::withLocation() {
    config_.features.useLocation = true;
    return *this;
}

QuickProjectBuilder& QuickProjectBuilder::withNotifications() {
    config_.features.useNotifications = true;
    return *this;
}

QuickProjectBuilder& QuickProjectBuilder::withDatabase() {
    config_.features.useDatabase = true;
    return *this;
}

QuickProjectBuilder& QuickProjectBuilder::withAuthentication() {
    config_.features.useAuthentication = true;
    return *this;
}

QuickProjectBuilder& QuickProjectBuilder::withAnalytics() {
    config_.features.useAnalytics = true;
    return *this;
}

QuickProjectBuilder& QuickProjectBuilder::addScreen(const ScreenDefinition& screen) {
    config_.screens.push_back(screen);
    return *this;
}

ProjectGenerationResult QuickProjectBuilder::build(const std::filesystem::path& outputDir) {
    config_.outputDirectory = outputDir;
    
    MobileProjectGenerator generator;
    return generator.generate(config_);
}

} // namespace Mobile
} // namespace Tools
} // namespace Sad
