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
        // SadType::toValueType() — تحويل النوع الموحد إلى نوع التشغيل المتوافق
        // =================================================================================
        SadTypeKind SadType::toValueType() const
        {
            switch (kind_)
            {
            case SadTypeKind::Void:
                return VT::Void;
            case SadTypeKind::Integer:
                return VT::Integer;
            case SadTypeKind::Float:
                return VT::Float;
            case SadTypeKind::Boolean:
                return VT::Boolean;
            case SadTypeKind::String:
                return VT::String;
            case SadTypeKind::Byte:
                return VT::Integer; // (AR) بايت يُعامَل كرقم في المفسر
            case SadTypeKind::Array:
                return VT::Array;
            case SadTypeKind::Map:
                return VT::Map;
            case SadTypeKind::Tuple:
                return VT::Tuple;
            case SadTypeKind::Function:
            case SadTypeKind::Closure:
                return VT::Function;
            case SadTypeKind::Class:
            case SadTypeKind::Struct:
            case SadTypeKind::Enum:
            case SadTypeKind::Trait:
                return VT::Class;
            // (AR) «أي» تُقابل نفسَها؛ إسقاطُها إلى فراغٍ عبر الافتراضِ هو نظيرُ العيبِ
            //      المُصلَحِ في `fromValueType` — يُسدّ هنا كي لا يبقى الاتّجاهان متباعدَين.
            // (EN) Any maps to itself; letting it fall to Void through the default is the
            //      mirror of the bug fixed in `fromValueType` — sealed so the two
            //      directions do not drift apart.
            case SadTypeKind::Any:
                return VT::Any;
            default:
                return VT::Void;
            }
        }


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
            }
            return reg.getVoid();
        }


        // =================================================================================
        // SadType::fromArabicName() — إنشاء نوع من الاسم العربي
        // =================================================================================
        SadTypePtr SadType::fromArabicName(const std::string &name)
        {
            return SadTypeRegistry::instance().fromArabicName(name);
        }

    } // namespace Types
} // namespace Sad
