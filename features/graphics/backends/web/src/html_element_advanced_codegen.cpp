/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: html_element_advanced_codegen.cpp
 * المسار: features/graphics/backends/web/src/html_element_advanced_codegen.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * توليد العناصر المتقدمة: بيانات، وسائط، حركة، تحكم، تنقل، حاويات.
 * يحتوي على generateAdvancedElement() و generateNavElement().
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/web/html_codegen.h"
#include "sad_ui/color_utils.h"
#include <optional>

namespace sad {
namespace ui {
namespace web {

// ═══ عناصر البيانات والوسائط والحركة والتحكم المتقدم ═══

void HtmlCodegen::generateAdvancedElement(
    std::ostringstream& out,
    const IRNode& node,
    int indentLevel
) {
    std::string i = ind(indentLevel);
    std::string style = generateInlineStyle(node);

    switch (node.getType()) {

        // ── §11 مكونات البيانات (Data Components) ──

        case UINodeType::Tooltip: {
            const auto* textProp = node.findProperty("text");
            if (!textProp) textProp = node.findProperty("\xd9\x86\xd8\xb5"); // نص
            if (!textProp) textProp = node.findProperty("\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89"); // محتوى
            std::string tip = textProp ?
                (std::get_if<std::string>(&textProp->value) ?
                    *std::get_if<std::string>(&textProp->value) : "\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad") : "\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad";
            out << i << "<div class=\"sad-tooltip\" title=\"" << tip << "\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</div>\n";
            break;
        }

        case UINodeType::DataTable: {
            out << i << "<table class=\"sad-data-table\"" << style << ">\n";
            out << i << "  <thead><tr>\n";
            const auto* colsProp = node.findProperty("columns");
            if (colsProp) {
                if (auto* cols = std::get_if<std::string>(&colsProp->value))
                    out << i << "    <th>" << *cols << "</th>\n";
            }
            out << i << "  </tr></thead>\n";
            out << i << "  <tbody>\n";
            for (const auto& child : node.getChildren()) {
                out << i << "    <tr><td>\n";
                out << generateNode(*child, indentLevel + 3);
                out << i << "    </td></tr>\n";
            }
            out << i << "  </tbody>\n";
            out << i << "</table>\n";
            break;
        }

        case UINodeType::TreeView: {
            out << i << "<ul class=\"sad-tree-view\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << i << "  <li class=\"sad-tree-node\">\n";
                out << generateNode(*child, indentLevel + 2);
                out << i << "  </li>\n";
            }
            out << i << "</ul>\n";
            break;
        }

        case UINodeType::Breadcrumb: {
            out << i << "<nav class=\"sad-breadcrumb\" aria-label=\"breadcrumb\"" << style << ">\n";
            out << i << "  <ol>\n";
            for (const auto& child : node.getChildren()) {
                out << i << "    <li class=\"sad-breadcrumb-item\">\n";
                out << generateNode(*child, indentLevel + 3);
                out << i << "    </li>\n";
            }
            out << i << "  </ol>\n";
            out << i << "</nav>\n";
            break;
        }

        case UINodeType::Pagination: {
            out << i << "<nav class=\"sad-pagination\" aria-label=\"pagination\"" << style << ">\n";
            out << i << "  <button class=\"sad-button\" onclick=\"prevPage()\">&laquo;</button>\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "  <button class=\"sad-button\" onclick=\"nextPage()\">&raquo;</button>\n";
            out << i << "</nav>\n";
            break;
        }

        case UINodeType::Timeline: {
            out << i << "<div class=\"sad-timeline\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << i << "  <div class=\"sad-timeline-item\">\n";
                out << i << "    <div class=\"sad-timeline-dot\"></div>\n";
                out << i << "    <div class=\"sad-timeline-content\">\n";
                out << generateNode(*child, indentLevel + 3);
                out << i << "    </div>\n";
                out << i << "  </div>\n";
            }
            out << i << "</div>\n";
            break;
        }

        // ── §12 مكونات الوسائط (Media Components) ──

        case UINodeType::Carousel: {
            out << i << "<div class=\"sad-carousel\" role=\"region\" aria-roledescription=\"carousel\" aria-label=\"\xd9\x85\xd8\xb9\xd8\xb1\xd8\xb6 \xd8\xb5\xd9\x88\xd8\xb1\"" << style << ">\n";
            out << i << "  <button class=\"sad-carousel-prev\" onclick=\"carouselPrev(this)\">&#10094;</button>\n";
            out << i << "  <div class=\"sad-carousel-track\">\n";
            for (const auto& child : node.getChildren()) {
                out << i << "    <div class=\"sad-carousel-slide\">\n";
                out << generateNode(*child, indentLevel + 3);
                out << i << "    </div>\n";
            }
            out << i << "  </div>\n";
            out << i << "  <button class=\"sad-carousel-next\" onclick=\"carouselNext(this)\">&#10095;</button>\n";
            out << i << "</div>\n";
            break;
        }

        case UINodeType::RichText: {
            out << i << "<div class=\"sad-rich-text\" contenteditable=\"false\"" << style << ">\n";
            const auto* textProp = node.findProperty("text");
            if (!textProp) textProp = node.findProperty("\xd9\x86\xd8\xb5"); // نص
            if (!textProp) textProp = node.findProperty("\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89"); // محتوى
            if (textProp) {
                if (auto* text = std::get_if<std::string>(&textProp->value))
                    out << i << "  " << *text << "\n";
            }
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</div>\n";
            break;
        }

        case UINodeType::Markdown: {
            out << i << "<div class=\"sad-markdown\"" << style << ">\n";
            const auto* textProp = node.findProperty("text");
            if (!textProp) textProp = node.findProperty("\xd9\x86\xd8\xb5"); // نص
            if (!textProp) textProp = node.findProperty("\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89"); // محتوى
            if (textProp) {
                if (auto* text = std::get_if<std::string>(&textProp->value))
                    out << i << "  <pre>" << *text << "</pre>\n";
            }
            out << i << "</div>\n";
            break;
        }

        case UINodeType::CodeBlock: {
            const auto* textProp = node.findProperty("text");
            if (!textProp) textProp = node.findProperty("\xd9\x86\xd8\xb5"); // نص
            if (!textProp) textProp = node.findProperty("\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89"); // محتوى
            if (!textProp) textProp = node.findProperty("\xd9\x83\xd9\x88\xd8\xaf"); // كود
            const auto* langProp = node.findProperty("language");
            if (!langProp) langProp = node.findProperty("\xd9\x84\xd8\xba\xd8\xa9"); // لغة
            std::string lang = langProp ?
                (std::get_if<std::string>(&langProp->value) ?
                    *std::get_if<std::string>(&langProp->value) : "") : "";
            // تحقق من خاصية أرقام_أسطر
            const auto* lineNumProp = node.findProperty("\xd8\xa3\xd8\xb1\xd9\x82\xd8\xa7\xd9\x85_\xd8\xa3\xd8\xb3\xd8\xb7\xd8\xb1");
            bool showLineNums = true;
            if (lineNumProp) {
                if (auto* b = std::get_if<bool>(&lineNumProp->value)) showLineNums = *b;
            }

            out << i << "<div class=\"sad-code-wrapper\"" << style << ">\n";
            if (showLineNums && textProp) {
                if (auto* codeText = std::get_if<std::string>(&textProp->value)) {
                    // حساب عدد الأسطر
                    int lineCount = 1;
                    for (char c : *codeText) if (c == '\n') ++lineCount;
                    out << i << "  <div class=\"sad-line-numbers\" aria-hidden=\"true\">";
                    for (int ln = 1; ln <= lineCount; ++ln) {
                        out << "<span>" << ln << "</span>";
                    }
                    out << "</div>\n";
                }
            }
            out << i << "  <pre class=\"sad-code-block\"><code";
            if (!lang.empty()) out << " class=\"language-" << lang << "\"";
            out << ">";
            if (textProp) {
                if (auto* text = std::get_if<std::string>(&textProp->value))
                    out << *text;
            }
            out << "</code></pre>\n";
            out << i << "</div>\n";
            break;
        }

        case UINodeType::AudioPlayer: {
            const auto* srcProp = node.findProperty("src");
            std::string src = srcProp ?
                (std::get_if<std::string>(&srcProp->value) ?
                    *std::get_if<std::string>(&srcProp->value) : "") : "";
            out << i << "<audio class=\"sad-audio\" controls" << style << ">\n";
            if (!src.empty())
                out << i << "  <source src=\"" << src << "\">\n";
            out << i << "</audio>\n";
            break;
        }

        case UINodeType::ImageGallery: {
            out << i << "<div class=\"sad-image-gallery\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << i << "  <div class=\"sad-gallery-item\">\n";
                out << generateNode(*child, indentLevel + 2);
                out << i << "  </div>\n";
            }
            out << i << "</div>\n";
            break;
        }

        // ── §13 مكونات الحركة (Animation Components) ──

        case UINodeType::Skeleton: {
            out << i << "<div class=\"sad-skeleton\"" << style << ">\n";
            out << i << "  <div class=\"sad-skeleton-line\"></div>\n";
            out << i << "  <div class=\"sad-skeleton-line short\"></div>\n";
            out << i << "</div>\n";
            break;
        }

        case UINodeType::Shimmer: {
            out << i << "<div class=\"sad-shimmer\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</div>\n";
            break;
        }

        case UINodeType::AnimatedList: {
            out << i << "<div class=\"sad-animated-list\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << i << "  <div class=\"sad-animated-item\">\n";
                out << generateNode(*child, indentLevel + 2);
                out << i << "  </div>\n";
            }
            out << i << "</div>\n";
            break;
        }

        case UINodeType::Expandable: {
            const auto* textProp = node.findProperty("text");
            if (!textProp) textProp = node.findProperty("\xd9\x86\xd8\xb5"); // نص
            if (!textProp) textProp = node.findProperty("\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86"); // عنوان
            std::string title = textProp ?
                (std::get_if<std::string>(&textProp->value) ?
                    *std::get_if<std::string>(&textProp->value) : "\xd8\xaa\xd9\x88\xd8\xb3\xd9\x8a\xd8\xb9") : "\xd8\xaa\xd9\x88\xd8\xb3\xd9\x8a\xd8\xb9";
            out << i << "<details class=\"sad-expandable\"" << style << ">\n";
            out << i << "  <summary>" << title << "</summary>\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</details>\n";
            break;
        }

        case UINodeType::Collapsible: {
            const auto* textProp = node.findProperty("text");
            if (!textProp) textProp = node.findProperty("\xd9\x86\xd8\xb5"); // نص
            if (!textProp) textProp = node.findProperty("\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86"); // عنوان
            std::string title = textProp ?
                (std::get_if<std::string>(&textProp->value) ?
                    *std::get_if<std::string>(&textProp->value) : "\xd8\xb7\xd9\x8a") : "\xd8\xb7\xd9\x8a";
            out << i << "<details class=\"sad-collapsible\"" << style << ">\n";
            out << i << "  <summary>" << title << "</summary>\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</details>\n";
            break;
        }

        case UINodeType::Swipeable: {
            out << i << "<div class=\"sad-swipeable\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</div>\n";
            break;
        }

        // ── §14 مكونات التحكم المتقدمة (Advanced Controls) ──

        case UINodeType::Stepper: {
            out << i << "<div class=\"sad-stepper\"" << style << ">\n";
            out << i << "  <button onclick=\"stepDown(this)\">\xe2\x88\x92</button>\n";
            out << i << "  <span class=\"sad-stepper-value\">0</span>\n";
            out << i << "  <button onclick=\"stepUp(this)\">+</button>\n";
            out << i << "</div>\n";
            break;
        }

        case UINodeType::SegmentedControl: {
            out << i << "<div class=\"sad-segmented-control\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << i << "  <button class=\"sad-segment\">\n";
                out << generateNode(*child, indentLevel + 2);
                out << i << "  </button>\n";
            }
            out << i << "</div>\n";
            break;
        }

        case UINodeType::RatingBar: {
            out << i << "<div class=\"sad-rating\" role=\"radiogroup\" aria-label=\"\xd8\xa7\xd9\x84\xd8\xaa\xd9\x82\xd9\x8a\xd9\x8a\xd9\x85\"" << style << ">\n";
            for (int s = 1; s <= 5; s++) {
                out << i << "  <span class=\"sad-star\" data-value=\"" << s << "\">\xe2\x98\x85</span>\n";
            }
            out << i << "</div>\n";
            break;
        }

        case UINodeType::TimePicker: {
            const auto* valProp = node.findProperty("\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9");
            std::string timeVal;
            if (valProp) { if (auto* s = std::get_if<std::string>(&valProp->value)) timeVal = *s; }
            out << i << "<label class=\"sad-time-picker-label\">\n";
            out << i << "  <span>\xd8\xa7\xd9\x84\xd9\x88\xd9\x82\xd8\xaa</span>\n";
            out << i << "  <input type=\"time\" class=\"sad-time-picker\""
                << (timeVal.empty() ? "" : " value=\"" + timeVal + "\"") << style << " />\n";
            out << i << "</label>\n";
            break;
        }

        case UINodeType::DatePicker: {
            const auto* valProp = node.findProperty("\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9");
            std::string dateVal;
            if (valProp) { if (auto* s = std::get_if<std::string>(&valProp->value)) dateVal = *s; }
            out << i << "<label class=\"sad-date-picker-label\">\n";
            out << i << "  <span>\xd8\xa7\xd9\x84\xd8\xaa\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xae</span>\n";
            out << i << "  <input type=\"date\" class=\"sad-date-picker\""
                << (dateVal.empty() ? "" : " value=\"" + dateVal + "\"") << style << " />\n";
            out << i << "</label>\n";
            break;
        }

        case UINodeType::Calendar: {
            const auto* valProp = node.findProperty("\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9");
            std::string calVal;
            if (valProp) { if (auto* s = std::get_if<std::string>(&valProp->value)) calVal = *s; }
            out << i << "<div class=\"sad-calendar-wrapper\"" << style << ">\n";
            out << i << "  <input type=\"date\" class=\"sad-calendar\""
                << (calVal.empty() ? "" : " value=\"" + calVal + "\"") << " />\n";
            out << i << "</div>\n";
            break;
        }

        case UINodeType::ColorPicker: {
            const auto* valProp = node.findProperty("\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9");
            if (!valProp) valProp = node.findProperty("\xd9\x84\xd9\x88\xd9\x86");
            std::string colorVal = "#000000";
            if (valProp) { if (auto* s = std::get_if<std::string>(&valProp->value)) colorVal = *s; }
            out << i << "<label class=\"sad-color-picker-label\">\n";
            out << i << "  <span>\xd8\xa7\xd9\x84\xd9\x84\xd9\x88\xd9\x86</span>\n";
            out << i << "  <input type=\"color\" class=\"sad-color-picker\" value=\"" << colorVal << "\"" << style << " />\n";
            out << i << "</label>\n";
            break;
        }

        case UINodeType::ColorWheel: {
            out << i << "<input type=\"color\" class=\"sad-color-wheel\"" << style << " />\n";
            break;
        }

        default:
            break;
    }
}

// ═══ عناصر التنقل والحاويات ═══

void HtmlCodegen::generateNavElement(
    std::ostringstream& out,
    const IRNode& node,
    int indentLevel
) {
    std::string i = ind(indentLevel);
    std::string style = generateInlineStyle(node);

    switch (node.getType()) {

        // ── §15 مكونات التنقل المتقدمة (Extended Navigation) ──

        case UINodeType::BottomNav: {
            out << i << "<nav class=\"sad-bottom-nav\" aria-label=\"\xd8\xa7\xd9\x84\xd8\xaa\xd9\x86\xd9\x82\xd9\x84 \xd8\xa7\xd9\x84\xd8\xb3\xd9\x81\xd9\x84\xd9\x8a\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << i << "  <a class=\"sad-bottom-nav-item\">\n";
                out << generateNode(*child, indentLevel + 2);
                out << i << "  </a>\n";
            }
            out << i << "</nav>\n";
            break;
        }

        case UINodeType::SideNav: {
            out << i << "<nav class=\"sad-side-nav\" aria-label=\"\xd8\xa7\xd9\x84\xd8\xaa\xd9\x86\xd9\x82\xd9\x84 \xd8\xa7\xd9\x84\xd8\xac\xd8\xa7\xd9\x86\xd8\xa8\xd9\x8a\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << i << "  <a class=\"sad-side-nav-item\">\n";
                out << generateNode(*child, indentLevel + 2);
                out << i << "  </a>\n";
            }
            out << i << "</nav>\n";
            break;
        }

        case UINodeType::Toolbar: {
            out << i << "<div class=\"sad-toolbar\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</div>\n";
            break;
        }

        case UINodeType::SplitView: {
            out << i << "<div class=\"sad-split-view\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << i << "  <div class=\"sad-split-pane\">\n";
                out << generateNode(*child, indentLevel + 2);
                out << i << "  </div>\n";
            }
            out << i << "</div>\n";
            break;
        }

        case UINodeType::Tabs: {
            out << i << "<div class=\"sad-tabs-bar\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << i << "  <button class=\"sad-tab\">\n";
                out << generateNode(*child, indentLevel + 2);
                out << i << "  </button>\n";
            }
            out << i << "</div>\n";
            break;
        }

        case UINodeType::MegaMenu: {
            out << i << "<nav class=\"sad-mega-menu\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << i << "  <div class=\"sad-mega-menu-section\">\n";
                out << generateNode(*child, indentLevel + 2);
                out << i << "  </div>\n";
            }
            out << i << "</nav>\n";
            break;
        }

        // ── أنواع أخرى ──

        case UINodeType::LazyGrid: {
            out << i << "<div class=\"sad-grid sad-lazy\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</div>\n";
            break;
        }

        case UINodeType::NavigationLink: {
            out << i << "<a class=\"sad-nav-link\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</a>\n";
            break;
        }

        case UINodeType::TabItem: {
            out << i << "<div class=\"sad-tab-item\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</div>\n";
            break;
        }

        case UINodeType::Section: {
            const auto* textProp = node.findProperty("text");
            if (!textProp) textProp = node.findProperty("\xd9\x86\xd8\xb5"); // نص
            if (!textProp) textProp = node.findProperty("\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86"); // عنوان
            std::string title = textProp ?
                (std::get_if<std::string>(&textProp->value) ?
                    *std::get_if<std::string>(&textProp->value) : "") : "";
            out << i << "<section class=\"sad-section\"" << style << ">\n";
            if (!title.empty())
                out << i << "  <h3 class=\"sad-section-header\">" << title << "</h3>\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</section>\n";
            break;
        }

        case UINodeType::ForEach: {
            out << i << "<div class=\"sad-foreach\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</div>\n";
            break;
        }

        case UINodeType::Sheet: {
            out << i << "<div class=\"sad-sheet\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</div>\n";
            break;
        }

        case UINodeType::Menu: {
            out << i << "<div class=\"sad-menu\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << i << "  <div class=\"sad-menu-item\">\n";
                out << generateNode(*child, indentLevel + 2);
                out << i << "  </div>\n";
            }
            out << i << "</div>\n";
            break;
        }

        case UINodeType::MapView: {
            const auto* latProp = node.findProperty("lat");
            const auto* lngProp = node.findProperty("lng");
            std::string lat = "24.7136", lng = "46.6753";
            if (latProp) { if (auto* d = std::get_if<double>(&latProp->value)) lat = std::to_string(*d); }
            if (lngProp) { if (auto* d = std::get_if<double>(&lngProp->value)) lng = std::to_string(*d); }
            out << i << "<div class=\"sad-map\"" << style << ">\n";
            out << i << "  <iframe src=\"https://www.openstreetmap.org/export/embed.html?bbox="
                << lng << "," << lat << "," << lng << "," << lat
                << "\" style=\"width:100%;height:100%;border:none;\"></iframe>\n";
            out << i << "  <noscript>\xf0\x9f\x97\xba\xef\xb8\x8f \xd8\xae\xd8\xb1\xd9\x8a\xd8\xb7\xd8\xa9</noscript>\n";
            out << i << "</div>\n";
            break;
        }

        case UINodeType::WebView: {
            const auto* srcProp = node.findProperty("src");
            std::string src = srcProp ?
                (std::get_if<std::string>(&srcProp->value) ?
                    *std::get_if<std::string>(&srcProp->value) : "about:blank") : "about:blank";
            out << i << "<iframe class=\"sad-webview\" src=\"" << src << "\"" << style << "></iframe>\n";
            break;
        }

        case UINodeType::VideoPlayer: {
            const auto* srcProp = node.findProperty("src");
            std::string src = srcProp ?
                (std::get_if<std::string>(&srcProp->value) ?
                    *std::get_if<std::string>(&srcProp->value) : "") : "";
            out << i << "<video class=\"sad-video\" controls" << style << ">\n";
            if (!src.empty())
                out << i << "  <source src=\"" << src << "\">\n";
            out << i << "</video>\n";
            break;
        }

        case UINodeType::CustomWidget: {
            out << i << "<div class=\"sad-custom-widget\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</div>\n";
            break;
        }

        case UINodeType::Conditional: {
            out << i << "<div class=\"sad-conditional\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</div>\n";
            break;
        }

        // ═══ الحاويات الأساسية والافتراضي ═══
        case UINodeType::Column:
        case UINodeType::Row:
        case UINodeType::Stack:
        case UINodeType::Grid:
        case UINodeType::Container:
        case UINodeType::Box:
        case UINodeType::Card:
        case UINodeType::ScrollView:
        case UINodeType::Surface:
        case UINodeType::Wrap:
        case UINodeType::Group:
        default: {
            std::string cssClass;
            switch (node.getType()) {
                case UINodeType::Column:    cssClass = "sad-column"; break;
                case UINodeType::Row:       cssClass = "sad-row"; break;
                case UINodeType::Stack:     cssClass = "sad-stack"; break;
                case UINodeType::Grid:      cssClass = "sad-grid"; break;
                case UINodeType::Card:      cssClass = "sad-card"; break;
                case UINodeType::ScrollView:cssClass = "sad-scroll-view"; break;
                case UINodeType::Container: cssClass = "sad-container"; break;
                case UINodeType::Surface:   cssClass = "sad-surface"; break;
                case UINodeType::Wrap:      cssClass = "sad-wrap"; break;
                case UINodeType::Group:     cssClass = "sad-group"; break;
                default:                    cssClass = "sad-column"; break;
            }

            out << i << "<div class=\"" << cssClass << "\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</div>\n";
            break;
        }
    }
}

} // namespace web
} // namespace ui
} // namespace sad
