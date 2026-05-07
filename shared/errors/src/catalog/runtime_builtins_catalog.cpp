/**
 * @file runtime_builtins_catalog.cpp
 * @brief (AR) قوالب أخطاء الدوال المدمجة (اضف، احذف، خريطة، رشح، اختزل، ...).
 */

#include "runtime_template_helpers.h"

namespace Sad
{
    namespace Errors
    {

        void registerBuiltinsRuntimeTemplates(ErrorCatalog &catalog)
        {
            using EC = ErrorCode;
            using Catalog::makeRT;

            catalog.registerTemplate(EC::RUN_BUILTIN_REQUIRES_ARG, makeRT(
                                                                       "RUN037",
                                                                       "الدالة المدمجة تتطلب معاملاً",
                                                                       "Builtin function requires an argument",
                                                                       "الدالة المدمجة '{builtin}' تتطلب {expected} معاملاً (تمّ تمرير {actual})",
                                                                       "Builtin '{builtin}' requires {expected} argument(s) (got {actual})",
                                                                       "أضف المعامل الناقص حسب توقيع الدالة المدمجة",
                                                                       "Add the missing argument as per the builtin signature"));

            catalog.registerTemplate(EC::RUN_BUILTIN_REQUIRES_FUNCTION, makeRT(
                                                                            "RUN038",
                                                                            "الدالة المدمجة تتطلب دالة كمعامل",
                                                                            "Builtin requires a function argument",
                                                                            "الدالة المدمجة '{builtin}' تحتاج دالة أو اسم دالة",
                                                                            "Builtin '{builtin}' needs a function or function name",
                                                                            "مرّر اسم دالة أو لامدا كمعامل",
                                                                            "Pass a function name or a lambda as the argument"));

            catalog.registerTemplate(EC::RUN_BUILTIN_INVALID_INDEX, makeRT(
                                                                        "RUN039",
                                                                        "فهرس غير صالح في دالة مدمجة",
                                                                        "Invalid index in builtin",
                                                                        "الدالة المدمجة '{builtin}' تلقّت فهرساً غير صالح: {index}",
                                                                        "Builtin '{builtin}' received invalid index: {index}"));

            catalog.registerTemplate(EC::RUN_BUILTIN_EMPTY_OPERATION, makeRT(
                                                                          "RUN040",
                                                                          "عملية مدمجة على مجموعة فارغة",
                                                                          "Builtin operation on empty collection",
                                                                          "لا يمكن تنفيذ '{builtin}' على {kind} فارغة",
                                                                          "Cannot perform '{builtin}' on an empty {kind}",
                                                                          "تحقّق أن المجموعة ليست فارغة قبل الاستدعاء",
                                                                          "Verify the collection is not empty before calling"));
        }

    } // namespace Errors
} // namespace Sad
