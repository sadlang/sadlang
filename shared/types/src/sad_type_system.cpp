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
// (AR) إلغاء تعريف ERROR الخاص بويندوز لتجنب التعارض مع ValueType::VOID إلخ
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
        using DT = Sad::Data::DataType;

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
            default:
                return VT::Void;
            }
        }

        // =================================================================================
        // SadType::toDataType() — تحويل النوع الموحد إلى DataType (AST)
        // =================================================================================
        Data::DataType SadType::toDataType() const
        {
            switch (kind_)
            {
            case SadTypeKind::Void:
                return DT::NONE;
            case SadTypeKind::Integer:
                return DT::INTEGER;
            case SadTypeKind::Float:
                return DT::FLOAT;
            case SadTypeKind::Boolean:
                return DT::BOOLEAN;
            case SadTypeKind::String:
                return DT::STRING;
            case SadTypeKind::Byte:
                return DT::BYTE;
            case SadTypeKind::Array:
                return DT::ARRAY;
            case SadTypeKind::Map:
                return DT::MAP;
            case SadTypeKind::Tuple:
                return DT::TUPLE;
            case SadTypeKind::Function:
            case SadTypeKind::Closure:
                return DT::FUNCTION;
            case SadTypeKind::Class:
            case SadTypeKind::Struct:
            case SadTypeKind::Trait:
                return DT::OBJECT;
            case SadTypeKind::Enum:
                return DT::ENUM;
            case SadTypeKind::Error:
                return DT::ERROR;
            case SadTypeKind::Unknown:
                return DT::UNKNOWN;
            default:
                return DT::UNKNOWN;
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
            }
            return reg.getVoid();
        }

        // =================================================================================
        // SadType::fromDataType() — إنشاء نوع موحد من DataType (AST)
        // =================================================================================
        SadTypePtr SadType::fromDataType(Data::DataType dt)
        {
            auto &reg = SadTypeRegistry::instance();
            switch (dt)
            {
            case DT::NONE:
                return reg.getVoid();
            case DT::INTEGER:
                return reg.getInteger();
            case DT::FLOAT:
                return reg.getFloat();
            case DT::BOOLEAN:
                return reg.getBoolean();
            case DT::STRING:
                return reg.getString();
            case DT::BYTE:
                return reg.getByte();
            case DT::ARRAY:
                return reg.makeArray();
            case DT::MAP:
                return reg.makeMap();
            case DT::TUPLE:
                return reg.makeTuple({});
            case DT::FUNCTION:
                return reg.makeFunction({});
            case DT::OBJECT:
                return reg.getAny();
            case DT::ENUM:
                return reg.getAny(); // (AR) تعداد عام — لا نعرف الاسم هنا
            case DT::ERROR:
                return reg.getError();
            case DT::UNKNOWN:
                return reg.getUnknown();
            }
            return reg.getUnknown();
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
