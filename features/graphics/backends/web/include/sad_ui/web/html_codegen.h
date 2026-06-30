/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: html_codegen.h
 * المسار: features/graphics/backends/web/include/sad_ui/web/html_codegen.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * الوصف:
 * ------
 * مولّد كود HTML5/CSS3/JavaScript للويب.
 *
 * يحوّل شجرة IR إلى صفحة ويب كاملة يمكن عرضها في المتصفح.
 * يدعم:
 * - HTML5 semantic elements
 * - CSS3 Flexbox layout
 * - Canvas API للرسم المخصص
 * - JavaScript للتفاعل وإدارة الحالة
 * - RTL layout تلقائي
 *
 * البنية المولّدة:
 * ────────────────
 * ملف HTML واحد يحتوي على:
 * - <style> بداخله CSS Flexbox
 * - <div> متداخلة تمثل العناصر
 * - <canvas> للرسم المخصص
 * - <script> لإدارة الحالة والأحداث
 *
 * مثال المخرجات:
 * ──────────────
 * @code{.html}
 *   <!DOCTYPE html>
 *   <html dir="rtl" lang="ar">
 *   <head>
 *       <meta charset="UTF-8">
 *       <style>
 *           .sad-column { display: flex; flex-direction: column; }
 *           .sad-row    { display: flex; flex-direction: row-reverse; }
 *       </style>
 *   </head>
 *   <body>
 *       <div class="sad-column" style="padding: 16px;">
 *           <span style="font-size: 24px;">مرحباً</span>
 *           <button onclick="عداد++; update();">اضغط</button>
 *       </div>
 *       <script>
 *           let عداد = 0;
 *           function update() { ... }
 *       </script>
 *   </body>
 *   </html>
 * @endcode
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_WEB_HTML_CODEGEN_H
#define SAD_UI_WEB_HTML_CODEGEN_H

#include "sad_ui/ir.h"
#include "sad_ui/types.h"

#include <string>
#include <sstream>
#include <memory>

namespace sad {
namespace ui {
namespace web {

// ═══════════════════════════════════════════════════════════════════════════════
// خيارات التوليد
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief خيارات توليد HTML
 */
struct HtmlCodegenOptions {
    std::string title = "تطبيق ص";       ///< عنوان الصفحة
    std::string lang = "ar";              ///< لغة الصفحة
    std::string dir = "rtl";              ///< اتجاه النص
    bool inline_styles = true;            ///< أنماط مضمّنة أم CSS خارجي
    bool include_reset_css = true;        ///< إضافة CSS Reset
    int indentSize = 2;                   ///< حجم المسافة البادئة
};

// ═══════════════════════════════════════════════════════════════════════════════
// مولّد HTML
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief يولّد كود HTML/CSS/JS من شجرة IR
 */
class HtmlCodegen {
public:
    explicit HtmlCodegen(const HtmlCodegenOptions& options = {});

    /**
     * @brief توليد صفحة HTML كاملة
     *
     * @param module وحدة IR المدخلة
     * @return كود HTML كنص
     */
    std::string generate(const IRModule& module);

    /**
     * @brief توليد HTML لعقدة واحدة (بدون head/body)
     *
     * @param node عقدة IR
     * @param indent مستوى المسافة البادئة
     * @return كود HTML للعقدة
     */
    std::string generateNode(const IRNode& node, int indent = 0);

private:
    /// توليد رأس الصفحة (<head>)
    void generateHead(std::ostringstream& out);

    /// توليد CSS الأساسي
    void generateBaseCSS(std::ostringstream& out);

    /// توليد عنصر HTML مع الأنماط — يوزّع على الدوال الفرعية
    void generateElement(std::ostringstream& out,
                         const IRNode& node, int indent);

    /// توليد العناصر الأساسية (نص، زر، صورة، حقول إدخال، ...)
    void generateBasicElement(std::ostringstream& out,
                              const IRNode& node, int indent);

    /// توليد عناصر البيانات والوسائط والحركة (جدول، شجرة، ...)
    void generateAdvancedElement(std::ostringstream& out,
                                const IRNode& node, int indent);

    /// توليد عناصر التنقل والحاويات (تبويبات، درج، ...)
    void generateNavElement(std::ostringstream& out,
                            const IRNode& node, int indent);

    /// توليد Canvas للرسم المخصص
    void generateCanvas(std::ostringstream& out,
                        const IRNode& node, int indent);

    /// توليد JavaScript للحالة والأحداث
    void generateScript(std::ostringstream& out, const IRModule& module);

    /// تحويل UINodeType إلى عنصر HTML
    std::string nodeTypeToHtmlTag(UINodeType type) const;

    /// توليد سلسلة style=""
    std::string generateInlineStyle(const IRNode& node) const;

    /// مسافة بادئة
    std::string ind(int level) const;

    HtmlCodegenOptions options_;
};

} // namespace web
} // namespace ui
} // namespace sad

#endif // SAD_UI_WEB_HTML_CODEGEN_H
