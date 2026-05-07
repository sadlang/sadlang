/**
 * @file error_catalog_init.cpp
 * @brief (AR) تسجيل القوالب الافتراضية لجميع رموز الأخطاء الأساسية
 *        (EN) Registers default templates for all core error codes
 *
 * (AR) يُستدعى من ErrorCatalog::registerDefaults الذي يستدعيه
 *      ErrorManager::initializeDefaults عند بدء البرنامج.
 *      كل رمز يحصل على عنوان (title*) و brief* كحد أدنى.
 *      detailed/teacher/fixHint اختيارية وتُملأ تدريجياً.
 *
 * (EN) Called from ErrorCatalog::registerDefaults which is invoked by
 *      ErrorManager::initializeDefaults at startup.
 *      Every code gets at minimum a title* and brief*.
 *      detailed/teacher/fixHint are optional and filled progressively.
 */

#include "error_catalog.h"
#include "catalog/runtime_template_helpers.h"

namespace Sad
{
    namespace Errors
    {

        namespace
        {

            // (AR) دالة مساعدة محلية لاختصار التسجيل
            // (EN) Local helper to keep registrations concise
            inline ErrorTemplate make(std::string id,
                                      std::string titleAr, std::string titleEn,
                                      std::string briefAr, std::string briefEn,
                                      std::string fixAr = "", std::string fixEn = "",
                                      std::string detailedAr = "", std::string detailedEn = "",
                                      std::string codeExample = "")
            {
                ErrorTemplate t;
                t.id = std::move(id);
                t.titleAr = std::move(titleAr);
                t.titleEn = std::move(titleEn);
                t.briefAr = std::move(briefAr);
                t.briefEn = std::move(briefEn);
                t.fixHintAr = std::move(fixAr);
                t.fixHintEn = std::move(fixEn);
                t.detailedAr = std::move(detailedAr);
                t.detailedEn = std::move(detailedEn);
                t.codeExample = std::move(codeExample);
                return t;
            }

        } // anonymous namespace

        void ErrorCatalog::registerDefaults()
        {
            using EC = ErrorCode;

            // ────────────────────────────────────────────────────────────
            // أخطاء معجمية | Lexical Errors
            // ────────────────────────────────────────────────────────────
            registerTemplate(EC::LEX_INVALID_CHARACTER, make(
                                                            "LEX001",
                                                            "رمز غير صالح", "Invalid character",
                                                            "رمز غير صالح '{char}' في المصدر",
                                                            "Invalid character '{char}' in source",
                                                            "احذف الرمز أو استبدله برمز صالح",
                                                            "Remove the character or replace it with a valid one"));

            registerTemplate(EC::LEX_UNTERMINATED_STRING, make(
                                                              "LEX002",
                                                              "نص غير مغلق", "Unterminated string",
                                                              "نص بدأ بـ \" ولم يُغلق", "String literal opened with \" was never closed",
                                                              "أضف \" في نهاية النص", "Add a closing \" at the end of the string"));

            registerTemplate(EC::LEX_INVALID_NUMBER, make(
                                                         "LEX003",
                                                         "صيغة رقم غير صالحة", "Invalid number format",
                                                         "الرقم '{number}' غير صالح", "Number '{number}' is invalid",
                                                         "تحقّق من بنية الرقم (نقطة عشرية أو أساس)",
                                                         "Check number format (decimal point or base)"));

            registerTemplate(EC::LEX_INVALID_ESCAPE, make(
                                                         "LEX004",
                                                         "تسلسل هروب غير صالح", "Invalid escape sequence",
                                                         "تسلسل '\\{char}' غير معروف", "Escape '\\{char}' is unknown"));

            registerTemplate(EC::LEX_NUMBER_TOO_LARGE, make(
                                                           "LEX005",
                                                           "رقم كبير جداً", "Number too large",
                                                           "الرقم يتجاوز نطاق نوع رقم", "Number exceeds رقم range"));

            registerTemplate(EC::LEX_INVALID_UTF8, make(
                                                       "LEX006",
                                                       "ترميز UTF-8 غير صالح", "Invalid UTF-8 encoding",
                                                       "بايت UTF-8 غير صالح", "Invalid UTF-8 byte sequence"));

            // ────────────────────────────────────────────────────────────
            // أخطاء نحوية | Syntax Errors
            // ────────────────────────────────────────────────────────────
            registerTemplate(EC::SYN_UNEXPECTED_TOKEN, make(
                                                           "SYN001",
                                                           "رمز غير متوقع", "Unexpected token",
                                                           "وجدتُ '{found}' لكن كنت أتوقع '{expected}'",
                                                           "Found '{found}' but expected '{expected}'"));

            registerTemplate(EC::SYN_MISSING_SEMICOLON, make(
                                                            "SYN002",
                                                            "فاصلة منقوطة مفقودة", "Missing semicolon",
                                                            "متوقع ';' أو '؛' في نهاية الجملة",
                                                            "Expected ';' at end of statement",
                                                            "أضف ؛ في نهاية السطر", "Add ; at end of line"));

            registerTemplate(EC::SYN_UNCLOSED_BRACKET, make(
                                                           "SYN003",
                                                           "قوس غير مغلق", "Unclosed bracket",
                                                           "قوس '{bracket}' فُتح ولم يُغلق",
                                                           "Bracket '{bracket}' was opened but never closed"));

            registerTemplate(EC::SYN_MISSING_IDENTIFIER, make(
                                                             "SYN004",
                                                             "معرّف مفقود", "Missing identifier",
                                                             "متوقع اسم متغير أو دالة هنا",
                                                             "Expected an identifier here"));

            registerTemplate(EC::SYN_INVALID_EXPRESSION, make(
                                                             "SYN005",
                                                             "تعبير غير صالح", "Invalid expression",
                                                             "بنية التعبير غير صحيحة", "Expression structure is invalid"));

            registerTemplate(EC::SYN_UNEXPECTED_EOF, make(
                                                         "SYN006",
                                                         "نهاية ملف غير متوقعة", "Unexpected end of file",
                                                         "انتهى الملف قبل اكتمال البنية",
                                                         "File ended before structure was complete"));

            registerTemplate(EC::SYN_MISSING_COLON, make(
                                                        "SYN007",
                                                        "نقطتان مفقودتان", "Missing colon",
                                                        "متوقع ':' هنا", "Expected ':' here"));

            registerTemplate(EC::SYN_INVALID_ASSIGNMENT, make(
                                                             "SYN008",
                                                             "إسناد غير صالح", "Invalid assignment",
                                                             "لا يمكن الإسناد إلى '{target}'",
                                                             "Cannot assign to '{target}'"));

            // ────────────────────────────────────────────────────────────
            // أخطاء دلالية | Semantic Errors
            // ────────────────────────────────────────────────────────────
            registerTemplate(EC::SEM_UNDEFINED_VARIABLE, make(
                                                             "SEM001",
                                                             "متغير غير معرَّف", "Undefined variable",
                                                             "المتغير '{name}' غير معرَّف",
                                                             "Variable '{name}' is not defined",
                                                             "هل قصدت '{suggestion}'؟ أو عرّفه أولاً بـ متغير {name} = ...",
                                                             "Did you mean '{suggestion}'? Or define it first: متغير {name} = ..."));

            registerTemplate(EC::SEM_TYPE_MISMATCH, make(
                                                        "SEM002",
                                                        "عدم تطابق الأنواع", "Type mismatch",
                                                        "متوقع '{expected}' لكن وُجد '{found}'",
                                                        "Expected '{expected}' but found '{found}'"));

            registerTemplate(EC::SEM_REDEFINITION, make(
                                                       "SEM003",
                                                       "إعادة تعريف", "Redefinition",
                                                       "'{name}' معرَّف مسبقاً",
                                                       "'{name}' is already defined"));

            registerTemplate(EC::SEM_UNDEFINED_FUNCTION, make(
                                                             "SEM004",
                                                             "دالة غير معرَّفة", "Undefined function",
                                                             "الدالة '{name}' غير معرَّفة",
                                                             "Function '{name}' is not defined"));

            registerTemplate(EC::SEM_WRONG_ARG_COUNT, make(
                                                          "SEM005",
                                                          "عدد معاملات خاطئ", "Wrong argument count",
                                                          "الدالة '{name}' تتوقّع {expected} معاملاً، لكن مُرِّر {found}",
                                                          "Function '{name}' expects {expected} arguments but got {found}"));

            registerTemplate(EC::SEM_INVALID_OPERATION, make(
                                                            "SEM006",
                                                            "عملية غير صالحة", "Invalid operation",
                                                            "لا يمكن تطبيق '{op}' على '{type}'",
                                                            "Cannot apply '{op}' to '{type}'"));

            registerTemplate(EC::SEM_CONST_ASSIGNMENT, make(
                                                           "SEM007",
                                                           "إسناد لثابت", "Assignment to constant",
                                                           "لا يمكن تعديل الثابت '{name}'",
                                                           "Cannot modify constant '{name}'",
                                                           "استخدم متغير بدل ثابت إذا احتجت تعديله",
                                                           "Use متغير instead of ثابت if you need to modify it"));

            registerTemplate(EC::SEM_UNDEFINED_CLASS, make(
                                                          "SEM008",
                                                          "صنف غير معرَّف", "Undefined class",
                                                          "الصنف '{name}' غير معرَّف",
                                                          "Class '{name}' is not defined"));

            registerTemplate(EC::SEM_PRIVATE_ACCESS, make(
                                                         "SEM009",
                                                         "وصول لعضو خاص", "Access to private member",
                                                         "العضو '{name}' خاص ولا يمكن الوصول إليه من الخارج",
                                                         "Member '{name}' is private and cannot be accessed externally"));

            // ────────────────────────────────────────────────────────────
            // أخطاء وقت التشغيل | Runtime Errors (Phase 4)
            // (AR) القوالب موزّعة على 9 ملفات في مجلد catalog/ حسب الفئة
            //      الدلالية. النظام القديم لرسائل المفسر مَحذوف نهائياً.
            // (EN) Templates are split across 9 files in catalog/ by
            //      semantic category. The legacy interpreter message
            //      system is fully removed.
            // ────────────────────────────────────────────────────────────
            registerArithmeticRuntimeTemplates(*this);
            registerCollectionsRuntimeTemplates(*this);
            registerOopRuntimeTemplates(*this);
            registerFunctionsRuntimeTemplates(*this);
            registerTypesRuntimeTemplates(*this);
            registerBuiltinsRuntimeTemplates(*this);
            registerContractsRuntimeTemplates(*this);
            registerConcurrencyRuntimeTemplates(*this);
            registerMiscRuntimeTemplates(*this);

            // (AR) قوالب وقت تشغيل غير مرتبطة بالمفسر (تبقى من النظام القديم)
            // (EN) Runtime templates not tied to the interpreter (legacy keep)
            registerTemplate(EC::RUN_STACK_OVERFLOW, make(
                                                         "RUN004",
                                                         "تجاوز سعة المكدس", "Stack overflow",
                                                         "تجاوز الحد الأقصى لعمق الاستدعاءات (تكرار غير منته؟)",
                                                         "Maximum call depth exceeded (infinite recursion?)"));

            registerTemplate(EC::RUN_FILE_ERROR, make(
                                                     "RUN007",
                                                     "خطأ في الملف", "File error",
                                                     "تعذّر الوصول إلى الملف '{path}': {reason}",
                                                     "Cannot access file '{path}': {reason}"));

            registerTemplate(EC::RUN_IMPORT_ERROR, make(
                                                       "RUN008",
                                                       "خطأ في الاستيراد", "Import error",
                                                       "تعذّر استيراد '{module}': {reason}",
                                                       "Cannot import '{module}': {reason}"));
        }

    } // namespace Errors
} // namespace Sad
