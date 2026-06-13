/**
 * @file type_bridge.cpp
 * @brief (AR) تنفيذ طبقة الجسر بين SadType و Value/ValueType
 * @brief (EN) Implementation of the bridge layer between SadType and Value/ValueType
 *
 * ═══════════════════════════════════════════════════════════════════════════════════
 * (AR) هذا الملف يحتوي على تنفيذ جميع دوال التحويل المعلنة في type_bridge.h
 *      بالإضافة إلى دوال SadValue::fromLegacy() و SadValue::toLegacy()
 *
 *      يضمّن كلاً من sad_type_system.h و value.h لأنه يحتاج التعريفات
 *      الكاملة لكلا النظامين. هذا هو المكان الوحيد الذي يُسمح فيه بذلك
 *      (لتجنب التبعيات الدائرية في الملفات الرأسية).
 *
 * (EN) This file contains implementation of all conversion functions declared
 *      in type_bridge.h, plus SadValue::fromLegacy() and SadValue::toLegacy().
 *      It includes both sad_type_system.h and value.h because it needs full
 *      definitions of both systems. This is the only place where this is allowed.
 * ═══════════════════════════════════════════════════════════════════════════════════
 *
 * @author فريق تطوير لغة ص / S Language Development Team
 * @date يناير 2026 / January 2026
 * @version 1.0 — ADR-01 Phase 1
 */

#include "type_bridge.h"
#include "sad_value.h"
#include "value.h"
#include "object_instance.h"
#include "function_ref.h"

// ═══════════════════════════════════════════════════════════════════════════════════
// (AR) إلغاء تعريفات ماكروز ويندوز المتعارضة
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

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  القسم ①: SadTypeKind ↔ ValueType
        //
        // █████████████████████████████████████████████████████████████████████████████████

        SadTypeKind toValueType(SadTypeKind kind)
        {
            switch (kind)
            {
            case SadTypeKind::Void:
                return VT::Void;
            case SadTypeKind::Integer:
                return VT::Integer;
            case SadTypeKind::Float:
                return VT::Float;
            case SadTypeKind::Float32:
                return VT::Float;
            case SadTypeKind::Float64:
                return VT::Float;
            case SadTypeKind::Boolean:
                return VT::Boolean;
            case SadTypeKind::String:
                return VT::String;
            case SadTypeKind::Byte:
                return VT::Integer;
            case SadTypeKind::Int8:
                return VT::Integer;
            case SadTypeKind::Int16:
                return VT::Integer;
            case SadTypeKind::Int32:
                return VT::Integer;
            case SadTypeKind::Int64:
                return VT::Integer;
            case SadTypeKind::UInt8:
                return VT::Integer;
            case SadTypeKind::UInt16:
                return VT::Integer;
            case SadTypeKind::UInt32:
                return VT::Integer;
            case SadTypeKind::UInt64:
                return VT::Integer;
            case SadTypeKind::Char:
                return VT::String;
            case SadTypeKind::Array:
                return VT::Array;
            case SadTypeKind::Map:
                return VT::Map;
            case SadTypeKind::Tuple:
                return VT::Tuple;
            case SadTypeKind::Slice:
                return VT::Array;
            case SadTypeKind::Function:
                return VT::Function;
            case SadTypeKind::Closure:
                return VT::Function;
            case SadTypeKind::Class:
                return VT::Class;
            case SadTypeKind::Struct:
                return VT::Class;
            case SadTypeKind::Enum:
                return VT::Class;
            case SadTypeKind::Trait:
                return VT::Class;
            // (AR) أنواع ليس لها مقابل مباشر في ValueType — تُحوَّل إلى أقرب نوع
            case SadTypeKind::Union:
                return VT::Void;
            case SadTypeKind::Intersection:
                return VT::Void;
            case SadTypeKind::Optional:
                return VT::Void;
            case SadTypeKind::Result:
                return VT::Void;
            case SadTypeKind::Generic:
                return VT::Void;
            case SadTypeKind::TypeParameter:
                return VT::Void;
            case SadTypeKind::TypeAlias:
                return VT::Void;
            case SadTypeKind::Pointer:
                return VT::Integer;
            case SadTypeKind::Reference:
                return VT::Void;
            case SadTypeKind::MutableRef:
                return VT::Void;
            case SadTypeKind::Any:
                return VT::Void;
            case SadTypeKind::Never:
                return VT::Void;
            case SadTypeKind::Unknown:
                return VT::Void;
            case SadTypeKind::Error:
                return VT::Void;
            case SadTypeKind::Future:
                return VT::Class;
            case SadTypeKind::Generator:
                return VT::Class;
            case SadTypeKind::Comprehension:
                return VT::Array;
            case SadTypeKind::Color:
                return VT::Class;
            case SadTypeKind::Widget:
                return VT::Class;
            case SadTypeKind::Window:
                return VT::Class;
            case SadTypeKind::Event:
                return VT::Class;
            }
            return VT::Void;
        }

        SadTypeKind fromValueType(SadTypeKind vt)
        {
            switch (vt)
            {
            case VT::Void:
                return SadTypeKind::Void;
            case VT::Integer:
                return SadTypeKind::Integer;
            case VT::Float:
                return SadTypeKind::Float;
            case VT::String:
                return SadTypeKind::String;
            case VT::Boolean:
                return SadTypeKind::Boolean;
            case VT::Array:
                return SadTypeKind::Array;
            case VT::Map:
                return SadTypeKind::Map;
            case VT::Tuple:
                return SadTypeKind::Tuple;
            case VT::Function:
                return SadTypeKind::Function;
            case VT::Class:
                return SadTypeKind::Class;
            }
            return SadTypeKind::Void;
        }

        Data::DataType toDataType(SadTypeKind kind)
        {
            switch (kind)
            {
            case SadTypeKind::Void:
                return DT::NONE;
            case SadTypeKind::Integer:
                return DT::INTEGER;
            case SadTypeKind::Float:
                return DT::FLOAT;
            case SadTypeKind::Float32:
                return DT::FLOAT;
            case SadTypeKind::Float64:
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
                return DT::FUNCTION;
            case SadTypeKind::Closure:
                return DT::FUNCTION;
            case SadTypeKind::Class:
                return DT::OBJECT;
            case SadTypeKind::Struct:
                return DT::OBJECT;
            case SadTypeKind::Enum:
                return DT::ENUM;
            case SadTypeKind::Trait:
                return DT::OBJECT;
            case SadTypeKind::Error:
                return DT::ERROR;
            case SadTypeKind::Unknown:
                return DT::UNKNOWN;
            default:
                return DT::UNKNOWN;
            }
        }

        SadTypeKind fromDataType(Data::DataType dt)
        {
            switch (dt)
            {
            case DT::NONE:
                return SadTypeKind::Void;
            case DT::INTEGER:
                return SadTypeKind::Integer;
            case DT::FLOAT:
                return SadTypeKind::Float;
            case DT::BOOLEAN:
                return SadTypeKind::Boolean;
            case DT::STRING:
                return SadTypeKind::String;
            case DT::BYTE:
                return SadTypeKind::Byte;
            case DT::ARRAY:
                return SadTypeKind::Array;
            case DT::MAP:
                return SadTypeKind::Map;
            case DT::TUPLE:
                return SadTypeKind::Tuple;
            case DT::FUNCTION:
                return SadTypeKind::Function;
            case DT::OBJECT:
                return SadTypeKind::Class;
            case DT::ENUM:
                return SadTypeKind::Enum;
            case DT::ERROR:
                return SadTypeKind::Error;
            case DT::UNKNOWN:
                return SadTypeKind::Unknown;
            }
            return SadTypeKind::Unknown;
        }

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  القسم ②: SadTypePtr ↔ ValueType
        //
        // █████████████████████████████████████████████████████████████████████████████████

        SadTypeKind sadTypeToValueType(const SadTypePtr &type)
        {
            if (!type)
                return VT::Void;
            return toValueType(type->getKind());
        }

        SadTypePtr sadTypeFromValueType(SadTypeKind vt)
        {
            // (AR) نفوّض لـ SadType::fromValueType الموجودة (مُعرَّفة في sad_type_system.cpp)
            return SadType::fromValueType(vt);
        }

        Data::DataType sadTypeToDataType(const SadTypePtr &type)
        {
            if (!type)
                return DT::UNKNOWN;
            return toDataType(type->getKind());
        }

        SadTypePtr sadTypeFromDataType(Data::DataType dt)
        {
            return SadType::fromDataType(dt);
        }

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  القسم ③: Value → SadTypePtr (استنتاج النوع)
        //
        // █████████████████████████████████████████████████████████████████████████████████

        SadTypePtr inferSadType(const Data::Value &value)
        {
            auto &reg = SadTypeRegistry::instance();

            switch (value.getType())
            {
            case VT::Void:
                return reg.getVoid();
            case Types::SadTypeKind::Null: // (AR) عدم — S-TS-P1
                return reg.getNull();

            case VT::Integer:
                return reg.getInteger();

            case VT::Float:
                return reg.getFloat();

            case VT::String:
                return reg.getString();

            case VT::Boolean:
                return reg.getBoolean();

            case VT::Array:
            {
                // (AR) استنتاج نوع العنصر من أول عنصر في المصفوفة
                const auto &arr = value.toArrayRef();
                if (!arr.empty())
                {
                    SadTypePtr elemType = inferSadType(arr.front());
                    return reg.makeArray(elemType);
                }
                return reg.makeArray(); // (AR) مصفوفة بدون نوع عنصر
            }

            case VT::Map:
            {
                // (AR) خرائط Value تستخدم مفاتيح نصية دائماً
                return reg.makeMap(reg.getString(), reg.getAny());
            }

            case VT::Tuple:
            {
                const auto &tup = value.toTupleRef();
                std::vector<SadTypePtr> elemTypes;
                elemTypes.reserve(tup.size());
                for (const auto &e : tup)
                {
                    elemTypes.push_back(inferSadType(e));
                }
                return reg.makeTuple(std::move(elemTypes));
            }

            case VT::Class:
            {
                // (AR) استنتاج اسم الصنف من ObjectInstance
                std::string className = value.getClassName();
                if (!className.empty())
                {
                    return reg.getOrCreateClass(className);
                }
                return reg.getAny();
            }

            case VT::Function:
            {
                return reg.makeFunction({});
            }
            }

            return reg.getVoid();
        }

        bool isValueCompatible(const Data::Value &value, const SadTypePtr &expectedType)
        {
            if (!expectedType)
                return true; // (AR) نوع متوقع مجهول — نسمح

            SadTypePtr inferred = inferSadType(value);
            if (!inferred)
                return true;

            return inferred->isAssignableTo(expectedType.get());
        }

        std::string valueToArabicTypeName(const Data::Value &value)
        {
            SadTypePtr type = inferSadType(value);
            return type ? type->arabicName() : "مجهول";
        }

        std::string valueToEnglishTypeName(const Data::Value &value)
        {
            SadTypePtr type = inferSadType(value);
            return type ? type->englishName() : "Unknown";
        }

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  القسم ④: SadValue::fromLegacy() و SadValue::toLegacy()
        //
        // █████████████████████████████████████████████████████████████████████████████████

        SadValue SadValue::fromLegacy(const Data::Value &legacy)
        {
            auto &reg = SadTypeRegistry::instance();

            switch (legacy.getType())
            {
            case VT::Void:
                return SadValue();

            case VT::Integer:
                return SadValue(legacy.toInt64());

            case VT::Float:
                return SadValue(legacy.toDouble());

            case VT::String:
                return SadValue(legacy.toStringRef());

            case VT::Boolean:
                return SadValue(legacy.toBool());

            case VT::Array:
            {
                const auto &arr = legacy.toArrayRef();
                SadValue::ArrayType newArr;
                newArr.reserve(arr.size());
                for (const auto &elem : arr)
                {
                    newArr.push_back(SadValue::fromLegacy(elem));
                }
                // (AR) استنتاج نوع العنصر
                SadTypePtr elemType = nullptr;
                if (!arr.empty())
                {
                    elemType = inferSadType(arr.front());
                }
                return SadValue(std::move(newArr), elemType);
            }

            case VT::Map:
            {
                const auto &map = legacy.toMapRef();
                SadValue::MapType newMap;
                for (const auto &[key, val] : map)
                {
                    newMap.emplace(key, SadValue::fromLegacy(val));
                }
                return SadValue(std::move(newMap));
            }

            case VT::Tuple:
            {
                const auto &tup = legacy.toTupleRef();
                std::vector<SadValue> elems;
                elems.reserve(tup.size());
                for (const auto &e : tup)
                {
                    elems.push_back(SadValue::fromLegacy(e));
                }
                return SadValue(SadValue::TupleType(std::move(elems)));
            }

            case VT::Class:
            {
                auto obj = legacy.toObject();
                std::string className = legacy.getClassName();
                if (!className.empty())
                {
                    return SadValue(obj, className);
                }
                return SadValue(obj);
            }

            case VT::Function:
            {
                auto funcRef = legacy.toFunction();
                return SadValue(funcRef);
            }
            }

            return SadValue();
        }

        Data::Value SadValue::toLegacy() const
        {
            auto kind = getKind();

            switch (kind)
            {
            case SadTypeKind::Void:
                return Data::Value();

            case SadTypeKind::Integer:
            case SadTypeKind::Byte:
            case SadTypeKind::Int8:
            case SadTypeKind::Int16:
            case SadTypeKind::Int32:
            case SadTypeKind::Int64:
            case SadTypeKind::UInt8:
            case SadTypeKind::UInt16:
            case SadTypeKind::UInt32:
            case SadTypeKind::UInt64:
                if (std::holds_alternative<int64_t>(data_))
                    return Data::Value(std::get<int64_t>(data_));
                return Data::Value();

            case SadTypeKind::Float:
            case SadTypeKind::Float32:
            case SadTypeKind::Float64:
                if (std::holds_alternative<double>(data_))
                    return Data::Value(std::get<double>(data_));
                return Data::Value();

            case SadTypeKind::Boolean:
                if (std::holds_alternative<bool>(data_))
                    return Data::Value(std::get<bool>(data_));
                return Data::Value();

            case SadTypeKind::String:
            case SadTypeKind::Char:
                if (std::holds_alternative<std::string>(data_))
                    return Data::Value(std::get<std::string>(data_));
                return Data::Value();

            case SadTypeKind::Array:
            case SadTypeKind::Slice:
            case SadTypeKind::Comprehension:
            {
                if (!std::holds_alternative<ArrayType>(data_))
                    return Data::Value();
                const auto &arr = std::get<ArrayType>(data_);
                Data::Value::ArrayType legacyArr;
                legacyArr.reserve(arr.size());
                for (const auto &elem : arr)
                {
                    legacyArr.push_back(elem.toLegacy());
                }
                return Data::Value(std::move(legacyArr));
            }

            case SadTypeKind::Map:
            {
                if (!std::holds_alternative<MapType>(data_))
                    return Data::Value();
                const auto &map = std::get<MapType>(data_);
                Data::Value::MapType legacyMap;
                for (const auto &[key, val] : map)
                {
                    legacyMap.emplace(key, val.toLegacy());
                }
                return Data::Value(std::move(legacyMap));
            }

            case SadTypeKind::Tuple:
            {
                if (!std::holds_alternative<TupleType>(data_))
                    return Data::Value();
                const auto &tup = std::get<TupleType>(data_);
                Data::Value::TupleType legacyTup;
                legacyTup.reserve(tup.size());
                for (const auto &e : tup.elements)
                {
                    legacyTup.push_back(e.toLegacy());
                }
                return Data::Value(Data::Value::TupleTag{}, std::move(legacyTup));
            }

            case SadTypeKind::Class:
            case SadTypeKind::Struct:
            case SadTypeKind::Enum:
            case SadTypeKind::Trait:
            case SadTypeKind::Future:
            case SadTypeKind::Generator:
            case SadTypeKind::Color:
            case SadTypeKind::Widget:
            case SadTypeKind::Window:
            case SadTypeKind::Event:
            {
                if (std::holds_alternative<ObjectPtr>(data_))
                    return Data::Value(std::get<ObjectPtr>(data_));
                return Data::Value();
            }

            case SadTypeKind::Function:
            case SadTypeKind::Closure:
            {
                if (std::holds_alternative<FunctionRefPtr>(data_))
                    return Data::Value(std::get<FunctionRefPtr>(data_));
                return Data::Value();
            }

            default:
                return Data::Value();
            }
        }

    } // namespace Types
} // namespace Sad
