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
#include <shared_mutex>

namespace Sad
{
    namespace Interpreter
    {
        namespace Utils
        {

            // (AR) قفل قارئ/كاتب لخرائط moduleCaptures كلها: الكاتب هو تنفيذ
            //      الاستيراد (وإعادته)، والقارئ كل تنفيذ لجسم طريقة — وقد
            //      يجريان في خيطين (استيراد في الرئيسي بينما خيط «أطلق» ينفذ
            //      طريقة صنف مستورد) فكانت unordered_map تقرأ وتكتب متزامنة
            //      بلا حماية (سلوك غير معرف — رصدته المراجعة).
            // (EN) Reader/writer lock over every moduleCaptures map: import
            //      execution writes, every method-body execution reads — and
            //      the two can race across threads (a main-thread import while
            //      a spawned thread runs an imported class method), which was
            //      an unguarded concurrent unordered_map access (UB).
            inline std::shared_mutex &moduleCapturesMutex()
            {
                static std::shared_mutex mutex;
                return mutex;
            }

            inline void injectClassModuleCapturesLocked(const Data::ClassType *cls,
                                                        Data::VariableManager &variables)
            {
                if (!cls)
                    return;

                // (AR) الأساس أولًا كي يتغلب المشتق عند تصادم الأسماء
                // (EN) Base first so the derived class wins on name collisions
                injectClassModuleCapturesLocked(cls->getBaseClass(), variables);
                for (const auto *additionalBase : cls->getAdditionalBases())
                {
                    injectClassModuleCapturesLocked(additionalBase, variables);
                }

                for (const auto &[capturedName, capturedValue] : cls->moduleCaptures)
                {
                    variables.define(capturedName, capturedValue);
                }
            }

            inline void injectClassModuleCaptures(const Data::ClassType *cls,
                                                  Data::VariableManager &variables)
            {
                std::shared_lock<std::shared_mutex> readGuard(moduleCapturesMutex());
                injectClassModuleCapturesLocked(cls, variables);
            }

        } // namespace Utils
    } // namespace Interpreter
} // namespace Sad
