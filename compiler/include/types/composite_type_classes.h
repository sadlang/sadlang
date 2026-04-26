// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4819)
#endif

/**
 * @file composite_type_classes.h
 * @brief أنواع مركبة: مصفوفة، صف، دالة / Composite types: Array, Tuple, Function
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 * @phase م-و01-04: فحص الأنواع المركبة
 */

#ifndef SAD_COMPOSITE_TYPE_CLASSES_H
#define SAD_COMPOSITE_TYPE_CLASSES_H

#include "types/type.h"
#include <optional>

namespace Sad {
namespace TypeSystem {

// =============================================================================
//                    ArrayType — نوع المصفوفة
// =============================================================================

/**
 * @class ArrayType
 * @brief نوع المصفوفة مع نوع العنصر وحجم اختياري
 * 
 *     مصفوفة<ع32, 256>  ← نوع عنصر ع32، حجم 256
 *     مصفوفة<نص>        ← نوع عنصر نص، حجم ديناميكي
 *     شريحة<ع32>        ← شريحة (بدون حجم ثابت)
 */
class ArrayType : public Type {
public:
    ArrayType(TypePtr elementType, std::optional<size_t> fixedSize = std::nullopt)
        : Type(SadTypeKind::Array)
        , elementType_(std::move(elementType))
        , fixedSize_(fixedSize)
        , isSlice_(false) {}
    
    /// إنشاء نوع شريحة (بدون حجم ثابت)
    static std::shared_ptr<ArrayType> createSlice(TypePtr elementType) {
        auto t = std::make_shared<ArrayType>(std::move(elementType));
        t->isSlice_ = true;
        return t;
    }
    
    TypePtr getElementType() const { return elementType_; }
    std::optional<size_t> getFixedSize() const { return fixedSize_; }
    bool isSlice() const { return isSlice_; }
    bool isFixedSize() const { return fixedSize_.has_value(); }
    
    // Type interface
    std::string getArabicName() const override {
        std::string name = isSlice_ ? u8"شريحة" : u8"مصفوفة";
        name += "<" + (elementType_ ? elementType_->getArabicName() : "?");
        if (fixedSize_) name += ", " + std::to_string(*fixedSize_);
        name += ">";
        return name;
    }
    
    std::string getEnglishName() const override {
        std::string name = isSlice_ ? "Slice" : "Array";
        name += "<" + (elementType_ ? elementType_->getEnglishName() : "?");
        if (fixedSize_) name += ", " + std::to_string(*fixedSize_);
        name += ">";
        return name;
    }
    
    std::string toString() const override { return getEnglishName(); }
    
    bool equals(const Type* other) const override {
        if (!other || other->getKind() != SadTypeKind::Array) return false;
        auto* arr = static_cast<const ArrayType*>(other);
        if (isSlice_ != arr->isSlice_) return false;
        if (fixedSize_ != arr->fixedSize_) return false;
        if (!elementType_ || !arr->elementType_) return elementType_ == arr->elementType_;
        return elementType_->equals(arr->elementType_.get());
    }
    
    std::shared_ptr<Type> clone() const override {
        auto c = std::make_shared<ArrayType>(
            elementType_ ? elementType_->clone() : nullptr, fixedSize_);
        c->isSlice_ = isSlice_;
        return c;
    }
    
    size_t getSizeInBytes() const override {
        if (!elementType_ || !fixedSize_) return 0;
        return elementType_->getSizeInBytes() * (*fixedSize_);
    }

private:
    TypePtr elementType_;
    std::optional<size_t> fixedSize_;
    bool isSlice_;
};

// =============================================================================
//                    TupleType — نوع الصف
// =============================================================================

/**
 * @class TupleType
 * @brief نوع الصف (مجموعة مرتبة من أنواع مختلفة)
 * 
 *     (ع32, نص, منطق)  ← صف من ثلاثة عناصر
 *     (ع8, ع16) ≠ (ع16, ع8)  ← الترتيب مهم
 */
class TupleType : public Type {
public:
    explicit TupleType(TypeList elementTypes)
        : Type(SadTypeKind::Tuple)
        , elementTypes_(std::move(elementTypes)) {}
    
    const TypeList& getElementTypes() const { return elementTypes_; }
    size_t getArity() const { return elementTypes_.size(); }
    
    TypePtr getElementAt(size_t index) const {
        if (index < elementTypes_.size()) return elementTypes_[index];
        return nullptr;
    }
    
    std::string getArabicName() const override {
        std::string name = "(";
        for (size_t i = 0; i < elementTypes_.size(); ++i) {
            if (i > 0) name += ", ";
            name += elementTypes_[i] ? elementTypes_[i]->getArabicName() : "?";
        }
        name += ")";
        return name;
    }
    
    std::string getEnglishName() const override {
        std::string name = "(";
        for (size_t i = 0; i < elementTypes_.size(); ++i) {
            if (i > 0) name += ", ";
            name += elementTypes_[i] ? elementTypes_[i]->getEnglishName() : "?";
        }
        name += ")";
        return name;
    }
    
    std::string toString() const override { return getEnglishName(); }
    
    bool equals(const Type* other) const override {
        if (!other || other->getKind() != SadTypeKind::Tuple) return false;
        auto* tup = static_cast<const TupleType*>(other);
        if (elementTypes_.size() != tup->elementTypes_.size()) return false;
        for (size_t i = 0; i < elementTypes_.size(); ++i) {
            if (!elementTypes_[i] || !tup->elementTypes_[i]) {
                if (elementTypes_[i] != tup->elementTypes_[i]) return false;
                continue;
            }
            if (!elementTypes_[i]->equals(tup->elementTypes_[i].get())) return false;
        }
        return true;
    }
    
    std::shared_ptr<Type> clone() const override {
        TypeList cloned;
        for (auto& t : elementTypes_) {
            cloned.push_back(t ? t->clone() : nullptr);
        }
        return std::make_shared<TupleType>(std::move(cloned));
    }
    
    size_t getSizeInBytes() const override {
        size_t total = 0;
        for (auto& t : elementTypes_) {
            if (t) total += t->getSizeInBytes();
        }
        return total;
    }

private:
    TypeList elementTypes_;
};

// =============================================================================
//                    FunctionType — نوع الدالة
// =============================================================================

/**
 * @class FunctionType
 * @brief نوع مؤشر الدالة مع أنواع المعاملات ونوع الإرجاع
 * 
 *     دالة(ع32, ع32) -> ع32  ← تأخذ عددين وتُرجع عدد
 *     دالة() -> فراغ          ← بدون معاملات ولا إرجاع
 */
class FunctionType : public Type {
public:
    FunctionType(TypeList paramTypes, TypePtr returnType)
        : Type(SadTypeKind::Function)
        , paramTypes_(std::move(paramTypes))
        , returnType_(std::move(returnType)) {}
    
    const TypeList& getParamTypes() const { return paramTypes_; }
    TypePtr getReturnType() const { return returnType_; }
    size_t getArity() const { return paramTypes_.size(); }
    
    TypePtr getParamAt(size_t index) const {
        if (index < paramTypes_.size()) return paramTypes_[index];
        return nullptr;
    }
    
    std::string getArabicName() const override {
        std::string name = u8"دالة(";
        for (size_t i = 0; i < paramTypes_.size(); ++i) {
            if (i > 0) name += ", ";
            name += paramTypes_[i] ? paramTypes_[i]->getArabicName() : "?";
        }
        name += ") -> ";
        name += returnType_ ? returnType_->getArabicName() : u8"فراغ";
        return name;
    }
    
    std::string getEnglishName() const override {
        std::string name = "fn(";
        for (size_t i = 0; i < paramTypes_.size(); ++i) {
            if (i > 0) name += ", ";
            name += paramTypes_[i] ? paramTypes_[i]->getEnglishName() : "?";
        }
        name += ") -> ";
        name += returnType_ ? returnType_->getEnglishName() : "Void";
        return name;
    }
    
    std::string toString() const override { return getEnglishName(); }
    
    bool equals(const Type* other) const override {
        if (!other || other->getKind() != SadTypeKind::Function) return false;
        auto* fn = static_cast<const FunctionType*>(other);
        if (paramTypes_.size() != fn->paramTypes_.size()) return false;
        for (size_t i = 0; i < paramTypes_.size(); ++i) {
            if (!paramTypes_[i] || !fn->paramTypes_[i]) {
                if (paramTypes_[i] != fn->paramTypes_[i]) return false;
                continue;
            }
            if (!paramTypes_[i]->equals(fn->paramTypes_[i].get())) return false;
        }
        if (!returnType_ || !fn->returnType_) return returnType_ == fn->returnType_;
        return returnType_->equals(fn->returnType_.get());
    }
    
    std::shared_ptr<Type> clone() const override {
        TypeList clonedParams;
        for (auto& p : paramTypes_) {
            clonedParams.push_back(p ? p->clone() : nullptr);
        }
        return std::make_shared<FunctionType>(
            std::move(clonedParams),
            returnType_ ? returnType_->clone() : nullptr);
    }

private:
    TypeList paramTypes_;
    TypePtr returnType_;
};

// =============================================================================
//                    DictionaryType — نوع القاموس
// =============================================================================

/**
 * @class DictionaryType
 * @brief نوع القاموس (Map) مع نوع المفتاح والقيمة
 */
class DictionaryType : public Type {
public:
    DictionaryType(TypePtr keyType, TypePtr valueType)
        : Type(SadTypeKind::Map)
        , keyType_(std::move(keyType))
        , valueType_(std::move(valueType)) {}
    
    TypePtr getKeyType() const { return keyType_; }
    TypePtr getValueType() const { return valueType_; }
    
    std::string getArabicName() const override {
        return u8"قاموس<" + 
               (keyType_ ? keyType_->getArabicName() : "?") + ", " +
               (valueType_ ? valueType_->getArabicName() : "?") + ">";
    }
    
    std::string getEnglishName() const override {
        return "Map<" + 
               (keyType_ ? keyType_->getEnglishName() : "?") + ", " +
               (valueType_ ? valueType_->getEnglishName() : "?") + ">";
    }
    
    std::string toString() const override { return getEnglishName(); }
    
    bool equals(const Type* other) const override {
        if (!other || other->getKind() != SadTypeKind::Map) return false;
        auto* dict = static_cast<const DictionaryType*>(other);
        bool keysEq = (!keyType_ && !dict->keyType_) || 
                      (keyType_ && dict->keyType_ && keyType_->equals(dict->keyType_.get()));
        bool valsEq = (!valueType_ && !dict->valueType_) || 
                      (valueType_ && dict->valueType_ && valueType_->equals(dict->valueType_.get()));
        return keysEq && valsEq;
    }
    
    std::shared_ptr<Type> clone() const override {
        return std::make_shared<DictionaryType>(
            keyType_ ? keyType_->clone() : nullptr,
            valueType_ ? valueType_->clone() : nullptr);
    }

private:
    TypePtr keyType_;
    TypePtr valueType_;
};

} // namespace TypeSystem
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // SAD_COMPOSITE_TYPE_CLASSES_H
