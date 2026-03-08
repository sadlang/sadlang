// بسم الله الرحمن الرحيم
// compiler_driver_ui.cpp - معالج أوامر UI لـ sadc
// Handles: sadc ui generate [platform] <file.ص>
// =============================================================================

#include "compiler_driver.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iomanip>

// SadUI Pipeline (يدعم تحليل .ص وتوليد كود أصلي)
#include "ui/sad_ui_pipeline.hpp"

#ifdef _WIN32
#include <windows.h>
#include <algorithm>
#endif

namespace fs = std::filesystem;

namespace sad {
namespace driver {

// ═══════════════════════════════════════════════════════════════════════════
// Stub للوظيفة القديمة
// ═══════════════════════════════════════════════════════════════════════════

bool CompilerDriver::run_ui_pipeline(const std::string& input_file,
                                      const std::string& ui_platform_str) {
    (void)input_file;
    (void)ui_platform_str;
    std::cerr << "[sadc] Use 'sadc ui generate' command instead." << std::endl;
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════
// ألوان UI
// ═══════════════════════════════════════════════════════════════════════════

struct UIColor {
    uint8_t r = 0, g = 0, b = 0;
    float a = 1.0f;
    
    UIColor() = default;
    UIColor(uint8_t r, uint8_t g, uint8_t b, float a = 1.0f)
        : r(r), g(g), b(b), a(a) {}
    
    static UIColor fromHex(const std::string& hex) {
        std::string h = hex;
        if (!h.empty() && h[0] == '#') h = h.substr(1);
        UIColor c;
        if (h.size() >= 6) {
            c.r = static_cast<uint8_t>(std::stoi(h.substr(0, 2), nullptr, 16));
            c.g = static_cast<uint8_t>(std::stoi(h.substr(2, 2), nullptr, 16));
            c.b = static_cast<uint8_t>(std::stoi(h.substr(4, 2), nullptr, 16));
        }
        return c;
    }
    
    std::string toComposeColor() const {
        std::ostringstream ss;
        ss << "Color(0xFF" 
           << std::hex << std::uppercase
           << std::setfill('0') << std::setw(2) << (int)r
           << std::setfill('0') << std::setw(2) << (int)g
           << std::setfill('0') << std::setw(2) << (int)b
           << ")";
        return ss.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════
// توليد كود Kotlin Compose من الصفر
// ═══════════════════════════════════════════════════════════════════════════

std::string generateMainActivity(const std::string& packageName, const std::string& appName) {
    std::ostringstream ss;
    
    ss << "package " << packageName << "\n\n";
    ss << "import android.os.Bundle\n";
    ss << "import androidx.activity.ComponentActivity\n";
    ss << "import androidx.activity.compose.setContent\n";
    ss << "import androidx.compose.foundation.layout.*\n";
    ss << "import androidx.compose.material3.*\n";
    ss << "import androidx.compose.runtime.*\n";
    ss << "import androidx.compose.ui.Alignment\n";
    ss << "import androidx.compose.ui.Modifier\n";
    ss << "import androidx.compose.ui.graphics.Color\n";
    ss << "import androidx.compose.ui.text.font.FontWeight\n";
    ss << "import androidx.compose.ui.text.style.TextAlign\n";
    ss << "import androidx.compose.ui.unit.dp\n";
    ss << "import androidx.compose.ui.unit.sp\n";
    ss << "import " << packageName << ".ui.theme.SadAppTheme\n";
    ss << "import " << packageName << ".R\n";
    ss << "import androidx.compose.ui.res.stringResource\n\n";
    
    ss << "class MainActivity : ComponentActivity() {\n";
    ss << "    override fun onCreate(savedInstanceState: Bundle?) {\n";
    ss << "        super.onCreate(savedInstanceState)\n";
    ss << "        setContent {\n";
    ss << "            SadAppTheme {\n";
    ss << "                MainScreen()\n";
    ss << "            }\n";
    ss << "        }\n";
    ss << "    }\n";
    ss << "}\n\n";
    
    // MainScreen Composable
    ss << "@OptIn(ExperimentalMaterial3Api::class)\n";
    ss << "@Composable\n";
    ss << "fun MainScreen() {\n";
    ss << "    var counter by remember { mutableIntStateOf(0) }\n";
    ss << "    var message by remember { mutableStateOf(stringResource(R.string.welcome_message)) }\n\n";
    
    ss << "    Scaffold(\n";
    ss << "        topBar = {\n";
    ss << "            TopAppBar(\n";
    ss << "                title = { Text(stringResource(R.string.app_name)) },\n";
    ss << "                colors = TopAppBarDefaults.topAppBarColors(\n";
    ss << "                    containerColor = MaterialTheme.colorScheme.primary,\n";
    ss << "                    titleContentColor = Color.White\n";
    ss << "                )\n";
    ss << "            )\n";
    ss << "        },\n";
    ss << "        floatingActionButton = {\n";
    ss << "            FloatingActionButton(\n";
    ss << "                onClick = { /* Share action */ }\n";
    ss << "            ) {\n";
    ss << "                Text(\"\\uD83D\\uDCE4\")\n";  // 📤 emoji
    ss << "            }\n";
    ss << "        }\n";
    ss << "    ) { padding ->\n";
    ss << "        Column(\n";
    ss << "            modifier = Modifier\n";
    ss << "                .fillMaxSize()\n";
    ss << "                .padding(padding)\n";
    ss << "                .padding(16.dp),\n";
    ss << "            horizontalAlignment = Alignment.CenterHorizontally\n";
    ss << "        ) {\n";
    
    // نص الترحيب
    ss << "            Text(\n";
    ss << "                text = stringResource(R.string.welcome_message),\n";
    ss << "                fontSize = 24.sp,\n";
    ss << "                fontWeight = FontWeight.Bold,\n";
    ss << "                color = MaterialTheme.colorScheme.primary\n";
    ss << "            )\n\n";
    
    ss << "            Spacer(modifier = Modifier.height(16.dp))\n\n";
    
    // بطاقة معلومات
    ss << "            Card(\n";
    ss << "                modifier = Modifier.fillMaxWidth()\n";
    ss << "            ) {\n";
    ss << "                Column(\n";
    ss << "                    modifier = Modifier.padding(16.dp)\n";
    ss << "                ) {\n";
    ss << "                    Text(\n";
    ss << "                        text = stringResource(R.string.sad_description),\n";
    ss << "                        fontSize = 18.sp,\n";
    ss << "                        fontWeight = FontWeight.SemiBold\n";
    ss << "                    )\n";
    ss << "                    Spacer(modifier = Modifier.height(8.dp))\n";
    ss << "                    Text(\n";
    ss << "                        text = stringResource(R.string.sad_subtitle),\n";
    ss << "                        fontSize = 14.sp,\n";
    ss << "                        color = Color.Gray\n";
    ss << "                    )\n";
    ss << "                }\n";
    ss << "            }\n\n";
    
    ss << "            Spacer(modifier = Modifier.height(32.dp))\n\n";
    
    // العداد
    ss << "            Row(\n";
    ss << "                horizontalArrangement = Arrangement.spacedBy(16.dp),\n";
    ss << "                verticalAlignment = Alignment.CenterVertically\n";
    ss << "            ) {\n";
    ss << "                IconButton(onClick = { counter-- }) {\n";
    ss << "                    Text(\"-\", fontSize = 24.sp, fontWeight = FontWeight.Bold)\n";
    ss << "                }\n";
    ss << "                Text(\n";
    ss << "                    text = counter.toString(),\n";
    ss << "                    fontSize = 48.sp,\n";
    ss << "                    fontWeight = FontWeight.Bold,\n";
    ss << "                    textAlign = TextAlign.Center,\n";
    ss << "                    modifier = Modifier.widthIn(min = 80.dp)\n";
    ss << "                )\n";
    ss << "                IconButton(onClick = { counter++ }) {\n";
    ss << "                    Text(\"+\", fontSize = 24.sp, fontWeight = FontWeight.Bold)\n";
    ss << "                }\n";
    ss << "            }\n\n";
    
    ss << "            Spacer(modifier = Modifier.height(16.dp))\n\n";
    
    // زر إعادة تعيين
    ss << "            Button(\n";
    ss << "                onClick = { counter = 0 },\n";
    ss << "                colors = ButtonDefaults.buttonColors(\n";
    ss << "                    containerColor = Color(0xFFFF9800)\n";
    ss << "                )\n";
    ss << "            ) {\n";
    ss << "                Text(stringResource(R.string.reset_button))\n";
    ss << "            }\n\n";
    
    ss << "            Spacer(modifier = Modifier.weight(1f))\n\n";
    
    // حقل إدخال
    ss << "            OutlinedTextField(\n";
    ss << "                value = message,\n";
    ss << "                onValueChange = { message = it },\n";
    ss << "                label = { Text(stringResource(R.string.input_hint)) },\n";
    ss << "                modifier = Modifier.fillMaxWidth()\n";
    ss << "            )\n\n";
    
    ss << "            Spacer(modifier = Modifier.height(8.dp))\n\n";
    
    // عرض الرسالة
    ss << "            Text(\n";
    ss << "                text = message,\n";
    ss << "                fontSize = 16.sp,\n";
    ss << "                color = Color.Gray,\n";
    ss << "                textAlign = TextAlign.Center\n";
    ss << "            )\n";
    
    ss << "        }\n";
    ss << "    }\n";
    ss << "}\n";
    
    return ss.str();
}

std::string generateTheme(const std::string& packageName) {
    std::ostringstream ss;
    
    ss << "package " << packageName << ".ui.theme\n\n";
    ss << "import androidx.compose.foundation.isSystemInDarkTheme\n";
    ss << "import androidx.compose.material3.*\n";
    ss << "import androidx.compose.runtime.Composable\n";
    ss << "import androidx.compose.ui.graphics.Color\n\n";
    
    ss << "private val LightColorScheme = lightColorScheme(\n";
    ss << "    primary = Color(0xFF2196F3),\n";
    ss << "    secondary = Color(0xFFFF9800),\n";
    ss << "    tertiary = Color(0xFF4CAF50),\n";
    ss << "    background = Color(0xFFFAFAFA),\n";
    ss << "    surface = Color.White,\n";
    ss << "    error = Color(0xFFF44336)\n";
    ss << ")\n\n";
    
    ss << "private val DarkColorScheme = darkColorScheme(\n";
    ss << "    primary = Color(0xFF90CAF9),\n";
    ss << "    secondary = Color(0xFFFFCC80),\n";
    ss << "    tertiary = Color(0xFFA5D6A7),\n";
    ss << "    background = Color(0xFF121212),\n";
    ss << "    surface = Color(0xFF1E1E1E),\n";
    ss << "    error = Color(0xFFEF9A9A)\n";
    ss << ")\n\n";
    
    ss << "@Composable\n";
    ss << "fun SadAppTheme(\n";
    ss << "    darkTheme: Boolean = isSystemInDarkTheme(),\n";
    ss << "    content: @Composable () -> Unit\n";
    ss << ") {\n";
    ss << "    val colorScheme = if (darkTheme) DarkColorScheme else LightColorScheme\n";
    ss << "    MaterialTheme(\n";
    ss << "        colorScheme = colorScheme,\n";
    ss << "        content = content\n";
    ss << "    )\n";
    ss << "}\n";
    
    return ss.str();
}

std::string generateBuildGradleKts(const std::string& packageName) {
    std::ostringstream ss;
    
    ss << "plugins {\n";
    ss << "    alias(libs.plugins.android.application)\n";
    ss << "    alias(libs.plugins.kotlin.android)\n";
    ss << "    alias(libs.plugins.kotlin.compose)\n";
    ss << "}\n\n";
    
    ss << "android {\n";
    ss << "    namespace = \"" << packageName << "\"\n";
    ss << "    compileSdk = 34\n\n";
    
    ss << "    defaultConfig {\n";
    ss << "        applicationId = \"" << packageName << "\"\n";
    ss << "        minSdk = 26\n";
    ss << "        targetSdk = 34\n";
    ss << "        versionCode = 1\n";
    ss << "        versionName = \"1.0.0\"\n";
    ss << "    }\n\n";
    
    ss << "    buildTypes {\n";
    ss << "        release {\n";
    ss << "            isMinifyEnabled = true\n";
    ss << "            proguardFiles(\n";
    ss << "                getDefaultProguardFile(\"proguard-android-optimize.txt\"),\n";
    ss << "                \"proguard-rules.pro\"\n";
    ss << "            )\n";
    ss << "        }\n";
    ss << "    }\n\n";
    
    ss << "    compileOptions {\n";
    ss << "        sourceCompatibility = JavaVersion.VERSION_17\n";
    ss << "        targetCompatibility = JavaVersion.VERSION_17\n";
    ss << "    }\n";
    
    ss << "    kotlinOptions {\n";
    ss << "        jvmTarget = \"17\"\n";
    ss << "    }\n\n";
    
    ss << "    buildFeatures {\n";
    ss << "        compose = true\n";
    ss << "    }\n";
    ss << "}\n\n";
    
    ss << "dependencies {\n";
    ss << "    implementation(libs.androidx.core.ktx)\n";
    ss << "    implementation(libs.androidx.lifecycle.runtime.ktx)\n";
    ss << "    implementation(libs.androidx.activity.compose)\n";
    ss << "    implementation(platform(libs.androidx.compose.bom))\n";
    ss << "    implementation(libs.androidx.ui)\n";
    ss << "    implementation(libs.androidx.ui.graphics)\n";
    ss << "    implementation(libs.androidx.ui.tooling.preview)\n";
    ss << "    implementation(libs.androidx.material3)\n";
    ss << "}\n";
    
    return ss.str();
}

std::string generateVersionCatalog() {
    return R"([versions]
agp = "8.5.2"
kotlin = "2.0.0"
coreKtx = "1.13.1"
lifecycleRuntimeKtx = "2.8.4"
activityCompose = "1.9.1"
composeBom = "2024.08.00"

[libraries]
androidx-core-ktx = { group = "androidx.core", name = "core-ktx", version.ref = "coreKtx" }
androidx-lifecycle-runtime-ktx = { group = "androidx.lifecycle", name = "lifecycle-runtime-ktx", version.ref = "lifecycleRuntimeKtx" }
androidx-activity-compose = { group = "androidx.activity", name = "activity-compose", version.ref = "activityCompose" }
androidx-compose-bom = { group = "androidx.compose", name = "compose-bom", version.ref = "composeBom" }
androidx-ui = { group = "androidx.compose.ui", name = "ui" }
androidx-ui-graphics = { group = "androidx.compose.ui", name = "ui-graphics" }
androidx-ui-tooling-preview = { group = "androidx.compose.ui", name = "ui-tooling-preview" }
androidx-material3 = { group = "androidx.compose.material3", name = "material3" }

[plugins]
android-application = { id = "com.android.application", version.ref = "agp" }
kotlin-android = { id = "org.jetbrains.kotlin.android", version.ref = "kotlin" }
kotlin-compose = { id = "org.jetbrains.kotlin.plugin.compose", version.ref = "kotlin" }
)";
}

std::string generateSettingsGradleKts(const std::string& appName) {
    std::ostringstream ss;
    ss << "pluginManagement {\n";
    ss << "    repositories {\n";
    ss << "        google()\n";
    ss << "        mavenCentral()\n";
    ss << "        gradlePluginPortal()\n";
    ss << "    }\n";
    ss << "}\n\n";
    
    ss << "dependencyResolutionManagement {\n";
    ss << "    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)\n";
    ss << "    repositories {\n";
    ss << "        google()\n";
    ss << "        mavenCentral()\n";
    ss << "    }\n";
    ss << "}\n\n";
    
    ss << "rootProject.name = \"" << appName << "\"\n";
    ss << "include(\":app\")\n";
    
    return ss.str();
}

std::string generateManifest(const std::string& packageName) {
    std::ostringstream ss;
    
    ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    ss << "<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\">\n\n";
    
    ss << "    <application\n";
    ss << "        android:allowBackup=\"true\"\n";
    ss << "        android:icon=\"@mipmap/ic_launcher\"\n";
    ss << "        android:label=\"@string/app_name\"\n";
    ss << "        android:roundIcon=\"@mipmap/ic_launcher_round\"\n";
    ss << "        android:supportsRtl=\"true\"\n";
    ss << "        android:theme=\"@style/Theme.SadApp\">\n\n";
    
    ss << "        <activity\n";
    ss << "            android:name=\".MainActivity\"\n";
    ss << "            android:exported=\"true\"\n";
    ss << "            android:theme=\"@style/Theme.SadApp\">\n";
    ss << "            <intent-filter>\n";
    ss << "                <action android:name=\"android.intent.action.MAIN\" />\n";
    ss << "                <category android:name=\"android.intent.category.LAUNCHER\" />\n";
    ss << "            </intent-filter>\n";
    ss << "        </activity>\n\n";
    
    ss << "    </application>\n\n";
    ss << "</manifest>\n";
    
    return ss.str();
}

std::string generateStringsXml(const std::string& appName) {
    // استخدام raw string literal مع UTF-8 BOM عبر XML encoding declaration
    std::ostringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    ss << "<resources>\n";
    ss << "    <string name=\"app_name\">" << appName << "</string>\n";
    // UTF-8 encoded Arabic strings using XML entities or direct UTF-8
    ss << "    <string name=\"welcome_message\">" 
       << "\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7\xD9\x8B \xD8\xA8\xD9\x83 \xD9\x81\xD9\x8A \xD9\x84\xD8\xBA\xD8\xA9 \xD8\xB5!" 
       << "</string>\n";  // مرحباً بك في لغة ص!
    ss << "    <string name=\"sad_description\">"
       << "\xD9\x84\xD8\xBA\xD8\xA9 \xD8\xB5 \u2014 \xD9\x84\xD8\xBA\xD8\xA9 \xD8\xA7\xD9\x84\xD8\xA8\xD8\xB1\xD9\x85\xD8\xAC\xD8\xA9 \xD8\xA7\xD9\x84\xD8\xB9\xD8\xB1\xD8\xA8\xD9\x8A\xD8\xA9"
       << "</string>\n";  // لغة ص — لغة البرمجة العربية
    ss << "    <string name=\"sad_subtitle\">"
       << "\xD8\xA3\xD9\x88\xD9\x84 \xD9\x84\xD8\xBA\xD8\xA9 \xD8\xA8\xD8\xB1\xD9\x85\xD8\xAC\xD8\xA9 \xD8\xB9\xD8\xB1\xD8\xA8\xD9\x8A\xD8\xA9 \xD8\xAA\xD8\xAF\xD8\xB9\xD9\x85 \xD8\xA3\xD9\x86\xD8\xAF\xD8\xB1\xD9\x88\xD9\x8A\xD8\xAF \xD8\xA8\xD8\xA7\xD9\x84\xD9\x83\xD8\xA7\xD9\x85\xD9\x84"
       << "</string>\n";  // أول لغة برمجة عربية تدعم أندرويد بالكامل
    ss << "    <string name=\"reset_button\">"
       << "\xD8\xA5\xD8\xB9\xD8\xA7\xD8\xAF\xD8\xA9 \xD8\xAA\xD8\xB9\xD9\x8A\xD9\x8A\xD9\x86"
       << "</string>\n";  // إعادة تعيين
    ss << "    <string name=\"input_hint\">"
       << "\xD8\xA3\xD8\xAF\xD8\xAE\xD9\x84 \xD8\xB1\xD8\xB3\xD8\xA7\xD9\x84\xD8\xA9..."
       << "</string>\n";  // أدخل رسالة...
    ss << "</resources>\n";
    return ss.str();
}

std::string generateThemesXml() {
    return R"(<?xml version="1.0" encoding="utf-8"?>
<resources>
    <style name="Theme.SadApp" parent="android:Theme.Material.Light.NoActionBar">
        <item name="android:statusBarColor">@color/primary</item>
    </style>
</resources>
)";
}

std::string generateColorsXml() {
    return R"(<?xml version="1.0" encoding="utf-8"?>
<resources>
    <color name="primary">#2196F3</color>
    <color name="secondary">#FF9800</color>
</resources>
)";
}

std::string generateGradleWrapperProperties() {
    return R"(distributionBase=GRADLE_USER_HOME
distributionPath=wrapper/dists
distributionUrl=https\://services.gradle.org/distributions/gradle-8.7-bin.zip
zipStoreBase=GRADLE_USER_HOME
zipStorePath=wrapper/dists
)";
}

std::string generateRootBuildGradleKts() {
    return R"(// Top-level build file
plugins {
    alias(libs.plugins.android.application) apply false
    alias(libs.plugins.kotlin.android) apply false
    alias(libs.plugins.kotlin.compose) apply false
}
)";
}

std::string generateProguardRules() {
    return R"(# Sad App ProGuard Rules
-keepattributes *Annotation*
-keepattributes SourceFile,LineNumberTable
-keep public class * extends android.app.Activity
-keep public class * extends android.app.Application
)";
}

std::string generateGitignore() {
    return R"(# Gradle
.gradle/
build/
local.properties

# IDE
.idea/
*.iml

# Generated
app/release/
)";
}

// ═══════════════════════════════════════════════════════════════════════════
// طباعة المساعدة
// ═══════════════════════════════════════════════════════════════════════════

void printUIHelp() {
    std::cout << u8R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                      sadc ui - مولد واجهات لغة ص                             ║
╚══════════════════════════════════════════════════════════════════════════════╝

الاستخدام:
  sadc ui generate [منصة] [خيارات]
  sadc واجهة توليد [منصة] [خيارات]

المنصات المدعومة:
  android / أندرويد    - Jetpack Compose (Kotlin)

الخيارات:
  -o, --output <مجلد>      مجلد الإخراج (افتراضي: build_ui/)
  --name <اسم>             اسم التطبيق
  --package <حزمة>         اسم الحزمة (com.example.app)

أمثلة:
  sadc ui generate android -o my_app/
  sadc ui generate android --name "تطبيقي" --package com.sad.myapp
  sadc واجهة توليد أندرويد -o تجربة/

)" << std::endl;
}

// ═══════════════════════════════════════════════════════════════════════════
// معالج أمر UI الرئيسي
// ═══════════════════════════════════════════════════════════════════════════

int handleUICommand(int argc, char* argv[]) {
    if (argc < 3) {
        printUIHelp();
        return 1;
    }
    
    std::string subCommand = argv[2];
    
    if (subCommand == "generate" || subCommand == u8"توليد") {
        if (argc < 4) {
            std::cerr << u8"خطأ: يجب تحديد المنصة المستهدفة\n" << std::endl;
            printUIHelp();
            return 1;
        }
        
        std::string platformStr = argv[3];
        
        if (platformStr != "android" && platformStr != u8"أندرويد" && platformStr != u8"اندرويد") {
            std::cerr << u8"خطأ: حالياً فقط android مدعوم\n";
            return 1;
        }
        
        // تحليل الخيارات
        std::string outputDir = "build_ui";
        std::string appName = u8"تطبيق ص";
        std::string packageName = "com.sad.app";
        std::string inputFile = "";  // ملف .ص اختياري
        
        for (int i = 4; i < argc; ++i) {
            std::string arg = argv[i];
            
            if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
                outputDir = argv[++i];
            } else if (arg == "--name" && i + 1 < argc) {
                appName = argv[++i];
            } else if (arg == "--package" && i + 1 < argc) {
                packageName = argv[++i];
            } else if (arg.size() > 3 && (arg.substr(arg.size()-3) == u8".ص" || 
                       arg.find(".sad") != std::string::npos)) {
                // ملف مصدر .ص (حرف ص = 2 بايت UTF-8 + نقطة = 3 بايت)
                inputFile = arg;
            }
        }
        
        std::cout << u8"\n═══════════════════════════════════════════════════════════════\n";
        std::cout << u8"  مولد واجهات لغة ص — Jetpack Compose\n";
        std::cout << u8"═══════════════════════════════════════════════════════════════\n\n";
        
        std::cout << u8"اسم التطبيق: " << appName << "\n";
        std::cout << u8"اسم الحزمة: " << packageName << "\n";
        std::cout << u8"مجلد الإخراج: " << outputDir << "\n";
        if (!inputFile.empty()) {
            std::cout << u8"ملف المصدر: " << inputFile << "\n";
        }
        std::cout << "\n";
        std::cout << u8"اسم الحزمة: " << packageName << "\n";
        std::cout << u8"مجلد الإخراج: " << outputDir << "\n\n";
        
        try {
            fs::path outPath = fs::u8path(outputDir);
            std::string pkgPath = packageName;
            std::replace(pkgPath.begin(), pkgPath.end(), '.', '/');
            
            // إنشاء المجلدات الأساسية
            fs::create_directories(outPath / "app/src/main/java" / pkgPath / "ui/theme");
            fs::create_directories(outPath / "app/src/main/res/values");
            fs::create_directories(outPath / "app/src/main/res/mipmap-hdpi");
            fs::create_directories(outPath / "gradle/wrapper");
            
            // ═══════════════════════════════════════════════════════════════
            // إذا كان هناك ملف مصدر .ص، استخدم SadUIPipeline
            // ═══════════════════════════════════════════════════════════════
            std::string mainActivityContent;
            
            if (!inputFile.empty()) {
                // قراءة ملف المصدر
                std::ifstream srcFile(fs::u8path(inputFile), std::ios::binary);
                if (!srcFile) {
                    std::cerr << u8"خطأ: فشل في قراءة " << inputFile << std::endl;
                    return 1;
                }
                std::ostringstream srcStream;
                srcStream << srcFile.rdbuf();
                std::string sourceCode = srcStream.str();
                srcFile.close();
                
                std::cout << u8"  ◇ تحليل كود الواجهة...\n";
                
                // استخدام SadUIPipeline
                using namespace sad::ui::ir;
                
                SadUIPipeline pipeline;
                PipelineConfig config;
                config.platform = TargetPlatform::Android;
                config.validate = true;
                config.optimize = true;
                config.applyTheme = true;
                config.verbose = true;
                pipeline.setConfig(config);
                
                auto result = pipeline.buildFromSource(sourceCode, appName);
                
                // طباعة سجل التقدم
                for (const auto& logMsg : result.log) {
                    std::cout << u8"    • " << logMsg << std::endl;
                }
                
                if (!result.success) {
                    std::cerr << u8"\n❌ فشل التحليل/التوليد:\n";
                    for (const auto& err : result.codeGenResult.errors) {
                        std::cerr << u8"  - " << err << std::endl;
                    }
                    return 1;
                }
                
                std::cout << u8"  ✓ تم التحليل بنجاح\n\n";
                
                // استخراج MainActivity.kt من نتيجة Pipeline
                bool foundMainActivity = false;
                for (const auto& genFile : result.codeGenResult.files) {
                    if (genFile.path.find("MainActivity") != std::string::npos ||
                        genFile.path.find("Screen") != std::string::npos) {
                        mainActivityContent = genFile.content;
                        foundMainActivity = true;
                        break;
                    }
                }
                
                // إذا لم يُعثر على MainActivity، استخدم أول ملف Kotlin
                if (!foundMainActivity && !result.codeGenResult.files.empty()) {
                    for (const auto& genFile : result.codeGenResult.files) {
                        if (genFile.path.find(".kt") != std::string::npos) {
                            mainActivityContent = genFile.content;
                            foundMainActivity = true;
                            break;
                        }
                    }
                }
                
                // إذا لم يُعثر على أي Kotlin، استخدم التوليد الافتراضي
                if (!foundMainActivity) {
                    std::cout << u8"  ⚠ لم يُعثر على Composable، استخدام القالب الافتراضي\n";
                    mainActivityContent = generateMainActivity(packageName, appName);
                } else {
                    // استبدال com.sad.app بالـ packageName الذي أدخله المستخدم
                    size_t pos = 0;
                    while ((pos = mainActivityContent.find("com.sad.app", pos)) != std::string::npos) {
                        mainActivityContent.replace(pos, 11, packageName);
                        pos += packageName.size();
                    }
                }
                
            } else {
                // لا يوجد ملف مصدر، توليد افتراضي
                mainActivityContent = generateMainActivity(packageName, appName);
            }
            
            // كتابة الملفات
            std::vector<std::pair<std::string, std::string>> files = {
                {"app/src/main/java/" + pkgPath + "/MainActivity.kt", mainActivityContent},
                {"app/src/main/java/" + pkgPath + "/ui/theme/Theme.kt", generateTheme(packageName)},
                {"app/build.gradle.kts", generateBuildGradleKts(packageName)},
                {"build.gradle.kts", generateRootBuildGradleKts()},
                {"settings.gradle.kts", generateSettingsGradleKts(appName)},
                {"gradle/libs.versions.toml", generateVersionCatalog()},
                {"gradle/wrapper/gradle-wrapper.properties", generateGradleWrapperProperties()},
                {"app/src/main/AndroidManifest.xml", generateManifest(packageName)},
                {"app/src/main/res/values/strings.xml", generateStringsXml(appName)},
                {"app/src/main/res/values/themes.xml", generateThemesXml()},
                {"app/src/main/res/values/colors.xml", generateColorsXml()},
                {"app/proguard-rules.pro", generateProguardRules()},
                {".gitignore", generateGitignore()}
            };
            
            for (const auto& [path, content] : files) {
                fs::path filePath = outPath / path;
                fs::create_directories(filePath.parent_path());
                
                std::ofstream out(filePath, std::ios::binary);
                if (!out) {
                    std::cerr << u8"خطأ: فشل في كتابة " << path << std::endl;
                    return 1;
                }
                out.write(content.c_str(), content.size());
                out.close();
                
                std::cout << u8"  ✓ " << path << std::endl;
            }
            
            std::cout << u8"\n═══════════════════════════════════════════════════════════════\n";
            std::cout << u8"  ✓ تم التوليد بنجاح!\n";
            std::cout << u8"═══════════════════════════════════════════════════════════════\n\n";
            
            std::cout << u8"الخطوات التالية:\n";
            std::cout << u8"  1. افتح المشروع في Android Studio\n";
            std::cout << u8"  2. انتظر مزامنة Gradle\n";
            std::cout << u8"  3. اضغط Run (Shift+F10)\n\n";
            
            return 0;
            
        } catch (const std::exception& e) {
            std::cerr << u8"خطأ: " << e.what() << std::endl;
            return 1;
        }
    }
    
    std::cerr << u8"خطأ: أمر غير معروف: " << subCommand << std::endl;
    printUIHelp();
    return 1;
}

} // namespace driver
} // namespace sad
