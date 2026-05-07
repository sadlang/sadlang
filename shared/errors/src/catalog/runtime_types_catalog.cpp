/**
 * @file runtime_types_catalog.cpp
 * @brief (AR) قوالب أخطاء وقت التشغيل المتعلقة بالأنواع: نوع المعامل،
 *             تحويل النوع، عامل 'في'، فحص النوع.
 */

#include "runtime_template_helpers.h"

namespace Sad
{
    namespace Errors
    {

        void registerTypesRuntimeTemplates(ErrorCatalog &catalog)
        {
            using EC = ErrorCode;
            using Catalog::makeRT;

            catalog.registerTemplate(EC::RUN_INVALID_CAST, makeRT(
                                                               "RUN006",
                                                               "تحويل نوع غير صالح", "Invalid type cast",
                                                               "لا يمكن تحويل القيمة من '{from}' إلى '{to}'",
                                                               "Cannot cast value from '{from}' to '{to}'"));

            catalog.registerTemplate(EC::RUN_OPERAND_TYPE_INVALID, makeRT(
                                                                       "RUN033",
                                                                       "نوع المعامل غير صالح", "Invalid operand type",
                                                                       "نوع المعامل '{operand}' غير صالح للعملية '{operator}'",
                                                                       "Operand type '{operand}' is invalid for operator '{operator}'",
                                                                       "حوّل القيم إلى نوع متوافق قبل العملية",
                                                                       "Convert operands to a compatible type before the operation"));

            catalog.registerTemplate(EC::RUN_TYPE_CONVERSION_FAILED, makeRT(
                                                                         "RUN034",
                                                                         "فشل تحويل النوع", "Type conversion failed",
                                                                         "فشل تحويل '{value}' من '{from}' إلى '{to}'",
                                                                         "Failed to convert '{value}' from '{from}' to '{to}'"));

            catalog.registerTemplate(EC::RUN_IN_OPERATOR_RHS_INVALID, makeRT(
                                                                          "RUN035",
                                                                          "عامل 'في' يحتاج مجموعة في اليمين",
                                                                          "'in' operator requires a collection on RHS",
                                                                          "عامل 'في' يتطلب مصفوفة أو خريطة أو نصاً، وُجد '{type}'",
                                                                          "'in' operator requires array/map/string, found '{type}'",
                                                                          "غيّر الجانب الأيمن إلى مصفوفة أو خريطة أو نص",
                                                                          "Change the right-hand side to an array, map, or string"));

            catalog.registerTemplate(EC::RUN_TYPE_CHECK_FAILED, makeRT(
                                                                    "RUN036",
                                                                    "فشل فحص النوع", "Type check failed",
                                                                    "القيمة من نوع '{actual}' لا تطابق النوع المتوقع '{expected}'",
                                                                    "Value of type '{actual}' does not match expected type '{expected}'"));
        }

    } // namespace Errors
} // namespace Sad
