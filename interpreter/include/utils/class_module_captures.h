// class_module_captures.h — حقن ثوابت وحدة التعريف في نطاق طرق الأصناف
//
// (AR) عيب ع-1 (تقرير sad-libs): طرق الأصناف المستوردة لا ترى ثوابت وحدتها
//      لأن الالتقاط كان مقصورًا على الدوال الحرة. هذا المساعد يُحقن به
//      ClassType::moduleCaptures في النطاق الجديد لكل موضع ينفذ جسم طريقة
//      (طريقة، باني، خاصية، عامل محمّل، __call__، __دخول__/__خروج__).
//      يُستدعى بعد enterScope مباشرة وقبل ربط «هذا» والحقول والمعاملات،
//      فتتغلب هذه على الالتقاط عند تصادم الأسماء (define يكتب فوق السابق
//      في النطاق نفسه). سلسلة الوراثة تُحقن قاعدةً فأبناء حتى يتغلب
//      التقاط الصنف المشتق على التقاط أساسه.
//      الالتقاط لقطةٌ للقراءة فقط: إسنادُ طريقةٍ إلى اسمٍ ملتقطٍ يعرّف
//      محليًّا يموت عند خروج النطاق ولا يُكتب عائدًا إلى الوحدة — بخلاف
//      الدوال الحرة التي تعيد كتابة التقاطها بعد التنفيذ.
// (EN) Defect ع-1: imported class methods could not see their module's
//      top-level constants (capture existed for free functions only).
//      Inject ClassType::moduleCaptures right after enterScope and before
//      binding «هذا»/fields/parameters, base classes first so derived
//      captures win on collision.
//      The captures are a read-only snapshot: a method assigning to a
//      captured name writes a scope-local that dies at exitScope and is
//      never written back to the module (unlike free-function captures,
//      which are written back after execution).

#pragma once

#include "class_type.h"
#include "variable_manager.h"

namespace Sad
{
    namespace Interpreter
    {
        namespace Utils
        {

            inline void injectClassModuleCaptures(const Data::ClassType *cls,
                                                  Data::VariableManager &variables)
            {
                if (!cls)
                    return;

                // (AR) الأساس أولًا كي يتغلب المشتق عند تصادم الأسماء
                // (EN) Base first so the derived class wins on name collisions
                injectClassModuleCaptures(cls->getBaseClass(), variables);
                for (const auto *additionalBase : cls->getAdditionalBases())
                {
                    injectClassModuleCaptures(additionalBase, variables);
                }

                for (const auto &[capturedName, capturedValue] : cls->moduleCaptures)
                {
                    variables.define(capturedName, capturedValue);
                }
            }

        } // namespace Utils
    } // namespace Interpreter
} // namespace Sad
