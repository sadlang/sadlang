/**
 * ==========================================================================
 * ملف: web_html_codegen.hpp
 * الوصف: مولد كود الويب — SadUI IR → HTML/CSS/JavaScript
 * ==========================================================================
 * بسم الله الرحمن الرحيم
 *
 * يُولّد تطبيق ويب كامل من SadUI IR.
 */

#ifndef WEB_HTML_CODEGEN_HPP
#define WEB_HTML_CODEGEN_HPP

#include "sad_ui_codegen.hpp"
#include <sstream>

namespace sad {
namespace ui {
namespace ir {

/**
 * @brief إعدادات مولد الويب
 */
struct WebCodeGenConfig {
    /// استخدام CSS Framework (tailwind/none)
    std::string cssFramework = "none";

    /// توليد SPA (Single Page App)
    bool singlePageApp = true;

    /// تضمين CSS في HTML أم ملف منفصل
    bool inlineCSS = false;

    /// تضمين JS في HTML أم ملف منفصل
    bool inlineJS = false;

    /// استخدام CSS Variables للثيمة
    bool useCSSVariables = true;

    /// توليد responsive layout
    bool responsive = true;

    /// دعم PWA
    bool enablePWA = false;
};

/**
 * @brief مولد HTML/CSS/JavaScript
 *
 * يُولّد:
 * - HTML5 semantic pages
 * - CSS3 styles + CSS Variables
 * - Vanilla JS for state/events/navigation
 * - Responsive layout
 */
class WebHTMLCodeGen : public ICodeGenerator {
public:
    explicit WebHTMLCodeGen(const WebCodeGenConfig& config = {});
    ~WebHTMLCodeGen() override = default;

    CodeGenResult generate(const AppIR& app) override;
    CodeGenResult generateScreen(const ScreenIR& screen) override;
    TargetPlatform targetPlatform() const override { return TargetPlatform::Web; }
    std::string platformName() const override { return "Web (HTML/CSS/JS)"; }

private:
    WebCodeGenConfig config_;

    /// توليد HTML لشاشة
    std::string generateScreenHTML(const ScreenIR& screen, const ThemeIR& theme);

    /// توليد CSS عام
    std::string generateGlobalCSS(const AppIR& app);

    /// توليد CSS لشاشة
    std::string generateScreenCSS(const ScreenIR& screen, const ThemeIR& theme);

    /// توليد JS لشاشة
    std::string generateScreenJS(const ScreenIR& screen);

    /// توليد index.html رئيسي
    std::string generateIndexHTML(const AppIR& app);

    /// توليد router.js للتنقل
    std::string generateRouterJS(const AppIR& app);

    /// توليد عنصر HTML (تكراري)
    void emitHTML(std::ostringstream& ss, const UINodeIRPtr& node, int indent);

    /// توليد CSS لعقدة (تكراري)
    void emitCSS(std::ostringstream& ss, const UINodeIRPtr& node);

    /// توليد JS أحداث لعقدة
    void emitJS(std::ostringstream& ss, const UINodeIRPtr& node);

    /// تحويل StyleIR → CSS properties
    std::string styleToCSS(const StyleIR& style);

    /// تحويل لون
    std::string cssColor(const Color& c);

    /// معرّف فريد для العقدة
    std::string nodeId(const UINodeIRPtr& node);

    /// CSS class name
    std::string nodeClass(const UINodeIRPtr& node);

    /// مسافة بادئة
    std::string pad(int indent);

    int idCounter_ = 0;
};


} // namespace ir
} // namespace ui
} // namespace sad

#endif // WEB_HTML_CODEGEN_HPP
