/*
 * ============================================================================
 * دعم الأصناف في LLVM - ملف الرأس
 * Class Support in LLVM - Header File
 * ============================================================================
 * 
 * هذا الملف يوفر دعماً للبرمجة كائنية التوجه (OOP) في LLVM IR
 * This file provides Object-Oriented Programming (OOP) support in LLVM IR
 * 
 * الميزات / Features:
 * - تعريف الأصناف (class definitions)
 * - الوراثة (inheritance)
 * - الدوال الافتراضية (virtual functions / vtables)
 * - المُنشئات والمُدمرات (constructors & destructors)
 * - التغليف (encapsulation)
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2025
 * الإصدار (Version): 1.0.0
 * المرحلة (Phase): 1.1.2 - LLVM Code Generator Enhancement (Day 5)
 * ============================================================================
 */

#pragma once



#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace Sad {
namespace LLVM {

// Forward declarations / التصريحات المسبقة
class LLVMTypeMapper;

// ============================================================================
// هيكل معلومات الصنف / Class Information Structure
// ============================================================================

/**
 * معلومات الصنف في LLVM
 * Class information in LLVM
 */
struct ClassInfo {
    std::string name;                                    ///< اسم الصنف / Class name
    llvm::StructType* structType;                        ///< نوع الهيكل / Struct type
    llvm::StructType* vtableType;                        ///< نوع vtable / VTable type
    std::vector<std::string> fieldNames;                 ///< أسماء الحقول / Field names
    std::vector<llvm::Type*> fieldTypes;                 ///< أنواع الحقول / Field types
    std::unordered_map<std::string, unsigned> fieldIndices;  ///< فهارس الحقول / Field indices
    std::vector<std::string> methodNames;                ///< أسماء الدوال / Method names
    std::unordered_map<std::string, llvm::Function*> methods;  ///< الدوال / Methods
    ClassInfo* baseClass;                                ///< الصنف الأساسي / Base class
    bool hasVirtualMethods;                              ///< يحتوي دوال افتراضية؟ / Has virtual methods?
};

// ============================================================================
// فئة دعم الأصناف / Class Support Class
// ============================================================================

/**
 * LLVMClassSupport - دعم البرمجة كائنية التوجه في LLVM
 * LLVMClassSupport - OOP support in LLVM
 */
class LLVMClassSupport {
public:
    // ========================================================================
    // المنشئ / Constructor
    // ========================================================================
    
    /**
     * منشئ دعم الأصناف
     * Class support constructor
     */
    LLVMClassSupport(llvm::LLVMContext& context,
                     llvm::IRBuilder<>& builder,
                     LLVMTypeMapper* typeMapper);
    
    /**
     * مدمر دعم الأصناف
     * Class support destructor
     */
    ~LLVMClassSupport() = default;
    
    // ========================================================================
    // تعريف الأصناف / Class Definition
    // ========================================================================
    
    /**
     * تعريف صنف جديد
     * Define new class
     */
    ClassInfo* defineClass(const std::string& name,
                          const std::vector<std::string>& fieldNames,
                          const std::vector<llvm::Type*>& fieldTypes,
                          ClassInfo* baseClass = nullptr);
    
    /**
     * الحصول على معلومات صنف
     * Get class information
     */
    ClassInfo* getClassInfo(const std::string& name);
    
    /**
     * إضافة دالة للصنف
     * Add method to class
     */
    void addMethod(ClassInfo* classInfo,
                   const std::string& methodName,
                   llvm::Function* method,
                   bool isVirtual = false);
    
    // ========================================================================
    // إنشاء الكائنات / Object Creation
    // ========================================================================
    
    /**
     * إنشاء كائن جديد
     * Create new object
     */
    llvm::Value* createObject(ClassInfo* classInfo,
                              const std::vector<llvm::Value*>& constructorArgs = {});
    
    /**
     * استدعاء منشئ
     * Call constructor
     */
    void callConstructor(llvm::Value* object,
                        ClassInfo* classInfo,
                        const std::vector<llvm::Value*>& args);
    
    /**
     * استدعاء مدمر
     * Call destructor
     */
    void callDestructor(llvm::Value* object, ClassInfo* classInfo);
    
    // ========================================================================
    // الوصول للحقول / Field Access
    // ========================================================================
    
    /**
     * الوصول لحقل
     * Get field value
     */
    llvm::Value* getField(llvm::Value* object,
                         ClassInfo* classInfo,
                         const std::string& fieldName);
    
    /**
     * تعيين حقل
     * Set field value
     */
    void setField(llvm::Value* object,
                 ClassInfo* classInfo,
                 const std::string& fieldName,
                 llvm::Value* value);
    
    /**
     * الحصول على مؤشر حقل
     * Get field pointer
     */
    llvm::Value* getFieldPtr(llvm::Value* object,
                            ClassInfo* classInfo,
                            const std::string& fieldName);
    
    // ========================================================================
    // استدعاء الدوال / Method Calls
    // ========================================================================
    
    /**
     * استدعاء دالة مباشرة
     * Call method directly
     */
    llvm::Value* callMethod(llvm::Value* object,
                           ClassInfo* classInfo,
                           const std::string& methodName,
                           const std::vector<llvm::Value*>& args);
    
    /**
     * استدعاء دالة افتراضية (من خلال vtable)
     * Call virtual method (through vtable)
     */
    llvm::Value* callVirtualMethod(llvm::Value* object,
                                   ClassInfo* classInfo,
                                   const std::string& methodName,
                                   const std::vector<llvm::Value*>& args);
    
    // ========================================================================
    // الوراثة / Inheritance
    // ========================================================================
    
    /**
     * تحويل كائن لصنف أساسي (upcast)
     * Cast object to base class (upcast)
     */
    llvm::Value* upcast(llvm::Value* object,
                       ClassInfo* derivedClass,
                       ClassInfo* baseClass);
    
    /**
     * تحويل كائن لصنف مشتق (downcast)
     * Cast object to derived class (downcast)
     */
    llvm::Value* downcast(llvm::Value* object,
                         ClassInfo* baseClass,
                         ClassInfo* derivedClass);
    
    /**
     * التحقق من نوع الكائن
     * Check object type
     */
    llvm::Value* instanceof(llvm::Value* object, ClassInfo* classInfo);
    
private:
    // ========================================================================
    // البيانات الخاصة / Private Data
    // ========================================================================
    
    llvm::LLVMContext& context_;                              ///< سياق LLVM / LLVM context
    llvm::IRBuilder<>& builder_;                              ///< بناء IR / IR builder
    LLVMTypeMapper* typeMapper_;                              ///< محول الأنواع / Type mapper
    
    std::unordered_map<std::string, std::unique_ptr<ClassInfo>> classes_;  ///< الأصناف المُعرّفة / Defined classes
    
    // ========================================================================
    // دوال مساعدة / Helper Functions
    // ========================================================================
    
    /**
     * إنشاء vtable للصنف
     * Create vtable for class
     */
    llvm::GlobalVariable* createVTable(ClassInfo* classInfo);
    
    /**
     * الحصول على فهرس دالة في vtable
     * Get method index in vtable
     */
    unsigned getVTableIndex(ClassInfo* classInfo, const std::string& methodName);
    
    /**
     * حساب offset حقل مع الوراثة
     * Calculate field offset with inheritance
     */
    unsigned calculateFieldOffset(ClassInfo* classInfo, const std::string& fieldName);
};

// ============================================================================
// فئة دعم Closures / Closure Support Class
// ============================================================================

/**
 * LLVMClosureSupport - دعم الدوال المتداخلة والـ closures
 * LLVMClosureSupport - Support for nested functions and closures
 */
class LLVMClosureSupport {
public:
    // ========================================================================
    // المنشئ / Constructor
    // ========================================================================
    
    /**
     * منشئ دعم closures
     * Closure support constructor
     */
    LLVMClosureSupport(llvm::LLVMContext& context,
                       llvm::IRBuilder<>& builder,
                       LLVMTypeMapper* typeMapper);
    
    /**
     * مدمر دعم closures
     * Closure support destructor
     */
    ~LLVMClosureSupport() = default;
    
    // ========================================================================
    // إنشاء Closures / Closure Creation
    // ========================================================================
    
    /**
     * إنشاء closure جديد
     * Create new closure
     */
    llvm::Value* createClosure(llvm::Function* function,
                               const std::vector<llvm::Value*>& capturedVars);
    
    /**
     * استدعاء closure
     * Call closure
     */
    llvm::Value* callClosure(llvm::Value* closure,
                            const std::vector<llvm::Value*>& args);
    
    /**
     * الحصول على دالة من closure
     * Get function from closure
     */
    llvm::Function* getFunctionFromClosure(llvm::Value* closure);
    
    /**
     * الحصول على بيئة من closure
     * Get environment from closure
     */
    llvm::Value* getEnvironmentFromClosure(llvm::Value* closure);
    
    // ========================================================================
    // إدارة البيئة / Environment Management
    // ========================================================================
    
    /**
     * إنشاء بيئة للمتغيرات الملتقطة
     * Create environment for captured variables
     */
    llvm::Value* createEnvironment(const std::vector<llvm::Value*>& capturedVars);
    
    /**
     * الوصول لمتغير في البيئة
     * Access variable in environment
     */
    llvm::Value* getFromEnvironment(llvm::Value* environment, unsigned index);
    
    /**
     * تعيين متغير في البيئة
     * Set variable in environment
     */
    void setInEnvironment(llvm::Value* environment, unsigned index, llvm::Value* value);
    
private:
    // ========================================================================
    // البيانات الخاصة / Private Data
    // ========================================================================
    
    llvm::LLVMContext& context_;      ///< سياق LLVM / LLVM context
    llvm::IRBuilder<>& builder_;      ///< بناء IR / IR builder
    LLVMTypeMapper* typeMapper_;      ///< محول الأنواع / Type mapper
    
    llvm::StructType* closureType_;   ///< نوع closure / Closure type
    
    // ========================================================================
    // دوال مساعدة / Helper Functions
    // ========================================================================
    
    /**
     * الحصول أو إنشاء نوع closure
     * Get or create closure type
     */
    llvm::StructType* getOrCreateClosureType();
};

} // namespace LLVM
} // namespace Sad
