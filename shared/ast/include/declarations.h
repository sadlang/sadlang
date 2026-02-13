/**
 * @file declarations.h
 * @brief Declaration AST node definitions / تعريفات عُقد AST للتصريحات
 * @author Sad Language Team
 * @date 11 November 2025
 */

#ifndef SAD_AST_DECLARATIONS_H
#define SAD_AST_DECLARATIONS_H

#include "ast_node.h"
#include "ast_visitor.h"
#include "expressions.h"
#include "statements.h"
#include "module_nodes.h"
#include <vector>
#include <string>

namespace Sad {
namespace AST {

// =========================================================================
// Function Declaration / تصريح الدالة
// =========================================================================

/**
 * @brief Function declaration node / عقدة تصريح الدالة
 * 
 * Represents a function declaration with optional decorators.
 * يمثل تصريح دالة مع مُزخرِفات اختيارية.
 * 
 * @example Examples / أمثلة:
 * - func add(a: int, b: int) -> int { return a + b; }
 * - دالة جمع(أ: صحيح، ب: صحيح) -> صحيح { أرجع أ + ب؛ }
 * - @staticmethod\nfunction test() {}
 * - @cache(maxsize=100)\nfunction expensive() {}
 */
class FunctionDecl : public Statement {
public:
    std::string name;               ///< Function name / اسم الدالة
    std::vector<Parameter> parameters; ///< Parameters / المعاملات
    Data::DataType returnType;      ///< Return type / نوع الإرجاع
    StmtPtr body;                   ///< Function body / جسم الدالة
    bool isExported;                ///< Is exported? / مصدّر؟
    bool isMainFunction;            ///< Is main function? / هل هي الدالة الرئيسية؟
    bool is_async;                  ///< Is async function? / دالة غير متزامنة؟
    bool isGenerator;               ///< Is generator function? / دالة مولد؟ (Phase 7)
    ExprList decorators;            ///< Decorators (@decorator) / المُزخرِفات
    
    /**
     * @brief Constructor without decorators / البناء بدون مُزخرِفات
     * @param name Function name / اسم الدالة
     * @param params Parameter list / قائمة المعاملات
     * @param retType Return type / نوع الإرجاع
     * @param body Function body / جسم الدالة
     * @param exported Is exported / مصدّر
     * @param async_func Is async function / دالة async
     * @param generator Is generator function / دالة مولد
     * @param pos Source position / الموقع في الكود
     */
    FunctionDecl(const std::string& name, std::vector<Parameter> params,
                 Data::DataType retType, StmtPtr body, bool exported = false,
                 bool async_func = false, bool generator = false,
                 const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), name(name), parameters(std::move(params)),
          returnType(retType), body(std::move(body)), isExported(exported),
          isMainFunction(false), is_async(async_func), isGenerator(generator), decorators() {}
    
    /**
     * @brief Constructor with decorators / البناء مع مُزخرِفات
     * @param name Function name / اسم الدالة
     * @param params Parameter list / قائمة المعاملات
     * @param retType Return type / نوع الإرجاع
     * @param body Function body / جسم الدالة
     * @param decs Decorator list / قائمة المُزخرِفات
     * @param exported Is exported / مصدّر
     * @param async_func Is async function / دالة async
     * @param generator Is generator function / دالة مولد
     * @param pos Source position / الموقع في الكود
     */
    FunctionDecl(const std::string& name, std::vector<Parameter> params,
                 Data::DataType retType, StmtPtr body, ExprList decs,
                 bool exported = false, bool async_func = false, bool generator = false,
                 const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), name(name), parameters(std::move(params)),
          returnType(retType), body(std::move(body)), isExported(exported),
          isMainFunction(false), is_async(async_func), isGenerator(generator), decorators(std::move(decs)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitFunctionDecl(*this);
    }
    
    std::string toString() const override;
};

// =========================================================================
// Class Declaration / تصريح الصنف
// =========================================================================

/**
 * @brief Access modifier / معدّل الوصول
 */
enum class AccessModifier {
    PUBLIC,     ///< Public / عام
    PRIVATE,    ///< Private / خاص
    PROTECTED   ///< Protected / محمي
};

/**
 * @brief Class declaration node / عقدة تصريح الصنف
 * 
 * Represents a class declaration with optional multiple inheritance.
 * يمثل تصريح صنف مع إمكانية الوراثة المتعددة.
 * 
 * @example Examples / أمثلة:
 * - class Person { ... }
 * - صنف شخص { ... }
 * - class Student extends Person { ... }
 * - class Assistant extends Teacher, Employee { ... }
 * - صنف مساعد يرث معلم، موظف { ... }
 * 
 * @note (AR) دعم الوراثة المتعددة: يمكن للصنف أن يرث من أكثر من صنف أب
 *       (EN) Multiple inheritance support: A class can inherit from multiple base classes
 */
class ClassDecl : public Statement {
public:
    std::string name;                        ///< Class name / اسم الصنف
    std::vector<std::string> superclasses;   ///< Base class names (multiple inheritance) / أسماء الأصناف الأساسية
    StmtList members;                        ///< Class members / أعضاء الصنف
    bool isExported;                         ///< Is exported? / مصدّر؟
    
    /**
     * @brief Constructor with multiple base classes / البناء مع أصناف أساسية متعددة
     * @param name Class name / اسم الصنف
     * @param bases Base class names / أسماء الأصناف الأساسية
     * @param members Member list / قائمة الأعضاء
     * @param exported Is exported / مصدّر
     * @param pos Source position / الموقع في الكود
     */
    ClassDecl(const std::string& name, const std::vector<std::string>& bases,
              StmtList members, bool exported = false,
              const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), name(name), superclasses(bases),
          members(std::move(members)), isExported(exported) {}
    
    /**
     * @brief Constructor with single base class (backward compatibility) / البناء مع صنف أساسي واحد
     * @param name Class name / اسم الصنف
     * @param superclass Single base class name / اسم الصنف الأساسي الواحد
     * @param members Member list / قائمة الأعضاء
     * @param exported Is exported / مصدّر
     * @param pos Source position / الموقع في الكود
     */
    ClassDecl(const std::string& name, const std::string& superclass,
              StmtList members, bool exported = false,
              const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), name(name), 
          superclasses(superclass.empty() ? std::vector<std::string>() : std::vector<std::string>{superclass}),
          members(std::move(members)), isExported(exported) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitClassDecl(*this);
    }
    
    std::string toString() const override;
};

// =========================================================================
// Field Declaration / تصريح الحقل
// =========================================================================

/**
 * @brief Field declaration node / عقدة تصريح الحقل
 * 
 * Represents a class field declaration.
 * يمثل تصريح حقل في صنف.
 * 
 * @example Examples / أمثلة:
 * - var name: string
 * - private var age: int = 0
 * - خاص متغير العمر: صحيح = 0
 */
class FieldDecl : public Statement {
public:
    std::string name;               ///< Field name / اسم الحقل
    Data::DataType type;            ///< Field type / نوع الحقل
    ExprPtr initializer;            ///< Initial value (optional) / القيمة الأولية
    AccessModifier access;          ///< Access modifier / معدّل الوصول
    bool isStatic;                  ///< Is static? / ثابت؟
    
    /**
     * @brief Constructor / البناء
     */
    FieldDecl(const std::string& name, Data::DataType type, ExprPtr init,
              AccessModifier access = AccessModifier::PUBLIC,
              bool isStatic = false,
              const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), name(name), type(type), initializer(std::move(init)),
          access(access), isStatic(isStatic) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitFieldDecl(*this);
    }
    
    std::string toString() const override;
};

// =========================================================================
// Method Declaration / تصريح الطريقة
// =========================================================================

/**
 * @brief Method declaration node / عقدة تصريح الطريقة
 * 
 * Represents a class method declaration.
 * يمثل تصريح طريقة في صنف.
 * 
 * @example Examples / أمثلة:
 * - func getName() -> string { ... }
 * - public func setAge(age: int) { ... }
 * - عام دالة اضبط_العمر(العمر: صحيح) { ... }
 */
class MethodDecl : public Statement {
public:
    std::string name;               ///< Method name / اسم الطريقة
    std::vector<Parameter> parameters; ///< Parameters / المعاملات
    Data::DataType returnType;      ///< Return type / نوع الإرجاع
    StmtPtr body;                   ///< Method body / جسم الطريقة
    AccessModifier access;          ///< Access modifier / معدّل الوصول
    bool isStatic;                  ///< Is static? / ثابت؟
    bool isVirtual;                 ///< Is virtual? / افتراضي؟
    bool isOverride;                ///< Is override? / تجاوز؟
    
    /**
     * @brief Constructor / البناء
     */
    MethodDecl(const std::string& name, std::vector<Parameter> params,
               Data::DataType retType, StmtPtr body,
               AccessModifier access = AccessModifier::PUBLIC,
               bool isStatic = false, bool isVirtual = false,
               bool isOverride = false,
               const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), name(name), parameters(std::move(params)),
          returnType(retType), body(std::move(body)), access(access),
          isStatic(isStatic), isVirtual(isVirtual), isOverride(isOverride) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitMethodDecl(*this);
    }
    
    std::string toString() const override;
};

// =========================================================================
// Constructor Declaration / تصريح الباني
// =========================================================================

/**
 * @brief Constructor declaration node / عقدة تصريح الباني
 * 
 * Represents a class constructor.
 * يمثل باني الصنف.
 * 
 * @example Examples / أمثلة:
 * - constructor(name: string, age: int) { ... }
 * - باني(الاسم: نص، العمر: صحيح) { ... }
 */
class ConstructorDecl : public Statement {
public:
    std::vector<Parameter> parameters; ///< Parameters / المعاملات
    StmtPtr body;                   ///< Constructor body / جسم الباني
    ExprList superArgs;             ///< Super constructor args / معاملات الباني الأب
    
    /**
     * @brief Constructor / البناء
     */
    ConstructorDecl(std::vector<Parameter> params, StmtPtr body,
                    ExprList superArgs = ExprList(),
                    const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), parameters(std::move(params)), body(std::move(body)),
          superArgs(std::move(superArgs)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitConstructorDecl(*this);
    }
    
    std::string toString() const override;
};

// =========================================================================
// Destructor Declaration / تصريح الهادم
// =========================================================================

/**
 * @brief Destructor declaration node / عقدة تصريح الهادم
 * 
 * Represents a class destructor (C++-style).
 * يمثل هادم الصنف (بأسلوب C++).
 * 
 * @example Examples / أمثلة:
 * - destructor { ... }
 * - ~ClassName() { ... }
 * - هادم { ... }
 */
class DestructorDecl : public Statement {
public:
    StmtPtr body;                   ///< Destructor body / جسم الهادم
    
    /**
     * @brief Constructor / البناء
     */
    DestructorDecl(StmtPtr body, const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), body(std::move(body)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitDestructorDecl(*this);
    }
    
    std::string toString() const override {
        return "destructor { ... }";
    }
};

// =========================================================================
// Enum Declaration / تصريح التعداد
// =========================================================================

/**
 * @brief Enum member / عضو التعداد
 */
struct EnumMember {
    std::string name;           ///< Member name / اسم العضو
    ExprPtr value;              ///< Member value (optional) / قيمة العضو
    
    EnumMember(const std::string& n, ExprPtr v = nullptr)
        : name(n), value(std::move(v)) {}
    
    // Copy constructor deleted (contains unique_ptr)
    EnumMember(const EnumMember&) = delete;
    EnumMember& operator=(const EnumMember&) = delete;
    
    // Move constructor and assignment
    EnumMember(EnumMember&&) = default;
    EnumMember& operator=(EnumMember&&) = default;
};

/**
 * @brief Enum declaration node / عقدة تصريح التعداد
 * 
 * Represents an enumeration declaration.
 * يمثل تصريح تعداد.
 * 
 * @example Examples / أمثلة:
 * - enum Color { RED, GREEN, BLUE }
 * - تعداد اللون { أحمر، أخضر، أزرق }
 * - enum Status { OK = 0, ERROR = 1 }
 */
class EnumDecl : public Statement {
public:
    std::string name;               ///< Enum name / اسم التعداد
    std::vector<EnumMember> members; ///< Enum members / أعضاء التعداد
    bool isExported;                ///< Is exported? / مصدّر؟
    
    /**
     * @brief Constructor / البناء
     */
    EnumDecl(const std::string& name, std::vector<EnumMember> members,
             bool exported = false,
             const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), name(name), members(std::move(members)),
          isExported(exported) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitEnumDecl(*this);
    }
    
    std::string toString() const override;
};

// =========================================================================
// (AR) نظام الوحدات - الاستيراد والتصدير / (EN) Module System
// =========================================================================

/**
 * @note (AR) تم نقل عُقد نظام الوحدات إلى ملف منفصل: module_nodes.h
 *       (EN) Module system nodes moved to separate file: module_nodes.h
 * 
 * @details
 * (AR) الأصناف الجديدة في module_nodes.h:
 *      - ImportStmt: استيراد كامل (استورد وحدة [كـ اسم])
 *      - FromImportStmt: استيراد انتقائي (من وحدة استورد رمز)
 *      - ExportDecl: تصدير (صدّر تصريح)
 *      - ImportItem: عنصر مستورد مع اسم مستعار
 * 
 * (EN) New classes in module_nodes.h:
 *      - ImportStmt: Full import
 *      - FromImportStmt: Selective import
 *      - ExportDecl: Export declaration
 *      - ImportItem: Imported item with alias
 * 
 * @see module_nodes.h
 */

// (AR) ExportStmt القديم محفوظ للتوافق / (EN) Legacy ExportStmt for compatibility
class ExportStmt : public Statement {
public:
    StmtPtr declaration;
    
    ExportStmt(StmtPtr decl, const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), declaration(std::move(decl)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitExportStmt(*this);
    }
    
    std::string toString() const override {
        return "export " + (declaration ? declaration->toString() : "");
    }
};

// =========================================================================
// Template Type Parameter / معامل نوع القالب
// =========================================================================

/**
 * @brief Template type parameter / معامل نوع القالب
 * 
 * Represents a type parameter in template declarations.
 * يمثل معامل نوع في تصريحات القوالب.
 * 
 * @example Examples / أمثلة:
 * - T (simple type parameter)
 * - T: Comparable (constrained type parameter)
 * - نوع ت (معامل نوع بسيط)
 * - ت: قابل_للمقارنة (معامل نوع مقيد)
 */
struct TypeParameter {
    std::string name;               ///< Parameter name / اسم المعامل (e.g., "T", "ت")
    std::string constraint;         ///< Type constraint (optional) / قيد النوع (اختياري)
    ExprPtr defaultType;            ///< Default type (optional) / النوع الافتراضي
    
    TypeParameter(const std::string& n, const std::string& c = "",
                  ExprPtr def = nullptr)
        : name(n), constraint(c), defaultType(std::move(def)) {}
    
    // Move operations
    TypeParameter(TypeParameter&&) = default;
    TypeParameter& operator=(TypeParameter&&) = default;
    
    // Copy constructor
    TypeParameter(const TypeParameter& other)
        : name(other.name), constraint(other.constraint), defaultType(nullptr) {}
};

// =========================================================================
// Template Function Declaration / تصريح دالة قالب
// =========================================================================

/**
 * @brief Template function declaration / تصريح دالة قالب
 * 
 * Represents a generic/template function declaration.
 * يمثل تصريح دالة عامة (قالب).
 * 
 * @example Examples / أمثلة:
 * - قالب<نوع ت> دالة أكبر(ت أ، ت ب) ت { ... }
 * - template<typename T> func max(a: T, b: T) -> T { ... }
 * - قالب<نوع ت، نوع م> دالة تبديل(إشارة ت أ، إشارة م ب) { ... }
 */
class TemplateFunctionDecl : public Statement {
public:
    std::vector<TypeParameter> typeParameters;  ///< Type parameters / معاملات الأنواع
    std::string name;                           ///< Function name / اسم الدالة
    std::vector<Parameter> parameters;          ///< Function parameters / معاملات الدالة
    Data::DataType returnType;                  ///< Return type / نوع الإرجاع
    StmtPtr body;                               ///< Function body / جسم الدالة
    bool isExported;                            ///< Is exported? / مصدّر؟
    
    /**
     * @brief Constructor / البناء
     */
    TemplateFunctionDecl(std::vector<TypeParameter> typeParams,
                         const std::string& name,
                         std::vector<Parameter> params,
                         Data::DataType retType,
                         StmtPtr body,
                         bool exported = false,
                         const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), typeParameters(std::move(typeParams)),
          name(name), parameters(std::move(params)),
          returnType(retType), body(std::move(body)), isExported(exported) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitTemplateFunctionDecl(*this);
    }
    
    std::string toString() const override;
};

// =========================================================================
// Template Class Declaration / تصريح صنف قالب
// =========================================================================

/**
 * @brief Template class declaration / تصريح صنف قالب
 * 
 * Represents a generic/template class declaration.
 * يمثل تصريح صنف عام (قالب).
 * 
 * @example Examples / أمثلة:
 * - قالب<نوع ت> صنف صندوق { ... نهاية }
 * - template<typename T> class Box { ... }
 * - قالب<نوع ك، نوع ق> صنف قاموس { ... نهاية }
 */
class TemplateClassDecl : public Statement {
public:
    std::vector<TypeParameter> typeParameters;  ///< Type parameters / معاملات الأنواع
    std::string name;                           ///< Class name / اسم الصنف
    std::vector<std::string> superclasses;      ///< Base class names / أسماء الأصناف الأساسية
    StmtList members;                           ///< Class members / أعضاء الصنف
    bool isExported;                            ///< Is exported? / مصدّر؟
    
    /**
     * @brief Constructor / البناء
     */
    TemplateClassDecl(std::vector<TypeParameter> typeParams,
                      const std::string& name,
                      const std::vector<std::string>& bases,
                      StmtList members,
                      bool exported = false,
                      const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), typeParameters(std::move(typeParams)),
          name(name), superclasses(bases),
          members(std::move(members)), isExported(exported) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitTemplateClassDecl(*this);
    }
    
    std::string toString() const override;
};

// =========================================================================
// Template Instantiation Expression / تعبير تنفيذ القالب
// =========================================================================

/**
 * @brief Template instantiation expression / تعبير تنفيذ القالب
 * 
 * Represents instantiation of a template with concrete types.
 * يمثل تنفيذ قالب بأنواع محددة.
 * 
 * @example Examples / أمثلة:
 * - صندوق<رقم> ص = جديد صندوق<رقم>(42)
 * - أكبر<نص>(أ، ب)
 * - Box<int> b = new Box<int>(42)
 * - max<string>(a, b)
 */
class TemplateInstantiation : public Expression {
public:
    std::string templateName;                   ///< Template name / اسم القالب
    std::vector<Data::DataType> typeArguments;  ///< Type arguments / وسائط الأنواع
    
    /**
     * @brief Constructor / البناء
     */
    TemplateInstantiation(const std::string& name,
                          std::vector<Data::DataType> typeArgs,
                          const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), templateName(name), typeArguments(std::move(typeArgs)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitTemplateInstantiation(*this);
    }
    
    Data::DataType getType() const override {
        // (AR) القالب يعيد نوع غير محدد حتى يتم instantiate
        // (EN) Template returns unknown type until instantiated
        return Data::DataType::UNKNOWN;
    }
    
    std::string toString() const override;
};

// =========================================================================
// Namespace Declaration / تصريح فضاء الأسماء
// =========================================================================

/**
 * @brief Namespace declaration / تصريح فضاء الأسماء
 * 
 * Represents a namespace for organizing code.
 * يمثل فضاء أسماء لتنظيم الكود.
 * 
 * @example Examples / أمثلة:
 * - فضاء رياضيات ... نهاية_فضاء
 * - namespace math { ... }
 */
class NamespaceDecl : public Statement {
public:
    std::string name;           ///< Namespace name / اسم الفضاء
    StmtList members;           ///< Namespace members / أعضاء الفضاء
    
    /**
     * @brief Constructor / البناء
     */
    NamespaceDecl(const std::string& name, StmtList members,
                  const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), name(name), members(std::move(members)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitNamespaceDecl(*this);
    }
    
    std::string toString() const override;
};

// =========================================================================
// Operator Overload Declaration / تصريح تحميل العامل
// =========================================================================

/**
 * @brief Operator overload declaration / تصريح تحميل العامل
 * 
 * Represents an operator overload in a class.
 * يمثل تحميل عامل في صنف.
 * 
 * @example Examples / أمثلة:
 * - عامل +(كسر آخر) كسر { ... }
 * - operator +(other: Fraction) -> Fraction { ... }
 */
class OperatorDecl : public Statement {
public:
    std::string operatorSymbol;     ///< Operator symbol / رمز العامل (+, -, *, etc.)
    std::vector<Parameter> parameters; ///< Parameters / المعاملات
    Data::DataType returnType;      ///< Return type / نوع الإرجاع
    StmtPtr body;                   ///< Operator body / جسم العامل
    AccessModifier access;          ///< Access modifier / معدّل الوصول
    
    /**
     * @brief Constructor / البناء
     */
    OperatorDecl(const std::string& op, std::vector<Parameter> params,
                 Data::DataType retType, StmtPtr body,
                 AccessModifier access = AccessModifier::PUBLIC,
                 const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), operatorSymbol(op), parameters(std::move(params)),
          returnType(retType), body(std::move(body)), access(access) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitOperatorDecl(*this);
    }
    
    std::string toString() const override;
};

} // namespace AST
} // namespace Sad

// =========================================================================
// (AR) عقد نظام النحلة (BeeOS) — إضافات المرحلة 9
// (EN) BeeOS AST Nodes — Phase 9 additions
// =========================================================================

namespace Sad {
namespace AST {

// =========================================================================
// Struct Declaration / تصريح بنية
// =========================================================================

/**
 * @brief Struct declaration / تصريح بنية
 * 
 * Represents a plain data structure (no inheritance, no vtable).
 * يمثل بنية بيانات بسيطة (بدون وراثة، بدون جدول دوال افتراضية).
 * 
 * @example Examples / أمثلة:
 * - بنية نقطة { عشري س، عشري ي }
 * - بنية<ن> صندوق { ن قيمة }
 * - struct Point { x: f64, y: f64 }
 */
struct StructField {
    std::string name;           ///< Field name / اسم الحقل
    Data::DataType type;        ///< Field type / نوع الحقل
    ExprPtr defaultValue;       ///< Default value (optional) / القيمة الافتراضية
    bool isPublic;              ///< Is public? / عام؟
    
    StructField(const std::string& n, Data::DataType t, 
                ExprPtr def = nullptr, bool pub = true)
        : name(n), type(t), defaultValue(std::move(def)), isPublic(pub) {}
    
    StructField(StructField&&) = default;
    StructField& operator=(StructField&&) = default;
    StructField(const StructField& other)
        : name(other.name), type(other.type), defaultValue(nullptr), isPublic(other.isPublic) {}
};

class StructDecl : public Statement {
public:
    std::string name;                           ///< Struct name / اسم البنية
    std::vector<StructField> fields;            ///< Struct fields / حقول البنية
    std::vector<TypeParameter> typeParameters;  ///< Generic params / معاملات الأنواع
    bool isPacked;                              ///< Packed? / محزومة؟
    bool isExported;                            ///< Exported? / مصدّرة؟
    StmtList methods;                           ///< Impl methods / الدوال المنفذة
    
    StructDecl(const std::string& name,
               std::vector<StructField> fields,
               std::vector<TypeParameter> typeParams = {},
               bool packed = false,
               bool exported = false,
               const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), name(name), fields(std::move(fields)),
          typeParameters(std::move(typeParams)),
          isPacked(packed), isExported(exported) {}
    
    void accept(ASTVisitor& visitor) override {
        // (AR) البنى تُعامل كأصناف مبسطة حتى يتم إضافة visitStructDecl
        // (EN) Structs treated as simplified classes until visitStructDecl is added
    }
    
    std::string toString() const override {
        return "struct " + name;
    }
};

// =========================================================================
// Trait Declaration / تصريح سمة
// =========================================================================

/**
 * @brief Trait declaration / تصريح سمة
 * 
 * Represents a trait (interface with optional default implementations).
 * يمثل سمة (واجهة مع تنفيذات افتراضية اختيارية).
 * 
 * @example Examples / أمثلة:
 * - سمة قابل_للعرض { دالة اعرض(هذا) نص }
 * - سمة<ن> مقارن { دالة قارن(هذا، آخر: ن) رقم }
 * - trait Display { fn display(&self) -> String }
 */
struct TraitMethod {
    std::string name;               ///< Method name / اسم الدالة
    std::vector<Parameter> params;  ///< Parameters / المعاملات
    Data::DataType returnType;      ///< Return type / نوع الإرجاع
    StmtPtr defaultImpl;            ///< Default implementation (optional) / التنفيذ الافتراضي
    
    TraitMethod(const std::string& n, std::vector<Parameter> p,
                Data::DataType ret, StmtPtr impl = nullptr)
        : name(n), params(std::move(p)), returnType(ret),
          defaultImpl(std::move(impl)) {}
    
    TraitMethod(TraitMethod&&) = default;
    TraitMethod& operator=(TraitMethod&&) = default;
};

class TraitDecl : public Statement {
public:
    std::string name;                           ///< Trait name / اسم السمة
    std::vector<TraitMethod> methods;           ///< Trait methods / دوال السمة
    std::vector<TypeParameter> typeParameters;  ///< Generic params / معاملات الأنواع
    std::vector<std::string> superTraits;       ///< Required traits / السمات المطلوبة
    bool isExported;                            ///< Exported? / مصدّرة؟
    
    TraitDecl(const std::string& name,
              std::vector<TraitMethod> methods,
              std::vector<TypeParameter> typeParams = {},
              std::vector<std::string> supers = {},
              bool exported = false,
              const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), name(name), methods(std::move(methods)),
          typeParameters(std::move(typeParams)),
          superTraits(std::move(supers)), isExported(exported) {}
    
    void accept(ASTVisitor& visitor) override {
        // (AR) السمات تُعامل مؤقتاً كأصناف حتى يتم إضافة visitTraitDecl
    }
    
    std::string toString() const override {
        return "trait " + name;
    }
};

// =========================================================================
// Impl Declaration / تصريح تنفيذ
// =========================================================================

/**
 * @brief Impl block declaration / تصريح كتلة التنفيذ
 * 
 * Implements a trait for a type, or adds methods to a struct.
 * ينفّذ سمة لنوع، أو يضيف دوال لبنية.
 * 
 * @example Examples / أمثلة:
 * - نفّذ قابل_للعرض لـ نقطة { دالة اعرض(هذا) نص { ... } }
 * - نفّذ نقطة { دالة المسافة(هذا) عشري { ... } }
 * - impl Display for Point { fn display(&self) -> String { ... } }
 */
class ImplDecl : public Statement {
public:
    std::string traitName;          ///< Trait name (empty if inherent impl) / اسم السمة
    std::string targetType;         ///< Target type name / اسم النوع المستهدف
    StmtList methods;               ///< Implemented methods / الدوال المنفذة
    std::vector<TypeParameter> typeParameters;  ///< Generic params / معاملات الأنواع
    
    ImplDecl(const std::string& trait, const std::string& target,
             StmtList methods,
             std::vector<TypeParameter> typeParams = {},
             const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), traitName(trait), targetType(target),
          methods(std::move(methods)), typeParameters(std::move(typeParams)) {}
    
    void accept(ASTVisitor& visitor) override {
        // (AR) كتل التنفيذ تُعامل مؤقتاً حتى يتم إضافة visitImplDecl
    }
    
    std::string toString() const override {
        if (traitName.empty())
            return "impl " + targetType;
        return "impl " + traitName + " for " + targetType;
    }
};

// =========================================================================
// Test Declaration / تصريح اختبار
// =========================================================================

/**
 * @brief Test function declaration / تصريح دالة اختبار
 * 
 * Represents a test case that can be run by the test framework.
 * يمثل حالة اختبار يمكن تشغيلها بإطار الاختبار.
 * 
 * @example Examples / أمثلة:
 * - اختبر("الجمع يعمل") { تأكد(1 + 1 == 2) }
 * - test("addition works") { assert(1 + 1 == 2) }
 */
class TestDecl : public Statement {
public:
    std::string testName;       ///< Test name / اسم الاختبار
    StmtPtr body;               ///< Test body / جسم الاختبار
    bool shouldFail;            ///< Expected to fail? / متوقع أن يفشل؟
    
    TestDecl(const std::string& name, StmtPtr body,
             bool shouldFail = false,
             const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), testName(name), body(std::move(body)),
          shouldFail(shouldFail) {}
    
    void accept(ASTVisitor& visitor) override {
        // (AR) الاختبارات تُعامل مؤقتاً حتى يتم إضافة visitTestDecl
    }
    
    std::string toString() const override {
        return "test \"" + testName + "\"";
    }
};

// =========================================================================
// Atomic Declaration / تصريح ذرّي
// =========================================================================

/**
 * @brief Atomic variable declaration / تصريح متغير ذرّي
 * 
 * @example Examples / أمثلة:
 * - ذرّي<رقم> عداد = 0
 * - atomic<int> counter = 0
 */
class AtomicDecl : public Statement {
public:
    std::string name;           ///< Variable name / اسم المتغير
    Data::DataType innerType;   ///< Inner type / النوع الداخلي
    ExprPtr initialValue;       ///< Initial value / القيمة الأولية
    
    AtomicDecl(const std::string& name, Data::DataType type,
               ExprPtr init = nullptr,
               const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), name(name), innerType(type),
          initialValue(std::move(init)) {}
    
    void accept(ASTVisitor& visitor) override {
        // (AR) الذريات تُعامل مؤقتاً حتى يتم إضافة visitAtomicDecl
    }
    
    std::string toString() const override {
        return "atomic " + name;
    }
};

} // namespace AST
} // namespace Sad

#endif // SAD_AST_DECLARATIONS_H
