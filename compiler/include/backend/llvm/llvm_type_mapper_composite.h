/*
 * ============================================================================
 * محول الأنواع المركبة - ملف الرأس
 * Composite Type Mapper - Header File
 * ============================================================================
 * 
 * م-أ02: ربط الأنواع المركبة في TypeMapper
 * تحويل المصفوفات، الصفوف، التعدادات بقيم (tagged unions)، ومؤشرات الدوال
 * إلى أنواع LLVM المقابلة
 * 
 * المؤلف: فريق مترجم لغة ص
 * التاريخ: فبراير 2026
 * الإصدار: 1.0.0
 * ============================================================================
 */

#pragma once

#include "llvm_type_mapper.h"
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace Sad {
namespace LLVM {

// ============================================================================
// أنواع مركبة مشتقة من Type الأساسي
// Composite types derived from base Type
// ============================================================================

/**
 * نوع الصف (Tuple) — مجموعة مرتبة من الأنواع المختلفة
 * Tuple type — ordered collection of different types
 * مثال: (ع8, ع16, ع64) → {i8, i16, i64}
 */
class TupleType : public Type {
public:
    explicit TupleType(std::vector<std::shared_ptr<Type>> elements)
        : elements_(std::move(elements)) {}
    
    bool isTuple() const { return true; }
    
    const std::vector<std::shared_ptr<Type>>& getElements() const { return elements_; }
    size_t getElementCount() const { return elements_.size(); }
    std::shared_ptr<Type> getElementAt(size_t index) const {
        return (index < elements_.size()) ? elements_[index] : nullptr;
    }
    
    std::string toString() const override {
        std::string result = "Tuple(";
        for (size_t i = 0; i < elements_.size(); ++i) {
            if (i > 0) result += ", ";
            result += elements_[i] ? elements_[i]->toString() : "?";
        }
        result += ")";
        return result;
    }

private:
    std::vector<std::shared_ptr<Type>> elements_;
};

/**
 * حالة تعداد — تمثل حالة واحدة في تعداد بقيم (tagged union)
 * Enum variant — represents a single case in a tagged union
 */
struct EnumVariant {
    std::string name;                          // اسم الحالة
    std::vector<std::shared_ptr<Type>> fields; // حقول القيمة المرتبطة (قد تكون فارغة)
};

/**
 * نوع التعداد بقيم (Tagged Union)
 * Tagged union type
 * مثال: تعداد نتيجة { نجاح(ع32), فشل(ع8) } → {i8, [8 x i8]}
 */
class TaggedUnionType : public Type {
public:
    TaggedUnionType(const std::string& name, std::vector<EnumVariant> variants)
        : name_(name), variants_(std::move(variants)) {}
    
    bool isTaggedUnion() const { return true; }
    
    const std::string& getName() const { return name_; }
    const std::vector<EnumVariant>& getVariants() const { return variants_; }
    size_t getVariantCount() const { return variants_.size(); }
    
    std::string toString() const override {
        return "TaggedUnion(" + name_ + ")";
    }

private:
    std::string name_;
    std::vector<EnumVariant> variants_;
};

/**
 * نوع مؤشر الدالة
 * Function pointer type
 * مثال: دالة(ع32) -> ع64 → i64 (i32)*
 */
class FunctionPointerType : public Type {
public:
    FunctionPointerType(std::shared_ptr<Type> returnType,
                       std::vector<std::shared_ptr<Type>> paramTypes,
                       bool isVarArg = false)
        : returnType_(std::move(returnType))
        , paramTypes_(std::move(paramTypes))
        , isVarArg_(isVarArg) {}
    
    bool isFunction() const override { return true; }
    bool isFunctionPointer() const { return true; }
    
    std::shared_ptr<Type> getReturnType() const { return returnType_; }
    const std::vector<std::shared_ptr<Type>>& getParamTypes() const { return paramTypes_; }
    bool isVarArg() const { return isVarArg_; }
    
    std::string toString() const override {
        std::string result = "FnPtr(";
        for (size_t i = 0; i < paramTypes_.size(); ++i) {
            if (i > 0) result += ", ";
            result += paramTypes_[i] ? paramTypes_[i]->toString() : "?";
        }
        result += ") -> ";
        result += returnType_ ? returnType_->toString() : "void";
        return result;
    }

private:
    std::shared_ptr<Type> returnType_;
    std::vector<std::shared_ptr<Type>> paramTypes_;
    bool isVarArg_;
};


// ============================================================================
// محول الأنواع المركبة
// Composite Type Mapper
// ============================================================================

/**
 * LLVMCompositeTypeMapper — يُوسّع LLVMTypeMapper بدعم الأنواع المركبة
 * 
 * يتعامل مع:
 * - الصفوف (Tuples): (ع8, ع16) → {i8, i16}
 * - التعدادات بقيم (Tagged Unions): نتيجة{نجاح(ع32), فشل} → {i8, [max_payload x i8]}
 * - مؤشرات الدوال: دالة(ع32)->ع64 → ptr (opaque pointer)
 * - المصفوفات بحجم ثابت: مصفوفة<ع32, 256> → [256 x i32]
 */
class LLVMCompositeTypeMapper {
public:
    /**
     * المنشئ — يحتاج سياق LLVM ومحول الأنواع الأساسي
     * @param context سياق LLVM
     * @param baseMapper محول الأنواع الأساسي لتحويل الأنواع البدائية
     */
    explicit LLVMCompositeTypeMapper(llvm::LLVMContext& context, LLVMTypeMapper& baseMapper);
    
    ~LLVMCompositeTypeMapper() = default;
    
    // ========================================================================
    // تحويل الأنواع المركبة
    // ========================================================================
    
    /**
     * تحويل نوع صف (Tuple) → LLVM StructType
     * (ع8, ع16, ع64) → {i8, i16, i64}
     */
    llvm::StructType* mapTupleType(const TupleType& tupleType);
    
    /**
     * تحويل تعداد بقيم (Tagged Union) → LLVM StructType
     * {tag: i8, payload: [maxPayloadSize x i8]}
     */
    llvm::StructType* mapTaggedUnionType(const TaggedUnionType& unionType);
    
    /**
     * تحويل مؤشر دالة → LLVM PointerType (opaque في LLVM 15+)
     */
    llvm::Type* mapFunctionPointerType(const FunctionPointerType& fnPtrType);
    
    /**
     * تحويل مصفوفة بحجم ثابت
     * مصفوفة<ع32, 256> → [256 x i32]
     */
    llvm::ArrayType* mapFixedArrayType(std::shared_ptr<Type> elementType, size_t size);
    
    /**
     * حساب حجم أكبر حالة في تعداد بقيم (payload size)
     * يُستخدم لتحديد حجم البيانات في tagged union
     */
    size_t calculateMaxPayloadSize(const TaggedUnionType& unionType);
    
    /**
     * تحويل نوع مركب عام — يُحدد النوع تلقائياً
     * يُستدعى من LLVMTypeMapper::mapSadType كامتداد
     */
    llvm::Type* mapCompositeType(std::shared_ptr<Type> sadType);
    
    /**
     * مسح التخزين المؤقت
     */
    void clearCache();

private:
    llvm::LLVMContext& context_;          // سياق LLVM
    LLVMTypeMapper& baseMapper_;          // محول الأنواع الأساسي
    
    // تخزين مؤقت للصفوف
    std::unordered_map<std::string, llvm::StructType*> tupleCache_;
    
    // تخزين مؤقت للتعدادات
    std::unordered_map<std::string, llvm::StructType*> unionCache_;
    
    /**
     * حساب حجم نوع LLVM بالبايتات
     */
    size_t getTypeSizeInBytes(llvm::Type* type) const;
};

} // namespace LLVM
} // namespace Sad
