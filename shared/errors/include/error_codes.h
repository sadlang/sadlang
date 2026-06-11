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

namespace Sad
{
    namespace Errors
    {

        /**
         * @enum ErrorCode
         * @brief (AR) رموز الأخطاء المصنفة
         *        (EN) Classified error codes
         */
        enum class ErrorCode
        {
            // ====================================================================
            // (AR) أخطاء معجمية / (EN) Lexical Errors
            // Range: LEX001 - LEX099
            // ====================================================================

            LEX_INVALID_CHARACTER, ///< LEX001: (AR) رمز غير صالح
                                   ///< (EN) Invalid character

            LEX_UNTERMINATED_STRING, ///< LEX002: (AR) نص غير مغلق
                                     ///< (EN) Unterminated string

            LEX_INVALID_NUMBER, ///< LEX003: (AR) رقم غير صالح
                                ///< (EN) Invalid number format

            LEX_INVALID_ESCAPE, ///< LEX004: (AR) تسلسل هروب غير صالح
                                ///< (EN) Invalid escape sequence

            LEX_NUMBER_TOO_LARGE, ///< LEX005: (AR) رقم كبير جداً
                                  ///< (EN) Number too large

            LEX_INVALID_UTF8, ///< LEX006: (AR) ترميز UTF-8 غير صالح
                              ///< (EN) Invalid UTF-8 encoding

            // ====================================================================
            // (AR) أخطاء نحوية / (EN) Syntax Errors
            // Range: SYN001 - SYN099
            // ====================================================================

            SYN_UNEXPECTED_TOKEN, ///< SYN001: (AR) رمز غير متوقع
                                  ///< (EN) Unexpected token

            SYN_MISSING_SEMICOLON, ///< SYN002: (AR) فاصلة منقوطة مفقودة
                                   ///< (EN) Missing semicolon

            SYN_UNCLOSED_BRACKET, ///< SYN003: (AR) قوس غير مغلق
                                  ///< (EN) Unclosed bracket

            SYN_MISSING_IDENTIFIER, ///< SYN004: (AR) معرّف مفقود
                                    ///< (EN) Missing identifier

            SYN_INVALID_EXPRESSION, ///< SYN005: (AR) تعبير غير صالح
                                    ///< (EN) Invalid expression

            SYN_UNEXPECTED_EOF, ///< SYN006: (AR) نهاية ملف غير متوقعة
                                ///< (EN) Unexpected end of file

            SYN_MISSING_COLON, ///< SYN007: (AR) نقطتان مفقودتان
                               ///< (EN) Missing colon

            SYN_INVALID_ASSIGNMENT, ///< SYN008: (AR) إسناد غير صالح
                                    ///< (EN) Invalid assignment

            // ====================================================================
            // (AR) أخطاء دلالية / (EN) Semantic Errors
            // Range: SEM001 - SEM099
            // ====================================================================

            SEM_UNDEFINED_VARIABLE, ///< SEM001: (AR) متغير غير معرّف
                                    ///< (EN) Undefined variable

            SEM_TYPE_MISMATCH, ///< SEM002: (AR) عدم تطابق الأنواع
                               ///< (EN) Type mismatch

            SEM_REDEFINITION, ///< SEM003: (AR) إعادة تعريف
                              ///< (EN) Redefinition

            SEM_UNDEFINED_FUNCTION, ///< SEM004: (AR) دالة غير معرّفة
                                    ///< (EN) Undefined function

            SEM_WRONG_ARG_COUNT, ///< SEM005: (AR) عدد معاملات خاطئ
                                 ///< (EN) Wrong argument count

            SEM_INVALID_OPERATION, ///< SEM006: (AR) عملية غير صالحة
                                   ///< (EN) Invalid operation

            SEM_CONST_ASSIGNMENT, ///< SEM007: (AR) إسناد لثابت
                                  ///< (EN) Assignment to constant

            SEM_UNDEFINED_CLASS, ///< SEM008: (AR) صنف غير معرّف
                                 ///< (EN) Undefined class

            SEM_PRIVATE_ACCESS, ///< SEM009: (AR) وصول لعضو خاص
                                ///< (EN) Access to private member

            // ====================================================================
            // (AR) أخطاء وقت التشغيل / (EN) Runtime Errors
            // Range: RUN001 - RUN099
            // ====================================================================

            RUN_DIVISION_BY_ZERO,   ///< RUN001: (AR) قسمة على صفر / (EN) Division by zero
            RUN_INDEX_OUT_OF_RANGE, ///< RUN002: (AR) فهرس خارج النطاق / (EN) Index out of range
            RUN_NULL_REFERENCE,     ///< RUN003: (AR) مرجع فارغ / (EN) Null reference
            RUN_STACK_OVERFLOW,     ///< RUN004: (AR) تجاوز سعة المكدس / (EN) Stack overflow
            RUN_KEY_NOT_FOUND,      ///< RUN005: (AR) مفتاح غير موجود / (EN) Key not found
            RUN_INVALID_CAST,       ///< RUN006: (AR) تحويل نوع غير صالح / (EN) Invalid type cast
            RUN_FILE_ERROR,         ///< RUN007: (AR) خطأ في الملف / (EN) File error
            RUN_IMPORT_ERROR,       ///< RUN008: (AR) خطأ في الاستيراد / (EN) Import error

            // ====================================================================
            // (AR) Phase 4 — رموز دلالية مفصّلة لتغطية كل throw في المفسر القديم
            // (EN) Phase 4 — semantic codes covering every legacy interpreter throw
            // ====================================================================

            // --- Arithmetic / حسابي ---
            RUN_FLOOR_DIVISION_BY_ZERO, ///< RUN009: (AR) قسمة صحيحة على صفر / (EN) Floor div by zero
            RUN_MODULO_BY_ZERO,         ///< RUN010: (AR) باقي القسمة على صفر / (EN) Modulo by zero
            RUN_NUMERIC_OVERFLOW,       ///< RUN011: (AR) تجاوز عددي / (EN) Numeric overflow

            // --- Collections / مجموعات ---
            RUN_STRING_INDEX_OUT_OF_RANGE, ///< RUN012: (AR) فهرس نص خارج النطاق / (EN) String index out of range
            RUN_STRING_INDEX_NOT_INTEGER,  ///< RUN013: (AR) فهرس النص ليس صحيحاً / (EN) String index not integer
            RUN_TUPLE_INDEX_NOT_NUMBER,    ///< RUN014: (AR) فهرس الصف ليس رقماً / (EN) Tuple index not number
            RUN_ARRAY_INDEX_NOT_NUMBER,    ///< RUN015: (AR) فهرس المصفوفة ليس رقماً / (EN) Array index not number
            RUN_SLICE_STEP_ZERO,           ///< RUN016: (AR) خطوة الشريحة صفر / (EN) Slice step is zero
            RUN_SLICE_TYPE_INVALID,        ///< RUN017: (AR) نوع غير صالح للشريحة / (EN) Invalid type for slicing
            RUN_INDEX_ASSIGN_TYPE_INVALID, ///< RUN018: (AR) إسناد بفهرس على نوع غير صالح / (EN) Indexed assignment on invalid type
            RUN_EMPTY_COLLECTION,          ///< RUN019: (AR) عملية على مجموعة فارغة / (EN) Operation on empty collection

            // --- OOP / كائنية ---
            RUN_CLASS_NOT_FOUND,        ///< RUN020: (AR) صنف غير موجود / (EN) Class not found
            RUN_BASE_CLASS_NOT_FOUND,   ///< RUN021: (AR) صنف أساسي غير موجود / (EN) Base class not found
            RUN_METHOD_NOT_FOUND,       ///< RUN022: (AR) طريقة غير موجودة / (EN) Method not found
            RUN_OBJECT_WITHOUT_CLASS,   ///< RUN023: (AR) كائن بدون معلومات صنف / (EN) Object without class info
            RUN_CONSTRUCTOR_FAILED,     ///< RUN024: (AR) فشل الباني / (EN) Constructor failed
            RUN_PROPERTY_NOT_FOUND,     ///< RUN025: (AR) خاصية غير موجودة / (EN) Property not found
            RUN_INHERITANCE_CYCLE,      ///< RUN026: (AR) دورة في الوراثة / (EN) Inheritance cycle
            RUN_ABSTRACT_INSTANTIATION, ///< RUN027: (AR) إنشاء صنف مجرد / (EN) Abstract class instantiation

            // --- Functions / دوال ---
            RUN_FUNCTION_NOT_FOUND,    ///< RUN028: (AR) دالة غير موجودة / (EN) Function not found
            RUN_MISSING_REQUIRED_ARG,  ///< RUN029: (AR) معامل إلزامي مفقود / (EN) Missing required argument
            RUN_TOO_MANY_ARGS,         ///< RUN030: (AR) معاملات زائدة / (EN) Too many arguments
            RUN_NOT_CALLABLE,          ///< RUN031: (AR) قيمة غير قابلة للاستدعاء / (EN) Value not callable
            RUN_LAMBDA_RETURN_INVALID, ///< RUN032: (AR) إرجاع لامدا غير صالح / (EN) Invalid lambda return

            // --- Types / أنواع ---
            RUN_OPERAND_TYPE_INVALID,    ///< RUN033: (AR) نوع معامل غير صالح / (EN) Invalid operand type
            RUN_TYPE_CONVERSION_FAILED,  ///< RUN034: (AR) فشل تحويل النوع / (EN) Type conversion failed
            RUN_IN_OPERATOR_RHS_INVALID, ///< RUN035: (AR) عامل 'في' يحتاج مصفوفة/خريطة/نص / (EN) 'in' operator RHS invalid
            RUN_TYPE_CHECK_FAILED,       ///< RUN036: (AR) فشل فحص النوع / (EN) Type check failed

            // --- Builtins / مدمجة ---
            RUN_BUILTIN_REQUIRES_ARG,      ///< RUN037: (AR) دالة مدمجة تتطلب معاملاً / (EN) Builtin requires argument
            RUN_BUILTIN_REQUIRES_FUNCTION, ///< RUN038: (AR) دالة مدمجة تتطلب دالة كمعامل / (EN) Builtin requires function arg
            RUN_BUILTIN_INVALID_INDEX,     ///< RUN039: (AR) فهرس غير صالح في دالة مدمجة / (EN) Invalid index in builtin
            RUN_BUILTIN_EMPTY_OPERATION,   ///< RUN040: (AR) عملية مدمجة على مجموعة فارغة / (EN) Builtin op on empty collection

            // --- Contracts / عقود ---
            RUN_CONTRACT_PRECOND_FAILED,  ///< RUN041: (AR) فشل الشرط المسبق / (EN) Precondition failed
            RUN_CONTRACT_POSTCOND_FAILED, ///< RUN042: (AR) فشل الشرط اللاحق / (EN) Postcondition failed
            RUN_CONTRACT_WHERE_FAILED,    ///< RUN043: (AR) فشل قيد النوع (حيث) / (EN) Where constraint failed

            // --- Concurrency / تزامن ---
            RUN_ASYNC_TASK_ERROR,         ///< RUN044: (AR) خطأ في مهمة غير متزامنة / (EN) Async task error
            RUN_GENERATOR_TYPE_INVALID,   ///< RUN045: (AR) نوع المولّد غير صالح / (EN) Invalid generator type
            RUN_CHANNEL_OPERATION_FAILED, ///< RUN046: (AR) فشل عملية قناة / (EN) Channel operation failed

            // --- Misc / متفرقات ---
            RUN_SECURITY_VIOLATION,  ///< RUN047: (AR) انتهاك أمني / (EN) Security violation
            RUN_ASSERTION_FAILED,    ///< RUN048: (AR) فشل تأكيد / (EN) Assertion failed
            RUN_PANIC,               ///< RUN049: (AR) ذعر غير قابل للتعافي / (EN) Unrecoverable panic
            RUN_PERMISSION_DENIED,   ///< RUN050: (AR) صلاحية مرفوضة / (EN) Permission denied
            RUN_OFFSET_OUT_OF_RANGE, ///< RUN051: (AR) إزاحة خارج النطاق / (EN) Offset out of range

            // --- User-thrown / استثناء المستخدم ---
            RUN_USER_THROWN, ///< RUN052: (AR) قيمة مرمية بـ ارمي / (EN) User-thrown value via 'throw'

            // ════════════════════════════════════════════════════════════════
            // (AR) أخطاء المترجم الداخلية (ICE) — خلل في المترجم نفسه، من الكتالوج.
            // (EN) Internal compiler errors (ICE) — compiler defects, catalog-based.
            // ════════════════════════════════════════════════════════════════
            INT_COMPILER_NULL_IR,          ///< INT001: (AR) عقدة IR فارغة / (EN) Null IR node
            INT_COMPILER_INVALID_OPERANDS, ///< INT002: (AR) عدد معاملات خاطئ / (EN) Wrong operand count
            INT_SIR_OPERAND_RESOLVE,       ///< INT003: (AR) تعذّر حلّ معامل / (EN) Operand resolution failed
            INT_SIR_UNDEFINED_REF,         ///< INT004: (AR) مرجع غير معرَّف / (EN) Undefined IR reference
            INT_SIR_FIELD_LAYOUT,          ///< INT005: (AR) تخطيط حقل/صنف / (EN) Class/field layout failed
            INT_BACKEND_EMIT,              ///< INT006: (AR) فشل إصدار الخلفية / (EN) Backend emit failure
            INT_SIR_TYPE_CONSTRAINT,       ///< INT007: (AR) انتهاك قيد نوع / (EN) Type constraint violated
            INT_INTERP_NAMELESS_DEFINITION, ///< INT008: (AR) تعريف دالة بلا اسم / (EN) Nameless function definition
            INT_INTERP_SCOPE_STACK          ///< INT009: (AR) خلل مكدّس النطاقات / (EN) Scope stack violation
        };

        /**
         * @enum Language
         * @brief (AR) اللغة المستخدمة في الرسائل
         *        (EN) Language used for messages
         */
        enum class Language
        {
            ARABIC,  ///< (AR) العربية
            ENGLISH, ///< (EN) English
            BOTH     ///< (AR) كلاهما / (EN) Both
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
