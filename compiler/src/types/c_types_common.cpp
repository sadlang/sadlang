#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif
// (AR) أنواع C الشائعة — CommonCTypes namespace
// (EN) Common C Types utilities
#include "types/c_types.h"
#include "types/composite_type_classes.h"
#include <stdexcept>
#include <cassert>
namespace Sad {
namespace TypeSystem {
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
} // namespace TypeSystem
} // namespace Sad
