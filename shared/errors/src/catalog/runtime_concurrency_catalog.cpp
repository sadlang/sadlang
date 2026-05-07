/**
 * @file runtime_concurrency_catalog.cpp
 * @brief (AR) قوالب أخطاء التزامن: المهام غير المتزامنة، المولّدات، القنوات.
 */

#include "runtime_template_helpers.h"

namespace Sad
{
    namespace Errors
    {

        void registerConcurrencyRuntimeTemplates(ErrorCatalog &catalog)
        {
            using EC = ErrorCode;
            using Catalog::makeRT;

            catalog.registerTemplate(EC::RUN_ASYNC_TASK_ERROR, makeRT(
                                                                   "RUN044",
                                                                   "خطأ في مهمة غير متزامنة", "Async task error",
                                                                   "فشل تنفيذ المهمة غير المتزامنة: {reason}",
                                                                   "Async task failed: {reason}"));

            catalog.registerTemplate(EC::RUN_GENERATOR_TYPE_INVALID, makeRT(
                                                                         "RUN045",
                                                                         "نوع المولّد غير صالح", "Invalid generator type",
                                                                         "تعبير المولّد يتطلب قيمة قابلة للتكرار، وُجد '{type}'",
                                                                         "Generator expression requires an iterable, found '{type}'",
                                                                         "مرّر مصفوفة أو خريطة أو نصاً للمولّد",
                                                                         "Pass an array, map, or string to the generator"));

            catalog.registerTemplate(EC::RUN_CHANNEL_OPERATION_FAILED, makeRT(
                                                                           "RUN046",
                                                                           "فشل عملية على قناة", "Channel operation failed",
                                                                           "فشلت عملية '{operation}' على القناة: {reason}",
                                                                           "Channel operation '{operation}' failed: {reason}"));
        }

    } // namespace Errors
} // namespace Sad
