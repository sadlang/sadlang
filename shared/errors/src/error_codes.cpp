/**
 * @file error_codes.cpp
 * @brief (AR) تطبيق دوال رموز الأخطاء للغة "ص"
 *        (EN) Implementation of error code functions for Sad language
 *
 * (AR) يحتوي هذا الملف على تطبيقات جميع الدوال المساعدة لرموز الأخطاء
 *      بما في ذلك الوصوفات ثنائية اللغة لكل رمز خطأ
 *
 * (EN) This file contains implementations of all error code helper functions
 *      including bilingual descriptions for each error code
 *
 * @author Sad Language Development Team
 * @date November 23, 2025
 * @version 1.0
 */

#include "error_codes.h"
#include <unordered_map>
#include <sstream>

namespace Sad
{
    namespace Errors
    {

        /**
         * @brief (AR) خريطة تربط ErrorCode بالنص (LEX001, SYN002, إلخ)
         *        (EN) Map linking ErrorCode to string (LEX001, SYN002, etc.)
         */
        static const std::unordered_map<ErrorCode, std::string> errorCodeStrings = {
            // Lexical Errors
            {ErrorCode::LEX_INVALID_CHARACTER, "LEX001"},
            {ErrorCode::LEX_UNTERMINATED_STRING, "LEX002"},
            {ErrorCode::LEX_INVALID_NUMBER, "LEX003"},
            {ErrorCode::LEX_INVALID_ESCAPE, "LEX004"},
            {ErrorCode::LEX_NUMBER_TOO_LARGE, "LEX005"},
            {ErrorCode::LEX_INVALID_UTF8, "LEX006"},

            // Syntax Errors
            {ErrorCode::SYN_UNEXPECTED_TOKEN, "SYN001"},
            {ErrorCode::SYN_MISSING_SEMICOLON, "SYN002"},
            {ErrorCode::SYN_UNCLOSED_BRACKET, "SYN003"},
            {ErrorCode::SYN_MISSING_IDENTIFIER, "SYN004"},
            {ErrorCode::SYN_INVALID_EXPRESSION, "SYN005"},
            {ErrorCode::SYN_UNEXPECTED_EOF, "SYN006"},
            {ErrorCode::SYN_MISSING_COLON, "SYN007"},
            {ErrorCode::SYN_INVALID_ASSIGNMENT, "SYN008"},

            // Semantic Errors
            {ErrorCode::SEM_UNDEFINED_VARIABLE, "SEM001"},
            {ErrorCode::SEM_TYPE_MISMATCH, "SEM002"},
            {ErrorCode::SEM_REDEFINITION, "SEM003"},
            {ErrorCode::SEM_UNDEFINED_FUNCTION, "SEM004"},
            {ErrorCode::SEM_WRONG_ARG_COUNT, "SEM005"},
            {ErrorCode::SEM_INVALID_OPERATION, "SEM006"},
            {ErrorCode::SEM_CONST_ASSIGNMENT, "SEM007"},
            {ErrorCode::SEM_UNDEFINED_CLASS, "SEM008"},
            {ErrorCode::SEM_PRIVATE_ACCESS, "SEM009"},

            // Runtime Errors
            {ErrorCode::RUN_DIVISION_BY_ZERO, "RUN001"},
            {ErrorCode::RUN_INDEX_OUT_OF_RANGE, "RUN002"},
            {ErrorCode::RUN_NULL_REFERENCE, "RUN003"},
            {ErrorCode::RUN_STACK_OVERFLOW, "RUN004"},
            {ErrorCode::RUN_KEY_NOT_FOUND, "RUN005"},
            {ErrorCode::RUN_INVALID_CAST, "RUN006"},
            {ErrorCode::RUN_FILE_ERROR, "RUN007"},
            {ErrorCode::RUN_IMPORT_ERROR, "RUN008"},
            // Phase 4 new codes (009-052)
            {ErrorCode::RUN_FLOOR_DIVISION_BY_ZERO, "RUN009"},
            {ErrorCode::RUN_MODULO_BY_ZERO, "RUN010"},
            {ErrorCode::RUN_NUMERIC_OVERFLOW, "RUN011"},
            {ErrorCode::RUN_STRING_INDEX_OUT_OF_RANGE, "RUN012"},
            {ErrorCode::RUN_STRING_INDEX_NOT_INTEGER, "RUN013"},
            {ErrorCode::RUN_TUPLE_INDEX_NOT_NUMBER, "RUN014"},
            {ErrorCode::RUN_ARRAY_INDEX_NOT_NUMBER, "RUN015"},
            {ErrorCode::RUN_SLICE_STEP_ZERO, "RUN016"},
            {ErrorCode::RUN_SLICE_TYPE_INVALID, "RUN017"},
            {ErrorCode::RUN_INDEX_ASSIGN_TYPE_INVALID, "RUN018"},
            {ErrorCode::RUN_EMPTY_COLLECTION, "RUN019"},
            {ErrorCode::RUN_CLASS_NOT_FOUND, "RUN020"},
            {ErrorCode::RUN_BASE_CLASS_NOT_FOUND, "RUN021"},
            {ErrorCode::RUN_METHOD_NOT_FOUND, "RUN022"},
            {ErrorCode::RUN_OBJECT_WITHOUT_CLASS, "RUN023"},
            {ErrorCode::RUN_CONSTRUCTOR_FAILED, "RUN024"},
            {ErrorCode::RUN_PROPERTY_NOT_FOUND, "RUN025"},
            {ErrorCode::RUN_INHERITANCE_CYCLE, "RUN026"},
            {ErrorCode::RUN_ABSTRACT_INSTANTIATION, "RUN027"},
            {ErrorCode::RUN_FUNCTION_NOT_FOUND, "RUN028"},
            {ErrorCode::RUN_MISSING_REQUIRED_ARG, "RUN029"},
            {ErrorCode::RUN_TOO_MANY_ARGS, "RUN030"},
            {ErrorCode::RUN_NOT_CALLABLE, "RUN031"},
            {ErrorCode::RUN_LAMBDA_RETURN_INVALID, "RUN032"},
            {ErrorCode::RUN_OPERAND_TYPE_INVALID, "RUN033"},
            {ErrorCode::RUN_TYPE_CONVERSION_FAILED, "RUN034"},
            {ErrorCode::RUN_IN_OPERATOR_RHS_INVALID, "RUN035"},
            {ErrorCode::RUN_TYPE_CHECK_FAILED, "RUN036"},
            {ErrorCode::RUN_BUILTIN_REQUIRES_ARG, "RUN037"},
            {ErrorCode::RUN_BUILTIN_REQUIRES_FUNCTION, "RUN038"},
            {ErrorCode::RUN_BUILTIN_INVALID_INDEX, "RUN039"},
            {ErrorCode::RUN_BUILTIN_EMPTY_OPERATION, "RUN040"},
            {ErrorCode::RUN_CONTRACT_PRECOND_FAILED, "RUN041"},
            {ErrorCode::RUN_CONTRACT_POSTCOND_FAILED, "RUN042"},
            {ErrorCode::RUN_CONTRACT_WHERE_FAILED, "RUN043"},
            {ErrorCode::RUN_ASYNC_TASK_ERROR, "RUN044"},
            {ErrorCode::RUN_GENERATOR_TYPE_INVALID, "RUN045"},
            {ErrorCode::RUN_CHANNEL_OPERATION_FAILED, "RUN046"},
            {ErrorCode::RUN_SECURITY_VIOLATION, "RUN047"},
            {ErrorCode::RUN_ASSERTION_FAILED, "RUN048"},
            {ErrorCode::RUN_PANIC, "RUN049"},
            {ErrorCode::RUN_PERMISSION_DENIED, "RUN050"},
            {ErrorCode::RUN_OFFSET_OUT_OF_RANGE, "RUN051"},
            {ErrorCode::RUN_USER_THROWN, "RUN052"}};

        /**
         * @struct ErrorDescription
         * @brief (AR) وصف الخطأ بلغتين
         *        (EN) Error description in two languages
         */
        struct ErrorDescription
        {
            std::string arabic;  ///< (AR) الوصف بالعربية
            std::string english; ///< (EN) English description
        };

        /**
         * @brief (AR) خريطة تربط ErrorCode بالوصف ثنائي اللغة
         *        (EN) Map linking ErrorCode to bilingual description
         */
        static const std::unordered_map<ErrorCode, ErrorDescription> errorDescriptions = {
            // ====================================================================
            // (AR) أخطاء معجمية / (EN) Lexical Errors
            // ====================================================================
            {ErrorCode::LEX_INVALID_CHARACTER, {"رمز غير صالح في الكود المصدري", "Invalid character in source code"}},
            {ErrorCode::LEX_UNTERMINATED_STRING, {"نص غير مغلق - متوقع علامة اقتباس '\"' في النهاية", "Unterminated string - expected closing quote '\"'"}},
            {ErrorCode::LEX_INVALID_NUMBER, {"تنسيق رقم غير صالح", "Invalid number format"}},
            {ErrorCode::LEX_INVALID_ESCAPE, {"تسلسل هروب غير صالح في النص", "Invalid escape sequence in string"}},
            {ErrorCode::LEX_NUMBER_TOO_LARGE, {"الرقم كبير جداً ويتجاوز الحد المسموح", "Number too large and exceeds allowed limit"}},
            {ErrorCode::LEX_INVALID_UTF8, {"ترميز UTF-8 غير صالح في الملف", "Invalid UTF-8 encoding in file"}},

            // ====================================================================
            // (AR) أخطاء نحوية / (EN) Syntax Errors
            // ====================================================================
            {ErrorCode::SYN_UNEXPECTED_TOKEN, {"رمز غير متوقع في هذا الموضع", "Unexpected token at this position"}},
            {ErrorCode::SYN_MISSING_SEMICOLON, {"فاصلة منقوطة ';' مفقودة في نهاية الجملة", "Missing semicolon ';' at end of statement"}},
            {ErrorCode::SYN_UNCLOSED_BRACKET, {"قوس غير مغلق - متوقع قوس إغلاق مطابق", "Unclosed bracket - expected matching closing bracket"}},
            {ErrorCode::SYN_MISSING_IDENTIFIER, {"معرّف مفقود - متوقع اسم متغير أو دالة", "Missing identifier - expected variable or function name"}},
            {ErrorCode::SYN_INVALID_EXPRESSION, {"تعبير غير صالح أو غير مكتمل", "Invalid or incomplete expression"}},
            {ErrorCode::SYN_UNEXPECTED_EOF, {"نهاية ملف غير متوقعة - الكود غير مكتمل", "Unexpected end of file - code is incomplete"}},
            {ErrorCode::SYN_MISSING_COLON, {"نقطتان ':' مفقودتان", "Missing colon ':'"}},
            {ErrorCode::SYN_INVALID_ASSIGNMENT, {"إسناد غير صالح - تحقق من الصيغة", "Invalid assignment - check syntax"}},

            // ====================================================================
            // (AR) أخطاء دلالية / (EN) Semantic Errors
            // ====================================================================
            {ErrorCode::SEM_UNDEFINED_VARIABLE, {"متغير غير معرّف - لم يتم الإعلان عن هذا المتغير", "Undefined variable - this variable was not declared"}},
            {ErrorCode::SEM_TYPE_MISMATCH, {"عدم تطابق الأنواع - الأنواع غير متوافقة", "Type mismatch - types are incompatible"}},
            {ErrorCode::SEM_REDEFINITION, {"إعادة تعريف - هذا الاسم معرّف مسبقاً", "Redefinition - this name is already defined"}},
            {ErrorCode::SEM_UNDEFINED_FUNCTION, {"دالة غير معرّفة - لم يتم الإعلان عن هذه الدالة", "Undefined function - this function was not declared"}},
            {ErrorCode::SEM_WRONG_ARG_COUNT, {"عدد معاملات خاطئ - لا يطابق توقيع الدالة", "Wrong argument count - does not match function signature"}},
            {ErrorCode::SEM_INVALID_OPERATION, {"عملية غير صالحة على هذه الأنواع", "Invalid operation on these types"}},
            {ErrorCode::SEM_CONST_ASSIGNMENT, {"إسناد لثابت - لا يمكن تغيير قيمة ثابت", "Assignment to constant - cannot change constant value"}},
            {ErrorCode::SEM_UNDEFINED_CLASS, {"صنف غير معرّف - لم يتم الإعلان عن هذا الصنف", "Undefined class - this class was not declared"}},
            {ErrorCode::SEM_PRIVATE_ACCESS, {"وصول لعضو خاص - العضو غير متاح خارج الصنف", "Access to private member - member not accessible outside class"}},

            // ====================================================================
            // (AR) أخطاء وقت التشغيل / (EN) Runtime Errors
            // ====================================================================
            {ErrorCode::RUN_DIVISION_BY_ZERO, {"قسمة على صفر - عملية رياضية غير معرّفة", "Division by zero - undefined mathematical operation"}},
            {ErrorCode::RUN_INDEX_OUT_OF_RANGE, {"فهرس خارج النطاق - الفهرس غير صالح للمصفوفة", "Index out of range - index invalid for array"}},
            {ErrorCode::RUN_NULL_REFERENCE, {"مرجع فارغ - محاولة الوصول لقيمة null", "Null reference - attempting to access null value"}},
            {ErrorCode::RUN_STACK_OVERFLOW, {"تجاوز سعة المكدس - ربما استدعاء متكرر غير محدود", "Stack overflow - possibly infinite recursion"}},
            {ErrorCode::RUN_KEY_NOT_FOUND, {"مفتاح غير موجود في القاموس", "Key not found in dictionary"}},
            {ErrorCode::RUN_INVALID_CAST, {"تحويل نوع غير صالح - لا يمكن تحويل القيمة", "Invalid type cast - cannot convert value"}},
            {ErrorCode::RUN_FILE_ERROR, {"خطأ في العمليات على الملف", "File operation error"}},
            {ErrorCode::RUN_IMPORT_ERROR, {"خطأ في استيراد الوحدة - الملف غير موجود أو به أخطاء", "Import error - file not found or contains errors"}},
            // Phase 4 — Arithmetic
            {ErrorCode::RUN_FLOOR_DIVISION_BY_ZERO, {"قسمة صحيحة على صفر", "Floor division by zero"}},
            {ErrorCode::RUN_MODULO_BY_ZERO, {"باقي القسمة على صفر", "Modulo by zero"}},
            {ErrorCode::RUN_NUMERIC_OVERFLOW, {"تجاوز عددي", "Numeric overflow"}},
            // Phase 4 — Collections
            {ErrorCode::RUN_STRING_INDEX_OUT_OF_RANGE, {"فهرس النص خارج النطاق", "String index out of range"}},
            {ErrorCode::RUN_STRING_INDEX_NOT_INTEGER, {"فهرس النص يجب أن يكون رقم صحيح", "String index must be integer"}},
            {ErrorCode::RUN_TUPLE_INDEX_NOT_NUMBER, {"فهرس الصف يجب أن يكون رقماً", "Tuple index must be a number"}},
            {ErrorCode::RUN_ARRAY_INDEX_NOT_NUMBER, {"فهرس المصفوفة يجب أن يكون رقماً", "Array index must be a number"}},
            {ErrorCode::RUN_SLICE_STEP_ZERO, {"خطوة الشريحة لا يمكن أن تكون صفر", "Slice step cannot be zero"}},
            {ErrorCode::RUN_SLICE_TYPE_INVALID, {"الشريحة تعمل فقط على المصفوفات والنصوص", "Slicing works only on arrays and strings"}},
            {ErrorCode::RUN_INDEX_ASSIGN_TYPE_INVALID, {"لا يمكن الإسناد بالفهرس إلا للمصفوفات والقواميس", "Indexed assignment only works on arrays and dictionaries"}},
            {ErrorCode::RUN_EMPTY_COLLECTION, {"عملية على مجموعة فارغة", "Operation on empty collection"}},
            // Phase 4 — OOP
            {ErrorCode::RUN_CLASS_NOT_FOUND, {"الصنف غير موجود", "Class not found"}},
            {ErrorCode::RUN_BASE_CLASS_NOT_FOUND, {"الصنف الأساسي غير موجود", "Base class not found"}},
            {ErrorCode::RUN_METHOD_NOT_FOUND, {"الطريقة غير موجودة", "Method not found"}},
            {ErrorCode::RUN_OBJECT_WITHOUT_CLASS, {"كائن بدون معلومات صنف", "Object without class info"}},
            {ErrorCode::RUN_CONSTRUCTOR_FAILED, {"فشل تنفيذ الباني", "Constructor failed"}},
            {ErrorCode::RUN_PROPERTY_NOT_FOUND, {"خاصية غير موجودة", "Property not found"}},
            {ErrorCode::RUN_INHERITANCE_CYCLE, {"دورة في الوراثة", "Inheritance cycle detected"}},
            {ErrorCode::RUN_ABSTRACT_INSTANTIATION, {"لا يمكن إنشاء كائن من صنف مجرد", "Cannot instantiate abstract class"}},
            // Phase 4 — Functions
            {ErrorCode::RUN_FUNCTION_NOT_FOUND, {"الدالة غير موجودة", "Function not found"}},
            {ErrorCode::RUN_MISSING_REQUIRED_ARG, {"معامل إلزامي مفقود", "Missing required argument"}},
            {ErrorCode::RUN_TOO_MANY_ARGS, {"عدد المعاملات أكثر من المطلوب", "Too many arguments"}},
            {ErrorCode::RUN_NOT_CALLABLE, {"القيمة غير قابلة للاستدعاء", "Value is not callable"}},
            {ErrorCode::RUN_LAMBDA_RETURN_INVALID, {"إرجاع لامدا غير صالح", "Invalid lambda return"}},
            // Phase 4 — Types
            {ErrorCode::RUN_OPERAND_TYPE_INVALID, {"نوع المعامل غير صالح للعملية", "Operand type invalid for operation"}},
            {ErrorCode::RUN_TYPE_CONVERSION_FAILED, {"فشل تحويل النوع", "Type conversion failed"}},
            {ErrorCode::RUN_IN_OPERATOR_RHS_INVALID, {"عامل 'في' يتطلب مصفوفة أو خريطة أو نص على اليمين", "'in' operator requires array, map or string on right side"}},
            {ErrorCode::RUN_TYPE_CHECK_FAILED, {"فشل فحص النوع", "Type check failed"}},
            // Phase 4 — Builtins
            {ErrorCode::RUN_BUILTIN_REQUIRES_ARG, {"الدالة المدمجة تتطلب معاملاً", "Builtin function requires an argument"}},
            {ErrorCode::RUN_BUILTIN_REQUIRES_FUNCTION, {"الدالة المدمجة تتطلب دالة كمعامل", "Builtin function requires a function as argument"}},
            {ErrorCode::RUN_BUILTIN_INVALID_INDEX, {"فهرس غير صالح في دالة مدمجة", "Invalid index in builtin"}},
            {ErrorCode::RUN_BUILTIN_EMPTY_OPERATION, {"لا يمكن تنفيذ هذه العملية على مجموعة فارغة", "Cannot perform this operation on empty collection"}},
            // Phase 4 — Contracts
            {ErrorCode::RUN_CONTRACT_PRECOND_FAILED, {"فشل العقد: الشرط المسبق (يتطلب)", "Contract failure: precondition (requires)"}},
            {ErrorCode::RUN_CONTRACT_POSTCOND_FAILED, {"فشل العقد: الشرط اللاحق (يضمن)", "Contract failure: postcondition (ensures)"}},
            {ErrorCode::RUN_CONTRACT_WHERE_FAILED, {"فشل قيد النوع (حيث)", "Where clause type constraint failed"}},
            // Phase 4 — Concurrency
            {ErrorCode::RUN_ASYNC_TASK_ERROR, {"خطأ في المهمة غير المتزامنة", "Async task error"}},
            {ErrorCode::RUN_GENERATOR_TYPE_INVALID, {"تعبير المولّد يتطلب قيمة قابلة للتكرار (مصفوفة أو خريطة)", "Generator expression requires iterable (array or map)"}},
            {ErrorCode::RUN_CHANNEL_OPERATION_FAILED, {"فشل عملية على القناة", "Channel operation failed"}},
            // Phase 4 — Misc
            {ErrorCode::RUN_SECURITY_VIOLATION, {"انتهاك أمني - تم انتهاك سياسة الأمان", "Security violation - policy breached"}},
            {ErrorCode::RUN_ASSERTION_FAILED, {"فشل التأكيد", "Assertion failed"}},
            {ErrorCode::RUN_PANIC, {"ذعر غير قابل للتعافي", "Unrecoverable panic"}},
            {ErrorCode::RUN_PERMISSION_DENIED, {"صلاحية مرفوضة", "Permission denied"}},
            {ErrorCode::RUN_OFFSET_OUT_OF_RANGE, {"الإزاحة خارج النطاق", "Offset out of range"}},
            {ErrorCode::RUN_USER_THROWN, {"قيمة مرمية بواسطة المستخدم بـ ارمي", "User-thrown value via 'throw'"}}};

        /**
         * @brief (AR) يحول رمز الخطأ إلى نص
         *        (EN) Converts error code to string
         */
        std::string getErrorCodeString(ErrorCode code)
        {
            auto it = errorCodeStrings.find(code);
            if (it != errorCodeStrings.end())
            {
                return it->second;
            }
            return "UNKNOWN";
        }

        /**
         * @brief (AR) يرجع وصف الخطأ باللغة المحددة
         *        (EN) Returns error description in specified language
         */
        std::string getErrorDescription(ErrorCode code, Language lang)
        {
            auto it = errorDescriptions.find(code);
            if (it != errorDescriptions.end())
            {
                switch (lang)
                {
                case Language::ARABIC:
                    return it->second.arabic;
                case Language::ENGLISH:
                    return it->second.english;
                case Language::BOTH:
                {
                    std::ostringstream oss;
                    oss << it->second.arabic << " / " << it->second.english;
                    return oss.str();
                }
                }
            }

            // (AR) في حالة عدم وجود وصف
            // (EN) If description not found
            if (lang == Language::ARABIC)
            {
                return "خطأ غير معروف";
            }
            else if (lang == Language::ENGLISH)
            {
                return "Unknown error";
            }
            else
            {
                return "خطأ غير معروف / Unknown error";
            }
        }

        /**
         * @brief (AR) يرجع اسم فئة الخطأ
         *        (EN) Returns error category name
         */
        std::string getErrorCategory(ErrorCode code)
        {
            std::string codeStr = getErrorCodeString(code);

            if (codeStr.substr(0, 3) == "LEX")
            {
                return "Lexical";
            }
            else if (codeStr.substr(0, 3) == "SYN")
            {
                return "Syntax";
            }
            else if (codeStr.substr(0, 3) == "SEM")
            {
                return "Semantic";
            }
            else if (codeStr.substr(0, 3) == "RUN")
            {
                return "Runtime";
            }

            return "Unknown";
        }

    } // namespace Errors
} // namespace Sad
