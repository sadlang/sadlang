/*
 * ============================================================================
 * محول أنواع Sad/SIR إلى LLVM - ملف الرأس
 * Sad/SIR to LLVM Type Mapper - Header File
 * ============================================================================
 * 
 * هذا الملف يحتوي على محول متقدم لتحويل أنواع Sad و SIR إلى أنواع LLVM IR
 * This file contains an advanced mapper for converting Sad and SIR types
 * to LLVM IR types
 * 
 * الميزات / Features:
 * - تحويل جميع الأنواع الأساسية (عدد صحيح، عشري، منطقي، نص، فارغ)
 * - تحويل الأنواع المركبة (مصفوفات، قواميس، دوال، أصناف)
 * - تحويل أنواع الدوال (توقيعات كاملة)
 * - دعم أحجام مختلفة (i8, i16, i32, i64, f32, f64)
 * - دعم المؤشرات والمراجع
 * - تخزين مؤقت للأنواع المحولة (caching)
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2025
 * الإصدار (Version): 1.0.0
 * المرحلة (Phase): 1.1.2 - LLVM Code Generator Enhancement
 * ============================================================================
 */

#pragma once

#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include "../frontend/include/sir_types.h"
#include "parser/ast/expressions.h"
#include "parser/ast/statements.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Sad {
namespace LLVM {

// ============================================================================
// تعريف نوع Sad المبسط / Simplified Sad Type Definition
// ============================================================================

/**
 * فئة Type تمثل نوع البيانات في لغة Sad
 * Type class represents a data type in Sad language
 * 
 * ملاحظة: هذه نسخة مبسطة، النسخة الكاملة في typed_ast.h
 * Note: This is a simplified version, full version is in typed_ast.h
 */
class Type {
public:
    // منشئ افتراضي / Default constructor
    Type() = default;
    
    // مدمر افتراضي / Default destructor
    virtual ~Type() = default;
    
    // استعلامات النوع / Type queries
    virtual bool isVoid() const { return false; }         // نوع فارغ / Void type
    virtual bool isInteger() const { return false; }      // عدد صحيح / Integer
    virtual bool isFloat() const { return false; }        // عدد عشري / Float
    virtual bool isBoolean() const { return false; }      // منطقي / Boolean
    virtual bool isString() const { return false; }       // نص / String
    virtual bool isPointer() const { return false; }      // مؤشر / Pointer
    virtual bool isArray() const { return false; }        // مصفوفة / Array
    virtual bool isFunction() const { return false; }     // دالة / Function
    virtual bool isClass() const { return false; }        // صنف / Class
    
    // حجم البتات للأعداد الصحيحة / Bit width for integers
    virtual int getBitWidth() const { return 64; }        // افتراضي 64-bit / Default 64-bit
    
    // نوع float أم double / float or double type
    virtual bool isFloat32() const { return false; }      // float (32-bit)
    virtual bool isFloat64() const { return true; }       // double (64-bit) - default
    
    // للمؤشرات: نوع البيانات المشار إليه / For pointers: pointee type
    virtual std::shared_ptr<Type> getPointeeType() const { return nullptr; }
    
    // للمصفوفات: نوع العنصر والحجم / For arrays: element type and size
    virtual std::shared_ptr<Type> getElementType() const { return nullptr; }
    virtual size_t getArraySize() const { return 0; }
    
    // تحويل إلى نص / Convert to string
    virtual std::string toString() const { return "Type"; }
};

// ============================================================================
// فئة محول الأنواع / Type Mapper Class
// ============================================================================

/**
 * LLVMTypeMapper - محول أنواع Sad/SIR إلى LLVM
 * LLVMTypeMapper - Converts Sad/SIR types to LLVM types
 * 
 * المسؤوليات / Responsibilities:
 * - تحويل جميع أنواع Sad الأساسية والمركبة إلى LLVM
 * - تحويل أنواع SIR إلى LLVM
 * - إدارة ذاكرة تخزين مؤقت للأنواع المحولة
 * - توفير دوال مساعدة لأنواع شائعة الاستخدام
 * 
 * الاستخدام / Usage:
 * @code
 * llvm::LLVMContext context;
 * LLVMTypeMapper mapper(context);
 * 
 * // تحويل نوع Sad إلى LLVM / Convert Sad type to LLVM
 * auto sadType = std::make_shared<IntType>();
 * llvm::Type* llvmType = mapper.mapSadType(sadType);
 * 
 * // تحويل نوع SIR إلى LLVM / Convert SIR type to LLVM
 * llvm::Type* llvmType2 = mapper.mapSIRType(SIR::SIRType::I64);
 * @endcode
 */
class LLVMTypeMapper {
public:
    // ========================================================================
    // المنشئ والمدمر / Constructor & Destructor
    // ========================================================================
    
    /**
     * منشئ محول الأنواع
     * Type mapper constructor
     * 
     * @param context سياق LLVM المطلوب للإنشاء / LLVM context for creation
     */
    explicit LLVMTypeMapper(llvm::LLVMContext& context);
    
    /**
     * مدمر محول الأنواع
     * Type mapper destructor
     */
    ~LLVMTypeMapper() = default;
    
    // ========================================================================
    // تحويل أنواع Sad / Sad Type Conversion
    // ========================================================================
    
    /**
     * تحويل نوع Sad إلى نوع LLVM
     * Convert Sad type to LLVM type
     * 
     * @param sadType نوع Sad المطلوب تحويله / Sad type to convert
     * @return نوع LLVM المقابل / Corresponding LLVM type
     */
    llvm::Type* mapSadType(std::shared_ptr<Type> sadType);
    
    /**
     * تحويل نوع دالة Sad إلى نوع دالة LLVM
     * Convert Sad function type to LLVM function type
     * 
     * @param returnType نوع الرجوع / Return type
     * @param paramTypes أنواع المعاملات / Parameter types
     * @param isVarArg هل الدالة تقبل عدد متغير من المعاملات؟ / Is function variadic?
     * @return نوع دالة LLVM / LLVM function type
     */
    llvm::FunctionType* mapFunctionType(
        std::shared_ptr<Type> returnType,
        const std::vector<std::shared_ptr<Type>>& paramTypes,
        bool isVarArg = false
    );
    
    // ========================================================================
    // تحويل أنواع SIR / SIR Type Conversion
    // ========================================================================
    
    /**
     * تحويل نوع SIR إلى نوع LLVM
     * Convert SIR type to LLVM type
     * 
     * @param sirType نوع SIR المطلوب تحويله / SIR type to convert
     * @return نوع LLVM المقابل / Corresponding LLVM type
     */
    llvm::Type* mapSIRType(Compiler::SIR::SIRType sirType);
    
    /**
     * تحويل نوع SIR إلى نوع مؤشر LLVM
     * Convert SIR type to LLVM pointer type
     * 
     * @param sirType نوع SIR المطلوب تحويله إلى مؤشر / SIR type to convert to pointer
     * @return نوع مؤشر LLVM / LLVM pointer type
     */
    llvm::PointerType* mapSIRTypeToPointer(Compiler::SIR::SIRType sirType);
    
    // ========================================================================
    // أنواع أساسية / Primitive Types
    // ========================================================================
    
    /**
     * الحصول على نوع فارغ LLVM / Get LLVM void type
     */
    llvm::Type* getVoidType() const;
    
    /**
     * الحصول على نوع منطقي (i1) / Get boolean type (i1)
     */
    llvm::Type* getBoolType() const;
    
    /**
     * الحصول على نوع عدد صحيح 8-bit / Get 8-bit integer type
     */
    llvm::Type* getInt8Type() const;
    
    /**
     * الحصول على نوع عدد صحيح 16-bit / Get 16-bit integer type
     */
    llvm::Type* getInt16Type() const;
    
    /**
     * الحصول على نوع عدد صحيح 32-bit / Get 32-bit integer type
     */
    llvm::Type* getInt32Type() const;
    
    /**
     * الحصول على نوع عدد صحيح 64-bit / Get 64-bit integer type
     */
    llvm::Type* getInt64Type() const;
    
    /**
     * الحصول على نوع عدد عشري 32-bit (float) / Get 32-bit float type
     */
    llvm::Type* getFloatType() const;
    
    /**
     * الحصول على نوع عدد عشري 64-bit (double) / Get 64-bit double type
     */
    llvm::Type* getDoubleType() const;
    
    /**
     * الحصول على نوع مؤشر i8* (للنصوص) / Get i8* pointer type (for strings)
     */
    llvm::PointerType* getStringPtrType() const;
    
    /**
     * الحصول على نوع عدد صحيح بحجم محدد / Get integer type with specific bit width
     * 
     * @param bitWidth حجم البتات (8, 16, 32, 64) / Bit width (8, 16, 32, 64)
     * @return نوع عدد صحيح / Integer type
     */
    llvm::Type* getIntType(unsigned bitWidth) const;
    
    // ========================================================================
    // أنواع مركبة / Composite Types
    // ========================================================================
    
    /**
     * إنشاء نوع مصفوفة LLVM / Create LLVM array type
     * 
     * @param elementType نوع عناصر المصفوفة / Array element type
     * @param size حجم المصفوفة / Array size
     * @return نوع مصفوفة LLVM / LLVM array type
     */
    llvm::ArrayType* createArrayType(llvm::Type* elementType, size_t size);
    
    /**
     * إنشاء نوع بنية (struct) / Create struct type
     * 
     * @param name اسم البنية / Struct name
     * @param fieldTypes أنواع الحقول / Field types
     * @param isPacked هل البنية مُرصّة؟ / Is struct packed?
     * @return نوع بنية LLVM / LLVM struct type
     */
    llvm::StructType* createStructType(
        const std::string& name,
        const std::vector<llvm::Type*>& fieldTypes,
        bool isPacked = false
    );
    
    /**
     * إنشاء نوع مؤشر / Create pointer type
     * 
     * @param pointeeType نوع البيانات المشار إليه / Pointee type
     * @return نوع مؤشر LLVM / LLVM pointer type
     */
    llvm::PointerType* createPointerType(llvm::Type* pointeeType);
    
    /**
     * إنشاء نوع دالة / Create function type
     * 
     * @param returnType نوع الرجوع / Return type
     * @param paramTypes أنواع المعاملات / Parameter types
     * @param isVarArg هل تقبل عدد متغير من المعاملات؟ / Is variadic?
     * @return نوع دالة LLVM / LLVM function type
     */
    llvm::FunctionType* createFunctionType(
        llvm::Type* returnType,
        const std::vector<llvm::Type*>& paramTypes,
        bool isVarArg = false
    );
    
    // ========================================================================
    // إدارة التخزين المؤقت / Cache Management
    // ========================================================================
    
    /**
     * مسح التخزين المؤقت للأنواع / Clear type cache
     */
    void clearCache();
    
    /**
     * الحصول على عدد الأنواع المخزنة مؤقتاً / Get number of cached types
     * 
     * @return عدد الأنواع / Number of types
     */
    size_t getCacheSize() const;
    
private:
    // ========================================================================
    // البيانات الخاصة / Private Data
    // ========================================================================
    
    llvm::LLVMContext& context_;  ///< سياق LLVM / LLVM context
    
    /**
     * تخزين مؤقت للأنواع المحولة لتحسين الأداء
     * Type cache for performance improvement
     * 
     * المفتاح: اسم نوع Sad (مثل "Int64", "Float32", "String")
     * Key: Sad type name (e.g., "Int64", "Float32", "String")
     * 
     * القيمة: نوع LLVM المقابل
     * Value: Corresponding LLVM type
     */
    std::unordered_map<std::string, llvm::Type*> typeCache_;
    
    /**
     * تخزين مؤقت للبنى المُعرّفة
     * Cache for defined structs
     * 
     * المفتاح: اسم البنية
     * Key: Struct name
     * 
     * القيمة: نوع البنية LLVM
     * Value: LLVM struct type
     */
    std::unordered_map<std::string, llvm::StructType*> structCache_;
    
    // ========================================================================
    // دوال مساعدة خاصة / Private Helper Functions
    // ========================================================================
    
    /**
     * تحويل نوع مصفوفة Sad / Convert Sad array type
     */
    llvm::Type* mapArrayType(std::shared_ptr<Type> sadType);
    
    /**
     * تحويل نوع مؤشر Sad / Convert Sad pointer type
     */
    llvm::Type* mapPointerType(std::shared_ptr<Type> sadType);
    
    /**
     * تحويل نوع صنف Sad / Convert Sad class type
     */
    llvm::Type* mapClassType(std::shared_ptr<Type> sadType);
    
    /**
     * الحصول على نوع من التخزين المؤقت / Get type from cache
     * 
     * @param typeName اسم النوع / Type name
     * @return نوع LLVM أو nullptr / LLVM type or nullptr
     */
    llvm::Type* getCachedType(const std::string& typeName) const;
    
    /**
     * حفظ نوع في التخزين المؤقت / Save type to cache
     * 
     * @param typeName اسم النوع / Type name
     * @param type نوع LLVM المطلوب حفظه / LLVM type to save
     */
    void cacheType(const std::string& typeName, llvm::Type* type);
};

} // namespace LLVM
} // namespace Sad
