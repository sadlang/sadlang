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
    ExprList decorators;            ///< Decorators (@decorator) / المُزخرِفات
    
    /**
     * @brief Constructor without decorators / البناء بدون مُزخرِفات
     * @param name Function name / اسم الدالة
     * @param params Parameter list / قائمة المعاملات
     * @param retType Return type / نوع الإرجاع
     * @param body Function body / جسم الدالة
     * @param exported Is exported / مصدّر
     * @param pos Source position / الموقع في الكود
     */
    FunctionDecl(const std::string& name, std::vector<Parameter> params,
                 Data::DataType retType, StmtPtr body, bool exported = false,
                 const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), name(name), parameters(std::move(params)),
          returnType(retType), body(std::move(body)), isExported(exported), 
          decorators() {}
    
    /**
     * @brief Constructor with decorators / البناء مع مُزخرِفات
     * @param name Function name / اسم الدالة
     * @param params Parameter list / قائمة المعاملات
     * @param retType Return type / نوع الإرجاع
     * @param body Function body / جسم الدالة
     * @param decs Decorator list / قائمة المُزخرِفات
     * @param exported Is exported / مصدّر
     * @param pos Source position / الموقع في الكود
     */
    FunctionDecl(const std::string& name, std::vector<Parameter> params,
                 Data::DataType retType, StmtPtr body, ExprList decs,
                 bool exported = false,
                 const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), name(name), parameters(std::move(params)),
          returnType(retType), body(std::move(body)), isExported(exported),
          decorators(std::move(decs)) {}
    
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
 * Represents a class declaration with fields and methods.
 * يمثل تصريح صنف مع حقول وطرق.
 * 
 * @example Examples / أمثلة:
 * - class Person { ... }
 * - صنف شخص { ... }
 * - class Student extends Person { ... }
 */
class ClassDecl : public Statement {
public:
    std::string name;               ///< Class name / اسم الصنف
    std::string superclass;         ///< Superclass name (optional) / اسم الصنف الأب
    StmtList members;               ///< Class members / أعضاء الصنف
    bool isExported;                ///< Is exported? / مصدّر؟
    
    /**
     * @brief Constructor / البناء
     * @param name Class name / اسم الصنف
     * @param superclass Superclass name / اسم الصنف الأب
     * @param members Member list / قائمة الأعضاء
     * @param exported Is exported / مصدّر
     * @param pos Source position / الموقع في الكود
     */
    ClassDecl(const std::string& name, const std::string& superclass,
              StmtList members, bool exported = false,
              const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), name(name), superclass(superclass),
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
// Import Statement / جملة الاستيراد
// =========================================================================

/**
 * @brief Import statement node / عقدة جملة الاستيراد
 * 
 * Represents a module import statement.
 * يمثل جملة استيراد وحدة.
 * 
 * @example Examples / أمثلة:
 * - import "math"
 * - import "graphics" as gfx
 * - استورد "رياضيات"
 * - from "math" import sin, cos
 */
class ImportStmt : public Statement {
public:
    std::string modulePath;         ///< Module path / مسار الوحدة
    std::string alias;              ///< Module alias (optional) / الاسم البديل
    std::vector<std::string> symbols; ///< Specific symbols to import / رموز محددة
    bool importAll;                 ///< Import all symbols / استيراد الكل
    
    /**
     * @brief Constructor / البناء
     */
    ImportStmt(const std::string& path, const std::string& alias = "",
               std::vector<std::string> symbols = {},
               bool importAll = false,
               const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), modulePath(path), alias(alias),
          symbols(std::move(symbols)), importAll(importAll) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitImportStmt(*this);
    }
    
    std::string toString() const override;
};

// =========================================================================
// Export Statement / جملة التصدير
// =========================================================================

/**
 * @brief Export statement node / عقدة جملة التصدير
 * 
 * Represents an export statement for modules.
 * يمثل جملة تصدير للوحدات.
 * 
 * @example Examples / أمثلة:
 * - export func calculate() { ... }
 * - export var PI = 3.14
 * - صدّر دالة احسب() { ... }
 */
class ExportStmt : public Statement {
public:
    StmtPtr declaration;            ///< Declaration to export / التصريح المصدَّر
    
    /**
     * @brief Constructor / البناء
     */
    ExportStmt(StmtPtr decl, const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), declaration(std::move(decl)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitExportStmt(*this);
    }
    
    std::string toString() const override {
        return "export " + declaration->toString();
    }
};

} // namespace AST
} // namespace Sad

#endif // SAD_AST_DECLARATIONS_H
