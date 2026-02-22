/**
 * ==========================================================================
 * ملف: ios_swiftui_codegen.cpp
 * الوصف: تنفيذ مولد كود iOS SwiftUI من SadUI IR
 * ==========================================================================
 * بسم الله الرحمن الرحيم
 */

#include "ui/ios_swiftui_codegen.hpp"
#include <iomanip>
#include <algorithm>

namespace sad {
namespace ui {
namespace ir {


IOSSwiftUICodeGen::IOSSwiftUICodeGen(const IOSCodeGenConfig& config)
    : config_(config)
{}


// ═══════════════════════════════════════════════════════════════════════════
// توليد المشروع الكامل
// ═══════════════════════════════════════════════════════════════════════════

CodeGenResult IOSSwiftUICodeGen::generate(const AppIR& app) {
    CodeGenResult result;
    result.success = true;

    std::string appName = app.appName.empty() ? "SadApp" : app.appName;
    std::string srcDir = appName + "/Sources/";

    // 1. توليد شاشات SwiftUI
    for (const auto& screen : app.screens) {
        std::string code = generateScreenSwift(screen, app.theme);
        std::string filename = srcDir + screen.name + "View.swift";
        result.files.push_back({filename, code, false, "swift"});
    }

    // 2. App.swift (entry point)
    result.files.push_back({
        srcDir + appName + "App.swift",
        generateAppSwift(app),
        false, "swift"
    });

    // 3. Theme.swift
    result.files.push_back({
        srcDir + "Theme.swift",
        generateThemeSwift(app),
        false, "swift"
    });

    // 4. Navigation Router
    if (app.screens.size() > 1) {
        result.files.push_back({
            srcDir + "AppRouter.swift",
            generateNavigationSwift(app),
            false, "swift"
        });
    }

    // 5. Package.swift
    result.files.push_back({
        appName + "/Package.swift",
        generatePackageSwift(app),
        false, "swift"
    });

    // 6. Info.plist
    result.files.push_back({
        appName + "/Info.plist",
        generateInfoPlist(app),
        false, "xml"
    });

    result.buildCommand = "cd " + appName + " && swift build";
    return result;
}


CodeGenResult IOSSwiftUICodeGen::generateScreen(const ScreenIR& screen) {
    CodeGenResult result;
    result.success = true;
    ThemeIR defaultTheme;
    std::string code = generateScreenSwift(screen, defaultTheme);
    result.files.push_back({screen.name + "View.swift", code, false, "swift"});
    return result;
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد شاشة SwiftUI
// ═══════════════════════════════════════════════════════════════════════════

std::string IOSSwiftUICodeGen::generateScreenSwift(
    const ScreenIR& screen,
    const ThemeIR& theme
) {
    std::ostringstream ss;

    // Header
    ss << "//\n";
    ss << "// " << screen.name << "View.swift\n";
    if (config_.includeArabicComments) {
        ss << "// \xd8\xaa\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd9\x88\xd9\x84\xd9\x8a\xd8\xaf \xd8\xaa\xd9\x84\xd9\x82\xd8\xa7\xd8\xa6\xd9\x8a\xd8\xa7\xd9\x8b \xd8\xa8\xd9\x88\xd8\xa7\xd8\xb3\xd8\xb7\xd8\xa9 SadUI\n"; // تم التوليد تلقائياً بواسطة SadUI
    }
    ss << "//\n\n";
    ss << "import SwiftUI\n\n";

    // Struct definition
    ss << "struct " << screen.name << "View: View {\n";

    // State properties
    if (!screen.stateBindings.empty()) {
        ss << "\n";
        emitStateProperties(ss, screen.stateBindings, 1);
        ss << "\n";
    }

    // Body
    ss << "    var body: some View {\n";

    if (screen.rootNode) {
        emitView(ss, screen.rootNode, theme, 2);
    } else {
        ss << "        EmptyView()\n";
    }

    ss << "    }\n";
    ss << "}\n";

    // Preview
    if (config_.generatePreviews) {
        ss << "\n#Preview {\n";
        ss << "    " << screen.name << "View()\n";
        ss << "}\n";
    }

    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد عقدة SwiftUI (تكراري)
// ═══════════════════════════════════════════════════════════════════════════

void IOSSwiftUICodeGen::emitView(
    std::ostringstream& ss,
    const UINodeIRPtr& node,
    const ThemeIR& theme,
    int indent
) {
    if (!node) return;

    const auto& style = node->style;
    bool hasChildren = !node->children.empty();

    switch (node->type) {
        // ─── النصوص ─────────────────────────────────────
        case WidgetType::Text: {
            std::string content = "\"\"";
            auto it = node->properties.find("content");
            if (it != node->properties.end()) {
                if (auto* s = std::get_if<std::string>(&it->second)) {
                    content = "\"" + *s + "\"";
                }
            }
            ss << pad(indent) << "Text(" << content << ")\n";
            emitModifiers(ss, style, indent);
            break;
        }

        // ─── الأزرار ────────────────────────────────────
        case WidgetType::Button: {
            std::string label = "\"\"";
            auto it = node->properties.find("content");
            if (it != node->properties.end()) {
                if (auto* s = std::get_if<std::string>(&it->second)) {
                    label = "\"" + *s + "\"";
                }
            }

            if (hasChildren) {
                // Button with custom content
                ss << pad(indent) << "Button(action: {\n";
                // Action from events
                for (const auto& ev : node->eventHandlers) {
                    if (ev.type == EventType::Tap) {
                        ss << pad(indent+1) << "// " << ev.handlerName << "()\n";
                    }
                }
                ss << pad(indent) << "}) {\n";
                for (const auto& child : node->children) {
                    emitView(ss, child, theme, indent + 1);
                }
                ss << pad(indent) << "}\n";
            } else {
                ss << pad(indent) << "Button(" << label << ") {\n";
                for (const auto& ev : node->eventHandlers) {
                    if (ev.type == EventType::Tap) {
                        ss << pad(indent+1) << "// " << ev.handlerName << "()\n";
                    }
                }
                ss << pad(indent) << "}\n";
            }
            emitModifiers(ss, style, indent);
            break;
        }

        // ─── حقل نص ────────────────────────────────────
        case WidgetType::TextField: {
            std::string placeholder = "\"\"";
            auto it = node->properties.find("placeholder");
            if (it != node->properties.end()) {
                if (auto* s = std::get_if<std::string>(&it->second)) {
                    placeholder = "\"" + *s + "\"";
                }
            }
            // Find state binding for text
            std::string binding = "$text";
            if (node->stateBinding) {
                binding = "$" + node->stateBinding->name;
            }
            ss << pad(indent) << "TextField(" << placeholder << ", text: " << binding << ")\n";
            emitModifiers(ss, style, indent);
            break;
        }

        // ─── Toggle ────────────────────────────────────
        case WidgetType::Toggle: {
            std::string label = "\"\"";
            auto it = node->properties.find("content");
            if (it != node->properties.end()) {
                if (auto* s = std::get_if<std::string>(&it->second)) {
                    label = "\"" + *s + "\"";
                }
            }
            std::string binding = "$isOn";
            if (node->stateBinding) {
                binding = "$" + node->stateBinding->name;
            }
            ss << pad(indent) << "Toggle(" << label << ", isOn: " << binding << ")\n";
            emitModifiers(ss, style, indent);
            break;
        }

        // ─── Slider ────────────────────────────────────
        case WidgetType::Slider: {
            std::string binding = "$value";
            if (node->stateBinding) {
                binding = "$" + node->stateBinding->name;
            }
            ss << pad(indent) << "Slider(value: " << binding << ")\n";
            emitModifiers(ss, style, indent);
            break;
        }

        // ─── Image ─────────────────────────────────────
        case WidgetType::Image: {
            std::string source = "\"placeholder\"";
            auto it = node->properties.find("source");
            if (it != node->properties.end()) {
                if (auto* s = std::get_if<std::string>(&it->second)) {
                    source = "\"" + *s + "\"";
                }
            }
            ss << pad(indent) << "Image(" << source << ")\n";
            ss << pad(indent) << "    .resizable()\n";
            ss << pad(indent) << "    .aspectRatio(contentMode: .fit)\n";
            emitModifiers(ss, style, indent);
            break;
        }

        // ─── Column (VStack) ───────────────────────────
        case WidgetType::Column:
        case WidgetType::LazyColumn: {
            std::string viewName = (node->type == WidgetType::LazyColumn) ? "LazyVStack" : "VStack";

            // Alignment
            std::string alignment = "";
            if (style.horizontalAlignment) {
                switch (*style.horizontalAlignment) {
                    case HAlignment::Leading: alignment = "alignment: .leading"; break;
                    case HAlignment::Center: alignment = "alignment: .center"; break;
                    case HAlignment::Trailing: alignment = "alignment: .trailing"; break;
                }
            }

            ss << pad(indent) << viewName << "(" << alignment << ") {\n";
            for (const auto& child : node->children) {
                emitView(ss, child, theme, indent + 1);
            }
            ss << pad(indent) << "}\n";
            emitModifiers(ss, style, indent);
            break;
        }

        // ─── Row (HStack) ──────────────────────────────
        case WidgetType::Row:
        case WidgetType::LazyRow: {
            std::string viewName = (node->type == WidgetType::LazyRow) ? "LazyHStack" : "HStack";

            std::string alignment = "";
            if (style.verticalAlignment) {
                switch (*style.verticalAlignment) {
                    case VAlignment::Top: alignment = "alignment: .top"; break;
                    case VAlignment::Center: alignment = "alignment: .center"; break;
                    case VAlignment::Bottom: alignment = "alignment: .bottom"; break;
                }
            }

            ss << pad(indent) << viewName << "(" << alignment << ") {\n";
            for (const auto& child : node->children) {
                emitView(ss, child, theme, indent + 1);
            }
            ss << pad(indent) << "}\n";
            emitModifiers(ss, style, indent);
            break;
        }

        // ─── Stack (ZStack) ────────────────────────────
        case WidgetType::Stack: {
            ss << pad(indent) << "ZStack {\n";
            for (const auto& child : node->children) {
                emitView(ss, child, theme, indent + 1);
            }
            ss << pad(indent) << "}\n";
            emitModifiers(ss, style, indent);
            break;
        }

        // ─── ScrollView ────────────────────────────────
        case WidgetType::ScrollView: {
            ss << pad(indent) << "ScrollView {\n";
            for (const auto& child : node->children) {
                emitView(ss, child, theme, indent + 1);
            }
            ss << pad(indent) << "}\n";
            emitModifiers(ss, style, indent);
            break;
        }

        // ─── List ──────────────────────────────────────
        case WidgetType::List: {
            ss << pad(indent) << "List {\n";
            for (const auto& child : node->children) {
                emitView(ss, child, theme, indent + 1);
            }
            ss << pad(indent) << "}\n";
            emitModifiers(ss, style, indent);
            break;
        }

        // ─── Scaffold (NavigationStack + VStack) ──────
        case WidgetType::Scaffold: {
            ss << pad(indent) << "NavigationStack {\n";
            // Separate AppBar from content
            for (const auto& child : node->children) {
                if (child->type == WidgetType::AppBar) {
                    // Will be handled as .navigationTitle
                    std::string title = "\"\"";
                    auto tit = child->properties.find("content");
                    if (tit != child->properties.end()) {
                        if (auto* s = std::get_if<std::string>(&tit->second)) {
                            title = "\"" + *s + "\"";
                        }
                    }
                    // Emit content first, then toolbar modifier
                    continue;
                }
                emitView(ss, child, theme, indent + 1);
            }
            // Navigation title from AppBar
            for (const auto& child : node->children) {
                if (child->type == WidgetType::AppBar) {
                    std::string title = "\"\"";
                    auto tit = child->properties.find("content");
                    if (tit != child->properties.end()) {
                        if (auto* s = std::get_if<std::string>(&tit->second)) {
                            title = "\"" + *s + "\"";
                        }
                    }
                    ss << pad(indent) << "    .navigationTitle(" << title << ")\n";
                    break;
                }
            }
            ss << pad(indent) << "}\n";
            emitModifiers(ss, style, indent);
            break;
        }

        // ─── AppBar → handled within Scaffold ─────────
        case WidgetType::AppBar: {
            // Standalone: just emit content
            auto it = node->properties.find("content");
            if (it != node->properties.end()) {
                if (auto* s = std::get_if<std::string>(&it->second)) {
                    ss << pad(indent) << "Text(\"" << *s << "\")\n";
                    ss << pad(indent) << "    .font(.title)\n";
                    ss << pad(indent) << "    .bold()\n";
                }
            }
            break;
        }

        // ─── Card (GroupBox) ───────────────────────────
        case WidgetType::Card: {
            ss << pad(indent) << "GroupBox {\n";
            for (const auto& child : node->children) {
                emitView(ss, child, theme, indent + 1);
            }
            ss << pad(indent) << "}\n";
            emitModifiers(ss, style, indent);
            break;
        }

        // ─── Container / Box / Surface → Group ────────
        case WidgetType::Container:
        case WidgetType::Box:
        case WidgetType::Surface: {
            if (hasChildren) {
                ss << pad(indent) << "Group {\n";
                for (const auto& child : node->children) {
                    emitView(ss, child, theme, indent + 1);
                }
                ss << pad(indent) << "}\n";
            } else {
                ss << pad(indent) << "Color.clear\n";
            }
            emitModifiers(ss, style, indent);
            break;
        }

        // ─── Spacer ────────────────────────────────────
        case WidgetType::Spacer: {
            ss << pad(indent) << "Spacer()\n";
            emitModifiers(ss, style, indent);
            break;
        }

        // ─── Divider ───────────────────────────────────
        case WidgetType::Divider: {
            ss << pad(indent) << "Divider()\n";
            emitModifiers(ss, style, indent);
            break;
        }

        // ─── ProgressBar ──────────────────────────────
        case WidgetType::ProgressBar: {
            ss << pad(indent) << "ProgressView()\n";
            emitModifiers(ss, style, indent);
            break;
        }

        // ─── CircleProgress ───────────────────────────
        case WidgetType::CircleProgress: {
            ss << pad(indent) << "ProgressView()\n";
            ss << pad(indent) << "    .progressViewStyle(.circular)\n";
            emitModifiers(ss, style, indent);
            break;
        }

        // ─── TabView ──────────────────────────────────
        case WidgetType::TabView: {
            ss << pad(indent) << "TabView {\n";
            for (const auto& child : node->children) {
                emitView(ss, child, theme, indent + 1);
            }
            ss << pad(indent) << "}\n";
            emitModifiers(ss, style, indent);
            break;
        }

        // ─── Dialog / Sheet ───────────────────────────
        case WidgetType::Dialog:
        case WidgetType::BottomSheet: {
            // As a sheet presentation
            ss << pad(indent) << "// TODO: Present as sheet\n";
            if (hasChildren) {
                for (const auto& child : node->children) {
                    emitView(ss, child, theme, indent);
                }
            }
            break;
        }

        // ─── ForEach ──────────────────────────────────
        case WidgetType::ForEach: {
            std::string items = "items";
            auto it = node->properties.find("dataSource");
            if (it != node->properties.end()) {
                if (auto* s = std::get_if<std::string>(&it->second)) {
                    items = *s;
                }
            }
            ss << pad(indent) << "ForEach(" << items << ", id: \\.self) { item in\n";
            for (const auto& child : node->children) {
                emitView(ss, child, theme, indent + 1);
            }
            ss << pad(indent) << "}\n";
            break;
        }

        // ─── Group (passthrough) ──────────────────────
        case WidgetType::Group: {
            if (hasChildren) {
                for (const auto& child : node->children) {
                    emitView(ss, child, theme, indent);
                }
            }
            break;
        }

        // ─── الافتراضي ──────────────────────────────
        default: {
            if (config_.includeArabicComments) {
                ss << pad(indent) << "// \xd9\x85\xd9\x83\xd9\x88\xd9\x86 \xd8\xba\xd9\x8a\xd8\xb1 \xd9\x85\xd8\xaf\xd8\xb9\xd9\x88\xd9\x85: "; // مكون غير مدعوم
                ss << static_cast<int>(node->type) << "\n";
            }
            ss << pad(indent) << "EmptyView()\n";
            break;
        }
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد Modifiers
// ═══════════════════════════════════════════════════════════════════════════

void IOSSwiftUICodeGen::emitModifiers(
    std::ostringstream& ss,
    const StyleIR& style,
    int indent
) {
    std::string p = pad(indent) + "    ";

    // Frame (width/height) — Dimension is a struct, not optional
    if (!style.width.isAuto() || !style.height.isAuto()) {
        ss << p << ".frame(";
        bool first = true;
        if (!style.width.isAuto() && !style.width.isFill()) {
            ss << "width: " << style.width.value;
            first = false;
        }
        if (!style.height.isAuto() && !style.height.isFill()) {
            if (!first) ss << ", ";
            ss << "height: " << style.height.value;
        }
        ss << ")\n";
    }

    // Full width/height (Fill dimension)
    if (style.width.isFill()) {
        ss << p << ".frame(maxWidth: .infinity)\n";
    }
    if (style.height.isFill()) {
        ss << p << ".frame(maxHeight: .infinity)\n";
    }

    // Padding — EdgeInsets is a struct, not optional
    {
        const auto& pad_ = style.padding;
        if (pad_.top > 0 || pad_.bottom > 0 || pad_.leading > 0 || pad_.trailing > 0) {
            if (pad_.top == pad_.bottom && pad_.top == pad_.leading && pad_.top == pad_.trailing) {
                ss << p << ".padding(" << pad_.top << ")\n";
            } else {
                ss << p << ".padding(EdgeInsets(top: " << pad_.top
                   << ", leading: " << pad_.leading
                   << ", bottom: " << pad_.bottom
                   << ", trailing: " << pad_.trailing << "))\n";
            }
        }
    }

    // Background color
    if (style.backgroundColor) {
        ss << p << ".background(" << swiftColor(*style.backgroundColor) << ")\n";
    }

    // Foreground/text color — field is foregroundColor, not textColor
    if (style.foregroundColor) {
        ss << p << ".foregroundColor(" << swiftColor(*style.foregroundColor) << ")\n";
    }

    // Font
    if (style.fontSize) {
        if (style.fontWeight) {
            std::string weight = "regular";
            switch (*style.fontWeight) {
                case FontWeight::Thin: weight = "thin"; break;
                case FontWeight::Light: weight = "light"; break;
                case FontWeight::Regular: weight = "regular"; break;
                case FontWeight::Medium: weight = "medium"; break;
                case FontWeight::SemiBold: weight = "semibold"; break;
                case FontWeight::Bold: weight = "bold"; break;
                case FontWeight::ExtraBold: weight = "heavy"; break;
                case FontWeight::Black: weight = "black"; break;
            }
            ss << p << ".font(.system(size: " << *style.fontSize
               << ", weight: ." << weight << "))\n";
        } else {
            ss << p << ".font(.system(size: " << *style.fontSize << "))\n";
        }
    } else if (style.fontWeight) {
        std::string weight = "regular";
        switch (*style.fontWeight) {
            case FontWeight::Bold: weight = "bold"; break;
            case FontWeight::SemiBold: weight = "semibold"; break;
            case FontWeight::Medium: weight = "medium"; break;
            default: break;
        }
        ss << p << ".fontWeight(." << weight << ")\n";
    }

    // Corner radius — borderRadius is float, not optional
    if (style.borderRadius > 0) {
        ss << p << ".cornerRadius(" << style.borderRadius << ")\n";
    }

    // Border — borderWidth is float, not optional
    if (style.borderWidth > 0 && style.borderColor) {
        ss << p << ".overlay(\n";
        ss << p << "    RoundedRectangle(cornerRadius: " << style.borderRadius << ")\n";
        ss << p << "        .stroke(" << swiftColor(*style.borderColor)
           << ", lineWidth: " << style.borderWidth << ")\n";
        ss << p << ")\n";
    }

    // Opacity — float, not optional (default 1.0)
    if (style.opacity < 1.0f) {
        ss << p << ".opacity(" << style.opacity << ")\n";
    }

    // Shadow
    if (style.shadow) {
        ss << p << ".shadow(color: " << swiftColor(style.shadow->color)
           << ", radius: " << style.shadow->blur
           << ", x: " << style.shadow->offsetX
           << ", y: " << style.shadow->offsetY << ")\n";
    }

    // Hidden — bool, not optional
    if (style.hidden) {
        ss << p << ".hidden()\n";
    }

    // Margin (using padding on outer container) — EdgeInsets is a struct, not optional
    {
        const auto& m = style.margin;
        if (m.top > 0 || m.bottom > 0 || m.leading > 0 || m.trailing > 0) {
            ss << p << ".padding(EdgeInsets(top: " << m.top
               << ", leading: " << m.leading
               << ", bottom: " << m.bottom
               << ", trailing: " << m.trailing << "))\n";
        }
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد State properties
// ═══════════════════════════════════════════════════════════════════════════

void IOSSwiftUICodeGen::emitStateProperties(
    std::ostringstream& ss,
    const std::vector<StateBindingIR>& bindings,
    int indent
) {
    for (const auto& sb : bindings) {
        std::string wrapper;
        std::string swiftType;
        std::string defaultVal;

        // Determine wrapper
        switch (sb.kind) {
            case StateBindingKind::Signal:
                wrapper = "@State private var";
                break;
            case StateBindingKind::Computed:
                // Computed properties don't use property wrappers
                wrapper = "private var";
                break;
            case StateBindingKind::Binding:
                wrapper = "@Binding var";
                break;
            case StateBindingKind::Observable:
                wrapper = config_.useObservable ? "@Observable var" : "@StateObject private var";
                break;
            case StateBindingKind::Environment:
                wrapper = "@Environment var";
                break;
            case StateBindingKind::AppStorage:
                wrapper = "@AppStorage(\"" + sb.name + "\") private var";
                break;
            default:
                wrapper = "@State private var";
                break;
        }

        // Determine Swift type and default
        switch (sb.valueType) {
            case StateValueType::Text:
                swiftType = "String";
                defaultVal = sb.initialValue.empty() ? "\"\"" : "\"" + sb.initialValue + "\"";
                break;
            case StateValueType::Number:
                swiftType = "Double";
                defaultVal = sb.initialValue.empty() ? "0" : sb.initialValue;
                break;
            case StateValueType::Boolean:
                swiftType = "Bool";
                defaultVal = sb.initialValue.empty() ? "false" : sb.initialValue;
                break;
            case StateValueType::NumberList:
                swiftType = "[Double]";
                defaultVal = "[]";
                break;
            case StateValueType::TextList:
                swiftType = "[String]";
                defaultVal = "[]";
                break;
            case StateValueType::ObjectList:
                swiftType = "[Any]";
                defaultVal = "[]";
                break;
            case StateValueType::Map:
                swiftType = "[String: Any]";
                defaultVal = "[:]";
                break;
            case StateValueType::Custom:
                swiftType = sb.typeName.empty() ? "Any?" : sb.typeName;
                defaultVal = "nil";
                break;
            default:
                swiftType = "String";
                defaultVal = "\"\"";
                break;
        }

        if (sb.kind == StateBindingKind::Computed) {
            ss << pad(indent) << wrapper << " " << sb.name << ": " << swiftType << " {\n";
            if (sb.computeExpression && !sb.computeExpression->empty()) {
                ss << pad(indent+1) << *sb.computeExpression << "\n";
            } else {
                ss << pad(indent+1) << defaultVal << "\n";
            }
            ss << pad(indent) << "}\n";
        } else {
            ss << pad(indent) << wrapper << " " << sb.name
               << ": " << swiftType << " = " << defaultVal << "\n";
        }
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد App.swift
// ═══════════════════════════════════════════════════════════════════════════

std::string IOSSwiftUICodeGen::generateAppSwift(const AppIR& app) {
    std::ostringstream ss;
    std::string appName = app.appName.empty() ? "SadApp" : app.appName;

    ss << "//\n";
    ss << "// " << appName << "App.swift\n";
    ss << "// \xd8\xaa\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd9\x88\xd9\x84\xd9\x8a\xd8\xaf \xd8\xa8\xd9\x88\xd8\xa7\xd8\xb3\xd8\xb7\xd8\xa9 SadUI\n"; // تم التوليد بواسطة SadUI
    ss << "//\n\n";
    ss << "import SwiftUI\n\n";

    ss << "@main\n";
    ss << "struct " << appName << "App: App {\n";
    ss << "    var body: some Scene {\n";
    ss << "        WindowGroup {\n";

    if (app.screens.size() > 1) {
        ss << "            AppRouter()\n";
    } else if (!app.screens.empty()) {
        ss << "            " << app.screens[0].name << "View()\n";
    } else {
        ss << "            Text(\"SadApp\")\n";
    }

    ss << "        }\n";
    ss << "    }\n";
    ss << "}\n";

    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد ملف الثيمة
// ═══════════════════════════════════════════════════════════════════════════

std::string IOSSwiftUICodeGen::generateThemeSwift(const AppIR& app) {
    std::ostringstream ss;
    const auto& theme = app.theme;

    ss << "//\n";
    ss << "// Theme.swift\n";
    ss << "//\n\n";
    ss << "import SwiftUI\n\n";

    // Color extension
    ss << "extension Color {\n";
    ss << "    static let sadPrimary = " << swiftColor(theme.primaryColor) << "\n";
    ss << "    static let sadSecondary = " << swiftColor(theme.secondaryColor) << "\n";
    ss << "    static let sadBackground = " << swiftColor(theme.backgroundColor) << "\n";
    ss << "    static let sadSurface = " << swiftColor(theme.surfaceColor) << "\n";
    ss << "    static let sadError = " << swiftColor(theme.errorColor) << "\n";
    ss << "    static let sadText = " << swiftColor(theme.textPrimary) << "\n";
    ss << "}\n\n";

    // Font extension
    ss << "extension Font {\n";
    ss << "    static let sadBody = Font.system(size: " << theme.fontSizeBody << ")\n";
    ss << "    static let sadTitle = Font.system(size: " << theme.fontSizeTitle << ", weight: .bold)\n";
    ss << "    static let sadCaption = Font.system(size: " << theme.fontSizeCaption << ")\n";
    ss << "}\n\n";

    // ViewModifier for theme
    ss << "struct SadThemeModifier: ViewModifier {\n";
    ss << "    func body(content: Content) -> some View {\n";
    ss << "        content\n";
    ss << "            .foregroundColor(.sadText)\n";
    ss << "            .font(.sadBody)\n";
    if (theme.isRTL) {
        ss << "            .environment(\\.layoutDirection, .rightToLeft)\n";
    }
    ss << "    }\n";
    ss << "}\n\n";

    ss << "extension View {\n";
    ss << "    func sadTheme() -> some View {\n";
    ss << "        self.modifier(SadThemeModifier())\n";
    ss << "    }\n";
    ss << "}\n";

    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد Navigation Router
// ═══════════════════════════════════════════════════════════════════════════

std::string IOSSwiftUICodeGen::generateNavigationSwift(const AppIR& app) {
    std::ostringstream ss;

    ss << "//\n";
    ss << "// AppRouter.swift\n";
    ss << "//\n\n";
    ss << "import SwiftUI\n\n";

    // Route enum
    ss << "enum AppRoute: Hashable {\n";
    for (const auto& screen : app.screens) {
        ss << "    case " << screen.name << "\n";
    }
    ss << "}\n\n";

    // Router view
    ss << "struct AppRouter: View {\n";
    ss << "    @State private var path = NavigationPath()\n\n";
    ss << "    var body: some View {\n";
    ss << "        NavigationStack(path: $path) {\n";

    // Initial screen
    std::string initialScreen = app.initialScreenName;
    if (initialScreen.empty() && !app.screens.empty()) {
        initialScreen = app.screens[0].name;
    }
    ss << "            " << initialScreen << "View()\n";

    ss << "                .navigationDestination(for: AppRoute.self) { route in\n";
    ss << "                    switch route {\n";
    for (const auto& screen : app.screens) {
        ss << "                    case ." << screen.name << ":\n";
        ss << "                        " << screen.name << "View()\n";
    }
    ss << "                    }\n";
    ss << "                }\n";
    ss << "        }\n";
    ss << "    }\n\n";

    // Navigate function
    ss << "    func navigate(to route: AppRoute) {\n";
    ss << "        path.append(route)\n";
    ss << "    }\n\n";

    ss << "    func goBack() {\n";
    ss << "        path.removeLast()\n";
    ss << "    }\n";
    ss << "}\n";

    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد Package.swift (SPM)
// ═══════════════════════════════════════════════════════════════════════════

std::string IOSSwiftUICodeGen::generatePackageSwift(const AppIR& app) {
    std::ostringstream ss;
    std::string appName = app.appName.empty() ? "SadApp" : app.appName;

    ss << "// swift-tools-version: " << config_.swiftVersion << "\n";
    ss << "import PackageDescription\n\n";
    ss << "let package = Package(\n";
    ss << "    name: \"" << appName << "\",\n";
    ss << "    platforms: [\n";
    ss << "        .iOS(.v" << config_.deploymentTarget.substr(0, 2) << "),\n";
    if (config_.supportIPad) {
        ss << "        .macOS(.v13),\n";
    }
    ss << "    ],\n";
    ss << "    products: [\n";
    ss << "        .library(name: \"" << appName << "\", targets: [\"" << appName << "\"]),\n";
    ss << "    ],\n";
    ss << "    targets: [\n";
    ss << "        .target(\n";
    ss << "            name: \"" << appName << "\",\n";
    ss << "            path: \"Sources\"\n";
    ss << "        ),\n";
    ss << "    ]\n";
    ss << ")\n";

    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد Info.plist
// ═══════════════════════════════════════════════════════════════════════════

std::string IOSSwiftUICodeGen::generateInfoPlist(const AppIR& app) {
    std::ostringstream ss;
    std::string appName = app.appName.empty() ? "SadApp" : app.appName;
    std::string bundleId = config_.bundleIdentifier.empty()
        ? "com.sad." + appName : config_.bundleIdentifier;

    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    ss << "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\"\n";
    ss << "    \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n";
    ss << "<plist version=\"1.0\">\n";
    ss << "<dict>\n";

    ss << "    <key>CFBundleDevelopmentRegion</key>\n";
    if (app.theme.isRTL) {
        ss << "    <string>ar</string>\n";
    } else {
        ss << "    <string>en</string>\n";
    }

    ss << "    <key>CFBundleExecutable</key>\n";
    ss << "    <string>$(EXECUTABLE_NAME)</string>\n";

    ss << "    <key>CFBundleIdentifier</key>\n";
    ss << "    <string>" << bundleId << "</string>\n";

    ss << "    <key>CFBundleName</key>\n";
    ss << "    <string>" << appName << "</string>\n";

    ss << "    <key>CFBundlePackageType</key>\n";
    ss << "    <string>APPL</string>\n";

    ss << "    <key>CFBundleShortVersionString</key>\n";
    ss << "    <string>1.0</string>\n";

    ss << "    <key>CFBundleVersion</key>\n";
    ss << "    <string>1</string>\n";

    ss << "    <key>LSRequiresIPhoneOS</key>\n";
    ss << "    <true/>\n";

    if (config_.supportIPad) {
        ss << "    <key>UIDeviceFamily</key>\n";
        ss << "    <array>\n";
        ss << "        <integer>1</integer>\n"; // iPhone
        ss << "        <integer>2</integer>\n"; // iPad
        ss << "    </array>\n";
    }

    ss << "    <key>UILaunchStoryboardName</key>\n";
    ss << "    <string>LaunchScreen</string>\n";

    ss << "    <key>UISupportedInterfaceOrientations</key>\n";
    ss << "    <array>\n";
    ss << "        <string>UIInterfaceOrientationPortrait</string>\n";
    ss << "        <string>UIInterfaceOrientationLandscapeLeft</string>\n";
    ss << "        <string>UIInterfaceOrientationLandscapeRight</string>\n";
    ss << "    </array>\n";

    ss << "</dict>\n";
    ss << "</plist>\n";

    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// أدوات مساعدة
// ═══════════════════════════════════════════════════════════════════════════

std::string IOSSwiftUICodeGen::swiftColor(const Color& c) {
    // Color.a is float (0.0-1.0), r/g/b are uint8_t
    if (c.a < 1.0f) {
        return "Color(red: " + std::to_string(c.r / 255.0)
             + ", green: " + std::to_string(c.g / 255.0)
             + ", blue: " + std::to_string(c.b / 255.0)
             + ", opacity: " + std::to_string(c.a) + ")";
    }
    return "Color(red: " + std::to_string(c.r / 255.0)
         + ", green: " + std::to_string(c.g / 255.0)
         + ", blue: " + std::to_string(c.b / 255.0) + ")";
}


std::string IOSSwiftUICodeGen::pad(int indent) {
    return std::string(indent * 4, ' ');
}


} // namespace ir
} // namespace ui
} // namespace sad
