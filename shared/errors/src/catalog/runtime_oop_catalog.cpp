/**
 * @file runtime_oop_catalog.cpp
 * @brief (AR) قوالب أخطاء وقت التشغيل الكائنية: الصنف، الطريقة، الباني،
 *             الصنف الأساسي، الكائن، الخاصية، الوراثة، التجريد.
 * @brief (EN) OOP runtime templates: class, method, constructor, base
 *             class, object, property, inheritance, abstract.
 */

#include "runtime_template_helpers.h"

namespace Sad
{
    namespace Errors
    {

        void registerOopRuntimeTemplates(ErrorCatalog &catalog)
        {
            using EC = ErrorCode;
            using Catalog::makeRT;

            catalog.registerTemplate(EC::RUN_CLASS_NOT_FOUND, makeRT(
                                                                  "RUN020",
                                                                  "الصنف غير موجود", "Class not found",
                                                                  "الصنف '{class}' غير موجود",
                                                                  "Class '{class}' not found",
                                                                  "تأكد من تعريف الصنف أو استيراده قبل الاستخدام",
                                                                  "Make sure the class is defined or imported before use"));

            catalog.registerTemplate(EC::RUN_BASE_CLASS_NOT_FOUND, makeRT(
                                                                       "RUN021",
                                                                       "الصنف الأساسي غير موجود", "Base class not found",
                                                                       "الصنف الأساسي '{base}' غير موجود للصنف '{class}'",
                                                                       "Base class '{base}' not found for class '{class}'",
                                                                       "تأكد من تعريف الصنف الأساسي قبل الوراثة منه",
                                                                       "Ensure the base class is defined before inheriting"));

            catalog.registerTemplate(EC::RUN_METHOD_NOT_FOUND, makeRT(
                                                                   "RUN022",
                                                                   "الطريقة غير موجودة", "Method not found",
                                                                   "الطريقة '{method}' غير موجودة في الصنف '{class}'",
                                                                   "Method '{method}' not found in class '{class}'",
                                                                   "راجع اسم الطريقة وحالة الأحرف، أو عرّفها داخل الصنف",
                                                                   "Check method name and case, or define it in the class"));

            catalog.registerTemplate(EC::RUN_OBJECT_WITHOUT_CLASS, makeRT(
                                                                       "RUN023",
                                                                       "كائن بدون معلومات صنف",
                                                                       "Object without class info",
                                                                       "الكائن المُستهدَف لا يحمل معلومات الصنف، لا يمكن استدعاء '{method}'",
                                                                       "Target object has no class info, cannot invoke '{method}'"));

            catalog.registerTemplate(EC::RUN_CONSTRUCTOR_FAILED, makeRT(
                                                                     "RUN024",
                                                                     "فشل تنفيذ الباني", "Constructor failed",
                                                                     "فشل بناء كائن من الصنف '{class}': {reason}",
                                                                     "Failed to construct object of class '{class}': {reason}"));

            catalog.registerTemplate(EC::RUN_PROPERTY_NOT_FOUND, makeRT(
                                                                     "RUN025",
                                                                     "خاصية غير موجودة", "Property not found",
                                                                     "الخاصية '{property}' غير موجودة في '{class}'",
                                                                     "Property '{property}' not found in '{class}'"));

            catalog.registerTemplate(EC::RUN_INHERITANCE_CYCLE, makeRT(
                                                                    "RUN026",
                                                                    "دورة في الوراثة", "Inheritance cycle",
                                                                    "اكتُشفت دورة في تسلسل وراثة الصنف '{class}'",
                                                                    "Inheritance cycle detected in class '{class}'",
                                                                    "أزل الوراثة الدائرية بإعادة تصميم تسلسل الأصناف",
                                                                    "Remove circular inheritance by redesigning class hierarchy"));

            catalog.registerTemplate(EC::RUN_ABSTRACT_INSTANTIATION, makeRT(
                                                                         "RUN027",
                                                                         "لا يمكن إنشاء كائن من صنف مجرد",
                                                                         "Cannot instantiate abstract class",
                                                                         "الصنف '{class}' مُجرّد ولا يمكن إنشاء كائن منه مباشرةً",
                                                                         "Class '{class}' is abstract and cannot be instantiated directly",
                                                                         "أنشئ صنفاً فرعياً يُنفّذ الأعضاء المجرّدة",
                                                                         "Create a concrete subclass that implements abstract members"));
        }

    } // namespace Errors
} // namespace Sad
