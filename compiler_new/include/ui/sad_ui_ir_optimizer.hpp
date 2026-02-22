/**
 * ==========================================================================
 * ملف: sad_ui_ir_optimizer.hpp
 * الوصف: مُحسّن SadUI IR — تحسين شجرة IR قبل توليد الكود
 * ==========================================================================
 * بسم الله الرحمن الرحيم
 */

#ifndef SAD_UI_IR_OPTIMIZER_HPP
#define SAD_UI_IR_OPTIMIZER_HPP

#include "sad_ui_ir.hpp"
#include <string>
#include <vector>

namespace sad {
namespace ui {
namespace ir {

/**
 * @brief إحصائيات التحسين
 */
struct OptimizationStats {
    int removedNodes = 0;       ///< عقد تم حذفها
    int mergedStyles = 0;       ///< أنماط تم دمجها
    int flattenedGroups = 0;    ///< مجموعات تم تسطيحها
    int simplifiedNesting = 0;  ///< تداخل تم تبسيطه
};

/**
 * @brief مُحسّن SadUI IR
 *
 * يُجري تحسينات على شجرة IR لتقليل التعقيد:
 * - إزالة الحاويات الفارغة
 * - تسطيح المجموعات المتداخلة بلا فائدة
 * - دمج الأنماط المتكررة
 * - حل الأنماط من الثيمة
 * - إزالة العقد المخفية
 */
class IROptimizer {
public:
    IROptimizer() = default;
    ~IROptimizer() = default;

    /**
     * @brief تحسين شاشة كاملة
     */
    void optimize(ScreenIR& screen);

    /**
     * @brief تحسين عقدة وأبنائها
     */
    UINodeIRPtr optimize(UINodeIRPtr node);

    /**
     * @brief تحسين تطبيق كامل
     */
    void optimize(AppIR& app);

    /**
     * @brief تطبيق ثيمة على الأنماط الافتراضية
     */
    void applyTheme(UINodeIRPtr node, const ThemeIR& theme);

    /**
     * @brief الحصول على إحصائيات آخر تحسين
     */
    const OptimizationStats& getStats() const { return stats_; }

    /// هل يجب إزالة العقد المخفية؟
    void setRemoveHidden(bool v) { removeHidden_ = v; }

    /// هل يجب تسطيح المجموعات المتداخلة؟
    void setFlattenGroups(bool v) { flattenGroups_ = v; }

private:
    OptimizationStats stats_;
    bool removeHidden_ = true;
    bool flattenGroups_ = true;

    /// إزالة الحاويات الفارغة
    UINodeIRPtr removeEmptyContainers(UINodeIRPtr node);

    /// تسطيح المجموعات المتداخلة
    UINodeIRPtr flattenNestedGroups(UINodeIRPtr node);

    /// دمج حاوية ابن وحيد مع الأب
    UINodeIRPtr simplifySingleChild(UINodeIRPtr node);

    /// إزالة العقد المخفية
    UINodeIRPtr removeHiddenNodes(UINodeIRPtr node);

    /// تطبيق الثيمة (تكراري)
    void applyThemeRecursive(UINodeIRPtr node, const ThemeIR& theme);
};


} // namespace ir
} // namespace ui
} // namespace sad

#endif // SAD_UI_IR_OPTIMIZER_HPP
