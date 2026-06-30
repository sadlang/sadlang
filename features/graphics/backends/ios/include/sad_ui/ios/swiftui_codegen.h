/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: swiftui_codegen.h
 * المسار: features/graphics/backends/ios/include/sad_ui/ios/swiftui_codegen.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * الوصف:
 * ------
 * مولّد كود SwiftUI لمنصة iOS / macOS.
 *
 * يحوّل شجرة IR إلى كود Swift/SwiftUI الذي يمكن بناؤه وتشغيله
 * على أجهزة Apple كتطبيق أصلي.
 *
 * استراتيجية التوليد (الهجين الذكي):
 * ─────────────────────────────────────
 * - Native → يولّد SwiftUI View أصلي:
 *   نص("مرحباً") → Text("مرحباً")
 *   زر("اضغط")   → Button("اضغط") { count += 1 }
 *
 * - Canvas → يولّد Canvas/Path drawing:
 *   عنصر_مخصص() → Canvas { context, size in context.fill(Path {...}) }
 *
 * البنية المولّدة:
 * ────────────────
 *   struct شاشة_رئيسية: View {
 *       @State private var عداد = 0
 *
 *       var body: some View {
 *           VStack {
 *               Text("العدد: \(عداد)")
 *                   .font(.title)
 *               Button("زد") { عداد += 1 }
 *           }
 *           .padding()
 *       }
 *   }
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_IOS_SWIFTUI_CODEGEN_H
#define SAD_UI_IOS_SWIFTUI_CODEGEN_H

#include "sad_ui/ir.h"
#include "sad_ui/types.h"

#include <string>
#include <memory>
#include <sstream>

namespace sad {
namespace ui {
namespace ios {

// ═══════════════════════════════════════════════════════════════════════════════
// خيارات التوليد (Codegen Options)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief خيارات التحكم في توليد كود SwiftUI
 */
struct SwiftUICodegenOptions {
    std::string moduleName = "SadApp";      ///< اسم الوحدة
    bool generatePreview = true;             ///< إنشاء #Preview
    bool useArabicNames = true;              ///< أسماء بنى عربية
    int indentSize = 4;                      ///< حجم المسافة البادئة
    std::string minimumIOSVersion = "17.0";  ///< أدنى إصدار iOS
};

// ═══════════════════════════════════════════════════════════════════════════════
// مولّد SwiftUI (SwiftUI Code Generator)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief يولّد كود Swift/SwiftUI من شجرة IR
 *
 * الاستخدام:
 * @code
 *   SwiftUICodegen codegen(options);
 *   std::string swiftCode = codegen.generate(irModule);
 * @endcode
 */
class SwiftUICodegen {
public:
    explicit SwiftUICodegen(const SwiftUICodegenOptions& options = {});

    /**
     * @brief توليد كود SwiftUI كامل من وحدة IR
     *
     * @param module وحدة IR المدخلة
     * @return كود Swift كنص
     */
    std::string generate(const IRModule& module);

    /**
     * @brief توليد كود لعقدة واحدة
     *
     * @param node عقدة IR
     * @param indent مستوى المسافة البادئة
     * @return كود SwiftUI للعقدة
     */
    std::string generateNode(const IRNode& node, int indent = 0);

private:
    // ─── توليد الأجزاء ──────────────────────

    /// توليد import statements
    void generateImports(std::ostringstream& out);

    /// توليد struct: View
    void generateStruct(std::ostringstream& out, const IRModule& module);

    /// توليد @State variables
    void generateStateVars(std::ostringstream& out,
                           const std::unordered_map<std::string, std::string>& states,
                           int indent);

    /// توليد عنصر أصلي (SwiftUI View)
    void generateNativeView(std::ostringstream& out,
                            const IRNode& node, int indent);

    /// توليد Canvas للرسم المخصص
    void generateCanvasView(std::ostringstream& out,
                            const IRNode& node, int indent);

    /// توليد سلسلة modifiers (.padding(), .font()...)
    void generateViewModifiers(std::ostringstream& out,
                               const IRNode& node, int indent);

    /// توليد #Preview
    void generatePreview(std::ostringstream& out, const std::string& structName);

    // ─── أدوات مساعدة ────────────────────────

    std::string ind(int level) const;
    std::string nodeTypeToSwiftUI(UINodeType type) const;

    // ─── البيانات ─────────────────────────────
    SwiftUICodegenOptions options_;
};

} // namespace ios
} // namespace ui
} // namespace sad

#endif // SAD_UI_IOS_SWIFTUI_CODEGEN_H
