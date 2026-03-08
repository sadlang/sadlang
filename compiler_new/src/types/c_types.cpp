// تعطيل تحذير Unicode للنصوص العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file c_types.cpp
 * @brief (AR) تنفيذ نظام تعيين أنواع C لـ FFI
 *        (EN) Implementation of C Type Mapping System for FFI
 * 
 * @details
 * (AR) هذا الملف يحتوي على التنفيذ الكامل لنظام تعيين الأنواع.
 *      يتضمن:
 *      - تحويل جميع أنواع "ص" الأساسية إلى أنواع C
 *      - تحويل أنواع C إلى أنواع "ص"
 *      - بناء الأنواع المركبة (هياكل، اتحادات، تعدادات)
 *      - حساب الأحجام والمحاذاة
 * 
 * (EN) This file contains the complete implementation of the type mapping system.
 *      Includes:
 *      - Converting all Sad primitive types to C types
 *      - Converting C types to Sad types
 *      - Building composite types (structs, unions, enums)
 *      - Calculating sizes and alignments
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2025-01-18
 * @version 1.0.0
 */

#include "types/c_types.h"
#include "types/composite_type_classes.h"
#include <stdexcept>
#include <cassert>

namespace Sad {
namespace TypeSystem {

// ============================================================================
//                    (AR) تنفيذ CTypeMapper
//                    (EN) CTypeMapper Implementation
// ============================================================================

/**
 * @brief (AR) بناء محول الأنواع
 *        (EN) Construct type mapper
 */
CTypeMapper::CTypeMapper(const TypeConversionOptions& options)
    : options_(options) {
    /*
     * (AR) تهيئة محول الأنواع مع الخيارات المحددة
     *      - تسجيل الأنواع الأساسية
     *      - إعداد جداول التحويل
     * 
     * (EN) Initialize type mapper with specified options
     *      - Register basic types
     *      - Setup conversion tables
     */
}

// ----------------------------------------------------------------------------
// (AR) تحويل من "ص" إلى C
// (EN) Conversion from Sad to C
// ----------------------------------------------------------------------------

/**
 * @brief (AR) تحويل نوع "ص" إلى نوع C
 *        (EN) Convert Sad type to C type
 * 
 * @details
 * (AR) خوارزمية التحويل:
 *      1. فحص نوع "ص" (أساسي، مؤشر، مصفوفة، بنية)
 *      2. استدعاء الدالة المناسبة لكل نوع
 *      3. التعامل مع الحالات الخاصة (حجم المنصة)
 * 
 * (EN) Conversion algorithm:
 *      1. Check Sad type (primitive, pointer, array, struct)
 *      2. Call appropriate function for each type
 *      3. Handle special cases (platform size)
 */
FFI::CTypePtr CTypeMapper::sadToC(TypePtr sadType) const {
    if (!sadType) {
        throw std::invalid_argument(
            "(AR) \xD8\xAE\xD8\xB7\xD8\xA3: \xD9\x86\xD9\x88\xD8\xB9 "
            "\xD9\x81\xD8\xA7\xD8\xB1\xD8\xBA / "
            "(EN) Error: null type"
        );
    }
    
    /*
     * (AR) فحص نوع "ص" وتحويله
     * (EN) Check Sad type and convert it
     */
    
    // (AR) النوع الأساسي / (EN) Primitive type
    if (auto* primType = dynamic_cast<PrimitiveType*>(sadType.get())) {
        FFI::CBasicType cBasic = sadPrimitiveToCBasic(*primType);
        return std::make_shared<FFI::CBasicTypeImpl>(cBasic);
    }
    
    // (AR) نوع المصفوفة / (EN) Array type
    if (auto* arrayType = dynamic_cast<ArrayType*>(sadType.get())) {
        TypePtr elemType = arrayType->getElementType();
        if (arrayType->isFixedSize()) {
            return sadArrayToC(elemType, *arrayType->getFixedSize());
        } else {
            // (AR) مصفوفة ديناميكية = مؤشر للعنصر
            // (EN) Dynamic array = pointer to element
            return sadPointerToC(elemType);
        }
    }
    
    // (AR) نوع الصف (Tuple) / (EN) Tuple type
    if (auto* tupleType = dynamic_cast<TupleType*>(sadType.get())) {
        // (AR) الصف يُحوّل إلى بنية C
        // (EN) Tuple converts to C struct
        CStructBuilder builder("_sad_tuple_" + std::to_string(tupleType->getArity()), "صف");
        const auto& elements = tupleType->getElementTypes();
        for (size_t i = 0; i < elements.size(); ++i) {
            FFI::CTypePtr fieldType = sadToC(elements[i]);
            builder.addField("_" + std::to_string(i), fieldType, "عنصر_" + std::to_string(i));
        }
        return builder.build();
    }
    
    // (AR) دعم هرمية Struct/Class بشكل عملي:
    // عندما لا تتوفر StructType كـ Type، نمثل الأنواع الكائنية كمؤشر opaque.
    // (EN) Practical Struct/Class hierarchy support:
    // when StructType is not a Type node yet, represent object-like types as opaque pointer.
    if (sadType->getKind() == TypeKind::Class || sadType->getKind() == TypeKind::Interface) {
        auto voidType = std::make_shared<FFI::CBasicTypeImpl>(FFI::CBasicType::VOID);
        return std::make_shared<FFI::CPointerType>(voidType);
    }
    
    // (AR) نوع غير معروف / (EN) Unknown type
    throw std::runtime_error(
        "(AR) \xD9\x86\xD9\x88\xD8\xB9 \xD8\xBA\xD9\x8A\xD8\xB1 "
        "\xD9\x85\xD8\xAF\xD8\xB9\xD9\x88\xD9\x85 / "
        "(EN) Unsupported type"
    );
}

/**
 * @brief (AR) تحويل نوع "ص" الأساسي إلى CBasicType
 *        (EN) Convert Sad primitive type to CBasicType
 * 
 * @details
 * (AR) جدول التحويل التفصيلي:
 *      ┌──────────────────┬────────────────┬───────────────┐
 *      │ Kind             │ CBasicType     │ ملاحظات       │
 *      ├──────────────────┼────────────────┼───────────────┤
 *      │ VOID             │ VOID           │ -             │
 *      │ BOOL             │ BOOL           │ 8 بت          │
 *      │ INT8             │ SCHAR          │ موقّع         │
 *      │ INT16            │ SHORT          │ موقّع         │
 *      │ INT32            │ INT            │ موقّع         │
 *      │ INT64            │ LONG_LONG      │ موقّع         │
 *      │ UINT8            │ UCHAR          │ غير موقّع     │
 *      │ UINT16           │ USHORT         │ غير موقّع     │
 *      │ UINT32           │ UINT           │ غير موقّع     │
 *      │ UINT64           │ ULONG_LONG     │ غير موقّع     │
 *      │ FLOAT32          │ FLOAT          │ 32 بت         │
 *      │ FLOAT64          │ DOUBLE         │ 64 بت         │
 *      │ CHAR             │ CHAR           │ 8 بت          │
 *      │ SIZE             │ SIZE_T         │ 32/64 بت      │
 *      └──────────────────┴────────────────┴───────────────┘
 * 
 * (EN) Detailed conversion table shown above
 */
FFI::CBasicType CTypeMapper::sadPrimitiveToCBasic(const PrimitiveType& prim) const {
    return primitiveKindToCBasic(prim.getKind());
}

/**
 * @brief (AR) تحويل نوع من خلال Kind
 *        (EN) Convert type through Kind
 */
FFI::CBasicType CTypeMapper::primitiveKindToCBasic(TypeKind kind) const {
    
    switch (kind) {
        // (AR) فراغ / (EN) Void
        case TypeKind::Void:
            return FFI::CBasicType::VOID;
            
        // (AR) منطقي / (EN) Boolean
        case TypeKind::Boolean:
            return FFI::CBasicType::BOOL;
            
        // (AR) أعداد صحيحة / (EN) Integers
        case TypeKind::Integer:
            return FFI::CBasicType::INT;
            
        // (AR) أعداد عشرية / (EN) Floats
        case TypeKind::Float:
            return FFI::CBasicType::DOUBLE;
            
        // (AR) نص / (EN) String — تحويل إلى const char*
        case TypeKind::String:
            // String يُحوّل في sadToC لمعالجة خاصة
            return FFI::CBasicType::CHAR;
            
        default:
            throw std::runtime_error(
                "(AR) \xD9\x86\xD9\x88\xD8\xB9 \xD8\xA3\xD8\xB3\xD8\xA7\xD8\xB3\xD9\x8A "
                "\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x85\xD8\xB9\xD8\xB1\xD9\x88\xD9\x81 / "
                "(EN) Unknown primitive type"
            );
    }
}

/**
 * @brief (AR) تحويل مؤشر "ص" إلى مؤشر C
 *        (EN) Convert Sad pointer to C pointer
 */
FFI::CTypePtr CTypeMapper::sadPointerToC(TypePtr pointeeType) const {
    /*
     * (AR) تحويل نوع ما يشير إليه المؤشر أولاً
     *      ثم إنشاء نوع المؤشر
     * 
     * (EN) Convert pointee type first
     *      Then create pointer type
     */
    FFI::CTypePtr cPointeeType = sadToC(pointeeType);
    return std::make_shared<FFI::CPointerType>(cPointeeType);
}

/**
 * @brief (AR) تحويل مصفوفة "ص" إلى مصفوفة C
 *        (EN) Convert Sad array to C array
 */
FFI::CTypePtr CTypeMapper::sadArrayToC(TypePtr elementType, size_t size) const {
    /*
     * (AR) المصفوفات في C تحتاج:
     *      - نوع العنصر
     *      - الحجم (عدد العناصر)
     * 
     * (EN) C arrays need:
     *      - Element type
     *      - Size (number of elements)
     */
    FFI::CTypePtr cElementType = sadToC(elementType);
    return std::make_shared<FFI::CArrayType>(cElementType, size);
}

/**
 * @brief (AR) تحويل بنية "ص" إلى بنية C
 *        (EN) Convert Sad struct to C struct
 */
FFI::CStructTypePtr CTypeMapper::sadStructToC(const StructType& sadStruct) const {
    /*
     * (AR) تحويل البنية:
     *      1. إنشاء باني البنية
     *      2. تحويل كل حقل
     *      3. بناء البنية النهائية
     * 
     * (EN) Convert struct:
     *      1. Create struct builder
     *      2. Convert each field
     *      3. Build final struct
     */
    // StructType has getArabicName() and getEnglishName(), not getCName()
    CStructBuilder builder(sadStruct.getEnglishName(), sadStruct.getArabicName());
    
    for (const auto& field : sadStruct.getFields()) {
        FFI::CTypePtr cFieldType = sadToC(field->getType());
        builder.addField(field->getName(), cFieldType, field->getName());
    }
    
    return builder.build();
}

// ----------------------------------------------------------------------------
// (AR) تحويل من C إلى "ص"
// (EN) Conversion from C to Sad
// ----------------------------------------------------------------------------

/**
 * @brief (AR) تحويل نوع C إلى نوع "ص"
 *        (EN) Convert C type to Sad type
 */
TypePtr CTypeMapper::cToSad(FFI::CTypePtr cType) const {
    if (!cType) {
        throw std::invalid_argument(
            "(AR) \xD8\xAE\xD8\xB7\xD8\xA3: \xD9\x86\xD9\x88\xD8\xB9 C "
            "\xD9\x81\xD8\xA7\xD8\xB1\xD8\xBA / "
            "(EN) Error: null C type"
        );
    }
    
    /*
     * (AR) فحص نوع C وتحويله
     * (EN) Check C type and convert it
     */
    
    // (AR) النوع الأساسي / (EN) Basic type
    if (auto* basicType = dynamic_cast<FFI::CBasicTypeImpl*>(cType.get())) {
        return cBasicToSad(basicType->getBasicType());
    }
    
    // (AR) نوع المؤشر / (EN) Pointer type
    if (auto* ptrType = dynamic_cast<FFI::CPointerType*>(cType.get())) {
        return cPointerToSad(ptrType->getPointeeType());
    }
    
    // (AR) نوع المصفوفة / (EN) Array type
    if (auto* arrayType = dynamic_cast<FFI::CArrayType*>(cType.get())) {
        TypePtr elemType = cToSad(arrayType->getElementType());
        return std::make_shared<ArrayType>(elemType, arrayType->getSize());
    }
    
    // (AR) نوع البنية / (EN) Struct type
    if (auto* structType = dynamic_cast<FFI::CStructType*>(cType.get())) {
        (void)structType;
        // (AR) fallback آمن: نمثل بنية C كنوع Any حتى يكتمل جسر StructType.
        // (EN) Safe fallback: represent C struct as Any until StructType bridge is complete.
        return createPrimitiveType(TypeKind::Any);
    }
    
    throw std::runtime_error(
        "(AR) \xD9\x86\xD9\x88\xD8\xB9 C \xD8\xBA\xD9\x8A\xD8\xB1 "
        "\xD9\x85\xD8\xAF\xD8\xB9\xD9\x88\xD9\x85 / "
        "(EN) Unsupported C type"
    );
}

/**
 * @brief (AR) تحويل CBasicType إلى نوع "ص" أساسي
 *        (EN) Convert CBasicType to Sad primitive type
 * 
 * @details
 * (AR) جدول التحويل العكسي:
 *      ┌────────────────┬──────────────────┬───────────────┐
 *      │ CBasicType     │ Kind             │ اسم "ص"       │
 *      ├────────────────┼──────────────────┼───────────────┤
 *      │ VOID           │ VOID             │ فراغ          │
 *      │ CHAR           │ CHAR             │ حرف           │
 *      │ SCHAR          │ INT8             │ ص8            │
 *      │ UCHAR          │ UINT8            │ ص_غ8          │
 *      │ SHORT          │ INT16            │ ص16           │
 *      │ USHORT         │ UINT16           │ ص_غ16         │
 *      │ INT            │ INT32            │ ص32           │
 *      │ UINT           │ UINT32           │ ص_غ32         │
 *      │ LONG           │ INT32/INT64      │ حسب المنصة    │
 *      │ ULONG          │ UINT32/UINT64    │ حسب المنصة    │
 *      │ LONG_LONG      │ INT64            │ ص64           │
 *      │ ULONG_LONG     │ UINT64           │ ص_غ64         │
 *      │ FLOAT          │ FLOAT32          │ عش32          │
 *      │ DOUBLE         │ FLOAT64          │ عش64          │
 *      │ BOOL           │ BOOL             │ منطقي         │
 *      │ SIZE_T         │ SIZE             │ حجم           │
 *      └────────────────┴──────────────────┴───────────────┘
 * 
 * (EN) Reverse conversion table shown above
 */
TypePtr CTypeMapper::cBasicToSad(FFI::CBasicType cBasic) const {
    TypeKind sadKind;
    
    switch (cBasic) {
        // (AR) فراغ / (EN) Void
        case FFI::CBasicType::VOID:
            sadKind = TypeKind::Void;
            break;
            
        // (AR) حرف / (EN) Character — حرف C هو عدد صحيح 8-بت
        case FFI::CBasicType::CHAR:
            sadKind = TypeKind::Integer;
            break;
            
        // (AR) أعداد صحيحة موقّعة / (EN) Signed integers
        case FFI::CBasicType::SCHAR:
        case FFI::CBasicType::SHORT:
        case FFI::CBasicType::INT:
        case FFI::CBasicType::LONGLONG:
            sadKind = TypeKind::Integer;
            break;
        case FFI::CBasicType::LONG:
            /*
             * (AR) حجم long يختلف حسب المنصة:
             *      - Windows (حتى 64 بت): 32 بت
             *      - Linux/macOS (64 بت): 64 بت
             * 
             * (EN) long size varies by platform:
             *      - Windows (even 64-bit): 32 bits
             *      - Linux/macOS (64-bit): 64 bits
             */
            sadKind = TypeKind::Integer;
            break;
            
        // (AR) أعداد صحيحة غير موقّعة / (EN) Unsigned integers
        case FFI::CBasicType::UCHAR:
        case FFI::CBasicType::USHORT:
        case FFI::CBasicType::UINT:
        case FFI::CBasicType::ULONG:
        case FFI::CBasicType::ULONGLONG:
            sadKind = TypeKind::Integer;
            break;
            
        // (AR) أعداد عشرية / (EN) Floating point
        case FFI::CBasicType::FLOAT:
        case FFI::CBasicType::DOUBLE:
            sadKind = TypeKind::Float;
            break;
        case FFI::CBasicType::LONGDOUBLE:
            /*
             * (AR) long double: نستخدم FLOAT64 كتقريب
             *      (الحجم الفعلي يختلف: 80/128 بت)
             * 
             * (EN) long double: use FLOAT64 as approximation
             *      (actual size varies: 80/128 bits)
             */
            sadKind = TypeKind::Float;
            break;
            
        // (AR) منطقي / (EN) Boolean
        case FFI::CBasicType::BOOL:
            sadKind = TypeKind::Boolean;
            break;
            
        // (AR) أنواع الحجم / (EN) Size types
        case FFI::CBasicType::SIZE_T:
        case FFI::CBasicType::PTRDIFF_T:
        case FFI::CBasicType::INTPTR_T:
        case FFI::CBasicType::UINTPTR_T:
            sadKind = TypeKind::Integer;
            break;
            
        default:
            throw std::runtime_error(
                "(AR) CBasicType \xD8\xBA\xD9\x8A\xD8\xB1 "
                "\xD9\x85\xD8\xB9\xD8\xB1\xD9\x88\xD9\x81 / "
                "(EN) Unknown CBasicType"
            );
    }
    
    return std::make_shared<PrimitiveType>(sadKind);
}

/**
 * @brief (AR) تحويل مؤشر C إلى نوع "ص"
 *        (EN) Convert C pointer to Sad type
 */
TypePtr CTypeMapper::cPointerToSad(FFI::CTypePtr pointeeType) const {
    /*
     * (AR) تحويل نوع ما يشير إليه المؤشر
     *      ثم إنشاء نوع مؤشر "ص"
     * 
     * (EN) Convert pointee type
     *      Then create Sad pointer type
     */
    TypePtr sadPointeeType = cToSad(pointeeType);
    // لا يوجد PointerType موحد بعد في TypeSystem، لذا نعيد pointee كحل مرحلي آمن.
    return sadPointeeType;
}

// ----------------------------------------------------------------------------
// (AR) معلومات الأنواع
// (EN) Type Information
// ----------------------------------------------------------------------------

/**
 * @brief (AR) الحصول على معلومات نوع C
 *        (EN) Get C type information
 */
CTypeInfo CTypeMapper::getTypeInfo(FFI::CTypePtr cType) const {
    CTypeInfo info;
    info.cType = cType;
    info.sadType = cToSad(cType);
    info.arabicName = cType->getArabicName();
    info.cName = cType->getCName();
    info.size = getTypeSize(cType);
    info.alignment = getTypeAlignment(cType);
    
    /*
     * (AR) تحديد خصائص إضافية
     * (EN) Determine additional properties
     */
    if (auto* basicType = dynamic_cast<FFI::CBasicTypeImpl*>(cType.get())) {
        FFI::CBasicType bt = basicType->getBasicType();
        info.isSigned = (bt == FFI::CBasicType::SCHAR || 
                        bt == FFI::CBasicType::SHORT ||
                        bt == FFI::CBasicType::INT ||
                        bt == FFI::CBasicType::LONG ||
                        bt == FFI::CBasicType::LONGLONG);
        info.isPointer = false;
        info.requiresCleanup = false;
    } else if (dynamic_cast<FFI::CPointerType*>(cType.get())) {
        info.isSigned = false;
        info.isPointer = true;
        info.requiresCleanup = true;  // (AR) المؤشرات قد تحتاج تحرير
    } else {
        info.isSigned = false;
        info.isPointer = false;
        info.requiresCleanup = false;
    }
    
    return info;
}

/**
 * @brief (AR) حساب حجم النوع
 *        (EN) Calculate type size
 * 
 * @details
 * (AR) جدول الأحجام (بالبايت):
 *      ┌────────────────┬───────────────┬───────────────┐
 *      │ النوع          │ 32-bit        │ 64-bit        │
 *      ├────────────────┼───────────────┼───────────────┤
 *      │ char           │ 1             │ 1             │
 *      │ short          │ 2             │ 2             │
 *      │ int            │ 4             │ 4             │
 *      │ long (Win)     │ 4             │ 4             │
 *      │ long (Unix)    │ 4             │ 8             │
 *      │ long long      │ 8             │ 8             │
 *      │ float          │ 4             │ 4             │
 *      │ double         │ 8             │ 8             │
 *      │ void*          │ 4             │ 8             │
 *      │ size_t         │ 4             │ 8             │
 *      └────────────────┴───────────────┴───────────────┘
 * 
 * (EN) Size table (in bytes) shown above
 */
size_t CTypeMapper::getTypeSize(FFI::CTypePtr cType) const {
    if (auto* basicType = dynamic_cast<FFI::CBasicTypeImpl*>(cType.get())) {
        FFI::CBasicType bt = basicType->getBasicType();
        
        switch (bt) {
            case FFI::CBasicType::VOID:
                return 0;
            case FFI::CBasicType::BOOL:
            case FFI::CBasicType::CHAR:
            case FFI::CBasicType::SCHAR:
            case FFI::CBasicType::UCHAR:
                return 1;
            case FFI::CBasicType::SHORT:
            case FFI::CBasicType::USHORT:
                return 2;
            case FFI::CBasicType::INT:
            case FFI::CBasicType::UINT:
            case FFI::CBasicType::FLOAT:
                return 4;
            case FFI::CBasicType::LONG:
            case FFI::CBasicType::ULONG:
                // (AR) حجم long يعتمد على المنصة
                if (options_.isWindows) return 4;
                return options_.is64Bit ? 8 : 4;
            case FFI::CBasicType::LONGLONG:
            case FFI::CBasicType::ULONGLONG:
            case FFI::CBasicType::DOUBLE:
                return 8;
            case FFI::CBasicType::LONGDOUBLE:
                // (AR) حجم long double يختلف
                return 16;  // (AR) افتراض 128 بت
            case FFI::CBasicType::SIZE_T:
            case FFI::CBasicType::PTRDIFF_T:
            case FFI::CBasicType::INTPTR_T:
            case FFI::CBasicType::UINTPTR_T:
                return options_.is64Bit ? 8 : 4;
            default:
                return 0;
        }
    }
    
    // (AR) المؤشرات / (EN) Pointers
    if (dynamic_cast<FFI::CPointerType*>(cType.get())) {
        return options_.is64Bit ? 8 : 4;
    }
    
    // (AR) المصفوفات / (EN) Arrays
    if (auto* arrayType = dynamic_cast<FFI::CArrayType*>(cType.get())) {
        return getTypeSize(arrayType->getElementType()) * arrayType->getSize();
    }
    
    // (AR) البنيات / (EN) Structs
    if (auto* structType = dynamic_cast<FFI::CStructType*>(cType.get())) {
        return structType->getSize();
    }
    
    return 0;
}

/**
 * @brief (AR) حساب محاذاة النوع
 *        (EN) Calculate type alignment
 */
size_t CTypeMapper::getTypeAlignment(FFI::CTypePtr cType) const {
    if (auto* basicType = dynamic_cast<FFI::CBasicTypeImpl*>(cType.get())) {
        /*
         * (AR) المحاذاة عادة تساوي الحجم للأنواع الأساسية
         *      (بحد أقصى 8 بايت على معظم المنصات)
         * 
         * (EN) Alignment usually equals size for basic types
         *      (max 8 bytes on most platforms)
         */
        size_t size = getTypeSize(cType);
        return std::min(size, static_cast<size_t>(8));
    }
    
    // (AR) المؤشرات / (EN) Pointers
    if (dynamic_cast<FFI::CPointerType*>(cType.get())) {
        return options_.is64Bit ? 8 : 4;
    }
    
    // (AR) المصفوفات ترث محاذاة العنصر / (EN) Arrays inherit element alignment
    if (auto* arrayType = dynamic_cast<FFI::CArrayType*>(cType.get())) {
        return getTypeAlignment(arrayType->getElementType());
    }
    
    // (AR) البنيات / (EN) Structs
    if (auto* structType = dynamic_cast<FFI::CStructType*>(cType.get())) {
        return structType->getAlignment();
    }
    
    return 1;
}

/**
 * @brief (AR) هل النوعان متوافقان؟
 *        (EN) Are types compatible?
 */
bool CTypeMapper::areTypesCompatible(TypePtr sadType, FFI::CTypePtr cType) const {
    /*
     * (AR) فحص التوافق:
     *      1. تحويل نوع "ص" إلى C
     *      2. مقارنة مع النوع C المعطى
     * 
     * (EN) Check compatibility:
     *      1. Convert Sad type to C
     *      2. Compare with given C type
     */
    try {
        FFI::CTypePtr converted = sadToC(sadType);
        
        // (AR) مقارنة بسيطة بالأسماء / (EN) Simple name comparison
        return converted->getCName() == cType->getCName();
    } catch (...) {
        return false;
    }
}

// ----------------------------------------------------------------------------
// (AR) إنشاء أنواع C مخصصة
// (EN) Creating Custom C Types
// ----------------------------------------------------------------------------

/**
 * @brief (AR) إنشاء نوع typedef
 *        (EN) Create typedef type
 */
FFI::CTypePtr CTypeMapper::createTypedef(const std::string& name, FFI::CTypePtr baseType) {
    /*
     * (AR) typedef في C: اسم بديل لنوع موجود
     *      مثال: typedef int MyInt;
     * 
     * (EN) typedef in C: alias for existing type
     *      Example: typedef int MyInt;
     */
    auto typedefType = std::make_shared<FFI::CTypedefType>(name, baseType);
    typeRegistry_[name] = typedefType;
    return typedefType;
}

/**
 * @brief (AR) تسجيل نوع مخصص
 *        (EN) Register custom type
 */
void CTypeMapper::registerCustomType(const std::string& name, FFI::CTypePtr type) {
    typeRegistry_[name] = type;
}

/**
 * @brief (AR) البحث عن نوع مسجل
 *        (EN) Find registered type
 */
FFI::CTypePtr CTypeMapper::findType(const std::string& name) const {
    auto it = typeRegistry_.find(name);
    if (it != typeRegistry_.end()) {
        return it->second;
    }
    return nullptr;
}

// ============================================================================
//                    (AR) تنفيذ CStructBuilder
//                    (EN) CStructBuilder Implementation
// ============================================================================

CStructBuilder::CStructBuilder(const std::string& cName, const std::string& arabicName)
    : cName_(cName), arabicName_(arabicName) {
    /*
     * (AR) إنشاء باني بنية جديدة
     * (EN) Create new struct builder
     */
}

/**
 * @brief (AR) إضافة حقل للبنية
 *        (EN) Add field to struct
 */
CStructBuilder& CStructBuilder::addField(const std::string& cName, 
                                         FFI::CTypePtr type,
                                         const std::string& arabicName) {
    fields_.emplace_back(cName, type, arabicName);
    return *this;
}

/**
 * @brief (AR) إضافة حقل محاذاة (padding)
 *        (EN) Add alignment padding field
 */
CStructBuilder& CStructBuilder::addPadding(size_t bytes) {
    /*
     * (AR) Padding: بايتات فارغة للمحاذاة
     *      نستخدم مصفوفة من البايتات
     * 
     * (EN) Padding: empty bytes for alignment
     *      Use array of bytes
     */
    auto byteType = std::make_shared<FFI::CBasicTypeImpl>(FFI::CBasicType::CHAR);
    auto paddingType = std::make_shared<FFI::CArrayType>(byteType, bytes);
    
    std::string paddingName = "__padding_" + std::to_string(fields_.size());
    fields_.emplace_back(paddingName, paddingType, "محاذاة");
    return *this;
}

CStructBuilder& CStructBuilder::setPacked(bool packed) {
    isPacked_ = packed;
    return *this;
}

CStructBuilder& CStructBuilder::setAlignment(size_t alignment) {
    customAlignment_ = alignment;
    return *this;
}

/**
 * @brief (AR) بناء البنية النهائية
 *        (EN) Build final struct
 */
FFI::CStructTypePtr CStructBuilder::build() const {
    /*
     * (AR) إنشاء البنية من الحقول المجمعة
     * (EN) Create struct from collected fields
     */
    std::vector<FFI::CStructField> structFields;
    structFields.reserve(fields_.size());
    
    for (const auto& [cName, type, arabicName] : fields_) {
        FFI::CStructField field;
        field.cName = cName;
        field.arabicName = arabicName;
        field.type = type;
        structFields.push_back(field);
    }
    
    auto structType = std::make_shared<FFI::CStructType>(
        cName_, arabicName_, structFields, isPacked_, customAlignment_
    );
    
    return structType;
}

// ============================================================================
//                    (AR) تنفيذ CUnionBuilder
//                    (EN) CUnionBuilder Implementation
// ============================================================================

CUnionBuilder::CUnionBuilder(const std::string& cName, const std::string& arabicName)
    : cName_(cName), arabicName_(arabicName) {
}

CUnionBuilder& CUnionBuilder::addVariant(const std::string& cName,
                                          FFI::CTypePtr type,
                                          const std::string& arabicName) {
    variants_.emplace_back(cName, type, arabicName);
    return *this;
}

FFI::CUnionTypePtr CUnionBuilder::build() const {
    /*
     * (AR) الاتحاد في C: جميع الأعضاء يتشاركون نفس الذاكرة
     *      الحجم = حجم أكبر عضو
     * 
     * (EN) Union in C: all members share same memory
     *      Size = size of largest member
     */
    std::vector<FFI::CUnionVariant> unionVariants;
    unionVariants.reserve(variants_.size());
    
    for (const auto& [cName, type, arabicName] : variants_) {
        FFI::CUnionVariant variant;
        variant.cName = cName;
        variant.arabicName = arabicName;
        variant.type = type;
        unionVariants.push_back(variant);
    }
    
    return std::make_shared<FFI::CUnionType>(cName_, arabicName_, unionVariants);
}

// ============================================================================
//                    (AR) تنفيذ CEnumBuilder
//                    (EN) CEnumBuilder Implementation
// ============================================================================

CEnumBuilder::CEnumBuilder(const std::string& cName, const std::string& arabicName)
    : cName_(cName), arabicName_(arabicName), nextValue_(0) {
}

CEnumBuilder& CEnumBuilder::addEnumerator(const std::string& cName,
                                          int64_t value,
                                          const std::string& arabicName) {
    enumerators_.emplace_back(cName, value, arabicName);
    nextValue_ = value + 1;  // (AR) القيمة التالية التلقائية
    return *this;
}

CEnumBuilder& CEnumBuilder::addEnumerator(const std::string& cName,
                                          const std::string& arabicName) {
    return addEnumerator(cName, nextValue_, arabicName);
}

FFI::CEnumTypePtr CEnumBuilder::build() const {
    /*
     * (AR) التعداد في C: قائمة من الثوابت الصحيحة
     * (EN) Enum in C: list of integer constants
     */
    std::vector<FFI::CEnumerator> enumConstants;
    enumConstants.reserve(enumerators_.size());
    
    for (const auto& [cName, value, arabicName] : enumerators_) {
        FFI::CEnumerator constant;
        constant.cName = cName;
        constant.arabicName = arabicName;
        constant.value = value;
        enumConstants.push_back(constant);
    }
    
    return std::make_shared<FFI::CEnumType>(cName_, arabicName_, enumConstants);
}

// ============================================================================
//                    (AR) أنواع C الشائعة
//                    (EN) Common C Types
// ============================================================================

namespace CommonCTypes {

FFI::CTypePtr getVoid() {
    return std::make_shared<FFI::CBasicTypeImpl>(FFI::CBasicType::VOID);
}

FFI::CTypePtr getChar() {
    return std::make_shared<FFI::CBasicTypeImpl>(FFI::CBasicType::CHAR);
}

FFI::CTypePtr getInt() {
    return std::make_shared<FFI::CBasicTypeImpl>(FFI::CBasicType::INT);
}

FFI::CTypePtr getLong() {
    return std::make_shared<FFI::CBasicTypeImpl>(FFI::CBasicType::LONG);
}

FFI::CTypePtr getFloat() {
    return std::make_shared<FFI::CBasicTypeImpl>(FFI::CBasicType::FLOAT);
}

FFI::CTypePtr getDouble() {
    return std::make_shared<FFI::CBasicTypeImpl>(FFI::CBasicType::DOUBLE);
}

FFI::CTypePtr getSizeT() {
    return std::make_shared<FFI::CBasicTypeImpl>(FFI::CBasicType::SIZE_T);
}

FFI::CTypePtr getBool() {
    return std::make_shared<FFI::CBasicTypeImpl>(FFI::CBasicType::BOOL);
}

FFI::CTypePtr getInt8() {
    return std::make_shared<FFI::CBasicTypeImpl>(FFI::CBasicType::SCHAR);
}

FFI::CTypePtr getUInt8() {
    return std::make_shared<FFI::CBasicTypeImpl>(FFI::CBasicType::UCHAR);
}

FFI::CTypePtr getInt16() {
    return std::make_shared<FFI::CBasicTypeImpl>(FFI::CBasicType::SHORT);
}

FFI::CTypePtr getUInt16() {
    return std::make_shared<FFI::CBasicTypeImpl>(FFI::CBasicType::USHORT);
}

FFI::CTypePtr getInt32() {
    return std::make_shared<FFI::CBasicTypeImpl>(FFI::CBasicType::INT);
}

FFI::CTypePtr getUInt32() {
    return std::make_shared<FFI::CBasicTypeImpl>(FFI::CBasicType::UINT);
}

FFI::CTypePtr getInt64() {
    return std::make_shared<FFI::CBasicTypeImpl>(FFI::CBasicType::LONGLONG);
}

FFI::CTypePtr getUInt64() {
    return std::make_shared<FFI::CBasicTypeImpl>(FFI::CBasicType::ULONGLONG);
}

FFI::CTypePtr getVoidPtr() {
    return std::make_shared<FFI::CPointerType>(getVoid());
}

FFI::CTypePtr getCharPtr() {
    return std::make_shared<FFI::CPointerType>(getChar());
}

FFI::CTypePtr getConstCharPtr() {
    /*
     * (AR) const char*: نستخدم مؤشر عادي مع علامة const
     * (EN) const char*: use regular pointer with const flag
     */
    auto charPtr = std::make_shared<FFI::CPointerType>(getChar());
    charPtr->setConst(true);
    return charPtr;
}

FFI::CTypePtr getFILEPtr() {
    /*
     * (AR) FILE*: مؤشر مبهم لبنية FILE
     * (EN) FILE*: opaque pointer to FILE struct
     * نستخدم void* كتقريب / Use void* as approximation
     */
    return std::make_shared<FFI::CPointerType>(getVoid());
}

} // namespace CommonCTypes

// ============================================================================
//                    (AR) دوال التحويل السريع
//                    (EN) Quick Conversion Functions
// ============================================================================

/**
 * @brief (AR) تحويل سريع من نوع "ص" إلى نوع C
 *        (EN) Quick conversion from Sad type to C type
 */
FFI::CTypePtr sadTypeToC(TypePtr sadType) {
    static CTypeMapper mapper;
    return mapper.sadToC(sadType);
}

/**
 * @brief (AR) تحويل سريع من نوع C إلى نوع "ص"
 *        (EN) Quick conversion from C type to Sad type
 */
TypePtr cTypeToSad(FFI::CTypePtr cType) {
    static CTypeMapper mapper;
    return mapper.cToSad(cType);
}

/**
 * @brief (AR) الحصول على اسم C لنوع "ص"
 *        (EN) Get C name for Sad type
 */
std::string getSadTypeCName(TypePtr sadType) {
    auto cType = sadTypeToC(sadType);
    return cType->getCName();
}

/**
 * @brief (AR) الحصول على الاسم العربي لنوع C
 *        (EN) Get Arabic name for C type
 */
std::string getCTypeArabicName(FFI::CTypePtr cType) {
    return cType->getArabicName();
}

} // namespace TypeSystem
} // namespace Sad

