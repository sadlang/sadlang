// تعطيل تحذير Unicode للنصوص العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file c_types.h
 * @brief (AR) نظام تعيين أنواع C لـ FFI - تحويل بين أنواع "ص" وأنواع C
 *        (EN) C Type Mapping System for FFI - Conversion between Sad and C types
 * 
 * @details
 * (AR) هذا الملف يحتوي على نظام تعيين الأنواع بين لغة "ص" ولغة C.
 *      يتيح هذا النظام:
 *      - تحويل أنواع "ص" الأساسية إلى أنواع C المكافئة
 *      - تحويل أنواع C إلى أنواع "ص" المكافئة
 *      - التعامل مع الفروقات بين المنصات (حجم long, محاذاة البنيات, إلخ)
 *      - إنشاء أنواع C مركبة (هياكل، اتحادات، تعدادات)
 * 
 *      جدول التعيين الأساسي:
 *      ┌──────────────────┬────────────────┬───────────────┐
 *      │ نوع "ص" (عربي)   │ نوع C          │ الحجم (بت)    │
 *      ├──────────────────┼────────────────┼───────────────┤
 *      │ ص8               │ int8_t         │ 8             │
 *      │ ص16              │ int16_t        │ 16            │
 *      │ ص32              │ int32_t        │ 32            │
 *      │ ص64              │ int64_t        │ 64            │
 *      │ ص_غ8             │ uint8_t        │ 8             │
 *      │ ص_غ16            │ uint16_t       │ 16            │
 *      │ ص_غ32            │ uint32_t       │ 32            │
 *      │ ص_غ64            │ uint64_t       │ 64            │
 *      │ عش32             │ float          │ 32            │
 *      │ عش64             │ double         │ 64            │
 *      │ منطقي            │ bool/_Bool     │ 8             │
 *      │ حرف              │ char           │ 8             │
 *      │ نص               │ char*          │ ptr           │
 *      │ حجم              │ size_t         │ 32/64         │
 *      └──────────────────┴────────────────┴───────────────┘
 * 
 * (EN) This file contains the type mapping system between Sad and C languages.
 *      This system enables:
 *      - Converting Sad basic types to equivalent C types
 *      - Converting C types to equivalent Sad types
 *      - Handling platform differences (long size, struct alignment, etc.)
 *      - Creating composite C types (structs, unions, enums)
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2025-01-18
 * @version 1.0.0
 * @license MIT License
 */

#ifndef SAD_TYPES_C_TYPES_H
#define SAD_TYPES_C_TYPES_H

#include "ffi/c_abi.h"
#include "types/type.h"
#include "types/primitive_type.h"
#include "types/struct_types.h"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>

namespace Sad {
namespace TypeSystem {

// ============================================================================
//                    (AR) إعادة توجيه الأنواع
//                    (EN) Type Forward Declarations
// ============================================================================

class CTypeMapper;
class CStructBuilder;
class CUnionBuilder;
class CEnumBuilder;

using CTypeMapperPtr = std::shared_ptr<CTypeMapper>;

// ============================================================================
//                    (AR) معلومات النوع
//                    (EN) Type Information
// ============================================================================

/**
 * @struct CTypeInfo
 * @brief (AR) معلومات تفصيلية عن نوع C
 *        (EN) Detailed information about C type
 */
struct CTypeInfo {
    FFI::CTypePtr cType;            // نوع C
    TypePtr sadType;                // نوع ص المكافئ
    std::string arabicName;         // الاسم العربي
    std::string cName;              // اسم C
    size_t size;                    // الحجم بالبايت
    size_t alignment;               // المحاذاة بالبايت
    bool isSigned;                  // هل موقّع؟
    bool isPointer;                 // هل مؤشر؟
    bool requiresCleanup;           // هل يحتاج تنظيف؟
};

// ============================================================================
//                    (AR) خيارات التحويل
//                    (EN) Conversion Options
// ============================================================================

/**
 * @struct TypeConversionOptions
 * @brief (AR) خيارات تحويل الأنواع بين "ص" و C
 *        (EN) Type conversion options between Sad and C
 */
struct TypeConversionOptions {
    bool is64Bit = true;                    // منصة 64 بت؟
    bool isWindows = true;                  // Windows؟
    bool strictAlignment = true;            // محاذاة صارمة؟
    bool warnOnNarrowing = true;            // تحذير عند التضييق؟
    bool allowImplicitConversion = false;   // السماح بالتحويل الضمني؟
    size_t maxPointerDepth = 10;            // أقصى عمق للمؤشرات
};

// ============================================================================
//                    (AR) محول الأنواع
//                    (EN) Type Mapper
// ============================================================================

/**
 * @class CTypeMapper
 * @brief (AR) محول الأنواع بين "ص" و C
 *        (EN) Type mapper between Sad and C
 * 
 * @details
 * (AR) يوفر هذا الصنف تحويلاً ثنائي الاتجاه بين أنواع "ص" وأنواع C.
 *      يتعامل مع جميع الأنواع الأساسية والمركبة.
 * 
 * (EN) This class provides bidirectional conversion between Sad and C types.
 *      Handles all basic and composite types.
 */
class CTypeMapper {
public:
    /**
     * @brief (AR) بناء محول الأنواع
     *        (EN) Construct type mapper
     * 
     * @param options (AR) خيارات التحويل (EN) Conversion options
     */
    explicit CTypeMapper(const TypeConversionOptions& options = TypeConversionOptions());
    
    ~CTypeMapper() = default;
    
    // ========================================================================
    // (AR) تحويل من "ص" إلى C
    // (EN) Conversion from Sad to C
    // ========================================================================
    
    /**
     * @brief (AR) تحويل نوع "ص" إلى نوع C
     *        (EN) Convert Sad type to C type
     * 
     * @param sadType (AR) نوع "ص" (EN) Sad type
     * @return (AR) نوع C المكافئ (EN) Equivalent C type
     */
    FFI::CTypePtr sadToC(TypePtr sadType) const;
    
    /**
     * @brief (AR) تحويل نوع "ص" الأساسي إلى CBasicType
     *        (EN) Convert Sad primitive type to CBasicType
     */
    FFI::CBasicType sadPrimitiveToCBasic(const PrimitiveType& prim) const;
    
    /**
     * @brief (AR) تحويل مؤشر "ص" إلى مؤشر C
     *        (EN) Convert Sad pointer to C pointer
     */
    FFI::CTypePtr sadPointerToC(TypePtr pointeeType) const;
    
    /**
     * @brief (AR) تحويل مصفوفة "ص" إلى مصفوفة C
     *        (EN) Convert Sad array to C array
     */
    FFI::CTypePtr sadArrayToC(TypePtr elementType, size_t size) const;
    
    /**
     * @brief (AR) تحويل بنية "ص" إلى بنية C
     *        (EN) Convert Sad struct to C struct
     */
    FFI::CStructTypePtr sadStructToC(const StructType& sadStruct) const;
    
    // ========================================================================
    // (AR) تحويل من C إلى "ص"
    // (EN) Conversion from C to Sad
    // ========================================================================
    
    /**
     * @brief (AR) تحويل نوع C إلى نوع "ص"
     *        (EN) Convert C type to Sad type
     * 
     * @param cType (AR) نوع C (EN) C type
     * @return (AR) نوع "ص" المكافئ (EN) Equivalent Sad type
     */
    TypePtr cToSad(FFI::CTypePtr cType) const;
    
    /**
     * @brief (AR) تحويل CBasicType إلى نوع "ص" أساسي
     *        (EN) Convert CBasicType to Sad primitive type
     */
    TypePtr cBasicToSad(FFI::CBasicType cBasic) const;
    
    /**
     * @brief (AR) تحويل مؤشر C إلى نوع "ص"
     *        (EN) Convert C pointer to Sad type
     */
    TypePtr cPointerToSad(FFI::CTypePtr pointeeType) const;
    
    // ========================================================================
    // (AR) معلومات الأنواع
    // (EN) Type Information
    // ========================================================================
    
    /**
     * @brief (AR) الحصول على معلومات نوع C
     *        (EN) Get C type information
     */
    CTypeInfo getTypeInfo(FFI::CTypePtr cType) const;
    
    /**
     * @brief (AR) حساب حجم النوع
     *        (EN) Calculate type size
     */
    size_t getTypeSize(FFI::CTypePtr cType) const;
    
    /**
     * @brief (AR) حساب محاذاة النوع
     *        (EN) Calculate type alignment
     */
    size_t getTypeAlignment(FFI::CTypePtr cType) const;
    
    /**
     * @brief (AR) هل النوعان متوافقان؟
     *        (EN) Are types compatible?
     */
    bool areTypesCompatible(TypePtr sadType, FFI::CTypePtr cType) const;
    
    // ========================================================================
    // (AR) إنشاء أنواع C مخصصة
    // (EN) Creating Custom C Types
    // ========================================================================
    
    /**
     * @brief (AR) إنشاء نوع typedef
     *        (EN) Create typedef type
     */
    FFI::CTypePtr createTypedef(const std::string& name, FFI::CTypePtr baseType);
    
    /**
     * @brief (AR) تسجيل نوع مخصص
     *        (EN) Register custom type
     */
    void registerCustomType(const std::string& name, FFI::CTypePtr type);
    
    /**
     * @brief (AR) البحث عن نوع مسجل
     *        (EN) Find registered type
     */
    FFI::CTypePtr findType(const std::string& name) const;
    
    // ========================================================================
    // (AR) خيارات المنصة
    // (EN) Platform Options
    // ========================================================================
    
    void setOptions(const TypeConversionOptions& options) { options_ = options; }
    const TypeConversionOptions& getOptions() const { return options_; }
    
    void setIs64Bit(bool value) { options_.is64Bit = value; }
    void setIsWindows(bool value) { options_.isWindows = value; }

private:
    TypeConversionOptions options_;                     // خيارات التحويل
    std::unordered_map<std::string, FFI::CTypePtr> typeRegistry_;  // سجل الأنواع
    
    // (AR) دوال مساعدة / (EN) Helper functions
    FFI::CBasicType primitiveKindToCBasic(TypeKind kind) const;
};

// ============================================================================
//                    (AR) باني البنية C
//                    (EN) C Struct Builder
// ============================================================================

/**
 * @class CStructBuilder
 * @brief (AR) باني لإنشاء بنيات C بشكل برمجي
 *        (EN) Builder for creating C structs programmatically
 * 
 * @example
 * @code{.cpp}
 * // إنشاء بنية C / Create C struct
 * CStructBuilder builder("Point", "نقطة");
 * builder.addField("x", makeBasicType(CBasicType::INT), "س");
 * builder.addField("y", makeBasicType(CBasicType::INT), "ص");
 * auto pointStruct = builder.build();
 * @endcode
 */
class CStructBuilder {
public:
    /**
     * @brief (AR) بناء باني البنية
     *        (EN) Construct struct builder
     * 
     * @param cName (AR) اسم البنية بـ C (EN) C struct name
     * @param arabicName (AR) الاسم العربي (EN) Arabic name
     */
    CStructBuilder(const std::string& cName, const std::string& arabicName = "");
    
    // --- (AR) إضافة الحقول / (EN) Adding fields ---
    
    /**
     * @brief (AR) إضافة حقل للبنية
     *        (EN) Add field to struct
     * 
     * @param cName (AR) اسم الحقل بـ C (EN) C field name
     * @param type (AR) نوع الحقل (EN) Field type
     * @param arabicName (AR) الاسم العربي (اختياري) (EN) Arabic name (optional)
     * @return (AR) مرجع للباني للتسلسل (EN) Reference to builder for chaining
     */
    CStructBuilder& addField(const std::string& cName, 
                            FFI::CTypePtr type,
                            const std::string& arabicName = "");
    
    /**
     * @brief (AR) إضافة حقل محاذاة (padding)
     *        (EN) Add alignment padding field
     */
    CStructBuilder& addPadding(size_t bytes);
    
    // --- (AR) خيارات البنية / (EN) Struct options ---
    
    /**
     * @brief (AR) تعيين packed (بدون محاذاة)
     *        (EN) Set packed (no alignment)
     */
    CStructBuilder& setPacked(bool packed = true);
    
    /**
     * @brief (AR) تعيين محاذاة مخصصة
     *        (EN) Set custom alignment
     */
    CStructBuilder& setAlignment(size_t alignment);
    
    // --- (AR) البناء / (EN) Building ---
    
    /**
     * @brief (AR) بناء البنية النهائية
     *        (EN) Build final struct
     */
    FFI::CStructTypePtr build() const;

private:
    std::string cName_;             // اسم C
    std::string arabicName_;        // الاسم العربي
    std::vector<std::tuple<std::string, FFI::CTypePtr, std::string>> fields_;  // الحقول
    bool isPacked_ = false;         // بدون محاذاة؟
    size_t customAlignment_ = 0;    // محاذاة مخصصة
};

// ============================================================================
//                    (AR) باني الاتحاد C
//                    (EN) C Union Builder
// ============================================================================

/**
 * @class CUnionBuilder
 * @brief (AR) باني لإنشاء اتحادات C بشكل برمجي
 *        (EN) Builder for creating C unions programmatically
 */
class CUnionBuilder {
public:
    CUnionBuilder(const std::string& cName, const std::string& arabicName = "");
    
    CUnionBuilder& addVariant(const std::string& cName,
                             FFI::CTypePtr type,
                             const std::string& arabicName = "");
    
    FFI::CUnionTypePtr build() const;

private:
    std::string cName_;
    std::string arabicName_;
    std::vector<std::tuple<std::string, FFI::CTypePtr, std::string>> variants_;
};

// ============================================================================
//                    (AR) باني التعداد C
//                    (EN) C Enum Builder
// ============================================================================

/**
 * @class CEnumBuilder
 * @brief (AR) باني لإنشاء تعدادات C بشكل برمجي
 *        (EN) Builder for creating C enums programmatically
 */
class CEnumBuilder {
public:
    CEnumBuilder(const std::string& cName, const std::string& arabicName = "");
    
    /**
     * @brief (AR) إضافة عنصر للتعداد
     *        (EN) Add enumerator
     * 
     * @param cName (AR) اسم العنصر بـ C (EN) C enumerator name
     * @param value (AR) القيمة (EN) Value
     * @param arabicName (AR) الاسم العربي (EN) Arabic name
     */
    CEnumBuilder& addEnumerator(const std::string& cName,
                               int64_t value,
                               const std::string& arabicName = "");
    
    /**
     * @brief (AR) إضافة عنصر بقيمة تلقائية
     *        (EN) Add enumerator with auto value
     */
    CEnumBuilder& addEnumerator(const std::string& cName,
                               const std::string& arabicName = "");
    
    FFI::CEnumTypePtr build() const;

private:
    std::string cName_;
    std::string arabicName_;
    std::vector<std::tuple<std::string, int64_t, std::string>> enumerators_;
    int64_t nextValue_ = 0;
};

// ============================================================================
//                    (AR) أنواع C الشائعة المُعرّفة مسبقاً
//                    (EN) Common Predefined C Types
// ============================================================================

/**
 * @namespace CommonCTypes
 * @brief (AR) أنواع C الشائعة المُعرّفة مسبقاً
 *        (EN) Common predefined C types
 */
namespace CommonCTypes {
    // --- (AR) أنواع أساسية / (EN) Basic types ---
    FFI::CTypePtr getVoid();
    FFI::CTypePtr getChar();
    FFI::CTypePtr getInt();
    FFI::CTypePtr getLong();
    FFI::CTypePtr getFloat();
    FFI::CTypePtr getDouble();
    FFI::CTypePtr getSizeT();
    FFI::CTypePtr getBool();
    
    // --- (AR) أنواع ثابتة الحجم / (EN) Fixed-size types ---
    FFI::CTypePtr getInt8();
    FFI::CTypePtr getUInt8();
    FFI::CTypePtr getInt16();
    FFI::CTypePtr getUInt16();
    FFI::CTypePtr getInt32();
    FFI::CTypePtr getUInt32();
    FFI::CTypePtr getInt64();
    FFI::CTypePtr getUInt64();
    
    // --- (AR) مؤشرات شائعة / (EN) Common pointers ---
    FFI::CTypePtr getVoidPtr();
    FFI::CTypePtr getCharPtr();
    FFI::CTypePtr getConstCharPtr();
    
    // --- (AR) أنواع FILE و stream / (EN) FILE and stream types ---
    FFI::CTypePtr getFILEPtr();
}

// ============================================================================
//                    (AR) دوال تحويل سريعة
//                    (EN) Quick Conversion Functions
// ============================================================================

/**
 * @brief (AR) تحويل سريع من نوع "ص" إلى نوع C
 *        (EN) Quick conversion from Sad type to C type
 */
FFI::CTypePtr sadTypeToC(TypePtr sadType);

/**
 * @brief (AR) تحويل سريع من نوع C إلى نوع "ص"
 *        (EN) Quick conversion from C type to Sad type
 */
TypePtr cTypeToSad(FFI::CTypePtr cType);

/**
 * @brief (AR) الحصول على اسم C لنوع "ص"
 *        (EN) Get C name for Sad type
 */
std::string getSadTypeCName(TypePtr sadType);

/**
 * @brief (AR) الحصول على الاسم العربي لنوع C
 *        (EN) Get Arabic name for C type
 */
std::string getCTypeArabicName(FFI::CTypePtr cType);

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_TYPES_C_TYPES_H
