/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: appkit_codegen.h
 * المسار: sad_ui/backends/macos/include/sad_ui/macos/appkit_codegen.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * الوصف:
 * ------
 * مولّد كود AppKit/Cocoa لمنصة macOS.
 *
 * يحوّل شجرة IR إلى كود Swift مع AppKit/Cocoa الذي يمكن بناؤه
 * وتشغيله كتطبيق macOS أصلي.
 *
 * استراتيجية التوليد:
 * ─────────────────
 * لكل عقدة IR، يولّد مكوّن AppKit أصلي:
 *
 *   نص("مرحباً")     →  NSTextField(labelWithString: "مرحباً")
 *   زر("اضغط")       →  NSButton(title: "اضغط", ...)
 *   عمود { ... }      →  NSStackView(orientation: .vertical)
 *   صف { ... }         →  NSStackView(orientation: .horizontal)
 *   صورة("logo.png")  →  NSImageView(image: NSImage(named: "logo.png"))
 *
 * البنية المولّدة:
 * ────────────────
 * لكل واجهة يُنتج:
 * 1. ملف Swift واحد (.swift)
 * 2. NSViewController subclass
 * 3. AppKit views مع Auto Layout
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_MACOS_APPKIT_CODEGEN_H
#define SAD_UI_MACOS_APPKIT_CODEGEN_H

#include "sad_ui/ir.h"
#include "sad_ui/types.h"

#include <string>
#include <memory>
#include <sstream>
#include <vector>

namespace sad {
namespace ui {
namespace macos {

// ═══════════════════════════════════════════════════════════════════════════════
// خيارات التوليد
// ═══════════════════════════════════════════════════════════════════════════════

struct AppKitCodegenOptions {
    std::string moduleName = "SadApp";           ///< اسم الوحدة
    bool useAutoLayout = true;                    ///< استخدام Auto Layout
    bool generateWindowController = true;         ///< إنشاء NSWindowController
    bool useArabicNames = true;                   ///< أسماء دوال عربية
    int indentSize = 4;                           ///< حجم المسافة البادئة
    std::string deploymentTarget = "13.0";        ///< إصدار macOS المستهدف
};

// ═══════════════════════════════════════════════════════════════════════════════
// مولّد AppKit
// ═══════════════════════════════════════════════════════════════════════════════

class AppKitCodegen {
public:
    explicit AppKitCodegen(const AppKitCodegenOptions& options = {});

    /// توليد كود Swift/AppKit من وحدة IR كاملة
    std::string generate(const IRModule& module);

    /// توليد كود لعقدة واحدة
    std::string generateNode(const IRNode& node, int indentLevel = 0);

private:
    AppKitCodegenOptions options_;

    // ─── توليد الأجزاء ──────────────────────────
    void generateImports(std::ostringstream& out);
    void generateViewController(std::ostringstream& out, const IRModule& module);
    void generateWindowController(std::ostringstream& out, const IRModule& module);
    void generateState(std::ostringstream& out,
                       const std::unordered_map<std::string, std::string>& states,
                       int indentLevel);

    void generateNativeWidget(std::ostringstream& out,
                              const IRNode& node, int indentLevel);
    void generateCanvasWidget(std::ostringstream& out,
                              const IRNode& node, int indentLevel);

    // ─── أدوات مساعدة ───────────────────────────
    std::string nodeTypeToAppKit(UINodeType type) const;
    std::string generateModifiers(const IRNode& node) const;
    std::string indent(int level) const;
    std::string colorToNSColor(const std::string& colorStr) const;
};

} // namespace macos
} // namespace ui
} // namespace sad

#endif // SAD_UI_MACOS_APPKIT_CODEGEN_H
