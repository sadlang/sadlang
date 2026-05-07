/**
 * @file runtime_contracts_catalog.cpp
 * @brief (AR) قوالب أخطاء العقود البرمجية: يتطلب، يضمن، حيث.
 */

#include "runtime_template_helpers.h"

namespace Sad
{
    namespace Errors
    {

        void registerContractsRuntimeTemplates(ErrorCatalog &catalog)
        {
            using EC = ErrorCode;
            using Catalog::makeRT;

            catalog.registerTemplate(EC::RUN_CONTRACT_PRECOND_FAILED, makeRT(
                                                                          "RUN041",
                                                                          "فشل الشرط المسبق", "Precondition failed",
                                                                          "فشل الشرط المسبق رقم {index} في الدالة '{function}': {expr}",
                                                                          "Precondition #{index} failed in function '{function}': {expr}",
                                                                          "تحقّق من قيم المُدخلات قبل استدعاء الدالة",
                                                                          "Validate inputs before calling the function",
                                                                          "العقد البرمجي (يتطلب) يفرض قيوداً على المُدخلات قبل التنفيذ. خرقها يعني استدعاءً غير صحيح.",
                                                                          "A precondition (يتطلب) imposes input constraints before execution. Violating it means an incorrect call."));

            catalog.registerTemplate(EC::RUN_CONTRACT_POSTCOND_FAILED, makeRT(
                                                                           "RUN042",
                                                                           "فشل الشرط اللاحق", "Postcondition failed",
                                                                           "فشل الشرط اللاحق رقم {index} في الدالة '{function}': {expr}",
                                                                           "Postcondition #{index} failed in function '{function}': {expr}",
                                                                           "راجع منطق الدالة لضمان تحقيق الشرط بعد التنفيذ",
                                                                           "Review function logic to ensure the postcondition holds"));

            catalog.registerTemplate(EC::RUN_CONTRACT_WHERE_FAILED, makeRT(
                                                                        "RUN043",
                                                                        "فشل قيد النوع (حيث)", "Where constraint failed",
                                                                        "فشل قيد النوع '{constraint}' في '{function}'",
                                                                        "Type constraint '{constraint}' failed in '{function}'"));
        }

    } // namespace Errors
} // namespace Sad
