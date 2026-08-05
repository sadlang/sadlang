/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: html_style_codegen.cpp
 * المسار: features/graphics/backends/web/src/html_style_codegen.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * أدوات مساعدة: تحويل نوع العنصر إلى وسم HTML، توليد الأنماط المضمّنة، Canvas.
 * جزء من تقسيم html_codegen.cpp الأصلي.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/web/html_codegen.h"
#include "sad_ui/color_utils.h"
#include "sad_ui/prop_keys.h" // (AR) SoT مفاتيح الخصائص (props::PADDING = «حشوة»)
#include <optional>

namespace sad {
namespace ui {
namespace web {

// ─── Canvas ──────────────────────────────────

void HtmlCodegen::generateCanvas(
    std::ostringstream& out,
    const IRNode& node,
    int indentLevel
) {
    std::string i = ind(indentLevel);
    std::string id = node.getId().empty() ? "canvas_custom" : node.getId();

    out << i << "<canvas id=\"" << id << "\" width=\"300\" height=\"200\""
        << generateInlineStyle(node) << "></canvas>\n";
}

// ─── تحويل نوع العنصر إلى وسم HTML ─────────────

std::string HtmlCodegen::nodeTypeToHtmlTag(UINodeType type) const {
    switch (type) {
        case UINodeType::Text:       return "span";
        case UINodeType::Button:     return "button";
        case UINodeType::Image:      return "img";
        case UINodeType::TextField:  return "input";
        case UINodeType::TextArea:   return "textarea";
        case UINodeType::Divider:    return "hr";
        case UINodeType::Toggle:     return "label";
        case UINodeType::Slider:     return "input";
        case UINodeType::Checkbox:   return "label";
        case UINodeType::Radio:      return "label";
        case UINodeType::Picker:     return "select";
        case UINodeType::SearchBar:  return "input";
        case UINodeType::ProgressBar:return "progress";
        case UINodeType::Badge:      return "span";
        case UINodeType::Chip:       return "span";
        case UINodeType::FAB:        return "button";
        case UINodeType::AppBar:     return "header";
        case UINodeType::Icon:       return "span";
        case UINodeType::Canvas:     return "canvas";
        case UINodeType::List:       return "ul";
        case UINodeType::Avatar:     return "div";
        case UINodeType::SnackBar:   return "div";
        // ── 11. بيانات ──
        case UINodeType::Tooltip:    return "div";
        case UINodeType::DataTable:  return "table";
        case UINodeType::TreeView:   return "ul";
        case UINodeType::Breadcrumb: return "nav";
        case UINodeType::Pagination: return "nav";
        case UINodeType::Timeline:   return "div";
        // ── 12. وسائط ──
        case UINodeType::Carousel:   return "div";
        case UINodeType::RichText:   return "div";
        case UINodeType::Markdown:   return "div";
        case UINodeType::CodeBlock:  return "pre";
        case UINodeType::AudioPlayer:return "audio";
        case UINodeType::ImageGallery:return "div";
        // ── 13. حركة ──
        case UINodeType::Skeleton:   return "div";
        case UINodeType::Shimmer:    return "div";
        case UINodeType::AnimatedList:return "div";
        case UINodeType::Expandable: return "details";
        case UINodeType::Collapsible:return "details";
        case UINodeType::Swipeable:  return "div";
        // ── 14. تحكم ──
        case UINodeType::Stepper:    return "div";
        case UINodeType::SegmentedControl:return "div";
        case UINodeType::RatingBar:  return "div";
        case UINodeType::TimePicker: return "input";
        case UINodeType::Calendar:   return "input";
        case UINodeType::ColorWheel: return "input";
        // ── 15. تنقل ──
        case UINodeType::BottomNav:  return "nav";
        case UINodeType::SideNav:    return "nav";
        case UINodeType::Toolbar:    return "div";
        case UINodeType::SplitView:  return "div";
        case UINodeType::Tabs:       return "div";
        case UINodeType::MegaMenu:   return "nav";
        default:                     return "div";
    }
}

// ─── دالة مساعدة: قراءة القيمة العددية ──────────

// (AR) دالة مساعدة: قراءة القيمة العددية من خاصية IR سواء كانت double أو int64_t
// (EN) Helper: extract numeric value from IR property (handles both double and int64_t)
static std::optional<double> getNumericValue(const sad::ui::IRProperty& prop) {
    if (auto* d = std::get_if<double>(&prop.value)) return *d;
    if (auto* i = std::get_if<int64_t>(&prop.value)) return static_cast<double>(*i);
    return std::nullopt;
}

// ─── توليد الأنماط المضمّنة (Inline Styles) ─────

std::string HtmlCodegen::generateInlineStyle(const IRNode& node) const {
    if (!options_.inline_styles) return "";

    std::ostringstream style;
    bool hasStyle = false;

    for (const auto& prop : node.getProperties()) {
        if (prop.key == "\xd8\xad\xd8\xac\xd9\x85_\xd8\xae\xd8\xb7" || prop.key == "\xd8\xad\xd8\xac\xd9\x85_\xd8\xa7\xd9\x84\xd8\xae\xd8\xb7") { // حجم_خط أو حجم_الخط
            if (auto v = getNumericValue(prop)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                style << "font-size: " << *v << "px; ";
            }
        } else if (prop.key == props::PADDING) { // SoT «حشوة»
            if (auto v = getNumericValue(prop)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                style << "padding: " << *v << "px; ";
            }
        } else if (prop.key == "\xd8\xb9\xd8\xb1\xd8\xb6") { // عرض
            if (auto v = getNumericValue(prop)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                style << "width: " << *v << "px; ";
            }
        } else if (prop.key == "\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9") { // ارتفاع
            if (auto v = getNumericValue(prop)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                style << "height: " << *v << "px; ";
            }
        } else if (prop.key == "\xd9\x86\xd8\xb5\xd9\x81_\xd9\x82\xd8\xb7\xd8\xb1" || prop.key == "\xd8\xb2\xd9\x88\xd8\xa7\xd9\x8a\xd8\xa7") { // نصف_قطر أو زوايا
            if (auto v = getNumericValue(prop)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                style << "border-radius: " << *v << "px; ";
            }
        } else if (prop.key == "\xd9\x84\xd9\x88\xd9\x86_\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9" || prop.key == "\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9") { // لون_خلفية أو خلفية
            if (auto* v = std::get_if<std::string>(&prop.value)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                // Check for gradient
                std::string grad = arabicGradientToCSS(*v);
                if (!grad.empty()) {
                    style << "background: " << grad << "; ";
                } else {
                    style << "background-color: " << arabicColorToCSS(*v) << "; ";
                }
            }
        } else if (prop.key == "\xd9\x84\xd9\x88\xd9\x86" || prop.key == "\xd9\x84\xd9\x88\xd9\x86_\xd8\xa7\xd9\x84\xd9\x86\xd8\xb5" || prop.key == "\xd9\x84\xd9\x88\xd9\x86_\xd9\x86\xd8\xb5") { // لون أو لون_النص أو لون_نص
            if (auto* v = std::get_if<std::string>(&prop.value)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                style << "color: " << arabicColorToCSS(*v) << "; ";
            }
        } else if (prop.key == "\xd8\xb9\xd8\xaa\xd8\xa7\xd9\x85\xd8\xa9") { // عتامة
            if (auto v = getNumericValue(prop)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                style << "opacity: " << *v << "; ";
            }
        } else if (prop.key == "\xd9\x87\xd8\xa7\xd9\x85\xd8\xb4") { // هامش
            if (auto v = getNumericValue(prop)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                style << "margin: " << *v << "px; ";
            }
        } else if (prop.key == "\xd8\xb8\xd9\x84") { // ظل
            if (auto v = getNumericValue(prop)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                style << "box-shadow: 0 " << *v << "px " << (*v * 2) << "px rgba(0,0,0,0.2); ";
            }
        } else if (prop.key == "\xd8\xad\xd8\xaf_\xd9\x84\xd9\x88\xd9\x86") { // حد_لون
            if (auto* v = std::get_if<std::string>(&prop.value)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                style << "border-color: " << arabicColorToCSS(*v) << "; border-style: solid; ";
            }
        } else if (prop.key == "\xd8\xad\xd8\xaf_\xd8\xb3\xd9\x85\xd8\xa7\xd9\x83\xd8\xa9") { // حد_سماكة
            if (auto v = getNumericValue(prop)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                style << "border-width: " << *v << "px; ";
            }
        } else if (prop.key == "\xd9\x88\xd8\xb2\xd9\x86" || prop.key == "\xd9\x88\xd8\xb2\xd9\x86_\xd8\xa7\xd9\x84\xd8\xae\xd8\xb7") { // وزن أو وزن_الخط
            if (auto* v = std::get_if<std::string>(&prop.value)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                std::string fw = *v;
                if (fw == "\xd8\xb9\xd8\xb1\xd9\x8a\xd8\xb6" || fw == "عريض") fw = "bold"; // عريض → bold
                else if (fw == "\xd8\xb9\xd8\xa7\xd8\xaf\xd9\x8a" || fw == "عادي") fw = "normal";
                else if (fw == "\xd8\xae\xd9\x81\xd9\x8a\xd9\x81" || fw == "خفيف") fw = "300";
                style << "font-weight: " << fw << "; ";
            }
        } else if (prop.key == "\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xaf") { // تباعد
            if (auto v = getNumericValue(prop)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                style << "gap: " << *v << "px; ";
            }
        } else if (prop.key == "\xd9\x85\xd8\xad\xd8\xa7\xd8\xb0\xd8\xa7\xd8\xa9") { // محاذاة
            if (auto* v = std::get_if<std::string>(&prop.value)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                std::string align = *v;
                if (align == "\xd9\x88\xd8\xb3\xd8\xb7" || align == "وسط") align = "center";
                else if (align == "\xd9\x8a\xd9\x85\xd9\x8a\xd9\x86" || align == "يمين") align = "right";
                else if (align == "\xd9\x8a\xd8\xb3\xd8\xa7\xd8\xb1" || align == "يسار") align = "left";
                style << "text-align: " << align << "; ";
            }
        } else if (prop.key == "\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac" || prop.key == "gradient") { // تدرج
            if (auto* v = std::get_if<std::string>(&prop.value)) {
                std::string grad = arabicGradientToCSS(*v);
                if (!grad.empty()) {
                    if (!hasStyle) { style << " style=\""; hasStyle = true; }
                    style << "background: " << grad << "; ";
                }
            }
        } else if (prop.key == "\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac_\xd9\x86\xd8\xb5" || prop.key == "text-gradient") { // تدرج_نص
            if (auto* v = std::get_if<std::string>(&prop.value)) {
                std::string grad = arabicGradientToCSS(*v);
                if (!grad.empty()) {
                    if (!hasStyle) { style << " style=\""; hasStyle = true; }
                    style << "background: " << grad << "; -webkit-background-clip: text; "
                          << "-webkit-text-fill-color: transparent; background-clip: text; ";
                }
            }
        } else if (prop.key == "\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac_\xd8\xad\xd8\xaf" || prop.key == "border-gradient") { // تدرج_حد
            if (auto* v = std::get_if<std::string>(&prop.value)) {
                std::string grad = arabicGradientToCSS(*v);
                if (!grad.empty()) {
                    if (!hasStyle) { style << " style=\""; hasStyle = true; }
                    style << "border-image: " << grad << " 1; ";
                }
            }
        } else if (prop.key == "\xd8\xaa\xd8\xad\xd9\x88\xd9\x84" || prop.key == "transform") { // تحول
            if (auto* v = std::get_if<std::string>(&prop.value)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                style << "transform: " << *v << "; ";
            }
        } else if (prop.key == "\xd8\xa7\xd9\x86\xd8\xaa\xd9\x82\xd8\xa7\xd9\x84" || prop.key == "transition") { // انتقال
            if (auto* v = std::get_if<std::string>(&prop.value)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                style << "transition: " << *v << "; ";
            }
        } else if (prop.key == props::MAX_WIDTH || prop.key == "max-width") { // أقصى_عرض — من مصدر الحقيقة
            if (auto v = getNumericValue(prop)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                style << "max-width: " << *v << "px; ";
            }
        } else if (prop.key == props::MIN_WIDTH || prop.key == "min-width") { // أدنى_عرض — من مصدر الحقيقة
            if (auto v = getNumericValue(prop)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                style << "min-width: " << *v << "px; ";
            }
        } else if (prop.key == "\xd8\xb4\xd9\x81\xd8\xa7\xd9\x81\xd9\x8a\xd8\xa9_\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9" || prop.key == "backdrop-filter") { // شفافية_خلفية
            if (auto* v = std::get_if<std::string>(&prop.value)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                style << "backdrop-filter: " << *v << "; -webkit-backdrop-filter: " << *v << "; ";
            }
        } else if (prop.key == "\xd8\xaa\xd9\x85\xd9\x88\xd9\x8a\xd9\x87" || prop.key == "blur") { // تمويه
            if (auto v = getNumericValue(prop)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                style << "filter: blur(" << *v << "px); ";
            }
        } else if (prop.key == "\xd8\xaf\xd9\x88\xd8\xb1\xd8\xa7\xd9\x86" || prop.key == "rotation") { // دوران
            if (auto v = getNumericValue(prop)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                style << "transform: rotate(" << *v << "deg); ";
            }
        } else if (prop.key == "\xd9\x85\xd9\x82\xd9\x8a\xd8\xa7\xd8\xb3" || prop.key == "scale") { // مقياس
            if (auto v = getNumericValue(prop)) {
                if (!hasStyle) { style << " style=\""; hasStyle = true; }
                style << "transform: scale(" << *v << "); ";
            }
        }
    }

    if (hasStyle) {
        style << "\"";
    }

    return style.str();
}

} // namespace web
} // namespace ui
} // namespace sad
