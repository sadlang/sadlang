/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: compose_codegen.h
 * المسار: features/graphics/backends/android/include/sad_ui/android/compose_codegen.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * الوصف:
 * ------
 * مولّد كود Jetpack Compose لمنصة أندرويد.
 *
 * يحوّل شجرة IR إلى كود Kotlin/Compose الذي يمكن بناؤه وتشغيله
 * على أجهزة أندرويد كتطبيق أصلي.
 *
 * استراتيجية التوليد (الهجين الذكي):
 * ─────────────────────────────────────
 * لكل عقدة IR، يتحقق من RenderingHint:
 *
 * - Native → يولّد Composable أصلي:
 *   نص("مرحباً")  →  Text("مرحباً")
 *   زر("اضغط")    →  Button(onClick = { ... }) { Text("اضغط") }
 *
 * - Canvas → يولّد Canvas drawing:
 *   عنصر_مخصص()   →  Canvas(modifier = ...) { drawRect(...) }
 *
 * - Hybrid → يولّد Composable مع Canvas للأجزاء المخصصة
 *
 * البنية المولّدة:
 * ────────────────
 * لكل واجهة (View) يُنتج:
 * 1. ملف Kotlin واحد (.kt)
 * 2. @Composable function للواجهة
 * 3. State variables مع remember { mutableStateOf() }
 * 4. Material3 theme
 *
 * مثال المخرجات:
 * ──────────────
 * @code{.kt}
 *   @Composable
 *   fun شاشة_رئيسية() {
 *       var عداد by remember { mutableStateOf(0) }
 *
 *       Column(
 *           modifier = Modifier.fillMaxSize().padding(16.dp),
 *           horizontalAlignment = Alignment.CenterHorizontally
 *       ) {
 *           Text(
 *               text = "العدد: $عداد",
 *               fontSize = 24.sp,
 *               color = MaterialTheme.colorScheme.onSurface
 *           )
 *           Spacer(modifier = Modifier.height(8.dp))
 *           Button(onClick = { عداد++ }) {
 *               Text("زد")
 *           }
 *       }
 *   }
 * @endcode
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_ANDROID_COMPOSE_CODEGEN_H
#define SAD_UI_ANDROID_COMPOSE_CODEGEN_H

#include "sad_ui/ir.h"
#include "sad_ui/types.h"

#include <string>
#include <memory>
#include <sstream>
#include <vector>

namespace sad {
namespace ui {
namespace android {

// ═══════════════════════════════════════════════════════════════════════════════
// خيارات التوليد (Codegen Options)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief خيارات التحكم في توليد كود Compose
 */
struct ComposeCodegenOptions {
    std::string packageName = "com.sad.app";    ///< اسم الحزمة
    bool useMaterial3 = true;                    ///< استخدام Material 3
    bool generatePreview = true;                 ///< إنشاء @Preview
    bool useArabicNames = true;                  ///< أسماء دوال عربية
    int indentSize = 4;                          ///< حجم المسافة البادئة
    std::string targetSdkVersion = "34";         ///< إصدار SDK المستهدف
};

// ═══════════════════════════════════════════════════════════════════════════════
// مولّد Compose (Compose Code Generator)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief يولّد كود Kotlin/Compose من شجرة IR
 *
 * الاستخدام:
 * @code
 *   ComposeCodegen codegen(options);
 *   std::string kotlinCode = codegen.generate(irModule);
 *   // حفظ الكود في ملف .kt
 * @endcode
 */
class ComposeCodegen {
public:
    /// إنشاء المولّد بخيارات محددة
    explicit ComposeCodegen(const ComposeCodegenOptions& options = {});

    /**
     * @brief توليد كود Compose كامل من وحدة IR
     *
     * يُنتج ملف Kotlin كامل مع:
     * - import statements
     * - @Composable functions
     * - State management
     * - Theme integration
     *
     * @param module وحدة IR المدخلة
     * @return كود Kotlin كنص
     */
    std::string generate(const IRModule& module);

    /**
     * @brief توليد كود لعقدة واحدة (بدون imports)
     *
     * مفيد للاختبار والتصحيح.
     *
     * @param node عقدة IR
     * @param indent مستوى المسافة البادئة
     * @return كود Compose للعقدة
     */
    std::string generateNode(const IRNode& node, int indent = 0);

private:
    // ─── توليد الأجزاء ──────────────────────

    /// توليد سطور import
    void generateImports(std::ostringstream& out);

    /// توليد @Composable function
    void generateComposable(std::ostringstream& out, const IRModule& module);

    /// توليد متغيرات الحالة (remember { mutableStateOf() })
    void generateState(std::ostringstream& out,
                       const std::unordered_map<std::string, std::string>& states,
                       int indent);

    /// توليد عنصر أصلي (Native Composable)
    void generateNativeWidget(std::ostringstream& out,
                              const IRNode& node, int indent);

    /// توليد عنصر Canvas (رسم مخصص)
    void generateCanvasWidget(std::ostringstream& out,
                              const IRNode& node, int indent);

    /// توليد Modifier chain
    std::string generateModifiers(const IRNode& node);

    /// توليد @Preview
    void generatePreview(std::ostringstream& out, const std::string& funcName);

    // ─── أدوات مساعدة ────────────────────────

    /// مسافة بادئة
    std::string indent(int level) const;

    /// تحويل UINodeType إلى اسم Composable
    std::string nodeTypeToComposable(UINodeType type) const;

    // ─── البيانات ─────────────────────────────

    ComposeCodegenOptions options_;   ///< خيارات التوليد
};

} // namespace android
} // namespace ui
} // namespace sad

#endif // SAD_UI_ANDROID_COMPOSE_CODEGEN_H
