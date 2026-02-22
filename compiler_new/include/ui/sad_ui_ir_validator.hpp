/**
 * ==========================================================================
 * ملف: sad_ui_ir_validator.hpp
 * الوصف: مُدقّق SadUI IR — يتحقق من صحة شجرة IR قبل توليد الكود
 * ==========================================================================
 * بسم الله الرحمن الرحيم
 */

#ifndef SAD_UI_IR_VALIDATOR_HPP
#define SAD_UI_IR_VALIDATOR_HPP

#include "sad_ui_ir.hpp"
#include <string>
#include <vector>

namespace sad {
namespace ui {
namespace ir {

// ═══════════════════════════════════════════════════════════════════════════
// نتيجة التحقق
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief مستوى خطورة مشكلة التحقق
 */
enum class ValidationLevel {
    Error,      ///< خطأ حقيقي — يمنع التوليد
    Warning,    ///< تحذير — قد يعمل لكن ربما يحتوي مشاكل
    Suggestion  ///< اقتراح تحسين
};

/**
 * @brief مشكلة تحقق واحدة
 */
struct ValidationIssue {
    ValidationLevel level;
    std::string messageAR;          ///< الرسالة بالعربية
    std::string messageEN;          ///< الرسالة بالإنجليزية
    std::string widgetPath;         ///< مسار المكون في الشجرة
    size_t sourceLine = 0;
    size_t sourceColumn = 0;
    int nodeId = 0;                 ///< معرّف العقدة المعنية
};

/**
 * @brief نتيجة التحقق الكاملة
 */
struct ValidationResult {
    std::vector<ValidationIssue> issues;

    bool isValid() const {
        for (const auto& issue : issues) {
            if (issue.level == ValidationLevel::Error) return false;
        }
        return true;
    }

    int errorCount() const {
        int count = 0;
        for (const auto& issue : issues) {
            if (issue.level == ValidationLevel::Error) count++;
        }
        return count;
    }

    int warningCount() const {
        int count = 0;
        for (const auto& issue : issues) {
            if (issue.level == ValidationLevel::Warning) count++;
        }
        return count;
    }

    std::vector<ValidationIssue> errors() const {
        std::vector<ValidationIssue> result;
        for (const auto& issue : issues) {
            if (issue.level == ValidationLevel::Error) result.push_back(issue);
        }
        return result;
    }
};


// ═══════════════════════════════════════════════════════════════════════════
// المُدقّق
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief مُدقّق SadUI IR
 *
 * يتحقق من:
 * - صحة البنية الشجرية (عقد فارغة، حاويات بدون أبناء، إلخ)
 * - توافق الأنواع (حقل_نص بدون حدث تغيير، إلخ)
 * - توافق المنصة (مكونات غير مدعومة في المنصة المستهدفة)
 * - اكتمال الأنماط (أزرار بدون نص، إلخ)
 * - حدود الأداء (شجرة عميقة جدًا، عقد كثيرة جدًا)
 */
class IRValidator {
public:
    explicit IRValidator(TargetPlatform platform = TargetPlatform::All);
    ~IRValidator() = default;

    /// تحقق من شاشة كاملة
    ValidationResult validate(const ScreenIR& screen);

    /// تحقق من عقدة وأبنائها
    ValidationResult validate(const UINodeIRPtr& node);

    /// تحقق من تطبيق كامل
    ValidationResult validate(const AppIR& app);

    /// تعيين المنصة المستهدفة
    void setTargetPlatform(TargetPlatform platform);

    /// الحد الأقصى لعمق الشجرة
    void setMaxDepth(int depth);

    /// الحد الأقصى لعدد العقد
    void setMaxNodes(int count);

private:
    TargetPlatform platform_;
    int maxDepth_ = 50;
    int maxNodes_ = 5000;

    std::vector<ValidationIssue> issues_;
    std::string currentPath_;

    /// التحقق من عقدة (تكراري)
    void validateNode(const UINodeIRPtr& node, int depth);

    /// التحقق من النمط
    void validateStyle(const StyleIR& style, WidgetType type, int nodeId);

    /// التحقق من الأحداث
    void validateEvents(const std::vector<EventHandlerIR>& events,
                        WidgetType type, int nodeId);

    /// التحقق من التوافق مع المنصة
    void validatePlatformCompat(WidgetType type, int nodeId);

    /// التحقق من أبناء المكون
    void validateChildren(const UINodeIRPtr& node);

    /// إضافة مشكلة
    void addIssue(ValidationLevel level,
                  const std::string& messageAR,
                  const std::string& messageEN,
                  int nodeId = 0,
                  size_t line = 0, size_t col = 0);
};


} // namespace ir
} // namespace ui
} // namespace sad

#endif // SAD_UI_IR_VALIDATOR_HPP
