/*
 * ============================================================================
 * نظام الأنواع - Typed AST
 * Type System - Typed AST
 * ============================================================================
 * 
 * الوصف (Description):
 *   هذا الملف يحتوي على تعريفات Typed AST وهي طبقة فوق AST العادي
 *   تضيف معلومات الأنواع لكل عقدة. يستخدم في Type Checking و Code Generation.
 * 
 *   This file contains Typed AST definitions which is a layer on top of
 *   regular AST that adds type information to each node. Used in Type
 *   Checking and Code Generation.
 * 
 * المكونات الرئيسية (Main Components):
 *   - Type: نظام الأنواع / Type system
 *   - TypedASTNode: عقدة AST مع معلومات النوع / AST node with type info
 *   - TypedExpr: تعبير مُطبّع / Typed expression
 *   - TypedStmt: جملة مُطبّعة / Typed statement
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 31, 2025
 * الإصدار (Version): 1.0.0
 * ============================================================================
 */

#ifndef SAD_TYPED_AST_H
#define SAD_TYPED_AST_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "../../../../include/parser/ast/ast_node.h"
#include "../../../../include/data/types/data_types.h"

namespace Sad {
namespace TypeChecker {

// ============================================================================
// Forward Declarations / التصريحات المسبقة
// ============================================================================

class TypedASTNode;
class TypedExpr;
class TypedStmt;
class TypedDecl;
class TypeContext;

// ============================================================================
// Type System / نظام الأنواع
// ============================================================================

/**
 * نوع بيانات في نظام الأنواع
 * Data type in the type system
 * 
 * يمثل نوع بيانات محدد (int, string, class, etc.)
 * Represents a specific data type (int, string, class, etc.)
 */
class Type {
public:
    /**
     * تصنيف الأنواع / Type classification
     */
    enum class Kind {
        // الأنواع الأساسية / Primitive types
        INT,        // عدد صحيح / Integer
        FLOAT,      // عدد عشري / Floating point
        BOOL,       // منطقي / Boolean
        STRING,     // نص / String
        CHAR,       // حرف / Character
        VOID,       // فارغ / Void
        
        // الأنواع المركبة / Compound types
        ARRAY,      // مصفوفة / Array
        DICT,       // قاموس / Dictionary
        TUPLE,      // صف / Tuple
        
        // الأنواع المخصصة / Custom types
        CLASS,      // صنف / Class
        INTERFACE,  // واجهة / Interface
        ENUM,       // تعداد / Enumeration
        
        // أنواع الدوال / Function types
        FUNCTION,   // دالة / Function
        METHOD,     // طريقة / Method
        LAMBDA,     // دالة لامدا / Lambda function
        
        // أنواع خاصة / Special types
        GENERIC,    // نوع عام / Generic type
        UNKNOWN,    // نوع مجهول / Unknown type
        ERROR       // نوع خطأ / Error type
    };

protected:
    Kind kind_;                     // نوع البيانات / Data type kind
    std::string name_;              // اسم النوع / Type name
    bool isNullable_;               // هل يمكن أن يكون null / Can be null
    bool isConst_;                  // هل هو ثابت / Is constant
    
public:
    /**
     * المنشئ / Constructor
     * 
     * @param kind نوع البيانات / Data type kind
     * @param name اسم النوع / Type name
     */
    Type(Kind kind, const std::string& name = "")
        : kind_(kind)
        , name_(name)
        , isNullable_(false)
        , isConst_(false)
    {}
    
    // المدمر الافتراضي / Virtual destructor
    virtual ~Type() = default;
    
    // الحصول على نوع البيانات / Get type kind
    Kind getKind() const { return kind_; }
    
    // الحصول على اسم النوع / Get type name
    std::string getName() const { return name_; }
    
    // هل يمكن أن يكون null / Can be null
    bool isNullable() const { return isNullable_; }
    void setNullable(bool nullable) { isNullable_ = nullable; }
    
    // هل هو ثابت / Is constant
    bool isConst() const { return isConst_; }
    void setConst(bool isConst) { isConst_ = isConst; }
    
    // مقارنة الأنواع / Type comparison
    virtual bool equals(const Type* other) const;
    
    // هل يمكن تحويل هذا النوع إلى نوع آخر / Can convert to another type
    virtual bool canCastTo(const Type* target) const;
    
    // تحويل لنص / Convert to string
    virtual std::string toString() const;
    
    // هل هو نوع أساسي / Is primitive type
    bool isPrimitive() const;
    
    // هل هو نوع مركب / Is compound type
    bool isCompound() const;
    
    // هل هو نوع دالة / Is function type
    bool isFunction() const;
};

/**
 * نوع مصفوفة / Array type
 * 
 * مصفوفة من نوع معين
 * Array of specific element type
 */
class ArrayType : public Type {
private:
    std::shared_ptr<Type> elementType_; // نوع العناصر / Element type
    int size_;                          // الحجم (-1 للديناميكي) / Size (-1 for dynamic)
    
public:
    /**
     * المنشئ / Constructor
     * 
     * @param elementType نوع العناصر / Element type
     * @param size حجم المصفوفة (-1 للديناميكي) / Array size (-1 for dynamic)
     */
    ArrayType(std::shared_ptr<Type> elementType, int size = -1)
        : Type(Kind::ARRAY, "مصفوفة")
        , elementType_(elementType)
        , size_(size)
    {}
    
    // الحصول على نوع العناصر / Get element type
    std::shared_ptr<Type> getElementType() const { return elementType_; }
    
    // الحصول على الحجم / Get size
    int getSize() const { return size_; }
    
    // تحديد الحجم / Set size
    void setSize(int size) { size_ = size; }
    
    // تحويل لنص / Convert to string
    std::string toString() const override;
    
    // مقارنة / Comparison
    bool equals(const Type* other) const override;
};

/**
 * نوع قاموس / Dictionary type
 * 
 * قاموس بمفاتيح من نوع معين وقيم من نوع معين
 * Dictionary with specific key and value types
 */
class DictType : public Type {
private:
    std::shared_ptr<Type> keyType_;   // نوع المفاتيح / Key type
    std::shared_ptr<Type> valueType_; // نوع القيم / Value type
    
public:
    /**
     * المنشئ / Constructor
     * 
     * @param keyType نوع المفاتيح / Key type
     * @param valueType نوع القيم / Value type
     */
    DictType(std::shared_ptr<Type> keyType, std::shared_ptr<Type> valueType)
        : Type(Kind::DICT, "قاموس")
        , keyType_(keyType)
        , valueType_(valueType)
    {}
    
    // الحصول على نوع المفاتيح / Get key type
    std::shared_ptr<Type> getKeyType() const { return keyType_; }
    
    // الحصول على نوع القيم / Get value type
    std::shared_ptr<Type> getValueType() const { return valueType_; }
    
    // تحويل لنص / Convert to string
    std::string toString() const override;
    
    // مقارنة / Comparison
    bool equals(const Type* other) const override;
};

/**
 * نوع دالة / Function type
 * 
 * دالة بمعاملات من أنواع معينة ونوع إرجاع محدد
 * Function with specific parameter types and return type
 */
class FunctionType : public Type {
private:
    std::vector<std::shared_ptr<Type>> paramTypes_; // أنواع المعاملات / Parameter types
    std::shared_ptr<Type> returnType_;              // نوع الإرجاع / Return type
    bool isVariadic_;                               // هل تقبل معاملات متغيرة / Is variadic
    
public:
    /**
     * المنشئ / Constructor
     * 
     * @param paramTypes أنواع المعاملات / Parameter types
     * @param returnType نوع الإرجاع / Return type
     * @param isVariadic هل تقبل معاملات متغيرة / Is variadic
     */
    FunctionType(
        const std::vector<std::shared_ptr<Type>>& paramTypes,
        std::shared_ptr<Type> returnType,
        bool isVariadic = false
    )
        : Type(Kind::FUNCTION, "دالة")
        , paramTypes_(paramTypes)
        , returnType_(returnType)
        , isVariadic_(isVariadic)
    {}
    
    // الحصول على أنواع المعاملات / Get parameter types
    const std::vector<std::shared_ptr<Type>>& getParamTypes() const { 
        return paramTypes_; 
    }
    
    // الحصول على نوع الإرجاع / Get return type
    std::shared_ptr<Type> getReturnType() const { return returnType_; }
    
    // هل تقبل معاملات متغيرة / Is variadic
    bool isVariadic() const { return isVariadic_; }
    
    // تحويل لنص / Convert to string
    std::string toString() const override;
    
    // مقارنة / Comparison
    bool equals(const Type* other) const override;
};

/**
 * نوع صنف / Class type
 * 
 * صنف محدد بأعضائه ودواله
 * Specific class with its members and methods
 */
class ClassType : public Type {
private:
    std::string className_;                                    // اسم الصنف / Class name
    std::unordered_map<std::string, std::shared_ptr<Type>> members_; // الأعضاء / Members
    std::unordered_map<std::string, std::shared_ptr<FunctionType>> methods_; // الطرق / Methods
    std::shared_ptr<ClassType> baseClass_;                     // الصنف الأساسي / Base class
    
public:
    /**
     * المنشئ / Constructor
     * 
     * @param className اسم الصنف / Class name
     */
    explicit ClassType(const std::string& className)
        : Type(Kind::CLASS, className)
        , className_(className)
        , baseClass_(nullptr)
    {}
    
    // الحصول على اسم الصنف / Get class name
    std::string getClassName() const { return className_; }
    
    // إضافة عضو / Add member
    void addMember(const std::string& name, std::shared_ptr<Type> type) {
        members_[name] = type;
    }
    
    // الحصول على نوع عضو / Get member type
    std::shared_ptr<Type> getMemberType(const std::string& name) const;
    
    // إضافة طريقة / Add method
    void addMethod(const std::string& name, std::shared_ptr<FunctionType> type) {
        methods_[name] = type;
    }
    
    // الحصول على نوع طريقة / Get method type
    std::shared_ptr<FunctionType> getMethodType(const std::string& name) const;
    
    // تحديد الصنف الأساسي / Set base class
    void setBaseClass(std::shared_ptr<ClassType> baseClass) {
        baseClass_ = baseClass;
    }
    
    // الحصول على الصنف الأساسي / Get base class
    std::shared_ptr<ClassType> getBaseClass() const { return baseClass_; }
    
    // هل يرث من صنف معين / Inherits from class
    bool inheritsFrom(const ClassType* other) const;
    
    // تحويل لنص / Convert to string
    std::string toString() const override;
    
    // مقارنة / Comparison
    bool equals(const Type* other) const override;
};

/**
 * نوع عام (Generic) / Generic type
 * 
 * نوع متغير يُحدد لاحقاً
 * Variable type determined later
 */
class GenericType : public Type {
private:
    std::string paramName_;               // اسم المعامل العام / Generic parameter name
    std::vector<std::shared_ptr<Type>> constraints_; // القيود / Constraints
    
public:
    /**
     * المنشئ / Constructor
     * 
     * @param paramName اسم المعامل العام / Generic parameter name
     */
    explicit GenericType(const std::string& paramName)
        : Type(Kind::GENERIC, paramName)
        , paramName_(paramName)
    {}
    
    // الحصول على اسم المعامل / Get parameter name
    std::string getParamName() const { return paramName_; }
    
    // إضافة قيد / Add constraint
    void addConstraint(std::shared_ptr<Type> constraint) {
        constraints_.push_back(constraint);
    }
    
    // الحصول على القيود / Get constraints
    const std::vector<std::shared_ptr<Type>>& getConstraints() const {
        return constraints_;
    }
    
    // تحويل لنص / Convert to string
    std::string toString() const override;
    
    // مقارنة / Comparison
    bool equals(const Type* other) const override;
};

// ============================================================================
// Type Factory / مصنع الأنواع
// ============================================================================

/**
 * مصنع الأنواع الأساسية
 * Factory for primitive types
 * 
 * يوفر أنواع أساسية جاهزة (singleton)
 * Provides ready-made primitive types (singleton)
 */
class TypeFactory {
private:
    // الأنواع الأساسية الجاهزة / Ready primitive types
    static std::shared_ptr<Type> intType_;
    static std::shared_ptr<Type> floatType_;
    static std::shared_ptr<Type> boolType_;
    static std::shared_ptr<Type> stringType_;
    static std::shared_ptr<Type> charType_;
    static std::shared_ptr<Type> voidType_;
    static std::shared_ptr<Type> unknownType_;
    static std::shared_ptr<Type> errorType_;
    
public:
    // الحصول على الأنواع الأساسية / Get primitive types
    static std::shared_ptr<Type> getIntType();
    static std::shared_ptr<Type> getFloatType();
    static std::shared_ptr<Type> getBoolType();
    static std::shared_ptr<Type> getStringType();
    static std::shared_ptr<Type> getCharType();
    static std::shared_ptr<Type> getVoidType();
    static std::shared_ptr<Type> getUnknownType();
    static std::shared_ptr<Type> getErrorType();
    
    // إنشاء أنواع مركبة / Create compound types
    static std::shared_ptr<ArrayType> createArrayType(
        std::shared_ptr<Type> elementType,
        int size = -1
    );
    
    static std::shared_ptr<DictType> createDictType(
        std::shared_ptr<Type> keyType,
        std::shared_ptr<Type> valueType
    );
    
    static std::shared_ptr<FunctionType> createFunctionType(
        const std::vector<std::shared_ptr<Type>>& paramTypes,
        std::shared_ptr<Type> returnType,
        bool isVariadic = false
    );
    
    static std::shared_ptr<ClassType> createClassType(
        const std::string& className
    );
    
    static std::shared_ptr<GenericType> createGenericType(
        const std::string& paramName
    );
};

// ============================================================================
// Typed AST Nodes / عُقد AST المُطبّعة
// ============================================================================

/**
 * عقدة AST مُطبّعة
 * Typed AST node
 * 
 * كل عقدة تحمل معلومات عن نوعها
 * Each node carries information about its type
 */
class TypedASTNode {
protected:
    std::shared_ptr<AST::ASTNode> originalNode_; // العقدة الأصلية / Original node
    std::shared_ptr<Type> type_;                 // النوع المُستنتج / Inferred type
    
public:
    /**
     * المنشئ / Constructor
     * 
     * @param originalNode العقدة الأصلية / Original node
     * @param type النوع / Type
     */
    TypedASTNode(
        std::shared_ptr<AST::ASTNode> originalNode,
        std::shared_ptr<Type> type
    )
        : originalNode_(originalNode)
        , type_(type)
    {}
    
    // المدمر الافتراضي / Virtual destructor
    virtual ~TypedASTNode() = default;
    
    // الحصول على العقدة الأصلية / Get original node
    std::shared_ptr<AST::ASTNode> getOriginalNode() const {
        return originalNode_;
    }
    
    // الحصول على النوع / Get type
    std::shared_ptr<Type> getType() const { return type_; }
    
    // تحديد النوع / Set type
    void setType(std::shared_ptr<Type> type) { type_ = type; }
};

/**
 * تعبير مُطبّع / Typed expression
 */
class TypedExpr : public TypedASTNode {
public:
    using TypedASTNode::TypedASTNode;
    
    // هل التعبير ثابت في وقت الترجمة / Is compile-time constant
    virtual bool isConstant() const { return false; }
    
    // هل التعبير له آثار جانبية / Has side effects
    virtual bool hasSideEffects() const { return false; }
};

/**
 * جملة مُطبّعة / Typed statement
 */
class TypedStmt : public TypedASTNode {
public:
    using TypedASTNode::TypedASTNode;
    
    // هل الجملة تُرجع قيمة / Returns a value
    virtual bool returns() const { return false; }
};

/**
 * تصريح مُطبّع / Typed declaration
 */
class TypedDecl : public TypedASTNode {
public:
    using TypedASTNode::TypedASTNode;
};

} // namespace TypeChecker
} // namespace Sad

#endif // SAD_TYPED_AST_H
