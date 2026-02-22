/**
 * ==========================================================================
 * ملف: desktop_sdl2_codegen.cpp
 * الوصف: تنفيذ مولد كود سطح المكتب — SadUI IR → C++/SDL2
 * ==========================================================================
 * بسم الله الرحمن الرحيم
 */

#include "ui/desktop_sdl2_codegen.hpp"
#include <sstream>
#include <algorithm>
#include <cstdio>  // for snprintf

namespace sad {
namespace ui {
namespace ir {

// ═══════════════════════════════════════════════════════════════════════════
// دوال الأمان / Security Functions
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief Escape string for C++ string literal (prevents code injection)
 * تهريب النص لاستخدامه كنص C++ (يمنع حقن الكود)
 * 
 * @param input النص المُدخل / Input string
 * @return النص المُهرَّب / Escaped string
 */
inline std::string escapeCppString(const std::string& input) {
    std::string result;
    result.reserve(input.length() * 2);  // Reserve extra space
    
    for (unsigned char c : input) {
        switch (c) {
            case '\\': result += "\\\\"; break;  // Backslash
            case '"':  result += "\\\""; break;  // Double quote
            case '\'': result += "\\'"; break;   // Single quote
            case '\n': result += "\\n"; break;   // Newline
            case '\r': result += "\\r"; break;   // Carriage return
            case '\t': result += "\\t"; break;   // Tab
            case '\a': result += "\\a"; break;   // Bell
            case '\b': result += "\\b"; break;   // Backspace
            case '\f': result += "\\f"; break;   // Form feed
            case '\v': result += "\\v"; break;   // Vertical tab
            case '\0': result += "\\0"; break;   // Null
            default:
                // Check for non-printable characters
                if (c < 32 || c == 127) {
                    // Use octal escape for control chars
                    char oct[8];
                    std::snprintf(oct, sizeof(oct), "\\%03o", c);
                    result += oct;
                } else {
                    result += static_cast<char>(c);
                }
        }
    }
    return result;
}

DesktopSDL2CodeGen::DesktopSDL2CodeGen(const DesktopCodeGenConfig& config)
    : config_(config) {}


// ═══════════════════════════════════════════════════════════════════════════
// توليد تطبيق كامل
// ═══════════════════════════════════════════════════════════════════════════

CodeGenResult DesktopSDL2CodeGen::generate(const AppIR& app) {
    CodeGenResult result;
    result.success = true;

    ThemeIR theme = app.theme;

    // 1. توليد ملفات الشاشات
    for (const auto& screen : app.screens) {
        GeneratedFile file;
        file.path = "src/" + screen.name + ".cpp";
        file.content = generateScreenSource(screen, theme);
        file.language = "cpp";
        result.files.push_back(std::move(file));
    }

    // 2. توليد main.cpp
    if (config_.generateLauncher) {
        GeneratedFile mainFile;
        mainFile.path = "src/main.cpp";
        mainFile.content = generateMainCpp(app);
        mainFile.language = "cpp";
        result.files.push_back(std::move(mainFile));
    }

    // 3. توليد CMakeLists.txt
    {
        GeneratedFile cmake;
        cmake.path = "CMakeLists.txt";
        cmake.content = generateCMakeLists(app);
        cmake.language = "cmake";
        result.files.push_back(std::move(cmake));
    }

    result.buildCommand = "cmake -S . -B build && cmake --build build";
    return result;
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد شاشة واحدة
// ═══════════════════════════════════════════════════════════════════════════

CodeGenResult DesktopSDL2CodeGen::generateScreen(const ScreenIR& screen) {
    CodeGenResult result;
    result.success = true;

    GeneratedFile file;
    file.path = "src/" + screen.name + ".cpp";
    file.content = generateScreenSource(screen, ThemeIR{});
    file.language = "cpp";
    result.files.push_back(std::move(file));

    return result;
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد كود شاشة واحدة
// ═══════════════════════════════════════════════════════════════════════════

std::string DesktopSDL2CodeGen::generateScreenSource(
    const ScreenIR& screen,
    const ThemeIR& theme
) {
    std::ostringstream ss;

    // الترويسة
    ss << "/**\n";
    ss << " * \xd8\xb4\xd8\xa7\xd8\xb4\xd8\xa9: " << screen.name << "\n";
    ss << " * \xd8\xaa\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd9\x88\xd9\x84\xd9\x8a\xd8\xaf \xd8\xaa\xd9\x84\xd9\x82\xd8\xa7\xd8\xa6\xd9\x8a\xd8\xa7\xd9\x8b \xd8\xa8\xd9\x88\xd8\xa7\xd8\xb3\xd8\xb7\xd8\xa9 SadUI IR \xe2\x86\x92 Desktop CodeGen\n";
    ss << " */\n\n";

    // التضمينات
    ss << "#include \"sad_ui_framework.h\"\n";
    ss << "#include \"sad_state.h\"\n";
    ss << "#include \"sad_navigator.h\"\n\n";

    // استخدام فضاء الأسماء
    ss << "using namespace sad::ui;\n\n";

    // فضاء الأسماء
    ss << "namespace app {\n\n";

    // إعلانات الحالة
    if (!screen.stateBindings.empty()) {
        ss << "// === \xd8\xa7\xd9\x84\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9 ===\n";
        emitStateDeclarations(ss, screen.stateBindings, 0);
        ss << "\n";
    }

    // دالة بناء الشاشة
    ss << "/**\n";
    ss << " * @brief \xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1 \xd8\xb4\xd8\xa7\xd8\xb4\xd8\xa9 " << screen.name << "\n";
    ss << " */\n";
    ss << "int build_" << screen.name << "(const std::unordered_map<std::string, std::string>& args) {\n";

    // بناء الشجرة
    if (screen.rootNode) {
        nextVarId_ = 0; // reset counter per screen
        std::string rootVar = emitWidget(ss, screen.rootNode, theme, 1);
        ss << pad(1) << "return " << rootVar << ";\n";
    } else {
        ss << pad(1) << "return -1;\n";
    }

    ss << "}\n\n";

    ss << "} // namespace app\n";
    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد عقدة widget (تكراري)
// ═══════════════════════════════════════════════════════════════════════════

std::string DesktopSDL2CodeGen::emitWidget(
    std::ostringstream& ss,
    const UINodeIRPtr& node,
    const ThemeIR& theme,
    int indent
) {
    if (!node) return "";

    // Unique variable name using counter
    std::string varName = "w" + std::to_string(nextVarId_++);
    std::string p = pad(indent);

    // إنشاء Widget — استدعاء دالة المصنع المناسبة
    ss << p << "int " << varName << " = " << widgetFactoryCall(node) << ";\n";

    // الأنماط — استدعاء دوال الضبط
    const auto& style = node->style;

    if (style.backgroundColor.has_value()) {
        const auto& c = *style.backgroundColor;
        ss << p << "widget_set_bg(" << varName << ", "
           << (int)c.r << ", " << (int)c.g << ", " << (int)c.b << ");\n";
    }
    if (style.foregroundColor.has_value()) {
        const auto& c = *style.foregroundColor;
        ss << p << "text_set_color(" << varName << ", "
           << (int)c.r << ", " << (int)c.g << ", " << (int)c.b << ");\n";
    }
    if (style.fontSize.has_value()) {
        ss << p << "text_set_size(" << varName << ", " << *style.fontSize << ");\n";
    }
    if (!style.width.isAuto()) {
        ss << p << "widget_set_width(" << varName << ", " << style.width.value << ");\n";
    }
    if (!style.height.isAuto()) {
        ss << p << "widget_set_height(" << varName << ", " << style.height.value << ");\n";
    }
    if (!style.padding.isZero()) {
        // Use 4-value padding if values differ, otherwise single value
        if (style.padding.top == style.padding.leading &&
            style.padding.top == style.padding.bottom &&
            style.padding.top == style.padding.trailing) {
            ss << p << "widget_set_padding(" << varName << ", " << style.padding.top << ");\n";
        } else {
            ss << p << "widget_set_padding4(" << varName << ", "
               << style.padding.top << ", " << style.padding.trailing << ", "
               << style.padding.bottom << ", " << style.padding.leading << ");\n";
        }
    }
    if (style.borderRadius > 0) {
        ss << p << "widget_set_border_radius(" << varName << ", " << style.borderRadius << ");\n";
    }
    if (style.borderWidth > 0) {
        if (style.borderColor.has_value()) {
            const auto& c = *style.borderColor;
            ss << p << "widget_set_border(" << varName << ", " << style.borderWidth
               << ", " << (int)c.r << ", " << (int)c.g << ", " << (int)c.b << ");\n";
        }
    }
    if (style.hidden) {
        ss << p << "widget_set_visible(" << varName << ", false);\n";
    }
    if (style.spacing > 0) {
        ss << p << "widget_set_spacing(" << varName << ", " << style.spacing << ");\n";
    }

    // معالجات الأحداث — كتعليقات توثيقية (الإطار الحقيقي يستخدم الاستطلاع)
    if (!node->eventHandlers.empty()) {
        ss << p << "// TODO: Event handlers (use button_was_clicked/collectEvents)\n";
        for (const auto& handler : node->eventHandlers) {
            if (!handler.handlerName.empty()) {
                ss << p << "// " << handler.handlerName << "()\n";
            }
        }
    }

    // إجراء التنقل
    if (node->navAction.has_value()) {
        ss << p << "// Navigation: " << node->navAction->targetRoute << "\n";
        ss << p << "g_widgets[" << varName << "].widgetName = \""
           << node->navAction->targetRoute << "\";\n";
    }

    // الأبناء (تكراري)
    for (const auto& child : node->children) {
        if (!child) continue;
        std::string childVar = emitWidget(ss, child, theme, indent + 1);
        if (!childVar.empty()) {
            ss << p << "widget_add_child(" << varName << ", " << childVar << ");\n\n";
        }
    }

    return varName;
}


// ═══════════════════════════════════════════════════════════════════════════
// إعلانات الحالة
// ═══════════════════════════════════════════════════════════════════════════

void DesktopSDL2CodeGen::emitStateDeclarations(
    std::ostringstream& ss,
    const std::vector<StateBindingIR>& bindings,
    int indent
) {
    std::string p = pad(indent);
    for (const auto& binding : bindings) {
        std::string cppType;
        switch (binding.valueType) {
            case StateValueType::Number:   cppType = "double"; break;
            case StateValueType::Text:     cppType = "std::string"; break;
            case StateValueType::Boolean:  cppType = "bool"; break;
            default:                       cppType = "sad::Value"; break;
        }

        switch (binding.kind) {
            case StateBindingKind::Signal:
                ss << p << "sad::state::Signal<" << cppType << "> " << binding.name;
                if (!binding.initialValue.empty()) {
                    ss << "{" << binding.initialValue << "}";
                }
                ss << ";\n";
                break;
            case StateBindingKind::Computed:
                ss << p << "// \xd9\x85\xd8\xad\xd8\xb3\xd9\x88\xd8\xa8: " << binding.name << "\n";
                ss << p << "sad::state::Computed<" << cppType << "> " << binding.name
                   << "([&]() { " << binding.computeExpression.value_or("return {};") << " });\n";
                break;
            case StateBindingKind::Observable:
                ss << p << "sad::state::Observable<" << cppType << "> " << binding.name;
                if (!binding.initialValue.empty()) {
                    ss << "{" << binding.initialValue << "}";
                }
                ss << ";\n";
                break;
            case StateBindingKind::AppStorage:
                ss << p << "// \xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86 \xd8\xaf\xd8\xa7\xd8\xa6\xd9\x85: " << binding.name << "\n";
                ss << p << "auto " << binding.name << " = sad::storage::load<" << cppType
                   << ">(\"" << binding.name << "\");\n";
                break;
            default:
                ss << p << cppType << " " << binding.name;
                if (!binding.initialValue.empty()) {
                    ss << " = " << binding.initialValue;
                }
                ss << ";\n";
                break;
        }
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// معالجات الأحداث
// ═══════════════════════════════════════════════════════════════════════════

void DesktopSDL2CodeGen::emitEventHandlers(
    std::ostringstream& ss,
    const std::vector<EventHandlerIR>& handlers,
    const std::string& widgetVar,
    int indent
) {
    std::string p = pad(indent);
    for (const auto& handler : handlers) {
        std::string eventName;
        switch (handler.type) {
            case EventType::Tap:            eventName = "onClick"; break;
            case EventType::DoubleTap:      eventName = "onDoubleTap"; break;
            case EventType::LongPress:      eventName = "onLongPress"; break;
            case EventType::TextChange:     eventName = "onTextChange"; break;
            case EventType::ValueChange:    eventName = "onValueChange"; break;
            case EventType::ToggleChange:   eventName = "onToggle"; break;
            case EventType::Submit:         eventName = "onSubmit"; break;
            case EventType::Appear:         eventName = "onAppear"; break;
            case EventType::Disappear:      eventName = "onDisappear"; break;
            case EventType::Scroll:         eventName = "onScroll"; break;
            default:                        eventName = "onClick"; break;
        }

        ss << p << widgetVar << "->" << eventName << " = [&]() {\n";
        if (!handler.closureBody.empty()) {
            ss << p << "    " << handler.closureBody << "\n";
        } else if (!handler.handlerName.empty()) {
            ss << p << "    " << handler.handlerName << "();\n";
        }
        ss << p << "};\n";
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// main.cpp
// ═══════════════════════════════════════════════════════════════════════════

std::string DesktopSDL2CodeGen::generateMainCpp(const AppIR& app) {
    std::ostringstream ss;
    ss << "/**\n";
    ss << " * " << app.appName << " \xe2\x80\x94 \xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82 \xd8\xb3\xd8\xb7\xd8\xad \xd8\xa7\xd9\x84\xd9\x85\xd9\x83\xd8\xaa\xd8\xa8\n";
    ss << " * \xd8\xaa\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd9\x88\xd9\x84\xd9\x8a\xd8\xaf \xd8\xaa\xd9\x84\xd9\x82\xd8\xa7\xd8\xa6\xd9\x8a\xd8\xa7\xd9\x8b \xd8\xa8\xd9\x88\xd8\xa7\xd8\xb3\xd8\xb7\xd8\xa9 sadc --\xd9\x85\xd9\x86\xd8\xb5\xd8\xa9 \xd8\xb3\xd8\xb7\xd8\xad_\xd8\xa7\xd9\x84\xd9\x85\xd9\x83\xd8\xaa\xd8\xa8\n";
    ss << " */\n\n";

    ss << "#include \"sad_ui_framework.h\"\n";
    ss << "#include \"sad_ui_internal.h\"\n";
    ss << "#include \"sad_navigator.h\"\n";
    ss << "#include \"sad_state.h\"\n";
    ss << "#include <iostream>\n\n";

    ss << "using namespace sad::ui;\n\n";

    // تصريح دوال الشاشات
    ss << "namespace app {\n";
    for (const auto& screen : app.screens) {
        ss << "    int build_" << screen.name << "(const std::unordered_map<std::string, std::string>& args);\n";
    }
    ss << "}\n\n";

    // الدالة الرئيسية
    ss << "int main(int argc, char* argv[]) {\n";

    int w = config_.windowWidth, h = config_.windowHeight;
    if (app.desktopConfig.has_value()) {
        w = app.desktopConfig->windowWidth;
        h = app.desktopConfig->windowHeight;
    }

    ss << "    // \xd8\xa5\xd9\x86\xd8\xb4\xd8\xa7\xd8\xa1 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82\n";
    ss << "    int appId = app_create(\"" << app.appName << "\", " << w << ", " << h << ");\n";
    ss << "    if (appId <= 0) return 1;\n\n";

    // لون الخلفية
    ss << "    app_set_bg_color(appId, "
       << (int)app.theme.backgroundColor.r << ", "
       << (int)app.theme.backgroundColor.g << ", "
       << (int)app.theme.backgroundColor.b << ");\n\n";

    // تسجيل الشاشات
    ss << "    // \xd8\xaa\xd8\xb3\xd8\xac\xd9\x8a\xd9\x84 \xd8\xa7\xd9\x84\xd8\xb4\xd8\xa7\xd8\xb4\xd8\xa7\xd8\xaa\n";
    for (const auto& screen : app.screens) {
        ss << "    sad::nav::registerRoute(\"" << screen.routeName
           << "\", app::build_" << screen.name << ");\n";
    }
    ss << "\n";

    // الانتقال للشاشة الأولى
    if (!app.initialScreenName.empty()) {
        ss << "    sad::nav::push(\"" << app.initialScreenName << "\");\n";
    }
    ss << "    int currentWidget = sad::nav::currentPageWidget();\n";
    ss << "    if (currentWidget > 0) app_set_content(appId, currentWidget);\n\n";

    // حلقة التشغيل
    ss << "    // \xd8\xad\xd9\x84\xd9\x82\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xb4\xd8\xba\xd9\x8a\xd9\x84\n";
    ss << "    while (app_is_running(appId)) {\n";
    ss << "        app_update(appId);\n\n";

    ss << "        // \xd8\xa7\xd9\x84\xd8\xaa\xd8\xad\xd9\x82\xd9\x82 \xd9\x85\xd9\x86 \xd8\xa3\xd8\xb2\xd8\xb1\xd8\xa7\xd8\xb1 \xd8\xa7\xd9\x84\xd8\xaa\xd9\x86\xd9\x82\xd9\x84\n";
    ss << "        for (auto& [id, w] : g_widgets) {\n";
    ss << "            if (w.widgetName.empty()) continue;\n";
    ss << "            if (button_was_clicked(id)) {\n";
    ss << "                if (w.widgetName == \"back\") {\n";
    ss << "                    if (sad::nav::canPop()) {\n";
    ss << "                        sad::nav::pop();\n";
    ss << "                        int nw = sad::nav::currentPageWidget();\n";
    ss << "                        if (nw > 0) app_set_content(appId, nw);\n";
    ss << "                    }\n";
    ss << "                } else {\n";
    ss << "                    sad::nav::push(w.widgetName);\n";
    ss << "                    int nw = sad::nav::currentPageWidget();\n";
    ss << "                    if (nw > 0) app_set_content(appId, nw);\n";
    ss << "                }\n";
    ss << "            }\n";
    ss << "        }\n";
    ss << "    }\n\n";

    ss << "    app_close(appId);\n";
    ss << "    return 0;\n";
    ss << "}\n";

    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// CMakeLists.txt
// ═══════════════════════════════════════════════════════════════════════════

std::string DesktopSDL2CodeGen::generateCMakeLists(const AppIR& app) {
    std::ostringstream ss;
    ss << "# " << app.appName << " \xe2\x80\x94 \xd8\xaa\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd9\x88\xd9\x84\xd9\x8a\xd8\xaf \xd8\xaa\xd9\x84\xd9\x82\xd8\xa7\xd8\xa6\xd9\x8a\xd8\xa7\xd9\x8b\n";
    ss << "cmake_minimum_required(VERSION 3.15)\n";
    ss << "project(" << app.appName << " LANGUAGES CXX)\n\n";
    ss << "set(CMAKE_CXX_STANDARD 17)\n\n";

    ss << "# مصادر التطبيق\n";
    ss << "file(GLOB APP_SOURCES \"src/*.cpp\")\n\n";

    ss << "add_executable(${PROJECT_NAME} ${APP_SOURCES})\n\n";

    ss << "# ربط مكتبات لغة ص\n";
    ss << "target_link_libraries(${PROJECT_NAME} PRIVATE sad_ui sad_core)\n\n";

    ss << "# SDL2\n";
    ss << "find_package(SDL2 REQUIRED)\n";
    ss << "target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)\n";

    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد استدعاء دالة المصنع حسب نوع الودجت
// ═══════════════════════════════════════════════════════════════════════════

std::string DesktopSDL2CodeGen::widgetFactoryCall(const UINodeIRPtr& node) {
    // Helper to get first string arg from constructorArgs or properties["content"]
    // يستخدم دالة التهريب الآمنة / Uses safe escape function
    auto getStr = [&](size_t idx = 0) -> std::string {
        std::string raw;
        if (idx < node->constructorArgs.size()) {
            if (auto* s = std::get_if<std::string>(&node->constructorArgs[idx])) {
                raw = *s;
            }
        }
        // Fallback: check properties for "content"
        if (raw.empty() && idx == 0) {
            auto it = node->properties.find("content");
            if (it != node->properties.end()) {
                if (auto* s = std::get_if<std::string>(&it->second)) {
                    raw = *s;
                }
            }
        }
        // تهريب النص للأمان / Escape string for security
        return escapeCppString(raw);
    };
    auto getNum = [&](size_t idx = 0) -> std::string {
        if (idx < node->constructorArgs.size()) {
            if (auto* d = std::get_if<double>(&node->constructorArgs[idx]))
                return std::to_string(*d);
            if (auto* i = std::get_if<int64_t>(&node->constructorArgs[idx]))
                return std::to_string(*i);
        }
        return "0";
    };

    switch (node->type) {
        // Layout
        case WidgetType::Column:        return "widget_column()";
        case WidgetType::Row:           return "widget_row()";
        case WidgetType::Stack:         return "widget_stack()";
        case WidgetType::Grid:          return "widget_gridview(2)";
        case WidgetType::Center:        return "widget_center()";
        case WidgetType::Wrap:          return "widget_wrap()";
        case WidgetType::Expanded:      return "widget_expanded()";
        case WidgetType::Container:     return "widget_container()";
        case WidgetType::Card:          return "widget_card()";
        case WidgetType::ScrollView:    return "widget_scrollview()";

        // Spacer/Divider
        case WidgetType::Spacer: {
            auto s = getNum(0);
            return "widget_spacer(" + s + ")";
        }
        case WidgetType::Divider:       return "widget_divider()";
        case WidgetType::SizedBox: {
            auto w = getNum(0), h = getNum(1);
            return "widget_sizedbox(" + w + ", " + h + ")";
        }

        // Text
        case WidgetType::Text:          return "widget_text(\"" + getStr() + "\")";
        case WidgetType::Image:         return "widget_image(\"" + getStr() + "\")";

        // Buttons
        case WidgetType::Button:        return "widget_button(\"" + getStr() + "\")";
        case WidgetType::IconButton:    return "widget_icon_button('*')";
        case WidgetType::FloatingButton: return "widget_floating_button('+')";
        case WidgetType::OutlineButton: return "widget_outline_button(\"" + getStr() + "\")";
        case WidgetType::TextButton:    return "widget_text_button(\"" + getStr() + "\")";

        // Input
        case WidgetType::TextField:     return "widget_textfield(\"" + getStr() + "\")";
        case WidgetType::TextArea:      return "widget_textarea(\"" + getStr() + "\")";
        case WidgetType::PasswordField: return "widget_password_field(\"" + getStr() + "\")";
        case WidgetType::SearchField:   return "widget_search_field(\"" + getStr() + "\")";
        case WidgetType::Checkbox:      return "widget_checkbox(\"" + getStr() + "\")";
        case WidgetType::Radio:         return "widget_radio(\"" + getStr() + "\")";
        case WidgetType::Toggle:        return "widget_switch(\"" + getStr() + "\")";
        case WidgetType::Slider:        return "widget_slider(0, 100)";
        case WidgetType::RangeSlider:   return "widget_range_slider(0, 100)";
        case WidgetType::Dropdown:      return "widget_dropdown(\"" + getStr() + "\")";
        case WidgetType::NumberInput:   return "widget_number_input(0, 100)";
        case WidgetType::ColorPicker:   return "widget_color_picker()";
        case WidgetType::RatingStars:   return "widget_rating_stars(5)";

        // Display
        case WidgetType::Icon:          return "widget_icon('*')";

        // Navigation
        case WidgetType::AppBar:        return "widget_app_bar(\"" + getStr() + "\")";
        case WidgetType::BottomBar:     return "widget_bottom_bar()";
        case WidgetType::TabBar:        return "widget_tab_bar()";
        case WidgetType::Drawer:        return "widget_drawer()";

        // Feedback
        case WidgetType::Dialog:        return "widget_dialog(\"" + getStr() + "\", \"\")";
        case WidgetType::Modal:         return "widget_modal()";

        // Data
        case WidgetType::ListTile:      return "widget_list_tile(\"" + getStr() + "\", \"\")";

        // Progress
        case WidgetType::ProgressBar:   return "widget_progress_bar(0)";
        case WidgetType::LoadingSpinner: return "widget_loading_spinner()";

        // Charts
        case WidgetType::BarChart:      return "widget_bar_chart()";
        case WidgetType::PieChart:      return "widget_pie_chart()";
        case WidgetType::LineChart:     return "widget_line_chart()";

        // Default fallback
        default:
            return "widget_container() /* " + widgetTypeToArabic(node->type) + " */";
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// مساعدات
// ═══════════════════════════════════════════════════════════════════════════

std::string DesktopSDL2CodeGen::colorToRGBA(const Color& c) {
    std::ostringstream ss;
    ss << "RGBA{" << (int)c.r << ", " << (int)c.g << ", " << (int)c.b
       << ", " << (int)(c.a * 255) << "}";
    return ss.str();
}

std::string DesktopSDL2CodeGen::pad(int indent) {
    return std::string(indent * 4, ' ');
}


} // namespace ir
} // namespace ui
} // namespace sad
