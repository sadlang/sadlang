/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: pipeline.h
 * المسار: features/graphics/pipeline/include/sad_ui/pipeline.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * الوصف:
 * ------
 * خط أنابيب واجهات ص (SadUI Pipeline) — الواجهة العليا التي تربط
 * جميع مراحل معالجة واجهات المستخدم في سلسلة واحدة:
 *
 *   كود ص (.ص) → UIParser → UINode Tree → IRBuilder → IR → CodeGen → كود أصلي
 *
 * هذا الملف يوفر واجهة بسيطة وموحّدة لتحويل كود واجهات ص إلى
 * كود المنصة المستهدفة (Compose, SwiftUI, HTML, أو SDL2).
 *
 * الاستخدام:
 * ──────────
 *   UIPipeline pipeline;
 *   pipeline.setTarget(TargetPlatform::Android);
 *
 *   // من ملف .ص
 *   auto result = pipeline.processFile("واجهة.ص");
 *
 *   // أو من شجرة UINode مبنية برمجياً
 *   auto result = pipeline.processTree(viewDef);
 *
 * النتيجة:
 * ─────────
 *   result.success       // هل نجحت العملية
 *   result.generatedCode // الكود المولّد (Kotlin, Swift, HTML...)
 *   result.errors        // قائمة الأخطاء (إن وجدت)
 *   result.irModule      // التمثيل الوسيط (للفحص)
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_PIPELINE_H
#define SAD_UI_PIPELINE_H

#include "sad_ui/types.h"
#include "sad_ui/node.h"
#include "sad_ui/ir.h"
#include "sad_ui/ir_builder.h"
#include "sad_ui/style.h"
#include "sad_ui/layout.h"
#include "sad_ui/state.h"
#include "sad_ui/platform_renderer.h"

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <functional>

namespace sad {
namespace ui {

// ═══════════════════════════════════════════════════════════════════════════════
// وضع المعالجة (Render Mode)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief وضع المعالجة — توليد كود أو رسم مباشر
 */
enum class RenderMode : uint8_t {
    CodeGen,       ///< توليد كود (Kotlin, Swift, HTML, AppKit) — الطريقة القديمة
    DirectRender   ///< رسم مباشر على الشاشة — الطريقة الجديدة
};

// ═══════════════════════════════════════════════════════════════════════════════
// نتيجة المعالجة (Pipeline Result)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief نتيجة معالجة خط الأنابيب
 *
 * تحتوي على كل مخرجات المعالجة: الكود المولّد، الأخطاء،
 * والتمثيل الوسيط للفحص.
 */
struct PipelineResult {
    bool success = false;                            ///< هل نجحت المعالجة
    std::string generatedCode;                       ///< الكود المولّد للمنصة
    std::vector<std::string> errors;                 ///< قائمة الأخطاء
    std::vector<std::string> warnings;               ///< قائمة التحذيرات
    std::optional<IRModule> irModule;                ///< التمثيل الوسيط (للفحص)

    /// تنسيق الأخطاء كنص
    std::string formatErrors() const;
};

// ═══════════════════════════════════════════════════════════════════════════════
// إعدادات خط الأنابيب (Pipeline Options)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief إعدادات المعالجة
 */
struct PipelineOptions {
    TargetPlatform target = TargetPlatform::Desktop;  ///< المنصة المستهدفة
    RenderMode renderMode = RenderMode::CodeGen;      ///< وضع المعالجة
    bool optimize = true;                             ///< تفعيل التحسينات
    bool includeDebugInfo = false;                     ///< تضمين معلومات التصحيح
    bool enableSmartRouting = true;                   ///< تفعيل التوجيه الذكي
    bool resolveStyles = true;                        ///< تطبيق الأنماط
    bool calculateLayout = false;                     ///< حساب التخطيط

    /// سمة التطبيق
    std::optional<Theme> theme;

    /// اسم الحزمة (لأندرويد)
    std::string packageName = "com.sad.app";

    /// اسم الوحدة (لـ iOS)
    std::string moduleName = "SadApp";

    /// عنوان الصفحة (للويب)
    std::string pageTitle = "تطبيق ص";

    /// إعدادات النافذة (للرسم المباشر)
    PlatformWindowOptions windowOptions;

    /// إعدادات الرسم (للرسم المباشر)
    PlatformRenderOptions renderOptions;
};

// ═══════════════════════════════════════════════════════════════════════════════
// خط الأنابيب الرئيسي (UI Pipeline)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief خط أنابيب واجهات ص الرئيسي
 *
 * يربط جميع المكونات في سلسلة معالجة واحدة:
 * Parser → Nodes → IR → Style → Layout → CodeGen
 */
class UIPipeline {
public:
    UIPipeline();
    explicit UIPipeline(const PipelineOptions& options);

    // ── إعدادات ─────────────────────────────────────

    /// تعيين المنصة المستهدفة
    void setTarget(TargetPlatform target);

    /// تعيين الإعدادات
    void setOptions(const PipelineOptions& options);

    /// الحصول على الإعدادات الحالية
    const PipelineOptions& getOptions() const;

    // ── معالجة ──────────────────────────────────────

    /// معالجة شجرة عرض كاملة
    PipelineResult processTree(const ViewDefinition& viewDef);

    /// معالجة عقدة واحدة
    PipelineResult processNode(std::shared_ptr<UINode> root, const std::string& name = "");

    // ── الرسم المباشر (Direct Rendering) ────────

    /**
     * @brief عرض شجرة واجهة مباشرة على الشاشة
     *
     * بدلاً من توليد كود، ينشئ نافذة أصلية ويرسم عليها.
     * يحجب حتى إغلاق النافذة.
     *
     * @param viewDef    تعريف العرض
     * @return نتيجة المعالجة (بدون generatedCode)
     */
    PipelineResult renderDirectly(const ViewDefinition& viewDef);

    /**
     * @brief عرض عقدة واحدة مباشرة
     */
    PipelineResult renderDirectly(std::shared_ptr<UINode> root,
                                   const std::string& name = "");

    /**
     * @brief عرض IR جاهز مباشرة (بدون بناء من UINode)
     */
    PipelineResult renderIR(std::shared_ptr<IRNode> irRoot);

    /**
     * @brief الحصول على نافذة الرسم المباشر (لأغراض متقدمة)
     *
     * يجب استدعاء renderDirectly أو إنشاء النافذة أولاً.
     */
    PlatformWindow* getWindow() const { return window_.get(); }

    // ── أدوات مساعدة ─────────────────────────────

    /// طباعة الشجرة الوسيطة (للتصحيح)
    void dumpIR(const IRModule& module) const;

    /// التحقق من صحة الشجرة
    std::vector<std::string> validate(const ViewDefinition& viewDef) const;

private:
    PipelineOptions options_;                         ///< الإعدادات
    IRBuilder irBuilder_;                             ///< بنّاء التمثيل الوسيط
    StyleResolver styleResolver_;                     ///< محلل الأنماط
    LayoutEngine layoutEngine_;                       ///< محرك التخطيط

    /// توليد الكود للمنصة المستهدفة
    std::string generateCode(const IRModule& module);

    /// توليد كود Compose (أندرويد)
    std::string generateCompose(const IRModule& module);

    /// توليد كود SwiftUI (iOS)
    std::string generateSwiftUI(const IRModule& module);

    /// توليد كود HTML (ويب)
    std::string generateHtml(const IRModule& module);

    /// توليد كود AppKit (macOS)
    std::string generateAppKit(const IRModule& module);

    /// نافذة الرسم المباشر
    std::unique_ptr<PlatformWindow> window_;

    /// مخزن الحالة (للرسم المباشر)
    std::shared_ptr<StateStore> stateStore_;
};

} // namespace ui
} // namespace sad

#endif // SAD_UI_PIPELINE_H
