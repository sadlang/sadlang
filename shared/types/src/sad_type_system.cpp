/**
 * @file sad_type_system.cpp
 * @brief (AR) تنفيذ دوال التحويل بين نظام الأنواع الموحد والأنظمة القديمة
 * @brief (EN) Implementation of conversion functions between unified type system and legacy systems
 *
 * (AR) هذا الملف يحتوي فقط على الدوال التي تحتاج value.h (لتجنب التضمين الدائري).
 *      كل شيء آخر مُعرَّف في الملف الرأسي sad_type_system.h مباشرة.
 *
 * (EN) This file contains only functions that need value.h to avoid circular includes.
 *      Everything else is defined inline in sad_type_system.h.
 */

#include "sad_type_system.h"
#include "value.h" // (AR) نحتاج ValueType الفعلي هنا

// ═══════════════════════════════════════════════════════════════════════════════════
// (AR) إلغاء تعريف ERROR الخاص بويندوز لتجنب التعارض مع ::Sad::Types::SadTypeKind::Void إلخ
// ═══════════════════════════════════════════════════════════════════════════════════
#ifdef VOID
#undef VOID
#endif
#ifdef ERROR
#undef ERROR
#endif

namespace Sad
{
    namespace Types
    {

        using VT = Sad::Types::SadTypeKind;
        // =================================================================================
        // SadType::fromValueType() — إنشاء نوع موحد من نوع التشغيل المتوافق
        // =================================================================================
        SadTypePtr SadType::fromValueType(SadTypeKind vt)
        {
            auto &reg = SadTypeRegistry::instance();
            switch (vt)
            {
            case VT::Void:
                return reg.getVoid();
            case VT::Integer:
                return reg.getInteger();
            case VT::Byte:
                return reg.getByte();
            case VT::UInt64:
                return reg.getUInt64();
            case VT::Float:
                return reg.getFloat();
            case VT::String:
                return reg.getString();
            case VT::Boolean:
                return reg.getBoolean();
            case VT::Array:
                return reg.makeArray();
            case VT::Map:
                return reg.makeMap();
            case VT::Tuple:
                return reg.makeTuple({});
            case VT::Function:
                return reg.makeFunction({});
            case VT::Class:
                return reg.getAny(); // (AR) كائن عام — لا نعرف الصنف هنا
            // (AR) [S-TS-P4] أنواع متقدّمة بلا نوع داخلي على مستوى الـkind → أي (Any)
            //      حتى لا تنهار إلى فراغ (Void) فتُطلِق تحذيرات كاذبة. النوع الداخلي
            //      الغنيّ يُبنى مباشرةً عبر makeOptional/makeResult/makeFuture/makeGenerator.
            case VT::Optional:
            case VT::Result:
            case VT::Future:
            case VT::Generator:
                return reg.getAny();
            // (AR) «أي» صراحةً: كان يسقط إلى فراغ عبر الافتراضِ أدناه، فيُطلِق المفسّرُ
            //      تحذيرَ «تعيينُ رقمٍ لمتغيّرٍ من نوع فراغ» على `أي س = ٤٢` السليمة.
            //      لم يظهر قبلًا إلّا لأنّ المحلّلَ كان يربط «أي» بـClass خطأً.
            // (EN) Explicit Any: it used to fall to the default below and become Void,
            //      making the interpreter warn on a perfectly valid `أي x = 42`. It was
            //      masked only because the parser wrongly mapped `أي` to Class.
            case VT::Any:
                return reg.getAny();
            // (AR) ISSUE-113: 🔑 **هذا هو الموضعُ الذي أبطل الإصلاحَ كلَّه.** عقدةُ
            //      التصريحِ تبني `sadType` من هنا (statements.h)، وغيابُ حالةِ Null
            //      يُسقِطها إلى `getVoid()` أدناه — فبقي المستعمِلُ يُقرأ عليه
            //      «لمتغيّر 'س' من نوع 'فراغ'» وهو كتب «عدم»، مهما أُصلح المحلّل.
            //      وهو الافتراضُ الصامتُ نفسُه الذي أوقع «أي» والاختياريَّ قبلَه:
            //      عيبٌ يتكرّر لأنّ الفرعَ الافتراضيَّ يُخمّن بدل أن يكسر.
            // (EN) ISSUE-113: this is what nullified the whole fix. Declaration nodes
            //      build their sadType here, and a missing Null case dropped it to
            //      Void — the same silent default that previously caught Any/Optional.
            case VT::Null:
                return reg.getNull();
            }
            return reg.getVoid();
        }


        // (AR) [ISSUE-113] حُذفت `toValueType` و`fromArabicName` — صفرُ مُنادٍ في
        //      المستودع (مقيسٌ ٢٠٢٦-٠٨-١٤). فبقيت `fromValueType` وحدَها هنا، وهي
        //      **الحاكمُ الفعليّ**: عقدةُ التصريحِ تبني `sadType` منها، وغيابُ حالةِ
        //      Null فيها هو ما أبطل إصلاحَ المواضعِ الستّةِ الأخرى بأكمله.
        // (EN) [ISSUE-113] toValueType/fromArabicName removed — zero callers. What
        //      remains is the one that actually governs declaration nodes.

    } // namespace Types
} // namespace Sad
