/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: html_codegen.cpp
 * المسار: features/graphics/backends/web/src/html_codegen.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * نقطة الدخول الرئيسية لمولّد كود HTML5/CSS3/JavaScript.
 * يحتوي فقط على: constructor, generate(), generateNode(), generateHead(), ind().
 *
 * الأجزاء المقسّمة:
 *   - html_css_codegen.cpp         → generateBaseCSS()
 *   - html_element_codegen.cpp     → generateElement(), generateBasicElement()
 *   - html_element_advanced_codegen.cpp → generateAdvancedElement(), generateNavElement()
 *   - html_script_codegen.cpp      → generateScript()
 *   - html_style_codegen.cpp       → generateCanvas(), nodeTypeToHtmlTag(),
 *                                     generateInlineStyle()
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/web/html_codegen.h"
#include "sad_ui/color_utils.h"  // أدوات الألوان

namespace sad {
namespace ui {
namespace web {

HtmlCodegen::HtmlCodegen(const HtmlCodegenOptions& options)
    : options_(options)
{
}

std::string HtmlCodegen::generate(const IRModule& module) {
    std::ostringstream out;

    out << "<!DOCTYPE html>\n"
        << "<html dir=\"" << options_.dir << "\" lang=\"" << options_.lang << "\">\n";

    generateHead(out);

    out << "<body>\n"
        << "  <a href=\"#main-content\" class=\"sad-skip-link\">تخطي إلى المحتوى الرئيسي</a>\n"
        << "  <div id=\"main-content\">\n";

    if (module.root) {
        out << generateNode(*module.root, 1);
    }

    out << "  </div>\n";

    // (AR) زر الرجوع لأعلى الصفحة — يظهر عند التمرير
    out << "  <button id=\"sad-scroll-top\" aria-label=\"العودة لأعلى\" "
        << "style=\"position:fixed;bottom:24px;right:24px;width:48px;height:48px;border-radius:50%;"
        << "background:var(--sad-primary);color:white;border:none;font-size:20px;cursor:pointer;"
        << "box-shadow:0 4px 8px rgba(0,0,0,0.2);opacity:0;visibility:hidden;"
        << "transition:opacity 0.3s,visibility 0.3s;z-index:999;display:flex;align-items:center;"
        << "justify-content:center;\">&#x2191;</button>\n";

    generateScript(out, module);

    out << "</body>\n"
        << "</html>\n";

    return out.str();
}

std::string HtmlCodegen::generateNode(const IRNode& node, int indentLevel) {
    std::ostringstream out;

    if (node.getRenderingHint() == RenderingHint::Canvas) {
        generateCanvas(out, node, indentLevel);
    } else {
        generateElement(out, node, indentLevel);
    }

    return out.str();
}

// ─── توليد الأجزاء ───────────────────────

void HtmlCodegen::generateHead(std::ostringstream& out) {
    out << "<head>\n"
        << ind(1) << "<meta charset=\"UTF-8\">\n"
        << ind(1) << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        << ind(1) << "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n"
        << ind(1) << "<meta name=\"color-scheme\" content=\"light dark\">\n"
        // (AR) PWA meta tags لتحسين التطبيق على الجوال
        << ind(1) << "<meta name=\"mobile-web-app-capable\" content=\"yes\">\n"
        << ind(1) << "<meta name=\"apple-mobile-web-app-capable\" content=\"yes\">\n"
        << ind(1) << "<meta name=\"apple-mobile-web-app-status-bar-style\" content=\"default\">\n"
        << ind(1) << "<meta name=\"theme-color\" content=\"#1E88E5\" media=\"(prefers-color-scheme: light)\">\n"
        << ind(1) << "<meta name=\"theme-color\" content=\"#121212\" media=\"(prefers-color-scheme: dark)\">\n"
        // (AR) SEO meta tags قابلة للتخصيص
        << ind(1) << "<meta name=\"description\" content=\"" << options_.title << "\">\n"
        << ind(1) << "<meta name=\"generator\" content=\"Sad Language UI (لغة ص)\">\n"
        << ind(1) << "<meta property=\"og:title\" content=\"" << options_.title << "\">\n"
        << ind(1) << "<meta property=\"og:type\" content=\"website\">\n"
        << ind(1) << "<meta property=\"og:locale\" content=\"" << options_.lang << "\">\n"
        << ind(1) << "<title>" << options_.title << "</title>\n"
        << ind(1) << "<link href=\"https://fonts.googleapis.com/css2?family=Cairo:wght@300;400;600;700&display=swap\" rel=\"stylesheet\">\n"
        << ind(1) << "<link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">\n"
        << ind(1) << "<link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin>\n";

    // CSS الأساسي
    out << ind(1) << "<style>\n";
    generateBaseCSS(out);
    out << ind(1) << "</style>\n";

    out << "</head>\n";
}

// ─── المسافة البادئة ─────────────────────────

std::string HtmlCodegen::ind(int level) const {
    return std::string(level * options_.indentSize, ' ');
}

} // namespace web
} // namespace ui
} // namespace sad
