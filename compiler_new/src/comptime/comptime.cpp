// ======================================================================
// comptime.cpp - تنفيذ تقييم وقت الترجمة / Compile-Time Evaluation Impl
// ======================================================================

#include "comptime/comptime.h"
#include <cmath>
#include <stdexcept>

namespace Sad {
namespace Comptime {

std::optional<ComptimeValue> ComptimeContext::evalBinaryOp(
    const ComptimeValue& left, const std::string& op, const ComptimeValue& right) const {
    
    // (AR) عمليات على الأعداد الصحيحة
    if (std::holds_alternative<int64_t>(left) && std::holds_alternative<int64_t>(right)) {
        int64_t a = std::get<int64_t>(left);
        int64_t b = std::get<int64_t>(right);
        
        if (op == "+" || op == "جمع") return ComptimeValue{a + b};
        if (op == "-" || op == "طرح") return ComptimeValue{a - b};
        if (op == "*" || op == "ضرب") return ComptimeValue{a * b};
        if (op == "/" || op == "قسمة") {
            if (b == 0) return std::nullopt; // (AR) خطأ: قسمة على صفر
            return ComptimeValue{a / b};
        }
        if (op == "%" || op == "باقي") {
            if (b == 0) return std::nullopt;
            return ComptimeValue{a % b};
        }
        if (op == "**" || op == "أس") return ComptimeValue{(int64_t)std::pow(a, b)};
        if (op == "&" || op == "و_ثنائي") return ComptimeValue{a & b};
        if (op == "|" || op == "أو_ثنائي") return ComptimeValue{a | b};
        if (op == "^" || op == "حصري") return ComptimeValue{a ^ b};
        if (op == "<<" || op == "إزاحة_يسار") return ComptimeValue{a << b};
        if (op == ">>" || op == "إزاحة_يمين") return ComptimeValue{a >> b};
        
        // (AR) مقارنات
        if (op == "==" || op == "يساوي") return ComptimeValue{a == b};
        if (op == "!=" || op == "لا_يساوي") return ComptimeValue{a != b};
        if (op == "<" || op == "أصغر") return ComptimeValue{a < b};
        if (op == ">" || op == "أكبر") return ComptimeValue{a > b};
        if (op == "<=" || op == "أصغر_يساوي") return ComptimeValue{a <= b};
        if (op == ">=" || op == "أكبر_يساوي") return ComptimeValue{a >= b};
    }
    
    // (AR) عمليات على الأعداد العشرية
    if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
        double a = std::get<double>(left);
        double b = std::get<double>(right);
        
        if (op == "+" || op == "جمع") return ComptimeValue{a + b};
        if (op == "-" || op == "طرح") return ComptimeValue{a - b};
        if (op == "*" || op == "ضرب") return ComptimeValue{a * b};
        if (op == "/" || op == "قسمة") {
            if (b == 0.0) return std::nullopt;
            return ComptimeValue{a / b};
        }
        if (op == "**" || op == "أس") return ComptimeValue{std::pow(a, b)};
        
        if (op == "==" || op == "يساوي") return ComptimeValue{a == b};
        if (op == "!=" || op == "لا_يساوي") return ComptimeValue{a != b};
        if (op == "<" || op == "أصغر") return ComptimeValue{a < b};
        if (op == ">" || op == "أكبر") return ComptimeValue{a > b};
    }
    
    // (AR) عمليات على النصوص
    if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
        const auto& a = std::get<std::string>(left);
        const auto& b = std::get<std::string>(right);
        
        if (op == "+" || op == "جمع") return ComptimeValue{a + b};
        if (op == "==" || op == "يساوي") return ComptimeValue{a == b};
        if (op == "!=" || op == "لا_يساوي") return ComptimeValue{a != b};
    }
    
    // (AR) عمليات منطقية
    if (std::holds_alternative<bool>(left) && std::holds_alternative<bool>(right)) {
        bool a = std::get<bool>(left);
        bool b = std::get<bool>(right);
        
        if (op == "&&" || op == "و") return ComptimeValue{a && b};
        if (op == "||" || op == "أو") return ComptimeValue{a || b};
        if (op == "==" || op == "يساوي") return ComptimeValue{a == b};
        if (op == "!=" || op == "لا_يساوي") return ComptimeValue{a != b};
    }
    
    // (AR) عملية مختلطة: رقم صحيح مع عشري — ترقية لعشري
    if (std::holds_alternative<int64_t>(left) && std::holds_alternative<double>(right)) {
        double a = static_cast<double>(std::get<int64_t>(left));
        double b = std::get<double>(right);
        return evalBinaryOp(ComptimeValue{a}, op, ComptimeValue{b});
    }
    if (std::holds_alternative<double>(left) && std::holds_alternative<int64_t>(right)) {
        double a = std::get<double>(left);
        double b = static_cast<double>(std::get<int64_t>(right));
        return evalBinaryOp(ComptimeValue{a}, op, ComptimeValue{b});
    }
    
    return std::nullopt;
}

std::optional<ComptimeValue> ComptimeContext::evalUnaryOp(
    const std::string& op, const ComptimeValue& operand) const {
    
    if (std::holds_alternative<int64_t>(operand)) {
        int64_t a = std::get<int64_t>(operand);
        if (op == "-" || op == "سالب") return ComptimeValue{-a};
        if (op == "~" || op == "عكس") return ComptimeValue{~a};
    }
    
    if (std::holds_alternative<double>(operand)) {
        double a = std::get<double>(operand);
        if (op == "-" || op == "سالب") return ComptimeValue{-a};
    }
    
    if (std::holds_alternative<bool>(operand)) {
        bool a = std::get<bool>(operand);
        if (op == "!" || op == "ليس") return ComptimeValue{!a};
    }
    
    return std::nullopt;
}

} // namespace Comptime
} // namespace Sad
