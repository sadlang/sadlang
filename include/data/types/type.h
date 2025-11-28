// ======================================================================
// type.h - نظام الأنواع الأساسي / Basic Type System
// ======================================================================
// الوصف بالعربية:
//   تعريف بسيط لنظام الأنواع في لغة ص
//   يوفر فئة Type الأساسية
//
// English Description:
//   Simple type system definition for S Language
//   Provides basic Type class
// ======================================================================

#pragma once

#include <string>

namespace Sad {
namespace Data {

/**
 * @class Type
 * @brief (AR) نوع بيانات أساسي
 * @brief (EN) Basic data type
 */
class Type {
public:
    std::string name;  ///< (AR) اسم النوع / (EN) type name
    
    /**
     * @brief (AR) منشئ
     * @brief (EN) Constructor
     */
    explicit Type(const std::string& typeName) : name(typeName) {}
    
    /**
     * @brief (AR) هادم افتراضي
     * @brief (EN) Virtual destructor
     */
    virtual ~Type() = default;
    
    /**
     * @brief (AR) التحقق من تساوي الأنواع
     * @brief (EN) Check type equality
     */
    virtual bool isEqual(const Type* other) const {
        if (!other) return false;
        return name == other->name;
    }
    
    /**
     * @brief (AR) التحقق من إمكانية التحويل
     * @brief (EN) Check type convertibility
     */
    virtual bool isConvertibleTo(const Type* target) const {
        return isEqual(target);
    }
    
    /**
     * @brief (AR) الحصول على اسم النوع
     * @brief (EN) Get type name
     */
    std::string getName() const { return name; }
};

} // namespace Data
} // namespace Sad
