/*
 * ============================================================================
 * بناء التعابير المعقدة في LLVM - ملف الرأس
 * LLVM Complex Expression Builder - Header File
 * ============================================================================
 * 
 * هذا الملف يحتوي على بناء متقدم للتعابير المعقدة في LLVM IR
 * This file contains an advanced builder for complex expressions in LLVM IR
 * 
 * الميزات / Features:
 * - دعم المصفوفات والقواميس (arrays & dictionaries)
 * - دعم الأصناف والكائنات (classes & objects)
 * - دعم الدوال المتداخلة (closures)
 * - دعم Pattern Matching متقدم
 * - دعم List/Dict Comprehensions
 * - دعم Lambda expressions
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2025
 * الإصدار (Version): 1.0.0
 * المرحلة (Phase): 1.1.2 - LLVM Code Generator Enhancement
 * ============================================================================
 */

#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Constants.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace Sad {
namespace LLVM {

// Forward declarations / التصريحات المسبقة
class LLVMTypeMapper;
class LLVMArraySupport;
class LLVMDictSupport;
class LLVMClassSupport;
class LLVMClosureSupport;
class LLVMMemoryManager;

// ============================================================================
// فئة ExpressionBuilder - بناء التعابير المعقدة
// ============================================================================

/**
 * ExpressionBuilder - بناء التعابير المعقدة في LLVM
 * ExpressionBuilder - Building complex expressions in LLVM
 * 
 * المسؤوليات / Responsibilities:
 * - بناء تعابير المصفوفات (array expressions)
 * - بناء تعابير القواميس (dictionary expressions)
 * - بناء استدعاءات الدوال (function calls)
 * - بناء الوصول للأعضاء (member access)
 * - بناء Comprehensions
 * - بناء Lambda expressions
 */
class LLVMExpressionBuilder {
public:
    // ========================================================================
    // المنشئ / Constructor
    // ========================================================================
    
    /**
     * منشئ بناء التعابير
     * Expression builder constructor
     * 
     * @param context سياق LLVM / LLVM context
     * @param builder بناء IR / IR builder
     * @param typeMapper محول الأنواع / Type mapper
     * @param memoryManager مدير الذاكرة (اختياري) / Memory manager (optional)
     */
    LLVMExpressionBuilder(llvm::LLVMContext& context,
                          llvm::IRBuilder<>& builder,
                          LLVMTypeMapper* typeMapper,
                          LLVMMemoryManager* memoryManager = nullptr);
    
    /**
     * مدمر بناء التعابير
     * Expression builder destructor
     * Note: Defined in .cpp to support incomplete types in unique_ptr
     */
    ~LLVMExpressionBuilder();
    
    // ========================================================================
    // تعابير المصفوفات / Array Expressions
    // ========================================================================
    
    /**
     * بناء إنشاء مصفوفة
     * Build array creation
     * 
     * @param elementType نوع العناصر / Element type
     * @param elements العناصر / Elements
     * @return قيمة المصفوفة / Array value
     */
    llvm::Value* buildArrayCreate(llvm::Type* elementType,
                                   const std::vector<llvm::Value*>& elements);
    
    /**
     * بناء الوصول لعنصر مصفوفة
     * Build array element access
     * 
     * @param array المصفوفة / Array
     * @param index الفهرس / Index
     * @return قيمة العنصر / Element value
     */
    llvm::Value* buildArrayAccess(llvm::Value* array, llvm::Value* index);
    
    /**
     * بناء تعيين عنصر مصفوفة
     * Build array element assignment
     * 
     * @param array المصفوفة / Array
     * @param index الفهرس / Index
     * @param value القيمة الجديدة / New value
     */
    void buildArraySet(llvm::Value* array, llvm::Value* index, llvm::Value* value);
    
    /**
     * بناء طول المصفوفة
     * Build array length
     * 
     * @param array المصفوفة / Array
     * @return طول المصفوفة / Array length
     */
    llvm::Value* buildArrayLength(llvm::Value* array);
    
    // ========================================================================
    // تعابير القواميس / Dictionary Expressions
    // ========================================================================
    
    /**
     * بناء إنشاء قاموس
     * Build dictionary creation
     * 
     * @param keyType نوع المفاتيح / Key type
     * @param valueType نوع القيم / Value type
     * @return قيمة القاموس / Dictionary value
     */
    llvm::Value* buildDictCreate(llvm::Type* keyType, llvm::Type* valueType);
    
    /**
     * بناء الوصول لقيمة في قاموس
     * Build dictionary value access
     * 
     * @param dict القاموس / Dictionary
     * @param key المفتاح / Key
     * @return قيمة العنصر / Element value
     */
    llvm::Value* buildDictAccess(llvm::Value* dict, llvm::Value* key);
    
    /**
     * بناء تعيين قيمة في قاموس
     * Build dictionary value assignment
     * 
     * @param dict القاموس / Dictionary
     * @param key المفتاح / Key
     * @param value القيمة / Value
     */
    void buildDictSet(llvm::Value* dict, llvm::Value* key, llvm::Value* value);
    
    /**
     * بناء حجم القاموس
     * Build dictionary size
     * 
     * @param dict القاموس / Dictionary
     * @return حجم القاموس / Dictionary size
     */
    llvm::Value* buildDictSize(llvm::Value* dict);
    
    // ========================================================================
    // تعابير الأصناف / Class Expressions
    // ========================================================================
    
    /**
     * بناء إنشاء كائن
     * Build object creation
     * 
     * @param classType نوع الصنف / Class type
     * @param args معاملات المنشئ / Constructor arguments
     * @return كائن جديد / New object
     */
    llvm::Value* buildObjectCreate(llvm::Type* classType,
                                    const std::vector<llvm::Value*>& args);
    
    /**
     * بناء الوصول لحقل
     * Build field access
     * 
     * @param object الكائن / Object
     * @param fieldName اسم الحقل / Field name
     * @return قيمة الحقل / Field value
     */
    llvm::Value* buildFieldAccess(llvm::Value* object, const std::string& fieldName);
    
    /**
     * بناء تعيين حقل
     * Build field assignment
     * 
     * @param object الكائن / Object
     * @param fieldName اسم الحقل / Field name
     * @param value القيمة الجديدة / New value
     */
    void buildFieldSet(llvm::Value* object, const std::string& fieldName, llvm::Value* value);
    
    /**
     * بناء استدعاء دالة عضو
     * Build method call
     * 
     * @param object الكائن / Object
     * @param methodName اسم الدالة / Method name
     * @param args المعاملات / Arguments
     * @return قيمة الرجوع / Return value
     */
    llvm::Value* buildMethodCall(llvm::Value* object,
                                  const std::string& methodName,
                                  const std::vector<llvm::Value*>& args);
    
    // ========================================================================
    // تعابير الدوال / Function Expressions
    // ========================================================================
    
    /**
     * بناء استدعاء دالة
     * Build function call
     * 
     * @param function الدالة / Function
     * @param args المعاملات / Arguments
     * @return قيمة الرجوع / Return value
     */
    llvm::Value* buildFunctionCall(llvm::Function* function,
                                    const std::vector<llvm::Value*>& args);
    
    /**
     * بناء استدعاء دالة غير مباشر (مؤشر دالة)
     * Build indirect function call (function pointer)
     * 
     * @param functionPtr مؤشر الدالة / Function pointer
     * @param args المعاملات / Arguments
     * @return قيمة الرجوع / Return value
     */
    llvm::Value* buildIndirectCall(llvm::Value* functionPtr,
                                    const std::vector<llvm::Value*>& args);
    
    /**
     * بناء closure (دالة متداخلة)
     * Build closure (nested function)
     * 
     * @param function الدالة / Function
     * @param capturedVars المتغيرات الملتقطة / Captured variables
     * @return closure object
     */
    llvm::Value* buildClosure(llvm::Function* function,
                              const std::vector<llvm::Value*>& capturedVars);
    
    // ========================================================================
    // Comprehensions
    // ========================================================================
    
    /**
     * بناء list comprehension
     * Build list comprehension
     * 
     * @param elementExpr تعبير العنصر / Element expression
     * @param iterVar متغير التكرار / Iterator variable
     * @param iterableExpr التعبير القابل للتكرار / Iterable expression
     * @param condition الشرط (اختياري) / Condition (optional)
     * @return مصفوفة النتيجة / Result array
     */
    llvm::Value* buildListComprehension(llvm::Value* elementExpr,
                                        const std::string& iterVar,
                                        llvm::Value* iterableExpr,
                                        llvm::Value* condition = nullptr);
    
    /**
     * بناء dict comprehension
     * Build dict comprehension
     * 
     * @param keyExpr تعبير المفتاح / Key expression
     * @param valueExpr تعبير القيمة / Value expression
     * @param iterVar متغير التكرار / Iterator variable
     * @param iterableExpr التعبير القابل للتكرار / Iterable expression
     * @param condition الشرط (اختياري) / Condition (optional)
     * @return قاموس النتيجة / Result dictionary
     */
    llvm::Value* buildDictComprehension(llvm::Value* keyExpr,
                                        llvm::Value* valueExpr,
                                        const std::string& iterVar,
                                        llvm::Value* iterableExpr,
                                        llvm::Value* condition = nullptr);
    
    // ========================================================================
    // تعابير متقدمة / Advanced Expressions
    // ========================================================================
    
    /**
     * بناء ternary operator (شرط ? صحيح : خطأ)
     * Build ternary operator (condition ? true : false)
     * 
     * @param condition الشرط / Condition
     * @param trueValue القيمة إذا صحيح / True value
     * @param falseValue القيمة إذا خطأ / False value
     * @return القيمة المختارة / Selected value
     */
    llvm::Value* buildTernary(llvm::Value* condition,
                              llvm::Value* trueValue,
                              llvm::Value* falseValue);
    
    /**
     * بناء string interpolation
     * Build string interpolation
     * 
     * @param format صيغة النص / Format string
     * @param values القيم المُدرجة / Embedded values
     * @return نص محسوب / Computed string
     */
    llvm::Value* buildStringInterpolation(const std::string& format,
                                          const std::vector<llvm::Value*>& values);
    
    /**
     * بناء range expression (مدى)
     * Build range expression
     * 
     * @param start البداية / Start
     * @param end النهاية / End
     * @param step الخطوة (افتراضي 1) / Step (default 1)
     * @return مصفوفة المدى / Range array
     */
    llvm::Value* buildRange(llvm::Value* start, llvm::Value* end, llvm::Value* step = nullptr);
    
private:
    // ========================================================================
    // البيانات الخاصة / Private Data
    // ========================================================================
    llvm::IRBuilder<>& builder_;        ///< بناء IR / IR builder
    LLVMTypeMapper& typeMapper_;        ///< محول الأنواع / Type mapper
    LLVMMemoryManager* memoryManager_;  ///< مدير الذاكرة والـ ARC / Memory & ARC manager
    
    std::unique_ptr<LLVMArraySupport> arraySupport_;      ///< دعم المصفوفات / Array support
    std::unique_ptr<LLVMDictSupport> dictSupport_;        ///< دعم القواميس / Dict support
    std::unique_ptr<LLVMClassSupport> classSupport_;      ///< دعم الأصناف / Class support
    std::unique_ptr<LLVMClosureSupport> closureSupport_;  ///< دعم Closures / Closure support
    
    // Type tracking for opaque pointers (LLVM 15+)
    // تتبع الأنواع للمؤشرات الشفافة (LLVM 15+)
    std::unordered_map<llvm::Value*, llvm::Type*> valuePointeeTypes_;
    
    // ========================================================================
    // دوال مساعدة / Helper Functions
    // ========================================================================
    
    /**
     * تسجيل نوع المؤشر (للدعم الأمثل مع Opaque Pointers)
     * Register pointer pointee type (for optimal Opaque Pointers support)
     * 
     * @param pointer المؤشر / Pointer value
     * @param pointeeType نوع العنصر المشار إليه / Pointee type
     */
    void registerPointeeType(llvm::Value* pointer, llvm::Type* pointeeType) {
        valuePointeeTypes_[pointer] = pointeeType;
    }
    
    /**
     * الحصول على نوع العنصر المشار إليه من المؤشر
     * Get pointee type from pointer
     * 
     * @param pointer المؤشر / Pointer value
     * @return نوع العنصر أو nullptr / Pointee type or nullptr
     */
    llvm::Type* getPointeeType(llvm::Value* pointer) {
        #if LLVM_VERSION_MAJOR >= 15
        // With opaque pointers, use tracked type
        auto it = valuePointeeTypes_.find(pointer);
        if (it != valuePointeeTypes_.end()) {
            return it->second;
        }
        return nullptr;
        #else
        // With typed pointers, extract from pointer type
        if (auto* ptrType = llvm::dyn_cast<llvm::PointerType>(pointer->getType())) {
            return ptrType->getPointerElementType();
        }
        return nullptr;
        #endif
    }
    
    /**
     * إنشاء runtime call لدالة مساعدة
     * Create runtime call to helper function
     * 
     * @param functionName اسم الدالة / Function name
     * @param args المعاملات / Arguments
     * @return قيمة الرجوع / Return value
     */
    llvm::Value* callRuntimeFunction(const std::string& functionName,
                                      const std::vector<llvm::Value*>& args);
    
    /**
     * التحقق من صحة الفهرس
     * Validate index
     * 
     * @param index الفهرس / Index
     * @param size الحجم / Size
     */
    void validateIndex(llvm::Value* index, llvm::Value* size);
};

} // namespace LLVM
} // namespace Sad
