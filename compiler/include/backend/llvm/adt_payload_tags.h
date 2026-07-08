#pragma once
#include <cstdint>

// ============================================================================
// (AR) مخطّط وسم حمولة ADT — بتّتان في MSB (63،62) لخانة الحقل ذات نوع المؤشّر.
//      المؤشّر المُعنوَن قانونيًّا في فضاء المستخدم يجعل bit63=bit62=0، فالوسم 00
//      محجوز للمؤشّرات/النصوص الحقيقيّة. الأنواع الأخرى تُعلَّم على النحو التالي:
//        00 → مؤشّر/نصّ (يُخزَّن المؤشّر الحقيقيّ)
//        01 → عشريّ مُعلَّب (boxed float): مؤشّر صندوق كومة يحوي الـdouble | bit62
//        10 → عدد صحيح: val | bit63
//        11 → منطقيّ: val | (bit63|bit62)
//      العشريّ **مُعلَّب** (الخانة تحمل مؤشّرًا موسومًا لا بتّات الـdouble الخام) كي
//      لا تصطدم بِتّة إشارة الـdouble ببتّات الوسم (ISSUE-076/082/084).
// (EN) ADT payload tag scheme — 2 MSB bits (63,62) in a pointer-typed field slot.
//      A canonical user-space pointer has bit63=bit62=0, so tag 00 is reserved for
//      real pointers/strings. Other types are tagged:
//        00 → pointer/string (the real pointer is stored)
//        01 → boxed float: heap box pointer holding the double, OR-ed with bit62
//        10 → integer: val | bit63
//        11 → bool: val | (bit63|bit62)
//      Floats are BOXED (the slot holds a tagged *pointer*, not the raw double bits)
//      so the double's own sign bit never collides with the tag (ISSUE-076/082/084).
// ============================================================================

namespace Sad
{
    namespace LLVM
    {
        /// (AR) قناع البتّة 63 / (EN) bit-63 mask
        inline constexpr uint64_t kAdtPayloadBit63 = 1ULL << 63;
        /// (AR) قناع البتّة 62 / (EN) bit-62 mask
        inline constexpr uint64_t kAdtPayloadBit62 = 1ULL << 62;
        /// (AR) قناع البتّتين (63|62) / (EN) 2-bit tag mask (63|62)
        inline constexpr uint64_t kAdtPayloadTagMask = 3ULL << 62;

        /// (AR) وسم المؤشّر/النصّ = 00 / (EN) pointer/string tag = 00
        inline constexpr uint64_t kAdtPayloadTagPointer = 0ULL;
        /// (AR) وسم العشريّ المُعلَّب = 01 / (EN) boxed-float tag = 01
        inline constexpr uint64_t kAdtPayloadTagFloat = 1ULL << 62;
        /// (AR) وسم العدد الصحيح = 10 / (EN) integer tag = 10
        inline constexpr uint64_t kAdtPayloadTagInteger = 1ULL << 63;
        /// (AR) وسم المنطقيّ = 11 / (EN) bool tag = 11
        inline constexpr uint64_t kAdtPayloadTagBool = 3ULL << 62;
    } // namespace LLVM
} // namespace Sad
