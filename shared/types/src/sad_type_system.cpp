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
// (AR) إلغاء تعريف ERROR الخاص بويندوز لتجنب التعارض مع ::Sad::Types::SadTypeKind::Unit إلخ
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
            case VT::Unit:
                return reg.getVoid();
            case VT::Integer:
                return reg.getInteger();
            case VT::UInt8:
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
            // ════════════════════════════════════════════════════════════════
            // (AR) 🔑 **ووقعَ العيبُ نفسُه رابعةً — وهذه المرّةُ أخطرُها.**
            //      `Unknown` معناه «لم يُصرَّحْ نوعُه»، وكان يسقطُ في الفرعِ
            //      الافتراضيِّ أدناه فيصيرُ **«خالي»** — أي أنّ «لا أعرف» تنقلبُ
            //      **يقينًا كاذبًا**. وكان ذلك بلا أثرٍ يومَ كان «فراغ» غيرَ
            //      مُصنَّفٍ في `isClassified`؛ فلمّا صارَ «خالي» نوعًا مُعلَنًا
            //      صارَ الحارسُ يحكمُ عليه، فيُرَدُّ **برنامجٌ صحيحٌ**:
            //      «قالب <نوع ت>» ثمّ «دالة ضاعف(س) ارجع س * 2» تُرفَضُ بـSEM054
            //      «العامل '*' لا يقبل 'خالي' مع 'رقم'» (مقيس).
            //      ⚠️ **وهو الموضعُ الذي أبطلَ إصلاحَ المحلّلِ كلَّه**: أُصلِحَت
            //      خمسةُ مواضعَ هناك لتُنتِجَ `Unknown`، فأعادَها هذا السطرُ
            //      «خالي» — وهو حرفيًّا ما يقولُه تعليقُ ISSUE-113 أعلاه عن
            //      نفسِه: «مهما أُصلح المحلّل». والدرسُ المكتوبُ ثلاثَ مرّاتٍ في
            //      هذا الملفِّ لم يُطبَّقْ على الحالةِ الرابعة: **فرعٌ افتراضيٌّ
            //      يُخمّنُ بدل أن يكسر**.
            // (EN) The same defect a FOURTH time, and the worst of the four. Unknown
            //      means "no type was declared" and fell to the default below,
            //      becoming Unit — turning "don't know" into a FALSE CERTAINTY. That
            //      was inert while Void was unclassified; once Unit became a declared
            //      kind the operand-domain guard began judging it and rejected CORRECT
            //      programs. This is the line that nullified the five parser fixes —
            //      exactly what the ISSUE-113 comment above says about itself.
            // ════════════════════════════════════════════════════════════════
            case VT::Unknown:
                return reg.getUnknown();
            }
            // (AR) 🔑 وقعَ العيبُ نفسُه ثالثةً: فُتحت ثمانيةُ ألفاظٍ عدديّةٍ في
            //      types.yaml (رقم8..رقم، طبيعي16/32، عشري32/64) فبلغَ نوعُها
            //      المحلّلَ سليمًا، ثمّ سقطَ **هنا** إلى `getVoid()` — فقُرِئ على
            //      المستعمِلِ «لمتغيّر 'س' من نوع 'فراغ'» وهو كتبَ «رقم32».
            //      والافتراضُ لا يُخمّنُ بعدَ اليوم: يسألُ الجدولَ المولَّدَ عن
            //      النوعِ، ولا يبقى «فراغ» إلّا لِما ليس عددًا ولا حالةَ له.
            // (EN) The same defect a third time: eight numeric words opened in
            //      types.yaml reached the parser correctly and then fell to
            //      getVoid() HERE. The default now asks the generated table
            //      instead of guessing.
            if (auto numeric = reg.getByKind(vt))
            {
                return numeric;
            }
            // ════════════════════════════════════════════════
            // (AR) 🔑 **وهذا آخرُ موضعٍ يُخمِّنُ في الدالّة — وقد زال.**
            //      كان `getVoid()`، فكلُ نوعٍ مُعلَنٍ في `types.yaml` بلا حالةٍ
            //      أعلاهُ ولا عُضوٍ عدديٍّ مُفهرَسٍ — بنيةٌ · تعدادٌ · سمةٌ ·
            //      مُغلَقٌ · اتِّحادٌ · مُعمَّمٌ · خطأٌ · أبدًا · مؤشِّرٌ · أنواعُ
            //      الرسومّيات — ينقلِبُ **«خالي»**، وهو نوعٌ مُصنَّفٌ يحكمُ
            //      عليهِ حارسُ نطاقاتِ المعامِلات. وتلكَ عينُ الآليّةِ
            //      الموصوفةِ فوقَ أربعَ مرّاتٍ في هذا الملفّ: «لا أعرف» تنقلِبُ
            //      يقينًا كاذبًا. والدرسُ كانَ مكتوبًا ولم يُطبَّق على
            //      السقوطِ نفسِهِ — إنّما على حالاتِهِ واحدةً واحدة.
            //      فصارَ السقوطُ `getUnknown()`: **إعلانُ جهلٍ لا ادِّعاءُ علم**،
            //      والمجهولُ يمرُّ بلا حكمٍ في `violates()`.
            //      أمرُ القياس: `python scripts/codegen/measure_type_layer_bridges.py`.
            // (EN) The last guessing fallback in this function. It returned Void,
            //      so every kind declared in types.yaml without a case above and
            //      without an indexed numeric member (Struct, Enum, Trait, Closure,
            //      Union, Generic, Error, Never, Pointer, the graphics kinds) became
            //      Unit — a CLASSIFIED type the operand-domain guard judges. That is
            //      the very mechanism this file documents four times over. The
            //      fallback is now getUnknown(): an admission of ignorance, and
            //      Unknown passes violates() unjudged.
            // ════════════════════════════════════════════════
            return reg.getUnknown();
        }


        // (AR) [ISSUE-113] حُذفت `toValueType` و`fromArabicName` — صفرُ مُنادٍ في
        //      المستودع (مقيسٌ ٢٠٢٦-٠٨-١٤). فبقيت `fromValueType` وحدَها هنا، وهي
        //      **الحاكمُ الفعليّ**: عقدةُ التصريحِ تبني `sadType` منها، وغيابُ حالةِ
        //      Null فيها هو ما أبطل إصلاحَ المواضعِ الستّةِ الأخرى بأكمله.
        // (EN) [ISSUE-113] toValueType/fromArabicName removed — zero callers. What
        //      remains is the one that actually governs declaration nodes.

    } // namespace Types
} // namespace Sad
