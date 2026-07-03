/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: html_element_codegen.cpp
 * المسار: features/graphics/backends/web/src/html_element_codegen.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * توليد العناصر الأساسية: نص، زر، صورة، حقول إدخال، قائمة، حاويات أساسية.
 * يحتوي على generateElement() (الموزّع) و generateBasicElement().
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/web/html_codegen.h"
#include "sad_ui/color_utils.h"
#include "sad_ui/prop_keys.h" // مصدر الحقيقة لمفاتيح الخصائص (لا سلاسل حرفيّة)
#include <optional>

namespace sad {
namespace ui {
namespace web {

namespace {
// (AR) نصّ العنصر بالترتيب القانونيّ («عنوان» أوّلًا) مع بدائل وهروب محارف
//   HTML (& < >) كي لا يكسر عنوانٌ فيها بنيةَ الصفحة أو يفتح ثغرة حقن. كان
//   فرع الزرّ يقرأ "text/نص/محتوى" فيفوّت «عنوان» ⇒ أزرار الويب فارغة.
inline std::string htmlLabel(const IRNode& node) {
    static const char* keys[] = {props::TITLE, props::ICON, props::TEXT_LATIN, props::TEXT, props::CONTENT};
    std::string raw;
    for (const char* k : keys) {
        const auto* p = node.findProperty(k);
        if (!p) continue;
        if (auto* s = std::get_if<std::string>(&p->value)) { raw = *s; break; }
        if (auto* iv = std::get_if<int64_t>(&p->value)) { raw = std::to_string(*iv); break; }
        if (auto* dv = std::get_if<double>(&p->value)) { raw = std::to_string(*dv); break; }
    }
    std::string out;
    for (char c : raw) {
        if (c == '&') out += "&amp;";
        else if (c == '<') out += "&lt;";
        else if (c == '>') out += "&gt;";
        else out += c;
    }
    return out;
}
} // namespace

// ═══ الموزّع الرئيسي — يوجّه كل نوع عقدة للدالة المناسبة ═══

void HtmlCodegen::generateElement(
    std::ostringstream& out,
    const IRNode& node,
    int indentLevel
) {
    switch (node.getType()) {
        // ═══ العناصر الأساسية (Basic Widgets) ═══
        case UINodeType::Text:
        case UINodeType::Button:
        case UINodeType::Image:
        case UINodeType::TextField:
        case UINodeType::TextArea:
        case UINodeType::Spacer:
        case UINodeType::Divider:
        case UINodeType::Toggle:
        case UINodeType::Slider:
        case UINodeType::Checkbox:
        case UINodeType::Radio:
        case UINodeType::Picker:
        case UINodeType::SearchBar:
        case UINodeType::ProgressBar:
        case UINodeType::Badge:
        case UINodeType::Chip:
        case UINodeType::Avatar:
        case UINodeType::FAB:
        case UINodeType::SnackBar:
        case UINodeType::AppBar:
        case UINodeType::Icon:
        case UINodeType::Canvas:
        case UINodeType::Alert:
        case UINodeType::Dialog:
        case UINodeType::Scaffold:
        case UINodeType::Drawer:
        case UINodeType::BottomSheet:
        case UINodeType::TabView:
        case UINodeType::NavigationView:
        case UINodeType::LazyColumn:
        case UINodeType::LazyRow:
        case UINodeType::List:
            generateBasicElement(out, node, indentLevel);
            break;

        // ═══ عناصر البيانات والوسائط والحركة (Data/Media/Animation) ═══
        case UINodeType::Tooltip:
        case UINodeType::DataTable:
        case UINodeType::TreeView:
        case UINodeType::Breadcrumb:
        case UINodeType::Pagination:
        case UINodeType::Timeline:
        case UINodeType::Carousel:
        case UINodeType::RichText:
        case UINodeType::Markdown:
        case UINodeType::CodeBlock:
        case UINodeType::AudioPlayer:
        case UINodeType::ImageGallery:
        case UINodeType::Skeleton:
        case UINodeType::Shimmer:
        case UINodeType::AnimatedList:
        case UINodeType::Expandable:
        case UINodeType::Collapsible:
        case UINodeType::Swipeable:
        case UINodeType::Stepper:
        case UINodeType::SegmentedControl:
        case UINodeType::RatingBar:
        case UINodeType::TimePicker:
        case UINodeType::DatePicker:
        case UINodeType::Calendar:
        case UINodeType::ColorPicker:
        case UINodeType::ColorWheel:
            generateAdvancedElement(out, node, indentLevel);
            break;

        // ═══ التنقل والحاويات وكل شيء آخر ═══
        default:
            generateNavElement(out, node, indentLevel);
            break;
    }
}

// ═══ العناصر الأساسية ═══

void HtmlCodegen::generateBasicElement(
    std::ostringstream& out,
    const IRNode& node,
    int indentLevel
) {
    std::string i = ind(indentLevel);
    std::string style = generateInlineStyle(node);

    switch (node.getType()) {
        case UINodeType::Text: {
            std::string text = htmlLabel(node);
            out << i << "<span" << style << ">" << text << "</span>\n";
            break;
        }

        case UINodeType::Button: {
            std::string text = htmlLabel(node);

            std::string onclick;
            for (const auto& evt : node.getEvents()) {
                if (evt.type == sad::ui::IREventType::OnTap) {
                    onclick = " onclick=\"" + evt.expression + "; update();\"";
                    break;
                }
            }

            // لون الخلفية
            std::string bgColor = "#1E88E5";
            const auto* bgColorProp = node.findProperty("\xd9\x84\xd9\x88\xd9\x86_\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9");
            if (bgColorProp) {
                if (auto* colorStr = std::get_if<std::string>(&bgColorProp->value)) {
                    bgColor = arabicColorToCSS(*colorStr);
                }
            }

            std::string btnStyle = "background: " + bgColor + ";";
            if (!style.empty()) {
                std::string extraStyle = style;
                auto pos = extraStyle.find("\"");
                if (pos != std::string::npos) {
                    extraStyle = extraStyle.substr(pos + 1);
                    if (!extraStyle.empty() && extraStyle.back() == '"')
                        extraStyle.pop_back();
                    btnStyle += " " + extraStyle;
                }
            }
            out << i << "<button class=\"sad-button\"" << onclick 
                << " style=\"" << btnStyle << "\">"
                << text << "</button>\n";
            break;
        }

        case UINodeType::Image: {
            const auto* srcProp = node.findProperty("\xd9\x85\xd8\xb5\xd8\xaf\xd8\xb1"); // مصدر
            if (!srcProp) srcProp = node.findProperty("src");
            if (!srcProp) srcProp = node.findProperty("\xd9\x85\xd8\xb3\xd8\xa7\xd8\xb1"); // مسار
            if (!srcProp) srcProp = node.findProperty("text");
            std::string src = srcProp ?
                (std::get_if<std::string>(&srcProp->value) ?
                    *std::get_if<std::string>(&srcProp->value) : "") : "";
            const auto* altProp = node.findProperty("\xd9\x88\xd8\xb5\xd9\x81"); // وصف
            if (!altProp) altProp = node.findProperty("alt");
            std::string alt = altProp ?
                (std::get_if<std::string>(&altProp->value) ?
                    *std::get_if<std::string>(&altProp->value) : "\xd8\xb5\xd9\x88\xd8\xb1\xd8\xa9") : "\xd8\xb5\xd9\x88\xd8\xb1\xd8\xa9";
            out << i << "<img src=\"" << src << "\" alt=\"" << alt << "\" loading=\"lazy\"" << style << " />\n";
            break;
        }

        case UINodeType::TextField: {
            const auto* valProp = node.findProperty("\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9"); // قيمة
            if (!valProp) valProp = node.findProperty("value");
            std::string val = valProp ?
                (std::get_if<std::string>(&valProp->value) ?
                    *std::get_if<std::string>(&valProp->value) : "") : "";
            const auto* phProp = node.findProperty("\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad"); // تلميح
            if (!phProp) phProp = node.findProperty("placeholder");
            std::string ph = phProp ?
                (std::get_if<std::string>(&phProp->value) ?
                    *std::get_if<std::string>(&phProp->value) : "") : "";
            const auto* typeP = node.findProperty("\xd9\x86\xd9\x88\xd8\xb9"); // نوع
            std::string inputType = "text";
            if (typeP) {
                if (auto* t = std::get_if<std::string>(&typeP->value)) {
                    if (*t == "\xd9\x83\xd9\x84\xd9\x85\xd8\xa9_\xd9\x85\xd8\xb1\xd9\x88\xd8\xb1" || *t == "password") inputType = "password";
                    else if (*t == "\xd8\xa8\xd8\xb1\xd9\x8a\xd8\xaf" || *t == "email") inputType = "email";
                    else if (*t == "\xd8\xb1\xd9\x82\xd9\x85" || *t == "number") inputType = "number";
                }
            }
            out << i << "<input type=\"" << inputType << "\" class=\"sad-input\""
                << (val.empty() ? "" : " value=\"" + val + "\"")
                << (ph.empty() ? "" : " placeholder=\"" + ph + "\"")
                << style << " />\n";
            break;
        }

        case UINodeType::TextArea: {
            const auto* valProp = node.findProperty("\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9"); // قيمة
            if (!valProp) valProp = node.findProperty("value");
            std::string val = valProp ?
                (std::get_if<std::string>(&valProp->value) ?
                    *std::get_if<std::string>(&valProp->value) : "") : "";
            const auto* phProp = node.findProperty("\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad"); // تلميح
            if (!phProp) phProp = node.findProperty("placeholder");
            std::string ph = phProp ?
                (std::get_if<std::string>(&phProp->value) ?
                    *std::get_if<std::string>(&phProp->value) : "") : "";
            out << i << "<textarea class=\"sad-input\""
                << (ph.empty() ? "" : " placeholder=\"" + ph + "\"")
                << style << ">" << val << "</textarea>\n";
            break;
        }

        case UINodeType::Spacer: {
            float spacerH = 0;
            const auto* hProp = node.findProperty("\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9");
            if (!hProp) hProp = node.findProperty("height");
            if (hProp) {
                if (auto* d = std::get_if<double>(&hProp->value)) spacerH = static_cast<float>(*d);
                else if (auto* iv = std::get_if<int64_t>(&hProp->value)) spacerH = static_cast<float>(*iv);
            }
            if (spacerH > 0)
                out << i << "<div class=\"sad-spacer\" style=\"height:" << static_cast<int>(spacerH) << "px\"></div>\n";
            else
                out << i << "<div class=\"sad-spacer\" style=\"flex:1\"></div>\n";
            break;
        }

        case UINodeType::Divider: {
            const auto* colorProp = node.findProperty("\xd9\x84\xd9\x88\xd9\x86");
            if (colorProp) {
                if (auto* cs = std::get_if<std::string>(&colorProp->value)) {
                    out << i << "<hr class=\"sad-divider\" style=\"border-color:" << *cs << ";\" />\n";
                    break;
                }
            }
            out << i << "<hr class=\"sad-divider\" />\n";
            break;
        }

        case UINodeType::Toggle: {
            const auto* onProp = node.findProperty("\xd9\x85\xd9\x81\xd8\xb9\xd9\x84"); // مفعّل
            bool isOn = false;
            if (onProp) {
                if (auto* b = std::get_if<bool>(&onProp->value)) isOn = *b;
                else if (auto* s = std::get_if<std::string>(&onProp->value))
                    isOn = (*s == "true" || *s == "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad");
            }
            const auto* textProp = node.findProperty("text");
            if (!textProp) textProp = node.findProperty("\xd9\x86\xd8\xb5"); // نص
            std::string label = textProp ?
                (std::get_if<std::string>(&textProp->value) ?
                    *std::get_if<std::string>(&textProp->value) : "") : "";
            out << i << "<label class=\"sad-toggle\">\n";
            out << i << "  <input type=\"checkbox\" role=\"switch\" aria-checked=\"" << (isOn ? "true" : "false") << "\"" << (isOn ? " checked" : "") << style << " />\n";
            if (!label.empty())
                out << i << "  <span>" << label << "</span>\n";
            out << i << "</label>\n";
            break;
        }

        case UINodeType::Slider: {
            const auto* valProp = node.findProperty("\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9");
            const auto* minProp = node.findProperty("\xd8\xa3\xd8\xaf\xd9\x86\xd9\x89");
            if (!minProp) minProp = node.findProperty("min");
            const auto* maxProp = node.findProperty("\xd8\xa3\xd9\x82\xd8\xb5\xd9\x89");
            if (!maxProp) maxProp = node.findProperty("max");
            std::string minVal = "0", maxVal = "100", curVal = "50";
            if (minProp) { if (auto* d = std::get_if<double>(&minProp->value)) minVal = std::to_string(static_cast<int>(*d)); }
            if (maxProp) { if (auto* d = std::get_if<double>(&maxProp->value)) maxVal = std::to_string(static_cast<int>(*d)); }
            if (valProp) { if (auto* d = std::get_if<double>(&valProp->value)) curVal = std::to_string(static_cast<int>(*d)); }
            out << i << "<input type=\"range\" class=\"sad-slider\" min=\"" << minVal
                << "\" max=\"" << maxVal << "\" value=\"" << curVal << "\"" << style << " />\n";
            break;
        }

        case UINodeType::Checkbox: {
            const auto* onProp = node.findProperty("\xd9\x85\xd9\x81\xd8\xb9\xd9\x84"); // مفعّل
            bool checked = false;
            if (onProp) {
                if (auto* b = std::get_if<bool>(&onProp->value)) checked = *b;
                else if (auto* s = std::get_if<std::string>(&onProp->value))
                    checked = (*s == "true" || *s == "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad");
            }
            const auto* textProp = node.findProperty("text");
            if (!textProp) textProp = node.findProperty("\xd9\x86\xd8\xb5"); // نص
            std::string label = textProp ?
                (std::get_if<std::string>(&textProp->value) ?
                    *std::get_if<std::string>(&textProp->value) : "\xd8\xae\xd9\x8a\xd8\xa7\xd8\xb1") : "\xd8\xae\xd9\x8a\xd8\xa7\xd8\xb1";
            out << i << "<label>\n";
            out << i << "  <input type=\"checkbox\" class=\"sad-checkbox\""
                << (checked ? " checked" : "") << style << " />\n";
            out << i << "  <span>" << label << "</span>\n";
            out << i << "</label>\n";
            break;
        }

        case UINodeType::Radio: {
            const auto* onProp = node.findProperty("\xd9\x85\xd9\x81\xd8\xb9\xd9\x84"); // مفعّل
            bool selected = false;
            if (onProp) {
                if (auto* b = std::get_if<bool>(&onProp->value)) selected = *b;
            }
            const auto* textProp = node.findProperty("text");
            if (!textProp) textProp = node.findProperty("\xd9\x86\xd8\xb5"); // نص
            const auto* groupProp = node.findProperty("\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\xd8\xa9"); // مجموعة
            std::string groupName = groupProp ?
                (std::get_if<std::string>(&groupProp->value) ?
                    *std::get_if<std::string>(&groupProp->value) : "group") : "group";
            std::string label = textProp ?
                (std::get_if<std::string>(&textProp->value) ?
                    *std::get_if<std::string>(&textProp->value) : "\xd8\xae\xd9\x8a\xd8\xa7\xd8\xb1") : "\xd8\xae\xd9\x8a\xd8\xa7\xd8\xb1";
            out << i << "<label>\n";
            out << i << "  <input type=\"radio\" class=\"sad-radio\" name=\"" << groupName << "\""
                << (selected ? " checked" : "") << style << " />\n";
            out << i << "  <span>" << label << "</span>\n";
            out << i << "</label>\n";
            break;
        }

        case UINodeType::Picker: {
            out << i << "<select class=\"sad-picker\"" << style << ">\n";
            out << i << "  <option>\xd8\xae\xd9\x8a\xd8\xa7\xd8\xb1 1</option>\n";
            out << i << "  <option>\xd8\xae\xd9\x8a\xd8\xa7\xd8\xb1 2</option>\n";
            out << i << "</select>\n";
            break;
        }

        case UINodeType::SearchBar: {
            out << i << "<input type=\"search\" class=\"sad-search\" placeholder=\"\xd8\xa8\xd8\xad\xd8\xab...\" role=\"search\" aria-label=\"\xd8\xa8\xd8\xad\xd8\xab\"" << style << " />\n";
            break;
        }

        case UINodeType::ProgressBar: {
            const auto* valProp = node.findProperty("\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9"); // قيمة
            float pval = 50.0f;
            if (valProp) {
                if (auto* d = std::get_if<double>(&valProp->value)) pval = static_cast<float>(*d);
                else if (auto* iv = std::get_if<int64_t>(&valProp->value)) pval = static_cast<float>(*iv);
            }
            const auto* maxProp = node.findProperty("\xd8\xa3\xd9\x82\xd8\xb5\xd9\x89"); // أقصى
            if (!maxProp) maxProp = node.findProperty("max");
            float pmax = 100.0f;
            if (maxProp) {
                if (auto* d = std::get_if<double>(&maxProp->value)) pmax = static_cast<float>(*d);
            }
            out << i << "<progress class=\"sad-progress\" value=\"" << (int)pval
                << "\" max=\"" << (int)pmax << "\" role=\"progressbar\" aria-valuenow=\"" << (int)pval
                << "\" aria-valuemin=\"0\" aria-valuemax=\"" << (int)pmax << "\"" << style << "></progress>\n";
            break;
        }

        case UINodeType::Badge: {
            const auto* textProp = node.findProperty("text");
            if (!textProp) textProp = node.findProperty("\xd9\x86\xd8\xb5"); // نص
            std::string badgeText = textProp ?
                (std::get_if<std::string>(&textProp->value) ?
                    *std::get_if<std::string>(&textProp->value) : "0") : "0";
            out << i << "<span class=\"sad-badge\"" << style << ">" << badgeText << "</span>\n";
            break;
        }

        case UINodeType::Chip: {
            const auto* textProp = node.findProperty("text");
            if (!textProp) textProp = node.findProperty("\xd9\x86\xd8\xb5"); // نص
            std::string label = textProp ?
                (std::get_if<std::string>(&textProp->value) ?
                    *std::get_if<std::string>(&textProp->value) : "\xd8\xb1\xd9\x82\xd8\xa7\xd9\x82\xd8\xa9") : "\xd8\xb1\xd9\x82\xd8\xa7\xd9\x82\xd8\xa9";
            out << i << "<span class=\"sad-chip\"" << style << ">" << label << "</span>\n";
            break;
        }

        case UINodeType::Avatar: {
            const auto* textProp = node.findProperty("text");
            if (!textProp) textProp = node.findProperty("\xd8\xad\xd8\xb1\xd9\x81"); // حرف
            const auto* srcProp = node.findProperty("\xd9\x85\xd8\xb5\xd8\xaf\xd8\xb1"); // مصدر
            if (!srcProp) srcProp = node.findProperty("src");
            std::string initial = textProp ?
                (std::get_if<std::string>(&textProp->value) ?
                    *std::get_if<std::string>(&textProp->value) : "\xd9\x85") : "\xd9\x85";
            if (srcProp) {
                if (auto* s = std::get_if<std::string>(&srcProp->value)) {
                    out << i << "<img class=\"sad-avatar\" src=\"" << *s << "\" alt=\"" << initial << "\"" << style << " />\n";
                    break;
                }
            }
            out << i << "<div class=\"sad-avatar\"" << style << ">" << initial << "</div>\n";
            break;
        }

        case UINodeType::FAB: {
            const auto* textProp = node.findProperty("text");
            if (!textProp) textProp = node.findProperty("\xd9\x86\xd8\xb5"); // نص
            if (!textProp) textProp = node.findProperty("\xd8\xa7\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9"); // ايقونة
            std::string fabIcon = textProp ?
                (std::get_if<std::string>(&textProp->value) ?
                    *std::get_if<std::string>(&textProp->value) : "+") : "+";
            std::string onclick;
            for (const auto& evt : node.getEvents()) {
                if (evt.type == sad::ui::IREventType::OnTap) {
                    onclick = " onclick=\"" + evt.expression + "; update();\"";
                    break;
                }
            }
            out << i << "<button class=\"sad-fab\" aria-label=\"\xd8\xa5\xd8\xac\xd8\xb1\xd8\xa7\xd8\xa1 \xd8\xb3\xd8\xb1\xd9\x8a\xd8\xb9\"" << onclick << style << ">" << fabIcon << "</button>\n";
            break;
        }

        case UINodeType::SnackBar: {
            const auto* textProp = node.findProperty("text");
            if (!textProp) textProp = node.findProperty("\xd9\x86\xd8\xb5"); // نص
            if (!textProp) textProp = node.findProperty("\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89"); // محتوى
            std::string msg = textProp ?
                (std::get_if<std::string>(&textProp->value) ?
                    *std::get_if<std::string>(&textProp->value) : "\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84\xd8\xa9") : "\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84\xd8\xa9";
            out << i << "<div class=\"sad-snackbar\" role=\"status\" aria-live=\"polite\"" << style << ">" << msg << "</div>\n";
            break;
        }

        case UINodeType::AppBar: {
            const auto* textProp = node.findProperty("text");
            if (!textProp) textProp = node.findProperty("\xd9\x86\xd8\xb5"); // نص
            if (!textProp) textProp = node.findProperty("\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86"); // عنوان
            std::string title = textProp ?
                (std::get_if<std::string>(&textProp->value) ?
                    *std::get_if<std::string>(&textProp->value) : "\xd8\xa7\xd9\x84\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86") : "\xd8\xa7\xd9\x84\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86";
            out << i << "<header class=\"sad-appbar\" role=\"banner\"" << style << ">" << title << "</header>\n";
            break;
        }

        case UINodeType::Icon: {
            const auto* textProp = node.findProperty("text");
            if (!textProp) textProp = node.findProperty("\xd8\xa7\xd8\xb3\xd9\x85"); // اسم
            if (!textProp) textProp = node.findProperty("\xd9\x86\xd8\xb5"); // نص
            std::string iconName = textProp ?
                (std::get_if<std::string>(&textProp->value) ?
                    *std::get_if<std::string>(&textProp->value) : "\xe2\x98\x85") : "\xe2\x98\x85";
            out << i << "<span class=\"sad-icon\"" << style << ">" << iconName << "</span>\n";
            break;
        }

        case UINodeType::Canvas: {
            generateCanvas(out, node, indentLevel);
            break;
        }

        case UINodeType::Alert: {
            out << i << "<div class=\"sad-alert\" role=\"alert\" aria-live=\"assertive\"" << style << ">\n";
            out << i << "  <p>\xd8\xaa\xd9\x86\xd8\xa8\xd9\x8a\xd9\x87</p>\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "  <button class=\"sad-button\">\xd8\xad\xd8\xb3\xd9\x86\xd8\xa7\xd9\x8b</button>\n";
            out << i << "</div>\n";
            break;
        }

        case UINodeType::Dialog: {
            out << i << "<div class=\"sad-dialog\" role=\"dialog\" aria-modal=\"true\" aria-label=\"\xd8\xad\xd9\x88\xd8\xa7\xd8\xb1\"" << style << ">\n";
            out << i << "  <p>\xd8\xad\xd9\x88\xd8\xa7\xd8\xb1</p>\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "  <div class=\"sad-row\" style=\"gap: 8px; justify-content: flex-end;\">\n";
            out << i << "    <button class=\"sad-button\">\xd8\xa5\xd9\x84\xd8\xba\xd8\xa7\xd8\xa1</button>\n";
            out << i << "    <button class=\"sad-button\">\xd8\xaa\xd8\xa3\xd9\x83\xd9\x8a\xd8\xaf</button>\n";
            out << i << "  </div>\n";
            out << i << "</div>\n";
            break;
        }

        case UINodeType::Scaffold: {
            out << i << "<main class=\"sad-scaffold\" role=\"main\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</main>\n";
            break;
        }

        case UINodeType::Drawer: {
            out << i << "<aside class=\"sad-drawer\" role=\"navigation\" aria-label=\"\xd8\xa7\xd9\x84\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xac\xd8\xa7\xd9\x86\xd8\xa8\xd9\x8a\xd8\xa9\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</aside>\n";
            break;
        }

        case UINodeType::BottomSheet: {
            out << i << "<div class=\"sad-bottom-sheet\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</div>\n";
            break;
        }

        case UINodeType::TabView: {
            out << i << "<div class=\"sad-tabs\" role=\"tablist\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</div>\n";
            break;
        }

        case UINodeType::NavigationView: {
            out << i << "<nav class=\"sad-nav\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</nav>\n";
            break;
        }

        case UINodeType::LazyColumn: {
            out << i << "<div class=\"sad-lazy-column\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</div>\n";
            break;
        }

        case UINodeType::LazyRow: {
            out << i << "<div class=\"sad-lazy-row\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel + 1);
            }
            out << i << "</div>\n";
            break;
        }

        case UINodeType::List: {
            out << i << "<ul class=\"sad-list\"" << style << ">\n";
            for (const auto& child : node.getChildren()) {
                out << i << "  <li class=\"sad-list-item\">\n";
                out << generateNode(*child, indentLevel + 2);
                out << i << "  </li>\n";
            }
            out << i << "</ul>\n";
            break;
        }

        default:
            break;
    }
}

} // namespace web
} // namespace ui
} // namespace sad
