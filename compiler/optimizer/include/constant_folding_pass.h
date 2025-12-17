/**
 * @file constant_folding_pass.h
 * @brief تمرير طي الثوابت
 * @brief Constant Folding Pass
 * 
 * @details
 * (AR) يقيّم العمليات على الثوابت في وقت الترجمة بدلاً من وقت التشغيل.
 *      مثال: 10 + 20 → 30, 5 * 6 → 30
 * 
 * (EN) Evaluates operations on constants at compile-time instead of runtime.
 *      Example: 10 + 20 → 30, 5 * 6 → 30
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#pragma once

#include "pass.h"
#include <optional>
#include <unordered_map>
#include <variant>

namespace Sad {
namespace Compiler {
namespace Optimizer {

/**
 * @brief قيمة ثابتة
 * @brief Constant Value
 */
using ConstantValue = std::variant<int64_t, double, bool, std::string>;

/**
 * @brief تمرير طي الثوابت
 * @brief Constant Folding Pass
 * 
 * @details
 * (AR) يحلل التعليمات ويحدد الثوابت، ثم يقيّم العمليات عليها.
 *      يستبدل العمليات الثابتة بنتائجها المحسوبة.
 * 
 * (EN) Analyzes instructions and identifies constants, then evaluates operations.
 *      Replaces constant operations with their computed results.
 * 
 * @example
 * Before:
 *   %0 = LOAD_CONST 10
 *   %1 = LOAD_CONST 20
 *   %2 = ADD %0, %1
 * 
 * After:
 *   %2 = LOAD_CONST 30
 */
class ConstantFoldingPass : public OptimizationPass {
public:
    /**
     * @brief Constructor / البناء
     */
    ConstantFoldingPass();
    
    /**
     * @brief تشغيل التمرير على دالة
     * @brief Run pass on function
     * 
     * @param function الدالة المراد تحسينها / Function to optimize
     * @return true إذا تم تعديل الدالة / if function was modified
     */
    bool runOnFunction(SIR::SIRFunction* function) override;

private:
    /**
     * @brief معالجة كتلة أساسية
     * @brief Process basic block
     */
    bool processBlock(SIR::SIRBasicBlock* block);
    
    /**
     * @brief طي عملية ثنائية على الأعداد الصحيحة
     * @brief Fold binary operation on integers
     * 
     * @param op رمز العملية / Operation code
     * @param left المعامل الأيسر / Left operand
     * @param right المعامل الأيمن / Right operand
     * @return النتيجة أو nullopt إذا لم يمكن الطي / Result or nullopt if cannot fold
     */
    std::optional<int64_t> foldIntBinary(
        SIR::SIROpcode op,
        int64_t left,
        int64_t right
    );
    
    /**
     * @brief طي عملية ثنائية على الأعداد العشرية
     * @brief Fold binary operation on floats
     */
    std::optional<double> foldFloatBinary(
        SIR::SIROpcode op,
        double left,
        double right
    );
    
    /**
     * @brief طي عملية مقارنة على الأعداد الصحيحة
     * @brief Fold comparison operation on integers
     */
    std::optional<bool> foldIntComparison(
        SIR::SIROpcode op,
        int64_t left,
        int64_t right
    );
    
    /**
     * @brief طي عملية أحادية
     * @brief Fold unary operation
     * 
     * @param op رمز العملية / Operation code
     * @param operand المعامل / Operand
     * @return النتيجة أو nullopt / Result or nullopt
     */
    std::optional<int64_t> foldIntUnary(
        SIR::SIROpcode op,
        int64_t operand
    );
    
    /**
     * @brief التحقق من كون السجل ثابتاً
     * @brief Check if register is constant
     * 
     * @param reg اسم السجل / Register name
     * @return true إذا كان ثابتاً / if constant
     */
    bool isConstant(const std::string& reg) const;
    
    /**
     * @brief الحصول على قيمة الثابت
     * @brief Get constant value
     * 
     * @param reg اسم السجل / Register name
     * @return قيمة الثابت أو nullopt / Constant value or nullopt
     */
    std::optional<ConstantValue> getConstantValue(const std::string& reg) const;
    
    /**
     * @brief تسجيل ثابت
     * @brief Record constant
     * 
     * @param reg اسم السجل / Register name
     * @param value القيمة / Value
     */
    void recordConstant(const std::string& reg, ConstantValue value);
    
    /**
     * @brief حذف ثابت (بعد إعادة تعريف السجل)
     * @brief Remove constant (after register redefinition)
     */
    void removeConstant(const std::string& reg);
    
    /**
     * @brief إعادة تعيين جدول الثوابت
     * @brief Reset constants table
     */
    void resetConstants();

private:
    // خريطة السجلات → القيم الثابتة / Register → Constant values map
    std::unordered_map<std::string, ConstantValue> constants_;
};

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
