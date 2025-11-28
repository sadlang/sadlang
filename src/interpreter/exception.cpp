/**
 * @file exception.cpp
 * @brief Implementation of Sad Language Exception System
 *        تنفيذ نظام الاستثناءات للغة ص
 * 
 * @author Sad Language Team
 * @date 2025-11-27
 * @version 1.0.0
 */

#include "interpreter/exception.h"
#include <sstream>

namespace Sad {
namespace Interpreter {

// ============================================================================
// SadException Implementation
// ============================================================================

SadException::SadException(const std::string& message, 
                           const std::string& type,
                           const Lexer::Position& position)
    : message_(message)
    , type_(type)
    , position_(position)
{
    // Constructor body intentionally empty
    // جسم المنشئ فارغ عمداً
}

void SadException::addStackFrame(const std::string& frame) {
    stackTrace_.push_back(frame);
}

std::string SadException::toString() const {
    std::ostringstream oss;
    
    // First line: ExceptionType: message
    // السطر الأول: نوع الاستثناء: الرسالة
    oss << type_ << ": " << message_;
    
    // Add position if available
    // إضافة الموضع إن كان متاحاً
    if (position_.line > 0) {
        oss << "\n  at " << position_.toString();
    }
    
    // Add stack trace
    // إضافة تتبع المكدس
    for (const auto& frame : stackTrace_) {
        oss << "\n  " << frame;
    }
    
    return oss.str();
}

const char* SadException::what() const noexcept {
    // std::exception::what() requires a const char* that remains valid
    // We use toString() and cache it (note: this is not thread-safe for modification)
    // 
    // std::exception::what() يتطلب const char* تبقى صالحة
    // نستخدم toString() ونخزنها مؤقتاً (ملاحظة: هذا ليس آمناً للخيوط عند التعديل)
    
    static std::string cached;
    cached = toString();
    return cached.c_str();
}

} // namespace Interpreter
} // namespace Sad
