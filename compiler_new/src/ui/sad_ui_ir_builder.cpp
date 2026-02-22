/**
 * ==========================================================================
 * ملف: sad_ui_ir_builder.cpp
 * الوصف: تنفيذ بنّاء SadUI IR — تحويل UINode AST → SadUI IR
 * ==========================================================================
 * بسم الله الرحمن الرحيم
 */

#include "ui/sad_ui_ir_builder.hpp"
#include <algorithm>
#include <sstream>

namespace sad {
namespace ui {
namespace ir {

// ═══════════════════════════════════════════════════════════════════════════
// ربط UINodeType → WidgetType
// ═══════════════════════════════════════════════════════════════════════════

namespace {

struct NodeTypeMapping {
    parser::UINodeType uiType;
    WidgetType irType;
};

static const NodeTypeMapping nodeTypeMappings[] = {
    // العرض
    {parser::UINodeType::Text,          WidgetType::Text},
    {parser::UINodeType::Image,         WidgetType::Image},
    {parser::UINodeType::Icon,          WidgetType::Icon},

    // الإدخال
    {parser::UINodeType::Button,        WidgetType::Button},
    {parser::UINodeType::TextField,     WidgetType::TextField},
    {parser::UINodeType::TextArea,      WidgetType::TextArea},
    {parser::UINodeType::Toggle,        WidgetType::Toggle},
    {parser::UINodeType::Slider,        WidgetType::Slider},
    {parser::UINodeType::Picker,        WidgetType::Dropdown},
    {parser::UINodeType::DatePicker,    WidgetType::DatePicker},
    {parser::UINodeType::ColorPicker,   WidgetType::ColorPicker},

    // التخطيط
    {parser::UINodeType::Column,        WidgetType::Column},
    {parser::UINodeType::Row,           WidgetType::Row},
    {parser::UINodeType::Stack,         WidgetType::Stack},
    {parser::UINodeType::Grid,          WidgetType::Grid},
    {parser::UINodeType::LazyColumn,    WidgetType::LazyColumn},
    {parser::UINodeType::LazyRow,       WidgetType::LazyRow},
    {parser::UINodeType::LazyGrid,      WidgetType::LazyGrid},
    {parser::UINodeType::Spacer,        WidgetType::Spacer},
    {parser::UINodeType::Divider,       WidgetType::Divider},

    // الحاويات
    {parser::UINodeType::Container,     WidgetType::Container},
    {parser::UINodeType::Card,          WidgetType::Card},
    {parser::UINodeType::Surface,       WidgetType::Surface},
    {parser::UINodeType::Scaffold,      WidgetType::Scaffold},
    {parser::UINodeType::Box,           WidgetType::Box},

    // التنقل
    {parser::UINodeType::NavigationView, WidgetType::NavigationView},
    {parser::UINodeType::NavigationLink, WidgetType::NavigationLink},
    {parser::UINodeType::TabView,       WidgetType::TabView},
    {parser::UINodeType::TabItem,       WidgetType::TabView},

    // القوائم
    {parser::UINodeType::List,          WidgetType::List},
    {parser::UINodeType::Section,       WidgetType::Section},
    {parser::UINodeType::ForEach,       WidgetType::ForEach},

    // الحوارات
    {parser::UINodeType::Alert,         WidgetType::Alert},
    {parser::UINodeType::Sheet,         WidgetType::BottomSheet},
    {parser::UINodeType::Dialog,        WidgetType::Dialog},
    {parser::UINodeType::Menu,          WidgetType::Dropdown},

    // متقدم
    {parser::UINodeType::Canvas,        WidgetType::Canvas},
    {parser::UINodeType::Map,           WidgetType::MapView},
    {parser::UINodeType::WebView,       WidgetType::WebView},
    {parser::UINodeType::VideoPlayer,   WidgetType::VideoPlayer},

    // خاص
    {parser::UINodeType::ViewBuilder,   WidgetType::Group},
    {parser::UINodeType::CustomView,    WidgetType::CustomWidget},
    {parser::UINodeType::Conditional,   WidgetType::Conditional},
    {parser::UINodeType::Group,         WidgetType::Group},
};

static const int NODE_TYPE_MAPPING_COUNT =
    sizeof(nodeTypeMappings) / sizeof(nodeTypeMappings[0]);

// ═══════════════════════════════════════════════════════════════════════════
// ربط الألوان المسماة بالعربية
// ═══════════════════════════════════════════════════════════════════════════

static const std::unordered_map<std::string, Color> arabicColors = {
    // ألوان أساسية
    {"\xd8\xa3\xd8\xad\xd9\x85\xd8\xb1",       Color(244, 67, 54)},    // أحمر
    {"\xd8\xa3\xd8\xb2\xd8\xb1\xd9\x82",       Color(33, 150, 243)},   // أزرق
    {"\xd8\xa3\xd8\xae\xd8\xb6\xd8\xb1",       Color(76, 175, 80)},    // أخضر
    {"\xd8\xa3\xd8\xb5\xd9\x81\xd8\xb1",       Color(255, 235, 59)},   // أصفر
    {"\xd8\xa8\xd8\xb1\xd8\xaa\xd9\x82\xd8\xa7\xd9\x84\xd9\x8a",   Color(255, 152, 0)},    // برتقالي
    {"\xd8\xa8\xd9\x86\xd9\x81\xd8\xb3\xd8\xac\xd9\x8a",   Color(156, 39, 176)},   // بنفسجي
    {"\xd9\x88\xd8\xb1\xd8\xaf\xd9\x8a",       Color(233, 30, 99)},    // وردي
    {"\xd8\xa3\xd8\xa8\xd9\x8a\xd8\xb6",       Color(255, 255, 255)},  // أبيض
    {"\xd8\xa3\xd8\xb3\xd9\x88\xd8\xaf",       Color(0, 0, 0)},        // أسود
    {"\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a",       Color(158, 158, 158)},  // رمادي
    {"\xd8\xa8\xd9\x86\xd9\x8a",         Color(121, 85, 72)},    // بني
    {"\xd8\xb3\xd9\x85\xd8\xa7\xd9\x88\xd9\x8a",       Color(0, 188, 212)},    // سماوي
    {"\xd9\x86\xd9\x8a\xd9\x84\xd9\x8a",       Color(63, 81, 181)},    // نيلي
    // ألوان دلالية
    {"\xd8\xa3\xd8\xb3\xd8\xa7\xd8\xb3\xd9\x8a",       Color(25, 118, 210)},   // أساسي
    {"\xd8\xab\xd8\xa7\xd9\x86\xd9\x88\xd9\x8a",       Color(255, 152, 0)},    // ثانوي
    {"\xd9\x86\xd8\xac\xd8\xa7\xd8\xad",         Color(76, 175, 80)},    // نجاح
    {"\xd8\xae\xd8\xb7\xd8\xa3",           Color(244, 67, 54)},    // خطأ
    {"\xd8\xaa\xd8\xad\xd8\xb0\xd9\x8a\xd8\xb1",       Color(255, 193, 7)},    // تحذير
    {"\xd9\x85\xd8\xb9\xd9\x84\xd9\x88\xd9\x85\xd8\xa7\xd8\xaa",     Color(33, 150, 243)},   // معلومات
    {"\xd8\xb4\xd9\x81\xd8\xa7\xd9\x81",         Color(0, 0, 0, 0.0f)},  // شفاف
};

// هل هذا المعدّل يُمثّل حدثًا (وليس نمطًا بصريًا)؟
bool isEventModifier(parser::ModifierType mod) {
    switch (mod) {
        case parser::ModifierType::OnTap:
        case parser::ModifierType::OnLongPress:
        case parser::ModifierType::OnDrag:
        case parser::ModifierType::OnAppear:
        case parser::ModifierType::OnDisappear:
            return true;
        default:
            return false;
    }
}

// تحويل ModifierType → EventType
EventType modifierToEventType(parser::ModifierType mod) {
    switch (mod) {
        case parser::ModifierType::OnTap:        return EventType::Tap;
        case parser::ModifierType::OnLongPress:  return EventType::LongPress;
        case parser::ModifierType::OnDrag:       return EventType::DragStart;
        case parser::ModifierType::OnAppear:     return EventType::Appear;
        case parser::ModifierType::OnDisappear:  return EventType::Disappear;
        default:                                 return EventType::Tap;
    }
}

} // anonymous namespace


// ═══════════════════════════════════════════════════════════════════════════
// IRBuilder — إنشاء وتدمير
// ═══════════════════════════════════════════════════════════════════════════

IRBuilder::IRBuilder(const IRBuilderConfig& config)
    : config_(config) {}

IRBuilder::~IRBuilder() = default;

void IRBuilder::setConfig(const IRBuilderConfig& config) {
    config_ = config;
}

const IRBuilderConfig& IRBuilder::getConfig() const {
    return config_;
}

void IRBuilder::setTheme(const ThemeIR& theme) {
    config_.defaultTheme = theme;
}

const std::vector<IRDiagnostic>& IRBuilder::getLastDiagnostics() const {
    return diagnostics_;
}

void IRBuilder::clearDiagnostics() {
    diagnostics_.clear();
}

int IRBuilder::generateId() {
    return nextId_++;
}

void IRBuilder::addDiagnostic(DiagnosticLevel level,
                               const std::string& messageAR,
                               const std::string& messageEN,
                               size_t line, size_t column) {
    diagnostics_.push_back({level, messageAR, messageEN, line, column, ""});
}


// ═══════════════════════════════════════════════════════════════════════════
// البناء الرئيسي: buildScreen
// ═══════════════════════════════════════════════════════════════════════════

IRBuildResult<ScreenIR> IRBuilder::buildScreen(
    const parser::ViewDefinition& viewDef
) {
    clearDiagnostics();
    nextId_ = 1;

    ScreenIR screen;
    screen.name = viewDef.name;
    screen.routeName = viewDef.name;
    screen.sourceFile = viewDef.sourceFile;
    screen.startLine = viewDef.startLine;
    screen.endLine = viewDef.endLine;

    // 1. تحويل ربط الحالة
    for (const auto& binding : viewDef.stateBindings) {
        screen.stateBindings.push_back(convertStateBinding(binding));
    }

    // 2. تحويل شجرة الواجهة
    if (viewDef.bodyTree) {
        screen.rootNode = convertNode(viewDef.bodyTree, 0);
    } else {
        addDiagnostic(DiagnosticLevel::Error,
                      "\xd8\xa7\xd9\x84\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9 \xd9\x84\xd8\xa7 \xd8\xaa\xd8\xad\xd8\xaa\xd9\x88\xd9\x8a \xd8\xb9\xd9\x84\xd9\x89 \xd8\xb4\xd8\xac\xd8\xb1\xd8\xa9 \xd9\x85\xd9\x83\xd9\x88\xd9\x86\xd8\xa7\xd8\xaa",
                      "View definition has no body tree",
                      viewDef.startLine, 0);
    }

    // 3. استخراج الدوال المعالجة
    for (const auto& funcName : viewDef.helperFunctionNames) {
        screen.handlerFunctions[funcName] = ""; // الجسم يُملأ لاحقًا من AST
    }

    IRBuildResult<ScreenIR> result;
    result.value = std::move(screen);
    result.diagnostics = diagnostics_;
    return result;
}


// ═══════════════════════════════════════════════════════════════════════════
// البناء الرئيسي: buildNode
// ═══════════════════════════════════════════════════════════════════════════

IRBuildResult<UINodeIRPtr> IRBuilder::buildNode(
    const parser::UINodePtr& uiNode
) {
    clearDiagnostics();
    nextId_ = 1;

    IRBuildResult<UINodeIRPtr> result;
    if (uiNode) {
        result.value = convertNode(uiNode, 0);
    } else {
        addDiagnostic(DiagnosticLevel::Error,
                      "\xd8\xa7\xd9\x84\xd8\xb9\xd9\x82\xd8\xaf\xd8\xa9 \xd9\x81\xd8\xa7\xd8\xb1\xd8\xba\xd8\xa9",
                      "NULL UINode pointer",
                      0, 0);
    }
    result.diagnostics = diagnostics_;
    return result;
}


// ═══════════════════════════════════════════════════════════════════════════
// البناء الرئيسي: buildApp
// ═══════════════════════════════════════════════════════════════════════════

IRBuildResult<AppIR> IRBuilder::buildApp(
    const std::vector<parser::ViewDefinition>& views,
    const std::unordered_map<std::string, std::string>& projectConfig
) {
    clearDiagnostics();
    nextId_ = 1;

    AppIR app;

    // قراءة إعدادات المشروع
    auto getConfig = [&](const std::string& key, const std::string& def = "") -> std::string {
        auto it = projectConfig.find(key);
        return it != projectConfig.end() ? it->second : def;
    };

    app.appName = getConfig("\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85", "تطبيقي");  // الاسم
    app.packageName = getConfig("\xd8\xa7\xd9\x84\xd8\xad\xd8\xb2\xd9\x85\xd8\xa9", "com.sad.app"); // الحزمة
    app.version = getConfig("\xd8\xa7\xd9\x84\xd8\xa5\xd8\xb5\xd8\xaf\xd8\xa7\xd8\xb1", "1.0.0"); // الإصدار
    app.description = getConfig("\xd8\xa7\xd9\x84\xd9\x88\xd8\xb5\xd9\x81"); // الوصف

    // تطبيق الثيمة
    app.theme = config_.defaultTheme;

    // بناء الشاشات
    for (const auto& viewDef : views) {
        auto screenResult = buildScreen(viewDef);
        if (screenResult.success()) {
            app.screens.push_back(std::move(*screenResult.value));
        }
        // نسخ التشخيصات
        for (const auto& d : screenResult.diagnostics) {
            diagnostics_.push_back(d);
        }
    }

    // الشاشة الأولى
    if (!app.screens.empty()) {
        app.initialScreenName = app.screens[0].name;
    }

    // بناء جدول المسارات
    for (const auto& screen : app.screens) {
        AppIR::Route route;
        route.name = screen.routeName;
        route.screenName = screen.name;
        app.routes.push_back(std::move(route));
    }

    // إعدادات أندرويد
    auto androidPkg = getConfig("android_package");
    if (!androidPkg.empty() || config_.targetPlatform == TargetPlatform::Android
                            || config_.targetPlatform == TargetPlatform::All) {
        AppIR::AndroidConfig android;
        android.minSdk = 24;
        android.targetSdk = 34;
        if (!androidPkg.empty()) {
            app.packageName = androidPkg;
        }
        app.androidConfig = android;
    }

    // إعدادات سطح المكتب
    if (config_.targetPlatform == TargetPlatform::Desktop
        || config_.targetPlatform == TargetPlatform::All) {
        AppIR::DesktopConfig desktop;
        desktop.windowWidth = 400;
        desktop.windowHeight = 700;
        app.desktopConfig = desktop;
    }

    IRBuildResult<AppIR> result;
    result.value = std::move(app);
    result.diagnostics = diagnostics_;
    return result;
}


// ═══════════════════════════════════════════════════════════════════════════
// تحويل UINodeType → WidgetType
// ═══════════════════════════════════════════════════════════════════════════

std::optional<WidgetType> IRBuilder::mapNodeType(parser::UINodeType uiType) {
    for (int i = 0; i < NODE_TYPE_MAPPING_COUNT; i++) {
        if (nodeTypeMappings[i].uiType == uiType) {
            return nodeTypeMappings[i].irType;
        }
    }
    return std::nullopt;
}


// ═══════════════════════════════════════════════════════════════════════════
// تحويل عقدة UINode → UINodeIR (تكراري)
// ═══════════════════════════════════════════════════════════════════════════

UINodeIRPtr IRBuilder::convertNode(const parser::UINodePtr& node, int depth) {
    if (!node) return nullptr;

    // فحص عمق التداخل
    if (depth > config_.maxDepth) {
        addDiagnostic(DiagnosticLevel::Error,
                      "\xd8\xaa\xd8\xac\xd8\xa7\xd9\x88\xd8\xb2 \xd8\xa7\xd9\x84\xd8\xad\xd8\xaf \xd8\xa7\xd9\x84\xd8\xa3\xd9\x82\xd8\xb5\xd9\x89 \xd9\x84\xd8\xb9\xd9\x85\xd9\x82 \xd8\xa7\xd9\x84\xd8\xb4\xd8\xac\xd8\xb1\xd8\xa9",
                      "Max tree depth exceeded",
                      node->startLine, node->startColumn);
        return nullptr;
    }

    auto irNode = std::make_shared<UINodeIR>();

    // 1. المعرّف
    irNode->id = config_.autoGenerateIds ? generateId() : 0;

    // 2. نوع المكون
    auto widgetType = mapNodeType(node->type);
    if (widgetType) {
        irNode->type = *widgetType;
    } else {
        irNode->type = WidgetType::CustomWidget;
        addDiagnostic(DiagnosticLevel::Warning,
                      "\xd9\x86\xd9\x88\xd8\xb9 \xd9\x85\xd9\x83\xd9\x88\xd9\x86 \xd8\xba\xd9\x8a\xd8\xb1 \xd9\x85\xd8\xb9\xd8\xb1\xd9\x88\xd9\x81: " + node->arabicName,
                      "Unknown widget type: " + node->arabicName,
                      node->startLine, node->startColumn);
    }

    // 3. الاسم العربي
    irNode->arabicName = node->arabicName;

    // 4. موقع المصدر
    irNode->sourceLine = node->startLine;
    irNode->sourceColumn = node->startColumn;

    // 5. وسائط البناء
    for (const auto& arg : node->constructorArgs) {
        irNode->constructorArgs.push_back(convertPropertyValue(arg));
    }

    // 6. الخصائص المسماة
    for (const auto& [key, val] : node->namedArgs) {
        irNode->properties[key] = convertPropertyValue(val);
    }

    // 7. الأنماط (من المعدّلات)
    irNode->style = convertModifiers(node->modifiers);

    // تطبيق RTL الافتراضي
    irNode->style.isRTL = config_.defaultRTL;

    // 8. الأحداث (من المعدّلات)
    irNode->eventHandlers = extractEvents(node->modifiers);

    // 9. إغلاق الحدث (للأزرار)
    if (!node->actionClosure.empty() && irNode->eventHandlers.empty()) {
        EventHandlerIR tapHandler;
        tapHandler.type = EventType::Tap;
        tapHandler.closureBody = node->actionClosure;
        irNode->eventHandlers.push_back(std::move(tapHandler));
    }

    // 10. الأبناء (تحويل تكراري)
    for (const auto& child : node->children) {
        auto irChild = convertNode(child, depth + 1);
        if (irChild) {
            irNode->children.push_back(std::move(irChild));
        }
    }

    return irNode;
}


// ═══════════════════════════════════════════════════════════════════════════
// تحويل المعدّلات → StyleIR
// ═══════════════════════════════════════════════════════════════════════════

StyleIR IRBuilder::convertModifiers(const std::vector<parser::UIModifier>& modifiers) {
    StyleIR style;

    for (const auto& mod : modifiers) {
        // تخطي معدّلات الأحداث
        if (isEventModifier(mod.type)) continue;

        applyModifier(mod, style);
    }

    return style;
}

void IRBuilder::applyModifier(const parser::UIModifier& modifier, StyleIR& style) {
    const auto& args = modifier.arguments;

    // دالة مساعدة لاستخراج القيمة العددية الأولى
    auto getFloat = [&]() -> std::optional<float> {
        if (args.empty()) return std::nullopt;
        if (auto* d = std::get_if<double>(&args[0])) return static_cast<float>(*d);
        if (auto* i = std::get_if<int64_t>(&args[0])) return static_cast<float>(*i);
        return std::nullopt;
    };

    // دالة مساعدة لاستخراج اللون
    auto getColor = [&]() -> std::optional<Color> {
        if (args.empty()) return std::nullopt;
        if (auto* cv = std::get_if<parser::ColorValue>(&args[0])) {
            return resolveColor(*cv);
        }
        if (auto* s = std::get_if<std::string>(&args[0])) {
            // محاولة حل اللون من نص "#RRGGBB" أو اسم عربي
            if (!s->empty() && (*s)[0] == '#') {
                return Color::fromHex(*s);
            }
            // بحث في الألوان المسماة
            auto it = arabicColors.find(*s);
            if (it != arabicColors.end()) return it->second;
        }
        return std::nullopt;
    };

    switch (modifier.type) {
        // === الألوان ===
        case parser::ModifierType::ForegroundColor:
            if (auto c = getColor()) style.foregroundColor = *c;
            break;
        case parser::ModifierType::BackgroundColor:
            if (auto c = getColor()) style.backgroundColor = *c;
            break;
        case parser::ModifierType::TintColor:
            if (auto c = getColor()) style.tintColor = *c;
            break;
        case parser::ModifierType::BorderColor:
            if (auto c = getColor()) style.borderColor = *c;
            break;

        // === الخطوط ===
        case parser::ModifierType::FontSize:
            if (auto v = getFloat()) style.fontSize = *v;
            break;
        case parser::ModifierType::FontWeight:
            if (!args.empty()) {
                if (auto* s = std::get_if<std::string>(&args[0])) {
                    // حل اسم سمك الخط
                    if (*s == "\xd8\xb9\xd8\xa7\xd8\xaf\xd9\x8a" || *s == "regular") style.fontWeight = FontWeight::Regular;
                    else if (*s == "\xd8\xb3\xd9\x85\xd9\x8a\xd9\x83" || *s == "bold") style.fontWeight = FontWeight::Bold;
                    else if (*s == "\xd8\xae\xd9\x81\xd9\x8a\xd9\x81" || *s == "light") style.fontWeight = FontWeight::Light;
                    else if (*s == "\xd9\x85\xd8\xaa\xd9\x88\xd8\xb3\xd8\xb7" || *s == "medium") style.fontWeight = FontWeight::Medium;
                    else if (*s == "\xd8\xb1\xd9\x81\xd9\x8a\xd8\xb9" || *s == "thin") style.fontWeight = FontWeight::Thin;
                    else style.fontWeight = FontWeight::Regular;
                }
            }
            break;
        case parser::ModifierType::FontFamily:
            if (!args.empty()) {
                if (auto* s = std::get_if<std::string>(&args[0])) {
                    style.fontFamily = *s;
                }
            }
            break;
        case parser::ModifierType::LineSpacing:
            if (auto v = getFloat()) style.lineSpacing = *v;
            break;

        // === الأبعاد ===
        case parser::ModifierType::Width:
            if (auto v = getFloat()) style.width = Dimension::points(*v);
            break;
        case parser::ModifierType::Height:
            if (auto v = getFloat()) style.height = Dimension::points(*v);
            break;
        case parser::ModifierType::MinWidth:
            if (auto v = getFloat()) style.minWidth = *v;
            break;
        case parser::ModifierType::MinHeight:
            if (auto v = getFloat()) style.minHeight = *v;
            break;
        case parser::ModifierType::MaxWidth:
            if (auto v = getFloat()) style.maxWidth = *v;
            break;
        case parser::ModifierType::MaxHeight:
            if (auto v = getFloat()) style.maxHeight = *v;
            break;

        // === التباعد ===
        case parser::ModifierType::Padding:
            if (auto v = getFloat()) style.padding = EdgeInsets::all(*v);
            break;
        case parser::ModifierType::PaddingTop:
            if (auto v = getFloat()) style.padding.top = *v;
            break;
        case parser::ModifierType::PaddingBottom:
            if (auto v = getFloat()) style.padding.bottom = *v;
            break;
        case parser::ModifierType::PaddingLeading:
            if (auto v = getFloat()) style.padding.leading = *v;
            break;
        case parser::ModifierType::PaddingTrailing:
            if (auto v = getFloat()) style.padding.trailing = *v;
            break;
        case parser::ModifierType::Margin:
            if (auto v = getFloat()) style.margin = EdgeInsets::all(*v);
            break;

        // === الحدود ===
        case parser::ModifierType::BorderRadius:
            if (auto v = getFloat()) style.borderRadius = *v;
            break;
        case parser::ModifierType::BorderWidth:
            if (auto v = getFloat()) style.borderWidth = *v;
            break;
        case parser::ModifierType::Border:
            if (auto v = getFloat()) style.borderWidth = *v;
            break;

        // === التأثيرات ===
        case parser::ModifierType::Opacity:
            if (auto v = getFloat()) style.opacity = *v;
            break;
        case parser::ModifierType::Shadow: {
            Shadow shadow;
            if (args.size() >= 1) { if (auto* d = std::get_if<double>(&args[0])) shadow.blur = (float)*d; }
            style.shadow = shadow;
            break;
        }
        case parser::ModifierType::Rotation:
            if (auto v = getFloat()) style.rotation = *v;
            break;
        case parser::ModifierType::Scale:
            if (auto v = getFloat()) style.scale = *v;
            break;
        case parser::ModifierType::Blur:
            // blur يُعالج كجزء من shadow أو تأثير مستقل
            break;

        // === الحالة ===
        case parser::ModifierType::Hidden:
            style.hidden = true;
            break;
        case parser::ModifierType::Disabled:
            style.disabled = true;
            break;

        // === الباقي (رسوم متحركة، وصولية) لا يُعدّل StyleIR ===
        default:
            break;
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// استخراج الأحداث من المعدّلات
// ═══════════════════════════════════════════════════════════════════════════

std::vector<EventHandlerIR> IRBuilder::extractEvents(
    const std::vector<parser::UIModifier>& modifiers
) {
    std::vector<EventHandlerIR> handlers;

    for (const auto& mod : modifiers) {
        if (!isEventModifier(mod.type)) continue;

        EventHandlerIR handler;
        handler.type = modifierToEventType(mod.type);

        // إذا كان هناك إغلاق (closure body)
        if (!mod.closureBody.empty()) {
            handler.closureBody = mod.closureBody;
        }

        // إذا كان هناك اسم دالة كوسيط
        if (!mod.arguments.empty()) {
            if (auto* s = std::get_if<std::string>(&mod.arguments[0])) {
                handler.handlerName = *s;
            }
        }

        handlers.push_back(std::move(handler));
    }

    return handlers;
}


// ═══════════════════════════════════════════════════════════════════════════
// تحويل ربط الحالة
// ═══════════════════════════════════════════════════════════════════════════

StateBindingIR IRBuilder::convertStateBinding(const parser::StateBinding& binding) {
    StateBindingIR ir;
    ir.name = binding.name;
    ir.typeName = binding.typeName;
    ir.initialValue = binding.initialValue;

    // ربط نوع الحالة
    switch (binding.type) {
        case parser::StateBindingType::State:
            ir.kind = StateBindingKind::Signal;
            break;
        case parser::StateBindingType::Binding:
            ir.kind = StateBindingKind::Binding;
            break;
        case parser::StateBindingType::Observed:
            ir.kind = StateBindingKind::Observable;
            break;
        case parser::StateBindingType::Environment:
        case parser::StateBindingType::EnvironmentObject:
            ir.kind = StateBindingKind::Environment;
            break;
        case parser::StateBindingType::Published:
            ir.kind = StateBindingKind::Observable;
            break;
        case parser::StateBindingType::AppStorage:
        case parser::StateBindingType::SceneStorage:
            ir.kind = StateBindingKind::AppStorage;
            break;
    }

    // تحديد نوع القيمة من اسم النوع
    if (binding.typeName == "\xd8\xb9\xd8\xaf\xd8\xaf" || binding.typeName == "Int" || binding.typeName == "Double") {
        ir.valueType = StateValueType::Number;
    } else if (binding.typeName == "\xd9\x86\xd8\xb5" || binding.typeName == "String") {
        ir.valueType = StateValueType::Text;
    } else if (binding.typeName == "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a" || binding.typeName == "Bool") {
        ir.valueType = StateValueType::Boolean;
    } else {
        ir.valueType = StateValueType::Custom;
    }

    return ir;
}


// ═══════════════════════════════════════════════════════════════════════════
// تحويل قيمة خاصية
// ═══════════════════════════════════════════════════════════════════════════

IRPropertyValue IRBuilder::convertPropertyValue(const parser::PropertyValue& value) {
    return std::visit([this](const auto& val) -> IRPropertyValue {
        using T = std::decay_t<decltype(val)>;

        if constexpr (std::is_same_v<T, std::monostate>) {
            return std::monostate{};
        }
        else if constexpr (std::is_same_v<T, bool>) {
            return val;
        }
        else if constexpr (std::is_same_v<T, int64_t>) {
            return val;
        }
        else if constexpr (std::is_same_v<T, double>) {
            return val;
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            return val;
        }
        else if constexpr (std::is_same_v<T, parser::ColorValue>) {
            auto c = resolveColor(val);
            return c.value_or(Color::black());
        }
        else if constexpr (std::is_same_v<T, parser::DimensionValue>) {
            Dimension d;
            switch (val.unit) {
                case parser::DimensionValue::Unit::Points:  d = Dimension::points(val.value); break;
                case parser::DimensionValue::Unit::Percent:  d = Dimension::percent(val.value); break;
                case parser::DimensionValue::Unit::Fill:     d = Dimension::fill(); break;
                case parser::DimensionValue::Unit::Fit:      d = Dimension::fit(); break;
                case parser::DimensionValue::Unit::Auto:     d = Dimension::autoSize(); break;
            }
            return d;
        }
        else if constexpr (std::is_same_v<T, parser::AlignmentValue>) {
            Alignment a;
            switch (val.horizontal) {
                case parser::AlignmentValue::Horizontal::Leading:  a.horizontal = HAlignment::Leading; break;
                case parser::AlignmentValue::Horizontal::Center:   a.horizontal = HAlignment::Center; break;
                case parser::AlignmentValue::Horizontal::Trailing: a.horizontal = HAlignment::Trailing; break;
            }
            switch (val.vertical) {
                case parser::AlignmentValue::Vertical::Top:    a.vertical = VAlignment::Top; break;
                case parser::AlignmentValue::Vertical::Center: a.vertical = VAlignment::Center; break;
                case parser::AlignmentValue::Vertical::Bottom: a.vertical = VAlignment::Bottom; break;
            }
            return a;
        }
        else if constexpr (std::is_same_v<T, parser::EdgeInsetsValue>) {
            // EdgeInsetsValue ليس في IRPropertyValue variant، نحوّله لنص
            return std::string("insets");
        }
        else if constexpr (std::is_same_v<T, std::shared_ptr<parser::PropertyValueArray>>) {
            // المصفوفات — نحوّل أول عنصر
            if (val && !val->values.empty()) {
                return this->convertPropertyValue(val->values[0]);
            }
            return std::monostate{};
        }
        else {
            return std::monostate{};
        }
    }, value);
}


// ═══════════════════════════════════════════════════════════════════════════
// حل اللون
// ═══════════════════════════════════════════════════════════════════════════

std::optional<Color> IRBuilder::resolveColor(const parser::ColorValue& colorVal) {
    switch (colorVal.type) {
        case parser::ColorValue::Type::Named: {
            // البحث في الألوان العربية المسماة
            auto it = arabicColors.find(colorVal.namedColor);
            if (it != arabicColors.end()) return it->second;
            // اللون غير معروف
            addDiagnostic(DiagnosticLevel::Warning,
                          "\xd9\x84\xd9\x88\xd9\x86 \xd8\xba\xd9\x8a\xd8\xb1 \xd9\x85\xd8\xb9\xd8\xb1\xd9\x88\xd9\x81: " + colorVal.namedColor,
                          "Unknown named color: " + colorVal.namedColor);
            return std::nullopt;
        }
        case parser::ColorValue::Type::RGB:
            return Color(colorVal.r, colorVal.g, colorVal.b);
        case parser::ColorValue::Type::RGBA:
            return Color(colorVal.r, colorVal.g, colorVal.b, colorVal.alpha);
        case parser::ColorValue::Type::Hex:
            return Color::fromHex(colorVal.hexValue);
    }
    return std::nullopt;
}


} // namespace ir
} // namespace ui
} // namespace sad
