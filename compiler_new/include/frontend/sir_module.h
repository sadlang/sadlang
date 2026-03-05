// ======================================================================
// sir_module.h - وحدة SIR / SIR Module
// ======================================================================
// الوصف بالعربية:
//   تمثيل وحدة SIR الكاملة (برنامج كامل)
//   تحتوي على دوال، متغيرات عامة، وأصناف
//
// English Description:
//   Representation of complete SIR module (whole program)
//   Contains functions, global variables, and classes
//
// الميزات / Features:
//   - إدارة الدوال / Function management
//   - المتغيرات العامة / Global variables
//   - الأصناف والبنى / Classes and structs
//   - التحقق من الصحة / Validation
//
// المؤلف / Author: Sad Compiler Team
// التاريخ / Date: December 2, 2025
// الإصدار / Version: 1.0
// ======================================================================

#pragma once

#include "sir_types.h"
#include "sir_instruction.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>

namespace Sad {
namespace Compiler {
namespace SIR {

// Forward declarations
class SIRFunction;
class SIRGlobalVariable;
class SIRClass;

// ======================================================================
// فئة الوحدة / Module Class
// ======================================================================

/**
 * @brief (AR) وحدة SIR - تمثل برنامج كامل
 * @brief (EN) SIR Module - represents a complete program
 * 
 * الوحدة هي أعلى مستوى في التمثيل الوسيط وتحتوي على:
 * Module is the top-level in IR and contains:
 * - قائمة الدوال / List of functions
 * - متغيرات عامة / Global variables
 * - تعريفات الأصناف / Class definitions
 * - ثوابت النصوص / String constants
 * 
 * @example
 * @code{.cpp}
 * SIRModule module("my_program");
 * 
 * // إضافة دالة / Add function
 * auto func = std::make_shared<SIRFunction>("main", makeVoidType());
 * module.addFunction(func);
 * 
 * // إضافة متغير عام / Add global variable
 * auto globalVar = std::make_shared<SIRGlobalVariable>("PI", makeF64Type(), 3.14159);
 * module.addGlobalVariable(globalVar);
 * @endcode
 */
class SIRModule {
public:
    std::string name;           ///< (AR) اسم الوحدة / (EN) Module name
    
    /**
     * @brief (AR) منشئ الوحدة
     * @brief (EN) Module constructor
     * 
     * @param moduleName اسم الوحدة / Module name
     */
    explicit SIRModule(const std::string& moduleName);
    
    /**
     * @brief (AR) إضافة دالة
     * @brief (EN) Add function
     * 
     * @param function الدالة المراد إضافتها / Function to add
     */
    void addFunction(std::shared_ptr<SIRFunction> function);
    
    /**
     * @brief (AR) الحصول على دالة بالاسم
     * @brief (EN) Get function by name
     * 
     * @param name اسم الدالة / Function name
     * @return مؤشر للدالة أو nullptr / Pointer to function or nullptr
     */
    std::shared_ptr<SIRFunction> getFunction(const std::string& name) const;
    
    /**
     * @brief (AR) الحصول على جميع الدوال
     * @brief (EN) Get all functions
     * 
     * @return قائمة الدوال / List of functions
     */
    const std::vector<std::shared_ptr<SIRFunction>>& getFunctions() const;
    
    /**
     * @brief (AR) إضافة متغير عام
     * @brief (EN) Add global variable
     * 
     * @param globalVar المتغير العام / Global variable
     */
    void addGlobalVariable(std::shared_ptr<SIRGlobalVariable> globalVar);
    
    /**
     * @brief (AR) الحصول على متغير عام بالاسم
     * @brief (EN) Get global variable by name
     * 
     * @param name اسم المتغير / Variable name
     * @return مؤشر للمتغير أو nullptr / Pointer to variable or nullptr
     */
    std::shared_ptr<SIRGlobalVariable> getGlobalVariable(const std::string& name) const;
    
    /**
     * @brief (AR) إضافة صنف
     * @brief (EN) Add class
     * 
     * @param sirClass الصنف / Class
     */
    void addClass(std::shared_ptr<SIRClass> sirClass);
    
    /**
     * @brief (AR) الحصول على صنف بالاسم
     * @brief (EN) Get class by name
     * 
     * @param name اسم الصنف / Class name
     * @return مؤشر للصنف أو nullptr / Pointer to class or nullptr
     */
    std::shared_ptr<SIRClass> getClass(const std::string& name) const;
    
    /**
     * @brief (AR) إضافة ثابت نصي
     * @brief (EN) Add string constant
     * 
     * @param str النص / String
     * @return معرف الثابت / Constant ID
     */
    std::string addStringConstant(const std::string& str);
    
    /**
     * @brief (AR) التحقق من صحة الوحدة
     * @brief (EN) Validate module
     * 
     * @return true إذا كانت صحيحة / if valid
     */
    bool validate() const;
    
    /**
     * @brief (AR) طباعة الوحدة بصيغة SIR
     * @brief (EN) Print module in SIR format
     * 
     * @return النص الممثل للوحدة / String representation
     */
    std::string toString() const;
    
    /**
     * @brief (AR) طباعة الوحدة للـ console
     * @brief (EN) Print module to console
     */
    void print() const;
    
    /**
     * @brief (AR) الحصول على عدد الدوال
     * @brief (EN) Get function count
     */
    size_t getFunctionCount() const { return functions_.size(); }
    
    /**
     * @brief (AR) الحصول على عدد المتغيرات العامة
     * @brief (EN) Get global variable count
     */
    size_t getGlobalCount() const { return globalVariables_.size(); }
    
    /**
     * @brief (AR) الحصول على إجمالي عدد التعليمات
     * @brief (EN) Get total instruction count
     */
    size_t getTotalInstructions() const;
    
    /**
     * @brief (AR) الحصول على قائمة المتغيرات العامة
     * @brief (EN) Get global variables list
     */
    const std::vector<std::shared_ptr<SIRGlobalVariable>>& getGlobalVariables() const {
        return globalVariables_;
    }
    
    /**
     * @brief (AR) الحصول على قائمة الثوابت
     * @brief (EN) Get constants list
     */
    const std::vector<std::string>& getConstants() const {
        return stringConstants_;
    }
    
    /**
     * @brief (AR) الحصول على قائمة الأصناف
     * @brief (EN) Get classes list
     */
    const std::vector<std::shared_ptr<SIRClass>>& getClasses() const {
        return classes_;
    }
    
private:
    std::vector<std::shared_ptr<SIRFunction>> functions_;
    std::unordered_map<std::string, std::shared_ptr<SIRFunction>> functionMap_;
    
    std::vector<std::shared_ptr<SIRGlobalVariable>> globalVariables_;
    std::unordered_map<std::string, std::shared_ptr<SIRGlobalVariable>> globalVarMap_;
    
    std::vector<std::shared_ptr<SIRClass>> classes_;
    std::unordered_map<std::string, std::shared_ptr<SIRClass>> classMap_;
    
    std::vector<std::string> stringConstants_;
    std::unordered_map<std::string, std::string> stringConstantMap_;
    int nextStringId_;
};

// ======================================================================
// فئة المعامل / Parameter Struct
// ======================================================================

/**
 * @brief (AR) معامل دالة
 * @brief (EN) Function parameter
 */
struct SIRParameter {
    std::string name;       ///< (AR) اسم المعامل / (EN) Parameter name
    SIRType type;           ///< (AR) نوع المعامل / (EN) Parameter type
    
    SIRParameter(const std::string& paramName, SIRType paramType)
        : name(paramName), type(paramType) {}
    
    std::string toString() const;
};

// ======================================================================
// فئة الدالة / Function Class
// ======================================================================

/**
 * @brief (AR) دالة SIR
 * @brief (EN) SIR Function
 * 
 * تمثل دالة كاملة مع معاملاتها وجسمها من التعليمات
 * Represents a complete function with parameters and instruction body
 */
class SIRFunction {
public:
    std::string name;                   ///< (AR) اسم الدالة / (EN) Function name
    std::string linkName;               ///< (AR) اسم الربط الخارجي (FFI) / (EN) FFI link name (empty = use function name)
    SIRType returnType;                 ///< (AR) نوع الإرجاع / (EN) Return type
    bool isCoroutine = false;           ///< (AR) دالة غير متزامنة (كوروتين) / (EN) Async function (coroutine)
    bool isGenerator = false;           ///< (AR) دالة مولّد / (EN) Generator function
    std::vector<SIRParameter> parameters;   ///< (AR) المعاملات / (EN) Parameters
    std::vector<std::shared_ptr<SIRBasicBlock>> basicBlocks; ///< (AR) الكتل الأساسية / (EN) Basic blocks
    
    /**
     * @brief (AR) منشئ الدالة
     * @brief (EN) Function constructor
     * 
     * @param funcName اسم الدالة / Function name
     * @param retType نوع الإرجاع / Return type
     */
    SIRFunction(const std::string& funcName, SIRType retType);
    
    /**
     * @brief (AR) إضافة معامل
     * @brief (EN) Add parameter
     * 
     * @param param المعامل / Parameter
     */
    void addParameter(const SIRParameter& param);
    
    /**
     * @brief (AR) إضافة كتلة أساسية
     * @brief (EN) Add basic block
     * 
     * @param block الكتلة / Block
     */
    void addBasicBlock(std::shared_ptr<SIRBasicBlock> block);
    
    /**
     * @brief (AR) الحصول على كتلة بالاسم
     * @brief (EN) Get block by name
     * 
     * @param label اسم الكتلة / Block label
     * @return مؤشر للكتلة أو nullptr / Pointer to block or nullptr
     */
    std::shared_ptr<SIRBasicBlock> getBasicBlock(const std::string& label);
    
    /**
     * @brief (AR) الحصول على المعاملات
     * @brief (EN) Get parameters
     */
    const std::vector<SIRParameter>& getParameters() const { return parameters; }
    
    /**
     * @brief (AR) الحصول على الكتل الأساسية
     * @brief (EN) Get basic blocks
     */
    const std::vector<std::shared_ptr<SIRBasicBlock>>& getBasicBlocks() const { return basicBlocks; }
    
    /**
     * @brief (AR) الحصول على اسم الدالة
     * @brief (EN) Get function name
     */
    const std::string& getName() const { return name; }
    
    /**
     * @brief (AR) الحصول على اسم الربط الخارجي (أو اسم الدالة إذا لم يُحدد)
     * @brief (EN) Get the link name (or function name if not specified)
     */
    const std::string& getLinkName() const { return linkName.empty() ? name : linkName; }
    
    /**
     * @brief (AR) الحصول على نوع الإرجاع
     * @brief (EN) Get return type
     */
    SIRType getReturnType() const { return returnType; }
    
    /**
     * @brief (AR) التحقق من صحة الدالة
     * @brief (EN) Validate function
     * 
     * @return true إذا كانت صحيحة / if valid
     */
    bool validate() const;
    
    /**
     * @brief (AR) طباعة الدالة بصيغة SIR
     * @brief (EN) Print function in SIR format
     * 
     * @return النص الممثل / String representation
     */
    std::string toString() const;
};

// ======================================================================
// فئة المتغير العام / Global Variable Class
// ======================================================================

/**
 * @brief (AR) متغير عام
 * @brief (EN) Global variable
 */
class SIRGlobalVariable {
public:
    std::string name;       ///< (AR) اسم المتغير / (EN) Variable name
    SIRType type;           ///< (AR) نوع المتغير / (EN) Variable type
    std::string initialValue;   ///< (AR) القيمة الأولية (اختياري) / (EN) Initial value (optional)
    bool isConstant;        ///< (AR) ثابت / (EN) Constant
    
    /**
     * @brief (AR) منشئ المتغير العام
     * @brief (EN) Global variable constructor
     * 
     * @param varName اسم المتغير / Variable name
     * @param varType نوع المتغير / Variable type
     * @param initValue القيمة الأولية / Initial value
     * @param constant ثابت / Constant
     */
    SIRGlobalVariable(
        const std::string& varName,
        SIRType varType,
        const std::string& initValue = "",
        bool constant = false
    );
    
    /**
     * @brief (AR) طباعة المتغير بصيغة SIR
     * @brief (EN) Print variable in SIR format
     * 
     * @return النص الممثل / String representation
     */
    std::string toString() const;
    
    /**
     * @brief (AR) الحصول على اسم المتغير
     * @brief (EN) Get variable name
     */
    const std::string& getName() const { return name; }
    
    /**
     * @brief (AR) الحصول على نوع المتغير
     * @brief (EN) Get variable type
     */
    SIRType getType() const { return type; }
    
    /**
     * @brief (AR) التحقق من كون المتغير ثابت
     * @brief (EN) Check if variable is constant
     */
    bool getIsConstant() const { return isConstant; }
};

// ======================================================================
// فئة الصنف / Class
// ======================================================================

/**
 * @brief (AR) صنف SIR
 * @brief (EN) SIR Class
 */
class SIRClass {
public:
    std::string name;           ///< (AR) اسم الصنف / (EN) Class name
    std::string parentClass;    ///< (AR) الصنف الأب (للوراثة) / (EN) Parent class (for inheritance)
    bool isAbstract = false;    ///< (AR) هل الصنف مجرد / (EN) Is class abstract
    std::unordered_map<std::string, SIRType> fields_;   ///< (AR) الحقول / (EN) Fields
    std::vector<std::string> fieldOrder_;                ///< (AR) ترتيب الحقول / (EN) Field insertion order
    std::unordered_map<std::string, std::shared_ptr<SIRFunction>> methods_;  ///< (AR) الدوال / (EN) Methods
    
    /// (AR) خريطة ربط معاملات الباني بالحقول (لاستنتاج الأنواع)
    /// (EN) Constructor param → field mapping (for type inference)
    /// e.g., if constructor has هذا.الاسم = اسم, then: "اسم" → "الاسم"
    std::unordered_map<std::string, std::string> paramToFieldMap_;
    
    /// (AR) ربط وسائط الأساس بمعاملات الباني (لتتبع أنواع حقول الأب)
    /// (EN) Super arg → child param mapping (for parent field type propagation)
    /// Maps parent ctor param index (0-based, excl self) → child ctor param name
    /// e.g., الأساس(ضلع, ضلع) → {0: "ضلع", 1: "ضلع"}
    std::map<int, std::string> superParamMapping_;
    
    /// (AR) ثوابت وسائط الأساس (لتحديث أنواع حقول الأب مباشرةً)
    /// (EN) Super arg constants (to update parent field types directly)
    /// Maps parent ctor param index (0-based, excl self) → {type, value}
    /// e.g., الأساس("مستطيل") → {0: {STRING, "مستطيل"}}
    std::map<int, std::pair<SIRType, std::string>> superConstantMapping_;
    
    /**
     * @brief (AR) منشئ الصنف
     * @brief (EN) Class constructor
     * 
     * @param className اسم الصنف / Class name
     * @param parent الصنف الأب (اختياري) / Parent class (optional)
     */
    SIRClass(const std::string& className, const std::string& parent = "");
    
    /**
     * @brief (AR) إضافة حقل
     * @brief (EN) Add field
     * 
     * @param fieldName اسم الحقل / Field name
     * @param fieldType نوع الحقل / Field type
     */
    void addField(const std::string& fieldName, SIRType fieldType);
    
    /**
     * @brief (AR) إضافة دالة
     * @brief (EN) Add method
     * 
     * @param method الدالة / Method
     */
    void addMethod(std::shared_ptr<SIRFunction> method);
    
    /**
     * @brief (AR) الحصول على حقل
     * @brief (EN) Get field
     * 
     * @param name اسم الحقل / Field name
     * @return نوع الحقل / Field type
     */
    const SIRType* getField(const std::string& name) const;
    
    /**
     * @brief (AR) الحصول على دالة
     * @brief (EN) Get method
     * 
     * @param name اسم الدالة / Method name
     * @return مؤشر للدالة / Pointer to method
     */
    std::shared_ptr<SIRFunction> getMethod(const std::string& name) const;
    
    /**
     * @brief (AR) الحصول على اسم الصنف
     * @brief (EN) Get class name
     */
    const std::string& getName() const { return name; }
    
    /**
     * @brief (AR) الحصول على الصنف الأب
     * @brief (EN) Get parent class
     */
    const std::string& getParentClass() const { return parentClass; }
    
    /**
     * @brief (AR) طباعة الصنف بصيغة SIR
     * @brief (EN) Print class in SIR format
     * 
     * @return النص الممثل / String representation
     */
    std::string toString() const;
};

} // namespace SIR
} // namespace Compiler
} // namespace Sad
