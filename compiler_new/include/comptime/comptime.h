// ======================================================================
// comptime.h - تقييم وقت الترجمة / Compile-Time Evaluation
// ======================================================================
// الوصف بالعربية:
//   نظام تقييم التعابير في وقت الترجمة (مثل comptime في Zig)
//   يسمح بحساب القيم والأنواع أثناء الترجمة لتحسين أداء وقت التشغيل
//
// English Description:
//   Compile-time expression evaluation system (like Zig's comptime)
//   Allows computing values and types at compile time for runtime optimization
// ======================================================================

#ifndef SAD_COMPTIME_H
#define SAD_COMPTIME_H

#include <string>
#include <variant>
#include <vector>
#include <optional>
#include <unordered_map>
#include <cstdint>

namespace Sad {
namespace Comptime {

/**
 * @brief Compile-time value / قيمة وقت الترجمة
 * 
 * (AR) يمكن أن تكون: عدد صحيح، عشري، نص، منطقي، أو لاشيء
 * (EN) Can be: integer, float, string, bool, or none
 */
using ComptimeValue = std::variant<
    int64_t,        // عدد صحيح / integer
    double,         // عشري / float
    std::string,    // نص / string
    bool,           // منطقي / bool
    std::monostate  // لاشيء / none
>;

/**
 * @brief Compile-time evaluation context / سياق تقييم وقت الترجمة
 * 
 * (AR) يحتفظ بالمتغيرات والدوال المعروفة في وقت الترجمة
 * (EN) Holds variables and functions known at compile time
 */
class ComptimeContext {
public:
    ComptimeContext() = default;
    
    /**
     * @brief Set a comptime variable / تعيين متغير وقت الترجمة
     */
    void setVar(const std::string& name, ComptimeValue value) {
        variables_[name] = std::move(value);
    }
    
    /**
     * @brief Get a comptime variable / الحصول على متغير وقت الترجمة
     */
    std::optional<ComptimeValue> getVar(const std::string& name) const {
        auto it = variables_.find(name);
        if (it != variables_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    /**
     * @brief Check if variable exists / التحقق من وجود متغير
     */
    bool hasVar(const std::string& name) const {
        return variables_.find(name) != variables_.end();
    }
    
    /**
     * @brief Evaluate binary operation / تقييم عملية ثنائية
     */
    std::optional<ComptimeValue> evalBinaryOp(
        const ComptimeValue& left, const std::string& op, const ComptimeValue& right) const;
    
    /**
     * @brief Evaluate unary operation / تقييم عملية أحادية
     */
    std::optional<ComptimeValue> evalUnaryOp(
        const std::string& op, const ComptimeValue& operand) const;
    
    /**
     * @brief Check if expression is comptime-evaluable / التحقق من قابلية التقييم
     */
    bool isComptimeExpr(const std::string& exprType) const {
        // (AR) العمليات الحسابية والمقارنات والمنطقية قابلة للتقييم
        return exprType == "binary" || exprType == "unary" || 
               exprType == "literal" || exprType == "variable";
    }
    
private:
    std::unordered_map<std::string, ComptimeValue> variables_;
};

/**
 * @brief Comptime evaluator / مقيّم وقت الترجمة
 * 
 * (AR) يقيّم التعابير المعلّمة بـ 'وقت_ترجمة' أثناء الترجمة
 * (EN) Evaluates expressions marked with 'comptime' during compilation
 * 
 * @example أمثلة:
 * - ثابت الحجم = وقت_ترجمة { 1024 * 1024 }  // = 1048576
 * - وقت_ترجمة { إذا حجم(رقم) == 8 { ... } }
 * - const SIZE = comptime { 1024 * 1024 }
 */
class ComptimeEvaluator {
public:
    ComptimeEvaluator() = default;
    
    /**
     * @brief Get context / الحصول على السياق
     */
    ComptimeContext& getContext() { return context_; }
    const ComptimeContext& getContext() const { return context_; }
    
    /**
     * @brief Convert value to string for embedding in code
     * (AR) تحويل القيمة لنص لتضمينها في الكود
     */
    static std::string valueToString(const ComptimeValue& value) {
        if (std::holds_alternative<int64_t>(value)) {
            return std::to_string(std::get<int64_t>(value));
        }
        if (std::holds_alternative<double>(value)) {
            return std::to_string(std::get<double>(value));
        }
        if (std::holds_alternative<std::string>(value)) {
            return "\"" + std::get<std::string>(value) + "\"";
        }
        if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value) ? "صحيح" : "خطأ";
        }
        return "لاشيء";
    }
    
    /**
     * @brief Check if a value is truthy / التحقق من صحة القيمة
     */
    static bool isTruthy(const ComptimeValue& value) {
        if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value) != 0;
        }
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value) != 0.0;
        }
        if (std::holds_alternative<std::string>(value)) {
            return !std::get<std::string>(value).empty();
        }
        if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value);
        }
        return false;
    }
    
private:
    ComptimeContext context_;
};

} // namespace Comptime
} // namespace Sad

#endif // SAD_COMPTIME_H
