/**
 * ==========================================================================
 * ملف: sad_ui_ir_builder.hpp
 * الوصف: بنّاء SadUI IR — يحوّل UINode AST إلى SadUI IR الموحد
 * الإصدار: 1.0.0
 * ==========================================================================
 *
 * بسم الله الرحمن الرحيم
 *
 * هذا الملف مسؤول عن تحويل شجرة UINode (الناتجة من المحلل النحوي)
 * إلى تمثيل SadUI IR الموحد الذي يمكن ترجمته لأي منصة.
 *
 * التحويل يتضمن:
 * 1. ربط أنواع UINodeType → WidgetType
 * 2. حل المعدّلات (modifiers) → StyleIR
 * 3. استخراج الأحداث → EventHandlerIR
 * 4. ربط الحالة → StateBindingIR
 * 5. حل التنقل → NavigationActionIR
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 */

#ifndef SAD_UI_IR_BUILDER_HPP
#define SAD_UI_IR_BUILDER_HPP

#include "sad_ui_ir.hpp"
#include "../../shared/parser/include/ui_parser.hpp"
#include <string>
#include <vector>
#include <memory>

namespace sad {
namespace ui {
namespace ir {

// ═══════════════════════════════════════════════════════════════════════════
// إعدادات البنّاء
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief إعدادات بنّاء SadUI IR
 */
struct IRBuilderConfig {
    /// المنصة المستهدفة (تؤثر على التحقق من التوافق)
    TargetPlatform targetPlatform = TargetPlatform::All;

    /// هل نطبق الثيمة على الأنماط الافتراضية؟
    bool applyDefaultTheme = true;

    /// الثيمة الافتراضية
    ThemeIR defaultTheme;

    /// هل RTL افتراضي؟
    bool defaultRTL = true;

    /// هل نحل الألوان المسماة؟
    bool resolveNamedColors = true;

    /// هل نولد معرّفات فريدة تلقائيًا؟
    bool autoGenerateIds = true;

    /// الحد الأقصى لعمق الشجرة
    int maxDepth = 100;
};

// ═══════════════════════════════════════════════════════════════════════════
// تشخيصات البنّاء (أخطاء وتحذيرات)
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief مستوى التشخيص
 */
enum class DiagnosticLevel {
    Error,      ///< خطأ — يمنع التوليد
    Warning,    ///< تحذير — لا يمنع لكن يجب الانتباه
    Info        ///< معلومة — للتنبيه فقط
};

/**
 * @brief رسالة تشخيص
 */
struct IRDiagnostic {
    DiagnosticLevel level;
    std::string message;                ///< الرسالة (بالعربية)
    std::string messageEN;              ///< الرسالة (بالإنجليزية)
    size_t line = 0;
    size_t column = 0;
    std::string sourceFile;
};

// ═══════════════════════════════════════════════════════════════════════════
// نتيجة البناء
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief نتيجة بناء SadUI IR
 */
template<typename T>
struct IRBuildResult {
    std::optional<T> value;
    std::vector<IRDiagnostic> diagnostics;

    bool success() const {
        if (!value.has_value()) return false;
        for (const auto& d : diagnostics) {
            if (d.level == DiagnosticLevel::Error) return false;
        }
        return true;
    }

    bool hasErrors() const {
        for (const auto& d : diagnostics) {
            if (d.level == DiagnosticLevel::Error) return true;
        }
        return false;
    }

    bool hasWarnings() const {
        for (const auto& d : diagnostics) {
            if (d.level == DiagnosticLevel::Warning) return true;
        }
        return false;
    }

    std::vector<IRDiagnostic> errors() const {
        std::vector<IRDiagnostic> result;
        for (const auto& d : diagnostics) {
            if (d.level == DiagnosticLevel::Error) result.push_back(d);
        }
        return result;
    }
};

// ═══════════════════════════════════════════════════════════════════════════
// بنّاء SadUI IR
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief بنّاء SadUI IR
 *
 * يحوّل شجرة UINode (من المحلل) إلى SadUI IR الموحد.
 *
 * الاستخدام:
 * @code
 * // تحليل المصدر
 * UIParser parser;
 * auto parseResult = parser.parseViewDefinition(source);
 *
 * // بناء SadUI IR
 * IRBuilder builder(config);
 * auto irResult = builder.buildScreen(parseResult.value);
 *
 * if (irResult.success()) {
 *     ScreenIR screen = *irResult.value;
 *     // مرّر للمولد المناسب
 * }
 * @endcode
 */
class IRBuilder {
public:
    explicit IRBuilder(const IRBuilderConfig& config = IRBuilderConfig{});
    ~IRBuilder();

    // ═══════════════════════════════════════════
    // البناء الرئيسي
    // ═══════════════════════════════════════════

    /**
     * @brief بناء شاشة كاملة من تعريف واجهة
     * @param viewDef تعريف الواجهة (من UIParser)
     * @return نتيجة البناء مع ScreenIR
     */
    IRBuildResult<ScreenIR> buildScreen(
        const parser::ViewDefinition& viewDef
    );

    /**
     * @brief بناء شجرة عقد IR من شجرة UINode
     * @param uiNode عقدة UI (من UIParser)
     * @return نتيجة البناء مع عقدة IR
     */
    IRBuildResult<UINodeIRPtr> buildNode(
        const parser::UINodePtr& uiNode
    );

    /**
     * @brief بناء تطبيق كامل من عدة تعريفات واجهة ومعلومات المشروع
     * @param views تعريفات الواجهات
     * @param projectConfig إعدادات المشروع (من مشروع.صم)
     * @return نتيجة البناء مع AppIR
     */
    IRBuildResult<AppIR> buildApp(
        const std::vector<parser::ViewDefinition>& views,
        const std::unordered_map<std::string, std::string>& projectConfig
    );

    // ═══════════════════════════════════════════
    // الإعدادات
    // ═══════════════════════════════════════════

    /// تعيين الإعدادات
    void setConfig(const IRBuilderConfig& config);

    /// الحصول على الإعدادات
    const IRBuilderConfig& getConfig() const;

    /// تعيين الثيمة
    void setTheme(const ThemeIR& theme);

    // ═══════════════════════════════════════════
    // التشخيص
    // ═══════════════════════════════════════════

    /// الحصول على آخر التشخيصات
    const std::vector<IRDiagnostic>& getLastDiagnostics() const;

    /// مسح التشخيصات
    void clearDiagnostics();

private:
    IRBuilderConfig config_;
    std::vector<IRDiagnostic> diagnostics_;
    int nextId_ = 1;

    // ═══════════════════════════════════════════
    // تحويل داخلي
    // ═══════════════════════════════════════════

    /// تحويل UINodeType → WidgetType
    std::optional<WidgetType> mapNodeType(parser::UINodeType uiType);

    /// تحويل عقدة UINode → UINodeIR (تكراري)
    UINodeIRPtr convertNode(const parser::UINodePtr& node, int depth);

    /// تحويل المعدّلات → StyleIR
    StyleIR convertModifiers(const std::vector<parser::UIModifier>& modifiers);

    /// تحويل معدّل واحد → تحديث StyleIR
    void applyModifier(const parser::UIModifier& modifier, StyleIR& style);

    /// استخراج الأحداث من المعدّلات
    std::vector<EventHandlerIR> extractEvents(
        const std::vector<parser::UIModifier>& modifiers
    );

    /// تحويل ربط الحالة
    StateBindingIR convertStateBinding(const parser::StateBinding& binding);

    /// تحويل قيمة خاصية
    IRPropertyValue convertPropertyValue(const parser::PropertyValue& value);

    /// حل اللون المسمى
    std::optional<Color> resolveColor(const parser::ColorValue& colorVal);

    /// توليد معرّف فريد
    int generateId();

    /// إضافة تشخيص
    void addDiagnostic(DiagnosticLevel level,
                       const std::string& messageAR,
                       const std::string& messageEN,
                       size_t line = 0, size_t column = 0);
};

} // namespace ir
} // namespace ui
} // namespace sad

#endif // SAD_UI_IR_BUILDER_HPP
