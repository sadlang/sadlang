// ======================================================================
// token.cpp - تنفيذ فئة الرمز / Token Class Implementation
// ======================================================================
// الوصف بالعربية:
//   تنفيذ جميع وظائف فئة Token مع سجلات تنقيح مفصلة
//
// English Description:
//   Implementation of all Token class functions with detailed debug logging
//
// ملاحظات / Notes:
//   - يتضمن سجلات DEBUG_PRINT لتتبع إنشاء الرموز
//     Includes DEBUG_PRINT logs for tracking token creation
//   - تحويل TokenType إلى نصوص قابلة للقراءة
//     Converts TokenType to human-readable strings
// ======================================================================

#include "../../include/lexer/token.h"
#include "../../include/utils/string_utils.h"
#include <sstream>
#include <algorithm>

namespace Sad {
namespace Lexer {

// ======================================================================
// المنشئات / Constructors
// ======================================================================

/**
 * @brief (AR) منشئ كامل - ينشئ رمز مع جميع المعلومات
 * @brief (EN) Full constructor - creates token with all information
 */
Token::Token(TokenType type, const std::string& value, const Position& position)
    : type_(type), value_(value), position_(position) {
    DEBUG_PRINT("إنشاء رمز جديد: " + toString());
}

/**
 * @brief (AR) منشئ مبسط بدون موقع
 * @brief (EN) Simplified constructor without position
 */
Token::Token(TokenType type, const std::string& value)
    : type_(type), value_(value), position_(Position()) {
    DEBUG_PRINT("إنشاء رمز بدون موقع: " + getTypeName() + " = '" + value + "'");
}

/**
 * @brief (AR) منشئ بنوع فقط
 * @brief (EN) Constructor with type only
 */
Token::Token(TokenType type)
    : type_(type), value_(""), position_(Position()) {
    DEBUG_PRINT("إنشاء رمز بنوع فقط: " + getTypeName());
}

// ======================================================================
// وظائف الفحص / Check Functions
// ======================================================================

/**
 * @brief (AR) التحقق من أن الرمز أحد عدة أنواع
 * @brief (EN) Check if token is one of several types
 */
bool Token::isOneOf(std::initializer_list<TokenType> types) const {
    DEBUG_PRINT("فحص إذا كان الرمز ضمن قائمة أنواع");
    
    for (TokenType t : types) {
        if (type_ == t) {
            DEBUG_PRINT("الرمز مطابق للنوع: " + typeToString(t));
            return true;
        }
    }
    
    DEBUG_PRINT("الرمز غير مطابق لأي نوع في القائمة");
    return false;
}

/**
 * @brief (AR) التحقق من أن الرمز كلمة مفتاحية
 * @brief (EN) Check if token is a keyword
 */
bool Token::isKeyword() const {
    DEBUG_PRINT("فحص إذا كان الرمز كلمة مفتاحية");
    
    // الكلمات المفتاحية تبدأ من KEYWORD_
    bool result = (type_ >= TokenType::KEYWORD_FUNCTION && 
                   type_ <= TokenType::KEYWORD_STATIC);
    
    DEBUG_PRINT("النتيجة: " + std::string(result ? "نعم" : "لا"));
    return result;
}

/**
 * @brief (AR) التحقق من أن الرمز عامل
 * @brief (EN) Check if token is an operator
 */
bool Token::isOperator() const {
    DEBUG_PRINT("فحص إذا كان الرمز عامل");
    
    // العوامل تبدأ من OP_
    bool result = (type_ >= TokenType::OP_PLUS && 
                   type_ <= TokenType::OP_DECREMENT);
    
    DEBUG_PRINT("النتيجة: " + std::string(result ? "نعم" : "لا"));
    return result;
}

/**
 * @brief (AR) التحقق من أن الرمز قيمة حرفية
 * @brief (EN) Check if token is a literal
 */
bool Token::isLiteral() const {
    DEBUG_PRINT("فحص إذا كان الرمز قيمة حرفية");
    
    bool result = (type_ == TokenType::NUMBER_INTEGER ||
                   type_ == TokenType::NUMBER_DOUBLE ||
                   type_ == TokenType::STRING_LITERAL ||
                   type_ == TokenType::LITERAL_TRUE ||
                   type_ == TokenType::LITERAL_FALSE ||
                   type_ == TokenType::LITERAL_NULL);
    
    DEBUG_PRINT("النتيجة: " + std::string(result ? "نعم" : "لا"));
    return result;
}

// ======================================================================
// وظائف التحويل / Conversion Functions
// ======================================================================

/**
 * @brief (AR) تحويل الرمز إلى نص للطباعة
 * @brief (EN) Convert token to string for printing
 */
std::string Token::toString() const {
    std::stringstream ss;
    ss << "Token(" 
       << getTypeName() 
       << ", '" << value_ << "', " 
       << position_.toString() 
       << ")";
    return ss.str();
}

/**
 * @brief (AR) الحصول على اسم نوع الرمز
 * @brief (EN) Get token type name
 */
std::string Token::getTypeName() const {
    return typeToString(type_);
}

/**
 * @brief (AR) تحويل نوع الرمز إلى اسم
 * @brief (EN) Convert token type to name
 */
std::string Token::typeToString(TokenType type) {
    switch (type) {
        // الكلمات المفتاحية - الدوال والبنيات
        case TokenType::KEYWORD_FUNCTION:    return "KEYWORD_FUNCTION";
        case TokenType::KEYWORD_RETURN:      return "KEYWORD_RETURN";
        case TokenType::KEYWORD_CLASS:       return "KEYWORD_CLASS";
        case TokenType::KEYWORD_NEW:         return "KEYWORD_NEW";
        case TokenType::KEYWORD_THIS:        return "KEYWORD_THIS";
        case TokenType::KEYWORD_CONSTRUCTOR: return "KEYWORD_CONSTRUCTOR";
        case TokenType::KEYWORD_LAMBDA:      return "KEYWORD_LAMBDA";
        case TokenType::KEYWORD_YIELD:       return "KEYWORD_YIELD";
        
        // الكلمات المفتاحية - التحكم في التدفق
        case TokenType::KEYWORD_IF:          return "KEYWORD_IF";
        case TokenType::KEYWORD_ELSE:        return "KEYWORD_ELSE";
        case TokenType::KEYWORD_ELSE_IF:     return "KEYWORD_ELSE_IF";
        case TokenType::KEYWORD_WHILE:       return "KEYWORD_WHILE";
        case TokenType::KEYWORD_FOR:         return "KEYWORD_FOR";
        case TokenType::KEYWORD_IN:          return "KEYWORD_IN";
        case TokenType::KEYWORD_BREAK:       return "KEYWORD_BREAK";
        case TokenType::KEYWORD_CONTINUE:    return "KEYWORD_CONTINUE";
        
        // الكلمات المفتاحية - معالجة الأخطاء
        case TokenType::KEYWORD_TRY:         return "KEYWORD_TRY";
        case TokenType::KEYWORD_CATCH:       return "KEYWORD_CATCH";
        case TokenType::KEYWORD_THROW:       return "KEYWORD_THROW";
        case TokenType::KEYWORD_FINALLY:     return "KEYWORD_FINALLY";
        
        // الكلمات المفتاحية - التحكم بالوصول
        case TokenType::KEYWORD_PUBLIC:      return "KEYWORD_PUBLIC";
        case TokenType::KEYWORD_PRIVATE:     return "KEYWORD_PRIVATE";
        case TokenType::KEYWORD_PROTECTED:   return "KEYWORD_PROTECTED";
        
        // الكلمات المفتاحية - الوراثة
        case TokenType::KEYWORD_EXTENDS:     return "KEYWORD_EXTENDS";
        case TokenType::KEYWORD_IMPLEMENTS:  return "KEYWORD_IMPLEMENTS";
        case TokenType::KEYWORD_INTERFACE:   return "KEYWORD_INTERFACE";
        case TokenType::KEYWORD_ABSTRACT:    return "KEYWORD_ABSTRACT";
        case TokenType::KEYWORD_VIRTUAL:     return "KEYWORD_VIRTUAL";
        case TokenType::KEYWORD_OVERRIDE:    return "KEYWORD_OVERRIDE";
        
        // الكلمات المفتاحية - الوحدات
        case TokenType::KEYWORD_IMPORT:      return "KEYWORD_IMPORT";
        case TokenType::KEYWORD_EXPORT:      return "KEYWORD_EXPORT";
        case TokenType::KEYWORD_MODULE:      return "KEYWORD_MODULE";
        case TokenType::KEYWORD_PACKAGE:     return "KEYWORD_PACKAGE";
        
        // الكلمات المفتاحية - المتغيرات
        case TokenType::KEYWORD_VAR:         return "KEYWORD_VAR";
        case TokenType::KEYWORD_CONST:       return "KEYWORD_CONST";
        case TokenType::KEYWORD_LET:         return "KEYWORD_LET";
        case TokenType::KEYWORD_STATIC:      return "KEYWORD_STATIC";
        
        // أنواع البيانات
        case TokenType::TYPE_INTEGER:        return "TYPE_INTEGER";
        case TokenType::TYPE_DOUBLE:         return "TYPE_DOUBLE";
        case TokenType::TYPE_STRING:         return "TYPE_STRING";
        case TokenType::TYPE_BOOLEAN:        return "TYPE_BOOLEAN";
        case TokenType::TYPE_VOID:           return "TYPE_VOID";
        case TokenType::TYPE_NULL:           return "TYPE_NULL";
        case TokenType::TYPE_ARRAY:          return "TYPE_ARRAY";
        case TokenType::TYPE_MAP:            return "TYPE_MAP";
        
        // القيم الحرفية
        case TokenType::LITERAL_TRUE:        return "LITERAL_TRUE";
        case TokenType::LITERAL_FALSE:       return "LITERAL_FALSE";
        case TokenType::LITERAL_NULL:        return "LITERAL_NULL";
        
        // المعرفات والقيم
        case TokenType::IDENTIFIER:          return "IDENTIFIER";
        case TokenType::NUMBER_INTEGER:      return "NUMBER_INTEGER";
        case TokenType::NUMBER_DOUBLE:       return "NUMBER_DOUBLE";
        case TokenType::STRING_LITERAL:      return "STRING_LITERAL";
        
        // العوامل الحسابية
        case TokenType::OP_PLUS:             return "OP_PLUS";
        case TokenType::OP_MINUS:            return "OP_MINUS";
        case TokenType::OP_MULTIPLY:         return "OP_MULTIPLY";
        case TokenType::OP_DIVIDE:           return "OP_DIVIDE";
        case TokenType::OP_MODULO:           return "OP_MODULO";
        case TokenType::OP_POWER:            return "OP_POWER";
        
        // عوامل التعيين
        case TokenType::OP_ASSIGN:           return "OP_ASSIGN";
        case TokenType::OP_PLUS_ASSIGN:      return "OP_PLUS_ASSIGN";
        case TokenType::OP_MINUS_ASSIGN:     return "OP_MINUS_ASSIGN";
        case TokenType::OP_MULTIPLY_ASSIGN:  return "OP_MULTIPLY_ASSIGN";
        case TokenType::OP_DIVIDE_ASSIGN:    return "OP_DIVIDE_ASSIGN";
        
        // عوامل المقارنة
        case TokenType::OP_EQUAL:            return "OP_EQUAL";
        case TokenType::OP_NOT_EQUAL:        return "OP_NOT_EQUAL";
        case TokenType::OP_LESS:             return "OP_LESS";
        case TokenType::OP_LESS_EQUAL:       return "OP_LESS_EQUAL";
        case TokenType::OP_GREATER:          return "OP_GREATER";
        case TokenType::OP_GREATER_EQUAL:    return "OP_GREATER_EQUAL";
        
        // العوامل المنطقية
        case TokenType::OP_AND:              return "OP_AND";
        case TokenType::OP_OR:               return "OP_OR";
        case TokenType::OP_NOT:              return "OP_NOT";
        
        // الزيادة والنقصان
        case TokenType::OP_INCREMENT:        return "OP_INCREMENT";
        case TokenType::OP_DECREMENT:        return "OP_DECREMENT";
        
        // الأقواس
        case TokenType::PAREN_LEFT:          return "PAREN_LEFT";
        case TokenType::PAREN_RIGHT:         return "PAREN_RIGHT";
        case TokenType::BRACE_LEFT:          return "BRACE_LEFT";
        case TokenType::BRACE_RIGHT:         return "BRACE_RIGHT";
        case TokenType::BRACKET_LEFT:        return "BRACKET_LEFT";
        case TokenType::BRACKET_RIGHT:       return "BRACKET_RIGHT";
        
        // علامات الترقيم
        case TokenType::COMMA:               return "COMMA";
        case TokenType::SEMICOLON:           return "SEMICOLON";
        case TokenType::COLON:               return "COLON";
        case TokenType::DOT:                 return "DOT";
        case TokenType::ARROW:               return "ARROW";
        case TokenType::FAT_ARROW:           return "FAT_ARROW";
        case TokenType::QUESTION:            return "QUESTION";
        case TokenType::AT_SIGN:             return "AT_SIGN";
        
        // رموز خاصة / Special tokens
        case TokenType::COMMENT:             return "COMMENT";
        case TokenType::NEWLINE:             return "NEWLINE";
        case TokenType::END_OF_FILE:         return "END_OF_FILE";
        case TokenType::INVALID:             return "INVALID";
        case TokenType::WHITESPACE:          return "WHITESPACE";
        
        default:                             return "UNKNOWN";
    }
}

} // namespace Lexer
} // namespace Sad
