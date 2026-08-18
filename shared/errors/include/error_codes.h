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

            // (AR) SYN009+ — رموز ترحيل عائلة المحلّل من errorBilingual إلى الكتالوج المركزي
            // (EN) SYN009+ — parser-family migration codes (errorBilingual → central catalog)
            SYN_EXPECTED_EXPRESSION,      ///< SYN009: (AR) تعبير متوقّع مفقود / (EN) Expected expression missing
            SYN_EXPECTED_NAME,            ///< SYN010: (AR) اسم متوقّع مفقود / (EN) Expected name missing
            SYN_EXPECTED_KEYWORD,         ///< SYN011: (AR) كلمة مفتاحية متوقّعة مفقودة / (EN) Expected keyword missing
            SYN_EXPECTED_SYMBOL,          ///< SYN012: (AR) رمز متوقّع مفقود / (EN) Expected symbol missing
            SYN_UNCLOSED_CONSTRUCT,       ///< SYN013: (AR) بنية غير مغلقة / (EN) Unclosed construct
            SYN_REMOVED_SYNTAX,           ///< SYN014: (AR) صيغة أُزيلت من اللغة / (EN) Removed syntax
            SYN_ADJECTIVE_ORDER,          ///< SYN015: (AR) ترتيب الصفة والموصوف / (EN) Adjective order
            SYN_NAME_HAS_SPACE,           ///< SYN016: (AR) اسم يحتوي على مسافة / (EN) Name contains a space
            SYN_RETURN_TYPE_AFTER_PARAMS, ///< SYN017: (AR) نوع الإرجاع بعد المعاملات / (EN) Return type after parameters
            SYN_EMPTY_CONSTRUCT,          ///< SYN018: (AR) بنية فارغة / (EN) Empty construct
            SYN_UNKNOWN_ELEMENT,          ///< SYN019: (AR) عنصر غير معروف / (EN) Unknown element
            SYN_INVALID_CONSTRUCT_FORM,   ///< SYN020: (AR) بنية مخالفة للصيغة / (EN) Construct violates expected form
            SYN_PARSE_UNKNOWN_ERROR,      ///< SYN021: (AR) خطأ تحليل غير معروف / (EN) Unknown parse error
            SYN_ADJECTIVE_GENDER,         ///< SYN022: (AR) مطابقة الصفة للموصوف جنسًا / (EN) Adjective gender agreement
            SYN_DECL_NOT_ALLOWED_HERE,    ///< SYN023: (AR) تصريح غير مسموح في هذا السياق / (EN) Declaration not allowed in this context
            SYN_DOC_ORPHAN,               ///< SYN024: (AR) توثيق يتيم لا يلتصق بتصريح / (EN) Orphan doc comment not attached to a declaration
            SYN_DOC_TRAILING,             ///< SYN025: (AR) توثيق في ذيل سطر كود / (EN) Doc comment trailing a code line
            SYN_DECORATOR_NOT_ALLOWED,    ///< SYN026: (AR) مُزخرِف في موضع لا يقبله / (EN) Decorator in a position that rejects it
            SYN_RESERVED_AS_DECL_NAME,    ///< SYN027: (AR) كلمة محجوزة في موضع اسم التصريح / (EN) Reserved keyword in a declaration-name position
            SYN_PARENS_FORM_REMOVED,      ///< SYN028: (AR) صيغة الأقواس أُزيلت من هذه البنية / (EN) Parenthesised form removed
            SYN_FOR_RANGE_MISSING_TO,     ///< SYN029: (AR) حدّ النهاية مفقود في حلقة المدى / (EN) Missing range terminator
            SYN_FOR_MISSING_ITERATOR_KEYWORD, ///< SYN030: (AR) كلمة التكرار مفقودة بعد متغيّر الحلقة / (EN) Missing iteration keyword
            SYN_MATCH_WRONG_ARM_KEYWORD,  ///< SYN031: (AR) كلمة خاطئة في موضع ذراع المطابقة / (EN) Wrong keyword in a match-arm position
            SYN_DEFAULT_CLAUSE_NOT_LAST,  ///< SYN032: (AR) بند «افتراضي» ليس آخرَ البنود في «حالة»/«طابق» / (EN) The default clause is not last in a switch/match

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

            // (AR) EM: رموز محدَّدة لترحيل reportError اليدوية (السابقة العامة) — SEM010+
            SEM_ONLY_EQUALITY_ALLOWED,    ///< SEM010: (AR) لا تُدعَم إلا == و!= / (EN) Only equality allowed
            SEM_INDEXING_NOT_SUPPORTED,   ///< SEM011: (AR) الفهرسة غير مدعومة / (EN) Indexing not supported
            SEM_INVALID_NUMBER_LITERAL,   ///< SEM012: (AR) ثابت عددي غير صالح / (EN) Invalid number literal
            SEM_CONTROL_OUTSIDE_LOOP,     ///< SEM013: (AR) تحكّم خارج حلقة / (EN) Control outside loop
            SEM_YIELD_OUTSIDE_GENERATOR,  ///< SEM014: (AR) أنتج خارج مولّد / (EN) yield outside generator
            SEM_THIS_SUPER_UNAVAILABLE,   ///< SEM015: (AR) this/super غير متاح / (EN) this/super unavailable
            SEM_EMPTY_FUNCTION_BODY,      ///< SEM016: (AR) جسم دالة فارغ / (EN) Empty function body
            SEM_COMPARE_DIFFERENT_TYPES,  ///< SEM017: (AR) مقارنة أنواع مختلفة / (EN) Comparing different types
            SEM_MAIN_FUNCTION_RULE,       ///< SEM018: (AR) قاعدة الدالة الرئيسية / (EN) Main function rule
            SEM_FREESTANDING_BUILTIN,     ///< SEM019: (AR) مدمجة غير متاحة في الوضع الحرّ / (EN) Builtin unavailable in freestanding
            SEM_FREESTANDING_SYS_BUILTIN_ARG, ///< SEM020: (AR) معامل مدمجة نظام غير صالح في الوضع الحرّ / (EN) Invalid system-builtin argument in freestanding
            SEM_SYMBOL_ADDR_ARG,          ///< SEM021: (AR) وسيط عنوان_رمز يجب أن يكون اسم رمز ثابتًا / (EN) symbol-address argument must be a constant symbol name
            SEM_SYMBOL_NAME_CONFLICT,     ///< SEM022: (AR) اسم رمز @رمز مُصدَّر متصادم أو محجوز / (EN) exported @رمز symbol name conflicts or is reserved
            SEM_STATIC_ARRAY_SIZE,        ///< SEM023: (AR) حجم المصفوفة الساكنة يجب أن يكون عددًا صحيحًا موجبًا / (EN) static array size must be a positive integer
            SEM_DUP_EXPORT_SYMBOL,        ///< SEM024: (AR) اسم رمز مُصدَّر مكرَّر عبر @رمز على الدوالّ / (EN) duplicate exported symbol name via @رمز on functions
            SEM_ASM_UNKNOWN_MNEMONIC,     ///< SEM025: (AR) منمنمة تجميع غير معجميّة / (EN) unknown assembly mnemonic (dialect)
            SEM_ASM_OPERAND_COUNT,        ///< SEM026: (AR) عدد معاملات منمنمة تجميع مخالف للمعجم / (EN) assembly mnemonic operand-count mismatch
            SEM_DIALECT_COMPILER_ONLY,    ///< SEM027: (AR) كتلة لهجة متاحة في المترجم فقط / (EN) dialect block available in the compiler only
            SEM_ASM_MULTIPLE_OUTPUTS,     ///< SEM028: (AR) أكثر من متغيّر ص مكتوب في كتلة تجميع (م١) / (EN) more than one written sad var in an asm block (M1)
            SEM_ASM_UNDEFINED_LABEL,      ///< SEM029: (AR) لصيقة/هدف قفز غير معرَّف في كتلة التجميع / (EN) undefined jump label/target in an asm block
            SEM_ASM_DUPLICATE_LABEL,      ///< SEM030: (AR) لصيقة مكرَّرة في كتلة التجميع / (EN) duplicate label in an asm block
            SEM_ASM_UNDEFINED_VAR,        ///< SEM031: (AR) متغيّر ص غير معرَّف داخل {…} في كتلة التجميع / (EN) undefined sad var in {…} in an asm block
            SEM_ASM_MEMORY_FORM,          ///< SEM032: (AR) صيغة عنونة غير مدعومة في كتلة التجميع (م١) / (EN) unsupported memory-addressing form in an asm block (M1)
            SEM_ASM_UNKNOWN_REGISTER,     ///< SEM033: (AR) سجلّ تجميع غير معجميّ / (EN) unknown assembly register (dialect)
            SEM_ASM_WRITES_SOURCE_VAR,    ///< SEM034: (AR) متغيّر ص في موضع مصدر منمنمة تكتب مصدرها (بادل، م١) / (EN) sad var in the source position of a source-writing mnemonic (xchg, M1)
            SEM_ASM_INVALID_PREFIX_OPERAND, ///< SEM035: (AR) منمنمة لا تصلح تابعةً لبادئة تكرار — كتاباتها الضمنيّة تتجاوز البادئة / (EN) mnemonic invalid as a repeat-prefix operand (implicit writes exceed the prefix's)
            SEM_NON_EXHAUSTIVE_MATCH,     ///< SEM036: (AR) مطابقة تعداد بحمولة غير مستنفِدة (معامل غير مُغطّى بلا فرع شامل) / (EN) non-exhaustive tagged-enum match (uncovered variant, no catch-all)
            SEM_FREESTANDING_HEAP_TOO_LARGE, ///< SEM037: (AR) حجم الكومة الحرّة يجاوز فضاء عنونة الهدف — حقل حجم قسم ELF32 يلتفّ صامتًا / (EN) freestanding heap exceeds the target address space — the ELF32 section-size field wraps silently
            SEM_TARGET_ARCH_UNSUPPORTED_BUILTIN, ///< SEM038: (AR) مدمجة تُخفَّض إلى تعليمة من عائلة معالج أخرى (rdtsc/outb/mov %crN لهدف ARM) / (EN) builtin lowers to a foreign CPU family's instruction
            SEM_STATIC_LOCAL_UNSUPPORTED, ///< SEM039: (AR) «متغير ساكن» داخل دالّة — مدّة تخزين ساكنة غير منفَّذة في المحرّكين / (EN) function-local static storage duration is not implemented
            SEM_VOID_NOT_A_VALUE_TYPE,    ///< SEM040: (AR) «فراغ» نوعُ إرجاعٍ لا نوعُ قيمة — لا يصلح لخانةٍ تُخزَّن (ISSUE-113) / (EN) void is a return type, not a value type
            SEM_IMPLICIT_CTOR_REQUIRES_ARGS, ///< SEM041: (AR) إنشاءٌ ضمنيٌّ لصنفٍ بانيه يشترط وسائط — «شخص ك» بلا تهيئة / (EN) implicit construction of a class whose constructor requires arguments
            SEM_COMPILER_FIELD_TYPE_UNSUPPORTED, ///< SEM042: (AR) حقلٌ نوعُه صنفٌ لا يخفضه المترجّمُ بعد — رفضٌ صريحٌ بدل بناءٍ ينهار / (EN) class-typed field not lowered yet — explicit rejection instead of a crashing build
            SEM_OPTIONAL_CALL_RECEIVER_NOT_SIMPLE, ///< SEM043: (AR) الوصولُ الآمنُ بنداءِ طريقةٍ يلزمه مستقبِلٌ بسيطٌ — أثرٌ جانبيٌّ مضاعَفٌ لا يُرى في مخرَجٍ صحيح / (EN) safe method call needs a simple receiver — a duplicated side effect no correct output reveals

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

            // (AR) EM: رموز محدَّدة لترحيل reportError اليدوية — RUN053+
            RUN_NUMERIC_REQUIRED,          ///< RUN053: (AR) تتطلّب قيمة رقمية / (EN) Numeric required
            RUN_COMPREHENSION_NEEDS_ARRAY, ///< RUN054: (AR) الاستيعاب يتطلّب مصفوفة / (EN) Comprehension needs array
            RUN_NOT_ITERABLE,              ///< RUN055: (AR) نوع غير قابل للتكرار / (EN) Not iterable
            RUN_NULL_ASSERTION,            ///< RUN056: (AR) فشل تأكيد عدم الفراغ (مؤكَّد) / (EN) Null assertion failed
            RUN_UI_LAUNCH_FAILED,          ///< RUN057: (AR) فشل تشغيل تطبيق الواجهة / (EN) UI application launch failed
            RUN_UI_STATE_ERROR,            ///< RUN058: (AR) خطأ في عمليّة الحالة / (EN) State operation error

            // ════════════════════════════════════════════════════════════════
            // (AR) الربط الخارجي (FFI) — قدرات غير مدعومة في المفسّر.
            // (EN) Foreign function interface — capabilities unsupported in interpreter.
            // ════════════════════════════════════════════════════════════════
            RUN_EXTERN_NOT_SUPPORTED, ///< RUN059: (AR) دالة خارجية غير مدعومة في المفسّر / (EN) Extern function unsupported in interpreter
            RUN_FFI_CALL_UNSAFE,      ///< RUN060: (AR) استدعاء ربط خارجي بلا ترتيب وسائط / (EN) FFI call without argument marshalling

            // (AR) التعابير النمطيّة / (EN) Regular expressions
            RUN_REGEX_UNKNOWN_FLAG,   ///< RUN061: (AR) رايةُ تعبيرٍ نمطيّ مجهولة / (EN) Unknown regex flag

            // ════════════════════════════════════════════════════════════════
            // (AR) بدائيّات العمليّات (وحدة عمليات) — fork/execvp، waitpid، pipe،
            //      close، open. كلّها تفشل صراحةً: صدَفةٌ تبني «&&» على رمزِ خروجٍ
            //      مختلَقٍ أسوأ من صدَفةٍ تتوقّف.
            // (EN) Process primitives (عمليات module). All fail loudly: a shell
            //      branching on a fabricated exit status is worse than one that stops.
            // ════════════════════════════════════════════════════════════════
            RUN_PROC_SPAWN_FAILED,          ///< RUN062: (AR) تعذّر تشغيل البرنامج / (EN) Failed to spawn program
            RUN_PROC_NOT_CHILD,             ///< RUN063: (AR) انتظار عمليّة ليست ابنًا / (EN) Waiting on a non-child
            RUN_PROC_BAD_DESCRIPTOR,        ///< RUN064: (AR) وصف ملفّ غير صالح / (EN) Invalid file descriptor
            RUN_PROC_BAD_MODE,              ///< RUN065: (AR) وضع فتح مجهول / (EN) Unknown open mode
            RUN_PROC_UNSUPPORTED_PLATFORM,  ///< RUN066: (AR) غير مدعومة على هذه المنصّة / (EN) Unsupported platform
            RUN_PROC_OPEN_FAILED,           ///< RUN067: (AR) تعذّر فتح وصف ملفّ / (EN) Failed to open descriptor
            RUN_PROC_PIPE_FAILED,           ///< RUN068: (AR) تعذّر إنشاء أنبوب / (EN) Failed to create pipe
            RUN_PROC_WAIT_FAILED,           ///< RUN069: (AR) فشل انتظار العمليّة / (EN) Waiting on process failed
            RUN_PROC_BAD_STREAM_KEY,        ///< RUN070: (AR) مفتاح مجرًى غير صالح / (EN) Invalid stream key
            RUN_PROC_REDIRECT_FAILED,       ///< RUN071: (AR) فشل توجيه المجاري في الابن / (EN) Child redirection failed
            RUN_PROC_CLOSE_FAILED,          ///< RUN072: (AR) فشل إغلاق الوصف / (EN) Closing the descriptor failed
            RUN_PROC_HANDLE_KIND,           ///< RUN073: (AR) نوع المقبض غير متوقّع / (EN) Wrong handle kind

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
            INT_INTERP_SCOPE_STACK,         ///< INT009: (AR) خلل مكدّس النطاقات / (EN) Scope stack violation
            INT_INTERP_UNEXPECTED_TOKEN,    ///< INT010: (AR) نوع رمز حرفي غير متوقَّع / (EN) Unexpected literal token type
            INT_MODULE_VERIFY,              ///< INT011: (AR) فشل التحقّق من وحدة LLVM / (EN) LLVM module verification failed
            INT_NATIVE_NO_ENTRY,            ///< INT012: (AR) لا دالّة دخول في التخفيض الأصليّ / (EN) No entry function in native lowering
            INT_NATIVE_UNSUPPORTED,         ///< INT013: (AR) بنية غير مدعومة في التخفيض الأصليّ الأدنى / (EN) Unsupported construct in minimal native lowering
            INT_NATIVE_REGALLOC_EXHAUSTED,  ///< INT014: (AR) نفاد سجلّات الحوض (بلا انسكاب) / (EN) Register pool exhausted (no spilling)
            INT_NATIVE_IMM_RANGE,           ///< INT015: (AR) فوريّ خارج المدى في التخفيض الأصليّ / (EN) Immediate out of range in native lowering
            INT_NATIVE_ENCODING_MISSING,    ///< INT016: (AR) لا صيغة ترميز أصليّة للتعليمة / (EN) No native encoding spec for instruction
            INT_NATIVE_UNDEF_VREG,          ///< INT017: (AR) سجلّ افتراضيّ غير معرَّف في التخفيض الأصليّ / (EN) Undefined virtual register in native lowering
            INT_NATIVE_LABEL_UNDEFINED,     ///< INT018: (AR) لصيقةُ قفزٍ لا تشير إلى كتلةٍ معرَّفة / (EN) Branch label targets an undefined block
            INT_NATIVE_OUTPUT_WRITE,        ///< INT019: (AR) تعذّرت كتابة ثنائيّ ELF64 إلى مسار المخرَج / (EN) Could not write the ELF64 binary to the output path
            INT_NATIVE_TARGET_UNSUPPORTED,  ///< INT020: (AR) هدفٌ لا مخفّضَ له أو لا يستعمل ELF / (EN) Target with no lowerer or not ELF-based
            INT_NATIVE_OUTPUT_TYPE,         ///< INT021: (AR) نوعُ مخرَجٍ لا تنتجه الخلفيّة الأصليّة / (EN) Output type the native backend does not produce
            INT_NATIVE_NO_MODULE            ///< INT022: (AR) بلوغُ الخلفيّة بلا وحدةِ SIR / (EN) Reached the backend with no SIR module
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
