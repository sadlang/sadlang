/*
 * ============================================================================
 * نظام الأنواع - Typed AST Implementation
 * Type System - Typed AST Implementation
 * ============================================================================
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 31, 2025
 * الإصدار (Version): 1.0.0
 * ============================================================================
 */

#include "../include/typed_ast.h"
#include <sstream>
#include <algorithm>

namespace Sad {
namespace TypeChecker {

// ============================================================================
// Type Implementation / تنفيذ Type
// ============================================================================

/**
 * مقارنة نوعين / Compare two types
 * 
 * @param other النوع الآخر للمقارنة / Other type to compare
 * @return true إذا كان النوعان متطابقان / true if types are equal
 */
bool Type::equals(const Type* other) const {
    // التحقق من null / Check for null
    if (!other) return false;
    
    // المقارنة الأساسية بالنوع والاسم / Basic comparison by kind and name
    return kind_ == other->kind_ && name_ == other->name_;
}

/**
 * هل يمكن تحويل هذا النوع إلى نوع آخر / Can convert to another type
 * 
 * @param target النوع المستهدف / Target type
 * @return true إذا كان التحويل ممكناً / true if conversion is possible
 */
bool Type::canCastTo(const Type* target) const {
    // التحقق من null / Check for null
    if (!target) return false;
    
    // نفس النوع يمكن التحويل دائماً / Same type can always cast
    if (equals(target)) return true;
    
    // الأنواع الرقمية يمكن تحويلها لبعضها / Numeric types can convert to each other
    if (isPrimitive() && target->isPrimitive()) {
        Kind k1 = getKind();
        Kind k2 = target->getKind();
        
        // int ↔ float / عدد صحيح ↔ عدد عشري
        if ((k1 == Kind::INT && k2 == Kind::FLOAT) ||
            (k1 == Kind::FLOAT && k2 == Kind::INT)) {
            return true;
        }
        
        // أي شيء ↔ string (تحويل نصي) / anything ↔ string (text conversion)
        if (k2 == Kind::STRING) {
            return true;
        }
    }
    
    // الصنف يمكن تحويله للصنف الأساسي / Class can cast to base class
    if (kind_ == Kind::CLASS && target->kind_ == Kind::CLASS) {
        const ClassType* classType = dynamic_cast<const ClassType*>(this);
        const ClassType* targetClass = dynamic_cast<const ClassType*>(target);
        if (classType && targetClass) {
            return classType->inheritsFrom(targetClass);
        }
    }
    
    return false;
}

/**
 * تحويل النوع لنص / Convert type to string
 * 
 * @return تمثيل نصي للنوع / String representation of type
 */
std::string Type::toString() const {
    std::stringstream ss;
    
    // إضافة const إذا كان ثابتاً / Add const if constant
    if (isConst_) {
        ss << "ثابت ";
    }
    
    // إضافة الاسم / Add name
    ss << name_;
    
    // إضافة nullable إذا كان قابلاً لـ null / Add nullable if nullable
    if (isNullable_) {
        ss << "؟";
    }
    
    return ss.str();
}

/**
 * هل النوع أساسي / Is type primitive
 * 
 * @return true إذا كان النوع أساسياً / true if type is primitive
 */
bool Type::isPrimitive() const {
    return kind_ == Kind::INT ||
           kind_ == Kind::FLOAT ||
           kind_ == Kind::BOOL ||
           kind_ == Kind::STRING ||
           kind_ == Kind::CHAR ||
           kind_ == Kind::VOID;
}

/**
 * هل النوع مركب / Is type compound
 * 
 * @return true إذا كان النوع مركباً / true if type is compound
 */
bool Type::isCompound() const {
    return kind_ == Kind::ARRAY ||
           kind_ == Kind::DICT ||
           kind_ == Kind::TUPLE;
}

/**
 * هل النوع دالة / Is type function
 * 
 * @return true إذا كان النوع دالة / true if type is function
 */
bool Type::isFunction() const {
    return kind_ == Kind::FUNCTION ||
           kind_ == Kind::METHOD ||
           kind_ == Kind::LAMBDA;
}

// ============================================================================
// ArrayType Implementation / تنفيذ ArrayType
// ============================================================================

/**
 * تحويل نوع المصفوفة لنص / Convert array type to string
 * 
 * @return تمثيل نصي / String representation
 */
std::string ArrayType::toString() const {
    std::stringstream ss;
    ss << "مصفوفة<" << elementType_->toString() << ">";
    if (size_ >= 0) {
        ss << "[" << size_ << "]";
    }
    return ss.str();
}

/**
 * مقارنة نوعي مصفوفة / Compare array types
 * 
 * @param other النوع الآخر / Other type
 * @return true إذا كانا متطابقين / true if equal
 */
bool ArrayType::equals(const Type* other) const {
    // التحقق من أنه نوع مصفوفة / Check it's an array type
    if (!other || other->getKind() != Kind::ARRAY) {
        return false;
    }
    
    // المقارنة بنوع العناصر / Compare element types
    const ArrayType* otherArray = dynamic_cast<const ArrayType*>(other);
    if (!otherArray) {
        return false;
    }
    
    // المصفوفات بنفس نوع العناصر متطابقة / Arrays with same element type are equal
    // الحجم غير مهم / Size doesn't matter
    return elementType_->equals(otherArray->elementType_.get());
}

// ============================================================================
// DictType Implementation / تنفيذ DictType
// ============================================================================

/**
 * تحويل نوع القاموس لنص / Convert dict type to string
 * 
 * @return تمثيل نصي / String representation
 */
std::string DictType::toString() const {
    std::stringstream ss;
    ss << "قاموس<" << keyType_->toString() 
       << ", " << valueType_->toString() << ">";
    return ss.str();
}

/**
 * مقارنة نوعي قاموس / Compare dict types
 * 
 * @param other النوع الآخر / Other type
 * @return true إذا كانا متطابقين / true if equal
 */
bool DictType::equals(const Type* other) const {
    // التحقق من أنه نوع قاموس / Check it's a dict type
    if (!other || other->getKind() != Kind::DICT) {
        return false;
    }
    
    // المقارنة بأنواع المفاتيح والقيم / Compare key and value types
    const DictType* otherDict = dynamic_cast<const DictType*>(other);
    if (!otherDict) {
        return false;
    }
    
    return keyType_->equals(otherDict->keyType_.get()) &&
           valueType_->equals(otherDict->valueType_.get());
}

// ============================================================================
// FunctionType Implementation / تنفيذ FunctionType
// ============================================================================

/**
 * تحويل نوع الدالة لنص / Convert function type to string
 * 
 * @return تمثيل نصي / String representation
 */
std::string FunctionType::toString() const {
    std::stringstream ss;
    ss << "دالة(";
    
    // إضافة أنواع المعاملات / Add parameter types
    for (size_t i = 0; i < paramTypes_.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << paramTypes_[i]->toString();
    }
    
    // إضافة variadic / Add variadic
    if (isVariadic_) {
        if (!paramTypes_.empty()) ss << ", ";
        ss << "...";
    }
    
    ss << ") -> " << returnType_->toString();
    return ss.str();
}

/**
 * مقارنة نوعي دالة / Compare function types
 * 
 * @param other النوع الآخر / Other type
 * @return true إذا كانا متطابقين / true if equal
 */
bool FunctionType::equals(const Type* other) const {
    // التحقق من أنه نوع دالة / Check it's a function type
    if (!other || !other->isFunction()) {
        return false;
    }
    
    // المقارنة بالمعاملات ونوع الإرجاع / Compare parameters and return type
    const FunctionType* otherFunc = dynamic_cast<const FunctionType*>(other);
    if (!otherFunc) {
        return false;
    }
    
    // عدد المعاملات يجب أن يكون متطابقاً / Parameter count must match
    if (paramTypes_.size() != otherFunc->paramTypes_.size()) {
        return false;
    }
    
    // مقارنة كل معامل / Compare each parameter
    for (size_t i = 0; i < paramTypes_.size(); ++i) {
        if (!paramTypes_[i]->equals(otherFunc->paramTypes_[i].get())) {
            return false;
        }
    }
    
    // مقارنة نوع الإرجاع / Compare return type
    return returnType_->equals(otherFunc->returnType_.get()) &&
           isVariadic_ == otherFunc->isVariadic_;
}

// ============================================================================
// ClassType Implementation / تنفيذ ClassType
// ============================================================================

/**
 * الحصول على نوع عضو / Get member type
 * 
 * @param name اسم العضو / Member name
 * @return نوع العضو أو nullptr / Member type or nullptr
 */
std::shared_ptr<Type> ClassType::getMemberType(const std::string& name) const {
    // البحث في الأعضاء المحلية / Search in local members
    auto it = members_.find(name);
    if (it != members_.end()) {
        return it->second;
    }
    
    // البحث في الصنف الأساسي / Search in base class
    if (baseClass_) {
        return baseClass_->getMemberType(name);
    }
    
    return nullptr;
}

/**
 * الحصول على نوع طريقة / Get method type
 * 
 * @param name اسم الطريقة / Method name
 * @return نوع الطريقة أو nullptr / Method type or nullptr
 */
std::shared_ptr<FunctionType> ClassType::getMethodType(const std::string& name) const {
    // البحث في الطرق المحلية / Search in local methods
    auto it = methods_.find(name);
    if (it != methods_.end()) {
        return it->second;
    }
    
    // البحث في الصنف الأساسي / Search in base class
    if (baseClass_) {
        return baseClass_->getMethodType(name);
    }
    
    return nullptr;
}

/**
 * هل يرث من صنف معين / Inherits from class
 * 
 * @param other الصنف للتحقق منه / Class to check
 * @return true إذا كان يرث منه / true if inherits from
 */
bool ClassType::inheritsFrom(const ClassType* other) const {
    // التحقق من null / Check for null
    if (!other) return false;
    
    // نفس الصنف / Same class
    if (className_ == other->className_) {
        return true;
    }
    
    // التحقق من الصنف الأساسي / Check base class
    if (baseClass_) {
        return baseClass_->inheritsFrom(other);
    }
    
    return false;
}

/**
 * تحويل نوع الصنف لنص / Convert class type to string
 * 
 * @return تمثيل نصي / String representation
 */
std::string ClassType::toString() const {
    return className_;
}

/**
 * مقارنة نوعي صنف / Compare class types
 * 
 * @param other النوع الآخر / Other type
 * @return true إذا كانا متطابقين / true if equal
 */
bool ClassType::equals(const Type* other) const {
    // التحقق من أنه نوع صنف / Check it's a class type
    if (!other || other->getKind() != Kind::CLASS) {
        return false;
    }
    
    // المقارنة بالاسم / Compare by name
    const ClassType* otherClass = dynamic_cast<const ClassType*>(other);
    if (!otherClass) {
        return false;
    }
    
    return className_ == otherClass->className_;
}

// ============================================================================
// GenericType Implementation / تنفيذ GenericType
// ============================================================================

/**
 * تحويل النوع العام لنص / Convert generic type to string
 * 
 * @return تمثيل نصي / String representation
 */
std::string GenericType::toString() const {
    std::stringstream ss;
    ss << paramName_;
    
    // إضافة القيود / Add constraints
    if (!constraints_.empty()) {
        ss << " : ";
        for (size_t i = 0; i < constraints_.size(); ++i) {
            if (i > 0) ss << " + ";
            ss << constraints_[i]->toString();
        }
    }
    
    return ss.str();
}

/**
 * مقارنة نوعين عامين / Compare generic types
 * 
 * @param other النوع الآخر / Other type
 * @return true إذا كانا متطابقين / true if equal
 */
bool GenericType::equals(const Type* other) const {
    // التحقق من أنه نوع عام / Check it's a generic type
    if (!other || other->getKind() != Kind::GENERIC) {
        return false;
    }
    
    // المقارنة بالاسم / Compare by name
    const GenericType* otherGeneric = dynamic_cast<const GenericType*>(other);
    if (!otherGeneric) {
        return false;
    }
    
    return paramName_ == otherGeneric->paramName_;
}

// ============================================================================
// TypeFactory Implementation / تنفيذ TypeFactory
// ============================================================================

// الأنواع الأساسية الثابتة / Static primitive types
std::shared_ptr<Type> TypeFactory::intType_ = nullptr;
std::shared_ptr<Type> TypeFactory::floatType_ = nullptr;
std::shared_ptr<Type> TypeFactory::boolType_ = nullptr;
std::shared_ptr<Type> TypeFactory::stringType_ = nullptr;
std::shared_ptr<Type> TypeFactory::charType_ = nullptr;
std::shared_ptr<Type> TypeFactory::voidType_ = nullptr;
std::shared_ptr<Type> TypeFactory::unknownType_ = nullptr;
std::shared_ptr<Type> TypeFactory::errorType_ = nullptr;

/**
 * الحصول على نوع عدد صحيح / Get integer type
 */
std::shared_ptr<Type> TypeFactory::getIntType() {
    if (!intType_) {
        intType_ = std::make_shared<Type>(Type::Kind::INT, "عدد_صحيح");
    }
    return intType_;
}

/**
 * الحصول على نوع عدد عشري / Get float type
 */
std::shared_ptr<Type> TypeFactory::getFloatType() {
    if (!floatType_) {
        floatType_ = std::make_shared<Type>(Type::Kind::FLOAT, "عدد_عشري");
    }
    return floatType_;
}

/**
 * الحصول على نوع منطقي / Get boolean type
 */
std::shared_ptr<Type> TypeFactory::getBoolType() {
    if (!boolType_) {
        boolType_ = std::make_shared<Type>(Type::Kind::BOOL, "منطقي");
    }
    return boolType_;
}

/**
 * الحصول على نوع نص / Get string type
 */
std::shared_ptr<Type> TypeFactory::getStringType() {
    if (!stringType_) {
        stringType_ = std::make_shared<Type>(Type::Kind::STRING, "نص");
    }
    return stringType_;
}

/**
 * الحصول على نوع حرف / Get char type
 */
std::shared_ptr<Type> TypeFactory::getCharType() {
    if (!charType_) {
        charType_ = std::make_shared<Type>(Type::Kind::CHAR, "حرف");
    }
    return charType_;
}

/**
 * الحصول على نوع فارغ / Get void type
 */
std::shared_ptr<Type> TypeFactory::getVoidType() {
    if (!voidType_) {
        voidType_ = std::make_shared<Type>(Type::Kind::VOID, "فارغ");
    }
    return voidType_;
}

/**
 * الحصول على نوع مجهول / Get unknown type
 */
std::shared_ptr<Type> TypeFactory::getUnknownType() {
    if (!unknownType_) {
        unknownType_ = std::make_shared<Type>(Type::Kind::UNKNOWN, "مجهول");
    }
    return unknownType_;
}

/**
 * الحصول على نوع خطأ / Get error type
 */
std::shared_ptr<Type> TypeFactory::getErrorType() {
    if (!errorType_) {
        errorType_ = std::make_shared<Type>(Type::Kind::ERROR, "خطأ");
    }
    return errorType_;
}

/**
 * إنشاء نوع مصفوفة / Create array type
 */
std::shared_ptr<ArrayType> TypeFactory::createArrayType(
    std::shared_ptr<Type> elementType,
    int size
) {
    return std::make_shared<ArrayType>(elementType, size);
}

/**
 * إنشاء نوع قاموس / Create dict type
 */
std::shared_ptr<DictType> TypeFactory::createDictType(
    std::shared_ptr<Type> keyType,
    std::shared_ptr<Type> valueType
) {
    return std::make_shared<DictType>(keyType, valueType);
}

/**
 * إنشاء نوع دالة / Create function type
 */
std::shared_ptr<FunctionType> TypeFactory::createFunctionType(
    const std::vector<std::shared_ptr<Type>>& paramTypes,
    std::shared_ptr<Type> returnType,
    bool isVariadic
) {
    return std::make_shared<FunctionType>(paramTypes, returnType, isVariadic);
}

/**
 * إنشاء نوع صنف / Create class type
 */
std::shared_ptr<ClassType> TypeFactory::createClassType(
    const std::string& className
) {
    return std::make_shared<ClassType>(className);
}

/**
 * إنشاء نوع عام / Create generic type
 */
std::shared_ptr<GenericType> TypeFactory::createGenericType(
    const std::string& paramName
) {
    return std::make_shared<GenericType>(paramName);
}

} // namespace TypeChecker
} // namespace Sad
