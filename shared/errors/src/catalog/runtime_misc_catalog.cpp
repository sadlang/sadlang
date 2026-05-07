/**
 * @file runtime_misc_catalog.cpp
 * @brief (AR) قوالب أخطاء متفرقات: تأكيد، أمان، صلاحيات، إزاحة، ذعر،
 *             ومُرمَى المستخدم بـ ارمي.
 */

#include "runtime_template_helpers.h"

namespace Sad
{
    namespace Errors
    {

        void registerMiscRuntimeTemplates(ErrorCatalog &catalog)
        {
            using EC = ErrorCode;
            using Catalog::makeRT;

            catalog.registerTemplate(EC::RUN_SECURITY_VIOLATION, makeRT(
                                                                     "RUN047",
                                                                     "انتهاك أمني", "Security violation",
                                                                     "تمّ انتهاك سياسة الأمان: {reason}",
                                                                     "Security policy breached: {reason}"));

            catalog.registerTemplate(EC::RUN_ASSERTION_FAILED, makeRT(
                                                                   "RUN048",
                                                                   "فشل تأكيد", "Assertion failed",
                                                                   "فشل التأكيد: {expr}",
                                                                   "Assertion failed: {expr}",
                                                                   "راجع التعبير المُؤكَّد ومدخلاته",
                                                                   "Review the asserted expression and its inputs"));

            catalog.registerTemplate(EC::RUN_PANIC, makeRT(
                                                        "RUN049",
                                                        "ذعر غير قابل للتعافي", "Unrecoverable panic",
                                                        "ذعر: {message}",
                                                        "Panic: {message}"));

            catalog.registerTemplate(EC::RUN_PERMISSION_DENIED, makeRT(
                                                                    "RUN050",
                                                                    "صلاحية مرفوضة", "Permission denied",
                                                                    "صلاحية مرفوضة لـ '{resource}'",
                                                                    "Permission denied for '{resource}'"));

            catalog.registerTemplate(EC::RUN_OFFSET_OUT_OF_RANGE, makeRT(
                                                                      "RUN051",
                                                                      "إزاحة خارج النطاق", "Offset out of range",
                                                                      "الإزاحة {offset} خارج النطاق المسموح",
                                                                      "Offset {offset} is outside the allowed range"));

            catalog.registerTemplate(EC::RUN_USER_THROWN, makeRT(
                                                              "RUN052",
                                                              "قيمة مرمية بـ ارمي", "User-thrown value via 'throw'",
                                                              "تمّ رمي قيمة من نوع '{type}': {message}",
                                                              "Threw a value of type '{type}': {message}",
                                                              "أضف كتلة 'امسك' للتعامل مع القيمة المرمية",
                                                              "Add an 'امسك' (catch) block to handle the thrown value"));
        }

    } // namespace Errors
} // namespace Sad
