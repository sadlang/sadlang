/**
 * @file error_codes.h
 * @brief (AR) رموز الأخطاء الموحدة للغة "ص"
 *        (EN) Unified error codes for Sad language
 * 
 * (AR) هذا الملف يحتوي على جميع رموز الأخطاء المستخدمة في اللغة
 *      مصنفة حسب المرحلة: معجمية، نحوية، دلالية، تنفيذية
 * 
 * (EN) This file contains all error codes used in the language
 *      classified by phase: lexical, syntax, semantic, runtime
 * 
 * @author Sad Language Development Team
 * @date November 23, 2025
 * @version 1.0
 */

#pragma once

#include <string>
#include <unordered_map>

namespace Sad {
namespace Errors {

/**
 * @enum ErrorCode
 * @brief (AR) رموز الأخطاء المصنفة
 *        (EN) Classified error codes
 */
enum class ErrorCode {
    // ====================================================================
    // (AR) أخطاء معجمية / (EN) Lexical Errors
    // Range: LEX001 - LEX099
    // ====================================================================
    
    LEX_INVALID_CHARACTER,       ///< LEX001: (AR) رمز غير صالح
                                ///< (EN) Invalid character
    
    LEX_UNTERMINATED_STRING,     ///< LEX002: (AR) نص غير مغلق
                                ///< (EN) Unterminated string
    
    LEX_INVALID_NUMBER,          ///< LEX003: (AR) رقم غير صالح
                                ///< (EN) Invalid number format
    
    LEX_INVALID_ESCAPE,          ///< LEX004: (AR) تسلسل هروب غير صالح
                                ///< (EN) Invalid escape sequence
    
    LEX_NUMBER_TOO_LARGE,        ///< LEX005: (AR) رقم كبير جداً
                                ///< (EN) Number too large
    
    LEX_INVALID_UTF8,            ///< LEX006: (AR) ترميز UTF-8 غير صالح
                                ///< (EN) Invalid UTF-8 encoding
    
    // ====================================================================
    // (AR) أخطاء نحوية / (EN) Syntax Errors
    // Range: SYN001 - SYN099
    // ====================================================================
    
    SYN_UNEXPECTED_TOKEN,        ///< SYN001: (AR) رمز غير متوقع
                                ///< (EN) Unexpected token
    
    SYN_MISSING_SEMICOLON,       ///< SYN002: (AR) فاصلة منقوطة مفقودة
                                ///< (EN) Missing semicolon
    
    SYN_UNCLOSED_BRACKET,        ///< SYN003: (AR) قوس غير مغلق
                                ///< (EN) Unclosed bracket
    
    SYN_MISSING_IDENTIFIER,      ///< SYN004: (AR) معرّف مفقود
                                ///< (EN) Missing identifier
    
    SYN_INVALID_EXPRESSION,      ///< SYN005: (AR) تعبير غير صالح
                                ///< (EN) Invalid expression
    
    SYN_UNEXPECTED_EOF,          ///< SYN006: (AR) نهاية ملف غير متوقعة
                                ///< (EN) Unexpected end of file
    
    SYN_MISSING_COLON,           ///< SYN007: (AR) نقطتان مفقودتان
                                ///< (EN) Missing colon
    
    SYN_INVALID_ASSIGNMENT,      ///< SYN008: (AR) إسناد غير صالح
                                ///< (EN) Invalid assignment
    
    // ====================================================================
    // (AR) أخطاء دلالية / (EN) Semantic Errors
    // Range: SEM001 - SEM099
    // ====================================================================
    
    SEM_UNDEFINED_VARIABLE,      ///< SEM001: (AR) متغير غير معرّف
                                ///< (EN) Undefined variable
    
    SEM_TYPE_MISMATCH,           ///< SEM002: (AR) عدم تطابق الأنواع
                                ///< (EN) Type mismatch
    
    SEM_REDEFINITION,            ///< SEM003: (AR) إعادة تعريف
                                ///< (EN) Redefinition
    
    SEM_UNDEFINED_FUNCTION,      ///< SEM004: (AR) دالة غير معرّفة
                                ///< (EN) Undefined function
    
    SEM_WRONG_ARG_COUNT,         ///< SEM005: (AR) عدد معاملات خاطئ
                                ///< (EN) Wrong argument count
    
    SEM_INVALID_OPERATION,       ///< SEM006: (AR) عملية غير صالحة
                                ///< (EN) Invalid operation
    
    SEM_CONST_ASSIGNMENT,        ///< SEM007: (AR) إسناد لثابت
                                ///< (EN) Assignment to constant
    
    SEM_UNDEFINED_CLASS,         ///< SEM008: (AR) صنف غير معرّف
                                ///< (EN) Undefined class
    
    SEM_PRIVATE_ACCESS,          ///< SEM009: (AR) وصول لعضو خاص
                                ///< (EN) Access to private member
    
    // ====================================================================
    // (AR) أخطاء وقت التشغيل / (EN) Runtime Errors
    // Range: RUN001 - RUN099
    // ====================================================================
    
    RUN_DIVISION_BY_ZERO,        ///< RUN001: (AR) قسمة على صفر
                                ///< (EN) Division by zero
    
    RUN_INDEX_OUT_OF_RANGE,      ///< RUN002: (AR) فهرس خارج النطاق
                                ///< (EN) Index out of range
    
    RUN_NULL_REFERENCE,          ///< RUN003: (AR) مرجع فارغ
                                ///< (EN) Null reference
    
    RUN_STACK_OVERFLOW,          ///< RUN004: (AR) تجاوز سعة المكدس
                                ///< (EN) Stack overflow
    
    RUN_KEY_NOT_FOUND,           ///< RUN005: (AR) مفتاح غير موجود
                                ///< (EN) Key not found
    
    RUN_INVALID_CAST,            ///< RUN006: (AR) تحويل نوع غير صالح
                                ///< (EN) Invalid type cast
    
    RUN_FILE_ERROR,              ///< RUN007: (AR) خطأ في الملف
                                ///< (EN) File error
    
    RUN_IMPORT_ERROR             ///< RUN008: (AR) خطأ في الاستيراد
                                ///< (EN) Import error
};

/**
 * @enum Language
 * @brief (AR) اللغة المستخدمة في الرسائل
 *        (EN) Language used for messages
 */
enum class Language {
    ARABIC,      ///< (AR) العربية
    ENGLISH,     ///< (EN) English
    BOTH         ///< (AR) كلاهما / (EN) Both
};

/**
 * @brief (AR) يحول رمز الخطأ إلى نص
 *        (EN) Converts error code to string
 * 
 * (AR) تحول ErrorCode إلى معرف نصي مثل "LEX001" أو "SYN002"
 * (EN) Converts ErrorCode to text identifier like "LEX001" or "SYN002"
 * 
 * @param code (AR) رمز الخطأ / (EN) Error code
 * @return (AR) النص المقابل (مثل "SYN002") / (EN) Corresponding string (e.g., "SYN002")
 * 
 * @example
 * @code
 * std::string code = getErrorCodeString(ErrorCode::SYN_MISSING_SEMICOLON);
 * // Returns: "SYN002"
 * @endcode
 */
std::string getErrorCodeString(ErrorCode code);

/**
 * @brief (AR) يرجع وصف الخطأ باللغة المحددة
 *        (EN) Returns error description in specified language
 * 
 * (AR) تُرجع وصفاً تفصيلياً للخطأ باللغة المحددة (عربي أو إنجليزي)
 * (EN) Returns detailed error description in specified language (Arabic or English)
 * 
 * @param code (AR) رمز الخطأ / (EN) Error code
 * @param lang (AR) اللغة المطلوبة / (EN) Requested language
 * @return (AR) الوصف باللغة المحددة / (EN) Description in specified language
 * 
 * @example
 * @code
 * std::string desc = getErrorDescription(
 *     ErrorCode::SYN_MISSING_SEMICOLON, 
 *     Language::ARABIC
 * );
 * // Returns: "فاصلة منقوطة مفقودة"
 * @endcode
 */
std::string getErrorDescription(ErrorCode code, Language lang);

/**
 * @brief (AR) يرجع اسم فئة الخطأ
 *        (EN) Returns error category name
 * 
 * (AR) تُرجع اسم الفئة التي ينتمي إليها الخطأ (معجمي، نحوي، دلالي، تنفيذي)
 * (EN) Returns category name the error belongs to (Lexical, Syntax, Semantic, Runtime)
 * 
 * @param code (AR) رمز الخطأ / (EN) Error code
 * @return (AR) اسم الفئة / (EN) Category name
 * 
 * @example
 * @code
 * std::string category = getErrorCategory(ErrorCode::SYN_MISSING_SEMICOLON);
 * // Returns: "Syntax"
 * @endcode
 */
std::string getErrorCategory(ErrorCode code);

} // namespace Errors
} // namespace Sad
