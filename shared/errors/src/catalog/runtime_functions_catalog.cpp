/**
 * @file runtime_functions_catalog.cpp
 * @brief (AR) قوالب أخطاء وقت التشغيل المتعلقة بالدوال: الدالة غير موجودة،
 *             معامل مفقود، معاملات زائدة، قيمة غير قابلة للاستدعاء، لامدا.
 */

#include "runtime_template_helpers.h"

namespace Sad
{
    namespace Errors
    {

        void registerFunctionsRuntimeTemplates(ErrorCatalog &catalog)
        {
            using EC = ErrorCode;
            using Catalog::makeRT;

            catalog.registerTemplate(EC::RUN_FUNCTION_NOT_FOUND, makeRT(
                                                                     "RUN028",
                                                                     "الدالة غير موجودة", "Function not found",
                                                                     "الدالة '{function}' غير موجودة",
                                                                     "Function '{function}' not found",
                                                                     "تأكد من تعريف الدالة أو استيرادها قبل الاستدعاء",
                                                                     "Define or import the function before calling"));

            catalog.registerTemplate(EC::RUN_MISSING_REQUIRED_ARG, makeRT(
                                                                       "RUN029",
                                                                       "معامل إلزامي مفقود", "Missing required argument",
                                                                       "المعامل الإلزامي '{param}' مفقود في استدعاء '{function}'",
                                                                       "Required parameter '{param}' missing in call to '{function}'",
                                                                       "أضف القيمة المطلوبة للمعامل أو حدّد قيمة افتراضية في التعريف",
                                                                       "Pass the required value or give the parameter a default in the definition"));

            catalog.registerTemplate(EC::RUN_TOO_MANY_ARGS, makeRT(
                                                                "RUN030",
                                                                "معاملات زائدة", "Too many arguments",
                                                                "تم تمرير {actual} معامل لكن '{function}' يقبل {expected} فقط",
                                                                "Passed {actual} arguments but '{function}' accepts only {expected}",
                                                                "احذف المعاملات الإضافية أو وسّع توقيع الدالة",
                                                                "Remove extra arguments or widen the function signature"));

            catalog.registerTemplate(EC::RUN_NOT_CALLABLE, makeRT(
                                                               "RUN031",
                                                               "القيمة غير قابلة للاستدعاء",
                                                               "Value is not callable",
                                                               "حاولت استدعاء قيمة من نوع '{type}' وهي ليست دالة",
                                                               "Attempted to call a value of type '{type}' which is not a function"));

            catalog.registerTemplate(EC::RUN_LAMBDA_RETURN_INVALID, makeRT(
                                                                        "RUN032",
                                                                        "إرجاع لامدا غير صالح", "Invalid lambda return",
                                                                        "تعبير اللامدا أرجع قيمة غير صالحة",
                                                                        "Lambda expression returned an invalid value"));
        }

    } // namespace Errors
} // namespace Sad
