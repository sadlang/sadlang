/**
 * @file runtime_collections_catalog.cpp
 * @brief (AR) قوالب أخطاء وقت التشغيل للمصفوفات والخرائط والشرائح والصفوف.
 * @brief (EN) Runtime error templates for arrays, maps, slices, and tuples.
 */

#include "runtime_template_helpers.h"

namespace Sad
{
    namespace Errors
    {

        void registerCollectionsRuntimeTemplates(ErrorCatalog &catalog)
        {
            using EC = ErrorCode;
            using Catalog::makeRT;

            // (AR) فهرس مصفوفة خارج النطاق
            catalog.registerTemplate(EC::RUN_INDEX_OUT_OF_RANGE, makeRT(
                                                                     "RUN002",
                                                                     "فهرس خارج النطاق", "Index out of range",
                                                                     "الفهرس {index} خارج نطاق المصفوفة (الطول {length})",
                                                                     "Index {index} out of range for array (length {length})",
                                                                     "تحقّق من حدود المصفوفة قبل الوصول",
                                                                     "Check array bounds before access",
                                                                     "المصفوفات تبدأ من 0 وتنتهي عند الطول-1. أي فهرس خارج هذا المدى يُسبّب توقّفاً آمناً.",
                                                                     "Arrays are 0-indexed up to length-1. Any out-of-range index causes a safe halt.",
                                                                     "إذا (ف >= 0 و ف < طول(م))\n   متغير ع = م[ف]\nنهاية"));

            // (AR) مرجع لاشيء
            catalog.registerTemplate(EC::RUN_NULL_REFERENCE, makeRT(
                                                                 "RUN003",
                                                                 "مرجع لاشيء", "Null reference",
                                                                 "محاولة الوصول إلى لاشيء (null)",
                                                                 "Attempt to access لاشيء (null)",
                                                                 "تحقّق أن القيمة ليست لاشيء قبل استخدامها",
                                                                 "Verify value is not لاشيء before use"));

            // (AR) مفتاح غير موجود
            catalog.registerTemplate(EC::RUN_KEY_NOT_FOUND, makeRT(
                                                                "RUN005",
                                                                "مفتاح غير موجود", "Key not found",
                                                                "المفتاح '{key}' غير موجود في الخريطة",
                                                                "Key '{key}' not found in map",
                                                                "استخدم .يحتوي(م) للتحقّق قبل الوصول",
                                                                "Use .يحتوي(key) to check before access"));

            // (AR) فهرس نص خارج النطاق
            catalog.registerTemplate(EC::RUN_STRING_INDEX_OUT_OF_RANGE, makeRT(
                                                                            "RUN012",
                                                                            "فهرس نص خارج النطاق", "String index out of range",
                                                                            "فهرس النص {index} خارج النطاق (الطول {length})",
                                                                            "String index {index} out of range (length {length})",
                                                                            "تحقّق من طول النص قبل الفهرسة",
                                                                            "Check string length before indexing"));

            // (AR) فهرس النص ليس صحيحاً
            catalog.registerTemplate(EC::RUN_STRING_INDEX_NOT_INTEGER, makeRT(
                                                                           "RUN013",
                                                                           "فهرس النص يجب أن يكون رقماً صحيحاً",
                                                                           "String index must be integer",
                                                                           "وُجد فهرس من نوع '{actual}' بدلاً من رقم صحيح",
                                                                           "Found index of type '{actual}' instead of integer",
                                                                           "حوّل القيمة إلى رقم صحيح بـ رقم(ق)",
                                                                           "Convert value to integer with رقم(value)"));

            // (AR) فهرس الصف ليس رقماً
            catalog.registerTemplate(EC::RUN_TUPLE_INDEX_NOT_NUMBER, makeRT(
                                                                         "RUN014",
                                                                         "فهرس الصف يجب أن يكون رقماً",
                                                                         "Tuple index must be a number",
                                                                         "نوع الفهرس الحالي: '{actual}'",
                                                                         "Current index type: '{actual}'"));

            // (AR) فهرس المصفوفة ليس رقماً
            catalog.registerTemplate(EC::RUN_ARRAY_INDEX_NOT_NUMBER, makeRT(
                                                                         "RUN015",
                                                                         "فهرس المصفوفة يجب أن يكون رقماً",
                                                                         "Array index must be a number",
                                                                         "نوع الفهرس الحالي: '{actual}'",
                                                                         "Current index type: '{actual}'"));

            // (AR) خطوة الشريحة صفر
            catalog.registerTemplate(EC::RUN_SLICE_STEP_ZERO, makeRT(
                                                                  "RUN016",
                                                                  "خطوة الشريحة لا يمكن أن تكون صفر",
                                                                  "Slice step cannot be zero",
                                                                  "حاولت إنشاء شريحة بخطوة = 0",
                                                                  "Attempted to create a slice with step = 0",
                                                                  "اختر خطوة موجبة أو سالبة (ليست صفر)",
                                                                  "Pick a positive or negative step (not zero)"));

            // (AR) نوع غير صالح للشريحة
            catalog.registerTemplate(EC::RUN_SLICE_TYPE_INVALID, makeRT(
                                                                     "RUN017",
                                                                     "الشريحة تعمل فقط على المصفوفات والنصوص",
                                                                     "Slicing works only on arrays and strings",
                                                                     "النوع '{type}' لا يدعم عملية الشريحة",
                                                                     "Type '{type}' does not support slicing"));

            // (AR) إسناد بفهرس على نوع غير صالح
            catalog.registerTemplate(EC::RUN_INDEX_ASSIGN_TYPE_INVALID, makeRT(
                                                                            "RUN018",
                                                                            "لا يمكن الإسناد بالفهرس إلا للمصفوفات والقواميس",
                                                                            "Indexed assignment only works on arrays and dictionaries",
                                                                            "النوع '{type}' لا يدعم الإسناد بالفهرس",
                                                                            "Type '{type}' does not support indexed assignment"));

            // (AR) عملية على مجموعة فارغة
            catalog.registerTemplate(EC::RUN_EMPTY_COLLECTION, makeRT(
                                                                   "RUN019",
                                                                   "عملية على مجموعة فارغة",
                                                                   "Operation on empty collection",
                                                                   "لا يمكن تنفيذ '{operation}' على {kind} فارغة",
                                                                   "Cannot perform '{operation}' on an empty {kind}",
                                                                   "تحقّق أن المجموعة ليست فارغة قبل العملية",
                                                                   "Verify the collection is not empty before the operation"));
        }

    } // namespace Errors
} // namespace Sad
