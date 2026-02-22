/**
 * ==========================================================================
 * ملف: android_compose_codegen.cpp
 * الوصف: تنفيذ مولد أندرويد — SadUI IR → Kotlin/Jetpack Compose
 * ==========================================================================
 * بسم الله الرحمن الرحيم
 */

#include "ui/android_compose_codegen.hpp"
#include <sstream>
#include <algorithm>
#include <iomanip>

namespace sad {
namespace ui {
namespace ir {

AndroidComposeCodeGen::AndroidComposeCodeGen(const AndroidCodeGenConfig& config)
    : config_(config) {}


// ═══════════════════════════════════════════════════════════════════════════
// توليد تطبيق كامل
// ═══════════════════════════════════════════════════════════════════════════

CodeGenResult AndroidComposeCodeGen::generate(const AppIR& app) {
    CodeGenResult result;
    result.success = true;

    std::string pkg = app.packageName;
    std::string pkgPath = pkg;
    std::replace(pkgPath.begin(), pkgPath.end(), '.', '/');
    std::string basePath = "app/src/main/java/" + pkgPath;

    ThemeIR theme = app.theme;

    // 1. شاشات Compose
    for (const auto& screen : app.screens) {
        GeneratedFile file;
        file.path = basePath + "/ui/" + screen.name + "Screen.kt";
        file.content = generateScreenKt(screen, pkg, theme);
        file.language = "kotlin";
        result.files.push_back(std::move(file));
    }

    // 2. MainActivity.kt
    {
        GeneratedFile file;
        file.path = basePath + "/MainActivity.kt";
        file.content = generateMainActivity(app);
        file.language = "kotlin";
        result.files.push_back(std::move(file));
    }

    // 3. Theme.kt
    {
        GeneratedFile file;
        file.path = basePath + "/ui/theme/Theme.kt";
        file.content = generateThemeKt(app);
        file.language = "kotlin";
        result.files.push_back(std::move(file));
    }

    // 4. Navigation.kt
    if (app.screens.size() > 1) {
        GeneratedFile file;
        file.path = basePath + "/ui/navigation/Navigation.kt";
        file.content = generateNavigationKt(app);
        file.language = "kotlin";
        result.files.push_back(std::move(file));
    }

    // 5. build.gradle.kts
    {
        GeneratedFile file;
        file.path = "app/build.gradle.kts";
        file.content = generateBuildGradle(app);
        file.language = "kotlin";
        result.files.push_back(std::move(file));
    }

    // 6. AndroidManifest.xml
    {
        GeneratedFile file;
        file.path = "app/src/main/AndroidManifest.xml";
        file.content = generateManifest(app);
        file.language = "xml";
        result.files.push_back(std::move(file));
    }

    // 7. settings.gradle.kts
    {
        GeneratedFile file;
        file.path = "settings.gradle.kts";
        file.content = generateSettingsGradle(app);
        file.language = "kotlin";
        result.files.push_back(std::move(file));
    }

    result.buildCommand = "./gradlew assembleDebug";
    return result;
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد شاشة واحدة
// ═══════════════════════════════════════════════════════════════════════════

CodeGenResult AndroidComposeCodeGen::generateScreen(const ScreenIR& screen) {
    CodeGenResult result;
    result.success = true;

    GeneratedFile file;
    file.path = screen.name + "Screen.kt";
    file.content = generateScreenKt(screen, "com.sad.app", ThemeIR{});
    file.language = "kotlin";
    result.files.push_back(std::move(file));

    return result;
}


// ═══════════════════════════════════════════════════════════════════════════
// Screen.kt
// ═══════════════════════════════════════════════════════════════════════════

std::string AndroidComposeCodeGen::generateScreenKt(
    const ScreenIR& screen,
    const std::string& packageName,
    const ThemeIR& theme
) {
    std::ostringstream ss;

    ss << "package " << packageName << ".ui\n\n";

    // الاستيرادات
    ss << "import androidx.compose.foundation.layout.*\n";
    ss << "import androidx.compose.foundation.lazy.*\n";
    ss << "import androidx.compose.material3.*\n";
    ss << "import androidx.compose.runtime.*\n";
    ss << "import androidx.compose.ui.*\n";
    ss << "import androidx.compose.ui.Modifier\n";
    ss << "import androidx.compose.ui.graphics.Color\n";
    ss << "import androidx.compose.ui.text.font.FontWeight\n";
    ss << "import androidx.compose.ui.unit.dp\n";
    ss << "import androidx.compose.ui.unit.sp\n\n";

    // الدالة التركيبية
    ss << "@Composable\n";
    ss << "fun " << screen.name << "Screen(\n";
    ss << "    onNavigate: (String) -> Unit = {}\n";
    ss << ") {\n";

    // إعلانات الحالة
    for (const auto& binding : screen.stateBindings) {
        std::string ktType;
        switch (binding.valueType) {
            case StateValueType::Number:   ktType = "Double"; break;
            case StateValueType::Text:     ktType = "String"; break;
            case StateValueType::Boolean:  ktType = "Boolean"; break;
            default:                       ktType = "Any"; break;
        }

        std::string initVal = binding.initialValue.empty() ? "\"\"" : binding.initialValue;

        switch (binding.kind) {
            case StateBindingKind::Signal:
                ss << "    var " << binding.name << " by remember { mutableStateOf("
                   << initVal << ") }\n";
                break;
            case StateBindingKind::Computed:
                ss << "    val " << binding.name << " = remember { derivedStateOf { "
                   << binding.computeExpression.value_or("null")
                   << " } }.value\n";
                break;
            default:
                ss << "    var " << binding.name << " by remember { mutableStateOf("
                   << initVal << ") }\n";
                break;
        }
    }
    if (!screen.stateBindings.empty()) ss << "\n";

    // شجرة الواجهة
    if (screen.rootNode) {
        emitComposable(ss, screen.rootNode, theme, 1);
    }

    ss << "}\n";
    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد Composable لعقدة
// ═══════════════════════════════════════════════════════════════════════════

void AndroidComposeCodeGen::emitComposable(
    std::ostringstream& ss,
    const UINodeIRPtr& node,
    const ThemeIR& theme,
    int indent
) {
    if (!node) return;
    std::string p = pad(indent);
    std::string mod = emitModifier(node->style);

    // استخراج النص من أول وسيط
    std::string textContent;
    if (!node->constructorArgs.empty()) {
        if (auto* s = std::get_if<std::string>(&node->constructorArgs[0])) {
            textContent = *s;
        }
    }

    switch (node->type) {
        // === العرض ===
        case WidgetType::Text:
            ss << p << "Text(\n";
            ss << p << "    text = \"" << textContent << "\",\n";
            if (node->style.fontSize.has_value()) {
                ss << p << "    fontSize = " << *node->style.fontSize << ".sp,\n";
            }
            if (node->style.foregroundColor.has_value()) {
                ss << p << "    color = " << composeColor(*node->style.foregroundColor) << ",\n";
            }
            if (node->style.fontWeight.has_value()) {
                ss << p << "    fontWeight = FontWeight.";
                switch (*node->style.fontWeight) {
                    case FontWeight::Bold:      ss << "Bold"; break;
                    case FontWeight::Light:     ss << "Light"; break;
                    case FontWeight::Medium:    ss << "Medium"; break;
                    case FontWeight::Thin:      ss << "Thin"; break;
                    case FontWeight::SemiBold:  ss << "SemiBold"; break;
                    case FontWeight::ExtraBold: ss << "ExtraBold"; break;
                    default:                    ss << "Normal"; break;
                }
                ss << ",\n";
            }
            if (!mod.empty()) {
                ss << p << "    modifier = " << mod << ",\n";
            }
            ss << p << ")\n";
            break;

        case WidgetType::Image:
            ss << p << "// TODO: Image(\"" << textContent << "\")\n";
            break;

        // === الإدخال ===
        case WidgetType::Button:
            ss << p << "Button(\n";
            ss << p << "    onClick = { ";
            if (!node->eventHandlers.empty()) {
                auto& eh = node->eventHandlers[0];
                if (!eh.handlerName.empty()) {
                    ss << eh.handlerName << "()";
                } else if (!eh.closureBody.empty()) {
                    ss << "/* " << eh.closureBody << " */";
                }
            }
            ss << " },\n";
            if (!mod.empty()) {
                ss << p << "    modifier = " << mod << ",\n";
            }
            ss << p << ") {\n";
            if (!node->children.empty()) {
                for (const auto& child : node->children) {
                    emitComposable(ss, child, theme, indent + 1);
                }
            } else if (!textContent.empty()) {
                ss << p << "    Text(\"" << textContent << "\")\n";
            }
            ss << p << "}\n";
            break;

        case WidgetType::TextField:
        case WidgetType::TextArea: {
            ss << p << "OutlinedTextField(\n";
            ss << p << "    value = \"\"";
            if (node->stateBinding.has_value()) {
                ss << ",\n" << p << "    onValueChange = { " << node->stateBinding->name << " = it }";
            } else {
                ss << ",\n" << p << "    onValueChange = {}";
            }
            if (!textContent.empty()) {
                ss << ",\n" << p << "    label = { Text(\"" << textContent << "\") }";
            }
            if (!mod.empty()) {
                ss << ",\n" << p << "    modifier = " << mod;
            }
            ss << "\n" << p << ")\n";
            break;
        }

        case WidgetType::Toggle:
            ss << p << "Switch(\n";
            ss << p << "    checked = false,\n";
            ss << p << "    onCheckedChange = { ";
            if (!node->eventHandlers.empty()) {
                ss << node->eventHandlers[0].handlerName << "()";
            }
            ss << " }\n";
            ss << p << ")\n";
            break;

        case WidgetType::Slider:
            ss << p << "Slider(\n";
            ss << p << "    value = 0f,\n";
            ss << p << "    onValueChange = {}\n";
            ss << p << ")\n";
            break;

        // === التخطيط ===
        case WidgetType::Column:
        case WidgetType::LazyColumn:
            if (node->type == WidgetType::LazyColumn) {
                ss << p << "LazyColumn(\n";
            } else {
                ss << p << "Column(\n";
            }
            if (!mod.empty()) {
                ss << p << "    modifier = " << mod << ",\n";
            }
            if (node->style.horizontalAlignment.has_value()) {
                ss << p << "    horizontalAlignment = Alignment.";
                switch (*node->style.horizontalAlignment) {
                    case HAlignment::Leading:  ss << "Start"; break;
                    case HAlignment::Center:   ss << "CenterHorizontally"; break;
                    case HAlignment::Trailing: ss << "End"; break;
                }
                ss << ",\n";
            }
            if (node->style.spacing > 0) {
                ss << p << "    verticalArrangement = Arrangement.spacedBy(" << node->style.spacing << ".dp),\n";
            }
            ss << p << ") {\n";
            if (node->type == WidgetType::LazyColumn) {
                ss << p << "    items(" << node->children.size() << ") {\n";
                for (const auto& child : node->children) {
                    emitComposable(ss, child, theme, indent + 2);
                }
                ss << p << "    }\n";
            } else {
                for (const auto& child : node->children) {
                    emitComposable(ss, child, theme, indent + 1);
                }
            }
            ss << p << "}\n";
            break;

        case WidgetType::Row:
        case WidgetType::LazyRow:
            if (node->type == WidgetType::LazyRow) {
                ss << p << "LazyRow(\n";
            } else {
                ss << p << "Row(\n";
            }
            if (!mod.empty()) {
                ss << p << "    modifier = " << mod << ",\n";
            }
            if (node->style.spacing > 0) {
                ss << p << "    horizontalArrangement = Arrangement.spacedBy(" << node->style.spacing << ".dp),\n";
            }
            ss << p << ") {\n";
            for (const auto& child : node->children) {
                emitComposable(ss, child, theme, indent + 1);
            }
            ss << p << "}\n";
            break;

        case WidgetType::Stack:
            ss << p << "Box(\n";
            if (!mod.empty()) {
                ss << p << "    modifier = " << mod << ",\n";
            }
            ss << p << ") {\n";
            for (const auto& child : node->children) {
                emitComposable(ss, child, theme, indent + 1);
            }
            ss << p << "}\n";
            break;

        // === الحاويات ===
        case WidgetType::Card:
            ss << p << "Card(\n";
            if (!mod.empty()) {
                ss << p << "    modifier = " << mod << ",\n";
            }
            if (node->style.borderRadius > 0) {
                ss << p << "    shape = RoundedCornerShape(" << node->style.borderRadius << ".dp),\n";
            }
            ss << p << ") {\n";
            for (const auto& child : node->children) {
                emitComposable(ss, child, theme, indent + 1);
            }
            ss << p << "}\n";
            break;

        case WidgetType::Container:
        case WidgetType::Box:
        case WidgetType::Surface:
            ss << p << "Surface(\n";
            if (!mod.empty()) {
                ss << p << "    modifier = " << mod << ",\n";
            }
            if (node->style.backgroundColor.has_value()) {
                ss << p << "    color = " << composeColor(*node->style.backgroundColor) << ",\n";
            }
            ss << p << ") {\n";
            for (const auto& child : node->children) {
                emitComposable(ss, child, theme, indent + 1);
            }
            ss << p << "}\n";
            break;

        case WidgetType::ScrollView:
            ss << p << "Column(\n";
            ss << p << "    modifier = Modifier.verticalScroll(rememberScrollState())\n";
            ss << p << ") {\n";
            for (const auto& child : node->children) {
                emitComposable(ss, child, theme, indent + 1);
            }
            ss << p << "}\n";
            break;

        // === الهيكل ===
        case WidgetType::Scaffold:
            ss << p << "Scaffold(\n";
            // البحث عن AppBar و BottomBar بين الأبناء
            for (const auto& child : node->children) {
                if (child && child->type == WidgetType::AppBar) {
                    ss << p << "    topBar = {\n";
                    emitComposable(ss, child, theme, indent + 2);
                    ss << p << "    },\n";
                }
                if (child && child->type == WidgetType::BottomBar) {
                    ss << p << "    bottomBar = {\n";
                    emitComposable(ss, child, theme, indent + 2);
                    ss << p << "    },\n";
                }
            }
            ss << p << ") { innerPadding ->\n";
            ss << p << "    Column(modifier = Modifier.padding(innerPadding)) {\n";
            for (const auto& child : node->children) {
                if (child && child->type != WidgetType::AppBar && child->type != WidgetType::BottomBar) {
                    emitComposable(ss, child, theme, indent + 2);
                }
            }
            ss << p << "    }\n";
            ss << p << "}\n";
            break;

        case WidgetType::AppBar:
            ss << p << "TopAppBar(\n";
            ss << p << "    title = { Text(\"" << textContent << "\") }\n";
            ss << p << ")\n";
            break;

        // === الفاصل ===
        case WidgetType::Spacer:
            ss << p << "Spacer(modifier = " << (mod.empty() ? "Modifier.height(16.dp)" : mod) << ")\n";
            break;

        case WidgetType::Divider:
            ss << p << "HorizontalDivider()\n";
            break;

        // === القوائم ===
        case WidgetType::List:
            ss << p << "LazyColumn {\n";
            for (const auto& child : node->children) {
                ss << p << "    item {\n";
                emitComposable(ss, child, theme, indent + 2);
                ss << p << "    }\n";
            }
            ss << p << "}\n";
            break;

        // === تقدم ===
        case WidgetType::ProgressBar:
            ss << p << "LinearProgressIndicator(modifier = " << (mod.empty() ? "Modifier" : mod) << ")\n";
            break;
        case WidgetType::CircleProgress:
        case WidgetType::LoadingSpinner:
            ss << p << "CircularProgressIndicator()\n";
            break;

        // === مجموعة/شرطي ===
        case WidgetType::Group:
            for (const auto& child : node->children) {
                emitComposable(ss, child, theme, indent);
            }
            break;

        // === الباقي ===
        default:
            ss << p << "// " << widgetTypeToArabic(node->type) << "\n";
            ss << p << "Box(" << (mod.empty() ? "" : "modifier = " + mod) << ") {\n";
            for (const auto& child : node->children) {
                emitComposable(ss, child, theme, indent + 1);
            }
            ss << p << "}\n";
            break;
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// Modifier
// ═══════════════════════════════════════════════════════════════════════════

std::string AndroidComposeCodeGen::emitModifier(const StyleIR& style) {
    if (!style.hasAnyStyle()) return "";

    std::ostringstream ss;
    ss << "Modifier";

    if (style.width.isFill()) {
        ss << ".fillMaxWidth()";
    } else if (!style.width.isAuto()) {
        ss << ".width(" << style.width.value << ".dp)";
    }

    if (style.height.isFill()) {
        ss << ".fillMaxHeight()";
    } else if (!style.height.isAuto()) {
        ss << ".height(" << style.height.value << ".dp)";
    }

    if (!style.padding.isZero()) {
        if (style.padding.top == style.padding.bottom &&
            style.padding.leading == style.padding.trailing &&
            style.padding.top == style.padding.leading) {
            ss << ".padding(" << style.padding.top << ".dp)";
        } else {
            ss << ".padding(start = " << style.padding.leading << ".dp, top = "
               << style.padding.top << ".dp, end = " << style.padding.trailing
               << ".dp, bottom = " << style.padding.bottom << ".dp)";
        }
    }

    if (style.backgroundColor.has_value()) {
        ss << ".background(" << composeColor(*style.backgroundColor) << ")";
    }

    if (style.borderRadius > 0) {
        ss << ".clip(RoundedCornerShape(" << style.borderRadius << ".dp))";
    }

    if (style.borderWidth > 0 && style.borderColor.has_value()) {
        ss << ".border(" << style.borderWidth << ".dp, "
           << composeColor(*style.borderColor) << ")";
    }

    if (style.opacity != 1.0f) {
        ss << ".alpha(" << style.opacity << "f)";
    }

    if (style.shadow.has_value()) {
        ss << ".shadow(" << style.shadow->blur << ".dp)";
    }

    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// MainActivity.kt
// ═══════════════════════════════════════════════════════════════════════════

std::string AndroidComposeCodeGen::generateMainActivity(const AppIR& app) {
    std::ostringstream ss;
    ss << "package " << app.packageName << "\n\n";
    ss << "import android.os.Bundle\n";
    ss << "import androidx.activity.ComponentActivity\n";
    ss << "import androidx.activity.compose.setContent\n";
    ss << "import androidx.compose.foundation.layout.fillMaxSize\n";
    ss << "import androidx.compose.material3.MaterialTheme\n";
    ss << "import androidx.compose.material3.Surface\n";
    ss << "import androidx.compose.ui.Modifier\n";
    ss << "import " << app.packageName << ".ui.theme." << app.appName << "Theme\n";

    if (app.screens.size() > 1) {
        ss << "import " << app.packageName << ".ui.navigation.AppNavigation\n";
    } else if (!app.screens.empty()) {
        ss << "import " << app.packageName << ".ui." << app.screens[0].name << "Screen\n";
    }

    ss << "\nclass MainActivity : ComponentActivity() {\n";
    ss << "    override fun onCreate(savedInstanceState: Bundle?) {\n";
    ss << "        super.onCreate(savedInstanceState)\n";
    ss << "        setContent {\n";
    ss << "            " << app.appName << "Theme {\n";
    ss << "                Surface(\n";
    ss << "                    modifier = Modifier.fillMaxSize(),\n";
    ss << "                    color = MaterialTheme.colorScheme.background\n";
    ss << "                ) {\n";

    if (app.screens.size() > 1) {
        ss << "                    AppNavigation()\n";
    } else if (!app.screens.empty()) {
        ss << "                    " << app.screens[0].name << "Screen()\n";
    }

    ss << "                }\n";
    ss << "            }\n";
    ss << "        }\n";
    ss << "    }\n";
    ss << "}\n";
    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// Theme.kt
// ═══════════════════════════════════════════════════════════════════════════

std::string AndroidComposeCodeGen::generateThemeKt(const AppIR& app) {
    std::ostringstream ss;
    ss << "package " << app.packageName << ".ui.theme\n\n";
    ss << "import androidx.compose.material3.*\n";
    ss << "import androidx.compose.runtime.Composable\n";
    ss << "import androidx.compose.ui.graphics.Color\n\n";

    ss << "private val LightColorScheme = lightColorScheme(\n";
    ss << "    primary = " << composeColor(app.theme.primaryColor) << ",\n";
    ss << "    secondary = " << composeColor(app.theme.secondaryColor) << ",\n";
    ss << "    background = " << composeColor(app.theme.backgroundColor) << ",\n";
    ss << "    surface = " << composeColor(app.theme.surfaceColor) << ",\n";
    ss << "    error = " << composeColor(app.theme.errorColor) << ",\n";
    ss << "    onPrimary = " << composeColor(app.theme.textOnPrimary) << ",\n";
    ss << "    onBackground = " << composeColor(app.theme.textPrimary) << ",\n";
    ss << ")\n\n";

    ss << "@Composable\n";
    ss << "fun " << app.appName << "Theme(\n";
    ss << "    content: @Composable () -> Unit\n";
    ss << ") {\n";
    ss << "    MaterialTheme(\n";
    ss << "        colorScheme = LightColorScheme,\n";
    ss << "        content = content\n";
    ss << "    )\n";
    ss << "}\n";
    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// Navigation.kt
// ═══════════════════════════════════════════════════════════════════════════

std::string AndroidComposeCodeGen::generateNavigationKt(const AppIR& app) {
    std::ostringstream ss;
    ss << "package " << app.packageName << ".ui.navigation\n\n";
    ss << "import androidx.compose.runtime.Composable\n";
    ss << "import androidx.navigation.compose.NavHost\n";
    ss << "import androidx.navigation.compose.composable\n";
    ss << "import androidx.navigation.compose.rememberNavController\n";

    for (const auto& screen : app.screens) {
        ss << "import " << app.packageName << ".ui." << screen.name << "Screen\n";
    }

    ss << "\n@Composable\n";
    ss << "fun AppNavigation() {\n";
    ss << "    val navController = rememberNavController()\n\n";
    ss << "    NavHost(\n";
    ss << "        navController = navController,\n";
    ss << "        startDestination = \"" << app.initialScreenName << "\"\n";
    ss << "    ) {\n";

    for (const auto& screen : app.screens) {
        ss << "        composable(\"" << screen.routeName << "\") {\n";
        ss << "            " << screen.name << "Screen(\n";
        ss << "                onNavigate = { route -> navController.navigate(route) }\n";
        ss << "            )\n";
        ss << "        }\n";
    }

    ss << "    }\n";
    ss << "}\n";
    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// build.gradle.kts
// ═══════════════════════════════════════════════════════════════════════════

std::string AndroidComposeCodeGen::generateBuildGradle(const AppIR& app) {
    std::ostringstream ss;
    ss << "plugins {\n";
    ss << "    id(\"com.android.application\")\n";
    ss << "    id(\"org.jetbrains.kotlin.android\")\n";
    ss << "}\n\n";

    ss << "android {\n";
    ss << "    namespace = \"" << app.packageName << "\"\n";
    ss << "    compileSdk = " << config_.compileSdk << "\n\n";

    ss << "    defaultConfig {\n";
    ss << "        applicationId = \"" << app.packageName << "\"\n";
    ss << "        minSdk = " << config_.minSdk << "\n";
    ss << "        targetSdk = " << config_.targetSdk << "\n";
    ss << "        versionCode = 1\n";
    ss << "        versionName = \"" << app.version << "\"\n";

    // NDK ABIs
    ss << "        ndk {\n";
    ss << "            abiFilters += listOf(";
    for (size_t i = 0; i < config_.architectures.size(); i++) {
        if (i > 0) ss << ", ";
        ss << "\"" << config_.architectures[i] << "\"";
    }
    ss << ")\n";
    ss << "        }\n";
    ss << "    }\n\n";

    ss << "    buildFeatures {\n";
    ss << "        compose = true\n";
    ss << "    }\n\n";

    ss << "    composeOptions {\n";
    ss << "        kotlinCompilerExtensionVersion = \"" << config_.composeVersion << "\"\n";
    ss << "    }\n";
    ss << "}\n\n";

    ss << "dependencies {\n";
    ss << "    implementation(\"androidx.core:core-ktx:1.12.0\")\n";
    ss << "    implementation(\"androidx.activity:activity-compose:1.8.2\")\n";
    ss << "    implementation(platform(\"androidx.compose:compose-bom:2024.02.00\"))\n";
    ss << "    implementation(\"androidx.compose.ui:ui\")\n";
    ss << "    implementation(\"androidx.compose.material3:material3\")\n";
    ss << "    implementation(\"androidx.compose.ui:ui-tooling-preview\")\n";
    ss << "    implementation(\"androidx.navigation:navigation-compose:2.7.7\")\n";
    ss << "}\n";

    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// AndroidManifest.xml
// ═══════════════════════════════════════════════════════════════════════════

std::string AndroidComposeCodeGen::generateManifest(const AppIR& app) {
    std::ostringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    ss << "<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\">\n\n";

    // الأذونات
    for (const auto& perm : config_.permissions) {
        ss << "    <uses-permission android:name=\"" << perm << "\" />\n";
    }
    if (app.androidConfig.has_value()) {
        for (const auto& perm : app.androidConfig->permissions) {
            ss << "    <uses-permission android:name=\"" << perm << "\" />\n";
        }
    }
    ss << "\n";

    ss << "    <application\n";
    ss << "        android:label=\"" << app.appName << "\"\n";
    ss << "        android:supportsRtl=\"" << (app.theme.isRTL ? "true" : "false") << "\"\n";
    ss << "        android:theme=\"@style/Theme.Material3.DayNight.NoActionBar\">\n\n";

    ss << "        <activity\n";
    ss << "            android:name=\".MainActivity\"\n";
    ss << "            android:exported=\"true\">\n";
    ss << "            <intent-filter>\n";
    ss << "                <action android:name=\"android.intent.action.MAIN\" />\n";
    ss << "                <category android:name=\"android.intent.category.LAUNCHER\" />\n";
    ss << "            </intent-filter>\n";
    ss << "        </activity>\n\n";

    ss << "    </application>\n";
    ss << "</manifest>\n";
    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// settings.gradle.kts
// ═══════════════════════════════════════════════════════════════════════════

std::string AndroidComposeCodeGen::generateSettingsGradle(const AppIR& app) {
    std::ostringstream ss;
    ss << "pluginManagement {\n";
    ss << "    repositories {\n";
    ss << "        google()\n";
    ss << "        mavenCentral()\n";
    ss << "        gradlePluginPortal()\n";
    ss << "    }\n";
    ss << "}\n\n";

    ss << "dependencyResolution {\n";
    ss << "    repositories {\n";
    ss << "        google()\n";
    ss << "        mavenCentral()\n";
    ss << "    }\n";
    ss << "}\n\n";

    ss << "rootProject.name = \"" << app.appName << "\"\n";
    ss << "include(\":app\")\n";
    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// مساعدات
// ═══════════════════════════════════════════════════════════════════════════

std::string AndroidComposeCodeGen::composeColor(const Color& c) {
    std::ostringstream ss;
    ss << "Color(0x" << std::hex << std::setfill('0')
       << std::setw(2) << (int)(c.a * 255)
       << std::setw(2) << (int)c.r
       << std::setw(2) << (int)c.g
       << std::setw(2) << (int)c.b
       << ")";
    return ss.str();
}

std::string AndroidComposeCodeGen::pad(int indent) {
    return std::string(indent * 4, ' ');
}


} // namespace ir
} // namespace ui
} // namespace sad
