/*
 * ============================================================================
 * محول الأنواع المركبة - ملف التنفيذ
 * Composite Type Mapper - Implementation File
 * ============================================================================
 * 
 * م-أ02: ربط الأنواع المركبة في TypeMapper
 * تنفيذ تحويل المصفوفات، الصفوف، التعدادات بقيم، ومؤشرات الدوال
 * 
 * المعادلات:
 * - (ع8, ع16, ع64) → {i8, i16, i64}  (صف)
 * - تعداد نتيجة {نجاح(ع32), فشل(ع8)} → {i8, [4 x i8]}  (تعداد بقيم)
 * - دالة(ع32) -> ع64 → ptr  (مؤشر دالة — opaque pointer في LLVM 15+)
 * - مصفوفة<ع32, 256> → [256 x i32]  (مصفوفة ثابتة الحجم)
 * 
 * المؤلف: فريق مترجم لغة ص
 * التاريخ: فبراير 2026
 * ============================================================================
 */

#include "llvm_type_mapper_composite.h"
#include <algorithm>
#include <numeric>

namespace Sad {
namespace LLVM {

// ============================================================================
// المنشئ
// ============================================================================

LLVMCompositeTypeMapper::LLVMCompositeTypeMapper(
    llvm::LLVMContext& context,
    LLVMTypeMapper& baseMapper)
    : context_(context)
    , baseMapper_(baseMapper)
{
    // لا حاجة لتهيئة إضافية — التخزين المؤقت يُملأ عند الطلب
}

// ============================================================================
// تحويل الصفوف (Tuples)
// ============================================================================

/**
 * تحويل نوع صف إلى LLVM StructType
 * 
 * الصف (ع8, ع16, ع64) يُحوّل إلى:
 *   %tuple.i8_i16_i64 = type { i8, i16, i64 }
 * 
 * الصفوف لا تُرصّ (not packed) لأنها تحتاج محاذاة صحيحة
 */
llvm::StructType* LLVMCompositeTypeMapper::mapTupleType(const TupleType& tupleType) {
    // بناء اسم فريد للصف
    std::string tupleName = tupleType.toString();
    
    // فحص التخزين المؤقت
    auto it = tupleCache_.find(tupleName);
    if (it != tupleCache_.end()) {
        return it->second;
    }
    
    // تحويل كل عنصر في الصف إلى نوع LLVM
    std::vector<llvm::Type*> elementTypes;
    elementTypes.reserve(tupleType.getElementCount());
    
    for (size_t i = 0; i < tupleType.getElementCount(); ++i) {
        auto elem = tupleType.getElementAt(i);
        if (elem) {
            elementTypes.push_back(baseMapper_.mapSadType(elem));
        } else {
            // نوع غير معروف — نستخدم i64 كاحتياطي
            elementTypes.push_back(baseMapper_.getInt64Type());
        }
    }
    
    // إنشاء StructType مسمّى للصف
    llvm::StructType* result = llvm::StructType::create(
        context_,
        elementTypes,
        "tuple." + tupleName,
        false  // غير مُرصّ — نحتاج المحاذاة الصحيحة
    );
    
    // تخزين في الذاكرة المؤقتة
    tupleCache_[tupleName] = result;
    return result;
}

// ============================================================================
// تحويل التعدادات بقيم (Tagged Unions)
// ============================================================================

/**
 * حساب حجم أكبر حالة في التعداد بقيم
 * 
 * مثال: تعداد نتيجة { نجاح(ع32), فشل(ع8) }
 *   نجاح: ع32 = 4 بايت
 *   فشل: ع8 = 1 بايت
 *   أكبر حمولة = 4 بايت
 */
size_t LLVMCompositeTypeMapper::calculateMaxPayloadSize(const TaggedUnionType& unionType) {
    size_t maxSize = 0;
    
    for (const auto& variant : unionType.getVariants()) {
        size_t variantSize = 0;
        for (const auto& field : variant.fields) {
            if (field) {
                llvm::Type* llvmType = baseMapper_.mapSadType(field);
                variantSize += getTypeSizeInBytes(llvmType);
            }
        }
        maxSize = std::max(maxSize, variantSize);
    }
    
    return maxSize;
}

/**
 * تحويل تعداد بقيم إلى LLVM StructType
 * 
 * التمثيل في LLVM:
 *   %tagged_union.نتيجة = type { i8, [maxPayload x i8] }
 * 
 * حيث:
 *   - الحقل الأول (i8): وسم (tag) يحدد الحالة النشطة
 *   - الحقل الثاني: مصفوفة بايتات بحجم أكبر حالة
 * 
 * لماذا i8 للوسم؟ لأن عدد الحالات لا يتجاوز 256 عادةً
 * إذا تجاوز 256 نستخدم i16
 */
llvm::StructType* LLVMCompositeTypeMapper::mapTaggedUnionType(const TaggedUnionType& unionType) {
    std::string unionName = unionType.toString();
    
    // فحص التخزين المؤقت
    auto it = unionCache_.find(unionName);
    if (it != unionCache_.end()) {
        return it->second;
    }
    
    // حساب حجم أكبر حمولة
    size_t maxPayload = calculateMaxPayloadSize(unionType);
    
    // تحديد نوع الوسم حسب عدد الحالات
    llvm::Type* tagType;
    if (unionType.getVariantCount() <= 256) {
        tagType = llvm::Type::getInt8Ty(context_);   // i8 يكفي لـ 256 حالة
    } else {
        tagType = llvm::Type::getInt16Ty(context_);   // i16 لأكثر من 256
    }
    
    // بناء حقول البنية: {tag, payload}
    std::vector<llvm::Type*> fields;
    fields.push_back(tagType);
    
    // إضافة حمولة فقط إذا كان هناك بيانات
    if (maxPayload > 0) {
        llvm::Type* payloadType = llvm::ArrayType::get(
            llvm::Type::getInt8Ty(context_),
            maxPayload
        );
        fields.push_back(payloadType);
    }
    
    // إنشاء البنية
    llvm::StructType* result = llvm::StructType::create(
        context_,
        fields,
        "tagged_union." + unionType.getName(),
        false  // غير مُرصّ
    );
    
    // تخزين مؤقت
    unionCache_[unionName] = result;
    return result;
}

// ============================================================================
// تحويل مؤشرات الدوال
// ============================================================================

/**
 * تحويل مؤشر دالة إلى نوع LLVM
 * 
 * في LLVM 15+ (مؤشرات شفافة — opaque pointers):
 *   دالة(ع32) -> ع64  →  ptr
 * 
 * نحتفظ بنوع الدالة الفعلي للتحقق من الأنواع في المراحل السابقة
 */
llvm::Type* LLVMCompositeTypeMapper::mapFunctionPointerType(const FunctionPointerType& fnPtrType) {
    // في LLVM 15+ نستخدم المؤشرات الشفافة
    // نوع الدالة الفعلي يُستخدم فقط عند الاستدعاء (call instruction)
    return llvm::PointerType::get(context_, 0);
}

// ============================================================================
// تحويل المصفوفات الثابتة الحجم
// ============================================================================

/**
 * تحويل مصفوفة ثابتة الحجم
 * 
 * مصفوفة<ع32, 256> → [256 x i32]
 */
llvm::ArrayType* LLVMCompositeTypeMapper::mapFixedArrayType(
    std::shared_ptr<Type> elementType, size_t size) {
    
    llvm::Type* llvmElemType = baseMapper_.mapSadType(elementType);
    return llvm::ArrayType::get(llvmElemType, size);
}

// ============================================================================
// تحويل نوع مركب عام
// ============================================================================

/**
 * نقطة الدخول العامة — يُحدد نوع المركب تلقائياً
 * يُستدعى من mapSadType عند اكتشاف نوع مركب غير مدعوم في الأساس
 */
llvm::Type* LLVMCompositeTypeMapper::mapCompositeType(std::shared_ptr<Type> sadType) {
    if (!sadType) {
        return llvm::Type::getVoidTy(context_);
    }
    
    // محاولة التحويل كصف
    if (auto* tuple = dynamic_cast<TupleType*>(sadType.get())) {
        return mapTupleType(*tuple);
    }
    
    // محاولة التحويل كتعداد بقيم
    if (auto* taggedUnion = dynamic_cast<TaggedUnionType*>(sadType.get())) {
        return mapTaggedUnionType(*taggedUnion);
    }
    
    // محاولة التحويل كمؤشر دالة
    if (auto* fnPtr = dynamic_cast<FunctionPointerType*>(sadType.get())) {
        return mapFunctionPointerType(*fnPtr);
    }
    
    // مصفوفة — يُعالجها المحول الأساسي
    if (sadType->isArray()) {
        auto elemType = sadType->getElementType();
        size_t size = sadType->getArraySize();
        if (size > 0 && elemType) {
            return mapFixedArrayType(elemType, size);
        }
    }
    
    // نوع مركب غير معروف — نُرجع i64 كاحتياطي
    return baseMapper_.getInt64Type();
}

// ============================================================================
// أدوات مساعدة
// ============================================================================

/**
 * حساب حجم نوع LLVM بالبايتات
 * تقدير مبسّط — الحجم الدقيق يعتمد على DataLayout
 */
size_t LLVMCompositeTypeMapper::getTypeSizeInBytes(llvm::Type* type) const {
    if (!type) return 0;
    
    if (type->isIntegerTy()) {
        // حجم الأعداد الصحيحة = عدد البتات / 8
        return (type->getIntegerBitWidth() + 7) / 8;
    }
    if (type->isFloatTy()) return 4;
    if (type->isDoubleTy()) return 8;
    if (type->isPointerTy()) return 8;  // 64-bit مؤشرات
    if (type->isArrayTy()) {
        auto* arrType = llvm::cast<llvm::ArrayType>(type);
        return arrType->getNumElements() * getTypeSizeInBytes(arrType->getElementType());
    }
    if (type->isStructTy()) {
        auto* structType = llvm::cast<llvm::StructType>(type);
        size_t total = 0;
        for (unsigned i = 0; i < structType->getNumElements(); ++i) {
            total += getTypeSizeInBytes(structType->getElementType(i));
        }
        return total;
    }
    
    // نوع غير معروف — افتراضي 8 بايت
    return 8;
}

/**
 * مسح التخزين المؤقت
 */
void LLVMCompositeTypeMapper::clearCache() {
    tupleCache_.clear();
    unionCache_.clear();
}

} // namespace LLVM
} // namespace Sad
