/**
 * ==========================================================================
 * ملف: web_html_codegen.cpp
 * الوصف: تنفيذ مولد كود الويب — SadUI IR → HTML/CSS/JavaScript
 * ==========================================================================
 * بسم الله الرحمن الرحيم
 */

#include "ui/web_html_codegen.hpp"
#include <algorithm>
#include <iomanip>

namespace sad {
namespace ui {
namespace ir {


WebHTMLCodeGen::WebHTMLCodeGen(const WebCodeGenConfig& config)
    : config_(config)
{}


// ═══════════════════════════════════════════════════════════════════════════
// توليد المشروع الكامل
// ═══════════════════════════════════════════════════════════════════════════

CodeGenResult WebHTMLCodeGen::generate(const AppIR& app) {
    CodeGenResult result;
    result.success = true;
    idCounter_ = 0;

    std::string appName = app.appName.empty() ? "SadApp" : app.appName;
    std::string distDir = appName + "/";

    // 1. index.html
    result.files.push_back({
        distDir + "index.html",
        generateIndexHTML(app),
        false, "html"
    });

    // 2. Global CSS
    result.files.push_back({
        distDir + "css/style.css",
        generateGlobalCSS(app),
        false, "css"
    });

    // 3. Screen HTML/CSS/JS
    for (const auto& screen : app.screens) {
        idCounter_ = 0; // Reset per screen

        // Screen CSS
        result.files.push_back({
            distDir + "css/" + screen.name + ".css",
            generateScreenCSS(screen, app.theme),
            false, "css"
        });

        // Screen JS
        result.files.push_back({
            distDir + "js/" + screen.name + ".js",
            generateScreenJS(screen),
            false, "javascript"
        });
    }

    // 4. Router JS (for SPA)
    if (config_.singlePageApp && app.screens.size() > 1) {
        result.files.push_back({
            distDir + "js/router.js",
            generateRouterJS(app),
            false, "javascript"
        });
    }

    result.buildCommand = "cd " + appName + " && python -m http.server 8080";
    return result;
}


CodeGenResult WebHTMLCodeGen::generateScreen(const ScreenIR& screen) {
    CodeGenResult result;
    result.success = true;
    ThemeIR defaultTheme;
    idCounter_ = 0;
    result.files.push_back({
        screen.name + ".html",
        generateScreenHTML(screen, defaultTheme),
        false, "html"
    });
    return result;
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد index.html
// ═══════════════════════════════════════════════════════════════════════════

std::string WebHTMLCodeGen::generateIndexHTML(const AppIR& app) {
    std::ostringstream ss;
    std::string appName = app.appName.empty() ? "SadApp" : app.appName;

    ss << "<!DOCTYPE html>\n";
    ss << "<html lang=\"" << (app.theme.isRTL ? "ar" : "en")
       << "\" dir=\"" << (app.theme.isRTL ? "rtl" : "ltr") << "\">\n";
    ss << "<head>\n";
    ss << "    <meta charset=\"UTF-8\">\n";
    ss << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    ss << "    <title>" << appName << "</title>\n";
    ss << "    <link rel=\"stylesheet\" href=\"css/style.css\">\n";

    // Per-screen CSS
    for (const auto& screen : app.screens) {
        ss << "    <link rel=\"stylesheet\" href=\"css/" << screen.name << ".css\">\n";
    }

    ss << "</head>\n";
    ss << "<body>\n";
    ss << "    <div id=\"app\">\n";

    // Render screens
    for (size_t i = 0; i < app.screens.size(); i++) {
        const auto& screen = app.screens[i];
        std::string display = (i == 0) ? "block" : "none";

        // Find initial screen
        if (!app.initialScreenName.empty()) {
            display = (screen.name == app.initialScreenName) ? "block" : "none";
        }

        ss << "        <div id=\"screen-" << screen.name
           << "\" class=\"screen\" style=\"display: " << display << "\">\n";

        if (screen.rootNode) {
            idCounter_ = 0;
            emitHTML(ss, screen.rootNode, 3);
        }

        ss << "        </div>\n";
    }

    ss << "    </div>\n\n";

    // Scripts
    if (config_.singlePageApp && app.screens.size() > 1) {
        ss << "    <script src=\"js/router.js\"></script>\n";
    }
    for (const auto& screen : app.screens) {
        ss << "    <script src=\"js/" << screen.name << ".js\"></script>\n";
    }

    ss << "</body>\n";
    ss << "</html>\n";

    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد CSS عام
// ═══════════════════════════════════════════════════════════════════════════

std::string WebHTMLCodeGen::generateGlobalCSS(const AppIR& app) {
    std::ostringstream ss;
    const auto& theme = app.theme;

    ss << "/* SadUI - Global Styles */\n";
    ss << "/* \xd8\xaa\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd9\x88\xd9\x84\xd9\x8a\xd8\xaf \xd8\xaa\xd9\x84\xd9\x82\xd8\xa7\xd8\xa6\xd9\x8a\xd8\xa7\xd9\x8b */\n\n"; // تم التوليد تلقائياً

    // CSS Variables
    if (config_.useCSSVariables) {
        ss << ":root {\n";
        ss << "    --sad-primary: " << cssColor(theme.primaryColor) << ";\n";
        ss << "    --sad-secondary: " << cssColor(theme.secondaryColor) << ";\n";
        ss << "    --sad-background: " << cssColor(theme.backgroundColor) << ";\n";
        ss << "    --sad-surface: " << cssColor(theme.surfaceColor) << ";\n";
        ss << "    --sad-error: " << cssColor(theme.errorColor) << ";\n";
        ss << "    --sad-text: " << cssColor(theme.textPrimary) << ";\n";
        ss << "    --sad-font-body: " << theme.fontSizeBody << "px;\n";
        ss << "    --sad-font-title: " << theme.fontSizeTitle << "px;\n";
        ss << "    --sad-font-caption: " << theme.fontSizeCaption << "px;\n";
        ss << "    --sad-radius: " << theme.borderRadiusMD << "px;\n";
        ss << "    --sad-font-family: system-ui, -apple-system, sans-serif;\n";
        ss << "}\n\n";
    }

    // Reset
    ss << "*, *::before, *::after {\n";
    ss << "    margin: 0;\n";
    ss << "    padding: 0;\n";
    ss << "    box-sizing: border-box;\n";
    ss << "}\n\n";

    ss << "html, body {\n";
    ss << "    height: 100%;\n";
    ss << "    font-family: var(--sad-font-family);\n";
    ss << "    font-size: var(--sad-font-body);\n";
    ss << "    color: var(--sad-text);\n";
    ss << "    background-color: var(--sad-background);\n";
    if (app.theme.isRTL) {
        ss << "    direction: rtl;\n";
    }
    ss << "}\n\n";

    ss << "#app {\n";
    ss << "    min-height: 100%;\n";
    ss << "}\n\n";

    ss << ".screen {\n";
    ss << "    min-height: 100vh;\n";
    ss << "}\n\n";

    // Utility classes
    ss << "/* Layout utilities */\n";
    ss << ".sad-column { display: flex; flex-direction: column; }\n";
    ss << ".sad-row { display: flex; flex-direction: row; }\n";
    ss << ".sad-stack { position: relative; }\n";
    ss << ".sad-scroll { overflow-y: auto; }\n";
    ss << ".sad-spacer { flex: 1; }\n";
    ss << ".sad-divider { border-top: 1px solid #e0e0e0; width: 100%; margin: 8px 0; }\n\n";

    ss << ".sad-scaffold {\n";
    ss << "    display: flex;\n";
    ss << "    flex-direction: column;\n";
    ss << "    min-height: 100vh;\n";
    ss << "}\n\n";

    ss << ".sad-appbar {\n";
    ss << "    display: flex;\n";
    ss << "    align-items: center;\n";
    ss << "    padding: 12px 16px;\n";
    ss << "    background-color: var(--sad-primary);\n";
    ss << "    color: white;\n";
    ss << "    font-size: var(--sad-font-title);\n";
    ss << "    font-weight: bold;\n";
    ss << "}\n\n";

    ss << ".sad-card {\n";
    ss << "    background: var(--sad-surface);\n";
    ss << "    border-radius: var(--sad-radius);\n";
    ss << "    padding: 16px;\n";
    ss << "    box-shadow: 0 2px 4px rgba(0,0,0,0.1);\n";
    ss << "}\n\n";

    ss << ".sad-button {\n";
    ss << "    display: inline-flex;\n";
    ss << "    align-items: center;\n";
    ss << "    justify-content: center;\n";
    ss << "    padding: 8px 16px;\n";
    ss << "    border: none;\n";
    ss << "    border-radius: var(--sad-radius);\n";
    ss << "    background-color: var(--sad-primary);\n";
    ss << "    color: white;\n";
    ss << "    cursor: pointer;\n";
    ss << "    font-size: inherit;\n";
    ss << "    transition: opacity 0.2s;\n";
    ss << "}\n";
    ss << ".sad-button:hover { opacity: 0.85; }\n\n";

    ss << ".sad-textfield {\n";
    ss << "    padding: 8px 12px;\n";
    ss << "    border: 1px solid #ccc;\n";
    ss << "    border-radius: var(--sad-radius);\n";
    ss << "    font-size: inherit;\n";
    ss << "    width: 100%;\n";
    ss << "}\n";
    ss << ".sad-textfield:focus { outline: none; border-color: var(--sad-primary); }\n\n";

    // Responsive
    if (config_.responsive) {
        ss << "/* Responsive */\n";
        ss << "@media (max-width: 768px) {\n";
        ss << "    .sad-row { flex-direction: column; }\n";
        ss << "}\n";
    }

    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد CSS لشاشة
// ═══════════════════════════════════════════════════════════════════════════

std::string WebHTMLCodeGen::generateScreenCSS(
    const ScreenIR& screen,
    const ThemeIR& theme
) {
    std::ostringstream ss;
    ss << "/* " << screen.name << " styles */\n\n";

    if (screen.rootNode) {
        idCounter_ = 0;
        emitCSS(ss, screen.rootNode);
    }

    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد JS لشاشة
// ═══════════════════════════════════════════════════════════════════════════

std::string WebHTMLCodeGen::generateScreenJS(const ScreenIR& screen) {
    std::ostringstream ss;
    ss << "// " << screen.name << " - JavaScript\n";
    ss << "// \xd8\xaa\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd9\x88\xd9\x84\xd9\x8a\xd8\xaf \xd8\xa8\xd9\x88\xd8\xa7\xd8\xb3\xd8\xb7\xd8\xa9 SadUI\n\n"; // تم التوليد بواسطة SadUI

    // State management
    if (!screen.stateBindings.empty()) {
        ss << "// State\n";
        ss << "const " << screen.name << "State = {\n";
        for (const auto& sb : screen.stateBindings) {
            std::string defaultVal;
            switch (sb.valueType) {
                case StateValueType::Text:
                    defaultVal = sb.initialValue.empty() ? "''" : "'" + sb.initialValue + "'";
                    break;
                case StateValueType::Number:
                    defaultVal = sb.initialValue.empty() ? "0" : sb.initialValue;
                    break;
                case StateValueType::Boolean:
                    defaultVal = sb.initialValue.empty() ? "false" : sb.initialValue;
                    break;
                case StateValueType::ObjectList:
                    defaultVal = "[]";
                    break;
                default:
                    defaultVal = "null";
                    break;
            }
            ss << "    " << sb.name << ": " << defaultVal << ",\n";
        }
        ss << "};\n\n";

        // Reactive state proxy
        ss << "const " << screen.name << " = new Proxy(" << screen.name << "State, {\n";
        ss << "    set(target, key, value) {\n";
        ss << "        target[key] = value;\n";
        ss << "        update" << screen.name << "();\n";
        ss << "        return true;\n";
        ss << "    }\n";
        ss << "});\n\n";

        ss << "function update" << screen.name << "() {\n";
        ss << "    // Re-render bound elements\n";
        ss << "    document.querySelectorAll('[data-bind]').forEach(el => {\n";
        ss << "        const key = el.getAttribute('data-bind');\n";
        ss << "        if (key in " << screen.name << ") {\n";
        ss << "            if (el.tagName === 'INPUT') {\n";
        ss << "                el.value = " << screen.name << "[key];\n";
        ss << "            } else {\n";
        ss << "                el.textContent = " << screen.name << "[key];\n";
        ss << "            }\n";
        ss << "        }\n";
        ss << "    });\n";
        ss << "}\n\n";
    }

    // Events
    if (screen.rootNode) {
        idCounter_ = 0;
        emitJS(ss, screen.rootNode);
    }

    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد Router JS
// ═══════════════════════════════════════════════════════════════════════════

std::string WebHTMLCodeGen::generateRouterJS(const AppIR& app) {
    std::ostringstream ss;

    ss << "// SadUI Router\n\n";

    ss << "const SadRouter = {\n";
    ss << "    currentScreen: '" << (app.initialScreenName.empty() && !app.screens.empty()
                                      ? app.screens[0].name : app.initialScreenName) << "',\n\n";

    ss << "    navigate(screenName) {\n";
    ss << "        document.querySelectorAll('.screen').forEach(el => {\n";
    ss << "            el.style.display = 'none';\n";
    ss << "        });\n";
    ss << "        const target = document.getElementById('screen-' + screenName);\n";
    ss << "        if (target) {\n";
    ss << "            target.style.display = 'block';\n";
    ss << "            this.currentScreen = screenName;\n";
    ss << "            window.history.pushState({screen: screenName}, '', '#' + screenName);\n";
    ss << "        }\n";
    ss << "    },\n\n";

    ss << "    back() {\n";
    ss << "        window.history.back();\n";
    ss << "    },\n\n";

    ss << "    init() {\n";
    ss << "        window.addEventListener('popstate', (e) => {\n";
    ss << "            if (e.state && e.state.screen) {\n";
    ss << "                this.navigate(e.state.screen);\n";
    ss << "            }\n";
    ss << "        });\n";
    ss << "        // Handle hash on load\n";
    ss << "        const hash = window.location.hash.slice(1);\n";
    ss << "        if (hash) this.navigate(hash);\n";
    ss << "    }\n";
    ss << "};\n\n";

    ss << "document.addEventListener('DOMContentLoaded', () => SadRouter.init());\n";

    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد HTML لشاشة مستقلة
// ═══════════════════════════════════════════════════════════════════════════

std::string WebHTMLCodeGen::generateScreenHTML(
    const ScreenIR& screen,
    const ThemeIR& theme
) {
    std::ostringstream ss;
    ss << "<!DOCTYPE html>\n<html>\n<head>\n";
    ss << "    <meta charset=\"UTF-8\">\n";
    ss << "    <title>" << screen.name << "</title>\n";
    ss << "</head>\n<body>\n";
    if (screen.rootNode) {
        idCounter_ = 0;
        emitHTML(ss, screen.rootNode, 1);
    }
    ss << "</body>\n</html>\n";
    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد عنصر HTML (تكراري)
// ═══════════════════════════════════════════════════════════════════════════

void WebHTMLCodeGen::emitHTML(
    std::ostringstream& ss,
    const UINodeIRPtr& node,
    int indent
) {
    if (!node) return;

    int myId = idCounter_++;
    std::string id = !node->tag.has_value()
        ? ("n" + std::to_string(myId))
        : node->tag.value();

    switch (node->type) {
        // ─── Text ──────────────────────────────
        case WidgetType::Text: {
            std::string content;
            auto it = node->properties.find("content");
            if (it != node->properties.end()) {
                if (auto* s = std::get_if<std::string>(&it->second)) {
                    content = *s;
                }
            }
            ss << pad(indent) << "<span id=\"" << id << "\" class=\"sad-text\"";
            std::string css = styleToCSS(node->style);
            if (!css.empty()) ss << " style=\"" << css << "\"";
            ss << ">" << content << "</span>\n";
            break;
        }

        // ─── Button ────────────────────────────
        case WidgetType::Button: {
            std::string content;
            auto it = node->properties.find("content");
            if (it != node->properties.end()) {
                if (auto* s = std::get_if<std::string>(&it->second)) {
                    content = *s;
                }
            }
            ss << pad(indent) << "<button id=\"" << id << "\" class=\"sad-button\"";
            std::string css = styleToCSS(node->style);
            if (!css.empty()) ss << " style=\"" << css << "\"";

            // Events
            for (const auto& ev : node->eventHandlers) {
                if (ev.type == EventType::Tap) {
                    ss << " onclick=\"" << ev.handlerName << "()\"";
                }
            }
            ss << ">";

            if (!node->children.empty()) {
                ss << "\n";
                for (const auto& child : node->children) {
                    emitHTML(ss, child, indent + 1);
                }
                ss << pad(indent);
            } else {
                ss << content;
            }

            ss << "</button>\n";
            break;
        }

        // ─── TextField ─────────────────────────
        case WidgetType::TextField: {
            std::string placeholder;
            auto it = node->properties.find("placeholder");
            if (it != node->properties.end()) {
                if (auto* s = std::get_if<std::string>(&it->second)) {
                    placeholder = *s;
                }
            }
            std::string dataBind;
            if (node->stateBinding) {
                const auto& sb = *node->stateBinding;
                dataBind = sb.name;
                break;
            }
            ss << pad(indent) << "<input id=\"" << id << "\" class=\"sad-textfield\" type=\"text\"";
            if (!placeholder.empty()) ss << " placeholder=\"" << placeholder << "\"";
            if (!dataBind.empty()) ss << " data-bind=\"" << dataBind << "\"";
            std::string css = styleToCSS(node->style);
            if (!css.empty()) ss << " style=\"" << css << "\"";
            ss << ">\n";
            break;
        }

        // ─── Toggle ────────────────────────────
        case WidgetType::Toggle: {
            std::string dataBind;
            if (node->stateBinding) {
                dataBind = node->stateBinding->name;
            }
            ss << pad(indent) << "<label class=\"sad-toggle\">\n";
            ss << pad(indent+1) << "<input id=\"" << id << "\" type=\"checkbox\"";
            if (!dataBind.empty()) ss << " data-bind=\"" << dataBind << "\"";
            ss << ">\n";
            // Label text
            auto it = node->properties.find("content");
            if (it != node->properties.end()) {
                if (auto* s = std::get_if<std::string>(&it->second)) {
                    ss << pad(indent+1) << "<span>" << *s << "</span>\n";
                }
            }
            ss << pad(indent) << "</label>\n";
            break;
        }

        // ─── Slider ────────────────────────────
        case WidgetType::Slider: {
            ss << pad(indent) << "<input id=\"" << id << "\" type=\"range\" class=\"sad-slider\"";
            std::string css = styleToCSS(node->style);
            if (!css.empty()) ss << " style=\"" << css << "\"";
            ss << ">\n";
            break;
        }

        // ─── Image ─────────────────────────────
        case WidgetType::Image: {
            std::string source = "";
            auto it = node->properties.find("source");
            if (it != node->properties.end()) {
                if (auto* s = std::get_if<std::string>(&it->second)) {
                    source = *s;
                }
            }
            ss << pad(indent) << "<img id=\"" << id << "\" src=\"" << source << "\" alt=\"\"";
            std::string css = styleToCSS(node->style);
            if (!css.empty()) ss << " style=\"" << css << "\"";
            ss << ">\n";
            break;
        }

        // ─── Column ────────────────────────────
        case WidgetType::Column:
        case WidgetType::LazyColumn: {
            ss << pad(indent) << "<div id=\"" << id << "\" class=\"sad-column\"";
            std::string css = styleToCSS(node->style);
            if (!css.empty()) ss << " style=\"" << css << "\"";
            ss << ">\n";
            for (const auto& child : node->children) {
                emitHTML(ss, child, indent + 1);
            }
            ss << pad(indent) << "</div>\n";
            break;
        }

        // ─── Row ───────────────────────────────
        case WidgetType::Row:
        case WidgetType::LazyRow: {
            ss << pad(indent) << "<div id=\"" << id << "\" class=\"sad-row\"";
            std::string css = styleToCSS(node->style);
            if (!css.empty()) ss << " style=\"" << css << "\"";
            ss << ">\n";
            for (const auto& child : node->children) {
                emitHTML(ss, child, indent + 1);
            }
            ss << pad(indent) << "</div>\n";
            break;
        }

        // ─── Stack ─────────────────────────────
        case WidgetType::Stack: {
            ss << pad(indent) << "<div id=\"" << id << "\" class=\"sad-stack\"";
            std::string css = styleToCSS(node->style);
            if (!css.empty()) ss << " style=\"" << css << "\"";
            ss << ">\n";
            for (const auto& child : node->children) {
                emitHTML(ss, child, indent + 1);
            }
            ss << pad(indent) << "</div>\n";
            break;
        }

        // ─── ScrollView ────────────────────────
        case WidgetType::ScrollView: {
            ss << pad(indent) << "<div id=\"" << id << "\" class=\"sad-scroll\"";
            std::string css = styleToCSS(node->style);
            if (!css.empty()) ss << " style=\"" << css << "\"";
            ss << ">\n";
            for (const auto& child : node->children) {
                emitHTML(ss, child, indent + 1);
            }
            ss << pad(indent) << "</div>\n";
            break;
        }

        // ─── Scaffold ──────────────────────────
        case WidgetType::Scaffold: {
            ss << pad(indent) << "<div id=\"" << id << "\" class=\"sad-scaffold\">\n";
            for (const auto& child : node->children) {
                emitHTML(ss, child, indent + 1);
            }
            ss << pad(indent) << "</div>\n";
            break;
        }

        // ─── AppBar ────────────────────────────
        case WidgetType::AppBar: {
            std::string title;
            auto it = node->properties.find("content");
            if (it != node->properties.end()) {
                if (auto* s = std::get_if<std::string>(&it->second)) {
                    title = *s;
                }
            }
            ss << pad(indent) << "<header id=\"" << id << "\" class=\"sad-appbar\">\n";
            ss << pad(indent+1) << "<span>" << title << "</span>\n";
            ss << pad(indent) << "</header>\n";
            break;
        }

        // ─── Card ──────────────────────────────
        case WidgetType::Card: {
            ss << pad(indent) << "<div id=\"" << id << "\" class=\"sad-card\"";
            std::string css = styleToCSS(node->style);
            if (!css.empty()) ss << " style=\"" << css << "\"";
            ss << ">\n";
            for (const auto& child : node->children) {
                emitHTML(ss, child, indent + 1);
            }
            ss << pad(indent) << "</div>\n";
            break;
        }

        // ─── Container / Box / Surface ─────────
        case WidgetType::Container:
        case WidgetType::Box:
        case WidgetType::Surface: {
            ss << pad(indent) << "<div id=\"" << id << "\"";
            std::string css = styleToCSS(node->style);
            if (!css.empty()) ss << " style=\"" << css << "\"";
            ss << ">\n";
            for (const auto& child : node->children) {
                emitHTML(ss, child, indent + 1);
            }
            ss << pad(indent) << "</div>\n";
            break;
        }

        // ─── Spacer ────────────────────────────
        case WidgetType::Spacer: {
            ss << pad(indent) << "<div class=\"sad-spacer\"></div>\n";
            break;
        }

        // ─── Divider ───────────────────────────
        case WidgetType::Divider: {
            ss << pad(indent) << "<hr class=\"sad-divider\">\n";
            break;
        }

        // ─── List ──────────────────────────────
        case WidgetType::List: {
            ss << pad(indent) << "<ul id=\"" << id << "\"";
            std::string css = styleToCSS(node->style);
            if (!css.empty()) ss << " style=\"" << css << "\"";
            ss << ">\n";
            for (const auto& child : node->children) {
                ss << pad(indent+1) << "<li>\n";
                emitHTML(ss, child, indent + 2);
                ss << pad(indent+1) << "</li>\n";
            }
            ss << pad(indent) << "</ul>\n";
            break;
        }

        // ─── ProgressBar ──────────────────────
        case WidgetType::ProgressBar: {
            ss << pad(indent) << "<progress id=\"" << id << "\"></progress>\n";
            break;
        }

        // ─── TabView ──────────────────────────
        case WidgetType::TabView: {
            ss << pad(indent) << "<div id=\"" << id << "\" class=\"sad-tabs\">\n";
            ss << pad(indent+1) << "<div class=\"sad-tab-bar\">\n";
            for (size_t i = 0; i < node->children.size(); i++) {
                ss << pad(indent+2) << "<button class=\"sad-tab\" onclick=\"showTab('"
                   << id << "', " << i << ")\">Tab " << (i+1) << "</button>\n";
            }
            ss << pad(indent+1) << "</div>\n";
            for (size_t i = 0; i < node->children.size(); i++) {
                ss << pad(indent+1) << "<div class=\"sad-tab-content\" style=\"display: "
                   << (i == 0 ? "block" : "none") << "\">\n";
                emitHTML(ss, node->children[i], indent + 2);
                ss << pad(indent+1) << "</div>\n";
            }
            ss << pad(indent) << "</div>\n";
            break;
        }

        // ─── ForEach ──────────────────────────
        case WidgetType::ForEach: {
            ss << pad(indent) << "<div id=\"" << id << "\" class=\"sad-foreach\">\n";
            ss << pad(indent+1) << "<!-- Template for dynamic items -->\n";
            for (const auto& child : node->children) {
                emitHTML(ss, child, indent + 1);
            }
            ss << pad(indent) << "</div>\n";
            break;
        }

        // ─── Group ─────────────────────────────
        case WidgetType::Group: {
            for (const auto& child : node->children) {
                emitHTML(ss, child, indent);
            }
            break;
        }

        // ─── Default ──────────────────────────
        default: {
            ss << pad(indent) << "<div id=\"" << id << "\"><!-- unsupported --></div>\n";
            break;
        }
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد CSS لعقدة
// ═══════════════════════════════════════════════════════════════════════════

void WebHTMLCodeGen::emitCSS(std::ostringstream& ss, const UINodeIRPtr& node) {
    if (!node) return;

    int myId = idCounter_++;
    std::string id = !node->tag.has_value() ? ("n" + std::to_string(myId)) : node->tag.value();

    // Only emit if node has significant custom styles
    std::string css = styleToCSS(node->style);
    if (!css.empty()) {
        ss << "#" << id << " {\n";

        // Break into lines
        std::string prop;
        for (char c : css) {
            if (c == ';') {
                if (!prop.empty()) {
                    ss << "    " << prop << ";\n";
                    prop.clear();
                }
            } else {
                prop += c;
            }
        }
        if (!prop.empty()) {
            ss << "    " << prop << ";\n";
        }

        ss << "}\n\n";
    }

    for (const auto& child : node->children) {
        emitCSS(ss, child);
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// توليد JS أحداث
// ═══════════════════════════════════════════════════════════════════════════

void WebHTMLCodeGen::emitJS(std::ostringstream& ss, const UINodeIRPtr& node) {
    if (!node) return;

    int myId = idCounter_++;
    std::string id = !node->tag.has_value() ? ("n" + std::to_string(myId)) : node->tag.value();

    for (const auto& ev : node->eventHandlers) {
        std::string jsEvent;
        switch (ev.type) {
            case EventType::Tap: jsEvent = "click"; break;
            case EventType::DoubleTap: jsEvent = "dblclick"; break;
            case EventType::LongPress: jsEvent = "contextmenu"; break;
            case EventType::TextChange: jsEvent = "input"; break;
            case EventType::ValueChange: jsEvent = "change"; break;
            case EventType::Focus: jsEvent = "focus"; break;
            case EventType::Blur: jsEvent = "blur"; break;
            case EventType::Submit: jsEvent = "submit"; break;
            default: continue;
        }

        ss << "document.getElementById('" << id << "')?.addEventListener('" << jsEvent << "', () => {\n";
        if (!ev.handlerName.empty()) {
            ss << "    " << ev.handlerName << "();\n";
        }
        if (!ev.closureBody.empty()) {
            ss << "    " << ev.closureBody << "\n";
        }
        ss << "});\n\n";
    }

    // State bindings for inputs
    if (node->stateBinding) {
        const auto& sb = *node->stateBinding;
        if (node->type == WidgetType::TextField) {
            ss << "document.getElementById('" << id << "')?.addEventListener('input', (e) => {\n";
            ss << "    " << sb.name << " = e.target.value;\n";
            ss << "});\n\n";
        }
    }

    for (const auto& child : node->children) {
        emitJS(ss, child);
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// تحويل StyleIR → CSS
// ═══════════════════════════════════════════════════════════════════════════

std::string WebHTMLCodeGen::styleToCSS(const StyleIR& style) {
    std::string css;

    if (!style.width.isAuto()) {
        css += "width: " + std::to_string((int)style.width.value) + "px;";
    }
    if (!style.height.isAuto()) {
        css += "height: " + std::to_string((int)style.height.value) + "px;";
    }
    if (style.width.isFill()) {
        css += "width: 100%;";
    }
    if (style.height.isFill()) {
        css += "height: 100%;";
    }

    if (style.padding.top > 0) {
        const auto& p = style.padding;
        css += "padding: " + std::to_string((int)p.top) + "px "
             + std::to_string((int)p.trailing) + "px "
             + std::to_string((int)p.bottom) + "px "
             + std::to_string((int)p.leading) + "px;";
    }
    if (style.margin.top > 0) {
        const auto& m = style.margin;
        css += "margin: " + std::to_string((int)m.top) + "px "
             + std::to_string((int)m.trailing) + "px "
             + std::to_string((int)m.bottom) + "px "
             + std::to_string((int)m.leading) + "px;";
    }

    if (style.backgroundColor) {
        css += "background-color: " + cssColor(*style.backgroundColor) + ";";
    }
    if (style.foregroundColor) {
        css += "color: " + cssColor(*style.foregroundColor) + ";";
    }

    if (style.fontSize) {
        css += "font-size: " + std::to_string((int)*style.fontSize) + "px;";
    }
    if (style.fontWeight) {
        int weight = 400;
        switch (*style.fontWeight) {
            case FontWeight::Thin: weight = 100; break;
            case FontWeight::Light: weight = 300; break;
            case FontWeight::Regular: weight = 400; break;
            case FontWeight::Medium: weight = 500; break;
            case FontWeight::SemiBold: weight = 600; break;
            case FontWeight::Bold: weight = 700; break;
            case FontWeight::ExtraBold: weight = 800; break;
            case FontWeight::Black: weight = 900; break;
        }
        css += "font-weight: " + std::to_string(weight) + ";";
    }

    if (style.borderRadius > 0) {
        css += "border-radius: " + std::to_string((int)style.borderRadius) + "px;";
    }

    if (style.borderWidth > 0) {
        css += "border-width: " + std::to_string((int)style.borderWidth) + "px;";
        css += "border-style: solid;";
    }
    if (style.borderColor) {
        css += "border-color: " + cssColor(*style.borderColor) + ";";
    }

    if (style.opacity < 1.0f) {
        css += "opacity: " + std::to_string(style.opacity) + ";";
    }

    if (style.shadow) {
        css += "box-shadow: " + std::to_string((int)style.shadow->offsetX) + "px "
             + std::to_string((int)style.shadow->offsetY) + "px "
             + std::to_string((int)style.shadow->blur) + "px "
             + cssColor(style.shadow->color) + ";";
    }

    if (style.hidden) {
        css += "display: none;";
    }

    // Alignment
    if (style.horizontalAlignment) {
        switch (*style.horizontalAlignment) {
            case HAlignment::Leading: css += "text-align: start;"; break;
            case HAlignment::Center: css += "text-align: center;"; break;
            case HAlignment::Trailing: css += "text-align: end;"; break;
        }
    }

    return css;
}


std::string WebHTMLCodeGen::cssColor(const Color& c) {
    if (c.a < 1.0f) {
        return "rgba(" + std::to_string(c.r) + ", " + std::to_string(c.g)
             + ", " + std::to_string(c.b) + ", " + std::to_string(c.a) + ")";
    }
    // Hex format
    std::ostringstream hex;
    hex << "#" << std::hex << std::setfill('0')
        << std::setw(2) << (int)c.r
        << std::setw(2) << (int)c.g
        << std::setw(2) << (int)c.b;
    return hex.str();
}


std::string WebHTMLCodeGen::nodeId(const UINodeIRPtr& node) {
    return !node->tag.has_value() ? ("node-" + std::to_string(idCounter_)) : node->tag.value();
}


std::string WebHTMLCodeGen::nodeClass(const UINodeIRPtr& node) {
    switch (node->type) {
        case WidgetType::Column: return "sad-column";
        case WidgetType::Row: return "sad-row";
        case WidgetType::Stack: return "sad-stack";
        case WidgetType::Card: return "sad-card";
        case WidgetType::Button: return "sad-button";
        case WidgetType::TextField: return "sad-textfield";
        case WidgetType::Scaffold: return "sad-scaffold";
        case WidgetType::AppBar: return "sad-appbar";
        default: return "";
    }
}


std::string WebHTMLCodeGen::pad(int indent) {
    return std::string(indent * 4, ' ');
}


} // namespace ir
} // namespace ui
} // namespace sad
