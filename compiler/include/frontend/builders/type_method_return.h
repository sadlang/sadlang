// ============================================================================
// type_method_return.h
// ============================================================================
// (AR) نوعُ عائدِ طرق الأنواع المدمجة (نصّ/مصفوفة/خريطة) — مرجعٌ واحد.
//      كان هذا النوع محبوسًا داخل بُناة الطرق (method_call_*.cpp)، فلم يبلغه
//      مُستنتِجُ الأنواع الذي يعمل في مرورٍ سابقٍ للبناء. فكانت كلُّ قيمةٍ عائدةٍ
//      من طريقةِ نوعٍ تُستنتَج «رقمًا» (الافتراضيّ)، فيَعُدُّ اختصارُ المقارنة
//      الصارمة `نص == رقم` خلطًا بين نوعين ويطوي المقارنةَ إلى «خطأ» ثابتة —
//      إمّا فشلَ بناءٍ مبهمًا في شرط «إذا»، أو نتيجةً خاطئةً صامتةً خارجَه.
// (EN) Return type of built-in type methods (string/array/map) — single reference.
//      This knowledge used to live only inside the method builders, out of reach
//      of the type-inference pass that runs before building; every type-method
//      result was therefore inferred as Integer.
// ============================================================================

#ifndef SAD_COMPILER_FRONTEND_BUILDERS_TYPE_METHOD_RETURN_H
#define SAD_COMPILER_FRONTEND_BUILDERS_TYPE_METHOD_RETURN_H

#include <string>

#include "sir_types.h"

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            // (AR) وصفُ عائدِ طريقةِ نوع
            // (EN) Description of a type method's return value
            struct TypeMethodReturn
            {
                // (AR) صحيحٌ إذا عُرفت الطريقةُ لهذا النوع
                bool known = false;
                // (AR) نوعُ القيمة العائدة
                SadTypeKind kind = SadTypeKind::Integer;
                // (AR) نوعُ العنصر حين يكون العائدُ مصفوفةً؛ Unknown لغيرها
                SadTypeKind elementKind = SadTypeKind::Unknown;
            };

            // (AR) يستخرج نوعَ عائدِ طريقةٍ مدمجةٍ من نوع الكائن واسم الطريقة.
            //      يعيد known=false للطرق المجهولة، فيبقى المتّصلُ على تصرّفه السابق.
            // (EN) Resolves a built-in type method's return type from the object type
            //      and the method name. Returns known=false for unknown methods so
            //      callers keep their previous behaviour.
            TypeMethodReturn typeMethodReturnKind(SadTypeKind objectKind,
                                                  const std::string &methodName);

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad

#endif // SAD_COMPILER_FRONTEND_BUILDERS_TYPE_METHOD_RETURN_H
